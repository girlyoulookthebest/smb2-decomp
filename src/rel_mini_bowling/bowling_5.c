/* Ball trail: a ring of recent ball positions drawn as a triangle strip. */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "gx.h"
#include "mathutil.h"

#ifndef __MWERKS__
/* pc_port: undeclared in the original; an implicit declaration passes the
 * float scale as a double on x86. */
void PSVECAdd(Vec *a, Vec *b, Vec *out);
void PSVECSubtract(Vec *a, Vec *b, Vec *out);
void PSVECScale(Vec *in, Vec *out, float scale);
/* pc_port: also undeclared; an implicit int return truncates the texture
 * data pointer on x64. */
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
#endif

struct BallTrailPoint
{
    Vec pos;
    float alpha;
    float tex;
};

struct BallTrail
{
    s16 head;
    float tex;
    struct BallTrailPoint points[32];
};

extern struct GMA *minigameGma[];

void g_something_with_GX_vtx_desc(u32 attrs);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumChans_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
s16 g_math_unk4(float x);
void fn_1_E2FD0(int width);
void fn_1_E2FE0(int a, int b, int c, int d);
void fn_1_E3014(Vec *start, Vec *end, GXColor color);
void fn_5_16D30(Vec *pos, Vec *prevPos, float radius, Vec *a, float *b, float *c);

struct BallTrail lbl_5_bss_18750;
struct BallTrail lbl_5_bss_189D8[3];

void fn_5_11A48(void)
{
    int i;
    struct BallTrailPoint *point;

    lbl_5_bss_18750.head = 0;
    lbl_5_bss_18750.tex = 0.0f;
    for (i = 0, point = lbl_5_bss_18750.points; i < 32; i++, point++)
    {
        point->pos = current_ball->pos;
        point->alpha = 0.0f;
        point->tex = 0.0f;
    }
}

void fn_5_11ADC(void)
{
    int width;
    int height;
    int fmt;
    GXTexObj *texObj;
    int i;

    if (!(g_some_other_flags & 0xA))
    {
        float speed = mathutil_vec_len(&current_ball->vel);
        struct BallTrailPoint *point;

        lbl_5_bss_18750.head = (lbl_5_bss_18750.head + 1) % 32;
        point = &lbl_5_bss_18750.points[lbl_5_bss_18750.head];
        point->pos = current_ball->pos;
        point->alpha = 10.000000000000002 * speed - 2.0000000000000004;
        if (point->alpha < 0.0)
            point->alpha = 0.0f;
        if (point->alpha > 1.0)
            point->alpha = 1.0f;
        lbl_5_bss_18750.tex -= 0.1f * speed;
        point->tex = lbl_5_bss_18750.tex;
        for (i = 0, point = lbl_5_bss_18750.points; i < 32; i++, point++)
        {
            if (i != lbl_5_bss_18750.head)
                point->tex += 0.02f * speed;
        }
    }
    texObj = minigameGma[0]->modelEntries[5].model->texObjs;
    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    GXSetCullMode_cached(0);
    GXSetBlendMode_cached(1, 4, 1, 0);
    GXLoadTexObj_cached(texObj, 0);
    fmt = GXGetTexObjFmt(texObj);
    height = GXGetTexObjHeight(texObj);
    width = GXGetTexObjWidth(texObj);
    GXInitTexObj(texObj, GXGetTexObjData(texObj), width, height, fmt, 0, 1, 0);
    g_something_with_GX_vtx_desc(0x2A00);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 2);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumChans_cached(1);
    GXSetNumIndStages_cached(0);
    GXBegin(0x98, 0, 0x3E);
    {
        int cur = (lbl_5_bss_18750.head + 1) % 32;
        int index = cur;
        Vec toCam;
        Vec dir;
        Vec side = {0.5f, 0.0f, 0.0f};
        Vec cross;
        Vec offset;

        for (i = 0; i < 31; i++)
        {
            float t = (float)i / 30.0f;
            struct BallTrailPoint *point;
            float angleScale;
            int alpha;

            cur = index;
            index = (cur + 1) % 32;
            point = &lbl_5_bss_18750.points[cur];
            PSVECSubtract(&point->pos, &g_current_camera->eye, &toCam);
            PSVECSubtract(&point->pos, &lbl_5_bss_18750.points[index].pos, &dir);
            mathutil_vec_cross_prod(&toCam, &dir, &cross);
            if (0.0 != cross.x || 0.0 != cross.y || 0.0 != cross.z)
                vec_set_len(0.5f, &cross, &side);
            PSVECScale(&side, &offset, (float)(0.5 * (1.0 + t)));
            angleScale = vec_dot_normalized_safe(&toCam, &dir);
            angleScale = (float)__fabs(math_sin(g_math_unk4(angleScale)));
            alpha = point->alpha * (angleScale * (255.0f * math_sin(32768.0f * t)));
            GXPosition3f32(point->pos.x + offset.x, point->pos.y + offset.y, point->pos.z + offset.z);
            GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
            GXTexCoord2f32(0.0f, point->tex);
            GXPosition3f32(point->pos.x - offset.x, point->pos.y - offset.y, point->pos.z - offset.z);
            GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
            GXTexCoord2f32(1.0f, point->tex);
        }
    }
    if (dip_switches & 8)
    {
        static const GXColor color = {0xFF, 0xFF, 0x00, 0xFF};
        Vec lineEnd;
        float unused1;
        float unused2;

        fn_1_E2FE0(0, 1, 0, 0);
        fn_5_16D30(&current_ball->pos, &current_ball->prevPos, current_ball->currRadius, &lineEnd, &unused1, &unused2);
        PSVECAdd(&current_ball->pos, &lineEnd, &lineEnd);
        mtxa_from_mtxb();
        fn_1_E2FD0(0x18);
        fn_1_E3014(&current_ball->pos, &lineEnd, color);
    }
}

void fn_5_12194(int slot)
{
    if (slot > -1 && slot < 3)
        lbl_5_bss_189D8[slot] = lbl_5_bss_18750;
}

void fn_5_121E4(int slot)
{
    if (slot > -1 && slot < 3)
        lbl_5_bss_18750 = lbl_5_bss_189D8[slot];
}
