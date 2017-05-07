/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the LICENSE file.
 */

#pragma once

#include "tree.h"

typedef struct tp_ {
    tree_key_string tree;
    tree_key_int tree2;

    int tp_id;

    /*
     * Internal description of the thing.
     */
    char *short_name;

    /*
     * Base filename
     */
    char *raw_name;

    /*
     * Animation tiles.
     */
    tree_rootp tiles;

    /*
     * Speed in milliseconds it takes to move one tile.
     */
    int speed;

    int blit_top_off;
    int blit_bot_off;
    int blit_left_off;
    int blit_right_off;

    uint8_t has_dir_anim:1;
    uint8_t is_animated:1;
    uint8_t is_animated_lr_flip:1;
    uint8_t is_animated_no_dir:1;
    uint8_t is_animation:1;
    uint8_t is_door:1;
    uint8_t is_floor:1;
    uint8_t is_key:1;
    uint8_t is_lava:1;
    uint8_t is_monst:1;
    uint8_t is_movement_blocking:1;
    uint8_t is_player:1;
    uint8_t is_solid_ground:1;
    uint8_t is_wall:1;
    uint8_t is_water:1;
    uint8_t is_rrr1:1;
    uint8_t is_rrr2:1;
    uint8_t is_rrr3:1;
    uint8_t is_rrr4:1;
    uint8_t is_rrr5:1;
    uint8_t is_rrr6:1;
    uint8_t is_rrr7:1;
    uint8_t is_rrr8:1;
    uint8_t is_rrr9:1;
    uint8_t is_rrr10:1;
    uint8_t is_rrr11:1;
    uint8_t is_rrr12:1;
    uint8_t is_rrr13:1;
    uint8_t is_rrr14:1;
    uint8_t is_rrr15:1;
    uint8_t is_rrr16:1;
    uint8_t is_rrr17:1;
    uint8_t is_rrr18:1;
    uint8_t is_rrr19:1;
    uint8_t is_rrr20:1;
    uint8_t is_rrr21:1;
    uint8_t is_rrr22:1;
    uint8_t is_rrr23:1;
    uint8_t is_rrr24:1;
    uint8_t is_rrr25:1;
    uint8_t is_rrr26:1;
    uint8_t is_rrr27:1;
    uint8_t is_rrr28:1;
    uint8_t is_rrr29:1;
    uint8_t is_rrr30:1;
    uint8_t is_rrr31:1;
    uint8_t is_rrr32:1;
    uint8_t is_rrr33:1;
    uint8_t is_rrr34:1;
    uint8_t is_rrr35:1;
    uint8_t is_rrr36:1;
    uint8_t is_rrr37:1;
    uint8_t is_rrr38:1;
    uint8_t is_rrr39:1;
    uint8_t is_rrr40:1;
    uint8_t is_rrr41:1;
    uint8_t is_rrr42:1;
    uint8_t is_rrr43:1;
    uint8_t is_rrr44:1;
    uint8_t is_rrr45:1;
    uint8_t is_rrr46:1;

} thing_template;

uint8_t tp_init(void);
void tp_fini(void);
tpp tp_load(int id, const char *file);
tpp tp_find(const char *name);
tpp tp_find_short_name(const char *name);
uint8_t thing_test(int argc, char *argv[]);
tpp string2thing_template(const char **s);

const char *tp_name(tpp);
const char *tp_short_name(tpp);
const char *tp_raw_name(tpp);

int tp_get_speed(tpp);

static inline int32_t tp_get_blit_top_off (tpp t)
{
    return (t->blit_top_off);
}

static inline int32_t tp_get_blit_bot_off (tpp t)
{
    return (t->blit_bot_off);
}

static inline int32_t tp_get_blit_left_off (tpp t)
{
    return (t->blit_left_off);
}

static inline int32_t tp_get_blit_right_off (tpp t)
{
    return (t->blit_right_off);
}

tree_rootp tp_get_tiles(tpp);

extern tree_rootp thing_templates;
extern tree_rootp thing_templates_create_order;

static inline int tp_to_id (tpp t)
{
    return (t->tp_id);
}

extern inline tpp id_to_tp(int id);

static inline uint8_t tp_is_floor (tpp t)
{
    return (t->is_floor);
}

static inline uint8_t tp_is_wall (tpp t)
{
    return (t->is_wall);
}

static inline uint8_t tp_is_door (tpp t)
{
    return (t->is_door);
}

static inline uint8_t tp_is_lava (tpp t)
{
    return (t->is_lava);
}

static inline uint8_t tp_is_water (tpp t)
{
    return (t->is_water);
}

static inline uint8_t tp_is_monst (tpp t)
{
    return (t->is_monst);
}

static inline uint8_t tp_is_player (tpp t)
{
    return (t->is_player);
}

static inline uint8_t tp_is_animation (tpp t)
{
    return (t->is_animation);
}

static inline uint8_t tp_is_animated (tpp t)
{
    return (t->is_animated);
}

static inline uint8_t tp_is_animated_no_dir (tpp t)
{
    return (t->is_animated_no_dir);
}

static inline uint8_t tp_is_rrr1 (tpp t)
{
    return (t->is_rrr1);
}

