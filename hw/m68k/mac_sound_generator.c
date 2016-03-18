#include "hw/hw.h"
#include "exec/address-spaces.h"
#include "audio/audio.h"
#include "exec/ram_addr.h"
#include <math.h>

#include "mac_sound_generator.h"

#define BUF_SIZE 370
#define PI 3.14159265

static const char *s_sg = "mac sound generator";

typedef struct sound_generator_state {
    MemoryRegion iomem;
    QEMUSoundCard card;
    SWVoiceOut *voice;
//    uint8_t mem_buf[BUF_SIZE*2];
    uint32_t current_memory;
    uint16_t sample_buf[BUF_SIZE];
} sound_generator_state;

/*
static uint64_t sound_generator_mem_read(void *opaque, hwaddr addr,
                              unsigned size)
{
    sound_generator_state *s = opaque;
    uint32_t ret = 0;

    if (addr >= BUF_SIZE*2) {
        hw_error("Bad sound generator read offset 0x%x", (int)addr);
    }

    switch (size) {
        case 1:
            ret = s->mem_buf[addr];
            break;
        case 2:
            ret = (s->mem_buf[addr] << 8) + s->mem_buf[addr + 1];
            break;
        case 4:
            ret = (s->mem_buf[addr] << 24) + (s->mem_buf[addr + 1] << 16) + (s->mem_buf[addr + 2] << 8) + s->mem_buf[addr + 3];
            break;
        default:
            hw_error("Strange size!");
    }
    qemu_log("sound_generator_mem_read\n");
    return ret;
}

static void sound_generator_mem_write(void *opaque, hwaddr addr,
                           uint64_t val, unsigned size)
{
    sound_generator_state *s = opaque;

    if (addr >= BUF_SIZE*2) {
        hw_error("Bad sound generator write offset 0x%x", (int)addr);
    }

    switch (size) {
        case 1:
            s->mem_buf[addr] = val;
            return;
        case 2:
            s->mem_buf[addr] = val >> 8;
            s->mem_buf[addr + 1] = val;
            return;
        case 4:
            s->mem_buf[addr] = val & (0xff000000);
            s->mem_buf[addr + 1] = val & (0x00ff0000);
            s->mem_buf[addr + 2] = val & (0x0000ff00);
            s->mem_buf[addr + 3] = val & (0x000000ff);
            return;
        default:
            hw_error("Strange size!");
            return;
    }
    qemu_log("sound_generator_mem_write write in %s 0x%x\n", addr, re);
}

static const MemoryRegionOps sound_generator_mem_ops = {
    .read = sound_generator_mem_read,
    .write = sound_generator_mem_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};
*/
static inline void convert_mem_buf(sound_generator_state *s)
{
    uint32_t i;
    uint8_t *src;    

    src = qemu_get_ram_ptr(s->current_memory);

    for (i = 0; i < BUF_SIZE; i++, src+=2) {
        s->sample_buf[i] = (uint16_t)(32768-32768*(*src)/255.0);
//        printf("%d %d\n",*src,s->sample_buf[i]);
//        qemu_log("%d %d\n",*src,s->sample_buf[i]);
    }

}

static void mac_sound_generator_callback(void *opaque, int free)
{
    sound_generator_state *s = opaque;

    convert_mem_buf(s);
    AUD_write(s->voice, s->sample_buf, BUF_SIZE);
}

void mac_sound_generator_set_enable(sound_generator_state *s, int on) {
    AUD_set_active_out(s->voice, on);
    if (on) {
        qemu_log("Sound generator enable\n");
    } else {
        qemu_log("Sound generator disable\n");
    } 
}

void mac_sound_generator_set_volume(sound_generator_state *s, uint8_t volume_level) {
    AUD_set_volume_out (s->voice, 0, (uint8_t)(255*(8-volume_level)/8.0), (uint8_t)(255*(8-volume_level)/8.0));
    qemu_log("Sound generator volume %d\n",(uint8_t)(255*(8-volume_level)/8.0));
}

void mac_sound_generator_change_mem_buffer(sound_generator_state *s, int on) {
    if (on) {
        s->current_memory = 0x1FD00;
    } else {
        s->current_memory = 0x1A100;
    }
    qemu_log("Sound generator memory buffer set to %x\n", s->current_memory);
}

static void sound_generator_reset(void *opaque)
{
    uint32_t i;    
    sound_generator_state *s = opaque;

    AUD_set_volume_out (s->voice, 0, 255, 255);
    AUD_set_active_out(s->voice, 0);
    s->current_memory = 0x1FD00;

    uint8_t *src;
    src = qemu_get_ram_ptr(s->current_memory);
    for (i = 0; i < BUF_SIZE; i++, src+=2) {
        *src = (uint8_t)(255*sin((i*PI)/370.0));
 //       qemu_log("%d %d\n",*src,(uint8_t)(255*sin((i*PI)/370.0)));
    }
}

sound_generator_state *mac_sound_generator_init(hwaddr base) {
    sound_generator_state *s = (sound_generator_state *)g_malloc0(sizeof(sound_generator_state));
    struct audsettings as = {44100, 1, AUD_FMT_U16, 0};
/*
    memory_region_init_io(&s->iomem, NULL, &sound_generator_mem_ops, s, "sound_generator", 740);
    memory_region_add_subregion(get_system_memory(), base, &s->iomem);
*/
    AUD_register_card(s_sg, &s->card);

    s->voice = AUD_open_out(&s->card, s->voice, s_sg, s, mac_sound_generator_callback, &as);
    if (!s->voice) {
        AUD_log(s_sg, "Could not open voice\n");
    }

    qemu_register_reset(sound_generator_reset, s);
    sound_generator_reset(s);

    return s;
}
