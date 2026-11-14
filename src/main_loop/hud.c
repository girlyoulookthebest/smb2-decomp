/* Banner and menu sprites: pause menu, title screen, stage intro, GO, GOAL, continue, game over. */
#include "types.h"
#include "ball.h"
#include "bitmap.h"
#include "dip.h"
#include "libc.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "sprite.h"
#include "gx.h"
#include "input.h"
#include "textdraw.h"

#ifndef __MWERKS__
int pc_text_is_lead(unsigned char c); /* PC: language-aware SJIS lead test (textdraw.c) */
#endif

struct PauseMenuState
{
    u8 unk0[4];
    /*0x04*/ u32 flags;
    /*0x08*/ s32 selection;
    /*0x0C*/ s32 itemCount;
    /*0x10*/ s32 menuType;
    u8 unk14;
    /*0x15*/ s8 playerId;
    u8 unk16[2];
    /*0x18*/ s8 hidden;
};

extern struct PauseMenuState g_related_to_pause_menu_input;
extern s32 active_monkey_id[];
extern s32 locale;
extern u32 frame_counter;
extern s32 sub_mode;
extern s32 lbl_1_bss_1538;

int fn_1_16C910(void);
int fn_1_D13C0(void);
void fn_1_AE99C(struct Sprite *sprite);
float g_get_string_sprite_width_2(char *str);
void destroy_sprite_with_unique_id(int tag);
struct Sprite *get_sprite_with_unique_id(int tag);
int g_smth_with_get_active_monkey_icon(int state, int timer, int id);

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
void textdraw_set_add_color(u32 color);
struct HudPracticeInfo
{
    s16 stageId;
    u32 flags;
};

struct HudStageSelectState
{
    s8 isStory;
};

extern struct HudPracticeInfo g_stage_id_in_practice_mode;
extern struct HudStageSelectState stageselect_is_storymode;
extern char *lbl_1_data_3204[];
extern s32 lbl_1_bss_A873C[6];
float textdraw_get_pixel_width_of_string_as_f32(char *str);
void sprite_monkey_counter_icon_disp(struct Sprite *sprite);

extern char current_stage_name[];
void textdraw_put_char(u8 c);
int g_smth_with_fonts_chara_load_wrapper_discard_result(char *str);
float textdraw_get_pixel_width_of_string(char *str);

#define RGBA(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
float g_call_get_string_sprite_width_3_discard_result(char *str);
void textdraw_set_border(void);
void mtxa_translate_neg_xyz(float x, float y, float z);
extern u32 g_some_other_flags;
extern u32 g_font_char_var_3;
void set_ui_element_sprite_pos(float x, float y);
void set_ui_element_sprite_depth(float z);
void set_ui_element_sprite_scale(float x, float y);
u32 lbl_1_data_3A9E8[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

s8 lbl_1_data_3AB58[] = {
    255, 0, 0,
    44, 101, 255,
    251, 255, 44,
    44, 255, 87,
};

char *lbl_1_data_3AB74[] = { "Yes", "", "", "", "", "はい" };
char *lbl_1_data_3AB98[] = { "No", "", "", "", "", "いいえ" };

static inline void hud_f32_to_u8(register float *in, register u8 *out)
{
#ifdef __MWERKS__
    asm
    {
        lfs f1, 0(in)
        psq_st f1, 0(out), 1, 2
    }
#else
    *out = pc_psq_st_u8(*in);
#endif
}

void sprite_pausemenu_disp(struct Sprite *sprite);
void g_something_with_pause_menu_and_game_over_sprite(s8 *status, struct Sprite *sprite);
void sprite_game_over_tick(s8 *status, struct Sprite *sprite);
void sprite_titlescreen_press_start_tick(s8 *status, struct Sprite *sprite);
void sprite_logo_tick(s8 *status, struct Sprite *sprite);
void fn_1_BB210(struct Sprite *sprite);

static float lbl_1_data_3ABB0[7] = { -128.0f, 240.0f, 240.0f, 240.0f, 240.0f, 608.0f, 608.0f };

void create_pausemenu_sprite(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 4;
        sprite->x = 315.0f;
        sprite->y = lbl_1_data_3ABB0[0];
        sprite->type = 1;
        sprite->fontId = 3;
        sprite->textAlign = 4;
        sprite->depth = 0.005f;
        sprite->userVar = 1;
        sprite->unk58 = 0.0f;
        sprite->opacity = 0.5f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->scaleX = 0.0f;
        sprite->scaleY = 0.0f;
        sprite->counter = 1;
        sprite->drawFunc = sprite_pausemenu_disp;
        strcpy(sprite->text, "pause menu");
    }
}

void g_something_with_pause_menu_and_game_over_sprite(s8 *status, struct Sprite *sprite)
{
    sprite->y += 0.075 * (lbl_1_data_3ABB0[sprite->userVar] - sprite->y);
    if (sprite->counter > 0)
    {
        sprite->scaleY = sprite->scaleX;
        sprite->counter--;
    }
    else
        sprite->scaleY += 0.2 * (sprite->scaleX - sprite->scaleY);
    if (sprite->userVar >= 2 && sprite->userVar != 6 && sprite->opacity < 0.99)
        sprite->opacity += 0.01;
    if (sprite->userVar == 2)
    {
        if ((double)sprite->mulR < 250.0)
            sprite->mulR += 5;
        sprite->mulG = sprite->mulR;
        sprite->mulB = sprite->mulR;
    }
    if (sprite->userVar == 4)
    {
        struct Sprite *over;

        sprite->userVar = 5;
        over = create_sprite();
        if (over != NULL)
        {
            over->x = 320.0f;
            over->y = 240.0f;
            over->mulR = 0xFF;
            over->mulG = 0;
            over->mulB = 0x20;
            over->fontId = 9;
            over->textAlign = 4;
            over->counter = 60;
            over->userVar = 60;
            over->depth = sprite->depth - 0.001;
            over->mainFunc = sprite_game_over_tick;
            over->flags = 0x1000000;
            over->widescreenArg = 0x140;
            strcpy(over->text, "GAME OVER");
        }
    }
}

