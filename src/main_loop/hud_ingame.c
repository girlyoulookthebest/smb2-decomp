/* In-game HUD sprites: score, timer, speed, stage name, banana and monkey counters. */
#include "types.h"
#include "ball.h"
#include "bitmap.h"
#include "course.h"
#include "dip.h"
#include "libc.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "sprite.h"
#include "textdraw.h"

struct Keyframe
{
    /*0x00*/ s32 frame;
    /*0x04*/ float value;
    /*0x08*/ float tangentIn;
    /*0x0C*/ float tangentOut;
}; /* size = 0x10 */

struct StageSelectState
{
    /*0x00*/ s8 isStoryMode;
    u8 unk1[0x17 - 0x1];
    /*0x17*/ u8 flags;
};

struct HudMonkeyCounterState
{
    /*0x00*/ s32 unk0;
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;
    u8 unkC[0x14 - 0xC];
    /*0x14*/ s32 unk14;
};

extern s32 locale;
extern char current_stage_name[];
extern struct StageSelectState stageselect_is_storymode;
extern struct HudMonkeyCounterState lbl_1_bss_A873C;
extern s32 lbl_1_bss_6FB68;  /* debug menu yint */
extern s32 lbl_1_bss_6FB6C;  /* debug menu xint */
extern float lbl_1_bss_6FB80;  /* debug menu ypos */
extern float lbl_1_bss_6FB84;  /* debug menu xpos */

void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_spacing(float x, float y);
void textdraw_set_alpha(float alpha);
void textdraw_set_mul_color(u32 color);
void textdraw_set_depth(float depth);
void textdraw_set_pos(float x, float y);
void textdraw_set_flags(u32 flags);
void textdraw_set_alignment(s8 alignment);
void textdraw_printf(char *fmt, ...);
float textdraw_get_pixel_width_of_string_as_f32(char *str);
struct Sprite *get_sprite_with_unique_id(int tag);
float fn_1_D2550(float frame, struct Keyframe *curve);
void sprite_final_stage_tick(s8 *status, struct Sprite *sprite);
void sprite_final_stage_disp(struct Sprite *sprite);
void sprite_debug_course_display_disp(struct Sprite *sprite);
void sprite_monkey_counter_icon_disp(struct Sprite *sprite);

void create_score_sprites(float x, float y);
void create_timer_sprites(float x, float y);
void create_speed_sprites(float x, float y);
void create_hud_stage_name_sprites(float x, float y);
void create_banana_counter_sprites(float x, float y);
void create_monkey_counter_sprites(float x, float y);
void create_final_stage_sprite(void);
void create_debug_course_display_sprites(void);
void sprite_score_tick(s8 *status, struct Sprite *sprite);
void sprite_score_disp(struct Sprite *sprite);
void fn_1_C9970(struct Sprite *sprite);
void sprite_timer_ball_tick(s8 *status, struct Sprite *sprite);
void sprite_speed_tick(s8 *status, struct Sprite *sprite);
void sprite_current_stage_display_tick(s8 *status, struct Sprite *sprite);
void sprite_current_stage_display_disp(struct Sprite *sprite);
void sprite_hud_stage_name_tick(s8 *status, struct Sprite *sprite);
void sprite_banana_icon_tick(s8 *status, struct Sprite *sprite);
void sprite_banana_icon_shadow_tick(s8 *status, struct Sprite *sprite);
void sprite_banana_count_tick(s8 *status, struct Sprite *sprite);
void sprite_monkey_counter_tick(s8 *status, struct Sprite *sprite);
void sprite_hud_player_num_disp(struct Sprite *sprite);

