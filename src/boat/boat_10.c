#include "boat.h"
#include "macros.h"
#include "bitmap.h"
#include "sprite.h"
#include "textdraw.h"

extern s32 locale;
extern u32 lbl_1_data_2E490[];

void call_SoundReqID_arg_0(u32 id);
void set_ui_widescreen_scale_mtx(int width);
void reset_ui_widescreen_scale_mtx(void);
float textdraw_get_pixel_height_of_string(char *text);
float textdraw_get_pixel_width_of_string_as_f32(char *text);
void textdraw_print(char *text);
void textdraw_printf(char *fmt, ...);
void textdraw_put_char(u8 c);
void textdraw_reset(void);
void textdraw_set_add_color(u32 color);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_alpha(float alpha);
void textdraw_set_border(void);
void textdraw_set_depth(float depth);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_font_style(int style);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);

#define VP_X(i) (s32) (640.0f * cameras[i].sub28.viewportX)
#define VP_Y(i) (s32) (480.0f * cameras[i].sub28.viewportY)
#define VP_W(i) (s32) (640.0f * cameras[i].sub28.viewportW)
#define VP_H(i) (s32) (480.0f * cameras[i].sub28.viewportH)
#define BOAT_FPS2 ((s32) ((2.0 * lbl_80198B34) / 15.0))
#define BALL_FLAG_BOAT_CPU ((u64) 1 << 43)

#define REQ_INIT(sx, sy, op) \
    req.scaleX = sx; \
    req.scaleY = sy; \
    req.u1 = req.v1 = 0.0f; \
    req.u2 = req.v2 = 1.0f; \
    req.rotation = 0; \
    req.unk30 = -1; \
    req.flags = 5; \
    req.opacity = op; \
    req.mulColor = 0xFFFFFF; \
    req.addColor = 0;

struct BoatRgb
{
    u8 r, g, b, a;
};

struct BoatMapInfo
{
    /*0x00*/ s32 bmpId;
    /*0x04*/ s32 x;
    /*0x08*/ s32 y;
    /*0x0C*/ float scale;
    /*0x10*/ float vScale;
    /*0x14*/ float height;
    /*0x18*/ u32 pad[3];
};


u32 lbl_14_data_1AF0[0x7A] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000, 0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000, 0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000, 0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000, 0x3F800000, 0x00000000, 0x00000000, 0x00000000,
    0x3E99999A, 0x3F800000, 0x3F800000, 0x3F666666, 0x00000000, 0x00000000, 0x3F4CCCCD, 0x00000000,
    0x00000000, 0x3F4CCCCD, 0x3F4CCCCD, 0x3F800000, 0x00000000, 0x3F4CCCCD, 0x3E4CCCCD, 0x3ECCCCCD,
    0x3ECCCCCD, 0x3F800000, 0x3F800000, 0x3F666666, 0x3F333333, 0x00000000, 0x3F800000, 0x3F333333,
    0x3F800000, 0x00000000,
};

struct BoatRgb lbl_14_data_1CD8[0xA] = {
    {0xFF, 0x00, 0x00, 0xFF}, {0x00, 0x4C, 0xFF, 0xFF}, {0xFF, 0xE5, 0x00, 0xFF}, {0x00, 0xCC, 0x00, 0xFF},
    {0x00, 0xCC, 0xCC, 0xFF}, {0xFF, 0x00, 0xCC, 0xFF}, {0x33, 0x66, 0x66, 0xFF}, {0xFF, 0xFF, 0xE5, 0xFF},
    {0xB2, 0x00, 0xFF, 0xFF}, {0xB2, 0xFF, 0x00, 0xFF},
};

u16 *lbl_14_bss_6350[4];

char *lbl_14_data_1D18[6] = {"] REPLAY ]", "] REPLAY ]", "] REPLAY ]", "] REPLAY ]", "] REPLAY ]", "-リプレイ-"};
char *lbl_14_data_1D3C[6] = {"ROUND %d", "ROUND %d", "ROUND %d", "ROUND %d", "ROUND %d", "ROUND %d"};
char *lbl_14_data_1D60[6] = {"FINAL ROUND", "FINAL ROUND", "FINAL ROUND", "FINAL ROUND", "FINAL ROUND", "FINAL ROUND"};

void fn_14_19618(float seconds)
{
    float height;
    float x;
    float y;
    struct Sprite *sprite;

    x = 320.0f;
    y = 240.0f;
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 0;
        if (locale == 5) {
            sprite->fontId = 3;
            sprite->unk58 = 2.0f;
            sprite->unk5C = 2.0f;
        } else {
            sprite->fontId = 9;
            sprite->unk58 = 0.7f;
            sprite->unk5C = 1.0f;
        }
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.09f;
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0x80;
        sprite->mainFunc = fn_14_19A14;
        sprite->drawFunc = fn_14_19BC8;
        sprite->counter = seconds * lbl_80198B34;
        sprite->userVar = seconds * lbl_80198B34;
        sprintf(sprite->text, "%s", lbl_14_bss_2AE8->names[locale]);
        if ((lbl_14_bss_20.unk2 & 0x10) || (lbl_14_bss_8 & 4)) {
            textdraw_reset();
            textdraw_set_font(sprite->fontId);
            height = 24.0 + sprite->unk5C * (0.5 * textdraw_get_pixel_height_of_string(lbl_14_bss_2AE8->names[locale]));
            sprite = create_sprite();
            if (sprite != NULL) {
                sprite->type = 0;
                sprite->fontId = 9;
                sprite->x = x;
                sprite->depth = 0.09f;
                sprite->textAlign = 4;
                sprite->flags = 0x01000000;
                sprite->widescreenArg = x;
                sprite->mulR = 0;
                sprite->mulG = 0xC0;
                sprite->mulB = 0xFF;
                sprite->mainFunc = fn_14_19A14;
                sprite->drawFunc = fn_14_19BC8;
                sprite->counter = seconds * lbl_80198B34;
                sprite->userVar = seconds * lbl_80198B34;
                sprite->unk58 = 0.5f;
                sprite->unk5C = 0.5f;
                if (lbl_14_bss_8 & 4) {
                    if (locale == 5) {
                        sprite->fontId = 3;
                        sprite->unk58 = 1.0f;
                        sprite->unk5C = 1.0f;
                    }
                    sprite->y = y + height;
                    sprintf(sprite->text, "%s", lbl_14_data_1D18[locale]);
                } else {
                    sprite->y = y - height;
                    if (lbl_14_bss_25 == 3) {
                        sprintf(sprite->text, lbl_14_data_1D60[locale]);
                    } else {
                        sprintf(sprite->text, lbl_14_data_1D3C[locale], lbl_14_bss_25);
                    }
                }
            }
        }
    }
}

void fn_14_19A14(s8 *status, struct Sprite *sprite)
{
    s32 t;

    t = sprite->userVar - sprite->counter;
    if (t < 0x1E) {
        sprite->opacity = 0.03333 * t;
        sprite->scaleX = math_sin(t * 0x222);
        sprite->scaleY = 1.0f;
    } else if (t < 0x3C) {
        sprite->opacity = 1.0f;
        sprite->scaleX = 1.0 - 0.2 * math_sin((t - 0x1E) * 0x444);
        sprite->scaleY = 1.0f;
    } else if (sprite->counter <= 0xF) {
        sprite->opacity = 0.06666 * sprite->counter;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 0.06666 * sprite->counter;
    } else {
        sprite->opacity = 1.0f;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
    }
    sprite->scaleX *= sprite->unk58;
    sprite->scaleY *= sprite->unk5C;
    if (--sprite->counter <= 0) {
        *status = 0;
    }
}

void fn_14_19BC8(struct Sprite *sprite)
{
    float scale;

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_alignment(sprite->textAlign);
    textdraw_set_pos(sprite->x, sprite->y);
    textdraw_set_depth(sprite->depth);
    textdraw_set_alpha(sprite->opacity);
    textdraw_set_mul_color((sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB);
    scale = 550.0 / (sprite->unk58 * textdraw_get_pixel_width_of_string_as_f32(sprite->text));
    if (scale > 1.0) {
        scale = 1.0f;
    }
    scale = 0.01 * (float) (s32) (100.0 * scale);
    textdraw_set_scale(sprite->scaleX * scale, sprite->scaleY);
    textdraw_set_border();
    textdraw_set_font_style(2);
    textdraw_print(sprite->text);
}

void fn_14_19CDC(void)
{
    struct Sprite *sprite;

    call_SoundReqID_arg_0(0x267);
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0xE22;
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->scaleX = 1.5f;
        sprite->scaleY = 1.5f;
        sprite->depth = 0.1f;
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = sprite->x;
        sprite->mainFunc = fn_14_19DB0;
        sprite->counter = lbl_80198B38 * 2;
        sprintf(sprite->text, "TIME BONUS");
    }
}

