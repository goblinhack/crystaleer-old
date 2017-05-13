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

void
thing_set_distance (thingp t)
{
    t->distance = t->at.x + t->at.y + t->at.z;
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

void thing_move_ (thingp t, fpoint3d p)
{
    verify(t);

LOG("TBD thing_move_");
}

static void 
thing_insert (thingp t)
{
    thing_set_distance(t);

    if (!things_display_sorted) {
        things_display_sorted = t;

        t->next = 0;
        t->prev = 0;
    } else {
        thingp n = things_display_sorted;
        thingp l = 0;

        while (n) {
            if (n->distance > t->distance) {
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
}

static void 
thing_move_left (thingp t)
{
    thingp p = t->prev;

    if (!p) {
        return;
    }

    p->next = t->next;
    t->next = p;
    t->prev = p->prev;
    p->prev = t;

    if (p == things_display_sorted) {
        things_display_sorted = t;
    }
}

static void 
thing_move_right (thingp t)
{
    thingp n = t->next;

    if (!n) {
        return;
    }

    n->prev = t->prev;
    t->prev = n;
    t->next = n->next;
    n->next = t;

    if (t == things_display_sorted) {
        things_display_sorted = n;
    }
}

void 
thing_incremental_sort (thingp t)
{
    double distance = t->distance;
    int moved = false;

    for (;;) {
        thingp n = t->next;
        if (!n) {
            break;
        }

        if (n->distance >= distance) {
            break;
        }

        thing_move_right(t);

        moved = true;
    }

    if (moved) {
        return;
    }

    for (;;) {
        thingp p = t->prev;
        if (!p) {
            break;
        }

        if (p->distance <= distance) {
            break;
        }

        thing_move_left(t);
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
    }

    t->at = p;
    thing_insert(t);
    t->is_on_map = true;

    thing_incremental_sort(t);

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

void thing_move_to (thingp t, fpoint3d to)
{
    verify(t);

    if ((t->last_at.x == -1.0) && 
        (t->last_at.y == -1.0) &&
        (t->last_at.z == -1.0)) {
        t->last_at = to;
    } else {
        t->last_at = t->at;
    }

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
