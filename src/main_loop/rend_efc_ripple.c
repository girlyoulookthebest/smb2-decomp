/* Sea-surface reflection for the pilot background: renders the stage mirrored
 * across the sea plane, then draws an indirect-offset map of scrolling water
 * layers and ball ripples for the background's sea shader to sample. */

#pragma peephole off

#include "types.h"
#include "gx.h"
#include "ball.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "item.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"

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

struct Ripple
{
    /*0x00*/ int life;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec vel;
    /*0x1C*/ struct GMAModel *model;
    /*0x20*/ float scale;
    /*0x24*/ float scaleVel;
    /*0x28*/ s16 rot;
    /*0x2A*/ s16 rotVel;
    /*0x2C*/ float alpha;
};  /* size = 0x30 */

struct RenderEffectSeaReflection
{
    /*0x0000*/ u32 active;
    /*0x0004*/ GXTexObj tex;
    /*0x0024*/ int format;
    /*0x0028*/ void *image;
    /*0x002C*/ u16 xres;
    /*0x002E*/ u16 yres;
    /*0x0030*/ GXTexObj indTex;
    /*0x0050*/ void *indBuf;
    /*0x0054*/ Mtx mtx;
    /*0x0084*/ u8 filler84[4];
    /*0x0088*/ u8 unk88;
    /*0x008C*/ float timer;
    /*0x0090*/ int next;
    /*0x0094*/ struct Ripple ripples[256];
};  /* size = 0x3094 */

/* world_theme.work while a pilot background is loaded; see bg_pil.c */
struct BgPilWork
{
    /* pointer-bearing regions are typed (not fillers) so the x64 layout
     * matches bg_pil.c / bg_pil2.c */
    /*0x000*/ s32 initialized;
    /*0x004*/ struct GMAModel *unk4[4];
    /*0x014*/ int animatedField;
    /*0x018*/ struct StageBgModel *sea;
    /*0x01C*/ struct GMAModel *seaModel;
    /*0x020*/ GXTexObj *waterSpecTex;
    /*0x024*/ GXTexObj *indMapATex;
    /*0x028*/ s32 oceanTexCount;
    /*0x02C*/ GXTexObj *oceanTex[32];
    /*0x0AC*/ int caustFrameCount;
    /*0x0B0*/ GXTexObj *caustFrames[0x20];
    /*0x130*/ struct GMAModel *seaMask;
    /*0x134*/ struct GMAModel *vtxSeaBall;
    /*0x138*/ int hamonCount;
    /*0x13C*/ struct GMAModel *hamon[8];
    u8 filler15C[0x1FC - 0x15C];
    /*0x1FC*/ struct RenderEffectSeaReflection *reflection;
    u8 filler200[0x28];
    /*0x228*/ Mtx layer0Mtx;
    u8 filler258[0x28];
    /*0x280*/ Mtx layer1Mtx;
    u8 filler2B0[0x28];
    /*0x2D8*/ Mtx layer2Mtx;
};

struct AvdispTevState
{
    /*0x00*/ int stage;
    /*0x04*/ int coord;
    /*0x08*/ int texMtx;
    /*0x0C*/ int texMap;
    /*0x10*/ int indStage;
    u8 filler14[8];
    /*0x1C*/ int indMtx;
    u8 filler20[0xC];
};  /* size = 0x2C */

struct AvdispTevContext
{
    /* BallEnvArg (avdisp.h) layout: tev follows an int and two pointers */
    /*0x00*/ s32 unk0;
    /*0x04*/ void *shape;
    /*0x08*/ void *layers;
    /*0x0C*/ struct AvdispTevState tev;
};

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
} g_some_render_flag;

extern s32 stage_heap;
extern void *lbl_801988E0;
extern void (*lbl_1_bss_D49B8)(void);

void *OSAllocFromHeap(int heap, u32 size);
void OSFreeToHeap(int heap, void *ptr);
void *memset(void *dest, int c, u32 n);

void gp_wait_wrapper(void);
u32 get_max_gx_fifo_use_size(void);
int fn_1_15F8B0(void);
void fn_1_E2D20(void);
void g_set_something3(int arg0);
void g_draw_stage(void);
void g_draw_bg(void);
void g_draw_stobjs(void);
void g_draw_ball_and_ape(void);
void g_draw_func_init(void);
void g_smth_with_rendefc_reflective_height(float height);
void draw_reflective_objects_in_viewstage(void);
void ord_tbl_draw_nodes(void);
void *g_yet_another_unk_draw_func(void *func);
void GXLoadTexMtxImm(Mtx m, u32 id, int type);

