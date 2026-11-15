/* Monkey Golf 2 (.text 0x1D260-0x312F4). */
#include "golf.h"

/* TU data (.rodata 0x810-0xF30, .data 0x10A0-0x4E48, .bss 0x2AB8-0x2B00).
 * Statics are addressed off the section start by the code, so order and sizes are load-bearing.
 * .rodata after 0x8E0 (float literals, local initializers) and .data after 0x4A68 (string literals,
 * jump tables) come from the functions themselves. */

/* ---- .rodata 0x810: animation frame lists (u16 sprite indices), pointed to by the tables below ---- */
static const u16 lbl_11_rodata_810[4] = { 0x0F, 0x48, 0x48, 0x48 };
static const u16 lbl_11_rodata_818[9] = { 0x20, 0x20, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31 };
static const u16 lbl_11_rodata_82C[8] = { 0x33, 0x33, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41 };
static const u16 lbl_11_rodata_83C[5] = { 0x03, 0x5D, 0x5D, 0x5D, 0x5D };
static const u16 lbl_11_rodata_848[4] = { 0x0F, 0x12, 0x12, 0x12 };
static const u16 lbl_11_rodata_850[9] = { 0x20, 0x20, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23 };
static const u16 lbl_11_rodata_864[8] = { 0x33, 0x33, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36 };
static const u16 lbl_11_rodata_874[5] = { 0x03, 0x08, 0x08, 0x08, 0x08 };
static const u16 lbl_11_rodata_880[15] = { 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x19, 0x19, 0x0F, 0x0F, 0x19, 0x0F, 0x0F };
static const u16 lbl_11_rodata_8A0[11] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x29, 0x29, 0x20 };
static const u16 lbl_11_rodata_8B8[9] = { 0x33, 0x33, 0x33, 0x33, 0x3B, 0x3B, 0x33, 0x33, 0x33 };
static const u16 lbl_11_rodata_8CC[9] = { 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x4F, 0x4F };

