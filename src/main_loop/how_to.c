/* "How to play" sprite: the controls and rules pages for each game mode. */
#include "types.h"
#include "bitmap.h"
#include "input.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "sprite.h"
#include "textdraw.h"
#include "gx.h"

struct PauseMenuInput
{
    u8 unk0[0x14];
    /*0x14*/ s8 pad;
};

void textdraw_set_depth(float depth);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_mul_color(u32 color);
void textdraw_set_scale(float x, float y);
void textdraw_set_pos(float x, float y);
void textdraw_print_and_fit_to_width(float width, char *text);
void textdraw_printf_and_fit_to_width(float width, char *format, ...);
void textdraw_print(char *text);
void textdraw_printf(char *format, ...);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_flags(u32 flags);
void textdraw_set_alpha(float alpha);
void textdraw_set_font_style(int style);
void textdraw_set_drop_shadow(void);
void textdraw_clear_drop_shadow(void);
float textdraw_get_pixel_width_of_string(char *str);
void g_how_to_sprite_draw_controller_tooltips(int side, struct Sprite *sprite, struct SpriteDrawRequest *req);
void g_how_to_sprite_draw_rules_page(s8 page, struct Sprite *sprite, struct SpriteDrawRequest *req, s8 rows, s8 cols, s8 style, s8 sel);
void g_how_to_sprite_mtx_setup_stuff(void);
void g_how_to_sprite_mtx_cleanup(void);
void mtxa_translate_neg_xyz(float x, float y, float z);
void call_free_bmp_by_id(int grpId);
void GXLoadPosMtxImm(Mtx m, int id);

extern struct PauseMenuInput g_related_to_pause_menu_input;
extern s8 g_focused_root_menu[];
extern u8 lbl_1_bss_16D0[];
extern s8 lbl_1_bss_16F0[];
extern s8 lbl_1_bss_1700;
extern s8 lbl_1_bss_1704;
extern u16 lbl_1_bss_1708[];
extern s8 lbl_1_bss_1718[];
extern s8 lbl_1_bss_1720;
extern u8 lbl_1_bss_1738[];
extern s8 lbl_6_bss_C;
extern s32 locale;

/* Each of these is referenced first by create_how_to_sprite, in this order. */
static s32 s_mode;
static s8 s_cooldown;
static s8 s_page;
static s8 s_style;
static s8 s_timer;
static s8 s_shownPage;
static s8 s_titleIdx;
static float s_alpha;
static float s_unk10;
static float s_unk14;
static float s_unk18;
static float s_unk1C;
static float s_unk20;
static float s_unk24;
static float s_unk28;
static float s_unk2C;
static float s_unk30;
static float s_unk34;
static float s_unk38;
static float s_unk3C;
static s8 s_hold;
static float s_zoom;
static float s_panX;
static float s_panY;
static s8 s_pageCount;

u8 lbl_1_data_3C020[] = {
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
s8 lbl_1_data_3C190[] = {
    0x02, 0x00, 0x02, 0x04, 0x03, 0x03, 0x03, 0x02, 0x02, 0x04, 0x03, 0x00, 0x02, 0x03, 0x02, 0x04,
    0x02, 0x02, 0x00, 0x02, 0x06, 0x04, 0x03, 0x06, 0x06, 0x03, 0x04, 0x05, 0x00, 0x04, 0x04, 0x02,
    0x05, 0x04, 0x00, 0x00,
};

void sprite_how_to_tick(s8 *status, struct Sprite *sprite);
void sprite_how_to_disp(struct Sprite *sprite);
void sprite_how_to_dest(struct Sprite *sprite);
void call_SoundReqID_arg_1(int id);

void create_how_to_sprite(void)
{
    struct Sprite *sprite;

    s_mode = modeCtrl.gameType - 15;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 10;
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->textAlign = 0;
        sprite->depth = 0.002f;
        sprite->userVar = 0;
        sprite->fontId = 3;
        if (main_mode == 1)
            sprite->mainFunc = sprite_how_to_tick;
        sprite->drawFunc = sprite_how_to_disp;
        if (main_mode != 1)
            sprite->destFunc = sprite_how_to_dest;
        strcpy(sprite->text, "how to");
        s_cooldown = 15;
        s_page = 0;
        s_style = 0;
        s_timer = 0;
        s_shownPage = 0;
        s_titleIdx = 0;
        s_alpha = 1.0f;
        s_unk10 = 0.0f;
        s_unk14 = 1.0f;
        s_unk18 = 0.0f;
        s_unk1C = 0.0f;
        s_unk20 = 0.0f;
        s_unk24 = 0.0f;
        s_unk28 = 0.0f;
        s_unk2C = 0.0f;
        s_unk30 = 1.0f;
        s_unk34 = 0.0f;
        s_unk38 = 1.0f;
        s_unk3C = 1.0f;
        s_hold = 0;
        s_zoom = 1.0f;
        s_panX = 0.0f;
        s_panY = 0.0f;
    }
}

/* Button repeat on one pad, from either the buttons or the analog stick. */
#define HOWTO_REPEAT(i, b) \
    ((pad_status_groups[i].unk0[4].button & (b)) || (analog_inputs[i][4] & (b)) \
     || (((pad_status_groups[i].unk0[0].button & (b)) || (analog_inputs[i][0] & (b))) \
         && (analog_inputs[i][0] & 0x200)))
#define HOWTO_REPEAT_ANY(b) \
    (HOWTO_REPEAT(0, b) || HOWTO_REPEAT(1, b) || HOWTO_REPEAT(2, b) || HOWTO_REPEAT(3, b))
#define HOWTO_REPEAT_PAUSE(b) HOWTO_REPEAT(g_related_to_pause_menu_input.pad, b)
#define HOWTO_ANALOG_ANY(b) \
    ((analog_inputs[0][0] & (b)) || (analog_inputs[1][0] & (b)) \
     || (analog_inputs[2][0] & (b)) || (analog_inputs[3][0] & (b)))

void sprite_how_to_tick(s8 *status, struct Sprite *sprite)
{
    int changed = 0;
    int prev = s_page;
    int left, right, up, down, other;
    int hold;
    int active;
    float pan;

    s_pageCount = lbl_1_data_3C190[modeCtrl.gameType + (main_mode == 1 ? 17 : 0)];
    if (modeCtrl.gameType == 3 && (*(u32 *)(lbl_1_bss_16D0 + 8) & 0x10))
        s_pageCount = 2;
    else if (modeCtrl.gameType == 10 && (lbl_1_bss_1708[2] & 1))
        s_pageCount = 2;
    if (s_cooldown > 0)
        s_cooldown--;

    if (main_mode == 1)
    {
        if (HOWTO_REPEAT_ANY(1) && s_cooldown == 0)
        {
            if (--s_page < 0)
                s_page = (s8)(s_pageCount - 1);
            if (s_page != prev)
                call_SoundReqID_arg_1(0x169);
            s_cooldown = 15;
            changed = 1;
        }
        else if (HOWTO_REPEAT_ANY(2) && s_cooldown == 0)
        {
            if (++s_page == s_pageCount)
                s_page = 0;
            if (s_page != prev)
                call_SoundReqID_arg_1(0x169);
            s_cooldown = 15;
            changed = 1;
        }
    }
    else
    {
        if (HOWTO_REPEAT_PAUSE(1) && s_cooldown == 0)
        {
            if (--s_page < 0)
                s_page = (s8)(s_pageCount - 1);
            if (s_page != prev)
                call_SoundReqID_arg_1(0x169);
            s_cooldown = 15;
            changed = 1;
        }
        else if (HOWTO_REPEAT_PAUSE(2) && s_cooldown == 0)
        {
            if (++s_page == s_pageCount)
                s_page = 0;
            if (s_page != prev)
                call_SoundReqID_arg_1(0x169);
            s_cooldown = 15;
            changed = 1;
        }
    }

    s_style = s_page;
    s_titleIdx = s_page;
    switch (modeCtrl.gameType)
    {
    case 3:
        if (main_mode == 1)
        {
            if (s_page == 4)
            {
                s_titleIdx = 2;
                s_style = 2;
            }
            else if (s_page == 5)
            {
                s_titleIdx = 6;
                s_style = 3;
            }
            else if (s_page > 0)
            {
                s_titleIdx = (s8)(s_page + 2);
                s_style = 1;
                if (changed)
                    s_timer = 45;
            }
        }
        else
        {
            if (s_style == 1)
                s_titleIdx = (s8)(g_focused_root_menu[0xF] + 3);
            if (s_page == 3)
                s_titleIdx = 6;
        }
        break;
    case 4:
        if (main_mode == 1)
        {
            if (s_page == 3)
            {
                s_titleIdx = 2;
                s_style = 2;
            }
            else if (s_page > 0)
            {
                s_titleIdx = (s8)(s_page == 1 ? 1 : 3);
                s_style = 1;
                if (changed)
                    s_timer = 45;
            }
        }
        else
        {
            if (s_style == 1 && g_focused_root_menu[0x15] != 0)
                s_titleIdx = 3;
        }
        break;
    case 5:
        switch (s_page)
        {
        case 0:
            s_style = 0;
            break;
        case 1:
            s_style = 1;
            break;
        case 2:
            s_style = 2;
            break;
        case 3:
            s_style = 3;
            break;
        }
        break;
    case 6:
        switch (s_page)
        {
        case 0:
            s_style = 0;
            if (main_mode == 1 && changed)
                s_timer = 45;
            break;
        case 1:
            s_style = 4;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            s_style = 1;
            if (main_mode == 1)
            {
                s_titleIdx = s_page;
                if (changed)
                    s_timer = 45;
            }
            else if (lbl_1_bss_16F0[6] == 1)
                s_titleIdx = (s8)(lbl_6_bss_C + 2);
            else
                s_titleIdx = (s8)(lbl_1_bss_16F0[7] + 2);
            break;
        }
        break;
    case 7:
        switch (s_page)
        {
        case 0:
            s_style = 0;
            if (main_mode == 1 && changed)
                s_timer = 45;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            s_style = 1;
            if (main_mode == 1)
            {
                s_titleIdx = s_page;
                if (s_page == 2)
                    s_titleIdx = 4;
                else if (s_page == 3)
                    s_titleIdx = 5;
                else if (s_page == 4)
                    s_titleIdx = 3;
                else if (s_page == 5)
                    s_titleIdx = 2;
                if (changed)
                    s_timer = 45;
            }
            else
                s_titleIdx = (s8)(lbl_1_bss_1700 + 1);
            break;
        }
        break;
    case 8:
        switch (s_page)
        {
        case 0:
            s_style = 0;
            if (main_mode == 1 && changed)
                s_timer = 45;
            break;
        case 1:
        case 2:
            s_style = 1;
            if (main_mode == 1)
            {
                s_titleIdx = (s8)(int)s_page;
                if (changed)
                    s_timer = 45;
            }
            else
                s_titleIdx = (s8)(lbl_1_bss_1704 + 1);
            break;
        }
        break;
    case 10:
        if (main_mode == 1)
        {
            if (s_page == 4)
            {
                s_titleIdx = 2;
                s_style = 2;
            }
            else if (s_page > 0)
            {
                s_titleIdx = (s8)(s_page + 2);
                s_style = 1;
                if (changed)
                    s_timer = 45;
            }
        }
        else
        {
            if (s_style == 1)
                s_titleIdx = (s8)(g_focused_root_menu[0x2B] + 3);
        }
        break;
    case 12:
        switch (s_page)
        {
        case 0:
            s_style = 0;
            if (main_mode == 1 && changed)
                s_timer = 45;
            break;
        case 1:
        case 2:
        case 3:
            s_style = 1;
            if (main_mode == 1)
            {
                if (changed)
                    s_timer = 45;
            }
            else
                s_titleIdx = (s8)(lbl_1_bss_1720 + 1);
            break;
        }
        break;
    case 13:
        if (main_mode == 1)
        {
            if (s_page == 3)
            {
                s_titleIdx = 2;
                s_style = 2;
            }
            else if (s_page > 0)
            {
                s_titleIdx = (s8)(s_page == 1 ? 1 : 3);
                s_style = 1;
                if (changed)
                    s_timer = 45;
            }
        }
        else
        {
            if (s_style == 1 && lbl_1_bss_1718[1] != 0)
                s_titleIdx = 3;
        }
        break;
    case 15:
        if (s_page > 0)
        {
            s_style = (s8)(s_page == 3 ? 2 : (s_page == 4 ? 3 : 1));
            if (changed)
                s_timer = 45;
        }
        break;
    case 16:
        if (s_page > 0)
        {
            s_style = 1;
            if (changed)
                s_timer = 45;
        }
        break;
    }

    if (s_timer > 0)
        s_timer--;
    if (s_timer == 0)
        s_shownPage = s_page;

    switch (sprite->userVar)
    {
    case 0:
        s_alpha += 0.1 * -s_alpha;
        s_unk10 += 0.1 * (100.0 - s_unk10);
        {
            double unused = 80.0;
            if (s_unk10 > 79.0)
                s_unk14 += 0.1 * (80.0 - s_unk14);
        }
        if (s_style == 0 || s_style == 4)
            s_unk18 += 0.1 * (1.0 - s_unk18);
        else if (s_unk1C < 0.1 && s_unk24 < 0.1)
            s_unk18 += 0.1 * -s_unk18;
        if (s_style == 0)
        {
            if (s_unk18 > 0.95 && s_unk24 < 0.01)
                s_unk1C += 0.2 * (1.0 - s_unk1C);
            if (s_unk1C > 0.99)
                s_unk20 += 0.1 * (1.0 - s_unk20);
        }
        else
        {
            s_unk20 += 0.3 * -s_unk20;
            if (s_unk20 < 0.01)
                s_unk1C += 0.3 * -s_unk1C;
        }
        if (s_style == 4)
        {
            if (s_unk18 > 0.95 && s_unk1C < 0.01)
                s_unk24 += 0.2 * (1.0 - s_unk24);
            if (s_unk24 > 0.99)
                s_unk28 += 0.1 * (1.0 - s_unk28);
        }
        else
        {
            s_unk28 += 0.3 * -s_unk28;
            if (s_unk28 < 0.01)
                s_unk24 += 0.3 * -s_unk24;
        }
        if (s_style == 1 && s_timer == 0)
        {
            if (s_unk1C < 0.01 && s_unk24 < 0.01)
                s_unk30 += 0.2 * -s_unk30;
            if (s_unk30 < 0.01)
                s_unk2C += 0.2 * (1.0 - s_unk2C);
            if (s_unk2C > 0.99)
                s_unk34 += 0.1 * (1.0 - s_unk34);
        }
        else
        {
            s_unk34 += 0.3 * -s_unk34;
            if (s_unk34 < 0.01)
                s_unk2C += 0.3 * -s_unk2C;
            if (s_unk2C < 0.01)
                s_unk30 += 0.1 * (1.0 - s_unk30);
        }
        if (s_style == 2)
        {
            if (s_unk2C < 0.01 && s_unk18 < 0.1)
                s_unk38 += 0.1 * -s_unk38;
        }
        else
            s_unk38 += 0.1 * (1.0 - s_unk38);
        if (s_style == 3)
        {
            if (s_unk18 < 0.1)
                s_unk3C += 0.1 * -s_unk3C;
        }
        else
            s_unk3C += 0.1 * (1.0 - s_unk3C);
        break;
    case 1:
        s_alpha += 0.1 * (1.0 - s_alpha);
        break;
    }

    if (main_mode == 1)
    {
        left = HOWTO_ANALOG_ANY(0x10);
        right = HOWTO_ANALOG_ANY(0x20);
        up = HOWTO_ANALOG_ANY(0x80);
        down = HOWTO_ANALOG_ANY(0x40);
        other = HOWTO_ANALOG_ANY(0x100);
    }
    else
    {
        left = (analog_inputs[g_related_to_pause_menu_input.pad][0] & 0x10) != 0;
        right = (analog_inputs[g_related_to_pause_menu_input.pad][0] & 0x20) != 0;
        up = (analog_inputs[g_related_to_pause_menu_input.pad][0] & 0x80) != 0;
        down = (analog_inputs[g_related_to_pause_menu_input.pad][0] & 0x40) != 0;
        other = (analog_inputs[g_related_to_pause_menu_input.pad][0] & 0x100) != 0;
    }

    active = 0;
    hold = s_hold;
    if (left || right || up || down || other)
    {
        if (++hold > 30)
            hold = 30;
        active = 1;
    }
    else
    {
        if (--hold < 0)
            hold = 0;
        if (hold >= 30)
            active = 1;
    }
    s_hold = (s8)hold;
    s_zoom = (float)(hold < 30 ? hold : 30) / 30.0f;

    pan = s_panX;
    if (left)
    {
        pan -= 1.0;
        if (pan < -30.0)
            pan = -30.0f;
    }
    else if (right)
    {
        pan += 1.0;
        if (pan > 30.0)
            pan = 30.0f;
    }
    else if (!active)
        pan -= pan / (float)(hold + 1);
    s_panX = pan;

    pan = s_panY;
    if (up)
    {
        pan -= 1.0;
        if (pan < -30.0)
            pan = -30.0f;
    }
    else if (down)
    {
        pan += 1.0;
        if (pan > 30.0)
            pan = 30.0f;
    }
    else if (!active)
        pan -= pan / (float)(hold + 1);
    s_panY = pan;
}

