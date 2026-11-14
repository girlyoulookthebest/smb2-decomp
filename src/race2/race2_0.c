#include "background.h"
#include "race2.h"
#include "item.h"
#include "libc.h"
#include "event.h"
#include "input.h"
#include "sprite.h"
#include "sound.h"

struct RaceInputRec
{
    s8 stickX;
    s8 stickY;
    u16 button;
    s16 cam;
};  /* size = 0x6 */

extern u32 lbl_1_bss_D3360;
extern s16 lbl_1_bss_D3366;
extern float lbl_1_bss_D3388[][4];
extern s16 player_count;
extern struct RaceInputRec *lbl_10_bss_1228;
extern s16 lbl_10_bss_122C;
#ifdef __MWERKS__
extern u32 lbl_10_bss_2D34;
#else
/* PC: s32 as defined in race2.c.  The replay sets it to -1 before its first
 * frame; read as u32, &lbl_10_bss_1228[-1] became an index of 0xFFFFFFFF,
 * which only wraps back to element -1 with 32-bit pointers (x64: crash in
 * Race's replay, harness mode raceend) */
extern s32 lbl_10_bss_2D34;
#endif
extern u32 lbl_10_bss_2D38;
extern s16 lbl_10_bss_2D50[8];
extern struct RacePlayer *lbl_10_bss_4238[8];
extern Vec lbl_10_bss_4260[8];
extern u16 lbl_1_data_65728;
extern float lbl_1_data_6572C;
extern float lbl_1_data_65730;
extern float lbl_1_data_65734;
extern float lbl_1_data_65738;
extern float lbl_1_data_6573C;
extern float lbl_1_data_65748;
extern float lbl_1_data_6576C;
extern s16 lbl_1_data_65772;
extern float lbl_1_data_65774;
extern float lbl_1_data_65778;
extern float lbl_1_data_6577C;
extern float lbl_1_data_65780;
extern float lbl_1_data_65784;
extern float lbl_1_data_65788;

void SoundDop(u16 a, u16 b);
void SoundIcsReq(s32 a, u8 b, s32 c);
void SoundPitch(u16 a, u16 b);
void call_SoundReqID_arg_0(u32 id);
s32 call_SoundReqID_arg_1_copy(u32 id);
void collide_ball_with_cylinder(struct PhysicsBall *a, struct StageColiCylinder *b);
void collide_ball_with_tri_edges(struct PhysicsBall *a, struct StageColiTri *b);
void collide_ball_with_tri_face(struct PhysicsBall *a, struct StageColiTri *b);
void collide_ball_with_tri_vertices(struct PhysicsBall *a, struct StageColiTri *b);
void destroy_sprite_with_unique_id(s32 id);
s32 did_ball_fallout(struct Ball *ball);
s16 fn_10_181C8(u16 a);
u16 fn_10_188C0(void);
float fn_10_18900(void);
s8 fn_1_2F8F0(s32 a, Vec *b, u8 *c, u8 *d);
sptr fn_80069870(sptr a);
void *fn_8006991C(sptr a);
void g_reset_ball(struct Ball *ball);
void g_maybe_sets_number_of_starting_monkeys(struct Ball *ball);
void g_rumble_controller(int a, int b, int c);
void g_fade_track_volume(s32 a, s32 b);
void g_smth_with_ape_rotation(struct Ape *ape, float *b);
void mtxa_to_quat(Quat *q);
void quat_mult(Quat *dst, Quat *a, Quat *b);
void quat_slerp(Quat *dst, Quat *a, Quat *b, float t);
void position_ball(struct Ball *ball, struct PhysicsBall *b);
void fn_1_4AF48(struct Ball *ball, struct PhysicsBall *b);

static inline void OSs16tof32(register s16 *in, register volatile float *out)
{
#ifdef __MWERKS__
    asm {
        psq_l fp1, 0(in), 1, 5
        stfs fp1, 0(out)
    }
#else
    *out = (float)*in;  /* GQR scale 0 */
#endif
}

static inline void OSs8tof32(register s8 *in, register volatile float *out)
{
#ifdef __MWERKS__
    asm {
        psq_l fp1, 0(in), 1, 4
        stfs fp1, 0(out)
    }
#else
    *out = (float)*in;  /* GQR scale 0 */
#endif
}

struct Race2Seg {
    float unk0;
    float unk4;
    s16 unk8;
    float unkC;
};

struct Race2Item
{
    s32 unk0;
    s32 unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    float unk10;
};
struct Race2Marker
{
    Vec pos;
    s16 rotX;
    s16 rotY;
    s16 rotZ;
};
void fn_10_DDA8(struct Effect *, struct Race2EffectLink *);
struct Race2Item *fn_10_10BDC(void);
void fn_10_17190(struct Ball *, struct StageExtraData *, float *);
void fn_10_181EC(struct StageExtraData *, float, Vec *);
s32 fn_10_1CA74(s32, s16 *);
void fn_10_1D9F0(s32);
void fn_10_1E614(s32);
void fn_10_1ECC0(s32);
void fn_1_3061C(s32, s32);
void fn_800698C0(sptr, void *);
void g_something_with_item_coli(Vec *, Vec *, float, Vec *, float);
s32 g_is_ball_in_ig_coli_range(struct ItemGroup *, struct StageCollHeader *, s32, Vec *);
s16 *tri_coli_grid_lookup(struct StageCollHeader *, float, float);
s32 fn_1_A74E4(Vec *, Vec *, float, Vec *, Vec *, float);
void fn_10_0(struct Ball *ball);
void fn_10_398(struct Ball *ball, s16 arg1);
void fn_10_408(void);
void fn_10_488(struct Ball *ball);
void fn_10_F18(struct Ball *ball);
void fn_10_1118(struct Ball *ball);
void fn_10_139C(struct Ball *ball);
void fn_10_1424(struct Ball *ball);
void fn_10_1520(struct Ball *ball);
void fn_10_1604(struct Ball *ball);
void fn_10_1700(struct Ball *ball);
void fn_10_1800(struct Ball *ball);
void fn_10_1EF4(struct Ball *ball);
void fn_10_21E4(struct Ball *ball);
void fn_10_2394(struct Ball *ball);
void fn_10_24C8(struct Ball *ball);
void fn_10_25AC(struct Ball *ball);
void fn_10_285C(struct Ball *ball);
void fn_10_2A34(struct Ball *ball);
void fn_10_2B4C(struct Ball *ball);
void fn_10_2CCC(struct Ball *ball);
void fn_10_2D6C(struct Ball *ball);
void fn_10_2F3C(struct Ball *ball);
void fn_10_328C(struct Ball *ball, s16 arg1);
void g_references_420_06(struct Ball *ball, s16 arg1);
void fn_10_45A8(struct Ball *ball);
void fn_10_46C8(struct Ball *ball);
s32 fn_10_47C4(struct Ball *ball);
void fn_10_4AFC(struct Ball *ball, s32 arg1);
void fn_10_4F70(struct Ball *ball, Vec *arg1);
void fn_10_51A8(struct Ball *ball, Vec *arg1);
s32 fn_10_5840(struct Ball *ball);
void fn_10_588C(struct Ball *ball);
float fn_10_5968(struct Ball *ball);
void fn_10_5B30(struct Ball *ball, Quat *arg1);
void fn_10_5C1C(struct Ball *ball);
void fn_10_5F1C(struct Ball *ball);
void fn_10_6164(struct Ball *ball);
void fn_10_62E0(struct Ball *ball, s16 arg1);
void fn_10_66AC(struct Ball *ball, s16 arg1);
void fn_10_679C(struct Ball *ball, Vec *arg1);
void fn_10_689C(struct Ball *ball);
s32 fn_10_68D0(struct Ball *ball);
void fn_10_6978(void);
void fn_10_7250(void);
void fn_10_74A4(struct PhysicsBall *arg0, struct StageDef *arg1);
void fn_10_772C(struct Ball *ball);
void fn_10_7B68(struct Ball *ball);

void (*lbl_10_data_0[])(struct Ball *) = {
    fn_10_488, fn_10_F18, fn_10_1118, fn_10_139C, fn_10_1424, fn_10_1520,
    fn_10_1604, fn_10_1700, fn_10_1800, fn_10_1EF4, fn_10_21E4, fn_10_2394,
    fn_10_24C8, fn_10_25AC, fn_10_285C, fn_10_2A34, fn_10_2B4C, fn_10_2CCC,
};

void fn_10_0(struct Ball *ball)
{
    struct Race2Course *course;
    float tf1_2;
    float tf3;
    float f0;
    float f0_2;
    s8 r30;
    s32 r27;
    struct RacePlayer *player;
    struct RaceInputRec *tr3;

    course = &lbl_10_bss_1208;
    player = &lbl_10_bss_1230[ball->playerId];
    player->unk38 = player->flags;
    player->unk2BE = player->unk2BC;
    player->unk2C4 = player->unk2C2;
    if (((u32) (dip_switches & 1) != 0) && ((s32) (course->unk2 & 0x10) != 0) && ((u32) (lbl_1_bss_D3360 & 0x20) != 0) && ((u32) (g_font_char_var_3 % 10) == 0)) {
        printf("%f\t%f\t%f\n", ball->pos.x, ball->pos.y, ball->pos.z);
    }
    if ((ball->playerId >= 0) && (ball->playerId <= 4)) {
        if ((s32) (course->unk2 & 0x40) != 0) {
            tr3 = &lbl_10_bss_1228[lbl_10_bss_2D34];
            player->unk2 = tr3->stickX;
            player->unk3 = tr3->stickY;
            player->unk4 = tr3->button;
            player->unk6 = tr3->cam;
        } else {
            if ((player->unk14 >= 0) && (player->unk14 <= 4)) {
                player->unk2 = pad_status_groups[player->unk14].unk0[0].stickX;
                player->unk3 = pad_status_groups[player->unk14].unk0[0].stickY;
                player->unk4 = pad_status_groups[player->unk14].unk0[2].button;
            }
            player->unk6 = cameras[ball->playerId].rotY;
        }
    }
    if ((ball->unk154 >= 0) && (ball->unk154 < 0x12)) {
        if ((ball->status == 2) && ((u32) ball->unk150 != 0)) {
            fn_10_2D6C(ball);
        }
        lbl_10_data_0[ball->unk154](ball);
        if (ball->status == 2) {
            fn_10_2F3C(ball);
            fn_10_45A8(ball);
            r30 = 0;
            r27 = 0;
            tf1_2 = 216.0f * mathutil_vec_len(&ball->vel);
            if ((player->unk8 > 3) && (tf1_2 > 10.0)) {
                tf3 = (tf1_2 - 10.0f) / 130.0f;
                if ((127.0f * tf3) < 127.0f) {
                    f0 = 127.0f * tf3;
                } else {
                    f0 = 127.0f;
                }
                r30 = (s8) f0;
                if ((10.0f * (127.0f * tf3)) < 80.0f) {
                    f0_2 = 10.0f * (127.0f * tf3);
                } else {
                    f0_2 = 80.0f;
                }
                r27 = (0.85f * f0_2);
            }
            if (((u32) (g_font_char_var_3 & 7) == (u32) ball->playerId) || (r30 == 0)) {
                SoundIcsReq(ball->playerId, (u8) r30, r27);
            }
        }
    }
}

void fn_10_398(struct Ball *ball, s16 arg1)
{
    if ((arg1 < 0) || (arg1 >= 0x12)) {
        OSPanic("race2_ball.c", 0xCE, "Illegal ball sub mode : %d\n");
    }
    ball->unk155 = (s8) arg1;
}

void fn_10_408(void)
{
    s16 r31;

    fn_10_6978();
    fn_10_7250();
    for (r31 = 0; r31 < player_count; r31++) {
        fn_10_772C(&balls[r31]);
        fn_10_7B68(&balls[r31]);
    }
}

