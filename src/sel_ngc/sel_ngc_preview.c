/* sel_ngc_preview.c: stage preview thumbnails on the menu (practice mode, the
 * fight party game and the dogfight/baseball/tennis party games) and the
 * per-game disc file lists that are queued for prefetch from the menu.
 *
 * The previews are struct Preview textures streamed from disc by the DOL's
 * preview API (g_load_preview_texture / fn_1_F9CAC / fn_1_F9CBC /
 * g_load_sprite_tex_from_dvd / fn_1_F9EAC / fn_1_F9E1C, src/main_loop/preview.c).
 * Sprites that show one store its struct Preview * in Sprite::tex. */
#include "sel_ngc.h"
#include "macros.h"

/* sel_ngc.c's work area and the sel.c globals, through the layouts declared in
 * sel_ngc.h (the extern declarations there still use u8). */
extern u8 lbl_3_bss_178[]; /* sel_ngc.c bss 0x178 (also in sel_ngc_data.h) */

#define menuWork ((struct SelNgcMenuWork *)&g_currently_visible_menu_screen)
#define menuCursor ((struct SelNgcBss178 *)lbl_3_bss_178)
#define rootMenu ((struct SelNgcRootMenu *)&g_focused_root_menu)
#define storySettings ((struct SelNgcStorySettings *)&stageselect_is_storymode)
#define fightSettings ((struct SelNgcSettings16DC *)&lbl_1_bss_16DC) /* unk5[i]: stage of fight round i */

#define MENUSCREEN_FIGHT_STAGE_SELECT 0x18

/* One fight-round thumbnail slot */
struct FightPreviewSlot
{
    /*0x00*/ s8 previewIdx; /* index into lbl_3_bss_248 currently shown */
    /*0x04*/ float highlight; /* 0..1, eases towards 1 while this round is selected */
}; /* size = 0x8 */

/* Files to prefetch from disc for one game */
struct PreviewFileList
{
    /*0x00*/ char **files;
    /*0x04*/ s32 count;
}; /* size = 0x8 */

/* Unreferenced table at the start of the TU's .data (no relocations point at
 * it; probably left over from an earlier menu layout). */
struct SelNgcUnusedEntry
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ s16 unk4;
    /*0x06*/ s16 unk6;
}; /* size = 0x8 */

void fn_3_1EF64(void); /* see the note at its prototype slot in sel_ngc.h */

/* ---- .bss ----
 * Defined before everything else. MWCC places the objects that functions/data
 * reference first in that order, and the rest in reverse definition order,
 * which is why the last four appear reversed here. */
struct Preview g_something_with_preview_textures; /* practice mode stage preview */
struct FightPreviewSlot lbl_3_bss_230[3];
struct Preview lbl_3_bss_248[3]; /* fight stage previews */
/* 0x18 unreferenced bytes end the TU's original .bss (0x630-0x648) */
u8 lbl_3_bss_7E8[0x18];
struct Preview lbl_3_bss_680[3]; /* dogfight previews */
struct Preview lbl_3_bss_518[3]; /* baseball previews */
struct Preview lbl_3_bss_3B0[3]; /* tennis previews */

/* ---- .data ---- */
s8 lbl_3_data_D310[19] = { -2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, -2, 13, 14, 15, -1 };
s8 lbl_3_data_D324[17] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1 };
struct SelNgcUnusedEntry lbl_3_data_D338[41] = {
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

/* ---- practice mode stage preview ---- */

/* Stage id shown for the cursor of the practice stage select (0: locked / none) */
#define STORY_WORLD ((s8)story->unk1[(s8)story->unk0])
#define STORY_STAGE ((s8)story->unk3[(s8)story->unk0][STORY_WORLD])

void sprite_practice_stage_preview_tick(s8 *status, struct Sprite *sprite)
{
    float opacity;
    float depth;
    struct SelNgcStorySettings *story = storySettings;
    struct Sprite *parent;
    int stageId;

    parent = get_sprite_with_unique_id(0x4F);
    if (parent != NULL)
    {
        if ((s8)story->unk0 == 1)
        {
            stageId = get_story_mode_stage_id(STORY_WORLD, STORY_STAGE);
            if (g_is_storymode_stage_unlocked(STORY_WORLD, STORY_STAGE) == 0)
                stageId = 0;
        }
        else
        {
            stageId = fn_1_A3C84(modeCtrl.difficulty, STORY_STAGE, modeCtrl.courseFlags);
            if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, STORY_STAGE + 1, modeCtrl.courseFlags) == 0)
                stageId = 0;
        }
        if (stageId != -1 && stageId != sprite->userVar)
        {
            fn_1_F9CAC(&g_something_with_preview_textures, stageId);
            sprite->userVar = stageId;
        }
        if (parent->userVar != 3)
            opacity = 0.0f;
        else
            opacity = 1.0f;
        sprite->opacity = opacity;
        if (stageId == 0)
            sprite->opacity = 0.0f;
        sprite->scaleX = 160.0f * sprite->opacity;
        sprite->scaleY = 112.0f * sprite->opacity;
        if (g_check_some_memcard_field() != 0)
            depth = 0.05f + sprite->unk58;
        else
            depth = sprite->unk58;
        sprite->depth = depth;
        g_load_sprite_tex_from_dvd((struct Preview *)sprite->tex);
    }
}

