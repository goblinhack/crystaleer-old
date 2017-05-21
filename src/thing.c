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

static thingp thing_scratch[MAX_THINGS_SCRATCH];
static size_t thing_scratch_count;

thingp things_todraw[MAX_THINGS_SCRATCH];
size_t things_todraw_count;

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


static void
spaceToIso (ipoint *p) 
{
    double z = p->z;
    double x = p->x + z;
    double y = p->y + z;

    p->x = x;
    p->y = y;
    p->h = (x-y)*sqrt(3)/2; // Math.cos(Math.PI/6)
    p->v = (x+y)/2;         // Math.sin(Math.PI/6)
}

static void 
thing_get_iso_verts (thingp t)
{
    fpoint3d s = { 1.0, 1.0, 1.0 };
    fpoint3d p = { t->at.x, t->at.y, t->at.z };

    ipoint rightDown = { p.x+s.x, p.y,     p.z };
    ipoint leftDown  = { p.x,     p.y+s.y, p.z };
    ipoint backDown  = { p.x+s.x, p.y+s.y, p.z };
    ipoint frontDown = { p.x,     p.y,     p.z };
    ipoint rightUp   = { p.x+s.x, p.y,     p.z+s.z };
    ipoint leftUp    = { p.x,     p.y+s.y, p.z+s.z };
    ipoint backUp    = { p.x+s.x, p.y+s.y, p.z+s.z };
    ipoint frontUp   = { p.x,     p.y,     p.z+s.z };

    spaceToIso(&rightDown);
    spaceToIso(&leftDown);
    spaceToIso(&backDown);
    spaceToIso(&frontDown);
    spaceToIso(&rightUp);
    spaceToIso(&leftUp);
    spaceToIso(&backUp);
    spaceToIso(&frontUp);

    t->rightDown = rightDown;
    t->leftDown  = leftDown;
    t->backDown  = backDown;
    t->frontDown = frontDown;
    t->rightUp   = rightUp;
    t->leftUp    = leftUp;
    t->backUp    = backUp;
    t->frontUp   = frontUp;
}

static void 
thing_get_iso_bounds (thingp t)
{
    thing_get_iso_verts(t);

    t->xmin = t->frontDown.x;
    t->xmax = t->backUp.x;
    t->ymin = t->frontDown.y;
    t->ymax = t->backUp.y;
    t->hmin = t->leftDown.h;
    t->hmax = t->rightDown.h;
}

static void 
thing_get_bounds (thingp t)
{
    t->xmin = t->at.x;
    t->xmax = t->at.x + 1.0;
    t->ymin = t->at.y;
    t->ymax = t->at.y + 1.0;
    t->zmin = t->at.z;
    t->zmax = t->at.z + 1.0;
}

/*
 * Determine if the given ranges are disjoint (i.e. do not overlap).
 * For determining drawing order, this camera considers two
 * ranges to be disjoint even if they share an endpoint.
 * Thus, we use less-or-equal (<=) instead of strictly less (<).
 */
static int
ranges_do_not_overlap (double amin, double amax, double bmin, double bmax)
{
    /*
     * amin ...... amax
     *                   bmin ...... bmax
     */
    if (amax <= bmin) {
	return (true);
    }

    /*
     *                  amin ...... amax
     * bmin ...... bmax
     */
    if (bmax <= amin) {
	return (true);
    }

    return (false);
}

static int
ranges_overlap (double amin, double amax, double bmin, double bmax)
{
    /*
     * amin ...... amax
     *                   bmin ...... bmax
     */
    if (amax <= bmin) {
	return (false);
    }

    /*
     *                  amin ...... amax
     * bmin ...... bmax
     */
    if (bmax <= amin) {
	return (false);
    }

    return (true);
}

/*
 * Try to find an axis in 2D isometric that separates the two given blocks.
 * This helps identify if the the two blocks are overlap on the screen.
 */
static int
things_iso_overlap (thingp a, thingp b)
{
    if (ranges_overlap(a->xmin, a->xmax, b->xmin, b->xmax) &&
        ranges_overlap(a->ymin, a->ymax, b->ymin, b->ymax) &&
        ranges_overlap(a->hmin, a->hmax, b->hmin, b->hmax)) {
	return (true);
    }

    return (false);
}