void fn_10_488(struct Ball *ball)
{
    Vec sp64;
    Vec sp58;
    s16 r3;
    struct StartPos *r30;
    struct Race2Course *course;
    struct Race2CourseInfo *tr28;
    struct RacePlayer *player;
    u8 tr27;

    tr27 = ball->state;
    r30 = stagedef->startPos;
    course = &lbl_10_bss_1208;
    tr28 = &lbl_10_data_2ADC[course->unk0];
    g_maybe_sets_number_of_starting_monkeys(ball);
    g_reset_ball(ball);
    ball->status = 2;
    ball->state = (s8) tr27;
    ball->unk155 = 1;
    if ((s32) modeCtrl.unk30 == 1) {
        ball->unk179 = 0;
    } else if ((s32) modeCtrl.unk30 == 2) {
        ball->unk179 = 1;
    } else {
        ball->unk179 = 4;
    }
    ball->unk150 = (u32) &lbl_10_bss_1230[ball->playerId];
    player = (struct RacePlayer *)ball->unk150;
    player->unk0 = ball->playerId;
    player->unk3E = 0;
    if (r30 != NULL) {
        if (((u32) (dip_switches & 1) != 0) && ((s32) (course->unk2 & 0x10) != 0) && ((u32) (lbl_1_bss_D3360 & 0x28) != 0)) {
            if ((u32) (lbl_1_bss_D3360 & 8) != 0) {
                r30 = &r30[lbl_1_data_65728];
            } else if ((u32) (lbl_1_bss_D3360 & 0x20) != 0) {
                r30 = &r30[lbl_1_bss_D3366];
            }
        } else if ((s32) (course->unk2 & 0x10) != 0) {
            r30 = &r30[*(s32 *)stagedef->unk6C - 1];
        } else if (((s32) (course->unk2 & 8) != 0) && (course->unk6 != 0)) {
            for (r3 = 0; r3 < player_count; r3++) {
                if (player == lbl_10_bss_4238[r3]) {
                    r30 = &r30[(player_count - r3) - 1];
                    player->unk3E = (player_count - r3);
                    break;
                }
            }
        } else {
            r30 = &r30[(player_count - ball->playerId) - 1];
        }
        ball->pos = r30->pos;
        ball->rotX = r30->xrot;
        ball->rotY = r30->yrot;
        ball->rotZ = r30->zrot;
        ball->pos.y += (ball->currRadius + (576.0f * (0.5f * ball->accel)));
    } else {
        ball->pos = (Vec){0.0f, 0.0f, 0.0f};
    }
    ball->prevPos = ball->pos;
    ball->vel = (Vec){0.0f, 0.0f, 0.0f};
    ball->flags |= 0x100000000ULL;
    if ((s32) (course->unk2 & 0x10) != 0) {
        ball->colorId = 3;
    } else {
        ball->colorId = ball->playerId;
    }
    ball->ape->flags |= 0x20000;
    player->unk18 = ball->pos;
    player->unk24 = player->unk18;
    player->unk30 = (ball->modelScale * (ball->modelScale * ball->modelScale));
    player->flags = 0;
    if (ball->playerId >= modeCtrl.unk30) {
        ball->flags |= 0x80000000000ULL;
        ball->flags |= 0x20000000000ULL;
    }
    if ((ball->flags & 0x80000000000ULL) != 0) {
        player->unk14 = -1;
    } else {
        player->unk14 = player_pad_map[ball->playerId];
        if ((s32) (course->unk10 & (1 << ball->playerId)) != 0) {
            player->flags |= 0x10000;
        }
    }
    player->unk38 = player->flags;
    player->unk3C = 0;
    player->unk40 = 0;
    player->unk42 = 0;
    player->unk44 = 0;
    player->unk46 = 0;
    for (r3 = 0; r3 < 0x32; r3++) {
        player->unk48[r3].unk0 = 0;
        player->unk48[r3].unk4 = 0.0f;
    }
    player->unk1D8 = 0x7FFFFFFF;
    player->unk1DC = 0.0f;
    player->unk1E0 = 0;
    player->unk1E4 = 0.0f;
    player->unk1E8 = 0x63;
    player->unk1E9 = 0x3B;
    player->unk1EA = 0x63;
    player->unk1EC = 0;
    player->unk8 = 0;
    player->unkA = 5;
    player->unkE = 0;
    player->unk10 = 0;
    fn_10_17190(ball, stagedef->extraData, &player->unk1F0);
    player->unk1F4 = player->unk1F0;
    player->unk1F8 = player->unk1F0;
    if (player->unk1F8 > 90.0f) {
        player->unk1F8 -= 100.0f;
    }
    player->unk1FC = player->unk1F8;
    player->unk208 = player->unk1F0;
    player->unk20C = player->unk1F4;
    player->unk210 = player->unk1F8;
    player->unk214 = player->unk1FC;
    player->unk200 = 0.0f;
    player->unk204 = 0.0f;
    player->unk218 = lbl_1_data_6573C;
    player->unk21C = 0.0f;
    for (r3 = 0; r3 < 8; r3++) {
        player->unk280[r3] = -1.0f;
        player->unk220[r3] = (Vec){0.0f, 0.0f, 0.0f};
    }
    fn_10_181EC(stagedef->extraData, player->unk1F0, &sp64);
    fn_10_181EC(stagedef->extraData, (float) (0.01 + player->unk1F0), &sp58);
    PSVECSubtract(&sp58, &sp64, &sp64);
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y((s16) (math_atan2(sp64.x, sp64.z) + 0x5000));
    mtxa_rotate_x(-0x1000);
    mtxa_rotate_z(0);
    mtxa_to_mtx(ball->unk30);
    if ((ball->flags & 0x80000000000ULL) != 0) {
        player->unk2A0 = tr28->unk14[ball->playerId - 1];
        player->unk338 = ball->rotY;
    }
    player->unk2A4 = 0.0f;
    player->unk2A8 = 0.0f;
    player->unk2AC = (Vec){0.0f, 0.0f, 0.0f};
    player->unk2B8 = 0;
    player->unk2BC = 0;
    player->unk2BE = 0;
    player->unk2C0 = 0;
    player->unk2C2 = 0;
    player->unk2C4 = 0;
    player->unk2C6 = 0;
    player->unk2CE = 0;
    player->unk2BA = 0;
    player->unk2D0 = 1.0f;
    player->unk2D4 = (Quat){0.0f, 0.0f, 0.0f, 0.0f};
    player->unk2E4 = (Quat){0.0f, 0.0f, 0.0f, 0.0f};
    for (r3 = 0; r3 < 8; r3++) {
        player->unk2F4[r3] = 0;
    }
    player->unk2FC = 0;
    player->unk2FD = 0;
    player->unk300 = 1.0f;
    for (r3 = 0; r3 < 3; r3++) {
        if ((s32) (course->unk2 & 0x10) != 0) {
            player->unk2C8[r3] = 1;
        } else {
            player->unk2C8[r3] = 0;
        }
    }
    player->unk304[0] = 0;
    player->unk304[1] = 0;
    player->unk304[2] = 0;
    player->unk304[3] = 0;
    player->unk304[4] = 0;
    player->unk304[5] = 0;
    player->unk304[6] = 0;
    player->unk304[7] = 0;
    player->unk304[8] = 0;
    player->unk304[9] = 0;
    for (r3 = 0; r3 < 8; r3++) {
        if (lbl_10_bss_2D68[r3] == NULL) {
            lbl_10_bss_2D68[r3] = player;
            break;
        }
    }
    player->unk318 = -1;
    player->unk31C = -1;
    player->unk334 = 0;
    player->unk336 = 0;
    for (r3 = 0; r3 < 0x20; r3++) {
        player->unk33A[r3] = 0;
    }
    player->unk35A = 0;
    player->unk35C = 0;
    player->unk35D = 0;
    player->unk35E = ball->rotY;
}

void fn_10_F18(struct Ball *ball)
{
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
    fn_10_46C8(ball);
    if ((ball->prevFlags & 0x1ULL) != 0) {
        fn_10_5F1C(ball);
        fn_10_6164(ball);
    }
    fn_10_4AFC(ball, 1);
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    if (fn_10_5840(ball) != 0) {
        call_SoundReqID_arg_0(0x1D);
        call_SoundReqID_arg_0(0x15);
        player->flags &= 0xFFFFFFF7;
        if (((s32) (player->unk2BC & 0x20) != 0) && (player->unk304[5] >= 0x78)) {
            player->unk304[5] = 0x78;
        }
        player->unk2BC = (player->unk2BC & 0x20);
        player->unk2C2 = 0;
        player->unk208 = player->unk1F0;
        player->unk20C = player->unk1F4;
        player->unk210 = player->unk1F8;
        player->unk214 = player->unk1FC;
        ball->unk155 = 8;
        if ((ball->flags & 0x80000000000ULL) == 0) {
            cameras[ball->playerId].unk1F = 4;
        }
        player->unk3C = 0x3C;
        return;
    }
    if ((u32) (player->flags & 2) != 0) {
        ball->unk155 = 9;
    }
}

void fn_10_1118(struct Ball *ball)
{
    struct Race2Course *course;
    Vec sp30;
    Vec sp24;
    float tf31;
    struct RacePlayer *player;

    course = &lbl_10_bss_1208;
    player = (struct RacePlayer *)ball->unk150;
    call_SoundReqID_arg_0(0x281E);
    if ((ball->flags & 0x80000000000ULL) == 0) {
        cameras[ball->playerId].unk1F = 1;
    }
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){0.0f, 0.0f, 0.0f};
    ball->unkB0 = (Quat){0.0f, 0.0f, 0.0f, 1.0f};
    ball->unkA0 = ball->unkB0;
    mtxa_to_quat(&ball->unkB0);
    ball->ape->flags &= 0x20000;
    fn_10_588C(ball);
    if (((ball->flags & 0x80000000000ULL) == 0) && ((s32) (course->unk2 & 0x40) == 0)) {
        tf31 = 100.0f * (3.0f / course->unk8);
        fn_10_181EC(stagedef->extraData, player->unk1F0, &sp30);
        fn_10_181EC(stagedef->extraData, player->unk1F0 + tf31, &sp24);
        PSVECSubtract(&sp24, &sp30, &sp30);
        cameras[ball->playerId].rotY = math_atan2(sp30.x, sp30.z) - 0x8000;
    }
    player->flags |= 0x10;
    player->flags &= 0xFFFFFFFE;
    ball->unk155 = 3;
    fn_10_139C(ball);
}

void fn_10_139C(struct Ball *ball)
{
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    if (player->unk3C == 0) {
        player->flags &= 0xFFFFFFF7;
        player->flags &= 0xFFFFFFEF;
        ball->unk155 = 1;
    } else if ((u32) (lbl_10_bss_2D38 & 4) != 0) {
        player->flags &= 0xFFFFFFF7;
    } else {
        player->flags |= 8;
    }
    fn_10_F18(ball);
}

void fn_10_1424(struct Ball *ball)
{
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){0.0f, 0.0f, 0.0f};
    ball->unkB0 = (Quat){0.0f, 0.0f, 0.0f, 1.0f};
    ball->unkA0 = ball->unkB0;
    mtxa_to_quat(&ball->unkB0);
    ball->ape->flags &= 0x20000;
    ball->unk155 = 5;
    fn_10_1520(ball);
}

void fn_10_1520(struct Ball *ball)
{

    if (((struct RacePlayer *)ball->unk150)->unk3C <= 0) {
        call_SoundReqID_arg_0(0x281E);
        ball->vel.x = ball->vel.y = ball->vel.z = 0.0f;
        ball->unk155 = 1;
    }
    fn_10_588C(ball);
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
    ball->vel.y -= ball->accel;
    ball->pos.y += ball->vel.y;
}

void fn_10_1604(struct Ball *ball)
{
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){0.0f, 0.0f, 0.0f};
    ball->unkB0 = (Quat){0.0f, 0.0f, 0.0f, 1.0f};
    ball->unkA0 = ball->unkB0;
    mtxa_to_quat(&ball->unkB0);
    ball->ape->flags &= 0x20000;
    ball->unk155 = 5;
    fn_10_1700(ball);
}

void fn_10_1700(struct Ball *ball)
{
    float tf1_2;

    if (((struct RacePlayer *)ball->unk150)->unk3C <= 0) {
        call_SoundReqID_arg_0(0x281E);
        ball->vel.x = ball->vel.y = ball->vel.z = 0.0f;
        ball->unk155 = 1;
    }
    fn_10_588C(ball);
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
    ball->vel.y -= ball->accel;
    ball->vel.x = ball->vel.z = 0.0f;
    tf1_2 = ball->pos.y;
    ball->pos.y = (tf1_2 + ball->vel.y);
    fn_10_588C(ball);
}

