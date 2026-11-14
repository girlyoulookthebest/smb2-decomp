#include "race2.h"
#include "macros.h"

struct Race2CamSeg
{
    float start;
    float end;
    s8 type;
    Vec unkC;
};

struct Race2CamFov
{
    u16 unk0;
    u16 unk2;
};

struct Race2DebugCam
{
    u8 filler0[8];
    s8 unk8;
    u8 filler9[0x18 - 9];
};

extern s32 lbl_1_bss_D336C;
extern u16 lbl_1_data_65754;
extern struct Race2DebugCam lbl_1_bss_D3B88[0x40];
extern s16 player_count;
void g_set_smth_with_sound(int);

static Vec lbl_10_bss_0;
static s8 lbl_10_bss_C[5];
static struct Race2CamSeg *lbl_10_bss_14;
static s16 lbl_10_bss_18;
static s8 lbl_10_bss_1A;
static s8 lbl_10_bss_1B;
static s32 lbl_10_bss_1C;
static float lbl_10_bss_20;
static float lbl_10_bss_24;
static Vec lbl_10_bss_28;
static float lbl_10_bss_34[5];
static struct Race2CamFov *lbl_10_bss_48;
static u16 lbl_10_bss_4C;
static u16 lbl_10_bss_50[4];

/* Stripped at link: makes lbl_10_bss_0 the first static referenced. */
Vec *race2_cam_bss_layout(void)
{
    return &lbl_10_bss_0;
}

void fn_10_7C5C(void);
void fn_10_7D08(struct Camera *camera, struct Ball *ball);
void fn_10_7DD0(void);
void fn_10_7FCC(void);
void fn_10_821C(void);
void fn_10_83E0(void);
void fn_10_8760(struct Camera *camera, struct Ball *ball);
void fn_10_89E0(struct Camera *camera, struct Ball *ball);
void fn_10_8F24(struct Camera *camera, struct Ball *ball);
void fn_10_9068(struct Camera *camera, struct Ball *ball);
void fn_10_935C(struct Camera *camera, struct Ball *ball);
void fn_10_9624(struct Camera *camera, struct Ball *ball);
void fn_10_9834(struct Camera *camera, struct Ball *ball);
void fn_10_9880(struct Camera *camera, struct Ball *ball);
void fn_10_99A0(struct Camera *camera, struct Ball *ball);
void fn_10_9AE4(struct Camera *camera, struct Ball *ball);
void fn_10_9CE8(struct Camera *camera, struct Ball *ball);
void fn_10_9E34(struct Camera *camera, struct Ball *ball);
void fn_10_A030(struct Camera *camera, struct Ball *ball);
void fn_10_A098(struct Camera *camera, struct Ball *ball);
void fn_10_A1E4(struct Camera *camera, struct Ball *ball);
void fn_10_A330(struct Camera *camera, struct Ball *ball);
void fn_10_A5E8(struct Camera *camera, struct Ball *ball);
void fn_10_A650(struct Camera *camera, struct Ball *ball);

