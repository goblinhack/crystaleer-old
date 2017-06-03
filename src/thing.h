/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the LICENSE file.
 */

#pragma once

#include "tree.h"
#include "config.h"
#include "thing_template.h"
#include <stdlib.h>
#include <math.h>
#include <Python.h>

uint8_t thing_init(void);
void thing_fini(void);
thingp thing_new(const char *name, long int thing_id, const char *tp_name);

/*
 * Python funcs
 */
void thing_destroyed_(thingp t, const char *reason);
void thing_set_tilename_(thingp t, const char *tilename);
void thing_set_tp_(thingp t, const char *tp);
void thing_move_delta_(thingp t, fpoint3d);
PyObject *thing_push_(thingp t, fpoint3d);
PyObject *thing_collision_check_(thingp t, fpoint3d);
void thing_pop_(thingp t);

/*
 * thing animate
 */
void thing_animate_all(levelp level);
void thing_animate(thingp);
tree_rootp thing_tile_tiles(thingp);

thingp thing_find(const char *name);
void thing_dead(thingp, thingp killer,
                const char *fmt, ...) __attribute__ ((format (printf, 3, 4)));
const char *thing_logname(thingp);

typedef uint8_t (*thing_is_fn)(thingp t);

void thing_set_dir_none(thingp t);
uint8_t thing_is_dir_none(thingp t);
void thing_set_dir_down(thingp t);
uint8_t thing_is_dir_down(thingp t);
void thing_set_dir_up(thingp t);
uint8_t thing_is_dir_up(thingp t);
void thing_set_dir_left(thingp t);
uint8_t thing_is_dir_left(thingp t);
void thing_set_dir_right(thingp t);
uint8_t thing_is_dir_right(thingp t);

void thing_set_dir_tl(thingp t);
uint8_t thing_is_dir_tl(thingp t);
void thing_set_dir_bl(thingp t);
uint8_t thing_is_dir_bl(thingp t);
void thing_set_dir_tr(thingp t);
uint8_t thing_is_dir_tr(thingp t);
void thing_set_dir_br(thingp t);
uint8_t thing_is_dir_br(thingp t);

void thing_set_is_dead(thingp t, uint8_t val);
const char *thing_name(thingp);
const char *thing_short_name(thingp);
tree_rootp thing_tiles(thingp);
thing_tilep thing_current_tile(thingp t);

enum {
    THING_DIR_NONE,
    THING_DIR_DOWN,
    THING_DIR_UP,
    THING_DIR_LEFT,
    THING_DIR_RIGHT,
    THING_DIR_TL,
    THING_DIR_BL,
    THING_DIR_TR,
    THING_DIR_BR,
};

typedef struct {
    double x, y, z, h, v;
} ipoint;

#define MAX_THINGS_SCRATCH (MAP_WIDTH * MAP_HEIGHT * MAP_DEPTH * 4)
#define MAX_THINGS_INFRONT MAX_THINGS_SCRATCH

typedef struct thing_ {

    tree_key_string tree;

    struct thing_ *infront[MAX_THINGS_INFRONT];
    size_t infront_count;
    size_t behind_count;

    /*
     * Grid coordinates.
     */
    fpoint3d at;

    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    double hmin;
    double hmax;
    double height;

    ipoint rightDown;
    ipoint leftDown;
    ipoint backDown;
    ipoint frontDown;
    ipoint rightUp;
    ipoint leftUp;
    ipoint backUp;
    ipoint frontUp;

    /*
     * Fake gravity
     */
    double fall_speed;
    double jump_speed;
    fpoint3d momentum;
    uint32_t last_move;

    /*
     * Allocated in python
     */
    long int thing_id;

    /*
     * Pointer to common settings for this thing.
     */
    tpp tp;

    /*
     * Last anim frame position. To be able to detect moving things.
     */
    fpoint3d last_anim_at;

    /*
     * Previous hop where we were. We use this to interpolate the real
     * position when moving.
     */
    fpoint3d last_at;

    tilep tile;

    /*
     * First tile found in an animation.
     */
    tilep first_tile;

    uint32_t timestamp_change_to_next_frame;

    /*
     * For animation.
     */
    int16_t current_tile;

    uint8_t dir;

    /*
     * Only used for display purposes.
     */
    int score;
    int gold;
    int hp;

    /*
     * Debugging this thing?
     */
    uint32_t debug:1;
    uint32_t is_on_map:1;
    uint32_t is_dead:1;
    uint32_t is_sleeping:1;
    uint32_t is_moving:1;
    uint32_t has_ever_moved:1;
    uint32_t is_open:1;

    /*
     * Collisions
     */
    uint32_t is_over_ladder:1;
    uint32_t is_over_solid_ground:1;
} thing;

#include "thing_template.h"

static inline int thing_is_dead (thingp t)
{
    return (t->is_dead);
}

static inline int thing_is_moving (thingp t)
{
    return (t->is_moving);
}

static inline int thing_is_open (thingp t)
{
    return (t->is_open);
}

static inline int thing_is_sleeping (thingp t)
{
    return (t->is_sleeping);
}

static inline tpp thing_tp (thingp t)
{
    verify(t);

    return (t->tp);
}

static inline uint8_t thing_is_monst (thingp t)
{
    verify(t);

    return (tp_is_monst(thing_tp(t)));
}

static inline uint8_t thing_is_door (thingp t)
{
    verify(t);

    return (tp_is_door(thing_tp(t)));
}

static inline uint8_t thing_is_wall (thingp t)
{
    verify(t);

    return (tp_is_wall(thing_tp(t)));
}

static inline uint8_t thing_is_player (thingp t)
{
    verify(t);

    return (tp_is_player(thing_tp(t)));
}

static inline uint8_t thing_is_ladder (thingp t)
{
    verify(t);

    return (tp_is_ladder(thing_tp(t)));
}

static inline uint8_t thing_is_solid_ground (thingp t)
{
    verify(t);

    return (tp_is_solid_ground(thing_tp(t)));
}

static inline uint8_t thing_is_movable (thingp t)
{
    verify(t);

    return (tp_is_movable(thing_tp(t)));
}

/*
 * thing.c
 */
extern void thing_incremental_sort(thingp t);
extern void thing_move_to(thingp t, fpoint3d at);
extern void thing_move_all(void);

extern tree_root *things;

/*
 * thing_dir.c
 */
void thing_dir(thingp t, double *dx, double *dy);
int thing_angle_to_dir(double dx, double dy);

#define FOR_ALL_THINGS(t) \
    { \
        TREE_WALK_UNSAFE(things, t) { \
        verify(t);

#define FOR_ALL_THINGS_END } }

/*
 * thing_iso.h
 */
extern thingp things_draw_list[MAX_THINGS_SCRATCH];
extern size_t things_draw_list_count;

extern int things_iso_intersect(thingp a, thingp b);
extern int things_iso_collision_check(thingp a, thingp b, fpoint3d at);
extern void things_iso_sort(void);
extern int things_iso_overlap(thingp a, thingp b);

/*
 * thing_move.c
 */
extern int thing_fall(thingp);
extern int thing_slide(thingp);
extern int thing_jump(thingp);