void fn_14_19DB0(s8 *status, struct Sprite *sprite)
{
    s32 phase;

    if (sprite->counter <= 0) {
        *status = 0;
        return;
    }
    sprite->counter--;
    phase = sprite->counter % BOAT_FPS2;
    if (phase > BOAT_FPS2 / 2) {
        sprite->unk8C &= ~1;
    } else {
        sprite->unk8C |= 1;
    }
    if (sprite->counter >= lbl_80198B38 * 2 - BOAT_FPS2 * 6 && phase == BOAT_FPS2 - 1) {
        call_SoundReqID_arg_0(0x270);
    }
}

void fn_14_19F48(void)
{
    float x;
    float y;
    struct Sprite *sprite;

    x = 320.0f;
    y = 220.0f;
    call_SoundReqID_arg_0(0x26C);
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 0;
        sprite->fontId = 9;
        sprite->x = x;
        sprite->y = y - 4.0;
        sprite->depth = 0.1f;
        sprite->scaleX = 0.48f;
        sprite->scaleY = 0.4f;
        sprite->textAlign = 5;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_1A0E4;
        sprite->counter = lbl_80198B38 * 2;
        sprintf(sprite->text, "FINAL LAP");
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0xE22;
        sprite->x = x;
        sprite->y = y - 4.0;
        sprite->scaleX = 1.5f;
        sprite->scaleY = 1.5f;
        sprite->depth = 0.1f;
        sprite->textAlign = 3;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_19DB0;
        sprite->counter = lbl_80198B38 * 2;
        sprintf(sprite->text, "TIME BONUS");
    }
}

void fn_14_1A0E4(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter <= 0) {
        *status = 0;
        return;
    }
    sprite->counter--;
    if (sprite->counter % (BOAT_FPS2 * 2) > BOAT_FPS2) {
        sprite->unk8C &= ~1;
    } else {
        sprite->unk8C |= 1;
    }
}

void fn_14_1A1BC(u8 playerId)
{
    float x;
    float y;
    struct BoatPlayer *pl;
    struct BoatLap *lap;
    struct Sprite *sprite;
    s8 align;

    pl = &lbl_14_bss_208[playerId];
    lap = &pl->unk50[pl->unk7 - 2];
    if (pl->unk7 == lbl_14_bss_20.unk1) {
        call_SoundReqID_arg_0(0x26C);
        x = VP_X(playerId) + VP_W(playerId) / 2;
        y = (VP_Y(playerId) + VP_H(playerId) / 2) - 10.0;
        sprite = create_sprite();
        if (sprite != NULL) {
            sprite->type = 0;
            sprite->fontId = 4;
            sprite->x = x;
            sprite->y = y - 2.0;
            sprite->depth = 0.1f;
            sprite->scaleX = 1.0f;
            sprite->scaleY = 0.8f;
            sprite->textAlign = 5;
            sprite->flags = 0x01000000;
            sprite->widescreenArg = x;
            sprite->mainFunc = fn_14_1A0E4;
            sprite->counter = lbl_80198B38 * 2;
            sprintf(sprite->text, "FINAL LAP");
        }
        y += 2.0;
        align = 3;
    } else {
        x = VP_X(playerId) + VP_W(playerId) / 2;
        y = VP_Y(playerId) + VP_H(playerId) / 2;
        align = 4;
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 0;
        sprite->fontId = 0x72;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->textAlign = align;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_19DB0;
        sprite->counter = lbl_80198B38 * 2;
        sprintf(sprite->text, "%02d;%02d<%02d", lap->unk8.min, lap->unk8.sec, lap->unk8.cs);
    }
}

void fn_14_1A5C8(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
        sprite->depth = 0.05f;
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = sprite->x;
        sprite->mainFunc = fn_14_1A664;
        sprite->drawFunc = fn_14_1A6EC;
    }
}

void fn_14_1A664(s8 *status, struct Sprite *sprite)
{
    if (lbl_80198B38 - 1 == lbl_14_bss_10 % lbl_80198B38) {
        call_SoundReqID_arg_0(0x26E);
    }
    if (lbl_14_bss_10 == 0) {
        *status = 0;
    }
}

void fn_14_1A6EC(struct Sprite *sprite)
{
    float scaleX;
    float scaleY;
    float alpha;
    float t;
    s16 sec;
    s16 frac;

    textdraw_reset();
    textdraw_set_font(9);
    textdraw_set_depth(sprite->depth);
    textdraw_set_flags(0x200000);
    sec = lbl_14_bss_10 / lbl_80198B38 + 1;
    frac = lbl_14_bss_10 % lbl_80198B38;
    if (frac >= 0x1E) {
        t = (float) (frac - 0x1E) * (1.0 / lbl_80198B34);
        scaleX = 1.0 + 1.5 * t;
        scaleY = scaleX;
        alpha = 1.0 - t;
    } else if (frac >= 0x14) {
        scaleX = 1.0f;
        scaleY = 1.0f;
        alpha = 1.0f;
    } else {
        t = 0.05 * frac;
        if (t > 0.0f) {
            scaleX = 1.0 / t;
            if (scaleX > 4.0) {
                scaleX = 4.0f;
            }
        } else {
            t = 0.0f;
        }
        scaleY = t;
        alpha = t;
    }
    textdraw_set_pos(sprite->x - 36.0 * scaleX, sprite->y - 32.0 * scaleY);
    textdraw_set_scale(scaleX, scaleY);
    textdraw_set_alpha(alpha);
    set_ui_widescreen_scale_mtx(sprite->x);
    textdraw_put_char(sec + 0x30);
    reset_ui_widescreen_scale_mtx();
}

void fn_14_1A918(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = 0x6C;
        sprite->type = 1;
        sprite->bmpId = 0xE15;
        sprite->x = (VP_X(0) + VP_W(0)) - 16.0;
        sprite->y = 16.0 + VP_Y(0);
        sprite->textAlign = 6;
        sprite->depth = 0.15f;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = sprite->x;
        sprintf(sprite->text, "boa_time_waku");
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = 0x6C;
        sprite->type = 0;
        sprite->x = ((VP_X(0) + VP_W(0)) - 16.0) - 4.0;
        sprite->y = 5.0 + (16.0 + VP_Y(0));
        sprite->fontId = 0x6E;
        sprite->textAlign = 6;
        sprite->depth = 0.11f;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = sprite->x;
        sprite->mulR = 0x5C;
        sprite->mulG = 0x3B;
        sprite->mulB = 0x1B;
        sprite->mainFunc = fn_14_1ABB8;
        sprintf(sprite->text, "000");
    }
}

void fn_14_1ABB8(s8 *status, struct Sprite *sprite)
{
    u16 t;

    t = lbl_14_bss_6C / lbl_80198B38;
    sprintf(sprite->text, "%03d", t);
}

void fn_14_stripped_time(struct Sprite *sprite) {
    float k;

    k = 100.0f;
    sprintf(sprite->text, ":%02d", (s32) (k * (float) (u32) lbl_14_bss_6C));
}

void fn_14_1AC0C(void)
{
    switch (modeCtrl.unk30) {
    case 2:
        if (modeCtrl.wideMode != 0) {
            fn_14_1AD14(0.0f, 1.0f);
        } else {
            fn_14_1ADF8(0.0f, 1.0f);
        }
        break;
    case 3:
        fn_14_1ADF8(0.0f, 1.0f);
        switch (modeCtrl.splitscreenMode) {
        default:
            fn_14_1AD14(0.5f, 1.0f);
            break;
        case 2:
            fn_14_1AD14(0.0f, 0.5f);
            break;
        case 3:
            fn_14_1AD14(0.0f, 1.0f);
            break;
        }
        break;
    case 4:
        fn_14_1AD14(0.0f, 1.0f);
        fn_14_1ADF8(0.0f, 1.0f);
        break;
    }
}

void fn_14_1AD14(float a, float b)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0x4C;
        sprite->depth = 0.5f;
        sprite->x = 320.0f;
        sprite->y = 240.0f * (a + b);
        sprite->scaleX = 1.0f;
        sprite->scaleY = 60.0 * (b - a);
        sprite->textAlign = 4;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = sprite->x;
        sprintf(sprite->text, "SEP TATE");
    }
}

void fn_14_1ADF8(float a, float b)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0x4C;
        sprite->depth = 0.4f;
        sprite->x = 320.0f * (a + b);
        sprite->y = 240.0f;
        sprite->rotation = 0x4000;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 80.0 * (b - a);
        sprite->textAlign = 4;
        sprite->flags = 0x200000;
        sprintf(sprite->text, "SEP YOKO");
    }
}

