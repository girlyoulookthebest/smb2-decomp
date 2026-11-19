/* Monkey Fight 2: the floating background object (.text 0x29F68-0x2B3BC).
 * A model group (GMA models 145-147) that drifts and bobs in front of the camera, a black
 * full-screen fade quad and a striped fade-in band. fn_12_2B350/2B374/2B398 trigger
 * its approach, leave and jump sequences. */
#include "fight.h"

/* GX prototypes not in the shared headers */
void g_something_with_GX_vtx_desc(u32 attrs);
void g_something_with_raster_state(int a);
void GXSetCullMode_cached(int mode);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetNumTevStages_cached(u8 n);
extern struct GMA *minigameGma;

/* State of the background object (lbl_12_bss_1F96C). */
struct Fight10Work
{
    /*0x00*/ s16 state;      /* approach/leave sequence */
    /*0x02*/ s16 timer;
    /*0x04*/ u8 filler4[8];
    /*0x0C*/ float fade;     /* 0 hidden .. 1 fully shown */
    /*0x10*/ float fadeStep;
    /*0x14*/ float fadeTarget;
    /*0x18*/ Vec pos;        /* view space */
    /*0x24*/ Vec targetPos;
    /*0x30*/ Vec rot;        /* s16 angles kept as floats */
    /*0x3C*/ Vec rotVel;
    /*0x48*/ s16 jumpState;
    /*0x4A*/ s16 jumpTimer;
    /*0x4C*/ Vec swayPos;
    /*0x58*/ Vec swayVel;
    /*0x64*/ float spin;     /* s16 angle kept as a float */
    /*0x68*/ float spinVel;
};  /* size 0x6C */

/* A screen-space rectangle at depth z (view space). */
struct Fight10Rect
{
    float x1;
    float y1;
    float x2;
    float y2;
    float z;
};

/* .bss */
float lbl_12_bss_1F968;  /* resting distance of the object from the camera */
struct Fight10Work lbl_12_bss_1F96C;

void fn_12_2AD5C(void);
void fn_12_2AF6C(struct Fight10Rect *rect);
void fn_12_2B160(void);

/* Resets the object: parked in front of the camera, random sway and spin. */
void fn_12_29F68(void)
{
    struct GMAModel *model = minigameGma->modelEntries[145].model;

    lbl_12_bss_1F968 = -model->boundSphereRadius / math_tan(0x1555);
    memset(&lbl_12_bss_1F96C, 0, sizeof(lbl_12_bss_1F96C));
    lbl_12_bss_1F96C.state = 0;
    lbl_12_bss_1F96C.pos.x = 0.0f;
    lbl_12_bss_1F96C.pos.y = 0.0f;
    lbl_12_bss_1F96C.pos.z = lbl_12_bss_1F968;
    lbl_12_bss_1F96C.targetPos = lbl_12_bss_1F96C.pos;
    lbl_12_bss_1F96C.swayPos.x = 10.0f * RAND_FLOAT();
    lbl_12_bss_1F96C.spin = 12288.0f * RAND_FLOAT();
}