void sprite_practice_stage_preview_disp(struct Sprite *sprite)
{
    fn_1_F9EAC((struct Preview *)sprite->tex, sprite->x, sprite->y, sprite->depth, sprite->scaleX,
               sprite->scaleY, (u8)(s32)(255.0f * sprite->opacity) | 0xFF0000 | 0xFFFF, 0);
}

void sprite_practice_stage_preview_mask_disp(s8 *status, struct Sprite *sprite)
{
    float opacity;
    struct Sprite *parent;

    parent = get_sprite_with_unique_id(0x4F);
    if (parent != NULL)
    {
        if (parent->userVar != 3)
            opacity = 0.0f;
        else
            opacity = 1.0f;
        sprite->opacity = opacity;
    }
}

void create_practice_mode_preview_sprites(void)
{
    float x;
    float y;
    struct Sprite *parent;
    struct Sprite *sprite;

    parent = get_sprite_with_unique_id(0x4F);
    x = 145.0f + parent->x;
    y = 12.5 + parent->y;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        g_load_preview_texture(&g_something_with_preview_textures, "preview/replist160x112.tpl", 0, 160, 112, 5);
        sprite->tex = (struct SpriteTexture *)&g_something_with_preview_textures;
        sprite->type = 2;
        sprite->tag = 0x66;
        sprite->x = x - 80.0f;
        sprite->y = y - 56.5f;
        sprite->scaleX = 160.0f;
        sprite->scaleY = 112.0f;
        sprite->textAlign = 4;
        sprite->depth = parent->depth - 0.05;
        sprite->unk58 = sprite->depth;
        sprite->opacity = 1.0f;
        sprite->userVar = 0;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_practice_stage_preview_tick;
        sprite->drawFunc = sprite_practice_stage_preview_disp;
        sprintf(sprite->text, "prac_stage prev");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->tag = 0x66;
        sprite->bmpId = 0x4B;
        sprite->x = x;
        sprite->y = y;
        sprite->scaleX = 20.0f;
        sprite->scaleY = 14.0f;
        sprite->textAlign = 4;
        sprite->depth = parent->depth - 0.025;
        sprite->opacity = 1.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = sprite_practice_stage_preview_mask_disp;
        sprintf(sprite->text, "prac_stage prev mask");
    }
}

void fn_3_1E1AC(void)
{
    destroy_sprite_with_unique_id(0x66);
    fn_1_F9E1C(&g_something_with_preview_textures);
}

/* ---- fight party game stage previews ---- */

/* userVar: round index (0-2) for the thumbnail, +10 for its frame */
void fn_3_1E1DC(s8 *status, struct Sprite *sprite)
{
    struct SelNgcSettings16DC *settings = fightSettings;
    float t;
    struct Sprite *parent;
    int round;
    struct FightPreviewSlot *slot;
    u32 c;

    parent = get_sprite_with_unique_id(0x4E);
    round = sprite->userVar % 10;
    slot = &lbl_3_bss_230[round];
    if (round == (s8)settings->unk5[menuCursor->unk0])
        slot->highlight = slot->highlight + 0.5 * (1.0 - slot->highlight);
    else
        slot->highlight = slot->highlight + 0.5 * -slot->highlight;
    t = slot->highlight;
    if (sprite->userVar >= 10)
    {
        c = 128.0 + 127.0 * fabs(math_sin(g_font_char_var_3 * 0x180));
        if (menuWork->unk4 == 0 && menuWork->unk8 != 0)
            c = ((g_font_char_var_3 >> 2) & 1) * 0x7F + 0x80;
        if (round != (s8)settings->unk5[menuCursor->unk0])
            c = 0x40;
        sprite->addR = c;
        sprite->addG = sprite->addR;
        sprite->addB = 0;
    }
    else
    {
        sprite->mulR = 255.0f * t + 64.0 * (1.0 - t);
        sprite->mulG = 255.0f * t + 64.0 * (1.0 - t);
        sprite->mulB = 255.0f * t + 64.0 * (1.0 - t);
    }
    if (parent != NULL)
    {
        if (parent->userVar == 3)
        {
            sprite->opacity += 0.2f * (1.0f - sprite->opacity);
            return;
        }
        sprite->opacity = 0.0f;
    }
}

