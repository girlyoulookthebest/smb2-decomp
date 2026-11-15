#include "boat.h"
#include "item.h"
#include "input.h"
#include "macros.h"

#define PL lbl_14_bss_29A8
#define BALL_IS_CPU(ball) ((ball)->flags & 0x80000000000LL)
#define PROJ(path, pos, t) (((float (*)(void *, Vec *, float)) lbl_14_bss_2A78)((path), (pos), (t)))
#define PROJ_INIT(path, pos, t) (((float (*)(void *, Vec *, float)) lbl_14_bss_2A7C)((path), (pos), (t)))

struct BoatInputRec
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u16 unk2;
    /*0x04*/ Vec unk4;
};  /* size = 0x10 */

extern float lbl_1_data_65790;
extern float lbl_1_data_65794;
extern float lbl_1_data_65798;
extern u8 lbl_1_data_6579C;
extern s16 lbl_1_data_6579E;
extern float lbl_1_data_657A0;
extern float lbl_1_data_657A4;
extern float lbl_1_data_657A8;
extern s16 lbl_1_data_657AC;
extern float lbl_1_data_657D0;
extern float lbl_1_data_657D4;
extern float lbl_1_data_657D8;
extern float lbl_1_data_657DC;
extern float lbl_1_data_657E0;
extern float lbl_1_data_657EC;
extern float lbl_1_data_657F0;
extern float lbl_1_data_657F4;
extern float lbl_1_data_657FC;
extern float lbl_1_data_65800;
extern u8 lbl_1_data_65804;
extern float lbl_1_data_65808;
extern float lbl_1_data_6580C;
extern float lbl_1_data_65810;
extern u8 lbl_1_bss_D4330;
extern u32 lbl_1_bss_D4334[];
extern float lbl_1_bss_D43B0;
extern struct BoatCpuPoint lbl_1_bss_D43B4[];
extern u8 lbl_14_data_1B8[];
#ifdef __MWERKS__
extern void qsort(void *base, u32 n, u32 size, s32 (*cmp)(void *, void *));
#endif
extern void g_math_unk19(Vec *pos, float *out, float tanHalfFov, float aspect);
extern float (*fn_1_1135A4(struct Ape *ape))[4];
extern void g_SoftStreamStart_with_some_defaults_2(s32);
extern void g_maybe_smth_with_music(s32, s32);
extern void call_SoundReqID_arg_0(s32);
extern void call_SoundReqID_arg_1(s32);
extern void destroy_sprite_with_unique_id(s32);

u32 lbl_14_data_818[0x5C] = {
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

void (*lbl_14_data_988[0xF])(struct Ball *) = {
    (void (*)(struct Ball *)) fn_14_A6CC,
    (void (*)(struct Ball *)) fn_14_B37C,
    (void (*)(struct Ball *)) fn_14_B3B8,
    (void (*)(struct Ball *)) fn_14_B420,
    (void (*)(struct Ball *)) fn_14_B5F8,
    (void (*)(struct Ball *)) fn_14_B66C,
    (void (*)(struct Ball *)) fn_14_BA58,
    (void (*)(struct Ball *)) fn_14_BC1C,
    (void (*)(struct Ball *)) fn_14_BE04,
    (void (*)(struct Ball *)) fn_14_BEF8,
    (void (*)(struct Ball *)) fn_14_C01C,
    (void (*)(struct Ball *)) fn_14_C200,
    (void (*)(struct Ball *)) fn_14_C23C,
    (void (*)(struct Ball *)) fn_14_C2D0,
    (void (*)(struct Ball *)) fn_14_C370,
};

char *lbl_14_data_ABC[0x14] = {
    "SE_SCREW_L",
    "SE_SCREW_R",
    "READY",
    "GOAL",
    "ADD_LAP",
    "RETIRE",
    "REVERSE",
    "ITEM_USE_REQ",
    "ITEM_ROTATE_REQ",
    "OBST_ARRESTED",
    "WAVE_EXIST",
    "WHALE_SPLASH",
    "ITEM_GET",
    "GOAL_VIEW",
    "PDL_OFF",
    "CHECKPOINT",
    "DIVE_NOW",
    "FORCE_FIRE",
    "TIMING_BAR_L_SHINE",
    "TIMING_BAR_R_SHINE",
};

struct BoatPlayer *lbl_14_bss_29A8;
Vec lbl_14_bss_29AC;
Vec lbl_14_bss_29B8;
struct BoatPlayer *lbl_14_bss_29C4[8];
struct BoatPlayer *lbl_14_bss_29E4[9];

void fn_14_66DC(struct Ball *ball)
{
    struct RaycastHit hit;
    s16 i;
    s32 ret;

    lbl_14_bss_30[6] = OSGetTick();
    if (ball->unk155 >= 0 && ball->unk155 < 0xF) {
        ball->unk154 = ball->unk155;
        ball->unk155 = -1;
    }
    PL = &lbl_14_bss_208[ball->playerId];
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20LL;
    PL->unkC = PL->unk8;
    PL->unk8 &= ~0x10200;
    PL->unk2E0 = PL->unk2DC;
    PL->unk2F0 = PL->unk2E4;
    PL->unk300 = PL->unk2FC;
    PL->unk318 = PL->unk314;
    PL->unk328 = PL->unk31C;
    PL->unk15 = PL->unk14;
    PL->unk1A = PL->unk19;
    PL->unk17 = PL->unk16;
    PL->unk1C = PL->unk1B;
    PL->unk34 = PL->unk30;
    if (ball->status == 2 && !BALL_IS_CPU(ball)) {
        struct BoatInputRec *rec = (struct BoatInputRec *) lbl_14_bss_18 + lbl_14_bss_60;

        if (lbl_14_bss_8 & 2) {
            if (lbl_14_bss_8 & 4) {
                PL->unk14 = rec->unk0;
                PL->unk19 = rec->unk1;
                PL->unk2D8 = rec->unk2;
            } else {
                PL->unk14 = rec->unk0 = pad_status_groups[PL->unk1].unk0[0].triggerLeft;
                PL->unk19 = rec->unk1 = pad_status_groups[PL->unk1].unk0[0].triggerRight;
                PL->unk2D8 = rec->unk2 = pad_status_groups[PL->unk1].unk0[2].button;
            }
            if (lbl_14_bss_60 <= lbl_80198B38 * 0xB4 - 2) {
                lbl_14_bss_60++;
            }
        } else {
            PL->unk14 = pad_status_groups[PL->unk1].unk0[0].triggerLeft;
            PL->unk19 = pad_status_groups[PL->unk1].unk0[0].triggerRight;
            PL->unk2D8 = pad_status_groups[PL->unk1].unk0[2].button;
        }
    }
    fn_14_D81C(&PL->unk16, PL->unk14, lbl_1_data_65804);
    fn_14_D81C(&PL->unk1B, PL->unk19, lbl_1_data_65804);
    for (i = 1; i < 4; i++) {
        PL->unk1E[i] = PL->unk1E[i - 1];
        PL->unk26[i] = PL->unk26[i - 1];
    }
    PL->unk1E[0] = PL->unk16 - PL->unk17;
    PL->unk26[0] = PL->unk1B - PL->unk1C;
    PL->unk30 = ((float) PL->unk14 - (float) PL->unk19) / 140.0;
    if (PL->unk38 > 0) {
        PL->unk38 -= lbl_1_data_6579C;
        if (PL->unk38 < PL->unk14) {
            PL->unk38 = PL->unk14;
        }
    }
    if (PL->unk3A > 0) {
        PL->unk3A -= lbl_1_data_6579C;
        if (PL->unk3A < PL->unk19) {
            PL->unk3A = PL->unk19;
        }
    }
    if (PL->unk1E[0] > 0 && PL->unk26[0] <= 0) {
        if (PL->unk38 <= lbl_1_data_6579E) {
            PL->unk38 += PL->unk1E[0];
        }
        PL->unk3A = 0;
    } else if (PL->unk26[0] > 0 && PL->unk1E[0] <= 0) {
        if (PL->unk3A <= lbl_1_data_6579E) {
            PL->unk3A += PL->unk26[0];
        }
        PL->unk38 = 0;
    }
    if (PL->unk14 == 0) {
        if (PL->unk18 < 0xFF) {
            PL->unk18++;
        }
        if (PL->unk18 >= 3) {
            PL->unk38 = 0;
        }
    } else {
        PL->unk18 = 0;
    }
    if (PL->unk19 == 0) {
        if (PL->unk1D < 0xFF) {
            PL->unk1D++;
        }
        if (PL->unk1D >= 3) {
            PL->unk3A = 0;
        }
    } else {
        PL->unk1D = 0;
    }
    PL->unk4A8 = PL->unk4A4;
    PL->unk4B4 = PL->unk4B0;
    PL->unk30C = PL->unk308;
    lbl_14_bss_30[7] = OSGetTick();
    ret = fn_14_13CA8(&ball->pos, &hit, 0);
    lbl_1_bss_D4334[7] += BOAT_TICKS(lbl_14_bss_30[7]);
    if (ret != 0) {
        PL->unk8 |= 0x400;
        PL->unk478 = hit.pos.y;
        PL->unk47C = hit.normal;
        PL->unk308 = PL->unk3AC + (PL->unk478 - ball->pos.y);
        if (PL->unk4A4 & 8) {
            if (PL->unk4BC[3] < 0.5 * lbl_80198B34) {
                PL->unk308 -= (float) PL->unk4BC[3] / (0.5 * lbl_80198B34);
            } else {
                PL->unk308 -= 1.0;
            }
        }
        if (PL->unk308 >= 0.0) {
            if (PL->unk488 > lbl_80198B38 / 2) {
                fn_14_12B24(0x260, &ball->pos);
                PL->unk8 |= 0x10000;
            }
            PL->unk488 = 0;
        } else if (PL->unk488 < 0xFFFF) {
            PL->unk488++;
        }
    } else {
        PL->unk8 &= ~0x400;
        PL->unk308 = -1.0f;
    }
    if (PL->unk3EC != 0) {
        PL->unk3EC--;
    }
    if (PL->unk8 & 0x40) {
        if (PL->unk3F0 != 0) {
            PL->unk3F0--;
        }
    } else {
        PL->unk3F0 = lbl_1_data_65800 * lbl_80198B34;
    }
    if (PL->unk3F2 != 0) {
        PL->unk3F2--;
    }
    if (PL->unk474 != 0) {
        PL->unk474--;
    }
    if (PL->unk476 != 0) {
        PL->unk476--;
    }
    for (i = 0; i < 5; i++) {
        if (PL->unk4BC[i] != 0) {
            PL->unk4BC[i]--;
        }
        if (PL->unk4D0[i] != 0) {
            PL->unk4D0[i]--;
        }
    }
    if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x2000)) {
        PL->unk14 = pad_status_groups[0].unk0[0].triggerLeft;
        PL->unk19 = pad_status_groups[0].unk0[0].triggerRight;
        PL->unk2D8 = pad_status_groups[0].unk0[2].button;
    }
    lbl_14_data_988[ball->unk154](ball);
    if (ball->status == 2) {
        fn_14_8E70(ball, PL);
        fn_14_108A4(ball, PL);
        if (!BALL_IS_CPU(ball) && (lbl_14_bss_8 & 2)) {
            struct BoatInputRec *rec = (struct BoatInputRec *) lbl_14_bss_18 + (lbl_14_bss_60 - 1);

            if (lbl_14_bss_8 & 4) {
                if (rec->unk4.x != ball->pos.x || rec->unk4.y != ball->pos.y || rec->unk4.z != ball->pos.z) {
                    g_some_other_flags |= 2;
                    printf("position error(%d)\n", lbl_14_bss_60 - 1);
                    printf("  rec (%f, %f, %f)\n", rec->unk4.x, rec->unk4.y, rec->unk4.z);
                    printf("  now (%f, %f, %f)\n", ball->pos.x, ball->pos.y, ball->pos.z);
                }
            } else {
                rec->unk4 = ball->pos;
            }
        }
    }
    lbl_1_bss_D4334[6] += BOAT_TICKS(lbl_14_bss_30[6]);
}

