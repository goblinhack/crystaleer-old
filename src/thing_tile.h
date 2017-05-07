/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the LICENSE file.
 */

typedef struct thing_tile_ {
    tree_key_int tree;
    int array_index;

    char *tilename;
    tilep tile;

    /*
     * Delay in ms between frames.
     */
    uint32_t delay_ms;

    /*
     * Move now at x speed
     */
    uint32_t move;

    uint8_t is_moving:1;
    uint8_t dir:4;
    uint8_t is_yyy5:1;
    uint8_t is_yyy6:1;
    uint8_t is_yyy7:1;
    uint8_t is_yyy8:1;
    uint8_t is_yyy9:1;
    uint8_t is_yyy10:1;
    uint8_t is_yyy11:1;
    uint8_t is_yyy12:1;
    uint8_t is_yyy13:1;
    uint8_t is_yyy14:1;
    uint8_t is_yyy15:1;
    uint8_t is_sleeping:1;
    uint8_t is_open:1;
    uint8_t is_dead:1;
    uint8_t is_end_of_anim:1;
    uint8_t is_dead_on_end_of_anim:1;
    uint8_t has_dir_anim:1;
} thing_tile;

void thing_tile_free(thing_tilep);
const char *thing_tile_name(thing_tilep);
uint32_t thing_tile_delay_ms(thing_tilep);
uint32_t thing_tile_move(thing_tilep);
uint8_t thing_tile_is_moving(thing_tilep);
uint8_t thing_tile_is_dir_up(thing_tilep);
uint8_t thing_tile_is_dir_down(thing_tilep);
uint8_t thing_tile_is_dir_left(thing_tilep);
uint8_t thing_tile_is_dir_right(thing_tilep);
uint8_t thing_tile_is_dir_none(thing_tilep);
uint8_t thing_tile_is_dir_tl(thing_tilep);
uint8_t thing_tile_is_dir_bl(thing_tilep);
uint8_t thing_tile_is_dir_tr(thing_tilep);
uint8_t thing_tile_is_dir_br(thing_tilep);
uint8_t thing_tile_is_yyy5(thing_tilep);
uint8_t thing_tile_is_yyy6(thing_tilep);
uint8_t thing_tile_is_yyy7(thing_tilep);
uint8_t thing_tile_is_yyy8(thing_tilep);
uint8_t thing_tile_is_yyy9(thing_tilep);
uint8_t thing_tile_is_yyy10(thing_tilep);
uint8_t thing_tile_is_yyy11(thing_tilep);
uint8_t thing_tile_is_yyy12(thing_tilep);
uint8_t thing_tile_is_yyy13(thing_tilep);
uint8_t thing_tile_is_yyy14(thing_tilep);
uint8_t thing_tile_is_yyy15(thing_tilep);
uint8_t thing_tile_is_sleeping(thing_tilep);
uint8_t thing_tile_is_open(thing_tilep);
uint8_t thing_tile_is_dead(thing_tilep);
uint8_t thing_tile_is_end_of_anim(thing_tilep);
uint8_t thing_tile_is_dead_on_end_of_anim(thing_tilep);
thing_tilep thing_tile_get_xy(tree_root *root, int x, int y);
thing_tilep thing_tile_first(tree_rootp root);
thing_tilep thing_tile_next(tree_rootp root, thing_tilep in);
thing_tilep thing_tile_random(tree_rootp root);

extern thing_tile thing_tile_arr[TILES_ANIM_MAX];
extern int thing_tile_count;