/* Per-frame update: approach/leave sequences, fade, wobble and jump physics. */
void fn_12_2A0CC(void)
{
    Vec v;

    switch (lbl_12_bss_1F96C.state)
    {
    case 1:
        lbl_12_bss_1F96C.state = 2;
        lbl_12_bss_1F96C.fadeTarget = 1.0f;
        lbl_12_bss_1F96C.fadeStep = 1.0f / 60.0f;
        lbl_12_bss_1F96C.targetPos.z = 2.0f * lbl_12_bss_1F968;
        lbl_12_bss_1F96C.targetPos.x = 1.3333334f * (0.0f * (lbl_12_bss_1F96C.targetPos.z * math_tan(0x1555)));
        lbl_12_bss_1F96C.targetPos.y = 0.25f * (lbl_12_bss_1F96C.targetPos.z * math_tan(0x1555));
    case 2:
        lbl_12_bss_1F96C.timer--;
        if (lbl_12_bss_1F96C.timer <= 0)
            lbl_12_bss_1F96C.state = 3;
        break;
    case 3:
        lbl_12_bss_1F96C.state = 4;
        lbl_12_bss_1F96C.targetPos.z = 1.5f * lbl_12_bss_1F968;
        lbl_12_bss_1F96C.targetPos.x = 1.3333334f * (0.0f * (lbl_12_bss_1F96C.targetPos.z * math_tan(0x1555)));
        lbl_12_bss_1F96C.targetPos.y = 0.25f * (lbl_12_bss_1F96C.targetPos.z * math_tan(0x1555));
        lbl_12_bss_1F96C.fadeTarget = 0.0f;
        lbl_12_bss_1F96C.fadeStep = 1.0f / 60.0f;
        break;
    case 5:
        lbl_12_bss_1F96C.state = 6;
        lbl_12_bss_1F96C.fadeTarget = 1.0f;
        lbl_12_bss_1F96C.fadeStep = 0.06666667f;
        lbl_12_bss_1F96C.targetPos.z = 3.0f * lbl_12_bss_1F968;
        lbl_12_bss_1F96C.targetPos.x = 1.3333334f * (0.7f * (lbl_12_bss_1F96C.targetPos.z * math_tan(0x1555)));
        lbl_12_bss_1F96C.targetPos.y = -0.75f * (lbl_12_bss_1F96C.targetPos.z * math_tan(0x1555));
        if (modeCtrl.wideMode)
            lbl_12_bss_1F96C.targetPos.y *= 0.8660254f;
    case 6:
        lbl_12_bss_1F96C.timer--;
        if (lbl_12_bss_1F96C.timer <= 0)
        {
            lbl_12_bss_1F96C.state = 0;
            lbl_12_bss_1F96C.fadeTarget = 0.0f;
            lbl_12_bss_1F96C.fadeStep = 0.033333335f;
        }
        break;
    }

    if (lbl_12_bss_1F96C.fade < lbl_12_bss_1F96C.fadeTarget)
    {
        lbl_12_bss_1F96C.fade += lbl_12_bss_1F96C.fadeStep;
        if (lbl_12_bss_1F96C.fade > lbl_12_bss_1F96C.fadeTarget)
            lbl_12_bss_1F96C.fade = lbl_12_bss_1F96C.fadeTarget;
    }
    else if (lbl_12_bss_1F96C.fade > lbl_12_bss_1F96C.fadeTarget)
    {
        lbl_12_bss_1F96C.fade -= lbl_12_bss_1F96C.fadeStep;
        if (lbl_12_bss_1F96C.fade < lbl_12_bss_1F96C.fadeTarget)
            lbl_12_bss_1F96C.fade = lbl_12_bss_1F96C.fadeTarget;
    }

    lbl_12_bss_1F96C.pos.x += 0.1f * (lbl_12_bss_1F96C.targetPos.x - lbl_12_bss_1F96C.pos.x);
    lbl_12_bss_1F96C.pos.y += 0.1f * (lbl_12_bss_1F96C.targetPos.y - lbl_12_bss_1F96C.pos.y);
    lbl_12_bss_1F96C.pos.z += 0.1f * (lbl_12_bss_1F96C.targetPos.z - lbl_12_bss_1F96C.pos.z);

    lbl_12_bss_1F96C.rotVel.x *= 0.96f;
    lbl_12_bss_1F96C.rotVel.y *= 0.96f;
    lbl_12_bss_1F96C.rotVel.z *= 0.96f;
    lbl_12_bss_1F96C.rotVel.x += -0.8f * lbl_12_bss_1F96C.rot.x;
    lbl_12_bss_1F96C.rotVel.y += -0.8f * lbl_12_bss_1F96C.rot.y;
    lbl_12_bss_1F96C.rotVel.z += -0.8f * lbl_12_bss_1F96C.rot.z;
    lbl_12_bss_1F96C.rot.x += lbl_12_bss_1F96C.rotVel.x;
    lbl_12_bss_1F96C.rot.y += lbl_12_bss_1F96C.rotVel.y;
    lbl_12_bss_1F96C.rot.z += lbl_12_bss_1F96C.rotVel.z;

    switch (lbl_12_bss_1F96C.jumpState)
    {
    case 0:
        lbl_12_bss_1F96C.swayVel.x *= 0.94f;
        lbl_12_bss_1F96C.swayVel.y *= 0.94f;
        lbl_12_bss_1F96C.swayVel.z *= 0.94f;
        lbl_12_bss_1F96C.swayVel.y += -0.16f;
        lbl_12_bss_1F96C.spinVel *= 0.9f;
        break;
    case 1:
        lbl_12_bss_1F96C.jumpState = 2;
        {
            float t = lbl_12_bss_1F96C.jumpTimer;

            lbl_12_bss_1F96C.swayVel.x = -0.5f * (-0.4f * t) + -lbl_12_bss_1F96C.swayPos.x / t;
            lbl_12_bss_1F96C.spinVel = -0.5f * (-207.0f * t) + -lbl_12_bss_1F96C.spin / t;
        }
    case 2:
        lbl_12_bss_1F96C.jumpTimer--;
        lbl_12_bss_1F96C.swayVel.x += -0.4f;
        lbl_12_bss_1F96C.swayVel.y = 0.1f * (5.0f - lbl_12_bss_1F96C.swayPos.y);
        lbl_12_bss_1F96C.spinVel += -207.0f;
        if (lbl_12_bss_1F96C.jumpTimer <= 0)
        {
            lbl_12_bss_1F96C.jumpState = 3;
            mtxa_from_rotate_y(rand() & 0x7FFF);
            mtxa_rotate_x(rand() & 0x7FFF);
            mtxa_tf_vec_xyz(0.0f, 0.0f, 2048.0f * (0.5f + RAND_FLOAT()), &v);
            lbl_12_bss_1F96C.rotVel.x += v.x;
            lbl_12_bss_1F96C.rotVel.y += v.y;
            lbl_12_bss_1F96C.rotVel.z += v.z;
        }
        break;
    case 3:
        lbl_12_bss_1F96C.jumpState = 4;
        lbl_12_bss_1F96C.jumpTimer = 30.0f * RAND_FLOAT();
    case 4:
        lbl_12_bss_1F96C.jumpTimer--;
        lbl_12_bss_1F96C.swayVel.x *= 0.85f;
        lbl_12_bss_1F96C.swayVel.y *= 0.85f;
        lbl_12_bss_1F96C.swayVel.z *= 0.85f;
        lbl_12_bss_1F96C.spinVel *= 0.85f;
        if (lbl_12_bss_1F96C.jumpTimer <= 0)
            lbl_12_bss_1F96C.jumpState = 0;
        break;
    }

    lbl_12_bss_1F96C.swayPos.x += lbl_12_bss_1F96C.swayVel.x;
    lbl_12_bss_1F96C.swayPos.y += lbl_12_bss_1F96C.swayVel.y;
    lbl_12_bss_1F96C.swayPos.z += lbl_12_bss_1F96C.swayVel.z;
    if (lbl_12_bss_1F96C.swayPos.x < 0.0f)
    {
        lbl_12_bss_1F96C.swayPos.x = 0.0f;
        if (lbl_12_bss_1F96C.swayVel.x < 0.0f)
            lbl_12_bss_1F96C.swayVel.x *= -0.8f;
    }
    if (lbl_12_bss_1F96C.swayPos.y < 0.0f)
    {
        lbl_12_bss_1F96C.swayPos.y = 0.0f;
        if (lbl_12_bss_1F96C.swayVel.y < 0.0f)
            lbl_12_bss_1F96C.swayVel.y *= -0.8f;
        lbl_12_bss_1F96C.spinVel *= 0.9f;
    }
    lbl_12_bss_1F96C.spin += lbl_12_bss_1F96C.spinVel;
    if (lbl_12_bss_1F96C.spin < 0.0f)
    {
        lbl_12_bss_1F96C.spin = 0.0f;
        if (lbl_12_bss_1F96C.spinVel < 0.0f)
            lbl_12_bss_1F96C.spinVel *= -0.8f;
    }
}