static char *lbl_1_data_3ACD8[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3AD74[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "View stage", "Level sehen", "Voir niveau", "Ver fase", "Guarda livello", "ステージをみわたす" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3AE34[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Save replay", "Wiederh. speich.", "Sauv. Replay", "Guar. Rept.", "Salva replay", "リプレイをセーブ" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3AF2C[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Retry", "Erneut", "R\xE9" "essayer", "Reint", "Riprova", "リトライ" },
    { "View stage", "Level sehen", "Voir niveau", "Ver fase", "Guarda livello", "ステージをみわたす" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Stage select", "Levelwahl", "Choix niveau", "Elegir Fase ", "Scelta livello", "ステージをえらぶ" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3AFBC[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Retry", "Erneut", "R\xE9" "essayer", "Reint", "Riprova", "リトライ" },
    { "Save replay", "Wiederh. speich.", "Sauv. Replay", "Guar. Rept.", "Salva replay", "リプレイをセーブ" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Stage select", "Levelwahl", "Choix niveau", "Elegir Fase ", "Scelta livello", "ステージをえらぶ" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3B04C[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Retry", "Erneut", "R\xE9" "essayer", "Reint", "Riprova", "リトライ" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3B0D4[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Guide", "Info", "Guide", "Gu\xED" "a", "Guida", "ガイド" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3B15C[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Save Game Data", "", "", "", "", "ゲームデータをセーブ" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3B1DC[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Retry", "Erneut", "R\xE9" "essayer", "Reint", "Riprova", "リトライ" },
    { "View stage", "Level sehen", "Voir niveau", "Ver fase", "Guarda livello", "ステージをみわたす" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Stage select", "---", "---", "---", "---", "ステージセレクトにもどる" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3B26C[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Retry", "Erneut", "R\xE9" "essayer", "Reint", "Riprova", "リトライ" },
    { "Save replay", "Wiederh. speich.", "Sauv. Replay", "Guar. Rept.", "Salva replay", "リプレイをセーブ" },
    { "How to play", "Spielregeln", "Comment jouer", "C\xF3mo jugar ", "Come giocare", "あそびかた" },
    { "Stage select", "---", "---", "---", "---", "ステージセレクトにもどる" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *lbl_1_data_3B2FC[][6] = {
    { "Continue game", "Weiterspielen", "Continuer", "Sigue Juego", "Continua partita", "ゲームをつづける" },
    { "Retry", "Erneut", "R\xE9" "essayer", "Reint", "Riprova", "リトライ" },
    { "Exit game", "Spielende", "Quitter", "Salir ju.", "Esci", "ゲームをおわる" },
};

static char *(*lbl_1_data_3B344[])[6] = {
    lbl_1_data_3ACD8, lbl_1_data_3AD74, lbl_1_data_3AF2C, lbl_1_data_3B04C,
    lbl_1_data_3B0D4, lbl_1_data_3B15C, lbl_1_data_3B1DC, lbl_1_data_3B2FC,
    lbl_1_data_3ACD8, lbl_1_data_3AE34, lbl_1_data_3AFBC, lbl_1_data_3B04C,
    lbl_1_data_3B0D4, lbl_1_data_3B15C, lbl_1_data_3B26C, lbl_1_data_3B2FC,
};
#ifndef __MWERKS__
/* PC port "Quit to Desktop" row (pause_menu.c / pc_quit.c) */
extern s32 pc_pause_quit_row;
const char *pc_quit_label(void);
int pc_quit_confirm_active(void);
void pc_quit_confirm_draw(float x, float y, float boxZ, float textZ);
#endif

void sprite_pausemenu_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    char text[32];
    float x;
    float y;
    float z;
    u32 flash;
    int i;
    int menuType;
    u32 flash2;

    z = fn_1_16C910() ? 0.011 + sprite->unk58 : sprite->depth;
    if (g_related_to_pause_menu_input.hidden == 1)
        return;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.rotation = 0;
    req.addColor = 0;
    req.unk30 = -1;
    req.flags = (sprite->flags & ~0xF) | 0xA;
#ifndef __MWERKS__
    /* with the Quit row the menus have 4, 5 or 7 rows: 5 fit the 4-row box
     * (30px rows), 7 the 6-row box (26px rows, 4px higher than 6 rows) */
    req.bmpId = g_related_to_pause_menu_input.itemCount >= 4 ? (g_related_to_pause_menu_input.itemCount >= 6 ? 0x5F : 5) : 0x44;
#else
    req.bmpId = g_related_to_pause_menu_input.itemCount >= 4 ? (g_related_to_pause_menu_input.itemCount == 6 ? 0x5F : 5) : 0x44;
#endif
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = 0.001 + z;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.opacity = 1.0f;
    req.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | 0xFFFFFF;
    draw_sprite_draw_request(&req);
    req.bmpId = g_smth_with_get_active_monkey_icon(0, 0, active_monkey_id[g_related_to_pause_menu_input.playerId]);
    req.x = sprite->x - 110.0f;
    req.y = 12.0f + (sprite->y + sprite->scaleY);
    req.z = z;
    req.scaleX = 0.5f;
    req.scaleY = 0.325f;
    req.opacity = 1.0f;
    req.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | 0xFFFFFF;
    draw_sprite_draw_request(&req);
    menuType = g_related_to_pause_menu_input.menuType;
    if (g_related_to_pause_menu_input.flags & 4)
        menuType += 8;
    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_flags(0x80000000);
    textdraw_set_drop_shadow();
    textdraw_set_depth(z - 0.001);
    flash = ((frame_counter >> 2) & 1) * 255;
    flash = RGBA(flash, flash, flash, 0);
    for (i = 0; i < g_related_to_pause_menu_input.itemCount; i++)
    {
#ifndef __MWERKS__
        if (g_related_to_pause_menu_input.itemCount == 7)
            z = i * 26 - 94;
        else
#endif
        if (g_related_to_pause_menu_input.itemCount == 6)
            z = i * 30 - 90;
        else if (g_related_to_pause_menu_input.itemCount == 5)
            z = i * 30 - 72;
        else if (g_related_to_pause_menu_input.itemCount == 4)
            z = i * 36 - 66;
        else
            z = i * 36 - 48 + (3 - g_related_to_pause_menu_input.itemCount) * 18;
#ifndef __MWERKS__
        /* the Quit to Desktop yes/no box replaces the rows while it is open
         * (the row text is not depth-sorted against the box) */
        if (pc_quit_confirm_active())
        {
            sprite->scaleX = 24.0f; /* monkey icon beside Yes / No */
            continue;
        }
#endif
#ifndef __MWERKS__
        if (i == pc_pause_quit_row)
            strcpy(text, pc_quit_label());
        else
#endif
        strcpy(text, lbl_1_data_3B344[menuType][i][locale]);
        x = sprite->x - 48.0f;
        y = sprite->y + z;
        textdraw_set_pos(x, y);
        textdraw_set_mul_color(i == g_related_to_pause_menu_input.selection ? 0xFFFF00 : 0x808000);
        textdraw_set_add_color(((g_related_to_pause_menu_input.flags & 1) && i == g_related_to_pause_menu_input.selection) ? flash : 0);
        textdraw_print(text);
        if (menuType == 4 && i == 1)
        {
            flash2 = (1.0 - fabs(math_sin(frame_counter << 9))) * 255.0;
            flash2 = RGBA(flash2, flash2, flash2, 0);
            strcpy(text, "ON");
            x += 96.0f;
            textdraw_set_pos(x, y);
            if (g_related_to_pause_menu_input.flags & 8)
                textdraw_set_mul_color(0xC0C000);
            else
                textdraw_set_mul_color(0x808000);
            if (i == g_related_to_pause_menu_input.selection && (g_related_to_pause_menu_input.flags & 8))
                textdraw_set_add_color(flash2);
            else
                textdraw_set_add_color(0);
            textdraw_print(text);
            strcpy(text, "OFF");
            x += 50.0f;
            textdraw_set_pos(x, y);
            if (!(g_related_to_pause_menu_input.flags & 8))
                textdraw_set_mul_color(0xC0C000);
            else
                textdraw_set_mul_color(0x808000);
            if (i == g_related_to_pause_menu_input.selection && !(g_related_to_pause_menu_input.flags & 8))
                textdraw_set_add_color(flash2);
            else
                textdraw_set_add_color(0);
            textdraw_print(text);
        }
        if (i == g_related_to_pause_menu_input.selection)
            sprite->scaleX = z;
    }
#ifndef __MWERKS__
    {
        /* the yes/no box, in front of the menu box */
        float depth = fn_1_16C910() ? 0.011 + sprite->unk58 : sprite->depth;

        pc_quit_confirm_draw(sprite->x, sprite->y, depth - 0.002f, depth - 0.003f);
    }
#endif
    if (g_related_to_pause_menu_input.hidden == 1)
        return;
    req.bmpId = 0x4B;
    req.x = 320.1f;
    req.y = 240.1f;
    req.z = sprite->userVar == 2 ? 0.001 : 0.002 + sprite->depth;
    sprite->unk58 = req.z;
    req.scaleX = 80.0f;
    req.scaleY = 60.0f;
    req.opacity = sprite->opacity;
    req.mulColor = RGBA(sprite->mulR, sprite->mulG, sprite->mulB, (u8)(255.0f * sprite->opacity));
    draw_sprite_draw_request(&req);
}

#ifndef __MWERKS__
/* PC port: "B:Quit" next to PRESS START on the title screen, in the style of
 * the menus' button prompts (sel_ngc_sprite.c sprite_button_disp: pad glyph,
 * ':' and the label in the menu font), and the Quit to Desktop yes/no box B
 * opens there (adv.c smd_adv_title_tick, pc_quit.c).  Tag 1 like the PRESS
 * START letters, so it comes and goes with them. */
const char *pc_quit_short_label(void);
void pc_menu_quit_draw(int where);

static void pc_title_quit_hint_draw(struct Sprite *sprite)
{
    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    textdraw_set_scale(0.9f, 0.9f);
    textdraw_set_border();
    textdraw_set_depth(0.04f);
    textdraw_set_pos(sprite->x, sprite->y);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_print("\x83\xA0"); /* B button glyph */
    textdraw_set_mul_color(0xFFFF00);
    textdraw_print(":");
    textdraw_print_and_fit_to_width(48.0f, (char *)pc_quit_short_label());
    textdraw_reset();
    reset_ui_widescreen_scale_mtx();
    pc_menu_quit_draw(1 /* PCMQ_TITLE */);
}
#endif

void create_titlescreen_press_start_sprites(int arg0)
{
    u8 buf[4];
    GXColor color;
    float y;
    float x;
    float width;
    float center;
    u8 *str;
    int counter;
    int step;
    char *const texts[6] = {
        "PRESS START/PAUSE",
        "DR\xDC" "CKE START/PAUSE!",
        "APPUYEZ SUR START/PAUSE",
        "PULSA START/PAUSE",
        "PREMI START/PAUSE",
        "スタートボタンを押してね",
    };
    GXColor colors[2] = { { 0xFF, 0xF0, 0xF0, 0 }, { 0xFF, 0xC0, 0x80, 0 } };
#ifndef __MWERKS__
    int pcTitle = (arg0 == 2); /* the title screen (adv.c smd_adv_title_*) */
#endif

    destroy_sprite_with_unique_id(1);
    textdraw_reset();
    textdraw_set_font(4);
    str = (u8 *)texts[locale];
#ifndef __MWERKS__
    /* PC port: "PRESS OPTIONS" / "PRESS MENU" for PlayStation / Xbox pads */
    {
        static char s_text[64];
        char *pc_button_prompts_text(char *str);
        char *src = pc_button_prompts_text((char *)str);
        int n;

        /* Font 4 (the bmp_com ASCII sheet) has no accented or kana glyphs
         * on the US disc: German "DR\xDC" "CKE" and the Japanese line come out
         * broken on the original too.  Those use the English line. */
        for (n = 0; str[n] != 0; n++)
            if (str[n] >= 0x80)
                break;
        if (str[n] != 0)
            src = pc_button_prompts_text(texts[0]);
        for (n = 0; n < (int)sizeof(s_text) - 1 && src[n] != 0; n++)
            s_text[n] = src[n];
        s_text[n] = 0;
        str = (u8 *)s_text;
    }
#endif
    width = g_get_string_sprite_width_2((char *)str);
    buf[1] = 0;
    switch (arg0)
    {
    case 3:
        center = 320.0f;
        y = 248.0f;
        x = center - 0.5f * width;
        break;
    case 2:
        center = 320.0f;
        y = 426.0f;
        x = center - 0.5f * width;
        break;
    default:
        center = 320.0f;
        y = 434.0f;
        x = center - 0.5f * width;
        break;
    }
    counter = 0;
    step = -1;
    arg0 = 0;
    color = colors[arg0];
    while (*str != 0)
    {
        struct Sprite *s;

        buf[0] = *str;
        width = g_get_string_sprite_width_2((char *)buf);
        if (*str == ' ' || *str == '/')
        {
            step -= 2;
            arg0 ^= 1;
            color = colors[arg0];
        }
        if (*str != ' ')
        {
            s = create_sprite();
            if (s != NULL)
            {
                s->tag = 1;
                s->x = x;
                s->y = y;
                s->depth = 0.05f;
                s->fontId = 4;
                s->textAlign = 2;
                s->flags = 0x1000000;
                s->mulR = color.r;
                s->mulG = color.g;
                s->mulB = color.b;
                s->widescreenArg = 0x140;
                s->counter = counter;
                s->mainFunc = sprite_titlescreen_press_start_tick;
                strcpy(s->text, (char *)buf);
            }
            counter += step;
        }
        if (*str == '/')
        {
            step -= 2;
            arg0 ^= 1;
            color = colors[arg0];
        }
        x += width;
        str++;
    }
#ifndef __MWERKS__
    if (pcTitle)
    {
        struct Sprite *s = create_sprite();

        if (s != NULL)
        {
            s->tag = 1;
            s->x = 540.0f;
            s->y = 406.0f;
            s->depth = 0.04f;
            s->drawFunc = pc_title_quit_hint_draw;
            strcpy(s->text, "pc_quit_hint");
        }
    }
#endif
}

void sprite_titlescreen_press_start_tick(s8 *status, struct Sprite *sprite)
{
    u8 rgb[4];
    float c[3];
    float t;

    sprite->counter++;
    t = 0.5f * (1.0f + math_sin(sprite->counter << 10));
    c[0] = 510.0f * t - 255.0f;
    c[1] = 447.0f * t - 255.0f;
    c[2] = 383.0f * t - 255.0f;
    hud_f32_to_u8(&c[0], &rgb[0]);
    hud_f32_to_u8(&c[1], &rgb[1]);
    hud_f32_to_u8(&c[2], &rgb[2]);
    sprite->addR = rgb[0];
    sprite->addG = rgb[1];
    sprite->addB = rgb[2];
}

void g_sprite_copyright_text_disp(struct Sprite *sprite)
{
    int t;

    sprite->drawFunc = NULL;
    t = fn_1_D13C0();
    if (t > 0)
    {
        sprite->x = 997 - t;
        sprite->y = 446.0f;
        fn_1_AE99C(sprite);
    }
    if (t < 640)
    {
        sprite->x = t + 357;
        sprite->y = 463.0f;
        fn_1_AE99C(sprite);
    }
    sprite->drawFunc = g_sprite_copyright_text_disp;
}

void g_create_smd_adv_logo_sprites(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->tag = 3;
        sprite->x = 320.0f;
        sprite->y = 115.0f;
        sprite->textAlign = 4;
        sprite->depth = 300.0f;
        if (lbl_1_bss_1538 != 0)
            sprite->flags = 0x1000000;
        sprite->widescreenArg = 0x140;
        sprite->bmpId = 0x101;
        strcpy(sprite->text, "logo plus");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->textAlign = 4;
        sprite->depth = 301.0f;
        sprite->bmpId = 0x102;
        sprite->counter = 0;
        sprite->mainFunc = sprite_logo_tick;
        sprite->flags |= 0x40000;
        if (lbl_1_bss_1538 != 0)
            sprite->scaleX = 0.75f;
        strcpy(sprite->text, "logo");
    }
}

void sprite_logo_tick(s8 *status, struct Sprite *sprite)
{
    struct Sprite *other;

    if (sub_mode == 20)
    {
        *status = 0;
        return;
    }
    other = get_sprite_with_unique_id(3);
    if (sprite->counter > 393)
    {
        if (sprite->counter <= 408)
            sprite->x -= 10.0f + 6.0f * math_sin((sprite->counter - 393) * 0x444);
        else if (sprite->counter <= 480)
            sprite->x -= 16.0f;
    }
    if (sprite->counter >= 378 && sprite->counter <= 408)
    {
        sprite->scaleX = 1.0 - 0.7 * math_sin((sprite->counter - 378) * 0x444);
        if (lbl_1_bss_1538 != 0)
            sprite->scaleX *= 0.75;
        sprite->scaleY = 1.0 + 0.7 * math_sin((sprite->counter - 378) * 0x444);
        if (other != NULL)
            other->opacity = 0.0333 * (408 - sprite->counter);
    }
    if (sprite->counter == 480)
    {
        sprite->x = 320.0f;
        sprite->bmpId = 0x100;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
        sprite->depth = 100.0f;
        if (lbl_1_bss_1538 != 0)
            sprite->flags = 0x1000000;
        sprite->widescreenArg = 0x140;
        if (other != NULL)
        {
            other->bmpId = 0x107;
            other->userVar = 5;
            other->drawFunc = fn_1_BB210;
            other->x = 194.0f;
            other->y = 74.0f;
        }
    }
    if (sprite->counter < 480)
        sprite->opacity = 1.0f;
    else if (sprite->counter <= 600)
        sprite->opacity = 0.0f;
    else if (sprite->counter <= 630)
    {
        sprite->opacity = 0.0333 * (sprite->counter - 600);
        if (other != NULL)
            other->opacity = sprite->opacity;
    }
    sprite->counter++;
}

void fn_1_BB210(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    int idx = sprite->userVar;

    req.bmpId = sprite->bmpId;
    req.x = sprite->x + (float)(idx % 2 == 1 ? 128 : -128);
    req.y = sprite->y + (float)(sprite->userVar == 0 ? 50 : 0);
    req.z = sprite->depth;
    req.u1 = 0.0f;
    req.v1 = 0.15625 * idx;
    req.u2 = 1.0f;
    req.v2 = 0.15625 + 0.15625 * idx;
    req.scaleX = sprite->scaleX * (req.u2 - req.u1);
    req.scaleY = 0.15625 * sprite->scaleY;
    req.rotation = sprite->rotation;
    req.opacity = sprite->opacity;
    req.unk30 = -1;
    req.flags = (sprite->flags & ~0xF) | 0xA;
    req.mulColor = RGBA(sprite->mulR, sprite->mulG, sprite->mulB, (u8)(255.0f * sprite->opacity));
    req.addColor = 0;
    draw_sprite_draw_request(&req);
}

void sprite_copyright_text_tick(s8 *status, struct Sprite *sprite);

void create_copyright_text_sprites(int arg0)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0x25;
        sprite->type = 1;
        sprite->x = 357.0f;
        sprite->y = 463.0f;
        sprite->textAlign = 2;
        sprite->depth = 0.03f;
        sprite->bmpId = 0xD;
        sprite->flags = 0x1000000;
        sprite->widescreenArg = 0x220;
        sprite->mainFunc = sprite_copyright_text_tick;
        strcpy(sprite->text, "(C)");
        if (arg0 == 1)
            sprite->drawFunc = g_sprite_copyright_text_disp;
        sprite = create_linked_sprite(sprite);
        if (sprite != NULL)
        {
            sprite->tag = 0x26;
            sprite->x = 6.0f;
            sprite->fontId = 1;
            sprite->depth = 0.03f;
            sprite->flags = 0x1000000;
            sprite->widescreenArg = 0x220;
            sprite->mainFunc = sprite_copyright_text_tick;
            strcpy(sprite->text, "AMUSEMENT VISION/SEGA,2001,2002");
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x27;
        sprite->type = 1;
        sprite->x = 16.0f;
        sprite->y = 462.0f;
        sprite->depth = 0.03f;
        sprite->bmpId = 0x52;
        sprite->textAlign = 2;
        sprite->flags = 0x1000000;
        sprite->widescreenArg = 0x10;
        sprite->mainFunc = sprite_copyright_text_tick;
        strcpy(sprite->text, "SEGA AV");
    }
}

void sprite_copyright_text_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar == -1 && sprite->opacity > 0.0f)
        sprite->opacity -= 0.05;
}

void sprite_title_tick(s8 *status, struct Sprite *sprite);
void sprite_title_disp(struct Sprite *sprite);

void create_title_sprite(int frames)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0x71;
        sprite->type = 1;
        sprite->depth = 0.095f;
        sprite->opacity = 0.0f;
        sprite->counter = (s16)frames;
        sprite->userVar = frames;
        sprite->mainFunc = sprite_title_tick;
        sprite->drawFunc = sprite_title_disp;
        strcpy(sprite->text, "title");
    }
}

