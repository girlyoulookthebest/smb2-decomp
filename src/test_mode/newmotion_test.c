#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "bitmap.h"
#include "camera.h"
#include "file.h"
#include "gx.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"

extern int currentHeap;
extern int main_heap;
extern u32 g_some_other_flags;

int sprintf(char *buf, const char *fmt, ...);
int printf(const char *fmt, ...);
char *strstr(const char *s, const char *sub);
void event_init(int id);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_set_color(int color);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void textdraw_set_font(int font);
void textdraw_set_drop_shadow(void);
void textdraw_set_scale(float x, float y);
void textdraw_set_drop_shadow_with_params(int x, int y, float a);
void textdraw_set_font_style(int style);
void textdraw_set_mul_color(int color);
void textdraw_set_alpha(float a);
void textdraw_set_depth(float d);
void textdraw_set_pos(float x, float y);
void textdraw_print(char *str);
void OSFreeToHeap(int heap, void *ptr);
u32 OSCheckHeap(int heap);
int DVDOpen(char *file, struct DiscFileInfo *info);
int DVDClose(struct DiscFileInfo *info);
int DVDChangeDir(char *dir);
int read_entire_file_using_dvdread_prio_async(struct DiscFileInfo *file, void *buf, u32 size, u32 off);
void GXDrawSphere(u8 numMajor, u8 numMinor);
void fn_1_4D8A0(Mtx mtx, int colorIdx, int mode);
void g_GXSetTevIndirect_zero_if_different(int stage);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void GXSetTevColor(int reg, GXColor color);
void fn_8_198E0(void);
void fn_8_19A50(void);
void fn_8_19C30(void);
void fn_8_1960C(void);

struct ModelPart
{
    /*0x00*/ u32 flags;
    /*0x04*/ u16 parent;
    /*0x06*/ u16 unk6;
    /*0x08*/ char *name;
    /*0x0C*/ s8 *list;
    /*0x10*/ Vec rot;
    /*0x1C*/ Vec pos;
}; /* size = 0x28 */

struct ModelFile
{
    /*0x00*/ char *dir;
    /*0x04*/ char *name;
    /*0x08*/ struct ModelPart *parts;
    /*0x0C*/ int count;
}; /* size = 0x10 */

static int workUnk0 = 0;
static int workLoaded = 0;
static int workCharaIdx = 0;
static int workLod = 0;
static int workColor = 0;
static void *workUnk14 = 0;
static void *workUnk18 = 0;
static void *workUnk1C = 0;
static void *workUnk20 = 0;
static struct Skeleton *workSkel = 0;
static struct Skeleton *workSkelHandR = 0;
static struct Skeleton *workSkelHandL = 0;
static struct Skeleton *workSkelFace = 0;
static struct Mta *workMotBody = 0;
static struct Mta *workMotHandR = 0;
static struct Mta *workMotHandL = 0;
static struct Mta *workMotFace = 0;
static int workCursor = 0;
static int workClipBody = 0;
static int workClipHandR = 0;
static int workClipHandL = 0;
static int workClipFace = 0;
static float workSpeed = 0.0f;
static int workInterp = 0;
int lbl_8_bss_41A0 = 0;
static int workUnk64 = 0;
static struct GMA *workGma = 0;
static struct TPL *workTpl = 0;
static void *workMal = 0;
static struct ApeData *workApe = 0;
static Mtx boneMtxs[42];

void fn_8_17BB8(void);
void fn_8_183F4(void);
void fn_8_188B4(void);
void fn_8_18938(void);
void fn_8_1898C(void);
void fn_8_18C8C(void);
void fn_8_18EFC(void);
void fn_8_190D4(void);
void fn_8_19164(void);
void fn_8_192D4(void);
struct GMAModel *fn_8_193FC(struct GMA *gma, char *name);
void fn_8_19480(struct Mta **mta, char *file, struct Skeleton **skel);
void fn_8_19530(void **data, char *file, struct Skeleton **skel);

