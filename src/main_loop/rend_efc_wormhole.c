/* Wormhole surfaces: renders the view through the destination wormhole into a
 * texture, maps it onto the surface model, and draws the rippling distortion
 * particles and the full-screen flash shown after warping. */

#pragma peephole off

#include "types.h"
#include "gx.h"
#include "ball.h"
#include "avdisp.h"
#include "camera.h"
#include "event.h"
#include "frustum.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "pool.h"
#include "rand.h"
#include "stage.h"

#define EV_RUNNING 2
#define EV_SUSPENDED 4

struct RenderEffect
{
    /*0x00*/ s8 state;
    u8 filler1[5];
    /*0x06*/ u16 cameraMask;
    /*0x08*/ u32 enableFlags;
    u8 fillerC[4];
    /*0x10*/ void *work;
    /*0x14*/ void *funcs;
};  /* size = 0x18 */

/* Screen-space bounds of the visible surfaces, in [-1, 1] */
struct WormholeBounds
{
    /*0x00*/ float left;
    /*0x04*/ float top;
    /*0x08*/ float right;
    /*0x0C*/ float bottom;
    /*0x10*/ float nearZ;
};  /* size = 0x14 */

struct WormholeParticle
{
    /*0x00*/ s16 type;
    /*0x02*/ s16 state;
    /*0x04*/ s16 life;
    u8 filler6[2];
    /*0x08*/ Vec pos;
    /*0x14*/ Vec vel;
    /*0x20*/ float scale;
    /*0x24*/ float scaleVel;
    /*0x28*/ S16Vec rot;
    /*0x2E*/ S16Vec rotVel;
    /*0x34*/ float alpha;
    /*0x38*/ u32 flags;
    /*0x3C*/ struct GMAModel *model;
};  /* size = 0x40 */

struct RenderEffectWormhole
{
    /*0x0000*/ struct WormholeBounds bounds;
    /*0x0014*/ Mtx texMtx;
    /*0x0044*/ int hasViewTex;
    /*0x0048*/ GXTexObj viewTex;
    /*0x0068*/ void *viewBuf;
    /*0x006C*/ int hasRippleTex;
    /*0x0070*/ GXTexObj rippleTex;
    /*0x0090*/ void *rippleBuf;
    /*0x0094*/ GXTexObj noiseTex;
    /*0x00B4*/ GXTexObj flashTex;
    /*0x00D4*/ void *flashBuf;
    /*0x00D8*/ int hasFlashTex;
    /*0x00DC*/ GXTexObj flashNoiseTex;
    /*0x00FC*/ int nearestIdx;
    /*0x0100*/ Vec srcPos;
    /*0x010C*/ Vec srcVel;
    /*0x0118*/ Vec srcFwd;
    /*0x0124*/ Vec srcUp;
    /*0x0130*/ Vec dstPos;
    /*0x013C*/ Vec dstFwd;
    /*0x0148*/ Vec dstUp;
    /*0x0154*/ int flashTimer;
    /*0x0158*/ float flashScale;
    /*0x015C*/ s16 activeCount;
    /*0x015E*/ s16 nextIdx;
    /*0x0160*/ s8 used[0x40];
    /*0x01A0*/ struct WormholeParticle particles[0x40];
};  /* size = 0x11A0 */

struct WormholeSurfaceNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ float alpha;
    /*0x0C*/ Mtx mtx;
    /*0x3C*/ struct GMAModel *model;
    /*0x40*/ struct RenderEffectWormhole *work;
};  /* size = 0x44 */

extern struct
{
    /*0x00*/ u32 flags;
    /*0x04*/ Vec unk4;
    /*0x10*/ Vec unk10;
    /*0x1C*/ u32 unk1C;
    /*0x20*/ Vec unk20;
    /*0x2C*/ Vec unk2C;
    /*0x38*/ u32 unk38;
    /*0x3C*/ Vec unk3C;
    /*0x48*/ Vec unk48;
    u8 filler54[4];
    /*0x58*/ GXColor bgColor;
} g_some_render_flag;

extern s32 stage_heap;
extern u32 g_some_other_flags;
extern struct GMAModel *wormhole_gma_model;
extern struct GMAModel *worm_surface_gma_model;

u32 lbl_1_bss_CF220[2];

void *OSAllocFromHeap(int heap, u32 size);
void OSFreeToHeap(int heap, void *ptr);
void *memset(void *dest, int c, u32 n);
void *memcpy_handler(void *dest, const void *src, u32 n);
void C_MTXLookAt(Mtx m, Vec *camPos, Vec *camUp, Vec *target);
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *out);

void fn_1_D4870(void);
int fn_1_D57C4(int enabled);
void fn_1_E2E18(void);
void fn_1_E2F1C(void);
void g_set_something3(int arg0);
void g_draw_stage(void);
void g_draw_bg(void);
void g_draw_stobjs(void);
void g_maybe_some_bg_effect_handler(void);
void g_smth_with_rendefc_reflective_height(float height);
void g_smth_with_projection_mtx(float *projParams);
void g_smth_with_bg_color_drawing(GXColor *color);
void g_something_with_GX_vtx_desc(u32 attrs);
void g_something_with_raster_state(int a);
void ord_tbl_draw_nodes(void);
void *ord_tbl_alloc_node(u32 size);
struct OrdTblNode *fn_1_CCC18(float z);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void *fn_1_10A69C(void *func);
float fn_1_59704(struct GMAModel *model);

u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
u8 GXGetTexObjMipMap(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp, u8 doEdgeLod, int maxAniso);
void GXLoadPosMtxImm(Mtx m, u32 id);
void load_gx_pos_nrm_mtx(Mtx m, int id);
void GXLoadTexMtxImm(Mtx m, u32 id, int type);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetCullMode_cached(int mode);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int matrixSel, int wrapS, int wrapT, u8 addPrev, u8 utcLod, int alphaSel);
void GXSetIndTexOrder_cached(int indStage, int coord, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, u32 mtx, int normalize, int postMtx);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXCopyTex(void *dest, u8 clear);
void GXInvalidateTexAll(void);
void GXSetViewport(float left, float top, float wd, float ht, float nearz, float farz);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], s8 scale);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);

