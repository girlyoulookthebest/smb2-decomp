#include "types.h"
#include "bitmap.h"
#include "camera.h"
#include "gx.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "stage.h"

extern int currentHeap;
extern s16 g_some_stage_id;

void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void *alloc_from_heap_or_panic(int heap, u32 size, char *file, int line);
void OSFreeToHeap(int heap, void *ptr);
u32 GXGetTexBufferSize(u16 w, u16 h, int fmt, u8 mip, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *buf, u16 w, u16 h, int fmt, int s, int t, u8 mip);
void GXSetTexCopySrc_cached(u16 x, u16 y, u16 w, u16 h);
void GXSetTexCopyDst_cached(u16 w, u16 h, int fmt, u8 mip);
void GXCopyTex(void *dst, u8 clear);
void GXPixModeSync(void);
void GXLoadPosMtxImm(Mtx m, u32 id);
void GXLoadTexMtxImm(Mtx m, u32 id, int type);
void GXSetVtxAttrFmt(int fmt, int attr, int cnt, int type, u8 frac);
void GXSetNumChans_cached(u8 n);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void g_something_with_raster_state(int a);
void g_something_with_GX_vtx_desc(int a);
void camera_setup_default_projection(void);
void PSMTXScale(Mtx m, float x, float y, float z);
void dest_all_events(void);
void fn_1_56C18(void);
void event_init(int id);
void load_stage(int id);
void game_main_draw_func(void);
void fn_8_CEB4(void);
void fn_8_CFDC(void);
void fn_8_12E64(void);
void fn_8_12EF8(void);
void fn_8_12EFC(void);
void fn_8_125E8(void);

