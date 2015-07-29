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
    uint8_t RWflag; // 1&1 - write; 0&1 - read; 1&2 - full cmd; 0&2 - no full cmd;
    uint8_t cmd;
    uint8_t param;
    uint8_t secReg0; 
    uint8_t secReg1; 
    uint8_t secReg2; 
    uint8_t secReg3; 
} via_state;

static void via_set_regAbuf(via_state *s, uint8_t val)
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

static void via_set_regBdir(via_state *s, uint8_t val)
{
    s->regs[vDirB] = val;
}

static void sender(via_state *s, uint8_t *val)
{
    uint8_t v;
    if (s->RWcount <= 7) {
        v = s->param;
        v = v >> (7 - s->RWcount);
        v = v & 1;
        *val = *val & 0xFE;
        *val = *val | v;
        s->RWcount++;
    }
    else {
        s->param = 0;
		s->cmd = 0;
		s->RWflag = 0;
    }
}

static void cmdHandlerW(via_state *s, uint8_t val)
{
    printf("cmd: %i %i\n", s->cmd, s->param);
   	switch (s->cmd) {
    case 0x81:
        printf("1OOOOOO1 | Seconds register 0 \n");
        s->secReg0 = s->param;
        break;
    case 0x85:
        printf("1OOOO1O1 | Seconds register 1 \n");
        s->secReg1 = s->param;
        break;
    case 0x89:
        printf("1OOO10O1 | Seconds register 2 \n");
        s->secReg2 = s->param;
        break;
    case 0x8D:
        printf("1OOO11O1 | Seconds register 3 \n");
        s->secReg3 = s->param;
        break;
    default:
        printf("Error \n");
        break;
    }
    s->param = 0;
	s->cmd = 0;
	s->RWflag = 0;
    s->RWcount = 8;
}

static void cmdHandlerR(via_state *s, uint8_t val)
{
   	switch (s->cmd) {
    case 0x01:
        printf("0OOOOOO1 | Seconds register 0 \n");
		s->param = s->secReg0;
        break;
    case 0x05:
        printf("0OOOO1O1 | Seconds register 1 \n");
		s->param = s->secReg1;
        break;
    case 0x09:
        printf("0OOO10O1 | Seconds register 2 \n");
		s->param = s->secReg2;
        break;
    case 0x0D:
        printf("0OOO11O1 | Seconds register 3 \n");
		s->param = s->secReg3;
        break;
    default:
        printf("Error \n");
        break;
    }
}

static void addBit(uint8_t *cmd, uint8_t val)
{
	*cmd = *cmd << 1;
	*cmd = *cmd | (val & 0x1);
}

static void via_set_regBbuf(via_state *s, uint8_t val)
{
    if ((val & 0x4) && (!(s->regs[vBufB] & 0x2)) && (val & 0x2)) {
        if ((s->RWflag & 0x2) == 0) {			
            if ((s->RWflag & 0x1) == 0)
				addBit(&s->cmd, val & 0x1);
			else if ((s->RWflag & 0x1) != 0)
				addBit(&s->param, val & 0x1);				
            s->RWcount--;
            if (s->RWcount == 0)
                s->RWflag = s->RWflag | 2;
        }
        if ((s->RWflag & 0x2) != 0) {
			if ((s->cmd & 128) && ((s->RWflag & 0x1) == 0)) {
				s->RWflag = s->RWflag | 1;
				s->RWflag = s->RWflag & 0xFD; 
                s->RWcount = 8;
			}
			else if ((s->RWflag & 0x1) != 0) {
                cmdHandlerW(s, val);
            }
			else if ((s->RWflag & 0x1) == 0) {
                if (s->RWcount == 0) 
                    cmdHandlerR(s, val);
                sender(s, &val);
            }
        }
    }
    else if (!(val & 0x4) && (s->regs[vBufB] & 0x4)) {
			s->regs[vBufB] = val;
		    s->param = 0;
			s->cmd = 0;
            s->RWcount = 8;
			s->RWflag = 0;
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
    //printf("via_write offset=0x%x value=0x%x\n", (int)offset, value); 
	//printf("bit2=0x%i bit1=0x%i bit0=0x%i\n", value & 0x04, value & 0x02, value & 0x01);  
    switch (offset) {
    case vBufA:
        via_set_regAbuf(s, value);
        break;
    case vBufB:
        via_set_regBbuf(s, value);
        break;
    case vDirB:
        via_set_regBdir(s, value);
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
    via_set_regAbuf(s, V_OVERLAY_MASK);
	via_set_regBdir(s, 0);
    s->cmd = 0;
    s->param = 0;
    s->RWcount = 8;
	s->RWflag = 0;
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
