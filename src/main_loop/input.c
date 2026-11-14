/* Per-frame controller processing: analog sticks and triggers as buttons, key repeat,
 * and the inputs merged over the pads in use. */
#include "types.h"
#include "input.h"
#include "mode.h"
#include "pool.h"

#define PAD_RESET_COMBO 0x1600  /* B + X + Start */

extern s32 lbl_80198BA8;  /* a pad is holding the reset combination */
extern u32 lbl_80198BA4;  /* pads (bits) not checked for the reset combination */
extern s8 g_active_players[4];

void *g_b_button_handler_for_sel_ngc(void);

void process_inputs(void)
{
    threshold_analog_inputs();
    g_calc_frames_since_last_input_change();
    merge_inputs();
}

/* Turn stick and trigger positions into button bits, with hysteresis: a direction that
 * is already held stays held down to a smaller deflection. */
void threshold_analog_inputs(void)
{
    int left;
    int x;
    int y;
    int i;
    u16 *buttons;
    int threshold;

    buttons = analog_inputs[0];
    for (i = 0; i < 4; i++, buttons += 5)
    {
        buttons[1] = buttons[0];
        buttons[0] = 0;

        x = pad_status_groups[i].unk0[0].stickX;
        y = pad_status_groups[i].unk0[0].stickY;

        if (buttons[1] & (1 << 0))
            threshold = -15;
        else
            threshold = -45;
        if (x < threshold)
            buttons[0] |= (1 << 0);

        if (buttons[1] & (1 << 1))
            threshold = 15;
        else
            threshold = 45;
        if (x > threshold)
            buttons[0] |= (1 << 1);

        if (buttons[1] & (1 << 2))
            threshold = -15;
        else
            threshold = -45;
        if (y < threshold)
            buttons[0] |= (1 << 2);

        if (buttons[1] & (1 << 3))
            threshold = 15;
        else
            threshold = 45;
        if (y > threshold)
            buttons[0] |= (1 << 3);

        x = pad_status_groups[i].unk0[0].substickX;
        y = pad_status_groups[i].unk0[0].substickY;

        if (buttons[1] & (1 << 4))
            threshold = -15;
        else
            threshold = -45;
        if (x < threshold)
            buttons[0] |= (1 << 4);

        if (buttons[1] & (1 << 5))
            threshold = 15;
        else
            threshold = 45;
        if (x > threshold)
            buttons[0] |= (1 << 5);

        if (buttons[1] & (1 << 6))
            threshold = -15;
        else
            threshold = -45;
        if (y < threshold)
            buttons[0] |= (1 << 6);

        if (buttons[1] & (1 << 7))
            threshold = 15;
        else
            threshold = 45;
        if (y > threshold)
            buttons[0] |= (1 << 7);

        left = pad_status_groups[i].unk0[0].triggerLeft;
        y = pad_status_groups[i].unk0[0].triggerRight;

        if (buttons[1] & (1 << 8))
            threshold = 40;
        else
            threshold = 80;
        if (left > threshold)
            buttons[0] |= (1 << 8);

        if (buttons[1] & (1 << 9))
            threshold = 40;
        else
            threshold = 80;
        if (y > threshold)
            buttons[0] |= (1 << 9);

        buttons[2] = buttons[0] & ~buttons[1];
        buttons[3] = buttons[0] & ~buttons[1];
    }
}

/* Key repeat: after 20 frames of unchanged input, report the held buttons every 4th
 * frame; otherwise report new presses. */
void g_calc_frames_since_last_input_change(void)
{
    struct ControllerInfo *cont = pad_status_groups;
    u16 *buttons = analog_inputs[0];
    int i;

    for (i = 0; i < 4; i++, cont++, buttons += 5)
    {
        if (cont->unk0[0].button == cont->unk0[1].button && buttons[0] == buttons[1])
            frames_since_last_input_change[i]++;
        else
            frames_since_last_input_change[i] = 0;

        if (frames_since_last_input_change[i] >= 20 && (frames_since_last_input_change[i] & 3) == 0)
        {
            cont->unk0[4].button = cont->unk0[0].button;
            buttons[4] = buttons[0];
        }
        else
        {
            cont->unk0[4].button = cont->unk0[2].button;
            buttons[4] = buttons[2];
        }
    }
}

/* Merge the inputs of every connected pad (menus) or of the pads of the players in play,
 * and note whether any of them holds the reset combination. */
/* Menus take input from every connected pad. */
static inline BOOL use_all_pads(void)
{
    BOOL allPads = FALSE;

    if (main_mode == 0 || main_mode == 5 || main_mode == 7 || main_mode == 3
     || (main_mode == 1 && sub_mode == 0x20 && g_b_button_handler_for_sel_ngc() == NULL))
        allPads = TRUE;
    return allPads;
}

void merge_inputs(void)
{
    s8 *status;
    int i;
    int j;
    BOOL allPads = use_all_pads();

    for (i = 0; i < 5; i++)
    {
        merged_digital_inputs[i] = 0;
        merged_analog_inputs[i] = 0;
    }
    lbl_80198BA8 = 0;
    lbl_80198BA4 = 0;

    if (allPads)
    {
        for (i = 0; i < 4; i++)
        {
            if (pad_status_groups[i].unk0[0].err == -1)
                continue;
            for (j = 0; j < 5; j++)
            {
                merged_digital_inputs[j] |= pad_status_groups[i].unk0[j].button;
                merged_analog_inputs[j] |= analog_inputs[i][j];
                lbl_80198BA8 |= (pad_status_groups[i].unk0[j].button & PAD_RESET_COMBO) == PAD_RESET_COMBO;
            }
            lbl_80198BA8 |= (pad_status_groups[i].unk0[0].button & PAD_RESET_COMBO) == PAD_RESET_COMBO;
        }
    }
    else
    {
        lbl_80198BA4 = 0xF;
        status = g_poolInfo.playerPool.statusList;
        if (status == NULL)
            return;
        for (i = 0; i < 4; i++, status++)
        {
            switch (modeCtrl.gameType)
            {
            case 6:
                if (g_active_players[i] == 0)
                    continue;
                break;
            default:
                if (*status != 2)
                    continue;
            }
            if (player_pad_map[i] == -1)
                continue;
            for (j = 0; j < 5; j++)
            {
                merged_digital_inputs[j] |= pad_status_groups[player_pad_map[i]].unk0[j].button;
                merged_analog_inputs[j] |= analog_inputs[player_pad_map[i]][j];
            }
            lbl_80198BA8 |= (pad_status_groups[player_pad_map[i]].unk0[0].button & PAD_RESET_COMBO) == PAD_RESET_COMBO;
            lbl_80198BA4 &= ~(1 << player_pad_map[i]);
        }
    }
}
