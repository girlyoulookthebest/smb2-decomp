/* Projected shadows cast onto stage models (SMB2-only). Casters queue a request; each frame the
 * requests are transformed into view space, then every receiving model is drawn once per group
 * of four shadows that overlap it, multiplying in the caster's texture and a depth ramp. */
#pragma peephole off
#include "types.h"
#include "avdisp.h"
#include "camera.h"
#include "frustum.h"
#include "gx.h"
#include "mathutil.h"
#include "nl2ngc.h"

struct ShadowTexInfo
{
    /*0x00*/ u8 unk0;  /* 0: perspective, otherwise orthographic */
    /*0x01*/ u8 ramp;  /* index into s_rampTexObjs */
    /*0x02*/ u16 flags;  /* 2: ignores fog */
    /*0x04*/ GXTexObj texObj;
};  /* size = 0x24 */

/* Everything before alpha (0x64 bytes on GC) is the caller's request; the rest is computed here. */
struct ShadowCastReq
{
    /*0x00*/ void *obj;
    /*0x04*/ Vec lightPos;
    /*0x10*/ float unk10;
    /*0x14*/ Vec pos;
    /*0x20*/ float radius;
    /*0x24*/ Vec up;
    /*0x30*/ u16 unk30;  /* receiver layers */
    /*0x32*/ u16 cameraMask;
    /*0x34*/ float unk34;  /* screen size at which the shadow fades in */
    /*0x38*/ float unk38;  /* screen size at which it is fully visible */
    /*0x3C*/ GXColor color;
    /*0x40*/ struct ShadowTexInfo tex;
    /*0x64*/ float alpha;
    /*0x68*/ Vec eye;
    /*0x74*/ float tanHalf;
    /*0x78*/ float invTan;
    /*0x7C*/ float dist;
    /*0x80*/ Vec viewLight;
    /*0x8C*/ Vec viewPos;
    /*0x98*/ Vec viewUp;
    /*0xA4*/ Mtx texMtx;
    /*0xD4*/ Mtx rampMtx;
    /*0x104*/ GXColor kcolor;
};  /* size = 0x108 */

/* The caller's part of a request (callers declare the struct up to tex) */
#ifdef __MWERKS__
#define SHADOW_REQ_SIZE 0x64
#else
#include <stddef.h>
#define SHADOW_REQ_SIZE offsetof(struct ShadowCastReq, alpha)  /* 0x68 on x64 */
#endif

struct ShadowReceive
{
    /*0x00*/ void *obj;  /* a caster never shadows itself */
    /*0x04*/ u16 layer;
    /*0x06*/ u16 modelType;  /* 0-2: NL, 3-5: NL with the alternate draw, 6: GMA */
    /*0x08*/ void *model;
};  /* size = 0xC */

/* Bookkeeping for one group of up to four shadows */
struct ShadowDrawState
{
    /*0x00*/ s32 stage;
    /*0x04*/ s32 tevStage;
    /*0x08*/ s32 texGen;
    /*0x0C*/ s32 texMtx;
    /*0x10*/ s32 numKColors;
    /*0x14*/ s32 texMap;
    /*0x18*/ u8 numTevStages;
    /*0x19*/ u8 numTexGens;
    /*0x1A*/ u8 rampMaps[4];
    /*0x1E*/ GXColor kcolors[4];
    /*0x2E*/ u8 stageSetUp[4];
};  /* size = 0x34 */

struct ShadowDebugQuad
{
    /*0x00*/ float z;
    /*0x04*/ float x1;
    /*0x08*/ float y1;
    /*0x0C*/ float x2;
    /*0x10*/ float y2;
    /*0x14*/ u32 flags;
    /*0x18*/ GXColor color;
    /*0x1C*/ s32 hasVerts;
    /*0x20*/ Vec verts[4];
};  /* size = 0x50 */

