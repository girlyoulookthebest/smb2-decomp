#include "input.h"
#include "macros.h"
#include "mathutil.h"
#include "baseball.h"

struct BBPoleData
{
    Vec pos;
    float radius;
    float height;
    S16Vec rot;
};

struct BBSwingBat
{
    /*0x00*/ u8 flags;
    /*0x04*/ struct GMAModel * DPTR model;
    /*0x08*/ u8 unk8;
    u8 filler9[0xF];
    /*0x18*/ Mtx mtx;
    /*0x48*/ Mtx poleMtx;
    /*0x78*/ struct BBPoleData data;
    /*0x94*/ Vec pos;
    /*0xA0*/ Vec prevPos;
    /*0xAC*/ float unkAC;
    /*0xB0*/ s16 unkB0;
    /*0xB4*/ float unkB4;
    /*0xB8*/ s16 unkB8;
    /*0xBA*/ S16Vec rot;
    /*0xC0*/ Vec trans;
    u8 fillerCC[0xC];
};

struct BBHelmet
{
    /*0x00*/ u32 flags;
    /*0x04*/ Mtx mtx;
    /*0x34*/ Vec pos;
    /*0x40*/ float unk40;
    /*0x44*/ S16Vec rot;
    /*0x4A*/ S16Vec rot2;
    u8 filler50[0x18];
};

struct BBBillboard
{
    /*0x00*/ struct GMAModel * DPTR model;
    /*0x04*/ Mtx mtx;
    /*0x34*/ Vec pos;
    /*0x40*/ Vec offset;
    u8 filler4C[8];
};

struct BBWheel
{
    /*0x00*/ struct GMAModel * DPTR model;
    /*0x04*/ Mtx mtx;
    /*0x34*/ Vec pos;
    /*0x40*/ Vec offset;
    /*0x4C*/ s16 state;
    /*0x50*/ float speed;
    u8 filler54[4];
    /*0x58*/ s16 angle;
    /*0x5C*/ float decel;
    u8 filler60[2];
    /*0x62*/ u8 unk62;
    u8 filler63[9];
    /*0x6C*/ s32 timer;
};

struct BBRoulette
{
    /*0x000*/ u32 flags;
    /*0x004*/ struct BBBillboard board;
    /*0x058*/ struct BBWheel wheel;
    /*0x0C8*/ struct BBBillboard boards[8];
    /*0x368*/ s8 order[10];
    /*0x372*/ s8 stop;
};

struct BBCoins
{
    /*0x00*/ u8 flags;
    /*0x04*/ struct GMAModel * DPTR model0;
    /*0x08*/ struct GMAModel * DPTR model1;
    u8 fillerC[0x30];
    /*0x3C*/ Vec pos;
    u8 filler48[6];
    /*0x4E*/ s16 angle;
    /*0x50*/ float height;
};

struct BBHelmetGroup
{
    /*0x00*/ u32 flags;
    /*0x04*/ struct GMAModel * DPTR model;
    /*0x08*/ struct BBHelmet items[4];
};

struct BBStage
{
    u8 filler0[4];
    /*0x004*/ struct GMA * DPTR gma;
    u8 filler8[4];
    /*0x00C*/ struct TPL * DPTR tpl;
    /*0x010*/ struct BBSwingBat bat;
    /*0x0E8*/ struct BBHelmetGroup helmets;
    /*0x290*/ struct BBRoulette roulette;
    u8 filler604[0];
};

struct BBWhole
{
    u8 filler0[0x124];
    struct BBHelmet helmets[4];
};

struct BBTrackPair
{
    struct BBTrack *a;
    struct BBTrack *b;
};

extern u8 lbl_18_data_7A118[];
extern u8 lbl_18_data_7A2A0[];
extern u8 lbl_18_data_7EF48[];
extern u8 lbl_18_data_7F0D0[];
extern u8 lbl_18_data_7CC78[];
extern u8 lbl_18_data_7CE00[];
extern u8 lbl_18_data_7BAE8[];
extern u8 lbl_18_data_7BC70[];

#define BB_STAGE ((struct BBStage *) lbl_18_bss_2E0.filler34)
#define BB_BAT ((struct BBSwingBat *) &lbl_18_bss_2E0.unk44)
#define BB_HELMETS ((struct BBHelmetGroup *) &lbl_18_bss_2E0.unk11C)
#define BB_ROULETTE ((struct BBRoulette *) &lbl_18_bss_2E0.unk2C4)
#define BB_BOARD ((struct BBBillboard *) &lbl_18_bss_2E0.unk2C8)
#define BB_WHEEL ((struct BBWheel *) &lbl_18_bss_2E0.unk31C)
#define BB_COINS ((struct BBCoins *) &lbl_18_bss_2E0.unk638)