u32 lbl_10_data_A8[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

static void (*lbl_10_data_218[])(struct Camera *, struct Ball *) = {
    fn_10_8760,
    fn_10_89E0,
    fn_10_8F24,
    fn_10_9068,
    fn_10_935C,
    fn_10_9624,
    fn_10_9834,
    fn_10_9880,
    fn_10_99A0,
    fn_10_9AE4,
    fn_10_9CE8,
    fn_10_9E34,
    fn_10_A030,
    fn_10_A098,
    fn_10_A1E4,
    fn_10_A330,
    fn_10_A5E8,
    fn_10_A650,
};

char *lbl_10_data_40C[] = {
    "SCAM_RACE2_INIT",
    "SCAM_RACE2_MAIN2",
    "SCAM_RACE2_LIVE2_INIT",
    "SCAM_RACE2_LIVE2",
    "SCAM_RACE2_FALL",
    "SCAM_RACE2_FALL_FIX",
    "SCAM_RACE2_GOAL_INIT",
    "SCAM_RACE2_GOAL",
    "SCAM_RACE2_GOAL_AIR_BREAK_INIT",
    "SCAM_RACE2_GOAL_AIR_BREAK",
    "SCAM_RACE2_REPLAY_FOLLOW_INIT",
    "SCAM_RACE2_REPLAY_FOLLOW",
    "SCAM_RACE2_REPLAY_FIX_INIT",
    "SCAM_RACE2_REPLAY_FIX",
    "SCAM_RACE2_LIVE_FOLLOW_INIT",
    "SCAM_RACE2_LIVE_FOLLOW",
    "SCAM_RACE2_LIVE_FIX_INIT",
    "SCAM_RACE2_LIVE_FIX",
};

static struct Race2CamFov lbl_10_data_454[] = { { 0xF300, 0x2AAA }, { 0xF680, 0x238E }, { 0xF680, 0x238E }, { 0xF680, 0x238E }, };

static struct Race2CamFov lbl_10_data_464[] = { { 0xF680, 0x238E }, { 0xF300, 0x31C7 }, { 0xF300, 0x31C7 }, };

static struct Race2CamSeg lbl_10_data_470[] = {
    { 0.0f, 8.7f, 13, { 53.623f, 2.351f, 32.472f } },
    { 8.7f, 29.8f, 11, { 7.0f, 2.0f, 0.0f } },
    { 29.8f, 55.3f, 13, { -58.95f, 4.554f, 20.436f } },
    { 55.3f, 69.6f, 13, { 52.507f, 1.748f, -113.698f } },
    { 69.6f, 84.4f, 13, { 107.532f, 1.654f, -114.089f } },
    { 84.4f, 93.2f, 11, { -4.0f, 2.0f, 0.0f } },
    { 93.2f, 100.2f, 13, { 53.623f, 2.351f, 32.472f } },
    { 0.0f, 0.0f, -1, { 0.0f, 0.0f, 0.0f } },
};

static struct Race2CamSeg lbl_10_data_530[] = {
    { 0.0f, 9.0f, 11, { 7.0f, 4.0f, 0.0f } },
    { 9.0f, 24.0f, 13, { -86.264f, -36.236f, -28.1f } },
    { 24.0f, 42.0f, 11, { -4.0f, 2.0f, 0.0f } },
    { 42.0f, 60.f, 13, { 44.521f, -34.724f, -123.654f } },
    { 60.f, 77.0f, 11, { 10.0f, 4.0f, 0.0f } },
    { 77.0f, 89.0f, 11, { -10.0f, 4.0f, 0.0f } },
    { 89.0f, 94.0f, 13, { -0.574f, -37.719f, 114.637f } },
    { 94.0f, 100.f, 11, { 7.0f, 4.0f, 0.0f } },
    { 0.0f, 0.0f, -1, { 0.0f, 0.0f, 0.0f } },
};

static struct Race2CamSeg lbl_10_data_608[] = {
    { 0.0f, 5.2f, 13, { -72.792f, -83.029f, -84.631f } },
    { 5.2f, 27.6f, 11, { 7.0f, 3.0f, 0.0f } },
    { 27.6f, 38.9f, 13, { -4.518f, -75.586f, -246.225f } },
    { 38.9f, 52.1f, 11, { -4.0f, 10.0f, 0.0f } },
    { 52.1f, 69.1f, 13, { -68.456f, -78.993f, -32.786f } },
    { 69.1f, 78.7f, 11, { 6.0f, 4.0f, 0.0f } },
    { 78.7f, 96.4f, 13, { -68.456f, -78.993f, -32.786f } },
    { 94.6f, 100.f, 13, { -72.792f, -83.029f, -84.631f } },
    { 0.0f, 0.0f, -1, { 0.0f, 0.0f, 0.0f } },
};

static struct Race2CamSeg lbl_10_data_6E0[] = {
    { 0.0f, 9.35f, 13, { 7.195f, 7.935f, -3.295f } },
    { 9.35f, 30.f, 13, { 65.5f, 8.685f, -210.343f } },
    { 30.f, 43.5f, 13, { -101.091f, 11.258f, -131.767f } },
    { 43.5f, 65.8f, 11, { 7.0f, 3.0f, 0.0f } },
    { 65.8f, 77.5f, 13, { 26.711f, 17.752f, -189.236f } },
    { 77.5f, 85.51f, 13, { -19.979f, 9.909f, -139.604f } },
    { 85.51f, 95.8f, 11, { -4.0f, 3.0f, 0.0f } },
    { 95.8f, 100.f, 13, { 7.195f, 7.935f, -3.295f } },
    { 0.0f, 0.0f, -1, { 0.0f, 0.0f, 0.0f } },
};

static struct Race2CamSeg lbl_10_data_7B8[] = {
    { 0.0f, 11.7f, 13, { -154.644f, 1.592f, -168.405f } },
    { 11.7f, 35.8f, 11, { 7.0f, 3.0f, 0.0f } },
    { 35.8f, 43.4f, 13, { 199.103f, 4.887f, -51.117f } },
    { 43.4f, 60.8f, 13, { 37.61f, 10.743f, 71.726f } },
    { 60.8f, 81.8f, 13, { 5.309f, 16.61f, -23.978f } },
    { 81.8f, 96.9f, 11, { -4.0f, 3.0f, 0.0f } },
    { 96.9f, 100.f, 13, { -154.644f, 1.592f, -168.405f } },
    { 0.0f, 0.0f, -1, { 0.0f, 0.0f, 0.0f } },
};

static struct Race2CamSeg lbl_10_data_878[] = {
    { 0.0f, 5.5f, 13, { -114.328f, -252.341f, -149.497f } },
    { 5.5f, 12.3f, 13, { -26.982f, -289.743f, -164.581f } },
    { 12.3f, 25.55f, 13, { 164.088f, -296.017f, 31.192f } },
    { 25.55f, 35.4f, 11, { -4.0f, 3.0f, 0.0f } },
    { 35.4f, 42.25f, 13, { -144.878f, -290.328f, -18.154f } },
    { 42.25f, 54.0f, 11, { 7.0f, 3.0f, 0.0f } },
    { 54.0f, 67.2f, 11, { -4.0f, 3.0f, 0.0f } },
    { 67.2f, 74.5f, 13, { 179.447f, -294.905f, 103.675f } },
    { 74.5f, 85.95f, 13, { 80.265f, -299.51f, -170.969f } },
    { 85.95f, 91.7f, 11, { 7.0f, 3.0f, 0.0f } },
    { 91.7f, 98.0f, 13, { -205.048f, -252.685f, -179.25f } },
    { 98.0f, 100.f, 13, { -114.328f, -252.341f, -149.497f } },
    { 0.0f, 0.0f, -1, { 0.0f, 0.0f, 0.0f } },
};

static struct Race2CamSeg *lbl_10_data_9B0[] = {
    lbl_10_data_470,
    lbl_10_data_530,
    lbl_10_data_608,
    lbl_10_data_6E0,
    lbl_10_data_7B8,
    lbl_10_data_878,
};

static float lbl_10_data_9C8 = 10.0f;
static float lbl_10_data_9CC = 4.0f;

void fn_10_7C5C(void)
{
    s16 i;

    for (i = 0; i < 5; i++) {
        lbl_10_bss_C[i] = -1;
    }
    if ((u32)(dip_switches & 1) != 0) {
        memset(lbl_1_bss_D3B88, 0, 0x600);
        for (i = 0; i < 0x40; i++) {
            lbl_1_bss_D3B88[i].unk8 = -1;
        }
    }
}

void fn_10_7D08(struct Camera *camera, struct Ball *ball)
{
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        if (lbl_10_bss_C[camera->idx] >= 0) {
            camera->unk1F = lbl_10_bss_C[camera->idx];
            lbl_10_bss_C[camera->idx] = -1;
        }
        if (camera->unk1F >= 0 && camera->unk1F < 0x12) {
            lbl_10_data_218[camera->unk1F](camera, ball);
        }
    }
}