void fn_1_DF540(struct RenderEffect *rendEfc);
void fn_1_DF6C8(struct RenderEffect *rendEfc);
void fn_1_DFB00(struct RenderEffectWormhole *work, struct WormholeParticle *src);
void fn_1_DFB6C(struct WormholeParticle *p, s8 *used);
void fn_1_E00A8(struct WormholeParticle *p, s8 *used);
void fn_1_E03FC(struct RenderEffect *rendEfc);
void fn_1_E0B10(struct RenderEffect *rendEfc);
void fn_1_E0DCC(struct RenderEffect *rendEfc);
void fn_1_E0E10(struct RenderEffect *rendEfc);
void fn_1_E10AC(struct OrdTblNode *node);
void fn_1_E112C(struct RenderEffectWormhole *work, float alpha);
void fn_1_E1160(struct RenderEffectWormhole *work, float alpha);
void fn_1_E15CC(struct RenderEffectWormhole *work, float alpha);
void fn_1_E17BC(struct RenderEffectWormhole *work);
int fn_1_E1954(void);
void fn_1_E195C(struct RenderEffectWormhole *work);
int fn_1_E1DC0(struct RenderEffectWormhole *work);
void fn_1_E2164(struct RenderEffect *rendEfc);
void fn_1_E23B4(struct RenderEffectWormhole *work);

