/*
 * Integrated Woz Machine (IWM) chip for disk control
 */

#include "hw/hw.h"
#include "mac128k.h"
#include "sy6522.h"

#define SEL_MASK (1 << 5)

typedef struct via_state via_state;

enum
{
    CA0 = 0,
    CA1 = 1,
    CA2 = 2,
    LSTRB = 3,
    ENABLE = 4,
    SELECT = 5, /* 0 - internal, 1 - external */
    Q6 = 6,
    Q7 = 7,
    IWM_REGS = 8
};

typedef struct {
    uint8_t DIRTN;
    uint8_t CSTIN;
    uint8_t STEP;
    uint8_t WRTPRT;
    uint8_t MOTORON;
    uint8_t TKO;
    uint8_t EIECT;
    uint8_t TACH;
    uint8_t RDDATAO;
    uint8_t RDDATA1;
    uint8_t SIDES;
    uint8_t DRVIN;
} diskReg_state;

typedef struct {
    M68kCPU *cpu;
    MemoryRegion iomem;
    /* base address */
    target_ulong base;
    uint8_t regs[IWM_REGS];
    uint8_t *SEL_bit;
    diskReg_state disk;
} iwm_state;

static void set_reg(iwm_state *s, hwaddr offset)
{
    //uint8_t old = s->regs[offset >> 1];
    //uint8_t new = offset % 2;
    /*if (!(s->regs[LSTRB])) {
        
        s->regs[offset >> 1] = offset % 2;
    } else if (s->regs[LSTRB]) && (offset >> 1 == LSTRB){
        s->regs[offset >> 1] = offset % 2;
    }*/
    s->regs[offset >> 1] = offset % 2;
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
    return s->regs[offset >> 1];
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

void iwm_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu, via_state *via)
{
    iwm_state *s;
    s = (iwm_state *)g_malloc0(sizeof(iwm_state));

    s->base = base;
    memory_region_init_io(&s->iomem, NULL, &iwm_ops, s,
                          "iwm", 0x2000);
    memory_region_add_subregion(sysmem, base & TARGET_PAGE_MASK, &s->iomem);

    s->regs[Q6] = 1;
    s->regs[Q7] = 0;
    s->cpu = cpu;

    s->regs[ENABLE] = 0;
    s->regs[SELECT] = 0;
    s->SEL_bit = &via->regs[vBufA];
}