u8 lbl_1_data_4FEC0[0x170] = {
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

s8 g_banana_disp_efc_req_count;
s8 g_banana_disp_efc_stack[0xC];

static inline int is_timer_dimmed(void)
{
    int ret = 0;
    if (modeCtrl.gameType == 2 && (stageselect_is_storymode.flags & 2))
        ret = 1;
    return ret;
}

static inline int is_story_game(void)
{
    int ret = 0;
    if (modeCtrl.gameType == 0xE || (modeCtrl.gameType == 2 && stageselect_is_storymode.isStoryMode == 1))
        ret = 1;
    return ret;
}

static inline int is_extra_course(void)
{
    int ret = 0;
    if (((modeCtrl.courseFlags & 8) && !(modeCtrl.courseFlags & 0x10)) || (modeCtrl.courseFlags & 0x800000))
        ret = 1;
    return ret;
}

void create_hud_sprites(void)
{
    create_score_sprites(16.0f, 24.0f);
    create_timer_sprites(320.0f, 16.0f);
    create_speed_sprites(16.0f, 410.0f);
    create_hud_stage_name_sprites(16.0f, 432.0f);
    create_banana_counter_sprites(modeCtrl.gameType != 0 ? 0x220 : 0x203, 24.0f);
    create_monkey_counter_sprites(modeCtrl.gameType != 0 ? 0x248 : 0x22F, 83.0f);
    if (modeCtrl.gameType == 0 && (dip_switches & 1))
        create_debug_course_display_sprites();
    if (get_sprite_with_unique_id(0xE) == NULL && (mode_info.flags & INFO_FLAG_FINAL_FLOOR) && modeCtrl.gameType != 2)
        create_final_stage_sprite();
}

void create_score_sprites(float x, float y)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->bmpId = 0x504;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "game_kiwaku_score");
        sprite = create_linked_sprite(sprite);
        if (sprite != NULL)
        {
            sprite->y = 1.0f;
            sprite->fontId = 0x5D;
            sprite->unk58 = balls[modeCtrl.currPlayer].score;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x60;
            sprite->mainFunc = sprite_score_tick;
            sprite->drawFunc = sprite_score_disp;
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->bmpId = 0x507;
        sprite->depth = 0.09f;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "game_socre_asc");
    }
}

void create_timer_sprites(float x, float y)
{
    struct Sprite *sprite;
    int dim = is_timer_dimmed();

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->textAlign = 3;
        sprite->depth = 0.15f;
        sprite->bmpId = 0x50F;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        if (dim)
        {
            sprite->mulR = 0x80;
            sprite->mulG = 0x80;
            sprite->mulB = 0x80;
        }
        sprintf(sprite->text, "game_time_waku");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = 7.0f + y;
        sprite->textAlign = 3;
        sprite->depth = 0.12f;
        sprite->bmpId = 0x50C;
        sprite->userVar = -0x40;
        sprite->unk50 = 0x88;
        sprite->unk54 = 0x4000;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        if (dim)
        {
            sprite->mulR = 0x80;
            sprite->mulG = 0x80;
            sprite->mulB = 0x80;
        }
        sprite->mainFunc = sprite_timer_ball_tick;
        sprintf(sprite->text, "game_time_ball");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = x;
        sprite->y = 27.0f + y;
        sprite->fontId = 0x5E;
        sprite->userVar = 0x5F;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->drawFunc = fn_1_C9970;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->textAlign = 3;
        sprite->bmpId = 0x50B;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        if (dim)
        {
            sprite->mulR = 0x80;
            sprite->mulG = 0x80;
            sprite->mulB = 0x80;
        }
        sprintf(sprite->text, "game_time_asc");
    }
}

void create_speed_sprites(float x, float y)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 48.0f + x;
        sprite->y = y;
        sprite->bmpId = 0x509;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "game_speed");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = x;
        sprite->y = y;
        sprite->fontId = 0x60;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x60;
        sprite->mainFunc = sprite_speed_tick;
    }
}

