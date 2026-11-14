/* Monkey Target 2 HUD: score, speed, altimeter, radar, wind and round banners. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "pilot2.h"
#include "pool.h"
#include "sprite.h"
#include "textdraw.h"
#include "va.h"

struct RadarDot
{
    /*0x00*/ Vec pos;
    /*0x0C*/ float scale;
};

struct RadarSet
{
    /*0x00*/ struct RadarDot *dots;
    /*0x04*/ s16 count;
};

struct ScreenPos
{
    s16 x;
    s16 y;
};

extern s32 active_monkey_id[];
extern s32 lbl_1_rodata_A838[];
extern float lbl_1_data_2E418[][3];
extern Vec lbl_1_bss_D4244;
extern u32 lbl_1_bss_D4A10;
extern struct GMA *minigameGma[];
extern u32 g_some_other_flags;
extern s32 locale;
extern float lbl_80198B34;
extern s16 lbl_13_bss_C;
extern s16 lbl_13_bss_12;
extern Vec lbl_13_bss_18;
extern s16 lbl_13_bss_2A;
extern s16 lbl_13_bss_2C;
extern int lbl_13_bss_30[];
extern int lbl_13_bss_200[];
extern int lbl_13_bss_244[];

void PSVECScale(Vec *src, Vec *dst, float scale);
void textdraw_reset(void);
void textdraw_set_font(int font);
void textdraw_set_font_style(int style);
void textdraw_set_scale(float x, float y);
void textdraw_set_spacing(float x, float y);
void textdraw_set_pos(float x, float y);
void textdraw_set_depth(float depth);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_mul_color(u32 color);
void textdraw_set_add_color(u32 color);
void textdraw_set_flags(u32 flags);
void textdraw_set_border(void);
void textdraw_set_drop_shadow(void);
void textdraw_print(char *str);
void textdraw_printf(char *fmt, ...);
void set_ui_widescreen_scale_mtx(int arg);
void reset_ui_widescreen_scale_mtx(void);

void fn_13_B020(void);
void fn_13_B174(void);
void fn_13_B1C8(struct Sprite *sprite);
void fn_13_B2C0(int player);
void fn_13_B640(struct Ball *ball);
void fn_13_BC28(struct Sprite *sprite);
void fn_13_BF5C(struct Sprite *sprite);
void fn_13_C2F4(struct Ball *ball);
void fn_13_C470(struct Sprite *sprite);
void fn_13_C510(struct Ball *ball);
void fn_13_CA10(struct Sprite *sprite);
void fn_13_CC70(struct Sprite *sprite);
void fn_13_CCF4(struct Ball *ball);
void fn_13_D050(struct Sprite *sprite);
void fn_13_D208(s8 *status, struct Sprite *sprite);
void fn_13_D228(struct Sprite *sprite);
void fn_13_D408(int player);
void fn_13_D64C(struct Sprite *sprite);
void fn_13_D718(s8 *status, struct Sprite *sprite);
void fn_13_D7DC(struct Sprite *sprite);
void fn_13_DB54(struct GMAModel *model, float x, float y, float scale);
void fn_13_DCA0(void);
void fn_13_DCD4(struct Sprite *sprite);
void fn_13_E1B4(int player);
void fn_13_E344(s8 *status, struct Sprite *sprite);
void fn_13_E47C(void);
void fn_13_E588(float top, float bottom);
void fn_13_E670(float left, float right);
void fn_13_E74C(void);
void fn_13_E8D8(s8 *status, struct Sprite *sprite);
void fn_13_E8E8(struct Sprite *sprite);
float fn_13_EB6C(struct Camera *camera, int anchor);
float fn_13_EBF8(struct Camera *camera, int anchor);
struct ScreenPos fn_13_EC84(int player, u32 anchor);
void g_some_printf_function_8(char *fmt, ...);
void g_some_printf_function_9(float x, float y, u32 mulColor, u32 addColor, char *fmt, ...);
void fn_13_F170(struct SpriteDrawRequest *req, float scaleX, float scaleY, float opacity);
void fn_13_F1D4(struct SpriteDrawRequest *req, int bmpId, float x, float y, float z);
int fn_13_F204(int value);
int fn_13_F238(int colorId);

const u16 lbl_13_rodata_3D0[4][6] =
{
    {0xF, 0xF, 0x46, 0x48, 0x48, 0x48},
    {0x20, 0x20, 0x2F, 0x31, 0x31, 0x31},
    {0x33, 0x33, 0x41, 0x41, 0x41, 0x41},
    {0x3, 0x5B, 0x5C, 0x5D, 0x5D, 0x5C}
};

u32 lbl_13_data_F10[] =
{
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000
};

struct RadarDot lbl_13_data_1080[] =
{
    {{0.0f, 0.0f, -500.0f}, 0.8f},
    {{0.0f, 0.0f, -1000.0f}, 0.8f},
    {{0.0f, 0.0f, -1500.0f}, 0.8f},
};

struct RadarDot lbl_13_data_10B0[] =
{
    {{0.0f, 0.0f, -500.0f}, 0.8f},
};

struct RadarDot lbl_13_data_10C0[] =
{
    {{0.0f, 0.0f, -500.0f}, 0.8f},
};

struct RadarSet lbl_13_data_10D0[] =
{
    {lbl_13_data_1080, 3},
    {lbl_13_data_10B0, 1},
    {lbl_13_data_10C0, 1},
    {lbl_13_data_1080, 3},
    {lbl_13_data_1080, 3},
    {lbl_13_data_1080, 3},
};

void fn_13_B020(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_13_BC28;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_13_BF5C;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_13_C470;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_13_CA10;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->bmpId = 0xB07;
        sprite->drawFunc = fn_13_CC70;
    }
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_13_D050;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->mainFunc = fn_13_D718;
        sprite->drawFunc = fn_13_D7DC;
        sprite->userVar = 0;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 135.0f;
        sprite->y = 215.0f;
        sprite->mainFunc = fn_13_D208;
        sprite->drawFunc = fn_13_D228;
    }
    fn_13_E47C();
    if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == 3)
        fn_13_E74C();
}

void fn_13_B174(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 200.0f;
        sprite->y = 170.0f;
        sprite->drawFunc = fn_13_B1C8;
    }
}

