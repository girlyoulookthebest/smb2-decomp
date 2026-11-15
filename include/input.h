#ifndef INPUT_H
#define INPUT_H

#include "types.h"

/* Matches the Dolphin SDK's PADStatus layout. */
typedef struct
{
    /*0x00*/ u16 button;
    /*0x02*/ s8 stickX;
    /*0x03*/ s8 stickY;
    /*0x04*/ s8 substickX;
    /*0x05*/ s8 substickY;
    /*0x06*/ u8 triggerLeft;
    /*0x07*/ u8 triggerRight;
    /*0x08*/ u8 analogA;
    /*0x09*/ u8 analogB;
    /*0x0A*/ s8 err;
} PADStatus; /* size = 0xC */

#define PAD_BUTTON_LEFT 0x0001
#define PAD_BUTTON_RIGHT 0x0002
#define PAD_BUTTON_DOWN 0x0004
#define PAD_BUTTON_UP 0x0008
#define PAD_TRIGGER_Z 0x0010
#define PAD_TRIGGER_R 0x0020
#define PAD_TRIGGER_L 0x0040
#define PAD_BUTTON_A 0x0100
#define PAD_BUTTON_B 0x0200
#define PAD_BUTTON_X 0x0400
#define PAD_BUTTON_Y 0x0800

/* [0] held, [1] previously held, [2] newly pressed, [3] released, [4] repeat (as in SMB1). */
struct ControllerInfo
{
    PADStatus unk0[5];
}; /* size = 0x3C */

extern struct ControllerInfo pad_status_groups[4];

/* Per-pad analog-stick "buttons" ([0] = held), and the player -> pad mapping. */
extern u16 analog_inputs[][5];
#ifdef __MWERKS__
extern s32 player_pad_map[4];
#else
extern s32 player_pad_map[4 + 10];  /* + the idle-chatter counters, see ball.c */
#endif
extern u16 merged_digital_inputs[];
extern u16 merged_analog_inputs[];
extern s32 frames_since_last_input_change[4];

void process_inputs(void);
void threshold_analog_inputs(void);
void g_calc_frames_since_last_input_change(void);
void merge_inputs(void);

#endif
