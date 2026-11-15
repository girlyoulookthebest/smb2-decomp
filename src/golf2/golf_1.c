/* Monkey Golf 2: ball state machine and ball physics (tee-up, shot, roll, stage collision). */
#include "golf.h"

/* DOL / main_loop */
extern u16 lbl_1_bss_6FB8C;  /* debug: show the stage/gravity scale */
void debug_textdraw_set_pos(int x, int y);
int debug_textdraw_vsprintf_buf_0(char *fmt, ...);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void call_SoundReqID_arg_0(int soundId);
void g_rumble_controller(s32 pad, s32 a, s32 b);
void g_smth_with_ape_rotation(struct Ape *ape, Vec *dir);
void fn_1_4AF48(struct Ball *ball, struct PhysicsBall *phys);
void position_ball(struct Ball *ball, struct PhysicsBall *phys);
void fn_1_45CA8(struct Ape *ape);
void fn_1_D2510(struct Ape *ape);
BOOL did_ball_fallout(struct Ball *ball);
BOOL g_is_ball_in_ig_coli_range(struct ItemGroup *ig, struct StageCollHeader *coll, int igId, struct Sphere *sphere);
s16 *tri_coli_grid_lookup(struct StageCollHeader *coll, float x, float z);
void collide_ball_with_tri_face(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_tri_edges(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_tri_vertices(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_cylinder(struct PhysicsBall *ball, struct StageColiCylinder *cylinder);
void collide_ball_with_sphere(struct PhysicsBall *ball, struct StageColiSphere *sphere);
void collide_ball_with_cone(struct PhysicsBall *ball, struct StageColiCone *cone);
void collide_ball_with_goal(struct PhysicsBall *ball, struct StageGoal *goal);
void collide_ball_with_plane(struct PhysicsBall *ball, struct ColiPlane *plane);
s16 g_math_unk4(float x);
void ray_scale(Vec *a, Vec *b, Vec *out, float t);
void mtxa_to_quat(Quat *q);

/* .data */
u8 lbl_11_data_880[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};
s16 lbl_11_data_9F0 = -1;  /* course the start positions were last set for */
float lbl_11_data_9F4 = 0.009799992f;  /* gravity (per frame^2) */

/* .bss */
u32 lbl_11_bss_29F8;  /* frame counter value at the last rolling update */
u32 lbl_11_bss_29FC;  /* only ever cleared (by the shot preview) */
u32 lbl_11_bss_2A00;  /* unused */
u32 lbl_11_bss_2A04;  /* consecutive rolling frames */
u32 lbl_11_bss_2A08;  /* frames in the hover/drop state */
u32 lbl_11_bss_2A0C;  /* frames the ball has been nearly still */
u32 lbl_11_bss_2A10;  /* player whose ball the stillness count is for */
struct RaycastHit lbl_11_bss_2A14;  /* ground under the ball (flags: 2 green, 4 rough, 8 bunker, ...) */
float lbl_11_bss_2A30;  /* ball speed last frame */
u32 lbl_11_bss_2A34;
u32 lbl_11_bss_2A38;
u32 lbl_11_bss_2A3C;
float lbl_11_bss_2A40;

void fn_11_174C8(struct Ball *ball);
void fn_11_178A4(struct Ball *ball);
void fn_11_17944(struct Ball *ball);
void fn_11_17C04(struct Ball *ball);
void fn_11_17E50(struct Ball *ball);
void fn_11_17F84(struct Ball *ball);
void fn_11_19004(struct Ball *ball, struct PhysicsBall *phys);

/* Per-frame ball callback: debug display of the scale setting, then the ball's golf state. */
void fn_11_171E8(struct Ball *ball)
{
    /* Unused: only references lbl_11_bss_29FC/2A00 early enough to keep the .bss order
       (MWCC lays out .bss in order of first reference). */
    u32 *bssOrder[3];

    ball->accel = 0.0f;
    lbl_11_data_9F4 = 0.01088888f;
    if (lbl_1_bss_6FB8C != 0) {
        debug_textdraw_set_pos(1, 32);
        switch (fn_11_EB18()) {
        case 0: debug_textdraw_vsprintf_buf_1("Stage: REAL\nG: x1/6"); break;
        case 1: debug_textdraw_vsprintf_buf_1("Stage: x1/12\nG: x1/6"); break;
        case 2: debug_textdraw_vsprintf_buf_1("Stage: x1/6\nG: x1/6"); break;
        case 3: debug_textdraw_vsprintf_buf_1("Stage: x1/24\nG: x1/6"); break;
        case 4: debug_textdraw_vsprintf_buf_1("Stage: REAL\nG: x1/12"); break;
        case 5: debug_textdraw_vsprintf_buf_1("Stage: x1/12\nG: x1/12"); break;
        case 6: debug_textdraw_vsprintf_buf_1("Stage: x1/6\nG: x1/12"); break;
        case 7: debug_textdraw_vsprintf_buf_1("Stage: x1/24\nG: x1/12"); break;
        case 8: debug_textdraw_vsprintf_buf_1("Stage: REAL\nG: x1/24"); break;
        case 9: debug_textdraw_vsprintf_buf_1("Stage: x1/12\nG: x1/24"); break;
        case 10: debug_textdraw_vsprintf_buf_1("Stage: x1/6\nG: x1/24"); break;
        case 11: debug_textdraw_vsprintf_buf_1("Stage: x1/24\nG: x1/24"); break;
        case 12: debug_textdraw_vsprintf_buf_1("Stage: REAL\nG: REAL"); break;
        case 13: debug_textdraw_vsprintf_buf_1("Stage: x1/12\nG: REAL"); break;
        case 14: debug_textdraw_vsprintf_buf_1("Stage: x1/6\nG: REAL"); break;
        case 15: debug_textdraw_vsprintf_buf_1("Stage: x1/24\nG: REAL"); break;
        case 16:
        case 17:
            break;
        }
    }
    switch (ball->unk154) {
    case 1:
        lbl_11_bss_29F8 = 0;
        bssOrder[1] = &lbl_11_bss_29FC;
        bssOrder[2] = &lbl_11_bss_2A00;
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_2A08 = 0;
        fn_11_174C8(ball);
        break;
    case 2:
        lbl_11_bss_29F8 = 0;
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_2A08 = 0;
        fn_11_178A4(ball);
        break;
    case 3:
        lbl_11_bss_29F8 = 0;
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_2A08 = 0;
        fn_11_17E50(ball);
        break;
    case 4:
        lbl_11_bss_2A08 = 0;
        fn_11_17F84(ball);
        break;
    case 5:
        lbl_11_bss_29F8 = 0;
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_2A0C = 0;
        fn_11_17944(ball);
        break;
    case 6:
        lbl_11_bss_29F8 = 0;
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_2A0C = 0;
        fn_11_17C04(ball);
        break;
    case 0:
        lbl_11_bss_29F8 = 0;
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_2A0C = 0;
        lbl_11_bss_2A08 = 0;
        break;
    default:
        lbl_11_bss_29F8 = 0;
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_2A0C = 0;
        lbl_11_bss_2A08 = 0;
        break;
    }
}

/* Tee-up: put the ball (and, on a new course, every player's ball) on the start position. */
void fn_11_174C8(struct Ball *ball)
{
    Vec dir;
    int i;

    if (lbl_11_data_9F0 != fn_11_EB18()) {
        for (i = 0; i < modeCtrl.playerCount; i++) {
            balls[i].pos = balls[i].prevPos = stagedef->startPos->pos;
        }
        lbl_11_data_9F0 = fn_11_EB18();
    }
    ball->pos.x = stagedef->startPos->pos.x;
    ball->pos.y = stagedef->startPos->pos.y
                + 0.5 * ((0.5 * lbl_80198B38) * (lbl_11_data_9F4 * (0.5 * lbl_80198B38)));
    ball->pos.z = stagedef->startPos->pos.z;
    ball->prevPos = ball->pos;
    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);
    ball->speed = ball->unkCC = 0.0f;
    {
        Vec v0 = {0.0f, 0.0f, 0.0f};
        Vec *v = &v0;

        ball->unkC0 = *v;
    }
    if (ball->ape != NULL) {
        ball->ape->flags &= ~0x4000;
    }
    {
        Quat q0 = {0.0f, 0.0f, 0.0f, 1.0f};
        Quat *q = &q0;

        ball->unkB0 = *q;
    }
    ball->unkA0 = ball->unkB0;
    ball->rotX = 0x2000;
    if (stagedef != NULL) {
        ball->rotY = stagedef->startPos->yrot - 0x4000;
    } else {
        ball->rotY = 0;
    }
    ball->rotZ = 0;
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
    ball->unk92 = stagedef->startPos->yrot;
    dir.x = -math_sin(ball->unk92);
    dir.y = 0.0f;
    dir.z = -math_sin(ball->unk92 + 0x4000);
    if (ball->ape != NULL) {
        g_smth_with_ape_rotation(ball->ape, &dir);
    }
    fn_11_EB5C(&ball->prevPos);
    fn_11_EB38(&ball->pos);
    fn_11_EBCC(&ball->unk92);
    fn_11_EBA4(&ball->unk92);
    ball->colorId = modeCtrl.currPlayer;
    lbl_11_bss_2A0C = 0;
    lbl_11_bss_2A10 = 9;
    ball->state = 26;
    ball->unk154 = 4;
}

/* Re-tee from the current position: remember it and give the ball a tiny downward push. */
void fn_11_178A4(struct Ball *ball)
{
    ball->unk92 = stagedef->startPos->yrot;
    fn_11_EB5C(&ball->prevPos);
    fn_11_EB38(&ball->pos);
    fn_11_EBCC(&ball->unk92);
    fn_11_EBA4(&ball->unk92);
    ball->vel.x = 0.0f;
    ball->vel.y = -1e-08f;
    ball->vel.z = 0.0f;
    ball->state = 26;
    ball->unk154 = 4;
}

/* Hover over the tee, then drop the ball and spin the monkey (after a long wait set by the score). */
void fn_11_17944(struct Ball *ball)
{
    struct PhysicsBall phys;
    u32 limit;

    switch (fn_11_16FD8()) {
    case -10:
        limit = 540;
        break;
    case -3:
        limit = 360;
        break;
    case -2:
        limit = 360;
        break;
    case -1:
        limit = 240;
        break;
    case 0:
        limit = 240;
        break;
#ifndef __MWERKS__
    default:
        /* GC leaves it uninitialised (the caller's ball pointer), so the hover never ends */
        limit = 0xFFFFFFFF;
        break;
#endif
    }
    if (lbl_11_bss_2A08 < limit) {
        if (ball->ape != NULL) {
            ball->ape->flags &= ~0x4000;
        }
        ball->prevPos.y = ball->pos.y;
        ball->pos.y = 2.0 + lbl_11_bss_38.y + 0.5 * math_sin(g_font_char_var_3 << 10);
        ball->vel.y = ball->pos.y - ball->prevPos.y;
        init_physicsball_from_ball(ball, &phys);
    } else if (lbl_11_bss_2A08 == limit) {
        if (ball->ape != NULL) {
            ball->ape->flags |= 0x4000;
        }
        ball->flags |= 0x40;
        cameras[modeCtrl.currPlayer].unk1F = 8;
        init_physicsball_from_ball(ball, &phys);
    } else {
        if (ball->ape != NULL) {
            mtxa_from_quat(&ball->ape->rot);
            mtxa_rotate_y(0x800);
            mtxa_to_quat(&ball->ape->rot);
            if (!(ball->flags & 0x200) && (ball->ape->flags & 0x4000)) {
                ball->flags &= ~0x500;
                ball->flags |= 0x200;
                call_SoundReqID_arg_0(0x126);
            }
        }
        fn_11_19004(ball, &phys);
    }
    lbl_11_bss_2A08++;
    fn_1_4AF48(ball, &phys);
    position_ball(ball, &phys);
    ball->unk80++;
}

/* Same as fn_11_17944 with a fixed one second hover (camera 0). */
void fn_11_17C04(struct Ball *ball)
{
    struct PhysicsBall phys;
    u32 limit = 60;

    if (lbl_11_bss_2A08 < limit) {
        if (ball->ape != NULL) {
            ball->ape->flags &= ~0x4000;
        }
        ball->prevPos.y = ball->pos.y;
        ball->pos.y = 2.0 + lbl_11_bss_38.y + 0.5 * math_sin(g_font_char_var_3 << 10);
        ball->vel.y = ball->pos.y - ball->prevPos.y;
        init_physicsball_from_ball(ball, &phys);
    } else if (lbl_11_bss_2A08 == limit) {
        if (ball->ape != NULL) {
            ball->ape->flags |= 0x4000;
        }
        ball->flags |= 0x40;
        cameras[0].unk1F = 8;
        init_physicsball_from_ball(ball, &phys);
    } else if (ball->ape != NULL) {
        mtxa_from_quat(&ball->ape->rot);
        mtxa_rotate_y(0x800);
        mtxa_to_quat(&ball->ape->rot);
        if (!(ball->flags & 0x200) && (ball->ape->flags & 0x4000)) {
            ball->flags &= ~0x500;
            ball->flags |= 0x200;
            call_SoundReqID_arg_0(0x126);
        }
        fn_11_19004(ball, &phys);
    }
    lbl_11_bss_2A08++;
    fn_1_4AF48(ball, &phys);
    position_ball(ball, &phys);
    ball->unk80++;
}

/* Back to the saved position (after a fallout or OB), dropped from a little height. */
void fn_11_17E50(struct Ball *ball)
{
    Vec dir;

    ball->vel.x = 0.0f;
    ball->vel.y = -1e-07f;
    ball->vel.z = 0.0f;
    ball->unk80 = 0;
    ball->unkA0.w = 1.0f;
    ball->unkA0.x = 0.0f;
    ball->unkA0.y = 0.0f;
    ball->unkA0.z = 0.0f;
    fn_11_EB80(&ball->pos);
    ball->pos.y += 0.5 * (24.0 * (24.0 * lbl_11_data_9F4));
    ball->prevPos = ball->pos;
    fn_11_EB38(&ball->pos);
    fn_11_EBB8(&ball->unk92);
    dir.x = -math_sin(ball->unk92);
    dir.y = 0.0f;
    dir.z = -math_sin(ball->unk92 + 0x4000);
    if (ball->ape != NULL) {
        g_smth_with_ape_rotation(ball->ape, &dir);
    }
    ball->state = 26;
    ball->unk154 = 4;
}

/* Rolling: physics step, tee/landing fixes, bounce sounds and the "ball has stopped" check. */
void fn_11_17F84(struct Ball *ball)
{
    struct PhysicsBall phys;

    ball->flags |= 0x100000000LL;
    if (g_font_char_var_3 - lbl_11_bss_29F8 != 1) {
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_29F8 = g_font_char_var_3;
        ball->unk80 = 0;
    } else {
        lbl_11_bss_2A04++;
        lbl_11_bss_29F8 = g_font_char_var_3;
    }
    fn_11_19004(ball, &phys);
    if (ball->pos.x == stagedef->startPos->pos.x
     && ball->pos.z == stagedef->startPos->pos.z
     && ball->pos.y < 0.5 + stagedef->startPos->pos.y
     && fn_11_EB28()) {
        ball->vel.y *= -0.45;
        if (__fabs(ball->vel.y) < 0.01) {
            ball->vel.y = 0.0f;
        }
        ball->pos.y = 0.5 + stagedef->startPos->pos.y;
        if (__fabs(ball->vel.y) > 0.1) {
            g_rumble_controller(player_pad_map[modeCtrl.currPlayer], 1, 30);
        }
    }
    if (fn_11_171C8()) {
        ball->pos.x = stagedef->startPos->pos.x;
        ball->pos.y = 0.5f + stagedef->startPos->pos.y;
        ball->pos.z = stagedef->startPos->pos.z;
    }
    if (!fn_11_EB28() && lbl_11_bss_80 > -1) {
        if (math_sqrt(mathutil_sum_of_sq_3(ball->pos.x - stagedef->startPos->pos.x,
                                           ball->pos.z - stagedef->startPos->pos.z, 0.0f)) < 0.5) {
            ball->pos.y = 0.5 + stagedef->startPos->pos.y;
        }
    }
    fn_1_4AF48(ball, &phys);
    position_ball(ball, &phys);
    if (ball->ape != NULL) {
        fn_1_45CA8(ball->ape);
    }
    if ((ball->flags & 4) && !fn_11_EB28()) {
        float speed = mathutil_vec_len(&ball->vel);

        if (speed < 0.1f) {
            call_SoundReqID_arg_0(0x17);
        } else if (speed < 0.3f) {
            call_SoundReqID_arg_0(0x18);
        } else {
            call_SoundReqID_arg_0(0x1A);
        }
        if (lbl_11_bss_2A14.flags & 2) {
            call_SoundReqID_arg_0(0x365);
        } else if (lbl_11_bss_2A14.flags & 4) {
            call_SoundReqID_arg_0(0x366);
        } else if (lbl_11_bss_2A14.flags & 8) {
            call_SoundReqID_arg_0(0x367);
        } else {
            call_SoundReqID_arg_0(0x365);
        }
    }
    if (cameras[modeCtrl.currPlayer].unk1F == 7) {
        ball->unk154 = 0;
        lbl_11_bss_2A0C = 0;
        lbl_11_bss_29F8 = 0;
        lbl_11_bss_2A04 = 0;
        lbl_11_bss_2A10 = 9;
    }
    if (lbl_11_bss_2A10 == modeCtrl.currPlayer) {
        if (lbl_11_bss_2A30 <= 0.005) {
            lbl_11_bss_2A0C++;
        } else {
            lbl_11_bss_2A0C = 0;
        }
        lbl_11_bss_2A30 = mathutil_vec_len(&ball->vel);
        if (lbl_11_bss_2A0C > 60) {
            ball->vel.x = 0.0f;
            ball->vel.y = 0.0f;
            ball->vel.z = 0.0f;
            ball->unk154 = 0;
            lbl_11_bss_2A0C = 0;
            lbl_11_bss_29F8 = 0;
            lbl_11_bss_2A04 = 0;
            lbl_11_bss_2A10 = 9;
        }
        if (fn_11_EB28()) {
            ball->pos.x = ball->prevPos.x;
            ball->pos.z = ball->prevPos.z;
            ball->vel.x = 0.0f;
            ball->vel.z = 0.0f;
        }
    } else {
        lbl_11_bss_2A10 = modeCtrl.currPlayer;
        lbl_11_bss_2A0C = 0;
    }
    ball->unk80++;
}

/* Stage collision from the previous position: faces, cones, spheres and cylinders, then edges, then vertices. */
void fn_11_184DC(struct PhysicsBall *ball, struct StageDef *stagedef)
{
    int j;
    s16 *faceIdx;
    struct StageColiCone *cone;
    struct StageColiTri *tri;
    struct StageCollHeader *coll;
    int k;
    int i;
    struct StageColiSphere *sphere;
    s16 *cellTris;
    struct StageColiCylinder *cylinder;
    s16 *cellTriIdx;

    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, coll++) {
        if (i != ball->itemGroupId) {
            tf_physball_to_itemgroup_space(ball, i);
        }
        cellTris = tri_coli_grid_lookup(coll, ball->prevPos.x, ball->prevPos.z);
        if (cellTris != NULL) {
            for (faceIdx = cellTris; *faceIdx >= 0; faceIdx++) {
                tri = &coll->triangles[*faceIdx];
                collide_ball_with_tri_face(ball, tri);
            }
        }
        cone = coll->coliCones;
        for (k = coll->coliConeCount; k > 0; k--, cone++) {
            collide_ball_with_cone(ball, cone);
        }
        sphere = coll->coliSpheres;
        for (j = coll->coliSphereCount; j > 0; j--, sphere++) {
            collide_ball_with_sphere(ball, sphere);
        }
        cylinder = coll->coliCylinders;
        for (j = coll->coliCylinderCount; j > 0; j--, cylinder++) {
            collide_ball_with_cylinder(ball, cylinder);
        }
    }
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, coll++) {
        if (i != ball->itemGroupId) {
            tf_physball_to_itemgroup_space(ball, i);
        }
        cellTris = tri_coli_grid_lookup(coll, ball->prevPos.x, ball->prevPos.z);
        if (cellTris != NULL) {
            for (cellTriIdx = cellTris; *cellTriIdx >= 0; cellTriIdx++) {
                tri = &coll->triangles[*cellTriIdx];
                collide_ball_with_tri_edges(ball, tri);
            }
        }
    }
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, coll++) {
        if (i != ball->itemGroupId) {
            tf_physball_to_itemgroup_space(ball, i);
        }
        cellTris = tri_coli_grid_lookup(coll, ball->prevPos.x, ball->prevPos.z);
        if (cellTris != NULL) {
            for (cellTriIdx = cellTris; *cellTriIdx >= 0; cellTriIdx++) {
                tri = &coll->triangles[*cellTriIdx];
                collide_ball_with_tri_vertices(ball, tri);
            }
        }
    }
    if (ball->itemGroupId != 0) {
        tf_physball_to_itemgroup_space(ball, 0);
    }
}
/* Goes to fail unless the triangle-space point p is inside the triangle, with the edges and
   corners pushed out by the ball radius. */
