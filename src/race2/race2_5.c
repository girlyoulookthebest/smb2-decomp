#include "background.h"
#include "race2.h"
#include "item.h"
#include "libc.h"
#include "event.h"
#include "input.h"
#include "sprite.h"
#include "sound.h"
#include "mathutil.h"
#include "camera.h"
#include "mode.h"
#include "ball.h"
#include "stage.h"
#include "bitmap.h"
#include "textdraw.h"

struct Race2Marker
{
    Vec unk0;
};

extern s32 lbl_10_bss_1224;
extern s16 lbl_10_bss_121C;
extern s16 lbl_10_bss_2D50[8];
extern s32 lbl_10_bss_418C;
extern struct RacePlayer *lbl_10_bss_4238[8];
extern u32 lbl_1_data_2E490[];
extern s32 locale;
extern s32 race_frames_remaining;
extern s16 player_count;
void textdraw_printf(char *fmt, ...);
void textdraw_put_char(u8 c);
#ifdef __MWERKS__
int strlen(const char *);
#endif

extern struct Race2Marker lbl_10_bss_4260[8];
void fn_10_21114(void);
float textdraw_get_pixel_height_of_string(const char *s);
float textdraw_get_pixel_width_of_string_as_f32(const char *s);
void textdraw_set_alignment(u8 align);
void textdraw_reset(void);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_mul_color(u32 color);
void textdraw_set_add_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
void textdraw_set_border(void);
void textdraw_set_font_style(int style);
void textdraw_print(const char *s);
void fn_10_19420(float farg0);
void fn_10_197E8(s8 *arg0, struct Sprite *arg1);
void fn_10_1999C(struct Sprite *arg0);
void fn_10_19AB0(void);
void fn_10_19B30(s8 *arg0, struct Sprite *arg1);
void fn_10_19BB8(struct Sprite *arg0);
void fn_10_19DD0(void);
void fn_10_19E3C(void);
void fn_10_19F08(void);
void fn_10_19F88(s16 arg0, float arg1);
void fn_10_19FF0(void);
void fn_10_1A194(void);
void fn_10_1A28C(s16 arg0, float arg1, float arg2);
void fn_10_1A2EC(void);
void fn_10_1A3F4(float farg0, float farg1);
void fn_10_1A4DC(float farg0, float farg1);
void g_draw_race_timer_ui(void);
void g_print_race_time_remaining(s8 *arg0, struct Sprite *arg1);
void fn_10_1A850(s16 arg0);
void fn_10_1AA48(s8 *arg0, struct Sprite *arg1);
void fn_10_1AAC4(s16 arg0);
void fn_10_1AD4C(s8 *arg0, struct Sprite *arg1);
void fn_10_1ADD0(s8 *arg0, struct Sprite *arg1);
void fn_10_1AE54(s16 arg0);
void fn_10_1AF84(struct Sprite *arg0);
void fn_10_1B320(struct Sprite *arg0, struct RaceCheckpoint *arg1);
void fn_10_1B484(s16 arg0);
void fn_10_1B6F0(s8 *arg0, struct Sprite *arg1);
void fn_10_1B7EC(s16 arg0);
void fn_10_1BA74(s8 *arg0, struct Sprite *arg1);
void fn_10_1BB64(s16 arg0);
void fn_10_1BD28(struct Sprite *arg0);
void fn_10_1BF5C(struct Sprite *arg0);
void fn_10_1C1F8(s16 arg0);
void fn_10_1C490(s8 *arg0, struct Sprite *arg1);
void fn_10_1CA1C(s8 *arg0, struct Sprite *arg1);
void fn_10_1CA74(s16 arg0);
void fn_10_1CD18(s8 *arg0, struct Sprite *arg1);
void fn_10_1CD34(struct Sprite *arg0);
void fn_10_1CEA4(void);
void fn_10_1D090(s8 *arg0, struct Sprite *arg1);
void fn_10_1D2B0(struct Sprite *arg0);
void fn_10_1D9F0(s16 arg0);
void fn_10_1E18C(s8 *arg0, struct Sprite *arg1);
void fn_10_1E28C(s8 *arg0, struct Sprite *arg1);
void fn_10_1E398(s8 *arg0, struct Sprite *arg1);
void fn_10_1E3D4(s8 *arg0, struct Sprite *arg1);
void fn_10_1E46C(s8 *arg0, struct Sprite *arg1);
void fn_10_1E590(s8 *arg0, struct Sprite *arg1);
void fn_10_1E614(s16 arg0);
void fn_10_1EB08(s8 *arg0, struct Sprite *arg1);
void fn_10_1EB84(s8 *arg0, struct Sprite *arg1);
void fn_10_1EC4C(s8 *arg0, struct Sprite *arg1);
void fn_10_1ECC0(s16 arg0);
void fn_10_1EF18(s8 *arg0, struct Sprite *arg1);
void fn_10_1EFA0(void);
void fn_10_1F040(void);
void fn_10_1F0E4(s8 *arg0, struct Sprite *arg1);
void fn_10_1F19C(void);
void fn_10_1F3F0(s8 *arg0, struct Sprite *arg1);
void fn_10_1F424(struct Sprite *arg0);
void fn_10_1F7DC(s16 arg0);
void fn_10_1FAAC(s8 *arg0, struct Sprite *arg1);
void fn_10_1FB70(float farg0, float farg1);
void fn_10_1FC5C(struct Sprite *arg0);
void fn_10_1FDA0(u8 arg0);
void fn_10_1FFE4(struct Sprite *arg0);
void fn_10_20334(void);
void fn_10_2051C(s8 *arg0, struct Sprite *arg1);
void fn_10_205F4(s8 *arg0, struct Sprite *arg1);
void fn_10_2078C(u8 arg0);
void fn_10_20CA0(void);
void fn_10_20E2C(s8 *arg0, struct Sprite *arg1);
void fn_10_20E3C(struct Sprite *arg0);

/* unreferenced */
u8 lbl_10_data_1748[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0xFE, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x00, 0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0F, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0D, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

GXColor lbl_10_data_18B8[10] = {
    { 255, 0, 0, 255 },
    { 0, 76, 255, 255 },
    { 255, 229, 0, 255 },
    { 0, 204, 0, 255 },
    { 0, 204, 204, 255 },
    { 255, 0, 204, 255 },
    { 51, 102, 102, 255 },
    { 255, 255, 229, 255 },
    { 178, 0, 255, 255 },
    { 178, 255, 0, 255 },
};

s16 lbl_10_data_18E0[8] = { 0x704, 0x705, 0x706, 0x707, 0x708, 0x709, 0x70A, 0x70B };
s16 lbl_10_data_18F0[8] = { 0x70C, 0x70D, 0x70E, 0x70F, 0x710, 0x711, 0x712, 0x713 };

void (*lbl_10_data_1900[4])(void) = { fn_10_19E3C, fn_10_19F08, fn_10_19FF0, fn_10_1A194 };

char *lbl_10_data_1928[6] = {
    "] REPLAY ]",
    "] REPLAY ]",
    "] REPLAY ]",
    "] REPLAY ]",
    "] REPLAY ]",
    "-リプレイ-",
};

char *lbl_10_data_194C[6] = {
    "ROUND %d",
    "ROUND %d",
    "ROUND %d",
    "ROUND %d",
    "ROUND %d",
    "ROUND %d",
};

char *lbl_10_data_1970[6] = {
    "FINAL ROUND",
    "FINAL ROUND",
    "FINAL ROUND",
    "FINAL ROUND",
    "FINAL ROUND",
    "FINAL ROUND",
};

void fn_10_19420(float farg0)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct Race2CourseInfo *info = &lbl_10_data_2ADC[course->unk0];
    float x = 320.0f;
    float y = 240.0f;
    float offset;
    struct Sprite *sprite;

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
        sprite->mainFunc = fn_10_197E8;
        sprite->drawFunc = fn_10_1999C;
        sprite->counter = farg0 * lbl_80198B34;
        sprite->userVar = farg0 * lbl_80198B34;
        sprintf(sprite->text, "%s", info->unk0[locale]);
        if ((course->unk2 & 0x48) != 0) {
            textdraw_reset();
            textdraw_set_font(sprite->fontId);
            offset = 24.0 + sprite->unk5C * (0.5 * textdraw_get_pixel_height_of_string(info->unk0[locale]));
            sprite = create_sprite();
            if (sprite == NULL) {
                return;
            }
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
            sprite->mainFunc = fn_10_197E8;
            sprite->drawFunc = fn_10_1999C;
            sprite->counter = farg0 * lbl_80198B34;
            sprite->userVar = farg0 * lbl_80198B34;
            sprite->unk58 = 0.5f;
            sprite->unk5C = 0.5f;
            if (course->unk2 & 0x40) {
                if (locale == 5) {
                    sprite->fontId = 3;
                    sprite->unk58 = 1.0f;
                    sprite->unk5C = 1.0f;
                }
                sprite->y = y + offset;
                sprintf(sprite->text, "%s", lbl_10_data_1928[locale]);
            } else {
                sprite->y = y - offset;
                if (course->unk6 >= 5) {
                    sprintf(sprite->text, lbl_10_data_1970[locale]);
                } else {
                    sprintf(sprite->text, lbl_10_data_194C[locale], course->unk6 + 1);
                }
            }
        }
    }
}

void fn_10_197E8(s8 *arg0, struct Sprite *arg1)
{
    s32 tr3;

    tr3 = arg1->userVar - arg1->counter;
    if (tr3 < 0x1E) {
        arg1->opacity = (0.03333 * tr3);
        arg1->scaleX = math_sin((s16) (tr3 * 0x222));
        arg1->scaleY = 1.0f;
    } else if (tr3 < 0x3C) {
        arg1->opacity = 1.0f;
        arg1->scaleX = (1.0 - (0.2 * math_sin((s16) ((tr3 - 0x1E) * 0x444))));
        arg1->scaleY = 1.0f;
    } else if (arg1->counter <= 0xF) {
        arg1->opacity = (0.06666 * arg1->counter);
        arg1->scaleX = 1.0f;
        arg1->scaleY = (0.06666 * arg1->counter);
    } else {
        arg1->opacity = 1.0f;
        arg1->scaleX = 1.0f;
        arg1->scaleY = 1.0f;
    }
    arg1->scaleX *= arg1->unk58;
    arg1->scaleY *= arg1->unk5C;
    if (--arg1->counter <= 0) {
        *arg0 = 0;
    }
}

void fn_10_1999C(struct Sprite *arg0)
{
    float f1;

    textdraw_reset();
    textdraw_set_font(arg0->fontId);
    textdraw_set_alignment(*(u8 *) &arg0->textAlign);
    textdraw_set_pos(arg0->x, arg0->y);
    textdraw_set_depth(arg0->depth);
    textdraw_set_alpha(arg0->opacity);
    textdraw_set_mul_color(arg0->mulB | ((arg0->mulR << 0x10) | (arg0->mulG << 8)));
    f1 = (550.0 / (arg0->unk58 * textdraw_get_pixel_width_of_string_as_f32(arg0->text)));
    if (f1 > 1.0) {
        f1 = 1.0f;
    }
    f1 = 0.01 * (float) (s32) (100.0 * f1);
    textdraw_set_scale(arg0->scaleX * f1, arg0->scaleY);
    textdraw_set_border();
    textdraw_set_font_style(2);
    textdraw_print(arg0->text);
}

void fn_10_19AB0(void)
{
    struct Sprite *tr3;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->x = 320.0f;
        tr3->y = 240.0f;
        tr3->scaleX = 1.0f;
        tr3->scaleY = 1.0f;
        tr3->depth = 0.05f;
        tr3->textAlign = 4;
        tr3->mainFunc = fn_10_19B30;
        tr3->drawFunc = fn_10_19BB8;
    }
}

void fn_10_19B30(s8 *arg0, struct Sprite *arg1)
{
    if ((s32) (lbl_80198B38 - 1) == (lbl_10_bss_1224 % lbl_80198B38)) {
        call_SoundReqID_arg_0(0xDE);
    }
    if (lbl_10_bss_1224 == 0) {
        *arg0 = 0;
    }
}

void fn_10_19BB8(struct Sprite *arg0)
{
    float f31;
    float f30;
    float f29;
    float f2;
    s16 tr0;
    s16 tr30;

    set_ui_widescreen_scale_mtx((s32) arg0->x);
    textdraw_reset();
    textdraw_set_font(9);
    textdraw_set_depth(arg0->depth);
    textdraw_set_flags(0x200000);
    textdraw_set_alignment(4);
    tr30 = (lbl_10_bss_1224 / (s32) lbl_80198B38) + 1;
    tr0 = lbl_10_bss_1224 % lbl_80198B38;
    if (tr0 >= 0x1E) {
        f2 = (tr0 - 0x1E) * (1.0f / lbl_80198B34);
        f31 = 1.0f + (1.5f * f2);
        f30 = f31;
        f29 = 1.0f - f2;
    } else if (tr0 >= 0x14) {
        f31 = 1.0f;
        f30 = 1.0f;
        f29 = 1.0f;
    } else {
        f2 = 0.05f * (float) tr0;
        if (f2 > 0.0f) {
            f31 = 1.0f / f2;
            if (f31 > 4.0f) {
                f31 = 4.0f;
            }
        } else {
            f2 = 0.0f;
        }
        f30 = f2;
        f29 = f2;
    }
    textdraw_set_pos(arg0->x - (36.0f * f31), arg0->y - (32.0f * f30));
    textdraw_set_scale(f31, f30);
    textdraw_set_alpha(f29);
    textdraw_put_char((u8) (tr30 + 0x30));
    reset_ui_widescreen_scale_mtx();
}

void fn_10_19DD0(void)
{
    void (*tr3)(void);

    if ((modeCtrl.unk30 >= 1) && (modeCtrl.unk30 <= 4)) {
        lbl_10_data_1900[modeCtrl.unk30 - 1]();
    }
}

