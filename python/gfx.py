import mm


def init_tex():

    mm.tex_load(file="data/gfx/wid1.tga", name="wid1")
    mm.tex_load(file="data/gfx/wid2.tga", name="wid2")
    mm.tex_load(file="data/gfx/sword.tga", name="sword")


def init_tiles_wid1():

    mm.tex_load_tiled(
            file="data/gfx/wid1.tga",
            name="wid1",
            width=64, height=64)

    tiles = [
        "wid1_tl",
        "wid1_top1",
        "wid1_top2",
        "wid1_top3",
        "wid1_top4",
        "wid1_top5",
        "wid1_top6",
        "wid1_tr",
        "wid1_left1",
        "wid1_1",
        "wid1_2",
        "wid1_3",
        "wid1_4",
        "wid1_5",
        "wid1_6",
        "wid1_right1",
        "wid1_left2",
        "wid1_7",
        "wid1_8",
        "wid1_9",
        "wid1_10",
        "wid1_11",
        "wid1_12",
        "wid1_right2",
        "wid1_left3",
        "wid1_13",
        "wid1_14",
        "wid1_15",
        "wid1_16",
        "wid1_17",
        "wid1_18",
        "wid1_right3",
        "wid1_left4",
        "wid1_19",
        "wid1_20",
        "wid1_21",
        "wid1_22",
        "wid1_23",
        "wid1_24",
        "wid1_right4",
        "wid1_left5",
        "wid1_25",
        "wid1_26",
        "wid1_27",
        "wid1_28",
        "wid1_29",
        "wid1_30",
        "wid1_right5",
        "wid1_left6",
        "wid1_31",
        "wid1_32",
        "wid1_33",
        "wid1_34",
        "wid1_35",
        "wid1_36",
        "wid1_right6",
        "wid1_bl",
        "wid1_bot1",
        "wid1_bot2",
        "wid1_bot3",
        "wid1_bot4",
        "wid1_bot5",
        "wid1_bot6",
        "wid1_br",
    ]

    mm.tile_load_arr(
            tex_name="wid1",
            tex_name_black_and_white="",
            width=64, height=64,
            arr=tiles)

    mm.wid_tiles_load(
            name="wid1",
            scale=0.5)


def init_tiles_wid2():

    mm.tex_load_tiled(
            file="data/gfx/wid2.tga",
            name="wid2",
            width=64, height=64)

    tiles = [
        "wid2_tl",
        "wid2_top1",
        "wid2_top2",
        "wid2_top3",
        "wid2_top4",
        "wid2_top5",
        "wid2_top6",
        "wid2_tr",
        "wid2_left1",
        "wid2_1",
        "wid2_2",
        "wid2_3",
        "wid2_4",
        "wid2_5",
        "wid2_6",
        "wid2_right1",
        "wid2_left2",
        "wid2_7",
        "wid2_8",
        "wid2_9",
        "wid2_10",
        "wid2_11",
        "wid2_12",
        "wid2_right2",
        "wid2_left3",
        "wid2_13",
        "wid2_14",
        "wid2_15",
        "wid2_16",
        "wid2_17",
        "wid2_18",
        "wid2_right3",
        "wid2_left4",
        "wid2_19",
        "wid2_20",
        "wid2_21",
        "wid2_22",
        "wid2_23",
        "wid2_24",
        "wid2_right4",
        "wid2_left5",
        "wid2_25",
        "wid2_26",
        "wid2_27",
        "wid2_28",
        "wid2_29",
        "wid2_30",
        "wid2_right5",
        "wid2_left6",
        "wid2_31",
        "wid2_32",
        "wid2_33",
        "wid2_34",
        "wid2_35",
        "wid2_36",
        "wid2_right6",
        "wid2_bl",
        "wid2_bot1",
        "wid2_bot2",
        "wid2_bot3",
        "wid2_bot4",
        "wid2_bot5",
        "wid2_bot6",
        "wid2_br",
    ]

    mm.tile_load_arr(
            tex_name="wid2",
            tex_name_black_and_white="",
            width=64, height=64,
            arr=tiles)

    mm.wid_tiles_load(
            name="wid2",
            scale=0.5)


