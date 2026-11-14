/* This unit retains signed 16-bit angles in 32-bit registers between calls. */
#define math_sin math_sin_s16
#include "baseball.h"
#include "avdisp.h"
#include "mathutil.h"
#undef math_sin
extern float math_sin(s32 angle);
#include "sprite.h"
#include "bitmap.h"
#include "pool.h"
#include "stage.h"
#include "sound.h"

struct BBCreateTitles { s32 ids[2]; };
struct BBCreateIds { s32 ids[3]; };
struct BBCreateTeams { s32 ids[6]; };
struct BBCreateHelp { s32 ids[7]; };
struct BBCreateTables {
    struct BBCreateTitles titles;
    struct BBCreateIds characters;
    struct BBCreateIds result;
    struct BBCreateIds message;
    struct BBCreateHelp help39;
    struct BBCreateHelp help38;
    float count_y[2][3];
    struct BBCreateTeams teams;
};

struct BBAnimate3 { s32 values[3]; };
struct BBAnimate4 { s32 values[4]; };
struct BBAnimate5 { s32 values[5]; };
struct BBAnimateTables {
    struct BBAnimate4 winner;
    struct BBAnimate3 banner;
    struct BBAnimate3 change;
    struct BBAnimate3 sounds;
    struct BBAnimate3 result;
    float banner_y[2][3];
    float count_y[2][3];
    struct BBAnimate5 start;
};

static inline void bb_flash_sprite(struct Sprite *sprite, float amount)
{
    u8 color = 255.0f * amount;
    sprite->addR = color;
    sprite->addG = color;
    sprite->addB = color;
    sprite->scaleX = (1.0f + 0.1f * (amount * amount));
    sprite->scaleY = (1.0f + 0.1f * (amount * amount));
}

/* Keep helper emission order for the original literal pool. The linker strips
 * these unreferenced address anchors and their out-of-line helper copies. */
static void *bb_anchor_bb_flash_sprite = (void *)bb_flash_sprite;

static inline void bb_flash_sprite_scaled(struct Sprite *sprite, float amount)
{
    u8 color = 255.0f * amount;
    sprite->addR = color;
    sprite->addG = color;
    sprite->addB = color;
    sprite->scaleX = lbl_18_bss_19274 * (1.0f + 0.1f * (amount * amount));
    sprite->scaleY = lbl_18_bss_19274 * (1.0f + 0.1f * (amount * amount));
}

static char bb_sprite_work_error[] = "!!!!!!!!!! Get Sprite Work ERROR !!!!!!!!!!\n";

static inline struct Sprite **bb_get_sprites(s32 count, s32 start)
{
    s32 i;
    struct Sprite **p;

    i = count;
    p = &lbl_18_bss_19270->unk338[start];
    while (i > 0) {
        if (*p != NULL) {
            g_poolInfo.spritePool.statusList[(*p)->poolIdx] = 0;
        }
        if ((*p = create_sprite()) != NULL) {
            (*p)->type = 1;
            (*p)->textAlign = 4;
            (*p)->text[0] = 0;
            (*p)->flags |= 0x01000000;
            (*p)->widescreenArg = 0x140;
            if (lbl_18_bss_19270->unk0 == 0xF) {
                (*p)->mulR = 0xE5;
                (*p)->mulG = 0xF2;
                (*p)->mulB = 0xF2;
            }
        } else {
            printf(bb_sprite_work_error);
            return NULL;
        }
        i--;
        p++;
    }
    return &lbl_18_bss_19270->unk338[start];
}

static inline struct Sprite **bb_get_sprites_counter(s32 count, s32 start)
{
    struct Sprite **p;
    s32 i;

    i = count;
    p = &lbl_18_bss_19270->unk338[start];
    while (i > 0) {
        if (*p != NULL) {
            g_poolInfo.spritePool.statusList[(*p)->poolIdx] = 0;
        }
        if ((*p = create_sprite()) != NULL) {
            (*p)->type = 1;
            (*p)->textAlign = 4;
            (*p)->text[0] = 0;
            (*p)->flags |= 0x01000000;
            (*p)->widescreenArg = 0x140;
            if (lbl_18_bss_19270->unk0 == 0xF) {
                (*p)->mulR = 0xE5;
                (*p)->mulG = 0xF2;
                (*p)->mulB = 0xF2;
            }
        } else {
            printf(bb_sprite_work_error);
            return NULL;
        }
        i--;
        p++;
    }
    return &lbl_18_bss_19270->unk338[start];
}

static inline struct Sprite **bb_get_sprites_next(s32 count, s32 *next)
{
    s32 i;
    s32 start;
    struct Sprite **p;

    i = count;
    start = (*next)++;
    p = &lbl_18_bss_19270->unk338[start];
    while (i > 0) {
        if (*p != NULL) {
            g_poolInfo.spritePool.statusList[(*p)->poolIdx] = 0;
        }
        if ((*p = create_sprite()) != NULL) {
            (*p)->type = 1;
            (*p)->textAlign = 4;
            (*p)->text[0] = 0;
            (*p)->flags |= 0x01000000;
            (*p)->widescreenArg = 0x140;
            if (lbl_18_bss_19270->unk0 == 0xF) {
                (*p)->mulR = 0xE5;
                (*p)->mulG = 0xF2;
                (*p)->mulB = 0xF2;
            }
        } else {
            printf(bb_sprite_work_error);
            return NULL;
        }
        i--;
        p++;
    }
    return &lbl_18_bss_19270->unk338[start];
}

static inline void bb_free_sprites(s32 count, s32 start)
{
    s32 i;
    struct Sprite **p;

    i = count;
    p = &lbl_18_bss_19270->unk338[start];
    while (i > 0) {
        if (*p != NULL) {
            g_poolInfo.spritePool.statusList[(*p)->poolIdx] = 0;
            *p = NULL;
        }
        i--;
        p++;
    }
}

char *lbl_18_data_7FC64[6][0x21] = {
    {
        "1P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "2P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "COM stops the wheel.  The order of the pockets",
        "and the number of the jump boards change",
        "according to the outcome.",
        "Normal Shift:",
        "A basic shift. You will have three jump boards.",
        "",
        "Homerun Shift:",
        "All jump boards appear, but all pockets become Out",
        "pockets.  You will have all the jump boards.",
        "Right Shift:",
        "Hit pockets move to the right side.  You will have",
        "the jump boards on the right.",
        "Left Shift:",
        "Hit pockets move to the left side.  You will have the",
        "jump boards on the left.",
        "Hit Shift:",
        "All pockets become One Base hits.  You will have",
        "no jump boards.",
        "Center Shift:",
        "Hit pockets move to the center.  You will have jump",
        "boards in the center.",
        "Half Out:",
        "Half of the pockets become Out pockets.  You will",
        "have two jump boards.",
        "Lucky Seven:",
        "All pockets become Hit pockets.",
        "You will have three jump boards.",
    },
    {
        "1P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "2P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "COM stops the wheel.  The order of the pockets",
        "and the number of the jump boards change",
        "according to the outcome.",
        "Normal Shift:",
        "A basic shift. You will have three jump boards.",
        "",
        "Homerun Shift:",
        "All jump boards appear, but all pockets become Out",
        "pockets.  You will have all the jump boards.",
        "Right Shift:",
        "Hit pockets move to the right side.  You will have",
        "the jump boards on the right.",
        "Left Shift:",
        "Hit pockets move to the left side.  You will have the",
        "jump boards on the left.",
        "Hit Shift:",
        "All pockets become One Base hits.  You will have",
        "no jump boards.",
        "Center Shift:",
        "Hit pockets move to the center.  You will have jump",
        "boards in the center.",
        "Half Out:",
        "Half of the pockets become Out pockets.  You will",
        "have two jump boards.",
        "Lucky Seven:",
        "All pockets become Hit pockets.",
        "You will have three jump boards.",
    },
    {
        "1P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "2P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "COM stops the wheel.  The order of the pockets",
        "and the number of the jump boards change",
        "according to the outcome.",
        "Normal Shift:",
        "A basic shift. You will have three jump boards.",
        "",
        "Homerun Shift:",
        "All jump boards appear, but all pockets become Out",
        "pockets.  You will have all the jump boards.",
        "Right Shift:",
        "Hit pockets move to the right side.  You will have",
        "the jump boards on the right.",
        "Left Shift:",
        "Hit pockets move to the left side.  You will have the",
        "jump boards on the left.",
        "Hit Shift:",
        "All pockets become One Base hits.  You will have",
        "no jump boards.",
        "Center Shift:",
        "Hit pockets move to the center.  You will have jump",
        "boards in the center.",
        "Half Out:",
        "Half of the pockets become Out pockets.  You will",
        "have two jump boards.",
        "Lucky Seven:",
        "All pockets become Hit pockets.",
        "You will have three jump boards.",
    },
    {
        "1P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "2P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "COM stops the wheel.  The order of the pockets",
        "and the number of the jump boards change",
        "according to the outcome.",
        "Normal Shift:",
        "A basic shift. You will have three jump boards.",
        "",
        "Homerun Shift:",
        "All jump boards appear, but all pockets become Out",
        "pockets.  You will have all the jump boards.",
        "Right Shift:",
        "Hit pockets move to the right side.  You will have",
        "the jump boards on the right.",
        "Left Shift:",
        "Hit pockets move to the left side.  You will have the",
        "jump boards on the left.",
        "Hit Shift:",
        "All pockets become One Base hits.  You will have",
        "no jump boards.",
        "Center Shift:",
        "Hit pockets move to the center.  You will have jump",
        "boards in the center.",
        "Half Out:",
        "Half of the pockets become Out pockets.  You will",
        "have two jump boards.",
        "Lucky Seven:",
        "All pockets become Hit pockets.",
        "You will have three jump boards.",
    },
    {
        "1P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "2P - Press the A Button to stop the wheel!  The",
        "order of the pockets and the number of the jump",
        "boards change according to the outcome.",
        "COM stops the wheel.  The order of the pockets",
        "and the number of the jump boards change",
        "according to the outcome.",
        "Normal Shift:",
        "A basic shift. You will have three jump boards.",
        "",
        "Homerun Shift:",
        "All jump boards appear, but all pockets become Out",
        "pockets.  You will have all the jump boards.",
        "Right Shift:",
        "Hit pockets move to the right side.  You will have",
        "the jump boards on the right.",
        "Left Shift:",
        "Hit pockets move to the left side.  You will have the",
        "jump boards on the left.",
        "Hit Shift:",
        "All pockets become One Base hits.  You will have",
        "no jump boards.",
        "Center Shift:",
        "Hit pockets move to the center.  You will have jump",
        "boards in the center.",
        "Half Out:",
        "Half of the pockets become Out pockets.  You will",
        "have two jump boards.",
        "Lucky Seven:",
        "All pockets become Hit pockets.",
        "You will have three jump boards.",
    },
    {
        "\202P\202o\202\315\202`\203{\203^\203\223\202\360\202\250\202\265\202\304 \203\213\201[\203\214\203b\203g\202\360\202\306\202\337\202\334\202\265\202\345\202\244\201I",
        "\203\213\201[\203\214\203b\203g\202\360\202\334\202\355\202\265\202\304\201A\203|\203P\203b\203g\202\314\202\310\202\347\202\321\202\251\202\275\202\342",
        "\203W\203\203\203\223\203v\202\276\202\242\202\314 \202\305\202\351\202\251\202\270\202\360 \202\253\202\337\202\334\202\267\201B",
        "\202Q\202o\202\315\202`\203{\203^\203\223\202\360\202\250\202\265\202\304 \203\213\201[\203\214\203b\203g\202\360\202\306\202\337\202\334\202\265\202\345\202\244\201I",
        "\203\213\201[\203\214\203b\203g\202\360\202\334\202\355\202\265\202\304\201A\203|\203P\203b\203g\202\314\202\310\202\347\202\321\202\251\202\275\202\342",
        "\203W\203\203\203\223\203v\202\276\202\242\202\314 \202\305\202\351\202\251\202\270\202\360 \202\253\202\337\202\334\202\267\201B",
        "\202b\202n\202l\202\252\203\213\201[\203\214\203b\203g\202\360\202\306\202\337\202\334\202\267\201B",
        "\203\213\201[\203\214\203b\203g\202\360\202\334\202\355\202\265\202\304\201A\203|\203P\203b\203g\202\314\202\310\202\347\202\321\202\251\202\275\202\342",
        "\203W\203\203\203\223\203v\202\276\202\242\202\314 \202\305\202\351\202\251\202\270\202\360 \202\253\202\337\202\334\202\267\201B",
        "\203m\201[\203}\203\213\203V\203t\203g",
        "\202\253\202\331\202\361\202\304\202\253\202\310\203V\203t\203g\202\305\202\267\201B",
        "\203W\203\203\203\223\203v\202\276\202\242\202\252 \202R\202\302\202\305\202\334\202\267\201B",
        "\202\242\202\301\202\317\202\302\203V\203t\203g",
        "\203|\203P\203b\203g\202\315\201@\202\272\202\361\202\324\203A\203E\203g\202\305\202\267\202\252\201A",
        "\202\267\202\327\202\304\202\314\203W\203\203\203\223\203v\202\276\202\242\202\252\202\305\202\334\202\267\201B",
        "\202\335\202\254\203V\203t\203g",
        "\203\211\203C\203g\202\252\202\355\202\311\201A\203q\203b\203g\202\314\203|\203P\203b\203g\202\252 \202\240\202\302\202\334\202\301\202\304\202\242\202\334\202\267\201B",
        "\203\211\203C\203g\202\252\202\355\202\314 \203W\203\203\203\223\203v\202\276\202\242\202\252 \202\305\202\334\202\267\201B",
        "\202\320\202\276\202\350\203V\203t\203g",
        "\203\214\203t\203g\202\252\202\355\202\311\201A\203q\203b\203g\202\314\203|\203P\203b\203g\202\252 \202\240\202\302\202\334\202\301\202\304\202\242\202\334\202\267\201B",
        "\203\214\203t\203g\202\252\202\355\202\314 \203W\203\203\203\223\203v\202\276\202\242\202\252 \202\305\202\334\202\267\201B",
        "\203R\203c\203R\203c\203V\203t\203g",
        "\203|\203P\203b\203g\202\252 \202\272\202\361\202\324 \202P\203x\201[\203X\203q\203b\203g\202\311\202\310\202\350\202\334\202\267\201B",
        "\203W\203\203\203\223\203v\202\276\202\242\202\315 \202\307\202\352\202\340 \202\305\202\334\202\271\202\361\201B",
        "\202\334\202\361\202\310\202\251\203V\203t\203g",
        "\203Z\203\223\203^\201[\202\311\201A\203q\203b\203g\202\314\203|\203P\203b\203g\202\252 \202\240\202\302\202\334\202\301\202\304\202\242\202\334\202\267\201B",
        "\203Z\203\223\203^\201[\202\314 \203W\203\203\203\223\203v\202\276\202\242\202\252 \202\305\202\334\202\267\201B",
        "\202\315\202\361\202\324\202\361\203A\203E\203g",
        "\203|\203P\203b\203g\202\314\202\315\202\361\202\324\202\361\202\252\201A\203A\203E\203g\202\314\203|\203P\203b\203g\202\311\202\310\202\350\202\334\202\267\201B",
        "\203W\203\203\203\223\203v\202\276\202\242\202\315 \202Q\202\302\202\305\202\334\202\267\201B",
        "\203\211\203b\203L\201[\203Z\203u\203\223",
        "\203|\203P\203b\203g\202\252\201@\202\272\202\361\202\324\203q\203b\203g\202\311\202\310\202\350\202\334\202\267\201B",
        "\203W\203\203\203\223\203v\202\276\202\242\202\315 \202R\202\302\202\305\202\334\202\267\201B",
    },
};

