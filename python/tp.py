import mm
import random

all_tps = {}
all_treasure_tps = []
all_floor_tps = []
all_wall_tps = []
all_door_tps = []
all_dungeon_tps = []


class Tp:
    """thing template"""

    def __init__(self,
                 name,
                 is_treasure=False,
                 is_floor=False,
                 is_wall=False,
                 is_door=False,
                 is_dungeon=False,
                 d1000_appearing_roll=0):

        self.name = name
        self.d1000_appearing_roll = d1000_appearing_roll

        self.is_treasure = is_treasure
        if is_treasure:
            all_treasure_tps.append(name)

        self.is_wall = is_wall
        if is_wall:
            all_wall_tps.append(name)

        self.is_door = is_door
        if is_door:
            all_door_tps.append(name)

        self.is_floor = is_floor
        if is_floor:
            all_floor_tps.append(name)

        self.is_dungeon = is_dungeon
        if is_dungeon:
            all_dungeon_tps.append(name)

        #
        # Load it into the game engine
        #
        mm.tp_load(self)
        self.set_raw_name(name)

        all_tps[name] = self
        self.long_name = None
        self.short_name = None
        self.raw_name = None
        self.speed = None
        self.blit_top_off = None
        self.blit_bot_off = None
        self.blit_left_off = None
        self.blit_right_off = None
        self.is_animated = False
        self.is_animated_no_dir = False
        self.is_animation = False
        self.is_monst = False
        self.is_player = False
        self.is_rrr1 = False
        self.is_rrr2 = False
        self.is_rrr3 = False
        self.is_rrr4 = False
        self.is_rrr5 = False
        self.is_rrr6 = False
        self.is_rrr7 = False
        self.is_rrr8 = False
        self.is_rrr9 = False
        self.is_rrr10 = False
        self.is_rrr11 = False
        self.is_rrr12 = False
        self.is_rrr13 = False
        self.is_rrr14 = False
        self.is_rrr15 = False
        self.is_rrr16 = False
        self.is_rrr17 = False
        self.is_rrr18 = False
        self.is_rrr19 = False
        self.is_rrr20 = False
        self.is_rrr21 = False
        self.is_rrr22 = False
        self.is_rrr23 = False
        self.is_rrr24 = False
        self.is_rrr25 = False
        self.is_rrr26 = False
        self.is_rrr27 = False
        self.is_rrr28 = False
        self.is_rrr29 = False
        self.is_rrr30 = False
        self.is_rrr31 = False
        self.is_rrr32 = False
        self.is_rrr33 = False
        self.is_rrr34 = False
        self.is_rrr35 = False
        self.is_rrr36 = False
        self.is_rrr37 = False
        self.is_rrr38 = False
        self.is_rrr39 = False
        self.is_rrr40 = False
        self.is_rrr41 = False
        self.is_rrr42 = False
        self.is_rrr43 = False
        self.is_rrr44 = False
        self.is_rrr45 = False
        self.is_rrr46 = False
        self.is_animated_lr_flip = False
        self.is_solid_ground = False
        self.is_movement_blocking = False
        self.is_key = False
        self.is_wall = False
        self.is_floor = False
        self.is_lava = False
        self.is_water = False
        self.tiles = []
        self.damage = None
        self.is_double_handed = False

    def set_tile(self, tile=None, **kw):
        self.tiles.append(tile)
        mm.tp_set_tile(self, tile, **kw)

    def set_long_name(self, value):
        self.long_name = value

    def set_damage(self, value):
        self.damage = value

    def set_is_double_handed(self, value):
        self.is_double_handed = value

    def set_raw_name(self, value):
        self.raw_name = value
        mm.tp_set_raw_name(self, value)

    def set_short_name(self, value):
        self.short_name = value
        mm.tp_set_short_name(self, value)

    def set_ms_to_move_one_tile(self, value):
        self.speed = value
        mm.tp_set_ms_to_move_one_tile(self, value)

    def set_blit_top_off(self, value):
        self.blit_top_off = value
        mm.tp_set_blit_top_off(self, value)

    def set_blit_bot_off(self, value):
        self.blit_bot_off = value
        mm.tp_set_blit_bot_off(self, value)

    def set_blit_left_off(self, value):
        self.blit_left_off = value
        mm.tp_set_blit_left_off(self, value)

    def set_blit_right_off(self, value):
        self.blit_right_off = value
        mm.tp_set_blit_right_off(self, value)

    def set_is_animated(self, value):
        self.is_animated = value
        mm.tp_set_is_animated(self, value)

    def set_is_animated_no_dir(self, value):
        self.is_animated_no_dir = value
        mm.tp_set_is_animated_no_dir(self, value)

    def set_is_animation(self, value):
        self.is_animation = value
        mm.tp_set_is_animation(self, value)

    def set_is_monst(self, value):
        self.is_monst = value
        mm.tp_set_is_monst(self, value)

    def set_is_player(self, value):
        self.is_player = value
        mm.tp_set_is_player(self, value)

    def set_is_rrr1(self, value):
        self.is_rrr1 = value
        mm.tp_set_is_rrr1(self, value)

    def set_is_rrr2(self, value):
        self.is_rrr2 = value
        mm.tp_set_is_rrr2(self, value)

    def set_is_rrr3(self, value):
        self.is_rrr3 = value
        mm.tp_set_is_rrr3(self, value)

    def set_is_rrr4(self, value):
        self.is_rrr4 = value
        mm.tp_set_is_rrr4(self, value)

    def set_is_rrr5(self, value):
        self.is_rrr5 = value
        mm.tp_set_is_rrr5(self, value)

    def set_is_rrr6(self, value):
        self.is_rrr6 = value
        mm.tp_set_is_rrr6(self, value)

    def set_is_rrr7(self, value):
        self.is_rrr7 = value
        mm.tp_set_is_rrr7(self, value)

    def set_is_rrr8(self, value):
        self.is_rrr8 = value
        mm.tp_set_is_rrr8(self, value)

    def set_is_rrr9(self, value):
        self.is_rrr9 = value
        mm.tp_set_is_rrr9(self, value)

    def set_is_rrr10(self, value):
        self.is_rrr10 = value
        mm.tp_set_is_rrr10(self, value)

    def set_is_rrr11(self, value):
        self.is_rrr11 = value
        mm.tp_set_is_rrr11(self, value)

    def set_is_rrr12(self, value):
        self.is_rrr12 = value
        mm.tp_set_is_rrr12(self, value)

    def set_is_rrr13(self, value):
        self.is_rrr13 = value
        mm.tp_set_is_rrr13(self, value)

    def set_is_rrr14(self, value):
        self.is_rrr14 = value
        mm.tp_set_is_rrr14(self, value)

    def set_is_rrr15(self, value):
        self.is_rrr15 = value
        mm.tp_set_is_rrr15(self, value)

    def set_is_rrr16(self, value):
        self.is_rrr16 = value
        mm.tp_set_is_rrr16(self, value)

    def set_is_rrr17(self, value):
        self.is_rrr17 = value
        mm.tp_set_is_rrr17(self, value)

    def set_is_rrr18(self, value):
        self.is_rrr18 = value
        mm.tp_set_is_rrr18(self, value)

    def set_is_rrr19(self, value):
        self.is_rrr19 = value
        mm.tp_set_is_rrr19(self, value)

    def set_is_rrr20(self, value):
        self.is_rrr20 = value
        mm.tp_set_is_rrr20(self, value)

    def set_is_rrr21(self, value):
        self.is_rrr21 = value
        mm.tp_set_is_rrr21(self, value)

    def set_is_rrr22(self, value):
        self.is_rrr22 = value
        mm.tp_set_is_rrr22(self, value)

    def set_is_rrr23(self, value):
        self.is_rrr23 = value
        mm.tp_set_is_rrr23(self, value)

    def set_is_rrr24(self, value):
        self.is_rrr24 = value
        mm.tp_set_is_rrr24(self, value)

    def set_is_rrr25(self, value):
        self.is_rrr25 = value
        mm.tp_set_is_rrr25(self, value)

    def set_is_rrr26(self, value):
        self.is_rrr26 = value
        mm.tp_set_is_rrr26(self, value)

    def set_is_rrr27(self, value):
        self.is_rrr27 = value
        mm.tp_set_is_rrr27(self, value)

    def set_is_rrr28(self, value):
        self.is_rrr28 = value
        mm.tp_set_is_rrr28(self, value)

    def set_is_rrr29(self, value):
        self.is_rrr29 = value
        mm.tp_set_is_rrr29(self, value)

    def set_is_rrr30(self, value):
        self.is_rrr30 = value
        mm.tp_set_is_rrr30(self, value)

    def set_is_rrr31(self, value):
        self.is_rrr31 = value
        mm.tp_set_is_rrr31(self, value)

    def set_is_rrr32(self, value):
        self.is_rrr32 = value
        mm.tp_set_is_rrr32(self, value)

    def set_is_rrr33(self, value):
        self.is_rrr33 = value
        mm.tp_set_is_rrr33(self, value)

    def set_is_rrr34(self, value):
        self.is_rrr34 = value
        mm.tp_set_is_rrr34(self, value)

    def set_is_rrr35(self, value):
        self.is_rrr35 = value
        mm.tp_set_is_rrr35(self, value)

    def set_is_rrr36(self, value):
        self.is_rrr36 = value
        mm.tp_set_is_rrr36(self, value)

    def set_is_rrr37(self, value):
        self.is_rrr37 = value
        mm.tp_set_is_rrr37(self, value)

    def set_is_rrr38(self, value):
        self.is_rrr38 = value
        mm.tp_set_is_rrr38(self, value)

    def set_is_rrr39(self, value):
        self.is_rrr39 = value
        mm.tp_set_is_rrr39(self, value)

    def set_is_rrr40(self, value):
        self.is_rrr40 = value
        mm.tp_set_is_rrr40(self, value)

    def set_is_rrr41(self, value):
        self.is_rrr41 = value
        mm.tp_set_is_rrr41(self, value)

    def set_is_rrr42(self, value):
        self.is_rrr42 = value
        mm.tp_set_is_rrr42(self, value)

    def set_is_rrr43(self, value):
        self.is_rrr43 = value
        mm.tp_set_is_rrr43(self, value)

    def set_is_rrr44(self, value):
        self.is_rrr44 = value
        mm.tp_set_is_rrr44(self, value)

    def set_is_rrr45(self, value):
        self.is_rrr45 = value
        mm.tp_set_is_rrr45(self, value)

    def set_is_rrr46(self, value):
        self.is_rrr46 = value
        mm.tp_set_is_rrr46(self, value)

    def set_is_animated_lr_flip(self, value):
        self.is_animated_lr_flip = value
        mm.tp_set_is_animated_lr_flip(self, value)

    def set_is_solid_ground(self, value):
        self.is_solid_ground = value
        mm.tp_set_is_solid_ground(self, value)

    def set_is_movement_blocking(self, value):
        self.is_movement_blocking = value
        mm.tp_set_is_movement_blocking(self, value)

    def set_is_treasure(self, value):
        self.is_treasure = value
        mm.tp_set_is_treasure(self, value)

    def set_is_key(self, value):
        self.is_key = value
        mm.tp_set_is_key(self, value)

    def set_is_wall(self, value):
        self.is_wall = value
        mm.tp_set_is_wall(self, value)

    def set_is_floor(self, value):
        self.is_floor = value
        mm.tp_set_is_floor(self, value)

    def set_is_door(self, value):
        self.is_door = value
        mm.tp_set_is_door(self, value)

    def set_is_lava(self, value):
        self.is_lava = value
        mm.tp_set_is_lava(self, value)

    def set_is_water(self, value):
        self.is_water = value
        mm.tp_set_is_water(self, value)

    def set_is_minable(self, value):
        self.is_minable = value


