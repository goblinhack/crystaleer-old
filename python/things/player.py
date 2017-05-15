import tp


def thing_init(t):
    return


def player_init(name, short_name, tiles=[]):
    x = tp.Tp(name)

    x.set_short_name(short_name)
    x.set_is_movement_blocking(True)
    x.set_is_player(True)
    x.set_is_solid_ground(True)
    x.set_blit_top_off(1)
    x.set_speed(300)

    if tiles is not None:
        for t in tiles:
            x.set_tile(t, delay_ms=150)
    else:
        x.set_tile(tile=name, delay_ms=150)

    x.thing_init = thing_init


def init():
    player_init(name="player1",
                short_name="A boring player",
                tiles=[
                       "player1",
                      ])

init()
