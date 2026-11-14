/* The ranking screen: scrolling top-five entries, title sprites and the "REPLAY" banner. */
#pragma peephole off
#include "types.h"
#include "bitmap.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"
#include "recplay.h"
#include "sprite.h"
#include "textdraw.h"

extern u32 g_some_other_flags;
extern u32 g_font_char_var_3;
extern s32 locale;

void set_ui_widescreen_scale_mtx(int width);
void reset_ui_widescreen_scale_mtx(void);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_pos(float x, float y);
void textdraw_set_mul_color(u32 color);
void textdraw_set_scale(float scaleX, float scaleY);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_alignment(s8 alignment);
void textdraw_printf(const char *fmt, ...);
void textdraw_print(const char *str);
void *fn_1_FC1AC(int difficulty, int arg1);
void *fn_1_118BAC(int difficulty, int arg1);
void fn_1_B3C14(int bmpId, int arg1, int arg2);

/* Rank badge (one of the ten digit sprites is drawn per score column). */
struct RankingRecordA
{
    u8 unk0[0xC];
    /*0x0C*/ u8 floor;
    /*0x0D*/ s8 type;
    u8 unkE[2];
};  /* size = 0x10 */

struct RankingRecordB
{
    /*0x00*/ char name[8];
    /*0x08*/ s32 score;
    /*0x0C*/ s32 chara;
    /*0x10*/ u8 floor;
    /*0x11*/ s8 type;
    u8 unk12[0x1C - 0x12];
};  /* size = 0x1C */

struct RankingAccent
{
    s8 letter;
    u8 r, g, b;
};

struct RankingLetter
{
    s16 unk0;
    /*0x02*/ s16 angle;
    /*0x04*/ s16 angleVel;
    u8 unk6[2];
    /*0x08*/ float x;
    /*0x0C*/ float y;
    /*0x10*/ float floorY;
    /*0x14*/ float velY;
};  /* size = 0x18 */

struct RankingRow
{
    /*0x00*/ float x;
    /*0x04*/ float targetX;
    /*0x08*/ s32 delay;
    /*0x0C*/ struct RankingRecordB *recB;
    /*0x10*/ struct RankingRecordA *recA;
    /*0x14*/ struct RankingRecordB *nextRecB;
    /*0x18*/ struct RankingRecordA *nextRecA;
};  /* size = 0x1C */

struct RankingWork
{
    /*0x000*/ s32 active;
    /*0x004*/ u32 state;
    /*0x008*/ s32 timer;
    /*0x00C*/ float fade;
    /*0x010*/ float fadeTarget;
    /*0x014*/ float unk14;
    /*0x018*/ u32 flags;
    /*0x01C*/ s32 lettersActive;
    /*0x020*/ struct RankingLetter letters[7];
    /*0x0C8*/ struct RankingRow rows[5];
    /*0x154*/ s32 barWidth;
    /*0x158*/ s32 barWidthTarget;
    /*0x15C*/ struct Color3f color1;
    /*0x168*/ struct Color3f color1Target;
    /*0x174*/ struct Color3f color2;
    /*0x180*/ struct Color3f color2Target;
    /*0x18C*/ struct RankingAccent accent;
    /*0x190*/ struct RankingRecordA *recsA;
    /*0x194*/ struct RankingRecordB *recsB;
};  /* size = 0x198 */

struct ReplayBanner
{
    /*0x00*/ s32 active;
    /*0x04*/ s32 state;
    /*0x08*/ float x;
    /*0x0C*/ float unkC;
    /*0x10*/ float targetX;
    /*0x14*/ float width;
    /*0x18*/ s16 replay;
    /*0x1C*/ struct ReplayHeader header;
    /*0x34*/ s32 mulColor;
    /*0x38*/ s32 addColor;
    /*0x3C*/ char text[0x20];
};  /* size = 0x5C */

const u16 lbl_1_rodata_63E98[4] = { 0x000F, 0x0048, 0x0048, 0x0048 };
const u16 lbl_1_rodata_63EA0[4] = { 0x0020, 0x0031, 0x0031, 0x0031 };
const u16 lbl_1_rodata_63EA8[4] = { 0x0033, 0x0041, 0x0041, 0x0041 };
const u16 lbl_1_rodata_63EB0[4] = { 0x0003, 0x005D, 0x005D, 0x005D };

#include "replay_screen_data.inc"

static int lbl_1_data_52CA8[1] = { 1 };

int lbl_1_bss_C8868;
struct RankingWork lbl_1_bss_C886C;
struct ReplayBanner lbl_1_bss_C8A04;

void fn_1_CF730(void);
void fn_1_D0160(void);
void fn_1_D02B8(int rank, int x, int y, struct RankingRecordB *rec);
void fn_1_D1AF0(s8 *status, struct Sprite *sprite);
void fn_1_D1B38(struct Sprite *sprite);
void fn_1_D0EC4(void);
void fn_1_D12CC(void);
int fn_1_D1344(void);
void fn_1_D1264(s8 *status, struct Sprite *sprite);
void fn_1_D12AC(struct Sprite *sprite);
void fn_1_D147C(s8 *status, struct Sprite *sprite);
void fn_1_D1B58(void);
void fn_1_D1DA8(void);

static float unused_rodata0(int a)
{
    float c;
    double b;

    c = 255.0f;
    b = 0.8;
    b += 0.7;
    b += 0.6;
    return c * a * b + lbl_1_bss_C8868;
}

