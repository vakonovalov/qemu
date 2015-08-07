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
#define speed 1000
#define V_OVERLAY_MASK (1 << 4)

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    MemoryRegion rom;
    MemoryRegion ram;
    /* base address */
    target_ulong base;
    /* registers */
    uint8_t regs[VIA_REGS];
    uint8_t num;
    uint8_t cmd;
    uint8_t buf;
    bool flag; // 1-read, 0-write
    bool write_prot;  // 1-write prot
    uint8_t sec_reg_0;
    uint8_t sec_reg_1;
    uint8_t sec_reg_2;
    uint8_t sec_reg_3;
    uint8_t rTCbuff[20];
    QEMUTimer *timer;
} via_state;

static void via_interrupt(void * opaque)
{
    via_state *s = (via_state *)opaque;
    int64_t now = qemu_clock_get_ns(rtc_clock);
    timer_mod(s->timer, now + get_ticks_per_sec()/speed);
    if (s->sec_reg_0 == 0xFF) {
        if(s->sec_reg_1 == 0xFF) {
            if(s->sec_reg_2 == 0xFF) {
                if(s->sec_reg_3 == 0xFF) {
                    printf("Time out");
                }
                s->sec_reg_3 ++;
            } 
            s->sec_reg_2 ++;
        }
        s->sec_reg_1 ++;
    } 
    s->sec_reg_0 ++;
    m68k_set_irq_level(s->cpu, 1, 0x64 >> 2);
    //m68k_set_irq_level(s->cpu, 0, 0x64 >> 2);
}

