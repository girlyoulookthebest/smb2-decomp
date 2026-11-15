/* Monkey Golf 2: module entry, game modes and per-frame logic (.text 0x0-0x171E8).
 * Original file name (OSPanic): golf2_mode.c */
#include "golf.h"

/* ---- .bss 0x0-0x29F8 ---- */

static s32 lbl_11_bss_0;                             /* 0x0000 unused */
static s32 lbl_11_bss_4;                             /* 0x0004 */
static s32 lbl_11_bss_8;                             /* 0x0008 frame counter */
static s32 lbl_11_bss_C;                             /* 0x000C */
static s32 lbl_11_bss_10;                            /* 0x0010 */
static u8 lbl_11_bss_14;                             /* 0x0014 */
static s32 lbl_11_bss_18;                            /* 0x0018 */
static s8 lbl_11_bss_1C;                             /* 0x001C */
static struct Ball *lbl_11_bss_20;                   /* 0x0020 current player ball */
struct Camera *lbl_11_bss_24;                        /* 0x0024 current player camera */
static struct Camera *lbl_11_bss_28;                 /* 0x0028 second camera */
static s16 lbl_11_bss_2C;                            /* 0x002C first hole of the course */
static s16 lbl_11_bss_2E;                            /* 0x002E last hole of the course */
static u32 lbl_11_bss_30;                            /* 0x0030 main state */
static s16 lbl_11_bss_34;                            /* 0x0034 */
Vec lbl_11_bss_38;                                   /* 0x0038 cup position */
static s8 lbl_11_bss_44;                             /* 0x0044 */
static u8 lbl_11_bss_45;                             /* 0x0045 */
static u8 lbl_11_bss_46;                             /* 0x0046 */
static u8 lbl_11_bss_47;                             /* 0x0047 */
static u8 lbl_11_bss_48;                             /* 0x0048 */
static u8 lbl_11_bss_49;                             /* 0x0049 */
static u8 lbl_11_bss_4A;                             /* 0x004A */
s32 lbl_11_bss_4C;                                   /* 0x004C selected club (0-14) */
s32 lbl_11_bss_50;                                   /* 0x0050 */
static s32 lbl_11_bss_54;                            /* 0x0054 */
float *lbl_11_bss_58;                                /* 0x0058 -> lbl_11_data_2FC[club][lie] */
f32 lbl_11_bss_5C;                                   /* 0x005C */
static s32 lbl_11_bss_60;                            /* 0x0060 trail point count (lbl_11_bss_198, lbl_11_bss_CBC) */
s32 golf_wind_angle;                                 /* 0x0064 */
f32 golf_wind_velocity;                              /* 0x0068 */
s32 lbl_11_bss_6C;                                   /* 0x006C */
s32 lbl_11_bss_70;                                   /* 0x0070 */
s32 lbl_11_bss_74;                                   /* 0x0074 */
s32 lbl_11_bss_78;                                   /* 0x0078 */
s8 lbl_11_bss_7C;                                    /* 0x007C */
s32 lbl_11_bss_80;                                   /* 0x0080 */
s32 lbl_11_bss_84;                                   /* 0x0084 */
static s32 lbl_11_bss_88;                            /* 0x0088 */
static s32 lbl_11_bss_8C;                            /* 0x008C */
static u8 lbl_11_bss_90;                             /* 0x0090 */
s32 lbl_11_bss_94;                                   /* 0x0094 */
static Mtx lbl_11_bss_98;                            /* 0x0098 */
static s16 lbl_11_bss_C8;                            /* 0x00C8 current hole (0-17) */
s8 lbl_11_bss_CA;                                    /* 0x00CA ball lie (0-3) */
static s16 lbl_11_bss_CC;                            /* 0x00CC */
s32 lbl_11_bss_D0;                                   /* 0x00D0 */
f32 lbl_11_bss_D4;                                   /* 0x00D4 */
f32 lbl_11_bss_D8;                                   /* 0x00D8 */
static s8 lbl_11_bss_DC;                             /* 0x00DC */
static u16 lbl_11_bss_DE;                            /* 0x00DE */
static s32 lbl_11_bss_E0;                            /* 0x00E0 */
static u32 lbl_11_bss_E4;                            /* 0x00E4 */
static u8 lbl_11_bss_E8;                             /* 0x00E8 */
static struct GolfScoreEC lbl_11_bss_EC[4];          /* 0x00EC per player */
static struct GolfScore11C lbl_11_bss_11C[4];        /* 0x011C per player */
static u32 lbl_11_bss_13C;                           /* 0x013C */
static u32 lbl_11_bss_140;                           /* 0x0140 */
static u8 lbl_11_bss_144;                            /* 0x0144 */
static u8 lbl_11_bss_145;                            /* 0x0145 */
static Vec lbl_11_bss_148;                           /* 0x0148 */
static Vec lbl_11_bss_154;                           /* 0x0154 */
static f32 lbl_11_bss_160;                           /* 0x0160 */
static u32 lbl_11_bss_164;                           /* 0x0164 */
static f32 lbl_11_bss_168;                           /* 0x0168 */
static u8 lbl_11_bss_16C;                            /* 0x016C */
static Vec lbl_11_bss_170;                           /* 0x0170 */
static u8 lbl_11_bss_17C;                            /* 0x017C */
f32 lbl_11_bss_180;                                  /* 0x0180 */
f32 lbl_11_bss_184;                                  /* 0x0184 */
static u8 lbl_11_bss_188;                            /* 0x0188 */
Vec lbl_11_bss_18C;                                  /* 0x018C */
Vec lbl_11_bss_198[200];                             /* 0x0198 shot trail points (lbl_11_bss_60 used, max 50) */
static s32 lbl_11_bss_AF8;                           /* 0x0AF8 */
static s32 lbl_11_bss_AFC;                           /* 0x0AFC */
static s32 lbl_11_bss_B00;                           /* 0x0B00 */
static u32 lbl_11_bss_B04;                           /* 0x0B04 */
static s8 lbl_11_bss_B08;                            /* 0x0B08 */
static f32 lbl_11_bss_B0C;                           /* 0x0B0C */
static s16 lbl_11_bss_B10;                           /* 0x0B10 */
static s16 lbl_11_bss_B12;                           /* 0x0B12 */
static s8 lbl_11_bss_B14;                            /* 0x0B14 */
s32 lbl_11_bss_B18;                                  /* 0x0B18 */
static f32 lbl_11_bss_B1C;                           /* 0x0B1C */
static f32 lbl_11_bss_B20;                           /* 0x0B20 */
static s16 lbl_11_bss_B24;                           /* 0x0B24 */
static Vec lbl_11_bss_B28;                           /* 0x0B28 */
static s32 lbl_11_bss_B34;                           /* 0x0B34 */
s32 lbl_11_bss_B38;                                  /* 0x0B38 */
static f32 lbl_11_bss_B3C;                           /* 0x0B3C */
s32 lbl_11_bss_B40;                                  /* 0x0B40 */
static Vec lbl_11_bss_B44;                           /* 0x0B44 */
static u16 lbl_11_bss_B50;                           /* 0x0B50 */
static Vec lbl_11_bss_B54[30];                       /* 0x0B54 spline control points (fn_11_15FB4) */
static Vec lbl_11_bss_CBC[50];                       /* 0x0CBC second trail (lbl_11_bss_60 used) */
s32 lbl_11_bss_F14;                                  /* 0x0F14 frame stamp */
static u8 lbl_11_bss_F18[0x1900];                    /* 0x0F18 unreferenced */
static void *lbl_11_bss_2818;                        /* 0x2818 texture buffer (fn_11_13AC4) */
static GXTexObj lbl_11_bss_281C;                     /* 0x281C */
static u8 lbl_11_bss_283C[0x1A8];                    /* 0x283C unreferenced */
Vec lbl_11_bss_29E4;                                 /* 0x29E4 */
f32 lbl_11_bss_29F0;                                 /* 0x29F0 */
s32 lbl_11_bss_29F4;                                 /* 0x29F4 used only by golf_1.c (whose .bss pool starts at 0x29F8) */

