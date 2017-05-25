/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the LICENSE file for license.
 */

#include "main.h"
#include "tile.h"
#include "thing_tile.h"
#include "python.h"
#include "config.h"
#include "math_util.h"
#include "time_util.h"

tree_root *things;

static void thing_destroy_internal(thingp t);
static int thing_init_done;

uint8_t thing_init (void)
{
    thing_init_done = true;

    return (true);
}

void thing_fini (void)
{
    if (thing_init_done) {
        thing_init_done = false;

        tree_destroy(&things,
            (tree_destroy_func)thing_destroy_internal);
    }
}

thingp thing_new (const char *name,
                  long int thing_id,
                  const char *tp_name)
{
    thingp t;

#ifdef ENABLE_THING_NAME_IS_UNIQUE_CHECK
    if (thing_find(name)) {
        ERR("thing name [%s] already used", name);
    }
#endif

    if (!things) {
        things = tree_alloc(TREE_KEY_STRING, "TREE ROOT: thing");
    }

    t = (thingp) myzalloc(sizeof(*t), "a thing");

    t->tree.key = dupstr(name, "TREE KEY: thing");
    t->thing_id = thing_id;

    if (!tree_insert(things, &t->tree.node)) {
        ERR("thing insert name [%s] failed", name);
    }

    t->tp = tp_find(tp_name);
    if (!t->tp) {
        THING_ERR(t, "thing [%s] not found", tp_name);
    }

    verify(t);

    THING_LOG(t, "created");

    return (t);
}

static void thing_destroy_internal (thingp t)
{
}

void thing_destroyed_ (thingp t, const char *reason)
{
    verify(t);

    /*
     * Too slow with level changes.
     */
//    THING_LOG(t, "Destroyed thing");

    tree_remove_found_node(things, &t->tree.node);

    thing_destroy_internal(t);

    myfree(t);
}

void thing_set_tp_ (thingp t, const char *tp_name)
{
    verify(t);

    t->tp = tp_find(tp_name);
    if (!t->tp) {
        THING_ERR(t, "thing [%s] not found", tp_name);
    }
}

PyObject *thing_push_ (thingp t, fpoint3d p)
{
    verify(t);

    if (unlikely(!game.tile_width)) {
        game.tile_width = get_game_tile_width();
        game.tile_height = get_game_tile_height();
        if (!game.tile_width) {
            DIE("python did not set tile size prior to thing push");
        }
    }

    if (unlikely(t->is_on_map)) {
        thing_pop_(t);
        Py_RETURN_NONE;
    }

    t->at = p;
    t->is_on_map = true;
    t->moving_start = p;

    PyObject *o = thing_collision_check_(t, p);

    return (o);
}

PyObject *thing_collision_check_ (thingp t, fpoint3d at)
{
    verify(t);

    if (unlikely(!game.tile_width)) {
        game.tile_width = get_game_tile_width();
        game.tile_height = get_game_tile_height();
        if (!game.tile_width) {
            DIE("python did not set tile size prior to thing collision");
        }
    }

    thingp o;
    FOR_ALL_THINGS(o) {
        if (t == o) {
            continue;
        }

        if (!o->is_on_map) {
            continue;
        }

        if (things_iso_collision_check(t, o, at)) {
            return (Py_BuildValue("i", o->thing_id));
        }
    } FOR_ALL_THINGS_END

    Py_RETURN_NONE;
}

void thing_pop_ (thingp t)
{
    verify(t);

    if (unlikely(!t->is_on_map)) {
        return;
    }

    t->is_on_map = false;

    fpoint3d oob = { -1, -1, -1 };
    t->at = oob;
    t->moving_start = oob;
}

static void 
thing_move_increment (thingp t, fpoint3d to)
{
    verify(t);

    if (!t->has_ever_moved) {
        t->last_at = to;
    } else {
        t->last_at = t->at;
    }

    t->has_ever_moved = true;
    t->at = to;

    if (tp_is_animated_lr_flip(thing_tp(t))) {
        if (fabs(t->at.x - t->last_at.x) <= 1) {
            if (t->at.x > t->last_at.x) {
                LOG("TBD flip");
            } else if (t->at.x < t->last_at.x) {
                LOG("TBD flip");
            }
        }
    }
}

void thing_move_ (thingp t, fpoint3d p)
{
    verify(t);

    double ms = tp_get_ms_to_move_one_tile(thing_tp(t));

    ms *= fdist3d(t->at, p);

    uint32_t thing_time = time_get_time_ms();
    t->moving_end = p;
    t->moving_start = t->at;
    t->timestamp_moving_begin = thing_time;
    t->timestamp_moving_end = thing_time + ms;
    t->is_moving = true;

    if (thing_is_player(t)) {
        py_call_void_module_void("hooks", "hook_player_move_start");
    }
}

