#ifndef WORLD_H
#define WORLD_H

#include "types.h"
#include "mathutil.h"

/* Per-player stage tilt state (SMB1's struct World, plus a smoothed "up" vector). */
struct World
{
    /*0x00*/ s16 xrot;
    /*0x02*/ s16 zrot;
    /*0x04*/ s16 xrotPrev;
    /*0x06*/ s16 zrotPrev;
    /*0x08*/ s8 state;
    /*0x09*/ s8 playerId;
    /*0x0C*/ float maxTilt;  /* degrees */
    /*0x10*/ Vec unk10;  /* gravity direction */
    /*0x1C*/ s16 unk1C;  /* frames to ignore stick input */
    /*0x20*/ Vec unk20;  /* smoothed stage "up" vector */
    u8 unk2C[0x3C - 0x2C];
}; /* size = 0x3C */

extern struct World world_infos[];
extern Vec g_gravity_dir;
extern struct World *current_world_info;

void event_world_init(void);
void event_world_tick(void);
void event_world_dest(void);

#endif