void fn_10_7DD0(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    float t = 0.0f;
    s16 i;
    struct Race2CamSeg *seg;

    lbl_10_bss_14 = lbl_10_data_9B0[course->unk0];
    lbl_10_bss_18 = -1;
    set_camera_view_settings(0, 0.0f, 0.0f, 0.0f, 0.0f);
    cameras[1].mode = 0x3C;
    cameras[1].unk1F = -1;
    cameras[1].flags |= 0x40;
    set_camera_view_settings(1, 0.0f, 0.0f, 1.0f, 1.0f);
    enable_camera(1);
    for (i = 0, seg = lbl_10_bss_14; seg->type >= 0; i++, seg++) {
        if (seg->start <= t && t <= seg->end) {
            break;
        }
    }
    lbl_10_bss_18 = i;
    switch (seg->type) {
    case 12:
        break;
    case 11:
        lbl_10_bss_C[1] = seg->type - 1;
        lbl_10_data_9C8 = seg->unkC.x;
        lbl_10_data_9CC = seg->unkC.y;
        break;
    case 13:
        lbl_10_bss_C[1] = seg->type - 1;
        lbl_10_bss_0 = seg->unkC;
        break;
    }
    fn_10_7FCC();
}

void fn_10_7FCC(void)
{
    struct Race2CamSeg *seg;
    struct Race2CamSeg *prev;
    struct RacePlayer *player = lbl_10_bss_1230;
    float t = player->unk208;
    s16 i;

    prev = &lbl_10_bss_14[lbl_10_bss_18];
    for (i = 0, seg = lbl_10_bss_14; seg->type >= 0; i++, seg++) {
        if (seg->start <= t && t <= seg->end) {
            break;
        }
    }
    if (i != lbl_10_bss_18) {
        lbl_10_bss_18 = i;
        switch (seg->type) {
        case 12:
            break;
        case 11:
            if (prev->unkC.x != seg->unkC.x && prev->unkC.y != seg->unkC.y) {
                lbl_10_bss_C[1] = seg->type - 1;
                lbl_10_data_9C8 = seg->unkC.x;
                lbl_10_data_9CC = seg->unkC.y;
            }
            break;
        case 13:
            lbl_10_bss_C[1] = seg->type - 1;
            lbl_10_bss_0 = seg->unkC;
            break;
        }
    }
    if ((u32)(player->flags & 0x32) != 0) {
        set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
        set_camera_view_settings(1, 0.0f, 0.0f, 0.0f, 0.0f);
        g_set_smth_with_sound(0);
    } else {
        set_camera_view_settings(0, 0.0f, 0.0f, 0.0f, 0.0f);
        set_camera_view_settings(1, 0.0f, 0.0f, 1.0f, 1.0f);
        g_set_smth_with_sound(1);
    }
}

