#include "types.h"
#include "bitmap.h"
#include "camera.h"
#include "gx.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"

extern int currentHeap;
extern u32 frame_counter;

#ifndef __MWERKS__
/* PC port: 0xCC008000 is GC hardware; dump this debug screen's raw s8
 * texcoord writes into a local sink (TODO: route to pc_gx_fifo_s8). */
#undef GXWGFifo
static union { u8 u8; s8 s8; u16 u16; s16 s16; u32 u32; s32 s32; float f32; } pc_wgfifo_sink;
#define GXWGFifo pc_wgfifo_sink
#endif

void *alloc_from_heap_or_panic(int heap, u32 size, char *file, int line);
void OSFreeToHeap(int heap, void *ptr);
u32 GXGetTexBufferSize(u16 w, u16 h, int fmt, u8 mip, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *buf, u16 w, u16 h, int fmt, int s, int t, u8 mip);
void GXSetTexCopySrc_cached(u16 x, u16 y, u16 w, u16 h);
void GXSetTexCopyDst_cached(u16 w, u16 h, int fmt, u8 mip);
void GXCopyTex(void *dst, u8 clear);
void GXSetViewport(float x, float y, float w, float h, float n, float f);
void GXSetScissor_cached(u32 x, u32 y, u32 w, u32 h);
void GXInvalidateTexAll(void);
void GXLoadPosMtxImm(Mtx m, u32 id);
void GXLoadNrmMtxImm(Mtx m, u32 id);
void GXLoadTexMtxImm(Mtx m, u32 id, int type);
void GXSetVtxAttrFmt(int fmt, int attr, int cnt, int type, u8 frac);
void GXSetChanMatColor(int chan, GXColor color);
void GXSetChanAmbColor(int chan, GXColor color);
void opti_GXSetChanCtrl(int chan, u8 enable, int amb, int mat, int light, int diff, int attn);
void GXSetNumChans_cached(u8 n);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetTevColor(int reg, GXColor color);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetBlendMode_cached(int type, int src, int dst, int op);
void GXSetZMode_cached(u8 test, int func, u8 update);
void GXSetLineWidth_cached(u8 width, int texOffset);
void g_something_with_GX_vtx_desc(int a);
void g_something_with_GXSetProjectionv(Mtx m, int type);
void fn_1_E2F1C(void);
void GXGetProjectionv(float *p);
void g_smth_with_projection_mtx(float *p);
void C_MTXLookAt(Mtx m, Vec *pos, Vec *up, Vec *target);
void C_MTXFrustum(Mtx m, float t, float b, float l, float r, float n, float f);
void PSMTXCopy(Mtx src, Mtx dst);
u32 PSMTXInverse(Mtx src, Mtx dst);
void PSMTXConcat(Mtx a, Mtx b, Mtx out);
void PSMTXScale(Mtx m, float x, float y, float z);
void PSMTXIdentity(Mtx m);
void GXDrawCube(void);
void GXDrawCylinder(u8 n);
void GXDrawSphere(u8 numMajor, u8 numMinor);
void DCFlushRange(void *addr, u32 size);
void *memset(void *dst, int c, u32 n);

struct ShadowLight
{
    Vec pos;
    Vec target;
    Vec up;
    float nearZ;
    float range;
    Mtx view;
    float tan;
    float invTan;
    void *arg;
    void (*getBounds)(void *arg, Vec *center, float *radius);
    void (*draw)(void *arg, struct ShadowLight *light);
    GXTexObj texObj;
    int bufIdx;
    void *bufs[2];
};

struct ShadowBall
{
    Vec pos;
    float radius;
};

void fn_8_104DC(struct ShadowLight *light);
void fn_8_11148(void);
void fn_8_11334(void);
void fn_8_11390(void);
void fn_8_11394(void);
void fn_8_11398(void);
void fn_8_113B8(void);
void fn_8_11B3C(void);
void fn_8_11C2C(void);
void fn_8_11C30(void);
void fn_8_11D0C(void);
void fn_8_11D68(void *arg, Vec *center, float *radius);
void fn_8_11D8C(void *arg, struct ShadowLight *light);

