#include "boat.h"
#include "macros.h"
#include "dip.h"
#include "input.h"
#include "stcoli.h"

extern u8 lbl_14_bss_24;

typedef void (*BoatCamFn)(struct Camera *, struct Ball *);

struct BoatCamRange
{
    /*0x00*/ float start;
    /*0x04*/ float end;
    /*0x08*/ s8 type;
    /*0x0C*/ Vec pos;
}; /* size = 0x18 */

struct BoatViewport
{
    /*0x00*/ float x;
    /*0x04*/ float y;
    /*0x08*/ float w;
    /*0x0C*/ float h;
    /*0x10*/ s16 unk10;
    /*0x12*/ u16 fov;
}; /* size = 0x14 */

struct BoatViewportSet
{
    /*0x00*/ s32 count;
    /*0x04*/ struct BoatViewport *views;
}; /* size = 0x8 */

Vec lbl_14_bss_2A08;
s8 lbl_14_bss_2A14[5];
struct BoatViewport *lbl_14_bss_2A1C;
struct BoatViewport *lbl_14_bss_2A20[5];
struct BoatCamRange *lbl_14_bss_2A34;
s16 lbl_14_bss_2A38;
s8 lbl_14_bss_2A3A;
s8 lbl_14_bss_2A3B;
s32 lbl_14_bss_2A3C;
float lbl_14_bss_2A40;
float lbl_14_bss_2A44;
Vec lbl_14_bss_2A48;
float lbl_14_bss_2A54[5];
float lbl_14_bss_2A68;
float lbl_14_bss_2A6C;
float lbl_14_bss_2A70[2];
sptr lbl_14_bss_2A78;
sptr lbl_14_bss_2A7C;

char lbl_14_data_B58[] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF,
};

u32 lbl_14_data_B80[] = {
    0x31000000, 0x0, 0x3800B400, 0x10000, 0x1000000, 0x20000,
    0x10004C00, 0x30000, 0x1000000, 0x40000, 0x18000000, 0x90000,
    0x40000000, 0x0, 0x1000000, 0xB0000, 0x18000000, 0xC0000,
    0x18000000, 0x50000, 0x40000000, 0x0, 0x1000000, 0x70000,
    0x18000000, 0x80000, 0x3800B400, 0xD0000, 0x40000000, 0x0,
    0x18000000, 0x140000, 0x40000000, 0x0, 0x1000000, 0x160000,
    0x18000000, 0x170000, 0x18000000, 0x180000, 0x18000000, 0xF0000,
    0x40000000, 0x0, 0x1000000, 0x110000, 0x18000000, 0x120000,
    0x18000000, 0x130000, 0x18000000, 0x0, 0x18000000, 0x10000,
    0x18000000, 0x20000, 0x18000000, 0x30000, 0x18000000, 0x40000,
    0x18000000, 0x50000, 0x18000000, 0x60000, 0x18000000, 0x70000,
    0x18000000, 0x80000, 0x18000000, 0x90000, 0x18000000, 0xA0000,
    0x18000000, 0xB0000, 0x18000000, 0xC0000, 0x18000000, 0xD0000,
    0x18000000, 0xE0000, 0x18000000, 0xF0000,
};

BoatCamFn lbl_14_data_CC8[] = {
    fn_14_EAE0,
    fn_14_ED00,
    fn_14_F3D4,
    fn_14_F40C,
    fn_14_F5E8,
    fn_14_F620,
    fn_14_F8DC,
    fn_14_F928,
    fn_14_FA14,
    fn_14_FB5C,
    fn_14_FD40,
    fn_14_FDA8,
    fn_14_FEDC,
    fn_14_1003C,
    fn_14_102C4,
    fn_14_1032C,
    fn_14_1045C,
};

struct BoatCamRange lbl_14_data_D0C[] = {
    {0.0f, 3.85f, 11, -5.486f, 4.712f, -1.794f},
    {3.85f, 22.0f, 9, 3.0f, 1.0f, 0.0f},
    {22.0f, 35.0f, 11, 49.208f, 1.919f, -89.04f},
    {35.0f, 52.0f, 11, 91.422f, 11.31f, -65.577f},
    {52.0f, 7e+01f, 9, -3.0f, 1.0f, 0.0f},
    {7e+01f, 85.0f, 11, 37.722f, 1.518f, 52.079f},
    {85.0f, 1e+02f, 11, -5.486f, 4.712f, -1.794f},
    {0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f},
};

struct BoatCamRange lbl_14_data_DCC[] = {
    {0.0f, 2.5f, 11, 5.234f, 4.918f, 0.508f},
    {2.5f, 7.0f, 11, 0.418f, 0.995f, -56.558f},
    {7.0f, 13.5f, 11, -0.336f, 1.206f, -50.339f},
    {13.5f, 23.1f, 9, 3.0f, 1.0f, 0.0f},
    {23.1f, 33.0f, 9, -3.0f, 1.0f, 0.0f},
    {33.0f, 42.3f, 11, -51.418f, 2.009f, -129.566f},
    {42.3f, 51.0f, 11, 7.635f, 1.407f, -151.579f},
    {51.0f, 57.3f, 11, 45.903f, 2.194f, -131.34f},
    {57.3f, 62.4f, 11, 80.409f, 8.599f, -105.068f},
    {62.4f, 69.8f, 11, 107.001f, 10.25f, -99.334f},
    {69.8f, 79.8f, 11, 94.321f, 3.54f, -22.333f},
    {79.8f, 89.0f, 11, 70.355f, 2.632f, 43.049f},
    {89.0f, 1e+02f, 11, 5.234f, 4.918f, 0.508f},
    {0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f},
};

