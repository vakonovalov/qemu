#ifndef Z8530
#define Z8530

typedef struct Z8530State Z8530State;

uint8_t z8530_get_reg(Z8530State *s, uint8_t chn_id, uint8_t number);
void z8530_set_reg(Z8530State *s, uint8_t chn_id, uint8_t number, uint8_t value);
void mouse_interrupt(void * opaque, uint8_t chn_id);

#endif 