void fn_10_1800(struct Ball *ball)
{
    struct Race2Course *course;
    float sp8;
    struct RaycastHit hit;
    float tf0;
    float tf0_2;
    float tf0_3;
    float tf0_4;
    float tf0_5;
    float tf0_6;
    float tf0_7;
    float tf3;
    struct RacePlayer *player;
    float *r4;
    struct StageExtraData *extra;
    struct Race2CourseInfo *info;

    course = &lbl_10_bss_1208;
    info = &lbl_10_data_2ADC[course->unk0];
    player = (struct RacePlayer *)ball->unk150;
    if (player->unk3C == 0) {
        extra = stagedef->extraData;
        ball->flags &= ~0x1ULL;
        player->flags &= 0xFFFFFFDF;
        player->unk2BC = 0;
        player->unk2C2 = 0;
        OSs16tof32(&player->unkA, &sp8);
        sp8 = (player->unk1F0 + (0.18000000000000002 * sp8 * course->unkC));
        if (sp8 >= 100.0) {
            sp8 = (100.0 - course->unkC);
        }
        r4 = info->unk38;
        for (; r4[0] > 0.0f; r4 += 2) {
            if ((r4[0] <= sp8) && (sp8 <= r4[1])) {
                sp8 = r4[1];
                break;
            }
        }
        player->unk1F0 = player->unk208 = sp8;
        if (player->unk1F8 >= 0.0) {
            if ((sp8 - course->unkC) >= 0.0) {
                tf0 = (sp8 - (0.5 * course->unkC));
                player->unk20C = tf0;
                player->unk1F4 = tf0;
            } else {
                player->unk1F0 = player->unk208 = course->unkC;
                tf0_2 = (0.5 * course->unkC);
                player->unk20C = tf0_2;
                player->unk1F4 = tf0_2;
                sp8 = player->unk1F0;
            }
            tf0_3 = player->unk1F0;
            player->unk210 = tf0_3;
            player->unk1F8 = tf0_3;
        } else {
            if (sp8 <= (100.0 - course->unkC)) {
                tf0_4 = (sp8 + (0.5 * course->unkC));
                player->unk20C = tf0_4;
                player->unk1F4 = tf0_4;
            } else {
                tf0_5 = (100.0 - course->unkC);
                player->unk208 = tf0_5;
                player->unk1F0 = tf0_5;
                tf0_6 = (100.0 - (0.5 * course->unkC));
                player->unk20C = tf0_6;
                player->unk1F4 = tf0_6;
                sp8 = player->unk1F0;
            }
            tf0_7 = (player->unk1F0 - 100.0);
            player->unk210 = tf0_7;
            player->unk1F8 = tf0_7;
        }
        if ((ball->flags & 0x80000000000ULL) != 0) {
            player->unk2A4 = player->unk1F0;
            player->unk2A8 = player->unk1F4;
        }
        fn_10_181EC(extra, sp8, &ball->pos);
        tf3 = ball->pos.y;
        ball->pos.y = (tf3 + (3.0f * ball->currRadius));
        if ((s32) raycast_stage_down(&ball->pos, &hit, NULL) == 0) {
            ball->pos.y += 100.0f;
            raycast_stage_down(&ball->pos, &hit, NULL);
        }
        ball->pos.y = (hit.pos.y + (3.0f * ball->currRadius));
        ball->vel = (Vec){0.0f, 0.0f, 0.0f};
        ball->unk80 = 0;
        ball->unk60.x = 0;
        ball->unk60.y = 0;
        ball->unk60.z = 0;
        ball->unk155 = 2;
        player->unk3C = (lbl_80198B38 * 2);
        fn_10_1118(ball);
        return;
    }
    ball->prevPos = ball->pos;
    tf0 = math_sqrt(mathutil_vec_sq_len(&ball->vel));
    ball->speed = tf0;
    ball->flags &= ~0x20ULL;
    fn_10_4AFC(ball, 0);
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
}

void fn_10_1EF4(struct Ball *ball)
{
    struct Race2Course *course;
    s16 *tr4;
    struct RacePlayer *player;

    course = &lbl_10_bss_1208;
    player = (struct RacePlayer *)ball->unk150;
    player->flags &= 0xFFFFFFF7;
    player->unk2BC = 0;
    player->unk2C2 = 0;
    player->unk42 = fn_10_181C8(player->unk3E);
    tr4 = &lbl_10_bss_2D50[ball->playerId];
    *tr4 += player->unk42;
    fn_10_1CA74(ball->playerId, tr4);
    if ((s32) (course->unk2 & 0x10) != 0) {
        call_SoundReqID_arg_0(0x2CA);
        call_SoundReqID_arg_0(0x2930);
        g_fade_track_volume(0, 8);
        lbl_10_bss_122C = (5.5 * lbl_80198B34);
        fn_1_3061C(0x166, 0x167);
    } else {
        if ((ball->flags & 0x80000000000ULL) == 0) {
            if (player->unk3E >= player_count) {
                call_SoundReqID_arg_0(0x2CB);
            } else {
                call_SoundReqID_arg_0(0x2CA);
            }
        }
        if (player->unk3E >= player_count) {
            call_SoundReqID_arg_0(0x292C);
            if ((ball->flags & 0x80000000000ULL) == 0) {
                g_fade_track_volume(0, 8);
                lbl_10_bss_122C = (3.0 * lbl_80198B34);
                fn_1_3061C(0x164, 0x165);
            }
        } else {
            s16 voices[] = {0x130, 0x12F, 0x12E, 0x12E, 0x12E, 0x12E, 0x12E, 0x12C};
            call_SoundReqID_arg_0(voices[player->unk3E - 1] | 0x2800);
            if ((ball->flags & 0x80000000000ULL) == 0) {
                g_fade_track_volume(0, 8);
                lbl_10_bss_122C = (5.5 * lbl_80198B34);
                fn_1_3061C(0x166, 0x167);
            }
        }
    }
    player->unk3C = (lbl_80198B38 * 2);
    ball->unk155 = 0xA;
    fn_10_21E4(ball);
}

void fn_10_21E4(struct Ball *ball)
{
    struct Race2Course *course;
    struct RacePlayer *player;

    course = &lbl_10_bss_1208;
    player = (struct RacePlayer *)ball->unk150;
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
    ball->vel.y -= ball->accel;
    PSVECScale(&ball->vel, &ball->vel, 0.97f * player->unk218);
    if ((ball->flags & 0x80000000000ULL) == 0) {
        if (mathutil_vec_sq_len(&ball->vel) < 0.25934502f) {
            cameras[ball->playerId].unk1F = 9;
        }
    }
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    if (player->unk3C <= 0) {
        if (((s32) (course->unk2 & 0x10) != 0) || (player->unk3E < player_count)) {
            ball->unk155 = 0xB;
            return;
        }
        ball->unk155 = 0xF;
    }
}

void fn_10_2394(struct Ball *ball)
{
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    ball->flags |= 0x240ULL;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){0.0f, 0.0f, 0.0f};
    player->flags &= 0xFFFFFFF7;
    player->unk2BC = 0;
    player->unk2C2 = 0;
    player->unk2BC = (player->unk2BC & 0xFFFFFFFE);
    ball->unk155 = 0xC;
    if ((ball->flags & 0x80000000000ULL) == 0) {
        cameras[ball->playerId].mode = 0xE;
    }
    player->unk3C = (lbl_80198B38 * 2);
    fn_10_24C8(ball);
}

void fn_10_24C8(struct Ball *ball)
{
    struct RacePlayer *player;
    struct Ball *b = ball;

    player = (struct RacePlayer *)ball->unk150;
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
    ball->vel.y += ball->accel;
    ball->pos.y += ball->vel.y;
    if (player->unk3C == 0) {
        ball->flags &= ~0x240ULL;
        ball->unk155 = 0xD;
    }
}

void fn_10_25AC(struct Ball *ball)
{
    struct Race2Course *course;
    s16 r5;
    struct RacePlayer *player;

    course = &lbl_10_bss_1208;
    player = (struct RacePlayer *)ball->unk150;
    if ((ball->flags & 0x80000000000ULL) == 0) {
        destroy_sprite_with_unique_id(ball->playerId + 0x6C);
        if ((s32) (course->unk2 & 0x10) != 0) {
            fn_10_1ECC0(ball->playerId);
        } else {
            fn_10_1D9F0(ball->playerId);
        }
    }
    for (r5 = 0; r5 < 8; r5++) {
        if (player == lbl_10_bss_2D68[r5]) {
            ball->pos = lbl_10_bss_4260[r5];
            break;
        }
    }
    player->flags |= 0x40000;
    ball->prevPos = ball->pos;
    ball->vel = (Vec){0.0f, 0.0f, 0.0f};
    ball->unkB0 = (Quat){0.0f, 0.0f, 0.0f, 1.0f};
    ball->unkA0 = ball->unkB0;
    ball->ape->rot = (Quat){0.0f, 0.0f, 0.0f, 1.0f};
    if ((ball->flags & 0x80000000000ULL) == 0) {
        cameras[ball->playerId].mode = 0x3C;
        cameras[ball->playerId].unk1F = 6;
    }
    ball->unk155 = 0xE;
}

void fn_10_285C(struct Ball *ball)
{
    struct RacePlayer *player = (struct RacePlayer *)ball->unk150;
    s16 voices[] = {0x130, 0x12F, 0x12E, 0x12E, 0x12E, 0x12E, 0x12E, 0x12E};
    s16 r5;

    if (((ball->flags & 0x80000000000ULL) == 0) && ((u32) (lbl_10_bss_2D38 % ((lbl_80198B38 * 2) + (ball->playerId * 0x19))) == 0)) {
        call_SoundReqID_arg_0(voices[player->unk3E - 1] | 0x2800);
    }
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
    ball->vel.y -= ball->accel;
    ball->pos.y += ball->vel.y;
    for (r5 = 0; r5 < 8; r5++) {
        if (player == lbl_10_bss_2D68[r5]) {
            ball->pos.x = lbl_10_bss_4260[r5].x;
            ball->pos.z = lbl_10_bss_4260[r5].z;
            break;
        }
    }
}

void fn_10_2A34(struct Ball *ball)
{
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    if ((ball->flags & 0x80000000000ULL) == 0) {
        destroy_sprite_with_unique_id(ball->playerId + 0x6C);
        if ((u32) (player->flags & 2) != 0) {
            fn_10_1D9F0(ball->playerId);
        } else {
            g_fade_track_volume(0, 8);
            lbl_10_bss_122C = (2.5 * lbl_80198B34);
            fn_1_3061C(0x164, 0x165);
            fn_10_1E614(ball->playerId);
        }
    }
    player->flags |= 0x80000;
    player->flags &= 0xFFFFFFF7;
    ball->unk155 = 0x10;
    fn_10_2B4C(ball);
}

void fn_10_2B4C(struct Ball *ball)
{
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
    ball->vel.y -= ball->accel;
    if (player->unk8 > 5) {
        PSVECScale(&ball->vel, &ball->vel, 0.97f * player->unk218);
    }
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    if (fn_10_5840(ball) != 0) {
        call_SoundReqID_arg_0(0x1D);
        call_SoundReqID_arg_0(0x15);
        player->unk208 = player->unk1F0;
        player->unk20C = player->unk1F4;
        player->unk210 = player->unk1F8;
        player->unk214 = player->unk1FC;
        ball->unk155 = 0x11;
        if ((ball->flags & 0x80000000000ULL) == 0) {
            cameras[ball->playerId].unk1F = 4;
        }
        player->unk3C = 0x3C;
    }
}

void fn_10_2CCC(struct Ball *ball)
{

    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
    fn_10_4AFC(ball, 0);
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
}