void fn_10_821C(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    float t = 0.0f;
    s16 i;
    struct Race2CamSeg *seg;

    lbl_10_bss_1A = 0;
    lbl_10_bss_1B = -1;
    lbl_10_bss_14 = lbl_10_data_9B0[course->unk0];
    lbl_10_bss_18 = -1;
    cameras[3].mode = 0x3C;
    cameras[3].unk1F = -1;
    cameras[3].flags |= 0x40;
    set_camera_view_settings(3, 0.5f, 0.5f, 0.5f, 0.5f);
    enable_camera(3);
    for (i = 0, seg = lbl_10_bss_14; seg->type >= 0; i++, seg++) {
        if (seg->start <= t && t <= seg->end) {
            break;
        }
    }
    lbl_10_bss_18 = i;
    switch (seg->type) {
    case 12:
        break;
    case 11:
        lbl_10_bss_C[3] = 0xE;
        lbl_10_data_9C8 = seg->unkC.x;
        lbl_10_data_9CC = seg->unkC.y;
        break;
    case 13:
        lbl_10_bss_C[3] = 0x10;
        lbl_10_bss_0 = seg->unkC;
        break;
    }
    fn_10_83E0();
}

void fn_10_83E0(void)
{
    struct RacePlayer *player = lbl_10_bss_1230;
    struct Ball *from;
    struct Ball *to;
    float t;
    s16 i;
    struct Race2CamSeg *seg;
    struct Race2CamSeg *prev;

    for (i = 0; i < player_count; i++) {
        player = lbl_10_bss_2D68[i];
        if ((u32)(player->flags & 2) == 0) {
            break;
        }
    }
    if (lbl_10_bss_1B < 0 && lbl_10_bss_1A != player->unk0) {
        lbl_10_bss_1B = player->unk0;
        if (lbl_10_bss_1230[lbl_10_bss_1B].unk200 - lbl_10_bss_1230[lbl_10_bss_1A].unk200 < -20.0) {
            lbl_10_bss_1C = 1;
        } else {
            lbl_10_bss_1C = 0.5 * lbl_80198B34;
        }
    }
    if (lbl_10_bss_1B >= 0) {
        from = &balls[lbl_10_bss_1A];
        to = &balls[lbl_10_bss_1B];
        lbl_10_bss_1C--;
        lbl_10_bss_20 = (float)lbl_10_bss_1C / (0.5 * lbl_80198B34);
        lbl_10_bss_24 = 1.0 - lbl_10_bss_20;
        lbl_10_bss_28.x = (from->pos.x * lbl_10_bss_20) + (to->pos.x * lbl_10_bss_24);
        lbl_10_bss_28.y = (from->pos.y * lbl_10_bss_20) + (to->pos.y * lbl_10_bss_24);
        lbl_10_bss_28.z = (from->pos.z * lbl_10_bss_20) + (to->pos.z * lbl_10_bss_24);
        if (lbl_10_bss_1C == 0) {
            lbl_10_bss_1A = lbl_10_bss_1B;
            lbl_10_bss_1B = -1;
        }
    } else {
        lbl_10_bss_28 = balls[lbl_10_bss_1A].pos;
    }
    t = player->unk208;
    prev = &lbl_10_bss_14[lbl_10_bss_18];
    for (i = 0, seg = lbl_10_bss_14; seg->type >= 0; i++, seg++) {
        if (seg->start <= t && t <= seg->end) {
            break;
        }
    }
    if (i != lbl_10_bss_18) {
        lbl_10_bss_18 = i;
        switch (seg->type) {
        case 12:
            break;
        case 11:
            if (prev->unkC.x != seg->unkC.x && prev->unkC.y != seg->unkC.y) {
                lbl_10_bss_C[3] = 0xE;
                lbl_10_data_9C8 = seg->unkC.x;
                lbl_10_data_9CC = seg->unkC.y;
            }
            break;
        case 13:
            lbl_10_bss_C[3] = 0x10;
            lbl_10_bss_0 = seg->unkC;
            break;
        }
    }
}

void fn_10_8760(struct Camera *camera, struct Ball *ball)
{
    Vec sp20;
    float *height;
    struct Race2CamFov *fov;

    height = &lbl_10_bss_34[camera->idx];
    camera_clear(camera);
    if (0.5 == camera->sub28.viewportW && 0.5 == camera->sub28.viewportH) {
        *height = 0.65f;
    } else if (0.5 == camera->sub28.viewportH) {
        *height = 0.65f;
    } else if (0.5 == camera->sub28.viewportW) {
        *height = 0.9f;
    } else {
        *height = 0.8f;
    }
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + *height;
    camera->lookAt.z = ball->pos.z;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(stagedef->startPos->yrot);
    sp20 = (Vec){ 0.0f, 1.0f, 3.0f };
    mtxa_tf_point(&sp20, &camera->eye);
    sp20 = (Vec){ 0.0f, 0.0f, 3.0f };
    mtxa_tf_point(&sp20, &camera->unkB4);
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp20);
    camera->rotY = math_atan2(sp20.x, sp20.z) - 0x8000;
    camera->rotX = math_atan2(sp20.y, math_sqrt(mathutil_sum_of_sq_2(sp20.x, sp20.z)));
    camera->rotZ = 0;
    camera->unk26 = 0xC;
    if (modeCtrl.unk30 == 2) {
        fov = &lbl_10_data_464[modeCtrl.wideMode];
    } else {
        fov = &lbl_10_data_454[modeCtrl.unk30 - 1];
    }
    lbl_10_bss_48 = fov;
    lbl_10_bss_4C = lbl_10_bss_48->unk0;
    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    camera->flags &= 0xFFFFFFFB;
    camera->flags |= 8;
    camera->unk1F = 1;
}