void fn_10_19E3C(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;

    if ((course->unk2 & 0x20) == 0) {
        g_draw_race_timer_ui();
    }
    fn_10_1FDA0(0);
    fn_10_1A850(0);
    fn_10_1AE54(0);
    fn_10_1B484(0);
    fn_10_1BB64(0);
    if ((course->unk2 & 0x10) != 0) {
        fn_10_1EFA0();
    } else {
        fn_10_1B7EC(0);
    }
    if ((course->unk2 & 0x40) != 0) {
        fn_10_1FB70(470.0f, 426.0f);
        return;
    }
    fn_10_1C1F8(0);
    fn_10_1F7DC(0);
}

void fn_10_19F08(void)
{
    s16 r31;

    fn_10_1A2EC();
    for (r31 = 0; r31 <= 1; r31++) {
        fn_10_19F88(r31, 240.0f * r31);
    }
}

void fn_10_19F88(s16 arg0, float arg1)
{
    fn_10_1A850(arg0);
    fn_10_1AAC4(arg0);
    fn_10_1B7EC(arg0);
    fn_10_1B484(arg0);
    fn_10_1BB64(arg0);
    fn_10_1C1F8(arg0);
    fn_10_1FDA0(arg0);
    fn_10_1F7DC(arg0);
}

void fn_10_19FF0(void)
{
    if (modeCtrl.splitscreenMode == 3) {
        fn_10_1A194();
    } else {
    fn_10_1A2EC();
    switch (modeCtrl.splitscreenMode) {       /* irregular */
    case 0:
        fn_10_1A850(0);
        fn_10_1AAC4(0);
        fn_10_1B7EC(0);
        fn_10_1B484(0);
        fn_10_1BB64(0);
        fn_10_1C1F8(0);
        fn_10_1FDA0(0);
        fn_10_1F7DC(0);
        fn_10_1A28C(1, 0.0f, 240.0f);
        fn_10_1A28C(2, 320.0f, 240.0f);
        break;
    case 1:
        fn_10_1A850(1);
        fn_10_1AAC4(1);
        fn_10_1B7EC(1);
        fn_10_1B484(1);
        fn_10_1BB64(1);
        fn_10_1C1F8(1);
        fn_10_1FDA0(1);
        fn_10_1F7DC(1);
        fn_10_1A28C(0, 0.0f, 240.0f);
        fn_10_1A28C(2, 320.0f, 240.0f);
        break;
    case 2:
        fn_10_1A28C(0, 0.0f, 0.0f);
        fn_10_1A28C(1, 320.0f, 0.0f);
        fn_10_1A850(2);
        fn_10_1AAC4(2);
        fn_10_1B7EC(2);
        fn_10_1B484(2);
        fn_10_1BB64(2);
        fn_10_1C1F8(2);
        fn_10_1FDA0(2);
        fn_10_1F7DC(2);
        break;
    }
    }
}

void fn_10_1A194(void)
{
    s16 r31;
    s16 r30;
    s16 pid;

    fn_10_1A2EC();
    for (r30 = 0; r30 <= 1; r30++) {
        for (r31 = 0; r31 <= 1; r31++) {
            pid = r31 + (r30 * 2);
            if (pid >= modeCtrl.unk30) {
                break;
            }
            fn_10_1A28C(pid, 320.0f * r31, 240.0f * r30);
        }
    }
}

void fn_10_1A28C(s16 arg0, float arg1, float arg2)
{
    fn_10_1A850(arg0);
    fn_10_1B7EC(arg0);
    fn_10_1BB64(arg0);
    fn_10_1B484(arg0);
    fn_10_1C1F8(arg0);
    fn_10_1FDA0(arg0);
    fn_10_1F7DC(arg0);
}

void fn_10_1A2EC(void)
{
    switch (modeCtrl.unk30) {                 /* switch 1; irregular */
    case 2:                                         /* switch 1 */
        if (modeCtrl.wideMode != 0) {
            fn_10_1A3F4(0.0f, 1.0f);
            return;
        }
        fn_10_1A4DC(0.0f, 1.0f);
        return;
    case 3:                                         /* switch 1 */
        fn_10_1A4DC(0.0f, 1.0f);
        switch (modeCtrl.splitscreenMode) {   /* switch 2; irregular */
        default:                                    /* switch 2 */
            fn_10_1A3F4(0.5f, 1.0f);
            return;
        case 2:                                     /* switch 2 */
            fn_10_1A3F4(0.0f, 0.5f);
            return;
        case 3:                                     /* switch 2 */
            fn_10_1A3F4(0.0f, 1.0f);
            return;
        }
        break;
    case 4:                                         /* switch 1 */
        fn_10_1A3F4(0.0f, 1.0f);
        fn_10_1A4DC(0.0f, 1.0f);
        break;
    }
}

void fn_10_1A3F4(float farg0, float farg1)
{
    struct Sprite *tr3;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->bmpId = 0x4C;
        tr3->depth = 0.5f;
        tr3->x = 320.0f;
        tr3->y = (240.0 * (farg0 + farg1));
        tr3->scaleX = 1.0f;
        tr3->scaleY = (60.0 * (farg1 - farg0));
        tr3->textAlign = 4;
        tr3->flags = 0x01200000;
        tr3->widescreenArg = (s16) tr3->x;
        sprintf(tr3->text, "SEP TATE");
    }
}

void fn_10_1A4DC(float farg0, float farg1)
{
    struct Sprite *tr3;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->bmpId = 0x4C;
        tr3->depth = 0.4f;
        tr3->x = (320.0 * (farg0 + farg1));
        tr3->y = 240.0f;
        tr3->rotation = 0x4000;
        tr3->scaleX = 1.0f;
        tr3->scaleY = (80.0 * (farg1 - farg0));
        tr3->textAlign = 4;
        tr3->flags = 0x200000;
        sprintf(tr3->text, "SEP YOKO");
    }
}

void g_draw_race_timer_ui(void)
{
    float tf31;
    struct Sprite *tr3;
    struct Sprite *tr3_2;

    tf31 = (((s32) (640.0f * cameras->sub28.viewportX) + (s32) (640.0f * cameras->sub28.viewportW)) - 16.0);
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = 0x6C;
        tr3->type = 1;
        tr3->bmpId = 0x725;
        tr3->x = tf31;
        tr3->y = (16.0 + (s32) (480.0f * cameras->sub28.viewportY));
        tr3->depth = 0.15f;
        tr3->textAlign = 6;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tf31;
        sprintf(tr3->text, "boa_time_waku");
    }
    tr3_2 = create_sprite();
    if (tr3_2 != NULL) {
        tr3_2->tag = 0x6C;
        tr3_2->type = 0;
        tr3_2->x = (tf31 - 4.0);
        tr3_2->y = (5.0 + (16.0 + (s32) (480.0f * cameras->sub28.viewportY)));
        tr3_2->fontId = 0x4A;
        tr3_2->textAlign = 6;
        tr3_2->depth = 0.11f;
        tr3_2->mulR = 0x5C;
        tr3_2->mulG = 0x3B;
        tr3_2->mulB = 0x1B;
        tr3_2->flags = 0x01200000;
        tr3_2->widescreenArg = (s16) tf31;
        tr3_2->mainFunc = g_print_race_time_remaining;
        sprintf(tr3_2->text, "000");
    }
}

void g_print_race_time_remaining(s8 *arg0, struct Sprite *arg1)
{
    sprintf(arg1->text, "%03d", race_frames_remaining / 60);
}

char lbl_10_data_19C0[6] = ":%02d";

void fn_10_1A850(s16 arg0)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    float tf31;
    struct Sprite *tr3;

    tf31 = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + 0x10);
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 4;
        tr3->x = tf31;
        tr3->y = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + 0x18);
        tr3->depth = 0.1f;
        tr3->textAlign = 0;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tf31;
        sprintf(tr3->text, "LAP");
        tr3 = create_linked_sprite(tr3);
        if (tr3 != NULL) {
            tr3->tag = arg0 + 0x6C;
            tr3->fontId = 0x49;
            tr3->textAlign = 2;
            if (course->unk4 < 0xA) {
                tr3->x = -12.0f;
            } else {
                tr3->x = 3.0f;
            }
            tr3->y = 1.0f;
            tr3->depth = 0.1f;
            tr3->flags = 0x01000000;
            tr3->widescreenArg = (s16) tf31;
            tr3->mainFunc = fn_10_1AA48;
            tr3->userVar = arg0;
        }
    }
}

void fn_10_1AA48(s8 *arg0, struct Sprite *arg1)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    s16 r5;

    r5 = ((struct RacePlayer *)balls[arg1->userVar].unk150)->unk44 + 1;
    if (r5 >= course->unk4) {
        r5 = course->unk4;
    }
    sprintf(arg1->text, "%2d:%d", r5, course->unk4);
}

void fn_10_1AAC4(s16 arg0)
{
    float tf30;
    float tf31;
    struct Sprite *tr3;

    tf31 = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + 0x10);
    tf30 = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + 0x2C);
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 1;
        tr3->bmpId = 0x718;
        tr3->x = tf31;
        tr3->y = tf30;
        tr3->depth = 0.11f;
        tr3->textAlign = 0;
        tr3->mulR = 0xF3;
        tr3->mulG = 0x7A;
        tr3->mulB = 0x1D;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tf31;
        sprintf(tr3->text, "lap time(now) base");
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 0x4F;
        tr3->x = (12.0 + tf31);
        tr3->y = (12.0 + tf30);
        tr3->depth = 0.1f;
        tr3->textAlign = 4;
        tr3->mainFunc = fn_10_1AD4C;
        tr3->userVar = arg0;
        tr3->flags = 0x01200000;
        tr3->widescreenArg = (s16) tf31;
        sprintf(tr3->text, "1");
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->fontId = 0x4F;
        tr3->x = (26.0 + tf31);
        tr3->y = (12.0 + tf30);
        tr3->depth = 0.1f;
        tr3->textAlign = 1;
        tr3->flags = 0x01200000;
        tr3->widescreenArg = (s16) tf31;
        tr3->mainFunc = fn_10_1ADD0;
        tr3->userVar = arg0;
    }
}

void fn_10_1AD4C(s8 *arg0, struct Sprite *arg1)
{
    u8 tr0;

    tr0 = ((struct RacePlayer *)balls[arg1->userVar].unk150)->unk44 + 1;
    if (tr0 >= 0xA) {
        arg1->scaleX = 0.7f;
    } else {
        arg1->scaleX = 1.0f;
    }
    sprintf(arg1->text, "%d", tr0);
}

void fn_10_1ADD0(s8 *arg0, struct Sprite *arg1)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct RacePlayer *tr6;
    struct RaceCheckpoint *cp;

    tr6 = (struct RacePlayer *)balls[arg1->userVar].unk150;
    if (tr6->unk44 >= course->unk4) {
        cp = &tr6->unk48[course->unk4 - 1];
    } else {
        cp = &tr6->unk48[tr6->unk44];
    }
    fn_10_1B320(arg1, cp);
}

void fn_10_1AE54(s16 arg0)
{
    struct Sprite *tr3;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 1;
        tr3->fontId = 4;
        tr3->x = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + 0x10);
        tr3->y = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + 0x2C);
        tr3->depth = 0.2f;
        tr3->drawFunc = fn_10_1AF84;
        tr3->userVar = arg0;
        sprintf(tr3->text, "lap time");
    }
}

