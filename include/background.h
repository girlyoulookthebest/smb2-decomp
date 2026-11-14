#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "types.h"
#include "bitmap.h"
#include "light.h"
/* stage.h's world_theme is the s16 first field of the struct declared here */
#define world_theme world_theme_id
#include "stage.h"
#undef world_theme

struct BackgroundInfo
{
    /*0x00*/ s16 bgId;
    /*0x04*/ float animTimer;
    /*0x08*/ u32 unk8;
    /*0x0C*/ GXColor backdropColor;
    /*0x10*/ u8 filler10[0x14 - 0x10];
    /*0x14*/ Vec unk14;
    /*0x20*/ Vec unk20;
    /*0x2C*/ Vec unk2C;
    /*0x38*/ Vec unk38;
    /*0x44*/ u8 filler44[4];
    /*0x48*/ Mtx unk48;
    /*0x78*/ int (*unk78)();
    /*0x7C*/ int (*unk7C)();
    /*0x80*/ struct NlModel *unk80;
    /*0x84*/ float unk84;
    /*0x88*/ u8 filler88[4];
    /*0x8C*/ void *unk8C;
    /*0x90*/ void *unk90;
    /*0x94*/ void *ballEnvFunc;
    /*0x98*/ void (*unk98)(void);
    /*0x9C*/ void *work;
    /*0xA0*/ u32 unkA0;
    /*0xA4*/ u32 unkA4;
    /*0xA8*/ s16 unkA8;
    /*0xAA*/ s16 unkAA;
    /*0xAC*/ struct Ape **guestApes;  /* bg_guest.c */
    /*0xB0*/ struct BgGuestTheme *guestTheme;
    /*0xB4*/ u32 guestFlags;
    /*0xB8*/ float guestAnimSpeed;
    /*0xBC*/ u8 guestMaxDraw;
    u8 fillerBD[0xC0 - 0xBD];
    /*0xC0*/ float timers[4];
};

enum
{
    BG_MDL_CMP_PREFIX,
    BG_MDL_CMP_FULL,
    BG_MDL_CMP_SUFFIX,
    BG_MDL_CMP_END,
};

struct BgShadowGroup
{
    /*0x00*/ struct BgShadowGroup *next;
    /*0x04*/ s32 count;
    /*0x08*/ struct StageBgModel **models;
    /*0x0C*/ float scale;
    /*0x10*/ u32 texFmt;
    /*0x14*/ GXTexObj *tex;
};

struct BgModelSearch
{
    s32 cmpType;
    char *name;
};

extern struct BackgroundInfo world_theme;
extern struct BgLightInfo lbl_1_bss_2F48;

void bg_init_base(void);
void bg_tick_base(void);
void bg_disp_base(void);
void g_init_stagedef_bg_fg_models(struct StageBgModel *bgObj, int bgObjCount, struct BgModelSearch *search,
    int (*callback)(int, struct StageBgModel *));
int is_game_paused_and_in_view_stage(void);
int fn_1_70290(struct BgShadowGroup *group);
int g_smth_with_challenge_mode_var_and_3p(void);
int g_something_with_texture_scroll_2(int enable);
void fn_1_10A4CC(int id, Mtx mtx);
void fn_1_D68D4(void);
void fn_1_D6920(void);
void fn_1_D7110(s16 rotY, s16 unk);

#endif