static inline uint8_t tp_is_rrr2 (tpp t)
{
    return (t->is_rrr2);
}

static inline uint8_t tp_is_rrr3 (tpp t)
{
    return (t->is_rrr3);
}

static inline uint8_t tp_is_rrr4 (tpp t)
{
    return (t->is_rrr4);
}

static inline uint8_t tp_is_rrr5 (tpp t)
{
    return (t->is_rrr5);
}

static inline uint8_t tp_is_rrr6 (tpp t)
{
    return (t->is_rrr6);
}

static inline uint8_t tp_is_rrr7 (tpp t)
{
    return (t->is_rrr7);
}

static inline uint8_t tp_is_rrr8 (tpp t)
{
    return (t->is_rrr8);
}

static inline uint8_t tp_is_rrr9 (tpp t)
{
    return (t->is_rrr9);
}

static inline uint8_t tp_is_rrr10 (tpp t)
{
    return (t->is_rrr10);
}

static inline uint8_t tp_is_rrr11 (tpp t)
{
    return (t->is_rrr11);
}

static inline uint8_t tp_is_rrr12 (tpp t)
{
    return (t->is_rrr12);
}

static inline uint8_t tp_is_rrr13 (tpp t)
{
    return (t->is_rrr13);
}

static inline uint8_t tp_is_rrr14 (tpp t)
{
    return (t->is_rrr14);
}

static inline uint8_t tp_is_rrr15 (tpp t)
{
    return (t->is_rrr15);
}

static inline uint8_t tp_is_rrr16 (tpp t)
{
    return (t->is_rrr16);
}

static inline uint8_t tp_is_rrr17 (tpp t)
{
    return (t->is_rrr17);
}

static inline uint8_t tp_is_rrr18 (tpp t)
{
    return (t->is_rrr18);
}

static inline uint8_t tp_is_rrr19 (tpp t)
{
    return (t->is_rrr19);
}

static inline uint8_t tp_is_rrr20 (tpp t)
{
    return (t->is_rrr20);
}

static inline uint8_t tp_is_rrr21 (tpp t)
{
    return (t->is_rrr21);
}

static inline uint8_t tp_is_rrr22 (tpp t)
{
    return (t->is_rrr22);
}

static inline uint8_t tp_is_rrr23 (tpp t)
{
    return (t->is_rrr23);
}

static inline uint8_t tp_is_rrr24 (tpp t)
{
    return (t->is_rrr24);
}

static inline uint8_t tp_is_rrr25 (tpp t)
{
    return (t->is_rrr25);
}

static inline uint8_t tp_is_rrr26 (tpp t)
{
    return (t->is_rrr26);
}

static inline uint8_t tp_is_rrr27 (tpp t)
{
    return (t->is_rrr27);
}

static inline uint8_t tp_is_rrr28 (tpp t)
{
    return (t->is_rrr28);
}

static inline uint8_t tp_is_rrr29 (tpp t)
{
    return (t->is_rrr29);
}

static inline uint8_t tp_is_rrr30 (tpp t)
{
    return (t->is_rrr30);
}

static inline uint8_t tp_is_rrr31 (tpp t)
{
    return (t->is_rrr31);
}

static inline uint8_t tp_is_rrr32 (tpp t)
{
    return (t->is_rrr32);
}

static inline uint8_t tp_is_rrr33 (tpp t)
{
    return (t->is_rrr33);
}

static inline uint8_t tp_is_rrr34 (tpp t)
{
    return (t->is_rrr34);
}

static inline uint8_t tp_is_rrr35 (tpp t)
{
    return (t->is_rrr35);
}

static inline uint8_t tp_is_rrr36 (tpp t)
{
    return (t->is_rrr36);
}

static inline uint8_t tp_is_rrr37 (tpp t)
{
    return (t->is_rrr37);
}

static inline uint8_t tp_is_rrr38 (tpp t)
{
    return (t->is_rrr38);
}

static inline uint8_t tp_is_rrr39 (tpp t)
{
    return (t->is_rrr39);
}

static inline uint8_t tp_is_rrr40 (tpp t)
{
    return (t->is_rrr40);
}

static inline uint8_t tp_is_rrr41 (tpp t)
{
    return (t->is_rrr41);
}

static inline uint8_t tp_is_rrr42 (tpp t)
{
    return (t->is_rrr42);
}

static inline uint8_t tp_is_rrr43 (tpp t)
{
    return (t->is_rrr43);
}

static inline uint8_t tp_is_rrr44 (tpp t)
{
    return (t->is_rrr44);
}

static inline uint8_t tp_is_rrr45 (tpp t)
{
    return (t->is_rrr45);
}

static inline uint8_t tp_is_rrr46 (tpp t)
{
    return (t->is_rrr46);
}

static inline uint8_t tp_is_animated_lr_flip (tpp t)
{
    return (t->is_animated_lr_flip);
}

static inline uint8_t tp_is_solid_ground (tpp t)
{
    return (t->is_solid_ground);
}

static inline uint8_t tp_is_movement_blocking (tpp t)
{
    return (t->is_movement_blocking);
}

static inline uint8_t tp_is_key (tpp t)
{
    return (t->is_key);
}

tilep tp_first_tile(tpp tp);