void fn_13_B1C8(struct Sprite *sprite)
{
    int i;
    struct Ball *ball;
    struct Pilot2Player *pl;

    for (i = 0; i < 5; i++)
    {
        ball = &balls[i];
        pl = &lbl_13_bss_70[ball->playerId];
        if (ball->status == 2
         && !(lbl_13_bss_70[lbl_13_bss_70[i].unk41].flags & 0x400)
         && (pl->flags & 4))
            fn_13_B640(ball);
    }
    if (lbl_1_bss_D4A10 & 8)
    {
        fn_13_B2C0(0);
        if (modeCtrl.unk30 == 2)
            fn_13_B2C0(1);
    }
}

void fn_13_B2C0(int player)
{
    struct SpriteDrawRequest req;
    float textX;
    float textY;
    float scaleX;
    float scaleY;
    float labelX;
    float offset;
    int count;
    int bmpId;
    struct Camera *camera;

    labelX = 0.0f;
    if (lbl_13_bss_70[player].flags & 0x400)
        return;
    count = lbl_13_bss_200[player];
    if (lbl_1_bss_D4A10 & 1)
    {
        if (modeCtrl.unk30 == 1 || lbl_13_bss_24 == 6 || lbl_13_bss_24 == 7)
            offset = 120.0f;
        else
            offset = 60.0f;
        camera = &cameras[player];
        scaleX = 0.6f;
        scaleY = 0.6f;
        textX = fn_13_EB6C(camera, 4) - 240.0f * scaleX;
        textY = fn_13_EBF8(camera, 2) + offset * scaleY + 100.0f * scaleY * lbl_13_bss_10;
        labelX = textX;
        textX += scaleX * (72.0f * (4.0f - fn_13_F204(count)));
        if (fn_13_F204(count) < 3)
            labelX += 72.0f * scaleX;
        else
            labelX = textX;
    }
    set_ui_widescreen_scale_mtx((int)fn_13_EB6C(camera, 4));
    textdraw_reset();
    textdraw_set_font(9);
    textdraw_set_scale(scaleX, scaleY);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_pos(textX, textY);
    textdraw_printf("%d", count);
    textdraw_set_scale(0.5f * scaleX, offset = 0.5f * scaleY);
    if (lbl_13_bss_24 == 6 || lbl_13_bss_24 == 7)
        textdraw_set_pos(labelX, textY - 64.0f * offset);
    else
        textdraw_set_pos(textX - 118.0f, textY + 32.0f * offset);
    textdraw_printf("TOTAL");
    labelX = 8.0f + (textX + 72.0f * scaleX * fn_13_F204(count));
    textY += 18.0f * scaleY;
    if (count == 1)
        bmpId = 0xB05;
    else
        bmpId = 0xB06;
    fn_13_F170(&req, scaleX, scaleY, 1.0f);
    fn_13_F1D4(&req, bmpId, labelX, textY, 0.1f);
    reset_ui_widescreen_scale_mtx();
}

