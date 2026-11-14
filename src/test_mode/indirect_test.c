#include "types.h"
#include "bitmap.h"
#include "camera.h"
#include "gx.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"

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
u16 GXGetTexObjWidth(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
u32 GXGetTexBufferSize(u16 w, u16 h, int fmt, u8 mip, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *buf, u16 w, u16 h, int fmt, int s, int t, u8 mip);
void GXSetCopyClear_cached(GXColor color, u32 z);
void GXSetTexCopySrc_cached(u16 x, u16 y, u16 w, u16 h);
void GXSetTexCopyDst_cached(u16 w, u16 h, int fmt, u8 mip);
void GXCopyTex(void *dst, u8 clear);
void GXSetViewport(float x, float y, float w, float h, float n, float f);
void GXInvalidateTexAll(void);
void GXLoadPosMtxImm(Mtx m, u32 id);
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
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void GXSetTevIndirect_cached(int stage, int indStage, int fmt, int biasSel, int mtxSel, int wrapS, int wrapT, u8 addPrev, u8 utcLod, int alphaSel);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void GXSetTevSwapMode_cached(int stage, int ras, int tex);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void g_something_with_GX_vtx_desc(int a);
void g_something_with_GXSetProjectionv(Mtx44 m, int type);
void camera_apply_main_projection(void);
void fn_1_E2F1C(void);
void free_nl2ngc_tpl_buf_to_heap(struct TPL *tpl);
void game_main_draw_func(void);
void fn_8_CEB4(void);
void fn_8_CFDC(void);
void fn_8_130F4(void);
void fn_8_13240(void);
void fn_8_132E0(void);
void fn_8_13300(void);
void fn_8_134B0(void);
void fn_8_13AF8(void);
void fn_8_13C7C(void);
void fn_8_13C80(void);
void fn_8_13F6C(void);

struct Particle
{
    Vec pos;
    Vec vel;
    S16Vec rot;
    Vec scale;
};

static struct TPL *tpl;
static struct
{
    GXTexObj indTex;
    void *indBuf;
    GXTexObj copyTex;
    void *copyBuf;
} work;
static struct Particle particles[0x100];

void fn_8_12F1C(void)
{
    u16 width;
    u16 height;
    int fmt;

    if (tpl == NULL)
        tpl = load_bmp("takatest.tpl");
    width = GXGetTexObjWidth(&tpl->texObjs[1]);
    height = GXGetTexObjHeight(&tpl->texObjs[1]);
    fmt = GXGetTexObjFmt(&tpl->texObjs[1]);
    GXInitTexObj(&tpl->texObjs[1], GXGetTexObjData(&tpl->texObjs[1]), width, height, fmt, 2, 2, 0);
    fn_8_130F4();
    fn_8_CEB4();
    camera_set_state(2);
}

void fn_8_1302C(void)
{
    fn_8_13240();
    if (tpl != NULL)
    {
        free_nl2ngc_tpl_buf_to_heap(tpl);
        tpl = NULL;
    }
}

void fn_8_13080(void)
{
    fn_8_132E0();
    fn_8_CFDC();
}

void fn_8_130A4(void)
{
    fn_8_13300();
    game_main_draw_func();
    fn_8_134B0();
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 1, 1);
    fn_1_E2F1C();
}

void fn_8_130F4(void)
{
    fn_8_13AF8();
    if (work.indBuf == NULL)
        work.indBuf = alloc_from_heap_or_panic(currentHeap, GXGetTexBufferSize(0x100, 0x100, 1, 0, 0), "indirect_test.c", 0xF5);
    GXInitTexObj(&work.indTex, work.indBuf, 0x100, 0x100, 1, 0, 0, 0);
    if (work.copyBuf == NULL)
        work.copyBuf = alloc_from_heap_or_panic(currentHeap, GXGetTexBufferSize(0x280, 0x1E0, 4, 0, 0), "indirect_test.c", 0x102);
    GXInitTexObj(&work.copyTex, work.copyBuf, current_render_mode->fbWidth, current_render_mode->efbHeight, 4, 0, 0, 0);
}

void fn_8_13240(void)
{
    if (work.indBuf != NULL)
    {
        OSFreeToHeap(currentHeap, work.indBuf);
        work.indBuf = NULL;
    }
    if (work.copyBuf != NULL)
    {
        OSFreeToHeap(currentHeap, work.copyBuf);
        work.copyBuf = NULL;
    }
    fn_8_13C7C();
}

void fn_8_132E0(void)
{
    fn_8_13C80();
}

