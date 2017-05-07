/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the LICENSE file for license.
 */


#include "main.h"
#include "string_util.h"
#include "tile.h"

#ifdef GORY_DEBUG
FILE *fp = 0;
#endif

thingp map_is_tp_at (levelp level, int32_t x, int32_t y, tpp tp)
{
    widp grid_wid;
    widp w;

    grid_wid = game.wid_grid;
    if (!grid_wid) {
        DIE("no grid wid");
    }

    /*
     * Look for a floor tile where we can place stuff.
     */
    uint8_t z;

    for (z = 0; z < Z_DEPTH; z++) {
        w = wid_grid_find_first(grid_wid, x, y, z);
        while (w) {
            thingp thing_it = wid_get_thing(w);

            if (!thing_it) {
                w = wid_grid_find_next(grid_wid, w, x, y, z);
                continue;
            }

            if (tp == wid_get_thing_template(w)) {
                return (thing_it);
            }

            w = wid_grid_find_next(grid_wid, w, x, y, z);
        }
    }

    return (0);
}

tpp map_is_x_at (levelp level,
                 int32_t x, int32_t y,
                 map_is_at_callback callback)
{
    widp grid_wid;
    widp w;

    grid_wid = game.wid_grid;
    if (!grid_wid) {
        DIE("no grid wid");
    }

    uint8_t z;

    for (z = 0; z < Z_DEPTH; z++) {
        w = wid_grid_find_first(grid_wid, x, y, z);
        while (w) {
            thingp thing_it = wid_get_thing(w);

            if (!thing_it) {
                w = wid_grid_find_next(grid_wid, w, x, y, z);
                continue;
            }

            if ((*callback)(thing_it->tp)) {
                return (thing_it->tp);
            }

            w = wid_grid_find_next(grid_wid, w, x, y, z);
        }
    }

    return (0);
}

static tpp map_is_x_at_z_depth (levelp level,
				int32_t x, int32_t y,
				map_is_at_callback callback,
				uint8_t z)
{
    widp grid_wid;
    widp w;

    grid_wid = game.wid_grid;
    if (!grid_wid) {
        DIE("no grid wid");
    }

	w = wid_grid_find_first(grid_wid, x, y, z);
	while (w) {
		thingp thing_it = wid_get_thing(w);

		if (!thing_it) {
			w = wid_grid_find_next(grid_wid, w, x, y, z);
			continue;
		}

		if ((*callback)(thing_it->tp)) {
			return (thing_it->tp);
		}

		w = wid_grid_find_next(grid_wid, w, x, y, z);
	}

	return (0);
}

uint8_t map_count_x_at (levelp level,
                        int32_t x, int32_t y,
                        map_is_at_callback callback)
{
    tpp tp;
    widp grid_wid;
    widp w;
    uint32_t count;

    count = 0;

    grid_wid = game.wid_grid;
    if (!grid_wid) {
        DIE("no grid wid");
    }

    /*
     * Look for a floor tile where we can place stuff.
     */
    uint8_t z;

    for (z = 0; z < Z_DEPTH; z++) {
        w = wid_grid_find_first(grid_wid, x, y, z);
        while (w) {
            tp = wid_get_thing_template(w);
            if (tp) {
                if ((*callback)(tp)) {
                    count++;
                }
            }

            w = wid_grid_find_next(grid_wid, w, x, y, z);
        }
    }

    return (count);
}

tpp map_is_player_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_player));
}

tpp map_is_monst_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_monst));
}

tpp map_is_door_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_door));
}

tpp map_is_chasm_smoke_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_chasm_smoke));
}

tpp map_is_wall_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_wall));
}

tpp map_is_cwall_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_cwall));
}

tpp map_is_hwall_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_hwall));
}

tpp map_is_water_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_water));
}

tpp map_is_lava_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_lava));
}

tpp map_is_floor_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_floor));
}

tpp map_is_corridor_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_corridor));
}

tpp map_is_dirt_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_dirt_deco, Z_DEPTH_DIRT));
}

tpp map_is_dirt_snow_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_dirt_snow_deco, Z_DEPTH_DIRT));
}

tpp map_is_grass_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_grass_deco, Z_DEPTH_GRASS));
}

tpp map_is_grass_snow_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_grass_snow_deco, Z_DEPTH_GRASS));
}

tpp map_is_carpet_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_carpet_deco, Z_DEPTH_FLOOR2));
}

tpp map_is_gravel_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_gravel_deco, Z_DEPTH_GRAVEL));
}

tpp map_is_gravel_snow_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_gravel_snow_deco, Z_DEPTH_GRAVEL));
}

tpp map_is_sand_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_sand_deco, Z_DEPTH_SAND));
}

tpp map_is_sand_snow_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_sand_snow_deco, Z_DEPTH_SAND));
}

tpp map_is_snow_deco_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_snow_deco, Z_DEPTH_SNOW));
}

tpp map_is_dirt_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_dirt, Z_DEPTH_DIRT));
}

tpp map_is_dirt_snow_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_dirt_snow, Z_DEPTH_DIRT));
}

tpp map_is_grass_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_grass, Z_DEPTH_GRASS));
}

tpp map_is_grass_snow_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_grass_snow, Z_DEPTH_GRASS));
}

tpp map_is_carpet_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_carpet, Z_DEPTH_FLOOR2));
}

tpp map_is_gravel_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_gravel, Z_DEPTH_GRAVEL));
}

tpp map_is_gravel_snow_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_gravel_snow, Z_DEPTH_GRAVEL));
}

tpp map_is_sand_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_sand, Z_DEPTH_SAND));
}

tpp map_is_sand_snow_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_sand_snow, Z_DEPTH_SAND));
}

tpp map_is_snow_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at_z_depth(level, x, y, tp_is_snow, Z_DEPTH_SNOW));
}

tpp map_is_dusty_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_dusty));
}

tpp map_is_focus_at (levelp level, int32_t x, int32_t y)
{
    return (map_is_x_at(level, x, y, tp_is_focus));
}

thingp map_thing_is_x_at (levelp level,
                          int32_t x, int32_t y,
                          map_is_at_callback callback)
{
    tpp tp;
    widp grid_wid;
    widp w;

    grid_wid = game.wid_grid;
    if (!grid_wid) {
        DIE("no grid wid");
    }

    /*
     * Look for a floor tile where we can place stuff.
     */
    uint8_t z;

    for (z = 0; z < Z_DEPTH; z++) {
        w = wid_grid_find_first(grid_wid, x, y, z);
        while (w) {
            thingp thing_it = wid_get_thing(w);

            if (!thing_it) {
                w = wid_grid_find_next(grid_wid, w, x, y, z);
                continue;
            }

            tp = wid_get_thing_template(w);
            if (tp) {
                if ((*callback)(tp)) {
                    return (thing_it);
                }
            }

            w = wid_grid_find_next(grid_wid, w, x, y, z);
        }
    }

    return (0);
}