void fn_10_89E0(struct Camera *camera, struct Ball *ball)
{
    Vec sp44;
    Vec sp38;
    s16 rotX;
    float sq;
    Vec sp2C;
    s32 x;
    s16 rotY;
    float *height;
    s32 c;
    struct RacePlayer *player;

    player = (void *)ball->unk150;
    height = &lbl_10_bss_34[camera->idx];
    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    sp38 = camera->eye;
    sp2C = camera->lookAt;
    PSVECSubtract(&camera->unkB4, &camera->lookAt, &sp44);
    sq = mathutil_vec_sq_len(&sp44);
    if (sq > FLT_EPSILON) {
        PSVECScale(&sp44, &sp44, math_rsqrt(sq));
    } else {
        sp44 = (Vec){ 1.0f, 0.0f, 0.0f };
    }
    sp44.x = (0.75 * sp44.x) + camera->lookAt.x;
    sp44.y = (0.75 * sp44.y) + camera->lookAt.y;
    sp44.z = (0.75 * sp44.z) + camera->lookAt.z;
    camera->lookAt = (Vec){ ball->pos.x, ball->pos.y + *height, ball->pos.z };
    PSVECSubtract(&camera->lookAt, &sp44, &sp44);
    if (ball->unk80 < 0x3C) {
        rotX = 0;
    } else {
        rotX = math_atan2(sp44.y, math_sqrt(mathutil_sum_of_sq_2(sp44.x, sp44.z)));
    }
    rotY = math_atan2(sp44.x, sp44.z) - 0x8000;
    x = (s16)(rotY - camera->rotY);
    x = x < -0x200 ? -0x200 : x > 0x200 ? 0x200 : x;
    rotY = camera->rotY + x;
    rotY += player->unk334;
    if ((camera->flags & 2) == 0 && (ball->flags & 0x1000) == 0) {
        x = (s16)(ball->unk92 - rotY);
        if (x > 0x800) {
            x -= 0x800;
        } else if (x < -0x800) {
            x += 0x800;
        } else {
            x = 0;
        }
        x >>= 7;
        c = camera->unk114;
        if (x == 0) {
            c = 0;
        } else if ((c < 0 && x > 0) || (c > 0 && x < 0)) {
            c = 0;
        } else if (x < 0) {
            if (x < c - 4) {
                c -= 4;
            } else {
                c = x;
            }
        } else if (x > c + 4) {
            c += 4;
        } else {
            c = x;
        }
        rotY += c;
        x = (s16)(rotY - camera->rotY);
        x = x < -0x300 ? -0x300 : x > 0x300 ? 0x300 : x;
        rotY = camera->rotY + x;
    }
    if (rotX < -0x1800) {
        rotX = -0x1800;
    } else if (rotX > 0x1800) {
        rotX = 0x1800;
    }
    rotX = camera->unkC0 = camera->unkC0 + (0.2 * (rotX - camera->unkC0));
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(rotY);
    mtxa_rotate_x(rotX);
    sp44.x = 0.0f;
    sp44.y = 0.0f;
    sp44.z = 3.0f;
    mtxa_tf_point(&sp44, &camera->unkB4);
    camera->unk114 = rotY - camera->rotY;
    camera->rotY = rotY;
    if (lbl_1_bss_D336C != 0) {
        camera->rotX = rotX + lbl_1_data_65754;
    } else {
        camera->rotX = rotX + lbl_10_bss_4C;
    }
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    sp44 = (Vec){ 0.0f, 0.0f, math_sqrt(mathutil_sum_of_sq_2(3.0f, 1.0f)) };
    mtxa_tf_point(&sp44, &camera->eye);
    PSVECSubtract(&camera->eye, &sp38, &camera->eyeVel);
    PSVECSubtract(&camera->lookAt, &sp2C, &camera->lookAtVel);
}

void fn_10_8F24(struct Camera *camera, struct Ball *ball)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct StageExtraData *data = stagedef->extraData;

    camera->lookAt = (Vec){ 0.0f, 0.0f, 0.0f };
    camera->eye = camera->lookAt;
    camera->lookAt = balls[player_count - 1].pos;
    fn_10_181EC(data, 100.0 * (10.0 / course->unk8), &camera->eye);
    camera->eye.y += 4.0f;
    camera->flags &= 0xFFFFFFFB;
    camera->flags |= 8;
    camera->unk1F = 3;
    fn_10_9068(camera, ball);
}