void fn_14_789C(struct Ball *ball, s32 state)
{
    ball->unk155 = (s8) state;
}

void fn_14_78A8(struct Ball *ball, s32 state)
{
    ball->unk154 = (s8) state;
    lbl_14_data_988[ball->unk154](ball);
}

void fn_14_78F0(void)
{
    lbl_14_bss_30[8] = OSGetTick();
    lbl_14_bss_30[9] = OSGetTick();
    fn_14_21A04();
    lbl_1_bss_D4334[9] += BOAT_TICKS(lbl_14_bss_30[9]);
    fn_14_A0E8();
    fn_14_A390();
    lbl_14_bss_30[10] = OSGetTick();
    fn_14_2363C();
    lbl_1_bss_D4334[10] += BOAT_TICKS(lbl_14_bss_30[10]);
    lbl_14_bss_30[11] = OSGetTick();
    if (lbl_14_bss_20.unk0 == 2) {
        fn_14_82BC();
    } else {
        fn_14_7B50();
    }
    lbl_1_bss_D4334[11] += BOAT_TICKS(lbl_14_bss_30[11]);
    fn_14_892C();
    fn_14_8D1C();
    lbl_1_bss_D4334[8] += BOAT_TICKS(lbl_14_bss_30[8]);
    if ((dip_switches & 1) && (lbl_14_bss_20.unk2 & 1) && (lbl_1_bss_D4328 & 8) && (g_font_char_var_3 % 10) == 0) {
        struct Ball *ball = balls;

        printf("%f\t%f\t%f\n", ball->pos.x, ball->pos.y, ball->pos.z);
    }
}

void fn_14_7B50(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    struct BoatLap *lap;
    float frac;
    s16 i;

    i = 0;
    ball = balls;
    pl = lbl_14_bss_208;
    for (; i < lbl_14_bss_24; i++, ball++, pl++) {
        pl->unk2DC = PROJ(lbl_14_bss_70, &ball->pos, pl->unk2DC);
        pl->unk338 = PROJ(lbl_14_bss_74, &ball->pos, pl->unk338);
        pl->unk33C = PROJ(lbl_14_bss_78, &ball->pos, pl->unk33C);
        if (BALL_IS_CPU(ball)) {
            pl->unk314 = PROJ(pl->unk310, &ball->pos, pl->unk314);
        }
        fn_14_11524(lbl_14_bss_70, pl->unk2DC, &pl->unk2E4);
        fn_14_11524(lbl_14_bss_74, pl->unk338, &pl->unk340);
        fn_14_11524(lbl_14_bss_78, pl->unk33C, &pl->unk34C);
        if (BALL_IS_CPU(ball)) {
            fn_14_11524(pl->unk310, pl->unk314, &pl->unk31C);
        }
        pl->unk8 &= ~0x8050;
        if (pl->unk8 & 0x2C) {
            continue;
        }
        if (pl->unk2E0 > 95.0 && pl->unk2DC < 5.0) {
            pl->unk2FC += (100.0 - pl->unk2E0) + pl->unk2DC;
        } else if (pl->unk2E0 < 5.0 && pl->unk2DC > 95.0) {
            pl->unk2FC -= pl->unk2E0 + (100.0 - pl->unk2DC);
            pl->unk8 |= 0x40;
        } else {
            pl->unk2FC += pl->unk2DC - pl->unk2E0;
            if (pl->unk2DC < pl->unk2E0 || ((pl->unkC & 0x40) && __fabs(pl->unk2DC - pl->unk2E0) < 0.0001)) {
                pl->unk8 |= 0x40;
            }
        }
        fn_14_8768(ball, pl);
        if (((dip_switches & 1) || (dip_switches & 0x20000000)) && pl->unk14 == 0x8C && pl->unk19 == 0x8C && (pl->unk2D8 & 0x400)) {
            pl->unk2FC = 100.1f;
            pl->unk300 = 99.9f;
        }
        lap = &pl->unk50[pl->unk7 - 1];
        if (pl->unk2FC >= 100.0) {
            pl->unk2FC -= 100.0;
            frac = (100.0 - pl->unk300) / (pl->unk2FC + (100.0 - pl->unk300));
            lap->unk4 += frac;
            pl->unk2FC = pl->unk2DC;
            fn_14_123D0(lap->unk0, lap->unk4, &lap->unk8);
            fn_14_8848(lap, &pl->unk2B4);
            fn_14_1276C(&lap->unk8, &pl->unk2A8.unk8, &pl->unk2A8.unk8);
            pl->unk2A8.unk0 = lbl_80198B38 * (pl->unk2A8.unk8.min * 60 - -pl->unk2A8.unk8.sec);
            pl->unk2A8.unk4 = 0.01 * pl->unk2A8.unk8.cs;
            if (pl->unk7 == lbl_14_bss_AC->unk1) {
                pl->unk8 |= 8;
            } else {
                pl->unk8 |= 0x10;
                pl->unk48A = 0;
                lap = &pl->unk50[pl->unk7];
                lap->unk4 = 1.0 - frac;
                fn_14_123D0(lap->unk0, lap->unk4, &lap->unk8);
                pl->unk7++;
            }
        } else if (pl->unk2FC <= -100.0) {
            pl->unk2FC += 100.0;
            frac = (100.0 + pl->unk300) / (-pl->unk2FC + (100.0 + pl->unk300));
            lap->unk4 += frac;
            pl->unk2FC = 100.0 - pl->unk2DC;
            fn_14_123D0(lap->unk0, lap->unk4, &lap->unk8);
            if (fn_14_8848(lap, &pl->unk2B4) != 0) {
                pl->unk2B4.unkB |= 2;
            }
            fn_14_1276C(&lap->unk8, &pl->unk2A8.unk8, &pl->unk2A8.unk8);
            pl->unk2A8.unk0 = lbl_80198B38 * (pl->unk2A8.unk8.min * 60 - -pl->unk2A8.unk8.sec);
            pl->unk2A8.unk4 = 0.01 * pl->unk2A8.unk8.cs;
            lap->unkB |= 2;
            pl->unk2A8.unkB |= 2;
            if (pl->unk7 == lbl_14_bss_AC->unk1) {
                pl->unk8 |= 8;
            } else {
                pl->unk8 |= 0x10;
                pl->unk48A = 0;
                lap = &pl->unk50[pl->unk7];
                lap->unk4 = 1.0 - frac;
                fn_14_123D0(lap->unk0, lap->unk4, &lap->unk8);
                pl->unk7++;
            }
        } else {
            lap->unk0++;
            fn_14_123D0(lap->unk0, lap->unk4, &lap->unk8);
        }
        if (pl->unk8 & 8) {
            pl->unk304 = lbl_14_bss_AC->unk1 * lbl_14_bss_2AE8->unk8;
        } else {
            pl->unk304 = 0.01 * (lbl_14_bss_2AE8->unk8 * (pl->unk2FC + (float) ((pl->unk7 - 1) * 100)));
        }
    }
}