void fn_10_1AF84(struct Sprite *arg0)
{
    struct RacePlayer *tr30;
    s16 r29;
    struct Race2Course *course = &lbl_10_bss_1208;
    struct SpriteDrawRequest req;
    float f31;
    s16 r28;
    s16 tr0;
    s16 r4;
    struct RaceCheckpoint *tr3;
    s32 t4;
    s32 t5;
    s32 t7;

    tr30 = (struct RacePlayer *)balls[arg0->userVar].unk150;
    set_ui_widescreen_scale_mtx((s32) arg0->x);
    textdraw_reset();
    textdraw_set_font(0x4F);
    textdraw_set_flags(0x200000);
    if (tr30->unk44 >= course->unk4) {
        r4 = tr30->unk44 - 5;
        r29 = course->unk4 - 1;
    } else {
        r4 = tr30->unk44 - 4;
        r29 = tr30->unk44;
    }
    if (r4 < 0) {
        r4 = 0;
    }
    f31 = arg0->y;
    for (r28 = r4; r28 <= r29; r28++) {
        req.scaleX = 1.0f;
        req.scaleY = 1.0f;
        req.u1 = req.v1 = 0.0f;
        req.u2 = req.v2 = 1.0f;
        req.rotation = 0;
        req.unk30 = -1;
        req.flags = 5;
        req.opacity = 1.0f;
        req.mulColor = 0xFFFFFF;
        req.addColor = 0;
        if (r28 == r29) {
            req.mulColor = 0xFFF37A1D;
        } else {
            req.mulColor = 0xFFF3F35A;
        }
        req.bmpId = 0x718;
        req.x = arg0->x;
        req.y = f31;
        req.z = arg0->depth;
        draw_sprite_draw_request(&req);
        textdraw_set_depth(arg0->depth - 0.01f);
        tr0 = r28 + 1;
        textdraw_set_pos((float) (12.0 + arg0->x), (float) (12.0 + f31));
        textdraw_set_alignment(4);
        if (tr0 <= 9) {
            textdraw_set_scale(1.0f, 1.0f);
        } else {
            textdraw_set_scale(0.7f, 1.0f);
        }
        textdraw_printf("%d", tr0);
        textdraw_set_alignment(0);
        textdraw_set_pos(27.0f + arg0->x, 4.0f + f31);
        textdraw_set_scale(1.0f, 1.0f);
        tr3 = &tr30->unk48[r28];
        t5 = tr3->unk0 / 60;
        t4 = t5 / 60;
        t5 %= 60;
        t7 = (1000.0f * ((float) (tr3->unk0 % 60) + tr3->unk4)) / 60.0f;
        t5 += t7 / 1000;
        t7 %= 1000;
        textdraw_printf("%02d;%02d<%02d", t4, t5, t7 / 10);
        f31 += 26.0f;
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_10_1B320(struct Sprite *arg0, struct RaceCheckpoint *arg1)
{
    s32 min;
    s32 sec;
    s32 ms;

    sec = arg1->unk0 / 60;
    min = sec / 60;
    sec %= 60;
    ms = 1000.0f * ((float) (arg1->unk0 % 60) + arg1->unk4) / 60.0f;
    sec += ms / 1000;
    ms %= 1000;
    sprintf(arg0->text, "%02d;%02d<%02d", min, sec, ms / 10);
}

void fn_10_1B484(s16 arg0)
{
    float f30;
    double tf1;
    struct Sprite *tr3;
    s32 t1;
    s32 t2;
    float tf31;
    struct Camera *cam;

    cam = &cameras[arg0];
    t1 = (s32) (640.0f * cameras[arg0].sub28.viewportX); t2 = (s32) (640.0f * cameras[arg0].sub28.viewportW) - 0x10; tf31 = t1 + t2;
    f30 = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + 0x39);
    if ((1.0 == cam->sub28.viewportW) && (1.0 == cam->sub28.viewportH)) {
        f30 += 60.0;
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 1;
        tr3->bmpId = 0x714;
        tr3->x = (4.0 + tf31);
        tr3->y = f30;
        tr3->depth = 0.1f;
        tr3->textAlign = 6;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tf31;
        sprintf(tr3->text, "km/h or mph");
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 0x4F;
        tf1 = 4.0 + tf31;
        tr3->x = (tf1 - 34.0);
        tr3->y = f30;
        tr3->depth = 0.1f;
        tr3->textAlign = 6;
        tr3->flags = 0x01200000;
        tr3->widescreenArg = (s16) tf31;
        tr3->mainFunc = fn_10_1B6F0;
        tr3->userVar = arg0;
        sprintf(tr3->text, "speed");
    }
}

void fn_10_1B6F0(s8 *arg0, struct Sprite *arg1)
{
    Vec sp8;
    s32 r5;
    struct Ball *tr4;

    tr4 = &balls[arg1->userVar];
    PSVECSubtract(&tr4->pos, &tr4->prevPos, &sp8);
    if (locale == 0) {
        r5 = 134.21985f * mathutil_vec_len(&sp8);
    } else {
        r5 = 216.0f * mathutil_vec_len(&sp8);
    }
    if (r5 >= 0x3E8) {
        r5 = 0x3E7;
    }
    sprintf(arg1->text, "%3d", r5);
}

void fn_10_1B7EC(s16 arg0)
{
    s32 t1;
    s32 t2;
    float tf31;
    float f30;
    struct Camera *tr5;
    struct Sprite *tr3;

    tr5 = &cameras[arg0];
    t1 = (s32) (640.0f * cameras[arg0].sub28.viewportX); t2 = (s32) (640.0f * cameras[arg0].sub28.viewportW) - 0x10; tf31 = t1 + t2;
    f30 = (16.0 + (s32) (480.0f * cameras[arg0].sub28.viewportY));
    if ((1.0 == tr5->sub28.viewportW) && (1.0 == tr5->sub28.viewportH)) {
        f30 += 45.0;
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        u8 sp8[4] = { 2, 1, 3, 0 };

        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 0x46;
        tr3->x = tf31 - 70.0f;
        tr3->y = f30;
        tr3->depth = 0.1f;
        tr3->textAlign = 6;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tf31;
        tr3->userVar = arg0;
        sprintf(tr3->text, "%c", 0x31 + arg0 + (sp8[arg0] * 4));
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 0x47;
        tr3->x = tf31;
        tr3->y = f30;
        tr3->depth = 0.1f;
        tr3->textAlign = 6;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tf31;
        tr3->mainFunc = fn_10_1BA74;
        tr3->userVar = arg0;
        sprintf(tr3->text, "1");
    }
}

void fn_10_1BA74(s8 *arg0, struct Sprite *arg1)
{
    struct RacePlayer *tr7 = (struct RacePlayer *)balls[arg1->userVar].unk150;
    Vec colors[8] = {
        { 255.0f, 62.0f, 25.0f },
        { 230.0f, 152.0f, 44.0f },
        { 237.0f, 199.0f, 39.0f },
        { 232.0f, 236.0f, 24.0f },
        { 161.0f, 236.0f, 19.0f },
        { 19.0f, 236.0f, 36.0f },
        { 24.0f, 230.0f, 201.0f },
        { 40.0f, 113.0f, 216.0f },
    };
    u16 r5;
    Vec *color;

    r5 = tr7->unk3E;
    if (r5 == 0) {
        r5 = player_count - tr7->unk0;
    }
    color = &colors[r5 - 1];
    arg1->mulR = color->x;
    arg1->mulG = color->y;
    arg1->mulB = color->z;
    sprintf(arg1->text, "%d", r5);
}

void fn_10_1BB64(s16 arg0)
{
    struct Sprite *tr3;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 1;
        tr3->textAlign = 0;
        { s32 t1 = (s32) (640.0f * cameras[arg0].sub28.viewportX); s32 t2 = (s32) (640.0f * cameras[arg0].sub28.viewportW) - 0x10; tr3->x = t1 + t2;}
        { s32 t1 = (s32) (480.0f * cameras[arg0].sub28.viewportY); s32 t2 = (s32) (480.0f * cameras[arg0].sub28.viewportH) - 0x10; tr3->y = t1 + t2; }
        tr3->depth = 0.1f;
        tr3->textAlign = 8;
        if (modeCtrl.unk30 == 1) {
            tr3->drawFunc = fn_10_1BF5C;
        } else {
            tr3->drawFunc = fn_10_1BD28;
        }
        tr3->userVar = arg0;
        sprintf(tr3->text, "%dP ITEM", arg0 + 1);
    }
}

void fn_10_1BD28(struct Sprite *arg0)
{
    struct SpriteDrawRequest req;
    float tf31;
    float tf30;
    struct RacePlayer *tr30;
    s16 i;
    s16 j;

    tr30 = (struct RacePlayer *)balls[arg0->userVar].unk150;
    if (tr30->unk2C8[0] == 0) {
        return;
    }
    set_ui_widescreen_scale_mtx((s32) arg0->x);
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.u1 = req.v1 = 0.0f;
    req.u2 = req.v2 = 1.0f;
    req.rotation = 0;
    req.unk30 = -1;
    req.flags = 5;
    req.opacity = 1.0f;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.flags = 0x20000F;
    tf31 = arg0->x;
    tf30 = arg0->y;
    for (i = 0; i < 10; i++) {
        if (tr30->unk2C8[0] & (1 << i)) {
            req.bmpId = lbl_10_data_18E0[i];
            req.x = tf31;
            req.y = tf30;
            req.z = 0.1f;
            draw_sprite_draw_request(&req);
            break;
        }
    }
    tf30 -= 8.0;
    for (i = 1; i < 3; i++) {
        if (tr30->unk2C8[i] == 0) {
            break;
        }
        tf30 -= 26.0f;
        for (j = 0; j < 10; j++) {
            if (tr30->unk2C8[i] & (1 << j)) {
                req.bmpId = lbl_10_data_18F0[j];
                req.x = tf31;
                req.y = tf30;
                req.z = 0.1f;
                draw_sprite_draw_request(&req);
                break;
            }
        }
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_10_1BF5C(struct Sprite *arg0)
{
    struct SpriteDrawRequest req;
    float tf31;
    float tf30;
    struct RacePlayer *tr30;
    s16 i;
    s16 j;

    tr30 = (struct RacePlayer *)balls[arg0->userVar].unk150;
    if (tr30->unk2C8[0] == 0) {
        return;
    }
    set_ui_widescreen_scale_mtx((s32) arg0->x);
    req.scaleX = 1.5f;
    req.scaleY = 1.5f;
    req.u1 = req.v1 = 0.0f;
    req.u2 = req.v2 = 1.0f;
    req.rotation = 0;
    req.unk30 = -1;
    req.flags = 5;
    req.opacity = 1.0f;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.flags = 0x20000F;
    tf31 = arg0->x;
    tf30 = arg0->y;
    for (i = 0; i < 10; i++) {
        if (tr30->unk2C8[0] & (1 << i)) {
            req.bmpId = lbl_10_data_18E0[i];
            req.x = tf31;
            req.y = tf30;
            req.z = 0.1f;
            draw_sprite_draw_request(&req);
            break;
        }
    }
    tf30 -= 16.0;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.u1 = req.v1 = 0.0f;
    req.u2 = req.v2 = 1.0f;
    req.rotation = 0;
    req.unk30 = -1;
    req.flags = 5;
    req.opacity = 1.0f;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.flags = 0x20000F;
    for (i = 1; i < 3; i++) {
        if (tr30->unk2C8[i] == 0) {
            break;
        }
        tf30 -= 34.0f;
        for (j = 0; j < 10; j++) {
            if (tr30->unk2C8[i] & (1 << j)) {
                req.bmpId = lbl_10_data_18E0[j];
                req.x = tf31;
                req.y = tf30;
                req.z = 0.1f;
                draw_sprite_draw_request(&req);
                break;
            }
        }
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_10_1C1F8(s16 arg0)
{
    struct Sprite *tr3;
    s32 hi;
    s32 lo;
    struct TPLTextureHeader *tr5;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->tag = arg0 + 0x6C;
        tr3->bmpId = 0x71F;
        tr3->depth = 0.2f;
        tr3->unk8C |= 1;
        tr3->userVar = arg0;
        if (modeCtrl.unk30 == 1) {
            hi = (s32) tr3->bmpId >> 8;
            lo = tr3->bmpId & 0xFF;
            tr5 = &bmp_infos[hi].tpl->texHeaders[lo];
            tr3->scaleX = 1.0f;
            tr3->scaleY = 1.0f;
            tr3->textAlign = 6;
            tr3->mainFunc = fn_10_1C490;
            tr3->unk58 = tr5->width;
            tr3->unk5C = tr5->height;
        } else {
            tr3->x = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + ((s32) (640.0f * cameras[arg0].sub28.viewportW) / 2));
            { s32 t1 = (s32) (480.0f * cameras[arg0].sub28.viewportY); s32 t2 = (s32) (480.0f * cameras[arg0].sub28.viewportH) / 2 - 0x28; tr3->y = t1 + t2; }
            tr3->scaleX = 0.6f;
            tr3->scaleY = 0.6f;
            tr3->textAlign = 4;
            tr3->mainFunc = fn_10_1CA1C;
        }
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tr3->x;
        sprintf(tr3->text, "%dP reverse", arg0 + 1);
    }
}

void fn_10_1C490(s8 *arg0, struct Sprite *arg1)
{
    float tf0;
    float tf1;
    float tf2;
    float tf3;
    float tf5;
    u8 tr0;
    struct RacePlayer *tr7;

    tr7 = &lbl_10_bss_1230[arg1->userVar];
    tr0 = arg1->userVar;
    tf5 = arg1->unk58;
    tf0 = arg1->unk5C;
    if (tr7->unk10 < 0x1E) {
        arg1->unk8C |= 1;
        return;
    }
    if (tr7->unk10 < (s32) (lbl_80198B38 * 2)) {
        if ((u32) (g_font_char_var_3 & 8) != 0) {
            arg1->unk8C |= 1;
            return;
        }
        arg1->scaleX = arg1->scaleY = 1.0f;
        arg1->x = ((0.5 * tf5) + (0.5 * (s32) (640.0f * cameras[tr0].sub28.viewportW)));
        arg1->y = (((0.5 * (s32) (480.0f * cameras[tr0].sub28.viewportH)) - (0.5 * tf0)) - 90.0);
        arg1->widescreenArg = (s16) (0.5 * (s32) (640.0f * cameras[tr0].sub28.viewportW));
        goto block_9;
    }
    if (tr7->unk10 < ((0.25 * lbl_80198B34) + (lbl_80198B38 * 2))) {
        tf1 = (tr7->unk10 - (lbl_80198B38 * 2)) / (float) (0.25 * lbl_80198B34);
        tf2 = (1.0 - tf1);
        tf3 = (tf2 + (0.5 * tf1));
        arg1->scaleY = tf3;
        arg1->scaleX = tf3;
        arg1->x = ((tf2 * ((0.5 * tf5) + (0.5 * (s32) (640.0f * cameras[tr0].sub28.viewportW)))) + (tf1 * ((s32) (640.0f * cameras[tr0].sub28.viewportW) - 16.0)));
        arg1->y = ((140.0 * tf1) + (tf2 * (((0.5 * (s32) (480.0f * cameras[tr0].sub28.viewportH)) - (0.5 * tf0)) - 90.0)));
        arg1->widescreenArg = (s16) ((tf2 * (0.5 * (s32) (640.0f * cameras[tr0].sub28.viewportW))) + (tf1 * ((s32) (640.0f * cameras[tr0].sub28.viewportW) - 16.0)));
    } else {
        arg1->scaleX = arg1->scaleY = 0.5f;
        arg1->x = ((s32) (640.0f * cameras[tr0].sub28.viewportW) - 16.0);
        arg1->y = 140.0f;
        arg1->widescreenArg = (s16) ((s32) (640.0f * cameras[tr0].sub28.viewportW) - 16.0);
    }
block_9:
    arg1->unk8C &= 0xFFFFFFFE;
}

void fn_10_1CA1C(s8 *arg0, struct Sprite *arg1)
{
    s32 idx = arg1->userVar;
    struct RacePlayer *player = &lbl_10_bss_1230[idx];

    if ((player->unk10 < 0x1E) || ((u32) (g_font_char_var_3 & 8) != 0)) {
        arg1->unk8C |= 1;
        return;
    }
    arg1->unk8C &= 0xFFFFFFFE;
}

void fn_10_1CA74(s16 arg0)
{
    struct Ball *tr5;
    struct Camera *tr30;
    struct Sprite *tr3;

    tr5 = &balls[arg0];
    tr30 = &cameras[arg0];
    if ((tr5->flags & 0x80000000000ULL) == 0) {
        tr3 = create_sprite();
        if (tr3 != NULL) {
            sprintf(tr3->text, "%dP GOAL", arg0 + 1);
            tr3->tag = arg0 + 0x6C;
            tr3->type = 1;
            tr3->bmpId = 0x703;
            tr3->textAlign = 4;
            tr3->depth = 0.2f;
            tr3->x = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + ((s32) (640.0f * cameras[arg0].sub28.viewportW) / 2));
            tr3->y = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + ((s32) (480.0f * cameras[arg0].sub28.viewportH) / 2));
            if ((0.5 == tr30->sub28.viewportW) && (0.5 == tr30->sub28.viewportH)) {
                tr3->scaleX = 0.5f;
                tr3->scaleY = 0.5f;
            } else if ((0.5 == tr30->sub28.viewportW) || (0.5 == tr30->sub28.viewportH)) {
                tr3->scaleX = 0.7f;
                tr3->scaleY = 0.7f;
            } else {
                tr3->scaleX = 1.0f;
                tr3->scaleY = 1.0f;
            }
            tr3->mainFunc = fn_10_1CD18;
            tr3->drawFunc = fn_10_1CD34;
            tr3->userVar = 0x18;
        }
    }
}

