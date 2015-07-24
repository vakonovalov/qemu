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

#define VALUE 0x0 
#define MAX_VALUE 0x4
#define IS_DEC 0x8
#define STEP 0xc
#define INT_FLAG_CLEAR 0x10

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    /* base address */
    target_ulong base;

    QEMUTimer *timer;
    int64_t initial;	
    unsigned int value;
    unsigned int max_value;
    unsigned int is_dec;
    unsigned int step;

    qemu_irq irq;
    uint8_t int_flag;
} my_state;

static void device_interrupt_update(void *opaque);

static void clk_int_handler(void *opaque, int n, int level) {

	my_state *s = (my_state *)opaque;
/*    if (level) {
        s->ipr |= 1 << irq;
    } else {
        s->ipr &= ~(1 << irq);
    }
*/
	printf("Hello! I want to play a game... %d\n",s->value);
}

static void writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
	my_state *s = (my_state *)opaque;

	switch(offset) {
	case MAX_VALUE:
		s->max_value = (s->max_value & 0xffffff00) + (value & 0xff);
		break;
	case IS_DEC:
		s->is_dec = (s->is_dec & 0xffffff00) + (value & 0xff);
		break;
	case STEP:
		s->step = (s->step & 0xffffff00) + (value & 0xff);
		break;
	case INT_FLAG_CLEAR:
		s->int_flag = value;
		device_interrupt_update(s);
		break;
		
	default:
		break;
	}	

	return;

//	s->value = (s->value & 0xffffff00) + (value & 0xff);
//	printf("via_write offset=0x%x value=0x%x\n", (int)offset, value);
}

static uint32_t via_readb(void *opaque, hwaddr offset)
{
	my_state *s = (my_state *)opaque;    
//	printf("via_read offset=0x%x\n", (int)offset);
	return s->value & 0xff;
}

static void writew(void *opaque, hwaddr offset,
                              uint32_t value)
{
	my_state *s = (my_state *)opaque;

	switch(offset) {
	case MAX_VALUE:
		s->max_value = (s->max_value & 0xffff0000) + (value & 0xffff);
		break;
	case IS_DEC:
		s->is_dec = (s->is_dec & 0xffff0000) + (value & 0xffff);
		break;
	case STEP:
		s->step = (s->step & 0xffff0000) + (value & 0xffff);
		break;
	case INT_FLAG_CLEAR:
		s->int_flag = value;
		device_interrupt_update(s);
		break;
	default:
		break;
	}	

	return;

//	s->value = (s->value & 0xffff0000) + (value & 0xffff);
//	printf("via_write offset=0x%x value=0x%x\n", (int)offset, value);
}

static void writel(void *opaque, hwaddr offset,
                              uint32_t value)
{
	my_state *s = (my_state *)opaque;

	switch(offset) {
	case MAX_VALUE:
		s->max_value = value;
		break;
	case IS_DEC:
		s->is_dec = value;
		break;
	case STEP:
		s->step = value;
		break;
	case INT_FLAG_CLEAR:
		s->int_flag = value;
		device_interrupt_update(s);
		break;
	default:
		break;
	}	

	return;
//	printf("via_write offset=0x%x value=0x%x\n", (int)offset, value);
}

static uint32_t via_readw(void *opaque, hwaddr offset)
{	
	my_state *s = (my_state *)opaque;    
//	printf("via_read offset=0x%x\n", (int)offset);
	return s->value & 0xffff;
}

static uint32_t via_readl(void *opaque, hwaddr offset)
{
	my_state *s = (my_state *)opaque;    
//	printf("via_read offset=0x%x\n", (int)offset);
//	return qemu_clock_get_ns(0) - s->initial;
	return s->value;
}


static const MemoryRegionOps via_ops = {
    .old_mmio = {
        .read = {
            via_readb,
            via_readw,
            via_readl,
        },
        .write = {
            writeb,
            writew,
            writel,
        },
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void device_interrupt_update(void *opaque) {
	my_state *s = (my_state *)opaque;

	m68k_set_irq_level(s->cpu,s->int_flag,25);
	
	return;
}

static void device_callback(void * opaque) {
	my_state *s = (my_state *)opaque;
	if (s->is_dec) s->value -= s->step;
	else s->value += s->step;
	if (s->value >= s->max_value) {
		if (s->is_dec) s->value = s->max_value + s->value;
		else s->value = s->value % s->max_value;
	}
 
	s->initial = qemu_clock_get_ns(0);
        timer_mod_ns(s->timer, s->initial + get_ticks_per_sec());

	qemu_set_irq(s->irq, 0);
	s->int_flag = 1;
	device_interrupt_update(s);

	return;
}

void true_master_crazy_code_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu)
{
    my_state *s;

    s = (my_state *)g_malloc0(sizeof(my_state));

    s->base = base;
    memory_region_init_io(&s->iomem, NULL, &via_ops, s,
                          "true_master via", 0x2000);
    memory_region_add_subregion(sysmem, base & TARGET_PAGE_MASK, &s->iomem);

    s->cpu = cpu;

	s->timer = timer_new_ns(QEMU_CLOCK_REALTIME, device_callback, s);
	s->initial = qemu_clock_get_ns(0);
	s->value = 5;
	s->max_value = 10;
	s->is_dec = 1;
	s->step = 2;
        timer_mod_ns(s->timer, s->initial + get_ticks_per_sec());

	s->irq = qemu_allocate_irq(clk_int_handler, s, 25);
}