void fn_14_82BC(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    struct BoatLap *lap;
    float frac;
    s16 i;

    i = 0;
    ball = balls;
    pl = lbl_14_bss_208;
    for (; i < lbl_14_bss_24; i++, ball++, pl++) {
        pl->unk2DC = PROJ(lbl_14_bss_70, &ball->pos, pl->unk2DC);
        pl->unk338 = PROJ(lbl_14_bss_74, &ball->pos, pl->unk338);
        pl->unk33C = PROJ(lbl_14_bss_78, &ball->pos, pl->unk33C);
        if (BALL_IS_CPU(ball)) {
            pl->unk314 = PROJ(pl->unk310, &ball->pos, pl->unk314);
        }
        if (pl->unk8 & 8) {
            pl->unk2DC = 99.99f;
        }
        fn_14_11524(lbl_14_bss_70, pl->unk2DC, &pl->unk2E4);
        fn_14_11524(lbl_14_bss_74, pl->unk338, &pl->unk340);
        fn_14_11524(lbl_14_bss_78, pl->unk33C, &pl->unk34C);
        if (BALL_IS_CPU(ball)) {
            fn_14_11524(pl->unk310, pl->unk314, &pl->unk31C);
        }
        pl->unk8 &= ~0x8000;
        if (pl->unk8 & 0x2C) {
            continue;
        }
        if (pl->unk2E0 > 95.0 && pl->unk2DC < 5.0) {
            pl->unk2FC += (100.0 - pl->unk2E0) + pl->unk2DC;
        } else if (pl->unk2E0 < 5.0 && pl->unk2DC > 95.0) {
            pl->unk2FC -= pl->unk2E0 + (100.0 - pl->unk2DC);
        } else {
            pl->unk2FC += pl->unk2DC - pl->unk2E0;
        }
        fn_14_8768(ball, pl);
        if (((dip_switches & 1) || (dip_switches & 0x20000000)) && pl->unk14 == 0x8C && pl->unk19 == 0x8C && (pl->unk2D8 & 0x400)) {
            pl->unk2FC = 100.1f;
            pl->unk300 = 99.9f;
        }
        lap = &pl->unk50[pl->unk7 - 1];
        if (pl->unk2FC >= 100.0) {
            pl->unk2FC -= 100.0;
            frac = (100.0 - pl->unk300) / (pl->unk2FC + (100.0 - pl->unk300));
            lap->unk4 += frac;
            pl->unk2FC = pl->unk2DC;
            fn_14_123D0(lap->unk0, lap->unk4, &lap->unk8);
            fn_14_1276C(&lap->unk8, &pl->unk2A8.unk8, &pl->unk2A8.unk8);
            pl->unk2A8.unk0 = lbl_80198B38 * (pl->unk2A8.unk8.min * 60 - -pl->unk2A8.unk8.sec);
            pl->unk2A8.unk4 = 0.01 * pl->unk2A8.unk8.cs;
            pl->unk8 |= 8;
            pl->unk2DC = 99.99f;
            fn_14_11524(lbl_14_bss_70, pl->unk2DC, &pl->unk2E4);
        } else {
            lap->unk0++;
            fn_14_123D0(lap->unk0, lap->unk4, &lap->unk8);
        }
        if (!(pl->unk8 & 8)) {
            fn_14_123D0(lap->unk0, lap->unk4, &lap->unk8);
        }
        pl->unk304 = 0.01 * (pl->unk2FC * lbl_14_bss_2AE8->unk8);
    }
}

void fn_14_8768(struct Ball *ball, struct BoatPlayer *pl)
{
    float *p;
    s16 i;

    i = 0;
    p = (float *) lbl_14_bss_2AE8->unk14;
    for (; *p > 0.0; i++, p++) {
        if (!(pl->unk48A & (u16) (1 << i)) && ((pl->unk2FC > 0.0 && pl->unk2DC >= *p && pl->unk2E0 < *p) || (pl->unk2FC < 0.0 && pl->unk2DC <= *p && pl->unk2E0 > *p))) {
            pl->unk8 |= 0x8000;
        }
    }
}

s32 fn_14_8848(struct BoatLap *lap, struct BoatLap *best)
{
    if ((dip_switches & 1) && lap->unk8.min == 0 && lap->unk8.sec == 0 && lap->unk8.cs == 0) {
        return 0;
    }
    if (PL->unk7 == 1) {
        *best = *lap;
        return 1;
    }
    if (lap->unk8.cs + (lap->unk8.sec + lap->unk8.min * 60) * 100 < best->unk8.cs + (best->unk8.sec + best->unk8.min * 60) * 100) {
        *best = *lap;
        return 1;
    }
    return 0;
}

void fn_14_892C(void)
{
    struct BoatPlayer **sorted;
    s16 i;

    sorted = lbl_14_bss_2988;
    for (i = 0; i < 8; i++) {
        sorted[i] = &lbl_14_bss_208[i];
    }
    qsort(sorted, lbl_14_bss_24, sizeof(*sorted), fn_14_8AA0);
    sorted[0]->unk2 = 1;
    for (i = 1; i < lbl_14_bss_24; i++) {
        if (sorted[i]->unk8 & 8) {
            struct BoatLap *a = &sorted[i - 1]->unk2A8;
            struct BoatLap *b = &sorted[i]->unk2A8;

            if (a->unk8.min == b->unk8.min && a->unk8.sec == b->unk8.sec && a->unk8.cs == b->unk8.cs) {
                sorted[i]->unk2 = sorted[i - 1]->unk2;
                continue;
            }
        }
        sorted[i]->unk2 = i + 1;
    }
}

s32 fn_14_8AA0(void *arg0, void *arg1)
{
    struct BoatPlayer *a;
    struct BoatPlayer *b;
    struct Ball *ballA;
    struct Ball *ballB;

    a = *(struct BoatPlayer **) arg0;
    b = *(struct BoatPlayer **) arg1;
    ballA = &balls[a->unk0];
    ballB = &balls[b->unk0];
    if (ballA->status != 2 && ballB->status != 2) {
        return a->unk0 > b->unk0 ? 1 : -1;
    }
    if (ballA->status != 2 && ballB->status == 2) {
        return 1;
    }
    if (!(a->unk8 & 8) && (b->unk8 & 8)) {
        return 1;
    }
    if ((a->unk8 & 8) && !(b->unk8 & 8)) {
        return -1;
    }
    if ((a->unk8 & 8) && (b->unk8 & 8)) {
        if (a->unk2A8.unk8.min == b->unk2A8.unk8.min && a->unk2A8.unk8.sec == b->unk2A8.unk8.sec && a->unk2A8.unk8.cs == b->unk2A8.unk8.cs) {
            return a->unk0 > b->unk0 ? 1 : -1;
        }
        return (float) a->unk2A8.unk0 + a->unk2A8.unk4 > (float) b->unk2A8.unk0 + b->unk2A8.unk4 ? 1 : -1;
    }
    if (100.0 * a->unk7 + a->unk2FC < 100.0 * b->unk7 + b->unk2FC) {
        return 1;
    }
    return -1;
}

void fn_14_8D1C(void)
{
#ifdef __MWERKS__
    float out[2];
#else
    /* g_math_unk19 transforms into a whole Vec (mtxa_tf_point) before
     * projecting: on GC the third float lands in the next stack slot, on PC
     * it clobbered the caller's saved registers */
    float out[3];
#endif
    struct Ball *ball;
    struct BoatPlayer *pl;
    struct Camera *camera;
    s16 i;
    s8 status;

    ball = balls;
    pl = lbl_14_bss_208;
    camera = cameras;
    i = 0;
    for (; i < 5; i++, ball++, pl++, camera++) {
        status = g_poolInfo.cameraPool.statusList[i];
        if (status != 0 && status != 4 && !(camera->flags & 0x40) && ball->status == 2) {
            mtxa_from_mtx(camera->unk14C);
            g_math_unk19(&ball->pos, out, camera->sub28.view.tanHalfFov, g_camera_aspect_ratio);
            pl->unk48C = 640.0 * (camera->sub28.viewportX + 0.5 * camera->sub28.viewportW * (1.0 + out[0]));
            pl->unk490 = 480.0 * (camera->sub28.viewportY + 0.5 * camera->sub28.viewportH * (1.0 - out[1]));
        }
    }
}

void fn_14_8E70(struct Ball *ball, struct BoatPlayer *pl)
{
    s16 i;

    if (pl->unk8 & 0x100) {
        u32 first;

        pl->unk8 &= ~0x100;
        first = pl->unk498[0];
        for (i = 1; i < 3; i++) {
            pl->unk498[i - 1] = pl->unk498[i];
            pl->unk498[i] = 0;
        }
        for (i = 0; i < 3; i++) {
            if (pl->unk498[i] == 0) {
                pl->unk498[i] = first;
                break;
            }
        }
    }
    if (pl->unk8 & 0x80) {
        pl->unk8 &= ~0x80;
        pl->unk4AC |= pl->unk498[0];
        for (i = 1; i < 3; i++) {
            pl->unk498[i - 1] = pl->unk498[i];
            pl->unk498[i] = 0;
        }
        if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x8000)) {
            pl->unk498[2] = 1 << (fn_14_12904() % 4);
        }
    }
    for (i = 0; i < 5; i++) {
        fn_14_9064(ball, pl, i);
        fn_14_9A74(ball, pl, i);
    }
}

