/* sel_ngc_option.c: Options sub-menus: game data (memory card), controller, sound, replay,
 * play points / gift shop. */

/* sel_ngc.h names these tags in prototypes before any declaration; declare them at file scope
 * first so the prototypes refer to the types defined below. */
struct StickPoint;
struct MemcardRequest;
struct TextBox;

#include "sel_ngc.h"

/* ---------------------------------------------------------------------------------------------
 * Types
 * ------------------------------------------------------------------------------------------- */

/* analog stick sample/calibration point (same as option.c) */
struct StickPoint {
    s8 x;
    s8 y;
};

/* memory card request (= option.c struct MemcardRequest) */
struct MemcardRequest { /* layout as in src/main_loop/memcard.c */
    /* 0x00 */ u8 file;
    /* 0x04 */ u32 flags;
    /* 0x08 */ char *comment;
    /* 0x0C */ void *buffer;
    /* 0x10 */ u32 size;
    /* 0x14 */ char *name;
}; /* size = 0x18 */

/* sel.c lbl_1_bss_175C: replay list state kept across menu visits */
struct SelReplayListState {
    /* 0x0 */ s32 flags; /* 1: list read, 2: returning from a replay */
    /* 0x4 */ s32 cursor;
    /* 0x8 */ s32 count;
};

/* main_loop.c lbl_1_bss_12AC: the replay file chosen for playback (= exoption.c struct ExOptionFile) */
struct SelReplayFileSel {
    /* 0x00 */ char name[0x20];
    /* 0x20 */ u32 size;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
};

/* replay list: slide-in position of the list and its fade */
struct OptionReplayListPos {
    /* 0x0 */ f32 x;
    /* 0x4 */ f32 y;
    /* 0x8 */ f32 alpha;
};

/* one replay file as listed from the memory card; the file name hex-encodes the replay header */
struct OptionReplayFile {
    /* 0x00 */ char name[0x20];
    /* 0x20 */ u32 size; /* nonzero: slot used */
    /* 0x24 */ s32 unk24; /* sort key */
}; /* size = 0x28 */

/* replay header hex-encoded in the first 24 characters of the file name (cf. exoption.c ExOptionFileId) */
struct OptionReplayFileId {
    /* 0x0 */ u8 flags;
    /* 0x1 */ s8 unk1;
    /* 0x2 */ s8 unk2;
    /* 0x3 */ s8 chara;
    /* 0x4 */ s16 stageId;
    /* 0x6 */ u8 filler6[2];
    /* 0x8 */ u32 time; /* seconds */
}; /* size = 0xC */

/* = OSCalendarTime; sel_ngc.h declares OSTicksToCalendarTime(s64, void *) */
struct OptionCalendarTime {
    int sec, min, hour, mday, mon, year, wday, yday, msec, usec;
};

/* decoded replay list entry */
struct OptionReplayEntry {
    /* 0x00 */ s8 unk0; /* mode */
    /* 0x01 */ s8 unk1;
    /* 0x02 */ s8 unk2;
    /* 0x03 */ u8 filler3;
    /* 0x04 */ s16 unk4; /* year */
    /* 0x06 */ s8 unk6; /* month */
    /* 0x07 */ s8 unk7; /* day */
    /* 0x08 */ s8 unk8; /* hour */
    /* 0x09 */ s8 unk9; /* minute */
    /* 0x0A */ s8 unkA; /* character */
    /* 0x0B */ u8 fillerB;
    /* 0x0C */ s16 unkC; /* stage id */
    /* 0x0E */ u8 fillerE[2];
    /* 0x10 */ f32 unk10; /* scroll offset, eases to 0 */
    /* 0x14 */ f32 unk14;
    /* 0x18 */ struct OptionReplayFile *unk18;
}; /* size = 0x1C on GC */

struct GameDataOption {
    /* 0x0 */ char **name; /* [locale] */
    /* 0x4 */ char **desc; /* [locale] */
    /* 0x8 */ s32 op;
    /* 0xC */ u8 *flag;
};
struct GameDataStat {
    /* 0x00 */ char *name;
    /* 0x04 */ u16 *a;
    /* 0x08 */ u16 *b;
    /* 0x0C */ s16 total;
    /* 0x0E */ s16 per;
    /* 0x10 */ s8 base;
};
struct ScreenOption {
    /* 0x00 */ char **name;
    /* 0x04 */ char **desc;
    /* 0x08 */ u8 *value;
    /* 0x0C */ s32 count;
    /* 0x10 */ char ***options;
    /* 0x14 */ void (*callback)(void);
};
struct ControllerOption {
    /* 0x0 */ char **name;
    /* 0x4 */ char **desc;
    /* 0x8 */ s8 unk8; /* state to enter */
};
struct RankingOption {
    /* 0x0 */ char **name;
    /* 0x4 */ char **desc;
};

#define replayList ((struct SelReplayListState *)lbl_1_bss_175C)
#define selReplayFile ((struct SelReplayFileSel *)lbl_1_bss_12AC)
#define menuWork ((struct SelNgcMenuWork *)&g_currently_visible_menu_screen)
#define controllerInfo ((struct ControllerInfo *)&lbl_80145210)

/* ---------------------------------------------------------------------------------------------
 * External functions and data not (correctly) declared in sel_ngc.h
 * ------------------------------------------------------------------------------------------- */

s32 did_any_pad_press_input(u32 input);      /* sel_ngc.c */
void fn_3_14D8(s32 arg0);                   /* sel_ngc.c */
void fn_3_15F8(void);                       /* sel_ngc.c */
void g_b_button_pressed_on_menu(s32 arg0);  /* sel_ngc.c */
void fn_3_140CC(int counter);                    /* sel_ngc_sprite.c */
void fn_3_14B40(void);                      /* sel_ngc_sprite.c */
void fn_3_14BAC(void);                      /* sel_ngc_sprite.c */
void fn_3_1EF64(void);                      /* sel_ngc_preview.c */
int g_check_if_partygame_unlocked(int id); /* unlock.c */
void fn_1_11DE54(int a, void *b);          /* name_entry2.c */
extern struct SelNgcBss44 lbl_3_bss_44;     /* sel_ngc.c */
extern const s8 lbl_3_rodata_4[12];         /* sel_ngc.c (sel_ngc_data.h) */

/* ---------------------------------------------------------------------------------------------
 * This file's functions
 * ------------------------------------------------------------------------------------------- */

void fn_3_242FC(struct Sprite *sprite);
void fn_3_24AF8(void);
void fn_3_24D04(void);
void fn_3_251E8(void);
void fn_3_25290(void);
void fn_3_252A4(void);
void fn_3_254B0(void);
void fn_3_25620(struct OptionReplayFile *file, struct OptionReplayEntry *entry);
void fn_3_25858(struct OptionReplayEntry *entries, s32 count);
s32 fn_3_25CFC(struct OptionReplayEntry *a, struct OptionReplayEntry *b);
void fn_3_25D70(s8 *, struct Sprite *);
void fn_3_25ECC(struct Sprite *sprite);
void fn_3_276E4(struct Sprite *sprite);
void fn_3_28D30(struct Sprite *sprite);

/* ---------------------------------------------------------------------------------------------
 * .bss (defined below, before the first function)
 * ------------------------------------------------------------------------------------------- */

/* Work area shared by all the option sub-screens (cleared on entry to each). */
struct SelOptionWork {
    /* 0x00 */ s32 state;
    /* 0x04 */ s32 cursor;     /* row on the current screen */
    /* 0x08 */ s32 subCursor;  /* column / pad / slot inside the selected row */
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 savedValue; /* value restored when the edit is cancelled */
    /* 0x14 */ s32 subState;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ void (*tick)(void); /* per-frame helper of the current screen (controller test) */
    /* 0x20 */ s32 timer;      /* frame counter, used for blinking */
    /* 0x24 */ s32 scrollTop;  /* replay list: first visible row */
    /* 0x28 */ s32 scrollAnim; /* replay list: scroll animation counter */
}; /* size = 0x2C on GC */
extern struct SelOptionWork option_game_data_current_operation;
extern s16 lbl_3_bss_82C[];
extern struct StickPoint lbl_3_bss_8B0[4][8];
extern s32 lbl_3_bss_6910;
extern s32 lbl_3_bss_6920;
extern struct OptionReplayEntry lbl_3_bss_7D40[];
extern s32 lbl_3_bss_8B40;
extern s32 g_replay_stage_id_to_load;
extern struct Preview lbl_3_bss_8B48;
extern s8 lbl_3_bss_8BC0;
extern s8 lbl_3_bss_8BC1;
extern s8 g_gift_menu_cursor_pos;
extern s32 lbl_3_bss_8BC8[];

/* ---- .rodata ---- */

/* Paired-single quantized store (GQR2 = u8): float -> u8 with clamping. */
static inline void option_f32_to_u8(register f32 *in, register u8 *out)
{
#ifdef __MWERKS__
    asm
    {
        lfs f1, 0(in)
        psq_st f1, 0(out), 1, 2
    }
#else
    f32 v = *in;
    *out = (v <= 0.0f) ? 0 : (v >= 255.0f) ? 255 : (u8)v;
#endif
}

/* forward declarations of this TU's data */
extern char *lbl_3_data_EF44[6];
extern char *lbl_3_data_EF6C[6];
extern char *lbl_3_data_EFA0[6];
extern char *lbl_3_data_EFE4[6];
extern char *lbl_3_data_F028[6];
extern char *lbl_3_data_F06C[6];
extern char *lbl_3_data_F0A4[6];
extern char *lbl_3_data_F108[6];
extern char *lbl_3_data_F168[6];
extern char *lbl_3_data_F1C8[6];
extern char *lbl_3_data_F20C[6];
extern char *lbl_3_data_F28C[6];
extern char *lbl_3_data_F2CC[6];
extern char *lbl_3_data_F338[6];
extern char *lbl_3_data_F6D0[6];
extern char *lbl_3_data_F708[6];
extern char *lbl_3_data_F73C[6];
extern char *lbl_3_data_F770[6];
extern char **lbl_3_data_F788[3];
extern char *lbl_3_data_F7E0[6];
extern char *lbl_3_data_F818[6];
extern char *lbl_3_data_F8C4[6];
extern char *lbl_3_data_F8F8[6];
extern char *lbl_3_data_F92C[6];
extern char *lbl_3_data_F960[6];
extern char *lbl_3_data_F988[6];
extern char **lbl_3_data_F9A0[4];
extern char *lbl_3_data_FA30[6];
extern char *lbl_3_data_FA6C[6];
extern char *lbl_3_data_FAE0[6];
extern char *lbl_3_data_FB50[6];
extern char *lbl_3_data_FB84[6];
extern char *lbl_3_data_FC8C[6];
extern char *lbl_3_data_FCE8[6];
extern char *lbl_3_data_FD90[6];
extern char *lbl_3_data_FE28[6];
extern char *lbl_3_data_FF00[6];
extern char **lbl_3_data_FF18[5];
extern char *lbl_3_data_FF38[6];
extern char *lbl_3_data_FF5C[6];
extern char *lbl_3_data_FFAC[6];
extern char *lbl_3_data_10028[6];
extern char *lbl_3_data_1006C[6];
extern char *lbl_3_data_100AC[6];
extern char *lbl_3_data_10124[6];
extern char *lbl_3_data_10154[6];
extern char *lbl_3_data_101C4[6];
extern char *lbl_3_data_10214[6];
extern char *lbl_3_data_10294[6];
extern char *lbl_3_data_102C0[6];
extern char *lbl_3_data_102EC[6];
extern char *lbl_3_data_10340[6];
extern char **lbl_3_data_10358[2];
extern char *lbl_3_data_10390[6];
extern char *lbl_3_data_10404[6];
extern char *lbl_3_data_10608[6];
extern char *lbl_3_data_1066C[6];
extern char *lbl_3_data_106B8[6];
extern char *lbl_3_data_10748[6];
extern char *lbl_3_data_107CC[6];
extern char *lbl_3_data_10820[6];
extern char *lbl_3_data_10874[6];
extern char *lbl_3_data_10958[6];
extern char *lbl_3_data_109EC[6];
extern char *lbl_3_data_10AF4[6];
extern char *lbl_3_data_10BCC[6];
extern char *lbl_3_data_10CC4[6];
extern char **lbl_3_data_10CDC[5];
extern char *lbl_3_data_10D74[6][3];
extern char *lbl_3_data_10E10[6][3];
extern char *lbl_3_data_1104C[6][3];
extern char *lbl_3_data_11300[6][3];
extern char *lbl_3_data_114BC[6][3];
extern char *lbl_3_data_115BC[6];
extern char *lbl_3_data_11644[6];
extern char *lbl_3_data_11714[6];
extern u8 lbl_3_data_EDC0[0x170];
extern struct GameDataOption lbl_3_data_F350[7];
extern struct GameDataStat lbl_3_data_F4A4[19];
extern struct ScreenOption lbl_3_data_F9B0[2];
extern struct ControllerOption lbl_3_data_FB9C[2];
extern u32 lbl_3_data_100C4[4];
extern struct RankingOption lbl_3_data_1022C[2];
extern s8 lbl_3_data_1023C[33];
extern s16 *lbl_3_data_10484[4];

/* ---- .bss ---- */
/* Layout (bss offsets relative to option_game_data_current_operation, bss 0x800). The target
 * addresses most of these through one base register; they are separate objects here. */
struct SelOptionWork option_game_data_current_operation;  /* 0x0000 */
s16 lbl_3_bss_82C[2];                          /* 0x002C: game-data debug view scroll x/y */
/* controller calibration (same as src/option/option.c) */
static struct StickPoint s_rawMax[4][8];       /* 0x0030: gate shape per pad */
static struct StickPoint s_seenMax[4][8];      /* 0x0070 */
struct StickPoint lbl_3_bss_8B0[4][8];         /* 0x00B0: s_seenMax clamped through the gate */
static Vec s_rawHistory[4][0x100];             /* 0x00F0 */
static u16 s_historyLen[4];                    /* 0x30F0 */
static s32 s_historyFull[4];                   /* 0x30F8 */
static s8 s_lastX[4];                          /* 0x3108 */
static s8 s_lastY[4];                          /* 0x310C */
static Vec s_clampedHistory[4][0x100];         /* 0x3110 */
s32 lbl_3_bss_6910;                            /* 0x6110 */
static struct OptionReplayListPos s_replayListPos; /* 0x6114 */
s32 lbl_3_bss_6920;                            /* 0x6120: replay preview texture loaded */
static struct OptionReplayFile s_replayFiles[0x80]; /* 0x6140 */
struct OptionReplayEntry lbl_3_bss_7D40[0x80]; /* 0x7540 */
s32 lbl_3_bss_8B40;                            /* 0x8340: number of replays */
s32 g_replay_stage_id_to_load;                 /* 0x8344 */
struct Preview lbl_3_bss_8B48;                 /* 0x8348 */
s8 lbl_3_bss_8BC0;                             /* 0x83C0 */
s8 lbl_3_bss_8BC1;                             /* 0x83C1: gift page */
static s8 s_giftMsgTimer;                      /* 0x83C2 */
s8 g_gift_menu_cursor_pos;                     /* 0x83C3: -1 = none */
static s32 s_giftBuyTimer;                     /* 0x83C4 */
s32 lbl_3_bss_8BC8[3];                         /* 0x83C8 */

/* Not referenced by any code in this file. */
u8 lbl_3_data_EDC0[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};
char *lbl_3_data_EF44[6] = {
    "Load",
    "",
    "",
    "",
    "",
    "\x83\x8D\x81\x5B\x83\x68",
};
char *lbl_3_data_EF6C[6] = {
    "Save",
    "",
    "",
    "",
    "",
    "\x83\x5A\x81\x5B\x83\x75",
};
char *lbl_3_data_EFA0[6] = {
    "Autosave",
    "",
    "",
    "",
    "",
    "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75",
};
char *lbl_3_data_EFE4[6] = {
    "Load -debug data-",
    "",
    "",
    "",
    "",
    "\x83\x66\x83\x6F\x83\x62\x83\x4F\x83\x66\x81\x5B\x83\x5E\x82\xCC\x83\x8D\x81\x5B\x83\x68",
};
char *lbl_3_data_F028[6] = {
    "Merge -debug data-",
    "",
    "",
    "",
    "",
    "\x83\x66\x83\x6F\x83\x62\x83\x4F\x83\x66\x81\x5B\x83\x5E\x82\xCC\x83\x7D\x81\x5B\x83\x57",
};
char *lbl_3_data_F06C[6] = {
    "Save -debug data-",
    "",
    "",
    "",
    "",
    "\x83\x66\x83\x6F\x83\x62\x83\x4F\x83\x66\x81\x5B\x83\x5E\x82\xCC\x83\x5A\x81\x5B\x83\x75",
};
char *lbl_3_data_F0A4[6] = {
    "Success rate disp",
    "",
    "",
    "",
    "",
    "\x83\x4E\x83\x8A\x83\x41\x82\xE8\x82\xC2",
};
char *lbl_3_data_F108[6] = {
    "This will load previously\nsaved Game Data.",
    "",
    "",
    "",
    "",
    "\x83\x51\x81\x5B\x83\x80\x83\x66\x81\x5B\x83\x5E\x82\xF0\x83\x8D\x81\x5B\x83\x68\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_F168[6] = {
    "This will save the current\nGame Data.",
    "",
    "",
    "",
    "",
    "\x83\x51\x81\x5B\x83\x80\x83\x66\x81\x5B\x83\x5E\x82\xF0\x83\x5A\x81\x5B\x83\x75\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_F1C8[6] = {
    "Will automatically save\nyour Game Data.",
    "",
    "",
    "",
    "",
    "\x82\xB6\x82\xC7\x82\xA4\x82\xC5\x83\x5A\x81\x5B\x83\x75\x82\xF0\x82\xA8\x82\xB1\x82\xC8\x82\xA2\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_F20C[6] = {
    "This will load previously\nsaved Debug Data.",
    "",
    "",
    "",
    "",
    "\x83\x51\x81\x5B\x83\x80\x83\x66\x81\x5B\x83\x5E\x82\xF0\x83\x8D\x81\x5B\x83\x68\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_F28C[6] = {
    "This will merge\npreviously saved Debug Data\nand the current Debug Data.",
    "",
    "",
    "",
    "",
    "\x83\x51\x81\x5B\x83\x80\x83\x66\x81\x5B\x83\x5E\x82\xF0\x83\x7D\x81\x5B\x83\x57\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_F2CC[6] = {
    "This will save the current\nDebug Data.",
    "",
    "",
    "",
    "",
    "\x83\x51\x81\x5B\x83\x80\x83\x66\x81\x5B\x83\x5E\x82\xF0\x83\x5A\x81\x5B\x83\x75\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_F338[6] = {
    "You can view the main game clear rate",
    "",
    "",
    "",
    "",
    "\x83\x81\x83\x43\x83\x93\x83\x51\x81\x5B\x83\x80\x82\xCC\x83\x4E\x83\x8A\x83\x41\x82\xE8\x82\xC2\x82\xF0\x82\xD0\x82\xE5\x82\xA4\x82\xB6\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
struct GameDataOption lbl_3_data_F350[7] = {
    { lbl_3_data_EF44, lbl_3_data_F108, 1, NULL },
    { lbl_3_data_EF6C, lbl_3_data_F168, 2, NULL },
    { lbl_3_data_EFA0, lbl_3_data_F1C8, 3, &storymode_save_files[0x31B] },
    { lbl_3_data_EFE4, lbl_3_data_F20C, 4, NULL },
    { lbl_3_data_F028, lbl_3_data_F28C, 5, NULL },
    { lbl_3_data_F06C, lbl_3_data_F2CC, 6, NULL },
    { lbl_3_data_F0A4, lbl_3_data_F338, 7, NULL },
};
struct GameDataStat lbl_3_data_F4A4[19] = {
    { "WORLD1  ", (u16 *)&lbl_1_bss_D1320[0xACC], (u16 *)&lbl_1_bss_D1320[0xA04], 10, 10, 0 },
    { "WORLD2  ", (u16 *)&lbl_1_bss_D1320[0xAE0], (u16 *)&lbl_1_bss_D1320[0xA18], 10, 10, 0 },
    { "WORLD3  ", (u16 *)&lbl_1_bss_D1320[0xAF4], (u16 *)&lbl_1_bss_D1320[0xA2C], 10, 10, 0 },
    { "WORLD4  ", (u16 *)&lbl_1_bss_D1320[0xB08], (u16 *)&lbl_1_bss_D1320[0xA40], 10, 10, 0 },
    { "WORLD5  ", (u16 *)&lbl_1_bss_D1320[0xB1C], (u16 *)&lbl_1_bss_D1320[0xA54], 10, 10, 0 },
    { "WORLD6  ", (u16 *)&lbl_1_bss_D1320[0xB30], (u16 *)&lbl_1_bss_D1320[0xA68], 10, 10, 0 },
    { "WORLD7  ", (u16 *)&lbl_1_bss_D1320[0xB44], (u16 *)&lbl_1_bss_D1320[0xA7C], 10, 10, 0 },
    { "WORLD8  ", (u16 *)&lbl_1_bss_D1320[0xB58], (u16 *)&lbl_1_bss_D1320[0xA90], 10, 10, 0 },
    { "WORLD9  ", (u16 *)&lbl_1_bss_D1320[0xB6C], (u16 *)&lbl_1_bss_D1320[0xAA4], 10, 10, 0 },
    { "WORLD10 ", (u16 *)&lbl_1_bss_D1320[0xB80], (u16 *)&lbl_1_bss_D1320[0xAB8], 10, 10, 0 },
    { "BEGINNER", (u16 *)&lbl_1_bss_D1320[0x7E8], (u16 *)&lbl_1_bss_D1320[0x7D4], 10, 10, 0 },
    { "BE EX   ", (u16 *)&lbl_1_bss_D1320[0x810], (u16 *)&lbl_1_bss_D1320[0x7FC], 10, 10, 0 },
    { "ADVANCED", (u16 *)&lbl_1_bss_D1320[0x860], (u16 *)&lbl_1_bss_D1320[0x824], 30, 30, 0 },
    { "BE AD   ", (u16 *)&lbl_1_bss_D1320[0x8B0], (u16 *)&lbl_1_bss_D1320[0x89C], 10, 10, 0 },
    { "EXPERT  ", (u16 *)&lbl_1_bss_D1320[0x928], (u16 *)&lbl_1_bss_D1320[0x8C4], 50, 25, 0 },
    { "EX EX   ", (u16 *)&lbl_1_bss_D1320[0x9A0], (u16 *)&lbl_1_bss_D1320[0x98C], 10, 10, 0 },
    { "MASTER  ", (u16 *)&lbl_1_bss_D1320[0x9C8], (u16 *)&lbl_1_bss_D1320[0x9B4], 10, 10, 0 },
    { "EX MA   ", (u16 *)&lbl_1_bss_D1320[0x9F0], (u16 *)&lbl_1_bss_D1320[0x9DC], 10, 10, 0 },
    { "LINEAR  ", (u16 *)&lbl_1_bss_D1320[0x3EC], (u16 *)&lbl_1_bss_D1320[0x4], 420, 35, 1 },
};

void sprite_gamedata_disp(struct Sprite *sprite) {
    struct GameDataOption *opt;
    struct GameDataStat *stat;
    f32 scale;
    f32 rate;
    s32 count;
    s32 i;
    s32 col;
    s32 page;
    s32 row;
    s32 idx;
    s32 x;
    s32 y;
    s32 played;
    s32 cleared;
    s32 failed;
    u32 glow;
    u32 k;
    char *str;

    set_ui_widescreen_scale_mtx(0x140);
    GXSetZMode_cached(1U, 7, 1U);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(0.11f);
    set_ui_element_sprite_pos(320.0f, 210.0f);
    set_ui_element_sprite_scale(1.0f, 1.5f);
    draw_ui_box(0x1701);
    count = 7;
    if ((dip_switches & 1) == 0 || (merged_digital_inputs[0] & 0x20) != 0) {
        count = 3;
    }
    for (i = 0, opt = lbl_3_data_F350; i < count; i++, opt++) {
        if (i >= 4) {
            textdraw_set_alpha(0.5f);
        }
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000U);
        textdraw_set_alignment(3);
        if (i == option_game_data_current_operation.cursor) {
            glow = (u32)(64.0 * fabs(math_sin((s16)(option_game_data_current_operation.timer * 0x180))));
            textdraw_set_mul_color(0xFFFF40U);
            textdraw_set_add_color(glow | (glow << 16) | (glow << 8));
        } else {
            textdraw_set_mul_color(0xC0C040U);
            textdraw_set_add_color(0U);
        }
        textdraw_set_scale(15.0f, 1.0f);
        textdraw_set_pos(320.0f, sprite->y + (f32)(i << 5));
        textdraw_print("_");
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0xA0000000U);
        if (i == option_game_data_current_operation.cursor) {
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_pos(sprite->x - 42.0f, 12.0f + (sprite->y + (f32)(i << 5)));
            scale = 26.0 / g_get_bmp_height(0x1700);
            set_ui_element_sprite_scale(26.0 / g_get_bmp_width(0x1700), scale);
            draw_ui_element(0x1700);
        }
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_scale(0.8f, 1.0f);
        textdraw_set_flags(0xA0000000U);
        x = sprite->x;
        y = sprite->y + (f32)(i << 5);
        textdraw_set_pos(x, y);
        textdraw_print(opt->name[locale]);
        if (opt->flag != NULL) {
            textdraw_set_alignment(3);
            textdraw_set_pos(x + 0xF0, y);
            if (option_game_data_current_operation.state != 3) {
                if (*opt->flag != 0) {
                    str = "ON";
                } else {
                    str = "OFF";
                }
                textdraw_printf("%s", str);
            } else {
                if (*opt->flag != 0) {
                    str = "<ON>";
                } else {
                    str = "<OFF>";
                }
                textdraw_printf("%s", str);
            }
            textdraw_set_alignment(0);
        }
    }
    textdraw_set_alpha(1.0f);
    textdraw_reset();
    fn_3_140CC(option_game_data_current_operation.timer);
    textdraw_set_pos(320.0f, 400.0f);
    textdraw_print_and_fit_to_width(390.0f, lbl_3_data_F350[option_game_data_current_operation.cursor].desc[locale]);
    reset_ui_widescreen_scale_mtx();

    /* debug: per-stage play statistics */
    if (option_game_data_current_operation.state == 7) {
        col = 0;
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_pos(320.0f, 240.0f);
        set_ui_element_sprite_scale(80.0f, 60.0f);
        set_ui_element_sprite_mult_color_from_u32(0);
        set_ui_element_sprite_alpha(0.8f);
        draw_ui_element(0x4B);
        textdraw_reset();
        textdraw_set_font(2);
        textdraw_set_scale(1.0f, 1.0714285f);
        for (k = 0, stat = lbl_3_data_F4A4; k < 19; k++, stat++) {
            debug_textdraw_set_pos(col * 9 + 1 - lbl_3_bss_82C[0], 0);
            debug_textdraw_vsprintf_buf_1(stat->name);
            for (page = 0; page < stat->total / stat->per; page++) {
                for (row = 0; row < stat->per; row++) {
                    idx = row + page * stat->per + stat->base;
                    cleared = stat->a[idx];
                    failed = stat->b[idx];
                    played = failed + cleared;
                    rate = 100.0 * ((f32)cleared / (f32)played);
                    debug_textdraw_set_pos((page + col) * 9 + 1 - lbl_3_bss_82C[0], row + 1);
                    switch (lbl_3_bss_82C[1]) {
                    case 0:
                        debug_textdraw_vsprintf_buf_1("%3d:%3.0f%%", idx + 1 - stat->base, rate);
                        break;
                    case 1:
                        debug_textdraw_vsprintf_buf_1("%3d:%4d", idx + 1 - stat->base, played);
                        break;
                    case 2:
                        debug_textdraw_vsprintf_buf_1("%3d:%4d", idx + 1 - stat->base, failed);
                        break;
                    case 3:
                        debug_textdraw_vsprintf_buf_1("%3d:%4d", idx + 1 - stat->base, cleared);
                        break;
                    }
                }
                debug_textdraw_set_pos((page + col) * 9 + 1 - lbl_3_bss_82C[0], 0x24);
                switch (lbl_3_bss_82C[1]) {
                case 0:
                    debug_textdraw_vsprintf_buf_1("RATE");
                    break;
                case 1:
                    debug_textdraw_vsprintf_buf_1("PLAYED");
                    break;
                case 2:
                    debug_textdraw_vsprintf_buf_1("FAILED");
                    break;
                case 3:
                    debug_textdraw_vsprintf_buf_1("CLEARED");
                    break;
                }
            }
            col += stat->total / stat->per;
        }
    }
    GXSetZMode_cached(1U, 3, 1U);
}