/* .bss (0xC99E0 - 0xCE0B0) */
static s32 s_reqCount;
static struct ShadowCastReq *s_reqNext;
static struct ShadowCastReq s_reqs[64];
static u16 s_reqLayers;
static s32 s_shadowsEnabled;
static GXTexObj s_rampTexObjs[4];
static u8 s_rampTexData[4][0x100] __attribute__((aligned(32)));
static Mtx s_rampMtx;

extern struct
{
    u8 filler0[0x4];
    /*0x04*/ float nearDist;
    /*0x08*/ float farDist;
    u8 fillerC[0x3];
    /*0x0F*/ s8 enabled;
} lbl_1_bss_CF22C;

void DCFlushRange(void *addr, u32 size);
void *memset(void *dst, int c, unsigned long n);
void memcpy_handler(void *dst, void *src, u32 size);
double fn_8007682C(double x);
void fn_1_10A2C0(struct GMAModel *model);
void *fn_1_10A69C(void *func);
void fn_1_81E4(GXTexObj *obj, struct ShadowDebugQuad *quad);
void GXGetProjectionv(float *proj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilter, int magFilter, float minLod, float maxLod, float lodBias, u8 biasClamp, u8 doEdgeLod, int maxAniso);
void GXLoadTexMtxImm(Mtx m, u32 id, int type);
void GXLoadTexObj_cached(GXTexObj *obj, int id);
void GXSetNumChans_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevSwapMode_cached(int stage, int ras, int tex);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_something_with_raster_state(int a);
void C_MTXLookAt(Mtx m, Vec *pos, Vec *up, Vec *target);

void fn_1_D3A90(void);
void fn_1_D3C88(u8 *p);
void fn_1_D3FF0(u8 *p);
void g_fill_some_memory_with_0x0_and_0xff(u8 *p);
void fn_1_D41B0(u8 *p);
void fn_1_D5378(void);
void fn_1_D53E0(int stage);
void fn_1_D5798(void);
int fn_1_D5A14(struct NlMesh *mesh);
int fn_1_D5A2C(void);

void fn_1_D3A40(void)
{
    fn_1_D3A90();
    s_reqCount = 0;
    s_reqNext = s_reqs;
    s_reqLayers = 0;
    s_shadowsEnabled = 1;
}

void fn_1_D3A90(void)
{
    int i;
    GXTexObj *obj;
    u8 *data;

    for (i = 0; i < 4; i++)
    {
        obj = &s_rampTexObjs[i];
        data = s_rampTexData[i];
        switch (i)
        {
        case 0:
            fn_1_D3C88(data);
            break;
        case 1:
            fn_1_D3FF0(data);
            break;
        case 2:
            g_fill_some_memory_with_0x0_and_0xff(data);
            break;
        case 3:
            fn_1_D41B0(data);
            break;
        }
        DCFlushRange(data, 0x100);
        GXInitTexObj(obj, data, 0x40, 4, 1, 0, 0, 0);
        GXInitTexObjLOD(obj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    }
    s_rampMtx[0][0] = 0.0f;
    s_rampMtx[0][1] = 0.0f;
    s_rampMtx[1][0] = 0.0f;
    s_rampMtx[1][1] = 0.0f;
    s_rampMtx[1][2] = 0.0f;
    s_rampMtx[1][3] = 0.0f;
    s_rampMtx[2][0] = 0.0f;
    s_rampMtx[2][1] = 0.0f;
    s_rampMtx[2][2] = 0.0f;
    s_rampMtx[2][3] = 1.0f;
}

void fn_1_D3C88(u8 *p)
{
    int x;
    int y;

    memset(p, 0, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 39);

            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 31);

            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 23);

            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 15);

            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 7);

            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x - 1);

            *p = 255.0f * t;
        }
    }
}

void fn_1_D3FF0(u8 *p)
{
    int x;
    int y;

    memset(p, 0, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            if (x != 1)
                *p = 0xFF;
            else
                *p = 0;
        }
    }
}

