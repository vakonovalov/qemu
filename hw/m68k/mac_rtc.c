#include "hw/irq.h"
#include "sysemu/sysemu.h"
#include "sy6522.h"
#include "mac_rtc.h"

#define RTCSEC_MASK 0x0C
#define RTCRAMBUF1_MASK 0x3C
#define RTCRAMBUF2_MASK 0x0C
#define HOST_TO_MAC_RTC (66 * 365 + 17) * 24 * 3600

typedef struct rtc_state {
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

static void rtc_write_handler(rtc_state *rtc)
{
    if (rtc->cmd == 0x35) {
        rtc->wr_pr_reg = rtc->param;
        qemu_log("rtc: write to wpReg 0x%x\n", rtc->param);
    }
    else if (!(rtc->wr_pr_reg & 0x80)) {
        if ((rtc->cmd & ~RTCSEC_MASK) == 0x01) {
            rtc->sec_reg[(rtc->cmd & RTCSEC_MASK) >> 2] = rtc->param;
            qemu_log("rtc: write to secReg%x 0x%x\n",
                    (rtc->cmd & RTCSEC_MASK) >> 2, rtc->param);
        } else if ((rtc->cmd & ~RTCRAMBUF1_MASK) == 0x41) {
            rtc->buf_RAM[(rtc->cmd & RTCRAMBUF1_MASK) >> 2] = rtc->param;
            qemu_log("rtc: write to rtcRAM[%i] 0x%x\n",
                    (rtc->cmd & RTCRAMBUF1_MASK) >> 2, rtc->param);
        } else if ((rtc->cmd & ~RTCRAMBUF2_MASK) == 0x21) {
            rtc->buf_RAM[16 + ((rtc->cmd & RTCRAMBUF2_MASK) >> 2)] = rtc->param;
            qemu_log("rtc: write to rtcRAM[%i] 0x%x\n",
                     16 + ((rtc->cmd & RTCRAMBUF2_MASK) >> 2), rtc->param);
        } else if (rtc->cmd == 0x31) {
            rtc->test_reg = rtc->param;
            qemu_log("rtc: write to tesrReg 0x%x\n", rtc->param);
        } else {
            qemu_log("rtc: write error: unknown command 0x%x\n", rtc->cmd);
        }
    } else {
        qemu_log("rtc: write error: write-protect enabled\n");
    }
}

static void rtc_read_handler(rtc_state *rtc)
{
    if ((rtc->cmd & ~RTCSEC_MASK & ~REGB_RTCRWBIT_MASK) == 0x01) {
        rtc->param = rtc->sec_reg[(rtc->cmd & RTCSEC_MASK) >> 2];
        qemu_log("rtc: read from secReg%x 0x%x\n",
                (rtc->cmd & RTCSEC_MASK) >> 2, rtc->param);
    } else if ((rtc->cmd & ~RTCRAMBUF1_MASK & ~REGB_RTCRWBIT_MASK) == 0x41) {
        rtc->param = rtc->buf_RAM[(rtc->cmd & RTCRAMBUF1_MASK) >> 2];
        qemu_log("rtc: read from rtcRAM[%i] 0x%x\n",
                (rtc->cmd & RTCRAMBUF1_MASK) >> 2, rtc->param);
    } else if ((rtc->cmd & ~RTCRAMBUF2_MASK & ~REGB_RTCRWBIT_MASK) == 0x21) {
        rtc->param = rtc->buf_RAM[16 + ((rtc->cmd & RTCRAMBUF2_MASK) >> 2)];
        qemu_log("rtc: read from rtcRAM[%i] 0x%x\n",
                 16 + ((rtc->cmd & RTCRAMBUF2_MASK) >> 2), rtc->param);
    } else {
        qemu_log("rtc: read error: unknown command 0x%x\n", rtc->cmd);
    }
}

void rtc_receive(rtc_state *rtc, uint8_t val)
{
    if (!(rtc->rw_flag)) {
        rtc->cmd |= (val & REGB_RTCDATA_MASK) << (7 - rtc->count);
    } else {
        rtc->param |= (val & REGB_RTCDATA_MASK) << (7 - rtc->count);
    }
    rtc->count++;
    if (rtc->count == 8) {
        if (!(rtc->cmd & REGB_RTCRWBIT_MASK) && !(rtc->rw_flag)) {
            rtc->rw_flag = 1;
            rtc->count = 0;
        } else if (rtc->rw_flag) {
            rtc_write_handler(rtc);
            rtc_param_reset(rtc);
        } else {
            rtc_read_handler(rtc);
            rtc->count = 0;
        }
    }
}

uint8_t rtc_send(rtc_state *rtc, uint8_t val)
{
    val &= ~REGB_RTCDATA_MASK;
    val |= (rtc->param >> (7 - rtc->count)) & REGB_RTCDATA_MASK;
    rtc->count++;
    if (rtc->count == 8) {
        rtc_param_reset(rtc);
    }
    return val;
}

void rtc_param_reset(rtc_state *rtc)
{
    rtc->param = 0;
    rtc->cmd = 0;
    rtc->rw_flag = 0;
    rtc->count = 0;
}

static void rtc_interrupt(void * opaque)
{
    rtc_state *rtc = opaque;
    timer_mod_ns(rtc->timer, qemu_clock_get_ns(rtc_clock) + get_ticks_per_sec());
    if (rtc->sec_reg[0] == 0xFF) {
        if (rtc->sec_reg[1] == 0xFF) {
            if (rtc->sec_reg[2] == 0xFF) {
                rtc->sec_reg[3]++;
            }
            rtc->sec_reg[2]++;
        }
        rtc->sec_reg[1]++;
    }
    rtc->sec_reg[0]++;
    qemu_irq_raise(rtc->irq);
}

static void rtc_reset(rtc_state *rtc)
{
    uint64_t now = qemu_clock_get_ns(rtc_clock) / get_ticks_per_sec()
                 + HOST_TO_MAC_RTC;
    uint8_t i;
    for (i = 0; i < 4; ++i) {
        rtc->sec_reg[i] = (now >> (8 * i)) & 0xFF;
    }
    rtc->wr_pr_reg = 0x80;
    timer_mod_ns(rtc->timer, now + get_ticks_per_sec());
}

rtc_state *rtc_init(qemu_irq irq)
{
    rtc_state *s = (rtc_state *)g_malloc0(sizeof(rtc_state));
    s->irq = irq;
    s->timer = timer_new_ns(rtc_clock, rtc_interrupt, s);
    rtc_reset(s);

    return s;
}