void create_gamedata_sprite(void) {
    struct Sprite *sprite;

    memset(&option_game_data_current_operation, 0, sizeof(option_game_data_current_operation));
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->x = 200.0f;
        sprite->y = 165.0f;
        sprite->depth = 0.0f;
        sprite->tag = 0x5F;
        sprite->drawFunc = sprite_gamedata_disp;
        strcpy(sprite->text, "gamedata");
    }
}

void menu_option_game_data_tick_child(void) {
    struct MemcardRequest req6;
    char name6[0x20];
    struct MemcardRequest req5;
    struct MemcardRequest req4;
    struct MemcardRequest req1;
    struct MemcardRequest req2;
    char name2[0x20];
    s16 var_r4;
    s16 var_r4_2;
    s32 temp_ret;
    s32 temp_ret_2;
    s32 temp_ret_3;
    s32 temp_ret_4;
    s32 temp_ret_5;
    s32 var_r29;
    s32 var_r30;
    s32 var_r31;
    s32 var_r3;
    s32 var_r3_2;
    struct GameDataOption *temp_r4;
    u8 var_r29_2;

    var_r31 = 0;
    if ((u32) option_game_data_current_operation.state <= 7U) {
        switch (option_game_data_current_operation.state) { /* switch 1 */
        case 0:                                     /* switch 1 */
            var_r29 = 7;
            if ((u32) (dip_switches & 1) == 0) {
                var_r29 = 3;
            }
            var_r30 = option_game_data_current_operation.cursor;
            if (did_any_pad_press_input(2) != 0) {
                var_r30 -= 1;
                if (var_r30 < 0) {
                    var_r30 = var_r29 - 1;
                }
            } else if (did_any_pad_press_input(3) != 0) {
                var_r30 += 1;
                if (var_r30 >= var_r29) {
                    var_r30 = 0;
                }
            }
            if (var_r30 != (s32) option_game_data_current_operation.cursor) {
                var_r31 = 2;
                option_game_data_current_operation.cursor = var_r30;
            }
            if ((did_any_pad_press_input(6) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
                var_r31 |= 1;
                temp_r4 = &lbl_3_data_F350[option_game_data_current_operation.cursor];
                if (temp_r4->flag != NULL) {
                    option_game_data_current_operation.savedValue = (s32) *temp_r4->flag;
                }
                option_game_data_current_operation.state = temp_r4->op;
            } else if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
                g_b_button_pressed_on_menu(1);
            }
            break;
        case 2:                                     /* switch 1 */
            switch ((s32) option_game_data_current_operation.subState) { /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                if (g_check_some_memcard_field() == 0) {
                    g_save_game_data();
                    memset(&req2, 0, sizeof(req2));
                    req2.file = 0;
                    g_sprintf_memcard_error(0, name2);
                    req2.comment = name2;
                    req2.buffer = storymode_save_files;
                    g_some_replay_func4(0, 4, &req2);
                    option_game_data_current_operation.subState += 1;
                }
                break;
            case 1:                                 /* switch 2 */
                if (g_check_some_memcard_field() == 0) {
                    if (g_get_some_memcard_var() != 0) {
                        smd_game_over_save_child();
                    } else {
                        temp_ret = g_get_result_code();
                        OSReport("result_code: %d ", temp_ret);
                    }
                    option_game_data_current_operation.state = 0;
                    option_game_data_current_operation.subState = 0;
                }
                break;
            }
            break;
        case 1:                                     /* switch 1 */
            switch ((s32) option_game_data_current_operation.subState) { /* switch 3; irregular */
            case 0:                                 /* switch 3 */
                if (g_check_some_memcard_field() == 0) {
                    memset(&req1, 0, sizeof(req1));
                    req1.file = 0;
                    req1.buffer = storymode_save_files;
                    g_some_replay_func4(1, 4, &req1);
                    option_game_data_current_operation.subState += 1;
                }
                break;
            case 1:                                 /* switch 3 */
                if (g_check_some_memcard_field() == 0) {
                    if (g_get_some_memcard_var() != 0) {
                        g_some_loading_function();
                        smd_game_over_save_child();
                    } else {
                        temp_ret_2 = g_get_result_code();
                        OSReport("result_code: %d ", temp_ret_2);
                    }
                    if (are_all_party_games_unlocked() != 0) {
                        var_r3 = 0x5A;
                    } else {
                        var_r3 = 5;
                    }
                    if (g_maybe_related_to_music_crossfading(var_r3) == -1) {
                        if (are_all_party_games_unlocked() != 0) {
                            var_r3_2 = 0x5A;
                        } else {
                            var_r3_2 = 5;
                        }
                        play_track_and_fade_out_other_tracks(var_r3_2, 0x1E, 0x3C);
                        lbl_3_bss_44.unk0 = -1;
                    }
                    option_game_data_current_operation.state = 0;
                    option_game_data_current_operation.subState = 0;
                }
                break;
            }
            break;
        case 3:                                     /* switch 1 */
            var_r29_2 = *lbl_3_data_F350[option_game_data_current_operation.cursor].flag;
            if ((did_any_pad_press_input(0) != 0) || (did_any_pad_press_input(1) != 0)) {
                var_r29_2 = var_r29_2 == 0;
            }
            if ((s32) var_r29_2 != (s32) *lbl_3_data_F350[option_game_data_current_operation.cursor].flag) {
                var_r31 = 2;
                *lbl_3_data_F350[option_game_data_current_operation.cursor].flag = var_r29_2;
            }
            if ((did_any_pad_press_input(6) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
                var_r31 |= 8;
                option_game_data_current_operation.state = 0;
            } else if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
                var_r31 |= 0x10;
                *lbl_3_data_F350[option_game_data_current_operation.cursor].flag = option_game_data_current_operation.savedValue;
                option_game_data_current_operation.state = 0;
            }
            break;
        case 4:                                     /* switch 1 */
            switch ((s32) option_game_data_current_operation.subState) { /* switch 4; irregular */
            case 0:                                 /* switch 4 */
                if (g_check_some_memcard_field() == 0) {
                    memset(&req4, 0, sizeof(req4));
                    req4.file = 1;
                    req4.buffer = lbl_1_bss_D1320;
                    g_some_replay_func4(1, 4, &req4);
                    option_game_data_current_operation.subState += 1;
                }
                break;
            case 1:                                 /* switch 4 */
                if (g_check_some_memcard_field() == 0) {
                    if (g_get_some_memcard_var() != 0) {
                        fn_1_F0330();
                    } else {
                        temp_ret_3 = g_get_result_code();
                        OSReport("result_code: %d ", temp_ret_3);
                    }
                    option_game_data_current_operation.state = 0;
                    option_game_data_current_operation.subState = 0;
                }
                break;
            }
            break;
        case 5:                                     /* switch 1 */
            switch ((s32) option_game_data_current_operation.subState) { /* switch 5; irregular */
            case 0:                                 /* switch 5 */
                if (g_check_some_memcard_field() == 0) {
                    fn_1_F034C();
                    memset(&req5, 0, sizeof(req5));
                    req5.file = 1;
                    req5.buffer = lbl_1_bss_D1320;
                    g_some_replay_func4(1, 4, &req5);
                    option_game_data_current_operation.subState += 1;
                }
                break;
            case 1:                                 /* switch 5 */
                if (g_check_some_memcard_field() == 0) {
                    if (g_get_some_memcard_var() != 0) {
                        fn_1_F0330();
                        fn_1_F0380();
                    } else {
                        temp_ret_4 = g_get_result_code();
                        OSReport("result_code: %d ", temp_ret_4);
                    }
                    option_game_data_current_operation.state = 0;
                    option_game_data_current_operation.subState = 0;
                }
                break;
            }
            break;
        case 6:                                     /* switch 1 */
            switch ((s32) option_game_data_current_operation.subState) { /* switch 6; irregular */
            case 0:                                 /* switch 6 */
                if (g_check_some_memcard_field() == 0) {
                    fn_1_F0314();
                    memset(&req6, 0, sizeof(req6));
                    req6.file = 1;
                    g_sprintf_memcard_error(1, name6);
                    req6.comment = name6;
                    req6.buffer = lbl_1_bss_D1320;
                    g_some_replay_func4(0, 4, &req6);
                    option_game_data_current_operation.subState += 1;
                }
                break;
            case 1:                                 /* switch 6 */
                if (g_check_some_memcard_field() == 0) {
                    if (g_get_some_memcard_var() == 0) {
                        temp_ret_5 = g_get_result_code();
                        OSReport("result_code: %d ", temp_ret_5);
                    }
                    option_game_data_current_operation.state = 0;
                    option_game_data_current_operation.subState = 0;
                }
                break;
            }
            break;
        case 7:                                     /* switch 1 */
            var_r4 = lbl_3_bss_82C[0];
            if (((s32) (pad_status_groups->unk0[0].button & 1) != 0) || ((s32) (analog_inputs[0][0] & 1) != 0)) {
                var_r4 -= 1;
                if (var_r4 < 0) {
                    var_r4 = 0;
                }
            } else if (((s32) (pad_status_groups->unk0[0].button & 2) != 0) || ((s32) (analog_inputs[0][0] & 2) != 0)) {
                var_r4 += 1;
                if (var_r4 > 0xE4) {
                    var_r4 = 0xE4;
                }
            }
            lbl_3_bss_82C[0] = var_r4;
            var_r4_2 = lbl_3_bss_82C[1];
            if ((s32) (pad_status_groups->unk0[2].button & 0x100) != 0) {
                var_r4_2 += 1;
                if (var_r4_2 >= 4) {
                    var_r4_2 = 0;
                }
            }
            lbl_3_bss_82C[1] = var_r4_2;
            if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
                option_game_data_current_operation.state = 0;
                option_game_data_current_operation.subState = 0;
            }
            break;
        }
    }
    if ((u32) (var_r31 & 1) != 0) {
        call_SoundReqID_arg_0(0x6A);
    } else if ((u32) (var_r31 & 2) != 0) {
        call_SoundReqID_arg_0(0x6C);
    } else if ((u32) (var_r31 & 4) != 0) {
        call_SoundReqID_arg_0(0x6B);
    } else if ((u32) (var_r31 & 8) != 0) {
        call_SoundReqID_arg_0(0x65);
    } else if ((u32) (var_r31 & 0x10) != 0) {
        call_SoundReqID_arg_0(0x66);
    }
    option_game_data_current_operation.timer += 1;
}

void destroy_gamedata_sprite(void) {
    destroy_sprite_with_unique_id(0x5F);
}

char *lbl_3_data_F6D0[6] = {
    "TV monitor",
    "",
    "",
    "",
    "",
    "\x82\x73\x82\x75\x83\x82\x83\x6A\x83\x5E\x81\x5B",
};
char *lbl_3_data_F708[6] = {
    "4:3 Normal",
    "",
    "",
    "",
    "",
    "\x82\xD0\x82\xE5\x82\xA4\x82\xB6\x82\xE3\x82\xF1(4:3)",
};
char *lbl_3_data_F73C[6] = {
    "16:9 Wide",
    "",
    "",
    "",
    "",
    "\x83\x8F\x83\x43\x83\x68(16:9)",
};
char *lbl_3_data_F770[6] = {
    "Letterbox",
    "",
    "",
    "",
    "",
    "\x83\x8C\x83\x5E\x81\x5B\x83\x7B\x83\x62\x83\x4E\x83\x58",
};
char **lbl_3_data_F788[3] = { lbl_3_data_F708, lbl_3_data_F73C, lbl_3_data_F770 };
char *lbl_3_data_F7E0[6] = {
    "Please select a ratio.",
    "",
    "",
    "",
    "",
    "\x82\xD0\x82\xE5\x82\xA4\x82\xB6\x82\xE3\x82\xF1\x82\xAA\x82\xDF\x82\xF1\x82\xA9\x81\x41\x83\x8F\x83\x43\x83\x68\x82\xAA\x82\xDF\x82\xF1\x82\xA9\x82\xF0 \x82\xA6\x82\xE7\x82\xD1\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_F818[6] = {
    "Split Screen",
    "",
    "",
    "",
    "",
    "\x82\xAA\x82\xDF\x82\xF1\x82\xD4\x82\xF1\x82\xA9\x82\xC2",
};
char *lbl_3_data_F8C4[6] = {
    "Allows you to choose the game screen\nlayout for 3 player simultaneous games.",
    "",
    "",
    "",
    "",
    "\x82\x52\x82\xC9\x82\xF1\x82\xC7\x82\xA4\x82\xB6\x83\x76\x83\x8C\x83\x43\x82\xC5\x82\xA0\x82\xBB\x82\xD4\x82\xC6\x82\xAB\x82\xCC\n\x83\x51\x81\x5B\x83\x80\x82\xAA\x82\xDF\x82\xF1\x83\x8C\x83\x43\x83\x41\x83\x45\x83\x67\x82\xF0\x82\xAB\x82\xDF\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_F8F8[6] = {
    "1 player wide",
    "",
    "",
    "",
    "",
    "1P \x83\x8F\x83\x43\x83\x68",
};
char *lbl_3_data_F92C[6] = {
    "2 player wide",
    "",
    "",
    "",
    "",
    "2P \x83\x8F\x83\x43\x83\x68",
};
char *lbl_3_data_F960[6] = {
    "3 player wide",
    "",
    "",
    "",
    "",
    "3P \x83\x8F\x83\x43\x83\x68",
};
char *lbl_3_data_F988[6] = {
    "LIVE",
    "",
    "",
    "",
    "",
    "\x83\x89\x83\x43\x83\x75",
};
char **lbl_3_data_F9A0[4] = { lbl_3_data_F8F8, lbl_3_data_F92C, lbl_3_data_F960, lbl_3_data_F988 };
struct ScreenOption lbl_3_data_F9B0[2] = {
    { lbl_3_data_F6D0, lbl_3_data_F7E0, &modeCtrl.wideMode, 3, lbl_3_data_F788, fn_3_1EF64 },
    { lbl_3_data_F818, lbl_3_data_F8C4, &modeCtrl.splitscreenMode, 4, lbl_3_data_F9A0, NULL },
};

void fn_3_201C8(struct Sprite *sprite) {
    struct ScreenOption *opt;
    f32 scale;
    s32 i;
    s32 x;
    s32 y;
    u32 glow;

    GXSetZMode_cached(1U, 7, 1U);
    set_ui_widescreen_scale_mtx(0x140);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(0.11f);
    set_ui_element_sprite_pos(320.0f, 210.0f);
    set_ui_element_sprite_scale(1.0f, 1.5f);
    draw_ui_box(0x1701);
    for (i = 0, opt = lbl_3_data_F9B0; i < sizeof(lbl_3_data_F9B0) / sizeof(lbl_3_data_F9B0[0]); i++, opt++) {
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000U);
        textdraw_set_alignment(3);
        if (i == option_game_data_current_operation.cursor) {
            glow = (u32)(64.0 * fabs(math_sin((s16)(option_game_data_current_operation.timer * 0x180))));
            textdraw_set_mul_color(0xFFFF40U);
            textdraw_set_add_color(glow | ((glow << 16) | (glow << 8)));
        } else {
            textdraw_set_mul_color(0xC0C040U);
            textdraw_set_add_color(0U);
        }
        textdraw_set_scale(15.0f, 1.0f);
        textdraw_set_pos(320.0f, sprite->y + (f32)(i << 5));
        textdraw_print("_");
        if (i == option_game_data_current_operation.cursor) {
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_pos(sprite->x - 26.0f, 12.0f + (sprite->y + (f32)(i << 5)));
            scale = 26.0 / g_get_bmp_height(0x1700);
            set_ui_element_sprite_scale(26.0 / g_get_bmp_width(0x1700), scale);
            draw_ui_element(0x1700);
        }
        x = sprite->x;
        y = sprite->y + (f32)(i << 5);
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_scale(0.8f, 1.0f);
        textdraw_set_flags(0xA0000000U);
        textdraw_set_pos(x, y);
        textdraw_print(opt->name[locale]);
        textdraw_set_alignment(3);
        textdraw_set_pos(x + 0xFA, y);
        if (option_game_data_current_operation.state == 1 && i == option_game_data_current_operation.cursor) {
            textdraw_printf("<%s>", opt->options[*opt->value][locale]);
        } else {
            textdraw_print(opt->options[*opt->value][locale]);
        }
    }
    textdraw_reset();
    fn_3_140CC(option_game_data_current_operation.timer);
    textdraw_set_pos(320.0f, 400.0f);
    textdraw_print_and_fit_to_width(390.0f, lbl_3_data_F9B0[option_game_data_current_operation.cursor].desc[locale]);
    reset_ui_widescreen_scale_mtx();
    GXSetZMode_cached(1U, 3, 1U);
}

void fn_3_20618(void) {
    struct Sprite *sprite;

    memset(&option_game_data_current_operation, 0, sizeof(option_game_data_current_operation));
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->x = 170.0f;
        sprite->y = 180.0f;
        sprite->tag = 0x5D;
        sprite->drawFunc = fn_3_201C8;
        strcpy(sprite->text, "screen");
    }
}