void g_fill_some_memory_with_0x0_and_0xff(u8 *p)
{
    memset(p, 0, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
}

void fn_1_D41B0(u8 *p)
{
    int x;
    int y;

    memset(p, 0, 0x20);
    p += 0x20;
    memset(p, 0xFF, 0x20);
    p += 0x20;
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 39);

            t = fn_8007682C(1.0 + 1.718281828459045 * t);
            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 31);

            t = fn_8007682C(1.0 + 1.718281828459045 * t);
            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 23);

            t = fn_8007682C(1.0 + 1.718281828459045 * t);
            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 15);

            t = fn_8007682C(1.0 + 1.718281828459045 * t);
            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            float t = (1.0f / 47.0f) * (x + 7);

            t = fn_8007682C(1.0 + 1.718281828459045 * t);
            *p = 255.0f * t;
        }
    }
    for (y = 4; y > 0; y--)
    {
        for (x = 8; x > 0; x--, p++)
        {
            if (x != 1)
            {
                float t = (1.0f / 47.0f) * (x - 1);

                t = fn_8007682C(1.0 + 1.718281828459045 * t);
                *p = 255.0f * t;
            }
            else
                *p = 0;
        }
    }
}

void fn_1_D462C(void)
{
    s_reqCount = 0;
    s_reqNext = s_reqs;
    s_reqLayers = 0;
}

int fn_1_D4650(struct ShadowCastReq *req)
{
    Vec v;
    float lenSq;
    float tanHalf;
    float rSq;

    if (s_reqCount > 0x40)
        return 0;
    lenSq = (req->lightPos.z - req->pos.z) * (req->lightPos.z - req->pos.z)
          + ((req->lightPos.x - req->pos.x) * (req->lightPos.x - req->pos.x)
           + (req->lightPos.y - req->pos.y) * (req->lightPos.y - req->pos.y));
    rSq = req->radius;
    rSq *= rSq;
    tanHalf = (lenSq - rSq > 0.0f) ? math_sqrt(rSq / (lenSq - rSq)) : 5.6713f;
    memcpy_handler(s_reqNext, req, SHADOW_REQ_SIZE);
    s_reqNext->tanHalf = tanHalf;
    s_reqNext->invTan = 1.0f / tanHalf;
    s_reqNext->dist = math_sqrt(lenSq);
    v.x = req->pos.x - req->lightPos.x;
    v.y = req->pos.y - req->lightPos.y;
    v.z = req->pos.z - req->lightPos.z;
    vec_set_len(0.5f * req->unk10, &v, &v);
    v.x += req->pos.x;
    v.y += req->pos.y;
    v.z += req->pos.z;
    s_reqNext->eye = v;
    s_reqLayers |= req->unk30;
    s_reqNext++;
    s_reqCount++;
    return 1;
}

int fn_1_D4860(void)
{
    return s_reqCount;
}

