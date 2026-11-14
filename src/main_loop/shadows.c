/* Shadows cast by stage models, goals and wormholes (SMB2-only). In single player each caster is
 * rendered to its own small texture first; otherwise a generic blob shadow texture is used. */
#include "types.h"
#include "avdisp.h"
#include "camera.h"
#include "gx.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "stage.h"

struct ShadowTexInfo
{
    /*0x00*/ u8 unk0;
    /*0x04*/ GXTexObj texObj;
};  /* size = 0x24 */

/* Request handed to the shadow renderer (same layout as ball_draw's). */
struct ShadowCastReq
{
    /*0x00*/ void *obj;
    /*0x04*/ Vec lightPos;
    /*0x10*/ float unk10;
    /*0x14*/ Vec pos;
    /*0x20*/ float radius;
    /*0x24*/ Vec up;
    /*0x30*/ s16 unk30;
    /*0x32*/ u16 cameraMask;
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ GXColor color;
    /*0x40*/ struct ShadowTexInfo tex;
};  /* size = 0x64 */

/* .bss (0x711F8 - 0x71210) */
static struct
{
    /*0x00*/ void *heap;
    /*0x04*/ s32 size;
    /*0x08*/ u8 *buf;  /* render-to-texture buffer, one 64x64 texture per caster at most */
} lbl_1_bss_711F8;
static u32 s_shadowFlags;  /* 1: TEV set up, 2: viewport changed, 4: copy clear colour set */
static u16 s_shadowCurW;
static u16 s_shadowCurH;
static u16 s_shadowMaxW;
static u16 s_shadowMaxH;

struct ShadowTexInfo lbl_1_data_2F200 = { 1 };

extern void *currentHeap;
extern struct GMAModel *wormhole_gma_model;
extern struct GMAModel *goal_gma_models[3];
extern struct Light s_lightPool[];
extern struct BgLightInfo lbl_1_bss_2F48;  /* light.c's s_bgLightInfo */
extern struct
{
    u8 filler0[0x58];
    /*0x58*/ GXColor bgColor;
} g_some_render_flag;