void fn_3_20698(void) {
    struct ScreenOption *opt;
    u32 sound;
    s32 sel;

    sound = 0;
    switch (option_game_data_current_operation.state) {
    case 0:
        sel = option_game_data_current_operation.cursor;
        if (did_any_pad_press_input(2) != 0) {
            sel -= 1;
            if (sel < 0) {
                sel = sizeof(lbl_3_data_F9B0) / sizeof(lbl_3_data_F9B0[0]) - 1;
            }
        } else if (did_any_pad_press_input(3) != 0) {
            sel += 1;
            if (sel >= sizeof(lbl_3_data_F9B0) / sizeof(lbl_3_data_F9B0[0])) {
                sel = 0;
            }
        }
        if (sel != option_game_data_current_operation.cursor) {
            sound |= 2;
            option_game_data_current_operation.cursor = sel;
        }
        if (did_any_pad_press_input(6) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            sound |= 8;
            option_game_data_current_operation.savedValue = *lbl_3_data_F9B0[option_game_data_current_operation.cursor].value;
            option_game_data_current_operation.state = 1;
        } else if (did_any_pad_press_input(7) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            g_b_button_pressed_on_menu(1);
        }
        break;
    case 1:
        opt = &lbl_3_data_F9B0[option_game_data_current_operation.cursor];
        sel = *opt->value;
        if (did_any_pad_press_input(0) != 0) {
            sel -= 1;
            if (sel < 0) {
                sel = opt->count - 1;
            }
        } else if (did_any_pad_press_input(1) != 0) {
            sel += 1;
            if (sel >= opt->count) {
                sel = 0;
            }
        }
        if (sel != *opt->value) {
            sound |= 2;
            *opt->value = sel;
            if (opt->callback != NULL) {
                opt->callback();
            }
        }
        if (did_any_pad_press_input(6) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            sound |= 8;
            option_game_data_current_operation.state = 0;
        } else if (did_any_pad_press_input(7) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            /* B: restore the value saved on entry */
            sound |= 0x10;
            *opt->value = option_game_data_current_operation.savedValue;
            if (opt->callback != NULL) {
                opt->callback();
            }
            option_game_data_current_operation.state = 0;
        }
        break;
    }
    if (sound & 1) {
        call_SoundReqID_arg_0(0x6A);
    } else if (sound & 2) {
        call_SoundReqID_arg_0(0x6C);
    } else if (sound & 4) {
        call_SoundReqID_arg_0(0x6B);
    } else if (sound & 8) {
        call_SoundReqID_arg_0(0x65);
    } else if (sound & 0x10) {
        call_SoundReqID_arg_0(0x66);
    }
    option_game_data_current_operation.timer += 1;
}

void fn_3_20A28(void) {
    destroy_sprite_with_unique_id(0x5D);
}

/* The next three helpers are file-local (static, not inline), as the .rodata order implies: MWCC
 * auto-inlines them into fn_3_20A4C but still compiles them out of line first, which is what puts
 * their literals at the front of fn_3_20A4C's part of .rodata; the REL linker (-strip_partial) then
 * drops the unreferenced out-of-line copies, so the target .text has none of them.
 * option_unused_half is a reconstruction: the target has an unreferenced 0.5 at exactly this point
 * of .rodata, i.e. some stripped helper used it. Its name and body are guesses; only the literal
 * matters. */

char *lbl_3_data_FA30[6] = {
    "Control Stick adjustment",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x8B\x83\x58\x83\x65\x83\x42\x83\x62\x83\x4E\x82\xBF\x82\xE5\x82\xA4\x82\xB9\x82\xA2",
};
char *lbl_3_data_FA6C[6] = {
    "Rumble Feature",
    "",
    "",
    "",
    "",
    "\x83\x6F\x83\x43\x83\x75\x83\x8C\x81\x5B\x83\x56\x83\x87\x83\x93",
};
char *lbl_3_data_FAE0[6] = {
    "Allows you to adjust\nthe Control Stick.",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x8B\x83\x58\x83\x65\x83\x42\x83\x62\x83\x4E\x82\xCC\n\x82\xBF\x82\xE5\x82\xA4\x82\xB9\x82\xA2\x82\xF0\x82\xA8\x82\xB1\x82\xC8\x82\xA2\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_FB50[6] = {
    "You can adjust the Rumble\nFeature settings.",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x89\x82\xCC\x82\xB5\x82\xF1\x82\xC7\x82\xA4\x82\xF0\x82\xB9\x82\xC1\x82\xC4\x82\xA2\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_FB84[6] = {
    "Controller",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x89",
};
struct ControllerOption lbl_3_data_FB9C[2] = {
    { lbl_3_data_FA30, lbl_3_data_FAE0, 1 },
    { lbl_3_data_FA6C, lbl_3_data_FB50, 3 },
};
char *lbl_3_data_FC8C[6] = {
    "Please move the Control Sticks\nfor all the connected Controllers around.\nWhen you are finished, press \x83\x9F.",
    "",
    "",
    "",
    "",
    "\x82\xB7\x82\xD7\x82\xC4\x82\xCC\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x89\x82\xCC\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x8B\x83\x58\x83\x65\x83\x42\x83\x62\x83\x4E\x82\xF0\n\x82\xA2\x82\xC1\x82\xCF\x82\xA2\x82\xC9\x82\xDC\x82\xED\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42\n\x82\xE6\x82\xAF\x82\xEA\x82\xCE\x83\x9F\x82\xF0\x82\xA8\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42",
};
char *lbl_3_data_FCE8[6] = {
    "Have you finished adjusting?\n ",
    "",
    "",
    "",
    "",
    "\x82\xB1\x82\xCC\x82\xBF\x82\xE5\x82\xA4\x82\xB9\x82\xA2\x82\xC5\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48\n ",
};
char *lbl_3_data_FD90[6] = {
    "The current settings will\nbe used from now on.\nPlease press any button.",
    "",
    "",
    "",
    "",
    "\x82\xB1\x82\xF1\x82\xB2\x81\x41\x82\xB1\x82\xCC\x82\xBF\x82\xE5\x82\xA4\x82\xB9\x82\xA2\x82\xF0\x82\xB5\x82\xE6\x82\xA4\x82\xB5\x82\xDC\x82\xB7\x81\x42\n\x82\xC8\x82\xC9\x82\xA9\x83\x7B\x83\x5E\x83\x93\x82\xF0\x82\xA8\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42",
};
char *lbl_3_data_FE28[6] = {
    "The current settings\nwill not be used.\nPlease press any button.",
    "",
    "",
    "",
    "",
    "\x82\xB1\x82\xCC\x82\xBF\x82\xE5\x82\xA4\x82\xB9\x82\xA2\x82\xCD\x82\xB5\x82\xE6\x82\xA4\x82\xB5\x82\xDC\x82\xB9\x82\xF1\x81\x42\n\x82\xC8\x82\xC9\x82\xA9\x83\x7B\x83\x5E\x83\x93\x82\xF0\x82\xA8\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42",
};
char *lbl_3_data_FF00[6] = {
    "Adjustments not completed.\nPlease move the Control Stick\nfor all connected Controllers.\n",
    "",
    "",
    "",
    "",
    "\x82\xBF\x82\xE5\x82\xA4\x82\xB9\x82\xA2\x82\xAA\x82\xA9\x82\xF1\x82\xBA\x82\xF1\x82\xC5\x82\xCD\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x42\n\x82\xB7\x82\xD7\x82\xC4\x82\xCC\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x89\x82\xC5\x81\x41\n\x82\xAB\x82\xBF\x82\xF1\x82\xC6\x82\xC9\x82\xE3\x82\xA4\x82\xE8\x82\xE5\x82\xAD\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42",
};
char **lbl_3_data_FF18[5] = { lbl_3_data_FC8C, lbl_3_data_FCE8, lbl_3_data_FD90, lbl_3_data_FE28, lbl_3_data_FF00 };
char *lbl_3_data_FF38[6] = {
    "Yes",
    "",
    "",
    "",
    "",
    "\x82\xCD\x82\xA2",
};
char *lbl_3_data_FF5C[6] = {
    "No",
    "",
    "",
    "",
    "",
    "\x82\xA2\x82\xA2\x82\xA6",
};
char *lbl_3_data_FFAC[6] = {
    "No Controller\nconnected",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x89\x82\xAA\n\x82\xB3\x82\xB3\x82\xC1\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1",
};
char *lbl_3_data_10028[6] = {
    "Adjust the Rumble Feature settings\nfor Controller %d.",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x93\x83\x67\x83\x8D\x81\x5B\x83\x89%d\x82\xCC\x82\xB5\x82\xF1\x82\xC7\x82\xA4\x82\xF0\x82\xB9\x82\xC1\x82\xC4\x82\xA2\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_3_data_1006C[6] = {
    "Block",
    "Block",
    "Bloc",
    "Bloque",
    "Blocco",
    "\x83\x75\x83\x8D\x83\x62\x83\x4E",
};
char *lbl_3_data_100AC[6] = {
    "Blocks",
    "Bl\xF6\x63ke",
    "Blocs",
    "Bloques",
    "Blocchi",
    "\x83\x75\x83\x8D\x83\x62\x83\x4E",
};
u32 lbl_3_data_100C4[4] = { 0x00FF7F7F, 0x007F7FFF, 0x00FFFF7F, 0x007FFF7F };

static inline void reset_stick_gate(int i) {
    s_rawMax[i][0] = (struct StickPoint){0x54, 0};
    s_rawMax[i][1] = (struct StickPoint){0x3B, 0x3B};
    s_rawMax[i][2] = (struct StickPoint){0, 0x54};
    s_rawMax[i][3] = (struct StickPoint){-0x3B, 0x3B};
    s_rawMax[i][4] = (struct StickPoint){-0x54, 0};
    s_rawMax[i][5] = (struct StickPoint){-0x3B, -0x3B};
    s_rawMax[i][6] = (struct StickPoint){0, -0x54};
    s_rawMax[i][7] = (struct StickPoint){0x3B, -0x3B};
}

static f64 option_unused_half(f64 x) {
    return x * 0.5;
}

/* blinking highlight: 0 -> 255 -> 0 over one second, as a grey RGB value */

static u32 option_blink_gray(void) {
    f32 t;
    u8 c;

    t = (f32)(option_game_data_current_operation.timer % 60) / 60.0f;
    t = 1.0 - fabs((f32)(2.0 * t - 1.0));
    t = fabs(t);
    t = t * 255.0;
    option_f32_to_u8(&t, &c);
    return c | ((c << 16) | (c << 8));
}

/* in 16:9 mode, squash the controller test screen back to 4:3 proportions */

static void option_load_wide_mtx(void) {
    if (modeCtrl.wideMode != 0) {
        mtxa_from_translate_xyz(320.0f, 240.0f, 0.0f);
        mtxa_scale_xyz(0.86603516f, 1.1538461f, 1.0f);
        mtxa_translate_xyz(-320.0f, -240.0f, 0.0f);
        GXLoadPosMtxImm(mtxa->mtxA, 0U);
    }
}

void fn_3_20A4C(struct Sprite *sprite) {
    struct ControllerOption *opt;
    f32 scale;
    f32 x;
    f32 y;
    s32 i;
    u32 glow;
    u8 rumble;
    char *str;

    GXSetZMode_cached(1U, 7, 1U);
    switch (option_game_data_current_operation.state) {
    case 0:
        set_ui_widescreen_scale_mtx(0x140);
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_depth(0.11f);
        set_ui_element_sprite_pos(320.0f, 210.0f);
        set_ui_element_sprite_scale(1.0f, 1.5f);
        draw_ui_box(0x1701);
        for (i = 0, opt = lbl_3_data_FB9C; i < sizeof(lbl_3_data_FB9C) / sizeof(lbl_3_data_FB9C[0]); i++, opt++) {
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_flags(0x80000000U);
            textdraw_set_alignment(3);
            if (i == option_game_data_current_operation.cursor) {
                glow = (u32)(64.0 * fabs(math_sin((s16)(option_game_data_current_operation.timer * 0x180))));
                textdraw_set_mul_color(0xFFFF40U);
                textdraw_set_add_color(glow | ((glow << 16) | (glow << 8)));
            } else {
                textdraw_set_mul_color(0xC0C040U);
                textdraw_set_add_color(0U);
            }
            textdraw_set_scale(15.0f, 1.0f);
            textdraw_set_pos(320.0f, sprite->y + (f32)(i << 5));
            textdraw_print("_");
            if (i == option_game_data_current_operation.cursor) {
                init_ui_element_sprite_with_defaults();
                set_ui_element_sprite_pos(sprite->x - 58.0f, 12.0f + (sprite->y + (f32)(i << 5)));
                scale = 26.0 / g_get_bmp_height(0x1700);
                set_ui_element_sprite_scale(26.0 / g_get_bmp_width(0x1700), scale);
                draw_ui_element(0x1700);
            }
            x = sprite->x - 20.0f;
            y = sprite->y + 32.0 * i;
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_flags(0xA0000000U);
            textdraw_set_scale(0.8f, 1.0f);
            textdraw_set_pos(x, y);
            textdraw_print(opt->name[locale]);
        }
        fn_3_140CC(option_game_data_current_operation.timer);
        opt = &lbl_3_data_FB9C[option_game_data_current_operation.cursor];
        textdraw_set_pos(320.0f, 400.0f);
        textdraw_print_and_fit_to_width(390.0f, opt->desc[locale]);
        reset_ui_widescreen_scale_mtx();
        break;
    case 2:
        option_load_wide_mtx();
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0xA0000000U);
        textdraw_set_scale(0.6135f, 0.6135f);
        textdraw_set_alignment(4);
        for (i = 0; i < 4; i++) {
            textdraw_set_pos(320.0f + 73.0f * (f32)(i * 2 - 3), 150.0f);
            textdraw_set_mul_color(0xFFFFFFU);
            textdraw_printf("%s%d", lbl_3_data_FB84[locale], i + 1);
            if (controllerInfo[i].unk0[0].err == -1) {
                textdraw_set_pos(320.0f + 73.0f * (f32)(i * 2 - 3), 233.0f);
                textdraw_set_depth(1.0f);
                textdraw_set_mul_color(lbl_3_data_100C4[i]);
                textdraw_print(lbl_3_data_FFAC[locale]);
            } else if (option_game_data_current_operation.subState <= 0) {
                textdraw_set_pos(320.0 + 320.0 * (((f32)pad_status_groups[i].unk0[0].stickX + 70.0f * (f32)(i * 2 - 3)) / 307.92),
                                 240.0 + 320.0 * ((f32)-pad_status_groups[i].unk0[0].stickY / 307.92));
                textdraw_set_mul_color(0xFFFFFFU);
                textdraw_print("\x83\xAD");
            }
        }
        option_load_wide_mtx();
        set_ui_widescreen_scale_mtx(0x140);
        textdraw_reset();
        fn_3_140CC(option_game_data_current_operation.timer);
        textdraw_set_flags(0x10000000U);
        textdraw_set_pos(320.0f, 400.0f);
        textdraw_print_and_fit_to_width(390.0f, lbl_3_data_FF18[option_game_data_current_operation.subState][locale]);
        if (option_game_data_current_operation.subState == 1) {
            x = 320.0f;
            y = 410.0f;
            textdraw_set_pos(x - 62.0, y);
            textdraw_set_alignment(3);
            textdraw_clear_drop_shadow();
            textdraw_set_border();
            if (option_game_data_current_operation.subCursor == 1) {
                textdraw_set_mul_color(0xFFFFFF00U);
                textdraw_print("\x81\x6D      \x81\x6E");
                textdraw_set_pos(x - 62.0, y);
                textdraw_set_add_color(option_blink_gray());
            } else {
                textdraw_set_mul_color(0xFF7F7F00U);
                textdraw_set_add_color(0U);
            }
            textdraw_print(lbl_3_data_FF38[locale]);
            textdraw_set_pos(62.0 + x, y);
            textdraw_set_alignment(3);
            if (option_game_data_current_operation.subCursor == 0) {
                textdraw_set_mul_color(0xFFFFFF00U);
                textdraw_print("\x81\x6D      \x81\x6E");
                textdraw_set_pos(62.0 + x, y);
                textdraw_set_add_color(option_blink_gray());
            } else {
                textdraw_set_mul_color(0xFF7F7F00U);
                textdraw_set_add_color(0U);
            }
            textdraw_print(lbl_3_data_FF5C[locale]);
        }
        reset_ui_widescreen_scale_mtx();
        break;
    case 3:
    case 4:
        set_ui_widescreen_scale_mtx(0x140);
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_depth(0.11f);
        set_ui_element_sprite_pos(320.0f, 210.0f);
        set_ui_element_sprite_scale(1.0f, 1.5f);
        draw_ui_box(0x1701);
        rumble = fn_1_FD63C();
        for (i = 0; i < 4; i++) {
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_flags(0x80000000U);
            textdraw_set_alignment(3);
            if (i == option_game_data_current_operation.subCursor) {
                glow = (u32)(64.0 * fabs(math_sin((s16)(option_game_data_current_operation.timer * 0x180))));
                textdraw_set_mul_color(0xFFFF40U);
                textdraw_set_add_color(glow | ((glow << 16) | (glow << 8)));
            } else {
                textdraw_set_mul_color(0xC0C040U);
                textdraw_set_add_color(0U);
            }
            textdraw_set_scale(15.0f, 1.0f);
            textdraw_set_pos(320.0f, (sprite->y + (f32)(i << 5)) - 30.0);
            textdraw_print("_");
            if (i == option_game_data_current_operation.subCursor) {
                init_ui_element_sprite_with_defaults();
                set_ui_element_sprite_pos(sprite->x - 60.0, 12.0 + ((sprite->y + (f32)(i << 5)) - 30.0));
                scale = 26.0 / g_get_bmp_height(0x1700);
                set_ui_element_sprite_scale(26.0 / g_get_bmp_width(0x1700), scale);
                draw_ui_element(0x1700);
            }
            x = sprite->x - 20.0;
            y = (sprite->y + 32.0 * i) - 30.0;
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_scale(0.8f, 1.0f);
            textdraw_set_flags(0xA0000000U);
            textdraw_set_pos(x, y);
            textdraw_printf("%s%d", lbl_3_data_FB84[locale], i + 1);
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_scale(0.8f, 1.0f);
            textdraw_set_flags(0xA0000000U);
            textdraw_set_alignment(3);
            textdraw_set_pos(260.0f + x, y);
            if (option_game_data_current_operation.state == 4 && option_game_data_current_operation.subCursor == i) {
                if (rumble & (1 << i)) {
                    str = "ON";
                } else {
                    str = "OFF";
                }
                textdraw_printf("<%s>", str);
            } else {
                if (rumble & (1 << i)) {
                    str = "ON";
                } else {
                    str = "OFF";
                }
                textdraw_printf("%s", str);
            }
        }
        fn_3_140CC(option_game_data_current_operation.timer);
        textdraw_set_pos(320.0f, 400.0f);
        textdraw_printf_and_fit_to_width(390.0f, lbl_3_data_10028[locale], option_game_data_current_operation.subCursor + 1);
        reset_ui_widescreen_scale_mtx();
        break;
    default:
        set_ui_widescreen_scale_mtx(0x140);
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_depth(0.11f);
        set_ui_element_sprite_pos(320.0f, 210.0f);
        set_ui_element_sprite_scale(1.45f, 1.5f);
        draw_ui_box(0x1701);
        reset_ui_widescreen_scale_mtx();
        break;
    }
    GXSetZMode_cached(1U, 3, 1U);
}

void fn_3_2181C(struct Sprite *sprite) {
    switch (option_game_data_current_operation.state) {
    case 2:
        if (modeCtrl.wideMode != 0) {
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_pos(320.0f, 222.0f);
            set_ui_element_sprite_depth(500.0f);
            set_ui_element_sprite_scale(520.0 / g_get_bmp_width(0x1701), 212.0 / g_get_bmp_height(0x1701));
            g_set_some_ui_element_sprite_flag(0x40000);
            draw_ui_box(0x1701);
        } else {
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_pos(320.0f, 224.0f);
            set_ui_element_sprite_depth(500.0f);
            set_ui_element_sprite_scale(600.0 / g_get_bmp_width(0x1701), 200.0 / g_get_bmp_height(0x1701));
            g_set_some_ui_element_sprite_flag(0x40000);
            draw_ui_box(0x1701);
        }
        break;
    }
}

void fn_3_219A8(void) {
    Vec sp1C[9];
    GXColor spC[4] = {
        { 0xFF, 0x00, 0x00, 0xFF },
        { 0x00, 0x00, 0xFF, 0xFF },
        { 0xFF, 0xFF, 0x00, 0xFF },
        { 0x00, 0xFF, 0x00, 0xFF },
    };
    GXColor sp8 = { 0x00, 0x00, 0x00, 0xFF };
    Vec sp88[9] = {
        { 60.0f, 0.0f, 0.0f },
        { 60.0f, 60.0f, 0.0f },
        { 0.0f, 60.0f, 0.0f },
        { -60.0f, 60.0f, 0.0f },
        { -60.0f, 0.0f, 0.0f },
        { -60.0f, -60.0f, 0.0f },
        { 0.0f, -60.0f, 0.0f },
        { 60.0f, -60.0f, 0.0f },
        { 60.0f, 0.0f, 0.0f },
    };
    s32 var_r30;
    s32 var_r3;

    if (option_game_data_current_operation.state == 2) {
    GXSetZMode_cached(1U, 7, 1U);
    mtxa_from_identity();
    fn_1_E2FD0(0x18);
    fn_1_E2FE0(1, 1, 0, 0);
    for (var_r3 = 0; var_r3 < 9; var_r3++) {
        sp1C[var_r3].z = -400.0f;
    }
    for (var_r30 = 0; var_r30 < 4; var_r30++) {
        if (((struct ControllerInfo *)&lbl_80145210)[var_r30].unk0[0].err != -1) {
            for (var_r3 = 0; var_r3 < 8; var_r3++) {
                sp1C[var_r3].x = sp88[var_r3].x + (70.0f * (f32) ((var_r30 * 2) - 3));
                sp1C[var_r3].y = sp88[var_r3].y;
            }
            sp1C[8] = sp1C[0];
            fn_1_E3164(9, sp1C, &sp8);
            for (var_r3 = 0; var_r3 < 8; var_r3++) {
                sp1C[var_r3].x = (70.0f * (f32) ((var_r30 * 2) - 3)) + (f32) lbl_3_bss_8B0[var_r30][var_r3].x;
                sp1C[var_r3].y = (f32) lbl_3_bss_8B0[var_r30][var_r3].y;
            }
            sp1C[8] = sp1C[0];
            fn_1_E3164(9, sp1C, &spC[var_r30]);
        }
    }
    GXSetZMode_cached(1U, 3, 1U);
    }
}

void g_draw_controller_sprites(void) {
    struct Sprite *sprite;
    struct Sprite *sprite2;

    memset(&option_game_data_current_operation, 0, sizeof(option_game_data_current_operation));
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = 0x5E;
        sprite->x = 214.0f;
        sprite->y = 180.0f;
        sprite->drawFunc = fn_3_20A4C;
        strcpy(sprite->text, "controllers");
    }
    sprite2 = create_sprite();
    if (sprite2 != NULL) {
        sprite2->tag = 0x5E;
        sprite2->flags |= 0x40000;
        sprite2->drawFunc = fn_3_2181C;
        strcpy(sprite2->text, "controller waku");
    }
    option_game_data_current_operation.tick = fn_3_219A8;
}

/* a pad's stick readings start from the shape of the octagonal gate */

void fn_3_21D50(void) {
    s32 sound = 0;
    s32 sel;
    s32 i;

    switch (option_game_data_current_operation.state) {
    case 0:
        sel = option_game_data_current_operation.cursor;
        if (did_any_pad_press_input(2) != 0) {
            sel -= 1;
            if (sel < 0) {
                sel = 1;
            }
        } else if (did_any_pad_press_input(3) != 0) {
            sel += 1;
            if ((u32) sel >= 2U) {
                sel = 0;
            }
        }
        if (sel != option_game_data_current_operation.cursor) {
            sound = 2;
            option_game_data_current_operation.cursor = sel;
        }
        if (did_any_pad_press_input(6) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            sound |= 1;
            option_game_data_current_operation.state = lbl_3_data_FB9C[option_game_data_current_operation.cursor].unk8;
            option_game_data_current_operation.subState = 0;
        } else if (did_any_pad_press_input(7) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            g_b_button_pressed_on_menu(1);
        }
        break;
    case 1:
        memset(s_rawHistory, 0, sizeof(s_rawHistory));
        memset(s_historyLen, 0, sizeof(s_historyLen));
        for (i = 0; i < 4; i++) {
            s_historyFull[i] = 0;
            s_lastX[i] = s_lastY[i] = -0x80;
            reset_stick_gate(i);
        }
        memset(s_seenMax, 0, sizeof(s_seenMax));
        memset(lbl_3_bss_8B0, 0, sizeof(lbl_3_bss_8B0));
        option_game_data_current_operation.state = 2;
        /* fallthrough */
    case 2:
        switch (option_game_data_current_operation.subState) {
        case 0:
        default:
            for (i = 0; i < 4; i++) {
                if (pad_status_groups[i].unk0[0].err == -1 || pad_status_groups[i].unk0[0].err == -3) {
                    memset(s_rawHistory[i], 0, sizeof(s_rawHistory[i]));
                    s_historyLen[i] = 0;
                    s_historyFull[i] = 0;
                    s_lastX[i] = s_lastY[i] = -0x80;
                    reset_stick_gate(i);
                    memset(s_seenMax[i], 0, sizeof(s_seenMax[i]));
                    memset(lbl_3_bss_8B0[i], 0, sizeof(lbl_3_bss_8B0[i]));
                } else {
                    struct StickPoint point;
                    s8 x = controllerInfo[i].unk0[0].stickX;
                    s8 y = controllerInfo[i].unk0[0].stickY;
                    f32 fx;
                    f32 fy;

                    if (s_rawMax[i][0].x < x) {
                        s_rawMax[i][0].x = x;
                        s_rawMax[i][0].y = y;
                    }
                    if (s_rawMax[i][4].x > x) {
                        s_rawMax[i][4].x = x;
                        s_rawMax[i][4].y = y;
                    }
                    if (s_rawMax[i][2].y < y) {
                        s_rawMax[i][2].x = x;
                        s_rawMax[i][2].y = y;
                    }
                    if (s_rawMax[i][6].y > y) {
                        s_rawMax[i][6].x = x;
                        s_rawMax[i][6].y = y;
                    }
                    if (s_seenMax[i][0].x < x) {
                        s_seenMax[i][0].x = lbl_3_bss_8B0[i][0].x = x;
                        s_seenMax[i][0].y = lbl_3_bss_8B0[i][0].y = y;
                        fn_8000A380(&lbl_3_bss_8B0[i][0], s_rawMax[i]);
                    }
                    if (s_seenMax[i][4].x > x) {
                        s_seenMax[i][4].x = lbl_3_bss_8B0[i][4].x = x;
                        s_seenMax[i][4].y = lbl_3_bss_8B0[i][4].y = y;
                        fn_8000A380(&lbl_3_bss_8B0[i][4], s_rawMax[i]);
                    }
                    if (s_seenMax[i][2].y < y) {
                        s_seenMax[i][2].x = lbl_3_bss_8B0[i][2].x = x;
                        s_seenMax[i][2].y = lbl_3_bss_8B0[i][2].y = y;
                        fn_8000A380(&lbl_3_bss_8B0[i][2], s_rawMax[i]);
                    }
                    if (s_seenMax[i][6].y > y) {
                        s_seenMax[i][6].x = lbl_3_bss_8B0[i][6].x = x;
                        s_seenMax[i][6].y = lbl_3_bss_8B0[i][6].y = y;
                        fn_8000A380(&lbl_3_bss_8B0[i][6], s_rawMax[i]);
                    }
                    if (abs(abs(x) - abs(y)) < 0x10) {
                        s32 k;
                        f32 rawDist;
                        f32 seenDist;
                        f32 dist;

                        if (y > 0) {
                            k = (x > 0) ? 1 : 3;
                        } else if (x < 0) {
                            k = 5;
                        } else {
                            k = 7;
                        }
                        rawDist = (f32) s_rawMax[i][k].y * (f32) s_rawMax[i][k].y + (f32) s_rawMax[i][k].x * (f32) s_rawMax[i][k].x;
                        seenDist = (f32) s_seenMax[i][k].y * (f32) s_seenMax[i][k].y + (f32) s_seenMax[i][k].x * (f32) s_seenMax[i][k].x;
                        dist = (f32) y * (f32) y + (f32) x * (f32) x;
                        if (rawDist < dist) {
                            s_rawMax[i][k].x = x;
                            s_rawMax[i][k].y = y;
                        }
                        if (seenDist < dist) {
                            s_seenMax[i][k].x = lbl_3_bss_8B0[i][k].x = x;
                            s_seenMax[i][k].y = lbl_3_bss_8B0[i][k].y = y;
                            fn_8000A380(&lbl_3_bss_8B0[i][k], s_rawMax[i]);
                        }
                    }
                    point.x = x;
                    point.y = y;
                    fn_8000A380(&point, s_rawMax[i]);
                    fx = 1.7f * point.x;
                    fy = 1.7f * point.y;
                    if (abs(s_lastX[i] - x) > 1 || abs(s_lastY[i] - y) > 1) {
                        s_lastX[i] = x;
                        s_lastY[i] = y;
                        if (s_historyLen[i] == 0x100) {
                            s_historyFull[i] = 1;
                            s_historyLen[i] = 0;
                        }
                        s_clampedHistory[i][s_historyLen[i]] = (Vec){0.01f * fx, 0.01f * fy, -2.0f};
                        s_rawHistory[i][s_historyLen[i]++] = (Vec){0.01f * x, 0.01f * y, -2.0f};
                    }
                }
            }
            if (did_any_pad_press_input(6) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
                sound = 1;
                option_game_data_current_operation.subCursor = 0;
                option_game_data_current_operation.subState = 1;
            } else if (did_any_pad_press_input(7) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
                sound = 4;
                option_game_data_current_operation.state = 0;
            }
            break;
        case 1:
            sel = option_game_data_current_operation.subCursor;
            if (did_any_pad_press_input(1) != 0) {
                sel = 0;
            }
            if (did_any_pad_press_input(0) != 0) {
                sel = 1;
            }
            if (sel != option_game_data_current_operation.subCursor) {
                sound = 2;
                option_game_data_current_operation.subCursor = sel;
            }
            if (did_any_pad_press_input(6) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
                if (option_game_data_current_operation.subCursor == 1) {
                    sound |= 1;
                    for (i = 0; i < 4; i++) {
                        if (pad_status_groups[i].unk0[0].err != -1
                         && pad_status_groups[i].unk0[0].err != -3
                         && (lbl_3_bss_8B0[i][0].x != 0x3C
                          || lbl_3_bss_8B0[i][1].x != 0x3C
                          || lbl_3_bss_8B0[i][1].y != 0x3C
                          || lbl_3_bss_8B0[i][2].y != 0x3C
                          || lbl_3_bss_8B0[i][3].x != -0x3C
                          || lbl_3_bss_8B0[i][3].y != 0x3C
                          || lbl_3_bss_8B0[i][4].x != -0x3C
                          || lbl_3_bss_8B0[i][5].x != -0x3C
                          || lbl_3_bss_8B0[i][5].y != -0x3C
                          || lbl_3_bss_8B0[i][6].y != -0x3C
                          || lbl_3_bss_8B0[i][7].x != 0x3C
                          || lbl_3_bss_8B0[i][7].y != -0x3C)) {
                            option_game_data_current_operation.subState = 4;
                            break;
                        }
                    }
                    if (option_game_data_current_operation.subState != 4) {
                        memcpy_handler(&lbl_80081AC0, s_rawMax, sizeof(s_rawMax));
                        option_game_data_current_operation.subState = 2;
                    }
                } else {
                    sound |= 4;
                    option_game_data_current_operation.subState = 3;
                }
            } else if (did_any_pad_press_input(7) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
                sound |= 4;
                option_game_data_current_operation.subState = 3;
            }
            break;
        case 2:
        case 3:
            if (merged_digital_inputs[2] != 0) {
                sound = 4;
                option_game_data_current_operation.state = 0;
            }
            break;
        case 4:
            if (merged_digital_inputs[2] != 0) {
                sound = 4;
                option_game_data_current_operation.subState = 0;
            }
            break;
        }
        break;
    case 3:
        sel = option_game_data_current_operation.subCursor;
        if (did_any_pad_press_input(2) != 0) {
            sel -= 1;
            if (sel < 0) {
                sel = 3;
            }
        } else if (did_any_pad_press_input(3) != 0) {
            sel += 1;
            if (sel >= 4) {
                sel = 0;
            }
        }
        if (sel != option_game_data_current_operation.subCursor) {
            sound = 2;
            option_game_data_current_operation.subCursor = sel;
        }
        if (did_any_pad_press_input(6) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            option_game_data_current_operation.state = 4;
            option_game_data_current_operation.savedValue = fn_1_FD63C();
        } else if (did_any_pad_press_input(7) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            sound |= 4;
            option_game_data_current_operation.state = 0;
        }
        break;
    case 4:
        sel = fn_1_FD63C();
        if (did_any_pad_press_input(0) != 0 || did_any_pad_press_input(1) != 0) {
            sound = 2;
            fn_1_FD62C(sel ^ (1 << option_game_data_current_operation.subCursor));
        }
        if (did_any_pad_press_input(6) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            sound |= 8;
            option_game_data_current_operation.state = 3;
        } else if (did_any_pad_press_input(7) != 0 && menuWork->unkC == 0 && menuWork->unk18 == 0) {
            sound |= 0x10;
            fn_1_FD62C(option_game_data_current_operation.savedValue);
            option_game_data_current_operation.state = 3;
        }
        break;
    }
    if (sound & 1) {
        call_SoundReqID_arg_0(0x6A);
    } else if (sound & 2) {
        call_SoundReqID_arg_0(0x6C);
    } else if (sound & 4) {
        call_SoundReqID_arg_0(0x6B);
    } else if (sound & 8) {
        call_SoundReqID_arg_0(0x65);
    } else if (sound & 0x10) {
        call_SoundReqID_arg_0(0x66);
    }
    option_game_data_current_operation.timer += 1;
}

void fn_3_2304C(void) {
    destroy_sprite_with_unique_id(0x5E);
    option_game_data_current_operation.tick = NULL;
}

void fn_3_23080(void) {
    if (option_game_data_current_operation.tick != NULL) {
        option_game_data_current_operation.tick();
    }
}

char *lbl_3_data_10124[6] = {
    "Main Game",
    "",
    "",
    "",
    "",
    "\x83\x81\x83\x43\x83\x93\x83\x51\x81\x5B\x83\x80",
};
char *lbl_3_data_10154[6] = {
    "Party Game",
    "",
    "",
    "",
    "",
    "\x83\x7E\x83\x6A\x83\x51\x81\x5B\x83\x80",
};
char *lbl_3_data_101C4[6] = {
    "The Main Game Story Mode \nand Challenge Mode rankings.",
    "",
    "",
    "",
    "",
    "\x83\x81\x83\x43\x83\x93\x83\x51\x81\x5B\x83\x80\x82\xCC\x83\x89\x83\x93\x83\x4C\x83\x93\x83\x4F\x82\xC5\x82\xB7",
};
char *lbl_3_data_10214[6] = {
    "The Party Games rankings.",
    "",
    "",
    "",
    "",
    "\x83\x7E\x83\x6A\x83\x51\x81\x5B\x83\x80\x82\xCC\x83\x89\x83\x93\x83\x4C\x83\x93\x83\x4F\x82\xC5\x82\xB7",
};
struct RankingOption lbl_3_data_1022C[2] = {
    { lbl_3_data_10124, lbl_3_data_101C4 },
    { lbl_3_data_10154, lbl_3_data_10214 },
};
s8 lbl_3_data_1023C[33] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
};