void fn_10_1CD18(s8 *arg0, struct Sprite *arg1)
{
    if (arg1->userVar > 0) {
        arg1->userVar--;
    }
}

const float lbl_10_rodata_dead1[2] = {12.0f, 150.0f};

void fn_10_1CD34(struct Sprite *arg0)
{
    struct SpriteDrawRequest req;
    s16 r29;
    float f;

    f = (float) arg0->userVar / 24.0f;
    f = 1.0 - fabs(2.0f * (f - 0.5f));
    r29 = 255.0f * f;
    if (r29 > 0xFF) {
        r29 = 0xFF;
    }
    set_ui_widescreen_scale_mtx((s32) arg0->x);
    req.scaleX = arg0->scaleX;
    req.scaleY = arg0->scaleY;
    req.u1 = req.v1 = 0.0f;
    req.u2 = req.v2 = 1.0f;
    req.rotation = 0;
    req.unk30 = -1;
    req.flags = 5;
    req.opacity = 1.0f;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.addColor = r29 | ((r29 << 0x10) | (r29 << 8));
    req.flags = 0xA;
    req.bmpId = arg0->bmpId;
    req.x = arg0->x;
    req.y = arg0->y;
    req.z = arg0->depth;
    draw_sprite_draw_request(&req);
    reset_ui_widescreen_scale_mtx();
}

void fn_10_1CEA4(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    char sp8[16];
    float tf31;
    float f30;
    struct Sprite *tr3;

    tf31 = 171.0f;
    textdraw_reset();
    if ((course->unk2 & 8) != 0) {
        textdraw_set_font(4);
        sprintf(sp8, "99 POINTS");
    } else {
        textdraw_set_font(0x4F);
        sprintf(sp8, "99;99<99");
    }
    tf31 += textdraw_get_pixel_width_of_string_as_f32(sp8);
    tf31 = (s32) (320.0 - (0.5 * tf31));
    f30 = 464.0f;
    if (player_count > 4) {
        f30 -= 126.0;
    } else {
        f30 -= (float) (player_count * 0x1C);
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 0;
        tr3->fontId = 4;
        tr3->x = tf31;
        tr3->y = f30;
        tr3->depth = 0.1f;
        tr3->textAlign = 0;
        tr3->drawFunc = fn_10_1D2B0;
        if (player_count > 4) {
            tr3->mainFunc = fn_10_1D090;
            tr3->unk58 = 112.0f;
            tr3->unk5C = tr3->unk58;
            tr3->unk60 = 0.0f;
            tr3->userVar = 0;
            tr3->unk50 = 0;
        } else {
            tr3->unk5C = 0.0f;
            tr3->userVar = 0;
        }
    }
}

void fn_10_1D090(s8 *arg0, struct Sprite *arg1)
{
    s16 r5;

    if (arg1->userVar != 0) {
        if (arg1->unk50 > 0) {
            arg1->unk5C += arg1->unk60;
            if (arg1->unk5C < 0.0) {
                arg1->unk5C = 0.0f;
            } else if (arg1->unk5C > arg1->unk58) {
                arg1->unk5C = arg1->unk58;
            }
            arg1->unk50--;
        }
        if (arg1->unk50 == 0) {
            for (r5 = 0; r5 < modeCtrl.unk30; r5++) {
                if (pad_status_groups[player_pad_map[r5]].unk0[0].err == 0) {
                    if (((s32) (*analog_inputs[player_pad_map[r5]] & 8) != 0) || ((s32) (pad_status_groups[player_pad_map[r5]].unk0[0].button & 8) != 0)) {
                        arg1->unk60 = -2.0f;
                        arg1->unk50 = 0xE;
                        break;
                    }
                    if (((s32) (*analog_inputs[player_pad_map[r5]] & 4) != 0) || ((s32) (pad_status_groups[player_pad_map[r5]].unk0[0].button & 4) != 0)) {
                        arg1->unk60 = 2.0f;
                        arg1->unk50 = 0xE;
                        break;
                    }
                }
            }
        }
    } else {
        arg1->unk5C -= 0.5;
        if (arg1->unk5C <= 0.0) {
            arg1->unk5C = 0.0f;
            arg1->userVar = 1;
        }
    }
}

void fn_10_1D2B0(struct Sprite *arg0)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct SpriteDrawRequest req;
    float tf31;
    float f30;
    float f29;
    float f28;
    float tf28;
    s16 r28;
    s16 r27;
    struct RacePlayer *tr26;
    struct Ball *tr25;
    s32 tr5;
    s32 r23;

    tf31 = arg0->y;
    r27 = -1;
    set_ui_widescreen_scale_mtx(0x140);
    for (r28 = 0; r28 < player_count; r28++) {
        tr26 = lbl_10_bss_2D68[r28];
        tr25 = &balls[tr26->unk0];
        f30 = ((28.0 * r28) - arg0->unk5C);
        if (!(f30 < -28.0)) {
            if (!(f30 >= 112.0)) {
                r27 = r28;
                if (f30 < 0.0) {
                    f28 = (1.0 + (f30 / 28.0));
                    f29 = f28;
                    f30 += (14.0 * (1.0 - f29));
                } else {
                    if (f30 >= 84.0) {
                        f28 = (1.0 - ((f30 - 84.0) / 28.0));
                        f29 = f28;
                        f30 -= (14.0 * (1.0 - f29));
                    } else {
                        f28 = 1.0f;
                        f29 = 1.0f;
                    }
                }
                if ((arg0->userVar != 0) && (player_count > modeCtrl.unk30) && ((tr25->flags & 0x80000000000ULL) == 0)) {
                    tr5 = (60.0 * (0.5 * (1.0 + math_sin((s16) ((g_font_char_var_3 << 0xB) + (r28 << 0xC))))));
                    r23 = (u8) tr5 | (((u8) tr5 << 0x10) | ((u8) tr5 << 8));
                } else {
                    r23 = 0;
                }
                textdraw_reset();
                textdraw_set_flags(0x200000);
                textdraw_set_alpha(f28);
                textdraw_set_scale(1.0f, f29);
                req.scaleX = 1.0f;
                req.scaleY = f29;
                req.u1 = req.v1 = 0.0f;
                req.u2 = req.v2 = 1.0f;
                req.rotation = 0;
                req.unk30 = -1;
                req.flags = 5;
                req.opacity = f28;
                req.mulColor = 0xFFFFFF;
                req.addColor = 0;
                req.mulColor = 0xFFFF0000;
                req.addColor = r23;
                req.bmpId = 0x718;
                req.x = arg0->x;
                req.y = tf31 + f30;
                req.z = arg0->depth;
                draw_sprite_draw_request(&req);
                textdraw_set_depth(arg0->depth - 0.01f);
                textdraw_set_alignment(4);
                textdraw_set_pos((float) (12.0 + arg0->x), (float) ((12.0 * f29) + (tf31 + f30)));
                textdraw_set_add_color(r23);
                if (((course->unk2 & 8) == 0) && ((tr26->flags & 2) == 0)) {
                    textdraw_set_font(5);
                    textdraw_printf("-");
                } else {
                    textdraw_set_font(0x4F);
                    textdraw_printf("%d", tr26->unk3E);
                }
                textdraw_set_font(4);
                textdraw_set_alignment(0);
                textdraw_set_pos(27.0f + arg0->x, 4.0f + (tf31 + f30));
                textdraw_set_mul_color(lbl_1_data_2E490[tr26->unk0]);
                if ((tr25->flags & 0x80000000000ULL) != 0) {
                    textdraw_printf("COM%d", tr26->unk0 + 1);
                } else {
                    textdraw_printf("PLAYER%d", tr26->unk0 + 1);
                }
                textdraw_set_pos(144.0f + (27.0f + arg0->x), 4.0f + (tf31 + f30));
                if ((course->unk2 & 8) != 0) {
                    textdraw_set_font(4);
                    if (lbl_10_bss_2D50[tr26->unk0] == 1) {
                        textdraw_printf("%2d POINT", lbl_10_bss_2D50[tr26->unk0]);
                    } else {
                        textdraw_printf("%2d POINTS", lbl_10_bss_2D50[tr26->unk0]);
                    }
                } else if ((tr26->flags & 2) != 0) {
                    textdraw_set_font(0x4F);
                    textdraw_printf("%02d;%02d<%02d", tr26->unk1E8, tr26->unk1E9, tr26->unk1EA);
                } else {
                    textdraw_set_font(4);
                    textdraw_printf("RETIRE");
                }
                textdraw_set_mul_color(-1);
            }
        }
    }
    if ((arg0->userVar != 0) && (player_count > 4)) {
        tf28 = (0.5 * fabs(math_sin((s16) (g_font_char_var_3 << 0xA))));
        if ((s16) (r27 - 3) > 0) {
            req.scaleX = 1.0f;
            req.scaleY = tf28;
            req.u1 = req.v1 = 0.0f;
            req.u2 = req.v2 = 1.0f;
            req.rotation = 0;
            req.unk30 = -1;
            req.flags = 5;
            req.opacity = 1.0f;
            req.mulColor = 0xFFFFFF;
            req.addColor = 0;
            req.flags = 0xD;
            req.bmpId = 0x738;
            req.x = arg0->x;
            req.y = (arg0->y - 2.0);
            req.z = (arg0->depth - 0.02);
            draw_sprite_draw_request(&req);
        }
        if (r27 < (s32) (player_count - 1)) {
            req.scaleX = 1.0f;
            req.scaleY = tf28;
            req.u1 = req.v1 = 0.0f;
            req.u2 = req.v2 = 1.0f;
            req.rotation = 0;
            req.unk30 = -1;
            req.flags = 5;
            req.opacity = 1.0f;
            req.mulColor = 0xFFFFFF;
            req.addColor = 0;
            req.flags = 0x100005;
            req.bmpId = 0x738;
            req.x = arg0->x;
            req.y = (112.0 + arg0->y);
            req.z = (arg0->depth - 0.02);
            draw_sprite_draw_request(&req);
        }
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_10_1D9F0(s16 arg0)
{
    s16 sp8[8] = { 0x728, 0x729, 0x72A, 0x72B, 0x72C, 0x72D, 0x72E, 0x72F };
    struct Race2Course *course = &lbl_10_bss_1208;
    float f31;
    float f30;
    float f29;
    float f28;
    float tf30;
    struct Camera *tc;
    struct Sprite *tr3;
    struct RacePlayer *tr28;

    tr28 = (struct RacePlayer *)balls[arg0].unk150;
    tc = &cameras[arg0];
    if ((0.5 == tc->sub28.viewportW) && (0.5 == tc->sub28.viewportH)) {
        f31 = (160.0 + (s32) (640.0f * cameras[arg0].sub28.viewportX));
        f30 = (20.0 + (145.0 + (s32) (480.0f * cameras[arg0].sub28.viewportY)));
        f29 = 0.8f;
        f28 = 0.8f;
    } else if ((0.5 == tc->sub28.viewportW) || (0.5 == tc->sub28.viewportH)) {
        f31 = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + (0.5 * (s32) (640.0f * cameras[arg0].sub28.viewportW)));
        f30 = (((s32) (480.0f * cameras[arg0].sub28.viewportY) + (s32) (480.0f * cameras[arg0].sub28.viewportH)) - 73.0);
        f29 = 0.8f;
        f28 = 0.8f;
    } else {
        f31 = 320.0f;
        f30 = 359.0f;
        f29 = 1.0f;
        f28 = 1.0f;
    }
    if ((modeCtrl.unk30 >= 2) && ((tr28->flags & 2) == 0)) {
        f30 -= 90.0f;
    }
    tr3 = create_sprite();
    if (tr3 == NULL) {
        return;
    }
    tr3->tag = arg0 + 0x6C;
    tr3->type = 1;
    tr3->bmpId = (u16) sp8[tr28->unk3E - 1];
    tr3->x = f31;
    tr3->y = f30;
    tr3->scaleX = f29;
    tr3->scaleY = f28;
    tr3->depth = 0.1f;
    tr3->textAlign = 4;
    tr3->counter = 0x14;
    tr3->flags = 0x01000000;
    tr3->widescreenArg = (s16) f31;
    tr3->mainFunc = fn_10_1E18C;
    tr3->userVar = arg0;
    sprintf(tr3->text, "%dP RANK RESULT", arg0 + 1);
    tf30 = f30 + (float) (s32) (2.0 + (25.0 * f28));
    tr3 = create_sprite();
    if (tr3 == NULL) {
        return;
    }
    tr3->tag = arg0 + 0x6C;
    tr3->type = 0;
    tr3->fontId = 4;
    tr3->x = 20.0f + f31;
    tr3->y = tf30;
    tr3->depth = 0.3f;
    tr3->textAlign = 3;
    tr3->flags = 0x01000000;
    tr3->widescreenArg = (s16) f31;
    tr3->mainFunc = fn_10_1E28C;
    tr3->counter = 0x14;
    tr3->userVar = arg0;
    if ((tr28->flags & 2) != 0) {
        sprintf(tr3->text, "TOTAL TIME");
    } else {
        sprintf(tr3->text, "RETIRE");
    }
    if ((tr28->flags & 2) != 0) {
        tr3 = create_sprite();
        if (tr3 == NULL) {
            return;
        }
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 0x4F;
        tr3->flags = 0x200000;
        tr3->x = 20.0f + f31;
        tr3->y = 20.0f + tf30;
        tr3->depth = 0.3f;
        tr3->textAlign = 3;
        tr3->flags = 0x01200000;
        tr3->widescreenArg = (s16) f31;
        tr3->mainFunc = fn_10_1E28C;
        tr3->userVar = arg0;
        tr3->counter = 0x1E;
        sprintf(tr3->text, "%02d;%02d<%02d", tr28->unk1E8, tr28->unk1E9, tr28->unk1EA);
    }
    if (((course->unk2 & 8) != 0) && ((tr28->flags & 2) != 0)) {
        tr3 = create_sprite();
        if (tr3 == NULL) {
            return;
        }
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 4;
        tr3->flags = 0x200000;
        tr3->x = 20.0f + f31;
        tr3->y = tf30;
        tr3->depth = 0.2f;
        tr3->textAlign = 3;
        tr3->flags = 0x01200000;
        tr3->widescreenArg = (s16) f31;
        tr3->mainFunc = fn_10_1E46C;
        tr3->counter = (lbl_80198B38 * 2) + 0x28;
        tr3->mulR = 0x40;
        if (tr28->unk42 == 1) {
            sprintf(tr3->text, "GET %d POINT!", tr28->unk42);
        } else {
            sprintf(tr3->text, "GET %d POINTS!", tr28->unk42);
        }
    }
    if ((course->unk2 & 8) != 0) {
        tr3 = create_sprite();
        if (tr3 == NULL) {
            return;
        }
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 4;
        tr3->flags = 0x200000;
        tr3->x = 20.0f + f31;
        tr3->y = 20.0f + tf30;
        tr3->depth = 0.2f;
        tr3->textAlign = 3;
        tr3->flags = 0x01200000;
        tr3->widescreenArg = (s16) f31;
        tr3->mainFunc = fn_10_1E46C;
        tr3->counter = (lbl_80198B38 * 2) + 0x32;
        tr3->mulB = 0x40;
        if (lbl_10_bss_2D50[tr28->unk0] == 1) {
            sprintf(tr3->text, "TOTAL %d POINT", lbl_10_bss_2D50[tr28->unk0]);
            return;
        }
        sprintf(tr3->text, "TOTAL %d POINTS", lbl_10_bss_2D50[tr28->unk0]);
    }
}

