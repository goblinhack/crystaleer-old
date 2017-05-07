import traceback
import mm
import thing
import sys
import os
import pickle
import tp


class Level:

    def __init__(self, xyz, seed):

        self.where = xyz

        self.seed = seed

        #
        # All things in the level
        #
        self.all_things = {}

        self.things_at = [[[] for x in range(mm.MAP_WIDTH)]
                          for y in range(mm.MAP_HEIGHT)]

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

        #
        # to avoid dictionary changed size during iteration, walk the keys
        #
        for thing_id, t in self.all_things.items():

            #
            # Delete from the parent too
            #
            if thing_id in self.level.all_things:
                del self.level.all_things[thing_id]

            t.destroy()

        self.log("} Destroyed level")
        del self

    def dump(self):

        for i in self.all_things:
            self.all_things[i].dump()

    def save(self, f):

        self.log("Save level")

        with open(os.path.normcase(
                  os.path.join(os.environ["APPDATA"], str(self))), 'wb') as f:
            pickle.dump(self.where, f, pickle.HIGHEST_PROTOCOL)
            pickle.dump(self.all_things, f, pickle.HIGHEST_PROTOCOL)
            pickle.dump(self.seed, f, pickle.HIGHEST_PROTOCOL)

    def upgrade(self):

        if self.version < 2:
            self.v2_field = 2

        self.debug("upgraded from ver {0} to {1}".format(
                   self.version, self.__class__.class_version))

        self.version = self.__class__.class_version

    def load(self, cx, cy):
        self.log("Load level at {0},{1}".format(cx, cy))

        with open(os.path.normcase(
                    os.path.join(os.environ["APPDATA"],
                                 str(self))), 'rb') as f:
            self.where = pickle.load(f)
            self.all_things = pickle.load(f)
            self.seed = pickle.load(f)

        if self.version != self.__class__.class_version:
            self.upgrade()

        self.biome_set_vectors()

        if self.biome_load:
            self.biome_load(self)

        #
        # recreate the widgets for this thing
        #
        for thing_id, t in self.all_things.items():
            t = self.all_things[thing_id]
            t.loaded(self, self.level)

    def thing_find(self, x, y, tp_name):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return None

        for t in self.hunk[x][y]:
            if t.tp.name == tp_name:
                return t

        return None

    def thing_find_same_type(self, x, y, tp_name):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return None

        f = tp.all_tps[tp_name]
        for t in self.things_at[x][y]:
            if tp.same_type(f, t.tp):
                return t

        return None

    def thing_top(self, x, y):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return None

        for t in self.things_at[x][y]:
            return t

        return None

    def things_at(self, x, y):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return []

        r = []
        for t in self.things_at[x][y]:
            r.append(t)

        return r

    def tp_is(self, x, y, value):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return None

        for t in self.things_at[x][y]:
            v = getattr(t.tp, value)
            if v is not None:
                if v:
                    return (x, y)

        return None

    def tp_is_where(self, value):

        for y in range(mm.MAP_HEIGHT):
            for x in range(mm.MAP_WIDTH):
                for t in self.things_at[x][y]:
                    v = getattr(t.tp, value)
                    if v is not None:
                        if v:
                            return (x, y)

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

        for f in self.things_at[x][y]:
            other_tp = f.tp

            if not tp.is_wall:
                if other_tp.is_wall:
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

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return False

        for t in self.things_at[x][y]:
            if t.tp.is_movement_blocking:
                return True

        return False

    def describe_position(self, x, y):

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return ""

        #
        # If water then hide things like treasure.
        #
        hide_treasure = True
        for t in self.things_at[x][y]:
            if t.tp.is_floor:
                hide_treasure = False

        for t in self.things_at[x][y]:
            if t.tp.is_treasure:
                if hide_treasure:
                    continue

            if t.tp.long_name is not None:
                return t.tp.long_name.title()

        return ""

    def thing_push(self, x, y, t):

        #
        # When loading the game again, things think they are on
        # the level already.
        #
        l = self.things_at[x][y]
        if t in l:
            return

        l.append(t)

        self.all_things[t.thing_id] = t

    def thing_pop(self, x, y, t):

        self.things_at[x][y].remove(t)
        del self.all_things[t.thing_id]
