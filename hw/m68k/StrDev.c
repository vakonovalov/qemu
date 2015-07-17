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
#include "hw/sysbus.h"
#include "sysemu/sysemu.h"

#define RTC_VL      0x00    /* Data register */
#define RTC_MX      0x04    /* Max Data register */
#define RTC_DC      0x08    /* Reverse register */

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    int value;
    int mxvalue;
    int is_dec;
    QEMUTimer *timer;
    /* base address */
    target_ulong base;
} via_state;


static void timer_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    via_state *s = (via_state *)opaque;
    if (offset > 0xF) {
        hw_error("Bad VIA write offset 0x%x", (int)offset);
        qemu_log("Bad VIA write offset 0x%x", (int)offset);
    }
    s->value = value;
    qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
}

static void timer_writew(void *opaque, hwaddr offset, uint32_t value)
{
    via_state *s = (via_state *)opaque;
    if (offset > 0xF) {
        hw_error("Bad VIA write offset 0x%x", (int)offset);
	 qemu_log("Bad VIA write offset 0x%x", (int)offset);
    }
    switch (offset) 
    {
        case RTC_VL:
	    s->value = value;
	    qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
            break;
        case RTC_MX:
            s->mxvalue = value;
 	     qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
            break; 
        case RTC_DC:
	     qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
            s->is_dec = value ;
            break;  
    }
}

static void timer_writel(void *opaque, hwaddr offset,  uint32_t value)
{
    via_state *s = (via_state *)opaque;
    if (offset > 0xF) {
        hw_error("Bad VIA write offset 0x%x", (int)offset);
	 qemu_log("Bad VIA write offset 0x%x", (int)offset);
    }
    switch (offset) 
    {
        case RTC_VL:
	    s->value = value;
	    qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
            break;
        case RTC_MX:
            s->mxvalue = value;
 	    qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
            break; 
        case RTC_DC:
	    qemu_log("via_write offset=0x%x value=0x%x\n", (int)offset, value);
            s->is_dec = value ;
            break;  
    }
}

static uint32_t timer_readb(void *opaque, hwaddr offset)
{
   via_state *s = (via_state *)opaque;
    if (offset > 0xF) {
        hw_error("Bad VIA read offset 0x%x", (int)offset);
    }
    switch (offset) 
    {
        case RTC_VL:
	   qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->value;
        case RTC_MX:
	    qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->mxvalue;
        case RTC_DC:
	   qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->is_dec;
    }
    return 0;
}


static uint32_t timer_readw(void *opaque, hwaddr offset)
{
    via_state *s = (via_state *)opaque;
    if (offset > 0xF) {
        hw_error("Bad VIA read offset 0x%x", (int)offset);
    }
    switch (offset) 
    {
        case RTC_VL:
	   qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->value;
        case RTC_MX:
	    qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->mxvalue;
        case RTC_DC:
	   qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->is_dec;
    }
    return 0;
}

static uint32_t timer_readl(void *opaque, hwaddr offset)
{
    via_state *s = (via_state *)opaque;
    if (offset > 0xF) 
    {
        hw_error("Bad VIA read offset 0x%x", (int)offset);
    }
    switch (offset) 
    {
        case RTC_VL:
	   qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->value;
        case RTC_MX:
	    qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->mxvalue;
        case RTC_DC:
	   qemu_log("via_read offset=0x%x\n", (int)offset);
	   qemu_log("via_read value=%d\n", s->value);
           return s->is_dec;
    }
    return 0;
}

static const MemoryRegionOps via_ops = {
    .old_mmio = {
        .read = {
            timer_readb,
            timer_readw,
            timer_readl,
	    
        },
        .write = {
            timer_writeb,
            timer_writew,
            timer_writel,
        },
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void timer_interrupt(void * opaque)
{
    via_state *s = (via_state *)opaque;
    s->is_dec = 1;
    s->value += 10;
    int64_t now = qemu_clock_get_ns(rtc_clock);
    timer_mod(s->timer, now + get_ticks_per_sec());
    qemu_log("Timer_mod value=%d\n", s->value);


}

void StrDev_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu)
{
    via_state *s;

    s = (via_state *)g_malloc0(sizeof(via_state));

    s->base = base;
    s->value = 0;
    memory_region_init_io(&s->iomem, NULL, &via_ops, s, "StrDev via", 0x2000);
    memory_region_add_subregion(sysmem, base & TARGET_PAGE_MASK, &s->iomem);
    s->timer = timer_new_ns(rtc_clock, timer_interrupt, s);
    //uint32_t ticks = 1;
    int64_t now = qemu_clock_get_ns(rtc_clock);
    timer_mod(s->timer, now + get_ticks_per_sec());
    s->cpu = cpu;
}