void fn_10_2D6C(struct Ball *ball)
{
    struct RacePlayer *player;

    if (ball->status == 2) {
        player = (struct RacePlayer *)ball->unk150;
        if (player != NULL) {
            if (player->unk3C > 0) {
                player->unk3C--;
            }
            if (player->unk1EC > 0) {
                player->unk1EC--;
            }
            if (player->unk2BA > 0) {
                player->unk2BA--;
                if (player->unk2BA == 0) {
                    player->flags &= 0xFFFFFFFB;
                }
            }
            if (player->unk304[0] > 0) {
                player->unk304[0]--;
            }
            if (player->unk304[1] > 0) {
                player->unk304[1]--;
            }
            if (player->unk304[2] > 0) {
                player->unk304[2]--;
            }
            if (player->unk304[3] > 0) {
                player->unk304[3]--;
            }
            if (player->unk304[4] > 0) {
                player->unk304[4]--;
            }
            if (player->unk304[5] > 0) {
                player->unk304[5]--;
            }
            if (player->unk304[6] > 0) {
                player->unk304[6]--;
            }
            if (player->unk304[7] > 0) {
                player->unk304[7]--;
            }
            if (player->unk304[8] > 0) {
                player->unk304[8]--;
            }
            if (player->unk304[9] > 0) {
                player->unk304[9]--;
            }
            if (player->unk318 >= 0) {
                player->unk318--;
            }
            if (player->unk2B8 > 0) {
                player->unk2B8--;
            }
            if (player->unk2CE > 0) {
                player->unk2CE--;
            }
            if (player->unk336 > 0) {
                player->unk336--;
            }
        }
    }
}

void fn_10_2F3C(struct Ball *ball)
{
    struct RacePlayer *player;
    s16 r30;
    s32 r29;
    int r3;
    u16 first;
    u16 mask;

    player = (struct RacePlayer *)ball->unk150;
    r29 = 0;
    if ((u32) (player->flags & 0x8000) != 0) {
        player->flags &= 0xFFFF7FFF;
        first = player->unk2C8[0];
        for (r30 = 1; r30 < 3; r30++) {
            player->unk2C8[r30 - 1] = player->unk2C8[r30];
            player->unk2C8[r30] = 0;
        }
        for (r30 = 0; r30 < 3; r30++) {
            if (player->unk2C8[r30] == 0) {
                player->unk2C8[r30] = first;
                break;
            }
        }
    }
    if ((u32) (player->flags & 0x4000) != 0) {
        player->flags &= 0xFFFFBFFF;
        player->unk2C0 |= player->unk2C8[0];
        for (r30 = 1; r30 < 3; r30++) {
            player->unk2C8[r30 - 1] = player->unk2C8[r30];
            player->unk2C8[r30] = 0;
        }
    }
    if (player->unk2C6 != 0) {
        if ((s32) (player->unk2C6 & 0xC) == 0xC) {
            if ((s32) (fn_10_188C0() & 1) != 0) {
                r3 = 4;
            } else {
                r3 = 8;
            }
            player->unk2C6 = (player->unk2C6 & ~r3);
        }
        if (((s32) (player->unk2C6 & 4) != 0) && ((s32) (player->unk2C2 & 8) != 0)) {
            player->unk2C2 = (player->unk2C2 & 0xFFFFFFF7);
        } else if (((s32) (player->unk2C6 & 8) != 0) && ((s32) (player->unk2C2 & 4) != 0)) {
            player->unk2C2 = (player->unk2C2 & 0xFFFFFFFB);
        }
        if ((ball->flags & 0x80000000000ULL) == 0) {
            fn_10_689C(ball);
        }
    }
    if ((ball->unk154 == 1) && ((ball->flags & 0x80000000000ULL) == 0) && (player->unk2C2 != 0) && (fn_10_68D0(ball) != 0)) {
        r29 = 1;
        player->unk2C2 = (player->unk2C2 & 0x100);
    }
    for (r30 = 0; r30 < 0xA; r30++) {
        fn_10_328C(ball, r30);
        g_references_420_06(ball, r30);
    }
    mask = 0x9C;
    if ((ball->unk154 == 1) && ((s32) (player->unk2C4 & mask) != 0) && ((s32) (player->unk2C2 & mask) == 0)) {
        r29 = 1;
    }
    if (r29 != 0) {
        call_SoundReqID_arg_0(0x168);
    }
}

void fn_10_328C(struct Ball *ball, s16 arg1)
{
    struct Effect spD8;
    struct Effect sp28;
    s16 r5;
    u8 tr27;
    struct RacePlayer *tr26;
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    if ((s32) (player->unk2C0 & (1 << arg1)) != 0) {
        player->unk2C0 = (player->unk2C0 & ~(1 << arg1));
        player->unk2BC = (player->unk2BC | (1 << arg1));
        player->unk2CE = 5;
        switch (arg1) {                             /* switch 1; irregular */
        case 0:                                     /* switch 1 */
            memset(&spD8, 0, 0xB0);
            if ((ball->flags & 0x80000000000ULL) == 0) {
                spD8.type = 2;
                spD8.playerId = ball->playerId;
                fn_10_DDA8(&spD8, NULL);
            }
            if (player->unk304[0] == 0) {
                spD8.type = 8;
                spD8.playerId = ball->playerId;
                fn_10_DDA8(&spD8, NULL);
            }
            call_SoundReqID_arg_0(0x2852);
            call_SoundReqID_arg_0(0xD3);
            player->unk304[0] += 0x168;
            if (player->unk304[0] < 0) {
                player->unk304[0] = 0x7FFF;
            }
            break;
        case 5:                                     /* switch 1 */
            if (player->unk304[5] < 0x78) {
                call_SoundReqID_arg_0(0x28D8);
            }
            call_SoundReqID_arg_0(0x52);
            player->unk304[5] += 0x1E0;
            if (player->unk304[5] < 0) {
                player->unk304[5] = 0x7FFF;
            }
            break;
        case 6:                                     /* switch 1 */
            for (r5 = 0; r5 < player_count; r5++) {
                player->unk2F4[r5] = lbl_10_bss_2D68[r5]->unk0;
            }
            player->unk2FC = (player->unk3E - 1);
            player->unk304[6] = (lbl_80198B38 * 0xA);
            player->unk2FD = 0;
            break;
        }
    }
    if (((s32) (player->unk2BE & (1 << arg1)) == 0) && ((s32) (player->unk2BC & (1 << arg1)) != 0)) {
        switch (arg1) {                             /* switch 2; irregular */
        case 1:                                     /* switch 2 */
            fn_10_66AC(ball, 2);
            player->unk2BC = (player->unk2BC & ~(1 << arg1));
            break;
        case 4:                                     /* switch 2 */
            fn_10_62E0(ball, 3);
            player->unk2BC = (player->unk2BC & ~(1 << arg1));
            break;
        case 2:                                     /* switch 2 */
            fn_10_62E0(ball, 4);
            player->unk2BC = (player->unk2BC & ~(1 << arg1));
            break;
        case 3:                                     /* switch 2 */
            fn_10_62E0(ball, 5);
            player->unk2BC = (player->unk2BC & ~(1 << arg1));
            break;
        case 7:                                     /* switch 2 */
            fn_10_62E0(ball, 6);
            player->unk2BC = (player->unk2BC & ~(1 << arg1));
            break;
        }
    }
    if ((s32) (player->unk2BC & (1 << arg1)) != 0) {
        switch (arg1) {                             /* switch 3; irregular */
        case 0:                                     /* switch 3 */
        {
            s32 sp8[8] = {0x3A, 0x3B, 0x3F, 0x53, 0x54, 0x55, 0x56, 0x57};

            ball->ape->flags |= 0x20000;
            if ((s32) (player->unk2C2 & 4) == 0) {
                if (mathutil_vec_sq_len(&ball->vel) > 0.3935185f) {
                    ball->ape->flags &= 0xFFFDFFFF;
                    if ((u32) (lbl_10_bss_2D38 % (((rand() & 0x7FFF) % 5) + 8)) == 0) {
                        call_SoundReqID_arg_0(sp8[(rand() & 0x7FFF) % 8]);
                    }
                }
            }
            break;
        }
        case 5:                                     /* switch 3 */
            if (player->unk304[5] < 0x78) {
                ball->modelScale -= 0.008333334f;
                if (ball->modelScale < 1.0f) {
                    ball->modelScale = 1.0f;
                }
            } else if (ball->modelScale < 2.0f) {
                ball->modelScale += 0.008333334f;
                if (ball->modelScale > 2.0f) {
                    ball->modelScale = 2.0f;
                }
            }
            ball->currRadius = (ball->unk14C * ball->modelScale);
            ball->ape->scale = ball->modelScale;
            player->unk30 = (ball->currRadius * (ball->currRadius * ball->currRadius));
            break;
        case 6:                                     /* switch 3 */
            if ((s32) (player->unk304[6] % 10) == 0) {
                tr27 = player->unk2FD;
                tr26 = &lbl_10_bss_1230[(u8) player->unk2F4[tr27]];
                memset(&sp28, 0, 0xB0);
                sp28.type = 3;
                sp28.playerId = ball->playerId;
                fn_10_DDA8(&sp28, NULL);
                call_SoundReqID_arg_0(0xE2);
                if (tr27 < player->unk2FC) {
                    tr26->unk2C6 = (tr26->unk2C6 | 0x100);
                }
                player->unk2FD++;
                if (player->unk2FD >= (player_count - 1)) {
                    player->unk304[6] = 0;
                }
            }
            break;
        }
        if (player->unk304[arg1] <= 0) {
            player->unk2BC = (player->unk2BC & ~(1 << arg1));
        }
    }
    if (((s32) (player->unk2BE & (1 << arg1)) != 0) && ((s32) (player->unk2BC & (1 << arg1)) == 0)) {
        switch (arg1) {                             /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            ball->ape->flags |= 0x20000;
            break;
        case 5:                                     /* switch 4 */
            ball->currRadius = ball->unk14C;
            ball->modelScale = 1.0f;
            ball->ape->scale = ball->modelScale;
            player->unk30 = (ball->currRadius * (ball->currRadius * ball->currRadius));
            break;
        }
        player->unk304[arg1] = 0;
    }
}

