/* In-game HUD: the monkey face sprites next to the lives counter. */
#include "types.h"
#include "ball.h"
#include "bitmap.h"
#include "dip.h"
#include "mathutil.h"
#include "mode.h"
#include "sprite.h"
#include "textdraw.h"

struct HudMonkeyCounterState
{
    /*0x00*/ s32 state;
    /*0x04*/ s32 prevState;
    /*0x08*/ s32 timer;
    /*0x0C*/ s16 angle;
    /*0x10*/ s32 counter;
    /*0x14*/ s32 mode;
    u8 unk18[4];
}; /* size = 0x1C */

struct TextBoxState
{
    /*0x00*/ s32 state;
    u8 unk4[4];
    /*0x08*/ u32 timer;
    u8 unkC[0x34 - 0xC];
}; /* size = 0x34 */

extern s32 active_monkey_id[];
extern s32 locale;
extern s32 g_sub_mode3;
extern u32 g_font_char_var_3;
extern char *lbl_1_data_3204[];
extern struct TextBoxState lbl_1_bss_A5C40[];

void reset_ui_widescreen_scale_mtx(void);
void set_ui_widescreen_scale_mtx(int width);
float textdraw_get_pixel_width_of_string_as_f32(char *str);
struct Sprite *get_sprite_with_unique_id(int tag);
int g_smth_with_get_active_monkey_icon(int state, int timer, int id);

const int lbl_1_rodata_A838[4] = {0x0F, 0x20, 0x33, 0x03};
static const int s_iconsSmile[48] = {
    0x15, 0x16, 0x17, 0x17, 0x16, 0x16, 0x16, 0x17, 0x16, 0x16, 0x16, 0x17,
    0x25, 0x26, 0x27, 0x27, 0x26, 0x26, 0x26, 0x27, 0x26, 0x26, 0x26, 0x27,
    0x37, 0x38, 0x39, 0x39, 0x38, 0x38, 0x38, 0x39, 0x38, 0x38, 0x38, 0x39,
    0x0B, 0x0E, 0x4D, 0x4D, 0x0E, 0x0E, 0x0E, 0x4D, 0x0E, 0x0E, 0x0E, 0x4D,
};
static const int s_iconsLook[48] = {
    0x18, 0x19, 0x18, 0x0F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0F, 0x18, 0x19,
    0x28, 0x29, 0x28, 0x20, 0x28, 0x28, 0x28, 0x28, 0x28, 0x20, 0x28, 0x29,
    0x3A, 0x3B, 0x3A, 0x33, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x33, 0x3A, 0x3B,
    0x4E, 0x4F, 0x4E, 0x03, 0x4E, 0x4E, 0x4E, 0x4E, 0x4E, 0x03, 0x4E, 0x4F,
};
static const int s_iconsScared[24] = {
    0x45, 0x46, 0x47, 0x48, 0x47, 0x48, 0x2E, 0x2F, 0x30, 0x31, 0x30, 0x31,
    0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x57, 0x5B, 0x5C, 0x5D, 0x5C, 0x5D,
};
static const int s_iconsBlink[12] = {
    0x18, 0x19, 0x18, 0x28, 0x29, 0x28, 0x3A, 0x3B, 0x3A, 0x4E, 0x4F, 0x4E,
};
static const int s_iconsCry[48] = {
    0x10, 0x11, 0x12, 0x12, 0x11, 0x11, 0x11, 0x12, 0x11, 0x11, 0x11, 0x12,
    0x21, 0x22, 0x23, 0x24, 0x23, 0x23, 0x23, 0x24, 0x23, 0x23, 0x23, 0x24,
    0x34, 0x35, 0x36, 0x36, 0x35, 0x35, 0x35, 0x36, 0x35, 0x35, 0x35, 0x36,
    0x04, 0x07, 0x08, 0x08, 0x07, 0x07, 0x07, 0x08, 0x07, 0x07, 0x07, 0x08,
};
static const int s_iconsDazed[4] = {0x12, 0x24, 0x36, 0x08};
static const int s_iconsSad[12] = {
    0x14, 0x14, 0x13, 0x21, 0x22, 0x23, 0x34, 0x35, 0x36, 0x0A, 0x0A, 0x09,
};
static const int s_iconsHappy[16] = {
    0x1A, 0x1B, 0x1C, 0x1D, 0x2A, 0x2B, 0x2C, 0x2D,
    0x3C, 0x3D, 0x3E, 0x3F, 0x51, 0x53, 0x54, 0x56,
};
static const int s_iconsHappy2[16] = {
    0x1A, 0x1B, 0x1C, 0x1D, 0x2A, 0x2B, 0x2C, 0x2D,
    0x3C, 0x3D, 0x3E, 0x3F, 0x51, 0x53, 0x54, 0x56,
};
static const int s_iconsOut[4] = {0x1E, 0x32, 0x42, 0x5E};
static const s8 s_apeEmotionStates[16] = {0, 3, 3, 0x10, 0x10, 0xE, 0xE, 0};