static char
thing_get_space_sep_axis (thingp a, thingp b)
{
    char sepAxis = '\0';;

    if (ranges_do_not_overlap(a->xmin, a->xmax, b->xmin, b->xmax)) {
	sepAxis = 'x';
    } else if (ranges_do_not_overlap(a->ymin, a->ymax, b->ymin, b->ymax)) {
	sepAxis = 'y';
    } else if (ranges_do_not_overlap(a->zmin, a->zmax, b->zmin, b->zmax)) {
	sepAxis = 'z';
    }

    return (sepAxis);
}

/*
 * In an isometric perspective of the two given blocks, determine
 * if they will overlap each other on the screen. If they do, then return
 * the block that will appear in front.
 */
static thingp 
getFrontBlock (thingp a, thingp b) 
{
    thing_get_iso_bounds(a);
    thing_get_iso_bounds(b);

    /*
     * If no isometric separation axis is found, then the two 
     * blocks do not overlap on the screen. This means there 
     * is no "front" block to identify.
     */
    if (!things_iso_overlap(a, b)) {
        return (0);
    }

    thing_get_bounds(a);
    thing_get_bounds(b);

    switch (thing_get_space_sep_axis(a, b)) {
	case 'x': return (a->xmin < b->xmin) ? a : b;
	case 'y': return (a->ymin < b->ymin) ? a : b;
	case 'z': return (a->zmin < b->zmin) ? a : b;
    }

    return (0);
}

static void
things_init_sort (void)
{
    thingp t;

    thing_scratch_count = 0;
    things_todraw_count = 0;

    FOR_ALL_THINGS(t) {
        if (thing_scratch_count >= ARRAY_SIZE(thing_scratch)) {
            ERR("scratch pad overflow");
            return;
        }

	thing_scratch[thing_scratch_count++] = t;

	t->infront_count = 0;
	t->behind_count = 0;
    } FOR_ALL_THINGS_END
}

static void
things_push_infront (thingp t, thingp o)
{
    if (t->infront_count >= ARRAY_SIZE(t->infront)) {
	ERR("overflow thing infront array");
	return;
    }

    t->infront[t->infront_count++] = o;

    // LOG("%f %f is in front of %f %f [%u]\n", o->at.x, o->at.y, t->at.x, t->at.y, (int)t->infront_count);
}

static void
things_push_behind (thingp t, thingp o)
{
    t->behind_count++;
}

static void
things_push_todraw (thingp t)
{
    if (!t) {
        DIE("bad push todraw");
    }

    if (things_todraw_count >= ARRAY_SIZE(things_todraw)) {
	ERR("overflow todraw array");
	return;
    }

    things_todraw[things_todraw_count++] = t;
}

static void
things_make_behind_and_infront_list (void)
{
    size_t i, j;

    for (i = 0; i < thing_scratch_count; i++) {
	for (j = i + 1; j < thing_scratch_count; j++) {
	    thingp a = thing_scratch[i];
	    thingp b = thing_scratch[j];

	    thingp f = getFrontBlock(a, b);
	    if (f) {
		if (f == b) {
		    things_push_behind(a, b);
		    things_push_infront(b, a);
		} else {
		    things_push_behind(b, a);
		    things_push_infront(a, b);
		}
            }
	}
    }
}

void
things_sort (void)
{
    things_init_sort();
    things_make_behind_and_infront_list();

    size_t i, j;

    for (i = 0; i < thing_scratch_count; i++) {
	thingp a = thing_scratch[i];
	if (a->behind_count == 0) {
	    things_push_todraw(a);
	}
    }

    for (i = 0; i < things_todraw_count; i++) {
	thingp a = things_todraw[i];
	for (j = 0; j < a->infront_count; j++) {
	    thingp f = a->infront[j];

	    f->behind_count--;
	    if (!f->behind_count) {
		things_push_todraw(f);
	    }
	}
    }
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


    t->at = p;
    t->is_on_map = true;
    t->moving_start = p;

    Py_RETURN_NONE;
}

void thing_pop_ (thingp t)
{
    verify(t);

    if (unlikely(!t->is_on_map)) {
        return;
    }

    t->is_on_map = false;
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

    thingp t;
    FOR_ALL_THINGS(t) {
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
    } FOR_ALL_THINGS_END
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