void g_references_420_06(struct Ball *ball, s16 arg1)
{
    struct Effect sp4A8;
    struct Effect sp3F8;
    struct Effect sp348;
    struct Effect sp298;
    struct Effect sp1E8;
    struct Effect sp138;
    struct Effect sp88;
    Vec sp7C;
    Vec sp70;
    Vec sp64;
    Quat sp54;
    struct Race2EffectLink sp28;
    struct Race2EffectLink sp20;
    struct Race2EffectLink sp18;
    struct Race2EffectLink sp10;
    float spC;
    s16 sp8;
    float tf3_4;
    float tf3_5;
    float tf3_3;
    float f2;
    s32 tr3_13;
    s16 r26;
    s16 r25;
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    if ((s32) (player->unk2C6 & (1 << arg1)) != 0) {
        player->unk2C6 = (player->unk2C6 & ~(1 << arg1));
        player->unk2C2 = (player->unk2C2 | (1 << arg1));
        if (arg1 != 8) {
            call_SoundReqID_arg_0(0x281C);
            if ((ball->flags & 0x80000000000ULL) == 0) {
                g_rumble_controller(player->unk14, 1, (s32) (0.5 * lbl_80198B34));
            }
        }
        switch (arg1) {                             /* switch 1; irregular */
        case 1:                                     /* switch 1 */
            player->flags |= 0x40;
            player->unk304[1] = 0x3C;
            call_SoundReqID_arg_0(0xD7);
            if (mathutil_vec_sq_len(&ball->vel) > 0.17361111111111113) {
                vec_set_len(0.41666666f, &ball->vel, &ball->vel);
            }
            break;
        case 4:                                     /* switch 1 */
            if (mathutil_vec_sq_len(&ball->vel) > 0.17361111111111113) {
                vec_set_len(0.41666666f, &ball->vel, &ball->vel);
            }
            memset(&sp4A8, 0, 0xB0);
            sp4A8.type = 0;
            sp4A8.playerId = ball->playerId;
            sp4A8.pos = ball->pos;
            sp4A8.scale = (Vec){4.0f, 4.0f, 4.0f};
            fn_10_DDA8(&sp4A8, NULL);
            sp7C = (Vec){0.0f, 30.0f * ball->accel, 0.0f};
            mtxa_from_rotate_y(0);
            mtxa_rotate_x(fn_10_188C0() % 2048);
            mtxa_rotate_z(fn_10_188C0() % 2048);
            mtxa_tf_vec(&sp7C, &sp7C);
            PSVECAdd(&ball->vel, &sp7C, &ball->vel);
            player->unk304[4] = 0x12C;
            ball->flags &= ~0x1ULL;
            ball->prevFlags &= ~0x1ULL;
            call_SoundReqID_arg_0(0x290B);
            break;
        case 2:                                     /* switch 1 */
            if ((s32) (player->unk2C4 & 4) == 0) {
                memset(&sp3F8, 0, 0xB0);
                sp3F8.playerId = ball->playerId;
                sp3F8.type = 6;
                sp28.slot0 = player->unk2C4;
                sp28.slot1 = 4;
                fn_10_DDA8(&sp3F8, &sp28);
                player->flags |= 0x20000;
            }
            player->unk304[2] = 0x1A4;
            fn_10_5B30(ball, &player->unk2D4);
            player->unk2D0 = 0.25f;
            ball->ape->flags |= 0x20000;
            tf3_3 = ball->vel.y;
            ball->vel.y = (tf3_3 + (15.0f * ball->accel));
            player->flags |= 8;
            call_SoundReqID_arg_0(0xD9);
            break;
        case 3:                                     /* switch 1 */
            if ((s32) (player->unk2C4 & 8) == 0) {
                memset(&sp348, 0, 0xB0);
                sp348.type = 6;
                sp348.playerId = ball->playerId;
                sp20.slot0 = player->unk2C4;
                sp20.slot1 = 8;
                fn_10_DDA8(&sp348, &sp20);
                player->flags |= 0x20000;
            }
            player->unk304[3] = 0x1E0;
            tf3_4 = ball->vel.y;
            ball->vel.y = (tf3_4 + (10.0f * ball->accel));
            player->flags |= 8;
            call_SoundReqID_arg_0(0xD9);
            break;
        case 7:                                     /* switch 1 */
            if (player->unk304[7] == 0) {
                memset(&sp298, 0, 0xB0);
                sp298.type = 5;
                sp298.playerId = ball->playerId;
                fn_10_DDA8(&sp298, NULL);
            }
            player->unk304[7] = 0x1E0;
            tf3_5 = ball->vel.y;
            ball->vel.y = (tf3_5 + (10.0f * ball->accel));
            call_SoundReqID_arg_0(0xD9);
            break;
        }
    }
    if (((s32) (player->unk2C4 & (1 << arg1)) == 0) && ((s32) (player->unk2C2 & (1 << arg1)) != 0)) {
        switch (arg1) {
        case 8:
            if ((u32) (player->flags & 2) != 0) {
                player->unk2C2 = (player->unk2C2 & ~(1 << arg1));
            } else {
                player->unk304[8] = 0x3C;
            }
            break;
        }
    }
    if ((s32) (player->unk2C2 & (1 << arg1)) != 0) {
        switch (arg1) {                         /* switch 2 */
        case 1:                                 /* switch 2 */
            if (player->unk8 > 5) {
                sp70 = (Vec){0.01f * ball->vel.x, 0.0f, 0.01f * ball->vel.z};
                mtxa_from_rotate_y((s16) (500.0f * math_sin((s16) (lbl_10_bss_2D38 * 0x5DC))));
                PSVECAdd(&ball->vel, &sp70, &ball->vel);
                mtxa_tf_vec(&ball->vel, &ball->vel);
            }
            break;
        case 4:                                 /* switch 2 */
            if ((ball->prevFlags & 0x1ULL) != 0) {
                sp64.x = 0.0f;
                sp64.y = (0.5 * (30.0f * ball->accel) * ((float) player->unk304[4] / 300.0f));
                sp64.z = 0.0f;
                mtxa_from_rotate_y(0);
                mtxa_rotate_x(fn_10_188C0() % 2048);
                mtxa_rotate_z(fn_10_188C0() % 2048);
                mtxa_tf_vec(&sp64, &sp64);
                PSVECAdd(&ball->vel, &sp64, &ball->vel);
            }
            break;
        case 2:                                 /* switch 2 */
            player->unk2E4 = player->unk2D4;
            fn_10_5B30(ball, &sp54);
            quat_slerp(&player->unk2D4, &player->unk2E4, &sp54, 0.1f);
            if ((player->unk8 == 5) || ((s32) (player->unk304[2] % 30) == 0)) {
                r26 = 0;
                r25 = 0;
                if (player->unk8 >= 5) {
                    f2 = math_sqrt(mathutil_vec_sq_len(&ball->vel));
                        f2 = ((216.0f * f2) - 10.0f) / 110.0f;
                    if (f2 < 0.0f) {
                        f2 = 0.0f;
                    } else if (f2 > 1.0f) {
                        f2 = 1.0f;
                    }
                    r25 = (s32) (8191.0f * f2) + 0x2000;
                    r26 = r25;
                }
                tr3_13 = call_SoundReqID_arg_1_copy(0x12A);
                SoundPitch((u16) tr3_13, (u16) r26);
                SoundDop((u16) tr3_13, (u16) r25);
            }
            player->flags |= 8;
            break;
        case 3:                                 /* switch 2 */
            fn_10_5C1C(ball);
            player->flags |= 8;
            break;
        case 8:                                 /* switch 2 */
            if (player->unk304[8] == 0x1E) {
                memset(&sp1E8, 0, 0xB0);
                sp1E8.type = 4;
                sp1E8.playerId = ball->playerId;
                fn_10_DDA8(&sp1E8, NULL);
            } else if (player->unk304[8] == 0) {
                player->unk2C6 = (player->unk2C6 | 0x10);
            }
            break;
        case 7:                                 /* switch 2 */
            if (player->unk304[7] > (s32) (0x1E0 - (lbl_80198B38 * 3))) {
                sp8 = 0x1E0 - player->unk304[7];
                OSs16tof32(&sp8, &spC);
                spC *= 1.0 / (3.0f * lbl_80198B34);
                player->unk300 = 1.0 + ((lbl_1_data_65774 - 1.0) * spC);
                player->unk218 = 1.0 + ((lbl_1_data_65778 - 1.0) * spC);
            } else {
                player->unk300 = lbl_1_data_65774;
                player->unk218 = lbl_1_data_65778;
            }
            break;
        }
        if (player->unk304[arg1] <= 0) {
            player->unk2C2 = (player->unk2C2 & ~(1 << arg1));
        }
    }
    if (((s32) (player->unk2C4 & (1 << arg1)) != 0) && ((s32) (player->unk2C2 & (1 << arg1)) == 0)) {
        switch (arg1) {                             /* switch 3; irregular */
        case 1:                                     /* switch 3 */
            player->flags &= 0xFFFFFFBF;
            break;
        case 2:                                     /* switch 3 */
            memset(&sp138, 0, 0xB0);
            sp138.type = 6;
            sp138.playerId = ball->playerId;
            sp18.slot0 = 4;
            sp18.slot1 = 0;
            fn_10_DDA8(&sp138, &sp18);
            player->flags |= 0x20000;
            player->flags &= 0xFFFFFFF7;
            player->unk2D0 = 1.0f;
            if (player->unk31C != -1) {
                SoundOff(player->unk31C);
                player->unk31C = -1;
            }
            break;
        case 3:                                     /* switch 3 */
            memset(&sp88, 0, 0xB0);
            sp88.type = 6;
            sp88.playerId = ball->playerId;
            sp10.slot0 = 8;
            sp10.slot1 = 0;
            fn_10_DDA8(&sp88, &sp10);
            player->flags |= 0x20000;
            player->flags &= 0xFFFFFFF7;
            ball->currRadius = (ball->unk14C * ball->modelScale);
            break;
        case 7:                                     /* switch 3 */
            player->unk300 = 1.0f;
            player->unk218 = lbl_1_data_6573C;
            break;
        }
        player->unk304[arg1] = 0;
    }
}

void fn_10_45A8(struct Ball *ball)
{
    struct RacePlayer *player;
    s16 r30;
    struct Camera *tr28;
    float *tr27;
    float tf1;

    player = (struct RacePlayer *)ball->unk150;
    if (player != 0) {
        for (r30 = 0; r30 < 5; r30++) {
            tr28 = &cameras[r30];
            tr27 = &player->unk320[r30];
            if (((s32) (tr28->flags & 0x40) == 0) && (r30 == ball->playerId)) {
                *tr27 = -1.0f;
            } else {
                tf1 = mathutil_vec_distance(&tr28->eye, &ball->pos);
                *tr27 = (240.0f * tr28->sub28.viewportH) / (tr28->sub28.view.tanHalfFov * tf1);
                *tr27 *= ball->modelScale;
            }
        }
    }
}

void fn_10_46C8(struct Ball *ball)
{
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    if (((ball->flags & 0x80000000000ULL) == 0) && ((s32) (player->unk4 & 0x800) != 0)) {
        player->flags |= 0x8000;
    }
    if ((player->unk2C8[0] != 0) && (player->unk2CE <= 0)) {
        if ((ball->flags & 0x80000000000ULL) != 0) {
            if (fn_10_47C4(ball) != 0) {
                player->flags |= 0x4000;
            }
        } else if ((s32) (player->unk4 & 0x100) != 0) {
            player->flags |= 0x4000;
        }
    }
}