static struct ShadowLight lights[2];
static GXTexObj rampTexObj;
static void *rampBuf;
static int frameCount;
static struct ShadowBall balls[1];

void fn_8_FDF0(void)
{
    int i;
    struct ShadowLight *light;

    fn_8_11390();
    light = lights;
    for (i = 2; i > 0; i--, light++)
    {
        u32 size;

        light->pos.x = 0.0f;
        light->pos.y = 10.0f;
        light->pos.z = 10.0f;
        mtxa_from_identity();
        mtxa_rotate_y((i * 0x10000) / 2);
        mtxa_tf_point(&light->pos, &light->pos);
        light->target.x = 0.0f;
        light->target.y = 0.0f;
        light->target.z = -1.0f;
        light->up.x = 0.0f;
        light->up.y = 1.0f;
        light->up.z = 0.0f;
        light->nearZ = 1.0f;
        light->range = 75.0f;
        C_MTXLookAt(light->view, &light->pos, &light->up, &light->target);
        light->tan = 1.0f;
        light->invTan = 1.0f / light->tan;
        light->arg = NULL;
        light->getBounds = NULL;
        light->draw = NULL;
        light->bufIdx = 0;
        size = GXGetTexBufferSize(0x100, 0x100, 1, 0, 0);
        light->bufs[0] = alloc_from_heap_or_panic(currentHeap, size, "shadow_test.c", 0xAC);
        light->bufs[1] = alloc_from_heap_or_panic(currentHeap, size, "shadow_test.c", 0xAD);
        GXInitTexObj(&light->texObj, light->bufs[light->bufIdx], 0x100, 0x100, 1, 0, 0, 0);
    }
    fn_8_11B3C();
    fn_8_11148();
}

void fn_8_FFC4(void)
{
    int i;
    struct ShadowLight *light;

    fn_8_11334();
    fn_8_11C2C();
    light = lights;
    for (i = 2; i > 0; i--, light++)
    {
        if (light->bufs[0] != NULL)
        {
            OSFreeToHeap(currentHeap, light->bufs[0]);
            light->bufs[0] = NULL;
        }
        if (light->bufs[1] != NULL)
        {
            OSFreeToHeap(currentHeap, light->bufs[1]);
            light->bufs[1] = NULL;
        }
    }
    fn_8_11394();
}

void fn_8_10070(void)
{
    int i;
    struct ShadowLight *light;

    fn_8_11398();
    fn_8_11C30();
    light = lights;
    for (i = 2; i > 0; i--, light++)
    {
        if (i == 2 && (pad_status_groups->unk0[0].button & 4))
            light->pos = g_current_camera->lookAt;
    }
}

void fn_8_100FC(void)
{
    Mtx proj;
    Mtx savedView;
    float projParams[7];
    float radius;
    int i;
    struct ShadowLight *light;

    GXGetProjectionv(projParams);
    PSMTXCopy(mtxa->mtxB, savedView);
    light = lights;
    for (i = 2; i > 0; i--, light++)
    {
        if (light->getBounds != NULL && light->draw != NULL)
        {
            float rSq;
            float dx;
            float dy;
            float dz;
            float dist;
            float distSq;
            float size;

            light->getBounds(light->arg, &light->target, &radius);
            rSq = radius * radius;
            dx = light->pos.x - light->target.x;
            dy = light->pos.y - light->target.y;
            dz = light->pos.z - light->target.z;
            distSq = dx * dx + dy * dy + dz * dz;
            dist = math_sqrt(distSq);
            light->nearZ = dist;
            light->range = 75.0f;
            C_MTXLookAt(light->view, &light->pos, &light->up, &light->target);
            PSMTXCopy(light->view, mtxa->mtxB);
            if (distSq > rSq)
            {
                radius *= 1.0117188f * math_sqrt(1.0f - rSq / distSq);
                light->tan = dist * radius / (distSq - rSq);
            }
            else
                light->tan = 5.6712818f;
            light->invTan = 1.0f / light->tan;
            size = 0.1f * light->tan;
            C_MTXFrustum(proj, size, -size, -size, size, 0.1f, 100000.0f);
            g_something_with_GXSetProjectionv(proj, 0);
            GXSetViewport(0.0f, 0.0f, 256.0f, 256.0f, 0.0f, 1.0f);
            GXSetScissor_cached(0, 0, 0x100, 0x100);
            light->draw(light->arg, light);
            GXSetTexCopySrc_cached(0, 0, 0x100, 0x100);
            GXSetTexCopyDst_cached(0x100, 0x100, 1, 0);
            GXCopyTex(light->bufs[light->bufIdx], 1);
            GXInitTexObj(&light->texObj, light->bufs[light->bufIdx], 0x100, 0x100, 1, 0, 0, 0);
            light->bufIdx ^= 1;
        }
    }
    PSMTXCopy(savedView, mtxa->mtxB);
    g_smth_with_projection_mtx(projParams);
    GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->xfbHeight, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
}