void g_smth_with_wormhole_surfaces(struct RenderEffect *rendEfc)
{
    struct RenderEffectWormhole *work;

    if (stagedef->wormholeCount == 0)
    {
        rendEfc->state = 0;
        return;
    }
    rendEfc->enableFlags = 0x1C;
    work = OSAllocFromHeap(stage_heap, sizeof(*work));
    if (work == NULL)
    {
        rendEfc->state = 0;
        rendEfc->work = NULL;
        return;
    }
    rendEfc->work = work;
    memset(work, 0, sizeof(*work));
    work->srcFwd.z = -1.0f;
    work->dstFwd.z = -1.0f;
    work->srcUp.y = 1.0f;
    work->dstUp.y = 1.0f;
    work->viewBuf = OSAllocFromHeap(stage_heap, GXGetTexBufferSize(0x80, 0x80, 4, 0, 0));
    work->hasViewTex = 0;
    work->rippleBuf = OSAllocFromHeap(stage_heap, GXGetTexBufferSize(0x40, 0x40, 1, 0, 0));
    work->flashBuf = OSAllocFromHeap(stage_heap, GXGetTexBufferSize(0x140, 0xE0, 4, 0, 0));
    {
        u8 mipmap;
        int format;
        u16 height;
        u16 width;
        GXTexObj *tex;

        tex = init_common_gma->modelEntries[0x5E].model->texObjs;
        mipmap = GXGetTexObjMipMap(tex);
        format = GXGetTexObjFmt(tex);
        height = GXGetTexObjHeight(tex);
        width = GXGetTexObjWidth(tex);
        GXInitTexObj(&work->noiseTex, GXGetTexObjData(tex), width, height, format, 2, 2, mipmap);
        GXInitTexObjLOD(&work->noiseTex, 5, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    }
    {
        GXTexObj *tex;
        u8 mipmap;
        int format;
        u16 height;
        u16 width;

        tex = init_common_gma->modelEntries[0x5E].model->texObjs;
        mipmap = GXGetTexObjMipMap(tex);
        format = GXGetTexObjFmt(tex);
        height = GXGetTexObjHeight(tex);
        width = GXGetTexObjWidth(tex);
        GXInitTexObj(&work->flashNoiseTex, GXGetTexObjData(tex), width, height, format, 0, 0, mipmap);
        GXInitTexObjLOD(&work->noiseTex, 5, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    }
    if (work->viewBuf == NULL || work->rippleBuf == NULL || work->flashBuf == NULL)
    {
        fn_1_DF540(rendEfc);
        rendEfc->state = 0;
        return;
    }
}

void fn_1_DF540(struct RenderEffect *rendEfc)
{
    struct RenderEffectWormhole *work = rendEfc->work;

    if (rendEfc->work != NULL)
    {
        if (work->flashBuf != NULL)
            OSFreeToHeap(stage_heap, work->flashBuf);
        if (work->rippleBuf != NULL)
            OSFreeToHeap(stage_heap, work->rippleBuf);
        if (work->viewBuf != NULL)
            OSFreeToHeap(stage_heap, work->viewBuf);
        OSFreeToHeap(stage_heap, rendEfc->work);
        rendEfc->work = NULL;
    }
}

void fn_1_DF5FC(struct RenderEffect *rendEfc)
{
    struct RenderEffectWormhole *work = rendEfc->work;

    if (!(g_some_other_flags & 0xA))
    {
        if (work->flashTimer > 0)
        {
            work->flashTimer -= 4;
            work->flashScale *= 0.99f;
            if (work->flashTimer < 0x40)
                work->flashTimer = 0;
        }
        if (lbl_1_bss_CF220[0] & 1)
        {
            work->flashTimer = 0xF0;
            work->flashScale = 0.025f;
            lbl_1_bss_CF220[0] &= ~1;
        }
        fn_1_DF6C8(rendEfc);
    }
}

void fn_1_DF6C8(struct RenderEffect *rendEfc)
{
    struct RenderEffectWormhole *work = rendEfc->work;
    int i;
    s8 *status;
    struct Ball *ball;
    float dist;
    float radius;
    Vec d;
    Vec pos;
    Vec vel;
    Vec normal;
    struct WormholeParticle p;

    pos = work->srcPos;
    vel = work->srcVel;
    normal = work->srcFwd;
    status = g_poolInfo.playerPool.statusList;
    ball = balls;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, ball++, status++)
    {
        if (*status == 0)
            return;
        radius = 20.0f * ball->currRadius;
        d = ball->pos;
        d.x -= pos.x;
        d.y -= pos.y;
        d.z -= pos.z;
        dist = __fabs(mathutil_vec_dot_prod(&d, &normal));
        if (dist < radius && RAND_FLOAT() < 1.0f - dist / radius)
        {
            memset(&p, 0, sizeof(p));
            if (rand() & 1)
                p.type = 0;
            else
            {
                p.type = 1;
                vec_to_euler(&normal, &p.rot);
            }
            d.x += dist * (RAND_FLOAT() - 0.5f);
            d.y += dist * (RAND_FLOAT() - 0.5f);
            d.z += dist * (RAND_FLOAT() - 0.5f);
            dist = mathutil_vec_dot_prod(&d, &normal);
            dist = -dist;
            p.pos = pos;
            p.pos.x += d.x + dist * normal.x;
            p.pos.y += d.y + dist * normal.y;
            p.pos.z += d.z + dist * normal.z;
            p.vel = vel;
            fn_1_DFB00(work, &p);
        }
    }

    {
        struct WormholeParticle *part;
        s8 *used;
        int count;

        count = 0;
        used = work->used;
        part = work->particles;
        for (i = 0x40; i > 0; i--, used++, part++)
        {
            if (*used == 0)
                continue;
            if (part->life > 0)
            {
                part->life--;
                if (part->life == 0)
                {
                    *used = 0;
                    continue;
                }
            }
            switch (part->type)
            {
            case 0:
                fn_1_DFB6C(part, used);
                break;
            case 1:
                fn_1_E00A8(part, used);
                break;
            }
            if (*used != 0)
                count++;
        }
        work->activeCount = count;
    }
}

void fn_1_DFB00(struct RenderEffectWormhole *work, struct WormholeParticle *src)
{
    struct WormholeParticle *p;
    s8 *used;

    used = &work->used[work->nextIdx];
    p = &work->particles[work->nextIdx];
    work->nextIdx--;
    if (work->nextIdx < 0)
        work->nextIdx = 0x3F;
    *used = 1;
    memcpy_handler(p, src, sizeof(*p));
}

void fn_1_DFB6C(struct WormholeParticle *p, s8 *used)
{
    Vec sp8;

    switch (p->state)
    {
    case 0:
        p->state = 1;
        p->model = init_common_gma->modelEntries[0x2B].model;
        p->scale = 0.1f * (0.25f + 0.5f * RAND_FLOAT());
        p->vel.x *= 2.0f;
        p->vel.y *= 2.0f;
        p->vel.z *= 2.0f;
        p->vel.x += 0.2f * (RAND_FLOAT() - 0.5f);
        p->vel.y += 0.2f * (RAND_FLOAT() - 0.5f);
        p->vel.z += 0.2f * (RAND_FLOAT() - 0.5f);
        p->rot.x = rand() & 0x7FFF;
        p->rot.y = rand() & 0x7FFF;
        p->rot.z = rand() & 0x7FFF;
        p->rotVel.x = (rand() & 0xFFF) - 0x800;
        p->rotVel.y = (rand() & 0xFFF) - 0x800;
        p->rotVel.z = (rand() & 0xFFF) - 0x800;
        p->model = init_common_gma->modelEntries[(rand() & 0x7FFF) % 7 + 0x2B].model;
        p->life = 60.0f * (0.125f + RAND_FLOAT());
        p->flags |= 1;
    }
    p->vel.y += 0.012f;
    mtxa_from_rotate_y(p->rot.y);
    mtxa_rotate_x(p->rot.x);
    mtxa_rotate_z(p->rot.z);
    mtxa_rigid_inv_tf_vec(&p->vel, &sp8);
    sp8.x *= 0.96f;
    sp8.y *= 0.8f;
    sp8.z *= 0.85f;
    mtxa_tf_vec(&sp8, &p->vel);
    p->pos.x += p->vel.x;
    p->pos.y += p->vel.y;
    p->pos.z += p->vel.z;
    p->rotVel.x += (s16)(1048576.0f * p->vel.z);
    p->rotVel.y += (s16)(1048576.0f * p->vel.y);
    p->rotVel.z += (s16)(1048576.0f * p->vel.x);
    p->rotVel.x = 0.98f * p->rotVel.x;
    p->rotVel.y = 0.985f * p->rotVel.y;
    p->rotVel.z = 0.99f * p->rotVel.z;
    p->rot.x += p->rotVel.x;
    p->rot.y += p->rotVel.y;
    p->rot.z += p->rotVel.z;
    if (p->life < 15.0f)
        p->alpha *= (float)p->life / (float)(p->life + 1);
    else
        p->alpha += 0.2f * (0.3f - p->alpha);
}

void fn_1_E00A8(struct WormholeParticle *p, s8 *used)
{
    switch (p->state)
    {
    case 0:
        p->state = 1;
        p->model = init_common_gma->modelEntries[0x2B].model;
        p->scale = 0.1f * RAND_FLOAT();
        p->scaleVel = 0.0050000004f + 0.010000001f * RAND_FLOAT();
        p->life = 60.0f * (0.15f + RAND_FLOAT());
    case 1:
        p->vel.x *= 0.98f;
        p->vel.y *= 0.98f;
        p->vel.z *= 0.98f;
        p->pos.x += p->vel.x;
        p->pos.y += p->vel.y;
        p->pos.z += p->vel.z;
        p->scale += p->scaleVel;
        if (p->life < 15.0f)
        {
            p->scaleVel *= 0.98f;
            p->alpha *= (float)p->life / (float)(p->life + 1);
        }
        else
            p->alpha += 0.2f * (0.5f - p->alpha);
    }
}

void fn_1_E02F8(int phase, struct RenderEffect *rendEfc)
{
    struct RenderEffectWormhole *work = rendEfc->work;

    if (!(g_some_render_flag.flags & 0x202))
    {
        switch (phase)
        {
        case 4:
            if (fn_1_E1DC0(work))
            {
                g_some_render_flag.flags |= 0x80;
                fn_1_E0B10(rendEfc);
                fn_1_E03FC(rendEfc);
                fn_1_E0DCC(rendEfc);
                g_some_render_flag.flags &= ~0x80;
            }
            else
            {
                work->hasViewTex = 0;
                work->hasRippleTex = 0;
            }
            break;
        case 0x10:
            fn_1_E0E10(rendEfc);
            fn_1_E195C(work);
            break;
        case 8:
            fn_1_E2164(rendEfc);
            break;
        }
    }
}

void fn_1_E03FC(struct RenderEffect *rendEfc)
{
    struct RenderEffectWormhole *work = rendEfc->work;
    int width;
    int height;
    int texWidth;
    int texHeight;
    int prevShadow;
    float left;
    float top;
    float right;
    float bottom;
    int l;
    int t;
    int r;
    int b;
    Mtx mtx;
    float proj[7];
    float newProj[7];
    Vec target;
    S16Vec rot;

    GXGetProjectionv(proj);
    target.x = work->dstPos.x + work->dstFwd.x;
    target.y = work->dstPos.y + work->dstFwd.y;
    target.z = work->dstPos.z + work->dstFwd.z;
    C_MTXLookAt(mtxa->mtxA, &work->dstPos, &work->dstUp, &target);
    mtxa_to_mtx(mtx);
    target.x = work->srcPos.x + work->srcFwd.x;
    target.y = work->srcPos.y + work->srcFwd.y;
    target.z = work->srcPos.z + work->srcFwd.z;
    C_MTXLookAt(mtxa->mtxA, &work->srcPos, &work->srcUp, &target);
    mtxa_rigid_invert();
    mtxa_mult_right(mtx);
    mtxa_push();
    mtxa_mult_left(mtxa->mtxB);
    mtxa_to_mtx(mtxa->mtxB);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(g_ord_tbl_stuff->matrices[3]);
    mtxa_to_mtx(g_ord_tbl_stuff->matrices[3]);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(g_ord_tbl_stuff->matrices[0]);
    mtxa_to_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(g_ord_tbl_stuff->matrices[4]);
    mtxa_to_mtx(g_ord_tbl_stuff->matrices[4]);
    mtxa_pop();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(g_something_with_camera3);
    left = 0.5f * (1.0f + work->bounds.left);
    top = 0.5f * (1.0f + work->bounds.top);
    right = 0.5f * (1.0f + work->bounds.right);
    bottom = 0.5f * (1.0f + work->bounds.bottom);
    l = 256.0f * left;
    t = 256.0f * top;
    r = 256.0f * right;
    b = 256.0f * bottom;
    l &= ~7;
    t &= ~7;
    r = (r + 7) & ~7;
    b = (b + 7) & ~7;
    if (l < 0)
        l = 0;
    else if (l > 0x100)
        l = 0x100;
    if (r < 0)
        r = 0;
    else if (r > 0x100)
        r = 0x100;
    if (t < 0)
        t = 0;
    else if (t > 0x100)
        t = 0x100;
    if (b < 0)
        b = 0;
    else if (b > 0x100)
        b = 0x100;
    if (l == r)
        r += 8;
    if (t == b)
        b += 8;
    width = r - l;
    height = b - t;
    texWidth = width >> 1;
    texHeight = height >> 1;
    left = 2.0f * ((float)l / 256.0f) - 1.0f;
    top = 2.0f * ((float)t / 256.0f) - 1.0f;
    right = 2.0f * ((float)r / 256.0f) - 1.0f;
    bottom = 2.0f * ((float)b / 256.0f) - 1.0f;
    memcpy_handler(newProj, proj, sizeof(proj));
    newProj[1] *= 2.0f / (right - left);
    newProj[2] += (right + left) / (right - left);
    newProj[3] *= 2.0f / (bottom - top);
    newProj[4] += (bottom + top) / (bottom - top);
    newProj[6] = work->bounds.nearZ;
    g_smth_with_projection_mtx(newProj);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = 2.0f / (right - left);
    mtxa->mtxA[0][2] = -((1.0f + left) / (right - left));
    mtxa->mtxA[1][1] = 2.0f / (bottom - top);
    mtxa->mtxA[1][2] = -((1.0f - bottom) / (bottom - top));
    mtxa_to_mtx(work->texMtx);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_inv_tf_vec_xyz(0.0f, 0.0f, -1.0f, &target);
    vec_to_euler(&target, &rot);
    mtxa_rigid_inv_tf_tl(&target);
    fn_1_22D64(&target, &rot, 0.005493164f * (s16)math_atan(newProj[3]), newProj[1] / newProj[3], -newProj[2], -newProj[4]);
    GXSetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, width, height);
    prevShadow = fn_1_D57C4(0);
    {
        s8 state = events[EVENT_STAGE].state;

        if (state == EV_RUNNING || state == EV_SUSPENDED)
            g_draw_stage();
    }
    if (events[EVENT_BACKGROUND].state == EV_RUNNING)
    {
        g_smth_with_rendefc_reflective_height(400.0f);
        g_draw_bg();
        g_smth_with_rendefc_reflective_height(0.0f);
    }
    if (events[EVENT_STOBJ].state == EV_RUNNING)
        g_draw_stobjs();
    ord_tbl_draw_nodes();
    fn_1_D57C4(prevShadow);
    GXSetZMode_cached(1, 3, 1);
    GXSetTexCopySrc_cached(0, 0, width, height);
    GXSetTexCopyDst_cached(texWidth, texHeight, 4, 1);
    GXCopyTex(work->viewBuf, 1);
    GXInitTexObj(&work->viewTex, work->viewBuf, texWidth, texHeight, 4, 0, 0, 0);
    work->hasViewTex = 1;
    g_smth_with_projection_mtx(proj);
}