static void via_set_regA(via_state *s, uint8_t val)
{
    uint8_t old = s->regs[vBufA];
    /* Switch vOverlay bit */
    if ((old & V_OVERLAY_MASK) != (val & V_OVERLAY_MASK)) {
        if (val & V_OVERLAY_MASK) {
            /* map ROM and RAM */
            memory_region_add_subregion_overlap(get_system_memory(), 0x0, &s->rom, 1);
            memory_region_add_subregion_overlap(get_system_memory(), 0x600000, &s->ram, 1);
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

static void handl_cmd(via_state *s)
{
    //printf("cmd = 0x%x, buf = 0x%x, flag = 0x%x\n", (s->cmd), (s->buf), (s->flag));
    if (s->buf) { // use command
        switch(s->buf & 0x7F) {
        case 0x01:
            printf("z0000101 set Seconds register 0\n");
            if (!s->write_prot)
                s->sec_reg_0 = s->cmd;
            break;
        case 0x05:
            printf("z0000101 set Seconds register 1\n");
            if (!s->write_prot)
                s->sec_reg_1 = s->cmd;
            break;
        case 0x09:
            printf("z0001001 set Seconds register 2\n");
            if (!s->write_prot)
                s->sec_reg_2 = s->cmd;
            break;
        case 0x0d:
            printf("z0001101 set Seconds register 3: old-0x%x ", s->sec_reg_3);
            if (!s->write_prot)
                s->sec_reg_3 = s->cmd;
            printf("new-0x%x\n", s->sec_reg_3);
            break;
        case 0x31:
            printf("Test register\n");
            break;
        case 0x35:
            if ((s->cmd) & 0x40)//00110101
                s->write_prot = 1;
            else s->write_prot = 0;
            printf("Write-protect register = %d\n", s->write_prot);
            break;
        default:
            if ((s->buf & 0x73) == 0x21 && !s->write_prot) { //z010aa01
                s->rTCbuff[16 + ((s->buf & 0x0C) >> 2)] = s->cmd;
                printf("z010aa01 rTCbuff[%d] = %d\n", 16 + ((s->buf & 0x0C) >> 2),s->rTCbuff[16 + ((s->buf & 0x0C) >> 2)]);
            } else if ((s->buf & 0x43) == 0x41 && !s->write_prot) { //z1aaaa01 
                s->rTCbuff[(s->buf & 0x3C) >> 2] = s->cmd;
                printf("z1aaaa01 rTCbuff[%d] = %d\n", (s->buf & 0x3C) >> 2, s->rTCbuff[(s->buf & 0x3C) >> 2]);
            } else printf("Unknown command\n");
            break;
        }
        s->buf = 0;
        s->cmd = 0;
    } else { // set flag
        if (s->cmd & 0x80) 
            s->flag = 1;
        else s->flag = 0;
        if (s->flag) // next write
        {
            switch(s->cmd & 0x7F) {
            case 0x01:
                s->cmd = s->sec_reg_0;
                m68k_set_irq_level(s->cpu, 0, 0x64 >> 2);
                break;
            case 0x05:
                s->cmd = s->sec_reg_1;
                m68k_set_irq_level(s->cpu, 0, 0x64 >> 2);
                break;
            case 0x09:
                s->cmd = s->sec_reg_2;
                m68k_set_irq_level(s->cpu, 0, 0x64 >> 2);
                break;
            case 0x0d:
                s->cmd = s->sec_reg_3;
                m68k_set_irq_level(s->cpu, 0, 0x64 >> 2);
                break;
            default:
                if ((s->cmd & 0x73) == 0x21) { //z010aa01
                    printf("write z010aa01\n");
                    s->cmd = s->rTCbuff[16 + ((s->cmd & 0x0C) >> 2)];
                } else if ((s->cmd & 0x43) == 0x41) { //z1aaaa01
                    printf("write z1aaaa01\n");
                    s->cmd = s->rTCbuff[(s->cmd & 0x3C) >> 2];
                } else printf("Unknown command\n");
                break;
            }
        } else { //next read, set cmd
            s->buf = s->cmd;
            s->cmd = 0;
        }
    }
}

static void via_set_regB(via_state *s, uint8_t val)
{
    //printf("cmd: %x, val: %x\n", s->cmd, val);
    if ((val & 0x4) == 4) {
        if((s->regs[vBufB] & 0x4) != (val & 0x4) && (s->num < 7)) {
            s->num = 0;
            s->cmd = 0;
            s->flag = 0;
        }
    } else {
        if (!s->flag) {
            if ((val & 0x2) == 2) {
                s->cmd = s->cmd | ((val & 1) << (s->num));
                if (s->num < 8) {
                    (s->num)++;
                    if (s->num == 8) {
                        s->num = 0;
                        handl_cmd(s);
                    }
                } else qemu_log("ERROR NUM");
            }
        } else {
            if (!(val & 0x2) && ((s->regs[vBufB] & 0x2) == 2)) {
                val &= 0xFE;
                val |= ((s->cmd) >> (s->num) & 0x1);
                if (s->num < 8) {
                    (s->num)++;
                    if (s->num == 8) {
                        //printf("send cmd: %x finished\n", s->cmd);
                        s->num = 0;
                        s->cmd = 0;
                        s->flag = 0;
                    }
                } else qemu_log("ERROR NUM");
            }
        }
    } 
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
    //printf("via_write offset=0x%x value= %d\n", (int)offset, value);
    switch (offset) {
    case vBufA:
        via_set_regA(s, value);
        break;
    case vBufB:
        via_set_regB(s, value);
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
    //printf("via_read offset=0x%x val=0x%x\n", (int)offset, ret);
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

static void sy6522_reset(void *opaque)
{
    via_state *s = opaque;
    /* Init registers */
    via_set_regA(s, V_OVERLAY_MASK);
    via_set_regB(s, 0);
}

void sy6522_init(MemoryRegion *rom, MemoryRegion *ram,
                 uint32_t base, M68kCPU *cpu)
{
    via_state *s;

    s = (via_state *)g_malloc0(sizeof(via_state));

    s->base = base;
    s->cpu = cpu;
    s->num = 0;
    s->cmd = 0;
    s->buf = 0;
    s->write_prot = 0;
    s->sec_reg_0 = 0xF0;
    s->sec_reg_1 = 0x1B;
    s->sec_reg_2 = 0x2C;
    s->sec_reg_3 = 0x01;
    s->flag = 0;
    s->regs[vBufB] =  s->regs[vBufB] & 0x4;
    memory_region_init_io(&s->iomem, NULL, &via_ops, s,
                          "sy6522 via", 0x2000);
    memory_region_add_subregion(get_system_memory(),
                                base & TARGET_PAGE_MASK, &s->iomem);
    /* TODO: Magic! */
    memory_region_init_alias(&s->rom, NULL, "ROM overlay", rom, 0x0, 0x10000);
    memory_region_set_readonly(&s->rom, true);
    memory_region_init_alias(&s->ram, NULL, "RAM overlay", ram, 0x0, 0x20000);

    qemu_register_reset(sy6522_reset, s);


    s->timer = timer_new_ns(rtc_clock, via_interrupt, s);
    int64_t now = qemu_clock_get_ns(rtc_clock);
    timer_mod(s->timer, now + get_ticks_per_sec() / speed);

    sy6522_reset(s);
}
