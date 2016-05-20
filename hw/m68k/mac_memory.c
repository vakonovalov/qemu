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

#define ROM_LOAD_ADDR 0x400000
#define MAX_ROM_SIZE  0x20000
#define IWM_BASE_ADDR 0xDFE1FF // dBase
#define VIA_BASE_ADDR 0xEFE1FE // vBase
#define RAM_SIZE 0x20000
#define ROM_SIZE 0x10000

static MemoryRegion *ram;

void *mac_get_ram_ptr(void)
{
    return memory_region_get_ram_ptr(ram);
}

static void map_memory(hwaddr start, hwaddr end, memory_region *mem_reg, memory_region *mem_subreg)
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
void memory_init(void)
{
    MemoryRegion *address_space_mem = get_system_memory();
    MemoryRegion *lower_memory = g_new(MemoryRegion, 1);
    MemoryRegion *lower_memory_overlay = g_new(MemoryRegion, 1);
    MemoryRegion *upper_memory = g_new(MemoryRegion, 1);
    ram = g_new(MemoryRegion, 1);
    MemoryRegion *rom = g_new(MemoryRegion, 1);

    memory_region_init(lower_memory, NULL, "Lower memory", 0x800000);
    memory_region_init(lower_memory_overlay, NULL, "Lower memory overlay", 0x800000);
    memory_region_init(upper_memory, NULL, "Upper memory", 0x800000);

    memory_region_add_subregion(address_space_mem, 0, lower_memory);
    memory_region_add_subregion(address_space_mem, 0, upper_memory);

    /* RAM at address zero */
    memory_region_allocate_system_memory(ram, NULL, "mac128k.ram", RAM_SIZE);

    /* ROM */
    memory_region_init_ram(rom, NULL, "mac128k.rom", MAX_ROM_SIZE, &error_abort);
    memory_region_set_readonly(rom, true);

    map_memory(0x0, 0x400000, lower_memory, ram);
    map_memory(0x400000, 0x500000, lower_memory, rom);
    map_memory(0x600000, 0x700000, lower_memory, rom);

    map_memory(0x0, 0x100000, lower_memory_overlay, rom);
    map_memory(0x200000, 0x300000, lower_memory_overlay, rom);
    map_memory(0x400000, 0x500000, lower_memory_overlay, rom);
    map_memory(0x600000, 0x800000, lower_memory_overlay, ram);
}