struct BoatCamRange lbl_14_data_F1C[] = {
    {0.0f, 7.0f, 11, -2.384f, 1.492f, -7.399f},
    {7.0f, 16.5f, 9, 4.0f, 1.0f, 0.0f},
    {16.5f, 20.7f, 11, 27.824f, 3.893f, -123.576f},
    {20.7f, 24.71f, 11, -5.476f, 8.143f, -94.702f},
    {24.71f, 29.2f, 11, -60.96f, 1.227f, -97.696f},
    {29.2f, 32.2f, 9, -4.0f, 1.0f, 0.0f},
    {32.2f, 35.87f, 11, -33.507f, 5.918f, -17.813f},
    {35.87f, 39.5f, 11, -29.614f, 5.801f, 44.272f},
    {39.5f, 43.4f, 9, 3.0f, 1.0f, 0.0f},
    {43.4f, 47.6f, 11, 101.065f, 3.865f, 65.954f},
    {48.6f, 53.0f, 9, -3.0f, 1.0f, 0.0f},
    {53.0f, 56.8f, 11, 75.56f, 2.95f, 40.576f},
    {56.8f, 57.78f, 11, 34.246f, 1.387f, 5.369f},
    {57.78f, 63.0f, 11, 30.186f, -0.356f, 8.366f},
    {63.0f, 67.72f, 11, 101.969f, -33.208f, -129.97f},
    {67.72f, 72.0f, 11, 60.36f, -45.47f, -225.697f},
    {72.0f, 77.0f, 9, 4.0f, 1.0f, 0.0f},
    {77.0f, 8e+01f, 11, -29.602f, -72.604f, -295.811f},
    {72.0f, 9e+01f, 9, 4.0f, 1.0f, 0.0f},
    {9e+01f, 1e+02f, 11, -16.885f, -137.562f, -475.297f},
    {0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f},
};

struct BoatCamRange *lbl_14_data_1114[] = {lbl_14_data_D0C, lbl_14_data_DCC, lbl_14_data_F1C};

float lbl_14_data_1120 = 1e+01f;
float lbl_14_data_1124 = 4.0f;

struct BoatViewport lbl_14_data_1128[] = {
    {0.0f, 0.0f, 1.0f, 1.0f, -3328, 0x2AAA},
};

struct BoatViewport lbl_14_data_113C[] = {
    {0.0f, 0.0f, 0.5f, 0.5f, -2432, 0x238E},
    {0.5f, 0.0f, 0.5f, 0.5f, -2432, 0x238E},
    {0.0f, 0.5f, 0.5f, 0.5f, -2432, 0x238E},
    {0.5f, 0.5f, 0.5f, 0.5f, -2432, 0x238E},
};

struct BoatViewportSet lbl_14_data_118C[] = {
    {1, lbl_14_data_1128},
    {-1, NULL},
    {-1, NULL},
    {4, lbl_14_data_113C},
};

struct BoatViewport lbl_14_data_11AC[] = {
    {0.0f, 0.0f, 1.0f, 0.5f, -2432, 0x238E},
    {0.0f, 0.5f, 1.0f, 0.5f, -2432, 0x238E},
};

struct BoatViewport lbl_14_data_11D4[] = {
    {0.0f, 0.0f, 0.5f, 1.0f, -3328, 12743},
    {0.5f, 0.0f, 0.5f, 1.0f, -3328, 12743},
};

struct BoatViewportSet lbl_14_data_11FC[] = {
    {2, lbl_14_data_11AC},
    {2, lbl_14_data_11D4},
    {2, lbl_14_data_11D4},
};

struct BoatViewport lbl_14_data_1214[] = {
    {0.0f, 0.0f, 1.0f, 0.5f, -2432, 0x238E},
    {0.0f, 0.5f, 0.5f, 0.5f, -2432, 0x238E},
    {0.5f, 0.5f, 0.5f, 0.5f, -2432, 0x238E},
};

struct BoatViewport lbl_14_data_1250[] = {
    {0.0f, 0.5f, 0.5f, 0.5f, -2432, 0x238E},
    {0.0f, 0.0f, 1.0f, 0.5f, -2432, 0x238E},
    {0.5f, 0.5f, 0.5f, 0.5f, -2432, 0x238E},
};

struct BoatViewport lbl_14_data_128C[] = {
    {0.0f, 0.0f, 0.5f, 0.5f, -2432, 0x238E},
    {0.5f, 0.0f, 0.5f, 0.5f, -2432, 0x238E},
    {0.0f, 0.5f, 1.0f, 0.5f, -2432, 0x238E},
};

