#include "background.h"
#include "boat.h"
#include "macros.h"
#include "bitmap.h"
#include "camera.h"
#include "effect.h"
#include "event.h"
#include "input.h"
#include "shadow.h"
#include "stcoli.h"

void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);

void fn_1_E3164(u16 n, Vec *pts, GXColor *color);

extern struct GMA *minigameGma;
extern u32 g_some_render_flag;
extern u32 lbl_1_bss_D432C;
extern u8 lbl_1_bss_D4330;
extern u32 lbl_1_bss_D4364[0x13];
extern Vec lbl_1_data_2E418[];
extern char *lbl_14_data_ABC[];

u32 lbl_14_data_2C0[0x5C] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000, 0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000, 0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000, 0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};
s16 lbl_14_data_430[4] = {1, 3, 5, 7};

struct BoatPlayer lbl_14_bss_208[8];
#ifndef __MWERKS__
/* for rend_efc_mirror.c, whose fixed-offset view of BoatPlayer is 32-bit only */
float pc_boat_player_unk478(int i) { return lbl_14_bss_208[i].unk478; }
#endif
struct BoatPlayer *lbl_14_bss_2988[8];

void fn_14_32FC(void)
{
    char *names[0x4D] = {
        "OAR",
        "ITEMBOX",
        "MISSILE01",
        "MISSILE01",
        "SPLASH",
        "BOAT_COVER_H",
        "BOAT_COVER_L",
        "TAR_IND_MAP_A",
        "pen_ocean1",
        "TAR_WATER_SPEC",
        "WAVE_EFFECT01_vtx",
        "MISSILE02",
        "SPIRAL_B_vtx",
        "MISSILE03",
        "WHALE_WATER",
        "WAVE_EFFECT02",
        "TAR_IND_MAP_B",
        "TAR_IND_MAP_C",
        "SPLASH_B",
        "SPIRAL_B_vtx",
        "SPIRAL_B2_vtx",
        "WHALE_WATER_C",
        "WHALE_WATER_B",
        "MIZUBASHIRA_A",
        "BANANA_KAYAK",
        "SUB_ENGINE_L",
        "SUB_SCOPE",
        "SUB_SCREW",
        "ITEM_A",
        "ITEM_B",
        "ITEM_C",
        "ITEM_D",
        "BALL_COLOR",
        "BALL_COLOR_L1",
        "BALL_COLOR_L2",
        "BALL_COLOR",
        "BALL_COLOR_L3",
        "BOWL_BOMB01",
        "BOWL_BOMB02",
        "BOWL_BOMB03",
        "BOWL_BOMB04",
        "BOWL_BOMB05",
        "BOWL_BOMB06",
        "BOWL_BOMB07",
        "BOWL_BOMB08",
        "BOWL_BOMB09",
        "BOWL_BOMB10",
        "BOWL_BOMB11",
        "BOWL_BOMB12",
        "BOWL_BOMB13",
        "BOWL_BOMB14",
        "BOWL_BOMB15",
        "BOWL_BOMB16",
        "BOWL_BOMB17",
        "BOWL_BOMB18",
        "BOWL_BOMB19",
        "BOWL_BOMB20",
        "BOWL_BOMB21",
        "BOWL_BOMB22",
        "BOWL_BOMB23",
        "BOWL_BOMB24",
        "BOWL_BOMB25",
        "BOWL_BOMB26",
        "BOWL_BOMB27",
        "BOWL_BOMB28",
        "BOWL_BOMB29",
        "BOWL_BOMB30",
        "BOWL_BOMB31",
        "BOWL_BOMB32",
        "BOWL_BOMB33",
        "BOWL_BOMB34",
        "BOWL_BOMB35",
        "BOWL_BOMB36",
        "BOWL_BOMB37",
        "BOWL_BOMB38",
        "BOWL_BOMB39",
        "BOWL_BOMB39",
    };
    s16 i;

    lbl_1_bss_D432C = 0;
    for (i = 0; i < 0x4D; i++) {
        if ((lbl_14_bss_B4[i] = fn_14_12598(minigameGma, names[i])) == NULL) {
            lbl_14_bss_B4[i] = init_common_gma->modelEntries[0x25].model;
        }
    }
}

void fn_14_33E8(u8 arg0)
{
    s32 (*func)(float);

    if (arg0 == 2) {
        func = fn_14_5D9C;
    } else {
        func = fn_14_5E08;
    }
    lbl_14_bss_1E8 = func;
}

struct BoatPathPoint
{
    u32 unk0;
    Vec pos;
};