void fn_14_1AED0(u8 playerId)
{
    float x;
    float y;
    struct Sprite *sprite;

    x = VP_X(playerId) + 0x10;
    y = VP_Y(playerId) + 0x18;
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 4;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->textAlign = 0;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprintf(sprite->text, "LAP");
        sprite = create_linked_sprite(sprite);
        if (sprite != NULL) {
            sprite->tag = playerId + 0x6C;
            sprite->fontId = 0x78;
            sprite->textAlign = 2;
            if (lbl_14_bss_20.unk1 < 0xA) {
                sprite->x = -12.0f;
            } else {
                sprite->x = 3.0f;
            }
            sprite->y = 1.0f;
            sprite->depth = 0.1f;
            sprite->flags = 0x01000000;
            sprite->widescreenArg = x;
            sprite->mainFunc = fn_14_1B0CC;
            sprite->userVar = playerId;
        }
    }
}

void fn_14_1B0CC(s8 *status, struct Sprite *sprite)
{
    struct BoatPlayer *pl;

    pl = &lbl_14_bss_208[sprite->userVar];
    sprintf(sprite->text, "%2d:%d", pl->unk7, lbl_14_bss_AC->unk1);
}

void fn_14_1B124(u8 playerId)
{
    s32 a;
    s32 w;
    float x;
    float y;
    struct Camera *cam;
    struct Sprite *sprite;

    cam = &cameras[playerId];
    a = VP_X(playerId);
    w = VP_W(playerId) - 0x10;
    x = a + w;
    y = 16.0 + VP_Y(playerId);
    if (1.0 == cam->sub28.viewportW && 1.0 == cam->sub28.viewportH) {
        y += 50.0;
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        u8 glyphs[4] = {2, 1, 3, 0};

        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 0x71;
        sprite->x = x - 70.0f;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->textAlign = 6;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->userVar = playerId;
        sprintf(sprite->text, "%c", playerId + 0x31 + glyphs[playerId] * 4);
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 0x70;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->textAlign = 6;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_1B3AC;
        sprite->userVar = playerId;
        sprintf(sprite->text, "%dP 1st");
    }
}

void fn_14_1B3AC(s8 *status, struct Sprite *sprite)
{
    struct BoatPlayer *pl;
    u8 rank;
    float *color;

    pl = &lbl_14_bss_208[sprite->userVar];
    rank = pl->unk2 != 0 ? pl->unk2 : sprite->userVar + 1;
    {
        float colors[8][3] = {
            {255.0f, 62.0f, 25.0f},
            {230.0f, 152.0f, 44.0f},
            {237.0f, 199.0f, 39.0f},
            {232.0f, 236.0f, 24.0f},
            {161.0f, 236.0f, 19.0f},
            {19.0f, 236.0f, 36.0f},
            {24.0f, 230.0f, 201.0f},
            {40.0f, 113.0f, 216.0f},
        };

        color = colors[rank - 1];
        sprite->mulR = color[0];
        sprite->mulG = color[1];
        sprite->mulB = color[2];
    }
    sprintf(sprite->text, "%d", rank);
}

void fn_14_1B490(void)
{
    float x;
    float y;
    struct Sprite *sprite;

    x = 624.0f;
    y = 51.0f;
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = 0x6C;
        sprite->type = 1;
        sprite->bmpId = 0xE05;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->textAlign = 6;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = sprite->x;
        sprintf(sprite->text, "TIME ATTACK");
    }
}

void fn_14_1B550(u8 playerId)
{
    s32 a;
    s32 w;
    float x;
    float y;
    struct Camera *cam;
    struct Sprite *sprite;

    cam = &cameras[playerId];
    a = VP_X(playerId);
    w = VP_W(playerId) - 0x10;
    x = a + w;
    y = VP_Y(playerId) + 0x4D;
    if (1.0 == cam->sub28.viewportW && 1.0 == cam->sub28.viewportH) {
        y += 40.0;
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 1;
        sprite->bmpId = 0xE23;
        sprite->x = 4.0 + x;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->textAlign = 6;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprintf(sprite->text, "km/h or mph");
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 0x72;
        sprite->x = (4.0 + x) - 34.0;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->textAlign = 6;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_1B7BC;
        sprite->userVar = playerId;
        sprintf(sprite->text, "speed");
    }
}

void fn_14_1B7BC(s8 *status, struct Sprite *sprite)
{
    s16 speed;

    if (locale == 0) {
        speed = 134.21985f * math_sqrt(mathutil_vec_sq_len(&balls[sprite->userVar].vel));
    } else {
        speed = 216.0f * math_sqrt(mathutil_vec_sq_len(&balls[sprite->userVar].vel));
    }
    if (speed > 0x3E7) {
        speed = 0x3E7;
    }
    sprintf(sprite->text, "%3d", speed);
}

void fn_14_1B8C4(u8 playerId)
{
    float x;
    float y;
    float width;
    float scaleX;
    float scaleY;
    struct Camera *cam;
    struct Sprite *sprite;

    cam = &cameras[playerId];
    x = VP_X(playerId) + 0.5 * VP_W(playerId);
    y = (VP_Y(playerId) + VP_H(playerId)) - 16.0;
    if (0.5 == cam->sub28.viewportW) {
        width = 80.0f;
        scaleY = 0.75f;
    } else if (0.5 == cam->sub28.viewportH) {
        width = 224.0f;
        scaleY = 0.75f;
    } else {
        width = 224.0f;
        scaleY = 1.0f;
    }
    if (modeCtrl.wideMode != 0) {
        width *= 1.3333333333333333;
    }
    scaleX = width / 16.0;
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->tag = playerId + 0x6C;
        sprite->bmpId = 0xE2D;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.5f;
        sprite->scaleX = scaleX;
        sprite->scaleY = scaleY;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->drawFunc = fn_14_1BD00;
        sprite->userVar = playerId;
        sprite->unk58 = width;
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->tag = playerId + 0x6C;
        sprite->bmpId = 0xE2D;
        sprite->x = ((x - 8.0) - width) - 8.0;
        sprite->y = y;
        sprite->depth = 0.5f;
        sprite->scaleX = scaleX;
        sprite->scaleY = scaleY;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->drawFunc = fn_14_1BD00;
        sprite->userVar = playerId;
        sprite->unk58 = width;
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->tag = playerId + 0x6C;
        sprite->bmpId = 0xE31;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.51f;
        sprite->scaleX = scaleX;
        sprite->scaleY = scaleY;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_1BF00;
        sprite->drawFunc = fn_14_1C37C;
        sprite->userVar = playerId;
        sprite->unk50 = 2;
        sprite->unk58 = width;
        sprite->unk5C = 0.0f;
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->tag = playerId + 0x6C;
        sprite->bmpId = 0xE31;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.51f;
        sprite->scaleX = scaleX;
        sprite->scaleY = scaleY;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_1BF00;
        sprite->drawFunc = fn_14_1C5D0;
        sprite->userVar = playerId;
        sprite->unk50 = 1;
        sprite->unk58 = width;
        sprite->unk5C = 0.0f;
    }
}

void fn_14_1BD00(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;

    REQ_INIT(1.0f, sprite->scaleY, 1.0f);
    req.flags = 0x8000D;
    req.bmpId = 0xE32;
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = sprite->depth;
    draw_sprite_draw_request(&req);
    REQ_INIT(sprite->scaleX, sprite->scaleY, 1.0f);
    req.flags = 0xD;
    req.bmpId = 0xE2D;
    req.x = 8.0 + sprite->x;
    req.y = sprite->y;
    req.z = sprite->depth;
    draw_sprite_draw_request(&req);
    REQ_INIT(1.0f, sprite->scaleY, 1.0f);
    req.flags = 0xD;
    req.bmpId = 0xE32;
    req.x = 8.0 + sprite->x + sprite->unk58;
    req.y = sprite->y;
    req.z = sprite->depth;
    draw_sprite_draw_request(&req);
}