struct BBScreen *lbl_18_bss_19270;
float lbl_18_bss_19274;

void fn_18_1A040(float farg0, float farg1)
{
    float f0;
    struct Sprite **cursor;
    s32 tr0;
    s32 r5_2;
    s32 r6;
    s32 r8;
    s32 r9;

    cursor = &lbl_18_bss_19270->unk338[0x32];
    (*cursor)->x = farg0;
    (*cursor)->y = farg1;
    cursor++;
    (*cursor)->x = farg0 - 256.0f;
    (*cursor)->y = farg1 - 18.0f;
    cursor++;
    (*cursor)->x = farg0 - 256.0f;
    (*cursor)->y = 40.0f + farg1;
    cursor++;
    for (r5_2 = 0; r5_2 < 0xC; r5_2++) {
        (*cursor)->x = ((farg0 - 204.0f) + (38.0f * (float) r5_2));
        (*cursor)->y = (farg1 - 59.0f);
        cursor++;
    }
    for (r5_2 = 0; r5_2 < 2; r5_2++) {
        for (r6 = 0; r6 < 0xD; r6++) {
            tr0 = lbl_18_bss_19270->unk8[r6][r5_2];
            if (tr0 >= 0) {
                r9 = 0;
                if ((tr0 >= 0xA) || ((r5_2 == 1) && (r6 >= lbl_18_bss_19270->unkDC) && ((r6 == 0xC) || ((s32) lbl_18_bss_19270->unk8[r6 + 1][1] < 0)) && ((s32) lbl_18_bss_19270->unk8[0][1] > (s32) lbl_18_bss_19270->unk8[0][0]))) {
                    r9 = 1;
                }
                r8 = r9 != 0 ? 2 : 1;
                while (r8 > 0) {
                    if (r6 == 0) {
                        f0 = 256.0f;
                    } else {
                        f0 = -205.0f + (38.0f * (float) (r6 - 1));
                    }
                    (*cursor)->x = (farg0 + f0);
                    (*cursor)->y = ((farg1 - 18.0f) + (58.0f * (float) r5_2));
                    cursor++;
                    r8--;
                }
            } else if ((r5_2 == 1) && (r6 >= lbl_18_bss_19270->unkDC) && ((s32) lbl_18_bss_19270->unk8[r6][0] >= 0) && ((s32) lbl_18_bss_19270->unk8[0][1] > (s32) lbl_18_bss_19270->unk8[0][0])) {
                (*cursor)->x = ((farg0 - 205.0f) + (38.0f * (float) (r6 - 1)));
                (*cursor)->y = (58.0f + (farg1 - 18.0f));
            }
        }
    }
}

void fn_18_1A35C(void)
{
    u32 sp8[3] = { 0x1649, 0x1645, 0x1646 };
    struct Sprite **cursor;
    s32 tr24;
    s32 r31_2;
    s32 r30_3;
    s32 r29_2;

    cursor = bb_get_sprites(3, 0x32);
    (*cursor)->bmpId = 0x1687;
    (*cursor++)->depth = 0.2f;
    (*cursor++)->bmpId = sp8[lbl_18_bss_19270->unk8C[0]];
    (*cursor)->bmpId = sp8[lbl_18_bss_19270->unk8C[1]];
    r29_2 = 0x35;
    for (r31_2 = 1; r31_2 <= 0xC; r31_2++) {
        cursor = bb_get_sprites_next(1, &r29_2);
        if ((r31_2 <= lbl_18_bss_19270->unkDC) || ((s32) lbl_18_bss_19270->unk8[r31_2][0] >= 0) || (((s32) lbl_18_bss_19270->unk8[r31_2 - 1][1] >= 0) && ((s32) lbl_18_bss_19270->unk8[0][0] == lbl_18_bss_19270->unk8[0][1]))) {
            (*cursor)->bmpId = (u16) (r31_2 + 0x165B);
        } else {
            (*cursor)->bmpId = 0x1668;
        }
    }
    for (r31_2 = 0; r31_2 < 2; r31_2++) {
        for (r30_3 = 0; r30_3 < 0xD; r30_3++) {
            tr24 = lbl_18_bss_19270->unk8[r30_3][r31_2];
            if (tr24 >= 0) {
                cursor = bb_get_sprites_next(1, &r29_2);
                (*cursor)->bmpId = (u16) ((tr24 % 10) + 0x1651);
                if (tr24 >= 0xA) {
                    (*cursor)->textAlign = 1;
                    cursor = bb_get_sprites_next(1, &r29_2);
                    (*cursor)->bmpId = (u16) ((tr24 / 10) + 0x1651);
                    (*cursor)->textAlign = 7;
                } else if ((r31_2 == 1) && (r30_3 >= lbl_18_bss_19270->unkDC) && ((r30_3 == 0xC) || ((s32) lbl_18_bss_19270->unk8[r30_3 + 1][1] < 0)) && ((s32) lbl_18_bss_19270->unk8[0][1] > (s32) lbl_18_bss_19270->unk8[0][0])) {
                    (*cursor)->textAlign = 7;
                    cursor = bb_get_sprites_next(1, &r29_2);
                    (*cursor)->bmpId = 0x165B;
                    (*cursor)->textAlign = 1;
                    (*cursor)->scaleX = 0.5f;
                }
            } else if ((r31_2 == 1) && (r30_3 >= lbl_18_bss_19270->unkDC) && ((s32) lbl_18_bss_19270->unk8[r30_3][0] >= 0) && ((s32) lbl_18_bss_19270->unk8[0][1] > (s32) lbl_18_bss_19270->unk8[0][0])) {
                cursor = bb_get_sprites_next(1, &r29_2);
                (*cursor)->bmpId = 0x165B;
            }
        }
    }
    fn_18_1A040(320.0f, 360.0f);
    lbl_18_bss_19270->unk154 = 0;
}

static const s32 bb_count_limits[3] = {3, 4, 3};

