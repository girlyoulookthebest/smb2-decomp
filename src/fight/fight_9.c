/* Monkey Fight 2: player HUD panels (face, KO points, win bar, handicap gauge), the countdown,
 * round banners and timer. */
#include "fight.h"

/* DOL / main_loop */
extern s32 active_monkey_id[];
extern u32 g_font_char_var_3;
extern Vec lbl_1_data_2E418[10];  /* ball colours (ball.c) */
void call_SoundReqID_arg_0(int soundId);
void set_ui_widescreen_scale_mtx(int width);
void reset_ui_widescreen_scale_mtx(void);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
void textdraw_put_char(u8 c);
void textdraw_print(char *str);
void textdraw_printf(char *fmt, ...);
void textdraw_set_spacing(float x, float y);
void textdraw_set_depth(float depth);
void textdraw_set_alignment(s8 alignment);
extern void *lbl_801988E0;  /* EFB copy buffer */
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInvalidateTexAll(void);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevColor(int reg, GXColor color);
void GXSetCullMode_cached(int mode);
void g_something_with_GX_vtx_desc(u32 attrs);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumChans_cached(u8 n);

/* ---- fight_9 data (owner: w10) ---- */

/* One star of the win bar (lbl_12_bss_1EED8[i].wins). */
struct FightHudWin
{
    /*0x00*/ float scale;
    /*0x04*/ float scaleVel;
    /*0x08*/ float scaleTarget;
    /*0x0C*/ float rot;
    /*0x10*/ float rotVel;
    /*0x14*/ float glow;
    /*0x18*/ float glowTarget;
    /*0x1C*/ u8 unk1C[4];
};  /* size 0x20 */

/* One segment of the handicap gauge: mul/add colours. */
struct FightHudGauge
{
    /*0x00*/ Vec mul;
    /*0x0C*/ Vec add;
};  /* size 0x18 */

/* HUD panel of one player (lbl_12_bss_1EED8[playerId]). */
struct FightHud
{
    /*0x000*/ float baseX;
    /*0x004*/ float baseY;
    /*0x008*/ float x;
    /*0x00C*/ float y;
    /*0x010*/ float velX;
    /*0x014*/ float velY;
    /*0x018*/ Mtx mtx;          /* panel sway rotation */
    /*0x048*/ float angle;
    /*0x04C*/ float angleVel;
    /*0x050*/ Vec color;        /* player colour * 255 */
    /*0x05C*/ s32 hidden;       /* panel closed (lbl_12_bss_1ACA0 flag 4 / single round) */
    /*0x060*/ float faceX;
    /*0x064*/ float faceY;
    /*0x068*/ float faceVelX;
    /*0x06C*/ float faceVelY;
    /*0x070*/ s32 koDisp;       /* displayed KO points, 16.16 */
    /*0x074*/ s32 koVel;
    /*0x078*/ float koScaleY;
    /*0x07C*/ float koScaleVel;
    /*0x080*/ float winOffsetY;
    /*0x084*/ float winOffsetVel;
    /*0x088*/ s32 winCount;
    /*0x08C*/ struct FightHudWin wins[5];
    /*0x12C*/ struct FightHudGauge gauge[5];
};  /* size 0x1A4 */

/* One digit of the countdown: scale spring. */
struct FightCountdownDigit
{
    /*0x00*/ float scale;
    /*0x04*/ float vel;
    /*0x08*/ float target;
};  /* size 0xC */

/* The "FIGHT COUNT DOWN" sprite state (lbl_12_bss_1F568). */
struct FightCountdown
{
    /*0x00*/ s16 state;
    /*0x02*/ s16 timer;
    /*0x04*/ s16 digit;         /* digit on screen, -1 = none */
    /*0x08*/ struct FightCountdownDigit digits[4];
    /*0x38*/ Vec fightScale;    /* "FIGHT" text scale (x, y) */
    /*0x44*/ Vec fightVel;
    /*0x50*/ Vec fightTarget;
};  /* size 0x5C */

/* Path of a round banner sprite (lbl_12_data_A0C8..A188, copied to lbl_12_bss_1F5C4). */
struct FightBannerPath
{
    /*0x00*/ Vec start;
    /*0x0C*/ Vec end;
    /*0x18*/ float unk18;
    /*0x1C*/ float unk1C;
};  /* size 0x20 */

/* One star of the win bar over a fighter (lbl_12_bss_1F648[i].wins). */
struct FightTagWin
{
    /*0x00*/ float scale;
    /*0x04*/ float scaleVel;
    /*0x08*/ float scaleTarget;
    /*0x0C*/ float rot;
    /*0x10*/ float rotVel;
    /*0x14*/ float glow;
    /*0x18*/ float glowTarget;
    /*0x1C*/ s32 timer;
};  /* size 0x20 */

