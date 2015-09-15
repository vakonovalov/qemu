#ifndef HW_MAC128K_H
#define HW_MAC128K_H

typedef struct via_state via_state;
typedef struct keyboard_state keyboard_state;

struct MemoryRegion;

/* iwm.c */
void iwm_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu, via_state *via);

keyboard_state *keyboard_init(via_state *via, qemu_irq irq);

void keyboard_handle_cmd(keyboard_state *s);

#endif
