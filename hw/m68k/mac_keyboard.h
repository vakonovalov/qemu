#ifndef MAC_KEYBOARD
#define MAC_KEYBOARD

typedef struct via_state via_state;
typedef struct keyboard_state keyboard_state;

keyboard_state *keyboard_init(via_state *via, qemu_irq irq);

void keyboard_handle_cmd(keyboard_state *s);

#endif 