void fn_1_D4870(void)
{
    Mtx texBias;
    Mtx lookAt;
    int i;
    struct ShadowCastReq *req;
    u16 camBit;
    int fog;
    float projScale;
    float fogNear;
    float fogFar;
    float fogScale;

    mtxa_push();
    camBit = 1 << g_current_camera->idx;
    fog = lbl_1_bss_CF22C.enabled;
    if (fog)
    {
        fogNear = lbl_1_bss_CF22C.nearDist;
        fogFar = lbl_1_bss_CF22C.farDist;
        fogScale = 1.0f / (fogFar - fogNear);
    }
    mtxa_from_identity();
    mtxa->mtxA[0][2] = 0.5f;
    mtxa->mtxA[1][2] = 0.5f;
    mtxa_to_mtx(texBias);
    projScale = g_current_camera->sub28.view.invTanHalfFov;
    for (req = s_reqs, i = s_reqCount; i > 0; i--, req++)
    {
        float min;
        float size;
        float max;
        float alpha;
        float z;

        req->alpha = 0.0f;
        if ((req->cameraMask & camBit) == 0)
            continue;
        mtxa_from_mtxb();
        if (!fn_1_233FC(&req->eye, 0.5f * req->unk10))
            continue;
        mtxa_tf_point(&req->pos, &req->viewPos);
        size = req->radius * projScale / mathutil_vec_len(&req->viewPos);
        min = req->unk34;
        if (size < min)
            continue;
        max = req->unk38;
        if (size > max)
            req->alpha = 1.0f;
        else
            req->alpha = (size - min) / (max - min);
        alpha = req->alpha;
        if (fog && !(req->tex.flags & 2))
        {
            z = -req->viewPos.z;
            if (z > fogFar)
            {
                req->alpha = 0.0f;
                continue;
            }
            if (z > fogNear)
                alpha *= 1.0f - fogScale * (z - fogNear);
        }
        req->kcolor.r = req->color.r * alpha;
        req->kcolor.g = req->color.g * alpha;
        req->kcolor.b = req->color.b * alpha;
        mtxa_tf_point(&req->lightPos, &req->viewLight);
        mtxa_tf_vec(&req->up, &req->viewUp);
        C_MTXLookAt(lookAt, &req->viewLight, &req->viewUp, &req->viewPos);
        mtxa_from_mtx(lookAt);
        s_rampMtx[0][2] = -0.875f / req->unk10;
        s_rampMtx[0][3] = 0.125f + (-0.875f * req->dist) / req->unk10;
        mtxa_mult_left(s_rampMtx);
        mtxa_to_mtx(req->rampMtx);
        mtxa_from_mtx(texBias);
        switch (req->tex.unk0)
        {
        case 0:
            {
                float s = 0.5f * req->invTan;
                mtxa_scale_xyz(s, s, 1.0f);
            }
            break;
        default:
            {
                float s = 0.5f / req->radius;
                mtxa_scale_xyz(-s, s, 0.0f);
                mtxa->mtxA[0][3] = 0.5f;
                mtxa->mtxA[1][3] = 0.5f;
                mtxa->mtxA[2][3] = 1.0f;
            }
            break;
        }
        mtxa_mult_right(lookAt);
        mtxa_to_mtx(req->texMtx);
    }
    mtxa_pop();
}

