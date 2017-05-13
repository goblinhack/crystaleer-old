/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the README file for license info for license.
 */

#include "main.h"
#include "game.h"
#include "thing.h"
#include "glapi.h"
#include "tile.h"

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

    tw /= 4;
    th /= 4;
    tw *= 3;
    th *= 3;

    double tw2 = tw / 2;
    double th2 = th / 2;

    thingp t = things_display_sorted;
    while (t) {
        thing_animate(t);

        t = t->next;
    }

    blit_init();

    t = things_display_sorted;
    while (t) {
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

        tile_blit_fat(tp, t->tile, 0, &tl, &br);

        t = t->next;
    }

    blit_flush();
}
