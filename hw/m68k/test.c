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
#define zero 0
typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    qemu_irq irq;
    /* base address */
    QEMUTimer *timer;
    target_ulong base;

    int value; 
    int max_value;
    int is_dec;
    int step;
} bia_state;

static void bia_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    bia_state *s = (bia_state *)opaque;
    //offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset > 0xF) {
        hw_error("Bad BIA write offset 0x%x", (int)offset);
    }
    printf("bia_write offset=0x%x value=0x%x\n", (int)offset, value);
    if (offset == 0x0)
    {
	s->value = value;
	m68k_set_irq_level(s->cpu, 1, 0x64 >> 2);
    }
    else if (offset == 0x4) s->max_value = value;
    else if (offset == 0x8) s->is_dec = value;
    else if (offset == 0xC) s->step = value;
}

static uint32_t bia_readb(void *opaque, hwaddr offset)
{
    bia_state *s = (bia_state *)opaque;
    //offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset > 0xF) {
        hw_error("Bad BIA read offset 0x%x", (int)offset);
    }
    printf("bia_read offset=0x%x, %i\n", (int)offset, s->value);
    m68k_set_irq_level(s->cpu, 0, 0x64 >> 2);
    return s->value;
}

static void bia_writew(void *opaque, hwaddr offset,
                              uint32_t value)
{
	bia_writeb(opaque, offset, value);
}

static void bia_writel(void *opaque, hwaddr offset,
                              uint32_t value)
{
	bia_writeb(opaque, offset, value);
}

static uint32_t bia_readw(void *opaque, hwaddr offset)
{
	return bia_readb(opaque, offset);
}

static uint32_t bia_readl(void *opaque, hwaddr offset)
{
	return bia_readb(opaque, offset);
}
static const MemoryRegionOps bia_ops = {
    .old_mmio = {
        .read = {
            bia_readb,
            bia_readw,
            bia_readl,
        },
        .write = {
            bia_writeb,
            bia_writew,
            bia_writel,
        },
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void mt_interrupt(void * opaque)
{
    bia_state *s = (bia_state *)opaque;
    timer_mod_ns(s->timer, qemu_clock_get_ns(QEMU_CLOCK_REALTIME) + 1 * get_ticks_per_sec());
    if (s->value != s->max_value)
    {
	if (s->is_dec == 1) s->value += s->step;
        else s->value -= s->step;
	m68k_set_irq_level(s->cpu, 1, 0x64 >> 2);
    }
}

void test_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu)
{
    bia_state *s;

    s = (bia_state *)g_malloc0(sizeof(bia_state));

    s->base = base;
    memory_region_init_io(&s->iomem, NULL, &bia_ops, s,
                          "test bia", 0x2000);
    memory_region_add_subregion(sysmem, base & TARGET_PAGE_MASK, &s->iomem);

    s->cpu = cpu;
    s->value = 0; 
    s->max_value = 10;
    s->is_dec = 1;
    s->step = 1;
    s->timer = timer_new_ms(QEMU_CLOCK_REALTIME, mt_interrupt, s);
    timer_mod_ns(s->timer, qemu_clock_get_ns(QEMU_CLOCK_REALTIME) + 1 * get_ticks_per_sec());
}
