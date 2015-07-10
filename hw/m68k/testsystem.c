/*
 * Synertek SY6522 Versatile Interface Adapter (AD)
 * IO mapping corresponds to Macintosh 128k
 * TODO: split AD structure and IO between two different files
 *
 * Copyright (c) 2015 Pavel Dovgalyuk
 *
 * This code is licensed under the GPL
 */
#include "hw/hw.h"
#include "qemu/timer.h"
#include "hw/m68k/mac128k.h"

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    /* base address */
    target_ulong base;
    char val;
} ad_state;

static void ad_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    ad_state *s = (ad_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset > 0xF) {
        hw_error("Bad AD write offset 0x%x", (int)offset);
    }
    s->val = value*10;
    printf("ad_write offset=0x%x value=%x\n", (int)offset, value);
}

static uint32_t ad_readb(void *opaque, hwaddr offset)
{
    ad_state *s = (ad_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset > 0xF) {
        hw_error("Bad AD read offset 0x%x", (int)offset);
    }
    printf("ad_read offset=%x\n", (int)s->val);
    return s->val;
}

static const MemoryRegionOps ad_ops = {
    .old_mmio = {
        .read = {
            ad_readb,
	    ad_readb,
	    ad_readb,
        },
        .write = {
            ad_writeb,
	    ad_writeb,
	    ad_writeb,
        },
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

void testsystem_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu)
{
    ad_state *s;

    s = (ad_state *)g_malloc0(sizeof(ad_state));

    s->base = base;
    memory_region_init_io(&s->iomem, NULL, &ad_ops, s,
                          "testsystem ad", 0x2000);
    memory_region_add_subregion(sysmem, base & TARGET_PAGE_MASK, &s->iomem);

    s->cpu = cpu;
}