void fn_14_3418(void)
{
    Vec pts[1000];
    Vec sp58;
    Vec sp4C;
    Vec sp40;
    Vec sp34;
    s16 i;
    struct Ball *saved;
    struct BoatPlayer *pl;
    u32 t;

    saved = current_ball;
    if (dip_switches & 1) {
        for (i = 0; i < 0x13; i++) {
            lbl_1_bss_D4364[i] = 0;
        }
    }
    fn_14_639C();
    for (i = 0; i < 5; i++) {
        s8 status = g_poolInfo.cameraPool.statusList[i];

        if (status == 0 || status == 4) {
            continue;
        }
        if (!(cameras[i].sub28.viewportW > 0.0f) || !(cameras[i].sub28.viewportH > 0.0f)) {
            continue;
        }
        camera_apply_viewport_if_changed(i);
        fn_1_D4870();
        g_maybe_some_bg_effect_handler();
        u_reset_light_group_stack(i);
        current_ball = &balls[i];
        pl = &lbl_14_bss_208[i];
        fn_14_6024(pl);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 1) && events[EVENT_REND_EFC].state == 2) {
            g_something_with_rend_efc(4);
        }
        lbl_1_bss_D4364[0] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 0x4000) {
            g_something_with_drawing_apes();
        }
        lbl_1_bss_D4364[0x38 / 4] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 2) {
            fn_14_4718();
        }
        lbl_1_bss_D4364[1] += BOAT_TICKS(t);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 4) && events[EVENT_STAGE].state == 2) {
            fn_14_53D4();
            fn_14_605C();
        }
        lbl_1_bss_D4364[2] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 0x40000) {
            if (events[EVENT_STAGE].state == 2) {
                g_draw_stage();
            }
            fn_1_120AD8();
        }
        lbl_1_bss_D4364[0x48 / 4] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 0x10) {
            draw_shadow_reqs();
        }
        lbl_1_bss_D4364[4] += BOAT_TICKS(t);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 0x20) && events[EVENT_BACKGROUND].state == 2) {
            g_smth_with_rendefc_reflective_height(400.0f);
            g_draw_bg();
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        lbl_1_bss_D4364[5] += BOAT_TICKS(t);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 0x40) && events[EVENT_ITEM].state == 2) {
            draw_items();
        }
        lbl_1_bss_D4364[6] += BOAT_TICKS(t);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 0x80) && events[EVENT_STOBJ].state == 2) {
            g_draw_stobjs();
        }
        lbl_1_bss_D4364[7] += BOAT_TICKS(t);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 0x100) && events[EVENT_EFFECT].state == 2) {
            avdisp_set_z_mode(1, 3, 0);
            fn_14_187A4();
            g_draw_effects();
            avdisp_set_z_mode(1, 3, 1);
        }
        lbl_1_bss_D4364[8] += BOAT_TICKS(t);

        if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x100) && lbl_14_bss_18 != NULL) {
            s16 j;

            fn_1_E2FD0(0x1E);
            mtxa_from_mtxb();
            sp58 = ((struct BoatPathPoint *) lbl_14_bss_18)[0].pos;
            for (j = 10; j < (s32) lbl_14_bss_60; j += 10) {
                { struct BoatPathPoint *pt = (struct BoatPathPoint *) lbl_14_bss_18 + j; sp4C = pt->pos; }
                fn_1_E3014(&sp58, &sp4C, &(GXColor){0x64, 0x64, 0xFF, 0xFF});
                sp58 = sp4C;
            }
        }
        if (((lbl_1_bss_D4328 & 0x200) || (lbl_1_bss_D4328 & 0x400)) && stagedef->extraData != NULL) {
            struct StageExtraChannels *path;
            s16 j;
            s16 n = 1000;

            if (n < 0) {
                n = 0;
            }
            if (n > 1000) {
                n = 1000;
            }
            fn_1_E2FD0(0x1E);
            mtxa_from_mtxb();
            if (lbl_1_bss_D4328 & 0x200) {
                path = (struct StageExtraChannels *) stagedef->extraData;
                for (j = 0; j < 1000; j++) {
                    fn_14_11524(path, 100.0 * ((float) j / 1000.0f), &pts[j]);
                }
                fn_1_E3164(n, pts, &(GXColor){0xFF, 0x64, 0x64, 0xFF});
            }
            if (lbl_1_bss_D4328 & 0x400) {
                path = stagedef->extraData->list;
                if ((s32) lbl_1_bss_D4330 < (s32) stagedef->extraData->listCount) {
                    path = &path[lbl_1_bss_D4330];
                }
                for (j = 0; j < 1000; j++) {
                    fn_14_11524(path, 100.0 * ((float) j / 1000.0f), &pts[j]);
                }
                fn_1_E3164(1000, pts, &(GXColor){0x64, 0xFF, 0x64, 0xFF});
            }
            if (lbl_1_bss_D4328 & 0x800) {
                float step = 0.0f;

                if (pad_status_groups[0].unk0[0].button & 2) {
                    step = 0.01f;
                } else if (pad_status_groups[0].unk0[0].button & 1) {
                    step = -0.01f;
                }
                if (pad_status_groups[0].unk0[0].button & 0x20) {
                    step *= 10.0;
                }
                lbl_14_bss_1EC += step;
                if (lbl_14_bss_1EC < 0.0) {
                    lbl_14_bss_1EC = 0.0f;
                } else if (lbl_14_bss_1EC > 100.0) {
                    lbl_14_bss_1EC = 100.0f;
                }
                if (lbl_1_bss_D4328 & 0x200) {
                    fn_14_11524(lbl_14_bss_70, lbl_14_bss_1EC, &sp40);
                    sp34 = (Vec){sp40.x, 3.0 + sp40.y, sp40.z};
                    fn_1_E3014(&sp40, &sp34, &(GXColor){0xFF, 0x64, 0x64, 0xFF});
                }
                if (lbl_1_bss_D4328 & 0x400) {
                    path = stagedef->extraData->list;
                    if ((s32) lbl_1_bss_D4330 < (s32) stagedef->extraData->listCount) {
                        path = &path[lbl_1_bss_D4330];
                    }
                    fn_14_11524(path, lbl_14_bss_1EC, &sp40);
                    sp34 = (Vec){sp40.x, 3.0 + sp40.y, sp40.z};
                    fn_1_E3014(&sp40, &sp34, &(GXColor){0x64, 0xFF, 0x64, 0xFF});
                }
                debug_textdraw_set_pos(1, 0xE);
                debug_textdraw_vsprintf_buf_1("path rate:%f\n", lbl_14_bss_1EC);
            }
        }
        if ((lbl_1_bss_D4328 & 0x1000) && i == 0) {
            s16 j;

            debug_textdraw_set_pos(1, 5);
            for (j = 0; j < 0x14; j++) {
                if (pl->unk8 & (1 << j)) {
                    debug_textdraw_vsprintf_buf_1("%s", lbl_14_data_ABC[j]);
                }
                debug_textdraw_vsprintf_buf_1("\n");
            }
        }
        if (events[EVENT_BALL].state == 2) {
            t = OSGetTick();
            if (lbl_1_bss_D432C & 0x200) {
                fn_14_4808();
            }
            lbl_1_bss_D4364[0x24 / 4] += BOAT_TICKS(t);

            t = OSGetTick();
            if (lbl_1_bss_D432C & 0x400) {
                fn_14_49FC();
            }
            lbl_1_bss_D4364[0x28 / 4] += BOAT_TICKS(t);

            t = OSGetTick();
            if (lbl_1_bss_D432C & 0x800) {
                fn_14_4BA0();
            }
            lbl_1_bss_D4364[0x2C / 4] += BOAT_TICKS(t);
        }

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 0x100) && events[EVENT_EFFECT].state == 2) {
            avdisp_set_z_mode(1, 3, 0);
            fn_14_18844();
            g_draw_effects();
            fn_14_188D8();
            avdisp_set_z_mode(1, 3, 1);
        }
        lbl_1_bss_D4364[8] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 0x20000) {
            fn_14_23CE0();
        }
        lbl_1_bss_D4364[0x44 / 4] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 0x10000) {
            fn_14_22FBC();
        }
        lbl_1_bss_D4364[0x40 / 4] += BOAT_TICKS(t);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 1) && events[EVENT_REND_EFC].state == 2) {
            g_something_with_rend_efc(0x10);
        }
        lbl_1_bss_D4364[0] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 8) {
            fn_14_136D4();
        }
        lbl_1_bss_D4364[3] += BOAT_TICKS(t);

        handle_test_camera();

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 0x1000) && (world_theme.unk8 & 1)) {
            fn_1_D6F9C(i);
        }
        lbl_1_bss_D4364[0x30 / 4] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 0x2000) {
            ord_tbl_draw_nodes();
        }
        lbl_1_bss_D4364[0x34 / 4] += BOAT_TICKS(t);

        t = OSGetTick();
        if (lbl_1_bss_D432C & 0x8000) {
            fn_14_21470();
        }
        lbl_1_bss_D4364[0x3C / 4] += BOAT_TICKS(t);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 0x1000) && (world_theme.unk8 & 1)) {
            fn_1_D6D84(i);
        }
        lbl_1_bss_D4364[0x30 / 4] += BOAT_TICKS(t);

        t = OSGetTick();
        if ((lbl_1_bss_D432C & 1) && events[EVENT_REND_EFC].state == 2) {
            g_something_with_rend_efc(8);
        }
        lbl_1_bss_D4364[0] += BOAT_TICKS(t);
    }
    current_ball = saved;
    camera_setup_default_projection();
}