def init_tiles_wid3():

    mm.tex_load_tiled(
            file="data/gfx/wid3.tga",
            name="wid3",
            width=64, height=64)

    tiles = [
        "wid3_tl",
        "wid3_top1",
        "wid3_top2",
        "wid3_top3",
        "wid3_top4",
        "wid3_top5",
        "wid3_top6",
        "wid3_tr",
        "wid3_left1",
        "wid3_1",
        "wid3_2",
        "wid3_3",
        "wid3_4",
        "wid3_5",
        "wid3_6",
        "wid3_right1",
        "wid3_left2",
        "wid3_7",
        "wid3_8",
        "wid3_9",
        "wid3_10",
        "wid3_11",
        "wid3_12",
        "wid3_right2",
        "wid3_left3",
        "wid3_13",
        "wid3_14",
        "wid3_15",
        "wid3_16",
        "wid3_17",
        "wid3_18",
        "wid3_right3",
        "wid3_left4",
        "wid3_19",
        "wid3_20",
        "wid3_21",
        "wid3_22",
        "wid3_23",
        "wid3_24",
        "wid3_right4",
        "wid3_left5",
        "wid3_25",
        "wid3_26",
        "wid3_27",
        "wid3_28",
        "wid3_29",
        "wid3_30",
        "wid3_right5",
        "wid3_left6",
        "wid3_31",
        "wid3_32",
        "wid3_33",
        "wid3_34",
        "wid3_35",
        "wid3_36",
        "wid3_right6",
        "wid3_bl",
        "wid3_bot1",
        "wid3_bot2",
        "wid3_bot3",
        "wid3_bot4",
        "wid3_bot5",
        "wid3_bot6",
        "wid3_br",
    ]

    mm.tile_load_arr(
            tex_name="wid3",
            tex_name_black_and_white="",
            width=64, height=64,
            arr=tiles)

    mm.wid_tiles_load(
            name="wid3",
            scale=0.5)


def init_tiles_button_plain():

    mm.tex_load_tiled(
            file="data/gfx/button_plain.tga",
            name="button_plain",
            width=64, height=64)

    tiles = [
        "button_plain_tl",
        "button_plain_top1",
        "button_plain_top2",
        "button_plain_tr",
        "button_plain_left1",
        "button_plain_1",
        "button_plain_2",
        "button_plain_right1",
        "button_plain_left2",
        "button_plain_3",
        "button_plain_4",
        "button_plain_right2",
        "button_plain_bl",
        "button_plain_bot1",
        "button_plain_bot2",
        "button_plain_br",
    ]

    mm.tile_load_arr(
            tex_name="button_plain",
            tex_name_black_and_white="",
            width=6, height=6,
            arr=tiles)

    mm.wid_tiles_load(
            name="button_plain",
            scale=0.5)


def init_tiles_button_green():

    mm.tex_load_tiled(
            file="data/gfx/button_green.tga",
            name="button_green",
            width=24, height=24)

    tiles = [
        "button_green_tl",
        "button_green_top1",
        "button_green_top2",
        "button_green_tr",
        "button_green_left1",
        "button_green_1",
        "button_green_2",
        "button_green_right1",
        "button_green_left2",
        "button_green_3",
        "button_green_4",
        "button_green_right2",
        "button_green_bl",
        "button_green_bot1",
        "button_green_bot2",
        "button_green_br",
    ]

    mm.tile_load_arr(
            tex_name="button_green",
            tex_name_black_and_white="",
            width=6, height=6,
            arr=tiles)

    mm.wid_tiles_load(
            name="button_green",
            scale=0.5)


def init_tiles_button_red():

    mm.tex_load_tiled(
            file="data/gfx/button_red.tga",
            name="button_red",
            width=24, height=24)

    tiles = [
        "button_red_tl",
        "button_red_top1",
        "button_red_top2",
        "button_red_tr",
        "button_red_left1",
        "button_red_1",
        "button_red_2",
        "button_red_right1",
        "button_red_left2",
        "button_red_3",
        "button_red_4",
        "button_red_right2",
        "button_red_bl",
        "button_red_bot1",
        "button_red_bot2",
        "button_red_br",
    ]

    mm.tile_load_arr(
            tex_name="button_red",
            tex_name_black_and_white="",
            width=6, height=6,
            arr=tiles)

    mm.wid_tiles_load(
            name="button_red",
            scale=0.5)


def init_tiles_iso():

    mm.tex_load_tiled(
            file="data/gfx/iso.tga",
            name="iso",
            width=128,
            height=128)

    tiles = [
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        #
        "player1",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        #
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "wall1",
        "wall2",
        "wall3",
        "wall4",
        "wall5",
        "wall6",
        "wall7",
        #
    ]

    mm.tile_load_arr(tex_name="iso",
                     tex_name_black_and_white="",
                     width=128, height=128,
                     arr=tiles)

init_tex()
init_tiles_wid1()
init_tiles_wid2()
init_tiles_wid3()
init_tiles_button_plain()
init_tiles_button_red()
init_tiles_button_green()
init_tiles_iso()
