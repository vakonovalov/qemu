/*
 * Integrated Woz Machine (IWM) chip for disk control
 */

#include "hw/hw.h"
#include "mac128k.h"
#include "sy6522.h"

#define HIGHBIT 7
#define HIGHBIT_MASK (1 << HIGHBIT)
#define LOWBIT 0
#define LOWBIT_MASK (1 << LOWBIT)
#define CMDW_MASK 0x06


typedef struct via_state via_state;

enum
{
    CA0       = 0,
    CA1       = 1,
    CA2       = 2,
    LSTRB     = 3,
    ENABLE    = 4,
    SELECT    = 5, /* 0 - internal, 1 - external */
    Q6        = 6,
    Q7        = 7,
    IWM_LINES = 8
};

enum
{
    DIRTN    = 0,
    CSTIN    = 1,
    STEP     = 2,
    WRTPRT   = 3,
    MOTORON  = 4,
    TKO      = 5,
    EIECT    = 6,
    TACH     = 7,
    RDDATAO  = 8,
    RDDATA1  = 9,
    SIDES    = 12,
    DRVIN    = 15,
    IWM_REGS = 16
};

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    /* base address */
    target_ulong base;
    via_state *via;
    uint8_t LINES[IWM_LINES];
    uint8_t internal_regs[IWM_REGS];
    uint8_t external_regs[IWM_REGS];
} iwm_state;

static uint8_t *iwm_get_regs(iwm_state *s)
{
    if (s->LINES[SELECT]) {
        return s->external_regs;
    } else {
        return s->internal_regs;
    }
}

static void cmd_handw(iwm_state *s)
{
    uint8_t sel = via_get_reg(s->via, vBufA);
    uint8_t cmd = 0;
    uint8_t *reg = iwm_get_regs(s);
    cmd |= s->LINES[CA1] & LOWBIT_MASK;
    cmd = (cmd << 1) | (s->LINES[CA0] & LOWBIT_MASK);
    cmd = (cmd << 1) | ((sel & REGA_SEL_MASK) >> SELBIT);
    if ((cmd & ~CMDW_MASK) == 0x00) {
        reg[cmd] &= ~LOWBIT_MASK;
        reg[cmd] |= (s->LINES[CA2] >> LOWBIT) & LOWBIT_MASK;
    } else {
        qemu_log("iwm error: unknown command 0x%x\n", cmd);
    }
}

static void cmd_handr(iwm_state *s)
{
    uint8_t sel = via_get_reg(s->via, vBufA);
    uint8_t cmd = 0;
    uint8_t *reg = iwm_get_regs(s);
    cmd |= s->LINES[CA2] & LOWBIT_MASK;
    cmd = (cmd << 1) | (s->LINES[CA1] & LOWBIT_MASK);
    cmd = (cmd << 1) | (s->LINES[CA0] & LOWBIT_MASK);
    cmd = (cmd << 1) | ((sel & REGA_SEL_MASK) >> SELBIT);
    if ((cmd == 6) || (cmd == 10) || (cmd == 11) || (cmd == 13) || (cmd == 14)) {
        qemu_log("iwm error: unknown command 0x%x\n", cmd);
    } else {
        s->LINES[Q7] &= ~HIGHBIT_MASK;
        s->LINES[Q7] |= (reg[cmd] << HIGHBIT) & HIGHBIT_MASK;
    }
}

static void iwm_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    iwm_state *s = (iwm_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset > 0xF) {
        hw_error("Bad IWM write offset 0x%x", (int)offset);
    }
    qemu_log("iwm_write offset=0x%x value=0x%x\n", (int)offset, value);
    if ((offset % 2) && (offset >> 1 == LSTRB)) {
        cmd_handw(s);
    }
    s->LINES[offset >> 1] = offset % 2;
}

static uint32_t iwm_readb(void *opaque, hwaddr offset)
{
    iwm_state *s = (iwm_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset >= IWM_REGS) {
        hw_error("Bad iwm read offset 0x%x", (int)offset);
    }
    qemu_log("iwm_read offset=0x%x\n", (int)offset);
    if (s->LINES[Q6] && (offset >> 1 == Q7) && !(offset >> 1 == LSTRB)) {
        cmd_handr(s);
    }
    s->LINES[offset >> 1] = offset % 2;
    return s->LINES[offset >> 1];
}

static const MemoryRegionOps iwm_ops = {
    .old_mmio = {
        .read = {
            iwm_readb,
            iwm_readb,
            iwm_readb,
        },
        .write = {
            iwm_writeb,
            iwm_writeb,
            iwm_writeb,
        },
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void iwm_reset(void *opaque)
{
    iwm_state *s = opaque;
    uint8_t i;
    for (i = 0; i < IWM_LINES; i++) {
        s->LINES[i] = 0;
    }
    s->internal_regs[MOTORON] = 1;
    s->external_regs[MOTORON] = 1;
    s->internal_regs[WRTPRT] = 1;
    s->external_regs[WRTPRT] = 1;
}

void iwm_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu, via_state *via)
{
    iwm_state *s;
    s = (iwm_state *)g_malloc0(sizeof(iwm_state));

    s->base = base;
    memory_region_init_io(&s->iomem, NULL, &iwm_ops, s,
                          "iwm", 0x2000);
    memory_region_add_subregion(sysmem, base & TARGET_PAGE_MASK, &s->iomem);

    s->cpu = cpu;
    s->via = via;

    iwm_reset(s);
}
