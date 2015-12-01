#include "hw/hw.h"
#include "exec/address-spaces.h"
#include "hw/irq.h"
#include "ui/input.h"
#include "sy6522.h"
#include "z8530.h"
#include "mac_mouse.h"

#define DIVIDER 100000

typedef struct mouse_state {
    qemu_irq irq;
    uint8_t cmd;
    uint8_t model_number_flag;
    QEMUTimer *timer_x;
    QEMUTimer *timer_y;    
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

    switch (evt->kind) {
    case INPUT_EVENT_KIND_REL:
        if (evt->rel->axis == INPUT_AXIS_X) {
            if (evt->rel->value == 0) break;
            printf("Check X  ");
            s->mouse_dx += evt->rel->value;         

            timer_mod_ns(s->timer_x, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + get_ticks_per_sec() / DIVIDER);
        } else if (evt->rel->axis == INPUT_AXIS_Y) {
            if (evt->rel->value == 0) break;
            printf("Check Y  ");
            s->mouse_dy -= evt->rel->value;

            timer_mod_ns(s->timer_y, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + get_ticks_per_sec() / DIVIDER);            
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

static void timer_x_callback(void *opaque)
{
    mouse_state *s = opaque;
    uint8_t dcd;    

    if (s->mouse_dx) {
        dcd = z8530_get_reg(s->z8530, 0, 0);
        z8530_set_reg(s->z8530, 0, 0, dcd ^ 0x08);
        if (s->mouse_dx > 0) {
            if (dcd == 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) & 0xef);
            if (dcd == 0x08) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) | 0x10);
            s->mouse_dx--;
        } else {
            if (dcd == 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) | 0x10);
            if (dcd == 0x08) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) & 0xef);
            s->mouse_dx++;
        } 
        mouse_interrupt(s->z8530, 0);
        if (s->mouse_dx)
            timer_mod_ns(s->timer_x, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + get_ticks_per_sec() / DIVIDER);
    }
}

static void timer_y_callback(void *opaque)
{
    mouse_state *s = opaque;
    uint8_t dcd;

    if (s->mouse_dy) {
        dcd = z8530_get_reg(s->z8530, 1, 0);
        z8530_set_reg(s->z8530, 1, 0, dcd ^ 0x08);
        if (s->mouse_dy > 0) {
            if (dcd == 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) & 0xdf);
            if (dcd == 0x08) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) | 0x20);
            s->mouse_dy--;
        } else {
            if (dcd == 0) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) | 0x20);
            if (dcd == 0x08) via_set_reg(s->via, vBufB, via_get_reg(s->via, vBufB) & 0xdf);
            s->mouse_dy++;
        } 
        mouse_interrupt(s->z8530, 1);
        if (s->mouse_dy)
            timer_mod_ns(s->timer_y, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + get_ticks_per_sec() / DIVIDER);
    }      
}

static void mouse_reset(void *opaque)
{
    mouse_state *s = (mouse_state *)opaque;
    s->cmd = 0;
    s->model_number_flag = 0;
}

mouse_state *mouse_init(Z8530State *z8530, via_state *via)
{
    mouse_state *s = (mouse_state *)g_malloc0(sizeof(mouse_state));
    
    s->via = via;
    s->z8530 = z8530;
    s->irq = NULL;
    qemu_input_handler_register((DeviceState *)s,
                                &mouse_handler);
    s->timer_x = timer_new_ns(QEMU_CLOCK_VIRTUAL, timer_x_callback, s);
    s->timer_y = timer_new_ns(QEMU_CLOCK_VIRTUAL, timer_y_callback, s);
    qemu_register_reset(mouse_reset, s);
    mouse_reset(s);

    return s;
}