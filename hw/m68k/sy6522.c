/*
 * Synertek SY6522 Versatile Interface Adapter (VIA)
 * IO mapping corresponds to Macintosh 128k
 * TODO: split VIA structure and IO between two different files
 *
 * Copyright (c) 2015 Pavel Dovgalyuk
 *
 * This code is licensed under the GPL
 */
#include "hw/hw.h"
#include "qemu/timer.h"
#include "exec/address-spaces.h"
#include "mac128k.h"
#include "sysemu/sysemu.h"
#include "hw/irq.h"
#include "sy6522.h"
#include "mac_rtc.h"
#include "mac_keyboard.h"

#define REGA_OVERLAY_MASK (1 << 4)

typedef struct vbi_state {
    qemu_irq irq;
    QEMUTimer *timer;
} vbi_state;

typedef struct via_state {
    M68kCPU *cpu;
    MemoryRegion iomem;
    MemoryRegion rom;
    MemoryRegion ram;
    /* base address */
    target_ulong base;
    /* registers */
    uint8_t regs[VIA_REGS];
    rtc_state *rtc;
    vbi_state *vbi;
    keyboard_state *keyboard;
} via_state;

const char *via_regs[VIA_REGS] = {"vBufB", "---", "vDirB", "vDirA", "vT1C", 
                                  "vT1CH", "vT1L", "vT1LH", "vT2C",
                                  "vT2CH", "vSR", "vACR", "vPCR",
                                  "vIFR", "vIER", "vBufA"};

static void via_set_reg_vBufA(via_state *s, uint8_t val)
{
    uint8_t old = s->regs[vBufA];

    /* Switch vOverlay bit */
    if ((old & REGA_OVERLAY_MASK) != (val & REGA_OVERLAY_MASK)) {
        if (val & REGA_OVERLAY_MASK) {
            /* map ROM and RAM */
            memory_region_add_subregion_overlap(get_system_memory(),
                                                0x0, &s->rom, 1);
            memory_region_add_subregion_overlap(get_system_memory(),
                                                0x600000, &s->ram, 1);
            qemu_log("via: Map ROM at 0x0\n");
        } else {
            /* unmap ROM and RAM */
            memory_region_del_subregion(get_system_memory(), &s->rom);
            memory_region_del_subregion(get_system_memory(), &s->ram);
            qemu_log("via: Unmap ROM from 0x0\n");
        }
        tlb_flush(CPU(s->cpu), 1);
    }

    if ((old & REGA_SEL_MASK) != (val & REGA_SEL_MASK)) {
        qemu_log("via: SEL bit set to %x\n", !!(val & REGA_SEL_MASK));
    }

    /* TODO: other bits */

    s->regs[vBufA] = val;
    qemu_log("via: vBufA set to 0x%x\n", s->regs[vBufA]);
}

static void via_set_reg_vDirB(via_state *s, uint8_t val)
{
    s->regs[vDirB] = val;
    qemu_log("via: vDirB set to 0x%x\n", s->regs[vDirB]);
}

static void via_set_reg_vBufB(via_state *s, uint8_t val)
{
    uint8_t old = s->regs[vBufB];
    if (!(val & REGB_RTCENB_MASK)) {
        if (!(old & REGB_RTCCLK_MASK) && (val & REGB_RTCCLK_MASK)
            && (s->regs[vDirB] & REGB_RTCDATA_MASK)) {
            rtc_receive(s->rtc, val);
        } else if ((old & REGB_RTCCLK_MASK) && !(val & REGB_RTCCLK_MASK)
                   && !(s->regs[vDirB] & REGB_RTCDATA_MASK)) {
            val = rtc_send(s->rtc, val);
        }
    } else if ((val & REGB_RTCENB_MASK) && !(s->regs[vBufB] & REGB_RTCENB_MASK)) {
        rtc_param_reset(s->rtc);
    }

    /* TODO: other bits */

    s->regs[vBufB] = val;
    qemu_log("via: vBufB set to 0x%x\n", s->regs[vBufB]);
}

static void via_set_reg_vIFR(via_state *s, uint8_t val)
{
    qemu_log("1via: vIFR set to 0x%x\n", s->regs[vIFR]);
    if (val & 0x80) {
        s->regs[vIFR] |= val;
    } else {
        s->regs[vIFR] &= ~val;
    }
    s->regs[vIFR] = (!!(s->regs[vIER] & s->regs[vIFR] & 0x7f) << 7) | (s->regs[vIFR] & 0x7f);

    if (s->regs[vIFR] & 0x80) {
        m68k_set_irq_level(s->cpu, 1, 0x64 >> 2);
    } else {
        m68k_set_irq_level(s->cpu, 0, 0x64 >> 2);
    }

    qemu_log("via: vIFR set to 0x%x\n", s->regs[vIFR]);
}

