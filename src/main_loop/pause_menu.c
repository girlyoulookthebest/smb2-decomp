/* The in-game pause menu: opening it, navigation, and what each item does. */
#include "types.h"
#include "ball.h"
#include "dip.h"
#include "input.h"
#include "libc.h"
#include "light.h"
#include "mode.h"
#include "sprite.h"

struct PauseMenuState
{
    /*0x00*/ s32 timer;  /* counts down to activating the selected item */
    /*0x04*/ u32 flags;
    /*0x08*/ s32 selection;
    /*0x0C*/ s32 itemCount;
    /*0x10*/ s32 menuType;
    /*0x14*/ s8 padId;
    /*0x15*/ s8 playerId;
    /*0x16*/ s16 unk16;
    /*0x18*/ s8 screenshotState;
};

struct ScreenFadeInfo
{
    /*0x00*/ s32 type;
    /*0x04*/ u32 color;
    /*0x08*/ s32 timer;
    /*0x0C*/ s32 timerMax;
};

/* Mirrors the request memcard.c consumes. */
struct PauseMemcardRequest
{
    /*0x00*/ u8 file;
    /*0x04*/ u32 flags;
    /*0x08*/ char *comment;
    /*0x0C*/ void *buffer;
    /*0x10*/ u32 size;
    /*0x14*/ char *name;
};

struct PauseReplayInfo
{
    u8 filler0[0xC];
};

extern s32 main_mode_request;
extern s32 sub_mode_request;
extern s8 lbl_1_bss_176A;
extern struct { u8 filler0[8]; u32 flags; } lbl_1_bss_16D0;
extern s8 lbl_1_bss_1700;
extern struct { u8 filler0[4]; u16 flags; } lbl_1_bss_1708;
extern int stage_heap;
extern struct ScreenFadeInfo g_screenfade_flags;
extern u32 lbl_1_bss_6FB9C;
extern u8 storymode_save_files[];
extern s8 g_last_used_memcard_slot_2;
extern s32 g_some_pausemenu_var4;

struct Sprite *get_sprite_with_unique_id(int);
void destroy_sprite_with_unique_id(int);
void create_pausemenu_sprite(void);
void g_fade_track_volume(int, int);
int call_SoundReqID_arg_1(u32);
void g_load_stage_for_menu_bg(s8, int);
void g_return_to_sel_ngc(int);
int OSSetCurrentHeap(int);
void OSReport(const char *, ...);
void load_bmp_by_id(int);
void call_free_bmp_by_id(int);
void create_how_to_sprite(void);
void sprite_how_to_tick(int, struct Sprite *);
void event_init(int);
void event_dest(int);
void *g_some_replay_func(void **, u32 *);
void g_some_replay_func2(struct PauseReplayInfo *);
char *g_some_replay_func3(struct PauseReplayInfo *, char *);
void g_some_replay_func4(u8, u8, struct PauseMemcardRequest *);
int g_some_replay_func5(void);
void g_save_storymode_progress(int);
int get_goal_score(u32 *, int *);
void set_storymode_score(int);
void set_storymode_bananas(int);
int g_get_next_stage_id(void);
s16 play_track_and_fade_out_other_tracks(int, int, int);
u8 g_check_some_memcard_field(void);
int g_get_some_memcard_var(void);
int g_get_result_code(void);
void fn_1_F08C8(void);
void compare_play_points_with_99999_after_exit_game(void);
void g_display_playpoint_or_gift_message(int, float, float);
int fn_1_16C910(void);
void fn_1_16C59C(void);
void g_save_game_data(void);
void g_sprintf_memcard_error(int, char *);
int fn_2_8D18(void);
int fn_2_8D2C(void);
void g_something_with_pause_menu_and_game_over_sprite(int, struct Sprite *);