void fn_14_469C(void)
{
    fn_14_4718();
    if (events[EVENT_STAGE].state == 2) {
        fn_14_58A0();
    }
    if (events[EVENT_ITEM].state == 2) {
        draw_items();
    }
    if (events[EVENT_BALL].state == 2) {
        fn_14_4808();
        fn_14_49FC();
    }
    ord_tbl_draw_nodes();
}

void fn_14_4718(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    s16 i;

    for (ball = balls, pl = lbl_14_bss_208, i = 0; i < (s32) lbl_14_bss_24; i++, ball++, pl++) {
        if (!(g_current_camera->flags & 0x40) && (lbl_14_bss_208[g_current_camera->idx].unk8 & 0x2000)) {
            if (!(pl->unk8 & 0x2000)) {
                continue;
            }
        } else {
            if (pl->unk8 & 0x2000) {
                continue;
            }
        }
        draw_ape(ball->ape);
    }
}

void fn_14_4808(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    s16 i;

    for (i = 0, ball = balls, pl = lbl_14_bss_208; i < (s32) lbl_14_bss_24; i++, ball++, pl++) {
        if (!(g_current_camera->flags & 0x40) && (lbl_14_bss_208[g_current_camera->idx].unk8 & 0x2000)) {
            if (!(pl->unk8 & 0x2000)) {
                continue;
            }
        } else {
            if (pl->unk8 & 0x2000) {
                continue;
            }
        }
        if (!(g_some_render_flag & 4) || !(fn_1_80D4(&ball->ape->pos) < 0.0f)) {
            fn_14_4934(ball, pl);
        }
    }
}