void sprite_title_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter > 0)
        sprite->counter--;
    sprite->opacity = 1.0 - (float)sprite->counter / (float)sprite->userVar;
}

void sprite_title_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;

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
    req.bmpId = lbl_1_bss_1538 != 0 ? 0x11C : 0x11B;
    req.x = 320.1f;
    req.y = 240.1f;
    req.z = 0.004 + sprite->depth;
    req.opacity = sprite->opacity;
    req.flags = (sprite->flags & ~0xF) | 0xA;
    draw_sprite_draw_request(&req);
}

void g_create_some_controls_description_sprite(void)
{
    struct Sprite *sprite;

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x11;
        sprite->x = 320.0 - 0.5 * textdraw_get_pixel_width_of_string_as_f32(lbl_1_data_3204[locale]);
        sprite->y = 60.0f;
        sprite->fontId = 0;
        sprite->type = 1;
        sprite->depth = 0.04f;
        sprite->drawFunc = sprite_monkey_counter_icon_disp;
        sprintf(sprite->text, "saru");
    }
    lbl_1_bss_A873C[0] = lbl_1_bss_A873C[1] = 0;
    lbl_1_bss_A873C[2] = 0;
    lbl_1_bss_A873C[5] = 14;
}

char lbl_1_data_3B460[] = "banana";
char lbl_1_data_3B468[] = "eieipu";

u32 lbl_1_data_3B470[] = {
    0x42C80000, 0x42480000, 0x43960000, 0x42480000, 0x43FA0000,
    0x42480000, 0x442F0000, 0x42480000, 0x00000000, 0x437A0000, 0x43480000,
    0x437A0000, 0x43C80000, 0x437A0000, 0x44160000, 0x437A0000, 0x44480000,
    0x437A0000, 0x42C80000, 0x43E10000, 0x43960000, 0x43E10000, 0x43FA0000,
    0x43E10000, 0x442F0000, 0x43E10000, 0x43480000, 0xC3160000, 0x43C80000,
    0xC3160000, 0x44160000, 0xC3160000, 0x44480000, 0xC3160000, 0x42C80000,
    0x42480000, 0x43960000, 0x42480000, 0x43FA0000, 0x42480000, 0x442F0000,
    0x42480000, 0x00000000, 0x437A0000, 0x43480000, 0x437A0000, 0x43C80000,
    0x437A0000, 0x44160000, 0x437A0000, 0x44480000, 0x437A0000, 0x42C80000,
    0x43E10000, 0x43960000, 0x43E10000, 0x43FA0000, 0x43E10000, 0x442F0000,
    0x43E10000, 0x00000000, 0x44228000, 0x43480000, 0x44228000, 0x43C80000,
    0x44228000, 0x44160000, 0x44228000, 0xC2C80000, 0x42480000, 0x42C80000,
    0x42480000, 0x43960000, 0x42480000, 0x43FA0000, 0x42480000, 0x442F0000,
    0x42480000, 0x00000000, 0x437A0000, 0x43480000, 0x437A0000, 0x43C80000,
    0x437A0000, 0x44160000, 0x437A0000, 0x44480000, 0x437A0000, 0xC2C80000,
    0x43E10000, 0x42C80000, 0x43E10000, 0x43960000, 0x43E10000, 0x43FA0000,
    0x43E10000, 0x442F0000, 0x43E10000, 0x00000048, 0x00000031, 0x00000041,
    0x00FFFF00, 0x00FF00FF, 0x0000FFFF,
};

void sprite_loadin_stage_name_tick(s8 *status, struct Sprite *sprite);
void sprite_loadin_stage_name_disp(struct Sprite *sprite);
void sprite_bonus_stage_tick(s8 *status, struct Sprite *sprite);
void sprite_final_stage_tick(s8 *status, struct Sprite *sprite);
void sprite_final_stage_disp(struct Sprite *sprite);

void create_stage_loadin_text_sprites(void)
{
    struct Sprite *sprite = create_sprite();
    float unused1 = 146.0f;
    float unused2 = 0.1f;
    float unused3 = 40.0f;
    double unused4 = 0.8;
    double unused5 = 0.6;
    float unused6 = 15.0f;
    float unused7 = 0.65f;
    float unused8 = 4096.0f;
    double unused9 = 0.65;
    float unused10 = 0.8f;

    if (sprite != NULL)
    {
        int stage;
        s16 tmp;

        sprite->tag = 0xF;
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->depth = 0.15f;
        if ((modeCtrl.gameType == 2 && (g_stage_id_in_practice_mode.flags & 8)) || (modeCtrl.courseFlags & 8))
        {
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0;
            sprite->addR = 0x20;
            sprite->addG = 0x20;
            sprite->addB = 0x20;
        }
        else
        {
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0;
        }
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->flags |= 0x1001000;
        sprite->unk58 = 0.0f;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_loadin_stage_name_tick;
        sprite->drawFunc = sprite_loadin_stage_name_disp;
        tmp = modeCtrl.gameType == 2 ? g_stage_id_in_practice_mode.stageId : mode_info.currFloor;
        stage = tmp;
        if (modeCtrl.gameType == 1)
            sprintf(sprite->text, "ROUND %d", stage);
        else if (modeCtrl.gameType == 14 || (modeCtrl.gameType == 2 && stageselect_is_storymode.isStory == 1))
            sprintf(sprite->text, "WORLD %d]%d", mode_info.storyWorld + 1, mode_info.storyStage + 1);
        else if ((modeCtrl.gameType == 2 && (g_stage_id_in_practice_mode.flags & 0x800000)) || (modeCtrl.courseFlags & 0x800000))
            sprintf(sprite->text, "MASTER EX%d", stage);
        else if ((modeCtrl.gameType == 2 && (g_stage_id_in_practice_mode.flags & 0x10)) || (modeCtrl.courseFlags & 0x10))
            sprintf(sprite->text, "MASTER %d", stage);
        else if ((modeCtrl.gameType == 2 && (g_stage_id_in_practice_mode.flags & 8)) || (modeCtrl.courseFlags & 8))
            sprintf(sprite->text, "EXTRA %d", stage);
        else
            sprintf(sprite->text, "STAGE %d", stage);
    }
    if ((mode_info.flags & 0x40) && modeCtrl.gameType != 1)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 0xE;
            sprite->x = 320.0f;
            sprite->y = 300.0f;
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->mulR = 0xFF;
            sprite->mulG = 0x80;
            sprite->mulB = 0;
            sprite->scaleX = 0.5f;
            sprite->scaleY = 0.5f;
            sprite->flags |= 0x1001000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = sprite_bonus_stage_tick;
            strcpy(sprite->text, "BONUS STAGE");
        }
    }
    if ((mode_info.flags & 0x1000) && modeCtrl.gameType != 2)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 0xE;
            sprite->x = 320.0f;
            sprite->y = 300.0f;
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->scaleX = 0.5f;
            sprite->scaleY = 0.5f;
            sprite->flags |= 0x1001000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = sprite_final_stage_tick;
            sprite->drawFunc = sprite_final_stage_disp;
            if (modeCtrl.gameType == 1)
                strcpy(sprite->text, "FINAL ROUND");
            else
                strcpy(sprite->text, "FINAL STAGE");
        }
    }
}

char stage_name_tilde_fmt_string[] = "\x81\x60 %s \x81\x60";

void sprite_loadin_stage_name_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar > 0)
        sprite->opacity = 0.06666 * sprite->userVar;
    else
        sprite->opacity = 1.0f;
    sprite->counter++;
    if (merged_digital_inputs[0] & 0x100)
        sprite->counter++;
    if (sprite->counter > 240)
        sprite->unk58 += 0.2f * -sprite->unk58;
    if (sprite->userVar != 0)
    {
        sprite->userVar--;
        if (sprite->userVar == 0)
            *status = 0;
    }
}

static inline int hud_is_story_mode(void)
{
    int ret = 0;

    if (modeCtrl.gameType == 14 || (modeCtrl.gameType == 2 && stageselect_is_storymode.isStory == 1))
        ret = 1;
    return ret;
}

#define HUD_MIN(a, b) ((a) < (b) ? (a) : (b))
#define HUD_CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

void sprite_loadin_stage_name_disp(struct Sprite *sprite)
{
    u8 buf[256];
    u8 text[12];
    u8 ch[4];
    float scale;
    float alpha;
    float t;
    int width;
    int i;
    int spaceIdx;
    int yOffs;
    int xOffs;
    int phase;

    spaceIdx = 0;
    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_flags(sprite->flags);
    textdraw_set_mul_color(sprite->mulB | ((sprite->mulR << 16) | (sprite->mulG << 8)));
    textdraw_set_add_color(sprite->addB | ((sprite->addR << 16) | (sprite->addG << 8)));
    width = (u32)strlen(sprite->text) * 57 / 2;
    strcpy((char *)text, sprite->text);
    for (i = 0; i < (u32)strlen(sprite->text); i++)
    {
        if (text[i] == ' ')
            spaceIdx = i;
    }
    for (i = 0; i < (u32)strlen(sprite->text); i++)
    {
        if (i == spaceIdx)
            continue;
        if (i > spaceIdx)
        {
            if (sprite->counter < spaceIdx * 16 + 30)
                continue;
        }
        else if (i > 0)
        {
            if (sprite->counter < (i - 1) * 16 + 30)
                continue;
        }
        xOffs = 0;
        yOffs = 0;
        if (i == 0 && sprite->counter < 30)
            xOffs = (30 - sprite->counter) * -5;
        else if (i > 0 && i < spaceIdx && sprite->counter < i * 16 + 30)
            xOffs = -3.6 * ((i * 16 + 30) - sprite->counter);
        else if (i > spaceIdx)
            xOffs = -28;
        if (sprite->userVar > 0)
        {
            if (i < spaceIdx)
                yOffs = (15 - sprite->userVar) * ((i & 1) ? 8 : -8);
            else
                yOffs = (15 - sprite->userVar) * ((i & 1) ? -8 : 8);
        }
        textdraw_set_pos(xOffs + (57.6 * i + (14.4 + (sprite->x - (float)width))), sprite->y - 25.6 + yOffs);
        textdraw_set_scale(sprite->scaleX - 0.01, sprite->scaleY + 0.01 * fabs((float)yOffs));
        if (i > spaceIdx)
            textdraw_set_alpha(HUD_MIN(0.0625 * (sprite->counter - 30 - spaceIdx * 16), sprite->opacity));
        else if (i > 0)
            textdraw_set_alpha(HUD_MIN(0.0625 * (sprite->counter - 30 - (i - 1) * 16), sprite->opacity));
        else if (i == 0 && sprite->userVar > 0)
            textdraw_set_alpha(sprite->opacity);
        textdraw_put_char(text[i]);
    }
    yOffs = hud_is_story_mode();
    scale = yOffs ? 1.4f : 0.8f;
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0xA0000000);
    textdraw_set_mul_color(0xFFFF00);
    textdraw_set_font_style(5);
    sprintf((char *)buf, stage_name_tilde_fmt_string, current_stage_name);
    textdraw_set_scale(scale, scale);
    i = g_smth_with_fonts_chara_load_wrapper_discard_result((char *)buf);
    spaceIdx = textdraw_get_pixel_width_of_string((char *)buf);
    textdraw_set_alignment(0);
    textdraw_set_pos(sprite->x - 0.5f * (float)spaceIdx, sprite->y + (float)(yOffs ? 22 : 20));
    for (yOffs = 0, spaceIdx = 0; yOffs < i; yOffs++, spaceIdx++)
    {
        ch[0] = buf[spaceIdx];
#ifdef __MWERKS__
        if (ch[0] >= 0x80)
#else
        if (pc_text_is_lead(ch[0]))
#endif
        {
            spaceIdx++;
            ch[1] = buf[spaceIdx];
            ch[2] = 0;
        }
        else
            ch[1] = 0;
        if (sprite->counter <= 120)
            break;
        phase = sprite->counter - 120 - yOffs * 4;
        t = (float)(16 - phase) / 8.0f < 1.0f ? 1.0f : ((float)(16 - phase) / 8.0f > 2.0f ? 2.0f : (float)(16 - phase) / 8.0f);
        alpha = (float)phase / 8.0f < 0.0f ? 0.0f : ((float)phase / 8.0f > 1.0f ? 1.0f : (float)phase / 8.0f);
        if (t > 1.9f)
            alpha = 0.0f;
        t *= scale;
        textdraw_set_scale(t, t);
        textdraw_set_alpha(alpha * sprite->opacity);
        textdraw_print((char *)ch);
    }
}

