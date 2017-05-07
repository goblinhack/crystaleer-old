/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the LICENSE file for license.
 */


#include "main.h"
#include "thing_tile.h"
#include "tile.h"

/*
 * Using static memory as these things never change once made.
 */
thing_template thing_templates_chunk[TP_MAX];

tree_root *thing_templates;
tree_root *thing_templates_create_order;

static uint8_t tp_init_done;
static void tp_destroy_internal(tpp t);

uint8_t tp_init (void)
{
    tp_init_done = true;

    return (true);
}

void tp_fini (void)
{
    if (tp_init_done) {
        tp_init_done = false;

        tree_destroy(&thing_templates,
            (tree_destroy_func)tp_destroy_internal);

        tree_destroy(&thing_templates_create_order,
            (tree_destroy_func)0);
    }
}

tpp tp_load (int id, const char *name)
{
    tpp t;

    if (tp_find(name)) {
        ERR("thing template name [%s] already used", name);
    }

    if (!thing_templates) {
        thing_templates = tree_alloc(TREE_KEY_STRING, "TREE ROOT: thing");
    }

    if (!thing_templates_create_order) {
        thing_templates_create_order =
                        tree_alloc(TREE_KEY_INTEGER,
                                   "TREE ROOT: thing create order");
    }

    if (id >= TP_MAX - 1) {
        ERR("too many thing templates");
    }

    t = &thing_templates_chunk[id];
    newptr(t, "tpp");

    t->tree.key = dupstr(name, "TREE KEY: thing");

    t->tp_id = id;

    if (!tree_insert_static(thing_templates, &t->tree.node)) {
        ERR("thing template insert name [%s] failed", name);
    }

    static int create_order;

    t->tree2.key = create_order++;
    if (!tree_insert_static(thing_templates_create_order, &t->tree2.node)) {
        ERR("thing template insert create order [%s] failed", name);
    }

    return (t);
}

static void tp_destroy_internal (tpp t)
{
    tree_destroy(&t->tiles, (tree_destroy_func)thing_tile_free);

    if (t->short_name) {
        myfree(t->short_name);
    }

    if (t->raw_name) {
        myfree(t->raw_name);
    }

    oldptr(t);
}

/*
 * Find an existing thing.
 */
tpp tp_find (const char *name)
{
    thing_template target;
    tpp result;

    if (!name) {
        ERR("no name for thing find");
    }

    // memset(&target, 0, sizeof(target)); intentional for speed
    target.tree.key = (char*) name;

    result = (TYPEOF(result)) tree_find(thing_templates, &target.tree.node);
    if (!result) {
        return (0);
    }

    return (result);
}

tpp tp_find_short_name (const char *name)
{
    tree_root *tree;
    tpp t;

    tree = thing_templates;

    TREE_WALK(tree, t) {
        if (!strcasecmp(name, t->short_name)) {
            return (t);
        }
    }

    ERR("did not find short template name \"%s\"", name);

    return (0);
}

/*
 * Given a string name, map to a thing template.
 */
tpp string2thing_template (const char **s)
{
    static char tmp[MAXSTR];
    static const char *eo_tmp = tmp + MAXSTR;
    const char *c = *s;
    char *t = tmp;

    while (t < eo_tmp) {
        if ((*c == '\0') || (*c == '$')) {
            break;
        }

        *t++ = *c++;
    }

    if (c == eo_tmp) {
        return (0);
    }

    *t++ = '\0';
    *s += (t - tmp);

    thing_template find;
    tpp target;

    memset(&find, 0, sizeof(find));
    find.tree.key = tmp;

    target = (TYPEOF(target)) tree_find(thing_templates, &find.tree.node);
    if (!target) {
        ERR("unknown thing [%s]", tmp);
    }

    return (target);
}

const char *tp_name (tpp t)
{
    return (t->tree.key);
}

const char *tp_short_name (tpp t)
{
    return (t->short_name);
}

const char *tp_raw_name (tpp t)
{
    return (t->raw_name);
}

int tp_get_speed (tpp t)
{
    return (t->speed);
}

tree_rootp tp_get_tiles (tpp t)
{
    return (t->tiles);
}

tilep tp_first_tile (tpp tp)
{
    tree_rootp thing_tiles;

    thing_tiles = tp_get_tiles(tp);
    if (!thing_tiles) {
        DIE("tp %s has no tiles", tp_name(tp));
    }

    thing_tilep thing_tile;

    /*
     * Get the first anim tile.
     */
    thing_tile = (TYPEOF(thing_tile)) tree_root_first(thing_tiles);

    if (!thing_tile) {
        DIE("tp %s has no tiles", tp_name(tp));
    }

    return (thing_tile->tile);
}