u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp, u8 doEdgeLod, int maxAniso);
void GXLoadPosMtxImm(Mtx m, u32 id);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTexCoordGen2_cached(int dst, int func, int src, u32 mtx, int normalize, int postMtx);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXCopyTex(void *dest, u8 clear);
void GXSetViewport(float left, float top, float wd, float ht, float nearz, float farz);
static inline void GXSetTexCoordGen_cached(int dst, int func, int src, u32 mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}

void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);

void g_smth_with_reflective_models(int arg0, struct RenderEffect *rendEfc);
void fn_1_DE6EC(int arg0, struct RenderEffect *rendEfc);
void fn_1_DE81C(struct RenderEffect *rendEfc);
void fn_1_DE8D8(struct AvdispTevContext *ctx);
void fn_1_DEC88(struct AvdispTevContext *ctx);
void fn_1_DF174(struct RenderEffect *rendEfc);

void g_smth_with_pil2_ref(struct RenderEffect *rendEfc)
{
    struct RenderEffectSeaReflection *work;
    u32 bufSize;

    rendEfc->enableFlags = 3;
    work = OSAllocFromHeap(stage_heap, sizeof(*work));
    if (work == NULL)
    {
        rendEfc->state = 0;
        return;
    }
    memset(work, 0, sizeof(*work));
    rendEfc->work = work;
    work->format = 4;
    work->xres = 640;
    work->yres = 448;
    work->image = lbl_801988E0;
    bufSize = GXGetTexBufferSize(0x100, 0x100, 3, 0, 0);
    work->indBuf = OSAllocFromHeap(stage_heap, bufSize);
    if (work->indBuf == NULL)
    {
        OSFreeToHeap(stage_heap, work);
        rendEfc->state = 0;
        return;
    }
    mtxa_from_identity();
    mtxa_to_mtx(work->mtx);
    work->unk88 = 0x60;
    ((struct BgPilWork *)world_theme.work)->reflection = work;
}

void g_something_with_stage_heap_and_target_theme(struct RenderEffect *rendEfc)
{
    if (rendEfc->work != NULL)
    {
        int needWait = 0;

        if (((struct RenderEffectSeaReflection *)rendEfc->work)->indBuf != NULL)
        {
            OSFreeToHeap(stage_heap, ((struct RenderEffectSeaReflection *)rendEfc->work)->indBuf);
            needWait = 1;
        }
        if (needWait)
            gp_wait_wrapper();
        OSFreeToHeap(stage_heap, rendEfc->work);
    }
    switch (world_theme.bgId)
    {
    case 0xB:
    case 0x1D:
        ((struct BgPilWork *)world_theme.work)->reflection = NULL;
    }
}

void fn_1_DD838(struct RenderEffect *rendEfc)
{
    struct RenderEffectSeaReflection *work;
    struct Ball *ball;
    struct Ripple *part;
    struct BgPilWork *bg;
    float speed;
    Vec pos;
    Vec vel;

    if (!(g_some_other_flags & 0xA))
    {
        bg = world_theme.work;
        work = rendEfc->work;
        ball = &balls[modeCtrl.currPlayer];
        speed = mathutil_vec_len(&ball->vel);
        pos = ball->pos;
        vel = ball->vel;
        speed = mathutil_vec_len(&vel);
        {
            float height = __fabs(pos.y);

            pos.x += 3.0f * (vel.x * height);
            pos.z += 3.0f * (vel.z * height);
            speed *= 4.0f / (1.0f + height);
        }
        speed -= 0.1f;
        if (speed > 0.0f)
        {
            work->timer += speed;
            if (work->timer > 1.0f)
            {
                work->timer = 0.0f;
                if (speed > 0.5f)
                    speed = 0.5f;
                part = &work->ripples[work->next];
                work->next--;
                if (work->next < 0)
                    work->next = 0xFF;
                part->life = 60.0f * (0.5f + 2.0f * (speed * RAND_FLOAT()));
                part->pos.x = pos.x;
                part->pos.y = 0.0f;
                part->pos.z = pos.z;
                part->vel.x = 0.5f * vel.x;
                part->vel.y = 0.0f;
                part->vel.z = 0.5f * vel.z;
                part->model = bg->hamon[(rand() & 0x7FFF) % bg->hamonCount];
                part->scale = 1.0f / part->model->boundSphereRadius;
                part->scaleVel = part->scale * (0.55f * (speed * (1.0f + RAND_FLOAT())));
                part->rot = rand() & 0x7FFF;
                part->rotVel = 16384.0f * (RAND_FLOAT() - 0.5f);
                part->alpha = 0.2f;
            }
        }
        {
            int i;

            part = work->ripples;
            for (i = 256; i > 0; i--, part++)
            {
                if (part->life != 0)
                {
                    part->vel.x *= 0.92f;
                    part->vel.y *= 0.92f;
                    part->vel.z *= 0.92f;
                    part->pos.x += part->vel.x;
                    part->pos.y += part->vel.y;
                    part->pos.z += part->vel.z;
                    part->rotVel = part->rotVel - (part->rotVel >> 7);
                    part->rot += part->rotVel;
                    part->scaleVel *= 0.99f;
                    part->scale += part->scaleVel;
                    part->life--;
                    if (part->life < 60.0f)
                        part->alpha *= (float)part->life / (float)(part->life + 1);
                    else
                        part->alpha += 0.3f * (1.0f - part->alpha);
                }
            }
        }
    }
}