void sprite_player_num_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter > 90)
    {
        sprite->x += (float)sprite->userVar;
        sprite->userVar++;
    }
    else if (sprite->counter <= 30)
    {
        sprite->x += (float)sprite->userVar;
        sprite->userVar--;
    }
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void sprite_ready_tick(s8 *status, struct Sprite *sprite);

void create_player_num_and_ready_sprites(int frames)
{
    struct Sprite *sprite;

    if (modeCtrl.gameType == 0 && modeCtrl.playerCount > 1)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            int idx = modeCtrl.playerCount > 1 ? current_ball->playerId : 3;

            sprite->x = 785.0f;
            sprite->y = 310.0f;
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->mulR = lbl_1_data_3AB58[idx * 3 + 0];
            sprite->mulG = lbl_1_data_3AB58[idx * 3 + 1];
            sprite->mulB = lbl_1_data_3AB58[idx * 3 + 2];
            sprite->userVar = -30;
            sprite->counter = 120;
            sprite->mainFunc = sprite_player_num_tick;
            sprintf(sprite->text, "%dP", current_ball->playerId + 1);
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->depth = modeCtrl.unk30 == 1 ? 0.1 : 0.05;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xC8;
        sprite->mulB = 0;
        sprite->counter = (s16)frames;
        sprite->userVar = frames;
        sprite->flags |= 0x1001000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_ready_tick;
        strcpy(sprite->text, "READY");
    }
    call_SoundReqID_arg_0(4);
}

void sprite_ready_tick(s8 *status, struct Sprite *sprite)
{
    int t = sprite->userVar - sprite->counter;

    if (t < 30)
    {
        sprite->opacity = 0.03333 * t;
        sprite->scaleX = 1.0f;
        sprite->scaleY = math_sin(t * 0x222);
    }
    else if (t < 60)
    {
        sprite->opacity = 1.0f;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0 - 0.2 * math_sin((t - 30) * 0x444);
    }
    else if (sprite->counter <= 15)
    {
        sprite->opacity = 0.06666 * sprite->counter;
        sprite->scaleX = 0.06666 * sprite->counter;
        sprite->scaleY = 1.0f;
    }
    else
    {
        sprite->opacity = 1.0f;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
    }
    if (--sprite->counter <= 0)
    {
        call_SoundReqID_arg_0(5);
        *status = 0;
    }
}

void sprite_bonus_stage_tick(s8 *status, struct Sprite *sprite)
{
    double unused1 = 216000.0;
    double unused2 = 1000.0;
    float unused3 = 999.0f;
    double unused4 = 999.0;
    float t;

    if (sprite->userVar > 0)
    {
        sprite->opacity = 0.06666 * sprite->userVar;
        sprite->rotation = 0;
    }
    else if (sprite->counter < 60)
    {
        sprite->opacity = 0.01666 * sprite->counter;
        sprite->rotation = (s16)((60 - sprite->counter) * 0x111);
    }
    else
    {
        sprite->opacity = 1.0f;
        sprite->rotation = 0;
    }
    if (sprite->counter >= 150)
    {
        sprite->x = 500.0f;
        sprite->y = 452.0f;
        sprite->scaleX = 0.3f;
        sprite->scaleY = sprite->scaleX;
        t = 1.0f;
    }
    else if (sprite->counter >= 120)
    {
        int d = sprite->counter - 120;

        sprite->x = d * 6 + 320;
        sprite->y = 300.0 + 5.066666 * d;
        sprite->scaleX = 0.5 - 0.006666 * d;
        sprite->scaleY = sprite->scaleX;
        t = (float)(sprite->counter - 120) / 30.0;
    }
    else
        t = 0.0f;
    sprite->widescreenArg = 320.0 + 320.0 * t;
    sprite->counter++;
    if (sprite->userVar != 0)
    {
        sprite->userVar--;
        if (sprite->userVar == 0)
            *status = 0;
    }
}

void sprite_final_stage_tick(s8 *status, struct Sprite *sprite)
{
    float t;

    if (modeCtrl.difficulty == 0)
    {
        sprite->mulR = 0;
        sprite->mulG = 0xD0;
        sprite->mulB = 0;
    }
    else if (modeCtrl.difficulty == 1)
    {
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0xE0;
    }
    else
    {
        sprite->mulR = 0xFF;
        sprite->mulG = 0xC0;
        sprite->mulB = 0;
    }
    if (sprite->counter >= 150)
    {
        float v = 64.0f * math_sin((s16)((sprite->counter - 150) << 9));

        sprite->mulR = HUD_CLAMP(sprite->mulR + v, 0.0f, 255.0f);
        sprite->mulG = HUD_CLAMP(sprite->mulG + v, 0.0f, 255.0f);
        sprite->mulB = HUD_CLAMP(sprite->mulB + v, 0.0f, 255.0f);
    }
    if (sprite->userVar > 0)
        sprite->opacity = 0.06666 * sprite->userVar;
    else
        sprite->opacity = 1.0f;
    if (sprite->counter >= 150)
    {
        if (modeCtrl.unk30 == 1)
        {
            sprite->x = 500.0f;
            sprite->y = 452.0f;
        }
        else
        {
            sprite->x = 140.0f;
            sprite->y = 242.0f;
        }
        sprite->scaleX = 0.3f;
        sprite->scaleY = sprite->scaleX;
        t = 1.0f;
    }
    else if (sprite->counter >= 120)
    {
        int d = sprite->counter - 120;

        if (modeCtrl.unk30 == 1)
        {
            sprite->x = d * 6 + 320;
            sprite->y = 300.0 + 5.066666 * d;
        }
        else
        {
            sprite->x = 320 - d * 6;
            sprite->y = 300.0 - 1.933333 * d;
        }
        sprite->scaleX = 0.5 - 0.006666 * d;
        sprite->scaleY = sprite->scaleX;
        t = (float)d / 30.0;
    }
    else
        t = 0.0f;
    sprite->widescreenArg = 320.0 + 320.0 * t;
    sprite->counter++;
    if (sprite->userVar != 0)
    {
        sprite->userVar--;
        if (sprite->userVar == 0)
            *status = 0;
    }
}

void sprite_final_stage_disp(struct Sprite *sprite)
{
    u8 buf[16];
    int w;
    int h;
    int i;
    int charW;
    int pos;
    float sx;
    float sy;
    float a;

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_flags(sprite->flags);
    textdraw_set_mul_color(RGBA(sprite->mulR, sprite->mulG, sprite->mulB, 0));
    textdraw_set_add_color(RGBA(sprite->addR, sprite->addG, sprite->addB, 0));
    w = sprite->scaleX * (((u32)strlen(sprite->text) - 2) * 72 + 48);
    h = 64.0f * sprite->scaleY;
    if (sprite->counter > 120)
    {
        textdraw_set_pos(sprite->x - 0.5 * w, sprite->y - 0.5 * h);
        textdraw_set_scale(sprite->scaleX, sprite->scaleY);
        textdraw_set_alpha(sprite->opacity);
        textdraw_print(sprite->text);
    }
    else
    {
        strcpy((char *)buf, sprite->text);
        pos = 0;
        for (i = 0; i < (u32)strlen(sprite->text); i++)
        {
            int offs;

            if (i > sprite->counter / 10)
                break;
            a = HUD_MIN((float)(sprite->counter - 10.0 * i) / 10.0, 1.0);
            if (buf[i] == 'I')
                charW = 30.0f * sprite->scaleX;
            else if (buf[i] == ' ')
                charW = 18.0f * sprite->scaleX;
            else
                charW = 72.0f * sprite->scaleX;
            offs = buf[i] == 'I' ? 0.5 * (-42.0f * sprite->scaleX) : 0.0;
            sx = 1.0 + (i % 2 == 0 ? 5.0 * (1.0 - a) : 0.0);
            sy = 1.0 + (i % 2 == 0 ? 0.0 : 5.0 * (1.0 - a));
            textdraw_set_pos(
                offs + (pos + (sprite->x - 0.5 * w)) - 0.5 * (charW * (sx - 1.0)),
                sprite->y - 0.5 * h - 0.5 * (h * (sy - 1.0)));
            textdraw_set_scale(sprite->scaleX * sx, sprite->scaleY * sy);
            textdraw_set_alpha(sprite->opacity * a);
            if (buf[i] != ' ')
                textdraw_put_char(buf[i]);
            pos += charW;
        }
    }
    textdraw_reset();
}

void sprite_go_tick(s8 *status, struct Sprite *sprite);
void sprite_go_disp(struct Sprite *sprite);
extern u32 g_font_char_var_3;
char lbl_1_data_3B694[] = "%d";
char lbl_1_data_3B698[] = "%3.0f";

void create_go_sprite(int frames)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->depth = 0.05f;
        sprite->mulR = 0;
        sprite->mulG = 0x80;
        sprite->mulB = 0xFF;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = (s16)frames;
        sprite->userVar = frames;
        sprite->flags |= 0x1001000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_go_tick;
        sprite->drawFunc = sprite_go_disp;
        strcpy(sprite->text, "GO");
    }
}

void sprite_go_tick(s8 *status, struct Sprite *sprite)
{
    int t = sprite->userVar - sprite->counter;

    if (t < 15)
    {
        sprite->opacity = 0.06666 * t;
        sprite->addR = 0;
        sprite->addG = sprite->addR;
        sprite->addB = sprite->addR;
        sprite->scaleX = 1.0 + 0.1 * (15 - t);
        sprite->scaleY = 1.0 + -0.04 * (15 - t);
    }
    else if (t < 30)
    {
        sprite->opacity = 1.0f;
        sprite->addR = ((g_font_char_var_3 >> 1) & 1) * 192;
        sprite->addG = sprite->addR;
        sprite->addB = sprite->addR;
        sprite->scaleX = 1.0 - 0.5 * math_sin((t - 15) * 0x888);
        sprite->scaleY = 1.0 + math_sin((t - 15) * 0x888);
    }
    else if (t < 45)
    {
        sprite->opacity = 1.0f;
        sprite->addR = ((g_font_char_var_3 >> 1) & 1) * 192;
        sprite->addG = sprite->addR;
        sprite->addB = sprite->addR;
        sprite->scaleX = 1.0 + 0.75 * math_sin((t - 30) * 0x888);
        sprite->scaleY = 1.0 - 0.375 * math_sin((t - 30) * 0x888);
    }
    else
    {
        sprite->opacity = 0.06666 * sprite->counter;
        sprite->addR = 0;
        sprite->addG = sprite->addR;
        sprite->addB = sprite->addR;
        sprite->scaleX = 1.0 + -0.04 * (15 - sprite->counter);
        sprite->scaleY = 1.0 + 0.1 * (15 - sprite->counter);
    }
    if (--sprite->counter <= 0)
        *status = 0;
}