void fn_14_4934(struct Ball *ball, struct BoatPlayer *pl)
{
    Mtx mtx;

    mtxa_from_mtxb_translate(&ball->pos);
    mtxa_rotate_y(pl->unk3E2);
    mtxa_rotate_x(pl->unk3E0);
    mtxa_rotate_z(pl->unk3E4);
    mtxa_scale_s(pl->unk3B0);
    if (lbl_1_bss_D4328 & 0x10) {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(lbl_14_bss_B4[0x18]);
        return;
    }
    mtxa_to_mtx(mtx);
    fn_1_4E350(mtx, lbl_14_bss_B4[ball->unk179 + 0x20], ball->colorId);
}

void fn_14_49FC(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    s16 i;

    for (i = 0, ball = balls, pl = lbl_14_bss_208; i < (s32) lbl_14_bss_24; i++, ball++, pl++) {
        if (!(g_current_camera->flags & 0x40) && (lbl_14_bss_208[g_current_camera->idx].unk8 & 0x2000)) {
            if (!(pl->unk8 & 0x2000)) {
                continue;
            }
        } else {
            if (pl->unk8 & 0x2000) {
                continue;
            }
        }
        if (pl->unk3A8 <= 0.0) {
            continue;
        }
        if (g_some_render_flag & 4) {
            if (pl->unk8 & 0x28) {
                if (fn_1_80D4((Vec *) &pl->unk390) < 0.0f) {
                    continue;
                }
            } else {
                if (fn_1_80D4(&ball->ape->pos) < 0.0f) {
                    continue;
                }
            }
        }
        mtxa_from_mtxb();
        mtxa_mult_right(pl->unk360);
        mtxa_scale_s(pl->unk3A8);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(lbl_14_bss_B4[0]);
    }
}