void fn_10_9068(struct Camera *camera, struct Ball *ball)
{
    Vec sp14;
    Vec sp8;
    float d;
    float s;
    struct Race2Course *course = &lbl_10_bss_1208;
    struct StageExtraData *data = stagedef->extraData;
    struct RacePlayer *player;
    s16 i;

    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    if (lbl_10_bss_2D68[0]->unk3E == 0) {
        player = (void *)balls[0].unk150;
    } else {
        player = lbl_10_bss_2D68[0];
        for (i = 0; i < player_count; i++) {
            if ((u32)(lbl_10_bss_2D68[i]->flags & 0x20) == 0 && (u32)(lbl_10_bss_2D68[i]->flags & 2) == 0) {
                player = lbl_10_bss_2D68[i];
                break;
            }
        }
    }
    if (i == player_count) {
        player = lbl_10_bss_2D68[player_count - 1];
    }
    PSVECSubtract(&balls[player->unk0].pos, &camera->lookAt, &sp8);
    d = mathutil_vec_len(&sp8);
    if (d > 0.0f) {
        s = d / 50.0;
        if (s > 0.3) {
            s = 0.3f;
        }
        PSVECScale(&sp8, &sp8, s);
        PSVECAdd(&camera->lookAt, &sp8, &camera->lookAt);
    }
    fn_10_181EC(data, player->unk1F0 + (100.0 * (10.0 / course->unk8)), &sp14);
    sp14.y += 4.0f;
    PSVECSubtract(&sp14, &camera->eye, &sp8);
    d = mathutil_vec_len(&sp8);
    if (d > 0.0f) {
        s = d / 50.0;
        if (s > 0.3) {
            s = 0.3f;
        }
        PSVECScale(&sp8, &sp8, s);
        PSVECAdd(&camera->eye, &sp8, &camera->eye);
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp8);
    camera->rotY = math_atan2(sp8.x, sp8.z) - 0x8000;
    camera->rotX = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));
    camera->rotZ = 0;
}

void fn_10_935C(struct Camera *camera, struct Ball *ball)
{
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    s16 rotX;
    s16 rotY;
    s16 rotZ;
    s32 delta;
    struct RacePlayer *player;

    player = (void *)ball->unk150;
    sp2C = camera->eye;
    sp20 = camera->lookAt;
    rotX = camera->rotX;
    rotY = camera->rotY;
    rotZ = camera->rotZ;
    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    if ((u32)(player->flags & 0x20) != 0) {
        camera->sub28.view.fov = camera->sub28.view.fov + (5461.0f * ((float)(0x3C - player->unk3C) / 60.0f));
    }
    PSVECAdd(&camera->eye, &camera->eyeVel, &camera->eye);
    if (current_stage_id == 0x85 && -110.0 < camera->lookAt.x && camera->lookAt.x < -30.0
        && 84.0 < camera->lookAt.z && camera->lookAt.z < 150.0
        && camera->lookAt.y < 1.0 + ball->pos.y) {
        camera->unk1F = 5;
    } else {
        camera->lookAt = ball->pos;
        camera->lookAt.y += lbl_10_bss_34[camera->idx];
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp14);
    delta = -camera->rotY;
    camera->rotY = math_atan2(sp14.x, sp14.z) - 0x8000;
    camera->rotX = math_atan2(sp14.y, math_sqrt(mathutil_sum_of_sq_2(sp14.x, sp14.z)));
    camera->rotZ = 0;
    delta += camera->rotY;
    if (delta < 0) {
        delta = -delta;
    }
    if (delta > 0x100) {
        camera->eye = sp2C;
        camera->lookAt = sp20;
        camera->rotX = rotX;
        camera->rotY = rotY;
        camera->rotZ = rotZ;
        camera->eyeVel = (Vec){ 0.0f, 0.0f, 0.0f };
    }
    PSVECScale(&camera->eyeVel, &camera->eyeVel, 0.995f);
}

void fn_10_9624(struct Camera *camera, struct Ball *ball)
{
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    s16 rotX;
    s16 rotY;
    s16 rotZ;
    s32 delta;
    struct RacePlayer *player;

    player = (void *)ball->unk150;
    sp2C = camera->eye;
    sp20 = camera->lookAt;
    rotX = camera->rotX;
    rotY = camera->rotY;
    rotZ = camera->rotZ;
    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    if ((u32)(player->flags & 0x20) != 0) {
        camera->sub28.view.fov = camera->sub28.view.fov + (5461.0f * ((float)(0x3C - player->unk3C) / 60.0f));
    }
    PSVECAdd(&camera->eye, &camera->eyeVel, &camera->eye);
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp14);
    delta = -camera->rotY;
    camera->rotY = math_atan2(sp14.x, sp14.z) - 0x8000;
    camera->rotX = math_atan2(sp14.y, math_sqrt(mathutil_sum_of_sq_2(sp14.x, sp14.z)));
    camera->rotZ = 0;
    delta += camera->rotY;
    if (delta < 0) {
        delta = -delta;
    }
    if (delta > 0x100) {
        camera->eye = sp2C;
        camera->lookAt = sp20;
        camera->rotX = rotX;
        camera->rotY = rotY;
        camera->rotZ = rotZ;
        camera->eyeVel = (Vec){ 0.0f, 0.0f, 0.0f };
    }
    PSVECScale(&camera->eyeVel, &camera->eyeVel, 0.995f);
}