void sprite_go_disp(struct Sprite *sprite)
{
    int i;
    int w;
    int h;
    int t = sprite->userVar - sprite->counter;
    int unused;
    float k;
    float xOffs;
    float yOffs;

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_depth(sprite->depth);
    textdraw_set_flags(sprite->flags);
    textdraw_set_alpha(sprite->opacity);
    textdraw_set_mul_color(RGBA(sprite->mulR, sprite->mulG, sprite->mulB, 0));
    textdraw_set_add_color(RGBA(sprite->addR, sprite->addG, sprite->addB, 0));
    textdraw_set_scale(1.5f * sprite->scaleX, 1.5f * sprite->scaleY);
    unused = 1.5f * (72.0f * sprite->scaleX);
    w = 1.5f * (36.0f * sprite->scaleX);
    h = 1.5f * (32.0f * sprite->scaleY);
    for (i = 0; i < 2; i++)
    {
        if (t < 15)
        {
            k = i == 0 ? -320.0f : 320.0f;
            xOffs = k * math_sin((15 - t) * 0x444);
            yOffs = 0.0f;
        }
        else if (t < 30)
        {
            xOffs = 0.0f;
            yOffs = 0.0f;
        }
        else if (t < 45)
        {
            xOffs = 0.0f;
            yOffs = 0.0f;
        }
        else
        {
            xOffs = 0.0f;
            k = i == 0 ? -240.0f : 240.0f;
            yOffs = k * math_sin((15 - sprite->counter) * 0x444);
        }
        textdraw_set_pos(
            (sprite->x + xOffs) - w + (i == 0 ? -w : w),
            (sprite->y + yOffs) - h);
        textdraw_put_char(i == 0 ? 'G' : 'O');
    }
}

char JUMP_TO_STAGE_STRING[] = "JUMP TO STAGE %d";
char lbl_1_data_3B6B8[] = "ZU LEVEL %d SPRINGEN";
char lbl_1_data_3B6D0[] = "SAUTER A L'ETAGE %d";
char lbl_1_data_3B6E4[] = "SALTA AL PISO %d";
char lbl_1_data_3B6F8[] = "SALTA AL PIANO %d";

char *lbl_1_data_3B70C[] = {
    JUMP_TO_STAGE_STRING,
    lbl_1_data_3B6B8,
    lbl_1_data_3B6D0,
    lbl_1_data_3B6E4,
    lbl_1_data_3B6F8,
};

void sprite_goal_tick(s8 *status, struct Sprite *sprite);
void sprite_goal_disp(struct Sprite *sprite);
void sprite_jump_to_stage_tick(s8 *status, struct Sprite *sprite);
void sprite_jump_to_stage_duplicate_tick(s8 *status, struct Sprite *sprite);
void sprite_fallout_tick(s8 *status, struct Sprite *sprite);
void create_bonus_finish_sprite(void);

void create_goal_and_jump_to_stage_sprites(int frames)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        if (modeCtrl.gameType == 1)
            sprite->y = 240.0f;
        else
            sprite->y = 320.0f;
        sprite->depth = 0.05f;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = (s16)frames;
        sprite->userVar = frames;
        sprite->flags |= 0x1001000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_goal_tick;
        sprite->drawFunc = sprite_goal_disp;
        strcpy(sprite->text, "GOAL");
    }
    if (mode_info.unk22 != 1)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 320.0f;
            sprite->y = 240.0f;
            sprite->mulR = 0;
            sprite->mulG = 0xC0;
            sprite->mulB = 0xFF;
            switch (locale)
            {
            case 1:
            case 2:
                sprite->scaleX = 1.200006f;
                break;
            default:
                sprite->scaleX = 1.33334f;
                break;
            }
            sprite->scaleY = 0.800004f;
            sprite->fontId = 3;
            sprite->textAlign = 4;
            sprite->counter = (s16)frames;
            sprite->userVar = frames;
            sprite->opacity = 1.0f;
            sprite->unk24 = 0;
            sprite->unk20 = sprite->opacity;
            sprite->flags |= 0xA0001000;
            sprite->mainFunc = sprite_jump_to_stage_tick;
            sprintf(sprite->text, JUMP_TO_STAGE_STRING, mode_info.currFloor);
        }
    }
}

float lbl_1_bss_A8730;
float lbl_1_bss_A8734;

void sprite_goal_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    int t;
    int flash;
    int i;
    float scale;
    float opacity;

    req.bmpId = 0x511;
    req.z = sprite->depth;
    req.rotation = sprite->rotation;
    req.unk30 = -1;
    req.flags = (sprite->flags & 0xFFFFFFF0) | 0xA;
    t = sprite->userVar - sprite->counter;
    if (t < 30)
    {
        opacity = 0.016666 * t;
        flash = 0;
    }
    else if (t < 45)
    {
        opacity = 1.0f;
        flash = math_sin((t - 30) * 0x888) * 255.0f;
    }
    else if (sprite->counter <= 15)
    {
        opacity = 0.06666 * sprite->counter;
        flash = 0;
    }
    else
    {
        opacity = 1.0f;
        flash = 0;
    }
    req.opacity = opacity;
    req.mulColor = RGBA(sprite->mulR, sprite->mulG, sprite->mulB, (u8)(opacity * 255.0f));
    req.addColor = RGBA(flash, flash, flash, 0);
    if (t >= 210)
    {
        sprite->x = 561.0f;
        sprite->y = 420.0f;
        sprite->scaleX = 0.5f;
        sprite->scaleY = sprite->scaleX;
        scale = 1.0f;
    }
    else if (t >= 180)
    {
        int d = t - 180;

        sprite->x = 320.0 + 8.03333 * d;
        if (modeCtrl.gameType == 1)
            sprite->y = 240.0 + 6.0 * d;
        else
            sprite->y = 320.0 + 3.33333 * d;
        sprite->scaleX = 1.0 - 0.01666 * d;
        sprite->scaleY = sprite->scaleX;
        scale = d / 30.0;
    }
    sprite->widescreenArg = 536.0 * scale + 320.0 * (1.0 - scale);
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = sprite->scaleX;
    req.scaleY = sprite->scaleY;
    if (t < 30)
    {
        struct { float x; float y; } offs[4] = {
            { -1.0f, 1.0f },
            { 1.0f, 1.0f },
            { 1.0f, -1.0f },
            { -1.0f, -1.0f },
        };

        opacity = 0.03333 * (30 - t);
        for (i = 0; i < 4; i++)
        {
            lbl_1_bss_A8730 = sprite->x;
            lbl_1_bss_A8734 = sprite->y;
            lbl_1_bss_A8730 += opacity * (216.0f * offs[i].x);
            lbl_1_bss_A8734 += opacity * (192.0f * offs[i].y);
            req.x = lbl_1_bss_A8730;
            req.y = lbl_1_bss_A8734;
            draw_sprite_draw_request(&req);
        }
    }
    else
    {
        req.x = sprite->x;
        req.y = sprite->y;
        draw_sprite_draw_request(&req);
    }
    textdraw_reset();
}

void sprite_jump_to_stage_tick(s8 *status, struct Sprite *sprite)
{
    int t = sprite->userVar - sprite->counter;

    if (t < 30)
        sprite->opacity = 0.03333 * t;
    else if (sprite->counter <= 15)
        sprite->opacity = 0.06666 * sprite->counter;
    else
        sprite->opacity = 1.0f;
    sprite->x = 320.0f + 20.0f * math_sin(t << 9);
    sprite->y = 240.0f + 10.0f * math_sin((t << 8) + 0x4000);
    if (t % 2 == 1)
    {
        struct Sprite *dup = create_sprite();

        if (dup != NULL)
        {
            dup->x = sprite->x;
            dup->y = sprite->y;
            dup->depth = 0.1f + sprite->depth;
            dup->mulR = sprite->mulR;
            dup->mulG = sprite->mulG;
            dup->mulB = sprite->mulB;
            dup->addR = 0;
            dup->addG = 96;
            dup->addB = 127;
            dup->scaleX = sprite->scaleX;
            dup->scaleY = sprite->scaleY;
            dup->fontId = 3;
            dup->textAlign = 4;
            dup->counter = 16;
            dup->opacity = 0.5f;
            dup->unk24 = 0;
            dup->unk20 = dup->opacity;
            dup->flags |= 0xA0001000;
            dup->mainFunc = sprite_jump_to_stage_duplicate_tick;
            sprintf(dup->text, lbl_1_data_3B70C[locale], mode_info.currFloor);
        }
    }
    if (--sprite->counter <= 0)
        *status = 0;
}

void sprite_jump_to_stage_duplicate_tick(s8 *status, struct Sprite *sprite)
{
    sprite->addG += 6;
    sprite->addB += 7;
    sprite->depth += 0.1f;
    sprite->scaleX += 0.008;
    sprite->scaleY += 0.008;
    sprite->opacity -= 0.03125;
    sprite->unk20 = sprite->opacity;
    if (--sprite->counter <= 0)
        *status = 0;
}

void create_fallout_or_bonus_finish_sprite(int frames)
{
    if (!(mode_info.flags & 0x40))
    {
        struct Sprite *sprite = create_sprite();

        if (sprite != NULL)
        {
            sprite->x = 320.0f;
            sprite->y = 240.0f;
            sprite->depth = 0.05f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0x8C;
            sprite->mulB = 0;
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->counter = (s16)frames;
            sprite->userVar = frames;
            sprite->flags |= 0x1001000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = sprite_fallout_tick;
            strcpy(sprite->text, "FALL OUT");
        }
    }
    else
        create_bonus_finish_sprite();
}

void sprite_fallout_tick(s8 *status, struct Sprite *sprite)
{
    int t = sprite->userVar - sprite->counter;

    if (modeCtrl.gameType != 5 && t == 90)
        call_SoundReqID_arg_0(12);
    if (t < 30)
    {
        sprite->opacity = 0.03333 * t;
        sprite->y = 240 - (30 - t) * 6;
    }
    else if (t < 60)
    {
        sprite->opacity = 1.0f;
        sprite->y = 240.0f - 32.0f * math_sin((t - 30) * 0x444);
    }
    else if (t < 90)
    {
        sprite->opacity = 1.0f;
        sprite->y = 240.0f - 8.0f * math_sin((t - 60) * 0x444);
    }
    else if (sprite->counter <= 15)
        sprite->opacity = 0.06666 * sprite->counter;
    else
        sprite->opacity = 1.0f;
    if (modeCtrl.gameType != 5 && modeCtrl.gameType != 7)
    {
        float k;

        if (t >= 120)
        {
            sprite->x = 496.0f;
            sprite->y = 420.0f;
            sprite->scaleX = 0.5f;
            sprite->scaleY = sprite->scaleX;
            k = 1.0f;
        }
        else if (t >= 90)
        {
            t -= 90;
            sprite->x = 320.0 + 5.86666 * t;
            sprite->y = t * 6 + 240;
            sprite->scaleX = 1.0 - 0.01666 * t;
            sprite->scaleY = sprite->scaleX;
            k = (float)t / 30.0;
        }
        else
            k = 0.0f;
        sprite->widescreenArg = 320.0 + 320.0 * k;
    }
    else
    {
        sprite->scaleX = 0.85f;
        sprite->scaleY = sprite->scaleX;
    }
    if (--sprite->counter <= 0)
        *status = 0;
}

void sprite_time_over_tick(s8 *status, struct Sprite *sprite);
void sprite_bonus_finish_or_perfect_tick(s8 *status, struct Sprite *sprite);

void create_time_over_sprite(int frames)
{
    if (!(mode_info.flags & 0x40))
    {
        struct Sprite *sprite = create_sprite();

        if (sprite != NULL)
        {
            sprite->x = 320.0f;
            sprite->y = 240.0f;
            sprite->depth = 0.05f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0x8C;
            sprite->mulB = 0;
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->counter = (s16)frames;
            sprite->userVar = frames;
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = sprite_time_over_tick;
            strcpy(sprite->text, "TIME OVER");
        }
    }
    else
        create_bonus_finish_sprite();
}

void sprite_time_over_tick(s8 *status, struct Sprite *sprite)
{
    int t = sprite->userVar - sprite->counter;

    if (t < 30)
    {
        sprite->opacity = 0.03333 * t;
        sprite->y = 240 - (30 - t) * 6;
    }
    else if (t < 60)
    {
        sprite->opacity = 1.0f;
        sprite->y = 240.0f - 32.0f * math_sin((t - 30) * 0x444);
    }
    else if (t < 90)
    {
        sprite->opacity = 1.0f;
        sprite->y = 240.0f - 8.0f * math_sin((t - 60) * 0x444);
    }
    else if (sprite->counter <= 15)
        sprite->opacity = 0.06666 * sprite->counter;
    else
        sprite->opacity = 1.0f;
    if (--sprite->counter <= 0)
        *status = 0;
}