void fn_3_230C4(struct Sprite *sprite) {
    s32 i;
    u32 glow;
    s32 x;
    s32 y;

    if (option_game_data_current_operation.state == 0) {
        GXSetZMode_cached(1U, 7, 1U);
        set_ui_widescreen_scale_mtx(0x140);
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_depth(0.11f);
        set_ui_element_sprite_pos(320.0f, 210.0f);
        set_ui_element_sprite_scale(1.0f, 1.5f);
        draw_ui_box(0x1701);
        for (i = 0; i < sizeof(lbl_3_data_1022C) / sizeof(lbl_3_data_1022C[0]); i++) {
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_flags(0x80000000U);
            textdraw_set_alignment(3);
            if (i == option_game_data_current_operation.cursor) {
                glow = 64.0 * fabs(math_sin(option_game_data_current_operation.timer * 0x180));
                glow = glow | ((glow << 16) | (glow << 8));
                textdraw_set_mul_color(0xFFFF40U);
                textdraw_set_add_color(glow);
            } else {
                textdraw_set_mul_color(0xC0C040U);
                textdraw_set_add_color(0U);
            }
            textdraw_set_scale(15.0f, 1.0f);
            textdraw_set_pos(320.0f, sprite->y + (i << 5));
            textdraw_print("_");
            if (i == option_game_data_current_operation.cursor) {
                init_ui_element_sprite_with_defaults();
                set_ui_element_sprite_pos(sprite->x - 36.0f, 12.0f + (sprite->y + (i << 5)));
                set_ui_element_sprite_scale(26.0 / g_get_bmp_width(0x1700), 26.0 / g_get_bmp_height(0x1700));
                draw_ui_element(0x1700);
            }
            x = sprite->x;
            y = sprite->y + (i << 5);
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_scale(0.8f, 1.0f);
            textdraw_set_flags(0xA0000000U);
            textdraw_set_pos(x, y);
            textdraw_print(lbl_3_data_1022C[i].name[locale]);
        }
        textdraw_reset();
        fn_3_140CC(option_game_data_current_operation.timer);
        textdraw_set_pos(320.0f, 400.0f);
        textdraw_print_and_fit_to_width(390.0f, lbl_3_data_1022C[option_game_data_current_operation.cursor].desc[locale]);
        reset_ui_widescreen_scale_mtx();
        GXSetZMode_cached(1U, 3, 1U);
    }
}

void fn_3_2345C(void) {
    struct Sprite *sprite;

    memset(&option_game_data_current_operation, 0, sizeof(option_game_data_current_operation));
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->x = 250.0f;
        sprite->y = 180.0f;
        sprite->tag = 0x5B;
        sprite->drawFunc = fn_3_230C4;
        strcpy(sprite->text, "ranking");
    }
}

void fn_3_234DC(void) {
    s32 temp_r31;
    s32 temp_r31_2;
    s32 var_r29;
    s32 var_r30;
    s32 var_r31;
    s32 var_r31_2;
    s32 var_r31_3;
    s32 var_r31_4;
    s32 var_r31_5;

    var_r30 = 0;
    switch ((s32) option_game_data_current_operation.state) { /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        var_r31 = option_game_data_current_operation.cursor;
        if (did_any_pad_press_input(2) != 0) {
            var_r31 -= 1;
            if (var_r31 < 0) {
                var_r31 = 1;
            }
        } else if (did_any_pad_press_input(3) != 0) {
            var_r31 += 1;
            if ((u32) var_r31 >= 2U) {
                var_r31 = 0;
            }
        }
        if (var_r31 != (s32) option_game_data_current_operation.cursor) {
            var_r30 = 2;
            option_game_data_current_operation.cursor = var_r31;
        }
        if ((did_any_pad_press_input(6) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            fn_3_14BAC();
            switch ((s32) option_game_data_current_operation.cursor) { /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                var_r30 |= 1;
                option_game_data_current_operation.state = 1;
                break;
            case 1:                                 /* switch 2 */
                var_r30 |= 1;
                option_game_data_current_operation.state = 2;
                break;
            }
        } else if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            g_b_button_pressed_on_menu(1);
        }
        break;
    case 1:                                         /* switch 1 */
        switch ((s32) option_game_data_current_operation.subState) { /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            temp_r31 = OSSetCurrentHeap(stage_heap);
            load_bmp_by_id(3);
            OSSetCurrentHeap(temp_r31);
            option_game_data_current_operation.unk18 = 0;
            option_game_data_current_operation.subCursor = 0;
            fn_1_D0EC4();
            fn_1_13F6B8();
            fn_1_F844C();
            option_game_data_current_operation.subState += 1;
            break;
        case 1:                                     /* switch 3 */
            if ((fn_1_D1344() != 0) && ((s32) option_game_data_current_operation.unk18 <= 0)) {
                var_r31_2 = option_game_data_current_operation.subCursor;
                if ((did_any_pad_press_input(0) != 0) || ((merged_analog_inputs[4] & 1) != 0) || ((merged_digital_inputs[4] & 0x40) != 0)) {
                    var_r31_2 -= 1;
                    if (var_r31_2 < 0) {
                        var_r31_2 = 0;
                    }
                }
                if ((did_any_pad_press_input(1) != 0) || ((merged_analog_inputs[4] & 2) != 0) || ((merged_digital_inputs[4] & 0x20) != 0)) {
                    if ((s32) (g_some_gift_menu_flags & 1) != 0) {
                        var_r31_2 += 1;
                        if (var_r31_2 >= 5) {
                            var_r31_2 = 4;
                        }
                    } else {
                        var_r31_2 += 1;
                        if (var_r31_2 >= 4) {
                            var_r31_2 = 3;
                        }
                    }
                }
                if (var_r31_2 != (s32) option_game_data_current_operation.subCursor) {
                    destroy_sprite_with_unique_id(0x65);
                    if (var_r31_2 == 0) {
                        var_r29 = 0;
loop_53:
                        if (var_r29 < 6) {
                            destroy_sprite_with_unique_id(var_r29 + 0x16);
                            destroy_sprite_with_unique_id(var_r29 + 0x34);
                            var_r29 += 1;
                            goto loop_53;
                        }
                        fn_1_13F6B8();
                    } else {
                        g_something_with_name_entry_get_course(var_r31_2 - 1);
                    }
                    if (var_r31_2 != 3) {
                        if (var_r31_2 < 3) {
                            if (var_r31_2 != 0) {
                                if (var_r31_2 < 0) {
                                    goto block_63;
                                }
                                fn_1_F844C();
                                fn_1_F83A4();
                            } else {
                                goto block_63;
                            }
                        } else if (var_r31_2 < 5) {
                            fn_1_F83A4();
                        } else {
block_63:
                            fn_1_F844C();
                        }
                    } else {
                        if ((s32) (g_some_gift_menu_flags & 1) != 0) {
                            fn_1_F844C();
                        }
                        fn_1_F83A4();
                    }
                    option_game_data_current_operation.subCursor = var_r31_2;
                }
            }
            if ((s32) option_game_data_current_operation.unk18 > 0) {
                option_game_data_current_operation.unk18 -= 1;
            }
            if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
                destroy_sprite_with_unique_id(0x65);
                if ((s32) option_game_data_current_operation.subCursor != 0) {
                    var_r31_3 = 0;
loop_78:
                    if (var_r31_3 < 6) {
                        destroy_sprite_with_unique_id(var_r31_3 + 0x16);
                        destroy_sprite_with_unique_id(var_r31_3 + 0x34);
                        var_r31_3 += 1;
                        goto loop_78;
                    }
                }
                fn_3_14B40();
                option_game_data_current_operation.subState += 1;
            }
            break;
        case 2:                                     /* switch 3 */
            option_game_data_current_operation.state = 0;
            option_game_data_current_operation.subState = 0;
            break;
        }
        break;
    case 2:                                         /* switch 1 */
        switch ((s32) option_game_data_current_operation.subState) { /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            option_game_data_current_operation.subCursor = 0;
            temp_r31_2 = OSSetCurrentHeap(stage_heap);
            load_bmp_by_id(3);
            OSSetCurrentHeap(temp_r31_2);
            fn_1_11DE54(lbl_3_data_1023C[option_game_data_current_operation.subCursor], 0);
            fn_1_F83A4();
            fn_1_F844C();
            option_game_data_current_operation.subState += 1;
            break;
        case 1:                                     /* switch 4 */
            var_r31_4 = option_game_data_current_operation.subCursor;
            if ((did_any_pad_press_input(0) != 0) || ((merged_analog_inputs[4] & 1) != 0)) {
                var_r31_4 -= 1;
                if (var_r31_4 < 0) {
                    var_r31_4 = 0x20;
                }
                if (var_r31_4 == 0xB) {
                    var_r31_4 = 5;
                }
            }
            if ((did_any_pad_press_input(1) != 0) || ((merged_analog_inputs[4] & 2) != 0)) {
                var_r31_4 += 1;
                if ((u32) var_r31_4 >= 0x21U) {
                    var_r31_4 = 0;
                }
                if (var_r31_4 == 6) {
                    var_r31_4 = 0xC;
                }
            }
            if ((merged_digital_inputs[4] & 0x40) != 0) {
                if (var_r31_4 != 0x17) {
                    if (var_r31_4 < 0x17) {
                        if (var_r31_4 < 0xC) {
                            if (var_r31_4 < 6) {
                                if (var_r31_4 < 0) {

                                } else {
                                    var_r31_4 = 0x1E;
                                }
                            }
                        } else if (var_r31_4 < 0x10) {
                            var_r31_4 = 0;
                        } else {
                            var_r31_4 = 0xC;
                        }
                    } else if (var_r31_4 < 0x1E) {
                        if (var_r31_4 < 0x1B) {
                            var_r31_4 = 0x17;
                        } else {
                            var_r31_4 = 0x18;
                        }
                    } else if (var_r31_4 < 0x21) {
                        var_r31_4 = 0x1B;
                    }
                } else {
                    var_r31_4 = 0x10;
                }
            }
            if ((merged_digital_inputs[4] & 0x20) != 0) {
                if (var_r31_4 != 0x17) {
                    if (var_r31_4 < 0x17) {
                        if (var_r31_4 < 0xC) {
                            if (var_r31_4 < 6) {
                                if (var_r31_4 < 0) {

                                } else {
                                    var_r31_4 = 0xC;
                                }
                            }
                        } else if (var_r31_4 < 0x10) {
                            var_r31_4 = 0x10;
                        } else {
                            var_r31_4 = 0x17;
                        }
                    } else if (var_r31_4 < 0x1E) {
                        if (var_r31_4 < 0x1B) {
                            var_r31_4 = 0x1B;
                        } else {
                            var_r31_4 = 0x1E;
                        }
                    } else if (var_r31_4 < 0x21) {
                        var_r31_4 = 0;
                    }
                } else {
                    var_r31_4 = 0x18;
                }
            }
            if (var_r31_4 != (s32) option_game_data_current_operation.subCursor) {
                destroy_sprite_with_unique_id(0x65);
                fn_1_11DE54(lbl_3_data_1023C[var_r31_4], 0);
                fn_1_F83A4();
                fn_1_F844C();
            }
            option_game_data_current_operation.subCursor = var_r31_4;
            if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
                destroy_sprite_with_unique_id(0x65);
                var_r31_5 = 0;
loop_149:
                if (var_r31_5 < 6) {
                    destroy_sprite_with_unique_id(var_r31_5 + 0x16);
                    destroy_sprite_with_unique_id(var_r31_5 + 0x34);
                    var_r31_5 += 1;
                    goto loop_149;
                }
                fn_3_14B40();
                option_game_data_current_operation.subState += 1;
            }
            break;
        case 2:                                     /* switch 4 */
            option_game_data_current_operation.state = 0;
            option_game_data_current_operation.subState = 0;
            break;
        }
        break;
    }
    if ((u32) (var_r30 & 1) != 0) {
        call_SoundReqID_arg_0(0x6A);
    } else if ((u32) (var_r30 & 2) != 0) {
        call_SoundReqID_arg_0(0x6C);
    } else if ((u32) (var_r30 & 4) != 0) {
        call_SoundReqID_arg_0(0x6B);
    } else if ((u32) (var_r30 & 8) != 0) {
        call_SoundReqID_arg_0(0x65);
    } else if ((u32) (var_r30 & 0x10) != 0) {
        call_SoundReqID_arg_0(0x66);
    }
    option_game_data_current_operation.timer += 1;
}

