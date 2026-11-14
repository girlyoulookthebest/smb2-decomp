#include "baseball.h"
#include "input.h"

extern u32 lbl_1_bss_6FB9C;

#define PAD_BUTTON_B_ 0x200
#define PAD_REPEAT(pad, btn) \
( \
    (pad_status_groups[pad].unk0[4].button & (btn)) \
 || (analog_inputs[pad][4] & (btn)) \
 || ( \
        ((pad_status_groups[pad].unk0[0].button & (btn)) || (analog_inputs[pad][0] & (btn))) \
     && (analog_inputs[pad][0] & PAD_BUTTON_B_) \
    ) \
)

s32 lbl_18_bss_1A320;
u8 lbl_18_bss_1A324;
s8 lbl_18_bss_1A325;
s32 lbl_18_bss_1A328;

void baseball_33_bss_pin(void)
{
    lbl_18_bss_1A320 = 0;
    lbl_18_bss_1A324 = 0;
    lbl_18_bss_1A325 = 0;
    lbl_18_bss_1A328 = 0;
}

const s8 lbl_18_rodata_40E8[8] = {3, 4, 6, 4, 4, 0, 0, 0};

#ifndef __MWERKS__
/* PC port: "Quit to Desktop" row after the game's rows, as in
 * main_loop/pause_menu.c (pc_quit.c). */
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
    case 0x53: case 0x54:
    case 0x55: case 0x56:
        return FALSE;
    }
    switch (sub_mode_request)
    {
    case 0x3C: case 0x3D: case 0x3E: case 0x3F: case 0x40:
    case 0x41: case 0x42: case 0x43: case 0x44: case 0x45:
    case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D:
    case 0x4E: case 0x4F: case 0x50: case 0x51: case 0x52:
    case 0x53: case 0x54:
    case 0x55: case 0x56:
        return FALSE;
    }
    if (g_check_flag_related_to_pausing() != 0)
        return FALSE;
    return TRUE;
}

