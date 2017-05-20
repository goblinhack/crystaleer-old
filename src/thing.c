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
thingp things_display_sorted;

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

/*
 * Convert a 3D space position to a 2D isometric position.
 */
static void 
thing_3d_2d (fpoint3d p, double *x, double *y, double *z, double *h)
{
    /*
     * New XY position simply adds Z to X and Y.
     */
    *x = p.x + p.z;
    *y = p.y + p.z;
    *z = p.y + p.z;

    /*
     * Compute horizontal distance from origin at 30 degrees
     *
     * h = (x - y) * Math.cos(Math.PI/6),
     *
     * √3/2 by 7/8 = 1/2 + 1/4 + 1/8 = 0.875:
     */
#if 0
    int H, d;

    H = d = *x - *y;
    H >>= 1;
    H += d;
    H >>= 1;
    H += d;
    H >>= 1;
    *h = H;
#endif
    *h = (x - y) * cos(PI/6);

    /*
     * Compute vertical distance from origin.
     *
     * v: (isoX + isoY) / 2;
     */
}

static void 
thing_set_min_max (thingp t)
{
    double x = t->at.x;
    double y = t->at.y;
    double z = t->at.z;

    double x_top, y_top, z_top, h_top;
    double x_bot, y_bot, z_bot, h_bot;
    double x_ltop, y_ltop, z_ltop, h_ltop;
    double x_rbot, y_rbot, z_rbot, h_rbot;

    {
	fpoint3d p = { x, y, z + 1 };
	thing_3d_2d(p, &x_top, &y_top, &z_top, &h_top);
    }

    {
	fpoint3d p = { x + 1, y + 1, z };
	thing_3d_2d(p, &x_bot, &y_bot, &z_bot, &h_bot);
    }

    {
	fpoint3d p = { x, y + 1, z + 1 };
	thing_3d_2d(p, &x_ltop, &y_ltop, &z_ltop, &h_ltop);
    }

    {
	fpoint3d p = { x + 1, y, z };
	thing_3d_2d(p, &x_rbot, &y_rbot, &z_rbot, &h_rbot);
    }

    t->xmin = min(x_top, min(x_bot, min(x_ltop, x_rbot)));
    t->xmax = max(x_top, max(x_bot, max(x_ltop, x_rbot)));

    t->ymin = min(y_top, min(y_bot, min(y_ltop, y_rbot)));
    t->ymax = max(y_top, max(y_bot, max(y_ltop, y_rbot)));

    t->zmin = min(z_top, min(z_bot, min(z_ltop, z_rbot)));
    t->zmax = max(z_top, max(z_bot, max(z_ltop, z_rbot)));

    t->hmin = min(h_top, min(h_bot, min(h_ltop, h_rbot)));
    t->hmax = max(h_top, max(h_bot, max(h_ltop, h_rbot)));
}

#if 0
static int 
things_overlap (thingp t, thingp r)
{
    return ((t->xmin < r->xmax) && (r->xmin < t->xmax) &&
	    (t->ymin < r->ymax) && (r->ymin < t->ymax) &&
	    (t->hmin < r->hmax) && (r->hmin < t->hmax));
}
#endif

static int 
is_thing_in_front (thingp t, thingp r) 
{
    /*
     * lower x value is in front
     */
    if (r->xmin >= r->xmax) { 
	return (false);
    } else if (r->xmin >= r->xmax) { 
        return (true);
    } else if (r->ymin >= r->ymax) {
        return (false);
    } else if (r->ymin >= r->ymax) {
        return (true);
    } else if (r->hmin >= r->hmax) {
        return (true);
    } else if (r->hmin >= r->hmax) {
        return (false);
    } else {
	ERR("things intersect, cannot sort");
        return (true);
    }
}

static void 
thing_insert (thingp t)
{
    thing_set_min_max(t);

    if (!things_display_sorted) {
        things_display_sorted = t;

        t->next = 0;
        t->prev = 0;
    } else {
        thingp n = things_display_sorted;
        thingp l = 0;

        while (n) {
            if (is_thing_in_front(n, t)) {
                t->prev = n->prev;
                t->next = n;

                if (n->prev) {
                    n->prev->next = t;
                }
                n->prev = t;

                if (n == things_display_sorted) {
                    things_display_sorted = t;
                }

                break;
            }

            l = n;
            n = n->next;
        }

        if (!n) {
            l->next = t;
            t->prev = l;
        }
    }
}

static void 
thing_extract (thingp t)
{
    thingp p = t->prev;
    thingp n = t->next;

    if (p) {
        p->next = n;
    }

    if (n) {
        n->prev = p;
    }

    if (t == things_display_sorted) {
        things_display_sorted = n;
    }

    t->prev = 0;
    t->next = 0;
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
    }


    thing_extract(t);

    t->at = p;
    t->is_on_map = true;
    t->moving_start = p;

    thing_insert(t);

    Py_RETURN_NONE;
}

void thing_pop_ (thingp t)
{
    verify(t);

    if (unlikely(!t->is_on_map)) {
        return;
    }

    thing_extract(t);
    t->is_on_map = false;
}

static void 
thing_move_increment (thingp t, fpoint3d to)
{
    verify(t);

    thing_extract(t);

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

    thing_insert(t);
}

void thing_move_ (thingp t, fpoint3d p)
{
    verify(t);

    double ms = tp_get_speed(thing_tp(t));

    ms *= fdist3d(t->at, p);
CON("move %f %f %f to %f %f %f in %f", t->at.x, t->at.y, t->at.z, p.x,p.y,p.z, ms); 
    uint32_t thing_time = time_get_time_ms();
    t->moving_end = p;
    t->moving_start = t->at;
    t->timestamp_moving_begin = thing_time;
    t->timestamp_moving_end = thing_time + ms;
    t->is_moving = true;
}

void thing_move_all (void)
{
    uint32_t thing_time = time_get_time_ms();
    thingp t = things_display_sorted;

    for (t = things_display_sorted; t; t = t->next) {
        if (!t->is_moving) {
            continue;
        }

        if (thing_time >= t->timestamp_moving_end) {
            t->is_moving = false;

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
