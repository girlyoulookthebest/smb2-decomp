#include "race2.h"
#include "item.h"
#include "libc.h"

/* race2 extension of an item, stored in its stageBanana field */
struct Race2Item
{
    /*0x00*/ s32 unk0;
    /*0x04*/ s32 unk4;
    /*0x08*/ s16 unk8;
    /*0x0A*/ s16 unkA;
    /*0x0C*/ s16 unkC;
    /*0x0E*/ s16 unkE;
    /*0x10*/ float unk10;
};  /* size = 0x14 */

struct Race2ItemModel
{
    /*0x00*/ s16 modelId;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ s32 unkC;
};  /* size = 0x10 */

extern struct Race2Item lbl_10_bss_2D88[256];
extern s16 lbl_10_bss_2D3C;
extern s16 player_count;
extern float lbl_1_data_65758;
extern float lbl_1_data_6575C;
extern float lbl_1_data_65760;

static u32 lbl_10_bss_11D8;
static Vec lbl_10_bss_11DC[3];
static s16 lbl_10_bss_1200[3];

void fn_10_107E0(void);
void fn_10_10CA0(struct Item *arg0);
void fn_10_10CF4(struct Item *arg0);
void fn_10_10DDC(struct Item *arg0);
void fn_10_110D0(struct Item *arg0, struct PhysicsBall *arg1);
void fn_10_11124(struct Item *arg0);
void fn_10_11170(struct Item *arg0);
void fn_10_11174(struct Item *arg0);
u16 fn_10_188C0(void);
float fn_10_18900(void);
s8 fn_1_2F8F0(s32 arg0, Vec *arg1, u8 *arg2, u8 *arg3);
s16 g_math_unk3(float arg0);
void ray_scale(Vec *a, Vec *b, Vec *dst, float t);
void call_SoundReqID_arg_0(u32 id);

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

static inline void OSf32tos16(register float *in, register volatile s16 *out)
{
#ifdef __MWERKS__
    asm {
        lfs fp1, 0(in)
        psq_st fp1, 0(out), 1, 5
    }
#else
    { float v = *in; *out = (s16)(v > 32767.0f ? 32767 : v < -32768.0f ? -32768 : (int)v); }
#endif
}

void fn_10_11178(struct Item *arg0);
void fn_10_11A6C(struct Item *arg0);
void fn_10_11F88(struct Item *arg0);
void fn_10_12438(struct Item *arg0);
void fn_10_1256C(struct Item *arg0);
void fn_10_12690(struct Item *arg0);
void fn_10_127C4(struct Item *arg0);
void fn_10_1125C(struct Item *arg0);
void fn_10_11B50(struct Item *arg0);
void fn_10_12064(struct Item *arg0);
void fn_10_12528(struct Item *arg0);
void fn_10_1264C(struct Item *arg0);
void fn_10_12780(struct Item *arg0);
void fn_10_128B4(struct Item *arg0);
void fn_10_116DC(struct Item *arg0, struct PhysicsBall *arg1);
void fn_10_11F68(struct Item *arg0, struct PhysicsBall *arg1);
void fn_10_1238C(struct Item *arg0, struct PhysicsBall *arg1);
void fn_10_12548(struct Item *arg0, struct PhysicsBall *arg1);
void fn_10_1266C(struct Item *arg0, struct PhysicsBall *arg1);
void fn_10_127A0(struct Item *arg0, struct PhysicsBall *arg1);
void fn_10_128D4(struct Item *arg0, struct PhysicsBall *arg1);
void fn_10_130D8(struct Item *arg0);
void fn_10_133C8(struct Item *arg0, u8 arg1);
void fn_10_13510(struct Item *arg0);
void fn_10_13710(struct Item *arg0);
void fn_10_13814(Vec *arg0, s16 arg1);

void (*lbl_10_data_F98[])(struct Item *) = {
    fn_10_11178, fn_10_11A6C, fn_10_11F88, fn_10_12438, fn_10_1256C, fn_10_12690, fn_10_127C4,
};
void (*lbl_10_data_FB4[])(struct Item *) = {
    fn_10_1125C, fn_10_11B50, fn_10_12064, fn_10_12528, fn_10_1264C, fn_10_12780, fn_10_128B4,
};
void (*lbl_10_data_FD0[])(struct Item *, struct PhysicsBall *) = {
    fn_10_116DC, fn_10_11F68, fn_10_1238C, fn_10_12548, fn_10_1266C, fn_10_127A0, fn_10_128D4,
};
u16 lbl_10_data_FEC[] = { 0x0004, 0x0010, 0x0008, 0x0002, 0x0020, 0x0080 };

void fn_10_10764(void)
{
    item_replace_type_funcs(2, &(struct ItemFuncs){
        fn_10_10CA0,
        fn_10_10CF4,
        fn_10_10DDC,
        fn_10_110D0,
        fn_10_11124,
        fn_10_11170,
        fn_10_11174,
    });
    g_item_tick_callback = fn_10_107E0;
}

