#ifndef HW_MAC128K_H
#define HW_MAC128K_H

typedef struct via_state via_state;

struct MemoryRegion;

/* iwm.c */
void iwm_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu, via_state *via);

void keyboard_init(void *via_state);

void via_set_reg_vSR(via_state *s, uint8_t val);
uint8_t via_read_reg_vSR(via_state *s);
void via_set_reg_vIFR(via_state *s, uint8_t val);

#endif
