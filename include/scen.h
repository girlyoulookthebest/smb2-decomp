#ifndef SCEN_H
#define SCEN_H

#include "types.h"

/* Story mode progress, defined in main_loop.c. */
struct ScenInfo
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 world;
    /*0x06*/ s16 unk6;
    /*0x08*/ u32 flags;
    /*0x0C*/ s16 nextWorld;
    u8 fillerE[2];
    /*0x10*/ int score;
    /*0x14*/ int bananas;
    /*0x18*/ void (*destFunc)(void);
    u8 filler1C[6];
    /*0x22*/ s8 saveSlot;
    u8 filler23;
    /*0x24*/ int stageId;
    /*0x28*/ u32 playTime;
    /*0x2C*/ u32 playTimeBase;
    /*0x30*/ u32 unk30;
    /*0x34*/ u32 unk34[1];
}; /* size = 0x38 */

extern struct ScenInfo scen_info;

#endif