void fn_13_B640(struct Ball *ball)
{
    struct SpriteDrawRequest req;
    struct Pilot2Player *pl;
    int base;
    int bmpId;
    u32 iconColor;
    int total;
    struct Camera *camera;
    float *color;
    u32 textColor;
    int multiplier;
    float r;
    float x;
    float y;
    float scaleX;
    float scaleY;
    float offset;
    float g;

    pl = &lbl_13_bss_70[ball->playerId];
    base = pl->unk2C;
    multiplier = pl->unk34;
    total = base * multiplier;
    if (lbl_1_bss_D4A10 & 1)
    {
        if (modeCtrl.unk30 == 1 || lbl_13_bss_24 == 6 || lbl_13_bss_24 == 7)
            offset = 120.0f;
        else
            offset = 60.0f;
        camera = &cameras[pl->unk41];
        scaleX = 0.6f;
        scaleY = 0.6f;
        x = fn_13_EB6C(camera, 4) - 240.0f * scaleX;
        y = fn_13_EBF8(camera, 2) + offset * scaleY + 100.0f * scaleY * pl->unk42;
        x += scaleX * (72.0f * (4.0f - fn_13_F204(total)));
        color = lbl_1_data_2E418[ball->playerId];
        r = 1.0f - 0.75f * (1.0f - color[0]);
        g = 1.0f - 0.75f * (1.0f - color[1]);
        offset = 1.0f - 0.75f * (1.0f - color[2]);
        textColor = ((u32)(255.0f * r) << 16) | ((u32)(255.0f * g) << 8) | (u32)(255.0f * offset);
        r = 1.0f - 0.5f * (1.0f - color[0]);
        g = 1.0f - 0.5f * (1.0f - color[1]);
        offset = 1.0f - 0.5f * (1.0f - color[2]);
        iconColor = ((u32)(255.0f * r) << 16) | ((u32)(255.0f * g) << 8) | (u32)(255.0f * offset);
    }
    else
    {
        if (ball->unk154 != 12 || pl->unk18 > 0)
            return;
        camera = &cameras[ball->playerId];
        if (lbl_13_bss_24 == 0)
        {
            scaleX = 1.0f;
            scaleY = 1.0f;
        }
        else
        {
            scaleX = 0.7f;
            scaleY = 0.7f;
        }
        x = fn_13_EB6C(camera, 4) - 120.0f * scaleX;
        y = fn_13_EBF8(camera, 4) - 70.0f * scaleY;
        x -= scaleX * (36.0f * (fn_13_F204(total) - 2.0f));
        textColor = 0xFFFFFF;
        iconColor = 0xFFFFFF;
    }
    set_ui_widescreen_scale_mtx((int)fn_13_EB6C(camera, 4));
    textdraw_reset();
    textdraw_set_font(9);
    textdraw_set_mul_color(textColor);
    textdraw_set_scale(scaleX, scaleY);
    textdraw_set_pos(x, y);
    textdraw_printf("%d", total);
    offset = 8.0f + (x + 72.0f * scaleX * fn_13_F204(total));
    x = y + 18.0f * scaleY;
    if (total == 1)
        bmpId = 0xB05;
    else
        bmpId = 0xB06;
    fn_13_F170(&req, scaleX, scaleY, 1.0f);
    req.mulColor = iconColor;
    fn_13_F1D4(&req, bmpId, offset, x, 0.1f);
    if (multiplier > 1)
    {
        int tx;
        int ty;

        scaleX = 0.5f * scaleX;
        x = 0.5f * scaleY;
        tx = offset - scaleX * (72.0f * (3.0f + fn_13_F204(base)));
        if (lbl_1_bss_D4A10 & 1)
            ty = y - 30.0f * scaleY;
        else
            ty = y - 40.0f * scaleY;
        textdraw_set_scale(scaleX, x);
        textdraw_set_mul_color(0xFFFF00);
        textdraw_set_pos(tx, ty);
        textdraw_printf("%d X %d", base, multiplier);
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_13_BC28(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float scale;
    float x;
    float y;
    int i;
    struct Ball *ball;
    struct Pilot2Player *pl;
    struct Camera *camera;

    if ((lbl_13_bss_12 < 4 || lbl_13_bss_12 > 5) && lbl_13_bss_12 < 13)
    {
        switch (lbl_13_bss_24)
        {
        case 0:
            scale = 1.0f;
            break;
        default:
            scale = 0.8f;
            break;
        }
        fn_13_F170(&req, scale, scale, 1.0f);
        for (i = 0; i < 4; i++)
        {
            ball = &balls[i];
            pl = &lbl_13_bss_70[ball->playerId];
            if (ball->status == 2)
            {
                if (pl->unk42 > 0)
                    break;
                camera = &cameras[ball->playerId];
                switch (lbl_13_bss_24)
                {
                case 0:
                    x = 30.0f + fn_13_EB6C(camera, 0);
                    y = 158.0f + fn_13_EBF8(camera, 2);
                    break;
                default:
                    x = 21.0f + fn_13_EB6C(camera, 0);
                    y = 50.0f + fn_13_EBF8(camera, 2);
                    break;
                }
                set_ui_widescreen_scale_mtx((int)(x + 30.0f * scale));
                if (locale == 0)
                    fn_13_F1D4(&req, 0xB2F, x, y, 0.11f);
                else
                    fn_13_F1D4(&req, 0xB1B, x, y, 0.11f);
                x += 15.0f * scale;
                y += 55.0f * scale;
                textdraw_reset();
                textdraw_set_scale(scale, scale);
                textdraw_set_font(0x59);
                if (locale == 0)
                {
                    textdraw_set_pos(x - 3.0f, y);
                    textdraw_printf("%2d", (int)(134.21985f * (1.5f * mathutil_vec_len(&lbl_13_bss_18))));
                }
                else
                {
                    textdraw_set_pos(x, y);
                    textdraw_printf("%2d", (int)(1.5f * (lbl_80198B34 * mathutil_vec_len(&lbl_13_bss_18))));
                }
            }
        }
        reset_ui_widescreen_scale_mtx();
    }
}

void fn_13_BF5C(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    int i;
    int j;
    int rank;
    u32 color;
    struct Ball *ball;
    struct Camera *camera;
    struct Pilot2Player *pl;

    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        pl = &lbl_13_bss_70[ball->playerId];
        if (ball->status == 2)
        {
            camera = &cameras[ball->playerId];
            fn_13_C2F4(ball);
            if (pl->unk42 > 0)
                break;
            if (modeCtrl.playerCount > 1)
            {
                rank = 0;
                for (j = 0; j < 4; j++)
                {
                    if (i != j && g_poolInfo.playerPool.statusList[j] != 0
                     && lbl_13_bss_30[j] >= lbl_13_bss_30[i])
                        rank++;
                }
            }
            color = 0xFFFFFF;
            if (modeCtrl.unk30 == 1)
            {
                x = 24.0f;
                y = 24.0f;
            }
            else
            {
                x = 16.0f + fn_13_EB6C(camera, 0);
                y = 16.0f + fn_13_EBF8(camera, 2);
            }
            set_ui_widescreen_scale_mtx((int)x);
            fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
            textdraw_reset();
            req.u1 = 0.25f * i;
            req.u2 = 0.25f + req.u1;
            req.v1 = 0.25f * fn_13_F238(ball->colorId);
            req.v2 = 0.25f + req.v1;
            req.scaleX = 0.25f;
            req.scaleY = 0.25f;
            if (modeCtrl.unk30 == 1)
            {
                fn_13_F1D4(&req, 0xB04, 60.0f + x, y, 0.1f);
                fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
                fn_13_F1D4(&req, 0xB01, 92.0f + x, y, 0.1f);
                textdraw_set_font(0x54);
                g_some_printf_function_9(60.0f + x, 18.0f + y, color, 0, "%6d", lbl_13_bss_30[i]);
                req.scaleX = 0.4375f;
                req.scaleY = 0.296875f;
                fn_13_F1D4(&req, lbl_1_rodata_A838[ball->ape->charaId], x, y, 0.1f);
            }
            else
            {
                fn_13_F1D4(&req, 0xB04, x, y, 0.1f);
                textdraw_set_font(0x55);
                g_some_printf_function_9(37.0f + x, y, color, 0, "%6d", lbl_13_bss_30[i]);
            }
        }
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_13_C2F4(struct Ball *ball)
{
    float x;
    float y;
    struct Camera *camera;
    struct Pilot2Player *pl;

    pl = &lbl_13_bss_70[ball->playerId];
    if (pl->unk42 > 0)
        ball = &balls[pl->unk41];
    if (pl->unk26 > 0)
    {
        if (!(g_some_other_flags & 0xA))
            pl->unk26--;
        if ((pl->unk26 >> 2) & 1)
        {
            camera = &cameras[ball->playerId];
            x = fn_13_EB6C(camera, 4);
            y = 40.0f + fn_13_EBF8(camera, 2);
            textdraw_reset();
            textdraw_set_font(0x55);
            textdraw_set_alignment(3);
            if (lbl_13_bss_24 != 0)
                textdraw_set_scale(0.8f, 0.8f);
            g_some_printf_function_9(x, y, 0xFFFF50, 0, "%3d", pl->unk30);
        }
    }
}

void fn_13_C470(struct Sprite *sprite)
{
    int i;
    struct Ball *ball;
    struct Pilot2Player *pl;

    switch (lbl_13_bss_24)
    {
    case 5:
        return;
    default:
        break;
    }
    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        pl = &lbl_13_bss_70[ball->playerId];
        if (ball->status == 2)
        {
            if (pl->unk42 > 0)
                break;
            fn_13_C510(ball);
        }
    }
}

void fn_13_C510(struct Ball *ball)
{
    struct SpriteDrawRequest req;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    s16 colorBmps[4] = {0xB22, 0xB20, 0xB23, 0xB21};
    struct Camera *camera;
    s16 bmpId;
    float x;
    float y;
    float iconX;
    float iconY;
    float gauge;
    float scale;

    camera = &cameras[ball->playerId];
    x = fn_13_EB6C(camera, 1) - 38.0f;
    switch (modeCtrl.unk30)
    {
    case 1:
        scale = 1.0f;
        y = 158.0f + fn_13_EBF8(camera, 2);
        break;
    default:
        scale = 0.8f;
        y = 50.0f + fn_13_EBF8(camera, 2);
        break;
    }
    set_ui_widescreen_scale_mtx((int)x);
    bmpId = 0xB1C;
    if (pl->flags & 0x1000)
    {
        if (pl->unk22 < 20.0f)
            pl->unk22++;
    }
    else if (pl->unk22 > 0)
        pl->unk22--;
    fn_13_F170(&req, scale, scale, 1.0f);
    fn_13_F1D4(&req, 0xB1D, x, y, 0.12f);
    gauge = 141.0f - 0.705f * ball->pos.y;
    if (gauge < -4.0f)
        gauge = -4.0f;
    else if (gauge > 181.0f)
        gauge = 181.0f;
    gauge *= scale;
    iconX = (x - 8.0) - 48.0f * scale;
    iconY = (y - 1.0f) + gauge;
    fn_13_F1D4(&req, 0xB1E, x - 7.0f * scale, iconY, 0.11f);
    fn_13_F1D4(&req, colorBmps[ball->colorId], iconX + 9.0f * scale, iconY - 19.0f * scale, 0.11f);
    if (pl->unk22 != 0)
    {
        req.flags = 0xF;
        req.scaleX = scale * (double)pl->unk22 / 20.0;
        req.scaleY = scale * req.scaleX;
        fn_13_F1D4(&req, bmpId, 10.0f + iconX, 10.0f + iconY, 0.11f);
        req.flags = 5;
    }
    req.scaleX = 0.25f * scale;
    req.scaleY = 0.1625f * scale;
    fn_13_F1D4(&req, lbl_1_rodata_A838[active_monkey_id[ball->playerId]], iconX + 15.0f * scale, iconY - 9.0f * scale, 0.11f);
    switch (modeCtrl.unk30)
    {
    case 1:
        req.scaleX = 3.0f;
        req.scaleY = 0.25f;
        fn_13_F1D4(&req, 0xB1F, x - 24.0, 5.0 + y, 0.12f);
        fn_13_F1D4(&req, 0xB1F, x - 24.0, 76.0 + y, 0.12f);
        fn_13_F1D4(&req, 0xB1F, x - 24.0, 147.0 + y, 0.12f);
        textdraw_reset();
        textdraw_set_depth(0.12f);
        textdraw_set_font(0x58);
        textdraw_set_pos(x - 24.0, y - 5.0);
        if (gauge <= -4.0f)
        {
            if ((g_font_char_var_3 >> 3) & 1)
                textdraw_printf("%d", (int)ball->pos.y);
        }
        else
            textdraw_printf("200");
        textdraw_set_pos(x - 24.0, 66.0 + y);
        textdraw_printf("100");
        textdraw_set_pos(16.0 + (x - 24.0), 137.0 + y);
        textdraw_printf("0");
        break;
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_13_CA10(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    float speed;
    int i;
    struct Ball *ball;
    struct Pilot2Player *pl;
    struct Camera *camera;

    switch (lbl_13_bss_24)
    {
    case 4:
    case 5:
    case 7:
        return;
    }
    fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        pl = &lbl_13_bss_70[ball->playerId];
        if (ball->status == 2)
        {
            if (pl->unk42 > 0)
                break;
            camera = &cameras[i];
            x = fn_13_EB6C(camera, 1) - 102.0f;
            y = fn_13_EBF8(camera, 3) - 30.0f;
            if (lbl_13_bss_24 == 6 || lbl_13_bss_24 == 7)
                x -= 20.0f;
            speed = mathutil_vec_len(&ball->vel);
            textdraw_reset();
            textdraw_set_font(0x56);
            textdraw_set_pos(x, y);
            set_ui_widescreen_scale_mtx((int)(fn_13_EB6C(camera, 1) - 16.0));
            if (locale == 0)
                textdraw_printf("%3d", (int)(134.21985f * speed));
            else
                textdraw_printf("%3d", (int)(216.0f * speed));
            fn_13_F1D4(&req, 0xB00, 45.0f + x, y, 0.1f);
            reset_ui_widescreen_scale_mtx();
        }
    }
}

void fn_13_CC70(struct Sprite *sprite)
{
    int i;
    struct Ball *ball;
    struct Pilot2Player *pl;

    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        pl = &lbl_13_bss_70[ball->playerId];
        if (ball->status == 2)
        {
            if (pl->unk42 > 0)
                break;
            fn_13_CCF4(ball);
        }
    }
}

void fn_13_CCF4(struct Ball *ball)
{
    struct SpriteDrawRequest req;
    Vec pos;
    struct Camera *camera;
    struct RadarDot *dots;
    int count;
    int i;
    int bmpId;
    float x;
    float y;
    float radius;
    float scale;
    float dx;
    float dy;
    float t;

    camera = &cameras[ball->playerId];
    switch (lbl_13_bss_24)
    {
    case 0:
        scale = 1.0f;
        dy = 36.0f;
        x = dy + (24.0f + fn_13_EB6C(camera, 0));
        y = 234.0f + dy;
        break;
    default:
        scale = 0.8f;
        dy = 36.0f * scale;
        x = dy + (16.0f + fn_13_EB6C(camera, 0));
        y = dy + fn_13_EBF8(camera, 4);
        break;
    }
    radius = 28.0f * scale;
    dots = lbl_13_data_10D0[lbl_13_bss_2C].dots;
    count = lbl_13_data_10D0[lbl_13_bss_2C].count;
    fn_13_F170(&req, scale, scale, 0.7f);
    req.flags = 0xA;
    set_ui_widescreen_scale_mtx((int)x);
    fn_13_F1D4(&req, 0xB07, x, y, 0.12f);
    req.opacity = 1.0f;
    mtxa_from_identity();
    mtxa_rotate_y(-camera->rotY);
    mtxa_translate_neg(&camera->eye);
    for (i = 0; i < count; i++)
    {
        mtxa_tf_point(&dots[i].pos, &pos);
        pos.y = 0.0f;
        if (math_sqrt(mathutil_sum_of_sq_2(pos.x, pos.z)) >= 900.0f)
        {
            vec_normalize_len(&pos);
            PSVECScale(&pos, &pos, 900.0f);
            req.rotation = (s16)math_atan2(-pos.x, -pos.z);
            req.scaleX = 1.0f;
            req.scaleY = req.scaleX;
            bmpId = 0xB08;
        }
        else
        {
            req.scaleX = dots[i].scale;
            req.scaleY = req.scaleX;
            bmpId = 0xB09;
        }
        t = (g_font_char_var_3 & 0x1F) / 31.0f;
        t = 1.0 - fabs(2.0f * (t - 0.5f));
        req.opacity = 0.5f + 0.5f * t;
        req.scaleX *= 0.8f * scale;
        req.scaleY *= 0.8f * scale;
        dx = pos.x * (radius / 900.0f);
        dy = pos.z * (radius / 900.0f);
        mtxa_push();
        fn_13_F1D4(&req, bmpId, x + dx, y + dy, 0.11f);
        mtxa_pop();
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_13_D050(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    float pivot;

    fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
    textdraw_reset();
    switch (lbl_13_bss_24)
    {
    case 4:
    default:
        x = 16.0f;
        y = 443.0f;
        pivot = x;
        break;
    case 6:
    case 7:
        x = 272.0f;
        y = 443.0f;
        pivot = 320.0f;
        break;
    case 1:
    case 2:
    case 5:
        if (lbl_13_bss_24 != 2 || modeCtrl.splitscreenMode != 3)
        {
            x = 16.0f;
            y = 228.0f;
            pivot = x;
            break;
        }
    case 3:
        x = 272.0f;
        y = 228.0f;
        pivot = x;
        break;
    }
    set_ui_widescreen_scale_mtx((int)pivot);
    fn_13_F1D4(&req, 0xB16, x, y, 0.11f);
    req.addColor = 0x5C3B1B;
    fn_13_F1D4(&req, 0xB14, 11.0 + x, 2.0 + y, 0.1f);
    textdraw_set_mul_color(0xFF000000);
    textdraw_set_add_color(req.addColor);
    textdraw_set_font(0x57);
    textdraw_set_pos(65.0 + x, 2.0 + y);
    textdraw_printf("%2d", (int)lbl_13_bss_2A);
    reset_ui_widescreen_scale_mtx();
}

void fn_13_D208(s8 *status, struct Sprite *sprite)
{
    if (lbl_13_bss_12 > 7)
        *status = 0;
}

void fn_13_D228(struct Sprite *sprite)
{
    float x;
    float y;
    float scale;

    x = sprite->x;
    y = sprite->y;
    scale = 0.85f;
    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    if (locale == 5)
    {
        textdraw_set_font(3);
        textdraw_set_scale(2.0f, 2.0f);
        textdraw_set_pos(320.0f, 220.0f);
        textdraw_set_flags(0x80000000);
        textdraw_set_font_style(0);
        textdraw_set_alignment(4);
        textdraw_set_border();
        if (lbl_13_bss_2A >= lbl_13_bss_C)
            textdraw_print("\x83\x74\x83\x40\x83\x43\x83\x69\x83\x8B\n\x83\x89\x83\x45\x83\x93\x83\x68");
        else
            textdraw_printf("\x83\x89\x83\x45\x83\x93\x83\x68 %d", (int)lbl_13_bss_2A);
    }
    else
    {
        textdraw_set_font(9);
        textdraw_set_scale(scale, scale);
        textdraw_set_depth(0.09f);
        if (lbl_13_bss_2A >= lbl_13_bss_C)
        {
            textdraw_set_pos(54.0f + x, y - 60.0f);
            textdraw_print("FINAL");
            textdraw_set_pos(35.0f + x, y);
            textdraw_print("ROUND");
        }
        else
        {
            if (lbl_13_bss_2A >= 10)
                x -= 18.0f * scale;
            textdraw_set_pos(x, y);
            textdraw_printf("ROUND %d", (int)lbl_13_bss_2A);
        }
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_13_D408(int player)
{
    float x;
    float scale;
    float y;
    struct Sprite *sprite;
    struct Pilot2Player *pl;
    struct Camera *camera;

    camera = &cameras[player];
    pl = &lbl_13_bss_70[player];
    if (pl->unk42 <= 0)
    {
        pl->flags |= 0x400;
        x = fn_13_EB6C(camera, 4);
        y = fn_13_EBF8(camera, 4);
        switch (lbl_13_bss_24)
        {
        case 0:
            scale = 0.85f;
            break;
        default:
            scale = 0.65f;
            break;
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = x;
            sprite->y = y - 25.0f * scale;
            sprite->type = 0;
            sprite->textAlign = 4;
            if (locale == 5)
            {
                sprite->fontId = 3;
                sprite->scaleX = 2.0f;
                sprite->scaleY = 2.0f;
                sprite->flags = 0x20000000;
                sprite->unk20 = 1.0f;
                sprite->unk24 = 0;
                sprintf(sprite->text, "\x82\xB5\x82\xC1\x82\xCF\x82\xA2");
            }
            else
            {
                sprite->fontId = 9;
                sprite->scaleX = scale;
                sprite->scaleY = scale;
                sprintf(sprite->text, "MISS");
            }
        }
        switch (lbl_13_bss_24)
        {
        case 0:
            y = y + 180.0f * scale;
            break;
        default:
            y = 68.4f + y;
            break;
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->type = 1;
            sprite->depth = 0.11f;
            sprite->bmpId = 0xB0A;
            sprite->x = x;
            sprite->y = y + 14.0f * scale;
            sprite->scaleX = 51.0f * scale;
            sprite->scaleY = 5.5f * scale;
            sprite->textAlign = 4;
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = x;
            sprite->y = y;
            sprite->scaleX = scale;
            sprite->scaleY = scale;
            sprite->drawFunc = fn_13_D64C;
        }
    }
}

void fn_13_D64C(struct Sprite *sprite)
{
    float x;
    float y;
    char *str;

    x = sprite->x;
    y = sprite->y;
    textdraw_reset();
    textdraw_set_scale(sprite->scaleX, sprite->scaleY);
    switch (locale)
    {
    case 5:
        str = "\x83\x7B\x81\x5B\x83\x8B\x82\xF0\x82\xC6\x82\xB6\x82\xC4\x81\x41\x82\xBF\x82\xE1\x82\xAD\x82\xBF\x82\xB5\x82\xE6\x82\xA4";
        break;
    default:
        str = "CLOSE BALL AND LAND";
        break;
    }
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    textdraw_set_flags(0x200000);
    textdraw_set_alignment(3);
    textdraw_set_drop_shadow();
    textdraw_set_pos(x, y);
    textdraw_print(str);
}

void fn_13_D718(s8 *status, struct Sprite *sprite)
{
    sprite->scaleX = 1.0 + 0.083333333333333329 * (float)sprite->userVar;
    sprite->scaleY = sprite->scaleX;
    sprite->x = 388.0 + -23.166666666666668 * (float)sprite->userVar;
    sprite->y = 12.0 + 5.666666666666667 * (float)sprite->userVar;
}

void fn_13_D7DC(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    int j;
    int i;
    struct Camera *camera;
    int step;
    struct Pilot2Player *pl;
    struct Ball *ball;
    struct GMAModel *model;
    float x;
    float y;
    float scale;
    float modelScale;
    float slotX;
    float t;

    fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        pl = &lbl_13_bss_70[ball->playerId];
        if (ball->status == 2)
        {
            if (pl->unk42 > 0)
                break;
            camera = &cameras[i];
            x = fn_13_EB6C(camera, 1) - 136.0f;
            y = 12.0f + fn_13_EBF8(camera, 2);
            if (modeCtrl.unk30 == 1)
            {
                scale = 1.0f;
                y += 30.0f;
            }
            else
            {
                scale = 0.8f;
                x += 40.0f;
                y += 15.0f;
            }
            step = 40.0 * scale;
            for (j = 0; j < 4; j++)
            {
                slotX = x + (float)(step * j);
                if (j != 3 && (&pl->unk48)[j] != 0)
                {
                    t = (g_font_char_var_3 & 0x1F) / 31.0f;
                    t = 1.0 - fabs(2.0f * (t - 0.5f));
                    t = 130.0f + 125.0f * t;
                    req.mulColor = (u8)t | (((u8)t << 16) | ((u8)t << 8));
                    req.opacity = 1.0f;
                    if (j == 0)
                    {
                        textdraw_reset();
                        textdraw_set_scale(0.75 * scale, 0.75 * scale);
                        textdraw_set_font(0x8F);
                        g_some_printf_function_9(slotX, y, 0xFFFFFF, 0, "%d", pl->unk48 + 1);
                    }
                    switch (j)
                    {
                    case 0:
                        model = init_common_gma->modelEntries[0x25].model;
                        modelScale = 0.08f;
                        break;
                    case 1:
                        model = minigameGma[0]->modelEntries[0xA5].model;
                        modelScale = 1.0f;
                        break;
                    case 2:
                        model = minigameGma[0]->modelEntries[0xA6].model;
                        modelScale = 1.0f;
                        break;
                    default:
                        model = NULL;
                        break;
                    }
                    if (model != NULL)
                        fn_13_DB54(model, slotX, y, modelScale * scale);
                }
            }
        }
    }
}

void fn_13_DB54(struct GMAModel *model, float x, float y, float scale)
{
    Vec saved;
    Vec dir;

    mtxa_from_mtxb();
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    mtxa_from_identity();
    mtxa_translate_xyz(x, y, -30.0f);
    mtxa_rotate_y(g_font_char_var_3 << 7);
    mtxa_rotate_z(-0x8000);
    mtxa_scale_s(18.0f * scale);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    dir.x = -1.0f;
    dir.y = -3.5f;
    dir.z = -40.0f;
    saved = lbl_1_bss_D4244;
    fn_1_10A3A0(&dir);
    avdisp_draw_model_unculled_sort_never(model);
    fn_1_10A3A0(&saved);
    g_some_sprite_reset_gx_state_func();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_13_DCA0(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_13_DCD4;
}

void fn_13_DCD4(struct Sprite *sprite)
{
    s16 colorBmps[4] = {0xB12, 0xB10, 0xB13, 0xB11};
    struct SpriteDrawRequest req;
    float x;
    float y;
    float t;
    int i;

    fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
    req.scaleX = 12.307693f;
    fn_13_F1D4(&req, 0xB0D, 0.0f, 24.0f, 0.13f);
    textdraw_reset();
    textdraw_set_font(0x8F);
    textdraw_set_font_style(2);
    textdraw_set_spacing(1.0f, 0.0f);
    textdraw_set_pos(36.0f, 37.0f);
    g_some_printf_function_8("MONKEY TARGET 2");
    textdraw_reset();
    textdraw_set_font(0x8F);
    textdraw_set_scale(0.5f, 0.6f);
    textdraw_set_spacing(1.0f, 0.0f);
    textdraw_set_pos(430.0f, 45.0f);
    g_some_printf_function_8("- %d ROUND FLIGHT -", (int)lbl_13_bss_C);
    set_ui_widescreen_scale_mtx(0x140);
    if (modeCtrl.playerCount <= 1)
        x = 200.0f;
    else
        x = 80.0f;
    if (modeCtrl.playerCount <= 2)
        y = 190.0f;
    else
        y = 160.0f;
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 1:
        case 3:
            x += 240.0f;
            break;
        case 2:
            x -= 240.0f;
            y += 104.0f;
            break;
        }
        if (g_poolInfo.playerPool.statusList[i] != 0)
        {
            fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
            fn_13_F1D4(&req, colorBmps[i], x, y, 0.11f);
            fn_13_F1D4(&req, 0xB0F, 104.0f + x, y, 0.11f);
            req.scaleX = 0.5f;
            req.scaleY = 0.325f;
            req.rotation = 2048.0f * mathutil_cos(g_font_char_var_3 * 400);
            req.flags = 10;
            fn_13_F1D4(&req, lbl_13_rodata_3D0[active_monkey_id[i]][0], 58.0f + x, 42.0f + y, 0.1f);
            t = (2.0f * (float)(g_font_char_var_3 % (int)lbl_80198B34)) / lbl_80198B34;
            if (t > 1.0f)
                t = 2.0f - t;
            req.opacity = t;
            fn_13_F1D4(&req, lbl_13_rodata_3D0[active_monkey_id[i]][4], 58.0f + x, 42.0f + y, 0.09f);
            textdraw_reset();
            textdraw_set_depth(0.1f);
            fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
            req.u1 = 0.25f * (float)i;
            req.u2 = 0.25f + req.u1;
            req.v1 = 0.25f * (float)fn_13_F238(balls[i].colorId);
            req.v2 = 0.25f + req.v1;
            req.scaleX = 0.25f;
            req.scaleY = 0.25f;
            fn_13_F1D4(&req, 0xB04, 44.0f + x, 66.0f + y, 0.1f);
            fn_13_F170(&req, 1.0f, 1.0f, 1.0f);
            req.mulColor = 0;
            req.addColor = 0x5C3B1B;
            fn_13_F1D4(&req, 0xB0C, 103.0f + x, 25.0f + y, 0.1f);
            textdraw_set_font(0x53);
            textdraw_set_mul_color(0xFF000000);
            textdraw_set_add_color(req.addColor);
            textdraw_set_pos(114.0f + x, 47.0f + y);
            textdraw_printf("%6d", lbl_13_bss_244[i]);
        }
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_13_E1B4(int player)
{
    struct ScreenPos pos;
    struct Sprite *sprite;
    int duration;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        duration = 180;
        pos = fn_13_EC84(player, 6);
        sprite->x = pos.x;
        sprite->y = pos.y;
        sprite->unk58 = pos.y;
        switch (lbl_13_bss_24)
        {
        case 0:
            sprite->scaleX = 0.85f;
            break;
        case 4:
            if (player == 0)
                sprite->scaleX = 0.8f;
            else
                sprite->scaleX = 0.3f;
            break;
        default:
            sprite->scaleX = 0.5f;
            break;
        }
        sprite->scaleY = sprite->scaleX;
        sprite->depth = 0.05f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0x8C;
        sprite->mulB = 0;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = duration;
        sprite->userVar = duration;
        sprite->mainFunc = fn_13_E344;
        strcpy(sprite->text, "FALL OUT");
    }
}

void fn_13_E344(s8 *status, struct Sprite *sprite)
{
    int t;

    t = sprite->userVar - sprite->counter;
    if (t < 30)
    {
        sprite->opacity = 0.03333f * t;
        sprite->y = sprite->unk58 - (30 - t) * 6;
    }
    else if (t < 60)
    {
        sprite->opacity = 1.0f;
        sprite->y = sprite->unk58 - 32.0f * math_sin((t - 30) * 0x444);
    }
    else if (t < 90)
    {
        sprite->opacity = 1.0f;
        sprite->y = sprite->unk58 - 8.0f * math_sin((t - 60) * 0x444);
    }
    else
        sprite->opacity = 1.0f;
    if (sprite->counter > 0)
        sprite->counter--;
}

void fn_13_E47C(void)
{
    switch (lbl_13_bss_24)
    {
    case 1:
    case 6:
        if (modeCtrl.wideMode != 0)
        {
            fn_13_E588(0.0f, 1.0f);
            return;
        }
        fn_13_E670(0.0f, 1.0f);
        return;
    case 2:
        fn_13_E670(0.0f, 1.0f);
        switch (modeCtrl.splitscreenMode)
        {
        default:
            fn_13_E588(0.5f, 1.0f);
            return;
        case 2:
            fn_13_E588(0.0f, 0.5f);
            return;
        case 3:
            fn_13_E588(0.0f, 1.0f);
            return;
        }
    case 3:
        fn_13_E588(0.0f, 1.0f);
        fn_13_E670(0.0f, 1.0f);
        break;
    }
}

void fn_13_E588(float top, float bottom)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->bmpId = 0x4C;
        sprite->depth = 0.5f;
        sprite->x = 320.0f;
        sprite->y = 240.0 * (top + bottom);
        sprite->scaleX = 1.0f;
        sprite->scaleY = 60.0 * (bottom - top);
        sprite->textAlign = 4;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = sprite->x;
        sprintf(sprite->text, "SEP TATE");
    }
}

void fn_13_E670(float left, float right)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->bmpId = 0x4C;
        sprite->depth = 0.4f;
        sprite->x = 320.0 * (left + right);
        sprite->y = 240.0f;
        sprite->rotation = 0x4000;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 80.0 * (right - left);
        sprite->textAlign = 4;
        sprite->flags = 0x200000;
        sprintf(sprite->text, "SEP YOKO");
    }
}

void fn_13_E74C(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->bmpId = 0xB2D;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->x = 336.0 + 32.0 * sprite->scaleX;
        sprite->y = 256.0 + 32.0 * sprite->scaleY;
        sprite->depth = 0.2f;
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = 0x150;
        sprintf(sprite->text, "LIVE BASE");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->bmpId = 0xB2E;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->x = 336.0 + 32.0 * sprite->scaleX;
        sprite->y = 256.0 + 32.0 * sprite->scaleY;
        sprite->depth = 0.1f;
        sprite->mulR = 0xB0;
        sprite->mulG = 0xFF;
        sprite->mulB = 0xC0;
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = 0x150;
        sprite->mainFunc = fn_13_E8D8;
        sprite->drawFunc = fn_13_E8E8;
        sprintf(sprite->text, "LIVE STAR");
    }
}

