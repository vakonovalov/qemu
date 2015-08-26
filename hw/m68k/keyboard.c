#include "hw/hw.h"
#include "exec/address-spaces.h"
#include "mac128k.h"
#include "hw/irq.h"
#include "ui/input.h"
#include "sy6522.h"

typedef struct keyboard_state{
    qemu_irq irq;
    uint8_t count;
    uint8_t rw_flag;
    uint8_t cmd;
    uint8_t last_key_pressed;
    QEMUTimer *timer;
    via_state *via;
} keyboard_state;

keyboard_state *__keyboard__123;

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


static void put_value_vSR(void *opaque, int value)
{
    keyboard_state *s = (keyboard_state *)opaque;

    s->cmd = 0;
    via_set_reg(s->via, vSR, value);
    via_set_reg(s->via, vIFR, via_get_reg(s->via, vIFR) | 0x04);
    printf("keycode %x\n",value);
}


static void keyboard_event(DeviceState *dev, QemuConsole *src,
                               InputEvent *evt)
{
    keyboard_state *s = (keyboard_state *)dev;
    int scancodes[3], count;

    count = qemu_input_key_value_to_scancode(evt->key->key,
                                             evt->key->down,
                                             scancodes);
    if (s->cmd == 0x10 || s->cmd == 0x14) {
        if (count == 3) put_value_vSR(s,0x7b);
        else {
            uint8_t keycode_mac;
            keycode_mac = macintosh128k_raw_keycode[scancodes[count-1] & 0x7f];
            keycode_mac = keycode_mac | (scancodes[count-1] & 0x80);
            put_value_vSR(s, keycode_mac);
        }
    }
}

static void keyboard_reset(void *opaque) {
    keyboard_state *kbd_state = (keyboard_state *)opaque;
    kbd_state->count = 0;
    kbd_state->rw_flag = 0;
    kbd_state->cmd = 0;
    kbd_state->last_key_pressed = 0;
}

static void cmd_handler(void *opaque, int irq, int level)
{
    keyboard_state *s = (keyboard_state *)opaque;
    if (irq == 1) {
        s->cmd = via_get_reg(s->via, vSR);
        printf("cmd = %x \n",s->cmd);

        switch(s->cmd) {
            case 0x10:
                timer_mod_ns(s->timer, qemu_clock_get_ns(QEMU_CLOCK_REALTIME) + get_ticks_per_sec());
                break;
            case 0x14:
                put_value_vSR(s, 0x7b);
                break;
            case 0x16:
                put_value_vSR(s, 0xff);
                break;
            case 0x36:
                put_value_vSR(s, 0x7d);
                break;
        }
    }
}

static void timer_callback(void *opaque)
{
    put_value_vSR(opaque, 0x7b);
}

void keyboard_handle_cmd(void) {
    qemu_irq_raise(__keyboard__123->irq);
}

void keyboard_init(via_state *via) {
    keyboard_state *s = (keyboard_state *)g_malloc0(sizeof(keyboard_state));
    
    __keyboard__123 = s;

    s->via = via;
    qemu_input_handler_register((DeviceState *)s,
                                &keyboard_handler);
    s->irq = qemu_allocate_irq(cmd_handler, s, 1);
    s->timer = timer_new_ns(QEMU_CLOCK_REALTIME, timer_callback, s);
    qemu_register_reset(keyboard_reset, s);
    keyboard_reset(s);
}