#define PAD_REPEAT(pad, btn) \
( \
    (pad_status_groups[pad].unk0[4].button & (btn)) \
 || (analog_inputs[pad][4] & (btn)) \
 || ( \
        ((pad_status_groups[pad].unk0[0].button & (btn)) || (analog_inputs[pad][0] & (btn))) \
     && (analog_inputs[pad][0] & PAD_BUTTON_B) \
    ) \
)

extern struct PauseMenuState g_related_to_pause_menu_input;

s8 g_repause_cooldown_counter;

const s8 pausemenu_entry_counts[8] = { 3, 4, 6, 4, 4, 4, 6, 3 };

#ifndef __MWERKS__
/* PC port: a "Quit to Desktop" row after the game's own rows (pc_quit.c).
 * pc_pause_quit_row is its index, so the game's rows keep their indices and
 * PAUSE_GAME_ITEMS is the original item count ("Exit game" = last of those). */
extern s32 pc_pause_quit_row;
void pc_quit_confirm_open(void);
void pc_quit_confirm_close(void);
int pc_quit_confirm_active(void);
int pc_quit_confirm_tick(unsigned int press);
#define PCQ_MOVED 1
#define PCQ_NO 2
#define PCQ_YES 3
#define PAUSE_GAME_ITEMS (pc_pause_quit_row >= 0 ? pc_pause_quit_row : g_related_to_pause_menu_input.itemCount)
#else
#define PAUSE_GAME_ITEMS g_related_to_pause_menu_input.itemCount
#endif

static int is_pausing_allowed(void)
{
    if (g_screenfade_flags.timer != 0)
        return FALSE;
    if (g_related_to_pause_menu_input.flags & 2)
        return FALSE;
    switch (sub_mode)
    {
    case 0x3C: case 0x3D: case 0x3E: case 0x3F: case 0x40:
    case 0x41: case 0x42: case 0x43: case 0x44: case 0x45:
    case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D:
    case 0x4E: case 0x4F: case 0x50: case 0x51: case 0x52:
    case 0x55: case 0x56:
    case 0x5A: case 0x5B:
        return FALSE;
    case 0x58:
        if (fn_2_8D18() == 0 && fn_2_8D2C() != 0)
            break;
        return FALSE;
    case 0x53:
    case 0x54:
        if (!(modeCtrl.courseFlags & 0x40000))
            return FALSE;
        break;
    }
    switch (sub_mode_request)
    {
    case 0x3C: case 0x3D: case 0x3E: case 0x3F: case 0x40:
    case 0x41: case 0x42: case 0x43: case 0x44: case 0x45:
    case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D:
    case 0x4E: case 0x4F: case 0x50: case 0x51: case 0x52:
    case 0x55: case 0x56:
    case 0x5A: case 0x5B:
        return FALSE;
    case 0x58:
        if (fn_2_8D18() == 0 && fn_2_8D2C() != 0)
            break;
        return FALSE;
    case 0x53:
    case 0x54:
        if (!(modeCtrl.courseFlags & 0x40000))
            return FALSE;
        break;
    }
    if (g_check_flag_related_to_pausing() != 0)
        return FALSE;
    return TRUE;
}

int should_open_pause_menu(void)
{
    if (!is_pausing_allowed())
        return FALSE;
    if (g_repause_cooldown_counter > 0)
        return FALSE;
    if (dip_switches & 1)
    {
        if (!(analog_inputs[0][0] & PAD_BUTTON_A)
         && !(analog_inputs[0][0] & PAD_BUTTON_B)
         && lbl_1_bss_6FB9C == 0
         && !(g_some_other_flags & 8)
         && (merged_digital_inputs[2] & 0x1000))
            return TRUE;
    }
    else
    {
        if (!(g_some_other_flags & 8) && (merged_digital_inputs[2] & 0x1000))
            return TRUE;
    }
    return FALSE;
}

struct PauseMenuState g_related_to_pause_menu_input;
s8 lbl_1_bss_13E8;
s32 pause_action;
s8 exit_save_stage;
s32 lbl_1_bss_13F4;
u8 lbl_1_bss_13F8[8];