static inline float bb_eval_track(struct BBKey *keys, s32 n, float t) {
    float res = 0.0f;
    u32 i;

    if (keys == NULL) {
        return res;
    }
    if (t > keys[n - 1].t) {
        return res;
    }
    for (i = 0; i < n; i++) {
        if (t <= keys[i].t) {
            break;
        }
    }
    if (i == 0) {
        i++;
    }
    return fn_18_15BC(t, (float *) &keys[i - 1], (float *) &keys[i]);
}

static inline void bb_coin_bob(void) {
    struct BBCoins *c2 = BB_COINS;

    if (fn_18_334() != 1) {
        c2->angle += 0x400;
        c2->angle &= 0x7FFF;
        c2->height = 0.3 * math_sin(c2->angle);
    }
}

static u32 lbl_18_data_78C10[92] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};

void baseball_21_data_pin(void) {
    lbl_18_data_78C10[0] = 0;
}

void fn_18_134B0(void)
{
    memset(lbl_18_bss_2E0.filler34, 0, 0x658);
    fn_18_135D0();
    fn_18_13664();
}

void fn_18_134EC(void)
{
    struct BBStage *stage = BB_STAGE;
    s32 heap;

    lbl_18_bss_2E0.unk44 &= ~1;
    lbl_18_bss_2E0.unk2C4 &= ~1;
    lbl_18_bss_2E0.unk11C &= ~1;
    lbl_18_bss_2E0.unk638 &= ~1;
    heap = OSSetCurrentHeap(stage_heap);
    if ((u32) stage->tpl != 0) {
        g_something_freeing_heap_2(stage->tpl);
        stage->tpl = NULL;
    }
    if ((u32) stage->gma != 0) {
        g_something_freeing_heap(stage->gma);
        stage->gma = NULL;
    }
    OSSetCurrentHeap(heap);
}

void fn_18_135D0(void)
{
    struct BBStage *stage = BB_STAGE;
    s32 heap;

    fn_18_3C0();
    heap = OSSetCurrentHeap(stage_heap);
    DVDChangeDir("stage");
    stage->tpl = g_load_tpl("bball_obj.tpl");
    stage->gma = g_load_gma("bball_obj.gma", stage->tpl);
    DVDChangeDir("../");
    OSSetCurrentHeap(heap);
}

void fn_18_13664(void)
{
    struct BBStage *stage = BB_STAGE;
    struct BBSwingBat *bat = &stage->bat;
    struct BBBillboard *board = &stage->roulette.board;
    struct BBWheel *wheel = &stage->roulette.wheel;
    struct BBCoins *coins = (struct BBCoins *) ((u8 *) stage + 0x604);
    struct BBBillboard *boards = stage->roulette.boards;
    struct BBHelmetGroup *helmets = &stage->helmets;
    struct GMAModelEntry *entry;

    if ((u32) stage->gma != 0) {
        entry = &stage->gma->modelEntries[0];
        bat->model = entry->model;
    }
    if ((u32) stage->gma != 0) {
        entry = &stage->gma->modelEntries[1];
        helmets->model = entry->model;
    }
    switch (fn_18_1818(0)) {
    case 0:
        entry = &stage->gma->modelEntries[11];
        break;
    case 1:
        entry = &stage->gma->modelEntries[14];
        break;
    case 2:
        entry = &stage->gma->modelEntries[12];
        break;
    case 3:
        entry = &stage->gma->modelEntries[13];
        break;
    }
    coins->model0 = entry->model;
    switch (fn_18_1818(1)) {
    case 0:
        entry = &stage->gma->modelEntries[11];
        break;
    case 1:
        entry = &stage->gma->modelEntries[14];
        break;
    case 2:
        entry = &stage->gma->modelEntries[12];
        break;
    case 3:
        entry = &stage->gma->modelEntries[13];
        break;
    }
    coins->model1 = entry->model;
    if ((u32) stage->gma != 0) {
        entry = &stage->gma->modelEntries[15];
        board->model = entry->model;
    }
    if ((u32) stage->gma != 0) {
        entry = &stage->gma->modelEntries[2];
        wheel->model = entry->model;
    }
    entry = &stage->gma->modelEntries[10];
    boards[0].model = entry->model;
    entry = &stage->gma->modelEntries[3];
    boards[1].model = entry->model;
    entry = &stage->gma->modelEntries[5];
    boards[2].model = entry->model;
    entry = &stage->gma->modelEntries[6];
    boards[3].model = entry->model;
    entry = &stage->gma->modelEntries[8];
    boards[4].model = entry->model;
    entry = &stage->gma->modelEntries[7];
    boards[5].model = entry->model;
    entry = &stage->gma->modelEntries[9];
    boards[6].model = entry->model;
    entry = &stage->gma->modelEntries[4];
    boards[7].model = entry->model;
}