void fn_8_10464(void)
{
    int i;
    struct ShadowLight *light;

    fn_8_113B8();
    fn_8_11D0C();
    light = lights;
    for (i = 2; i > 0; i--, light++)
    {
        if (pad_status_groups->unk0[0].button & 8)
            fn_8_104DC(light);
    }
}

#define V3(x, y, z) GXPosition3f32(x, y, z)

static inline void put(Vec *v)
{
    GXPosition3f32(v->x, v->y, v->z);
}

void fn_8_104DC(struct ShadowLight *light)
{
    GXColor c = {0xFF, 0xFF, 0xFF, 0xFF};
    Mtx m;
    Vec v0;
    Vec v4;
    Vec v1;
    Vec v5;
    Vec v2;
    Vec v6;
    Vec v3;
    Vec v7;
    float s;
    int stage;
    int coord;
    int texMtx;
    int i;

    c.r = 0xFF;
    c.g = 0xFF;
    c.b = 0xFF;
    c.a = 0xFF;
    GXSetChanMatColor(4, c);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 1);
    GXSetNumChans_cached(1);
    stage = 0;
    coord = 0;
    g_GXSetTevOrder_wrapper(stage, 0xFF, 0xFF, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 4);
    stage++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 1, 0, 0);
    GXSetZMode_cached(1, 1, 1);
    fn_1_E2F1C();
    g_something_with_GX_vtx_desc(0x200);
    GXSetVtxAttrFmt(6, 9, 1, 4, 0);
    v0.x = light->tan * light->nearZ;
    v0.y = light->tan * light->nearZ;
    v0.z = -light->nearZ;
    v4.x = light->tan * (light->nearZ + light->range);
    v4.y = light->tan * (light->nearZ + light->range);
    v4.z = -(light->nearZ + light->range);
    v1.x = -v0.x;
    v1.y = v0.y;
    v1.z = v0.z;
    v5.x = -v4.x;
    v5.y = v4.y;
    v5.z = v4.z;
    v2.x = -v0.x;
    v2.y = -v0.y;
    v2.z = v0.z;
    v6.x = -v4.x;
    v6.y = -v4.y;
    v6.z = v4.z;
    v3.y = -v0.y;
    v3.z = v0.z;
    v7.x = v4.x;
    v7.y = -v4.y;
    v7.z = v4.z;
    PSMTXInverse(light->view, mtxa->mtxA);
    PSMTXConcat(mtxa->mtxB, mtxa->mtxA, mtxa->mtxA);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXSetLineWidth_cached(6, 0);
    GXBegin(0xA8, 6, 8);
    V3(0.0f, 0.0f, 0.0f);
    V3(v0.x, v0.y, v0.z);
    V3(0.0f, 0.0f, 0.0f);
    put(&v1);
    V3(0.0f, 0.0f, 0.0f);
    put(&v2);
    V3(0.0f, 0.0f, 0.0f);
    v3.x = v0.x;
    put(&v3);
    GXSetLineWidth_cached(0x12, 0);
    GXBegin(0xA8, 6, 8);
    V3(v0.x, v0.y, v0.z);
    put(&v4);
    put(&v1);
    put(&v5);
    put(&v2);
    put(&v6);
    v3.x = v0.x;
    put(&v3);
    put(&v7);
    GXBegin(0xB0, 6, 5);
    V3(v0.x, v0.y, v0.z);
    put(&v1);
    put(&v2);
    v3.x = v0.x;
    put(&v3);
    V3(v0.x, v0.y, v0.z);
    GXBegin(0xB0, 6, 5);
    put(&v4);
    put(&v5);
    put(&v6);
    put(&v7);
    put(&v4);
    c.r = 0xFF;
    c.g = 0xC0;
    c.b = 0x60;
    c.a = 0xFF;
    GXSetChanMatColor(4, c);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 1);
    GXSetNumChans_cached(1);
    GXLoadTexObj_cached(&light->texObj, 0);
    GXLoadTexObj_cached(&rampTexObj, 1);
    stage = 0;
    coord = 0;
    texMtx = 0x1E;
    GXSetTexCoordGen2_cached(coord, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 3);
    stage++;
    coord++;
    GXSetTexCoordGen2_cached(coord, 0, 4, texMtx, 0, 0x7D);
    PSMTXScale(m, 0.0f, 0.0f, 0.0f);
    m[0][0] = 1.1111112f;
    m[0][3] = -0.055555582f;
    m[1][1] = 1.1111112f;
    m[1][3] = -0.055555582f;
    m[2][3] = 1.0f;
    GXLoadTexMtxImm(m, texMtx, 0);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 3);
    GXSetTevColorIn_cached(stage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(stage, 1, 0, 0, 1, 0);
    stage++;
    coord++;
    texMtx += 3;
    GXSetTexCoordGen2_cached(coord, 0, 0, texMtx, 0, 0x7D);
    PSMTXScale(m, 0.0f, 0.0f, 0.0f);
    m[2][3] = 1.0f;
    m[0][2] = -0.5f / light->range;
    m[0][3] = 0.5f + (-0.5f * light->nearZ) / light->range;
    GXLoadTexMtxImm(m, texMtx, 0);
    g_GXSetTevOrder_wrapper(stage, coord, 1, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 4);
    GXSetTevColorIn_cached(stage, 0xF, 0, 8, 0xF);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;
    g_GXSetTevOrder_wrapper(stage, 0xFF, 0xFF, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 4);
    GXSetTevColorIn_cached(stage, 0xF, 0xA, 0, 0xF);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 1, 1, 0);
    GXSetZMode_cached(1, 1, 0);
    fn_1_E2F1C();
    g_something_with_GX_vtx_desc(0x2200);
    GXSetVtxAttrFmt(6, 9, 1, 4, 0);
    GXSetVtxAttrFmt(6, 0xD, 1, 1, 0);
    PSMTXInverse(light->view, mtxa->mtxA);
    PSMTXConcat(mtxa->mtxB, mtxa->mtxA, mtxa->mtxA);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0x80, 6, 0x40);
    s = (float)(frame_counter & 0x1F) / 32.0f / 8.0f;
    for (i = 8; i > 0;)
    {
        v0.x = s * (light->tan * (light->nearZ + light->range));
        v0.y = v0.x;
        v0.z = -s * (light->nearZ + light->range);
        V3(v0.x, v0.y, v0.z);
        GXWGFifo.s8 = 1;
        GXWGFifo.s8 = 0;
        V3(-v0.x, v0.y, v0.z);
        GXWGFifo.s8 = 0;
        GXWGFifo.s8 = 0;
        V3(-v0.x, -v0.y, v0.z);
        GXWGFifo.s8 = 0;
        GXWGFifo.s8 = 1;
        V3(v0.x, -v0.y, v0.z);
        GXWGFifo.s8 = 1;
        GXWGFifo.s8 = 1;
        V3(-v0.x, v0.y, v0.z);
        GXWGFifo.s8 = 0;
        GXWGFifo.s8 = 0;
        V3(v0.x, v0.y, v0.z);
        GXWGFifo.s8 = 1;
        GXWGFifo.s8 = 0;
        V3(v0.x, -v0.y, v0.z);
        GXWGFifo.s8 = 1;
        GXWGFifo.s8 = 1;
        V3(-v0.x, -v0.y, v0.z);
        GXWGFifo.s8 = 0;
        GXWGFifo.s8 = 1;
        i--;
        s += 0.125f;
    }
}