void construct_pause_menu_sprite(struct Sprite *sprite)
{
    int i;

    g_some_other_flags |= 8;
    g_related_to_pause_menu_input.padId = 0;
    for (i = 0; i < 4; i++)
    {
        if (pad_status_groups[i].unk0[0].err == 0
         && (pad_status_groups[i].unk0[2].button & 0x1000))
        {
            g_related_to_pause_menu_input.padId = i;
            break;
        }
    }

    lbl_1_bss_13E8 = 0;
    g_repause_cooldown_counter = 30;
    g_related_to_pause_menu_input.playerId = 0;
    if (modeCtrl.unk40 == 1)
    {
        for (i = 0; i < modeCtrl.playerCount; i++)
        {
            if (g_related_to_pause_menu_input.padId == player_pad_map[i])
            {
                g_related_to_pause_menu_input.playerId = i;
                break;
            }
        }
    }
    else
    {
        if (modeCtrl.gameType == 6)
        {
            if (modeCtrl.playerCount == 1)
                g_related_to_pause_menu_input.playerId = 0;
            else if (g_related_to_pause_menu_input.unk16 == -1)
                g_related_to_pause_menu_input.playerId = 0;
            else
                g_related_to_pause_menu_input.playerId = (g_related_to_pause_menu_input.padId == player_pad_map[lbl_1_bss_176A]) ? lbl_1_bss_176A : 1 - lbl_1_bss_176A;
        }
        else
            g_related_to_pause_menu_input.playerId = modeCtrl.currPlayer;
    }

    switch (modeCtrl.gameType)
    {
    case 0:
        switch (sub_mode)
        {
        case 0x53:
        case 0x54:
            g_related_to_pause_menu_input.menuType = 7;
            break;
        default:
            g_related_to_pause_menu_input.menuType = 1;
            break;
        }
        break;
    case 1:
        g_related_to_pause_menu_input.menuType = 1;
        break;
    case 2:
        g_related_to_pause_menu_input.menuType = 2;
        break;
    case 3:
        if (lbl_1_bss_16D0.flags & 0x10)
            g_related_to_pause_menu_input.menuType = 3;
        else if (!(lbl_1_bss_16D0.flags & 8))
            g_related_to_pause_menu_input.menuType = 3;
        else
            g_related_to_pause_menu_input.menuType = 0;
        break;
    case 6:
        if (g_related_to_pause_menu_input.unk16 == g_related_to_pause_menu_input.padId)
            g_related_to_pause_menu_input.menuType = 4;
        else
            g_related_to_pause_menu_input.menuType = 0;
        break;
    case 7:
        if (lbl_1_bss_1700 == 1)
            g_related_to_pause_menu_input.menuType = 3;
        else
            g_related_to_pause_menu_input.menuType = 0;
        break;
    case 10:
        if (lbl_1_bss_1708.flags & 1)
            g_related_to_pause_menu_input.menuType = 3;
        else if (!(lbl_1_bss_1708.flags & 0x10))
            g_related_to_pause_menu_input.menuType = 3;
        else
            g_related_to_pause_menu_input.menuType = 0;
        break;
    case 14:
        switch (sub_mode)
        {
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5E:
            g_related_to_pause_menu_input.menuType = 5;
            break;
        default:
            g_related_to_pause_menu_input.menuType = 6;
            break;
        }
        break;
    default:
        g_related_to_pause_menu_input.menuType = 0;
        break;
    }

    g_related_to_pause_menu_input.flags &= ~1;
    g_related_to_pause_menu_input.selection = 0;
    g_related_to_pause_menu_input.itemCount = pausemenu_entry_counts[g_related_to_pause_menu_input.menuType];
#ifndef __MWERKS__
    pc_pause_quit_row = g_related_to_pause_menu_input.itemCount++;
    pc_quit_confirm_close();
#endif
    g_related_to_pause_menu_input.timer = 0;
    pause_action = -1;
    exit_save_stage = 0;
    if (sprite == NULL)
    {
        create_pausemenu_sprite();
        get_sprite_with_unique_id(4);
    }
    else
        sprite->userVar = 1;
    call_SoundReqID_arg_1(0x70);
    g_fade_track_volume(50, 10);
}

