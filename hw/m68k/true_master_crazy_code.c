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
#include "hw/m68k/mac128k.h"

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    /* base address */
    target_ulong base;
    unsigned char value;
} my_state;

static void via_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    my_state *s = (my_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset > 0xF) {
        hw_error("Bad VIA write offset 0x%x", (int)offset);
    }
    s->value = value;
    printf("via_write offset=0x%x value=0x%x\n", (int)offset, value);
}

static uint32_t via_readb(void *opaque, hwaddr offset)
{
    my_state *s = (my_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset > 0xF) {
        hw_error("Bad VIA read offset 0x%x", (int)offset);
    }
    
    printf("via_read offset=0x%x\n", (int)offset);
    return 10*s->value;
}
/*
static void via_writew(void *opaque, hwaddr offset,
                              uint32_t value)
{
	via_writeb(opaque, offset, value);
}

static void via_writel(void *opaque, hwaddr offset,
                              uint32_t value)
{
	via_writeb(opaque, offset, value);
}

static uint32_t via_readw(void *opaque, hwaddr offset)
{
	return via_readb(opaque, offset);
}

static uint32_t via_readl(void *opaque, hwaddr offset)
{
	return via_readb(opaque, offset);
}
*/

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

void true_master_crazy_code_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu)
{
    my_state *s;

    s = (my_state *)g_malloc0(sizeof(my_state));

    s->base = base;
    memory_region_init_io(&s->iomem, NULL, &via_ops, s,
                          "true_master via", 0x2000);
    memory_region_add_subregion(sysmem, base & TARGET_PAGE_MASK, &s->iomem);

    s->cpu = cpu;
}
