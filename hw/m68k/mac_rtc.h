#ifndef MAC_RTC
#define MAC_RTC

typedef struct via_state via_state;
typedef struct rtc_state rtc_state;

rtc_state *rtc_init(qemu_irq irq);

void rtc_receive(rtc_state *rtc, uint8_t val);
uint8_t rtc_send(rtc_state *rtc, uint8_t val);
void rtc_param_reset(rtc_state *rtc);

#endif 