void fn_10_1E18C(s8 *arg0, struct Sprite *arg1)
{
    struct RacePlayer *player = (struct RacePlayer *)balls[arg1->userVar].unk150;
    float scale;
    float tf3;

    if (arg1->counter > 0) {
        arg1->counter--;
    }
    switch (modeCtrl.unk30) {
    case 1:
        scale = 1.0f;
        break;
    case 2:
    case 3:
    case 4:
        scale = 0.8f;
        break;
    }
    tf3 = (float) arg1->counter / 20.0f;
    arg1->opacity = (1.0 - tf3);
    arg1->scaleX = scale * (1.0f + (2.0f * tf3));
    arg1->scaleY = scale * (1.0f + (2.0f * tf3));
    if (player->unk3E == 1) {
        arg1->rotation = arg1->counter * arg1->counter * 0x96;
    }
}

void fn_10_1E28C(s8 *arg0, struct Sprite *arg1)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct RacePlayer *player = (struct RacePlayer *)balls[arg1->userVar].unk150;
    float f;

    if (arg1->counter >= 0) {
        arg1->counter--;
    }
    f = (float) arg1->counter / 20.0f;
    if (arg1->counter >= 0x14) {
        arg1->opacity = 0.0f;
    } else if (arg1->counter >= 0) {
        arg1->opacity = (1.0 - f);
        arg1->x -= 1.0f;
    }
    if (((course->unk2 & 8) != 0) && ((u32) (player->flags & 2) != 0) && (arg1->counter < 0)) {
        arg1->mainFunc = fn_10_1E398;
        arg1->counter = lbl_80198B38 * 2;
    }
}

void fn_10_1E398(s8 *arg0, struct Sprite *arg1)
{
    if (arg1->counter >= 0) {
        arg1->counter--;
    }
    if (arg1->counter < 0) {
        arg1->mainFunc = fn_10_1E3D4;
        arg1->counter = 0x14;
    }
}

void fn_10_1E3D4(s8 *arg0, struct Sprite *arg1)
{
    float f;

    if (arg1->counter >= 0) {
        arg1->counter--;
    }
    f = arg1->counter / 20.0f;
    if (arg1->counter >= 0) {
        arg1->opacity = f;
        arg1->x -= 1.0f;
    }
    if (arg1->counter < 0) {
        *arg0 = 0;
    }
}

void fn_10_1E46C(s8 *arg0, struct Sprite *arg1)
{
    float f;
    struct Race2Course *course = &lbl_10_bss_1208;

    if (((course->unk2 & 8) != 0) && (modeCtrl.unk30 >= 2) && (course->unk6 < 5) && (lbl_10_bss_121C == 8) && (lbl_10_bss_1224 == (lbl_80198B38 * 4))) {
        arg1->mainFunc = fn_10_1E590;
        arg1->counter = 0x14;
        return;
    }
    if (arg1->counter >= 0) {
        arg1->counter--;
    }
    f = (float) arg1->counter / 20.0f;
    if (arg1->counter >= 0x14) {
        arg1->opacity = 0.0f;
        return;
    }
    if (arg1->counter >= 0) {
        arg1->opacity = (1.0 - f);
        arg1->x -= 1.0f;
    }
}

void fn_10_1E590(s8 *arg0, struct Sprite *arg1)
{
    float f;

    if (arg1->counter >= 0) {
        arg1->counter--;
    }
    f = arg1->counter / 20.0f;
    if (arg1->counter >= 0) {
        arg1->opacity = f;
        arg1->x -= 1.0f;
    }
}

void fn_10_1E614(s16 arg0)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    double f1;
    double f1_2;
    struct Camera *tr28;
    struct Sprite *tr3;

    tr28 = &cameras[arg0];
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        if ((1.0 == tr28->sub28.viewportW) && (1.0 == tr28->sub28.viewportH)) {
            tr3->fontId = 9;
            tr3->x = 320.0f;
            tr3->y = 390.0f;
            tr3->scaleX = 0.7f;
            tr3->scaleY = 0.7f;
            tr3->mulR = 0xFF;
            tr3->mulG = 0xA0;
            tr3->mulB = 0x55;
        } else {
            tr3->fontId = 4;
            tr3->x = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + (0.5 * (s32) (640.0f * cameras[arg0].sub28.viewportW)));
            f1 = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + (s32) (480.0f * cameras[arg0].sub28.viewportH));
            tr3->y = (f1 - 74.0);
            tr3->scaleX = 1.0f;
            tr3->scaleY = 1.0f;
        }
        tr3->depth = 0.1f;
        tr3->textAlign = 4;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tr3->x;
        tr3->mainFunc = fn_10_1EB08;
        tr3->counter = 0x14;
        tr3->unk58 = tr3->scaleY;
        sprintf(tr3->text, "RETIRE");
    }
    if (((course->unk2 & 8) != 0) && (tr3 = create_sprite(), ((tr3 == NULL) == 0))) {
        tr3->tag = arg0 + 0x6C;
        tr3->fontId = 4;
        if ((1.0 == tr28->sub28.viewportW) && (1.0 == tr28->sub28.viewportH)) {
            tr3->x = 320.0f;
            tr3->y = 430.0f;
        } else {
            tr3->x = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + (0.5 * (s32) (640.0f * cameras[arg0].sub28.viewportW)));
            f1_2 = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + (s32) (480.0f * cameras[arg0].sub28.viewportH));
            tr3->y = (f1_2 - 25.0);
        }
        tr3->depth = 0.1f;
        tr3->scaleX = 1.0f;
        tr3->scaleY = 1.0f;
        tr3->mulR = 0xFF;
        tr3->mulG = 0xFF;
        tr3->mulB = 0x40;
        tr3->textAlign = 4;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tr3->x;
        tr3->mainFunc = fn_10_1EB84;
        tr3->counter = 0x14;
        tr3->unk58 = tr3->scaleY;
        if (lbl_10_bss_2D50[arg0] == 1) {
            sprintf(tr3->text, "TOTAL %d POINT", lbl_10_bss_2D50[arg0]);
            return;
        }
        sprintf(tr3->text, "TOTAL %d POINTS", lbl_10_bss_2D50[arg0]);
    }
}

void fn_10_1EB08(s8 *arg0, struct Sprite *arg1)
{
    float f;

    f = (float) arg1->counter / 20.0f;
    if (arg1->counter > 0) {
        arg1->counter--;
    }
    arg1->scaleY = (arg1->unk58 * (1.0 - f));
}

void fn_10_1EB84(s8 *arg0, struct Sprite *arg1)
{
    float f;

    f = (float) arg1->counter / 20.0f;
    if (arg1->counter > 0) {
        arg1->counter--;
        if (arg1->counter == 0) {
            struct Race2Course *course = &lbl_10_bss_1208;
            if ((course->unk2 & 8) != 0) {
                arg1->mainFunc = fn_10_1EC4C;
                arg1->counter = lbl_80198B38 * 3;
            }
        }
    }
    arg1->scaleY = (arg1->unk58 * (1.0 - f));
}

void fn_10_1EC4C(s8 *arg0, struct Sprite *arg1)
{
    if (arg1->counter > 0) {
        arg1->counter--;
        return;
    }
    if (arg1->opacity > 0.0) {
        arg1->opacity -= (1.0 / lbl_80198B34);
        if (arg1->opacity < 0.0) {
            arg1->opacity = 0.0f;
        }
    }
}

void fn_10_1ECC0(s16 arg0)
{
    struct Sprite *tr3;
    struct RacePlayer *tr29;

    tr29 = (struct RacePlayer *)balls[arg0].unk150;
    tr3 = create_sprite();
    if (tr3 == NULL) {
        return;
    }
    tr3->tag = arg0 + 0x6C;
    tr3->type = 0;
    tr3->fontId = 4;
    tr3->x = 320.0f;
    tr3->y = 355.0f;
    tr3->depth = 0.1f;
    tr3->textAlign = 4;
    tr3->flags = 0x01000000;
    tr3->widescreenArg = (s16) tr3->x;
    sprintf(tr3->text, "TOTAL TIME");
    tr3 = create_sprite();
    if (tr3 == NULL) {
        return;
    }
    tr3->tag = arg0 + 0x6C;
    tr3->type = 0;
    tr3->fontId = 0x4F;
    tr3->x = 320.0f;
    tr3->y = 375.0f;
    tr3->depth = 0.1f;
    tr3->textAlign = 4;
    tr3->flags = 0x01000000;
    tr3->widescreenArg = (s16) tr3->x;
    sprintf(tr3->text, "%02d;%02d<%02d", tr29->unk1E8, tr29->unk1E9, tr29->unk1EA);
    if (lbl_10_bss_418C < 0) {
        return;
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 0;
        tr3->fontId = 9;
        tr3->x = 320.0f;
        tr3->y = 412.0f;
        tr3->depth = 0.1f;
        tr3->scaleX = 0.4f;
        tr3->scaleY = 0.7f;
        tr3->mulR = 0xFF;
        tr3->mulG = 0x80;
        tr3->mulB = 0x20;
        tr3->textAlign = 4;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tr3->x;
        tr3->mainFunc = fn_10_1EF18;
        tr3->counter = 0x14;
        if (lbl_10_bss_418C == 0) {
            sprintf(tr3->text, "BEST RECORD\\");
            call_SoundReqID_arg_0(0x1E3);
            return;
        }
        sprintf(tr3->text, "NEW RECORD[");
        call_SoundReqID_arg_0(0x1E5);
    }
}

void fn_10_1EF18(s8 *arg0, struct Sprite *arg1)
{
    float tf2;

    tf2 = arg1->counter / 20.0f;
    if (arg1->counter > 0) {
        arg1->counter--;
    }
    arg1->scaleX = 0.4f + (2.0f * tf2);
    arg1->scaleY = 0.7f + (2.0f * tf2);
    arg1->opacity = 1.0f - tf2;
}

void fn_10_1EFA0(void)
{
    struct Sprite *tr3;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = 0x6C;
        tr3->type = 1;
        tr3->bmpId = 0x736;
        tr3->x = 624.0f;
        tr3->y = 50.0f;
        tr3->depth = 0.1f;
        tr3->textAlign = 6;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tr3->x;
        sprintf(tr3->text, "TIME ATTACK");
    }
}

const double lbl_10_rodata_dead2[3] = {0.1, 0.05, 2.1};

void fn_10_1F040(void)
{
    struct Sprite *tr3;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->bmpId = 0x4B;
        tr3->x = 0.0f;
        tr3->y = 0.0f;
        tr3->depth = 0.05f;
        tr3->scaleX = 80.0f;
        tr3->scaleY = 60.0f;
        tr3->opacity = 0.0f;
        tr3->mulR = 0;
        tr3->mulG = 0;
        tr3->mulB = 0;
        tr3->mainFunc = fn_10_1F0E4;
        tr3->userVar = 0;
    }
}

void fn_10_1F0E4(s8 *arg0, struct Sprite *arg1)
{
    arg1->opacity += (0.5 / lbl_80198B38);
    if (arg1->opacity > 0.5) {
        arg1->opacity = 0.5f;
        if (arg1->userVar == 0) {
            fn_10_1F19C();
            arg1->userVar = -1;
        }
    }
}

void fn_10_1F19C(void)
{
    char sp8[16];
    float tf31;
    float f30;
    s16 r30;
    struct Sprite *tr3;

    tf31 = 171.0f;
    textdraw_reset();
    textdraw_set_font(4);
    sprintf(sp8, "99 POINTS");
    tf31 += textdraw_get_pixel_width_of_string_as_f32(sp8);
    tf31 = (s32) (320.0 - (0.5 * tf31));
    f30 = 240.0 - (0.5 * (float) (player_count * 0x1C));
    if (modeCtrl.unk30 == 1) {
        f30 -= 18.0;
    } else if (player_count > 4) {
        f30 += 56.0;
    } else {
        f30 += 14.0;
    }
    for (r30 = 0; r30 < 8; r30++) {
        if (lbl_10_bss_4238[r30] == NULL) {
            break;
        }
        tr3 = create_sprite();
        if (tr3 == NULL) {
            break;
        }
        tr3->type = 0;
        tr3->x = 20.0f + tf31 + (float) (r30 * 0x14);
        tr3->y = f30 + (float) (r30 * 0x1C);
        tr3->depth = 0.04f;
        tr3->fontId = 4;
        tr3->textAlign = 0;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = 0x140;
        tr3->mainFunc = fn_10_1F3F0;
        tr3->drawFunc = fn_10_1F424;
        tr3->userVar = r30;
        tr3->counter = (r30 * 0x14) + 0x14;
    }
}