void *OSAllocFromHeap(void *heap, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
u8 GXGetTexObjMipMap(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp,
    u8 doEdgeLod, int maxAniso);
void GXSetCopyClear_cached(GXColor color, u32 z);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetCullMode_cached(int mode);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void g_something_with_raster_state(int a);
void g_smth_with_bg_color_drawing(GXColor *color);
void C_MTXLookAt(Mtx m, Vec *camPos, Vec *camUp, Vec *target);
void C_MTXOrtho(Mtx44 m, float t, float b, float l, float r, float n, float f);
struct GMAModel *fn_1_58280(struct StageModelRef *ref);
void fn_1_D4650(struct ShadowCastReq *req);
float fn_1_D5824(struct ShadowCastReq *req);
void fn_1_10A69C(void *func);
void fn_1_E2E18(void);

int g_something_with_shadow_cast(void);
void fn_1_5C460(void);
void fn_1_5C494(void);
int fn_1_5C60C(struct GMAModel *model, struct ShadowCastReq *req, u8 **bufPtr, u8 *bufEnd);
void fn_1_5CA5C(void);
int fn_1_5CB40(void);

void g_init_shadows(void)
{
    u32 texSize;
    u32 count;
    void *heap;
    int size;

    heap = currentHeap;
    lbl_1_bss_711F8.heap = heap;
    count = g_something_with_shadow_cast();
    if (count == 0)
    {
        lbl_1_bss_711F8.buf = NULL;
        lbl_1_bss_711F8.size = 0;
    }
    else
    {
        texSize = GXGetTexBufferSize(64, 64, 0, 0, 0);
        for (size = texSize * count; size > 0; size -= texSize)
        {
            lbl_1_bss_711F8.buf = OSAllocFromHeap(heap, size);
            if (lbl_1_bss_711F8.buf != NULL)
                break;
        }
        if (lbl_1_bss_711F8.buf != NULL)
            lbl_1_bss_711F8.size = size;
    }
}

void fn_1_5BC10(void)
{
    void *heap = lbl_1_bss_711F8.heap;

    if (lbl_1_bss_711F8.buf != NULL)
    {
        OSFreeToHeap(heap, lbl_1_bss_711F8.buf);
        lbl_1_bss_711F8.buf = NULL;
        lbl_1_bss_711F8.size = 0;
    }
}

void g_draw_goal_and_wormhole_shadows(void)
{
    u8 *bufPtr;
    Vec lightOfs;
    Vec center;
    struct ShadowCastReq req;
    float red, green, blue;
    struct StageCollHeader *coll;
    struct ItemGroup *ig;
    struct StageWormhole *wormhole;
    struct StageModelRef * DPTR *refs;
    struct StageGoal *goal;
    int i;
    GXTexObj *tex;
    int j;
    int count;
    int refCount;
    struct Light *light;
    struct GMAModel *model;
    int lastIg;
    u8 *bufEnd;
    BOOL single;

    if (modeCtrl.unk30 > 1)
        single = FALSE;
    else
        single = TRUE;

    light = s_lightPool;
    mtxa_from_rotate_y(light->rotY);
    mtxa_rotate_x(light->rotX);
    lightOfs.x = 0.0f;
    lightOfs.y = 0.0f;
    lightOfs.z = -15.0f;
    mtxa_tf_vec(&lightOfs, &lightOfs);

    req.unk30 = 0x44;
    req.unk34 = 0.075f;
    req.unk38 = 0.1f;
    req.up.x = 0.0f;
    req.up.y = 1.0f;
    req.up.z = 0.0f;
    mtxa_tf_vec(&req.up, &req.up);
    req.cameraMask = 0xFFFF;
    red = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.r));
    green = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.g));
    blue = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.b));

    req.color.r = red;
    req.color.g = green;
    req.color.b = blue;

    tex = &init_common_gma->modelEntries[94].model->texObjs[0];
    GXInitTexObj(&lbl_1_data_2F200.texObj,
        GXGetTexObjData(tex),
        GXGetTexObjWidth(tex),
        GXGetTexObjHeight(tex),
        GXGetTexObjFmt(tex),
        0,
        0,
        GXGetTexObjMipMap(tex));
    GXInitTexObjLOD(&lbl_1_data_2F200.texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);

    if (single)
        fn_1_5C460();

    lastIg = -1;
    bufPtr = lbl_1_bss_711F8.buf;
    bufEnd = lbl_1_bss_711F8.buf + lbl_1_bss_711F8.size;
    count = stagedef->itemGroupCount;
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < count; i++, coll++, ig++)
    {
        refCount = coll->modelRefCount;
        refs = coll->modelRefs;
        for (j = 0; j < refCount; j++, refs++)
        {
            if (!((*refs)->flags & 1))
                continue;
            req.obj = refs;
            model = fn_1_58280(*refs);
            if (model == NULL)
                continue;
            if (lastIg != i)
            {
                mtxa_from_mtx(ig->transform);
                lastIg = i;
            }
            mtxa_tf_point(&model->boundSphereCenter, &center);
            req.lightPos.x = lightOfs.x + center.x;
            req.lightPos.y = lightOfs.y + center.y;
            req.lightPos.z = lightOfs.z + center.z;
            req.unk10 = 8.0f * model->boundSphereRadius;
            req.pos = center;
            req.radius = model->boundSphereRadius;
            req.tex = lbl_1_data_2F200;
            if (!single || fn_1_5C60C(model, &req, &bufPtr, bufEnd))
                fn_1_D4650(&req);
        }

        model = wormhole_gma_model;
        if (model != NULL && coll->wormholeCount != 0)
        {
            req.unk10 = 3.0f * model->boundSphereRadius;
            req.radius = model->boundSphereRadius;
            wormhole = coll->wormholes;
            for (j = coll->wormholeCount; j > 0; j--, wormhole++)
            {
                if (!(wormhole->unk0 & 1))
                    continue;
                if (lastIg != i)
                {
                    mtxa_from_mtx(ig->transform);
                    lastIg = i;
                }
                mtxa_push();
                mtxa_translate(&wormhole->pos);
                mtxa_rotate_z(wormhole->rotZ);
                mtxa_rotate_y(wormhole->rotY);
                mtxa_rotate_x(wormhole->rotX);
                req.obj = wormhole;
                mtxa_tf_point(&model->boundSphereCenter, &center);
                req.lightPos.x = lightOfs.x + center.x;
                req.lightPos.y = lightOfs.y + center.y;
                req.lightPos.z = lightOfs.z + center.z;
                req.pos = center;
                req.tex = lbl_1_data_2F200;
                if (single && !fn_1_5C60C(model, &req, &bufPtr, bufEnd))
                    mtxa_pop();
                else
                {
                    fn_1_D4650(&req);
                    mtxa_pop();
                }
            }
        }

        if (coll->goalCount != 0)
        {
            goal = coll->goals;
            for (j = coll->goalCount; j > 0; j--, goal++)
            {
                if (!(goal->flags & 1))
                    continue;
                model = goal_gma_models[goal->type];
                if (lastIg != i)
                {
                    mtxa_from_mtx(ig->transform);
                    lastIg = i;
                }
                mtxa_push();
                mtxa_translate(&goal->pos);
                mtxa_rotate_z(goal->rotZ);
                mtxa_rotate_y(goal->rotY);
                mtxa_rotate_x(goal->rotX);
                req.obj = goal;
                req.unk10 = 3.0f * model->boundSphereRadius;
                req.radius = model->boundSphereRadius;
                mtxa_tf_point(&model->boundSphereCenter, &center);
                req.lightPos.x = lightOfs.x + center.x;
                req.lightPos.y = lightOfs.y + center.y;
                req.lightPos.z = lightOfs.z + center.z;
                req.pos = center;
                req.tex = lbl_1_data_2F200;
                if (single && !fn_1_5C60C(model, &req, &bufPtr, bufEnd))
                    mtxa_pop();
                else
                {
                    fn_1_D4650(&req);
                    mtxa_pop();
                }
            }
        }
    }

    if (single)
        fn_1_5C494();
}