#define GOLF_TRI_CHECK(p, fail)                                                         \
    nx = 0.0f;                                                                          \
    ny = 1.0f;                                                                          \
    if ((dx = (p).x, dy = (p).y, nx * dx + ny * dy) < -ball->radius)                    \
        goto fail;                                                                      \
    nx = tri->edge2Normal.x;                                                            \
    ny = tri->edge2Normal.y;                                                            \
    if ((dx = (p).x - tri->vert2.x, dy = (p).y - tri->vert2.y, nx * dx + ny * dy) < -ball->radius)\
        goto fail;                                                                      \
    nx = tri->edge3Normal.x;                                                            \
    ny = tri->edge3Normal.y;                                                            \
    if ((dx = (p).x - tri->vert3.x, dy = (p).y - tri->vert3.y, nx * dx + ny * dy) < -ball->radius)\
        goto fail;                                                                      \
    nx = 0.0 + tri->edge3Normal.x;                                                      \
    ny = 1.0 + tri->edge3Normal.y;                                                      \
    len2 = nx * nx + ny * ny;                                                           \
    if (len2 < FLT_EPSILON) {                                                           \
        nx = tri->vert2.x;                                                              \
        ny = tri->vert2.y;                                                              \
        len2 = nx * nx + ny * ny;                                                       \
    }                                                                                   \
    inv = 1.0 / math_sqrt(len2);                                                        \
    nx *= inv;                                                                          \
    ny *= inv;                                                                          \
    if ((dx = (p).x, dy = (p).y, nx * dx + ny * dy) < -ball->radius)                    \
        goto fail;                                                                      \
    nx = 0.0 + tri->edge2Normal.x;                                                      \
    ny = 1.0 + tri->edge2Normal.y;                                                      \
    len2 = nx * nx + ny * ny;                                                           \
    if (len2 < FLT_EPSILON) {                                                           \
        nx = -tri->vert2.x;                                                             \
        ny = -tri->vert2.y;                                                             \
        len2 = nx * nx + ny * ny;                                                       \
    }                                                                                   \
    inv = 1.0 / math_sqrt(len2);                                                        \
    nx *= inv;                                                                          \
    ny *= inv;                                                                          \
    if ((dx = (p).x - tri->vert2.x, dy = (p).y - tri->vert2.y, nx * dx + ny * dy) < -ball->radius)\
        goto fail;                                                                      \
    nx = tri->edge2Normal.x + tri->edge3Normal.x;                                       \
    ny = tri->edge2Normal.y + tri->edge3Normal.y;                                       \
    len2 = nx * nx + ny * ny;                                                           \
    if (len2 < FLT_EPSILON) {                                                           \
        nx = -tri->vert3.x;                                                             \
        ny = -tri->vert3.y;                                                             \
        len2 = nx * nx + ny * ny;                                                       \
    }                                                                                   \
    inv = 1.0 / math_sqrt(len2);                                                        \
    nx *= inv;                                                                          \
    ny *= inv;                                                                          \
    if ((dx = (p).x - tri->vert3.x, dy = (p).y - tri->vert3.y, nx * dx + ny * dy) < -ball->radius)\
        goto fail;