/* KO points and win bar shown over a fighter (lbl_12_bss_1F648[playerId], w11). */
struct FightTag
{
    /*0x00*/ u32 flags;   /* 1: shown, 2: on screen */
    /*0x04*/ s32 koTarget;  /* KO points (match total), 16.16 */
    /*0x08*/ s32 koDisp;    /* displayed KO points, 16.16 */
    /*0x0C*/ s32 koVel;
    /*0x10*/ s16 winTimer;
    /*0x12*/ s16 winCount;  /* stars shown */
    /*0x14*/ struct FightTagWin wins[5];
};  /* size 0xB4 */

/* RGBA as floats (0..255, clamped when quantized) */
struct FightColorF
{
    float r, g, b, a;
};

/* RGB as floats */
struct FightColor3F
{
    float r, g, b;
};

/* Blurred top/bottom screen bands shown during the round intro (lbl_12_bss_1F920). */
struct FightBlur
{
    /*0x00*/ s32 unk0;
    /*0x04*/ s32 active;          /* sprite exists */
    /*0x08*/ s32 shown;           /* fading in (else fading out) */
    /*0x0C*/ struct FightColorF kcolor;   /* konst colour: blend of the blurred copy, a = opacity */
    /*0x1C*/ struct FightColor3F color;   /* tev colour */
    /*0x28*/ struct FightColorF kcolorTarget;
    /*0x38*/ struct FightColor3F colorTarget;
};  /* size 0x44 */

/* One blurred band: screen rectangle, depth and colours (fn_12_29810). */
struct FightBlurBand
{
    /*0x00*/ float x0;
    /*0x04*/ float y0;
    /*0x08*/ float x1;
    /*0x0C*/ float y1;
    /*0x10*/ float z;
    /*0x14*/ struct FightColorF kcolor;
    /*0x24*/ struct FightColor3F color;
};  /* size 0x30 */

/* the same unreferenced table every fight TU starts its .data with */
u8 lbl_12_data_9E48[0x170] = {
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

/* HUD panel positions per player count */
static s16 lbl_12_data_9FB8[1][2] = { { 254, 420 } };
static s16 lbl_12_data_9FBC[2][2] = { { 182, 420 }, { 328, 420 } };
static s16 lbl_12_data_9FC4[3][2] = { { 108, 420 }, { 254, 420 }, { 400, 420 } };
static s16 lbl_12_data_9FD0[4][2] = { { 36, 420 }, { 182, 420 }, { 328, 420 }, { 474, 420 } };
static s16 (*lbl_12_data_9FE0[4])[2] = { lbl_12_data_9FB8, lbl_12_data_9FBC, lbl_12_data_9FC4, lbl_12_data_9FD0 };
/* win bar offset per lbl_12_bss_1ACA0.winsNeeded (wins needed) */
static s16 lbl_12_data_9FF0[5][2] = { { 86, -12 }, { 74, -12 }, { 62, -12 }, { 56, -12 }, { 50, -12 } };
/* "player id" label texture rects (u1, v1, u2, v2), [4] = COM */
static float lbl_12_data_A004[5][4] = {
    { 0.0f, 0.0f, 0.2f, 0.25f },
    { 0.2f, 0.0f, 0.4f, 0.25f },
    { 0.4f, 0.0f, 0.6f, 0.25f },
    { 0.6f, 0.0f, 0.8f, 0.25f },
    { 0.8f, 0.0f, 1.0f, 0.25f },
};
static float lbl_12_data_A054[4] = { 0.5f, 0.25f, 0.75f, 0.0f };
/* face bitmap per monkey */
static u16 lbl_12_data_A064[4] = { 0x1305, 0x1308, 0x1306, 0x1307 };

/* ---- fight_9 bss ---- */
struct FightHud lbl_12_bss_1EED8[4];
struct FightCountdown lbl_12_bss_1F568;
struct FightBannerPath lbl_12_bss_1F5C4[4];  /* w11 */
u32 lbl_12_bss_1F644;                       /* flags, also used by fight_3.c */
struct FightTag lbl_12_bss_1F648[4];        /* w11: KO points / win bar over each fighter */
float lbl_12_bss_1F918[2];                  /* w11: timer sprite position */
struct FightBlur lbl_12_bss_1F920;          /* blurred screen bands */

#include "fight/fight_9_23858.inc"
#include "fight/fight_9_26798.inc"