void fn_8_11148(void)
{
    if (rampBuf == NULL)
    {
        u32 size = GXGetTexBufferSize(0x20, 4, 1, 0, 0);
        u8 *p;
        int j;
        int i;

        rampBuf = alloc_from_heap_or_panic(currentHeap, size, "shadow_test.c", 0x2FB);
        p = rampBuf;
        memset(p, 0, 0x20);
        p += 0x20;
        memset(p, 0, 0x20);
        p += 0x20;
        for (i = 4; i > 0; i--)
        {
            for (j = 8; j > 0; j--, p++)
            {
                float v = 0.5f + 0.0625f * j;

                *p = 255.0f * v;
            }
        }
        for (i = 4; i > 0; i--)
        {
            for (j = 8; j > 0; j--, p++)
            {
                float v = 0.071428575f * (j - 1);

                *p = 255.0f * v;
            }
        }
        DCFlushRange(rampBuf, size);
    }
    GXInitTexObj(&rampTexObj, rampBuf, 0x20, 4, 1, 0, 0, 0);
}

void fn_8_11334(void)
{
    if (rampBuf != NULL)
    {
        OSFreeToHeap(currentHeap, rampBuf);
        rampBuf = NULL;
    }
}

void fn_8_11390(void) {}

void fn_8_11394(void) {}