void g_some_shadow_draw_func(struct ShadowReceive *receive)
{
    struct ShadowDrawState st;
    struct ShadowReceive rcv;
    Vec center;
    Vec viewPos;
    Vec axis;
    Vec delta;
    int j;
    int i;
    struct ShadowCastReq *req;
    int started;
    void *oldFunc;
    int ramp;
    float radius;

    if (s_reqCount == 0 || s_shadowsEnabled == 0)
        return;
    started = 0;
    rcv = *receive;
    switch (rcv.modelType)
    {
    case 0:
    case 1:
    case 2:
        center = ((struct NlModel *)rcv.model)->boundSphereCenter;
        radius = ((struct NlModel *)rcv.model)->boundSphereRadius;
        break;
    default:
        center = ((struct GMAModel *)rcv.model)->boundSphereCenter;
        radius = ((struct GMAModel *)rcv.model)->boundSphereRadius;
        break;
    }
    st.stage = 0;
    st.tevStage = 0;
    st.texGen = 0;
    st.texMtx = 0x1E;
    st.numKColors = 0;
    st.texMap = 0;
    st.numTevStages = 0xFF;
    st.numTexGens = 0xFF;
    memset(st.rampMaps, 0xFF, 4);
    memset(st.kcolors, 0, 0x10);
    memset(st.stageSetUp, 0, 4);
    for (req = s_reqs, i = s_reqCount; i > 0; i--, req++)
    {
        float len;
        float d;
        float r;

        if (0.0f == req->alpha)
            continue;
        if (req->obj == rcv.obj)
            continue;
        if ((req->unk30 & rcv.layer) == 0)
            continue;
        len = req->unk10;
        mtxa_rigid_inv_tf_point(&req->viewPos, &viewPos);
        mtxa_rigid_inv_tf_point(&req->viewLight, &delta);
        axis.x = viewPos.x - delta.x;
        axis.y = viewPos.y - delta.y;
        axis.z = viewPos.z - delta.z;
        vec_normalize_len(&axis);
        delta.x = center.x - viewPos.x;
        delta.y = center.y - viewPos.y;
        delta.z = center.z - viewPos.z;
        d = mathutil_vec_dot_prod(&axis, &delta);
        if (d < -radius)
            continue;
        if (d > len + radius)
            continue;
        r = req->radius + req->tanHalf * len;
        if (mathutil_vec_sq_len(&delta) - d * d > (r + radius) * (r + radius))
            continue;
        if (!started)
        {
            if (rcv.modelType >= 3U)
                oldFunc = fn_1_10A69C(fn_1_D5A2C);
            fn_1_D5378();
            GXSetZCompLoc_cached(1);
            started = 1;
        }
        if (st.stageSetUp[st.stage] == 0)
        {
            fn_1_D53E0(st.stage);
            st.stageSetUp[st.stage] = 1;
        }
        {
            GXColor color;
            GXColor *c;
            int n;

            color = req->kcolor;
            n = st.numKColors;
            c = st.kcolors;
            for (j = 0; j < n; j++, c++)
            {
                if (c->r == color.r && c->g == color.g && c->b == color.b)
                    break;
            }
            if (j == n)
            {
                GXSetTevKColor_cached(j, color);
                st.kcolors[j] = color;
                st.numKColors++;
            }
        }
        GXSetTevKColorSel_cached(st.tevStage, j + 0xC);
        mtxa_push();
        mtxa_mult_left(req->texMtx);
        GXLoadTexMtxImm(mtxa->mtxA, st.texMtx, 0);
        st.texMtx += 3;
        mtxa_pop();
        mtxa_push();
        mtxa_mult_left(req->rampMtx);
        GXLoadTexMtxImm(mtxa->mtxA, st.texMtx, 0);
        st.texMtx += 3;
        mtxa_pop();
        GXLoadTexObj_cached(&req->tex.texObj, st.texMap);
        st.texMap++;
        ramp = req->tex.ramp;
        j = st.rampMaps[ramp];
        if (j >= 8)
        {
            j = st.texMap;
            GXLoadTexObj_cached(&s_rampTexObjs[ramp], j);
            st.rampMaps[ramp] = j;
        }
        g_GXSetTevOrder_wrapper(st.tevStage + 1, st.texGen + 1, j, 0xFF);
        st.texMap++;
        if (st.stage == 0)
            st.tevStage += 2;
        else
            st.tevStage += 3;
        st.texGen += 2;
        st.stage++;
        if (st.stage < 4)
            continue;
        {
            u8 n = st.tevStage;

            if (st.numTevStages != n)
            {
                GXSetNumTevStages_cached(n);
                st.numTevStages = n;
            }
        }
        {
            u8 n = st.texGen;

            if (st.numTexGens != n)
            {
                GXSetNumTexGens_cached(n);
                st.numTexGens = n;
            }
        }
        switch (rcv.modelType)
        {
        case 0:
        case 1:
        case 2:
            u_nl2ngc_draw_model_with_mesh_func(rcv.model, fn_1_D5A14);
            break;
        case 3:
        case 4:
        case 5:
            fn_1_10A2C0(rcv.model);
            break;
        case 6:
            avdisp_draw_model_unculled_sort_never(rcv.model);
            break;
        }
        st.stage = 0;
        st.tevStage = 0;
        st.texGen = 0;
        st.texMtx = 0x1E;
        st.numKColors = 0;
        st.texMap = 0;
    }
    if (st.stage != 0)
    {
        {
            u8 n = st.tevStage;

            if (st.numTevStages != n)
                GXSetNumTevStages_cached(n);
        }
        {
            u8 n = st.texGen;

            if (st.numTexGens != n)
                GXSetNumTexGens_cached(n);
        }
        switch (rcv.modelType)
        {
        case 0:
        case 1:
        case 2:
            u_nl2ngc_draw_model_with_mesh_func(rcv.model, fn_1_D5A14);
            break;
        case 3:
        case 4:
        case 5:
            fn_1_10A2C0(rcv.model);
            break;
        case 6:
            avdisp_draw_model_unculled_sort_never(rcv.model);
            break;
        }
    }
    if (started)
    {
        if (rcv.modelType >= 3U)
            fn_1_10A69C(oldFunc);
        fn_1_D5798();
        GXSetZCompLoc_cached(0);
    }
}