void create_hud_stage_name_sprites(float x, float y)
{
    struct Sprite *sprite;
    int story;
    int extra;
    int id;
    s16 floor;
    float width;

    story = is_story_game();
    extra = is_extra_course();
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->tag = 0x70;
        sprite->x = x;
        sprite->y = y;
        sprite->bmpId = 0x505;
        sprite->depth = 0.12f;
        sprite->mainFunc = sprite_current_stage_display_tick;
        sprite->drawFunc = sprite_current_stage_display_disp;
        sprite->counter = 0x3C;
        sprite->unk58 = 0.0f;
        sprite->unk5C = 0.0f;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "game_kiwaku_world");
    }
    floor = modeCtrl.gameType == 2 ? g_stage_id_in_practice_mode[0].floor : mode_info.currFloor;
    id = floor;
    width = 34.0f;
    if (modeCtrl.difficulty == 0)
        width += 4.0f;
    if (extra)
    {
        width += 8.0f;
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 3.0f + ((x + width) - 8.0f);
            sprite->y = 3.0f + (8.0f + y);
            sprite->fontId = 3;
            sprite->depth = 0.11f;
            sprite->unk58 = 0.45f;
            sprite->unk5C = 0.0f;
            sprite->opacity = 0.0f;
            sprite->mulR = 0;
            sprite->mulG = 0;
            sprite->mulB = 0;
            sprite->scaleX = 0.5f;
            sprite->scaleY = 0.7f;
            sprite->flags |= 0x21000000;
            sprite->unk20 = 0.45f;
            sprite->unk24 = 0;
            sprite->widescreenArg = 0x60;
            sprintf(sprite->text, "EX");
            sprite->mainFunc = sprite_hud_stage_name_tick;
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 0x22;
            sprite->x = (x + width) - 8.0f;
            sprite->y = 8.0f + y;
            sprite->fontId = 3;
            sprite->unk58 = 1.0f;
            sprite->unk5C = 0.0f;
            sprite->opacity = 0.0f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0;
            sprite->scaleX = 0.5f;
            sprite->scaleY = 0.7f;
            sprite->flags |= 0x21000000;
            sprite->unk20 = 1.0f;
            sprite->unk24 = 0;
            sprite->widescreenArg = 0x60;
            sprintf(sprite->text, "EX");
            sprite->mainFunc = sprite_hud_stage_name_tick;
        }
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        width += 5.0f + 0.5f * textdraw_get_pixel_width_of_string_as_f32(sprite->text);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 3.0f + (x + width);
        sprite->y = 3.0f + (8.0f + y);
        sprite->fontId = 0x60;
        sprite->depth = 0.11f;
        sprite->unk58 = 0.5f;
        sprite->unk5C = 0.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x60;
        if (modeCtrl.gameType == 0xE || (modeCtrl.gameType == 2 && stageselect_is_storymode.isStoryMode == 1))
            sprintf(sprite->text, "%d:%d", mode_info.storyWorld + 1, mode_info.storyStage + 1);
        else
            sprintf(sprite->text, "%d", id);
        sprite->mainFunc = sprite_hud_stage_name_tick;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x23;
        sprite->x = x + width;
        sprite->y = 8.0f + y;
        sprite->fontId = 0x60;
        sprite->unk58 = 1.0f;
        sprite->unk5C = 0.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x60;
        if (modeCtrl.gameType == 0xE || (modeCtrl.gameType == 2 && stageselect_is_storymode.isStoryMode == 1))
            sprintf(sprite->text, "%d:%d", mode_info.storyWorld + 1, mode_info.storyStage + 1);
        else
            sprintf(sprite->text, "%d", id);
        sprite->mainFunc = sprite_hud_stage_name_tick;
        textdraw_reset();
        textdraw_set_font(0x60);
        width += 5.0f + textdraw_get_pixel_width_of_string_as_f32(sprite->text);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 3.0f + (x + width);
        sprite->y = 3.0f + (8.0f + y);
        sprite->fontId = 3;
        sprite->depth = 0.11f;
        sprite->unk58 = 0.45f;
        sprite->unk5C = 0.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->scaleX = 0.7f;
        sprite->scaleY = 0.7f;
        sprite->unk20 = 0.45f;
        sprite->unk24 = 0;
        sprite->flags |= 0xA1000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "%s", current_stage_name);
        sprite->mainFunc = sprite_hud_stage_name_tick;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = x + width;
        sprite->y = 8.0f + y;
        sprite->fontId = 3;
        sprite->unk58 = 1.0f;
        sprite->unk5C = 0.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->scaleX = 0.7f;
        sprite->scaleY = 0.7f;
        sprite->unk20 = 1.0f;
        sprite->unk24 = 0;
        sprite->flags |= 0xA1000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "%s", current_stage_name);
        sprite->mainFunc = sprite_hud_stage_name_tick;
    }
}