void fn_10_9834(struct Camera *camera, struct Ball *ball)
{
    lbl_10_bss_50[ball->playerId] = ball->rotY;
    camera->unk1F = 7;
    fn_10_9880(camera, ball);
}

void fn_10_9880(struct Camera *camera, struct Ball *ball)
{
    Vec sp8;

    camera->lookAt = ball->pos;
    lbl_10_bss_50[ball->playerId] += 0x50;
    mtxa_from_rotate_y(lbl_10_bss_50[ball->playerId]);
    mtxa_rotate_x(-0x1800);
    mtxa_tf_vec_xyz(0.0f, 0.0f, 6.0f, &camera->eye);
    PSVECAdd(&camera->eye, &camera->lookAt, &camera->eye);
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp8);
    camera->rotY = math_atan2(sp8.x, sp8.z) - 0x8000;
    camera->rotX = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));
    camera->rotZ = 0;
}

void fn_10_99A0(struct Camera *camera, struct Ball *ball)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct StageExtraData *data = stagedef->extraData;

    camera->lookAt = (Vec){ 0.0f, 0.0f, 0.0f };
    camera->eye = camera->lookAt;
    camera->lookAt = balls[player_count - 1].pos;
    fn_10_181EC(data, 100.0 * (10.0 / course->unk8), &camera->eye);
    camera->eye.y += 4.0f;
    camera->flags &= 0xFFFFFFFB;
    camera->flags |= 8;
    camera->unk1F = 9;
    fn_10_9AE4(camera, ball);
}

void fn_10_9AE4(struct Camera *camera, struct Ball *ball)
{
    Vec sp20;
    Vec sp14;
    float d;
    float s;

    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    PSVECSubtract(&ball->pos, &camera->lookAt, &sp14);
    d = mathutil_vec_len(&sp14);
    if (d > 0.0f) {
        s = d / 50.0;
        if (s > 0.3) {
            s = 0.3f;
        }
        PSVECScale(&sp14, &sp14, s);
        PSVECAdd(&camera->lookAt, &sp14, &camera->lookAt);
    }
    sp20 = (Vec){ 0.0f, 0.0f, -3.0f };
    mtxa_from_quat(&ball->ape->rot);
    mtxa_tf_vec(&sp20, &sp20);
    PSVECAdd(&sp20, &ball->pos, &sp20);
    sp20.y += 1.0f;
    PSVECSubtract(&sp20, &camera->eye, &sp14);
    d = mathutil_vec_len(&sp14);
    if (d > 0.0f) {
        s = d / 50.0;
        if (s > 0.3) {
            s = 0.3f;
        }
        PSVECScale(&sp14, &sp14, s);
        PSVECAdd(&camera->eye, &sp14, &camera->eye);
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp14);
    camera->rotY = math_atan2(sp14.x, sp14.z) - 0x8000;
    camera->rotX = math_atan2(sp14.y, math_sqrt(mathutil_sum_of_sq_2(sp14.x, sp14.z)));
    camera->rotZ = 0;
}

void fn_10_9CE8(struct Camera *camera, struct Ball *ball)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct StageExtraData *data = stagedef->extraData;
    s16 i;

    camera->lookAt = (Vec){ 0.0f, 0.0f, 0.0f };
    camera->eye = camera->lookAt;
    camera->lookAt = balls[0].pos;
    fn_10_181EC(data, course->unkC * lbl_10_data_9C8, &camera->eye);
    camera->eye.y += lbl_10_data_9CC;
    camera->flags &= 0xFFFFFFFB;
    camera->flags |= 8;
    camera->unk1F = 0xB;
    for (i = 0; i < 3; i++) {
        fn_10_9E34(camera, ball);
    }
}

void fn_10_9E34(struct Camera *camera, struct Ball *ball)
{
    Vec sp14;
    Vec sp8;
    float d;
    float s;
    struct Race2Course *course = &lbl_10_bss_1208;
    struct StageExtraData *data = stagedef->extraData;
    struct RacePlayer *player = lbl_10_bss_1230;

    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    PSVECSubtract(&balls[0].pos, &camera->lookAt, &sp8);
    d = mathutil_vec_len(&sp8);
    if (d > 0.0f) {
        s = d / 20.0;
        if (s > 0.9) {
            s = 0.9f;
        }
        PSVECScale(&sp8, &sp8, s);
        PSVECAdd(&camera->lookAt, &sp8, &camera->lookAt);
    }
    fn_10_181EC(data, player->unk208 + (course->unkC * lbl_10_data_9C8), &sp14);
    sp14.y += lbl_10_data_9CC;
    PSVECSubtract(&sp14, &camera->eye, &sp8);
    d = mathutil_vec_len(&sp8);
    if (d > 0.0f) {
        s = d / 20.0;
        if (s > 0.9) {
            s = 0.9f;
        }
        PSVECScale(&sp8, &sp8, s);
        PSVECAdd(&camera->eye, &sp8, &camera->eye);
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp8);
    camera->rotY = math_atan2(sp8.x, sp8.z) - 0x8000;
    camera->rotX = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));
    camera->rotZ = 0;
}

