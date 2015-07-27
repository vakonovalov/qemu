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

#define V_OVERLAY_MASK (1 << 4)
#define MY_MASK (1 << 2)

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    MemoryRegion rom;
    MemoryRegion ram;
    /* base address */
    target_ulong base;
    /* registers */
    uint8_t regs[VIA_REGS];
    uint8_t RWcount;
    uint8_t cmd;
} via_state;

static void via_set_regA(via_state *s, uint8_t val)
{
    uint8_t old = s->regs[vBufA];

    /* Switch vOverlay bit */
    if ((old & V_OVERLAY_MASK) != (val & V_OVERLAY_MASK)) {
        if (val & V_OVERLAY_MASK) {
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

static void cmdHandler(via_state *s)
{
   switch (s->cmd) {
    case 0x81:
        printf("1OOOOOO1 | Seconds register 0 \n");
        break;
    case 0x01:
        printf("0OOOOOO1 | Seconds register 0 \n");
        break;
    case 0x85:
        printf("1OOOO1O1 | Seconds register 1 \n");
        break;
    case 0x05:
        printf("0OOOO1O1 | Seconds register 1 \n");
        break;
    case 0x89:
        printf("1OOO10O1 | Seconds register 2 \n");
        break;
    case 0x09:
        printf("0OOO10O1 | Seconds register 2 \n");
        break;
    case 0x8D:
        printf("1OOO11O1 | Seconds register 3 \n");
        break;
    case 0x0D:
        printf("0OOO11O1 | Seconds register 3 \n");
        break;
    default:
        printf("Error \n");
        break;
    }
}

static void via_set_regB(via_state *s, uint8_t val)
{
    s->regs[vBufB] = val;
    if (val & 0x4) {
        if(s->RWcount != 0)
        {
            if (val & 0x2) {
                s->cmd = s->cmd << 1;
                s->cmd = s->cmd | (val & 0x1);
                s->RWcount--;
            }
        }
        else
        {
            printf("cmd: %i\n", s->cmd);
            cmdHandler(s);
            s->cmd = 0;
            s->RWcount = 8;
        }
    }
}

static void via_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    via_state *s = (via_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset >= VIA_REGS) {
        hw_error("Bad VIA write offset 0x%x", (int)offset);
    }
    printf("via_write offset=0x%x value=0x%x\n", (int)offset, value);
    printf("bit2=0x%i bit1=0x%i bit0=0x%i\n", value & 0x04, value & 0x02, value & 0x01);   
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
    s->RWcount = 8;
}

void sy6522_init(MemoryRegion *rom, MemoryRegion *ram,
                 uint32_t base, M68kCPU *cpu)
{
    via_state *s;

    s = (via_state *)g_malloc0(sizeof(via_state));

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

    qemu_register_reset(sy6522_reset, s);

    sy6522_reset(s);
}