/* Ball vs. one triangle face: collide with its plane when the ball crossed it inside the (rounded) triangle. */
void fn_11_18720(struct PhysicsBall *ball, struct StageColiTri *tri)
{
    struct ColiPlane plane;
    Vec pos;
    Vec prevPos;
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float dx;
    float dy;
    float len2;
    float inv;

    prevPos = ball->prevPos;
    pos = ball->pos;
    x = prevPos.x - tri->pos.x;
    y = prevPos.y - tri->pos.y;
    z = prevPos.z - tri->pos.z;
    if (z * tri->normal.z + (x * tri->normal.x + y * tri->normal.y) < 0.0) {
        return;
    }
    x = pos.x - tri->pos.x;
    y = pos.y - tri->pos.y;
    z = pos.z - tri->pos.z;
    if (z * tri->normal.z + (x * tri->normal.x + y * tri->normal.y) > ball->radius) {
        return;
    }
    mtxa_from_translate(&tri->pos);
    mtxa_rotate_y(tri->rot.y);
    mtxa_rotate_x(tri->rot.x);
    mtxa_rotate_z(tri->rot.z);
    mtxa_rigid_inv_tf_point(&prevPos, &prevPos);
    mtxa_rigid_inv_tf_point(&pos, &pos);
    GOLF_TRI_CHECK(prevPos, check_pos)
    goto hit;
check_pos:
    GOLF_TRI_CHECK(pos, end)
hit:
    plane.point = tri->pos;
    plane.normal = tri->normal;
    plane.flags = tri->flags;
    collide_ball_with_plane(ball, &plane);
end:
    ;
}