s8 lbl_1_bss_A8738;
struct HudMonkeyCounterState lbl_1_bss_A873C;

void sprite_monkey_counter_icon_disp(struct Sprite *sprite)
{
    struct Ball *ball;
    int i;
    int icon;
    s16 angle;
    int changed;
    int flag;
    int count;
    struct SpriteDrawRequest req;
    double scale;
    float speed;
    double kmh;
    static int fadeIcon = 0xF;

    flag = lbl_1_bss_A8738;  /* dead read: puts this variable first in .bss */
    ball = current_ball;
    speed = mathutil_vec_len(&ball->vel);
    kmh = speed * 216000.0 / 1000.0;
    if (!(g_some_other_flags & 0xA))
        lbl_1_bss_A873C.timer++;
    flag = 0;
    if (lbl_1_bss_A873C.timer > 0x20)
    {
        changed = 0;
        if (sub_mode == 0x3B && !(mode_info.flags & INFO_FLAG_BONUS_STAGE) && modeCtrl.submodeTimer <= 0x20)
        {
            lbl_1_bss_A873C.state = 0xA;
        }
        else if (sub_mode == 0x3B && !(mode_info.flags & INFO_FLAG_BONUS_STAGE))
        {
            lbl_1_bss_A873C.state = 8;
            if (!(mode_info.flags & INFO_FLAG_BONUS_STAGE) && ball->unk2 == 5)
                lbl_1_bss_A873C.counter = -600;
        }
        else if (sub_mode == 0x39)
        {
            lbl_1_bss_A873C.state = 9;
        }
        else if (get_sprite_with_unique_id(0x10) != NULL)
        {
            lbl_1_bss_A873C.state = 3;
        }
        else if (ball->ape != NULL && ball->ape->unk78 >= 0 && ball->ape->unk7C >= 0)
        {
            switch (ball->ape->unk78)
            {
            case 1:
                lbl_1_bss_A873C.state = s_apeEmotionStates[ball->ape->unk7C];
                break;
            case 4:
                lbl_1_bss_A873C.state = 0xE;
                break;
            case 7:
                lbl_1_bss_A873C.state = 3;
                break;
            case 8:
                lbl_1_bss_A873C.state = 9;
                break;
            default:
                lbl_1_bss_A873C.state = 0;
                break;
            }
        }
        else
        {
            lbl_1_bss_A873C.state = 0;
        }
        if (lbl_1_bss_A873C.timer == 0xF0)
            changed = 1;
        if (lbl_1_bss_A873C.state != lbl_1_bss_A873C.prevState || changed != 0)
        {
            lbl_1_bss_A873C.timer = 0;
            switch (lbl_1_bss_A873C.prevState)
            {
            case 1:
                lbl_1_bss_A873C.state = 5;
                break;
            case 2:
                lbl_1_bss_A873C.state = 6;
                break;
            case 3:
            case 11:
                if (lbl_1_bss_A873C.state != 0xB)
                    lbl_1_bss_A873C.state = 4;
                break;
            case 14:
                lbl_1_bss_A873C.state = 0xF;
                break;
            case 16:
                lbl_1_bss_A873C.state = 0x11;
                break;
            }
            if (lbl_1_bss_A873C.state == 8)
                lbl_1_bss_A873C.angle = 1800.0f * math_sin(lbl_1_bss_A873C.angle);
            lbl_1_bss_A873C.prevState = lbl_1_bss_A873C.state;
        }
    }
    if (main_mode != 2)
    {
        if (lbl_1_bss_A873C.mode == 0xE && modeCtrl.submodeTimer == 0x13EB)
            lbl_1_bss_A873C.counter = 0x20;
        if (lbl_1_bss_A873C.counter > 0 && !(g_some_other_flags & 0xA))
            lbl_1_bss_A873C.counter--;
        if (sprite->userVar == 1)
        {
            sprite->userVar = 2;
            lbl_1_bss_A873C.counter = -0x1F;
        }
        if (lbl_1_bss_A873C.counter < -1 && !(g_some_other_flags & 0xA))
            lbl_1_bss_A873C.counter++;
    }
    else
    {
        if (lbl_1_bss_A873C.counter > 0 && !(g_some_other_flags & 0xA))
            lbl_1_bss_A873C.counter--;
        else if (get_sprite_with_unique_id(0x10) != NULL)
            get_sprite_with_unique_id(0x10);
        if (lbl_1_bss_A873C.counter < 0 && !(g_some_other_flags & 0xA))
        {
            if (sub_mode == 0x30 && lbl_1_bss_A873C.counter < -0x20)
                lbl_1_bss_A873C.counter = -0x20;
            lbl_1_bss_A873C.counter++;
        }
    }
    icon = g_smth_with_get_active_monkey_icon(lbl_1_bss_A873C.state, lbl_1_bss_A873C.timer,
        active_monkey_id[modeCtrl.currPlayer]);
    if (!(g_some_other_flags & 0xA))
    {
        if (lbl_1_bss_A8738 == 0 && fadeIcon != icon)
        {
            lbl_1_bss_A8738 = 3;
        }
        else if (lbl_1_bss_A8738 > 0)
        {
            if (--lbl_1_bss_A8738 == 0)
                fadeIcon = icon;
        }
    }
    angle = lbl_1_bss_A873C.angle;
    if (!(g_some_other_flags & 0xA))
    {
        switch (lbl_1_bss_A873C.state)
        {
        default:
        case 0:
        case 3:
        case 4:
        case 5:
        case 11:
            angle += 0x180;
            break;
        case 1:
            angle += 0x280;
            break;
        case 6:
        case 7:
            angle += 0x100;
            break;
        case 2:
            angle += 0x80;
            break;
        case 8:
            angle = (s16)(angle - (lbl_1_bss_A873C.timer << 5 < 0x1000 ? lbl_1_bss_A873C.timer << 5 : 0x1000));
            break;
        case 10:
        case 13:
            angle = (s16)angle + 0.1 * -(u16)angle;
            break;
        case 12:
            angle = (s16)angle + 0.1 * -(s16)angle;
            break;
        case 9:
            break;
        }
    }
    lbl_1_bss_A873C.angle = angle;
    switch (lbl_1_bss_A873C.state)
    {
    default:
        angle = 1800.0f * math_sin(angle);
        break;
    case 8:
    case 10:
    case 13:
        angle = lbl_1_bss_A873C.angle;
        break;
    }
    if (ball->unk2 < 5 || (ball->unk2 == 5 && lbl_1_bss_A873C.counter > 0) || main_mode != 2 || flag != 0)
    {
        count = lbl_1_bss_A873C.mode == 0xE ? 3 : 3;
        scale = (float)(lbl_1_bss_A873C.mode == 0xE ? 0.3 : (main_mode == 1 ? 1.0 : 0.5));
        for (i = 0; i < count - 1; i++)
        {
            req.bmpId = main_mode == 1 ? g_smth_with_get_active_monkey_icon(lbl_1_bss_A873C.state, lbl_1_bss_A873C.timer, i) : icon;
            if (flag != 0)
            {
                req.x = sprite->x + 10.0f * math_sin(g_font_char_var_3 * 0x60 + 0x4000);
            }
            else if (main_mode == 1)
            {
                req.x = sprite->x + (i * 0x5A);
                if (lbl_1_bss_A873C.counter < 0)
                    req.x += (lbl_1_bss_A873C.counter + 0x1F) * 5;
            }
            else if (main_mode != 2)
            {
                textdraw_reset();
                textdraw_set_font(3);
                textdraw_set_flags(0x80000000);
                reset_ui_widescreen_scale_mtx();
                req.x = sprite->x + i * textdraw_get_pixel_width_of_string_as_f32(lbl_1_data_3204[locale]);
                set_ui_widescreen_scale_mtx(0x140);
                if (lbl_1_bss_A873C.counter < 0)
                    req.x += i == 0 ? (lbl_1_bss_A873C.counter + 0x1F) * -5 : (lbl_1_bss_A873C.counter + 0x1F) * 5;
            }
            else if (count == 5 && lbl_1_bss_A873C.counter > 0)
            {
                req.x = sprite->x + 0.03125 * lbl_1_bss_A873C.counter * (i * 0x50);
            }
            else if (lbl_1_bss_A873C.counter < 0)
            {
                req.x = sprite->x + 0.03125 * (lbl_1_bss_A873C.counter + 0x20) * (i * 0x50);
            }
            else
            {
                req.x = sprite->x + (lbl_1_bss_A873C.mode == 0x33 ? 0 : i * 0x50);
            }
            if (lbl_1_bss_A873C.mode == 0x33 && i == 0)
                req.x += 4.0f;
            if (flag != 0)
            {
                req.y = sprite->y + 4.0f * math_sin(g_font_char_var_3 << 9);
            }
            else if (main_mode != 2)
            {
                if (lbl_1_bss_A873C.counter > 0)
                {
                    if (lbl_1_bss_A873C.counter >= 0x10)
                        req.y = sprite->y - 3.0 * (lbl_1_bss_A873C.counter - 0x10);
                    else
                        req.y = sprite->y - 12.0f * math_sin((0x10 - lbl_1_bss_A873C.counter) << 11);
                }
                else
                {
                    req.y = sprite->y;
                }
            }
            else if (i == count - 2)
            {
                if (lbl_1_bss_A873C.state == 0xB)
                {
                    if (lbl_1_bss_A873C.timer < 0x10)
                        req.y = sprite->y - 3.0 * (0x10 - lbl_1_bss_A873C.timer);
                    else if (lbl_1_bss_A873C.timer < 0x20)
                        req.y = sprite->y - 12.0f * math_sin((lbl_1_bss_A873C.timer - 0x10) << 11);
                    else
                        req.y = sprite->y;
                }
                else
                {
                    if (lbl_1_bss_A873C.counter > 0)
                        continue;
                    req.y = sprite->y;
                }
            }
            else
            {
                req.y = sprite->y;
            }
            if (lbl_1_bss_A873C.mode == 0x33 && i == 0)
                req.y += 4.0f;
            if (g_sub_mode3 == 0xE && lbl_1_bss_A873C.counter < 0)
                req.rotation = (s16)angle + (i == 0 ? (lbl_1_bss_A873C.counter + 0x1F) << 11 : -((lbl_1_bss_A873C.counter + 0x1F) << 11));
            else if (main_mode != 2 && lbl_1_bss_A873C.counter < 0)
                req.rotation = (s16)angle + (int)-((lbl_1_bss_A873C.counter + 0x1F) << 11);
            else
                req.rotation = (s16)angle;
            if (flag != 0)
            {
                struct TextBoxState *box = &lbl_1_bss_A5C40[1];

                if (box->state == 1)
                    req.opacity = 0.25 * box->timer / 30.0;
                else if (box->state >= 0x14)
                    req.opacity = 0.25 * (0x1E - box->timer) / 30.0;
                else if (box->state >= 0xA)
                    req.opacity = 0.25f;
                else
                    req.opacity = 0.0f;
            }
            else if (main_mode != 2 && lbl_1_bss_A873C.counter > 0)
            {
                req.opacity = 1.0f;
            }
            else if (main_mode != 2 && lbl_1_bss_A873C.counter < 0)
            {
                req.opacity = -0.032258 * lbl_1_bss_A873C.counter;
            }
            else
            {
                req.opacity = (lbl_1_bss_A873C.mode == 0x33 && i == 0) ? 0.3f : 1.0f;
            }
            if (main_mode == 1 && i != active_monkey_id[modeCtrl.currPlayer])
                req.opacity *= 0.6;
            if (main_mode == 1 && i != active_monkey_id[modeCtrl.currPlayer])
            {
                req.scaleX = 0.5 * scale;
                req.scaleY = 0.5 * (0.65 * scale);
            }
            else
            {
                req.scaleX = scale;
                req.scaleY = 0.65 * scale;
            }
            if (main_mode == 1 && i == active_monkey_id[modeCtrl.currPlayer])
                req.z = sprite->depth + 0.01 * i - 0.02;
            else
                req.z = sprite->depth + (lbl_1_bss_A873C.mode == 0x33 ? 0.0 : 0.01 * i);
            if (lbl_1_bss_A873C.mode == 0x33 && i == 0)
                req.z += 0.01f;
            req.u1 = 0.0f;
            req.v1 = 0.0f;
            req.u2 = 1.0f;
            req.v2 = 1.0f;
            req.unk30 = -1;
            req.flags = 0x100A;
            req.mulColor = (lbl_1_bss_A873C.mode == 0x33 && i == 0) ? 0 : -1;
            req.addColor = 0;
            draw_sprite_draw_request(&req);
            if (lbl_1_bss_A873C.mode == 0x33 && i == 1)
            {
                req.bmpId = fadeIcon;
                req.z = sprite->depth - 0.001f;
                req.opacity = 0.25f * lbl_1_bss_A8738;
                draw_sprite_draw_request(&req);
            }
            if (flag != 0)
            {
                req.bmpId = g_smth_with_get_active_monkey_icon(-1, 0, active_monkey_id[modeCtrl.currPlayer]);
                req.z = sprite->depth - 0.01;
                draw_sprite_draw_request(&req);
            }
        }
    }
    else
    {
        req.bmpId = icon;
        req.x = sprite->x;
        req.y = sprite->y;
        req.z = sprite->depth;
        req.scaleX = 0.5f;
        req.scaleY = 0.325f;
        req.u1 = 0.0f;
        req.v1 = 0.0f;
        req.u2 = 1.0f;
        req.v2 = 1.0f;
        req.rotation = (s16)angle;
        req.opacity = 1.0f;
        req.unk30 = -1;
        req.flags = 0x100A;
        req.mulColor = -1;
        req.addColor = 0;
        draw_sprite_draw_request(&req);
        if (lbl_1_bss_A873C.state == 0xB)
        {
            if (lbl_1_bss_A873C.timer < 0x10)
                req.y = sprite->y - 3.0 * (0x10 - lbl_1_bss_A873C.timer);
            else if (lbl_1_bss_A873C.timer < 0x20)
                req.y = sprite->y - 12.0f * math_sin((lbl_1_bss_A873C.timer - 0x10) << 11);
            else
                req.y = sprite->y;
            req.z = sprite->depth - 0.01;
            req.opacity = 0.03125 * lbl_1_bss_A873C.timer;
            draw_sprite_draw_request(&req);
        }
    }
}