void fn_14_1BF00(s8 *status, struct Sprite *sprite)
{
    struct BoatPlayer *pl;
    s32 t;
    u8 chargeR;
    u8 chargeG;
    u8 chargeB;
    u8 flashR;
    u8 flashG;
    u8 flashB;
    u8 fullR;
    u8 fullG;
    u8 fullB;

    pl = &lbl_14_bss_208[sprite->userVar];
    t = pl->unk4C;
    chargeR = 0xC8;
    chargeG = 0xFF;
    chargeB = 0xE6;
    flashR = 0xC8;
    flashG = 0x28;
    flashB = 0x14;
    fullR = 0xFF;
    fullG = 0xFF;
    fullB = 0xB4;
    if (pl->unk4A4 & 8) {
        if (pl->unk3C == sprite->unk50) {
            if (t > 35.0) {
                sprite->unk5C = 1.0f;
            } else {
                sprite->unk5C = (float) t / 40.0;
            }
        } else if ((pl->unk3C == 1 || pl->unk3C == 2) && t > 40.0) {
            sprite->unk5C += 1.0 / (0.1 * lbl_80198B34);
            if (sprite->unk5C > 1.0) {
                sprite->unk5C = 1.0f;
            }
        } else {
            sprite->unk5C -= 1.0 / (0.2 * lbl_80198B34);
            if (sprite->unk5C < 0.0) {
                sprite->unk5C = 0.0f;
            }
        }
    } else if (pl->unk3C == sprite->unk50 && t < 35.0 + 0.4 * lbl_80198B34) {
        if (t > 35.0) {
            sprite->unk5C = 1.0f;
        } else {
            sprite->unk5C = (float) t / 40.0;
        }
    } else {
        sprite->unk5C -= 1.0 / (0.2 * lbl_80198B34);
        if (sprite->unk5C < 0.0) {
            sprite->unk5C = 0.0f;
        }
    }
    if (sprite->unk50 == 1) {
        pl->unk8 &= ~0x40000;
    } else {
        pl->unk8 &= ~0x80000;
    }
    if (sprite->unk5C < 0.2) {
        sprite->mulR = chargeR;
        sprite->mulG = chargeG;
        sprite->mulB = chargeB;
    } else if (sprite->unk5C < 1.0) {
        sprite->mulR = chargeR;
        sprite->mulG = chargeG;
        sprite->mulB = chargeB;
    } else {
        if (sprite->unk50 == 1) {
            pl->unk8 |= 0x40000;
        } else {
            pl->unk8 |= 0x80000;
        }
        if (g_font_char_var_3 & 2) {
            sprite->mulR = flashR;
            sprite->mulG = flashG;
            sprite->mulB = flashB;
        } else {
            sprite->mulR = fullR;
            sprite->mulG = fullG;
            sprite->mulB = fullB;
        }
    }
    if (lbl_1_bss_D4328 & 0x80000) {
        if (sprite->unk50 == 1) {
            if (!(pl->unkC & 0x40000) && (pl->unk8 & 0x40000)) {
                call_SoundReqID_arg_0(pl->unk0 + 0x3E1);
            } else if ((pl->unkC & 0x40000) && !(pl->unk8 & 0x40000)) {
                SoundOffID(pl->unk0 + 0x3E1);
            }
        } else {
            if (!(pl->unkC & 0x80000) && (pl->unk8 & 0x80000)) {
                call_SoundReqID_arg_0(pl->unk0 + 0x3E5);
            } else if ((pl->unkC & 0x80000) && !(pl->unk8 & 0x80000)) {
                SoundOffID(pl->unk0 + 0x3E5);
            }
        }
    }
}

void fn_14_1C37C(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    s32 color;

    color = 0xFF000000 | (sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB;
    if (0.0 != sprite->unk5C) {
        REQ_INIT(1.0f, sprite->scaleY, 1.0f);
        req.mulColor = color;
        req.flags = 0x8000D;
        req.bmpId = 0xE33;
        req.x = sprite->x;
        req.y = sprite->y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
        REQ_INIT(sprite->scaleX * sprite->unk5C, sprite->scaleY, 1.0f);
        req.mulColor = color;
        req.flags = 0xD;
        req.bmpId = 0xE31;
        req.x = 8.0 + sprite->x;
        req.y = sprite->y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
        REQ_INIT(1.0f, sprite->scaleY, 1.0f);
        req.mulColor = color;
        req.flags = 0xD;
        req.bmpId = 0xE33;
        req.x = 8.0 + sprite->x + sprite->unk58 * sprite->unk5C;
        req.y = sprite->y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
    }
}

void fn_14_1C5D0(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    s32 color;

    color = 0xFF000000 | (sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB;
    if (0.0 != sprite->unk5C) {
        REQ_INIT(1.0f, sprite->scaleY, 1.0f);
        req.mulColor = color;
        req.flags = 0xF;
        req.bmpId = 0xE33;
        req.x = sprite->x;
        req.y = sprite->y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
        REQ_INIT(sprite->scaleX * sprite->unk5C, sprite->scaleY, 1.0f);
        req.mulColor = color;
        req.flags = 0xF;
        req.bmpId = 0xE31;
        req.x = sprite->x - 8.0;
        req.y = sprite->y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
        REQ_INIT(1.0f, sprite->scaleY, 1.0f);
        req.mulColor = color;
        req.flags = 0x8000F;
        req.bmpId = 0xE33;
        req.x = (sprite->x - 8.0) - sprite->unk58 * sprite->unk5C;
        req.y = sprite->y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
    }
}

void fn_14_1C824(u8 playerId)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 2;
        sprite->x = VP_X(playerId) + 0x10;
        sprite->y = VP_Y(playerId) + 0x2C;
        sprite->depth = 0.1f;
        sprite->textAlign = 0;
        sprite->drawFunc = fn_14_1C95C;
        sprite->userVar = playerId;
        sprintf(sprite->text, "lap time");
    }
}

void fn_14_1C95C(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float y;
    struct BoatLap *lap;
    s32 lapCount;
    struct BoatPlayer *pl;
    s16 i;
    s16 num;

    pl = &lbl_14_bss_208[sprite->userVar];
    textdraw_reset();
    textdraw_set_font(0x72);
    textdraw_set_flags(0x200000);
    lapCount = pl->unk7;
    num = lapCount - 5;
    if (num < 0) {
        num = 0;
    }
    lap = &pl->unk50[num];
    y = sprite->y;
    set_ui_widescreen_scale_mtx(sprite->x);
    for (i = num; i < (s16) lapCount; i++, lap++) {
        REQ_INIT(1.0f, 1.0f, 1.0f);
        if (i == (s16) lapCount - 1) {
            req.mulColor = 0xFF0060FF;
        } else {
            req.mulColor = 0xFFB4C8F0;
        }
        req.bmpId = 0xE25;
        req.x = sprite->x;
        req.y = y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
        textdraw_set_depth(sprite->depth - 0.001);
        num = i + 1;
        textdraw_set_pos(12.0 + sprite->x, 12.0 + y);
        textdraw_set_alignment(4);
        if (num <= 9) {
            textdraw_set_scale(1.0f, 1.0f);
        } else {
            textdraw_set_scale(0.7f, 1.0f);
        }
        textdraw_printf("%d", num);
        textdraw_set_alignment(0);
        textdraw_set_pos(27.0 + sprite->x, 4.0 + y);
        textdraw_set_scale(1.0f, 1.0f);
        textdraw_printf("%02d;%02d<%02d", lap->unk8.min, lap->unk8.sec, lap->unk8.cs);
        y += 26.0;
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_14_1CBBC(u8 playerId)
{
    float x;
    float y;
    struct Sprite *sprite;

    x = VP_X(playerId) + 0x10;
    y = VP_Y(playerId) + 0x2C;
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->bmpId = 0xE25;
        sprite->textAlign = 0;
        sprite->depth = 0.11f;
        sprite->mulR = 0;
        sprite->mulG = 0x60;
        sprite->mulB = 0xFF;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprintf(sprite->text, "lap time(now) base");
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 0x72;
        sprite->x = 12.0 + x;
        sprite->y = 12.0 + y;
        sprite->depth = 0.1f;
        sprite->textAlign = 4;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_1CE44;
        sprite->userVar = playerId;
        sprintf(sprite->text, "1");
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->fontId = 0x72;
        sprite->x = 26.0 + x;
        sprite->y = 12.0 + y;
        sprite->depth = 0.1f;
        sprite->textAlign = 1;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_1CEC0;
        sprite->userVar = playerId;
    }
}

void fn_14_1CE44(s8 *status, struct Sprite *sprite)
{
    struct BoatPlayer *pl;
    u8 lapCount;

    pl = &lbl_14_bss_208[sprite->userVar];
    lapCount = pl->unk7;
    if (lapCount >= 0xA) {
        sprite->scaleX = 0.7f;
    } else {
        sprite->scaleX = 1.0f;
    }
    sprintf(sprite->text, "%d", lapCount);
}

void fn_14_1CEC0(s8 *status, struct Sprite *sprite)
{
    struct BoatPlayer *pl;
    struct BoatLap *lap;

    pl = &lbl_14_bss_208[sprite->userVar];
    lap = &pl->unk50[pl->unk7 - 1];
    sprintf(sprite->text, "%02d;%02d<%02d", lap->unk8.min, lap->unk8.sec, lap->unk8.cs);
}

void fn_14_1CF24(u8 playerId)
{
    static u16 itemIds1[8] = {0xE00, 0xE01, 0xE02, 0xE03, 0, 0, 0, 0};
    static u16 itemIds2[8] = {0xE08, 0xE09, 0xE0A, 0xE0B, 0, 0, 0, 0};
    struct Sprite *sprite;
    s32 a;
    s32 w;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 2;
        a = VP_X(playerId);
        w = VP_W(playerId) - 0x10;
        sprite->x = a + w;
        a = VP_Y(playerId);
        w = VP_H(playerId) - 0x10;
        sprite->y = a + w;
        sprite->depth = 0.1f;
        sprite->scaleX = 1.5f;
        sprite->scaleY = 1.5f;
        sprite->textAlign = 8;
        sprite->drawFunc = fn_14_1D190;
        sprite->userVar = playerId;
        sprintf(sprite->text, "item");
        if (modeCtrl.unk30 == 1) {
            lbl_14_bss_6350[playerId] = itemIds1;
            sprite->unk58 = 32.0f;
        } else {
            lbl_14_bss_6350[playerId] = itemIds2;
            sprite->unk58 = 24.0f;
        }
        sprite->unk58 = (s32) (sprite->unk58 * sprite->scaleX);
        sprite->unk5C = 0.75 * sprite->unk58;
    }
}

