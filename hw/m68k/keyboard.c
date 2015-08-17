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
} keyboard_state;

static void keyboard_event(DeviceState *dev, QemuConsole *src, InputEvent *evt);

static QemuInputHandler keyboard_handler = {
    .name  = "QEMU Macintosh 128K Keyboard",
    .mask  = INPUT_EVENT_MASK_KEY,
    .event = keyboard_event,
};


static void put_keycode(void *opaque, int keycode)
{
//    keyboard_state *s = opaque;
    printf("%x\n",keycode);

}


static void keyboard_event(DeviceState *dev, QemuConsole *src,
                               InputEvent *evt)
{
    keyboard_state *s = (keyboard_state *)dev;
    int scancodes[3], i, count;

    
    count = qemu_input_key_value_to_scancode(evt->key->key,
                                             evt->key->down,
                                             scancodes);
    for (i = 0; i < count; i++) {
        put_keycode(s, scancodes[i]);
    }
}

static void keyboard_reset(void *opaque) {
    keyboard_state *kbd_state = (keyboard_state *)opaque;
    kbd_state->count = 0;
    kbd_state->rw_flag = 0;
    kbd_state->cmd = 0;
    kbd_state->param = 0;
}

void keyboard_init(void) {
    keyboard_state *s = (keyboard_state *)g_malloc0(sizeof(keyboard_state));
    
    qemu_input_handler_register((DeviceState *)s,
                                &keyboard_handler);
    qemu_register_reset(keyboard_reset, s);
    keyboard_reset(s);
}