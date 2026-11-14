#ifndef OPTION_H
#define OPTION_H

#include "types.h"
#include "mathutil.h"

struct Panel
{
    /*0x00*/ s8 state;
    /*0x01*/ s8 counter;
    /*0x04*/ float x;
    /*0x08*/ float y;
    /*0x0C*/ float unkC;
    /*0x10*/ s32 mode;
    /*0x14*/ s32 sel;
    /*0x18*/ s32 sel2;
    /*0x1C*/ s32 unk1C;
};

struct OptionWork
{
    /*0x000*/ s32 unk0;
    /*0x004*/ s32 unk4;
    /*0x008*/ s32 unk8;
    /*0x00C*/ float unkC[3];
    /*0x018*/ s32 unk18;
    /*0x01C*/ s16 scroll;
    /*0x01E*/ s16 scrollOffset;
    /*0x020*/ float scale[3];
    /*0x02C*/ float yOffset[3];
    /*0x038*/ u8 unk38;
    /*0x039*/ s8 unk39;
    /*0x03A*/ s8 unk3A;
    /*0x03B*/ s8 unk3B;
    /*0x03C*/ struct Panel panels[10];
    /*0x17C*/ s32 unk17C;
    /*0x180*/ u32 flags;
    /*0x184*/ Mtx mtx;
};

/* one replay entry */
struct ReplayEntry
{
    /*0x00*/ u8 unk0[2];
    /*0x02*/ u16 unk2;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s32 unk8;
    /*0x0C*/ s32 unkC;
    /*0x10*/ u8 unk10[4];
    /*0x14*/ s8 unk14;
    u8 unk15[0x18 - 0x15];
};

extern struct ReplayEntry lbl_9_bss_634C[];

extern struct OptionWork lbl_9_bss_84;

struct Panel *fn_9_BC84(int id);
void fn_9_0(s32 arg0);
void fn_9_68(s32 arg0);
void fn_9_7E4(void);
void fn_9_10E8(void);
void fn_9_2DAC(void);
void fn_9_3B78(void);
void fn_9_4510(void);
void fn_9_5194(void);
void fn_9_5AD0(void);
void fn_9_64A8(void);
void fn_9_712C(void);
void fn_9_7E5C(void);

#endif