void create_banana_counter_sprites(float x, float y)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->bmpId = 0x502;
        sprite->depth = 0.11f;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x220;
        sprintf(sprite->text, "game_kiwaku_banana");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x15;
        sprite->type = 1;
        sprite->x = x - 2.0f;
        sprite->y = y - 2.0f;
        sprite->bmpId = 0x501;
        sprite->depth = 0.09f;
        sprite->counter = 0;
        sprite->mainFunc = sprite_banana_icon_tick;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x220;
        sprintf(sprite->text, "game_banana");
        g_banana_disp_efc_req_count = 0;
        sprite = create_linked_sprite(sprite);
        if (sprite != NULL)
        {
            sprite->type = 1;
            sprite->x = -22.0f;
            sprite->y = 2.0f;
            sprite->bmpId = 0x501;
            sprite->mulR = 0;
            sprite->mulG = 0;
            sprite->mulB = 0;
            sprite->opacity = 0.5f;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x220;
            sprite->mainFunc = sprite_banana_icon_shadow_tick;
            sprintf(sprite->text, "game_banana kage");
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 23.0f + x;
        sprite->y = 1.0f + y;
        sprite->fontId = 0x5D;
        sprite->depth = 0.09f;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x220;
        sprite->mainFunc = sprite_banana_count_tick;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 2.0f + (23.0f + x);
        sprite->y = 2.0f + (1.0f + y);
        sprite->fontId = 0x5D;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->opacity = 0.5f;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x220;
        sprite->mainFunc = sprite_banana_count_tick;
    }
}

void create_monkey_counter_sprites(float x, float y)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->bmpId = 0x503;
        sprite->depth = 0.11f;
        sprite->textAlign = 4;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x220;
        sprintf(sprite->text, "game_kiwaku_saru");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x + lbl_1_bss_6FB6C;
        sprite->y = (y - 2.0f) + lbl_1_bss_6FB68;
        sprite->depth = 0.08f;
        sprite->drawFunc = sprite_monkey_counter_icon_disp;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x220;
        sprintf(sprite->text, "saru");
    }
    lbl_1_bss_A873C.unk0 = lbl_1_bss_A873C.unk4 = 0;
    lbl_1_bss_A873C.unk8 = 0;
    lbl_1_bss_A873C.unk14 = 0x33;
    if (modeCtrl.gameType == 0)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 2.0f + (25.0f + x);
            sprite->y = 2.0f + (y - 17.0f);
            sprite->fontId = 0x61;
            sprite->textAlign = 4;
            sprite->opacity = 0.5f;
            sprite->mulR = 0;
            sprite->mulG = 0;
            sprite->mulB = 0;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x220;
            strcpy(sprite->text, ":");
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 25.0f + x;
            sprite->y = y - 17.0f;
            sprite->fontId = 0x61;
            sprite->textAlign = 4;
            sprite->depth = 0.09f;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x220;
            strcpy(sprite->text, ":");
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->type = 1;
            sprite->x = 48.0f + x;
            sprite->y = y - 24.0f;
            sprite->bmpId = 0x503;
            sprite->depth = 0.11f;
            sprite->textAlign = 4;
            sprite->scaleX = 0.71428f;
            sprite->scaleY = 0.71428f;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x220;
            sprintf(sprite->text, "game_kiwaku_saru mini");
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 2.0f + (48.0f + x);
            sprite->y = 2.0f + (y - 24.0f);
            sprite->fontId = 0x61;
            sprite->textAlign = 4;
            sprite->opacity = 0.5f;
            sprite->mulR = 0;
            sprite->mulG = 0;
            sprite->mulB = 0;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x220;
            sprite->userVar = balls[modeCtrl.currPlayer].unk2;
            sprite->mainFunc = sprite_monkey_counter_tick;
            sprintf(sprite->text, "%d", balls[modeCtrl.currPlayer].unk2);
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 48.0f + x;
            sprite->y = y - 24.0f;
            sprite->fontId = 0x61;
            sprite->textAlign = 4;
            sprite->depth = 0.09f;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x220;
            sprite->userVar = balls[modeCtrl.currPlayer].unk2;
            sprite->mainFunc = sprite_monkey_counter_tick;
            sprintf(sprite->text, "%d", balls[modeCtrl.currPlayer].unk2);
        }
        if (modeCtrl.playerCount > 1)
        {
            sprite = create_sprite();
            if (sprite != NULL)
            {
                sprite->type = 1;
                sprite->x = 610.0f;
                sprite->y = 96.0f;
                sprite->textAlign = 4;
                sprite->userVar = modeCtrl.currPlayer;
                sprite->drawFunc = sprite_hud_player_num_disp;
                sprite->flags |= 0x01000000;
                sprite->widescreenArg = 0x220;
                sprintf(sprite->text, "%dP", modeCtrl.currPlayer + 1);
            }
        }
    }
}

