import traceback
import mm
import tp
import game
from point import Point


class Thing:

    class_version = 2

    def __init__(self, tp_name, level=None, at=None):

        self.version = self.__class__.class_version
        self.v1_field = 1

        self.level = level
        self.tp_name = tp_name
        self.is_moving = False
        self.nexthops = []

        level.max_thing_id += 1
        self.thing_id = level.max_thing_id

        if tp_name not in tp.all_tps:
            self.err("Thing template {} does not exist".format(tp_name))

        self.tp = tp.all_tps[tp_name]

        if at is None:
            self.at = Point(-1, -1, -1)
        else:
            self.at = at

        self.on_level = False
        self.tilename = None

        # self.debug("Created thing")

        #
        # Save on the level all things list. We can't save onto the level
        # yet as that depends on the thing co-ords which we do not have
        # until it is pushed.
        #
        if self.thing_id in self.level.all_things:
            self.die("thing ID {} is already in the level list".format(
                     self.thing_id))
            return

        level.all_things[self.thing_id] = self
        self.level.all_things[self.thing_id] = self

        self.name = str(self)
        mm.thing_new(self, self.thing_id, tp_name)

        if self.tp.thing_init is not None:
            self.tp.thing_init(self)

    def __getstate__(self):
        result = self.__dict__.copy()

        if "tp" not in result:
            self.dump()
            self.die("Trying to save thing that has no template")
        del result['tp']

        if "level" not in result:
            self.dump()
            self.die("Trying to save thing that has no level")
        del result['level']

        if self.tp.is_player:
            self.log("Save player on level {}".format(self.level))
        return result

    def __setstate__(self, dict):
        self.__dict__ = dict

    def __str__(self):
        return "{}:{} at {}".format(self.thing_id, self.tp_name, str(self.at))

    def log(self, msg):
        mm.log("Thing {}: {}".format(str(self), msg))

    def con(self, msg):
        mm.con("Thing {}: {}".format(str(self), msg))

    def debug(self, msg):
        mm.log("Thing {}: {}".format(str(self), msg))

    def err(self, msg):
        mm.con("".join(traceback.format_stack()))
        mm.err("Thing {}: ERROR: {}".format(self, msg))

    def die(self, msg):
        mm.con("".join(traceback.format_stack()))
        mm.die("Thing {}: FATAL ERROR: {}".format(self, msg))

    def dump(self):
        self.log("@ {},{} on level {}".format(str(self.at), self.level))

    def destroy(self, reason="no reason"):

        if hasattr(self.tp, "thing_destroyed"):
            if self.tp.thing_destroyed is not None:
                self.tp.thing_destroyed(self)

        if self.on_level:
            self.atop()

        # self.debug("Destroying thing, {}".format(reason) + " {")

        if self.thing_id in self.level.all_things:
            del self.level.all_things[self.thing_id]

        mm.thing_destroyed(self, reason)

        # self.debug("} " + "Destroyed thing, {}".format(reason))
        del self

    def upgrade(self):

        if self.version < 2:
            self.v2_field = 2

#        self.debug("upgraded from ver {} to {}".format(
#                   self.version, self.__class__.class_version))

        self.version = self.__class__.class_version

    #
    # Loaded from save file into a level
    #
    def loaded(self, level):

        if self.version != self.__class__.class_version:
            self.upgrade()

        level.all_things[self.thing_id] = self

        self.level = level
        self.level = level.level
        self.tp = tp.all_tps[self.tp_name]

        mm.thing_new(self, self.thing_id, self.tp_name)

        if self.on_level:
            self.on_level = False
            self.atush()
            if self.tilename is not None:
                self.set_tilename(self.tilename)

        if self.tp.is_player:
            game.g.player = self

#        self.log("Loaded thing on level {}".format(self.level))
#        self.con("loaded thing at {} {}".format(self.x, self.y))
        if self.depth:
            self.set_depth(self.depth)

    #
    # Move a thing and see it move smoothly on the map
    #
    def move(self, to):
        if to.oob():
            self.nexthops = []
            return

        if self.collision_check(to):
            self.nexthops = []
            return

        if self == game.g.player:
            if self.is_moving:
                if len(self.nexthops) > 1:
                    return

                delta = to.sub(self.at)
                self.nexthops.append(delta)
                return

        self.update_pos(to)
        mm.thing_move(self, to)

    def update_pos(self, at):

        self.at = at

    #
    # Associate the thing with a given level
    #
    def push(self, at=None):

        if at is None:
            at = self.at

        if at.oob():
            self.die("Out of bounds at {}".format(self.at))
            return

        self.on_level = True
        # self.debug("pushed")

        self.at = at

        self.level.thing_push(self.at, self)

        collision_thing_id = mm.thing_push(self)
        if collision_thing_id:
            self.die("Thing collision at {}".format(self.at))
            return

        if hasattr(self.tp, "thing_pushed"):
            if self.tp.thing_pushed is not None:
                self.tp.thing_pushed(self)

    #
    # Check if the new position would collide and return what with
    #
    def collision_check(self, at=None):

        if at is None:
            at = self.at

        if at.oob():
            return False

        old = self.at
        self.at = at
        collision_thing_id = mm.thing_collision_check(self)
        self.at = old

        if collision_thing_id:
            return self.level.all_things[collision_thing_id]

        return None

    #
    # De_associate the thing with its level
    #
    def pop(self):
        if not self.on_level:
            self.err("Is not on the map")
            return
        self.on_level = False
        # self.debug("pop")

        self.level.thing_pop(self.offset_x, self.offset_y, self)

        mm.thing_pop(self)

    def set_long_name(self, value=""):
        self.long_name = value

    def set_tilename(self, name):
        self.tilename = name
        mm.thing_set_tilename(self, name)

    def set_tp(self, tp_name):
        self.tp = tp.all_tps[tp_name]
        mm.thing_set_tp(self, tp_name)