s32 fn_10_47C4(struct Ball *ball)
{
    float tf31;
    struct RacePlayer *player;
    struct Ball *r29;
    struct RacePlayer *tr28;
    s16 r27;
    s16 n;

    player = (struct RacePlayer *)ball->unk150;
    n = 15;
    tf31 = 0.9f;
    if ((u32) (dip_switches & 0x20) != 0) {
        return 0;
    }
    if (player->unk2C8[0] == 0) {
        return 0;
    }
    if ((u32) ((lbl_10_bss_2D38 + (ball->playerId * 8)) % n) != 0) {
        return 0;
    }
    if ((player->unk2C8[0] == 1) || (player->unk2C8[0] == 2) || (player->unk2C8[0] == 0x20) || (player->unk2C8[0] == 0x40)) {
        return 1;
    }
    for (r27 = 1; r27 < 3; r27++) {
        if ((player->unk2C8[r27] == 1) || (player->unk2C8[r27] == 2) || (player->unk2C8[r27] == 0x20) || (player->unk2C8[r27] == 0x40)) {
            player->flags |= 0x8000;
            return 0;
        }
    }
    r27 = 0;
    r29 = balls;
    for (; r27 < 8; r27++, r29++) {
        if ((r29->status == 2) && (r29->playerId != player->unk0)) {
            tr28 = (struct RacePlayer *)r29->unk150;
            if ((tr28 != 0) && ((u32) (tr28->flags & 2) == 0) && ((u32) (tr28->flags & 0x10) == 0) && ((u32) (tr28->flags & 0x20) == 0) && (((r29->flags & 0x80000000000ULL) == 0) || ((balls[lbl_10_bss_2D68[0]->unk0].flags & 0x80000000000ULL) != 0) || !(fn_10_18900() < 0.95)) && (tr28->unk2C2 == 0) && (tr28->unk3E <= player->unk3E) && (tr28->unk2B8 <= 0) && !(player->unk280[r27] > 40.0f) && !(fn_10_18900() > tf31)) {
                if (vec_dot_normalized_safe(&ball->vel, &player->unk220[r27]) >= 0.980785) {
                    tr28->unk2B8 = 0x3C;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void fn_10_4AFC(struct Ball *ball, s32 arg1)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct Race2CourseInfo *tr29 = &lbl_10_data_2ADC[course->unk0];
    struct RacePlayer *player = (struct RacePlayer *)ball->unk150;
    Vec sp20 = {0.0f, 0.0f, 0.0f};
    Vec sp14;
    Vec sp8;
    float tf0_2;
    float f2;
    float f2_2;
    float tf5;

    if (arg1 != 0) {
        if (((u32) (dip_switches & 1) != 0) && ((u32) (lbl_1_bss_D3360 & 8) != 0) && ((s32) (course->unk2 & 0x10) != 0)) {
            fn_10_51A8(ball, &sp20);
        } else if ((ball->flags & 0x80000000000ULL) != 0) {
            fn_10_51A8(ball, &sp20);
        } else {
            fn_10_4F70(ball, &sp20);
        }
        if ((u32) (player->flags & 0x40) == 0) {
            if (player->unk8 < 5) {
                f2 = (1.0 - (player->unkA / (0.5 * lbl_80198B34)));
                if (f2 < 0.0f) {
                    f2 = 0.0f;
                } else if (f2 > 1.0f) {
                    f2 = 1.0f;
                }
                sp20.x *= 0.28f * f2;
                sp20.z *= 0.28f * f2;
            }
            if (player->unk336 > 0) {
                PSVECScale(&ball->vel, &ball->vel, lbl_1_data_6576C);
            }
        }
    }
    if ((ball->flags & 0x200ULL) != 0) {
        sp20.y = ball->accel;
    } else if ((ball->flags & 0x100ULL) != 0) {
        sp20.y = 0.0f;
    } else {
        sp20.y = -ball->accel * player->unk300;
    }
    PSVECAdd(&ball->vel, &sp20, &ball->vel);
    PSVECScale(&ball->vel, &ball->vel, player->unk218 + player->unk21C);
    if (((ball->flags & 0x80000000000ULL) == 0) && ((u32) (player->flags & 1) == 0) && (player->unk2C2 == 0) && (player->unk8 >= 5)) {
        if (!(mathutil_vec_len(&ball->vel) < (lbl_1_data_65748 / 216.0f))) {
            sp14 = player->unk18;
            PSVECSubtract(&sp14, &ball->pos, &sp14);
            vec_normalize_len(&sp14);
            tf0_2 = mathutil_vec_dot_prod(&ball->vel, &sp14);
            if (tf0_2 < 0.0f) {
                tf5 = tr29->unk5C;
                if (player->unk1F0 < 25.0 * course->unkC) {
                    f2_2 = player->unk1F0;
                f2_2 = f2_2 / (25.0 * course->unkC);
                } else if (player->unk1F0 > 100.0 - (25.0 * course->unkC)) {
                    f2_2 = ((100.0 - player->unk1F0) / (25.0 * course->unkC));
                } else {
                    f2_2 = 1.0f;
                }
                tf0_2 *= f2_2;
                PSVECScale(&sp14, &sp8, -tf0_2 * tf5);
                mathutil_vec_len(&ball->vel);
                PSVECAdd(&ball->vel, &sp8, &ball->vel);
            }
        }
    }
}

void fn_10_4F70(struct Ball *ball, Vec *arg1)
{
    struct RacePlayer *player;
    u16 r29;
    float tf31;
    u16 tr3_2;

    player = (struct RacePlayer *)ball->unk150;
    fn_10_679C(ball, arg1);
    if (mathutil_vec_sq_len(&ball->vel) > 0.01) {
        r29 = ((u16)math_atan2(ball->vel.x, ball->vel.z) - 0x8000) - player->unk6;
        mtxa_from_rotate_y((s16) r29);
        mtxa_rigid_inv_tf_vec(arg1, arg1);
    } else {
        r29 = 0;
    }
    tf31 = fn_10_5968(ball);
    tr3_2 = (u16)math_atan2(arg1->x, -arg1->z);
    if ((tr3_2 < 0x3000) || ((tr3_2 > 0xD000) && (tr3_2 < 0xFFFF))) {
        arg1->z *= 1.0f / math_sin((s16) (tr3_2 + 0x4000));
    } else if (((tr3_2 > 0x4000) && (tr3_2 < 0x6000)) || ((tr3_2 > 0xA000) && (tr3_2 < 0xC000))) {
        arg1->z = 0.0f;
    }
    arg1->x *= tf31 * (player->unk2D0 * lbl_1_data_65730);
    if (arg1->z <= 0.0f) {
        arg1->z *= tf31 * (player->unk2D0 * lbl_1_data_6572C);
    } else {
        arg1->z *= tf31 * (player->unk2D0 * lbl_1_data_65734);
    }
    if ((s32) (player->unk2BC & 1) != 0) {
        arg1->x *= 1.5f;
        arg1->z *= 1.5f;
    }
    mtxa_from_rotate_y((s16) r29);
    mtxa_tf_vec(arg1, arg1);
    mtxa_from_rotate_y(player->unk6);
    mtxa_tf_vec(arg1, arg1);
}

void fn_10_51A8(struct Ball *ball, Vec *arg1)
{
    struct Race2Course *course;
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    struct Race2Seg *r5;
    float tf0;
    float tf2;
    float f31;
    float tf30;
    float tf3;
    float tf4;
    struct RacePlayer *player;
    s16 tr29;
    s32 r0;
    struct StageExtraChannels *r3;
    struct Race2Seg *r6;

    course = &lbl_10_bss_1208;
    player = (struct RacePlayer *)ball->unk150;
    r3 = &stagedef->extraData->list[player->unk2A0];
    if (((u32) (dip_switches & 1) != 0) && ((u32) (lbl_1_bss_D3360 & 8) != 0)) {
        if (ball->playerId != 0) {
            *arg1 = (Vec){0.0f, 0.0f, 0.0f};
            return;
        }
        r3 = &stagedef->extraData->list[lbl_1_data_65728];
        r5 = (struct Race2Seg *)lbl_1_bss_D3388;
        r6 = r5 + 1;
    } else {
        r5 = (struct Race2Seg *)lbl_10_data_2ADC[course->unk0].unk1C[ball->playerId - 1];
        r6 = r5 + 1;
    }
    r0 = 0;
    for (; r6->unk0 >= 0.0f; r5++, r6++) {
        if ((r5->unk0 <= player->unk2A4) && (player->unk2A4 <= r6->unk0)) {
            r0 = 1;
            break;
        }
    }
    if (r0 == 0) {
        *arg1 = (Vec){0.0f, 0.0f, 0.0f};
        return;
    }
    tf3 = player->unk2A4 - r5->unk0;
    tf3 /= r6->unk0 - r5->unk0;
    tf4 = 1.0f - tf3;
    tf30 = (r5->unk4 * tf4) + (r6->unk4 * tf3);
    tr29 = ((r5->unk8 * tf4) + (r6->unk8 * tf3));
    f31 = (r5->unkC * tf4) + (r6->unkC * tf3);
    if ((current_stage_id == 0x86) && (94.0 < player->unk208) && (player->unk208 < 96.34) && (12.4 < ball->pos.z) && (ball->pos.z < 32.5) && (-26.0 < ball->pos.x) && (ball->pos.x < -11.6)) {
        mtxa_from_rotate_y(-0x8000);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp44);
        f31 = 0.0f;
        vec_set_len(player->unk2D0 * (lbl_1_data_6572C * tf30), &sp44, arg1);
    } else if ((current_stage_id == 0x88) && (57.0 < player->unk208) && (player->unk208 < 61.0) && (120.0 < ball->pos.z) && (ball->pos.z < 140.0) && (ball->pos.y <= -4.5)) {
        mtxa_from_rotate_y(0x6800);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp44);
        f31 = 0.0f;
        vec_set_len(player->unk2D0 * (lbl_1_data_6572C * tf30), &sp44, arg1);
    } else {
        if (player->unk336 > 0) {
            mtxa_from_rotate_y(player->unk338);
            mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp44);
        } else {
            tf2 = player->unk2A4;
            fn_10_181EC((struct StageExtraData *) r3, (float) (tf2 + (5.0 * course->unkC)), &sp44);
            PSVECSubtract(&sp44, &ball->pos, &sp44);
            vec_normalize_len(&sp44);
        }
        vec_set_len(player->unk2D0 * (lbl_1_data_6572C * tf30), &sp44, arg1);
        PSVECSubtract(&player->unk2AC, &ball->pos, &sp44);
        vec_normalize_len(&sp44);
        tf2 = mathutil_vec_dot_prod(&ball->vel, &sp44);
        if (tf2 > 0.0f) {
            tf0 = mathutil_vec_dot_prod(arg1, &sp44);
            if (tf0 > 0.0f) {
                vec_set_len((float) (0.3 * -tf0), &sp44, &sp38);
                PSVECAdd(arg1, &sp38, arg1);
            }
        }
        mtxa_from_rotate_y(tr29);
        mtxa_tf_vec(arg1, arg1);
    }
    if ((s32) (player->unk2BC & 1) != 0) {
        PSVECScale(arg1, arg1, 1.5f);
    }
    if ((f31 > 0.0f) && (player->unk8 > 5) && (player->unk2C2 == 0)) {
        sp2C = (Vec){ball->vel.x, 0.0f, ball->vel.z};
        sp44 = player->unk2AC;
        PSVECSubtract(&sp44, &ball->pos, &sp44);
        sp44.y = 0.0f;
        vec_normalize_len(&sp44);
        tf2 = mathutil_vec_dot_prod(&sp2C, &sp44);
        if (tf2 < 0.0f) {
            PSVECScale(&sp44, &sp44, -tf2 * f31);
            PSVECAdd(&sp2C, &sp44, &sp2C);
        }
        ball->vel.x = sp2C.x;
        ball->vel.z = sp2C.z;
    }
}

s32 fn_10_5840(struct Ball *ball)
{
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    if (did_ball_fallout(ball) != 0) {
        player->flags |= 0x20;
        return 1;
    }
    return 0;
}

void fn_10_588C(struct Ball *ball)
{
    struct Race2Course *course;
    Vec sp14;
    Vec sp8;
    float tf31;
    struct StageExtraData *tr30;
    struct RacePlayer *player;

    course = &lbl_10_bss_1208;
    tr30 = stagedef->extraData;
    player = (struct RacePlayer *)ball->unk150;
    tf31 = 100.0f * (3.0f / course->unk8);
    fn_10_181EC(tr30, player->unk1F0, &sp14);
    fn_10_181EC(tr30, player->unk1F0 + tf31, &sp8);
    PSVECSubtract(&sp8, &sp14, &sp8);
    sp8.y = 0.0f;
    vec_normalize_len(&sp8);
    g_smth_with_ape_rotation(ball->ape, &sp8.x);
}

float fn_10_5968(struct Ball *ball)
{
    float tf3;
    struct Race2Course *course = &lbl_10_bss_1208;
    struct RacePlayer *player = (struct RacePlayer *)ball->unk150;

    if (player_count <= 1) {
        return 1.0f;
    }
    if ((s32) (course->unk2 & 2) == 0) {
        return 1.0f;
    }
    if ((ball->flags & 0x80000000000ULL) != 0) {
        return 1.0f;
    }
    if ((u32) (player->flags & 1) != 0) {
        return 1.0f;
    }
    if (player->unk1F8 < 0.0f) {
        return 1.0f;
    }
    if ((player->unk3E <= 1) && (lbl_10_bss_2D68[1] != NULL)) {
        tf3 = player->unk200 - lbl_10_bss_2D68[1]->unk200;
        if (tf3 > lbl_1_data_65738) {
            float result = lbl_1_data_65784;
            return result;
        }
        return (float) (lbl_1_data_65784 + (((1.0 - lbl_1_data_65784) * tf3) / lbl_1_data_65738));
    }
    tf3 = lbl_10_bss_2D68[0]->unk200 - player->unk200;
    if (tf3 > lbl_1_data_65738) {
        return lbl_1_data_65788;
    }
    return (float) (1.0 + (((lbl_1_data_65788 - 1.0) * tf3) / lbl_1_data_65738));
}

void fn_10_5B30(struct Ball *ball, Quat *arg1)
{
    struct RaycastHit hit;
    Vec sp34;
    Quat sp24;
    Quat sp14;

    sp24 = ball->ape->rot;
    raycast_stage_down(&ball->pos, &hit, NULL);
    if ((u32) (hit.flags & 1) != 0) {
        sp34 = (Vec){0.0f, 1.0f, 0.0f};
        quat_from_dirs(&sp14, &sp34, &hit.normal);
        quat_mult(arg1, &sp14, &sp24);
        return;
    }
    *arg1 = sp24;
}

void fn_10_5C1C(struct Ball *ball)
{
    Vec sp50[5];
    Vec sp2C[3];
    Vec sp20;
    Vec sp14;
    Vec sp8;
    struct RacePlayer *player;
    u32 *tr29;
    sptr tr28;
    float f31;
    float f30;
    s16 r27;
    s16 r4;
    float tf1;
    float tf31;
    float f1;

    player = (struct RacePlayer *)ball->unk150;
    tr29 = fn_8006991C((sptr) minigameGma->modelEntries[0x15].model);
    tr28 = fn_80069870((sptr) minigameGma->modelEntries[0x15].model);
    f31 = -1.0f;
    f30 = -1.0f;
    mtxa_from_mtx(ball->unk30);
    mtxa_scale_s(ball->modelScale);
    r27 = 0;
    while ((u32) r27 < (u32) *tr29) {
        fn_800698C0(tr28 + (r27 << 5), sp50);
        mtxa_tf_vec(sp50, &sp2C[0]);
        fn_800698C0(tr28 + ((r27 + 1) << 5), sp50);
        mtxa_tf_vec(sp50, &sp2C[1]);
        fn_800698C0(tr28 + ((r27 + 2) << 5), sp50);
        mtxa_tf_vec(sp50, &sp2C[2]);
        if (!(sp2C[0].y > 0.0f) && !(sp2C[1].y > 0.0f) && !(sp2C[2].y > 0.0f)) {
            PSVECSubtract(&sp2C[1], &sp2C[0], &sp20);
            PSVECSubtract(&sp2C[2], &sp2C[0], &sp14);
            vec_normalize_len(&sp20);
            vec_normalize_len(&sp14);
            mathutil_vec_cross_prod(&sp20, &sp14, &sp8);
            if (0.0f != sp8.y) {
                tf1 = -(sp2C[0].y + (((sp8.x * sp2C[0].x) + (sp8.z * sp2C[0].z)) / sp8.y));
                if ((f31 < 0.0f) || ((tf1 > 0.0f) && (f31 > tf1))) {
                    f31 = tf1;
                    f1 = sp2C[0].y;
                    for (r4 = 1; r4 < 3; r4++) {
                        if (sp2C[r4].y < f1) {
                            f1 = sp2C[r4].y;
                        }
                    }
                    f30 = -f1;
                }
            }
        }
        r27 += 3;
    }
    if (!(f31 <= 0.0f)) {
        tf31 = f30 - ball->currRadius;
        if (player->unk8 == 1) {
            call_SoundReqID_arg_0(0x133);
        }
        if (player->unk8 > 5) {
            if (tf31 > 0.0f) {
                ball->pos.y += (8.0f * tf31);
            } else {
                ball->pos.y += tf31;
            }
        } else {
            ball->pos.y += tf31;
        }
        ball->currRadius = f30;
    }
}

void fn_10_5F1C(struct Ball *ball)
{
    struct RacePlayer *player = (struct RacePlayer *)ball->unk150;
    struct StageExtraData *tr29 = stagedef->extraData;
    struct Race2Marker *r28 = (struct Race2Marker *)tr29->unk20.frames;
    Vec sp14;
    Vec sp8 = {0.0f, 0.0f, -1.0f};
    s16 r26;

    if ((u32) (player->flags & 0x80) != 0) {
        player->flags |= 0x100;
    } else {
        player->flags &= 0xFFFFFEFF;
    }
    player->flags &= 0xFFFFFF7F;
    for (r26 = 0; r26 < tr29->unk20.count; r26++, r28++) {
        if (mathutil_vec_sq_distance(&ball->pos, &r28->pos) <= 9.0f) {
            mtxa_from_translate(&r28->pos);
            mtxa_rotate_z(r28->rotZ);
            mtxa_rotate_y(r28->rotY);
            mtxa_rotate_x(r28->rotX);
            mtxa_rigid_inv_tf_point(&ball->pos, &sp14);
            sp14.z *= 2.0;
            if (-1.0f <= sp14.x) {
                if (sp14.x <= 1.0f) {
                    if (-1.0f <= sp14.z) {
                        if (sp14.z <= 1.0f) {
                            player->flags |= 0x80;
                            sp8.z *= 0.2f * mathutil_vec_len(&ball->vel);
                            mtxa_tf_vec(&sp8, &sp8);
                            if ((u32) (player->flags & 0x100) == 0) {
                                if (vec_dot_normalized_safe(&ball->vel, &sp8) >= 0.0f) {
                                    call_SoundReqID_arg_0(0x5052);
                                    call_SoundReqID_arg_0(0x3B0D2);
                                } else {
                                    call_SoundReqID_arg_0(0x3B0DD);
                                }
                            }
                            PSVECAdd(&ball->vel, &sp8, &ball->vel);
                            return;
                        }
                    }
                }
            }
        }
    }
}

void fn_10_6164(struct Ball *ball)
{
    Vec sp8;
    struct StageExtraData *tr29;
    struct Race2Marker *r28;
    s16 r27;
    struct RacePlayer *player;

    tr29 = stagedef->extraData;
    r28 = (struct Race2Marker *)tr29->unk30.frames;
    for (r27 = 0; r27 < tr29->unk30.count; r27++, r28++) {
        if (!(mathutil_vec_sq_distance(&ball->pos, &r28->pos) > 4.0f)) {
            mtxa_from_translate(&r28->pos);
            mtxa_rotate_z(r28->rotZ);
            mtxa_rotate_y(r28->rotY);
            mtxa_rotate_x(r28->rotX);
            mtxa_rigid_inv_tf_point(&ball->pos, &sp8);
            if (!(sp8.x < -1.0f) && !(sp8.x > 1.0f) && !(sp8.z < -1.0f) && !(sp8.z > 1.0f)) {
                player = (struct RacePlayer *)ball->unk150;
                if (player->unk2C2 == 0) {
                    return;
                }
                player->unk2C2 = 0;
                call_SoundReqID_arg_0(0x52);
                if ((ball->flags & 0x80000000000ULL) != 0) {
                    return;
                }
                call_SoundReqID_arg_0(0xDC);
                return;
            }
        }
    }
}

void fn_10_62E0(struct Ball *ball, s16 arg1)
{
    struct Item sp2C;
    Vec sp20;
    Vec sp14;
    struct RacePlayer *player;
    struct Race2Item *tr3;
    struct Ball *r28;
    s16 r27;
    s16 r26_2;
    s32 r26;
    float f31;
    float tf1_2;

    player = (struct RacePlayer *)ball->unk150;
    tr3 = fn_10_10BDC();
    if (tr3 != NULL) {
        call_SoundReqID_arg_0(0xE2);
        memset(&sp2C, 0, 0xB4);
        sp2C.type = 2;
        sp2C.subType = arg1;
        sp2C.animGroupId = 0;
        sp2C.pos = ball->pos;
        sp2C.vel = ball->vel;
        if ((ball->flags & 0x80000000000ULL) != 0) {
            if (mathutil_vec_len(&ball->vel) > 0.0f) {
                sp20 = ball->vel;
            } else {
                mtxa_from_quat(&ball->ape->rot);
                mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp20);
            }
        } else {
            mtxa_from_rotate_y(player->unk6);
            mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp20);
        }
        vec_set_len(0.8f, &sp20, &sp20);
        PSVECAdd(&sp2C.vel, &sp20, &sp2C.vel);
        r26 = 0;
        if ((ball->flags & 0x80000000000ULL) != 0) {
            sp14 = sp2C.vel;
            vec_normalize_len(&sp14);
        } else {
            sp14 = (Vec){0.0f, 0.0f, -1.0f};
            mtxa_from_rotate_y(player->unk6);
            mtxa_tf_vec(&sp14, &sp14);
            if (mathutil_vec_dot_prod(&sp14, &sp2C.vel) < 0.0f) {
                r26 = 1;
            }
        }
        r27 = -1;
        if (r26 == 0) {
            f31 = -1.0f;
            r26_2 = 0;
            r28 = balls;
            for (; r26_2 < 8; r26_2++, r28++) {
                if ((r28->status == 2) && (r28->playerId != ball->playerId) && !(player->unk280[r28->playerId] > 40.0f) && !(vec_dot_normalized_safe(&sp14, &player->unk220[r28->playerId]) < 0.0f)) {
                    tf1_2 = vec_dot_normalized_safe(&sp14, &player->unk220[r28->playerId]);
                    tf1_2 *= (1.0f - (player->unk280[r28->playerId] / 40.0f));
                    if (tf1_2 > f31) {
                        r27 = r28->playerId;
                        f31 = tf1_2;
                    }
                }
            }
        }
        tr3->unk0 = 1;
        tr3->unk4 = 0;
        tr3->unk8 = ball->playerId;
        tr3->unkA = r27;
        tr3->unkC = (lbl_80198B38 * 3);
        tr3->unkE = 5;
        tr3->unk10 = 1.0f;
        sp2C.stageBanana = (struct StageBanana *)tr3;
        spawn_item(&sp2C);
    }
}