struct BoatViewport lbl_14_data_12C8[] = {
    {0.0f, 0.0f, 0.5f, 0.5f, -2432, 0x238E},
    {0.5f, 0.0f, 0.5f, 0.5f, -2432, 0x238E},
    {0.0f, 0.5f, 0.5f, 0.5f, -2432, 0x238E},
    {0.5f, 0.5f, 0.5f, 0.5f, -2432, 0x238E},
};

struct BoatViewportSet lbl_14_data_1318[] = {
    {3, lbl_14_data_1214},
    {3, lbl_14_data_1250},
    {3, lbl_14_data_128C},
    {4, lbl_14_data_12C8},
};

float lbl_14_data_1338 = 0.001f;
float lbl_14_data_133C = 1.0f;

/* stripped at link; pins the .bss order, which mwcc assigns by first use */
void boat_cam_bss_layout(void)
{
    lbl_14_bss_2A08.x = 0.0f;
    lbl_14_bss_2A14[0] = 0;
    lbl_14_bss_2A1C = NULL;
    lbl_14_bss_2A20[0] = NULL;
    lbl_14_bss_2A34 = NULL;
    lbl_14_bss_2A38 = 0;
    lbl_14_bss_2A3A = 0;
    lbl_14_bss_2A3B = 0;
    lbl_14_bss_2A3C = 0;
    lbl_14_bss_2A40 = 0.0f;
    lbl_14_bss_2A44 = 0.0f;
    lbl_14_bss_2A48.x = 0.0f;
    lbl_14_bss_2A54[0] = 0.0f;
    lbl_14_bss_2A68 = 0.0f;
    lbl_14_bss_2A6C = 0.0f;
    lbl_14_bss_2A70[0] = 0.0f;
    lbl_14_bss_2A78 = 0;
    lbl_14_bss_2A7C = 0;
}

void fn_14_E0FC(void)
{
    struct Camera *camera;
    s16 i;

    i = 0;
    camera = cameras;
    while (i < 5) {
        camera->unk1F = 0;
        lbl_14_bss_2A14[i] = -1;
        i++;
        camera++;
    }
}

void fn_14_E148(struct Camera *camera, struct Ball *ball)
{
    s8 *mode;

    if ((u32)(g_some_other_flags & 0xA) != 0) {
        if ((u32)(dip_switches & 1) != 0 && (u32)(lbl_1_bss_D4328 & 1) != 0) {
            fn_14_1045C(camera, ball);
        }
        return;
    }
    mode = &lbl_14_bss_2A14[camera->idx];
    if (*mode >= 0 && *mode < 0x11) {
        camera->unk1F = *mode;
        *mode = -1;
    }
    lbl_14_bss_29A8 = &lbl_14_bss_208[camera->idx];
    lbl_14_bss_2A1C = lbl_14_bss_2A20[camera->idx];
    camera->sub28.view.fov = lbl_14_bss_2A1C->fov;
    lbl_14_data_CC8[camera->unk1F](camera, ball);
    if ((u32)(dip_switches & 1) != 0 && (u32)(lbl_1_bss_D4328 & 1) != 0) {
        fn_14_1045C(camera, ball);
    }
}

void fn_14_E2C4(void)
{
    struct BoatViewportSet *set;
    struct BoatViewport *view;
    s16 i;

    switch (modeCtrl.unk30) {
    case 2:
        set = &lbl_14_data_11FC[modeCtrl.wideMode];
        break;
    case 3:
        set = &lbl_14_data_1318[modeCtrl.splitscreenMode];
        break;
    default:
        set = &lbl_14_data_118C[modeCtrl.unk30 - 1];
        break;
    }
    for (i = 0, view = set->views; i < set->count; i++, view++) {
        lbl_14_bss_2A20[i] = view;
        set_camera_view_settings(i, view->x, view->y, view->w, view->h);
    }
}

void fn_14_E3CC(struct Camera *camera, s8 mode)
{
    lbl_14_bss_2A14[camera->idx] = mode;
}

void fn_14_E3E4(void)
{
    s16 i;
    struct BoatCamRange *range;
    float zero = 0.0f;

    lbl_14_bss_2A34 = lbl_14_data_1114[lbl_14_bss_20.unk0];
    lbl_14_bss_2A38 = -1;
    i = 0;
    range = lbl_14_bss_2A34;
    while (range->type >= 0) {
        if (range->start <= zero && zero <= range->end) {
            break;
        }
        i++;
        range++;
    }
    lbl_14_bss_2A38 = i;
    switch (range->type) {
    case 10:
        break;
    case 9:
        lbl_14_bss_2A14[0] = range->type - 1;
        lbl_14_data_1120 = range->pos.x;
        lbl_14_data_1124 = range->pos.y;
        break;
    case 11:
        lbl_14_bss_2A14[0] = range->type - 1;
        lbl_14_bss_2A08 = range->pos;
        break;
    }
    g_set_smth_with_sound(1);
    fn_14_E50C();
}

