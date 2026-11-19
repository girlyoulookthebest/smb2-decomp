/* sel_ngc_sprite.c: main menu sprites: creation, tick (mainFunc) and draw (drawFunc)
 * callbacks for the title/info strings, the menu entry lists, the menu title, the game
 * settings box, the practice/fight stage selects and the button prompts. */
/* sel_ngc.h declares sel_ngc.c's menu work area and the sel.c menu settings globals as plain
 * bytes. This file accesses them through their real types (the "core" section of sel_ngc.h,
 * as sel_ngc.c does), so the byte declarations are renamed out of the way while the headers
 * are read and the objects are re-declared with their struct types below. */
#define g_currently_visible_menu_screen g_currently_visible_menu_screen_as_u8
#define g_focused_root_menu g_focused_root_menu_as_u8
#define stageselect_is_storymode stageselect_is_storymode_as_u8
#define lbl_1_bss_16DC lbl_1_bss_16DC_as_u8
#include "sel_ngc.h"
#include "sel_ngc_data.h"
#include "macros.h"
#undef g_currently_visible_menu_screen
#undef g_focused_root_menu
#undef stageselect_is_storymode
#undef lbl_1_bss_16DC

extern struct SelNgcMenuWork g_currently_visible_menu_screen; /* sel_ngc.c bss 0x00 */
extern struct SelNgcRootMenu g_focused_root_menu;             /* sel.c struct SelMenuState */
extern struct SelNgcStorySettings stageselect_is_storymode;   /* sel.c struct SelStorySettings */
extern struct SelNgcSettings16DC lbl_1_bss_16DC;              /* sel.c struct SelSettings9 */

/* main_loop/unlock.c (sel_ngc.h still declares these without parameters) */
int g_check_if_partygame_unlocked(int id);
int get_num_of_unlocked_party_games(void);
int fn_1_16CEE4(void);
int fn_1_16CFFC(void);

#define menuWork (&g_currently_visible_menu_screen)
#define menuCursor (&lbl_3_bss_178)
#define rootMenu (&g_focused_root_menu)
#define storySettings (&stageselect_is_storymode)
#define fightSettings (&lbl_1_bss_16DC)

/* Sprite callbacks of this file */
void sprite_title_str_tick(s8 *status, struct Sprite *sprite);
void sprite_title_str_disp(struct Sprite *sprite);
void sprite_info_str_tick(s8 *status, struct Sprite *sprite);
void sprite_info_str_disp(struct Sprite *sprite);
void sprite_start_mask_tick(s8 *status, struct Sprite *sprite);
void sprite_menu_basic_tick(s8 *status, struct Sprite *sprite);
void sprite_menu_basic_disp(struct Sprite *sprite);
void sprite_menu_basic_stack_tick(s8 *status, struct Sprite *sprite);
void sprite_menu_basic_title_tick(s8 *status, struct Sprite *sprite);
void sprite_menu_basic_title_disp(struct Sprite *sprite);
void sprite_game_settings_tick(s8 *status, struct Sprite *sprite);
void sprite_game_settings_disp(struct Sprite *sprite);
void sprite_practice_stage_select_tick(s8 *status, struct Sprite *sprite);
void sprite_practice_stage_select_disp(struct Sprite *sprite);
void sprite_fight_stage_select_tick(s8 *status, struct Sprite *sprite);
void sprite_fight_stage_select_disp(struct Sprite *sprite);
void fn_3_1A9F0(s8 *status, struct Sprite *sprite);
void fn_3_1ABE0(struct Sprite *sprite);
void sprite_button_tick(s8 *status, struct Sprite *sprite);
void sprite_button_disp(struct Sprite *sprite);
/* other helpers of this file */
struct Sprite *fn_3_17E5C(struct Sprite *sprite);
int fn_3_1B984(int mode);

/* ---- .bss ---- */
s32 lbl_3_bss_190;
s32 lbl_3_bss_194;

/* ---- .data ---- */
/* Unreferenced tables at the start of the TU's .data (no relocations point at them; the
 * same tables also start sel_ngc_preview.c's .data). */
struct SelNgcUnusedEntry
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ s16 unk4;
    /*0x06*/ s16 unk6;
}; /* size = 0x8 */

s8 lbl_3_data_C7C0[19] = { -2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, -2, 13, 14, 15, -1 };
s8 lbl_3_data_C7D4[17] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1 };
struct SelNgcUnusedEntry lbl_3_data_C7E8[41] = {
    { 0x31, 0x00, 0x00, 0x00, 0x00, 0 },
    { 0x38, 0x00, 0xB4, 0x00, 0x01, 0 },
    { 0x01, 0x00, 0x00, 0x00, 0x02, 0 },
    { 0x10, 0x00, 0x4C, 0x00, 0x03, 0 },
    { 0x01, 0x00, 0x00, 0x00, 0x04, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x09, 0 },
    { 0x40, 0x00, 0x00, 0x00, 0x00, 0 },
    { 0x01, 0x00, 0x00, 0x00, 0x0B, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x0C, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x05, 0 },
    { 0x40, 0x00, 0x00, 0x00, 0x00, 0 },
    { 0x01, 0x00, 0x00, 0x00, 0x07, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x08, 0 },
    { 0x38, 0x00, 0xB4, 0x00, 0x0D, 0 },
    { 0x40, 0x00, 0x00, 0x00, 0x00, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x14, 0 },
    { 0x40, 0x00, 0x00, 0x00, 0x00, 0 },
    { 0x01, 0x00, 0x00, 0x00, 0x16, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x17, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x18, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x0F, 0 },
    { 0x40, 0x00, 0x00, 0x00, 0x00, 0 },
    { 0x01, 0x00, 0x00, 0x00, 0x11, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x12, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x13, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x00, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x01, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x02, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x03, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x04, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x05, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x06, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x07, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x08, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x09, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x0A, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x0B, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x0C, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x0D, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x0E, 0 },
    { 0x18, 0x00, 0x00, 0x00, 0x0F, 0 },
};

/* Text style of the description line under the menu entries */
void fn_3_140CC(int counter)
{
    textdraw_set_font(3);
    textdraw_set_mul_color(0xFFFF00);
    textdraw_set_alignment(4);
    textdraw_set_spacing(0.0f, 8.0f);
    textdraw_set_drop_shadow();
    textdraw_set_flags(0x80000000);
    textdraw_set_scale(0.8f, 1.0f);
    g_textdraw_set_counter(counter);
}

void create_main_menu_sprites(void)
{
    struct Sprite *sprite;
    int i;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x1C;
        sprite->type = SPRITE_TYPE_BITMAP;
        sprite->x = 320.0f;
        sprite->y = 50.0f;
        sprite->textAlign = 4;
        sprite->depth = 0.2f;
        sprite->bmpId = 0x1702;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprintf(sprite->text, "sel2_kiwaku_title");
    }

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 9;
        sprite->x = 320.0f;
        sprite->y = 49.0f;
        sprite->fontId = 3;
        sprite->textAlign = 4;
        sprite->userVar = 0;
        sprite->counter = 0;
        sprite->unk58 = 0.0f;
        sprite->unk5C = 0.0f;
        sprite->mainFunc = sprite_title_str_tick;
        sprite->drawFunc = sprite_title_str_disp;
        sprintf(sprite->text, "title_str");
    }

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x1D;
        sprite->type = SPRITE_TYPE_BITMAP;
        sprite->x = 320.0f;
        sprite->y = 400.0f;
        sprite->textAlign = 4;
        sprite->depth = 0.2f;
        sprite->bmpId = 0x1701;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprintf(sprite->text, "sel2_kiwaku_info");
    }

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0xC;
        sprite->x = 320.0f;
        sprite->y = 399.0f;
        sprite->fontId = 3;
        sprite->textAlign = 4;
        sprite->depth = 0.15f;
        sprite->userVar = 0;
        sprite->counter = 0;
        sprite->unk58 = 0.0f;
        sprite->mainFunc = sprite_info_str_tick;
        sprite->drawFunc = sprite_info_str_disp;
        sprintf(sprite->text, "info_str");
    }

    {
        /* the three "start mask" pictures: the first three of these, or three random ones */
        s32 maskBmpIds[7] = { 0x1700, 0x0F, 0x59, 0x20, 0x33, 0x03, 0x0C };
        float maskScaleX[7] = { 1.6f, 1.0f, 0.7f, 1.0f, 1.0f, 1.0f, 0.7f };
        float maskScaleY[7] = { 1.6f, 0.6f, 0.7f, 0.6f, 0.6f, 0.6f, 0.7f };
        float maskX[3] = { 160.0f, 320.0f, 480.0f };
        s8 mask;

        for (i = 0; i < 3; i++)
        {
            if (sel_menu_info.unkC & 1)
                mask = i;
            else
                mask = (rand() & 0x7FFF) % 6;
            sprite = create_sprite();
            if (sprite != NULL)
            {
                sprite->type = SPRITE_TYPE_BITMAP;
                sprite->tag = i + 5;
                sprite->x = maskX[i];
                sprite->y = 240.0f;
                sprite->textAlign = 4;
                sprite->bmpId = maskBmpIds[mask];
                sprite->depth = 0.0f;
                sprite->opacity = 0.02f;
                sprite->scaleX = maskScaleX[mask];
                sprite->scaleY = maskScaleY[mask];
                if (modeCtrl.wideMode)
                {
                    sprite->scaleX *= 0.99996597f;
                    sprite->scaleY *= 1.333288f;
                }
                sprite->flags |= 0x40000;
                sprite->userVar = 0;
                sprite->mainFunc = sprite_start_mask_tick;
                sprintf(sprite->text, "start mask %d", i);
            }
        }
    }

    for (i = 0; i < 12; i++)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = i + 0x3C;
            sprite->fontId = 3;
            sprite->textAlign = 4;
            sprite->scaleX = 0.0f;
            sprite->scaleY = 0.0f;
            sprite->userVar = 0;
            sprite->counter = 0;
            sprite->unk50 = 0;
            sprite->unk54 = 0;
            sprite->unk58 = 0.0f;
            sprite->unk5C = 0.0f;
            sprite->mainFunc = sprite_menu_basic_tick;
            sprite->drawFunc = sprite_menu_basic_disp;
            sprintf(sprite->text, "menu_basic %d", i);
        }
    }

    lbl_3_bss_190 = 0;
    for (i = 0; i < 5; i++)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = i + 0x49;
            sprite->fontId = 3;
            sprite->textAlign = 4;
            sprite->userVar = 0;
            sprite->counter = 0;
            sprite->unk50 = 0;
            sprite->unk54 = 0;
            sprite->unk58 = 0.0f;
            sprite->unk5C = 0.0f;
            sprite->unk60 = 0.0f;
            sprite->shadowAlpha = 0.0f;
            sprite->mainFunc = sprite_menu_basic_stack_tick;
            sprite->drawFunc = sprite_menu_basic_disp;
            sprintf(sprite->text, "menu_basic_stack %d", i);
        }
    }

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x48;
        sprite->fontId = 3;
        sprite->x = 320.0f;
        sprite->y = 326.0f;
        sprite->textAlign = 4;
        sprite->scaleX = 0.0f;
        sprite->scaleY = 0.0f;
        sprite->opacity = 0.0f;
        sprite->depth = 0.1f;
        sprite->userVar = 0;
        sprite->counter = 0;
        sprite->mainFunc = sprite_menu_basic_title_tick;
        sprite->drawFunc = sprite_menu_basic_title_disp;
        sprintf(sprite->text, "menu_basic_title");
    }

    /* button prompts (see sprite_button_tick/sprite_button_disp) */
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->fontId = 3;
        sprite->x = 540.0f;
        sprite->y = 384.0f;
        sprite->textAlign = 4;
        sprite->depth = 0.1f;
        sprite->mainFunc = sprite_button_tick;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->userVar = 0;
        sprite->unk50 = 0;
        sprite->unk54 = 0;
        sprite->opacity = 0.0f;
        sprite->drawFunc = sprite_button_disp;
        sprintf(sprite->text, "button");
    }
}

/* Text of the character select sprite (see fn_3_1BBA0) */
char lbl_3_data_C9C0[] = "chara_sel";

void g_display_game_settings_sprite(void)
{
    struct Sprite *sprite;

    if (get_sprite_with_unique_id(0xD) != NULL)
        destroy_sprite_with_unique_id(0xD);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0xD;
        sprite->type = SPRITE_TYPE_BITMAP;
        sprite->x = 320.0f;
        sprite->y = 235.0f;
        sprite->textAlign = 4;
        sprite->depth = 0.1f;
        sprite->bmpId = 0x1701;
        sprite->userVar = 1;
        sprite->counter = 8;
        sprite->unk50 = 0x20;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_game_settings_tick;
        sprite->drawFunc = sprite_game_settings_disp;
        sprintf(sprite->text, "game_setting_waku");
    }
}

void create_practice_mode_stage_select_sprite(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0x4F;
        sprite->type = SPRITE_TYPE_BITMAP;
        sprite->x = 320.0f;
        sprite->y = 235.0f;
        sprite->textAlign = 4;
        sprite->depth = 0.1f;
        sprite->bmpId = 0x1701;
        sprite->userVar = 1;
        sprite->counter = 8;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_practice_stage_select_tick;
        sprite->drawFunc = sprite_practice_stage_select_disp;
        sprintf(sprite->text, "prac_stage");
    }
}

void fn_3_14998(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0x4E;
        sprite->type = SPRITE_TYPE_BITMAP;
        sprite->x = 320.0f;
        sprite->y = 235.0f;
        sprite->textAlign = 4;
        sprite->depth = 0.1f;
        sprite->bmpId = 0x1701;
        sprite->userVar = 1;
        sprite->counter = 8;
        sprite->unk58 = 0.0f;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_fight_stage_select_tick;
        sprite->drawFunc = sprite_fight_stage_select_disp;
        sprintf(sprite->text, "fight_stage");
    }
}

void fn_3_14A60(void)
{
    struct Sprite *sprite;

    if (get_sprite_with_unique_id(0x33) != NULL)
        destroy_sprite_with_unique_id(0x33);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x33;
        sprite->type = SPRITE_TYPE_BITMAP;
        sprite->x = 320.0f;
        sprite->y = 235.0f;
        sprite->textAlign = 4;
        sprite->depth = 0.1f;
        sprite->bmpId = 0x1701;
        sprite->userVar = 1;
        sprite->counter = 8;
        sprite->unk58 = 0.0f;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = fn_3_1A9F0;
        sprite->drawFunc = fn_3_1ABE0;
        sprintf(sprite->text, "boat_course");
    }
}