void fn_1_E0B10(struct RenderEffect *rendEfc)
{
    struct RenderEffectWormhole *work = rendEfc->work;

    work->hasRippleTex = 0;
    if (work->activeCount != 0)
    {
        int width;
        int height;
        int texWidth;
        int texHeight;
        int drawn;
        int i;
        struct WormholeParticle *p;
        s8 *used;
        struct GMAModel *model;

        width = 0x80;
        height = 0x80;
        texWidth = width >> 1;
        texHeight = height >> 1;
        GXSetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
        GXSetScissor_cached(0, 0, width, height);
        {
            GXColor color = { 0, 0, 0, 0 };

            g_smth_with_bg_color_drawing(&color);
        }
        g_set_something3(0);
        avdisp_set_z_mode(1, 7, 0);
        drawn = 0;
        used = work->used;
        p = work->particles;
        for (i = 0x40; i > 0; i--, used++, p++)
        {
            if (*used == 0)
                continue;
            mtxa_from_mtxb_translate(&p->pos);
            if (p->flags & 1)
                mtxa_sq_from_identity();
            else
            {
                Vec tl;
                Vec dir;

                mtxa_rotate_y(p->rot.y);
                mtxa_rotate_x(p->rot.x);
                mtxa_rotate_z(p->rot.z);
                mathutil_mtxA_get_translate(&tl);
                mtxa_tf_vec_xyz(0.0f, 0.0f, 1.0f, &dir);
                if (mathutil_vec_dot_prod(&tl, &dir) > 0.0f)
                    mtxa_rotate_y(-0x8000);
            }
            mtxa_scale_s(p->scale);
            model = p->model;
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, p->scale))
            {
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_set_alpha(p->alpha);
                avdisp_draw_model_unculled_sort_never(model);
                drawn++;
            }
        }
        avdisp_set_z_mode(1, 3, 1);
        fn_1_E2E18();
        if (drawn == 0)
            g_smth_with_bg_color_drawing(&g_some_render_flag.bgColor);
        else
        {
            GXSetZMode_cached(1, 3, 1);
            GXSetTexCopySrc_cached(0, 0, width, height);
            GXSetTexCopyDst_cached(texWidth, texHeight, 0x28, 1);
            GXCopyTex(work->rippleBuf, 1);
            GXInitTexObj(&work->rippleTex, work->rippleBuf, texWidth, texHeight, 1, 2, 2, 1);
            work->hasRippleTex = 1;
        }
    }
}

