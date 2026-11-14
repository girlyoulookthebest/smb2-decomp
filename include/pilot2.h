#ifndef PILOT2_H
#define PILOT2_H

#include "types.h"

/* Per-player state for the Monkey Target 2 minigame. */
struct Pilot2Player
{
    /*0x00*/ u32 flags;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ int unkC;
    /*0x10*/ float unk10;
    /*0x14*/ float unk14;
    /*0x18*/ s16 unk18;
    /*0x1A*/ s16 unk1A;
    /*0x1C*/ u16 unk1C;
    u8 unk1E[0x20 - 0x1E];
    /*0x20*/ u16 unk20;
    /*0x22*/ s16 unk22;
    /*0x24*/ s16 unk24;
    /*0x26*/ s16 unk26;
    /*0x28*/ s16 unk28;
    u8 unk2A[0x2C - 0x2A];
    /*0x2C*/ int unk2C;
    /*0x30*/ int unk30;
    /*0x34*/ s8 unk34;
    u8 unk35[0x38 - 0x35];
    /*0x38*/ float unk38;
    /*0x3C*/ float unk3C;
    /*0x40*/ s8 unk40;  /* controller */
    /*0x41*/ s8 unk41;
    /*0x42*/ s8 unk42;
    u8 unk43;
    /*0x44*/ s16 unk44;
    /*0x46*/ s16 unk46;
    /*0x48*/ s16 unk48;
    /*0x4A*/ s16 unk4A;
    /*0x4C*/ s16 unk4C;
    /*0x4E*/ s16 unk4E;
}; /* size = 0x50 */

extern u8 *lbl_13_bss_4;
extern u8 *lbl_13_bss_8;
extern s16 lbl_13_bss_10;
extern int lbl_13_bss_24;
extern int lbl_13_bss_50[];
extern struct Pilot2Player lbl_13_bss_70[];
extern s8 lbl_13_bss_210[];
extern s16 lbl_13_bss_218[];
extern s8 lbl_13_bss_224[];
extern s8 lbl_13_bss_234[];
extern Vec lbl_13_bss_338;

#endif
