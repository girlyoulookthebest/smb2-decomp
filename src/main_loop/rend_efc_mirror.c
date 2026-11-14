#pragma peephole off

#include "types.h"
/* This TU passes a truncated double straight through, without the s16 sign extension. */
#define math_tan math_tan_s16
#include "mathutil.h"
#undef math_tan
#include "gx.h"
#include "ball.h"
#include "avdisp.h"
#include "camera.h"
#include "effect.h"
#include "event.h"
#include "item.h"
#include "light.h"
#include "mode.h"
#include "nl2ngc.h"
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

struct RenderEffectFlatMirror
{
    /*0x00*/ GXTexObj texObj;
    /*0x20*/ int format;
    /*0x24*/ void *imageBuf;
    /*0x28*/ u16 xres;
    /*0x2A*/ u16 yres;
    /*0x2C*/ Mtx unk2C;
    u8 filler5C[4];
    /*0x60*/ u8 unk60;
};  /* size = 0x64 */

struct RenderEffectWavyMirror
{
    /*0x00*/ GXTexObj unk0;
    /*0x20*/ int format;
    /*0x24*/ u16 xres;
    /*0x26*/ u16 yres;
    /*0x28*/ void *unk28;
    /*0x2C*/ Mtx unk2C;
    /*0x5C*/ Mtx unk5C;
    u8 filler8C[4];
    /*0x90*/ u8 unk90;
    /*0x94*/ GXTexObj unk94;
    /*0xB4*/ void *unkB4;
};  /* size = 0xB8 */

/* Texture slots handed to the avdisp tev callback; written back when it returns. */
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

struct AvdispTevMaterial
{
    u8 filler0[0x12];
    /*0x12*/ u8 indirect;
};

struct AvdispTevTexture  /* a GMA TEV layer (disc layout) */
{
    u8 filler0[8];
    /*0x08*/ GXTexObj * DPTR texObj;
};

struct AvdispTevContext
{
    u8 filler0[4];
    /*0x04*/ struct AvdispTevMaterial *material;
    /*0x08*/ struct AvdispTevTexture *texture;
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
#ifdef __MWERKS__
struct MiniTargetPlayer
{
    u8 filler0[0x478];
    /*0x478*/ float unk478;
    u8 filler47C[0x4F0 - 0x47C];
};

extern struct MiniTargetPlayer lbl_14_bss_208[];
#define BOAT_PLAYER_UNK478(i) (lbl_14_bss_208[i].unk478)
#else
/* the boat REL's BoatPlayer holds pointers (x64: 0x4F8 bytes, unk478 at
 * 0x480), so this view is wrong there; read it through boat_1.c */
float pc_boat_player_unk478(int i);
#define BOAT_PLAYER_UNK478(i) pc_boat_player_unk478(i)
#endif
extern struct GMAModel *lbl_14_bss_B4[];  /* boat REL model table */
extern u32 g_font_char_var_3;

void *OSAllocFromHeap(int heap, u32 size);
void OSFreeToHeap(int heap, void *ptr);
void *memset(void *dest, int c, u32 n);
void *memcpy(void *dest, const void *src, u32 n);
void *get_GmaBuffer_entry(struct GMA *gma, char *name);
float math_tan(int angle);

void fn_1_E2F1C(void);
void fn_1_E2D20(void);
void g_set_something3(int arg0);
void g_draw_stage(void);
void g_draw_bg(void);
void g_draw_stobjs(void);
void g_draw_ball_and_ape(void);
void g_draw_func_init(void);
void g_mini_draw_func_handler(void);
void g_smth_with_rendefc_reflective_height(float height);
void draw_reflective_objects_in_viewstage(void);
void ord_tbl_draw_nodes(void);
void *g_yet_another_unk_draw_func(void *func);
void *fn_1_10A69C(void *func);
void fn_14_469C(void);
void fn_14_13AB0(void);
void fn_5_16438(void);

u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp, u8 doEdgeLod, int maxAniso);
void GXLoadPosMtxImm(Mtx m, u32 id);
void load_gx_pos_nrm_mtx(Mtx m, int id);
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
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int matrixSel, int wrapS, int wrapT, u8 addPrev, u8 utcLod, int alphaSel);
void GXSetIndTexOrder_cached(int indStage, int coord, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXCopyTex(void *dest, u8 clear);
void GXInvalidateTexAll(void);
void GXSetViewport(float left, float top, float wd, float ht, float nearz, float farz);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], s8 scale);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);

