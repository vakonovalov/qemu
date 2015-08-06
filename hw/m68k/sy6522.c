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

/* register offsets */
enum
{
    vBufB = 0,
    vDirB = 2,
    vDirA = 3,
    vT1C  = 4,
    vT1CH = 5,
    vT1L  = 6,
    vT1LH = 7,
    vT2C  = 8,
    vT2CH = 9,
    vSR   = 10,
    vACR  = 11,
    vPCR  = 12,
    vIFR  = 13,
    vIER  = 14,
    vBufA = 15,
    VIA_REGS = 16
};

#define REGA_OVERLAY_MASK (1 << 4)
#define REGB_RTCDATA_MASK (1 << 0)
#define REGB_RTCCLK_MASK (1 << 1)
#define REGB_RTCENB_MASK (1 << 2)
#define REGB_RTCRWBIT_MASK (1 << 7)
#define RTCSEC_MASK 0x0C
#define RTCRAMBUF1_MASK 0x3C
#define RTCRAMBUF2_MASK 0x0C

typedef struct {
    qemu_irq irq;
    uint8_t count;
    uint8_t rw_flag;
    uint8_t cmd;
    uint8_t param;
    uint8_t sec_reg[4]; 
    uint8_t test_reg;
    uint8_t wr_pr_reg;
    uint8_t buf_RAM[20];
    QEMUTimer *timer;
} rtc_state;

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    MemoryRegion rom;
    MemoryRegion ram;
    /* base address */
    target_ulong base;
    /* registers */
    uint8_t regs[VIA_REGS];
    rtc_state rtc;
} via_state;

static void via_set_regAbuf(via_state *s, uint8_t val)
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
            qemu_log("Map ROM at 0x0\n");
        } else {
            /* unmap ROM and RAM */
            memory_region_del_subregion(get_system_memory(), &s->rom);
            memory_region_del_subregion(get_system_memory(), &s->ram);
            qemu_log("Unmap ROM from 0x0\n");
        }
        tlb_flush(CPU(s->cpu), 1);
    }

    /* TODO: other bits */

    s->regs[vBufA] = val;
}

static void via_set_regBdir(via_state *s, uint8_t val)
{
    s->regs[vDirB] = val;
}

static void rtc_param_reset(rtc_state *rtc)
{
    rtc->param = 0;
    rtc->cmd = 0;
    rtc->rw_flag = 0;
    rtc->count = 0;
}

static void rtc_cmd_handler_w(rtc_state *rtc, uint8_t val)
{
    if (rtc->cmd == 0x35) {
        rtc->wr_pr_reg = rtc->param;
    }
    else if (!(rtc->wr_pr_reg & 0x80)) {
        if ((rtc->cmd & ~RTCSEC_MASK) == 0x01) {
            rtc->sec_reg[(rtc->cmd & RTCSEC_MASK) >> 2] = rtc->param;
            timer_mod_ns(rtc->timer, qemu_clock_get_ns(rtc_clock) + get_ticks_per_sec());
        } else if ((rtc->cmd & ~RTCRAMBUF1_MASK) == 0x41) {
            rtc->buf_RAM[(rtc->cmd & RTCRAMBUF1_MASK) >> 2] = rtc->param;
        } else if ((rtc->cmd & ~RTCRAMBUF2_MASK) == 0x21) {
             rtc->buf_RAM[16 + ((rtc->cmd & RTCRAMBUF2_MASK) >> 2)] = rtc->param;
        } else if (rtc->cmd == 0x31) {
             rtc->test_reg = rtc->param;
        } else {
            qemu_log("rtc error: unknown command\n");
        }
    } else {
        qemu_log("rtc error: write protect enabled\n");
    }
}

static void rtc_cmd_handler_r(rtc_state *rtc, uint8_t val)
{
    if ((rtc->cmd & ~RTCSEC_MASK & ~REGB_RTCRWBIT_MASK) == 0x01) {
        rtc->param = rtc->sec_reg[(rtc->cmd & RTCSEC_MASK) >> 2];
        qemu_irq_lower(rtc->irq);
    } else if ((rtc->cmd & ~RTCRAMBUF1_MASK & ~REGB_RTCRWBIT_MASK) == 0x41) {
        rtc->param = rtc->buf_RAM[(rtc->cmd & RTCRAMBUF1_MASK) >> 2];
    } else if ((rtc->cmd & ~RTCRAMBUF2_MASK & ~REGB_RTCRWBIT_MASK) == 0x21) {
        rtc->param = rtc->buf_RAM[16 + ((rtc->cmd & RTCRAMBUF2_MASK) >> 2)];
    } else {
        qemu_log("rtc error: unknown command\n");
    }
}

