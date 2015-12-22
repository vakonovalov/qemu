#include "hw/hw.h"
#include "int_control.h"
/*
typedef struct int_state {
	uint8_t level;
	uint8_t vector;
} int_state;
*/
void set_hw_irq(void *cpu, bool bit, uint8_t vector)
{
    static uint8_t stored_level = 0;

    qemu_log("bit %x, vector %x\n", bit, vector);
    stored_level &= ~(1 << (vector - 0x19));
    stored_level |= (bit << (vector - 0x19));
    vector = 0x18 + stored_level;
    qemu_log("stored level %x, vector %x\n", stored_level, vector);
    if (stored_level == 3) {
        m68k_set_irq_level(cpu, 2, 0x68 >> 2);
        //m68k_set_irq_level(cpu, stored_level, vector);
    } else {
        m68k_set_irq_level(cpu, stored_level, vector);
    }
}
/*
void int_init()
{
    int_state *s;

    s = (int_state *)g_malloc0(sizeof(int_state));

    s->level = 0;
    s->vector = 0;

    return;
}
*/