void thing_move_all (void)
{
    int call_hook_player_move_end = false;

    uint32_t thing_time = time_get_time_ms();

    thingp t;
    FOR_ALL_THINGS(t) {
        if (!t->is_moving) {
            continue;
        }

        if (thing_time >= t->timestamp_moving_end) {
            t->is_moving = false;

            if (thing_is_player(t)) {
                call_hook_player_move_end = true;
            }

            thing_move_increment(t, t->moving_end);

            continue;
        }

        double time_step =
            (double)(thing_time - t->timestamp_moving_begin) /
            (double)(t->timestamp_moving_end - t->timestamp_moving_begin);

        fpoint3d p;

        p.x = (time_step * (double)(t->moving_end.x - t->moving_start.x)) +
            t->moving_start.x;
        p.y = (time_step * (double)(t->moving_end.y - t->moving_start.y)) +
            t->moving_start.y;
        p.z = (time_step * (double)(t->moving_end.z - t->moving_start.z)) +
            t->moving_start.z;

        thing_move_increment(t, p);
    } FOR_ALL_THINGS_END

    if (call_hook_player_move_end) {
        py_call_void_module_void("hooks", "hook_player_move_end");
    }
}

void thing_set_tilename_ (thingp t, const char *name)
{
    fast_verify(w);

    tilep tile = tile_find(name);
    if (!tile) {
        ERR("failed to find wid tile %s", name);
    }

    t->tile = tile;
    if (!t->first_tile) {
        t->first_tile = tile;
    }
}

/*
 * Find an existing thing.
 */
thingp thing_find (const char *name)
{
    thing_template target;
    thingp result;

    if (!name) {
        ERR("no name for thing find");
    }

    // memset(&target, 0, sizeof(target)); intentional for speed
    target.tree.key = (char*) name;

    result = (TYPEOF(result)) tree_find(things, &target.tree.node);
    if (!result) {
        return (0);
    }

    return (result);
}

const char *thing_logname (thingp t)
{
    /*
     * Return constant strings from a small pool.
     */
    static char tmp[10][MAXSTR];
    static int loop;

    verify(t);

    if (loop >= 10) {
        loop = 0;
    }

    snprintf(tmp[loop], sizeof(tmp[loop]) - 1, "%s", t->tree.key);

    return (tmp[loop++]);
}

tree_rootp thing_tile_tiles (thingp t)
{
    verify(t);

    return (tp_get_tiles(thing_tp(t)));
}

void thing_set_is_dead (thingp t, uint8_t val)
{
    verify(t);

    t->is_dead = val;
}

tree_rootp thing_tiles (thingp t)
{
    verify(t);

    return (tp_get_tiles(thing_tp(t)));
}

thing_tilep thing_current_tile (thingp t)
{
    verify(t);

    if (!t->current_tile) {
        return (0);
    }

    return (&thing_tile_arr[t->current_tile]);
}

void thing_move_set_dir (thingp t,
                         double *x,
                         double *y,
                         uint8_t up,
                         uint8_t down,
                         uint8_t left,
                         uint8_t right)
{
    double ox = t->at.x;
    double oy = t->at.y;

    if (*x < 0) {
        *x = 0;
    }

    if (*y < 0) {
        *y = 0;
    }

    if (*x > MAP_WIDTH - 1) {
        *x = MAP_WIDTH - 1;
    }

    if (*y > MAP_HEIGHT - 1) {
        *y = MAP_HEIGHT - 1;
    }

    if (*x > ox) {
        right = true;
    }

    if (*x < ox) {
        left = true;
    }

    if (*y > oy) {
        down = true;
    }

    if (*y < oy) {
        up = true;
    }

    if (up) {
        if (left) {
            thing_set_dir_tl(t);
        } else if (right) {
            thing_set_dir_tr(t);
        } else {
            thing_set_dir_up(t);
        }
    } else if (down) {
        if (left) {
            thing_set_dir_bl(t);
        } else if (right) {
            thing_set_dir_br(t);
        } else {
            thing_set_dir_down(t);
        }
    } else if (left) {
        thing_set_dir_left(t);
    } else if (right) {
        thing_set_dir_right(t);
    }
}

void thing_dead (thingp t,
                 thingp killer,
                 const char *reason, ...)
{
    ERR("thing dead TBD");
}