void create_final_stage_sprite(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0xE;
        sprite->x = 320.0f;
        sprite->y = 300.0f;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->scaleX = 0.5f;
        sprite->scaleY = 0.5f;
        sprite->counter = 0x96;
        sprite->flags |= 0x1000;
        sprite->mainFunc = sprite_final_stage_tick;
        sprite->drawFunc = sprite_final_stage_disp;
        strcpy(sprite->text, "FINAL STAGE");
    }
}

void create_debug_course_display_sprites(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 100.0f;
        sprite->y = 440.0f;
        sprite->fontId = 0;
        sprite->textAlign = 2;
        sprite->scaleX = 1.5f;
        sprite->scaleY = 1.5f;
        sprite->drawFunc = sprite_debug_course_display_disp;
        sprintf(sprite->text, "course");
    }
}

void sprite_score_tick(s8 *status, struct Sprite *sprite)
{
    if (balls[modeCtrl.currPlayer].score > sprite->unk58)
    {
        sprite->unk58 += 0.1f * ((balls[modeCtrl.currPlayer].score + 1) - sprite->unk58);
        sprite->unk58 = sprite->unk58 < balls[modeCtrl.currPlayer].score ? sprite->unk58 : balls[modeCtrl.currPlayer].score;
    }
    sprintf(sprite->text, "%07d", (u32)sprite->unk58);
}