void create_bonus_finish_or_perfect_sprite(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->depth = 0.05f;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = 0;
        sprite->userVar = 1;
        sprite->flags |= 0x1000;
        sprite->mainFunc = sprite_bonus_finish_or_perfect_tick;
        strcpy(sprite->text, "PERFECT");
    }
}

void create_bonus_finish_sprite(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->depth = 0.05f;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = 0;
        sprite->userVar = 0;
        sprite->mulR = 0xFF;
        sprite->mulG = 0x80;
        sprite->mulB = 0;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->flags |= 0x1000;
        sprite->mainFunc = sprite_bonus_finish_or_perfect_tick;
        strcpy(sprite->text, "BONUS FINISH");
    }
}

void sprite_bonus_finish_or_perfect_tick(s8 *status, struct Sprite *sprite)
{
    float t = sprite->counter / 30.0f;

    sprite->scaleX = 0.8 * t + 5.0 * (1.0 - t);
    sprite->scaleY = 0.8 * t + 5.0 * (1.0 - t);
    if (sprite->counter == 29)
        call_SoundReqID_arg_0(sprite->userVar != 0 ? 0x48 : 0x45);
    if (sprite->counter < 30)
        sprite->counter++;
}

void sprite_continue_tick(s8 *status, struct Sprite *sprite);
void sprite_continues_remaining_tick(s8 *status, struct Sprite *sprite);
void sprite_continue_yes_or_no_tick(s8 *status, struct Sprite *sprite);
int fn_1_16D420(void);
int fn_1_16D3E0(void);
int fn_1_16C8D8(void);

void create_continue_sprites(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 50.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xC8;
        sprite->mulB = 0;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->userVar = 0;
        sprite->opacity = 0.0f;
        sprite->mainFunc = sprite_continue_tick;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x140;
        strcpy(sprite->text, "CONTINUE?");
    }
    if (fn_1_16D420() == 0)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 490.0f;
            sprite->y = 90.0f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0xC8;
            sprite->mulB = 0;
            sprite->fontId = 3;
            sprite->textAlign = 4;
            sprite->depth = 0.1f;
            sprite->opacity = 0.0f;
            sprite->flags |= 0xA0000000;
            sprite->unk20 = 1.0f;
            sprite->unk24 = 0;
            sprite->mainFunc = sprite_continues_remaining_tick;
            strcpy(sprite->text, "h/NOKORI");
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 160.0f;
        sprite->y = 260.0f;
        sprite->mulR = 0x8C;
        sprite->mulG = 0xFF;
        sprite->mulB = 0xFF;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->userVar = 1;
        sprite->counter = 0;
        sprite->opacity = 0.0f;
        sprite->mainFunc = sprite_continue_yes_or_no_tick;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0xA0;
        strcpy(sprite->text, "YES");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 480.0f;
        sprite->y = 260.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xA0;
        sprite->mulB = 0xBE;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->userVar = 0;
        sprite->counter = 0;
        sprite->opacity = 0.0f;
        sprite->mainFunc = sprite_continue_yes_or_no_tick;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x1E0;
        strcpy(sprite->text, "NO");
    }
}

void sprite_continue_tick(s8 *status, struct Sprite *sprite)
{
    if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1 && fn_1_16C8D8() == 0)
    {
        sprite->opacity += 0.1 * -sprite->opacity;
        return;
    }
    if (sprite->userVar == 0)
    {
        call_SoundReqID_arg_0(0x47);
        sprite->userVar = 1;
    }
    sprite->opacity += 0.1 * (1.0 - sprite->opacity);
}

char *lbl_1_data_3B7C8[] =
{
    "Continue(s):%d",
    "Continue(s):%d",
    "Cr\xE9" "dit(s):%d",
    "Cr\xE9" "dito(s):%d",
    "Continua:%d",
    "\x82\xcc\x82\xb1\x82\xe8\x82\xa9\x82\xa2\x82\xb7\x82\xa4:%d",
};

void sprite_continues_remaining_tick(s8 *status, struct Sprite *sprite)
{
    if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1 && fn_1_16C8D8() == 0)
        sprite->opacity += 0.1 * -sprite->opacity;
    else
        sprite->opacity += 0.1 * (1.0 - sprite->opacity);
    sprintf(sprite->text, lbl_1_data_3B7C8[locale], fn_1_16D3E0());
}

void sprite_continue_yes_or_no_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar == 1)
    {
        sprite->mulR = 0x8C;
        sprite->mulG = 0xFF;
        sprite->mulB = 0xFF;
    }
    else
    {
        sprite->mulR = 0xFF;
        sprite->mulG = 0xA0;
        sprite->mulB = 0xBE;
    }

    if (modeCtrl.courseFlags & 4)
    {
        sprite->opacity += 0.1 * (1.0 - sprite->opacity);
        if (sprite->userVar == modeCtrl.unk10)
        {
            int val;

            sprite->x += 0.1 * (320.0 - sprite->x);
            sprite->y += 0.1 * (200.0 - sprite->y);
            switch (g_font_char_var_3 & 7)
            {
            case 0:
            case 7:
                val = 0;
                break;
            case 1:
            case 6:
                val = 0x4F;
                break;
            case 2:
            case 5:
                val = 0x8F;
                break;
            default:
                val = 0xCF;
                break;
            }
            sprite->addR = (u8)val;
            sprite->addG = (u8)val;
            sprite->addB = (u8)val;
            return;
        }
        sprite->y += sprite->counter;
        sprite->counter++;
        sprite->mulR /= 2;
        sprite->mulG /= 2;
        sprite->mulB /= 2;
        sprite->addR = 0;
        sprite->addG = 0;
        sprite->addB = 0;
        return;
    }

    if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1 && fn_1_16C910() == 0)
    {
        sprite->opacity += 0.1 * -sprite->opacity;
        return;
    }

    sprite->opacity += 0.1 * (1.0 - sprite->opacity);
    if (sprite->userVar == modeCtrl.unk10)
    {
        float t = 2.0 * ((float)((g_font_char_var_3 + 36) % 60) / 59.0);
        int val;

        if (t > 1.0)
            t = 2.0 - t;
        val = t * 255.0f;
        sprite->addR = (u8)val;
        sprite->addG = (u8)val;
        sprite->addB = (u8)val;
    }
    else
    {
        sprite->mulR /= 2;
        sprite->mulG /= 2;
        sprite->mulB /= 2;
        sprite->addR = 0;
        sprite->addG = 0;
        sprite->addB = 0;
    }
}

void fn_1_BF758(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter > 90)
    {
        sprite->x += sprite->userVar;
        sprite->userVar += 1;
    }
    else if (sprite->counter <= 30)
    {
        sprite->y += sprite->userVar;
        sprite->userVar += 2;
    }
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void create_game_over_letter_sprite(int duration, int x, int y, int delay, char *text);

void g_create_some_game_over_and_player_num_sprites(int duration)
{
    struct Sprite *sprite;

    create_game_over_letter_sprite(duration, 57, 240, 0, "G");
    create_game_over_letter_sprite(duration, 129, 240, 4, "A");
    create_game_over_letter_sprite(duration, 201, 240, 8, "M");
    create_game_over_letter_sprite(duration, 273, 240, 12, "E");
    create_game_over_letter_sprite(duration, 367, 240, 16, "O");
    create_game_over_letter_sprite(duration, 439, 240, 20, "V");
    create_game_over_letter_sprite(duration, 511, 240, 24, "E");
    create_game_over_letter_sprite(duration, 583, 240, 28, "R");
    if (modeCtrl.gameType == 0 && modeCtrl.playerCount > 1)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            int idx = modeCtrl.playerCount > 1 ? current_ball->playerId : 3;

            sprite->x = 785.0f;
            sprite->y = 310.0f;
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->mulR = lbl_1_data_3AB58[idx * 3 + 0];
            sprite->mulG = lbl_1_data_3AB58[idx * 3 + 1];
            sprite->mulB = lbl_1_data_3AB58[idx * 3 + 2];
            sprite->userVar = -30;
            sprite->counter = 120;
            sprite->depth = 0.008f;
            sprite->mainFunc = fn_1_BF758;
            sprite->flags = 0x1000000;
            sprite->widescreenArg = 0x140;
            sprintf(sprite->text, "%dP", current_ball->playerId + 1);
        }
    }
}

void create_game_over_letter_sprite(int duration, int x, int y, int delay, char *text)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = x;
        sprite->y = y;
        sprite->mulR = 0xFF;
        sprite->mulG = 0;
        sprite->mulB = 0x20;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = duration;
        sprite->userVar = duration;
        sprite->unk50 = delay;
        sprite->unk54 = -0x100;
        sprite->depth = 0.008f;
        sprite->mainFunc = sprite_game_over_tick;
        sprite->flags = 0x1000000;
        sprite->widescreenArg = 0x140;
        strcpy(sprite->text, text);
    }
}

extern int lbl_80198B38;

void sprite_game_over_tick(s8 *status, struct Sprite *sprite)
{
    int t = sprite->userVar - sprite->counter;

    if (t < lbl_80198B38)
    {
        int f = t - sprite->unk50;

        if (f < 0)
            f = 0;
        if (f > 30)
            f = 30;
        sprite->opacity = f * 0.03333;
        if (sprite->unk50 + 30 > t && t > sprite->unk50)
            {
            float a = (t - sprite->unk50) / 30.0f;

            sprite->y = 240.0f - 200.0f * math_sin(32768.0f * a);
        }
        else if (t > sprite->unk50 + 30)
            sprite->y = 240.0f;
    }
    else if (lbl_80198B38 <= t && t < (int)(1.3334 * lbl_80198B38))
    {
    }
    else if ((int)(1.3334 * lbl_80198B38) <= t && t < 2.5 * lbl_80198B38)
    {
        sprite->opacity = 1.0f;
        sprite->rotation += sprite->unk54;
        {
            int d = -0x1000 - sprite->rotation;

            sprite->unk54 += d;
        }
        sprite->unk54 *= 0.9;
    }
    else if (2.5 * lbl_80198B38 <= t && t < (int)(3.3334 * lbl_80198B38))
    {
    }
    else if ((int)(3.3334 * lbl_80198B38) <= t && t < lbl_80198B38 * 4)
    {
        sprite->opacity = 1.0f;
        sprite->rotation += sprite->unk54;
        {
            int d = -sprite->rotation;

            sprite->unk54 += d;
        }
        sprite->unk54 *= 0.85;
    }
    else
    {
        sprite->opacity = (float)sprite->counter / (float)lbl_80198B38;
        sprite->depth = 0.008f;
        sprite->rotation = 0;
    }
    if (--sprite->counter <= 0)
        *status = 0;
}

void sprite_1up_tick(s8 *status, struct Sprite *sprite);

void create_1up_sprite(int frames)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 16;
        sprite->x = 320.0f;
        sprite->y = 300.0f;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->opacity = 0.0f;
        sprite->counter = frames + 30;
        sprite->userVar = frames;
        sprite->mainFunc = sprite_1up_tick;
        strcpy(sprite->text, "1UP");
    }
}

void sprite_1up_tick(s8 *status, struct Sprite *sprite)
{
    int t = sprite->userVar - sprite->counter;
    float x;

    if (t >= 0)
    {
        if (t == 0)
        {
            call_SoundReqID_arg_1(0xA00E);
            SoundRevID(14, 100);
            SoundChoID(14, 100);
        }
        if (t < 30)
        {
            sprite->opacity = t * 0.03333;
            x = 320.0f * math_sin((t * 0x222) + 0x4000);
        }
        else if (sprite->counter < 30)
        {
            sprite->opacity = sprite->counter * 0.03333;
            x = -320.0f * math_sin((sprite->counter * 0x222) + 0x4000);
        }
        else
        {
            sprite->opacity = 1.0f;
            x = 0.0f;
        }
        sprite->x = 320.0f + x;
        {
            int colors[] =
            {
                  0,   0,   0,
                255,   0,   0,
                255, 255, 255,
                255, 255,   0,
            };
            int idx = ((t >> 1) % 4) * 3;

            sprite->mulR = (u8)colors[idx + 0];
            sprite->mulG = (u8)colors[idx + 1];
            sprite->mulB = (u8)colors[idx + 2];
        }
    }
    if (--sprite->counter <= 0)
        *status = 0;
}

void fn_1_C02AC(void)
{
}
void sprite_replay_tick(s8 *status, struct Sprite *sprite);