/* Show the title/info frames */
void fn_3_14B40(void)
{
    struct Sprite *sprite;

    lbl_3_bss_194 = 0;
    sprite = get_sprite_with_unique_id(0x1C);
    if (sprite != NULL)
        sprite->opacity = 1.0f;
    sprite = get_sprite_with_unique_id(0x1D);
    if (sprite != NULL)
        sprite->opacity = 1.0f;
}

/* Hide the title/info frames */
void fn_3_14BAC(void)
{
    struct Sprite *sprite;

    lbl_3_bss_194 = 1;
    sprite = get_sprite_with_unique_id(0x1C);
    if (sprite != NULL)
        sprite->opacity = 0.0f;
    sprite = get_sprite_with_unique_id(0x1D);
    if (sprite != NULL)
        sprite->opacity = 0.0f;
}

/* userVar: 1 = new screen title slides in, -1 = previous title slides back, 0 = idle.
 * unk5C: spacing of the current title, unk58: spacing of the previous (stacked) one. */
void sprite_title_str_tick(s8 *status, struct Sprite *sprite)
{
    if (menuWork->unkC != 0)
    {
        sprite->userVar = 1;
        sprite->counter = 16;
    }
    else if (sprite->userVar != -1 && menuWork->unk4 != 0 && sel_menu_info.menuStackPtr > 0)
    {
        sprite->userVar = -1;
        sprite->counter = 16;
    }

    if (sprite->userVar == 1)
    {
        sprite->counter--;
        sprite->unk58 = 3.0f * (CLAMP(sprite->counter - 8, 0, 8) - 8);
        sprite->unk5C = -3.0f * CLAMP(sprite->counter, 0, 8);
        if (sprite->counter == 0)
        {
            sprite->unk5C = 0.0f;
            sprite->userVar = 0;
        }
    }
    else if (sprite->userVar == -1)
    {
        sprite->counter--;
        sprite->unk58 = -3.0f * CLAMP(sprite->counter, 0, 8);
        sprite->unk5C = 3.0f * (CLAMP(sprite->counter - 8, 0, 8) - 8);
        if (sprite->counter == 0)
        {
            sprite->unk5C = 0.0f;
            sprite->userVar = 0;
        }
    }
}

void sprite_title_str_disp(struct Sprite *sprite)
{
    struct MenuScreen *screen = &menu_screen_list[menuWork->currScreen];
    struct MenuScreen *prevScreen = NULL;
    float minSpacing;

    if ((s8)g_language_id == 5)
        minSpacing = -24.0f;
    else
        minSpacing = -16.0f;

    if (lbl_3_bss_194 != 0)
        return;

    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_depth(sprite->depth);
    textdraw_set_mul_color(0xFF8000);
    textdraw_set_alignment(sprite->textAlign);
    textdraw_set_border();
    textdraw_set_flags(0x80000000);
    textdraw_set_scale(1.2f, 1.2f);
    textdraw_set_font_style(5);
    if (sel_menu_info.menuStackPtr > 0)
    {
        prevScreen = &menu_screen_list[(s8)sel_menu_info.menuStack[sel_menu_info.menuStackPtr - 1]];
        if (prevScreen->bitflag & 0x200)
            prevScreen = &menu_screen_list[(s8)sel_menu_info.menuStack[sel_menu_info.menuStackPtr - 2]];
    }

    textdraw_set_spacing(sprite->unk5C > minSpacing ? sprite->unk5C : minSpacing, 0.0f);
    textdraw_set_alpha(-24.0f == sprite->unk5C ? 0.0f : 1.0f);
    textdraw_set_pos(sprite->x, sprite->y);
    textdraw_print_and_fit_to_width(390.0f, (s8)screen->unk0 == -1 ? "" : lbl_3_data_C568[(s8)screen->unk0][(s8)g_language_id]);

    if (prevScreen != NULL && sprite->counter > 0)
    {
        textdraw_set_spacing(sprite->unk58 > minSpacing ? sprite->unk58 : minSpacing, 0.0f);
        textdraw_set_alpha(-24.0f == sprite->unk58 ? 0.0f : 1.0f);
        textdraw_set_pos(sprite->x, sprite->y);
        textdraw_print_and_fit_to_width(390.0f, (s8)prevScreen->unk0 == -1 ? "" : lbl_3_data_C568[(s8)prevScreen->unk0][(s8)g_language_id]);
    }
    reset_ui_widescreen_scale_mtx();
}

void sprite_info_str_tick(s8 *status, struct Sprite *sprite)
{
    if (menuWork->unkC != 0 || menuWork->unk18 != 0)
        sprite->userVar = 0;
    if (sprite->userVar < 0x200)
        sprite->userVar = sprite->userVar + (((s8)g_language_id == 5) ? 2 : 4);
    if (sprite->counter == 0 && menuWork->unk8 == 0xF && menuWork->unk4 == 0)
        sprite->counter = 16;
    if (sprite->counter > 0)
    {
        sprite->counter--;
        sprite->unk58 = 0.033333f * (15 - sprite->counter);
    }
}

/* Description line of the selected menu entry; userVar is the typewriter counter. */
void sprite_info_str_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    char text[0x100];
    struct MenuScreen *screen = &menu_screen_list[menuWork->currScreen];

    if (lbl_3_bss_194 == 0 && screen->entries != NULL)
    {
        set_ui_widescreen_scale_mtx(0x140);
        if (sprite->counter > 0)
        {
            /* frame flash when the description changes */
            req.bmpId = 0x1701;
            req.x = sprite->x;
            req.y = 1.0f + sprite->y;
            req.z = sprite->depth - 0.01f;
            req.u1 = 0.5f - sprite->unk58;
            req.v1 = 0.5f - sprite->unk58;
            req.u2 = 0.5f + sprite->unk58;
            req.v2 = 0.5f + sprite->unk58;
            req.scaleX = 2.0f * sprite->unk58;
            req.scaleY = 2.0f * sprite->unk58;
            req.rotation = 0;
            req.opacity = 1.0f;
            req.unk30 = -1;
            req.flags = 0xA;
            req.mulColor = -1;
            req.addColor = 0;
            draw_sprite_draw_request(&req);
            if (sprite->counter == 1)
            {
                reset_ui_widescreen_scale_mtx();
                return;
            }
        }

        textdraw_reset();
        fn_3_140CC(MAX(sprite->userVar - 4, 0) >> 1);
        textdraw_set_depth(sprite->depth);
        textdraw_set_pos(sprite->x, sprite->y);
        strcpy(text, screen->entries[(s8)rootMenu->unk0[menuWork->currScreen]].description[(s8)g_language_id]);
        if (menuWork->unk38 != NULL)
            strcpy(text, menuWork->unk38);

        if (menuWork->currScreen == MENUSCREEN_MODE_SELECT && (s8)rootMenu->unk0[menuWork->currScreen] == 1)
            textdraw_printf_and_fit_to_width(390.0f, text, get_num_of_unlocked_party_games());
        else if (menuWork->currScreen == MENUSCREEN_PARTY_GAME_SELECT)
            textdraw_printf_and_fit_to_width(390.0f, text, 2500);
        else if (menuWork->currScreen == MENUSCREEN_NUMBER_OF_PLAYERS)
            textdraw_printf_and_fit_to_width(390.0f, text, menuWork->unk3C);
        else if (screen->bitflag & 0x40)
        {
            int isEight = 1;

            if (!((u8)(menuWork->currScreen - 0x12) <= 1))
                isEight = 0;
            textdraw_printf_and_fit_to_width(390.0f, text, isEight ? 8 : 4);
        }
        else
            textdraw_print_and_fit_to_width(390.0f, text);
        reset_ui_widescreen_scale_mtx();
    }
}

void sprite_start_mask_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar == 1)
    {
        sprite->depth = 0.1f;
        sprite->userVar = 0;
    }
    else if (sprite->userVar == 2)
        *status = 0;
}

/* Menu entry sprite (tags 0x3C-0x47, one per entry of the current screen).
 * userVar is the animation state: 1/10/30/40 pop in, 2/31/41 idle, 3/32/42 selected
 * (flash and grow), 4 fade out, 5-7 swing in/out, 12/22 picked, 20 unfold. */
void sprite_menu_basic_tick(s8 *status, struct Sprite *sprite)
{
    struct MenuScreen *screen = &menu_screen_list[menuWork->currScreen];
    int entryIdx = sprite->tag - 0x3C;
    struct MenuScreen *screen2 = &menu_screen_list[menuWork->currScreen];
    float scale;
    float baseScale;
    float scale2;

    if (menuWork->currScreen == 2)
        baseScale = 1.0f;
    else if (screen2->bitflag & 0x10)
        baseScale = 0.6f;
    else if (screen2->bitflag & 0x4000)
        baseScale = 0.8f;
    else if (screen2->bitflag & 0x8000)
        baseScale = 1.2f;
    else
        baseScale = 1.0f;
    scale = baseScale;

    scale2 = 1.0f;
    if (menuWork->currScreen == 1 || menuWork->currScreen == 0x16)
        scale2 = 1.2f;

    switch (sprite->userVar)
    {
    case 1:
        if (--sprite->counter == 0)
            sprite->userVar = 2;
        if (sprite->counter >= 8)
            sprite->scaleX = 0.0f;
        else
            sprite->scaleX = math_sin((8 - sprite->counter) << 11);
        sprite->scaleX *= scale * scale2;
        break;
    case 2:
        sprite->counter++;
        break;
    case 3:
        if (sprite->unk50 == 0)
        {
            sprite->unk50 = 45;
            sprite->shadowAlpha = scale * scale2;
            sprite->depth = 0.08f;
        }
        if (--sprite->unk50 == 0 && !(menu_screen_list[menuWork->nextScreen].bitflag & 0x100))
        {
            if (menuWork->nextScreen == 2)
                sprite->userVar = 4;
            else
            {
                fn_3_17E5C(sprite);
                sprite->userVar = 0;
                sprite->scaleX = 0.0f;
            }
        }
        else
        {
            sprite->unk58 += 0.2f * (320.0f - sprite->unk58);
            sprite->y += 0.2f * (220.0f - sprite->y);
            sprite->scaleX += 0.2f * (1.1f - sprite->scaleX);
            sprite->addR = 64.0f * math_sin(((60 - sprite->unk50) % 15) * 0x492);
            sprite->addG = sprite->addR;
            sprite->addB = sprite->addR;
        }
        break;
    case 4:
        if (sprite->unk50 == 0)
            sprite->unk50 = 12;
        if (--sprite->unk50 == 0)
            sprite->userVar = 0;
        sprite->opacity = sprite->unk50 / 12.0f;
        break;
    case 5:
        if (--sprite->counter == 0)
            sprite->userVar = 2;
        if (sprite->counter >= 16)
        {
            sprite->scaleX = 1.0f;
            sprite->unk58 = sprite->x - 100.0f * math_sin(0x4000);
            sprite->opacity = 0.0f;
            sprite->rotation = 0x4000;
        }
        else
        {
            sprite->scaleX = 1.0f;
            sprite->unk58 = sprite->x - 100.0f * math_sin(sprite->counter << 10);
            sprite->opacity = math_sin((16 - sprite->counter) << 10);
            sprite->rotation = 16384.0f * math_sin(sprite->counter << 10);
        }
        sprite->scaleX *= scale;
        break;
    case 6:
        if (sprite->unk50 == 0)
        {
            sprite->unk50 = 45;
            sprite->depth = 0.08f;
        }
        sprite->unk50--;
        sprite->unk58 += 0.2f * (sprite->x - sprite->unk58);
        sprite->opacity += 0.2f * (1.0f - sprite->opacity);
        sprite->rotation = sprite->rotation + 0.2f * -(float)sprite->rotation;
        sprite->addR = 64.0f * math_sin(((60 - sprite->unk50) % 15) * 0x492);
        sprite->addG = sprite->addR;
        sprite->addB = sprite->addR;
        break;
    case 7:
        sprite->unk58 += 4.0f;
        sprite->opacity += 0.2 * -sprite->opacity;
        sprite->rotation -= 0x200;
        break;
    case 10:
        if (--sprite->counter == 0)
            sprite->userVar = 11;
        if (sprite->counter >= 8)
            sprite->scaleX = 0.0f;
        else
            sprite->scaleX = math_sin((8 - sprite->counter) << 11);
        sprite->scaleX *= scale;
        break;
    case 12:
        if (sprite->unk50 == 0)
            sprite->unk50 = 45;
        if (--sprite->unk50 == 0 && !(menu_screen_list[menuWork->nextScreen].bitflag & 0x100))
            sprite->userVar = 4;
        sprite->scaleX += 0.5f * (1.0f - sprite->scaleX);
        break;
    case 20:
        if (--sprite->counter == 0)
            sprite->userVar = 21;
        sprite->scaleY = (8 - sprite->counter < 4 ? 8 - sprite->counter : 4) / 4.0f;
        sprite->scaleX = (4 - sprite->counter > 1 ? 4 - sprite->counter : 1) / 4.0f;
        break;
    case 22:
        if (sprite->unk50 == 0)
            sprite->unk50 = 45;
        if (--sprite->unk50 == 0 && !(menu_screen_list[menuWork->nextScreen].bitflag & 0x100))
            sprite->userVar = 4;
        sprite->scaleX += 0.5f * (1.0f - sprite->scaleX);
        sprite->scaleY += 0.5f * (1.0f - sprite->scaleY);
        break;
    case 30:
        if (--sprite->counter == 0)
            sprite->userVar = 31;
        if (sprite->counter >= 8)
            sprite->scaleX = 0.0f;
        else
            sprite->scaleX = math_sin((8 - sprite->counter) << 11);
        sprite->scaleX *= scale;
        break;
    case 31:
        sprite->counter++;
        break;
    case 32:
        if (sprite->unk50 == 0)
        {
            sprite->unk50 = 45;
            sprite->depth = 0.08f;
        }
        if (--sprite->unk50 == 0 && !(menu_screen_list[menuWork->nextScreen].bitflag & 0x100))
            sprite->userVar = 4;
        sprite->scaleX += 0.2f * (1.2f * (scale * scale2) - sprite->scaleX);
        sprite->addR = 64.0f * math_sin(((60 - sprite->unk50) % 15) * 0x492);
        sprite->addG = sprite->addR;
        sprite->addB = sprite->addR;
        break;
    case 33:
        sprite->userVar = 4;
        break;
    case 40:
        if (--sprite->counter == 0)
            sprite->userVar = 41;
        if (sprite->counter >= 8)
            sprite->scaleX = 0.0f;
        else
            sprite->scaleX = math_sin((8 - sprite->counter) << 11);
        sprite->scaleX *= scale * scale2;
        break;
    case 41:
        sprite->counter++;
        break;
    case 42:
        if (sprite->unk50 == 0)
            sprite->unk50 = 45;
        if (--sprite->unk50 == 0)
            sprite->userVar = 4;
        sprite->scaleX += 0.2f * (scale - sprite->scaleX);
        sprite->addR = 64.0f * math_sin(((60 - sprite->unk50) % 15) * 0x492);
        sprite->addG = sprite->addR;
        sprite->addB = sprite->addR;
        break;
    }

    if (sprite->userVar != 0)
    {
        if (screen->bitflag & 0x2000)
        {
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0xFF;
        }
        else if ((s8)rootMenu->unk0[menuWork->currScreen] == entryIdx && !(screen->bitflag & 0x800))
        {
            if (sprite->userVar == 2 || sprite->userVar == 31)
                sprite->scaleX += 0.2f * (scale * scale2 - sprite->scaleX);
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0xFF;
            sprite->scaleY = sprite->scaleX;
        }
        else
        {
            if (sprite->userVar == 2 || sprite->userVar == 31)
                sprite->scaleX += 0.2f * (0.8f * scale - sprite->scaleX);
            sprite->mulR = 0xA0;
            sprite->mulG = 0xA0;
            sprite->mulB = 0xA0;
            sprite->scaleY = sprite->scaleX;
        }
    }
}