def get_random_treasure(toughness=0):
    while True:
        tp = all_tps[random.choice(all_treasure_tps)]
        if tp.is_minable:
            continue

        roll = random.randint(1, 1000) + toughness * 10
        if roll >= tp.d1000_appearing_roll:
            return tp


def get_random_wall():
    while True:
        tp = all_tps[random.choice(all_wall_tps)]

        roll = random.randint(1, 1000)
        if roll >= tp.d1000_appearing_roll:
            return tp


def get_random_door():
    while True:
        tp = all_tps[random.choice(all_door_tps)]

        roll = random.randint(1, 1000)
        if roll >= tp.d1000_appearing_roll:
            return tp


def get_random_floor():
    while True:
        tp = all_tps[random.choice(all_floor_tps)]

        roll = random.randint(1, 1000)
        if roll >= tp.d1000_appearing_roll:
            return tp


def get_random_dungeon():
    while True:
        tp = all_tps[random.choice(all_dungeon_tps)]

        roll = random.randint(1, 1000)
        if roll >= tp.d1000_appearing_roll:
            return tp


def get_random_minable_treasure(toughness=0):
    while True:
        tp = all_tps[random.choice(all_treasure_tps)]
        if not tp.is_minable:
            continue

        roll = random.randint(1, 1000) + toughness * 10
        if roll >= tp.d1000_appearing_roll:
            return tp


def same_type(tp1, tp2):
    if tp1.is_monst and tp2.is_monst:
        return True
    if tp1.is_player and tp2.is_player:
        return True
    if tp1.is_key and tp2.is_key:
        return True
    if tp1.is_wall and tp2.is_wall:
        return True
    if tp1.is_floor and tp2.is_floor:
        return True
    if tp1.is_lava and tp2.is_lava:
        return True
    if tp1.is_water and tp2.is_water:
        return True
    if tp1.is_carpet and tp2.is_carpet:
        return True
    if tp1.is_lawn and tp2.is_lawn:
        return True
    return False
