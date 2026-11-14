/* Dogfight HUD: sprites for the scores, gauges, radar and results, and the screen dividers. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "item.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "sprite.h"
#include "stage.h"
#include "textdraw.h"
#include "va.h"

struct DogfightPlayer
{
    u32 flags;
    u8 filler4[0x1C - 0x4];
    s16 unk1C;
    u8 filler1E[0x34 - 0x1E];
    s16 unk34;
    u8 filler36[2];
    s16 unk38;
    s8 unk3A;
    u8 filler3B[0x44 - 0x3B];
    s32 unk44;
    u8 filler48[0x5C - 0x48];
    s16 unk5C;
    u8 filler5E;
    s8 unk5F;
    s8 unk60;
    u8 filler61[0x65 - 0x61];
    u8 unk65;
    u8 unk66;
    u8 filler67[0xB0 - 0x67];
};

struct DogfightShot
{
    s8 status;
    u8 filler1[7];
    s32 unk8;
    s32 flags;
    u8 filler10[4];
    Vec pos;
    u8 filler20[0x74 - 0x20];
    s16 unk74;
    u8 filler76[0x84 - 0x76];
    s32 unk84;
    u8 filler88[0x4EC - 0x88];
};

struct ResultBitmaps
{
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
    u16 unk8;
    u16 unkA;
};

extern struct GMA *minigameGma[];
extern s32 locale;
extern float lbl_80198B34;
extern s8 lbl_1_bss_D4D2D;
extern u32 lbl_1_bss_D4D30;
extern s16 lbl_17_bss_14;
extern s32 lbl_17_bss_24;
extern s8 lbl_17_bss_28;
extern s32 lbl_17_bss_2C[];
extern s8 lbl_17_bss_40[];
extern struct DogfightPlayer lbl_17_bss_48[];
extern s16 lbl_17_bss_308;
extern s32 lbl_17_bss_314[];
extern s32 lbl_17_bss_348;
extern s32 lbl_17_bss_350;
extern s32 lbl_17_bss_354[];
extern s16 lbl_17_bss_364[];
extern struct DogfightShot lbl_17_bss_3DC[];
extern s32 lbl_1_rodata_A838[];
void set_ui_widescreen_scale_mtx(int width);
void reset_ui_widescreen_scale_mtx(void);
void init_ui_element_sprite_with_defaults(void);
void set_ui_element_sprite_pos(float x, float y);
void set_ui_element_sprite_depth(float z);
void g_set_ui_element_sprite_flags(u32 flags);
void set_ui_element_sprite_scale(float x, float y);
void draw_ui_box(s32 bmpId);
void textdraw_print(char *text);
void textdraw_printf(char *fmt, ...);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_scale(float x, float y);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_pos(float x, float y);
void textdraw_set_mul_color(u32 color);
void textdraw_set_add_color(u32 color);
void textdraw_set_depth(float depth);
void textdraw_set_alpha(float alpha);
void textdraw_set_drop_shadow(void);

void fn_17_ABC4(struct Sprite *sprite);
void fn_17_ADB4(struct Ball *ball);
void fn_17_B13C(struct Sprite *sprite);
void fn_17_B880(void);
void fn_17_BCC4(struct Ball *ball);
void fn_17_BE08(struct Sprite *sprite);
void fn_17_C138(struct Sprite *sprite);
void fn_17_C304(struct Sprite *sprite);
void fn_17_C390(struct Ball *ball);
void fn_17_CA20(struct Sprite *sprite);
void fn_17_CCD8(s8 *status, struct Sprite *sprite);
void fn_17_CD40(struct Sprite *sprite);
void fn_17_CE60(int id, float x, float y, float radius);
void fn_17_D3F8(float *px, float *py, float x, float y, float radius);
void fn_17_D484(struct Sprite *sprite);
void fn_17_DA50(s8 *status, struct Sprite *sprite);
void fn_17_DD30(void);
void fn_17_DD80(s8 *status, struct Sprite *sprite);
void fn_17_DED4(struct Sprite *sprite);
void fn_17_E30C(void);
void fn_17_E418(float a, float b);
void fn_17_E500(float a, float b);
void fn_17_E5DC(void);
void fn_17_E768(s8 *status, struct Sprite *sprite);
void fn_17_E778(struct Sprite *sprite);
void fn_17_EAD0(s8 *status, struct Sprite *sprite);
void fn_17_EE04(s8 *status, struct Sprite *sprite);
float fn_17_EE54(struct Camera *camera, int mode);
float fn_17_EEE0(struct Camera *camera, int mode);
void g_some_printf_function_10(char *fmt, ...);
void fn_17_F320(struct SpriteDrawRequest *req, float scaleX, float scaleY, float opacity);
void fn_17_F384(struct SpriteDrawRequest *req, int bmpId, float x, float y, float z);
int fn_17_F3B4(int colorId);

const struct ResultBitmaps lbl_17_rodata_4D0[] = {
    { 0xF, 0xF, 0x46, 0x48, 0x48, 0x48 },
    { 0x20, 0x20, 0x2F, 0x31, 0x31, 0x31 },
    { 0x33, 0x33, 0x41, 0x41, 0x41, 0x41 },
    { 0x3, 0x5B, 0x5C, 0x5D, 0x5D, 0x5C },
};

u32 lbl_17_data_F68[] = {
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
    0x18000000, 0x000F0000,
};

void fn_17_A8AC(void)
{
    struct Sprite *sprite;
    struct Camera *camera;
    float scale;
    float x;
    float y;
    float screenX;
    int i;

    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_17_ABC4;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_17_B13C;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_17_BE08;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_17_C138;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_17_C304;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_17_CA20;
    switch (lbl_17_bss_24)
    {
    case 0:
        scale = 1.0f;
        x = 24.0f;
        y = 168.0f;
        break;
    case 1:
        scale = 0.6111111f;
        x = 24.0f;
        y = 96.0f;
        break;
    case 4:
        scale = 0.6111111f;
        x = 12.0f;
        y = 168.0f;
        break;
    default:
        scale = 0.55555558f;
        x = 12.0f;
        y = 96.0f;
        break;
    }
    for (i = 0; i < lbl_17_bss_28; i++)
    {
        camera = &cameras[i];
        if (lbl_17_bss_2C[i] != 0)
        {
            sprite = create_sprite();
            if (sprite != NULL)
            {
                sprite->type = 1;
                sprite->scaleX = scale;
                sprite->scaleY = scale;
                screenX = x + fn_17_EE54(camera, 0);
                sprite->x = 72.0f * sprite->scaleX + screenX;
                sprite->y = 72.0f * sprite->scaleY + (y + fn_17_EEE0(camera, 2));
                sprite->depth = 0.25f;
                sprite->bmpId = 0x1221;
                sprite->textAlign = 4;
                sprite->flags = 0x40000;
                sprite->mainFunc = fn_17_CCD8;
                if (modeCtrl.wideMode != 0)
                    sprite->scaleX *= 0.75;
            }
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->scaleX = scale;
        sprite->scaleY = scale;
        sprite->x = x;
        sprite->y = y;
        sprite->mainFunc = fn_17_CCD8;
        sprite->drawFunc = fn_17_CD40;
    }
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_17_D484;
    if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == 3)
        fn_17_E5DC();
    fn_17_E30C();
}

void fn_17_ABC4(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    int i;
    struct Ball *ball;
    struct Camera *camera;
    struct DogfightPlayer *player;

    if (lbl_17_bss_14 == 0xD)
    {
        for (i = 0; i < 4; i++)
        {
            ball = &balls[i];
            if (ball->status == 2 && lbl_17_bss_2C[i] != 0)
            {
                player = &lbl_17_bss_48[ball->playerId];
                camera = &cameras[i];
                if (!(player->flags & 8))
                {
                    fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
                    req.flags = 0xA;
                    switch (lbl_17_bss_24)
                    {
                    default:
                        req.scaleX = 0.8f;
                        req.scaleY = 0.8f;
                        break;
                    case 0:
                        break;
                    }
                    if (player->unk66 != 0)
                    {
                        if (g_font_char_var_3 & 4)
                        {
                            x = fn_17_EE54(camera, 4);
                            y = 30.0f + fn_17_EEE0(camera, 4);
                            req.opacity = 0.8f;
                            fn_17_F384(&req, 0x123A, x, y, 0.11f);
                        }
                    }
                    else if (player->unk65 != 0 && (g_font_char_var_3 & 8))
                    {
                        x = fn_17_EE54(camera, 4);
                        fn_17_F384(&req, 0x1223, x, 30.0f + fn_17_EEE0(camera, 4), 0.11f);
                    }
                }
                fn_17_ADB4(ball);
            }
        }
    }
}

void fn_17_ADB4(struct Ball *ball)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    struct DogfightPlayer *player;
    struct Camera *camera;
    int attacker;
    int color;

    player = &lbl_17_bss_48[ball->playerId];
    camera = &cameras[ball->playerId];
    if (lbl_17_bss_2C[ball->playerId] == 0)
        return;
    if (player->unk38 > 0)
        attacker = 1;
    else if (player->unk34 > 0)
        attacker = 0;
    else
        return;
    switch (attacker)
    {
    case 0:
        if ((float)player->unk34 > 1.5f * lbl_80198B34)
            return;
        if (player->unk34 % 10 < 3)
            return;
        break;
    case 1:
        if (player->unk38 > lbl_80198B38 * 2)
            return;
        if (player->unk38 % 10 < 3)
            return;
        break;
    }
    x = 30.0f + fn_17_EE54(camera, 4);
    if (1.0f == camera->sub28.viewportH)
        y = fn_17_EEE0(camera, 3) - 100.0f;
    else
        y = fn_17_EEE0(camera, 3) - 50.0f;
    textdraw_reset();
    textdraw_set_font(4);
    textdraw_set_scale(0.625f, 0.75f);
    textdraw_set_alignment(6);
    switch (attacker)
    {
    case 0:
        textdraw_set_pos(x, y);
        textdraw_printf("MISSILE\"HIT");
        break;
    case 1:
        x -= 10.0f;
        textdraw_set_mul_color(0xFFFF00);
        textdraw_set_pos(x, y);
        textdraw_printf("SHOT\"DOWN");
        break;
    }
    attacker = player->unk3A;
    color = fn_17_F3B4(balls[attacker].colorId);
    if (lbl_17_bss_48[attacker].flags & 0x800)
        attacker = 4;
    fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
    req.u1 = 0.2f * (float)attacker;
    req.u2 = 0.2f + req.u1;
    req.v1 = 0.25f * (float)color;
    req.v2 = 0.25f + req.v1;
    req.scaleX = 0.17142858f;
    req.scaleY = 0.234375f;
    fn_17_F384(&req, 0x121E, 7.0f + x, y - 2.0f, 0.1f);
}

void fn_17_B13C(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float scale;
    float x;
    float y;
    float px;
    float py;
    int i;
    struct Ball *ball;
    struct DogfightPlayer *player;
    struct Camera *camera;
    int j;

    if (lbl_17_bss_14 >= 9)
    {
        fn_17_B880();
        for (i = 0; i < 4; i++)
        {
            ball = &balls[i];
            player = &lbl_17_bss_48[i];
            if (ball->status == 2 && lbl_17_bss_2C[i] != 0)
            {
                camera = &cameras[i];
                fn_17_BCC4(ball);
                switch (lbl_17_bss_24)
                {
                case 0:
                    x = 24.0f + fn_17_EE54(camera, 0);
                    y = 24.0f + fn_17_EEE0(camera, 2);
                    break;
                case 4:
                    x = 24.0f + fn_17_EE54(camera, 0);
                    y = 24.0f + fn_17_EEE0(camera, 2);
                    if (640.0f == fn_17_EE54(camera, 1))
                        x += 16.0;
                    break;
                default:
                    x = 16.0f + fn_17_EE54(camera, 0);
                    y = 12.0f + fn_17_EEE0(camera, 2);
                    break;
                }
                set_ui_widescreen_scale_mtx(x);
                switch (lbl_17_bss_24)
                {
                case 0:
                    fn_17_F320(&req, 0.4375f, 0.296875f, 1.0f);
                    fn_17_F384(&req, lbl_1_rodata_A838[ball->ape->charaId], x, y, 0.1f);
                    x += 60.0f;
                    break;
                }
                fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
                req.u1 = 0.2f * (float)i;
                req.u2 = 0.2f + req.u1;
                req.v1 = 0.25f * (float)fn_17_F3B4(ball->colorId);
                req.v2 = 0.25f + req.v1;
                req.scaleX = 0.2f;
                req.scaleY = 0.25f;
                fn_17_F384(&req, 0x121E, x, y, 0.1f);
                textdraw_reset();
                switch (lbl_17_bss_24)
                {
                case 0:
                    textdraw_set_font(0x81);
                    textdraw_set_pos(144.0f + x, y);
                    break;
                case 1:
                    textdraw_set_font(0x82);
                    textdraw_set_pos(144.0f + x, y);
                    break;
                default:
                    textdraw_set_font(0x82);
                    textdraw_set_pos(104.0f + x, y);
                    break;
                }
                if (lbl_1_bss_D4D2D != 1)
                {
                    textdraw_set_drop_shadow();
                    textdraw_set_alignment(6);
                    textdraw_printf("%d", lbl_17_bss_314[i]);
                }
                fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
                switch (lbl_17_bss_24)
                {
                case 0:
                    y += 24.0f;
                    fn_17_F384(&req, 0x1218, x, y, 0.11f);
                    req.u2 = (float)player->unk5C / (float)lbl_17_bss_308;
                    req.scaleX = req.u2;
                    fn_17_F384(&req, 0x1216, 34.0f + x, 2.0f + y, 0.1f);
                    break;
                case 1:
                    y += 20.0f;
                    fn_17_F384(&req, 0x1218, x, y, 0.11f);
                    req.u2 = (float)player->unk5C / (float)lbl_17_bss_308;
                    req.scaleX = req.u2;
                    fn_17_F384(&req, 0x1216, 34.0f + x, 2.0f + y, 0.1f);
                    break;
                default:
                    y += 20.0f;
                    fn_17_F384(&req, 0x123C, x, y, 0.11f);
                    req.u2 = (float)player->unk5C / (float)lbl_17_bss_308;
                    req.scaleX = req.u2;
                    fn_17_F384(&req, 0x123B, 2.0f + x, 2.0f + y, 0.1f);
                    break;
                }
                switch (lbl_17_bss_24)
                {
                case 0:
                    scale = 1.0f;
                    x -= 60.0f;
                    y += 20.0f;
                    break;
                case 1:
                case 4:
                    scale = 1.0f;
                    y += 16.0f;
                    break;
                default:
                    scale = 0.75f;
                    y += 12.0f;
                    break;
                }
                fn_17_F320(&req, scale, scale, 1.0f);
                for (j = 0; j < player->unk5F; j++)
                {
                    px = x + 20.0f * (float)j * scale;
                    py = y;
                    req.opacity = 1.0f;
                    if (j <= player->unk5F - 1)
                    {
                        if (j == player->unk5F - 1 && (player->flags & 0x10000) && (g_font_char_var_3 & 8))
                        {
                            float opacity[8] = { 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f };

                            req.opacity = opacity[g_font_char_var_3 & 7];
                        }
                        req.flags = 5;
                        fn_17_F384(&req, 0x121C, px, py, 0.1f);
                    }
                }
            }
        }
        reset_ui_widescreen_scale_mtx();
    }
}

void fn_17_B880(void)
{
    struct SpriteDrawRequest req;
    float boxX;
    float boxY;
    float px;
    float py;
    float x;
    float y;
    float width;
    int i;
    struct Ball *ball;
    struct DogfightPlayer *player;

    switch (lbl_17_bss_24)
    {
    case 0:
        x = 536.0f;
        y = 58.0f;
        width = 602.0f;
        break;
    case 4:
        x = 280.0f;
        y = 424.0f;
        width = 320.0f;
        break;
    default:
        x = 536.0f;
        y = 244.0f;
        width = 602.0f;
        break;
    }
    set_ui_widescreen_scale_mtx(width);
    if (lbl_17_bss_24 == 4 && (lbl_1_bss_D4D30 & 1))
    {
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_pos(320.0f, 417.0f);
        set_ui_element_sprite_depth(0.3f);
        g_set_ui_element_sprite_flags(4);
        set_ui_element_sprite_scale(0.24f, 1.0f);
        draw_ui_box(5);
    }
    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        player = &lbl_17_bss_48[i];
        if (ball->status == 2 && (player->flags & 0x800))
        {
            fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
            req.u1 = 0.8f;
            req.u2 = 0.2f + req.u1;
            req.v1 = 0.25f * (float)fn_17_F3B4(ball->colorId);
            req.v2 = 0.25f + req.v1;
            req.scaleX = 0.15714286f;
            req.scaleY = 0.1875f;
            fn_17_F384(&req, 0x121E, x, y, 0.1f);
            if (lbl_1_bss_D4D2D != 1)
            {
                textdraw_reset();
                textdraw_set_font(0x83);
                textdraw_set_drop_shadow();
                textdraw_set_alignment(6);
                textdraw_set_pos(80.0f + x, y);
                textdraw_printf("%d", lbl_17_bss_314[i]);
            }
            if (!(player->flags & 8) || !(g_font_char_var_3 & 8))
            {
                boxX = 1.0f + x;
                boxY = 13.0f + y;
                fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
                fn_17_F384(&req, 0x1219, boxX, boxY, 0.11f);
                req.u2 = (float)player->unk5C / (float)lbl_17_bss_308;
                req.scaleX = (78.0f * req.u2) / 80.0f;
                fn_17_F384(&req, 0x1217, boxX, 2.0f + boxY, 0.1f);
            }
            if (player->flags & 0x10000)
            {
                fn_17_F320(&req, 0.875f, 0.8333333f, 1.0f);
                if (g_font_char_var_3 & 8)
                {
                    float opacity[8] = { 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f };

                    req.opacity = opacity[g_font_char_var_3 & 7];
                }
                px = x - 3.0f;
                py = y;
                req.flags = 7;
                fn_17_F384(&req, 0x121C, px, py, 0.11f);
            }
            y += 21.0f;
        }
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_17_BCC4(struct Ball *ball)
{
    float x;
    float y;
    struct Camera *camera;
    struct DogfightPlayer *player;

    player = &lbl_17_bss_48[ball->playerId];
    camera = &cameras[ball->playerId];
    if (player->unk1C > 0)
    {
        if (!(g_some_other_flags & 0xA))
            player->unk1C--;
        if ((player->unk1C >> 2) & 1)
        {
            x = fn_17_EE54(camera, 4);
            y = 20.0f + fn_17_EEE0(camera, 2);
            textdraw_reset();
            textdraw_set_scale(0.6f, 0.6f);
            textdraw_set_font(0x80);
            textdraw_set_alignment(3);
            textdraw_set_mul_color(0xFFFF50);
            textdraw_set_pos(x, y);
            g_some_printf_function_10("%d", player->unk44);
        }
    }
}

void fn_17_BE08(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    float alpha;
    float width;

    switch (lbl_17_bss_24)
    {
    case 0:
        x = 532.0f;
        y = 22.0f;
        width = 602.0f;
        break;
    case 1:
    case 2:
        x = 532.0f;
        y = 204.0f;
        width = 602.0f;
        break;
    case 4:
        x = 278.0f;
        y = 16.0f;
        width = 320.0f;
        break;
    default:
        x = 278.0f;
        y = 224.0f;
        width = 320.0f;
        break;
    }
    set_ui_widescreen_scale_mtx(width);
    fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
    fn_17_F384(&req, 0x1233, x, y, 0.11f);
    if (lbl_1_bss_D4D2D == 0)
    {
        alpha = 1.0f;
        if ((lbl_1_bss_D4D30 & 0x20) && lbl_17_bss_348 <= lbl_80198B38 * 10 && (lbl_17_bss_348 & 8))
        {
            textdraw_reset();
            textdraw_set_font(4);
            textdraw_set_scale(0.625f, 0.75f);
            textdraw_set_alignment(1);
            textdraw_set_pos(x - 100.0f, 16.0f + y);
            textdraw_set_mul_color(0xFFFF00FF);
            textdraw_print("CHANCE\"TIME");
        }
        if (lbl_17_bss_348 < lbl_80198B38 * 6 && lbl_17_bss_348 > 0 && (g_font_char_var_3 & 8))
        {
            float opacity[8] = { 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f };

            alpha = opacity[g_font_char_var_3 & 7];
        }
        textdraw_reset();
        textdraw_set_font(0x87);
        textdraw_set_pos(32.0f + x, 6.0f + y);
        textdraw_set_mul_color(0x5C3B1B);
        textdraw_set_alpha(alpha);
        textdraw_printf("%3d", lbl_17_bss_348 / lbl_80198B38);
    }
    else
    {
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_alignment(4);
        textdraw_set_pos(23.0f + (32.0f + x), 16.0f + y);
        textdraw_set_scale(1.5f, 1.2f);
        textdraw_set_mul_color(0x5C3B1B);
        textdraw_print("/cd8187/");
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_17_C138(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    int i;
    struct Camera *camera;
    struct DogfightPlayer *player;

    if (lbl_17_bss_14 >= 0xB)
    {
        fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
        textdraw_reset();
        textdraw_set_font(0x82);
        for (i = 0; i < 4; i++)
        {
            player = &lbl_17_bss_48[i];
            camera = &cameras[i];
            if (lbl_17_bss_2C[i] != 0)
            {
                x = 25.0f + fn_17_EE54(camera, 0);
                y = fn_17_EEE0(camera, 3) - 34.0f;
                if (lbl_17_bss_24 == 4 && (lbl_1_bss_D4D30 & 1) && 320.0f == fn_17_EE54(camera, 0))
                    x += 24.0;
                fn_17_F384(&req, 0x1208, x, y, 0.1f);
                if (player->unk60 != 0 || !(g_font_char_var_3 & 8))
                {
                    textdraw_set_pos(17.0f + x, 2.0f + y);
                    textdraw_printf("%d", player->unk60);
                }
            }
        }
    }
}

void fn_17_C304(struct Sprite *sprite)
{
    int i;

    if (lbl_17_bss_14 >= 9)
    {
        for (i = 0; i < 4; i++)
        {
            struct Ball *ball = &balls[i];

            if (ball->status == 2 && lbl_17_bss_2C[i] != 0)
                fn_17_C390(ball);
        }
    }
}

void fn_17_C390(struct Ball *ball)
{
    struct Camera *camera = &cameras[ball->playerId];
    s16 markers[4] = { 0x1205, 0x1203, 0x1206, 0x1204 };
    s16 arrows[4] = { 0x1238, 0x1215, 0x1239, 0x1237 };
    struct SpriteDrawRequest req;
    struct Ball *other;
    int i;
    float x;
    float y;
    float markerY;
    float h;
    float scale;
    float maxHeight;
    float faceX;
    float dead;

    switch (current_stage_id)
    {
    case 0x98:
    case 0x9B:
        maxHeight = 400.0f;
        break;
    case 0x99:
    case 0x9A:
        maxHeight = 1000.0f;
        break;
    default:
        maxHeight = 200.0f;
        break;
    }
    x = fn_17_EE54(camera, 1) - 38.0f;
    switch (lbl_17_bss_24)
    {
    case 0:
    case 4:
        scale = 1.0f;
        y = 158.0f + fn_17_EEE0(camera, 2);
        break;
    default:
        scale = 1.0f;
        y = 50.0f + fn_17_EEE0(camera, 2);
        if (640.0f == fn_17_EE54(camera, 1) && 0.0f == fn_17_EEE0(camera, 2))
        {
            if (lbl_17_bss_24 != 3)
                y = 30.0f + fn_17_EEE0(camera, 2);
        }
        else if (640.0f == fn_17_EE54(camera, 1) && 240.0f == fn_17_EEE0(camera, 2))
        {
            y = 54.0f + fn_17_EEE0(camera, 2);
        }
        break;
    }
    set_ui_widescreen_scale_mtx(x);
    switch (modeCtrl.unk30)
    {
    case 3:
    case 4:
        fn_17_F320(&req, 0.5 * scale, scale, 0.5f);
        fn_17_F384(&req, 0x1200, 4.0f + x, y, 0.12f);
        break;
    default:
        fn_17_F320(&req, scale, scale, 0.5f);
        fn_17_F384(&req, 0x1200, x, y, 0.12f);
        break;
    }
    fn_17_F320(&req, scale, scale, 1.0f);
    h = 141.0f - ball->pos.y * (141.0f / maxHeight);
    if (h < -4.0f)
        h = -4.0f;
    else if (h > 181.0f)
        h = 181.0f;
    h *= scale;
    markerY = 3.0f + y + h;
    req.flags = 9;
    req.rotation = 0x8000;
    fn_17_F384(&req, arrows[ball->colorId], x + 10.0f * scale, markerY, 0.11f);
    faceX = x - 8.0 - 48.0f * scale;
    switch (modeCtrl.unk30)
    {
    case 1:
    case 2:
        fn_17_F320(&req, scale, scale, 1.0f);
        fn_17_F384(&req, markers[ball->colorId], faceX + 9.0f * scale, markerY - 23.0f * scale, 0.11f);
        req.scaleX = 0.25f * scale;
        req.scaleY = 0.1625f * scale;
        fn_17_F384(&req, lbl_1_rodata_A838[ball->ape->charaId], faceX + 15.0f * scale, markerY - 13.0f * scale, 0.11f);
        break;
    }
    fn_17_F320(&req, scale, scale, 1.0f);
    dead = 8.0f;
    for (i = 0; i < 4; i++)
    {
        other = &balls[i];
        if (other->status == 2 && other != ball)
        {
            float otherH = 141.0f - other->pos.y * (141.0f / maxHeight);

            if (otherH < -4.0f)
                otherH = -4.0f;
            else if (otherH > 181.0f)
                otherH = 181.0f;
            otherH *= scale;
            markerY = 3.0f + y + otherH;
            req.flags = 9;
            fn_17_F384(&req, arrows[other->colorId], x + 6.0f * scale, markerY, 0.11f);
        }
    }
    switch (modeCtrl.unk30)
    {
    case 1:
    case 2:
        fn_17_F320(&req, 3.0f, 0.25f, 1.0f);
        fn_17_F384(&req, 0x1202, x - 24.0f, 5.0f + y, 0.12f);
        fn_17_F384(&req, 0x1202, x - 24.0f, 76.0f + y, 0.12f);
        fn_17_F384(&req, 0x1202, x - 24.0f, 147.0f + y, 0.12f);
        textdraw_reset();
        textdraw_set_depth(0.12f);
        textdraw_set_font(0x85);
        textdraw_set_alignment(6);
        textdraw_set_pos(x, y - 5.0f);
        if (h <= -4.0f)
        {
            if ((g_font_char_var_3 >> 3) & 1)
                textdraw_printf("%d", (int)ball->pos.y);
        }
        else
        {
            textdraw_printf("%d", (int)maxHeight);
        }
        textdraw_set_pos(x, 66.0f + y);
        textdraw_printf("%d", (int)(0.5f * maxHeight));
        textdraw_set_pos(x, 137.0f + y);
        textdraw_printf("0");
        break;
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_17_CA20(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float speed;
    float x;
    float y;
    int i;
    struct Ball *ball;
    struct Camera *camera;

    if (lbl_17_bss_14 >= 9)
    {
        fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
        for (i = 0; i < 4; i++)
        {
            ball = &balls[i];
            camera = &cameras[i];
            if (ball->status == 2 && lbl_17_bss_2C[i] != 0)
            {
                x = fn_17_EE54(camera, 1) - 102.0f;
                y = fn_17_EEE0(camera, 3) - 30.0f;
                if (lbl_17_bss_24 == 4)
                {
                    if ((lbl_1_bss_D4D30 & 1) && 320.0f == fn_17_EE54(camera, 1))
                        x -= 24.0f;
                }
                else if (640.0f == fn_17_EE54(camera, 1) && 0.0f == fn_17_EEE0(camera, 2))
                {
                    switch (lbl_17_bss_24)
                    {
                    case 1:
                    case 2:
                        y = fn_17_EEE0(camera, 3) - 55.0f;
                        break;
                    }
                }
                speed = mathutil_vec_len(&ball->vel);
                textdraw_reset();
                textdraw_set_font(0x7E);
                textdraw_set_pos(x, y);
                set_ui_widescreen_scale_mtx(fn_17_EE54(camera, 1) - 16.0);
                if (locale == 0)
                    textdraw_printf("%3d", (int)(134.21985f * speed));
                else
                    textdraw_printf("%3d", (int)(216.0f * speed));
                fn_17_F384(&req, 0x1214, 45.0f + x, y, 0.1f);
                reset_ui_widescreen_scale_mtx();
            }
        }
    }
}

void fn_17_CCD8(s8 *status, struct Sprite *sprite)
{
    if (lbl_17_bss_14 < 0xB)
        sprite->unk8C |= 1;
    else
        sprite->unk8C &= ~1;
    if (lbl_17_bss_14 >= 0xF && cameras[0].unk1F >= 5)
        *status = 0;
}

void fn_17_CD40(struct Sprite *sprite)
{
    float screenX;
    float screenY;
    float cx;
    float cy;
    float radius;
    int i;
    struct Ball *ball;
    struct Camera *camera;

    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        camera = &cameras[i];
        if (ball->status == 2 && lbl_17_bss_2C[i] != 0)
        {
            screenX = sprite->x + fn_17_EE54(camera, 0);
            cx = 72.0f * sprite->scaleX + screenX;
            screenY = sprite->y + fn_17_EEE0(camera, 2);
            cy = 72.0f * sprite->scaleY + screenY;
            radius = 72.0f * sprite->scaleX;
            fn_17_CE60(i, cx, cy, radius);
        }
    }
}

void fn_17_CE60(int id, float x, float y, float radius)
{
    Mtx mtx;
    float k;
    float scale;
    float s;
    int i;
    struct Camera *camera;

    camera = &cameras[id];
    k = 1.0f;
    scale = 0.1f;
    scale *= k;
    mtxa_from_identity();
    mtxa_translate_xyz(x, y, -0.2f);
    if (modeCtrl.wideMode != 0)
        mtxa_scale_xyz(0.75f, 1.0f, 1.0f);
    mtxa_rotate_x(-0x4000);
    mtxa_rotate_y(-camera->rotY);
    mtxa_scale_s(scale);
    mtxa_to_mtx(mtx);
    for (i = 0; i < 0x100; i++)
    {
        if (g_poolInfo.itemPool.statusList[i] != 0)
        {
            struct Item *item = &items[i];

            if (item->type == 6)
            {
                switch (item->subType)
                {
                case 8:
                    set_post_mult_color(1.0f, 0.5f, 0.0f, 1.0f);
                    break;
                case 2:
                    set_post_mult_color(1.0f, 1.0f, 0.3f, 1.0f);
                    break;
                default:
                    continue;
                }
                {
                    Vec pos;

                    pos.x = item->pos.x - camera->eye.x;
                    pos.y = 0.0f;
                    pos.z = item->pos.z - camera->eye.z;
                    mtxa_from_identity();
                    mathutil_mtxA_set_translate(&pos);
                    mtxa_mult_left(mtx);
                    fn_17_D3F8(&mtxa->mtxA[0][3], &mtxa->mtxA[1][3], x, y, radius);
                    s = 0.25f / scale;
                    mtxa_scale_s(s);
                    avdisp_set_scale_factor(s);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_auto(minigameGma[0]->modelEntries[0x76].model);
                    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
        }
    }
    for (i = 0; i < 4; i++)
    {
        s16 models[4] = { 0x54, 0x55, 0x56, 0x57 };
        struct Ball *ball = &balls[i];
        struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];

        if (ball->status == 2 && i != id && !(player->flags & 0x10))
        {
            Vec pos;

            pos.x = ball->pos.x - camera->eye.x;
            pos.y = 0.0f;
            pos.z = ball->pos.z - camera->eye.z;
            mtxa_from_identity();
            mtxa_rotate_y(ball->rotY);
            mathutil_mtxA_set_translate(&pos);
            mtxa_mult_left(mtx);
            fn_17_D3F8(&mtxa->mtxA[0][3], &mtxa->mtxA[1][3], x, y, radius);
            s = 1.0f / scale;
            mtxa_scale_s(s);
            avdisp_set_scale_factor(s);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(minigameGma[0]->modelEntries[models[i]].model);
        }
    }
    for (i = 0; i < 0x3C; i++)
    {
        s16 models[4] = { 0x54, 0x55, 0x56, 0x57 };
        struct DogfightShot *shot = &lbl_17_bss_3DC[i];

        if (shot->status == 2 && shot->unk8 == 0 && !(shot->flags & 2)
         && (!(shot->flags & 1) || (shot->unk84 <= lbl_80198B38 && !((shot->unk84 >> 2) & 1))))
        {
            Vec pos;

            pos.x = shot->pos.x - camera->eye.x;
            pos.y = 0.0f;
            pos.z = shot->pos.z - camera->eye.z;
            mtxa_from_identity();
            mtxa_rotate_y(shot->unk74);
            mathutil_mtxA_set_translate(&pos);
            mtxa_mult_left(mtx);
            fn_17_D3F8(&mtxa->mtxA[0][3], &mtxa->mtxA[1][3], x, y, radius);
            s = 0.65f / scale;
            mtxa_scale_s(s);
            avdisp_set_scale_factor(s);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            set_post_mult_color(1.0f, 0.5f, 0.0f, 1.0f);
            avdisp_draw_model_unculled_sort_auto(minigameGma[0]->modelEntries[0x74].model);
            avdisp_draw_model_unculled_sort_auto(minigameGma[0]->modelEntries[0x73].model);
            set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    g_some_sprite_reset_gx_state_func();
    mtxa_from_identity();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
}

void fn_17_D3F8(float *px, float *py, float x, float y, float radius)
{
    if (*py > y + radius)
        *py = y + radius;
    if (*py < y - radius)
        *py = y - radius;
    if (modeCtrl.wideMode != 0)
        radius *= 0.75;
    if (*px > x + radius)
        *px = x + radius;
    if (*px < x - radius)
        *px = x - radius;
}

void fn_17_D484(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float dead;
    char *text;

    fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
    textdraw_reset();
    switch (lbl_17_bss_24)
    {
    default:
        dead = 1.0f;
        break;
    case 1:
    case 2:
    case 4:
        if (lbl_17_bss_24 == 2 && modeCtrl.splitscreenMode == 3)
            break;
        dead = 430.0f;
        dead = 228.0f;
        dead = 272.0f;
        dead = 135.0f;
        dead = 215.0f;
        dead = 0.85f;
        dead = 0.09f;
        text = "TIME UP";
        break;
    case 3:
        dead = 1.0f;
        break;
    }
}

void fn_17_D520(int id)
{
    float x;
    float y;
    float scale;
    struct Camera *camera;
    struct Sprite *sprite;

    camera = &cameras[id];
    x = fn_17_EE54(camera, 4);
    y = fn_17_EEE0(camera, 4);
    switch (lbl_17_bss_24)
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
        sprite->y = y - 40.0f * scale;
        sprite->type = 0;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->scaleX = scale;
        sprite->scaleY = scale;
        sprite->depth = 0.09f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0x40;
        sprintf(sprite->text, "WIN");
    }
}

void fn_17_D64C(int id)
{
    float x;
    float y;
    float scale;
    struct Camera *camera;
    struct Sprite *sprite;

    camera = &cameras[id];
    x = fn_17_EE54(camera, 4);
    y = fn_17_EEE0(camera, 4);
    switch (lbl_17_bss_24)
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
        sprite->y = y - 40.0f * scale;
        sprite->type = 0;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->scaleX = 0.8f * scale;
        sprite->scaleY = scale;
        sprite->depth = 0.09f;
        sprintf(sprite->text, "LOSE");
    }
}

void fn_17_D768(int id)
{
    float x;
    float y;
    float scale;
    struct Camera *camera;
    struct Sprite *sprite;

    camera = &cameras[id];
    x = fn_17_EE54(camera, 4);
    y = fn_17_EEE0(camera, 4);
    switch (lbl_17_bss_24)
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
        sprite->y = y - 40.0f * scale;
        sprite->type = 0;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->scaleX = 0.8f * scale;
        sprite->scaleY = scale;
        sprite->depth = 0.09f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0x40;
        sprintf(sprite->text, "DRAW");
    }
}

void fn_17_D89C(void)
{
    int i;
    struct Sprite *sprite;

    for (i = 0; i < 4; i++)
    {
        lbl_17_bss_354[i] = 0;
        lbl_17_bss_364[i] = (int)(0.5f * lbl_80198B34) + (int)(0.5f * math_sqrt((float)lbl_17_bss_314[i]));
        if (lbl_17_bss_364[i] > lbl_80198B38 * 2)
            lbl_17_bss_364[i] = lbl_80198B38 * 2;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->bmpId = 0x4B;
        sprite->x = 0.0f;
        sprite->y = 0.0f;
        sprite->depth = 0.075f;
        sprite->scaleX = 80.0f;
        sprite->scaleY = 60.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->mainFunc = fn_17_DA50;
        sprite->userVar = 0;
    }
    fn_17_DD30();
}

void fn_17_DA50(s8 *status, struct Sprite *sprite)
{
    int i;
    int step;

    if ((float)lbl_17_bss_350 >= lbl_80198B34 + 0.5f * lbl_80198B34)
    {
        for (i = 0; i < 4; i++)
        {
            if (lbl_17_bss_354[i] < lbl_17_bss_314[i])
            {
                step = lbl_17_bss_314[i] / lbl_17_bss_364[i];
                if (step + lbl_17_bss_354[i] > lbl_17_bss_314[i])
                    step = lbl_17_bss_314[i] - lbl_17_bss_354[i];
                else if (step < 1)
                    step = 1;
                lbl_17_bss_354[i] += step;
                if (lbl_17_bss_364[i] > 0)
                    lbl_17_bss_364[i]--;
            }
            else
            {
                lbl_17_bss_364[i] = 0;
            }
        }
    }
    if (lbl_17_bss_350 < lbl_80198B38 * 3)
    {
        if (sprite->opacity < 0.5f)
            sprite->opacity += 0.5f / (0.5f * lbl_80198B34);
        else
            sprite->opacity = 0.5f;
    }
    else if (lbl_17_bss_350 > lbl_80198B38 * 5)
    {
        if (sprite->opacity > 0.0f)
        {
            sprite->opacity -= 0.5f / (0.5f * lbl_80198B34);
        }
        else
        {
            sprite->opacity = 0.0f;
            *status = 0;
        }
    }
}

void fn_17_DD30(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 640.0f;
        sprite->mainFunc = fn_17_DD80;
        sprite->drawFunc = fn_17_DED4;
    }
}

void fn_17_DD80(s8 *status, struct Sprite *sprite)
{
    float dead;

    dead = 0.05f;
    dead = 180.0f;
    if (sprite->x > 0.0f)
        sprite->x -= 640.0f / (0.5f * lbl_80198B34);
    if ((float)lbl_17_bss_350 > lbl_80198B34 + (float)(lbl_80198B38 * 4))
        sprite->x -= 640.0f / (0.5f * lbl_80198B34);
    if ((float)lbl_17_bss_350 > lbl_80198B34 + (float)(lbl_80198B38 * 5))
        *status = 0;
}

void fn_17_DED4(struct Sprite *sprite)
{
    s16 panels[4] = { 0x122B, 0x1229, 0x122C, 0x122A };
    struct SpriteDrawRequest req;
    float x;
    float y;
    float t;
    int icon;
    int i;

    set_ui_widescreen_scale_mtx(0x140);
    x = 80.0f + sprite->x;
    if (lbl_17_bss_28 <= 2)
        y = 188.0f;
    else
        y = 136.0f;
    for (i = 0; i < lbl_17_bss_28; i++)
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
        fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
        fn_17_F384(&req, panels[i], x, y, 0.04f);
        fn_17_F384(&req, 0x1228, 104.0f + x, y, 0.04f);
        req.scaleX = 0.5f;
        req.scaleY = 0.325f;
        req.rotation = 2048.0f * math_sin(g_font_char_var_3 * 0x190 + 0x4000);
        req.flags = 0xA;
        fn_17_F384(&req, lbl_17_rodata_4D0[lbl_17_bss_40[i]].unk0, 58.0f + x, 42.0f + y, 0.03f);
        t = 2.0f * (float)(g_font_char_var_3 % (int)lbl_80198B34) / lbl_80198B34;
        if (t > 1.0f)
            t = 2.0f - t;
        req.opacity = t;
        fn_17_F384(&req, lbl_17_rodata_4D0[lbl_17_bss_40[i]].unk8, 58.0f + x, 42.0f + y, 0.02f);
        textdraw_reset();
        textdraw_set_depth(0.03f);
        icon = i;
        if (lbl_17_bss_48[i].flags & 0x800)
            icon = 4;
        fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
        req.u1 = 0.2f * (float)icon;
        req.u2 = 0.2f + req.u1;
        req.v1 = 0.25f * (float)fn_17_F3B4(balls[i].colorId);
        req.v2 = 0.25f + req.v1;
        req.scaleX = 0.2f;
        req.scaleY = 0.25f;
        fn_17_F384(&req, 0x121E, 44.0f + x, 66.0f + y, 0.03f);
        fn_17_F320(&req, 1.0f, 1.0f, 1.0f);
        req.mulColor = 0;
        req.addColor = 0x5C3B1B;
        fn_17_F384(&req, 0x121A, 103.0f + x, 25.0f + y, 0.03f);
        textdraw_set_font(0x80);
        textdraw_set_mul_color(0xFF000000);
        textdraw_set_add_color(req.addColor);
        textdraw_set_pos(114.0f + x, 47.0f + y);
        textdraw_printf("%6d", lbl_17_bss_354[i]);
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_17_E30C(void)
{
    switch (lbl_17_bss_24)
    {
    case 1:
    case 4:
        if (modeCtrl.wideMode != 0)
            fn_17_E418(0.0f, 1.0f);
        else
            fn_17_E500(0.0f, 1.0f);
        break;
    case 2:
        fn_17_E500(0.0f, 1.0f);
        switch (modeCtrl.splitscreenMode)
        {
        default:
            fn_17_E418(0.5f, 1.0f);
            break;
        case 2:
            fn_17_E418(0.0f, 0.5f);
            break;
        case 3:
            fn_17_E418(0.0f, 1.0f);
            break;
        }
        break;
    case 3:
        fn_17_E418(0.0f, 1.0f);
        fn_17_E500(0.0f, 1.0f);
        break;
    }
}

void fn_17_E418(float a, float b)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->bmpId = 0x4C;
        sprite->depth = 0.5f;
        sprite->x = 320.0f;
        sprite->y = 240.0 * (a + b);
        sprite->scaleX = 1.0f;
        sprite->scaleY = 60.0 * (b - a);
        sprite->textAlign = 4;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = sprite->x;
        sprintf(sprite->text, "SEP TATE");
    }
}

void fn_17_E500(float a, float b)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->bmpId = 0x4C;
        sprite->depth = 0.4f;
        sprite->x = 320.0 * (a + b);
        sprite->y = 240.0f;
        sprite->rotation = 0x4000;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 80.0 * (b - a);
        sprite->textAlign = 4;
        sprite->flags = 0x200000;
        sprintf(sprite->text, "SEP YOKO");
    }
}

void fn_17_E5DC(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->bmpId = 0x1225;
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
        sprite->bmpId = 0x1235;
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
        sprite->mainFunc = fn_17_E768;
        sprite->drawFunc = fn_17_E778;
        sprintf(sprite->text, "LIVE STAR");
    }
}

void fn_17_E768(s8 *status, struct Sprite *sprite)
{
    sprite->rotation += 0x300;
}

void fn_17_E778(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float dyProduct;
    float outY;
    register float s;
    register float dy;
    register float dx;
    register float fade;
    register float t;
    s16 i;
    s16 angle;

    angle = g_font_char_var_3 << 9;
    for (i = 0; i < 0xD; i++)
    {
        angle += 0x400;
        if (angle < 0x1800 || angle > 0x5800)
        {
            if (angle > 0 && angle < 0x1800)
                fade = (float)(0x1800 - angle) / 6144.0f;
            else if (angle > 0x5800 && angle < 0x7000)
                fade = (float)(angle - 0x5800) / 6144.0f;
            else
                fade = 1.0f;
            t = (float)(i + 1) / 13.0f;
            fade *= t;
#ifdef __MWERKS__
            /* Keep the double products unrounded until the final conversion. */
            {
                register float sine;
                register double scalar;

                sine = math_sin(angle + 0x4000);
                scalar = 32.0;
                asm
                {
                    fmul sine, scalar, sine
                }
                scalar = sprite->scaleX;
                asm
                {
                    fmul dx, scalar, sine
                    frsp dx, dx
                }
            }
            {
                register float sine;
                register double scalar;

                sine = math_sin(angle);
                scalar = 6.0;
                asm
                {
                    fmul sine, scalar, sine
                }
                scalar = sprite->scaleY;
                asm
                {
                    fmul dy, scalar, sine
                    frsp dy, dy
                }
            }
