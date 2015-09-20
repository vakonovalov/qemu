#ifndef HW_MAC128K_H
#define HW_MAC128K_H

typedef struct via_state via_state;

struct MemoryRegion;

/* sy6522.c */
via_state *sy6522_init(MemoryRegion *rom, MemoryRegion *ram, uint32_t base, M68kCPU *cpu);
/* iwm.c */
void iwm_init(MemoryRegion *sysmem, uint32_t base, M68kCPU *cpu, via_state *via);

#endif