void sprite_score_disp(struct Sprite *sprite)
{
    struct TPLTextureHeader *tex =
        &((struct BitmapGroup *)bmp_infos)[sprite->prev->bmpId >> 8].tpl->texHeaders[sprite->prev->bmpId & 0xFF];

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_spacing(1.0f, 0.0f);
    textdraw_set_alpha(0.5f);
    textdraw_set_mul_color(0);
    textdraw_set_depth(0.01f + sprite->depth);
    textdraw_set_pos(2.0f + (sprite->prev->x + tex->width), 2.0f + (sprite->prev->y + sprite->y));
    textdraw_printf(sprite->text);
    textdraw_set_alpha(1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_depth(sprite->depth);
    textdraw_set_pos(sprite->prev->x + tex->width, sprite->prev->y + sprite->y);
    textdraw_printf(sprite->text);
    textdraw_reset();
}

void fn_1_C9970(struct Sprite *sprite)
{
    int dim = is_timer_dimmed();

    textdraw_reset();
    textdraw_set_spacing(1.0f, 0.0f);
    textdraw_set_depth(sprite->depth);
    textdraw_set_alignment(3);
    textdraw_set_mul_color(dim ? 0x808080 : 0xFFFFFF);
    textdraw_set_font(sprite->fontId);
    textdraw_set_pos(sprite->x, sprite->y);
    textdraw_printf("%03d\n", mode_info.timerCurr / 60);
    textdraw_set_font(sprite->userVar);
    textdraw_set_pos(sprite->x - 5.0f, 40.0f + sprite->y);
    textdraw_printf(":%02d\n", (int)(100.0f * ((mode_info.timerCurr % 60) / 60.0f)));
    textdraw_reset();
}

void sprite_timer_ball_tick(s8 *status, struct Sprite *sprite)
{
    int dim = is_timer_dimmed();

    if (mode_info.timerCurr == 0 || (mode_info.flags & INFO_FLAG_05))
        sprite->userVar += 0x20;
    else if (mode_info.timerCurr < 600)
        sprite->userVar -= 4;
    sprite->userVar = sprite->userVar < 0 ? sprite->userVar : 0;
    if (mode_info.timerCurr == 0 || (mode_info.flags & INFO_FLAG_05) || !(mode_info.flags & INFO_FLAG_TIMER_PAUSED))
        sprite->rotation += sprite->userVar;
    if (mode_info.timerCurr == 0 && !(mode_info.flags & INFO_FLAG_TIMER_PAUSED))
    {
        sprite->mulR = sprite->mulR + 0.02f * (0x40 - sprite->mulR);
        sprite->mulG = sprite->mulG + 0.02f * -sprite->mulG;
        sprite->mulB = sprite->mulG;
    }
    else if (mode_info.timerCurr < 600 && !(mode_info.flags & INFO_FLAG_TIMER_PAUSED))
    {
        sprite->unk54 += sprite->unk54 > 0x4000 ? sprite->unk50 * 6 : sprite->unk50;
        sprite->unk54 = sprite->unk54 < 0 ? 0 : (sprite->unk54 > 0x8000 ? 0x8000 : sprite->unk54);
        {
            float t = 1.0f - math_sin(sprite->unk54);

            sprite->mulG = (255.0f - 192.0f * t) * (dim ? 0.5f : 1.0f);
        }
        sprite->mulB = sprite->mulG;
        if (sprite->unk54 == 0x8000)
        {
            sprite->unk54 = 0;
            sprite->unk50 += 0x44;
        }
    }
}

void sprite_speed_tick(s8 *status, struct Sprite *sprite)
{
    struct Ball *ball = &balls[modeCtrl.currPlayer];

    if (locale == 0)
        sprintf(sprite->text, "%3.0f", 134.21985f * mathutil_vec_len(&ball->vel) < 999.0f ? 134.21985f * mathutil_vec_len(&ball->vel) : 999.0f);
    else
        sprintf(sprite->text, "%3.0f", 216.0f * mathutil_vec_len(&ball->vel) < 999.0f ? 216.0f * mathutil_vec_len(&ball->vel) : 999.0f);
}

void sprite_current_stage_display_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter > 0)
        sprite->counter--;
    sprite->unk58 = -7.0f * ((sprite->counter / 24 + 1) * (sprite->counter / 24 + 1)) * math_sin((sprite->counter % 24) * 0x555);
    sprite->unk5C = 1.0f - 0.45f * (sprite->counter / 24.0f);
}