void fn_1_E0DCC(struct RenderEffect *rendEfc)
{
    camera_apply_viewport(g_something_with_camera3);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(g_something_with_camera3);
}

void fn_1_E0E10(struct RenderEffect *rendEfc)
{
    void *prevFunc;
    struct RenderEffectWormhole *work = rendEfc->work;
    int texReady;
    int i;
    int j;
    struct ItemGroup *group;
    struct StageCollHeader *hdr;
    struct StageWormhole *worm;
    struct GMAModel *holeModel;
    struct GMAModel *surfModel;
    struct WormholeSurfaceNode *node;
    struct OrdTblNode *list;
    float alpha;
    Vec sp14;
    Vec sp8;

    if (work->hasViewTex == 0)
        return;
    holeModel = wormhole_gma_model;
    surfModel = worm_surface_gma_model;
    if (holeModel == NULL || surfModel == NULL)
        return;
    texReady = 0;
    prevFunc = fn_1_10A69C(fn_1_E1954);
    group = itemgroups;
    hdr = stagedef->collHeaders;
    for (i = stagedef->itemGroupCount; i > 0; i--, group++, hdr++)
    {
        if (hdr->wormholeCount == 0)
            continue;
        mtxa_from_mtxb();
        mtxa_mult_right(group->transform);
        worm = hdr->wormholes;
        for (j = hdr->wormholeCount; j > 0; j--, worm++)
        {
            mtxa_push();
            mtxa_translate(&worm->pos);
            mtxa_rotate_z(worm->rotZ);
            mtxa_rotate_y(worm->rotY);
            mtxa_rotate_x(worm->rotX);
            if (surfModel != NULL && g_is_sphere_visible(&surfModel->boundSphereCenter, surfModel->boundSphereRadius))
            {
                float size;
                float lod;

                alpha = 0.2f + fn_1_59704(surfModel);
                mtxa_tf_point(&surfModel->boundSphereCenter, &sp8);
                lod = 1.0f;
                if (sp8.z < 0.0f)
                {
                    size = g_current_camera->sub28.view.tanHalfFov;
                    size = -(surfModel->boundSphereRadius / (size * sp8.z));
                    if (size < 0.045f)
                        lod = 0.0f;
                    else if (size < 0.055f)
                        lod = 100.00002f * (size - 0.045f);
                }
                alpha *= lod;
                if (alpha >= 1.0f)
                {
                    if (texReady == 0)
                    {
                        fn_1_E112C(work, 1.0f);
                        texReady = 1;
                    }
                    fn_1_E17BC(work);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_never(surfModel);
                }
                else if (alpha > 0.0f)
                {
                    mtxa_tf_point(&holeModel->boundSphereCenter, &sp14);
                    list = fn_1_CCC18(sp14.z + holeModel->boundSphereRadius);
                    node = ord_tbl_alloc_node(sizeof(*node));
                    if (node != NULL)
                    {
                        node->node.drawFunc = fn_1_E10AC;
                        node->alpha = alpha;
                        node->model = surfModel;
                        node->work = work;
                        mtxa_to_mtx(node->mtx);
                        fn_1_CD418(list, &node->node);
                    }
                }
            }
            mtxa_pop();
        }
    }
    fn_1_10A69C(prevFunc);
}