/* ---- menu entry sprites (sprite_menu_basic_disp) ---- */

/* Bitmap ids of the entry icons, one array per menu screen, indexed by the entry's item
 * number (see lbl_3_data_CC60). */
s32 menu_bmp_ids[3] = { 0x170D, 0x1733, 0x170F };
s32 lbl_3_data_CAC0[3] = { 0x1728, 0x1726, 0x1727 };
s32 lbl_3_data_CACC[4] = { 0x1710, 0x1710, 0x1710, 0x1710 };
s32 lbl_3_data_CADC[3] = { 0x1729, 0x1725, 0x172C };
s32 lbl_3_data_CAE8[2] = { 0x1721, 0x1722 };
s32 lbl_3_data_CAF0[2] = { 0x1730, 0x172F };
s32 lbl_3_data_CAF8[4] = { 0x1717, 0x1715, 0x1716, 0x1714 };
s32 lbl_3_data_CB08[3] = { 0x1719, 0x171A, 0x1718 };
s32 lbl_3_data_CB14[3] = { 0x171B, 0x171C, 0x171D };
s32 lbl_3_data_CB20[2] = { 0x1724, 0x1723 };
s32 lbl_3_data_CB28[3] = { 0x1729, 0x1725, 0x172C };
s32 lbl_3_data_CB34[2] = { 0x171E, 0x171F };
s32 lbl_3_data_CB3C[3] = { 0x1730, 0x172F, 0x172A };
s32 lbl_3_data_CB48[2] = { 0x1730, 0x172F };
s32 lbl_3_data_CB50[3] = { 0x172B, 0x1720, 0x172F };
s32 lbl_3_data_CB5C[2] = { 0x1710, 0x1710 };
s32 lbl_3_data_CB64[3] = { 0x1710, 0x1710, 0x1710 };
s32 lbl_3_data_CB70[3] = { 0x1710, 0x1710, 0x1710 };
s32 lbl_3_data_CB7C[14] = {
    0x1731, 0x1731, 0x1731, 0x1731, 0x1731, 0x1731, 0x1731,
    0x1731, 0x1731, 0x1731, 0x1731, 0x1731, 0x1731, 0x1731,
};
s32 lbl_3_data_CBB4[4] = { 0x1735, 0x1736, 0x1734, 0x174C };
s32 lbl_3_data_CBC4[3] = { 0x1738, 0x1739, 0x173A };
s32 lbl_3_data_CBD0[4] = { 0x1707, 0x1709, 0x170A, 0x1708 };
s32 lbl_3_data_CBE0[4] = { 0, 0, 0, 0 };
s32 lbl_3_data_CBF0[6] = { 0x1740, 0x1741, 0x1742, 0x1743, 0x1745, 0x1744 };
s32 lbl_3_data_CC08[3] = { 0x1746, 0x1747, 0x1748 };
s32 lbl_3_data_CC14[5] = { 0, 0, 0, 0, 0 };
s32 lbl_3_data_CC28[3] = { 0x1749, 0x174A, 0x174B };
s32 lbl_3_data_CC34[8] = { 0x1732, 0x1732, 0x1732, 0x1732, 0x1732, 0x1732, 0x1732, 0x1732 };
s32 lbl_3_data_CC54[3] = { 0, 0, 0 };

struct MenuEntryBmps
{
    /*0x00*/ s8 screen; /* MENUSCREEN_* */
    /*0x04*/ s32 *bmpIds; /* indexed by item */
    /*0x08*/ u32 unk8;
}; /* size = 0xC */

struct MenuEntryBmps lbl_3_data_CC60[34] = {
    { 0x00, menu_bmp_ids, 0 },
    { 0x07, lbl_3_data_CAC0, 0 },
    { 0x04, lbl_3_data_CACC, 0 },
    { 0x0F, lbl_3_data_CADC, 0 },
    { 0x15, lbl_3_data_CAE8, 0 },
    { 0x1F, lbl_3_data_CAF0, 0 },
    { 0x20, lbl_3_data_CAF8, 0 },
    { 0x24, lbl_3_data_CB08, 0 },
    { 0x25, lbl_3_data_CB14, 0 },
    { 0x28, lbl_3_data_CB20, 0 },
    { 0x2B, lbl_3_data_CB28, 0 },
    { 0x38, lbl_3_data_CB34, 0 },
    { 0x3D, lbl_3_data_CB3C, 0 },
    { 0x42, lbl_3_data_CB48, 0 },
    { 0x48, lbl_3_data_CB50, 0 },
    { 0x49, lbl_3_data_CB5C, 0 },
    { 0x01, lbl_3_data_CB7C, 0 },
    { 0x08, lbl_3_data_CBB4, 0 },
    { 0x26, lbl_3_data_CBC4, 0 },
    { 0x05, lbl_3_data_CBD0, 0 },
    { 0x06, lbl_3_data_CBD0, 0 },
    { 0x3E, lbl_3_data_CBE0, 0 },
    { 0x10, lbl_3_data_CBF0, 0 },
    { 0x11, lbl_3_data_CBF0, 0 },
    { 0x2C, lbl_3_data_CC08, 0 },
    { 0x2D, lbl_3_data_CC08, 0 },
    { 0x16, lbl_3_data_CC14, 0 },
    { 0x43, lbl_3_data_CBD0, 0 },
    { 0x32, lbl_3_data_CC28, 0 },
    { 0x09, lbl_3_data_CBB4, 0 },
    { 0x02, lbl_3_data_CC34, 0 },
    { 0x44, lbl_3_data_CC54, 0 },
    { 0x4A, lbl_3_data_CB64, 0 },
    { 0x4B, lbl_3_data_CB70, 0 },
};

/* MENUSCREEN 4 (player count): item n draws n+1 monkey icons; their offsets start at
 * index 0, 1, 3 or 6 of these two tables (triangular: 1 + 2 + 3 + 4 entries). */
s8 lbl_3_data_CDF8[10] = { 0, 0, 0, 0, 0, 0, 0, -27, 27, 0 };
s8 lbl_3_data_CE04[10] = { -2, -27, 21, -36, -2, 32, -34, -2, -2, 30 };
float lbl_3_data_CE10[4] = { 1.0f, 0.727f, 0.5909f, 0.56818f };

/* team screens (0x49-0x4B): offsets of the four player markers */
s8 lbl_3_data_CE20[4] = { -24, 24, -24, 24 };
s8 lbl_3_data_CE24[4] = { -23, -23, 16, 16 };

/* screens with bitflag 0x2000: five-part pictures, per item: bitmap id, position (centred
 * on 66, 72) and animation type of each part */
s32 lbl_3_data_CE28[4][5] = {
    { 0x1704, 0x1703, 0x1706, 0x1705, 0x1706 },
    { 0x1704, 0x1704, 0x1706, 0x1703, 0x1705 },
    { 0x1703, 0x1706, 0x1705, 0x1705, 0x1704 },
    { 0x1703, 0x1705, 0x1706, 0x1704, 0x1703 },
};
u8 lbl_3_data_CE78[4][5] = {
    { 0x42, 0x1C, 0x68, 0x42, 0x42 },
    { 0x1C, 0x68, 0x42, 0x42, 0x42 },
    { 0x42, 0x42, 0x1C, 0x68, 0x42 },
    { 0x10, 0x42, 0x74, 0x42, 0x42 },
};
u8 lbl_3_data_CE8C[4][5] = {
    { 0x15, 0x28, 0x28, 0x43, 0x64 },
    { 0x0E, 0x0E, 0x21, 0x43, 0x65 },
    { 0x17, 0x39, 0x52, 0x52, 0x64 },
    { 0x16, 0x16, 0x16, 0x42, 0x64 },
};
/* 0: static, 1: bobs vertically, 2: bobs horizontally, 3: pulses and rocks */
u8 lbl_3_data_CEA0[4][5] = {
    { 1, 0, 0, 2, 3 },
    { 1, 1, 0, 2, 3 },
    { 1, 0, 2, 2, 3 },
    { 1, 1, 1, 2, 3 },
};

#ifndef __MWERKS__
int pc_party_game_available(int gameType);    /* pc_port pc_party_avail.c */
const char *pc_party_unavail_tag(void);
#endif

void sprite_menu_basic_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    struct SpriteDrawRequest saved;
    struct MenuScreen *screenInfo;
    struct MenuEntryBmps *bmps;
    int entry;
    s8 item;
    s8 screen;
    s8 shift;
    int i;
    float f;
    int badge;
#ifndef __MWERKS__
    int pcUnavail = 0;
#endif

    entry = sprite->tag - 0x3C;
    item = sprite->unk54 & 0xFF;
    screen = (sprite->unk54 >> 8) & 0xFF;
    shift = 0;
    screenInfo = &menu_screen_list[screen];
    set_ui_widescreen_scale_mtx(320);

    /* entries the selected player count can't use are dimmed */
    if (menuWork->currScreen == 4 && sprite->tag < 0x49 && item >= (s8)sel_menu_info.unkB)
        shift = 2;
#ifndef __MWERKS__
    /* PC: so are the party games that aren't decompiled yet */
    if (screen == MENUSCREEN_PARTY_GAME_SELECT && sprite->tag < 0x49 && item >= 0 && item < 12
     && !pc_party_game_available(lbl_3_rodata_4[item]))
    {
        pcUnavail = 1;
        shift = 1;
    }
