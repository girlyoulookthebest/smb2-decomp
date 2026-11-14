#ifndef DIP_H
#define DIP_H

#include "types.h"

/* Debug switches; names from the game's own DIP switch menu table. Bit values differ
 * from SMB1's. */
enum
{
    DIP_DEBUG = 1 << 0,
    DIP_DISP = 1 << 1,
    DIP_STCOLI = 1 << 2,
    DIP_TRIANGLE = 1 << 3,
    DIP_TAIKEN = 1 << 4,
    DIP_TIME_STOP = 1 << 5,
    DIP_NAMEENTRY = 1 << 6,
    DIP_FIX_WORLD = 1 << 7,
    DIP_TEST_CAM = 1 << 8,
    DIP_NO_INTR = 1 << 9,
    DIP_CAPTURE = 1 << 10,
    DIP_PERF_ALWAYS = 1 << 11,
    DIP_PLAY_STG_ALL = 1 << 12,
    DIP_PLAY_PNT_x10 = 1 << 13,
    DIP_SARU_0 = 1 << 14,
    DIP_SWITCH15 = 1 << 15,
    DIP_ONLY24MB = 1 << 16,
    DIP_SHADOW_DISP = 1 << 17,
    DIP_SPRITE_OFF = 1 << 18,
    DIP_SNDREQ_DISP = 1 << 19,
    DIP_SE_VOL_RESET = 1 << 20,
    DIP_APE_FACEDIR = 1 << 21,
    DIP_SEL_ALL_CTRL = 1 << 22,
    DIP_APE_NUMBER = 1 << 23,
    DIP_1P_TEST_CAM = 1 << 24,
    DIP_PREVIEW_SAVE = 1 << 25,
    DIP_AUTHOR_DEBUG = 1 << 26,
    DIP_SWITCH27 = 1 << 27,
    DIP_SOA = 1 << 28,
    DIP_TEST_TEAM = 1 << 29,
    DIP_NO_MINIMAP = 1 << 30,
    DIP_NO_STAGE = 1 << 31,
};

extern u32 dip_switches;
extern u32 g_some_other_flags;  /* SMB1's gamePauseStatus */

#endif
