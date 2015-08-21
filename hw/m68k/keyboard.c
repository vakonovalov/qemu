#include "hw/hw.h"
#include "exec/address-spaces.h"
#include "mac128k.h"
#include "sysemu/sysemu.h"
#include "hw/irq.h"
#include "ui/input.h"

typedef struct keyboard_state{
    qemu_irq irq;
    uint8_t count;
    uint8_t rw_flag;
    uint8_t cmd;
    uint8_t param;
    QEMUTimer *timer;
    struct via_state *via;
} keyboard_state;

static const unsigned char macintosh128k_raw_keycode[128] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0x19, 0x1b, 0x1d, 0x1f, 0x23, 0x21, 0x41, 0x45, 0x3f, 0x47,  0,  0,  0,  0,  0x1,  0x3,
  0x5,  0x7,  0xb,  0x9,  0x4d,  0x51,  0x4b,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

static void keyboard_event(DeviceState *dev, QemuConsole *src, InputEvent *evt);

static QemuInputHandler keyboard_handler = {
    .name  = "QEMU Macintosh 128K Keyboard",
    .mask  = INPUT_EVENT_MASK_KEY,
    .event = keyboard_event,
};


static void put_keycode(void *opaque, int keycode)
{
    keyboard_state *s = (keyboard_state *)opaque;

    via_set_reg_vSR(s->via, keycode);
    via_set_reg_vIFR(s->via, s->via->regs[13] | 0x04);
    printf("%x\n",keycode);
}


static void keyboard_event(DeviceState *dev, QemuConsole *src,
                               InputEvent *evt)
{
    keyboard_state *s = (keyboard_state *)dev;
    int scancodes[3], count;

    printf("%x ", qemu_input_key_value_to_number(evt->key->key));
    count = qemu_input_key_value_to_scancode(evt->key->key,
                                             evt->key->down,
                                             scancodes);

    if (count == 3) put_keycode(s,0);
    else put_keycode(s, scancodes[count-1]);
}

static void keyboard_reset(void *opaque) {
    keyboard_state *kbd_state = (keyboard_state *)opaque;
    kbd_state->count = 0;
    kbd_state->rw_flag = 0;
    kbd_state->cmd = 0;
    kbd_state->param = 0;
}

void keyboard_init(void *via) {
    keyboard_state *s = (keyboard_state *)g_malloc0(sizeof(keyboard_state));
    
    s->via = via;
    qemu_input_handler_register((DeviceState *)s,
                                &keyboard_handler);
    qemu_register_reset(keyboard_reset, s);
    keyboard_reset(s);
}