#ifndef __MWERKS__
/* The Quit to Desktop yes/no box: left/right pick, A confirms, B/START
 * cancel (back to the menu on the Quit row).  TRUE while it is open. */
static int pc_pause_quit_confirm_input(void)
{
    int pad = g_related_to_pause_menu_input.padId;
    unsigned int press;

    if (!pc_quit_confirm_active())
        return FALSE;
    press = pad_status_groups[pad].unk0[2].button & (PAD_BUTTON_A | PAD_BUTTON_B | 0x1000);
    if (PAD_REPEAT(pad, PAD_BUTTON_LEFT))
        press |= PAD_BUTTON_LEFT;
    if (PAD_REPEAT(pad, PAD_BUTTON_RIGHT))
        press |= PAD_BUTTON_RIGHT;
    switch (pc_quit_confirm_tick(press))
    {
    case PCQ_MOVED:
        call_SoundReqID_arg_1(0x6F);
        break;
    case PCQ_NO:
        call_SoundReqID_arg_1(0x70);
        break;
    case PCQ_YES:
        /* freeze the menu (selected row flashes) until pc_quit_poll exits */
        call_SoundReqID_arg_1(0x6E);
        g_related_to_pause_menu_input.flags |= 1;
        break;
    }
    return TRUE;
}
#endif

void check_pause_menu_input(struct Sprite *sprite)
{
    s8 prevSelection = g_related_to_pause_menu_input.selection;

#ifndef __MWERKS__
    if (pc_pause_quit_confirm_input())
        return;
#endif

    if (lbl_1_bss_13E8 > 0)
        lbl_1_bss_13E8--;
    if (PAD_REPEAT(g_related_to_pause_menu_input.padId, PAD_BUTTON_UP) && lbl_1_bss_13E8 == 0)
    {
        if (--g_related_to_pause_menu_input.selection < 0)
            g_related_to_pause_menu_input.selection = g_related_to_pause_menu_input.itemCount - 1;
        lbl_1_bss_13E8 = 10;
    }
    else if (PAD_REPEAT(g_related_to_pause_menu_input.padId, PAD_BUTTON_DOWN) && lbl_1_bss_13E8 == 0)
    {
        if (++g_related_to_pause_menu_input.selection > g_related_to_pause_menu_input.itemCount - 1)
            g_related_to_pause_menu_input.selection = 0;
        lbl_1_bss_13E8 = 10;
    }
    if (prevSelection != g_related_to_pause_menu_input.selection)
        call_SoundReqID_arg_1(0x6F);
    if (g_related_to_pause_menu_input.menuType == 4
     && g_related_to_pause_menu_input.selection == 1)
    {
        if (PAD_REPEAT(g_related_to_pause_menu_input.padId, PAD_BUTTON_LEFT)
         || PAD_REPEAT(g_related_to_pause_menu_input.padId, PAD_BUTTON_RIGHT))
        {
            g_related_to_pause_menu_input.flags |= 0x10;
            call_SoundReqID_arg_1(0x169);
        }
    }
    if ((pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & PAD_BUTTON_A)
     || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & PAD_BUTTON_B)
     || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000))
    {
#ifndef __MWERKS__
        if (g_related_to_pause_menu_input.selection == pc_pause_quit_row
         && !(pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & PAD_BUTTON_B)
         && !(pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000))
        {
            pc_quit_confirm_open();
            call_SoundReqID_arg_1(0x6E);
            return;
        }
#endif
        g_related_to_pause_menu_input.flags |= 1;
        call_SoundReqID_arg_1(0x6E);
        g_related_to_pause_menu_input.timer = 2;
        if ((pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & PAD_BUTTON_B)
         || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000))
            g_related_to_pause_menu_input.selection = 0;
        switch (g_related_to_pause_menu_input.menuType)
        {
        case 2:
            if (g_related_to_pause_menu_input.selection == PAUSE_GAME_ITEMS - 2)
            {
                g_related_to_pause_menu_input.timer = 60;
                if (sprite != NULL)
                    sprite->userVar = 2;
                g_screenfade_flags.color = 0xFFFFFF;
                g_fade_track_volume(59, 2);
                g_load_stage_for_menu_bg(-2, 1);
            }
            break;
        case 6:
            if (g_related_to_pause_menu_input.selection == PAUSE_GAME_ITEMS - 2)
            {
                g_related_to_pause_menu_input.timer = 60;
                if (sprite != NULL)
                    sprite->userVar = 2;
                g_screenfade_flags.color = 0xFFFFFF;
                g_fade_track_volume(59, 2);
            }
            break;
        }
        if (PAUSE_GAME_ITEMS >= 2
         && g_related_to_pause_menu_input.selection == PAUSE_GAME_ITEMS - 1)
        {
            g_related_to_pause_menu_input.timer = 60;
            if (sprite != NULL)
                sprite->userVar = 3;
            g_fade_track_volume(60, 2);
            g_load_stage_for_menu_bg(-1, 1);
        }
    }
}