void fn_14_4BA0(void)
{
    Vec sp80[8];
    Vec sp20[8];
    Vec sp14;
    float sp10;
    float spC;
    float sp8;
    float f31;
    struct Ball *ball;
    struct BoatPlayer *pl;
    Vec *a;
    Vec *b;
    s16 i;
    s16 r5;

    if ((g_current_camera->flags & 0x40) || !(lbl_14_bss_208[current_ball->playerId].unk8 & 8)) {
        mtxa_from_mtxb();
        i = 0;
        ball = balls;
        pl = lbl_14_bss_208;
        while (i < (s32) lbl_14_bss_24) {
            if (i != current_ball->playerId || (g_current_camera->flags & 0x40)) {
                a = &sp80[i];
                *a = ball->pos;
                a->y += ball->currRadius;
                if (a->y < pl->unk478) {
                    a->y = pl->unk478;
                }
                mtxa_tf_point(a, a);
                mtxa_tf_point(&ball->pos, &sp20[i]);
            }
            i++;
            ball++;
            pl++;
        }
        i = 0;
        ball = balls;
        pl = lbl_14_bss_208;
        while (i < (s32) lbl_14_bss_24) {
            if ((i != current_ball->playerId || (g_current_camera->flags & 0x40)) && !(pl->unk8 & 0x2000)) {
                a = &sp80[i];
                b = &sp20[i];
                if ((g_current_camera->flags & 0x40) || b->z < -4.0) {
                    fn_14_4FFC(a, &sp10);
                    fn_14_5240(ball, a, sp10, 0);
                } else if (b->z > -1.0) {
                    if (fn_14_5100(b, &spC) != 0) {
                        b->z -= 0.001 * ball->playerId;
                        fn_14_5240(ball, b, spC, -0x3800);
                    }
                } else {
                    f31 = (b->z - -1.0) / -3.0;
                    if (f31 > 1.0) {
                        f31 = 1.0f;
                    } else if (f31 < 0.0) {
                        f31 = 0.0f;
                    }
                    f31 = (f31 * (f31 * f31)) * (f31 * (f31 * f31));
                    if (fn_14_5100(b, &spC) != 0) {
                        b->z -= 0.001 * ball->playerId;
                        fn_14_4FFC(a, &sp10);
                        PSVECSubtract(a, b, &sp14);
                        PSVECScale(&sp14, &sp14, f31);
                        PSVECAdd(&sp14, b, &sp14);
                        sp8 = spC + f31 * (sp10 - spC);
                        r5 = 14336.0 * (1.0 - f31);
                        r5 = -r5;
                    } else {
                        sp14 = *a;
                        fn_14_4FFC(&sp14, &sp8);
                        r5 = 0;
                    }
                    fn_14_5240(ball, &sp14, sp8, r5);
                }
            }
            i++;
            ball++;
            pl++;
        }
    }
}

s32 fn_14_4FFC(Vec *pos, float *out)
{
    float h;
    float inv;
    float dist;
    float ratio;

    h = 240.0f * g_current_camera->sub28.viewportH;
    inv = 1.0 / h;
    dist = g_current_camera->sub28.view.tanHalfFov * mathutil_vec_len(pos);
    ratio = h / dist;
    if (ratio < 20.0) {
        *out = 20.0 * dist * inv;
    } else if (ratio > 70.0f) {
        *out = 70.0 * dist * inv;
    } else {
        *out = 1.0f;
    }
    return 1;
}

s32 fn_14_5100(Vec *pos, float *out)
{
    double v;
    double w;
    float x;
    float y;

    if (pos->z > 20.0) {
        return 0;
    }
    *out = 0.3 + (1.0 - pos->z / 20.0);
    if (*out > 1.0) {
        v = 1.0;
    } else {
        v = *out;
    }
    *out = 0.03 * v;
    w = 0.5 * (640.0f * g_current_camera->sub28.viewportW);
    x = w * (1.0 + (lbl_1_bss_2440 * (pos->x * g_current_camera->sub28.view.tanHalfFov)) / g_current_camera->sub28.view.tanHalfFov);
    y = 480.0f * g_current_camera->sub28.viewportH - -10.0;
    pos->z = -0.15f;
    fn_14_12638(g_current_camera, pos, x, y);
    return 1;
}

void fn_14_5240(struct Ball *ball, Vec *pos, float scale, s16 rot)
{
    mtxa_from_identity();
    mathutil_mtxA_set_translate(pos);
    mtxa_rotate_x(rot);
    mtxa_scale_s(scale);
    avdisp_set_scale_factor(scale);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    if (ball->flags & 0x80000000000LL) {
        Vec *c = &lbl_1_data_2E418[ball->playerId];

        set_post_mult_color(c->x, c->y, c->z, 1.0f);
        avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[9].model);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        return;
    }
    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[lbl_14_data_430[ball->playerId]].model);
}

