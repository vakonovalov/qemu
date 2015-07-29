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

#define rTCData (1 << 0)
#define rTCClk (1 << 1)
#define rTCEnb (1 << 2)

#define V_OVERLAY_MASK (1 << 4)

typedef struct {
    QEMUTimer *timer;
    int64_t initial;	

    qemu_irq irq;

    uint8_t ram[20];
    uint8_t regs[6];
//	bool write_protect;
	uint8_t current_command;
	
} clk_state;

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    MemoryRegion rom;
    MemoryRegion ram;
    /* base address */
    target_ulong base;
    /* registers */
    uint8_t regs[VIA_REGS];

	clk_state *clk;
} via_state;

static unsigned char RTC_new_tact(clk_state *s,bool rTCData_bit,bool rTCEnb_bit) {
	static uint8_t aux_buf = 0;
	static uint8_t counter = 0;
	static bool command_received = 0;
	uint8_t *pointer;
	uint8_t temp,error_flag = 0;
	uint8_t ret = rTCData_bit;

	if (rTCEnb_bit) {
		s->current_command = 0;
		aux_buf = 0;
		counter = 0;
		command_received = 0;
		ret = rTCData_bit;
		return ret;
	}

	if (!command_received) {
		aux_buf = (aux_buf << 1) + rTCData_bit; 
		counter++;
//		printf("command %x", aux_buf);
		if (counter == 8) {				
			s->current_command = aux_buf;
			aux_buf = 0;
			counter = 0;
			command_received = 1;
		}		
		return ret;
	}

	temp = (s->current_command & 0x7c) >> 2;
	if (temp < 4) pointer = &s->regs[temp];
	else if (temp == 0xc) pointer = &s->regs[4];
	else if (temp == 0xd) pointer = &s->regs[5];
	else if (temp >=8 && temp <= 0xb) {
		temp = temp & 0x3;
		pointer = &s->ram[16+temp];
	} else if (temp >=0x10 && temp <= 0x1f) {
		temp = temp & 0xf;
		pointer = &s->ram[temp];
	} else error_flag = 1;

	if ((s->current_command & 0x03) != 1) error_flag = 1;

	if (error_flag) {
		printf("Invalid_command\n");
		s->current_command = 0;
		aux_buf = 0;
		counter = 0;
		command_received = 0;
		return ret;
	}	

	switch(s->current_command & 0x80) {
		case 0:
			aux_buf = (aux_buf << 1) + rTCData_bit; 
			counter++;
//			printf("command %x", s->aux_buf);
			break;	
		case 0x80:
			ret = (*pointer & (1 << (7-counter))) >> (7-counter);
			counter++;
			break;
	}

	if (counter == 8) {				
		s->current_command = 0;
		aux_buf = 0;
		counter = 0;
		command_received = 0;
//		*pointer = aux_buf;
	}

	return ret;
}

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

static void via_set_regB(via_state *s, uint8_t val)
{
	uint8_t old = s->regs[vBufB];

	if (!(old & rTCClk) && (val & rTCClk))
		val = val | RTC_new_tact(s->clk, val & rTCData, val & rTCEnb);

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
//    printf("via_write offset=0x%x value=0x%x\n", (int)offset, value);
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
//    printf("via_read offset=0x%x val=0x%x\n", (int)offset, ret);
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

static void timer_callback(void * opaque) {
//	uint32_t aux;
	clk_state *s = (clk_state *)opaque;
 
	s->initial = qemu_clock_get_ns(0);
	timer_mod_ns(s->timer, s->initial + get_ticks_per_sec());
/*	aux = (s->regs[0] << 24) + (s->regs[1] << 16) + (s->regs[2] << 8) + s->regs[3];
	aux++;
	s->regs[0] = (aux & 0xff000000) >> 24;
	s->regs[1] = (aux & 0x00ff0000) >> 16;
	s->regs[2] = (aux & 0x0000ff00) >> 8;
	s->regs[3] = aux & 0x000000ff;
*/
	(*(uint32_t *)s->regs)++;

	qemu_set_irq(s->irq, 0);

	return;
}

static void clk_int_handler(void *opaque, int n, int level) {
	via_state *s = (via_state *)opaque;

	printf("From RTC with love! :3 %d %d %d %d\n",s->clk->regs[0],s->clk->regs[1],s->clk->regs[2],s->clk->regs[3]);
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

	s->clk = (clk_state *)g_malloc0(sizeof(clk_state));
	s->clk->timer = timer_new_ns(QEMU_CLOCK_REALTIME, timer_callback, s->clk);
	s->clk->initial = qemu_clock_get_ns(0);
	s->clk->regs[0] = 250;
	s->clk->regs[1] = 251;
	s->clk->regs[2] = 252;
	s->clk->regs[3] = 253;

    sy6522_reset(s);

	s->clk->irq = qemu_allocate_irq(clk_int_handler, s, 25);
	timer_mod_ns(s->clk->timer, s->clk->initial + get_ticks_per_sec());
}