void fn_3_23DB8(void) {
    call_free_bmp_by_id(3);
    destroy_sprite_with_unique_id(0x5B);
}

void fn_3_23DE4(void) {
    s32 var_r3;
    struct Sprite *sprite;

    lbl_3_bss_6910 = -1;
    if (are_all_party_games_unlocked() != 0) {
        var_r3 = 0x5A;
    } else {
        var_r3 = 5;
    }
    g_change_music_volume(var_r3, 0x1EU, 0x64);
    memset(&option_game_data_current_operation, 0, sizeof(option_game_data_current_operation));
    option_game_data_current_operation.cursor = OSGetSoundMode();
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->x = 178.0f;
        sprite->y = 198.0f;
        sprite->tag = 0x5C;
        sprite->drawFunc = fn_3_242FC;
        strcpy(sprite->text, "sound");
    }
}

char *lbl_3_data_10294[6] = {
    "Sound Setting",
    "",
    "",
    "",
    "",
    "\x83\x54\x83\x45\x83\x93\x83\x68\x82\xB9\x82\xC1\x82\xC4\x82\xA2",
};
char *lbl_3_data_102C0[6] = {
    "Mono",
    "",
    "",
    "",
    "",
    "\x83\x82\x83\x6D\x83\x89\x83\x8B",
};
char *lbl_3_data_102EC[6] = {
    "Stereo",
    "",
    "",
    "",
    "",
    "\x83\x58\x83\x65\x83\x8C\x83\x49",
};
char *lbl_3_data_10340[6] = {
    "This is the speaker setting.",
    "",
    "",
    "",
    "",
    "\x83\x58\x83\x73\x81\x5B\x83\x4A\x81\x5B\x82\xCC\x82\xB9\x82\xC1\x82\xC4\x82\xA2\x82\xC5\x82\xB7\x81\x42",
};
char **lbl_3_data_10358[2] = { lbl_3_data_102C0, lbl_3_data_102EC };

void fn_3_23EA8(void) {
    s32 sounds[4] = { 0x12D, 0x130, 0x12F, 0x12E };
    s32 temp_r4;
    s32 var_r28;
    s32 var_r30;
    s32 var_r31;
    s8 temp_r28;
    s8 temp_r29;

    var_r31 = 0;
    switch ((s32) option_game_data_current_operation.state) { /* irregular */
    case 0:
        if ((did_any_pad_press_input(6) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            var_r31 = 8;
            option_game_data_current_operation.savedValue = option_game_data_current_operation.cursor;
            option_game_data_current_operation.state = 1;
        } else if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            g_b_button_pressed_on_menu(1);
        }
        break;
    case 1:
        var_r28 = option_game_data_current_operation.cursor;
        if ((did_any_pad_press_input(0) != 0) || (did_any_pad_press_input(1) != 0)) {
            var_r28 ^= 1;
        }
        if (var_r28 != (s32) option_game_data_current_operation.cursor) {
            var_r31 = 2;
            option_game_data_current_operation.cursor = var_r28;
            fn_1_367E4(option_game_data_current_operation.cursor);
        }
        if ((did_any_pad_press_input(6) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            var_r31 |= 8;
            option_game_data_current_operation.state = 0;
        } else if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            var_r31 |= 0x10;
            option_game_data_current_operation.cursor = option_game_data_current_operation.savedValue;
            option_game_data_current_operation.state = 0;
        }
        break;
    }
    if ((u32) (g_font_char_var_3 % 60) == 0) {
        temp_r29 = (s8) (63.0f * math_sin((s16) (g_font_char_var_3 << 9)));
        temp_r28 = (s8) (63.0f * math_sin((s16) ((g_font_char_var_3 << 9) + 0x8000)));
        temp_r4 = (s32) (rand() & 0x7FFF) >> 0xD;
        var_r30 = sounds[temp_r4];
        if (var_r30 == (s32) lbl_3_bss_6910) {
            var_r30 = sounds[temp_r4 & 4]; /* sic: always index 0 */
        }
        g_smth_with_sound = 0;
        g_player_id_for_sound = 0;
        call_SoundReqID_arg_0(((temp_r28 & 0x7F) << 0x19) | (var_r30 | 0x10000 | 0xF800 | ((temp_r29 & 0x7F) << 0x12)));
        lbl_3_bss_6910 = var_r30;
    }
    if ((u32) (var_r31 & 1) != 0) {
        call_SoundReqID_arg_0(0x6A);
    } else if ((u32) (var_r31 & 2) != 0) {
        call_SoundReqID_arg_0(0x6C);
    } else if ((u32) (var_r31 & 4) != 0) {
        call_SoundReqID_arg_0(0x6B);
    } else if ((u32) (var_r31 & 8) != 0) {
        call_SoundReqID_arg_0(0x65);
    } else if ((u32) (var_r31 & 0x10) != 0) {
        call_SoundReqID_arg_0(0x66);
    }
    option_game_data_current_operation.timer += 1;
}

void fn_3_242B4(void) {
    s32 var_r3;

    if (are_all_party_games_unlocked() != 0) {
        var_r3 = 0x5A;
    } else {
        var_r3 = 5;
    }
    g_change_music_volume(var_r3, 0x1EU, 0x3C);
    destroy_sprite_with_unique_id(0x5C);
}

void fn_3_242FC(struct Sprite *sprite) {
    f32 x;
    f32 y;
    u32 glow;

    GXSetZMode_cached(1U, 7, 1U);
    set_ui_widescreen_scale_mtx(0x140);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(0.11f);
    set_ui_element_sprite_pos(320.0f, 210.0f);
    set_ui_element_sprite_scale(1.0f, 1.2f);
    draw_ui_box(0x1701);
    x = sprite->x;
    y = sprite->y;
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000U);
    textdraw_set_alignment(3);
    glow = 64.0 * fabs(math_sin(option_game_data_current_operation.timer * 0x180));
    glow = glow | ((glow << 16) | (glow << 8));
    textdraw_set_mul_color(0xFFFF40U);
    textdraw_set_add_color(glow);
    textdraw_set_scale(15.0f, 1.0f);
    textdraw_set_pos(320.0f, y);
    textdraw_print("_");
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(x - 30.0, 12.0 + y);
    set_ui_element_sprite_scale(26.0 / g_get_bmp_width(0x1700), 26.0 / g_get_bmp_height(0x1700));
    draw_ui_element(0x1700);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_scale(0.8f, 1.0f);
    textdraw_set_flags(0x80000000U);
    textdraw_set_border();
    textdraw_set_pos(x, y);
    textdraw_printf("%s", lbl_3_data_10294[locale]);
    textdraw_set_alignment(3);
    textdraw_set_pos(276.0 + x, y);
    switch (option_game_data_current_operation.state) {
    case 0:
        textdraw_print(lbl_3_data_10358[option_game_data_current_operation.cursor][locale]);
        break;
    case 1:
        textdraw_printf("<%s>", lbl_3_data_10358[option_game_data_current_operation.cursor][locale]);
        break;
    }
    fn_3_140CC(option_game_data_current_operation.timer);
    textdraw_set_pos(320.0f, 400.0f);
    textdraw_print_and_fit_to_width(390.0f, lbl_3_data_10340[locale]);
    reset_ui_widescreen_scale_mtx();
    GXSetZMode_cached(1U, 3, 1U);
}

void fn_3_2462C(void) {
    s32 temp_r3;

    memset(&option_game_data_current_operation, 0, sizeof(option_game_data_current_operation));
    memset(&s_replayListPos, 0, sizeof(s_replayListPos));
    s_replayListPos.x = 992.0f;
    if ((u32) (replayList->flags & 1) != 0) {
        temp_r3 = fn_1_EFE64(3);
        if (temp_r3 < 1) {
            if (temp_r3 < -1) {
                goto block_4;
            }
            goto block_5;
        }
block_4:
        replayList->flags &= ~3;
        return;
    }
block_5:
    lbl_3_bss_6920 = 0;
    option_game_data_current_operation.state = 0;
}

char *lbl_3_data_10390[6] = {
    "Delete",
    "L\xF6\x73" "chen",
    "Effacer",
    "Borrar",
    "Elimina",
    "\x82\xAF\x82\xB7",
};
char *lbl_3_data_10404[6] = {
    "Are you sure?",
    "Sicher?",
    "Tu es s\xFB\x72 ?",
    "\xBF" "Est\xE1\x73 seguro?",
    "Sei sicuro?",
    "\x82\xAF\x82\xB5\x82\xC4\x82\xE0\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48",
};

void fn_3_246E8(void) {
    struct MemcardRequest req;
    struct OptionReplayFile *file;
    struct OptionReplayEntry *entry;
    f32 y;
    f32 diff;
    f32 targetY;
    s32 count;
    s32 i;
    s32 visible;
    u8 mode;

    switch (option_game_data_current_operation.state) {
    case 0:
        option_game_data_current_operation.state = 1;
        memset(s_replayFiles, 0, sizeof(s_replayFiles));
        memset(&req, 0, sizeof(req));
        req.file = 2;
        req.buffer = s_replayFiles;
        if ((replayList->flags & 1) == 0) {
            replayList->cursor = 0;
            mode = 4;
        } else {
            mode = 3;
            req.flags |= 0x20000000;
        }
        g_some_replay_func4(2, mode, &req);
        /* fallthrough */
    case 1:
        if (g_check_some_memcard_field() == 0) {
            if (g_get_some_memcard_var() == 0) {
                OSReport("result_code: %d ", g_get_result_code());
                g_b_button_pressed_on_menu(1);
            } else {
                option_game_data_current_operation.state = 2;
                count = 0;
                file = s_replayFiles;
                entry = lbl_3_bss_7D40;
                for (i = 0x80; i > 0; i--, file++) {
                    if (file->size != 0) {
                        fn_3_25620(file, entry);
                        entry++;
                        count++;
                    }
                }
                lbl_3_bss_8B40 = count;
                if (count == 0) {
                    option_game_data_current_operation.state = -1;
                    g_b_button_pressed_on_menu(1);
                } else {
                    if ((replayList->flags & 1) != 0 && replayList->count != count) {
                        replayList->cursor = 0;
                    }
                    replayList->flags |= 1;
                    replayList->count = count;
                    fn_3_25858(lbl_3_bss_7D40, count);
                }
            }
        }
        break;
    case 2:
        option_game_data_current_operation.state = 3;
        fn_3_24AF8();
        if ((replayList->flags & 2) != 0 && selReplayFile->unk24 == 0 && selReplayFile->unk28 == -6) {
            option_game_data_current_operation.state = 4;
            break;
        }
        /* fallthrough */
    case 3:
        fn_3_24D04();
        break;
    case 4:
        option_game_data_current_operation.state = 5;
        fn_3_25290();
        /* fallthrough */
    case 5:
        fn_3_252A4();
        break;
    case 6:
        fn_3_254B0();
        break;
    }

    y = 122.0 + 30.0 * option_game_data_current_operation.scrollAnim * 0.25
        + 30.0 * (lbl_3_bss_7D40[replayList->cursor].unk10 + (replayList->cursor - option_game_data_current_operation.scrollTop));
    visible = 0;
    if (option_game_data_current_operation.state < 7 && option_game_data_current_operation.state >= 4) {
        visible = 1;
    }
    if (visible) {
        diff = y - 240.0f;
        if (fabs(diff) > 80.0) {
            targetY = 240.0f;
        } else if (diff < 0.0f) {
            targetY = 84.0f + y;
        } else {
            targetY = y - 80.0f;
        }
        if (s_replayListPos.x > 960.0f) {
            s_replayListPos.y = targetY;
        } else {
            s_replayListPos.y += 0.18f * (targetY - s_replayListPos.y);
        }
        s_replayListPos.x += 0.15f * (320.0f - s_replayListPos.x);
        s_replayListPos.alpha += 0.2f * (1.0f - s_replayListPos.alpha);
        return;
    }
    s_replayListPos.x += 0.1f * (992.0f - s_replayListPos.x);
    s_replayListPos.alpha += 0.1f * -s_replayListPos.alpha;
}

void fn_3_24AF8(void) {
    s32 temp_r27;
    struct Sprite *sprite;
    struct Sprite *sprite2;
    struct Sprite *temp_r3_3;

    temp_r27 = replayList->cursor;
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->tag = 0x59;
        sprite->x = 320.0f;
        sprite->y = 224.0f;
        sprite->depth = 0.0f;
        sprite->scaleX = 1.45f;
        sprite->scaleY = 1.72f;
        sprite->drawFunc = fn_3_25ECC;
        strcpy(sprite->text, "replay");
    }
    lbl_3_bss_6920 = 1;
    g_replay_stage_id_to_load = lbl_3_bss_7D40[replayList->cursor].unkC;
    g_load_preview_texture(&lbl_3_bss_8B48, "preview/replist400x112.tpl", g_replay_stage_id_to_load, 0x190, 0x70, 5);
    sprite2 = create_sprite();
    if (sprite2 != NULL) {
        sprite2->tag = 0x59;
        sprite2->tex = (struct SpriteTexture *) (&lbl_3_bss_8B48);
        sprite2->type = 2;
        sprite2->x = 320.0f;
        sprite2->y = 399.0f;
        sprite2->textAlign = 4;
        sprite2->depth = 0.07f;
        sprite2->flags |= 0x01000000;
        sprite2->widescreenArg = 0x140;
    }
    temp_r3_3 = create_sprite();
    if (temp_r3_3 != NULL) {
        temp_r3_3->tag = 0x59;
        temp_r3_3->fontId = 3;
        temp_r3_3->flags = 0x40000000;
        temp_r3_3->spacingX = -5;
        temp_r3_3->x = 512.0f;
        temp_r3_3->y = 453.0f;
        temp_r3_3->shadowAlpha = 0.8f;
        temp_r3_3->shadowX = 2;
        temp_r3_3->shadowY = 2;
        temp_r3_3->scaleX = 0.7f;
        temp_r3_3->scaleY = 1.0f;
        temp_r3_3->mulR = 0xFF;
        temp_r3_3->mulG = 0xFF;
        temp_r3_3->mulB = 0;
        temp_r3_3->textAlign = 8;
        temp_r3_3->depth = 0.02f;
        temp_r3_3->mainFunc = fn_3_25D70;
        temp_r3_3->flags |= 0x01000000;
        temp_r3_3->widescreenArg = 0x140;
        strcpy(temp_r3_3->text, "-----");
    }
    if (temp_r27 > 3) {
        option_game_data_current_operation.scrollTop = temp_r27 - 3;
    }
}

void fn_3_24D04(void) {
    s32 temp_r3;
    s32 var_r27;
    s32 var_r28;
    s32 var_r29;
    s32 var_r3;
    s32 var_r4;
    struct OptionReplayFile *temp_r30;
    struct OptionReplayEntry *var_r30;

    var_r27 = 0;
    var_r28 = lbl_3_bss_8B40;
    var_r29 = replayList->cursor;
    if (did_any_pad_press_input(2) != 0) {
        var_r29 -= 1;
        if (var_r29 < 0) {
            var_r29 = 0;
        }
    } else if (did_any_pad_press_input(3) != 0) {
        var_r29 += 1;
        if (var_r29 >= var_r28) {
            var_r29 = var_r28 - 1;
        }
    }
    if (var_r29 != (s32) replayList->cursor) {
        var_r27 = 2;
        replayList->cursor = var_r29;
    }
    var_r3 = option_game_data_current_operation.scrollTop;
    if (var_r3 < (s32) (var_r29 - 6)) {
        var_r3 = var_r29 - 6;
        option_game_data_current_operation.scrollAnim = 4;
    } else if (var_r3 > var_r29) {
        var_r3 = var_r29;
        option_game_data_current_operation.scrollAnim = -4;
    } else if ((merged_analog_inputs[4] & 0x80) != 0) {
        var_r3 -= 1;
        if (var_r3 < (s32) (var_r29 - 6)) {
            var_r3 = var_r29 - 6;
        } else if (var_r3 < 0) {
            var_r3 = 0;
        } else {
            var_r27 |= 8;
            option_game_data_current_operation.scrollAnim = -4;
        }
    } else if ((merged_analog_inputs[4] & 0x40) != 0) {
        var_r3 += 1;
        if (var_r3 > var_r29) {
            var_r3 = var_r29;
        } else if (var_r3 > (s32) (var_r28 - 1)) {
            var_r3 = var_r28 - 1;
        } else {
            var_r27 |= 8;
            option_game_data_current_operation.scrollAnim = 4;
        }
    }
    option_game_data_current_operation.scrollTop = var_r3;
    temp_r3 = fn_1_EFE64(3);
    if (temp_r3 < 1) {
        if (temp_r3 < -1) {
            goto block_27;
        }
        if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            replayList->flags = (s32) (replayList->flags & 0xFFFFFFFC);
            g_b_button_pressed_on_menu(1);
        } else if ((did_any_pad_press_input(6) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            main_mode_request = 7;
            sub_mode_request = 0x101;
            replayList->flags = (s32) (replayList->flags | 2);
            temp_r30 = lbl_3_bss_7D40[var_r29].unk18;
            strcpy(selReplayFile->name, temp_r30->name);
            selReplayFile->size = temp_r30->size;
            var_r27 |= 1;
        } else if (did_any_pad_press_input(8) != 0) {
            option_game_data_current_operation.state = 4;
            var_r27 |= 1;
        }
        var_r4 = option_game_data_current_operation.scrollAnim;
        if (var_r4 > 0) {
            var_r4 -= 1;
        } else if (var_r4 < 0) {
            var_r4 += 1;
        }
        option_game_data_current_operation.scrollAnim = var_r4;
        var_r30 = lbl_3_bss_7D40;
loop_47:
        if (var_r28 > 0) {
            var_r30->unk14 += -0.008f;
            var_r30->unk14 *= 0.99f;
            var_r30->unk10 += var_r30->unk14;
            if (var_r30->unk10 < 0.0f) {
                var_r30->unk10 = 0.0f;
                if (var_r30->unk14 < 0.0f) {
                    var_r30->unk14 *= -(0.4f + (0.4f * ((f32) rand() / 32767.0f)));
                }
            }
            var_r28 -= 1;
            var_r30 += 0x1C;
            goto loop_47;
        }
        if ((s32) g_replay_stage_id_to_load != (s16) lbl_3_bss_7D40[var_r29].unkC) {
            g_replay_stage_id_to_load = (s32) lbl_3_bss_7D40[var_r29].unkC;
            fn_1_F9CAC(&lbl_3_bss_8B48, g_replay_stage_id_to_load);
        }
        g_load_sprite_tex_from_dvd(&lbl_3_bss_8B48);
        option_game_data_current_operation.timer += 1;
        if ((u32) (var_r27 & 1) != 0) {
            call_SoundReqID_arg_0(0x6A);
            return;
        }
        if ((u32) (var_r27 & 2) != 0) {
            call_SoundReqID_arg_0(0x6C);
            return;
        }
        if ((u32) (var_r27 & 4) != 0) {
            call_SoundReqID_arg_0(0x6B);
            return;
        }
        if ((u32) (var_r27 & 8) != 0) {
            call_SoundReqID_arg_0(0x65);
            return;
        }
        if ((u32) (var_r27 & 0x10) != 0) {
            call_SoundReqID_arg_0(0x66);
        }
        return;
    }
block_27:
    option_game_data_current_operation.state = 0;
    fn_3_251E8();
}

void fn_3_251E8(void) {
    destroy_sprite_with_unique_id(0x59);
    if ((s32) lbl_3_bss_6920 != 0) {
        fn_1_F9E1C(&lbl_3_bss_8B48);
        lbl_3_bss_6920 = 0;
    }
}

void fn_3_2523C(void) {
    destroy_sprite_with_unique_id(0x59);
    if ((s32) lbl_3_bss_6920 != 0) {
        fn_1_F9E1C(&lbl_3_bss_8B48);
        lbl_3_bss_6920 = 0;
    }
}

void fn_3_25290(void) {
    option_game_data_current_operation.subCursor = 1;
}

void fn_3_252A4(void) {
    struct MemcardRequest req;
    s32 temp_r28;
    s32 temp_r28_2;
    s32 var_r29;
    s32 var_r30;

    var_r29 = option_game_data_current_operation.subCursor;
    temp_r28 = var_r29;
    var_r30 = 0;
    if ((did_any_pad_press_input(0) != 0) || (did_any_pad_press_input(1) != 0)) {
        var_r29 ^= 1;
    }
    if (var_r29 != temp_r28) {
        var_r30 = 2;
        option_game_data_current_operation.subCursor = var_r29;
    }
    if ((did_any_pad_press_input(6) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
        if (var_r29 != 0) {
            option_game_data_current_operation.state = 3;
            var_r30 |= 4;
        } else {
            option_game_data_current_operation.state = 6;
            temp_r28_2 = replayList->cursor;
            memset(&req, 0, sizeof(req));
            req.file = 2;
            req.name = lbl_3_bss_7D40[temp_r28_2].unk18->name;
            g_some_replay_func4(3, 3, &req);
        }
        var_r30 |= 1;
    } else if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
        option_game_data_current_operation.state = 3;
        var_r30 |= 4;
    }
    if ((u32) (var_r30 & 1) != 0) {
        call_SoundReqID_arg_0(0x6A);
        return;
    }
    if ((u32) (var_r30 & 2) != 0) {
        call_SoundReqID_arg_0(0x6C);
        return;
    }
    if ((u32) (var_r30 & 4) != 0) {
        call_SoundReqID_arg_0(0x6B);
        return;
    }
    if ((u32) (var_r30 & 8) != 0) {
        call_SoundReqID_arg_0(0x65);
        return;
    }
    if ((u32) (var_r30 & 0x10) != 0) {
        call_SoundReqID_arg_0(0x66);
    }
}