void fn_14_9064(struct Ball *ball, struct BoatPlayer *pl, u16 item)
{
    struct Effect fx;
    struct Item it;
    struct BoatItemInfo info;
    Vec back;
    u32 bit;
    s16 tr0;
    float tf1;

    bit = 1 << item;
    if (pl->unk4AC & bit) {
        pl->unk4AC &= ~bit;
        switch (item) {
        case 3:
            pl->unk4A4 |= bit;
            if (pl->unk4BC[3] == 0) {
                memset(&fx, 0, sizeof(fx));
                fx.type = 1;
                fx.playerId = ball->playerId;
                fn_14_186F4(&fx, 0);
                memset(&fx, 0, sizeof(fx));
                fx.type = 4;
                fx.playerId = ball->playerId;
                fx.model = lbl_14_bss_B4[0x68 / 4];
                fx.pos = (Vec){0.0f, 0.0f, 0.0f};
                fx.unk8C = (Vec){0.0f, 1.0f, 0.0f};
                fn_14_186F4(&fx, 0);
                fx.model = lbl_14_bss_B4[0x64 / 4];
                fx.pos = (Vec){-0.35f, -0.3f, 0.5f};
                fx.unk8C = (Vec){-0.5f, 0.0f, 0.0f};
                fn_14_186F4(&fx, 0);
                fx.pos = (Vec){0.35f, -0.3f, 0.5f};
                fx.unk8C = (Vec){0.5f, 0.0f, 0.0f};
                fn_14_186F4(&fx, 0);
                fx.type = 5;
                fx.model = lbl_14_bss_B4[0x6C / 4];
                fx.pos = (Vec){-0.35f, -0.3f, 0.5f};
                fx.unk8C = (Vec){-0.5f, 0.0f, 0.0f};
                fn_14_186F4(&fx, 0);
                fx.pos = (Vec){0.35f, -0.3f, 0.5f};
                fx.unk8C = (Vec){0.5f, 0.0f, 0.0f};
                fn_14_186F4(&fx, 0);
            }
            if (pl->unk4BC[3] <= 0.5 * lbl_80198B34 - 2.0) {
                call_SoundReqID_arg_0(0x275);
                if (BALL_IS_CPU(ball)) {
                    call_SoundReqID_arg_1(ball->playerId + 0x287);
                    pl->unk8 |= 1;
                    call_SoundReqID_arg_1(ball->playerId + 0x28B);
                    pl->unk8 |= 2;
                }
            }
            pl->unk4BC[3] = (float) pl->unk4BC[3] + lbl_1_data_65790 * lbl_80198B34;
            break;
        case 0:
        case 1:
        case 2:
            memset(&it, 0, sizeof(it));
            memset(&info, 0, 0x18);
            it.type = item;
            it.pos = ball->pos;
            it.vel = ball->vel;
            it.vel.y = 0.0f;
            back = (Vec){0.0f, 0.0f, 1.1 * -ball->currRadius};
            mtxa_from_rotate_y(pl->unk3E2);
            mtxa_tf_vec(&back, &back);
            PSVECAdd(&it.pos, &back, &it.pos);
            vec_set_len(lbl_1_data_65798, &back, &back);
            PSVECAdd(&it.vel, &back, &it.vel);
            info.unk14 = ball->playerId;
            info.unk15 = fn_14_A588(pl);
            fn_14_15880(&it, &info);
            break;
        }
    }
    if (pl->unk4A4 & bit) {
        switch (item) {
        case 3:
            tr0 = 0.5 * lbl_80198B34;
            tf1 = 0.25 / tr0;
            if ((pl->unk8 & 0x28) && pl->unk4BC[item] > tr0) {
                pl->unk4BC[item] = tr0;
            }
            if (pl->unk4BC[item] > tr0) {
                pl->unk3B0 += tf1;
                if (pl->unk3B0 > 1.0) {
                    pl->unk3B0 = 1.0f;
                }
            } else {
                pl->unk3B0 -= tf1;
                if (pl->unk3B0 < 0.75) {
                    pl->unk3B0 = 0.75f;
                }
            }
            pl->unk3AC = 0.5 * pl->unk3B0;
            pl->unk3A8 = (1.0 - pl->unk3B0) / 0.25;
            if (pl->unk4BC[item] <= tr0 - 1) {
                if (pl->unk8 & 1) {
                    SoundOffID(ball->playerId + 0x287);
                    pl->unk8 &= ~1;
                    call_SoundReqID_arg_1(0x276);
                }
                if (pl->unk8 & 2) {
                    SoundOffID(ball->playerId + 0x28B);
                    pl->unk8 &= ~2;
                    call_SoundReqID_arg_1(0x276);
                }
            } else if (!BALL_IS_CPU(ball)) {
                if (pl->unk3C == 1) {
                    if (!(pl->unk8 & 1)) {
                        call_SoundReqID_arg_1(ball->playerId + 0x287);
                        pl->unk8 |= 1;
                    }
                    if (pl->unk4C > 40.0) {
                        if (!(pl->unk8 & 2)) {
                            call_SoundReqID_arg_1(ball->playerId + 0x28B);
                            pl->unk8 |= 2;
                        }
                    } else if (pl->unk8 & 2) {
                        SoundOffID(ball->playerId + 0x28B);
                        pl->unk8 &= ~2;
                        call_SoundReqID_arg_1(0x276);
                    }
                } else if (pl->unk3C == 2) {
                    if (!(pl->unk8 & 2)) {
                        call_SoundReqID_arg_1(ball->playerId + 0x28B);
                        pl->unk8 |= 2;
                    }
                    if (pl->unk4C > 40.0) {
                        if (!(pl->unk8 & 1)) {
                            call_SoundReqID_arg_1(ball->playerId + 0x287);
                            pl->unk8 |= 1;
                        }
                    } else if (pl->unk8 & 1) {
                        SoundOffID(ball->playerId + 0x287);
                        pl->unk8 &= ~1;
                        call_SoundReqID_arg_1(0x276);
                    }
                } else {
                    if (pl->unk8 & 1) {
                        SoundOffID(ball->playerId + 0x287);
                        pl->unk8 &= ~1;
                        call_SoundReqID_arg_1(0x276);
                    }
                    if (pl->unk8 & 2) {
                        SoundOffID(ball->playerId + 0x28B);
                        pl->unk8 &= ~2;
                        call_SoundReqID_arg_1(0x276);
                    }
                }
            }
            break;
        }
        if (pl->unk4BC[item] == 0) {
            pl->unk4A4 &= ~bit;
            switch (item) {
            case 3:
                pl->unk3B0 = 0.75f;
                pl->unk3AC = 0.5 * pl->unk3B0;
                pl->unk3A8 = 1.0f;
                if (pl->unk8 & 1) {
                    SoundOffID(ball->playerId + 0x287);
                    pl->unk8 &= ~1;
                    call_SoundReqID_arg_1(0x276);
                }
                if (pl->unk8 & 2) {
                    SoundOffID(ball->playerId + 0x28B);
                    pl->unk8 &= ~2;
                    call_SoundReqID_arg_1(0x276);
                }
                break;
            }
        }
    }
}

void fn_14_9A74(struct Ball *ball, struct BoatPlayer *pl, u16 item)
{
    struct Effect fx;
    struct BoatParticleParam param;
    u32 bit;
    u32 *timer;

    bit = 1 << item;
    timer = &pl->unk4D0[item];
    if (pl->unk4B8 & bit) {
        pl->unk4B8 &= ~bit;
        pl->unk4B0 |= bit;
        switch (item) {
        case 1:
            break;
        case 0:
            *timer = lbl_80198B38 * 2;
            pl->unk4E8 = 0x800;
            break;
        case 2:
            if (*timer == 0) {
                call_SoundReqID_arg_0(ball->playerId + 0x283);
            }
            *timer = lbl_80198B38 * 5;
            pl->unk4EC = 0;
            ball->vel.x = 0.0f;
            ball->vel.z = 0.0f;
            break;
        }
    }
    if (pl->unk4B0 & bit) {
        switch (item) {
        case 1:
            break;
        case 0:
            pl->unk4E4 = 0.0012f;
            pl->unk3E0 += pl->unk4E8;
            pl->unk3E4 += pl->unk4E8;
            if (pl->unk4E8 > 0) {
                pl->unk4E8 -= 2048.0 / (2.0 * lbl_80198B34);
                if (pl->unk4E8 < 0) {
                    pl->unk4E8 = 0;
                }
            }
            break;
        case 2:
            if ((pl->unk8 & 0x400) && *timer > 0.5 * lbl_80198B34 && (pl->unk308 > -lbl_1_data_657D0 || ball->vel.y < -0.04f)) {
                pl->unk8 |= 0x800;
            } else {
                pl->unk8 &= ~0x800;
            }
            if (pl->unk4EC < 0x20000) {
                pl->unk3E2 += 0x300;
                pl->unk4EC += 0x300;
                if (pl->unk4EC >= 0x20000) {
                    *timer = 0;
                }
            }
            if ((g_font_char_var_3 & 3) == ball->playerId) {
                memset(&fx, 0, sizeof(fx));
                fx.type = 2;
                fx.pos.x = (0.2f + ball->pos.x) - 0.4f * RAND_FLOAT();
                fx.pos.y = pl->unk478;
                fx.pos.z = (0.2f + ball->pos.z) - 0.4f * RAND_FLOAT();
                fx.colorFactor = -pl->unk308;
                fn_14_186F4(&fx, 0);
            }
            if ((g_font_char_var_3 & 1) == (ball->playerId & 1)) {
                s8 models[1] = {0x12};

                fn_14_2190C(&param);
                param.pos = &ball->pos;
                param.accel.y = -0.012495f;
                param.velScale = 0.2f;
                param.drag = 0.95f;
                param.scale = 1.5f;
                param.posScale = 0.3f;
                param.life = 0x14;
                param.count = 2;
                param.models = models;
                param.numModels = 1;
                fn_14_215F8(&param);
            }
            break;
        }
        if (pl->unk8 & 8) {
            *timer = 0;
        }
        if (*timer == 0) {
            pl->unk4B0 &= ~bit;
            switch (item) {
            case 1:
                break;
            case 0:
                pl->unk4E4 = 0.0f;
                break;
            case 2:
                pl->unk8 &= ~0x800;
                SoundOffID(ball->playerId + 0x283);
                break;
            }
        }
    }
}

#pragma auto_inline off

void fn_14_9F18(struct Ball *ball, struct BoatPlayer *pl)
{
    struct Ball *other;
    struct BoatPlayer *otherPl;
    float chance;
    s16 i;
    s16 n;

    if (pl->unk498[0] != 0) {
        n = 15;
        chance = 0.9f;
        if ((lbl_14_bss_C + ball->playerId * 8) % n == 0) {
            if (pl->unk498[0] == 8) {
                pl->unk8 |= 0x80;
                return;
            }
            i = 0;
            other = balls;
            otherPl = lbl_14_bss_208;
            for (; i < lbl_14_bss_24; i++, other++, otherPl++) {
                if (!BALL_IS_CPU(other) && other->playerId != ball->playerId && pl->unk4B0 == 0 && !(otherPl->unk8 & 8) && pl->unk2 >= otherPl->unk2 && !(pl->unk3F4[i] > 40.0) && !(fn_14_12944() > chance)) {
                    if (vec_dot_normalized_safe(&ball->vel, &pl->unk414[i]) >= 0.980785) {
                        pl->unk8 |= 0x80;
                        return;
                    }
                }
            }
        }
    }
}