void fn_10_107E0(void)
{
    Vec sp74;
    Vec sp68;
    Vec sp5C;
    Vec sp50;
    Vec sp44;
    Vec sp38;
    float tf0;
    struct Item *tr30;
    struct Item *tr29;
    s16 r28;
    s16 r27;

    for (r28 = 0; r28 < g_poolInfo.itemPool.count - 1; r28++) {
        if (g_poolInfo.itemPool.statusList[r28] != 0) {
            tr30 = &items[r28];
            if (tr30->subType < 2) {
                for (r27 = r28 + 1; r27 < g_poolInfo.itemPool.count; r27++) {
                    if (g_poolInfo.itemPool.statusList[r27] != 0) {
                        tr29 = &items[r27];
                        if (tr29->subType < 2) {
                            if (!(mathutil_vec_sq_distance(&tr30->pos, &tr29->pos) > (tr30->unk14 + tr29->unk14) * (tr30->unk14 + tr29->unk14))) {
                                ray_scale(&tr30->pos, &tr29->pos, &sp74, 0.5f);
                                PSVECSubtract(&tr30->pos, &sp74, &sp68);
                                vec_normalize_len(&sp68);
                                PSVECScale(&sp68, &sp38, tr30->unk14);
                                PSVECAdd(&sp74, &sp38, &tr30->pos);
                                tf0 = mathutil_vec_dot_prod(&sp68, &tr30->vel);
                                if (tf0 < 0.0f) {
                                    PSVECScale(&sp68, &sp38, -tf0);
                                    PSVECAdd(&tr30->vel, &sp38, &tr30->vel);
                                    PSVECScale(&sp38, &sp50, -0.3f);
                                } else {
                                    sp50 = (Vec){0.0f, 0.0f, 0.0f};
                                }
                                PSVECSubtract(&tr29->pos, &sp74, &sp5C);
                                vec_normalize_len(&sp5C);
                                PSVECScale(&sp5C, &sp38, tr29->unk14);
                                PSVECAdd(&sp74, &sp38, &tr29->pos);
                                tf0 = mathutil_vec_dot_prod(&sp5C, &tr29->vel);
                                if (tf0 < 0.0f) {
                                    PSVECScale(&sp5C, &sp38, -tf0);
                                    PSVECAdd(&tr29->vel, &sp38, &tr29->vel);
                                    PSVECScale(&sp38, &sp44, -0.3f);
                                } else {
                                    sp44 = (Vec){0.0f, 0.0f, 0.0f};
                                }
                                PSVECAdd(&tr30->vel, &sp44, &tr30->vel);
                                PSVECAdd(&tr29->vel, &sp50, &tr29->vel);
                                if (tr30->subType == 1) {
                                    tr30->pos = tr30->prevPos;
                                    tr30->vel = (Vec){0.0f, 0.0f, 0.0f};
                                }
                                if (tr29->subType == 1) {
                                    tr29->pos = tr29->prevPos;
                                    tr29->vel = (Vec){0.0f, 0.0f, 0.0f};
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

struct Race2Item *fn_10_10BDC(void)
{
    s16 i;
    struct Race2Item *r31;

    r31 = NULL;
    for (i = 0; i < 0x100; i++) {
        if (lbl_10_bss_2D88[i].unk0 == 0) {
            r31 = &lbl_10_bss_2D88[i];
            break;
        }
    }
    if ((u32)(dip_switches & 1) != 0 && r31 == NULL && (u32)(g_some_other_flags & 4) != 0) {
        printf("ITEM OVER!");
    }
    return r31;
}

struct Race2ItemModel lbl_10_data_1004[] = {
    { 0x45, 0.2f, 1.5f, 0 },
    { -1, 0.0f, 0.0f, 0 },
};
struct Race2ItemModel lbl_10_data_1024[] = {
    { 0x3C, -3.0f, 3.0f, 0 },
    { -1, 0.0f, 0.0f, 0 },
};

void fn_10_10CA0(struct Item *arg0)
{
    if (arg0->subType >= 0 && arg0->subType < 7) {
        lbl_10_data_F98[arg0->subType](arg0);
    }
}

void fn_10_10CF4(struct Item *arg0)
{
    struct Race2Item *tr4_2;

    if (arg0->subType >= 0 && arg0->subType < 7) {
        arg0->prevPos = arg0->pos;
        arg0->prevRotX = arg0->rotX;
        arg0->prevRotY = arg0->rotY;
        arg0->prevRotZ = arg0->rotZ;
        lbl_10_data_FB4[arg0->subType](arg0);
        if (arg0->stageBanana != NULL) {
            tr4_2 = (struct Race2Item *)arg0->stageBanana;
            if (tr4_2->unkC > 0) {
                tr4_2->unkC--;
                if (tr4_2->unkC == 0) {
                    tr4_2->unk8 = -1;
                }
            }
            if (tr4_2->unkE > 0) {
                tr4_2->unkE--;
            }
        }
    }
}

void fn_10_10DDC(struct Item *arg0)
{
    Vec sp14;
    float tf31;
    float tf30;
    float tf3;
    float f29;
    s16 id;
    struct Race2ItemModel *r30;
    struct GMAModel *tr29;

    r30 = arg0->modelLODs;
    tf31 = arg0->unk14;
    f29 = -1.0f;
    switch (arg0->subType) {
    case 0:
        if (arg0->unk12 < 0x3C && (arg0->unk12 & 8) != 0) {
            return;
        }
        break;
    }
    for (; (id = r30->modelId) >= 0; r30++) {
        tr29 = minigameGma->modelEntries[id].model;
        sp14 = (Vec){arg0->pos.x, arg0->pos.y + r30->unk4 * (tf31 / r30->unk8), arg0->pos.z};
        mtxa_from_mtxb_translate(&sp14);
        if (r30->unkC != 0) {
            mtxa_sq_from_identity();
        } else {
            mtxa_rotate_y(arg0->rotY);
            mtxa_rotate_x(arg0->rotX);
            mtxa_rotate_z(arg0->rotZ);
        }
        tf30 = tf31 / tr29->boundSphereRadius;
        if (1.0f != tf30) {
            mtxa_scale_xyz(tf30, tf30, tf30);
        }
        if (f29 < 0.0) {
            if (g_is_sphere_visible_scaled(&tr29->boundSphereCenter, tr29->boundSphereRadius, tf30) == 0) {
                return;
            }
            mathutil_mtxA_get_translate(&sp14);
            tf3 = -sp14.z;
            switch (arg0->subType) {
            case 0:
            case 1:
                if (tf3 > 60.0) {
                    return;
                }
                if (tf3 > 50.0) {
                    f29 = 1.0 - (tf3 - 50.0) / 10.0;
                } else {
                    f29 = (0.1f + (tf3 + tf31)) / tf31;
                }
                break;
            case 2:
                if (tf3 > 60.0) {
                    return;
                }
                if (tf3 > 50.0) {
                    f29 = 1.0 - (tf3 - 50.0) / 10.0;
                } else {
                    f29 = 1.0f;
                }
                break;
            default:
                f29 = (0.1f + (tf3 + tf31)) / tf31;
                break;
            }
            if (f29 <= 0.0) {
                return;
            }
            if (f29 > 1.0) {
                f29 = 1.0f;
            }
        }
        avdisp_set_alpha(f29);
        avdisp_set_scale_factor(tf30);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(tr29);
    }
}

void fn_10_110D0(struct Item *arg0, struct PhysicsBall *arg1)
{
    if (arg0->subType >= 0 && arg0->subType < 7) {
        lbl_10_data_FD0[arg0->subType](arg0, arg1);
    }
}

void fn_10_11124(struct Item *arg0)
{
    struct Race2Item *tr4;

    if (arg0->stageBanana != NULL) {
        tr4 = (struct Race2Item *)arg0->stageBanana;
        tr4->unk0 = 0;
        tr4->unk4 = 0;
        tr4->unk8 = -1;
        tr4->unkA = -1;
        tr4->unkC = 0;
        tr4->unkE = 0;
        arg0->stageBanana = NULL;
    }
}

void fn_10_11170(struct Item *arg0)
{
}

void fn_10_11174(struct Item *arg0)
{
}

void fn_10_11178(struct Item *arg0)
{
    arg0->modelLODs = lbl_10_data_1004;
    arg0->flags |= 0x22;
    arg0->unk14 = 0.05f;
    arg0->unk18 = 0.25f;
    arg0->shadowModel = init_common_gma->modelEntries[0x67].model;
    arg0->shadowColor = (GXColor){0x46, 0x47, 0x5F, 0x00};
    arg0->unk7C = (Vec){arg0->unk14, 0.8f * arg0->unk14, arg0->unk14};
    arg0->rotVelY = 0x300;
    arg0->unk12 = (s16) (lbl_80198B38 * 0xA);
    arg0->state = 0;
}



void fn_10_1125C(struct Item *arg0)
{
    Vec spC;
    s8 sp9;
    s8 sp8;
    s8 sound;
    float tf0;
    struct Race2Item *tr30;
    Vec *tr29;

    tr30 = (struct Race2Item *)arg0->stageBanana;
    switch (arg0->state) {
    case 0:
        arg0->state = 1;
        /* fallthrough */
    case 1:
        arg0->vel.y -= 3.6 * (9.8 / (lbl_80198B34 * lbl_80198B34));
        arg0->unk14 += 0.05f * (0.5f - arg0->unk14);
        if (arg0->unk12 <= 0) {
            g_poolInfo.itemPool.statusList[arg0->id] = 3;
        }
        break;
    case 2:
        arg0->vel.y += 3.6 * (9.8 / (lbl_80198B34 * lbl_80198B34));
        arg0->unk14 *= 1.0f - 1.0f / (float)(arg0->unk12 + 1);
        break;
    case 3:
    case 4:
        arg0->state = 5;
        arg0->unk10 = 0.25 * lbl_80198B38;
        /* fallthrough */
    case 5:
        if (--arg0->unk10 <= 0) {
            arg0->state = 6;
        }
        break;
    case 6:
        arg0->unk14 -= 0.5 / (0.25 * lbl_80198B34);
        if (arg0->unk14 < 1.1920929e-7f) {
            g_poolInfo.itemPool.statusList[arg0->id] = 3;
        }
        break;
    }
    PSVECScale(&arg0->vel, &arg0->vel, 0.995f);
    PSVECAdd(&arg0->pos, &arg0->vel, &arg0->pos);
    if (tr30->unkA >= 0) {
        arg0->pos.x = balls[tr30->unkA].pos.x;
        arg0->pos.z = balls[tr30->unkA].pos.z;
    }
    arg0->rotX += arg0->rotVelX;
    arg0->rotY += arg0->rotVelY;
    arg0->rotZ += arg0->rotVelZ;
    if ((u32)(arg0->flags & 0x40) != 0 && arg0->pos.y - arg0->hit.pos.y < arg0->unk14) {
        tr29 = &balls[tr30->unk8].pos;
        arg0->pos.y = arg0->hit.pos.y + arg0->unk14;
        PSVECSubtract(&arg0->pos, &arg0->prevPos, &arg0->vel);
        if (arg0->vel.y > 0.0f) {
            PSVECScale(&arg0->vel, &arg0->vel, 0.98f);
        }
        if (arg0->vel.y < 0.0f) {
            arg0->vel.y *= -0.4f;
        }
        if (tr30->unk4 == 0) {
            sound = fn_1_2F8F0(modeCtrl.playerCount, &arg0->pos, (u8 *)&sp9, (u8 *)&sp8);
            call_SoundReqID_arg_0(((sp8 & 0x7F) << 0x19) | (((sound - 0x3F) & 0x7F) << 0xB | 0x132 | ((sp9 & 0x7F) << 0x12)));
            tr30->unk4 = 1;
        }
        PSVECSubtract(tr29, &arg0->pos, &spC);
        vec_normalize_len(&spC);
        tf0 = mathutil_vec_dot_prod(&spC, &arg0->vel);
        if (tf0 < 0.0f) {
            PSVECScale(&spC, &spC, -0.2f);
        }
    }
    arg0->unk6C.z = -arg0->rotY;
    arg0->unk7C.x = arg0->unk14;
    arg0->unk7C.y = 0.7f * arg0->unk14;
}

void fn_10_116DC(struct Item *arg0, struct PhysicsBall *arg1)
{
    Vec sp8;
    float f31;
    struct RacePlayer *tr30;
    struct Race2Item *ext;
    s16 r29;
    s32 r28;
    s16 r5;

    tr30 = (struct RacePlayer *)current_ball->unk150;
    ext = (struct Race2Item *)arg0->stageBanana;
    if ((u32)(tr30->flags & 2) != 0) {
        arg0->flags &= 0xFFFFFFFD;
        arg0->state = 4;
        return;
    }
    ext->unkA = tr30->unk0;
    arg0->flags &= 0xFFFFFFFD;
    arg0->state = 3;
    arg0->vel.y = 0.1875 * arg0->unk14;
    arg0->rotVelY *= 4;
    PSVECScale(&arg1->vel, &sp8, 0.25f);
    PSVECAdd(&arg0->vel, &sp8, &arg0->vel);
    r29 = 2;
    if (tr30->unk2C8[2] != 0) {
        for (r5 = 1; r5 < 3; r5++) {
            tr30->unk2C8[r5 - 1] = tr30->unk2C8[r5];
        }
    } else {
        for (r5 = 0; r5 < 3; r5++) {
            if (tr30->unk2C8[r5] == 0) {
                r29 = r5;
                break;
            }
        }
    }
    r28 = 0;
    if ((u32)(tr30->flags & 1) == 0 && tr30->unk3E >= 2) {
        f31 = 0.0f;
        for (r5 = 0; r5 < tr30->unk3E; r5++) {
            f31 += lbl_10_bss_2D68[r5]->unk200 - tr30->unk200;
        }
        f31 /= (float)tr30->unk3E;
        if (fn_10_18900() < f31 / lbl_1_data_65760) {
            r28 = 1;
        }
    }
    if (r28 != 0) {
        if ((current_ball->flags & (1ULL << 43)) != 0) {
            tr30->unk2C8[r29] = (fn_10_188C0() & 7) != 0 ? 1 : 0x40;
        } else {
            tr30->unk2C8[r29] = (fn_10_188C0() & 3) != 0 ? 1 : 0x40;
        }
    } else {
        tr30->unk2C8[r29] = lbl_10_data_FEC[fn_10_188C0() % 6U];
    }
    tr30->flags |= 4;
    tr30->unk2BA = lbl_80198B38;
    if ((current_ball->flags & (1ULL << 43)) == 0) {
        call_SoundReqID_arg_0(0x39);
        call_SoundReqID_arg_0(0x20);
    }
}

void fn_10_11A6C(struct Item *arg0)
{
    fn_10_11178(arg0);
}

void fn_10_11B50(struct Item *arg0)
{
    s8 sp9;
    s8 sp8;
    s8 sound;
    struct Race2Item *tr30;

    tr30 = (struct Race2Item *)arg0->stageBanana;
    arg0->unk12 = lbl_80198B38;
    switch (arg0->state) {
    case 0:
        arg0->state = 1;
        /* fallthrough */
    case 1:
        arg0->vel.y -= 3.6 * (9.8 / (lbl_80198B34 * lbl_80198B34));
        arg0->unk14 += 0.05f * (0.5f - arg0->unk14);
        break;
    case 2:
        arg0->vel.y += 3.6 * (9.8 / (lbl_80198B34 * lbl_80198B34));
        arg0->unk14 *= 1.0f - 1.0f / (float)(arg0->unk12 + 1);
        break;
    case 3:
    case 4:
        lbl_10_bss_11D8 &= ~(u16)(1 << tr30->unk8);
        arg0->state = 5;
        arg0->unk10 = 0.25 * lbl_80198B38;
        /* fallthrough */
    case 5:
        if (--arg0->unk10 <= 0) {
            arg0->state = 6;
        }
        break;
    case 6:
        arg0->unk14 -= 0.5 / (0.25 * lbl_80198B34);
        if (arg0->unk14 < 1.1920929e-7f) {
            g_poolInfo.itemPool.statusList[arg0->id] = 3;
        }
        break;
    }
    arg0->vel.y *= 0.995;
    arg0->pos.y += arg0->vel.y;
    if (tr30->unkA >= 0) {
        arg0->pos.x = balls[tr30->unkA].pos.x;
        arg0->pos.z = balls[tr30->unkA].pos.z;
    }
    arg0->rotX += arg0->rotVelX;
    arg0->rotY += arg0->rotVelY;
    arg0->rotZ += arg0->rotVelZ;
    if ((u32)(arg0->flags & 0x40) != 0 && arg0->pos.y - arg0->hit.pos.y < arg0->unk14) {
        arg0->pos.y = arg0->hit.pos.y + arg0->unk14;
        arg0->vel.y = arg0->pos.y - arg0->prevPos.y;
        if (arg0->vel.y > 0.0f) {
            arg0->vel.y *= 0.98;
        } else {
            arg0->vel.y *= -0.4f;
        }
        if (tr30->unk4 == 0) {
            sound = fn_1_2F8F0(modeCtrl.playerCount, &arg0->pos, (u8 *)&sp9, (u8 *)&sp8);
            call_SoundReqID_arg_0(((sp8 & 0x7F) << 0x19) | (((sound - 0x3F) & 0x7F) << 0xB | 0x132 | ((sp9 & 0x7F) << 0x12)));
            tr30->unk4 = 1;
        }
    }
    arg0->unk6C.z = -arg0->rotY;
    arg0->unk7C.x = arg0->unk14;
    arg0->unk7C.y = 0.7f * arg0->unk14;
}

void fn_10_11F68(struct Item *arg0, struct PhysicsBall *arg1)
{
    fn_10_116DC(arg0, arg1);
}

void fn_10_11F88(struct Item *arg0)
{
    arg0->modelLODs = lbl_10_data_1024;
    arg0->flags |= 0x22;
    arg0->unk14 = 0.5f;
    arg0->unk18 = 0.25f;
    arg0->shadowModel = init_common_gma->modelEntries[0x67].model;
    arg0->shadowColor = (GXColor){0x46, 0x47, 0x5F, 0x00};
    arg0->unk7C = (Vec){arg0->unk14, 0.8f * arg0->unk14, arg0->unk14};
    arg0->unk12 = (s16) (lbl_80198B38 * 0x78);
    arg0->state = 0;
}

void fn_10_12064(struct Item *arg0)
{
    struct Race2ItemModel *tr3;

    switch (arg0->state) {
    case 0:
        arg0->state = 1;
        /* fallthrough */
    case 1:
        arg0->vel.y -= 3.0 * (3.6 * (9.8 / (lbl_80198B34 * lbl_80198B34)));
        if (arg0->unk12 < 5) {
            arg0->state = 2;
            arg0->flags &= 0xFFFFFFFD;
        } else {
            tr3 = arg0->modelLODs;
            arg0->unk14 += 0.01f * tr3->unk8;
            if (arg0->unk14 > tr3->unk8) {
                arg0->unk14 = tr3->unk8;
            }
        }
        break;
    case 2:
        arg0->unk14 *= 1.0f - 1.0f / (float)(arg0->unk12 + 1);
        break;
    case 4:
        arg0->state = 5;
        arg0->unk10 = 0.25 * lbl_80198B38;
        /* fallthrough */
    case 5:
        if (--arg0->unk10 <= 0) {
            arg0->state = 6;
        }
        break;
    case 6:
        arg0->unk14 -= 0.5 / (0.25 * lbl_80198B34);
        if (arg0->unk14 < 1.1920929e-7f) {
            g_poolInfo.itemPool.statusList[arg0->id] = 3;
        }
        break;
    case 3:
        g_poolInfo.itemPool.statusList[arg0->id] = 3;
        break;
    }
    PSVECScale(&arg0->vel, &arg0->vel, 0.94525f);
    PSVECAdd(&arg0->pos, &arg0->vel, &arg0->pos);
    if ((u32)(arg0->flags & 0x40) != 0 && arg0->pos.y - arg0->hit.pos.y < arg0->unk14) {
        arg0->pos.y = arg0->hit.pos.y + arg0->unk14;
        if (arg0->vel.y < 0.0f) {
            arg0->vel.y *= -0.8f;
        }
    }
    if (arg0->subType == 5) {
        arg0->rotY = math_atan2(arg0->vel.x, arg0->vel.z) + 0x8000;
    } else {
        arg0->rotX += arg0->rotVelX;
        arg0->rotY += arg0->rotVelY;
        arg0->rotZ += arg0->rotVelZ;
    }
    arg0->unk6C.z = -arg0->rotY;
    arg0->unk7C.x = 0.4f * arg0->unk14;
    arg0->unk7C.y = 0.4f * arg0->unk14;
}

struct Race2ItemModel lbl_10_data_1060[] = {
    { 0x44, 0.0f, 1.0f, 0 },
    { -1, 0.0f, 0.0f, 0 },
};
struct Race2ItemModel lbl_10_data_1080[] = {
    { 0x8C, 0.0f, 1.0f, 0 },
    { -1, 0.0f, 0.0f, 0 },
};
struct Race2ItemModel lbl_10_data_10A0[] = {
    { 0x8D, 0.0f, 1.0f, 0 },
    { -1, 0.0f, 0.0f, 0 },
};
struct Race2ItemModel lbl_10_data_10C0[] = {
    { 0x26, 0.0f, 1.0f, 0 },
    { -1, 0.0f, 0.0f, 0 },
};
u8 lbl_10_data_10E0[] = { 0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 3, 4, 5, 6, 7, 4, 5, 6, 7 };

void fn_10_1238C(struct Item *arg0, struct PhysicsBall *arg1)
{
    struct RacePlayer *tr6;
    struct Race2Item *tr4;

    tr6 = (struct RacePlayer *)current_ball->unk150;
    tr4 = (struct Race2Item *)arg0->stageBanana;
    if (tr4 != NULL) {
        if (tr4->unk8 == current_ball->playerId || tr6->unkA > 0 || (u32)(tr6->flags & 0x20000) != 0 || (u32)(tr6->flags & 0x10) != 0 || (u32)(tr6->flags & 2) != 0) {
            arg0->unkC = 0;
            return;
        }
        arg0->flags &= 0xFFFFFFFD;
        arg0->state = 3;
        tr6->unk2C6 |= 2;
    }
}

void fn_10_12438(struct Item *arg0)
{
    arg0->modelLODs = lbl_10_data_1060;
    arg0->flags |= 0x22;
    arg0->unk14 = 0.5f;
    arg0->unk18 = 0.25f;
    arg0->shadowModel = init_common_gma->modelEntries[0x67].model;
    arg0->shadowColor = (GXColor){0x46, 0x47, 0x5F, 0x00};
    arg0->unk7C = (Vec){arg0->unk14, 0.8f * arg0->unk14, arg0->unk14};
    arg0->rotVelX = 0x300;
    arg0->rotVelY = 0x300;
    arg0->rotVelZ = 0x300;
    arg0->unk12 = (s16) lbl_80198B38;
    arg0->state = 0;
}

void fn_10_12528(struct Item *arg0)
{
    fn_10_130D8(arg0);
}

void fn_10_12548(struct Item *arg0, struct PhysicsBall *arg1)
{
    fn_10_133C8(arg0, 0x10);
}

void fn_10_1256C(struct Item *arg0)
{
    arg0->modelLODs = lbl_10_data_1080;
    arg0->flags |= 0x22;
    arg0->unk14 = 0.5f;
    arg0->unk18 = 0.25f;
    arg0->shadowModel = init_common_gma->modelEntries[0x67].model;
    arg0->shadowColor = (GXColor){0x46, 0x47, 0x5F, 0x00};
    arg0->unk7C = (Vec){arg0->unk14, 0.8f * arg0->unk14, arg0->unk14};
    arg0->rotVelY = 0x300;
    arg0->unk12 = (s16) lbl_80198B38;
    arg0->state = 0;
}

void fn_10_1264C(struct Item *arg0)
{
    fn_10_130D8(arg0);
}

void fn_10_1266C(struct Item *arg0, struct PhysicsBall *arg1)
{
    fn_10_133C8(arg0, 4);
}

void fn_10_12690(struct Item *arg0)
{
    arg0->modelLODs = lbl_10_data_10A0;
    arg0->flags |= 0x22;
    arg0->unk14 = 0.5f;
    arg0->unk18 = 0.25f;
    arg0->shadowModel = init_common_gma->modelEntries[0x67].model;
    arg0->shadowColor = (GXColor){0x46, 0x47, 0x5F, 0x00};
    arg0->unk7C = (Vec){arg0->unk14, 0.8f * arg0->unk14, arg0->unk14};
    arg0->rotVelX = 0x300;
    arg0->rotVelY = 0x300;
    arg0->rotVelZ = 0x300;
    arg0->unk12 = (s16) lbl_80198B38;
    arg0->state = 0;
}

void fn_10_12780(struct Item *arg0)
{
    fn_10_130D8(arg0);
}

void fn_10_127A0(struct Item *arg0, struct PhysicsBall *arg1)
{
    fn_10_133C8(arg0, 8);
}

void fn_10_127C4(struct Item *arg0)
{
    arg0->modelLODs = lbl_10_data_10C0;
    arg0->flags |= 0x22;
    arg0->unk14 = 0.5f;
    arg0->unk18 = 0.25f;
    arg0->shadowModel = init_common_gma->modelEntries[0x67].model;
    arg0->shadowColor = (GXColor){0x46, 0x47, 0x5F, 0x00};
    arg0->unk7C = (Vec){arg0->unk14, 0.8f * arg0->unk14, arg0->unk14};
    arg0->rotVelX = 0x300;
    arg0->rotVelY = 0x300;
    arg0->rotVelZ = 0x300;
    arg0->unk12 = (s16) lbl_80198B38;
    arg0->state = 0;
}

void fn_10_128B4(struct Item *arg0)
{
    fn_10_130D8(arg0);
}

void fn_10_128D4(struct Item *arg0, struct PhysicsBall *arg1)
{
    fn_10_133C8(arg0, 0x80);
}

void fn_10_128F8(void)
{
    struct Item item;
    float f31;
    float tf30;
    float tf29;
    float f1;
    struct Race2Item *r30;
    struct Race2Course *course = &lbl_10_bss_1208;
    struct Race2CourseInfo *tr28;
    struct StageExtraData *tr27;
    struct Ball *tr26;
    struct RacePlayer *tr25;
    s16 tr24;

    tr28 = &lbl_10_data_2ADC[course->unk0];
    tr27 = stagedef->extraData;
    if (lbl_10_bss_2D3C <= 0) {
        r30 = fn_10_10BDC();
        if (r30 != NULL) {
            tr24 = lbl_10_data_10E0[fn_10_188C0() % 30U];
            if (tr24 >= player_count) {
                lbl_10_bss_2D3C = 0;
                return;
            }
            tr26 = &balls[lbl_10_bss_2D68[tr24]->unk0];
            tr25 = (struct RacePlayer *)tr26->unk150;
            if ((u32)(tr25->flags & 2) != 0) {
                lbl_10_bss_2D3C = 0xA;
                return;
            }
            memset(&item, 0, sizeof(item));
            item.type = 2;
            item.subType = 0;
            item.animGroupId = 0;
            r30->unk0 = 1;
            r30->unk4 = 0;
            r30->unk8 = lbl_10_bss_2D68[tr24]->unk0;
            r30->unkA = -1;
            r30->unkC = -1;
            item.stageBanana = (struct StageBanana *)r30;
            f1 = lbl_1_data_65758 * math_sqrt(mathutil_sum_of_sq_3(tr26->vel.x, 0.0f, tr26->vel.z)) / course->unk8;
            f1 *= 100.0f;
            f31 = 0.001f;
            if (tr25->unkE > lbl_80198B38) {
                f1 = -f1;
                f31 = -f31;
            }
            f1 = f1 + tr25->unk1F0;
            f31 = f31 + f1;
            if (f1 >= 100.0f) {
                f1 -= 100.0f;
            } else if (f1 < 0.0f) {
                f1 += 100.0f;
            }
            if (f31 >= 100.0f) {
                f31 -= 100.0f;
            } else if (f31 < 0.0f) {
                f31 += 100.0f;
            }
            fn_10_181EC(tr27, f1, &item.pos);
            tf30 = tr28->unkC * (fn_10_18900() - 0.5f);
            tf29 = tr28->unkC * (fn_10_18900() - 0.5f);
            item.pos.x += tf30;
            item.pos.z += tf29;
            fn_10_181EC(tr27, f31, &item.vel);
            item.vel.x += tf30;
            item.vel.z += tf29;
            PSVECSubtract(&item.vel, &item.pos, &item.vel);
            item.vel.y = 0.0f;
            vec_set_len(lbl_1_data_6575C, &item.vel, &item.vel);
            item.pos.y += 3.5f;
            spawn_item(&item);
            lbl_10_bss_2D3C = 2.5 * lbl_80198B38;
        }
    }
}

void fn_10_12D34(void)
{
    Vec sp20;
    Vec sp14;
    Vec sp8;
    struct Race2Course *course = &lbl_10_bss_1208;
    struct Race2CourseInfo *tr28;
    struct StageExtraData *tr27;
    s16 r30;

    tr28 = &lbl_10_data_2ADC[course->unk0];
    tr27 = stagedef->extraData;
    lbl_10_bss_11D8 = 0;
    fn_10_181EC(tr27, 10.0f * course->unkC, &sp20);
    fn_10_181EC(tr27, 11.0 * course->unkC, &sp14);
    PSVECSubtract(&sp14, &sp20, &sp14);
    sp14.y = 0.0f;
    vec_set_len(2.0f, &sp14, &sp14);
    mtxa_from_rotate_y(0x4000);
    mtxa_tf_vec(&sp14, &sp14);
    PSVECScale(&sp14, &sp8, 1.0 + tr28->unk10);
    PSVECAdd(&sp20, &sp8, &sp20);
    for (r30 = 0; r30 < 3; r30++) {
        lbl_10_bss_11DC[r30] = sp20;
        lbl_10_bss_1200[r30] = lbl_80198B38 * 0xA;
        PSVECSubtract(&sp20, &sp14, &sp20);
    }
}

void fn_10_12EB8(void)
{
    struct Item item;
    s16 r29;
    struct Race2Item *r30;

    for (r29 = 0; r29 < 3; r29++) {
        if ((u32)(lbl_10_bss_11D8 & (1 << r29)) == 0) {
            if (lbl_10_bss_1200[r29] > 0) {
                lbl_10_bss_1200[r29]--;
            } else {
                r30 = fn_10_10BDC();
                if (r30 != NULL) {
                    lbl_10_bss_11D8 |= 1 << r29;
                    lbl_10_bss_1200[r29] = lbl_80198B38 * 3;
                    memset(&item, 0, sizeof(item));
                    item.type = 2;
                    item.subType = 1;
                    item.animGroupId = 0;
                    item.pos = lbl_10_bss_11DC[r29];
                    item.pos.y += 3.5;
                    r30->unk0 = 1;
                    r30->unk4 = 0;
                    r30->unk8 = r29;
                    r30->unkA = -1;
                    r30->unkC = -1;
                    item.stageBanana = (struct StageBanana *)r30;
                    spawn_item(&item);
                }
            }
        }
    }
}

void fn_10_130D8(struct Item *arg0)
{
    struct Race2ItemModel *tr3;

    switch (arg0->state) {
    case 0:
        arg0->state = 1;
        /* fallthrough */
    case 1:
        if (arg0->unk12 < 5) {
            arg0->state = 2;
            arg0->flags &= 0xFFFFFFFD;
        } else {
            tr3 = arg0->modelLODs;
            arg0->unk14 += 0.1f * tr3->unk8;
            if (arg0->unk14 > tr3->unk8) {
                arg0->unk14 = tr3->unk8;
            }
        }
        break;
    case 2:
        arg0->unk14 *= 1.0f - 1.0f / (float)(arg0->unk12 + 1);
        break;
    case 4:
        arg0->state = 5;
        arg0->unk10 = 0.25 * lbl_80198B38;
        /* fallthrough */
    case 5:
        if (--arg0->unk10 <= 0) {
            arg0->state = 6;
        }
        break;
    case 6:
        arg0->unk14 -= 0.5 / (0.25 * lbl_80198B34);
        if (arg0->unk14 < 1.1920929e-7f) {
            g_poolInfo.itemPool.statusList[arg0->id] = 3;
        }
        break;
    case 3:
        g_poolInfo.itemPool.statusList[arg0->id] = 3;
        break;
    }
    fn_10_13510(arg0);
    fn_10_13710(arg0);
    arg0->vel.y -= 3.6 * (9.8 / (lbl_80198B34 * lbl_80198B34));
    PSVECScale(&arg0->vel, &arg0->vel, 0.995f);
    PSVECAdd(&arg0->pos, &arg0->vel, &arg0->pos);
    if ((u32)(arg0->flags & 0x40) != 0 && arg0->pos.y - arg0->hit.pos.y < 1.2 * arg0->unk14) {
        arg0->pos.y = arg0->hit.pos.y + 1.2 * arg0->unk14;
    }
    arg0->rotX += arg0->rotVelX;
    arg0->rotY += arg0->rotVelY;
    arg0->rotZ += arg0->rotVelZ;
    arg0->unk6C.z = -arg0->rotY;
    arg0->unk7C.x = arg0->unk14;
    arg0->unk7C.y = 0.7f * arg0->unk14;
}

void fn_10_133C8(struct Item *arg0, u8 arg1)
{
    struct RacePlayer *tr6;
    struct Race2Item *tr31;

    tr6 = (struct RacePlayer *)current_ball->unk150;
    tr31 = (struct Race2Item *)arg0->stageBanana;
    if (tr31 != NULL) {
        if (tr31->unk8 == current_ball->playerId || (u32)(tr6->flags & 0x20000) != 0 || (u32)(tr6->flags & 0x10) != 0 || (u32)(tr6->flags & 2) != 0) {
            arg0->unkC = 0;
            return;
        }
        if ((arg1 & 2) == 0 && (tr6->unk2BC & 0x20) != 0) {
            PSVECScale(&arg0->vel, &arg0->vel, -0.95f);
            tr31->unk8 = current_ball->playerId;
            tr31->unkA = -1;
            tr31->unkC = lbl_80198B38 * 3;
            arg0->unkC = 0;
            return;
        }
        arg0->flags &= 0xFFFFFFFD;
        arg0->state = 3;
        tr6->unk2C6 |= arg1;
    }
}

void fn_10_13510(struct Item *arg0)
{
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    float spC;
    s16 sp8;
    float tf31;
    float tf1;
    struct Race2Item *tr30;
    struct Ball *tr3;
    struct RacePlayer *tr4;

    tr30 = (struct Race2Item *)arg0->stageBanana;
    if (tr30 != NULL && tr30->unkA >= 0) {
        tr3 = &balls[tr30->unkA];
        tr4 = (struct RacePlayer *)tr3->unk150;
        if ((tr4->unk2C2 & 0x10) != 0) {
            tr30->unkA = -1;
            return;
        }
        PSVECSubtract(&tr3->pos, &arg0->pos, &sp28);
        tf31 = vec_normalize_len(&sp28);
        sp1C = arg0->vel;
        vec_normalize_len(&sp1C);
        mathutil_vec_cross_prod(&sp1C, &sp28, &sp10);
        tf1 = vec_normalize_len(&sp10);
        sp8 = g_math_unk3(tf1);
        if (sp8 >= 0x2000) {
            tr30->unkA = -1;
            return;
        }
        if (tr30->unkE > 0) {
            if (tf31 < 3.0) {
                tr30->unkA = -1;
            }
            return;
        }
        OSs16tof32(&sp8, &spC);
        spC *= 0.1 * tr30->unk10;
        OSf32tos16(&spC, &sp8);
        if (sp8 > 0x800) {
            sp8 = 0x800;
        }
        if (sp8 != 0) {
            mtxa_from_identity();
            fn_10_13814(&sp10, sp8);
            mtxa_tf_vec(&arg0->vel, &arg0->vel);
        }
        tr30->unk10 *= 0.95;
    }
}

void fn_10_13710(struct Item *arg0)
{
    Vec sp8;
    float tf0;
    float tf30;

    tf0 = mathutil_vec_dot_prod(&arg0->hit.normal, &arg0->vel);
    if (tf0 > 0.0) {
        tf30 = mathutil_vec_len(&arg0->vel);
        PSVECScale(&arg0->hit.normal, &sp8, -tf0 * (6.0 / lbl_80198B34));
        PSVECAdd(&arg0->vel, &sp8, &arg0->vel);
        vec_set_len(tf30, &arg0->vel, &arg0->vel);
    }
}

void fn_10_13814(Vec *arg0, s16 arg1)
{
    Mtx sp8;
    float sinA;
    float cosA;
    float t;
    float x;
    float y;
    float z;
    float yS;
    float zS;
    float xy;
    float yz;
    float zx;

    sinA = math_sin(arg1);
    cosA = math_sin(arg1 + 0x4000);
    t = 1.0f - cosA;
    x = arg0->x;
    y = arg0->y;
    z = arg0->z;
    xy = t * (x * y);
    yz = t * (y * z);
    zx = t * (z * x);
    t = x * sinA;
    yS = y * sinA;
    zS = z * sinA;
    x = x * x;
    y = y * y;
    z = z * z;
    sp8[0][0] = x + (1.0f - x) * cosA;
    sp8[1][0] = xy + zS;
    sp8[2][0] = zx - yS;
    sp8[0][1] = xy - zS;
    sp8[1][1] = y + (1.0f - y) * cosA;
    sp8[2][1] = yz + t;
    sp8[0][2] = zx + yS;
    sp8[1][2] = yz - t;
    sp8[2][2] = z + (1.0f - z) * cosA;
    sp8[0][3] = 0.0f;
    sp8[1][3] = 0.0f;
    sp8[2][3] = 0.0f;
    mtxa_mult_right(sp8);
}

