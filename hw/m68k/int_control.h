#ifndef INT_CONTROL
#define INT_CONTROL

typedef struct int_state int_state;

void set_hw_irq(void *cpu, void *int_st, bool bit, uint8_t vector);
int_state *int_init(void);

#endif