#pragma auto_inline reset

void fn_14_A0E8(void)
{
    Vec offs[4] = {
        {0.04f, 0.145f, -0.02f},
        {0.04f, 0.145f, -0.02f},
        {0.04f, 0.12f, -0.02f},
        {0.11f, 0.18f, 0.0f},
    };
    S16Vec rots[4] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0x04FA, 0xE000, 0xFC72},
    };
    struct Ball *ball;
    struct BoatPlayer *pl;
    Vec *off;
    S16Vec *rot;
    s16 i;

    ball = balls;
    pl = lbl_14_bss_208;
    i = 0;
    for (; i < lbl_14_bss_24; i++, ball++, pl++) {
        if (!(pl->unk8 & 0x4000)) {
            off = &offs[ball->ape->charaId];
            rot = &rots[ball->ape->charaId];
            mtxa_from_quat(&ball->ape->rot);
            mtxa_mult_right(fn_1_1135A4(ball->ape));
            mtxa_translate(off);
            mtxa_rotate_z(rot->z);
            mtxa_rotate_y(rot->y);
            mtxa_rotate_x(rot->x);
            mtxa_to_mtx(pl->unk360);
            mtxa_from_translate_xyz(ball->ape->pos.x, ball->ape->pos.y - 0.12, ball->ape->pos.z);
            mtxa_mult_right(pl->unk360);
            mathutil_mtxA_get_translate(&pl->unk390);
        } else if (pl->unk3A8 > 0.0) {
            mtxa_from_mtx(pl->unk360);
            mathutil_mtxA_get_translate(&pl->unk390);
            PSVECAdd(&pl->unk390, &pl->unk39C, &pl->unk390);
            pl->unk39C.y -= ball->accel;
            PSVECScale(&pl->unk39C, &pl->unk39C, lbl_1_data_6580C);
            mathutil_mtxA_set_translate(&pl->unk390);
            pl->unk3A8 -= 1.0 / 60.0;
            if (pl->unk3A8 < 0.0) {
                pl->unk3A8 = 0.0f;
            }
        }
        mtxa_to_mtx(pl->unk360);
    }
}

void fn_14_A390(void)
{
    Vec dir;
    float len;
    struct Ball *a;
    struct Ball *b;
    struct BoatPlayer *pa;
    struct BoatPlayer *pb;
    s16 i;
    s16 j;

    a = balls;
    pa = lbl_14_bss_208;
    i = 0;
    for (; i < 7; i++, a++, pa++) {
        if (a->status == 2) {
            b = &balls[i + 1];
            pb = &lbl_14_bss_208[i + 1];
            j = i + 1;
            for (; j < 8; j++, b++, pb++) {
                if (b->status == 2) {
                    PSVECSubtract(&b->pos, &a->pos, &dir);
                    len = vec_normalize_len(&dir);
                    pb->unk3F4[i] = len;
                    pa->unk3F4[j] = len;
                    pa->unk414[j] = dir;
                    pb->unk414[i] = (Vec){-dir.x, -dir.y, -dir.z};
                } else {
                    pa->unk3F4[j] = -1.0f;
                    pa->unk414[j] = (Vec){0.0f, 0.0f, 0.0f};
                }
            }
        }
    }
}

s8 fn_14_A588(struct BoatPlayer *pl)
{
    Vec fwd;
    float best;
    float dot;
    s8 ret;
    Vec *dir;
    struct BoatPlayer *other;
    float *dist;
    s16 i;

    best = 0.0f;
    ret = -1;
    mtxa_from_rotate_y(pl->unk3E2);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &fwd);
    other = lbl_14_bss_208;
    dist = pl->unk3F4;
    dir = pl->unk414;
    i = 0;
    for (; i < 8; i++, other++, dist++, dir++) {
        if (!(*dist < 0.0) && !(*dist > 40.0) && other->unk4B0 == 0) {
            dot = mathutil_vec_dot_prod(&fwd, dir);
            if (dot <= 0.17) {
                continue;
            }
            dot = dot * (1.0 - *dist / 40.0);
            if (dot > best) {
                ret = i;
                best = dot;
            }
        }
    }
    return ret;
}

void fn_14_A6CC(struct Ball *ball)
{
    struct BoatPlayer plBackup;
    struct Ball backup;
    struct Effect fx;
    struct StartPos *start;
    s16 idx;
    s16 i;

    start = stagedef->startPos;
    PL = &lbl_14_bss_208[ball->playerId];
    backup = *ball;
    memset(ball, 0, sizeof(*ball));
    ball->playerId = backup.playerId;
    ball->status = backup.status;
    ball->ape = backup.ape;
    ball->state = backup.state;
    ball->unk178 = backup.unk178;
    ball->unk154 = backup.unk154;
    ball->unk155 = backup.unk155;
    ball->colorId = backup.colorId;
    ball->accel = backup.accel;
    ball->currRadius = backup.currRadius;
    ball->unk179 = backup.unk179;
    ball->flags = backup.flags;
    ball->flags &= 0x80000000000LL;
    if (start != NULL) {
        if (lbl_14_bss_20.unk2 & 1) {
            if ((dip_switches & 1) && ((lbl_1_bss_D4328 & 8) || (lbl_1_bss_D4328 & 4)) && lbl_1_bss_D4330 + 1 <= *(s32 *) stagedef->unk6C - 1 && lbl_1_bss_D4330 < stagedef->extraData->listCount) {
                idx = lbl_1_bss_D4330 + 1;
            } else {
                idx = *(s32 *) stagedef->unk6C - 1;
            }
        } else {
            idx = ball->playerId;
        }
        start = &start[idx];
        ball->pos = start->pos;
        ball->rotX = start->xrot;
        ball->rotY = start->yrot;
        ball->rotZ = start->zrot;
    } else {
        ball->pos = (Vec){0.0f, 0.0f, 0.0f};
        ball->pos.x += 2.0 * ball->playerId;
        ball->rotX = 0;
        ball->rotY = 0;
        ball->rotZ = 0;
    }
    plBackup = *PL;
    memset(PL, 0, sizeof(*PL));
    PL->unk0 = plBackup.unk0;
    PL->unk1 = plBackup.unk1;
    if (lbl_14_bss_8 & 4) {
        PL->unk3 = plBackup.unk3;
        PL->unk10 = plBackup.unk10;
        PL->unk2C0 = plBackup.unk2C0;
        PL->unk2CC = plBackup.unk2CC;
    } else if ((lbl_14_bss_20.unk2 & 0x10) && lbl_14_bss_25 != 0) {
        PL->unk5 = plBackup.unk5;
        PL->unk6 = plBackup.unk6;
    }
    if (BALL_IS_CPU(ball)) {
        struct StageExtraChannels *list = stagedef->extraData->list;
        if (ball->playerId > 0 && ball->playerId < stagedef->extraData->listCount) {
            PL->unk310 = list + ball->playerId - 1;
        } else {
            PL->unk310 = stagedef->extraData;
        }
        PL->unk334 = ((struct BoatCpuPoint **) &lbl_14_bss_2AE8->unk18)[ball->playerId];
    } else {
        if ((dip_switches & 1) && (lbl_14_bss_20.unk2 & 1) && (lbl_1_bss_D4328 & 4)) {
            ball->flags |= 0x80000000000LL;
            PL->unk310 = &stagedef->extraData->list[lbl_1_bss_D4330];
        }
        fn_14_1AED0((u8) ball->playerId);
        if (lbl_14_bss_20.unk2 & 1) {
            fn_14_1B490();
        } else {
            fn_14_1B124((u8) ball->playerId);
        }
        fn_14_1B550((u8) ball->playerId);
        fn_14_1CF24((u8) ball->playerId);
        if (modeCtrl.unk30 == 1) {
            fn_14_1C824((u8) ball->playerId);
        } else if (1.0 == cameras[ball->playerId].sub28.viewportW) {
            fn_14_1CBBC((u8) ball->playerId);
        }
        fn_14_20124((u8) ball->playerId);
        if (!(lbl_14_bss_8 & 4)) {
            memset(&fx, 0, sizeof(fx));
            fx.type = 3;
            fx.playerId = ball->playerId;
            fn_14_186F4(&fx, 0);
            fn_14_1B8C4((u8) ball->playerId);
        }
    }
    ball->prevPos = ball->pos;
    ball->restitution = 0.01f;
    PL->unk7 = 1;
    PL->unk2B4.unk8.min = 0x63;
    PL->unk2B4.unk8.sec = 0x3B;
    PL->unk2B4.unk8.cs = 0x63;
    PL->unk2DC = PROJ_INIT(lbl_14_bss_70, &ball->pos, 0.1f);
    PL->unk2E0 = PL->unk2DC;
    fn_14_11524(lbl_14_bss_70, PL->unk2DC, &PL->unk2E4);
    PL->unk2F0 = PL->unk2E4;
    PL->unk2FC = PL->unk2DC > 90.0 ? PL->unk2DC - 100.0 : PL->unk2DC;
    PL->unk300 = PL->unk2FC;
    PL->unk338 = PROJ_INIT(lbl_14_bss_74, &ball->pos, 0.1f);
    PL->unk33C = PROJ_INIT(lbl_14_bss_78, &ball->pos, 0.1f);
    fn_14_11524(lbl_14_bss_74, PL->unk338, &PL->unk340);
    fn_14_11524(lbl_14_bss_78, PL->unk33C, &PL->unk34C);
    if (BALL_IS_CPU(ball)) {
        PL->unk314 = PROJ_INIT(PL->unk310, &ball->pos, 0.1f);
        PL->unk318 = PL->unk314;
        fn_14_11524(PL->unk310, PL->unk314, &PL->unk31C);
        PL->unk328 = PL->unk31C;
    }
    PL->unk3E0 = ball->rotX;
    PL->unk3E2 = ball->rotY;
    PL->unk3E4 = ball->rotZ;
    PL->unk35C = 0.8 * -ball->currRadius;
    PL->unk3A8 = 1.0f;
    PL->unk3F0 = lbl_1_data_65800 * lbl_80198B34;
    PL->unk3C = 0;
    PL->unk40 = -1;
    PL->unk44 = 1.4f;
    PL->unk3B0 = 0.75f;
    PL->unk3AC = 0.5 * PL->unk3B0;
    PL->unk3B4 = PL->unk3AC;
    PL->unk478 = fn_14_13E1C(&ball->pos, &PL->unk47C);
    if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x8000)) {
        for (i = 0; i < 3; i++) {
            PL->unk498[i] = 1 << (fn_14_12904() % 4);
        }
    }
    if (lbl_14_bss_20.unk2 & 1) {
        s16 j;
        for (j = 0; j < 3; j++) {
            PL->unk498[j] = 8;
        }
    }
    PL->unk8 |= 4;
    ball->unk155 = 1;
}