static void via_set_regBbuf(via_state *s, uint8_t val)
{
    uint8_t old = s->regs[vBufB];

    if (!(val & REGB_RTCENB_MASK)) {
        if (!(old & REGB_RTCCLK_MASK) && (val & REGB_RTCCLK_MASK)
            && (s->regs[vDirB] & REGB_RTCDATA_MASK)) {
            if (!(s->rtc.rw_flag)) {
                s->rtc.cmd |= (val & REGB_RTCDATA_MASK) << (7 - s->rtc.count);
            } else {
                s->rtc.param |= (val & REGB_RTCDATA_MASK) << (7 - s->rtc.count);	
            }
            s->rtc.count++;
            if (s->rtc.count == 8) {
                if (!(s->rtc.cmd & REGB_RTCRWBIT_MASK) && !(s->rtc.rw_flag)) {
                    s->rtc.rw_flag = 1;
                    s->rtc.count = 0;
                } else if (s->rtc.rw_flag) {
                    rtc_cmd_handler_w(&s->rtc, val);
                    rtc_param_reset(&s->rtc);
                } else {
                    rtc_cmd_handler_r(&s->rtc, val);
                    s->rtc.count = 0;
                }
            }
        } else if ((old & REGB_RTCCLK_MASK) && !(val & REGB_RTCCLK_MASK)
                   && !(s->regs[vDirB] & REGB_RTCDATA_MASK)) {
            val &= ~REGB_RTCDATA_MASK;
            val |= (s->rtc.param >> (7 - s->rtc.count)) & REGB_RTCDATA_MASK;
            s->rtc.count++;
            if (s->rtc.count == 8) {
                rtc_param_reset(&s->rtc);
            }
        }
    } else if ((val & REGB_RTCENB_MASK) && !(s->regs[vBufB] & REGB_RTCENB_MASK)) {
        rtc_param_reset(&s->rtc);
    }

    /* TODO: other bits */

    s->regs[vBufB] = val;
}

static void via_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    via_state *s = (via_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset >= VIA_REGS) {
        hw_error("Bad VIA write offset 0x%x", (int)offset);
    }
    qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
    switch (offset) {
    case vBufA:
        via_set_regAbuf(s, value);
        break;
    case vBufB:
        via_set_regBbuf(s, value);
        break;
    case vDirB:
        via_set_regBdir(s, value);
        break;
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
    ret = s->regs[offset];
    qemu_log("via_read offset=0x%x val=0x%x\n", (int)offset, ret);
    return ret;
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

static void rtc_interrupt(void * opaque)
{
    rtc_state *rtc = opaque;
    timer_mod_ns(rtc->timer, qemu_clock_get_ns(rtc_clock) + get_ticks_per_sec());
    if (rtc->sec_reg[0] == 0xFF) {
        if (rtc->sec_reg[1] == 0xFF) {
            if (rtc->sec_reg[2] == 0xFF) {
                rtc->sec_reg[3]++;
            }
            rtc->sec_reg[2]++;
        }
        rtc->sec_reg[1]++;
    }
    rtc->sec_reg[0]++;
    qemu_irq_raise(rtc->irq);
}

static void rtc_reset(rtc_state *rtc)
{
    uint64_t now = qemu_clock_get_ns(rtc_clock) / get_ticks_per_sec();
    uint8_t i;
    for (i = 0; i < 4; ++i) {
        rtc->sec_reg[i] = (now >> (8 * i)) & 0xFF;
    }
    rtc->wr_pr_reg = 0x80;
    timer_mod_ns(rtc->timer, now + get_ticks_per_sec());
}

static void rtc_init(void *opaque, qemu_irq irq)
{
    rtc_state *rtc = (rtc_state *)opaque;
    rtc->irq = irq;
    rtc->timer = timer_new_ms(rtc_clock, rtc_interrupt, rtc);
    rtc_reset(rtc);
}

static void set_rtc_irq(void *opaque, int irq, int level)
{
    via_state *s = (via_state *)opaque;
    if (irq == 0) {
        m68k_set_irq_level(s->cpu, level, 0x64 >> 2); 
    }
}

static void sy6522_reset(void *opaque)
{
    via_state *s = opaque;
    /* Init registers */
    via_set_regAbuf(s, REGA_OVERLAY_MASK);
    via_set_regBbuf(s, 0);
    via_set_regBdir(s, 0);
    rtc_param_reset(&s->rtc);
}

void sy6522_init(MemoryRegion *rom, MemoryRegion *ram,
                 uint32_t base, M68kCPU *cpu)
{
    via_state *s;
    qemu_irq *pic;

    s = (via_state *)g_malloc0(sizeof(via_state));
    pic = qemu_allocate_irqs(set_rtc_irq, s, 1);

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

    rtc_init(&s->rtc, pic[0]);

    qemu_register_reset(sy6522_reset, s);
    sy6522_reset(s);
}