struct BBCountOffsets { float y[2][3]; };
static const struct BBCountOffsets bb_count_offsets = {{{-32.0f, 0.0f, 32.0f}, {0.0f, -32.0f, 32.0f}}};

static inline float bb_count_x(float x, s32 i, s32 count)
{
    if (i < count)
        return 32.0f + x + 32.0f * (float)i;
    return -256.0f;
}

static void *bb_anchor_bb_count_x = (void *)bb_count_x;

static inline void bb_position_counts(float x, s32 flash, float y[2][3])
{
    struct Sprite **p;
    s32 row;
    s32 i;
    float px;

    *(struct BBCountOffsets *)y = bb_count_offsets;
    p = &lbl_18_bss_19270->unk338[6];
    if (*p != NULL) {
        (*p)->x = x;
        (*p)->y = 410.0f;
        p++;
        for (row = 0; row < 3; row++) {
            for (i = 0; i < bb_count_limits[row] - 1; i++) {
                px = bb_count_x(x, i, lbl_18_bss_19270->unkCC[row] + (row == flash ? 1 : 0));
                (*p)->x = px;
                (*p)->y = 410.0f + y[locale == 5 ? 0 : 1][row];
                p++;
            }
        }
    }
}

static void *bb_anchor_bb_position_counts = (void *)bb_position_counts;

static inline void bb_position_counts_default(s32 flash)
{
    float y[2][3];
    struct Sprite **p;
    s32 row;
    s32 i;
    float px;

    *(struct BBCountOffsets *)y = bb_count_offsets;
    p = &lbl_18_bss_19270->unk338[6];
    if (*p != NULL) {
        (*p)->x = 32.0f;
        (*p)->y = 410.0f;
        p++;
        for (row = 0; row < 3; row++) {
            for (i = 0; i < bb_count_limits[row] - 1; i++) {
                if (i < lbl_18_bss_19270->unkCC[row] + (row == flash ? 1 : 0))
                    px = 64.0f + 32.0f * (float)i;
                else
                    px = -256.0f;
                (*p)->x = px;
                (*p)->y = 410.0f + y[locale == 5 ? 0 : 1][row];
                p++;
            }
        }
    }
}

static inline void bb_position_counts_created(struct BBCreateTables *tables, s32 flash)
{
    struct Sprite **p;
    s32 row;
    s32 i;
    float px;

    *(struct BBCountOffsets *)tables->count_y = bb_count_offsets;
    p = &lbl_18_bss_19270->unk338[6];
    if (*p != NULL) {
        (*p)->x = 32.0f;
        (*p)->y = 410.0f;
        p++;
        for (row = 0; row < 3; row++) {
            for (i = 0; i < bb_count_limits[row] - 1; i++) {
                if (i < lbl_18_bss_19270->unkCC[row] + (row == flash ? 1 : 0))
                    px = 64.0f + 32.0f * (float)i;
                else
                    px = -256.0f;
                (*p)->x = px;
                (*p)->y = 410.0f + tables->count_y[locale == 5 ? 0 : 1][row];
                p++;
            }
        }
    }
}

static void *bb_anchor_bb_position_counts_default = (void *)bb_position_counts_default;

static inline s32 bb_animate_start(struct BBAnimateTables *tables)
{
    static const struct BBAnimate5 data = {{8, 16, 16, 8, 16}};
    struct Sprite **cursor;
    float tf3;
    float tf3_2;
    s32 tr0;
    struct Sprite *tr3_2;
    struct Sprite *tr3_3;

    tables->start = data;
    cursor = &lbl_18_bss_19270->unk338[0];
    tr0 = lbl_18_bss_19270->unk158;
    switch (tr0) {
    case 0:
        (*cursor)->x = 320.0f;
        (*cursor)->y = (240.0f + (0.1171875f * (float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248)));
        (*cursor)->scaleX = (1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f));
        (*cursor)->scaleY = (1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f));
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            fn_18_218EC(0);
        }
        break;
    case 2:
        tf3 = 1.0f - ((float) lbl_18_bss_19270->unk248 / 16.0f);
        tr3_2 = *cursor;
        bb_flash_sprite(tr3_2, tf3);
        break;
    case 3:
        tf3_2 = lbl_18_bss_19270->unk248 / 16.0f;
        tr3_3 = *cursor;
        bb_flash_sprite(tr3_3, tf3_2);
        break;
    case 5:
        (*cursor)->opacity = ((float) lbl_18_bss_19270->unk248 / 16.0f);
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            bb_free_sprites(1, 0);
            return 0;
        } else {
            break;
        }
        break;
    }

    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        lbl_18_bss_19270->unk248 = tables->start.values[lbl_18_bss_19270->unk158++];
    }
    return 1;
}

static void *bb_anchor_bb_animate_start = (void *)bb_animate_start;

static inline void bb_create_start(void)
{
    struct Sprite **r4_5;

    r4_5 = bb_get_sprites(1, 0x0);
    (*r4_5)->bmpId = 0x164F;
    (*r4_5)->x = 320.0f;
    (*r4_5)->y = 640.0f;
    lbl_18_bss_19270->unk248 = 0x20;
}

static void *bb_anchor_bb_create_start = (void *)bb_create_start;

static inline void bb_create_count_message(struct BBCreateTables *tables)
{
    static const struct BBCreateIds data = {{5768, 5750, 5758}};
    struct Sprite **r4_6;

    tables->message = data;
    r4_6 = bb_get_sprites(1, 0x0);
    (*r4_6)->bmpId = (u16) tables->message.ids[lbl_18_bss_19270->unk154 - 0x20];
    (*r4_6)->x = 320.0f;
    (*r4_6)->y = 200.0f;
    (*r4_6)->scaleX = 0.0f;
    (*r4_6)->scaleY = 0.0f;
    lbl_18_bss_19270->unk248 = 0x10;
}

static void *bb_anchor_bb_create_count_message = (void *)bb_create_count_message;

s32 fn_18_1AD98(void)
{
    s32 table_8[2][2] = {{5755, 5753}, {5636, 5753}};
    s32 table_48[5] = {48, 32, 16, 16, 128};
    struct Sprite **cursor;
    float tf0;
    s32 tr0;
    s32 tr0_2;
    s32 tr0_3;
    s32 tr30;
    s32 r0_3;
    s32 r0_6;
    u16 r0;
    u16 r0_2;
    u16 r0_7;
    u16 r0_8;
    u32 tr3_2;

    void *tr4;

    cursor = &lbl_18_bss_19270->unk338[0];
    tr30 = lbl_18_bss_19270->unk154 - 0x20;
    tr0 = lbl_18_bss_19270->unk158;
    switch (tr0) {
    case 0:
        tf0 = 1.0f - ((float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 256.0f);
        (*cursor)->scaleX = tf0;
        (*cursor)->scaleY = tf0;
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            tr0_2 = lbl_18_bss_19270->unk154;
            switch (tr0_2) {
            case 32:
                if ((s32) lbl_18_bss_19270->unkCC[0] < 2) {
                    r0 = 1;
                } else {
                    r0 = 2;
                }
                fn_18_218EC((s32) r0);
                break;
            case 33:
                fn_18_218EC(5);
                break;
            case 34:
                if ((s32) lbl_18_bss_19270->unkCC[0] < 3) {
                    r0_2 = 7;
                } else {
                    r0_2 = 4;
                }
                fn_18_218EC((s32) r0_2);
                break;
            }
            if ((s32) lbl_18_bss_19270->unk154 == 0x22) {
                lbl_18_bss_19270->unkCC[0] = 0;
                lbl_18_bss_19270->unkCC[1] = 0;
            }
            if ((tr30 < 2) && ((s32) lbl_18_bss_19270->unkCC[tr30] >= (bb_count_limits[tr30] - 1))) {
                lbl_18_bss_19270->unk248 = 8;
                lbl_18_bss_19270->unk158++;
            }
        }
        break;
    case 1:
        tr0_3 = lbl_18_bss_19270->unk248 / 8;
        tr3_2 = (u32) tr0_3 >> 0x1F;
        if ((s32) (((tr0_3 & 1) ^ tr3_2) - tr3_2) != 0) {
            r0_3 = tr30;
        } else {
            r0_3 = -1;
        }

        bb_position_counts_default(r0_3);
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            lbl_18_bss_19270->unkCC[tr30]++;
        }
        break;
    case 2:

        bb_position_counts_default(-1);
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            if (((s32) lbl_18_bss_19270->unkCC[tr30] < (s32) bb_count_limits[tr30]) || (((s32) lbl_18_bss_19270->unk154 == 0x22) && (fn_18_18B88() != 0))) {
                bb_free_sprites(1, 0);
                return 0;
            }
            if ((s32) lbl_18_bss_19270->unk154 == 0x20) {
                g_get_sprite_work(lbl_18_bss_19270, 0x22);
                return 0;
            }
            break;
        }
        break;
    case 3:
        (*cursor)->scaleY = ((float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 256.0f);
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            if ((s32) locale == 5) {
                r0_6 = 0;
            } else {
                r0_6 = 1;
            }
            (*cursor)->bmpId = (u16) table_8[r0_6][tr30 - 1];
        }
        break;
    case 4:
        (*cursor)->scaleY = (1.0f - ((float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 256.0f));
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            if ((s32) locale == 5) {
                if ((s32) lbl_18_bss_19270->unk154 == 0x21) {
                    r0_7 = 6;
                } else {
                    r0_7 = 0x11;
                }
                fn_18_218EC((s32) r0_7);
            } else {
                if ((s32) lbl_18_bss_19270->unk154 == 0x21) {
                    r0_8 = 0x4B;
                } else {
                    r0_8 = 0x11;
                }
                fn_18_218EC((s32) r0_8);
            }
        }
        break;
    case 5:
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            bb_free_sprites(1, 0);
            return 0;
        }
        break;
    }
    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        lbl_18_bss_19270->unk248 = table_48[lbl_18_bss_19270->unk158++];
    }
    return 1;
}

static inline void bb_create_strikeout(void)
{
    struct Sprite **r4_7;

    r4_7 = bb_get_sprites(1, 0x0);
    (*r4_7)->bmpId = 0x167A;
    (*r4_7)->x = 320.0f;
    (*r4_7)->y = 384.0f;
    (*r4_7)->scaleX = 0.0f;
    (*r4_7)->scaleY = 0.0f;
    lbl_18_bss_19270->unk248 = 0x10;
}

static void *bb_anchor_bb_create_strikeout = (void *)bb_create_strikeout;