#else
            dx = sprite->scaleX * (32.0 * math_sin(angle + 0x4000));
            dy = sprite->scaleY * (6.0 * math_sin(angle));
#endif
            fn_17_F320(&req, sprite->scaleX * t, sprite->scaleY * t, sprite->opacity * fade);
            req.flags = 0x20000A;
            req.rotation = sprite->rotation;
            req.mulColor = sprite->mulB | ((sprite->mulR << 16) | 0xFF000000 | (sprite->mulG << 8));
            s = dy * math_sin(-0x5800);
            outY = (sprite->y + dx * math_sin(0x6800)) - -s;
            dyProduct = dy * math_sin(0x6800);
            fn_17_F384(&req, sprite->bmpId, (sprite->x + dx * math_sin(-0x5800)) - dyProduct, outY, sprite->depth);
        }
    }
}

void fn_17_E9FC(void)
{
    float x;
    float y;
    struct Sprite *sprite;

    x = 320.0f;
    y = 240.0f;
    if (modeCtrl.unk30 == 1)
        y -= 120.0;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 0;
        sprite->fontId = 9;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.05f;
        sprite->opacity = 0.0f;
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_17_EAD0;
    }
}

void fn_17_EAD0(s8 *status, struct Sprite *sprite)
{
    float d;
    float o;
    int n;
    int t;

    n = lbl_17_bss_348 / lbl_80198B38;
    t = lbl_17_bss_348 % lbl_80198B38;
    sprintf(sprite->text, "%d", n);
    if ((double)t >= 0.49999999999999989 * lbl_80198B34)
    {
        d = ((double)t - 0.49999999999999989 * lbl_80198B34) / (0.5 * lbl_80198B34);
        o = 1.0 - d;
        sprite->scaleX = 1.0 + d;
        sprite->scaleY = sprite->scaleX;
        sprite->opacity = o;
    }
    else if ((double)t >= 0.29999999999999988 * lbl_80198B34)
    {
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
        sprite->opacity = 1.0f;
    }
    else if ((double)t >= 0.099999999999999867 * lbl_80198B34)
    {
        d = ((double)t - 0.099999999999999867 * lbl_80198B34) / (0.20000000000000001 * lbl_80198B34);
        o = 1.0 - d;
        sprite->scaleX = 1.0 + 1.5 * o;
        sprite->scaleY = d;
        sprite->opacity = d;
    }
    else
    {
        sprite->scaleX = 0.0f;
        sprite->scaleY = 0.0f;
        sprite->opacity = 0.0f;
    }
}