#endif

    req.bmpId = 0x170E;
    req.x = sprite->unk58;
    req.y = sprite->y;
    req.z = 0.02f + sprite->depth;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = sprite->scaleX;
    req.scaleY = sprite->scaleY;
    req.rotation = sprite->rotation;
    req.opacity = sprite->opacity;
    req.unk30 = -1;
    req.flags = 0xA;
    req.mulColor = ((sprite->mulR >> shift) << 16) | ((sprite->mulG >> shift) << 8) | (sprite->mulB >> shift);
    req.addColor = (sprite->addR << 16) | (sprite->addG << 8) | sprite->addB;

    /* the entry's frame */
    if (!(screenInfo->bitflag & 0x400))
    {
        if (screenInfo->bitflag & 0x800)
        {
            reset_ui_widescreen_scale_mtx();
            req.z = 3.5f;
            req.flags |= 0x40000;
            if (modeCtrl.wideMode)
            {
                s8 wideOffsets[4] = { 56, 18, -18, -56 };

                req.x += wideOffsets[entry];
                req.scaleX *= 0.65999997f;
                req.scaleY *= 0.88f;
            }
            draw_sprite_draw_request(&req);
        }
        else if (screenInfo->bitflag & 0x2000)
        {
            req.bmpId = 0x170B;
            draw_sprite_draw_request(&req);
        }
        else if (screenInfo->bitflag & 0x4000)
        {
            req.x += (entry & 1) ? 35 : -35;
            draw_sprite_draw_request(&req);
        }
        else if (screenInfo->bitflag & 0x8000)
        {
            req.x += (entry - 1) * 20;
            req.bmpId = 0x170C;
            req.scaleX *= 1.2199951f;
            req.scaleY *= 1.1049955f;
            draw_sprite_draw_request(&req);
            /* top and bottom edges */
            req.u1 = 0.05f;
            req.u2 = 0.95f;
            req.v1 = 0.05f;
            req.v2 = 0.5f;
            req.scaleX *= req.u2 - 0.05f;
            req.scaleY *= req.v2 - 0.05f;
            req.z -= 0.001f;
            req.opacity = MIN(1.1f * sprite->opacity, 1.0f);
            req.y += 19.9f * sprite->scaleY;
            draw_sprite_draw_request(&req);
            req.y -= 39.8f * sprite->scaleY;
            draw_sprite_draw_request(&req);
            req.u1 = 0.0f;
            req.u2 = 1.0f;
            req.v1 = 0.0f;
            req.v2 = 1.0f;
            req.scaleX = sprite->scaleX;
            req.scaleY = sprite->scaleY;
            req.opacity = sprite->opacity;
        }
        else if (screen == 2)
        {
            req.bmpId = 0x170C;
            req.y -= (entry < 4) ? 40 : 15;
            if (!are_all_party_games_unlocked())
            {
                /* the party games entry is hidden until one of them is unlocked */
                if (entry != 1)
                {
                    if (entry == 0)
                        req.x += 75.0f;
                    else if (entry <= 3)
                        req.x -= 75.0f;
                    draw_sprite_draw_request(&req);
                }
                else
                {
                    return;
                }
            }
            else
            {
                draw_sprite_draw_request(&req);
            }
        }
        else
        {
            draw_sprite_draw_request(&req);
        }
    }

    saved = req;

    /* the entry's icon */
    for (i = 0, bmps = lbl_3_data_CC60; i < 34; i++, bmps++)
    {
        if (bmps->screen != screen)
            continue;

        req.bmpId = bmps->bmpIds[item];
        req.z -= 0.005f;
        if (screen == 0)
        {
            /* keep the frame's colours */
        }
        else if (screen == 1)
        {
            req.scaleX *= 1.3f;
            req.scaleY *= 1.3f;
            req.mulColor = (u8)(0.2f * (sprite->mulR >> shift)) << 16;
        }
        else if (screenInfo->bitflag & 0x400)
        {
            /* keep the frame's colours */
        }
        else if (screenInfo->bitflag & 0x800)
        {
            req.scaleX *= 0.95f;
            req.scaleY *= 0.95f;
            req.mulColor = 0xFFFFFF;
            if (sprite->userVar == 10)
                req.rotation = 8192.0f * math_sin(sprite->counter << 11);
        }
        else if (screenInfo->bitflag & 0x2000)
        {
            /* the five-part picture sets its own colours */
        }
        else if (screenInfo->bitflag & 0x4000)
        {
            req.scaleX *= 0.7f;
            if (entry == 2)
                req.scaleX *= 0.35f;
            req.scaleY = req.scaleX;
            if (entry == 4)
            {
                req.x += 5.0f;
                req.y += 5.0f;
            }
            else if (entry == 5)
            {
                req.x += 3.0f;
                req.y += 3.0f;
            }
            /* green, blue and red pairs */
            if (entry == 0 || entry == 1)
                req.mulColor = (entry == (s8)rootMenu->unk0[menuWork->currScreen]) ? 0x00FF00 : 0x48A048;
            else if (entry == 2 || entry == 3)
                req.mulColor = (entry == (s8)rootMenu->unk0[menuWork->currScreen]) ? 0x0000FF : 0x4848A0;
            else if (entry == 4 || entry == 5)
                req.mulColor = (entry == (s8)rootMenu->unk0[menuWork->currScreen]) ? 0xFF0000 : 0xA04848;
            if (entry != (s8)rootMenu->unk0[menuWork->currScreen])
                req.opacity *= 0.5f;
        }
        else if (screen == 2)
        {
            req.scaleX *= 1.2f;
            req.scaleY *= 1.2f;
            req.mulColor = (u8)(0.2f * (sprite->mulR >> shift)) << 16;
        }
        else
        {
            req.mulColor = (u8)(0.2f * (sprite->mulR >> shift)) << 16;
        }

        /* the focused entry glows */
        if (item == (s8)rootMenu->unk0[menuWork->currScreen] && menuWork->unk8 == 0 && sprite->tag < 0x49)
        {
            int glow;

            f = (menuWork->currScreen == 0) ? 32.0f : 64.0f;
            glow = f * fabs(math_sin(sprite->counter * 0x180));
            req.addColor = ((u8)glow << 16) | ((u8)glow << 8) | (u8)glow;
        }

        if (screen == 1)
        {
            /* 7 x 2 grid of icons, cell 6 unused */
            s8 cell = item + ((item >= 6) ? 1 : 0);

            req.u1 = (cell % 7) / 7.0f;
            req.u2 = 0.142857f + req.u1;
            req.v1 = (cell / 7) * 0.5f;
            req.v2 = 0.5f + req.v1;
            req.scaleX *= req.u2 - req.u1;
            req.scaleY *= req.v2 - req.v1;
        }
        else if (screen == 2)
        {
            /* 4 x 2 grid of icons */
            s8 cells[8] = { 1, 7, 6, 3, 4, 5, 2, 0 };
            s8 cell = cells[item];

            req.u1 = (cell % 4) * 0.25f;
            req.u2 = 0.25f + req.u1;
            req.v1 = (cell / 4) * 0.5f;
            req.v2 = 0.5f + req.v1;
            req.scaleX *= req.u2 - req.u1;
            req.scaleY *= req.v2 - req.v1;
        }
        else if (screen == 4)
        {
            /* player count: one monkey icon per player */
            s8 start;
            int j;

            if (item == 0)
                start = 0;
            else if (item == 1)
                start = 1;
            else if (item == 2)
                start = 3;
            else
                start = 6;
            for (j = 0; j < item + 1; j++)
            {
                req.scaleX = sprite->scaleX * lbl_3_data_CE10[item];
                req.scaleY = req.scaleX;
                req.x = sprite->unk58 + sprite->scaleX * lbl_3_data_CDF8[start + j];
                req.y = sprite->y + sprite->scaleX * lbl_3_data_CE04[start + j];
                draw_sprite_draw_request(&req);
            }
            break;
        }
        else if (screen == 0x49 || screen == 0x4A || screen == 0x4B)
        {
            /* team split: "VS" over the four player markers */
            char *vsStr[6] = { "VS", "", "", "", "", "VS" };
            char *cpuStr[6] = { "C", "", "", "", "", "C" };
            u32 teamColors[4] = { 0xC04040, 0x4040C0, 0xC0C040, 0x40C040 };
            s8 teams2[2][4] = { { 1, 2, 0, 0 }, { 1, 0, 2, 0 } };
            s8 teams3[3][4] = { { 1, 0, 2, 3 }, { 1, 2, 3, 0 }, { 1, 3, 2, 0 } };
            s8 teams4[3][4] = { { 1, 2, 3, 4 }, { 1, 3, 2, 4 }, { 1, 4, 2, 3 } };
            s8 (*teams)[4];
            s8 *team;
            int j;

            if (modeCtrl.playerCount == 4)
                teams = teams4;
            else if (modeCtrl.playerCount == 3)
                teams = teams3;
            else
                teams = teams2;
            team = teams[item];

            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_scale(0.7f * sprite->scaleX, 0.7f * sprite->scaleY);
            textdraw_set_alignment(4);
            textdraw_set_flags(0x80000000);
            textdraw_set_alpha(req.opacity);
            textdraw_set_depth(req.z);
            textdraw_set_mul_color((u8)(0.2f * (sprite->mulR >> shift)) << 16);
            textdraw_set_add_color(req.addColor);
            textdraw_set_pos(sprite->unk58, sprite->y + 2.0f * sprite->scaleY);
            textdraw_print(vsStr[(s8)g_language_id]);
            textdraw_set_rot_z(0x100);
            textdraw_set_alpha(0.5f);
            textdraw_set_scale(6.0f * sprite->scaleX, 0.4f * sprite->scaleY);
            textdraw_set_pos(sprite->unk58, sprite->y + 2.0f * sprite->scaleY);
            textdraw_print("-");
            textdraw_set_rot_z(0);
            textdraw_set_alpha(req.opacity);
            req.z += 0.0025f;
            textdraw_set_depth(req.z - 0.0025f + 0.1f * lbl_1_bss_6FB7C);
            textdraw_set_scale(0.9f * sprite->scaleX, 0.7f * sprite->scaleY);
            textdraw_set_font_style(2);
            for (j = 0; j < 4; j++, team++)
            {
                req.scaleX = 0.5f * sprite->scaleX;
                req.scaleY = req.scaleX;
                req.x = sprite->unk58 + lbl_3_data_CE20[j] * sprite->scaleX;
                req.y = sprite->y + lbl_3_data_CE24[j] * sprite->scaleX;
                draw_sprite_draw_request(&req);
                textdraw_set_pos(req.x, req.y + 3.0f * sprite->scaleY);
                if (*team != 0)
                {
                    textdraw_set_mul_color(teamColors[*team - 1]);
                    textdraw_printf("%d", *team);
                }
                else
                {
                    textdraw_set_mul_color(0xC0C0C0);
                    textdraw_print(cpuStr[(s8)g_language_id]);
                }
            }
            break;
        }
        else if (screenInfo->bitflag & 0x2000)
        {
            /* five-part picture */
            int j;

            req.addColor = 0;
            req.opacity = sprite->opacity * (sprite->scaleX * sprite->scaleY);
            for (j = 0; j < 5; j++)
            {
                req.bmpId = lbl_3_data_CE28[item][j];
                req.x = sprite->unk58 + req.opacity * (lbl_3_data_CE78[item][j] - 66);
                req.y = sprite->y + req.opacity * (lbl_3_data_CE8C[item][j] - 72);
                switch (lbl_3_data_CEA0[item][j])
                {
                case 1:
                    req.y += CLAMP(20.0f * math_sin(g_font_char_var_3 * 0x180), -5.0f, 0.0f);
                    break;
                case 2:
                    req.x += CLAMP(20.0f * math_sin(g_font_char_var_3 * 0x180), -5.0f, 5.0f);
                    break;
                case 3:
                    req.scaleX *= 1.2f;
                    if (menuWork->unk20[item] > 0)
                        req.scaleX += 0.5f * math_sin((menuWork->unk20[item] % 30) * 0x222);
                    req.scaleY = req.scaleX;
                    req.z -= 0.005f;
                    req.rotation = 2048.0f * math_sin(g_font_char_var_3 * 0x180);
                    break;
                }
                draw_sprite_draw_request(&req);
            }
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_scale(0.6f, 0.6f);
            textdraw_set_alignment(4);
            textdraw_set_flags(0x80000000);
            textdraw_set_alpha(sprite->opacity * sprite->scaleX);
            textdraw_set_depth(0.01f + sprite->depth);
            textdraw_set_mul_color((u8)(0.2f * (sprite->mulR >> shift)) << 16);
            textdraw_set_pos(sprite->unk58, 55.0f + sprite->y);
            textdraw_print(screenInfo->entries[entry].name[(s8)g_language_id]);
            break;
        }

        if (req.bmpId != 0)
            draw_sprite_draw_request(&req);
        if (screenInfo->bitflag & 0x40000)
        {
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_scale(2.2f * sprite->scaleX, 2.2f * sprite->scaleX);
            textdraw_set_alignment(4);
            textdraw_set_alpha(MIN(sprite->opacity * sprite->scaleX, 1.0f));
            textdraw_set_depth(sprite->depth);
            textdraw_set_border();
            textdraw_set_mul_color((u8)(0.2f * (sprite->mulR >> shift)) << 16);
            textdraw_set_pos(sprite->unk58, sprite->y);
            textdraw_print(screenInfo->entries[entry].name[(s8)g_language_id]);
        }
        break;
    }

    /* locked entries get a flashing lock */
    req = saved;
    if ((locked_menu_items & (1U << item)) && sprite->tag < 0x49)
    {
        req.bmpId = 0x1737;
        req.z -= 0.01f;
        req.opacity *= 0.5f;
        req.mulColor = (u8)(255.0 * fabs(math_sin(g_font_char_var_3 * 0x180))) << 16;
        draw_sprite_draw_request(&req);
    }
    req = saved;
#ifndef __MWERKS__
    /* PC: and tagged "not yet" (the description box says why) */
    if (pcUnavail)
    {
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_scale(0.5f * sprite->scaleX, 0.5f * sprite->scaleY);
        textdraw_set_alignment(4);
        textdraw_set_flags(0x80000000);
        textdraw_set_alpha(MIN(sprite->opacity * sprite->scaleX, 1.0f));
        textdraw_set_depth(sprite->depth - 0.02f);
        textdraw_set_border();
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(sprite->unk58, sprite->y + 26.0f * sprite->scaleY);
        textdraw_print((char *)pc_party_unavail_tag());
    }
#endif

    if (sprite->userVar != 2 && sprite->userVar != 0x29)
    {
        reset_ui_widescreen_scale_mtx();
        return;
    }

    /* "new" badge on the entries that lead to something that can be unlocked */
    badge = FALSE;
    f = 1.0f;
    if (!are_all_party_games_unlocked() && is_able_to_unlock_party_game()
     && ((screen == 0 && entry == 1)
      || (screen == 1 && entry == 6 && !g_check_if_partygame_unlocked(lbl_3_rodata_4[6]))
      || (screen == 1 && entry == 7 && !g_check_if_partygame_unlocked(lbl_3_rodata_4[7]))
      || (screen == 1 && entry == 8 && !g_check_if_partygame_unlocked(lbl_3_rodata_4[8]))
      || (screen == 1 && entry == 9 && !g_check_if_partygame_unlocked(lbl_3_rodata_4[9]))
      || (screen == 1 && entry == 10 && !g_check_if_partygame_unlocked(lbl_3_rodata_4[10]))
      || (screen == 1 && entry == 11 && !g_check_if_partygame_unlocked(lbl_3_rodata_4[11]))))
        badge = TRUE;
    if (are_all_party_games_unlocked() && !are_all_gifts_unlocked()
     && ((is_able_to_unlock_gift() && !are_all_extra_monkeys_unlocked())
      || (fn_1_16CEE4() && !are_all_movies_unlocked())
      || (fn_1_16CFFC() && !is_staff_credits_game_unlocked()))
     && ((screen == 0 && entry == 2) || (screen == 2 && entry == 1)))
    {
        badge = TRUE;
        if (screen == 2)
            f = 0.8f;
    }
    if (badge)
    {
        if (req.scaleX > f)
            req.scaleX = f;
        req.bmpId = 0x173C;
        req.x += -30.0f * req.scaleX;
        req.y += 20.0f * req.scaleX;
        req.z -= 0.015f;
        req.scaleX = req.scaleX * (2.5 * MIN(fabs(math_sin(g_font_char_var_3 << 9)), 0.5));
        req.scaleY = req.scaleX;
        req.mulColor = (entry == (s8)rootMenu->unk0[menuWork->currScreen]) ? 0xFFFFFF : 0x808080;
        req.addColor = 0;
        draw_sprite_draw_request(&req);

        req.scaleX = sprite->scaleX;
        if (req.scaleX > f)
            req.scaleX = f;
        req.bmpId = 0x173B;
        req.x += -15.0f * req.scaleX - 15.0f * req.scaleX;
        req.y += 36.0f * req.scaleX + 5.0f * req.scaleX;
        req.x += req.scaleX * (30.0 * MIN(fabs(math_sin(g_font_char_var_3 << 10)), 0.5));
        req.y -= req.scaleX * (10.0 * MIN(fabs(math_sin(g_font_char_var_3 << 10)), 0.5));
        req.z -= 0.01f;
        req.scaleX = req.scaleX * 0.8f;
        req.scaleY = req.scaleX;
        req.rotation = 0x1000;
        draw_sprite_draw_request(&req);
    }
    reset_ui_widescreen_scale_mtx();
}