static inline s32 bb_animate_result(struct BBAnimateTables *tables)
{
    static const struct BBAnimate3 sounds = {{9, 11, 13}};
    static const struct BBAnimate3 times = {{16, 16, 128}};
    struct Sprite **cursor;
    float tf0_2;
    float tf3_3;
    float tf3_4;
    s32 tr0_5;
    struct Sprite *tr3_5;
    struct Sprite *tr3_6;

    tables->sounds = sounds;
    tables->result = times;
    cursor = &lbl_18_bss_19270->unk338[0];
    tr0_5 = lbl_18_bss_19270->unk158;
    switch (tr0_5) {
    case 0:
        tf0_2 = 1.0f - ((float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 256.0f);
        (*cursor)->scaleY = tf0_2;
        (*cursor)->scaleX = tf0_2;
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            fn_18_218EC((s32) (u16) tables->sounds.values[lbl_18_bss_19270->unk154 - 0x24]);
        }
        break;
    case 1:
        tf3_3 = 1.0f - ((float) lbl_18_bss_19270->unk248 / 16.0f);
        tr3_5 = *cursor;
        bb_flash_sprite(tr3_5, tf3_3);
        break;
    case 2:
        tf3_4 = lbl_18_bss_19270->unk248 / 16.0f;
        tr3_6 = *cursor;
        bb_flash_sprite(tr3_6, tf3_4);
        break;
    case 3:
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            bb_free_sprites(1, 0);
            return 0;
        } else {
            break;
        }
        break;
    default:
        return 0;
    }

    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        lbl_18_bss_19270->unk248 = tables->result.values[lbl_18_bss_19270->unk158++];
    }
    return 1;
}

static void *bb_anchor_bb_animate_result = (void *)bb_animate_result;

static inline void bb_create_result(struct BBCreateTables *tables)
{
    static const struct BBCreateIds data = {{5633, 5634, 5635}};
    struct Sprite **r4_8;

    tables->result = data;
    r4_8 = bb_get_sprites(1, 0x0);
    (*r4_8)->bmpId = (u16) tables->result.ids[lbl_18_bss_19270->unk154 - 0x24];
    (*r4_8)->x = 320.0f;
    (*r4_8)->y = 384.0f;
    (*r4_8)->scaleX = 0.0f;
    (*r4_8)->scaleY = 0.0f;
    lbl_18_bss_19270->unk248 = 0x10;
}

static void *bb_anchor_bb_create_result = (void *)bb_create_result;

static inline s32 bb_animate_change(struct BBAnimateTables *tables)
{
    static const struct BBAnimate3 data = {{8, 16, 16}};
    struct Sprite **cursor;
    float tf0_3;
    float tf3_5;
    float tf3_6;
    s32 tr0_8;
    struct Sprite *tr3_8;
    struct Sprite *tr3_9;

    tables->change = data;
    cursor = &lbl_18_bss_19270->unk338[0];
    tr0_8 = lbl_18_bss_19270->unk158;
    switch (tr0_8) {
    case 0:
        (*cursor)->y = (384.0f + (0.125f * (float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248)));
        tf0_3 = 1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f);
        (*cursor)->scaleY = tf0_3;
        (*cursor)->scaleX = tf0_3;
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            fn_18_218EC(0x10);
        }
        break;
    case 2:
        tf3_5 = 1.0f - ((float) lbl_18_bss_19270->unk248 / 16.0f);
        tr3_8 = *cursor;
        bb_flash_sprite(tr3_8, tf3_5);
        break;
    case 3:
        tf3_6 = lbl_18_bss_19270->unk248 / 16.0f;
        tr3_9 = *cursor;
        bb_flash_sprite(tr3_9, tf3_6);
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            return 0;
        } else {
            break;
        }
        break;
    }

    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        lbl_18_bss_19270->unk248 = tables->change.values[lbl_18_bss_19270->unk158++];
    }
    return 1;
}

static void *bb_anchor_bb_animate_change = (void *)bb_animate_change;

static inline void bb_create_change(void)
{
    struct Sprite **r4_9;

    r4_9 = bb_get_sprites(1, 0x0);
    (*r4_9)->bmpId = 0x167C;
    (*r4_9)->x = 320.0f;
    (*r4_9)->y = 512.0f;
    (*r4_9)->scaleX = 0.0f;
    (*r4_9)->scaleY = 0.0f;
    lbl_18_bss_19270->unk248 = 0x20;
}

static void *bb_anchor_bb_create_change = (void *)bb_create_change;

static inline s32 bb_animate_drop(void)
{
    struct Sprite **cursor;
    s32 r4;

    cursor = &lbl_18_bss_19270->unk338[0];
    (*cursor)->y = (240.0f + (0.5625f * (float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248)));
    r4 = lbl_18_bss_19270->unk248 > 0;
    return r4;
}

static void *bb_anchor_bb_animate_drop = (void *)bb_animate_drop;

void fn_18_1B6C4(void)
{
    s32 table_2C[6] = {5646, 5677, 5647, 5662, 5661, 5676};
    s32 table_14[6] = {15, 32, 51, 3, 15, 15};
    s32 table_8[3] = {5660, 5644, 5645};
    struct Sprite **cursor;
    float f0;
    float f0_2;
    s32 r0_2;
    s32 r29;
    s32 r4;

    if ((s32) lbl_18_bss_19270->unkD8 < 0xA) {
        r29 = 0xE;
    } else {
        r29 = 0xF;
    }
    cursor = bb_get_sprites(r29, 0x12);
    for (r4 = 0; r4 < 2; r4++) {
        cursor[0x0]->bmpId = (u16) ((lbl_18_bss_19270->unk8[0][r4] % 10) + 0x163A);
        cursor[0x1]->bmpId = (u16) ((lbl_18_bss_19270->unk8[0][r4] / 10) + 0x163A);
        if (r4 == 0) {
            if ((s32) lbl_18_bss_19270->unk80 != 0) {
                cursor[0x0]->x = 482.0f;
                cursor[0x0]->y = 414.0f;
                cursor[0x0]->scaleX = 0.6f;
                cursor[0x0]->scaleY = 0.6f;
                cursor[0x1]->x = 432.0f;
                cursor[0x1]->y = 414.0f;
                cursor[0x1]->scaleX = 0.6f;
                cursor[0x1]->scaleY = 0.6f;
            } else {
                cursor[0x0]->x = 450.0f;
                cursor[0x0]->y = 426.0f;
                cursor[0x0]->scaleX = 0.33333334f;
                cursor[0x0]->scaleY = 0.33333334f;
                cursor[0x1]->x = 422.0f;
                cursor[0x1]->y = 426.0f;
                cursor[0x1]->scaleX = 0.33333334f;
                cursor[0x1]->scaleY = 0.33333334f;
            }
        } else if ((s32) lbl_18_bss_19270->unk80 != 0) {
            cursor[0x0]->x = 596.0f;
            cursor[0x0]->y = 426.0f;
            cursor[0x0]->scaleX = 0.33333334f;
            cursor[0x0]->scaleY = 0.33333334f;
            cursor[0x1]->x = 568.0f;
            cursor[0x1]->y = 426.0f;
            cursor[0x1]->scaleX = 0.33333334f;
            cursor[0x1]->scaleY = 0.33333334f;
        } else {
            cursor[0x0]->x = 604.0f;
            cursor[0x0]->y = 414.0f;
            cursor[0x0]->scaleX = 0.6f;
            cursor[0x0]->scaleY = 0.6f;
            cursor[0x1]->x = 554.0f;
            cursor[0x1]->y = 414.0f;
            cursor[0x1]->scaleX = 0.6f;
            cursor[0x1]->scaleY = 0.6f;
        }
        cursor[0x0]->widescreenArg = 0x220;
        cursor[0x1]->widescreenArg = 0x220;
        cursor += 2;
    }
    (*cursor)->bmpId = 0x1644;
    if ((s32) lbl_18_bss_19270->unk80 != 0) {
        f0 = 528.0f;
    } else {
        f0 = 498.0f;
    }
    (*cursor)->x = f0;
    (*cursor)->y = 418.0f;
    (*cursor)->scaleX = 0.5f;
    (*cursor)->scaleY = 0.5f;
    (*cursor)->widescreenArg = 0x220;
    cursor++;
    (*cursor)->bmpId = (u16) table_8[lbl_18_bss_19270->unk8C[0]];
    if ((s32) lbl_18_bss_19270->unk80 != 0) {
        (*cursor)->x = 466.0f;
        (*cursor)->y = 372.0f;
    } else {
        (*cursor)->x = 442.0f;
        (*cursor)->y = 396.0f;
    }
    (*cursor)->widescreenArg = 0x220;
    cursor++;
    (*cursor)->bmpId = (u16) table_8[lbl_18_bss_19270->unk8C[1]];
    (*cursor)->x = 590.0f;
    if ((s32) lbl_18_bss_19270->unk80 != 0) {
        f0_2 = 396.0f;
    } else {
        f0_2 = 374.0f;
    }
    (*cursor)->y = f0_2;
    (*cursor)->widescreenArg = 0x220;
    cursor++;
    (*cursor)->bmpId = 0x1610;
    (*cursor)->depth = 0.2f;
    (*cursor)->x = 76.0f;
    (*cursor)->y = 312.0f;
    (*cursor)->widescreenArg = 0x60;
    cursor++;
    (*cursor)->bmpId = (u16) (lbl_18_bss_19270->unk148 + 0x1612);
    (*cursor)->x = 40.0f;
    (*cursor)->y = 343.0f;
    (*cursor)->widescreenArg = 0x60;
    cursor++;
    if ((s32) lbl_18_bss_19270->unk80 != 0) {
        r0_2 = 0;
    } else {
        r0_2 = 1;
    }
    r4 = lbl_18_bss_19270->unk9C[r0_2];
    (*cursor)->bmpId = (u16) table_2C[r4];
    (*cursor)->x = 86.0f;
    (*cursor)->y = 343.0f;
    (*cursor)->widescreenArg = 0x60;
    cursor++;
    (*cursor)->bmpId = (u16) table_14[r4];
    (*cursor)->x = 76.0f;
    (*cursor)->y = 302.0f;
    (*cursor)->scaleX = 0.6f;
    (*cursor)->scaleY = 0.4f;
    (*cursor)->widescreenArg = 0x60;
    cursor++;
    (*cursor)->depth = 0.2f;
    if ((s32) locale == 5) {
        (*cursor)->bmpId = 0x161F;
        (*cursor)->x = 552.0f;
        (*cursor)->y = 352.0f;
        (*cursor)->widescreenArg = 0x220;
        cursor++;
        (*cursor)->bmpId = (u16)(lbl_18_bss_19270->unk80 != 0 ? 0x162A : 0x162B);
        (*cursor)->x = 592.0f;
        (*cursor)->y = 340.0f;
        (*cursor)->widescreenArg = 0x220;
        cursor++;
        (*cursor)->bmpId = (u16) ((lbl_18_bss_19270->unkD8 % 10) + 0x1620);
        (*cursor)->x = 534.0f;
        (*cursor)->y = 316.0f;
        (*cursor)->widescreenArg = 0x220;
        cursor++;
        if ((s32) lbl_18_bss_19270->unkD8 >= 0xA) {
            (*cursor)->bmpId = (u16) ((lbl_18_bss_19270->unkD8 / 10) + 0x1620);
            (*cursor)->x = 504.0f;
            (*cursor)->y = 316.0f;
            (*cursor)->widescreenArg = 0x220;
        }
    } else {
        if ((s32) lbl_18_bss_19270->unkD8 < 0xA) {
            (*cursor)->bmpId = 0x160B;
            (*cursor)->x = 560.0f;
            (*cursor)->y = 340.0f;
        } else {
            (*cursor)->bmpId = 0x160A;
            (*cursor)->x = 552.0f;
            (*cursor)->y = 340.0f;
        }
        (*cursor)->widescreenArg = 0x220;
        cursor++;
        (*cursor)->bmpId = (u16)(lbl_18_bss_19270->unk80 != 0 ? 0x1669 : 0x1605);
        (*cursor)->x = 582.0f;
        (*cursor)->y = 350.0f;
        (*cursor)->widescreenArg = 0x220;
        cursor++;
        (*cursor)->bmpId = (u16) ((lbl_18_bss_19270->unkD8 % 10) + 0x1620);
        (*cursor)->x = 518.0f;
        (*cursor)->y = 324.0f;
        (*cursor)->widescreenArg = 0x220;
        cursor++;
        if ((s32) lbl_18_bss_19270->unkD8 >= 0xA) {
            (*cursor)->bmpId = (u16) ((lbl_18_bss_19270->unkD8 / 10) + 0x1620);
            (*cursor)->x = 488.0f;
            (*cursor)->y = 324.0f;
            (*cursor)->widescreenArg = 0x220;
        }
    }
    lbl_18_bss_19270->unk154 = 0;
}