void fn_1_CEDC0(void)
{
    int i;
    struct RankingLetter *letter;
    struct RankingRow *row;
    int v;

    if (g_some_other_flags & 0xA)
        return;
    switch (lbl_1_bss_C886C.state)
    {
    case 1:
        if (lbl_1_bss_C886C.barWidth == 0)
        {
            if (lbl_1_bss_C886C.flags & 1)
                lbl_1_bss_C886C.state = 3;
            else
            {
                lbl_1_bss_C886C.state = 2;
                lbl_1_bss_C886C.timer = 90;
            }
            lbl_1_bss_C886C.color1.r = 512.0f;
            lbl_1_bss_C886C.color1.g = 512.0f;
            lbl_1_bss_C886C.color1.b = 512.0f;
        }
        else
            lbl_1_bss_C886C.state = 3;
        lbl_1_bss_C886C.barWidthTarget = 640;
        break;
    case 2:
        lbl_1_bss_C886C.timer--;
        if (lbl_1_bss_C886C.timer < 0)
            lbl_1_bss_C886C.state = 3;
        break;
    case 3:
        if (lbl_1_bss_C886C.lettersActive != 0)
            lbl_1_bss_C886C.state = 5;
        else
        {
            lbl_1_bss_C886C.state = 4;
            lbl_1_bss_C886C.timer = 0;
            lbl_1_bss_C886C.lettersActive = 1;
            {
                int j;
                struct RankingLetter *l = lbl_1_bss_C886C.letters;

                for (j = 7; j > 0; j--, l++)
                {
                    l->angle = (s16)(rand() & 0x7FFF);
                    l->angleVel = (s16)((rand() & 0x3FFF) - 0x1FFF);
                    l->y = -32.0 + -128.0 * (rand() / 32767.0f);
                    l->velY = 16.0 * (rand() / 32767.0f);
                }
            }
        }
        lbl_1_bss_C886C.fadeTarget = 1.0f;
        break;
    case 4:
        lbl_1_bss_C886C.timer--;
        if (lbl_1_bss_C886C.timer < 0)
            lbl_1_bss_C886C.state = 5;
        break;
    case 5:
        lbl_1_bss_C886C.state = 7;
        row = lbl_1_bss_C886C.rows;
        v = 0;
        for (i = 5; i > 0; i--, row++, v += 12)
        {
            if (740.0f == row->x)
            {
                row->delay = v;
                row->targetX = 0.0f;
            }
        }
        break;
    case 8:
        lbl_1_bss_C886C.state = 0;
        lbl_1_bss_C886C.barWidthTarget = 0;
        lbl_1_bss_C886C.fadeTarget = 0.0f;
        fn_1_D12CC();
        break;
    }

    if (lbl_1_bss_C886C.lettersActive != 0)
    {
        letter = lbl_1_bss_C886C.letters;
        for (i = 7; i > 0; i--, letter++)
        {
            letter->velY += 0.512;
            letter->y += letter->velY;
            letter->angleVel = (s16)(letter->angleVel - (letter->angleVel >> 5));
            letter->angle += letter->angleVel;
            if (letter->y > letter->floorY)
            {
                letter->y = letter->floorY;
                if (letter->velY > 0.0)
                    letter->velY *= -0.15 * (3.0 - math_sin((s16)(letter->angle + 0x4000)));
                if (letter->angle > -16 && letter->angle < 16)
                {
                    if (letter->angle < 0)
                        letter->angle++;
                    else if (letter->angle > 0)
                        letter->angle--;
                }
                else
                    letter->angle = (s16)(letter->angle - (letter->angle >> 4));
                letter->angleVel = (s16)(letter->angleVel >> 2);
                letter->angleVel = (s16)(letter->angleVel - (letter->angle >> 4));
            }
        }
    }

    if (lbl_1_bss_C886C.fade > lbl_1_bss_C886C.fadeTarget)
    {
        lbl_1_bss_C886C.fade -= 0.03333333333333333;
        if (lbl_1_bss_C886C.fade < lbl_1_bss_C886C.fadeTarget)
            lbl_1_bss_C886C.fade = lbl_1_bss_C886C.fadeTarget;
    }
    else if (lbl_1_bss_C886C.fade < lbl_1_bss_C886C.fadeTarget)
    {
        lbl_1_bss_C886C.fade += 0.03333333333333333;
        if (lbl_1_bss_C886C.fade > lbl_1_bss_C886C.fadeTarget)
            lbl_1_bss_C886C.fade = lbl_1_bss_C886C.fadeTarget;
    }

    row = lbl_1_bss_C886C.rows;
    for (i = 5; i > 0; i--, row++)
    {
        if (row->recA != NULL)
        {
            if (row->delay > 0)
                row->delay--;
            else
            {
                if (row->nextRecA != NULL)
                    row->targetX = -562.0f;
                if (row->x > row->targetX)
                {
                    row->x -= 21.333333333333332;
                    if (row->x < row->targetX)
                        row->x = row->targetX;
                }
                else if (row->x < row->targetX)
                {
                    row->x += 21.333333333333332;
                    if (row->x > row->targetX)
                        row->x = row->targetX;
                }
                if (-562.0f == row->x)
                {
                    row->x = 740.0f;
                    row->targetX = 0.0f;
                    row->recA = row->nextRecA;
                    row->recB = row->nextRecB;
                    row->nextRecA = NULL;
                    row->nextRecB = NULL;
                    row->delay = 0;
                }
            }
        }
    }

    if (lbl_1_bss_C886C.barWidth < lbl_1_bss_C886C.barWidthTarget)
    {
        lbl_1_bss_C886C.barWidth += 21;
        if (lbl_1_bss_C886C.barWidth > lbl_1_bss_C886C.barWidthTarget)
            lbl_1_bss_C886C.barWidth = lbl_1_bss_C886C.barWidthTarget;
    }
    else if (lbl_1_bss_C886C.barWidth > lbl_1_bss_C886C.barWidthTarget)
    {
        lbl_1_bss_C886C.barWidth -= 21;
        if (lbl_1_bss_C886C.barWidth < lbl_1_bss_C886C.barWidthTarget)
            lbl_1_bss_C886C.barWidth = lbl_1_bss_C886C.barWidthTarget;
    }

    lbl_1_bss_C886C.color1.r += 0.045 * (lbl_1_bss_C886C.color1Target.r - lbl_1_bss_C886C.color1.r);
    lbl_1_bss_C886C.color1.g += 0.045 * (lbl_1_bss_C886C.color1Target.g - lbl_1_bss_C886C.color1.g);
    lbl_1_bss_C886C.color1.b += 0.045 * (lbl_1_bss_C886C.color1Target.b - lbl_1_bss_C886C.color1.b);
    lbl_1_bss_C886C.color2.r += 0.045 * (lbl_1_bss_C886C.color2Target.r - lbl_1_bss_C886C.color2.r);
    lbl_1_bss_C886C.color2.g += 0.045 * (lbl_1_bss_C886C.color2Target.g - lbl_1_bss_C886C.color2.g);
    lbl_1_bss_C886C.color2.b += 0.045 * (lbl_1_bss_C886C.color2Target.b - lbl_1_bss_C886C.color2.b);

    if (lbl_1_bss_C886C.barWidth == 0
     && lbl_1_bss_C886C.fadeTarget == lbl_1_bss_C886C.fade
     && fn_1_D1344() != 0)
        fn_1_D0EC4();
}