void create_replay_sprite(int frames)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 0;
        sprite->x = 760.0f;
        sprite->y = 460.0f;
        sprite->fontId = 5;
        sprite->textAlign = 8;
        sprite->depth = 0.09f;
        sprite->scaleX = 1.25f;
        sprite->scaleY = 1.25f;
        sprite->counter = frames - 17;
        sprite->mainFunc = sprite_replay_tick;
        strcpy(sprite->text, "REPLAY");
    }
    sprite = get_sprite_with_unique_id(14);
    if (sprite != NULL)
        sprite->userVar = 15;
}

void sprite_replay_tick(s8 *status, struct Sprite *sprite)
{
    sprite->counter--;
    if (sprite->counter < 0 || !(mode_info.flags & 0x10))
    {
        sprite->x += 8.0f;
        if (sprite->x > 760.0f)
            *status = 0;
    }
    else if (sprite->x > 624.0f)
    {
        sprite->x -= 12.0f;
        if (sprite->x < 624.0f)
            sprite->x = 624.0f;
    }
}

char *lbl_1_data_3B83C[] = { "GREATEST", "2ND", "3RD" };
s16 lbl_1_data_3B848[] = { 72, 72, 48, 600, 528, 0 };
char lbl_1_data_3B854[] = "OUT OF RANK";
char lbl_1_data_3B860[] = "YOU ARE %s BALLER!";
char lbl_1_data_3B874[] = "YOU ARE %dTH BALLER!";
char lbl_1_data_3B88C[] = "00";
char lbl_1_data_3B890[] = "NAME";
char lbl_1_data_3B898[] = "ENTRY";
char lbl_1_data_3B8A0[] = "CONGRATULATIONS";
char lbl_1_data_3B8B0[] = "THANK  YOU  FOR  PLAYING";

struct StaffCreditLine
{
    s16 time;
    char *text;
    u8 isName;
};

struct StaffCreditLine lbl_1_data_3BB68[] =
{
    { 0, "Staff", 0 },
    { 35, "Chief Programmer", 0 },
    { 55, "Hisashi Endo", 1 },
    { 80, "Programmers", 0 },
    { 100, "Jun Tokuhara", 1 },
    { 115, "Yoshinori Suzuki", 1 },
    { 130, "Eiji Takaki", 1 },
    { 155, "Chief Designer", 0 },
    { 175, "Junichi Yamada", 1 },
    { 200, "Designers", 0 },
    { 220, "Mika Kojima", 1 },
    { 235, "Yukinobu Arikawa", 1 },
    { 250, "Yukio Oda", 1 },
    { 265, "Mari Sasaki", 1 },
    { 290, "Sound Design", 0 },
    { 310, "Yoshiyuki Kadooka", 1 },
    { 325, "Chamy", 1 },
    { 340, "Hiroyuki Hamada", 1 },
    { 355, "Hidenori Shoji", 1 },
    { 370, "Sakae Osumi", 1 },
    { 395, "Monkey Voice", 0 },
    { 415, "Kaoru Morota", 1 },
    { 430, "Konami Yoshida", 1 },
    { 445, "Rio Natsuki", 1 },
    { 470, "Mechatronics", 0 },
    { 490, "Masao Yoshimoto", 1 },
    { 505, "Akihiko Kohno", 1 },
    { 520, "Kazuhiro Nojo", 1 },
    { 535, "Hiroki Koyama", 1 },
    { 550, "Masahito Yanase", 1 },
    { 565, "Seiji Ishii", 1 },
    { 580, "Yutaka Yokoyama", 1 },
    { 595, "Eiji Inoue", 1 },
    { 620, "Publicity", 0 },
    { 640, "Masae Otoshi", 1 },
    { 665, "Cooperation", 0 },
    { 685, "Dole Food Company,Inc", 1 },
    { 700, "Dole Japan,Ltd.", 1 },
    { 757, "Special Thanks", 0 },
    { 777, "HAIKYO", 1 },
    { 792, "STURM Co.,Ltd.", 1 },
    { 807, "T's music Co.,Ltd.", 1 },
    { 864, "Producer & Director", 0 },
    { 884, "Toshihiro Nagoshi", 1 },
    { 0, NULL, 0 },
};

u16 lbl_1_data_3BD84[] =
{
    0x2d0, 0x205, 0x232, 0x1e0, 0x200, 0x33b, 0x205, 0x256, 0x1e0, 0x300, 0x3c0, 0x202, 0x140, 0x1e0, 0x0, 0x474, 0x200, 0x140, 0x1e0, 0x100,
};
u8 lbl_1_data_3BDAC[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
char lbl_1_data_3BDB8[] = "ENDING PICTURE";

char *lbl_1_data_3BDD4[] = { "GO TO THE", "", "", "", "", "GO TO THE" };
char *lbl_1_data_3BE0C[][6] =
{
    { "EXTRA", "", "", "", "", "EXTRA" },
    { "MASTER", "", "", "", "", "MASTER" },
    { "MASTER EXTRA", "", "", "", "", "MASTER EXTRA" },
};
char *lbl_1_data_3BE5C[] = { "STAGE", "", "", "", "", "STAGE" };


void g_sprite_extra_all_mask_tick(s8 *status, struct Sprite *sprite);
void g_sprite_go_to_the_tick(s8 *status, struct Sprite *sprite);
void g_sprite_go_to_the_disp(struct Sprite *sprite);
s8 fn_1_B2FD4(void);

struct ScreenFadeInfo
{
    /*0x00*/ s32 type;
    /*0x04*/ u32 color;
    /*0x08*/ s32 timer;
    /*0x0C*/ s32 timerMax;
};
extern struct ScreenFadeInfo g_screenfade_flags;

void create_go_to_extra_or_master_stages_sprites(void)
{
    float unused1 = 450.0f;
    float unused2 = 456.0f;
    double unused3 = 10.666666666666666;
    float unused4 = 640.0f;
    float unused5 = 880.0f;
    float unused6 = 440.0f;
    float unused7 = 350.0f;
    double unused8 = 260.0;
    double unused9 = 7.0;
    double unused10 = 0.125;
    float unused11 = 0.25f;
    float unused12 = 120.0f;
    double unused13 = 4.0;
    struct Sprite *sprite;
    int idx;

    if (modeCtrl.courseFlags & 0x800000)
        idx = 2;
    else if (modeCtrl.courseFlags & 0x10)
        idx = 1;
    else
        idx = 0;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 320.0f;
        sprite->y = 32.0f;
        sprite->textAlign = 4;
        sprite->bmpId = 0x4B;
        sprite->depth = 0.1f;
        sprite->opacity = 1.0f;
        sprite->scaleX = 128.0f;
        sprite->scaleY = 8.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprintf(sprite->text, "extra mask");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 320.0f;
        sprite->y = 448.0f;
        sprite->textAlign = 4;
        sprite->bmpId = 0x4B;
        sprite->depth = 0.1f;
        sprite->opacity = 1.0f;
        sprite->scaleX = 128.0f;
        sprite->scaleY = 8.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprintf(sprite->text, "extra mask");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 100.0f;
        sprite->textAlign = 4;
        sprite->fontId = 9;
        sprite->depth = 0.1f;
        sprite->opacity = 0.01f;
        sprite->scaleX = 0.6f;
        sprite->scaleY = 1.0f;
        sprite->flags |= 0x40000;
        sprite->counter = -120;
        sprite->userVar = 0;
        sprite->mainFunc = g_sprite_go_to_the_tick;
        sprite->drawFunc = g_sprite_go_to_the_disp;
        sprintf(sprite->text, lbl_1_data_3BDD4[locale]);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 392.0f;
        sprite->textAlign = 4;
        sprite->fontId = 9;
        sprite->depth = 0.1f;
        sprite->opacity = 0.01f;
        sprite->scaleX = idx == 2 ? 0.5f : 0.6f;
        sprite->scaleY = 1.0f;
        sprite->flags |= 0x40000;
        sprite->counter = -180;
        sprite->userVar = 1;
        sprite->mainFunc = g_sprite_go_to_the_tick;
        sprite->drawFunc = g_sprite_go_to_the_disp;
        sprintf(sprite->text, "%s %s", lbl_1_data_3BE0C[idx][locale], lbl_1_data_3BE5C[locale]);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->textAlign = 4;
        sprite->bmpId = 0x4B;
        sprite->depth = 0.2f;
        sprite->opacity = 0.5f;
        sprite->scaleX = 128.0f;
        sprite->scaleY = 60.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->flags |= 0x40000;
        sprite->mainFunc = g_sprite_extra_all_mask_tick;
        sprintf(sprite->text, "extra all mask");
    }
}

void g_sprite_extra_all_mask_tick(s8 *status, struct Sprite *sprite)
{
    float t = fn_1_B2FD4() == 1
        ? 1.0f - (float)g_screenfade_flags.timer / (float)g_screenfade_flags.timerMax
        : (float)g_screenfade_flags.timer / (float)g_screenfade_flags.timerMax;

    sprite->opacity = 0.5f + 0.5f * t;
}

void g_sprite_go_to_the_tick(s8 *status, struct Sprite *sprite)
{
    sprite->counter++;
}

#define GOTO_T (sprite->counter * 2 - i * 16)

void g_sprite_go_to_the_disp(struct Sprite *sprite)
{
    char ch[2] = {0, 0};
    float width;
    float offs = 0.0f;
    float half = 0.5f * (72.0f * sprite->scaleX);
    float fade;
    float y;
    int i;

    switch (g_screenfade_flags.type & 0xFF)
    {
    default:
        fade = 1.0 - (float)g_screenfade_flags.timer / (float)g_screenfade_flags.timerMax;
        break;
    case 1:
        fade = (float)g_screenfade_flags.timer / (float)g_screenfade_flags.timerMax;
        break;
    }

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_alignment(sprite->textAlign);
    textdraw_set_depth(sprite->depth);
    textdraw_set_scale(sprite->scaleX, sprite->scaleY);
    textdraw_set_flags(sprite->flags);
    width = textdraw_get_pixel_width_of_string(sprite->text);

    for (i = 0; i < g_smth_with_fonts_chara_load_wrapper_discard_result(sprite->text); i++)
    {
        float a;

        y = (sprite->userVar != 0 ? 1.0f : -1.0f) * (GOTO_T < 0 ? 0 : GOTO_T > 60 ? 60 : GOTO_T);
        a = math_sin((GOTO_T > 0 ? GOTO_T : 0) << 9) > sprite->opacity
            ? math_sin((GOTO_T > 0 ? GOTO_T : 0) << 9)
            : sprite->opacity;
        if (0.0f != y)
        {
            if (60.0f != y && -60.0f != y)
                a = sprite->opacity;
            a *= fade;
            ch[0] = sprite->text[i];
            textdraw_set_alpha(a);
            textdraw_set_add_color(sprite->userVar != 0 ? 0xFFFF00 : 0xFFFF);
            textdraw_set_pos(offs + (half + (sprite->x - 0.5f * width)), sprite->y + y);
            textdraw_print(ch);
            offs += g_call_get_string_sprite_width_3_discard_result(ch);
        }
    }
}

void sprite_goal_tick(s8 *status, struct Sprite *sprite);
void sprite_clear_score_disp(struct Sprite *sprite);
void sprite_warp_bonus_disp(struct Sprite *sprite);
void sprite_time_bonus_disp(struct Sprite *sprite);
void sprite_stage_score_disp(struct Sprite *sprite);

void create_postgoal_score_sprites(int clearScore, int stageScore, u32 flags)
{
    int duration = 360;
    int y = 128;
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 214.0f;
        sprite->y = y;
        if (flags & 8)
        {
            sprite->mulR = 0xFF;
            sprite->mulG = 0x60;
            sprite->mulB = 0x60;
        }
        else if (flags & 4)
        {
            sprite->mulR = 0x60;
            sprite->mulG = 0xFF;
            sprite->mulB = 0x60;
        }
        else
        {
            sprite->mulR = 0x60;
            sprite->mulG = 0x60;
            sprite->mulB = 0xFF;
        }
        sprite->fontId = 5;
        sprite->textAlign = 4;
        sprite->counter = duration;
        sprite->userVar = duration;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x60;
        sprite->mainFunc = sprite_goal_tick;
        sprite->drawFunc = sprite_clear_score_disp;
        sprintf(sprite->text, " %5d", clearScore);
    }
    if (flags & 2)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            y += 20;
            sprite->x = 214.0f;
            sprite->y = y;
            sprite->fontId = 5;
            sprite->textAlign = 4;
            sprite->counter = duration;
            sprite->userVar = duration;
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x60;
            sprite->mainFunc = sprite_goal_tick;
            sprite->drawFunc = sprite_warp_bonus_disp;
            if (mode_info.unk22 < 10)
                sprintf(sprite->text, "   X %d", mode_info.unk22);
            else
                sprintf(sprite->text, "  X %d", mode_info.unk22);
        }
    }
    if (flags & 1)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            y += 20;
            sprite->x = 214.0f;
            sprite->y = y;
            sprite->fontId = 5;
            sprite->textAlign = 4;
            sprite->counter = duration;
            sprite->userVar = duration;
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x60;
            sprite->mainFunc = sprite_goal_tick;
            sprite->drawFunc = sprite_time_bonus_disp;
            sprintf(sprite->text, "   X 2");
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        y += 24;
        sprite->x = 214.0f;
        sprite->y = y;
        sprite->fontId = 5;
        sprite->textAlign = 4;
        sprite->counter = duration;
        sprite->userVar = duration;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x60;
        sprite->mainFunc = sprite_goal_tick;
        sprite->drawFunc = sprite_stage_score_disp;
        sprintf(sprite->text, "%6d", stageScore);
    }
}