void fn_14_E50C(void)
{
    struct BoatPlayer *player;
    float dist;
    struct BoatCamRange *range;
    struct BoatCamRange *prev;
    s16 i;

    player = lbl_14_bss_208;
    dist = player->unk2DC;
    prev = &lbl_14_bss_2A34[lbl_14_bss_2A38];
    i = 0;
    range = lbl_14_bss_2A34;
    while (range->type >= 0) {
        if (range->start <= dist && dist <= range->end) {
            break;
        }
        i++;
        range++;
    }
    if (i != lbl_14_bss_2A38) {
        lbl_14_bss_2A38 = i;
        switch (range->type) {
        case 10:
            break;
        case 9:
            if (prev->pos.x != range->pos.x && prev->pos.y != range->pos.y) {
                lbl_14_bss_2A14[0] = range->type - 1;
            }
            lbl_14_data_1120 = range->pos.x;
            lbl_14_data_1124 = range->pos.y;
            break;
        case 11:
            lbl_14_bss_2A14[0] = range->type - 1;
            lbl_14_bss_2A08 = range->pos;
            break;
        }
    }
}

void fn_14_E638(void)
{
    s16 i;
    struct BoatCamRange *range;
    float zero = 0.0f;

    lbl_14_bss_2A3A = 0;
    lbl_14_bss_2A3B = -1;
    lbl_14_bss_2A34 = lbl_14_data_1114[lbl_14_bss_20.unk0];
    lbl_14_bss_2A38 = -1;
    i = 0;
    range = lbl_14_bss_2A34;
    while (range->type >= 0) {
        if (range->start <= zero && zero <= range->end) {
            break;
        }
        i++;
        range++;
    }
    lbl_14_bss_2A38 = i;
    switch (range->type) {
    case 10:
        break;
    case 9:
        lbl_14_bss_2A14[3] = 0xC;
        lbl_14_data_1120 = range->pos.x;
        lbl_14_data_1124 = range->pos.y;
        break;
    case 11:
        lbl_14_bss_2A14[3] = 0xE;
        lbl_14_bss_2A08 = range->pos;
        break;
    }
    fn_14_E760();
}

void fn_14_E760(void)
{
    struct BoatPlayer *player;
    float dist;
    struct BoatCamRange *range;
    struct BoatCamRange *prev;
    s16 i;

    player = lbl_14_bss_208;
    i = 0;
    while (i < lbl_14_bss_24) {
        player = lbl_14_bss_2988[i];
        if (!(player->unk8 & 8)) {
            break;
        }
        i++;
    }
    if (lbl_14_bss_2A3B < 0 && lbl_14_bss_2A3A != player->unk0) {
        lbl_14_bss_2A3B = player->unk0;
        if (lbl_14_bss_208[lbl_14_bss_2A3B].unk304 - lbl_14_bss_208[lbl_14_bss_2A3A].unk304 < -20.0) {
            lbl_14_bss_2A3C = 1;
        } else {
            lbl_14_bss_2A3C = 0.5 * lbl_80198B34;
        }
    }
    if (lbl_14_bss_2A3B >= 0) {
        struct Ball *a = &balls[lbl_14_bss_2A3A];
        struct Ball *b = &balls[lbl_14_bss_2A3B];
        lbl_14_bss_2A3C--;
        lbl_14_bss_2A40 = (float)lbl_14_bss_2A3C / (0.5 * lbl_80198B34);
        lbl_14_bss_2A44 = 1.0 - lbl_14_bss_2A40;
        lbl_14_bss_2A48.x = a->pos.x * lbl_14_bss_2A40 + b->pos.x * lbl_14_bss_2A44;
        lbl_14_bss_2A48.y = a->pos.y * lbl_14_bss_2A40 + b->pos.y * lbl_14_bss_2A44;
        lbl_14_bss_2A48.z = a->pos.z * lbl_14_bss_2A40 + b->pos.z * lbl_14_bss_2A44;
        if (lbl_14_bss_2A3C == 0) {
            lbl_14_bss_2A3A = lbl_14_bss_2A3B;
            lbl_14_bss_2A3B = -1;
        }
    } else {
        lbl_14_bss_2A48 = balls[lbl_14_bss_2A3A].pos;
    }
    dist = player->unk2DC;
    prev = &lbl_14_bss_2A34[lbl_14_bss_2A38];
    i = 0;
    range = lbl_14_bss_2A34;
    while (range->type >= 0) {
        if (range->start <= dist && dist <= range->end) {
            break;
        }
        i++;
        range++;
    }
    if (i != lbl_14_bss_2A38) {
        lbl_14_bss_2A38 = i;
        switch (range->type) {
        case 10:
            break;
        case 9:
            if (prev->pos.x != range->pos.x && prev->pos.y != range->pos.y) {
                lbl_14_bss_2A14[3] = 0xC;
            }
            lbl_14_data_1120 = range->pos.x;
            lbl_14_data_1124 = range->pos.y;
            break;
        case 11:
            lbl_14_bss_2A14[3] = 0xE;
            lbl_14_bss_2A08 = range->pos;
            break;
        }
    }
}