void g_some_rendefc_func_2(int arg0, struct RenderEffect *rendEfc)
{
    switch (arg0)
    {
    case 1:
        g_smth_with_reflective_models(arg0, rendEfc);
        fn_1_DE6EC(arg0, rendEfc);
        {
            struct RenderEffectSeaReflection *work = rendEfc->work;

            work->active = 1;
        }
    case 2:
        break;
    }
}

void fn_1_DDD7C(int arg0, struct RenderEffect *rendEfc)
{
    int skip = fn_1_15F8B0() == 0;

    if ((dip_switches & 1) && get_max_gx_fifo_use_size() > 0xD9999)
        skip = 1;
    switch (arg0)
    {
    case 1:
        if (skip)
        {
            struct RenderEffectSeaReflection *work = rendEfc->work;

            work->active = 0;
            return;
        }
    }
    switch (arg0)
    {
    case 1:
        g_smth_with_reflective_models(arg0, rendEfc);
        fn_1_DE6EC(arg0, rendEfc);
        {
            struct RenderEffectSeaReflection *work = rendEfc->work;

            work->active = 1;
        }
    case 2:
        break;
    }
}

void g_smth_with_reflective_models(int arg0, struct RenderEffect *rendEfc)
{
    struct Camera cameraBackup;
    struct RenderEffectSeaReflection *work;
    struct Camera *camera;
    struct StageBgModel *sea;
    Vec eyeLocal;

    camera_apply_viewport(modeCtrl.currPlayer);
    work = rendEfc->work;
    camera = g_current_camera;
    cameraBackup = *camera;
    push_light_group();
    sea = ((struct BgPilWork *)world_theme.work)->sea;
    mtxa_from_translate(&sea->pos);
    mtxa_rotate_z(sea->rotZ);
    mtxa_rotate_y(sea->rotY);
    mtxa_rotate_x(sea->rotX);
    mtxa_to_mtx(work->mtx);
    mtxa_mult_left(mtxa->mtxB);
    mtxa_rigid_inv_tf_tl(&eyeLocal);
    if (eyeLocal.y < 0.0f)
    {
        mtxa_from_mtx(work->mtx);
        mtxa_rotate_z(-0x8000);
        mtxa_to_mtx(work->mtx);
    }

    {
        Mtx spF0;
        Mtx spC0;
        Mtx sp90;
        Mtx44 projMtx;
        Vec basisI = {1.0f, 0.0f, 0.0f};
        Vec basisJ = {0.0f, 1.0f, 0.0f};
        Vec basisK = {0.0f, 0.0f, 1.0f};
        Vec origin = {0.0f, 0.0f, 0.0f};

        mtxa_from_translate(&camera->eye);
        mtxa_rotate_y(camera->rotY);
        mtxa_rotate_x(camera->rotX);
        mtxa_rotate_z(camera->rotZ);
        mtxa_mult_right(mtxa->mtxB);
        mtxa_to_mtx(sp90);
        mtxa_mult_right(work->mtx);
        mtxa_rigid_inv_tf_vec(&basisI, &basisI);
        mtxa_rigid_inv_tf_vec(&basisJ, &basisJ);
        mtxa_rigid_inv_tf_vec(&basisK, &basisK);
        mtxa_rigid_inv_tf_point(&origin, &origin);
        basisI.y = -basisI.y;
        basisJ.y = -basisJ.y;
        basisK.y = -basisK.y;
        origin.y = -origin.y;
        mtxa_tf_vec(&basisI, &basisI);
        mtxa_tf_vec(&basisJ, &basisJ);
        mtxa_tf_vec(&basisK, &basisK);
        mtxa_tf_point(&origin, &origin);
        mtxa->mtxA[0][0] = basisI.x;
        mtxa->mtxA[1][0] = basisI.y;
        mtxa->mtxA[2][0] = basisI.z;
        mtxa->mtxA[0][1] = basisJ.x;
        mtxa->mtxA[1][1] = basisJ.y;
        mtxa->mtxA[2][1] = basisJ.z;
        mtxa->mtxA[0][2] = basisK.x;
        mtxa->mtxA[1][2] = basisK.y;
        mtxa->mtxA[2][2] = basisK.z;
        mtxa->mtxA[0][3] = origin.x;
        mtxa->mtxA[1][3] = origin.y;
        mtxa->mtxA[2][3] = origin.z;
        mtxa_invert();
        mtxa_to_mtx(spC0);
        mtxa_from_identity();
        mtxa_scale_xyz(-1.0f, 1.0f, 1.0f);
        mtxa_to_mtx(spF0);
        mtxa_from_identity();
        mtxa_rotate_z(-camera->rotZ);
        mtxa_rotate_x(-camera->rotX);
        mtxa_rotate_y(-camera->rotY);
        mtxa_translate_neg(&camera->eye);
        mtxa_mult_right(spC0);
        mtxa_mult_right(sp90);
        mtxa_mult_left(spF0);
        mtxa_to_mtx(camera->unk14C);
        mtxa_to_mtx(camera->unk17C);
        mtxa_from_identity();
        mtxa_rotate_z(-camera->rotZ);
        mtxa_rotate_x(-camera->rotX);
        mtxa_rotate_y(-camera->rotY);
        mtxa_translate_neg(&camera->eye);
        mtxa_mult_right(spC0);
        mtxa_translate(&camera->eye);
        mtxa_rotate_y(camera->rotY);
        mtxa_rotate_x(camera->rotX);
        mtxa_rotate_z(camera->rotZ);
        mtxa_mult_right(g_ord_tbl_stuff->matrices[0]);
        mtxa_mult_left(spF0);
        mtxa_to_mtx(camera->unk1AC);
        mtxa_to_mtx(camera->unk1DC);
        camera_apply_viewport(g_something_with_camera3);
        C_MTXPerspective(projMtx, 0.005493164f * camera->sub28.view.fov, camera->sub28.view.aspect, 0.1f, 20000.0f);
        projMtx[0][2] -= projMtx[0][0] * camera->sub28.view.unk28 * camera->sub28.view.aspect * camera->sub28.view.tanHalfFov;
        projMtx[1][2] -= projMtx[1][1] * camera->sub28.view.unk2C * camera->sub28.view.tanHalfFov;
        g_something_with_GXSetProjectionv(projMtx, 0);
        GXSetViewport(0.0f, 0.0f, work->xres, work->yres, 0.0f, 1.0f);
        GXSetScissor_cached(0, 0, work->xres, work->yres);
        mtxa_from_mtxb();
        g_set_active_light_group(5);
        g_some_render_flag.flags |= 4;
        mtxa_from_mtx(work->mtx);
        mathutil_mtxA_get_translate(&g_some_render_flag.unk4);
        mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &g_some_render_flag.unk10);
        mtxa_from_mtxb();
        mtxa_tf_point(&g_some_render_flag.unk4, &g_some_render_flag.unk20);
        mtxa_tf_vec(&g_some_render_flag.unk10, &g_some_render_flag.unk2C);
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_rigid_invert();
        mtxa_mult_right(mtxa->mtxB);
        mtxa_tf_point(&g_some_render_flag.unk4, &g_some_render_flag.unk3C);
        mtxa_tf_vec(&g_some_render_flag.unk10, &g_some_render_flag.unk48);
    }

    if (events[EVENT_VIEW].state != EV_RUNNING)
    {
        int above;

        mtxa_from_mtx(work->mtx);
        {
            Vec ballLocal;
            int flag;

            mtxa_rigid_inv_tf_point(&current_ball->pos, &ballLocal);
            flag = 0;
            if (ballLocal.y > 0.0 && modeCtrl.gameType == 5)
                flag = 1;
            above = flag;
        }
        g_draw_func_init();
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
        if (events[EVENT_ITEM].state == EV_RUNNING)
            draw_items();
        if (events[EVENT_STOBJ].state == EV_RUNNING)
            g_draw_stobjs();
        if (events[EVENT_EFFECT].state == EV_RUNNING)
            g_draw_effects();
        if (events[EVENT_BALL].state == EV_RUNNING)
            g_draw_ball_and_ape();
        if (above && lbl_1_bss_D49B8 != NULL)
            lbl_1_bss_D49B8();
        ord_tbl_draw_nodes();
    }
    else
        draw_reflective_objects_in_viewstage();
    g_some_render_flag.flags &= ~4;
    GXSetZMode_cached(1, 3, 1);
    GXSetTexCopySrc_cached(0, 0, work->xres, work->yres);
    GXSetTexCopyDst_cached(work->xres, work->yres, work->format, 0);
    GXCopyTex(work->image, 1);
    GXInitTexObj(&work->tex, work->image, work->xres, work->yres, work->format, 0, 0, 0);
    *camera = cameraBackup;
    camera_apply_viewport(modeCtrl.currPlayer);
    pop_light_group();
}