void fn_3_254B0(void) {
    struct OptionReplayEntry *entry;
    s32 count;
    s32 n;
    s32 cursor;

    if (g_check_some_memcard_field() == 0) {
        if (g_get_some_memcard_var() == 0) {
            OSReport("result_code: %d ", g_get_result_code());
            option_game_data_current_operation.state = 0;
            return;
        }
        /* remove the deleted entry from the list */
        cursor = replayList->cursor;
        entry = &lbl_3_bss_7D40[cursor];
        for (n = lbl_3_bss_8B40 - cursor - 1; n > 0; n--, entry++) {
            entry[0] = entry[1];
            entry->unk10 += 1.0f;
        }
        count = lbl_3_bss_8B40 - 1;
        if (cursor >= count) {
            cursor = count - 1;
        }
        replayList->cursor = cursor;
        lbl_3_bss_8B40 = count;
        if (count != 0) {
            option_game_data_current_operation.state = 3;
            return;
        }
        replayList->flags &= ~3;
        option_game_data_current_operation.state = 0;
        destroy_sprite_with_unique_id(0x59);
        if (lbl_3_bss_6920 != 0) {
            fn_1_F9E1C(&lbl_3_bss_8B48);
            lbl_3_bss_6920 = 0;
        }
    }
}

void fn_3_25620(struct OptionReplayFile *file, struct OptionReplayEntry *entry) {
    struct OptionReplayFileId id;
    struct OptionCalendarTime cal;
    u8 *src;
    u8 *dst;
    s32 i;
    s32 v;
    u8 c;

    src = (u8 *)file->name;
    entry->unk18 = file;
    for (i = 12, dst = (u8 *)&id; i > 0; i--, dst++) {
        c = *src;
        if (c >= '0' && c <= '9') {
            v = c - '0';
        } else {
            v = c - 0x37;
        }
        v <<= 4;
        c = src[1];
        if (c >= '0' && c <= '9') {
            v += c - '0';
        } else {
            v += c - 0x37;
        }
        src += 2;
        *dst = v;
    }
    entry->unk1 = id.unk1;
    entry->unk2 = id.unk2;
    if (id.flags & 1) {
        entry->unk0 = 8;
        entry->unk1++;
        entry->unk2++;
    } else if (id.flags & 8) {
        entry->unk0 = 7;
    } else if (id.flags & 4) {
        entry->unk0 = 3;
    } else {
        switch (id.unk1) {
        case 0:
            entry->unk0 = (id.flags & 2) ? 4 : 0;
            break;
        case 1:
            entry->unk0 = (id.flags & 2) ? 5 : 1;
            break;
        case 2:
            entry->unk0 = (id.flags & 2) ? 6 : 2;
            break;
        }
    }
    entry->unkA = id.chara;
    entry->unkC = id.stageId;
    OSTicksToCalendarTime((u64)id.time * (*(u32 *)0x800000F8 / 4), &cal);
    entry->unk4 = cal.year;
    entry->unk6 = cal.mon + 1;
    entry->unk7 = cal.mday;
    entry->unk8 = cal.hour;
    entry->unk9 = cal.min;
}

void fn_3_25858(struct OptionReplayEntry *entries, s32 count) {
    struct OptionReplayEntry tmp;
    s32 half;
    s32 i;
    s32 j;

    if (count == 1) {
        return;
    }
    half = count >> 1;
    fn_3_25858(entries, half);
    fn_3_25858(entries + half, count - half);
    for (i = half; i < count; i++) {
        tmp = entries[i];
        for (j = i - 1; j >= 0; j--) {
            if (fn_3_25CFC(&entries[j], &tmp) <= 0) {
                break;
            }
            entries[j + 1] = entries[j];
        }
        entries[j + 1] = tmp;
    }
}

s32 fn_3_25CFC(struct OptionReplayEntry *a, struct OptionReplayEntry *b) {
    s32 d;

    d = a->unk0 - b->unk0;
    if (d == 0) {
        d = (a->unk1 * 16 + a->unk2) - (b->unk1 * 16 + b->unk2);
        if (d == 0) {
            d = a->unk18->unk24 - b->unk18->unk24;
        }
    }
    return d;
}

void fn_3_25D70(s8 *unused, struct Sprite *sprite) {
    u32 blocks;

    if ((u32)lbl_3_bss_8B40 == 0) {
        sprintf(sprite->text, "-----");
        sprite->x = 512.0f;
        return;
    }
    /* replay size in 8 KiB memory card blocks */
    blocks = lbl_3_bss_7D40[replayList->cursor].unk18->size >> 13;
    if ((s32)blocks == 1) {
        sprintf(sprite->text, "1 %s", lbl_3_data_1006C[locale]);
    } else {
        sprintf(sprite->text, "%2d %s", blocks, lbl_3_data_100AC[locale]);
    }
    sprite->x = 512.0f - ((f32)(s8)sprite->spacingX * (f32)strlen(sprite->text));
}

s16 lbl_3_data_10474[2] = { 0x48, 0x19 };
s16 lbl_3_data_10478[2] = { 0x20, 0x31 };
s16 lbl_3_data_1047C[2] = { 0x33, 0x41 };
s16 lbl_3_data_10480[2] = { 0x5D, 0x03 };
s16 *lbl_3_data_10484[4] = { lbl_3_data_10474, lbl_3_data_10478, lbl_3_data_1047C, lbl_3_data_10480 };

void fn_3_25ECC(struct Sprite *arg0) {
    u32 sp70;
    u32 sp6C;
    s32 sp68;
    f32 sp20;
    f32 sp1C;
    f32 sp18;
    u8 sp16;
    u8 sp15;
    u8 sp14;
    f32 sp10;
    f32 spC;
    u8 sp9;
    u8 sp8;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f0_4;
    f32 temp_f27;
    f32 temp_f28;
    f32 temp_f28_2;
    f32 temp_f28_3;
    f32 temp_f28_4;
    f32 temp_f29;
    f32 temp_f30;
    f32 temp_f30_2;
    f32 temp_f30_3;
    f32 temp_f30_4;
    f32 temp_f30_5;
    f32 temp_f31;
    f32 temp_f31_2;
    f32 temp_f31_3;
    f32 var_f30;
    f32 var_f30_2;
    f32 var_f31;
    f32 var_f31_2;
    s16 temp_r22;
    s32 temp_r14;
    s32 temp_r17;
    s32 temp_r27;
    s32 temp_r4;
    s32 var_r18;
    s32 var_r26;
    s8 temp_r16_2;
    s8 temp_r18;
    s8 temp_r19;
    s8 temp_r20;
    s8 temp_r20_2;
    s8 temp_r21;
    s8 var_r19;
    u32 temp_r16;
    u32 temp_r3;
    u32 var_r16;
    u8 temp_r0;
    struct OptionReplayEntry *temp_r28;
    char *var_r23;

    temp_f30 = s_replayListPos.alpha;
    temp_f29 = 1.0f - temp_f30;
    set_ui_widescreen_scale_mtx(0x140);
    GXSetZMode_cached(1U, 7, 1U);
    sp18 = (143.0f * temp_f29) + (64.0f * temp_f30);
    sp1C = (143.0f * temp_f29) + (60.0f * temp_f30);
    sp20 = 0.0f;
    option_f32_to_u8(&sp18, &sp14);
    option_f32_to_u8(&sp1C, &sp15);
    option_f32_to_u8(&sp20, &sp16);
    sp70 = sp16 | ((sp14 << 0x10) | (sp15 << 8));
    sp18 = (192.0f * temp_f29) + (96.0f * temp_f30);
    sp1C = (192.0f * temp_f29) + (96.0f * temp_f30);
    sp20 = (64.0f * temp_f29) + (32.0f * temp_f30);
    option_f32_to_u8(&sp18, &sp14);
    option_f32_to_u8(&sp1C, &sp15);
    option_f32_to_u8(&sp20, &sp16);
    sp6C = sp16 | ((sp14 << 0x10) | (sp15 << 8));
    sp18 = (255.0f * temp_f29) + (160.0f * temp_f30);
    sp1C = (255.0f * temp_f29) + (140.0f * temp_f30);
    sp20 = (255.0f * temp_f29) + (140.0f * temp_f30);
    option_f32_to_u8(&sp18, &sp14);
    option_f32_to_u8(&sp1C, &sp15);
    option_f32_to_u8(&sp20, &sp16);
    sp68 = sp16 | ((sp14 << 0x10) | (sp15 << 8));
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(arg0->x, arg0->y);
    set_ui_element_sprite_mult_color_from_u32(sp68);
    set_ui_element_sprite_depth(0.001f);
    set_ui_element_sprite_scale(arg0->scaleX, arg0->scaleY);
    draw_ui_box(0x1701);
    GXSetZMode_cached(1U, 6, 1U);
    temp_f31 = (f32) ((f64) arg0->x - 240.0);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_depth(0.001f);
    textdraw_set_spacing(-5.0f, 0.0f);
    temp_r27 = lbl_3_bss_8B40;
    var_r26 = -1;
loop_33:
    if (var_r26 < 8) {
        temp_r17 = var_r26 + option_game_data_current_operation.scrollTop;
        if ((temp_r17 >= 0) && (temp_r17 < temp_r27)) {
            temp_r28 = &lbl_3_bss_7D40[temp_r17];
            temp_f30_2 = (f32) (((f64) arg0->y - 102.0) + (30.0 * (f64) option_game_data_current_operation.scrollAnim * 0.25) + (30.0 * (f64) ((f32) var_r26 + temp_r28->unk10)));
            textdraw_set_alignment(3);
            if (temp_r17 == (s32) replayList->cursor) {
                temp_r3 = (u32) (64.0 * fabs(math_sin((s16) (option_game_data_current_operation.timer * 0x180))));
                temp_r16 = temp_r3 | ((temp_r3 << 0x10) | (temp_r3 << 8));
                textdraw_set_mul_color(0xFFFF40U);
                textdraw_set_add_color(temp_r16);
            } else {
                textdraw_set_mul_color(sp6C);
                textdraw_set_add_color(0U);
            }
            textdraw_set_scale(25.0f, 1.0f);
            textdraw_set_pos(arg0->x, temp_f30_2);
            textdraw_print("_");
            textdraw_set_mul_color(-1U);
            textdraw_set_add_color(0U);
            textdraw_set_alignment(0);
            if (temp_r17 == (s32) replayList->cursor) {
                init_ui_element_sprite_with_defaults();
                set_ui_element_sprite_pos((f32) (((f64) temp_f31 - 35.0) + (f64) lbl_1_bss_6FB6C), 12.0f + temp_f30_2);
                temp_f28 = (f32) (26.0 / (f64) g_get_bmp_height(0x1700));
                set_ui_element_sprite_scale((f32) (26.0 / (f64) g_get_bmp_width(0x1700)), temp_f28);
                draw_ui_element(0x1700);
            }
            textdraw_set_scale(0.7f, 1.0f);
            textdraw_set_spacing(-4.0f, 0.0f);
            if (temp_r17 == (s32) replayList->cursor) {
                var_r16 = 0xFFFF00;
            } else {
                var_r16 = sp70;
            }
            var_r18 = 0;
            var_r19 = 0;
#ifndef __MWERKS__
            var_r23 = ""; /* the original leaves this unset for a corrupt difficulty byte (> 8) */
#endif
            temp_r0 = temp_r28->unk0;
            if ((u32) (s8) temp_r0 <= 8U) {
                switch ((s8) temp_r0) {
                case 0:
                    var_r23 = "BEGINNER";
                    break;
                case 1:
                    var_r23 = "ADVANCED";
                    break;
                case 2:
                    var_r23 = "EXPERT";
                    break;
                case 3:
                    var_r23 = "MASTER";
                    break;
                case 4:
                    var_r23 = "BEGINNER EXTRA";
                    break;
                case 5:
                    var_r23 = "ADVANCED EXTRA";
                    break;
                case 6:
                    var_r23 = "EXPERT EXTRA";
                    break;
                case 7:
                    var_r23 = "MASTER EXTRA";
                    break;
                case 8:
                    var_r23 = "WORLD";
                    var_r18 = 1;
                    var_r19 = temp_r28->unk1;
                    break;
                }
            }
            temp_r20 = temp_r28->unk2;
            textdraw_set_mul_color(0U);
            textdraw_set_pos((f32) (2.0 + ((f64) temp_f31 - 15.0)), (f32) (2.0 + (f64) temp_f30_2));
            if (var_r18 != 0) {
                textdraw_printf("%s %d-%d", var_r23, var_r19, temp_r20);
            } else {
                textdraw_printf("%s %d", var_r23, temp_r20);
            }
            textdraw_set_mul_color(var_r16);
            textdraw_set_pos((f32) ((f64) temp_f31 - 15.0), temp_f30_2);
            if (var_r18 != 0) {
                textdraw_printf("%s %d-%d", var_r23, var_r19, temp_r20);
            } else {
                textdraw_printf("%s %d", var_r23, temp_r20);
            }
            textdraw_set_alignment(3);
            textdraw_set_spacing(-5.0f, 0.0f);
            temp_r22 = temp_r28->unk4;
            temp_r21 = temp_r28->unk6;
            temp_r20_2 = temp_r28->unk7;
            temp_r19 = temp_r28->unk8;
            temp_r18 = temp_r28->unk9;
            textdraw_set_mul_color(0U);
            textdraw_set_pos((f32) (2.0 + (330.0 + (f64) temp_f31)), (f32) (2.0 + (f64) temp_f30_2));
            textdraw_printf("%2d/%2d/%d %02d:%02d", temp_r21, temp_r20_2, temp_r22, temp_r19, temp_r18);
            textdraw_set_mul_color(var_r16);
            textdraw_set_pos((f32) (330.0 + (f64) temp_f31), temp_f30_2);
            textdraw_printf("%2d/%2d/%d %02d:%02d", temp_r21, temp_r20_2, temp_r22, temp_r19, temp_r18);
            textdraw_set_alignment(0);
            temp_r16_2 = temp_r28->unkA;
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_depth(0.001f);
            set_ui_element_sprite_pos((f32) (470.0 + (f64) temp_f31), (f32) (10.0 + (f64) temp_f30_2));
            if (temp_r17 != (s32) replayList->cursor) {
                set_ui_element_sprite_mult_color_from_u32(sp68);
            }
            set_ui_element_sprite_scale(0.366f, 0.24f);
            set_ui_element_sprite_rot_z((s32) (4096.0f * math_sin((s16) ((option_game_data_current_operation.timer * 0x190) + 0x4000))));
            draw_ui_element(lbl_3_data_10484[temp_r16_2][0]);
            temp_r4 = (option_game_data_current_operation.timer + (var_r26 * 0x1E)) % ((temp_r16_2 * 0xA) + 0x5A);
            if (temp_r4 < 0x10) {
                set_ui_element_sprite_alpha((f32) (1.0 - (fabs((f32) (temp_r4 - 8)) * 0.125)));
                draw_ui_element(lbl_3_data_10484[temp_r16_2][1]);
            }
        }
        var_r26 += 1;
        goto loop_33;
    }
    GXSetZMode_cached(1U, 7, 1U);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(arg0->x, arg0->y);
    set_ui_element_sprite_depth(0.001f);
    set_ui_element_sprite_scale(arg0->scaleX, arg0->scaleY);
    draw_ui_box_only_borders(0x1701);
    temp_f28_2 = (f32) g_get_bmp_width(0x1702);
    temp_f30_3 = (f32) g_get_bmp_height(0x1702);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(320.0f, 96.0f);
    set_ui_element_sprite_scale((f32) ((188.0 + (f64) temp_f28_2) / (f64) temp_f28_2), (f32) (((f64) temp_f30_3 - 21.0) / (f64) temp_f30_3));
    draw_ui_box(0x1702);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_alignment(4);
    textdraw_set_flags(0x80000000U);
    textdraw_set_scale(0.7f, 0.7f);
    textdraw_set_pos(174.0f, 96.0f);
    textdraw_print("STAGE");
    textdraw_set_pos(382.0f, 96.0f);
    textdraw_print("DATE");
    textdraw_set_pos(475.0f, 96.0f);
    textdraw_print("TIME");
    textdraw_set_pos(550.0f, 96.0f);
    textdraw_print("MONKEY");
    var_f30 = (f32) ((f64) option_game_data_current_operation.scrollTop - ((f64) option_game_data_current_operation.scrollAnim * 0.25)) / (f32) temp_r27;
    var_f31 = (f32) (7.0 + ((f64) option_game_data_current_operation.scrollTop - ((f64) option_game_data_current_operation.scrollAnim * 0.25))) / (f32) temp_r27;
    var_f30_2 = (var_f30 < 0.0) ? 0.0 : (var_f30 > 1.0) ? 1.0 : var_f30;
    var_f31_2 = (var_f31 < 0.0) ? 0.0 : (var_f31 > 1.0) ? 1.0 : var_f31;
    if ((f32) (0.05 + (f64) var_f30_2) > var_f31_2) {
        var_f31_2 = (f32) (0.05 + (f64) var_f30_2);
        if (var_f31_2 > (f32) 1.0) {
            var_f30_2 = (f32) ((f64) var_f30_2 - ((f64) var_f31_2 - 1.0));
            var_f31_2 = 1.0f;
        }
    }
    temp_f28_3 = (f32) g_get_bmp_width(0x1702);
    temp_f27 = (f32) g_get_bmp_height(0x1702);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(602.0f, (f32) (124.0 + (200.0 * (f64) var_f30_2)));
    g_set_ui_element_sprite_flags(6);
    set_ui_element_sprite_scale((f32) (16.0 / (f64) temp_f28_3), (f32) ((200.0 * (f64) (var_f31_2 - var_f30_2)) / (f64) temp_f27));
    draw_ui_box(0x1702);
    GXSetZMode_cached(1U, 3, 1U);
    temp_f31_2 = s_replayListPos.x;
    temp_f30_4 = s_replayListPos.y;
    if (temp_f31_2 < 960.0f) {
        temp_r14 = option_game_data_current_operation.subCursor;
        GXSetZMode_cached(1U, 7, 1U);
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_pos(temp_f31_2, temp_f30_4);
        set_ui_element_sprite_depth(0.1f);
        temp_f28_4 = 112.0f / (f32) g_get_bmp_height(0x1701);
        set_ui_element_sprite_scale(320.0f / (f32) g_get_bmp_width(0x1701), temp_f28_4);
        draw_ui_box(0x1701);
        GXSetZMode_cached(1U, 3, 1U);
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_drop_shadow();
        textdraw_set_alignment(4);
        textdraw_set_flags(0x80000000U);
        textdraw_set_depth(0.05f);
        textdraw_set_mul_color(0xFFEF9A00U);
        textdraw_set_pos(temp_f31_2, temp_f30_4 - 32.0f);
        textdraw_print(lbl_3_data_10390[locale]);
        textdraw_set_pos(temp_f31_2, temp_f30_4);
        textdraw_print(lbl_3_data_10404[locale]);
        textdraw_set_border();
        if (temp_r14 == 0) {
            textdraw_set_mul_color(-0x100U);
            temp_f0 = (f32) (g_font_char_var_3 % 60) / 60.0f;
            sp10 = temp_f0;
            temp_f0_2 = (f32) (1.0 - fabs((f32) ((2.0 * (f64) temp_f0) - 1.0)));
            sp10 = temp_f0_2;
            sp10 = (f32) fabs(temp_f0_2);
            sp10 = (f32) ((f64) sp10 * 255.0);
            option_f32_to_u8(&sp10, &sp9);
            textdraw_set_add_color(sp9 | ((sp9 << 0x10) | (sp9 << 8)));
            textdraw_set_pos(temp_f31_2 - 64.0f, 28.0f + temp_f30_4);
            textdraw_print("\x81\x6D      \x81\x6E");
        } else {
            textdraw_set_mul_color(0xFF7F7F00U);
            textdraw_set_add_color(0U);
        }
        textdraw_set_pos(temp_f31_2 - 64.0f, 28.0f + temp_f30_4);
        textdraw_print(lbl_3_data_FF38[locale]);
        if (temp_r14 == 1) {
            textdraw_set_mul_color(-0x100U);
            temp_f0_3 = (f32) (g_font_char_var_3 % 60) / 60.0f;
            spC = temp_f0_3;
            temp_f0_4 = (f32) (1.0 - fabs((f32) ((2.0 * (f64) temp_f0_3) - 1.0)));
            spC = temp_f0_4;
            spC = (f32) fabs(temp_f0_4);
            spC = (f32) ((f64) spC * 255.0);
            option_f32_to_u8(&spC, &sp8);
            textdraw_set_add_color(sp8 | ((sp8 << 0x10) | (sp8 << 8)));
            textdraw_set_pos(64.0f + temp_f31_2, 28.0f + temp_f30_4);
            textdraw_print("\x81\x6D      \x81\x6E");
        } else {
            textdraw_set_mul_color(0xFF7F7F00U);
            textdraw_set_add_color(0U);
        }
        textdraw_set_pos(64.0f + temp_f31_2, 28.0f + temp_f30_4);
        textdraw_print(lbl_3_data_FF5C[locale]);
    }
    if (temp_f29 > 0.0f) {
        temp_f31_3 = 540.0f;
        temp_f30_5 = 384.0f;
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000U);
        textdraw_set_scale(0.9f, 0.9f);
        textdraw_set_border();
        textdraw_set_spacing(0.0f, 4.0f);
        textdraw_set_pos(temp_f31_3, temp_f30_5);
        textdraw_set_alpha(temp_f29);
        textdraw_set_depth(0.1f);
        textdraw_set_mul_color(-1U);
        textdraw_print("\x83\xB4");
        textdraw_set_depth(0.4f);
        textdraw_set_mul_color(-0x100U);
        textdraw_print(":");
        textdraw_print_and_fit_to_width(48.0f, lbl_3_data_10390[locale]);
        textdraw_print("\n");
    }
    reset_ui_widescreen_scale_mtx();
}

