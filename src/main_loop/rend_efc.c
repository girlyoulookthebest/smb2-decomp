/* Full-screen render effects: an event owning four effect slots, plus the
 * blur, motion blur, focus and kaleidoscope effects (SMB1 rend_efc.c). */
#pragma peephole off
#include "types.h"
#include "gx.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "input.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "stage.h"

struct RenderEffect
{
    /*0x00*/ s8 state;
    /*0x01*/ u8 filler1[5];
    /*0x06*/ u16 cameraMask;  /* bit per camera the effect is enabled for */
    /*0x08*/ u32 enableFlags;
    /*0x0C*/ u8 fillerC[4];
    /*0x10*/ void *work;  /* effect-specific data */
    /*0x14*/ struct RenderEffectFuncs *funcs;
};  /* size = 0x18 */

struct RenderEffectFuncs
{
    void (*init)(struct RenderEffect *);
    void (*destroy)(struct RenderEffect *);
    void (*main)(struct RenderEffect *);
    void (*draw)(int, struct RenderEffect *);
};

struct RenderEffectBlur
{
    /*0x00*/ GXTexObj texObj;
    /*0x20*/ void *imageBuf;
};  /* size = 0x24 */

struct RenderEffectFocusCam
{
    /*0x00*/ s32 fov;
    /*0x04*/ float distance;
    /*0x08*/ float velocity;
    /*0x0C*/ float amount;
    /*0x10*/ s32 disabled;
    /*0x14*/ s32 waiting;
};  /* size = 0x18 */

struct RenderEffectFocus
{
    /*0x00*/ GXTexObj texObj;
    /*0x20*/ void *imageBuf;
    /*0x24*/ struct RenderEffectFocusCam cams[5];
};  /* size = 0x9C */

struct RenderEffectKaleidoscope
{
    /*0x00*/ s32 mode;
    /*0x04*/ GXTexObj colorTex;
    /*0x24*/ GXTexObj alphaTex;
    /*0x44*/ void *colorBuf;
    /*0x48*/ void *alphaBuf;
};  /* size = 0x4C */

struct PerfInfo
{
    u8 filler0[0x1C];
    /*0x1C*/ u32 rendEfcTime;
};

extern u32 monkey_flags;
extern struct PerfInfo perfInfo;
extern void *currentHeap;
extern void *lbl_801988E0;