void fn_1_E10AC(struct OrdTblNode *node_)
{
    void *prevFunc;
    struct WormholeSurfaceNode *node = (struct WormholeSurfaceNode *)node_;

    fn_1_E112C(node->work, node->alpha);
    prevFunc = fn_1_10A69C(fn_1_E1954);
    mtxa_from_mtx(node->mtx);
    fn_1_E17BC(node->work);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(node->model);
    fn_1_10A69C(prevFunc);
}

void fn_1_E112C(struct RenderEffectWormhole *work, float alpha)
{
    if (work->hasRippleTex != 0)
        fn_1_E1160(work, alpha);
    else
        fn_1_E15CC(work, alpha);
}

void fn_1_E1160(struct RenderEffectWormhole *work, float alpha)
{
    int stage;
    int coord;
    int indStage;
    GXColor color;
    float mtx[2][3];

    GXInvalidateTexAll();
    GXLoadTexObj_cached(&work->viewTex, 0);
    GXLoadTexObj_cached(&work->rippleTex, 1);
    GXLoadTexObj_cached(&work->noiseTex, 2);
    GXLoadTexObj_cached(&work->rippleTex, 3);
    mtx[0][0] = 0.041666668f;
    mtx[0][1] = 0.083333336f;
    mtx[0][2] = 0.0f;
    mtx[1][0] = 0.083333336f;
    mtx[1][1] = 0.041666668f;
    mtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(1, mtx, 0);
    mtx[0][0] = -0.4f;
    mtx[0][1] = -0.6f;
    mtx[0][2] = 0.0f;
    mtx[1][0] = 0.6f;
    mtx[1][1] = 0.4f;
    mtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(2, mtx, -1);
    color.r = 0x7F;
    color.g = 0x8F;
    color.b = 0xEF;
    color.a = 255.0f * alpha;
    GXSetTevKColor_cached(0, color);
    stage = 0;
    coord = 0;
    indStage = 0;

    GXSetTevKColorSel_cached(stage, 0xC);
    GXSetTevKAlphaSel_cached(stage, 0x1C);
    GXSetTevSwapMode_cached(stage, 0, 0);
    GXSetTexCoordGen2_cached(coord, 0, 0, 0x1E, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 0xE, 8, 0xF);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTexCoordGen2_cached(coord + 1, 0, 0, 0x21, 0, 0x7D);
    GXSetIndTexOrder_cached(indStage, coord + 1, 1);
    GXSetTevIndirect_cached(stage, indStage, 0, 0, 1, 0, 0, 0, 0, 0);
    stage++;
    indStage++;
    coord += 2;

    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevOrder_wrapper(stage, coord - 1, 3, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 1, 1, 1);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    stage++;

    GXSetTevSwapMode_cached(stage, 0, 0);
    GXSetTexCoordGen2_cached(coord, 0, 0, 0x24, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 2, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 8, 2, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevIndirect_cached(stage, indStage - 1, 0, 0, 2, 0, 0, 0, 0, 0);
    stage++;
    coord++;

    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(indStage);
    if (1.0f == alpha)
        GXSetBlendMode_cached(0, 1, 0, 0);
    else
        GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 3, 1);
    fn_1_E2F1C();
}

void fn_1_E15CC(struct RenderEffectWormhole *work, float alpha)
{
    int stage;
    int coord;
    int indStage;
    GXColor color;

    GXInvalidateTexAll();
    GXLoadTexObj_cached(&work->viewTex, 0);
    color.r = 0x7F;
    color.g = 0x8F;
    color.b = 0xEF;
    color.a = 255.0f * alpha;
    GXSetTevKColor_cached(0, color);
    stage = 0;
    coord = 0;
    indStage = 0;

    GXSetTevKColorSel_cached(stage, 0xC);
    GXSetTevKAlphaSel_cached(stage, 0x1C);
    GXSetTevSwapMode_cached(stage, 0, 0);
    GXSetTexCoordGen2_cached(coord, 0, 0, 0x1E, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 0xE, 8, 0xF);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    stage++;
    coord++;

    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(indStage);
    if (1.0f == alpha)
        GXSetBlendMode_cached(0, 1, 0, 0);
    else
        GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 3, 1);
    fn_1_E2F1C();
}

void fn_1_E17BC(struct RenderEffectWormhole *work)
{
    Mtx mtx;
    Mtx proj;

    mtxa_push();
    mtxa_to_mtx(mtx);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = -0.5f * g_current_camera->sub28.view.invTanHalfFov * (1.0f / g_current_camera->sub28.view.aspect);
    mtxa->mtxA[0][2] = 0.5f + -0.5f * g_current_camera->sub28.view.unk28;
    mtxa->mtxA[1][1] = 0.5f * g_current_camera->sub28.view.invTanHalfFov;
    mtxa->mtxA[1][2] = 0.5f + -0.5f * g_current_camera->sub28.view.unk2C;
    mtxa_to_mtx(proj);
    mtxa_mult_right(mtx);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 0);
    mtxa_mult_left(work->texMtx);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
    mtxa_from_mtx(proj);
    mtxa_scale_xyz(1.5f, 1.5f, 1.0f);
    mtxa_mult_right(mtx);
    GXLoadTexMtxImm(mtxa->mtxA, 0x24, 0);
    mtxa_pop();
}

int fn_1_E1954(void)
{
    return 1;
}

static inline float dot_xyz(register float x1, register float y1, register float z1, register float x2, register float y2, register float z2)
{
#ifdef __MWERKS__
    asm
    {
        fmuls x1, x1, x2
        fmadds x1, y1, y2, x1
        fmadds x1, z1, z2, x1
    }
#else
    x1 = x1 * x2 + y1 * y2 + z1 * z2;
#endif
    return x1;
}