void fn_1_D5378(void)
{
    GXSetNumChans_cached(0);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 0, 2, 0);
    GXSetZMode_cached(1, 2, 0);
    avdisp_set_z_mode(1, 2, 0);
    g_something_with_raster_state(0);
}

void fn_1_D53E0(int stage)
{
    int tev;
    int coord;
    int mtx;
    int map;

    if (stage == 0)
    {
        tev = 0;
        coord = 0;
        mtx = 0x1E;
        map = 0;
        GXSetTexCoordGen2_cached(coord, 0, 0, mtx, 0, 0x7D);
        g_GXSetTevOrder_wrapper(tev, coord, map, 0xFF);
        g_GXSetTevIndirect_zero_if_different(tev);
        GXSetTevSwapMode_cached(tev, 0, 1);
        GXSetTevColorIn_cached(tev, 0xF, 0xE, 8, 0xF);
        GXSetTevColorOp_cached(tev, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(tev, 7, 7, 7, 4);
        GXSetTevAlphaOp_cached(tev, 0, 0, 0, 1, 0);
        tev++;
        coord++;
        mtx += 3;
        map++;
        GXSetTexCoordGen2_cached(coord, 0, 0, mtx, 0, 0x7D);
        g_GXSetTevIndirect_zero_if_different(tev);
        GXSetTevSwapMode_cached(tev, 0, 1);
        GXSetTevColorIn_cached(tev, 0xF, 8, 0, 0xC);
        GXSetTevColorOp_cached(tev, 1, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(tev, 7, 0, 4, 7);
        GXSetTevAlphaOp_cached(tev, 0, 0, 0, 1, 0);
    }
    else
    {
        tev = (stage - 1) * 3 + 2;
        coord = stage * 2;
        mtx = (stage << 1) * 3 + 0x1E;
        map = stage * 2;
        GXSetTexCoordGen2_cached(coord, 0, 0, mtx, 0, 0x7D);
        g_GXSetTevOrder_wrapper(tev, coord, map, 0xFF);
        g_GXSetTevIndirect_zero_if_different(tev);
        GXSetTevSwapMode_cached(tev, 0, 1);
        GXSetTevColorIn_cached(tev, 0xF, 0xE, 8, 0xF);
        GXSetTevColorOp_cached(tev, 0, 0, 0, 1, 1);
        GXSetTevAlphaIn_cached(tev, 7, 7, 7, 4);
        GXSetTevAlphaOp_cached(tev, 0, 0, 0, 1, 1);
        tev++;
        coord++;
        mtx += 3;
        map++;
        GXSetTexCoordGen2_cached(coord, 0, 0, mtx, 0, 0x7D);
        g_GXSetTevIndirect_zero_if_different(tev);
        GXSetTevSwapMode_cached(tev, 0, 1);
        GXSetTevColorIn_cached(tev, 0xF, 8, 2, 0xC);
        GXSetTevColorOp_cached(tev, 1, 0, 0, 1, 1);
        GXSetTevAlphaIn_cached(tev, 7, 1, 4, 7);
        GXSetTevAlphaOp_cached(tev, 0, 0, 0, 1, 1);
        tev++;
        g_GXSetTevOrder_wrapper(tev, 0xFF, 0xFF, 0xFF);
        g_GXSetTevIndirect_zero_if_different(tev);
        GXSetTevColorIn_cached(tev, 0xF, 2, 0, 0xF);
        GXSetTevColorOp_cached(tev, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(tev, 1, 7, 7, 0);
        GXSetTevAlphaOp_cached(tev, 0, 0, 0, 1, 0);
    }
}

void fn_1_D5798(void)
{
    avdisp_set_z_mode(1, 3, 1);
}

int fn_1_D57C4(int enabled)
{
    int old = s_shadowsEnabled;

    s_shadowsEnabled = enabled;
    return old;
}

int g_check_some_condition(u32 layers)
{
    int ret = 0;

    if (s_reqCount != 0 && s_shadowsEnabled != 0 && (s_reqLayers & layers) != 0)
        ret = 1;
    return ret;
}

float fn_1_D5824(struct ShadowCastReq *req)
{
    Vec v;
    Vec pos;
    struct Camera *camera;
    float size;
    float minSize;

    camera = g_current_camera;
    if ((req->cameraMask & (1 << g_something_with_camera3)) == 0)
        return 0.0f;
    mtxa_from_mtxb();
    pos = req->pos;
    mtxa_tf_point(&pos, &v);
    {
        float d = mathutil_vec_len(&v);

        if (0.0f == d)
            return 1000.0f;
        size = req->radius * camera->sub28.view.invTanHalfFov / d;
    }
    minSize = req->unk34;
    if (size < minSize)
        return 0.0f;
    v.x = pos.x - req->lightPos.x;
    v.y = pos.y - req->lightPos.y;
    v.z = pos.z - req->lightPos.z;
    vec_set_len(0.5f * req->unk10, &v, &v);
    v.x += pos.x;
    v.y += pos.y;
    v.z += pos.z;
    if (g_is_sphere_visible(&v, 0.5f * req->unk10))
        return size / minSize - 1.0f;
    return 0.0f;
}

int fn_1_D5A14(struct NlMesh *mesh)
{
    return (*(u32 *)mesh & 0x80) != 0;
}

int fn_1_D5A2C(void)
{
    return 1;
}

void debug_draw_shadow_textures(void)
{
    float proj[7];
    struct ShadowDebugQuad quad;
    int i;
    struct ShadowCastReq *req;
    float aspect;

    GXGetProjectionv(proj);
    aspect = proj[1] / proj[3];
    quad.x1 = -1.0f;
    quad.y1 = -1.0f;
    quad.flags = 0;
    quad.color.r = 0xFF;
    quad.color.g = 0xFF;
    quad.color.b = 0xFF;
    quad.color.a = 0xFF;
    for (req = s_reqs, i = s_reqCount; i > 0; i--, req++)
    {
        float x;
        float y;
        float z;
        float radius;
        float size;
        float hi;
        float lo;

        radius = req->radius;
        size = 0.2f;
        x = req->viewPos.x - radius;
        y = req->viewPos.y + radius;
        z = req->viewPos.z + radius;
        if (z > proj[6])
            z = proj[6];
        x = -(proj[2] + x * proj[1] / z);
        y = proj[4] + y * proj[3] / z;
        lo = -1.0f + size * aspect;
        hi = 1.0f - size * aspect;
        if (x < lo)
        {
            y *= lo / x;
            x = lo;
        }
        else if (x > hi)
        {
            y *= hi / x;
            x = hi;
        }
        lo = -1.0f + size;
        hi = 1.0f - size;
        if (y < lo)
        {
            x *= lo / y;
            y = lo;
        }
        else if (y > hi)
        {
            x *= hi / y;
            y = hi;
        }
        quad.z = z;
        quad.x1 = x - size * aspect;
        quad.y1 = y - size;
        quad.x2 = x + size * aspect;
        quad.y2 = y + size;
        x = req->viewPos.x;
        y = req->viewPos.y;
        z = req->viewPos.z;
        if (z > proj[6])
            quad.hasVerts = 0;
        else
        {
            quad.hasVerts = 1;
            quad.verts[0].x = x - radius;
            quad.verts[0].y = y + radius;
            quad.verts[0].z = z;
            quad.verts[1].x = x + radius;
            quad.verts[1].y = y + radius;
            quad.verts[1].z = z;
            quad.verts[2].x = x + radius;
            quad.verts[2].y = y - radius;
            quad.verts[2].z = z;
            quad.verts[3].x = x - radius;
            quad.verts[3].y = y - radius;
            quad.verts[3].z = z;
        }
        fn_1_81E4(&req->tex.texObj, &quad);
    }
}
