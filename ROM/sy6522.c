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
#define rTCClk  (1 << 1)
#define rTCData (1 << 0)

typedef struct {
    uint8_t seconds1;
    uint8_t seconds2;
    uint8_t seconds3;
    uint8_t test;
    uint8_t write_protect;
    uint8_t RAM[20];
    uint8_t ROM[20];
    uint8_t cmd;
} RTC_clock;

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    MemoryRegion rom;
    MemoryRegion ram;
    /* base address */
    target_ulong base;
    /* registers */
    uint8_t regs[VIA_REGS];
    RTC_clock *clk;
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

static void RTC_clock_tics(RTC_clock *s)
{
    printf("HERE\n");
}


static void set_regB(via_state *s, uint8_t val)
{
   uint8_t old = s->regs[vBufB];
   static uint8_t bits=0;
   if(!(old & rTCClk) && (val & rTCClk))
   {
        if(!s->clk->cmd) s->clk->cmd = val & 1;
        
        s->clk->cmd = val & 1;
        bits++;
        printf("reg = %d\n", s->clk->cmd);
        if(bits == 8)
            RTC_clock_tics(s->clk);
        tlb_flush(CPU(s->cpu), 1);
    }
    s->regs[vBufB] = val;
}

static void via_writeb(void *opaque, hwaddr offset, uint32_t value)
{
    via_state *s = (via_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset >= VIA_REGS) 
    {
        hw_error("Bad VIA write offset 0x%x", (int)offset);
    }
    qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
    //printf("via_read offset=0x%x val=0x%x\n", (int)offset, value);
    switch (offset) 
    {
        case vBufA:
            via_set_regA(s, value);
            break;
        case vBufB:
            set_regB(s, value);
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

static void sy6522_reset(void *opaque)
{
    via_state *s = opaque;
    /* Init registers */
    via_set_regA(s, V_OVERLAY_MASK);
}

void sy6522_init(MemoryRegion *rom, MemoryRegion *ram,  uint32_t base, M68kCPU *cpu)
{
    via_state *s;

    s = (via_state *)g_malloc0(sizeof(via_state));
    s->clk = (RTC_clock*)g_malloc0(sizeof(RTC_clock));
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