void fn_1_E195C(struct RenderEffectWormhole *work)
{
    int idx;
    struct ItemGroup *group;
    int i;
    int j;
    struct StageCollHeader *hdr;
    int bestGroup;
    struct StageWormhole *dest;
    int best;
    struct StageWormhole *worm;
    int groupCount;
    float bestDist;
    Vec pos;
    Vec fwd;
    Vec up;
    Vec dstPos;
    Vec dstFwd;
    Vec dstUp;
    Vec prevPos;
    Vec camPos;
    Vec camFwd;

    mtxa_from_mtx(g_current_camera->unk14C);
    mtxa_rigid_inv_tf_tl(&camPos);
    mtxa_rigid_inv_tf_vec_xyz(0.0f, 0.0f, -1.0f, &camFwd);
    bestDist = -1.0f;
    best = -1;
    groupCount = stagedef->itemGroupCount;
    idx = 0;
    group = itemgroups;
    hdr = stagedef->collHeaders;
    for (i = groupCount; i > 0; i--, group++, hdr++)
    {
        if (hdr->wormholeCount == 0)
            continue;
        mtxa_from_mtx(group->transform);
        worm = hdr->wormholes;
        for (j = hdr->wormholeCount; j > 0; j--, worm++, idx++)
        {
            mtxa_tf_point(&worm->pos, &prevPos);
            if (!(dot_xyz(camFwd.x, camFwd.y, camFwd.z, prevPos.x - camPos.x, prevPos.y - camPos.y, prevPos.z - camPos.z) < 0.0f))
            {
                float dist = mathutil_vec_distance(&prevPos, &camPos);

                if (bestDist > dist || bestDist < 0.0f)
                {
                    bestDist = dist;
                    best = idx;
                    bestGroup = groupCount - i;
                }
            }
        }
    }
    work->nearestIdx = best;
    if (best >= 0)
    {
        group = &itemgroups[bestGroup];
        worm = &stagedef->wormholes[best];
        mtxa_from_mtx(group->transform);
        mtxa_translate(&worm->pos);
        mtxa_rotate_z(worm->rotZ);
        mtxa_rotate_y(worm->rotY);
        mtxa_rotate_x(worm->rotX);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &fwd);
        mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &up);
        mtxa_tf_point_xyz(0.0f, 2.2f, 0.0f, &pos);
        mtxa_from_mtx(group->prevTransform);
        mtxa_translate(&worm->pos);
        mtxa_rotate_z(worm->rotZ);
        mtxa_rotate_y(worm->rotY);
        mtxa_rotate_x(worm->rotX);
        mtxa_tf_point_xyz(0.0f, 2.2f, 0.0f, &prevPos);
        work->srcPos = pos;
        work->srcVel.x = pos.x - prevPos.x;
        work->srcVel.y = pos.y - prevPos.y;
        work->srcVel.z = pos.z - prevPos.z;
        work->srcFwd = fwd;
        work->srcUp = up;
    }
    if (best >= 0)
    {
        dest = stagedef->wormholes[best].dest;
        best = ((u32)dest - (u32)stagedef->wormholes) / sizeof(*dest);
        group = itemgroups;
        hdr = stagedef->collHeaders;
        for (i = stagedef->itemGroupCount; i > 0; i--, group++, hdr++)
        {
            if (hdr->wormholeCount != 0)
            {
                best -= hdr->wormholeCount;
                if (best < 0)
                {
                    mtxa_from_mtx(group->transform);
                    break;
                }
            }
        }
        mtxa_translate(&dest->pos);
        mtxa_rotate_z(dest->rotZ);
        mtxa_rotate_y(dest->rotY);
        mtxa_rotate_x(dest->rotX);
        mtxa_tf_vec_xyz(0.0f, 0.0f, 1.0f, &dstFwd);
        mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &dstUp);
        mtxa_tf_point_xyz(0.0f, 2.2f, 0.0f, &dstPos);
        work->dstPos = dstPos;
        work->dstFwd = dstFwd;
        work->dstUp = dstUp;
    }
}

