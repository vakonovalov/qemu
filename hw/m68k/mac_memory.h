#ifndef MAC_MEMORY
#define MAC_MEMORY

typedef struct memory_state memory_state;

#define ROM_LOAD_ADDR 0x400000
//#define IWM_BASE_ADDR 0xDFE1FF
//#define VIA_BASE_ADDR 0xEFE1FE
#define IWM_BASE_ADDR 0x500000
#define VIA_BASE_ADDR 0x680000
#define MAX_ROM_SIZE 0x20000
#define RAM_SIZE 0x20000
#define ROM_SIZE 0x10000

void *mac_get_ram_ptr(void);
memory_state *mac_memory_init(void);
MemoryRegion *mac_get_upper_memory(memory_state *s);
void mac_change_overlay_memory(memory_state *s, bool overlay_bit);

#endif