int g_smth_with_get_active_monkey_icon(int state, int timer, int id)
{
    int frame;

    switch (state)
    {
    default:
    case 0:
        return lbl_1_rodata_A838[id];
    case 1:
        frame = timer >> 3;
        return s_iconsSmile[id * 12 + (frame < 11 ? frame : 11)];
    case 2:
        frame = timer >> 4;
        return s_iconsLook[id * 12 + (frame < 11 ? frame : 11)];
    case 3:
    case 11:
        frame = timer >> 3;
        return s_iconsScared[id * 6 + (frame < 5 ? frame : 5)];
    case 4:
        frame = timer >> 3;
        return s_iconsScared[id * 6 + (frame < 5 ? 5 - frame : 0)];
    case 5:
        frame = timer >> 3;
        return s_iconsSmile[id * 12 + (frame < 3 ? 3 - frame : 0)];
    case 6:
        frame = timer >> 4;
        return s_iconsBlink[id * 3 + (frame < 2 ? 2 - frame : 0)];
    case 7:
        return lbl_1_rodata_A838[id];
    case 8:
    case 14:
        frame = timer >> 3;
        return s_iconsCry[id * 12 + (frame < 11 ? frame : 11)];
    case 10:
    case 15:
        frame = timer >> 3;
        return s_iconsSad[id * 3 + (frame < 2 ? 2 - frame : 0)];
    case 13:
        return s_iconsDazed[id];
    case 9:
    case 16:
        frame = timer >> 3;
        return s_iconsHappy[id * 4 + (frame < 3 ? frame : 3)];
    case 12:
    case 17:
        frame = timer >> 3;
        return s_iconsHappy2[id * 4 + (frame < 3 ? 3 - frame : 0)];
    case -1:
        return s_iconsOut[id];
    }
}