void fn_18_138C0(void)
{
    lbl_18_bss_2E0.unk44 |= 1;
    fn_18_14224();
}

void fn_18_138F8(void)
{
    struct BBSwingBat *bat = BB_BAT;
    struct Ball *ball = balls;
    Mtx sp20;
    Vec sp14;
    Vec sp8;

    fn_18_13B48();
    mtxa_from_identity();
    mtxa_from_quat(&ball->ape->rot);
    mtxa_to_mtx(sp20);
    bat->prevPos = bat->pos;
    bat->pos = ball->ape->pos;
    mtxa_from_translate(&bat->pos);
    mtxa_scale_s(ball->ape->scale);
    mtxa_translate(&ball->ape->unk264);
    mtxa_mult_right(sp20);
    mtxa_mult_right(g_get_ape_anim_smth(ball->ape, 2));
    mtxa_translate_xyz(bat->trans.x, bat->trans.y, bat->trans.z);
    mtxa_rotate_z(bat->rot.z);
    mtxa_rotate_y(bat->rot.y);
    mtxa_rotate_x(bat->rot.x);
    mtxa_to_mtx(bat->mtx);
    mtxa_translate_xyz(1.0f, 0.0f, 0.0f);
    mtxa_to_mtx(sp20);
    sp14.x = sp20[0][3];
    sp14.y = sp20[1][3];
    sp14.z = sp20[2][3];
    if ((fn_18_A210(4) == 1) || (fn_18_A210(5) == 1)) {
        fn_18_223C4(&sp14);
    } else if ((fn_18_A210(6) == 1) || (fn_18_A210(7) == 1)) {
        sp8.x = 0.5f;
        sp8.y = 0.0f;
        sp8.z = 0.0f;
        mtxa_tf_point(&sp8, &sp14);
        sp8.x = 0.6f;
        sp8.y = 0.0f;
        sp8.z = 0.0f;
        mtxa_tf_point(&sp8, &sp8);
        PSVECSubtract(&sp8, &sp14, &sp8);
        fn_18_22414(&sp14, &sp8);
    }
}

void fn_18_13AE4(void)
{
    struct BBSwingBat *bat = BB_BAT;

    if ((u32) bat->model != 0) {
        mtxa_from_mtxb();
        mtxa_mult_right(bat->mtx);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(bat->model);
    }
}

void fn_18_13B48(void)
{
    struct BBSwingBat *bat = BB_BAT;
    u8 *anim;
    struct Ape *ape;
    Vec sp1C;
    Vec sp10;
    struct BBTrack *spC;
    struct BBTrack *sp8;

    spC = NULL;
    sp8 = NULL;
    ape = balls->ape;
    anim = (u8 *) &((struct Skeleton *) ape->bodyAnim)->clock;   /* +0x40 on GC; frame at +0xC */
    if ((ape->unk6C == 0x65) && (fn_18_14008((s16) ape->unk70, &spC, &sp8) != 0)) {
        fn_18_14174(ape->unk70, *(float *) (anim + 0xC));
        fn_18_15664(spC, &sp1C.x, *(float *) (anim + 0xC));
        fn_18_15664(sp8, &sp10.x, *(float *) (anim + 0xC));
        bat->unkAC = *(float *) (anim + 0xC);
        bat->unkB0 = (s16) ape->unk70;
        bat->rot.x = 182.04445f * sp1C.x;
        bat->rot.y = 182.04445f * sp1C.y;
        bat->rot.z = 182.04445f * sp1C.z;
        bat->trans.x = 0.1 * sp10.x;
        bat->trans.y = 0.1 * sp10.y;
        bat->trans.z = 0.1 * sp10.z;
    }
}

void fn_18_13CD4(void)
{
    struct BBSwingBat *bat = BB_BAT;

    bat->unkB4 = bat->unkAC;
    bat->unkB8 = bat->unkB0;
}

