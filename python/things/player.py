import tp


def thing_init(t):
    return


def player_init(name, short_name, tiles=[]):
    x = tp.Tp(name)

    x.set_short_name(short_name)
    x.set_is_movement_blocking(True)
    x.set_is_animated(True)
    x.set_is_player(True)
    x.set_is_solid_ground(True)
    x.set_blit_top_off(1)
    x.set_ms_to_move_one_tile(150)

    delay = 150
    x.set_tile(tile=name + "-bl-3",
               is_dir_left=True,
               is_moving=True,
               delay_ms=delay)
    x.set_tile(tile=name + "-bl-2",
               is_dir_left=True,
               is_moving=True,
               delay_ms=delay)
    x.set_tile(tile=name + "-bl-1",
               is_dir_left=True,
               is_moving=True,
               delay_ms=delay)

    x.set_tile(tile=name + "-bl-2",
               is_dir_left=True,
               is_moving=False,
               delay_ms=delay)

    x.set_tile(tile=name + "-br-3",
               is_dir_up=True,
               is_moving=True,
               delay_ms=delay)
    x.set_tile(tile=name + "-br-2",
               is_dir_up=True,
               is_moving=True,
               delay_ms=delay)
    x.set_tile(tile=name + "-br-1",
               is_dir_up=True,
               is_moving=True,
               delay_ms=delay)

    x.set_tile(tile=name + "-br-2",
               is_dir_up=True,
               is_moving=False,
               delay_ms=delay)

    x.set_tile(tile=name + "-tl-3",
               is_dir_down=True,
               is_moving=True,
               delay_ms=delay)
    x.set_tile(tile=name + "-tl-2",
               is_dir_down=True,
               is_moving=True,
               delay_ms=delay)
    x.set_tile(tile=name + "-tl-1",
               is_dir_down=True,
               is_moving=True,
               delay_ms=delay)

    x.set_tile(tile=name + "-tl-2",
               is_dir_down=True,
               is_moving=False,
               delay_ms=delay)

    x.set_tile(tile=name + "-tr-3",
               is_dir_right=True,
               is_moving=True,
               delay_ms=delay)
    x.set_tile(tile=name + "-tr-2",
               is_dir_right=True,
               is_moving=True,
               delay_ms=delay)
    x.set_tile(tile=name + "-tr-1",
               is_dir_right=True,
               is_moving=True,
               delay_ms=delay)

    x.set_tile(tile=name + "-tr-2",
               is_dir_right=True,
               is_moving=False,
               delay_ms=delay)

    for _ in range(20):
        x.set_tile(tile=name + "-none-2",
                   is_dir_none=True, delay_ms=2000)
        x.set_tile(tile=name + "-none-1",
                   is_dir_none=True, delay_ms=50)
    x.set_tile(tile=name + "-none-3",
               is_dir_none=True, delay_ms=200)
    x.set_tile(tile=name + "-none-4",
               is_dir_none=True, delay_ms=200)

    x.thing_init = thing_init


def init():
    player_init(name="player1", short_name="A boring player")

init()