void *OSAllocFromHeap(void *heap, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
void perf_init_timer(int timer);
u32 perf_stop_timer(int timer);
float get_gpu_frame_time(void);
BOOL is_stage_id_not_for_party_game(int stageId);
void fn_1_E2F1C(void);

u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXGetProjectionv(float *proj);
void GXLoadPosMtxImm(Mtx m, u32 id);
void GXLoadTexMtxImm(Mtx m, u32 id, int type);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetCullMode_cached(int mode);
void GXSetNumChans_cached(u8 n);
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
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXCopyTex(void *dest, u8 clear);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_something_with_GX_vtx_desc(u32 attrs);

void fn_1_D82E4(struct RenderEffect *rendEfc);
void fn_1_D8378(struct RenderEffect *rendEfc);
void fn_1_D83B8(struct RenderEffect *rendEfc);
void fn_1_D83BC(int arg0, struct RenderEffect *rendEfc);
void fn_1_D8B4C(struct RenderEffect *rendEfc);
void fn_1_D8C3C(struct RenderEffect *rendEfc);
void fn_1_D8CAC(struct RenderEffect *rendEfc);
void fn_1_D8CB0(int arg0, struct RenderEffect *rendEfc);
void fn_1_D90F4(struct RenderEffect *rendEfc);
void fn_1_D920C(struct RenderEffect *rendEfc);
void fn_1_D924C(struct RenderEffect *rendEfc);
void fn_1_D9400(int arg0, struct RenderEffect *rendEfc);
void fn_1_D9BE8(struct RenderEffect *rendEfc);
void fn_1_D9D2C(struct RenderEffect *rendEfc);
void fn_1_D9DC4(struct RenderEffect *rendEfc);
void fn_1_D9E00(int arg0, struct RenderEffect *rendEfc);
void g_smth_with_loading_reflective_stgobjs(struct RenderEffect *rendEfc);
void fn_1_DAF6C(struct RenderEffect *rendEfc);
void fn_1_DAFDC(struct RenderEffect *rendEfc);
void g_smth_calling_reflective_obj_draw_hdlr(int arg0, struct RenderEffect *rendEfc);
void g_some_rendefc_func_1(struct RenderEffect *rendEfc);
void fn_1_DC10C(struct RenderEffect *rendEfc);
void fn_1_DC1A4(struct RenderEffect *rendEfc);
void fn_1_DC1A8(int arg0, struct RenderEffect *rendEfc);
void g_smth_with_wormhole_surfaces(struct RenderEffect *rendEfc);
void fn_1_DF540(struct RenderEffect *rendEfc);
void fn_1_DF5FC(struct RenderEffect *rendEfc);
void fn_1_E02F8(int arg0, struct RenderEffect *rendEfc);

struct RenderEffect g_haze_type[4];

struct RenderEffectFuncs lbl_1_data_53330[] =
{
    { NULL, NULL, NULL, NULL },
    { fn_1_D82E4, fn_1_D8378, fn_1_D83B8, fn_1_D83BC },
    { fn_1_D8B4C, fn_1_D8C3C, fn_1_D8CAC, fn_1_D8CB0 },
    { fn_1_D90F4, fn_1_D920C, fn_1_D924C, fn_1_D9400 },
    { fn_1_D9BE8, fn_1_D9D2C, fn_1_D9DC4, fn_1_D9E00 },
    { g_smth_with_loading_reflective_stgobjs, fn_1_DAF6C, fn_1_DAFDC, g_smth_calling_reflective_obj_draw_hdlr },
    { g_some_rendefc_func_1, fn_1_DC10C, fn_1_DC1A4, fn_1_DC1A8 },
    { g_smth_with_wormhole_surfaces, fn_1_DF540, fn_1_DF5FC, fn_1_E02F8 },
};

void event_rend_efc_init(void)
{
    memset(g_haze_type, 0, sizeof(g_haze_type));
}

void event_rend_efc_tick(void)
{
    int i;
    struct RenderEffect *rendEfc = g_haze_type;

    for (i = 4; i > 0; i--, rendEfc++)
    {
        if (rendEfc->state != 0)
        {
            struct RenderEffectFuncs *funcs = rendEfc->funcs;
            void (*func)(struct RenderEffect *) = funcs->main;

            if (func != NULL)
                func(rendEfc);
            if (rendEfc->state == 3)
            {
                func = funcs->destroy;
                if (func != NULL)
                    func(rendEfc);
                rendEfc->state = 0;
            }
        }
    }
}

void event_rend_efc_dest(void)
{
    int i;
    struct RenderEffect *rendEfc = g_haze_type;

    for (i = 4; i > 0; i--, rendEfc++)
    {
        if (rendEfc->state != 0)
        {
            void (*destroy)(struct RenderEffect *) = rendEfc->funcs->destroy;

            if (destroy != NULL)
                destroy(rendEfc);
            rendEfc->state = 0;
        }
    }
}

void g_something_with_rend_efc(int enableFlags)
{
    int i;
    u16 cameraMask;
    struct RenderEffect *rendEfc;

    if (monkey_flags & 0x80)
    {
        perf_init_timer(8);
        rendEfc = g_haze_type;
        cameraMask = 1 << g_something_with_camera3;

        for (i = 4; i > 0; i--, rendEfc++)
        {
            if (rendEfc->state != 0 && (rendEfc->enableFlags & enableFlags) && (rendEfc->cameraMask & cameraMask))
            {
                void (*draw)(int, struct RenderEffect *) = rendEfc->funcs->draw;

                if (draw != NULL)
                    draw(enableFlags, rendEfc);
            }
        }
        perfInfo.rendEfcTime += perf_stop_timer(8);
    }
}

void fn_1_D8130(int index, int type, struct RenderEffect *params)
{
    struct RenderEffect *rendEfc;

    if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
    {
        rendEfc = &g_haze_type[index];
        if (rendEfc->state != 0)
        {
            void (*destroy)(struct RenderEffect *) = rendEfc->funcs->destroy;

            if (destroy != NULL)
                destroy(rendEfc);
            rendEfc->state = 0;
        }
        memcpy_handler(rendEfc, params, sizeof(*rendEfc));
        if (type != 0)
            rendEfc->funcs = &lbl_1_data_53330[type];
        if (rendEfc->funcs == NULL)
        {
            rendEfc->state = 0;
            return;
        }
        rendEfc->state = 1;
        {
            void (*init)(struct RenderEffect *) = rendEfc->funcs->init;

            if (init != NULL)
                init(rendEfc);
        }
        if (rendEfc->state == 3)
        {
            void (*destroy)(struct RenderEffect *) = rendEfc->funcs->destroy;

            if (destroy != NULL)
                destroy(rendEfc);
            rendEfc->state = 0;
        }
    }
}

void fn_1_D8264(int index)
{
    struct RenderEffect *rendEfc;

    if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
    {
        rendEfc = &g_haze_type[index];
        if (rendEfc->state != 0)
        {
            void (*destroy)(struct RenderEffect *) = rendEfc->funcs->destroy;

            if (destroy != NULL)
                destroy(rendEfc);
            rendEfc->state = 0;
        }
    }
}

void fn_1_D82E4(struct RenderEffect *rendEfc)
{
    struct RenderEffectBlur *work;

    rendEfc->enableFlags = 8;
    work = OSAllocFromHeap(currentHeap, sizeof(*work));
    if (work == NULL)
    {
        rendEfc->work = NULL;
        rendEfc->state = 0;
        return;
    }
    rendEfc->work = work;
    work->imageBuf = lbl_801988E0;
    GXInitTexObj(&work->texObj, work->imageBuf, 640, 448, 4, 0, 0, 0);
}

void fn_1_D8378(struct RenderEffect *rendEfc)
{
    if (rendEfc->work != NULL)
        OSFreeToHeap(currentHeap, rendEfc->work);
}

void fn_1_D83B8(struct RenderEffect *rendEfc) {}

void fn_1_D83BC(int arg0, struct RenderEffect *rendEfc)
{
    float halfW;
    float halfH;
    float offX;
    float offY;
    float scale;
    float xOff;
    float yOff;
    struct RenderEffectBlur *work;
    int stage;
    int coord;
    int texMtx;
    u16 width;
    u16 height;
    float fbWidth;
    float fbHeight;
    u16 left;
    u16 top;

    work = rendEfc->work;

    fbWidth = current_render_mode->fbWidth;
    fbHeight = current_render_mode->efbHeight;
    left = fbWidth * g_current_camera->sub28.viewportX;
    top = fbHeight * g_current_camera->sub28.viewportY;
    width = fbWidth * g_current_camera->sub28.viewportW;
    height = fbHeight * g_current_camera->sub28.viewportH;
    left = left & ~1;
    top = top & ~1;
    width = (width + 1) & ~1;
    height = (height + 1) & ~1;

    GXSetTexCopySrc_cached(left, top, width, height);
    GXSetTexCopyDst_cached(width, height, 4, 0);
    GXCopyTex(work->imageBuf, 0);
    GXInitTexObj(&work->texObj, work->imageBuf, width, height, 4, 0, 0, 0);
    GXSetNumChans_cached(0);
    GXLoadTexObj_cached(&work->texObj, 0);

    stage = 0;
    coord = 0;
    texMtx = 30;
    GXSetTexCoordGen2_cached(coord, 1, 4, texMtx, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 15, 8, 14, 15);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    texMtx += 3;

    GXSetTexCoordGen2_cached(coord, 1, 4, texMtx, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 15, 8, 14, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    texMtx += 3;

    GXSetTexCoordGen2_cached(coord, 1, 4, texMtx, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 15, 8, 14, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    texMtx += 3;

    GXSetTexCoordGen2_cached(coord, 1, 4, texMtx, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevKAlphaSel_cached(stage, 2);
    GXSetTevColorIn_cached(stage, 15, 8, 14, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 3, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;

    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(0, 7, 0);
    GXSetCullMode_cached(2);
    fn_1_E2F1C();
    scale = 3.0f;
    xOff = scale / width;
    yOff = scale / height;
    mtxa_from_identity();
    mtxa->mtxA[0][3] = xOff;
    mtxa->mtxA[1][3] = 0.0f;
    GXLoadTexMtxImm(mtxa->mtxA, 30, 1);
    mtxa->mtxA[0][3] = -xOff;
    mtxa->mtxA[1][3] = 0.0f;
    GXLoadTexMtxImm(mtxa->mtxA, 33, 1);
    mtxa->mtxA[0][3] = 0.0f;
    mtxa->mtxA[1][3] = yOff;
    GXLoadTexMtxImm(mtxa->mtxA, 36, 1);
    mtxa->mtxA[0][3] = 0.0f;
    mtxa->mtxA[1][3] = -yOff;
    GXLoadTexMtxImm(mtxa->mtxA, 39, 1);
    g_something_with_GX_vtx_desc((1 << 13) | (1 << 9));
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);

    halfW = g_current_camera->sub28.view.tanHalfFov * g_current_camera->sub28.view.aspect;
    halfH = g_current_camera->sub28.view.tanHalfFov;
    offX = halfW * g_current_camera->sub28.view.unk28;
    offY = halfH * g_current_camera->sub28.view.unk2C;
    GXBegin(0x80, 0, 4);
    GXPosition3f32(-halfW - offX, halfH - offY, -1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(halfW - offX, halfH - offY, -1.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(halfW - offX, -halfH - offY, -1.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-halfW - offX, -halfH - offY, -1.0f);
    GXTexCoord2f32(0.0f, 1.0f);

    GXSetZMode_cached(1, 3, 1);
}

void fn_1_D8B4C(struct RenderEffect *rendEfc)
{
    struct RenderEffectBlur *work;
    u32 bufSize;

    rendEfc->enableFlags = 2;
    work = OSAllocFromHeap(currentHeap, sizeof(*work));
    if (work == NULL)
    {
        rendEfc->state = 0;
        rendEfc->work = NULL;
        return;
    }
    rendEfc->work = work;
    bufSize = GXGetTexBufferSize(640, 448, 6, 0, 0);
    work->imageBuf = OSAllocFromHeap(currentHeap, bufSize);
    if (work->imageBuf == NULL)
    {
        OSFreeToHeap(currentHeap, work);
        rendEfc->state = 0;
        return;
    }
    GXInitTexObj(&work->texObj, work->imageBuf, 640, 448, 6, 0, 0, 0);
}

void fn_1_D8C3C(struct RenderEffect *rendEfc)
{
    if (rendEfc->work != NULL)
    {
        if (((struct RenderEffectBlur *)rendEfc->work)->imageBuf != NULL)
            OSFreeToHeap(currentHeap, ((struct RenderEffectBlur *)rendEfc->work)->imageBuf);
        OSFreeToHeap(currentHeap, rendEfc->work);
    }
}

void fn_1_D8CAC(struct RenderEffect *rendEfc) {}

void fn_1_D8CB0(int arg0, struct RenderEffect *rendEfc)
{
    GXColor color;
    Mtx44 mtx;
    u8 unused[4];
    struct RenderEffectBlur *work;
    int stage;
    int coord;
    float halfW;
    float halfH;
    float tan;

    work = rendEfc->work;
    C_MTXPerspective(mtx, 59.996338f, 1.3333334f, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(mtx, 0);
    GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->efbHeight, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
    GXSetNumChans_cached(0);
    GXLoadTexObj_cached(&work->texObj, 0);
    stage = 0;
    coord = 0;
    color.a = 192;
    GXSetTevKColor_cached(0, color);
    GXSetTevKAlphaSel_cached(stage, 28);
    GXSetTexCoordGen2_cached(coord, 1, 4, 60, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevColorIn_cached(stage, 15, 15, 15, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(0, 7, 0);
    GXSetCullMode_cached(2);
    fn_1_E2F1C();
    g_something_with_GX_vtx_desc((1 << 13) | (1 << 9));
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    tan = math_tan(0x1555);
    halfW = 1.3333334f * tan;
    halfH = tan;

    GXBegin(0x80, 0, 4);
    GXPosition3f32(-halfW, halfH, -1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(halfW, halfH, -1.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(halfW, -halfH, -1.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-halfW, -halfH, -1.0f);
    GXTexCoord2f32(0.0f, 1.0f);

    GXSetTexCopySrc_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
    GXSetTexCopyDst_cached(current_render_mode->fbWidth, current_render_mode->efbHeight, 6, 0);
    GXCopyTex(work->imageBuf, 0);
    GXInitTexObj(&work->texObj, work->imageBuf, current_render_mode->fbWidth, current_render_mode->efbHeight, 6, 0, 0, 0);
    GXSetZMode_cached(1, 3, 1);
    camera_apply_viewport(g_something_with_camera3);
}

static inline void focus_reset_cams(struct RenderEffectFocus *work, struct Camera *camera)
{
    struct RenderEffectFocusCam *cam;
    int i;

    cam = work->cams;
    for (i = 5; i > 0; i--, camera++, cam++)
    {
        cam->fov = camera->sub28.view.fov;
        cam->distance = 1.0f;
        cam->velocity = 0.0f;
        cam->amount = 0.0f;
        cam->disabled = 0;
        cam->waiting = 1;
    }
}

void fn_1_D90F4(struct RenderEffect *rendEfc)
{
    struct RenderEffectFocus *work;

    rendEfc->enableFlags = 8;
    work = OSAllocFromHeap(currentHeap, sizeof(*work));
    if (work == NULL)
    {
        rendEfc->state = 0;
        rendEfc->work = NULL;
        return;
    }
    rendEfc->work = work;
    work->imageBuf = lbl_801988E0;
    GXInitTexObj(&work->texObj, work->imageBuf, 640, 448, 4, 0, 0, 0);
    {
        struct Camera *camera = cameras;

        focus_reset_cams(work, camera);
    }
}

void fn_1_D920C(struct RenderEffect *rendEfc)
{
    if (rendEfc->work != NULL)
        OSFreeToHeap(currentHeap, rendEfc->work);
}

void fn_1_D924C(struct RenderEffect *rendEfc)
{
    int i;
    struct RenderEffectFocusCam *cam;
    struct Camera *camera;
    struct RenderEffectFocus *work = rendEfc->work;

    camera = cameras;
    cam = work->cams;
    for (i = 5; i > 0; i--, camera++, cam++)
    {
        int fov;
        float dist;
        float delta;
        float diff;

        if (camera->unk26 != 5)
        {
            cam->disabled = 1;
            cam->waiting = 1;
        }
        else if (cam->waiting == 0)
            cam->disabled = 1;
        else if (get_gpu_frame_time() < 0.95f)
            cam->disabled = 0;
        else
        {
            cam->waiting = 0;
            cam->disabled = 1;
        }

        fov = camera->sub28.view.fov;
        diff = math_tan(cam->fov >> 1) - math_tan(fov >> 1);
        cam->amount = 512.0 * fabs(diff);
        cam->fov = fov;
        dist = mathutil_vec_distance(&camera->eye, &camera->lookAt);
        delta = cam->distance - dist;
        diff = cam->velocity - delta;
        cam->amount += 64.0 * fabs(diff);
        cam->distance = dist;
        cam->velocity += (delta - cam->velocity) * 0.2f;
        cam->amount -= 0.25f;
    }
}

void fn_1_D9400(int arg0, struct RenderEffect *rendEfc)
{
    float yOff;
    float x0;
    float xOff;
    float y0;
    float x1;
    float y1;
    float amount;
    struct RenderEffectFocus *work;
    int stage;
    int coord;
    int texMtx;
    u16 width;
    u16 height;
    float fbWidth;
    float fbHeight;
    u16 left;
    u16 top;
    float proj[7];

    if (g_some_other_flags & 0xA)
        return;

    work = rendEfc->work;

    if (work->cams[g_something_with_camera3].disabled != 0)
        return;
    amount = work->cams[g_something_with_camera3].amount;
    if (amount <= 0.0f)
        return;

    if (amount > 5.0f)
        amount = 5.0f;

    fbWidth = current_render_mode->fbWidth;
    fbHeight = current_render_mode->efbHeight;
    left = fbWidth * g_current_camera->sub28.viewportX;
    top = fbHeight * g_current_camera->sub28.viewportY;
    width = fbWidth * g_current_camera->sub28.viewportW;
    height = fbHeight * g_current_camera->sub28.viewportH;
    left = left & ~1;
    top = top & ~1;
    width = (width + 1) & ~1;
    height = (height + 1) & ~1;

    GXSetTexCopySrc_cached(left, top, width, height);
    GXSetTexCopyDst_cached(width, height, 4, 0);
    GXCopyTex(work->imageBuf, 0);
    GXInitTexObj(&work->texObj, work->imageBuf, width, height, 4, 0, 0, 0);
    GXSetNumChans_cached(0);
    GXLoadTexObj_cached(&work->texObj, 0);

    stage = 0;
    coord = 0;
    texMtx = 30;
    GXSetTexCoordGen2_cached(coord, 1, 4, texMtx, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 15, 8, 14, 15);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    texMtx += 3;

    GXSetTexCoordGen2_cached(coord, 1, 4, texMtx, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 15, 8, 14, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    texMtx += 3;

    GXSetTexCoordGen2_cached(coord, 1, 4, texMtx, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 15, 8, 14, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    texMtx += 3;

    GXSetTexCoordGen2_cached(coord, 1, 4, texMtx, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevKAlphaSel_cached(stage, 2);
    GXSetTevColorIn_cached(stage, 15, 8, 14, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 3, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;

    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(0, 7, 0);
    GXSetCullMode_cached(0);
    fn_1_E2F1C();
    xOff = amount / width;
    yOff = amount / height;
    mtxa_from_identity();
    mtxa->mtxA[0][3] = xOff;
    mtxa->mtxA[1][3] = 0.0f;
    GXLoadTexMtxImm(mtxa->mtxA, 30, 1);
    mtxa->mtxA[0][3] = -xOff;
    mtxa->mtxA[1][3] = 0.0f;
    GXLoadTexMtxImm(mtxa->mtxA, 33, 1);
    mtxa->mtxA[0][3] = 0.0f;
    mtxa->mtxA[1][3] = yOff;
    GXLoadTexMtxImm(mtxa->mtxA, 36, 1);
    mtxa->mtxA[0][3] = 0.0f;
    mtxa->mtxA[1][3] = -yOff;
    GXLoadTexMtxImm(mtxa->mtxA, 39, 1);
    g_something_with_GX_vtx_desc((1 << 13) | (1 << 9));
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);

    GXGetProjectionv(proj);
    x0 = (-1.0f * (1.0f - proj[2])) / proj[1];
    y0 = (-1.0f * (-1.0f - proj[4])) / proj[3];
    x1 = (-1.0f * (-1.0f - proj[2])) / proj[1];
    y1 = (-1.0f * (1.0f - proj[4])) / proj[3];
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x0, y0, -1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y0, -1.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x1, y1, -1.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x0, y1, -1.0f);
    GXTexCoord2f32(0.0f, 1.0f);

    GXSetZMode_cached(1, 3, 1);
}

void fn_1_D9BE8(struct RenderEffect *rendEfc)
{
    struct RenderEffectKaleidoscope *work;
    u32 bufSize;

    rendEfc->enableFlags = 2;
    work = OSAllocFromHeap(currentHeap, sizeof(*work));
    if (work == NULL)
    {
        rendEfc->state = 0;
        rendEfc->work = NULL;
        return;
    }
    rendEfc->work = work;
    work->mode = 0;
    bufSize = GXGetTexBufferSize(640, 448, 5, 0, 0);
    work->colorBuf = OSAllocFromHeap(currentHeap, bufSize);
    if (work->colorBuf == NULL)
    {
        OSFreeToHeap(currentHeap, work);
        rendEfc->state = 0;
        return;
    }
    bufSize = GXGetTexBufferSize(640, 448, 1, 0, 0);
    work->alphaBuf = OSAllocFromHeap(currentHeap, bufSize);
    if (work->alphaBuf == NULL)
    {
        OSFreeToHeap(currentHeap, work->colorBuf);
        OSFreeToHeap(currentHeap, work);
        rendEfc->state = 0;
        return;
    }
}

void fn_1_D9D2C(struct RenderEffect *rendEfc)
{
    if (rendEfc->work != NULL)
    {
        if (((struct RenderEffectKaleidoscope *)rendEfc->work)->colorBuf != NULL)
            OSFreeToHeap(currentHeap, ((struct RenderEffectKaleidoscope *)rendEfc->work)->colorBuf);
        if (((struct RenderEffectKaleidoscope *)rendEfc->work)->alphaBuf != NULL)
            OSFreeToHeap(currentHeap, ((struct RenderEffectKaleidoscope *)rendEfc->work)->alphaBuf);
        OSFreeToHeap(currentHeap, rendEfc->work);
    }
}

void fn_1_D9DC4(struct RenderEffect *rendEfc)
{
    struct RenderEffectKaleidoscope *work = rendEfc->work;

    if (pad_status_groups[0].unk0[2].button & 0x100)
    {
        if (++work->mode >= 3)
            work->mode = 0;
    }
}

void fn_1_D9E00(int arg0, struct RenderEffect *rendEfc)
{
    GXColor color;
    Mtx44 mtx;
    u8 unused[4];
    struct RenderEffectKaleidoscope *work = rendEfc->work;
    int stage;
    int coord;
    float halfW;
    float halfH;
    float tan;

    C_MTXPerspective(mtx, 59.996338f, 1.3333334f, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(mtx, 0);
    GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->efbHeight, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
    GXSetNumChans_cached(0);
    switch (work->mode)
    {
    case 0:
        GXSetTexCopySrc_cached(192, 112, 256, 256);
        GXSetTexCopyDst_cached(256, 256, 5, 0);
        GXCopyTex(work->colorBuf, 0);
        GXInitTexObj(&work->colorTex, work->colorBuf, 256, 256, 5, 1, 1, 0);
        break;
    case 1:
        GXSetTexCopySrc_cached(192, 112, 256, 256);
        GXSetTexCopyDst_cached(256, 256, 5, 0);
        GXCopyTex(work->colorBuf, 0);
        GXInitTexObj(&work->colorTex, work->colorBuf, 256, 256, 5, 2, 2, 0);
        break;
    case 2:
        GXSetTexCopySrc_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
        GXSetTexCopyDst_cached(current_render_mode->fbWidth, current_render_mode->efbHeight, 5, 0);
        GXCopyTex(work->colorBuf, 0);
        GXInitTexObj(&work->colorTex, work->colorBuf, current_render_mode->fbWidth, current_render_mode->efbHeight, 5, 0, 0, 0);
        GXSetTexCopySrc_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
        GXSetTexCopyDst_cached(current_render_mode->fbWidth, current_render_mode->efbHeight, 1, 0);
        GXCopyTex(work->alphaBuf, 0);
        GXInitTexObj(&work->alphaTex, work->alphaBuf, current_render_mode->fbWidth, current_render_mode->efbHeight, 1, 0, 0, 0);
        break;
    }
    switch (work->mode)
    {
    case 0:
    case 1:
        GXLoadTexObj_cached(&work->colorTex, 0);
        break;
    case 2:
        GXLoadTexObj_cached(&work->alphaTex, 0);
        break;
    }
    stage = 0;
    coord = 0;
    color.a = 0xFF;
    GXSetTevKColor_cached(0, color);
    GXSetTevKAlphaSel_cached(stage, 28);
    GXSetTexCoordGen2_cached(coord, 1, 4, 60, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevColorIn_cached(stage, 15, 15, 15, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(0, 7, 0);
    GXSetCullMode_cached(2);
    fn_1_E2F1C();
    g_something_with_GX_vtx_desc((1 << 13) | (1 << 9));
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    tan = math_tan(0x1555);
    halfW = 1.3333334f * tan;
    halfH = tan;
    switch (work->mode)
    {
    case 0:
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-halfW, halfH, -1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(halfW, halfH, -1.0f);
        GXTexCoord2f32(4.0f, 0.0f);
        GXPosition3f32(halfW, -halfH, -1.0f);
        GXTexCoord2f32(4.0f, 3.0f);
        GXPosition3f32(-halfW, -halfH, -1.0f);
        GXTexCoord2f32(0.0f, 3.0f);
        break;
    case 1:
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-halfW, halfH, -1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(halfW, halfH, -1.0f);
        GXTexCoord2f32(4.0f, 0.0f);
        GXPosition3f32(halfW, -halfH, -1.0f);
        GXTexCoord2f32(4.0f, 4.0f);
        GXPosition3f32(-halfW, -halfH, -1.0f);
        GXTexCoord2f32(0.0f, 4.0f);
        break;
    case 2:
        GXBegin(0x80, 0, 4);
        GXPosition3f32(-halfW, halfH, -1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(halfW, halfH, -1.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(halfW, -halfH, -1.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-halfW, -halfH, -1.0f);
        GXTexCoord2f32(0.0f, 1.0f);

        GXLoadTexObj_cached(&work->colorTex, 0);

        GXBegin(0x80, 0, 4);
        GXPosition3f32(0.1 + -halfW, halfH - 0.075, -1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(halfW - 0.1, halfH - 0.075, -1.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(halfW - 0.1, 0.075 + -halfH, -1.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(0.1 + -halfW, 0.075 + -halfH, -1.0f);
        GXTexCoord2f32(0.0f, 1.0f);
        break;
    }
    GXSetZMode_cached(1, 3, 1);
    camera_apply_viewport(g_something_with_camera3);
}

void g_init_rendefc_for_stage(void)
{
    struct RenderEffect rendEfc;

    switch (modeCtrl.gameType)
    {
    case 0:
    case 2:
    case 4:
    case 14:
        if (stagedef->reflectives != NULL)
        {
            memset(&rendEfc, 0, sizeof(rendEfc));
            rendEfc.cameraMask = 0xFFFF;
            fn_1_D8130(1, 5, &rendEfc);
        }
        else if (g_some_draw_var != NULL || current_stage_id == 179)
        {
            memset(&rendEfc, 0, sizeof(rendEfc));
            rendEfc.cameraMask = 0xFFFF;
            fn_1_D8130(1, 6, &rendEfc);
        }
        else if (is_stage_id_not_for_party_game(current_stage_id))
        {
            memset(&rendEfc, 0, sizeof(rendEfc));
            rendEfc.cameraMask = 0xFFFF;
            fn_1_D8130(1, 7, &rendEfc);
        }
        break;
    case 7:
    case 10:
        memset(&rendEfc, 0, sizeof(rendEfc));
        rendEfc.cameraMask = 0xFFFF;
        fn_1_D8130(1, 6, &rendEfc);
        break;
    }
}