void fn_10_1F3F0(s8 *arg0, struct Sprite *arg1)
{
    if (arg1->counter > 0) {
        arg1->x -= 1.0f;
        arg1->counter--;
    }
}

void fn_10_1F424(struct Sprite *arg0)
{
    struct SpriteDrawRequest req;
    float tf31;
    s32 tr5;
    s32 r26;
    struct RacePlayer *tr29;
    struct Ball *tr28;

    tr29 = lbl_10_bss_4238[arg0->userVar];
    tr28 = &balls[tr29->unk0];
    if (arg0->counter <= 0x14) {
        tf31 = 1.0 - (float) arg0->counter / 20.0f;
        if ((player_count > modeCtrl.unk30) && ((tr28->flags & 0x80000000000ULL) == 0)) {
            tr5 = (60.0 * (0.5 * (1.0 + math_sin((s16) ((g_font_char_var_3 << 0xB) + (tr28->playerId << 0xC))))));
            r26 = (u8) tr5 | (((u8) tr5 << 0x10) | ((u8) tr5 << 8));
        } else {
            r26 = 0;
        }
        textdraw_reset();
        textdraw_set_flags(0x200000);
        textdraw_set_alpha(tf31);
        tf31 = arg0->y;
        req.scaleX = 1.0f;
        req.scaleY = 1.0f;
        req.u1 = req.v1 = 0.0f;
        req.u2 = req.v2 = 1.0f;
        req.rotation = 0;
        req.unk30 = -1;
        req.flags = 5;
        req.opacity = 1.0f;
        req.mulColor = 0xFFFFFF;
        req.addColor = 0;
        req.mulColor = 0xFFFF0000;
        req.addColor = r26;
        req.bmpId = 0x718;
        req.x = arg0->x;
        req.y = tf31;
        req.z = arg0->depth;
        draw_sprite_draw_request(&req);
        textdraw_set_depth(arg0->depth);
        textdraw_set_alignment(4);
        textdraw_set_pos((float) (12.0 + arg0->x), (float) (12.0 + arg0->y));
        textdraw_set_font(0x4F);
        textdraw_set_add_color(r26);
        textdraw_printf("%d", tr29->unk40);
        textdraw_set_font(4);
        textdraw_set_alignment(0);
        textdraw_set_mul_color(lbl_1_data_2E490[tr29->unk0]);
        textdraw_set_pos(27.0f + arg0->x, 4.0f + tf31);
        if ((tr28->flags & 0x80000000000ULL) != 0) {
            textdraw_printf("COM%d", tr29->unk0 + 1);
        } else {
            textdraw_printf("PLAYER%d", tr29->unk0 + 1);
        }
        textdraw_set_font(4);
        textdraw_set_pos(144.0f + (27.0f + arg0->x), 4.0f + tf31);
        if (lbl_10_bss_2D50[tr29->unk0] == 1) {
            textdraw_printf("%2d POINT", lbl_10_bss_2D50[tr29->unk0]);
        } else {
            textdraw_printf("%2d POINTS", lbl_10_bss_2D50[tr29->unk0]);
        }
        textdraw_set_mul_color(-1);
    }
}

char lbl_10_data_1B20[6] = "READY";

void fn_10_1F7DC(s16 arg0)
{
    float tf31;
    float f30;
    struct Sprite *tr3;

    tf31 = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + (0.5 * (s32) (640.0f * cameras[arg0].sub28.viewportW)));
    f30 = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + (0.5 * (s32) (480.0f * cameras[arg0].sub28.viewportH)));
    if (modeCtrl.unk30 == 1) {
        f30 -= 35.0;
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = arg0 + 0x6C;
        tr3->type = 1;
        tr3->bmpId = 2;
        tr3->unk90 = 0.6f;
        tr3->unk98 = (0.2 + tr3->unk90);
        tr3->unk94 = 0.5f;
        tr3->unk9C = (0.5 + tr3->unk94);
        if (modeCtrl.unk30 == 1) {
            tr3->scaleX = tr3->scaleY = 3.0f;
        } else {
            tr3->scaleX = tr3->scaleY = 2.0f;
        }
        tr3->scaleX *= 0.2;
        tr3->scaleY *= 0.5;
        tr3->x = tf31;
        tr3->y = f30;
        tr3->depth = 0.1f;
        tr3->textAlign = 4;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tf31;
        tr3->mainFunc = fn_10_1FAAC;
        tr3->userVar = arg0;
        sprintf(tr3->text, "%dP lever_gacha", arg0 + 1);
    }
}

void fn_10_1FAAC(s8 *arg0, struct Sprite *arg1)
{
    struct Ball *ball = &balls[arg1->userVar];
    struct RacePlayer *player = &lbl_10_bss_1230[arg1->userVar];

    if ((ball->unk154 == 1) && ((u32) (player->unk2C2 & 0xFFFFFEFF) != 0)) {
        arg1->unk8C &= 0xFFFFFFFE;
        arg1->rotation = (s16) (5120.0f * math_sin((s16) (g_font_char_var_3 << 0xD)));
        return;
    }
    arg1->unk8C |= 1;
}

void fn_10_1FB70(float farg0, float farg1)
{
    struct Sprite *tr3;

    create_sprite();
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->bmpId = 0x726;
        tr3->textAlign = 6;
        tr3->x = farg0;
        tr3->y = farg1;
        tr3->depth = 0.1f;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = 0x270;
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->bmpId = 0x727;
        tr3->textAlign = 0;
        tr3->x = farg0;
        tr3->y = farg1;
        tr3->depth = 0.1f;
        tr3->drawFunc = fn_10_1FC5C;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = 0x270;
    }
}

void fn_10_1FC5C(struct Sprite *arg0)
{
    struct SpriteDrawRequest req;
    s16 a = 20;
    s16 b = 4;
    s16 r28;

    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.u1 = req.v1 = 0.0f;
    req.u2 = req.v2 = 1.0f;
    req.rotation = 0;
    req.unk30 = -1;
    req.flags = 5;
    req.opacity = 1.0f;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.flags |= 0x200000;
    for (r28 = 0; r28 < (g_font_char_var_3 % (a * b)) / a; r28++) {
        req.bmpId = arg0->bmpId;
        req.x = (arg0->x + (32.0 * r28));
        req.y = arg0->y;
        req.z = arg0->depth;
        draw_sprite_draw_request(&req);
    }
}

void fn_10_1FDA0(u8 arg0)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct Camera *tr30;
    struct Sprite *tr3;
    struct Race2CourseInfo *tr28;
    s32 tr6;
    s32 tr7;

    tr28 = &lbl_10_data_2ADC[course->unk0];
    tr30 = &cameras[arg0];
    tr3 = create_sprite();
    if (tr3 != NULL) {
        struct Race2MapInfo *tr4 = (struct Race2MapInfo *) &tr28->unk3C;

        tr3->type = 1;
        tr3->tag = arg0 + 0x6C;
        tr3->bmpId = (u16) tr4->bmpId;
        tr3->textAlign = 2;
        tr3->x = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + 0x10);
        tr6 = (480.0f * cameras[arg0].sub28.viewportY);
        tr7 = (s32) (480.0f * cameras[arg0].sub28.viewportH) - 0x18;
        tr3->y = tr6 + tr7;
        if ((1.0 == tr30->sub28.viewportW) && (1.0 == tr30->sub28.viewportH)) {
            tr3->scaleX = tr4->unk14;
            tr3->scaleY = tr4->unk14;
        } else {
            tr3->scaleX = tr4->unk18;
            tr3->scaleY = tr4->unk18;
            tr3->opacity = 0.4f;
        }
        tr3->depth = 0.1f;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) tr3->x;
        tr3->userVar = arg0;
        tr3->unk50 = tr4->unk4;
        tr3->unk54 = tr4->unk8;
        tr3->unk58 = tr4->unkC;
        tr3->unk5C = tr4->unk10;
        tr3->drawFunc = fn_10_1FFE4;
        sprintf(tr3->text, "%dP MAP", arg0 + 1);
    }
}

void fn_10_1FFE4(struct Sprite *arg0)
{
    struct SpriteDrawRequest req;
    float tf31;
    float tf30;
    float tf29;
    float f28;
    float f27;
    float zero;
    float one;
    float f1;
    float f5;
    float f6;
    GXColor *tr5;
    u8 tr30;
    struct Ball *r29;
    struct RacePlayer *r28;
    s16 r27;

    tr30 = arg0->userVar;
    tf31 = arg0->unk50;
    tf30 = arg0->unk54;
    tf29 = arg0->unk58;
    zero = 0.0f;
    one = 1.0f;
    if (modeCtrl.unk30 == 1) {
        f28 = 1.0f;
        f27 = 0.8f;
    } else {
        f28 = 0.8f;
        f27 = 0.6f;
    }
    req.scaleX = arg0->scaleX;
    req.scaleY = arg0->scaleY;
    req.u1 = req.v1 = 0.0f;
    req.u2 = req.v2 = 1.0f;
    req.rotation = 0;
    req.unk30 = -1;
    req.flags = 5;
    req.opacity = arg0->opacity;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.flags = 0xD;
    req.v1 = zero;
    req.v2 = one;
    req.bmpId = arg0->bmpId;
    req.x = arg0->x;
    req.y = arg0->y;
    req.z = (0.2 + arg0->depth);
    draw_sprite_draw_request(&req);
    req.scaleX = arg0->scaleX;
    req.scaleY = arg0->scaleY;
    req.u1 = req.v1 = 0.0f;
    req.u2 = req.v2 = 1.0f;
    req.rotation = 0;
    req.unk30 = -1;
    req.flags = 5;
    req.opacity = 0.9f;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.flags = 0xA;
    r27 = 0;
    r29 = balls;
    r28 = lbl_10_bss_1230;
    {
        for (; r27 < player_count; r27++, r29++, r28++) {
            tr5 = &lbl_10_data_18B8[r29->playerId];
            req.mulColor = tr5->b | ((tr5->r << 0x10) | 0xFF000000 | (tr5->g << 8));
            req.rotation = r28->unk35E;
            f5 = tf29 * (r29->pos.x - tf31);
            f6 = tf29 * (tf30 - r29->pos.z);
            f5 *= arg0->scaleX;
            f6 = -f6 * arg0->scaleY;
            if (r29->playerId == tr30) {
                f1 = arg0->depth;
                req.scaleY = f28;
                req.scaleX = f28;
            } else {
                f1 = (arg0->depth + (2.0 * (0.01 * r27)));
                req.scaleY = f27;
                req.scaleX = f27;
            }
            req.bmpId = 0x737;
            req.x = arg0->x + f5;
            req.y = arg0->y + f6;
            req.z = f1;
            draw_sprite_draw_request(&req);
        }
    }
}

void fn_10_20334(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct RacePlayer *player = &lbl_10_bss_1230[0];
    float x;
    float y;
    s8 r30;
    struct Sprite *tr3;

    if (course->unk4 == (player->unk44 + 1)) {
        call_SoundReqID_arg_0(0x1E4);
        x = 320.0f;
        y = 220.0f;
        tr3 = create_sprite();
        if (tr3 != NULL) {
            tr3->type = 0;
            tr3->fontId = 9;
            tr3->x = x;
            tr3->y = (y - 4.0);
            tr3->depth = 0.1f;
            tr3->scaleX = 0.48f;
            tr3->scaleY = 0.4f;
            tr3->textAlign = 5;
            tr3->flags = 0x01000000;
            tr3->widescreenArg = (s16) x;
            tr3->mainFunc = fn_10_2051C;
            tr3->counter = lbl_80198B38 * 2;
            sprintf(tr3->text, "FINAL LAP");
        }
        y += 4.0;
        r30 = 3;
    } else {
        call_SoundReqID_arg_0(0x2C9);
        x = 320.0f;
        y = 240.0f;
        r30 = 4;
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->bmpId = 0x730;
        tr3->x = x;
        tr3->y = y;
        tr3->scaleX = 1.5f;
        tr3->scaleY = 1.5f;
        tr3->depth = 0.1f;
        tr3->textAlign = r30;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) x;
        tr3->mainFunc = fn_10_205F4;
        tr3->counter = lbl_80198B38 * 2;
        sprintf(tr3->text, "TIME BONUS");
    }
}

void fn_10_2051C(s8 *arg0, struct Sprite *arg1)
{
    if (arg1->counter <= 0) {
        *arg0 = 0;
        return;
    }
    arg1->counter--;
    if ((s32) (arg1->counter % ((s32) ((2.0 * lbl_80198B34) / 15.0) * 2)) > (s32) ((2.0 * lbl_80198B34) / 15.0)) {
        arg1->unk8C &= 0xFFFFFFFE;
        return;
    }
    arg1->unk8C |= 1;
}

void fn_10_205F4(s8 *arg0, struct Sprite *arg1)
{
    s32 tr0;
    s32 tr5;

    if (arg1->counter <= 0) {
        *arg0 = 0;
        return;
    }
    arg1->counter--;
    tr0 = arg1->counter % (s32) ((2.0 * lbl_80198B34) / 15.0);
    tr5 = ((2.0 * lbl_80198B34) / 15.0);
    if (tr0 > (tr5 / 2)) {
        arg1->unk8C &= 0xFFFFFFFE;
    } else {
        arg1->unk8C |= 1;
    }
    if ((arg1->counter >= ((lbl_80198B38 * 2) - ((s32) ((2.0 * lbl_80198B34) / 15.0) * 6))) && (tr0 == ((s32) ((2.0 * lbl_80198B34) / 15.0) - 1))) {
        call_SoundReqID_arg_0(0x134);
    }
}