void fn_14_1D190(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    struct BoatPlayer *pl;
    u16 *ids;
    u32 item;
    s16 i;

    pl = &lbl_14_bss_208[sprite->userVar];
    ids = lbl_14_bss_6350[sprite->userVar];
    item = pl->unk498[0];
    if (item != 0) {
        set_ui_widescreen_scale_mtx(sprite->x);
        REQ_INIT(sprite->scaleX, sprite->scaleY, 1.0f);
        req.flags = 0x20000F;
        x = sprite->x;
        y = sprite->y;
        req.bmpId = ids[fn_14_1D380(item)];
        req.x = x;
        req.y = y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
        y -= 2.0 + sprite->unk58;
        req.scaleX *= 0.75;
        req.scaleY *= 0.75;
        for (i = 1; i < 3; i++) {
            item = pl->unk498[i];
            if (item == 0) {
                break;
            }
            req.bmpId = ids[fn_14_1D380(item)];
            req.x = x;
            req.y = y;
            req.z = sprite->depth;
            draw_sprite_draw_request(&req);
            y -= 2.0 + sprite->unk5C;
        }
        reset_ui_widescreen_scale_mtx();
    }
}

u32 fn_14_1D380(u32 flag)
{
    u32 i;

    for (i = 0; i < 5; i++) {
        if (flag == (u32) (1 << i)) {
            return i;
        }
    }
    return 0;
}

void fn_14_stripped_reverse(struct Sprite *sprite) {
    sprintf(sprite->text, "REVERSE");
}

void fn_14_1D3B4(u8 playerId)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 1;
        sprite->bmpId = 0xE17;
        sprite->x = (s32) (VP_X(playerId) + (float) VP_W(playerId) / 2.0);
        sprite->y = (s32) (VP_Y(playerId) + (float) VP_H(playerId) / 2.0);
        sprite->depth = 0.1f;
        if (modeCtrl.unk30 == 1) {
            sprite->scaleX = sprite->scaleY = 1.0f;
        } else if (modeCtrl.unk30 == 2 || (modeCtrl.unk30 == 3 && playerId == modeCtrl.splitscreenMode)) {
            sprite->scaleX = sprite->scaleY = 0.8f;
        } else {
            sprite->scaleX = sprite->scaleY = 0.6f;
        }
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = sprite->x;
        sprite->mainFunc = fn_14_1D69C;
        sprite->counter = sprite->userVar = 0.5 * lbl_80198B34;
        sprintf(sprite->text, "GOAL!!");
    }
}

void fn_14_1D69C(s8 *status, struct Sprite *sprite)
{
    float t;
    s16 half;

    half = sprite->userVar / 2.0;
    if (sprite->counter != 0) {
        sprite->counter--;
        if (sprite->counter > half) {
            t = 1.0 - (float) (sprite->counter - half) / (float) half;
        } else {
            t = sprite->counter / (float) half;
        }
        t *= 255.0;
        sprite->addR = t;
        sprite->addG = t;
        sprite->addB = t;
    }
}

void fn_14_1D7EC(u8 playerId)
{
    float x;
    struct BoatPlayer *pl;
    struct Sprite *sprite;

    create_sprite();
    x = VP_X(playerId) + VP_W(playerId) / 2;
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        if (modeCtrl.unk30 == 1) {
            sprite->fontId = 9;
            sprite->x = x;
            sprite->y = ((VP_Y(playerId) + VP_H(playerId)) - 16.0) - 65.0;
            sprite->scaleX = 0.7f;
            sprite->scaleY = 0.7f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0xA0;
            sprite->mulB = 0x55;
        } else {
            sprite->fontId = 4;
            sprite->x = x;
            sprite->y = ((VP_Y(playerId) + VP_H(playerId)) - 32.0) - 42.0;
        }
        sprite->depth = 0.1f;
        sprite->textAlign = 4;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = x;
        sprite->mainFunc = fn_14_1DCFC;
        sprite->counter = 0x14;
        sprite->userVar = sprite->counter;
        sprite->unk58 = sprite->scaleY;
        sprintf(sprite->text, "RETIRE");
    }
    if (lbl_14_bss_20.unk2 & 0x10) {
        sprite = create_sprite();
        if (sprite != NULL) {
            pl = &lbl_14_bss_208[playerId];
            sprite->tag = playerId + 0x6C;
            sprite->type = 0;
            sprite->fontId = 4;
            sprite->x = x;
            if (modeCtrl.unk30 == 1) {
                sprite->y = ((VP_Y(playerId) + VP_H(playerId)) - 16.0) - 35.0;
            } else {
                sprite->y = (VP_Y(playerId) + VP_H(playerId)) - 32.0;
            }
            sprite->depth = 0.1f;
            sprite->scaleX = 1.0f;
            sprite->scaleY = 1.0f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0x40;
            sprite->textAlign = 4;
            sprite->flags = 0x01200000;
            sprite->widescreenArg = x;
            sprite->mainFunc = fn_14_1DD94;
            sprite->counter = 0x14;
            sprite->userVar = sprite->counter;
            sprite->unk58 = sprite->scaleY;
            sprite->unk50 = 0;
            if (pl->unk4 == 1) {
                sprintf(sprite->text, "TOTAL %d POINT", pl->unk5);
            } else {
                sprintf(sprite->text, "TOTAL %d POINTS", pl->unk5);
            }
        }
    }
}

void fn_14_1DCFC(s8 *status, struct Sprite *sprite)
{
    float t;

    t = (float) sprite->counter / (float) sprite->userVar;
    if (sprite->counter > 0) {
        sprite->counter--;
    }
    sprite->scaleY = sprite->unk58 * (1.0 - t);
}

void fn_14_1DD94(s8 *status, struct Sprite *sprite)
{
    switch (sprite->unk50) {
    case 0:
        sprite->scaleY = sprite->unk58 * (1.0 - (float) sprite->counter / (float) sprite->userVar);
        if (--sprite->counter == 0) {
            sprite->scaleY = sprite->unk58;
            if (modeCtrl.unk30 == 1 || lbl_14_bss_20.unk0 == 2) {
                sprite->counter = 1.5 * lbl_80198B34;
            } else {
                sprite->counter = 1;
            }
            sprite->unk50++;
        }
        break;
    case 1:
        if (--sprite->counter == 0) {
            sprite->unk50++;
        }
        break;
    case 2:
        if (fn_14_BE8() == 0xA && lbl_14_bss_10 <= lbl_80198B38 * 6) {
            sprite->counter = sprite->userVar;
            sprite->unk50++;
        }
        break;
    case 3:
        sprite->scaleY = sprite->unk58 * ((float) sprite->counter / (float) sprite->userVar);
        if (--sprite->counter == 0) {
            sprite->scaleY = 0.0f;
            sprite->unk50++;
        }
        break;
    }
}

void fn_14_1DFC8(u8 playerId)
{
    float x;
    float diff;
    struct BoatPlayer *pl;
    struct Camera *cam;
    struct Sprite *sprite;

    pl = &lbl_14_bss_208[playerId];
    cam = &cameras[playerId];
    x = VP_X(playerId) + (float) VP_W(playerId) / 2.0;
    {
    s16 rankIds[8] = {0xE18, 0xE19, 0xE1A, 0xE1B, 0xE1C, 0xE1D, 0xE1E, 0xE1F};
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 1;
        sprite->bmpId = rankIds[pl->unk2 - 1];
        sprite->x = x;
        sprite->y = (float) VP_Y(playerId) + (float) VP_H(playerId);
        sprite->depth = 0.1f;
        sprite->textAlign = 4;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = x;
        sprite->opacity = 0.0f;
        if (0.5 == cam->sub28.viewportW && 0.5 == cam->sub28.viewportH) {
            sprite->y -= 80.0;
            sprite->scaleX = sprite->scaleY = 0.65f;
        } else if (0.5 == cam->sub28.viewportW || 0.5 == cam->sub28.viewportH) {
            sprite->y -= 85.0;
            sprite->scaleX = sprite->scaleY = 0.8f;
        } else {
            if (lbl_14_bss_68 >= 0) {
                sprite->y -= 185.0;
            } else {
                sprite->y -= 125.0;
            }
            sprite->scaleX = sprite->scaleY = 1.0f;
        }
        sprite->mainFunc = fn_14_1E3A8;
        sprite->counter = 0.2 * lbl_80198B34;
        sprite->userVar = sprite->counter;
        sprite->unk50 = pl->unk2;
        sprite->unk58 = sprite->scaleX;
        sprite->scaleX *= 2.0;
        sprite->scaleY *= 2.0;
        diff = sprite->scaleX - sprite->unk58;
        sprite->unk5C = diff / (float) sprite->counter;
        sprintf(sprite->text, "%dP rank:%d", playerId + 1, pl->unk2);
    }
    }
}

