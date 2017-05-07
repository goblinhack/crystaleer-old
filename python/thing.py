import traceback
import mm
import tp
import game


class Thing:

    class_version = 2

    def __init__(self, tp_name, level=None, x=None, y=None):

        self.version = self.__class__.class_version
        self.v1_field = 1

        self.level = level
        self.tp_name = tp_name

        level.max_thing_id += 1
        self.thing_id = level.max_thing_id

        if tp_name not in tp.all_tps:
            self.err("Thing template {0} does not exist".format(tp_name))

        self.tp = tp.all_tps[tp_name]

        if x is None:
            self.x = -1
            self.y = -1
        else:
            self.x = x
            self.y = y

        self.depth = 0

        self.on_level = False
        self.tilename = None

        # self.debug("Created thing")

        #
        # Save on the level all things list. We can't save onto the level
        # yet as that depends on the thing co-ords which we do not have
        # until it is pushed.
        #
        if self.thing_id in self.level.all_things:
            self.die("thing ID {0} is already in the level list".format(
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
            self.log("Save player on level {0}".format(self.level))
        return result

    def __setstate__(self, dict):
        self.__dict__ = dict

    def __str__(self):
        return "{0}:{1}".format(self.thing_id, self.tp_name)

    def log(self, msg):
        mm.log("Thing {0}: {1}".format(str(self), msg))

    def con(self, msg):
        mm.con("Thing {0}: {1}".format(str(self), msg))

    def debug(self, msg):
        mm.log("Thing {0}: {1}".format(str(self), msg))

    def err(self, msg):
        mm.con("".join(traceback.format_stack()))
        mm.err("Thing {0}: ERROR: {1}".format(self, msg))

    def die(self, msg):
        mm.con("".join(traceback.format_stack()))
        mm.die("Thing {0}: FATAL ERROR: {1}".format(self, msg))

    def dump(self):
        self.log("@ {0},{1} on level {2}".format(self.x, self.y, self.level))

    def destroy(self, reason="no reason"):

        if hasattr(self.tp, "thing_destroyed"):
            if self.tp.thing_destroyed is not None:
                self.tp.thing_destroyed(self)

        if self.on_level:
            self.pop()

        # self.debug("Destroying thing, {0}".format(reason) + " {")

        if self.thing_id in self.level.all_things:
            del self.level.all_things[self.thing_id]

        mm.thing_destroyed(self, reason)

        # self.debug("} " + "Destroyed thing, {0}".format(reason))
        del self

    def upgrade(self):

        if self.version < 2:
            self.v2_field = 2

#        self.debug("upgraded from ver {0} to {1}".format(
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
            self.push()
            if self.tilename is not None:
                self.set_tilename(self.tilename)

        if self.tp.is_player:
            game.g.player = self

#        self.log("Loaded thing on level {0}".format(self.level))
#        self.con("loaded thing at {0} {1}".format(self.x, self.y))
        if self.depth:
            self.set_depth(self.depth)

    #
    # Move a thing and see it move smoothly on the map
    #
    def move(self, x, y):
        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            return

        self.update_pos(x, y)

        mm.thing_move(self, x, y)

    def update_pos(self, x, y):

        self.x = x
        self.y = y

    #
    # Associate the thing with a given level
    #
    def push(self, x=None, y=None):

        if x is None:
            x = self.x
            y = self.y

        if x >= mm.MAP_WIDTH or y >= mm.MAP_HEIGHT or x < 0 or y < 0:
            self.die("Out of bounds at {0},{1}".format(self.x, self.y))
            return

        self.on_level = True
        # self.debug("pushed")

        self.x = x
        self.y = y

        self.level.thing_push(self.offset_x, self.offset_y, self)

        mm.thing_push(self, x, y)

        if hasattr(self.tp, "thing_pushed"):
            if self.tp.thing_pushed is not None:
                self.tp.thing_pushed(self)

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
