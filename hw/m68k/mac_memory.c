#include "hw/hw.h"
#include "mac128k.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "elf.h"
#include "exec/address-spaces.h"
#include "exec/ram_addr.h"
#include "exec/helper-proto.h"
#include "exec/cpu_ldst.h"
#include "sysemu/qtest.h"
#include "ui/console.h"
#include "ui/pixel_ops.h"
#include "mac_memory.h"

typedef struct memory_state {
    MemoryRegion *address_space_mem;
    MemoryRegion *lower_memory;
    MemoryRegion *lower_memory_overlay;
    MemoryRegion *upper_memory;
    MemoryRegion *ram;
    MemoryRegion *rom;
} memory_state;

static MemoryRegion *ram_pt;

void *mac_get_ram_ptr(void)
{
    return memory_region_get_ram_ptr(ram_pt);
}

MemoryRegion *mac_get_upper_memory(memory_state *s)
{
    return s->upper_memory;
}

void mac_change_overlay_memory(memory_state *s, bool overlay_bit)
{
    if (overlay_bit) {
        memory_region_add_subregion_overlap(s->address_space_mem, 0x0, s->lower_memory_overlay, 1);
    } else {
        memory_region_del_subregion(s->address_space_mem, s->lower_memory_overlay);
    }            
}

static void map_memory(hwaddr start, hwaddr end, MemoryRegion *mem_reg, MemoryRegion *mem_subreg)
{
    hwaddr iter;
    uint64_t mem_reg_size = memory_region_size(mem_subreg);

    for (iter = start ; iter < end ; iter += mem_reg_size) {
        MemoryRegion *alias = g_new(MemoryRegion, 1);
        memory_region_init_alias(alias, NULL, "Mirror", mem_subreg, 0x0, mem_reg_size);
        memory_region_add_subregion(mem_reg, iter, alias);
    }
}

//lower_memory - before 0x800000
//upper_memory - after 0x800000
memory_state *mac_memory_init(void)
{
    memory_state *s;

    s = (memory_state *)g_malloc0(sizeof(memory_state));
    s->address_space_mem = get_system_memory();
    s->lower_memory = g_new(MemoryRegion, 1);
    s->lower_memory_overlay = g_new(MemoryRegion, 1);
    s->upper_memory = g_new(MemoryRegion, 1);
    s->ram = g_new(MemoryRegion, 1);
    s->rom = g_new(MemoryRegion, 1);
    ram_pt = s->ram;

    memory_region_init(s->lower_memory, NULL, "Lower memory", 0x800000);
    memory_region_init(s->lower_memory_overlay, NULL, "Lower memory overlay", 0x800000);
    memory_region_init(s->upper_memory, NULL, "Upper memory", 0x800000);

    memory_region_add_subregion(s->address_space_mem, 0, s->lower_memory);
    memory_region_add_subregion(s->address_space_mem, 0x800000, s->upper_memory);

    /* RAM at address zero */
    memory_region_allocate_system_memory(s->ram, NULL, "mac128k.ram", RAM_SIZE);

    /* ROM */
    memory_region_init_ram(s->rom, NULL, "mac128k.rom", ROM_SIZE, &error_abort);
    memory_region_set_readonly(s->rom, true);

    map_memory(0x0, 0x400000, s->lower_memory, s->ram);
    map_memory(0x400000, 0x500000, s->lower_memory, s->rom);
    map_memory(0x600000, 0x700000, s->lower_memory, s->rom);

    map_memory(0x0, 0x100000, s->lower_memory_overlay, s->rom);
    map_memory(0x200000, 0x300000, s->lower_memory_overlay, s->rom);
    map_memory(0x400000, 0x500000, s->lower_memory_overlay, s->rom);
    map_memory(0x600000, 0x800000, s->lower_memory_overlay, s->ram);

    return s;
}