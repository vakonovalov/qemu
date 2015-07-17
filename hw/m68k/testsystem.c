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
#include "hw/sysbus.h"
#include "sysemu/sysemu.h"

#define RTC_VL      0x00    /* value */
#define RTC_MV      0x04    /* max value */
#define RTC_ID      0x08    /* is_dec */
#define RTC_ST      0x0c    /* step */
#define DPRINTF(fmt, ...) do {} while(0)

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    qemu_irq irq;
    QEMUTimer *timer;
    /* base address */
    target_ulong base;
    uint32_t val;
    uint32_t max_val;
    uint32_t is_dec;
    uint32_t step;
} ad_state;

static void ad_interrupt(void * opaque)
{
    ad_state *s = (ad_state *)opaque;
    if (s->is_dec == 0)
    {
	if (s->max_val < s->val) printf("Error val_max\n"); else
	s->val = s->val + s->step;
    } else
    if (s->max_val > s->val) printf("Error val_max\n"); else
	s->val = s->val - s->step;
    uint32_t ticks = 1;
    int64_t now = qemu_clock_get_ns(rtc_clock);
    timer_mod(s->timer, now + (int64_t)ticks * get_ticks_per_sec());
    printf("Alarm raised\n");
}

static void ad_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    ad_state *s = (ad_state *)opaque;
    if (offset > 0xF) {
        hw_error("Bad AD write offset 0x%x", (int)offset);
    }
    switch(offset) {
    case RTC_VL:
    	s->val = value;
	break;
    case RTC_MV:
  	s->max_val = value;
	break;
    case RTC_ID:
  	s->is_dec = value;
	break;
    case RTC_ST:
  	s->step = value;
	break;
    }
}

static uint32_t ad_readb(void *opaque, hwaddr offset)
{
    ad_state *s = (ad_state *)opaque;
    if (offset > 0xF) {
        hw_error("Bad AD read offset 0x%x", (int)offset);
    }
    uint32_t value = 0;
    switch(offset) {
    case RTC_VL:
    	value = s->val;
	break;
    case RTC_MV:
  	value = s->max_val;
	break;
    case RTC_ID:
  	value = s->is_dec;
	break;
    case RTC_ST:
  	value = s->step;
	break;
    }
    return value;
}

static void ad_writew(void *opaque, hwaddr offset,
                              uint32_t value)
{
	ad_writeb(opaque, offset, value);
}

static void ad_writel(void *opaque, hwaddr offset,
                              uint32_t value)
{
	ad_writeb(opaque, offset, value);
}

static uint32_t ad_readw(void *opaque, hwaddr offset)
{
	return ad_readb(opaque, offset);
}

static uint32_t ad_readl(void *opaque, hwaddr offset)
{
	return ad_readb(opaque, offset);
}

static const MemoryRegionOps ad_ops = {
    .old_mmio = {
        .read = {
            ad_readb,
	    ad_readw,
	    ad_readl,
        },
        .write = {
            ad_writeb,
	    ad_writew,
	    ad_writel,
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

    s->timer = timer_new_ns(rtc_clock, ad_interrupt, s);
    uint32_t ticks = 1;
    int64_t now = qemu_clock_get_ns(rtc_clock);
    timer_mod(s->timer, now + (int64_t)ticks * get_ticks_per_sec());
    s->cpu = cpu;
}