/* Stage collision (current position): golf triangle faces, then cones, spheres, cylinders and goals. */
void fn_11_18D78(struct PhysicsBall *ball, struct StageDef *stagedef)
{
    struct StageColiTri *tri;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    struct StageColiCone *cone;
    s16 *cellTriIdx;
    int k;
    int i;
    struct StageColiCylinder *cylinder;
    struct Sphere sphere;
    s16 *cellTris;
    struct StageColiSphere *coliSphere;
    struct StageGoal *goal;
    int j;

    sphere.pos = ball->pos;
    sphere.radius = ball->radius;

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++) {
        if (!g_is_ball_in_ig_coli_range(ig, coll, i, &sphere)) {
            continue;
        }
        if (i != ball->itemGroupId) {
            tf_physball_to_itemgroup_space(ball, i);
        }
        cellTris = tri_coli_grid_lookup(coll, ball->pos.x, ball->pos.z);
        if (cellTris != NULL) {
            for (cellTriIdx = cellTris; *cellTriIdx >= 0; cellTriIdx++) {
                tri = &coll->triangles[*cellTriIdx];
                fn_11_18720(ball, tri);
            }
        }
    }

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++) {
        if (coll->coliCones == NULL && coll->coliSpheres == NULL && coll->coliCylinders == NULL
         && coll->goals == NULL) {
            continue;
        }
        if (!g_is_ball_in_ig_coli_range(ig, coll, i, &sphere)) {
            continue;
        }
        if ((g_some_frame_counter.flags & (1 << 1))
         && (coll->unkA6 & (1 << (g_some_frame_counter.unkC - 1))) != 0) {
            continue;
        }
        if (i != ball->itemGroupId) {
            tf_physball_to_itemgroup_space(ball, i);
        }
        cone = coll->coliCones;
        for (k = coll->coliConeCount; k > 0; k--, cone++) {
            collide_ball_with_cone(ball, cone);
        }
        coliSphere = coll->coliSpheres;
        for (j = coll->coliSphereCount; j > 0; j--, coliSphere++) {
            collide_ball_with_sphere(ball, coliSphere);
        }
        cylinder = coll->coliCylinders;
        for (j = coll->coliCylinderCount; j > 0; j--, cylinder++) {
            collide_ball_with_cylinder(ball, cylinder);
        }
        goal = coll->goals;
        for (j = coll->goalCount; j > 0; j--, goal++) {
            collide_ball_with_goal(ball, goal);
        }
    }

    if (ball->itemGroupId != 0) {
        tf_physball_to_itemgroup_space(ball, 0);
    }
}