void g_reflective_object_draw_handler(int unused, struct RenderEffect *rendEfc);
void g_reflective_object_draw_handler_2(int unused, struct RenderEffect *rendEfc);
void fn_1_DBBAC(GXTexObj *tex, Mtx mtx);
void fn_1_DBBE8(struct AvdispTevContext *ctx);
int fn_1_DBF48();
int fn_1_DBF50();
void fn_1_DC21C(int unused, struct RenderEffect *rendEfc);
void fn_1_DCAD0(int unused, struct RenderEffect *rendEfc);
void fn_1_DCE60(int unused, struct RenderEffect *rendEfc);

static struct
{
    /*0x00*/ GXTexObj *tex;
    /*0x04*/ Mtx mtx;
} s_mirrorTevArgs;

void g_smth_with_loading_reflective_stgobjs(struct RenderEffect *rendEfc)
{
    struct RenderEffectFlatMirror *work;
    struct StageReflectiveModel *refl;
    int i;
    u32 bufSize;

    rendEfc->enableFlags = 0x11;
    work = OSAllocFromHeap(stage_heap, sizeof(*work));
    if (work == NULL)
    {
        rendEfc->state = 0;
        return;
    }
    rendEfc->work = work;
    work->format = 6;
    if (stagedef->dynamicReflectionPlane != NULL)
    {
        switch (stagedef->dynamicReflectionPlane->texFormat)
        {
        case 4:
            work->format = 0;
        case 3:
            work->format = 1;
        case 2:
            work->format = 4;
        case 1:
            work->format = 5;
        case 0:
            work->format = 6;
        }
    }
    work->xres = 640;
    work->yres = 448;
    if (stagedef->dynamicReflectionPlane != NULL)
    {
        work->xres = work->xres >> (u16)stagedef->dynamicReflectionPlane->resShift;
        work->yres = work->yres >> (u16)stagedef->dynamicReflectionPlane->resShift;
    }
    bufSize = GXGetTexBufferSize(work->xres, work->yres, work->format, 0, 0);
    work->imageBuf = OSAllocFromHeap(stage_heap, bufSize);
    if (work->imageBuf == NULL)
    {
        OSFreeToHeap(stage_heap, work);
        rendEfc->state = 0;
        return;
    }
    refl = stagedef->reflectives;
    for (i = 0; i < stagedef->reflectiveCount; i++, refl++)
        refl->model = get_GmaBuffer_entry(stage_gma, refl->modelName);
    work->unk60 = 0xFF;
    mtxa_from_identity();
    if (stagedef->dynamicReflectionPlane != NULL && (stagedef->dynamicReflectionPlane->flags & 1))
    {
        mtxa_translate(&stagedef->dynamicReflectionPlane->pos);
        mtxa_rotate_z(stagedef->dynamicReflectionPlane->rot.z);
        mtxa_rotate_y(stagedef->dynamicReflectionPlane->rot.y);
        mtxa_rotate_x(stagedef->dynamicReflectionPlane->rot.x);
    }
    else
    {
        struct GMAModel *model = NULL;
        refl = stagedef->reflectives;
        for (i = 0; i < stagedef->reflectiveCount; i++)
        {
            if (refl->model != NULL)
            {
                model = refl->model;
                break;
            }
        }
        if (model != NULL)
            mtxa_translate_xyz(0.0f, model->boundSphereCenter.y, 0.0f);
    }
    mtxa_to_mtx(work->unk2C);
}

void fn_1_DAF6C(struct RenderEffect *rendEfc)
{
    if (rendEfc->work != NULL)
    {
        if (((struct RenderEffectFlatMirror *)rendEfc->work)->imageBuf != NULL)
            OSFreeToHeap(stage_heap, ((struct RenderEffectFlatMirror *)rendEfc->work)->imageBuf);
        OSFreeToHeap(stage_heap, rendEfc->work);
    }
}

