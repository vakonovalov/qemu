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
#define rTCEnable (1 << 2)

typedef struct {
    uint8_t seconds0;
    uint8_t seconds1;
    uint8_t seconds2;
    uint8_t seconds3;
    uint8_t test;
    uint8_t write_protect;
    uint8_t RAM[20];
    uint8_t cmd;
    uint8_t data;
    int is_dec;
    int mxvalue;
    int step;
    bool    command;
    uint8_t bits;
    uint8_t count;
    bool lock;
    bool flag;
    uint8_t high_bit;
} RTC_clock;

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    MemoryRegion rom;
    MemoryRegion ram;
    QEMUTimer *timer;
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

static uint8_t RTC_clock_tics(RTC_clock *s, uint8_t val)
{
    
    if (val & rTCEnable) 
    {
         s->data = 0;
         s->command = 0;
         s->flag = 0;
         s->command = 0;
         s->bits = 0;
    }
    if (!s->command)
    {
        s->cmd = s->cmd << 1;
        s->cmd = s->cmd | (val & 1);
        s->bits++;
        if (s->bits == 8) 
        {
            s->bits = 0;
            s->flag = 1;
            s->command = 1;
            s->high_bit = s->cmd & 0x80;
            s->cmd = s->cmd & 0x7f;
            printf("RTC = %d\n", s->cmd);
            //printf("H_B = %d\n", s->high_bit);
        }    
    }
    
    if (s->flag)
    {
        switch (s->cmd) 
        {
            case 0x1:
                if (s->high_bit)
                    {    
                           
                       uint8_t data;
                       data = ((s->seconds0>>s->count) & 0x1) | (val & 0xfe);

                       s->count--;
                       return data;
                    }
                else
                    {   
                        if (!s->lock)
                        {
                            printf("Recording is not possible, the register is locked!\n");
                            return val;
                        }
                        if (!s->bits) 
                        {
                            s->bits++;
                            return val;
                        }
                        s->data = s->data << 1;
                        s->data = s->data | (val & 1);
                        s->bits++;
                        if (s->bits == 8)
                        { 
                            printf("DATA = %d\n", s->data);
                            s->seconds0 = s->data;
                            s->data = 0;
                            s->bits = 0;
                            s->command = 0;
                            s->flag = 0;
                        } 
                    } 
                    return val;          
            case 0x5:
                if (s->high_bit)
                    {    
                        if (s->bits == 8)
                        {
                            s->data = 0;
                            s->bits = 0;
                            s->command = 0;
                            s->flag = 0;
                            s->count = 7;
                            return val;
                        } 
                        uint8_t data;
                        data = ((s->seconds1>>s->count) & 0x1) | (val & 0xfe);
                        s->count--;
                        printf("send = %d\n", data & 0x1);
                        s->bits++;
                        return data;
                    }
                else
                    {   
                        if (!s->lock)
                        {
                            printf("Recording is not possible, the register is locked!\n");
                            return val;
                        } 
                        if (!s->bits) 
                        {
                            s->bits++;
                            return val;
                        }
                        s->data = s->data << 1;
                        s->data = s->data | (val & 1);
                        s->bits++;
                        if (s->bits > 8)
                        { 
                            printf("DATA = %d\n", s->data);
                            s->seconds1 = s->data;
                            s->data = 0;
                            s->bits = 0;
                            s->command = 0;
                            s->flag = 0;
                        } 
                    } 
                    return val;                    
            case 0x9:
                if (s->high_bit)
                    {    
                       uint8_t data;
                       data = ((s->seconds2>>s->count) & 0x1) | (val & 0xfe);
                       s->count--;
                       return data;
                    }
                else
                    {   
                        if (!s->lock)
                        {
                            printf("Recording is not possible, the register is locked!\n");
                            return val;
                        }
                        if (!s->bits) 
                        if (!s->bits) 
                        {
                            s->bits++;
                            return val;
                        }
                        s->data = s->data << 1;
                        s->data = s->data | (val & 1);
                        s->bits++;
                        if (s->bits > 8)
                        { 
                            printf("DATA = %d\n", s->data);
                            s->seconds2 = s->data;
                            s->data = 0;
                            s->bits = 0;
                            s->command = 0;
                            s->flag = 0;
                        } 
                    } 
                    return val;          
            case 0x13:
                if (s->high_bit)
                {    
                    uint8_t data;
                    data = ((s->seconds3>>s->count) & 0x1) | (val & 0xfe);
                    s->count--;
                    return data;
                }
                else
                {   
                    if (!s->lock)
                    {
                        printf("Recording is not possible, the register is locked!\n");
                        return val;
                    }
                    if (!s->bits) 
                    {
                        s->bits++;
                        return val;
                    }
                    s->data = s->data << 1;
                    s->data = s->data | (val & 1);
                    s->bits++;
                    if (s->bits > 8)
                    { 
                        printf("DATA = %d\n", s->data);
                        s->seconds3 = s->data;
                        s->data = 0;
                        s->bits = 0;
                        s->command = 0;
                        s->flag = 0;
                    } 
                } 
                return val;
            case 0x49:
                 if (s->high_bit)
                {    
                    printf("Error! Register (test) is only write!\n");
                    return val;
                }
                else
                {   
                    if (!s->lock)
                    {
                        printf("Recording is not possible, the register is locked!\n");
                        return val;
                    }
                    if (!s->bits) 
                    {
                        s->bits++;
                        return val;
                    }
                    s->data = s->data << 1;
                    s->data = s->data | (val & 1);
                    s->bits++;
                    if (s->bits > 8)
                    { 
                        printf("DATA = %d\n", s->data);
                        s->test = s->data;
                        s->data = 0;
                        s->bits = 0;
                        s->command = 0;
                        s->flag = 0;
                    } 
                }
                return val;
            case 0x53:
                if (s->high_bit)
                {    
                    printf("Error! Register (write_protect) is only write!\n");
                    return val;
                }
                else
                {   
                    if (!s->bits) 
                    {
                        s->bits++;
                        return val;
                    }
                    s->data = s->data << 1;
                    s->data = s->data | (val & 1);
                    s->bits++;
                    if (s->bits > 8)
                    { 
                        printf("DATA = %d\n", s->data);
                        s->write_protect = s->data;
                        if(s->write_protect & 0x80)
                            s->lock = 0;
                        s->data = 0;
                        s->bits = 0;
                        s->command = 0;
                        s->flag = 0;
                    } 
                }
                return val;
            case 0x54:
                return val;
            case 0x55:
                return val;
        }
    }
return val;       
}