void fn_14_B37C(struct Ball *ball)
{
    ball->vel.y -= ball->accel;
    ball->vel.y *= lbl_1_data_6580C;
    ball->pos.y += ball->vel.y;
}

void fn_14_B3B8(struct Ball *ball)
{
    PL->unk3EC = 0.2 * lbl_80198B34;
    ball->unk155 = 3;
    fn_14_B420(ball);
}

void fn_14_B420(struct Ball *ball)
{
    Vec bottom;
    Vec top;
    Vec kick;

    mtxa_from_identity();
    mtxa_sq_from_mtx(PL->unk360);
    mtxa_tf_vec_xyz(0.0f, 0.9f, 0.0f, &top);
    mtxa_tf_vec_xyz(0.0f, -0.9f, 0.0f, &bottom);
    if (top.y < -0.45 || bottom.y < -0.45) {
        kick = (Vec){0.0f, 0.0f, -0.4f};
        mtxa_from_rotate_y(PL->unk3E2);
        mtxa_rotate_x(0x1000);
        mtxa_tf_vec(&kick, &kick);
        PSVECAdd(&ball->vel, &kick, &ball->vel);
        PL->unk3EC = 0.1 * lbl_80198B34;
        ball->unk155 = 4;
        fn_14_B5F8(ball);
        return;
    }
    ball->vel.y -= ball->accel;
    ball->vel.y *= lbl_1_data_6580C;
    ball->pos.y += ball->vel.y;
    if (PL->unk3EC == 0 && (PL->unk3C != 2 || PL->unk3C != 1)) {
        PL->unk8 |= 0x20000;
    }
}

void fn_14_B5F8(struct Ball *ball)
{
    PL->unk8 &= ~0x20000;
    if (PL->unk3EC == 0) {
        PL->unk3B4 = 0.86f;
        ball->unk155 = 5;
    }
    fn_14_B66C(ball);
}

void fn_14_B66C(struct Ball *ball)
{
    Vec flat;
    struct BoatPlayer *pl;

    fn_14_D900(&PL->unk3E0, &PL->unk3E6, 100.0f);
    fn_14_D900(&PL->unk3E4, &PL->unk3E8, 100.0f);
    if (!(PL->unk4B0 & 4)) {
        if (BALL_IS_CPU(ball)) {
            fn_14_C7A4(ball, PL);
        } else {
            fn_14_C498(ball, PL);
        }
        fn_14_CB3C(ball, PL);
        fn_14_236A4(ball, PL);
    }
    fn_14_CED4(ball, PL);
    fn_14_CFA0(ball, PL);
    if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x80)) {
        flat = (Vec){ball->vel.x, 0.0f, ball->vel.z};
        if (216.0f * mathutil_vec_len(&flat) >= lbl_1_data_657E0) {
            vec_set_len(lbl_1_data_657E0 / 216.0f, &flat, &flat);
            ball->vel.x = flat.x;
            ball->vel.z = flat.z;
        }
    }
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    PL->unk3E2 += PL->unk358;
    fn_14_D800(ball, PL);
    if (PL->unk4A4 & 8) {
        PL->unk3E6 = 0;
        PL->unk3E8 = 0;
    } else {
        fn_14_D864(ball, PL);
    }
    fn_14_DD70(ball, PL);
    pl = PL;
    if (BALL_IS_CPU(ball)) {
        fn_14_9F18(ball, pl);
    } else if ((pl->unk2D8 & 0x100) && pl->unk498[0] != 0 && pl->unk3F2 == 0) {
        pl->unk8 |= 0x80;
        pl->unk3F2 = (lbl_80198B38 * 0xA) / 60;
    } else if (pl->unk2D8 & 0x800) {
        pl->unk8 |= 0x100;
    }
    if (PL->unk8 & 8) {
        ball->unk155 = 6;
    }
}

void fn_14_BA58(struct Ball *ball)
{
    if (!BALL_IS_CPU(ball)) {
        fn_14_E3CC(&cameras[ball->playerId], 2);
        fn_14_1D3B4((u8) ball->playerId);
        if (lbl_14_bss_14 == -1) {
            g_maybe_smth_with_music(-1, 1);
        }
        if (lbl_14_bss_24 >= 2 && PL->unk2 >= lbl_14_bss_24) {
            g_SoftStreamStart_with_some_defaults_2(0x4C);
            call_SoundReqID_arg_0(0x266);
            lbl_14_bss_14 = 3.0 * lbl_80198B34;
        } else {
            g_SoftStreamStart_with_some_defaults_2(0x4D);
            call_SoundReqID_arg_0(0x265);
            lbl_14_bss_14 = 5.5 * lbl_80198B34;
        }
    }
    PL->unk3E6 = PL->unk3E8 = 0;
    PL->unk3EC = lbl_80198B38 * 2;
    ball->unk155 = 7;
    fn_14_BC1C(ball);
}

void fn_14_BC1C(struct Ball *ball)
{
    fn_14_D900(&PL->unk3E0, &PL->unk3E6, 1024.0f);
    fn_14_D900(&PL->unk3E4, &PL->unk3E8, 1024.0f);
    fn_14_CB3C(ball, PL);
    fn_14_CED4(ball, PL);
    fn_14_CFA0(ball, PL);
    PSVECScale(&ball->vel, &ball->vel, 0.97f);
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    PL->unk3E2 += PL->unk358;
    if (PL->unk3EC == 0) {
        PL->unk4 = lbl_14_data_1B8[lbl_14_bss_24 - PL->unk2];
        PL->unk5 += PL->unk4;
        if (PL->unk2 >= 2 && PL->unk2 == lbl_14_bss_24) {
            ball->unk155 = 0xC;
            return;
        }
        ball->unk155 = 8;
    }
}

void fn_14_BE04(struct Ball *ball)
{
    if (!BALL_IS_CPU(ball)) {
        fn_14_E3CC(&cameras[ball->playerId], 4);
    }
    PL->unk3E6 = 0;
    PL->unk3E8 = 0;
    ball->flags |= 0x240;
    PL->unk3EC = lbl_80198B38 * 2;
    ball->unk155 = 9;
    fn_14_BEF8(ball);
}

void fn_14_BEF8(struct Ball *ball)
{
    struct Effect fx;

    memset(&fx, 0, sizeof(fx));
    fx.type = 4;
    fx.playerId = ball->playerId;
    fx.pos = ball->pos;
    spawn_effect(&fx);
    fn_14_D900(&PL->unk3E0, &PL->unk3E6, 100.0f);
    fn_14_D900(&PL->unk3E4, &PL->unk3E8, 100.0f);
    PSVECScale(&ball->vel, &ball->vel, lbl_1_data_6580C);
    ball->vel.y += ball->accel;
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    if (PL->unk3EC == 0) {
        ball->unk155 = 0xA;
    }
}

void fn_14_C01C(struct Ball *ball)
{
    if (!BALL_IS_CPU(ball)) {
        destroy_sprite_with_unique_id(ball->playerId + 0x6C);
        fn_14_E3CC(&cameras[ball->playerId], 6);
        fn_14_1FC2C((u8) ball->playerId);
        if (lbl_14_bss_20.unk2 & 1) {
            fn_14_208E4();
        } else {
            fn_14_1DFC8((u8) ball->playerId);
        }
    }
    PL->unk8 |= 0x2000;
    ball->prevPos = lbl_14_bss_2AF0.unk0[PL->unk2 - 1];
    ball->pos = ball->prevPos;
    ball->vel = (Vec){0.0f, 0.0f, 0.0f};
    ball->restitution = 0.5f;
    PL->unk3B4 = PL->unk3AC;
    PL->unk3E0 = 0;
    PL->unk3E2 = 0;
    PL->unk3E4 = 0;
    ball->unk155 = 0xB;
    fn_14_C200(ball);
}

void fn_14_C200(struct Ball *ball)
{
    ball->vel.y -= ball->accel;
    ball->vel.y *= lbl_1_data_6580C;
    ball->pos.y += ball->vel.y;
}

void fn_14_C23C(struct Ball *ball)
{
    if (!BALL_IS_CPU(ball)) {
        destroy_sprite_with_unique_id(ball->playerId + 0x6C);
        fn_14_1FC2C((u8) ball->playerId);
        fn_14_1DFC8((u8) ball->playerId);
    }
    ball->unk155 = 0xE;
    fn_14_C370(ball);
}

void fn_14_C2D0(struct Ball *ball)
{
    if (!BALL_IS_CPU(ball)) {
        destroy_sprite_with_unique_id(ball->playerId + 0x6C);
        fn_14_1D7EC((u8) ball->playerId);
    }
    PL->unk8 |= 0x20;
    ball->unk155 = 0xE;
    fn_14_C370(ball);
}

void fn_14_C370(struct Ball *ball)
{
    fn_14_D900(&PL->unk3E0, &PL->unk3E6, 100.0f);
    fn_14_D900(&PL->unk3E4, &PL->unk3E8, 100.0f);
    fn_14_CB3C(ball, PL);
    fn_14_D864(ball, PL);
    fn_14_CED4(ball, PL);
    fn_14_CFA0(ball, PL);
    PSVECScale(&ball->vel, &ball->vel, 0.97f);
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    PL->unk3E2 += PL->unk358;
}