static inline void pause_resume(void)
{
    pause_action = 0;
    destroy_sprite_with_unique_id(4);
    g_fade_track_volume(100, 10);
}

static inline void pause_exit(void)
{
    pause_action = 1;
    g_return_to_sel_ngc(-1);
}

static inline void pause_how_to_play(struct Sprite *sprite)
{
    pause_action = 4;
    if (sprite != NULL)
        sprite->userVar = 6;
    if (main_mode == 4 && modeCtrl.gameType == 8)
    {
        int heap = OSSetCurrentHeap(stage_heap);
        load_bmp_by_id(0xC);
        OSSetCurrentHeap(heap);
    }
    else
        load_bmp_by_id(0xC);
    create_how_to_sprite();
    g_some_other_flags |= 8;
}

static inline void pause_view_or_save_replay(struct Sprite *sprite)
{
    if (g_related_to_pause_menu_input.flags & 4)
    {
        void *data;
        u32 size;
        struct PauseReplayInfo info;
        struct PauseMemcardRequest req;
        char name[0x40];

        pause_action = 3;
        g_some_other_flags |= 8;
        g_some_replay_func(&data, &size);
        g_some_replay_func2(&info);
        g_some_replay_func3(&info, name);
        if (data != NULL)
        {
            memset(&req, 0, sizeof(req));
            req.file = 2;
            req.name = name;
            req.comment = (char *)data + 0x24;
            req.size = size;
            req.buffer = data;
            g_some_replay_func4(0, 4, &req);
        }
    }
    else
    {
        pause_action = 2;
        g_some_other_flags |= 8;
        event_init(0xD);
        if (sprite != NULL)
            sprite->unk8C |= 1;
    }
}

