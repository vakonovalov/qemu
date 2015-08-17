/*
 * Integrated Woz Machine (IWM) chip for disk control
 */

#include "hw/hw.h"
#include "mac128k.h"
#include "sy6522.h"

typedef struct via_state via_state;

enum
{
    CA0      = 0,
    CA1      = 1,
    CA2      = 2,
    LSTRB    = 3,
    ENABLE   = 4,
    SELECT   = 5, /* 0 - internal, 1 - external */
    Q6       = 6,
    Q7       = 7,
    IWM_BITS = 8
};

enum
{
    DIRTN   = 0,
    CSTIN   = 1,
    STEP    = 2,
    WRTPRT  = 3,
    MOTORON = 4,
    TKO     = 5,
    EIECT   = 6,
    TACH    = 7,
    RDDATAO = 8,
    RDDATA1 = 9,
    SIDES   = 12,
    DRVIN   = 15,
    IWM_REGS = 16
};

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    /* base address */
    target_ulong base;
    via_state *via;
    uint8_t bits[IWM_BITS];
    uint8_t regs[IWM_REGS];
} iwm_state;

static void cmd_handw(iwm_state *s)
{
    uint8_t SEL = ret_reg(s->via, vBufA);
    if (!SEL) {
        if (s->bits[CA0]) {
            if (s->bits[CA1]) {
                s->regs[EIECT] = s->regs[CA2];
            } else {
                s->regs[STEP] = s->regs[CA2];
            }
        } else {
            if (s->bits[CA1]) {
                s->regs[MOTORON] = s->regs[CA2];
            } else {
                s->regs[DIRTN] = s->regs[CA2];
            }
        }
    }
}

static void cmd_handr(iwm_state *s)
{
    uint8_t SEL = ret_reg(s->via, vBufA);
    uint8_t cmd = 0;
    cmd |= s->bits[CA2];
    cmd <<= 1;
    cmd |= s->bits[CA1];
    cmd <<= 1;
    cmd |= s->bits[CA0];
    cmd <<= 1;
    cmd |= s->bits[SEL];

	s->bits[Q7] = s->regs[cmd];
    s->bits[Q7] <<= 7;
}

static void set_reg(iwm_state *s, hwaddr offset)
{
    uint8_t old = s->bits[offset >> 1];
    uint8_t new = offset % 2;

    if (!(old & REGA_SEL_MASK) && (new & REGA_SEL_MASK)) {
        cmd_handw(s);
    }

    s->bits[offset >> 1] = new;
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
    set_reg(s, offset);
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
    if (s->bits[Q6]) {
        cmd_handr(s);
    }
    return s->bits[offset >> 1];
}

static void iwm_writew(void *opaque, hwaddr offset,
                              uint32_t value)
{
	iwm_writeb(opaque, offset, value);
}

static void iwm_writel(void *opaque, hwaddr offset,
                              uint32_t value)
{
	iwm_writeb(opaque, offset, value);
}

static uint32_t iwm_readw(void *opaque, hwaddr offset)
{
    iwm_readb(opaque, offset);
	return 0;
}

static uint32_t iwm_readl(void *opaque, hwaddr offset)
{
    iwm_readb(opaque, offset);
	return 0;
}

static const MemoryRegionOps iwm_ops = {
    .old_mmio = {
        .read = {
            iwm_readb,
            iwm_readw,
            iwm_readl,
        },
        .write = {
            iwm_writeb,
            iwm_writew,
            iwm_writel,
        },
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void iwm_reset(void *opaque)
{
    iwm_state *s = opaque;
    s->bits[Q6] = 1;
    s->bits[Q7] = 0;
    s->bits[ENABLE] = 0;
    s->bits[SELECT] = 0;
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