void fn_14_C498(struct Ball *ball, struct BoatPlayer *pl)
{
    Vec v;
    float f31;
    float f30;
    float f29;
    float f28;

    f31 = pl->unk4C / 40.0;
    f30 = 1.0f;
    f29 = 1.0f;
    f28 = 1.0f;
    if (pl->unk3C == 0) {
        return;
    }
    if (pl->unk4A4 & 8) {
        if (f31 > 1.0) {
            f31 = 1.0f;
        }
    } else {
        if (pl->unk308 < 0.0f || pl->unk308 > pl->unk3AC) {
            return;
        }
        if (f31 > 1.0) {
            return;
        }
    }
    if (pl->unk3C == 1) {
        if (pl->unk38 >= 0xC8) {
            f30 = pl->unk38 / 200.0;
            f29 = f30;
        }
        f28 = -1.0f;
    } else if (pl->unk3C == 2) {
        if (pl->unk3A > 0x12C) {
            f30 = pl->unk3A / 200.0;
            f29 = f30;
        }
        f28 = 1.0f;
    }
    if (f30 > lbl_1_data_657A4) {
        f30 = lbl_1_data_657A4;
    }
    if (f29 > lbl_1_data_657A0) {
        f29 = lbl_1_data_657A0;
    }
    f30 = 1.0 / f30;
    if (f31 < 0.75) {
        f30 *= 0.5;
    }
    f30 = f30 * fn_14_CA68(ball, pl);
    if (pl->unk4A4 & 8) {
        if (f31 >= 1.0) {
            f30 *= lbl_1_data_65794;
        }
    }
    if (!(pl->unk4A4 & 8) || f31 < 1.0) {
        pl->unk358 = (float) pl->unk358 + f29 * (lbl_1_data_657F0 * (f28 * f31));
    }
    mtxa_from_rotate_y(pl->unk3E2);
    mtxa_rotate_x((s16) (pl->unk3E0 + 0x800));
    mtxa_rotate_z(pl->unk3E4);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -(f30 * (f31 * lbl_1_data_657F4)), &v);
    PSVECAdd(&pl->unk3B8, &v, &pl->unk3B8);
    fn_14_DADC(ball, pl);
}

void fn_14_C7A4(struct Ball *ball, struct BoatPlayer *pl)
{
    Vec v;
    struct BoatCpuPoint *p;
    struct BoatCpuPoint *q;
    float t;
    float t1;
    float speed;
    s16 steer;

    if (!(pl->unk4A4 & 8) && (pl->unk308 < 0.0 || pl->unk308 > pl->unk3AC)) {
        return;
    }
    p = pl->unk334;
    if (dip_switches & 1) {
        if (lbl_1_bss_D4328 & 0x40000) {
            return;
        }
        if (lbl_1_bss_D4328 & 4) {
            p = lbl_1_bss_D43B4;
        }
    }
    q = p + 1;
    for (; q->unk0 >= 0.0; p++, q++) {
        if (p->unk0 <= pl->unk314 && pl->unk314 < q->unk0) {
            break;
        }
    }
    if (q->unk0 < 0.0) {
        return;
    }
    t = (pl->unk314 - p->unk0) / (q->unk0 - p->unk0);
    t1 = 1.0 - t;
    speed = p->unk4 * t1 + q->unk4 * t;
    steer = (float) p->unk8 * t1 + (float) q->unk8 * t;
    fn_14_11524(pl->unk310, pl->unk314 + 5.0 * lbl_14_bss_2AE8->unkC, &v);
    PSVECSubtract(&v, &ball->pos, &v);
    mtxa_from_rotate_y(math_atan2(v.x, v.z) + 0x8000);
    mtxa_rotate_x((s16) (pl->unk3E0 + 0x800));
    mtxa_rotate_z(pl->unk3E4);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &v);
    pl->unk358 += steer;
    if (pl->unk4A4 & 8) {
        PSVECScale(&v, &v, 1.8 * (0.0009 * speed));
    } else {
        PSVECScale(&v, &v, 0.0009 * speed);
    }
    PSVECAdd(&pl->unk3B8, &v, &pl->unk3B8);
}

float fn_14_CA68(struct Ball *ball, struct BoatPlayer *pl)
{
    float diff;

    if (!(lbl_14_bss_20.unk2 & 8)) {
        return 1.0f;
    }
    if (modeCtrl.unk30 <= 1) {
        return 1.0f;
    }
    if (pl->unk8 & 0x40) {
        return 1.0f;
    }
    if (pl->unk2 <= 1) {
        return 1.0f;
    }
    diff = lbl_14_bss_2988[0]->unk304 - pl->unk304;
    if (diff <= 0.0) {
        return 1.0f;
    }
    if (diff > 30.0) {
        return 1.2f;
    }
    return 1.0 + 0.2 * (diff / 30.0);
}

void fn_14_CB3C(struct Ball *ball, struct BoatPlayer *pl)
{
    Vec force;
    Vec p1;
    Vec p2;
    Vec p3;
    Vec p4;
    Vec d1;
    Vec d2;
    float step;
    float len1;
    float len2;
    float f2;
    s16 diff;
    s16 r4;
    s16 r5;
    struct StageExtraChannels *path;

    force = (Vec){0.0f, 0.0f, 0.0f};
    if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x20000)) {
        return;
    }
    if (!(pl->unk308 < 0.0)) {
        if (lbl_14_bss_20.unk0 != 2 || pl->unk2DC < 83.0 || pl->unk2DC > 87.5) {
            step = 3.0 * lbl_14_bss_2AE8->unkC;
            fn_14_11524(lbl_14_bss_7C, pl->unk2DC, &p1);
            fn_14_11524(lbl_14_bss_7C, pl->unk2DC + step, &p2);
            PSVECSubtract(&p2, &p1, &force);
            vec_set_len(lbl_1_bss_D43B0, &force, &force);
        } else if (lbl_14_bss_80 >= 2) {
            fn_14_11524(lbl_14_bss_7C, pl->unk2DC, &p3);
            fn_14_11524((struct StageExtraChannels *) lbl_14_bss_7C + 1, pl->unk2DC, &p4);
            PSVECSubtract(&p3, &ball->pos, &d1);
            PSVECSubtract(&p4, &ball->pos, &d2);
            len1 = vec_normalize_len(&d1);
            len2 = vec_normalize_len(&d2);
            if (pl->unk2DC > 84.4 || mathutil_vec_dot_prod(&d1, &d2) > 0.0) {
                path = lbl_14_bss_7C;
                if (len1 < len2) {
                    path++;
                }
                step = 3.0 * lbl_14_bss_2AE8->unkC;
                fn_14_11524(path, pl->unk2DC, &p1);
                fn_14_11524(path, pl->unk2DC + step, &p2);
                PSVECSubtract(&p2, &p1, &force);
                vec_set_len(lbl_1_bss_D43B0, &force, &force);
            }
        }
        PSVECAdd(&pl->unk3C4, &force, &pl->unk3C4);
        diff = (s16) (math_atan2(force.x, force.z) + 0x8000) - pl->unk3E2;
        if (diff > 0x3800 || diff < -0x3800) {
            f2 = 1.0f;
            r5 = 0x64;
        } else {
            f2 = lbl_1_data_657A8;
            r5 = lbl_1_data_657AC;
        }
        r4 = (float) diff * f2;
        if (r4 > r5) {
            r4 = r5;
        } else if (r4 < -r5) {
            r4 = -r5;
        }
        pl->unk358 += r4;
    }
}

void fn_14_CED4(struct Ball *ball, struct BoatPlayer *pl)
{
    float f4;
    float f5;

    if (pl->unk308 < 0.0) {
        f4 = 1.0f;
        f5 = 0.0f;
    } else if (pl->unk308 >= pl->unk3AC) {
        f4 = 0.0f;
        f5 = 1.0f;
    } else {
        f5 = pl->unk308 / pl->unk3AC;
        f4 = 1.0 - f5;
    }
    pl->unk3B8.y += -f4 * ball->accel + lbl_1_data_65808 * (f5 * ball->accel);
    pl->unk3B8.y -= pl->unk4E4;
    pl->unk3DC = f4 * lbl_1_data_6580C + f5 * lbl_1_data_65810;
}

void fn_14_CFA0(struct Ball *ball, struct BoatPlayer *pl)
{
    Vec a;
    Vec d;
    Vec e;
    Vec f;
    Vec b;
    Vec c;
    s16 sp8;
    float dot;

    PSVECAdd(&ball->vel, &pl->unk3B8, &ball->vel);
    PSVECAdd(&ball->vel, &pl->unk3C4, &ball->vel);
    PSVECAdd(&ball->vel, &pl->unk3D0, &ball->vel);
    if (pl->unk4B0 & 4) {
        if (pl->unk8 & 0x800) {
            ball->vel.y += lbl_1_data_657D4 * ball->accel;
        } else {
            ball->vel.y += lbl_1_data_657D8 * ball->accel;
        }
    }
    PSVECScale(&ball->vel, &ball->vel, pl->unk3DC);
    pl->unk358 = lbl_1_data_657DC * ((float) pl->unk358 * pl->unk3DC);
    if (!(pl->unk4B0 & 1) && 0.0f < pl->unk308 && pl->unk308 < pl->unk3AC && pl->unk30C < pl->unk308) {
        float dot2;
        float f2;

        dot2 = mathutil_vec_dot_prod(&pl->unk47C, &ball->vel);
        if (dot2 < 0.0) {
            f2 = lbl_1_data_657FC;
            if (pl->unk8 & 0x10000) {
                f2 *= 1.5;
                if (f2 > 0.99) {
                    f2 = 0.99f;
                }
            }
            PSVECScale(&pl->unk47C, &a, -dot2 * f2);
            PSVECAdd(&ball->vel, &a, &ball->vel);
        }
    }
    b = (Vec){ball->vel.x, 0.0f, ball->vel.z};
    mtxa_from_rotate_y(pl->unk3E2);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &c);
    dot = mathutil_vec_dot_prod(&b, &c);
    PSVECScale(&c, &c, -dot);
    PSVECAdd(&b, &c, &d);
    b = pl->unk3C4;
    vec_normalize_len(&b);
    dot = mathutil_vec_dot_prod(&b, &d);
    PSVECScale(&b, &e, dot);
    PSVECScale(&b, &b, -dot);
    PSVECAdd(&d, &b, &f);
    if (ball->playerId == 0) {
        lbl_14_bss_29AC = e;
        lbl_14_bss_29B8 = f;
    }
    PSVECScale(&e, &e, 0.1 * -lbl_1_data_657EC);
    PSVECAdd(&ball->vel, &e, &ball->vel);
    PSVECScale(&f, &f, -lbl_1_data_657EC);
    PSVECAdd(&ball->vel, &f, &ball->vel);
    if (BALL_IS_CPU(ball)) {
        fn_14_D4B4(ball, pl);
        if (pl->unk308 > 0.0) {
            sp8 = math_atan2(ball->vel.x, ball->vel.z) + 0x8000;
            fn_14_D900(&pl->unk3E2, &sp8, 100.0f);
        }
    }
    pl->unk3B8 = (Vec){0.0f, 0.0f, 0.0f};
    pl->unk3C4 = (Vec){0.0f, 0.0f, 0.0f};
    pl->unk3D0 = (Vec){0.0f, 0.0f, 0.0f};
}