static int should_open_pause_menu(void)
{
    if (!is_pausing_allowed())
        return FALSE;
    if (lbl_18_bss_1A324 != 0)
        return FALSE;
    if (dip_switches & 1)
    {
        if (!(analog_inputs[0][0] & 0x100)
         && !(analog_inputs[0][0] & 0x200)
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

void fn_18_25298(void)
{
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_18_252CC(struct Sprite *sprite)
{
    s32 i;

    g_some_other_flags |= 8;
    g_related_to_pause_menu_input.padId = 0;
    for (i = 0; i < 4; i++) {
        if (pad_status_groups[i].unk0[0].err == 0 && (pad_status_groups[i].unk0[2].button & 0x1000)) {
            g_related_to_pause_menu_input.padId = i;
            break;
        }
    }
    lbl_18_bss_1A325 = 0;
    lbl_18_bss_1A324 = 0x1E;
    g_related_to_pause_menu_input.playerId = 0;
    if (modeCtrl.unk40 == 1) {
        for (i = 0; i < modeCtrl.playerCount; i++) {
            if (g_related_to_pause_menu_input.padId == player_pad_map[i]) {
                g_related_to_pause_menu_input.playerId = i;
                break;
            }
        }
    } else if (modeCtrl.gameType == 6) {
        if (modeCtrl.playerCount == 1) {
            g_related_to_pause_menu_input.playerId = 0;
        } else if (g_related_to_pause_menu_input.unk16 == -1) {
            g_related_to_pause_menu_input.playerId = 0;
        } else {
            g_related_to_pause_menu_input.playerId = (g_related_to_pause_menu_input.padId == player_pad_map[lbl_1_bss_176A]) ? lbl_1_bss_176A : 1 - lbl_1_bss_176A;
        }
    } else {
        g_related_to_pause_menu_input.playerId = modeCtrl.currPlayer;
    }
    g_related_to_pause_menu_input.menuType = 0;
    g_related_to_pause_menu_input.flags &= ~1;
    g_related_to_pause_menu_input.selection = 0;
    g_related_to_pause_menu_input.itemCount = lbl_18_rodata_40E8[g_related_to_pause_menu_input.menuType];
#ifndef __MWERKS__
    pc_pause_quit_row = g_related_to_pause_menu_input.itemCount++;
    pc_quit_confirm_close();
#endif
    g_related_to_pause_menu_input.timer = 0;
    lbl_18_bss_1A328 = -1;
    if (sprite == NULL) {
        create_pausemenu_sprite();
        get_sprite_with_unique_id(4);
    } else {
        sprite->userVar = 1;
    }
    fn_18_218EC(0x6C);
    if (lbl_18_bss_1A320 == 0) {
        fn_18_21BB8(0x80, 0x20);
    }
}

void fn_18_255D4(struct Sprite *sprite)
{
    s8 prev = g_related_to_pause_menu_input.selection;

#ifndef __MWERKS__
    if (pc_quit_confirm_active()) {
        s32 pad = g_related_to_pause_menu_input.padId;
        unsigned int press = pad_status_groups[pad].unk0[2].button & (0x100 | 0x200 | 0x1000);

        if (PAD_REPEAT(pad, PAD_BUTTON_LEFT))
            press |= PAD_BUTTON_LEFT;
        if (PAD_REPEAT(pad, PAD_BUTTON_RIGHT))
            press |= PAD_BUTTON_RIGHT;
        switch (pc_quit_confirm_tick(press)) {
        case PCQ_MOVED:
            fn_18_218EC(0x6B);
            break;
        case PCQ_NO:
            fn_18_218EC(0x6C);
            break;
        case PCQ_YES:
            fn_18_218EC(0x6D);
            g_related_to_pause_menu_input.flags |= 1;
            break;
        }
        return;
    }
#endif

    if (lbl_18_bss_1A325 > 0) {
        lbl_18_bss_1A325--;
    }
    if (PAD_REPEAT(g_related_to_pause_menu_input.padId, PAD_BUTTON_UP) && lbl_18_bss_1A325 == 0) {
        if (--g_related_to_pause_menu_input.selection < 0) {
            g_related_to_pause_menu_input.selection = g_related_to_pause_menu_input.itemCount - 1;
        }
    } else if (PAD_REPEAT(g_related_to_pause_menu_input.padId, PAD_BUTTON_DOWN) && lbl_18_bss_1A325 == 0) {
        if (++g_related_to_pause_menu_input.selection > g_related_to_pause_menu_input.itemCount - 1) {
            g_related_to_pause_menu_input.selection = 0;
        }
    }
    if (prev != g_related_to_pause_menu_input.selection) {
        fn_18_218EC(0x6B);
        lbl_18_bss_1A325 = 0xA;
    }
    if ((pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x100) || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x200) || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000)) {
#ifndef __MWERKS__
        if (g_related_to_pause_menu_input.selection == pc_pause_quit_row
         && !(pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x200)
         && !(pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000)) {
            pc_quit_confirm_open();
            fn_18_218EC(0x6D);
            return;
        }
#endif
        g_related_to_pause_menu_input.flags |= 1;
        fn_18_218EC(0x6D);
        g_related_to_pause_menu_input.timer = 2;
        if ((pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x200) || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000)) {
            g_related_to_pause_menu_input.selection = 0;
        }
        if (PAUSE_GAME_ITEMS >= 2 && g_related_to_pause_menu_input.selection == PAUSE_GAME_ITEMS - 1) {
            g_related_to_pause_menu_input.timer = 0x3C;
            if (sprite != NULL) {
                sprite->userVar = 3;
            }
            fn_18_21A68();
        }
    }
}

void fn_18_25A80(void)
{
    if (lbl_18_bss_1A324 != 0) {
        lbl_18_bss_1A324--;
    }
}

s32 fn_18_25AB8(void)
{
    struct Sprite *sprite;
    struct Sprite *tr3;
    s32 heap;

    sprite = get_sprite_with_unique_id(4);
    g_related_to_pause_menu_input.flags &= ~4;
    if (should_open_pause_menu()) {
        fn_18_252CC(sprite);
        return -1;
    }
    if (sprite == NULL) {
        return 0;
    }
    if (!(g_related_to_pause_menu_input.flags & 1) && is_pausing_allowed()) {
        fn_18_255D4(sprite);
    }
    if (PAUSE_GAME_ITEMS >= 2 && g_related_to_pause_menu_input.selection == PAUSE_GAME_ITEMS - 1 && g_related_to_pause_menu_input.timer == 0x28 && sprite != NULL) {
        sprite->userVar = 5;
    }
    if (g_related_to_pause_menu_input.timer > 0) {
        if (--g_related_to_pause_menu_input.timer == 0) {
            g_some_other_flags &= ~8;
            switch (g_related_to_pause_menu_input.selection) {
            case 0:
                lbl_18_bss_1A328 = 0;
                destroy_sprite_with_unique_id(4);
                if (lbl_18_bss_1A320 == 0) {
                    fn_18_21BB8(0xFF, 0x20);
                }
                lbl_18_bss_1A320 = 0;
                break;
            case 1:
                lbl_18_bss_1A328 = 4;
                if (sprite != NULL) {
                    sprite->userVar = 6;
                }
                heap = OSSetCurrentHeap(main_heap);
                load_bmp_by_id(0xC);
                OSSetCurrentHeap(heap);
                fn_18_22CE0();
                g_some_other_flags |= 8;
                break;
            case 2:
                lbl_18_bss_1A328 = 1;
                g_return_to_sel_ngc(-1);
                lbl_18_bss_1A320 = 0;
                break;
            }
        }
    } else if (lbl_18_bss_1A328 == 4) {
        tr3 = get_sprite_with_unique_id(0xA);
        if (tr3 != NULL) {
            fn_18_22E7C(NULL, tr3);
        }
        if ((pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x100) || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x200) || (pad_status_groups[g_related_to_pause_menu_input.padId].unk0[2].button & 0x1000)) {
            fn_18_218EC(0x6C);
            g_related_to_pause_menu_input.flags &= ~1;
            destroy_sprite_with_unique_id(0xA);
            call_free_bmp_by_id(0xC);
            if (sprite != NULL) {
                sprite->userVar = 1;
            }
            lbl_18_bss_1A328 = -1;
        }
    }
    if (sprite != NULL) {
        g_something_with_pause_menu_and_game_over_sprite(NULL, sprite);
    }
    if (lbl_18_bss_1A328 == 0) {
        return 0;
    }
    if (lbl_18_bss_1A328 == 1) {
        return 1;
    }
    return -1;
}

