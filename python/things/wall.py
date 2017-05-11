import tp


def thing_init(t):
    return


def wall_init(name, short_name, tiles=[]):
    x = tp.Tp(name)

    x.set_short_name(short_name)
    x.set_is_movement_blocking(True)
    x.set_is_wall(True)
    x.set_is_solid_ground(True)
    x.set_blit_top_off(1)

    if tiles is not None:
        for t in tiles:
            x.set_tile(t, delay_ms=150)
    else:
        x.set_tile(tile=name, delay_ms=150)

    x.thing_init = thing_init


def init():
    wall_init(name="wall1",
              short_name="A boring wall",
              tiles=[
                     "wall1",
                    ])

init()