/* ---- .data 0x0-0x434 (string literals and jump tables follow, emitted by the code) ---- */

/* unreferenced */
static u8 lbl_11_data_0[0x28] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF,
};
/* unreferenced */
static struct GolfData28 lbl_11_data_28[41] = {
    { 0x31000000, 0, 0 },
    { 0x3800B400, 1, 0 },
    { 0x01000000, 2, 0 },
    { 0x10004C00, 3, 0 },
    { 0x01000000, 4, 0 },
    { 0x18000000, 9, 0 },
    { 0x40000000, 0, 0 },
    { 0x01000000, 11, 0 },
    { 0x18000000, 12, 0 },
    { 0x18000000, 5, 0 },
    { 0x40000000, 0, 0 },
    { 0x01000000, 7, 0 },
    { 0x18000000, 8, 0 },
    { 0x3800B400, 13, 0 },
    { 0x40000000, 0, 0 },
    { 0x18000000, 20, 0 },
    { 0x40000000, 0, 0 },
    { 0x01000000, 22, 0 },
    { 0x18000000, 23, 0 },
    { 0x18000000, 24, 0 },
    { 0x18000000, 15, 0 },
    { 0x40000000, 0, 0 },
    { 0x01000000, 17, 0 },
    { 0x18000000, 18, 0 },
    { 0x18000000, 19, 0 },
    { 0x18000000, 0, 0 },
    { 0x18000000, 1, 0 },
    { 0x18000000, 2, 0 },
    { 0x18000000, 3, 0 },
    { 0x18000000, 4, 0 },
    { 0x18000000, 5, 0 },
    { 0x18000000, 6, 0 },
    { 0x18000000, 7, 0 },
    { 0x18000000, 8, 0 },
    { 0x18000000, 9, 0 },
    { 0x18000000, 10, 0 },
    { 0x18000000, 11, 0 },
    { 0x18000000, 12, 0 },
    { 0x18000000, 13, 0 },
    { 0x18000000, 14, 0 },
    { 0x18000000, 15, 0 },
};
static s16 lbl_11_data_170[4] = { 15, 25, 40, 80 };
static s16 lbl_11_data_178[4] = { 10, 20, 40, 80 };
/* per club */
static s16 lbl_11_data_180[16] = {
    0x0937, 0x0A65, 0x0B9B, 0x0C95, 0x0D89, 0x0E14, 0x0EE9, 0x1048,
    0x12B8, 0x1554, 0x1968, 0x1F6E,
};
/* per club */
static f32 lbl_11_data_1A0[15] = {
    2.2f, 2.0f, 1.9f, 1.8f, 1.7f, 1.6f, 1.5f, 1.4f,
    1.3f, 1.2f, 1.1f, 0.9f, 0.5f, 0.3f, 0.15f,
};
static s16 lbl_11_data_1DC[18] = {
    0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
    0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2,
};
static u8 lbl_11_data_200 = 18;
/* stage id per hole */
static s16 lbl_11_data_204[18] = {
    0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
    0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2,
};
/* par per hole */
static s8 lbl_11_data_228[20] = { 4, 4, 5, 3, 4, 4, 5, 3, 4, 5, 4, 3, 4, 4, 4, 3, 4, 5 };
/* stroke limit per hole */
static s8 lbl_11_data_23C[20] = { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 };
static s32 lbl_11_data_250 = -1;
static s8 lbl_11_data_254 = -1;
/* per player */
static u8 lbl_11_data_258[4] = { 0 };
/* per player */
static u8 lbl_11_data_25C[4] = { 0 };
/* model index per club */
static s32 lbl_11_data_260[15] = {
    37, 37, 37, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27,
};
/* strokes per player per hole */
static u8 lbl_11_data_29C[4][18] = { 0 };
static u8 lbl_11_data_2E4[8] = { 0 }; /* per player (0-3), then [4] */
/* power factor per lie */
static f32 lbl_11_data_2EC[4] = { 1.0f, 0.9f, 0.7f, 1.0f };
/* per club, per lie (row 12: lie 3) */
static f32 lbl_11_data_2FC[13][3] = {
    { 0.07f, 0.05f, 0.04f },
    { 0.08f, 0.06f, 0.045f },
    { 0.09f, 0.07f, 0.045f },
    { 0.1f, 0.08f, 0.05f },
    { 0.11f, 0.085f, 0.05f },
    { 0.12f, 0.09f, 0.05f },
    { 0.13f, 0.095f, 0.06f },
    { 0.14f, 0.1f, 0.06f },
    { 0.15f, 0.105f, 0.07f },
    { 0.16f, 0.11f, 0.09f },
    { 0.18f, 0.14f, 0.12f },
    { 0.2f, 0.17f, 0.15f },
    { 0.0f, 0.0f, 0.0f },
};
/* unreferenced */
static Vec lbl_11_data_398[13] = {
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { -4.5f, -1e+01f, -5.0f },
    { -3.0f, -1e+01f, -5.0f },
    { 3.0f, -7.0f, -5.0f },
    { 6.5f, -7.0f, -5.0f },
    { 6.5f, -7.0f, -3.0f },
    { 6.5f, -3.0f, 3.0f },
    { 6.5f, -3.0f, 6.0f },
    { 3.0f, -3.0f, 6.0f },
    { -3.0f, 1.0f, 6.0f },
    { -6.5f, 1.0f, 6.0f },
    { -6.5f, 1.0f, 0.0f },
};