static void set_regB(via_state *s, uint8_t val)
{
    uint8_t old = s->regs[vBufB];
    if (!(old & rTCClk) && (val & rTCClk))
    {  
       val = RTC_clock_tics(s->clk, val);
       //printf("VAL = %d\n", val & 1);
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


static void timer_interrupt(void * opaque)
{
    via_state *s = (via_state *)opaque;    
   
    if (s->clk->seconds1 >= s->clk->mxvalue)
    {
        s->clk->is_dec = 1;
    } 
    else if (s->clk->seconds1 <= 0)
    {
        s->clk->is_dec = 0;
    }

    if (s->clk->is_dec==1) 
         {
             s->clk->seconds1 = s->clk->seconds1 - s->clk->step;
             int64_t now = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);
             timer_mod(s->timer, now + get_ticks_per_sec());
             //qemu_log("Timer_mod value=%d\n", s->clk->value);
         }
    else if(s->clk->is_dec==0)
     {
             s->clk->seconds1 = s->clk->seconds1 + s->clk->step;  
             int64_t now = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);
             timer_mod(s->timer, now + get_ticks_per_sec());
            // qemu_log("Timer_mod value=%d\n", s->clk->value); 
     }  

   m68k_set_irq_level(s->cpu, 1, 25);
}

void sy6522_init(MemoryRegion *rom, MemoryRegion *ram,  uint32_t base, M68kCPU *cpu)
{
    via_state *s;

    s = (via_state *)g_malloc0(sizeof(via_state));
    s->clk = (RTC_clock*)g_malloc0(sizeof(RTC_clock));
    s->clk->command = 0;
    s->clk->bits = 0;
    s->clk->flag = 0;
    s->clk->count = 7;
    s->clk->lock = 1;
    s->clk->seconds1 = 4;
    s->base = base;
    s->cpu = cpu;
    s->clk->step = 1;
    s->clk->mxvalue = 10;
    s->clk->is_dec = 1;
    memory_region_init_io(&s->iomem, NULL, &via_ops, s, "sy6522 via", 0x2000);
    memory_region_add_subregion(get_system_memory(), base & TARGET_PAGE_MASK, &s->iomem);
    /* TODO: Magic! */
    memory_region_init_alias(&s->rom, NULL, "ROM overlay", rom, 0x0, 0x10000);
    memory_region_set_readonly(&s->rom, true);
    memory_region_init_alias(&s->ram, NULL, "RAM overlay", ram, 0x0, 0x20000);

    qemu_register_reset(sy6522_reset, s);
    s->timer = timer_new_ns(QEMU_CLOCK_REALTIME, timer_interrupt, s);
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);
    timer_mod(s->timer, now + get_ticks_per_sec());

    sy6522_reset(s);
}