void fn_1_DE6EC(int arg0, struct RenderEffect *rendEfc)
{
    struct RenderEffectSeaReflection *work = rendEfc->work;

    g_set_something3(0);
    GXSetViewport(0.0f, 0.0f, 256.0f, 256.0f, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, 0x100, 0x100);
    fn_1_DE81C(rendEfc);
    fn_1_DF174(rendEfc);
    GXSetZMode_cached(1, 3, 1);
    GXSetTexCopySrc_cached(0, 0, 0x100, 0x100);
    GXSetTexCopyDst_cached(0x100, 0x100, 0x2C, 0);
    GXCopyTex(work->indBuf, 1);
    GXInitTexObj(&work->indTex, work->indBuf, 0x100, 0x100, 3, 2, 2, 0);
    GXInitTexObjLOD(&work->indTex, 1, 1, 0.0f, 10.0f, 0.0f, 0, 0, 0);
    fn_1_E2D20();
    camera_apply_viewport(modeCtrl.currPlayer);
}

void fn_1_DE81C(struct RenderEffect *rendEfc)
{
    struct RenderEffectSeaReflection *work = rendEfc->work;
    struct BgPilWork *bg = world_theme.work;
    void *prev;

    mtxa_from_mtxb();
    mtxa_mult_right(work->mtx);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    if (bg->caustFrameCount == 0)
        prev = g_yet_another_unk_draw_func(fn_1_DE8D8);
    else if (bg->animatedField != 0)
        prev = g_yet_another_unk_draw_func(fn_1_DEC88);
    else
        prev = g_yet_another_unk_draw_func(fn_1_DE8D8);
    avdisp_draw_model_culled_sort_never(bg->seaMask);
    g_yet_another_unk_draw_func(prev);
}