/* One physics step of the golf ball: gravity, stage collision, air drag/spin/wind in the air,
   surface response (green, rough, bunker, fairway) on landing, and slope friction. */
void fn_11_19004(struct Ball *ball, struct PhysicsBall *phys)
{
    Vec up;
    Vec toCup;
    float gx;
    float gy;
    float gz;
    float speed;
    float dist;
    int slope;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20;
    mtxa_from_identity();
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    mtxa_tf_vec(&up, &up);
    gx = 0.0f;
    gy = -lbl_11_data_9F4;
    gz = 0.0f;
    if (ball->flags & 0x200) {
        gy = -gy;
    } else if (ball->flags & 0x100) {
        gy = 0.0f;
    }
    ball->vel.x += gx;
    ball->vel.y += gy;
    ball->vel.z += gz;
    ball->pos.x += ball->vel.x;
    ball->pos.y += ball->vel.y;
    ball->pos.z += ball->vel.z;
    init_physicsball_from_ball(ball, phys);
    phys->restitution = 0.3f;
    fn_11_184DC(phys, stagedef);
    apply_physicsball_to_ball(ball, phys);

    if (!(phys->flags & 1)) {
        /* in the air */
        if (ball->vel.y > 0.0f) {
            ball->vel.x *= 0.995;
            ball->vel.y *= 0.995;
            ball->vel.z *= 0.995;
        } else {
            ball->vel.x *= 0.998;
            ball->vel.y *= 0.998;
            ball->vel.z *= 0.998;
        }
        if (ball->vel.y > 0.0f) {
            ball->vel.x *= 0.995;
            ball->vel.y *= 1.0001;
            ball->vel.z *= 0.995;
        }
        if (lbl_11_bss_184 != 0.0) {
            speed = mathutil_vec_len(&ball->vel);
            ball->vel.x *= 1.0 + 0.003 * lbl_11_bss_184;
            ball->vel.y *= 1.0 - 0.01 * lbl_11_bss_184;
            ball->vel.z *= 1.0 + 0.003 * lbl_11_bss_184;
            speed /= mathutil_vec_len(&ball->vel);
            speed *= 0.9995;
            ball->vel.x *= speed;
            ball->vel.y *= speed;
            ball->vel.z *= speed;
        }
        mtxa_from_identity();
        mtxa_rotate_y(128.0f * -lbl_11_bss_180);
        mtxa_tf_vec(&ball->vel, &ball->vel);
        lbl_11_bss_180 *= 0.995f;
        lbl_11_bss_184 *= 0.995f;
        if (!(ball->flags & 0x200) && !(ball->flags & 0x100)
         && lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14) {
            ball->vel.x -= golf_wind_velocity * math_sin(golf_wind_angle) / 6000.0;
            ball->vel.z -= golf_wind_velocity * math_sin(golf_wind_angle + 0x4000) / 6000.0;
        }
    }

    if (phys->flags & 1) {
        /* touching the ground */
        speed = mathutil_vec_len(&ball->vel);
        if (lbl_1_bss_6FB8C != 0) {
            debug_textdraw_set_pos(10, 10);
        }
        if ((s32)raycast_stage_down(&ball->pos, &lbl_11_bss_2A14, NULL) != 0) {
            if (lbl_11_bss_2A14.flags & 2) {
                lbl_11_bss_7C = 1;
                toCup.x = lbl_11_bss_38.x - ball->pos.x;
                toCup.y = lbl_11_bss_38.y - ball->pos.y;
                toCup.z = lbl_11_bss_38.z - ball->pos.z;
                if (speed > 0.01) {
                    dist = mathutil_vec_len(&toCup);
                    vec_normalize_len(&toCup);
                    vec_normalize_len(&ball->vel);
                    if (30.0f > dist && dist > 5.0f && vec_dot_normalized(&toCup, &ball->vel) > 0.985) {
                        ray_scale(&ball->vel, &toCup, &ball->vel, 0.03f);
                        vec_normalize_len(&ball->vel);
                    }
                    if (lbl_11_bss_4C >= 12) {
                        ball->vel.x *= speed;
                        ball->vel.y *= 0.99 * speed;
                        ball->vel.z *= speed;
                    } else {
                        ball->vel.x *= 0.99 * speed;
                        ball->vel.y *= 0.9 * speed;
                        ball->vel.z *= 0.99 * speed;
                    }
                }
                if (lbl_1_bss_6FB8C != 0) {
                    debug_textdraw_vsprintf_buf_0("GREEN                   ");
                }
            } else if (lbl_11_bss_2A14.flags & 4) {
                float p = fn_11_16ACC(math_sin(abs(g_math_unk4(vec_dot_normalized(&lbl_11_bss_2A14.normal, &ball->vel)) - 0x4000) + 0x4000), 30);

                ball->vel.x *= 0.5 + 0.46 * p;
                ball->vel.y *= 0.5 + 0.46 * p;
                ball->vel.z *= 0.5 + 0.46 * p;
                if (lbl_1_bss_6FB8C != 0) {
                    debug_textdraw_vsprintf_buf_0("ROUGH                   ");
                }
            } else if (lbl_11_bss_2A14.flags & 8) {
                float p = fn_11_16ACC(math_sin(abs(g_math_unk4(vec_dot_normalized(&lbl_11_bss_2A14.normal, &ball->vel)) - 0x4000) + 0x4000), 30);

                ball->vel.x *= 0.93 * p;
                ball->vel.y *= 0.93 * p;
                ball->vel.z *= 0.93 * p;
                if (lbl_1_bss_6FB8C != 0) {
                    debug_textdraw_vsprintf_buf_0("BUNK %lf               ", p);
                }
            } else if (math_sqrt(mathutil_sum_of_sq_3(ball->pos.x - lbl_11_bss_38.x,
                                                      ball->pos.y - lbl_11_bss_38.y,
                                                      ball->pos.z - lbl_11_bss_38.z)) > 3.0) {
                float p = fn_11_16ACC(math_sin(abs(g_math_unk4(vec_dot_normalized(&lbl_11_bss_2A14.normal, &ball->vel)) - 0x4000) + 0x4000), 30);

                ball->vel.x *= 0.59 + 0.38 * p;
                ball->vel.y *= 0.59 + 0.38 * p;
                ball->vel.z *= 0.59 + 0.38 * p;
                if (lbl_1_bss_6FB8C != 0) {
                    debug_textdraw_vsprintf_buf_0("Fairway                ");
                }
            } else if (lbl_1_bss_6FB8C != 0) {
                debug_textdraw_vsprintf_buf_0("Cup                    ");
            }
            if (lbl_11_bss_2A14.flags & 2) {
                lbl_11_bss_7C = 1;
            } else {
                lbl_11_bss_7C = 0;
            }
        } else {
            if (lbl_1_bss_6FB8C != 0) {
                debug_textdraw_vsprintf_buf_0("Air or Fall              ");
            }
            lbl_11_bss_7C = 0;
        }
    }

    if (phys->flags & 1) {
        if (phys->hardestColiItemGroupId == 0) {
            ball->unk11C.x = -phys->hardestColiNormal.x;
            ball->unk11C.y = -phys->hardestColiNormal.y;
            ball->unk11C.z = -phys->hardestColiNormal.z;
        } else {
            mtxa_from_mtx(itemgroups[phys->hardestColiItemGroupId].transform);
            mtxa_tf_vec(&phys->hardestColiNormal, &ball->unk11C);
            ball->unk11C.x = -ball->unk11C.x;
            ball->unk11C.y = -ball->unk11C.y;
            ball->unk11C.z = -ball->unk11C.z;
        }
    }
    slope = __fabs(math_atan(math_sqrt(mathutil_sum_of_sq_3(ball->unk11C.x, 0.0f, ball->unk11C.z)) / ball->unk11C.y));
    if (lbl_11_bss_2A04 > 60 && ball->unk80 > 60 && slope < 0x1000
     && mathutil_vec_len(&ball->vel) < 0.1 && (phys->flags & 1)) {
        float decel = 0.173648177 * lbl_11_data_9F4;
        float h2 = mathutil_sum_of_sq_2(ball->vel.x, ball->vel.z);

        if (h2 < decel * decel) {
            ball->vel.x = 0.0f;
            ball->vel.y = 0.0f;
            ball->vel.z = 0.0f;
            ball->unk154 = 0;
            lbl_11_bss_29F8 = 0;
            lbl_11_bss_2A04 = 0;
            lbl_11_bss_2A0C = 0;
            lbl_11_bss_2A10 = 9;
        } else {
            float h = math_sqrt(h2);
            float scale = (h - decel) / h;

            ball->vel.x *= scale;
            ball->vel.z *= scale;
        }
    }
}

