#ifndef TEXTDRAW_H
#define TEXTDRAW_H

#include "types.h"

/* Current text state, set up by the textdraw_set_* functions. */
struct TextDrawInfo
{
    /*0x00*/ float startX;
    /*0x04*/ float x;
    /*0x08*/ float y;
    /*0x0C*/ s16 fontId;
    /*0x10*/ u32 mulColor;
    /*0x14*/ u32 addColor;
    /*0x18*/ s16 rotZ;
    /*0x1C*/ float depth;
    /*0x20*/ float scaleX;
    /*0x24*/ float scaleY;
    /*0x28*/ float alpha;
    /*0x2C*/ u32 flags;
    /*0x30*/ s8 alignment;
    /*0x32*/ s16 unk32;
    /*0x34*/ s16 shadowX;
    /*0x36*/ s16 shadowY;
    /*0x38*/ float shadowAlpha;
    /*0x3C*/ s32 unk3C;
    /*0x40*/ float unk40;
    /*0x44*/ float unk44;
    /*0x48*/ u8 fontStyle;
    /*0x4C*/ float spacingX;
    /*0x50*/ float spacingY;
    /*0x54*/ u16 counter;
}; /* size = 0x58 */

struct TextdrawUnk8
{
    /*0x00*/ s32 unk0;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ float unkC;
    /*0x10*/ float unk10;
}; /* size = 0x14 */

extern struct TextDrawInfo textdraw_pos_x;
extern struct TextdrawUnk8 g_textdraw_unk8;

#endif