void fn_17_ED48(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 640.0f;
        sprite->y = 240.0f;
        sprite->type = 0;
        sprite->fontId = 9;
        sprite->textAlign = 1;
        sprite->mulR = 0xFF;
        sprite->mulG = 0;
        sprite->mulB = 0xFF;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
        sprite->depth = 0.05f;
        sprintf(sprite->text, "CHANCE TIME");
        sprite->mainFunc = fn_17_EE04;
    }
}

void fn_17_EE04(s8 *status, struct Sprite *sprite)
{
    sprite->x -= 640.0f / lbl_80198B34;
    if (sprite->x <= -1000.0f)
        *status = 0;
}

float fn_17_EE54(struct Camera *camera, int mode)
{
    float r;

    switch (mode)
    {
    case 0:
        r = 640.0f * camera->sub28.viewportX;
        break;
    case 1:
        r = 640.0f * (camera->sub28.viewportX + camera->sub28.viewportW);
        break;
    case 4:
        r = 640.0f * camera->sub28.viewportX + 320.0f * camera->sub28.viewportW;
        break;
    }
    return r;
}

float fn_17_EEE0(struct Camera *camera, int mode)
{
    float r;

    switch (mode)
    {
    case 2:
        r = 480.0f * camera->sub28.viewportY;
        break;
    case 3:
        r = 480.0f * (camera->sub28.viewportY + camera->sub28.viewportH);
        break;
    case 4:
        r = 480.0f * camera->sub28.viewportY + 240.0f * camera->sub28.viewportH;
        break;
    }
    return r;
}