#define WATER_LAYER_TEV(map) \
    GXSetTevSwapMode_cached(tev.stage, 0, 0); \
    g_GXSetTevOrder_wrapper(tev.stage, tev.coord, map, 0xFF); \
    GXSetTevColorIn_cached(tev.stage, 0xF, 8, 0, 0xF); \
    GXSetTevColorOp_cached(tev.stage, 0, 0, 0, 1, 0); \
    GXSetTevAlphaIn_cached(tev.stage, 7, 7, 7, 0); \
    GXSetTevAlphaOp_cached(tev.stage, 0, 0, 0, 1, 0); \
    g_GXSetTevIndirect_zero_if_different(tev.stage); \
    tev.stage++; \
    tev.coord++; \
    tev.texMtx += 3;

void fn_1_DE8D8(struct AvdispTevContext *ctx)
{
    struct BgPilWork *bg = world_theme.work;
    struct AvdispTevState tev = ctx->tev;

    GXLoadTexObj_cached(bg->indMapATex, tev.texMap);
    GXLoadTexMtxImm(bg->layer0Mtx, tev.texMtx, 1);
    GXSetTexCoordGen_cached(tev.coord, 1, 0, tev.texMtx);
    WATER_LAYER_TEV(tev.texMap)
    GXLoadTexMtxImm(bg->layer1Mtx, tev.texMtx, 1);
    GXSetTexCoordGen_cached(tev.coord, 1, 0, tev.texMtx);
    WATER_LAYER_TEV(tev.texMap)
    GXLoadTexMtxImm(bg->layer2Mtx, tev.texMtx, 1);
    GXSetTexCoordGen_cached(tev.coord, 1, 0, tev.texMtx);
    WATER_LAYER_TEV(tev.texMap)
    tev.texMap++;
    ctx->tev = tev;
}