void handle_paused_input_in_menu(struct Sprite *sprite)
{
    g_some_other_flags &= ~8;
    switch (g_related_to_pause_menu_input.menuType)
    {
    case 0:
        switch (g_related_to_pause_menu_input.selection)
        {
        case 0:
            pause_resume();
            if (modeCtrl.gameType == 6)
                g_related_to_pause_menu_input.flags |= 0x20;
            break;
        case 1:
            pause_how_to_play(sprite);
            break;
        case 2:
            pause_exit();
            break;
        }
        break;
    case 1:
        switch (g_related_to_pause_menu_input.selection)
        {
        case 0:
            pause_resume();
            break;
        case 1:
            pause_view_or_save_replay(sprite);
            break;
        case 2:
            pause_how_to_play(sprite);
            break;
        case 3:
            if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1)
            {
                pause_action = 5;
                sub_mode_request = 0x4F;
            }
            else
                pause_exit();
            break;
        }
        break;
    case 2:
        switch (g_related_to_pause_menu_input.selection)
        {
        case 0:
            pause_resume();
            break;
        case 1:
            mode_info.attempts++;
            BALL_FOREACH( ball->flags |= 0x8000000000ULL; )
            sub_mode_request = 0x30;
            g_fade_track_volume(100, 10);
            break;
        case 2:
            pause_view_or_save_replay(sprite);
            break;
        case 3:
            pause_how_to_play(sprite);
            break;
        case 4:
            modeCtrl.courseFlags |= 0x8000;
            pause_exit();
            break;
        case 5:
            pause_exit();
            break;
        }
        break;
    case 3:
        switch (g_related_to_pause_menu_input.selection)
        {
        case 0:
            pause_resume();
            break;
        case 1:
            destroy_sprite_with_unique_id(4);
            modeCtrl.courseFlags |= 0x4000;
            g_fade_track_volume(100, 10);
            break;
        case 2:
            pause_how_to_play(sprite);
            break;
        case 3:
            pause_exit();
            break;
        }
        break;
    case 4:
        switch (g_related_to_pause_menu_input.selection)
        {
        case 0:
            pause_resume();
            g_related_to_pause_menu_input.flags |= 0x20;
            break;
        case 1:
            pause_resume();
            g_related_to_pause_menu_input.flags |= 0x20;
            break;
        case 2:
            pause_how_to_play(sprite);
            break;
        case 3:
            pause_exit();
            break;
        }
        break;
    case 5:
        switch (g_related_to_pause_menu_input.selection)
        {
        case 0:
            pause_resume();
            break;
        case 1:
            g_save_storymode_progress(0);
            pause_action = 8;
            g_some_other_flags |= 8;
            if (exit_save_stage == 0)
                exit_save_stage = 1;
            lbl_1_bss_13F4 = 0;
            break;
        case 2:
            pause_how_to_play(sprite);
            break;
        case 3:
            g_save_storymode_progress(0);
            modeCtrl.courseFlags |= 0x8000000;
            pause_action = 5;
            sub_mode_request = 0x4F;
            break;
        }
        break;
    case 6:
        switch (g_related_to_pause_menu_input.selection)
        {
        case 0:
            pause_resume();
            break;
        case 1:
            mode_info.attempts++;
            BALL_FOREACH( ball->flags |= 0x8000000000ULL; )
            sub_mode_request = 0x30;
            g_fade_track_volume(100, 10);
            break;
        case 2:
            pause_view_or_save_replay(sprite);
            break;
        case 3:
            pause_how_to_play(sprite);
            break;
        case 4:
            pause_action = 6;
            mode_info.flags &= ~0x10;
            main_mode_request = 2;
            sub_mode_request = 0x59;
            if (modeCtrl.unk43 != 0)
            {
                struct Ball *ball = &balls[modeCtrl.currPlayer];

                set_storymode_score(ball->score + get_goal_score(NULL, NULL));
                set_storymode_bananas(ball->bananas);
                if (g_get_next_stage_id() >= 0)
                    play_track_and_fade_out_other_tracks(0, 60, 100);
            }
            else
                play_track_and_fade_out_other_tracks(0, 60, 100);
            break;
        case 5:
            g_save_storymode_progress(0);
            modeCtrl.courseFlags |= 0x8000000;
            pause_action = 5;
            sub_mode_request = 0x4F;
            break;
        }
        break;
    case 7:
        switch (g_related_to_pause_menu_input.selection)
        {
        case 0:
            pause_resume();
            break;
        case 1:
            destroy_sprite_with_unique_id(4);
            g_some_pausemenu_var4 = 1;
            g_fade_track_volume(100, 10);
            break;
        case 2:
            pause_exit();
            break;
        }
        break;
    }
}