s32 fn_18_13CF4(Vec *arg0, Vec *arg1, Vec *arg2)
{
    struct PhysicsBall spA0;
    Mtx sp70;
    Mtx sp40;
    Vec sp34;
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    struct BBTrack *spC;
    struct BBTrack *sp8;
    struct BBSwingBat *bat = BB_BAT;
    double dead2 = 0.01;
    float dead0 = 1.5f;
    float dead1 = 5.0f;
    s16 tr24;
    s16 tr23;
    s16 tr22;
    struct Ball *p;
    float f31;
    float tf30;
    float tf29;
    float tf28;
    float f1;
    struct Ball *ball;
    u8 r28;

    spC = NULL;
    sp8 = NULL;
    p = balls;
    ball = &balls[1];
    r28 = 0;
    if (fn_18_14008(bat->unkB0, &spC, &sp8) == 0) {
        return 0;
    }
    mtxa_from_identity();
    mtxa_from_quat(&p->ape->rot);
    mtxa_to_mtx(sp70);
    mtxa_from_translate(&bat->pos);
    mtxa_scale_s(p->ape->scale);
    mtxa_translate(&p->ape->unk264);
    mtxa_mult_right(sp70);
    mtxa_mult_right(g_get_ape_anim_smth(p->ape, 2));
    mtxa_to_mtx(sp70);
    f1 = bat->unkAC - 1.0;
    if (f1 < 0.0) {
        f1 = 0.0f;
    }
    for (f31 = f1; f31 <= bat->unkAC; f31 += 0.01) {
        fn_18_15664(spC, &sp34.x, f31);
        fn_18_15664(sp8, &sp28.x, f31);
        tr24 = 182.04445f * sp34.x;
        tr23 = 182.04445f * sp34.y;
        tr22 = 182.04445f * sp34.z;
        tf30 = 0.1 * sp28.x;
        tf29 = 0.1 * sp28.y;
        tf28 = 0.1 * sp28.z;
        mtxa_from_mtx(sp70);
        mtxa_translate_xyz(tf30, tf29, tf28);
        mtxa_rotate_z(tr22);
        mtxa_rotate_y(tr23);
        mtxa_rotate_x(tr24);
        mtxa_to_mtx(sp40);
        init_physicsball_from_ball(ball, &spA0);
        if (fn_18_17B7C(&spA0, sp40) == 1) {
            r28 = 1;
            break;
        }
    }
    if ((u8) r28 == 1) {
        PSVECSubtract(&ball->pos, &ball->prevPos, &sp1C);
        vec_normalize_len(&sp1C);
        sp10 = spA0.hardestColiNormal;
        PSVECSubtract(&sp10, &sp1C, &sp10);
        PSVECScale(&sp10, &sp10, 0.6f);
        *arg0 = sp10;
        *arg1 = spA0.hardestColiPoint;
        *arg2 = spA0.vel;
    }
    return (u8) r28;
}
struct BBTrackPair lbl_18_data_78DAC[4] = {
    { (struct BBTrack *) lbl_18_data_7A118, (struct BBTrack *) lbl_18_data_7A2A0 },
    { (struct BBTrack *) lbl_18_data_7EF48, (struct BBTrack *) lbl_18_data_7F0D0 },
    { (struct BBTrack *) lbl_18_data_7CC78, (struct BBTrack *) lbl_18_data_7CE00 },
    { (struct BBTrack *) lbl_18_data_7BAE8, (struct BBTrack *) lbl_18_data_7BC70 },
};

s32 fn_18_14008(s16 arg0, struct BBTrack **arg1, struct BBTrack **arg2)
{
    u8 idx = fn_18_1818(fn_18_198C8());
    s16 r4;

    *arg1 = lbl_18_data_78DAC[idx].a;
    *arg2 = lbl_18_data_78DAC[idx].b;
    r4 = 0;
    switch (arg0) {
    case 0:
        r4 = 7;
    case 1:
        r4 = (s16) (r4 + 0);
        break;
    case 2:
        r4 = 7;
    case 3:
        r4 = (s16) (r4 + 1);
        break;
    case 4:
        r4 = 7;
    case 5:
        r4 = (s16) (r4 + 2);
        break;
    case 6:
        r4 = 7;
    case 7:
        r4 = (s16) (r4 + 3);
        break;
    case 8:
        r4 = 7;
    case 9:
        r4 = (s16) (r4 + 4);
        break;
    case 10:
        r4 = 7;
    case 11:
        r4 = (s16) (r4 + 5);
        break;
    case 12:
        r4 = 7;
    case 13:
        r4 = (s16) (r4 + 6);
        break;
    default:
        r4 = -1;
        break;
    }
    if (r4 != -1) {
        *arg1 += r4;
        *arg2 += r4;
        return 1;
    }
    *arg1 = NULL;
    *arg2 = NULL;
    return 0;
}

void fn_18_14174(s32 arg0, float arg1)
{
    struct BBSwingBat *bat = BB_BAT;

    switch (arg0) {
    case 0:
    case 1:
        bat->unk8 = 0;
        break;
    case 2:
    case 3:
        bat->unk8 = 0;
        break;
    case 4:
    case 5:
        bat->unk8 = 0;
        break;
    case 6:
    case 7:
        bat->unk8 = 1;
        break;
    case 8:
    case 9:
        bat->unk8 = 0;
        break;
    }
}

s32 fn_18_141F8(void)
{
    struct BBSwingBat *bat = BB_BAT;

    return (u8) bat->unk8 == 1;
}

