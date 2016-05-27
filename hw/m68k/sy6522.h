#ifndef SY6522
#define SY6522

typedef struct via_state via_state;

#define REGB_RTCDATA_MASK (1 << 0)
#define REGB_RTCCLK_MASK (1 << 1)
#define REGB_RTCENB_MASK (1 << 2)
#define REGB_SNDENB_MASK (1 << 7)
#define SELBIT 5
#define REGA_SNDVOL_MASK 0x7
#define REGA_SNDPG2_MASK (1 << 3)
#define REGA_OVERLAY_MASK (1 << 4)
#define REGA_SEL_MASK (1 << SELBIT)

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

uint8_t via_get_reg(via_state *via, uint8_t offset);
void via_set_reg(via_state *via, uint8_t offset, uint8_t value);

#endif 