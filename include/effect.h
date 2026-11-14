#ifndef EFFECT_H
#define EFFECT_H

#include "types.h"
#include "mathutil.h"

/* SMB2 effect type IDs (renumbered from SMB1) */
enum
{
    ET_COLI_PARTICLE = 2,
    ET_LEVITATE = 4,
    ET_COLISTAR_PARTICLE = 5,
    ET_COLIFLASH = 10,
};

struct Effect
{
    /*0x00*/ s16 poolIndex;
    /*0x02*/ s16 uid;
    /*0x04*/ u32 flags;
    /*0x08*/ s16 type;
    /*0x0A*/ s16 state;
    /*0x0C*/ s32 timer;
    /*0x10*/ s32 u_otherTimer;
    /*0x14*/ s16 playerId;
    /*0x16*/ u16 cameras;
    /*0x18*/ float unk18;
    /*0x1C*/ float unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ Vec scale;
    /*0x30*/ struct GMAModel * DPTR model;  /* DPTR: Effect stays 0xB0 on x64 (literal 0xB0 memsets) */
    /*0x34*/ Vec pos;
    /*0x40*/ Vec vel;
    /*0x4C*/ s16 rotX;
    /*0x4E*/ s16 rotY;
    /*0x50*/ s16 rotZ;
    /*0x52*/ s16 unk52;
    /*0x54*/ s16 unk54;
    /*0x56*/ s16 unk56;
    /*0x58*/ Vec prevPos;
    /*0x64*/ Vec unk64;
    /*0x70*/ Vec unk70;
    /*0x7C*/ u8 filler7C[4];
    /*0x80*/ Vec unk80;
    /*0x8C*/ Vec unk8C;
    /*0x98*/ Vec unk98;
    /*0xA4*/ s16 unkA4;
    /*0xA6*/ s16 unkA6;
    /*0xA8*/ s16 unkA8;
    /*0xAC*/ float colorFactor;
};  /* size = 0xB0 */

struct EffectFuncs
{
    void (*init)(struct Effect *);
    void (*tick)(struct Effect *);
    void (*disp)(struct Effect *);
    void (*dest)(struct Effect *);
};

#define MAX_EFFECTS 512

extern struct Effect effects[MAX_EFFECTS];

void event_effect_init(void);
void event_effect_tick(void);
void event_effect_dest(void);
void g_draw_effects(void);
int spawn_effect(struct Effect *effect);
void fn_1_6869C(int type);
void fn_1_6870C(int id);
struct Effect *fn_1_6876C(int id);
void overwrite_effect_funcs(int type, struct EffectFuncs *newFuncs);

#endif