void fn_10_66AC(struct Ball *ball, s16 arg1)
{
    struct Item sp8;
    struct Race2Item *tr3;

    tr3 = fn_10_10BDC();
    if (tr3 != NULL) {
        call_SoundReqID_arg_0(0xE2);
        memset(&sp8, 0, 0xB4);
        sp8.type = 2;
        sp8.subType = arg1;
        sp8.animGroupId = 0;
        sp8.pos = ball->pos;
        vec_set_len(0.8f, &ball->vel, &sp8.vel);
        tr3->unk0 = 1;
        tr3->unk4 = 0;
        tr3->unk8 = ball->playerId;
        tr3->unkA = -1;
        tr3->unkC = (lbl_80198B38 * 3);
        sp8.stageBanana = (struct StageBanana *)tr3;
        spawn_item(&sp8);
    }
}

static void fn_10_ball_to_physball(struct Ball *ball, struct PhysicsBall *phys) {
    struct RacePlayer *player = (struct RacePlayer *)ball->unk150;

    phys->flags = 0;
    phys->pos = ball->pos;
    phys->prevPos = ball->prevPos;
    phys->vel = ball->vel;
    phys->radius = ball->currRadius;
    phys->gravityAccel = ball->accel;
    phys->restitution = ball->restitution;
    phys->hardestColiSpeed = 0.0f;
    phys->itemGroupId = 0;
    phys->hardestColiItemGroupId = 0;
    phys->friction = 0.01f;
    if ((s32) (player->unk2C2 & 4) != 0) {
        phys->friction *= 0.01f;
    } else if ((s32) (player->unk2C2 & 2) != 0) {
        phys->friction *= 0.5f;
    }
    if ((ball->unk154 == 3) && (player->unk8 <= 5)) {
        float t = 5 - player->unk8;
        t /= 5.0f;
        t *= 10.0f;
        t += 1.0f;
        phys->friction *= t;
    }
}

void fn_10_679C(struct Ball *ball, Vec *arg1)
{
    float spC;
    float sp8;
    struct RacePlayer *player = (struct RacePlayer *)ball->unk150;

    OSs8tof32((s8 *)&player->unk2, &spC);
    OSs8tof32((s8 *)&player->unk3, &sp8);
    sp8 = -sp8;
    if (((u32) (dip_switches & 1) != 0) && ((u32) (lbl_1_bss_D3360 & 0x10) != 0) && (ball->playerId != 0)) {
        spC = 0.0f;
        sp8 = -60.0f;
    }
    arg1->x = (spC / 60.0);
    arg1->y = 0.0f;
    arg1->z = (sp8 / 60.0);
    if (mathutil_vec_sq_len(arg1) > 1.0f) {
        vec_normalize_len(arg1);
    }
}

void fn_10_689C(struct Ball *ball)
{
    struct RacePlayer *player;

    player = &lbl_10_bss_1230[ball->playerId];
    player->unk35A = 0;
    player->unk35C = player->unk2;
    player->unk35D = player->unk3;
}

s32 fn_10_68D0(struct Ball *ball)
{
    s32 da;
    s16 t;
    struct RacePlayer *player;

    player = &lbl_10_bss_1230[ball->playerId];
    t = player->unk35A;
    da = abs((s8) player->unk2 - (s8) player->unk35C);
    player->unk35A = (da + abs((s8) player->unk3 - (s8) player->unk35D)) + t;
    player->unk35C = player->unk2;
    player->unk35D = player->unk3;
    return (s16)player->unk35A > lbl_1_data_65772;
}

