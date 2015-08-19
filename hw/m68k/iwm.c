/*
 * Integrated Woz Machine (IWM) chip for disk control
 */

#include "hw/hw.h"
#include "mac128k.h"
#include "sy6522.h"

typedef struct via_state via_state;
#define HIGHBIT 7
#define HIGHBIT_MASK (1 << HIGHBIT)
#define LOWBIT 0
#define LOWBIT_MASK (1 << LOWBIT)
#define CMDW_MASK 0x06

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
    IWM_lines = 8
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
    uint8_t lines[IWM_lines];
    uint8_t regs[IWM_REGS];
} iwm_state;

static void cmd_handw(iwm_state *s)
{
    uint8_t SEL = via_get_reg(s->via, vBufA);
    uint8_t cmd = 0;
    cmd |= s->lines[CA1];
    cmd <<= 1;
    cmd |= s->lines[CA0];
    cmd <<= 1;
    cmd |= (SEL & REGA_SEL_MASK) >> SELBIT; 
    if ((cmd & ~CMDW_MASK) == 0x00) {
        s->regs[cmd] &= ~LOWBIT_MASK;
        s->regs[cmd] |= (s->lines[CA2] >> LOWBIT) & LOWBIT_MASK;
        printf("    Write s->reg[%d] = %x\n", cmd, s->regs[cmd]);
    } else {
        printf("Unknown command: 0x%x\n", cmd);
    }
}

static void cmd_handr(iwm_state *s)
{
    uint8_t SEL = via_get_reg(s->via, vBufA);
    uint8_t cmd = 0;
    cmd |= s->lines[CA2];
    cmd <<= 1;
    cmd |= s->lines[CA1];
    cmd <<= 1;
    cmd |= s->lines[CA0];
    cmd <<= 1;
    cmd |= (SEL & REGA_SEL_MASK) >> SELBIT;
    if ((cmd == 6) || (cmd == 10) || (cmd == 11) || (cmd == 13) || (cmd == 14)) {
        printf("Unknown command: 0x%x\n", cmd);
    } else {
        s->lines[Q7] &= ~HIGHBIT_MASK;
        s->lines[Q7] |= (s->regs[cmd] << HIGHBIT) & HIGHBIT_MASK;
        printf("    Reads->reg[%d] = %x lines=%x\n", cmd, s->regs[cmd], s->lines[Q7]);
    }
}

static void iwm_set_reg(iwm_state *s, hwaddr offset)
{
    uint8_t old = s->lines[offset >> 1];
    uint8_t new = offset % 2;

    if (!old && new && (offset >> 1 == LSTRB)) {
        cmd_handw(s);
    }

    s->lines[offset >> 1] = new;
}

static void iwm_writeb(void *opaque, hwaddr offset,
                              uint32_t value)
{
    iwm_state *s = (iwm_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset > 0xF) {
        hw_error("Bad IWM write offset 0x%x", (int)offset);
    }
    qemu_log("iwm_write\n");
    iwm_set_reg(s, offset);
    printf("iwm_write offset=%x, regs[%x]=%x\n",
          (int)offset, ((int)offset >> 1), s->regs[offset >> 1]);
}

static uint32_t iwm_readb(void *opaque, hwaddr offset)
{
    iwm_state *s = (iwm_state *)opaque;
    offset = (offset - (s->base & ~TARGET_PAGE_MASK)) >> 9;
    if (offset >= IWM_REGS) {
        hw_error("Bad iwm read offset 0x%x", (int)offset);
    }
    qemu_log("iwm_read\n");
    // printf("iwm_read offset=%x, regs[%x]=%x\n",
    //       (int)offset, ((int)offset >> 1), s->regs[offset >> 1]);
    if (s->lines[Q6] && (offset >> 1 == 7)) {
        cmd_handr(s);
    }
    return s->lines[offset >> 1];
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
    for (i = 0; i < IWM_lines; i++) {
        s->lines[i] = 0;
    }
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