void fn_14_1E3A8(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter != 0) {
        sprite->counter--;
        if (sprite->unk50 == 1) {
            sprite->rotation = sprite->counter * 0x880;
        }
        sprite->scaleX -= sprite->unk5C;
        sprite->scaleY -= sprite->unk5C;
        sprite->opacity = 1.0 - (float) sprite->counter / (float) sprite->userVar;
    }
}

void fn_14_1E474(u8 playerId)
{
    float x;
    float y;
    struct Sprite *sprite;
    struct BoatPlayer *pl;
    struct Camera *cam;

    pl = &lbl_14_bss_208[playerId];
    cam = &cameras[playerId];
    x = VP_X(playerId) + VP_W(playerId) / 2;
    y = (VP_Y(playerId) + VP_H(playerId)) - 34.0;
    if (1.0 == cam->sub28.viewportW && 1.0 == cam->sub28.viewportH) {
        y -= 20.0;
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 4;
        sprite->flags = 0x200000;
        sprite->x = x;
        sprite->y = y - 18.0;
        sprite->depth = 0.1f;
        sprite->textAlign = 5;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprite->opacity = 0.0f;
        sprite->mulR = 0x40;
        sprite->mulG = 0xFF;
        sprite->mulB = 0xFF;
        if (pl->unk4 == 1) {
            sprintf(sprite->text, "GET %d POINT!", pl->unk4);
        } else {
            sprintf(sprite->text, "GET %d POINTS!", pl->unk4);
        }
        sprite->mainFunc = fn_14_1E854;
        sprite->counter = 0.5 * lbl_80198B34;
        sprite->userVar = sprite->counter;
        sprite->unk58 = 30.0f;
        sprite->x += sprite->unk58;
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = playerId + 0x6C;
        sprite->type = 0;
        sprite->fontId = 4;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->textAlign = 5;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = x;
        sprite->opacity = 0.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0x40;
        if (pl->unk4 == 1) {
            sprintf(sprite->text, "TOTAL %d POINT", pl->unk5);
        } else {
            sprintf(sprite->text, "TOTAL %d POINTS", pl->unk5);
        }
        sprite->mainFunc = fn_14_1E854;
        sprite->counter = 0.5 * lbl_80198B34;
        sprite->userVar = sprite->counter;
        sprite->unk50 = 0;
        sprite->unk58 = 30.0f;
        sprite->x += sprite->unk58;
    }
}

void fn_14_1E854(s8 *status, struct Sprite *sprite)
{
    switch (sprite->unk50) {
    case 0:
        sprite->x -= sprite->unk58 / (float) sprite->userVar;
        sprite->opacity = 1.0 - (float) sprite->counter / (float) sprite->userVar;
        if (--sprite->counter == 0) {
            sprite->opacity = 1.0f;
            if (modeCtrl.unk30 == 1 || lbl_14_bss_20.unk0 == 2) {
                sprite->counter = 1.5 * lbl_80198B34;
            } else {
                sprite->counter = 1;
            }
            sprite->unk50++;
        }
        break;
    case 1:
        if (--sprite->counter == 0) {
            sprite->unk50++;
        }
        break;
    case 2:
        if (fn_14_BE8() == 0xA && lbl_14_bss_10 <= lbl_80198B38 * 6) {
            sprite->counter = sprite->userVar;
            sprite->unk50++;
        }
        break;
    case 3:
        sprite->x -= sprite->unk58 / (float) sprite->userVar;
        sprite->opacity = sprite->counter / (float) sprite->userVar;
        if (--sprite->counter == 0) {
            sprite->opacity = 0.0f;
            sprite->unk50++;
        }
        break;
    }
}

void fn_14_1EAE0(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0x4B;
        sprite->x = 0.0f;
        sprite->y = 0.0f;
        sprite->depth = 0.09f;
        sprite->scaleX = 80.0f;
        sprite->scaleY = 60.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->mainFunc = fn_14_1EB84;
        sprite->userVar = 0;
    }
}

void fn_14_1EB84(s8 *status, struct Sprite *sprite)
{
    sprite->opacity += 0.5 / lbl_80198B34;
    if (sprite->opacity > 0.5) {
        sprite->opacity = 0.5f;
        if (sprite->userVar == 0) {
            fn_14_1EC1C(0);
            sprite->userVar = -1;
        }
    }
}

void fn_14_1EC1C(s16 index)
{
    char buf[0x10];
    float x;
    struct Sprite *sprite;

    if (index < lbl_14_bss_24) {
        x = 171.0f;
        textdraw_reset();
        textdraw_set_font(4);
        sprintf(buf, "99 POINTS");
        x += textdraw_get_pixel_width_of_string_as_f32(buf);
        x = (s32) (320.0 - 0.5 * x);
        sprite = create_sprite();
        if (sprite != NULL) {
            sprite->type = 1;
            sprite->counter = 0.4 * lbl_80198B34;
            sprite->x = x + (float) sprite->counter;
            sprite->y = 228.0 + (float) (index * 0x1C);
            sprite->depth = 0.08f;
            sprite->textAlign = 0;
            sprite->flags = 0x01000000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = fn_14_1EDC0;
            sprite->drawFunc = fn_14_1F018;
            sprite->userVar = index;
            sprite->unk50 = sprite->counter;
        }
    }
}

void fn_14_1EDC0(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter > 0) {
        sprite->counter--;
        sprite->x -= 1.0f;
        if (sprite->counter == 0 && sprite->userVar + 1 < lbl_14_bss_24) {
            fn_14_1EC1C(sprite->userVar + 1);
        }
    }
    sprite->opacity = 1.0 - (float) sprite->counter / (float) sprite->unk50;
}

void fn_14_1F018(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    struct BoatPlayer *pl;
    struct Ball *ball;
    s32 c;
    u32 addColor;

    pl = lbl_14_bss_29C4[sprite->userVar];
    ball = &balls[pl->unk0];
    if (lbl_14_bss_24 > modeCtrl.unk30 && !(ball->flags & BALL_FLAG_BOAT_CPU)) {
        c = 60.0 * (0.5 * (1.0 + math_sin((g_font_char_var_3 << 11) + (ball->playerId << 12))));
        addColor = ((u8) c << 16) | ((u8) c << 8) | (u8) c;
    } else {
        addColor = 0;
    }
    textdraw_reset();
    textdraw_set_flags(0x200000);
    textdraw_set_alpha(sprite->opacity);
    textdraw_set_depth(sprite->depth);
    textdraw_set_add_color(addColor);
    REQ_INIT(1.0f, 1.0f, sprite->opacity);
    req.mulColor = 0xFF0060FF;
    req.addColor = addColor;
    req.bmpId = 0xE25;
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = sprite->depth;
    draw_sprite_draw_request(&req);
    textdraw_set_font(0x72);
    textdraw_set_alignment(4);
    textdraw_set_pos(12.0 + sprite->x, 12.0 + sprite->y);
    textdraw_set_mul_color(-1);
    textdraw_printf("%d", pl->unk6);
    textdraw_set_font(4);
    textdraw_set_alignment(0);
    textdraw_set_pos(27.0 + sprite->x, 4.0 + sprite->y);
    textdraw_set_mul_color(lbl_1_data_2E490[pl->unk0]);
    if (ball->flags & BALL_FLAG_BOAT_CPU) {
        textdraw_printf("COM%d", pl->unk0 + 1);
    } else {
        textdraw_printf("PLAYER%d", pl->unk0 + 1);
    }
    textdraw_set_pos(144.0 + (27.0 + sprite->x), 4.0 + sprite->y);
    if (pl->unk5 == 1) {
        textdraw_printf("%2d POINT", pl->unk5);
    } else {
        textdraw_printf("%2d POINTS", pl->unk5);
    }
}

