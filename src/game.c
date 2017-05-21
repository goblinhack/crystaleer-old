/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the README file for license info for license.
 */

#include <SDL.h>

#include "main.h"
#include "sdl.h"
#include "game.h"
#include "thing.h"
#include "glapi.h"
#include "tile.h"
#include "game.h"
#include "python.h"

void game_map_create (void)
{
}

void game_map_destroy (void)
{
}

void game_display (void)
{
    double mx = game.video_pix_width / 2;
    double my = game.video_pix_width / 4;
    my += game.video_pix_width / 16;

    double tw = game.tile_width;
    double th = game.tile_height;

    tw /= 8;
    th /= 8;
    tw *= 5;
    th *= 5;

    double tw2 = tw / 2;
    double th2 = th / 2;

    size_t i;

    things_sort();

    for (i = 0; i < things_draw_list_count; i++) {
	thingp t = things_draw_list[i];

        thing_animate(t);
    }

    blit_init();

    for (i = 0; i < things_draw_list_count; i++) {
	thingp t = things_draw_list[i];

        double X = t->at.x;
        double Y = t->at.y;
        double Z = t->at.z;

        double x = mx - X * (tw / 2);
        double y = my + X * (th / 4);

        x += Y * (tw / 2);
        y += Y * (th / 4);
        y -= Z * (th / 4) * 3;

        fpoint tl;
        fpoint br;

        tl.x = x - tw2;
        tl.y = y - th2;
        br.x = x + tw2;
        br.y = y + th2;

        tpp tp = thing_tp(t);

        if (!t->tile) {
            ERR("no tile for %s", tp_name(tp));
            continue;
        }

        tile_blit_fat(tp, t->tile, 0, &tl, &br);
    }

    blit_flush();
}

uint8_t
game_mouse_motion (int32_t x, int32_t y,
                   int32_t relx, int32_t rely,
                   int32_t wheelx, int32_t wheely)
{
    py_call_void_module_ptr_iiiii("game",
                                  "game_mouse_over",
                                  0, 0, 0,
                                  wheelx, wheely,
                                  mouse_down);
    return (true);
}

void
game_mouse_over (int32_t relx, int32_t rely,
                int32_t wheelx, int32_t wheely)
{

    py_call_void_module_ptr_iiiii("game",
                                  "game_mouse_over",
                                  0, 0, 0,
                                  wheelx, wheely,
                                  mouse_down);
}

uint8_t
game_mouse_down (int32_t x, int32_t y, uint32_t button)
{
    int ret;

    ret = py_call_int_module_ptr_iii("game",
                                     "game_mouse_down",
                                     0, 0, 0,
                                     button);
    return (ret);
}

uint8_t
game_key_down (const struct SDL_KEYSYM *key)
{
    int ret;

    ret = py_call_int_module_ptr_ii("game",
                                    "game_key_down",
                                    0,
                                    key->sym, key->mod);
    return (ret);
}