/* Stage thumbnails on the fight stage select screen; userVar: round index */
void fn_3_1E458(s8 *status, struct Sprite *sprite)
{
    struct Sprite *parent = get_sprite_with_unique_id(0x4E);
    struct SelNgcSettings16DC *settings = fightSettings;
    float offsets[3] = { 46.0f, 104.0f, 162.0f };
    int round;
    struct FightPreviewSlot *slot;
    s8 stage;
    u32 c;

    if (parent != NULL)
    {
        round = sprite->userVar;
        slot = &lbl_3_bss_230[round];
        stage = settings->unk5[round];
        if (stage != slot->previewIdx)
        {
            sprite->tex = (struct SpriteTexture *)&lbl_3_bss_248[stage];
            slot->previewIdx = stage;
        }
        sprite->x = parent->x + parent->scaleX * offsets[round];
        sprite->y = parent->y + 36.0f * parent->scaleY;
        sprite->scaleX = 0.3 * parent->scaleX;
        sprite->scaleY = 0.3 * parent->scaleX;
        if (round < menuCursor->unk0 || (menuWork->unk8 != 0 && menuWork->unk4 == 0))
        {
            sprite->mulR = 0xFF;
            sprite->mulG = sprite->mulR;
            sprite->mulB = sprite->mulR;
            sprite->addR = 0;
            sprite->addG = sprite->addR;
            sprite->addB = sprite->addR;
        }
        else
        {
            c = 64.0 * fabs(math_sin(g_font_char_var_3 * 0x180));
            if (round != menuCursor->unk0)
                c = 0;
            sprite->mulR = 0;
            sprite->mulG = sprite->mulR;
            sprite->mulB = sprite->mulR;
            sprite->addR = c;
            sprite->addG = sprite->addR;
            sprite->addB = 0;
        }
        if (parent->userVar != 3)
        {
            sprite->opacity = 0.0f;
            return;
        }
        sprite->opacity = 1.0f;
    }
}

void create_fight_stage_preview_sprites(void)
{
    struct SelNgcSettings16DC *settings = fightSettings;
    float x;
    float y;
    struct Sprite *parent;
    int i;
    struct Sprite *sprite;

    parent = get_sprite_with_unique_id(0x4E);
    for (i = 0; i < 3; i++)
    {
        struct Preview *preview = &lbl_3_bss_248[i];

        g_load_preview_texture(preview, "preview/fight160x112.tpl", i, 160, 112, 5);
        fn_1_F9CBC(preview);
    }
    for (i = 0; i < 3; i++)
    {
        float xs[3] = { 190.0f, 320.0f, 450.0f };

        x = xs[i];
        y = 205.0f;
        sprite = create_sprite();
        if (sprite != NULL)
        {
            lbl_3_bss_230[i].previewIdx = settings->unk5[i];
            sprite->tex = (struct SpriteTexture *)&lbl_3_bss_248[i];
            sprite->type = 2;
            sprite->tag = 0x66;
            sprite->x = x;
            sprite->y = y;
            sprite->scaleX = 0.7f;
            sprite->scaleY = 0.7f;
            sprite->userVar = i;
            sprite->textAlign = 4;
            sprite->depth = 0.07f;
            sprite->opacity = 0.0f;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = fn_3_1E1DC;
            sprintf(sprite->text, "fight_stage %d", i);
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            lbl_3_bss_230[i].previewIdx = settings->unk5[i];
            sprite->tex = (struct SpriteTexture *)&lbl_3_bss_248[i];
            sprite->type = 2;
            sprite->tag = 0x66;
            sprite->x = x;
            sprite->y = y;
            sprite->scaleX = 0.75f;
            sprite->scaleY = 0.77f;
            sprite->userVar = i + 10;
            sprite->textAlign = 4;
            sprite->depth = 0.08f;
            sprite->opacity = 0.0f;
            sprite->mulR = 0;
            sprite->mulG = 0;
            sprite->mulB = 0;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = fn_3_1E1DC;
            sprintf(sprite->text, "fight_stage %d waku", i);
        }
    }
    if (menuWork->currScreen != MENUSCREEN_FIGHT_STAGE_SELECT || parent == NULL)
        return;
    for (i = 0; i < 3; i++)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            s8 stage = settings->unk5[i];

            lbl_3_bss_230[i].previewIdx = stage;
            sprite->tex = (struct SpriteTexture *)&lbl_3_bss_248[stage];
            sprite->type = 2;
            sprite->tag = 0x66;
            sprite->x = 0.0f;
            sprite->y = 0.0f;
            sprite->scaleX = 0.0f;
            sprite->scaleY = 0.0f;
            sprite->userVar = i;
            sprite->textAlign = 4;
            sprite->depth = 0.1f;
            sprite->opacity = 1.0f;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = fn_3_1E458;
            sprintf(sprite->text, "fight_stage stock %d", i);
        }
    }
}

