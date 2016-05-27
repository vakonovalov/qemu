/*
 * Macintosh 128K system emulation.
 *
 * This code is licensed under the GPL
 */

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
#include "mac_int_control.h"
#include "mac_sound_generator.h"
#include "mac_memory.h"

#define SCREEN_WIDTH  512
#define SCREEN_HEIGHT 342

typedef struct {
    QemuConsole *con;
    int invalidate;
} mac_display;

/* Display controller */

typedef void (*drawfn)(uint8_t *, const uint8_t *, int);

#define DEPTH 8
#include "hw/display/mac_display_template.h"
#define DEPTH 15
#include "hw/display/mac_display_template.h"
#define DEPTH 16
#include "hw/display/mac_display_template.h"
#define DEPTH 32
#include "hw/display/mac_display_template.h"

static drawfn draw_line_table[33] = {
    [0 ... 32]	= NULL,
    [8]		= draw_line_8,
    [15]	= draw_line_15,
    [16]	= draw_line_16,
    [32]	= draw_line_32,
};

static void mac_update_display(void *opaque)
{
    mac_display *s = (mac_display*)opaque;
    DisplaySurface *surface = qemu_console_surface(s->con);
    uint8_t *dest;
    uint8_t *src;
    int line;

    if (!surface_bits_per_pixel(surface)) {
        return;
    }

    drawfn draw_line = draw_line_table[surface_bits_per_pixel(surface)];
    dest = surface_data(surface);
    src = qemu_get_ram_ptr(0x1a700);
    for (line = 0 ; line < SCREEN_HEIGHT ; ++line) {
        draw_line(dest, src, SCREEN_WIDTH);
        dest += surface_stride(surface);
        src += SCREEN_WIDTH / 8;
    }
    dpy_gfx_update(s->con, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    s->invalidate = 0;
}

static void mac_invalidate_display(void *opaque) {
    mac_display *s = opaque;
    s->invalidate = 1;
}


static const GraphicHwOps mac_display_ops = {
    .invalidate  = mac_invalidate_display,
    .gfx_update  = mac_update_display,
};

/* Board init.  */

static void mac128k_init(MachineState *machine)
{
    const char *cpu_model = machine->cpu_model;
    const char *kernel_filename = machine->kernel_filename;
    M68kCPU *cpu;
    int kernel_size;
    memory_state *mem_st;
    via_state *via_st;
    int_state *int_st;
    sound_generator_state *snd_st;
    mac_display *display = (mac_display *)g_malloc0(sizeof(mac_display));

    if (!cpu_model) {
        cpu_model = "m68000";
    }

    cpu = cpu_m68k_init(cpu_model);
    if (!cpu) {
        hw_error("Unable to find m68k CPU definition\n");
    }

    mem_st = mac_memory_init();
    int_st = int_init();
    snd_st = mac_sound_generator_init();
    via_st = sy6522_init(mem_st, VIA_BASE_ADDR, int_st, snd_st, cpu);
    iwm_init(mem_st, IWM_BASE_ADDR, cpu, via_st);
    z8530_init(mem_st, 0x0, via_st, int_st, cpu);

    /* Display */
    display->con = graphic_console_init(NULL, 0, &mac_display_ops, display);
    qemu_console_resize(display->con, SCREEN_WIDTH, SCREEN_HEIGHT);

    /* Load kernel.  */
    if (kernel_filename) {
        kernel_size = load_image_targphys(kernel_filename,
                                          ROM_LOAD_ADDR,
                                          MAX_ROM_SIZE);
        if (kernel_size < 0) {
            fprintf(stderr, "qemu: could not load kernel '%s'\n",
                    kernel_filename);
            exit(1);
        }
    }
}

static QEMUMachine mac128k_machine = {
    .name = "mac128k",
    .desc = "Macintosh 128K",
    .init = mac128k_init,
};

static void mac128k_machine_init(void)
{
    qemu_register_machine(&mac128k_machine);
}

machine_init(mac128k_machine_init);
