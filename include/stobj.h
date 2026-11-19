#ifndef STOBJ_H
#define STOBJ_H

#include "types.h"
#include "mathutil.h"

struct PhysicsBall;

/* Stage object (bumpers, jamabars, goal tapes, ...). Layout follows SMB1. */
struct Stobj
{
    /*0x00*/ s16 id;
    s16 unk2;
    /*0x04*/ s16 type;
    /*0x06*/ s16 unk6;  /* mini_fight2: player of a glove stobj */
    u32 unk8;
    /*0x0C*/ s16 state;
    /*0x0E*/ s16 counter;
    /*0x10*/ Vec u_model_origin;
    /*0x1C*/ Vec position;
    /*0x28*/ Vec position_2;
    /*0x34*/ float boundSphereRadius;
    /*0x38*/ void (*coliFunc)(struct Stobj *, struct PhysicsBall *);
    Vec unk3C;
    float unk48;
    float unk4C;
    float unk50;
    /*0x54*/ struct GMAModel *model;
    /*0x58*/ Vec u_some_pos;
    Vec unk64;
    /*0x70*/ s16 rotX;
    /*0x72*/ s16 rotY;
    /*0x74*/ s16 rotZ;
    s16 unk76;
    s16 unk78;
    /*0x7A*/ s16 unk7A;
    Vec unk7C;
    s16 unk88;
    s16 unk8A;
    s16 unk8C;
    u8 filler8E[2];
    Vec unk90;
    float unk9C;
    /*0xA0*/ s8 animGroupId;
    /*0xA4*/ void *extraData;
    Vec unkA8;
    /*0xB4*/ Vec u_local_pos;
    /*0xC0*/ Vec u_local_vel;
};  /* size = 0xCC */

#endif