static inline void bb_create_score(void)
{
    float f0_2;
    float f0_3;
    s32 r3_2;
    s32 score;
    struct Sprite **r4_10;

    r4_10 = bb_get_sprites(5, 0x12);
    for (r3_2 = 0; r3_2 < 2; r3_2++) {
        score = lbl_18_bss_19270->unk8[0][r3_2];
        r4_10[0x0]->bmpId = (u16) ((score % 10) + 0x163A);
        if (r3_2 == 0) {
            f0_2 = 236.0f;
        } else {
            f0_2 = 492.0f;
        }
        r4_10[0x0]->x = f0_2;
        r4_10[0x0]->y = 400.0f;
        r4_10[0x1]->bmpId = (u16) ((score / 10) + 0x163A);
        if (r3_2 == 0) {
            f0_3 = 156.0f;
        } else {
            f0_3 = 412.0f;
        }
        r4_10[0x1]->x = f0_3;
        r4_10[0x1]->y = 400.0f;
        if ((s32) score < 0xA) {
            r4_10[0x0]->textAlign = 7;
            r4_10[0x1]->y = 640.0f;
        }
        r4_10 += 2;
    }
    r4_10[0x0]->bmpId = 0x1644;
    r4_10[0x0]->depth = 0.2f;
    r4_10[0x0]->x = 320.0f;
    r4_10[0x0]->y = 404.0f;
    lbl_18_bss_19270->unk154 = 0;
}

static void *bb_anchor_bb_create_score = (void *)bb_create_score;

static inline s32 bb_animate_score(void)
{
    s32 tr0_13;
    s32 r4;

    tr0_13 = 0x20 - lbl_18_bss_19270->unk248;
    fn_18_1A040(320.0f, 360.0f + (0.21875f * (float) (tr0_13 * tr0_13)));
    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        bb_free_sprites(0x43, 50);
        r4 = 0;
    } else {
        r4 = 1;
    }
    return r4;
}

static void *bb_anchor_bb_animate_score = (void *)bb_animate_score;

static inline s32 bb_animate_summary(void)
{
    struct Sprite **cursor;
    float tf1;
    float tf2;
    s32 r4;

    tf2 = (float)(lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 1024.0f;
    cursor = &lbl_18_bss_19270->unk338[50];
    (*cursor)->y = 96.0f - 224.0f * tf2;
    cursor++;
    (*cursor)->y = 96.0f - 224.0f * tf2;
    cursor++;
    (*cursor)->scaleX = 1.0f - tf2;
    (*cursor)->scaleY = 1.0f - tf2;
    cursor++;
    tf1 = 144.0f - 304.0f * tf2;
    (*cursor)->x = tf1;
    cursor++;
    (*cursor)->x = tf1 - 80.0f;
    cursor++;
    (*cursor)->x = 28.0f + tf1;
    cursor++;
    tf1 = 496.0f + 304.0f * tf2;
    (*cursor)->x = tf1;
    cursor++;
    (*cursor)->x = 80.0f + tf1;
    cursor++;
    (*cursor)->x = tf1 - 28.0f;
    r4 = lbl_18_bss_19270->unk248 > 0;
    return r4;
}

static void *bb_anchor_bb_animate_summary = (void *)bb_animate_summary;

static inline void bb_create_summary(struct BBCreateTables *tables)
{
    static const struct BBCreateTitles titles = {{5757, 5751}};
    static const struct BBCreateTeams teams = {{5703, 5709, 5704, 5706, 5707, 5708}};
    static const struct BBCreateIds characters = {{5705, 5701, 5702}};
    struct Sprite **p;
    s32 side;

    tables->titles = titles;
    tables->teams = teams;
    tables->characters = characters;
    p = bb_get_sprites(9, 0x32);
    (*p)->bmpId = tables->titles.ids[lbl_18_bss_19270->unkE0];
    (*p)->x = 320.0f;
    (*p)->y = -256.0f;
    p++;
    (*p)->bmpId = tables->titles.ids[lbl_18_bss_19270->unkE0];
    (*p)->x = 320.0f;
    (*p)->y = -256.0f;
    p++;
    (*p)->bmpId = 0x1650;
    (*p)->x = 320.0f;
    (*p)->y = 284.0f;
    (*p)->scaleX = 0.0f;
    (*p)->scaleY = 0.0f;
    p++;
    for (side = 0; side < 2; side++) {
        (*p)->bmpId = 0x164E;
        (*p)->depth = 0.2f;
        (*p)->x = -256.0f;
        (*p)->y = 424.0f;
        p++;
        (*p)->bmpId = tables->characters.ids[lbl_18_bss_19270->unk8C[side]];
        (*p)->x = -256.0f;
        (*p)->y = 424.0f;
        p++;
        (*p)->bmpId = tables->teams.ids[lbl_18_bss_19270->unk9C[side]];
        (*p)->x = -256.0f;
        (*p)->y = 424.0f;
        p++;
    }
    lbl_18_bss_19270->unk248 = 0x20;
}

static void *bb_anchor_bb_create_summary = (void *)bb_create_summary;

static inline void bb_set_sine_pair(struct Sprite **cursor, s32 angle)
{
    (*cursor)->scaleX = math_sin(angle);
    cursor++;
    (*cursor)->scaleX = math_sin(angle);
}

s32 fn_18_1C128(void)
{
    s32 table_8[2] = {5757, 5751};
    s32 table_10[5] = {16, 16, 64, 16, 16};
    struct Sprite **cursor;
    float tf0;
    float tf2;
    s32 tr0;
    s16 tr0_7;
    struct Sprite *tr3_2;
    struct Sprite *tr3_3;
    struct Sprite *tr3_4;
    struct Sprite *tr3_5;

    cursor = &lbl_18_bss_19270->unk338[50];
    tr0 = lbl_18_bss_19270->unk158;
    switch (tr0) {
    case 0:
        tf0 = (float)(lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 1024.0f;
        (*cursor)->scaleX = (1.0f - tf0);
        (*cursor)->scaleY = (1.0f - tf0);
        cursor++;
        (*cursor)->scaleX = (1.0f - tf0);
        (*cursor)->scaleY = (1.0f - tf0);
        cursor++;
        (*cursor)->scaleX = (1.0f - tf0);
        (*cursor)->scaleY = (1.0f - tf0);
        cursor++;
        tf2 = 144.0f - (304.0f * tf0);
        (*cursor)->x = tf2;
        cursor++;
        (*cursor)->x = (tf2 - 80.0f);
        cursor++;
        (*cursor)->x = (28.0f + tf2);
        cursor++;
        tf2 = 496.0f + (304.0f * tf0);
        (*cursor)->x = tf2;
        cursor++;
        (*cursor)->x = (80.0f + tf2);
        cursor++;
        (*cursor)->x = (tf2 - 28.0f);
        break;
    case 1:
        tf0 = 1.0f - ((float) lbl_18_bss_19270->unk248 / 16.0f);
        tr3_2 = *cursor;
        bb_flash_sprite(tr3_2, tf0);
        cursor++;
        tr3_3 = *cursor;
        bb_flash_sprite(tr3_3, tf0);
        break;
    case 2:
        tf0 = lbl_18_bss_19270->unk248 / 16.0f;
        tr3_4 = *cursor;
        bb_flash_sprite(tr3_4, tf0);
        cursor++;
        tr3_5 = *cursor;
        bb_flash_sprite(tr3_5, tf0);
        break;
    case 4:
        bb_set_sine_pair(cursor, (s16)((lbl_18_bss_19270->unk248 << 0xE) / 16));
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            cursor = &lbl_18_bss_19270->unk338[50];
            (*cursor)->bmpId = (u16) table_8[lbl_18_bss_19270->unkE0];
            (*cursor)->x = 320.0f;
            (*cursor)->y = 96.0f;
            cursor++;
            (*cursor)->bmpId = (u16) table_8[lbl_18_bss_19270->unkE0];
            (*cursor)->x = 320.0f;
            (*cursor)->y = 96.0f;
        }
        break;
    case 5:
        tr0_7 = (lbl_18_bss_19270->unk248 << 0xE) / 16;
        (*cursor)->scaleX = math_sin((s16)(tr0_7 + 0x4000));
        cursor++;
        (*cursor)->scaleX = math_sin((s16)(tr0_7 + 0x4000));
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            return 0;
        }
        break;
    }
    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        lbl_18_bss_19270->unk248 = table_10[lbl_18_bss_19270->unk158++];
    }
    return 1;
}

