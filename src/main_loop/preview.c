/* Streams single preview images (stage and ranking backdrops) from disc into
 * a texture, draws them as a quad, and with DIP_PREVIEW_SAVE downsamples and
 * dumps the frame buffer to preview/work/. */
#pragma peephole off
#include "types.h"
#include "dip.h"
#include "bitmap.h"
#include "gx.h"
#include "input.h"
#include "mathutil.h"
#include "os.h"

struct PreviewFileInfo
{
    /*0x00*/ struct
    {
        /*0x00*/ u8 filler0[0x2C];
        /*0x2C*/ void *userData;
    } cb;
    /*0x30*/ u32 startAddr;
    /*0x34*/ u32 length;
    /*0x38*/ u32 unk38;
}; /* size = 0x3C, DVDFileInfo */

struct Preview
{
    /*0x00*/ s8 state;
    /*0x01*/ s8 isLoading;
    /*0x02*/ u16 flags;
    /*0x04*/ int index;
    /*0x08*/ GXTexObj texObj;
    /*0x28*/ void *imagePtr;
    /*0x2C*/ u32 size;
    /*0x30*/ u16 width;
    /*0x32*/ u16 height;
    /*0x34*/ int format;
    /*0x38*/ void *heap;
    /*0x3C*/ struct PreviewFileInfo fileInfo;
}; /* size = 0x78 */

extern void *currentHeap;
extern void *lbl_801988E0;

void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void OSFreeToHeap(void *heap, void *ptr);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInvalidateTexAll(void);
void GXLoadTexObj_cached(GXTexObj *obj, int id);
s32 DVDOpen(char *path, struct PreviewFileInfo *fileInfo);
s32 DVDClose(struct PreviewFileInfo *fileInfo);
s32 DVDReadAsyncPrio(struct PreviewFileInfo *fileInfo, void *addr, s32 length, s32 offset,
                     void (*callback)(s32, struct PreviewFileInfo *), s32 prio);
void call_dvd_cancel_async_and_wait_for_completion(struct PreviewFileInfo *fileInfo);
void g_idle_function(int a);
void fn_800668FC(void);
void g_something_with_GX_vtx_desc(u32 attrs);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetTevSwapMode_cached(int stage, int ras, int tex);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void opti_GXSetChanCtrl(int chan, int enable, int ambSrc, int matSrc, int lightMask, int diffFn, int attnFn);
void g_something_with_raster_state(int a);
void GXSetCullMode_cached(int mode);
void load_gx_pos_nrm_mtx(Mtx m, int id);
void C_MTXOrtho(Mtx44 m, float t, float b, float l, float r, float n, float f);
void g_something_with_GXSetProjectionv(Mtx44 m, int type);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXSetTevColor(int id, GXColor color);
void DCFlushRange(void *addr, u32 size);
int sprintf(char *buf, char *fmt, ...);
int printf(char *fmt, ...);
void *fn_8006F330(char *path, char *mode);  /* fopen */
int fclose(void *fp);
u32 fwrite(void *buf, u32 size, u32 count, void *fp);

void g_load_sprite_tex_from_dvd(struct Preview *preview);
void fn_1_F9B54(struct Preview *preview, void *image, char *path, int index, u16 width, u16 height, int format);
void fn_1_F9CAC(struct Preview *preview, int index);

u8 lbl_1_data_62CD0[] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

static int lbl_1_bss_D2C30;

static inline void init_preview(struct Preview *preview, char *path, int index, u16 width, u16 height, int format)
{
    preview->state = 0;
    preview->width = (u16)width;
    preview->height = (u16)height;
    preview->format = format;
    preview->size = GXGetTexBufferSize((u16)width, (u16)height, format, 0, 0);
    if (!(preview->flags & 1))
    {
        preview->heap = currentHeap;
        preview->imagePtr = alloc_from_heap_or_panic(preview->heap, preview->size, "preview.c", 0x26);
    }
    preview->isLoading = 0;
    GXInitTexObj(&preview->texObj, preview->imagePtr, preview->width, preview->height, preview->format, 0, 0, 0);
    if (DVDOpen(path, &preview->fileInfo) == 0)
        OSPanic("preview.c", 0x2D, "cannot Open");
    preview->fileInfo.cb.userData = preview;
    fn_1_F9CAC(preview, index);
    g_load_sprite_tex_from_dvd(preview);
}

void g_load_preview_texture(struct Preview *preview, char *path, int index, u16 width, u16 height, int format)
{
    preview->flags = 0;
    init_preview(preview, path, index, width, height, format);
}

void fn_1_F9B04(struct Preview *preview, char *path, int index, u16 width, u16 height, int format)
{
    fn_1_F9B54(preview, lbl_801988E0, path, index, width, height, format);
}

void fn_1_F9B54(struct Preview *preview, void *image, char *path, int index, u16 width, u16 height, int format)
{
    preview->flags = 1;
    preview->imagePtr = image;
    init_preview(preview, path, index, width, height, format);
}

void g_some_dvd_callback2(s32 result, struct PreviewFileInfo *fileInfo)
{
    struct Preview *preview = fileInfo->cb.userData;

    if (result < 0)
        preview->isLoading = result;
    else
        preview->isLoading = 0;
}

void fn_1_F9CAC(struct Preview *preview, int index)
{
    preview->state = 1;
    preview->index = index;
}

void fn_1_F9CBC(struct Preview *preview)
{
    while (preview->isLoading != 0)
        g_idle_function(1);
    g_load_sprite_tex_from_dvd(preview);
}