void fn_1_DAFDC(struct RenderEffect *rendEfc) {}

void g_smth_calling_reflective_obj_draw_hdlr(int arg0, struct RenderEffect *rendEfc)
{
    switch (arg0)
    {
    case 1:
        g_reflective_object_draw_handler(arg0, rendEfc);
        break;
    case 16:
        g_reflective_object_draw_handler_2(arg0, rendEfc);
        break;
    }
}

void g_reflective_object_draw_handler(int unused, struct RenderEffect *rendEfc)
{
    struct Camera cameraBackup;
    struct RenderEffectFlatMirror *work;
    struct Camera *camera;
    Mtx planeMtx;

    camera_apply_viewport(modeCtrl.currPlayer);
    work = rendEfc->work;
    camera = g_current_camera;
    cameraBackup = *camera;
    push_light_group();
    mtxa_from_identity();
    if (stagedef->dynamicReflectionPlane != NULL)
    {
        int idx = stagedef->dynamicReflectionPlane->itemgroupIdx;

        if (idx > 0)
            mtxa_mult_right(itemgroups[idx].transform);
    }
    mtxa_mult_right(work->unk2C);
    mtxa_to_mtx(planeMtx);

    {
        Mtx spB0;
        Mtx sp80;
        Mtx sp50;
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
        mtxa_to_mtx(sp50);
        mtxa_mult_right(planeMtx);
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
        mtxa_to_mtx(sp80);
        mtxa_from_identity();
        mtxa_scale_xyz(-1.0f, 1.0f, 1.0f);
        mtxa_to_mtx(spB0);
        mtxa_from_identity();
        mtxa_rotate_z(-camera->rotZ);
        mtxa_rotate_x(-camera->rotX);
        mtxa_rotate_y(-camera->rotY);
        mtxa_translate_neg(&camera->eye);
        mtxa_mult_right(sp80);
        mtxa_mult_right(sp50);
        mtxa_mult_left(spB0);
        mtxa_to_mtx(camera->unk14C);
        mtxa_to_mtx(camera->unk17C);
        mtxa_from_identity();
        mtxa_rotate_z(-camera->rotZ);
        mtxa_rotate_x(-camera->rotX);
        mtxa_rotate_y(-camera->rotY);
        mtxa_translate_neg(&camera->eye);
        mtxa_mult_right(sp80);
        mtxa_translate(&camera->eye);
        mtxa_rotate_y(camera->rotY);
        mtxa_rotate_x(camera->rotX);
        mtxa_rotate_z(camera->rotZ);
        mtxa_mult_right(g_ord_tbl_stuff->matrices[0]);
        mtxa_mult_left(spB0);
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
        mtxa_from_mtx(planeMtx);
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
        u32 mask = 0xFFFFFFFF;

        if (stagedef->dynamicReflectionPlane != NULL && stagedef->dynamicReflectionPlane->drawMask != 0)
            mask = stagedef->dynamicReflectionPlane->drawMask;
        if (mask & 2)
        {
            s8 state = events[EVENT_STAGE].state;

            if (state == EV_RUNNING || state == EV_SUSPENDED)
                g_draw_stage();
        }
        if ((mask & 4) && events[EVENT_BACKGROUND].state == EV_RUNNING)
        {
            g_smth_with_rendefc_reflective_height(400.0f);
            g_draw_bg();
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        if ((mask & 8) && events[EVENT_ITEM].state == EV_RUNNING)
            draw_items();
        if ((mask & 0x20) && events[EVENT_STOBJ].state == EV_RUNNING)
            g_draw_stobjs();
        if ((mask & 0x10) && events[EVENT_EFFECT].state == EV_RUNNING)
            g_draw_effects();
        if ((mask & 1) && events[EVENT_BALL].state == EV_RUNNING)
            g_draw_ball_and_ape();
        ord_tbl_draw_nodes();
    }
    else
        draw_reflective_objects_in_viewstage();
    g_some_render_flag.flags &= ~4;
    GXSetZMode_cached(1, 3, 1);
    GXSetTexCopySrc_cached(0, 0, work->xres, work->yres);
    GXSetTexCopyDst_cached(work->xres, work->yres, work->format, 0);
    GXCopyTex(work->imageBuf, 1);
    GXInitTexObj(&work->texObj, work->imageBuf, work->xres, work->yres, work->format, 0, 0, 0);
    *camera = cameraBackup;
    camera_apply_viewport(modeCtrl.currPlayer);
    pop_light_group();
    GXInvalidateTexAll();
    camera_apply_viewport(modeCtrl.currPlayer);
}

void g_reflective_object_draw_handler_2(int unused, struct RenderEffect *rendEfc)
{
    struct ItemGroup *group;
    struct RenderEffectFlatMirror *work;
    void *prevFunc;
    int i;
    struct StageReflectiveModel *refl;
    int j;
    float temp;
    Mtx texMtx;
    Mtx44 projMtx;

    work = rendEfc->work;
    C_MTXPerspective(projMtx, 0.005493164f * g_current_camera->sub28.view.fov, g_current_camera->sub28.view.aspect, 0.1f, 20000.0f);
    mtxa_from_identity();
    mtxa->mtxA[0][2] = 0.5 - 0.5 * (projMtx[0][0] * g_current_camera->sub28.view.unk28 * g_current_camera->sub28.view.aspect * g_current_camera->sub28.view.tanHalfFov);
    mtxa->mtxA[1][2] = 0.5 - 0.5 * (projMtx[1][1] * g_current_camera->sub28.view.unk2C * g_current_camera->sub28.view.tanHalfFov);
    temp = 0.5f / math_tan(0.5 * g_current_camera->sub28.view.fov);
    mtxa->mtxA[0][0] = temp / g_current_camera->sub28.view.aspect;
    mtxa->mtxA[1][1] = temp;
    mtxa_to_mtx(texMtx);
    fn_1_E2F1C();
    prevFunc = g_yet_another_unk_draw_func(fn_1_DBBE8);
    group = itemgroups;
    for (i = 0; i < stagedef->itemGroupCount; i++, group++)
    {
        mtxa_from_mtxb();
        mtxa_mult_right(group->transform);
        mtxa_mult_left(texMtx);
        GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
        fn_1_DBBAC(&work->texObj, mtxa->mtxA);
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(group->transform);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        refl = stagedef->collHeaders[i].reflectives;
        for (j = 0; j < stagedef->collHeaders[i].reflectiveCount; j++, refl++)
        {
            if (refl->model != NULL)
                avdisp_draw_model_unculled_sort_never(refl->model);
        }
    }
    g_yet_another_unk_draw_func(prevFunc);
}

void fn_1_DBBAC(GXTexObj *tex, Mtx mtx)
{
    s_mirrorTevArgs.tex = tex;
    memcpy(s_mirrorTevArgs.mtx, mtx, sizeof(Mtx));
}

void fn_1_DBBE8(struct AvdispTevContext *ctx)
{
    int map0;
    int map1;
    int indirect;
    struct AvdispTevState tev;
    float indMtx[2][3];
    int coord;
    int coord2;
    int texMtx;
    int stage;

    tev = ctx->tev;
    indirect = ctx->material->indirect != 0;
    map0 = tev.texMap++;
    GXLoadTexObj_cached(s_mirrorTevArgs.tex, map0);
    if (indirect != 0)
    {
        map1 = tev.texMap++;
        GXLoadTexObj_cached(ctx->texture->texObj, map1);
    }
    coord = tev.coord++;
    texMtx = tev.texMtx;
    tev.texMtx += 3;
    GXSetTexCoordGen2_cached(coord, 0, 0, texMtx, 0, 0x7D);
    coord2 = tev.coord++;
    GXSetTexCoordGen2_cached(coord2, 0, 4, 0x3C, 0, 0x7D);
    GXLoadTexMtxImm(s_mirrorTevArgs.mtx, texMtx, 0);
    stage = tev.stage++;
    g_GXSetTevOrder_wrapper(stage, coord, map0, 4);
    GXSetTevSwapMode_cached(stage, 0, 0);
    if (indirect != 0)
        GXSetTevColorIn_cached(stage, 8, 0xF, 0xF, 0xF);
    else
        GXSetTevColorIn_cached(stage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 0, 4, 7);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    if (indirect != 0)
    {
        int ind;

        GXSetIndTexOrder_cached(0, coord2, map1);
        ind = tev.indStage++;
        GXSetTevIndirect_cached(stage, ind, 0, 7, 1, 0, 0, 0, 0, 0);
        indMtx[0][0] = 0.16666667f;
        indMtx[0][1] = 0.16666667f;
        indMtx[0][2] = 0.0f;
        indMtx[1][0] = 0.16666667f;
        indMtx[1][1] = 0.16666667f;
        indMtx[1][2] = 0.0f;
        g_GXSetIndTexMtx_wrapper(tev.indMtx++, indMtx, 0);
    }
    else
        g_GXSetTevIndirect_zero_if_different(stage);
    ctx->tev = tev;
}

int fn_1_DBF48()
{
    return 1;
}

int fn_1_DBF50()
{
    return 1;
}

void g_some_rendefc_func_1(struct RenderEffect *rendEfc)
{
    struct RenderEffectWavyMirror *work;
    u32 bufSize;

    rendEfc->enableFlags = 0x11;
    work = OSAllocFromHeap(stage_heap, sizeof(*work));
    if (work == NULL)
    {
        rendEfc->state = 0;
        return;
    }
    memset(work, 0, sizeof(*work));
    rendEfc->work = work;
    work->format = 6;
    work->xres = 640;
    work->yres = 224;
    bufSize = GXGetTexBufferSize(work->xres, work->yres, work->format, 0, 0);
    work->unk28 = OSAllocFromHeap(stage_heap, bufSize);
    if (work->unk28 == NULL)
    {
        OSFreeToHeap(stage_heap, work);
        rendEfc->state = 0;
        return;
    }
    bufSize = GXGetTexBufferSize(256, 256, 3, 0, 0);
    work->unkB4 = OSAllocFromHeap(stage_heap, bufSize);
    if (work->unkB4 == NULL)
    {
        OSFreeToHeap(stage_heap, work->unk28);
        OSFreeToHeap(stage_heap, work);
        rendEfc->state = 0;
        return;
    }
    switch (current_stage_id)
    {
    case 0x8A:
    case 0x8B:
    case 0x8C:
        work->unk90 = 0x40;
        break;
    case 0x9F:
    case 0xB3:
        work->unk90 = 0x20;
        break;
    default:
        work->unk90 = 0x60;
        break;
    }
}

void fn_1_DC10C(struct RenderEffect *rendEfc)
{
    if (rendEfc->work != NULL)
    {
        if (((struct RenderEffectWavyMirror *)rendEfc->work)->unk28 != NULL)
            OSFreeToHeap(stage_heap, ((struct RenderEffectWavyMirror *)rendEfc->work)->unk28);
        if (((struct RenderEffectWavyMirror *)rendEfc->work)->unkB4 != NULL)
            OSFreeToHeap(stage_heap, ((struct RenderEffectWavyMirror *)rendEfc->work)->unkB4);
        OSFreeToHeap(stage_heap, rendEfc->work);
    }
}

void fn_1_DC1A4(struct RenderEffect *rendEfc) {}

void fn_1_DC1A8(int arg0, struct RenderEffect *rendEfc)
{
    switch (arg0)
    {
    case 1:
        fn_1_DC21C(arg0, rendEfc);
        fn_1_DCAD0(arg0, rendEfc);
        break;
    case 16:
        fn_1_DCE60(arg0, rendEfc);
        break;
    }
}

void fn_1_DC21C(int unused, struct RenderEffect *rendEfc)
{
    struct Camera cameraBackup;
    struct RenderEffectWavyMirror *work;
    struct Camera *camera;

    if (!(g_some_render_flag.flags & 0x20))
    {
        camera_apply_viewport(modeCtrl.currPlayer);
        light_main();
        work = rendEfc->work;
        camera = g_current_camera;
        cameraBackup = *camera;
        push_light_group();
        mtxa_from_identity();
        switch (current_stage_id)
        {
        default:
            mtxa_translate_xyz(0.0f, 0.02f, 0.0f);
            mtxa_to_mtx(work->unk2C);
            break;
        case 0x8A:
        case 0x8B:
            mtxa_translate_xyz(0.0f, -0.4f, 0.0f);
            mtxa_to_mtx(work->unk2C);
            break;
        case 0x8C:
            mtxa_from_translate_xyz(0.0f, BOAT_PLAYER_UNK478(current_ball->playerId) - 0.4f, 0.0f);
            mtxa_to_mtx(work->unk2C);
            break;
        case 0x9F:
        case 0xB3:
            mtxa_translate_xyz(0.0f, -0.39f, 0.0f);
            mtxa_to_mtx(work->unk2C);
            break;
        }
        mtxa_to_mtx(work->unk5C);

        {
            Mtx spB0;
            Mtx sp80;
            Mtx sp50;
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
            mtxa_to_mtx(sp50);
            mtxa_mult_right(work->unk2C);
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
            mtxa_to_mtx(sp80);
            mtxa_from_identity();
            mtxa_scale_xyz(-1.0f, 1.0f, 1.0f);
            mtxa_to_mtx(spB0);
            mtxa_from_identity();
            mtxa_rotate_z(-camera->rotZ);
            mtxa_rotate_x(-camera->rotX);
            mtxa_rotate_y(-camera->rotY);
            mtxa_translate_neg(&camera->eye);
            mtxa_mult_right(sp80);
            mtxa_mult_right(sp50);
            mtxa_mult_left(spB0);
            mtxa_to_mtx(camera->unk14C);
            mtxa_to_mtx(camera->unk17C);
            mtxa_from_identity();
            mtxa_rotate_z(-camera->rotZ);
            mtxa_rotate_x(-camera->rotX);
            mtxa_rotate_y(-camera->rotY);
            mtxa_translate_neg(&camera->eye);
            mtxa_mult_right(sp80);
            mtxa_mult_right(sp50);
            mtxa_mult_left(spB0);
            mtxa_to_mtx(camera->unk1AC);
            mtxa_to_mtx(camera->unk1DC);
            camera_apply_viewport(modeCtrl.currPlayer);
            C_MTXPerspective(projMtx, 0.005493164f * camera->sub28.view.fov, camera->sub28.view.aspect, 0.1f, 20000.0f);
            projMtx[0][2] -= projMtx[0][0] * camera->sub28.view.unk28 * camera->sub28.view.aspect * camera->sub28.view.tanHalfFov;
            projMtx[1][2] -= projMtx[1][1] * camera->sub28.view.unk2C * camera->sub28.view.tanHalfFov;
            g_something_with_GXSetProjectionv(projMtx, 0);
            GXSetViewport(0.0f, 0.0f, work->xres, work->yres, 0.0f, 1.0f);
            GXSetScissor_cached(0, 0, work->xres, work->yres);
            mtxa_from_mtxb();
            g_set_active_light_group(5);
            g_some_render_flag.flags |= 4;
            mtxa_from_mtx(work->unk2C);
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

        if (modeCtrl.gameType == 10)
            fn_14_469C();
        else if (events[EVENT_VIEW].state != EV_RUNNING)
        {
            if (!(g_some_render_flag.flags & 0x10))
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
            if (modeCtrl.gameType == 7)
                g_mini_draw_func_handler();
            ord_tbl_draw_nodes();
        }
        else
            draw_reflective_objects_in_viewstage();
        g_some_render_flag.flags &= ~4;
        GXSetZMode_cached(1, 3, 1);
        GXSetTexCopySrc_cached(0, 0, work->xres, work->yres);
        GXSetTexCopyDst_cached(work->xres, work->yres, work->format, 0);
        GXCopyTex(work->unk28, 1);
        GXInitTexObj(&work->unk0, work->unk28, work->xres, work->yres, work->format, 0, 0, 0);
        *camera = cameraBackup;
        camera_apply_viewport(modeCtrl.currPlayer);
        pop_light_group();
    }
}

void fn_1_DCAD0(int unused, struct RenderEffect *rendEfc)
{
    struct RenderEffectWavyMirror *work;
    int stage;
    int coord;
    Mtx mtx;

    work = rendEfc->work;
    g_set_something3(0);
    GXSetViewport(0.0f, 0.0f, 256.0f, 256.0f, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, 0x100, 0x100);
    GXLoadTexObj_cached(init_common_gma->modelEntries[0x61].model->texObjs, 0);
    stage = 0;
    coord = 0;
    GXSetTevKAlphaSel_cached(stage, 0);
    GXSetTexCoordGen2_cached(coord, 0, 1, 0x3C, 1, 0x40);
    mtxa_from_translate_xyz(0.5f, 0.5f, 1.0f);
    mtxa_to_mtx(mtx);
    mtxa_from_mtxb();
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    mtxa_mult_left(mtx);
    mtxa_scale_xyz(1.0f, 0.0f, 1.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x40, 0);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(0, 1, 0, 0);
    GXSetZMode_cached(1, 3, 1);
    GXSetCullMode_cached(2);
    if (g_some_draw_var != NULL)
    {
        struct DynamicStagePart *part;

        mtxa_from_mtxb();
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        nl2ngc_set_scale(1.0f);
        part = g_some_draw_var;
        while (part->modelName != NULL)
        {
            if (part->tempModel != NULL)
                u_nl2ngc_draw_model_with_mesh_func(part->tempModel, fn_1_DBF50);
            part++;
        }
    }
    if (modeCtrl.gameType == 7)
    {
        void *prev = fn_1_10A69C(fn_1_DBF48);

        fn_5_16438();
        fn_1_10A69C(prev);
    }
    else if (modeCtrl.gameType == 10)
    {
        void *prev = fn_1_10A69C(fn_1_DBF48);

        fn_14_13AB0();
        fn_1_10A69C(prev);
    }
    GXSetZMode_cached(1, 3, 1);
    GXSetTexCopySrc_cached(0, 0, 0x100, 0x100);
    GXSetTexCopyDst_cached(0x100, 0x100, 0x2C, 0);
    GXCopyTex(work->unkB4, 1);
    GXInitTexObj(&work->unk94, work->unkB4, 0x100, 0x100, 3, 0, 0, 0);
    GXInitTexObjLOD(&work->unk94, 1, 1, 0.0f, 10.0f, 0.0f, 0, 0, 0);
    fn_1_E2D20();
    camera_apply_viewport(modeCtrl.currPlayer);
}

void fn_1_DCE60(int unused, struct RenderEffect *rendEfc)
{
    struct RenderEffectWavyMirror *work;
    int stage;
    int coord;
    int indStage;
    float temp;
    GXColor color;
    Mtx spA4;
    Mtx sp74;
    Mtx44 projMtx;
    float sp1C[2][3];
    int scale;

    work = rendEfc->work;
    GXSetNumChans_cached(0);
    GXInvalidateTexAll();
    GXLoadTexObj_cached(&work->unk0, 0);
    GXLoadTexObj_cached(&work->unk94, 1);
    if (modeCtrl.gameType == 10)
        GXLoadTexObj_cached(lbl_14_bss_B4[0x10]->texObjs, 2);
    stage = 0;
    coord = 0;
    indStage = 0;
    color.a = work->unk90;
    GXSetTevKColor_cached(0, color);
    GXSetTevKAlphaSel_cached(stage, 0x1C);
    GXSetTexCoordGen2_cached(coord, 0, 0, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2_cached(coord + 1, 0, 0, 0x21, 0, 0x7D);
    C_MTXPerspective(projMtx, 0.005493164f * g_current_camera->sub28.view.fov, g_current_camera->sub28.view.aspect, 0.1f, 20000.0f);
    mtxa_from_identity();
    mtxa->mtxA[0][2] = 0.5 - 0.5 * (projMtx[0][0] * g_current_camera->sub28.view.unk28 * g_current_camera->sub28.view.aspect * g_current_camera->sub28.view.tanHalfFov);
    mtxa->mtxA[1][2] = 0.5 - 0.5 * (projMtx[1][1] * g_current_camera->sub28.view.unk2C * g_current_camera->sub28.view.tanHalfFov);
    temp = 0.5f / math_tan(0.5 * g_current_camera->sub28.view.fov);
    mtxa->mtxA[0][0] = temp / g_current_camera->sub28.view.aspect;
    mtxa->mtxA[1][1] = temp;
    mtxa_to_mtx(sp74);
    mtxa_from_mtxb();
    mtxa_mult_right(work->unk5C);
    mtxa_from_mtx(sp74);
    mtxa_to_mtx(spA4);
    mtxa_from_mtxb();
    mtxa_mult_right(work->unk5C);
    mtxa_mult_left(spA4);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = -0.5f * g_current_camera->sub28.view.invTanHalfFov * (1.0f / g_current_camera->sub28.view.aspect);
    mtxa->mtxA[0][2] = 0.5f + (-0.5f * g_current_camera->sub28.view.unk28);
    mtxa->mtxA[1][1] = 0.5f * g_current_camera->sub28.view.invTanHalfFov;
    mtxa->mtxA[1][2] = 0.5f + (-0.5f * g_current_camera->sub28.view.unk2C);
    mtxa_mult_right(mtxa->mtxB);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 0);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 0);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    sp1C[0][0] = 0.0f;
    sp1C[0][1] = 1.6f;
    sp1C[0][2] = 0.0f;
    sp1C[1][0] = 1.2f;
    sp1C[1][1] = 0.0f;
    sp1C[1][2] = 0.0f;
    scale = 0;
    if (__fabsf(sp1C[0][1]) > __fabsf(sp1C[1][0]))
    {
        while (__fabsf(sp1C[0][1]) >= 1.0)
        {
            sp1C[0][1] *= 0.5f;
            sp1C[1][0] *= 0.5f;
            scale++;
        }
    }
    else
    {
        while (__fabsf(sp1C[1][0]) >= 1.0)
        {
            sp1C[0][1] *= 0.5f;
            sp1C[1][0] *= 0.5f;
            scale++;
        }
    }
    g_GXSetIndTexMtx_wrapper(1, sp1C, scale);
    GXSetIndTexOrder_cached(indStage, coord + 1, 1);
    GXSetTevIndirect_cached(stage, indStage, 0, 7, 1, 0, 0, 0, 0, 0);
    if (modeCtrl.gameType != 10)
    {
        stage += 1;
        coord += 2;
        indStage += 1;
    }
    else
    {
        float scrollMtx[2][3] = {{0.0f, 0.2f, 0.0f}, {0.0f, 0.0f, 0.2f}};

        coord += 2;
        indStage += 1;
        mtxa_push();
        mtxa_from_translate_xyz(0.0f, 0.002f * g_font_char_var_3, 0.0f);
        GXLoadTexMtxImm(mtxa->mtxA, 0x24, 1);
        mtxa_pop();
        GXSetTexCoordGen2_cached(coord, 1, 4, 0x24, 0, 0x7D);
        GXSetIndTexOrder_cached(indStage, coord, 2);
        g_GXSetIndTexMtx_wrapper(2, scrollMtx, 0);
        GXSetTevIndirect_cached(0, indStage, 0, 0, 2, 0, 0, 0, 0, 0);
        stage += 1;
        coord += 1;
        indStage += 1;
    }
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(indStage);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 3, 1);
    fn_1_E2F1C();
    if (g_some_draw_var != NULL)
    {
        struct DynamicStagePart *part;

        mtxa_from_mtxb();
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        part = g_some_draw_var;
        while (part->modelName != NULL)
        {
            if (part->tempModel != NULL)
                u_nl2ngc_draw_model_with_mesh_func(part->tempModel, fn_1_DBF50);
            part++;
        }
    }
    if (modeCtrl.gameType == 7)
    {
        void *prev = fn_1_10A69C(fn_1_DBF48);

        mtxa_from_mtxb();
        mtxa_mult_left(spA4);
        GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
        fn_5_16438();
        fn_1_10A69C(prev);
    }
    else if (modeCtrl.gameType == 10)
    {
        void *prev = fn_1_10A69C(fn_1_DBF48);

        mtxa_from_mtxb();
        mtxa_mult_left(spA4);
        GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
        fn_14_13AB0();
        fn_1_10A69C(prev);
    }
}

/* Stripped function; only its float literals survive in the rodata pool. */
void fn_mirror_stripped(float *out)
{
    out[0] = 1.5f;
    out[1] = 0.0078125f;
}