void fn_1_DEC88(struct AvdispTevContext *ctx)
{
    struct BgPilWork *bg = world_theme.work;
    GXTexObj *tex;
    struct AvdispTevState tev = ctx->tev;
    Vec trans;
    u32 count = bg->caustFrameCount;

    tex = bg->caustFrames[world_theme.unkA4 % count];
    GXLoadTexObj_cached(bg->indMapATex, tev.texMap);
    GXLoadTexObj_cached(tex, tev.texMap + 1);
    GXLoadTexMtxImm(bg->layer0Mtx, tev.texMtx, 1);
    GXSetTexCoordGen_cached(tev.coord, 1, 0, tev.texMtx);
    WATER_LAYER_TEV(tev.texMap)
    mtxa_push();
    mtxa_from_mtx(bg->layer1Mtx);
    mathutil_mtxA_get_translate(&trans);
    mtxa_scale_s(1.5f);
    mathutil_mtxA_set_translate_xyz(0.25f * trans.x, 0.25f * trans.y, 0.25f * trans.z);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtx, 1);
    mtxa_pop();
    GXSetTexCoordGen_cached(tev.coord, 1, 0, tev.texMtx);
    WATER_LAYER_TEV(tev.texMap + 1)
    mtxa_push();
    mtxa_from_mtx(bg->layer1Mtx);
    mathutil_mtxA_get_translate(&trans);
    mtxa_scale_s(2.0f);
    mathutil_mtxA_set_translate(&trans);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtx, 1);
    mtxa_pop();
    GXSetTexCoordGen_cached(tev.coord, 1, 0, tev.texMtx);
    WATER_LAYER_TEV(tev.texMap + 1)
    tev.texMap += 2;
    ctx->tev = tev;
}

void fn_1_DF174(struct RenderEffect *rendEfc)
{
    static const float s_alphaScale[] = {0.8f, 0.0f, 1.75f, 0.0f};
    struct RenderEffectSeaReflection *work = rendEfc->work;
    int i;
    struct Ripple *part;

    mtxa_from_mtxb();
    mtxa_mult_right(work->mtx);
    avdisp_set_z_mode(1, 7, 0);
    part = work->ripples;
    for (i = 256; i > 0; i--, part++)
    {
        if (part->life != 0)
        {
            float alpha;

            mtxa_push();
            mtxa_translate(&part->pos);
            mtxa_rotate_x(-0x4000);
            mtxa_rotate_z(part->rot);
            mtxa_scale_s(part->scale);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            avdisp_set_scale_factor(part->scale);
            alpha = s_alphaScale[0] * part->alpha;
            set_post_mult_color(alpha, alpha, alpha, alpha);
            avdisp_draw_model_culled_sort_never(part->model);
            mtxa_pop();
        }
    }
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
}

#ifndef __MWERKS__
/* PC: the pil/pil2 sea reflection RenderEffectFuncs tables (main_loop .data,
 * not decompiled; bg_pil.c / bg_pil2.c pass them to fn_1_D8130). They used
 * to be u32 word tables in pc_port data_init_gen.c, which on x64 does not
 * match rend_efc.c's struct of four function pointers. Same layout as
 * rend_efc.c's struct RenderEffectFuncs. */
struct PcRenderEffectFuncs
{
    void (*init)(struct RenderEffect *);
    void (*destroy)(struct RenderEffect *);
    void (*main)(struct RenderEffect *);
    void (*draw)(int, struct RenderEffect *);
};

struct PcRenderEffectFuncs lbl_1_data_533F0 = {
    g_smth_with_pil2_ref, g_something_with_stage_heap_and_target_theme, fn_1_DD838, g_some_rendefc_func_2,
};
struct PcRenderEffectFuncs lbl_1_data_53400 = {
    g_smth_with_pil2_ref, g_something_with_stage_heap_and_target_theme, fn_1_DD838, fn_1_DDD7C,
};
#endif