void fn_10_2078C(u8 arg0)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    float tf30;
    float f30;
    float f31;
    s32 tr30;
    s32 tr29;
    s32 tr28;
    struct Sprite *tr3;
    s8 r27;
    struct RacePlayer *tr4;
    struct RaceCheckpoint *tr7;

    tr4 = &lbl_10_bss_1230[arg0];
    tr7 = &tr4->unk48[tr4->unk44 - 1];
    tr29 = tr7->unk0 / lbl_80198B38;
    tr30 = tr29 / 60;
    tr29 %= 60;
    tr28 = (1000.0f * (tr7->unk4 + (float) (tr7->unk0 % lbl_80198B38))) / lbl_80198B34;
    tr29 += tr28 / 1000;
    tr28 %= 1000;
    tr28 /= 10;
    if (course->unk4 == (tr4->unk44 + 1)) {
        call_SoundReqID_arg_0(0x1E4);
        f31 = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + ((s32) (640.0f * cameras[arg0].sub28.viewportW) / 2));
        tf30 = (((s32) (480.0f * cameras[arg0].sub28.viewportY) + ((s32) (480.0f * cameras[arg0].sub28.viewportH) / 2)) - 10.0);
        tr3 = create_sprite();
        if (tr3 != NULL) {
            tr3->type = 0;
            tr3->fontId = 4;
            tr3->x = f31;
            tr3->y = (tf30 - 2.0);
            tr3->depth = 0.1f;
            tr3->scaleX = 1.0f;
            tr3->scaleY = 0.8f;
            tr3->textAlign = 5;
            tr3->flags = 0x01000000;
            tr3->widescreenArg = (s16) f31;
            tr3->mainFunc = fn_10_2051C;
            tr3->counter = lbl_80198B38 * 2;
            sprintf(tr3->text, "FINAL LAP");
        }
        f30 = tf30;
        f30 += 2.0;
        r27 = 3;
    } else {
        f31 = ((s32) (640.0f * cameras[arg0].sub28.viewportX) + ((s32) (640.0f * cameras[arg0].sub28.viewportW) / 2));
        f30 = ((s32) (480.0f * cameras[arg0].sub28.viewportY) + ((s32) (480.0f * cameras[arg0].sub28.viewportH) / 2));
        r27 = 4;
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 0;
        tr3->fontId = 0x4F;
        tr3->x = f31;
        tr3->y = f30;
        tr3->depth = 0.1f;
        tr3->textAlign = r27;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = (s16) f31;
        tr3->mainFunc = fn_10_205F4;
        tr3->counter = lbl_80198B38 * 2;
        sprintf(tr3->text, "%02d;%02d<%02d", tr30, tr29, tr28);
    }
}

void fn_10_20CA0(void)
{
    struct Sprite *tr3;

    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->bmpId = 0x720;
        tr3->scaleX = 0.8f;
        tr3->scaleY = 0.8f;
        tr3->x = (624.0 - (32.0 * tr3->scaleX));
        tr3->y = (256.0 + (32.0 * tr3->scaleY));
        tr3->depth = 0.2f;
        tr3->textAlign = 4;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = 0x270;
        sprintf(tr3->text, "LIVE BASE");
    }
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->type = 1;
        tr3->bmpId = 0x735;
        tr3->scaleX = 0.8f;
        tr3->scaleY = 0.8f;
        tr3->x = (624.0 - (32.0 * tr3->scaleX));
        tr3->y = (256.0 + (32.0 * tr3->scaleY));
        tr3->depth = 0.1f;
        tr3->mulR = 0xB0;
        tr3->mulG = 0xFF;
        tr3->mulB = 0xC0;
        tr3->textAlign = 4;
        tr3->flags = 0x01000000;
        tr3->widescreenArg = 0x270;
        tr3->mainFunc = fn_10_20E2C;
        tr3->drawFunc = fn_10_20E3C;
        sprintf(tr3->text, "LIVE STAR");
    }
}

void fn_10_20E2C(s8 *arg0, struct Sprite *arg1)
{
    arg1->rotation += 0x300;
}

void fn_10_20E3C(struct Sprite *arg0)
{
    struct SpriteDrawRequest req;
    float tf31;
    float tf28;
    float tf30;
    float f29;
    s16 r30;
    s16 r29;

    r29 = g_font_char_var_3 << 9;
    for (r30 = 0; r30 < 0xD; r30++) {
        r29 += 0x400;
        if ((r29 < 0x1800) || (r29 > 0x5800)) {
            if ((r29 > 0) && (r29 < 0x1800)) {
                f29 = (0x1800 - r29) / 6144.0f;
            } else if ((r29 > 0x5800) && (r29 < 0x7000)) {
                f29 = (r29 - 0x5800) / 6144.0f;
            } else {
                f29 = 1.0f;
            }
            tf28 = (r30 + 1) / 13.0f;
            f29 *= tf28;
            tf31 = (arg0->scaleX * (32.0 * math_sin((s16) (r29 + 0x4000))));
            tf30 = (arg0->scaleY * (6.0 * math_sin(r29)));
            req.scaleX = arg0->scaleX * tf28;
            req.scaleY = arg0->scaleY * tf28;
            req.u1 = req.v1 = 0.0f;
            req.u2 = req.v2 = 1.0f;
            req.rotation = 0;
            req.unk30 = -1;
            req.flags = 5;
            req.opacity = arg0->opacity * f29;
            req.mulColor = 0xFFFFFF;
            req.addColor = 0;
            req.flags = 0x20000A;
            req.rotation = arg0->rotation;
            req.mulColor = arg0->mulB | ((arg0->mulR << 0x10) | 0xFF000000 | (arg0->mulG << 8));
            req.bmpId = arg0->bmpId;
            f29 = tf30 * math_sin(0x6800);
            req.x = (arg0->x + (tf31 * math_sin(-0x5800))) - f29;
            f29 = tf30 * math_sin(-0x5800);
            {
                float y = arg0->y + tf31 * math_sin(0x6800);
                y += f29;
                req.y = y;
            }
            req.z = arg0->depth;
            draw_sprite_draw_request(&req);
        }
    }
}

char *lbl_10_data_1BA0[6] = {
    "CHARMING\nBANANA ROAD",
    "CHARMING\nBANANA ROAD",
    "CHARMING\nBANANA ROAD",
    "CHARMING\nBANANA ROAD",
    "CHARMING\nBANANA ROAD",
    "みわくのバナナロード",
};

char *lbl_10_data_1BE0[6] = {
    "LOVELY\nHEART RING",
    "LOVELY\nHEART RING",
    "LOVELY\nHEART RING",
    "LOVELY\nHEART RING",
    "LOVELY\nHEART RING",
    "ラブリーゆうえんち",
};

char *lbl_10_data_1C24[6] = {
    "CLOCK TOWER\nHILL",
    "CLOCK TOWER\nHILL",
    "CLOCK TOWER\nHILL",
    "CLOCK TOWER\nHILL",
    "CLOCK TOWER\nHILL",
    "チクタクはぐるまざか",
};

char *lbl_10_data_1C68[6] = {
    "CANNON BALL\nCIRCUIT",
    "CANNON BALL\nCIRCUIT",
    "CANNON BALL\nCIRCUIT",
    "CANNON BALL\nCIRCUIT",
    "CANNON BALL\nCIRCUIT",
    "アッチッチーサーキット",
};

char *lbl_10_data_1CAC[6] = {
    "STARLIGHT\nHIGHWAY",
    "STARLIGHT\nHIGHWAY",
    "STARLIGHT\nHIGHWAY",
    "STARLIGHT\nHIGHWAY",
    "STARLIGHT\nHIGHWAY",
    "スペースコロコロニー",
};

char *lbl_10_data_1CEC[6] = {
    "SUBMARINE\nSTREET",
    "SUBMARINE\nSTREET",
    "SUBMARINE\nSTREET",
    "SUBMARINE\nSTREET",
    "SUBMARINE\nSTREET",
    "ちんぼつストリート",
};

struct Race2CameraKey
{
    float unk0;
    float unk4;
    s32 unk8;
    float unkC;
};

struct Race2CameraKey lbl_10_data_1D04[14] = {
    { 0.0f, 1.1f, 0x0, 0.1f },
    { 9.3f, 1.1f, 0x0, 0.1f },
    { 9.4f, 1.1f, 0xF0000000, 0.1f },
    { 34.0f, 1.1f, 0xF0000000, 0.1f },
    { 37.5f, 1.1f, 0x0, 0.1f },
    { 42.0f, 1.1f, 0x8000000, 0.1f },
    { 56.0f, 1.1f, 0x0, 0.1f },
    { 66.0f, 0.6f, 0x20000000, 0.1f },
    { 71.0f, 0.6f, 0x20000000, 0.1f },
    { 71.099998f, 0.6f, 0xE0000000, 0.1f },
    { 82.5f, 1.1f, 0xF0000000, 0.1f },
    { 84.5f, 1.1f, 0x0, 0.1f },
    { 100.0f, 1.1f, 0x0, 0.1f },
    { -1.0f, 1.0f, 0x10000, 1.0f },
};

struct Race2CameraKey lbl_10_data_1DE4[16] = {
    { 0.0f, 1.02f, 0xFF000000, 0.1f },
    { 4.4f, 1.02f, 0x0, 0.1f },
    { 9.0f, 1.02f, 0xF0000000, 0.1f },
    { 34.400002f, 1.02f, 0xF0000000, 0.1f },
    { 37.599998f, 1.02f, 0x0, 0.1f },
    { 47.799999f, 1.02f, 0x0, 0.1f },
    { 51.799999f, 1.02f, 0xF0000000, 0.1f },
    { 64.0f, 1.02f, 0xF0000000, 0.1f },
    { 73.0f, 1.02f, 0x0, 0.1f },
    { 76.0f, 1.02f, 0xE8000000, 0.1f },
    { 89.699997f, 1.02f, 0xE0000000, 0.1f },
    { 90.0f, 1.02f, 0xE0000000, 0.1f },
    { 93.0f, 1.02f, 0xD0000000, 0.1f },
    { 95.0f, 1.02f, 0x0, 0.1f },
    { 100.0f, 1.02f, 0x0, 0.1f },
    { -1.0f, 1.0f, 0x10000, 1.0f },
};

struct Race2CameraKey lbl_10_data_1EE4[12] = {
    { 0.0f, 1.06f, 0x0, 0.1f },
    { 17.0f, 1.06f, 0x0, 0.1f },
    { 19.0f, 1.06f, 0xE0000000, 0.1f },
    { 31.0f, 1.06f, 0xE0000000, 0.1f },
    { 36.5f, 1.06f, 0x0, 0.1f },
    { 69.0f, 1.06f, 0x0, 0.1f },
    { 70.5f, 1.06f, 0xF0000000, 0.1f },
    { 77.0f, 1.06f, 0xF0000000, 0.1f },
    { 80.0f, 1.21f, 0x0, 0.1f },
    { 96.0f, 1.21f, 0x0, 0.1f },
    { 100.0f, 1.06f, 0x0, 0.1f },
    { -1.0f, 1.0f, 0x10000, 1.0f },
};

struct Race2CameraKey lbl_10_data_1FA4[12] = {
    { 0.0f, 1.0f, 0x0, 0.1f },
    { 17.0f, 1.0f, 0x0, 0.1f },
    { 19.0f, 1.0f, 0xE0000000, 0.1f },
    { 31.0f, 1.0f, 0xE0000000, 0.1f },
    { 36.5f, 1.0f, 0x0, 0.1f },
    { 69.0f, 1.0f, 0x0, 0.1f },
    { 70.5f, 1.0f, 0xF0000000, 0.1f },
    { 77.0f, 1.0f, 0xF0000000, 0.1f },
    { 80.0f, 1.15f, 0x0, 0.1f },
    { 96.0f, 1.15f, 0x0, 0.1f },
    { 100.0f, 1.0f, 0x0, 0.1f },
    { -1.0f, 1.0f, 0x10000, 1.0f },
};

struct Race2CameraKey lbl_10_data_2064[27] = {
    { 0.0f, 1.1f, 0x0, 0.1f },
    { 13.0f, 1.1f, 0x0, 0.1f },
    { 18.0f, 1.1f, 0x10000000, 0.1f },
    { 28.5f, 1.1f, 0x10000000, 0.1f },
    { 29.0f, 1.1f, 0x0, 0.1f },
    { 30.0f, 0.7f, 0x10000000, 0.1f },
    { 30.1f, 1.1f, 0x20000000, 0.1f },
    { 33.200001f, 1.1f, 0x20000000, 0.1f },
    { 35.5f, 1.1f, 0xF0000000, 0.1f },
    { 43.0f, 1.1f, 0xF0000000, 0.1f },
    { 43.700001f, 0.9f, 0x0, 0.1f },
    { 58.5f, 1.1f, 0x0, 0.1f },
    { 60.0f, 1.1f, 0xF0000000, 0.1f },
    { 64.0f, 1.1f, 0xF0000000, 0.1f },
    { 66.0f, 1.1f, 0x10000000, 0.1f },
    { 68.900002f, 1.1f, 0x10000000, 0.1f },
    { 70.0f, 1.1f, 0x0, 0.1f },
    { 73.800003f, 0.7f, 0x0, 0.1f },
    { 74.0f, 0.7f, 0x20000000, 0.1f },
    { 77.099998f, 0.7f, 0x20000000, 0.1f },
    { 85.400002f, 1.1f, 0xF8000000, 0.1f },
    { 94.099998f, 1.1f, 0x0, 0.1f },
    { 94.400002f, 1.1f, 0x20000000, 0.1f },
    { 96.800003f, 1.1f, 0x20000000, 0.1f },
    { 97.0f, 1.1f, 0xF8000000, 0.1f },
    { 100.0f, 1.1f, 0xF8000000, 0.1f },
    { -1.0f, 1.0f, 0x10000, 1.0f },
};