static s8 lbl_7438[] = { -2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, -2, 13, 14, 15, -1, 0 };
static s8 lbl_744C[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, 0, 0, 0 };
static u32 lbl_7460[] = {
    0x31000000, 0x0, 0x3800b400, 0x10000, 0x1000000, 0x20000, 0x10004c00, 0x30000, 0x1000000, 0x40000, 0x18000000, 0x90000, 0x40000000, 0x0, 0x1000000, 0xb0000, 0x18000000, 0xc0000, 0x18000000, 0x50000, 0x40000000, 0x0, 0x1000000, 0x70000, 0x18000000, 0x80000, 0x3800b400, 0xd0000, 0x40000000, 0x0, 0x18000000, 0x140000, 0x40000000, 0x0, 0x1000000, 0x160000, 0x18000000, 0x170000, 0x18000000, 0x180000, 0x18000000, 0xf0000, 0x40000000, 0x0, 0x1000000, 0x110000, 0x18000000, 0x120000, 0x18000000, 0x130000, 0x18000000, 0x0, 0x18000000, 0x10000, 0x18000000, 0x20000, 0x18000000, 0x30000, 0x18000000, 0x40000, 0x18000000, 0x50000, 0x18000000, 0x60000, 0x18000000, 0x70000, 0x18000000, 0x80000, 0x18000000, 0x90000, 0x18000000, 0xa0000, 0x18000000, 0xb0000, 0x18000000, 0xc0000, 0x18000000, 0xd0000, 0x18000000, 0xe0000, 0x18000000, 0xf0000
};
static int lbl_75A8 = 1;
static int lbl_75AC = 1;
static float lbl_75B0[] = { 23.0f, 21.1f, 22.0f, 21.5f };
static s8 lbl_75C0[] = { -2, 1, -2, -2, -2, -2, -2, -2, -2, -2, 5, 6, -2, -2, -2, -2, -2, 9, 10, -2, -2, -2, -2, 13, -2, -2, 15, 16, -2, -2, 18, -2, 19, -2, 20, 21, -2, -2, 23, -2, 24, -1, 0, 0 };
static s8 lbl_75EC[] = { 1, 5, 6, 9, 10, 13, 15, 16, 18, 19, 20, 21, 23, 24, -1, 0 };
static s8 lbl_75FC[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23, -1, 0 };
static char s_EARL[] = "EARL";
static char s_EARR[] = "EARR";
static char s_FACE[] = "FACE";
static char s_BODY[] = "BODY";
static char s_EYE[] = "EYE";
static char s_HANDL[] = "HANDL";
static char s_HANDR[] = "HANDR";
static char s_HAIR[] = "HAIR";
static struct ModelPart lbl_7650[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, lbl_75FC, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_75EC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_7790[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, lbl_75FC, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_75EC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static char s_EARL_MID[] = "EARL_MID";
static char s_EARR_MID[] = "EARR_MID";
static struct ModelPart lbl_78E8[] = {
    { 0x4000000, 3, 0, s_EARL_MID, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR_MID, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, 0, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_75EC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static u8 lbl_7A00[] = { 0xfe, 0x1, 0xfe, 0xfe, 0x3, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x5, 0x6, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x9, 0xa, 0xfe, 0xfe, 0xfe, 0xfe, 0xd, 0xfe, 0xfe, 0xf, 0x10, 0xfe, 0xfe, 0x12, 0xfe, 0x13, 0xfe, 0x14, 0x15, 0xfe, 0xfe, 0x17, 0xfe };
static s8 lbl_7A28[] = { 24, -1, 0, 0 };
static s8 lbl_7A2C[] = { 13, 15, 16, 18, 19, 20, 21, 23, 24, 1, 5, 6, 9, 10, -1, 0 };
static s8 lbl_7A3C[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 13, 14, 9, 10, 11, 15, 20, 21, 22, 23, 16, 17, 18, 19, -1, 0, 0, 0 };
static char s_FACE_SKL[] = "FACE_SKL";
static char s_HIGH_FLOWER[] = "HIGH_FLOWER";
static char s_ACE_FLOWER[] = "ACE_FLOWER";
static struct ModelPart lbl_7A7C[] = {
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE_SKL, lbl_7A3C, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_7A2C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { -0.00039478f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000002, 13, 0, s_HIGH_FLOWER, 0, { 45.0f, -8.1e+02f, 45.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000002, 3, 0, s_ACE_FLOWER, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static char s_MIE_FLOWER[] = "MIE_FLOWER";
static struct ModelPart lbl_7C18[] = {
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE_SKL, lbl_7A3C, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_7A2C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { -0.00039478f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000002, 13, 0, s_MIE_FLOWER, 0, { 45.0f, -8.1e+02f, 45.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000002, 3, 0, s_ACE_FLOWER, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static char s_LOW_FLOWER[] = "LOW_FLOWER";
static struct ModelPart lbl_7DB4[] = {
    { 0x6000000, 8, 0, s_HANDL, 0, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, 0, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_7A2C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL_MID, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR_MID, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000002, 13, 0, s_LOW_FLOWER, 0, { 45.0f, -8.1e+02f, 45.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000002, 3, 0, s_ACE_FLOWER, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static u8 lbl_7F1C[] = { 0xfe, 0x1, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x5, 0x6, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x9, 0xa, 0xfe, 0xfe, 0xfe, 0xfe, 0xd, 0xfe, 0xfe, 0xf, 0x10, 0xfe, 0xfe, 0x12, 0xfe, 0x13, 0xfe, 0x14, 0x15, 0xfe, 0xfe, 0x17, 0xfe, 0x18, 0xff };
static s8 lbl_7F44[] = { 13, 15, 16, 18, 19, 20, 21, 23, 24, 1, 9, 10, 5, 6, -1, 0 };
static s8 lbl_7F54[] = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1, -1, 0, 0, 0 };
static struct ModelPart lbl_7F64[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.002f, 0.09f, 0.078f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.002f, -0.09f, 0.078f } },
    { 0x1000000, 3, 0, s_FACE, lbl_7F54, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_7F44, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_80A4[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.002f, 0.09f, 0.078f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.002f, -0.09f, 0.078f } },
    { 0x1000000, 3, 0, s_FACE, lbl_7F54, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_7F44, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_81E4[] = {
    { 0x4000000, 3, 0, s_EARL_MID, 0, { 0.0f, 0.0f, 0.0f }, { -0.002f, 0.09f, 0.078f } },
    { 0x5000000, 3, 0, s_EARR_MID, 0, { 0.0f, 0.0f, 0.0f }, { -0.002f, -0.09f, 0.078f } },
    { 0x1000000, 3, 0, s_FACE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_7F44, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static u8 lbl_82FC[] = { 0xfe, 0xfe, 0x1, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x5, 0x6, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x9, 0xa, 0xfe, 0xfe, 0xfe, 0xfe, 0xd, 0xfe, 0xfe, 0xf, 0x10, 0xfe, 0x12, 0x13, 0xfe, 0x14, 0x15, 0xfe, 0x17, 0x18, 0xff, 0x0, 0x0, 0x0 };
static s8 lbl_8324[] = { 1, 5, 6, 8, 9, 10, 12, 13, 15, 16, 18, 19, 20, 21, 23, 24, -1, 0, 0, 0 };
static s8 lbl_8338[] = { 0, 1, 5, 6, 7, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, -1, 0, 0, 0 };
static struct ModelPart lbl_8354[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.19f, 0.15f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, -0.19f, 0.15f } },
    { 0x1000000, 3, 0, s_FACE, lbl_8338, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_8324, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_846C[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.19f, 0.15f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, -0.19f, 0.15f } },
    { 0x1000000, 3, 0, s_FACE, lbl_8338, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_8324, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_8584[] = {
    { 0x4000000, 3, 0, s_EARL_MID, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.19f, 0.15f } },
    { 0x5000000, 3, 0, s_EARR_MID, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, -0.19f, 0.15f } },
    { 0x1000000, 3, 0, s_FACE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_75EC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static s8 lbl_869C[] = { 1, 10, 11, 12, 5, 6, 7, 15, 17, 18, 20, 21, 22, 23, 25, 26, -1, 0, 0, 0 };
static s8 lbl_86B0[] = { 0, 1, 5, 6, 7, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 27, 28, 30, 31, 32, 33, -1, 0, 0, 0 };
static s8 lbl_86D4[] = { 0, 1, 2, 3, 4, 5, -1, 0 };
static char s_HEAD[] = "HEAD";
static char s_TAIL[] = "TAIL";
static char s_WHEEL_L[] = "WHEEL_L";
static char s_WHEEL_R[] = "WHEEL_R";
static char s_EYEGRASS[] = "EYEGRASS";
static char s_BACKPERA[] = "BACKPERA";
static char s_BACKPACK[] = "BACKPACK";
static char s_HELICAP[] = "HELICAP";
static char s_HERIBAR[] = "HERIBAR";
static struct ModelPart lbl_8730[] = {
    { 0x0, 0, 0, s_BODY, lbl_869C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 14, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { 0.077763f, 0.12726f, 0.22095f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { 0.077763f, -0.12725f, 0.22096f } },
    { 0x1000000, 3, 0, s_FACE, lbl_86B0, { -0.0011f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HEAD, 0, { -0.0011f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 9, 0, s_HANDL, lbl_744C, { 0.00018587f, 0.0f, -0.00017739f }, { 0.0f, 0.0f, 0.0f } },
    { 0x8000000, 15, 0, s_TAIL, lbl_86D4, { 0.0f, 0.0f, 0.0f }, { 0.0766666f, 0.0f, 0.0f } },
    { 0x18000000, 25, 0, s_WHEEL_L, 0, { 0.0f, 0.00055379f, 0.0f }, { 0.15144f, 0.0f, -0.00065772f } },
    { 0x18000000, 20, 0, s_WHEEL_R, 0, { 0.0f, 0.00056166f, 0.0f }, { 0.15144f, 0.0f, 0.00064015f } },
    { 0xe000000, 3, 0, s_EYEGRASS, 0, { 1.8e+02f, -0.00053392f, -9e+01f }, { 0.18445f, -0.055732f, 0.16279f } },
    { 0x14000000, 1, 0, s_BACKPERA, 0, { -45.0f, -9e+01f, -45.0f }, { -0.04759f, 0.3346f, 0.0f } },
    { 0x15000000, 1, 0, s_BACKPACK, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x16000000, 1, 0, s_HELICAP, 0, { -45.0f, -9e+01f, -45.0f }, { 0.85771f, 0.28539f, -2e-05f } },
    { 0x17000000, 1, 0, s_HERIBAR, 0, { -45.0f, -9e+01f, -45.0f }, { 0.18685f, 0.28539f, 0.0f } },
};
static struct ModelPart lbl_8988[] = {
    { 0x0, 0, 0, s_BODY, lbl_869C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 14, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { 0.077763f, 0.12726f, 0.22095f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { 0.077763f, -0.12725f, 0.22096f } },
    { 0x1000000, 3, 0, s_FACE, lbl_86B0, { -0.0011f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 9, 0, s_HANDL, lbl_744C, { 0.00018587f, 0.0f, -0.00017739f }, { 0.0f, 0.0f, 0.0f } },
    { 0x8000000, 15, 0, s_TAIL, 0, { 0.0f, 0.0f, 0.0f }, { 0.076659f, 0.0f, 0.0f } },
    { 0x18000000, 25, 0, s_WHEEL_L, 0, { 0.0f, 0.00055379f, 0.0f }, { 0.15144f, 0.0f, -0.00065772f } },
    { 0x18000000, 20, 0, s_WHEEL_R, 0, { 0.0f, 0.00056166f, 0.0f }, { 0.15144f, 0.0f, 0.00064015f } },
    { 0x18000000, 3, 0, s_EYEGRASS, 0, { 1.8e+02f, -0.00053392f, -9e+01f }, { 0.18445f, -0.055732f, 0.16279f } },
};
static struct ModelPart lbl_8B18[] = {
    { 0x0, 0, 0, s_BODY, lbl_869C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 14, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { 0.077763f, 0.12726f, 0.22095f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { 0.077763f, -0.12725f, 0.22096f } },
    { 0x1000000, 3, 0, s_FACE, lbl_86B0, { -0.0011f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 9, 0, s_HANDL, lbl_744C, { 0.00018587f, 0.0f, -0.00017739f }, { 0.0f, 0.0f, 0.0f } },
    { 0x8000000, 15, 0, s_TAIL, 0, { 0.0f, 0.0f, 0.0f }, { 0.076659f, 0.0f, 0.0f } },
    { 0x18000000, 25, 0, s_WHEEL_L, 0, { 0.0f, 0.00055379f, 0.0f }, { 0.15144f, 0.0f, -0.00065772f } },
    { 0x18000000, 20, 0, s_WHEEL_R, 0, { 0.0f, 0.00056166f, 0.0f }, { 0.15144f, 0.0f, 0.00064015f } },
    { 0x18000000, 3, 0, s_EYEGRASS, 0, { 1.8e+02f, -0.00053392f, -9e+01f }, { 0.18445f, -0.055732f, 0.16279f } },
};
static s8 lbl_8CA8[] = { 1, 5, 6, 7, 3, 10, 11, 12, 15, 17, 18, 20, 21, 22, 23, 25, 26, -1, 0, 0 };
static s8 lbl_8CBC[] = { 0, 15, 20, 21, 22, 23, 16, 17, 18, 19, 1, 5, 6, 7, 2, 3, 4, 8, 12, 13, 14, 9, 10, 11, 28, 30, 31, 24, 26, 27, -1, 0 };
static char s_ZORIL[] = "ZORIL";
static char s_ZORIR[] = "ZORIR";
static char s_WHEEL_LA[] = "WHEEL_LA";
static char s_WHEEL_LB[] = "WHEEL_LB";
static char s_WHEEL_RA[] = "WHEEL_RA";
static char s_WHEEL_RB[] = "WHEEL_RB";
static struct ModelPart lbl_8D1C[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { 0.012065f, 0.11359f, 0.14496f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { 0.012065f, -0.11359f, 0.14496f } },
    { 0x18000000, 26, 0, s_ZORIL, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000000, 21, 0, s_ZORIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000000, 26, 0, s_WHEEL_LA, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, 0.00641f, 0.00518f } },
    { 0x18000000, 26, 0, s_WHEEL_LB, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, -0.16266f, 0.00518f } },
    { 0x18000000, 21, 0, s_WHEEL_RA, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, 0.00641f, 0.00518f } },
    { 0x18000000, 21, 0, s_WHEEL_RB, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, -0.16266f, 0.00518f } },
    { 0x7000000, 14, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_8CA8, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, lbl_8CBC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 9, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_8EFC[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { 0.012065f, 0.11359f, 0.14496f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { 0.012065f, -0.11359f, 0.14496f } },
    { 0x18000000, 26, 0, s_ZORIL, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000000, 21, 0, s_ZORIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000000, 26, 0, s_WHEEL_LA, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, 0.00641f, 0.00518f } },
    { 0x18000000, 26, 0, s_WHEEL_LB, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, -0.16266f, 0.00518f } },
    { 0x18000000, 21, 0, s_WHEEL_RA, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, 0.00641f, 0.00518f } },
    { 0x18000000, 21, 0, s_WHEEL_RB, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, -0.16266f, 0.00518f } },
    { 0x7000000, 14, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_8CA8, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, lbl_8CBC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 9, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_90DC[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { 0.012065f, 0.11359f, 0.14496f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { 0.012065f, -0.11359f, 0.14496f } },
    { 0x18000000, 26, 0, s_ZORIL, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000000, 21, 0, s_ZORIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x18000000, 26, 0, s_WHEEL_LA, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, 0.00641f, 0.00518f } },
    { 0x18000000, 26, 0, s_WHEEL_LB, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, -0.16266f, 0.00518f } },
    { 0x18000000, 21, 0, s_WHEEL_RA, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, 0.00641f, 0.00518f } },
    { 0x18000000, 21, 0, s_WHEEL_RB, 0, { 0.0f, 0.0f, 0.0f }, { 0.05658f, -0.16266f, 0.00518f } },
    { 0x7000000, 14, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_8CA8, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, lbl_8CBC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 9, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static s8 lbl_92BC[] = { 1, 3, 5, 6, 9, 10, 13, 15, 16, 18, 19, 20, 21, 23, 24, -1 };
static s8 lbl_92CC[] = { 1, 5, 6, 9, 10, 13, 15, 16, 18, 19, 20, 21, 23, 24, -1, 0 };
static s8 lbl_92DC[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23, -1, 0 };
static struct ModelPart lbl_92F4[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, lbl_92DC, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_92CC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_9434[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, lbl_92DC, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_92CC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_9574[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, lbl_92DC, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_92CC, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static s8 lbl_96B4[] = { 1, 3, 5, 6, 9, 10, 13, 15, 16, 18, 19, 20, 21, 23, 24, -1 };
static s8 lbl_96C4[] = { 13, 15, 16, 18, 19, 20, 21, 23, 24, 1, 5, 6, 9, 10, -1, 0 };
static s8 lbl_96D4[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 13, 14, 9, 10, 11, 15, 20, 21, 22, 23, 16, 17, 18, 19, -1, 0, 0, 0 };
static struct ModelPart lbl_96F0[] = {
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, lbl_96D4, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_96C4, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { -0.00039478f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_9830[] = {
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, lbl_96D4, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_96C4, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { -0.00039478f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_9970[] = {
    { 0x6000000, 8, 0, s_HANDL, 0, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, 0, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_96C4, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static s8 lbl_9A88[] = { 1, 3, 5, 6, 9, 10, 13, 15, 16, 18, 19, 20, 21, 23, 24, -1 };
static s8 lbl_9A98[] = { 1, 5, 6, 9, 10, 13, 15, 16, 18, 19, 20, 21, 23, 24, -1, 0 };
static s8 lbl_9AA8[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23, -1, 0 };
static struct ModelPart lbl_9AC0[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, lbl_9AA8, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_9A98, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_9C00[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, lbl_9AA8, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_9A98, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_9D40[] = {
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x1000000, 3, 0, s_FACE, lbl_9AA8, { -0.00097034f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_9A98, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static s8 lbl_9E80[] = { 1, 3, 5, 6, 9, 10, 13, 15, 16, 18, 19, 20, 21, 23, 24, -1 };
static s8 lbl_9E90[] = { 13, 15, 16, 18, 19, 20, 21, 23, 24, 1, 5, 6, 9, 10, -1, 0 };
static s8 lbl_9EA0[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 13, 14, 9, 10, 11, 15, 20, 21, 22, 23, 16, 17, 18, 19, -1, 0, 0, 0 };
static struct ModelPart lbl_9EBC[] = {
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, lbl_9EA0, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_9E90, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { -0.00039478f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_9FFC[] = {
    { 0x6000000, 8, 0, s_HANDL, lbl_744C, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, lbl_744C, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, lbl_9EA0, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_9E90, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x2000000, 3, 0, s_HAIR, 0, { -0.00039478f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static struct ModelPart lbl_A13C[] = {
    { 0x6000000, 8, 0, s_HANDL, 0, { -1.8e+02f, 1.8e+02f, 1.8e+02f }, { 0.0016364f, 0.0f, 0.0f } },
    { 0x7000000, 12, 0, s_HANDR, 0, { 0.0f, 0.0f, 0.0f }, { 0.0015f, 0.0f, 0.0f } },
    { 0x1000000, 3, 0, s_FACE, 0, { -0.00093996f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x0, 0, 0, s_BODY, lbl_9E90, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x4000000, 3, 0, s_EARL, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, 0.12f, 0.1f } },
    { 0x5000000, 3, 0, s_EARR, 0, { 0.0f, 0.0f, 0.0f }, { -0.03f, -0.12f, 0.1f } },
    { 0x3000000, 3, 0, s_EYE, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
};
static char s_ape[] = "ape";
static char s_aiai_h[] = "aiai_h";
static char s_aiai_m[] = "aiai_m";
static char s_aiai_l[] = "aiai_l";
static char s_meemee_h[] = "meemee_h";
static char s_meemee_m[] = "meemee_m";
static char s_meemee_l[] = "meemee_l";
static char s_baby_h[] = "baby_h";
static char s_baby_m[] = "baby_m";
static char s_baby_l[] = "baby_l";
static char s_gongon_h[] = "gongon_h";
static char s_gongon_m[] = "gongon_m";
static char s_gongon_l[] = "gongon_l";
static char s_madh_h[] = "madh_h";
static char s_madh_m[] = "madh_m";
static char s_madh_l[] = "madh_l";
static char s_kobun_h[] = "kobun_h";
static char s_kobun_m[] = "kobun_m";
static char s_kobun_l[] = "kobun_l";
static char s_male_h[] = "male_h";
static char s_male_m[] = "male_m";
static char s_male_l[] = "male_l";
static char s_female_h[] = "female_h";
static char s_female_m[] = "female_m";
static char s_female_l[] = "female_l";
static char s_jiji_h[] = "jiji_h";
static char s_jiji_m[] = "jiji_m";
static char s_jiji_l[] = "jiji_l";
static char s_baba_h[] = "baba_h";
static char s_baba_m[] = "baba_m";
static char s_baba_l[] = "baba_l";
static struct ModelFile modelFiles[][4] = {
    {
        { s_ape, s_aiai_h, lbl_7650, 8 },
        { s_ape, s_aiai_m, lbl_7790, 8 },
        { s_ape, s_aiai_l, lbl_78E8, 7 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_meemee_h, lbl_7A7C, 10 },
        { s_ape, s_meemee_m, lbl_7C18, 10 },
        { s_ape, s_meemee_l, lbl_7DB4, 9 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_baby_h, lbl_7F64, 8 },
        { s_ape, s_baby_m, lbl_80A4, 8 },
        { s_ape, s_baby_l, lbl_81E4, 7 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_gongon_h, lbl_8354, 7 },
        { s_ape, s_gongon_m, lbl_846C, 7 },
        { s_ape, s_gongon_l, lbl_8584, 7 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_madh_h, lbl_8730, 15 },
        { s_ape, s_madh_m, lbl_8988, 10 },
        { s_ape, s_madh_l, lbl_8B18, 10 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_kobun_h, lbl_8D1C, 12 },
        { s_ape, s_kobun_m, lbl_8EFC, 12 },
        { s_ape, s_kobun_l, lbl_90DC, 12 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_male_h, lbl_92F4, 8 },
        { s_ape, s_male_m, lbl_9434, 8 },
        { s_ape, s_male_l, lbl_9574, 8 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_female_h, lbl_96F0, 8 },
        { s_ape, s_female_m, lbl_9830, 8 },
        { s_ape, s_female_l, lbl_9970, 7 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_jiji_h, lbl_9AC0, 8 },
        { s_ape, s_jiji_m, lbl_9C00, 8 },
        { s_ape, s_jiji_l, lbl_9D40, 8 },
        { 0, 0, 0, 0 },
    },
    {
        { s_ape, s_baba_h, lbl_9EBC, 8 },
        { s_ape, s_baba_m, lbl_9FFC, 8 },
        { s_ape, s_baba_l, lbl_A13C, 7 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
};
static struct ModelFile *currModelFile = (struct ModelFile *)modelFiles;
static char s_MDL_AIAI[] = "MDL_AIAI";
static char s_MDL_MEEMEE[] = "MDL_MEEMEE";
static char s_MDL_BABY[] = "MDL_BABY";
static char s_MDL_GONGON[] = "MDL_GONGON";
static char s_MDL_MADH[] = "MDL_MADH";
static char s_MDL_KOBUN[] = "MDL_KOBUN";
static char s_MDL_MALE[] = "MDL_MALE";
static char s_MDL_FEMALE[] = "MDL_FEMALE";
static char s_MDL_JIJI[] = "MDL_JIJI";
static char s_MDL_BABA[] = "MDL_BABA";
static char *lbl_A828[] = { s_MDL_AIAI, s_MDL_MEEMEE, s_MDL_BABY, s_MDL_GONGON, s_MDL_MADH, s_MDL_KOBUN, s_MDL_MALE, s_MDL_FEMALE, s_MDL_JIJI, s_MDL_BABA };
static char s_LOD_High[] = "LOD_High";
static char s_LOD_Middle[] = "LOD_Middle";
static char s_LOD_Low[] = "LOD_Low";
static char *lbl_A870[] = { s_LOD_High, s_LOD_Middle, s_LOD_Low };
static char s_skeleton[] = "skeleton";
static char s_aiai_bin[] = "aiai.bin";
static char s_APE_SKELETON[] = "APE_SKELETON";
static char s_meemee_bin[] = "meemee.bin";
static char s_GAL_SKELETON[] = "GAL_SKELETON";
static char s_baby_bin[] = "baby.bin";
static char s_KID_SKELETON[] = "KID_SKELETON";
static char s_gongon_bin[] = "gongon.bin";
static char s_GOR_SKELETON[] = "GOR_SKELETON";
static char s_MAD_SKELETON[] = "MAD_SKELETON";
static char s_KOB_SKELETON[] = "KOB_SKELETON";
static char *lbl_A918[] = { s_skeleton, s_aiai_bin, s_APE_SKELETON, s_skeleton, s_meemee_bin, s_GAL_SKELETON, s_skeleton, s_baby_bin, s_KID_SKELETON, s_skeleton, s_gongon_bin, s_GOR_SKELETON, s_skeleton, s_aiai_bin, s_MAD_SKELETON, s_skeleton, s_aiai_bin, s_KOB_SKELETON };
static char s_Default[] = "Default";
static char s_Blue[] = "Blue";
static char s_Yellow[] = "Yellow";
static char s_Green[] = "Green";
static char *lbl_A980[] = { s_Default, s_Blue, s_Yellow, s_Green };

void fn_8_175E0(void)
{
    workUnk14 = NULL;
    workUnk18 = NULL;
    workUnk1C = NULL;
    workUnk20 = NULL;
    workSkel = NULL;
    workSkelHandR = NULL;
    workSkelHandL = NULL;
    workSkelFace = NULL;
    workMotBody = NULL;
    workMotHandR = NULL;
    workMotHandL = NULL;
    workMotFace = NULL;
    event_init(0x10);
    enable_cameras_for_active_players();
    camera_set_state(2);
    g_current_camera->eye.x = 0.0f;
    g_current_camera->eye.y = 0.5f;
    g_current_camera->eye.z = -1.0f;
    g_current_camera->lookAt.x = 0.0f;
    g_current_camera->lookAt.y = 0.25f;
    g_current_camera->lookAt.z = 0.0f;
    fn_8_18C8C();
    fn_8_190D4();
    workSkelHandL->flags |= 2;
    workSkelHandR->flags &= ~2;
    fn_8_19164();
    workCursor = 0;
    workCharaIdx = 0;
    workClipBody = 0;
    workClipHandR = 0;
    workClipHandL = 0;
    workClipFace = 0;
    workLod = 0;
    workColor = 0;
    workSpeed = 1.0f;
    workInterp = 0;
    lbl_8_bss_41A0 = 1;
    g_unk_ape_anim1(workSkel, 0, workClipBody, workInterp, 0);
    workLoaded = 1;
    workUnk64 = 0;
}

void fn_8_177C4(void)
{
    if ((g_some_other_flags & 0xA) == 0)
    {
        if (lbl_8_bss_41A0 == 0)
        {
            fn_8_198E0();
            return;
        }
        fn_8_17BB8();
        if (workSkel != NULL)
            fn_1_10BE3C(workSkel);
    }
}

void fn_8_1783C(void)
{
    if (lbl_8_bss_41A0 == 0)
    {
        debug_textdraw_set_pos(1, 1);
        debug_textdraw_set_color(0xF3);
        debug_textdraw_vsprintf_buf_1("[MotionEdit]");
        debug_textdraw_set_color(0xFF);
        fn_8_19A50();
    }
    else
    {
        debug_textdraw_set_pos(1, 1);
        debug_textdraw_set_color(0xF3);
        debug_textdraw_vsprintf_buf_1("[MotionTest]");
        debug_textdraw_set_color(0xFF);
        fn_8_183F4();
        fn_8_188B4();
        if (workUnk64 != 0)
            fn_8_18938();
        fn_8_1898C();
    }
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_drop_shadow_with_params(2, 2, 1.0f);
    textdraw_set_font_style(0);
    textdraw_set_mul_color(-1);
    textdraw_set_alpha(1.0f);
    textdraw_set_depth(0.11f);
    textdraw_set_pos(510.0f, 448.0f);
    textdraw_print("/cd83B4/ ボール");
    if (lbl_8_bss_41A0 == 0)
    {
        textdraw_set_pos(510.0f, 424.0f);
        textdraw_print("/cd83Af/ LOD");
        textdraw_set_pos(510.0f, 400.0f);
        textdraw_print("/cd83A1/TYPE");
    }
}

void fn_8_179A0(void)
{
    if (lbl_8_bss_41A0 == 0)
    {
        fn_8_19C30();
        lbl_8_bss_41A0 = 1;
    }
    if (workLoaded != 0)
    {
        int heap = OSSetCurrentHeap(main_heap);

        g_something_freeing_heap(workGma);
        g_something_freeing_heap_2(workTpl);
        OSSetCurrentHeap(heap);
        OSFreeToHeap(currentHeap, workUnk14);
        OSFreeToHeap(currentHeap, workUnk18);
        OSFreeToHeap(currentHeap, workUnk1C);
        OSFreeToHeap(currentHeap, workUnk20);
        OSFreeToHeap(currentHeap, workSkel);
        OSFreeToHeap(currentHeap, workSkelHandL);
        OSFreeToHeap(currentHeap, workSkelHandR);
        OSFreeToHeap(currentHeap, workSkelFace);
        OSFreeToHeap(currentHeap, workMotBody);
        OSFreeToHeap(currentHeap, workMotHandR);
        OSFreeToHeap(currentHeap, workMotHandL);
        OSFreeToHeap(currentHeap, workMotFace);
        OSFreeToHeap(currentHeap, workMal);
        workGma = NULL;
        workTpl = NULL;
        workUnk14 = NULL;
        workUnk18 = NULL;
        workUnk1C = NULL;
        workUnk20 = NULL;
        workSkel = NULL;
        workSkelHandR = NULL;
        workSkelHandL = NULL;
        workSkelFace = NULL;
        workMotBody = NULL;
        workMotHandR = NULL;
        workMotHandL = NULL;
        workMotFace = NULL;
        workLoaded = 0;
        workMal = NULL;
        g_something_freeing_heap_3(workApe);
        workApe = NULL;
    }
}

#define BUTTON_HIT(b) \
    ((pad_status_groups->unk0[2].button & (b)) || (pad_status_groups->unk0[4].button & (b)) \
     || ((pad_status_groups->unk0[0].button & (b)) && (pad_status_groups->unk0[0].button & 0x20)))

void fn_8_17BB8(void)
{
    int nBody = workMotBody->count & 0x7FFFFFFF;
    int nHandR = workMotHandR->count & 0x7FFFFFFF;
    int nHandL = workMotHandL->count & 0x7FFFFFFF;
    int nFace = workMotFace->count & 0x7FFFFFFF;
    int *val;
    int max;
    int flags;

    if (pad_status_groups->unk0[2].button & 0x800)
        workUnk64 = ~workUnk64;
    if (pad_status_groups->unk0[2].button & 0x400)
        workUnk0 = ~workUnk0;
    if ((pad_status_groups->unk0[2].button & 8) || (pad_status_groups->unk0[2].button & 4))
    {
        if (BUTTON_HIT(8))
            workCursor = (workCursor + 9) % 10;
        else if (BUTTON_HIT(4))
            workCursor = (workCursor + 1) % 10;
    }
    switch (workCursor)
    {
    case 0:
        val = &workClipBody;
        max = nBody;
        break;
    case 1:
        val = &workCharaIdx;
        max = 17;
        break;
    case 2:
        val = &workLod;
        max = 4;
        break;
    case 3:
        val = &workColor;
        max = 4;
        break;
    case 4:
        val = &workClipFace;
        max = nFace;
        break;
    case 5:
        val = &workClipHandR;
        max = nHandR;
        break;
    case 6:
        val = &workClipHandL;
        max = nHandL;
        break;
    case 8:
        val = &workInterp;
        max = 60;
        break;
    }
    switch (workCursor)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 8:
        if (BUTTON_HIT(1))
            *val = (*val + max - 1) % max;
        else if (BUTTON_HIT(2))
            *val = (*val + 1) % max;
        break;
    case 7:
        if (BUTTON_HIT(1))
        {
            workSpeed -= 0.01f;
            if (workSpeed < 0.0)
                workSpeed = 0.0f;
        }
        else if (BUTTON_HIT(2))
        {
            workSpeed += 0.01f;
            if (workSpeed > 10.0)
                workSpeed = 10.0f;
        }
        break;
    }
    if (BUTTON_HIT(1) || BUTTON_HIT(2))
    {
        flags = 0;
        flags |= lbl_75A8;
        flags |= lbl_75AC << 1;
        switch (workCursor)
        {
        case 0:
            g_unk_ape_anim1(workSkel, 0, workClipBody, workInterp, flags);
            break;
        case 1:
        case 2:
            currModelFile = &modelFiles[workCharaIdx][workLod];
            fn_8_19164();
            OSReport("model_load HeapRemain: %p\n", OSCheckHeap(currentHeap));
            fn_8_18EFC();
            OSReport("bone_reload HeapRemain: %p\n", OSCheckHeap(currentHeap));
            g_unk_ape_anim1(workSkel, 0, workClipBody, workInterp, flags);
            g_unk_ape_anim1(workSkelFace, 0, workClipFace, workInterp, flags);
            g_unk_ape_anim1(workSkelHandR, 0, workClipHandR, workInterp, flags);
            g_unk_ape_anim1(workSkelHandL, 0, workClipHandL, workInterp, flags);
            workSkelHandL->flags |= 2;
            OSReport("%d end HeapRemain: %p\n", g_font_char_var_3, OSCheckHeap(currentHeap));
            break;
        case 4:
            g_unk_ape_anim1(workSkelFace, 0, workClipFace, workInterp, flags);
            break;
        case 5:
            g_unk_ape_anim1(workSkelHandR, 0, workClipHandR, workInterp, flags);
            break;
        case 6:
            g_unk_ape_anim1(workSkelHandL, 0, workClipHandL, workInterp, flags);
            workSkelHandL->flags |= 2;
            break;
        case 7:
            workSkel->clock.speed = workSpeed;
            workSkelFace->clock.speed = workSpeed;
            workSkelHandR->clock.speed = workSpeed;
            workSkelHandL->clock.speed = workSpeed;
            break;
        }
    }
    if (workCursor == 9 && BUTTON_HIT(0x100))
    {
        fn_8_1960C();
        lbl_8_bss_41A0 = 0;
    }
}

static inline int motCount(struct Mta *m)
{
    return m->count & 0x7FFFFFFF;
}

void fn_8_183F4(void)
{
    char *colorNames[4] = { "red(default)", "blue", "yellow", "green" };
    int nBody = motCount(workMotBody);
    int nFace = motCount(workMotFace);
    int nHandR = motCount(workMotHandR);
    int nHandL = motCount(workMotHandL);
    char *name;
    int i = 0;

    debug_textdraw_set_pos(1, workCursor + 2);
    debug_textdraw_set_color(0xC3);
    debug_textdraw_vsprintf_buf_1("\x1c");
    name = fn_1_10DFDC(workMotBody, workClipBody);
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%3d/%3d] %s", g_table_index(workMotBody, name), nBody, name);
    debug_textdraw_set_pos(46, i + 2);
    debug_textdraw_vsprintf_buf_1("[%5.3lf]\n", workSkel->clock.frame / workSkel->clock.length);
    i++;
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%3d/%3d] %s", workCharaIdx, 17, lbl_A828[workCharaIdx]);
    i++;
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%3d/%3d] %s\n", workLod, 4, lbl_A870[workLod]);
    i++;
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%3d/%3d] %s\n", workColor, 4, lbl_A980[workColor]);
    i++;
    name = fn_1_10DFDC(workMotFace, workClipFace);
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%3d/%3d] %s", g_table_index(workMotFace, name), nFace, name);
    debug_textdraw_set_pos(46, i + 2);
    debug_textdraw_vsprintf_buf_1("[%5.3lf]\n", workSkelFace->clock.frame / workSkelFace->clock.length);
    i++;
    name = fn_1_10DFDC(workMotHandR, workClipHandR);
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%3d/%3d] %s", g_table_index(workMotHandR, name), nHandR, name);
    debug_textdraw_set_pos(46, i + 2);
    debug_textdraw_vsprintf_buf_1("[%5.3lf]\n", workSkelHandR->clock.frame / workSkelHandR->clock.length);
    i++;
    name = fn_1_10DFDC(workMotHandL, workClipHandL);
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%3d/%3d] %s", g_table_index(workMotHandL, name), nHandL, name);
    debug_textdraw_set_pos(46, i + 2);
    debug_textdraw_vsprintf_buf_1("[%5.3lf]\n", workSkelHandL->clock.frame / workSkelHandL->clock.length);
    i++;
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%7.3lf] speed", workSpeed);
    i++;
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[%6d ] interp", workInterp);
    i++;
    debug_textdraw_set_pos(2, i + 2);
    if (workCursor == i)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("[ ----- ] Goto MotionEdit");
}

void fn_8_188B4(void)
{
    mtxa_from_mtxb();
    mtxa_translate_xyz(0.0f, -0.5f, 0.0f);
    set_post_mult_color(0.9f, 0.9f, 1.0f, 1.0f);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[32].model);
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
}

void fn_8_18938(void)
{
    fn_8_192D4();
    mtxa_from_mtxb_translate(&g_current_camera->lookAt);
    mtxa_scale_s(0.05f);
    g_smth_with_gpu_and_locked_cache_mtx();
    GXDrawSphere(8, 8);
}

static inline int listLen(s8 *list)
{
    int n = 0;

    while (list[n++] != -1)
        ;
    return n - 1;
}

void fn_8_1898C(void)
{
    int i;

    fn_8_192D4();
    mtxa_push();
    for (i = 0; i < currModelFile->count; i++)
    {
        struct ModelPart *part = &currModelFile->parts[i];
        struct GMAModel *model = fn_8_193FC(workGma, part->name);

        if (model != NULL)
        {
            mtxa_from_mtxb();
            mtxa_translate_xyz(0.0f, -0.12f, 0.0f);
            if (part->parent != 0)
            {
                struct Bone *b = &workSkel->bones[part->parent];

                mtxa_mult_right(b->mtx);
            }
            mtxa_translate(&part->pos);
            mtxa_rotate_z(182.04445f * part->rot.z);
            mtxa_rotate_y(182.04445f * part->rot.y);
            mtxa_rotate_x(182.04445f * part->rot.x);
            g_smth_with_gpu_and_locked_cache_mtx();
            if (part->list != NULL)
            {
                struct Skeleton *skel;
                int j;
                int n = listLen(part->list);

                if (strstr(part->name, s_FACE) != NULL)
                    skel = workSkelFace;
                else if (strstr(part->name, s_HANDL) != NULL)
                    skel = workSkelHandL;
                else if (strstr(part->name, s_HANDR) != NULL)
                    skel = workSkelHandR;
                else
                    skel = workSkel;
                for (j = 0; j < n; j++)
                {
                    if (part->list[j] >= 0)
                    {
                        struct Bone *b = &skel->bones[part->list[j]];

                        mtx_copy(b->mtx, boneMtxs[j]);
                    }
                }
                if (strstr(part->name, s_FACE) != NULL)
                    draw_poly(model, NULL);
                else if (strstr(part->name, s_HANDL) != NULL)
                    draw_poly(model, NULL);
                else if (strstr(part->name, s_HANDR) != NULL)
                    draw_poly(model, NULL);
                else
                    pointer_range_advance(model, boneMtxs);
            }
            else
            {
                draw_poly(model, NULL);
                g_smth_with_gpu_and_locked_cache_mtx();
            }
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    mtxa_pop();
    if (workUnk0 != 0)
    {
        mtxa_from_mtxb();
        mtxa_rotate_y(g_font_char_var_3 * 16);
        mtxa_rotate_x(g_font_char_var_3 << 5);
        g_smth_with_gpu_and_locked_cache_mtx();
        fn_1_4D8A0(mtxa->mtxA, 3, 3);
    }
}

void fn_8_18C8C(void)
{
    static char *lbl_AB58[] = { "aiai", "meemee", "baby", "gongon", "madh", "kobun", "male", "female", "jiji", "baba" };
    char path[0x100];
    struct DiscFileInfo file;
    u32 size;

    DVDChangeDir(s_ape);
    DVDOpen("skel.ska", &file);
    size = OSRoundUp32B(file.length);
    workUnk14 = alloc_from_heap_or_panic(currentHeap, size, "newmotion_test.c", 1024);
    read_entire_file_using_dvdread_prio_async(&file, workUnk14, size, 0);
    DVDClose(&file);
    DVDChangeDir("../ape");
    sprintf(path, "%s_test.mta", lbl_AB58[workCharaIdx]);
    if (DVDOpen(path, &file))
    {
        size = OSRoundUp32B(file.length);
        workMotBody = alloc_from_heap_or_panic(currentHeap, size, "newmotion_test.c", 1035);
        read_entire_file_using_dvdread_prio_async(&file, workMotBody, size, 0);
        DVDClose(&file);
    }
    else
    {
        OSPanic("newmotion_test.c", 1040, "No such file...");
    }
    sprintf(path, "%s_common.mal", lbl_AB58[workCharaIdx]);
    if (DVDOpen(path, &file))
    {
        size = OSRoundUp32B(file.length);
        workMal = alloc_from_heap_or_panic(currentHeap, size, "newmotion_test.c", 1047);
        read_entire_file_using_dvdread_prio_async(&file, workMal, size, 0);
        DVDClose(&file);
        workApe = g_get_structure_from_ape_common_mal_struct(workMal);
        fn_1_11589C(workApe, workUnk14);
        workSkel = g_alloc_some_ape_state_0x8(workApe, workCharaIdx);
        *(u8 *)workApe = workCharaIdx;
        fn_1_115960(workApe, workMotBody);
        g_ape_anim_unk3(workApe);
    }
    else
    {
        OSPanic("newmotion_test.c", 1061, "No such file...");
    }
    DVDChangeDir("../ape");
    fn_8_19530(&workUnk18, "aiai_handr.skl", &workSkelHandR);
    fn_8_19530(&workUnk1C, "aiai_handl.skl", &workSkelHandL);
    workSkelHandL->flags |= 2;
    fn_8_19530(&workUnk20, "aiai_face.skl", &workSkelFace);
    DVDChangeDir("..");
}

void fn_8_18EFC(void)
{
    OSFreeToHeap(currentHeap, workMotBody);
    OSFreeToHeap(currentHeap, workMotHandR);
    OSFreeToHeap(currentHeap, workMotHandL);
    OSFreeToHeap(currentHeap, workMotFace);
    OSFreeToHeap(currentHeap, workUnk14);
    OSFreeToHeap(currentHeap, workUnk18);
    OSFreeToHeap(currentHeap, workUnk1C);
    OSFreeToHeap(currentHeap, workUnk20);
    OSFreeToHeap(currentHeap, workSkel);
    OSFreeToHeap(currentHeap, workSkelHandR);
    OSFreeToHeap(currentHeap, workSkelHandL);
    OSFreeToHeap(currentHeap, workSkelFace);
    OSFreeToHeap(currentHeap, workMal);
    workUnk14 = NULL;
    workUnk18 = NULL;
    workUnk1C = NULL;
    workUnk20 = NULL;
    workSkel = NULL;
    workSkelHandR = NULL;
    workSkelHandL = NULL;
    workSkelFace = NULL;
    workMotBody = NULL;
    workMotHandR = NULL;
    workMotHandL = NULL;
    workMotFace = NULL;
    workMal = NULL;
    g_something_freeing_heap_3(workApe);
    workApe = NULL;
    fn_8_18C8C();
    fn_8_190D4();
    workSkelHandL->flags |= 2;
    workSkelHandR->flags &= ~2;
}

void fn_8_190D4(void)
{
    DVDChangeDir(s_ape);
    fn_8_19480(&workMotHandR, "aiai_hand.mta", &workSkelHandR);
    fn_8_19480(&workMotHandL, "aiai_hand.mta", &workSkelHandL);
    workSkelHandL->flags |= 2;
    fn_8_19480(&workMotFace, "aiai_face.mta", &workSkelFace);
    DVDChangeDir("..");
}

void fn_8_19164(void)
{
    char path[0x100];
    int heap;

    DVDChangeDir(currModelFile->dir);
    heap = OSSetCurrentHeap(main_heap);
    if (workGma != NULL)
    {
        g_something_freeing_heap(workGma);
        workGma = NULL;
    }
    if (workTpl != NULL)
    {
        g_something_freeing_heap_2(workTpl);
        workTpl = NULL;
    }
    sprintf(path, "%s.tpl", currModelFile->name);
    printf("load %s\n", path);
    workTpl = g_load_tpl(path);
    sprintf(path, "%s.gma", currModelFile->name);
    printf("load %s\n", path);
    workGma = g_load_gma(path, workTpl);
    OSReport("HeapRemain: %p\n", OSCheckHeap(currentHeap));
    OSSetCurrentHeap(heap);
    DVDChangeDir("..");
}

void fn_8_192D4(void)
{
    static const GXColor colors[] = {
        { 0xFF, 0xFF, 0xFF, 0x80 },
        { 0xFF, 0x7F, 0xBF, 0xFF },
        { 0x4C, 0x99, 0xFF, 0xFF },
        { 0xE5, 0xA5, 0x4C, 0xFF },
        { 0x4C, 0xB2, 0x4C, 0xFF },
    };
    GXColor color = colors[0];

    GXSetNumChans_cached(1);
    GXSetNumTexGens_cached(0);
    GXSetNumTevStages_cached(1);
    g_GXSetTevIndirect_zero_if_different(0);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetTevSwapMode_cached(0, 0, 0);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 2);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetCullMode_cached(0);
    GXSetTevColor(1, color);
}

struct GMAModel *fn_8_193FC(struct GMA *gma, char *name)
{
    struct GMAModelEntry *entry;
    int i;

    if (gma == NULL)
        return NULL;
    entry = gma->modelEntries;
    i = gma->numModels;
    while (i > 0)
    {
        if (strstr(entry->name, name) != NULL)
            return entry->model;
        i--;
        entry++;
    }
    return NULL;
}

void fn_8_19480(struct Mta **mta, char *file, struct Skeleton **skel)
{
    struct DiscFileInfo info;
    u32 size;

    DVDOpen(file, &info);
    size = OSRoundUp32B(info.length);
    *mta = alloc_from_heap_or_panic(currentHeap, size, "newmotion_test.c", 1271);
    read_entire_file_using_dvdread_prio_async(&info, *mta, size, 0);
    DVDClose(&info);
    g_init_mta(*skel, *mta);
    g_init_skeleton_animation(*skel, 0, 0);
}

void fn_8_19530(void **data, char *file, struct Skeleton **skel)
{
    struct DiscFileInfo info;
    u32 size;
    u16 size1;
    u32 size2;

    DVDOpen(file, &info);
    size = OSRoundUp32B(info.length);
    *data = alloc_from_heap_or_panic(currentHeap, size, "newmotion_test.c", 1288);
    read_entire_file_using_dvdread_prio_async(&info, *data, size, 0);
    DVDClose(&info);
    size1 = *(u16 *)*data;
    size2 = size1 * sizeof(struct Bone) + sizeof(struct Skeleton);
    *skel = alloc_from_heap_or_panic(currentHeap, size2, "newmotion_test.c", 1292);
    init_component_state(*skel, (struct Bone *)(*skel + 1), *data);
}

static u32 lbl_AC54[] = { 9, 1, 3, 2 };
static u32 lbl_AC64[] = { 7, 5, 7, 6 };
static u32 lbl_AC74[] = { 1, 1, 3, 2 };
static u32 lbl_AC84[] = { 7, 1, 3, 2 };
static u32 lbl_AC94[] = { 10, 1, 3, 2 };
static u32 *lbl_ACA4[13];
static u32 *lbl_ACA4[13] = { (u32 *)0x4, (u32 *)0x9, (u32 *)0xb, (u32 *)0xa, lbl_AC54, lbl_AC64, (u32 *)0x0, (u32 *)0x0, lbl_AC94, (u32 *)lbl_ACA4, lbl_AC74, lbl_AC84, (u32 *)0x0 };