void fn_8_11398(void)
{
    frameCount++;
}

static inline int isFourth(int i)
{
    return (i & 3) == 0;
}

static inline void loadLightTexMtxs(Mtx model)
{
    Mtx depth;
    Mtx proj;
    Mtx tmp;
    int id;
    int i;
    struct ShadowLight *light;

    id = 0x1E;
    PSMTXIdentity(proj);
    proj[0][2] = 0.5f;
    proj[1][2] = 0.5f;
    PSMTXScale(depth, 0.0f, 0.0f, 0.0f);
    depth[2][3] = 1.0f;
    light = lights;
    for (i = 2; i > 0; i--, light++)
    {
        proj[0][0] = -0.5f * light->invTan;
        proj[1][1] = 0.5f * light->invTan;
        PSMTXConcat(proj, light->view, tmp);
        PSMTXConcat(tmp, model, tmp);
        GXLoadTexMtxImm(tmp, id, 0);
        id += 3;
        depth[0][2] = -0.5f / light->range;
        depth[0][3] = 0.5f + (-0.5f * light->nearZ) / light->range;
        PSMTXConcat(depth, light->view, tmp);
        PSMTXConcat(tmp, model, tmp);
        GXLoadTexMtxImm(tmp, id, 0);
        id += 3;
    }
}