void sprite_current_stage_display_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    struct SpriteDrawRequest req2;
    int story;
    struct Sprite *other;
    int i;
    int id;
    float width;
    float scale;

    story = is_story_game();
    scale = 0.0f;
    textdraw_reset();
    if (modeCtrl.difficulty == 0)
        scale += 4.0f;
    other = get_sprite_with_unique_id(0x22);
    if (other != NULL)
    {
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        scale += 5.0f + (8.0f + 0.5f * textdraw_get_pixel_width_of_string_as_f32(other->text));
    }
    other = get_sprite_with_unique_id(0x23);
    if (other != NULL)
    {
        textdraw_set_font(0x60);
        scale += 5.0f + textdraw_get_pixel_width_of_string_as_f32(other->text);
    }
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    scale += 5.0f + 0.7f * textdraw_get_pixel_width_of_string_as_f32(current_stage_name);
    width = scale - 40.0f;
    scale = 0.05f * width;
    id = modeCtrl.gameType == 2 ? g_stage_id_in_practice_mode[0].floor : mode_info.currFloor;
    {
        float us[3] = {0.0f, 0.5f, 0.75f};
        float vs[3] = {0.5f, 0.75f, 1.0f};

        req.bmpId = sprite->bmpId;
        req.y = sprite->y;
        req.z = sprite->depth;
        req.v1 = 0.0f;
        req.v2 = 1.0f;
        req.scaleY = 1.0f;
        req.opacity = 1.0f;
        req.rotation = 0;
        req.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | 0xFFFFFF;
        req.addColor = 0;
        req.unk30 = -1;
        for (i = 0; i < 3; i++)
        {
            req.u1 = us[i];
            req.u2 = vs[i];
            switch (i)
            {
            case 0:
                req.flags = (sprite->flags & ~0xF) | 5;
                req.scaleX = req.u2 - req.u1;
                req.x = sprite->x + sprite->unk58;
                break;
            case 1:
                {
                    float t = -0.05f * sprite->unk58 * (1.0 / (1.0 + scale));

                    req.flags = (sprite->flags & ~0xF) | 7;
                    req.scaleX = (1.0 + 0.1 * lbl_1_bss_6FB84) * ((1.0 + scale) * ((1.0f + t) * (req.u2 - req.u1)));
                    req.x = lbl_1_bss_6FB80 + (width + (sprite->x + bmp_infos[req.bmpId >> 8].tpl->texHeaders[req.bmpId & 0xFF].width / 4 * 3));
                }
                break;
            case 2:
                req.flags = (sprite->flags & ~0xF) | 5;
                req.scaleX = req.u2 - req.u1;
                req.x = lbl_1_bss_6FB80 + (width + (sprite->x + bmp_infos[req.bmpId >> 8].tpl->texHeaders[req.bmpId & 0xFF].width / 4 * 3));
                break;
            }
            draw_sprite_draw_request(&req);
        }
    }
    req2.u1 = 0.0f;
    req2.v1 = 0.0f;
    req2.u2 = 1.0f;
    req2.v2 = 1.0f;
    req2.scaleX = sprite->unk5C;
    req2.scaleY = sprite->unk5C;
    req2.rotation = 0;
    req2.addColor = 0;
    req2.unk30 = -1;
    req2.flags = (sprite->flags & ~0xF) | 0xA;
    if (story)
        req2.bmpId = 0x512;
    else
    {
        int ids[4] = {0x514, 0x513, 0x515, 0x517};

        req2.bmpId = ids[(modeCtrl.courseFlags & 0x10) ? 3 : modeCtrl.difficulty];
    }
    req2.x = 2.0f + (16.0f + sprite->x + sprite->unk58);
    req2.y = 2.0f + (16.0f + sprite->y);
    req2.z = sprite->depth - 0.01f;
    req2.opacity = 0.5f;
    req2.mulColor = (u8)(255.0f * sprite->opacity) << 24;
    draw_sprite_draw_request(&req2);
    req2.x = 16.0f + sprite->x + sprite->unk58;
    req2.y = 16.0f + sprite->y;
    req2.z = sprite->depth - 0.02f;
    req2.opacity = 1.0f;
    req2.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | 0xFFFFFF;
    draw_sprite_draw_request(&req2);
}

void sprite_hud_stage_name_tick(s8 *status, struct Sprite *sprite)
{
    struct Sprite *owner = get_sprite_with_unique_id(0x70);

    if (owner != NULL && owner->counter == 0 && sprite->unk5C < 1.0f)
        sprite->unk5C += 0.05f;
    sprite->opacity = sprite->unk58 * sprite->unk5C;
}