/* ---- dogfight / baseball / tennis previews (game 0 / 1 / 2) ---- */

const s8 lbl_3_rodata_980[3] = { 3, 3, 3 }; /* preview count per game */
struct Preview *lbl_3_data_D51C[3] = { lbl_3_bss_680, lbl_3_bss_518, lbl_3_bss_3B0 };
char *lbl_3_data_D548[3] = { "dogfight", "baseball", "tennis" };

void fn_3_1EA0C(void)
{
    struct Preview *preview;
    int i;

    destroy_sprite_with_unique_id(0x66);
    preview = lbl_3_bss_248;
    for (i = 0; i < 3; i++, preview++)
        fn_1_F9E1C(preview);
}

/* userVar: preview index, unk50: game; follows sprite 0x3C + game */
void fn_3_1EA6C(s8 *status, struct Sprite *sprite)
{
    float t;
    float opacity;
    int i;
    struct Sprite *parent;

    i = sprite->userVar;
    parent = get_sprite_with_unique_id(i + 0x3C);
    if (parent != NULL)
    {
        if (i == (s8)rootMenu->unk0[menuWork->currScreen])
            sprite->unk58 = sprite->unk58 + 0.5 * (1.0 - sprite->unk58);
        else
            sprite->unk58 = sprite->unk58 + 0.5 * -sprite->unk58;
        t = sprite->unk58;
        sprite->x = parent->x + 20.0f * (i - 1);
        sprite->y = parent->y;
        sprite->scaleX = 0.75f * parent->scaleX;
        sprite->scaleY = sprite->scaleX;
        if (parent->userVar == 0x1E)
            opacity = 1.0f;
        else if (parent->opacity * parent->scaleX < 1.0f)
            opacity = parent->opacity * parent->scaleX;
        else
            opacity = 1.0f;
        sprite->opacity = opacity;
        sprite->depth = parent->depth - 0.005f;
        sprite->mulR = (u8)(255.0f * t + 127.0 * (1.0 - t));
        sprite->mulG = (u8)(255.0f * t + 127.0 * (1.0 - t));
        sprite->mulB = (u8)(255.0f * t + 127.0 * (1.0 - t));
    }
}

void fn_3_1EC64(int game)
{
    char path[256];
    float x;
    float y;
    struct Sprite *parent;
    int i;
    struct Sprite *sprite;

    parent = get_sprite_with_unique_id(game + 0x3C);
    if (parent != NULL)
    {
        for (i = 0; i < lbl_3_rodata_980[game]; i++)
        {
            struct Preview *preview;

            sprintf(path, "preview/%s160x112.tpl", lbl_3_data_D548[game]);
            preview = &lbl_3_data_D51C[game][i];
            g_load_preview_texture(preview, path, i, 160, 112, 5);
            fn_1_F9CBC(preview);
        }
        for (i = 0; i < lbl_3_rodata_980[game]; i++)
        {
            x = parent->unk58;
            y = parent->y;
            sprite = fn_1_AED58(0x3C);
            if (sprite != NULL)
            {
                sprite->tex = (struct SpriteTexture *)&lbl_3_data_D51C[game][i];
                sprite->type = 2;
                sprite->tag = 0x66;
                sprite->x = x;
                sprite->y = y;
                sprite->userVar = i;
                sprite->unk50 = game;
                sprite->unk58 = 0.0f;
                sprite->textAlign = 4;
                sprite->depth = 0.07f;
                sprite->flags |= 0x01000000;
                sprite->widescreenArg = 0x140;
                sprite->mainFunc = fn_3_1EA6C;
                sprintf(sprite->text, "simple prev %d", i);
            }
        }
    }
}

/* 4 unreferenced zero bytes follow the last string literal in the original
 * .data (0x26C); nothing in the REL points at them. */
char lbl_3_data_D57C[4] = "";