void fn_14_D4B4(struct Ball *ball, struct BoatPlayer *pl)
{
    Vec v;
    Vec a;
    Vec b;
    Vec c;
    Vec n;
    float dot;
    float f30;
    float t;

    v = (Vec){ball->vel.x, 0.0f, ball->vel.z};
    n = (Vec){0.0f, 0.0f, 0.0f};
    if ((dip_switches & 1) && (lbl_1_bss_D4328 & 0x40000)) {
        return;
    }
    if (!(pl->unk308 < 0.0) && !(pl->unk8 & 0x200) && pl->unk4B0 == 0) {
        t = mathutil_vec_dot_prod(&pl->unk47C, &v);
        PSVECScale(&pl->unk47C, &c, -t);
        PSVECAdd(&v, &c, &v);
        PSVECScale(&pl->unk47C, &n, t);
        mathutil_vec_len(&v);
        fn_14_11524(pl->unk310, pl->unk314 + (double) lbl_14_bss_2AE8->unkC, &a);
        PSVECSubtract(&a, &pl->unk31C, &a);
        vec_normalize_len(&a);
        t = mathutil_vec_dot_prod(&pl->unk47C, &a);
        PSVECScale(&pl->unk47C, &c, -t);
        PSVECAdd(&a, &c, &a);
        vec_normalize_len(&a);
        dot = mathutil_vec_dot_prod(&a, &v);
        PSVECScale(&a, &c, -dot);
        PSVECAdd(&v, &c, &b);
        f30 = vec_normalize_len(&b);
        PSVECSubtract(&ball->pos, &pl->unk31C, &c);
        vec_normalize_len(&c);
        t = vec_dot_normalized_safe(&c, &b);
        if (t > 0.0f) {
            f30 *= 0.5;
        }
        PSVECScale(&a, &a, dot);
        PSVECScale(&b, &b, f30);
        PSVECAdd(&a, &b, &v);
        PSVECAdd(&v, &n, &v);
        ball->vel.x = v.x;
        ball->vel.z = v.z;
    }
}

void fn_14_D800(struct Ball *ball, struct BoatPlayer *pl)
{
    if (pl->unk308 < 0.0) {
        return;
    }
}

void fn_14_D81C(u8 *value, u8 target, u8 rate)
{
    if (*value >= rate) {
        *value -= rate;
    } else {
        *value = 0;
    }
    if (*value < target) {
        *value = target;
    }
}

void fn_14_D864(struct Ball *ball, struct BoatPlayer *pl)
{
    Vec n;

    if (pl->unk8 & 0x400) {
        if (pl->unk308 < 0.0f || pl->unk308 > pl->unk3AC) {
            return;
        }
        mtxa_from_rotate_y(pl->unk3E2);
        mtxa_rigid_inv_tf_vec(&pl->unk47C, &n);
        pl->unk3E6 = math_atan2(n.z, n.y);
        pl->unk3E8 = math_atan2(-n.x, n.y);
    }
}

void fn_14_D900(s16 *cur, s16 *target, float step)
{
    s32 diff;

    if (*cur != *target) {
        diff = *target - *cur;
        if (diff >= 0x8000) {
            *cur = (float) *cur - step;
            if (*cur > 0 && *cur < *target) {
                *cur = *target;
            }
        } else if (diff <= -0x8000) {
            *cur = (float) *cur + step;
            if (*cur < 0 && *cur > *target) {
                *cur = *target;
            }
        } else if (*cur < *target) {
            *cur = (float) *cur + step;
            if (*cur > *target) {
                *cur = *target;
            }
        } else {
            *cur = (float) *cur - step;
            if (*cur < *target) {
                *cur = *target;
            }
        }
    }
}

void fn_14_DADC(struct Ball *ball, struct BoatPlayer *pl)
{
    struct BoatParticleParam param;
    Vec pos;
    Vec vel;
    s8 models[1] = {0x12};
    float f1;
    float t;
    s32 sound;

    switch (modeCtrl.unk30) {
    case 1:
    case 2:
        break;
    case 3:
    case 4:
        if ((g_font_char_var_3 & 3) != ball->playerId) {
            return;
        }
        break;
    }
    if (pl->unk14 != 0 || pl->unk19 != 0) {
        mtxa_sq_from_mtx(pl->unk360);
        mathutil_mtxA_set_translate(&ball->ape->pos);
        if (pl->unk14 > pl->unk19) {
            mtxa_translate_xyz(0.0f, 0.9f, 0.0f);
            sound = 0x25D;
        } else {
            mtxa_translate_xyz(0.0f, -0.9f, 0.0f);
            sound = 0x25E;
        }
        mathutil_mtxA_get_translate(&pos);
        if (!(pos.y > pl->unk478)) {
            pos.y = 0.1f + pl->unk478;
            if (pl->unk474 == 0) {
                fn_14_12B24(sound, &pos);
                pl->unk474 = 0.5 * lbl_80198B34;
            }
            if (pl->unk44 < 2.81f) {
                f1 = pl->unk44;
            } else {
                f1 = 2.81f;
            }
            t = 0.6849315f * (f1 - 1.35f);
            vel.x = 0.5f * ball->vel.x;
            vel.y = 0.1f + 0.1f * t;
            vel.z = 0.5f * ball->vel.z;
            fn_14_2190C(&param);
            param.pos = &pos;
            param.vel = &vel;
            param.accel.y = -0.012495f;
            param.velScale = 0.03f + 0.04f * t;
            param.drag = 0.95f;
            param.scale = 0.7f;
            param.posScale = 0.17f;
            param.life = 13.0f + 8.0f * t;
            param.count = 1;
            param.models = models;
            param.numModels = 1;
            fn_14_215F8(&param);
        }
    }
}

void fn_14_DD70(struct Ball *ball, struct BoatPlayer *pl)
{
    struct BoatParticleParam param;
    Vec right;
    Vec pos;
    Vec vel;
    s8 models[1] = {0x0F};
    float f1;
    float t;
    float sq;

    switch (modeCtrl.unk30) {
    case 1:
    case 2:
        if ((g_font_char_var_3 & 1) != (ball->playerId & 1)) {
            return;
        }
        break;
    case 3:
    case 4:
        if ((g_font_char_var_3 & 3) != ball->playerId) {
            return;
        }
        break;
    }
    if (pl->unk308 <= 0.1f) {
        return;
    }
    if (pl->unk308 >= 1.5f * pl->unk3AC) {
        return;
    }
    if (ball->vel.y >= -0.005f) {
        return;
    }
    sq = mathutil_sum_of_sq_3(0.5 * ball->vel.x, ball->vel.y, 0.5 * ball->vel.z);
    if (sq < 0.0025000002f) {
        return;
    }
    f1 = math_sqrt(sq);
    t = 4.347826f * ((f1 < 0.23f) ? f1 : 0.23f);
    if (pl->unk8 & 0x10000) {
        pl->unk476 = 0.5 * lbl_80198B34;
    } else if (pl->unk476 == 0) {
        fn_14_12B24(0x280, &ball->pos);
        pl->unk476 = 0.2 * lbl_80198B34;
    }
    mtxa_from_rotate_y(pl->unk3E2);
    mtxa_translate_xyz(pl->unk3AC, 0.0f, 0.0f);
    mathutil_mtxA_get_translate(&right);
    PSVECAdd(&ball->pos, &right, &pos);
    vel.x = 0.3f * ball->vel.x + 0.2f * right.x;
    vel.y = 0.1f + 0.1f * t;
    vel.z = 0.3f * ball->vel.z + 0.2f * right.z;
    fn_14_2190C(&param);
    param.pos = &pos;
    param.vel = &vel;
    param.accel.y = -0.012495f;
    param.velScale = 0.15f * t;
    param.drag = 0.95f;
    param.scale = 0.7f;
    param.posScale = 0.2f;
    param.life = 13.0f + 8.0f * t;
    param.count = 1;
    param.models = models;
    param.numModels = 1;
    fn_14_215F8(&param);
    PSVECSubtract(&ball->pos, &right, &pos);
    vel.x = 0.3f * ball->vel.x - 0.2f * right.x;
    vel.y = 0.1f + 0.1f * t;
    vel.z = 0.3f * ball->vel.z - 0.2f * right.z;
    param.pos = &pos;
    param.vel = &vel;
    fn_14_215F8(&param);
}

const float lbl_14_rodata_D70_tail[3] = {0.0f, 0.0f, 0.0f};

/* pin .bss order; stripped at link */
void boat_bss_pin_29C4(void) { lbl_14_bss_29C4[0] = NULL; }