void fn_18_1C6AC(void)
{
    s32 teams[6] = {5703, 5709, 5704, 5706, 5707, 5708};
    s32 characters[3] = {5705, 5701, 5702};
    struct Sprite **p;
    s32 side;

    p = bb_get_sprites(9, 0x32);
    (*p)->bmpId = lbl_18_bss_19270->unk150 + 0x166A;
    (*p)->x = 320.0f;
    (*p)->y = 60.0f;
    (*p)->scaleX = 0.0f;
    (*p)->scaleY = 0.0f;
    p++;
    (*p)->bmpId = 0x1670;
    (*p)->x = 320.0f;
    (*p)->y = 132.0f;
    (*p)->scaleX = 0.0f;
    (*p)->scaleY = 0.0f;
    p++;
    (*p)->bmpId = 0x1650;
    (*p)->x = 320.0f;
    (*p)->y = 284.0f;
    (*p)->scaleX = 0.0f;
    (*p)->scaleY = 0.0f;
    p++;
    for (side = 0; side < 2; side++) {
        (*p)->bmpId = 0x164E;
        (*p)->depth = 0.2f;
        (*p)->x = -256.0f;
        (*p)->y = 424.0f;
        p++;
        (*p)->bmpId = characters[lbl_18_bss_19270->unk8C[side]];
        (*p)->x = -256.0f;
        (*p)->y = 424.0f;
        p++;
        (*p)->bmpId = teams[lbl_18_bss_19270->unk9C[side]];
        (*p)->x = -256.0f;
        (*p)->y = 424.0f;
        p++;
    }
    lbl_18_bss_19270->unk248 = 0x20;
}

static inline struct Sprite **bb_count_row(s32 row, struct Sprite **p, float tf4, float y[2][3])
{
    s32 i;
    float px;
    for (i = 0; i < bb_count_limits[row] - 1; i++) {
        if (i < lbl_18_bss_19270->unkCC[row] + (row == -1 ? 1 : 0))
            px = 32.0f + tf4 + 32.0f * (float)i;
        else
            px = -256.0f;
        (*p)->x = px;
        (*p)->y = 410.0f + y[locale == 5 ? 0 : 1][row];
        p++;
    }
    return p;
}

static inline s32 bb_animate_counts_out(struct Sprite **cursor, float y[2][3])
{
    s32 row;
    struct Sprite **p;
    float tf4;
    s32 r4;

    (*cursor)->x = 320.0f;
    (*cursor)->y = (96.0f + (0.140625f * (float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248)));
    (*cursor)->scaleX = (1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f));
    (*cursor)->scaleY = (1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f));
    tf4 = 32.0f - (0.125f * (float) ((0x20 - lbl_18_bss_19270->unk248) * (0x20 - lbl_18_bss_19270->unk248)));

    *(struct BBCountOffsets *)y = bb_count_offsets;
    p = &lbl_18_bss_19270->unk338[6];
    if (*p != NULL) {
        (*p)->x = tf4;
        (*p)->y = 410.0f;
        p++;
        for (row = 0; row < 3; row++) {
            p = bb_count_row(row, p, tf4, y);
        }
    }

    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        fn_18_218EC(0x12);
        bb_free_sprites(8, 6);
        r4 = 0;
    } else {
        r4 = 1;
    }
    return r4;
}

static inline void bb_animate_counts_out_void(struct Sprite **cursor, float y[2][3])
{
    s32 row;
    struct Sprite **p;
    float tf4;

    (*cursor)->x = 320.0f;
    (*cursor)->y = (96.0f + (0.140625f * (float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248)));
    (*cursor)->scaleX = (1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f));
    (*cursor)->scaleY = (1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f));
    tf4 = 32.0f - (0.125f * (float) ((0x20 - lbl_18_bss_19270->unk248) * (0x20 - lbl_18_bss_19270->unk248)));

    *(struct BBCountOffsets *)y = bb_count_offsets;
    p = &lbl_18_bss_19270->unk338[6];
    if (*p != NULL) {
        (*p)->x = tf4;
        (*p)->y = 410.0f;
        p++;
        for (row = 0; row < 3; row++) {
            p = bb_count_row(row, p, tf4, y);
        }
    }

    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        fn_18_218EC(0x12);
        bb_free_sprites(8, 6);
    }
}

static void *bb_anchor_bb_animate_counts_out = (void *)bb_animate_counts_out;

struct BBBannerTimes { s32 values[3]; };
static const struct BBBannerTimes bb_banner_times = {{64, 16, 16}};

s32 fn_18_1CA08(void)
{
    s32 table_8[3] = {16, 16, 16};
    struct Sprite **cursor;
    struct BBCountOffsets offsets;
    float px;
    float tf3;
    float tf3_2;
    float tf4;
    s32 tr0;
    s32 r4;
    s32 r5;
    struct Sprite **r6;
    struct Sprite *tr4_2;
    struct Sprite *tr4_3;

    cursor = &lbl_18_bss_19270->unk338[0];
    tr0 = lbl_18_bss_19270->unk158;
    switch (tr0) {
    case 0:
        (*cursor)->x = 320.0f;
        (*cursor)->y = (96.0f + (0.140625f * (float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248)));
        (*cursor)->scaleX = (1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f));
        (*cursor)->scaleY = (1.0f - ((float) lbl_18_bss_19270->unk248 / 32.0f));
        tf4 = 32.0f - (0.125f * (float) ((0x20 - lbl_18_bss_19270->unk248) * (0x20 - lbl_18_bss_19270->unk248)));

        offsets = bb_count_offsets;
        r6 = &lbl_18_bss_19270->unk338[6];
        if (*r6 != NULL) {
            (*r6)->x = tf4;
            (*r6)->y = 410.0f;
            r6++;
            for (r5 = 0; r5 < 3; r5++) {
                for (r4 = 0; r4 < bb_count_limits[r5] - 1; r4++) {
                    px = bb_count_x(tf4, r4, lbl_18_bss_19270->unkCC[r5] + (r5 == -1 ? 1 : 0));
                    (*r6)->x = px;
                    (*r6)->y = 410.0f + offsets.y[locale == 5 ? 0 : 1][r5];
                    r6++;
                }
            }
        }

        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            fn_18_218EC(0x12);
            bb_free_sprites(8, 6);
        }
        break;
    case 1:
        cursor++;
        (*cursor)->opacity = (1.0f - ((float) lbl_18_bss_19270->unk248 / 16.0f));
        break;
    case 2:
        cursor++;
        tf3 = 1.0f - ((float) lbl_18_bss_19270->unk248 / 16.0f);
        tr4_2 = *cursor;
        bb_flash_sprite(tr4_2, tf3);
        break;
    case 3:
        cursor++;
        tf3_2 = lbl_18_bss_19270->unk248 / 16.0f;
        tr4_3 = *cursor;
        bb_flash_sprite(tr4_3, tf3_2);
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            return 0;
        }
        break;
    }
    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        lbl_18_bss_19270->unk248 = table_8[lbl_18_bss_19270->unk158++];
    }
    return 1;
}

static inline s32 bb_animate_winner(struct BBAnimateTables *tables)
{
    static const struct BBAnimate4 data = {{16, 16, 16, 32}};
    float y;
    struct Sprite **p;
    s32 i;

    tables->winner = data;
    switch (lbl_18_bss_19270->unk158) {
    case 0:
        if (lbl_18_bss_19270->unk8[0][0] == lbl_18_bss_19270->unk8[0][1])
            lbl_18_bss_19270->unk158 = 1;
    case 1:
        y = 96.0f - 0.5625f * (float)(lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248);
        lbl_18_bss_19270->unk338[lbl_18_bss_19270->unk158]->y = y;
        break;
    case 2:
    case 3:
        if (lbl_18_bss_19270->unk158 == 2)
            y = 1.0f - (float)lbl_18_bss_19270->unk248 / 16.0f;
        else
            y = (float)lbl_18_bss_19270->unk248 / 16.0f;
        bb_flash_sprite(lbl_18_bss_19270->unk338[0], y);
        bb_flash_sprite(lbl_18_bss_19270->unk338[1], y);
        break;
    case 4:
        y = 400.0f + 0.140625f * (float)(lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248);
        p = &lbl_18_bss_19270->unk338[18];
        i = 0;
        while (i < 4) {
            if (*p != NULL)
                (*p)->y = y;
            i++;
            p++;
        }
        (*p)->y = 4.0f + y;
        if (lbl_18_bss_19270->unk248 == 0)
            return 0;
    }
    if (lbl_18_bss_19270->unk248 == 0)
        lbl_18_bss_19270->unk248 = tables->winner.values[lbl_18_bss_19270->unk158++];
    return 1;
}

static void *bb_anchor_bb_animate_winner = (void *)bb_animate_winner;

void fn_18_1CF8C(void)
{
    s32 table_8[3] = {5639, 5745, 5746};
    struct Sprite **cursor;
    float f0;
    float f0_2;
    float f0_3;
    float f0_4;
    s32 tr28;
    s32 r0;
    s32 r4;
    s32 score;

    if ((s32) lbl_18_bss_19270->unk8[0][0] > (s32) lbl_18_bss_19270->unk8[0][1]) {
        r0 = 0;
    } else {
        r0 = 1;
    }
    tr28 = lbl_18_bss_19270->unk8C[r0];
    cursor = bb_get_sprites(2, 0x0);
    (*cursor)->bmpId = (u16) table_8[tr28];
    if (tr28 == 0) {
        f0 = 216.0f;
    } else {
        f0 = 224.0f;
    }
    (*cursor)->x = f0;
    (*cursor)->y = -256.0f;
    cursor++;
    if ((s32) lbl_18_bss_19270->unk8[0][0] != lbl_18_bss_19270->unk8[0][1]) {
        (*cursor)->bmpId = 0x1674;
        if (tr28 == 0) {
            f0_2 = 416.0f;
        } else {
            f0_2 = 376.0f;
        }
        (*cursor)->x = f0_2;
        (*cursor)->y = -256.0f;
    } else {
        (*cursor)->bmpId = 0x1608;
        (*cursor)->x = 320.0f;
        (*cursor)->y = -256.0f;
    }
    cursor = bb_get_sprites(5, 0x12);
    for (r4 = 0; r4 < 2; r4++) {
        score = lbl_18_bss_19270->unk8[0][r4];
        (*cursor)->bmpId = (u16) ((score % 10) + 0x163A);
        if (r4 == 0) {
            f0_3 = 236.0f;
        } else {
            f0_3 = 492.0f;
        }
        (*cursor)->x = f0_3;
        (*cursor)->y = 640.0f;
        if ((s32) score < 0xA) {
            (*cursor)->textAlign = 7;
            cursor++;

            g_poolInfo.spritePool.statusList[(s8) (*cursor)->poolIdx] = 0;
            *cursor = NULL;
        } else {
            cursor++;
            (*cursor)->bmpId = (u16) ((score / 10) + 0x163A);
            if (r4 == 0) {
                f0_4 = 156.0f;
            } else {
                f0_4 = 412.0f;
            }
            (*cursor)->x = f0_4;
            (*cursor)->y = 640.0f;
        }
        cursor++;
    }
    (*cursor)->bmpId = 0x1644;
    (*cursor)->depth = 0.2f;
    (*cursor)->x = 320.0f;
    (*cursor)->y = 640.0f;
    lbl_18_bss_19270->unk248 = 0x10;
}

