/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the README file for license info for license.
 */

extern void game_map_create(void);
extern void game_map_destroy(void);
extern void game_display(void);

extern uint8_t
game_mouse_motion(int32_t x, int32_t y,
                  int32_t relx, int32_t rely,
                  int32_t wheelx, int32_t wheely);

extern void
game_mouse_over(int32_t relx, int32_t rely,
                int32_t wheelx, int32_t wheely);

extern uint8_t
game_mouse_down(int32_t x, int32_t y, uint32_t button);

extern uint8_t
game_key_down(const struct SDL_KEYSYM *key);