void fn_1_CF730(void)
{
    struct SpriteDrawRequest reqD;
    int div;
    int i;
    int x;
    int y;
    struct RankingRecordB *rec;
    struct RankingRow *row;
    int col;
    int rem;
    int digit;
    float unusedX;
    float unusedY;
    double t;

    set_ui_widescreen_scale_mtx(320);
    fn_1_D0160();
    reset_ui_widescreen_scale_mtx();
    {
        struct SpriteDrawRequest req = { 0x32A, 0.0f, 20.0f, 5.0f, 80.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };

        req.bmpId = lbl_1_data_525B8[lbl_1_bss_C886C.accent.letter - '0'];
        draw_sprite_draw_request(&req);
        req.y = 440.0f;
        draw_sprite_draw_request(&req);
    }
    set_ui_widescreen_scale_mtx(320);
    {
        struct SpriteDrawRequest req = { 0x329, 320.0f, 16.0f, 0.3f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x7, -1, 0 };

        req.mulColor = 0;
        req.opacity = 0.5f;
        req.z = 4.5f;
        req.scaleX = 0.9f;
        req.y = 22.0f;
        draw_sprite_draw_request(&req);
        req.mulColor = -1;
        req.opacity = 1.0f;
        req.z = 4.0f;
        req.scaleX = 1.0f;
        req.y = 16.0f;
        draw_sprite_draw_request(&req);
        req.flags = 0x80000;
        req.mulColor = 0;
        req.opacity = 0.5f;
        req.z = 4.5f;
        req.scaleX = 0.9f;
        req.y = 22.0f;
        draw_sprite_draw_request(&req);
        req.mulColor = -1;
        req.opacity = 1.0f;
        req.z = 4.0f;
        req.scaleX = 1.0f;
        req.y = 16.0f;
        draw_sprite_draw_request(&req);
    }
    reset_ui_widescreen_scale_mtx();
    textdraw_reset();
    textdraw_set_alpha(1.0f);
    reqD.bmpId = 0x329;
    reqD.z = 1.03f;
    unusedX = 0.59375f;
    unusedY = 0.484375f;
    reqD.rotation = 0;
    reqD.opacity = 1.0f;
    reqD.unk30 = 2;
    reqD.flags = 5;
    reqD.mulColor = 0xFFFFFF;
    reqD.addColor = 0;
    row = lbl_1_bss_C886C.rows;
    t = g_font_char_var_3 % 15 / 15.0;
    if (!t)
    {
        lbl_1_bss_C8868++;
        if (lbl_1_bss_C8868 == 4)
            lbl_1_bss_C8868 = 0;
        lbl_1_data_52CA8[0]++;
        if (lbl_1_data_52CA8[0] == 4)
            lbl_1_data_52CA8[0] = 0;
    }
    for (i = 0; i < 5; i++, y += 46, row++)
    {
        struct RankingRecordA *recA = row->recA;

        rec = row->recB;
        if (recA != NULL && rec != NULL)
        {
            if (main_mode == 5)
                y = 116.0 + 62.0 * i;
            else
                y = 112.0 + 62.0 * i;
            x = row->x;
            reqD.bmpId = 0x314;
            reqD.z = 5.0f;
            reqD.x = x;
            reqD.y = y;
            reqD.scaleX = 2.5f;
            reqD.scaleY = 1.0f;
            reqD.u1 = 0.0f;
            reqD.v1 = 0.0f;
            reqD.u2 = 1.0f;
            reqD.v2 = 1.0f;
            reqD.opacity = 0.9f;
            draw_sprite_draw_request(&reqD);
            set_ui_widescreen_scale_mtx(320);
            textdraw_reset();
            textdraw_set_font(0x22);
            textdraw_set_alignment(0);
            textdraw_set_alpha(1.0f);
            textdraw_set_depth(1.0f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(x + 39, y + 3);
            textdraw_printf("%d", i + 1);
            textdraw_set_alpha(1.0f);
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(x + 36, y);
            textdraw_printf("%d", i + 1);
            reset_ui_widescreen_scale_mtx();
            set_ui_widescreen_scale_mtx(320);
            textdraw_set_font(0x20);
            textdraw_set_scale(0.99f, 0.99f);
            textdraw_set_alignment(0);
            textdraw_set_depth(2.0f);
            textdraw_set_alpha(0.8f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(x + 122, y + 3);
            textdraw_print(rec->name);
            textdraw_set_depth(1.0f);
            textdraw_set_alpha(1.0f);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(x + 119, y);
            textdraw_print(rec->name);
            reset_ui_widescreen_scale_mtx();
            set_ui_widescreen_scale_mtx(320);
            fn_1_D02B8(i, x + 340, y, rec);
            reset_ui_widescreen_scale_mtx();
            set_ui_widescreen_scale_mtx(320);
            {
                int started;

                rem = rec->score;
                started = 0;
                for (div = 1000000, col = 0; div > 0; div /= 10, col++)
                {
                    if (started || div < 10 || rem / div >= 0)
                    {
                        digit = rem / div;
                        lbl_1_data_52AA4[digit]->opacity = 1.0f;
                        lbl_1_data_52AA4[digit]->mulColor = -1;
                        lbl_1_data_52AA4[digit]->x = col * 18 + 407 + x;
                        lbl_1_data_52AA4[digit]->y = y + 2;
                        draw_sprite_draw_request(lbl_1_data_52AA4[digit]);
                        lbl_1_data_52AA4[digit]->opacity = 0.8f;
                        lbl_1_data_52AA4[digit]->x = col * 18 + 410 + x;
                        lbl_1_data_52AA4[digit]->y = y + 5;
                        lbl_1_data_52AA4[digit]->mulColor = 0;
                        draw_sprite_draw_request(lbl_1_data_52AA4[digit]);
                        rem %= div;
                        started = 1;
                    }
                }
            }
            {
                int a = x + 573;
                int b = y + 12;
                struct SpriteDrawRequest *req;
                const u16 *tbl;

                if (rec->chara < 0 || rec->chara >= 4)
                    return;
                tbl = lbl_1_data_52B6C[rec->chara];
                req = &lbl_1_data_52B1C;
                req->bmpId = tbl[lbl_1_data_52CA8[0]];
                req->x = a;
                req->y = b;
                req->z = 1.0f;
                req->opacity = 1.0f;
                req->rotation = 5120.0f * math_sin(g_font_char_var_3 << 9);
                draw_sprite_draw_request(req);
                req = &lbl_1_data_52B1C;
                req->bmpId = tbl[lbl_1_bss_C8868];
                req->z = 0.5f;
                req->opacity = 1.0 - g_font_char_var_3 % 15 / 15.0;
                draw_sprite_draw_request(req);
            }
            reset_ui_widescreen_scale_mtx();
        }
    }
    set_ui_widescreen_scale_mtx(320);
    digit = lbl_1_bss_C886C.accent.letter - '0';
    {
        struct SpriteDrawRequest reqA = { 0x32F, 146.0f, 28.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, 0xFFFFFFF, 0 };
        struct SpriteDrawRequest reqs[4] = {
            { 0x300, 390.0f, 28.0f, 0.1f, 1.0f, 0.25f, 0.0f, 0.0f, 1.0f, 0.25f, 0, 1.0f, -1, 0x6, -1, 0 },
            { 0x300, 390.0f, 28.0f, 0.1f, 1.0f, 0.25f, 0.0f, 0.25f, 1.0f, 0.5f, 0, 1.0f, -1, 0x6, -1, 0 },
            { 0x300, 390.0f, 28.0f, 0.1f, 1.0f, 0.25f, 0.0f, 0.5f, 1.0f, 0.75f, 0, 1.0f, -1, 0x6, -1, 0 },
            { 0x300, 390.0f, 28.0f, 0.1f, 1.0f, 0.25f, 0.0f, 0.75f, 1.0f, 1.0f, 0, 1.0f, -1, 0x6, -1, 0 },
        };

        draw_sprite_draw_request(&reqA);
        draw_sprite_draw_request(&reqs[digit]);
        reqA.mulColor = 0;
        reqA.x = 149.0f;
        reqA.y = 31.0f;
        reqA.z = 1.5f;
        reqA.opacity = 0.3f;
        reqs[digit].mulColor = 0;
        reqs[digit].x = 393.0f;
        reqs[digit].y = 31.0f;
        reqs[digit].z = 1.5f;
        reqs[digit].opacity = 0.3f;
        draw_sprite_draw_request(&reqA);
        draw_sprite_draw_request(&reqs[digit]);
    }
    reset_ui_widescreen_scale_mtx();
    g_some_sprite_reset_gx_state_func();
}

void fn_1_D0160(void)
{
    {
        struct SpriteDrawRequest req = { 0x324, 41.0f, 74.0f, 1.0f, 0.49107143f, 0.140625f, 0.0f, 0.0f, 0.49107143f, 0.140625f, 0, 1.0f, -1, 0x5, -1, 0 };

        req.opacity = lbl_1_bss_C886C.fade;
        draw_sprite_draw_request(&req);
    }
    {
        struct SpriteDrawRequest req = { 0x324, 188.0f, 74.0f, 1.0f, 0.5f, 0.140625f, 0.0f, 0.140625f, 0.5f, 0.28125f, 0, 1.0f, -1, 0x5, -1, 0 };

        req.opacity = lbl_1_bss_C886C.fade;
        draw_sprite_draw_request(&req);
    }
    {
        struct SpriteDrawRequest req = { 0x324, 322.0f, 74.0f, 1.0f, 0.625f, 0.140625f, 0.0f, 0.5625f, 0.625f, 0.703125f, 0, 1.0f, -1, 0x5, -1, 0 };

        req.opacity = lbl_1_bss_C886C.fade;
        draw_sprite_draw_request(&req);
    }
    {
        struct SpriteDrawRequest req = { 0x324, 429.0f, 74.0f, 1.0f, 0.58035713f, 0.140625f, 0.0f, 0.421875f, 0.58035713f, 0.5625f, 0, 1.0f, -1, 0x5, -1, 0 };

        req.opacity = lbl_1_bss_C886C.fade;
        draw_sprite_draw_request(&req);
    }
    {
        struct SpriteDrawRequest req = { 0x324, 538.0f, 74.0f, 1.0f, 0.6964286f, 0.140625f, 0.0f, 0.703125f, 0.6964286f, 0.84375f, 0, 1.0f, -1, 0x5, -1, 0 };

        req.opacity = lbl_1_bss_C886C.fade;
        draw_sprite_draw_request(&req);
    }
}

void fn_1_D02B8(int rank, int x, int y, struct RankingRecordB *rec)
{
    switch (rec->type)
    {
    case 0:
        if (rec->floor == 0xFF)
        {
            lbl_1_data_52ACC.mulColor = 0;
            lbl_1_data_52ACC.z = 2.0f;
            lbl_1_data_52ACC.x = x + 3;
            lbl_1_data_52ACC.y = y + 3;
            draw_sprite_draw_request(&lbl_1_data_52ACC);
            lbl_1_data_52ACC.x = x + 23;
            draw_sprite_draw_request(&lbl_1_data_52ACC);
            lbl_1_data_52ACC.mulColor = -1;
            lbl_1_data_52ACC.z = 1.0f;
            lbl_1_data_52ACC.x = x;
            lbl_1_data_52ACC.y = y;
            draw_sprite_draw_request(&lbl_1_data_52ACC);
            lbl_1_data_52ACC.x = x + 20;
            draw_sprite_draw_request(&lbl_1_data_52ACC);
        }
        else
        {
            textdraw_set_font(0x21);
            textdraw_set_alignment(0);
            textdraw_set_depth(2.0f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(x + 12, y + 2);
            textdraw_printf("%2d", rec->floor);
            textdraw_set_depth(1.0f);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(x + 10, y);
            textdraw_printf("%2d", rec->floor);
        }
        break;
    case 1:
        lbl_1_data_52694.mulColor = 0;
        lbl_1_data_52694.x = x - 1;
        lbl_1_data_52694.y = y + 2;
        lbl_1_data_52694.z = 0.1f;
        draw_sprite_draw_request(&lbl_1_data_52694);
        lbl_1_data_52694.mulColor = -1;
        lbl_1_data_52694.x = x - 3;
        lbl_1_data_52694.y = y;
        lbl_1_data_52694.z = 0.05f;
        draw_sprite_draw_request(&lbl_1_data_52694);
        textdraw_set_font(0x21);
        textdraw_set_alignment(0);
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(x + 12, y + 3);
        textdraw_printf("%2d", rec->floor);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(x + 10, y + 1);
        textdraw_printf("%2d", rec->floor);
        break;
    case 2:
        lbl_1_data_526E4.mulColor = 0;
        lbl_1_data_526E4.x = x - 1;
        lbl_1_data_526E4.y = y + 2;
        lbl_1_data_526E4.z = 0.1f;
        draw_sprite_draw_request(&lbl_1_data_526E4);
        lbl_1_data_526E4.mulColor = -1;
        lbl_1_data_526E4.x = x - 3;
        lbl_1_data_526E4.y = y;
        lbl_1_data_526E4.z = 0.05f;
        draw_sprite_draw_request(&lbl_1_data_526E4);
        textdraw_set_font(0x21);
        textdraw_set_alignment(0);
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(x + 10, y + 3);
        textdraw_printf("%2d", rec->floor);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(x + 10, y + 1);
        textdraw_printf("%2d", rec->floor);
        break;
    case 3:
        lbl_1_data_52734.mulColor = 0;
        lbl_1_data_52734.x = x - 1;
        lbl_1_data_52734.y = y + 2;
        lbl_1_data_52734.z = 0.1f;
        draw_sprite_draw_request(&lbl_1_data_52734);
        lbl_1_data_52734.mulColor = -1;
        lbl_1_data_52734.x = x - 3;
        lbl_1_data_52734.y = y;
        lbl_1_data_52734.z = 0.05f;
        draw_sprite_draw_request(&lbl_1_data_52734);
        textdraw_set_font(0x21);
        textdraw_set_alignment(0);
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(x + 10, y + 3);
        textdraw_printf("%2d", rec->floor);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(x + 10, y + 1);
        textdraw_printf("%2d", rec->floor);
        break;
    }
}

void fn_1_D0A84(int arg0, int x, int y, struct RankingRecordA *rec)
{
    float fx = x;
    float fy = y;
    float v;
    struct SpriteDrawRequest req;
    char buf[8];
    int len;

    req.z = 1.03f;
    req.rotation = 0;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 0x20005;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    switch (rec->type)
    {
    case 0:
        textdraw_set_font(3);
        if (rec->floor == 0xFF)
        {
            textdraw_set_pos(302.0 + fx, fy);
            textdraw_print("--");
        }
        else
        {
            sprintf(buf, "%d", rec->floor);
            len = strlen(buf);
            textdraw_set_pos(328.0 + fx - 13.0 * len, fy);
            textdraw_print(buf);
        }
        break;
    case 1:
        textdraw_set_font(3);
        sprintf(buf, "%d", rec->floor);
        textdraw_set_pos(32.0 + (328.0 + fx - 29.0), fy);
        textdraw_print(buf);
        req.bmpId = 0x30A;
        req.x = 328.0 + fx - 29.0;
        req.y = fy;
        req.scaleX = 0.96875f;
        req.scaleY = 0.96875f;
        req.u1 = 0.0f;
        req.v1 = 0.0f;
        req.u2 = 0.96875f;
        req.v2 = 0.96875f;
        draw_sprite_draw_request(&req);
        break;
    default:
        v = 0.0f;
        fn_1_B3C14(0x309, 1, 1);
        req.bmpId = 0x309;
        req.x = 328.0 + fx - 47.0;
        req.y = fy;
        req.scaleX = 0.7265625f;
        req.scaleY = 0.96875f;
        req.u1 = v;
        req.v1 = 0.0f;
        req.u2 = 0.7265625 + req.u1;
        req.v2 = 0.96875f;
        v = 384.0 * (math_sin((g_font_char_var_3 << 10) + arg0) - 0.5);
        if (v > 0.0)
        {
            int r;
            int g;
            int b;

            if (v < 128.0f)
                textdraw_set_alpha(1.0f - 0.0078125f * v);
            else
                textdraw_set_alpha(0.0f);
            r = v;
            g = (int)v - 24;
            b = (int)v - 64;
            if (r < 0)
                r = 0;
            else if (r > 0xFF)
                r = 0xFF;
            if (g < 0)
                g = 0;
            else if (g > 0xFF)
                g = 0xFF;
            if (b < 0)
                b = 0;
            else if (b > 0xFF)
                b = 0xFF;
            req.addColor = (r << 16) | (g << 8) | b;
        }
        draw_sprite_draw_request(&req);
        sprintf(buf, "%d", rec->floor);
        len = strlen(buf);
        textdraw_set_pos(328.0 + fx - 13.0 * len, fy);
        textdraw_set_depth(0.09f);
        textdraw_print(buf);
        textdraw_set_alpha(1.0f);
        break;
    }
}

void fn_1_D0EC4(void)
{
    int i;
    s16 *a;
    Vec *b;
    struct RankingLetter *letter;

    lbl_1_bss_C886C.active = 0;
    lbl_1_bss_C886C.state = 0;
    lbl_1_bss_C886C.lettersActive = 0;
    a = lbl_1_data_52630;
    b = lbl_1_data_52640;
    letter = lbl_1_bss_C886C.letters;
    for (i = 7; i > 0; i--, a++, b++, letter++)
    {
        letter->unk0 = *a;
        letter->x = b->x;
        letter->floorY = b->y;
    }
    lbl_1_bss_C886C.fade = 0.0f;
    lbl_1_bss_C886C.fadeTarget = 0.0f;
    lbl_1_bss_C886C.barWidth = 0;
    lbl_1_bss_C886C.barWidthTarget = 0;
}

void fn_1_D0FA4(int difficulty)
{
    int isNew;
    int i;
    struct RankingRow *row;
    struct RankingRecordA *recA;
    struct RankingRecordB *recB;
    int delay;

    isNew = !lbl_1_bss_C886C.active;
    if (lbl_1_bss_C886C.active == 0)
    {
        struct Sprite *sprite = create_sprite();

        if (sprite != NULL)
        {
            sprite->mainFunc = fn_1_D1264;
            sprite->drawFunc = fn_1_D12AC;
            strcpy(sprite->text, "RANKING");
        }
    }
    lbl_1_bss_C886C.active = 1;
    lbl_1_bss_C886C.state = 1;
    lbl_1_bss_C886C.color1Target = lbl_1_data_525C0[difficulty];
    lbl_1_bss_C886C.color2Target = lbl_1_data_525F0[difficulty];
    lbl_1_bss_C886C.accent = lbl_1_data_52620[difficulty];
    switch (main_mode)
    {
    case 5:
        lbl_1_bss_C886C.unk14 = 32.0f;
        lbl_1_bss_C886C.flags = 1;
        break;
    default:
        lbl_1_bss_C886C.unk14 = 0.0f;
        lbl_1_bss_C886C.flags = 0;
        break;
    }
    lbl_1_bss_C886C.recsA = fn_1_FC1AC(difficulty, 0);
    lbl_1_bss_C886C.recsB = fn_1_118BAC(difficulty, 0);
    row = lbl_1_bss_C886C.rows;
    recA = lbl_1_bss_C886C.recsA;
    recB = lbl_1_bss_C886C.recsB;
    delay = 0;
    for (i = 5; i > 0; i--, row++, recA++, recB++, delay += 12)
    {
        row->delay = delay;
        if (isNew)
        {
            row->recA = recA;
            row->recB = recB;
            row->nextRecA = NULL;
            row->nextRecB = NULL;
            row->targetX = 740.0f;
            row->x = row->targetX;
        }
        else
        {
            if (row->recA != NULL)
                row->nextRecA = recA;
            else
                row->recA = recA;
            if (row->recB != NULL)
                row->nextRecB = recB;
            else
                row->recB = recB;
        }
    }
}

void fn_1_D1220(int immediate)
{
    if (lbl_1_bss_C886C.active != 0)
    {
        if (immediate)
            lbl_1_bss_C886C.active = 0;
        else
            lbl_1_bss_C886C.state = 8;
    }
}

void fn_1_D1264(s8 *status, struct Sprite *sprite)
{
    fn_1_CEDC0();
    if (lbl_1_bss_C886C.active == 0)
        *status = 0;
}

void fn_1_D12AC(struct Sprite *sprite)
{
    fn_1_CF730();
}

void fn_1_D12CC(void)
{
    int i;
    struct RankingRow *row = lbl_1_bss_C886C.rows;
    int delay = 0;

    for (i = 5; i > 0; i--, row++, delay += 12)
    {
        row->targetX = -562.0f;
        row->nextRecA = NULL;
        row->nextRecB = NULL;
        if (0.0f == row->x && row->delay == 0)
            row->delay = delay;
    }
}

int fn_1_D1344(void)
{
    int ret;
    int i;
    struct RankingRow *row;
    int done = 1;

    row = lbl_1_bss_C886C.rows;
    for (i = 5; i > 0; i--, row++)
    {
        if (row->recA != NULL && row->recB != NULL && row->targetX != row->x && row->delay <= 0)
        {
            done = 0;
            break;
        }
    }
    if (done == 0)
        ret = 0;
    else
        ret = 1;
    return ret;
}

int fn_1_D13C0(void)
{
    return lbl_1_bss_C886C.barWidth;
}

void fn_1_D13D0(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->tag = 3;
        sprite->x = 16.0f;
        sprite->y = 16.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0xFF;
        sprite->textAlign = 0;
        sprite->bmpId = 0x30C;
        sprite->depth = 0.26f;
        sprite->mainFunc = fn_1_D147C;
        strcpy(sprite->text, "RNK TITILE");
    }
}

void fn_1_D147C(s8 *status, struct Sprite *sprite)
{
    float width = lbl_1_bss_C886C.barWidth;

    if (128.0 + sprite->x > 640.0 - width)
    {
        sprite->x = 640.0 - width - 128.0;
        sprite->rotation += 0x400;
    }
    if (width >= 640.0)
        *status = 0;
}

void g_replay_screen_stage_name(void)
{
    char *s1;
    char *s2;
    int len;

    lbl_1_bss_C8A04.mulColor = 0xFFFFFF;
    lbl_1_bss_C8A04.addColor = 0;
    if (lbl_1_bss_C8A04.header.flags & 0x20)
    {
        lbl_1_bss_C8A04.mulColor = 0xFFFF00;
        lbl_1_bss_C8A04.addColor = 0x404040;
    }
    if (lbl_1_bss_C8A04.header.flags & 0x1000)
    {
        sprintf(lbl_1_bss_C8A04.text, "WORLD %d-%d",
                lbl_1_bss_C8A04.header.difficulty + 1,
                lbl_1_bss_C8A04.header.floor + 1);
    }
    else
    {
        s2 = NULL;
        if (lbl_1_bss_C8A04.header.flags & 0x800)
        {
            s1 = lbl_1_data_52C90[locale];
            s2 = lbl_1_data_52C60[locale];
        }
        else if (lbl_1_bss_C8A04.header.flags & 0x40)
            s1 = lbl_1_data_52C90[locale];
        else if (lbl_1_bss_C8A04.header.flags & 0x20)
        {
            s1 = lbl_1_data_52C14[lbl_1_bss_C8A04.header.difficulty][locale];
            s2 = lbl_1_data_52C60[locale];
        }
        else
            s1 = lbl_1_data_52C14[lbl_1_bss_C8A04.header.difficulty][locale];
        if (s2 != NULL)
        {
            switch (locale)
            {
            case 2:
            case 3:
                sprintf(lbl_1_bss_C8A04.text, "%s %s %d", s2, s1, lbl_1_bss_C8A04.header.floor);
                break;
            default:
                sprintf(lbl_1_bss_C8A04.text, "%s %s %d", s1, s2, lbl_1_bss_C8A04.header.floor);
                break;
            }
        }
        else
            sprintf(lbl_1_bss_C8A04.text, "%s %d", s1, lbl_1_bss_C8A04.header.floor);
    }
    len = strlen(lbl_1_bss_C8A04.text) * 20;
    lbl_1_bss_C8A04.width = 40.0 + (float)len;
}

void fn_1_D1828(void)
{
    lbl_1_bss_C8A04.active = 0;
    lbl_1_bss_C8A04.state = 1;
    lbl_1_bss_C8A04.replay = (s16)lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14];
    fn_1_62AA0(lbl_1_bss_C8A04.replay, &lbl_1_bss_C8A04.header);
    g_replay_screen_stage_name();
    lbl_1_bss_C8A04.x = -lbl_1_bss_C8A04.width;
    lbl_1_bss_C8A04.unkC = 0.0f;
    lbl_1_bss_C8A04.targetX = lbl_1_bss_C8A04.x;
}

void fn_1_D1900(void)
{
    int isNew;

    isNew = !lbl_1_bss_C8A04.active;
    if (lbl_1_bss_C8A04.active == 0)
    {
        struct Sprite *sprite = create_sprite();

        if (sprite != NULL)
        {
            sprite->mainFunc = fn_1_D1AF0;
            sprite->drawFunc = fn_1_D1B38;
            strcpy(sprite->text, "RANKING");
        }
    }
    lbl_1_bss_C8A04.active = 1;
    lbl_1_bss_C8A04.state = 1;
    if (isNew)
    {
        lbl_1_bss_C8A04.replay = (s16)lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14];
        fn_1_62AA0(lbl_1_bss_C8A04.replay, &lbl_1_bss_C8A04.header);
        g_replay_screen_stage_name();
    }
}