static inline int score_label_intro(struct Sprite *sprite, int t, float a)
{
    float alpha;
    int xOffs;

    if (t < 60)
        return 0;
    if (t < 90)
    {
        alpha = 0.03333 * (t - 60) * a;
        xOffs = (t - 90) * 5;
    }
    else if (sprite->counter <= 15)
    {
        alpha = 0.06666 * sprite->counter * MIN(0.2 + a, 1.0);
        xOffs = 0;
    }
    else
    {
        alpha = (double)a;
        xOffs = 0;
    }
    textdraw_set_alpha(alpha);
    textdraw_set_scale(sprite->scaleX, sprite->scaleY);
    textdraw_set_pos(sprite->x - 190.0f + xOffs, sprite->y - 10.0f);
    return 1;
}

static inline int score_value_intro(struct Sprite *sprite, int t, float a)
{
    float alpha;

    if (t < 120)
        return 0;
    if (t < 150)
        alpha = 0.03333 * (t - 120) * a;
    else if (sprite->counter <= 15)
        alpha = 0.06666 * sprite->counter * MIN(0.2 + a, 1.0);
    else
        alpha = (double)a;
    textdraw_set_alpha(alpha);
    return 1;
}

void sprite_clear_score_disp(struct Sprite *sprite)
{
    int t;
    float a;

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    t = sprite->userVar - sprite->counter;
    a = 1.0f;
    if (score_label_intro(sprite, t, a))
    {
        textdraw_set_mul_color((sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB);
        textdraw_print("CLEAR SCORE : ");
    }
    if (score_value_intro(sprite, t, a))
    {
        textdraw_set_mul_color((sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB);
        textdraw_print(sprite->text);
    }
    textdraw_reset();
}

void sprite_warp_bonus_disp(struct Sprite *sprite)
{
    int t;
    float a;

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_border();
    t = sprite->userVar - sprite->counter - 10;
    a = 1.0f;
    if (score_label_intro(sprite, t, a))
    {
        textdraw_set_mul_color(0xC0FF);
        textdraw_print(" WARP BONUS   ");
    }
    if (score_value_intro(sprite, t, a))
    {
        textdraw_set_mul_color(0xC0FF);
        textdraw_print(sprite->text);
    }
    textdraw_reset();
}

void sprite_time_bonus_disp(struct Sprite *sprite)
{
    int t;
    float a;

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_border();
    t = sprite->userVar - sprite->counter - 20;
    a = 1.0f;
    if (score_label_intro(sprite, t, a))
    {
        textdraw_set_mul_color(0xFFFF00);
        textdraw_print(" TIME BONUS   ");
    }
    if (score_value_intro(sprite, t, a))
    {
        textdraw_set_mul_color(0xFFFF00);
        textdraw_print(sprite->text);
    }
    textdraw_reset();
}

void sprite_stage_score_disp(struct Sprite *sprite)
{
    int t;
    int c;
    float a;

    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_border();
    t = sprite->userVar - sprite->counter - 60;
    a = 1.0f;
    c = MAX(255.0f * math_sin(t * 0x280), 0.0f);
    if (score_label_intro(sprite, t, a))
    {
        textdraw_set_mul_color(0xFF8000);
        textdraw_set_add_color((c << 16) | (c << 8) | c);
        textdraw_print("STAGE SCORE : ");
    }
    if (score_value_intro(sprite, t, a))
    {
        textdraw_set_mul_color(0xFF8000);
        textdraw_set_add_color((c << 16) | (c << 8) | c);
        textdraw_print(sprite->text);
    }
    textdraw_reset();
}

void sprite_goal_tick(s8 *status, struct Sprite *sprite)
{
    if (!(g_some_other_flags & 0xA) && --sprite->counter <= 0)
        *status = 0;
}

struct RetryOption
{
    char *text;
    float x;
    float y;
    u32 unkC;
    u8 color[4];
};

struct RetryOption lbl_1_data_3BEFC[] =
{
    { "YES", 160.0f, 260.0f, 0, { 0x8C, 0xFF, 0xFF, 0xFF } },
    { "NO", 480.0f, 260.0f, 0, { 0xFF, 0xA0, 0xBE, 0xFF } },
};

void fn_1_C1C78(s8 *status, struct Sprite *sprite);
void fn_1_C1CB0(s8 *status, struct Sprite *sprite);

void g_make_some_continue_screen_sprites(void)
{
    struct Sprite *sprite;
    int i;
    struct RetryOption *opt;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 50.0f;
        sprite->mulR = 255;
        sprite->mulG = 200;
        sprite->mulB = 0;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->userVar = 0;
        sprite->opacity = 0.0f;
        sprite->mainFunc = fn_1_C1C78;
        strcpy(sprite->text, "RETRY?");
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 320;
    }
    opt = lbl_1_data_3BEFC;
    for (i = 0; i < 2; i++, opt++)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = opt->x;
            sprite->y = opt->y;
            sprite->mulR = opt->color[0];
            sprite->mulG = opt->color[1];
            sprite->mulB = opt->color[2];
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->scaleX = 0.8f;
            sprite->scaleY = 0.8f;
            sprite->userVar = i;
            sprite->counter = 0;
            sprite->opacity = 0.0f;
            sprite->mainFunc = fn_1_C1CB0;
            strcpy(sprite->text, opt->text);
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 320;
        }
    }
}

void fn_1_C1C78(s8 *status, struct Sprite *sprite)
{
    sprite->opacity += 0.1 * (1.0 - sprite->opacity);
}

static inline void f32_to_u8(register float *in, register u8 *out)
{
#ifdef __MWERKS__
    asm
    {
        lfs f1, 0(in)
        psq_st f1, 0(out), 1, 2
    }
#else
    *out = pc_psq_st_u8(*in);
#endif
}

void fn_1_C1CB0(s8 *status, struct Sprite *sprite)
{
    int selected = sprite->userVar == modeCtrl.unk10;
    float color[3];
    float v;

    sprite->opacity += 0.1 * (1.0 - sprite->opacity);
    if (modeCtrl.submodeTimer > 0)
    {
        if (selected)
        {
            if ((modeCtrl.submodeTimer & 3) == 0)
            {
                sprite->addR = 128;
                sprite->addG = 128;
                sprite->addB = 128;
            }
            else
            {
                sprite->addR = 0;
                sprite->addG = 0;
                sprite->addB = 0;
            }
        }
    }
    else if (selected)
    {
        v = 0.5f * (1.0f + math_sin(g_font_char_var_3 << 10));
        color[0] = 128.0f + sprite->mulR * v;
        color[1] = 128.0f + sprite->mulG * v;
        color[2] = 128.0f + sprite->mulB * v;
        f32_to_u8(&color[0], &sprite->addR);
        f32_to_u8(&color[1], &sprite->addG);
        f32_to_u8(&color[2], &sprite->addB);
    }
    else
    {
        sprite->addR = 0;
        sprite->addG = 0;
        sprite->addB = 0;
    }
}

void sprite_save_tick(s8 *status, struct Sprite *sprite);
void sprite_save_disp(struct Sprite *sprite);

void create_save_sprite(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->depth = 0.1f;
        sprite->scaleY = 0.0f;
        sprite->userVar = 0;
        sprite->mainFunc = sprite_save_tick;
        sprite->drawFunc = sprite_save_disp;
        strcpy(sprite->text, "Save?");
    }
}

void sprite_save_tick(s8 *status, struct Sprite *sprite)
{
    double unused1 = 0.2f;
    float unused2 = -0.3f;
    int state = sprite->userVar;
    float angle = sprite->unk58;
    float vel = sprite->unk5C;
    float scale = sprite->scaleY;
    float target;

    switch (state)
    {
    case 0:
        state = 1;
        /* fall through */
    case 1:
        target = 1.0f;
        if (modeCtrl.courseFlags & 4)
        {
            state = 2;
            vel += 1024.0f * (1.0f + rand() / 32767.0f);
            if (rand() & 1)
                vel = -vel;
        }
        break;
    case 2:
        target = 1.0f;
        if ((float)modeCtrl.submodeTimer < 7.5f)
            state = 3;
        break;
    case 3:
        target = 0.0f;
        if (scale <= 0.0f)
            *status = 0;
        break;
    }
    vel += -1280.0f * math_sin(angle);
    vel *= 0.9f;
    angle += vel;
    if (scale < target)
    {
        scale += 0.13333334f;
        if (scale > target)
            scale = target;
    }
    else if (scale > target)
    {
        scale -= 0.13333334f;
        if (scale < target)
            scale = target;
    }
    sprite->userVar = state;
    sprite->unk58 = angle;
    sprite->unk5C = vel;
    sprite->scaleY = scale;
}

u32 fn_1_C2478(void);

void sprite_save_disp(struct Sprite *sprite)
{
    float scale = sprite->scaleY;
    float x;
    float y;
    int selected;
    int wide;
    int bmpId;
    s16 rot;
    u32 color;

    if (scale <= 0.0f)
        return;
    rot = (int)sprite->unk58;
    wide = modeCtrl.wideMode != 0;
    selected = modeCtrl.unk10;
    x = sprite->x;
    y = sprite->y;
    mtxa_from_identity();
    mtxa_translate_xyz(x, y, 0.0f);
    if (wide)
        mtxa_scale_xyz(0.75f, 1.0f, 1.0f);
    mtxa_rotate_z(rot);
    mtxa_scale_xyz(0.5f * (1.0f + scale), scale, 1.0f);
    mtxa_translate_neg_xyz(x, y, 0.0f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    bmpId = 0x44;
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(x, y);
    set_ui_element_sprite_depth(0.2f);
    scale = 112.0f / (float)g_get_bmp_height(bmpId);
    set_ui_element_sprite_scale(320.0f / (float)g_get_bmp_width(bmpId), scale);
    draw_ui_box(bmpId);

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_alignment(4);
    textdraw_set_flags(0x80000000);
    textdraw_set_depth(0.1f);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_pos(x, y - 24.0f);
    textdraw_print("SAVE GAME DATA");
    color = fn_1_C2478();
    if ((modeCtrl.courseFlags & 4) && (g_font_char_var_3 & 3) == 0)
        color = 0xFFFFFF;
    textdraw_set_border();
    if (selected == 0)
    {
        textdraw_set_mul_color(0xFFFFFF00);
        textdraw_set_add_color(color);
        textdraw_set_pos(x - 64.0f, 24.0f + y);
        textdraw_print("\x81m      \x81n");
    }
    else
    {
        textdraw_set_mul_color(0xFF7F7F00);
        textdraw_set_add_color(0);
    }
    textdraw_set_pos(x - 64.0f, 24.0f + y);
    textdraw_print(lbl_1_data_3AB74[locale]);
    if (selected == 1)
    {
        textdraw_set_mul_color(0xFFFFFF00);
        textdraw_set_add_color(color);
        textdraw_set_pos(64.0f + x, 24.0f + y);
        textdraw_print("\x81m      \x81n");
    }
    else
    {
        textdraw_set_mul_color(0xFF7F7F00);
        textdraw_set_add_color(0);
    }
    textdraw_set_pos(64.0f + x, 24.0f + y);
    textdraw_print(lbl_1_data_3AB98[locale]);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

u32 fn_1_C2478(void)
{
    float t;
    u8 c;

    t = (float)(g_font_char_var_3 % 60) / 60.0f;
    t = 1.0 - __fabs((float)(2.0 * t - 1.0));
    t = __fabs(t);
    t *= 255.0;
    f32_to_u8(&t, &c);
    return (c << 16) | (c << 8) | c;
}
