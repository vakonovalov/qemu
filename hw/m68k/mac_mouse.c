#include "hw/hw.h"
#include "exec/address-spaces.h"
#include "hw/irq.h"
#include "ui/input.h"
#include "sy6522.h"
#include "z8530.h"
#include "mac_mouse.h"

typedef struct mouse_state {
    qemu_irq irq;
    uint8_t cmd;
    uint8_t model_number_flag;
    QEMUTimer *timer;
    via_state *via;
    Z8530State *z8530;
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
    uint8_t dcd;

    switch (evt->kind) {
    case INPUT_EVENT_KIND_REL:
        if (evt->rel->axis == INPUT_AXIS_X) {
            printf("Check X  ");
            s->mouse_dx += evt->rel->value;
            dcd = z8530_get_reg(s->z8530, 0, 0);
            z8530_set_reg(s->z8530, 0, 0, dcd ^ 0x08);
            if (dcd == 0 && evt->rel->value < 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) & 0xef);
            if (dcd == 0 && evt->rel->value > 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) | 0x10);
            if (dcd == 1 && evt->rel->value < 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) | 0x10);
            if (dcd == 1 && evt->rel->value > 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) & 0xef);
            mouse_interrupt(s->z8530, 0);
        } else if (evt->rel->axis == INPUT_AXIS_Y) {
            printf("Check Y  ");
            s->mouse_dy -= evt->rel->value;
            dcd = z8530_get_reg(s->z8530, 1, 0);
            printf("Dcd %x", dcd);
            z8530_set_reg(s->z8530, 1, 0, dcd ^ 0x08);
            if (dcd == 0 && evt->rel->value < 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) & 0xdf);
            if (dcd == 0 && evt->rel->value > 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) | 0x20);
            if (dcd == 1 && evt->rel->value < 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) | 0x20);
            if (dcd == 1 && evt->rel->value > 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) & 0xdf);
            mouse_interrupt(s->z8530, 1);
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

mouse_state *mouse_init(Z8530State *z8530, via_state *via)
{
    mouse_state *s = (mouse_state *)g_malloc0(sizeof(mouse_state));
    
    s->via = via;
    s->z8530 = z8530;
    s->irq = NULL;
    qemu_input_handler_register((DeviceState *)s,
                                &mouse_handler);
//    s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, timer_callback, s);
    qemu_register_reset(mouse_reset, s);
    mouse_reset(s);

    return s;
}