void fn_14_53D4(void)
{
    Mtx sp38;
    Mtx sp8;
    float f31;
    struct StageCollHeader *hdr;
    void *fog;
    struct StageModelRef * DPTR *ref;
    struct StageModelInstance *inst;
    struct GMAModel *model;
    struct ItemGroup *ig;
    s32 i;
    s32 j;
    u32 visMask;
    s32 k;

    if (dip_switches & 1) {
        if (lbl_1_bss_D4328 & 0x20) {
            lbl_14_bss_B0 = 1;
            debug_textdraw_set_pos(1, 1);
        } else {
            lbl_14_bss_B0 = 0;
        }
    }
    fog = world_theme.unk8C;
    if (fog != 0) {
        g_yet_another_unk_draw_func(fog);
    }
    fn_14_59D8(9);
    fn_14_5BF8(8);
    ig = itemgroups;
    hdr = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, hdr++) {
        if ((hdr->animGroupId < 8 || hdr->animGroupId > 0xF) && hdr->animGroupId != 1) {
            mtxa_from_mtxb();
            if (i > 0) {
                mtxa_mult_right(ig->transform);
            }
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            ref = hdr->modelRefs;
            switch (hdr->animGroupId) {
            case 3:
                if (g_current_camera->eye.y <= -3.0) {
                    for (j = 0; j < hdr->modelRefCount; j++, ref++) {
                        model = g_some_draw_func2(*ref);
                        if (model != NULL) {
                            fn_14_5E10(model);
                        }
                    }
                }
                break;
            case 2:
                for (j = 0; j < hdr->modelRefCount; j++, ref++) {
                    model = g_some_draw_func2(*ref);
                    if (model != NULL && fn_14_613C(model) == 0 && lbl_14_bss_1E8(model->boundSphereCenter.y) == 0) {
                        fn_14_5E10(model);
                    }
                }
                break;
            default:
                for (j = 0; j < hdr->modelRefCount; j++, ref++) {
                    model = g_some_draw_func2(*ref);
                    if (model != NULL && lbl_14_bss_1E8(model->boundSphereCenter.y) == 0) {
                        fn_14_5E10(model);
                    }
                }
                break;
            }
            visMask = g_smth_with_challenge_mode_var_and_3p();
            inst = stagedef->collHeaders[i].instances;
            mtxa_push();
            for (j = 0; j < stagedef->collHeaders[i].instanceCount; j++, inst++) {
                if (!(inst->flags & visMask)) {
                    mtxa_peek();
                    mtxa_translate(&inst->pos);
                    mtxa_rotate_z(inst->rotZ);
                    mtxa_rotate_y(inst->rotY);
                    mtxa_rotate_x(inst->rotX);
                    mtxa_scale(&inst->scale);
                    if (inst->scale.x > inst->scale.y) {
                        f31 = inst->scale.x;
                    } else {
                        f31 = inst->scale.y;
                    }
                    f31 = inst->scale.z > f31 ? inst->scale.z : f31;
                    model = g_some_draw_func4(inst->modelRef, f31);
                    if (model != NULL && lbl_14_bss_1E8(inst->pos.y) == 0) {
                        avdisp_set_scale_factor(f31);
                        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                        fn_14_5F18(model, f31);
                    }
                }
            }
            mtxa_pop();
        }
    }
    fn_14_58F8(1);
    fn_14_62A4();
    for (k = 0xA; k <= 0xF; k++) {
        if (k == 0xC) {
            fn_14_5AE0(k);
        } else {
            fn_14_59D8(k);
        }
    }
    if (fog != 0) {
        g_yet_another_unk_draw_func(0);
    }
    if (dip_switches & 1) {
        lbl_14_bss_B0 = 0;
        if (dip_switches & 4) {
            mtx_copy(mtxa->mtxB, sp38);
            mtxa_from_identity();
            mtxa_scale_s(0.99f);
            mtxa_to_mtx(sp8);
            mtxa_from_mtxb();
            mtxa_mult_left(sp8);
            mtxa_to_mtx(mtxa->mtxB);
            g_draw_stage_collision();
            mtx_copy(sp38, mtxa->mtxB);
        }
    }
}

void fn_14_58A0(void)
{
    void *fog = world_theme.unk8C;

    if (fog != 0) {
        g_yet_another_unk_draw_func(fog);
    }
    fn_14_5BF8(8);
    if (fog != 0) {
        g_yet_another_unk_draw_func(0);
    }
}

void fn_14_58F8(s32 group)
{
    struct ItemGroup *ig = itemgroups;
    struct StageCollHeader *hdr = stagedef->collHeaders;
    struct StageModelRef * DPTR *ref;
    s32 i, j;

    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, hdr++) {
        if (hdr->animGroupId == group) {
            mtxa_from_mtxb();
            if (i > 0) {
                mtxa_mult_right(ig->transform);
            }
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            ref = hdr->modelRefs;
            for (j = 0; j < hdr->modelRefCount; j++, ref++) {
                struct GMAModel *model = g_some_draw_func2(*ref);

                if (model != NULL) {
                    fn_14_5E10(model);
                }
            }
        }
    }
}