void fn_18_1D480(void)
{
    struct Sprite **cursor;
    float tf0;
    float tf1;
    float tf2;
    s32 r5;

    cursor = &lbl_18_bss_19270->unk338[34];
    tf0 = (*cursor)->x;
    tf1 = (*cursor)->y;
    tf2 = (*cursor)->scaleX;
    (*cursor)->widescreenArg = (s16) tf0;
    cursor++;
    (*cursor)->x = (tf0 + (6.4444447f * lbl_18_bss_19270->unkE4 * tf2));
    (*cursor)->y = (tf1 + ((108.0f + (6.4444447f * lbl_18_bss_19270->unkE8)) * tf2));
    (*cursor)->scaleX = tf2;
    (*cursor)->scaleY = tf2;
    (*cursor)->widescreenArg = (s16) tf0;
    cursor++;
    r5 = 0;
    while (r5 < 7) {
        (*cursor)->x = (tf0 + (6.4444447f * lbl_18_bss_19270->unkEC[r5].x * tf2));
        (*cursor)->y = (tf1 + ((108.0f + (6.4444447f * lbl_18_bss_19270->unkEC[r5].z)) * tf2));
        (*cursor)->scaleX = tf2;
        (*cursor)->scaleY = tf2;
        (*cursor)->widescreenArg = (s16) tf0;
        r5++;
        cursor++;
    }
    r5 = 0;
    while (r5 < 4) {
        if (r5 < (s32) lbl_18_bss_19270->unk144) {
            (*cursor)->x = (tf0 + (6.4444447f * lbl_18_bss_19270->unk124[r5].x * tf2));
            (*cursor)->y = (tf1 + ((108.0f + (6.4444447f * lbl_18_bss_19270->unk124[r5].z)) * tf2));
            (*cursor)->scaleX = tf2;
            (*cursor)->scaleY = tf2;
        } else {
            (*cursor)->y = -256.0f;
        }
        (*cursor)->widescreenArg = (s16) tf0;
        r5++;
        cursor++;
    }
}

static inline s32 bb_animate_map(void)
{
    struct Sprite **cursor;
    float tf0_6;
    float f2;
    s32 r4;

    cursor = &lbl_18_bss_19270->unk338[0x22];
    if ((s32) lbl_18_bss_19270->unk154 == 0x36) {
        f2 = ((0x10 - lbl_18_bss_19270->unk248) * (0x10 - lbl_18_bss_19270->unk248)) / 256.0f;
    } else {
        f2 = (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 256.0f;
    }
    (*cursor)->x = (540.0f + (-220.0f * f2));
    (*cursor)->y = (80.0f + (160.0f * f2));
    tf0_6 = 0.5f + (0.5f * f2);
    (*cursor)->scaleY = tf0_6;
    (*cursor)->scaleX = tf0_6;
    fn_18_1D480();
    r4 = lbl_18_bss_19270->unk248 > 0;
    return r4;
}

static void *bb_anchor_bb_animate_map = (void *)bb_animate_map;

void fn_18_1D6C4(void *sprite)
{
    struct Sprite *arg0 = sprite;
    s32 r0;

    textdraw_reset();
    textdraw_set_font((s32) arg0->fontId);
    textdraw_set_flags(0x80000000);
    textdraw_set_depth(arg0->depth);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_scale(arg0->scaleX, arg0->scaleY);
    if ((s32) *(u8 *) arg0->text < 0xC) {
        r0 = 3;
    } else {
        r0 = 0;
    }
    textdraw_set_alignment((s8) r0);
    textdraw_set_pos(arg0->x, arg0->y - (12.0f * arg0->scaleY));
    set_ui_widescreen_scale_mtx(0x140);
    textdraw_print_and_fit_to_width(432.0f, lbl_18_data_7FC64[locale][*(u8 *) arg0->text]);
    reset_ui_widescreen_scale_mtx();
}

struct BBHelpBitmaps { s32 ids[7]; };
static const struct BBHelpBitmaps bb_help_bitmaps = {{5763, 5766, 5764, 5760, 5762, 5765, 5761}};

static inline void bb_set_help(struct BBCreateHelp *bitmaps, s32 index)
{
    struct Sprite **p;
    s32 i;

    *(struct BBHelpBitmaps *)bitmaps = bb_help_bitmaps;
    p = &lbl_18_bss_19270->unk338[51];
    textdraw_set_flags(0x80000000);
    for (i = 0; i < 3; i++) {
        ((u8 *)(*p)->text)[0] = (u8)(i + index * 3);
        (*p)->text[1] = 0;
        if (index < 4)
            (*p)->x = 320.0f;
        else
            (*p)->x = 144.0f;
        p++;
    }
    if (index >= 4) {
        (*p)->bmpId = bitmaps->ids[index - 4];
        (*p)->x = 96.0f;
    } else {
        (*p)->x = 1000.0f;
    }
}
static inline void bb_set_help_created(struct BBCreateHelp *bitmaps)
{
    struct Sprite **p;
    s32 index;
    s32 i;

    index = lbl_18_bss_19270->unk14C;
    *(struct BBHelpBitmaps *)bitmaps = bb_help_bitmaps;
    p = &lbl_18_bss_19270->unk338[51];
    textdraw_set_flags(0x80000000);
    for (i = 0; i < 3; i++) {
        ((u8 *)(*p)->text)[0] = (u8)(i + index * 3);
        (*p)->text[1] = 0;
        if (index < 4)
            (*p)->x = 320.0f;
        else
            (*p)->x = 144.0f;
        p++;
    }
    if (index >= 4) {
        (*p)->bmpId = bitmaps->ids[index - 4];
        (*p)->x = 96.0f;
    } else {
        (*p)->x = 1000.0f;
    }
}
static void *bb_anchor_bb_set_help = (void *)bb_set_help;

static inline s32 bb_animate_help(void)
{
    struct Sprite **cursor;
    float tf1_5;

    cursor = &lbl_18_bss_19270->unk338[50];
    tf1_5 = 416.0f + 0.5f * (float)(lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248);
    (*cursor++)->y = tf1_5;
    (*cursor++)->y = tf1_5 - 26.0f;
    (*cursor++)->y = tf1_5;
    (*cursor++)->y = 26.0f + tf1_5;
    (*cursor)->y = tf1_5;
    return lbl_18_bss_19270->unk248 > 0;
}

static void *bb_anchor_bb_animate_help = (void *)bb_animate_help;

static inline void bb_create_counters(struct BBCreateTables *tables)
{
    s32 i;
    struct Sprite **p;
    p = bb_get_sprites_counter(8, 0x6);
    (*p)->widescreenArg = 0x60;
    (*p++)->bmpId = (u16)(locale == 5 ? 0x1636 : 0x161B);
    for (i = 0; i < 2; i++) {
        (*p)->widescreenArg = 0x60;
        (*p++)->bmpId = 0x1638;
    }
    for (i = 0; i < 3; i++) {
        (*p)->widescreenArg = 0x60;
        (*p++)->bmpId = 0x1637;
    }
    for (i = 0; i < 2; i++) {
        (*p)->widescreenArg = 0x60;
        (*p++)->bmpId = 0x1639;
    }

    bb_position_counts_created(tables, -1);
    lbl_18_bss_19270->unk154 = 0;
}

static inline void bb_create_banner_panel(void)
{
    struct Sprite **p;
    p = bb_get_sprites(2, 0);
    (*p)->bmpId = 0x1609;
    (*p)->x = 320.0f;
    (*p)->y = 240.0f;
    (*p)->scaleX = 0.0f;
    (*p)->scaleY = 0.0f;
    p++;
    (*p)->bmpId = 0x1600;
    (*p)->x = 320.0f;
    (*p)->y = 40.0f;
    (*p)->opacity = 0.0f;
    lbl_18_bss_19270->unk248 = 0x20;
}

static inline void bb_create_map_panel(s32 animate)
{
    s32 i;
    struct Sprite **p;
    p = bb_get_sprites(13, 0x22);
    (*p)->bmpId = 0x162F;
    (*p)->depth = 0.2f;
    if (lbl_18_bss_19270->unk154 == 0x34 || lbl_18_bss_19270->unk154 == 0x37) {
        (*p)->x = 320.0f;
        (*p)->y = 240.0f;
    } else {
        (*p)->x = 540.0f;
        (*p)->y = 80.0f;
        (*p)->scaleX = 0.5f;
        (*p)->scaleY = 0.5f;
    }
    p++;
    (*p)->bmpId = 0x1635;
    p++;
    for (i = 0; i < 7; i++, p++)
        (*p)->bmpId = 0x1631;
    for (i = 0; i < 4; i++, p++)
        (*p)->bmpId = 0x1633;
    fn_18_1D480();
    lbl_18_bss_19270->unk154 = 0;
    if (animate)
        lbl_18_bss_19270->unk248 = 16;
}

static inline void bb_create_help_panel(void)
{
    s32 i;
    struct Sprite **p;
    p = bb_get_sprites(5, 0x32);
    (*p)->bmpId = 0x167F;
    (*p)->depth = 0.2f;
    (*p)->x = 320.0f;
    (*p)->y = 640.0f;
    p++;
    for (i = 0; i < 3; p++, i++) {
        (*p)->type = 0;
        (*p)->fontId = 3;
        (*p)->textAlign = 1;
        (*p)->scaleX = 0.68f;
        (*p)->scaleY = 0.8f;
        (*p)->drawFunc = (void (*)(struct Sprite *))fn_18_1D6C4;
        (*p)->x = 144.0f;
        (*p)->y = 640.0f;
    }
    (*p)->y = 640.0f;
}

s32 g_get_sprite_work(struct BBScreen *arg0, u32 arg1)
{
    struct BBCreateTables tables;
    s32 r0_4;
    s32 r5;

    lbl_18_bss_19270 = arg0;
    if ((s32) lbl_18_bss_19270->unk24C[arg1] > 0) {
        return 0;
    }
    lbl_18_bss_19270->unk15C[arg1] = 0;
    lbl_18_bss_19270->unk248 = 0;
    lbl_18_bss_19270->unk154 = arg1;
    switch (arg1) {
    case 0x1:
        for (r5 = 2; r5 < 0x3B; r5++) {
            lbl_18_bss_19270->unk24C[r5] = 0;
        }
        bb_free_sprites(0x80, 0);
        lbl_18_bss_19270->unk154 = 0;
        break;
    case 0x1D:
        fn_18_1A35C();
        break;
    case 0x1F:
        bb_create_counters(&tables);
        break;
    case 0x1E:
        bb_create_start();
        break;
    case 0x20:
    case 0x21:
    case 0x22:
        bb_create_count_message(&tables);
        break;
    case 0x23:
        bb_create_strikeout();
        break;
    case 0x24:
    case 0x25:
    case 0x26:
        bb_create_result(&tables);
        break;
    case 0x27:
        bb_create_change();
        break;
    case 0x28:
        lbl_18_bss_19270->unk248 = 0x10;
        break;
    case 0x29:
        fn_18_1B6C4();
        break;
    case 0x2A:
        bb_create_score();
        break;
    case 0x2B:
        if ((s32) lbl_18_bss_19270->unk80 != 0) {
            r0_4 = 0;
        } else {
            r0_4 = 2;
        }
        lbl_18_bss_19274 = lbl_18_bss_19270->unk338[0x12 + r0_4]->scaleX;
        lbl_18_bss_19270->unk248 = 0x10;
        break;
    case 0x2C:
        lbl_18_bss_19270->unk248 = 0x20;
        break;
    case 0x2D:
        bb_create_summary(&tables);
        break;
    case 0x2E:
        fn_18_1C6AC();
        break;
    case 0x2F:
        lbl_18_bss_19270->unk248 = 0x20;
        break;
    case 0x30:
    case 0x31:
        bb_create_banner_panel();
        break;
    case 0x32:
        fn_18_1CF8C();
        break;
    case 0x33:
    case 0x34:
        bb_create_map_panel(0);
        break;
    case 0x35:
        fn_18_1D480();
        lbl_18_bss_19270->unk154 = 0;
        break;
    case 0x36:
    case 0x37:
        bb_create_map_panel(1);
        break;
    case 0x38:
        bb_create_help_panel();
        bb_set_help_created(&tables.help38);
        lbl_18_bss_19270->unk248 = 0x10;
        break;
    case 0x39:
        bb_set_help(&tables.help39, lbl_18_bss_19270->unk14C);
        lbl_18_bss_19270->unk154 = 0;
        break;
    case 0x3A:
        lbl_18_bss_19270->unk248 = 0x10;
        break;
    }
    lbl_18_bss_19270->unk24C[arg1] = lbl_18_bss_19270->unk248;
    return 1;
}

static inline s32 bb_animate_strikeout(void)
{
    struct Sprite **r6_2;
    s32 tr0_4;
    float tf0;
    r6_2 = &lbl_18_bss_19270->unk338[0];
    tr0_4 = lbl_18_bss_19270->unk158;
    switch (tr0_4) {
    case 0:
        tf0 = 1.0f - ((float) (lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 256.0f);
        (*r6_2)->scaleX = tf0;
        (*r6_2)->scaleY = tf0;
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            fn_18_218EC(0xE);
            lbl_18_bss_19270->unk248 = 0x80;
            lbl_18_bss_19270->unk158++;
        }
        break;
    case 1:
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            bb_free_sprites(1, 0);
            return 0;
        } else {
            break;
        }
        break;
    default:
        return 0;
    }

    return 1;
}