char *lbl_3_data_10608[6] = {
    "Number of current Play Points",
    "Zahl der Spielpunkte",
    "Nombre actuel de Pts jeux",
    "N\xB0 actual de puntos juego",
    "Punti gioco attuali",
    "\x82\xB0\x82\xF1\x82\xB4\x82\xA2\x82\xCC\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67",
};
char *lbl_3_data_1066C[6] = {
    "Points needed to get the next Party Game",
    "",
    "",
    "",
    "",
    "\x83\x70\x81\x5B\x83\x65\x83\x42\x81\x5B\x83\x51\x81\x5B\x83\x80\x83\x51\x83\x62\x83\x67\x82\xDC\x82\xC5 \x82\xA0\x82\xC6",
};
char *lbl_3_data_106B8[6] = {
    "Unlocked Party Games",
    "",
    "",
    "",
    "",
    "\x83\x51\x83\x62\x83\x67\x82\xB5\x82\xBD\x83\x70\x81\x5B\x83\x65\x83\x42\x81\x5B\x83\x51\x81\x5B\x83\x80",
};
char *lbl_3_data_10748[6] = {
    "You don't have enough Play Points to get a gift.",
    "",
    "",
    "",
    "",
    "\x82\xA8\x82\xDC\x82\xAF\x82\xF0 \x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xE9\x82\xBE\x82\xAF\x82\xCC\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAA \x82\xDC\x82\xBE \x82\xBD\x82\xDC\x82\xC1\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x42",
};
char *lbl_3_data_107CC[6] = {
    "You have enough Play Points to get a gift!",
    "",
    "",
    "",
    "",
    "\x82\xA8\x82\xDC\x82\xAF\x82\xF0 \x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xE9\x82\xBE\x82\xAF\x82\xCC\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAA \x82\xBD\x82\xDC\x82\xC1\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x81\x49",
};
char *lbl_3_data_10820[6] = {
    "Newest Play Point record",
    "",
    "",
    "",
    "",
    "\x82\xB3\x82\xA2\x82\xB5\x82\xF1\x82\xCC\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAB\x82\xEB\x82\xAD",
};
char *lbl_3_data_10874[6] = {
    "Highest Play Points record",
    "",
    "",
    "",
    "",
    "\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xCC \x83\x54\x83\x43\x83\x52-\x82\xAB\x82\xEB\x82\xAD",
};
char *lbl_3_data_10958[6] = {
    "You still have locked Party Games.\nPlay more Story Mode\nor Challenge Mode!",
    "",
    "",
    "",
    "",
    "\x82\xA0\x82\xBB\x82\xD7\x82\xC8\x82\xA2 \x83\x70\x81\x5B\x83\x65\x83\x42\x81\x5B\x83\x51\x81\x5B\x83\x80\x82\xAA \x82\xDC\x82\xBE \x82\xA0\x82\xE8\x82\xDC\x82\xB7\x81\x42\n\x83\x81\x83\x43\x83\x93\x83\x51\x81\x5B\x83\x80\x82\xCC\x83\x58\x83\x67\x81\x5B\x83\x8A\x81\x5B\x83\x82\x81\x5B\x83\x68\x82\xA9\x81\x41\n\x83\x60\x83\x83\x83\x8C\x83\x93\x83\x57\x83\x82\x81\x5B\x83\x68\x82\xF0 \x82\xD0\x82\xC6\x82\xE8\x82\xC5 \x82\xA0\x82\xBB\x82\xD1\x82\xDC\x82\xB5\x82\xE5\x82\xA4\x81\x49",
};
char *lbl_3_data_109EC[6] = {
    "Now you have enough Play Points\nto get a Party Game!",
    "",
    "",
    "",
    "",
    "\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAA \x82\xBD\x82\xDC\x82\xE8\x82\xDC\x82\xB5\x82\xBD\x81\x42\n\x83\x70\x81\x5B\x83\x65\x83\x42\x81\x5B\x83\x51\x81\x5B\x83\x80\x82\xAA \x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xDC\x82\xB7\x81\x49",
};
char *lbl_3_data_10AF4[6] = {
    "You don't have enough Play Points\nto get a gift. Play more Story Mode\nor Challenge Mode!",
    "",
    "",
    "",
    "",
    "\x82\xA8\x82\xDC\x82\xAF\x82\xF0 \x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xE9\x82\xBE\x82\xAF\x82\xCC\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAA\n\x82\xDC\x82\xBE \x82\xBD\x82\xDC\x82\xC1\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x42\x83\x81\x83\x43\x83\x93\x83\x51\x81\x5B\x83\x80\x82\xCC\x83\x58\x83\x67\x81\x5B\x83\x8A\x81\x5B\x83\x82\x81\x5B\x83\x68\x82\xA9\x81\x41\n\x83\x60\x83\x83\x83\x8C\x83\x93\x83\x57\x83\x82\x81\x5B\x83\x68\x82\xF0 \x82\xD0\x82\xC6\x82\xE8\x82\xC5 \x82\xA0\x82\xBB\x82\xD1\x82\xDC\x82\xB5\x82\xE5\x82\xA4\x81\x49",
};
char *lbl_3_data_10BCC[6] = {
    "You have enough Play Points\nto get a gift! Check the\nGifts in the Options Menu!",
    "",
    "",
    "",
    "",
    "\x82\xA8\x82\xDC\x82\xAF\x82\xF0 \x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xE9\x82\xBE\x82\xAF\x82\xCC\n\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAA \x82\xBD\x82\xDC\x82\xC1\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x81\x49\n\x83\x49\x83\x76\x83\x56\x83\x87\x83\x93\x82\xC9\x82\xA0\x82\xE9 \x82\xA8\x82\xDC\x82\xAF\x82\xF0 \x83\x60\x83\x46\x83\x62\x83\x4E\x82\xB5\x82\xDC\x82\xB5\x82\xE5\x82\xA4\x81\x49",
};
char *lbl_3_data_10CC4[6] = {
    "You've unlocked everything!\nNow, you can try to set the\nhigh score for the most Play Points!",
    "",
    "",
    "",
    "",
    "\x82\xA0\x82\xC8\x82\xBD\x82\xCD\x81\x41\x82\xB7\x82\xD7\x82\xC4\x82\xCC\x82\xE0\x82\xCC\x82\xF0 \x83\x51\x83\x62\x83\x67\x82\xB5\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x81\x49\n\x82\xB1\x82\xEA\x82\xA9\x82\xE7\x82\xCD\x81\x41\x82\x50\x83\x76\x83\x8C\x83\x43\x82\xC5 \x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xE9\n\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAB\x82\xEB\x82\xAD\x82\xC9 \x83\x60\x83\x83\x83\x8C\x83\x93\x83\x57\x82\xB5\x82\xDC\x82\xB5\x82\xE5\x82\xA4\x81\x49",
};
char **lbl_3_data_10CDC[5] = { lbl_3_data_10958, lbl_3_data_109EC, lbl_3_data_10AF4, lbl_3_data_10BCC, lbl_3_data_10CC4 };

void fn_3_26F40(void) {
    struct Sprite *sprite;

    memset(&option_game_data_current_operation, 0, sizeof(option_game_data_current_operation));
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->x = 320.0f;
        sprite->y = 400.0f;
        sprite->tag = 0x21;
        sprite->drawFunc = fn_3_276E4;
        strcpy(sprite->text, "playpoint");
    }
}

void fn_3_26FC0(void) {
    /* PC: one struct, not 16 separate locals; their stack order is only the GC compiler's */
    struct SpriteDrawRequest req;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f31;
    f32 temp_f31_2;
    f32 temp_f31_3;
    f32 temp_f31_4;
    s32 temp_r0;
    s32 temp_ret;
    s32 var_r0_3;
    s32 var_r30;
    s32 var_r3;
    s32 var_r3_2;
    s8 var_r0_2;
    u32 temp_r4;
    u32 var_r0;

    temp_f31 = 145.0f;
    if (((s8) menuWork->unk8 == 0) && (did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
        g_b_button_pressed_on_menu(1);
    }
    set_ui_widescreen_scale_mtx(0x140);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(0.11f);
    set_ui_element_sprite_pos(320.0f, 210.0f);
    set_ui_element_sprite_scale(1.2f, 1.6f);
    draw_ui_box(0x1701);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_scale(0.8f, 1.0f);
    textdraw_set_border();
    textdraw_set_flags(0x80000000U);
    textdraw_set_alignment(1);
    textdraw_set_pos(100.0f, temp_f31);
    temp_ret = are_all_gifts_unlocked();
    temp_r4 = (u32) (u64) temp_ret;
    if (temp_ret != 0) {
        textdraw_printf_and_fit_to_width(340.0f, lbl_3_data_10820[locale], temp_r4);
    } else {
        textdraw_printf_and_fit_to_width(340.0f, lbl_3_data_10608[locale], temp_r4);
    }
    textdraw_clear_flags(0x80000000U);
    textdraw_print(":");
    textdraw_set_alignment(7);
    textdraw_set_pos(540.0f, temp_f31);
    if (are_all_gifts_unlocked() != 0) {
        textdraw_printf_and_fit_to_width(80.0f, "%5d", fn_1_16C7F8());
    } else {
        textdraw_printf_and_fit_to_width(80.0f, "%5d", get_play_point_count());
    }
    temp_f31_2 = temp_f31 + 32.0f;
    if (are_all_gifts_unlocked() != 0) {
        textdraw_set_flags(0x80000000U);
        textdraw_set_alignment(1);
        textdraw_set_pos(100.0f, temp_f31_2);
        textdraw_printf_and_fit_to_width(340.0f, lbl_3_data_10874[locale]);
        textdraw_clear_flags(0x80000000U);
        textdraw_print(":");
        textdraw_clear_flags(0x80000000U);
        textdraw_set_alignment(7);
        textdraw_set_pos(540.0f, temp_f31_2);
        textdraw_printf_and_fit_to_width(80.0f, "%5d", get_play_point_count());
        lbl_3_bss_8BC0 = 4;
    } else if (are_all_party_games_unlocked() != 0) {
        if (((is_able_to_unlock_gift() != 0) && (are_all_extra_monkeys_unlocked() == 0)) || ((fn_1_16CEBC() != 0) && (are_all_movies_unlocked() == 0)) || ((fn_1_16CFD4() != 0) && (is_staff_credits_game_unlocked() == 0))) {
            textdraw_set_flags(0x80000000U);
            textdraw_set_alignment(1);
            textdraw_set_pos(100.0f, temp_f31_2);
            textdraw_printf_and_fit_to_width(440.0f, lbl_3_data_107CC[locale]);
            lbl_3_bss_8BC0 = 3;
        } else {
            textdraw_set_flags(0x80000000U);
            textdraw_set_alignment(1);
            textdraw_set_pos(100.0f, temp_f31_2);
            textdraw_printf_and_fit_to_width(440.0f, lbl_3_data_10748[locale]);
            lbl_3_bss_8BC0 = 2;
        }
    } else {
        if ((s32) (0x9C4 - get_play_point_count()) > 0) {
            var_r0 = 0x9C4 - get_play_point_count();
        } else {
            var_r0 = 0U;
        }
        textdraw_set_flags(0x80000000U);
        textdraw_set_alignment(1);
        textdraw_set_pos(100.0f, temp_f31_2);
        textdraw_printf_and_fit_to_width(340.0f, lbl_3_data_1066C[locale]);
        textdraw_clear_flags(0x80000000U);
        textdraw_print(":");
        textdraw_clear_flags(0x80000000U);
        textdraw_set_alignment(7);
        textdraw_set_pos(540.0f, temp_f31_2);
        textdraw_printf_and_fit_to_width(80.0f, "%5d", var_r0);
        if (var_r0 == 0U) {
            var_r0_2 = 1;
        } else {
            var_r0_2 = 0;
        }
        lbl_3_bss_8BC0 = var_r0_2;
    }
    temp_f31_3 = temp_f31_2 + 32.0f;
    textdraw_set_flags(0x80000000U);
    textdraw_set_alignment(1);
    textdraw_set_pos(100.0f, temp_f31_3);
    textdraw_printf_and_fit_to_width(340.0f, lbl_3_data_106B8[locale]);
    textdraw_clear_flags(0x80000000U);
    textdraw_print(":");
    temp_f31_4 = temp_f31_3 + 32.0f;
    var_r30 = 0;
loop_40:
    if (var_r30 < 6) {
        req.bmpId = 0x170E;
        req.x = (f32) ((var_r30 * 0x4A) + 0x86);
        req.y = 20.0f + temp_f31_4;
        req.z = 0.1f;
        req.u1 = 0.0f;
        req.v1 = 0.0f;
        req.u2 = 1.0f;
        req.v2 = 1.0f;
        req.scaleX = 0.5f;
        req.scaleY = 0.5f;
        req.rotation = 0;
        req.opacity = 1.0f;
        req.unk30 = -1;
        req.flags = 0xA;
        if (g_check_if_partygame_unlocked(lbl_3_rodata_4[var_r30 + 6]) != 0) {
            var_r3 = 0xFFFFFF;
        } else {
            var_r3 = 0x202020;
        }
        req.mulColor = var_r3;
        req.addColor = 0;
        draw_sprite_draw_request(&req);
        req.bmpId = 0x1731;
        req.z -= 0.01f;
        req.scaleX *= 1.3f;
        req.scaleY *= 1.3f;
        if (g_check_if_partygame_unlocked(lbl_3_rodata_4[var_r30 + 6]) != 0) {
            var_r0_3 = 0x330000;
        } else {
            var_r0_3 = 0x60000;
        }
        req.mulColor = var_r0_3;
        if (var_r30 >= 6) {
            var_r3_2 = 1;
        } else {
            var_r3_2 = 0;
        }
        temp_r0 = var_r3_2 + 7;
        temp_f0 = (f32) ((var_r30 + temp_r0) % 7) / 7.0f;
        req.u1 = temp_f0;
        req.u2 = 0.142857f + temp_f0;
        temp_f0_2 = (f32) ((var_r30 + temp_r0) / 7) * 0.5f;
        req.v1 = temp_f0_2;
        req.v2 = 0.5f + temp_f0_2;
        req.scaleX *= req.u2 - temp_f0;
        req.scaleY *= req.v2 - temp_f0_2;
        draw_sprite_draw_request(&req);
        var_r30 += 1;
        goto loop_40;
    }
    reset_ui_widescreen_scale_mtx();
    option_game_data_current_operation.timer += 1;
}

void fn_3_276E4(struct Sprite *arg0) {
    set_ui_widescreen_scale_mtx(0x140);
    fn_3_140CC(option_game_data_current_operation.timer);
    textdraw_set_pos(arg0->x, arg0->y);
    textdraw_print_and_fit_to_width(390.0f, lbl_3_data_10CDC[lbl_3_bss_8BC0][(s8)g_language_id]);
    reset_ui_widescreen_scale_mtx();
}

void fn_3_27780(void) {
    destroy_sprite_with_unique_id(0x21);
}

char *lbl_3_data_10D74[6][3] = {
    {"Monkeys\n%dpts", "Movie %dpts", "Staff Credit\nGame %dpts"},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"\x83\x54\x83\x8B\x82\xCC\x82\xA9\x82\xB8\n%dpts", "\x83\x80\x81\x5B\x83\x72\x81\x5B %dpts", "\x83\x58\x83\x5E\x83\x62\x83\x74\x83\x8D\x81\x5B\x83\x8B\n\x83\x51\x81\x5B\x83\x80 %dpts"},
};
char *lbl_3_data_10E10[6][3] = {
    {"Monkeys", "Movie", "Staff Credit\nGame"},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"\x83\x54\x83\x8B\x82\xCC\x82\xA9\x82\xB8", "\x83\x80\x81\x5B\x83\x72\x81\x5B", "\x83\x58\x83\x5E\x83\x62\x83\x74\x83\x8D\x81\x5B\x83\x8B\n\x83\x51\x81\x5B\x83\x80\n "},
};
char *lbl_3_data_1104C[6][3] = {
    {"You can add monkeys to your\nstarting life in Challenge Mode.\nYou don't have enough Play Points.", "You can get a movie which\nyou have seen in the game.", "You can get the Staff\nCredit Game from Main Game.\nYou don't have enough Play Points."},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"\x83\x60\x83\x83\x83\x8C\x83\x93\x83\x57\x83\x82\x81\x5B\x83\x68\x82\xC5\x81\x41\x82\xB3\x82\xA2\x82\xB5\x82\xE5\x82\xC9\x82\xE0\x82\xC1\x82\xC4\x82\xA2\x82\xE9\n\x83\x54\x83\x8B\x82\xCC\x82\xA9\x82\xB8\x82\xF0 \x82\xD3\x82\xE2\x82\xB9\x82\xDC\x82\xB7\x81\x42\n\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAA \x82\xDC\x82\xBE \x82\xBD\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x42", "\x82\xA2\x82\xBF\x82\xC7 \x82\xDD\x82\xBD\x82\xB1\x82\xC6\x82\xCC\x82\xA0\x82\xE9 \x83\x80\x81\x5B\x83\x72\x81\x5B\x82\xF0\n\x83\x51\x83\x62\x83\x67\x82\xB7\x82\xE9\x82\xB1\x82\xC6\x82\xAA \x82\xC5\x82\xAB\x82\xDC\x82\xB7\x81\x42", "\x83\x81\x83\x43\x83\x93\x83\x51\x81\x5B\x83\x80\x82\xCC \x83\x58\x83\x5E\x83\x62\x83\x74\x83\x8D\x81\x5B\x83\x8B\x83\x51\x81\x5B\x83\x80\x82\xF0\n\x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xDC\x82\xB7\x81\x42\n\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAA \x82\xDC\x82\xBE \x82\xBD\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x42"},
};
char *lbl_3_data_11300[6][3] = {
    {"Please press \x83\x9F to add\na monkey. A monkey costs you\n/bcff8000/%d/bcffff00/ Play Points.", "Please press \x83\x9F to get\na movie. A movie costs you\n/bcff8000/%d/bcffff00/ Play Points.", "Please press \x83\x9F to get the\nStaff Credit Game. The Staff Credit\nGame costs /bcff8000/%d/bcffff00/ Play Points"},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"\x83\x9F\x82\xF0 \x82\xA8\x82\xB7\x82\xC6\x81\x41\x83\x54\x83\x8B\x82\xCC\x82\xA9\x82\xB8\x82\xAA\n\x82\xD3\x82\xA6\x82\xDC\x82\xB7\x81\x42\x83\x54\x83\x8B\x82\x50\x82\xD2\x82\xAB\x82\xC9\x82\xC2\x82\xAB\x81\x41\n/bcff8000/%d/bcffff00/\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67 \x82\xC2\x82\xA9\x82\xA2\x82\xDC\x82\xB7\x81\x42", "\x83\x9F\x82\xF0 \x82\xA8\x82\xB7\x82\xC6\x81\x41\x82\xB1\x82\xCC\x83\x80\x81\x5B\x83\x72\x81\x5B\x82\xAA \x83\x51\x83\x62\x83\x67\n\x82\xC5\x82\xAB\x82\xDC\x82\xB7\x81\x42\x83\x80\x81\x5B\x83\x72\x81\x5B\x82\x50\x82\xDB\x82\xF1\x82\xC9\x82\xC2\x82\xAB\x81\x41\n/bcff8000/%d/bcffff00/\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67 \x82\xC2\x82\xA9\x82\xA2\x82\xDC\x82\xB7\x81\x42", "\x83\x9F\x82\xF0 \x82\xA8\x82\xB7\x82\xC6\x81\x41\x83\x58\x83\x5E\x83\x62\x83\x74\x83\x8D\x81\x5B\x83\x8B\x83\x51\x81\x5B\x83\x80\x82\xAA\n\x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xDC\x82\xB7\x81\x42\n/bcff8000/%d/bcffff00/\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67 \x82\xC2\x82\xA9\x82\xA2\x82\xDC\x82\xB7\x81\x42"},
};
char *lbl_3_data_114BC[6][3] = {
    {"You cannot add any more monkeys.", "You already have all the movies.", "You can play the Staff Credit Game\nfrom Main Game. Please press \x83\x9F to\n/bcff8000/start the Staff Credit Game."},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"", "", ""},
    {"\x82\xB1\x82\xEA\x82\xA2\x82\xB6\x82\xE5\x82\xA4\x81\x41\x83\x54\x83\x8B\x82\xCD \x82\xD3\x82\xA6\x82\xDC\x82\xB9\x82\xF1\x81\x42", "\x82\xB7\x82\xD7\x82\xC4\x82\xCC \x83\x80\x81\x5B\x83\x72\x81\x5B\x82\xF0 \x83\x51\x83\x62\x83\x67\x82\xB5\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x81\x42", "\x83\x81\x83\x43\x83\x93\x83\x51\x81\x5B\x83\x80\x82\xCC \x83\x58\x83\x5E\x83\x62\x83\x74\x83\x8D\x81\x5B\x83\x8B\x83\x51\x81\x5B\x83\x80\x82\xC5\n\x82\xA0\x82\xBB\x82\xD7\x82\xDC\x82\xB7\x81\x42\x83\x9F\x82\xF0 \x82\xA8\x82\xB7\x82\xC6\x81\x41\n/bcff8000/\x83\x58\x83\x5E\x83\x62\x83\x74\x83\x8D\x81\x5B\x83\x8B\x83\x51\x81\x5B\x83\x80\x82\xAA \x82\xCD\x82\xB6\x82\xDC\x82\xE8\x82\xDC\x82\xB7\x81\x42"},
};
char *lbl_3_data_115BC[6] = {
    "You can get a movie which\nyou have seen in the game.\nYou don't have enough Play Points.",
    "",
    "",
    "",
    "",
    "\x82\xA2\x82\xBF\x82\xC7 \x82\xDD\x82\xBD\x82\xB1\x82\xC6\x82\xCC\x82\xA0\x82\xE9 \x83\x80\x81\x5B\x83\x72\x81\x5B\x82\xF0\n\x83\x51\x83\x62\x83\x67\x82\xB7\x82\xE9\x82\xB1\x82\xC6\x82\xAA \x82\xC5\x82\xAB\x82\xDC\x82\xB7\x81\x42\n\x83\x76\x83\x8C\x83\x43\x83\x7C\x83\x43\x83\x93\x83\x67\x82\xAA \x82\xDC\x82\xBE \x82\xBD\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x42",
};
char *lbl_3_data_11644[6] = {
    "You can't get the movie\nbecause you haven't seen it.",
    "",
    "",
    "",
    "",
    "\x82\xB1\x82\xCC\x83\x80\x81\x5B\x83\x72\x81\x5B\x82\xCD\x81\x41\x82\xDC\x82\xBE \x82\xDD\x82\xC4\x82\xA2\x82\xC8\x82\xA2\x82\xCC\x82\xC5\x81\x41\x83\x51\x83\x62\x83\x67\x82\xC5\x82\xAB\x82\xDC\x82\xB9\x82\xF1\x81\x42",
};
char *lbl_3_data_11714[6] = {
    "Please press \x83\x9F to\n/bcff8000/see this movie.\n/bcffff00/Press \x83\xA0 to stop.",
    "",
    "",
    "",
    "",
    "\x83\x9F\x82\xF0\x82\xA8\x82\xB7\x82\xC6\x81\x41/bcff8000/\x82\xB1\x82\xCC\x83\x80\x81\x5B\x83\x72\x81\x5B\x82\xAA \x82\xDD\x82\xEA\x82\xDC\x82\xB7\x81\x42\n/bcffff00/\x82\xC6\x82\xBF\x82\xE3\x82\xA4\x82\xC5 \x82\xE2\x82\xDF\x82\xBD\x82\xA2\x82\xC6\x82\xAB\x82\xCD\x81\x41\n\x83\xA0\x82\xF0 \x82\xA8\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42",
};

