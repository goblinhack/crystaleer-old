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

static void 
thing_move_abs_to(thingp t, fpoint3d to);

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

    t->current_tile = -1;

    t->is_over_ladder = false;
    t->is_over_solid_ground = true;

    verify(t);

    t->height = 1.0;
    if (thing_is_player(t)) {
        t->height = 2.0;
    }

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

    thing_move_abs_to(t, p);

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
}

static void 
thing_move_abs_to (thingp t, fpoint3d to)
{
    verify(t);

    thing_animate(t);

    if (!t->has_ever_moved) {
        t->last_at = to;
    } else {
        t->last_at = t->at;
    }

    t->has_ever_moved = true;
    t->is_on_map = true;
    t->at = to;

    t->last_move = time_get_time_ms();
}

static void 
thing_move_delta (thingp t, fpoint3d delta)
{
    verify(t);

    thing_animate(t);

    t->has_ever_moved = true;
    t->is_on_map = true;
    t->at = fadd3d(t->at, delta);

    t->last_move = time_get_time_ms();
}

void thing_move_delta_ (thingp t, fpoint3d delta)
{
    verify(t);

    /*
     * If not moving and this is the first move then break out of the
     * idle animation.
     */
    if (thing_is_dir_none(t)) {
        t->timestamp_change_to_next_frame = time_get_time_ms_cached();
    }

    if (delta.x > 0) {
        thing_set_dir_left(t);
        t->is_moving = true;
    }

    if (delta.x < 0) {
        thing_set_dir_right(t);
        t->is_moving = true;
    }

    if (delta.y > 0) {
        thing_set_dir_up(t);
        t->is_moving = true;
    }

    if (delta.y < 0) {
        thing_set_dir_down(t);
        t->is_moving = true;
    }

    thing_animate(t);

    t->momentum = fadd3d(t->momentum, delta);

    if (thing_is_player(t)) {
        py_call_void_module_void("hooks", "hook_player_move_start");
    }
}

void thing_move_all (void)
{
    int call_hook_player_move_end = false;

    thingp t;
    FOR_ALL_THINGS(t) {
        fpoint3d delta = {0};

        delta = t->momentum;

        if (!t->is_over_solid_ground) {
            delta.z -= t->fall_speed;

            t->fall_speed += game.thing_fall_speed;

            if (t->fall_speed > game.thing_fall_speed_max) {
                t->fall_speed = game.thing_fall_speed_max;
            }
        }

        if (!thing_is_movable(t)) {
            continue;
        }

        fpoint3d to;

        to = fadd3d(t->at, delta);

        /*
         * Check for collisions
         */
        t->is_over_ladder = false;
        t->is_over_solid_ground = false;

        thingp o;
        FOR_ALL_THINGS(o) {

            if (o == t) {
                continue;
            }

            if (things_iso_collision_check(t, o, to)) {
                delta.x = 0;
                delta.y = 0;
                delta.z = 0;
            }

            if (thing_is_player(t)) {
                if (thing_is_ladder(o)) {
                    if (things_iso_overlap(t, o)) {
                        t->is_over_ladder = true;
                        t->fall_speed = 0;
                    }
                }
            }

            if (thing_is_solid_ground(o)) {
                if (!t->is_over_solid_ground) {
                    if (things_iso_collision_check(t, o, to)) {
                        t->is_over_solid_ground = true;
                    }
                }
            }

        } FOR_ALL_THINGS_END

        thing_move_delta(t, delta);

        t->momentum.x *= game.thing_momentum_decay;
        t->momentum.y *= game.thing_momentum_decay;

        t->momentum.x = min(game.thing_momentum_max, t->momentum.x);
        t->momentum.y = min(game.thing_momentum_max, t->momentum.y);

        if ((fabs(t->momentum.x) < 0.01) &&
            (fabs(t->momentum.y) < 0.01)) {
            t->momentum.x = 0.0;
            t->momentum.y = 0.0;
            t->is_moving = false;
        }

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

    snprintf(tmp[loop], sizeof(tmp[loop]) - 1, "%s at (%f,%f,%f)", 
             t->tree.key,
             t->at.x, t->at.y, t->at.z);

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

    if (t->current_tile == -1) {
        return (0);
    }

    return (&thing_tile_arr[t->current_tile]);
}

void thing_dead (thingp t,
                 thingp killer,
                 const char *reason, ...)
{
    ERR("thing dead TBD");
}