void fn_14_EAE0(struct Camera *camera, struct Ball *ball)
{
    float *offset;
    Vec dir;

    offset = &lbl_14_bss_2A54[camera->idx];
    camera_clear(camera);
    camera->unk26 = 0xD;
    if (0.5 == camera->sub28.viewportW && 0.5 == camera->sub28.viewportH) {
        *offset = 0.7f;
    } else if (0.5 == camera->sub28.viewportH) {
        *offset = 0.7f;
    } else {
        *offset = 1.0f;
    }
    camera->lookAt = (Vec){ball->pos.x, ball->pos.y + *offset, ball->pos.z};
    fn_14_11524(lbl_14_bss_70, lbl_14_bss_29A8->unk2DC + 10.0 * lbl_14_bss_2AE8->unkC, &dir);
    PSVECSubtract(&dir, (Vec *)&lbl_14_bss_29A8->unk2E4, &dir);
    mtxa_from_rotate_y(math_atan2(dir.x, dir.z) + 0x8000);
    mtxa_rotate_x(-0x880);
    mtxa_tf_vec_xyz(0.0f, 0.0f, 3.0f, &camera->unkB4);
    lbl_14_bss_2A68 = camera->unkB4.y;
    lbl_14_bss_2A6C = mathutil_vec_len(&(Vec){camera->unkB4.x, 0.0f, camera->unkB4.z});
    PSVECAdd(&camera->lookAt, &camera->unkB4, &camera->eye);
    lbl_14_bss_2A14[camera->idx] = 1;
    fn_14_ED00(camera, ball);
}

void fn_14_ED00(struct Camera *camera, struct Ball *ball)
{
    Vec prevEye;
    Vec prevLookAt;
    Vec dir;
    Vec a;
    Vec b;
    Vec ballXZ;
    Vec playerXZ;
    Vec p340;
    Vec p34C;
    Vec d1;
    Vec d2;
    Vec lookDir;
    struct RaycastHit hit;
    s16 rot;
    float len;
    float len2;
    float y;
    s32 found;

    prevEye = camera->eye;
    prevLookAt = camera->lookAt;
    camera->lookAt = (Vec){ball->pos.x, ball->pos.y + lbl_14_bss_2A54[camera->idx], ball->pos.z};
    fn_14_11524(lbl_14_bss_70, lbl_14_bss_29A8->unk2DC + 10.0 * lbl_14_bss_2AE8->unkC, &dir);
    PSVECSubtract(&dir, (Vec *)&lbl_14_bss_29A8->unk2E4, &dir);
    if (lbl_14_bss_20.unk0 == 2) {
        if (lbl_14_bss_29A8->unk2DC + 10.0 * lbl_14_bss_2AE8->unkC >= 100.0 - lbl_14_bss_2AE8->unkC) {
            fn_14_11524(lbl_14_bss_70, 100.0 - 3.0 * lbl_14_bss_2AE8->unkC, &a);
            fn_14_11524(lbl_14_bss_70, 100.0 - 2.0 * lbl_14_bss_2AE8->unkC, &b);
            PSVECSubtract(&b, &a, &dir);
        }
    }
    mtxa_from_rotate_y(math_atan2(dir.x, dir.z) + 0x8000);
    ballXZ = (Vec){ball->pos.x, 0.0f, ball->pos.z};
    playerXZ = (Vec){(*(Vec *)&lbl_14_bss_29A8->unk2E4).x, 0.0f, (*(Vec *)&lbl_14_bss_29A8->unk2E4).z};
    p340 = (Vec){(*(Vec *)&lbl_14_bss_29A8->unk340).x, 0.0f, (*(Vec *)&lbl_14_bss_29A8->unk340).z};
    p34C = (Vec){(*(Vec *)&lbl_14_bss_29A8->unk34C).x, 0.0f, (*(Vec *)&lbl_14_bss_29A8->unk34C).z};
    rot = 0x400;
    PSVECSubtract(&ballXZ, &playerXZ, &d1);
    len = vec_normalize_len(&d1);
    PSVECSubtract(&p34C, &playerXZ, &d2);
    len2 = vec_normalize_len(&d2);
    if (mathutil_vec_dot_prod(&d1, &d2) < 0.0) {
        PSVECSubtract(&p340, &playerXZ, &d2);
        len2 = vec_normalize_len(&d2);
        rot = -rot;
    }
    len2 *= 0.8;
    if ((len - len2) / len2 > 0.5) {
        mtxa_rotate_y((float)rot * (len - len2));
    }
    mtxa_tf_vec_xyz(0.0f, lbl_14_bss_2A68, lbl_14_bss_2A6C, &camera->unkB4);
    PSVECAdd(&camera->unkB4, &camera->lookAt, &camera->unkB4);
    found = fn_14_13CA8(&camera->unkB4, &hit, 2);
    if (found == 0) {
        dir = camera->unkB4;
        dir.y += 3.0;
        found = raycast_stage_down(&dir, &hit, NULL);
    }
    if (found != 0) {
        y = lbl_14_bss_2A68 + (hit.pos.y + lbl_14_bss_2A54[camera->idx]);
        if (y > camera->unkB4.y) {
            camera->unkB4.y = y;
        }
    }
    PSVECSubtract(&camera->unkB4, &camera->eye, &camera->eyeVel);
    PSVECScale(&camera->eyeVel, &camera->eyeVel, 1.0 / (0.5 * lbl_80198B34));
    PSVECAdd(&camera->eye, &camera->eyeVel, &camera->eye);
    PSVECSubtract(&camera->eye, &camera->lookAt, &camera->eye);
    y = camera->eye.y;
    vec_set_len(lbl_14_bss_2A6C, &camera->eye, &camera->eye);
    camera->eye.y = y;
    PSVECAdd(&camera->eye, &camera->lookAt, &camera->eye);
    if (ball->unk154 >= 5) {
        found = fn_14_13CA8(&camera->eye, &hit, 2);
        if (found == 0) {
            dir = camera->eye;
            dir.y += 3.0;
            found = raycast_stage_down(&dir, &hit, NULL);
        }
        if (found != 0) {
            y = lbl_14_bss_2A68 + (hit.pos.y + lbl_14_bss_2A54[camera->idx]);
            if (y > camera->lookAt.y) {
                camera->eye.y += (y - camera->eye.y) / (0.1 * lbl_80198B34);
            }
        }
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &lookDir);
    camera->rotY = math_atan2(lookDir.x, lookDir.z) - 0x8000;
    camera->rotX = math_atan2(lookDir.y, math_sqrt(mathutil_sum_of_sq_2(lookDir.x, lookDir.z)));
    camera->rotZ = 0;
}

