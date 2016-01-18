#include "hw/hw.h"
#include "int_control.h"

typedef struct int_state {
	uint8_t level;
	uint8_t vector;
} int_state;

void set_hw_irq(void *cpu, void *int_st, bool bit, uint8_t vector)
{
    int_state *s = (int_state *)int_st;
    
    qemu_log("bit %x, vector %x\n", bit, vector);
    s->level &= ~(1 << (vector - 0x19));
    s->level |= (bit << (vector - 0x19));
    s->vector = 0x18 + s->level;
    qemu_log("level %x, vector %x\n", s->level, s->vector);
    if (s->level == 3) {
        m68k_set_irq_level(cpu, 2, 0x68 >> 2);
        //m68k_set_irq_level(cpu, stored_level, vector);
    } else {
        m68k_set_irq_level(cpu, s->level, s->vector);
    }
}

int_state *int_init(void)
{
    int_state *s;

    s = (int_state *)g_malloc0(sizeof(int_state));

    s->level = 0;
    s->vector = 0;

    return s;
}
