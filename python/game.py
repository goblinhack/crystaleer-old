import util
import mm
import level
import thing
import pickle
import os.path
import game
import wid_help
import wid_help_editor
import wid_tp_editor
import wid_quit
import console

global g


class Game:

    class_version = 1

    def __init__(self):

        self.version = self.__class__.class_version

        #
        # Max thing ID in use in any level. This grows forever.
        #
        self.wid_player_location = None
        self.save_file = "save_file"
        self.player = None
        self.editor_mode = False
        self.editor_mode_draw = False
        self.editor_mode_erase = False
        self.editor_mode_yank = False
        self.editor_mode_tp = None

        self.last_level_seed = None
        self.last_selected_tile_x = 0
        self.last_selected_tile_y = 0

    def new_game(self):

        self.sdl_delay = 1
        self.seed = 10
        self.where = util.Xyz(0, 0, 0)
        self.load_level(self.seed)

    def load_level(self, seed):

        self.game_map_create()
        self.level = level.Level(xyz=self.where, seed=seed)

    def load_level_finalize(self):
        mm.game_set_sdl_delay(self.sdl_delay)

    def save(self):
        l = self.level
        s = os.path.normcase(
                os.path.join(os.environ["APPDATA"], self.save_file))

        with open(s, 'wb') as f:
            pickle.dump(self.seed, f, pickle.HIGHEST_PROTOCOL)
            pickle.dump(self.sdl_delay, f, pickle.HIGHEST_PROTOCOL)
            pickle.dump(self.where, f, pickle.HIGHEST_PROTOCOL)

            self.last_level_seed = l.seed
            pickle.dump(self.last_level_seed, f, pickle.HIGHEST_PROTOCOL)

            l.save(f)
            mm.con("Game saved @ level {0} to {1}".format(str(l), s))

    def upgrade(self):

        if self.version < 2:
            self.v2_field = 2

