#include "boat.h"
#include "macros.h"
#include "dip.h"

extern u8 lbl_14_bss_24;

void fn_14_108A4(struct Ball *ball, struct BoatPlayer *pl)
{
    struct Ape *ape;
    float t;

    ape = ball->ape;
    if (ball->unk154 >= 3 && ball->unk154 <= 5) {
        if ((pl->unk8 & 0x20000) != 0 && pl->unk3C == 0) {
            fn_14_10DE0(ball, pl);
        } else if ((ball->flags & 0x80000000000LL) != 0) {
            fn_14_11298(ball, pl);
        } else {
            fn_14_10E58(ball, pl);
        }
    } else if ((pl->unk8 & 8) != 0) {
        if (pl->unk2 >= 2 && pl->unk2 == lbl_14_bss_24) {
            fn_14_10CE4(ball, pl);
        } else {
            fn_14_10B28(ball, pl);
        }
    } else if ((pl->unk8 & 0x20) != 0) {
        fn_14_10CE4(ball, pl);
    } else {
        fn_1_11355C(ape, 1.0f);
        fn_1_113190(ape, 0x12, 0.0f, 0, 0);
    }
    g_ape_anim_body(ape);
    if ((ape->flags & 8) == 0) {
        g_move_ape_into_ball(ape);
    }
    if ((lbl_1_bss_D4328 & 0x10) == 0) {
        if ((pl->unk8 & 0x28) != 0) {
            if (pl->unk494 < 0.30000000000000004) {
                pl->unk494 += 0.30000000000000004 / (0.2 * lbl_80198B34);
                if (pl->unk494 > 0.30000000000000004) {
                    pl->unk494 = 0.3f;
                }
            }
        } else if (pl->unk4BC[3] != 0) {
            t = (1.0 - pl->unk3B0) / 0.25;
            pl->unk494 = lbl_1_data_657F8 * t;
        } else {
            pl->unk494 = lbl_1_data_657F8;
        }
    }
    ape->pos.y += pl->unk494;
    mtxa_from_rotate_y(pl->unk3E2);
    mtxa_rotate_x(pl->unk3E0);
    mtxa_rotate_z(pl->unk3E4);
    mtxa_to_quat(&ape->rot);
}

void fn_14_10B28(struct Ball *ball, struct BoatPlayer *pl)
{
    struct Ape *ape;

    ape = ball->ape;
    if (pl->unk4C > 0.0) {
        pl->unk4C -= 40.0 / (0.1 * lbl_80198B34);
        if (pl->unk4C < 0.0) {
            pl->unk4C = 0.0f;
        }
    }
    fn_1_11355C(ape, 1.0f);
    if (ape->unk68 == 18) {
        if (ape->unk70 != 6) {
            fn_1_113190(ape, 0x12, 0.0f, 0, 6);
        } else {
            if ((ape->flags & 0x2000) != 0) {
                s16 anims[4] = {8, 6, 4, 2};

                fn_1_113190(ape, 0, 0.0f, 7, anims[pl->unk2 - 1]);
            }
            if ((pl->unk8 & 0x4000) == 0) {
                if (fn_1_1135E8(ape) >= 23.0) {
                    pl->unk8 |= 0x4000;
                    PSVECScale(&ball->vel, &pl->unk39C, 0.1f);
                    pl->unk39C.y += 15.0 * ball->accel;
                }
            }
        }
    } else if (ape->unk68 == 0) {
        if (ape->unk6C == 2) {
            fn_1_113190(ape, 0x12, 0.0f, 0, 6);
        }
    }
}

void fn_14_10CE4(struct Ball *ball, struct BoatPlayer *pl)
{
    struct Ape *ape;

    ape = ball->ape;
    if (pl->unk4C > 0.0) {
        pl->unk4C -= 40.0 / (0.1 * lbl_80198B34);
        if (pl->unk4C < 0.0) {
            pl->unk4C = 0.0f;
        }
    }
    fn_1_11355C(ape, 1.0f);
    if ((pl->unk8 & 0x4000) == 0) {
        pl->unk8 |= 0x4000;
        pl->unk39C = ball->vel;
    }
    fn_1_113190(ape, 0, 0.0f, 8, 1);
}

void fn_14_10DE0(struct Ball *ball, struct BoatPlayer *pl)
{
    struct Ape *ape;

    ape = ball->ape;
    pl->unk44 = 0.9f;
    pl->unk3C = 2;
    fn_1_11355C(ape, pl->unk44);
    fn_1_113190(ape, 0x12, 0.0f, 0, pl->unk3C);
}