/* Unused per-frame hook. */
void fn_12_2ABC8(void) {}

/* Sets up a rectangle covering the view at depth z. */
#define FIGHT10_SCREEN_RECT(rect, depth) \
    do \
    { \
        (rect).z = (depth); \
        (rect).y1 = (rect).z * g_current_camera->sub28.view.tanHalfFov; \
        (rect).y2 = -(rect).y1; \
        (rect).x1 = (rect).y1 * g_current_camera->sub28.view.aspect; \
        (rect).x2 = -(rect).x1; \
    } while (0)

/* Draws the object and its fades while it is visible. */
void fn_12_2ABCC(void)
{
    struct Fight10Rect rect;

    if (lbl_12_bss_1F96C.fade <= 0.0f)
        return;
    if (lbl_12_bss_1F96C.fade >= 1.0f)
    {
        FIGHT10_SCREEN_RECT(rect, -10000.0f);
        fn_12_2AF6C(&rect);
    }
    else
    {
        FIGHT10_SCREEN_RECT(rect, -0.1001f);
        fn_12_2AF6C(&rect);
        fn_12_2B160();
    }
    fn_12_2AD5C();
    FIGHT10_SCREEN_RECT(rect, -10000.0f);
    fn_12_2AF6C(&rect);
}

/* Draws the three object models (body, spinning part, bobbing part). */
void fn_12_2AD5C(void)
{
    struct GMAModel *model = minigameGma->modelEntries[145].model;

    mtxa_from_mtxb();
    mathutil_mtxA_set_translate(&lbl_12_bss_1F96C.pos);
    mtxa_rotate_y(-0x8000);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(model);
    mtxa_push();
    mtxa_rotate_y(lbl_12_bss_1F96C.rot.y);
    mtxa_rotate_x(lbl_12_bss_1F96C.rot.x);
    mtxa_rotate_z(lbl_12_bss_1F96C.rot.z);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(minigameGma->modelEntries[146].model);
    mtxa_pop();
    mtxa_translate_xyz(25.0f + lbl_12_bss_1F96C.swayPos.x, lbl_12_bss_1F96C.swayPos.y,
        lbl_12_bss_1F96C.swayPos.z + -16.0f * (math_sin(16384.0f + lbl_12_bss_1F96C.spin) - 1.0f));
    mtxa_rotate_y(-lbl_12_bss_1F96C.spin);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(minigameGma->modelEntries[147].model);
}

