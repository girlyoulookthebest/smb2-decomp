#ifndef RECPLAY_H
#define RECPLAY_H

#include "types.h"
#include "mathutil.h"

/* One interpolated frame of a recorded ball (SMB1's ReplayBallFrame). */
struct ReplayBallFrame
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    /*0x12*/ s16 unk12;  /* collision normal, scaled by 32767 */
    /*0x14*/ s16 unk14;
    /*0x16*/ s16 unk16;
    /*0x18*/ u32 flags;  /* low word of the ball flags */
    /*0x1C*/ float unk1C;
};  /* size = 0x20 */

/* SMB1's lbl_80250A68: the current replay, per player. */
struct ReplayHeader
{
    /*0x00*/ u16 flags;  /* 1: the replay reached the goal; 0x200: no data; 0x1000: world (not stage) */
    /*0x02*/ u16 stageId;
    /*0x04*/ u8 difficulty;
    /*0x05*/ u8 floor;
    /*0x06*/ u8 monkey;
    u8 unk7;
    /*0x08*/ char name[4];
    /*0x0C*/ float grade;
    /*0x10*/ s32 score;
    u8 unk14[4];
}; /* size = 0x18 */

struct ReplayInfo
{
    /*0x00*/ s32 unk0[4];  /* replay IDs */
    /*0x10*/ float unk10;  /* length in frames */
    /*0x14*/ s32 unk14;  /* player whose replay drives the clock */
};

extern struct ReplayInfo lbl_1_bss_71EC8;

/* Parameter order as in vec_set_len: chosen for evaluation order, ABI-identical. */
void g_get_replay_frame_data(int replay, float time, struct ReplayBallFrame *frame);

float fn_1_61B18(int replay);  /* replay length (SMB1's func_8004964C) */
void fn_1_62AA0(int replay, void *header);  /* SMB1's get_replay_header */

#endif