void fn_13_E8D8(s8 *status, struct Sprite *sprite)
{
    sprite->rotation += 0x300;
}

void fn_13_E8E8(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float fade;
    float py;
    float rx;
    float t;
    float size;
    float ry;
    s16 i;
    s16 angle;

    angle = g_font_char_var_3 << 9;
    for (i = 0; i < 13; i++)
    {
        angle += 0x400;
        if (angle < 0x1800 || angle > 0x5800)
        {
            if (angle > 0 && angle < 0x1800)
                fade = (0x1800 - angle) / 6144.0f;
            else if (angle > 0x5800 && angle < 0x7000)
                fade = (angle - 0x5800) / 6144.0f;
            else
                fade = 1.0f;
            size = (i + 1) / 13.0f;
            fade = fade * size;
            rx = sprite->scaleX * (32.0 * math_sin(angle + 0x4000));
            ry = sprite->scaleY * (6.0 * math_sin(angle));
            fn_13_F170(&req, sprite->scaleX * size, sprite->scaleY * size, sprite->opacity * fade);
            req.flags = 0x20000A;
            req.rotation = sprite->rotation;
            req.mulColor = 0xFF000000 | (sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB;
            t = ry * math_sin(-0x5800);
            py = (sprite->y + rx * math_sin(0x6800)) - -t;
            ry = ry * math_sin(0x6800);
            fn_13_F1D4(&req, sprite->bmpId, (sprite->x + rx * math_sin(-0x5800)) - ry, py, sprite->depth);
        }
    }
}

float fn_13_EB6C(struct Camera *camera, int anchor)
{
    float pos;

    switch (anchor)
    {
    case 0:
        pos = 640.0f * camera->sub28.viewportX;
        break;
    case 1:
        pos = 640.0f * (camera->sub28.viewportX + camera->sub28.viewportW);
        break;
    case 4:
        pos = 640.0f * camera->sub28.viewportX + 320.0f * camera->sub28.viewportW;
        break;
    }
    return pos;
}

float fn_13_EBF8(struct Camera *camera, int anchor)
{
    float pos;

    switch (anchor)
    {
    case 2:
        pos = 480.0f * camera->sub28.viewportY;
        break;
    case 3:
        pos = 480.0f * (camera->sub28.viewportY + camera->sub28.viewportH);
        break;
    case 4:
        pos = 480.0f * camera->sub28.viewportY + 240.0f * camera->sub28.viewportH;
        break;
    }
    return pos;
}

struct ScreenPos fn_13_EC84(int player, u32 anchor)
{
    struct ScreenPos pos;
    struct Camera *camera;

    camera = &cameras[player];
    switch (anchor)
    {
    case 0:
        pos.x = 640.0f * camera->sub28.viewportX;
        pos.y = 480.0f * camera->sub28.viewportY;
        break;
    case 1:
        pos.x = 640.0f * (camera->sub28.viewportX + camera->sub28.viewportW);
        pos.y = 480.0f * camera->sub28.viewportY;
        break;
    case 2:
        pos.x = 640.0f * camera->sub28.viewportX;
        pos.y = 480.0f * camera->sub28.viewportY + 240.0f * camera->sub28.viewportH;
        break;
    case 3:
        pos.x = 640.0f * (camera->sub28.viewportX + camera->sub28.viewportW);
        pos.y = 480.0f * camera->sub28.viewportY + 240.0f * camera->sub28.viewportH;
        break;
    case 4:
        pos.x = 640.0f * camera->sub28.viewportX;
        pos.y = 480.0f * (camera->sub28.viewportY + camera->sub28.viewportH);
        break;
    case 5:
        pos.x = 640.0f * (camera->sub28.viewportX + camera->sub28.viewportW);
        pos.y = 480.0f * (camera->sub28.viewportY + camera->sub28.viewportH);
        break;
    case 6:
        pos.x = 640.0f * camera->sub28.viewportX + 320.0f * camera->sub28.viewportW;
        pos.y = 480.0f * camera->sub28.viewportY + 240.0f * camera->sub28.viewportH;
        break;
    }
    return pos;
}

void g_some_printf_function_8(char *fmt, ...)
{
    va_list args;
    char buf[32];
    float x;
    float y;
    u32 mulColor;
    u32 addColor;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    x = textdraw_pos_x.x;
    y = textdraw_pos_x.y;
    mulColor = textdraw_pos_x.mulColor;
    addColor = textdraw_pos_x.addColor;
    textdraw_set_mul_color(0xFF404040);
    textdraw_set_add_color(0);
    textdraw_set_pos(2.0f + x, 2.0f + y);
    textdraw_set_depth(0.11f);
    textdraw_print(buf);
    textdraw_set_mul_color(mulColor);
    textdraw_set_add_color(addColor);
    textdraw_set_pos(x, y);
    textdraw_set_depth(0.1f);
    textdraw_print(buf);
}

void g_some_printf_function_9(float x, float y, u32 mulColor, u32 addColor, char *fmt, ...)
{
    va_list args;
    char buf[32];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    textdraw_set_mul_color(0xFF404040);
    textdraw_set_pos(2.0f + x, 2.0f + y);
    textdraw_set_depth(0.11f);
    textdraw_print(buf);
    textdraw_set_mul_color(mulColor);
    textdraw_set_add_color(addColor);
    textdraw_set_pos(x, y);
    textdraw_set_depth(0.1f);
    textdraw_print(buf);
}

void fn_13_F170(struct SpriteDrawRequest *req, float scaleX, float scaleY, float opacity)
{
    req->scaleX = scaleX;
    req->scaleY = scaleY;
    req->u1 = req->v1 = 0.0f;
    req->u2 = req->v2 = 1.0f;
    req->rotation = 0;
    req->opacity = opacity;
    req->unk30 = -1;
    req->flags = 5;
    req->mulColor = 0xFFFFFF;
    req->addColor = 0;
}

void fn_13_F1D4(struct SpriteDrawRequest *req, int bmpId, float x, float y, float z)
{
    req->bmpId = bmpId;
    req->x = x;
    req->y = y;
    req->z = z;
    draw_sprite_draw_request(req);
}

int fn_13_F204(int value)
{
    int digits;

    for (digits = 1; value >= 10; digits++)
        value /= 10;
    return digits;
}

int fn_13_F238(int colorId)
{
    switch (colorId)
    {
    case 3:
        return 0;
    case 1:
        return 1;
    case 0:
        return 2;
    case 2:
        return 3;
    default:
        return 0;
    }
}