int fn_17_EF6C(int id, int corner)
{
    s16 pos[2];
    struct Camera *camera;

    camera = &cameras[id];
    switch (corner)
    {
    case 0:
        pos[0] = 640.0f * camera->sub28.viewportX;
        pos[1] = 480.0f * camera->sub28.viewportY;
        break;
    case 1:
        pos[0] = 640.0f * (camera->sub28.viewportX + camera->sub28.viewportW);
        pos[1] = 480.0f * camera->sub28.viewportY;
        break;
    case 2:
        pos[0] = 640.0f * camera->sub28.viewportX;
        pos[1] = 480.0f * camera->sub28.viewportY + 240.0f * camera->sub28.viewportH;
        break;
    case 3:
        pos[0] = 640.0f * (camera->sub28.viewportX + camera->sub28.viewportW);
        pos[1] = 480.0f * camera->sub28.viewportY + 240.0f * camera->sub28.viewportH;
        break;
    case 4:
        pos[0] = 640.0f * camera->sub28.viewportX;
        pos[1] = 480.0f * (camera->sub28.viewportY + camera->sub28.viewportH);
        break;
    case 5:
        pos[0] = 640.0f * (camera->sub28.viewportX + camera->sub28.viewportW);
        pos[1] = 480.0f * (camera->sub28.viewportY + camera->sub28.viewportH);
        break;
    case 6:
        pos[0] = 640.0f * camera->sub28.viewportX + 320.0f * camera->sub28.viewportW;
        pos[1] = 480.0f * camera->sub28.viewportY + 240.0f * camera->sub28.viewportH;
        break;
    }
    return *(int *)pos;
}

void g_some_printf_function_10(char *fmt, ...)
{
    va_list args;
    char buf[0x20];
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

void fn_17_F320(struct SpriteDrawRequest *req, float scaleX, float scaleY, float opacity)
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

void fn_17_F384(struct SpriteDrawRequest *req, int bmpId, float x, float y, float z)
{
    req->bmpId = bmpId;
    req->x = x;
    req->y = y;
    req->z = z;
    draw_sprite_draw_request(req);
}

int fn_17_F3B4(int colorId)
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