void fn_14_F3D4(struct Camera *camera, struct Ball *ball)
{
    lbl_14_bss_2A14[camera->idx] = 3;
    fn_14_F40C(camera, ball);
}

void fn_14_F40C(struct Camera *camera, struct Ball *ball)
{
    Vec d;
    Vec dir;
    float len;

    PSVECSubtract(&ball->pos, &camera->lookAt, &d);
    len = mathutil_vec_len(&d);
    if (len > 0.0f) {
        len /= 50.0;
        if (len > 0.3) {
            len = 0.3f;
        }
        PSVECScale(&d, &d, len);
        PSVECAdd(&camera->lookAt, &d, &camera->lookAt);
    }
    mtxa_from_rotate_y(lbl_14_bss_29A8->unk3E2);
    mtxa_tf_vec(&(Vec){0.0f, 0.0f, -3.0f}, &camera->unkB4);
    PSVECAdd(&camera->unkB4, &ball->pos, &camera->unkB4);
    camera->unkB4.y += 1.0;
    PSVECSubtract(&camera->unkB4, &camera->eye, &d);
    len = mathutil_vec_len(&d);
    if (len > 0.0f) {
        len /= 50.0;
        if (len > 0.3) {
            len = 0.3f;
        }
        PSVECScale(&d, &d, len);
        PSVECAdd(&camera->eye, &d, &camera->eye);
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &dir);
    camera->rotY = math_atan2(dir.x, dir.z) - 0x8000;
    camera->rotX = math_atan2(dir.y, math_sqrt(mathutil_sum_of_sq_2(dir.x, dir.z)));
    camera->rotZ = 0;
}

void fn_14_F5E8(struct Camera *camera, struct Ball *ball)
{
    lbl_14_bss_2A14[camera->idx] = 5;
    fn_14_F620(camera, ball);
}

void fn_14_F620(struct Camera *camera, struct Ball *ball)
{
    Vec d;
    float dot;
    float f;
    float len;

    d.x = ball->pos.x - camera->eye.x;
    d.y = 0.0f;
    d.z = ball->pos.z - camera->eye.z;
    dot = mathutil_vec_dot_prod(&d, &camera->eyeVel);
    vec_normalize_len(&d);
    camera->eyeVel.y *= 0.97;
    f = -0.01 * dot;
    camera->eyeVel.x += f * d.x;
    camera->eyeVel.z += f * d.z;
    if (f < 0.0) {
        f *= 0.5;
        camera->eyeVel.x += f * d.z;
        camera->eyeVel.z += f * d.x;
    }
    PSVECAdd(&camera->eye, &camera->eyeVel, &camera->eye);
    PSVECSubtract(&ball->pos, &camera->lookAt, &camera->lookAtVel);
    PSVECScale(&camera->lookAtVel, &camera->lookAtVel, 0.3f);
    PSVECAdd(&camera->lookAt, &camera->lookAtVel, &camera->lookAt);
    d.x = camera->eye.x - camera->lookAt.x;
    d.y = 0.0f;
    d.z = camera->eye.z - camera->lookAt.z;
    len = math_sqrt(mathutil_sum_of_sq_2(d.x, d.z));
    if (len > FLT_EPSILON) {
        f = (len + 0.08 * (2.0 - len)) / len;
        camera->eye.x = camera->lookAt.x + d.x * f;
        camera->eye.z = camera->lookAt.z + d.z * f;
        if (!(ball->flags & 0x200LL)) {
            d.y = camera->lookAt.y - camera->eye.y;
            camera->eye.y += 0.01 * d.y;
        }
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &d);
    camera->rotY = math_atan2(d.x, d.z) - 0x8000;
    camera->rotX = math_atan2(d.y, math_sqrt(mathutil_sum_of_sq_2(d.x, d.z)));
    camera->rotZ = 0;
}

void fn_14_F8DC(struct Camera *camera, struct Ball *ball)
{
    camera->unkC0 = lbl_14_bss_29A8->unk3E2;
    lbl_14_bss_2A14[camera->idx] = 7;
    fn_14_F928(camera, ball);
}