/* Physics step without collision (used to predict the shot): gravity and air drag only. */
void fn_11_19CB0(struct Ball *ball, struct PhysicsBall *phys)
{
    Vec up;
    float gx;
    float gy;
    float gz;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20;
    mtxa_from_identity();
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    mtxa_tf_vec(&up, &up);
    gx = 0.0f;
    gy = -lbl_11_data_9F4;
    gz = 0.0f;
    if (ball->flags & 0x200) {
        gy = -gy;
    } else if (ball->flags & 0x100) {
        gy = 0.0f;
    }
    ball->vel.x += gx;
    ball->vel.y += gy;
    ball->vel.z += gz;
    ball->pos.x += ball->vel.x;
    ball->pos.y += ball->vel.y;
    ball->pos.z += ball->vel.z;
    if (ball->vel.y > 0.0f) {
        ball->vel.x *= 0.995;
        ball->vel.y *= 0.995;
        ball->vel.z *= 0.995;
    } else {
        ball->vel.x *= 0.998;
        ball->vel.y *= 0.998;
        ball->vel.z *= 0.998;
    }
    if (ball->vel.y > 0.0f) {
        ball->vel.x *= 0.995;
        ball->vel.y *= 1.0001;
        ball->vel.z *= 0.995;
    }
}

/* Shot preview: run a copy of the ball (balls[4]) forward with fn_11_19CB0 and record every fourth
   position in lbl_11_bss_198 until it lands, stops, falls out or runs out of time/points.
   lbl_11_bss_B18: 0 start, 1 running, 2 done. */