void fn_8_113B8(void)
{
    GXLightObj lightObj;
    Mtx scale;
    Mtx model;
    Vec pos;
    GXColor c = {0xFF, 0xFF, 0xFF, 0xFF};
    struct ShadowLight *light;
    int stage;
    int coord;
    int i;
    int texMtx;
    int a;
    int j;

    mtxa_from_mtxb();
    GXInitLightSpot(&lightObj, 360.0f, 0);
    GXInitLightDistAttn(&lightObj, 0.0f, 0.0f, 0);
    c.r = 0xFF;
    c.g = 0xFF;
    c.b = 0xFF;
    c.a = 0xFF;
    GXInitLightColor(&lightObj, c);
    light = lights;
    for (i = 0; i < 2; i++, light++)
    {
        mtxa_tf_point(&light->pos, &pos);
        GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
        GXLoadLightObjImm(&lightObj, i + 1);
    }
    c.r = 0xFF;
    c.g = 0xFF;
    c.b = 0xFF;
    c.a = 0xFF;
    GXSetChanMatColor(4, c);
    c.r = 0x20;
    c.g = 0x20;
    c.b = 0x40;
    c.a = 0xFF;
    GXSetChanAmbColor(4, c);
    GXSetTevColor(1, c);
    opti_GXSetChanCtrl(4, 1, 0, 0, 1, 2, 1);
    GXSetNumChans_cached(1);
    stage = 0;
    GXInvalidateTexAll();
    j = stage;
    GXLoadTexObj_cached(&rampTexObj, stage);
    stage++;
    light = lights;
    for (i = 2; i > 0; i--, light++, stage++)
        GXLoadTexObj_cached(&light->texObj, stage);
    stage = 0;
    coord = 0;
    texMtx = 0x1E;
    a = 0xA;
    light = lights;
    for (i = 2; i > 0; i--, light++)
    {
        GXSetTexCoordGen2_cached(coord, 0, 0, texMtx, 0, 0x7D);
        g_GXSetTevOrder_wrapper(stage, coord, j + 1, 4);
        g_GXSetTevIndirect_zero_if_different(stage);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 4);
        GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
        GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
        stage++;
        coord++;
        texMtx += 3;
        GXSetTexCoordGen2_cached(coord, 0, 0, texMtx, 0, 0x7D);
        g_GXSetTevOrder_wrapper(stage, coord, j, 4);
        g_GXSetTevIndirect_zero_if_different(stage);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 4);
        GXSetTevColorIn_cached(stage, 0xF, 8, 0, 0xF);
        GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
        stage++;
        coord++;
        texMtx += 3;
        g_GXSetTevOrder_wrapper(stage, 0xFF, 0xFF, 4);
        g_GXSetTevIndirect_zero_if_different(stage);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 4);
        GXSetTevColorIn_cached(stage, a, 2, 0, 0xF);
        if (i == 1)
            GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
        else
        {
            GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 2);
            a = 4;
        }
        stage++;
    }
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 1, 0, 0);
    GXSetZMode_cached(1, 1, 1);
    fn_1_E2F1C();
    for (i = 20; i > 0; i--)
    {
        int draw = isFourth(i);

        mtxa_from_identity();
        mtxa_translate_xyz(-100.0f, -5.0f, -100.0f + 10.0f * (i - 1));
        for (j = 20; j > 0; j--)
        {
            PSMTXScale(scale, 5.0f, 5.0f, 5.0f);
            PSMTXConcat(mtxa->mtxA, scale, model);
            loadLightTexMtxs(model);
            PSMTXConcat(mtxa->mtxB, model, model);
            GXLoadPosMtxImm(model, 0);
            GXLoadNrmMtxImm(model, 0);
            GXDrawCube();
            if (draw)
            {
                mtxa_push();
                mtxa_translate_xyz(0.0f, 20.0f, 0.0f);
                mtxa_rotate_x(0x4000);
                PSMTXScale(scale, 2.0f, 2.0f, 20.0f);
                PSMTXConcat(mtxa->mtxA, scale, model);
                loadLightTexMtxs(model);
                PSMTXConcat(mtxa->mtxB, model, model);
                GXLoadPosMtxImm(model, 0);
                GXLoadNrmMtxImm(model, 0);
                GXDrawCylinder(0xC);
                mtxa_pop();
            }
            mtxa_translate_xyz(10.0f, 0.0f, 0.0f);
        }
    }
}

void fn_8_11B3C(void)
{
    int i;
    struct ShadowBall *ball;
    struct ShadowLight *light;

    ball = balls;
    for (i = 1; i > 0; i--, ball++)
    {
        ball->pos.x = 10.0f;
        ball->pos.y = 10.0f;
        ball->pos.z = 0.0f;
        mtxa_from_identity();
        mtxa_rotate_y(i * 0x10000);
        mtxa_tf_point(&ball->pos, &ball->pos);
        ball->radius = 5.0f;
    }
    light = lights;
    for (i = 2; i > 0; i--, light++)
    {
        int idx = 0;

        ball = &balls[idx];
        light->arg = ball;
        light->getBounds = fn_8_11D68;
        light->draw = fn_8_11D8C;
    }
}

void fn_8_11C2C(void) {}

void fn_8_11C30(void)
{
    int i;
    struct ShadowBall *ball;

    ball = balls;
    for (i = 1; i > 0; i--, ball++)
    {
        if (!(pad_status_groups->unk0[0].button & 2))
        {
            ball->pos.x += 0.005f * (g_current_camera->lookAt.x - ball->pos.x);
            ball->pos.y += 0.005f * (g_current_camera->lookAt.y - ball->pos.y);
            ball->pos.z += 0.005f * (g_current_camera->lookAt.z - ball->pos.z);
        }
    }
}