void fn_18_14224(void)
{
    struct BBSwingBat *bat = BB_BAT;
    struct BBPoleData *data = &bat->data;

    data->pos.x = 1.0f;
    data->pos.y = 0.0f;
    data->pos.z = 0.0f;
    data->radius = 0.3f;
    data->height = 1.4f;
    data->rot.x = 0;
    data->rot.y = 0;
    data->rot.z = 0x4000;
    *(s16 *) (&data->rot.z + 1) = 0;
}

void fn_18_14288(void)
{
    lbl_18_bss_2E0.unk2C4 |= 1;
    fn_18_150C0();
    fn_18_14308();
    fn_18_1454C();
    fn_18_14BB0();
}

void fn_18_142C8(void)
{
    lbl_18_bss_2E0.unk2C4 &= ~1;
}

void fn_18_142E0(void)
{
    fn_18_14470();
    fn_18_14EAC();
    fn_18_146E4();
}

void fn_18_14308(void)
{
    struct BBBillboard *board = BB_BOARD;
    struct GMAModel *model = board->model;

    memset(board, 0, 0x54);
    board->model = model;
    {
        Vec sp8 = { 0.0f, 5.5f, -5.0f };
        Vec *p = &sp8;

        board->offset = *p;
    }
}

s32 fn_18_14394(void)
{
    struct BBBillboard *board = BB_BOARD;

    if (board->offset.y > 0.6) {
        board->offset.y -= 0.1f;
        return 0;
    }
    board->offset.y = 0.6f;
    return 1;
}

s32 fn_18_143E0(void)
{
    struct BBBillboard *board = BB_BOARD;
    Vec sp14;
    {
        Vec sp8 = { 0.0f, 5.5f, -5.0f };
        Vec *p = &sp8;

        sp14 = *p;
    }
    if (sp14.y > board->offset.y) {
        board->offset.y += 0.1f;
        return 0;
    }
    board->offset.y = sp14.y;
    return 1;
}