/* Counts the stage models, wormholes and goals that cast a shadow. */
int g_something_with_shadow_cast(void)
{
    int n;
    struct StageCollHeader *coll;
    struct ItemGroup *ig;
    struct StageModelRef * DPTR *refs;
    int i;
    int j;
    int count;
    int refCount;
    struct StageWormhole *wormhole;
    struct StageGoal *goal;

    n = 0;
    count = stagedef->itemGroupCount;
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < count; i++, coll++, ig++)
    {
        refCount = coll->modelRefCount;
        refs = coll->modelRefs;
        for (j = 0; j < refCount; j++, refs++)
        {
            if ((*refs)->flags & 1)
                n++;
        }
        wormhole = coll->wormholes;
        for (j = coll->wormholeCount; j > 0; j--, wormhole++)
        {
            if (wormhole->unk0 & 1)
                n++;
        }
        goal = coll->goals;
        for (j = coll->goalCount; j > 0; j--, goal++)
        {
            if (goal->flags & 1)
                n++;
        }
    }
    return n;
}

void fn_1_5C460(void)
{
    s_shadowFlags = 0;
    s_shadowCurW = 0;
    s_shadowCurH = 0;
    s_shadowMaxW = 0;
    s_shadowMaxH = 0;
}

/* Restores the GX state changed while rendering shadow textures. */
void fn_1_5C494(void)
{

    if (s_shadowFlags & 1)
    {
        fn_1_10A69C(NULL);
        fn_1_E2E18();
    }
    if (s_shadowFlags & 4)
    {
        if (s_shadowCurW != s_shadowMaxW || s_shadowCurH != s_shadowMaxH)
        {
            GXSetViewport(0.0f, 0.0f, s_shadowMaxW, s_shadowMaxH, 0.0f, 1.0f);
            GXSetScissor_cached(0, 0, s_shadowMaxW, s_shadowMaxH);
            s_shadowFlags |= 2;
        }
        g_smth_with_bg_color_drawing(&g_some_render_flag.bgColor);
        GXSetCopyClear_cached(g_some_render_flag.bgColor, 0xFFFFFF);
        s_shadowFlags |= 2;
    }
    if (s_shadowFlags & 2)
        camera_apply_viewport(g_something_with_camera3);
}