void fn_11_19F24(struct Ball *src)
{
    struct PhysicsBall phys2;
    struct PhysicsBall phys;
    Vec lastPos;
    Vec savedPrev;
    struct Ball *ghost;
    int n;

    lbl_11_bss_29F4 = 0;
    if (lbl_11_bss_B18 == 2) {
        return;
    }
#ifndef __MWERKS__
    ghost = &balls[4];
#endif
    if (lbl_11_bss_B18 == 1) {
        ghost = &balls[4];
        lastPos = ghost->pos;
    } else if (lbl_11_bss_B18 == 0) {
        balls[4] = *src;
        balls[4].status = 0;
        ghost = &balls[4];
        ghost->unk80 = 0;
        lbl_11_bss_2A34 = 0;
        lbl_11_bss_2A38 = 9;
        lbl_11_bss_50 = 0;
        lbl_11_bss_B18 = 1;
        lastPos = ghost->pos;
    }
    while (lbl_11_bss_29F4 < 2000 && lbl_11_bss_50 < 200) {
        ghost->flags |= 0x100000000LL;
        lbl_11_bss_2A34++;
        fn_11_19CB0(ghost, &phys2);
        if (lbl_11_bss_2A38 == modeCtrl.currPlayer) {
            if (lbl_11_bss_2A40 <= 0.005) {
                lbl_11_bss_2A3C++;
            } else {
                lbl_11_bss_2A3C = 0;
            }
            lbl_11_bss_2A40 = mathutil_vec_len(&ghost->vel);
            if (lbl_11_bss_2A3C > 60) {
                ghost->vel.x = 0.0f;
                ghost->vel.y = 0.0f;
                ghost->vel.z = 0.0f;
                ghost->unk154 = 0;
                lbl_11_bss_2A3C = 0;
                lbl_11_bss_29FC = 0;
                lbl_11_bss_2A34 = 0;
                lbl_11_bss_2A38 = 9;
                break;
            }
        } else {
            lbl_11_bss_2A38 = modeCtrl.currPlayer;
            lbl_11_bss_2A3C = 0;
        }
        ghost->unk80++;
        lbl_11_bss_29F4++;
        if (ghost->unk80 % 4 == 0 && ghost->vel.y < 0.0f) {
            /* every fourth frame while falling: sweep the last four frames against the stage */
            balls[5] = balls[4];
            savedPrev = ghost->prevPos;
            ghost->prevPos = lastPos;
            ghost->vel.x = ghost->pos.x - ghost->prevPos.x;
            ghost->vel.y = ghost->pos.y - ghost->prevPos.y;
            ghost->vel.z = ghost->pos.z - ghost->prevPos.z;
            if (ghost->vel.y < 0.0f) {
                init_physicsball_from_ball(ghost, &phys);
                phys.restitution = 0.3f;
                phys.radius = 1.0f;
                fn_11_18D78(&phys, stagedef);
            }
            if (ghost->unk80 > 4 && (phys.flags & 1)) {
                /* landed: drop the recorded points below the landing height */
                n = 30;
                do {
                    lbl_11_bss_50--;
                    n--;
                    if (lbl_11_bss_50 <= 2) {
                        lbl_11_bss_50 = 2;
                        break;
                    }
                    if (!(lbl_11_bss_198[lbl_11_bss_50 - 1].y < phys.pos.y)) {
                        break;
                    }
                } while (n > 0);
                lbl_11_bss_B18 = 2;
                lbl_11_bss_50--;
                balls[4] = balls[5];
                break;
            }
            if (did_ball_fallout(ghost)) {
                lbl_11_bss_B18 = 2;
                lbl_11_bss_50 -= 2;
                balls[4] = balls[5];
                break;
            }
            ghost->prevPos = savedPrev;
            ghost->vel.x = ghost->pos.x - ghost->prevPos.x;
            ghost->vel.y = ghost->pos.y - ghost->prevPos.y;
            ghost->vel.z = ghost->pos.z - ghost->prevPos.z;
            lastPos = ghost->pos;
            balls[4] = balls[5];
        }
        if (ghost->unk80 > 2000) {
            lbl_11_bss_50--;
            lbl_11_bss_B18 = 2;
            break;
        }
        if (ghost->unk80 > 4 && (ghost->flags & 1)) {
            lbl_11_bss_50--;
            lbl_11_bss_B18 = 2;
            break;
        }
        if (ghost->unk80 % 4 == 0 && lbl_11_bss_50 < 200) {
            lbl_11_bss_198[lbl_11_bss_50] = ghost->pos;
            lbl_11_bss_50++;
        }
    }
    lbl_11_bss_2A04 = lbl_11_bss_2A34;
}