void fn_10_6978(void)
{
    s32 r30;
    s32 r29;
    struct Ball *tr28;
    struct Ball *r27;
    struct Ball *r26;
    struct RacePlayer *tr25;
    struct RacePlayer *tr24;
    struct PhysicsBall sp9C;
    struct PhysicsBall sp3C;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    s8 sp9;
    s8 sp8;
    float tf31;
    float tf30;
    float f30;
    float tf1_5;
    s8 tr4_2;
    s32 r3;
    s32 r3_2;
    s32 tr3_2;
    s16 pitch;
    float tf2;

    tr28 = current_ball;
    r30 = 0;
    r27 = balls;
    for (; r30 < (s32) (player_count - 1); r30++, r27++) {
        tr25 = (struct RacePlayer *)r27->unk150;
        if ((u32) (tr25->flags & 0x10) == 0) {
            fn_10_ball_to_physball(r27, &sp9C);
            if ((r27->flags & 0x1ULL) != 0) {
                r3 = 1;
            } else {
                r3 = 0;
            }
            sp9C.flags |= r3;
            r29 = r30 + 1;
            r26 = &balls[r30 + 1];
            for (; r29 < player_count; r29++, r26++) {
                tr24 = (struct RacePlayer *)r26->unk150;
                if ((u32) (tr24->flags & 0x10) == 0) {
                    fn_10_ball_to_physball(r26, &sp3C);
                    if ((r26->flags & 0x1ULL) != 0) {
                        r3_2 = 1;
                    } else {
                        r3_2 = 0;
                    }
                    sp3C.flags |= r3_2;
                    PSVECSubtract(&sp3C.prevPos, &sp9C.prevPos, &sp30);
                    PSVECSubtract(&sp9C.vel, &sp3C.vel, &sp24);
                    PSVECSubtract(&sp9C.pos, &sp3C.pos, &spC);
                    if (((mathutil_vec_dot_prod(&sp30, &sp24)) > 0.0f) && (fn_1_A74E4(&sp9C.prevPos, &sp9C.pos, sp9C.radius, &sp3C.prevPos, &sp3C.pos, sp3C.radius) != 0)) {
                        tr25->flags |= 0x1000;
                        tr24->flags |= 0x1000;
                        tf31 = mathutil_vec_sq_len(&sp9C.vel);
                        tf30 = mathutil_vec_sq_len(&sp3C.vel);
                        g_something_with_item_coli(&spC, &sp9C.vel, tr25->unk30, &sp3C.vel, tr24->unk30);
                        tf31 = fabs(mathutil_vec_sq_len(&sp9C.vel) - tf31);
                        tf30 = fabs(mathutil_vec_sq_len(&sp3C.vel) - tf30);
                        f30 = tf31 + tf30;
                        PSVECAdd(&sp9C.pos, &sp9C.vel, &sp9C.pos);
                        PSVECAdd(&sp3C.pos, &sp3C.vel, &sp3C.pos);
                        if (f30 > 0.001) {
                            g_rumble_controller(tr25->unk14, 1, (s32) (0.3 * lbl_80198B34));
                            g_rumble_controller(tr24->unk14, 1, (s32) (0.3 * lbl_80198B34));
                            current_ball = r27;
                            call_SoundReqID_arg_0(0x17);
                            current_ball = r26;
                            call_SoundReqID_arg_0(0x18);
                            tr4_2 = fn_1_2F8F0(modeCtrl.playerCount, &current_ball->pos, (u8 *)&sp9, (u8 *)&sp8);
                            tr3_2 = call_SoundReqID_arg_1_copy(((sp8 & 0x7F) << 0x19) | ((((tr4_2 & 0x7F) << 0xB) | 0x15B) | ((sp9 & 0x7F) << 0x12)));
                            f30 *= 6.6666665f;
                            if (f30 > 1.0f) {
                                f30 = 1.0f;
                            }
                            pitch = (s32) (131071.0f * f30) + 0x2000;
                            SoundPitch(tr3_2, pitch);
                        }
                    }
                    tf2 = vec_normalize_len(&spC) - 0.01;
                    tf1_5 = (sp9C.radius + sp3C.radius) - tf2;
                    if (tf1_5 > 0.0) {
                        PSVECScale(&spC, &sp18, 0.5 * tf1_5);
                        PSVECAdd(&sp9C.pos, &sp18, &sp9C.pos);
                        PSVECSubtract(&sp3C.pos, &sp18, &sp3C.pos);
                    }
                    apply_physicsball_to_ball(r26, &sp3C);
                }
            }
            apply_physicsball_to_ball(r27, &sp9C);
        }
    }
    r27 = balls;
    r30 = 0;
    for (; r30 < 8; r30++, r27++) {
        if (r27->status == 2) {
            current_ball = r27;
            fn_10_ball_to_physball(r27, &sp9C);
            fn_10_74A4(&sp9C, stagedef);
            apply_physicsball_to_ball(r27, &sp9C);
            fn_1_4AF48(r27, &sp9C);
            position_ball(r27, &sp9C);
            r27->unk80++;
        }
    }
    current_ball = tr28;
}

void fn_10_7250(void)
{
    Vec sp14[8];
    Vec sp8;
    struct Ball *r30;
    struct Ball *r29;
    struct RacePlayer *r28;
    struct RacePlayer *r27;
    s16 r26;
    s16 r25;
    float tf0;
    float tf1_2;

    r30 = balls;
    r28 = lbl_10_bss_1230;
    for (r26 = 0; r26 < 8; r26++, r30++, r28++) {
        if (r30->status == 2) {
            vec_set_len(-1.0f, &r30->vel, &sp14[r26]);
        }
    }
    r30 = balls;
    r28 = lbl_10_bss_1230;
    for (r26 = 0; r26 < 8; r26++, r30++, r28++) {
        r28->unk21C = 0.0f;
        r29 = balls;
        r27 = lbl_10_bss_1230;
        for (r25 = 0; r25 < 8; r25++, r29++, r27++) {
            if ((r29->status == 2) && (r26 != r25)) {
                tf0 = mathutil_vec_dot_prod(&sp14[r25], &r27->unk220[r26]);
                if (!(tf0 > lbl_1_data_65780)) {
                    PSVECScale(&sp14[r25], &sp8, -tf0);
                    tf1_2 = mathutil_vec_len(&sp8);
                    if (!(tf1_2 > r29->currRadius)) {
                        tf1_2 = 1.0 - (tf1_2 / r29->currRadius);
                        tf1_2 *= (float) (1.0 - (tf0 / lbl_1_data_6577C));
                        if (tf1_2 > r28->unk21C) {
                            r28->unk21C = tf1_2;
                        }
                    }
                }
            }
        }
        r28->unk21C *= (lbl_1_data_65780 * (1.0 - r28->unk218));
    }
}

void fn_10_74A4(struct PhysicsBall *arg0, struct StageDef *arg1)
{
    struct ItemGroup *r31;
    struct StageCollHeader *r30;
    s32 r29;
    s16 *tr3;
    struct StageColiTri *tri;
    float tf31;
    struct {
        Vec pos;
        float radius;
    } sp8;

    sp8.pos = arg0->pos;
    sp8.radius = arg0->radius;
    r31 = itemgroups;
    r30 = arg1->collHeaders;
    r29 = 0;
    for (; r29 < (s32) arg1->itemGroupCount; r29++, r31++, r30++) {
        if (g_is_ball_in_ig_coli_range(r31, r30, r29, &sp8.pos) != 0) {
            if (r29 != arg0->itemGroupId) {
                tf_physball_to_itemgroup_space(arg0, r29);
            }
            tr3 = tri_coli_grid_lookup(r30, arg0->prevPos.x, arg0->prevPos.z);
            if (tr3 != NULL) {
                s16 *r26 = tr3;

                for (; *r26 >= 0; r26++) {
                    tri = &r30->triangles[*r26];
                    collide_ball_with_tri_face(arg0, tri);
                }
            }
            {
                struct StageColiCylinder *cyl = r30->coliCylinders;
                s32 r25 = r30->coliCylinderCount;

                for (; r25 > 0; r25--, cyl++) {
                    collide_ball_with_cylinder(arg0, cyl);
                }
            }
        }
    }
    tf31 = arg0->restitution;
    arg0->restitution = 0.0f;
    r31 = itemgroups;
    r30 = arg1->collHeaders;
    r29 = 0;
    for (; r29 < (s32) arg1->itemGroupCount; r29++, r31++, r30++) {
        if (r29 != arg0->itemGroupId) {
            tf_physball_to_itemgroup_space(arg0, r29);
        }
        tr3 = tri_coli_grid_lookup(r30, arg0->prevPos.x, arg0->prevPos.z);
        if (tr3 != NULL) {
            s16 *r26 = tr3;

            for (; *r26 >= 0; r26++) {
                tri = &r30->triangles[*r26];
                    collide_ball_with_tri_edges(arg0, tri);
            }
        }
    }
    r31 = itemgroups;
    r30 = arg1->collHeaders;
    r29 = 0;
    for (; r29 < (s32) arg1->itemGroupCount; r29++, r31++, r30++) {
        if (r29 != arg0->itemGroupId) {
            tf_physball_to_itemgroup_space(arg0, r29);
        }
        tr3 = tri_coli_grid_lookup(r30, arg0->prevPos.x, arg0->prevPos.z);
        if (tr3 != NULL) {
            s16 *r26 = tr3;

            for (; *r26 >= 0; r26++) {
                tri = &r30->triangles[*r26];
                    collide_ball_with_tri_vertices(arg0, tri);
            }
        }
    }
    arg0->restitution = tf31;
    if ((s32) arg0->itemGroupId != 0) {
        tf_physball_to_itemgroup_space(arg0, 0);
    }
}

void fn_10_772C(struct Ball *ball)
{
    struct Effect sp44;
    Vec sp38;
    Vec sp2C;
    float f31;
    s32 r30;
    struct RacePlayer *tr30;

    tr30 = &lbl_10_bss_1230[ball->playerId];
    if (((ball->flags & 0x1ULL) != 0) && ((ball->flags & 0x80000000000ULL) == 0)) {
        f31 = mathutil_vec_len(&ball->vel) / 0.9259259259259259;
        if (f31 > 1.0) {
            f31 = 1.0f;
        }
        f31 = math_sqrt(f31);
        f31 *= 3.0;
        if (!(f31 <= 1.8)) {
            switch ((s32) modeCtrl.unk30) {
            case 1:
                break;
            case 2:
                if ((u32) (g_font_char_var_3 & 1) == (u32) ball->playerId) {
                    break;
                }
                return;
            case 3:
                if ((s32) modeCtrl.splitscreenMode == ball->playerId) {
                    if (((u32) (g_font_char_var_3 & 3) == (u32) ball->playerId) || ((u32) (g_font_char_var_3 & 3) == 3)) {
                        break;
                    }
                    return;
                }
            case 4:
                if ((u32) (g_font_char_var_3 & 3) != (u32) ball->playerId) {
                    return;
                }
                break;
            }
                sp2C.x = -ball->unk11C.x;
                sp2C.y = -ball->unk11C.y;
                sp2C.z = -ball->unk11C.z;
                memset(&sp44, 0, 0xB0);
                sp44.type = 7;
                sp44.playerId = ball->playerId;
                if ((s32) (tr30->unk2BC & 1) != 0) {
                    *(Vec *) &sp44.unk18 = (Vec){0.5f, 0.7f, 1.0f};
                }
                PSVECScale(&ball->unk11C, &sp44.pos, ball->currRadius);
                PSVECAdd(&sp44.pos, &ball->pos, &sp44.pos);
                PSVECScale(&ball->vel, &sp38, 0.85f);
                for (r30 = f31; r30 > 0; r30--) {
                    sp44.vel = (Vec){(float) rand() / 32767.0f, (float) rand() / 32767.0f, (float) rand() / 32767.0f};
                    PSVECScale(&sp44.vel, &sp44.vel, 1.5f);
                    PSVECSubtract(&sp44.vel, &(Vec){0.75f, 0.75f, 0.75f}, &sp44.vel);
                    PSVECAdd(&sp44.vel, &sp2C, &sp44.vel);
                    PSVECScale(&sp44.vel, &sp44.vel, 0.1 * (f31 * ((float) rand() / 32767.0f)));
                    PSVECAdd(&sp44.vel, &sp38, &sp44.vel);
                    fn_10_DDA8(&sp44, NULL);
                }
        }
    }
}

void fn_10_7B68(struct Ball *ball)
{
    Vec sp14;
    struct RacePlayer *tr30;

    tr30 = &lbl_10_bss_1230[ball->playerId];
    sp14 = (Vec){ball->vel.x, 0.0f, ball->vel.z};
    if (mathutil_vec_sq_len(&sp14) < 0.00002143347050754458) {
        mtxa_from_quat(&ball->ape->rot);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp14);
    }
    tr30->unk35E = (math_atan2(sp14.x, sp14.z) + 0x8000);
}