void fn_3_1EE08(int game)
{
    struct Preview *preview;
    int i;

    destroy_sprite_with_unique_id(0x66);
    preview = lbl_3_data_D51C[game];
    for (i = 0; i < lbl_3_rodata_980[game]; i++, preview++)
        fn_1_F9E1C(preview);
}

/* ---- disc prefetch lists, indexed by party game ---- */
char *lbl_3_data_D878[] = {
    "snd/mkb/race.pool",
    "snd/mkb/race.proj",
    "snd/mkb/race.sdir",
    "snd/mkb/race.samp",
    "init/m_race.tpl",
    "init/m_race.gma",
    "bmp/bmp_rac.tpl",
    "ape/aiai_race.mta",
    "ape/aiai_face.mta",
    "ape/aiai_hand.mta",
    "ape/aiai_common.mal",
    "ape/aiai_race.mal",
    "ape/aiai_sl.tpl",
    "ape/aiai_sl.gma",
    "ape/aiai_rep.tpl",
    "ape/meemee_race.mta",
    "ape/meemee_face.mta",
    "ape/meemee_hand.mta",
    "ape/meemee_common.mal",
    "ape/meemee_race.mal",
    "ape/meemee_sl.tpl",
    "ape/meemee_sl.gma",
    "ape/meemee_rep.tpl",
    "ape/baby_race.mta",
    "ape/baby_face.mta",
    "ape/baby_hand.mta",
    "ape/baby_common.mal",
    "ape/baby_race.mal",
    "ape/baby_sl.tpl",
    "ape/baby_sl.gma",
    "ape/baby_rep.tpl",
    "ape/gongon_race.mta",
    "ape/gongon_face.mta",
    "ape/gongon_hand.mta",
    "ape/gongon_common.mal",
    "ape/gongon_race.mal",
    "ape/gongon_sl.tpl",
    "ape/gongon_sl.gma",
    "ape/gongon_rep.tpl",
};
char *lbl_3_data_DA90[] = {
    "bmp/bmp_nml.tpl",
    "bmp/bmp_fgt2.tpl",
    "init/m_fight2.tpl",
    "init/m_fight2.gma",
    "snd/mkb/fight.pool",
    "snd/mkb/fight.proj",
    "snd/mkb/fight.sdir",
    "snd/mkb/fight.samp",
    "ape/aiai_main.mal",
    "ape/aiai_l.tpl",
    "ape/aiai_l.gma",
    "ape/meemee_main.mal",
    "ape/meemee_l.tpl",
    "ape/meemee_l.gma",
    "ape/baby_main.mal",
    "ape/baby_l.tpl",
    "ape/baby_l.gma",
    "ape/gongon_main.mal",
    "ape/gongon_l.tpl",
    "ape/gongon_l.gma",
};
char *lbl_3_data_DC74[] = {
    "stage/STAGE155.lz",
    "stage/st155.tpl",
    "stage/st155.gma",
    "bg/bg_tar2.tpl",
    "bg/bg_tar2.gma",
    "mkb2.pilot2.rel",
    "snd/mkb/target.pool",
    "snd/mkb/target.proj",
    "snd/mkb/target.sdir",
    "snd/mkb/target.samp",
    "bmp/bmp_tgt.tpl",
    "init/m_tar2.tpl",
    "init/m_tar2.gma",
    "ape/aiai_pilot.mta",
    "ape/aiai_pilot.mal",
    "ape/meemee_pilot.mta",
    "ape/meemee_pilot.mal",
    "ape/baby_pilot.mta",
    "ape/baby_pilot.mal",
    "ape/gongon_pilot.mta",
    "ape/gongon_pilot.mal",
};
char *lbl_3_data_DE10[] = {
    "snd/mkb/bil.pool",
    "snd/mkb/bil.proj",
    "snd/mkb/bil.sdir",
    "snd/mkb/bil.samp",
    "bmp/bmp_bil.tpl",
    "init/m_bill2.tpl",
    "init/m_bill2.gma",
    "ape/aiai_bill.mta",
    "ape/aiai_face.mta",
    "ape/aiai_hand.mta",
    "ape/aiai_common.mal",
    "ape/aiai_sl.tpl",
    "ape/aiai_sl.gma",
    "ape/aiai_bill.mal",
    "ape/aiai_rep.tpl",
    "ape/meemee_bill.mta",
    "ape/meemee_face.mta",
    "ape/meemee_hand.mta",
    "ape/meemee_common.mal",
    "ape/meemee_sl.tpl",
    "ape/meemee_sl.gma",
    "ape/meemee_bill.mal",
    "ape/meemee_rep.tpl",
    "ape/baby_bill.mta",
    "ape/baby_face.mta",
    "ape/baby_hand.mta",
    "ape/baby_common.mal",
    "ape/baby_sl.tpl",
    "ape/baby_sl.gma",
    "ape/baby_bill.mal",
    "ape/baby_rep.tpl",
    "ape/gongon_bill.mta",
    "ape/gongon_face.mta",
    "ape/gongon_hand.mta",
    "ape/gongon_common.mal",
    "ape/gongon_sl.tpl",
    "ape/gongon_sl.gma",
    "ape/gongon_bill.mal",
    "ape/gongon_rep.tpl",
    "ape/aiai_sh.tpl",
    "ape/aiai_sh.gma",
};
char *lbl_3_data_E06C[] = {
    "snd/mkb/bowl.pool",
    "snd/mkb/bowl.proj",
    "snd/mkb/bowl.sdir",
    "snd/mkb/bowl.samp",
    "init/m_bowl2.tpl",
    "init/m_bowl2.gma",
    "bmp/bmp_bwl.tpl",
    "ape/aiai_bowl.mta",
    "ape/aiai_face.mta",
    "ape/aiai_hand.mta",
    "ape/aiai_common.mal",
    "ape/aiai_bowl.mal",
    "ape/aiai_h.tpl",
    "ape/aiai_h.gma",
    "ape/aiai_rep.tpl",
    "ape/meemee_bowl.mta",
    "ape/meemee_face.mta",
    "ape/meemee_hand.mta",
    "ape/meemee_common.mal",
    "ape/meemee_bowl.mal",
    "ape/meemee_h.tpl",
    "ape/meemee_h.gma",
    "ape/meemee_rep.tpl",
    "ape/baby_bowl.mta",
    "ape/baby_face.mta",
    "ape/baby_hand.mta",
    "ape/baby_common.mal",
    "ape/baby_bowl.mal",
    "ape/baby_h.tpl",
    "ape/baby_h.gma",
    "ape/baby_rep.tpl",
    "ape/gongon_bowl.mta",
    "ape/gongon_face.mta",
    "ape/gongon_hand.mta",
    "ape/gongon_common.mal",
    "ape/gongon_bowl.mal",
    "ape/gongon_h.tpl",
    "ape/gongon_h.gma",
    "ape/gongon_rep.tpl",
};
char *lbl_3_data_E1C0[] = {
    "snd/mkb/golf.pool",
    "snd/mkb/golf.proj",
    "snd/mkb/golf.sdir",
    "snd/mkb/golf.samp",
    "init/m_golf.tpl",
    "init/m_golf.gma",
    "bmp/bmp_glf2.tpl",
    "stage/STAGE170.lz",
    "stage/st170.tpl",
    "stage/st170.gma",
};
char *lbl_3_data_E350[] = {
    "snd/mkb/boat.pool",
    "snd/mkb/boat.proj",
    "snd/mkb/boat.sdir",
    "snd/mkb/boat.samp",
    "init/m_boa2.tpl",
    "init/m_boa2.gma",
    "bmp/bmp_boa.tpl",
    "ape/aiai_boat.mta",
    "ape/aiai_face.mta",
    "ape/aiai_hand.mta",
    "ape/aiai_common.mal",
    "ape/aiai_boat.mal",
    "ape/aiai_sm.tpl",
    "ape/aiai_sm.gma",
    "ape/aiai_sl.tpl",
    "ape/aiai_sl.gma",
    "ape/aiai_rep.tpl",
    "ape/meemee_boat.mta",
    "ape/meemee_face.mta",
    "ape/meemee_hand.mta",
    "ape/meemee_common.mal",
    "ape/meemee_boat.mal",
    "ape/meemee_sm.tpl",
    "ape/meemee_sm.gma",
    "ape/meemee_sl.tpl",
    "ape/meemee_sl.gma",
    "ape/meemee_rep.tpl",
    "ape/baby_boat.mta",
    "ape/baby_face.mta",
    "ape/baby_hand.mta",
    "ape/baby_common.mal",
    "ape/baby_boat.mal",
    "ape/baby_sl.tpl",
    "ape/baby_sl.gma",
    "ape/baby_rep.tpl",
    "ape/gongon_boat.mta",
    "ape/gongon_face.mta",
    "ape/gongon_hand.mta",
    "ape/gongon_common.mal",
    "ape/gongon_boat.mal",
    "ape/gongon_sl.tpl",
    "ape/gongon_sl.gma",
    "ape/gongon_rep.tpl",
};
char *lbl_3_data_E5B0[] = {
    "snd/mkb/gun.pool",
    "snd/mkb/gun.proj",
    "snd/mkb/gun.sdir",
    "snd/mkb/gun.samp",
    "init/efcmdl.tpl",
    "init/efcmdl.gma",
    "ape/aiai_guns.mta",
    "ape/aiai_face.mta",
    "ape/aiai_hand.mta",
    "ape/aiai_common.mal",
    "ape/aiai_guns.mal",
    "ape/meemee_guns.mta",
    "ape/meemee_face.mta",
    "ape/meemee_hand.mta",
    "ape/meemee_common.mal",
    "ape/meemee_guns.mal",
    "ape/baby_guns.mta",
    "ape/baby_face.mta",
    "ape/baby_hand.mta",
    "ape/baby_common.mal",
    "ape/baby_guns.mal",
    "ape/gongon_guns.mta",
    "ape/gongon_face.mta",
    "ape/gongon_hand.mta",
    "ape/gongon_common.mal",
    "ape/gongon_guns.mal",
    "ape/aiai_rep.tpl",
    "ape/meemee_rep.tpl",
    "ape/baby_rep.tpl",
    "ape/gongon_rep.tpl",
    "bmp/bmp_gun2.tpl",
    "ape/aiai_m.tpl",
    "ape/aiai_m.gma",
    "ape/meemee_m.tpl",
    "ape/meemee_m.gma",
    "ape/baby_m.tpl",
    "ape/baby_m.gma",
    "ape/gongon_m.tpl",
    "ape/gongon_m.gma",
};
char *lbl_3_data_E770[] = {
    "snd/mkb/dog.pool",
    "snd/mkb/dog.proj",
    "snd/mkb/dog.sdir",
    "snd/mkb/dog.samp",
    "ape/aiai_dogf.mta",
    "ape/aiai_face.mta",
    "ape/aiai_hand.mta",
    "ape/aiai_common.mal",
    "ape/aiai_dogf.mal",
    "ape/meemee_dogf.mta",
    "ape/meemee_face.mta",
    "ape/meemee_hand.mta",
    "ape/meemee_common.mal",
    "ape/meemee_dogf.mal",
    "ape/baby_dogf.mta",
    "ape/baby_face.mta",
    "ape/baby_hand.mta",
    "ape/baby_common.mal",
    "ape/baby_dogf.mal",
    "ape/gongon_dogf.mta",
    "ape/gongon_face.mta",
    "ape/gongon_hand.mta",
    "ape/gongon_common.mal",
    "ape/gongon_dogf.mal",
    "ape/aiai_rep.tpl",
    "ape/meemee_rep.tpl",
    "ape/baby_rep.tpl",
    "ape/gongon_rep.tpl",
    "bmp/bmp_dog2.tpl",
    "init/m_dog2.tpl",
    "init/m_dog2.gma",
    "ape/aiai_sl.tpl",
    "ape/aiai_sl.gma",
    "ape/meemee_sl.tpl",
    "ape/meemee_sl.gma",
    "ape/baby_sl.tpl",
    "ape/baby_sl.gma",
    "ape/gongon_sl.tpl",
    "ape/gongon_sl.gma",
};
char *lbl_3_data_E940[] = {
    "snd/mkb/futsal.pool",
    "snd/mkb/futsal.proj",
    "snd/mkb/futsal.sdir",
    "snd/mkb/futsal.samp",
    "init/m_fut2.tpl",
    "init/m_fut2.gma",
    "bmp/bmp_fut2.tpl",
    "ape/aiai_futsal.mta",
    "ape/aiai_face.mta",
    "ape/aiai_hand.mta",
    "ape/aiai_common.mal",
    "ape/aiai_futsal.mal",
    "ape/aiai_sl.tpl",
    "ape/aiai_sl.gma",
    "ape/aiai_rep.tpl",
    "ape/meemee_futsal.mta",
    "ape/meemee_face.mta",
    "ape/meemee_hand.mta",
    "ape/meemee_common.mal",
    "ape/meemee_futsal.mal",
    "ape/meemee_sl.tpl",
    "ape/meemee_sl.gma",
    "ape/meemee_rep.tpl",
    "ape/baby_futsal.mta",
    "ape/baby_face.mta",
    "ape/baby_hand.mta",
    "ape/baby_common.mal",
    "ape/baby_futsal.mal",
    "ape/baby_sl.tpl",
    "ape/baby_sl.gma",
    "ape/baby_rep.tpl",
    "ape/gongon_futsal.mta",
    "ape/gongon_face.mta",
    "ape/gongon_hand.mta",
    "ape/gongon_common.mal",
    "ape/gongon_futsal.mal",
    "ape/gongon_sl.tpl",
    "ape/gongon_sl.gma",
    "ape/gongon_rep.tpl",
};
char *lbl_3_data_EB88[] = {
    "mkb2.baseball.rel",
    "snd/mkbbball.pool",
    "snd/mkbbball.proj",
    "snd/mkbbball.samp",
    "snd/mkbbball.sdir",
    "stage/mdl_banana_st.tpl",
    "stage/mdl_banana_st.gma",
    "stage/bball_obj.tpl",
    "stage/bball_obj.gma",
    "bmp/bmp_bbl.tpl",
    "tennis/bt_effect.tpl",
    "tennis/bt_effect.gma",
    "ape/aiai_bball.mta",
    "ape/aiai_bball.mal",
    "ape/meemee_bball.mta",
    "ape/meemee_bball.mal",
    "ape/baby_bball.mta",
    "ape/baby_bball.mal",
    "ape/gongon_bball.mta",
    "ape/gongon_bball.mal",
};
char *lbl_3_data_ECCC[] = {
    "tennis/bt_effect.tpl",
    "tennis/bt_effect.gma",
    "tennis/mdl_tennis.tpl",
    "tennis/mdl_tennis.gma",
    "ape/aiai_tennis.mta",
    "ape/aiai_tennis.mal",
    "ape/meemee_tennis.mta",
    "ape/meemee_tennis.mal",
    "ape/baby_tennis.mta",
    "ape/baby_tennis.mal",
    "ape/gongon_tennis.mta",
    "ape/gongon_tennis.mal",
    "ape/aiai_face.mta",
    "ape/aiai_hand.mta",
    "ape/aiai_common.mal",
    "ape/aiai_h.tpl",
    "ape/aiai_h.gma",
    "ape/aiai_rep.tpl",
    "ape/meemee_face.mta",
    "ape/meemee_hand.mta",
    "ape/meemee_common.mal",
    "ape/meemee_h.tpl",
    "ape/meemee_h.gma",
    "ape/meemee_rep.tpl",
    "ape/baby_face.mta",
    "ape/baby_hand.mta",
    "ape/baby_common.mal",
    "ape/baby_h.tpl",
    "ape/baby_h.gma",
    "ape/baby_rep.tpl",
    "ape/gongon_face.mta",
    "ape/gongon_hand.mta",
    "ape/gongon_common.mal",
    "ape/gongon_h.tpl",
    "ape/gongon_h.gma",
    "ape/gongon_rep.tpl",
    "bmp/bmp_tennis.tpl",
};
struct PreviewFileList lbl_3_data_ED60[12] = {
    { lbl_3_data_D878, ARRAY_COUNT(lbl_3_data_D878) },
    { lbl_3_data_DA90, ARRAY_COUNT(lbl_3_data_DA90) },
    { lbl_3_data_DC74, ARRAY_COUNT(lbl_3_data_DC74) },
    { lbl_3_data_DE10, ARRAY_COUNT(lbl_3_data_DE10) },
    { lbl_3_data_E06C, ARRAY_COUNT(lbl_3_data_E06C) },
    { lbl_3_data_E1C0, ARRAY_COUNT(lbl_3_data_E1C0) },
    { lbl_3_data_E350, ARRAY_COUNT(lbl_3_data_E350) },
    { lbl_3_data_E5B0, ARRAY_COUNT(lbl_3_data_E5B0) },
    { lbl_3_data_E770, ARRAY_COUNT(lbl_3_data_E770) },
    { lbl_3_data_E940, ARRAY_COUNT(lbl_3_data_E940) },
    { lbl_3_data_EB88, ARRAY_COUNT(lbl_3_data_EB88) },
    { lbl_3_data_ECCC, ARRAY_COUNT(lbl_3_data_ECCC) },
};

void fn_3_1EE90(int game)
{
    char **files;
    int oldGroup;
    int count;
    int i;

    if (game < 0 || (u32)game >= 12)
        return;
    oldGroup = g_swapDiscQueueGroup(2);
    files = lbl_3_data_ED60[game].files;
    count = lbl_3_data_ED60[game].count;
    for (i = 0; i < count; i++, files++)
        queue_disc_read(*files);
    g_swapDiscQueueGroup(oldGroup);
}

void fn_3_1EF40(void)
{
    disc_queue_status(2);
}

void fn_3_1EF64(void)
{
    set_render_mode(modeCtrl.wideMode);
    camera_update_aspect_ratios();
}