void g_load_sprite_tex_from_dvd(struct Preview *preview)
{
    switch (preview->state)
    {
    case 1:
        if (preview->isLoading <= 0)
        {
            preview->isLoading = 1;
            preview->state = 5;
            if (DVDReadAsyncPrio(&preview->fileInfo, preview->imagePtr, preview->size, preview->size * preview->index,
                                 g_some_dvd_callback2, 2) != 1)
                OSPanic("preview.c", 0x7E, "cannot DVDReadAsync");
        }
        break;
    case 5:
        if (preview->isLoading <= 0)
        {
            if (preview->isLoading < 0)
                preview->state = 0;
            else
            {
                GXInitTexObj(&preview->texObj, preview->imagePtr, preview->width, preview->height, preview->format, 0, 0, 0);
                GXInvalidateTexAll();
                preview->state = 3;
            }
        }
        break;
    }
}

void fn_1_F9E1C(struct Preview *preview)
{
    if (preview->isLoading > 0)
        call_dvd_cancel_async_and_wait_for_completion(&preview->fileInfo);
    if (DVDClose(&preview->fileInfo) != 1)
        OSPanic("preview.c", 0x93, "cannot Close");
    if (!(preview->flags & 1))
        OSFreeToHeap(preview->heap, preview->imagePtr);
    preview->imagePtr = NULL;
}

void fn_1_F9EAC(struct Preview *preview, float x, float y, float z, float width, float height, u32 color0, u32 color1)
{
    GXColor color;
    float x1;
    float x2;
    float y2;
    float y1;

    x1 = x;
    y1 = y;

    if ((int)preview->state == 3)
    {
        GXLoadTexObj_cached(&preview->texObj, 0);
        color.r = (color0 >> 16) & 0xFF;
        color.g = (color0 >> 8) & 0xFF;
        color.b = color0 & 0xFF;
        color.a = 255;
        GXSetTevColor(1, color);
        color.r = (color1 >> 16) & 0xFF;
        color.g = (color1 >> 8) & 0xFF;
        color.b = color1 & 0xFF;
        color.a = 0;
        GXSetTevColor(2, color);
        x2 = x1 + width;
        y2 = y1 + height;
        if (preview->flags & 2)
        {
            float scale = 0.002405626 * z;

            x1 = (x1 - 320.0) * scale;
            y1 = -((y1 - 240.0) * scale);
            x2 = (x2 - 320.0) * scale;
            y2 = -((y2 - 240.0) * scale);
        }
        GXBegin(0x80, 7, 4);
        GXPosition3f32(x1, y1, -z);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x2, y1, -z);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x2, y2, -z);
        GXTexCoord2f32(1.0f, 1.0 - (1.0 / preview->height));
        GXPosition3f32(x1, y2, -z);
        GXTexCoord2f32(0.0f, 1.0 - (1.0 / preview->height));
    }
}

void g_maybe_save_previews(void)
{
    Mtx44 proj;
    GXTexObj texObj;
    char path[256];
    int width;
    int height;
    u8 *buf;
    void *fp;

    if (!(dip_switches & DIP_PREVIEW_SAVE))
        return;
    buf = lbl_801988E0;
    fn_800668FC();
    g_something_with_GX_vtx_desc(1 << 13 | 1 << 9);
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    g_GXSetTevIndirect_zero_if_different(0);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 0);
    GXSetTexCoordGen2_cached(0, 1, 4, 60, 0, 125);
    g_GXSetTevOrder_wrapper(0, 0, 0, 255);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTevColorIn_cached(0, 8, 15, 15, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 4, 7, 7, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetBlendMode_cached(1, 4, 5, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(2);
    mtxa_from_identity();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    C_MTXOrtho(proj, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, 20000.0f);
    g_something_with_GXSetProjectionv(proj, 1);
    GXSetZMode_cached(1, 7, 0);
    width = current_render_mode->fbWidth;
    height = current_render_mode->efbHeight;
    GXSetTexCopySrc_cached(0, 0, width, height);
    GXSetTexCopyDst_cached(width >> 1, height >> 1, 5, 1);
    GXCopyTex(buf, 0);
    buf += GXGetTexBufferSize(width >> 1, height >> 1, 5, 0, 0);
    GXInitTexObj(&texObj, lbl_801988E0, width >> 1, height >> 1, 5, 0, 0, 0);
    GXLoadTexObj_cached(&texObj, 0);
    GXInvalidateTexAll();
    GXBegin(0x80, 7, 4);
    GXPosition3f32(0.0f, 0.0f, -1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(320.0f, 0.0f, -1.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(320.0f, 240.0f, -1.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.0f, 240.0f, -1.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXSetTexCopySrc_cached(0, 0, width >> 1, height >> 1);
    GXSetTexCopyDst_cached(width >> 2, height >> 2, 5, 1);
    GXCopyTex(buf, 0);
    GXSetZMode_cached(1, 3, 1);
    if ((analog_inputs[0][0] & 0x100) && (pad_status_groups[0].unk0[2].button & 0x400))
    {
        sprintf(path, "preview/work/%dx%d_%02d.gct", width >> 2, height >> 2, lbl_1_bss_D2C30);
        fp = fn_8006F330(path, "wb");
        if (fp != NULL)
        {
            u32 size = GXGetTexBufferSize(width >> 2, height >> 2, 5, 0, 0);

            DCFlushRange(buf, size);
            fwrite(buf, size, 1, fp);
            fclose(fp);
            lbl_1_bss_D2C30++;
        }
        else
            printf("cannot open file: %s\n", path);
    }
}
