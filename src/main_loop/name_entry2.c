/* Name entry and rankings screens shown at the end of a game. */
#include "types.h"
#include "ball.h"
#include "dip.h"
#include "libc.h"
#include "macros.h"
#include "mode.h"
#include "camera.h"
#include "avdisp.h"
#include "bitmap.h"
#include "effect.h"
#include "input.h"
#include "mathutil.h"
#include "stage.h"
#include "sprite.h"

/* one entry of the story-mode ranking kept while playing */
struct NameEntryRec
{
    /*0x00*/ char name[8];
    /*0x08*/ s32 score;
    /*0x0C*/ s32 unkC;
    /*0x10*/ u8 unk10;
    /*0x11*/ s8 unk11;
    /*0x12*/ u8 unk12;
    u8 unk13[1];
    /*0x14*/ s32 unk14;
    /*0x18*/ s8 unk18;
    u8 unk19[3];
}; /* size = 0x1C */

struct NameEntryData
{
    /*0x00*/ char name[8];
    /*0x08*/ s32 score;
    /*0x0C*/ s32 unkC;
    /*0x10*/ u8 unk10;
    /*0x11*/ s8 unk11;
    /*0x12*/ s8 unk12;
    u8 unk13[1];
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ struct Ape *unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ u8 unk24;
    /*0x25*/ u8 unk25;
    /*0x26*/ u8 unk26;
    u8 unk27;
    /*0x28*/ u16 unk28;
    /*0x2A*/ u16 unk2A;
    /*0x2C*/ u8 unk2C;
    u8 unk2D[3];
}; /* size = 0x30 */

struct NameEntryDefault
{
    /*0x00*/ char name[8];
    /*0x08*/ s32 unk8;
    /*0x0C*/ s32 unkC;
    /*0x10*/ s32 unk10;
}; /* size = 0x14 */

struct NameEntryCont
{
    /*0x00*/ s32 score;
    /*0x04*/ s32 floor;
    /*0x08*/ s8 course;
}; /* size = 0xC */

struct NameEntrySlot
{
    /*0x00*/ s32 unk0;
    /*0x04*/ u8 unk4;
    /*0x08*/ void *unk8;
}; /* size = 0xC */

struct NameEntryCursor
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ s8 unk4;
    /*0x05*/ s8 unk5;
    /*0x06*/ s8 unk6;
    /*0x07*/ s8 unk7;
    /*0x08*/ s8 unk8;
    /*0x0C*/ s32 unkC;
    /*0x10*/ s32 unk10;
    /*0x14*/ u8 unk14;
}; /* size = 0x18 */

struct NameEntrySys
{
    /*0x00*/ s8 unk0;
    u8 unk1[3];
    /*0x04*/ s32 unk4;
    /*0x08*/ s8 unk8;
    /*0x09*/ s8 unk9;
    /*0x0A*/ s8 rank;
    u8 unkB;
    /*0x0C*/ s32 score;
    /*0x10*/ u8 unk10;
    /*0x11*/ u8 unk11;
    /*0x12*/ u8 unk12;
    u8 unk13;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ s8 floor;
    /*0x19*/ u8 name[7];
    /*0x20*/ s32 unk20[6];
    /*0x38*/ s16 unk38;
    /*0x3A*/ s8 unk3A;
    /*0x3B*/ s8 unk3B;
    u8 unk3C;
    /*0x3D*/ s8 course;
    /*0x3E*/ s16 unk3E;
    /*0x40*/ u32 unk40;
    /*0x44*/ u8 unk44;
    /*0x45*/ s8 unk45;
    u8 unk46[2];
    /*0x48*/ struct Ape *ape;
    /*0x4C*/ s32 apeType;
    /*0x50*/ s8 unk50;
    /*0x51*/ u8 unk51;
    /*0x52*/ s8 unk52;
    s8 unk53;
    /*0x54*/ u32 unk54;
    s8 unk58[4];
}; /* size = 0x5C */

struct NameEntryExtra
{
    /*0x00*/ char name[8];
    /*0x08*/ s32 score;
    u8 unkC[8];
    /*0x14*/ s32 apeType;
    u8 unk18[8];
    /*0x20*/ s32 unk20;
    /*0x24*/ u8 unk24;
    /*0x25*/ u8 unk25;
    /*0x26*/ u8 unk26;
    u8 unk27;
    /*0x28*/ u16 unk28;
    /*0x2A*/ u16 unk2A;
    /*0x2C*/ u8 unk2C;
    u8 unk2D[3];
}; /* size = 0x30 */

extern s32 lbl_80198B38;
extern u32 g_font_char_var_3;
extern u8 storymode_save_files[];
void fn_1_12264C(struct Ball *ball);
void fn_1_36140(void);
void fn_1_11E3F8(void);
struct NETimeRecord
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 unk7;
    /*0x08*/ u8 minutes;
    /*0x09*/ u8 seconds;
    /*0x0A*/ u8 centis;
    /*0x0B*/ u8 unkB;
}; /* size = 0xC */

struct NEScoreRecord
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 unk7;
    /*0x08*/ s32 score;
    /*0x0C*/ s32 unkC;
}; /* size = 0x10 */

struct NEShortRecord
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 score;
}; /* size = 0xA */

struct NEPairRecord
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 a;
    /*0x0A*/ u16 b;
}; /* size = 0xC */

struct NETimeRecord2
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 unk7;
    /*0x08*/ u8 minutes;
    /*0x09*/ u8 seconds;
    /*0x0A*/ u8 centis;
}; /* size = 0xB */

struct NEScoreRecord2
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 unk7;
    /*0x08*/ s32 score;
}; /* size = 0xC */

int fn_1_13DE08(int a, struct NETimeRecord *rec, int c, int d);
int fn_1_13E17C(int a, struct NETimeRecord *rec, int c, int d);
int fn_1_13E3BC(int a, struct NEScoreRecord *rec, int c, int d);
int fn_1_13E64C(int a, struct NEShortRecord *rec, int c, int d);
int fn_1_13E908(int a, struct NEPairRecord *rec, int c, int d);
int fn_1_13EC0C(int a, struct NETimeRecord2 *rec, int c, int d);
int fn_1_13EF3C(int a, struct NETimeRecord2 *rec, int c, int d);
int fn_1_13F174(int a, struct NEScoreRecord2 *rec, int c, int d);
int fn_1_13F3F4(int a, struct NEScoreRecord *rec, int c, int d);
void fn_1_12342C(void);
void fn_1_123E94(int a);
void fn_1_1240C8(void);
void fn_1_1241E8(void);
void fn_1_1242CC(void);
void fn_1_1243A8(void);
void fn_1_121188(void);
void fn_1_120DC0(void);
void fn_1_120D48(u8 *name);
void fn_1_12325C(void);
void fn_1_122DA8(void);
void fn_1_121800(void);
void fn_1_121EE0(void);
void fn_1_121748(u8 a, int b, int c);
void fn_1_1215FC(void);
void debug_textdraw_set_pos(int x, int y);
struct TextBox
{
    /*0x00*/ int state;
    u8 unk4[0xC - 0x4];
    /*0x0C*/ s16 x;
    /*0x0E*/ s16 y;
    u8 unk10[0x14 - 0x10];
    /*0x14*/ s8 unk14;
    /*0x15*/ s8 lines;
    /*0x16*/ u8 unk16;
    u8 unk17[0x1C - 0x17];
    /*0x1C*/ void (*callback)(struct TextBox *tb);
    u8 unk20[0x34 - 0x20];
}; /* size = 0x34 */

extern s32 locale;
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_add_color(u32 color);
void textdraw_set_mul_color(u32 color);
void textdraw_set_depth(float depth);
void textdraw_set_pos(float x, float y);
void textdraw_printf(char *fmt, ...);
void textdraw_print(char *str);
void textdraw_set_flags(u32 flags);
void textdraw_set_scale(float x, float y);
void set_ui_widescreen_scale_mtx(int width);
void reset_ui_widescreen_scale_mtx(void);
struct Sprite *get_sprite_with_unique_id(int id);
void g_smth_with_playpoint_or_gift_msg(int id, char *msg);
void g_display_playpoint_or_gift_message_child(int a, int b, struct TextBox *tb);
void dest_all_events(void);
void load_stage(int stage);
void event_init(int id);
void load_bmp_by_id(int id);
void fade_screen_to_color(s32 type, u32 color, int duration);
void set_sub_mode_destination(void (*func)(void));
void g_very_similar_to_puts(char *str);
int get_storymode_score(void);
int get_storymode_banana_count(void);
u32 fn_2_8D98(void);
extern struct { u32 unk0; u32 flags; } g_related_to_pause_menu_input;
void fn_1_11BC50(void);
void fn_1_11C580(void);
void g_fade_track_volume(int a, int b);
void *fn_1_5D6F4(char *name);
void fn_1_122FA4(void);
void fn_1_122594(void);
void fn_1_122C60(void);
void ape_assignment(void);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void g_draw_stage(void);
void g_draw_bg(void);
void ord_tbl_draw_nodes(void);
void fn_1_122ABC(struct Ball *ball);
void fn_1_4EAFC(Mtx mtx, int colorIdx, int mode);
void fn_1_122C60(void);
void fn_1_122FA4(void);
struct Ape *g_load_ape_variant0(int chara, int lod, int scene);
void fn_1_1132D0(struct Ape *ape, int b, int c, int d);
void g_ape_anim_body(struct Ape *ape);
void g_smth_with_ape_rotation(struct Ape *ape, Vec *dir);
extern s32 active_monkey_id[];
void draw_ape(struct Ape *ape);
void fn_1_127390(void);
void fn_1_1285E8(void);
void fn_1_12862C(void);
void fn_1_129BDC(void);
void fn_1_126264(void);
void fn_1_1288FC(void);
void fn_1_11C5E4(void);
void destroy_sprite_with_unique_id(int id);
void g_something_with_freeing_chara_heap_ape(struct Ape *ape);
void call_free_bmp_by_id(int grpId);
extern s32 locale;

s8 lbl_1_bss_D5878;
s8 lbl_1_bss_D5879;
s8 lbl_1_bss_D587A;
s8 lbl_1_bss_D587B;
s32 lbl_1_bss_D587C;
s8 lbl_1_bss_D5880;
s8 lbl_1_bss_D5881;
s8 lbl_1_bss_D5882;
s8 lbl_1_bss_D5883;
s8 lbl_1_bss_D5884;
s8 lbl_1_bss_D5885;
s8 lbl_1_bss_D5886;
s8 lbl_1_bss_D5887;
s8 lbl_1_bss_D5888;
struct NameEntryRec lbl_1_bss_D588C[4][5];
struct NameEntryDefault lbl_1_bss_D5ABC[5];
struct NameEntryCont lbl_1_bss_D5B20[4][5];
struct NameEntrySys lbl_1_bss_D5C10;
s8 lbl_1_bss_D5C6C;
s8 lbl_1_bss_D5C6D;
s16 lbl_1_bss_D5C6E;
struct NameEntryData lbl_1_bss_D5C70[5];
struct NameEntryData lbl_1_bss_D5D60;
s8 lbl_1_bss_D5D90;
s8 lbl_1_bss_D5D91;
s16 lbl_1_bss_D5D92;
s32 lbl_1_bss_D5D94;
s8 lbl_1_bss_D5D98[4];
s8 lbl_1_bss_D5D9C;
s8 lbl_1_bss_D5D9D;
s16 lbl_1_bss_D5D9E;
struct NameEntryExtra lbl_1_bss_D5DA0[4];
struct NameEntryExtra lbl_1_bss_D5E60;
struct NameEntrySlot lbl_1_bss_D5E90[5];
void *lbl_1_bss_D5ECC[0x2D];
s32 lbl_1_bss_D5F80[5];
struct NameEntrySlot lbl_1_bss_D5F94;
u8 g_some_gift_menu_flags;
u8 g_nameentry_did_get_top_5;
s16 lbl_1_bss_D5FA2;
s32 lbl_1_bss_D5FA4;
s32 lbl_1_bss_D5FA8[6];
struct NameEntryCursor lbl_1_bss_D5FC0;
s32 lbl_1_bss_D5FD8;
s8 lbl_1_bss_D5FDC;
s8 lbl_1_bss_D5FDD;
s16 lbl_1_bss_D5FDE;
s16 lbl_1_bss_D5FE0[5];
s16 lbl_1_bss_D5FEA;
s32 lbl_1_bss_D5FEC;
struct Ball lbl_1_bss_D5FF0[6];
s32 lbl_1_bss_D6A10[0x3C];
struct Ball lbl_1_bss_D6B00[0x3C];
struct Ape *lbl_1_bss_DD040[6];
s32 lbl_1_bss_DD058[6];
s8 lbl_1_bss_DD070;
u8 lbl_1_bss_DD071;
s16 lbl_1_bss_DD072;
s32 lbl_1_bss_DD074;
s32 lbl_1_bss_DD078;
s32 lbl_1_bss_DD07C[6];
s16 lbl_1_bss_DD094[0x3C];
s32 lbl_1_bss_DD10C;
s32 lbl_1_bss_DD110;
s32 lbl_1_bss_DD114;
u8 lbl_1_bss_DD118[0xC];
struct NameEntryData lbl_1_bss_DD124[5];
struct NameEntryData lbl_1_bss_DD214[5];
struct NameEntryData lbl_1_bss_DD304[5];
struct NameEntryData lbl_1_bss_DD3F4[5];
u8 lbl_1_bss_DD4E4[4];

FORCE_BSS_ORDER(lbl_1_bss_D5878)
FORCE_BSS_ORDER(lbl_1_bss_D5879)
FORCE_BSS_ORDER(lbl_1_bss_D587A)
FORCE_BSS_ORDER(lbl_1_bss_D587B)
FORCE_BSS_ORDER(lbl_1_bss_D587C)
FORCE_BSS_ORDER(lbl_1_bss_D5880)
FORCE_BSS_ORDER(lbl_1_bss_D5881)
FORCE_BSS_ORDER(lbl_1_bss_D5882)
FORCE_BSS_ORDER(lbl_1_bss_D5883)
FORCE_BSS_ORDER(lbl_1_bss_D5884)
FORCE_BSS_ORDER(lbl_1_bss_D5885)
FORCE_BSS_ORDER(lbl_1_bss_D5886)
FORCE_BSS_ORDER(lbl_1_bss_D5887)
FORCE_BSS_ORDER(lbl_1_bss_D5888)
FORCE_BSS_ORDER(lbl_1_bss_D588C)
FORCE_BSS_ORDER(lbl_1_bss_D5ABC)
FORCE_BSS_ORDER(lbl_1_bss_D5B20)
FORCE_BSS_ORDER(lbl_1_bss_D5C10)
FORCE_BSS_ORDER(lbl_1_bss_D5C6C)
FORCE_BSS_ORDER(lbl_1_bss_D5C6D)
FORCE_BSS_ORDER(lbl_1_bss_D5C6E)
FORCE_BSS_ORDER(lbl_1_bss_D5C70)
FORCE_BSS_ORDER(lbl_1_bss_D5D60)
FORCE_BSS_ORDER(lbl_1_bss_D5D90)
FORCE_BSS_ORDER(lbl_1_bss_D5D91)
FORCE_BSS_ORDER(lbl_1_bss_D5D92)
FORCE_BSS_ORDER(lbl_1_bss_D5D94)
FORCE_BSS_ORDER(lbl_1_bss_D5D98)
FORCE_BSS_ORDER(lbl_1_bss_D5D9C)
FORCE_BSS_ORDER(lbl_1_bss_D5D9D)
FORCE_BSS_ORDER(lbl_1_bss_D5D9E)
FORCE_BSS_ORDER(lbl_1_bss_D5DA0)
FORCE_BSS_ORDER(lbl_1_bss_D5E60)
FORCE_BSS_ORDER(lbl_1_bss_D5E90)
FORCE_BSS_ORDER(lbl_1_bss_D5ECC)
FORCE_BSS_ORDER(lbl_1_bss_D5F80)
FORCE_BSS_ORDER(lbl_1_bss_D5F94)
FORCE_BSS_ORDER(g_some_gift_menu_flags)
FORCE_BSS_ORDER(g_nameentry_did_get_top_5)
FORCE_BSS_ORDER(lbl_1_bss_D5FA2)
FORCE_BSS_ORDER(lbl_1_bss_D5FA4)
FORCE_BSS_ORDER(lbl_1_bss_D5FA8)
FORCE_BSS_ORDER(lbl_1_bss_D5FC0)
FORCE_BSS_ORDER(lbl_1_bss_D5FD8)
FORCE_BSS_ORDER(lbl_1_bss_D5FDC)
FORCE_BSS_ORDER(lbl_1_bss_D5FDD)
FORCE_BSS_ORDER(lbl_1_bss_D5FDE)
FORCE_BSS_ORDER(lbl_1_bss_D5FE0)
FORCE_BSS_ORDER(lbl_1_bss_D5FEA)
FORCE_BSS_ORDER(lbl_1_bss_D5FEC)
FORCE_BSS_ORDER(lbl_1_bss_D5FF0)
FORCE_BSS_ORDER(lbl_1_bss_D6A10)
FORCE_BSS_ORDER(lbl_1_bss_D6B00)
FORCE_BSS_ORDER(lbl_1_bss_DD040)
FORCE_BSS_ORDER(lbl_1_bss_DD058)
FORCE_BSS_ORDER(lbl_1_bss_DD070)
FORCE_BSS_ORDER(lbl_1_bss_DD071)
FORCE_BSS_ORDER(lbl_1_bss_DD072)
FORCE_BSS_ORDER(lbl_1_bss_DD074)
FORCE_BSS_ORDER(lbl_1_bss_DD078)
FORCE_BSS_ORDER(lbl_1_bss_DD07C)
FORCE_BSS_ORDER(lbl_1_bss_DD094)
FORCE_BSS_ORDER(lbl_1_bss_DD10C)
FORCE_BSS_ORDER(lbl_1_bss_DD110)
FORCE_BSS_ORDER(lbl_1_bss_DD114)
FORCE_BSS_ORDER(lbl_1_bss_DD118)
FORCE_BSS_ORDER(lbl_1_bss_DD124)
FORCE_BSS_ORDER(lbl_1_bss_DD214)
FORCE_BSS_ORDER(lbl_1_bss_DD304)
FORCE_BSS_ORDER(lbl_1_bss_DD3F4)
FORCE_BSS_ORDER(lbl_1_bss_DD4E4)

const u16 lbl_1_rodata_65690[][6] = {
    { 0x0F, 0x0F, 0x46, 0x48, 0x48, 0x48 },
    { 0x20, 0x20, 0x2F, 0x31, 0x31, 0x31 },
    { 0x33, 0x33, 0x41, 0x41, 0x41, 0x41 },
    { 0x03, 0x5B, 0x5C, 0x5D, 0x5D, 0x5C },
};
const Vec lbl_1_rodata_656C0[] = {
    { -2.5f, -2.15f, 1.5f },
    { -1.5f, -2.15f, 1.5f },
    { -0.5f, -2.15f, 1.5f },
    { 0.5f, -2.15f, 1.5f },
    { 1.5f, -2.15f, 1.5f },
    { 2.5f, -2.15f, 1.5f },
};
const Vec lbl_1_rodata_65708[] = {
    { -2.5f, -1.2500001f, 1.5f },
    { -1.5f, -1.2500001f, 1.5f },
    { -0.5f, -1.2500001f, 1.5f },
    { 0.5f, -1.2500001f, 1.5f },
    { 1.5f, -1.2500001f, 1.5f },
    { 2.5f, -1.2500001f, 1.5f },
};
const u32 lbl_1_rodata_65750[] = { 0x000F000F, 0x00480048, 0x00480048 };
const u32 lbl_1_rodata_6575C[] = { 0x00200020, 0x00310031, 0x00310031 };
const u32 lbl_1_rodata_65768[] = { 0x00330033, 0x00410041, 0x00410041 };
const u32 lbl_1_rodata_65774[] = { 0x00030003, 0x005D005D, 0x005D005D };

void fn_1_12A994();
void fn_1_12AA00();
void fn_1_12AA6C();
void fn_1_12AAD8();
void fn_1_12AB44();
void fn_1_12ABB0();
void fn_1_12AC1C();

extern u32 lbl_1_data_6B4A0[];
extern char lbl_1_data_6B610[];
extern char lbl_1_data_6B618[];
extern char lbl_1_data_6B620[];
extern char lbl_1_data_6B628[];
extern void *lbl_1_data_6B630[];
extern s32 lbl_1_data_6B640[][6];
extern char lbl_1_data_6B6A0[];
extern char lbl_1_data_6B6A4[];
extern char lbl_1_data_6B6A8[];
extern char lbl_1_data_6B6AC[];
extern char lbl_1_data_6B6B0[];
extern void *lbl_1_data_6B6B4[];
extern char lbl_1_data_6B910[];
extern s32 lbl_1_data_6BAE0[];
extern u32 lbl_1_data_6BAE4[];
extern Vec lbl_1_data_6BDCC;
extern Vec lbl_1_data_6BDD8;
extern Vec lbl_1_data_6BDE4;
extern Vec lbl_1_data_6BDF0;
extern Vec lbl_1_data_6BDFC;
extern Vec lbl_1_data_6BE08;
extern Vec lbl_1_data_6BE14;
extern Vec lbl_1_data_6BE20;
extern Vec lbl_1_data_6BE2C;
extern Vec lbl_1_data_6BE38;
extern Vec lbl_1_data_6BE44;
extern Vec lbl_1_data_6BE50;
extern Vec lbl_1_data_6BE5C;
extern Vec lbl_1_data_6BE68;
extern Vec lbl_1_data_6BE74;
extern u32 lbl_1_data_6BE80[];
extern char lbl_1_data_6BE8C[];
extern char lbl_1_data_6BE98[];
extern char lbl_1_data_6BEA4[];
extern char lbl_1_data_6BEB0[];
extern void *lbl_1_data_6BEC0[];
extern Vec lbl_1_data_6BED0;
extern Vec lbl_1_data_6BEDC;
extern char lbl_1_data_6BEE8[];
extern char lbl_1_data_6BEFC[];
extern char lbl_1_data_6BF10[];
extern char lbl_1_data_6BF1C[];
extern char lbl_1_data_6BF2C[];
extern char lbl_1_data_6BF38[];
extern char lbl_1_data_6BF48[];
extern char lbl_1_data_6BF58[];
extern char lbl_1_data_6BF68[];
extern void *lbl_1_data_6BF78[];
extern u16 lbl_1_data_6BFE0[];
extern char lbl_1_data_6C03C[];
extern char lbl_1_data_6C06C[];
extern char lbl_1_data_6C078[];
extern char lbl_1_data_6C084[];
extern char lbl_1_data_6C08C[];
extern char lbl_1_data_6C094[];
extern char lbl_1_data_6C09C[];
extern char lbl_1_data_6C0A4[];
extern char lbl_1_data_6C0AC[];
extern char lbl_1_data_6C0B4[];
extern char lbl_1_data_6C0BC[];
extern char lbl_1_data_6C0C4[];
extern char lbl_1_data_6C0CC[];
extern char lbl_1_data_6C0D4[];
extern char lbl_1_data_6C0DC[];
extern char lbl_1_data_6C0E4[];
extern char lbl_1_data_6C0EC[];
extern char lbl_1_data_6C0F4[];
extern char lbl_1_data_6C0FC[];
extern char lbl_1_data_6C104[];
extern char lbl_1_data_6C10C[];
extern char lbl_1_data_6C114[];
extern char lbl_1_data_6C11C[];
extern char lbl_1_data_6C124[];
extern char lbl_1_data_6C12C[];
extern char lbl_1_data_6C134[];
extern char lbl_1_data_6C13C[];
extern char lbl_1_data_6C144[];
extern char lbl_1_data_6C14C[];
extern char lbl_1_data_6C154[];
extern char lbl_1_data_6C15C[];
extern char lbl_1_data_6C164[];
extern char lbl_1_data_6C16C[];
extern char lbl_1_data_6C174[];
extern char lbl_1_data_6C17C[];
extern char lbl_1_data_6C184[];
extern char lbl_1_data_6C18C[];
extern char lbl_1_data_6C194[];
extern char lbl_1_data_6C19C[];
extern char lbl_1_data_6C1A4[];
extern char lbl_1_data_6C1B4[];
extern char lbl_1_data_6C1C0[];
extern char lbl_1_data_6C1CC[];
extern char lbl_1_data_6C1D8[];
extern char lbl_1_data_6C1E4[];
extern char lbl_1_data_6C1F0[];
extern char *lbl_1_data_6C200[];
struct NERankA
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 ape;
    /*0x08*/ u8 unk8;
    /*0x09*/ u8 unk9;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
}; /* size = 0xC */

struct NERankB
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 ape;
    /*0x08*/ s32 score;
    /*0x0C*/ u8 unkC;
}; /* size = 0x10 */

struct NERankC
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 ape;
    /*0x08*/ u16 unk8;
}; /* size = 0xA */

struct NERankD
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 ape;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
}; /* size = 0xC */

struct NERankE
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 ape;
    /*0x08*/ u8 unk8;
    /*0x09*/ u8 unk9;
    /*0x0A*/ u8 unkA;
}; /* size = 0xB */

struct NERankF
{
    /*0x00*/ char name[7];
    /*0x07*/ u8 ape;
    /*0x08*/ s32 score;
}; /* size = 0xC */

struct NEA { char name[8]; s16 a; };
struct NEB { char name[8]; s16 a; s16 b; };
struct NEC { char name[8]; u8 a; u8 b; u8 c; };
struct NERankTables { struct NEA a[7][5]; struct NEB b[5]; struct NEC c[3][6]; };
extern struct NERankTables lbl_1_data_6C8E0;
struct NERankEntry
{
    /*0x00*/ void *fn;
    /*0x04*/ void *ptr;
    /*0x08*/ void *n;
}; /* size = 0xC */

extern struct NERankEntry lbl_1_data_6C2B4[];
extern struct NETimeRecord lbl_1_data_6C440[];
extern struct NETimeRecord lbl_1_data_6C47C;
extern struct NETimeRecord lbl_1_data_6C488[];
extern struct NETimeRecord lbl_1_data_6C4C4;
extern struct NETimeRecord lbl_1_data_6C4D0[];
extern struct NETimeRecord lbl_1_data_6C50C;
extern struct NETimeRecord lbl_1_data_6C518[];
extern struct NETimeRecord lbl_1_data_6C554;
extern struct NETimeRecord lbl_1_data_6C560[];
extern struct NETimeRecord lbl_1_data_6C59C;
extern struct NETimeRecord lbl_1_data_6C5A8[];
extern struct NETimeRecord lbl_1_data_6C5E4;
extern struct NETimeRecord lbl_1_data_6C5F0[];
extern struct NETimeRecord lbl_1_data_6C638[];
extern struct NETimeRecord lbl_1_data_6C680[];
extern struct NETimeRecord lbl_1_data_6C6C8[];
extern struct NETimeRecord lbl_1_data_6C710[];
extern struct NETimeRecord lbl_1_data_6C758[];
extern struct NERankB lbl_1_data_6C7A0[];
extern struct NERankB lbl_1_data_6C7F0[];
extern struct NERankB lbl_1_data_6C840[];
extern struct NERankB lbl_1_data_6C890[];
extern struct NERankF lbl_1_data_6CB40[];
extern struct NERankF lbl_1_data_6CB7C[];
extern struct NERankF lbl_1_data_6CBB8[];
extern struct NERankB lbl_1_data_6CBF4[];
extern struct NERankB lbl_1_data_6CC44[];
extern struct NERankB lbl_1_data_6CC94[];
extern void *lbl_1_data_6CCE4[];
extern void *lbl_1_data_6CCFC[];
extern u32 lbl_1_data_6CD08[];
extern u32 lbl_1_data_6CD58[];
extern u32 lbl_1_data_6CDA8[];
extern u32 lbl_1_data_6CDF8[];
extern u32 lbl_1_data_6CE48[];
extern u32 lbl_1_data_6CE98[];
extern u32 lbl_1_data_6CEE8[];
extern u32 lbl_1_data_6CF38[];
extern u32 lbl_1_data_6CF88[];
extern u32 lbl_1_data_6CFD8[];
extern u32 lbl_1_data_6D028[];
extern u32 lbl_1_data_6D078[];
extern u32 lbl_1_data_6D0C8[];
extern u32 lbl_1_data_6D118[];
extern u32 lbl_1_data_6D178[];
extern u32 lbl_1_data_6D1C8[];
extern u32 lbl_1_data_6D218[];
extern u32 lbl_1_data_6D268[];
extern u32 lbl_1_data_6D2B8[];
extern struct SpriteDrawRequest lbl_1_data_6D54C;

u32 lbl_1_data_6B4A0[] = { 0xFE000102, 0x3040506, 0x708090A, 0xB0CFE0D, 0xE0FFF00, 0x10203, 0x4050607, 0x8090A0B, 0xC0D0E0F, 0xFF000000, 0x31000000, 0x0, 0x3800B400, 0x10000, 0x1000000, 0x20000, 0x10004C00, 0x30000, 0x1000000, 0x40000, 0x18000000, 0x90000, 0x40000000, 0x0, 0x1000000, 0xB0000, 0x18000000, 0xC0000, 0x18000000, 0x50000, 0x40000000, 0x0, 0x1000000, 0x70000, 0x18000000, 0x80000, 0x3800B400, 0xD0000, 0x40000000, 0x0, 0x18000000, 0x140000, 0x40000000, 0x0, 0x1000000, 0x160000, 0x18000000, 0x170000, 0x18000000, 0x180000, 0x18000000, 0xF0000, 0x40000000, 0x0, 0x1000000, 0x110000, 0x18000000, 0x120000, 0x18000000, 0x130000, 0x18000000, 0x0, 0x18000000, 0x10000, 0x18000000, 0x20000, 0x18000000, 0x30000, 0x18000000, 0x40000, 0x18000000, 0x50000, 0x18000000, 0x60000, 0x18000000, 0x70000, 0x18000000, 0x80000, 0x18000000, 0x90000, 0x18000000, 0xA0000, 0x18000000, 0xB0000, 0x18000000, 0xC0000, 0x18000000, 0xD0000, 0x18000000, 0xE0000, 0x18000000, 0xF0000 };
char lbl_1_data_6B610[] = " AIAI ";
char lbl_1_data_6B618[] = "MEEMEE";
char lbl_1_data_6B620[] = " BABY ";
char lbl_1_data_6B628[] = "GONGON";
void *lbl_1_data_6B630[] = { lbl_1_data_6B610, lbl_1_data_6B618, lbl_1_data_6B620, lbl_1_data_6B628 };
s32 lbl_1_data_6B640[][6] = {
    { 0x1, 0x2, 0xa, 0x2, 0xa, 0x1 },
    { 0xe, 0x6, 0x6, 0xe, 0x6, 0x6 },
    { 0x1, 0x3, 0x2, 0x3, 0x1a, 0x1 },
    { 0x8, 0x10, 0xf, 0x8, 0x10, 0xf },
};
char lbl_1_data_6B6A0[] = "SEX";
char lbl_1_data_6B6A4[] = "XXX";
char lbl_1_data_6B6A8[] = "XX ";
char lbl_1_data_6B6AC[] = "XX";
char lbl_1_data_6B6B0[] = " XX";
void *lbl_1_data_6B6B4[] = { (void *)0x58205800, lbl_1_data_6B6A0, lbl_1_data_6B6A4, lbl_1_data_6B6A8, lbl_1_data_6B6AC, lbl_1_data_6B6B0, lbl_1_data_6B6B4, (void *)0x4E455F52, (void *)0x45414459, (void *)0x5F494E49, (void *)0x54000000, (void *)0x0, (void *)0x0, (void *)0x4E455F52, (void *)0x45414459, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x4E455F49, (void *)0x4E505554, (void *)0x5F494E49, (void *)0x54000000, (void *)0x0, (void *)0x0, (void *)0x4E455F49, (void *)0x4E505554, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x4E455F50, (void *)0x524F4849, (void *)0x42495445, (void *)0x445F4348, (void *)0x45434B00, (void *)0x0, (void *)0x4E455F49, (void *)0x4E505554, (void *)0x5F454E44, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x4E455F52, (void *)0x414E4B49, (void *)0x4E470000, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x4E455F45, (void *)0x4E440000, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x53544152, (void *)0x54000000, (void *)0x0, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x43484543, (void *)0x4B000000, (void *)0x0, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x52454144, (void *)0x595F4341, (void *)0x4D534554, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x52454144, (void *)0x595F494E, (void *)0x49540000, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x52454144, (void *)0x59000000, (void *)0x0, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x494E5055, (void *)0x545F4341, (void *)0x4D534554, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x494E5055, (void *)0x545F494E, (void *)0x49540000, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x494E5055, (void *)0x54000000, (void *)0x0, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x50524F48, (void *)0x49424954, (void *)0x45440000, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x4E455854, (void *)0x5F504C41, (void *)0x5945525F, (void *)0x57414954, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x52455345, (void *)0x54000000, (void *)0x0, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x494E5055, (void *)0x545F454E, (void *)0x44000000, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x52414E4B, (void *)0x494E475F, (void *)0x43414D53, (void *)0x45540000, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x52414E4B, (void *)0x494E4700, (void *)0x0, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x53415645, (void *)0x0, (void *)0x0, (void *)0x0, (void *)0x4E455F4D, (void *)0x494E495F, (void *)0x454E4400, (void *)0x0, (void *)0x0, (void *)0x0 };
char lbl_1_data_6B910[] = "BANANA";
char *lbl_1_data_6B918[4][5] = {
    { lbl_1_data_6B610, lbl_1_data_6B618, lbl_1_data_6B620, lbl_1_data_6B628, lbl_1_data_6B910 },
    { lbl_1_data_6B610, lbl_1_data_6B618, lbl_1_data_6B620, lbl_1_data_6B628, lbl_1_data_6B910 },
    { lbl_1_data_6B610, lbl_1_data_6B618, lbl_1_data_6B620, lbl_1_data_6B628, lbl_1_data_6B910 },
    { lbl_1_data_6B610, lbl_1_data_6B618, lbl_1_data_6B620, lbl_1_data_6B628, lbl_1_data_6B910 },
};
s32 lbl_1_data_6B968[4][5] = {
    { 50000, 40000, 30000, 20000, 10000 },
    { 50000, 40000, 30000, 20000, 10000 },
    { 50000, 40000, 30000, 20000, 10000 },
    { 50000, 40000, 30000, 20000, 10000 },
};
s32 lbl_1_data_6B9B8[4][5] = {
    { 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255 },
};
s32 lbl_1_data_6BA08[4][5] = {
    { 0, 1, 2, 3, 0 },
    { 0, 1, 2, 3, 0 },
    { 0, 1, 2, 3, 0 },
    { 0, 1, 2, 3, 0 },
};
struct NameEntryData *lbl_1_data_6BA58[4] = { lbl_1_bss_DD3F4, lbl_1_bss_DD304, lbl_1_bss_DD214, lbl_1_bss_DD124 };
struct NameEntryDefault lbl_1_data_6BA68[5] = {
    { " AIAI ", 50000, 50, 324000 },
    { " AIAI ", 40000, 40, 288000 },
    { " AIAI ", 30000, 30, 252000 },
    { " AIAI ", 20000, 20, 216000 },
    { " AIAI ", 10000, 10, 180000 },
};
float lbl_1_data_6BACC[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
s32 lbl_1_data_6BAE0[] = { 0x1 };
u32 lbl_1_data_6BAE4[] = { 0x3F18C34C, 0x4096745E, 0xC07F2531, 0x3F71F362, 0x3FEDD47C, 0xC0802F6A, 0x3FFDADB0, 0x40B79204, 0xC06E8C48, 0x4054D0D5, 0x40CBFB9E, 0xC086AF23, 0x3F8E242D, 0x408855A4, 0xBFE8DA90, 0x400E5575, 0x40974904, 0xC009EAC4, 0x3F939FED, 0x40B4717A, 0xC010F594, 0xC005E126, 0x406DE558, 0xC084F2DA, 0x4005C055, 0x4081326C, 0xC08CCE89, 0x3FADCA2A, 0x408E1264, 0xC0B9B680, 0x40315554, 0x40BE9115, 0xBFD9C7D6, 0x3F71159C, 0x403D1937, 0xC076D6F1, 0x40370FD8, 0x409F36D8, 0xC0B6EFD8, 0x3EED3049, 0x40B9C1EE, 0xBF9BEA03, 0x401D17B9, 0x40BBBDB8, 0xC0D35BAB, 0xBED24DB1, 0x40F16579, 0xC08DC272, 0x3FBB65FE, 0x40F28DAA, 0xC0B38052, 0xBF38BFC2, 0x40D50976, 0xC0DB4340, 0xBFD30603, 0x409D146E, 0xC0BBA1F3, 0xBF738690, 0x3ED1CF78, 0xC0800000, 0x4049F25E, 0x40A36E0F, 0xC0825636, 0xC0112F1F, 0x4098A9BB, 0xC04A7A53, 0xBFD79B4E, 0x409C819D, 0xBFAA56AC, 0x400C3598, 0x40E82416, 0xC089C137, 0xC0628E7E, 0x40BB252F, 0xC08A993F, 0x3D8A17B1, 0x40614D72, 0xC097E365, 0xC00D17CA, 0x410B49C2, 0xC0992BD4, 0xBF7BFEF4, 0x40DD328B, 0xBF5E0360, 0xBE913405, 0x4091463D, 0xBFA58833, 0x3E297D4A, 0x40E6D3EF, 0xBF89476B, 0xC026D91F, 0x40F79C8D, 0xC0B07734, 0x3DE1D191, 0x41105E43, 0xC0CE43C8, 0xC0451B76, 0x40CF5E31, 0xC0BDB882, 0xBEFA9C56, 0x40BEBF5B, 0xC08B609E, 0xBFB42492, 0x40FF5126, 0xC06D995B, 0x3F036BD3, 0x40CC050A, 0xC0ABBE29, 0xBFF7D884, 0x40DB2640, 0xC0B44CFD, 0xBE9C47E5, 0x4093B60B, 0xC0B29186, 0x3FBB4BB6, 0x4089C930, 0xC0486949, 0xC03E57D6, 0x40C922B1, 0xBFDB98D0, 0x3FD6F0D8, 0x40BF5FA6, 0xC0A73290, 0x3F6F863C, 0x410CF05D, 0xC08C750E, 0x3E9F87AD, 0x40A861D2, 0xC0D360AF, 0x3F706467, 0x40F220CB, 0xC08192C9, 0xC03BFEA0, 0x40F330A7, 0xC040716D, 0xBEE645E5, 0x40EA3B71, 0xC0B280DC, 0xC001531A, 0x40BD2640, 0xC0DBB2D9, 0x3F695DD5, 0x40CFFFF8, 0xBDDE78E2, 0xC047033A, 0x40B69F1D, 0xC049A145, 0xBEA07400, 0x409B563D, 0xC01F5E0B, 0xC03B65ED, 0x40910232, 0xC0AE84DC, 0xBF6F0D20, 0x405B15DF, 0xC05C1038, 0xBFCEA22B, 0x40D02040, 0xC0768B15, 0xBF9C9824, 0x4100E6E7, 0xC0C5C0D3, 0xBE99A8CE, 0x402B83D3, 0xC07FE0F4, 0x3F77C1DF, 0x40D58EB0, 0xC0D9A82A, 0x3E966213, 0x3F3FCE74, 0xC0800000, 0xBFE029E1, 0x40AFF586, 0xC0199620, 0xBF5E5AAB, 0x40B1AE0E, 0xBEC1C21A, 0x3E9A0104, 0x4052CF29, 0xC01043FE };
float lbl_1_data_6BDB4[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
Vec lbl_1_data_6BDCC = { 0.0f, 6.5f, -4.0f };
Vec lbl_1_data_6BDD8 = { -22.0f, 9.0f, 0.2f };
Vec lbl_1_data_6BDE4 = { 2.0f, -0.1f, -5.5f };
Vec lbl_1_data_6BDF0 = { 0.0f, 4.6f, 14.0f };
Vec lbl_1_data_6BDFC = { 0.0f, 3.5f, -6.0f };
Vec lbl_1_data_6BE08 = { 0.0f, 1.8f, 3.0f };
Vec lbl_1_data_6BE14 = { 0.0f, 0.8f, 0.0f };
Vec lbl_1_data_6BE20 = { 0.8f, 0.8f, 3.3f };
Vec lbl_1_data_6BE2C = { 0.8f, 0.7f, 0.4f };
Vec lbl_1_data_6BE38 = { 0.0f, 0.16f, 8.3f };
Vec lbl_1_data_6BE44 = { 0.0f, 0.0f, 3.0f };
Vec lbl_1_data_6BE50 = { 0.0f, -1.7f, 5.5f };
Vec lbl_1_data_6BE5C = { 0.0f, -1.7f, -1.7f };
Vec lbl_1_data_6BE68 = { 5.0f, 10.0f, 12.0f };
Vec lbl_1_data_6BE74 = { 0.0f, 6.0f, -2.0f };
u32 lbl_1_data_6BE80[] = { 0x0, 0x40000000, 0xC0900000 };
char lbl_1_data_6BE8C[] = "NE_BL_WAIT";
char lbl_1_data_6BE98[] = "NE_BL_DEBUT";
char lbl_1_data_6BEA4[] = "NE_BL_DISP";
char lbl_1_data_6BEB0[] = "NE_BL_FALLOUT";
void *lbl_1_data_6BEC0[] = { lbl_1_data_6BE8C, lbl_1_data_6BE98, lbl_1_data_6BEA4, lbl_1_data_6BEB0 };
Vec lbl_1_data_6BED0 = { 0.0f, 1.0f, -0.5f };
Vec lbl_1_data_6BEDC = { 0.0f, 1.8f, -0.5f };
char lbl_1_data_6BEE8[] = "NE_APE_CALL_INIT";
char lbl_1_data_6BEFC[] = "NE_APE_CALL_WAIT";
char lbl_1_data_6BF10[] = "NE_APE_CALL";
char lbl_1_data_6BF1C[] = "NE_APE_TAKE_OFF";
char lbl_1_data_6BF2C[] = "NE_APE_FALL";
char lbl_1_data_6BF38[] = "NE_APE_LANDING";
char lbl_1_data_6BF48[] = "NE_APE_DESERT";
char lbl_1_data_6BF58[] = "NE_APE_IN_BALL";
char lbl_1_data_6BF68[] = "NE_APE_ON_BALL";
void *lbl_1_data_6BF78[] = { (void *)0x4E455F41, (void *)0x50455F52, (void *)0x45534554, (void *)0x0, lbl_1_data_6BEE8, lbl_1_data_6BEFC, lbl_1_data_6BF10, lbl_1_data_6BF1C, lbl_1_data_6BF2C, lbl_1_data_6BF38, lbl_1_data_6BF48, lbl_1_data_6BF58, lbl_1_data_6BF68, lbl_1_data_6BF78, (void *)0x204142, (void *)0x43444546, (void *)0x4748494A, (void *)0x4B4C4D4E, (void *)0x4F505152, (void *)0x53545556, (void *)0x5758595A, (void *)0x30313233, (void *)0x34353637, (void *)0x38392126, (void *)0x2D2E3F40, (void *)0x8000000 };
/* letter ball model per wheel position (u16: read as u16 from u32 words, each pair came
 * out swapped on little endian and the balls showed the next letter) */
u16 lbl_1_data_6BFE0[] = { 0x3C, 0x0, 0x32, 0x37, 0x38, 0x39, 0x3A, 0x3E, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x49, 0x4B, 0x4D, 0x4F, 0x51, 0x52, 0x55, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x4C, 0x57, 0x56, 0x40, 0x3F, 0x54, 0x53, 0x3B, 0x4A, 0x3D, 0x33, 0x48, 0x4E, 0x50, 0x36, 0x37, 0x0 };
/* name entry letters by wheel position (bytes: as u32 words they came out byte-reversed
 * on little endian, so the wheel entered the wrong letters) */
char lbl_1_data_6C03C[] = { 0, ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '&', '-', '.', '?', '@', 0x08, 0, 0, 0 };
char lbl_1_data_6C06C[] = "NAME_END";
char lbl_1_data_6C078[] = "NAME_SPACE";
char lbl_1_data_6C084[] = "NAME_A";
char lbl_1_data_6C08C[] = "NAME_B";
char lbl_1_data_6C094[] = "NAME_C";
char lbl_1_data_6C09C[] = "NAME_D";
char lbl_1_data_6C0A4[] = "NAME_E";
char lbl_1_data_6C0AC[] = "NAME_F";
char lbl_1_data_6C0B4[] = "NAME_G";
char lbl_1_data_6C0BC[] = "NAME_H";
char lbl_1_data_6C0C4[] = "NAME_I";
char lbl_1_data_6C0CC[] = "NAME_J";
char lbl_1_data_6C0D4[] = "NAME_K";
char lbl_1_data_6C0DC[] = "NAME_L";
char lbl_1_data_6C0E4[] = "NAME_M";
char lbl_1_data_6C0EC[] = "NAME_N";
char lbl_1_data_6C0F4[] = "NAME_O";
char lbl_1_data_6C0FC[] = "NAME_P";
char lbl_1_data_6C104[] = "NAME_Q";
char lbl_1_data_6C10C[] = "NAME_R";
char lbl_1_data_6C114[] = "NAME_S";
char lbl_1_data_6C11C[] = "NAME_T";
char lbl_1_data_6C124[] = "NAME_U";
char lbl_1_data_6C12C[] = "NAME_V";
char lbl_1_data_6C134[] = "NAME_W";
char lbl_1_data_6C13C[] = "NAME_X";
char lbl_1_data_6C144[] = "NAME_Y";
char lbl_1_data_6C14C[] = "NAME_Z";
char lbl_1_data_6C154[] = "NAME0";
char lbl_1_data_6C15C[] = "NAME1";
char lbl_1_data_6C164[] = "NAME2";
char lbl_1_data_6C16C[] = "NAME3";
char lbl_1_data_6C174[] = "NAME4";
char lbl_1_data_6C17C[] = "NAME5";
char lbl_1_data_6C184[] = "NAME6";
char lbl_1_data_6C18C[] = "NAME7";
char lbl_1_data_6C194[] = "NAME8";
char lbl_1_data_6C19C[] = "NAME9";
char lbl_1_data_6C1A4[] = "NAME_BIKKURI";
char lbl_1_data_6C1B4[] = "NAME_AND";
char lbl_1_data_6C1C0[] = "NAME_MINUS";
char lbl_1_data_6C1CC[] = "NAME_DOT";
char lbl_1_data_6C1D8[] = "NAME_HATENA";
char lbl_1_data_6C1E4[] = "NAME_ATMARK";
char lbl_1_data_6C1F0[] = "NAME_BACKSPACE";
char *lbl_1_data_6C200[] = {
    lbl_1_data_6C06C,
    lbl_1_data_6C078,
    lbl_1_data_6C084,
    lbl_1_data_6C08C,
    lbl_1_data_6C094,
    lbl_1_data_6C09C,
    lbl_1_data_6C0A4,
    lbl_1_data_6C0AC,
    lbl_1_data_6C0B4,
    lbl_1_data_6C0BC,
    lbl_1_data_6C0C4,
    lbl_1_data_6C0CC,
    lbl_1_data_6C0D4,
    lbl_1_data_6C0DC,
    lbl_1_data_6C0E4,
    lbl_1_data_6C0EC,
    lbl_1_data_6C0F4,
    lbl_1_data_6C0FC,
    lbl_1_data_6C104,
    lbl_1_data_6C10C,
    lbl_1_data_6C114,
    lbl_1_data_6C11C,
    lbl_1_data_6C124,
    lbl_1_data_6C12C,
    lbl_1_data_6C134,
    lbl_1_data_6C13C,
    lbl_1_data_6C144,
    lbl_1_data_6C14C,
    lbl_1_data_6C154,
    lbl_1_data_6C15C,
    lbl_1_data_6C164,
    lbl_1_data_6C16C,
    lbl_1_data_6C174,
    lbl_1_data_6C17C,
    lbl_1_data_6C184,
    lbl_1_data_6C18C,
    lbl_1_data_6C194,
    lbl_1_data_6C19C,
    lbl_1_data_6C1A4,
    lbl_1_data_6C1B4,
    lbl_1_data_6C1C0,
    lbl_1_data_6C1CC,
    lbl_1_data_6C1D8,
    lbl_1_data_6C1E4,
    lbl_1_data_6C1F0,
};
struct NERankEntry lbl_1_data_6C2B4[] = { fn_1_12A994, lbl_1_data_6C440, (void *)0xC, fn_1_12A994, lbl_1_data_6C488, (void *)0xC, fn_1_12A994, lbl_1_data_6C4D0, (void *)0xC, fn_1_12A994, lbl_1_data_6C518, (void *)0xC, fn_1_12A994, lbl_1_data_6C560, (void *)0xC, fn_1_12A994, lbl_1_data_6C5A8, (void *)0xC, fn_1_12A994, lbl_1_data_6C5F0, (void *)0xC, fn_1_12A994, lbl_1_data_6C638, (void *)0xC, fn_1_12A994, lbl_1_data_6C680, (void *)0xC, fn_1_12A994, lbl_1_data_6C6C8, (void *)0xC, fn_1_12A994, lbl_1_data_6C710, (void *)0xC, fn_1_12A994, lbl_1_data_6C758, (void *)0xC, fn_1_12AA00, lbl_1_data_6C7A0, (void *)0x10, fn_1_12AA00, lbl_1_data_6C7F0, (void *)0x10, fn_1_12AA00, lbl_1_data_6C840, (void *)0x10, fn_1_12AA00, lbl_1_data_6C890, (void *)0x10, fn_1_12AA6C, lbl_1_data_6C8E0.a[0], (void *)0xA, fn_1_12AA6C, lbl_1_data_6C8E0.a[1], (void *)0xA, fn_1_12AA6C, lbl_1_data_6C8E0.a[2], (void *)0xA, fn_1_12AA6C, lbl_1_data_6C8E0.a[3], (void *)0xA, fn_1_12AA6C, lbl_1_data_6C8E0.a[4], (void *)0xA, fn_1_12AA6C, lbl_1_data_6C8E0.a[5], (void *)0xA, fn_1_12AA6C, lbl_1_data_6C8E0.a[6], (void *)0xA, fn_1_12AAD8, lbl_1_data_6C8E0.b, (void *)0xC, fn_1_12AB44, lbl_1_data_6C8E0.c[0], (void *)0xB, fn_1_12AB44, lbl_1_data_6C8E0.c[1], (void *)0xB, fn_1_12AB44, lbl_1_data_6C8E0.c[2], (void *)0xB, fn_1_12ABB0, lbl_1_data_6CB40, (void *)0xC, fn_1_12ABB0, lbl_1_data_6CB7C, (void *)0xC, fn_1_12ABB0, lbl_1_data_6CBB8, (void *)0xC, fn_1_12AC1C, lbl_1_data_6CBF4, (void *)0x10, fn_1_12AC1C, lbl_1_data_6CC44, (void *)0x10, fn_1_12AC1C, lbl_1_data_6CC94, (void *)0x10 };
struct NETimeRecord lbl_1_data_6C440[] = {
    { " AIAI ", 0, 2, 0, 0, 0 },
    { "MEEMEE", 1, 2, 5, 0, 0 },
    { " BABY ", 2, 2, 10, 0, 0 },
    { "GONGON", 3, 2, 15, 0, 0 },
    { "BANANA", 0, 2, 20, 0, 0 },
};
struct NETimeRecord lbl_1_data_6C47C = { " AIAI ", 0, 0, 25, 0, 0 };
struct NETimeRecord lbl_1_data_6C488[] = {
    { " AIAI ", 0, 1, 40, 0, 0 },
    { "MEEMEE", 1, 1, 45, 0, 0 },
    { " BABY ", 2, 1, 50, 0, 0 },
    { "GONGON", 3, 1, 55, 0, 0 },
    { "BANANA", 0, 2, 0, 0, 0 },
};
struct NETimeRecord lbl_1_data_6C4C4 = { " AIAI ", 0, 0, 20, 0, 0 };
struct NETimeRecord lbl_1_data_6C4D0[] = {
    { " AIAI ", 0, 1, 40, 0, 0 },
    { "MEEMEE", 1, 1, 45, 0, 0 },
    { " BABY ", 2, 1, 50, 0, 0 },
    { "GONGON", 3, 1, 55, 0, 0 },
    { "BANANA", 0, 2, 0, 0, 0 },
};
struct NETimeRecord lbl_1_data_6C50C = { " AIAI ", 0, 0, 25, 0, 0 };
struct NETimeRecord lbl_1_data_6C518[] = {
    { " AIAI ", 0, 2, 0, 0, 0 },
    { "MEEMEE", 1, 2, 5, 0, 0 },
    { " BABY ", 2, 2, 10, 0, 0 },
    { "GONGON", 3, 2, 15, 0, 0 },
    { "BANANA", 0, 2, 20, 0, 0 },
};
struct NETimeRecord lbl_1_data_6C554 = { " AIAI ", 0, 0, 40, 0, 0 };
struct NETimeRecord lbl_1_data_6C560[] = {
    { " AIAI ", 0, 1, 40, 0, 0 },
    { "MEEMEE", 1, 1, 45, 0, 0 },
    { " BABY ", 2, 1, 50, 0, 0 },
    { "GONGON", 3, 1, 55, 0, 0 },
    { "BANANA", 0, 2, 0, 0, 0 },
};
struct NETimeRecord lbl_1_data_6C59C = { " AIAI ", 0, 0, 50, 0, 0 };
struct NETimeRecord lbl_1_data_6C5A8[] = {
    { " AIAI ", 0, 2, 0, 0, 0 },
    { "MEEMEE", 1, 2, 5, 0, 0 },
    { " BABY ", 2, 2, 10, 0, 0 },
    { "GONGON", 3, 2, 15, 0, 0 },
    { "BANANA", 0, 2, 20, 0, 0 },
};
struct NETimeRecord lbl_1_data_6C5E4 = { " AIAI ", 0, 0, 55, 0, 0 };
struct NETimeRecord lbl_1_data_6C5F0[] = {
    { " AIAI ", 0, 99, 59, 99, 0 },
    { "MEEMEE", 1, 99, 59, 99, 0 },
    { " BABY ", 2, 99, 59, 99, 0 },
    { "GONGON", 3, 99, 59, 99, 0 },
    { "BANANA", 0, 99, 59, 99, 0 },
    { " AIAI ", 0, 99, 59, 99, 0 },
};
struct NETimeRecord lbl_1_data_6C638[] = {
    { " AIAI ", 0, 99, 59, 99, 0 },
    { "MEEMEE", 1, 99, 59, 99, 0 },
    { " BABY ", 2, 99, 59, 99, 0 },
    { "GONGON", 3, 99, 59, 99, 0 },
    { "BANANA", 0, 99, 59, 99, 0 },
    { " AIAI ", 0, 99, 59, 99, 0 },
};
struct NETimeRecord lbl_1_data_6C680[] = {
    { " AIAI ", 0, 99, 59, 99, 0 },
    { "MEEMEE", 1, 99, 59, 99, 0 },
    { " BABY ", 2, 99, 59, 99, 0 },
    { "GONGON", 3, 99, 59, 99, 0 },
    { "BANANA", 0, 99, 59, 99, 0 },
    { " AIAI ", 0, 99, 59, 99, 0 },
};
struct NETimeRecord lbl_1_data_6C6C8[] = {
    { " AIAI ", 0, 99, 59, 99, 0 },
    { "MEEMEE", 1, 99, 59, 99, 0 },
    { " BABY ", 2, 99, 59, 99, 0 },
    { "GONGON", 3, 99, 59, 99, 0 },
    { "BANANA", 0, 99, 59, 99, 0 },
    { " AIAI ", 0, 99, 59, 99, 0 },
};
struct NETimeRecord lbl_1_data_6C710[] = {
    { " AIAI ", 0, 99, 59, 99, 0 },
    { "MEEMEE", 1, 99, 59, 99, 0 },
    { " BABY ", 2, 99, 59, 99, 0 },
    { "GONGON", 3, 99, 59, 99, 0 },
    { "BANANA", 0, 99, 59, 99, 0 },
    { " AIAI ", 0, 99, 59, 99, 0 },
};
struct NETimeRecord lbl_1_data_6C758[] = {
    { " AIAI ", 0, 99, 59, 99, 0 },
    { "MEEMEE", 1, 99, 59, 99, 0 },
    { " BABY ", 2, 99, 59, 99, 0 },
    { "GONGON", 3, 99, 59, 99, 0 },
    { "BANANA", 0, 99, 59, 99, 0 },
    { " AIAI ", 0, 99, 59, 99, 0 },
};
struct NERankB lbl_1_data_6C7A0[] = {
    { " AIAI ", 0, 50, 0 },
    { "MEEMEE", 1, 40, 0 },
    { " BABY ", 2, 30, 0 },
    { "GONGON", 3, 20, 0 },
    { " AIAI ", 0, 10, 0 },
};
struct NERankB lbl_1_data_6C7F0[] = {
    { " AIAI ", 0, 50, 0 },
    { "MEEMEE", 1, 40, 0 },
    { " BABY ", 2, 30, 0 },
    { "GONGON", 3, 20, 0 },
    { " AIAI ", 0, 10, 0 },
};
struct NERankB lbl_1_data_6C840[] = {
    { " AIAI ", 0, 50, 0 },
    { "MEEMEE", 1, 40, 0 },
    { " BABY ", 2, 30, 0 },
    { "GONGON", 3, 20, 0 },
    { " AIAI ", 0, 10, 0 },
};
struct NERankB lbl_1_data_6C890[] = {
    { " AIAI ", 0, 50, 0 },
    { "MEEMEE", 1, 40, 0 },
    { " BABY ", 2, 30, 0 },
    { "GONGON", 3, 20, 0 },
    { " AIAI ", 0, 10, 0 },
};
struct NERankTables lbl_1_data_6C8E0 = {
    {
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x32 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x28 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0x1E },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0x14 },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0xA },
        },
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x32 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x28 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0x1E },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0x14 },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0xA },
        },
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x32 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x28 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0x1E },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0x14 },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0xA },
        },
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x3 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x2 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0x2 },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0x1 },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x1 },
        },
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x19 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x14 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0xF },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0xA },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x5 },
        },
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x19 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x14 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0xF },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0xA },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x5 },
        },
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x19 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x14 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0xF },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0xA },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x5 },
        },
    },
    {
        { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x32, 0x32 },
        { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x3C, 0x3C },
        { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0x46, 0x46 },
        { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0x50, 0x50 },
        { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x5A, 0x5A },
    },
    {
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x3, 0x0, 0x0 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x3, 0x5, 0x0 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0x3, 0xA, 0x0 },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0x3, 0xF, 0x0 },
            { { 0x42, 0x41, 0x4E, 0x41, 0x4E, 0x41, 0x0, 0x0 }, 0x3, 0x14, 0x0 },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x1, 0x0, 0x0 },
        },
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x3, 0x0, 0x0 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x3, 0x5, 0x0 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0x3, 0xA, 0x0 },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0x3, 0xF, 0x0 },
            { { 0x42, 0x41, 0x4E, 0x41, 0x4E, 0x41, 0x0, 0x0 }, 0x3, 0x14, 0x0 },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x1, 0x1E, 0x0 },
        },
        {
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x2, 0xA, 0x0 },
            { { 0x4D, 0x45, 0x45, 0x4D, 0x45, 0x45, 0x0, 0x1 }, 0x2, 0xF, 0x0 },
            { { 0x20, 0x42, 0x41, 0x42, 0x59, 0x20, 0x0, 0x2 }, 0x2, 0x14, 0x0 },
            { { 0x47, 0x4F, 0x4E, 0x47, 0x4F, 0x4E, 0x0, 0x3 }, 0x2, 0x19, 0x0 },
            { { 0x42, 0x41, 0x4E, 0x41, 0x4E, 0x41, 0x0, 0x0 }, 0x2, 0x1E, 0x0 },
            { { 0x20, 0x41, 0x49, 0x41, 0x49, 0x20, 0x0, 0x0 }, 0x2, 0xA, 0x0 },
        },
    },
};
struct NERankF lbl_1_data_6CB40[] = {
    { " AIAI ", 0, 50 },
    { "MEEMEE", 1, 40 },
    { " BABY ", 2, 30 },
    { "GONGON", 3, 20 },
    { " AIAI ", 0, 10 },
};
struct NERankF lbl_1_data_6CB7C[] = {
    { " AIAI ", 0, 50 },
    { "MEEMEE", 1, 40 },
    { " BABY ", 2, 30 },
    { "GONGON", 3, 20 },
    { " AIAI ", 0, 10 },
};
struct NERankF lbl_1_data_6CBB8[] = {
    { " AIAI ", 0, 50 },
    { "MEEMEE", 1, 40 },
    { " BABY ", 2, 30 },
    { "GONGON", 3, 20 },
    { " AIAI ", 0, 10 },
};
struct NERankB lbl_1_data_6CBF4[] = {
    { " AIAI ", 0, 50, 0 },
    { "MEEMEE", 1, 40, 0 },
    { " BABY ", 2, 30, 0 },
    { "GONGON", 3, 20, 0 },
    { " AIAI ", 0, 10, 0 },
};
struct NERankB lbl_1_data_6CC44[] = {
    { " AIAI ", 0, 50, 0 },
    { "MEEMEE", 1, 40, 0 },
    { " BABY ", 2, 30, 0 },
    { "GONGON", 3, 20, 0 },
    { " AIAI ", 0, 10, 0 },
};
struct NERankB lbl_1_data_6CC94[] = {
    { " AIAI ", 0, 50, 0 },
    { "MEEMEE", 1, 40, 0 },
    { " BABY ", 2, 30, 0 },
    { "GONGON", 3, 20, 0 },
    { " AIAI ", 0, 10, 0 },
};
void *lbl_1_data_6CCE4[] = { &lbl_1_data_6C47C, &lbl_1_data_6C4C4, &lbl_1_data_6C50C, &lbl_1_data_6C554, &lbl_1_data_6C59C, &lbl_1_data_6C5E4 };
void *lbl_1_data_6CCFC[] = { lbl_1_data_6C8E0.c[0] + 5, lbl_1_data_6C8E0.c[1] + 5, lbl_1_data_6C8E0.c[2] + 5 };
u32 lbl_1_data_6CD08[] = { 0x319, 0x43A00000, 0x423C0000, 0x41100000, 0x3F800000, 0x3F800000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0xA, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CD58[] = { 0x319, 0x43A00000, 0x43C80000, 0x41100000, 0x3F99999A, 0x3FC00000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x6, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CDA8[] = { 0x324, 0x42240000, 0x42B60000, 0x3F800000, 0x3EFB6DB7, 0x3E100000, 0x0, 0x0, 0x3EFB6DB7, 0x3E100000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CDF8[] = { 0x324, 0x43400000, 0x42B60000, 0x3F800000, 0x3F000000, 0x3E100000, 0x0, 0x3E100000, 0x3F000000, 0x3E900000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CE48[] = { 0x324, 0x43C58000, 0x42B60000, 0x3F800000, 0x3F149249, 0x3E100000, 0x0, 0x3ED80000, 0x3F149249, 0x3F100000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CE98[] = { 0x324, 0x43E60000, 0x42B60000, 0x3F800000, 0x3F149249, 0x3E100000, 0x0, 0x3ED80000, 0x3F149249, 0x3F100000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CEE8[] = { 0x324, 0x43A10000, 0x42940000, 0x3F800000, 0x3F200000, 0x3E100000, 0x0, 0x3F100000, 0x3F200000, 0x3F340000, 0x0, 0x3F800000, 0x0, 0x1, 0xFFFFFFFF, 0x5, 0xFFFF0000, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CF38[] = { 0x324, 0x44058000, 0x42B60000, 0x3F800000, 0x3F324925, 0x3E100000, 0x0, 0x3F340000, 0x3F324925, 0x3F580000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CF88[] = { 0x324, 0x43C58000, 0x42B60000, 0x3F800000, 0x3EE49249, 0x3E100000, 0x3EFB6DB7, 0x0, 0x3F700000, 0x3E100000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6CFD8[] = { 0x324, 0x43C70000, 0x42B60000, 0x3F800000, 0x3EC49249, 0x3E100000, 0x3F000000, 0x3E100000, 0x3F624925, 0x3E900000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D028[] = { 0x324, 0x43AB0000, 0x42B60000, 0x3F800000, 0x3EC49249, 0x3E100000, 0x3F124925, 0x3E900000, 0x3F749249, 0x3ED80000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D078[] = { 0x324, 0x43E20000, 0x42B60000, 0x3F800000, 0x3F124925, 0x3E100000, 0x0, 0x3E900000, 0x3F124925, 0x3ED80000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D0C8[] = { 0x324, 0x0, 0x42B60000, 0x3F800000, 0x3F324925, 0x3E100000, 0x0, 0x3F580000, 0x3F324925, 0x3F7C0000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D118[] = { 0x314, 0x0, 0x0, 0x40A00000, 0x40200000, 0x3F800000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFFEF0101, 0xFF4E7DFF, 0xFFF1DE21, 0xFF07A55B };
u32 lbl_1_data_6D178[] = { 0x325, 0x0, 0x0, 0x3F800000, 0x3E800000, 0x3E800000, 0x0, 0x0, 0x3E800000, 0x3E800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D1C8[] = { 0x325, 0x0, 0x0, 0x3F800000, 0x3E800000, 0x3E800000, 0x0, 0x3F000000, 0x3E800000, 0x3F400000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D218[] = { 0x325, 0x0, 0x0, 0x3F800000, 0x3E800000, 0x3E800000, 0x3E800000, 0x3E800000, 0x3F000000, 0x3F000000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D268[] = { 0x325, 0x0, 0x0, 0x3F800000, 0x3E800000, 0x3E800000, 0x3F000000, 0x3F400000, 0x3F400000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D2B8[] = { 0x325, 0x0, 0x0, 0x3F800000, 0x3E800000, 0x3E800000, 0x3F400000, 0x0, 0x3F800000, 0x3E800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
struct SpriteDrawRequest *lbl_1_data_6D308[] = { (struct SpriteDrawRequest *)lbl_1_data_6D178, (struct SpriteDrawRequest *)lbl_1_data_6D1C8, (struct SpriteDrawRequest *)lbl_1_data_6D218, (struct SpriteDrawRequest *)lbl_1_data_6D268, (struct SpriteDrawRequest *)lbl_1_data_6D2B8 };
u32 lbl_1_data_6D31C[] = { 0x31F, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x3F800000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D36C[] = { 0x320, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x3F800000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x31B, 0x43A00000, 0x43C68000, 0x3F800000, 0x42890000, 0x3F800000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0xA, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x318, 0x42680000, 0x43D20000, 0x3F800000, 0x3F800000, 0x3F800000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D45C[] = { 0x317, 0x42400000, 0x43D40000, 0x3F800000, 0x3F800000, 0x3F800000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D4AC[] = { 0x31, 0x0, 0x0, 0x3F800000, 0x3F000000, 0x3EA80000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
u32 lbl_1_data_6D4FC[] = { 0x31C, 0x0, 0x42400000, 0x3DCCCCCD, 0x3F800000, 0x3F800000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0x5, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
struct SpriteDrawRequest lbl_1_data_6D54C = { 0x321, 0.0f, 0.0f, 1.0f, 0.33333334f, 1.0f, 0.33333334f, 0.0f, 0.6666667f, 1.0f, 0, 1.0f, -1, 5, -1, 0 };
u32 lbl_1_data_6D59C[] = { 0xF, 0x0, 0x0, 0x3F800000, 0x3F000000, 0x3EA80000, 0x0, 0x0, 0x3F800000, 0x3F800000, 0x0, 0x3F800000, 0xFFFFFFFF, 0xA, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0 };
void *lbl_1_data_6D5EC[] = { (void *)lbl_1_rodata_65750, (void *)lbl_1_rodata_6575C, (void *)lbl_1_rodata_65768, (void *)lbl_1_rodata_65774 };

void g_init_rankings_to_defaults(void);

void g_init_rankings_to_defaults_wrapper(void)
{
    g_init_rankings_to_defaults();
}

void g_init_rankings_to_defaults(void)
{
    int i;
    int j;
    struct NameEntryRec *r;
    struct NameEntryRec *rec;
    struct NameEntryData *data;
    struct NameEntryDefault *def;

    if (g_some_other_flags & 4)
        printf("[NameEntry2_PowerOnInit]\n");
    printf("nameentry_data\n");
    rec = (void *)lbl_1_bss_D588C;
    memset(rec, 0, 0x230);
    for (i = 0; i < 4; i++, rec += 5)
    {
        r = rec;
        for (j = 0; j < 5; j++, r++)
        {
            strcpy(r->name, lbl_1_data_6B918[i][j]);
            r->unkC = lbl_1_data_6BA08[i][j];
            r->score = lbl_1_data_6B968[i][j];
            r->unk10 = 0xFF;
            r->unk11 = 0;
            r->unk14 = 0x57E3F;
            r->unk18 = -1;
        }
    }
    for (i = 0; i < 4; i++)
    {
        data = lbl_1_data_6BA58[i];
        for (j = 0; j < 5; j++)
        {
            strcpy(data[j].name, lbl_1_data_6B918[i][j]);
            data[j].unk14 = lbl_1_data_6BA08[i][j];
            data[j].score = lbl_1_data_6B968[i][j];
            data[j].unk10 = 0xFF;
            data[j].unk11 = 0;
            data[j].unkC = 0x57E3F;
            data[j].unk12 = -1;
        }
    }
    def = (void *)lbl_1_bss_D5ABC;
    for (i = 0; i < 5; i++, def++)
    {
        strcpy(def->name, lbl_1_data_6BA68[i].name);
        def->unk8 = lbl_1_data_6BA68[i].unk8;
        def->unkC = lbl_1_data_6BA68[i].unkC;
        def->unk10 = lbl_1_data_6BA68[i].unk10;
    }
}

void fn_1_118504(void)
{
    int i;
    int j;
    s8 course;
    int score;
    int floor;
    struct Ball *ball;
    int rank;
    struct NameEntryCont *arr;

    ball = &balls[modeCtrl.currPlayer];
    if (g_some_other_flags & 4)
        printf("NameEntry2_KeepPlayerDataContinue\n");
    arr = lbl_1_bss_D5B20[modeCtrl.currPlayer];
    course = (modeCtrl.courseFlags & 0x800000) ? 3
        : (modeCtrl.courseFlags & 0x10) ? 2
        : (modeCtrl.courseFlags & 8) ? 1
        : 0;
    score = ball->score;
    floor = mode_info.currFloor;
    for (i = 0; i < 5; i++)
    {
        if (score > arr[i].score)
        {
            for (j = 4; j > i; j--)
                arr[j] = arr[j - 1];
            arr[i].score = score;
            arr[i].course = course;
            arr[i].floor = floor;
            rank = i + 1;
            break;
        }
    }
    lbl_1_bss_D5C10.ape = ball->ape;
    strcpy((char *)lbl_1_bss_D5C10.name, "______");
    lbl_1_bss_D5C10.score = score;
    lbl_1_bss_D5C10.course = course;
    lbl_1_bss_D5C10.floor = floor;
    lbl_1_bss_D5882 = rank;
    lbl_1_bss_D5C6C = 0;
}

int fn_1_11A1B8(void);
struct NameEntryRec *fn_1_118BAC(int course, struct NameEntryRec *out);
int fn_1_11A7A4(struct NameEntryData data);

void g_smth_with_ending_course_2(void)
{
    int i;
    struct Ball *ball;
    struct NameEntryRec *rec;
    s8 rank;
    struct NameEntryCont *arr;

    rank = 0;
    if (g_some_other_flags & 4)
        printf("NameEntry2_KeepPlayerData\n");
    ball = &balls[modeCtrl.currPlayer];
    lbl_1_bss_D5C10.unk0 = 0;
    rec = fn_1_118BAC(fn_1_11A1B8(), NULL);
    for (i = 0; i < 5; i++, rec++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, rec->name);
        lbl_1_bss_D5C70[i].score = rec->score;
        lbl_1_bss_D5C70[i].unk10 = rec->unk10;
        lbl_1_bss_D5C70[i].unk14 = rec->unkC;
        lbl_1_bss_D5C70[i].unk11 = rec->unk11;
        lbl_1_bss_D5C70[i].unk12 = -1;
        printf("[%d]ne_data name score:%s %d\n", i, lbl_1_bss_D5C70[i].name, lbl_1_bss_D5C70[i].score);
    }
    strcpy((char *)lbl_1_bss_D5C10.name, "______");
    lbl_1_bss_D5C10.score = ball->score;
    lbl_1_bss_D5C10.floor = mode_info.currFloor;
    lbl_1_bss_D5C10.ape = ball->ape;
    lbl_1_bss_D5C10.apeType = lbl_1_bss_D5C10.ape->charaId;
    if (modeCtrl.courseFlags & 0x800000)
        lbl_1_bss_D5C10.course = 3;
    else if (modeCtrl.courseFlags & 0x10)
        lbl_1_bss_D5C10.course = 2;
    else if (modeCtrl.courseFlags & 8)
        lbl_1_bss_D5C10.course = 1;
    else
        lbl_1_bss_D5C10.course = 0;
    arr = lbl_1_bss_D5B20[modeCtrl.currPlayer];
    for (i = 0; i < 5; i++)
    {
        int r;

        lbl_1_bss_D5D60.score = arr[i].score;
        lbl_1_bss_D5D60.unk12 = modeCtrl.currPlayer;
        lbl_1_bss_D5D60.unk14 = lbl_1_bss_D5C10.apeType;
        lbl_1_bss_D5D60.unk10 = arr[i].floor;
        lbl_1_bss_D5D60.unk11 = arr[i].course;
        r = fn_1_11A7A4(lbl_1_bss_D5D60);
        if (r != 0 && rank == 0)
            rank = r;
    }
    lbl_1_bss_D5D60.score = lbl_1_bss_D5C10.score;
    lbl_1_bss_D5D60.unk11 = lbl_1_bss_D5C10.course;
    lbl_1_bss_D5D60.unk10 = lbl_1_bss_D5C10.floor;
    lbl_1_bss_D5D60.unk14 = lbl_1_bss_D5C10.apeType;
    lbl_1_bss_D5D60.unk12 = modeCtrl.currPlayer;
    lbl_1_bss_D5C10.rank = fn_1_11A7A4(lbl_1_bss_D5D60);
    if (lbl_1_bss_D5C10.rank == 0 && rank != 0)
    {
        lbl_1_bss_D5C10.rank = rank;
        lbl_1_bss_D5C10.score = lbl_1_bss_D5C70[rank - 1].score;
        lbl_1_bss_D5C10.floor = lbl_1_bss_D5C70[rank - 1].unk10;
        lbl_1_bss_D5C10.course = lbl_1_bss_D5C70[rank - 1].unk11;
    }
    if (lbl_1_bss_D5C10.rank != 0)
        lbl_1_bss_D5D90 = 0;
    if (lbl_1_bss_D5C10.rank == 0)
        lbl_1_bss_D5D90 = 1;
    lbl_1_bss_D5C6C = 1;
}

struct NameEntryRec *fn_1_118BAC(int course, struct NameEntryRec *out)
{
    struct NameEntryRec (*base)[5];
    struct NameEntryRec *rec;

    if (g_some_other_flags & 4)
        printf("NameEntry2_GetCourse\n");
    base = (void *)lbl_1_bss_D588C;
    rec = base[course];
    if (out != NULL)
    {
        memcpy_handler(out, rec, 0x8C);
        return out;
    }
    return rec;
}

void g_NameEntry2_InitFirst(void)
{
    int i;
    int j;
    struct NameEntryRec *r;
    struct NameEntryRec (*rec)[5];
    struct NameEntryData *data;
    struct NameEntryRec (*rec2)[5];
    struct NameEntryRec (*base)[5];

    base = (void *)lbl_1_bss_D588C;
    rec = base;
    if (g_some_other_flags & 4)
        printf("NameEntry2_InitFirst\n");
    for (i = 4; i > 0; i--, rec++)
    {
        r = *rec;
        for (j = 5; j > 0; j--, r++)
            r->unk18 = -1;
    }
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 5; j++)
        {
            lbl_1_bss_D5B20[i][j].score = -1;
            lbl_1_bss_D5B20[i][j].floor = 0xFF;
            lbl_1_bss_D5B20[i][j].course = -1;
        }
    }
    for (i = 0; i < 5; i++)
        lbl_1_bss_D5C70[i].unk12 = -1;
    lbl_1_bss_D5D94 = 0;
    lbl_1_bss_D5C6C = 0;
    printf("mode.course:%d\n", modeCtrl.difficulty);
    base = (void *)lbl_1_bss_D588C;
    rec2 = base;
    for (i = 0; i < 4; i++, rec2++)
    {
        data = lbl_1_data_6BA58[i];
        r = *rec2;
        for (j = 0; j < 5; j++, r++)
        {
            strcpy(data[j].name, r->name);
            data[j].score = r->score;
            data[j].unk14 = r->unkC;
            data[j].unk10 = r->unk10;
            data[j].unk11 = r->unk11;
            data[j].unkC = 0x57E3F;
            data[j].unk12 = -1;
        }
    }
}

void fn_1_118E64(void)
{
    if (lbl_1_bss_D5C10.unk0 != 0 && lbl_1_bss_D5C10.unk0 == 1)
    {
        switch (lbl_1_bss_D5C10.unk50)
        {
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            lbl_1_bss_D5C10.unk50 -= 6;
            break;
        }
        switch (lbl_1_bss_D5C10.unk50)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            strcpy((char *)lbl_1_bss_D5C10.name, "      ");
            lbl_1_bss_D5C10.rank = lbl_1_bss_D5D98[lbl_1_bss_D5D9C];
            lbl_1_bss_D5C10.unk10 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk24;
            lbl_1_bss_D5C10.unk11 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk25;
            lbl_1_bss_D5C10.unk12 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk26;
            lbl_1_bss_D5C10.apeType = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
            lbl_1_bss_D5C10.unk44 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk2C;
            break;
        case 0x18:
        case 0x19:
        case 0x1A:
            strcpy((char *)lbl_1_bss_D5C10.name, "      ");
            lbl_1_bss_D5C10.rank = lbl_1_bss_D5D98[lbl_1_bss_D5D9C];
            lbl_1_bss_D5C10.unk10 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk24;
            lbl_1_bss_D5C10.unk11 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk25;
            lbl_1_bss_D5C10.unk12 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk26;
            lbl_1_bss_D5C10.apeType = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
            break;
        case 0x17:
            strcpy((char *)lbl_1_bss_D5C10.name, "      ");
            lbl_1_bss_D5C10.rank = lbl_1_bss_D5D98[lbl_1_bss_D5D9C];
            lbl_1_bss_D5C10.unk16 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk2A;
            lbl_1_bss_D5C10.unk14 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk28;
            lbl_1_bss_D5C10.apeType = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
            break;
        default:
            strcpy((char *)lbl_1_bss_D5C10.name, "      ");
            lbl_1_bss_D5C10.rank = lbl_1_bss_D5D98[lbl_1_bss_D5D9C];
            lbl_1_bss_D5C10.score = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].score;
            lbl_1_bss_D5C10.apeType = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
            printf("\tnsys.rank:%d\n", lbl_1_bss_D5C10.rank);
            printf("\tnsys.score:%d\n", lbl_1_bss_D5C10.score);
            printf("\tnsys.ape_type:%d\n", lbl_1_bss_D5C10.apeType);
            break;
        }
    }
    lbl_1_bss_D5C10.unk9 = 0;
    lbl_1_bss_D5C10.unk4 = lbl_80198B38;
}

void fn_1_1191F4(void)
{
    int idx;

    idx = lbl_1_bss_D5C10.rank - 1;
    if (lbl_1_bss_D5C10.unk0 == 0)
    {
        lbl_1_bss_D5C70[idx].unk1C = lbl_1_bss_D5C10.ape;
        lbl_1_bss_D5C70[idx].score = lbl_1_bss_D5C10.score;
        lbl_1_bss_D5C70[idx].unk10 = lbl_1_bss_D5C10.floor;
        lbl_1_bss_D5C70[idx].unk11 = lbl_1_bss_D5C10.course;
        strcpy(lbl_1_bss_D5C70[idx].name, (char *)lbl_1_bss_D5C10.name);
    }
    else if (lbl_1_bss_D5C10.unk0 == 1)
    {
        switch (lbl_1_bss_D5C10.unk50)
        {
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            lbl_1_bss_D5C10.unk50 -= 6;
            break;
        }
        switch (lbl_1_bss_D5C10.unk50)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            if (lbl_1_bss_D5C10.unk54 & 4)
            {
                lbl_1_bss_D5E60.apeType = lbl_1_bss_D5C10.apeType;
                strcpy(lbl_1_bss_D5E60.name, (char *)lbl_1_bss_D5C10.name);
            }
            else if (lbl_1_bss_D5C10.unk54 & 2)
            {
                lbl_1_bss_D5E60.apeType = lbl_1_bss_D5C10.apeType;
                lbl_1_bss_D5E60.unk2C = lbl_1_bss_D5C10.unk44;
                strcpy(lbl_1_bss_D5E60.name, (char *)lbl_1_bss_D5C10.name);
                lbl_1_bss_D5C70[idx].unk20 = lbl_1_bss_D5C10.rank;
                lbl_1_bss_D5C70[idx].unk14 = lbl_1_bss_D5C10.apeType;
                lbl_1_bss_D5C70[idx].unk24 = lbl_1_bss_D5C10.unk10;
                lbl_1_bss_D5C70[idx].unk25 = lbl_1_bss_D5C10.unk11;
                lbl_1_bss_D5C70[idx].unk26 = lbl_1_bss_D5C10.unk12;
                lbl_1_bss_D5C70[idx].unk2C = lbl_1_bss_D5C10.unk44;
                strcpy(lbl_1_bss_D5C70[idx].name, (char *)lbl_1_bss_D5C10.name);
            }
            else
            {
                lbl_1_bss_D5C70[idx].unk20 = lbl_1_bss_D5C10.rank;
                lbl_1_bss_D5C70[idx].unk14 = lbl_1_bss_D5C10.apeType;
                lbl_1_bss_D5C70[idx].unk24 = lbl_1_bss_D5C10.unk10;
                lbl_1_bss_D5C70[idx].unk25 = lbl_1_bss_D5C10.unk11;
                lbl_1_bss_D5C70[idx].unk26 = lbl_1_bss_D5C10.unk12;
                lbl_1_bss_D5C70[idx].unk2C = lbl_1_bss_D5C10.unk44;
                strcpy(lbl_1_bss_D5C70[idx].name, (char *)lbl_1_bss_D5C10.name);
            }
            break;
        case 0x18:
        case 0x19:
        case 0x1A:
            if (lbl_1_bss_D5C10.unk54 & 4)
            {
                lbl_1_bss_D5E60.apeType = lbl_1_bss_D5C10.apeType;
                strcpy(lbl_1_bss_D5E60.name, (char *)lbl_1_bss_D5C10.name);
            }
            else if (lbl_1_bss_D5C10.unk54 & 2)
            {
                lbl_1_bss_D5E60.apeType = lbl_1_bss_D5C10.apeType;
                strcpy(lbl_1_bss_D5E60.name, (char *)lbl_1_bss_D5C10.name);
                lbl_1_bss_D5C70[idx].unk20 = lbl_1_bss_D5C10.rank;
                lbl_1_bss_D5C70[idx].unk14 = lbl_1_bss_D5C10.apeType;
                lbl_1_bss_D5C70[idx].unk24 = lbl_1_bss_D5C10.unk10;
                lbl_1_bss_D5C70[idx].unk25 = lbl_1_bss_D5C10.unk11;
                lbl_1_bss_D5C70[idx].unk26 = lbl_1_bss_D5C10.unk12;
                strcpy(lbl_1_bss_D5C70[idx].name, (char *)lbl_1_bss_D5C10.name);
            }
            else
            {
                lbl_1_bss_D5C70[idx].unk20 = lbl_1_bss_D5C10.rank;
                lbl_1_bss_D5C70[idx].unk14 = lbl_1_bss_D5C10.apeType;
                lbl_1_bss_D5C70[idx].unk24 = lbl_1_bss_D5C10.unk10;
                lbl_1_bss_D5C70[idx].unk25 = lbl_1_bss_D5C10.unk11;
                lbl_1_bss_D5C70[idx].unk26 = lbl_1_bss_D5C10.unk12;
                strcpy(lbl_1_bss_D5C70[idx].name, (char *)lbl_1_bss_D5C10.name);
            }
            break;
        case 0x17:
            lbl_1_bss_D5C70[idx].unk20 = lbl_1_bss_D5C10.rank;
            lbl_1_bss_D5C70[idx].unk14 = lbl_1_bss_D5C10.apeType;
            lbl_1_bss_D5C70[idx].unk2A = lbl_1_bss_D5C10.unk16;
            lbl_1_bss_D5C70[idx].unk28 = lbl_1_bss_D5C10.unk14;
            strcpy(lbl_1_bss_D5C70[idx].name, (char *)lbl_1_bss_D5C10.name);
            break;
        default:
            lbl_1_bss_D5C70[idx].unk20 = lbl_1_bss_D5C10.rank;
            lbl_1_bss_D5C70[idx].unk14 = lbl_1_bss_D5C10.apeType;
            lbl_1_bss_D5C70[idx].score = lbl_1_bss_D5C10.score;
            strcpy(lbl_1_bss_D5C70[idx].name, (char *)lbl_1_bss_D5C10.name);
            break;
        }
    }
}

void fn_1_11979C(struct NameEntryData *unused)
{
    if (lbl_1_bss_D5C10.unk0 == 2)
    {
        int k;
        struct NameEntryDefault *def;
        struct NameEntryDefault *base;
        char blank[7] = "______";

        base = (void *)lbl_1_bss_D5ABC;
        def = base;
        for (k = 0; k < 5; k++)
        {
            if (strncmp(lbl_1_bss_D5C70[k].name, blank, 6) == 0)
                strncpy(lbl_1_bss_D5C70[k].name, lbl_1_data_6B630[lbl_1_bss_D5C10.apeType], 6);
            strcpy(lbl_1_data_6BA68[k].name, lbl_1_bss_D5C70[k].name);
            lbl_1_data_6BA68[k].unk8 = lbl_1_bss_D5C70[k].score;
            lbl_1_data_6BA68[k].unkC = lbl_1_bss_D5C70[k].unk18;
            lbl_1_data_6BA68[k].unk10 = lbl_1_bss_D5C70[k].unkC;
        }
        for (k = 0; k < 5; k++, def++)
        {
            strcpy(def->name, lbl_1_data_6BA68[k].name);
            def->unk8 = lbl_1_data_6BA68[k].unk8;
            def->unkC = lbl_1_data_6BA68[k].unkC;
            def->unk10 = lbl_1_data_6BA68[k].unk10;
        }
    }
    else if (lbl_1_bss_D5C10.unk0 == 0)
    {
        struct NameEntryRec *rec;
        struct NameEntryRec *r;
        int j;
        int i;
        struct NameEntryData *data;
        struct NameEntryRec *all;

        printf("entry_data_keep\n");
        data = lbl_1_data_6BA58[fn_1_11A1B8()];
        for (i = 0; i < 5; i++)
        {
            strcpy(data[i].name, lbl_1_bss_D5C70[i].name);
            data[i].unk14 = lbl_1_bss_D5C70[i].unk14;
            data[i].score = lbl_1_bss_D5C70[i].score;
            data[i].unk10 = lbl_1_bss_D5C70[i].unk10;
            data[i].unk11 = lbl_1_bss_D5C70[i].unk11;
            data[i].unk12 = lbl_1_bss_D5C70[i].unk12;
        }
        all = (void *)lbl_1_bss_D588C;
        memset(all, 0, 0x230);
        rec = all;
        memset(rec, 0, 0x8C);
        for (i = 0; i < 4; i++, rec += 5)
        {
            data = lbl_1_data_6BA58[i];
            r = rec;
            for (j = 0; j < 5; j++, r++)
            {
                strcpy(r->name, data[j].name);
                r->unkC = data[j].unk14;
                r->score = data[j].score;
                r->unk10 = data[j].unk10;
                r->unk11 = data[j].unk11;
                r->unk14 = 0;
                r->unk18 = -1;
                printf("%d,%d,%s,%d\n", i, j, r->name, r->score);
            }
        }
    }
}

void fn_1_119B1C(void)
{
    int i;
    int x;

    x = lbl_1_bss_D5C10.unk50;
    switch (x)
    {
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        x -= 6;
        break;
    }
    switch (x)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
        struct NERankA *rows = lbl_1_data_6C2B4[x].ptr;
        struct NERankA *one = lbl_1_data_6CCE4[x];

        if (lbl_1_bss_D5C10.unk54 & 4)
        {
            strcpy(one->name, lbl_1_bss_D5E60.name);
            one->unk8 = lbl_1_bss_D5E60.unk24;
            one->unk9 = lbl_1_bss_D5E60.unk25;
            one->unkA = lbl_1_bss_D5E60.unk26;
            one->ape = lbl_1_bss_D5E60.apeType;
            one->unkB = lbl_1_bss_D5E60.unk2C;
        }
        else
        {
            for (i = 0; i < 5; i++)
            {
                strcpy(rows[i].name, lbl_1_bss_D5C70[i].name);
                rows[i].ape = lbl_1_bss_D5C70[i].unk14;
                rows[i].unk8 = lbl_1_bss_D5C70[i].unk24;
                rows[i].unk9 = lbl_1_bss_D5C70[i].unk25;
                rows[i].unkA = lbl_1_bss_D5C70[i].unk26;
                rows[i].unkB = lbl_1_bss_D5C70[i].unk2C;
            }
            if (lbl_1_bss_D5C10.unk54 & 2)
            {
                strcpy(one->name, lbl_1_bss_D5E60.name);
                one->unk8 = lbl_1_bss_D5E60.unk24;
                one->unk9 = lbl_1_bss_D5E60.unk25;
                one->unkA = lbl_1_bss_D5E60.unk26;
                one->ape = lbl_1_bss_D5E60.apeType;
                one->unkB = lbl_1_bss_D5E60.unk2C;
                printf("keep lap.ape_type:%d\n", lbl_1_bss_D5E60.apeType);
                printf("keep lap->ape_data:%d\n", one->ape);
            }
        }
        break;
    }
    case 0xC:
    case 0xD:
    case 0xE:
    case 0xF:
    {
        struct NERankB *rows = lbl_1_data_6C2B4[x].ptr;

        for (i = 0; i < 5; i++)
        {
            strcpy(rows[i].name, lbl_1_bss_D5C70[i].name);
            rows[i].ape = lbl_1_bss_D5C70[i].unk14;
            rows[i].score = lbl_1_bss_D5C70[i].score;
            rows[i].unkC = 0;
        }
        break;
    }
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    {
        struct NERankC *rows = lbl_1_data_6C2B4[x].ptr;

        for (i = 0; i < 5; i++)
        {
            strcpy(rows[i].name, lbl_1_bss_D5C70[i].name);
            rows[i].ape = lbl_1_bss_D5C70[i].unk14;
            rows[i].unk8 = lbl_1_bss_D5C70[i].score;
        }
        break;
    }
    case 0x17:
    {
        struct NERankD *rows = lbl_1_data_6C2B4[x].ptr;

        for (i = 0; i < 5; i++)
        {
            strcpy(rows[i].name, lbl_1_bss_D5C70[i].name);
            rows[i].ape = lbl_1_bss_D5C70[i].unk14;
            rows[i].unkA = lbl_1_bss_D5C70[i].unk2A;
            rows[i].unk8 = lbl_1_bss_D5C70[i].unk28;
        }
        break;
    }
    case 0x18:
    case 0x19:
    case 0x1A:
    {
        struct NERankE *rows = lbl_1_data_6C2B4[x].ptr;
        struct NERankE *one = lbl_1_data_6CCFC[x - 0x18];

        if (lbl_1_bss_D5C10.unk54 & 4)
        {
            strcpy(one->name, lbl_1_bss_D5E60.name);
            one->ape = lbl_1_bss_D5E60.apeType;
            one->unk8 = lbl_1_bss_D5E60.unk24;
            one->unk9 = lbl_1_bss_D5E60.unk25;
            one->unkA = lbl_1_bss_D5E60.unk26;
        }
        else
        {
            for (i = 0; i < 5; i++)
            {
                strcpy(rows[i].name, lbl_1_bss_D5C70[i].name);
                rows[i].ape = lbl_1_bss_D5C70[i].unk14;
                rows[i].unk8 = lbl_1_bss_D5C70[i].unk24;
                rows[i].unk9 = lbl_1_bss_D5C70[i].unk25;
                rows[i].unkA = lbl_1_bss_D5C70[i].unk26;
            }
            if (lbl_1_bss_D5C10.unk54 & 2)
            {
                strcpy(one->name, lbl_1_bss_D5E60.name);
                one->ape = lbl_1_bss_D5E60.apeType;
                one->unk8 = lbl_1_bss_D5E60.unk24;
                one->unk9 = lbl_1_bss_D5E60.unk25;
                one->unkA = lbl_1_bss_D5E60.unk26;
            }
        }
        break;
    }
    case 0x1B:
    case 0x1C:
    case 0x1D:
    {
        struct NERankF *rows = lbl_1_data_6C2B4[x].ptr;

        for (i = 0; i < 5; i++)
        {
            strcpy(rows[i].name, lbl_1_bss_D5C70[i].name);
            rows[i].ape = lbl_1_bss_D5C70[i].unk14;
            rows[i].score = lbl_1_bss_D5C70[i].score;
        }
        break;
    }
    case 0x1E:
    case 0x1F:
    case 0x20:
    {
        struct NERankB *rows = lbl_1_data_6C2B4[x].ptr;

        for (i = 0; i < 5; i++)
        {
            strcpy(rows[i].name, lbl_1_bss_D5C70[i].name);
            rows[i].ape = lbl_1_bss_D5C70[i].unk14;
            rows[i].score = lbl_1_bss_D5C70[i].score;
            rows[i].unkC = 0;
        }
        break;
    }
    }
    lbl_1_bss_D5C10.unk54 = 0;
}

int fn_1_11A1B8(void)
{
    int course = modeCtrl.difficulty;

    if (modeCtrl.courseFlags & 0x10)
        g_some_gift_menu_flags |= 1;
    if ((modeCtrl.courseFlags & 0x10) && (modeCtrl.courseFlags & 0x2000000))
        course = 3;
    if (!(modeCtrl.courseFlags & 0x2000000) && (modeCtrl.courseFlags & 0x10))
        course = modeCtrl.difficulty;
    if (lbl_1_bss_D5C10.unk0 == 2)
        course = 4;
    return course;
}

int fn_1_11A28C(struct NameEntryData data)
{
    struct NameEntryData tmp2;
    struct NameEntryData tmp;
    int rank;
    int found;
    int i;

    rank = 0;
    found = 0;
    for (i = 0; i < 5; i++)
    {
        if (found)
        {
            tmp = lbl_1_bss_D5C70[i];
            lbl_1_bss_D5C70[i] = tmp2;
            tmp2 = tmp;
        }
        else if (data.score > lbl_1_bss_D5C70[i].score)
        {
            tmp2 = lbl_1_bss_D5C70[i];
            lbl_1_bss_D5C70[i] = data;
            rank = i + 1;
            found = 1;
        }
        else if (data.score == lbl_1_bss_D5C70[i].score)
        {
            if (data.unk18 > lbl_1_bss_D5C70[i].unk18)
            {
                tmp2 = lbl_1_bss_D5C70[i];
                lbl_1_bss_D5C70[i] = data;
                rank = i + 1;
                found = 1;
            }
            else if (data.unk18 == lbl_1_bss_D5C70[i].unk18)
            {
                if (data.unkC <= lbl_1_bss_D5C70[i].unkC)
                {
                    tmp2 = lbl_1_bss_D5C70[i];
                    lbl_1_bss_D5C70[i] = data;
                    rank = i + 1;
                    found = 1;
                }
            }
        }
    }
    printf(" story mode ranking check!!!!!!!!!!!! \n");
    if (found)
        return rank;
    return 0;
}

int fn_1_11A7A4(struct NameEntryData data)
{
    struct NameEntryData tmp2;
    struct NameEntryData tmp;
    s8 found;
    int rank;
    int i;

    found = 0;
    rank = 0;
    for (i = 0; i < 5; i++)
    {
        if (found)
        {
            tmp = lbl_1_bss_D5C70[i];
            lbl_1_bss_D5C70[i] = tmp2;
            tmp2 = tmp;
        }
        else if (data.score > lbl_1_bss_D5C70[i].score)
        {
            tmp2 = lbl_1_bss_D5C70[i];
            lbl_1_bss_D5C70[i] = data;
            rank = i + 1;
            found = 1;
        }
        else if (data.score == lbl_1_bss_D5C70[i].score)
        {
            if (g_some_other_flags & 4)
                printf("equal\n");
        }
    }
    if (found == 0)
    {
        g_nameentry_did_get_top_5 = 0;
        return 0;
    }
    g_nameentry_did_get_top_5 = 1;
    return rank;
}

void g_nameentry2_init(void)
{
    if (g_some_other_flags & 4)
        g_very_similar_to_puts("[nameentry2_init]");
    dest_all_events();
    load_stage(0xC8);
    event_init(0);
    event_init(0x14);
    event_init(0x11);
    event_init(0x13);
    lbl_1_bss_243C = 0;
    event_init(0x10);
    load_bmp_by_id(3);
    fade_screen_to_color(0x100, 0xFFFFFF, 0x3C);
    g_related_to_pause_menu_input.flags |= 2;
    lbl_1_bss_D5878 = 0;
    lbl_1_bss_D5C10.unk0 = 0;
    if (g_some_gift_menu_flags & 4)
        lbl_1_bss_D5C10.unk0 = 2;
    g_some_gift_menu_flags &= ~4;
    strcpy((char *)lbl_1_bss_D5C10.name, "      ");
    if (lbl_1_bss_D5C10.unk0 == 0 && lbl_1_bss_D5C6C == 0)
    {
        if (sub_mode == 0xC3)
        {
            struct Ball *ball;
            int course;
            int i;
            struct NameEntryRec *rec;

            if (g_some_other_flags & 4)
                printf("nameentry2_init not set already\n");
            ball = &balls[modeCtrl.currPlayer];
            course = fn_1_11A1B8();
            rec = fn_1_118BAC(course, NULL);
            for (i = 0; i < 5; i++, rec++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rec->name);
                lbl_1_bss_D5C70[i].score = rec->score;
                lbl_1_bss_D5C70[i].unk10 = rec->unk10;
                lbl_1_bss_D5C70[i].unk14 = rec->unkC;
                lbl_1_bss_D5C70[i].unk11 = rec->unk11;
                lbl_1_bss_D5C70[i].unk12 = -1;
            }
            if (sub_mode == 0xC3)
                lbl_1_bss_D5D60.score = ((rand() & 0x7FFF) % 500 + 100) * 100;
            else if (sub_mode == 0x3F)
                lbl_1_bss_D5D60.score = ball->score;
            lbl_1_bss_D5D60.unk20 = fn_1_11A7A4(lbl_1_bss_D5D60);
            if (lbl_1_bss_D5D60.unk20 != 0)
            {
                lbl_1_bss_D5D60.unk1C = ball->ape;
                if (modeCtrl.courseFlags & 0x800000)
                    lbl_1_bss_D5D60.unk11 = 3;
                else if (modeCtrl.courseFlags & 0x10)
                    lbl_1_bss_D5D60.unk11 = 2;
                else if (modeCtrl.courseFlags & 8)
                    lbl_1_bss_D5D60.unk11 = 1;
                else
                    lbl_1_bss_D5D60.unk11 = 0;
                lbl_1_bss_D5C10.apeType = lbl_1_bss_D5D60.unk1C->charaId;
            }
            if (lbl_1_bss_D5D60.unk20 == 0)
            {
                lbl_1_bss_D5C10.unk8 = 6;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 5;
                camera_set_state(3);
                g_current_camera->eye = lbl_1_data_6BE68;
                g_current_camera->lookAt = lbl_1_data_6BE74;
            }
        }
        else
        {
            int i;
            struct NameEntryRec *rec;
            int course;
            struct NameEntryCont *arr;
            int rank = 0;

            if (g_some_other_flags & 4)
                printf("continue to nameentry direct\n");
            course = fn_1_11A1B8();
            rec = fn_1_118BAC(course, NULL);
            for (i = 0; i < 5; i++, rec++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rec->name);
                lbl_1_bss_D5C70[i].score = rec->score;
                lbl_1_bss_D5C70[i].unk10 = rec->unk10;
                lbl_1_bss_D5C70[i].unk14 = rec->unkC;
                lbl_1_bss_D5C70[i].unk11 = rec->unk11;
                lbl_1_bss_D5C70[i].unk12 = -1;
            }
            lbl_1_bss_D5C10.apeType = lbl_1_bss_D5C10.ape->charaId;
            arr = lbl_1_bss_D5B20[modeCtrl.currPlayer];
            for (i = 0; i < 5; i++)
            {
                int r;

                if (arr[i].score == -1)
                    break;
                lbl_1_bss_D5D60.score = arr[i].score;
                lbl_1_bss_D5D60.unk12 = (s8)modeCtrl.currPlayer;
                lbl_1_bss_D5D60.unk14 = lbl_1_bss_D5C10.apeType;
                lbl_1_bss_D5D60.unk10 = arr[i].floor;
                lbl_1_bss_D5D60.unk11 = arr[i].course;
                r = fn_1_11A7A4(lbl_1_bss_D5D60);
                if (r != 0 && rank == 0)
                    rank = r;
            }
            for (i = 0; i < 5; i++)
            {
                if (modeCtrl.currPlayer == lbl_1_bss_D5C70[i].unk12)
                {
                    strcpy((char *)lbl_1_bss_D5C10.name, "______");
                    lbl_1_bss_D5C10.rank = (s8)(i + 1);
                    lbl_1_bss_D5C10.score = lbl_1_bss_D5C70[i].score;
                    lbl_1_bss_D5C10.floor = (s8)lbl_1_bss_D5C70[i].unk10;
                    lbl_1_bss_D5C10.course = lbl_1_bss_D5C70[i].unk11;
                    lbl_1_bss_D5D90 = 0;
                    break;
                }
                lbl_1_bss_D5C10.rank = 0;
            }
            if (lbl_1_bss_D5C10.rank == 0)
                lbl_1_bss_D5D90 = 1;
        }
    }
    if (lbl_1_bss_D5C10.unk0 == 0)
    {
        if (modeCtrl.courseFlags & 0x800000)
            lbl_1_bss_D5C10.course = 3;
        else if (modeCtrl.courseFlags & 0x10)
            lbl_1_bss_D5C10.course = 2;
        else if (modeCtrl.courseFlags & 8)
            lbl_1_bss_D5C10.course = 1;
        else
            lbl_1_bss_D5C10.course = 0;
    }
    if (lbl_1_bss_D5C10.unk0 == 2)
    {
        int i;

        for (i = 0; i < 5; i++)
        {
            strcpy(lbl_1_bss_D5C70[i].name, lbl_1_data_6BA68[i].name);
            lbl_1_bss_D5C70[i].score = lbl_1_data_6BA68[i].unk8;
            lbl_1_bss_D5C70[i].unk18 = lbl_1_data_6BA68[i].unkC;
            lbl_1_bss_D5C70[i].unkC = lbl_1_data_6BA68[i].unk10;
        }
        lbl_1_bss_D5C10.score = get_storymode_score();
        lbl_1_bss_D5C10.unk3E = (s16)get_storymode_banana_count();
        lbl_1_bss_D5C10.unk40 = fn_2_8D98() / 60;
        lbl_1_bss_D5D60.score = lbl_1_bss_D5C10.score;
        lbl_1_bss_D5D60.unk18 = lbl_1_bss_D5C10.unk3E;
        lbl_1_bss_D5D60.unkC = lbl_1_bss_D5C10.unk40;
        lbl_1_bss_D5C10.rank = (s8)fn_1_11A28C(lbl_1_bss_D5D60);
        if (lbl_1_bss_D5C10.rank == 0)
        {
            lbl_1_bss_D5C10.unk8 = 6;
            lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 5;
            camera_set_state(3);
            g_current_camera->eye = lbl_1_data_6BE68;
            g_current_camera->lookAt = lbl_1_data_6BE74;
        }
    }
    fn_1_11BC50();
    if (lbl_1_bss_D5C10.unk8 == 6)
    {
        camera_set_state(3);
        g_current_camera->eye = lbl_1_data_6BE68;
        g_current_camera->lookAt = lbl_1_data_6BE74;
    }
    lbl_1_bss_D5C6C = 1;
    set_sub_mode_destination(fn_1_11C580);
}

void fn_1_11BC50(void)
{
    int i;
    char **names;
    void **out;

    lbl_1_bss_D5878 = 0;
    g_fade_track_volume(0x30, 0);
    enable_cameras_for_active_players();
    camera_set_state(3);
    for (i = 0; i < 4; i++)
        set_camera_view_settings(i, 0.0f, 0.0f, 1.0f, 1.0f);
    names = lbl_1_data_6C200;
    out = lbl_1_bss_D5ECC;
    for (i = 0x2D; i > 0; i--, names++, out++)
    {
        if (*names == NULL)
            *out = NULL;
        else
            *out = fn_1_5D6F4(*names);
    }
    lbl_1_bss_D5878 = 0;
    fn_1_122FA4();
    lbl_1_bss_D5FA4 = 0;
    for (i = 0; i < 6; i++)
        lbl_1_bss_D5FA8[i] = 7;
    lbl_1_bss_D5FC0.unk0 = 0;
    lbl_1_bss_D5FC0.unk2 = 0;
    lbl_1_bss_D5FC0.unk4 = 0;
    lbl_1_bss_D5FC0.unk5 = 0;
    lbl_1_bss_D5FC0.unk6 = 0;
    lbl_1_bss_D5FC0.unk7 = 0;
    lbl_1_bss_D5FC0.unk8 = 0;
    lbl_1_bss_D5FC0.unkC = 0;
    lbl_1_bss_D5FC0.unk10 = 0;
    lbl_1_bss_D5FC0.unk14 = 1;
    lbl_1_bss_D5881 = 0;
    lbl_1_data_6BDCC.x = 0.0f;
    lbl_1_data_6BDCC.y = 6.5f;
    lbl_1_data_6BDCC.z = -4.0f;
    lbl_1_bss_D5FD8 = 0;
    lbl_1_bss_D5FDC = 0;
    lbl_1_bss_D5FDD = 0;
    for (i = 0; i < 5; i++)
    {
        lbl_1_bss_D5E90[i].unk4 = (u8)((char *)lbl_1_data_6C03C)[i];
        lbl_1_bss_D5E90[i].unk8 = lbl_1_bss_D5ECC[i];
        lbl_1_bss_D5E90[i].unk0 = i;
        lbl_1_bss_D5F80[i] = lbl_1_bss_D5E90[i].unk0;
    }
    lbl_1_bss_D5888 = 0;
    for (i = 0; i > 0x2D; i++)
        lbl_1_bss_D5E90[i].unk0 = i;
    lbl_1_bss_D5F94 = lbl_1_bss_D5E90[2];
    for (i = 0; i < 5; i++)
    {
        lbl_1_bss_D5FE0[i] = 0;
        lbl_1_data_6BACC[i] = 0.0f;
    }
    lbl_1_bss_D5C10.unk4 = 0;
    lbl_1_bss_D5C10.unk9 = 0;
    lbl_1_bss_D5C10.unk38 = 0;
    lbl_1_bss_D5C10.unk3B = 0;
    lbl_1_bss_D5C10.unk3A = 0;
    lbl_1_bss_D5C10.unk45 = 0;
    if (lbl_1_bss_D5C10.rank == 0)
    {
        lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 5;
        lbl_1_bss_D5C10.unk8 = 6;
    }
    else
        lbl_1_bss_D5C10.unk8 = 0;
    fn_1_122594();
    fn_1_122C60();
    g_current_camera->unk128.x = g_current_camera->unk128.y = g_current_camera->unk128.z = 0.0f;
    g_current_camera->unk140.x = g_current_camera->unk140.y = g_current_camera->unk140.z = 0.0f;
    {
        int j;

        for (j = 0; j < 5; j++)
            printf("ne_data[%d]:%s\n", j, lbl_1_bss_D5C70[j].name);
    }
}

void fn_1_11C02C(void)
{
    if (locale == 5)
        ape_assignment();
    else if (lbl_1_bss_D5C10.unk0 == 2)
        ape_assignment();
    else if (lbl_1_bss_D5C10.unk0 == 0)
        ape_assignment();
}

void game_nameentry_draw_func(void)
{
    Vec v;
    int i;

    camera_apply_viewport_if_changed(modeCtrl.currPlayer);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(modeCtrl.currPlayer);
    g_draw_stage();
    g_draw_bg();
    g_draw_effects();
    mtxa_from_mtxb();
    mtxa_rotate_z(lbl_1_bss_D5FC0.unk2);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(stage_gma->modelEntries[0x2F].model);
    v.x = v.y = 0.0f;
    v.z = 0.1f;
    if ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[0].button & 0x100)
     && lbl_1_bss_D5FC0.unk4 != 0
     && (lbl_1_bss_D5C10.unk8 == 3 || lbl_1_bss_D5C10.unk8 == 0xF))
        v.z = -0.02f;
    if (lbl_1_bss_D5FDC != 0)
        v.z = -0.02f;
    mtxa_from_mtxb();
    mtxa_translate(&v);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(stage_gma->modelEntries[10].model);
    {
        Vec v2;
        s16 angles[5] = { 0x3000, 0x1800, 0, -0x1800, -0x3000 };

        for (i = 0; i < 5; i++)
        {
            v2.x = v2.y = v2.z = 0.0f;
            mtxa_from_mtxb_translate(&v2);
            mtxa_rotate_z(lbl_1_bss_D5FC0.unk0 + angles[i]);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(lbl_1_bss_D5E90[i].unk8);
        }
    }
    fn_1_122ABC(lbl_1_bss_D5FF0);
    for (i = 0; i < 0x3C; i++)
    {
        avdisp_set_alpha(1.0f);
        if (lbl_1_bss_D6A10[i] != 0)
        {
            mtxa_from_mtxb_translate(&lbl_1_bss_D6B00[i].pos);
            mtxa_scale_s(1.5f);
            mtxa_rotate_z(lbl_1_bss_D6B00[i].rotZ);
            mtxa_rotate_y(lbl_1_bss_D6B00[i].rotY);
            mtxa_rotate_x(lbl_1_bss_D6B00[i].rotX);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(stage_gma->modelEntries[lbl_1_bss_D6A10[i]].model);
        }
        lbl_1_bss_D6B00[i].colorId = 0;
        lbl_1_bss_D6B00[i].unk179 = 2;
        mtxa_from_mtxb_translate(&lbl_1_bss_D6B00[i].pos);
        mtxa_scale_s(1.7f);
        mtxa_rotate_z(lbl_1_bss_D6B00[i].rotZ);
        mtxa_rotate_y(lbl_1_bss_D6B00[i].rotY);
        mtxa_rotate_x(lbl_1_bss_D6B00[i].rotX);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        fn_1_4EAFC(mtxa->mtxA, lbl_1_bss_D6B00[i].colorId, lbl_1_bss_D6B00[i].unk179);
    }
    for (i = 0; i < 6; i++)
    {
        if (lbl_1_bss_DD040[i] != NULL)
            draw_ape(lbl_1_bss_DD040[i]);
    }
    if (lbl_1_bss_D5C10.unk0 == 0)
    {
        switch (lbl_1_bss_D5C10.unk8)
        {
        case 2:
        case 3:
        case 4:
            fn_1_127390();
            fn_1_1285E8();
            break;
        case 5:
            fn_1_127390();
            fn_1_12862C();
            fn_1_129BDC();
            break;
        case 6:
        case 7:
            fn_1_12862C();
            fn_1_129BDC();
            break;
        }
    }
    else if (lbl_1_bss_D5C10.unk0 == 2)
    {
        switch (lbl_1_bss_D5C10.unk8)
        {
        case 2:
        case 3:
        case 4:
            fn_1_126264();
            break;
        case 5:
            fn_1_126264();
            fn_1_1288FC();
            break;
        case 6:
            fn_1_1288FC();
            break;
        }
    }
    ord_tbl_draw_nodes();
    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(stage_gma->modelEntries[0x5F].model);
}

void fn_1_11C580(void)
{
    switch (sub_mode)
    {
    case 0x3F:
    case 0x40:
    case 0xC3:
    case 0xC4:
        set_sub_mode_destination(fn_1_11C580);
        break;
    default:
        fn_1_11C5E4();
        break;
    }
}

void fn_1_11C5E4(void)
{
    int i;

    g_related_to_pause_menu_input.flags &= ~2;
    if (sub_mode != 0x40 && sub_mode != 0xC4)
    {
        destroy_sprite_with_unique_id(0x65);
        for (i = 0; i < 6; i++)
        {
            destroy_sprite_with_unique_id(i + 0x16);
            destroy_sprite_with_unique_id(i + 0x34);
        }
    }
    else
        destroy_sprite_with_unique_id(0);
    for (i = 0; i < 6; i++)
    {
        g_something_with_freeing_chara_heap_ape(lbl_1_bss_DD040[i]);
        lbl_1_bss_DD040[i] = NULL;
    }
    g_fade_track_volume(0x30, 1);
    call_free_bmp_by_id(3);
    dest_all_events();
    if (g_some_other_flags & 4)
        printf("nameentry2 dest\n");
}

static void ape_assignment_reset_a(void)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        lbl_1_bss_DD058[i] = 0;
        lbl_1_bss_D5FA8[i] = 7;
    }
}

static void ape_assignment_reset_b(void)
{
    int i;

    for (i = 0; i < 5; i++)
    {
        lbl_1_bss_D5E90[i].unk4 = (u8)((char *)lbl_1_data_6C03C)[i];
        lbl_1_bss_D5E90[i].unk8 = lbl_1_bss_D5ECC[i];
        lbl_1_bss_D5E90[i].unk0 = i;
        lbl_1_bss_D5F80[i] = lbl_1_bss_D5E90[i].unk0;
    }
    lbl_1_bss_D5F94 = lbl_1_bss_D5E90[2];
}

void ape_assignment(void)
{
    int i;

    if (g_some_other_flags & 0xA)
        return;
    if (pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x800)
    {
        if (g_some_other_flags & 4)
            printf("gumball_geom\n");
        for (i = 0; i < 0x3C; i++)
            printf("{ %f,%f,%f },\n", lbl_1_bss_D6B00[i].pos.x, lbl_1_bss_D6B00[i].pos.y, lbl_1_bss_D6B00[i].pos.z);
    }
    fn_1_12264C(lbl_1_bss_D5FF0);
    fn_1_12342C();
    switch (lbl_1_bss_D5C10.unk8)
    {
    case 0:
        camera_set_state(3);
        g_current_camera->eye = lbl_1_data_6BDD8;
        g_current_camera->lookAt = lbl_1_data_6BDE4;
        lbl_1_bss_D5FC0.unk4 = 0;
        ape_assignment_reset_a();
        ape_assignment_reset_b();
        lbl_1_bss_D5FA4 = 0;
        lbl_1_bss_D5C10.unk9 = 0;
        lbl_1_bss_D5C10.unk4 = 7.5 * lbl_80198B38;
        lbl_1_bss_D5C10.unk8 = 1;
        g_current_camera->unk11C = lbl_1_data_6BDF0;
        g_current_camera->unk134 = lbl_1_data_6BDFC;
        fn_1_123E94(1.5 * lbl_80198B38);
        break;
    case 1:
        lbl_1_bss_D5C10.unk4--;
        if (lbl_1_bss_D5C10.unk4 > 6.0 * lbl_80198B38)
            fn_1_1240C8();
        if (lbl_1_bss_D5C10.unk4 == 6.0 * lbl_80198B38)
        {
            fn_1_1241E8();
            g_current_camera->unk11C = lbl_1_data_6BE08;
            g_current_camera->unk134 = lbl_1_data_6BE14;
            fn_1_123E94(0.9 * lbl_80198B38);
        }
        if (lbl_1_bss_D5C10.unk4 > 4.0 * lbl_80198B38 && lbl_1_bss_D5C10.unk4 < 4.9 * lbl_80198B38)
            fn_1_1240C8();
        if (lbl_1_bss_D5C10.unk4 == 4.0 * lbl_80198B38)
        {
            fn_1_1242CC();
            g_current_camera->unk11C = lbl_1_data_6BE20;
            g_current_camera->unk134 = lbl_1_data_6BE2C;
            fn_1_123E94(0.5 * lbl_80198B38);
        }
        if (2.5 * lbl_80198B38 >= lbl_1_bss_D5C10.unk4 && lbl_1_bss_D5C10.unk4 > 2.0 * lbl_80198B38)
            fn_1_1240C8();
        if (lbl_1_bss_D5C10.unk4 == 2.0 * lbl_80198B38)
        {
            g_display_playpoint_or_gift_message_child(1, 0x14, 0);
            fn_1_1243A8();
        }
        if (lbl_1_bss_D5FDD == 0)
        {
            if (lbl_1_bss_D5C10.unk4 == 4.0 * lbl_80198B38)
                lbl_1_bss_D5FC0.unkC = 1;
            if (lbl_1_bss_D5C10.unk4 == 3.5 * lbl_80198B38)
                lbl_1_bss_D5FC0.unkC = 1;
            if (lbl_1_bss_D5C10.unk4 == 3.0 * lbl_80198B38)
                lbl_1_bss_D5FC0.unkC = -1;
            if (lbl_1_bss_D5C10.unk4 == 2.5 * lbl_80198B38)
                lbl_1_bss_D5FC0.unkC = -1;
            fn_1_121188();
        }
        else
        {
            lbl_1_bss_D5FC0.unk0 = 0;
            lbl_1_bss_D5FC0.unk2 = 0;
            lbl_1_bss_D5FC0.unk4 = 0;
            lbl_1_bss_D5FC0.unk5 = 0;
            lbl_1_bss_D5FC0.unkC = 0;
            lbl_1_bss_D5FC0.unk10 = 0;
            lbl_1_bss_D5FC0.unk14 = 1;
            lbl_1_bss_D5881 = 0;
            ape_assignment_reset_b();
        }
        if (1.8 * lbl_80198B38 >= lbl_1_bss_D5C10.unk4 && lbl_1_bss_D5C10.unk4 > lbl_80198B38)
        {
            if (lbl_1_bss_D5C10.unk4 == 1.5 * lbl_80198B38)
            {
                lbl_1_bss_D5FDC = 1;
                call_SoundReqID_arg_0(0x10);
            }
            if (lbl_1_bss_D5C10.unk4 == 1.1 * lbl_80198B38)
                lbl_1_bss_D5FDC = 0;
        }
        if (pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x100)
        {
            if (4.0 * lbl_80198B38 > lbl_1_bss_D5C10.unk4 && lbl_1_bss_D5C10.unk4 > 2.0 * lbl_80198B38)
            {
                fn_1_1241E8();
                lbl_1_bss_D5C10.unk4 = 1.0 + 2.0 * lbl_80198B38;
                lbl_1_bss_D5FDD = 1;
            }
            else if (2.0 * lbl_80198B38 >= lbl_1_bss_D5C10.unk4)
            {
                fn_1_1241E8();
                lbl_1_bss_D5C10.unk4 = 1;
            }
        }
        if (lbl_1_bss_D5C10.unk4 < 0)
            lbl_1_bss_D5C10.unk4 = 0;
        if (lbl_1_bss_D5C10.unk4 == 0)
        {
            g_display_playpoint_or_gift_message_child(1, 0x14, 0);
            lbl_1_bss_D5FA8[0] = 0;
            lbl_1_bss_D5FDC = 0;
            lbl_1_bss_D5FDD = 0;
            lbl_1_bss_D5C10.unk4 = lbl_80198B38;
            lbl_1_bss_D5C10.unk8 = 2;
            g_current_camera->unk11C = lbl_1_data_6BE38;
            g_current_camera->unk134 = lbl_1_data_6BE44;
            fn_1_123E94(lbl_80198B38);
        }
        break;
    case 2:
        fn_1_1240C8();
        lbl_1_bss_D5C10.unk4--;
        if (lbl_1_bss_D5C10.unk4 == 0)
        {
            lbl_1_bss_D5FC0.unk4 = 1;
            lbl_1_bss_D5C10.unk8 = 3;
            lbl_1_bss_D5C10.unk4 = lbl_80198B38;
        }
        break;
    case 3:
        camera_set_state(3);
        g_current_camera->eye = lbl_1_data_6BE38;
        g_current_camera->lookAt = lbl_1_data_6BE44;
        lbl_1_bss_D5FC0.unk4 = 1;
        fn_1_120DC0();
        if (lbl_1_bss_D5C10.unk4 == 0)
        {
            g_current_camera->unk11C = lbl_1_data_6BE50;
            g_current_camera->unk134 = lbl_1_data_6BE5C;
            fn_1_123E94(lbl_80198B38);
            lbl_1_bss_D5C10.unk4 = 0;
            lbl_1_bss_D5C10.unk8 = 4;
        }
        break;
    case 4:
    {
        struct NameEntryData *dest;
        int j;

        if (g_some_other_flags & 4)
            printf("ne_prohibited_check\n");
        fn_1_120D48(lbl_1_bss_D5C10.name);
        if (lbl_1_bss_D5C10.unk0 == 0)
        {
            strcpy(lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].name, (char *)lbl_1_bss_D5C10.name);
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].unk14 = lbl_1_bss_D5C10.apeType;
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].unk10 = (u8)lbl_1_bss_D5C10.floor;
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].score = lbl_1_bss_D5C10.score;
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].unk11 = lbl_1_bss_D5C10.course;
            for (j = 0; j < 5; j++)
            {
                if (modeCtrl.currPlayer == lbl_1_bss_D5C70[j].unk12)
                    strcpy(lbl_1_bss_D5C70[j].name, (char *)lbl_1_bss_D5C10.name);
            }
            dest = lbl_1_data_6BA58[fn_1_11A1B8()];
            for (j = 0; j < 5; j++)
            {
                strcpy(dest[j].name, lbl_1_bss_D5C70[j].name);
                dest[j].unk14 = lbl_1_bss_D5C70[j].unk14;
                dest[j].score = lbl_1_bss_D5C70[j].score;
                dest[j].unk10 = lbl_1_bss_D5C70[j].unk10;
                dest[j].unk11 = lbl_1_bss_D5C70[j].unk11;
            }
        }
        else if (lbl_1_bss_D5C10.unk0 == 2)
        {
            strcpy(lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].name, (char *)lbl_1_bss_D5C10.name);
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].score = lbl_1_bss_D5C10.score;
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].unk18 = lbl_1_bss_D5C10.unk3E;
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].unkC = lbl_1_bss_D5C10.unk40;
            for (j = 0; j < 5; j++)
            {
                strcpy(lbl_1_data_6BA68[j].name, lbl_1_bss_D5C70[j].name);
                lbl_1_data_6BA68[j].unk8 = lbl_1_bss_D5C10.score;
                lbl_1_data_6BA68[j].unkC = lbl_1_bss_D5C10.unk3E;
                lbl_1_data_6BA68[j].unk10 = lbl_1_bss_D5C10.unk40;
            }
        }
        printf("prohibited:\n ne_data[%d]ape_type:%d\tscore:%d\tname:%s\n", lbl_1_bss_D5C10.rank - 1,
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].unk14, lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].score,
            lbl_1_bss_D5C70[lbl_1_bss_D5C10.rank - 1].name);
        lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 2;
        lbl_1_bss_D5C10.unk8 = 5;
        break;
    }
    case 5:
        lbl_1_bss_D5C10.unk4--;
        lbl_1_bss_D5FC0.unk4 = 0;
        if (lbl_1_bss_D5C10.unk4 < lbl_80198B38)
            fn_1_1241E8();
        else if (lbl_1_bss_D5C10.unk4 < lbl_80198B38 * 2)
            fn_1_1240C8();
        if (lbl_1_bss_D5C10.unk4 == 0)
        {
            printf("input_end\n");
            lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 5;
            lbl_1_bss_D5C10.unk8 = 6;
        }
        break;
    case 6:
        if (lbl_1_bss_D5C10.unk4 > 0)
            lbl_1_bss_D5C10.unk4--;
        if (lbl_1_bss_D5C10.rank == 0)
        {
            float r = 15.0f;

            camera_set_state(3);
            g_current_camera->lookAt = lbl_1_data_6BE74;
            g_current_camera->eye.x = r * math_sin(16384.0 + 3.1415927410125732 * ((lbl_1_bss_D5C10.unk4 << 6) / 25.0));
            g_current_camera->eye.z = r;
            g_current_camera->eye.y = 5.0 + mathutil_cos(lbl_1_bss_D5C10.unk4 << 6);
        }
        if (lbl_1_bss_D5C10.unk4 == 0
         || ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x100)
             && lbl_1_bss_D5C10.unk4 < 3.5 * lbl_80198B38))
        {
            lbl_1_bss_D5C10.unk4 = 0;
            fade_screen_to_color(0x101, 0xFFFFFF, 0x3C);
            fn_1_11979C(lbl_1_bss_D5C70);
            lbl_1_bss_D5C10.unk4 = 0x3C;
            lbl_1_bss_D5C10.unk8 = 7;
        }
        break;
    case 7:
        lbl_1_bss_D5C10.unk4--;
        if (lbl_1_bss_D5C10.unk4 == 0)
            modeCtrl.submodeTimer = 0;
        break;
    }
    for (i = 0; i < 0x3C; i++)
        lbl_1_bss_D6B00[i].prevPos = lbl_1_bss_D6B00[i].pos;
    if (!((g_font_char_var_3 % 10) / 10.0))
    {
        lbl_1_bss_D587C++;
        if (lbl_1_bss_D587C == 6)
            lbl_1_bss_D587C = 0;
        lbl_1_data_6BAE0[0]++;
        if (lbl_1_data_6BAE0[0] == 6)
            lbl_1_data_6BAE0[0] = 0;
    }
}

void fn_1_11DE54(int a, void *b)
{
    int i;

    if (main_mode != 1)
    {
        fn_1_36140();
        g_fade_track_volume(0x30, 0);
    }
    if (main_mode == 1)
        lbl_1_bss_D5C10.unk8 = 0x15;
    g_related_to_pause_menu_input.flags |= 2;
    destroy_sprite_with_unique_id(0x65);
    switch (a)
    {
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
        for (i = 5; i < 6; i++)
        {
            destroy_sprite_with_unique_id(i + 0x16);
            destroy_sprite_with_unique_id(i + 0x34);
        }
        break;
    }
    ((void (*)(int, void *))lbl_1_data_6C2B4[a].fn)(a, b);
}

int fn_1_11DF64(int a, u8 *b, int c)
{
    int i;
    int ret;

    lbl_1_bss_D5878 = 1;
    lbl_1_bss_DD070 = (s8)c;
    fn_1_11E3F8();
    lbl_1_bss_D5885 = (lbl_1_bss_DD070 == 1) ? (s8)1 : (s8)0;
    switch (a)
    {
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        a -= 6;
        break;
    }
    ret = 0;
    for (i = 0; i < c; i++)
    {
        switch (a)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            ret |= fn_1_13DE08(a, (void *)b, i, 0);
            b += 0xC;
            break;
        case 12:
        case 13:
        case 14:
        case 15:
            ret |= fn_1_13E3BC(a, (void *)b, i, 0);
            b += 0x10;
            break;
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
            ret |= fn_1_13E64C(a, (void *)b, i, 0);
            b += 0xA;
            break;
        case 23:
            ret |= fn_1_13E908(a, (void *)b, i, 0);
            b += 0xC;
            break;
        case 24:
        case 25:
        case 26:
            ret |= fn_1_13EC0C(a, (void *)b, i, 0);
            b += 0xB;
            break;
        case 27:
        case 28:
        case 29:
            ret |= fn_1_13F174(a, (void *)b, i, 0);
            b += 0xC;
            break;
        case 30:
        case 31:
        case 32:
            ret |= fn_1_13F3F4(a, (void *)b, i, 0);
            b += 0x10;
            break;
        }
    }
    switch (a)
    {
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        a -= 6;
        break;
    }
    switch (a)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        if (fn_1_13E17C(a, (void *)b, c, 0) != 0)
            ret = 1;
        break;
    case 24:
    case 25:
    case 26:
        if (fn_1_13EF3C(a, (void *)b, c, 0) != 0)
            ret = 1;
        break;
    }
    return ret;
}

int fn_1_11E1C0(int a, void *b)
{
    int x;
    int y;

    lbl_1_bss_D5C10.unk52 = 0;
    switch (a)
    {
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        a -= 6;
        break;
    }
    switch (a)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        x = fn_1_13DE08(a, (void *)b, 0, 1);
        y = fn_1_13E17C(a, (void *)b, 1, 1);
        if (x >= 0 && y >= 0)
            return x;
        if (y < 0)
            return x;
        return y;
    case 12:
    case 13:
    case 14:
    case 15:
        return fn_1_13E3BC(a, (void *)b, 0, 1);
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
        return fn_1_13E64C(a, (void *)b, 0, 1);
    case 23:
        return fn_1_13E908(a, (void *)b, 0, 1);
    case 24:
    case 25:
    case 26:
        x = fn_1_13EC0C(a, (void *)b, 0, 1);
        y = fn_1_13EF3C(a, (void *)b, 1, 1);
        if (x >= 0 && y >= 0)
            return x;
        if (y < 0)
            return x;
        return y;
    case 27:
    case 28:
    case 29:
        return fn_1_13F174(a, (void *)b, 0, 1);
    case 30:
    case 31:
    case 32:
        return fn_1_13F3F4(a, (void *)b, 0, 1);
    }
    return -1;
}

int fn_1_11E3A4(void)
{
    if ((lbl_1_bss_D5C10.unk8 == 0x15 && lbl_1_bss_D5C10.unk4 < 0xF0) || lbl_1_bss_D5C10.unk8 == 0x17)
        return 0;
    return -1;
}

void fn_1_11E3F8(void)
{
    int i;
    char **names;
    void **out;

    lbl_1_bss_D5C10.unk58[0] = -1;
    for (i = 0; i < 4; i++)
        lbl_1_bss_D5D98[i] = -1;
    for (i = 0; i < 5; i++)
        lbl_1_bss_D5C70[i].unk12 = -1;
    if (sub_mode != 0xC3 && sub_mode != 0x3F)
    {
        load_stage(200);
        fade_screen_to_color(0x100, 0xFFFFFF, 0x3C);
        enable_cameras_for_active_players();
        camera_set_state(3);
        names = lbl_1_data_6C200;
        out = lbl_1_bss_D5ECC;
        for (i = 0x2D; i > 0; i--, names++, out++)
        {
            if (*names == NULL)
                *out = NULL;
            else
                *out = fn_1_5D6F4(*names);
        }
        lbl_1_bss_D5FC0.unk0 = 0;
        lbl_1_bss_D5FC0.unk2 = 0;
        lbl_1_bss_D5FC0.unk4 = 0;
        lbl_1_bss_D5FC0.unk5 = 0;
        lbl_1_bss_D5FC0.unk6 = 0;
        lbl_1_bss_D5FC0.unk7 = 0;
        lbl_1_bss_D5FC0.unk8 = 0;
        lbl_1_bss_D5FC0.unkC = 0;
        lbl_1_bss_D5FC0.unk10 = 0;
        lbl_1_bss_D5881 = 0;
        lbl_1_data_6BDCC.x = 0.0f;
        lbl_1_data_6BDCC.y = 6.5f;
        lbl_1_data_6BDCC.z = -4.0f;
        lbl_1_bss_D5FD8 = 0;
        lbl_1_bss_D5FDC = 0;
        lbl_1_bss_D5FDD = 0;
        for (i = 0; i < 5; i++)
        {
            lbl_1_bss_D5E90[i].unk4 = (u8)((char *)lbl_1_data_6C03C)[i];
            lbl_1_bss_D5E90[i].unk8 = lbl_1_bss_D5ECC[i];
            lbl_1_bss_D5E90[i].unk0 = i;
            lbl_1_bss_D5F80[i] = lbl_1_bss_D5E90[i].unk0;
        }
        lbl_1_bss_D5888 = 0;
        for (i = 0; i > 0x2D; i++)
            lbl_1_bss_D5E90[i].unk0 = i;
        lbl_1_bss_D5F94 = lbl_1_bss_D5E90[2];
        for (i = 0; i < 5; i++)
        {
            lbl_1_bss_D5FE0[i] = 0;
            lbl_1_data_6BACC[i] = 0.0f;
        }
        fn_1_122594();
        fn_1_122C60();
        g_current_camera->unk128.x = g_current_camera->unk128.y = g_current_camera->unk128.z = 0.0f;
        g_current_camera->unk140.x = g_current_camera->unk140.y = g_current_camera->unk140.z = 0.0f;
    }
    lbl_1_bss_D5C10.unk52 = 0;
    lbl_1_bss_D5C10.unk9 = 0;
    lbl_1_bss_D5C10.unk4 = 0;
    lbl_1_bss_D5C10.unk3A = 0;
    lbl_1_bss_D5C10.unk45 = 0;
    strcpy((char *)lbl_1_bss_D5C10.name, "      ");
    lbl_1_bss_D5878 = 1;
    lbl_1_bss_D5884 = 0;
    lbl_1_bss_D5C10.unk8 = 8;
}

void fn_1_11E790(int a, u32 b)
{
    int i;

    if (main_mode == 1 && sub_mode == 0x20)
    {
        lbl_1_bss_D5C10.unk4 = 6000;
        switch (a)
        {
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            a -= 6;
            break;
        }
        switch (a)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        {
            struct NERankA *rows = lbl_1_data_6C2B4[a].ptr;
            struct NERankA *one = lbl_1_data_6CCE4[a];

            for (i = 0; i < 5; i++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rows[i].name);
                lbl_1_bss_D5C70[i].unk14 = rows[i].ape;
                lbl_1_bss_D5C70[i].unk24 = rows[i].unk8;
                lbl_1_bss_D5C70[i].unk25 = rows[i].unk9;
                lbl_1_bss_D5C70[i].unk26 = rows[i].unkA;
                lbl_1_bss_D5C70[i].unk2C = rows[i].unkB;
            }
            strcpy(lbl_1_bss_D5E60.name, one->name);
            lbl_1_bss_D5E60.unk24 = one->unk8;
            lbl_1_bss_D5E60.unk25 = one->unk9;
            lbl_1_bss_D5E60.unk26 = one->unkA;
            lbl_1_bss_D5E60.apeType = one->ape;
            lbl_1_bss_D5E60.unk2C = one->unkB;
            break;
        }
        case 12:
        case 13:
        case 14:
        case 15:
        {
            struct NERankB *rows = lbl_1_data_6C2B4[a].ptr;

            for (i = 0; i < 5; i++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rows[i].name);
                lbl_1_bss_D5C70[i].unk14 = rows[i].ape;
                lbl_1_bss_D5C70[i].score = rows[i].score;
            }
            break;
        }
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        {
            struct NERankC *rows = lbl_1_data_6C2B4[a].ptr;

            for (i = 0; i < 5; i++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rows[i].name);
                lbl_1_bss_D5C70[i].unk14 = rows[i].ape;
                lbl_1_bss_D5C70[i].score = rows[i].unk8;
            }
            break;
        }
        case 23:
        {
            struct NERankD *rows = lbl_1_data_6C2B4[a].ptr;

            for (i = 0; i < 5; i++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rows[i].name);
                lbl_1_bss_D5C70[i].unk14 = rows[i].ape;
                lbl_1_bss_D5C70[i].unk28 = rows[i].unk8;
                lbl_1_bss_D5C70[i].unk2A = rows[i].unkA;
            }
            break;
        }
        case 24:
        case 25:
        case 26:
        {
            struct NERankE *rows = lbl_1_data_6C2B4[a].ptr;
            struct NERankE *one = lbl_1_data_6CCFC[a - 24];

            for (i = 0; i < 5; i++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rows[i].name);
                lbl_1_bss_D5C70[i].unk14 = rows[i].ape;
                lbl_1_bss_D5C70[i].unk24 = rows[i].unk8;
                lbl_1_bss_D5C70[i].unk25 = rows[i].unk9;
                lbl_1_bss_D5C70[i].unk26 = rows[i].unkA;
            }
            strcpy(lbl_1_bss_D5E60.name, one->name);
            lbl_1_bss_D5E60.unk24 = one->unk8;
            lbl_1_bss_D5E60.unk25 = one->unk9;
            lbl_1_bss_D5E60.unk26 = one->unkA;
            lbl_1_bss_D5E60.apeType = one->ape;
            break;
        }
        case 27:
        case 28:
        case 29:
        {
            struct NERankF *rows = lbl_1_data_6C2B4[a].ptr;

            for (i = 0; i < 5; i++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rows[i].name);
                lbl_1_bss_D5C70[i].unk14 = rows[i].ape;
                lbl_1_bss_D5C70[i].score = rows[i].score;
            }
            break;
        }
        case 30:
        case 31:
        case 32:
        {
            struct NERankB *rows = lbl_1_data_6C2B4[a].ptr;

            for (i = 0; i < 5; i++)
            {
                strcpy(lbl_1_bss_D5C70[i].name, rows[i].name);
                lbl_1_bss_D5C70[i].unk14 = rows[i].ape;
                lbl_1_bss_D5C70[i].score = rows[i].score;
            }
            break;
        }
        }
        lbl_1_bss_D5C10.unk0 = 1;
        lbl_1_bss_D5C10.unk50 = (s8)a;
        lbl_1_bss_D5C10.unk54 |= b;
        lbl_1_bss_D5C10.unk3A = 0;
    }
    else
    {
        int j;

        load_bmp_by_id(3);
        lbl_1_bss_D5C10.unk0 = 1;
        lbl_1_bss_D5C10.unk50 = (s8)a;
        for (j = 0; j < 5; j++)
            lbl_1_bss_D5C70[j].unk20 = -1;
        {
            int k;
            for (k = lbl_1_bss_DD070 - 1; k >= 0; k--)
            {
                if (lbl_1_bss_D5D98[k] > 0)
                    lbl_1_bss_D5D9C = (s8)k;
            }
        }
        strcpy((char *)lbl_1_bss_D5C10.name, "      ");
        printf("player_id:%d\n", lbl_1_bss_D5D9C);
        lbl_1_bss_D5C10.rank = (s8)lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk20;
        lbl_1_bss_D5C10.apeType = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
        lbl_1_bss_D5C10.unk54 |= b;
        lbl_1_bss_D5C10.unk3A = 0;
        lbl_1_bss_D5883 = 0;
        lbl_1_bss_D5878 = 1;
        fn_1_122FA4();
        lbl_1_bss_D5FA4 = 0;
        for (j = 0; j < 6; j++)
            lbl_1_bss_D5FA8[j] = 7;
        switch (a)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            lbl_1_bss_D5C10.unk10 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk24;
            lbl_1_bss_D5C10.unk11 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk25;
            lbl_1_bss_D5C10.unk12 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk26;
            lbl_1_bss_D5C10.unk44 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk2C;
            break;
        case 24:
        case 25:
        case 26:
            lbl_1_bss_D5C10.unk10 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk24;
            lbl_1_bss_D5C10.unk11 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk25;
            lbl_1_bss_D5C10.unk12 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk26;
            break;
        case 23:
            lbl_1_bss_D5C10.unk16 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk2A;
            lbl_1_bss_D5C10.unk14 = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].unk28;
            break;
        default:
            lbl_1_bss_D5C10.score = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].score;
            break;
        }
        if (b == 0)
        {
            lbl_1_bss_D5D90 = 1;
            lbl_1_bss_D5C10.unk8 = 0x14;
        }
        if (b != 0)
        {
            lbl_1_bss_D5D90 = 0;
            lbl_1_bss_D5C10.unk8 = 8;
        }
    }
}

void fn_1_11F024(void)
{
    int i;

    if (main_mode == 1)
    {
        lbl_1_bss_D5C10.unk4--;
        lbl_1_bss_D5C10.unk8 = 0x15;
        if (lbl_1_bss_D5C10.unk4 < 0)
            lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 100;
        if (!((g_font_char_var_3 % 10) / 10.0))
        {
            lbl_1_bss_D587C++;
            if (lbl_1_bss_D587C == 6)
                lbl_1_bss_D587C = 0;
            lbl_1_data_6BAE0[0]++;
            if (lbl_1_data_6BAE0[0] == 6)
                lbl_1_data_6BAE0[0] = 0;
        }
    }
    else
    {
        char *unused[0x21] = {
            "RANK2_RACE0", "RANK2_RACE1", "RANK2_RACE2", "RANK2_RACE3", "RANK2_RACE4", "RANK2_RACE5",
            "RANK2_RACE0_R", "RANK2_RACE1_R", "RANK2_RACE2_R", "RANK2_RACE3_R", "RANK2_RACE4_R", "RANK2_RACE5_R",
            "RANK2_TARGET0", "RANK2_TARGET1", "RANK2_TARGET2", "RANK2_TARGET3", "RANK2_BOWL_NML", "RANK2_BOWL_NML_9",
            "RANK2_BOWL_NML_S", "RANK2_BOWL_CHA", "RANK2_BOWL_SPE0", "RANK2_BOWL_SPE1", "RANK2_BOWL_SPE2", "RANK2_GOLF",
            "RANK2_BOAT0", "RANK2_BOAT1", "RANK2_BOAT2", "RANK2_SHOOT0", "RANK2_SHOOT1", "RANK2_SHOOT2",
            "RANK2_DOG0", "RANK2_DOG1", "RANK2_DOG2",
        };

        debug_textdraw_set_pos(1, 0xF);
        fn_1_12342C();
        lbl_1_bss_D5878 = 1;
        if (lbl_1_bss_D5C10.unk8 == 8)
            lbl_1_bss_D5C10.unk8 = 0xB;
        if (lbl_1_bss_D5C10.unk8 == 9)
        {
            if ((s8)g_nameentry_did_get_top_5 != 0)
                lbl_1_bss_D5C10.unk8 = 0xB;
            if ((s8)g_nameentry_did_get_top_5 == 0)
                lbl_1_bss_D5C10.unk8 = 0x15;
        }
        fn_1_12264C(lbl_1_bss_D5FF0);
        switch (lbl_1_bss_D5C10.unk8)
        {
        case 11:
            camera_set_state(3);
            g_current_camera->eye = lbl_1_data_6BDD8;
            g_current_camera->lookAt = lbl_1_data_6BDE4;
            ape_assignment_reset_b();
            lbl_1_bss_D5FC0.unk4 = 0;
            for (i = 0; i < 6; i++)
            {
                lbl_1_bss_DD058[i] = 0;
                lbl_1_bss_D5FA8[i] = 7;
            }
            lbl_1_bss_D5FA4 = 0;
            {
                int k;

                for (k = lbl_1_bss_DD070 - 1; k >= 0; k--)
                {
                    if (lbl_1_bss_D5D98[k] > 0)
                        lbl_1_bss_D5D9C = (s8)k;
                }
            }
            lbl_1_bss_D5C10.rank = lbl_1_bss_D5D98[lbl_1_bss_D5D9C];
            lbl_1_bss_D5C10.unk53 = lbl_1_bss_D5D9C;
            strcpy((char *)lbl_1_bss_D5C10.name, "      ");
            lbl_1_bss_D5C10.unk9 = 0;
            lbl_1_bss_D5C10.unk4 = 3.0 * lbl_80198B38;
            lbl_1_bss_D5C10.unk8 = 0xA;
            g_current_camera->unk11C = lbl_1_data_6BDF0;
            g_current_camera->unk134 = lbl_1_data_6BDFC;
            fn_1_123E94(1.5 * lbl_80198B38);
            break;
        case 10:
            lbl_1_bss_D5C10.unk4--;
            if (lbl_1_bss_D5C10.unk4 == 1.0 * lbl_80198B38)
            {
                fn_1_1241E8();
                g_current_camera->unk11C = lbl_1_data_6BE08;
                g_current_camera->unk134 = lbl_1_data_6BE14;
                fn_1_123E94(1.0 * lbl_80198B38);
            }
            else if (lbl_1_bss_D5C10.unk4 <= 1.5 * lbl_80198B38 && lbl_1_bss_D5C10.unk4 > 1.0 * lbl_80198B38)
                fn_1_1241E8();
            else
                fn_1_1240C8();
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                lbl_1_bss_D5C10.unk8 = 0xC;
                lbl_1_bss_D5C10.unk4 = 4.1 * lbl_80198B38;
            }
            break;
        case 12:
            lbl_1_bss_D5C10.unk4--;
            if (lbl_1_bss_D5C10.unk4 == 4.0 * lbl_80198B38)
            {
                fn_1_1242CC();
                g_current_camera->unk11C = lbl_1_data_6BE20;
                g_current_camera->unk134 = lbl_1_data_6BE2C;
                fn_1_123E94(0.5 * lbl_80198B38);
            }
            if (2.5 * lbl_80198B38 >= lbl_1_bss_D5C10.unk4 && lbl_1_bss_D5C10.unk4 > 2.0 * lbl_80198B38)
                fn_1_1240C8();
            if (lbl_1_bss_D5C10.unk4 == 2.0 * lbl_80198B38)
            {
                g_display_playpoint_or_gift_message_child(1, 0x14, 0);
                fn_1_1243A8();
            }
            if (lbl_1_bss_D5FDD == 0)
            {
                if (lbl_1_bss_D5C10.unk4 == 4.0 * lbl_80198B38)
                    lbl_1_bss_D5FC0.unkC = 1;
                if (lbl_1_bss_D5C10.unk4 == 3.5 * lbl_80198B38)
                    lbl_1_bss_D5FC0.unkC = 1;
                if (lbl_1_bss_D5C10.unk4 == 3.0 * lbl_80198B38)
                    lbl_1_bss_D5FC0.unkC = -1;
                if (lbl_1_bss_D5C10.unk4 == 2.5 * lbl_80198B38)
                    lbl_1_bss_D5FC0.unkC = -1;
                fn_1_121188();
            }
            else
            {
                lbl_1_bss_D5FC0.unk0 = 0;
                lbl_1_bss_D5FC0.unk2 = 0;
                lbl_1_bss_D5FC0.unk4 = 0;
                lbl_1_bss_D5FC0.unk5 = 0;
                lbl_1_bss_D5FC0.unkC = 0;
                lbl_1_bss_D5FC0.unk10 = 0;
                ape_assignment_reset_b();
            }
            if (lbl_1_bss_D5C10.unk4 == 1.5 * lbl_80198B38)
            {
                lbl_1_bss_D5FDC = 1;
                call_SoundReqID_arg_0(0x10);
            }
            if (lbl_1_bss_D5C10.unk4 == 1.1 * lbl_80198B38)
                lbl_1_bss_D5FDC = 0;
            if (pad_status_groups[player_pad_map[lbl_1_bss_D5D9C]].unk0[2].button & 0x100)
            {
                if (4.0 * lbl_80198B38 > lbl_1_bss_D5C10.unk4 && lbl_1_bss_D5C10.unk4 > 2.0 * lbl_80198B38)
                {
                    fn_1_1241E8();
                    lbl_1_bss_D5C10.unk4 = 1.0 + 2.0 * lbl_80198B38;
                    lbl_1_bss_D5FDD = 1;
                }
                else if (2.0 * lbl_80198B38 >= lbl_1_bss_D5C10.unk4)
                {
                    fn_1_1241E8();
                    lbl_1_bss_D5C10.unk4 = 1;
                }
            }
            if (lbl_1_bss_D5C10.unk4 < 0)
                lbl_1_bss_D5C10.unk4 = 0;
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                g_display_playpoint_or_gift_message_child(1, 0x14, 0);
                lbl_1_bss_D5FDC = 0;
                lbl_1_bss_D5FDD = 0;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38;
                lbl_1_bss_D5C10.unk8 = 0xD;
                g_current_camera->unk11C = lbl_1_data_6BE38;
                g_current_camera->unk134 = lbl_1_data_6BE44;
                fn_1_123E94(lbl_80198B38);
            }
            break;
        case 13:
            lbl_1_bss_D5C10.unk4--;
            fn_1_1240C8();
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                lbl_1_bss_D5C10.unk4 = lbl_80198B38;
                lbl_1_bss_D5C10.unk8 = 0xE;
            }
            break;
        case 14:
            camera_set_state(3);
            g_current_camera->eye = lbl_1_data_6BE38;
            g_current_camera->lookAt = lbl_1_data_6BE44;
            lbl_1_bss_D5FC0.unk4 = 1;
            lbl_1_bss_D5FA8[0] = 0;
            lbl_1_bss_DD058[0] = 0;
            {
                int i;

                for (i = 1; i < 6; i++)
                {
                    lbl_1_bss_D5FA8[i] = 7;
                    lbl_1_bss_DD058[i] = 0;
                }
            }
            lbl_1_bss_D5884++;
            lbl_1_bss_D5C10.unk4 = lbl_80198B38;
            lbl_1_bss_D5C10.unk8 = 0xF;
            break;
        case 15:
            camera_set_state(3);
            g_current_camera->eye = lbl_1_data_6BE38;
            g_current_camera->lookAt = lbl_1_data_6BE44;
            lbl_1_bss_D5FC0.unk4 = 1;
            fn_1_120DC0();
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                lbl_1_bss_D5C10.unk8 = 0x10;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 2;
            }
            break;
        case 16:
            lbl_1_bss_D5C10.unk4--;
            if ((lbl_1_bss_D5C10.unk52 == 1 || (lbl_1_bss_D5C10.unk54 & 4)) && lbl_1_bss_D5C10.unk4 == 0)
            {
                fn_1_120D48(lbl_1_bss_D5C10.name);
                lbl_1_bss_D5D9C = 0;
                fn_1_1191F4();
                g_current_camera->unk11C = lbl_1_data_6BE68;
                g_current_camera->unk134 = lbl_1_data_6BE74;
                fn_1_123E94(1.0 * lbl_80198B38);
                lbl_1_bss_D5C10.unk8 = 0x13;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 2;
            }
            else if (lbl_1_bss_D5884 < lbl_1_bss_D5C10.unk52 && lbl_1_bss_D5C10.unk4 == 0 && lbl_1_bss_D5D9C < 3)
            {
                fn_1_120D48(lbl_1_bss_D5C10.name);
                fn_1_1191F4();
                if (g_some_other_flags & 4)
                    printf("next input player search...\n");
                {
                    int i;

                    for (i = lbl_1_bss_D5D9C + 1; i < 4; i++)
                    {
                        if (lbl_1_bss_D5D98[i] > 0)
                        {
                            lbl_1_bss_D5C10.unk53 = (s8)i;
                            break;
                        }
                    }
                    for (i = 0; i < 6; i++)
                        lbl_1_bss_D5FA8[i] = 9;
                }
                lbl_1_bss_D5D9C = lbl_1_bss_D5C10.unk53;
                printf("player_id:%d\n", lbl_1_bss_D5D9C);
                lbl_1_bss_D5C10.unk38 = 0.5 * lbl_80198B38;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38;
                lbl_1_bss_D5C10.unk8 = 0x11;
            }
            else if ((lbl_1_bss_D5884 == lbl_1_bss_D5C10.unk52 || lbl_1_bss_D5C10.unk53 == 3) && lbl_1_bss_D5C10.unk4 == 0)
            {
                fn_1_120D48(lbl_1_bss_D5C10.name);
                fn_1_1191F4();
                g_current_camera->unk11C = lbl_1_data_6BE68;
                g_current_camera->unk134 = lbl_1_data_6BE74;
                fn_1_123E94(lbl_80198B38);
                lbl_1_bss_D5C10.unk8 = 0x13;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 2;
            }
            break;
        case 17:
            lbl_1_bss_D5C10.unk4--;
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                lbl_1_bss_D5FA4 = 0;
                for (i = 0; i < 5; i++)
                {
                    lbl_1_bss_D5E90[i].unk4 = (u8)((char *)lbl_1_data_6C03C)[i];
                    lbl_1_bss_D5E90[i].unk8 = lbl_1_bss_D5ECC[i];
                    lbl_1_bss_D5E90[i].unk0 = i;
                    lbl_1_bss_D5F80[i] = lbl_1_bss_D5E90[i].unk0;
                }
                lbl_1_bss_D5888 = 0;
                lbl_1_bss_D5FC0.unk6 = 0;
                lbl_1_bss_D5FC0.unk10 = 0;
                lbl_1_bss_D5FC0.unkC = 0;
                lbl_1_bss_D5FC0.unk5 = 0;
                lbl_1_bss_D5FC0.unk4 = 0;
                lbl_1_bss_D5FC0.unk7 = 0;
                lbl_1_bss_D5FC0.unk8 = 0;
                lbl_1_bss_D5FC0.unk0 = 0;
                lbl_1_bss_D5FC0.unk2 = 0;
                lbl_1_bss_D5881 = 0;
                for (i = 0; i > 0x2D; i++)
                    lbl_1_bss_D5E90[i].unk0 = i;
                lbl_1_bss_D5F94 = lbl_1_bss_D5E90[2];
                g_current_camera->eye = lbl_1_data_6BE38;
                g_current_camera->lookAt = lbl_1_data_6BE44;
            }
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                lbl_1_bss_D5C10.unk4 = lbl_80198B38;
                lbl_1_bss_D5C10.unk8 = 0x12;
            }
            break;
        case 18:
            if (lbl_1_bss_D5C10.unk4 == lbl_80198B38)
            {
                int i;

                for (i = 0; i < 6; i++)
                {
                    lbl_1_bss_DD058[i] = 0;
                    lbl_1_data_6BDB4[i] = 0.0f;
                }
                fn_1_12325C();
                fn_1_118E64();
            }
            lbl_1_bss_D5C10.unk4--;
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                printf("rank:%d\t", lbl_1_bss_D5C10.rank);
                printf("score:%d\n", lbl_1_bss_D5C10.score);
                lbl_1_bss_D5C10.unk9 = 0;
                lbl_1_bss_D5C10.unk8 = 0xE;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38;
            }
            break;
        case 19:
            lbl_1_bss_D5C10.unk4--;
            lbl_1_bss_D5D9C = 0;
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                lbl_1_bss_D5C10.unk8 = 0x14;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38;
            }
            break;
        case 20:
            lbl_1_bss_D5C10.unk4--;
            if (lbl_1_bss_D5D90 != 0)
                lbl_1_bss_D5C10.unk4 = 0;
            else
                lbl_1_bss_D5C10.unk4 = 0;
            if (lbl_1_bss_D5C10.unk4 == 0)
            {
                fn_1_119B1C();
                lbl_1_bss_D5C10.unk8 = 0x15;
                lbl_1_bss_D5C10.unk4 = lbl_80198B38 * 5;
            }
            break;
        case 21:
        {
            float r = 15.0f;

            lbl_1_bss_D5C10.unk4--;
            if (lbl_1_bss_D5D90 != 0)
            {
                camera_set_state(3);
                g_current_camera->lookAt = lbl_1_data_6BE74;
                g_current_camera->eye.x = r * math_sin(16384.0 + 3.1415927410125732 * ((g_font_char_var_3 << 6) / 25.0));
                g_current_camera->eye.z = r;
                g_current_camera->eye.y = 5.0 + mathutil_cos(g_font_char_var_3 << 6);
            }
            else
            {
                camera_set_state(3);
                g_current_camera->lookAt = lbl_1_data_6BE74;
                g_current_camera->eye.x = r * math_sin(16384.0 + 3.1415927410125732 * ((lbl_1_bss_D5C10.unk4 << 6) / 25.0));
                g_current_camera->eye.z = r * math_sin(3.1415927410125732 * ((lbl_1_bss_D5C10.unk4 << 6) / 25.0));
                g_current_camera->eye.y = 5.0 + mathutil_cos(lbl_1_bss_D5C10.unk4 << 6);
            }
            if (lbl_1_bss_D5C10.unk4 < 0)
                lbl_1_bss_D5C10.unk4 = 0;
            break;
        }
        case 23:
            if (lbl_1_bss_D5C10.unk4 != 0)
            {
                int difficulty = modeCtrl.difficulty;
                int i;

                for (i = 0; i < 5; i++)
                {
                    strcpy(lbl_1_bss_D5C70[i].name, lbl_1_data_6B918[difficulty][i]);
                    lbl_1_bss_D5C70[i].score = lbl_1_data_6B968[difficulty][i];
                    lbl_1_bss_D5C70[i].unk10 = (u8)lbl_1_data_6B9B8[difficulty][i];
                    lbl_1_bss_D5C70[i].unk14 = lbl_1_data_6BA08[difficulty][i];
                }
            }
            if (--lbl_1_bss_D5C10.unk4 <= 0)
                lbl_1_bss_D5C10.unk4 = 0;
            break;
        }
        if (!((g_font_char_var_3 % 10) / 10.0))
        {
            lbl_1_bss_D587C++;
            if (lbl_1_bss_D587C == 6)
                lbl_1_bss_D587C = 0;
            lbl_1_data_6BAE0[0]++;
            if (lbl_1_data_6BAE0[0] == 6)
                lbl_1_data_6BAE0[0] = 0;
        }
    }
}

void fn_1_120AD8(void)
{
    Vec v;
    int i;

    camera_apply_viewport_if_changed(0);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(0);
    g_draw_stage();
    g_draw_bg();
    mtxa_from_mtxb();
    mtxa_rotate_z(lbl_1_bss_D5FC0.unk2);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(stage_gma->modelEntries[0x2F].model);
    v.x = v.y = 0.0f;
    v.z = 0.1f;
    if ((pad_status_groups[player_pad_map[lbl_1_bss_D5D9C]].unk0[0].button & 0x100)
     && lbl_1_bss_D5FC0.unk4 != 0
     && lbl_1_bss_D5C10.unk8 == 0xF)
        v.z = -0.02f;
    if (lbl_1_bss_D5FDC != 0)
        v.z = -0.02f;
    mtxa_from_mtxb();
    mtxa_translate(&v);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(stage_gma->modelEntries[10].model);
    {
        Vec v2;
        s16 angles[5] = { 0x3000, 0x1800, 0, -0x1800, -0x3000 };

        for (i = 0; i < 5; i++)
        {
            v2.x = v2.y = v2.z = 0.0f;
            mtxa_from_mtxb_translate(&v2);
            mtxa_rotate_z(lbl_1_bss_D5FC0.unk0 + angles[i]);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(lbl_1_bss_D5E90[i].unk8);
        }
    }
    fn_1_122DA8();
    fn_1_122ABC(lbl_1_bss_D5FF0);
    for (i = 0; i < 6; i++)
    {
        if (lbl_1_bss_DD040[i] != NULL)
            draw_ape(lbl_1_bss_DD040[i]);
    }
    ord_tbl_draw_nodes();
    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(stage_gma->modelEntries[0x5F].model);
}

void fn_1_120D48(u8 *name)
{
    int i;
    int blank = 1;

    for (i = 0; i < 6; i++)
    {
        if (name[i] != ' ')
        {
            blank = 0;
            break;
        }
    }
    if (blank != 0)
        strncpy((char *)name, lbl_1_data_6B630[lbl_1_bss_D5C10.apeType], 6);
}

void fn_1_120DC0(void)
{
    int pad;

    if (lbl_1_bss_D5C10.unk0 == 0 || lbl_1_bss_D5C10.unk0 == 2)
        pad = player_pad_map[modeCtrl.currPlayer];
    else if (lbl_1_bss_D5C10.unk0 == 1)
        pad = player_pad_map[lbl_1_bss_D5D9C];
    else
        pad = player_pad_map[modeCtrl.currPlayer];
    if (lbl_1_bss_D5C10.unk9 == 6)
        fn_1_121800();
    else
    {
        fn_1_121188();
        lbl_1_bss_D5F94.unk4 = lbl_1_bss_D5E90[2].unk4;
        lbl_1_bss_D5F94.unk0 = lbl_1_bss_D5E90[2].unk0;
    }
    if (lbl_1_bss_D5FC0.unk7 == 0)
        lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = lbl_1_bss_D5F94.unk4;
    fn_1_121EE0();
    if ((pad_status_groups[pad].unk0[2].button & 0x200) && lbl_1_bss_D5C10.unk3B == 0)
    {
        lbl_1_bss_D5881 = 0;
        if (lbl_1_bss_D5C10.unk9 != 0)
            lbl_1_bss_D5C10.unk3B = 0x2D;
        if (lbl_1_bss_D5C10.unk9 == 0)
        {
            if (lbl_1_bss_D5FA8[lbl_1_bss_D5C10.unk9] != 2)
                lbl_1_bss_D5C10.unk3B = 0x2D;
            else
                lbl_1_bss_D5C10.unk3B = 0;
        }
        call_SoundReqID_arg_0(0x3BD);
        if (lbl_1_bss_D5C10.unk9 > 0)
        {
            lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = ' ';
            lbl_1_bss_D5C10.unk9--;
            lbl_1_bss_DD071 = lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9];
            lbl_1_bss_DD074 = lbl_1_bss_D5C10.unk20[lbl_1_bss_D5C10.unk9];
            lbl_1_bss_DD078 = lbl_1_bss_D5F94.unk0;
            lbl_1_bss_D5FC0.unk7 = 1;
            lbl_1_bss_D5FC0.unk6 = 0;
            lbl_1_bss_D5C10.unk38 = 0.3 * lbl_80198B38;
            lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = ' ';
            lbl_1_bss_D5FA8[lbl_1_bss_D5C10.unk9] = 8;
        }
    }
    if (lbl_1_bss_D5FC0.unk7 != 0)
        fn_1_121748(lbl_1_bss_DD071, lbl_1_bss_DD074, lbl_1_bss_DD078);
    else if (lbl_1_bss_D5FC0.unk7 == 0)
        lbl_1_bss_D5FC0.unk8 = 0;
    if ((pad_status_groups[pad].unk0[2].button & 0x200) && lbl_1_bss_DD058[0] == 0 && lbl_1_bss_D5FA8[0] == 7)
        lbl_1_bss_D5C10.unk3B = 0;
    if (lbl_1_bss_D5C10.unk3B > 0)
        lbl_1_bss_D5C10.unk3B--;
}

void fn_1_121188(void)
{
    int pad;

    if (lbl_1_bss_D5C10.unk0 == 1)
        pad = player_pad_map[lbl_1_bss_D5D9C];
    else
        pad = player_pad_map[modeCtrl.currPlayer];
    if (lbl_1_bss_D5C10.unk8 == 3 || lbl_1_bss_D5C10.unk8 == 0xF)
    {
        if (pad_status_groups[pad].unk0[0].stickX > 5.0 || (pad_status_groups[pad].unk0[0].button & 2))
        {
            lbl_1_bss_D5FC0.unk10--;
            if ((pad_status_groups[pad].unk0[0].button & 0x20) || (pad_status_groups[pad].unk0[0].button & 0x40))
                lbl_1_bss_D5FC0.unk10 = -5;
            if (lbl_1_bss_D5FC0.unk10 < -5)
                lbl_1_bss_D5FC0.unk10 = 0;
        }
        else if (pad_status_groups[pad].unk0[0].stickX < -5.0 || (pad_status_groups[pad].unk0[0].button & 1))
        {
            lbl_1_bss_D5FC0.unk10++;
            if ((pad_status_groups[pad].unk0[0].button & 0x40) || (pad_status_groups[pad].unk0[0].button & 0x20))
                lbl_1_bss_D5FC0.unk10 = 5;
            if (lbl_1_bss_D5FC0.unk10 > 5)
                lbl_1_bss_D5FC0.unk10 = 0;
        }
        else
            lbl_1_bss_D5FC0.unk10 = 0;
        if (lbl_1_bss_D5FC0.unk10 == 0 && lbl_1_bss_D5FC0.unkC == 0)
        {
            if (pad_status_groups[pad].unk0[2].button & 0x20)
                lbl_1_bss_D5FC0.unkC = -1;
            if (pad_status_groups[pad].unk0[2].button & 0x40)
                lbl_1_bss_D5FC0.unkC = 1;
        }
        if (lbl_1_bss_D5FC0.unk10 == -5)
            lbl_1_bss_D5FC0.unkC = -1;
        else if (lbl_1_bss_D5FC0.unk10 == 5)
            lbl_1_bss_D5FC0.unkC = 1;
    }
    if (lbl_1_bss_D5FC0.unk10 != 0)
        lbl_1_bss_D5FC0.unk4 = 0;
    if (lbl_1_bss_D5FC0.unk10 == 0)
        lbl_1_bss_D5FC0.unk4 = 1;
    if (lbl_1_bss_D5FC0.unkC == -1)
    {
        if (lbl_1_bss_D5FC0.unk0 > -0x1800)
        {
            lbl_1_bss_D5FC0.unk0 -= 0x300;
            lbl_1_bss_D5FC0.unk2 -= 0x800;
        }
        else
        {
            call_SoundReqID_arg_0(0x3BD);
            lbl_1_bss_D5FC0.unk5 = -1;
            lbl_1_bss_D5FC0.unkC = 0;
        }
    }
    else if (lbl_1_bss_D5FC0.unkC == 1)
    {
        if (lbl_1_bss_D5FC0.unk0 < 0x1800)
        {
            lbl_1_bss_D5FC0.unk0 += 0x300;
            lbl_1_bss_D5FC0.unk2 += 0x800;
        }
        else
        {
            call_SoundReqID_arg_0(0x3BD);
            lbl_1_bss_D5FC0.unk5 = 1;
            lbl_1_bss_D5FC0.unkC = 0;
        }
    }
    if (lbl_1_bss_D5FC0.unk5 != 0)
        fn_1_1215FC();
    if (lbl_1_bss_D5FC0.unkC == 0 && lbl_1_bss_D5FC0.unk5 == 0)
        lbl_1_bss_D5FC0.unk0 = 0;
}

void fn_1_1215FC(void)
{
    int i;

    for (i = 0; i < 5; i++)
    {
        if (lbl_1_bss_D5FC0.unk5 == -1)
        {
            if (lbl_1_bss_D5F80[i] > 0)
                lbl_1_bss_D5F80[i]--;
            else
                lbl_1_bss_D5F80[i] = 0x2C;
        }
        else if (lbl_1_bss_D5FC0.unk5 == 1)
        {
            if (lbl_1_bss_D5F80[i] < 0x2C)
                lbl_1_bss_D5F80[i]++;
            else
                lbl_1_bss_D5F80[i] = 0;
        }
        lbl_1_bss_D5E90[i].unk4 = (u8)((char *)lbl_1_data_6C03C)[lbl_1_bss_D5F80[i]];
        lbl_1_bss_D5E90[i].unk8 = lbl_1_bss_D5ECC[lbl_1_bss_D5F80[i]];
        lbl_1_bss_D5E90[i].unk0 = lbl_1_bss_D5F80[i];
    }
    lbl_1_bss_D5FC0.unk4 = 1;
    lbl_1_bss_D5FC0.unk5 = 0;
}

void fn_1_121748(u8 a, int b, int c)
{
    lbl_1_bss_D5F94.unk4 = lbl_1_bss_D5E90[2].unk4;
    lbl_1_bss_D5F94.unk0 = lbl_1_bss_D5E90[2].unk0;
    if (lbl_1_bss_D5F94.unk4 == lbl_1_bss_DD071)
    {
        lbl_1_bss_D5FC0.unk8 = 1;
        lbl_1_bss_D5FC0.unk7 = 0;
    }
    else
    {
        lbl_1_bss_D5FC0.unk4 = 0;
        if (lbl_1_bss_D5FC0.unk4 == 0 && lbl_1_bss_D5FC0.unk8 == 0)
        {
            if (c - b > 0)
                lbl_1_bss_D5FC0.unk5 = -1;
            else
                lbl_1_bss_D5FC0.unk5 = 1;
        }
    }
}

void fn_1_121800(void)
{
    int pad;

    if (lbl_1_bss_D5C10.unk0 == 0 || lbl_1_bss_D5C10.unk0 == 2)
        pad = player_pad_map[modeCtrl.currPlayer];
    else if (lbl_1_bss_D5C10.unk0 == 1)
        pad = player_pad_map[lbl_1_bss_D5D9C];
    else
        pad = player_pad_map[modeCtrl.currPlayer];
    lbl_1_bss_D5F94.unk4 = lbl_1_bss_D5E90[2].unk4;
    lbl_1_bss_D5F94.unk0 = lbl_1_bss_D5E90[2].unk0;
    if (lbl_1_bss_D5881 == 0)
    {
        if (lbl_1_bss_D5F94.unk4 == 0)
        {
            lbl_1_bss_D5881 = 1;
            lbl_1_bss_D5FC0.unk6 = 1;
            lbl_1_bss_D5FC0.unk14 = 1;
            lbl_1_bss_D5880 = 0;
        }
        else
        {
            lbl_1_bss_D5FC0.unk4 = 0;
            lbl_1_bss_D5FC0.unk14 = 0;
            if (lbl_1_bss_D5FC0.unk6 == 0)
            {
                if (lbl_1_bss_D5F94.unk0 > 0x16)
                    lbl_1_bss_D5FC0.unk5 = 1;
                else
                    lbl_1_bss_D5FC0.unk5 = -1;
            }
            lbl_1_bss_D5FC0.unk6 = 0;
            lbl_1_bss_D5880 = 0;
        }
        fn_1_1215FC();
    }
    else if (lbl_1_bss_D5881 == 1)
    {
        if (lbl_1_bss_D5F94.unk4 != 0 && lbl_1_bss_D5F94.unk4 != 8)
            lbl_1_bss_D5881 = 0;
        if (lbl_1_bss_D5FC0.unkC == 0)
        {
            if (lbl_1_bss_D5880 == 0)
            {
                if (pad_status_groups[pad].unk0[0].stickX > 5.0 || (pad_status_groups[pad].unk0[0].button & 2)
                 || (pad_status_groups[pad].unk0[2].button & 0x20))
                {
                    lbl_1_bss_D5FC0.unkC = -1;
                    lbl_1_bss_D5880 = 1;
                }
            }
            else if (lbl_1_bss_D5880 == 1)
            {
                if (pad_status_groups[pad].unk0[0].stickX < -5.0 || (pad_status_groups[pad].unk0[0].button & 1)
                 || (pad_status_groups[pad].unk0[2].button & 0x40))
                {
                    lbl_1_bss_D5FC0.unkC = 1;
                    lbl_1_bss_D5880 = 0;
                }
            }
        }
        if (lbl_1_bss_D5FC0.unkC == -1)
        {
            if (lbl_1_bss_D5FC0.unk0 > -0x1800)
            {
                lbl_1_bss_D5FC0.unk0 -= 0x300;
                lbl_1_bss_D5FC0.unk2 -= 0x800;
            }
            else
            {
                call_SoundReqID_arg_0(0x3BD);
                lbl_1_bss_D5FC0.unk5 = -1;
                lbl_1_bss_D5FC0.unkC = 0;
            }
        }
        else if (lbl_1_bss_D5FC0.unkC == 1)
        {
            if (lbl_1_bss_D5FC0.unk0 < 0x1800)
            {
                lbl_1_bss_D5FC0.unk0 += 0x300;
                lbl_1_bss_D5FC0.unk2 += 0x800;
            }
            else
            {
                call_SoundReqID_arg_0(0x3BD);
                lbl_1_bss_D5FC0.unk5 = 1;
                lbl_1_bss_D5FC0.unkC = 0;
            }
        }
        if (lbl_1_bss_D5FC0.unk5 != 0)
            fn_1_1215FC();
        lbl_1_bss_D5F94.unk4 = lbl_1_bss_D5E90[2].unk4;
        lbl_1_bss_D5F94.unk0 = lbl_1_bss_D5E90[2].unk0;
        if (lbl_1_bss_D5FC0.unkC == 0 && lbl_1_bss_D5FC0.unk5 == 0)
            lbl_1_bss_D5FC0.unk0 = 0;
    }
}

void fn_1_121EE0(void)
{
    int pad;
    int i;
    int flag;

    if (lbl_1_bss_D5C10.unk0 == 0)
        pad = player_pad_map[modeCtrl.currPlayer];
    else if (lbl_1_bss_D5C10.unk0 == 1)
        pad = player_pad_map[lbl_1_bss_D5D9C];
    else
        pad = player_pad_map[lbl_1_bss_D5D9C];
    if ((pad_status_groups[pad].unk0[2].button & 0x100) && lbl_1_bss_D5C10.unk3B == 0
     && lbl_1_bss_D5FC0.unk4 != 0 && lbl_1_bss_D5D9C < 4)
        lbl_1_bss_D5C10.unk3A = 1;
    if (lbl_1_bss_D5C10.unk3A != 0)
    {
        call_SoundReqID_arg_0(0x10);
        if (lbl_1_bss_D5C10.unk9 == 5)
            lbl_1_bss_D5C10.unk3B = 0x3C;
        switch (lbl_1_bss_D5F94.unk4)
        {
        case 8:
            lbl_1_bss_D5FC0.unk6 = 0;
            if (lbl_1_bss_D5C10.unk9 == 6)
                lbl_1_bss_D5881 = 0;
            if (lbl_1_bss_D5C10.unk9 != 0)
            {
                lbl_1_bss_D5C10.unk9--;
                lbl_1_bss_D5C10.unk38 = 0.3 * lbl_80198B38;
                lbl_1_bss_D5C10.unk3B = 0x2D;
                lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = 0x20;
                lbl_1_bss_D5FA8[lbl_1_bss_D5C10.unk9] = 8;
            }
            else if (lbl_1_bss_D5C10.unk9 == 0)
            {
                lbl_1_bss_D5C10.unk3B = 0xA;
                lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = 0x20;
            }
            break;
        case 0:
            if (lbl_1_bss_D5C10.unk9 == 0)
            {
                int j;

                strcpy((char *)lbl_1_bss_D5C10.name, lbl_1_data_6B630[lbl_1_bss_D5C10.apeType]);
                for (j = 0; j < 6; j++)
                {
                    lbl_1_bss_D5FA8[j] = 0;
                    lbl_1_bss_DD058[j] = 1;
                    lbl_1_bss_DD07C[j] = lbl_1_data_6BFE0[lbl_1_data_6B640[lbl_1_bss_D5C10.apeType][j]];
                }
                lbl_1_bss_D5FA4 = 1;
            }
            else if (lbl_1_bss_D5C10.unk9 == 6)
            {
                lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = lbl_1_bss_D5F94.unk4;
                flag = 1;
                for (i = 0; i < 6; i++)
                {
                    if (lbl_1_bss_D5C10.name[i] != 0x20)
                    {
                        flag = 0;
                        break;
                    }
                }
                if (flag)
                {
                    strncpy((char *)lbl_1_bss_D5C10.name, lbl_1_data_6B630[lbl_1_bss_D5C10.apeType], 6);
                    for (i = 0; i < 6; i++)
                        lbl_1_bss_DD07C[i] = lbl_1_data_6BFE0[lbl_1_data_6B640[lbl_1_bss_D5C10.apeType][i]];
                }
            }
            else
            {
                lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = lbl_1_bss_D5F94.unk4;
                lbl_1_bss_D5FA8[lbl_1_bss_D5C10.unk9] = 3;
                lbl_1_bss_D5888 = lbl_1_bss_D5C10.unk9;
                for (i = lbl_1_bss_D5C10.unk9; i < 6; i++)
                {
                    lbl_1_bss_D5FA8[i] = 0;
                    lbl_1_bss_DD058[i] = 1;
                    lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = 0x20;
                }
                lbl_1_bss_D5FA4 = 2;
                flag = 1;
                for (i = 0; i < 6; i++)
                {
                    if (lbl_1_bss_D5C10.name[i] != 0x20)
                    {
                        flag = 0;
                        break;
                    }
                }
                if (flag)
                {
                    strncpy((char *)lbl_1_bss_D5C10.name, lbl_1_data_6B630[lbl_1_bss_D5C10.apeType], 6);
                    for (i = 0; i < 6; i++)
                        lbl_1_bss_DD07C[i] = lbl_1_data_6BFE0[lbl_1_data_6B640[lbl_1_bss_D5C10.apeType][i]];
                }
            }
            lbl_1_bss_D5C10.unk45 = 1;
            lbl_1_bss_D5C10.unk3B = 0;
            lbl_1_bss_D5C10.unk4 = 0;
            break;
        default:
            lbl_1_bss_D5FA8[lbl_1_bss_D5C10.unk9] = 3;
            lbl_1_bss_D5888 = 0;
            lbl_1_bss_D5C10.name[lbl_1_bss_D5C10.unk9] = lbl_1_bss_D5F94.unk4;
            lbl_1_bss_D5C10.unk20[lbl_1_bss_D5C10.unk9] = lbl_1_bss_D5F94.unk0;
            lbl_1_bss_DD058[lbl_1_bss_D5C10.unk9] = 1;
            lbl_1_bss_DD07C[lbl_1_bss_D5C10.unk9] = lbl_1_data_6BFE0[lbl_1_bss_D5F94.unk0];
            lbl_1_bss_D5C10.unk9++;
            if (lbl_1_bss_D5C10.unk9 == 6)
                call_SoundReqID_arg_0(0x3BD);
            if (lbl_1_bss_D5C10.unk9 < 6)
                lbl_1_bss_D5FA8[lbl_1_bss_D5C10.unk9] = 0;
            break;
        }
    }
    lbl_1_bss_D5C10.unk3A = 0;
}

void fn_1_122594(void)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        lbl_1_bss_DD058[i] = 0;
        lbl_1_bss_D5FF0[i].pos.x = 0.0f;
        lbl_1_bss_D5FF0[i].pos.y = 2.0f;
        lbl_1_bss_D5FF0[i].pos.z = -4.5f;
        lbl_1_bss_DD07C[i] = 5;
        balls[i].pos = lbl_1_bss_D5FF0[i].pos;
    }
}

void fn_1_12264C(struct Ball *ball)
{
    Vec v[6];
    int i;

    for (i = 0; i < 6; i++, ball++)
    {
        ball->vel.x = ball->vel.y = ball->vel.z = 0.05f;
        switch (lbl_1_bss_DD058[i])
        {
        case 0:
            ball->pos = *(Vec *)lbl_1_data_6BE80;
            ball->rotX = 0;
            ball->rotY = 0;
            ball->rotZ = 0;
            lbl_1_bss_DD07C[i] = 0;
            break;
        case 1:
            v[i].x = v[i].y = v[i].z = 0.0f;
            if (ball->pos.y > -2.15f)
                ball->pos.y -= 0.3;
            else if (ball->pos.y <= -2.15f)
            {
                if (i >= 0)
                {
                    float dx = lbl_1_rodata_656C0[i].x - ball->pos.x;

                    ball->pos.y = -2.15f;
                    ball->pos.z += 0.2;
                    if (ball->pos.z >= -1.0f && dx != 0.0)
                        ball->pos.x += dx / 6.0;
                    if (ball->pos.z >= lbl_1_rodata_656C0[i].z)
                        lbl_1_bss_DD058[i] = 2;
                }
                else
                {
                    float dx = lbl_1_rodata_656C0[i].x - ball->pos.x;

                    ball->pos.y = -2.15f;
                    ball->pos.z += 0.2;
                    if (ball->pos.z >= -1.0f)
                        ball->pos.x += dx / 6.0;
                    if (ball->pos.z >= lbl_1_rodata_656C0[i].z)
                    {
                        ball->pos.x = lbl_1_rodata_656C0[i].x;
                        lbl_1_bss_DD058[i] = 2;
                    }
                }
            }
            v[i].x = ball->pos.x - ball->prevPos.x;
            v[i].y = ball->pos.y - ball->prevPos.y;
            v[i].z = ball->pos.z - ball->prevPos.z;
            v[i].x *= 20860.4375f;
            v[i].y *= 20860.4375f;
            v[i].z *= 20860.4375f;
            ball->rotX += v[i].z;
            ball->rotZ += v[i].x;
            break;
        case 2:
            ball->pos = lbl_1_rodata_656C0[i];
            lbl_1_bss_D5FC0.unk4 = 1;
            break;
        case 3:
            if (ball->pos.y > -2.15f)
                ball->pos.y -= 0.2;
            else
            {
                ball->pos.x = lbl_1_rodata_656C0[i].x;
                ball->pos.z += 0.2;
                ball->rotX -= 0x300;
                if (ball->pos.z >= 2.5f)
                {
                    ball->pos.z = 2.5f;
                    ball->pos.y -= 0.2;
                }
                if (ball->pos.y < -3.0)
                    lbl_1_bss_DD058[i] = 0;
            }
            break;
        }
        ball->prevPos = ball->pos;
    }
}

void fn_1_122ABC(struct Ball *arg)
{
    struct Ball *ball = arg;
    int i;

    for (i = 0; i < 6; i++, ball++)
    {
        if (lbl_1_bss_DD07C[i] != 0)
        {
            ball->modelScale = 1.0 + 0.2 * mathutil_cos(g_font_char_var_3 << 10);
            avdisp_set_alpha(1.0f);
            mtxa_from_mtxb();
            mtxa_translate_xyz(ball->pos.x, ball->pos.y, ball->pos.z);
            if (lbl_1_bss_D5FA8[i] == 5)
                mtxa_scale_s(ball->modelScale);
            else
                mtxa_scale_s(1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_set_z_mode(1, 3, 1);
            avdisp_draw_model_culled_sort_auto(stage_gma->modelEntries[lbl_1_bss_DD07C[i]].model);
        }
        ball->colorId = 0;
        ball->unk179 = 3;
        mtxa_from_mtxb_translate(&ball->pos);
        mtxa_rotate_z(ball->rotZ);
        mtxa_rotate_x(ball->rotX);
        mtxa_rotate_y(ball->rotY);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        fn_1_4EAFC(mtxa->mtxA, ball->colorId, ball->unk179);
    }
}

void fn_1_122C60(void)
{
    int i;

    for (i = 0; i < 0x3C; i++)
    {
        lbl_1_bss_DD094[i] = (s16)((rand() & 0x7FFF) % 4);
        lbl_1_bss_D6B00[i].rotY = (s16)(rand() & 0x7FFF);
        lbl_1_bss_D6B00[i].rotX = (s16)(rand() & 0x7FFF);
        lbl_1_bss_D6B00[i].rotZ = (s16)(rand() & 0x7FFF);
        lbl_1_bss_D6A10[i] = lbl_1_data_6BFE0[(rand() & 0x7FFF) % 0x2D];
        lbl_1_bss_D6B00[i].pos = ((Vec *)lbl_1_data_6BAE4)[i];
    }
}

void fn_1_122DA8(void)
{
    int i;

    for (i = 0; i < 0x3C; i++)
    {
        avdisp_set_alpha(1.0f);
        if (lbl_1_bss_D6A10[i] != 0)
        {
            mtxa_from_mtxb_translate(&lbl_1_bss_D6B00[i].pos);
            mtxa_scale_s(1.5f);
            mtxa_rotate_z(lbl_1_bss_D6B00[i].rotZ);
            mtxa_rotate_y(lbl_1_bss_D6B00[i].rotY);
            mtxa_rotate_x(lbl_1_bss_D6B00[i].rotX);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(stage_gma->modelEntries[lbl_1_bss_D6A10[i]].model);
        }
        lbl_1_bss_D6B00[i].colorId = 0;
        lbl_1_bss_D6B00[i].unk179 = 2;
        mtxa_from_mtxb_translate(&lbl_1_bss_D6B00[i].pos);
        mtxa_scale_s(1.7f);
        mtxa_rotate_z(lbl_1_bss_D6B00[i].rotZ);
        mtxa_rotate_y(lbl_1_bss_D6B00[i].rotY);
        mtxa_rotate_x(lbl_1_bss_D6B00[i].rotX);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        fn_1_4EAFC(mtxa->mtxA, lbl_1_bss_D6B00[i].colorId, lbl_1_bss_D6B00[i].unk179);
    }
}

// The functions named unused_* are never called. The linker strips their code,
// but the strings and constants they use stay in .data and .rodata.
static void unused_plus_string(void)
{
    printf("plus");
}

void fn_1_122FA4(void)
{
    Vec dir;
    Vec unused;
    int lod = 1;
    int i;

    if (lbl_1_bss_D5878 == 0)
    {
        lbl_1_bss_DD10C = active_monkey_id[modeCtrl.currPlayer];
        lbl_1_bss_D5C10.apeType = active_monkey_id[modeCtrl.currPlayer];
    }
    else if (lbl_1_bss_D5878 == 1)
    {
        printf("player_id:%d\n", lbl_1_bss_D5D9C);
        lbl_1_bss_DD10C = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
        lbl_1_bss_D5C10.apeType = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
        printf("now_ape_type:%d\n", lbl_1_bss_DD10C);
    }
    else
    {
        lbl_1_bss_DD10C = 0;
        lbl_1_bss_D5C10.apeType = 0;
    }
    for (i = 0; i < 6; i++)
    {
        lbl_1_bss_DD040[i] = g_load_ape_variant0(lbl_1_bss_DD10C, lod, 0);
        dir.x = 0.0f;
        dir.y = 0.0f;
        dir.z = 1.0f;
        lbl_1_bss_D5FA8[i] = 7;
        lbl_1_bss_DD040[i]->pos.x = lbl_1_bss_D5FF0[i].pos.x;
        lbl_1_bss_DD040[i]->pos.y = lbl_1_bss_D5FF0[i].pos.y;
        lbl_1_bss_DD040[i]->pos.z = lbl_1_bss_D5FF0[i].pos.z;
        lbl_1_bss_DD040[i]->unk270.x = 0.0f;
        lbl_1_bss_DD040[i]->unk270.y = 0.0f;
        lbl_1_bss_DD040[i]->unk270.z = 0.0f;
        lbl_1_bss_DD040[i]->colorIndex = modeCtrl.currPlayer;
        fn_1_1132D0(lbl_1_bss_DD040[i], 0, 7, 8);
        g_ape_anim_body(lbl_1_bss_DD040[i]);
        g_smth_with_ape_rotation(lbl_1_bss_DD040[i], &dir);
    }
    lbl_1_bss_D5FA4 = 0;
}

void fn_1_12325C(void)
{
    Vec dir;
    Vec unused;
    int i;
    int lod;

    for (i = 0; i < 6; i++)
    {
        g_something_with_freeing_chara_heap_ape(lbl_1_bss_DD040[i]);
        lbl_1_bss_DD040[i] = NULL;
    }
    lod = 1;
    lbl_1_bss_DD10C = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
    lbl_1_bss_D5C10.apeType = lbl_1_bss_D5DA0[lbl_1_bss_D5D9C].apeType;
    for (i = 0; i < 6; i++)
    {
        lbl_1_bss_DD040[i] = g_load_ape_variant0(lbl_1_bss_DD10C, lod, 0);
        dir.x = 0.0f;
        dir.y = 0.0f;
        dir.z = 1.0f;
        lbl_1_bss_D5FA8[i] = 7;
        lbl_1_bss_DD040[i]->pos = lbl_1_bss_D5FF0[i].pos;
        lbl_1_bss_DD040[i]->unk270.x = lbl_1_bss_DD040[i]->unk270.y = lbl_1_bss_DD040[i]->unk270.z = 0.0f;
        lbl_1_bss_DD040[i]->colorIndex = lbl_1_bss_D5D9C;
        g_ape_anim_body(lbl_1_bss_DD040[i]);
        g_smth_with_ape_rotation(lbl_1_bss_DD040[i], &dir);
    }
    lbl_1_bss_D5FA4 = 0;
}

// Unreferenced, so the linker strips the code, but its floats stay in .rodata
static void unused_rodata_floats(float *out)
{
    out[0] = 3.1f;
    out[1] = 0.5f;
    out[2] = 0.85f;
}

void fn_1_12342C(void)
{
    Vec dir;
    int i;

    for (i = 0; i < 6; i++)
    {
        dir.x = 0.0f;
        dir.y = 0.0f;
        dir.z = 1.0f;
        switch (lbl_1_bss_D5FA8[i])
        {
        case 0:
            lbl_1_bss_DD040[i]->pos = lbl_1_data_6BED0;
            lbl_1_bss_DD040[i]->scale = 1.0f;
            lbl_1_bss_D5FA8[i] = 1;
            lbl_1_bss_D5C10.unk38 = 5;
            fn_1_1132D0(lbl_1_bss_DD040[i], 0, 7, 0);
            break;
        case 1:
            lbl_1_bss_D5C10.unk38--;
            if (i == 0)
                lbl_1_bss_D5FA8[i] = 2;
            else if (lbl_1_bss_D5FA8[i - 1] != 2)
                lbl_1_bss_D5FA8[i] = 2;
            else if (lbl_1_bss_D5FA8[i - 1] == 2 && lbl_1_bss_D5FA8[i - 1] == 5)
                lbl_1_bss_D5FA8[i] = 2;
            if (lbl_1_bss_D5C10.unk38 <= 0)
                lbl_1_bss_D5C10.unk38 = 0;
            break;
        case 2:
            {
                int j;

                if (lbl_1_bss_DD040[i]->pos.y < lbl_1_data_6BEDC.y - 0.1)
                    lbl_1_bss_DD040[i]->pos.y += 0.1;
                else
                {
                    lbl_1_bss_DD040[i]->pos = lbl_1_data_6BEDC;
                    if (lbl_1_bss_D5FA4 == 1)
                    {
                        for (j = 0; j < 6; j++)
                            lbl_1_bss_D5FA8[j] = 3;
                    }
                    if (lbl_1_bss_D5FA4 == 2)
                    {
                        for (j = lbl_1_bss_D5C10.unk9; j < 6; j++)
                            lbl_1_bss_D5FA8[j] = 3;
                    }
                }
                lbl_1_bss_DD040[i]->pos.x = lbl_1_data_6BEDC.x;
                lbl_1_bss_DD040[i]->pos.z = lbl_1_data_6BEDC.z;
                lbl_1_bss_DD040[i]->scale = 1.0f;
                fn_1_1132D0(lbl_1_bss_DD040[i], 0, 7, 0);
                for (j = i + 1; j < 6; j++)
                {
                    if (lbl_1_bss_D5FA8[j] == 2)
                        lbl_1_bss_D5FA8[j] = 7;
                }
            }
            break;
        case 3:
            if (lbl_1_bss_DD040[i]->pos.y < 3.0)
                lbl_1_bss_DD040[i]->pos.y += 0.096;
            else
                lbl_1_bss_D5FA8[i] = 4;
            lbl_1_bss_DD040[i]->pos.x = 0.0f;
            lbl_1_bss_DD040[i]->pos.z = -0.5f;
            lbl_1_bss_DD040[i]->scale = 1.0f;
            fn_1_1132D0(lbl_1_bss_DD040[i], 0, 7, 1);
            break;
        case 4:
            {
                float dx = lbl_1_rodata_65708[i].x - lbl_1_bss_DD040[i]->pos.x;
                float dz = lbl_1_rodata_65708[i].z - lbl_1_bss_DD040[i]->pos.z;

                lbl_1_bss_DD040[i]->pos.x += dx / 10.0;
                lbl_1_bss_DD040[i]->pos.z += dz / 10.0;
                if (i == 1)
                    lbl_1_bss_DD040[i]->pos.z = 1.0f;
                lbl_1_bss_DD040[i]->pos.y -= lbl_1_data_6BDB4[i];
                lbl_1_data_6BDB4[i] += 0.006;
                if (lbl_1_bss_DD040[i]->pos.y <= lbl_1_rodata_65708[i].y)
                    lbl_1_bss_D5FA8[i] = 5;
            }
            break;
        case 5:
            {
                int tbl[5] = { 8, 6, 4, 2, 0 };

                lbl_1_data_6BDB4[i] = 0.0f;
                lbl_1_bss_DD040[i]->pos = lbl_1_rodata_65708[i];
                fn_1_1132D0(lbl_1_bss_DD040[i], 0, 7, tbl[lbl_1_bss_D5C10.rank - 1]);
            }
            break;
        case 8:
            lbl_1_bss_D5C10.unk38--;
            fn_1_1132D0(lbl_1_bss_DD040[i], 0, 4, 0);
            lbl_1_bss_DD040[i]->pos = lbl_1_rodata_65708[i];
            fn_1_1132D0(lbl_1_bss_DD040[i], 0, 3, 0);
            if (lbl_1_bss_D5C10.unk38 < 0)
                lbl_1_bss_D5C10.unk38 = 0;
            if (lbl_1_bss_D5C10.unk38 == 0)
                lbl_1_bss_D5FA8[i] = 6;
            break;
        case 6:
            lbl_1_bss_DD058[lbl_1_bss_D5C10.unk9] = 3;
            lbl_1_bss_DD040[i]->pos.x = lbl_1_rodata_656C0[i].x;
            lbl_1_bss_DD040[i]->pos.y = 0.9f + lbl_1_bss_D5FF0[i].pos.y;
            lbl_1_bss_DD040[i]->pos.z = lbl_1_bss_D5FF0[i].pos.z;
            fn_1_1132D0(lbl_1_bss_DD040[i], 0, 5, 0);
            if (lbl_1_bss_D5878 == 0 && lbl_1_bss_DD040[i]->pos.y < (double)-3.15f)
            {
                if (i != 0)
                    lbl_1_bss_D5FA8[i] = 7;
                if (i == 0)
                    lbl_1_bss_D5FA8[i] = 0;
            }
            if (lbl_1_bss_D5878 == 1 && lbl_1_bss_DD040[i]->pos.y < (double)-3.15f)
            {
                s8 flag = 0;

                if (i != 0)
                {
                    int j;

                    lbl_1_bss_D5FA8[i] = 7;
                    for (j = 0; j < 6; j++)
                    {
                        if (lbl_1_bss_D5FA8[j] == 2)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 0)
                        lbl_1_bss_D5FA8[i] = 0;
                }
                if (i == 0)
                    lbl_1_bss_D5FA8[i] = 0;
            }
            if (lbl_1_bss_DD058[i] == 0)
                lbl_1_bss_D5FA8[i] = 7;
            break;
        case 7:
            lbl_1_bss_DD040[i]->pos = *(Vec *)lbl_1_data_6BE80;
            lbl_1_bss_DD040[i]->scale = __fabs(math_sin(g_font_char_var_3 << 10));
            fn_1_1132D0(lbl_1_bss_DD040[i], 0, 7, 0);
            break;
        case 9:
            fn_1_1132D0(lbl_1_bss_DD040[i], 0, 7, 4);
            if (lbl_1_bss_DD040[i]->pos.y < 15.0)
            {
                lbl_1_bss_D5887 = 0;
                lbl_1_bss_DD058[i] = 3;
                lbl_1_bss_DD040[i]->pos.x = lbl_1_rodata_656C0[i].x;
                lbl_1_bss_DD040[i]->pos.y += lbl_1_data_6BDB4[i];
                lbl_1_data_6BDB4[i] += 0.006;
                lbl_1_bss_DD040[i]->pos.z = lbl_1_bss_D5FF0[i].pos.z;
                fn_1_1132D0(lbl_1_bss_DD040[i], 0, 7, 5);
            }
            else
            {
                if (i == 0)
                    lbl_1_bss_D5FA8[i] = 0;
                else
                    lbl_1_bss_D5FA8[i] = 7;
                lbl_1_bss_D5887 = 1;
                lbl_1_data_6BDB4[i] = 0.0f;
            }
            break;
        default:
            lbl_1_bss_DD040[i]->pos.x = lbl_1_bss_D5FF0[i].pos.x;
            lbl_1_bss_DD040[i]->pos.y = lbl_1_bss_D5FF0[i].pos.y;
            lbl_1_bss_DD040[i]->pos.z = lbl_1_bss_D5FF0[i].pos.z;
            break;
        }
        g_ape_anim_body(lbl_1_bss_DD040[i]);
        g_smth_with_ape_rotation(lbl_1_bss_DD040[i], &dir);
    }
}

void fn_1_123E94(int n)
{
    g_current_camera->unk128.x = (g_current_camera->unk11C.x - g_current_camera->eye.x) / n;
    g_current_camera->unk128.y = (g_current_camera->unk11C.y - g_current_camera->eye.y) / n;
    g_current_camera->unk128.z = (g_current_camera->unk11C.z - g_current_camera->eye.z) / n;
    g_current_camera->unk140.x = (g_current_camera->unk134.x - g_current_camera->lookAt.x) / n;
    g_current_camera->unk140.y = (g_current_camera->unk134.y - g_current_camera->lookAt.y) / n;
    g_current_camera->unk140.z = (g_current_camera->unk134.z - g_current_camera->lookAt.z) / n;
}

void fn_1_1240C8(void)
{
    camera_set_state(3);
    g_current_camera->eye.x = g_current_camera->eye.x + g_current_camera->unk128.x;
    g_current_camera->eye.y += g_current_camera->unk128.y;
    g_current_camera->eye.z += g_current_camera->unk128.z;
    g_current_camera->lookAt.x += g_current_camera->unk140.x;
    g_current_camera->lookAt.y += g_current_camera->unk140.y;
    g_current_camera->lookAt.z += g_current_camera->unk140.z;
}

void fn_1_1241E8(void)
{
    camera_set_state(3);
    g_current_camera->eye.x = g_current_camera->unk11C.x;
    g_current_camera->eye.y = g_current_camera->unk11C.y;
    g_current_camera->eye.z = g_current_camera->unk11C.z;
    g_current_camera->lookAt.x = g_current_camera->unk134.x;
    g_current_camera->lookAt.y = g_current_camera->unk134.y;
    g_current_camera->lookAt.z = g_current_camera->unk134.z;
}

const int lbl_1_rodata_659A8[] = { 0, 8, 0xB };

void fn_1_1242CC(void)
{
    int x = 0x140;
    int y = 0xC8;
    struct TextBox tb;
    char *msgs[6] = {
        "please use c/0xffffff/p/LEVER/c/0x000000/a/ to choose a character",
        "***",
        "***",
        "***",
        "***",
        "\x82\x6E\x82\xC5\x82\xE0\x82\xB6\x82\xF0\x82\xA6\x82\xE7\x82\xF1\x82\xC5",
    };

    memset(&tb, 0, sizeof(tb));
    tb.unk16 = 0xB;
    tb.x = (s16)x;
    tb.y = (s16)y;
    tb.unk14 = 0;
    tb.lines = 1;
    tb.callback = NULL;
    g_display_playpoint_or_gift_message_child(1, 1, &tb);
    g_smth_with_playpoint_or_gift_msg(1, msgs[locale]);
}

void fn_1_1243A8(void)
{
    int x = 0x140;
    int y = 0xE6;
    struct TextBox tb;
    char *msgs[6] = {
        "and press c/0xffffff/p/BOTANN_A/ c/0x000000/a/to select",
        "and press A to select",
        "and press A to select",
        "and press A to select",
        "and press A to select",
        "\x82\x60\x82\xC5\x82\xAB\x82\xDF\x82\xC4\x82\xCB",
    };

    memset(&tb, 0, sizeof(tb));
    tb.unk16 = 0xB;
    tb.x = (s16)x;
    tb.y = (s16)y;
    tb.unk14 = 0;
    tb.lines = 1;
    tb.callback = NULL;
    g_display_playpoint_or_gift_message_child(1, 1, &tb);
    g_smth_with_playpoint_or_gift_msg(1, msgs[locale]);
}

static void unused_rodata_floats_2(float *out)
{
    out[0] = 5120.0f;
    out[1] = 0.51f;
    out[2] = 3.0f;
}

static void draw_press_a_button(void)
{
    char *msgs[6] = {
        "PRESS A BUTTON",
        "*** press a button ***",
        "*** press a button ***",
        "*** press a button ***",
        "*** press a button ***",
        "PRESS A BUTTON",
    };

    char *str;

    set_ui_widescreen_scale_mtx(0x140);
    if (main_mode != 1)
    {
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        textdraw_set_scale(0.625f, 0.625f);
        str = msgs[locale];
        textdraw_set_depth(0.51f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(13.0f, 453.0f);
        textdraw_print(str);
        textdraw_set_depth(0.5f);
        textdraw_set_mul_color(-1);
        textdraw_set_pos(10.0f, 450.0f);
        textdraw_print(str);
        textdraw_set_scale(1.0f, 1.0f);
    }
    reset_ui_widescreen_scale_mtx();
}

static void draw_choose_character_prompt(int y, char **str, float *out)
{
    char *msgs[6] = {
        "please use /bcffffff//cd83ad//bcff8000/ to choose a character\n and press /bcffffff//cd839f//bcff8000/ to select",
        "***",
        "***",
        "***",
        "***",
        "/bcffffff//cd83ad//bcff8000/\x82\xC5\x82\xE0\x82\xB6\x82\xF0\x82\xA6\x82\xE7\x82\xF1\x82\xC5/bcffffff//cd839f//bcff8000/\x82\xC5\x82\xAB\x82\xDF\x82\xC4\x82\xCB",
    };
    char *msgs2[6] = {
        "please use /cd83ad/ to choose a character\n and press /cd839f/ to select",
        "***",
        "***",
        "***",
        "***",
        "/cd83ad/\x82\xC5\x82\xE0\x82\xB6\x82\xF0\x82\xA6\x82\xE7\x82\xF1\x82\xC5/cd839f/\x82\xC5\x8C\x88\x82\xDF\x82\xC4\x82\xCB",
    };

    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_alignment(3);
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    textdraw_set_scale(0.8f, 0.8f);
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(0);
    if (out != NULL)
    {
        *out = 400.0f;
        *str = "/cd83ad/\x82\xC5\x82\xE0\x82\xB6\x82\xF0\x82\xA6\x82\xE7\x82\xF1\x82\xC5/cd839f/\x82\xC5\x82\xAB\x82\xDF\x82\xC4\x82\xCB";
    }
    textdraw_set_pos(322.0f, (float)(y + 2));
    textdraw_print(msgs2[locale]);
    textdraw_set_depth(0.5f);
    textdraw_set_mul_color(0xFF8000);
    textdraw_set_pos(320.0f, (float)y);
    textdraw_print(msgs[locale]);
    textdraw_set_scale(1.0f, 1.0f);
    reset_ui_widescreen_scale_mtx();
}

static inline u32 blink_color(void)
{
    u32 t = g_font_char_var_3 % 60;

    if (t > 30)
        t = 60 - t;
    t <<= 3;
    t |= (t << 16) | (t << 8);
    if (main_mode == 1)
        t = 0;
    return t;
}

void fn_1_124484(float x, float y, int idx)
{
    textdraw_reset();
    textdraw_set_font(0x22);
    textdraw_set_alignment(0);
    if (lbl_1_bss_D5C10.unk0 == 0)
    {
        if (idx == lbl_1_bss_D5C10.rank - 1)
            textdraw_set_add_color(blink_color());
        else if (modeCtrl.currPlayer == lbl_1_bss_D5C70[idx].unk12)
            textdraw_set_add_color(blink_color());
        else
            textdraw_set_add_color(0);
    }
    else if (lbl_1_bss_D5C10.unk0 == 2)
    {
        if (idx == lbl_1_bss_D5C10.rank - 1)
            textdraw_set_add_color(blink_color());
        else
            textdraw_set_add_color(0);
    }
    else if (lbl_1_bss_D5C10.unk0 == 1)
    {
        if (lbl_1_bss_D5C70[idx].unk20 > 0)
            textdraw_set_add_color(blink_color());
        else
            textdraw_set_add_color(0);
    }
    textdraw_set_depth(2.0f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(2.0f + x, 2.0f + y);
    textdraw_printf("%d", idx + 1);
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_pos(x, y);
    textdraw_printf("%d", idx + 1);
}

void fn_1_1247EC(float x, float y, u8 *str, int idx)
{
    int i;

    textdraw_reset();
    textdraw_set_font(0x20);
    textdraw_set_alignment(0);
    switch (lbl_1_bss_D5C10.unk0)
    {
    case 1:
        if (idx == 5)
        {
            if ((lbl_1_bss_D5C10.unk54 & 2) || (lbl_1_bss_D5C10.unk54 & 4))
                textdraw_set_add_color(blink_color());
            else if (lbl_1_bss_D5883 != 0)
                textdraw_set_add_color(blink_color());
        }
        else
        {
            textdraw_set_add_color(0);
            if (lbl_1_bss_D5C70[idx].unk20 > 0)
                textdraw_set_add_color(blink_color());
            else
                textdraw_set_add_color(0);
        }
        break;
    case 0:
        if (idx == lbl_1_bss_D5C10.rank - 1)
            textdraw_set_add_color(blink_color());
        else if (modeCtrl.currPlayer == lbl_1_bss_D5C70[idx].unk12)
            textdraw_set_add_color(blink_color());
        else
            textdraw_set_add_color(0);
        break;
    case 2:
        if (idx == lbl_1_bss_D5C10.rank - 1)
            textdraw_set_add_color(blink_color());
        else
            textdraw_set_add_color(0);
        break;
    }
    for (i = 0; i < 6; i++)
    {
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(2.0f + x + (float)(i * 0x22), 2.0f + y);
        textdraw_printf("%c", str[i]);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(x + (float)(i * 0x22), y);
        textdraw_printf("%c", str[i]);
    }
}

struct SpriteDrawRequest lbl_1_data_6DDA0 = { 0x301, 338.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 6, -1, 0 };
struct SpriteDrawRequest lbl_1_data_6DDF0 = { 0x302, 338.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 6, -1, 0 };
struct SpriteDrawRequest lbl_1_data_6DE40 = { 0x303, 38.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 6, -1, 0 };

void fn_1_124CBC(float x, float y, int num, int idx)
{
    int course;

    if (idx == -1)
        course = lbl_1_bss_D5C10.course;
    else
        course = lbl_1_bss_D5C70[idx].unk11;
    textdraw_reset();
    if (num == 0xFF)
    {
        lbl_1_data_6D54C.mulColor = 0;
        lbl_1_data_6D54C.z = 2.0f;
        lbl_1_data_6D54C.x = 3.0f + (2.0f + x);
        lbl_1_data_6D54C.y = 2.0f + y;
        draw_sprite_draw_request(&lbl_1_data_6D54C);
        lbl_1_data_6D54C.x = 2.0f + (20.0f + x);
        draw_sprite_draw_request(&lbl_1_data_6D54C);
        lbl_1_data_6D54C.mulColor = -1;
        lbl_1_data_6D54C.z = 1.0f;
        lbl_1_data_6D54C.x = 3.0f + x;
        lbl_1_data_6D54C.y = y;
        draw_sprite_draw_request(&lbl_1_data_6D54C);
        lbl_1_data_6D54C.x = 20.0f + x;
        draw_sprite_draw_request(&lbl_1_data_6D54C);
    }
    else
    {
        switch (course)
        {
        case 1:
            if (lbl_1_bss_D5C10.unk8 == 6 || lbl_1_bss_D5C10.unk8 == 5)
            {
                if (idx == lbl_1_bss_D5C10.rank - 1 || modeCtrl.currPlayer == lbl_1_bss_D5C70[idx].unk12)
                    lbl_1_data_6DDA0.addColor = blink_color();
                else
                    lbl_1_data_6DDA0.addColor = 0;
            }
            else
                lbl_1_data_6DDA0.addColor = 0;
            lbl_1_data_6DDA0.mulColor = 0;
            lbl_1_data_6DDA0.x = 2.0f + x;
            lbl_1_data_6DDA0.y = 1.0f + (2.0f + y);
            lbl_1_data_6DDA0.z = 1.0f;
            draw_sprite_draw_request(&lbl_1_data_6DDA0);
            lbl_1_data_6DDA0.mulColor = -1;
            lbl_1_data_6DDA0.x = x;
            lbl_1_data_6DDA0.y = 1.0f + y;
            lbl_1_data_6DDA0.z = 0.5f;
            draw_sprite_draw_request(&lbl_1_data_6DDA0);
            break;
        case 2:
            if (lbl_1_bss_D5C10.unk8 == 6 || lbl_1_bss_D5C10.unk8 == 5)
            {
                if (idx == lbl_1_bss_D5C10.rank - 1 || modeCtrl.currPlayer == lbl_1_bss_D5C70[idx].unk12)
                    lbl_1_data_6DDF0.addColor = blink_color();
                else
                    lbl_1_data_6DDF0.addColor = 0;
            }
            else
                lbl_1_data_6DDF0.addColor = 0;
            lbl_1_data_6DDF0.mulColor = 0;
            lbl_1_data_6DDF0.x = 2.0f + x;
            lbl_1_data_6DDF0.y = 1.0f + (2.0f + y);
            lbl_1_data_6DDF0.z = 1.0f;
            draw_sprite_draw_request(&lbl_1_data_6DDF0);
            lbl_1_data_6DDF0.mulColor = -1;
            lbl_1_data_6DDF0.x = x;
            lbl_1_data_6DDF0.y = 1.0f + y;
            lbl_1_data_6DDF0.z = 0.5f;
            draw_sprite_draw_request(&lbl_1_data_6DDF0);
            break;
        case 3:
            if (lbl_1_bss_D5C10.unk8 == 5 || lbl_1_bss_D5C10.unk8 == 6)
            {
                if (idx == lbl_1_bss_D5C10.rank - 1 || modeCtrl.currPlayer == lbl_1_bss_D5C70[idx].unk12)
                    lbl_1_data_6DE40.addColor = blink_color();
                else
                    lbl_1_data_6DE40.addColor = 0;
            }
            else
                lbl_1_data_6DE40.addColor = 0;
            lbl_1_data_6DE40.mulColor = 0;
            lbl_1_data_6DE40.x = 2.0f + x;
            lbl_1_data_6DE40.y = 1.0f + (2.0f + y);
            lbl_1_data_6DE40.z = 1.0f;
            draw_sprite_draw_request(&lbl_1_data_6DE40);
            lbl_1_data_6DE40.mulColor = -1;
            lbl_1_data_6DE40.x = x;
            lbl_1_data_6DE40.y = 1.0f + y;
            lbl_1_data_6DE40.z = 0.5f;
            draw_sprite_draw_request(&lbl_1_data_6DE40);
            break;
        case 0:
            break;
        }
        if (lbl_1_bss_D5C10.unk8 == 6 || lbl_1_bss_D5C10.unk8 == 5)
        {
            if (idx == lbl_1_bss_D5C10.rank - 1)
                textdraw_set_add_color(blink_color());
            else if (modeCtrl.currPlayer == lbl_1_bss_D5C70[idx].unk12)
                textdraw_set_add_color(blink_color());
            else
                textdraw_set_add_color(0);
        }
        else
            textdraw_set_add_color(0);
        textdraw_set_font(0x21);
        textdraw_set_alignment(0);
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(16.0f + (2.0f + x), 1.0f + (2.0f + y));
        textdraw_printf("%2d", num);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(16.0f + x, 1.0f + y);
        textdraw_printf("%2d", num);
    }
    textdraw_reset();
}

void fn_1_1254AC(int x, int y, int idx)
{
    char buf[4];
    float offs[3] = { 0.0f, 40.0f, 84.0f };
    int i;
    int val;
    float px;

    textdraw_reset();
    textdraw_set_font(0x21);
    if (lbl_1_bss_D5C10.unk0 == 1)
    {
        if (idx == 5)
        {
            if ((lbl_1_bss_D5C10.unk54 & 2) || (lbl_1_bss_D5C10.unk54 & 4))
                textdraw_set_add_color(blink_color());
            else if (lbl_1_bss_D5883 != 0)
                textdraw_set_add_color(blink_color());
        }
        else
        {
            textdraw_set_add_color(0);
            if (lbl_1_bss_D5C70[idx].unk20 > 0)
                textdraw_set_add_color(blink_color());
        }
    }
    if (idx == lbl_1_bss_D5C10.rank - 1 && (lbl_1_bss_D5885 != 0 || lbl_1_bss_D5C10.unk0 == 0))
        textdraw_set_add_color(blink_color());
    for (i = 0; i < 3; i++)
    {
        val = (&lbl_1_bss_D5C70[idx].unk24)[i];
        if (i == 1)
            val = CLAMP(val, 0, 59);
        else
            val = CLAMP(val, 0, 99);
        sprintf(buf, "%d", val / 10);
        px = (float)x + offs[i];
        textdraw_set_depth(0.51f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(3.0f + px, 3.0f + (float)y);
        textdraw_print(buf);
        textdraw_set_depth(0.5f);
        textdraw_set_mul_color(-1);
        textdraw_set_pos(px, (float)y);
        textdraw_print(buf);
        sprintf(buf, "%d", val % 10);
        px = 17.0f + ((float)x + offs[i]);
        textdraw_set_depth(0.51f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(3.0f + px, 3.0f + (float)y);
        textdraw_print(buf);
        textdraw_set_depth(0.5f);
        textdraw_set_mul_color(-1);
        textdraw_set_pos(px, (float)y);
        textdraw_print(buf);
    }
    textdraw_set_depth(0.51f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(3.0f + (float)(x + 0x22), 3.0f + (float)y);
    textdraw_print(":");
    textdraw_set_depth(0.5f);
    textdraw_set_mul_color(-1);
    textdraw_set_pos((float)(x + 0x22), (float)y);
    textdraw_print(":");
    textdraw_set_depth(0.51f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(3.0f + (float)(x + 0x4A), 3.0f + (float)y);
    textdraw_print(";");
    textdraw_set_depth(0.5f);
    textdraw_set_mul_color(-1);
    textdraw_set_pos((float)(x + 0x4A), (float)y);
    textdraw_print(";");
}

void fn_1_125B14(int id, float x, float y, int a, int b, int c)
{
    struct Sprite *sprite;

    sprite = get_sprite_with_unique_id(id + 0x16);
    if (sprite == NULL)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = id + 0x16;
            sprite->type = 1;
            sprite->bmpId = lbl_1_rodata_65690[a][c];
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x140;
            sprite->textAlign = 0;
            sprite->x = x;
            sprite->y = y;
            sprite->scaleX = 0.5f;
            sprite->scaleY = 0.328125f;
            sprite->depth = 2.0f;
            sprite->rotation = 5120.0f * math_sin(g_font_char_var_3 << 9);
            sprite->opacity = 1.0f;
            sprintf(sprite->text, "face A %d", id);
        }
    }
    else
    {
        sprite->bmpId = lbl_1_rodata_65690[a][c];
        sprite->x = x;
        sprite->y = y;
        sprite->rotation = 5120.0f * math_sin(g_font_char_var_3 << 9);
    }
    sprite = get_sprite_with_unique_id(id + 0x34);
    if (sprite == NULL)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = id + 0x34;
            sprite->type = 1;
            sprite->bmpId = lbl_1_rodata_65690[a][b];
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x140;
            sprite->textAlign = 0;
            sprite->x = x;
            sprite->y = y;
            sprite->scaleX = 0.5f;
            sprite->scaleY = 0.328125f;
            sprite->depth = 1.0f;
            sprite->rotation = 5120.0f * math_sin(g_font_char_var_3 << 9);
            sprite->opacity = 1.0 - (g_font_char_var_3 % 10) / 10.0;
            sprintf(sprite->text, "face B %d", id);
        }
    }
    else
    {
        sprite->bmpId = lbl_1_rodata_65690[a][b];
        sprite->x = x;
        sprite->y = y;
        sprite->rotation = 5120.0f * math_sin(g_font_char_var_3 << 9);
        sprite->opacity = 1.0 - (g_font_char_var_3 % 10) / 10.0;
    }
}

static inline u32 blink_color2(void)
{
    u32 t = g_font_char_var_3 % 30;

    if (t > 15)
        t = 30 - t;
    t *= 0x11;
    t = (t << 16) | (t << 8);
    if (main_mode == 1)
        t = 0;
    return t;
}

void fn_1_125E90(float x, float y, s8 idx)
{
    struct SpriteDrawRequest *req = (struct SpriteDrawRequest *)lbl_1_data_6D118;

    if (lbl_1_bss_D5C10.unk0 == 0)
    {
        if (idx == lbl_1_bss_D5C10.rank - 1)
            req->addColor = blink_color2();
        else if (modeCtrl.currPlayer == lbl_1_bss_D5C70[idx].unk12)
            req->addColor = blink_color2();
        else
            req->addColor = 0;
    }
    else if (lbl_1_bss_D5C10.unk0 == 2)
    {
        if (idx == lbl_1_bss_D5C10.rank - 1)
            req->addColor = blink_color2();
        else
            req->addColor = 0;
    }
    else if (lbl_1_bss_D5C10.unk0 == 1)
    {
        if (idx == 5)
        {
            if ((lbl_1_bss_D5C10.unk54 & 2) || (lbl_1_bss_D5C10.unk54 & 4))
            {
                lbl_1_bss_D5883 = 1;
                req->addColor = blink_color2();
            }
            else if (lbl_1_bss_D5883 != 0)
                req->addColor = blink_color2();
        }
        else
        {
            req->addColor = 0;
            if (lbl_1_bss_D5C70[idx].unk20 > 0)
                req->addColor = blink_color2();
            else if (lbl_1_bss_D5D98[idx] == 0)
                req->addColor = 0;
            else
                req->addColor = 0;
        }
    }
    else
        req->addColor = 0;
    req->x = x;
    req->y = y;
    draw_sprite_draw_request(req);
}

#define PRINT_SHADOWED(x, y, str)                     \
    textdraw_set_depth(0.51f);                        \
    textdraw_set_mul_color(0);                        \
    textdraw_set_pos(3.0f + (float)(x), 3.0f + (y));  \
    textdraw_print(str);                              \
    textdraw_set_depth(0.5f);                         \
    textdraw_set_mul_color(0xFFFFFF);                 \
    textdraw_set_pos((float)(x), (y));                \
    textdraw_print(str)

#define PRINT_SHADOWED_F(x, y, str)                   \
    textdraw_set_depth(0.51f);                        \
    textdraw_set_mul_color(0);                        \
    textdraw_set_pos(3.0f + (x), 3.0f + (y));         \
    textdraw_print(str);                              \
    textdraw_set_depth(0.5f);                         \
    textdraw_set_mul_color(0xFFFFFF);                 \
    textdraw_set_pos((x), (y));                       \
    textdraw_print(str)

static inline int get_result_panel_offset(void)
{
    int v;

    if (lbl_1_bss_D5C10.unk8 == 2)
    {
        v = 0x280 - (lbl_80198B38 - lbl_1_bss_D5C10.unk4) * 20;
        if (v <= 0)
            v = 0;
    }
    else if (lbl_1_bss_D5C10.unk8 == 13)
    {
        v = 0x280 - (lbl_80198B38 - lbl_1_bss_D5C10.unk4) * 20;
        if (v <= 0)
            v = 0;
    }
    else if (lbl_1_bss_D5C10.unk8 == 5)
        v = 15.0 * (lbl_1_bss_D5C10.unk4 - 2.0 * lbl_80198B38);
    else if (lbl_1_bss_D5C10.unk8 == 16 && lbl_1_bss_D5C10.unk4 < lbl_80198B38)
        v = (lbl_1_bss_D5C10.unk4 - lbl_80198B38) * 15;
    else if (lbl_1_bss_D5C10.unk8 == 17)
        v = 800;
    else if (lbl_1_bss_D5C10.unk8 == 18)
    {
        v = 0x280 - (lbl_80198B38 - lbl_1_bss_D5C10.unk4) * 15;
        if (v <= 0)
            v = 0;
    }
    else
        v = 0;
    return v;
}

#define REQ(n) ((struct SpriteDrawRequest *)lbl_1_data_##n)

static inline void update_prompt_position(void)
{
    switch (lbl_1_bss_D5C10.unk8)
    {
    case 16:
        lbl_1_bss_DD110 += lbl_80198B38 * 2 - lbl_1_bss_D5C10.unk4;
        REQ(6CD58)->y += lbl_80198B38 * 2 - lbl_1_bss_D5C10.unk4;
        break;
    case 5:
        lbl_1_bss_DD110 += lbl_80198B38 * 2 - lbl_1_bss_D5C10.unk4;
        REQ(6CD58)->y += lbl_80198B38 * 2 - lbl_1_bss_D5C10.unk4;
        break;
    case 3:
    case 15:
        lbl_1_bss_DD110 = 415;
        REQ(6CD58)->y = 400.0f;
        break;
    default:
        lbl_1_bss_DD110 = 415;
        REQ(6CD58)->y = 400.0f;
        break;
    }
}

void fn_1_126264(void)
{
    char numBuf[4];
    char rankBuf[8];
    char nameBuf[8];
    float y = 54.0f;
    int x;
    int barY = 29;
    int rankX;
    int nameX;
    int scoreX;
    int floorX;
    int timeX;
    int i;
    int digit;
    int printing;
    int divisor;
    int score;
    int t;
    s16 hours;
    s16 minutes;
    s16 seconds;

    x = get_result_panel_offset();
    update_prompt_position();
    draw_sprite_draw_request(REQ(6CD58));
    draw_choose_character_prompt(lbl_1_bss_DD110, NULL, NULL);

    set_ui_widescreen_scale_mtx(0x140);
    REQ(6CDA8)->y = REQ(6CDF8)->y = REQ(6D0C8)->y = REQ(6CE48)->y = REQ(6CF88)->y = barY;
    REQ(6CDA8)->x = x + 0x24;
    REQ(6CDF8)->x = x + 0xA0;
    REQ(6CE48)->x = x + 0x142;
    REQ(6D0C8)->x = x + 0x19F;
    REQ(6CF88)->x = x + 0x212;
    draw_sprite_draw_request(REQ(6CDA8));
    draw_sprite_draw_request(REQ(6CDF8));
    draw_sprite_draw_request(REQ(6D0C8));
    draw_sprite_draw_request(REQ(6CE48));
    draw_sprite_draw_request(REQ(6CF88));
    reset_ui_widescreen_scale_mtx();

    rankX = x + 0x18;
    nameX = x + 0x64;
    scoreX = x + 0x124;
    floorX = x + 0x1AC;
    timeX = x + 0x1ED;
    REQ(6D118)->x = x;
    REQ(6D118)->y = y;
    REQ(6D118)->addColor = 0;
    draw_sprite_draw_request(REQ(6D118));
    set_ui_widescreen_scale_mtx(0x140);

    textdraw_reset();
    textdraw_set_font(0x22);
    textdraw_set_alignment(0);
    sprintf(rankBuf, "%d", lbl_1_bss_D5C10.rank);
    PRINT_SHADOWED(rankX, y, rankBuf);

    textdraw_set_font(0x20);
    textdraw_set_alignment(0);
    textdraw_set_scale(0.8125f, 0.8125f);
    for (i = 0; i < 6; i++)
    {
        sprintf(nameBuf, "%c", lbl_1_bss_D5C10.name[i]);
        PRINT_SHADOWED(nameX + i * 0x1C, 2.0f + y, nameBuf);
    }

    textdraw_reset();
    textdraw_set_font(0x21);
    score = lbl_1_bss_D5C10.score;
    printing = FALSE;
    for (divisor = 1000000, digit = 0; divisor > 0; divisor /= 10, digit++)
    {
        if (printing != FALSE || divisor < 10 || score / divisor >= 0)
        {
            sprintf(numBuf, "%d", score / divisor);
            PRINT_SHADOWED(scoreX + digit * 0x11, 1.0f + y, numBuf);
            score %= divisor;
            printing = TRUE;
        }
    }

    textdraw_reset();
    textdraw_set_font(0x21);
    sprintf(numBuf, "%3d", lbl_1_bss_D5C10.unk3E);
    PRINT_SHADOWED(floorX, 1.0f + y, numBuf);

    textdraw_reset();
    textdraw_set_font(0x21);
    t = lbl_1_bss_D5C10.unk40;
    hours = t / 3600;
    t -= hours * 3600;
    minutes = t / 60;
    seconds = t - minutes * 60;
    sprintf(numBuf, "%d", hours / 10);
    PRINT_SHADOWED(timeX, 1.0f + y, numBuf);
    sprintf(numBuf, "%d", hours % 10);
    PRINT_SHADOWED(timeX + 0x11, 1.0f + y, numBuf);
    textdraw_set_font(3);
    textdraw_set_scale(0.66f, 1.1f);
    PRINT_SHADOWED(timeX + 0x20, 1.0f + y, ":");
    textdraw_set_font(0x21);
    textdraw_set_scale(1.0f, 1.0f);
    sprintf(numBuf, "%d", minutes / 10);
    PRINT_SHADOWED(timeX + 0x2F, 1.0f + y, numBuf);
    sprintf(numBuf, "%d", minutes % 10);
    PRINT_SHADOWED(timeX + 0x40, 1.0f + y, numBuf);
    textdraw_set_font(3);
    textdraw_set_scale(0.66f, 1.1f);
    PRINT_SHADOWED(timeX + 0x4F, 1.0f + y, ":");
    textdraw_set_font(0x21);
    textdraw_set_scale(1.0f, 1.0f);
    sprintf(numBuf, "%d", seconds / 10);
    PRINT_SHADOWED(timeX + 0x5E, 1.0f + y, numBuf);
    sprintf(numBuf, "%d", seconds % 10);
    PRINT_SHADOWED(timeX + 0x6F, 1.0f + y, numBuf);
    reset_ui_widescreen_scale_mtx();
}

#define FADE_REQ(n)                                                             \
    if (lbl_1_bss_D5C10.unk8 == 2)                                              \
        opacity = (float)(lbl_80198B38 - lbl_1_bss_D5C10.unk4) / (float)lbl_80198B38; \
    if (lbl_1_bss_D5C10.unk8 == 13)                                             \
        opacity = (float)(lbl_80198B38 - lbl_1_bss_D5C10.unk4) / (float)lbl_80198B38; \
    if (lbl_1_bss_D5C10.unk8 == 5)                                              \
        opacity = (float)lbl_1_bss_D5C10.unk4 / (float)lbl_80198B38;            \
    if (lbl_1_bss_D5C10.unk8 == 16)                                             \
    {                                                                           \
        opacity = (float)lbl_1_bss_D5C10.unk4 / (float)lbl_80198B38;            \
        if (opacity > 0.0)                                                      \
            opacity = 1.0f;                                                     \
    }                                                                           \
    if (lbl_1_bss_D5C10.unk8 == 19 || lbl_1_bss_D5C10.unk8 == 17)               \
        opacity = 1.0f;                                                         \
    REQ(n)->opacity = 1.0f

void fn_1_127390(void)
{
    char rankBuf[8];
    char nameBuf[8];
    char numBuf[8];
    float textX;
    float y = 54.0f;
    float opacity;
    int score;
    int divisor;
    int digit;
    int bar0;
    int barY = 29;
    int nameX;
    int scoreX;
    int floorX;
    int timeX;
    int x;
    int i;
    int printing;

    x = get_result_panel_offset();
    update_prompt_position();
    draw_sprite_draw_request(REQ(6CD58));
    draw_choose_character_prompt(lbl_1_bss_DD110, NULL, NULL);

    set_ui_widescreen_scale_mtx(0x140);
    bar0 = 0x29;
    nameX = 0xBC;
    scoreX = 0x14C;
    floorX = 0x1AD;
    timeX = 0x21A;

    REQ(6CDA8)->x = x + bar0;
    REQ(6CDA8)->y = barY;
    FADE_REQ(6CDA8);
    draw_sprite_draw_request(REQ(6CDA8));

    REQ(6CDF8)->x = x + nameX;
    REQ(6CDF8)->y = barY;
    FADE_REQ(6CDF8);
    draw_sprite_draw_request(REQ(6CDF8));

    REQ(6CF38)->x = x + timeX;
    REQ(6CF38)->y = barY;
    FADE_REQ(6CF38);
    draw_sprite_draw_request(REQ(6CF38));

    REQ(6CEE8)->x = x + scoreX;
    REQ(6CEE8)->y = barY;
    FADE_REQ(6CEE8);
    draw_sprite_draw_request(REQ(6CEE8));

    REQ(6CE48)->x = x + floorX;
    REQ(6CE48)->y = barY;
    FADE_REQ(6CE48);
    draw_sprite_draw_request(REQ(6CE48));
    reset_ui_widescreen_scale_mtx();

    REQ(6D118)->x = x;
    REQ(6D118)->y = y;
    REQ(6D118)->addColor = 0;
    draw_sprite_draw_request(REQ(6D118));
    set_ui_widescreen_scale_mtx(0x140);

    textdraw_reset();
    textdraw_set_font(0x22);
    textdraw_set_alignment(0);
    textX = x + 0x24;
    sprintf(rankBuf, "%d", lbl_1_bss_D5C10.rank);
    PRINT_SHADOWED_F(textX, y, rankBuf);

    textdraw_set_font(0x20);
    textdraw_set_alignment(0);
    textX = x + 0x77;
    for (i = 0; i < 6; i++)
    {
        sprintf(nameBuf, "%c", lbl_1_bss_D5C10.name[i]);
        PRINT_SHADOWED_F(textX + (float)(i * 0x22), y, nameBuf);
    }

    fn_1_124CBC(x + 0x155, y, lbl_1_bss_D5C10.floor, -1);

    textdraw_set_font(0x21);
    textdraw_set_alignment(0);
    score = lbl_1_bss_D5C10.score;
    printing = FALSE;
    for (divisor = 1000000, digit = 0; divisor > 0; divisor /= 10, digit++)
    {
        if (printing != FALSE || divisor < 10 || score / divisor >= 0)
        {
            textX = x + 0x197;
            sprintf(numBuf, "%d", score / divisor);
            PRINT_SHADOWED_F(textX + (float)(digit * 0x11), 1.0f + y, numBuf);
            score %= divisor;
            printing = TRUE;
        }
    }

    fn_1_125B14(5, x + 0x21F, y - 5.0f, lbl_1_bss_D5C10.apeType, lbl_1_bss_D587C, lbl_1_data_6BAE0[0]);
    reset_ui_widescreen_scale_mtx();
}

void fn_1_1285E8(void)
{
    float unused[] = { 119.0f, 153.0f, 187.0f, 221.0f, 255.0f, 289.0f };
}

static inline int update_mode_banner_y(void)
{
    int limit = 47;
    int y;

    if (lbl_1_bss_D5C10.unk8 == 5 || lbl_1_bss_D5C10.unk8 == 0x13)
    {
        y = (lbl_80198B38 * 2 - lbl_1_bss_D5C10.unk4) * 3 - 150;
        if (y > limit)
            y = limit;
        REQ(6CD08)->y = y;
    }
    else
    {
        y = limit;
        REQ(6CD08)->y = limit;
    }
    return y;
}

static inline int get_mode_banner_id(void)
{
    int mode = modeCtrl.difficulty;

    if (modeCtrl.courseFlags & 0x10)
        g_some_gift_menu_flags |= 1;
    if ((modeCtrl.courseFlags & 0x10) && (modeCtrl.courseFlags & 0x2000000))
        mode = 3;
    if (!(modeCtrl.courseFlags & 0x2000000) && (modeCtrl.courseFlags & 0x10))
        mode = modeCtrl.difficulty;
    if (lbl_1_bss_D5C10.unk0 == 2)
        mode = 4;
    return mode;
}

void fn_1_12862C(void)
{
    char *strs[][5] = {
        { "BEGINNER MODE", "ADVANCED MODE", "EXPERT MODE", "MASTER MODE", "STORY MODE" },
        { "***", "***", "***", "***", "***" },
        { "***", "***", "***", "***", "***" },
        { "***", "***", "***", "***", "***" },
        { "***", "***", "***", "***", "***" },
        { "\x8F\x89\x8B\x89", "\x92\x86\x8B\x89", "\x8F\xE3\x8B\x89", "\x92\x42\x90\x6C",
          "\x83\x58\x83\x67\x81\x5B\x83\x8A\x81\x5B\x83\x82\x81\x5B\x83\x68" },
    };
    int y = update_mode_banner_y();
    int mode = get_mode_banner_id();
    char *str;

    draw_sprite_draw_request(REQ(6CD08));
    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_flags(0xA0000000);
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    str = strs[locale][mode];
    textdraw_set_depth(0.51f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(323.0f, 3.0f + y);
    textdraw_print(str);
    textdraw_set_depth(0.5f);
    textdraw_set_mul_color(0xFFFF00);
    textdraw_set_pos(320.0f, y);
    textdraw_print(str);
    reset_ui_widescreen_scale_mtx();
}

#define RANK_ROW_ADD_COLOR(i)                                     \
    if ((i) == lbl_1_bss_D5C10.rank - 1)                          \
        textdraw_set_add_color(blink_color());                    \
    else                                                          \
        textdraw_set_add_color(0)

static inline int get_ranking_panel_offset(void)
{
    int v;

    if (lbl_1_bss_D5C10.unk8 == 5)
    {
        v = 0x280 - (lbl_80198B38 * 2 - lbl_1_bss_D5C10.unk4) * 10;
        if (v <= 0)
            v = 0;
    }
    else if (lbl_1_bss_D5C10.unk8 == 0x13)
    {
        v = 0x280 - (lbl_80198B38 * 2 - lbl_1_bss_D5C10.unk4) * 10;
        if (v <= 0)
            v = 0;
    }
    else
        v = 0;
    return v;
}

void fn_1_1288FC(void)
{
    int i;
    int y = 0x7A;
    float spacing = 68.0f;
    int x = get_ranking_panel_offset();
    char numBuf[4];
    char charBuf[4];
    int nameX;
    int scoreX;
    int floorX;
    int timeX;
    int rankX;
    int digit;
    int score;
    int divisor;
    int printing;
    int j;
    s16 hours;
    s16 minutes;
    s16 seconds;
    int t;

    {
        int bannerY = update_mode_banner_y();
        char *strs[] = { "STORY MODE", "***", "***", "***", "***",
                         "\x83\x58\x83\x67\x81\x5B\x83\x8A\x81\x5B\x83\x82\x81\x5B\x83\x68" };

        set_ui_widescreen_scale_mtx(0x140);
        REQ(6CD08)->x = 320.0f;
        REQ(6CD08)->y = bannerY;
        draw_sprite_draw_request(REQ(6CD08));
        textdraw_reset();
        textdraw_set_flags(0xA0000000);
        textdraw_set_font(3);
        textdraw_set_alignment(4);
        textdraw_set_depth(0.05f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(323.0f, bannerY);
        textdraw_print(strs[locale]);
        textdraw_set_depth(0.04f);
        textdraw_set_mul_color(0xFFFF00);
        textdraw_set_pos(320.0f, bannerY);
        textdraw_print(strs[locale]);
    }

    set_ui_widescreen_scale_mtx(0x140);
    REQ(6CDA8)->y = REQ(6CDF8)->y = REQ(6D0C8)->y = REQ(6CE48)->y = REQ(6CF88)->y = 91.0f;
    REQ(6CDA8)->x = x + 0x24;
    REQ(6CDF8)->x = x + 0xA0;
    REQ(6CE48)->x = x + 0x142;
    REQ(6D0C8)->x = x + 0x19F;
    REQ(6CF88)->x = x + 0x212;
    draw_sprite_draw_request(REQ(6CDA8));
    draw_sprite_draw_request(REQ(6CDF8));
    draw_sprite_draw_request(REQ(6D0C8));
    draw_sprite_draw_request(REQ(6CE48));
    draw_sprite_draw_request(REQ(6CF88));
    reset_ui_widescreen_scale_mtx();

    rankX = x + 0x18;
    nameX = x + 0x64;
    scoreX = x + 0x124;
    floorX = x + 0x1AC;
    timeX = x + 0x1ED;
    for (i = 0; i < 5; i++)
    {
        fn_1_125E90(x, y, i);
        set_ui_widescreen_scale_mtx(0x140);
        fn_1_124484(rankX, y - 1, i);
        textdraw_reset();
        textdraw_set_font(0x20);
        textdraw_set_alignment(0);
        textdraw_set_scale(0.8125f, 0.8125f);
        RANK_ROW_ADD_COLOR(i);
        for (j = 0; j < 6; j++)
        {
            sprintf(charBuf, "%c", ((u8 *)lbl_1_bss_D5C70[i].name)[j]);
            PRINT_SHADOWED(nameX + j * 0x1C, (float)(y + 2), charBuf);
        }

        textdraw_reset();
        textdraw_set_font(0x21);
        score = lbl_1_bss_D5C70[i].score;
        printing = FALSE;
        for (divisor = 1000000, digit = 0; divisor > 0; divisor /= 10, digit++)
        {
            if (printing != 0 || divisor < 10 || score / divisor >= 0)
            {
                RANK_ROW_ADD_COLOR(i);
                sprintf(numBuf, "%d", score / divisor);
                PRINT_SHADOWED(scoreX + digit * 0x11, (float)(y + 1), numBuf);
                score %= divisor;
                printing = TRUE;
            }
        }

        textdraw_reset();
        textdraw_set_font(0x21);
        RANK_ROW_ADD_COLOR(i);
        sprintf(numBuf, "%3d", lbl_1_bss_D5C70[i].unk18);
        PRINT_SHADOWED(floorX, (float)(y + 1), numBuf);

        textdraw_reset();
        textdraw_set_font(0x21);
        RANK_ROW_ADD_COLOR(i);
        t = lbl_1_bss_D5C70[i].unkC;
        hours = t / 3600;
        t -= hours * 3600;
        minutes = t / 60;
        seconds = t - minutes * 60;

        sprintf(numBuf, "%d", hours / 10);
        PRINT_SHADOWED(timeX, (float)(y + 1), numBuf);
        sprintf(numBuf, "%d", hours % 10);
        PRINT_SHADOWED(timeX + 0x11, (float)(y + 1), numBuf);
        textdraw_set_font(3);
        textdraw_set_scale(0.66f, 1.1f);
        PRINT_SHADOWED(timeX + 0x20, (float)(y + 1), ":");
        textdraw_set_font(0x21);
        textdraw_set_scale(1.0f, 1.0f);
        sprintf(numBuf, "%d", minutes / 10);
        PRINT_SHADOWED(timeX + 0x2F, (float)(y + 1), numBuf);
        sprintf(numBuf, "%d", minutes % 10);
        PRINT_SHADOWED(timeX + 0x40, (float)(y + 1), numBuf);
        textdraw_set_font(3);
        textdraw_set_scale(0.66f, 1.1f);
        PRINT_SHADOWED(timeX + 0x4F, (float)(y + 1), ":");
        textdraw_set_font(0x21);
        textdraw_set_scale(1.0f, 1.0f);
        sprintf(numBuf, "%d", seconds / 10);
        PRINT_SHADOWED(timeX + 0x5E, (float)(y + 1), numBuf);
        sprintf(numBuf, "%d", seconds % 10);
        PRINT_SHADOWED(timeX + 0x6F, (float)(y + 1), numBuf);
        reset_ui_widescreen_scale_mtx();
        y = (float)y + spacing;
    }
}

void fn_1_129BDC(void)
{
    float xf;
    float y;
    float spacing = 68.0f;
    float opacity;
    int i;
    int digit;
    int score;
    int divisor;
    int x;
    int bar0;
    int nameX;
    int scoreX;
    int floorX;
    int timeX;
    int barY;
    int textX;
    int printing;

    x = get_ranking_panel_offset();
    xf = x;
    y = 122.0f;
    set_ui_widescreen_scale_mtx(0x140);
    barY = 0x5B;
    bar0 = 0x29;
    nameX = 0xBC;
    scoreX = 0x14C;
    floorX = 0x1AD;
    timeX = 0x21A;

    REQ(6CDA8)->x = x + bar0;
    REQ(6CDA8)->y = barY;
    FADE_REQ(6CDA8);
    draw_sprite_draw_request(REQ(6CDA8));

    REQ(6CDF8)->x = x + nameX;
    REQ(6CDF8)->y = barY;
    FADE_REQ(6CDF8);
    draw_sprite_draw_request(REQ(6CDF8));

    REQ(6CF38)->x = x + timeX;
    REQ(6CF38)->y = barY;
    FADE_REQ(6CF38);
    draw_sprite_draw_request(REQ(6CF38));

    REQ(6CEE8)->x = x + scoreX;
    REQ(6CEE8)->y = barY;
    FADE_REQ(6CEE8);
    draw_sprite_draw_request(REQ(6CEE8));

    REQ(6CE48)->x = x + floorX;
    REQ(6CE48)->y = barY;
    FADE_REQ(6CE48);
    draw_sprite_draw_request(REQ(6CE48));
    reset_ui_widescreen_scale_mtx();

    for (i = 0; i < 5; i++)
    {
        fn_1_125E90(xf, y, i);
        set_ui_widescreen_scale_mtx(0x140);
        fn_1_124484(36.0f + xf, y - 1.0, i);
        fn_1_1247EC(119.0f + xf, y - 1.0, (u8 *)&lbl_1_bss_D5C70[i], i);
        fn_1_124CBC(341.0f + xf, y - 1.0, lbl_1_bss_D5C70[i].unk10, i);
        textdraw_reset();
        textdraw_set_font(0x21);
        if (i == lbl_1_bss_D5C10.rank - 1 || modeCtrl.currPlayer == lbl_1_bss_D5C70[i].unk12)
            textdraw_set_add_color(blink_color());
        else
            textdraw_set_add_color(0);

        score = lbl_1_bss_D5C70[i].score;
        printing = FALSE;
        for (divisor = 1000000, digit = 0; divisor > 0; divisor /= 10, digit++)
        {
            if (printing != 0 || divisor < 10 || score / divisor >= 0)
            {
                textX = x + 0x197;
                textdraw_set_depth(2.0f);
                textdraw_set_mul_color(0);
                textdraw_set_pos(textX + digit * 0x11 + 2, 1.0f + (2.0f + y));
                textdraw_printf("%d", score / divisor);
                textdraw_set_depth(1.0f);
                textdraw_set_mul_color(0xFFFFFF);
                textdraw_set_pos(textX + digit * 0x11, 1.0f + y);
                textdraw_printf("%d", score / divisor);
                score %= divisor;
                printing = TRUE;
            }
        }

        fn_1_125B14(i, 543.0f + xf, y - 4.0f, lbl_1_bss_D5C70[i].unk14, lbl_1_bss_D587C, lbl_1_data_6BAE0[0]);
        reset_ui_widescreen_scale_mtx();
        y += spacing;
    }
}

void fn_1_12AC88(struct Sprite *sprite);
void fn_1_12CBF0(int shared);
void fn_1_12E8D8(int shared);
void fn_1_130668(int x, int spacing);
void fn_1_132BB0(struct Sprite *sprite);
void fn_1_134A9C(int shared);
void fn_1_136764(int shared);

#define DEFINE_RANK_SPRITE_CREATOR(name, draw)                \
    void name(int a, u32 b)                                   \
    {                                                         \
        struct Sprite *sprite;                                \
                                                              \
        fn_1_11E790(a, b);                                    \
        sprite = create_sprite();                             \
        if (sprite != NULL)                                   \
        {                                                     \
            sprite->tag = 0x65;                               \
            sprite->depth = 1.0f;                             \
            sprite->mainFunc = (void *)fn_1_11F024;           \
            sprite->drawFunc = draw;                          \
            strcpy(sprite->text, "RANK");                     \
        }                                                     \
    }

DEFINE_RANK_SPRITE_CREATOR(fn_1_12A994, fn_1_12AC88)
DEFINE_RANK_SPRITE_CREATOR(fn_1_12AA00, (void *)fn_1_12CBF0)
DEFINE_RANK_SPRITE_CREATOR(fn_1_12AA6C, (void *)fn_1_12E8D8)
DEFINE_RANK_SPRITE_CREATOR(fn_1_12AAD8, (void *)fn_1_130668)
DEFINE_RANK_SPRITE_CREATOR(fn_1_12AB44, fn_1_132BB0)
DEFINE_RANK_SPRITE_CREATOR(fn_1_12ABB0, (void *)fn_1_134A9C)
DEFINE_RANK_SPRITE_CREATOR(fn_1_12AC1C, (void *)fn_1_136764)

void fn_1_138464(void);
void fn_1_139CE0(void);
void fn_1_13A4E0(void);
void fn_1_13ACFC(void);
void fn_1_13B55C(void);
void fn_1_13CC90(void);
void fn_1_13D54C(void);
void textdraw_print_and_fit_to_width(float width, char *str);

static inline void draw_face(struct NameEntryData *entry, float px, float py)
{
    int c = lbl_1_data_6BAE0[0];
    int b = lbl_1_bss_D587C;
    int a = entry->unk14;

    if (a >= 0 && a < 4)
    {
        struct SpriteDrawRequest *req;
        u16 *tbl = ((u16 **)lbl_1_data_6D5EC)[a];

        req = REQ(6D59C);
        req->bmpId = tbl[c];
        req->x = px;
        req->y = py;
        req->z = 1.0f;
        req->opacity = 1.0f;
        req->rotation = 5120.0f * math_sin(g_font_char_var_3 << 9);
        draw_sprite_draw_request(req);
        req = REQ(6D59C);
        req->bmpId = tbl[b];
        req->z = 0.5f;
        req->opacity = 1.0 - (g_font_char_var_3 % 10) / 10.0;
        draw_sprite_draw_request(req);
    }
}

static inline void draw_default_prompt(void)
{
    draw_choose_character_prompt(lbl_1_bss_DD110, NULL, NULL);
}

void fn_1_12AC88(struct Sprite *sprite)
{
    float opacity;
    int bar0 = 0x29;
    int barY;
    int nameX = 0xC0;
    int floorX = 0x18B;
    int timeX = 0x216;

    switch (lbl_1_bss_D5C10.unk8)
    {
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        int x = get_result_panel_offset();

        fn_1_138464();
        update_prompt_position();
        draw_sprite_draw_request(REQ(6CD58));
        draw_default_prompt();

        set_ui_widescreen_scale_mtx(0x140);
        barY = 29;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CF88)->x = x + floorX;
        REQ(6CF88)->y = barY;
        FADE_REQ(6CF88);
        draw_sprite_draw_request(REQ(6CF88));
        reset_ui_widescreen_scale_mtx();
        break;
    }
    case 19:
    case 20:
    case 21:
    {
        char numBuf[4];
        char charBuf[4];
        int y;
        int spacing = 0x37;
        int x = get_ranking_panel_offset();
        int row;
        int i;
        int val;
        int n;
        float fx;
        float rowY;

        y = update_mode_banner_y();
        fx = x;
        rowY = 122.0f;
        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request(REQ(6CD08));
        reset_ui_widescreen_scale_mtx();
        {
            int course = lbl_1_bss_D5C10.unk50;
            char *strs[][6] = {
            { "Race -Charming Banana Road-", "Race -Lovely Heart Ring-", "Race -Clock Tower Hill-", "Race -Cannon Ball Circuit-", "Race -Submarine Street-", "Race -Starlight Highway-" },
            { "Race g -Banana Road-", "Race g -Amusement Park-", "Race g -Clock Tower Hill-", "Race g -Magma Circuit-", "Race g -Submarine Street-", "Race g -Starlight Highway-" },
            { "Race f -Banana Road-", "Race f -Amusement Park-", "Race f -Clock Tower Hill-", "Race f -Magma Circuit-", "Race f -Submarine Street-", "Race f -Starlight Highway-" },
            { "Race s -Banana Road-", "Race s -Amusement Park-", "Race s -Clock Tower Hill-", "Race s -Magma Circuit-", "Race s -Submarine Street-", "Race s -Starlight Highway-" },
            { "Race i -Banana Road-", "Race i -Amusement Park-", "Race i -Clock Tower Hill-", "Race i -Magma Circuit-", "Race i -Submarine Street-", "Race i -Starlight Highway-" },
            { "\x83\x8C\x81\x5B\x83\x58\x20\x2D\x82\xDD\x82\xED\x82\xAD\x82\xCC\x83\x6F\x83\x69\x83\x69\x83\x8D\x81\x5B\x83\x68\x2D", "\x83\x8C\x81\x5B\x83\x58\x20\x2D\x83\x89\x83\x75\x83\x8A\x81\x5B\x82\xE4\x82\xA4\x82\xA6\x82\xF1\x82\xBF\x2D", "\x83\x8C\x81\x5B\x83\x58\x20\x2D\x83\x60\x83\x4E\x83\x5E\x83\x4E\x82\xCD\x82\xAE\x82\xE9\x82\xDC\x82\xB4\x82\xA9\x2D", "\x83\x8C\x81\x5B\x83\x58\x20\x2D\x83\x41\x83\x62\x83\x60\x83\x62\x83\x60\x81\x5B\x83\x54\x81\x5B\x83\x4C\x83\x62\x83\x67\x2D", "\x83\x8C\x81\x5B\x83\x58\x20\x2D\x82\xBF\x82\xF1\x82\xDA\x82\xC2\x83\x58\x83\x67\x83\x8A\x81\x5B\x83\x67\x2D", "\x83\x8C\x81\x5B\x83\x58\x20\x2D\x83\x58\x83\x79\x81\x5B\x83\x58\x83\x52\x83\x8D\x83\x52\x83\x8D\x83\x6A\x81\x5B\x2D" },
            };
            char *str;

            textdraw_reset();
            set_ui_widescreen_scale_mtx(0x140);
            textdraw_set_flags(0xA0000000);
            textdraw_set_font(3);
            textdraw_set_alignment(4);
            textdraw_set_scale(0.85f, 0.85f);
            str = strs[locale][course];
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(323.0f, 3.0f + y);
            textdraw_print_and_fit_to_width(390.0f, str);
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_pos(320.0f, y);
            textdraw_print_and_fit_to_width(390.0f, str);
            textdraw_set_scale(1.0f, 1.0f);
            reset_ui_widescreen_scale_mtx();
        }

        set_ui_widescreen_scale_mtx(0x140);
        barY = 0x5B;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CF88)->x = x + floorX;
        REQ(6CF88)->y = barY;
        FADE_REQ(6CF88);
        draw_sprite_draw_request(REQ(6CF88));
        reset_ui_widescreen_scale_mtx();

        for (row = 0; row < 5; row++)
        {
            fn_1_125E90(fx, rowY, row);
            set_ui_widescreen_scale_mtx(0x140);
            fn_1_124484(4.0f + (32.0f + fx), rowY - 1.0f, row);
            fn_1_1247EC(4.0f + (115.0f + fx), rowY - 1.0f, (u8 *)&lbl_1_bss_D5C70[row], row);
            fn_1_1254AC(360.0f + fx, rowY, row);
            draw_face(&lbl_1_bss_D5C70[row], 32.0f + (543.0f + fx), 12.0f + rowY);
            if (lbl_1_bss_D5C70[row].unk2C & 1)
            {
                REQ(6D4FC)->x = x + 0x24C;
                REQ(6D4FC)->y = rowY - 6.0f;
                draw_sprite_draw_request(REQ(6D4FC));
            }
            reset_ui_widescreen_scale_mtx();
            rowY += spacing;
        }

        fn_1_125E90(fx, 407.0f, 5);
        set_ui_widescreen_scale_mtx(0x140);
        REQ(6D45C)->x = 36.0f + fx;
        REQ(6D45C)->y = 407.0f;
        draw_sprite_draw_request(REQ(6D45C));
        textdraw_set_font(0x20);
        textdraw_set_alignment(0);
        n = 119.0f + fx;
        for (i = 0; i < 6; i++)
        {
            sprintf(charBuf, "%c", ((u8 *)lbl_1_bss_D5E60.name)[i]);
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(3.0f + (float)(n + i * 0x22), 410.0f);
            textdraw_print(charBuf);
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(-1);
            textdraw_set_pos((float)(n + i * 0x22), 407.0f);
            textdraw_print(charBuf);
        }

        {
            float offs[3] = { 0.0f, 40.0f, 84.0f };
            float px;
            float timeBase = 360.0f + fx;

            textdraw_reset();
            textdraw_set_font(0x21);
            for (i = 0; i < 3; i++)
            {
                val = (&lbl_1_bss_D5E60.unk24)[i];
                if (i == 1)
                    val = CLAMP(val, 0, 59);
                else
                    val = CLAMP(val, 0, 99);
                sprintf(numBuf, "%d", val / 10);
                px = timeBase + offs[i];
                textdraw_set_depth(0.51f);
                textdraw_set_mul_color(0);
                textdraw_set_pos(3.0f + px, 410.0f);
                textdraw_print(numBuf);
                textdraw_set_depth(0.5f);
                textdraw_set_mul_color(-1);
                textdraw_set_pos(px, 407.0f);
                textdraw_print(numBuf);
                sprintf(numBuf, "%d", val % 10);
                px = 17.0f + (timeBase + offs[i]);
                textdraw_set_depth(0.51f);
                textdraw_set_mul_color(0);
                textdraw_set_pos(3.0f + px, 410.0f);
                textdraw_print(numBuf);
                textdraw_set_depth(0.5f);
                textdraw_set_mul_color(-1);
                textdraw_set_pos(px, 407.0f);
                textdraw_print(numBuf);
            }
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(3.0f + (34.0f + timeBase), 410.0f);
            textdraw_print(":");
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(-1);
            textdraw_set_pos(34.0f + timeBase, 407.0f);
            textdraw_print(":");
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(3.0f + (74.0f + timeBase), 410.0f);
            textdraw_print(";");
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(-1);
            textdraw_set_pos(74.0f + timeBase, 407.0f);
            textdraw_print(";");
        }

        draw_face((struct NameEntryData *)&lbl_1_bss_D5E60, 32.0f + (543.0f + fx), 419.0f);
        if (lbl_1_bss_D5E60.unk2C & 1)
        {
            REQ(6D4FC)->x = 588.0f + fx;
            REQ(6D4FC)->y = 401.0f;
            draw_sprite_draw_request(REQ(6D4FC));
        }
        if (lbl_1_bss_D5C10.unk4 < 0x78 && lbl_1_bss_D5C10.unk8 == 0x15)
            draw_press_a_button();
        reset_ui_widescreen_scale_mtx();
        break;
    }
    }
}

void fn_1_12CBF0(int shared)
{
    float opacity;
    int bar0 = 0x29;
    int barY;
    int nameX = 0xC0;
    int floorX = 0x18B;
    int timeX = 0x216;

    switch (lbl_1_bss_D5C10.unk8)
    {
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        shared = get_result_panel_offset();

        fn_1_139CE0();
        update_prompt_position();
        draw_sprite_draw_request(REQ(6CD58));
        draw_default_prompt();

        set_ui_widescreen_scale_mtx(0x140);
        barY = 29;
        REQ(6CDA8)->x = shared + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = shared + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = shared + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CE48)->x = shared + floorX;
        REQ(6CE48)->y = barY;
        FADE_REQ(6CE48);
        draw_sprite_draw_request(REQ(6CE48));
        reset_ui_widescreen_scale_mtx();
        break;
    }
    case 19:
    case 20:
    case 21:
    {
        char numBuf[4];
        int y;
        int row;
        int x;
        float fx;
        float rowY;

        shared = 0x44;
        x = get_ranking_panel_offset();
        y = update_mode_banner_y();
        fx = x;
        rowY = 122.0f;
        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request(REQ(6CD08));
        {
            int course = lbl_1_bss_D5C10.unk50 - 12;
            char *strs[][4] = {
            { "Target -3 rounds 1 ball-", "Target -6 rounds 1 ball-", "Target -3 rounds 5 balls-", "Target -6 rounds 5 balls-" },
            { "Ziel -3Runden 1?\?-", "Ziel -6Runden 1?\?-", "Ziel -3Runden 5?\?-", "Ziel -6Runden 5?\?-" },
            { "Target -3rounds 1?\?-", "Target -6rounds 1?\?-", "Target -3rounds 5?\?-", "Target -6rounds 5?\?-" },
            { "Blanco -3rondas 1?\?-", "Blanco -6rondas 1?\?-", "Blanco -3rondas 5?\?-", "Blanco -6rondas 5?\?-" },
            { "Obiettivo -3round 1?\?-", "Obiettivo -6round 1?\?-", "Obiettivo -3round 5?\?-", "Obiettivo -6round 5?\?-" },
            { "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x20\x2D\x33\x83\x89\x83\x45\x83\x93\x83\x68\x20\x31\x83\x7B\x81\x5B\x83\x8B\x2D", "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x20\x2D\x36\x83\x89\x83\x45\x83\x93\x83\x68\x20\x31\x83\x7B\x81\x5B\x83\x8B\x2D", "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x20\x2D\x33\x83\x89\x83\x45\x83\x93\x83\x68\x20\x35\x83\x7B\x81\x5B\x83\x8B\x2D", "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x20\x2D\x36\x83\x89\x83\x45\x83\x93\x83\x68\x20\x35\x83\x7B\x81\x5B\x83\x8B\x2D" },
            };
            char *str;

            textdraw_reset();
            textdraw_set_flags(0xA0000000);
            textdraw_set_font(3);
            textdraw_set_alignment(4);
            textdraw_set_scale(0.9f, 0.9f);
            str = strs[locale][course];
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(323.0f, 3.0f + y);
            textdraw_print(str);
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_pos(320.0f, y);
            textdraw_print(str);
            textdraw_set_scale(1.0f, 1.0f);
        }

        barY = 0x5B;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CE48)->x = x + floorX;
        REQ(6CE48)->y = barY;
        FADE_REQ(6CE48);
        draw_sprite_draw_request(REQ(6CE48));
        reset_ui_widescreen_scale_mtx();

        for (row = 0; row < 5; row++)
        {
            fn_1_125E90(fx, rowY, row);
            set_ui_widescreen_scale_mtx(0x140);
            fn_1_124484(4.0f + (32.0f + fx), rowY - 1.0f, row);
            fn_1_1247EC(4.0f + (115.0f + fx), rowY - 1.0f, (u8 *)&lbl_1_bss_D5C70[row], row);
            textdraw_reset();
            textdraw_set_font(0x21);
            if (row == lbl_1_bss_D5C10.rank - 1 && lbl_1_bss_D5885 != 0)
                textdraw_set_add_color(blink_color());
            {
            BOOL printing;

            nameX = lbl_1_bss_D5C70[row].score;
            printing = FALSE;
            for (barY = 1000000, floorX = 0; barY > 0; barY /= 10, floorX++)
            {
                if (printing != FALSE || barY < 10 || nameX / barY >= 0)
                {
                    sprintf(numBuf, "%d", nameX / barY);
                    PRINT_SHADOWED_F((350.0f + fx) + (float)(floorX * 0x11), rowY, numBuf);
                    nameX %= barY;
                    printing = TRUE;
                }
            }
            }
            REQ(6D36C)->mulColor = 0;
            if (row == lbl_1_bss_D5C10.rank - 1 && lbl_1_bss_D5885 != 0)
                REQ(6D36C)->addColor = blink_color();
            else
                REQ(6D36C)->addColor = 0;
            REQ(6D36C)->z = 2.0f;
            REQ(6D36C)->x = 2.0f + (474.0f + fx);
            REQ(6D36C)->y = 9.0f + (2.0f + rowY);
            draw_sprite_draw_request(REQ(6D36C));
            REQ(6D36C)->mulColor = -1;
            REQ(6D36C)->z = 1.0f;
            REQ(6D36C)->x = 474.0f + fx;
            REQ(6D36C)->y = 9.0f + rowY;
            draw_sprite_draw_request(REQ(6D36C));
            draw_face(&lbl_1_bss_D5C70[row], 32.0f + (543.0f + fx), 12.0f + rowY);
            reset_ui_widescreen_scale_mtx();
            rowY += shared;
        }

        set_ui_widescreen_scale_mtx(0x140);
        if (lbl_1_bss_D5C10.unk4 < 0x78 && lbl_1_bss_D5C10.unk8 == 0x15)
            draw_press_a_button();
        reset_ui_widescreen_scale_mtx();
        break;
    }
    }
}

#define DRAW_SUFFIX(n)                                                \
    REQ(n)->flags |= 5;                                               \
    REQ(n)->mulColor = 0;                                             \
    if (lbl_1_bss_D5C70[row].unk20 > 0)                               \
        REQ(n)->addColor = blink_color();                             \
    else                                                              \
        REQ(n)->addColor = 0;                                         \
    REQ(n)->z = 2.0f;                                                 \
    REQ(n)->x = 2.0f + (450.0f + fx);                                 \
    REQ(n)->y = 2.0f + (9.0f + rowY);                                 \
    draw_sprite_draw_request(REQ(n));                                 \
    REQ(n)->mulColor = -1;                                            \
    REQ(n)->z = 1.0f;                                                 \
    REQ(n)->x = 450.0f + fx;                                          \
    REQ(n)->y = 9.0f + rowY;                                          \
    draw_sprite_draw_request(REQ(n))

void fn_1_12E8D8(int shared)
{
    float opacity;
    int bar0 = 0x29;
    int barY;
    int nameX = 0xC0;
    int floorX = 0x18B;
    int timeX = 0x216;

    switch (lbl_1_bss_D5C10.unk8)
    {
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        shared = get_result_panel_offset();

        fn_1_13A4E0();
        update_prompt_position();
        draw_sprite_draw_request(REQ(6CD58));
        draw_default_prompt();

        set_ui_widescreen_scale_mtx(0x140);
        barY = 29;
        REQ(6CDA8)->x = shared + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = shared + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = shared + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CE48)->x = shared + floorX;
        REQ(6CE48)->y = barY;
        FADE_REQ(6CE48);
        draw_sprite_draw_request(REQ(6CE48));
        reset_ui_widescreen_scale_mtx();
        break;
    }
    case 19:
    case 20:
    case 21:
    {
        int y;
        int row;
        int x;
        float fx;
        float rowY;

        shared = 0x44;
        x = get_ranking_panel_offset();
        y = update_mode_banner_y();
        fx = x;
        rowY = 122.0f;
        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request(REQ(6CD08));
        {
            int course = lbl_1_bss_D5C10.unk50 - 16;
            char *strs[][7] = {
            { "Bowling -Normal Rule-", "Bowling -9 pins Rule-", "Bowling -Strike Rule-", "Bowling -Challenge Mode-", "Bowling -Special Mode lv.1-", "Bowling -Special Mode lv.2-", "Bowling -Special Mode lv.3-" },
            { "Bowling -Normal Rule-", "Bowling -9 pins Rule-", "Bowling -Strike Rule-", "Bowling -Herausforder-", "Bowling -Special Mode lv.1-", "Bowling -Special Mode lv.2-", "Bowling -Special Mode lv.3-" },
            { "Bowling -Normal Rule-", "Bowling -9 pins Rule-", "Bowling -Strike Rule-", "Bowling -mode Challenge-", "Bowling -Special Mode lv.1-", "Bowling -Special Mode lv.2-", "Bowling -Special Mode lv.3-" },
            { "Bolos -Normal Rule-", "Bolos -9 pins Rule-", "Bolos -Strike Rule-", "\x42\x6F\x6C\x6F\x73\x20\x2D\x4D\x64\x2E\x44\x65\x73\x61\x66\xED\x6F", "Bolos -Special Mode lv.1-", "Bolos -Special Mode lv.2-", "Bolos -Special Mode lv.3-" },
            { "Bowling -Normal Rule-", "Bowling -9 pins Rule-", "Bowling -Strike Rule-", "Bowling -mod.sfida-", "Bowling -Special Mode lv.1-", "Bowling -Special Mode lv.2-", "Bowling -Special Mode lv.3-" },
            { "\x83\x7B\x83\x45\x83\x8A\x83\x93\x83\x4F\x20\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x83\x8B\x81\x5B\x83\x8B", "\x83\x7B\x83\x45\x83\x8A\x83\x93\x83\x4F\x20\x39\x20\x70\x69\x6E\x73\x20\x83\x8B\x81\x5B\x83\x8B", "\x83\x7B\x83\x45\x83\x8A\x83\x93\x83\x4F\x20\x83\x58\x83\x67\x83\x89\x83\x43\x83\x4E\x20\x83\x8B\x81\x5B\x83\x8B", "\x83\x7B\x83\x45\x83\x8A\x83\x93\x83\x4F\x20\x83\x60\x83\x83\x83\x8C\x83\x93\x83\x57\x83\x82\x81\x5B\x83\x68", "\x83\x7B\x83\x45\x83\x8A\x83\x93\x83\x4F\x20\x83\x58\x83\x79\x83\x56\x83\x83\x83\x8B\x20\x83\x8C\x83\x78\x83\x8B\x31", "\x83\x7B\x83\x45\x83\x8A\x83\x93\x83\x4F\x20\x83\x58\x83\x79\x83\x56\x83\x83\x83\x8B\x20\x83\x8C\x83\x78\x83\x8B\x32", "\x83\x7B\x83\x45\x83\x8A\x83\x93\x83\x4F\x20\x83\x58\x83\x79\x83\x56\x83\x83\x83\x8B\x20\x83\x8C\x83\x78\x83\x8B\x33" },
            };
            char *str;

            textdraw_reset();
            textdraw_set_flags(0xA0000000);
            textdraw_set_font(3);
            textdraw_set_alignment(4);
            textdraw_set_scale(0.8f, 0.8f);
            str = strs[locale][course];
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(323.0f, 3.0f + y);
            textdraw_print(str);
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_pos(320.0f, y);
            textdraw_print(str);
            textdraw_set_scale(1.0f, 1.0f);
        }

        barY = 0x5B;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CE48)->x = x + floorX;
        REQ(6CE48)->y = barY;
        FADE_REQ(6CE48);
        draw_sprite_draw_request(REQ(6CE48));
        reset_ui_widescreen_scale_mtx();

        for (row = 0; row < 5; row++)
        {
            fn_1_125E90(fx, rowY, row);
            set_ui_widescreen_scale_mtx(0x140);
            fn_1_124484(4.0f + (32.0f + fx), rowY - 1.0, row);
            fn_1_1247EC(4.0f + (115.0f + fx), rowY - 1.0, (u8 *)&lbl_1_bss_D5C70[row], row);
            textdraw_reset();
            textdraw_set_font(0x23);
            textdraw_set_alignment(0);
            if (lbl_1_bss_D5C70[row].unk20 > 0)
                textdraw_set_add_color(blink_color());
            else
                textdraw_set_add_color(0);
            textdraw_set_mul_color(0);
            textdraw_set_depth(2.0f);
            textdraw_set_pos(3.0f + (360.0f + fx), 3.0f + rowY);
            textdraw_printf("%3d", lbl_1_bss_D5C70[row].score);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_depth(1.0f);
            textdraw_set_pos(360.0f + fx, rowY);
            textdraw_printf("%3d", lbl_1_bss_D5C70[row].score);
            if (lbl_1_bss_D5C10.unk50 == 0x13)
            {
                DRAW_SUFFIX(6D31C);
            }
            else
            {
                DRAW_SUFFIX(6D36C);
            }
            draw_face(&lbl_1_bss_D5C70[row], 32.0f + (543.0f + fx), 12.0f + rowY);
            reset_ui_widescreen_scale_mtx();
            rowY += shared;
        }

        set_ui_widescreen_scale_mtx(0x140);
        if (lbl_1_bss_D5C10.unk4 < 0x78 && lbl_1_bss_D5C10.unk8 == 0x15)
            draw_press_a_button();
        reset_ui_widescreen_scale_mtx();
        break;
    }
    }
}

void fn_1_130668(int x, int spacing)
{
    float opacity;
    int bar0 = 0x29;
    int barY;
    int nameX = 0xC0;
    int col3 = 0x15F;
    int col4 = 0x192;
    int col5 = 0x1C6;
    int col6 = 0x22A;

    switch (lbl_1_bss_D5C10.unk8)
    {
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        x = get_result_panel_offset();

        fn_1_13ACFC();
        update_prompt_position();
        draw_sprite_draw_request(REQ(6CD58));
        draw_default_prompt();

        set_ui_widescreen_scale_mtx(0x140);
        barY = 29;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + col6;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6D028)->x = x + col3;
        REQ(6D028)->y = barY;
        FADE_REQ(6D028);
        draw_sprite_draw_request(REQ(6D028));

        REQ(6CFD8)->x = x + col4;
        REQ(6CFD8)->y = barY;
        FADE_REQ(6CFD8);
        draw_sprite_draw_request(REQ(6CFD8));

        REQ(6D078)->x = x + col5;
        REQ(6D078)->y = barY;
        FADE_REQ(6D078);
        draw_sprite_draw_request(REQ(6D078));
        reset_ui_widescreen_scale_mtx();
        break;
    }
    case 19:
    case 20:
    case 21:
    {
        int y;
        int row;
        float fx;
        float rowY;

        spacing = 0x44;
        x = get_ranking_panel_offset();
        y = update_mode_banner_y();
        fx = x;
        rowY = 122.0f;
        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request(REQ(6CD08));
        {
            char *strs[] = { "Golf -full round 18 holes-", "Golf -ger-", "Golf -fra-", "Golf -spa-", "Golf -ita-", "\x83\x53\x83\x8B\x83\x74\x20\x82\x50\x82\x57\x83\x7A\x81\x5B\x83\x8B" };

            textdraw_reset();
            textdraw_set_alignment(4);
            textdraw_set_flags(0xA0000000);
            textdraw_set_font(3);
            textdraw_set_scale(0.9f, 0.9f);
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(323.0f, 3.0f + y);
            textdraw_print("Golf -full round 18 holes-");
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_pos(320.0f, y);
            textdraw_print("Golf -full round 18 holes-");
        }

        barY = 0x5B;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + col6;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6D028)->x = x + col3;
        REQ(6D028)->y = barY;
        FADE_REQ(6D028);
        draw_sprite_draw_request(REQ(6D028));

        REQ(6CFD8)->x = x + col4;
        REQ(6CFD8)->y = barY;
        FADE_REQ(6CFD8);
        draw_sprite_draw_request(REQ(6CFD8));

        REQ(6D078)->x = x + col5;
        REQ(6D078)->y = barY;
        FADE_REQ(6D078);
        draw_sprite_draw_request(REQ(6D078));
        reset_ui_widescreen_scale_mtx();

        for (row = 0; row < 5; row++)
        {
            fn_1_125E90(fx, rowY, row);
            set_ui_widescreen_scale_mtx(0x140);
            fn_1_124484(4.0f + (32.0f + fx), rowY - 1.0f, row);
            fn_1_1247EC(4.0f + (115.0f + fx), rowY - 1.0f, (u8 *)&lbl_1_bss_D5C70[row], row);
            textdraw_set_font(0x21);
            textdraw_set_alignment(3);
            if (row == lbl_1_bss_D5C10.rank - 1 && lbl_1_bss_D5885 != 0)
                textdraw_set_add_color(blink_color());
            if (lbl_1_bss_D5C70[row].unk20 > 0)
                textdraw_set_add_color(blink_color());
            else if (lbl_1_bss_D5C70[row].unk20 == 0)
                textdraw_set_add_color(0);
            textdraw_set_scale(0.8f, 0.8f);
            col5 = x + 0x174;
            textdraw_set_depth(2.0f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(col5 + 2, 5.0f + (2.0f + rowY));
            textdraw_printf("%2d", lbl_1_bss_D5C70[row].unk28);
            textdraw_set_depth(1.0f);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(col5, 5.0f + rowY);
            textdraw_printf("%2d", lbl_1_bss_D5C70[row].unk28);
            col5 = x + 0x1A7;
            textdraw_set_depth(2.0f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(col5 + 2, 5.0f + (2.0f + rowY));
            textdraw_printf("%2d", lbl_1_bss_D5C70[row].unk2A);
            textdraw_set_depth(1.0f);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(col5, 5.0f + rowY);
            textdraw_printf("%2d", lbl_1_bss_D5C70[row].unk2A);
            textdraw_set_scale(1.0f, 1.0f);
            col5 = x + 0x1E4;
            textdraw_set_depth(2.0f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(col5 + 2, 1.0f + (2.0f + rowY));
            textdraw_printf("%3d", lbl_1_bss_D5C70[row].unk2A + lbl_1_bss_D5C70[row].unk28);
            textdraw_set_depth(1.0f);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(col5, 1.0f + rowY);
            textdraw_printf("%3d", lbl_1_bss_D5C70[row].unk2A + lbl_1_bss_D5C70[row].unk28);
            draw_face(&lbl_1_bss_D5C70[row], x + 0x23F, 12.0f + rowY);
            reset_ui_widescreen_scale_mtx();
            rowY += spacing;
        }

        set_ui_widescreen_scale_mtx(0x140);
        if (lbl_1_bss_D5C10.unk4 < 0x78 && lbl_1_bss_D5C10.unk8 == 0x15)
            draw_press_a_button();
        reset_ui_widescreen_scale_mtx();
        break;
    }
    }
}

void fn_1_132BB0(struct Sprite *sprite)
{
    float opacity;
    int bar0 = 0x29;
    int barY;
    int nameX = 0xB8;
    int floorX = 0x18B;
    int timeX = 0x216;

    switch (lbl_1_bss_D5C10.unk8)
    {
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        int x = get_result_panel_offset();

        fn_1_13B55C();
        update_prompt_position();
        draw_sprite_draw_request(REQ(6CD58));
        draw_default_prompt();

        set_ui_widescreen_scale_mtx(0x140);
        barY = 29;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CF88)->x = x + floorX;
        REQ(6CF88)->y = barY;
        FADE_REQ(6CF88);
        draw_sprite_draw_request(REQ(6CF88));
        reset_ui_widescreen_scale_mtx();
        break;
    }
    case 19:
    case 20:
    case 21:
    {
        char numBuf[4];
        char charBuf[4];
        int y;
        int spacing = 0x37;
        int x = get_ranking_panel_offset();
        int row;
        int i;
        int val;
        int n;
        float fx;
        float rowY;

        y = update_mode_banner_y();
        fx = x;
        rowY = 122.0f;
        if (lbl_1_bss_D5C10.unk50 == 0x1A)
            spacing = 0x44;
        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request(REQ(6CD08));
        {
            int course = lbl_1_bss_D5C10.unk50 - 24;
            char *strs[][3] = {
            { "Boat -Flower Garden Side-", "Boat -Woody Arch River-", "Boat -Water Dragon Route-" },
            { "***", "***", "***" },
            { "***", "***", "***" },
            { "***", "***", "***" },
            { "***", "***", "***" },
            { "\x83\x7B\x81\x5B\x83\x67\x20\x2D\x83\x74\x83\x89\x83\x8F\x81\x5B\x83\x4B\x81\x5B\x83\x66\x83\x93\x20\x83\x54\x83\x43\x83\x68\x2D", "\x83\x7B\x81\x5B\x83\x67\x20\x2D\x83\x45\x83\x62\x83\x66\x83\x42\x81\x5B\x83\x41\x81\x5B\x83\x60\x20\x83\x8A\x83\x6F\x81\x5B\x2D", "\x83\x7B\x81\x5B\x83\x67\x20\x2D\x83\x45\x83\x48\x81\x5B\x83\x5E\x81\x5B\x83\x68\x83\x89\x83\x53\x83\x93\x20\x83\x8B\x81\x5B\x83\x67\x2D" },
            };
            char *str;

            textdraw_reset();
            textdraw_set_flags(0xA0000000);
            textdraw_set_font(3);
            textdraw_set_alignment(4);
            textdraw_set_scale(0.9f, 0.9f);
            str = strs[locale][course];
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(323.0f, 3.0f + y);
            textdraw_print(str);
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_pos(320.0f, y);
            textdraw_print(str);
            textdraw_set_scale(1.0f, 1.0f);
        }

        barY = 0x5B;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CF88)->x = x + floorX;
        REQ(6CF88)->y = barY;
        FADE_REQ(6CF88);
        draw_sprite_draw_request(REQ(6CF88));
        reset_ui_widescreen_scale_mtx();

        for (row = 0; row < 5; row++)
        {
            fn_1_125E90(fx, rowY, row);
            set_ui_widescreen_scale_mtx(0x140);
            fn_1_124484(4.0f + (32.0f + fx), rowY - 1.0f, row);
            fn_1_1247EC(4.0f + (115.0f + fx), rowY - 1.0f, (u8 *)&lbl_1_bss_D5C70[row], row);
            fn_1_1254AC(360.0f + fx, rowY, row);
            draw_face(&lbl_1_bss_D5C70[row], 32.0f + (543.0f + fx), 12.0f + rowY);
            reset_ui_widescreen_scale_mtx();
            rowY += spacing;
        }

        if (lbl_1_bss_D5C10.unk50 != 0x1A)
        {
            fn_1_125E90(fx, 407.0f, 5);
            set_ui_widescreen_scale_mtx(0x140);
            REQ(6D45C)->x = 36.0f + fx;
            REQ(6D45C)->y = 407.0f;
            draw_sprite_draw_request(REQ(6D45C));
            textdraw_set_font(0x20);
            textdraw_set_alignment(0);
            n = x + 0x77;
            for (i = 0; i < 6; i++)
            {
                sprintf(charBuf, "%c", ((u8 *)lbl_1_bss_D5E60.name)[i]);
                textdraw_set_depth(0.51f);
                textdraw_set_mul_color(0);
                textdraw_set_pos(3.0f + (float)(n + i * 0x22), 410.0f);
                textdraw_print(charBuf);
                textdraw_set_depth(0.5f);
                textdraw_set_mul_color(-1);
                textdraw_set_pos((float)(n + i * 0x22), 407.0f);
                textdraw_print(charBuf);
            }

            {
                float offs[3] = { 0.0f, 40.0f, 84.0f };
                float px;

                fx = x + 0x168;
                textdraw_reset();
                textdraw_set_font(0x21);
                for (i = 0; i < 3; i++)
                {
                    val = (&lbl_1_bss_D5E60.unk24)[i];
                    if (i == 1)
                        val = CLAMP(val, 0, 59);
                    else
                        val = CLAMP(val, 0, 99);
                    sprintf(numBuf, "%d", val / 10);
                    px = fx + offs[i];
                    textdraw_set_depth(0.51f);
                    textdraw_set_mul_color(0);
                    textdraw_set_pos(3.0f + px, 410.0f);
                    textdraw_print(numBuf);
                    textdraw_set_depth(0.5f);
                    textdraw_set_mul_color(-1);
                    textdraw_set_pos(px, 407.0f);
                    textdraw_print(numBuf);
                    sprintf(numBuf, "%d", val % 10);
                    px = 17.0f + (fx + offs[i]);
                    textdraw_set_depth(0.51f);
                    textdraw_set_mul_color(0);
                    textdraw_set_pos(3.0f + px, 410.0f);
                    textdraw_print(numBuf);
                    textdraw_set_depth(0.5f);
                    textdraw_set_mul_color(-1);
                    textdraw_set_pos(px, 407.0f);
                    textdraw_print(numBuf);
                }
                textdraw_set_depth(0.51f);
                textdraw_set_mul_color(0);
                textdraw_set_pos(3.0f + (34.0f + fx), 410.0f);
                textdraw_print(":");
                textdraw_set_depth(0.5f);
                textdraw_set_mul_color(-1);
                textdraw_set_pos(34.0f + fx, 407.0f);
                textdraw_print(":");
                textdraw_set_depth(0.51f);
                textdraw_set_mul_color(0);
                textdraw_set_pos(3.0f + (74.0f + fx), 410.0f);
                textdraw_print(";");
                textdraw_set_depth(0.5f);
                textdraw_set_mul_color(-1);
                textdraw_set_pos(74.0f + fx, 407.0f);
                textdraw_print(";");
            }

            draw_face((struct NameEntryData *)&lbl_1_bss_D5E60, x + 0x23F, 419.0f);
        }
        if (lbl_1_bss_D5C10.unk4 < 0x78 && lbl_1_bss_D5C10.unk8 == 0x15)
            draw_press_a_button();
        reset_ui_widescreen_scale_mtx();
        break;
    }
    }
}

void fn_1_134A9C(int shared)
{
    float opacity;
    int bar0 = 0x29;
    int barY;
    int nameX = 0xB8;
    int floorX = 0x18B;
    int timeX = 0x216;

    switch (lbl_1_bss_D5C10.unk8)
    {
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        shared = get_result_panel_offset();

        fn_1_13CC90();
        update_prompt_position();
        draw_sprite_draw_request(REQ(6CD58));
        draw_default_prompt();

        set_ui_widescreen_scale_mtx(0x140);
        barY = 29;
        REQ(6CDA8)->x = shared + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = shared + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = shared + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CE48)->x = shared + floorX;
        REQ(6CE48)->y = barY;
        FADE_REQ(6CE48);
        draw_sprite_draw_request(REQ(6CE48));
        reset_ui_widescreen_scale_mtx();
        break;
    }
    case 19:
    case 20:
    case 21:
    {
        char numBuf[4];
        int y;
        int row;
        int x;
        float fx;
        float rowY;

        shared = 0x44;
        x = get_ranking_panel_offset();
        y = update_mode_banner_y();
        fx = x;
        rowY = 122.0f;
        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request(REQ(6CD08));
        {
            int course = lbl_1_bss_D5C10.unk50 - 27;
            char *strs[][3] = {
            { "Shot -Beginner Stage-", "Shot -Advanced Stage-", "Shot -Expert Stage-" },
            { "MONKEY SHOT -lv.1-", "MONKEY SHOT -lv.2-", "MONKEY SHOT -lv.3-" },
            { "MONKEY SHOT -lv.1-", "MONKEY SHOT -lv.2-", "MONKEY SHOT -lv.3-" },
            { "MONKEY SHOT -lv.1-", "MONKEY SHOT -lv.2-", "MONKEY SHOT -lv.3-" },
            { "MONKEY SHOT -lv.1-", "MONKEY SHOT -lv.2-", "MONKEY SHOT -lv.3-" },
            { "\x83\x56\x83\x85\x81\x5B\x83\x65\x83\x42\x83\x93\x83\x4F\x20\x2D\x8F\x89\x8B\x89\x2D", "\x83\x56\x83\x85\x81\x5B\x83\x65\x83\x42\x83\x93\x83\x4F\x20\x2D\x92\x86\x8B\x89\x2D", "\x83\x56\x83\x85\x81\x5B\x83\x65\x83\x42\x83\x93\x83\x4F\x20\x2D\x8F\xE3\x8B\x89\x2D" },
            };
            char *str;

            textdraw_reset();
            textdraw_set_flags(0xA0000000);
            textdraw_set_font(3);
            textdraw_set_alignment(4);
            str = strs[locale][course];
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(323.0f, 3.0f + y);
            textdraw_print(str);
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_pos(320.0f, y);
            textdraw_print(str);
            textdraw_set_scale(1.0f, 1.0f);
        }

        barY = 0x5B;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CE48)->x = x + floorX;
        REQ(6CE48)->y = barY;
        FADE_REQ(6CE48);
        draw_sprite_draw_request(REQ(6CE48));
        reset_ui_widescreen_scale_mtx();

        for (row = 0; row < 5; row++)
        {
            fn_1_125E90(fx, rowY, row);
            set_ui_widescreen_scale_mtx(0x140);
            fn_1_124484(4.0f + (32.0f + fx), rowY - 1.0f, row);
            fn_1_1247EC(4.0f + (115.0f + fx), rowY - 1.0f, (u8 *)&lbl_1_bss_D5C70[row], row);
            textdraw_reset();
            textdraw_set_font(0x21);
            if (lbl_1_bss_D5C70[row].unk20 > 0)
                textdraw_set_add_color(blink_color());
            else
                textdraw_set_add_color(0);
            {
            BOOL printing;

            nameX = lbl_1_bss_D5C70[row].score;
            printing = FALSE;
            for (barY = 1000000, floorX = 0; barY > 0; barY /= 10, floorX++)
            {
                if (printing != FALSE || barY < 10 || nameX / barY >= 0)
                {
                    sprintf(numBuf, "%d", nameX / barY);
                    PRINT_SHADOWED_F((350.0f + fx) + (float)(floorX * 0x11), rowY, numBuf);
                    nameX %= barY;
                    printing = FALSE;
                }
            }
            }
            REQ(6D36C)->mulColor = 0;
            if (lbl_1_bss_D5C70[row].unk20 > 0)
                REQ(6D36C)->addColor = blink_color();
            else
                REQ(6D36C)->addColor = 0;
            REQ(6D36C)->z = 2.0f;
            REQ(6D36C)->x = 2.0f + (474.0f + fx);
            REQ(6D36C)->y = 2.0f + (9.0f + rowY);
            draw_sprite_draw_request(REQ(6D36C));
            REQ(6D36C)->mulColor = -1;
            REQ(6D36C)->z = 1.0f;
            REQ(6D36C)->x = 474.0f + fx;
            REQ(6D36C)->y = 9.0f + rowY;
            draw_sprite_draw_request(REQ(6D36C));
            draw_face(&lbl_1_bss_D5C70[row], 32.0f + (543.0f + fx), 12.0f + rowY);
            reset_ui_widescreen_scale_mtx();
            rowY += shared;
        }

        set_ui_widescreen_scale_mtx(0x140);
        if (lbl_1_bss_D5C10.unk4 < 0x78 && lbl_1_bss_D5C10.unk8 == 0x15)
            draw_press_a_button();
        reset_ui_widescreen_scale_mtx();
        break;
    }
    }
}

void fn_1_136764(int shared)
{
    float opacity;
    int bar0 = 0x29;
    int barY;
    int nameX = 0xB8;
    int floorX = 0x18B;
    int timeX = 0x216;

    switch (lbl_1_bss_D5C10.unk8)
    {
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        shared = get_result_panel_offset();

        fn_1_13D54C();
        update_prompt_position();
        draw_sprite_draw_request(REQ(6CD58));
        draw_default_prompt();

        set_ui_widescreen_scale_mtx(0x140);
        barY = 29;
        REQ(6CDA8)->x = shared + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = shared + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = shared + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CE48)->x = shared + floorX;
        REQ(6CE48)->y = barY;
        FADE_REQ(6CE48);
        draw_sprite_draw_request(REQ(6CE48));
        reset_ui_widescreen_scale_mtx();
        break;
    }
    case 19:
    case 20:
    case 21:
    {
        char numBuf[4];
        int y;
        int row;
        int x;
        float fx;
        float rowY;

        shared = 0x44;
        x = get_ranking_panel_offset();
        y = update_mode_banner_y();
        fx = x;
        rowY = 122.0f;
        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request(REQ(6CD08));
        reset_ui_widescreen_scale_mtx();
        {
            int course = lbl_1_bss_D5C10.unk50 - 30;
            char *strs[][3] = {
            { "Dogfight -3 minutes-", "Dogfight -5 minutes-", "Dogfight -10 minutes-" },
            { "DOGFIGHT -3 minutes-", "DOGFIGHT -5 minutes-", "DOGFIGHT -10 minutes-" },
            { "DOGFIGHT -3 minutes-", "DOGFIGHT -5 minutes-", "DOGFIGHT -10 minutes-" },
            { "DOGFIGHT -3 minutes-", "DOGFIGHT -5 minutes-", "DOGFIGHT -10 minutes-" },
            { "DOGFIGHT -3 minutes-", "DOGFIGHT -5 minutes-", "DOGFIGHT -10 minutes-" },
            { "\x83\x68\x83\x62\x83\x4F\x83\x74\x83\x40\x83\x43\x83\x67\x20\x2D\x33\x95\xAA\x2D", "\x83\x68\x83\x62\x83\x4F\x83\x74\x83\x40\x83\x43\x83\x67\x20\x2D\x35\x95\xAA\x2D", "\x83\x68\x83\x62\x83\x4F\x83\x74\x83\x40\x83\x43\x83\x67\x20\x2D\x31\x30\x95\xAA\x2D" },
            };
            char *str;

            textdraw_reset();
            set_ui_widescreen_scale_mtx(0x140);
            textdraw_set_flags(0xA0000000);
            textdraw_set_font(3);
            textdraw_set_alignment(4);
            textdraw_set_scale(0.9f, 0.9f);
            str = strs[locale][course];
            textdraw_set_depth(0.51f);
            textdraw_set_mul_color(0);
            textdraw_set_pos(323.0f, 3.0f + y);
            textdraw_print(str);
            textdraw_set_depth(0.5f);
            textdraw_set_mul_color(0xFFFF00);
            textdraw_set_pos(320.0f, y);
            textdraw_print(str);
            textdraw_set_scale(1.0f, 1.0f);
            reset_ui_widescreen_scale_mtx();
        }

        set_ui_widescreen_scale_mtx(0x140);

        barY = 0x5B;
        REQ(6CDA8)->x = x + bar0;
        REQ(6CDA8)->y = barY;
        FADE_REQ(6CDA8);
        draw_sprite_draw_request(REQ(6CDA8));

        REQ(6CDF8)->x = x + nameX;
        REQ(6CDF8)->y = barY;
        FADE_REQ(6CDF8);
        draw_sprite_draw_request(REQ(6CDF8));

        REQ(6CF38)->x = x + timeX;
        REQ(6CF38)->y = barY;
        FADE_REQ(6CF38);
        draw_sprite_draw_request(REQ(6CF38));

        REQ(6CE48)->x = x + floorX;
        REQ(6CE48)->y = barY;
        FADE_REQ(6CE48);
        draw_sprite_draw_request(REQ(6CE48));
        reset_ui_widescreen_scale_mtx();

        for (row = 0; row < 5; row++)
        {
            fn_1_125E90(fx, rowY, row);
            set_ui_widescreen_scale_mtx(0x140);
            fn_1_124484(4.0f + (32.0f + fx), rowY - 1.0f, row);
            fn_1_1247EC(4.0f + (115.0f + fx), rowY - 1.0f, (u8 *)&lbl_1_bss_D5C70[row], row);
            textdraw_reset();
            textdraw_set_font(0x21);
            if (row == lbl_1_bss_D5C10.rank - 1 && lbl_1_bss_D5885 != 0)
                textdraw_set_add_color(blink_color());
            {
            BOOL printing;

            nameX = lbl_1_bss_D5C70[row].score;
            printing = FALSE;
            for (barY = 1000000, floorX = 0; barY > 0; barY /= 10, floorX++)
            {
                if (printing != FALSE || barY < 10 || nameX / barY >= 0)
                {
                    sprintf(numBuf, "%d", nameX / barY);
                    PRINT_SHADOWED_F((350.0f + fx) + (float)(floorX * 0x11), rowY, numBuf);
                    nameX %= barY;
                    printing = TRUE;
                }
            }
            }
            REQ(6D36C)->mulColor = 0;
            if (row == lbl_1_bss_D5C10.rank - 1 && lbl_1_bss_D5885 != 0)
                REQ(6D36C)->addColor = blink_color();
            else
                REQ(6D36C)->addColor = 0;
            REQ(6D36C)->z = 2.0f;
            REQ(6D36C)->x = 2.0f + (474.0f + fx);
            REQ(6D36C)->y = 2.0f + (9.0f + rowY);
            draw_sprite_draw_request(REQ(6D36C));
            REQ(6D36C)->mulColor = -1;
            REQ(6D36C)->z = 1.0f;
            REQ(6D36C)->x = 474.0f + fx;
            REQ(6D36C)->y = 9.0f + rowY;
            draw_sprite_draw_request(REQ(6D36C));
            draw_face(&lbl_1_bss_D5C70[row], 32.0f + (543.0f + fx), 12.0f + rowY);
            reset_ui_widescreen_scale_mtx();
            rowY += shared;
        }

        set_ui_widescreen_scale_mtx(0x140);
        if (lbl_1_bss_D5C10.unk4 < 0x78 && lbl_1_bss_D5C10.unk8 == 0x15)
            draw_press_a_button();
        reset_ui_widescreen_scale_mtx();
        break;
    }
    }
}

#define DRAW_SUFFIX(n)                                                \
    REQ(n)->flags |= 5;                                               \
    REQ(n)->mulColor = 0;                                             \
    if (lbl_1_bss_D5C70[row].unk20 > 0)                               \
        REQ(n)->addColor = blink_color();                             \
    else                                                              \
        REQ(n)->addColor = 0;                                         \
    REQ(n)->z = 2.0f;                                                 \
    REQ(n)->x = 2.0f + (450.0f + fx);                                 \
    REQ(n)->y = 2.0f + (9.0f + rowY);                                 \
    draw_sprite_draw_request(REQ(n));                                 \
    REQ(n)->mulColor = -1;                                            \
    REQ(n)->z = 1.0f;                                                 \
    REQ(n)->x = 450.0f + fx;                                          \
    REQ(n)->y = 9.0f + rowY;                                          \
    draw_sprite_draw_request(REQ(n))

#define PRINT_SHADOWED_II(px, py, str)                     \
    textdraw_set_depth(0.51f);                             \
    textdraw_set_mul_color(0);                             \
    textdraw_set_pos(3.0f + (float)(px), 3.0f + (float)(py)); \
    textdraw_print(str);                                   \
    textdraw_set_depth(0.5f);                              \
    textdraw_set_mul_color(-1);                            \
    textdraw_set_pos((float)(px), (float)(py));            \
    textdraw_print(str)

#define PRINT_SHADOWED_FI(px, py, str)                     \
    textdraw_set_depth(0.51f);                             \
    textdraw_set_mul_color(0);                             \
    textdraw_set_pos(3.0f + (px), 3.0f + (float)(py));     \
    textdraw_print(str);                                   \
    textdraw_set_depth(0.5f);                              \
    textdraw_set_mul_color(-1);                            \
    textdraw_set_pos((px), (float)(py));                   \
    textdraw_print(str)

#define CLAMP_TIME(val, i)     \
    if ((i) == 1)              \
        val = CLAMP(val, 0, 59); \
    else                       \
        val = CLAMP(val, 0, 99)

static inline void draw_face_sys(float px, float py)
{
    int c = lbl_1_data_6BAE0[0];
    int b = lbl_1_bss_D587C;
    int a = lbl_1_bss_D5C10.apeType;

    if (a >= 0 && a < 4)
    {
        struct SpriteDrawRequest *req;
        u16 *tbl = ((u16 **)lbl_1_data_6D5EC)[a];

        req = REQ(6D59C);
        req->bmpId = tbl[c];
        req->x = px;
        req->y = py;
        req->z = 1.0f;
        req->opacity = 1.0f;
        req->rotation = 5120.0f * math_sin(g_font_char_var_3 << 9);
        draw_sprite_draw_request(req);
        req = REQ(6D59C);
        req->bmpId = tbl[b];
        req->z = 0.5f;
        req->opacity = 1.0 - (g_font_char_var_3 % 10) / 10.0;
        draw_sprite_draw_request(req);
    }
}

void fn_1_138464(void)
{
    char numBuf[4];
    char numBuf2[4];
    char numBuf3[4];
    int y;
    int x = get_result_panel_offset();

    y = 0x36;

    textdraw_reset();
    if (lbl_1_bss_D5C10.unk54 & 4)
    {
        int i;
        int n;

        REQ(6D118)->x = x;
        REQ(6D118)->y = y;
        REQ(6D118)->addColor = 0;
        draw_sprite_draw_request(REQ(6D118));
        set_ui_widescreen_scale_mtx(0x140);
        REQ(6D45C)->x = x + 0x24;
        REQ(6D45C)->y = y;
        draw_sprite_draw_request(REQ(6D45C));
        textdraw_set_font(0x20);
        textdraw_set_alignment(0);
        n = x + 0x77;
        {
        char charBuf[4];

        for (i = 0; i < 6; i++)
        {
            sprintf(charBuf, "%c", lbl_1_bss_D5C10.name[i]);
            PRINT_SHADOWED_II(n + i * 0x22, y, charBuf);
        }
        }
        {
            float offs[3] = { 0.0f, 40.0f, 84.0f };
            int val;
            float px;

            n = x + 0x168;
            textdraw_reset();
            textdraw_set_font(0x21);
            for (i = 0; i < 3; i++)
            {
                val = (&lbl_1_bss_D5E60.unk24)[i];
                CLAMP_TIME(val, i);
                sprintf(numBuf, "%d", val / 10);
                px = (float)n + offs[i];
                PRINT_SHADOWED_FI(px, y, numBuf);
                sprintf(numBuf, "%d", val % 10);
                px = 17.0f + ((float)n + offs[i]);
                PRINT_SHADOWED_FI(px, y, numBuf);
            }
            PRINT_SHADOWED_II(n + 0x22, y, ":");
            PRINT_SHADOWED_II(n + 0x4A, y, ";");
        }
        if (lbl_1_bss_D5E60.unk2C & 1)
        {
            REQ(6D4FC)->x = x + 0x24C;
            REQ(6D4FC)->y = y - 6;
            draw_sprite_draw_request(REQ(6D4FC));
        }
        draw_face((struct NameEntryData *)&lbl_1_bss_D5E60, x + 0x23F, y + 0xC);
        reset_ui_widescreen_scale_mtx();
    }
    else
    {
        int n;
        int i;

        REQ(6D118)->x = x;
        REQ(6D118)->y = y;
        REQ(6D118)->addColor = 0;
        draw_sprite_draw_request(REQ(6D118));
        set_ui_widescreen_scale_mtx(0x140);
        textdraw_set_font(0x22);
        textdraw_set_alignment(0);
        n = x + 0x24;
        {
        char rankBuf[4];

        sprintf(rankBuf, "%d", lbl_1_bss_D5C10.rank);
        PRINT_SHADOWED_II(n, y, rankBuf);
        }
        textdraw_set_font(0x20);
        textdraw_set_alignment(0);
        n = x + 0x77;
        {
        char charBuf[4];

        for (i = 0; i < 6; i++)
        {
            sprintf(charBuf, "%c", lbl_1_bss_D5C10.name[i]);
            PRINT_SHADOWED_II(n + i * 0x22, y, charBuf);
        }
        }
        n = x + 0x168;
        {
            float offs[3] = { 0.0f, 40.0f, 84.0f };
            int val;
            float px;

            textdraw_reset();
            textdraw_set_font(0x21);
            for (i = 0; i < 3; i++)
            {
                val = (&lbl_1_bss_D5C10.unk10)[i];
                CLAMP_TIME(val, i);
                sprintf(numBuf2, "%d", val / 10);
                px = (float)n + offs[i];
                PRINT_SHADOWED_FI(px, y, numBuf2);
                sprintf(numBuf2, "%d", val % 10);
                px = 17.0f + ((float)n + offs[i]);
                PRINT_SHADOWED_FI(px, y, numBuf2);
            }
            PRINT_SHADOWED_II(n + 0x22, y, ":");
            PRINT_SHADOWED_II(n + 0x4A, y, ";");
        }
        if (lbl_1_bss_D5C10.unk44 & 1)
        {
            REQ(6D4FC)->x = x + 0x24C;
            REQ(6D4FC)->y = y - 6;
            draw_sprite_draw_request(REQ(6D4FC));
        }
        draw_face_sys(x + 0x23F, y + 0xC);
        reset_ui_widescreen_scale_mtx();
        if (lbl_1_bss_D5C10.unk54 & 2)
        {
            int i;
            int n;

            REQ(6D118)->x = x;
            REQ(6D118)->y = y + 0x28;
            REQ(6D118)->addColor = 0;
            draw_sprite_draw_request(REQ(6D118));
            set_ui_widescreen_scale_mtx(0x140);
            REQ(6D45C)->x = x + 0x24;
            REQ(6D45C)->y = y + 0x28;
            draw_sprite_draw_request(REQ(6D45C));
            textdraw_set_font(0x20);
            textdraw_set_alignment(0);
            n = x + 0x77;
            {
            char charBuf2[4];

            for (i = 0; i < 6; i++)
            {
                sprintf(charBuf2, "%c", lbl_1_bss_D5C10.name[i]);
                PRINT_SHADOWED_II(n + i * 0x22, y + 0x28, charBuf2);
            }
            }
            {
                float offs[3] = { 0.0f, 40.0f, 84.0f };
                float fx;
                int val;
                float px;

                fx = x + 0x168;
                textdraw_reset();
                textdraw_set_font(0x21);
                for (i = 0; i < 3; i++)
                {
                    val = (&lbl_1_bss_D5E60.unk24)[i];
                    CLAMP_TIME(val, i);
                    sprintf(numBuf3, "%d", val / 10);
                    px = fx + offs[i];
                    PRINT_SHADOWED_FI(px, y + 0x28, numBuf3);
                    sprintf(numBuf3, "%d", val % 10);
                    px = 17.0f + (fx + offs[i]);
                    PRINT_SHADOWED_FI(px, y + 0x28, numBuf3);
                }
                PRINT_SHADOWED_FI(34.0f + fx, y + 0x28, ":");
                PRINT_SHADOWED_FI(74.0f + fx, y + 0x28, ";");
            }
            draw_face((struct NameEntryData *)&lbl_1_bss_D5E60, x + 0x23F, y + 0x34);
            if (lbl_1_bss_D5E60.unk2C & 1)
            {
                REQ(6D4FC)->x = x + 0x24C;
                REQ(6D4FC)->y = y + 0x22;
                draw_sprite_draw_request(REQ(6D4FC));
            }
            reset_ui_widescreen_scale_mtx();
        }
    }
}

void fn_1_139CE0(void)
{
    char numBuf[4];
    int x = get_result_panel_offset();
    float fx;
    float y;
    int digit;
    int i;
    int divisor;
    BOOL printing;

    y = 54.0f;

    textdraw_reset();
    REQ(6D118)->x = x;
    REQ(6D118)->y = y;
    REQ(6D118)->addColor = 0;
    draw_sprite_draw_request(REQ(6D118));
    set_ui_widescreen_scale_mtx(0x140);
    if (lbl_1_bss_D5885 != 0)
    {
        lbl_1_data_6D308[0]->x = x + 0xF;
        lbl_1_data_6D308[0]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[0]);
    }
    else
    {
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->x = x + 0xF;
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]);
    }
    textdraw_set_font(0x22);
    textdraw_set_alignment(0);
    fx = x + 0x24;
    textdraw_set_depth(2.0f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(2.0f + fx, 2.0f + y);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_pos(fx, y);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_font(0x20);
    textdraw_set_alignment(0);
    fx = x + 0x77;
    for (i = 0; i < 6; i++)
    {
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        textdraw_set_pos((2.0f + fx) + (float)(i * 0x22), 2.0f + y);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(fx + (float)(i * 0x22), y);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
    }
    fx = x + 0x15E;
    textdraw_set_font(0x21);
    textdraw_set_alignment(0);
    i = lbl_1_bss_D5C10.score;
    printing = FALSE;
    for (divisor = 1000000, digit = 0; divisor > 0; divisor /= 10, digit++)
    {
        if (printing != FALSE || divisor < 10 || i / divisor >= 0)
        {
            sprintf(numBuf, "%d", i / divisor);
            PRINT_SHADOWED_F(fx + (float)(digit * 0x11), y, numBuf);
            i %= divisor;
            printing = TRUE;
        }
    }
    fx = x + 0x1DA;
    REQ(6D36C)->mulColor = 0;
    REQ(6D36C)->z = 2.0f;
    REQ(6D36C)->x = 2.0 + fx;
    REQ(6D36C)->y = 2.0 + (9.0 + y);
    draw_sprite_draw_request(REQ(6D36C));
    REQ(6D36C)->mulColor = -1;
    REQ(6D36C)->z = 1.0f;
    REQ(6D36C)->x = fx;
    REQ(6D36C)->y = 9.0 + y;
    draw_sprite_draw_request(REQ(6D36C));
    draw_face_sys(x + 0x23F, 12.0f + y);
    reset_ui_widescreen_scale_mtx();
}

void fn_1_13A4E0(void)
{
    char numBuf[4];
    int x = get_result_panel_offset();
    float fx;
    float y;
    int i;

    y = 54.0f;

    textdraw_reset();
    REQ(6D118)->x = x;
    REQ(6D118)->y = y;
    REQ(6D118)->addColor = 0;
    draw_sprite_draw_request(REQ(6D118));
    set_ui_widescreen_scale_mtx(0x140);
    if (lbl_1_bss_D5885 != 0)
    {
        lbl_1_data_6D308[0]->x = x + 0xF;
        lbl_1_data_6D308[0]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[0]);
    }
    else
    {
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->x = x + 0xF;
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]);
    }
    textdraw_set_font(0x22);
    textdraw_set_alignment(0);
    fx = x + 0x24;
    textdraw_set_depth(2.0f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(2.0f + fx, 2.0f + y);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_pos(fx, y);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_font(0x20);
    textdraw_set_alignment(0);
    fx = x + 0x77;
    for (i = 0; i < 6; i++)
    {
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        textdraw_set_pos((2.0f + fx) + (float)(i * 0x22), 2.0f + y);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(fx + (float)(i * 0x22), y);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
    }
    textdraw_set_font(0x23);
    textdraw_set_alignment(0);
    sprintf(numBuf, "%3d", lbl_1_bss_D5C10.score);
    PRINT_SHADOWED(x + 0x168, y, numBuf);
    if (lbl_1_bss_D5C10.unk50 == 0x13)
    {
        fx = x + 0x1C7;
        REQ(6D31C)->mulColor = 0;
        REQ(6D31C)->z = 2.0f;
        REQ(6D31C)->x = 2.0f + fx;
        REQ(6D31C)->y = 9.0f + (2.0f + y);
        draw_sprite_draw_request(REQ(6D31C));
        REQ(6D31C)->mulColor = -1;
        REQ(6D31C)->z = 1.0f;
        REQ(6D31C)->x = fx;
        REQ(6D31C)->y = 9.0f + y;
        draw_sprite_draw_request(REQ(6D31C));
    }
    else
    {
        fx = x + 0x1C7;
        REQ(6D36C)->mulColor = 0;
        REQ(6D36C)->z = 2.0f;
        REQ(6D36C)->x = fx;
        REQ(6D36C)->y = 2.0 + (9.0 + y);
        draw_sprite_draw_request(REQ(6D36C));
        REQ(6D36C)->mulColor = -1;
        REQ(6D36C)->z = 1.0f;
        REQ(6D36C)->x = fx;
        REQ(6D36C)->y = 9.0 + y;
        draw_sprite_draw_request(REQ(6D36C));
    }
    draw_face_sys(x + 0x23F, 12.0f + y);
    reset_ui_widescreen_scale_mtx();
}

void fn_1_13ACFC(void)
{
    char buf0[4];
    char buf1[4];
    char buf2[4];
    int x = get_result_panel_offset();
    float fx;
    float y;
    int i;

    y = 54.0f;

    textdraw_reset();
    REQ(6D118)->x = x;
    REQ(6D118)->y = y;
    REQ(6D118)->addColor = 0;
    draw_sprite_draw_request(REQ(6D118));
    set_ui_widescreen_scale_mtx(0x140);
    if (lbl_1_bss_D5885 != 0)
    {
        lbl_1_data_6D308[0]->x = (int)(15.0 + x);
        lbl_1_data_6D308[0]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[0]);
    }
    else
    {
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->x = (int)(15.0 + x);
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]);
    }
    textdraw_set_font(0x22);
    textdraw_set_alignment(0);
    fx = x + 0x24;
    textdraw_set_depth(2.0f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(2.0f + fx, 2.0f + y);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_pos(fx, y);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_font(0x20);
    textdraw_set_alignment(0);
    fx = x + 0x77;
    for (i = 0; i < 6; i++)
    {
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        textdraw_set_pos((2.0f + fx) + (float)(i * 0x22), 2.0f + y);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(fx + (float)(i * 0x22), y);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
    }
    textdraw_set_font(0x21);
    textdraw_set_alignment(3);
    textdraw_set_scale(0.8f, 0.8f);
    fx = x + 0x174;
    sprintf(buf0, "%2d", lbl_1_bss_D5C10.unk14);
    PRINT_SHADOWED_F(fx, 5.0f + y, buf0);
    fx = x + 0x1A7;
    sprintf(buf1, "%2d", lbl_1_bss_D5C10.unk16);
    PRINT_SHADOWED_F(fx, 5.0f + y, buf1);
    textdraw_set_scale(1.0f, 1.0f);
    fx = x + 0x1E4;
    sprintf(buf2, "%3d", lbl_1_bss_D5C10.unk16 + lbl_1_bss_D5C10.unk14);
    PRINT_SHADOWED_F(fx, 1.0f + y, buf2);
    draw_face_sys(x + 0x23F, 12.0f + y);
    reset_ui_widescreen_scale_mtx();
}

void fn_1_13B55C(void)
{
    char numBuf[4];
    char numBuf2[4];
    char numBuf3[4];
    int y;
    int x = get_result_panel_offset();

    y = 0x36;

    textdraw_reset();
    if (lbl_1_bss_D5C10.unk54 & 4)
    {
        int i;
        int n;

        REQ(6D118)->x = x;
        REQ(6D118)->y = y;
        REQ(6D118)->addColor = 0;
        draw_sprite_draw_request(REQ(6D118));
        set_ui_widescreen_scale_mtx(0x140);
        REQ(6D45C)->x = x + 0x24;
        REQ(6D45C)->y = y;
        draw_sprite_draw_request(REQ(6D45C));
        textdraw_set_font(0x20);
        textdraw_set_alignment(0);
        n = x + 0x77;
        {
        char charBuf[4];

        for (i = 0; i < 6; i++)
        {
            sprintf(charBuf, "%c", lbl_1_bss_D5C10.name[i]);
            PRINT_SHADOWED_II(n + i * 0x22, y, charBuf);
        }
        }
        {
            float offs[3] = { 0.0f, 40.0f, 84.0f };
            int val;
            float px;

            n = x + 0x168;
            textdraw_reset();
            textdraw_set_font(0x21);
            for (i = 0; i < 3; i++)
            {
                val = (&lbl_1_bss_D5E60.unk24)[i];
                CLAMP_TIME(val, i);
                sprintf(numBuf, "%d", val / 10);
                px = (float)n + offs[i];
                PRINT_SHADOWED_FI(px, y, numBuf);
                sprintf(numBuf, "%d", val % 10);
                px = 17.0f + ((float)n + offs[i]);
                PRINT_SHADOWED_FI(px, y, numBuf);
            }
            PRINT_SHADOWED_II(n + 0x22, y, ":");
            PRINT_SHADOWED_II(n + 0x4A, y, ";");
        }
        draw_face((struct NameEntryData *)&lbl_1_bss_D5E60, x + 0x23F, y);
        reset_ui_widescreen_scale_mtx();
    }
    else
    {
        int n;
        int i;

        REQ(6D118)->x = x;
        REQ(6D118)->y = y;
        REQ(6D118)->addColor = 0;
        draw_sprite_draw_request(REQ(6D118));
        set_ui_widescreen_scale_mtx(0x140);
        textdraw_set_font(0x22);
        textdraw_set_alignment(0);
        n = x + 0x24;
        {
        char rankBuf[4];

        sprintf(rankBuf, "%d", lbl_1_bss_D5C10.rank);
        PRINT_SHADOWED_II(n, y, rankBuf);
        }
        textdraw_set_font(0x20);
        textdraw_set_alignment(0);
        n = x + 0x77;
        {
        char charBuf[4];

        for (i = 0; i < 6; i++)
        {
            sprintf(charBuf, "%c", lbl_1_bss_D5C10.name[i]);
            PRINT_SHADOWED_II(n + i * 0x22, y, charBuf);
        }
        }
        n = x + 0x168;
        {
            float offs[3] = { 0.0f, 40.0f, 84.0f };
            int val;
            float px;

            textdraw_reset();
            textdraw_set_font(0x21);
            for (i = 0; i < 3; i++)
            {
                val = (&lbl_1_bss_D5C10.unk10)[i];
                CLAMP_TIME(val, i);
                sprintf(numBuf2, "%d", val / 10);
                px = (float)n + offs[i];
                PRINT_SHADOWED_FI(px, y, numBuf2);
                sprintf(numBuf2, "%d", val % 10);
                px = 17.0f + ((float)n + offs[i]);
                PRINT_SHADOWED_FI(px, y, numBuf2);
            }
            PRINT_SHADOWED_II(n + 0x22, y, ":");
            PRINT_SHADOWED_II(n + 0x4A, y, ";");
        }
        draw_face_sys(x + 0x23F, y + 0xC);
        reset_ui_widescreen_scale_mtx();
        if (lbl_1_bss_D5C10.unk54 & 2)
        {
            int i;
            int n;

            REQ(6D118)->x = x;
            REQ(6D118)->y = y + 0x28;
            REQ(6D118)->addColor = 0;
            draw_sprite_draw_request(REQ(6D118));
            set_ui_widescreen_scale_mtx(0x140);
            REQ(6D45C)->x = x + 0x24;
            REQ(6D45C)->y = y + 0x28;
            draw_sprite_draw_request(REQ(6D45C));
            textdraw_set_font(0x20);
            textdraw_set_alignment(0);
            n = x + 0x77;
            {
            char charBuf2[4];

            for (i = 0; i < 6; i++)
            {
                sprintf(charBuf2, "%c", lbl_1_bss_D5C10.name[i]);
                PRINT_SHADOWED_II(n + i * 0x22, y + 0x28, charBuf2);
            }
            }
            {
                float offs[3] = { 0.0f, 40.0f, 84.0f };
                float fx;
                int val;
                float px;

                fx = x + 0x168;
                textdraw_reset();
                textdraw_set_font(0x21);
                for (i = 0; i < 3; i++)
                {
                    val = (&lbl_1_bss_D5E60.unk24)[i];
                    CLAMP_TIME(val, i);
                    sprintf(numBuf3, "%d", val / 10);
                    px = fx + offs[i];
                    PRINT_SHADOWED_FI(px, y + 0x28, numBuf3);
                    sprintf(numBuf3, "%d", val % 10);
                    px = 17.0f + (fx + offs[i]);
                    PRINT_SHADOWED_FI(px, y + 0x28, numBuf3);
                }
                PRINT_SHADOWED_FI(34.0f + fx, y + 0x28, ":");
                PRINT_SHADOWED_FI(74.0f + fx, y + 0x28, ";");
            }
            draw_face((struct NameEntryData *)&lbl_1_bss_D5E60, x + 0x23F, y + 0x34);
            reset_ui_widescreen_scale_mtx();
        }
    }
}

void fn_1_13CC90(void)
{
    char numBuf[4];
    int n;
    int i;
    int y;
    int x = get_result_panel_offset();
    int divisor;
    int score;
    BOOL printing;
    int px;

    y = 0x36;

    textdraw_reset();
    REQ(6D118)->x = x;
    REQ(6D118)->y = y;
    REQ(6D118)->addColor = 0;
    draw_sprite_draw_request(REQ(6D118));
    set_ui_widescreen_scale_mtx(0x140);
    if (lbl_1_bss_D5885 != 0)
    {
        lbl_1_data_6D308[0]->x = x + 0xF;
        lbl_1_data_6D308[0]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[0]);
    }
    else
    {
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->x = x + 0xF;
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]);
    }
    textdraw_set_font(0x22);
    textdraw_set_alignment(0);
    n = x + 0x24;
    textdraw_set_depth(2.0f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(n + 2, y + 2);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_pos(n, y);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_font(0x20);
    textdraw_set_alignment(0);
    n = x + 0x77;
    for (i = 0; i < 6; i++)
    {
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        px = n + i * 0x22;
        textdraw_set_pos(px + 2, y + 2);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(n + i * 0x22, y);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
    }
    n = x + 0x15E;
    textdraw_set_font(0x21);
    textdraw_set_alignment(0);
    score = lbl_1_bss_D5C10.score;
    printing = FALSE;
    for (divisor = 1000000, i = 0; divisor > 0; divisor /= 10, i++)
    {
        if (printing != FALSE || divisor < 10 || score / divisor >= 0)
        {
            sprintf(numBuf, "%d", score / divisor);
            PRINT_SHADOWED(n + i * 0x11, y + 1, numBuf);
            score %= divisor;
            printing = TRUE;
        }
    }
    REQ(6D36C)->mulColor = 0;
    REQ(6D36C)->z = 2.0f;
    REQ(6D36C)->x = x + 0x1DC;
    REQ(6D36C)->y = y + 0xB;
    draw_sprite_draw_request(REQ(6D36C));
    REQ(6D36C)->mulColor = -1;
    REQ(6D36C)->z = 1.0f;
    REQ(6D36C)->x = x + 0x1DA;
    REQ(6D36C)->y = y + 9;
    draw_sprite_draw_request(REQ(6D36C));
    draw_face_sys(x + 0x23F, y + 0xC);
    reset_ui_widescreen_scale_mtx();
}

void fn_1_13D54C(void)
{
    char numBuf[4];
    int n;
    int i;
    int y;
    int x = get_result_panel_offset();
    int divisor;
    int score;
    BOOL printing;
    int px;

    y = 0x36;

    textdraw_reset();
    REQ(6D118)->x = x;
    REQ(6D118)->y = y;
    REQ(6D118)->addColor = 0;
    draw_sprite_draw_request(REQ(6D118));
    set_ui_widescreen_scale_mtx(0x140);
    if (lbl_1_bss_D5885 != 0)
    {
        lbl_1_data_6D308[0]->x = x + 0xF;
        lbl_1_data_6D308[0]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[0]);
    }
    else
    {
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->x = x + 0xF;
        lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]->y = 50.0f;
        draw_sprite_draw_request(lbl_1_data_6D308[lbl_1_bss_D5D9C + 1]);
    }
    textdraw_set_font(0x22);
    textdraw_set_alignment(0);
    n = x + 0x24;
    textdraw_set_depth(2.0f);
    textdraw_set_mul_color(0);
    textdraw_set_pos(n + 2, y + 2);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_pos(n, y);
    textdraw_printf("%d", lbl_1_bss_D5C10.rank);
    textdraw_set_font(0x20);
    textdraw_set_alignment(0);
    n = x + 0x77;
    for (i = 0; i < 6; i++)
    {
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0);
        px = n + i * 0x22;
        textdraw_set_pos(px + 2, y + 2);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(n + i * 0x22, y);
        textdraw_printf("%c", lbl_1_bss_D5C10.name[i]);
    }
    textdraw_set_font(0x21);
    textdraw_set_alignment(0);
    n = x + 0x15E;
    score = lbl_1_bss_D5C10.score;
    printing = FALSE;
    for (divisor = 1000000, i = 0; divisor > 0; divisor /= 10, i++)
    {
        if (printing != FALSE || divisor < 10 || score / divisor >= 0)
        {
            sprintf(numBuf, "%d", score / divisor);
            PRINT_SHADOWED(n + i * 0x11, y + 1, numBuf);
            score %= divisor;
            printing = TRUE;
        }
    }
    REQ(6D36C)->mulColor = 0;
    REQ(6D36C)->z = 2.0f;
    REQ(6D36C)->x = x + 0x1DC;
    REQ(6D36C)->y = y + 0xB;
    draw_sprite_draw_request(REQ(6D36C));
    REQ(6D36C)->mulColor = -1;
    REQ(6D36C)->z = 1.0f;
    REQ(6D36C)->x = x + 0x1DA;
    REQ(6D36C)->y = y + 9;
    draw_sprite_draw_request(REQ(6D36C));
    draw_face_sys(x + 0x23F, y + 0xC);
    reset_ui_widescreen_scale_mtx();
}

#define RECORD_TIME(r) ((r)->seconds * 100 + (r)->minutes * 6000 + (r)->centis)

int fn_1_13DE08(int a, struct NETimeRecord *rec, int c, int d)
{
    struct NETimeRecord *tbl;
    int ret = 0;
    int i;
    int j;
    int time = RECORD_TIME(rec);
    int t;

    tbl = lbl_1_data_6C2B4[a].ptr;

    switch (a)
    {
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        g_some_gift_menu_flags |= 2;
        break;
    }
    for (i = 0; i < 5; i++)
    {
        t = RECORD_TIME(&tbl[i]);
        if (time < t)
        {
            if (d != 0)
                return i;
            for (j = 4; j > i; j--)
                tbl[j] = tbl[j - 1];
            memset(rec, 0, 6);
            tbl[i] = *rec;
            lbl_1_bss_D5D98[c] = i + 1;
            lbl_1_bss_D5C10.unk52++;
            for (j = c - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D5D98[j] - 1 >= i)
                {
                    if (lbl_1_bss_D5D98[j] - 1 < 4)
                        lbl_1_bss_D5D98[j]++;
                    else
                        lbl_1_bss_D5D98[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (d != 0)
        return -1;
    for (i = 0; i < 5; i++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, tbl[i].name);
        lbl_1_bss_D5C70[i].unk14 = tbl[i].unk7;
        lbl_1_bss_D5C70[i].unk24 = tbl[i].minutes;
        lbl_1_bss_D5C70[i].unk25 = tbl[i].seconds;
        lbl_1_bss_D5C70[i].unk26 = tbl[i].centis;
        lbl_1_bss_D5C70[i].unk20 = lbl_1_bss_D5D98[c] - 1;
        lbl_1_bss_D5C70[i].unk2C = tbl[i].unkB;
    }
    strcpy(lbl_1_bss_D5DA0[c].name, "      ");
    lbl_1_bss_D5DA0[c].unk24 = rec->minutes;
    lbl_1_bss_D5DA0[c].unk25 = rec->seconds;
    lbl_1_bss_D5DA0[c].unk26 = rec->centis;
    lbl_1_bss_D5DA0[c].apeType = rec->unk7;
    lbl_1_bss_D5DA0[c].unk20 = lbl_1_bss_D5D98[c];
    lbl_1_bss_D5DA0[c].unk2C = rec->unkB;
    return ret;
}

int fn_1_13E17C(int a, struct NETimeRecord *rec, int c, int d)
{
    struct NETimeRecord *tbl;
    int ret = 0;
    int best = 0;
    int time = RECORD_TIME(rec);
    int t;
    int i;

    for (i = 1; i < c; i++)
    {
        t = RECORD_TIME(&rec[i]);
        if (time > t)
            best = i;
    }
    tbl = lbl_1_data_6CCE4[a];
    time = RECORD_TIME(&rec[best]);
    t = RECORD_TIME(tbl);
    if (time < t)
    {
        if (d != 0)
            return 5;
        memset(rec, 0, 6);
        *tbl = *rec;
        if (lbl_1_bss_D5D98[best] >= 0)
        {
            lbl_1_bss_D5C10.unk54 |= 2;
            printf(" both entry \n");
        }
        else
        {
            lbl_1_bss_D5C10.unk54 |= 4;
            printf(" lap entry only \n");
            lbl_1_bss_D5D98[best] = 5;
        }
        ret = 1;
    }
    if (d != 0)
        return -1;
    strcpy(lbl_1_bss_D5E60.name, tbl[best].name);
    lbl_1_bss_D5E60.unk24 = tbl[best].minutes;
    lbl_1_bss_D5E60.unk25 = tbl[best].seconds;
    lbl_1_bss_D5E60.unk26 = tbl[best].centis;
    lbl_1_bss_D5E60.apeType = tbl[best].unk7;
    lbl_1_bss_D5E60.unk2C = tbl[best].unkB;
    return ret;
}

int fn_1_13E3BC(int a, struct NEScoreRecord *rec, int c, int d)
{
    struct NEScoreRecord *tbl;
    int ret = 0;
    int i;
    int j;

    tbl = lbl_1_data_6C2B4[a].ptr;

    for (i = 0; i < 5; i++)
    {
        if (rec->score > tbl[i].score)
        {
            if (d != 0)
                return i;
            for (j = 4; j > i; j--)
                tbl[j] = tbl[j - 1];
            memset(rec, 0, 6);
            tbl[i] = *rec;
            lbl_1_bss_D5D98[c] = i + 1;
            lbl_1_bss_D5C10.unk52++;
            for (j = c - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D5D98[j] - 1 >= i)
                {
                    if (lbl_1_bss_D5D98[j] - 1 < 4)
                        lbl_1_bss_D5D98[j]++;
                    else
                        lbl_1_bss_D5D98[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (d != 0)
        return -1;
    for (i = 0; i < 5; i++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, tbl[i].name);
        lbl_1_bss_D5C70[i].unk14 = tbl[i].unk7;
        lbl_1_bss_D5C70[i].score = tbl[i].score;
        lbl_1_bss_D5C70[i].unk20 = lbl_1_bss_D5D98[c] - 1;
    }
    strcpy(lbl_1_bss_D5DA0[c].name, "      ");
    lbl_1_bss_D5DA0[c].score = rec->score;
    lbl_1_bss_D5DA0[c].apeType = rec->unk7;
    lbl_1_bss_D5DA0[c].unk20 = lbl_1_bss_D5D98[c];
    return ret;
}

int fn_1_13E64C(int a, struct NEShortRecord *rec, int c, int d)
{
    struct NEShortRecord *tbl;
    int ret = 0;
    int i;
    int j;
    int k;

    tbl = lbl_1_data_6C2B4[a].ptr;

    for (i = 0; i < 5; i++)
    {
        if ((u16)rec->score > tbl[i].score)
        {
            if (d != 0)
                return i;
            for (j = 4; j > i; j--)
                tbl[j] = tbl[j - 1];
            memset(rec, 0, 6);
            tbl[i] = *rec;
            lbl_1_bss_D5D98[c] = i + 1;
            lbl_1_bss_D5C10.unk52++;
            for (j = c - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D5D98[j] - 1 >= i)
                {
                    if (lbl_1_bss_D5D98[j] - 1 < 4)
                        lbl_1_bss_D5D98[j]++;
                    else
                        lbl_1_bss_D5D98[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (d != 0)
        return -1;
    for (i = 0; i < 5; i++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, tbl[i].name);
        lbl_1_bss_D5C70[i].unk14 = tbl[i].unk7;
        lbl_1_bss_D5C70[i].score = tbl[i].score;
        lbl_1_bss_D5C70[i].unk20 = lbl_1_bss_D5D98[c] - 1;
    }
    strcpy(lbl_1_bss_D5DA0[c].name, "      ");
    lbl_1_bss_D5DA0[c].score = rec->score;
    lbl_1_bss_D5DA0[c].apeType = rec->unk7;
    lbl_1_bss_D5DA0[c].unk20 = lbl_1_bss_D5D98[c];
    for (k = lbl_1_bss_DD070 - 1; k >= 0; k--)
    {
        if (lbl_1_bss_D5D98[k] > 0)
            lbl_1_bss_D5D9C = k;
    }
    return ret;
}

int fn_1_13E908(int a, struct NEPairRecord *rec, int c, int d)
{
    struct NEPairRecord *tbl;
    int ret = 0;
    int i;
    int j;

    tbl = lbl_1_data_6C2B4[a].ptr;
    lbl_1_bss_D5C10.unk50 = a;

    for (i = 0; i < 5; i++)
    {
        if (rec->a + rec->b < tbl[i].a + tbl[i].b)
        {
            if (d != 0)
                return i;
            for (j = 4; j > i; j--)
                tbl[j] = tbl[j - 1];
            memset(rec, 0, 6);
            tbl[i] = *rec;
            lbl_1_bss_D5D98[c] = i + 1;
            lbl_1_bss_D5C10.unk52++;
            for (j = c - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D5D98[j] - 1 >= i)
                {
                    if (lbl_1_bss_D5D98[j] - 1 < 4)
                        lbl_1_bss_D5D98[j]++;
                    else
                        lbl_1_bss_D5D98[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (d != 0)
        return -1;
    for (i = 0; i < 5; i++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, tbl[i].name);
        lbl_1_bss_D5C70[i].unk14 = tbl[i].unk7;
        lbl_1_bss_D5C70[i].unk28 = tbl[i].a;
        lbl_1_bss_D5C70[i].unk2A = tbl[i].b;
        lbl_1_bss_D5C70[i].unk20 = lbl_1_bss_D5D98[c] - 1;
    }
    for (d = 0; d < 5; d++)
        printf("rank_tbl[%d]:%d\n", d, lbl_1_bss_D5D98[d]);
    strcpy(lbl_1_bss_D5DA0[c].name, "      ");
    lbl_1_bss_D5DA0[c].unk2A = rec->b;
    lbl_1_bss_D5DA0[c].unk28 = rec->a;
    lbl_1_bss_D5DA0[c].apeType = rec->unk7;
    lbl_1_bss_D5DA0[c].unk20 = lbl_1_bss_D5D98[c];
    return ret;
}

int fn_1_13EC0C(int a, struct NETimeRecord2 *rec, int c, int d)
{
    struct NETimeRecord2 *tbl;
    int ret = 0;
    int i;
    int j;
    int time = RECORD_TIME(rec);
    int t;

    tbl = lbl_1_data_6C2B4[a].ptr;

    for (i = 0; i < 5; i++)
    {
        t = RECORD_TIME(&tbl[i]);
        if (time < t)
        {
            if (d != 0)
                return i;
            for (j = 4; j > i; j--)
                tbl[j] = tbl[j - 1];
            memset(rec, 0, 6);
            tbl[i] = *rec;
            lbl_1_bss_D5D98[c] = i + 1;
            lbl_1_bss_D5C10.unk52++;
            for (j = c - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D5D98[j] - 1 >= i)
                {
                    if (lbl_1_bss_D5D98[j] - 1 < 4)
                        lbl_1_bss_D5D98[j]++;
                    else
                        lbl_1_bss_D5D98[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (d != 0)
        return -1;
    for (i = 0; i < 5; i++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, tbl[i].name);
        lbl_1_bss_D5C70[i].unk14 = tbl[i].unk7;
        lbl_1_bss_D5C70[i].unk24 = tbl[i].minutes;
        lbl_1_bss_D5C70[i].unk25 = tbl[i].seconds;
        lbl_1_bss_D5C70[i].unk26 = tbl[i].centis;
        lbl_1_bss_D5C70[i].unk20 = lbl_1_bss_D5D98[c] - 1;
    }
    strcpy(lbl_1_bss_D5DA0[c].name, "      ");
    lbl_1_bss_D5DA0[c].unk24 = rec->minutes;
    lbl_1_bss_D5DA0[c].unk25 = rec->seconds;
    lbl_1_bss_D5DA0[c].unk26 = rec->centis;
    lbl_1_bss_D5DA0[c].apeType = rec->unk7;
    lbl_1_bss_D5DA0[c].unk20 = lbl_1_bss_D5D98[c];
    return ret;
}

int fn_1_13EF3C(int a, struct NETimeRecord2 *rec, int c, int d)
{
    struct NETimeRecord2 *tbl;
    int ret = 0;
    int best = 0;
    int time = RECORD_TIME(rec);
    int t;
    int i;

    for (i = 1; i < c; i++)
    {
        t = RECORD_TIME(&rec[i]);
        if (time > t)
            best = i;
    }
    tbl = lbl_1_data_6CCFC[a - 24];
    time = RECORD_TIME(&rec[best]);
    t = RECORD_TIME(tbl);
    if (time < t)
    {
        if (d != 0)
            return 5;
        memset(rec, 0, 6);
        *tbl = *rec;
        if (lbl_1_bss_D5D98[best] >= 0)
        {
            lbl_1_bss_D5C10.unk54 |= 2;
            printf(" both entry\n");
        }
        else
        {
            lbl_1_bss_D5C10.unk54 |= 4;
            printf(" lap entry only \n");
            lbl_1_bss_D5D98[best] = 5;
        }
        ret = 1;
    }
    if (d != 0)
        return -1;
    strcpy(lbl_1_bss_D5E60.name, tbl[best].name);
    lbl_1_bss_D5E60.unk24 = tbl[best].minutes;
    lbl_1_bss_D5E60.unk25 = tbl[best].seconds;
    lbl_1_bss_D5E60.unk26 = tbl[best].centis;
    lbl_1_bss_D5E60.apeType = tbl[best].unk7;
    return ret;
}

int fn_1_13F174(int a, struct NEScoreRecord2 *rec, int c, int d)
{
    struct NEScoreRecord2 *tbl;
    int ret = 0;
    int i;
    int j;

    tbl = lbl_1_data_6C2B4[a].ptr;

    for (i = 0; i < 5; i++)
    {
        if (rec->score > tbl[i].score)
        {
            if (d != 0)
                return i;
            for (j = 4; j > i; j--)
                tbl[j] = tbl[j - 1];
            memset(rec, 0, 6);
            tbl[i] = *rec;
            lbl_1_bss_D5D98[c] = i + 1;
            lbl_1_bss_D5C10.unk52++;
            for (j = c - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D5D98[j] - 1 >= i)
                {
                    if (lbl_1_bss_D5D98[j] - 1 < 4)
                        lbl_1_bss_D5D98[j]++;
                    else
                        lbl_1_bss_D5D98[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (d != 0)
        return -1;
    for (i = 0; i < 5; i++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, tbl[i].name);
        lbl_1_bss_D5C70[i].unk14 = tbl[i].unk7;
        lbl_1_bss_D5C70[i].score = tbl[i].score;
        lbl_1_bss_D5C70[i].unk20 = lbl_1_bss_D5D98[c] - 1;
    }
    strcpy(lbl_1_bss_D5DA0[c].name, "      ");
    lbl_1_bss_D5DA0[c].score = rec->score;
    lbl_1_bss_D5DA0[c].apeType = rec->unk7;
    lbl_1_bss_D5DA0[c].unk20 = lbl_1_bss_D5D98[c];
    return ret;
}

int fn_1_13F3F4(int a, struct NEScoreRecord *rec, int c, int d)
{
    struct NEScoreRecord *tbl;
    int ret = 0;
    int i;
    int j;

    tbl = lbl_1_data_6C2B4[a].ptr;

    for (i = 0; i < 5; i++)
    {
        if (rec->score > tbl[i].score)
        {
            if (d != 0)
                return i;
            for (j = 4; j > i; j--)
                tbl[j] = tbl[j - 1];
            memset(rec, 0, 6);
            tbl[i] = *rec;
            lbl_1_bss_D5D98[c] = i + 1;
            lbl_1_bss_D5C10.unk52++;
            for (j = c - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D5D98[j] - 1 >= i)
                {
                    if (lbl_1_bss_D5D98[j] - 1 < 4)
                        lbl_1_bss_D5D98[j]++;
                    else
                        lbl_1_bss_D5D98[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (d != 0)
        return -1;
    for (i = 0; i < 5; i++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, tbl[i].name);
        lbl_1_bss_D5C70[i].unk14 = tbl[i].unk7;
        lbl_1_bss_D5C70[i].score = tbl[i].score;
        lbl_1_bss_D5C70[i].unk20 = lbl_1_bss_D5D98[c] - 1;
    }
    strcpy(lbl_1_bss_D5DA0[c].name, "      ");
    lbl_1_bss_D5DA0[c].score = rec->score;
    lbl_1_bss_D5DA0[c].apeType = rec->unk7;
    lbl_1_bss_D5DA0[c].unk20 = lbl_1_bss_D5D98[c];
    printf("score:%7d\n", rec->score);
    return ret;
}

void fn_1_13F694(void) {}

void fn_1_13F698(void)
{
    fn_1_1288FC();
}

void fn_1_13F6B8(void)
{
    struct Sprite *sprite;
    int i;

    destroy_sprite_with_unique_id(0x65);
    for (i = 0; i < 5; i++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, lbl_1_data_6BA68[i].name);
        lbl_1_bss_D5C70[i].score = lbl_1_data_6BA68[i].unk8;
        lbl_1_bss_D5C70[i].unkC = lbl_1_data_6BA68[i].unk10;
        lbl_1_bss_D5C70[i].unk18 = lbl_1_data_6BA68[i].unkC;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->depth = 1.0f;
        sprite->mainFunc = (void *)fn_1_13F694;
        sprite->drawFunc = (void *)fn_1_13F698;
        strcpy(sprite->text, "RANK");
    }
}

void sprite_rank_tick(void)
{
    if (!((g_font_char_var_3 % 10) / 10.0))
    {
        lbl_1_bss_D587C++;
        if (lbl_1_bss_D587C == 6)
            lbl_1_bss_D587C = 0;
        lbl_1_data_6BAE0[0]++;
        if (lbl_1_data_6BAE0[0] == 6)
            lbl_1_data_6BAE0[0] = 0;
    }
}

void sprite_rank_disp(void)
{
    static u16 lbl_1_data_6EAE8[] = { 0x32B, 0x32A, 0x32C, 0x32D };

    {
        struct SpriteDrawRequest a = { 0x32A, 0.0f, 20.0f, 5.0f, 80.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, -1 };

        a.bmpId = lbl_1_data_6EAE8[lbl_1_bss_DD114];
        draw_sprite_draw_request(&a);
        a.y = 440.0f;
        draw_sprite_draw_request(&a);
    }
    set_ui_widescreen_scale_mtx(0x140);
    {
        struct SpriteDrawRequest b = { 0x329, 320.0f, 16.0f, 0.3f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 7, -1 };

        b.mulColor = 0;
        b.opacity = 0.5f;
        b.z = 4.5f;
        b.scaleX = 0.9f;
        b.y = 22.0f;
        draw_sprite_draw_request(&b);
        b.mulColor = -1;
        b.opacity = 1.0f;
        b.z = 4.0f;
        b.scaleX = 1.0f;
        b.y = 16.0f;
        draw_sprite_draw_request(&b);
        b.flags = 0x80000;
        b.mulColor = 0;
        b.opacity = 0.5f;
        b.z = 4.5f;
        b.scaleX = 0.9f;
        b.y = 22.0f;
        draw_sprite_draw_request(&b);
        b.mulColor = -1;
        b.opacity = 1.0f;
        b.z = 4.0f;
        b.scaleX = 1.0f;
        b.y = 16.0f;
        draw_sprite_draw_request(&b);
    }
    reset_ui_widescreen_scale_mtx();
    set_ui_widescreen_scale_mtx(0x140);
    {
        struct SpriteDrawRequest c = { 0x32F, 146.0f, 28.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, -1 };
        struct SpriteDrawRequest d[4] = {
            { 0x300, 390.0f, 28.0f, 0.1f, 1.0f, 0.25f, 0.0f, 0.0f, 1.0f, 0.25f, 0, 1.0f, -1, 6, -1 },
            { 0x300, 390.0f, 28.0f, 0.1f, 1.0f, 0.25f, 0.0f, 0.25f, 1.0f, 0.5f, 0, 1.0f, -1, 6, -1 },
            { 0x300, 390.0f, 28.0f, 0.1f, 1.0f, 0.25f, 0.0f, 0.5f, 1.0f, 0.75f, 0, 1.0f, -1, 6, -1 },
            { 0x300, 390.0f, 28.0f, 0.1f, 1.0f, 0.25f, 0.0f, 0.75f, 1.0f, 1.0f, 0, 1.0f, -1, 6, -1 },
        };

        draw_sprite_draw_request(&c);
        draw_sprite_draw_request(&d[lbl_1_bss_DD114]);
        c.mulColor = 0;
        c.x = 149.0f;
        c.y = 31.0f;
        c.z = 1.5f;
        c.opacity = 0.3f;
        d[lbl_1_bss_DD114].mulColor = 0;
        d[lbl_1_bss_DD114].x = 393.0f;
        d[lbl_1_bss_DD114].y = 31.0f;
        d[lbl_1_bss_DD114].z = 1.5f;
        d[lbl_1_bss_DD114].opacity = 0.3f;
        draw_sprite_draw_request(&c);
        draw_sprite_draw_request(&d[lbl_1_bss_DD114]);
    }
    reset_ui_widescreen_scale_mtx();
    fn_1_129BDC();
}

void g_something_with_name_entry_get_course(int a)
{
    struct Sprite *sprite;
    int i;
    struct NameEntryRec *rec;
    struct NameEntryRec (*base)[5];

    destroy_sprite_with_unique_id(0x65);
    if (g_some_other_flags & 4)
        printf("NameEntry2_GetCourse\n");
    base = lbl_1_bss_D588C;
    rec = base[a];
    for (i = 0; i < 5; i++, rec++)
    {
        strcpy(lbl_1_bss_D5C70[i].name, rec->name);
        lbl_1_bss_D5C70[i].score = rec->score;
        lbl_1_bss_D5C70[i].unk10 = rec->unk10;
        lbl_1_bss_D5C70[i].unk11 = rec->unk11;
        lbl_1_bss_D5C70[i].unk14 = rec->unkC;
    }
    lbl_1_bss_DD114 = a;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->depth = 1.0f;
        sprite->mainFunc = (void *)sprite_rank_tick;
        sprite->drawFunc = (void *)sprite_rank_disp;
        strcpy(sprite->text, "RANK");
    }
}

#ifndef __MWERKS__
/* PC port: the save file holds the ranking tables lbl_1_data_6C440..6CCE4 as
 * one 0x8A4-byte block, copied with a single memcpy on GC because the tables
 * are contiguous there. On PC they are separate objects in an order and with
 * padding chosen by the compiler (GCC places them in reverse), so a single
 * copy would overrun into other data. Copy each table at its GC offset. */
/* rec: record size; every record starts with name[7] and the ape number */
static const struct { void *ptr; u32 size; u32 rec; } s_pcRankBlock[] = {
    { &lbl_1_data_6C440, sizeof(lbl_1_data_6C440), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C47C, sizeof(lbl_1_data_6C47C), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C488, sizeof(lbl_1_data_6C488), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C4C4, sizeof(lbl_1_data_6C4C4), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C4D0, sizeof(lbl_1_data_6C4D0), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C50C, sizeof(lbl_1_data_6C50C), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C518, sizeof(lbl_1_data_6C518), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C554, sizeof(lbl_1_data_6C554), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C560, sizeof(lbl_1_data_6C560), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C59C, sizeof(lbl_1_data_6C59C), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C5A8, sizeof(lbl_1_data_6C5A8), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C5E4, sizeof(lbl_1_data_6C5E4), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C5F0, sizeof(lbl_1_data_6C5F0), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C638, sizeof(lbl_1_data_6C638), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C680, sizeof(lbl_1_data_6C680), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C6C8, sizeof(lbl_1_data_6C6C8), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C710, sizeof(lbl_1_data_6C710), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C758, sizeof(lbl_1_data_6C758), sizeof(struct NETimeRecord) },
    { &lbl_1_data_6C7A0, sizeof(lbl_1_data_6C7A0), sizeof(struct NERankB) },
    { &lbl_1_data_6C7F0, sizeof(lbl_1_data_6C7F0), sizeof(struct NERankB) },
    { &lbl_1_data_6C840, sizeof(lbl_1_data_6C840), sizeof(struct NERankB) },
    { &lbl_1_data_6C890, sizeof(lbl_1_data_6C890), sizeof(struct NERankB) },
    { &lbl_1_data_6C8E0.a, sizeof(lbl_1_data_6C8E0.a), sizeof(struct NEA) },
    { &lbl_1_data_6C8E0.b, sizeof(lbl_1_data_6C8E0.b), sizeof(struct NEB) },
    { &lbl_1_data_6C8E0.c, sizeof(lbl_1_data_6C8E0.c), sizeof(struct NEC) },
    { &lbl_1_data_6CB40, sizeof(lbl_1_data_6CB40), sizeof(struct NERankF) },
    { &lbl_1_data_6CB7C, sizeof(lbl_1_data_6CB7C), sizeof(struct NERankF) },
    { &lbl_1_data_6CBB8, sizeof(lbl_1_data_6CBB8), sizeof(struct NERankF) },
    { &lbl_1_data_6CBF4, sizeof(lbl_1_data_6CBF4), sizeof(struct NERankB) },
    { &lbl_1_data_6CC44, sizeof(lbl_1_data_6CC44), sizeof(struct NERankB) },
    { &lbl_1_data_6CC94, sizeof(lbl_1_data_6CC94), sizeof(struct NERankB) }
};

int pc_save_rank_fix(u8 *story); /* pc_gci.c */
void pc_save_rank_mark(u8 *story);
int pc_save_rank_rec_ok(const u8 *r);

static void pc_rank_block_copy(u8 *save, int toSave)
{
    u32 off = 0;
    u32 i;

    for (i = 0; i < sizeof(s_pcRankBlock) / sizeof(s_pcRankBlock[0]); i++)
    {
        if (toSave)
            memcpy_handler(save + off, s_pcRankBlock[i].ptr, s_pcRankBlock[i].size);
        else
        {
            /* port builds before the per-table copy saved whatever followed
             * the first table in memory here: keep the current (at boot: the
             * default) table when any of its records is not a name record */
            u32 r;

            for (r = 0; r < s_pcRankBlock[i].size; r += s_pcRankBlock[i].rec)
                if (!pc_save_rank_rec_ok(save + off + r))
                    break;
            if (r >= s_pcRankBlock[i].size)
                memcpy_handler(s_pcRankBlock[i].ptr, save + off, s_pcRankBlock[i].size);
            else
                printf("[rank-migrate] ranking table at save offset 0x%X is not name records, kept the defaults\n",
                       (unsigned)(0x5F4 + off));
        }
        off += s_pcRankBlock[i].size;
    }
}

_Static_assert(sizeof(lbl_1_data_6C440) + sizeof(lbl_1_data_6C47C) + sizeof(lbl_1_data_6C488) + sizeof(lbl_1_data_6C4C4)
             + sizeof(lbl_1_data_6C4D0) + sizeof(lbl_1_data_6C50C) + sizeof(lbl_1_data_6C518) + sizeof(lbl_1_data_6C554)
             + sizeof(lbl_1_data_6C560) + sizeof(lbl_1_data_6C59C) + sizeof(lbl_1_data_6C5A8) + sizeof(lbl_1_data_6C5E4)
             + sizeof(lbl_1_data_6C5F0) + sizeof(lbl_1_data_6C638) + sizeof(lbl_1_data_6C680) + sizeof(lbl_1_data_6C6C8)
             + sizeof(lbl_1_data_6C710) + sizeof(lbl_1_data_6C758) + sizeof(lbl_1_data_6C7A0) + sizeof(lbl_1_data_6C7F0)
             + sizeof(lbl_1_data_6C840) + sizeof(lbl_1_data_6C890) + sizeof(lbl_1_data_6C8E0) + sizeof(lbl_1_data_6CB40)
             + sizeof(lbl_1_data_6CB7C) + sizeof(lbl_1_data_6CBB8) + sizeof(lbl_1_data_6CBF4) + sizeof(lbl_1_data_6CC44)
             + sizeof(lbl_1_data_6CC94) == 0x8A4, "ranking block layout");
_Static_assert(sizeof(lbl_1_bss_D5ABC) == 0x64 && sizeof(lbl_1_bss_D588C) == 0x230, "ranking record layout");
#endif

void fn_1_13FCFC(void)
{
    memcpy_handler(storymode_save_files + 0x35C, lbl_1_bss_D5ABC, 0x64);
    storymode_save_files[0x3C0] = g_some_gift_menu_flags;
    memcpy_handler(storymode_save_files + 0x3C4, lbl_1_bss_D588C, 0x230);
#ifdef __MWERKS__
    memcpy_handler(storymode_save_files + 0x5F4, lbl_1_data_6C440, 0x8A4);
#else
    pc_rank_block_copy(storymode_save_files + 0x5F4, 1);
    pc_save_rank_mark(storymode_save_files); /* tells this layout from the pre-1.1.3 one */
#endif
}

void fn_1_13FD84(void)
{
    struct NameEntryData *data;
    struct NameEntryRec *rec;
    struct NameEntryRec *row;
    struct NameEntryDefault *def;
    int j;
    int i;

    memcpy_handler(lbl_1_bss_D5ABC, storymode_save_files + 0x35C, 0x64);
    g_some_gift_menu_flags = storymode_save_files[0x3C0];
    memcpy_handler(lbl_1_bss_D588C, storymode_save_files + 0x3C4, 0x230);
#ifdef __MWERKS__
    memcpy_handler(lbl_1_data_6C440, storymode_save_files + 0x5F4, 0x8A4);
#else
    /* saves of port builds before 1.1.3 hold these tables byte-reversed */
    pc_save_rank_fix(storymode_save_files);
    pc_rank_block_copy(storymode_save_files + 0x5F4, 0);
#endif
    def = lbl_1_bss_D5ABC;
    for (i = 0; i < 5; i++, def++)
    {
        strcpy(lbl_1_data_6BA68[i].name, def->name);
        lbl_1_data_6BA68[i].unk8 = def->unk8;
        lbl_1_data_6BA68[i].unkC = def->unkC;
        lbl_1_data_6BA68[i].unk10 = def->unk10;
    }
    row = lbl_1_bss_D588C[0];
    for (i = 0; i < 4; i++, row += 5)
    {
        data = lbl_1_data_6BA58[i];
        rec = row;
        for (j = 0; j < 5; j++, rec++)
        {
            strcpy(data[j].name, rec->name);
            data[j].score = rec->score;
            data[j].unk14 = rec->unkC;
            data[j].unk10 = rec->unk10;
            data[j].unk11 = rec->unk11;
            data[j].unkC = 0x57E3F;
            data[j].unk12 = -1;
        }
    }
}