void fn_3_277A4(void) {
    s8 var_r29;
    struct Sprite *sprite;

    var_r29 = 0;
    if (are_all_extra_monkeys_unlocked() != 0) {
        var_r29 = 1;
    }
    if ((are_all_movies_unlocked() == 0) && (fn_1_16CEE4() != 0)) {
        var_r29 = 1;
    }
    if ((is_staff_credits_game_unlocked() == 0) && (fn_1_16CFFC() != 0)) {
        var_r29 = 2;
    }
    lbl_3_bss_8BC1 = var_r29;
    s_giftMsgTimer = 0x1E;
    g_gift_menu_cursor_pos = -1;
    s_giftBuyTimer = 0;
    memset(&option_game_data_current_operation, 0, sizeof(option_game_data_current_operation));
    sprite = create_sprite();
    if (sprite != NULL) {
        sprite->x = 320.0f;
        sprite->y = 400.0f;
        sprite->tag = 0x21;
        sprite->drawFunc = fn_3_28D30;
        strcpy(sprite->text, "omake");
    }
}

static const s32 s_giftCosts[3] = { 500, 1000, 1000 };

void g_draw_gift_menu(void) {
    f32 posX[3] = { 90.0f, 320.0f, 550.0f };
    f32 textOffX[3] = { 0.0f, -60.0f, 0.0f };
    f32 posY[3] = { 230.0f, 230.0f, 230.0f };
    f32 titleOffY[3] = { -20.0f, -57.0f, -20.0f };
    f32 bodyOffY[3] = { 30.0f, 15.0f, 30.0f };
    f32 boxScaleX[3] = { 0.3f, 0.7f, 0.3f };
    f32 boxScaleY[3] = { 1.0f, 1.3f, 1.0f };
    f32 textWidth[3] = { 100.0f, 120.0f, 100.0f };
    char *monkeyLabel[6] = { "Monkeys", "", "", "", "", "\x83\x54\x83\x8B\x82\xCC\x82\xA9\x82\xB8" };
    char *movieNames[12][6] = {
        { "Prologue", "", "", "", "", "\x83\x76\x83\x8D\x83\x8D\x81\x5B\x83\x4F" },
        { "Scene 1", "1", "1", "1", "1", "\x83\x56\x81\x5B\x83\x93 1" },
        { "Scene 2", "2", "2", "2", "2", "\x83\x56\x81\x5B\x83\x93 2" },
        { "Scene 3", "3", "3", "3", "3", "\x83\x56\x81\x5B\x83\x93 3" },
        { "Scene 4", "4", "4", "4", "4", "\x83\x56\x81\x5B\x83\x93 4" },
        { "Scene 5", "5", "5", "5", "5", "\x83\x56\x81\x5B\x83\x93 5" },
        { "Scene 6", "6", "6", "6", "6", "\x83\x56\x81\x5B\x83\x93 6" },
        { "Scene 7", "7", "7", "7", "7", "\x83\x56\x81\x5B\x83\x93 7" },
        { "Scene 8", "8", "8", "8", "8", "\x83\x56\x81\x5B\x83\x93 8" },
        { "Scene 9", "9", "9", "9", "9", "\x83\x56\x81\x5B\x83\x93 9" },
        { "Scene 10", "10", "10", "10", "10", "\x83\x56\x81\x5B\x83\x93 10" },
        { "Epilogue", "", "", "", "", "\x83\x47\x83\x73\x83\x8D\x81\x5B\x83\x4F" },
    };
    char *movieTags[12][6] = {
        { "P", "", "", "", "", "\x83\x76" },
        { "1", "1", "1", "1", "1", "1" },
        { "2", "2", "2", "2", "2", "2" },
        { "3", "3", "3", "3", "3", "3" },
        { "4", "4", "4", "4", "4", "4" },
        { "5", "5", "5", "5", "5", "5" },
        { "6", "6", "6", "6", "6", "6" },
        { "7", "7", "7", "7", "7", "7" },
        { "8", "8", "8", "8", "8", "8" },
        { "9", "9", "9", "9", "9", "9" },
        { "10", "10", "10", "10", "10", "10" },
        { "E", "", "", "", "", "\x83\x47" },
    };
    s32 i;
    s32 j;
    s32 bought;
    s32 enabled;
    s32 maxItem;
    s32 dimmed;
    s32 pad;
    u32 flash;
    u32 movieFlash;
    u32 color;
    u32 t;

    bought = 0;
    t = (u32) (32.0 * fabs(math_sin((s16) (g_font_char_var_3 * 0x180))));
    flash = t | ((t << 0x10) | (t << 8));
    for (i = 0; i < 3; i++) {
        lbl_3_bss_8BC8[i] = 0;
    }
    if (are_all_extra_monkeys_unlocked() != 0) {
        lbl_3_bss_8BC8[0] = 1;
    }
    if (are_all_movies_unlocked() != 0) {
        lbl_3_bss_8BC8[1] = 1;
    }
    if (is_staff_credits_game_unlocked() != 0) {
        lbl_3_bss_8BC8[2] = 1;
    }
    if (s_giftBuyTimer > 0) {
        s_giftBuyTimer--;
        t = (((u32) g_font_char_var_3 >> 2U) & 1) << 6;
        flash = t | ((t << 0x10) | (t << 8));
        if (s_giftBuyTimer == 0x3C) {
            g_change_music_volume(-1, 0x3CU, 0);
            fade_screen_to_color(0x101, 0U, 0x3C);
            goto draw;
        }
        if (s_giftBuyTimer == 0) {
            switch (lbl_3_bss_8BC1) {
            case 1:
                modeCtrl.unk10 = g_gift_menu_cursor_pos;
                modeCtrl.playerCount = 1;
                modeCtrl.unk30 = 1;
                main_mode_request = 6;
                sub_mode_request = 0xFA;
                break;
            case 2:
                modeCtrl.courseFlags |= 0x40000;
                main_mode_request = 2;
                sub_mode_request = 0x53;
                break;
            }
            return;
        }
        goto draw;
    }
    if ((s8) menuWork->unk8 == 0) {
        if ((did_any_pad_press_input(7) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            if (g_gift_menu_cursor_pos == -1) {
                if (s_giftMsgTimer == 0) {
                    fn_3_15F8();
                    g_b_button_pressed_on_menu(1);
                }
            } else {
                call_SoundReqID_arg_0(0x66);
                g_gift_menu_cursor_pos = -1;
            }
        }
        if (did_any_pad_press_input(1) != 0) {
            if (g_gift_menu_cursor_pos == -1) {
                if ((g_some_gift_menu_flags & 8) != 0) {
                    maxItem = 2;
                } else {
                    maxItem = 1;
                }
                lbl_3_bss_8BC1++;
                if (lbl_3_bss_8BC1 > maxItem) {
                    lbl_3_bss_8BC1 = 0;
                }
            } else if (g_gift_menu_cursor_pos % 4 == 3) {
                g_gift_menu_cursor_pos -= 3;
            } else {
                g_gift_menu_cursor_pos += 1;
            }
            call_SoundReqID_arg_0(0x6C);
        } else if (did_any_pad_press_input(0) != 0) {
            if (g_gift_menu_cursor_pos == -1) {
                s8 maxItem2;

                if ((g_some_gift_menu_flags & 8) != 0) {
                    maxItem2 = 2;
                } else {
                    maxItem2 = 1;
                }
                lbl_3_bss_8BC1--;
                if (lbl_3_bss_8BC1 < 0) {
                    lbl_3_bss_8BC1 = maxItem2;
                }
            } else if (g_gift_menu_cursor_pos % 4 == 0) {
                g_gift_menu_cursor_pos += 3;
            } else {
                g_gift_menu_cursor_pos -= 1;
            }
            call_SoundReqID_arg_0(0x6C);
        } else if (g_gift_menu_cursor_pos != -1) {
            if (did_any_pad_press_input(3) != 0) {
                if (g_gift_menu_cursor_pos / 4 == 2) {
                    g_gift_menu_cursor_pos -= 8;
                } else {
                    g_gift_menu_cursor_pos += 4;
                }
                call_SoundReqID_arg_0(0x6C);
            } else if (did_any_pad_press_input(2) != 0) {
                if (g_gift_menu_cursor_pos / 4 == 0) {
                    g_gift_menu_cursor_pos += 8;
                } else {
                    g_gift_menu_cursor_pos -= 4;
                }
                call_SoundReqID_arg_0(0x6C);
            }
        }
        if (s_giftMsgTimer > 0) {
            s_giftMsgTimer--;
        } else if ((did_any_pad_press_input(6) != 0) && ((s32) menuWork->unkC == 0) && ((s32) menuWork->unk18 == 0)) {
            switch (lbl_3_bss_8BC1) {
            case 0:
                if ((lbl_3_bss_8BC8[0] == 0) && (is_able_to_unlock_gift() != 0)) {
                    g_something_with_assinging_unlockables();
                    bought = 1;
                }
            default:
            result:
                if (s_giftBuyTimer != 0) {
                    call_SoundReqID_arg_0(0x6A);
                } else if (bought != 0) {
                    if (are_all_gifts_unlocked() != 0) {
                        fn_3_15F8();
                        fn_1_16C55C();
                    } else {
                        fn_3_14D8(1);
                    }
                    s_giftMsgTimer = 0x1E;
                    option_game_data_current_operation.timer = 0;
                } else {
                    call_SoundReqID_arg_0(0x64);
                }
                if (are_all_extra_monkeys_unlocked() != 0) {
                    lbl_3_bss_8BC8[0] = 1;
                }
                if (are_all_movies_unlocked() != 0) {
                    lbl_3_bss_8BC8[1] = 1;
                }
                if (is_staff_credits_game_unlocked() != 0) {
                    lbl_3_bss_8BC8[2] = 1;
                }
                break;
            case 1:
                if (g_gift_menu_cursor_pos == -1) {
                    g_gift_menu_cursor_pos = 0;
                    call_SoundReqID_arg_0(0x65);
                } else {
                    if ((g_is_movie_unlocked(g_gift_menu_cursor_pos) != 0) && (fn_1_16D350(g_gift_menu_cursor_pos) == 0) && (fn_1_16CEBC() != 0)) {
                        unlock_movie(g_gift_menu_cursor_pos);
                        bought = 1;
                    } else if (fn_1_16D350(g_gift_menu_cursor_pos) != 0) {
                        for (pad = 0; pad < 4; pad++) {
                            player_pad_map[pad] = -1;
                        }
                        for (pad = 0; pad < 4; pad++) {
                            if (pad_status_groups[pad].unk0[0].err == 0 && (pad_status_groups[pad].unk0[2].button & 0x100) != 0) {
                                break;
                            }
                        }
                        player_pad_map[0] = pad;
                        player_pad_map[1] = pad;
                        player_pad_map[2] = pad;
                        player_pad_map[3] = pad;
                        *active_monkey_id = 0;
                        modeCtrl.playerCount = 1;
                        s_giftBuyTimer = 0x78;
                    }
                    goto result;
                }
                break;
            case 2:
                if ((lbl_3_bss_8BC8[2] == 0) && (fn_1_16CFD4() != 0)) {
                    unlock_staff_credits_game();
                    bought = 1;
                } else if (lbl_3_bss_8BC8[2] != 0) {
                    for (pad = 1; pad < 4; pad++) {
                        player_pad_map[pad] = -1;
                    }
                    for (pad = 0; pad < 4; pad++) {
                        if (pad_status_groups[pad].unk0[0].err == 0 && (pad_status_groups[pad].unk0[2].button & 0x100) != 0) {
                            break;
                        }
                    }
                    player_pad_map[0] = pad;
                    *active_monkey_id = 0;
                    modeCtrl.playerCount = 1;
                    s_giftBuyTimer = 0x78;
                }
                goto result;
            }
        }
    }
draw:
    set_ui_widescreen_scale_mtx(0x140);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_depth(0.2f);
    g_set_ui_element_sprite_flags(0xA);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_scale(0.8f, 1.0f);
    textdraw_set_drop_shadow();
    textdraw_set_flags(0x80000000U);
    textdraw_set_alignment(4);
    for (i = 0; i < 3; i++) {
        enabled = 0;
        if ((i != 2) || ((g_some_gift_menu_flags & 8) != 0)) {
            enabled = 1;
        }
        set_ui_element_sprite_scale(boxScaleX[i], boxScaleY[i]);
        if (enabled != 0) {
            if (i == lbl_3_bss_8BC1) {
                dimmed = 0;
                if ((s_giftMsgTimer != 0) || (g_gift_menu_cursor_pos != -1)) {
                    dimmed = 1;
                }
                color = (dimmed != 0) ? 0xC0C0C0 : 0xFFFFFF;
            } else {
                color = 0x808080;
            }
            set_ui_element_sprite_mult_color_from_u32(color);
        } else {
            set_ui_element_sprite_mult_color_from_u32(0x202020);
        }
        if (i == lbl_3_bss_8BC1) {
            dimmed = 0;
            if ((s_giftMsgTimer != 0) || (g_gift_menu_cursor_pos != -1)) {
                dimmed = 1;
            }
            color = (dimmed != 0) ? 0 : flash;
        } else {
            color = 0;
        }
        set_ui_element_sprite_add_color_from_u32(color);
        set_ui_element_sprite_pos(posX[i], posY[i]);
        draw_ui_box(0x1701);
        if (enabled == 0) {
            textdraw_set_scale(3.0f, 3.0f);
            textdraw_set_border();
            textdraw_set_mul_color(0x400000U);
            textdraw_set_pos(posX[i] + textOffX[i], posY[i]);
            textdraw_print("?");
            continue;
        }
        if (i == lbl_3_bss_8BC1) {
            dimmed = 0;
            if ((s_giftMsgTimer != 0) || (g_gift_menu_cursor_pos != -1)) {
                dimmed = 1;
            }
            color = (dimmed != 0) ? 0xC0C000 : 0xFFFF00;
        } else {
            color = 0x404000;
        }
        textdraw_set_mul_color(color);
        textdraw_set_pos(posX[i] + textOffX[i], posY[i] + titleOffY[i]);
        if (lbl_3_bss_8BC8[i] != 0) {
            textdraw_printf_and_fit_to_width(textWidth[i], lbl_3_data_10E10[locale][i], s_giftCosts[i]);
        } else {
            textdraw_printf_and_fit_to_width(textWidth[i], lbl_3_data_10D74[locale][i], s_giftCosts[i]);
        }
        switch (i) {
        case 0:
            (void) monkeyLabel;
            textdraw_set_scale(0.6f, 0.8f);
            textdraw_set_pos(posX[i], posY[i] + bodyOffY[i]);
            textdraw_printf_and_fit_to_width(textWidth[i], "            x %d", get_num_unlocked_monkeys());
            textdraw_set_scale(0.8f, 1.0f);
            set_ui_element_sprite_depth(0.05f);
            set_ui_element_sprite_scale(0.5f, 0.3f);
            if (i == lbl_3_bss_8BC1) {
                dimmed = 0;
                if ((s_giftMsgTimer != 0) || (g_gift_menu_cursor_pos != -1)) {
                    dimmed = 1;
                }
                color = (dimmed != 0) ? 0xC0C0C0 : 0xFFFFFF;
            } else {
                color = 0x808080;
            }
            set_ui_element_sprite_mult_color_from_u32(color);
            set_ui_element_sprite_add_color_from_u32(0U);
            set_ui_element_sprite_pos(posX[i] - 20.0f, posY[i] + bodyOffY[i]);
            draw_ui_element(0xF);
            if (i == lbl_3_bss_8BC1) {
                set_ui_element_sprite_alpha((f32) fabs(math_sin((s16) (g_font_char_var_3 * 0x300))));
                set_ui_element_sprite_depth(-0.0000000014901161f);
                draw_ui_element(0x48);
            }
            set_ui_element_sprite_alpha(1.0f);
            set_ui_element_sprite_depth(0.2f);
            break;
        case 1:
            movieFlash = (u32) (64.0 * fabs(math_sin((s16) (g_font_char_var_3 * 0x180))));
            if (s_giftBuyTimer != 0) {
                movieFlash = (((u32) g_font_char_var_3 >> 2U) & 1) << 7;
            }
            set_ui_element_sprite_depth(0.075f);
            set_ui_element_sprite_scale(1.1f, 1.0f);
            for (j = 0; j < 12; j++) {
                if (g_gift_menu_cursor_pos == -1) {
                    if (lbl_3_bss_8BC1 != 1) {
                        color = (g_is_movie_unlocked(j) != 0) ? 0x606060 : 0x404040;
                        set_ui_element_sprite_mult_color_from_u32(color);
                    } else {
                        color = (g_is_movie_unlocked(j) != 0) ? 0xA0A0A0 : 0x606060;
                        set_ui_element_sprite_mult_color_from_u32(color);
                    }
                } else {
                    dimmed = 0;
                    if ((s_giftMsgTimer != 0) && (g_gift_menu_cursor_pos == j)) {
                        dimmed = 1;
                    }
                    if (dimmed != 0) {
                        color = 0xC0C0C0;
                    } else if (g_is_movie_unlocked(j) != 0) {
                        color = 0xFFFFFF;
                    } else {
                        color = 0x606060;
                    }
                    set_ui_element_sprite_mult_color_from_u32(color);
                }
                if (j == g_gift_menu_cursor_pos) {
                    color = movieFlash | ((movieFlash << 0x10) | (movieFlash << 8));
                } else {
                    color = 0;
                }
                set_ui_element_sprite_add_color_from_u32(color);
                set_ui_element_sprite_pos((posX[i] - 96.0f) + (f32) ((j % 4) * 0x3E), (f32) ((j / 4) * 0x26) + ((posY[i] + bodyOffY[i]) - 38.0f));
                draw_ui_element(0x174D);
            }
            set_ui_element_sprite_depth(0.2f);
            if (g_gift_menu_cursor_pos != -1) {
                textdraw_set_scale(0.6f, 0.8f);
                textdraw_set_pos(60.0f + posX[i], 2.0f + (posY[i] + titleOffY[i]));
                textdraw_printf_and_fit_to_width(100.0f, "\x81\x60 %s \x81\x60", movieNames[g_gift_menu_cursor_pos][(s8) g_language_id]);
            }
            textdraw_set_scale(0.7f, 0.7f);
            textdraw_clear_drop_shadow();
            for (j = 0; j < 12; j++) {
                textdraw_set_depth(0.045f);
                textdraw_set_mul_color(0U);
                textdraw_set_add_color(0U);
                textdraw_set_pos((posX[i] - 96.0f) + (f32) ((j % 4) * 0x3E), (f32) ((j / 4) * 0x26) + ((posY[i] + bodyOffY[i]) - 38.0f));
                textdraw_printf(movieTags[j][(s8) g_language_id]);
                if (fn_1_16D350(j) != 0) {
                    textdraw_set_depth(0.07f);
                    textdraw_set_border();
                    textdraw_set_rot_z(0xC000);
                    if (g_gift_menu_cursor_pos == -1) {
                        if (lbl_3_bss_8BC1 != 1) {
                            textdraw_set_mul_color(0x4000U);
                        } else {
                            textdraw_set_mul_color(0x8000U);
                        }
                    } else {
                        textdraw_set_mul_color(0xFF00U);
                    }
                    textdraw_set_pos(30.0f + ((posX[i] - 96.0f) + (f32) ((j % 4) * 0x3E)), 6.0f + ((f32) ((j / 4) * 0x26) + ((posY[i] + bodyOffY[i]) - 38.0f)));
                    textdraw_print("\x81\xA3");
                    textdraw_set_rot_z(0);
                    textdraw_clear_border();
                }
            }
            textdraw_set_depth(0.1f);
            textdraw_set_scale(0.8f, 1.0f);
            textdraw_set_drop_shadow();
            textdraw_set_add_color(0U);
            break;
        case 2:
            set_ui_element_sprite_depth(0.05f);
            set_ui_element_sprite_scale(1.1f, 1.1f);
            if (is_staff_credits_game_unlocked() != 0) {
                if (i == lbl_3_bss_8BC1) {
                    dimmed = 0;
                    if ((s_giftMsgTimer != 0) || (g_gift_menu_cursor_pos != -1)) {
                        dimmed = 1;
                    }
                    color = (dimmed != 0) ? 0xC0C0C0 : 0xFFFFFF;
                } else {
                    color = 0x808080;
                }
                set_ui_element_sprite_mult_color_from_u32(color);
            } else {
                color = (i == lbl_3_bss_8BC1) ? 0x404040 : 0x202020;
                set_ui_element_sprite_mult_color_from_u32(color);
            }
            set_ui_element_sprite_add_color_from_u32(0U);
            set_ui_element_sprite_pos(posX[i], posY[i] + bodyOffY[i]);
            draw_ui_element(0x174E);
            break;
        }
    }
    reset_ui_widescreen_scale_mtx();
    option_game_data_current_operation.timer += 1;
}

void fn_3_28D30(struct Sprite *sprite) {
    char buf[0x100];

    set_ui_widescreen_scale_mtx(0x140);
    fn_3_140CC(option_game_data_current_operation.timer);
    textdraw_set_flags(0x10000000U);
    if (g_gift_menu_cursor_pos != -1) {
        if (fn_1_16D350(g_gift_menu_cursor_pos) != 0) {
            strcpy(buf, lbl_3_data_11714[locale]);
        } else if (g_is_movie_unlocked(g_gift_menu_cursor_pos) == 0) {
            strcpy(buf, lbl_3_data_11644[locale]);
        } else if (get_play_point_count() >= s_giftCosts[lbl_3_bss_8BC1]) {
            sprintf(buf, lbl_3_data_11300[locale][lbl_3_bss_8BC1], s_giftCosts[lbl_3_bss_8BC1]);
        } else {
            strcpy(buf, lbl_3_data_115BC[locale]);
        }
    } else if (lbl_3_bss_8BC8[lbl_3_bss_8BC1] != 0) {
        strcpy(buf, lbl_3_data_114BC[locale][lbl_3_bss_8BC1]);
    } else if ((get_play_point_count() >= s_giftCosts[lbl_3_bss_8BC1]) && ((lbl_3_bss_8BC1 != 1) || (g_gift_menu_cursor_pos != -1))) {
        sprintf(buf, lbl_3_data_11300[locale][lbl_3_bss_8BC1], s_giftCosts[lbl_3_bss_8BC1]);
    } else if ((lbl_3_bss_8BC1 == 1) && (get_play_point_count() < s_giftCosts[lbl_3_bss_8BC1])) {
        strcpy(buf, lbl_3_data_115BC[locale]);
    } else {
        strcpy(buf, lbl_3_data_1104C[locale][lbl_3_bss_8BC1]);
    }
    textdraw_set_pos(sprite->x, sprite->y);
    textdraw_print_and_fit_to_width(390.0f, buf);
    reset_ui_widescreen_scale_mtx();
}

void fn_3_290CC(void) {
    destroy_sprite_with_unique_id(0x21);
}
/* Warning: struct MemcardRequest is not defined (only forward-declared) */
/* Warning: struct StickPoint is not defined (only forward-declared) */
/* Warning: struct Preview is not defined (only forward-declared) */