/* Renders the caster's silhouette from the light into a texture taken from the shadow buffer. */
int fn_1_5C60C(struct GMAModel *model, struct ShadowCastReq *req, u8 **bufPtr, u8 *bufEnd)
{
    Vec eye;
    Mtx lookAt;
    Mtx44 proj;
    float size;
    int limit;
    u32 texSize;
    int texW;
    int texH;
    int w;
    int h;

    mtxa_push();
    size = fn_1_D5824(req);
    mtxa_pop();
    if (size <= 0.0f)
        return 0;
    limit = 64.0f * size;
    w = 128;
    h = 128;
    while (w > 4 && limit < w)
    {
        w >>= 1;
        h >>= 1;
    }
    if (w <= 4)
        return 1;
    texW = w >> 1;
    texH = h >> 1;
    texSize = GXGetTexBufferSize(texW, texH, 0, 0, 0);
    if (bufEnd - *bufPtr < texSize)
        return 0;
    if (!(s_shadowFlags & 1))
    {
        fn_1_5CA5C();
        s_shadowFlags |= 1;
    }

    req->radius *= (float)(w + 4) / (float)w;
    size = req->radius;
    eye = req->lightPos;
    eye.x -= req->pos.x;
    eye.y -= req->pos.y;
    eye.z -= req->pos.z;
    vec_set_len(2.0f * size, &eye, &eye);
    eye.x += req->pos.x;
    eye.y += req->pos.y;
    eye.z += req->pos.z;
    C_MTXLookAt(lookAt, &eye, &req->up, &req->pos);
    C_MTXOrtho(proj, -size, size, size, -size, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(proj, 1);

    if (s_shadowCurW != w || s_shadowCurH != h)
    {
        GXSetViewport(0.0f, 0.0f, w, h, 0.0f, 1.0f);
        GXSetScissor_cached(0, 0, w, h);
        s_shadowCurW = w;
        s_shadowCurH = h;
    }
    s_shadowFlags |= 2;
    if (s_shadowMaxW < w || s_shadowMaxH < h)
    {
        GXColor clearColor = {0};
        g_smth_with_bg_color_drawing(&clearColor);
        if (s_shadowMaxW < w)
            s_shadowMaxW = w;
        if (s_shadowMaxH < h)
            s_shadowMaxH = h;
        if (!(s_shadowFlags & 4))
        {
            GXSetCopyClear_cached(clearColor, 0xFFFFFF);
            s_shadowFlags |= 4;
        }
    }

    mtxa_push();
    {
        GXColor white;
        white.r = 0xFF;
        white.g = 0xFF;
        white.b = 0xFF;
        GXSetTevKColor_cached(0, white);
    }
    mtxa_mult_left(lookAt);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(model);
    mtxa_pop();

    GXSetTexCopySrc_cached(0, 0, w, h);
    GXSetTexCopyDst_cached(texW, texH, 0x20, 1);
    GXCopyTex(*bufPtr, 1);
    GXInitTexObj(&req->tex.texObj, *bufPtr, texW, texH, 0, 0, 0, 0);
    GXInitTexObjLOD(&req->tex.texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    *bufPtr = (u8 *)((u32 *)*bufPtr + (texSize >> 2));
    return 1;
}

/* TEV setup for shadow textures: constant white, alpha from the constant colour. */
void fn_1_5CA5C(void)
{
    GXSetCullMode_cached(2);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 0xFF);
    GXSetTevKColorSel_cached(0, 12);
    GXSetTevKAlphaSel_cached(0, 0);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevColorIn_cached(0, 15, 15, 15, 14);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    g_something_with_raster_state(0);
    fn_1_10A69C(fn_1_5CB40);
}

int fn_1_5CB40(void)
{
    return 1;
}