/* Spawns a "stacked" copy (tags 0x49-0x4D) of a picked menu entry sprite, which then
 * flies from the picked position back to the entry's slot in the stack row. */
struct Sprite *fn_3_17E5C(struct Sprite *src)
{
    struct Sprite *sprite;

    if (lbl_3_bss_190 >= 5)
        return NULL;
    sprite = get_sprite_with_unique_id(lbl_3_bss_190 + 0x49);
    if (sprite == NULL)
        return NULL;
    sprite->x = src->x;
    sprite->unk58 = src->unk58;
    sprite->y = src->unk5C;
    sprite->unk5C = sprite->x;
    sprite->unk60 = sprite->y;
    sprite->depth = src->depth;
    sprite->scaleX = src->scaleX;
    sprite->shadowAlpha = src->shadowAlpha;
    sprite->scaleY = src->scaleY;
    sprite->opacity = src->opacity;
    sprite->mulR = src->mulR;
    sprite->mulG = src->mulG;
    sprite->mulB = src->mulB;
    sprite->addR = src->addR;
    sprite->addG = src->addG;
    sprite->addB = src->addB;
    sprite->userVar = 1;
    sprite->unk50 = src->tag - 0x3C;
    sprite->unk54 = src->unk54;
    lbl_3_bss_190++;
    return sprite;
}

/* Stacked entry sprite: 1/3 move to (and shrink into) its slot in the stack row,
 * 2 moves back to the entry's own position until that entry is visible again. */
void sprite_menu_basic_stack_tick(s8 *status, struct Sprite *sprite)
{
    static const float slotX[5] = { 160.0f, 240.0f, 320.0f, 400.0f, 480.0f };
    int idx = sprite->tag - 0x49;
    struct Sprite *entry;
    float rate;

    switch (sprite->userVar)
    {
    case 0:
        sprite->scaleX = 0.0f;
        break;
    case 1:
    case 3:
        rate = (sprite->userVar == 3) ? 1.0f : 0.2f;
        sprite->unk58 += rate * (slotX[idx] - sprite->unk58);
        sprite->y += (110.0f - sprite->y) * rate;
        sprite->scaleX += (0.4f - sprite->scaleX) * rate;
        sprite->addR = sprite->addR + -(float)sprite->addR * rate;
        sprite->addG = sprite->addR;
        sprite->addB = sprite->addR;
        break;
    case 2:
        if (sprite->counter == 0)
            sprite->counter = 17;
        if (sprite->counter > 1)
            sprite->counter--;
        if (sprite->counter == 1
            && ((entry = get_sprite_with_unique_id(sprite->unk50 + 0x3C)) == NULL || entry->userVar >= 2))
        {
            sprite->userVar = 0;
            sprite->scaleX = 0.0f;
            sprite->counter = 0;
        }
        else
        {
            sprite->unk58 += 0.25f * (sprite->unk5C - sprite->unk58);
            sprite->y += 0.25f * (sprite->unk60 - sprite->y);
            sprite->scaleX += 0.25f * (sprite->shadowAlpha - sprite->scaleX);
        }
        break;
    }
    sprite->scaleY = sprite->scaleX;
}

/* Menu title plate: 1 unfold, 2 fade in, 3 idle, 4 fold away. */
void sprite_menu_basic_title_tick(s8 *status, struct Sprite *sprite)
{
    switch (sprite->userVar)
    {
    case 1:
        if (sprite->counter == 0)
            sprite->counter = 16;
        if (--sprite->counter == 0)
            sprite->userVar = 2;
        sprite->scaleY = (16 - sprite->counter < 8 ? 16 - sprite->counter : 8) / 8.0f;
        sprite->scaleX = (8 - sprite->counter > 1 ? 8 - sprite->counter : 1) / 8.0f;
        break;
    case 2:
        if (sprite->counter == 0)
            sprite->counter = 8;
        if (--sprite->counter == 0)
            sprite->userVar = 3;
        sprite->opacity = (8 - sprite->counter) / 8.0f;
        break;
    case 3:
        break;
    case 4:
        if (sprite->counter == 0)
            sprite->counter = 16;
        if (--sprite->counter == 0)
        {
            sprite->userVar = 0;
            sprite->opacity = 0.0f;
        }
        sprite->scaleY = 1.0f - (16 - sprite->counter < 7 ? 16 - sprite->counter : 7) / 8.0f;
        sprite->scaleX = 1.0f - (8 - sprite->counter > 0 ? 8 - sprite->counter : 0) / 8.0f;
        break;
    }
}

/* Draws the title plate and the name of the focused entry of the current screen on it. */
void sprite_menu_basic_title_disp(struct Sprite *sprite)
{
    struct MenuScreen *screen = &menu_screen_list[menuWork->currScreen];
    struct SpriteDrawRequest req;

    set_ui_widescreen_scale_mtx(0x140);
    req.bmpId = 0x1711;
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = 0.02f + sprite->depth;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = 1.6f * sprite->scaleX;
    req.scaleY = 0.8f * sprite->scaleY;
    req.rotation = 0;
    req.opacity = 1.0f;
    req.unk30 = -1;
    req.flags = 0xA;
    req.mulColor = 0xFFC0C0C0;
    req.addColor = 0;
    draw_sprite_draw_request(&req);

    if (screen->entries == NULL || fn_3_1B984(1) != 0)
    {
        reset_ui_widescreen_scale_mtx();
        return;
    }
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_alignment(4);
    textdraw_set_flags(0x80000000);
    textdraw_set_mul_color(0xC0);
    textdraw_set_font_style(3);
    textdraw_set_alpha(sprite->opacity);
    textdraw_set_scale(sprite->scaleX, sprite->scaleY);
    textdraw_set_pos(sprite->x - 1.0f, sprite->y - 2.0f);
    textdraw_print_and_fit_to_width(240.0f,
        screen->entries[(s8)rootMenu->unk0[menuWork->currScreen]].name[(s8)g_language_id]);
    reset_ui_widescreen_scale_mtx();
}

/* Removes the sprite once the current screen no longer shows the character panels. */
void fn_3_185E8(s8 *status, struct Sprite *sprite)
{
    struct MenuScreen *screen = &menu_screen_list[menuWork->currScreen];

    if (!(screen->bitflag & 0x800))
        *status = 0;
}

/* Per-player controller button icons drawn under the character panels: unpressed
 * frame, then pressed frames picked by menuWork->unk20[player]. */
s32 lbl_3_data_CEC4[4] = { 0x0F, 0x45, 0x46, 0x47 };
s32 lbl_3_data_CED4[4] = { 0x20, 0x2E, 0x2F, 0x30 };
s32 lbl_3_data_CEE4[4] = { 0x33, 0x40, 0x41, 0x41 };
s32 lbl_3_data_CEF4[4] = { 0x03, 0x57, 0x5B, 0x5C };
s32 *lbl_3_data_CF04[4] = { lbl_3_data_CEC4, lbl_3_data_CED4, lbl_3_data_CEE4, lbl_3_data_CEF4 };

/* Character panels (one per menu entry sprite 0x3C-0x3F) with the monkey's name and,
 * while the disc is idle, the player's button icon. */
void fn_3_18624(struct Sprite *sprite)
{
    char *names[4] = { "AIAI", "MEEMEE", "BABY", "GONGON" };
    struct Sprite *s;
    int i;
    s16 rot;

    set_ui_widescreen_scale_mtx(0x140);
    rot = 4096.0f * math_sin(g_font_char_var_3 * 0x300);
    for (i = 0; i < 4; i++)
    {
        s = get_sprite_with_unique_id(i + 0x3C);
        if (s == NULL)
            continue;
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_pos(s->unk58, 60.0f + s->y);
        set_ui_element_sprite_depth(0.15f);
        set_ui_element_sprite_scale(s->scaleX - 0.2f, s->scaleX - 0.2f);
        set_ui_element_sprite_alpha(s->opacity);
        g_set_ui_element_sprite_flags(0xA);
        draw_ui_element(0x1711);

        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_drop_shadow();
        textdraw_set_alignment(4);
        textdraw_set_flags(0x80000000);
        textdraw_set_mul_color(0xC0);
        textdraw_set_font_style(3);
        textdraw_set_depth(0.1f);
        textdraw_set_alpha(s->opacity);
        textdraw_set_scale(0.85f * s->scaleX, s->scaleX);
        textdraw_set_pos(s->unk58 - 2.0f, (60.0f + s->y) - 1.0f);
        textdraw_print(names[i]);

        if (disc_queue_distance(1) > 0)
        {
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_pos(s->unk58, s->y + (float)lbl_1_bss_6FB68);
            set_ui_element_sprite_depth(0.1f);
            set_ui_element_sprite_scale(s->scaleX - 0.2f, 0.6f * (s->scaleX - 0.2f));
            set_ui_element_sprite_alpha(s->opacity);
            g_set_ui_element_sprite_flags(0xA);
            set_ui_element_sprite_rot_z(rot);
            if (menuWork->unk20[i] != 0)
                draw_ui_element(lbl_3_data_CF04[i][(int)(3.0f * math_sin((menuWork->unk20[i] % 15) * 0x888))]);
            else
                draw_ui_element(lbl_3_data_CF04[i][0]);
        }
    }
    reset_ui_widescreen_scale_mtx();
}

/* Game settings box: 1 unfold, 2 blow up and fade out (then removed), 3 idle. Spins
 * faster and faster while menuWork->unk8 is set. */
void sprite_game_settings_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar != 2 && menuWork->unk4 != 0)
    {
        sprite->userVar = 2;
        sprite->counter = 14;
    }
    if (menuWork->unk4 == 0 && menuWork->unk8 != 0)
    {
        if (sprite->unk50 < 0x1000)
            sprite->unk50 *= 2;
        sprite->rotation -= sprite->unk50;
    }
    switch (sprite->userVar)
    {
    case 1:
        if (--sprite->counter == 0)
            sprite->userVar = 3;
        sprite->scaleX = (8 - sprite->counter) / 8.0f;
        sprite->scaleY = 1.4f * sprite->scaleX;
        break;
    case 2:
        if (--sprite->counter == 0)
            *status = 0;
        sprite->scaleX += 0.2f * (2.0f - sprite->scaleX);
        sprite->scaleY = sprite->scaleX;
        sprite->opacity += 0.2f * -sprite->opacity;
        break;
    case 3:
        if (sprite->counter < 100)
            sprite->counter++;
        break;
    }
}

/* Game settings box: the option list of the current screen (e.g. the party game rules) */
void sprite_game_settings_disp(struct Sprite *sprite)
{
    float x;
    float y;
    float scale;
    float lineH;
    int i;
    struct MenuScreen *screen;
    u32 color;
    u32 addColor;
    u32 glow;
    struct SpriteDrawRequest req;

    screen = &menu_screen_list[menuWork->currScreen];
    x = sprite->x;
    if ((s8)screen->entry_count == 5)
        scale = 0.85f;
    else
        scale = 1.0f;
    lineH = 30.0f * scale;
    y = sprite->y - 0.5f * (s8)screen->entry_count * lineH;

    req.bmpId = sprite->bmpId;
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = 0.02f + sprite->depth;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = sprite->scaleX;
    req.scaleY = sprite->scaleY;
    req.rotation = 0;
    req.opacity = sprite->opacity;
    req.unk30 = -1;
    req.flags = 0xA;
    req.mulColor = -1;
    req.addColor = 0;
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(req.z);
    set_ui_element_sprite_scale(req.scaleX, req.scaleY);
    set_ui_element_sprite_alpha(req.opacity);
    g_set_ui_element_sprite_flags(req.flags);
    set_ui_element_sprite_pos(req.x, req.y);
    draw_ui_box(0x1701);

    /* cursor */
    req.bmpId = 0x1700;
    req.x = x - 175.0f;
    req.y = (s8)rootMenu->unk0[menuWork->currScreen] * lineH + y;
    req.z = sprite->depth;
    req.scaleX = 0.5 * sprite->scaleX;
    req.scaleY = req.scaleX;
    req.opacity = (sprite->userVar == 3) ? 1.0f : 0.0f;
    req.mulColor = lbl_3_bss_6C[0] ? 0x808080 : 0xFFFFFF;
    req.rotation = sprite->rotation;
    draw_sprite_draw_request(&req);

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_alpha((sprite->userVar == 3) ? 1.0f : 0.0f);
    g_textdraw_set_counter(sprite->counter);
    textdraw_set_flags(0x80000000);
    for (i = 0; i <= (s8)screen->entry_count; i++)
    {
        addColor = 0;
        if (screen->entries == NULL)
            continue;

        /* underline */
        textdraw_clear_drop_shadow();
        textdraw_set_depth(0.105f);
        textdraw_set_alignment(4);
        textdraw_set_scale(16.0f + lbl_1_bss_6FB84, 0.6f * scale);
        if (i == (s8)rootMenu->unk0[menuWork->currScreen])
        {
            glow = 64.0 * fabs(math_sin(g_font_char_var_3 * 0x180));
            if (menuWork->unk4 == 0 && menuWork->unk8 != 0)
                glow = ((g_font_char_var_3 >> 2) & 1) * 0xC0;
            if (lbl_3_bss_6C[0])
                glow >>= 1;
            glow = glow | ((glow << 16) | (glow << 8));
            if (i == (s8)screen->entry_count)
                color = 0xFFC040;
            else
                color = 0xFFFF40;
            if (lbl_3_bss_6C[0])
            {
                if (i == (s8)screen->entry_count)
                    color = 0x804000;
                else
                    color = 0x808000;
            }
            textdraw_set_mul_color(color);
            textdraw_set_add_color(glow);
        }
        else
        {
            color = (i == (s8)screen->entry_count) ? 0xC08040 : 0xC0C040;
            textdraw_set_mul_color(color);
            textdraw_set_add_color(0);
        }
        textdraw_set_pos(10.0f + x + lbl_1_bss_6FB6C, 5.0f + y + i * lineH);
        textdraw_print("_");

        /* entry name */
        textdraw_set_drop_shadow();
        textdraw_set_depth(0.1f);
        textdraw_set_scale(0.8f, scale);
        textdraw_set_alignment(1);
        if (i == (s8)rootMenu->unk0[menuWork->currScreen])
        {
            glow = 64.0 * fabs(math_sin(g_font_char_var_3 * 0x180));
            if (menuWork->unk4 == 0 && menuWork->unk8 != 0)
                glow = ((g_font_char_var_3 >> 2) & 1) << 6;
            addColor = glow | ((glow << 16) | (glow << 8));
            if (lbl_3_bss_6C[0])
                glow >>= 1;
            glow = glow | ((glow << 16) | (glow << 8));
            if (i == (s8)screen->entry_count)
                color = 0xFF8000;
            else
                color = 0xFFFF00;
            if (lbl_3_bss_6C[0])
                textdraw_set_mul_color((i == (s8)screen->entry_count) ? 0x804000 : 0x808000);
            else
                textdraw_set_mul_color(color);
            textdraw_set_add_color(glow);
        }
        else
        {
            if (i == (s8)screen->entry_count)
                color = 0x804000;
            else
                color = 0x808000;
            textdraw_set_mul_color(color);
            textdraw_set_add_color(0);
        }
        textdraw_set_pos(x - 150.0f, y + i * lineH);
        textdraw_print_and_fit_to_width(210.0f, screen->entries[i].name[(s8)g_language_id]);
        if (i == (s8)screen->entry_count)
            break;

        /* value */
        textdraw_set_alignment(4);
        textdraw_set_mul_color(color);
        textdraw_set_add_color(addColor);
        textdraw_set_pos(120.0f + x, y + i * lineH);
        fn_3_198(1, i);
        if (i == (s8)rootMenu->unk0[menuWork->currScreen] && lbl_3_bss_6C[0])
        {
            /* arrows around the value being changed */
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_spacing(72.0f, 0.0f);
            textdraw_set_pos(120.0f + x, y + i * lineH);
            textdraw_print("<>");
            textdraw_set_spacing(0.0f, 0.0f);
        }
    }
}

