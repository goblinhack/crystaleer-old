import mm
import game


def hook_new_game():
    mm.con("New game hook")
    game.game_new()


def hook_destroy_game():
    mm.con("Destroy game hook")

    if game.g is not None:
        game.g.destroy()


def hook_game_tick():
    if game.g is None:
        return

    game.g.tick()


def hook_player_move_start():
    if game.g is None:
        return
    if game.g.player is None:
        return
    game.g.player.is_moving = True


def hook_player_move_end():
    if game.g is None:
        return

    player = game.g.player
    if not player:
        return

    player.is_moving = False

    if len(player.nexthops) > 0:
        delta = player.nexthops[0]
        to = player.at.add(delta)
        mm.con("delta {}".format(str(delta)))
        player.nexthops.pop()
        player.move(to)