void fn_14_F928(struct Camera *camera, struct Ball *ball)
{
    Vec d;

    camera->lookAt = ball->pos;
    camera->unkC0 += 0x50;
    mtxa_from_rotate_y(camera->unkC0);
    mtxa_rotate_x(-0x1000);
    mtxa_tf_vec_xyz(0.0f, 0.0f, 2.5f, &camera->eye);
    PSVECAdd(&camera->eye, &camera->lookAt, &camera->eye);
    PSVECSubtract(&camera->lookAt, &camera->eye, &d);
    camera->rotY = math_atan2(d.x, d.z) - 0x8000;
    camera->rotX = math_atan2(d.y, math_sqrt(mathutil_sum_of_sq_2(d.x, d.z)));
    camera->rotZ = 0;
}

void fn_14_FA14(struct Camera *camera, struct Ball *ball)
{
    s16 i;

    camera->lookAt = (Vec){0.0f, 0.0f, 0.0f};
    camera->eye = camera->lookAt;
    camera->lookAt = balls[0].pos;
    fn_14_11524(lbl_14_bss_70, lbl_14_data_1120 * lbl_14_bss_2AE8->unkC, &camera->eye);
    camera->eye.y += lbl_14_data_1124;
    camera->flags &= ~4;
    camera->flags |= 8;
    camera->unk1F = 9;
    for (i = 0; i < 3; i++) {
        fn_14_FB5C(camera, ball);
    }
}

void fn_14_FB5C(struct Camera *camera, struct Ball *ball)
{
    struct BoatPlayer *player;
    float len;
    Vec target;
    Vec d;

    player = lbl_14_bss_208;
    PSVECSubtract(&balls[0].pos, &camera->lookAt, &d);
    len = mathutil_vec_len(&d);
    if (len > 0.0f) {
        len /= 20.0;
        if (len > 0.9) {
            len = 0.9f;
        }
        PSVECScale(&d, &d, len);
        PSVECAdd(&camera->lookAt, &d, &camera->lookAt);
    }
    fn_14_11524(lbl_14_bss_70, player->unk2DC + lbl_14_data_1120 * lbl_14_bss_2AE8->unkC, &target);
    target.y += lbl_14_data_1124;
    PSVECSubtract(&target, &camera->eye, &d);
    len = mathutil_vec_len(&d);
    if (len > 0.0f) {
        len /= 20.0;
        if (len > 0.9) {
            len = 0.9f;
        }
        PSVECScale(&d, &d, len);
        PSVECAdd(&camera->eye, &d, &camera->eye);
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &d);
    camera->rotY = math_atan2(d.x, d.z) - 0x8000;
    camera->rotX = math_atan2(d.y, math_sqrt(mathutil_sum_of_sq_2(d.x, d.z)));
    camera->rotZ = 0;
}

void fn_14_FD40(struct Camera *camera, struct Ball *ball)
{
    camera->lookAt = balls[0].pos;
    camera->flags &= ~4;
    camera->flags |= 8;
    camera->unk1F = 0xB;
    fn_14_FDA8(camera, ball);
}

void fn_14_FDA8(struct Camera *camera, struct Ball *ball)
{
    Vec d;

    PSVECSubtract(&balls[0].pos, &camera->lookAt, &d);
    if (mathutil_vec_len(&d) > 0.8) {
        vec_set_len(0.8f, &d, &d);
        PSVECAdd(&camera->lookAt, &d, &camera->lookAt);
    } else {
        camera->lookAt = balls[0].pos;
    }
    camera->eye = lbl_14_bss_2A08;
    PSVECSubtract(&camera->lookAt, &camera->eye, &d);
    camera->rotY = math_atan2(d.x, d.z) - 0x8000;
    camera->rotX = math_atan2(d.y, math_sqrt(mathutil_sum_of_sq_2(d.x, d.z)));
    camera->rotZ = 0;
}

void fn_14_FEDC(struct Camera *camera, struct Ball *ball)
{
    s16 i;

    camera->lookAt = (Vec){0.0f, 0.0f, 0.0f};
    camera->eye = camera->lookAt;
    camera->lookAt = balls[lbl_14_bss_2A3A].pos;
    fn_14_11524(lbl_14_bss_70, lbl_14_data_1120 * lbl_14_bss_2AE8->unkC, &camera->eye);
    camera->eye.y += lbl_14_data_1124;
    camera->flags &= ~4;
    camera->flags |= 8;
    camera->unk1F = 0xD;
    for (i = 0; i < 3; i++) {
        fn_14_1003C(camera, ball);
    }
}