char * lbl_1_data_3C260[] = {
    "Challenge Mode controls",
    "",
    "",
    "",
    "",
    "チャレンジモードのそうさ",
    "Challenge Mode Rules",
    "",
    "",
    "",
    "",
    "チャレンジモードのルール",
};
char * lbl_1_data_3C2F4[] = {
    "Practice Mode controls",
    "",
    "",
    "",
    "",
    "プラクティスモードのそうさ",
    "Practice Mode Rules",
    "",
    "",
    "",
    "",
    "プラクティスモードのルール",
};
char * lbl_1_data_3C540[] = {
    "Monkey Race 2 controls",
    "",
    "",
    "",
    "",
    "モンキーレース２のそうさ",
    "One Course Race Rules",
    "Spielregeln Einzelkurse",
    "Expl. Circuit Unique",
    "Reg. Carr. 1 Circuito",
    "Regole corsa singola",
    "１コースレースのルール",
    "About Items (Part1)",
    "",
    "",
    "",
    "",
    "アイテムについて（その１）",
    "One Course Race Rules",
    "Spielregeln Einzelkurse",
    "Expl. Circuit Unique",
    "Reg. Carr. 1 Circuito",
    "Regole corsa singola",
    "１コースレースのルール",
    "Grand Prix Rules",
    "Grand-Prix-Regeln",
    "Expl. Grand Prix",
    "Reg. Grand Prix ",
    "Regole grand prix",
    "グランプリのルール",
    "Time Attack Rules",
    "Zeitangriff-Regeln",
    "Expl. mode Chrono",
    "Reg. Contrarreloj",
    "Regole sfida a tempo",
    "タイムアタックのルール",
    "About Items (Part2)",
    "",
    "",
    "",
    "",
    "アイテムについて（その２）",
};
char * lbl_1_data_3C6C4[] = {
    "Monkey Fight 2 controls",
    "",
    "",
    "",
    "",
    "モンキーファイト２のそうさ",
    "Normal Mode rules",
    "",
    "",
    "",
    "",
    "ノーマルモードのルール",
    "About Items",
    "Objektinfo",
    "Les objets",
    "Acerca Obj.",
    "Oggetti",
    "アイテムについて",
    "Survival Mode Rules",
    "",
    "",
    "",
    "",
    "サバイバルモードのルール",
};
char * lbl_1_data_3C798[] = {
    "Monkey Target 2 controls",
    "",
    "",
    "",
    "",
    "モンキーターゲット２のそうさ",
    "Monkey Target 2 rules",
    "",
    "",
    "",
    "",
    "モンキーターゲット２のルール",
    "About Items",
    "Objektinfo",
    "Les objets",
    "Acerca Obj.",
    "Oggetti",
    "アイテムについて",
};
char * lbl_1_data_3C98C[] = {
    "View Mode controls",
    "Ansichtssteuerung",
    "Comm. mode Vue",
    "Control Md. Vista",
    "Comandi visuale",
    "ビューモードのそうさ",
    "Shot Mode Controls",
    "Schussmodussteuerung",
    "Comm. mode de Tir",
    "Control Md. Tiro",
    "Comandi mod. tiro",
    "ショットモードのそうさ",
    "US Nine-ball Rules",
    "",
    "",
    "",
    "",
    "ＵＳナインボールのルール",
    "JAPAN Nine-ball Rules",
    "",
    "",
    "",
    "",
    "ＪＰＮナインボールのルール",
    "Rotation Rules",
    "",
    "",
    "",
    "",
    "ローテーションのルール",
    "Eight-ball Rules",
    "",
    "",
    "",
    "",
    "エイトボールのルール",
};
char * lbl_1_data_3CBB8[] = {
    "Monkey Bowling 2 controls",
    "",
    "",
    "",
    "",
    "モンキーボーリング２のそうさ",
    "Normal Mode \x81`Normal\x81` Rules",
    "",
    "",
    "",
    "",
    "ノーマルモード ～ノーマル～ のルール",
    "Challenge Mode Rules",
    "Regeln Herausforder.",
    "Expl. mode Challenge",
    "Reglas Md. Desaf\xEDo  ",
    "Regole mod. sfida",
    "チャレンジモードのルール",
    "Special Mode Rules",
    "",
    "",
    "",
    "",
    "スペシャルモードのルール",
    "Normal Mode \x81`9 pins\x81` Rules",
    "",
    "",
    "",
    "",
    "ノーマルモード ～9ピンズ～ のルール",
    "Normal Mode \x81`Strike\x81` Rules",
    "",
    "",
    "",
    "",
    "ノーマルモード ～ストライク～ のルール",
};
char * lbl_1_data_3CD68[] = {
    "Monkey Golf 2 controls",
    "",
    "",
    "",
    "",
    "モンキーゴルフ２のそうさ",
    "Stroke Play rules",
    "Regeln Z\xE4hlspiel",
    "Expl. Mode Stroke",
    "Reg. Juego Ataque",
    "Regole gioco a tiri",
    "ストロークプレイのルール",
    "Match Play rules",
    "Regeln Lochspiel",
    "Expl. Match",
    "Reg. Match Play",
    "Regole scontro",
    "マッチプレイのルール",
};
char * lbl_1_data_3CE14[] = {
    "Monkey Shot controls",
    "",
    "",
    "",
    "",
    "モンキーショットのそうさ",
    "Monkey Shot rules",
    "",
    "",
    "",
    "",
    "モンキーショットのルール",
    "About Items (Part1)",
    "Objektinfo",
    "Les objets",
    "Acerca Obj.",
    "Oggetti",
    "アイテムについて（その１）",
    "About Items (Part2)",
    "",
    "",
    "",
    "",
    "アイテムについて（その２）",
};
char * lbl_1_data_3CEA4[] = {
    "Monkey Boat controls",
    "",
    "",
    "",
    "",
    "モンキーボートのそうさ",
    "One Course Race Rules",
    "Spielregeln Einzelkurse",
    "Expl. Circuit Unique",
    "Reg. Carr. 1 Circuito",
    "Regole corsa singola",
    "１コースレースのルール",
    "About Items",
    "",
    "",
    "",
    "",
    "アイテムについて",
    "One Course Race Rules",
    "Spielregeln Einzelkurse",
    "Expl. Circuit Unique",
    "Reg. Carr. 1 Circuito",
    "Regole corsa singola",
    "１コースレースのルール",
    "Grand Prix Rules",
    "Grand-Prix-Regeln",
    "Expl. Grand Prix",
    "Reg. Grand Prix ",
    "Regole grand prix",
    "グランプリのルール",
    "Time Attack Rules",
    "Zeitangriff-Regeln",
    "Expl. mode Chrono",
    "Reg. Contrarreloj",
    "Regole sfida a tempo",
    "タイムアタックのルール",
};
char * lbl_1_data_3CFF8[] = {
    "Monkey Soccer controls",
    "",
    "",
    "",
    "",
    "モンキーフットサルのそうさ",
    "Exhibition Mode Rules",
    "",
    "",
    "",
    "",
    "エキシビジョンモードのルール",
    "Tournament Mode Rules",
    "",
    "",
    "",
    "",
    "トーナメントモードのルール",
    "PK Mode Rules",
    "",
    "",
    "",
    "",
    "ＰＫモードのルール",
};
char * lbl_1_data_3D094[] = {
    "Monkey Dogfight controls",
    "",
    "",
    "",
    "",
    "モンキードッグファイトのそうさ",
    "Normal Mode rules",
    "",
    "",
    "",
    "",
    "ノーマルモードのルール",
    "About Items",
    "",
    "",
    "",
    "",
    "アイテムについて",
    "Survival Mode Rules",
    "",
    "",
    "",
    "",
    "サバイバルモードのルール",
};
char * lbl_1_data_3D154[] = {
    "Story Mode controls",
    "",
    "",
    "",
    "",
    "ストーリーモード のそうさ",
    "Story Mode Rules",
    "",
    "",
    "",
    "",
    "ストーリーモードのルール",
};
char * lbl_1_data_3D230[] = {
    "Monkey Baseball controls",
    "",
    "",
    "",
    "",
    "モンキーベースボールのそうさ",
    "Exhibition Mode Rules",
    "",
    "",
    "",
    "",
    "エキシビジョンモードのルール",
    "Tournament Mode Rules",
    "",
    "",
    "",
    "",
    "トーナメントモードのルール",
    "About the Wheel (Part1)",
    "",
    "",
    "",
    "",
    "ルーレットについて（その１）",
    "About the Wheel (Part2)",
    "",
    "",
    "",
    "",
    "ルーレットについて（その２）",
};
char * lbl_1_data_3D334[] = {
    "Monkey Tennis Controls",
    "",
    "",
    "",
    "",
    "モンキーテニスのそうさ",
    "Singles Mode Rules",
    "",
    "",
    "",
    "",
    "シングルスモードのルール",
    "Doubles Mode Rules",
    "",
    "",
    "",
    "",
    "ダブルスモードのルール",
    "Tournament Mode Rules",
    "",
    "",
    "",
    "",
    "トーナメントモードのルール",
};
void * lbl_1_data_3D394[] = {
    lbl_1_data_3C260,
    0,
    lbl_1_data_3C2F4,
    lbl_1_data_3C540,
    lbl_1_data_3C6C4,
    lbl_1_data_3C798,
    lbl_1_data_3C98C,
    lbl_1_data_3CBB8,
    lbl_1_data_3CD68,
    lbl_1_data_3CE14,
    lbl_1_data_3CEA4,
    0,
    lbl_1_data_3CFF8,
    lbl_1_data_3D094,
    lbl_1_data_3D154,
    lbl_1_data_3D230,
    lbl_1_data_3D334,
};
u32 lbl_1_data_3D3D8[] = {
    0x00000C24, 0x0005001A, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C23, 0x0005001A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000C26, 0x0005001A, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000C22, 0x0005001A, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C25, 0x0005001A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000C19, 0x0005001A, 0x00310000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000C1A, 0x0005001A, 0x00310000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C1C, 0x0005001A,
    0x002C0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000C1B, 0x0005001A, 0x002C0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000C1E, 0x0005001A, 0x002C0000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C1D, 0x0005001A,
    0x002C0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000C29, 0x0005001A, 0x002C0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000C20, 0x0005001A, 0x00310043, 0x00550000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C1F, 0x0005001A,
    0x002C0043, 0x00550000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000C21, 0x0005001A, 0x002C0043, 0x00550067,
    0x00790000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000C34, 0x0005001A, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C32, 0x0005001A,
    0x002C0043, 0x00550067, 0x0079008B, 0x00A200B4, 0x00C60000, 0x00000000,
    0x00000000, 0x00000000, 0x00000C36, 0x0005001A, 0x002C0043, 0x00550000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000C37, 0x0005001A, 0x002C0043, 0x00550000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C38, 0x00000005,
    0x00140000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000C43, 0x0005001A, 0x002C003E, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000C29, 0x00050016, 0x00230030, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C1F, 0x00050016,
    0x00230030, 0x003F004C, 0x00590000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000C21, 0x00050016, 0x00230030, 0x003F004E,
    0x005D006C, 0x007B0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000C40, 0x0005001A, 0x002C003E, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000C44, 0x0005001B,
    0x002C003D, 0x00570068, 0x0079008A, 0x009B00AC, 0x00BD00CE, 0x00EC00FD,
    0x010E011F, 0x01300141,
};
char * lbl_1_data_3D808[] = {
    "Control Stick",
    "Roll the ball.",
    0,
};
char * lbl_1_data_3D824[] = {
    "Control Stick",
    "Kugel rollen",
    0,
};
char * lbl_1_data_3D854[] = {
    "Stick direct.",
    "Faire rouler boule",
    0,
};
char * lbl_1_data_3D884[] = {
    "Stick de Control",
    "Rueda la bola",
    0,
};
char * lbl_1_data_3D89C[] = {
    "Control Stick",
    "Muovi palla",
    0,
};
char * lbl_1_data_3D8D4[] = {
    "コントロールスティック",
    "ボールをコロがす",
    0,
};
char * lbl_1_data_3D8F8[] = {
    "START/PAUSE",
    "Pause Menu",
    0,
};
char * lbl_1_data_3D910[] = {
    "START/PAUSE",
    "Pausenmen\xFC",
    0,
};
char * lbl_1_data_3D928[] = {
    "START/PAUSE",
    "Menu pause",
    0,
};
char * lbl_1_data_3D940[] = {
    "START/PAUSE",
    "Men\xFA Pausa",
    0,
};
char * lbl_1_data_3D958[] = {
    "START/PAUSE",
    "Menu pausa",
    0,
};
char * lbl_1_data_3D984[] = {
    "スタートボタン",
    "ポーズメニュー",
    0,
};
char * lbl_1_data_3D9C4[] = {
    "A Button",
    "Adjust the size of",
    "the on-screen map",
    0,
};
char * lbl_1_data_3D9FC[] = {
    "A-Knopf",
    "Gr\xF6\xDF" "e der",
    "Karte einstellen.",
    0,
};
char * lbl_1_data_3DA3C[] = {
    "Bouton A",
    "R\xE9glage de la",
    "taille de la carte.",
    0,
};
char * lbl_1_data_3DA74[] = {
    "Bot\xF3n A",
    "Ajusta el tama\xF1o",
    "del mapa.",
    0,
};
char * lbl_1_data_3DAB8[] = {
    "Pulsante A",
    "Ridimensiona la",
    "mappa sullo schermo.",
    0,
};
char * lbl_1_data_3DAFC[] = {
    "Ａボタン",
    "マップひょうじの",
    "おおきさをかえる",
    0,
};
char * lbl_1_data_3DB18[] = {
    "A Button",
    "Use Item.",
    0,
};
char * lbl_1_data_3DB34[] = {
    "A-Knopf",
    "Objekt nutzen",
    0,
};
char * lbl_1_data_3DB48[] = {
    "Bouton A",
    "Objet",
    0,
};
char * lbl_1_data_3DB60[] = {
    "Bot\xF3n A",
    "Usa Obj.",
    0,
};
char * lbl_1_data_3DB78[] = {
    "Pulsante A",
    "Usa oggetto",
    0,
};
char * lbl_1_data_3DB98[] = {
    "Ａボタン",
    "アイテムをつかう",
    0,
};
char * lbl_1_data_3DBCC[] = {
    "A Button",
    "Punch in the direction",
    "you are facing.",
    0,
};
char * lbl_1_data_3DBDC[] = {
    "A-Knopf",
    "",
    "",
    0,
};
char * lbl_1_data_3DBEC[] = {
    "Bouton A",
    "",
    "",
    0,
};
char * lbl_1_data_3DBFC[] = {
    "Bot\xF3n A",
    "",
    "",
    0,
};
char * lbl_1_data_3DC0C[] = {
    "Pulsante A",
    "",
    "",
    0,
};
char * lbl_1_data_3DC3C[] = {
    "Ａボタン",
    "むいているほうに",
    "パンチする",
    0,
};
char * lbl_1_data_3DCE4[] = {
    "Control Stick",
    "/bcff0000/-Before taking flight-",
    "Roll the ball down the ramp.",
    "/bcff0000/-During flight-",
    "Push up to dive",
    "Pull down to climb",
    "Left/right to turn",
    0,
};
char * lbl_1_data_3DDA0[] = {
    "Control Stick",
    "/bcff0000/-Bevor du abhebst-",
    "Rolle die Kugel die Rampe runter",
    "/bcff0000/-Im Flug-",
    "Oben verringert H\xF6he.",
    "Unten vergr\xF6\xDF" "ert H\xF6he.",
    "Links/Rechts lenkt.",
    0,
};
char * lbl_1_data_3DE48[] = {
    "Stick direct.",
    "/bcff0000/-Avant l'envol-",
    "Rouler sur le tremplin",
    "/bcff0000/-En vol-",
    "Haut pour descendre",
    "Bas pour monter",
    "Gauche/Droite pour tourner",
    0,
};
char * lbl_1_data_3DF00[] = {
    "Stick de Control",
    "/bcff0000/-Antes de despegar-",
    "Rueda la bola rampa abajo",
    "/bcff0000/-En vuelo-",
    "Arriba para ir abajo.",
    "Abajo para ir arriba.",
    "Izq./Dch. girar\xE1.",
    0,
};
char * lbl_1_data_3DFC4[] = {
    "Control Stick",
    "/bcff0000/-Prima di volare-",
    "Spingi la palla gi\xF9 dalla rampa",
    "/bcff0000/-Durante il volo-",
    "Premi su per andare gi\xF9.",
    "Premi gi\xF9 per andare su.",
    "Sx/dx per girare.",
    0,
};
char * lbl_1_data_3E05C[] = {
    "コントロールスティック",
    "/bcff0000/\x81`\x82\xC6\x82\xD4\x82\xDC\x82\xA6\x81`",
    "ボールをコロがす",
    "/bcff0000/～とんでいるとき～",
    "●うえで さげる",
    "●したで あげる",
    "●みぎ・ひだりでせんかいする",
    0,
};
char * lbl_1_data_3E0A8[] = {
    "A Button",
    "1. Expand wings.",
    "2. Go back to the ball.",
    0,
};
char * lbl_1_data_3E0B8[] = {
    "A-Knopf",
    "",
    "",
    0,
};
char * lbl_1_data_3E0C8[] = {
    "Bouton A",
    "",
    "",
    0,
};
char * lbl_1_data_3E0D8[] = {
    "Bot\xF3n A",
    "",
    "",
    0,
};
char * lbl_1_data_3E0E8[] = {
    "Pulsante A",
    "",
    "",
    0,
};
char * lbl_1_data_3E120[] = {
    "Ａボタン",
    "１．ハネをひろげる",
    "２．ボールにもどる",
    0,
};
char * lbl_1_data_3E160[] = {
    "Control Stick",
    "Press left or right to",
    "change the direction.",
    0,
};
char * lbl_1_data_3E19C[] = {
    "Control Stick",
    "Dr\xFC" "cke Links/Rechts",
    "zum Richtungswechsel.",
    0,
};
char * lbl_1_data_3E1D0[] = {
    "Stick direct.",
    "Gauche/Droite pour",
    "la direction.",
    0,
};
char * lbl_1_data_3E208[] = {
    "Stick de Control",
    "Pulsa dch. o izq.",
    "para cambiar dir.",
    0,
};
char * lbl_1_data_3E244[] = {
    "Control Stick",
    "Cambia direzione con",
    "destra o sinistra.",
    0,
};
char * lbl_1_data_3E27C[] = {
    "コントロールスティック",
    "みぎ・ひだりでうつ",
    "ほうこうをかえる",
    0,
};
char * lbl_1_data_3E2D0[] = {
    "A Button",
    "Once you choose an angle,",
    "it will change to /bcff0000/shot mode.",
    0,
};
char * lbl_1_data_3E324[] = {
    "A-Knopf",
    "Wenn du eine Richtung gew\xE4hlt hast,",
    "bist du im /bcff0000/Sto\xDFmodus.",
    0,
};
char * lbl_1_data_3E374[] = {
    "Bouton A",
    "Une fois la direction r\xE9gl\xE9" "e,",
    "la vue passe en mode de tir.",
    0,
};
char * lbl_1_data_3E3BC[] = {
    "Bot\xF3n A",
    "Cuando decidas una direcci\xF3n,",
    "cambiar\xE1 al modo tiro.",
    0,
};
char * lbl_1_data_3E404[] = {
    "Pulsante A",
    "Decisa la direzione,",
    "ti troverai in modalit\xE0 tiro.",
    0,
};
char * lbl_1_data_3E458[] = {
    "Ａボタン",
    "ほうこうをけっていして",
    "/bcff0000/\x83V\x83\x87\x83" "b\x83g\x83\x82\x81[\x83h/bc000000/\x82\xC9\x82\xB7\x82\xE9",
    0,
};
char * lbl_1_data_3E498[] = {
    "B Button",
    "Return to previous",
    "camera view.",
    0,
};
char * lbl_1_data_3E4C8[] = {
    "B-Knopf",
    "Zur\xFC" "ck zur Kamerasicht",
    0,
};
char * lbl_1_data_3E4F8[] = {
    "Bouton B",
    "Revenir \xE0 vue cam\xE9ra",
    0,
};
char * lbl_1_data_3E524[] = {
    "Bot\xF3n B",
    "Regresar a vista c\xE1m.",
    0,
};
char * lbl_1_data_3E558[] = {
    "Pulsante B",
    "Torna a visuale telecamera",
    0,
};
char * lbl_1_data_3E588[] = {
    "Ｂボタン",
    "カメラしてんをもどす",
    0,
};
char * lbl_1_data_3E5AC[] = {
    "C Stick",
    "Camera movement",
    0,
};
char * lbl_1_data_3E5D0[] = {
    "C-Stick",
    "Kamerabewegung",
    0,
};
char * lbl_1_data_3E5F4[] = {
    "Stick C",
    "Mouv. cam\xE9ra",
    0,
};
char * lbl_1_data_3E60C[] = {
    "Stick C",
    "Mov. c\xE1mara",
    0,
};
char * lbl_1_data_3E62C[] = {
    "Stick C",
    "Movimenti visuale",
    0,
};
char * lbl_1_data_3E658[] = {
    "Ｃスティック",
    "カメラいどう",
    0,
};
char * lbl_1_data_3E67C[] = {
    "X Button",
    "Ball number",
    0,
};
char * lbl_1_data_3E69C[] = {
    "X-Knopf",
    "Kugelnummer",
    0,
};
char * lbl_1_data_3E6C0[] = {
    "Bouton X",
    "N\xB0 boule",
    0,
};
char * lbl_1_data_3E6E0[] = {
    "Bot\xF3n X",
    "N\xFAmero Bola",
    0,
};
char * lbl_1_data_3E704[] = {
    "Pulsante X",
    "N. palla",
    0,
};
char * lbl_1_data_3E730[] = {
    "Ｘボタン",
    "ボールのばんごう",
    0,
};
char * lbl_1_data_3E774[] = {
    "Y Button",
    "Change between",
    "2D top view and 3D view.",
    0,
};
char * lbl_1_data_3E7B8[] = {
    "Y-Knopf",
    "Zwischen 2D- und",
    "3D-Sicht umschalten.",
    0,
};
char * lbl_1_data_3E7F8[] = {
    "Bouton Y",
    "Changer entre",
    "vue 2D et vue 3D.",
    0,
};
char * lbl_1_data_3E83C[] = {
    "Bot\xF3n Y",
    "Cambia entre vista",
    "3D y vista 2D a\xE9rea.",
    0,
};
char * lbl_1_data_3E87C[] = {
    "Pulsante Y",
    "Alterna visuale",
    "dall'alto 2D e 3D.",
    0,
};
char * lbl_1_data_3E8C4[] = {
    "Ｙボタン",
    "２Ｄトップビューと",
    "３Ｄビューのきりかえ",
    0,
};
char * lbl_1_data_3E8FC[] = {
    "Control Stick",
    "Change the shot point",
    "of the ball.",
    0,
};
char * lbl_1_data_3E93C[] = {
    "Control Stick",
    "Du kannst den Kontaktpunkt",
    "der Kugel \xE4ndern.",
    0,
};
char * lbl_1_data_3E980[] = {
    "Stick direct.",
    "Tu peux changer le point",
    "d'impact de ta boule.",
    0,
};
char * lbl_1_data_3E9B8[] = {
    "Stick de Control",
    "Puedes cambiar los",
    "puntos de tu bola.",
    0,
};
char * lbl_1_data_3E9F0[] = {
    "Control Stick",
    "Puoi cambiare i",
    "punti della tua palla.",
    0,
};
char * lbl_1_data_3EA24[] = {
    "コントロールスティック",
    "ボールをつく",
    "ポイントをかえる",
    0,
};
char * lbl_1_data_3EA3C[] = {
    "A Button",
    "Shot",
    0,
};
char * lbl_1_data_3EA50[] = {
    "A-Knopf",
    "Sto\xDF",
    0,
};
char * lbl_1_data_3EA60[] = {
    "Bouton A",
    "Tir",
    0,
};
char * lbl_1_data_3EA74[] = {
    "Bot\xF3n A",
    "Tiro",
    0,
};
char * lbl_1_data_3EA88[] = {
    "Pulsante A",
    "Spara",
    0,
};
char * lbl_1_data_3EAA0[] = {
    "Ａボタン",
    "ショット",
    0,
};
char * lbl_1_data_3EACC[] = {
    "B Button",
    "Return to /bcff0000/View Mode.",
    0,
};
char * lbl_1_data_3EAF4[] = {
    "B-Knopf",
    "Zum /bcff0000/Ansichtsmodus",
    0,
};
char * lbl_1_data_3EB14[] = {
    "Bouton B",
    "Revenir en mode Vue",
    0,
};
char * lbl_1_data_3EB34[] = {
    "Bot\xF3n B",
    "Volver a Md. Vista",
    0,
};
char * lbl_1_data_3EB58[] = {
    "Pulsante B",
    "Torna a mod. visuale",
    0,
};
char * lbl_1_data_3EB90[] = {
    "Ｂボタン",
    "/bcff0000/\x83r\x83\x85\x81[\x83\x82\x81[\x83h/bc000000/\x82\xC9\x82\xE0\x82\xC7\x82\xE9",
    0,
};
char * lbl_1_data_3EBC0[] = {
    "Y Button",
    "Change the strength",
    "of your shot.",
    0,
};
char * lbl_1_data_3EBEC[] = {
    "Y-Knopf",
    "St\xE4rke des",
    "Sto\xDF" "es \xE4ndern",
    0,
};
char * lbl_1_data_3EC18[] = {
    "Bouton Y",
    "Changer la",
    "force du coup",
    0,
};
char * lbl_1_data_3EC48[] = {
    "Bot\xF3n Y",
    "Cambia la fuerza",
    "de tu tiro",
    0,
};
char * lbl_1_data_3EC74[] = {
    "Pulsante Y",
    "Cambia forza",
    "del tiro",
    0,
};
char * lbl_1_data_3ECA0[] = {
    "Ｙボタン",
    "ショットの",
    "つよさをかえる",
    0,
};
char * lbl_1_data_3ECE8[] = {
    "Control Stick",
    "Press left or right to change",
    "your throwing position.",
    0,
};
char * lbl_1_data_3ED30[] = {
    "Control Stick",
    "Dr\xFC" "cke Rechts/Links,",
    "um die Wurfposition zu \xE4ndern.",
    0,
};
char * lbl_1_data_3ED7C[] = {
    "Stick direct.",
    "Appuyer sur Droite ou Gauche",
    "pour changer de position",
    0,
};
char * lbl_1_data_3EDC8[] = {
    "Stick de Control",
    "Pulsa izq o dch para cambiar",
    "posici\xF3n de lanzamiento.",
    0,
};
char * lbl_1_data_3EE0C[] = {
    "Control Stick",
    "Cambia la posizione di tiro",
    "con sinistra e destra",
    0,
};
char * lbl_1_data_3EE3C[] = {
    "コントロールスティック",
    "さゆうでなげる",
    "いちのいどう",
    0,
};
char * lbl_1_data_3EE9C[] = {
    "A Button",
    "Select:",
    "1. Throwing position",
    "2. Throwing direction",
    "3. Throwing strength",
    0,
};
char * lbl_1_data_3EEEC[] = {
    "A-Knopf",
    "W\xE4hle:",
    "1. Wurfposition",
    "2. Wurfrichtung",
    "3. Wurfst\xE4rke",
    0,
};
char * lbl_1_data_3EF50[] = {
    "Bouton A",
    "Choisir",
    "1. Position du lancer",
    "2. Direction du lancer",
    "3. Force du lancer",
    0,
};
char * lbl_1_data_3EFA8[] = {
    "Bot\xF3n A",
    "Elige:",
    "1. Posici\xF3n lanz.",
    "2. Direcci\xF3n lanz.",
    "3. Fuerza lanz.",
    0,
};
char * lbl_1_data_3F00C[] = {
    "Pulsante A",
    "Scegli:",
    "1. Posizione di tiro",
    "2. Direzione del tiro",
    "3. Forza del tiro",
    0,
};
char * lbl_1_data_3F068[] = {
    "Ａボタン",
    "けってい",
    "１．なげるいち",
    "２．なげるほうこう",
    "３．なげるつよさ",
    0,
};
char * lbl_1_data_3F0A0[] = {
    "B Button",
    "Cancel the",
    "throwing position.",
    0,
};
char * lbl_1_data_3F0D8[] = {
    "B-Knopf",
    "Bewegung der",
    "Wurfposition abbrechen.",
    0,
};
char * lbl_1_data_3F108[] = {
    "Bouton B",
    "Annuler la",
    "position du lancer",
    0,
};
char * lbl_1_data_3F140[] = {
    "Bot\xF3n B",
    "Cancelar mov.",
    "de posici\xF3n lanzamiento",
    0,
};
char * lbl_1_data_3F174[] = {
    "Pulsante B",
    "Annulla movim.",
    "posizione di tiro",
    0,
};
char * lbl_1_data_3F1A8[] = {
    "Ｂボタン",
    "なげるいちの",
    "いどうをキャンセル",
    0,
};
char * lbl_1_data_3F1F4[] = {
    "C Button",
    "Change view before",
    "selecting throwing position",
    0,
};
char * lbl_1_data_3F20C[] = {
    "C-Knopf",
    "",
    "",
    0,
};
char * lbl_1_data_3F228[] = {
    "Bouton C",
    "",
    "",
    0,
};
char * lbl_1_data_3F240[] = {
    "Bot\xF3n C",
    "",
    "",
    0,
};
char * lbl_1_data_3F25C[] = {
    "Pulsante C",
    "",
    "",
    0,
};
char * lbl_1_data_3F2AC[] = {
    "Ｃボタン",
    "なげるいちをきめるあいだ",
    "カメラいちのきりかえ",
    0,
};
char * lbl_1_data_3F2D0[] = {
    "X Button",
    "Toggle scorecard.",
    0,
};
char * lbl_1_data_3F2EC[] = {
    "X-Knopf",
    "Punkte wechseln",
    0,
};
char * lbl_1_data_3F304[] = {
    "Bouton X",
    "Basc. score",
    0,
};
char * lbl_1_data_3F320[] = {
    "Bot\xF3n X",
    "Cambia Punt.",
    0,
};
char * lbl_1_data_3F33C[] = {
    "Pulsante X",
    "Cambia punti",
    0,
};
char * lbl_1_data_3F35C[] = {
    "Ｘボタン",
    "スコアのきりかえ",
    0,
};
char * lbl_1_data_3F380[] = {
    "L,R Button",
    "Ball spin",
    0,
};
char * lbl_1_data_3F3AC[] = {
    "L- und R-Taste",
    "Spin der Kugel",
    0,
};
char * lbl_1_data_3F3D4[] = {
    "Boutons L, R",
    "Rotation",
    0,
};
char * lbl_1_data_3F3FC[] = {
    "Bot\xF3n L, R",
    "Rotaci\xF3n bola",
    0,
};
char * lbl_1_data_3F428[] = {
    "Pulsante L, R",
    "Rotaz. palla",
    0,
};
char * lbl_1_data_3F460[] = {
    "Ｌ・Ｒトリガーボタン",
    "ボールのかいてん",
    0,
};
char * lbl_1_data_3F4AC[] = {
    "Control Stick",
    "Press left or right to change",
    "your hitting direction",
    "Press up or down to",
    "select your club",
    0,
};
char * lbl_1_data_3F4F4[] = {
    "Control Stick",
    "Links oder Rechts \xE4ndert",
    "die Schlagrichtung.",
    "",
    "",
    0,
};
char * lbl_1_data_3F524[] = {
    "Stick direct.",
    "Gauche/Droite pour",
    "changer la direction.",
    "",
    "",
    0,
};
char * lbl_1_data_3F574[] = {
    "Stick de Control",
    "Pulsa Izq. o dch. para",
    "cambiar direcci\xF3n del golpe.",
    "",
    "",
    0,
};
char * lbl_1_data_3F5C0[] = {
    "Control Stick",
    "Cambia la direzione del",
    "tiro con destra e sinistra.",
    "",
    "",
    0,
};
char * lbl_1_data_3F60C[] = {
    "コントロールスティック",
    "みぎ・ひだりでうつ",
    "むきをかえる",
    "うえ・したでうつ",
    "クラブをえらぶ",
    0,
};
char * lbl_1_data_3F634[] = {
    "A Button",
    "Hit the ball.",
    0,
};
char * lbl_1_data_3F650[] = {
    "A-Knopf",
    "Ball schlagen",
    0,
};
char * lbl_1_data_3F66C[] = {
    "Bouton A",
    "Taper la balle",
    0,
};
char * lbl_1_data_3F684[] = {
    "Bot\xF3n A",
    "Golpea bola",
    0,
};
char * lbl_1_data_3F69C[] = {
    "Pulsante A",
    "Colpisci",
    0,
};
char * lbl_1_data_3F6B8[] = {
    "Ａボタン",
    "ボールをうつ",
    0,
};
char * lbl_1_data_3F6D8[] = {
    "B Button",
    "Back Camera",
    "Cancel",
    0,
};
char * lbl_1_data_3F6F4[] = {
    "B-Knopf",
    "",
    "Abbrechen",
    0,
};
char * lbl_1_data_3F70C[] = {
    "Bouton B",
    "",
    "Annuler",
    0,
};
char * lbl_1_data_3F724[] = {
    "Bot\xF3n B",
    "",
    "Cancela",
    0,
};
char * lbl_1_data_3F73C[] = {
    "Pulsante B",
    "",
    "Annulla",
    0,
};
char * lbl_1_data_3F774[] = {
    "Ｂボタン",
    "うしろからみたカメラしてん",
    "キャンセル",
    0,
};
char * lbl_1_data_3F784[] = {
    "C Stick",
    "Camera movement",
    0,
};
char * lbl_1_data_3F790[] = {
    "C-Stick",
    "",
    0,
};
char * lbl_1_data_3F79C[] = {
    "Stick C",
    "",
    0,
};
char * lbl_1_data_3F7A8[] = {
    "Stick C",
    "",
    0,
};
char * lbl_1_data_3F7B4[] = {
    "Stick C",
    "",
    0,
};
char * lbl_1_data_3F7D4[] = {
    "Ｃスティック",
    "カメラをうごかす",
    0,
};
char * lbl_1_data_3F7F4[] = {
    "X Button",
    "Landing site Camera",
    0,
};
char * lbl_1_data_3F800[] = {
    "X-Knopf",
    "",
    0,
};
char * lbl_1_data_3F80C[] = {
    "Bouton X",
    "",
    0,
};
char * lbl_1_data_3F818[] = {
    "Bot\xF3n X",
    "",
    0,
};
char * lbl_1_data_3F824[] = {
    "Pulsante X",
    "",
    0,
};
char * lbl_1_data_3F854[] = {
    "Ｘボタン",
    "とんでいくばしょにカメラをうごかす",
    0,
};
char * lbl_1_data_3F878[] = {
    "Z Button",
    "View score",
    0,
};
char * lbl_1_data_3F89C[] = {
    "Z-Knopf",
    "Punkte sehen",
    0,
};
char * lbl_1_data_3F8C0[] = {
    "Bouton Z",
    "Voir score",
    0,
};
char * lbl_1_data_3F8E0[] = {
    "Bot\xF3n Z",
    "Ver punt.",
    0,
};
char * lbl_1_data_3F90C[] = {
    "Pulsante Z",
    "Mostra punteggio",
    0,
};
char * lbl_1_data_3F934[] = {
    "Ｚボタン",
    "スコアをみる",
    0,
};
char * lbl_1_data_3F9EC[] = {
    "Control Stick",
    "Press right or left to turn the",
    "the camera right or left",
    "Press up or left to move",
    "the camera forward or back",
    0,
    "Press up or down to move the camera forward or back.",
};
char * lbl_1_data_3FA6C[] = {
    "Control Stick",
    "Links oder Rechts dreht",
    "die Kamera links/rechts.",
    "Oben oder Unten bewegt",
    "die Kamera vor/zur\xFC" "ck.",
    0,
};
char * lbl_1_data_3FB00[] = {
    "Stick direct.",
    "Droite/Gauche pour tourner",
    "la cam\xE9ra \xE0 droite ou \xE0 gauche.",
    "Haut/Bas pour d\xE9placer la cam\xE9ra",
    "vers l'avant ou l'arri\xE8re.",
    0,
};
char * lbl_1_data_3FB90[] = {
    "Stick de Control",
    "Pulsa dch. o Izq. para girar",
    "la c\xE1mara a derecha o izquierda.",
    "Pulsa arribabajo para mover",
    "c\xE1mara adelante/atr\xE1s.",
    0,
};
char * lbl_1_data_3FC30[] = {
    "Control Stick",
    "Premi destra e sinistra per girare",
    "la telecamera in quella direzione.",
    "Premi su e gi\xF9 per spostare",
    "la telecamera avanti e indietro.",
    0,
};
char * lbl_1_data_3FC90[] = {
    "コントロールスティック",
    "左右でカメラが",
    "左まわり・右まわり",
    "上下でカメラが",
    "ぜんしん・こうたい",
    0,
    "Press up or down to move the camera forward or back.",
};
char * lbl_1_data_3FCD8[] = {
    "A Button",
    "Face the camera point",
    "Return to Shot Mode",
    0,
};
char * lbl_1_data_3FCE8[] = {
    "A-Knopf",
    "",
    "",
    0,
};
char * lbl_1_data_3FCF8[] = {
    "Bouton A",
    "",
    "",
    0,
};
char * lbl_1_data_3FD08[] = {
    "Bot\xF3n A",
    "",
    "",
    0,
};
char * lbl_1_data_3FD18[] = {
    "Pulsante A",
    "",
    "",
    0,
};
char * lbl_1_data_3FD58[] = {
    "Ａボタン",
    "カメラポイントをむき",
    "ショットモードにもどる",
    0,
};
char * lbl_1_data_3FDA4[] = {
    "B Button",
    "Return to Shot Mode without",
    "changing where you are facing.",
    0,
};
char * lbl_1_data_3FDB4[] = {
    "B-Knopf",
    "",
    "",
    0,
};
char * lbl_1_data_3FDC4[] = {
    "Bouton B",
    "",
    "",
    0,
};
char * lbl_1_data_3FDD4[] = {
    "Bot\xF3n B",
    "",
    "",
    0,
};
char * lbl_1_data_3FDE4[] = {
    "Pulsante B",
    "",
    "",
    0,
};
char * lbl_1_data_3FE08[] = {
    "Ｂボタン",
    "むきはかわらずに",
    "ショットモードにもどる",
    0,
};
char * lbl_1_data_3FE34[] = {
    "C Stick",
    "Move the",
    "camera point",
    0,
};
char * lbl_1_data_3FE58[] = {
    "C-Stick",
    "Kamerapunkt",
    "bewegen",
    0,
};
char * lbl_1_data_3FE7C[] = {
    "Stick C",
    "D\xE9placer",
    "la vue",
    0,
};
char * lbl_1_data_3FEA4[] = {
    "Stick C",
    "Mover punto",
    "de c\xE1mara",
    0,
};
char * lbl_1_data_3FED4[] = {
    "Stick C",
    "Sposta direzione",
    "telecamera",
    0,
};
char * lbl_1_data_3FF04[] = {
    "Ｃスティック",
    "カメラポイントを",
    "うごかす",
    0,
};
char * lbl_1_data_3FF24[] = {
    "X Button",
    "Lower camera",
    0,
};
char * lbl_1_data_3FF40[] = {
    "X-Knopf",
    "Kamera runter",
    0,
};
char * lbl_1_data_3FF5C[] = {
    "Bouton X",
    "Lever la vue",
    0,
};
char * lbl_1_data_3FF78[] = {
    "Bot\xF3n X",
    "Bajar c\xE1mara",
    0,
};
char * lbl_1_data_3FF9C[] = {
    "Pulsante X",
    "Telecamera inferiore",
    0,
};
char * lbl_1_data_3FFB8[] = {
    "Ｘボタン",
    "カメラを下げる",
    0,
};
char * lbl_1_data_3FFD4[] = {
    "Y Button",
    "Raise camera",
    0,
};
char * lbl_1_data_3FFEC[] = {
    "Y-Knopf",
    "Kamera rauf",
    0,
};
char * lbl_1_data_40004[] = {
    "Bouton Y",
    "Baisser vue",
    0,
};
char * lbl_1_data_40020[] = {
    "Bot\xF3n Y",
    "Subir c\xE1mara",
    0,
};
char * lbl_1_data_4003C[] = {
    "Pulsante Y",
    "Alza telecamera",
    0,
};
char * lbl_1_data_40058[] = {
    "Ｙボタン",
    "カメラを上げる",
    0,
};
char * lbl_1_data_40088[] = {
    "Y Button",
    "Select the item",
    "you will use next.",
    0,
};
char * lbl_1_data_40098[] = {
    "Y-Knopf",
    "",
    "",
    0,
};
char * lbl_1_data_400A8[] = {
    "Bouton Y",
    "",
    "",
    0,
};
char * lbl_1_data_400B8[] = {
    "Bot\xF3n Y",
    "",
    "",
    0,
};
char * lbl_1_data_400C8[] = {
    "Pulsante Y",
    "",
    "",
    0,
};
char * lbl_1_data_400FC[] = {
    "Ｙボタン",
    "つぎにつかう",
    "アイテムを えらぶ",
    0,
};
char * lbl_1_data_40134[] = {
    "C Stick",
    "Punch in the",
    "direction you press.",
    0,
};
char * lbl_1_data_40144[] = {
    "C-Stick",
    "",
    "",
    0,
};
char * lbl_1_data_40154[] = {
    "Stick C",
    "",
    "",
    0,
};
char * lbl_1_data_40164[] = {
    "Stick C",
    "",
    "",
    0,
};
char * lbl_1_data_40174[] = {
    "Stick C",
    "",
    "",
    0,
};
char * lbl_1_data_40194[] = {
    "Ｃスティック",
    "ハジいたほうに",
    "パンチする",
    0,
};
char * lbl_1_data_40230[] = {
    "R Button or B Button",
    "Press and hold to /bcff0000/generate and",
    "/bcff0000/retain power/bc000000/. Release to /bcff0000/punch/bc000000/.",
    0,
};
char * lbl_1_data_40240[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40250[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40260[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40270[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_402F4[] = {
    "Ｒトリガーボタン、またはＢボタン",
    "\x82\xA8\x82\xB5\x82\xC4/bcff0000/\x83`\x83J\x83\x89\x82\xF0\x82\xBD\x82\xDF/bc000000/",
    "\x82\xCD\x82\xC8\x82\xB5\x82\xC4/bcff0000/\x83p\x83\x93\x83`/bc000000/\x82\xB7\x82\xE9",
    0,
};
char * lbl_1_data_40354[] = {
    "B Button",
    "During /bcff0000/flight with 2 or 5",
    "/bcff0000/balls/bc000000/ Formation Change",
    0,
};
char * lbl_1_data_40364[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40374[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40384[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40394[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_403E0[] = {
    "Ｂボタン",
    "/bcff0000/２コか５コでとぶとき",
    "フォーメーションをかえる",
    0,
};
char * lbl_1_data_40434[] = {
    "Y Button",
    "/bcff0000/Press and hold/bc000000/ to change",
    "to top camera view.",
    0,
};
char * lbl_1_data_40444[] = {
    "Y-Knopf",
    "",
    "",
    0,
};
char * lbl_1_data_40454[] = {
    "Bouton Y",
    "",
    "",
    0,
};
char * lbl_1_data_40464[] = {
    "Bot\xF3n Y",
    "",
    "",
    0,
};
char * lbl_1_data_40474[] = {
    "Pulsante Y",
    "",
    "",
    0,
};
char * lbl_1_data_404CC[] = {
    "Ｙボタン",
    "/bcff0000/おしているあいだ/bc000000/ まうえから",
    "みたカメラしてんになる",
    0,
};
char * lbl_1_data_40534[] = {
    "R Button",
    "/bcff0000/With 'GUIDE ON'/bc000000/ you will",
    "see the path of your ball.",
    0,
};
char * lbl_1_data_40544[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40554[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40564[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40574[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_405EC[] = {
    "Ｒトリガーボタン",
    "/bcff0000/ガイドありのとき/bc000000/ ボールにあたった",
    "あと どうコロがるかをみる",
    0,
};
char * lbl_1_data_4060C[] = {
    "Y Button",
    "Top view Camera",
    0,
};
char * lbl_1_data_40618[] = {
    "Y-Knopf",
    "",
    0,
};
char * lbl_1_data_40624[] = {
    "Bouton Y",
    "",
    0,
};
char * lbl_1_data_40630[] = {
    "Bot\xF3n Y",
    "",
    0,
};
char * lbl_1_data_4063C[] = {
    "Pulsante Y",
    "",
    0,
};
char * lbl_1_data_40664[] = {
    "Ｙボタン",
    "まうえからみたカメラしてん",
    0,
};
char * lbl_1_data_406A4[] = {
    "L,R Button",
    "Press right and left alter-",
    "nately to move forward.",
    0,
};
char * lbl_1_data_406B4[] = {
    "L- und R-Taste",
    "",
    "",
    0,
};
char * lbl_1_data_406C4[] = {
    "Boutons L, R",
    "",
    "",
    0,
};
char * lbl_1_data_406D4[] = {
    "Bot\xF3n L, R",
    "",
    "",
    0,
};
char * lbl_1_data_406E4[] = {
    "Pulsante L, R",
    "",
    "",
    0,
};
char * lbl_1_data_40720[] = {
    "Ｌ・Ｒトリガーボタン",
    "みぎ・ひだりを こうごに",
    "おして まえにすすむ",
    0,
};
char * lbl_1_data_40754[] = {
    "R Button",
    "Press repeatedly",
    "to move left.",
    0,
};
char * lbl_1_data_40764[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40774[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40784[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40794[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_407C4[] = {
    "Ｒトリガーボタン",
    "れんだすると",
    "ひだりにまがる",
    0,
};
char * lbl_1_data_407F0[] = {
    "L Button",
    "Press repeatedly",
    "to move right.",
    0,
};
char * lbl_1_data_40800[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40810[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40820[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40830[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_40864[] = {
    "Ｌトリガーボタン",
    "れんだすると",
    "みぎにまがる",
    0,
};
char * lbl_1_data_40874[] = {
    "Y Button",
    "Select the item",
    "you will use next.",
    0,
};
char * lbl_1_data_40884[] = {
    "Y-Knopf",
    "",
    "",
    0,
};
char * lbl_1_data_40894[] = {
    "Bouton Y",
    "",
    "",
    0,
};
char * lbl_1_data_408A4[] = {
    "Bot\xF3n Y",
    "",
    "",
    0,
};
char * lbl_1_data_408B4[] = {
    "Pulsante Y",
    "",
    "",
    0,
};
char * lbl_1_data_408C4[] = {
    "Ｙボタン",
    "つぎにつかう",
    "アイテムを えらぶ",
    0,
};
char * lbl_1_data_408E4[] = {
    "L,R Button",
    "Strategy change",
    0,
};
char * lbl_1_data_408F0[] = {
    "L- und R-Taste",
    "",
    0,
};
char * lbl_1_data_408FC[] = {
    "Boutons L, R",
    "",
    0,
};
char * lbl_1_data_40908[] = {
    "Bot\xF3n L, R",
    "",
    0,
};
char * lbl_1_data_40914[] = {
    "Pulsante L, R",
    "",
    0,
};
char * lbl_1_data_40934[] = {
    "Ｌ・Ｒトリガーボタン",
    "さくせんをかえる",
    0,
};
char * lbl_1_data_40A54[] = {
    "Control Stick",
    "Arrowed Character",
    "movement",
    "/bcff0000/PK, when you are the kicker",
    "Press Up and Down on the Control",
    "Stick to set kicking power.",
    "Press Left and Right on the Control",
    "Stick to set the direction.",
    "/bcff0000/PK, when you are the goal keeper",
    "Press Left and Right on the Control",
    "Stick to move your character",
    0,
};
char * lbl_1_data_40A90[] = {
    "Control Stick",
    "",
    "",
    "/bcff0000/",
    "",
    "",
    "",
    "",
    "/bcff0000/",
    "",
    "",
    0,
};
char * lbl_1_data_40AC0[] = {
    "Stick direct.",
    "",
    "",
    "/bcff0000/",
    "",
    "",
    "",
    "",
    "/bcff0000/",
    "",
    "",
    0,
};
char * lbl_1_data_40AF0[] = {
    "Stick de Control",
    "",
    "",
    "/bcff0000/",
    "",
    "",
    "",
    "",
    "/bcff0000/",
    "",
    "",
    0,
};
char * lbl_1_data_40B20[] = {
    "Control Stick",
    "",
    "",
    "/bcff0000/",
    "",
    "",
    "",
    "",
    "/bcff0000/",
    "",
    "",
    0,
};
char * lbl_1_data_40BF4[] = {
    "コントロールスティック",
    "やじるしのついた",
    "キャラをうごかす",
    "/bcff0000/\x82o\x82j\x82\xC5\x83L\x83" "b\x83J\x81[\x82\xCC\x82\xC6\x82\xAB",
    "うえ・したで",
    "ける つよさ",
    "みぎ・ひだりで",
    "ける ほうこう",
    "/bcff0000/\x82o\x82j\x82\xC5\x83L\x81[\x83p\x81[\x82\xCC\x82\xC6\x82\xAB",
    "みぎ・ひだりで",
    "キャラをうごかす",
    0,
};
char * lbl_1_data_40C6C[] = {
    "A Button",
    "/bcff0000/-Attack mode-",
    "Shoot",
    "/bcff0000/-Defense mode-",
    "Dash/Tackle",
    0,
};
char * lbl_1_data_40C84[] = {
    "A-Knopf",
    "/bcff0000/",
    "",
    "/bcff0000/",
    "",
    0,
};
char * lbl_1_data_40C9C[] = {
    "Bouton A",
    "/bcff0000/",
    "",
    "/bcff0000/",
    "",
    0,
};
char * lbl_1_data_40CB4[] = {
    "Bot\xF3n A",
    "/bcff0000/",
    "",
    "/bcff0000/",
    "",
    0,
};
char * lbl_1_data_40CCC[] = {
    "Pulsante A",
    "/bcff0000/",
    "",
    "/bcff0000/",
    "",
    0,
};
char * lbl_1_data_40D3C[] = {
    "Ａボタン",
    "/bcff0000/～せめるとき～",
    "シュート",
    "/bcff0000/～まもるとき～",
    "ダッシュ・タックル",
    0,
};
char * lbl_1_data_40D5C[] = {
    "B Button",
    "/bcff0000/-Attack mode-",
    "Pass",
    "/bcff0000/-Defense mode-",
    "Dash/Tackle",
    0,
};
char * lbl_1_data_40D74[] = {
    "B-Knopf",
    "/bcff0000/",
    "",
    "/bcff0000/",
    "",
    0,
};
char * lbl_1_data_40D8C[] = {
    "Bouton B",
    "/bcff0000/",
    "",
    "/bcff0000/",
    "",
    0,
};
char * lbl_1_data_40DA4[] = {
    "Bot\xF3n B",
    "/bcff0000/",
    "",
    "/bcff0000/",
    "",
    0,
};
char * lbl_1_data_40DBC[] = {
    "Pulsante B",
    "/bcff0000/",
    "",
    "/bcff0000/",
    "",
    0,
};
char * lbl_1_data_40DDC[] = {
    "Ｂボタン",
    "/bcff0000/～せめるとき～",
    "パス",
    "/bcff0000/～まもるとき～",
    "ダッシュ・タックル",
    0,
};
char * lbl_1_data_40E38[] = {
    "",
    "/bcff0000/* You don't use the A and",
    "/bcff0000/B Buttons in PK mode.",
    0,
};
char * lbl_1_data_40E48[] = {
    "",
    "/bcff0000/",
    "/bcff0000/",
    0,
};
char * lbl_1_data_40E58[] = {
    "",
    "/bcff0000/",
    "/bcff0000/",
    0,
};
char * lbl_1_data_40E68[] = {
    "",
    "/bcff0000/",
    "/bcff0000/",
    0,
};
char * lbl_1_data_40E78[] = {
    "",
    "/bcff0000/",
    "/bcff0000/",
    0,
};
char * lbl_1_data_40ED0[] = {
    "",
    "/bcff0000/※ＰＫせんでは、Ａ・Ｂボタンを",
    "/bcff0000/つかいません。",
    0,
};
char * lbl_1_data_40FD0[] = {
    "コントロールスティック",
    "/bcff0000/なげるまえ",
    "ポジションをうごかす",
    "/bcff0000/なげたあと",
    "～ピッチャーのボール～",
    "うえでスローボール",
    "したでスピードボール",
    "さゆうでコントロール",
    "/bcff0000/うたれたあと",
    "～ピッチャーがわ～",
    "しゅびをうごかしてボールをとる",
    0,
};
char * lbl_1_data_41138[] = {
    "Control Stick",
    "/bcff0000/Before throwing",
    "/bcff0000/the ball",
    "Position change",
    "/bcff0000/After throwing",
    "/bcff0000/the ball",
    "-Pitch ball-",
    "Push up for Slow ball",
    "Push down for",
    "Speed ball",
    "Push right or left for",
    "Direction controls",
    "/bcff0000/After the ball",
    "/bcff0000/gets hit",
    "-Defense side-",
    "Change Defense",
    "position to",
    "catch the ball",
    0,
};
char * lbl_1_data_411C0[] = {
    "Ａボタン",
    "/bcff0000/\x83s\x83" "b\x83`\x83\x83\x81[",
    "ボールをなげる",
    "/bcff0000/\x83o\x83" "b\x83^\x81[",
    "ボールをうつ",
    0,
};
char * lbl_1_data_41210[] = {
    "A Button",
    "/bcff0000/Pitcher",
    "Throw the ball.",
    "/bcff0000/Batter",
    "Hit the ball.",
    0,
};
char * lbl_1_data_4125C[] = {
    "Ｌ・Ｒトリガーボタン",
    "/bcff0000/\x83s\x83" "b\x83`\x83\x83\x81[\x82\xAA\x82\xED",
    "しゅびいちをうごかす",
    0,
};
char * lbl_1_data_412A4[] = {
    "L,R Button",
    "/bcff0000/Pitcher Side",
    "Change your defense position",
    0,
};
char * lbl_1_data_412C8[] = {
    "コントロールスティック",
    "キャラをうごかす",
    "",
    "いれたほうこうに",
    "ボールをうつ",
    0,
};
char * lbl_1_data_4132C[] = {
    "Control Stick",
    "Character",
    "movement",
    "Hit the ball in",
    "the direction",
    "you press the",
    "Control Stick.",
    0,
};
char * lbl_1_data_41374[] = {
    "Ａボタン",
    "トップスピン",
    "（はやくて、たかい）",
    0,
};
char * lbl_1_data_413A0[] = {
    "A Button",
    "Top Spin",
    "(Fast and High)",
    0,
};
char * lbl_1_data_413D8[] = {
    "Ｂボタン",
    "スライススピン",
    "（おそくて、ひくい）",
    0,
};
char * lbl_1_data_41404[] = {
    "B Button",
    "Slice Spin",
    "(Slow and Low)",
    0,
};
char * lbl_1_data_41434[] = {
    "Ｘボタン",
    "ロブ",
    "（やまなりのボール）",
    0,
};
char * lbl_1_data_41448[] = {
    "X Button",
    "Lob",
    0,
};
char * lbl_1_data_41454[] = {
    "Ｙボタン",
    "スコアをみる",
    0,
};
char * lbl_1_data_41460[] = {
    "Y Button",
    "View score",
    0,
};
char * lbl_1_data_414C8[] = {
    "R Button",
    "/bcff0000/Sight the crosshairs to the",
    "/bcff0000/nearest opponents/bc000000/ and shoot.",
    0,
};
char * lbl_1_data_414D8[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_414E8[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_414F8[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_41508[] = {
    "",
    "",
    "",
    0,
};
char * lbl_1_data_4156C[] = {
    "Ｒトリガーボタン",
    "/bcff0000/しょうじゅんマークをちかくの",
    "/bcff0000/\x82\xC4\x82\xAB\x82\xC9\x82\xA0\x82\xED\x82\xB9/bc000000/\x83^\x83}\x82\xF0\x82\xA4\x82\xC2",
    0,
};
char * lbl_1_data_415F0[] = {
    "A Button",
    "Shoot a bullet",
    "/bcff0000/When you run out of",
    "/bcff0000/bullets, /bc000000/press rapidly",
    "to get 50 more bullets",
    0,
};
char * lbl_1_data_41608[] = {
    "A-Knopf",
    "",
    "",
    "",
    "",
    0,
};
char * lbl_1_data_41620[] = {
    "Bouton A",
    "",
    "",
    "",
    "",
    0,
};
char * lbl_1_data_41638[] = {
    "Bot\xF3n A",
    "",
    "",
    "",
    "",
    0,
};
char * lbl_1_data_41650[] = {
    "Pulsante A",
    "",
    "",
    "",
    "",
    0,
};
char * lbl_1_data_416B8[] = {
    "Ａボタン",
    "タマをうつ",
    "/bcff0000/～タマぎれのとき～",
    "れんだ しつづけると",
    "50ぱつ ふえる",
    0,
};
char * lbl_1_data_416DC[] = {
    "Control Stick",
    "Move the",
    "crosshairs.",
    0,
};
char * lbl_1_data_416EC[] = {
    "Control Stick",
    "",
    "",
    0,
};
char * lbl_1_data_416FC[] = {
    "Stick direct.",
    "",
    "",
    0,
};
char * lbl_1_data_4170C[] = {
    "Stick de Control",
    "",
    "",
    0,
};
char * lbl_1_data_4171C[] = {
    "Control Stick",
    "",
    "",
    0,
};
char * lbl_1_data_41744[] = {
    "コントロールスティック",
    "しょうじゅんマークを",
    "うごかす",
    0,
};
char * lbl_1_data_41774[] = {
    "L Button or B Button",
    "Reload",
    0,
};
char * lbl_1_data_41780[] = {
    "",
    "",
    0,
};
char * lbl_1_data_4178C[] = {
    "",
    "",
    0,
};
char * lbl_1_data_41798[] = {
    "",
    "",
    0,
};
char * lbl_1_data_417A4[] = {
    "",
    "",
    0,
};
char * lbl_1_data_417E0[] = {
    "Ｌトリガーボタン、またはＢボタン",
    "リロード",
    0,
};
char * lbl_1_data_41804[] = {
    "B Button",
    "Fire /bcff0000/missile",
    0,
};
char * lbl_1_data_41810[] = {
    "B-Knopf",
    "",
    0,
};
char * lbl_1_data_4181C[] = {
    "Bouton B",
    "",
    0,
};
char * lbl_1_data_41828[] = {
    "Bot\xF3n B",
    "",
    0,
};
char * lbl_1_data_41834[] = {
    "Pulsante B",
    "",
    0,
};
char * lbl_1_data_41864[] = {
    "Ｂボタン",
    "/bcff0000/\x83~\x83T\x83" "C\x83\x8B/bc000000/\x82\xF0\x82\xA4\x82\xC2",
    0,
};
char * lbl_1_data_41880[] = {
    "L Button",
    "Decrease speed",
    0,
};
char * lbl_1_data_4188C[] = {
    "",
    "",
    0,
};
char * lbl_1_data_41898[] = {
    "",
    "",
    0,
};
char * lbl_1_data_418A4[] = {
    "",
    "",
    0,
};
char * lbl_1_data_418B0[] = {
    "",
    "",
    0,
};
char * lbl_1_data_418D0[] = {
    "Ｌトリガーボタン",
    "スピードをさげる",
    0,
};
char * lbl_1_data_418EC[] = {
    "R Button",
    "Increase speed",
    0,
};
char * lbl_1_data_418F8[] = {
    "",
    "",
    0,
};
char * lbl_1_data_41904[] = {
    "",
    "",
    0,
};
char * lbl_1_data_41910[] = {
    "",
    "",
    0,
};
char * lbl_1_data_4191C[] = {
    "",
    "",
    0,
};
char * lbl_1_data_4193C[] = {
    "Ｒトリガーボタン",
    "スピードをあげる",
    0,
};
char * lbl_1_data_41954[] = {
    "A Button",
    "Fire bullet",
    0,
};
char * lbl_1_data_41960[] = {
    "A-Knopf",
    "",
    0,
};
char * lbl_1_data_4196C[] = {
    "Bouton A",
    "",
    0,
};
char * lbl_1_data_41978[] = {
    "Bot\xF3n A",
    "",
    0,
};
char * lbl_1_data_41984[] = {
    "Pulsante A",
    "",
    0,
};
char * lbl_1_data_41990[] = {
    "Ａボタン",
    "タマをうつ",
    0,
};
char * lbl_1_data_4199C[] = {
    "Control Stick",
    "Push up to dive",
    "Pull down to climb",
    "Left/right to turn",
    0,
};
char * lbl_1_data_419B0[] = {
    "Control Stick",
    "",
    "",
    "",
    0,
};
char * lbl_1_data_419C4[] = {
    "Stick direct.",
    "",
    "",
    "",
    0,
};
char * lbl_1_data_419D8[] = {
    "Stick de Control",
    "",
    "",
    "",
    0,
};
char * lbl_1_data_419EC[] = {
    "Control Stick",
    "",
    "",
    "",
    0,
};
char * lbl_1_data_41A3C[] = {
    "コントロールスティック",
    "うえで さげる",
    "したで あげる",
    "みぎ・ひだりで せんかいする",
    0,
};
void * lbl_1_data_41A50[] = {
    (void *)0x0300000F,
    (void *)0x00870000,
    lbl_1_data_3D808,
    (void *)0x0300000F,
    (void *)0x00870000,
    lbl_1_data_3D824,
    (void *)0x0300000F,
    (void *)0x00870000,
    lbl_1_data_3D854,
    (void *)0x0300000F,
    (void *)0x00870000,
    lbl_1_data_3D884,
    (void *)0x0300000F,
    (void *)0x00870000,
    lbl_1_data_3D89C,
    (void *)0x0300000F,
    (void *)0x00870000,
    lbl_1_data_3D8D4,
    (void *)0x000000F3,
    (void *)0x01570000,
    lbl_1_data_3D8F8,
    (void *)0x000000F3,
    (void *)0x01570000,
    lbl_1_data_3D910,
    (void *)0x000000F3,
    (void *)0x01570000,
    lbl_1_data_3D928,
    (void *)0x000000F3,
    (void *)0x01570000,
    lbl_1_data_3D940,
    (void *)0x000000F3,
    (void *)0x01570000,
    lbl_1_data_3D958,
    (void *)0x000000F3,
    (void *)0x01570000,
    lbl_1_data_3D984,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3D9C4,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3D9FC,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DA3C,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DA74,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DAB8,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DAFC,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_3DB18,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_3DB34,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_3DB48,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_3DB60,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_3DB78,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_3DB98,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DBCC,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DBDC,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DBEC,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DBFC,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DC0C,
    (void *)0x080001D7,
    (void *)0x00F60000,
    lbl_1_data_3DC3C,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_3DCE4,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_3DDA0,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_3DE48,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_3DF00,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_3DFC4,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_3E05C,
    (void *)0x050001D7,
    (void *)0x00F60000,
    lbl_1_data_3E0A8,
    (void *)0x050001D7,
    (void *)0x00F60000,
    lbl_1_data_3E0B8,
    (void *)0x050001D7,
    (void *)0x00F60000,
    lbl_1_data_3E0C8,
    (void *)0x050001D7,
    (void *)0x00F60000,
    lbl_1_data_3E0D8,
    (void *)0x050001D7,
    (void *)0x00F60000,
    lbl_1_data_3E0E8,
    (void *)0x050001D7,
    (void *)0x00F60000,
    lbl_1_data_3E120,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E160,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E19C,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E1D0,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E208,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E244,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E27C,
    (void *)0x0000000F,
    (void *)0x013F0000,
    lbl_1_data_3D8F8,
    (void *)0x0000000F,
    (void *)0x013F0000,
    lbl_1_data_3D910,
    (void *)0x0000000F,
    (void *)0x013F0000,
    lbl_1_data_3D928,
    (void *)0x0000000F,
    (void *)0x013F0000,
    lbl_1_data_3D940,
    (void *)0x0000000F,
    (void *)0x013F0000,
    lbl_1_data_3D958,
    (void *)0x0000000F,
    (void *)0x013F0000,
    lbl_1_data_3D984,
    (void *)0x080001D7,
    (void *)0x00FF0000,
    lbl_1_data_3E2D0,
    (void *)0x080001D7,
    (void *)0x00FF0000,
    lbl_1_data_3E324,
    (void *)0x080001D7,
    (void *)0x00FF0000,
    lbl_1_data_3E374,
    (void *)0x080001D7,
    (void *)0x00FF0000,
    lbl_1_data_3E3BC,
    (void *)0x080001D7,
    (void *)0x00FF0000,
    lbl_1_data_3E404,
    (void *)0x080001D7,
    (void *)0x00FF0000,
    lbl_1_data_3E458,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_3E498,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3E4C8,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3E4F8,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3E524,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3E558,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3E588,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3E5AC,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3E5D0,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3E5F4,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3E60C,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3E62C,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3E658,
    (void *)0x000001D7,
    (void *)0x00BD0000,
    lbl_1_data_3E67C,
    (void *)0x000001D7,
    (void *)0x00BD0000,
    lbl_1_data_3E69C,
    (void *)0x000001D7,
    (void *)0x00BD0000,
    lbl_1_data_3E6C0,
    (void *)0x000001D7,
    (void *)0x00BD0000,
    lbl_1_data_3E6E0,
    (void *)0x000001D7,
    (void *)0x00BD0000,
    lbl_1_data_3E704,
    (void *)0x000001D7,
    (void *)0x00BD0000,
    lbl_1_data_3E730,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3E774,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3E7B8,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3E7F8,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3E83C,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3E87C,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3E8C4,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E8FC,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E93C,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E980,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E9B8,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3E9F0,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3EA24,
    (void *)0x010001D7,
    (void *)0x00FF0000,
    lbl_1_data_3EA3C,
    (void *)0x010001D7,
    (void *)0x00FF0000,
    lbl_1_data_3EA50,
    (void *)0x010001D7,
    (void *)0x00FF0000,
    lbl_1_data_3EA60,
    (void *)0x010001D7,
    (void *)0x00FF0000,
    lbl_1_data_3EA74,
    (void *)0x010001D7,
    (void *)0x00FF0000,
    lbl_1_data_3EA88,
    (void *)0x010001D7,
    (void *)0x00FF0000,
    lbl_1_data_3EAA0,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3EACC,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3EAF4,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3EB14,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3EB34,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3EB58,
    (void *)0x020001D7,
    (void *)0x014F0000,
    lbl_1_data_3EB90,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3EBC0,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3EBEC,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3EC18,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3EC48,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3EC74,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_3ECA0,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3ECE8,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3ED30,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3ED7C,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3EDC8,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3EE0C,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_3EE3C,
    (void *)0x0C0001D7,
    (void *)0x00E30000,
    lbl_1_data_3EE9C,
    (void *)0x0C0001D7,
    (void *)0x00E30000,
    lbl_1_data_3EEEC,
    (void *)0x0C0001D7,
    (void *)0x00E30000,
    lbl_1_data_3EF50,
    (void *)0x0C0001D7,
    (void *)0x00E30000,
    lbl_1_data_3EFA8,
    (void *)0x0C0001D7,
    (void *)0x00E30000,
    lbl_1_data_3F00C,
    (void *)0x0C0001D7,
    (void *)0x00E30000,
    lbl_1_data_3F068,
    (void *)0x090001D7,
    (void *)0x01570000,
    lbl_1_data_3F0A0,
    (void *)0x090001D7,
    (void *)0x01570000,
    lbl_1_data_3F0D8,
    (void *)0x090001D7,
    (void *)0x01570000,
    lbl_1_data_3F108,
    (void *)0x090001D7,
    (void *)0x01570000,
    lbl_1_data_3F140,
    (void *)0x090001D7,
    (void *)0x01570000,
    lbl_1_data_3F174,
    (void *)0x090001D7,
    (void *)0x01570000,
    lbl_1_data_3F1A8,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F2D0,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F2EC,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F304,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F320,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F33C,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F35C,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_3F380,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_3F3AC,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_3F3D4,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_3F3FC,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_3F428,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_3F460,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3F4AC,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3F4F4,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3F524,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3F574,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3F5C0,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3F60C,
    (void *)0x010001D7,
    (void *)0x00F70000,
    lbl_1_data_3F634,
    (void *)0x010001D7,
    (void *)0x00F70000,
    lbl_1_data_3F650,
    (void *)0x010001D7,
    (void *)0x00F70000,
    lbl_1_data_3F66C,
    (void *)0x010001D7,
    (void *)0x00F70000,
    lbl_1_data_3F684,
    (void *)0x010001D7,
    (void *)0x00F70000,
    lbl_1_data_3F69C,
    (void *)0x010001D7,
    (void *)0x00F70000,
    lbl_1_data_3F6B8,
    (void *)0x060001D7,
    (void *)0x014F0000,
    lbl_1_data_3F6D8,
    (void *)0x060001D7,
    (void *)0x014F0000,
    lbl_1_data_3F6F4,
    (void *)0x060001D7,
    (void *)0x014F0000,
    lbl_1_data_3F70C,
    (void *)0x060001D7,
    (void *)0x014F0000,
    lbl_1_data_3F724,
    (void *)0x060001D7,
    (void *)0x014F0000,
    lbl_1_data_3F73C,
    (void *)0x060001D7,
    (void *)0x014F0000,
    lbl_1_data_3F774,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3F784,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3F790,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3F79C,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3F7A8,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3F7B4,
    (void *)0x040000F3,
    (void *)0x01570000,
    lbl_1_data_3F7D4,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F7F4,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F800,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F80C,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F818,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F824,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3F854,
    (void *)0x0F0001D7,
    (void *)0x00630000,
    lbl_1_data_3F878,
    (void *)0x0F0001D7,
    (void *)0x00630000,
    lbl_1_data_3F89C,
    (void *)0x0F0001D7,
    (void *)0x00630000,
    lbl_1_data_3F8C0,
    (void *)0x0F0001D7,
    (void *)0x00630000,
    lbl_1_data_3F8E0,
    (void *)0x0F0001D7,
    (void *)0x00630000,
    lbl_1_data_3F90C,
    (void *)0x0F0001D7,
    (void *)0x00630000,
    lbl_1_data_3F934,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3F9EC,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3FA6C,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3FB00,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3FB90,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3FC30,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_3FC90,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_3FCD8,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_3FCE8,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_3FCF8,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_3FD08,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_3FD18,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_3FD58,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_3FDA4,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_3FDB4,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_3FDC4,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_3FDD4,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_3FDE4,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_3FE08,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3FE34,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3FE58,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3FE7C,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3FEA4,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3FED4,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3FF04,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3FF24,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3FF40,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3FF5C,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3FF78,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3FF9C,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_3FFB8,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_3FFD4,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_3FFEC,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40004,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40020,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_4003C,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40058,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_40088,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_40098,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_400A8,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_400B8,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_400C8,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_400FC,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_40134,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_40144,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_40154,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_40164,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_40174,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_40194,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40230,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40240,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40250,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40260,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40270,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_402F4,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40354,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40364,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40374,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40384,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_40394,
    (void *)0x090000F3,
    (void *)0x004F0000,
    lbl_1_data_403E0,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_40434,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_40444,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_40454,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_40464,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_40474,
    (void *)0x070001D7,
    (void *)0x00770000,
    lbl_1_data_404CC,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40534,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40544,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40554,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40564,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40574,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_405EC,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_4060C,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40618,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40624,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40630,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_4063C,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40664,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_406A4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_406B4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_406C4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_406D4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_406E4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_40720,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40754,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40764,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40774,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40784,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_40794,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_407C4,
    (void *)0x07000011,
    (void *)0x006D0000,
    lbl_1_data_407F0,
    (void *)0x07000011,
    (void *)0x006D0000,
    lbl_1_data_40800,
    (void *)0x07000011,
    (void *)0x006D0000,
    lbl_1_data_40810,
    (void *)0x07000011,
    (void *)0x006D0000,
    lbl_1_data_40820,
    (void *)0x07000011,
    (void *)0x006D0000,
    lbl_1_data_40830,
    (void *)0x07000011,
    (void *)0x006D0000,
    lbl_1_data_40864,
    (void *)0x070001D7,
    (void *)0x00BD0000,
    lbl_1_data_40874,
    (void *)0x070001D7,
    (void *)0x00BD0000,
    lbl_1_data_40884,
    (void *)0x070001D7,
    (void *)0x00BD0000,
    lbl_1_data_40894,
    (void *)0x070001D7,
    (void *)0x00BD0000,
    lbl_1_data_408A4,
    (void *)0x070001D7,
    (void *)0x00BD0000,
    lbl_1_data_408B4,
    (void *)0x070001D7,
    (void *)0x00BD0000,
    lbl_1_data_408C4,
    (void *)0x010001D7,
    (void *)0x010A0000,
    lbl_1_data_3DB18,
    (void *)0x010001D7,
    (void *)0x010A0000,
    lbl_1_data_3DB34,
    (void *)0x010001D7,
    (void *)0x010A0000,
    lbl_1_data_3DB48,
    (void *)0x010001D7,
    (void *)0x010A0000,
    lbl_1_data_3DB60,
    (void *)0x010001D7,
    (void *)0x010A0000,
    lbl_1_data_3DB78,
    (void *)0x010001D7,
    (void *)0x010A0000,
    lbl_1_data_3DB98,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_408E4,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_408F0,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_408FC,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40908,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40914,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_40934,
    (void *)0x1000000F,
    (void *)0x00550000,
    lbl_1_data_40A54,
    (void *)0x1000000F,
    (void *)0x00550000,
    lbl_1_data_40A90,
    (void *)0x1000000F,
    (void *)0x00550000,
    lbl_1_data_40AC0,
    (void *)0x1000000F,
    (void *)0x00550000,
    lbl_1_data_40AF0,
    (void *)0x1000000F,
    (void *)0x00550000,
    lbl_1_data_40B20,
    (void *)0x1000000F,
    (void *)0x00550000,
    lbl_1_data_40BF4,
    (void *)0x110001D7,
    (void *)0x00F70000,
    lbl_1_data_40C6C,
    (void *)0x110001D7,
    (void *)0x00F70000,
    lbl_1_data_40C84,
    (void *)0x110001D7,
    (void *)0x00F70000,
    lbl_1_data_40C9C,
    (void *)0x110001D7,
    (void *)0x00F70000,
    lbl_1_data_40CB4,
    (void *)0x110001D7,
    (void *)0x00F70000,
    lbl_1_data_40CCC,
    (void *)0x110001D7,
    (void *)0x00F70000,
    lbl_1_data_40D3C,
    (void *)0x120000F3,
    (void *)0x01570000,
    lbl_1_data_40D5C,
    (void *)0x120000F3,
    (void *)0x01570000,
    lbl_1_data_40D74,
    (void *)0x120000F3,
    (void *)0x01570000,
    lbl_1_data_40D8C,
    (void *)0x120000F3,
    (void *)0x01570000,
    lbl_1_data_40DA4,
    (void *)0x120000F3,
    (void *)0x01570000,
    lbl_1_data_40DBC,
    (void *)0x120000F3,
    (void *)0x01570000,
    lbl_1_data_40DDC,
    (void *)0x130001C7,
    (void *)0x01720000,
    lbl_1_data_40E38,
    (void *)0x130001C7,
    (void *)0x01720000,
    lbl_1_data_40E48,
    (void *)0x130001C7,
    (void *)0x01720000,
    lbl_1_data_40E58,
    (void *)0x130001C7,
    (void *)0x01720000,
    lbl_1_data_40E68,
    (void *)0x130001C7,
    (void *)0x01720000,
    lbl_1_data_40E78,
    (void *)0x130001C7,
    (void *)0x01720000,
    lbl_1_data_40ED0,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_4132C,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_4132C,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_4132C,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_4132C,
    (void *)0x0E00000F,
    (void *)0x00870000,
    lbl_1_data_4132C,
    (void *)0x0D00000F,
    (void *)0x00870000,
    lbl_1_data_412C8,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_413A0,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_413A0,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_413A0,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_413A0,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_413A0,
    (void *)0x080001D7,
    (void *)0x00F70000,
    lbl_1_data_41374,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_41404,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_41404,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_41404,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_41404,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_41404,
    (void *)0x090001D7,
    (void *)0x014F0000,
    lbl_1_data_413D8,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_41448,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_41448,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_41448,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_41448,
    (void *)0x000001D7,
    (void *)0x009F0000,
    lbl_1_data_41448,
    (void *)0x070001D7,
    (void *)0x009F0000,
    lbl_1_data_41434,
    (void *)0x0B00000F,
    (void *)0x00640000,
    lbl_1_data_41138,
    (void *)0x0B00000F,
    (void *)0x00640000,
    lbl_1_data_41138,
    (void *)0x0B00000F,
    (void *)0x00640000,
    lbl_1_data_41138,
    (void *)0x0B00000F,
    (void *)0x00640000,
    lbl_1_data_41138,
    (void *)0x0B00000F,
    (void *)0x00640000,
    lbl_1_data_41138,
    (void *)0x0B00000F,
    (void *)0x00640000,
    lbl_1_data_40FD0,
    (void *)0x1900000F,
    (void *)0x00520000,
    lbl_1_data_41138,
    (void *)0x1900000F,
    (void *)0x00520000,
    lbl_1_data_41138,
    (void *)0x1900000F,
    (void *)0x00520000,
    lbl_1_data_41138,
    (void *)0x1900000F,
    (void *)0x00520000,
    lbl_1_data_41138,
    (void *)0x1900000F,
    (void *)0x00520000,
    lbl_1_data_41138,
    (void *)0x1000000F,
    (void *)0x00780000,
    lbl_1_data_40FD0,
    (void *)0x0D00000F,
    (void *)0x01480000,
    lbl_1_data_41138,
    (void *)0x0D00000F,
    (void *)0x01480000,
    lbl_1_data_41138,
    (void *)0x0D00000F,
    (void *)0x01480000,
    lbl_1_data_41138,
    (void *)0x0D00000F,
    (void *)0x01480000,
    lbl_1_data_41138,
    (void *)0x0D00000F,
    (void *)0x01480000,
    lbl_1_data_41138,
    (void *)0x0D00000F,
    (void *)0x01480000,
    lbl_1_data_40FD0,
    (void *)0x110001D7,
    (void *)0x00D00000,
    lbl_1_data_41210,
    (void *)0x110001D7,
    (void *)0x00D00000,
    lbl_1_data_41210,
    (void *)0x110001D7,
    (void *)0x00D00000,
    lbl_1_data_41210,
    (void *)0x110001D7,
    (void *)0x00D00000,
    lbl_1_data_41210,
    (void *)0x110001D7,
    (void *)0x00D00000,
    lbl_1_data_41210,
    (void *)0x110001D7,
    (void *)0x00D00000,
    lbl_1_data_411C0,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_412A4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_412A4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_412A4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_412A4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_412A4,
    (void *)0x070000F3,
    (void *)0x004F0000,
    lbl_1_data_4125C,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_414C8,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_414D8,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_414E8,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_414F8,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_41508,
    (void *)0x070001D7,
    (void *)0x006D0000,
    lbl_1_data_4156C,
    (void *)0x0C0001D7,
    (void *)0x00F60000,
    lbl_1_data_415F0,
    (void *)0x0C0001D7,
    (void *)0x00F60000,
    lbl_1_data_41608,
    (void *)0x0C0001D7,
    (void *)0x00F60000,
    lbl_1_data_41620,
    (void *)0x0C0001D7,
    (void *)0x00F60000,
    lbl_1_data_41638,
    (void *)0x0C0001D7,
    (void *)0x00F60000,
    lbl_1_data_41650,
    (void *)0x0C0001D7,
    (void *)0x00F60000,
    lbl_1_data_416B8,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_416DC,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_416EC,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_416FC,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_4170C,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_4171C,
    (void *)0x0B00000F,
    (void *)0x00870000,
    lbl_1_data_41744,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_41774,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_41780,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_4178C,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_41798,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_417A4,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_417E0,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_41804,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_41810,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_4181C,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_41828,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_41834,
    (void *)0x020000F3,
    (void *)0x004F0000,
    lbl_1_data_41864,
    (void *)0x00000011,
    (void *)0x006D0000,
    lbl_1_data_41880,
    (void *)0x00000011,
    (void *)0x006D0000,
    lbl_1_data_4188C,
    (void *)0x00000011,
    (void *)0x006D0000,
    lbl_1_data_41898,
    (void *)0x00000011,
    (void *)0x006D0000,
    lbl_1_data_418A4,
    (void *)0x00000011,
    (void *)0x006D0000,
    lbl_1_data_418B0,
    (void *)0x00000011,
    (void *)0x006D0000,
    lbl_1_data_418D0,
    (void *)0x000001D7,
    (void *)0x006D0000,
    lbl_1_data_418EC,
    (void *)0x000001D7,
    (void *)0x006D0000,
    lbl_1_data_418F8,
    (void *)0x000001D7,
    (void *)0x006D0000,
    lbl_1_data_41904,
    (void *)0x000001D7,
    (void *)0x006D0000,
    lbl_1_data_41910,
    (void *)0x000001D7,
    (void *)0x006D0000,
    lbl_1_data_4191C,
    (void *)0x000001D7,
    (void *)0x006D0000,
    lbl_1_data_4193C,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_41954,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_41960,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_4196C,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_41978,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_41984,
    (void *)0x010001D7,
    (void *)0x00F60000,
    lbl_1_data_41990,
    (void *)0x1800000F,
    (void *)0x00CD0000,
    lbl_1_data_4199C,
    (void *)0x1800000F,
    (void *)0x00CD0000,
    lbl_1_data_419B0,
    (void *)0x1800000F,
    (void *)0x00CD0000,
    lbl_1_data_419C4,
    (void *)0x1800000F,
    (void *)0x00CD0000,
    lbl_1_data_419D8,
    (void *)0x1800000F,
    (void *)0x00CD0000,
    lbl_1_data_419EC,
    (void *)0x1800000F,
    (void *)0x00CD0000,
    lbl_1_data_41A3C,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_41460,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_41460,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_41460,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_41460,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_41460,
    (void *)0x000000F3,
    (void *)0x004F0000,
    lbl_1_data_41454,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3F1F4,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3F20C,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3F228,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3F240,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3F25C,
    (void *)0x0A0000F3,
    (void *)0x01570000,
    lbl_1_data_3F2AC,
};
u8 lbl_1_data_42E90[] = { 0x00, 0x01, 0x02, 0xFF, 0x00, 0x01, 0xFF };
u8 lbl_1_data_42E98[] = { 0x00, 0x01, 0x02, 0xFF };
u8 lbl_1_data_42E9C[] = { 0x00, 0x01, 0x03, 0x23, 0xFF };
u8 lbl_1_data_42EA4[] = { 0x00, 0x08, 0x04, 0x24, 0x25, 0xFF };
u8 lbl_1_data_42EAC[] = { 0x05, 0x01, 0x06, 0x26, 0x27, 0xFF };
u8 lbl_1_data_42EB4[] = { 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x28, 0xFF };
u8 lbl_1_data_42EC0[] = { 0x08, 0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x11, 0x28, 0xFF };
u8 lbl_1_data_42ECC[] = { 0x08, 0x12, 0x13, 0x14, 0x15, 0x16, 0x47, 0xFF };
u8 lbl_1_data_42ED4[] = { 0x08, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x29, 0xFF };
u8 lbl_1_data_42EE0[] = { 0x08, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0xFF };
u8 lbl_1_data_42EE8[] = { 0x01, 0x3D, 0x3E, 0x3F, 0x40, 0xFF };
u8 lbl_1_data_42EF0[] = { 0x01, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0xFF };
u8 lbl_1_data_42EF8[] = { 0x08, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF };
u8 lbl_1_data_42F00[] = { 0x01, 0x41, 0x42, 0x43, 0x44, 0x45, 0xFF };
u8 lbl_1_data_42F08[] = { 0x00, 0x01, 0x02, 0xFF };
void * lbl_1_data_42F1C[] = {
    lbl_1_data_42E90,
    0,
    lbl_1_data_42E98,
    lbl_1_data_42E9C,
    lbl_1_data_42EA4,
    lbl_1_data_42EAC,
    lbl_1_data_42EB4,
    lbl_1_data_42ECC,
    lbl_1_data_42ED4,
    lbl_1_data_42EE8,
    lbl_1_data_42EF0,
    0,
    lbl_1_data_42EF8,
    lbl_1_data_42F00,
    lbl_1_data_42F08,
    "\"(*+\xFF",
    "#\"$%&5\xFF",
    0,
    0,
    0,
    0,
    0,
    0,
    lbl_1_data_42EC0,
    0,
    lbl_1_data_42EE0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
u8 lbl_1_data_42FA4[] = { 0x43, 0x23, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xA0, 0x00, 0x00, 0x43, 0xAD, 0x00, 0x00, 0x43, 0xA0, 0x00, 0x00, 0x43, 0x6E, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x89, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x89, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x89, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0x23, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x89, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0x23, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0x23, 0x00, 0x00, 0x43, 0xAD, 0x00, 0x00, 0x43, 0x9B, 0x00, 0x00, 0x43, 0x6E, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x8C, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xEE, 0x00, 0x00, 0x43, 0xA9, 0x80, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xA0, 0x00, 0x00, 0x43, 0xAD, 0x00, 0x00, 0x43, 0xAF, 0x00, 0x00, 0x43, 0x96, 0x00, 0x00, 0x00, 0xEE, 0x9C, 0x00, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x55, 0x00, 0x00, 0x43, 0xDB, 0x80, 0x00, 0x43, 0x5F, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xA0, 0x00, 0x00, 0x43, 0x0C, 0x00, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x3E, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0x23, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x8C, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xEE, 0x00, 0x00, 0x43, 0xA9, 0x80, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xA0, 0x00, 0x00, 0x43, 0x0C, 0x00, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x3E, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0x23, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x7F, 0x00, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xEE, 0x00, 0x00, 0x43, 0xB9, 0x00, 0x00, 0x43, 0xBD, 0x80, 0x00, 0x43, 0x7F, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x2D, 0x00, 0x00, 0x43, 0xD9, 0x00, 0x00, 0x43, 0x55, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xA0, 0x00, 0x00, 0x42, 0xFA, 0x00, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0x23, 0x00, 0x00, 0x43, 0x1C, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x8A, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xEE, 0x00, 0x00, 0x43, 0xA9, 0x80, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xA0, 0x00, 0x00, 0x43, 0xAD, 0x00, 0x00, 0x43, 0xAF, 0x00, 0x00, 0x43, 0x96, 0x00, 0x00, 0x00, 0xEE, 0x9C, 0x00, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x37, 0x00, 0x00, 0x43, 0xD9, 0x00, 0x00, 0x43, 0x55, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x42, 0xF6, 0x00, 0x00, 0x43, 0xD2, 0x80, 0x00, 0x43, 0x2F, 0x00, 0x00, 0x00, 0x6B, 0x40, 0x8C, 0x43, 0x23, 0x00, 0x00, 0x43, 0x1C, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x8A, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xEE, 0x00, 0x00, 0x43, 0xA9, 0x80, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xA0, 0x00, 0x00, 0x43, 0xAD, 0x00, 0x00, 0x43, 0xAF, 0x00, 0x00, 0x43, 0x96, 0x00, 0x00, 0x00, 0xEE, 0x9C, 0x00, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x37, 0x00, 0x00, 0x43, 0xD9, 0x00, 0x00, 0x43, 0x55, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xA0, 0x00, 0x00, 0x42, 0xF6, 0x00, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x3E, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x0F, 0x00, 0x00, 0x43, 0xCA, 0x80, 0x00, 0x43, 0x43, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xA0, 0x00, 0x00, 0x43, 0xAD, 0x00, 0x00, 0x43, 0xAF, 0x00, 0x00, 0x43, 0x96, 0x00, 0x00, 0x00, 0xEE, 0x9C, 0x00, 0x43, 0xA0, 0x00, 0x00, 0x43, 0x0C, 0x00, 0x00, 0x43, 0xB6, 0x80, 0x00, 0x43, 0x66, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xA0, 0x00, 0x00, 0x43, 0x0C, 0x00, 0x00, 0x43, 0xB6, 0x80, 0x00, 0x43, 0x66, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x0F, 0x00, 0x00, 0x43, 0xCA, 0x80, 0x00, 0x43, 0x43, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x05, 0x00, 0x00, 0x43, 0xD2, 0x80, 0x00, 0x43, 0x2A, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xA0, 0x00, 0x00, 0x42, 0xF6, 0x00, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x3E, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xA0, 0x00, 0x00, 0x43, 0x0C, 0x00, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x05, 0x00, 0x00, 0x43, 0xD2, 0x80, 0x00, 0x43, 0x2A, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0x24, 0x00, 0x00, 0x43, 0x05, 0x00, 0x00, 0x43, 0x5B, 0x00, 0x00, 0x43, 0x2A, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x55, 0x00, 0x00, 0x43, 0xCF, 0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x93, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xA0, 0x00, 0x00, 0x42, 0xFA, 0x00, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0x23, 0x00, 0x00, 0x42, 0xD2, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x89, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xA0, 0x00, 0x00, 0x43, 0xAD, 0x00, 0x00, 0x43, 0xB8, 0x80, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x23, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x8D, 0x00, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xEE, 0x00, 0x00, 0x43, 0xA9, 0x80, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x37, 0x00, 0x00, 0x43, 0xD9, 0x00, 0x00, 0x43, 0x55, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0x23, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0x23, 0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0x43, 0x56, 0x00, 0x00, 0x43, 0x63, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0x23, 0x00, 0x00, 0x43, 0xAE, 0x00, 0x00, 0x43, 0x5A, 0x00, 0x00, 0x43, 0x76, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x70, 0x00, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0xA0, 0x00, 0x00, 0x42, 0xFA, 0x00, 0x00, 0x43, 0x7A, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x05, 0x00, 0x00, 0x43, 0xD2, 0x80, 0x00, 0x43, 0x2A, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x89, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0x23, 0x00, 0x00, 0x43, 0x21, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x53, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xA0, 0x00, 0x00, 0x42, 0xFA, 0x00, 0x00, 0x43, 0xB6, 0x80, 0x00, 0x43, 0x66, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0xA0, 0x00, 0x00, 0x42, 0xFA, 0x00, 0x00, 0x43, 0xB6, 0x80, 0x00, 0x43, 0x66, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x29, 0x43, 0x24, 0x00, 0x00, 0x43, 0x05, 0x00, 0x00, 0x43, 0x5B, 0x00, 0x00, 0x43, 0x2A, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x05, 0x00, 0x00, 0x43, 0xD2, 0x80, 0x00, 0x43, 0x2A, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xEE, 0x00, 0x00, 0x43, 0x89, 0x80, 0x00, 0x43, 0xCD, 0x80, 0x00, 0x43, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x62, 0x43, 0x23, 0x00, 0x00, 0x43, 0x67, 0x00, 0x00, 0x43, 0x5E, 0x00, 0x00, 0x43, 0x67, 0x00, 0x00, 0x00, 0x00, 0xB2, 0xEB, 0x43, 0xA0, 0x00, 0x00, 0x42, 0xFA, 0x00, 0x00, 0x43, 0xBE, 0x80, 0x00, 0x43, 0x3E, 0x00, 0x00, 0x00, 0xA3, 0xAB, 0xBA, 0x43, 0xA0, 0x00, 0x00, 0x43, 0xAD, 0x00, 0x00, 0x43, 0xAF, 0x00, 0x00, 0x43, 0x96, 0x00, 0x00, 0x00, 0xEE, 0x9C };
char * lbl_1_data_43874[] = {
    "If you can guide the ball to the goal without falling off the track,",
    "you'll continue on to the next stage!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ボールが おちないように、コントロールスティックを",
    "うごかして、ゴールできれば、つぎに すすめます！",
    "There are delicious bananas scattered throughout the stages!",
    "If you eat 100, you'll get an extra life!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ステージには、おいしいバナナが あります！",
    "バナナを １００ぽん あつめると、１ＵＰします！",
    "If you fall off the track, you'll lose a life!",
    "If you run out of time, you'll also lose a life!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ステージから おちたら、フォールアウトで１ ミスです！",
    "のこりじかんが ０になると、タイムオーバーで １ミスです！",
    "If you keep falling off or keep running out of time",
    "and you run out of extra monkeys, the game will be over!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ミスをかさねて、のこりの おサルさんが いなくなると",
    "ゲームオーバーです！",
};
char * lbl_1_data_43C2C[] = {
    "You can select any stage that you've played before",
    "and practice doing it over and over!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "プレイしたことのあるステージのなかから",
    "１つ えらんで、くりかえし れんしゅうが できます！",
    "Use the Control Stick to guide the ball to the goal",
    "without falling off the track! Good luck!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ボールがおちないように、コントロールスティックを",
    "うごかして、ゴールできるように がんばりましょう！",
    "When you want to practice a new stage, just press START/PAUSE,",
    "select the \"Stage Select\" option, and change the stage.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "スタート／ポーズボタンで ポーズメニューを ひらき、",
    "「ステージをえらぶ」で、ほかのステージを えらべます。",
    "When you want to quit practicing,",
    "just press START/PAUSE and select the \"Exit Game\" option.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "スタート／ポーズボタンで ポーズメニューを ひらき、",
    "「ゲームをやめる」で、れんしゅうを やめられます。",
};
char * lbl_1_data_45178[] = {
    "You pick one course and race! Roll your ball",
    "as fast as you can and try to come in first!",
    "W\xE4hle einen Kurs f\xFCr ein Rennen! Rolle deine",
    "Kugel so schnell du kannst und werde Erster!",
    "Choisis un circuit et fais la course !",
    "Fonce avec ta boule pour arriver premier !",
    "\xA1" "Elige un circuito y corre! \xA1Rueda la bola",
    "lo m\xE1s r\xE1pido posible y llega el primero!",
    "Scegli un tracciato e corri! Guida la palla",
    "pi\xF9 veloce che puoi, cerca di arrivare primo!",
    "コースを１つだけえらんで、レースをします！",
    "１ばんに なれるように、はやく コロがりましょう！",
    "Once you complete the selected number of laps, you will reach",
    "the goal! When all places have been filled, the race will end.",
    "Nach der gew\xE4hlten Rundenzahl erreichst du das Ziel!",
    "Sobald alle Pl\xE4tze vergeben sind, ist das Rennen beendet.",
    "Une fois que le nombre de tours choisi est termin\xE9, c'est",
    "l'arriv\xE9" "e ! Une fois toutes les places prises, la course se termine.",
    "\xA1" "Cuando completes el n\xFAmero de vueltas elegidas, llegar\xE1s",
    "a la meta! Cuando se ocupen todas las plazas, la carrera terminar\xE1.",
    "Completato il numero di giri stabilito, raggiungerai il traguardo!",
    "Quando tutti i posti sono stati occupati, la corsa finir\xE0.",
    "きめられたラップ数を まわれば、ゴールです！",
    "じゅんいが きまった じてんで、レースは おわりです。",
    "If you hit an Item Box, you will get items. You can grab up to three items!",
    "Choose with the Y Button and use them with the A Button.",
    "",
    "",
    "",
    "",
    "アイテムボックスに あたると、アイテムが とれます！",
    "３コまでとれて、Ｙボタンでえらび、Ａボタンでつかえます！",
    0,
    0,
    0,
    0,
    "If you are playing 1 player, there is a time limit! If time",
    "runs out, the game will end and you will be forced to retire!",
    "Wenn du als Einzelspieler spielst, gibt es ein Zeitlimit!",
    "Wenn es erreicht ist, ist das Spiel zu Ende und du musst aufh\xF6ren!",
    "Si tu es le joueur 1, il y a une limite de temps ! Si le temps",
    "expire, la partie se termine et tu es oblig\xE9 d'abandonner !",
    "\xA1Si juegas el modo 1 jugador, hay l\xEDmite de tiempo! \xA1Si se",
    "agota el tiempo, el juego terminar\xE1 y tendr\xE1s que retirarte!",
    "Se stai giocando in singolo c'\xE8 un limite di tempo! Se il",
    "tempo finisce terminer\xE0 la partita e sarai costretto a ritirarti!",
    "ひとりプレイのときは、せいげんじかんが あります！",
    "０になると タイムオーバーで、リタイアです！",
    "Race on all the courses and get points by winning!",
    "The player with the most points at the end of all the races wins!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "６コースぜんぶで レースして、ポイントを とっていきます！",
    "ごうけいポイントが、１ばん おおいプレイヤーの かちです！",
    "Once you complete the selected number of laps, you will reach",
    "the goal! The better place you come in, the more points you will get!",
    "Nach der gew\xE4hlten Rundenzahl erreichst du das Ziel!",
    "Je weiter vorne du ankommst, desto mehr Punkte gibt es!",
    "Une fois que tu as termin\xE9 le nombre de tours choisi, c'est",
    "l'arriv\xE9" "e ! Plus la place est bonne, plus il y a de points \xE0 gagner !",
    "\xA1" "Cuando completes el n\xFAmero de vueltas elegidas llegar\xE1s",
    "a la meta! \xA1" "Cuanto mejor te clasifiques, m\xE1s puntos ganar\xE1s!",
    "Completato il numero di giri stabilito, raggiungerai il traguardo!",
    "Se arriverai al miglior posto riceverai pi\xF9 punti!",
    "きめられた ラップすうを まわれば、ゴールです！",
    "じゅんいが いいほど、ポイントを たくさん もらえます！",
    "If you hit an item box, you can get items! You can grab up to three items.",
    "Choose with the Y Button and use them with the A Button!",
    "",
    "",
    "",
    "",
    "アイテムボックスに あたると、アイテムが とれます！",
    "３コまでとれて、Ｙボタンでえらび、Ａボタンでつかえます！",
    0,
    0,
    0,
    0,
    "If you are playing 1 player, there is a time limit! If time",
    "runs out, the game will end and you will be forced to retire!",
    "Wenn du als Einzelspieler spielst, gibt es ein Zeitlimit!",
    "Wenn es erreicht ist, ist das Spiel zu Ende und du musst aufh\xF6ren!",
    "Si tu es le joueur 1, il y a une limite de temps ! Si le temps",
    "expire, la partie se termine et tu es oblig\xE9 d'abandonner !",
    "\xA1Si juegas el modo 1 jugador, hay l\xEDmite de tiempo! \xA1Si se",
    "agota el tiempo, el juego terminar\xE1 y tendr\xE1s que retirarte!",
    "Se stai giocando in singolo c'\xE8 un limite di tempo! Se il",
    "tempo finisce terminer\xE0 la partita e sarai costretto a ritirarti!",
    "ひとりプレイのときは、せいげんじかんが あります！",
    "０になると タイムオーバーで、リタイアです！",
    "Pick one course and see how fast",
    "you can complete it by yourself!",
    "W\xE4hle eine Strecke und schau,",
    "wie schnell du sie fahren kannst!",
    "Tout seul, choisis un circuit",
    "pour voir ton meilleur temps !",
    "\xA1" "Elige un circuito y a ver",
    "lo r\xE1pido que puedes completarlo!",
    "Scegli un tracciato e guarda",
    "quanto sei veloce da solo!",
    "コースを１つえらんで、どれだけはやく コロがれるか",
    "ひとりプレイで チャレンジします！",
    "In this mode, you won't find any Item Boxes,",
    "but you will start the race with three Speed Up items!",
    "Hier gibt es keine Objektboxen,",
    "du beginnst das Spiel aber mit drei Tempostars!",
    "Dans ce mode il n'y a pas de bo\xEEtes \xE0 objet,",
    "mais la course commence avec trois objets de vitesse !",
    "\xA1" "En este modo no hay cajas de objetos,",
    "pero iniciar\xE1s la carrera con tres objetos de Aceleraci\xF3n!",
    "In questa modalit\xE0 non troverai scatole oggetto,",
    "ma inizierai la gara con tre oggetti velocit\xE0!",
    "このモードでは、アイテムボックスが でてきませんが",
    "はじめから、スピードアップアイテムを ３つ もっています！",
    "Try and find the best places to use your Speed Up items",
    "and get as fast a time as possible!",
    "Versuche, die Tempostars optimal einzusetzen",
    "und die schnellste Zeit zu fahren!",
    "Il faut trouver le meilleur moment pour utiliser",
    "les objets de vitesse et foncer !",
    "\xA1Usa los objetos de Aceleraci\xF3n en los mejores",
    "lugares y llega lo m\xE1s r\xE1pido posible!",
    "Cerca i posti migliori per usare gli oggetti",
    "velocit\xE0 e ottenere il tempo migliore!",
    "スピードアップアイテムの つかいどころを かんがえて、",
    "ベストタイムを たたきだし、ランクインを ねらいましょう！",
    "There is a time limit in this mode!! If the time",
    "runs out, the game will end and you will be forced to retire!",
    "In diesem Modus gibt es ein Zeitlimit! Wenn es",
    "erreicht ist, ist das Spiel zu Ende und du musst aufh\xF6ren!",
    "Il y a une limite de temps ! Si le temps expire,",
    "la partie se termine et il faut abandonner !",
    "\xA1" "En este modo hay l\xEDmite de tiempo! \xA1Si se agota",
    "el tiempo, el juego termina y tendr\xE1s que retirarte!",
    "C'\xE8 un tempo limite in questa modalit\xE0! Quando finisce",
    "il tempo la partita terminer\xE0 e ti dovrai ritirare!",
    "このモードでは、せいげんじかんが あります！",
    "０になると タイムオーバーで、リタイアです！",
};
char * lbl_1_data_4580C[] = {
    "Punch and hit your opponent!",
    "You will get KO point(s)!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてに パンチを うまく ヒットさせましょう！",
    "ＫＯポイントが もらえます！",
    "When the time limit runs out, the player",
    "with the most points wins the match!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "のこりじかんが ０になったとき、ＫＯポイントが",
    "１ばん おおかった プレイヤーの かちです。",
    "The person with the most wins after",
    "the determined number of matches wins!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "きめられた ラウンドすうを さきにとれば、かちです！",
    "",
    "If there's a tie,",
    "the game will go into sudden death!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "さいしゅうせんで どうてんのばあいは、",
    "サドンデスです！",
    "Punch your opponent to make them fall off!",
    "The player who stays on the stage until the end wins.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてに パンチを うまく ヒットさせて、おとしましょう！",
    "さいごまで いきのこった プレイヤーの かちです。",
    "Stages crumble, piece by piece!",
    "The player who falls off first loses.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ステージは、だんだんと こわれていきます！",
    "さきに おちた プレイヤーの まけです。",
    "The player with the most wins after",
    "the determined number of matches wins!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "きめられた ラウンドすうを さきにとった",
    "プレイヤーのかちと なります！",
    "Punch and hit",
    "your opponent!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてに パンチを うまく",
    "ヒットさせましょう！",
};
char * lbl_1_data_45CDC[] = {
    "After lifting off the ramp, press the A Button in the air,",
    "and wings will open and you can fly in the sky!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ジャンプだいからとびだして、くうちゅうで",
    "Ａボタンをおせば、ボールがハネにかわり、そらをとべます！",
    "When you get close to the target, press the A Button",
    "again to go back into the ball and land it!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ターゲットにちかづいたら、ふたたび Ａボタンをおして",
    "ボールにもどり、ターゲットに うまく とまりましょう！",
    "Land on the target successfully and you will get points!",
    "After flying the set number of rounds, your points are tallied.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "うまくとまることができれば、ポイントをゲットできます！",
    "きめられた かいすうをとび、ごうけいポイントできそいます。",
    "While flying with 2 or 5 Monkey Balls,",
    "you can change the formation with the B Button!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "２コか５コの モンキーボールで とんでいるときは、",
    "Ｂボタンで フォーメーションを かえられます！",
};
char * lbl_1_data_46A28[] = {
    "The player who first pockets",
    "the 9-ball wins!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてよりも さきに ９ばんボールを",
    "ポケットに おとしたほうが、かちです！",
    "Start by pocketing the ball with the lowest number and work your way up!",
    "If you miss, it becomes your opponent's turn.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "もっとも ちいさいナンバーのボールから ねらいましょう！",
    "はずれると ファールとなり、あいてのばんに なります。",
    "After you hit the ball with the lowest number,",
    "and if it goes in a pocket, you can continue playing!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "もっとも ちいさいナンバーのボールに あてたあと、",
    "ナンバーのついたボールをおとせば、つづけて できます！",
    "If your own ball falls into a pocket, it's considered a foul,",
    "and it becomes your opponent's turn.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "じぶんのボールが ポケットに おちてしまったら、",
    "ファールになって、あいてのばんに なります。",
    "Hit the ball with the lowest number. When you pocket",
    "the 3-, 5-, 7-, 9-balls, you get points!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ちいさいナンバーのボールから じゅんばんに あてていき、",
    "３・５・７・９ばんボールを おとせば、てんが もらえます！",
    "The 3-, 5-, 7-balls are worth 1 point, and the 9-ball is worth 2 points",
    "if hit into a corner pocket. Points will be doubled if hit into a side pocket.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "コーナーだと、３・５・７ばんは１てん、９ばんは２てんです。",
    "サイドだと、３・５・７ばんは２てん、９ばんは４てんです。",
    "Whenever the 3-, 5-, 7-, 9-balls fall into a pocket before their turn,",
    "they will be returned to the table.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "３・５・７・９ばんボールを そのじゅんばんよりも はやく",
    "おとしたばあいは、なんどでもフットスポットにもどされます。",
    "After you pocket the 9-ball at the end of the game,",
    "if the cue ball enters the break area, you can continue playing!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "さいごに ９ばんボールをおとしたあと、じぶんのボールを",
    "ブレイクエリアに もどせば、ふたたびブレイクできます！",
    "All 15 balls are used. Start by pocketing the ball",
    "with the lowest number and work your way up!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "１ばんから15ばんまでの ナンバーがついたボールがあり、",
    "ちいさいナンバーから じゅんばんに ねらっていきます！",
    "When you pocket any ball, you receive",
    "the same number of points as the ball #.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ナンバーのついたボールを ポケットにおとすと、",
    "そのナンバーとおなじ てんすうが もらえます。",
    "The total number of points is 120. The player who gets 61 points,",
    "more than half of the total points, wins.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "１ばんから15ばんまでの ごうけいは 120てんです。",
    "そのはんぶんよりおおい 61てんをこえたほうか かちです。",
    "If the game is tied at 60,",
    "the player who pockets the last ball wins.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "おたがいが 60てんで おなじてんすうになる ばあいは、",
    "さいごのボールを おとしたほうが かちとなります。",
    "Balls 1-7 are the low group,",
    "and balls 9-15 are the high group.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "１～７をローボール、９～15をハイボールとグループわけして、",
    "じぶんのグループを きめます。",
    "After breaking the game, the player who first pockets",
    "a ball will take the group of the first pocketed ball.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ブレイクして、さいしょに おとしたボールのグループが、",
    "そのプレイヤーのグループとなります。",
    "If you pocket the 8-ball after pocketing",
    "all of your group balls, you will win.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "じぶんのグループを すべて おとしたあと、",
    "８ばんボールを おとすと かちです。",
    "After break, if you pocket the 8-ball before",
    "pocketing your group balls, you will lose.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "じぶんのグループを すべて おとすまえに、",
    "８ばんボールを おとしてしまうと まけです。",
};
char * lbl_1_data_4853C[] = {
    "At the start, you will use the Control Stick to move left or right",
    "in order to determine your throwing position. Press the A Button to decide.",
    "Zu Beginn legst du deine Wurfposition \xFC" "ber den Control Stick fest.",
    "Dr\xFC" "cke den A-Knopf, um sie festzulegen.",
    "Il faut d'abord utiliser le Stick directionnel pour se d\xE9placer lat\xE9ralement",
    "et choisir la position du lancer. Appuyer sur le Bouton A pour d\xE9" "cider.",
    "Primero usar\xE1s el Stick de Control para mover a Izq. o dch.",
    "y determinar tu posici\xF3n de lanzamiento. Pulsa el Bot\xF3n A para decidir.",
    "All'inizio, usa il Control Stick per muoverti a destra e sinistra",
    "e scegliere la posizione di tiro. Premi il Pulsante A per confermare.",
    "はじめに、コントロールスティックのみぎ・ひだりで、",
    "ボールをなげるいちをうごかして、Ａボタンできめます。",
    "Next, an arrow that shows the direction of the throw will appear.",
    "Press the A Button to stop the arrow from moving back and forth!",
    "Dann erscheint ein Pfeil, der die Richtung des Wurfes angibt.",
    "Dr\xFC" "cke den A-Knopf, damit sich der Pfeil nicht mehr bewegt.",
    "Ensuite, une fl\xE8" "che indique la direction du lancer. Il faut",
    "appuyer sur le Bouton A pour arr\xEAter la fl\xE8" "che de bouger !",
    "Luego aparecer\xE1 una flecha que indica la direcci\xF3n del lanzamiento.",
    "\xA1Pulsa el Bot\xF3n A para que la flecha deje de moverse!",
    "Dopo apparir\xE0 una freccia che mostra la direzione del tiro.",
    "Premi il Pulsante A per fermare il suo movimento avanti e indietro!",
    "つぎに、なげる ほうこうを しめすひょうじが",
    "あらわれるので、タイミングよく Ａボタンをおします！",
    "After that, the throwing strength gauge will appear. Press the A",
    "Button when the meter reaches the strength you want to throw with!",
    "Danach erscheint die Anzeige der Wurfst\xE4rke. Dr\xFC" "cke den",
    "A-Knopf, wenn die Anzeige die gew\xFCnschte Wurfst\xE4rke erreicht hat!",
    "Puis la jauge de force du lancer appara\xEEt. Il faut appuyer",
    "sur le Bouton A quand la jauge atteint la force d\xE9sir\xE9" "e !",
    "Despu\xE9s ver\xE1s el indicador de fuerza de lanzamiento.",
    "\xA1Pulsa el Bot\xF3n A cuando el indicador llegue a la fuerza deseada!",
    "Poi comparir\xE0 l'indicatore della forza. Premi il Pulsante",
    "A quando l'indicatore raggiunge la forza con cui vuoi tirare!",
    "そのあと、なげる つよさを しめすゲージが",
    "あらわれるので、タイミングよく Ａボタンをおします！",
    "Before the ball is thrown, you can press",
    "the L and R Buttons to add spin to your ball!",
    "Bevor die Kugel geworfen wird, kannst du die",
    "L- und R-Taste dr\xFC" "cken, um deinen Ball anzuschneiden!",
    "Avant de lancer la boule, appuyer sur les",
    "Boutons L ou R pour donner un effet \xE0 la boule !",
    "\xA1Por \xFAltimo antes de lanzar la bola, pulsa",
    "el Bot\xF3n L, R si quieres a\xF1" "adir efecto a tu bola!",
    "Infine, prima di lanciare la palla,",
    "puoi premere i Pulsanti L e R per darle un effetto!",
    "さいごに、ボールが なげだされる まえに、",
    "Ｌ・Ｒトリガーボタンで、ボールに かいてんを あたえます！",
    "Throw the ball with the right angle, strength, and spin to knock",
    "over all the pins! Your score will be totaled up from 10 frames!",
    "Wirf die Kugel mit richtigem Winkel, St\xE4rke und Spin, um alle",
    "Pins zu erwischen! Deine Punktzahl wird in 10 Frames addiert!",
    "Lancer la boule avec un angle, la force et l'effet pour",
    "faire tomber les quilles ! Le score s'ajoute sur 10 s\xE9ries !",
    "\xA1Lanza la bola con el \xE1ngulo, fuerza y efecto correcto para",
    "derribar todos los bolos! \xA1Se sumar\xE1 la puntuaci\xF3n de los 10 juegos!",
    "Lancia la palla con angolo, forza ed effetto adatti per abbattere",
    "tutti i birilli! Il tuo punteggio sar\xE0 la somma dei 10 tiri!",
    "ボールをうまくコロがして、たくさんピンをたおしましょう！",
    "１０フレーム なげて、トータルスコアを きそいます！",
    "Try to knock down",
    "all the pins with one throw!",
    "Versuche, die Pins",
    "mit einem Wurf abzur\xE4umen!",
    "Tente de tout faire",
    "tomber en un seul lancer !",
    "\xA1Intenta derribar",
    "todos los bolos de una vez!",
    "Cerca di abbattere",
    "i birilli con un tiro solo!",
    "ボールを １かいだけ コロがして、",
    "レーンの うえにある ピンすべてを たおしましょう！",
    "There are 10 different patterns with which the pins can",
    "be set! You can choose whatever pattern you want!",
    "Es gibt 10 verschiedene Muster, in denen die Pins stehen!",
    "Du kannst ein beliebiges Muster w\xE4hlen. ",
    "Il y a dix fa\xE7ons diff\xE9rentes d'aligner les quilles !",
    "Tu peux choisir l'alignement que tu d\xE9sires !",
    "\xA1Los bolos se pueden colocar de 10 maneras diferentes!",
    "\xA1" "Elige el patr\xF3n que quieras!",
    "Ci sono 10 diverse disposizioni per i birilli,",
    "scegli quella che preferisci!",
    "ピンの おきかたは、ぜんぶで １０パターンです！",
    "チャレンジする じゅんばんは、じゆうに きめられます。",
    "Once you master how to spin the ball, try to",
    "knock down the pins in all the different patterns!",
    "Sobald du wei\xDFt, wie man die Kugel anschneidet,",
    "versuche doch, die Pins aller Muster zu r\xE4umen!",
    "Une fois l'effet ma\xEEtris\xE9, essaye d'abattre",
    "les quilles dans les diff\xE9rents alignements !",
    "\xA1" "Cuando domines c\xF3mo girar la bola, derriba",
    "todos los bolos en todos los diferentes patrones!",
    "Quando hai imparato come dare effetto alla palla,",
    "prova le altre disposizioni!",
    "ボールのかいてん テクニックと、うんを みかたにして、",
    "すべてのパターンを クリアしましょう！",
    "You have 15 tries to knock down the pins!",
    "Once you've thrown the ball 15 times, it's game over!",
    "Du hast 15 Versuche, die Pins zu treffen!",
    "Nach den 15 W\xFCrfen ist das Spiel vorbei!",
    "15 essais pour abattre les quilles !",
    "D\xE8s que la boule est lanc\xE9" "e 15 fois, la partie est finie !",
    "\xA1Tienes 15 intentos para derribar todos los bolos!",
    "\xA1" "Cuando lances 15 veces se termina el juego!",
    "Hai 15 tentativi per abbattere i birilli!",
    "Quando li avrai usati tutti la partita finir\xE0.",
    "ボールをなげられる チャンスは、ぜんぶで１５かいです！",
    "のこりのボールが なくなったら、ゲームオーバーです！",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "At the start, you will use the Control Stick to move left or right",
    "in order to determine your throwing position. Press the A Button to decide.",
    "Zu Beginn legst du deine Wurfposition \xFC" "ber den Control Stick fest.",
    "Dr\xFC" "cke den A-Knopf, um sie festzulegen.",
    "Il faut d'abord utiliser le Stick directionnel pour se d\xE9placer lat\xE9ralement",
    "et choisir la position du lancer. Appuyer sur le Bouton A pour d\xE9" "cider.",
    "Primero usar\xE1s el Stick de Control para mover a Izq. o dch.",
    "y determinar tu posici\xF3n de lanzamiento. Pulsa el Bot\xF3n A para decidir.",
    "All'inizio, usa il Control Stick per muoverti a destra e sinistra",
    "e scegliere la posizione di tiro. Premi il Pulsante A per confermare.",
    "はじめに、コントロールスティックのみぎ・ひだりで、",
    "ボールをなげるいちをうごかして、Ａボタンできめます。",
    "Next, an arrow that shows the direction of the throw will appear.",
    "Press the A Button to stop the arrow from moving back and forth!",
    "Dann erscheint ein Pfeil, der die Richtung des Wurfes angibt.",
    "Dr\xFC" "cke den A-Knopf, damit sich der Pfeil nicht mehr bewegt.",
    "Ensuite, une fl\xE8" "che indique la direction du lancer. Il faut",
    "appuyer sur le Bouton A pour arr\xEAter la fl\xE8" "che de bouger !",
    "Luego aparecer\xE1 una flecha que indica la direcci\xF3n del lanzamiento.",
    "\xA1Pulsa el Bot\xF3n A para que la flecha deje de moverse!",
    "Dopo apparir\xE0 una freccia che mostra la direzione del tiro.",
    "Premi il Pulsante A per fermare il suo movimento avanti e indietro!",
    "つぎに、なげる ほうこうを しめすひょうじが",
    "あらわれるので、タイミングよく Ａボタンをおします！",
    "After that, the throwing strength gauge will appear. Press the A",
    "Button when the meter reaches the strength you want to throw with!",
    "Danach erscheint die Anzeige der Wurfst\xE4rke. Dr\xFC" "cke den",
    "A-Knopf, wenn die Anzeige die gew\xFCnschte Wurfst\xE4rke erreicht hat!",
    "Puis la jauge de force du lancer appara\xEEt. Il faut appuyer",
    "sur le Bouton A quand la jauge atteint la force d\xE9sir\xE9" "e !",
    "Despu\xE9s ver\xE1s el indicador de fuerza de lanzamiento.",
    "\xA1Pulsa el Bot\xF3n A cuando el indicador llegue a la fuerza deseada!",
    "Poi comparir\xE0 l'indicatore della forza. Premi il Pulsante",
    "A quando l'indicatore raggiunge la forza con cui vuoi tirare!",
    "そのあと、なげる つよさを しめすゲージが",
    "あらわれるので、タイミングよく Ａボタンをおします！",
    "Before the ball is thrown, you can press",
    "the L and R Buttons to add spin to your ball!",
    "Bevor die Kugel geworfen wird, kannst du die",
    "L- und R-Taste dr\xFC" "cken, um deinen Ball anzuschneiden!",
    "Avant de lancer la boule, appuyer sur les",
    "Boutons L ou R pour donner un effet \xE0 la boule !",
    "\xA1Por \xFAltimo antes de lanzar la bola, pulsa",
    "el Bot\xF3n L, R si quieres a\xF1" "adir efecto a tu bola!",
    "Infine, prima di lanciare la palla,",
    "puoi premere i Pulsanti L e R per darle un effetto!",
    "さいごに、ボールが なげだされる まえに、",
    "Ｌ・Ｒトリガーボタンで、ボールに かいてんを あたえます！",
    "Play in strange shaped bowling lanes.",
    "Your score will be totaled up from 10 frames!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "かたちの かわった レーンで、ボウリングを します。",
    "10フレームなげて、トータルスコアを きそいます！",
    "At the start, you will use the Control Stick to move left or right",
    "in order to determine your throwing position. Press the A Button to decide.",
    "Zu Beginn legst du deine Wurfposition \xFC" "ber den Control Stick fest.",
    "Dr\xFC" "cke den A-Knopf, um sie festzulegen.",
    "Il faut d'abord utiliser le Stick directionnel pour se d\xE9placer lat\xE9ralement",
    "et choisir la position du lancer. Appuyer sur le Bouton A pour d\xE9" "cider.",
    "Primero usar\xE1s el Stick de Control para mover a Izq. o dch.",
    "y determinar tu posici\xF3n de lanzamiento. Pulsa el Bot\xF3n A para decidir.",
    "All'inizio, usa il Control Stick per muoverti a destra e sinistra",
    "e scegliere la posizione di tiro. Premi il Pulsante A per confermare.",
    "はじめに、コントロールスティックのみぎ・ひだりで、",
    "ボールをなげるいちをうごかして、Ａボタンできめます。",
    "Next, an arrow that shows the direction of the throw will appear.",
    "Press the A Button to stop the arrow from moving back and forth!",
    "Dann erscheint ein Pfeil, der die Richtung des Wurfes angibt.",
    "Dr\xFC" "cke den A-Knopf, damit sich der Pfeil nicht mehr bewegt.",
    "Ensuite, une fl\xE8" "che indique la direction du lancer. Il faut",
    "appuyer sur le Bouton A pour arr\xEAter la fl\xE8" "che de bouger !",
    "Luego aparecer\xE1 una flecha que indica la direcci\xF3n del lanzamiento.",
    "\xA1Pulsa el Bot\xF3n A para que la flecha deje de moverse!",
    "Dopo apparir\xE0 una freccia che mostra la direzione del tiro.",
    "Premi il Pulsante A per fermare il suo movimento avanti e indietro!",
    "つぎに、なげる ほうこうを しめすひょうじが",
    "あらわれるので、タイミングよく Ａボタンをおします！",
    "After that, the throwing strength gauge will appear. Press the A",
    "Button when the meter reaches the strength you want to throw with!",
    "Danach erscheint die Anzeige der Wurfst\xE4rke. Dr\xFC" "cke den",
    "A-Knopf, wenn die Anzeige die gew\xFCnschte Wurfst\xE4rke erreicht hat!",
    "Puis la jauge de force du lancer appara\xEEt. Il faut appuyer",
    "sur le Bouton A quand la jauge atteint la force d\xE9sir\xE9" "e !",
    "Despu\xE9s ver\xE1s el indicador de fuerza de lanzamiento.",
    "\xA1Pulsa el Bot\xF3n A cuando el indicador llegue a la fuerza deseada!",
    "Poi comparir\xE0 l'indicatore della forza. Premi il Pulsante",
    "A quando l'indicatore raggiunge la forza con cui vuoi tirare!",
    "そのあと、なげる つよさを しめすゲージが",
    "あらわれるので、タイミングよく Ａボタンをおします！",
    "Before the ball is thrown, you can press",
    "the L and R Buttons to add spin to your ball!",
    "Bevor die Kugel geworfen wird, kannst du die",
    "L- und R-Taste dr\xFC" "cken, um deinen Ball anzuschneiden!",
    "Avant de lancer la boule, appuyer sur les",
    "Boutons L ou R pour donner un effet \xE0 la boule !",
    "\xA1Por \xFAltimo antes de lanzar la bola, pulsa",
    "el Bot\xF3n L, R si quieres a\xF1" "adir efecto a tu bola!",
    "Infine, prima di lanciare la palla,",
    "puoi premere i Pulsanti L e R per darle un effetto!",
    "さいごに、ボールが なげだされる まえに、",
    "Ｌ・Ｒトリガーボタンで、ボールに かいてんを あたえます！",
    "You will play with 9 pins. The missing pin will be",
    "counted as you knock down the set and score.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "９ほんのピンでプレイしますが、たりない１ぽんぶんは",
    "たおしたものとして、スコアに プラスされます。",
    "At the start, you will use the Control Stick to move left or right",
    "in order to determine your throwing position. Press the A Button to decide.",
    "Zu Beginn legst du deine Wurfposition \xFC" "ber den Control Stick fest.",
    "Dr\xFC" "cke den A-Knopf, um sie festzulegen.",
    "Il faut d'abord utiliser le Stick directionnel pour se d\xE9placer lat\xE9ralement",
    "et choisir la position du lancer. Appuyer sur le Bouton A pour d\xE9" "cider.",
    "Primero usar\xE1s el Stick de Control para mover a Izq. o dch.",
    "y determinar tu posici\xF3n de lanzamiento. Pulsa el Bot\xF3n A para decidir.",
    "All'inizio, usa il Control Stick per muoverti a destra e sinistra",
    "e scegliere la posizione di tiro. Premi il Pulsante A per confermare.",
    "はじめに、コントロールスティックのみぎ・ひだりで、",
    "ボールをなげるいちをうごかして、Ａボタンできめます。",
    "Next, an arrow that shows the direction of the throw will appear.",
    "Press the A Button to stop the arrow from moving back and forth!",
    "Dann erscheint ein Pfeil, der die Richtung des Wurfes angibt.",
    "Dr\xFC" "cke den A-Knopf, damit sich der Pfeil nicht mehr bewegt.",
    "Ensuite, une fl\xE8" "che indique la direction du lancer. Il faut",
    "appuyer sur le Bouton A pour arr\xEAter la fl\xE8" "che de bouger !",
    "Luego aparecer\xE1 una flecha que indica la direcci\xF3n del lanzamiento.",
    "\xA1Pulsa el Bot\xF3n A para que la flecha deje de moverse!",
    "Dopo apparir\xE0 una freccia che mostra la direzione del tiro.",
    "Premi il Pulsante A per fermare il suo movimento avanti e indietro!",
    "つぎに、なげる ほうこうを しめすひょうじが",
    "あらわれるので、タイミングよく Ａボタンをおします！",
    "After that, the throwing strength gauge will appear. Press the A",
    "Button when the meter reaches the strength you want to throw with!",
    "Danach erscheint die Anzeige der Wurfst\xE4rke. Dr\xFC" "cke den",
    "A-Knopf, wenn die Anzeige die gew\xFCnschte Wurfst\xE4rke erreicht hat!",
    "Puis la jauge de force du lancer appara\xEEt. Il faut appuyer",
    "sur le Bouton A quand la jauge atteint la force d\xE9sir\xE9" "e !",
    "Despu\xE9s ver\xE1s el indicador de fuerza de lanzamiento.",
    "\xA1Pulsa el Bot\xF3n A cuando el indicador llegue a la fuerza deseada!",
    "Poi comparir\xE0 l'indicatore della forza. Premi il Pulsante",
    "A quando l'indicatore raggiunge la forza con cui vuoi tirare!",
    "そのあと、なげる つよさを しめすゲージが",
    "あらわれるので、タイミングよく Ａボタンをおします！",
    "Before the ball is thrown, you can press",
    "the L and R Buttons to add spin to your ball!",
    "Bevor die Kugel geworfen wird, kannst du die",
    "L- und R-Taste dr\xFC" "cken, um deinen Ball anzuschneiden!",
    "Avant de lancer la boule, appuyer sur les",
    "Boutons L ou R pour donner un effet \xE0 la boule !",
    "\xA1Por \xFAltimo antes de lanzar la bola, pulsa",
    "el Bot\xF3n L, R si quieres a\xF1" "adir efecto a tu bola!",
    "Infine, prima di lanciare la palla,",
    "puoi premere i Pulsanti L e R per darle un effetto!",
    "さいごに、ボールが なげだされる まえに、",
    "Ｌ・Ｒトリガーボタンで、ボールに かいてんを あたえます！",
    "Throw the ball at the right angle, strength, and spin to knock",
    "over all the pins! Your score will be totaled up after 20 throws!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ボールをうまくコロがして、たくさんピンをたおしましょう！",
    " 20かい なげて、トータルスコアを きそいます！",
};
char * lbl_1_data_499AC[] = {
    "Play the selected number of holes, and total the scores.",
    "The player with the lowest total score will get the better grade.",
    "Spiele die gew\xE4hlte L\xF6" "cherzahl und addiere die Schl\xE4ge.",
    "Der Spieler mit den wenigsten insgesamt erh\xE4lt den besseren Rang.",
    "Joue le nombre de trous choisi et fais attention au score.",
    "Le joueur avec le score total le plus bas l'emporte.",
    "Juega el n\xFAmero de hoyos elegido y suma los puntos.",
    "El jugador con la menor puntuaci\xF3n total gana el mejor grado.",
    "Gioca il numero stabilito di buche e somma i punteggi.",
    "Il giocatore con il minor punteggio finale avr\xE0 un voto migliore.",
    "えらんだホールすうをまわり、トータルスコアをきそいます。",
    "トータルスコアの すくないほうが、いい せいせき です。",
    "Hit the ball and put it in the cup!",
    "The player with the fewest hits will get the better grade.",
    "Schlag den Ball und loche ein! Der Spieler mit den",
    "wenigsten Schl\xE4gen erh\xE4lt den besseren Rang.",
    "Taper la balle pour la mettre dans le trou !",
    "Le joueur avec le moins de coups l'emporte.",
    "\xA1Golpea la bola y m\xE9tela en el hoyo!",
    "El jugador con menos golpes ganar\xE1 el mejor grado.",
    "Colpisci la palla e mandala in buca!",
    "Il giocatore con meno tiri avr\xE0 il voto migliore.",
    "ボールをうってコロがし、カップのなかに いれましょう！",
    "うつ かいすうの すくないほうが、いいせいせきです。",
    "If the ball falls off the track, it counts as an",
    "\"Out of Bounds\"! A penalty of 1 stroke will be incurred!",
    "Wenn der Ball die Bahn verl\xE4sst, z\xE4hlt dies",
    "als 'Ball im Aus'! Dies z\xE4hlt als Strafschlag!",
    "Si la balle sort du circuit, c'est compt\xE9 comme",
    "\"Hors limites\" ! Une p\xE9nalit\xE9 d'un coup est inflig\xE9" "e !",
    "\xA1Si la bola se sale de la calle, cuenta como",
    "\"Fuera de L\xEDmites\" \xA1Se penaliza con 1 golpe!",
    "Se la palla esce dal tracciato, varr\xE0 come un \"fuori",
    "dalla pista\"! Sar\xE0 dato 1 colpo di penalit\xE0.",
    "ホールのそとに ボールが おちてしまうと、ＯＢです！",
    "ペナルティーとして、１かい だすうが プラスされます！",
    "If you can't knock the ball into the hole within 9 hits, you will be",
    "forced to give up! You will be given a score of \"10\" for that hole.",
    "Wenn der Ball nicht mit 9 Schl\xE4gen eingelocht wird, musst",
    "du aufgeben! Du bekommst dann 10 Punkte f\xFCr das Loch notiert.",
    "Si tu ne peux pas faire rentrer la balle en moins de neuf coups,",
    "il faut abandonner ! Un score de \"10\" est attribu\xE9 pour ce trou.",
    "\xA1Si no consigues meter la bola en el hoyo en 9 golpes,",
    "deber\xE1s retirarte! Recibir\xE1s una puntuaci\xF3n de \"10\" para ese hoyo.",
    "Se non riesci a mandare la palla in buca in 9 tiri, dovrai",
    "abbandonare! Ti sar\xE0 assegnato un punteggio di \"10\" per quella buca.",
    "９かいめで カップインしなければ、ギブアップです！",
    "スコアに「10」がつき、そのホールのプレイがおわります。",
    "The player to win each hole will receive points for that hole.",
    "The player with the fewest number of strokes wins.",
    "Der Spieler, der das Loch gewinnt, erh\xE4lt Punkte daf\xFCr.",
    "Der Spieler mit den wenigsten Schl\xE4gen gewinnt.",
    "Le joueur qui gagne un trou obtient des points pour ce trou.",
    "Le joueur avec le moins de coups l'emporte.",
    "El jugador que gane cada hoyo recibir\xE1 puntos por ese hoyo.",
    "El jugador con el menor n\xFAmero de golpes gana.",
    "Il giocatore vincitore di ogni buca ricever\xE0 dei punti.",
    "Vincer\xE0 il giocatore con meno tiri.",
    "かくホールごとに しょうぶして、よりすくない かいすうで",
    "カップインできた プレイヤーの かちになります。",
    "If the difference in the score between the losing player and winning",
    "player becomes too great for the player to catch up, the game will end.",
    "Wenn die Differenz zwischen dem zur\xFC" "ckliegenden und dem",
    "f\xFChrenden Spieler zu gro\xDF wird, um noch aufzuholen, ist das Spiel beendet.",
    "Si la diff\xE9rence de score entre le gagnant et le perdant",
    "est trop grande pour pouvoir revenir au score, la partie se termine.",
    "Si la diferencia de puntuaciones entre el jugador que gana",
    "y el que pierde se ampl\xED" "a demasiado, el juego terminar\xE1.",
    "Se la differenza tra il giocatore in vantaggio e quello in",
    "svantaggio diventa troppo grande da recuperare, la partita finir\xE0.",
    "さがついて、まけプレイヤーが かちプレイヤーの",
    "かちすうを おいぬけなくなったら、プレイは おわりです。",
    "If the ball falls off the track, it counts as an",
    "\"Out of Bounds\"! A penalty of 1 stroke will be incurred!",
    "Wenn der Ball die Bahn verl\xE4sst, z\xE4hlt dies",
    "als 'Ball im Aus'! Dies z\xE4hlt als Strafschlag!",
    "Si la balle sort du circuit, c'est compt\xE9 comme",
    "\"Hors limites\" ! Une p\xE9nalit\xE9 d'un coup est inflig\xE9" "e !",
    "\xA1Si la bola se sale de la calle, cuenta como",
    "\"Fuera de L\xEDmites\" \xA1Se penaliza con 1 golpe!",
    "Se la palla esce dal tracciato, varr\xE0 come un \"fuori",
    "dalla pista\"! Sar\xE0 dato 1 colpo di penalit\xE0.",
    "ホールのそとに ボールが おちてしまうと、ＯＢです！",
    "ペナルティーとして、１かい だすうが プラスされます！",
    "If you can't knock the ball into the hole within 9 hits, you will be",
    "forced to give up! You will be given a score of \"10\" for that hole.",
    "Wenn der Ball nicht mit 9 Schl\xE4gen eingelocht wird, musst",
    "du aufgeben! Du bekommst dann 10 Punkte f\xFCr das Loch notiert.",
    "Si tu ne peux pas faire rentrer la balle en moins de neuf coups,",
    "il faut abandonner ! Un score de \"10\" est attribu\xE9 pour ce trou.",
    "\xA1Si no consigues meter la bola en el hoyo en 9 golpes,",
    "deber\xE1s retirarte! Recibir\xE1s una puntuaci\xF3n de \"10\" para ese hoyo.",
    "Se non riesci a mandare la palla in buca in 9 tiri, dovrai",
    "abbandonare! Ti sar\xE0 assegnato un punteggio di \"10\" per quella buca.",
    "９かいめで カップインしなければ、ギブアップです！",
    "スコアに「10」がつき、そのホールのプレイがおわります。",
};
char * lbl_1_data_49F68[] = {
    "Shoot the targets which appear one",
    "after another and earn points!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "つぎつぎに あらわれる てきを ねらって、",
    "タマを うち、ポイントを たくさん ゲットしましょう！",
    "If you hit the targets continuously without",
    "any mistakes, you will earn more bonus points!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "タマをハズさずに、れんぞくヒットさせていくと、",
    "ボーナスポイントが どんどん プラスされます。",
    "Bullets framed in the same color as",
    "your player color will head toward you.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "プレイヤーカラーと おなじいろのワクで かこまれたタマが",
    "そのプレイヤーめがけて、とんできます。",
    "If you run out of bullets, press the A Button",
    "rapidly! Then you will get 50 more bullets.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "タマぎれに なったら、いそいで Ａボタンを なんかいも",
    "おしましょう！ タマが 50ぱつ プラスされます。",
    "Your life decreases when you get damaged.",
    "If your life runs out, the game is over.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "てきのタマにあたると ダメージをうけて ライフがへり、",
    "ライフが ０になると ゲームオーバーです。",
    "The enemy Boss is waiting for you at the end.",
    "Aim for the weakpoints and shoot!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ステージの さいごには、ボスが あらわれます！",
    "ボスの じゃくてんを ねらって、タマを うちましょう！",
};
char * lbl_1_data_4A52C[] = {
    "You pick one course and race!",
    "Paddle a boat as fast as you can and try to come in first!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "コースを １つだけえらんで、レースをします！",
    "１ばんに なれるように、はやく こぎましょう",
    "Once you complete the selected number of laps, you will reach the goal!",
    "Once all the ranked places have been filled, the race will end.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "きめられた ラップすうを まわれば、ゴールです！",
    "じゅんいが きまった じてんで、レースは おわりです。",
    "If you hit an item box, you can get items! You can grab up to three items,",
    "choose them with the Y Button and use them with the A Button.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "アイテムボックスに あたると、アイテムが とれます！",
    "３コまでとれて、Ｙボタンでえらび、Ａボタンでつかえます！",
    "With good timing, when you press",
    "the L and R Buttons, you can move forward.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ｌ・Ｒトリガーボタンを タイミングよく",
    "かわりばんこに おすと、まえに すすみます。",
    "Press the L Button repeatedly to turn right,",
    "the R Button to turn left.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "みぎに まがるときには Ｌトリガーボタンをれんだして、",
    "ひだりに まがるときは Ｒトリガーボタンをれんだします。",
    "Race on all the courses and get points by winning!",
    "The player with the most points at the end of all the races wins!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "３コースぜんぶで レースして、ポイントを とっていきます！",
    "ごうけいポイントが、１ばん おおいプレイヤーの かちです！",
    "Once you complete the selected number of laps, you will reach the goal!",
    "The better place you come in, the more points you will get!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "きめられた ラップすうを まわれば、ゴールです！じゅんいが いいほど、ポイントを たくさん もらえます！",
    0,
    "If you hit an item box, you can get items! You can grab up to three items,",
    "choose them with the Y Button and use them with the A Button.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "アイテムボックスに あたると、アイテムが とれます！",
    "３コまでとれて、Ｙボタンでえらび、Ａボタンでつかえます！",
    "With good timing, when you press",
    "the L and R Buttons, you can move forward.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ｌ・Ｒトリガーボタンを タイミングよく",
    "かわりばんこに おすと、まえに すすみます。",
    "Press the L Button repeatedly to turn right,",
    "the R Button to turn left.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "みぎに まがるときには Ｌトリガーボタンをれんだして、",
    "ひだりに まがるときは Ｒトリガーボタンをれんだします。",
    "Pick one course and see how fast",
    "you can complete it by yourself!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "コースを１つえらんで、どれだけ はやく こげるのか",
    "ひとりプレイで チャレンジします！",
    "In this mode, you won't find any Item Boxes,",
    "but you will start the race with three Speed Up items!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "このモードでは、アイテムボックスが でてきませんが",
    "はじめから、スピードアップアイテムを ３つ もっています！",
    "Try and find the best places to use your Speed Up items",
    "and get as fast a time as possible!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "スピードアップアイテムの つかいどころを かんがえて、",
    "ベストタイムを たたきだし、ランクインを ねらいましょう！",
    "There is a time limit in this mode!! If time runs out,",
    "the game will end and you will be forced to retire!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "このモードでは、せいげんじかんが あります！",
    "０になると タイムオーバーで、リタイアです！",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};
char * lbl_1_data_4AF10[] = {
    "Up to 2 players can play one game.",
    "Shoot and score into your opponent's goal!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ふたりまでプレイでき、１しあいを たたかいます。",
    "あいてゴールに、シュートを きめましょう！",
    "Guide the soccer ball to the opponent's side,",
    "and if you shoot it into the goal, you get 1 point.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "サッカーボールを あいてサイドまで うまく はこび、",
    "あいてゴールに シュートをきめれば、１てん はいります。",
    "In 'PK ON' gameplay mode, you will have a PK game",
    "in the case of a tie score.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "「ＰＫあり」のゲームプレイせっていで どうてんのばあいは、",
    "ＰＫせんを おこないます。",
    "In 'PK OFF' gameplay mode, you will go into overtime periods.",
    "The first player to get a goal wins.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "「ＰＫなし」のゲームプレイせっていで どうてんのばあいは、",
    "えんちょうせんで さきに てんをいれたほうが かちです。",
    "At halftime,",
    "the teams switch sides.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ハーフタイムになったら、おたがいのゴールが いれかわり、",
    "せめる ほうこうが はんたいに なります。",
    "In 1 player mode, you can play against the COM.",
    " Win against four teams and get a victory!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ひとりプレイせんようで、ＣＯＭと かちぬきせんをします。",
    "４チームを たおして、ゆうしょうを めざしましょう！",
    "Guide the soccer ball to the opponent's side,",
    "and if you shoot it into the goal, you get 1 point.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "サッカーボールを あいてサイドまで うまく はこび、",
    "あいてゴールに シュートをきめれば、１てん はいります。",
    "In 'PK ON' gameplay mode, you will have a PK game",
    "in the case of a tie score.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "「ＰＫあり」のゲームプレイせっていで どうてんのばあいは、",
    "ＰＫせんを おこないます。",
    "In 'PK OFF' gameplay mode, you will go into overtime periods.",
    "The first player to get a goal wins.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "「ＰＫなし」のゲームプレイせっていで どうてんのばあいは、",
    "えんちょうせんで さきに てんをいれたほうが かちです。",
    "At halftime,",
    "the teams switch sides.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ハーフタイムになったら、おたがいのゴールが いれかわり、",
    "せめる ほうこうが はんたいに なります。",
    "Up to 2 players can play a PK game.",
    "Shoot and score into your opponent's goal!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ふたりまでプレイでき、ＰＫせんで たたかいます。",
    "あいてゴールに、シュートを きめましょう！",
    "You use only the Control Stick in PK Mode.",
    "The A and B Buttons are not used.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ＰＫせんで つかうのは、コントロールスティックだけです。",
    "Ａボタンや、Ｂボタンは つかいません。",
    "If you get close to the ball, you shoot automatically.",
    "Set the strength and the direction using the Control Stick.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ボールにちかづくと、じどうてきに シュートします。",
    "コントロールスティックで、つよさと ほうこうを きめます。",
    "If you are the goal keeper, use the Control Stick",
    "to move your character to block the opponent's shot.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "キーパーがわは、コントロールスティックのみぎ・ひだりで",
    "キャラをうごかして、あいてのシュートをブロックします。",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};
char * lbl_1_data_4B5B4[] = {
    "Fly in the air and shoot down the opponents",
    "with your guns and missiles!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "くうちゅうを とびまわり、タマや ミサイルをうって、",
    "たいせんあいてを うちおとしましょう！",
    "If you get an opponent inside your shooting range,",
    "an aim cross will appear.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "タマが とどく きょりまで、あいてに ちかづくと",
    "＋マークの しょうじゅんが でます。",
    "If you get the opponent insde the Lock-on frame,",
    "the color of the square frame changes.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてを ロックオンはんいの わくのなかに いれると、",
    "あいてを かこむ しかくい わくのいろが かわります。",
    "When the square frame becomes red,",
    "it is time to shoot!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてをかこむ しかくい わくが あか に かわったら、",
    "ミサイルを うちましょう！",
    "You earn points",
    "if you hit opponents.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてに こうげきを うまく ヒットさせると、",
    "ポイントが もらえます！",
    "When the time expires, the player with",
    "the most points wins the match!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "のこりじかんが ０になったとき、ポイントが",
    "１ばん おおかった プレイヤーの かちです。",
    "Fly in the air and shoot down the opponents",
    "with your guns and missiles!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "くうちゅうを とびまわり、タマや ミサイルをうって、",
    "たいせんあいてを うちおとしましょう！",
    "If you get an opponent inside your shooting range,",
    "an aim cross will appear.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "タマが とどく きょりまで、あいてに ちかづくと",
    "＋マークの しょうじゅんが でます。",
    "If you get the opponent insde the Lock-on frame,",
    "the color of the square frame changes.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてを ロックオンはんいの わくのなかに いれると、",
    "あいてを かこむ しかくい わくのいろが かわります。",
    "When the square frame becomes red,",
    "it is time to shoot!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "あいてをかこむ しかくい わくが あか に かわったら、",
    "ミサイルを うちましょう！",
    "Survive to win!",
    "Shoot all your opponents down!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ほかの みんなを うちおとしましょう！",
    "さいごまで いきのこった プレイヤーの かちです。",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};
char * lbl_1_data_4B984[] = {
    "If you can guide the ball to the goal without",
    "falling off the track, you'll clear the stage!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ボールが おちないように、コントロールスティックを",
    "うごかして、ゴールできれば、ステージクリアです！",
    "There are delicious bananas scattered throughout the stages!",
    "If you eat them, you'll get bonus points!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ステージには、おいしいバナナが あります！",
    "バナナを とると、ボーナスてんが もらえます！",
    "If you fall off the track,",
    "the game will be over!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ステージから おちたら、フォールアウトで",
    "ゲームオーバーです！",
    "If you run out of time,",
    "the game will be over!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "のこりじかんが ０になると、タイムオーバーで",
    "ゲームオーバーです！",
};
char * lbl_1_data_4BED4[] = {
    "ピッチャーは あいてバッターに ヒットやホームランを",
    "うたれないように、うまく コロがりましょう！",
    "バッターは タイミングよく バットを ふってください！",
    "コロがってくるボールを うちましょう！",
    "ヒット・ツーベース・スリーベースのポケットに",
    "はいれば、 つぎのバッターに だじゅんが まわります！",
    "うったボールがジャンプだいの うえを とおると",
    "ボールは スタンドまで ジャンプして ホームランです！",
    "アウトポケットにおちたり、あいてにキャッチされると",
    "アウトになり、３アウトで チェンジします。",
    "イニングごとに ルーレットをまわし、でためによって",
    "ポケットや ジャンプだいのならびかたが、かわります。",
    "ノーマルシフト：",
    "なにもない めで とまったときの きほんてきなシフトです。",
    "いっぱつシフト：",
    "すべてのジャンプだいがでて、ポケットはぜんぶアウトです。",
    "みぎシフト：",
    "ライトがわに、ヒットのポケットが あつまっています。",
    "ひだりシフト：",
    "レフトがわに、ヒットのポケットが あつまっています。",
    "イニングごとに ルーレットをまわし、でためによって",
    "ポケットや ジャンプだいのならびかたが、かわります。",
    "コツコツシフト：",
    "ポケットが ぜんぶ １ベースヒットになります。",
    "まんなかシフト：",
    "センターに、ヒットのポケットが あつまっています。",
    "はんぶんアウト：",
    "ポケットのはんぶんが、アウトのポケットになります。",
    "ラッキーセブン：",
    "７かいのみ、むじるしのめが ラッキーゾーンになります。",
};
char * lbl_1_data_4C378[] = {
    "The pitcher should perform a nice roll so the",
    "batter doesn't make a hit or a homer!",
    "Batter must swing at the right time to hit the",
    "ball! Hit the rolling ball!",
    "If the ball falls into one base, two base, or three base",
    "pockets, the next batter is up!",
    "If the ball hits the jump board, it will jump to the",
    "stands and become a homerun!",
    "If the ball falls into the Out pocket or is caught by",
    "the other team, it is Out. After 3 Outs your turn is over.",
    "Spin the wheel each inning. Depending on what number you get,",
    "the position of the pockets and the jump boards change.",
    "Normal Shift:",
    "A basic shift occurs when the wheel stops at the No-symbol outcome.",
    "Homerun Shift:",
    "All jump boards appear, and all pockets become Out pockets.",
    "Right Shift:",
    "Hit pockets move to the right side.",
    "Left Shift:",
    "Hit pockets move to the left side.",
    "Spin the wheel each inning. Depending on what number you get,",
    "the position of the pockets and the jump boards change.",
    "Hit Shift:",
    "All pockets become One Base hits.",
    "Center Shift:",
    "Hit pockets move to the center.",
    "Half Out:",
    "Half of the pockets become Out pockets.",
    "Lucky Seven:",
    "On the seventh match, Non-symbol outcomes become lucky zones.",
};
char * lbl_1_data_4C5D8[] = {
    "15、30、40とポイントをとっていき、さきに４ポイントを",
    "とれば、そのプレイヤーのゲームとなります。",
    "おたがいに３ポイントをとると、スコアはデュースとなり、",
    "そのあと ２ポイントの さがでるまで つづけます。",
    "きめられたゲームすうを さきにとったプレイヤーが、",
    "そのセットを とったことになります。",
    "きめられたセットすうを さきにとったプレイヤーが、",
    "そのしあいに かったことになります。",
    0,
    0,
    "じめんに あらわれる スマッシュマークの うえで",
    "タイミングよく ボールをうつと、スマッシュが きまります！",
};
char * lbl_1_data_4C7B0[] = {
    "Points are named 15,30,40. The Player who",
    "gets 4 points first wins the game.",
    "If the score is tied at 3 points, a Deuce occurs.",
    "You continue to play until the difference is 2.",
    "The player who wins the determined number of",
    "games wins the set.",
    "The first player to win the majority of the",
    "determined number of sets wins the match.",
    0,
    0,
    "You can smash if you hit the ball just on the Smash",
    "Mark which appears on the ground!",
};
void * lbl_1_data_4C7E0[] = {
    lbl_1_data_43874,
    0,
    lbl_1_data_43C2C,
    lbl_1_data_45178,
    lbl_1_data_4580C,
    lbl_1_data_45CDC,
    lbl_1_data_46A28,
    lbl_1_data_4853C,
    lbl_1_data_499AC,
    lbl_1_data_49F68,
    lbl_1_data_4A52C,
    0,
    lbl_1_data_4AF10,
    lbl_1_data_4B5B4,
    lbl_1_data_4B984,
    0,
    0,
};
void * lbl_1_data_4C824[] = {
    lbl_1_data_4C378,
    lbl_1_data_4C378,
    lbl_1_data_4C378,
    lbl_1_data_4C378,
    lbl_1_data_4C378,
    lbl_1_data_4BED4,
    lbl_1_data_4C7B0,
    lbl_1_data_4C7B0,
    lbl_1_data_4C7B0,
    lbl_1_data_4C7B0,
    lbl_1_data_4C7B0,
    lbl_1_data_4C5D8,
};
char * lbl_1_data_4D0C0[] = {
    "If you run into an Item Box, you will receive an item...",
    "but only luck will determine what item you receive!",
    "Wenn du \xFC" "ber eine Objektbox l\xE4ufst,",
    "erh\xE4ltst du einen zuf\xE4lligen Gegenstand.",
    "Si tu passe sur une bo\xEEte \xE0 objet,",
    "tu obtiendras un objet qui sera d\xE9termin\xE9 au hasard.",
    "Si caes en una Caja Objeto recibir\xE1s un objeto,",
    "pero la suerte decidir\xE1 el objeto que recibes.",
    "Se passi sopra a una scatola oggetto,",
    "ne riceverai uno: la fortuna determiner\xE0 quale!",
    "アイテムボックスにあたれば、アイテムをとれますが",
    "どのアイテムがとれるかは、プレイヤーのうんです。",
    "Speed Star : For a short time,",
    "your speed will be increased!",
    "Tempostar :",
    "Deine Schnelligkeit wird kurz erh\xF6ht!",
    "Etoile : La vitesse est",
    "augment\xE9" "e pour un moment !",
    "Est. Veloz : \xA1Tu velocidad",
    "aumentar\xE1 por un breve periodo!",
    "Stella della velocit\xE0 :",
    "Aumenta per un po' la tua velocit\xE0!",
    "\x83X\x83s\x81[\x83h\x83X\x83^\x81[ : ",
    "しばらくのあいだ、スピードアップします！",
    "Big Ball : You will become big enough to prevent",
    "almost any attack from other opponents!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ビッグボール：",
    "おおきくなって、ほとんどの こうげきを はねかえします！",
    "Banana Peel : You will drop a banana peel on the ground",
    "and if anyone touches it, they will slip!",
    "Bananenschale : Du l\xE4sst eine Bananenschale fallen,",
    "auf der die anderen Spieler ausrutschen!",
    "Peau de banane : Tu l\xE2" "ches une peau de banane et",
    "si quelqu'un la touche, \xE7" "a glisse dr\xF4lement !",
    "Piel Banana : \xA1" "Arroja una piel de banana",
    "al suelo y si alguien la pisa, se resbalar\xE1!",
    "Buccia di banana : Farai cadere una buccia sul",
    "terreno e se qualcuno la toccher\xE0 scivoler\xE0!",
    "むきむきバナナ :",
    "バナナのかわを コースにおいて、あいてを ころばせます！",
    "Bowling Bomber : You will release a bomb",
    "and if you hit your opponent, it will explode!",
    "Bowling-Bomber : Du wirfst eine Bombe.",
    "Wenn diese deinen Gegner trifft, explodiert sie!",
    "Bombe roulante : Une bombe est l\xE2" "ch\xE9" "e et",
    "quand elle touche l'adversaire, elle explose !",
    "Bomba de Bolos : \xA1Lanzar\xE1s una bomba",
    "y si alcanza a tu adversario, explotar\xE1!",
    "Bomba bowling : Lancerai una bomba che",
    "esploder\xE0 se colpirai l'avversario!",
    "ボーリングボンバー :",
    "ばくだんを あいてに あてれば、バクハツします！",
};
char * lbl_1_data_4D710[] = {
    "If an Item Box appears, punch it to break it open!",
    "If you pick up an item, your punches will become stronger!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "アイテムボックスがおちてきたら、パンチでこわしましょう！",
    "アイテムを とれば、パンチが パワーアップします！",
    "Long Punch : The length of your punches will increase,",
    "and you can even hit far away opponents!",
    "Langer Schlag : Die L\xE4nge der Schl\xE4ge erh\xF6ht sich,",
    "du kannst sogar entfernte Gegner treffen!",
    "Poing long : La port\xE9" "e des coups de poing",
    "augmente pour toucher m\xEAme ceux qui sont loin !",
    "Pu\xF1o Largo : \xA1" "Aumentar\xE1 la longitud de tus",
    "pu\xF1" "etazos y podr\xE1s golpear a lejanos adversarios!",
    "Pugno lungo : Il raggio dei tuoi pugni aumenter\xE0",
    "e potrai colpire anche gli avversari lontani! ",
    "ロングパンチ :",
    "パンチのきょりがのびて、とおくのあいてにも、とどきます！",
    "Iron Punch : Your punches will become as hard as",
    "iron and your striking power will become stronger!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "てっけんパンチ :",
    "てつのようにかたくなり、こうげきりょくがあがります！",
    "Spinning Punch : For a short time,",
    "you will do spinning punches!",
    "Drehschlag : F\xFCr eine kurze",
    "Zeit teilst du Drehschl\xE4ge aus!",
    "Poing tournant : Pendant un moment,",
    "tu fais des coups de poing tournants !",
    "Pu\xF1o Giratorio : \xA1Por unos instantes,",
    "tus pu\xF1" "etazos girar\xE1n!",
    "Pugno rotante : Per breve tempo,",
    "darai dei pugni rotanti!",
    "ぐるぐるパンチ :",
    "しばらくのあいだ、まわりをグルグルまわるパンチがうてます！",
};
char * lbl_1_data_4DB48[] = {
    "You will find items in the air.",
    "Get the items by touching them.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "くうちゅうには アイテムが うかんでいます。",
    "アイテムに ちょくせつ ぶつかると、とれます。",
    "Brake Ball : After you land, this will slow",
    "you down quickly, so you won't roll as much!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ブレーキボール :",
    "ボールの コロがりをおさえ、とまりやすくします！",
    "Magnet Ball : When you land on a target,",
    "you stick to the place that you land on!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "マグネットボール：",
    "ボールが ターゲットに ピッタリ くっつきます！",
    "Lucky Star : Whatever target you land on,",
    "it will double to quintuple your score!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ラッキースター：",
    "ターゲットのてんすうが、２～５ばいになります！",
    "Banana Bunch : If you get a Banana Bunch,",
    "you will get 10 points added to your score!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ふさバナナ：",
    "てんすうが 10てん プラスされます！",
    "Banana : If you get a Banana,",
    "you will get 1 point added to your score.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "バナナ：",
    "てんすうが 1てん プラスされます！",
};
char * lbl_1_data_4DE84[] = {
    "You can get items just",
    "by aiming at them.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ゲームちゅうに でてくる アイテムは",
    "しょうじゅんマークを あわせるだけで とれます。",
    "Big Gun : Your bullets become large",
    "and you can easily hit your opponents.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ビッグガン：",
    "タマが おおきくなり、てきに あたりやすくなります。",
    "X2 Bonus : You earn double points",
    "for a short time.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "×２ボーナス：",
    "しばらくのあいだ、ゲットできるポイントが２ばいになります。",
    "Automatic : You can shoot 18 bullets",
    "without reloading.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "オートマチック：",
    "タマを18ぱつぶん、リロードなしに れんぞくで うてます。",
};
char * lbl_1_data_4E214[] = {
    "If you run into an Item Box, you will receive an item,",
    "but only luck will determine what item you receive.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "アイテムボックスにあたれば、アイテムをとれますが",
    "どのアイテムがとれるかは、プレイヤーのうんです。",
    "Piranha Torpedo : You will release a torpedo and",
    "if you hit your opponent, it will explode and sink your opponent!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ピラニアぎょらい：",
    "あたると ばくはつして、あいてを しずめます！",
    "Whale Bomb : You release an explosion",
    "of water that will blow your opponent up!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "クジラばくだん：",
    "しおを ふいて、あいてを ふきあげます！",
    "Whirling Swirl : There will be a tidal",
    "swell that can suck in your opponent!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "クルリンうずまき：",
    "うずしおが できて、あいてが うずに まきこまれます！",
    "Monkey Submarine : You will dive",
    "and move forward underwater !",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "モンキーサブマリン：",
    "すいちゅうを もぐって、すすみます",
};
char * lbl_1_data_4E45C[] = {
    "You will find items in the air.",
    "Get the items by touching them.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "くうちゅうには アイテムが うかんでいます。",
    "アイテムに ちょくせつ ぶつかると、とれます。",
    "Banana :",
    "This will replenish your life.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "バナナ：",
    "ライフが かいふく します。",
    "Pineapple : ",
    "You get 5 missiles.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "パイナップル：",
    "ミサイルが 5はつ ふえます。",
    "Hurry and get a pineapple. If you run out of",
    "missiles, you'll be in trouble.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ミサイルが ０になると、Ｂボタンを おしても はっしゃ",
    "できなくなるので、いそいで パイナップルを とりましょう。",
};
void * lbl_1_data_4E51C[] = {
    0,
    0,
    0,
    lbl_1_data_4D0C0,
    lbl_1_data_4D710,
    lbl_1_data_4DB48,
    0,
    0,
    0,
    lbl_1_data_4DE84,
    lbl_1_data_4E214,
    0,
    0,
    lbl_1_data_4E45C,
    0,
    0,
    0,
};
char * lbl_1_data_4EAC8[] = {
    "If you run into an Item Box, you will receive an item,",
    "but only luck will determine what item you receive.",
    "Wenn du \xFC" "ber eine Objektbox l\xE4ufst,",
    "erh\xE4ltst du einen zuf\xE4lligen Gegenstand.",
    "Si tu passe sur une bo\xEEte \xE0 objet,",
    "tu obtiendras un objet qui sera d\xE9termin\xE9 au hasard.",
    "Si caes en una Caja Objeto recibir\xE1s un objeto,",
    "pero la suerte decidir\xE1 el objeto que recibes.",
    "Se passi sopra a una scatola oggetto,",
    "ne riceverai uno: la fortuna determiner\xE0 quale!",
    "アイテムボックスにあたれば、アイテムをとれますが",
    "どのアイテムがとれるかは、プレイヤーのうんです。",
    "Polygon Ball : For a short time, your",
    "opponent's ball will become polygonal!",
    "Oberfl\xE4" "che : Die Kugel deines",
    "Gegners wird auf einmal eckig!",
    "Polygone : Pendant un moment, la boule",
    "de l'adversaire devient polygonale !",
    "Bola Poligon : \xA1Por un corto tiempo,",
    "la bola de tu adversario ser\xE1 poligonal! ",
    "Palla poligono : La palla dell'avversario",
    "diventer\xE0 per un po' un poligono!",
    "ポリゴンボール : ",
    "しばらくのあいだ、あいてのボールが カクカクになります！",
    "Ice Cube : For a short time, you will turn your",
    "opponent into ice and he will slide all over!",
    "Eisw\xFCrfel : F\xFCr eine kurze Zeit verwandelt",
    "sich dein Gegner in einen Eisw\xFCrfel und rutscht!",
    "Gla\xE7on : Pendant un moment, l'adversaire est",
    "transform\xE9 en gla\xE7on pour des glissades garanties !",
    "Hielos : \xA1Por unos instantes, convertir\xE1s",
    "en hielo a tus adversario y ellos resbalar\xE1n!",
    "Cubo di ghiaccio : Per un po' l'avversario",
    "diventer\xE0 di ghiaccio scivolando da tutte le parti!",
    "\x83" "A\x83" "C\x83X\x83L\x83\x85\x81[\x83u : ",
    "しばらくのあいだ、あいてを こおらせて、すべらせます！",
    "Gravity Bomber : For a short time, your opponents",
    "will become heavy and their movements will be slowed!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "じゅうりょくだん：",
    "しばらくのあいだ、あいてを おもくして、ノロノロにします！",
    "Hunter Missile : You will let loose missiles",
    "that will hit all opponents who are in front of you.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ハンターミサイル：",
    "まえをコロがっている みんなに ミサイルをうちます！",
};
char * lbl_1_data_4F610[] = {
    "If you turn the \"Wheel of Danger\" On in the Gameplay settings,",
    "then before each flight, the Wheel of Danger will spin!",
    "Wenn du das Risikorad in den Spieleinstellungen",
    "aktivierst, so dreht es sich vor jedem Flug!",
    "La Roue du danger tourne avant chaque envol si elle",
    "est activ\xE9" "e (On) dans les param\xE8tres de jeu !",
    "\xA1Si activas la \"Rueda del Peligro\" en la configuraci\xF3n",
    "del juego, antes de cada vuelo, la Rueda del Peligro girar\xE1!",
    "Se attivi la \"ruota del pericolo\" nelle impostazioni",
    "di gioco, questa girer\xE0 prima di ogni volo!",
    "ゲームプレイせっていで「おじゃまルーレット」をＯＮにするとまいかいフライトまえに、ルーレットをまわすことになります！",
    0,
    "Safe (where there's no symbols) : No traps",
    "will be set. You can fly without a worry.",
    "Sicher (keine Symbole) : Es werden keine",
    "Fallen gestellt. Flieg einfach nur.",
    "Indemne (sans symboles) :",
    "Pas de pi\xE8ge. Vol sans histoires.",
    "Seg. (donde no hay s\xEDmbolos) :",
    "No hay trampas. Puedes volar sin peligro.",
    "Salvezza :",
    "Non ci saranno trappole. Vola tranquillo.",
    "セーフ（マークがないばしょ） :",
    "トラップはでません。あんぜんにフライトできます。",
    "Smoke Screen : A fog-like cloud",
    "makes the targets hard to see!",
    "Nebelsicht : Durch eine Wolke",
    "sind die Ziele kaum zu sehen!",
    "Brume : La cible est difficile",
    "\xE0 voir \xE0 cause de la brume !",
    "Pant. Humo :",
    "\xA1Una especie de neblina oculta el blanco!",
    "Cortina di fumo :",
    "Nebbia che impedisce di vedere i bersagli!",
    "くもがくれのじゅつ :",
    "きりのようなくもがでて、ターゲットがみえにくくなります！",
    "Spiked Ball : There are spiked balls floating in the sky!",
    "If you fly into one, you'll be forced to land!",
    "Stachelkugel : In der Luft fliegen Stachelkugeln!",
    "Triffst du eine, bist du zur Landung gezwungen!",
    "Mines : Des mines flottent dans l'espace !",
    "Si tu en percutes une, tu es oblig\xE9 d'atterrir !",
    "Bola Clavos : \xA1Por el cielo flotan bolas de clavos!",
    "\xA1Si chocas contra ellas tendr\xE1s que aterrizar!",
    "Palla spinosa : Ci sono delle palle spinose nel cielo!",
    "Se ti scontri con una di esse dovrai atterrare!",
    "トゲトゲボール :",
    "トゲのついたボールがそらにうかんで、あたれば落ちます！",
    "Big Bomb : There are bombs set on top of some of",
    "the targets! If you land on one, it will explode!",
    "Bomben : Auf einigen Zielen befinden sich Bomben!",
    "Wenn du darauf landest, explodieren sie!",
    "Bombes : Des bombes se trouvent sur certaines",
    "cibles ! Si tu en touches une, elle explose !",
    "GranBomb : \xA1Hay bombas encima de algunos blancos!",
    "\xA1Si caes sobre una, \xE9sta explotar\xE1!",
    "Grande bomba : Ci sono dei gruppi di bombe",
    "su alcuni bersagli! Se atterri su una esploder\xE0!",
    "ドッカーンボム :",
    "ターゲット上にバクダンがおかれ、ぶつかるとバクハツします！",
};
char * lbl_1_data_4F7BC[] = {
    "You can get items just",
    "by aiming at them.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ゲームちゅうに でてくる アイテムは",
    "しょうじゅんマークを あわせるだけで とれます。",
    "Stopwatch : You can stop time",
    "for 5 seconds.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ストップウォッチ：",
    "５びょうかんだけ、じかんを とめます。",
    "Bullet Plus : ",
    "You get 30 bullets.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "タマプラス：",
    "タマが 30はつ ふえます。",
    "Banana :",
    "This will replenish your life.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "バナナ：",
    "ライフが かいふく します。",
};
void * lbl_1_data_4F87C[] = {
    0,
    0,
    0,
    lbl_1_data_4EAC8,
    0,
    lbl_1_data_4F610,
    0,
    0,
    0,
    lbl_1_data_4F7BC,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
u32 lbl_1_data_4F8C0[] = {
    0x003761E2, 0x0019A871, 0x00FACE19, 0x00E2193E, 0x00962897,
};
u32 lbl_1_data_4F8D4[] = {
    0x007E421F, 0x00E2193E, 0x00FACE19, 0x00962897, 0x0019A871, 0x003761E2,
};
u32 lbl_1_data_4F8EC[] = {
    0x003761E2, 0x0019A871, 0x00FACE19, 0x00E2193E,
};
u32 lbl_1_data_4F8FC[] = {
    0x007E421F, 0x00E2193E, 0x00FACE19, 0x00962897, 0x0019A871, 0x003761E2,
};
u32 lbl_1_data_4F914[] = {
    0x003761E2, 0x0019A871, 0x00FACE19, 0x00E2193E, 0x00962897,
};
void * lbl_1_data_4F928[] = {
    0,
    lbl_1_data_4F8D4,
    lbl_1_data_4F8EC,
    lbl_1_data_4F8FC,
    lbl_1_data_4F8D4,
    lbl_1_data_4F8FC,
    lbl_1_data_4F8FC,
};
void * lbl_1_data_4F944[] = {
    0,
    lbl_1_data_4F8D4,
    0,
    lbl_1_data_4F914,
    lbl_1_data_4F8FC,
    lbl_1_data_4F8FC,
};
u32 lbl_1_data_4F95C[] = {
    0x42E00000, 0x43420000, 0x438A0000, 0x43B30000,
};
u32 lbl_1_data_4F96C[] = {
    0x42E00000, 0x43300000, 0x43700000, 0x43980000, 0x43B80000,
};
u32 lbl_1_data_4F980[] = {
    0x42C40000, 0x431A0000, 0x43520000, 0x43850000, 0x43A10000, 0x43BD0000,
};
void * lbl_1_data_4F998[] = {
    0,
    0,
    0,
    0,
    lbl_1_data_4F95C,
    lbl_1_data_4F96C,
    lbl_1_data_4F980,
};
u32 lbl_1_data_4F9B4[] = {
    0x0000000F, 0x00000020, 0x00000033, 0x00000003, 0x0000000F, 0x00000020,
};
u32 lbl_1_data_4F9CC[] = {
    0x00000045, 0x0000002E, 0x00000040, 0x00000057, 0x00000045, 0x0000002E,
};
u32 lbl_1_data_4F9E4[] = {
    0x00000046, 0x0000002F, 0x00000041, 0x0000005B, 0x00000046, 0x0000002F,
};
u32 lbl_1_data_4F9FC[] = {
    0x00000047, 0x00000030, 0x00000041, 0x0000005C, 0x00000047, 0x00000030,
    0x0000000F, 0x00000020, 0x00000033, 0x00000003, 0x00000045, 0x0000002E,
    0x00000034, 0x00000004, 0x00000046, 0x0000002F, 0x00000035, 0x00000007,
    0x00000047, 0x00000030, 0x00000036, 0x00000008,
};
u32 lbl_1_data_4FA54[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000F,
};
u32 lbl_1_data_4FA68[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000045,
};
u32 lbl_1_data_4FA7C[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000046,
};
u32 lbl_1_data_4FA90[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000047,
};
void * lbl_1_data_4FAA4[] = {
    lbl_1_data_4F9B4,
    lbl_1_data_4F9B4,
    lbl_1_data_4FA54,
    lbl_1_data_4F9B4,
};
void * lbl_1_data_4FAB4[] = {
    lbl_1_data_4F9CC,
    lbl_1_data_4F9CC,
    lbl_1_data_4FA68,
    lbl_1_data_4F9CC,
};
void * lbl_1_data_4FAC4[] = {
    lbl_1_data_4F9E4,
    lbl_1_data_4F9E4,
    lbl_1_data_4FA7C,
    lbl_1_data_4F9E4,
};
void * lbl_1_data_4FAD4[] = {
    lbl_1_data_4F9FC,
    lbl_1_data_4F9FC,
    lbl_1_data_4FA90,
    lbl_1_data_4F9FC,
};
u32 lbl_1_data_4FAE4[] = {
    0x00000C09, 0x00000C0C, 0x00000C17, 0x00000C07, 0x00000C08,
};
u32 lbl_1_data_4FAF8[] = {
    0x00000C09, 0x00000C0B, 0x00000C0A, 0x00000C18, 0x00000C2C,
};
u32 lbl_1_data_4FB0C[] = {
    0x00000C04, 0x00000C05, 0x00000C03, 0x00000C06,
};
u32 lbl_1_data_4FB1C[] = {
    0x0000000F, 0x00000C2F, 0x00000C33, 0x00000C35, 0x00000C2D, 0x00000C2E,
};
u32 lbl_1_data_4FB34[] = {
    0x00000000,
};
u32 lbl_1_data_4FB38[] = {
    0x0000000F, 0x00000C12, 0x00000C14, 0x00000C11,
};
u32 lbl_1_data_4FB48[] = {
    0x0000000F, 0x00000C15, 0x00000C16, 0x00000C13,
};
u32 lbl_1_data_4FB58[] = {
    0x00000C31, 0x00000C0D, 0x00000C0E, 0x00000C10, 0x00000C0F,
};
u32 lbl_1_data_4FB6C[] = {
    0x0000000F, 0x00000C41, 0x00000C42, 0x0000000F,
};
void * lbl_1_data_4FB7C[] = {
    0,
    lbl_1_data_4FAE4,
    lbl_1_data_4FB0C,
    lbl_1_data_4FB1C,
    lbl_1_data_4FB58,
    lbl_1_data_4FB38,
    lbl_1_data_4FB6C,
};
void * lbl_1_data_4FB98[] = {
    0,
    lbl_1_data_4FAF8,
    0,
    lbl_1_data_4FB34,
    0,
    lbl_1_data_4FB48,
};
u32 lbl_1_data_4FBB0[] = {
    0xFFFFFFFF, 0x00000C3C, 0x00000C3F, 0x00000C3D, 0x00000C39, 0x00000C3B,
    0x00000C3E, 0x00000C3A,
};
struct HowToBmp
{
    /*0x00*/ s32 bmpId;
    /*0x04*/ s16 lineY[18];
}; /* size = 0x28 */

struct HowToTooltip
{
    /*0x00*/ s8 bmpIdx;
    /*0x02*/ s16 x;
    /*0x04*/ s16 y;
    /*0x08*/ char **lines;
}; /* size = 0xC */

struct HowToArrow
{
    /*0x00*/ float x1;
    /*0x04*/ float y1;
    /*0x08*/ float x2;
    /*0x0C*/ float y2;
    /*0x10*/ u32 color;
}; /* size = 0x14 */

#ifndef __MWERKS__
/* PC port: lbl_1_data_41A50 (HowToTooltip), lbl_1_data_3D3D8 (HowToBmp) and
 * lbl_1_data_42FA4 (HowToArrow) are images of the big-endian GameCube data,
 * written as u32 words / bytes, so they can't be read through the structs on
 * a little-endian host. Convert the HowToBmp words in place and decode the
 * tooltips and arrows into native tables (the arrow byte array is one byte
 * short of 72 entries: its trailing zero byte is missing). The tooltip table
 * is a void *[] of 3 words per entry
 * (two packed words + the lines pointer); its elements are pointer-sized, so
 * on x64 neither its stride nor HowToTooltip's size is 0xC and it can't be
 * converted in place. */
#define HOWTO_TOOLTIP_COUNT (sizeof(lbl_1_data_41A50) / (3 * sizeof(lbl_1_data_41A50[0])))
static struct HowToTooltip s_pcTooltips[HOWTO_TOOLTIP_COUNT];
#define HOWTO_BMP_COUNT (sizeof(lbl_1_data_3D3D8) / sizeof(struct HowToBmp))
#define HOWTO_ARROW_COUNT ((sizeof(lbl_1_data_42FA4) + sizeof(struct HowToArrow) - 1) / sizeof(struct HowToArrow))
static struct HowToArrow s_pcArrows[HOWTO_ARROW_COUNT];

static u32 pc_how_to_be32(const u8 *data, u32 size, u32 offset)
{
    u32 value = 0;
    u32 i;

    for (i = 0; i < 4; i++)
        value = (value << 8) | (offset + i < size ? data[offset + i] : 0);
    return value;
}

static void pc_fix_how_to_tables(void)
{
    static int done = 0;
    u32 i;
    u32 j;

    if (done)
        return;
    done = 1;

    for (i = 0; i < HOWTO_TOOLTIP_COUNT; i++)
    {
        void **raw = &lbl_1_data_41A50[i * 3];
        u32 w0 = (u32)(uintptr_t)raw[0];
        u32 w1 = (u32)(uintptr_t)raw[1];
        struct HowToTooltip *tip = &s_pcTooltips[i];

        tip->bmpIdx = (s8)(w0 >> 24);
        tip->x = (s16)(w0 & 0xFFFF);
        tip->y = (s16)(w1 >> 16);
        tip->lines = (char **)raw[2];
    }

    for (i = 0; i < HOWTO_BMP_COUNT; i++)
    {
        u32 *raw = &lbl_1_data_3D3D8[i * (sizeof(struct HowToBmp) / 4)];
        struct HowToBmp bmp;

        bmp.bmpId = (s32)raw[0];
        for (j = 0; j < 18; j += 2)
        {
            u32 w = raw[1 + j / 2];
            bmp.lineY[j] = (s16)(w >> 16);
            bmp.lineY[j + 1] = (s16)(w & 0xFFFF);
        }
        memcpy(raw, &bmp, sizeof(bmp));
    }

    for (i = 0; i < HOWTO_ARROW_COUNT; i++)
    {
        u32 base = i * 0x14;
        u32 w;

        w = pc_how_to_be32(lbl_1_data_42FA4, sizeof(lbl_1_data_42FA4), base + 0x00);
        memcpy(&s_pcArrows[i].x1, &w, 4);
        w = pc_how_to_be32(lbl_1_data_42FA4, sizeof(lbl_1_data_42FA4), base + 0x04);
        memcpy(&s_pcArrows[i].y1, &w, 4);
        w = pc_how_to_be32(lbl_1_data_42FA4, sizeof(lbl_1_data_42FA4), base + 0x08);
        memcpy(&s_pcArrows[i].x2, &w, 4);
        w = pc_how_to_be32(lbl_1_data_42FA4, sizeof(lbl_1_data_42FA4), base + 0x0C);
        memcpy(&s_pcArrows[i].y2, &w, 4);
        s_pcArrows[i].color = pc_how_to_be32(lbl_1_data_42FA4, sizeof(lbl_1_data_42FA4), base + 0x10);
    }
}
#endif

void g_how_to_sprite_draw_controller_tooltips(int side, struct Sprite *sprite, struct SpriteDrawRequest *req)
{
    float dx;
    float dy;
    float opacity;
    float z;
    float scaleX;
    float scaleY;
    float x1;
    int i;
    int j;
    struct HowToTooltip *tip;
    struct HowToBmp *bmp;
    s8 *list;
    s8 id;
    u32 color;

#ifndef __MWERKS__
    pc_fix_how_to_tables();
#endif
    opacity = side ? *(float *)&s_unk28 : *(float *)&s_unk20;
    list = ((s8 **)lbl_1_data_42F1C)[modeCtrl.gameType + side * 17];
    if (list != NULL)
    {
        for (i = 0; list[i] != -1; i++)
        {
            id = list[i];
            if (modeCtrl.gameType == 15 || modeCtrl.gameType == 16)
            {
                if (id == 0x22)
                    id = 1;
                else
                    id += 0x11;
            }
#ifdef __MWERKS__
            tip = &((struct HowToTooltip (*)[6])lbl_1_data_41A50)[id][locale];
#else
            tip = &((struct HowToTooltip (*)[6])s_pcTooltips)[id][locale];
#endif
            textdraw_set_depth(sprite->depth - 0.002);
            bmp = &((struct HowToBmp *)lbl_1_data_3D3D8)[tip->bmpIdx];
            req->bmpId = bmp->bmpId;
            req->x = tip->x;
            req->y = tip->y;
            req->x += 0.05f * (244.0f - req->x);
            if (id == 0x2A)
                req->addColor = 0x200080;
            else if (id == 0x2B)
                req->addColor = 0x80;
            else if (id == 0x2C)
                req->addColor = 0x400080;
            else
                req->addColor = 0;
            req->y += 10.0f;
            req->x -= 11.4;
            req->scaleX = 1.15f;
            draw_sprite_draw_request(req);
            req->x += 11.4f;
            req->scaleX = 1.0f;
            if (opacity < 0.01f)
                continue;
            for (j = 0; tip->lines[j] != NULL; j++)
            {
                float scale = j == 0 ? 0.541666 : 0.58;

                color = j == 0 ? 0xFFFFFF : 0;
                textdraw_set_alignment(3);
                textdraw_set_mul_color(color);
                textdraw_set_scale(0.8f * scale, scale);
                textdraw_set_pos(76.0f + req->x, req->y + bmp->lineY[j]);
                textdraw_print_and_fit_to_width(165.0f, tip->lines[j]);
                textdraw_set_alignment(0);
            }
            {
                struct HowToArrow *arrow;
                int flags;
                int mulColor;

                z = req->z;
                scaleX = req->scaleX;
                scaleY = req->scaleY;
                flags = req->flags;
                mulColor = req->mulColor;
#ifdef __MWERKS__
                arrow = &((struct HowToArrow *)lbl_1_data_42FA4)[id];
#else
                arrow = &s_pcArrows[id];
#endif
                x1 = arrow->x1 + 0.15f * (320.0f - arrow->x1);
                dx = arrow->x2 - x1;
                dy = arrow->y2 - arrow->y1;
                req->z += 0.001f;
                req->opacity = opacity;
                req->mulColor = arrow->color;
                if (arrow->color != 0)
                {
                    req->bmpId = 0xC2B;
                    req->x = 0.5 * dx + x1;
                    req->y = 0.5 * dy + arrow->y1;
                    req->scaleX = 1.0f;
                    req->scaleY = 0.125 * math_sqrt(dx * dx + dy * dy);
                    req->rotation = math_atan2(dx, dy);
                    req->flags = (sprite->flags & ~0xF) | 0xA;
                    req->y += 10.0f;
                    draw_sprite_draw_request(req);
                    req->bmpId = 0xC2A;
                    req->x = arrow->x2;
                    req->y = arrow->y2;
                    req->z -= 0.001;
                    req->scaleX = 1.0f;
                    req->scaleY = 1.0f;
                    req->rotation += 0x8000;
                    req->y += 10.0f;
                    draw_sprite_draw_request(req);
                    if (id == 0x16 || id == 0x2A || id == 0x2F || id == 0x3C)
                    {
                        dx = 140.0f + (arrow->x2 - x1);
                        req->bmpId = 0xC2B;
                        req->x = 0.5 * dx + x1;
                        req->y = 0.5 * dy + arrow->y1;
                        req->z = 0.0005f + (0.0005f + z);
                        req->scaleX = 1.0f;
                        req->scaleY = 0.125 * math_sqrt(dx * dx + dy * dy);
                        req->rotation = math_atan2(dx, dy);
                        req->y += 10.0f;
                        draw_sprite_draw_request(req);
                        req->bmpId = 0xC2A;
                        req->x = 140.0f + arrow->x2;
                        req->y = arrow->y2;
                        req->z -= 0.001;
                        req->scaleX = 1.0f;
                        req->scaleY = 1.0f;
                        req->rotation += 0x8000;
                        req->y += 10.0f;
                        draw_sprite_draw_request(req);
                    }
                    else if (id == 0x25 || id == 0x40)
                    {
                        if (id == 0x25)
                        {
                            dx = 70.0f;
                            dy = 21.0f;
                        }
                        else
                        {
                            dx = -70.0f;
                            dy = 36.0f;
                        }
                        req->bmpId = 0xC2B;
                        req->x = 0.5 * dx + x1;
                        req->y = 0.5 * dy + arrow->y1;
                        req->z = 0.0005f + (0.0005f + z);
                        req->scaleX = 1.0f;
                        req->scaleY = 0.125 * math_sqrt(dx * dx + dy * dy);
                        req->rotation = math_atan2(dx, dy);
                        req->y += 10.0f;
                        draw_sprite_draw_request(req);
                        req->bmpId = 0xC2A;
                        if (id == 0x25)
                            req->x = 390.0f;
                        else
                            req->x = 250.0f;
                        req->y = 161.0f;
                        req->z -= 0.001;
                        req->scaleX = 1.0f;
                        req->scaleY = 1.0f;
                        req->rotation += 0x8000;
                        req->y += 10.0f;
                        draw_sprite_draw_request(req);
                    }
                }
                req->z = z;
                req->scaleX = scaleX;
                req->scaleY = scaleY;
                req->rotation = 0;
                req->opacity = 1.0f;
                req->flags = flags;
                req->mulColor = mulColor;
                req->addColor = 0;
            }
        }
    }
}

struct HowToRule
{
    char *line1;
    char *line2;
};

#define HOWTO_ROW_IS_HEADER(page, i)                                                       \
    ((page) == 0 || ((page) == 3 && (i) == 0) || ((page) == 5 && (i) == 0)                 \
     || ((page) == 6 && ((i) == 0 || (i) == 3)) || ((page) == 0x67 && (i) == 0)            \
     || ((page) == 0x68 && (i) == 0) || ((page) == 0x69 && (i) == 0))

#define HOWTO_ICON(table, style, i) (((s32 **)(table))[style][i])

void g_how_to_sprite_draw_rules_page(s8 page, struct Sprite *sprite, struct SpriteDrawRequest *req, s8 rows, s8 cols, s8 style, s8 sel)
{
    float x;
    float y;
    float scale;
    s8 i;
    char *line1;
    char *line2;

    for (i = 0; i < rows; i++)
    {
        textdraw_set_alpha(1.0f);
        if (page == 0)
        {
            x = 42.0f;
            req->y = 400.0f * s_unk30 + ((float **)lbl_1_data_4F998)[rows][i];
        }
        else if (page >= 100)
        {
            x = 42.0f - 640.0f * s_unk3C;
            req->y = ((float **)lbl_1_data_4F998)[rows][i];
        }
        else
        {
            x = 42.0f + 640.0f * s_unk38;
            req->y = ((float **)lbl_1_data_4F998)[rows][i];
        }
        if (page == 0)
            req->mulColor = lbl_1_data_4F8C0[i];
        else if (page >= 100)
            req->mulColor = ((s32 *)lbl_1_data_4F944[page - 100])[i];
        else
            req->mulColor = ((s32 **)lbl_1_data_4F928)[page][i];
        scale = rows == 6 ? 0.86666 : 1.0;
        req->scaleY = scale;
        req->bmpId = 0xC01;
        req->x = x;
        if (HOWTO_ROW_IS_HEADER(page, i))
        {
            req->u1 = 0.0f;
            req->u2 = 0.333333f;
        }
        else
        {
            req->u1 = 0.333333f;
            req->u2 = 0.666666f;
        }
        req->scaleX = req->u2 - req->u1;
        draw_sprite_draw_request(req);
        req->bmpId = 0xC01;
        req->x = 32.0f + x;
        req->u1 = 0.333333f;
        req->u2 = 0.666666f;
        if (page == 0)
            req->scaleX = s_unk2C * (14.775 * (0.99401f * (1.046875f * (req->u2 - req->u1))));
        else
            req->scaleX = 14.775 * (0.99401f * (1.046875f * (req->u2 - req->u1)));
        draw_sprite_draw_request(req);
        req->scaleX = 1.0f;
        req->bmpId = 0xC01;
        if (page == 0)
            req->x = (32.0f + x) + 491.99704f * s_unk2C;
        else
            req->x = 491.99704f + (32.0f + x);
        req->u1 = 0.666666f;
        req->u2 = 1.0f;
        req->scaleX = req->u2 - req->u1;
        draw_sprite_draw_request(req);
        req->bmpId = 0xC02;
        req->x = 3.0f + x;
        req->y += 2.0f;
        req->z = sprite->depth - 0.001f;
        req->u1 = 0.0f;
        req->u2 = 1.0f;
        req->scaleX = scale * (req->u2 - req->u1);
        req->mulColor = 0xFFFFFF;
        if (HOWTO_ROW_IS_HEADER(page, i))
            draw_sprite_draw_request(req);
        if (page == 0 || (modeCtrl.gameType == 15 && i == 0))
            req->bmpId = HOWTO_ICON(lbl_1_data_4FAA4, style, i);
        else if (modeCtrl.gameType == 15)
            req->bmpId = lbl_1_data_4FBB0[i + ((page < 4 ? page : 4) - 3) * 4 - 1];
        else if (page >= 100)
            req->bmpId = ((s32 *)lbl_1_data_4FB98[page - 100])[i];
        else
            req->bmpId = ((s32 **)lbl_1_data_4FB7C)[page][i];
        req->x = 3.0f + x;
        req->y += (float)(rows == 6 ? 6 : 7);
        y = req->y;
        req->z = sprite->depth - 0.002f;
        req->u1 = 0.0f;
        req->u2 = 1.0f;
        if (page == 0 || req->bmpId == 15)
        {
            req->scaleX = 0.4 * scale;
            req->scaleY = 0.26 * scale;
        }
        else
        {
            req->x += 3.0f;
            if (modeCtrl.gameType == 15)
            {
                req->scaleX = 0.6875f;
                req->scaleY = 0.6875f;
            }
            else
            {
                switch (page)
                {
                case 1:
                case 0x65:
                    req->x += 2.0f;
                    if (i > 0)
                        req->x += 1.0f;
                    if (i == 0)
                        req->y -= 2.0f;
                    break;
                case 2:
                    req->x += 2.0f;
                    if (i > 0)
                        req->x += 2.0f;
                    if (i == 0)
                        req->y -= 2.0f;
                    break;
                case 3:
                    req->y -= 3.0f;
                    break;
                case 5:
                case 0x69:
                    req->y -= 1.0f;
                    break;
                case 6:
                    req->x += 4.0f;
                    break;
                case 0x67:
                    req->x += 4.0f;
                    break;
                }
                req->scaleX = 1.0f;
                req->scaleY = 1.0f;
            }
        }
        req->mulColor = 0xFFFFFF;
        if (req->bmpId != -1)
        {
            if (req->bmpId != 0)
            {
                if (page == 0 || req->bmpId == 15)
                {
                    switch (((frame_counter >> 2) + i * 4) & 0xF)
                    {
                    case 10:
                    case 15:
                        req->bmpId = HOWTO_ICON(lbl_1_data_4FAB4, style, i);
                        break;
                    case 11:
                    case 14:
                        req->bmpId = HOWTO_ICON(lbl_1_data_4FAC4, style, i);
                        break;
                    case 12:
                    case 13:
                        req->bmpId = HOWTO_ICON(lbl_1_data_4FAD4, style, i);
                        break;
                    default:
                        req->bmpId = HOWTO_ICON(lbl_1_data_4FAA4, style, i);
                        break;
                    }
                }
                draw_sprite_draw_request(req);
            }
            else
            {
                textdraw_set_scale(1.3f, 1.3f);
                textdraw_set_mul_color(0);
                textdraw_set_alignment(3);
                textdraw_set_pos(16.0f + (9.0f + req->x), 4.0f + req->y);
                textdraw_printf("%d", i + 1);
                textdraw_set_alignment(0);
                textdraw_set_scale(0.68f, 0.7f);
                textdraw_set_mul_color(0xFFFFC0);
            }
        }
        req->z = sprite->depth;
        req->scaleY = 1.0f;
        req->x = 3.0f + x;
        req->y = y;
        if (page == 0)
        {
            if (s_unk34 < 0.01f)
                continue;
            textdraw_set_alpha(s_unk34);
        }
        else
            textdraw_set_alpha(1.0f);
        if (req->x < -550.0f || req->x > 600.0f || req->y > 480.0f)
            continue;
        if (modeCtrl.gameType == 15 || modeCtrl.gameType == 16)
        {
            line1 = ((struct HowToRule *(*)[6])lbl_1_data_4C824)[s_mode][locale][i + cols * sel].line1;
            line2 = ((struct HowToRule *(*)[6])lbl_1_data_4C824)[s_mode][locale][i + cols * sel].line2;
        }
        else if (page == 0)
        {
            if (((void **)lbl_1_data_4C7E0)[modeCtrl.gameType] == NULL)
                continue;
            line1 = ((struct HowToRule (**)[6])lbl_1_data_4C7E0)[modeCtrl.gameType][i + cols * sel][locale].line1;
            line2 = ((struct HowToRule (**)[6])lbl_1_data_4C7E0)[modeCtrl.gameType][i + cols * sel][locale].line2;
        }
        else if (page >= 100)
        {
            if (((void **)lbl_1_data_4F87C)[modeCtrl.gameType] == NULL)
                continue;
            line1 = ((struct HowToRule (**)[6])lbl_1_data_4F87C)[modeCtrl.gameType][i + cols * sel][locale].line1;
            line2 = ((struct HowToRule (**)[6])lbl_1_data_4F87C)[modeCtrl.gameType][i + cols * sel][locale].line2;
        }
        else
        {
            if (((void **)lbl_1_data_4E51C)[modeCtrl.gameType] == NULL)
                continue;
            line1 = ((struct HowToRule (**)[6])lbl_1_data_4E51C)[modeCtrl.gameType][i + cols * sel][locale].line1;
            line2 = ((struct HowToRule (**)[6])lbl_1_data_4E51C)[modeCtrl.gameType][i + cols * sel][locale].line2;
        }
        textdraw_set_pos(req->x + (float)(rows == 6 ? 50 : 56), req->y - 1.0f);
        textdraw_print_and_fit_to_width(482.0f, line1);
        textdraw_set_pos(req->x + (float)(rows == 6 ? 50 : 56), req->y + (float)(rows == 6 ? 16 : 21));
        textdraw_print_and_fit_to_width(482.0f, line2);
    }
}

#define HOWTO_MIN(a, b) ((a) < (b) ? (a) : (b))

void sprite_how_to_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    char *text;
    float width;
    s8 sel;
    s8 rows;
    s8 cols;
    s8 style;
    s8 page;
    int isStory;

    g_how_to_sprite_mtx_setup_stuff();
    req.z = sprite->depth;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.opacity = 1.0f;
    req.rotation = 0;
    req.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | 0xFFFFFF;
    req.addColor = 0;
    req.unk30 = -1;
    req.flags = (sprite->flags & ~0xF) | 5;
    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_depth(sprite->depth);
    textdraw_set_flags(0x80000000);
    GXSetZMode_cached(1, 7, 1);
    req.bmpId = 0x4B;
    req.x = 320.1f;
    req.y = 240.1f;
    req.z = 0.003 + sprite->depth;
    req.scaleX = s_unk10;
    req.scaleY = s_unk14;
    req.opacity = 0.6666f;
    req.mulColor = 0xC0C030;
    req.flags = (sprite->flags & ~0xF) | 0xA;
    draw_sprite_draw_request(&req);
    req.z = sprite->depth;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.opacity = 1.0f;
    req.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | 0xFFFFFF;
    req.flags = (sprite->flags & ~0xF) | 5;
    GXSetZMode_cached(1, 3, 1);
    req.bmpId = 0xC00;
    req.x = 112.0f;
    req.y = 22.0f - 56.0f * s_alpha;
    draw_sprite_draw_request(&req);
    if (s_pageCount != 0)
    {
        text = ((char *(**)[6])lbl_1_data_3D394)[modeCtrl.gameType][s_titleIdx][locale];
        textdraw_set_drop_shadow();
        textdraw_set_alignment(3);
        textdraw_set_depth(sprite->depth - 0.002);
        textdraw_set_mul_color(0xFF8000);
        textdraw_set_pos(320.0f, 15.0f + req.y);
        textdraw_print_and_fit_to_width(384.0f, text);
    }
    {
        char *pagesText[6] = { "pages", " ", " ", " ", " ", "ページ" };
        textdraw_clear_drop_shadow();
        textdraw_set_alignment(3);
        textdraw_set_scale(0.5f, 0.5f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(550.0f, 15.0f + req.y);
        textdraw_printf_and_fit_to_width(34.0f, "%d/%d\n%s", s_page + 1, s_pageCount, pagesText[locale]);
        textdraw_set_depth(sprite->depth);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_alignment(0);
        textdraw_set_mul_color(0);
        textdraw_set_scale(0.6f, 0.6f);
        textdraw_set_alignment(4);
    }
    {
        char *backText[6] = { "Back", "Zur\xFC" "ck", "Retour", "Atr\xE1s", "Indietro", "まえ" };
        text = backText[locale];
        req.bmpId = 0xC27;
        req.x = 44.0f - 40.0f * s_alpha;
        req.y = 46.0f;
        req.flags = (sprite->flags & ~0xF) | 0xA;
        draw_sprite_draw_request(&req);
        textdraw_set_pos(req.x, 32.0f + req.y);
        textdraw_print_and_fit_to_width(45.0f, text);
    }
    {
        char *nextText[6] = { "Next", "Weiter", "Suiv.", "Sig.", "Succ.", "つぎ" };
        text = nextText[locale];
        req.x = 596.0f + 40.0f * s_alpha;
        req.y = 46.0f;
        req.flags = (sprite->flags & ~0xF) | 0x8000A;
        draw_sprite_draw_request(&req);
        textdraw_set_pos(req.x, 32.0f + req.y);
        textdraw_print_and_fit_to_width(45.0f, text);
    }
    req.flags = (sprite->flags & ~0xF) | 5;
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_alignment(0);
    {
        char *moveText[6] = { "Movement", "Bewegen", "D\xE9pl.", "Mov.", "Muovi", "いどう" };
        text = moveText[locale];
        textdraw_set_depth(sprite->depth - 0.002f);
        textdraw_set_scale(0.8f, 1.0f);
        width = textdraw_get_pixel_width_of_string(text);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(20.0f - (20.0f + width) * s_alpha, 440.0f);
        textdraw_print("\x83\xAD");
        textdraw_set_mul_color(0);
        textdraw_set_pos(30.0f + (20.0f - (20.0f + width) * s_alpha), 440.0f);
        textdraw_print(text);
        textdraw_set_depth(sprite->depth);
    }
    {
        char *returnText[6] = { "Return", "", "", "", "", "もどる" };
        text = returnText[locale];
        textdraw_set_scale(0.8f, 1.0f);
        width = textdraw_get_pixel_width_of_string(text);
        textdraw_set_mul_color(0);
        textdraw_set_pos((620.0f - width) + (20.0f + width) * s_alpha, 440.0f);
        textdraw_print(text);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos((620.0f - width) + (20.0f + width) * s_alpha - 28.0f, 440.0f);
        textdraw_print("\x83\xA0");
    }
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    if (s_pageCount == 0)
        return;
    req.bmpId = 0xC28;
    req.x = 320.0f;
    req.y = 264.0f;
    req.scaleX = s_unk18;
    req.scaleY = s_unk18;
    req.rotation = 65535.0 * (1.0 - s_unk18);
    req.flags = (sprite->flags & ~0xF) | 0xA;
    draw_sprite_draw_request(&req);
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.flags = (sprite->flags & ~0xF) | 5;
    textdraw_set_font_style(2);
    req.scaleX = 1.0f;
    req.scaleY = s_unk1C;
    req.z = sprite->depth - 0.001;
    textdraw_set_alpha(s_unk20);
    g_how_to_sprite_draw_controller_tooltips(0, sprite, &req);
    req.scaleX = 1.0f;
    req.scaleY = s_unk24;
    req.z = sprite->depth - 0.001;
    textdraw_set_alpha(s_unk28);
    g_how_to_sprite_draw_controller_tooltips(1, sprite, &req);
    req.z = sprite->depth;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.flags = (sprite->flags & ~0xF) | 5;
    req.addColor = 0;
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_depth(sprite->depth);
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_alpha(1.0f);
    textdraw_set_scale(0.68f, 0.7f);
    textdraw_set_mul_color(0xFFFFC0);
    textdraw_set_depth(sprite->depth - 0.002);

    page = 0;
    rows = 4;
    cols = 4;
    sel = 0;
    isStory = FALSE;
    if (modeCtrl.gameType == 0 || modeCtrl.gameType == 8)
        isStory = TRUE;
    style = isStory ? 1 : 0;
    switch (modeCtrl.gameType)
    {
    case 3:
        if (main_mode == 1)
        {
            if (s_shownPage > 0)
                sel = HOWTO_MIN(s_shownPage - 1, 2);
        }
        else
            sel = g_focused_root_menu[15];
        break;
    case 4:
        if (main_mode == 1)
        {
            if (s_shownPage > 0)
                sel = HOWTO_MIN(s_shownPage - 1, 1);
        }
        else
            sel = g_focused_root_menu[21];
        break;
    case 6:
        if (main_mode == 1)
        {
            if (s_shownPage > 1)
                sel = HOWTO_MIN(s_shownPage - 2, 3);
        }
        else if (lbl_1_bss_16F0[6] == 1)
            sel = lbl_6_bss_C;
        else
            sel = lbl_1_bss_16F0[7];
        break;
    case 7:
        if (main_mode == 1)
        {
            if (s_shownPage > 0)
            {
                sel = s_shownPage - 1;
                if (sel == 1)
                    sel = 3;
                else if (sel == 2)
                    sel = 4;
                else if (sel == 3)
                    sel = 2;
                else if (sel == 4)
                    sel = 1;
            }
            cols = 5;
        }
        else
        {
            sel = lbl_1_bss_1700;
            cols = 5;
        }
        if (sel != 1)
        {
            rows = 5;
            style = 2;
        }
        break;
    case 8:
        if (main_mode == 1)
        {
            if (s_shownPage > 1)
                sel = s_shownPage - 1;
        }
        else
            sel = lbl_1_bss_1704;
        break;
    case 10:
        if (main_mode == 1)
        {
            if (s_shownPage > 0)
                sel = HOWTO_MIN(s_shownPage - 1, 2);
        }
        else
            sel = g_focused_root_menu[43];
        style = 3;
        cols = 5;
        if (sel != 2)
            rows = 5;
        break;
    case 12:
        if (main_mode == 1)
        {
            if (s_shownPage > 0)
                sel = s_shownPage - 1;
        }
        else
            sel = lbl_1_bss_1720;
        cols = 5;
        if (sel != 2)
            rows = 5;
        break;
    case 9:
        cols = 6;
        rows = 6;
        break;
    case 13:
        if (main_mode == 1)
        {
            if (s_shownPage > 0)
                sel = HOWTO_MIN(s_shownPage - 1, 1);
        }
        else
            sel = lbl_1_bss_1718[1];
        cols = 6;
        rows = sel == 0 ? 6 : 5;
        break;
    case 15:
        rows = 5;
        cols = 5;
        break;
    case 16:
        rows = 6;
        if (((s8 *)lbl_1_bss_1738)[10] == 0)
        {
            if (locale == 5)
            {
                ((char **)lbl_1_data_4C5D8)[8] = "きめられたゲームすうを おたがいに とったときは";
                ((char **)lbl_1_data_4C5D8)[9] = "そのあと ２ゲームの さがでるまで つづけます。";
            }
            else
            {
                ((char **)lbl_1_data_4C7B0)[8] = "If both players win the determined number of games,";
                ((char **)lbl_1_data_4C7B0)[9] = "you will continue play until the difference is 2 games";
            }
        }
        else
        {
            if (locale == 5)
            {
                ((char **)lbl_1_data_4C5D8)[8] = "きめられたゲームすうを おたがいがとったときは";
                ((char **)lbl_1_data_4C5D8)[13] = "タイブレークで、さいごのゲームをとったほうが かちです。";
            }
            else
            {
                ((char **)lbl_1_data_4C7B0)[8] = "If both players get the determined number of the games, you";
                ((char **)lbl_1_data_4C7B0)[9] = "will have a tie break. The player who gets the last game wins.";
            }
        }
        break;
    }
    g_how_to_sprite_draw_rules_page(page, sprite, &req, rows, cols, style, sel);

    sel = 0;
    switch (modeCtrl.gameType)
    {
    case 3:
        page = 1;
        rows = 5;
        cols = 5;
        break;
    case 4:
        page = 2;
        rows = 4;
        cols = 4;
        break;
    case 5:
        page = 3;
        rows = 6;
        cols = 6;
        break;
    case 9:
        page = 5;
        rows = 4;
        cols = 4;
        break;
    case 10:
        page = 4;
        rows = 5;
        cols = 5;
        break;
    case 13:
        page = 6;
        rows = 4;
        cols = 4;
        break;
    case 15:
        page = 3;
        rows = 5;
        cols = 5;
        sel = page - 2;
        break;
    }
    if (page != 0)
        g_how_to_sprite_draw_rules_page(page, sprite, &req, rows, cols, style, sel);

    page = 0;
    sel = 0;
    switch (modeCtrl.gameType)
    {
    case 3:
        page = 0x65;
        rows = 5;
        cols = 5;
        break;
    case 9:
        page = 0x69;
        rows = 4;
        cols = 4;
        break;
    case 15:
        page = 0x68;
        rows = 5;
        cols = 5;
        sel = page - 0x66;
        break;
    }
    if (page != 0)
        g_how_to_sprite_draw_rules_page(page, sprite, &req, rows, cols, style, sel);
    g_how_to_sprite_mtx_cleanup();
}

void sprite_how_to_dest(struct Sprite *sprite)
{
    call_free_bmp_by_id(12);
}

void g_how_to_sprite_mtx_setup_stuff(void)
{
    float v;
    float x;
    float y;

    mtxa_from_identity();
    mtxa_translate_xyz(320.0f, 240.0f, 0.0f);
    v = s_zoom;
    v = 1.0 / ((1.0 - v) + 0.625 * v);
    mtxa_scale_xyz(v, v, 1.0f);
    mtxa_translate_neg_xyz(320.0f, 240.0f, 0.0f);
    v = s_panX / 30.0;
    x = 120.0 * v;
    v = s_panY / 30.0;
    y = 90.0 * v;
    mtxa_translate_xyz(-x, -y, 0.0f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void g_how_to_sprite_mtx_cleanup(void)
{
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}
