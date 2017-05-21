import traceback
import mm
import thing
import sys
import os
import pickle
import tp
from point import Point


class Level:

    def __init__(self, at, seed):

        self.where = at
        self.seed = seed
        self.max_thing_id = 0

        #
        # All things in the level
        #
        self.all_things = {}

        self.things_at = [[[[] for z in range(mm.MAP_DEPTH)]
                          for y in range(mm.MAP_HEIGHT)]
                          for x in range(mm.MAP_WIDTH)]

    def log(self, msg):
        mm.log("Level {}: {}".format(str(self), msg))

    def con(self, msg):
        mm.con("Level {}: {}".format(str(self), msg))

    def debug(self, msg):
        return
        mm.log("Level {}: {}".format(str(self), msg))

    def err(self, msg):
        mm.con("".join(traceback.format_stack()))
        mm.err("Level {}: ERROR: {}".format(self, msg))

    def die(self, msg):
        mm.con("".join(traceback.format_stack()))
        mm.die("Level {}: FATAL ERROR: {}".format(self, msg))

    def __str__(self):
        return "level.{}".format(str(self.where))

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

        self.debug("upgraded from ver {} to {}".format(
                   self.version, self.__class__.class_version))

        self.version = self.__class__.class_version

    def load(self, cx, cy):
        self.log("Load level at {},{}".format(cx, cy))

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

    def thing_find(self, p, tp_name):

        if p.oob():
            return ""

        for t in self.things_at[p.x][p.y][p.z]:
            if t.tp.name == tp_name:
                return t

        return None

    def thing_find_same_type(self, p, tp_name):

        if p.oob():
            return ""

        f = tp.all_tps[tp_name]
        for t in self.things_at[p.x][p.y][p.z]:
            if tp.same_type(f, t.tp):
                return t

        return None

    def thing_top(self, p):

        if p.oob():
            return ""

        for t in self.things_at[p.x][p.y][p.z]:
            return t

        return None

    def things_at(self, p):

        if p.oob():
            return ""

        r = []
        for t in self.things_at[p.x][p.y][p.z]:
            r.append(t)

        return r

    def tp_is_where(self, value):

        for z in range(mm.MAP_HEIGHT):
            for y in range(mm.MAP_HEIGHT):
                for x in range(mm.MAP_WIDTH):
                    for t in self.things_at[x][y][z]:
                        v = getattr(t.tp, value)
                        if v is not None:
                            if v:
                                return (Point(x, y, z))

        return None

    def things_remove_all_except_player(self):

        for z in range(mm.MAP_HEIGHT):
            for y in range(mm.MAP_HEIGHT):
                for x in range(mm.MAP_WIDTH):
                    for t in self.things_at[x][y][z]:
                        if t.tp.is_player:
                            continue
                        t.destroy("via things remove all")

    def things_flood_fill_(self, p, tp, walked):

        sys.setrecursionlimit(mm.MAP_WIDTH * mm.MAP_HEIGHT)

        if p.oob():
            return ""

        if walked[p.x][p.y] == 1:
            return

        walked[p.x][p.y] = 1

        for f in self.things_at[p.x][p.y]:
            other_tp = f.tp

            if not tp.is_wall:
                if other_tp.is_wall:
                    return

        t = self.thing_find_same_type(p, tp.name)
        if t is not None:
            t.destroy("via flood fill")

        t = thing.Thing(level=self,
                        tp_name=tp.name,
                        x=p.x, y=p.y, z=p.z)
        t.push()

        self.things_flood_fill_(Point(p.x - 1, p.y, p.z), tp, walked)
        self.things_flood_fill_(Point(p.x + 1, p.y, p.z), tp, walked)
        self.things_flood_fill_(Point(p.x, p.y - 1, p.z), tp, walked)
        self.things_flood_fill_(Point(p.x, p.y + 1, p.z), tp, walked)

    def things_flood_fill(self, p, tp):

        if tp is None:
            return

        walked = [[0 for i in range(mm.MAP_HEIGHT)]
                  for j in range(mm.MAP_WIDTH)]

        self.things_flood_fill_(p, tp, walked)

    def is_movement_blocking_at(self, p):

        if p.oob():
            return ""

        for t in self.things_at[p.x][p.y][p.z]:
            if t.tp.is_movement_blocking:
                return True

        return False

    def describe_position(self, p):

        if p.oob():
            return ""

        #
        # If water then hide things like treasure.
        #
        hide_treasure = True
        for t in self.things_at[p.x][p.y][p.z]:
            if t.tp.is_floor:
                hide_treasure = False

        for t in self.things_at[p.x][p.y][p.z]:
            if t.tp.is_treasure:
                if hide_treasure:
                    continue

            if t.tp.long_name is not None:
                return t.tp.long_name.title()

        return ""

    def thing_push(self, p, t):

        #
        # When loading the game again, things think they are on
        # the level already.
        #
        l = self.things_at[p.x][p.y][p.z]
        if t in l:
            return

        l.append(t)

        self.all_things[t.thing_id] = t

    def thing_pop(self, p, t):

        self.things_at[p.x][p.y].remove(t)
        del self.all_things[t.thing_id]
