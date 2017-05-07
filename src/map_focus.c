/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the LICENSE file for license.
 */

#include "main.h"
#include "python.h"
#include "string_util.h"

#if 0
static uint8_t
map_mouse_motion (int32_t x, int32_t y,
                  int32_t relx, int32_t rely,
                  int32_t wheelx, int32_t wheely)
{

    py_call_void_module_ptr_iiiii("game",
                                  "map_mouse_over",
                                  0, 0, // TBD
                                  wheelx, wheely,
                                  mouse_down);
    return (true);
}

static void
map_mouse_over (int32_t relx, int32_t rely,
                int32_t wheelx, int32_t wheely)
{

    py_call_void_module_ptr_iiiii("game",
                                  "map_mouse_over",
                                  0, 0, // TBD
                                  wheelx, wheely,
                                  mouse_down);
}

static uint8_t
map_mouse_down (int32_t x, int32_t y, uint32_t button)
{
    int ret;

    ret = py_call_int_module_ptr_iii("game",
                                     "map_mouse_down",
                                     0, 0, // TBD
                                     button);
    return (ret);
}

static uint8_t
map_key_down (const struct SDL_KEYSYM *key)
{
    int ret;

    ret = py_call_int_module_ptr_ii("game",
                                    "map_key_down",
                                    key->sym, key->mod);
    return (ret);
}
#endif
