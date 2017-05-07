import traceback
import mm
import thing
import sys


class Level:

    def __init__(self, xyz, seed):

        self.where = xyz

        self.seed = seed

        #
        # All things in the level
        #
        self.all_things = {}

    def log(self, msg):
        mm.log("Level {0}: {1}".format(str(self), msg))

    def con(self, msg):
        mm.con("Level {0}: {1}".format(str(self), msg))

    def debug(self, msg):
        return
        mm.log("Level {0}: {1}".format(str(self), msg))

    def err(self, msg):
        mm.con("".join(traceback.format_stack()))
        mm.err("Level {0}: ERROR: {1}".format(self, msg))

    def die(self, msg):
        mm.con("".join(traceback.format_stack()))
        mm.die("Level {0}: FATAL ERROR: {1}".format(self, msg))

    def __str__(self):
        return "level.{0}".format(str(self.where))

    def destroy(self):
        self.log("Destroying level {")

        for cx in range(0, mm.CHUNK_ACROSS):
            for cy in range(0, mm.CHUNK_DOWN):
                self.chunk[cx][cy].destroy()

        self.log("Destroy cached levels")
        for chunk_name, c in list(self.chunk_cache.items()):
            c.destroy()

        self.log("} Destroyed level")
        del self

    def tick(self):
        return

    def dump(self):

        for cx in range(0, mm.CHUNK_ACROSS):
            for cy in range(0, mm.CHUNK_DOWN):
                self.chunk[cx][cy].dump()

    def save(self, f):

        self.log("Save level")

        for cx in range(0, mm.CHUNK_ACROSS):
            for cy in range(0, mm.CHUNK_DOWN):
                self.chunk[cx][cy].save()

        self.log("Save cached levels")
        for chunk_name, c in self.chunk_cache.items():
            c.save()

    def thing_find(self, x, y, tp_name):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return None

        (chunk, x, y) = self.xy_to_chunk_xy(x, y)
        return chunk.thing_find(x, y, tp_name)

    def thing_find_same_type(self, x, y, tp_name):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return None

        (chunk, x, y) = self.xy_to_chunk_xy(x, y)
        return chunk.thing_find_same_type(x, y, tp_name)

    def thing_top(self, x, y):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return None

        (chunk, x, y) = self.xy_to_chunk_xy(x, y)
        return chunk.thing_top(x, y)

    def things_at(self, x, y):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return []

        (chunk, x, y) = self.xy_to_chunk_xy(x, y)
        return chunk.things_at(x, y)

    def tp_is(self, x, y, value):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return None

        (chunk, x, y) = self.xy_to_chunk_xy(x, y)
        return chunk.tp_is(x, y, value)

    def tp_is_where(self, value):

        for cx in range(0, mm.CHUNK_ACROSS):
            for cy in range(0, mm.CHUNK_DOWN):
                found = self.chunk[cx][cy].tp_is_where(value)
                if found is not None:
                    (x, y) = found
                    return (self.chunk_xy_to_xy(cx, cy, x, y))
        return None

    def things_remove_all_except_player(self):
        for y in range(mm.MAP_HEIGHT):
            for x in range(mm.MAP_WIDTH):
                r = self.things_at(x, y)
                for t in r:
                    if t.tp.is_player:
                        continue

                    t.destroy("via things remove all")

    def things_flood_fill_(self, x, y, tp, walked):

        sys.setrecursionlimit(mm.MAP_WIDTH * mm.MAP_HEIGHT)

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return

        if walked[x][y] == 1:
            return

        walked[x][y] = 1

        (chunk, ox, oy) = self.xy_to_chunk_xy(x, y)

        for f in chunk.things_on_chunk[ox][oy]:
            other_tp = f.tp

            if not tp.is_wall and \
               not tp.is_landrock and \
               not tp.is_landrock_snow and \
               not tp.is_cwall and \
               not tp.is_hwall:
                if other_tp.is_wall or \
                   other_tp.is_landrock or \
                   other_tp.is_landrock_snow or \
                   other_tp.is_hwall or \
                   other_tp.is_cwall:
                    return

            if tp.is_wall or \
               tp.is_door or \
               tp.is_landrock or \
               tp.is_landrock_snow or \
               tp.is_cwall or \
               tp.is_hwall or \
               tp.is_bridge or \
               tp.is_water:
                if other_tp.is_wall or \
                   other_tp.is_door or \
                   other_tp.is_landrock or \
                   other_tp.is_landrock_snow or \
                   other_tp.is_cwall or \
                   other_tp.is_hwall or \
                   other_tp.is_bridge or \
                   other_tp.is_water:
                    return

            if tp.is_door:
                if other_tp.is_wall or \
                   other_tp.is_cwall or \
                   other_tp.is_hwall or \
                   other_tp.is_water:
                    return

            if tp.is_dirt or \
               tp.is_grass or \
               tp.is_lawn or \
               tp.is_snow or \
               tp.is_ice or \
               tp.is_gravel or \
               tp.is_gravel_snow or \
               tp.is_snow or \
               tp.is_sand or \
               tp.is_water:
                if other_tp.is_dirt or \
                   other_tp.is_grass or \
                   other_tp.is_lawn or \
                   other_tp.is_snow or \
                   other_tp.is_ice or \
                   other_tp.is_gravel or \
                   other_tp.is_gravel_snow or \
                   other_tp.is_snow or \
                   other_tp.is_sand or \
                   other_tp.is_water:
                    return

        t = self.thing_find_same_type(x, y, tp.name)
        if t is not None:
            t.destroy("via flood fill")

        t = thing.Thing(level=self,
                        tp_name=tp.name,
                        x=x, y=y)
        t.push()

        self.things_flood_fill_(x - 1, y, tp, walked)
        self.things_flood_fill_(x + 1, y, tp, walked)
        self.things_flood_fill_(x, y - 1, tp, walked)
        self.things_flood_fill_(x, y + 1, tp, walked)

    def things_flood_fill(self, x, y, tp):

        if tp is None:
            return

        walked = [[0 for i in range(mm.MAP_HEIGHT)]
                  for j in range(mm.MAP_WIDTH)]

        self.things_flood_fill_(x, y, tp, walked)

    def is_movement_blocking_at(self, x, y):

        (chunk, ox, oy) = self.xy_to_chunk_xy(x, y)
        if chunk.is_movement_blocking_at(ox, oy):
            return True
        return False

    def describe_position(self, x, y):

        (chunk, ox, oy) = self.xy_to_chunk_xy(x, y)
        return chunk.describe_position(ox, oy)