/* keeps the unreferenced .data tables */
void golf_pin_data(void)
{
    void *p;

    p = lbl_11_data_0;
    p = lbl_11_data_28;
    p = lbl_11_data_398;
}

/* MWCC lays out .bss in order of first reference; this (unreferenced, dead-stripped) function
 * fixes that order before any code uses the variables. */
void golf_pin_bss(void)
{
    void *p;

    p = &lbl_11_bss_0;
    p = &lbl_11_bss_4;
    p = &lbl_11_bss_8;
    p = &lbl_11_bss_C;
    p = &lbl_11_bss_10;
    p = &lbl_11_bss_14;
    p = &lbl_11_bss_18;
    p = &lbl_11_bss_1C;
    p = &lbl_11_bss_20;
    p = &lbl_11_bss_24;
    p = &lbl_11_bss_28;
    p = &lbl_11_bss_2C;
    p = &lbl_11_bss_2E;
    p = &lbl_11_bss_30;
    p = &lbl_11_bss_34;
    p = &lbl_11_bss_38;
    p = &lbl_11_bss_44;
    p = &lbl_11_bss_45;
    p = &lbl_11_bss_46;
    p = &lbl_11_bss_47;
    p = &lbl_11_bss_48;
    p = &lbl_11_bss_49;
    p = &lbl_11_bss_4A;
    p = &lbl_11_bss_4C;
    p = &lbl_11_bss_50;
    p = &lbl_11_bss_54;
    p = &lbl_11_bss_58;
    p = &lbl_11_bss_5C;
    p = &lbl_11_bss_60;
    p = &golf_wind_angle;
    p = &golf_wind_velocity;
    p = &lbl_11_bss_6C;
    p = &lbl_11_bss_70;
    p = &lbl_11_bss_74;
    p = &lbl_11_bss_78;
    p = &lbl_11_bss_7C;
    p = &lbl_11_bss_80;
    p = &lbl_11_bss_84;
    p = &lbl_11_bss_88;
    p = &lbl_11_bss_8C;
    p = &lbl_11_bss_90;
    p = &lbl_11_bss_94;
    p = &lbl_11_bss_98;
    p = &lbl_11_bss_C8;
    p = &lbl_11_bss_CA;
    p = &lbl_11_bss_CC;
    p = &lbl_11_bss_D0;
    p = &lbl_11_bss_D4;
    p = &lbl_11_bss_D8;
    p = &lbl_11_bss_DC;
    p = &lbl_11_bss_DE;
    p = &lbl_11_bss_E0;
    p = &lbl_11_bss_E4;
    p = &lbl_11_bss_E8;
    p = lbl_11_bss_EC;
    p = lbl_11_bss_11C;
    p = &lbl_11_bss_13C;
    p = &lbl_11_bss_140;
    p = &lbl_11_bss_144;
    p = &lbl_11_bss_145;
    p = &lbl_11_bss_148;
    p = &lbl_11_bss_154;
    p = &lbl_11_bss_160;
    p = &lbl_11_bss_164;
    p = &lbl_11_bss_168;
    p = &lbl_11_bss_16C;
    p = &lbl_11_bss_170;
    p = &lbl_11_bss_17C;
    p = &lbl_11_bss_180;
    p = &lbl_11_bss_184;
    p = &lbl_11_bss_188;
    p = &lbl_11_bss_18C;
    p = lbl_11_bss_198;
    p = &lbl_11_bss_AF8;
    p = &lbl_11_bss_AFC;
    p = &lbl_11_bss_B00;
    p = &lbl_11_bss_B04;
    p = &lbl_11_bss_B08;
    p = &lbl_11_bss_B0C;
    p = &lbl_11_bss_B10;
    p = &lbl_11_bss_B12;
    p = &lbl_11_bss_B14;
    p = &lbl_11_bss_B18;
    p = &lbl_11_bss_B1C;
    p = &lbl_11_bss_B20;
    p = &lbl_11_bss_B24;
    p = &lbl_11_bss_B28;
    p = &lbl_11_bss_B34;
    p = &lbl_11_bss_B38;
    p = &lbl_11_bss_B3C;
    p = &lbl_11_bss_B40;
    p = &lbl_11_bss_B44;
    p = &lbl_11_bss_B50;
    p = lbl_11_bss_B54;
    p = lbl_11_bss_CBC;
    p = &lbl_11_bss_F14;
    p = lbl_11_bss_F18;
    p = &lbl_11_bss_2818;
    p = &lbl_11_bss_281C;
    p = lbl_11_bss_283C;
    p = &lbl_11_bss_29E4;
    p = &lbl_11_bss_29F0;
    p = &lbl_11_bss_29F4;
}

#include "golf2/golf_0.inc"      /* .text 0x0-0x900 */
#include "golf2/golf_900.inc"    /* .text 0x900-0x9460 */
#include "golf2/golf_9460.inc"   /* .text 0x9460-0xD94C */
#include "golf2/golf_D94C.inc"   /* .text 0xD94C-0x129FC */
#include "golf2/golf_129FC.inc"  /* .text 0x129FC-0x171E8 */