void fn_8_11D0C(void)
{
    int i;
    struct ShadowBall *ball;

    ball = balls;
    for (i = 1; i > 0; i--, ball++)
        fn_8_11D8C(ball, NULL);
}

void fn_8_11D68(void *arg, Vec *center, float *radius)
{
    struct ShadowBall *ball = arg;

    *center = ball->pos;
    *radius = ball->radius;
}

void fn_8_11D8C(void *arg, struct ShadowLight *shadow)
{
    Mtx m;
    GXLightObj lightObj;
    Vec pos;
    GXColor c = {0xFF, 0xFF, 0xFF, 0xFF};
    int i;
    struct ShadowBall *ball = arg;

    if (shadow != NULL)
    {
        GXSetNumChans_cached(1);
        c.r = 0xFF;
        c.g = 0xFF;
        c.b = 0xFF;
        c.a = 0xFF;
        GXSetChanMatColor(4, c);
        c.r = 0;
        c.g = 0;
        c.b = 0;
        c.a = 0;
        GXSetChanAmbColor(4, c);
        opti_GXSetChanCtrl(4, 0, 0, 0, 1, 2, 1);
        g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);
        GXSetNumTexGens_cached(0);
        GXSetNumTevStages_cached(1);
        GXSetBlendMode_cached(1, 1, 0, 0);
        GXSetZMode_cached(1, 1, 0);
        fn_1_E2F1C();
    }
    else
    {
        mtxa_from_mtxb();
        GXInitLightSpot(&lightObj, 360.0f, 0);
        GXInitLightDistAttn(&lightObj, 0.0f, 0.0f, 0);
        c.r = 0xFF;
        c.g = 0xFF;
        c.b = 0xFF;
        c.a = 0xFF;
        GXInitLightColor(&lightObj, c);
        mtxa_tf_point(&lights[0].pos, &pos);
        GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
        GXLoadLightObjImm(&lightObj, 1);
        GXSetNumChans_cached(1);
        c.r = 0xFF;
        c.g = 0xFF;
        c.b = 0xFF;
        c.a = 0xFF;
        GXSetChanMatColor(4, c);
        c.r = 32.0f + 111.5f * (1.0f + math_sin(frame_counter << 8));
        c.g = 0x20;
        c.b = 0x40;
        c.a = 0xFF;
        GXSetChanAmbColor(4, c);
        opti_GXSetChanCtrl(4, 1, 0, 0, 1, 2, 1);
        g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);
        GXSetNumTexGens_cached(0);
        GXSetNumTevStages_cached(1);
        GXSetBlendMode_cached(1, 1, 0, 0);
        GXSetZMode_cached(1, 1, 1);
        fn_1_E2F1C();
    }
    mtxa_from_identity();
    mtxa_translate(&ball->pos);
    PSMTXScale(m, 0.25f * ball->radius, 0.25f * ball->radius, 0.25f * ball->radius);
    PSMTXConcat(mtxa->mtxA, m, mtxa->mtxA);
    PSMTXConcat(mtxa->mtxB, mtxa->mtxA, mtxa->mtxA);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    GXDrawSphere(8, 8);
    mtxa_from_identity();
    mtxa_translate(&ball->pos);
    mtxa_rotate_y(frameCount << 7);
    mtxa_rotate_x(frameCount << 6);
    mtxa_rotate_z(frameCount << 5);
    for (i = 16; i > 0; i--)
    {
        mtxa_push();
        mtxa_rotate_y((i * 0x10000) >> 4);
        mtxa_translate_xyz(0.0f, 0.0f, -0.5f * ball->radius);
        PSMTXScale(m, 0.05f * ball->radius, 0.05f * ball->radius, 0.25f * ball->radius);
        PSMTXConcat(mtxa->mtxA, m, mtxa->mtxA);
        PSMTXConcat(mtxa->mtxB, mtxa->mtxA, mtxa->mtxA);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        GXDrawCylinder(8);
        mtxa_pop();
    }
}