void fn_8_13300(void)
{
    static const GXColor clearColor = {0, 0, 0, 0};
    GXColor color = clearColor;
    Mtx44 proj;

    GXSetCopyClear_cached(color, 0xFFFFFF);
    GXSetTexCopySrc_cached(0, 0, 0x20, 0x20);
    GXSetTexCopyDst_cached(0x20, 0x20, 0, 0);
    GXCopyTex(work.indBuf, 1);
    GXSetViewport(0.0f, 0.0f, 256.0f, 256.0f, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, 0x100, 0x100);
    C_MTXPerspective(proj, 59.996338f, 1.3333334f, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(proj, 0);
    fn_8_13F6C();
    GXSetTexCopySrc_cached(0, 0, 0x100, 0x100);
    GXSetTexCopyDst_cached(0x100, 0x100, 0x28, 0);
    GXCopyTex(work.indBuf, 1);
    GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->efbHeight, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
    camera_apply_main_projection();
}

void fn_8_134B0(void)
{
    Mtx44 proj;
    float indMtx1[2][3];
    float indMtx2[2][3];
    u16 width;
    u16 height;
    int stage;
    int ind;
    int coord;
    int mtxId;
    int indMtxId;
    int indCoord;

    C_MTXPerspective(proj, 59.996338f, 1.3333334f, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(proj, 0);
    width = current_render_mode->fbWidth;
    height = current_render_mode->efbHeight;
    GXSetTexCopySrc_cached(0, 0, width, height);
    GXSetTexCopyDst_cached(width, height, 4, 0);
    GXCopyTex(work.copyBuf, 0);
    GXSetNumChans_cached(0);
    GXInvalidateTexAll();
    GXLoadTexObj_cached(&work.indTex, 0);
    GXLoadTexObj_cached(&work.copyTex, 1);
    GXLoadTexObj_cached(&tpl->texObjs[1], 2);
    stage = 0;
    ind = 0;
    coord = 0;
    mtxId = 0x1E;
    indMtxId = 1;
    GXSetTexCoordGen2_cached(coord, 0, 0, mtxId, 0, 0x7D);
    GXSetTexCoordGen2_cached(coord + 1, 0, 0, mtxId, 0, 0x7D);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = -0.64951909f;
    mtxa->mtxA[0][2] = 0.5f;
    mtxa->mtxA[1][1] = 0.86602545f;
    mtxa->mtxA[1][2] = 0.5f - (0.5f / current_render_mode->efbHeight);
    GXLoadTexMtxImm(mtxa->mtxA, mtxId, 0);
    g_GXSetTevOrder_wrapper(stage, coord, 1, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    indCoord = coord + 1;
    GXSetIndTexOrder_cached(ind, indCoord, 0);
    GXSetTevIndirect_cached(stage, ind, 0, 0, indMtxId, 0, 0, 0, 0, 0);
    indMtx1[0][0] = 0.25f;
    indMtx1[0][1] = 0.0f;
    indMtx1[0][2] = 0.0f;
    indMtx1[1][0] = 0.25f;
    indMtx1[1][1] = 0.0f;
    indMtx1[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(indMtxId, indMtx1, 0);
    stage++;
    ind++;
    coord += 2;
    mtxId += 3;
    indMtxId++;
    GXSetTexCoordGen2_cached(coord, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 2, 0xFF);
    GXSetTevColorIn_cached(stage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevIndirect_cached(stage, ind - 1, 0, 0, indMtxId, 0, 0, 0, 0, 0);
    indMtx2[0][0] = 0.5f;
    indMtx2[0][1] = 0.0f;
    indMtx2[0][2] = 0.0f;
    indMtx2[1][0] = -0.5f;
    indMtx2[1][1] = 0.0f;
    indMtx2[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(indMtxId, indMtx2, 0);
    stage++;
    coord++;
    GXSetTevSwapMode_cached(stage, 0, 1);
    g_GXSetTevOrder_wrapper(stage, indCoord, 0, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 4);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(ind);
    GXSetBlendMode_cached(0, 1, 0, 0);
    GXSetZMode_cached(1, 7, 0);
    fn_1_E2F1C();
    g_something_with_GX_vtx_desc(0x2200);
    GXSetVtxAttrFmt(6, 9, 1, 4, 0);
    GXSetVtxAttrFmt(6, 0xD, 1, 4, 0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0x80, 6, 4);
    GXPosition3f32(-0.76980036f, 0.57735026f, -1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.76980036f, 0.57735026f, -1.0f);
    GXTexCoord2f32(10.0f, 0.0f);
    GXPosition3f32(0.76980036f, -0.57735026f, -1.0f);
    GXTexCoord2f32(10.0f, 10.0f);
    GXPosition3f32(-0.76980036f, -0.57735026f, -1.0f);
    GXTexCoord2f32(0.0f, 10.0f);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetNumIndStages_cached(0);
    camera_apply_main_projection();
}

void fn_8_13AF8(void)
{
    int i;
    struct Particle *p = particles;

    for (i = 0x100; i > 0; i--, p++)
    {
        p->pos.x = 640.0f * (rand() / 32768.0f - 0.5f);
        p->pos.y = 480.0f * (rand() / 32768.0f - 0.5f);
        p->vel.x = 0.0f;
        p->vel.y = 0.0f;
        p->rot.z = rand();
        p->scale.x = 16.0f * (0.5f + 2.0f * (rand() / 32768.0f));
        p->scale.y = 16.0f * (0.5f + 2.0f * (rand() / 32768.0f));
        p->scale.z = 1.0f;
    }
}

void fn_8_13C7C(void)
{
}

void fn_8_13C80(void)
{
    int i;
    struct Particle *p = particles;

    for (i = 0x100; i > 0; i--, p++)
    {
        p->vel.x *= 0.995f;
        p->vel.y *= 0.995f;
        p->vel.y += -0.016000001f;
        p->pos.x += p->vel.x;
        p->pos.y += p->vel.y;
        p->rot.z += 0x60;
        if (p->pos.y < -240.0f - p->scale.y)
        {
            if ((pad_status_groups->unk0[0].button & 8) == 0)
            {
                p->pos.x = 640.0f * (rand() / 32768.0f - 0.5f);
                p->pos.y = 480.0f * (0.5f * (rand() / 32768.0f));
                p->vel.x = 0.1f * (rand() / 32768.0f - 0.5f);
                p->vel.y = 0.0f;
            }
            else
            {
                p->pos.x = 300.0f * math_sin(frame_counter << 5);
                p->pos.y = 240.0f;
                p->vel.x = 4.0f * (rand() / 32768.0f - 0.5f);
                p->vel.y = -4.0f * (rand() / 32768.0f) - 4.0f;
            }
            p->rot.z = rand();
        p->scale.x = 16.0f * (0.5f + 2.0f * (rand() / 32768.0f));
        p->scale.y = 16.0f * (0.5f + 2.0f * (rand() / 32768.0f));
        p->scale.z = 1.0f;
        }
    }
}

void fn_8_13F6C(void)
{
    GXColor color;
    int i;
    struct Particle *p;
    int stage;
    int ind;
    int coord;

    color.r = 0x80;
    color.g = 0x80;
    color.b = 0x80;
    color.a = 0xFF;
    GXSetChanMatColor(4, color);
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 0xFF;
    GXSetChanAmbColor(4, color);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 1);
    GXSetNumChans_cached(1);
    GXLoadTexObj_cached(&tpl->texObjs[0], 0);
    stage = 0;
    ind = 0;
    coord = 0;
    GXSetTexCoordGen2_cached(coord, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(stage, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    stage++;
    coord++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(ind);
    GXSetBlendMode_cached(1, 1, 1, 0);
    GXSetZMode_cached(1, 7, 0);
    fn_1_E2F1C();
    g_something_with_GX_vtx_desc(0x2200);
    GXSetVtxAttrFmt(6, 9, 1, 4, 0);
    GXSetVtxAttrFmt(6, 0xD, 1, 1, 0);
    p = particles;
    for (i = 0x100; i > 0; i--, p++)
    {
        mtxa_from_identity();
        mtxa_translate_xyz(p->pos.x, p->pos.y, -415.6922f);
        mtxa_scale(&p->scale);
        mtxa_rotate_z(p->rot.z);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXBegin(0x80, 6, 4);
        GXPosition3f32(-1.0f, 1.0f, 0.0f);
        GXWGFifo.s8 = 0;
        GXWGFifo.s8 = 0;
        GXPosition3f32(1.0f, 1.0f, 0.0f);
        GXWGFifo.s8 = 1;
        GXWGFifo.s8 = 0;
        GXPosition3f32(1.0f, -1.0f, 0.0f);
        GXWGFifo.s8 = 1;
        GXWGFifo.s8 = 1;
        GXPosition3f32(-1.0f, -1.0f, 0.0f);
        GXWGFifo.s8 = 0;
        GXWGFifo.s8 = 1;
    }
}