static u32 data_0[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

static struct
{
    float nearDist;
    float farDist;
    s32 maskNum;
    float blur;
    GXTexObj texObj;
    u32 bufSize;
    void *buf;
} focusWork;

void fn_8_1226C(void)
{
    fn_8_12E64();
    focusWork.nearDist = 10.0f;
    focusWork.farDist = 100.0f;
    focusWork.maskNum = 2;
    focusWork.blur = 5.0f;
    if (focusWork.buf == NULL)
    {
        focusWork.bufSize = GXGetTexBufferSize(0x280, 0x1E0, 4, 0, 0);
        focusWork.buf = alloc_from_heap_or_panic(currentHeap, focusWork.bufSize, "focus_test.c", 0x74);
    }
    GXInitTexObj(&focusWork.texObj, focusWork.buf, current_render_mode->fbWidth, current_render_mode->efbHeight, 4, 0, 0, 0);
    fn_8_CEB4();
    enable_camera(0);
    camera_set_state(2);
}

void fn_8_123A8(void)
{
    if (focusWork.buf != NULL)
    {
        OSFreeToHeap(currentHeap, focusWork.buf);
        focusWork.buf = NULL;
    }
}

void fn_8_12404(void)
{
    Vec pos;

    fn_8_12EF8();
    if (pad_status_groups->unk0[2].button & 8)
        focusWork.maskNum++;
    if (focusWork.maskNum > 0 && (pad_status_groups->unk0[2].button & 4))
        focusWork.maskNum--;
    if (pad_status_groups->unk0[0].button & 2)
        focusWork.blur -= 0.01f;
    if (pad_status_groups->unk0[0].button & 1)
        focusWork.blur += 0.01f;
    mtxa_from_mtxb();
    mtxa_tf_point(&g_current_camera->lookAt, &pos);
    focusWork.nearDist = -pos.z;
    fn_8_CFDC();
}

void fn_8_12534(void)
{
    debug_textdraw_set_pos(8, 8);
    debug_textdraw_vsprintf_buf_1("FOCUS TEST\n");
    debug_textdraw_vsprintf_buf_1("MASK NUM:%d\n", focusWork.maskNum);
    debug_textdraw_vsprintf_buf_1("BLUR    :%7.3f\n", focusWork.blur);
    fn_8_12EFC();
    camera_apply_viewport_if_changed(0);
    fn_8_125E8();
    camera_setup_default_projection();
    GXSetBlendMode_cached(0, 1, 0, 0);
    GXSetZMode_cached(1, 1, 1);
    g_something_with_raster_state(0);
}

void fn_8_125E8(void)
{
    Mtx texMtx;
    float z;
    float step;
    float y;
    float x;
    float blurAmt;
    float nearDist;
    int i;
    int stage;
    int coord;
    int mtxId;
    u16 width = current_render_mode->fbWidth;
    u16 height = current_render_mode->efbHeight;

    GXSetTexCopySrc_cached(0, 0, width, height);
    GXSetTexCopyDst_cached(width, height, 4, 0);
    GXCopyTex(focusWork.buf, 0);
    GXPixModeSync();
    GXSetNumChans_cached(0);
    GXLoadTexObj_cached(&focusWork.texObj, 0);
    stage = 0;
    coord = 0;
    mtxId = 0x1E;
    GXSetTexCoordGen2_cached(coord, 1, 4, mtxId, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 0xF, 8, 0xE, 0xF);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    mtxId += 3;
    GXSetTexCoordGen2_cached(coord, 1, 4, mtxId, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    mtxId += 3;
    GXSetTexCoordGen2_cached(coord, 1, 4, mtxId, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    mtxId += 3;
    GXSetTexCoordGen2_cached(coord, 1, 4, mtxId, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevKAlphaSel_cached(stage, 4);
    GXSetTevColorIn_cached(stage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 3, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    g_something_with_raster_state(0);
    g_something_with_GX_vtx_desc(0x2200);
    GXSetVtxAttrFmt(1, 9, 1, 4, 0);
    GXSetVtxAttrFmt(1, 0xD, 1, 4, 0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    if (focusWork.maskNum != 0)
    {
        PSMTXScale(texMtx, 1.0f, 1.0f, 1.0f);
        nearDist = -focusWork.nearDist;
        blurAmt = focusWork.blur;
        GXSetZMode_cached(1, 4, 0);
        step = nearDist / (focusWork.maskNum + 2);
        z = step;
        for (i = focusWork.maskNum; i > 0;)
        {
            float blur = -(blurAmt * (z - nearDist)) / z;

            x = 0.0015625f * blur;
            y = 0.0020833334f * blur;
            texMtx[0][3] = x;
            texMtx[1][3] = 0.0f;
            GXLoadTexMtxImm(texMtx, 0x1E, 1);
            texMtx[0][3] = -x;
            GXLoadTexMtxImm(texMtx, 0x21, 1);
            texMtx[0][3] = 0.0f;
            texMtx[1][3] = y;
            GXLoadTexMtxImm(texMtx, 0x24, 1);
            texMtx[1][3] = -y;
            GXLoadTexMtxImm(texMtx, 0x27, 1);
            y = g_current_camera->sub28.view.aspect * (z * -g_current_camera->sub28.view.tanHalfFov);
            x = z * -g_current_camera->sub28.view.tanHalfFov;
            GXBegin(0x80, 1, 4);
            GXPosition3f32(-y, x, z);
            GXTexCoord2f32(0.0f, 0.0f);
            GXPosition3f32(y, x, z);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3f32(y, -x, z);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(-y, -x, z);
            GXTexCoord2f32(0.0f, 1.0f);
            i--;
            z += step;
        }
        GXSetZMode_cached(1, 1, 0);
        step = -focusWork.farDist / (focusWork.maskNum + 1);
        z = nearDist - focusWork.farDist;
        for (i = focusWork.maskNum; i > 0;)
        {
            float blur = -(blurAmt * (nearDist - z)) / z;

            x = 0.0015625f * blur;
            y = 0.0020833334f * blur;
            texMtx[0][3] = x;
            texMtx[1][3] = 0.0f;
            GXLoadTexMtxImm(texMtx, 0x1E, 1);
            texMtx[0][3] = -x;
            GXLoadTexMtxImm(texMtx, 0x21, 1);
            texMtx[0][3] = 0.0f;
            texMtx[1][3] = y;
            GXLoadTexMtxImm(texMtx, 0x24, 1);
            texMtx[1][3] = -y;
            GXLoadTexMtxImm(texMtx, 0x27, 1);
            y = g_current_camera->sub28.view.aspect * (z * -g_current_camera->sub28.view.tanHalfFov);
            x = z * -g_current_camera->sub28.view.tanHalfFov;
            GXBegin(0x80, 1, 4);
            GXPosition3f32(-y, x, z);
            GXTexCoord2f32(0.0f, 0.0f);
            GXPosition3f32(y, x, z);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3f32(y, -x, z);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(-y, -x, z);
            GXTexCoord2f32(0.0f, 1.0f);
            i--;
            z -= step;
        }
    }
}

void fn_8_12E64(void)
{
    dest_all_events();
    fn_1_56C18();
    load_stage(g_some_stage_id);
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(4);
    event_init(9);
    event_init(5);
    event_init(7);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x14);
}

void fn_8_12EF8(void)
{
}

void fn_8_12EFC(void)
{
    game_main_draw_func();
}
