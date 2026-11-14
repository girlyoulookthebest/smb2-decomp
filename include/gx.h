#ifndef GX_H
#define GX_H

#include "types.h"
#include "mathutil.h"

/* The Dolphin SDK's GXRenderModeObj. */
typedef struct
{
    /*0x00*/ u32 viTVmode;
    /*0x04*/ u16 fbWidth;
    /*0x06*/ u16 efbHeight;
    /*0x08*/ u16 xfbHeight;
    /*0x0A*/ u16 viXOrigin;
    /*0x0C*/ u16 viYOrigin;
    /*0x0E*/ u16 viWidth;
    /*0x10*/ u16 viHeight;
    /*0x14*/ u32 xFBmode;
    /*0x18*/ u8 field_rendering;
    /*0x19*/ u8 aa;
    /*0x1A*/ u8 sample_pattern[12][2];
    /*0x32*/ u8 vfilter[7];
} GXRenderModeObj;  /* size = 0x3C */

extern GXRenderModeObj ntsc_interlaced_render_mode;
extern GXRenderModeObj ntsc_progressive_render_mode;
extern GXRenderModeObj pal_interlaced_render_mode;
extern GXRenderModeObj mpal_interlaced_render_mode;
extern GXRenderModeObj overscan_adjusted_render_mode;

extern GXRenderModeObj *current_render_mode;

typedef struct
{
    u8 r, g, b, a;
} GXColor;

typedef struct
{
    u32 dummy[16];
} GXLightObj;  /* size = 0x40 */

enum
{
    GX_SP_OFF,
    GX_SP_FLAT,
    GX_SP_COS,
    GX_SP_COS2,
    GX_SP_SHARP,
    GX_SP_RING1,
    GX_SP_RING2,
};

void GXInitLightSpot(GXLightObj *obj, float cutoff, int spotFn);
void GXInitLightDistAttn(GXLightObj *obj, float refDist, float refBrightness, int distFn);
void GXInitLightAttnK(GXLightObj *obj, float k0, float k1, float k2);
void GXInitLightColor(GXLightObj *obj, GXColor color);
void GXInitLightPos(GXLightObj *obj, float x, float y, float z);
void GXInitLightDir(GXLightObj *obj, float nx, float ny, float nz);
void GXLoadLightObjImm(GXLightObj *obj, int lightId);
void GXGetLightPos(GXLightObj *obj, float *x, float *y, float *z);
void GXGetLightColor(GXLightObj *obj, GXColor *color);

void GXSetViewport(float left, float top, float width, float height, float nearZ, float farZ);
void GXSetScissor_cached(u32 left, u32 top, u32 width, u32 height);
void GXSetDispCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetDispCopyDst_cached(u16 width, u16 height);
u32 GXSetDispCopyYScale(float vscale);
void GXSetCopyFilter(u8 aa, u8 samplePattern[12][2], u8 vf, u8 vfilter[7]);
void GXAdjustForOverscan(GXRenderModeObj *rmin, GXRenderModeObj *rmout, u16 hor, u16 ver);
void GXSetBlendMode_cached(int type, int srcFactor, int dstFactor, int op);
void GXSetZMode_cached(u8 compareEnable, int func, u8 updateEnable);
void GXSetZCompLoc_cached(u8 beforeTex);
void GXSetAlphaCompare(int comp0, u8 ref0, int op, int comp1, u8 ref1);
void VIConfigure(GXRenderModeObj *rm);
u32 VIGetTvFormat(void);
void g_something_with_GXSetProjectionv(Mtx44 mtx, int type);
void C_MTXPerspective(Mtx44 m, float fovY, float aspect, float n, float f);


/* GX write-gather FIFO: vertex data goes straight to this address. */
#define GXWGFifo (*(volatile union { u8 u8; s8 s8; u16 u16; s16 s16; u32 u32; s32 s32; float f32; } *)0xCC008000)

#ifndef __MWERKS__
/* PC port: 0xCC008000 is GC hardware; route immediate-mode vertex data to
 * typed shim sinks instead (src/game/shim/gx_fifo.c). */
void pc_gx_pos3f(float x, float y, float z);
void pc_gx_norm3f(float x, float y, float z);
void pc_gx_color4u8(u8 r, u8 g, u8 b, u8 a);
void pc_gx_tex2f(float s, float t);
void pc_gx_pos3s16(s16 x, s16 y, s16 z);
void pc_gx_tex2s16(s16 s, s16 t);
void pc_gx_fifo_s8(s8 v);

static inline void GXPosition3f32(float x, float y, float z) { pc_gx_pos3f(x, y, z); }
static inline void GXNormal3f32(float x, float y, float z) { pc_gx_norm3f(x, y, z); }
static inline void GXColor4u8(u8 r, u8 g, u8 b, u8 a) { pc_gx_color4u8(r, g, b, a); }
static inline void GXTexCoord2f32(float s, float t) { pc_gx_tex2f(s, t); }
#else
static inline void GXPosition3f32(float x, float y, float z)
{
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
}

static inline void GXNormal3f32(float x, float y, float z)
{
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
}

static inline void GXColor4u8(u8 r, u8 g, u8 b, u8 a)
{
    GXWGFifo.u8 = r;
    GXWGFifo.u8 = g;
    GXWGFifo.u8 = b;
    GXWGFifo.u8 = a;
}

static inline void GXTexCoord2f32(float s, float t)
{
    GXWGFifo.f32 = s;
    GXWGFifo.f32 = t;
}
#endif

void GXBegin(int type, int vtxFmt, u16 nverts);
static inline void GXEnd(void) {}

#endif