/* Not referenced anywhere, so the linker strips it; only its literals (0.48, 0.6, 0.39, 0.3, 2.0f)
   survive in .rodata. The original contents are unknown. */
void fn_11_golf_1_stripped(struct Ball *ball)
{
    ball->vel.x *= 0.48;
    ball->vel.y *= 0.6;
    ball->vel.z *= 0.39;
    ball->vel.y *= 0.3;
    ball->vel.y *= 2.0f;
}

/* Monkey update for the golfer: pick the falling/still pose, keep it upright on the ball and look at the cup. */
void fn_11_1A610(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    struct RaycastHit hit;
    int r28;

    if (!(g_some_other_flags & 0xA)) {
        raycast_stage_down(&ball->pos, &hit, NULL);
        ape->flags &= ~0x13;
        if (!(hit.flags & 1) && ball->vel.y < -0.16203703f) {
            ape->flags |= 2;
        } else if (mathutil_vec_len(&ball->unkC0) < 0.00027777778f) {
            ape->flags |= 1;
        }
        r28 = (ball->flags & 0x1000) != 0;
        r28 |= (ape->flags & 3) == 0;
        g_ape_upright(ape);
        if (r28 != 0) {
            fn_1_45A8C(ape);
        } else {
            mtxa_from_quat(&ape->rot);
            mtxa_normalize_basis();
            if (ape->flags & 2) {
                fn_1_46958(ape);
            }
        }
        if (ball->flags & 0x20) {
            mathutil_vec_len(&ball->vel);
        }
        fn_1_45CA8(ape);
        mtxa_to_quat(&ape->rot);
        fn_1_D2510(ape);
        if (!(ape->flags & 8)) {
            g_move_ape_into_ball(ape);
        }
        balls[ape->ballIdx].unk10C = lbl_11_bss_38;
        ball->unk108 = 0;
        ball->unk118 = 0.0f;
    }
}

/* Used only by golf_2.c (w6), but their .bss (0x2A48-0x2A80) is the end of this TU's: golf_2.c's
   own .bss starts at its anchor 0x2A80. MWCC emits unreferenced globals after the referenced ones
   in reverse definition order, hence 2A50, 2A4C, 2A48 here. */
Mtx lbl_11_bss_2A50;
u8 lbl_11_bss_2A4C;
s32 lbl_11_bss_2A48;
u32 lbl_11_bss_2A44;  /* unused (w5); defined last so it is emitted first of the unreferenced ones */