void fn_10_A030(struct Camera *camera, struct Ball *ball)
{
    camera->lookAt = balls[0].pos;
    camera->flags &= 0xFFFFFFFB;
    camera->flags |= 8;
    camera->unk1F = 0xD;
    fn_10_A098(camera, ball);
}

void fn_10_A098(struct Camera *camera, struct Ball *ball)
{
    Vec sp8;

    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    PSVECSubtract(&balls[0].pos, &camera->lookAt, &sp8);
    if (mathutil_vec_len(&sp8) > 0.8) {
        vec_set_len(0.8f, &sp8, &sp8);
        PSVECAdd(&camera->lookAt, &sp8, &camera->lookAt);
    } else {
        camera->lookAt = balls[0].pos;
    }
    camera->eye = lbl_10_bss_0;
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp8);
    camera->rotY = math_atan2(sp8.x, sp8.z) - 0x8000;
    camera->rotX = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));
    camera->rotZ = 0;
}

void fn_10_A1E4(struct Camera *camera, struct Ball *ball)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct StageExtraData *data = stagedef->extraData;
    s16 i;

    camera->lookAt = (Vec){ 0.0f, 0.0f, 0.0f };
    camera->eye = camera->lookAt;
    camera->lookAt = lbl_10_bss_28;
    fn_10_181EC(data, course->unkC * lbl_10_data_9C8, &camera->eye);
    camera->eye.y += lbl_10_data_9CC;
    camera->flags &= 0xFFFFFFFB;
    camera->flags |= 8;
    camera->unk1F = 0xF;
    for (i = 0; i < 3; i++) {
        fn_10_A330(camera, ball);
    }
}

void fn_10_A330(struct Camera *camera, struct Ball *ball)
{
    Vec sp14;
    Vec sp8;
    float d;
    float s;
    struct Race2Course *course = &lbl_10_bss_1208;
    struct StageExtraData *data = stagedef->extraData;
    struct RacePlayer *player = lbl_10_bss_1230;

    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    PSVECSubtract(&lbl_10_bss_28, &camera->lookAt, &sp8);
    d = mathutil_vec_len(&sp8);
    if (d > 0.0f) {
        s = d / 20.0;
        if (s > 0.9) {
            s = 0.9f;
        }
        PSVECScale(&sp8, &sp8, s);
        PSVECAdd(&camera->lookAt, &sp8, &camera->lookAt);
    }
    fn_10_181EC(data, player->unk208 + (course->unkC * lbl_10_data_9C8), &sp14);
    if (lbl_10_bss_1B >= 0) {
        fn_10_181EC(data,
                    (lbl_10_data_9C8 * course->unkC)
                        + ((lbl_10_bss_20 * lbl_10_bss_1230[lbl_10_bss_1A].unk208)
                           + (lbl_10_bss_24 * lbl_10_bss_1230[lbl_10_bss_1B].unk208)),
                    &sp14);
    } else {
        fn_10_181EC(data, lbl_10_bss_1230[lbl_10_bss_1A].unk208 + (lbl_10_data_9C8 * course->unkC), &sp14);
    }
    sp14.y += lbl_10_data_9CC;
    PSVECSubtract(&sp14, &camera->eye, &sp8);
    d = mathutil_vec_len(&sp8);
    if (d > 0.0f) {
        s = d / 20.0;
        if (s > 0.9) {
            s = 0.9f;
        }
        PSVECScale(&sp8, &sp8, s);
        PSVECAdd(&camera->eye, &sp8, &camera->eye);
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp8);
    camera->rotY = math_atan2(sp8.x, sp8.z) - 0x8000;
    camera->rotX = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));
    camera->rotZ = 0;
}

void fn_10_A5E8(struct Camera *camera, struct Ball *ball)
{
    camera->lookAt = lbl_10_bss_28;
    camera->flags &= 0xFFFFFFFB;
    camera->flags |= 8;
    camera->unk1F = 0x11;
    fn_10_A650(camera, ball);
}

void fn_10_A650(struct Camera *camera, struct Ball *ball)
{
    Vec sp8;

    camera->sub28.view.fov = (s16)lbl_10_bss_48->unk2;
    PSVECSubtract(&lbl_10_bss_28, &camera->lookAt, &sp8);
    if (mathutil_vec_len(&sp8) > 0.8) {
        vec_set_len(0.8f, &sp8, &sp8);
        PSVECAdd(&camera->lookAt, &sp8, &camera->lookAt);
    } else {
        camera->lookAt = lbl_10_bss_28;
    }
    camera->eye = lbl_10_bss_0;
    PSVECSubtract(&camera->lookAt, &camera->eye, &sp8);
    camera->rotY = math_atan2(sp8.x, sp8.z) - 0x8000;
    camera->rotX = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));
    camera->rotZ = 0;
}