/* Practice mode stage select frame. userVar: 1 = opening, 2 = closing, 3 = shown */
void sprite_practice_stage_select_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar != 2 && menuWork->unk4 != 0)
    {
        sprite->userVar = 2;
        sprite->counter = 14;
    }
    switch (sprite->userVar)
    {
    case 1:
        if (--sprite->counter == 0)
            sprite->userVar = 3;
        sprite->scaleX = 1.2f * ((8 - sprite->counter) / 8.0f);
        sprite->scaleY = 1.166666f * sprite->scaleX;
        break;
    case 2:
        if (--sprite->counter == 0)
            *status = 0;
        sprite->scaleX += 1.2f * (0.2f * (2.0f - sprite->scaleX));
        sprite->scaleY = 1.166666f * sprite->scaleX;
        sprite->opacity += 0.2f * -sprite->opacity;
        sprite->unk58 += 0.4f * -sprite->unk58;
        break;
    case 3:
        if (sprite->counter < 100)
            sprite->counter++;
        sprite->unk58 += 0.05f * (1.0f - sprite->unk58);
        break;
    }
}

/* Practice mode stage select: Story/Challenge mode switch, the world (story) or difficulty
 * (challenge) row and the grid of stage numbers, plus the name of the selected stage. */
void sprite_practice_stage_select_disp(struct Sprite *sprite)
{
    struct SelNgcStorySettings *settings = storySettings;
    s32 diffCount;
    s32 stageCount;
    float x;
    float y;
    float z;
    float scaleX;
    float scaleY;
    float opacity;
    s32 flags;
    float gridX;
    float gridY;
    u32 glow;
    s32 i;
    s32 savedDifficulty;
    u32 savedCourseFlags;
    s8 savedSel;
    s32 j;
    BOOL available;
    BOOL unlocked;
    s32 amp;
    s32 icon;
    u8 sel;

    g_get_some_category_count(0, &diffCount, &stageCount);
    x = sprite->x;
    y = sprite->y;
    z = 0.05f + sprite->depth;
    scaleX = sprite->scaleX;
    scaleY = sprite->scaleY;
    opacity = sprite->opacity;
    flags = 0xA;
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(z);
    set_ui_element_sprite_scale(scaleX, scaleY);
    set_ui_element_sprite_alpha(opacity);
    g_set_ui_element_sprite_flags(flags);
    set_ui_element_sprite_pos(x, y);
    draw_ui_box(0x1701);
    if (sprite->userVar != 3)
        return;

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0x90000000);
    textdraw_set_drop_shadow();
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(sprite->depth - 0.01f);
    g_set_ui_element_sprite_flags(4);
    textdraw_set_depth(0.025 + sprite->depth);
    textdraw_set_alignment(3);
    textdraw_clear_drop_shadow();

    /* background panels of the two halves (mode row / stage row); the focused one is lit */
    textdraw_set_mul_color((settings->unk17 & 1) ? 0x202000 : 0xFFFF00);
    textdraw_set_alpha((settings->unk17 & 1) ? 0.4f : 0.2f);
    textdraw_set_scale(14.5f, 1.0f);
    textdraw_set_pos(sprite->x - 90.0f, sprite->y - 53.0f);
    textdraw_print("\x81\xA1");
    textdraw_set_mul_color((settings->unk17 & 1) ? 0xFFFF00 : 0x202000);
    textdraw_set_alpha((settings->unk17 & 1) ? 0.2f : 0.4f);
    textdraw_set_scale(14.5f, 4.8f);
    textdraw_set_pos(sprite->x - 90.0f, sprite->y - 35.0f);
    textdraw_print("\x81\xA1");

    textdraw_set_alignment(0);
    textdraw_set_depth(0.1f);
    textdraw_set_alpha(1.0f);
    textdraw_set_drop_shadow();
    {
        char *storyStr[6] = { "Story ", "", "", "", "", "\x83\x58\x83\x67\x81\x5B\x83\x8A\x81\x5B " };
        char *challengeStr[6] = { "Challenge ", "", "", "", "", "\x83\x60\x83\x83\x83\x8C\x83\x93\x83\x57 " };
        char *modeStr[6] = { "Mode", "", "", "", "", "\x83\x82\x81\x5B\x83\x68" };
        char *hintStr[6] = {
            " ... \x83\xB4 to change mode.", "", "", "", "",
            " ... \x83\xB4 \x82\xC5\x83\x82\x81\x5B\x83\x68\x82\xF0\x82\xD6\x82\xF1\x82\xB1\x82\xA4\x82\xB5\x82\xDC\x82\xB7\x81\x42",
        };

        /* "Story / Challenge Mode ... (button) to change mode." */
        glow = 32.0 * fabs(math_sin(g_font_char_var_3 * 0x180));
        glow = glow | ((glow << 16) | (glow << 8));
        textdraw_set_scale(0.6f, 0.6f);
        textdraw_set_pos(sprite->x - 230.0f, sprite->y - 70.0f);
        textdraw_set_mul_color(((s8)settings->unk0 == 1) ? 0xFF8000 : 0x402000);
        textdraw_set_add_color(((s8)settings->unk0 == 1) ? glow : 0);
        textdraw_print(storyStr[(s8)g_language_id]);
        textdraw_set_mul_color(((s8)settings->unk0 == 0) ? 0xFF8000 : 0x402000);
        textdraw_set_add_color(((s8)settings->unk0 == 0) ? glow : 0);
        textdraw_print(challengeStr[(s8)g_language_id]);
        textdraw_set_mul_color(0xFF8000);
        textdraw_set_add_color(0);
        textdraw_print(modeStr[(s8)g_language_id]);
        textdraw_set_mul_color(0xFFFF00);
        textdraw_print_and_fit_to_width(240.0f, hintStr[(s8)g_language_id]);
    }
    {
        char *worldStr[6] = { "World ", "", "", "", "", "\x83\x8F\x81\x5B\x83\x8B\x83\x68 " };
        /* difficulty icons: beginner, advanced, expert, master */
        s32 diffIcons[4] = { 0x514, 0x513, 0x515, 0x517 };

        if (settings->unk17 & 1)
            amp = ((s8)settings->unk0 == 1) ? 0x20 : 0x10;
        else
            amp = 0x80;
        glow = fabs(math_sin(g_font_char_var_3 * 0x180)) * amp;
        glow = glow | ((glow << 16) | (glow << 8));
        if ((s8)settings->unk0 == 1)
        {
            /* "World 1 2 3 ... 10" */
            textdraw_set_scale(0.64f, 0.7f);
            textdraw_set_pos(sprite->x - 230.0f, sprite->y - 50.0f);
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_add_color(0);
            textdraw_print(worldStr[(s8)g_language_id]);
            textdraw_set_spacing(0.0f, 0.0f);
            for (i = 0; i < 10; i++)
            {
                unlocked = FALSE;
                for (j = 0; j < 10; j++)
                {
                    if (g_is_storymode_stage_unlocked(i, j))
                    {
                        unlocked = TRUE;
                        break;
                    }
                }
                if (i == (s8)settings->unk1[1])
                {
                    textdraw_set_mul_color(unlocked ? 0xFF8000 : 0x202000);
                    textdraw_set_add_color(glow);
                }
                else
                {
                    textdraw_set_mul_color(unlocked ? ((settings->unk17 & 1) ? 0x808000 : 0xFFFF00) : 0x202000);
                    textdraw_set_add_color(0);
                }
                textdraw_printf("%d ", i + 1);
            }
        }
        else
        {
            /* difficulty icons; "EX" marks the extra courses */
            savedDifficulty = modeCtrl.difficulty;
            savedCourseFlags = modeCtrl.courseFlags;
            savedSel = settings->unk1[0];
            set_ui_element_sprite_scale(0.65f, 0.65f);
            for (i = 0; i < diffCount; i++)
            {
                settings->unk1[0] = i;
                g_get_some_challenge_mode_difficulty_info(0);
                available = g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, 0, modeCtrl.courseFlags);
                settings->unk1[0] = savedSel;
                modeCtrl.difficulty = savedDifficulty;
                modeCtrl.courseFlags = savedCourseFlags;
                if (i > 5)
                    icon = 3;
                else
                    icon = i % 3;
                if (i == (s8)settings->unk1[0])
                {
                    set_ui_element_sprite_mult_color_from_u32(available ? 0xFFFFFF : 0x202020);
                    set_ui_element_sprite_add_color_from_u32(glow);
                }
                else
                {
                    set_ui_element_sprite_mult_color_from_u32(
                        available ? ((settings->unk17 & 1) ? 0x808080 : 0xFFFFFF) : 0x202020);
                    set_ui_element_sprite_add_color_from_u32(0);
                }
                set_ui_element_sprite_pos((sprite->x - 230.0f) + i * 24, sprite->y - 50.0f);
                draw_ui_element(diffIcons[icon]);
                if (i >= 3 && i != 6)
                {
                    textdraw_set_scale(0.4f, 0.4f);
                    textdraw_set_depth(sprite->depth - 0.02f);
                    textdraw_set_mul_color(available ? ((settings->unk17 & 1) ? 0x808000 : 0xFFFF00) : 0x202000);
                    textdraw_set_add_color((i == (s8)settings->unk1[0]) ? glow : 0);
                    textdraw_set_pos(10.0f + (sprite->x - 230.0f) + i * 24, 10.0f + (sprite->y - 50.0f));
                    textdraw_print("EX");
                }
            }
        }
    }

    /* stage number grid, 10 per row */
    sel = settings->unk1[(s8)settings->unk0];
    if ((s8)settings->unk0 == 1)
        stageCount = 10;
    if (settings->unk17 & 1)
        amp = 0x80;
    else
        amp = 0x20;
    glow = amp * fabs(math_sin(g_font_char_var_3 * 0x180));
    if (menuWork->unk4 == 0 && menuWork->unk8 != 0)
        glow = ((g_font_char_var_3 >> 2) & 1) << 7;
    glow = glow | ((glow << 16) | (glow << 8));
    gridX = sprite->x - 220.0f;
    gridY = (15.0f + sprite->y) - 0.5f * ((stageCount / 10 - 1) * ((stageCount == 50) ? 18.0f : 22.0f));
    textdraw_set_scale(0.7f, 0.6f);
    textdraw_set_alignment(3);
    textdraw_set_spacing(0.0f, 0.0f);
    if ((s8)settings->unk0 == 1)
    {
        for (i = 0; i < 10; i++)
        {
            unlocked = g_is_storymode_stage_unlocked((s8)sel, i);
            if (i == (s8)settings->unk3[(s8)settings->unk0][(s8)sel])
            {
                if (settings->unk17 & 1)
                {
                    textdraw_set_mul_color(unlocked ? 0xFF8000 : 0x202000);
                    textdraw_set_add_color(glow);
                }
                else
                {
                    textdraw_set_mul_color(unlocked ? (!(settings->unk17 & 1) ? 0xC0C000 : 0xFFFF00) : 0x202000);
                    textdraw_set_add_color(0);
                }
            }
            else
            {
                textdraw_set_mul_color(unlocked ? ((settings->unk17 & 1) ? 0xFFFF00 : 0x808000) : 0x202000);
                textdraw_set_add_color(0);
            }
            textdraw_set_pos(gridX + 29.0f * (i % 10), gridY + (i / 10) * ((stageCount == 50) ? 18.0f : 22.0f));
            textdraw_printf("%d", i + 1);
        }
    }
    else
    {
        for (i = 0; i < stageCount; i++)
        {
            unlocked = g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, i + 1, modeCtrl.courseFlags);
            if (i == (s8)settings->unk3[(s8)settings->unk0][(s8)sel])
            {
                if (settings->unk17 & 1)
                {
                    textdraw_set_mul_color(unlocked ? 0xFF8000 : 0x202000);
                    textdraw_set_add_color(glow);
                }
                else
                {
                    textdraw_set_mul_color(unlocked ? (!(settings->unk17 & 1) ? 0xC0C000 : 0xFFFF00) : 0x202000);
                    textdraw_set_add_color(0);
                }
            }
            else
            {
                textdraw_set_mul_color(unlocked ? ((settings->unk17 & 1) ? 0xFFFF00 : 0x808000) : 0x202000);
                textdraw_set_add_color(0);
            }
            textdraw_set_pos(gridX + 29.0f * (i % 10), gridY + (i / 10) * ((stageCount == 50) ? 18.0f : 22.0f));
            textdraw_printf("%d", i + 1);
        }
    }

    /* name of the selected stage */
    if (settings->unk17 & 1)
    {
        if ((s8)settings->unk0 == 1)
            unlocked = g_is_storymode_stage_unlocked(
                (s8)settings->unk1[(s8)settings->unk0],
                (s8)settings->unk3[(s8)settings->unk0][(s8)settings->unk1[(s8)settings->unk0]]);
        else
            unlocked = g_smth_with_cm_entries_in_main_menu(
                modeCtrl.difficulty,
                (s8)settings->unk3[(s8)settings->unk0][(s8)settings->unk1[(s8)settings->unk0]] + 1,
                modeCtrl.courseFlags);
        textdraw_set_scale(7.4f, 0.70000005f);
        textdraw_set_mul_color(0);
        textdraw_set_add_color(0);
        textdraw_set_alpha(unlocked ? 0.5f : 0.0f);
        textdraw_set_depth(sprite->depth - 0.06f);
        textdraw_clear_drop_shadow();
        textdraw_set_alignment(4);
        textdraw_set_pos(145.0f + sprite->x, 60.0f + sprite->y);
        textdraw_print("\x81\xA1");
        textdraw_set_scale(0.5f, 0.6f);
        textdraw_set_mul_color(0xFFFF00);
        textdraw_set_add_color(0);
        textdraw_set_depth(sprite->depth - 0.07f);
        textdraw_set_spacing(0.0f, 0.0f);
        textdraw_set_alpha(unlocked ? 0.8f : 0.0f);
        textdraw_set_border();
        textdraw_set_pos(145.0f + sprite->x, 60.0f + sprite->y);
        textdraw_printf_and_fit_to_width(156.0f, "\x81\x60 %s \x81\x60", lbl_3_bss_78);
    }
}