int fn_1_E1DC0(struct RenderEffectWormhole *work)
{
    int i;
    int j;
    struct ItemGroup *group;
    struct StageCollHeader *hdr;
    struct WormholeBounds bounds;
    struct GMAModel *model;
    int visible;
    float tanHalfFov;
    float invAspect;
    struct StageWormhole *worm;
    Vec pos;

    visible = 0;
    tanHalfFov = g_current_camera->sub28.view.tanHalfFov;
    invAspect = 1.0f / g_current_camera->sub28.view.aspect;
    model = worm_surface_gma_model;
    bounds.left = 1.0f;
    bounds.top = 1.0f;
    bounds.right = -1.0f;
    bounds.bottom = -1.0f;
    bounds.nearZ = -20000.0f;
    group = itemgroups;
    hdr = stagedef->collHeaders;
    for (i = stagedef->itemGroupCount; i > 0; i--, group++, hdr++)
    {
        if (hdr->wormholeCount == 0)
            continue;
        mtxa_from_mtxb();
        mtxa_mult_right(group->transform);
        worm = hdr->wormholes;
        for (j = hdr->wormholeCount; j > 0; j--, worm++)
        {
            mtxa_push();
            mtxa_translate(&worm->pos);
            mtxa_rotate_z(worm->rotZ);
            mtxa_rotate_y(worm->rotY);
            mtxa_rotate_x(worm->rotX);
            if (model == NULL || !g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
            {
                mtxa_pop();
                continue;
            }
            mtxa_tf_point(&model->boundSphereCenter, &pos);
            mtxa_pop();
            if (pos.z < 0.0f)
            {
                float minX;
                float minY;
                float maxX;
                float maxY;
                float scale = -(1.0f / (tanHalfFov * pos.z));
                float radius = model->boundSphereRadius * scale;

                if (radius < 0.045f)
                    continue;
                pos.x *= scale;
                pos.y *= scale;
                minX = invAspect * (pos.x - radius);
                minY = pos.y - radius;
                maxX = invAspect * (pos.x + radius);
                maxY = pos.y + radius;
                radius = pos.z + radius;
                if (bounds.left > minX)
                    bounds.left = minX;
                if (bounds.top > minY)
                    bounds.top = minY;
                if (bounds.right < maxX)
                    bounds.right = maxX;
                if (bounds.bottom < maxY)
                    bounds.bottom = maxY;
                if (bounds.nearZ < radius)
                    bounds.nearZ = radius;
            }
            visible = 1;
        }
    }
    if (visible != 0)
    {
        bounds.left += g_current_camera->sub28.view.unk28;
        bounds.right += g_current_camera->sub28.view.unk28;
        bounds.top += g_current_camera->sub28.view.unk2C;
        bounds.bottom += g_current_camera->sub28.view.unk2C;
        if (bounds.left < -1.0f)
            bounds.left = -1.0f;
        if (bounds.top < -1.0f)
            bounds.top = -1.0f;
        if (bounds.right > 1.0f)
            bounds.right = 1.0f;
        if (bounds.bottom > 1.0f)
            bounds.bottom = 1.0f;
        if (bounds.nearZ > -0.1f)
            bounds.nearZ = -0.1f;
        work->bounds = bounds;
        if (bounds.left >= bounds.right || bounds.top >= bounds.bottom)
            visible = 0;
    }
    return visible;
}

void fn_1_E2164(struct RenderEffect *rendEfc)
{
    struct RenderEffectWormhole *work = rendEfc->work;
    struct Camera *camera = g_current_camera;

    if (work->flashTimer <= 0)
    {
        work->hasFlashTex = 0;
        return;
    }
    if (work->hasFlashTex != 0 && work->flashTimer > 0)
        fn_1_E23B4(work);
    if (work->flashTimer > 0 || (lbl_1_bss_CF220[0] & 1))
    {
        int x = camera->sub28.viewportX * current_render_mode->fbWidth;
        int y = camera->sub28.viewportY * current_render_mode->efbHeight;
        int w = ((x & 1) + (int)(camera->sub28.viewportW * current_render_mode->fbWidth)) & ~1;
        int h = ((y & 1) + (int)(camera->sub28.viewportH * current_render_mode->efbHeight)) & ~1;
        int texWidth;
        int texHeight;

        x &= ~1;
        y &= ~1;
        texWidth = w >> 1;
        texHeight = h >> 1;
        if (texWidth > 0x140)
            texWidth = 0x140;
        if (texHeight > 0xE0)
            texHeight = 0xE0;
        GXSetTexCopySrc_cached(x, y, w, h);
        GXSetTexCopyDst_cached(texWidth, texHeight, 4, 1);
        GXCopyTex(work->flashBuf, 0);
        GXInitTexObj(&work->flashTex, work->flashBuf, texWidth, texHeight, 4, 0, 0, 0);
        work->hasFlashTex = 1;
    }
}

void fn_1_E23B4(struct RenderEffectWormhole *work)
{
    int stage;
    int coord;
    int indStage;
    float x1;
    float y1;
    float x0;
    float y0;
    float z;
    float scale;
    float offset;
    GXColor color;
    float proj[7];

    g_something_with_GX_vtx_desc((1 << 9) | (1 << 13) | (1 << 14));
    GXSetCullMode_cached(0);
    GXLoadTexObj_cached(&work->flashTex, 0);
    GXLoadTexObj_cached(&work->flashNoiseTex, 1);
    color.r = 0xDF;
    color.g = 0xE1;
    color.b = 0xEF;
    if (work->flashTimer > 0xFF)
        color.a = 0xFF;
    else
        color.a = work->flashTimer;
    GXSetTevKColor_cached(0, color);
    stage = 0;
    coord = 0;
    indStage = 0;

    GXSetTevKColorSel_cached(stage, 0xC);
    GXSetTevKAlphaSel_cached(stage, 0x1C);
    GXSetTevSwapMode_cached(stage, 0, 0);
    GXSetTexCoordGen2_cached(coord, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 0xE, 8, 0xF);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    stage++;
    coord++;

    GXSetTevSwapMode_cached(stage, 0, 1);
    GXSetTexCoordGen2_cached(coord, 1, 5, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 1, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 0, 7, 4, 7);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    stage++;
    coord++;

    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(indStage);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 7, 0);
    g_something_with_raster_state(0);
    GXGetProjectionv(proj);
    z = -0.5f * (proj[6] / proj[5]);
    switch (*(int *)&proj[0])
    {
    case 0:
        scale = z / proj[1];
        x1 = (1.0f - proj[2]) * scale;
        x0 = (-1.0f - proj[2]) * scale;
        scale = z / proj[3];
        y1 = (1.0f - proj[4]) * scale;
        y0 = (-1.0f - proj[4]) * scale;
        break;
    default:
        scale = 1.0f / proj[1];
        x1 = (1.0f - proj[2]) * scale;
        x0 = (-1.0f - proj[2]) * scale;
        scale = 1.0f / proj[3];
        y1 = (1.0f - proj[4]) * scale;
        y0 = (-1.0f - proj[4]) * scale;
        break;
    }
    offset = 0.5f * ((float)work->flashTimer / 255.0f);
    scale = work->flashScale;
    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();
    GXBegin(0x80, 0, 4);
        GXPosition3f32(x0, y1, z);
        GXTexCoord2f32(1.0f - scale, 1.0f - scale);
        GXTexCoord2f32(1.0f + offset, 1.0f + offset);
        GXPosition3f32(x1, y1, z);
        GXTexCoord2f32(scale, 1.0f - scale);
        GXTexCoord2f32(-offset, 1.0f + offset);
        GXPosition3f32(x1, y0, z);
        GXTexCoord2f32(scale, scale);
        GXTexCoord2f32(-offset, -offset);
        GXPosition3f32(x0, y0, z);
        GXTexCoord2f32(1.0f - scale, scale);
        GXTexCoord2f32(1.0f + offset, -offset);
    GXEnd();
}