void fn_14_10E58(struct Ball *ball, struct BoatPlayer *pl)
{
    struct Ape *ape;
    s32 next;
    float accel;

    ape = ball->ape;
    next = -1;
    accel = 0.0f;
    switch (pl->unk3C) {
    case 0:
        if (pl->unk40 == -1) {
            pl->unk44 = 1.4f;
            if (pl->unk1E[0] > 0 && pl->unk14 > pl->unk19) {
                pl->unk40 = 1;
            } else if (pl->unk26[0] > 0 && pl->unk14 < pl->unk19) {
                pl->unk40 = 2;
            }
        }
        if (pl->unk4C > 3.0 && pl->unk40 >= 0) {
            next = pl->unk40;
            pl->unk40 = -1;
        }
        break;
    case 1:
        if (pl->unk26[0] > 0) {
            pl->unk40 = 2;
        }
        if (pl->unk14 == 0x8C) {
            pl->unk44 = 1.6f;
        } else {
            accel = lbl_1_data_657E4 * (float)__abs(pl->unk1E[0]);
        }
        if (pl->unk4C >= 20.0 && pl->unk40 >= 0) {
            next = pl->unk40;
            pl->unk40 = -1;
        } else if (pl->unk4C >= 35.0) {
            if (pl->unk38 > 0x12C) {
                pl->unk40 = 1;
                next = 0;
            } else if (pl->unk18 >= 3 || pl->unk1E[0] < 0) {
                next = 0;
            }
        }
        break;
    case 2:
        if (pl->unk1E[0] > 0) {
            pl->unk40 = 1;
        }
        if (pl->unk19 == 0x8C) {
            pl->unk44 = 1.6f;
        } else {
            accel = lbl_1_data_657E4 * (float)__abs(pl->unk26[0]);
        }
        if (pl->unk4C >= 20.0 && pl->unk40 >= 0) {
            next = pl->unk40;
            pl->unk40 = -1;
        } else if (pl->unk4C >= 35.0) {
            if (pl->unk3A > 0x12C) {
                pl->unk40 = 2;
                next = 0;
            } else if (pl->unk1D >= 3 || pl->unk26[0] < 0) {
                next = 0;
            }
        }
        break;
    default:
        next = 0;
        break;
    }
    if ((dip_switches & 1) != 0 && (lbl_1_bss_D4328 & 0x40) != 0) {
        next = 0;
    }
    if (next >= 0 && pl->unk3C != next) {
        pl->unk4C = 0.0f;
        fn_1_113190(ape, 0x12, 0.0f, 0, next);
        pl->unk3C = next;
    } else {
        pl->unk4C += pl->unk44;
        if (pl->unk4C >= 1000.0f) {
            pl->unk4C = 1000.0f;
        }
    }
    if (pl->unk44 < 1.6) {
        pl->unk44 += accel;
        if (pl->unk44 > 1.6) {
            pl->unk44 = 1.6f;
        }
    }
    fn_1_11355C(ape, pl->unk44);
    if ((pl->unk4A4 & 8) != 0) {
        if (pl->unk4BC[3] > 0.5 * lbl_80198B34) {
            fn_14_113A4(ball, pl);
            return;
        }
        if (ape->unk6C != 0x12) {
            fn_1_113190(ape, 0x12, 0.0f, 0, pl->unk3C);
        }
    }
}

void fn_14_11298(struct Ball *ball, struct BoatPlayer *pl)
{
    struct Ape *ape;

    ape = ball->ape;
    if (pl->unk3C == 2) {
        if ((ape->flags & 0x2000) == 0) {
            return;
        }
        pl->unk3C = -1;
    }
    if ((pl->unk4A4 & 8) != 0 && pl->unk4BC[3] > 0.5 * lbl_80198B34) {
        fn_14_113A4(ball, pl);
    } else if (0.0f < pl->unk308 && pl->unk308 < pl->unk3AC) {
        fn_1_113190(ape, 0x12, 0.0f, 0, 3);
    } else {
        fn_1_113190(ape, 0x12, 0.0f, 0, 0);
    }
    fn_1_11355C(ape, 1.0f);
}

void fn_14_113A4(struct Ball *ball, struct BoatPlayer *pl)
{
    float speeds[12] = {0.0f, 1.0f, 2.5f, 4.0f, 5.5f, 7.0f, 8.4f, 10.0f, 13.0f, 16.0f, 30.0f, 65.0f};
    float speed;
    s32 anim;
    s16 i;

    speed = 216.0f * mathutil_vec_len(&ball->vel);
    anim = 0;
    if (speed >= 100.0) {
        if (speed > 160.0) {
            speed = 160.0f;
        }
        anim = 0xC;
        fn_1_11355C(ball->ape, 0.65 + 0.35 * ((speed - 100.0) / 60.0));
    } else {
        for (i = 0xB; i >= 0; i--) {
            if (speeds[i] <= speed) {
                anim = i;
                break;
            }
        }
        fn_1_11355C(ball->ape, 1.0f);
    }
    fn_1_113190(ball->ape, 0, 0.0f, 2, anim);
}

