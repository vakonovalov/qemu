#ifndef MAC_MOUSE
#define MAC_MOUSE

typedef struct via_state via_state;
typedef struct Z8530State Z8530State;
typedef struct mouse_state mouse_state;

mouse_state *mouse_init(Z8530State *z8530, via_state *via);

#endif