/* Fight (party game) stage select frame: like the practice one, but also closes on screen 0xF */
void sprite_fight_stage_select_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar != 2 && menuWork->unk4 != 0)
    {
        sprite->userVar = 2;
        sprite->counter = 14;
    }
    else if (sprite->userVar != 2 && menuWork->unk4 == 0 && menuWork->unk8 == 0xF)
    {
        sprite->userVar = 2;
        sprite->counter = 14;
    }
    switch (sprite->userVar)
    {
    case 1:
        if (--sprite->counter == 0)
            sprite->userVar = 3;
        sprite->scaleX = (8 - sprite->counter) / 8.0f;
        sprite->scaleY = 1.4f * sprite->scaleX;
        break;
    case 2:
        if (--sprite->counter == 0)
            *status = 0;
        sprite->scaleX += 0.2f * (2.0f - sprite->scaleX);
        sprite->scaleY = sprite->scaleX;
        sprite->opacity += 0.2f * -sprite->opacity;
        sprite->unk58 += 0.4f * -sprite->unk58;
        break;
    case 3:
        if (sprite->counter < 100)
            sprite->counter++;
        sprite->unk58 += 0.05f * (1.0f - sprite->unk58);
        break;
    }
}

/* Fight stage select: help text above the stage list, and arrows on screen 0x18 */
void sprite_fight_stage_select_disp(struct Sprite *sprite)
{
    struct SelNgcSettings16DC *settings = fightSettings;
    BOOL multiRound;
    float x;
    float y;
    float z;
    float scaleX;
    float scaleY;
    float opacity;
    s32 flags;

    multiRound = (s8)settings->unk4 > 1;
    x = sprite->x;
    y = sprite->y;
    z = 0.05f + sprite->depth;
    scaleX = sprite->scaleX;
    scaleY = sprite->scaleY;
    opacity = sprite->opacity;
    flags = 0xA;
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(z);
    set_ui_element_sprite_scale(scaleX, scaleY);
    set_ui_element_sprite_alpha(opacity);
    g_set_ui_element_sprite_flags(flags);
    set_ui_element_sprite_pos(x, y);
    draw_ui_box(0x1701);

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_alpha((sprite->userVar == 3) ? 1.0f : 0.0f);
    g_textdraw_set_counter(sprite->counter);
    textdraw_set_flags(0x80000000);
    textdraw_set_alignment(multiRound ? 1 : 4);
    textdraw_set_scale(0.8f, 0.8f);
    textdraw_set_spacing(0.0f, 4.0f);
    textdraw_set_depth(sprite->depth - 0.075f);
    textdraw_set_mul_color(0xFFFF00);
    {
        /* [multiRound][language] */
        char *helpStr[2][6] = {
            {
                "Select a stage for\nthe battle here.",
                "", "", "", "",
                "\x82\xBD\x82\xBD\x82\xA9\x82\xA4\x83\x58\x83\x65\x81\x5B\x83\x57\x82\xF0\n"
                "\x82\x50\x82\xC2 \x82\xA6\x82\xE7\x82\xD1\x82\xDC\x82\xB7\x81\x42",
            },
            {
                "When you finish 3 stages, you\nwill go back to the 1st stage.",
                "", "", "", "",
                "\x82\x52\x83\x58\x83\x65\x81\x5B\x83\x57\x82\xA8\x82\xED\x82\xE9\x82\xC6\x81\x41\n"
                "\x82\x50\x82\xC2\x82\xDF\x82\xC9 \x82\xE0\x82\xC7\x82\xE8\x82\xDC\x82\xB7\x81\x42",
            },
        };

        textdraw_set_pos(sprite->x + (multiRound ? -190 : 0), 46.0f + sprite->y);
        textdraw_print_and_fit_to_width(multiRound ? 200 : 420, helpStr[multiRound ? 1 : 0][(s8)g_language_id]);
    }
    if ((s8)menuWork->currScreen == 0x18)
    {
        textdraw_set_alignment(4);
        textdraw_set_scale(0.8f, 0.4f);
        textdraw_set_pos(75.0f + sprite->x, 28.0f + sprite->y);
        textdraw_print("\x81\xA8");
        textdraw_set_pos(133.0f + sprite->x, 28.0f + sprite->y);
        textdraw_print("\x81\xA8");
    }
}

/* ---- boat course select box (MENUSCREEN 0x2C) ---- */

void fn_3_1A9F0(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar != 2 && menuWork->unk4 != 0)
    {
        sprite->userVar = 2;
        sprite->counter = 14;
    }
    else if (menuWork->currScreen == 0x2C && sprite->userVar != 2 && menuWork->unk4 == 0
          && menuWork->unk8 == 15)
    {
        sprite->userVar = 2;
        sprite->counter = 14;
    }
    switch (sprite->userVar)
    {
    case 1:
        if (--sprite->counter == 0)
            sprite->userVar = 3;
        sprite->scaleX = (8 - sprite->counter) / 8.0f;
        sprite->scaleY = 1.4f * sprite->scaleX;
        break;
    case 2:
        if (--sprite->counter == 0)
            *status = 0;
        sprite->scaleX += 0.2f * (2.0f - sprite->scaleX);
        sprite->scaleY = sprite->scaleX;
        sprite->opacity += 0.2f * -sprite->opacity;
        sprite->unk58 += 0.4f * -sprite->unk58;
        break;
    case 3:
        if (sprite->counter < 100)
            sprite->counter++;
        sprite->unk58 += 0.05f * (1.0f - sprite->unk58);
        break;
    }
}

void fn_3_1ABE0(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    int i;

    req.bmpId = sprite->bmpId;
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = 0.05f + sprite->depth;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = sprite->scaleX;
    req.scaleY = sprite->scaleY;
    req.rotation = 0;
    req.opacity = sprite->opacity;
    req.unk30 = -1;
    req.flags = 0xA;
    req.mulColor = -1;
    req.addColor = 0;
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(req.z);
    set_ui_element_sprite_scale(req.scaleX, req.scaleY);
    set_ui_element_sprite_alpha(req.opacity);
    g_set_ui_element_sprite_flags(req.flags);
    set_ui_element_sprite_pos(req.x, req.y);
    draw_ui_box(0x1701);

    /* the three course icons */
    for (i = 0; i < 3; i++)
    {
        float scales[3] = { 0.675f, 0.675f, 0.675f };
        s32 xOffsets[3] = { -131, -120, -60 };
        s32 yOffsets[3] = { 31, 25, -7 };
        s32 selectedColors[3] = { 0x80FF80, 0x8080FF, 0xFF8080 };
        s32 colors[3] = { 0x204020, 0x202040, 0x402020 };

        req.bmpId = lbl_3_data_CC08[i];
        req.x = sprite->x + xOffsets[i];
        req.y = sprite->y + yOffsets[i];
        req.scaleX = sprite->scaleX * scales[i];
        req.scaleY = req.scaleX;
        req.rotation = 0xD400;
        req.opacity = (sprite->userVar != 2) ? sprite->unk58 : 0.0f;
        req.z = sprite->depth + 0.01 * i;
        if (i == (s8)rootMenu->unk0[menuWork->currScreen])
            req.z -= 0.05f;
        req.mulColor = (i == (s8)rootMenu->unk0[menuWork->currScreen]) ? selectedColors[i] : colors[i];
        if (i == (s8)rootMenu->unk0[menuWork->currScreen])
        {
            u32 c = 64.0 * fabs(math_sin(g_font_char_var_3 * 0x180));

            if (menuWork->unk4 == 0 && menuWork->unk8 != 0)
                c = ((g_font_char_var_3 >> 2) & 1) << 6;
            c = (c << 16) | (c << 8) | c;
            req.addColor = c;
        }
        else
            req.addColor = 0;
        draw_sprite_draw_request(&req);
    }

    /* description of the selected course */
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_alpha((sprite->userVar == 3) ? 1.0f : 0.0f);
    g_textdraw_set_counter(sprite->counter);
    textdraw_set_flags(0x80000000);
    textdraw_set_alignment(6);
    textdraw_set_scale(0.8f, 0.8f);
    textdraw_set_spacing(0.0f, 4.0f);
    textdraw_set_depth(sprite->depth - 0.075f);
    textdraw_set_mul_color(0xFFFF00);
    {
        /* [course][language] */
        char *descriptions[3][6] = {
            {
                "The stream is mild\nin this course.\nEasy to paddle.",
                "", "", "", "",
                "\x82\xDD\x82\xB8\x82\xCC \x82\xC8\x82\xAA\x82\xEA\x82\xAA\n\x82\xE6\x82\xED\x82\xAD\x82\xC4"
                "\x81\x41\x82\xB1\x82\xAC\x82\xE2\x82\xB7\x82\xA2\n\x83\x52\x81\x5B\x83\x58\x82\xC5\x82\xB7\x81\x42",
            },
            {
                "Race under the wooden arch.\nThe stream is a\nlittle rough.",
                "", "", "", "",
                "\x82\xDD\x82\xB8\x82\xCC \x82\xC8\x82\xAA\x82\xEA\x82\xAA \x82\xE2\x82\xE2 \x82\xC2\x82\xE6\x82\xAD"
                "\x81\x41\n\x82\xB6\x82\xE3\x82\xE0\x82\xAD\x82\xCC\x83\x41\x81\x5B\x83\x60\x82\xF0\n\x82\xAD\x82\xAE"
                "\x82\xE8\x82\xC8\x82\xAA\x82\xE7\x81\x41\n\x83\x8C\x81\x5B\x83\x58\x82\xF0\x82\xB5\x82\xDC\x82\xB7\x81\x42",
            },
            {
                "This is a straight course with\na swift current. The\ngoal appears right\nafter the tunnel.",
                "", "", "", "",
                "\x82\xB0\x82\xAB\x82\xE8\x82\xE3\x82\xA4\x82\xAD\x82\xBE\x82\xE8 \x82\xCC\x82\xE6\x82\xA4\x82\xC8\n"
                "\x82\x50\x82\xDB\x82\xF1\x82\xDD\x82\xBF\x82\xCC\x83\x52\x81\x5B\x83\x58\x82\xC5\x81\x41\n\x82\xC7"
                "\x82\xA4\x82\xAD\x82\xC2\x82\xF0 \x82\xAD\x82\xAE\x82\xEA\x82\xCE\x81\x41\n\x83\x53\x81\x5B\x83\x8B"
                "\x82\xCD \x82\xB7\x82\xAE\x82\xBB\x82\xB1\x82\xC5\x82\xB7\x81\x42",
            },
        };

        textdraw_set_pos(160.0f + sprite->x, sprite->y - 64.0f);
        textdraw_print(descriptions[(s8)rootMenu->unk0[menuWork->currScreen]][(s8)g_language_id]);
    }
}

/* ---- button prompts at the bottom of the menu (sprite "button") ----
 * Three rows: "<glyph>:<label>". The glyphs are characters of the menu font (font 3,
 * SJIS 0x83xx codes of the pad button icons; see main_loop/name_entry2.c, where
 * /cd839f/ is the A button). THIS is where the GameCube button glyphs are chosen: the
 * SPRITE_BUTTON_GLYPH_* literals below, used by sprite_button_disp. */
#define SPRITE_BUTTON_GLYPH_LESSON "\x83\xB4" /* row 0 (Lesson/Tutorial): Y button icon (assumed) */
#define SPRITE_BUTTON_GLYPH_SELECT "\x83\x9F" /* row 1 (Select/OK): A button icon */
#define SPRITE_BUTTON_GLYPH_CANCEL "\x83\xA0" /* row 2 (Cancel): B button icon */

/* Fades the prompt colours in/out. Colours: mulR/G/B (rows 0-1 label) and
 * userVar/unk50/unk54 (row 2 label); opacity: row 0 (only shown on some screens). */
#ifndef __MWERKS__
const char *pc_quit_short_label(void); /* pc_port pc_quit.c */
void pc_menu_quit_draw(int where);
#endif

