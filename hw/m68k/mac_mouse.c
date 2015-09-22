#include "hw/hw.h"
#include "exec/address-spaces.h"
#include "hw/irq.h"
#include "ui/input.h"
#include "sy6522.h"
#include "mac_mouse.h"

typedef struct mouse_state {
    qemu_irq irq;
    uint8_t cmd;
    uint8_t model_number_flag;
    QEMUTimer *timer;
    via_state *via;
    int32_t mouse_dx;
    int32_t mouse_dy;
    uint8_t mouse_button;
} mouse_state;

static void mouse_event(DeviceState *dev, QemuConsole *src, InputEvent *evt);

static QemuInputHandler mouse_handler = {
    .name  = "QEMU Macintosh 128K mouse",
    .mask  = INPUT_EVENT_MASK_BTN | INPUT_EVENT_MASK_REL,
    .event = mouse_event,
};

static void mouse_event(DeviceState *dev, QemuConsole *src,
                               InputEvent *evt)
{
    mouse_state *s = (mouse_state *)dev;

    switch (evt->kind) {
    case INPUT_EVENT_KIND_REL:
        if (evt->rel->axis == INPUT_AXIS_X) {
            s->mouse_dx += evt->rel->value;
        } else if (evt->rel->axis == INPUT_AXIS_Y) {
            s->mouse_dy -= evt->rel->value;
        }
        printf("Mouse: x = %d, y = %d\n", s->mouse_dx, s->mouse_dy);
        break;

    case INPUT_EVENT_KIND_BTN:
        if (evt->btn->down) {
            s->mouse_button = 1;
        } else {
            s->mouse_button = 0;
        }
        printf("Mouse: button = %d\n", s->mouse_button);
        break;

    default:
        printf("Mouse: wait\n");
        /* keep gcc happy */
        break;
    }
    //printf("Mouse: x = %d, y = %d, button = %d\n", s->mouse_dx, s->mouse_dy, s->mouse_button);
}

static void mouse_reset(void *opaque)
{
    mouse_state *kbd_state = (mouse_state *)opaque;
    kbd_state->cmd = 0;
    kbd_state->model_number_flag = 0;
}

mouse_state *mouse_init(via_state *via, qemu_irq irq)
{
    mouse_state *s = (mouse_state *)g_malloc0(sizeof(mouse_state));
    
    s->via = via;
    qemu_input_handler_register((DeviceState *)s,
                                &mouse_handler);
//    s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, timer_callback, s);
    s->irq = irq;
    qemu_register_reset(mouse_reset, s);
    mouse_reset(s);

    return s;
}