static void via_set_reg_vIER(via_state *s, uint8_t val)
{
    if (val & 0x80) {
        s->regs[vIER] |= val;
    } else {
        s->regs[vIER] &= ~val;
    }

    s->regs[vIER] |= 0x80;

    qemu_log("via: vIER set to 0x%x\n", s->regs[vIER]);
}

static void via_set_reg_vSR(via_state *s, uint8_t val)
{
    s->regs[vSR] = val;
    qemu_log("via: vSR set to 0x%x\n", s->regs[vSR]);
    via_set_reg_vIFR(s, 0x04);
}

static void via_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    via_state *s = (via_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset >= VIA_REGS) {
        hw_error("Bad VIA write offset 0x%x", (int)offset);
    }
    qemu_log("via: write in %s 0x%x\n", via_regs[offset], value);
    via_set_reg(s, offset, value);
    if (offset == vSR) {
        keyboard_handle_cmd(s->keyboard);
    }
}

static uint32_t via_readb(void *opaque, hwaddr offset)
{
    via_state *s = (via_state *)opaque;
    uint32_t ret = 0;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset >= VIA_REGS) {
        hw_error("Bad VIA read offset 0x%x", (int)offset);
    }

    ret = via_get_reg(s, offset);
    qemu_log("via: read from %s 0x%x\n", via_regs[offset], ret);
    return ret;
}

uint8_t via_get_reg(via_state *s, uint8_t offset)
{
    if (offset == vSR) {
        via_set_reg_vIFR(s, s->regs[vIFR] & 0xfb);
    }

    return s->regs[offset];
}

void via_set_reg(via_state *via, uint8_t offset, uint8_t value) 
{
    switch (offset) {
    case vBufA:
        via_set_reg_vBufA(via, value);
        break;
    case vBufB:
        via_set_reg_vBufB(via, value);
        break;
    case vDirB:
        via_set_reg_vDirB(via, value);
        break;
    case vSR:
        via_set_reg_vSR(via, value);
        break;
    case vIFR:
        via_set_reg_vIFR(via, value);
        break;
    case vIER:
        via_set_reg_vIER(via, value);
        break;
    }
}

static const MemoryRegionOps via_ops = {
    .old_mmio = {
        .read = {
            via_readb,
            via_readb,
            via_readb,
        },
        .write = {
            via_writeb,
            via_writeb,
            via_writeb,
        },
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void via_set_irq(void *opaque, int irq, int level)
{
    via_state *s = (via_state *)opaque;
    via_set_reg_vIFR(s, (0x01 << irq) | 0x80);
}

static void sy6522_reset(void *opaque)
{
    via_state *s = opaque;
    /* Init registers */
    via_set_reg_vBufA(s, REGA_OVERLAY_MASK);
    via_set_reg_vBufB(s, 0);
    via_set_reg_vDirB(s, 0);
    via_set_reg_vIFR(s, 0);
    via_set_reg_vIER(s, 0);
}

static void vbi_interrupt(void * opaque)
{
    vbi_state *vbi = opaque;
    
    timer_mod_ns(vbi->timer, qemu_clock_get_ns(rtc_clock) + get_ticks_per_sec() / 60.0);
    qemu_irq_raise(vbi->irq);
}

static vbi_state *vbi_init(qemu_irq irq)
{
    vbi_state *s = (vbi_state *)g_malloc0(sizeof(vbi_state));
    
    s->irq = irq;
    s->timer = timer_new_ns(rtc_clock, vbi_interrupt, s);
    timer_mod_ns(vbi->timer, qemu_clock_get_ns(rtc_clock) + get_ticks_per_sec() / 60.0);
    return s;
}

via_state *sy6522_init(MemoryRegion *rom, MemoryRegion *ram,
                 uint32_t base, M68kCPU *cpu)
{
    via_state *s;
    qemu_irq *pic;

    s = (via_state *)g_malloc0(sizeof(via_state));
    pic = qemu_allocate_irqs(via_set_irq, s, 8);

    s->base = base;
    s->cpu = cpu;
    memory_region_init_io(&s->iomem, NULL, &via_ops, s,
                          "sy6522 via", 0x2000);
    memory_region_add_subregion(get_system_memory(),
                                base & TARGET_PAGE_MASK, &s->iomem);
    /* TODO: Magic! */
    memory_region_init_alias(&s->rom, NULL, "ROM overlay", rom, 0x0, 0x10000);
    memory_region_set_readonly(&s->rom, true);
    memory_region_init_alias(&s->ram, NULL, "RAM overlay", ram, 0x0, 0x20000);

    s->rtc = rtc_init(pic[0]);
    s->vbi = vbi_init(pic[1]);
    s->keyboard = keyboard_init(s, pic[2]);

    qemu_register_reset(sy6522_reset, s);
    sy6522_reset(s);
    return s;
}