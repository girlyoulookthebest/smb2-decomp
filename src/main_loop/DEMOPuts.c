/* Dolphin SDK DEMOPuts.c: debug bitmap font and ROM font text drawing.
 * fn_1_F0AEC is DEMOInitCaption, g_something_with_fonts is DEMOInitROMFont,
 * fn_1_F0E4C is DEMOSetROMFontSize, fn_1_F0E80 is DEMORFPuts and
 * g_some_printf_function_6 is DEMORFPrintf. */

#include "types.h"
#include "bitmap.h"
#include "gx.h"
#include "os.h"
#include "va.h"

typedef struct OSFontHeader
{
    /*0x00*/ u16 fontType;
    /*0x02*/ u16 firstChar;
    /*0x04*/ u16 lastChar;
    /*0x06*/ u16 invalChar;
    /*0x08*/ u16 ascent;
    /*0x0A*/ u16 descent;
    /*0x0C*/ u16 width;
    /*0x0E*/ u16 leading;
    /*0x10*/ u16 cellWidth;
    /*0x12*/ u16 cellHeight;
    /*0x14*/ u32 sheetSize;
    /*0x18*/ u16 sheetFormat;
    /*0x1A*/ u16 sheetColumn;
    /*0x1C*/ u16 sheetRow;
    /*0x1E*/ u16 sheetWidth;
    /*0x20*/ u16 sheetHeight;
    /*0x22*/ u16 widthTable;
    /*0x24*/ u32 sheetImage;
    /*0x28*/ u32 sheetFullSize;
    /*0x2C*/ u8 c0;
    /*0x2D*/ u8 c1;
    /*0x2E*/ u8 c2;
    /*0x2F*/ u8 c3;
} OSFontHeader;  /* size = 0x30 */

extern u32 DEMOFontBitmap[];

u16 OSGetFontEncode(void);
int OSInitFont(OSFontHeader *fontData);
u8 *fn_8000F954(u8 *string, void **image, s32 *x, s32 *y, s32 *width);
void *fn_1_15F0(u32 size, int align, const char *file, int line);
void C_MTXOrtho(Mtx44 m, float t, float b, float l, float r, float n, float f);
void PSMTXIdentity(Mtx m);
void PSMTXScale(Mtx m, float x, float y, float z);
void GXLoadPosMtxImm(Mtx m, int id);
void GXSetCurrentMtx(int id);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int attr, int type);
void GXSetVtxAttrFmt(int fmt, int attr, int cnt, int type, u8 frac);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp,
                     u8 doEdgeLod, int maxAniso);
void GXSetNumChans_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);

static s32 fontShift;
static GXTexObj fontTexObj;
OSFontHeader *lbl_1_bss_D2B7C;
static s16 FontSize;
static s16 FontSpace;
static void *LastSheet;

void DEMOResetFontShift(void)
{
    fontShift = 0;
}

void DEMOLoadFont(int texMap, int texMtx, int texFlt)
{
    Mtx fontTMtx;
    u16 width = 64;
    u16 height = 0x1800 / width;

    GXInitTexObj(&fontTexObj, DEMOFontBitmap, width, height, 0, 0, 0, 0);
    GXInitTexObjLOD(&fontTexObj, texFlt, texFlt, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    fontShift = 0;
    GXLoadTexObj_cached(&fontTexObj, texMap);
    PSMTXScale(fontTMtx, 1.0f / width, 1.0f / height, 1.0f);
    GXLoadTexMtxImm(fontTMtx, texMtx, 1);
    GXSetNumTexGens_cached(1);
    GXSetTexCoordGen2_cached(0, 1, 4, texMtx, 0, 0x7D);
}

void DEMOSetupScrnSpc(s32 width, s32 height, float depth)
{
    Mtx44 pMtx;
    Mtx mMtx;

    C_MTXOrtho(pMtx, 0.0f, height, 0.0f, width, 0.0f, -depth);
    g_something_with_GXSetProjectionv(pMtx, 1);
    PSMTXIdentity(mMtx);
    GXLoadPosMtxImm(mMtx, 0);
    GXSetCurrentMtx(0);
}

void fn_1_F0AEC(s32 fontType, s32 width, s32 height)
{
    DEMOSetupScrnSpc(width, height, 100.0f);
    GXSetZMode_cached(1, 7, 1);
    GXSetNumChans_cached(0);
    GXSetNumTevStages_cached(1);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 3);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    DEMOLoadFont(0, 0x1E, 0);
    switch (fontType)
    {
    case 1:
        GXSetBlendMode_cached(2, 0, 0, 0xC);
        break;
    case 2:
        GXSetBlendMode_cached(1, 1, 1, 0);
        break;
    case 0:
    default:
        GXSetBlendMode_cached(1, 1, 0, 0);
        break;
    }
}

