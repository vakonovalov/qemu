#ifndef MAC_MOUSE
#define MAC_MOUSE

typedef struct via_state via_state;
typedef struct mouse_state mouse_state;

mouse_state *mouse_init(via_state *via, qemu_irq irq);

#endif