/* Preserve the shared dispatcher frame and inline state-return branches. */
#pragma always_inline on
#pragma inline_max_size(10000)
#pragma inline_max_total_size(100000)
static inline s32 bb_animate_banner_jp(struct BBAnimateTables *tables)
{
    struct Sprite **cursor;
    *(struct BBBannerTimes *)&tables->banner = bb_banner_times;
    cursor = &lbl_18_bss_19270->unk338[0];
    switch (lbl_18_bss_19270->unk158) {
    case 0:
        bb_animate_counts_out_void(cursor, tables->banner_y);
        break;
    case 2:
        (*cursor)->scaleY = (float)(lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 256.0f;
        if (lbl_18_bss_19270->unk248 == 0)
            (*cursor)->bmpId = 0x1606;
        break;
    case 3:
        (*cursor)->scaleY = 1.0f - (float)(lbl_18_bss_19270->unk248 * lbl_18_bss_19270->unk248) / 256.0f;
        if (lbl_18_bss_19270->unk248 == 0) {
            return 0;
        }
        break;
    }
    if (lbl_18_bss_19270->unk248 == 0)
        lbl_18_bss_19270->unk248 = tables->banner.values[lbl_18_bss_19270->unk158++];
    return 1;
}
#pragma always_inline reset

static inline s32 bb_animate_banner_locales(struct BBAnimateTables *tables)
{
    if (locale == 5)
        return bb_animate_banner_jp(tables);
    return fn_18_1CA08();
}

static inline s32 bb_animate_score_flash(void)
{
    s32 r0;
    s32 tr4;
    s32 tr6;
    struct Sprite **tr3_10;
    struct Sprite *tr5;
    struct Sprite *tr5_2;
    struct Sprite *tr5_4;
    struct Sprite *tr4_5;
    s32 *tr5_3;
    float tf0_4;
    if ((s32) lbl_18_bss_19270->unk80 != 0) {
        r0 = 0;
    } else {
        r0 = 1;
    }
#ifdef __MWERKS__
    tr3_10 = (struct Sprite **)((u8 *)lbl_18_bss_19270 + r0 * 8 + 0x380);
#else
    tr3_10 = &lbl_18_bss_19270->unk338[0x12 + r0 * 2];   /* 0x380 = unk338[0x12] on 32-bit */
#endif
    tr4 = lbl_18_bss_19270->unk158;
    switch (tr4) {
    case 0:
        tf0_4 = 1.0f - ((float) lbl_18_bss_19270->unk248 / 16.0f);
        tr5 = tr3_10[0];
        bb_flash_sprite_scaled(tr5, tf0_4);
        tr5_2 = tr3_10[1];
        bb_flash_sprite_scaled(tr5_2, tf0_4);
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            tr5_3 = (s32 *)((u8 *)lbl_18_bss_19270 + r0 * 4);
            tr6 = tr5_3[2] + 1;
            tr5_3[2] = tr6;
            tr3_10[0x0]->bmpId = (u16) ((tr6 % 10) + 0x163A);
            tr3_10[0x1]->bmpId = (u16) ((tr6 / 10) + 0x163A);
            if (tr6 >= 0xA) {
                tr3_10[0x0]->textAlign = 4;
                tr3_10[0x1]->y = tr3_10[0x0]->y;
            }
            lbl_18_bss_19270->unk248 = 0x10;
            lbl_18_bss_19270->unk158++;
        }
        break;
    case 1:
        tf0_4 = lbl_18_bss_19270->unk248 / 16.0f;
        tr5_4 = tr3_10[0];
        bb_flash_sprite_scaled(tr5_4, tf0_4);
        tr4_5 = tr3_10[1];
        bb_flash_sprite_scaled(tr4_5, tf0_4);
        if ((s32) lbl_18_bss_19270->unk248 == 0) {
            return 0;
        } else {
            break;
        }
        break;
    default:
        return 0;
    }
    return 1;
}

static inline s32 bb_animate_summary_out(void)
{
    float tf2_2;
    float tf1_3;
    struct Sprite **r6_5;
    tf2_2 = (float)((0x20 - lbl_18_bss_19270->unk248) * (0x20 - lbl_18_bss_19270->unk248)) / 1024.0f;
    r6_5 = &lbl_18_bss_19270->unk338[50];
    (*r6_5)->y = 96.0f - 224.0f * tf2_2;
    r6_5++;
    (*r6_5)->y = 96.0f - 224.0f * tf2_2;
    r6_5++;
    (*r6_5)->scaleX = 1.0f - tf2_2;
    (*r6_5)->scaleY = 1.0f - tf2_2;
    r6_5++;
    tf1_3 = 144.0f - 304.0f * tf2_2;
    (*r6_5)->x = tf1_3;
    r6_5++;
    (*r6_5)->x = tf1_3 - 80.0f;
    r6_5++;
    (*r6_5)->x = 28.0f + tf1_3;
    r6_5++;
    tf1_3 = 496.0f + 304.0f * tf2_2;
    (*r6_5)->x = tf1_3;
    r6_5++;
    (*r6_5)->x = 80.0f + tf1_3;
    r6_5++;
    (*r6_5)->x = tf1_3 - 28.0f;
    if ((s32) lbl_18_bss_19270->unk248 == 0) {
        bb_free_sprites(9, 50);
        return 0;
    } else {
        return 1;
    }
}

void fn_18_1F400(struct BBScreen *arg0, s32 arg1)
{
    float tf1_6;
    s32 r30;
    s32 r4;
    struct Sprite **tr5_8;
    struct BBAnimateTables tables;

    r4 = arg1;
    if (r4 == 0) {
        lbl_18_bss_19270 = arg0;
        for (r30 = 0; r30 < 0x3B; r30++) {
            if ((s32) lbl_18_bss_19270->unk24C[r30] != 0) {
                lbl_18_bss_19270->unk154 = r30;
                lbl_18_bss_19270->unk158 = lbl_18_bss_19270->unk15C[r30];
                lbl_18_bss_19270->unk248 = (lbl_18_bss_19270->unk24C[r30] - 1);
                switch (r30) {
                case 0x1E:
                    r4 = bb_animate_start(&tables);
                    break;
                case 0x20:
                case 0x21:
                case 0x22:
                    r4 = fn_18_1AD98();
                    break;
                case 0x23:
                    r4 = bb_animate_strikeout();
                    break;
                case 0x24:
                case 0x25:
                case 0x26:
                    r4 = bb_animate_result(&tables);
                    break;
                case 0x27:
                    r4 = bb_animate_change(&tables);
                    break;
                case 0x28:
                    r4 = bb_animate_drop();
                    break;
                case 0x2B:
                    r4 = bb_animate_score_flash();
                    break;
                case 0x2C:
                    r4 = bb_animate_score();
                    break;
                case 0x2D:
                    r4 = bb_animate_summary();
                    break;
                case 0x2E:
                    r4 = fn_18_1C128();
                    break;
                case 0x2F:
                    r4 = bb_animate_summary_out();
                    break;
                case 0x30:
                    r4 = bb_animate_counts_out(&lbl_18_bss_19270->unk338[0], tables.count_y);
                    break;
                case 0x31:
                    r4 = bb_animate_banner_locales(&tables);
                    break;
                case 0x32:
                    r4 = bb_animate_winner(&tables);
                    break;
                case 0x36:
                case 0x37:
                    r4 = bb_animate_map();
                    break;
                case 0x38:
                    r4 = bb_animate_help();
                    break;
                case 0x3A:
                    tr5_8 = &lbl_18_bss_19270->unk338[50];
                    tf1_6 = 416.0f + (0.5f * (float)((0x10 - lbl_18_bss_19270->unk248) * (0x10 - lbl_18_bss_19270->unk248)));
                    (*tr5_8++)->y = tf1_6;
                    (*tr5_8++)->y = tf1_6 - 26.0f;
                    (*tr5_8++)->y = tf1_6;
                    (*tr5_8++)->y = 26.0f + tf1_6;
                    (*tr5_8)->y = tf1_6;
                    if ((s32) lbl_18_bss_19270->unk248 == 0) {
                        bb_free_sprites(5, 50);
                        r4 = 0;
                    } else {
                        r4 = 1;
                    }
                    break;
                }
                lbl_18_bss_19270->unk15C[r30] = lbl_18_bss_19270->unk158;
                lbl_18_bss_19270->unk24C[r30] = lbl_18_bss_19270->unk248;
                if (r4 == 0) {
                    lbl_18_bss_19270->unk24C[r30] = 0;
                }
            }
        }
    }
}