void fn_14_1003C(struct Camera *camera, struct Ball *ball)
{
    Vec target;
    Vec d;
    float len;

    PSVECSubtract(&lbl_14_bss_2A48, &camera->lookAt, &d);
    len = mathutil_vec_len(&d);
    if (len > 0.0f) {
        len /= 20.0;
        if (len > 0.9) {
            len = 0.9f;
        }
        PSVECScale(&d, &d, len);
        PSVECAdd(&camera->lookAt, &d, &camera->lookAt);
    }
    if (lbl_14_bss_2A3B >= 0) {
        fn_14_11524(lbl_14_bss_70,
                    lbl_14_data_1120 * lbl_14_bss_2AE8->unkC +
                        (lbl_14_bss_2A40 * lbl_14_bss_208[lbl_14_bss_2A3A].unk2DC +
                         lbl_14_bss_2A44 * lbl_14_bss_208[lbl_14_bss_2A3B].unk2DC),
                    &target);
    } else {
        fn_14_11524(lbl_14_bss_70, lbl_14_bss_208[lbl_14_bss_2A3A].unk2DC + lbl_14_data_1120 * lbl_14_bss_2AE8->unkC, &target);
    }
    target.y += lbl_14_data_1124;
    PSVECSubtract(&target, &camera->eye, &d);
    len = mathutil_vec_len(&d);
    if (len > 0.0f) {
        len /= 20.0;
        if (len > 0.9) {
            len = 0.9f;
        }
        PSVECScale(&d, &d, len);
        PSVECAdd(&camera->eye, &d, &camera->eye);
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &d);
    camera->rotY = math_atan2(d.x, d.z) - 0x8000;
    camera->rotX = math_atan2(d.y, math_sqrt(mathutil_sum_of_sq_2(d.x, d.z)));
    camera->rotZ = 0;
}

void fn_14_102C4(struct Camera *camera, struct Ball *ball)
{
    camera->lookAt = lbl_14_bss_2A48;
    camera->flags &= ~4;
    camera->flags |= 8;
    camera->unk1F = 0xF;
    fn_14_1032C(camera, ball);
}

void fn_14_1032C(struct Camera *camera, struct Ball *ball)
{
    Vec d;

    PSVECSubtract(&lbl_14_bss_2A48, &camera->lookAt, &d);
    if (mathutil_vec_len(&d) > 0.8) {
        vec_set_len(0.8f, &d, &d);
        PSVECAdd(&camera->lookAt, &d, &camera->lookAt);
    } else {
        camera->lookAt = lbl_14_bss_2A48;
    }
    camera->eye = lbl_14_bss_2A08;
    PSVECSubtract(&camera->lookAt, &camera->eye, &d);
    camera->rotY = math_atan2(d.x, d.z) - 0x8000;
    camera->rotX = math_atan2(d.y, math_sqrt(mathutil_sum_of_sq_2(d.x, d.z)));
    camera->rotZ = 0;
}

void fn_14_1045C(struct Camera *camera, struct Ball *ball)
{
    Vec d;

    if ((analog_inputs[0][0] & 0x100) && (analog_inputs[0][0] & 0x200) && (pad_status_groups[0].unk0[2].button & 0x200)) {
        lbl_14_data_133C = -lbl_14_data_133C;
    }
    lbl_14_bss_2A70[0] += 0.035 * (pad_status_groups[0].unk0[0].stickY / 60.0) * lbl_14_data_133C;
    lbl_14_data_1338 += 0.02 * (pad_status_groups[0].unk0[0].substickY / 60.0);
    if (lbl_14_data_1338 < 0.001) {
        lbl_14_data_1338 = 0.001f;
    }
    if (lbl_14_bss_20.unk0 == 2) {
        if (lbl_14_bss_2A70[0] < 0.0) {
            lbl_14_bss_2A70[0] = 0.0f;
        } else if (lbl_14_bss_2A70[0] > 99.999) {
            lbl_14_bss_2A70[0] = 99.999f;
        }
        if (lbl_14_data_133C > 0.0f) {
            if (lbl_14_bss_2A70[0] + lbl_14_data_1338 >= 99.999) {
                lbl_14_bss_2A70[0] = 99.999 - lbl_14_data_1338;
            }
        } else {
            if (lbl_14_bss_2A70[0] - lbl_14_data_1338 <= 0.0001) {
                lbl_14_bss_2A70[0] = 0.001 + lbl_14_data_1338;
            }
        }
    } else if (lbl_14_bss_2A70[0] < 0.0) {
        lbl_14_bss_2A70[0] = 100.0 - lbl_14_bss_2A70[0];
    } else if (lbl_14_bss_2A70[0] >= 100.0) {
        lbl_14_bss_2A70[0] = lbl_14_bss_2A70[0] - 100.0;
    }
    fn_14_11524((struct StageExtraChannels *)stagedef->extraData, lbl_14_bss_2A70[0], &camera->eye);
    fn_14_11524((struct StageExtraChannels *)stagedef->extraData, lbl_14_bss_2A70[0] + lbl_14_data_1338 * lbl_14_data_133C, &camera->lookAt);
    PSVECSubtract(&camera->lookAt, &camera->eye, &camera->lookAt);
    vec_set_len(10.0f, &camera->lookAt, &camera->lookAt);
    PSVECAdd(&camera->lookAt, &camera->eye, &camera->lookAt);
    camera->eye.y += 3.2;
    camera->lookAt.y += 0.5;
    PSVECSubtract(&camera->lookAt, &camera->eye, &d);
    camera->rotY = math_atan2(d.x, d.z) - 0x8000;
    camera->rotX = math_atan2(d.y, math_sqrt(mathutil_sum_of_sq_2(d.x, d.z)));
    camera->rotZ = 0;
}