struct Race2CameraKey lbl_10_data_2214[27] = {
    { 0.0f, 1.0f, 0x0, 0.1f },
    { 13.0f, 1.0f, 0x0, 0.1f },
    { 18.0f, 1.0f, 0x10000000, 0.1f },
    { 28.5f, 1.0f, 0x10000000, 0.1f },
    { 29.0f, 1.0f, 0x0, 0.1f },
    { 30.0f, 0.6f, 0x10000000, 0.1f },
    { 30.1f, 1.0f, 0x20000000, 0.1f },
    { 33.200001f, 1.0f, 0x20000000, 0.1f },
    { 35.5f, 1.0f, 0xF0000000, 0.1f },
    { 43.0f, 1.0f, 0xF0000000, 0.1f },
    { 43.700001f, 0.8f, 0x0, 0.1f },
    { 58.5f, 1.0f, 0x0, 0.1f },
    { 60.0f, 1.0f, 0xF0000000, 0.1f },
    { 64.0f, 1.0f, 0xF0000000, 0.1f },
    { 66.0f, 1.0f, 0x10000000, 0.1f },
    { 68.900002f, 1.0f, 0x10000000, 0.1f },
    { 70.0f, 1.0f, 0x0, 0.1f },
    { 73.800003f, 0.6f, 0x0, 0.1f },
    { 74.0f, 0.6f, 0x20000000, 0.1f },
    { 77.099998f, 0.6f, 0x20000000, 0.1f },
    { 85.400002f, 1.0f, 0xF8000000, 0.1f },
    { 94.099998f, 1.0f, 0x0, 0.1f },
    { 94.400002f, 1.0f, 0x20000000, 0.1f },
    { 96.800003f, 1.0f, 0x20000000, 0.1f },
    { 97.0f, 1.0f, 0xF8000000, 0.1f },
    { 100.0f, 1.0f, 0xF8000000, 0.1f },
    { -1.0f, 1.0f, 0x10000, 1.0f },
};

struct Race2CameraKey lbl_10_data_23C4[17] = {
    { 0.0f, 1.1f, 0x0, 0.1f },
    { 7.0f, 1.1f, 0x0, 0.1f },
    { 8.2f, 1.1f, 0xF8000000, 0.1f },
    { 38.900002f, 1.1f, 0xF8000000, 0.1f },
    { 39.5f, 1.1f, 0x20000000, 0.1f },
    { 42.200001f, 1.1f, 0x20000000, 0.1f },
    { 43.0f, 1.1f, 0x0, 0.1f },
    { 44.299999f, 1.1f, 0x0, 0.1f },
    { 46.700001f, 1.1f, 0x8000000, 0.1f },
    { 84.400002f, 1.1f, 0x8000000, 0.1f },
    { 85.5f, 1.1f, 0x0, 0.1f },
    { 94.400002f, 1.1f, 0x0, 0.1f },
    { 95.0f, 1.1f, 0x20000000, 0.1f },
    { 97.5f, 1.1f, 0x20000000, 0.1f },
    { 98.099998f, 1.1f, 0x0, 0.1f },
    { 100.0f, 1.1f, 0x0, 0.1f },
    { -1.0f, 1.0f, 0x10000, 1.0f },
};

struct Race2CameraKey lbl_10_data_24D4[38] = {
    { 0.0f, 1.0f, 0x0, 0.1f },
    { 9.0f, 1.0f, 0x0, 0.1f },
    { 10.5f, 0.4f, 0x30000000, 0.1f },
    { 11.5f, 0.4f, 0x30000000, 0.1f },
    { 12.0f, 1.0f, 0x0, 0.1f },
    { 22.5f, 1.0f, 0xF0000000, 0.1f },
    { 23.200001f, 1.0f, 0xF0000000, 0.3f },
    { 24.0f, 1.0f, 0x0, 0.1f },
    { 36.0f, 1.0f, 0x0, 0.1f },
    { 36.5f, 1.0f, 0xE0000000, 0.1f },
    { 38.5f, 1.0f, 0xE0000000, 0.1f },
    { 39.0f, 1.0f, 0xF0000000, 0.1f },
    { 44.0f, 0.9f, 0xF0000000, 0.1f },
    { 44.5f, 1.0f, 0x0, 0.1f },
    { 51.0f, 1.0f, 0x0, 0.1f },
    { 51.099998f, 1.0f, 0x20000000, 0.1f },
    { 52.299999f, 1.0f, 0x20000000, 0.1f },
    { 53.0f, 1.0f, 0x10000000, 0.1f },
    { 58.5f, 1.0f, 0x10000000, 0.1f },
    { 59.0f, 1.0f, 0xE0000000, 0.1f },
    { 61.299999f, 1.0f, 0xE0000000, 0.1f },
    { 61.400002f, 1.0f, 0x0, 0.1f },
    { 83.5f, 1.0f, 0x0, 0.1f },
    { 84.0f, 1.0f, 0x20000000, 0.1f },
    { 84.800003f, 1.0f, 0x10000000, 0.1f },
    { 86.0f, 1.0f, 0x0, 0.1f },
    { 90.5f, 0.8f, 0x0, 0.1f },
    { 91.099998f, 0.8f, 0xE0000000, 0.1f },
    { 92.199997f, 0.8f, 0xE0000000, 0.1f },
    { 93.0f, 0.8f, 0x0, 0.1f },
    { 93.400002f, 0.8f, 0x20000000, 0.1f },
    { 94.300003f, 0.8f, 0x20000000, 0.1f },
    { 97.0f, 0.8f, 0x0, 0.1f },
    { 97.400002f, 0.8f, 0x20000000, 0.1f },
    { 98.0f, 0.8f, 0x20000000, 0.1f },
    { 98.099998f, 0.8f, 0x0, 0.1f },
    { 100.0f, 0.8f, 0x0, 0.1f },
    { -1.0f, 1.0f, 0x10000, 1.0f },
};

float lbl_10_data_2734[16] = {
    8.24f, 9.05f, 34.0f, 34.810001f,
    52.34f, 53.139999f, 75.199997f, 77.080002f,
    79.940002f, 81.099998f, 83.919998f, 84.410004f,
    91.230003f, 92.059998f, -1.0f, -1.0f,
};

float lbl_10_data_2774[42] = {
    2.88f, 3.9f, 25.299999f, 25.700001f,
    28.76f, 29.299999f, 32.360001f, 32.82f,
    36.060001f, 36.52f, 45.25f, 45.639999f,
    46.959999f, 47.459999f, 48.900002f, 49.450001f,
    50.73f, 51.240002f, 52.610001f, 53.009998f,
    54.259998f, 54.720001f, 56.0f, 56.509998f,
    74.080002f, 75.199997f, 78.860001f, 79.400002f,
    83.019997f, 83.57f, 87.580002f, 88.07f,
    90.190002f, 93.199997f, 93.910004f, 94.440002f,
    95.220001f, 95.699997f, 96.529999f, 96.980003f,
    -1.0f, -1.0f,
};

float lbl_10_data_281C[6] = {
    34.580002f, 37.07f, 70.360001f, 77.699997f,
    -1.0f, -1.0f,
};

float lbl_10_data_2834[26] = {
    1.05f, 1.61f, 18.58f, 18.870001f,
    22.299999f, 22.6f, 26.09f, 26.35f,
    29.77f, 30.059999f, 47.23f, 47.82f,
    68.470001f, 69.080002f, 73.910004f, 77.099998f,
    78.980003f, 82.019997f, 85.639999f, 86.25f,
    94.860001f, 96.029999f, 96.449997f, 98.019997f,
    -1.0f, -1.0f,
};

float lbl_10_data_289C[114] = {
    0.96f, 2.0f, 8.57f, 8.71f,
    10.37f, 10.5f, 12.14f, 12.3f,
    13.94f, 14.11f, 15.75f, 15.9f,
    17.530001f, 17.68f, 19.309999f, 19.48f,
    21.08f, 21.27f, 22.889999f, 23.07f,
    24.68f, 24.870001f, 26.469999f, 26.66f,
    28.280001f, 28.450001f, 30.07f, 30.25f,
    31.879999f, 32.040001f, 33.669998f, 33.830002f,
    35.470001f, 35.630001f, 37.259998f, 37.419998f,
    43.099998f, 43.240002f, 44.279999f, 44.25f,
    45.48f, 45.639999f, 46.689999f, 46.830002f,
    47.869999f, 48.040001f, 49.060001f, 49.220001f,
    50.27f, 50.43f, 50.459999f, 51.630001f,
    52.66f, 52.810001f, 53.860001f, 54.0f,
    55.02f, 55.209999f, 56.240002f, 56.400002f,
    57.439999f, 57.599998f, 58.630001f, 58.790001f,
    59.830002f, 59.990002f, 61.040001f, 61.189999f,
    62.220001f, 62.369999f, 63.43f, 63.57f,
    64.57f, 64.769997f, 65.790001f, 65.970001f,
    67.0f, 67.160004f, 68.199997f, 68.349998f,
    69.400002f, 69.550003f, 70.589996f, 70.739998f,
    71.769997f, 71.949997f, 72.970001f, 73.139999f,
    74.190002f, 74.339996f, 75.370003f, 75.529999f,
    76.57f, 76.720001f, 77.769997f, 77.919998f,
    78.949997f, 79.110001f, 80.150002f, 80.309998f,
    81.339996f, 81.510002f, 82.559998f, 82.699997f,
    83.739998f, 83.900002f, 84.940002f, 85.089996f,
    87.279999f, 87.599998f, 93.739998f, 93.910004f,
    -1.0f, -1.0f,
};

float lbl_10_data_2A64[30] = {
    2.08f, 2.68f, 10.28f, 11.6f,
    22.73f, 23.120001f, 36.220001f, 37.400002f,
    42.650002f, 43.330002f, 45.68f, 46.200001f,
    50.639999f, 52.470001f, 53.889999f, 54.200001f,
    58.52f, 60.669998f, 83.220001f, 84.279999f,
    87.660004f, 87.980003f, 90.599998f, 91.709999f,
    92.980003f, 94.18f, 97.209999f, 98.309998f,
    -1.0f, -1.0f,
};

struct Race2CourseInfo lbl_10_data_2ADC[] = {
    {
        lbl_10_data_1BA0, 131, 65, 10.0f, 4.0f, 0.0f,
        { 6, 5, 4, 3, 2, 1, 0 },
        { lbl_10_data_1D04, lbl_10_data_1D04, lbl_10_data_1D04, lbl_10_data_1D04, lbl_10_data_1D04, lbl_10_data_1D04, lbl_10_data_1D04 },
        lbl_10_data_2734, 1792, 4294967126, 120,
        0.4196721315383911f, 128.0f, 1.3f, 0.8f, 45, 25, 0.05f, 29956,
        { 41.762001f, 2.112f, -141.238007f },
    },
    {
        lbl_10_data_1BE0, 132, 65, 20.5f, 9.0f, 0.0f,
        { 6, 5, 4, 3, 2, 1, 0 },
        { lbl_10_data_1DE4, lbl_10_data_1DE4, lbl_10_data_1DE4, lbl_10_data_1DE4, lbl_10_data_1DE4, lbl_10_data_1DE4, lbl_10_data_1DE4 },
        lbl_10_data_2774, 1793, 4294967151, 145,
        0.4413793087005615f, 136.0f, 1.3f, 0.8f, 40, 20, 0.0f, 32139,
        { -1.187f, -35.402f, -77.921997f },
    },
    {
        lbl_10_data_1C24, 133, 69, 21.0f, 9.0f, -5.0f,
        { 6, 5, 4, 3, 2, 1, 0 },
        { lbl_10_data_1EE4, lbl_10_data_1FA4, lbl_10_data_1EE4, lbl_10_data_1FA4, lbl_10_data_1EE4, lbl_10_data_1FA4, lbl_10_data_1EE4 },
        lbl_10_data_281C, 1822, 4294967166, 190,
        0.8f, 392.0f, 0.6f, 0.4f, 45, 25, 0.02f, 24281,
        { -59.518002f, -76.023003f, -113.985001f },
    },
    {
        lbl_10_data_1C68, 134, 69, 12.0f, 4.0f, 0.0f,
        { 6, 5, 4, 3, 2, 1, 0 },
        { lbl_10_data_2064, lbl_10_data_2214, lbl_10_data_2064, lbl_10_data_2214, lbl_10_data_2064, lbl_10_data_2214, lbl_10_data_2064 },
        lbl_10_data_2834, 1842, 4294967143, 62,
        0.41830065846443176f, 144.0f, 1.3f, 0.8f, 60, 40, 0.05f, 42882,
        { -17.311001f, 20.608f, -190.341003f },
    },
    {
        lbl_10_data_1CEC, 136, 73, 7.0f, 2.5f, 0.0f,
        { 6, 5, 4, 3, 2, 1, 0 },
        { lbl_10_data_24D4, lbl_10_data_24D4, lbl_10_data_24D4, lbl_10_data_24D4, lbl_10_data_24D4, lbl_10_data_24D4, lbl_10_data_24D4 },
        lbl_10_data_2A64, 1844, 4294967118, 201,
        0.2969837486743927f, 136.0f, 1.3f, 0.8f, 70, 50, 0.0f, 26624,
        { 39.999001f, 4.364f, -18.676001f },
    },
    {
        lbl_10_data_1CAC, 135, 73, 18.0f, 2.0f, 0.0f,
        { 6, 5, 4, 3, 2, 1, 0 },
        { lbl_10_data_23C4, lbl_10_data_23C4, lbl_10_data_23C4, lbl_10_data_23C4, lbl_10_data_23C4, lbl_10_data_23C4, lbl_10_data_23C4 },
        lbl_10_data_289C, 1843, 4294967064, 232,
        0.27586206793785095f, 128.0f, 1.3f, 0.8f, 75, 55, 0.05f, 52627,
        { -204.343994f, -250.509995f, -168.059006f },
    },
};

char lbl_10_data_2D7C[13] = "WINSTAGE_1ST";
char lbl_10_data_2D8C[13] = "WINSTAGE_2ND";
char lbl_10_data_2D9C[13] = "WINSTAGE_3RD";
char lbl_10_data_2DAC[13] = "WINSTAGE_4TH";
char lbl_10_data_2DBC[13] = "WINSTAGE_5TH";
char lbl_10_data_2DCC[13] = "WINSTAGE_6TH";
char lbl_10_data_2DDC[13] = "WINSTAGE_7TH";
char lbl_10_data_2DEC[13] = "WINSTAGE_8TH";