void fn_14_59D8(s32 group)
{
    struct ItemGroup *ig = itemgroups;
    struct StageCollHeader *hdr = stagedef->collHeaders;
    struct StageModelRef * DPTR *ref;
    struct GMAModel *model;
    s32 i, j;

    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, hdr++) {
        if (hdr->animGroupId == group) {
            mtxa_from_mtxb();
            if (i > 0) {
                mtxa_mult_right(ig->transform);
            }
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            ref = hdr->modelRefs;
            for (j = 0; j < hdr->modelRefCount; j++, ref++) {
                model = g_some_draw_func2(*ref);
                if (model != NULL && lbl_14_bss_1E8(model->boundSphereCenter.y) == 0) {
                    fn_14_5E10(model);
                }
            }
        }
    }
}

void fn_14_5AE0(s32 group)
{
    struct ItemGroup *ig = itemgroups;
    struct StageCollHeader *hdr = stagedef->collHeaders;
    struct StageModelRef * DPTR *ref;
    struct GMAModel *model;
    s32 j;
    s32 i;

    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, hdr++) {
        if (hdr->animGroupId == group) {
            mtxa_from_mtxb();
            if (i > 0) {
                mtxa_mult_right(ig->transform);
            }
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            ref = hdr->modelRefs;
            for (j = 0; j < hdr->modelRefCount; j++, ref++) {
                model = g_some_draw_func2(*ref);
                if (model != NULL && fn_14_613C(model) == 0 && lbl_14_bss_1E8(model->boundSphereCenter.y) == 0) {
                    fn_14_5E10(model);
                }
            }
        }
    }
}

void fn_14_5BF8(s32 group)
{
    float y;
    struct ItemGroup *ig = itemgroups;
    struct StageCollHeader *hdr = stagedef->collHeaders;
    struct StageModelRef * DPTR *ref;
    s32 i, j;

    if (g_current_camera->eye.y > -3.0) {
        y = 300.0f;
    } else if (-13.0 < g_current_camera->eye.y && g_current_camera->eye.y < -3.0) {
        y = 1.0 - (-3.0 - g_current_camera->eye.y) / 10.0;
        y *= 300.0;
    } else {
        y = 0.0f;
    }
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, hdr++) {
        if (hdr->animGroupId == group) {
            mtxa_from_mtxb();
            mtxa_translate_xyz(0.0f, y, 0.0f);
            if (i > 0) {
                mtxa_mult_right(ig->transform);
            }
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            ref = hdr->modelRefs;
            for (j = 0; j < hdr->modelRefCount; j++, ref++) {
                struct GMAModel *model = g_some_draw_func2(*ref);

                if (model != NULL) {
                    avdisp_draw_model_culled_sort_never(model);
                }
            }
        }
    }
}

s32 fn_14_5D9C(float y)
{
    if (g_current_camera->eye.y >= -3.0) {
        if (y < -3.0) {
            return 1;
        }
    } else if (y >= -3.0) {
        return 1;
    }
    return 0;
}

s32 fn_14_5E08(float y)
{
    return 0;
}

void fn_14_5E10(struct GMAModel *model)
{
    s16 i;

    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius) != 0) {
        avdisp_draw_model_unculled_sort_never(model);
        if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x20) && lbl_14_bss_B0 != 0) {
            for (i = 0; i < (s32) stage_gma->numModels; i++) {
                if (stage_gma->modelEntries[i].model == model) {
                    debug_textdraw_vsprintf_buf_1("%s\n", stage_gma->modelEntries[i].name);
                    return;
                }
            }
        }
    }
}

void fn_14_5F18(struct GMAModel *model, float scale)
{
    s16 i;

    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, scale) != 0) {
        avdisp_draw_model_unculled_sort_always(model);
        if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x20) && lbl_14_bss_B0 != 0) {
            for (i = 0; i < (s32) stage_gma->numModels; i++) {
                if (stage_gma->modelEntries[i].model == model) {
                    debug_textdraw_vsprintf_buf_1("%s\n", stage_gma->modelEntries[i].name);
                    return;
                }
            }
        }
    }
}

void fn_14_6024(struct BoatPlayer *pl)
{
    g_some_frame_counter.frameCounter = 60.0f * pl->unk2DC;
}

