#ifndef SOUND_GEERATOR_H
#define SOUND_GEERATOR_H

typedef struct sound_generator_state sound_generator_state;

sound_generator_state *mac_sound_generator_init(void);
void mac_sound_generator_set_enable(sound_generator_state *s, int on);
void mac_sound_generator_set_volume(sound_generator_state *s, uint8_t volume_level);
void mac_sound_generator_change_mem_buffer(sound_generator_state *s, int on);

#endif