void handle_paused_input_out_of_menu(struct Sprite *sprite)
{
    struct Sprite *howTo;
    struct PauseMemcardRequest req;
    char name[0x28];

    switch (pause_action)
    {
    case 2:
        if ((pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & PAD_BUTTON_A)
         || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & PAD_BUTTON_B)
         || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000))
        {
            event_dest(0xD);
            call_SoundReqID_arg_1(0x70);
            g_related_to_pause_menu_input.flags &= ~1;
            if (sprite != NULL)
                sprite->unk8C &= ~1;
            pause_action = -1;
        }
        break;
    case 3:
        if (g_check_some_memcard_field() == 0)
        {
            if (g_get_some_memcard_var() == 0)
                OSReport("result_code: %d ", g_get_result_code());
            fn_1_F08C8();
            call_SoundReqID_arg_1(0x70);
            g_related_to_pause_menu_input.flags &= ~1;
            pause_action = -1;
        }
        break;
    case 4:
        howTo = get_sprite_with_unique_id(10);
        if (howTo != NULL)
            sprite_how_to_tick(0, howTo);
        if ((pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & PAD_BUTTON_A)
         || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & PAD_BUTTON_B)
         || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000))
        {
            call_SoundReqID_arg_1(0x70);
            g_related_to_pause_menu_input.flags &= ~1;
            destroy_sprite_with_unique_id(10);
            call_free_bmp_by_id(0xC);
            if (sprite != NULL)
                sprite->userVar = 1;
            pause_action = -1;
        }
        break;
    case 7:
        if (g_check_some_memcard_field() == 0)
        {
            if (g_get_some_memcard_var() == 0)
                OSReport("result_code: %d ", g_get_result_code());
            call_SoundReqID_arg_1(0x70);
            g_related_to_pause_menu_input.flags &= ~1;
            pause_action = -1;
        }
        break;
    case 8:
        switch (exit_save_stage)
        {
        case 1:
            compare_play_points_with_99999_after_exit_game();
            g_display_playpoint_or_gift_message(3, 320.0f, 400.0f);
            exit_save_stage++;
            break;
        case 2:
            if (fn_1_16C910() == 0)
            {
                fn_1_16C59C();
                exit_save_stage++;
            }
            break;
        default:
            pause_action = 7;
            g_some_other_flags |= 8;
            g_save_game_data();
            memset(&req, 0, sizeof(req));
            req.file = 0;
            g_sprintf_memcard_error(0, name);
            req.comment = name;
            req.buffer = storymode_save_files;
            g_some_replay_func4(0, (u8)g_last_used_memcard_slot_2, &req);
            break;
        }
        break;
    }
}

void pause_game(void)
{
    struct Sprite *sprite = get_sprite_with_unique_id(4);

    if ((modeCtrl.gameType == 0 || modeCtrl.gameType == 2 || modeCtrl.gameType == 14)
     && !(mode_info.flags & 0x100)
     && ((mode_info.flags & 0x20) || (mode_info.flags & 0x40) || (mode_info.flags & 0x10))
     && g_some_replay_func5() != 0)
        g_related_to_pause_menu_input.flags |= 4;
    else
        g_related_to_pause_menu_input.flags &= ~4;

    if (g_repause_cooldown_counter > 0)
        g_repause_cooldown_counter--;
    if (should_open_pause_menu())
        construct_pause_menu_sprite(sprite);
    else if (g_some_other_flags & 8)
    {
        if (!(g_related_to_pause_menu_input.flags & 1))
            check_pause_menu_input(sprite);
        if (PAUSE_GAME_ITEMS >= 2
         && g_related_to_pause_menu_input.selection == PAUSE_GAME_ITEMS - 1
         && g_related_to_pause_menu_input.timer == 40
         && sprite != NULL)
            sprite->userVar = 5;
        if (g_related_to_pause_menu_input.timer > 0)
        {
            if (--g_related_to_pause_menu_input.timer == 0)
                handle_paused_input_in_menu(sprite);
        }
        else
            handle_paused_input_out_of_menu(sprite);
        if (!(dip_switches & 1) || !(g_some_other_flags & 2))
        {
            if (sprite != NULL)
                g_something_with_pause_menu_and_game_over_sprite(0, sprite);
        }
    }
}
