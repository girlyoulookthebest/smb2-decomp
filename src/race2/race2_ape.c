#include "background.h"
#include "race2.h"
#include "item.h"
#include "libc.h"
#include "event.h"
#include "input.h"
#include "sprite.h"
#include "sound.h"
#include "mathutil.h"
#include "camera.h"
#include "mode.h"
#include "ball.h"
#include "stage.h"

void fn_1_113190(struct Ape *, int, float, int, int);
void fn_1_1132D0(struct Ape *, int, int, int);
void fn_1_11355C(struct Ape *, float);
float fn_1_45A8C();
void fn_1_46958();
void g_ape_anim_body();
void g_ape_upright();
void g_move_ape_into_ball();
extern s16 lbl_10_bss_121C;
extern s32 lbl_10_bss_1224;
void mtxa_to_quat();
void fn_10_18EA4(struct Ape *ape, float farg0);

void fn_10_18CAC(struct Ape *ape);
void fn_10_18EA4(struct Ape *ape, float farg0);

void fn_10_18CAC(struct Ape *ape)
{
    struct RaycastHit hit;
    float f31;
    s32 tr28;
    struct Ball *tr30;

    tr30 = &balls[ape->ballIdx];
    if ((u32) (g_some_other_flags & 0xA) == 0) {
        raycast_stage_down(&tr30->pos, &hit, NULL);
        ape->flags &= ~0x13;
        if (((u32) (hit.flags & 1) == 0) && (tr30->vel.y < -0.16203703703703703)) {
            ape->flags |= 2;
        } else {
            if (mathutil_vec_len(&tr30->unkC0) < 0.0002777777777777778) {
                ape->flags |= 1;
            }
        }
        tr28 = !(ape->flags & 3);
        g_ape_upright(ape);
        if (tr28 != 0) {
            f31 = fn_1_45A8C(ape);
        } else {
            f31 = 0.0f;
            mtxa_from_quat(&ape->rot);
            mtxa_normalize_basis();
            if ((u32) (ape->flags & 2) != 0) {
                fn_1_46958(ape);
            }
        }
        if ((tr30->flags & 0x20ULL) != 0) {
            f31 = mathutil_vec_len(&tr30->vel);
        }
        mtxa_to_quat(&ape->rot);
        fn_10_18EA4(ape, f31);
        g_ape_anim_body(ape);
        if ((u32) (ape->flags & 8) == 0) {
            g_move_ape_into_ball(ape);
        }
    }
}

void fn_10_18EA4(struct Ape *ape, float farg0)
{
    struct Ball *tr30;
    struct RacePlayer *tr29;

    tr30 = &balls[ape->ballIdx];
    tr29 = (struct RacePlayer *)tr30->unk150;
    fn_1_11355C(ape, 1.0f);
    if (lbl_10_bss_121C <= 3) {
        if (lbl_10_bss_121C == 2) {
            fn_1_113190(ape, 0, 0.0f, 0, 0);
        } else if ((u32) (ape->flags & 0x2000) != 0) {
            fn_1_113190(ape, 0, 0.0f, 1, 0);
        }
        return;
    }
    if (lbl_10_bss_121C <= 5) {
        if ((lbl_10_bss_1224 > 0xF0) && ((tr30->flags & 0x1ULL) != 0)) {
            fn_1_1132D0(ape, 0xC, 0, 5);
            ape->flags &= 0xFFFFFFFE;
        } else if (lbl_10_bss_1224 <= 0xF0) {
            fn_1_1132D0(ape, 0xC, 0, 0);
            ape->flags &= 0xFFFFFFFE;
        }
        return;
    }
    if ((u32) (ape->flags & 2) != 0) {
        fn_1_113190(ape, 0, 0.0f, 5, 0);
        return;
    }
    if ((u32) (tr29->flags & 0x80000) != 0) {
        fn_1_113190(ape, 0, 0.0f, 8, 1);
        return;
    }
    if ((u32) (tr29->flags & 2) != 0) {
        if (tr29->unk3E >= 5) {
            fn_1_113190(ape, 0, 0.0f, 7, 0);
        } else {
            fn_1_113190(ape, 0, 0.0f, 7, (5 - tr29->unk3E) * 2);
        }
        return;
    }
    if ((s32) (tr29->unk2C2 & 2) != 0) {
        if (((s32) ape->unk78 == 0) && (((s32) ape->unk7C == 6) || (((s32) ape->unk7C == 1) && ((u32) (ape->flags & 0x2000) != 0)))) {
            fn_1_1132D0(ape, 0xC, 0, 6);
            ape->flags &= 0xFFFFFFFE;
        } else {
            fn_1_1132D0(ape, 0xC, 0, 1);
            ape->flags &= 0xFFFFFFFE;
        }
        return;
    }
    if ((s32) (tr29->unk2C2 & 0x10) != 0) {
        fn_1_1132D0(ape, 0xC, 0, 4);
        ape->flags &= 0xFFFFFFFE;
        return;
    }
    if ((s32) (tr29->unk2C2 & 4) != 0) {
        mtxa_from_quat(&ape->rot);
        mtxa_rotate_y(0x800);
        mtxa_to_quat(&ape->rot);
    }
    if (((s32) (tr29->unk2BC & 0x20) != 0) && (tr29->unk304[5] >= 0x78) && (tr30->modelScale < 2.0)) {
        fn_1_11355C(ape, 3.0f);
        fn_1_113190(ape, 0, 0.0f, 1, 4);
        return;
    }
    if ((tr30->flags & 0x20ULL) != 0) {
        fn_1_113190(ape, 0, farg0, 3, 0);
        return;
    }
    if ((tr30->flags & 0x2ULL) != 0) {
        fn_1_113190(ape, 0, 0.0f, 4, 1);
        return;
    }
    if ((u32) (ape->flags & 1) != 0) {
        fn_1_113190(ape, 0, 0.0f, 1, 0);
        return;
    }
    {
        float sp8[12] = {0.0f, 1.0f, 2.5f, 4.0f, 5.5f, 7.0f, 8.4f, 10.0f, 13.0f, 16.0f, 30.0f, 65.0f};
        float f1;
        s32 r29;
        s16 r4;

        f1 = 216.0f * farg0;
        r29 = 0;
        if (f1 >= 100.0) {
            if (f1 > 160.0) {
                f1 = 160.0f;
            }
            r29 = 0xC;
            fn_1_11355C(ape, 0.65 + (0.35 * ((f1 - 100.0) / 60.0)));
        } else {
            for (r4 = 0xB; r4 >= 0; r4--) {
                if (sp8[r4] <= f1) {
                    r29 = r4;
                    break;
                }
            }
        }
        fn_1_113190(ape, 0, 0.0f, 2, r29);
    }
}