void sprite_button_tick(s8 *status, struct Sprite *sprite)
{
    struct MenuScreen *nextScreen = &menu_screen_list[menuWork->nextScreen];
    int showLesson = FALSE;
    int show;
    int leaving;

    if (menuWork->currScreen == 7 || menuWork->currScreen == 1)
        showLesson = TRUE;
#ifndef __MWERKS__
    /* PC port: "X:Quit" (Quit to Desktop, sel_ngc.c) in this row on Mode Select */
    if (menuWork->currScreen == MENUSCREEN_MODE_SELECT)
        showLesson = TRUE;
#endif
    show = showLesson;
    leaving = menuWork->unk8 != 0;
    if (leaving)
    {
        sprite->mulR = sprite->mulR + 0.5f * (0x80 - sprite->mulR);
        sprite->mulG = sprite->mulG + 0.5f * (0x80 - sprite->mulG);
        sprite->mulB = sprite->mulB + 0.5f * -sprite->mulB;
        if ((nextScreen->bitflag & 0x100) && menuWork->unk8 > 60)
        {
            sprite->userVar = sprite->userVar + 0.5f * (0xFF - sprite->userVar);
            sprite->unk50 = sprite->unk50 + 0.5f * (0xFF - sprite->unk50);
            sprite->unk54 = sprite->unk54 + 0.5f * -sprite->unk54;
        }
        else
        {
            sprite->userVar = sprite->userVar + 0.5f * (0x80 - sprite->userVar);
            sprite->unk50 = sprite->unk50 + 0.5f * (0x80 - sprite->unk50);
            sprite->unk54 = sprite->unk54 + 0.5f * -sprite->unk54;
        }
    }
    else
    {
        sprite->mulR = sprite->mulR + 0.5f * (0xFF - sprite->mulR);
        sprite->mulG = sprite->mulG + 0.5f * (0xFF - sprite->mulG);
        sprite->mulB = sprite->mulB + 0.5f * -sprite->mulB;
        sprite->userVar = sprite->userVar + 0.5f * (0xFF - sprite->userVar);
        sprite->unk50 = sprite->unk50 + 0.5f * (0xFF - sprite->unk50);
        sprite->unk54 = sprite->unk54 + 0.5f * -sprite->unk54;
    }
    if ((dip_switches & 0x10) && (s8)rootMenu->unk0[MENUSCREEN_PARTY_GAME_SELECT] >= 6)
        show = FALSE;
    if (show)
        sprite->opacity += 0.25f * (1.0f - sprite->opacity);
    else
        sprite->opacity += 0.25f * -sprite->opacity;
}

void sprite_button_disp(struct Sprite *sprite)
{
    char *glyphs[3] = { SPRITE_BUTTON_GLYPH_LESSON, SPRITE_BUTTON_GLYPH_SELECT, SPRITE_BUTTON_GLYPH_CANCEL };
    /* [row][language]: English, German, French, Spanish, Italian, Japanese */
    char *labels[3][6] = {
        { "Lesson", "Tutorial", "Le\xE7on", "Tut.", "Lezione", "\x82\xBB\x82\xA4\x82\xB3\x82\xB9\x82\xC2\x82\xDF\x82\xA2" },
        { "Select", "W\xE4hlen", "OK", "Elegir", "Scegli", "\x82\xAF\x82\xC1\x82\xC4\x82\xA2" },
        { "Cancel", "Abbrechen", "Annul.", "Cancelar", "Annulla", "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B" },
    };
    int i;

#ifndef __MWERKS__
    if (menuWork->currScreen == MENUSCREEN_MODE_SELECT)
        labels[0][(s8)g_language_id] = (char *)pc_quit_short_label();
#endif
    if (lbl_3_bss_194 == 0)
    {
        set_ui_widescreen_scale_mtx(320);
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        textdraw_set_scale(0.9f, 0.9f);
        textdraw_set_border();
        textdraw_set_spacing(0.0f, 4.0f);
        textdraw_set_pos(sprite->x, sprite->y);
        for (i = 0; i < 3; i++)
        {
            if (i == 0)
                textdraw_set_alpha(sprite->opacity);
            else
                textdraw_set_alpha(1.0f);
            textdraw_set_depth(0.1f);
            if (i == 2)
                textdraw_set_mul_color((sprite->userVar << 16) | (sprite->userVar << 8) | sprite->userVar);
            else
                textdraw_set_mul_color((sprite->mulR << 16) | (sprite->mulR << 8) | sprite->mulR);
            textdraw_print(glyphs[i]);
            textdraw_set_depth(0.4f);
            if (i == 2)
                textdraw_set_mul_color((sprite->userVar << 16) | (sprite->unk50 << 8) | sprite->unk54);
            else
                textdraw_set_mul_color((sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB);
            textdraw_print(":");
            textdraw_print_and_fit_to_width(48.0f, labels[i][(s8)g_language_id]);
            textdraw_print("\n");
        }
        reset_ui_widescreen_scale_mtx();
    }
#ifndef __MWERKS__
    pc_menu_quit_draw(2 /* PCMQ_MODE_SELECT */);
#endif
}

/* ---- menu entry sprites ---- */

/* Character select screens: 1: is one of them the current screen (or on the screen
 * stack), 2: is one of them the next screen, otherwise: is one on the screen stack. */
int fn_3_1B984(int mode)
{
    if (mode == 1)
    {
        return menuWork->currScreen == MENUSCREEN_CHARACTER_SELECT_1
            || menuWork->currScreen == 6
            || menuWork->currScreen == 0x3E
            || menuWork->currScreen == 0x43
            || g_is_screen_in_screen_stack(MENUSCREEN_CHARACTER_SELECT_1)
            || g_is_screen_in_screen_stack(6)
            || g_is_screen_in_screen_stack(0x3E)
            || g_is_screen_in_screen_stack(0x43);
    }
    if (mode == 2)
    {
        return menuWork->nextScreen == MENUSCREEN_CHARACTER_SELECT_1 || menuWork->nextScreen == 6
            || menuWork->nextScreen == 0x3E || menuWork->nextScreen == 0x43;
    }
    return g_is_screen_in_screen_stack(MENUSCREEN_CHARACTER_SELECT_1) || g_is_screen_in_screen_stack(6)
        || g_is_screen_in_screen_stack(0x3E) || g_is_screen_in_screen_stack(0x43);
}

/* Starts the appear animation of the current screen's entry sprites (tags 0x3C+). */
void fn_3_1BBA0(void)
{
    struct MenuScreen *screen = &menu_screen_list[menuWork->currScreen];
    int i;
    struct Sprite *sprite;

    for (i = 0; i < 12; i++)
    {
        sprite = get_sprite_with_unique_id(i + 0x3C);
        if (sprite != NULL)
        {
            sprite->userVar = 0;
            sprite->scaleX = 0.0f;
            sprite->scaleY = 0.0f;
        }
    }
    if ((!fn_3_1B984(1) || (screen->bitflag & 0x400) || (screen->bitflag & 0x800)
         || (screen->bitflag & 0x2000) || (screen->bitflag & 0x4000) || (screen->bitflag & 0x8000)
         || (screen->bitflag & 0x40000))
     && !g_is_screen_in_screen_stack(MENUSCREEN_OPTIONS))
    {
        g_something_with_menus(0);
        for (i = 0; i <= (s8)screen->entry_count; i++)
        {
            sprite = get_sprite_with_unique_id(i + 0x3C);
            if (sprite != NULL)
            {
                sprite->flags &= ~0x40000;
                if (screen->bitflag & 0x400)
                {
                    sprite->userVar = 5;
                    sprite->counter = i + 16;
                }
                else if (screen->bitflag & 0x800)
                {
                    sprite->userVar = 10;
                    sprite->counter = 8;
                    sprite->flags |= 0x40000;
                }
                else if (screen->bitflag & 0x2000)
                {
                    sprite->userVar = 20;
                    sprite->counter = 8;
                }
                else if ((screen->bitflag & 0x4000) || (screen->bitflag & 0x8000) || (screen->bitflag & 0x40000))
                {
                    sprite->userVar = 30;
                    sprite->counter = i + 8;
                }
                else if (menuWork->currScreen == MENUSCREEN_OPTIONS)
                {
                    sprite->userVar = 40;
                    sprite->counter = i + 8;
                }
                else
                {
                    sprite->userVar = 1;
                    sprite->counter = i + 8;
                }
                sprite->unk58 = sprite->x;
                sprite->unk5C = sprite->y;
                sprite->addR = 0;
                sprite->addG = 0;
                sprite->addB = 0;
                sprite->depth = 0.1f;
                sprite->rotation = 0;
                sprite->opacity = 1.0f;
                sprite->unk50 = 0;
                sprite->unk54 = 0;
                sprite->unk54 = (menuWork->currScreen << 8) | (sprite->tag - 0x3C);
            }
        }
        if (screen->bitflag & 0x800)
        {
            if (get_sprite_with_unique_id(0x24) != NULL)
                destroy_sprite_with_unique_id(0x24);
            sprite = create_sprite();
            if (sprite != NULL)
            {
                sprite->tag = 0x24;
                sprite->mainFunc = fn_3_185E8;
                sprite->drawFunc = fn_3_18624;
                sprintf(sprite->text, lbl_3_data_C9C0);
            }
        }
    }
}

void g_blink_selected_menu_item(void)
{
    struct MenuScreen *screen = &menu_screen_list[menuWork->currScreen];
    int i;
    struct Sprite *sprite;

    if (!fn_3_1B984(0) || (screen->bitflag & 0x400) || (screen->bitflag & 0x800) || (screen->bitflag & 0x2000)
     || (screen->bitflag & 0x4000) || (screen->bitflag & 0x8000) || (screen->bitflag & 0x40000))
    {
        for (i = 0; i <= (s8)screen->entry_count; i++)
        {
            sprite = get_sprite_with_unique_id(i + 0x3C);
            if (sprite == NULL)
                continue;
            if ((s8)rootMenu->unk0[menuWork->currScreen] == sprite->tag - 0x3C)
            {
                if (screen->bitflag & 0x400)
                    sprite->userVar = 6;
                else if (screen->bitflag & 0x800)
                    sprite->userVar = 12;
                else if (screen->bitflag & 0x2000)
                    sprite->userVar = 22;
                else if ((screen->bitflag & 0x4000) || (screen->bitflag & 0x8000) || (screen->bitflag & 0x40000))
                    sprite->userVar = 32;
                else if (menuWork->currScreen == MENUSCREEN_OPTIONS)
                    sprite->userVar = 42;
                else
                    sprite->userVar = 3;
            }
            else
            {
                if (screen->bitflag & 0x400)
                    sprite->userVar = 7;
                else if (screen->bitflag & 0x800)
                    sprite->userVar = 12;
                else if (screen->bitflag & 0x2000)
                    sprite->userVar = 22;
                else if ((screen->bitflag & 0x4000) || (screen->bitflag & 0x8000) || (screen->bitflag & 0x40000))
                    sprite->userVar = 33;
                else
                    sprite->userVar = 4;
            }
        }
    }
}

void g_something_with_menus5(void)
{
    struct MenuScreen *screen = &menu_screen_list[menuWork->currScreen];
    int i;
    struct Sprite *sprite;

    if ((!fn_3_1B984(0) || (screen->bitflag & 0x400) || (screen->bitflag & 0x800) || (screen->bitflag & 0x2000)
         || (screen->bitflag & 0x4000) || (screen->bitflag & 0x8000) || (screen->bitflag & 0x40000))
     && !g_is_screen_in_screen_stack(MENUSCREEN_OPTIONS))
    {
        if (lbl_3_bss_190 != 0 && !(screen->bitflag & 0x400) && !(screen->bitflag & 0x4000)
         && !(screen->bitflag & 0x8000) && !(screen->bitflag & 0x40000))
        {
            sprite = get_sprite_with_unique_id(lbl_3_bss_190 + 0x48);
            if (sprite != NULL)
            {
                sprite->userVar = 2;
                lbl_3_bss_190--;
            }
        }
        if (!fn_3_1B984(0) || (screen->bitflag & 0x400) || (screen->bitflag & 0x800) || (screen->bitflag & 0x2000)
         || (screen->bitflag & 0x4000) || (screen->bitflag & 0x8000) || (screen->bitflag & 0x40000))
        {
            for (i = 0; i <= 11; i++)
            {
                sprite = get_sprite_with_unique_id(i + 0x3C);
                if (sprite != NULL)
                    sprite->userVar = 4;
            }
        }
    }
}

/* Rebuilds the stacked entry sprites for every screen on the menu stack. */
void fn_3_1C514(void)
{
    int currScreen = menuWork->currScreen;
    int stackPtr = sel_menu_info.menuStackPtr;
    int i;
    struct Sprite *sprite;
    struct Sprite *copy;
    struct MenuScreen *screen;

    for (i = 0; i < stackPtr; i++)
    {
        menuWork->currScreen = sel_menu_info.menuStack[i];
        sel_menu_info.menuStackPtr = i + 1;
        if (menu_screen_list[menuWork->currScreen].bitflag & 0x200)
            continue;
        if (fn_3_1B984(1))
            continue;
        if (g_is_screen_in_screen_stack(MENUSCREEN_OPTIONS)
         || (menuWork->currScreen == MENUSCREEN_MODE_SELECT && (s8)rootMenu->unk0[menuWork->currScreen] == 2))
            continue;
        fn_3_1BBA0();
        sprite = get_sprite_with_unique_id((s8)rootMenu->unk0[menuWork->currScreen] + 0x3C);
        if (sprite == NULL)
            continue;
        sprite->unk58 = sprite->x;
        sprite->unk5C = sprite->y;
        sprite->addR = 0;
        sprite->addG = 0;
        sprite->addB = 0;
        sprite->depth = 0.1f;
        sprite->rotation = 0;
        sprite->opacity = 1.0f;
        sprite->unk50 = 0;
        sprite->unk54 = 0;
        sprite->unk54 = (menuWork->currScreen << 8) | (sprite->tag - 0x3C);
        screen = &menu_screen_list[menuWork->currScreen];
        if (menuWork->currScreen == MENUSCREEN_OPTIONS)
            sprite->shadowAlpha = 1.0f;
        else if (screen->bitflag & 0x10)
            sprite->shadowAlpha = 0.6f;
        else if (screen->bitflag & 0x4000)
            sprite->shadowAlpha = 0.8f;
        else if (screen->bitflag & 0x8000)
            sprite->shadowAlpha = 1.2f;
        else
            sprite->shadowAlpha = 1.0f;
        if (menuWork->currScreen == MENUSCREEN_PARTY_GAME_SELECT || menuWork->currScreen == 0x16)
            sprite->shadowAlpha *= 1.2f;
        sprite->depth = 0.08f;
        copy = fn_3_17E5C(sprite);
        if (copy != NULL)
            copy->userVar = 3;
    }
    menuWork->currScreen = currScreen;
    sel_menu_info.menuStackPtr = stackPtr;
}

void fn_3_1CA00(void)
{
    struct Sprite *sprite = get_sprite_with_unique_id(0x48);

    if (sprite != NULL && sprite->userVar == 0)
        sprite->userVar = 1;
}

void g_set_some_option_menu_sprite_param(void)
{
    struct Sprite *sprite = get_sprite_with_unique_id(0x48);

    if (sprite != NULL && sprite->userVar != 0)
        sprite->userVar = 4;
}