void fn_1_D19FC(int immediate)
{
    if (immediate)
    {
        lbl_1_bss_C8A04.active = 0;
        lbl_1_bss_C8A04.state = 1;
        lbl_1_bss_C8A04.replay = (s16)lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14];
        fn_1_62AA0(lbl_1_bss_C8A04.replay, &lbl_1_bss_C8A04.header);
        g_replay_screen_stage_name();
        lbl_1_bss_C8A04.x = -lbl_1_bss_C8A04.width;
        lbl_1_bss_C8A04.unkC = 0.0f;
        lbl_1_bss_C8A04.targetX = lbl_1_bss_C8A04.x;
    }
    else
        lbl_1_bss_C8A04.state = 4;
}

void fn_1_D1AF0(s8 *status, struct Sprite *sprite)
{
    fn_1_D1B58();
    if (lbl_1_bss_C8A04.active == 0)
        *status = 0;
}

void fn_1_D1B38(struct Sprite *sprite)
{
    fn_1_D1DA8();
}

void fn_1_D1B58(void)
{
    float target;

    switch (lbl_1_bss_C8A04.state)
    {
    case 1:
        lbl_1_bss_C8A04.state = 2;
        lbl_1_bss_C8A04.targetX = 8.0f;
        break;
    case 2:
        if (lbl_1_bss_C8A04.replay != lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14])
        {
            lbl_1_bss_C8A04.state = 3;
            lbl_1_bss_C8A04.targetX = -lbl_1_bss_C8A04.width;
        }
        break;
    case 3:
        if (lbl_1_bss_C8A04.x == lbl_1_bss_C8A04.targetX)
        {
            lbl_1_bss_C8A04.state = 1;
            lbl_1_bss_C8A04.replay = (s16)lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14];
            fn_1_62AA0(lbl_1_bss_C8A04.replay, &lbl_1_bss_C8A04.header);
            g_replay_screen_stage_name();
        }
        break;
    case 4:
        lbl_1_bss_C8A04.state = 0;
        lbl_1_bss_C8A04.targetX = -lbl_1_bss_C8A04.width;
        break;
    }
    target = lbl_1_bss_C8A04.targetX;
    if (lbl_1_bss_C8A04.x > target)
    {
        lbl_1_bss_C8A04.x -= 10.666666666666666;
        if (lbl_1_bss_C8A04.x < target)
            lbl_1_bss_C8A04.x = target;
    }
    else if (lbl_1_bss_C8A04.x < target)
    {
        lbl_1_bss_C8A04.x += 10.666666666666666;
        if (lbl_1_bss_C8A04.x > target)
            lbl_1_bss_C8A04.x = target;
    }
}

void fn_1_D1DA8(void)
{
    float x = lbl_1_bss_C8A04.x;
    float y = 8.0f;

    if (x + lbl_1_bss_C8A04.width > 640.0 - lbl_1_bss_C886C.barWidth)
        x = (float)(640.0 - lbl_1_bss_C886C.barWidth) - lbl_1_bss_C8A04.width;
    textdraw_reset();
    textdraw_set_font(4);
    textdraw_set_pos(x, y);
    textdraw_set_mul_color(0xD0D0FF);
    textdraw_printf("%s", lbl_1_bss_C8A04.text);
    textdraw_set_mul_color(0xFFFFFF);
    y += 18.0f;
    textdraw_reset();
    textdraw_set_font(4);
    textdraw_set_pos(x, y);
    textdraw_printf("REPLAY");
}

float unused_rodata1(void)
{
    return 17.0f;
}