/* Draws an untextured black quad over rect. */
void fn_12_2AF6C(struct Fight10Rect *rect)
{
    float x1 = rect->x1;
    float y1 = rect->y1;
    float x2 = rect->x2;
    float y2 = rect->y2;
    float z = rect->z;

    g_something_with_GX_vtx_desc(0x200);
    GXSetBlendMode_cached(1, 0, 1, 0);
    GXSetZMode_cached(1, 7, 1);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    g_GXSetTevIndirect_zero_if_different(0);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 0xFF);
    GXSetTevKAlphaSel_cached(0, 0);
    GXSetTevColorIn_cached(0, 15, 15, 15, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 3, 1, 0);
    GXSetNumTevStages_cached(1);
    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x2, y1, z);
    GXPosition3f32(x1, y1, z);
    GXPosition3f32(x1, y2, z);
    GXPosition3f32(x2, y2, z);
    GXEnd();
    GXSetZMode_cached(1, 3, 1);
}

/* Draws 20 horizontal bands whose height grows with the fade (a venetian-blind wipe). */
void fn_12_2B160(void)
{
    float x;
    float halfW;
    int i;
    float bandH;
    float top;
    float h;
    float y;
    float t;

    GXSetZMode_cached(1, 7, 1);
    bandH = 0.05f * (20000.0f * g_current_camera->sub28.view.tanHalfFov);
    halfW = 10000.0f * g_current_camera->sub28.view.tanHalfFov * g_current_camera->sub28.view.aspect;
    top = -10000.0f * g_current_camera->sub28.view.tanHalfFov;
    GXBegin(0x80, 0, 80);
    t = 4.0f * lbl_12_bss_1F96C.fade;
    y = top + 0.5f * bandH;
    for (i = 20; i > 0; i--, y += bandH)
    {
        h = t;
        if (h > 1.0f)
            h = 1.0f;
        else if (h < 0.0f)
            h = 0.0f;
        x = halfW;
        h = 0.5f * (bandH * h);
        /* each corner's y (then x) is computed before its FIFO writes */
        {
            float vy = y + h;

            GXPosition3f32(x, vy, -10000.0f);
        }
        {
            float vx;
            float vy;

            vy = y + h;
            vx = -x;
            GXPosition3f32(vx, vy, -10000.0f);
        }
        {
            float vx;
            float vy;

            vy = y - h;
            vx = -x;
            GXPosition3f32(vx, vy, -10000.0f);
        }
        {
            float vy = y - h;

            GXPosition3f32(x, vy, -10000.0f);
        }
        t -= 0.2f;
    }
    GXEnd();
    GXSetZMode_cached(1, 3, 1);
}

/* Starts the approach sequence for the given number of frames. */
void fn_12_2B350(int frames)
{
    lbl_12_bss_1F96C.state = 1;
    lbl_12_bss_1F96C.timer = (s16)frames;
}

/* Starts the leave sequence for the given number of frames. */
void fn_12_2B374(int frames)
{
    lbl_12_bss_1F96C.state = 5;
    lbl_12_bss_1F96C.timer = (s16)frames;
}

/* Starts a jump lasting the given number of frames. */
void fn_12_2B398(int frames)
{
    lbl_12_bss_1F96C.jumpState = 1;
    lbl_12_bss_1F96C.jumpTimer = (s16)frames;
}
