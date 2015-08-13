#ifndef HW_MAC128K_H
#define HW_MAC128K_H

struct MemoryRegion;

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

typedef struct {
    qemu_irq irq;
    uint8_t count;
    uint8_t rw_flag;
    uint8_t cmd;
    uint8_t param;
    uint8_t sec_reg[4]; 
    uint8_t test_reg;
    uint8_t wr_pr_reg;
    uint8_t buf_RAM[20];
    QEMUTimer *timer;
} rtc_state;

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    MemoryRegion rom;
    MemoryRegion ram;
    /* base address */
    target_ulong base;
    /* registers */
    uint8_t regs[VIA_REGS];
    rtc_state rtc;
} via_state;

/* iwm.c */
void iwm_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu, via_state *via);

/* sy6522.c */
void sy6522_init(MemoryRegion *rom, MemoryRegion *ram, uint32_t base, M68kCPU *cpu, via_state *via);

#endif