void fn_18_14470(void)
{
    struct Camera *cam = cameras;
    struct BBBillboard *board = BB_BOARD;
    s16 spA;
    s16 sp8;

    mtxa_from_mtxb();
    mtxa_push();
    mtxa_from_translate(&cam->eye);
    mtxa_rotate_z(cam->rotZ);
    mtxa_rotate_y(cam->rotY);
    mtxa_rotate_x(cam->rotX);
    mtxa_tf_point(&board->offset, &board->pos);
    mtxa_from_translate(&board->pos);
    ray_to_euler_xy(&cam->eye, &board->pos, &spA, &sp8);
    mtxa_rotate_y(sp8);
    mtxa_rotate_x(spA);
    mtxa_rotate_z(0);
    mtxa_to_mtx(board->mtx);
    mtxa_pop();
    mtxa_mult_right(board->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(board->model);
}

void fn_18_1454C(void)
{
    struct BBWheel *wheel = BB_WHEEL;
    struct GMAModel *model = wheel->model;

    memset(wheel, 0, 0x70);
    wheel->model = model;
    {
        Vec sp8 = { 0.0f, 5.5f, -5.0f };
        Vec *p = &sp8;

        wheel->offset = *p;
    }
    wheel->speed = 3276.8f;
    wheel->decel = 0.0f;
    wheel->angle = 0;
    wheel->timer = 0;
    wheel->state = 0;
}

s32 fn_18_14608(void)
{
    struct BBWheel *wheel = BB_WHEEL;

    if (wheel->offset.y > 0.6) {
        wheel->offset.y -= 0.1f;
        return 0;
    }
    wheel->offset.y = 0.6f;
    return 1;
}

s32 fn_18_14654(void)
{
    struct BBWheel *wheel = BB_WHEEL;
    Vec sp14;
    {
        Vec sp8 = { 0.0f, 5.5f, -5.0f };
        Vec *p = &sp8;

        sp14 = *p;
    }
    if (sp14.y > wheel->offset.y) {
        wheel->offset.y += 0.1f;
        return 0;
    }
    wheel->offset.y = sp14.y;
    return 1;
}

void fn_18_146E4(void)
{
    struct Camera *cam = cameras;
    struct BBWheel *wheel = BB_WHEEL;
    s16 spA;
    s16 sp8;

    mtxa_from_mtxb();
    mtxa_push();
    mtxa_from_translate(&cam->eye);
    mtxa_rotate_z(cam->rotZ);
    mtxa_rotate_y(cam->rotY);
    mtxa_rotate_x(cam->rotX);
    mtxa_tf_point(&wheel->offset, &wheel->pos);
    mtxa_from_translate(&wheel->pos);
    ray_to_euler_xy(&cam->eye, &wheel->pos, &spA, &sp8);
    mtxa_rotate_y(sp8);
    mtxa_rotate_x(spA);
    mtxa_rotate_z(0);
    mtxa_rotate_z(wheel->angle);
    mtxa_to_mtx(wheel->mtx);
    mtxa_pop();
    mtxa_mult_right(wheel->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(wheel->model);
}

void fn_18_147C8(void)
{
    register struct BBWheel *wheel = BB_WHEEL;

    if (wheel->state != 0) {
        wheel->speed -= wheel->decel;
        if (wheel->speed <= 0.0f) {
            wheel->speed = 0.0f;
            wheel->state = 2;
        }
    }
    wheel->angle -= (s16) wheel->speed;
    if (wheel->state == 0) {
        wheel->timer++;
        wheel->timer %= 0x14;
        if ((wheel->angle > 0) && (wheel->angle < 0x2D8)) {
            wheel->angle = 0;
            wheel->timer = 0;
        } else if (wheel->angle < 0) {
            register s32 rawAngle;
            register s32 limit;
            register s32 angle;

#ifdef __MWERKS__
            /* Keep sign extension and limit construction in separate operands. */
            asm
            {
                lha rawAngle, wheel->angle
                extsh angle, rawAngle
                lis limit, 1
                addi limit, limit, -0x2D9
            }
#else
            rawAngle = wheel->angle;
            angle = (s16)rawAngle;
            limit = 0xFD27;
#endif
            if (angle > (s16)limit) {
                wheel->angle = 0;
                wheel->timer = 0;
            }
        }
    }
}

void fn_18_148F0(void)
{
    struct BBWheel *wheel = BB_WHEEL;
    s16 r;
    float dead0 = 131072.0f;
    float dead1 = 180.0f;
    u32 t;

    r = rand() & 0x7FFF;
    wheel->unk62 = (s16) (__abs(r) % 10);
    wheel->unk62 = ((9 - wheel->unk62) * 0x24) + 0x12;
    t = (u32) (131072.0f + (3276.8f * (float) (0x13 - wheel->timer)) + (182.04445f * (float) wheel->unk62));
    wheel->decel = 10737418.760000005 / (t * 2);
    wheel->state = 1;
}

s32 fn_18_14A20(void)
{
    struct BBWheel *wheel = BB_WHEEL;

    return (s16) wheel->state == 2;
}

void fn_18_14A4C(void)
{
    struct BBWheel *wheel = BB_WHEEL;
    struct BBRoulette *rl = BB_ROULETTE;
    s16 sign;
    s16 tmp;
    s16 val;

    sign = wheel->angle & 0x8000;
    tmp = wheel->angle & 0x7FFF;
    val = tmp / 6553;
    if ((s16) sign != 0) {
        rl->stop = val + 5;
    } else {
        rl->stop = val;
    }
    printf("stop_kind >> %d\n", rl->stop);
}

u8 fn_18_14AF4(void)
{
    struct BBRoulette *rl = BB_ROULETTE;

    switch (rl->order[rl->stop]) {
    case 0:
        return 4;
    case 1:
        return 5;
    case 2:
        return 6;
    case 3:
        return 3;
    case 4:
        return 1;
    case 5:
        return 2;
    default:
        if (fn_18_19928() == 7) {
            return 7;
        }
        return 0;
    }
}

void fn_18_14BB0(void)
{
    struct BBRoulette *rl = BB_ROULETTE;
    s32 i;
    struct GMAModel *model;

    for (i = 0; i < 8; i++) {
        model = rl->boards[i].model;
        memset(&rl->boards[i], 0, 0x54);
        rl->boards[i].model = model;
        {
            Vec sp8 = { 0.0f, 5.5f, -5.0f };
            Vec *p = &sp8;

            rl->boards[i].offset = *p;
        }
    }
}

s32 fn_18_14C78(void)
{
    struct BBRoulette *rl = BB_ROULETTE;
    u8 sp8[8];
    s32 i;
    u8 count;

    memset(sp8, 0, 8);
    for (i = 0; i < 8; i++) {
        if (rl->boards[i].offset.y > 0.6) {
            rl->boards[i].offset.y -= 0.1f;
        } else {
            rl->boards[i].offset.y = 0.6f;
            sp8[i] = 1;
        }
    }
    count = 0;
    for (i = 0; i < 8; i++) {
        if (sp8[i] == 1) {
            count = (u8) count + 1;
        }
    }
    return (u8) count == 8;
}

s32 fn_18_14D78(void)
{
    struct BBRoulette *rl = BB_ROULETTE;
    Vec sp1C;
    u8 sp8[8];
    s32 i;
    u8 count;
    {
        Vec sp10 = { 0.0f, 5.5f, -5.0f };
        Vec *p = &sp10;

        sp1C = *p;
    }
    memset(sp8, 0, 8);
    for (i = 0; i < 8; i++) {
        if (sp1C.y > rl->boards[i].offset.y) {
            rl->boards[i].offset.y += 0.1f;
        } else {
            rl->boards[i].offset.y = sp1C.y;
            sp8[i] = 1;
        }
    }
    count = 0;
    for (i = 0; i < 8; i++) {
        if (sp8[i] == 1) {
            count = (u8) count + 1;
        }
    }
    return (u8) count == 8;
}

void fn_18_14EAC(void)
{
    struct Camera *cam = cameras;
    struct BBRoulette *rl = BB_ROULETTE;
    struct BBBillboard *board;
    s32 i;
    u8 kind;
    s16 spA;
    s16 sp8;

    kind = fn_18_19928();
    for (i = 0; i < 0xA; i++) {
        switch (rl->order[i]) {
        case 0:
            board = &rl->boards[1];
            break;
        case 1:
            board = &rl->boards[2];
            break;
        case 2:
            board = &rl->boards[3];
            break;
        case 3:
            board = &rl->boards[4];
            break;
        case 4:
            board = &rl->boards[5];
            break;
        case 5:
            board = &rl->boards[6];
            break;
        case 6:
            if (kind == 7) {
                board = &rl->boards[7];
            } else {
                board = &rl->boards[0];
            }
            break;
        case 7:
            if (kind == 7) {
                board = &rl->boards[7];
            } else {
                board = &rl->boards[0];
            }
            break;
        case 8:
            if (kind == 7) {
                board = &rl->boards[7];
            } else {
                board = &rl->boards[0];
            }
            break;
        case 9:
            if (kind == 7) {
                board = &rl->boards[7];
            } else {
                board = &rl->boards[0];
            }
            break;
        }
        mtxa_from_mtxb();
        mtxa_push();
        mtxa_from_translate(&cam->eye);
        mtxa_rotate_z(cam->rotZ);
        mtxa_rotate_y(cam->rotY);
        mtxa_rotate_x(cam->rotX);
        mtxa_tf_point(&board->offset, &board->pos);
        mtxa_from_translate(&board->pos);
        ray_to_euler_xy(&cam->eye, &board->pos, &spA, &sp8);
        mtxa_rotate_y(sp8);
        mtxa_rotate_x(spA);
        mtxa_rotate_z(0);
        mtxa_rotate_z((s16) (182.04444885253906 * (36.0 * i)));
        mtxa_to_mtx(board->mtx);
        mtxa_pop();
        mtxa_mult_right(board->mtx);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(board->model);
    }
}

void fn_18_150C0(void)
{
    struct BBRoulette *rl = BB_ROULETTE;
    u32 i;
    u8 n;
    u32 j;
    s16 r;
    u8 val;
    u8 dup;

    memset(rl->order, -1, 0xA);
    n = 0;
    for (i = 0; i < 0xA; i++) {
        do {
            r = rand() & 0x7FFF;
            val = __abs(r) % 10;
            dup = 0;
            for (j = 0; j < 0xA; j++) {
                if ((s32) val == rl->order[j]) {
                    dup = 1;
                }
            }
        } while (dup != 0);
        rl->order[n] = (s8) val;
        n++;
    }
}

void fn_18_151B8(struct Ball *arg0)
{
    memset(&((struct BBWhole *) &lbl_18_bss_2E0)->helmets[arg0->playerId - 2], 0, 0x68);
}

void fn_18_15200(void)
{
    lbl_18_bss_2E0.unk11C |= 1;
}

void fn_18_15218(struct Ball *arg0, void *arg1)
{
    Mtx sp14;
    Vec sp8;
    struct BBHelmet *helmet = &((struct BBWhole *) &lbl_18_bss_2E0)->helmets[arg0->playerId - 2];
    struct Ball *ball = &balls[1];
    register float dx;
    register float dy;
    register float dz;
    float tf1;
    float f2;

    dx = ball->pos.x - arg0->pos.x;
    dy = ball->pos.y - arg0->pos.y;
    dz = ball->pos.z - arg0->pos.z;
    dx = dx * dx;
    dx = __fmadds(dy, dy, dx);
    dx = __fmadds(dz, dz, dx);
    tf1 = math_sqrt(dx);
    if (tf1 <= 4.2) {
        f2 = 1.0 / tf1;
        if (f2 > 1.0) {
            f2 = 1.0f;
        }
        if (9.0 * f2 > helmet->unk40) {
            helmet->unk40 = 9.0 * f2;
        }
    }
    fn_18_154A8(helmet);
    helmet->pos = arg0->pos;
    mtxa_from_quat(&arg0->ape->rot);
    mtxa_rotate_y(0x4000);
    mtxa_to_mtx(sp14);
    mtxa_from_translate(&helmet->pos);
    mtxa_mult_right(sp14);
    mtxa_rotate_z(helmet->rot.z);
    mtxa_rotate_y(helmet->rot.y);
    mtxa_rotate_x(helmet->rot.x);
    mtxa_rotate_z(helmet->rot2.z);
    mtxa_rotate_y(helmet->rot2.y);
    mtxa_rotate_x(helmet->rot2.x);
    mtxa_to_mtx(helmet->mtx);
    if (((u32) (helmet->flags & 1) == 0) && (((u8 *) arg1)[7] == 1)) {
        helmet->flags |= 1;
        sp8.x = 1.0f;
        sp8.y = 0.0f;
        sp8.z = 0.0f;
        mtxa_tf_point(&sp8, &sp8);
        fn_18_22454(&sp8);
    }
}

void fn_18_153EC(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        fn_18_1542C((u8) i);
    }
}

void fn_18_1542C(u8 arg0)
{
    struct BBHelmetGroup *helmets = BB_HELMETS;

    if ((u32) helmets->model != 0) {
        mtxa_from_mtxb();
        mtxa_mult_right(helmets->items[arg0].mtx);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(helmets->model);
    }
}

void fn_18_154A8(struct BBHelmet *arg0)
{
    struct BBKey *sp1C[3];
    s16 *sp10[3];
    s16 sp8[3];
    struct BBTrack *track = (struct BBTrack *) lbl_18_data_7F2B8;
    s32 i;
    s32 j;

    for (i = 0; i < 1; i++, track++) {
        sp1C[0] = track->keys[0];
        sp1C[1] = track->keys[1];
        sp1C[2] = track->keys[2];
        sp8[0] = track->counts[0];
        sp8[1] = track->counts[1];
        sp8[2] = track->counts[2];
        if (i == 0) {
            sp10[0] = &arg0->rot.x;
            sp10[1] = &arg0->rot.y;
            sp10[2] = &arg0->rot.z;
        } else {
            sp10[0] = &arg0->rot2.x;
            sp10[1] = &arg0->rot2.y;
            sp10[2] = &arg0->rot2.z;
        }
        for (j = 0; j < 3; j++) {
            float v = bb_eval_track(sp1C[j], sp8[j], arg0->unk40);

            *sp10[j] = 182.04445f * v;
        }
    }
}

void fn_18_15664(struct BBTrack *arg0, float *arg1, float farg0)
{
    struct BBKey *sp1C[3];
    float *sp10[3];
    s16 sp8[3];
    s32 i;

    sp10[0] = arg1;
    sp10[1] = arg1 + 1;
    sp10[2] = arg1 + 2;
    sp1C[0] = arg0->keys[0];
    sp1C[1] = arg0->keys[1];
    sp1C[2] = arg0->keys[2];
    sp8[0] = arg0->counts[0];
    sp8[1] = arg0->counts[1];
    sp8[2] = arg0->counts[2];
    for (i = 0; i < 3; i++) {
        *sp10[i] = bb_eval_track(sp1C[i], sp8[i], farg0);
    }
}

void fn_18_157A8(void)
{
    double dead = 0.3;

    lbl_18_bss_2E0.unk638 |= 1;
}

void fn_18_157C4(void)
{
    struct BBCoins *coins = BB_COINS;
    struct GMAModel *model;

    if ((lbl_18_bss_2E0.unk638 & 1) != 0) {
        if (fn_18_198C8() == 0) {
            model = coins->model0;
        } else {
            model = coins->model1;
        }
        bb_coin_bob();
        if (fn_18_19B90() == 1) {
            {
                Vec sp20 = { 9.5752f, 0.9f, -9.9543f };
                Vec *p = &sp20;

                coins->pos = *p;
            }
            mtxa_from_mtxb();
            mtxa_translate(&coins->pos);
            mtxa_translate_xyz(0.0f, coins->height, 0.0f);
            mtxa_sq_from_identity();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(model);
        }
        if (fn_18_19BB8() == 1) {
            {
                Vec sp14 = { 0.0f, 0.9f, -19.5289f };
                Vec *p = &sp14;

                coins->pos = *p;
            }
            mtxa_from_mtxb();
            mtxa_translate(&coins->pos);
            mtxa_translate_xyz(0.0f, coins->height, 0.0f);
            mtxa_sq_from_identity();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(model);
        }
        if (fn_18_19BE0() == 1) {
            {
                Vec sp8 = { -9.5752f, 0.9f, -9.9541f };
                Vec *p = &sp8;

                coins->pos = *p;
            }
            mtxa_from_mtxb();
            mtxa_translate(&coins->pos);
            mtxa_translate_xyz(0.0f, coins->height, 0.0f);
            mtxa_sq_from_identity();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(model);
        }
    }
}

