#ifndef SPRITE_H
#define SPRITE_H

#include "types.h"
#include "bitmap.h"

enum
{
    SPRITE_TYPE_TEXT,
    SPRITE_TYPE_BITMAP,
    SPRITE_TYPE_TEXTURE,
};

/* On-screen 2D element. */
struct Sprite
{
    /*0x00*/ s8 type;
    /*0x01*/ u8 fontId;
    /*0x02*/ s8 poolIdx;
    /*0x03*/ s8 textAlign;
    /*0x04*/ float x;
    /*0x08*/ float y;
    /*0x0C*/ u8 mulR;
    /*0x0D*/ u8 mulG;
    /*0x0E*/ u8 mulB;
    /*0x0F*/ s8 tag;
    /*0x10*/ s16 counter;
    /*0x12*/ s16 widescreenArg;
    /*0x14*/ float shadowAlpha;
    /*0x18*/ s8 shadowX;
    /*0x19*/ s8 shadowY;
    /*0x1A*/ s8 spacingX;
    /*0x1B*/ s8 spacingY;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ u32 unk24;
    /*0x28*/ u8 fontStyle;
    u8 unk29[0x2C - 0x29];
    /*0x2C*/ struct SpriteTexture *tex;
    /*0x30*/ void (*destFunc)(struct Sprite *);
    /*0x34*/ void (*mainFunc)(s8 *, struct Sprite *);
    /*0x38*/ void (*drawFunc)(struct Sprite *);
    /*0x3C*/ u16 bmpId;
    u8 unk3E[0x40 - 0x3E];
    /*0x40*/ float scaleX;
    /*0x44*/ float scaleY;
    /*0x48*/ float depth;
    /*0x4C*/ s32 userVar;
    /*0x50*/ s32 unk50;
    /*0x54*/ s32 unk54;
    /*0x58*/ float unk58;
    /*0x5C*/ float unk5C;
    /*0x60*/ float unk60;
    /*0x64*/ struct Sprite *prev;
    /*0x68*/ struct Sprite *next;
    /*0x6C*/ s32 left;
    /*0x70*/ s32 top;
    /*0x74*/ s32 right;
    /*0x78*/ s32 bottom;
    /*0x7C*/ s16 rotation;
    u8 unk7E[0x80 - 0x7E];
    /*0x80*/ float opacity;
    /*0x84*/ u8 addR;
    /*0x85*/ u8 addG;
    /*0x86*/ u8 addB;
    /*0x88*/ u32 flags;
    /*0x8C*/ u32 unk8C;
    /*0x90*/ float unk90;
    /*0x94*/ float unk94;
    /*0x98*/ float unk98;
    /*0x9C*/ float unk9C;
    /*0xA0*/ char text[0x30];
};

/* Texture drawn by SPRITE_TYPE_TEXTURE sprites. */
struct SpriteTexture
{
    /*0x00*/ s8 type;
    u8 unk1[8 - 1];
    /*0x08*/ GXTexObj texObj;
    u8 unk28[0x30 - 0x28];
    /*0x30*/ u16 width;
    /*0x32*/ u16 height;
};


struct Sprite *create_sprite(void);
struct Sprite *create_linked_sprite(struct Sprite *parent);

#endif