OSFontHeader *g_something_with_fonts(void)
{
    if (OSGetFontEncode() == 1)
        lbl_1_bss_D2B7C = fn_1_15F0(0x120F00, 0x20, "DEMOPuts.c", 412);
    else
        lbl_1_bss_D2B7C = fn_1_15F0(0x20120, 0x20, "DEMOPuts.c", 416);
    if (lbl_1_bss_D2B7C == NULL)
        OSPanic("DEMOPuts.c", 420, "Ins. memory to load ROM font.");
    if (!OSInitFont(lbl_1_bss_D2B7C))
        OSPanic("DEMOPuts.c", 424, "ROM font is available in boot ROM ver 0.8 or later.");
    FontSize = lbl_1_bss_D2B7C->cellWidth * 16;
    FontSpace = -16;
    return lbl_1_bss_D2B7C;
}

void fn_1_F0E4C(s16 size, s16 space)
{
    FontSize = size * 16;
    FontSpace = space * 16;
}

static inline void LoadSheet(void *image, int texMapID)
{
    Mtx mtx;
    GXTexObj texObj;

    if (LastSheet == image)
        return;
    LastSheet = image;
    GXInitTexObj(&texObj, image, lbl_1_bss_D2B7C->sheetWidth, lbl_1_bss_D2B7C->sheetHeight, lbl_1_bss_D2B7C->sheetFormat, 0, 0, 0);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj_cached(&texObj, texMapID);
    PSMTXScale(mtx, 1.0f / lbl_1_bss_D2B7C->sheetWidth, 1.0f / lbl_1_bss_D2B7C->sheetHeight, 1.0f);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXSetNumTexGens_cached(1);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x1E, 0, 0x7D);
}

static inline void GXPosition3s16(const s16 x, const s16 y, const s16 z)
{
#ifndef __MWERKS__
    pc_gx_pos3s16(x, y, z);
#else
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = z;
#endif
}

static inline void GXTexCoord2s16(const s16 s, const s16 t)
{
#ifndef __MWERKS__
    pc_gx_tex2s16(s, t);
#else
    GXWGFifo.s16 = s;
    GXWGFifo.s16 = t;
#endif
}

static inline void DrawFontChar(int x, int y, int z, int xChar, int yChar)
{
    s16 posLeft = x;
    s16 posRight = posLeft + FontSize;
    s16 posTop = y - lbl_1_bss_D2B7C->ascent * FontSize / lbl_1_bss_D2B7C->cellWidth;
    s16 posBottom = y + lbl_1_bss_D2B7C->descent * FontSize / lbl_1_bss_D2B7C->cellWidth;
    s16 texLeft = xChar;
    s16 texRight = xChar + lbl_1_bss_D2B7C->cellWidth;
    s16 texTop = yChar;
    s16 texBottom = yChar + lbl_1_bss_D2B7C->cellHeight;

    GXBegin(0x80, 0, 4);
    GXPosition3s16(posLeft, posTop, z);
    GXTexCoord2s16(texLeft, texTop);
    GXPosition3s16(posRight, posTop, z);
    GXTexCoord2s16(texRight, texTop);
    GXPosition3s16(posRight, posBottom, z);
    GXTexCoord2s16(texRight, texBottom);
    GXPosition3s16(posLeft, posBottom, z);
    GXTexCoord2s16(texLeft, texBottom);
}

int fn_1_F0E80(s16 x, s16 y, s16 z, u8 *string)
{
    s32 cx;
    void *image;
    s32 xChar;
    s32 yChar;
    s32 width;

    LastSheet = NULL;
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 3, 4);
    GXSetVtxAttrFmt(0, 0xD, 1, 3, 0);
    x *= 16;
    y *= 16;
    z *= 16;
    width = 0;
    while (*string != 0)
    {
        if (*string == '\n')
        {
            width = 0;
            y += lbl_1_bss_D2B7C->leading * FontSize / lbl_1_bss_D2B7C->cellWidth;
            string++;
            continue;
        }
        string = fn_8000F954(string, &image, &xChar, &yChar, &cx);
        LoadSheet(image, 0);
        DrawFontChar((s16)x + width, y, z, xChar, yChar);
        width += FontSize * cx / lbl_1_bss_D2B7C->cellWidth + FontSpace;
    }
    return (width + 15) / 16;
}

int g_some_printf_function_6(s16 x, s16 y, s16 z, char *fmt, ...)
{
    va_list args;
    char buf[0x100];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    return fn_1_F0E80(x, y, z, (u8 *)buf);
}

void DEMODumpROMFont(u32 *image, s32 width)
{
    OSReport("%08x%08x%08x%08x%08x%08x\n", image[0], image[1], image[2], image[3], image[4], image[5]);
    OSReport("\nwidth %d\n", width);
}