#        self.debug("upgraded from ver {0} to {1}".format(
#                   self.version, self.__class__.class_version))

        self.version = self.__class__.class_version

    def load(self):

        s = os.path.normcase(os.path.join(os.environ["APPDATA"],
                                          self.save_file))
        with open(s, 'rb') as f:
            mm.log("Game loading from {0}".format(s))

            self.seed = pickle.load(f)
            self.sdl_delay = pickle.load(f)
            self.where = pickle.load(f)
            self.last_level_seed = pickle.load(f)

            self.load_level(self.last_level_seed)
            mm.log("Game loaded @ level {0} to {1}".format(str(self.level), s))
            mm.con("Loaded previously saved game")

        if self.version != self.__class__.class_version:
            self.upgrade()

    def destroy(self):
        l = self.level
        l.destroy()

    def game_map_create(self):
        mm.game_map_create()

    def map_wid_destroy(self):
        mm.game_map_destroy()

    #
    # Get rid of the path indicators where the player will move
    #
    def map_clear_focus(self):
        mm.game_map_clear_selection_buttons()

    #
    # Mouse is over a map tile; show the route back to the player
    #
    def map_mouse_over(self, w, x, y, wheelx, wheely, button):
        #
        # Check we can get back from the chosen point to the player.
        #
        player = self.player

        #
        # Want to scroll without the focus moving
        #
        if wheelx != 0 or wheely != 0:

            if self.editor_mode:
                if wheelx > 0:
                    player.move(player.x - 1, player.y)
                if wheelx < 0:
                    player.move(player.x + 1, player.y)
                if wheely > 0:
                    player.move(player.x, player.y - 1)
                if wheely < 0:
                    player.move(player.x, player.y + 1)
                return True
            else:
                if wheelx > 0:
                    self.map_mouse_down(w, player.x - 1, player.y, 1)
                if wheelx < 0:
                    self.map_mouse_down(w, player.x + 1, player.y, 1)
                if wheely > 0:
                    self.map_mouse_down(w, player.x, player.y - 1, 1)
                if wheely < 0:
                    self.map_mouse_down(w, player.x, player.y + 1, 1)
                return True

        self.map_clear_focus()
        self.map_selected_tile(x, y)

        if self.editor_mode:
            if button == 1:
                return self.map_mouse_down(w, x, y, button)
            return True

    #
    # Mouse is over a map tile; show the route back to the player
    #
    def map_selected_tile(self, x, y):

        self.last_selected_tile_x = x
        self.last_selected_tile_y = y

    #
    # Move the player to the chosen tile
    #
    def map_mouse_down(self, w, x, y, button):

        l = self.level

        if self.editor_mode:

            if button == 3 or self.editor_mode_erase:
                got_one = False
                if self.editor_mode_tp:
                    t = l.thing_find(x, y, self.editor_mode_tp.name)
                    if t is not None:
                        t.destroy("via editor erase")
                        got_one = True

                if not got_one:
                    t = l.thing_top(x, y)
                    if t is not None and not t.tp.is_player:
                        t.destroy("via editor erase 2")

            elif self.editor_mode_draw and self.editor_mode_tp:
                t = l.thing_find_same_type(x, y,
                                           self.editor_mode_tp.name)
                if t is not None:
                    t.destroy("via editor draw")

                t = thing.Thing(level=l,
                                tp_name=self.editor_mode_tp.name,
                                x=x, y=y)
                t.push()

            return True

        return True

    #
    # Player input
    #
    def map_key_down(self, w, sym, mod):

        self.map_help()
        self.map_clear_focus()

        x = self.last_selected_tile_x
        y = self.last_selected_tile_y
        l = self.level

        if self.editor_mode:
            if sym == mm.SDLK_TAB:
                self.editor_mode_draw = True
                self.editor_mode_erase = False
                wid_tp_editor.visible()
                self.map_help()
                return True

            if sym == mm.SDLK_d:
                self.editor_mode_draw = True
                self.editor_mode_erase = False
                self.map_help()
                return True

            if sym == mm.SDLK_x:
                self.editor_mode_draw = False
                self.editor_mode_erase = True
                self.map_help()
                return True

            if sym == mm.SDLK_y:
                t = l.thing_top(x, y)
                if t and not t.tp.is_player:
                    self.editor_mode_tp = t.tp
                    self.editor_mode_draw = True
                    self.editor_mode_erase = False
                self.map_help()
                return True

            if sym == mm.SDLK_ESCAPE:
                wid_tp_editor.hide()
                self.editor_mode = False
                self.editor_mode_draw = True
                self.editor_mode_erase = False
                self.map_help()
                return True

            if mod == mm.KMOD_LCTRL or mod == mm.KMOD_RCTRL:
                if sym == mm.SDLK_z:
                    l.things_remove_all_except_player()
                    return True

            if mod == mm.KMOD_LCTRL or mod == mm.KMOD_RCTRL:
                if sym == mm.SDLK_f:
                    l.things_flood_fill(x, y, self.editor_mode_tp)
                    return True

            if mod == mm.KMOD_LSHIFT or mod == mm.KMOD_RSHIFT:
                if sym == mm.SDLK_EQUALS:
                    t = l.thing_top(x, y)
                    if t:
                        t.set_depth(t.depth + 1)
                    return True

            if sym == mm.SDLK_MINUS:
                t = l.thing_top(x, y)
                if t:
                    t.set_depth(t.depth - 1)
                return True

            if sym == mm.SDLK_SPACE:
                return self.map_mouse_down(w, x, y, 1)

            if sym == mm.SDLK_DELETE or sym == mm.SDLK_BACKSPACE:
                return self.map_mouse_down(w, x, y, 3)

            if sym == mm.SDLK_s:
                self.save()
                return True

            if sym == mm.SDLK_q:
                wid_quit.visible()
                return True

            if sym == mm.SDLK_LCTRL:
                return False
            if sym == mm.SDLK_LSHIFT:
                return False
            if sym == mm.SDLK_LALT:
                return False
            if sym == mm.SDLK_LGUI:
                return False
            if sym == mm.SDLK_RCTRL:
                return False
            if sym == mm.SDLK_RSHIFT:
                return False
            if sym == mm.SDLK_RALT:
                return False
            if sym == mm.SDLK_RGUI:
                return False

            wid_help_editor.visible()

            return True
        else:
            if sym == mm.SDLK_s:
                self.save()
                return True

            if sym == mm.SDLK_q:
                wid_quit.visible()
                return True

        if mod == mm.KMOD_LCTRL or mod == mm.KMOD_RCTRL:
            if sym == mm.SDLK_e:
                if not self.editor_mode:
                    game.g.editor_mode = True
                    game.g.editor_mode_draw = True
                    game.g.editor_mode_erase = False
                self.map_help()
                return True

        if sym == mm.SDLK_LCTRL:
            return False
        if sym == mm.SDLK_LSHIFT:
            return False
        if sym == mm.SDLK_LALT:
            return False
        if sym == mm.SDLK_LGUI:
            return False
        if sym == mm.SDLK_RCTRL:
            return False
        if sym == mm.SDLK_RSHIFT:
            return False
        if sym == mm.SDLK_RALT:
            return False
        if sym == mm.SDLK_RGUI:
            return False

        wid_help.visible()

        return True

    def map_help(self):

        mm.tip("")
        mm.tip2("")

        if self.editor_mode:
            tip = ""

            if self.editor_mode_tp:
                tp_name = self.editor_mode_tp.name
                tip += "%%tp={}$ ".format(tp_name)

            if self.editor_mode_draw:
                tip += "%%fg=green$Draw mode. "
                tip += "%%fg=green$h%%fg=reset$ for help. "
                tip += "%%fg=green$x%%fg=reset$ erase. "

            if self.editor_mode_erase:
                tip += "%%fg=green$Erase mode. "
                tip += "%%fg=green$h%%fg=reset$ for help. "
                tip += "%%fg=green$d%%fg=reset$ draw mode. "

            tip += "%%fg=green$y%%fg=reset$ yank. "
            tip += "%%fg=green$TAB%%fg=reset$ to select. "
            tip += "%%fg=red$ESC%%fg=reset$ to game mode"

            mm.tip2(tip)
        else:
            mm.tip2("Press h for help. Click to move.")


def map_mouse_over(w, relx, rely, wheelx, wheely, button):
    g.map_mouse_over(w, relx, rely, wheelx, wheely, button)


def map_mouse_down(w, x, y, button):
    return g.map_mouse_down(w, x, y, button)


def map_key_down(w, sym, mod):
    return g.map_key_down(w, sym, mod)


g = None


def game_new():
    global g

    game_dir = os.path.join(os.environ["APPDATA"], "crystaleer")

    mm.log("Appdata dir is " + game_dir)

    g = Game()
    s = os.path.normcase(os.path.join(os.environ["APPDATA"], g.save_file))
    if os.path.isfile(s):
        g.load()
    else:
        g.new_game()

    g.load_level_finalize()

    console.hello()