/* ---- .data 0x10A0 ---- */
/* Common header blob present in every minigame REL (unused here). */
static u8 lbl_11_data_10A0[0x170] = {
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
/* .data 0x1210 */
static s32 lbl_11_data_1210[5] = { 0, 2, 4, 6, 8 };
/* .data 0x1224 */
static struct SpriteDrawRequest lbl_11_data_1224 = { 0x1000, 0.0f, 0.0f, 0.05f, 0.5f, 0.14285715f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
/* .data 0x1274 */
static struct SpriteDrawRequest lbl_11_data_1274 = { 0x1007, 0.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 24576, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_12C4 = { 0x1002, 82.0f, 382.0f, 0.13f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
/* club icons, indexed by club */
static struct SpriteDrawRequest lbl_11_data_1314[4] = {
    /* 0 0x1314 */ { 0x1004, 82.0f, 382.0f, 0.125f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /* 1 0x1364 */ { 0x1006, 82.0f, 382.0f, 0.125f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /* 2 0x13B4 */ { 0x1003, 82.0f, 382.0f, 0.125f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /* 3 0x1404 */ { 0x1005, 82.0f, 382.0f, 0.125f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
};
/* .data 0x1454-0x3614: scorecard and HUD sprite records. Separate objects where fn_11_252A8 addresses
 * each one off the pool base; arrays where it indexes (per-player tags, the two digit sets). */
static struct SpriteDrawRequest lbl_11_data_1454[4] = {
    /*  0 0x1454 */ { 0x100D, 109.0f, 388.0f, 0.11f, 0.25f, 0.25f, 0.0f, 0.5f, 0.25f, 0.75f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  1 0x14A4 */ { 0x100D, 109.0f, 388.0f, 0.11f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  2 0x14F4 */ { 0x100D, 109.0f, 388.0f, 0.11f, 0.25f, 0.25f, 0.5f, 0.75f, 0.75f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  3 0x1544 */ { 0x100D, 109.0f, 388.0f, 0.11f, 0.25f, 0.25f, 0.75f, 0.0f, 1.0f, 0.25f, 0, 1.0f, -1, 0x5, -1, 0 },
};
static struct SpriteDrawRequest lbl_11_data_1594[6] = {
    /*   0 0x1594 */ { 0x101C, 148.0f, 427.0f, 0.16f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   1 0x15E4 */ { 0x1012, 245.0f, 432.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   2 0x1634 */ { 0x1014, 158.0f, 431.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   3 0x1684 */ { 0x1013, 158.0f, 431.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   4 0x16D4 */ { 0x1011, 158.0f, 431.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   5 0x1724 */ { 0x100E, 158.0f, 431.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
};
/* separate objects: fn_11_311C0 (0x1774-0x1864) and fn_11_1E794 (0x18B4, 0x1904) pool them */
static struct SpriteDrawRequest lbl_11_data_1774 = { 0x1016, 242.0f, 433.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_17C4 = { 0x1017, 242.0f, 433.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1814 = { 0x1018, 242.0f, 433.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1864 = { 0x1019, 242.0f, 433.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_18B4 = { 0x1001, 90.0f, 402.0f, 0.12f, 0.421875f, 0.28125f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1904 = { 0x102D, 0.0f, 400.0f, 0.12f, 80.0f, 6.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1954[14] = {
    /*   0 0x1954 */ { 0x102D, 320.0f, 12.0f, 0.095f, 120.0f, 6.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x6, -1, 0 },
    /*   1 0x19A4 */ { 0x102F, 36.0f, 25.0f, 0.09f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   2 0x19F4 */ { 0x1030, 301.0f, 35.0f, 0.09f, 1.0f, 0.33333334f, 0.0f, 0.0f, 1.0f, 0.33333334f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   3 0x1A44 */ { 0x1030, 308.0f, 35.0f, 0.09f, 1.0f, 0.33333334f, 0.0f, 0.0f, 1.0f, 0.33333334f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   4 0x1A94 */ { 0x1030, 313.0f, 35.0f, 0.09f, 0.8421f, 0.33333334f, 0.0f, 0.33333334f, 0.8421f, 0.6666667f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   5 0x1AE4 */ { 0x1030, 320.0f, 35.0f, 0.09f, 0.8421f, 0.33333334f, 0.0f, 0.33333334f, 0.8421f, 0.6666667f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   6 0x1B34 */ { 0x1030, 381.0f, 35.0f, 0.09f, 0.63157f, 0.33333334f, 0.0f, 0.6666667f, 0.63157f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   7 0x1B84 */ { 0x1030, 388.0f, 35.0f, 0.09f, 0.63157f, 0.33333334f, 0.0f, 0.6666667f, 0.63157f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   8 0x1BD4 */ { 0x102C, 436.0f, 37.0f, 0.09f, 0.88636f, 0.5f, 0.0f, 0.0f, 0.88636f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   9 0x1C24 */ { 0x102C, 443.0f, 37.0f, 0.09f, 0.88636f, 0.5f, 0.0f, 0.0f, 0.88636f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  10 0x1C74 */ { 0x102C, 519.0f, 37.0f, 0.09f, 0.20454f, 0.5f, 0.0f, 0.5f, 0.20454f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  11 0x1CC4 */ { 0x102C, 525.0f, 37.0f, 0.09f, 0.13636f, 0.5f, 0.20454f, 0.5f, 0.3409f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  12 0x1D14 */ { 0x102C, 540.0f, 37.0f, 0.09f, 0.63636f, 0.5f, 0.3409f, 0.5f, 0.97727f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  13 0x1D64 */ { 0x102C, 598.0f, 37.0f, 0.09f, 0.06818f, 0.5f, 0.0f, 0.0f, 0.06818f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 },
};
static struct SpriteDrawRequest lbl_11_data_1DB4 = { 0x1036, 50.0f, 176.0f, 0.09f, 0.2987805f, 0.63461536f, 0.0f, 0.0f, 0.2987805f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1E04 = { 0x1036, 50.0f, 76.0f, 0.09f, 0.2987805f, 0.63461536f, 0.0f, 0.0f, 0.2987805f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1E54 = { 0x1036, 50.0f, 274.0f, 0.09f, 0.2987805f, 0.63461536f, 0.0f, 0.0f, 0.2987805f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1EA4 = { 0x1036, 66.0f, 130.0f, 0.09f, 0.2987805f, 0.63461536f, 0.0f, 0.0f, 0.2987805f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1EF4 = { 0x1036, 66.0f, 68.0f, 0.09f, 0.2987805f, 0.63461536f, 0.0f, 0.0f, 0.2987805f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1F44 = { 0x1036, 66.0f, 204.0f, 0.09f, 0.2987805f, 0.63461536f, 0.0f, 0.0f, 0.2987805f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1F94 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.09146342f, 0.63461536f, 0.2987805f, 0.0f, 0.3902439f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_1FE4 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.09146342f, 0.63461536f, 0.3902439f, 0.0f, 0.4817073f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2034 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.20121951f, 0.63461536f, 0.4817073f, 0.0f, 0.68292683f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2084 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.30487806f, 0.63461536f, 0.68292683f, 0.0f, 0.9878049f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_20D4 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.0121951215f, 0.63461536f, 0.9878049f, 0.0f, 1.0f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2124 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.2987805f, 0.28846154f, 0.0f, 0.63461536f, 0.2987805f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2174 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.09146342f, 0.28846154f, 0.2987805f, 0.63461536f, 0.3902439f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_21C4 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.09146342f, 0.28846154f, 0.3902439f, 0.63461536f, 0.4817073f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2214 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.20121951f, 0.28846154f, 0.4817073f, 0.63461536f, 0.68292683f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2264 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.30487806f, 0.28846154f, 0.68292683f, 0.63461536f, 0.9878049f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_22B4 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.0121951215f, 0.28846154f, 0.9878049f, 0.63461536f, 1.0f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2304 = { 0x1036, 50.0f, 176.0f, 0.09f, 0.2987805f, 0.03846154f, 0.0f, 0.9230769f, 0.2987805f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2354 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.09146342f, 0.03846154f, 0.2987805f, 0.9230769f, 0.3902439f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_23A4 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.09146342f, 0.03846154f, 0.3902439f, 0.9230769f, 0.4817073f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_23F4 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.20121951f, 0.03846154f, 0.4817073f, 0.9230769f, 0.68292683f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2444 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.30487806f, 0.03846154f, 0.68292683f, 0.9230769f, 0.9878049f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2494 = { 0x1036, 0.0f, 0.0f, 0.09f, 0.0121951215f, 0.03846154f, 0.9878049f, 0.9230769f, 1.0f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_24E4 = { 0x1038, 66.0f, 278.0f, 0.09f, 0.5833333f, 0.63461536f, 0.0f, 0.0f, 0.5833333f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2534 = { 0x1038, 66.0f, 340.0f, 0.09f, 0.5833333f, 0.63461536f, 0.0f, 0.0f, 0.5833333f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2584 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.17857143f, 0.63461536f, 0.5833333f, 0.0f, 0.7619048f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_25D4 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.17857143f, 0.63461536f, 0.7619048f, 0.0f, 0.9404762f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2624 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.023809524f, 0.63461536f, 0.9404762f, 0.0f, 0.96428573f, 0.63461536f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2674 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.5833333f, 0.28846154f, 0.0f, 0.63461536f, 0.5833333f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_26C4 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.17857143f, 0.28846154f, 0.5833333f, 0.63461536f, 0.7619048f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2714 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.17857143f, 0.28846154f, 0.7619048f, 0.63461536f, 0.9404762f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2764 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.023809524f, 0.28846154f, 0.9404762f, 0.63461536f, 0.96428573f, 0.9230769f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_27B4 = { 0x1038, 66.0f, 278.0f, 0.09f, 0.5833333f, 0.03846154f, 0.0f, 0.9230769f, 0.5833333f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2804 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.17857143f, 0.03846154f, 0.5833333f, 0.9230769f, 0.7619048f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_2854 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.17857143f, 0.03846154f, 0.7619048f, 0.9230769f, 0.9404762f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_28A4 = { 0x1038, 0.0f, 0.0f, 0.09f, 0.023809524f, 0.03846154f, 0.9404762f, 0.9230769f, 0.96428573f, 0.96153843f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_28F4 = { 0x102D, 0.0f, 0.0f, 0.08f, 3.75f, 3.75f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.5f, -1, 0x5, 0, 0 };
static struct SpriteDrawRequest lbl_11_data_2944[10] = {
    /*  0 0x2944 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.0f, 0.0f, 0.2f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  1 0x2994 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.2f, 0.0f, 0.4f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  2 0x29E4 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.4f, 0.0f, 0.6f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  3 0x2A34 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.6f, 0.0f, 0.8f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  4 0x2A84 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.8f, 0.0f, 1.0f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  5 0x2AD4 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.0f, 0.5f, 0.2f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  6 0x2B24 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.2f, 0.5f, 0.4f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  7 0x2B74 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.4f, 0.5f, 0.6f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  8 0x2BC4 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.6f, 0.5f, 0.8f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  9 0x2C14 */ { 0x1031, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.8f, 0.5f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
};
static struct SpriteDrawRequest lbl_11_data_2C64[10] = {
    /*  0 0x2C64 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.0f, 0.0f, 0.2f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  1 0x2CB4 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.2f, 0.0f, 0.4f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  2 0x2D04 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.4f, 0.0f, 0.6f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  3 0x2D54 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.6f, 0.0f, 0.8f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  4 0x2DA4 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.8f, 0.0f, 1.0f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  5 0x2DF4 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.0f, 0.5f, 0.2f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  6 0x2E44 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.2f, 0.5f, 0.4f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  7 0x2E94 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.4f, 0.5f, 0.6f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  8 0x2EE4 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.6f, 0.5f, 0.8f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /*  9 0x2F34 */ { 0x1032, 0.0f, 0.0f, 0.07f, 0.2f, 0.5f, 0.8f, 0.5f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
};
static struct SpriteDrawRequest lbl_11_data_2F84 = { 0x1035, 0.0f, 0.0f, 0.07f, 0.95652175f, 0.5f, 0.0f, 0.0f, 0.95652175f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_2FD4 = { 0x1035, 0.0f, 0.0f, 0.07f, 0.32608697f, 0.5f, 0.0f, 0.5f, 0.32608697f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_3024 = { 0x1035, 0.0f, 0.0f, 0.07f, 0.67391306f, 0.5f, 0.32608697f, 0.5f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_3074 = { 0x1034, 0.0f, 0.0f, 0.07f, 1.0f, 0.33333334f, 0.0f, 0.0f, 1.0f, 0.33333334f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_30C4 = { 0x1034, 0.0f, 0.0f, 0.07f, 1.0f, 0.33333334f, 0.0f, 0.33333334f, 1.0f, 0.6666667f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_3114 = { 0x1034, 0.0f, 0.0f, 0.07f, 1.0f, 0.33333334f, 0.0f, 0.6666667f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_3164 = { 0x1033, 0.0f, 0.0f, 0.07f, 1.0f, 0.33333334f, 0.0f, 0.0f, 1.0f, 0.33333334f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_31B4 = { 0x1033, 0.0f, 0.0f, 0.07f, 1.0f, 0.33333334f, 0.0f, 0.33333334f, 1.0f, 0.6666667f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_3204 = { 0x1033, 0.0f, 0.0f, 0.07f, 1.0f, 0.33333334f, 0.0f, 0.6666667f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 };
static struct SpriteDrawRequest lbl_11_data_3254[4] = {
    /*  0 0x3254 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.3560606f, 0.25f, 0.0f, 0.5f, 0.3560606f, 0.75f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  1 0x32A4 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.3560606f, 0.25f, 0.0f, 0.25f, 0.3560606f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  2 0x32F4 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.3560606f, 0.25f, 0.0f, 0.75f, 0.3560606f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  3 0x3344 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.3560606f, 0.25f, 0.0f, 0.0f, 0.3560606f, 0.25f, 0, 1.0f, -1, 0x5, -1, 0 },
};
static struct SpriteDrawRequest lbl_11_data_3394[4] = {
    /*  0 0x3394 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.25f, 0.25f, 0.3560606f, 0.5f, 0.6060606f, 0.75f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  1 0x33E4 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.25f, 0.25f, 0.3560606f, 0.25f, 0.6060606f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  2 0x3434 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.25f, 0.25f, 0.3560606f, 0.75f, 0.6060606f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  3 0x3484 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.25f, 0.25f, 0.3560606f, 0.0f, 0.6060606f, 0.25f, 0, 1.0f, -1, 0x5, -1, 0 },
};
static struct SpriteDrawRequest lbl_11_data_34D4[4] = {
    /*  0 0x34D4 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.3939394f, 0.25f, 0.6060606f, 0.5f, 1.0f, 0.75f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  1 0x3524 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.3939394f, 0.25f, 0.6060606f, 0.25f, 1.0f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  2 0x3574 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.3939394f, 0.25f, 0.6060606f, 0.75f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*  3 0x35C4 */ { 0x1037, 0.0f, 0.0f, 0.08f, 0.3939394f, 0.25f, 0.6060606f, 0.0f, 1.0f, 0.25f, 0, 1.0f, -1, 0x5, -1, 0 },
};
/* .data 0x3614-0x4794: sprite records. Separate objects: fn_11_20858 addresses 0x3754, 0x3AC4 (and its [1]),
 * 0x3B64, 0x3BB4, 0x3C04, 0x3C54, 0x3CA4 each as an object start off the pool base. */
/* .data 0x3614 */
static struct SpriteDrawRequest lbl_11_data_3614[4] = {
    /*   0 0x3614 */ { 0xF, 0.0f, 0.0f, 0.06f, 0.3359375f, 0.21875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   1 0x3664 */ { 0x20, 0.0f, 0.0f, 0.06f, 0.3359375f, 0.21875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   2 0x36B4 */ { 0x33, 0.0f, 0.0f, 0.06f, 0.3359375f, 0.21875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   3 0x3704 */ { 0x3, 0.0f, 0.0f, 0.06f, 0.3359375f, 0.21875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
};
/* .data 0x3754: digits 0-9 (shot-info panel and score digits) */
static struct SpriteDrawRequest lbl_11_data_3754[10] = {
    /*   0 0x3754 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.0f, 0.0f, 0.2f, 0.33333334f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   1 0x37A4 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.2f, 0.0f, 0.4f, 0.33333334f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   2 0x37F4 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.4f, 0.0f, 0.6f, 0.33333334f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   3 0x3844 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.6f, 0.0f, 0.8f, 0.33333334f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   4 0x3894 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.8f, 0.0f, 1.0f, 0.33333334f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   5 0x38E4 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.0f, 0.33333334f, 0.2f, 0.6666667f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   6 0x3934 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.2f, 0.33333334f, 0.4f, 0.6666667f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   7 0x3984 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.4f, 0.33333334f, 0.6f, 0.6666667f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   8 0x39D4 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.6f, 0.33333334f, 0.8f, 0.6666667f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   9 0x3A24 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.8f, 0.33333334f, 1.0f, 0.6666667f, 0, 1.0f, -1, 0x5, -1, 0 },
};
/* .data 0x3A74 */
static struct SpriteDrawRequest lbl_11_data_3A74 = { 0x1029, 0.0f, 0.0f, 0.06f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.5f, -1, 0x5, -1, 0 };
/* .data 0x3AC4: '+', '-' */
static struct SpriteDrawRequest lbl_11_data_3AC4[2] = {
    /*   0 0x3AC4 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.0f, 0.6666667f, 0.2f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    /*   1 0x3B14 */ { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.2f, 0.6666667f, 0.4f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
};
/* .data 0x3B64: distance unit when lbl_11_bss_7C is set */
static struct SpriteDrawRequest lbl_11_data_3B64 = { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.4f, 0.6666667f, 0.6f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
/* .data 0x3BB4: distance unit otherwise */
static struct SpriteDrawRequest lbl_11_data_3BB4 = { 0x100B, 0.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
/* .data 0x3C04: '.' */
static struct SpriteDrawRequest lbl_11_data_3C04 = { 0x1009, 0.0f, 0.0f, 0.1f, 0.2f, 0.33333334f, 0.6f, 0.6666667f, 0.8f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
/* .data 0x3C54: shot-info panel: label of the distance line */
static struct SpriteDrawRequest lbl_11_data_3C54 = { 0x1008, 252.0f, 22.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
/* .data 0x3CA4: shot-info panel: label of the height line */
static struct SpriteDrawRequest lbl_11_data_3CA4 = { 0x100A, 252.0f, 46.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
/* .data 0x3CF4 */
/* 0x3CF4-0x4794 (fn_11_1FD20, fn_11_1FA7C, fn_11_1F7FC pool these as separate objects) */
static struct SpriteDrawRequest lbl_11_data_3CF4 = { 0x1025, 0.0f, 0.0f, 0.05f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_3D44 = { 0x1026, 0.0f, 0.0f, 0.05f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_3D94 = { 0x1027, 0.0f, 0.0f, 0.05f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_3DE4 = { 0x1028, 0.0f, 0.0f, 0.05f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_3E34 = { 0x102B, 0.0f, 0.0f, 0.05f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_3E84 = { 0x1039, 0.0f, 0.0f, 0.05f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_3ED4 = { 0x102E, 0.0f, 0.0f, 0.05f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_3F24 = { 0x1024, 24.0f, 12.0f, 0.13f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_3F74 = { 0x103D, 27.0f, 70.0f, 0.12f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFFFBE102, 0 };
static struct SpriteDrawRequest lbl_11_data_3FC4 = { 0x1022, 83.0f, 70.0f, 0.12f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFFFB5302, 0 };
/* digit sprites 0-9 */
static struct SpriteDrawRequest lbl_11_data_4014[10] = {
    /* 0 0x4014 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.0f, 0.0f, 0.2f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 1 0x4064 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.2f, 0.0f, 0.4f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 2 0x40B4 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.4f, 0.0f, 0.6f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 3 0x4104 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.6f, 0.0f, 0.8f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 4 0x4154 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.8f, 0.0f, 1.0f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 5 0x41A4 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.0f, 0.5f, 0.2f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 6 0x41F4 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.2f, 0.5f, 0.4f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 7 0x4244 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.4f, 0.5f, 0.6f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 8 0x4294 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.6f, 0.5f, 0.8f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
    /* 9 0x42E4 */ { 0x1021, 34.0f, 15.0f, 0.11f, 0.2f, 0.5f, 0.8f, 0.5f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5C3B1B, 0 },
};
static struct SpriteDrawRequest lbl_11_data_4334 = { 0x101F, 496.0f, 0.0f, 0.13f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_4384 = { 0x1020, 504.0f, 138.0f, 0.12f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_43D4 = { 0x102D, 497.0f, 0.0f, 1.1f, 20.0f, 17.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.5f, -1, 0x40005, 0, 0 };
static struct SpriteDrawRequest lbl_11_data_4424 = { 0x101D, 504.0f, 138.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 };
/* digit sprites 0-9 */
static struct SpriteDrawRequest lbl_11_data_4474[10] = {
    /* 0 0x4474 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.0f, 0.0f, 0.2f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 1 0x44C4 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.2f, 0.0f, 0.4f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 2 0x4514 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.4f, 0.0f, 0.6f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 3 0x4564 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.6f, 0.0f, 0.8f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 4 0x45B4 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.8f, 0.0f, 1.0f, 0.5f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 5 0x4604 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.0f, 0.5f, 0.2f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 6 0x4654 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.2f, 0.5f, 0.4f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 7 0x46A4 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.4f, 0.5f, 0.6f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 8 0x46F4 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.6f, 0.5f, 0.8f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
    /* 9 0x4744 */ { 0x101E, 504.0f, 138.0f, 0.11f, 0.2f, 0.5f, 0.8f, 0.5f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF2238A7, 0 },
};
/* .data 0x4794 */
static struct SpriteDrawRequest lbl_11_data_4794 = { 0x100C, 0.0f, 0.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_47E4 = { 0x4A, 0.0f, 0.0f, 0.12f, 0.125f, 0.375f, 0.125f, 0.125f, 0.25f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_4834 = { 0x4A, 0.0f, 0.0f, 0.12f, 0.1875f, 0.375f, 0.25f, 0.125f, 0.375f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 };
static struct SpriteDrawRequest lbl_11_data_4884 = { 0x4A, 0.0f, 0.0f, 0.12f, 0.125f, 0.375f, 0.375f, 0.125f, 0.5f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 };
/* .data 0x48D4: per-character values, [character][0] is a sprite bmpId */
static u16 lbl_11_data_48D4[4] = { 0x48, 0x0F, 0x17, 0x12 };
static u16 lbl_11_data_48DC[4] = { 0x31, 0x20, 0x27, 0x23 };
static u16 lbl_11_data_48E4[4] = { 0x41, 0x33, 0x39, 0x36 };
static u16 lbl_11_data_48EC[4] = { 0x5D, 0x03, 0x4D, 0x08 };
/* .data 0x48F4 */
static struct SpriteDrawRequest lbl_11_data_48F4 = { 0xF, 0.0f, 0.0f, 1.0f, 0.3359375f, 0.21875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
/* .data 0x4944 */
static struct SpriteDrawRequest lbl_11_data_4944 = { 0xF, 0.0f, 0.0f, 1.0f, 0.3359375f, 0.21875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
/* .data 0x4994 */
static struct SpriteDrawRequest lbl_11_data_4994 = { 0xF, 0.0f, 0.0f, 1.0f, 0.3359375f, 0.21875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
/* .data 0x49E4 */
static struct GolfFrameList lbl_11_data_49E4[4] = {
    { lbl_11_rodata_810, 4 },
    { lbl_11_rodata_818, 9 },
    { lbl_11_rodata_82C, 8 },
    { lbl_11_rodata_83C, 5 },
};
/* .data 0x4A04 */
static struct GolfFrameList lbl_11_data_4A04[4] = {
    { lbl_11_rodata_848, 4 },
    { lbl_11_rodata_850, 9 },
    { lbl_11_rodata_864, 8 },
    { lbl_11_rodata_874, 5 },
};
/* .data 0x4A24 */
static struct GolfFrameList lbl_11_data_4A24[4] = {
    { lbl_11_rodata_880, 15 },
    { lbl_11_rodata_8A0, 11 },
    { lbl_11_rodata_8B8, 9 },
    { lbl_11_rodata_8CC, 9 },
};
/* .data 0x4A44: colours (0x00RRGGBB) */
static u32 lbl_11_data_4A44[9] = { 0x00CDFA00, 0x00E5F200, 0x00FBE102, 0x00F6C307, 0x00F1A70C, 0x00EF8D10, 0x00EB7114, 0x00E95E16, 0x00E74B1A };





#include "golf2/golf_4_1D260.inc"  /* .text 0x1D260-0x20858 */
#include "golf2/golf_4_20858.inc"  /* .text 0x20858-0x252A8 */
#include "golf2/golf_4_252A8.inc"  /* .text 0x252A8-0x2DBC0 */
#include "golf2/golf_4_2DBC0.inc"  /* .text 0x2DBC0-0x312F4 */

/* ---- .bss 0x2AB8 (globals: golf.c and golf_3.c use some of them).
 * Defined after the functions that use them: MWCC then lays them out in reverse definition order
 * (defined before their first use they would follow first-use order instead). ---- */
Vec lbl_11_bss_2AD0[4];
u32 lbl_11_bss_2ACC;
s16 lbl_11_bss_2AC8;
u32 lbl_11_bss_2AC4;  /* unreferenced */
u32 lbl_11_bss_2AC0;