void fn_14_1F358(float x, float y)
{
    struct Sprite *sprite;

    create_sprite();
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0xE20;
        sprite->textAlign = 6;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = VP_W(0) - 16.0;
        sprintf(sprite->text, "REPLAY");
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0xE21;
        sprite->textAlign = 0;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.1f;
        sprite->widescreenArg = VP_W(0) - 16.0;
        sprite->drawFunc = fn_14_1F4FC;
        sprintf(sprite->text, "REPLAY ARROW");
    }
}

void fn_14_1F4FC(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    s16 period;
    s16 count;
    s16 i;

    period = 20;
    count = 4;
    REQ_INIT(1.0f, 1.0f, 1.0f);
    req.flags |= 0x200000;
    set_ui_widescreen_scale_mtx(sprite->widescreenArg);
    for (i = 0; i < (u32) (g_font_char_var_3 % (period * count)) / period; i++) {
        req.bmpId = sprite->bmpId;
        req.x = sprite->x + 32.0 * i;
        req.y = sprite->y;
        req.z = sprite->depth;
        draw_sprite_draw_request(&req);
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_14_1F64C(void)
{
    char buf[0x10];
    float x;
    struct Sprite *sprite;

    x = 171.0f;
    textdraw_reset();
    if (lbl_14_bss_20.unk2 & 0x10) {
        textdraw_set_font(4);
        sprintf(buf, "99 POINTS");
    } else {
        textdraw_set_font(0x72);
        sprintf(buf, "99;99<99");
    }
    x += textdraw_get_pixel_width_of_string_as_f32(buf);
    x = (s32) (320.0 - 0.5 * x);
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 0;
        sprite->fontId = 2;
        sprite->x = x;
        sprite->y = 464.0 - (float) (28.0 * lbl_14_bss_24);
        sprite->depth = 0.1f;
        sprite->textAlign = 0;
        sprite->drawFunc = fn_14_1F7BC;
        sprintf(sprite->text, "RESULT");
    }
}

void fn_14_1F7BC(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x;
    float y;
    struct BoatPlayer **list;
    s16 i;
    struct BoatPlayer *pl;
    struct BoatLap *total;
    struct Ball *ball;
    s32 c;
    u32 addColor;

    x = sprite->x;
    y = sprite->y;
    if (lbl_14_bss_20.unk2 & 0x10) {
        list = lbl_14_bss_29C4;
    } else {
        list = lbl_14_bss_2988;
    }
    set_ui_widescreen_scale_mtx(0x140);
    for (i = 0; i < lbl_14_bss_24; i++) {
        pl = list[i];
        total = &pl->unk2A8;
        ball = &balls[pl->unk0];
        if (lbl_14_bss_24 > modeCtrl.unk30 && !(ball->flags & BALL_FLAG_BOAT_CPU)) {
            c = 60.0 * (0.5 * (1.0 + math_sin((g_font_char_var_3 << 11) + (i << 12))));
            addColor = ((u8) c << 16) | ((u8) c << 8) | (u8) c;
        } else {
            addColor = 0;
        }
        REQ_INIT(1.0f, 1.0f, 1.0f);
        req.mulColor = 0xFF0060FF;
        req.addColor = addColor;
        req.bmpId = 0xE25;
        req.x = x;
        req.y = y;
        req.z = 0.01 + sprite->depth;
        draw_sprite_draw_request(&req);
        textdraw_set_depth(sprite->depth);
        textdraw_set_flags(0x200000);
        textdraw_set_alignment(4);
        textdraw_set_add_color(addColor);
        textdraw_set_pos(12.0 + x, 12.0 + y);
        if (!(lbl_14_bss_20.unk2 & 0x10) && !(pl->unk8 & 8)) {
            textdraw_set_font(5);
            textdraw_printf("-");
        } else {
            textdraw_set_font(0x72);
            textdraw_printf("%d", lbl_14_bss_8C[pl->unk0] + 1);
        }
        textdraw_set_font(4);
        textdraw_set_alignment(0);
        textdraw_set_pos(27.0 + x, 4.0 + y);
        textdraw_set_mul_color(lbl_1_data_2E490[pl->unk0]);
        if (ball->flags & BALL_FLAG_BOAT_CPU) {
            textdraw_printf("COM%d", pl->unk0 + 1);
        } else {
            textdraw_printf("PLAYER%d", pl->unk0 + 1);
        }
        textdraw_set_pos(144.0 + (27.0 + x), 4.0 + y);
        if (lbl_14_bss_20.unk2 & 0x10) {
            textdraw_set_font(4);
            if (pl->unk5 == 1) {
                textdraw_printf("%2d POINT", pl->unk5);
            } else {
                textdraw_printf("%2d POINTS", pl->unk5);
            }
        } else if (pl->unk8 & 8) {
            textdraw_set_font(0x72);
            textdraw_printf("%02d;%02d<%02d", total->unk8.min, total->unk8.sec, total->unk8.cs);
        } else {
            textdraw_set_font(4);
            textdraw_printf("RETIRE");
        }
        textdraw_set_mul_color(-1);
        y += 28.0;
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_14_1FC2C(u8 playerId)
{
    float x;
    float y;
    struct Sprite *sprite;
    struct BoatLap *total;
    struct Camera *cam;

    total = &lbl_14_bss_208[playerId].unk2A8;
    cam = &cameras[playerId];
    x = VP_X(playerId) + VP_W(playerId) / 2;
    y = (VP_Y(playerId) + VP_H(playerId)) - 34.0;
    if (1.0 == cam->sub28.viewportW && 1.0 == cam->sub28.viewportH) {
        if (lbl_14_bss_68 >= 0) {
            y -= 90.0;
        } else {
            y -= 20.0;
        }
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 0;
        sprite->tag = playerId + 0x6C;
        sprite->fontId = 4;
        sprite->flags = 0x200000;
        sprite->x = x;
        sprite->y = y - 18.0;
        sprite->depth = 0.11f;
        sprite->textAlign = 5;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = x;
        sprintf(sprite->text, "TOTAL TIME");
        if (lbl_14_bss_20.unk2 & 0x10) {
            sprite->mainFunc = fn_14_20014;
            sprite->userVar = playerId;
            sprite->counter = lbl_80198B38 * 2;
            sprite->unk50 = 0.5 * lbl_80198B34;
            sprite->unk54 = sprite->unk50;
            sprite->unk58 = 30.0f;
        }
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 0;
        sprite->tag = playerId + 0x6C;
        sprite->fontId = 0x72;
        sprite->flags = 0x200000;
        sprite->x = x;
        sprite->y = y;
        sprite->depth = 0.11f;
        sprite->textAlign = 5;
        sprite->flags = 0x01200000;
        sprite->widescreenArg = x;
        sprintf(sprite->text, "%02d;%02d<%02d", total->unk8.min, total->unk8.sec, total->unk8.cs);
        if (lbl_14_bss_20.unk2 & 0x10) {
            sprite->mainFunc = fn_14_20014;
            sprite->userVar = playerId;
            sprite->counter = lbl_80198B38 * 2;
            sprite->unk50 = 0.5 * lbl_80198B34;
            sprite->unk54 = sprite->unk50;
            sprite->unk58 = 30.0f;
        }
    }
}

void fn_14_20014(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter > 0) {
        sprite->counter--;
        if (sprite->counter == 0) {
            fn_14_1E474(sprite->userVar);
        }
        return;
    }
    sprite->x -= sprite->unk58 / (float) sprite->unk54;
    sprite->opacity = sprite->unk50 / (float) sprite->unk54;
    if (sprite->unk50 > 0) {
        sprite->unk50--;
        if (sprite->unk50 == 0) {
            *status = 3;
        }
    }
}

void fn_14_20124(u8 playerId)
{
    static struct BoatMapInfo mapInfos[3] = {
        {0xE2E, -20, 65, 1.1636363f, 1.0f, 184.0f},
        {0xE2F, -90, 110, 0.58181816f, 1.0f, 160.0f},
        {0xE2C, -90, 110, 0.58181816f, 0.45f, 360.0f},
    };
    static Vec mapPos0[3] = {{0.0f, 8.0f, 1.0f}, {0.0f, 8.0f, 0.7f}, {0.0f, 8.0f, 0.6f}};
    static Vec mapPos1[3] = {{-10.0f, 40.0f, 1.3f}, {-10.0f, 40.0f, 1.0f}, {-10.0f, 30.0f, 0.8f}};
    static Vec mapPos2[3] = {{0.0f, -8.0f, 1.3f}, {0.0f, 0.0f, 0.8f}, {0.0f, 0.0f, 0.8f}};
    static Vec *mapPos[3] = {mapPos0, mapPos1, mapPos2};
    float tmp;
    float tmp2;
    s32 a;
    s32 h;
    struct Camera *cam;
    struct Sprite *sprite;
    struct BoatMapInfo *info;
    Vec *pos;

    cam = &cameras[playerId];
    sprite = create_sprite();
    if (sprite != NULL) {
        info = &mapInfos[lbl_14_bss_20.unk0];
        pos = mapPos[lbl_14_bss_20.unk0];
        sprite->type = 1;
        sprite->tag = playerId + 0x6C;
        sprite->bmpId = info->bmpId;
        sprite->textAlign = 2;
        if (0.5 == cam->sub28.viewportW && 0.5 == cam->sub28.viewportH) {
            pos += 2;
        } else if (0.5 == cam->sub28.viewportW || 0.5 == cam->sub28.viewportH) {
            pos += 1;
        }
        sprite->x = pos->x + (float) (VP_X(playerId) + 0x10);
        tmp2 = pos->y;
        a = VP_Y(playerId);
        h = VP_H(playerId) - 0x28;
        sprite->y = tmp2 + (float) (a + h);
        sprite->scaleX = sprite->scaleY = pos->z;
        if (modeCtrl.unk30 >= 2) {
            sprite->opacity = 0.5f;
        }
        sprite->depth = 0.1f;
        sprite->userVar = playerId;
        sprite->unk50 = info->x;
        sprite->unk54 = info->y;
        sprite->unk58 = info->scale;
        sprite->unk5C = info->vScale;
        sprite->unk60 = info->height;
        sprite->drawFunc = fn_14_2039C;
        sprintf(sprite->text, "%dP MAP", playerId + 1);
    }
}

void fn_14_2039C(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float originX;
    float originY;
    float scale;
    float vScale;
    float height;
    float frac;
    float vTop;
    float vBot;
    float mine;
    float other;
    float z;
    u8 playerId;
    struct Ball *ball;
    struct BoatPlayer *pl;
    s16 half;
    s16 i;
    struct BoatRgb *color;

    playerId = sprite->userVar;
    originX = sprite->unk50;
    originY = sprite->unk54;
    scale = sprite->unk58;
    vScale = sprite->unk5C;
    height = sprite->unk60;
    pl = &lbl_14_bss_208[playerId];
    half = (u32) lbl_14_bss_24 >> 1;
    if (modeCtrl.unk30 == 1) {
        mine = 1.0f;
        other = 0.8f;
    } else {
        mine = 0.8f;
        other = 0.6f;
    }
    if (lbl_14_bss_20.unk0 == 2) {
        vTop = 1.0 - vScale;
        if (pl->unk2DC > 58.0) {
            frac = (scale * -(pl->unk2E4.z - originY)) / height;
            if (frac > 0.5 * vScale) {
                vTop = (1.0 - frac) - 0.5 * vScale;
            }
        }
        if (vTop < 0.0f) {
            vTop = 0.0f;
        }
        vBot = vTop + vScale;
        if (vBot > 1.0) {
            vBot = 1.0f;
            vTop = 1.0 - vScale;
        }
    } else {
        vTop = 0.0f;
        vBot = 1.0f;
    }
    set_ui_widescreen_scale_mtx(sprite->x);
    REQ_INIT(sprite->scaleX, sprite->scaleY * vScale, sprite->opacity);
    req.flags = 0xD;
    req.v1 = vTop;
    req.v2 = vBot;
    req.bmpId = sprite->bmpId;
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = 0.2 + sprite->depth;
    draw_sprite_draw_request(&req);
    REQ_INIT(sprite->scaleX, sprite->scaleY, 0.9f);
    req.flags = 0xA;
    i = 0;
    ball = balls;
    pl = lbl_14_bss_208;
    while (i < lbl_14_bss_24) {
        float h;
        float v;

        color = &lbl_14_data_1CD8[ball->playerId];
        req.mulColor = 0xFF000000 | (color->r << 16) | (color->g << 8) | color->b;
        h = scale * (pl->unk2E4.x - originX);
        v = scale * (originY - pl->unk2E4.z);
        if (vTop < 1.0 - vScale) {
            v += height * (vTop - (1.0 - vScale));
        }
        if (v <= 0.0) {
            v = 0.0f;
            req.rotation = 0;
        } else if (v >= height * vScale) {
            v = height * vScale;
            req.rotation = 0x8000;
        } else {
            req.rotation = pl->unk3E2;
        }
        h = h * sprite->scaleX + (float) ((i - half) * 2);
        v = -v * sprite->scaleY + (float) ((i - half) * 2);
        if (ball->playerId == playerId) {
            z = sprite->depth;
            req.scaleX = req.scaleY = mine;
        } else {
            z = sprite->depth + 2.0 * (0.01 * i);
            req.scaleX = req.scaleY = other;
        }
        req.bmpId = 0xE30;
        req.x = sprite->x + h;
        req.y = sprite->y + v;
        req.z = z;
        draw_sprite_draw_request(&req);
        i++;
        ball++;
        pl++;
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_14_208E4(void)
{
    float x;
    struct Sprite *sprite;

    x = 0.5 * VP_W(0);
    if (lbl_14_bss_68 >= 0) {
        sprite = create_sprite();
        if (sprite != NULL) {
            sprite->tag = 0x6C;
            sprite->type = 0;
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->scaleX = 0.4f;
            sprite->scaleY = 0.7f;
            sprite->x = x;
            sprite->y = (VP_H(0) - 48.0) - 32.0 * sprite->scaleY;
            sprite->depth = 0.1f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0x80;
            sprite->mulB = 0x20;
            sprite->opacity = 0.0f;
            if (lbl_14_bss_68 == 0) {
                sprintf(sprite->text, "BEST RECORD\\");
                call_SoundReqID_arg_0(0x272);
            } else {
                sprintf(sprite->text, "NEW RECORD[");
                call_SoundReqID_arg_0(0x271);
            }
            sprite->flags = 0x01200000;
            sprite->widescreenArg = x;
            sprite->mainFunc = fn_14_20B1C;
            sprite->counter = 0.5 * lbl_80198B34;
            sprite->userVar = sprite->counter;
            sprite->unk58 = 2.0f;
            sprite->scaleX += sprite->unk58;
            sprite->scaleY += sprite->unk58;
        }
    }
}

void fn_14_20B1C(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter > 0) {
        sprite->scaleX -= sprite->unk58 / (float) sprite->userVar;
        sprite->scaleY -= sprite->unk58 / (float) sprite->userVar;
        sprite->opacity = 1.0 - (float) sprite->counter / (float) sprite->userVar;
        sprite->counter--;
    }
}

void fn_14_20C24(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0xE04;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->x = 624.0 - 32.0 * sprite->scaleX;
        sprite->y = 256.0 + 32.0 * sprite->scaleY;
        sprite->depth = 0.2f;
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = 0x270;
        sprintf(sprite->text, "LIVE BASE");
    }
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->type = 1;
        sprite->bmpId = 0xE11;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->x = 624.0 - 32.0 * sprite->scaleX;
        sprite->y = 256.0 + 32.0 * sprite->scaleY;
        sprite->depth = 0.1f;
        sprite->mulR = 0xB0;
        sprite->mulG = 0xFF;
        sprite->mulB = 0xC0;
        sprite->textAlign = 4;
        sprite->flags = 0x01000000;
        sprite->widescreenArg = 0x270;
        sprite->mainFunc = fn_14_20DB0;
        sprite->drawFunc = fn_14_20DC0;
        sprintf(sprite->text, "LIVE STAR");
    }
}

void fn_14_20DB0(s8 *status, struct Sprite *sprite)
{
    sprite->rotation += 0x300;
}

void fn_14_20DC0(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float rx;
    float ry;
    float fade;
    float t;
    s16 i;
    s16 angle;

    angle = g_font_char_var_3 << 9;
    for (i = 0; i < 0xD; i++) {
        angle += 0x400;
        if (angle < 0x1800 || angle > 0x5800) {
            if (angle > 0 && angle < 0x1800) {
                fade = (0x1800 - angle) / 6144.0f;
            } else if (angle > 0x5800 && angle < 0x7000) {
                fade = (angle - 0x5800) / 6144.0f;
            } else {
                fade = 1.0f;
            }
            t = (i + 1) / 13.0f;
            fade *= t;
            rx = sprite->scaleX * (32.0 * math_sin(angle + 0x4000));
            ry = sprite->scaleY * (6.0 * math_sin(angle));
            REQ_INIT(sprite->scaleX * t, sprite->scaleY * t, sprite->opacity * fade);
            req.flags = 0x20000A;
            req.rotation = sprite->rotation;
            req.mulColor = 0xFF000000 | (sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB;
            req.bmpId = sprite->bmpId;
            req.x = (sprite->x + rx * math_sin(-0x5800)) - ry * math_sin(0x6800);
            req.y = sprite->y + rx * math_sin(0x6800) + ry * math_sin(-0x5800);
            req.z = sprite->depth;
            draw_sprite_draw_request(&req);
        }
    }
}