void sprite_banana_icon_tick(s8 *status, struct Sprite *sprite)
{
    static const struct
    {
        s8 duration;
        u16 rate;
    } efcTable[] = {{0x14, 0x0CCC}, {0x3C, 0x0CCC}};
    static struct Keyframe curveR[7] = {
        {0, 1.0f, 0.0f, 0.0f}, {1, 1.0f, 0.0f, 0.0f}, {2, 1.0f, 0.0f, 0.0f}, {3, 0.0f, 0.0f, 0.0f},
        {4, 0.0f, 0.0f, 0.0f}, {5, 1.0f, 0.0f, 0.0f}, {6, 1.0f, 0.0f, 0.0f},
    };
    static struct Keyframe curveG[7] = {
        {0, 0.0f, 0.0f, 0.0f}, {1, 0.5f, 0.0f, 0.0f}, {2, 1.0f, 0.0f, 0.0f}, {3, 1.0f, 0.0f, 0.0f},
        {4, 0.0f, 0.0f, 0.0f}, {5, 0.0f, 0.0f, 0.0f}, {6, 0.0f, 0.0f, 0.0f},
    };
    static struct Keyframe curveB[7] = {
        {0, 0.0f, 0.0f, 0.0f}, {1, 0.0f, 0.0f, 0.0f}, {2, 0.0f, 0.0f, 0.0f}, {3, 0.0f, 0.0f, 0.0f},
        {4, 1.0f, 0.0f, 0.0f}, {5, 1.0f, 0.0f, 0.0f}, {6, 1.0f, 0.0f, 0.0f},
    };

    if (sprite->counter == 0)
    {
        if (g_banana_disp_efc_req_count > 0)
            sprite->counter = efcTable[g_banana_disp_efc_stack[g_banana_disp_efc_req_count - 1]].duration + 1;
    }
    else
    {
        sprite->counter--;
        if (sprite->counter == 0)
        {
            g_banana_disp_efc_req_count--;
            sprite->rotation = 0;
            sprite->addR = 0;
            sprite->addG = 0;
            sprite->addB = 0;
        }
        else
        {
            sprite->rotation = 8192.0f * math_sin(sprite->counter * efcTable[g_banana_disp_efc_stack[g_banana_disp_efc_req_count - 1]].rate);
            if (g_banana_disp_efc_stack[g_banana_disp_efc_req_count - 1] == 1)
            {
                float t = 0.5 * (float)(sprite->counter % 12);

                sprite->addR = 128.0f * fn_1_D2550(t, curveR);
                sprite->addG = 128.0f * fn_1_D2550(t, curveG);
                sprite->addB = 128.0f * fn_1_D2550(t, curveB);
            }
        }
    }
}

void sprite_banana_icon_shadow_tick(s8 *status, struct Sprite *sprite)
{
    sprite->rotation = sprite->prev->rotation;
}

void sprite_banana_count_tick(s8 *status, struct Sprite *sprite)
{
    sprintf(sprite->text, "%03d", balls[modeCtrl.currPlayer].bananas);
}

void g_banana_disp_efc_req(s8 type)
{
    if (get_sprite_with_unique_id(0x15) != NULL)
    {
        if (g_banana_disp_efc_req_count == 8)
            printf("### ERROR ### BananaDispEfcReq stack over.\n");
        else
        {
            g_banana_disp_efc_stack[g_banana_disp_efc_req_count] = type == 1 ? 0 : 1;
            g_banana_disp_efc_req_count++;
        }
    }
}

void sprite_monkey_counter_tick(s8 *status, struct Sprite *sprite)
{
    int lives = balls[modeCtrl.currPlayer].unk2;

    lives = lives - 1 > 0 ? lives - 1 : 0;
    sprite->scaleX = lives >= 10 ? 0.6f : 1.0f;
    sprintf(sprite->text, "%d", lives);
}

void sprite_hud_player_num_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    s8 rows[4] = {2, 1, 3, 0};
    int row = rows[balls[sprite->userVar].playerId];
    struct TPLTextureHeader *tex;

    req.bmpId = 0x516;
    req.rotation = sprite->rotation;
    req.opacity = sprite->opacity;
    req.unk30 = -1;
    req.flags = (sprite->flags & ~0xF) | 0xA;
    req.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | (sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB;
    req.addColor = (sprite->addR << 16) | (sprite->addG << 8) | sprite->addB;
    tex = &bmp_infos[(req.bmpId & 0xFF00) >> 8].tpl->texHeaders[req.bmpId & 0xFF];
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = sprite->depth;
    req.u1 = 28.0f * sprite->userVar / tex->width;
    req.v1 = 16.0f * row / tex->height;
    req.u2 = req.u1 + 28.0f / tex->width;
    req.v2 = req.v1 + 16.0f / tex->height;
    req.scaleX = sprite->scaleX * (req.u2 - req.u1);
    req.scaleY = sprite->scaleY * (req.v2 - req.v1);
    draw_sprite_draw_request(&req);
}