void fn_14_605C(void)
{
    s16 i;

    if ((g_current_camera->flags & 0x40) || !(lbl_14_bss_208[g_current_camera->idx].unk8 & 0x2000)) {
        return;
    }
    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    for (i = 0; i < 4; i++) {
        if (lbl_14_bss_1F0[i] != NULL) {
            avdisp_draw_model_culled_sort_auto(lbl_14_bss_1F0[i]);
        }
    }
}

s32 fn_14_613C(struct GMAModel *model)
{
    float x = model->boundSphereCenter.x - 30.0;
    float z = model->boundSphereCenter.z - -30.0;
    float camX = g_current_camera->eye.x - 30.0;
    float camZ = g_current_camera->eye.z - -30.0;

    if (camX > 0.0 && camZ > 0.0) {
        if (x < 0.0 && z < 0.0) {
            return 1;
        }
        return 0;
    }
    if (camX < 0.0 && camZ < 0.0) {
        if (x > 0.0 && z > 0.0) {
            return 1;
        }
        return 0;
    }
    if (camX < 0.0 && camZ > 0.0) {
        if (x > 0.0 && z < 0.0) {
            return 1;
        }
        return 0;
    }
    if (camX > 0.0 && camZ < 0.0) {
        if (x < 0.0 && z > 0.0) {
            return 1;
        }
        return 0;
    }
    return 0;
}

void fn_14_62A4(void)
{
    struct BoatPlayer *pl;

    if (g_current_camera->flags & 0x40) {
        pl = lbl_14_bss_208;
    } else {
        pl = &lbl_14_bss_208[g_current_camera->idx];
    }
    if (pl->unk7 == lbl_14_bss_AC->unk1 && pl->unk2DC >= 50.0) {
        if (lbl_14_bss_200 != NULL) {
            mtxa_from_mtxb();
            avdisp_draw_model_unculled_sort_never(lbl_14_bss_200);
        }
    } else if (lbl_14_bss_204 != NULL) {
        mtxa_from_mtxb();
        avdisp_draw_model_unculled_sort_never(lbl_14_bss_204);
    }
}

void fn_14_639C(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    s16 i;

    for (ball = balls, pl = lbl_14_bss_208, i = 0; i < (s32) lbl_14_bss_24; i++, ball++, pl++) {
        if (ball->unk154 <= 4) {
            fn_14_642C(ball);
        }
    }
}

void fn_14_642C(struct Ball *ball)
{
    struct ShadowReq req;
    struct RaycastHit hit;
    Vec sp8;
    static GXColor colors[11] = {
        {0x47, 0x5F, 0x5F, 0xFF},
        {0x5F, 0x57, 0x47, 0xFF},
        {0x47, 0x49, 0x5F, 0xFF},
        {0x5F, 0x4B, 0x5F, 0xFF},
        {0x5F, 0x4B, 0x4B, 0xFF},
        {0x47, 0x5F, 0x4B, 0xFF},
        {0x5C, 0x5A, 0x5A, 0xFF},
        {0x47, 0x47, 0x49, 0xFF},
    };
    float t;
    float r;

    req.unk20 = 0.025f;
    req.unk24 = 0.05f;
    req.model = init_common_gma->modelEntries[0x67].model;
    if ((s32) raycast_stage_down(&ball->pos, &hit, &sp8) != 0) {
        t = 1.0 - 0.2 * (ball->pos.y - hit.pos.y);
        if (!(t < 0.0f)) {
            req.color = colors[ball->colorId];
            req.color.r = req.color.r * t;
            req.color.g = req.color.g * t;
            req.color.b = req.color.b * t;
            r = ball->currRadius * (1.0f + 2.0f * (1.0f - t));
            req.scale.x = r;
            req.scale.y = req.scale.x;
            req.scale.z = req.scale.x;
            vec_to_euler(&hit.normal, &req.rot);
            req.rot.z = ball->rotZ;
            req.pos = hit.pos;
            submit_shadow_req(&req);
        }
    }
}

void fn_14_65E4(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    s16 i;

    for (i = 0, ball = balls, pl = lbl_14_bss_208; i < 8; i++, ball++, pl++) {
        g_maybe_sets_number_of_starting_monkeys(ball);
        g_reset_ball(ball);
        memset(pl, 0, sizeof(*pl));
        pl->unk0 = (u8) i;
        pl->unk1 = (s8) player_pad_map[i];
    }
    for (i = 0; i < 8; i++) {
        lbl_14_bss_2988[i] = &lbl_14_bss_208[i];
    }
}

