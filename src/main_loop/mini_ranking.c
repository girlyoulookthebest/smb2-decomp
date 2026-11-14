#pragma peephole off
#include "types.h"
#include "bitmap.h"
#include "dip.h"
#include "input.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"
#include "sprite.h"

struct MiniRankingState
{
    /*0x00*/ u32 flags;
    /*0x04*/ s8 mode;
    /*0x05*/ s8 unk5;
    /*0x06*/ s8 unk6;
    /*0x07*/ s8 unk7;
    /*0x08*/ s8 unk8;
    /*0x09*/ s8 unk9;
    /*0x0A*/ s8 unkA;
    /*0x0B*/ s8 unkB[4];
    /*0x0F*/ s8 unkF;
    /*0x10*/ s8 unk10;
    /*0x11*/ s8 unk11[3];
    /*0x14*/ u8 unk14;
    u8 pad15[0x18 - 0x15];
    /*0x18*/ u32 unk18;
    /*0x1C*/ float unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ u8 tex[0xA8 - 0x2C];
};

struct RecTime
{
    /*0x00*/ char name[3];
    /*0x03*/ u8 chara;
    /*0x04*/ u8 min;
    /*0x05*/ u8 sec;
    /*0x06*/ u8 cs;
    /*0x07*/ u8 unk7;
};

struct RecScore
{
    /*0x00*/ char name[3];
    /*0x03*/ u8 chara;
    /*0x04*/ u16 score;
    /*0x06*/ u8 unk6;
    u8 pad7;
};

struct RecBowl
{
    /*0x00*/ char name[3];
    /*0x03*/ u8 chara;
    /*0x04*/ u16 score;
};

struct RecGolf
{
    /*0x00*/ char name[3];
    /*0x03*/ u8 chara;
    /*0x04*/ u16 a;
    /*0x06*/ u16 b;
};

struct RankingMode
{
    void (*init)(int, int);
    void *entries;
    int entrySize;
    char *bgPath;
};

extern s32 sub_mode;
extern s32 sub_mode_request;
extern u32 g_font_char_var_3;

void event_init(int id);
void g_fade_track_volume(int a, int b);
void set_sub_mode_destination(void (*func)(void));
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(const char *fmt, ...);
void load_bmp_by_id(int id);
void call_free_bmp_by_id(int id);
void destroy_sprite_with_unique_id(int id);
int strcmp(const char *a, const char *b);
#ifdef __MWERKS__
unsigned long strlen(const char *s);
int strncmp(const char *a, const char *b, unsigned long n);
char *strncpy(char *a, const char *b, unsigned long n);
#endif

void call_SoundReqID_arg_0(int id);
extern s32 player_pad_map[4];

void fn_1_F9B04(void *tex, char *path, int a, int b, int c, int d);
void fn_1_F9CBC(void *tex);
void fn_1_F9E1C(void *tex);
void fn_1_F8060(int mode, int flag);
void fn_1_F9EAC(void *tex, float a, float b, float c, float d, float e, int f, int g);
void GXLoadPosMtxImm(Mtx m, int id);
void textdraw_reset(void);
void textdraw_set_font(int font);
void textdraw_set_depth(float depth);
void textdraw_set_scale(float x, float y);
void textdraw_set_pos(float x, float y);
void textdraw_set_mul_color(u32 color);
void textdraw_set_add_color(u32 color);
void textdraw_print(char *str);
void textdraw_printf(char *fmt, ...);
void fn_1_F35F4(struct Sprite *sprite);
void fn_1_F1AD0(void);
void fn_1_F828C(void);
void fn_1_F8588(struct RecTime *rec);
void fn_1_F86B0(struct RecScore *rec);
void fn_1_F8768(struct RecBowl *rec);
void fn_1_F8810(struct RecBowl *rec);
void fn_1_F88B8(struct RecGolf *rec);
void fn_1_F89A0(struct RecTime *rec);
int fn_1_F9744(int mode, void *rec, int count);

static struct MiniRankingState lbl_1_bss_D2B88;

static u8 lbl_1_data_61C00[0x170] = {
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
static u8 lbl_1_data_61D70[45] = "\\]ABCDEFGHIJKLMNOPQRSTUVWXYZ.!?&-@0123456789[";
static const u16 lbl_1_rodata_64D90[4] = { 0x000F, 0x0048, 0x0048, 0x0048 };
static const u16 lbl_1_rodata_64D98[9] = { 0x0020, 0x0020, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031 };
static const u16 lbl_1_rodata_64DAC[8] = { 0x0033, 0x0033, 0x0041, 0x0041, 0x0041, 0x0041, 0x0041, 0x0041 };
static const u16 lbl_1_rodata_64DBC[5] = { 0x0003, 0x005D, 0x005D, 0x005D, 0x005D };

int fn_1_dead(void)
{
    return lbl_1_data_61C00[0] + lbl_1_data_61D70[0];
}

#define PAD_REPEAT(btn) \
    ((pad_status_groups[0].unk0[4].button & (btn)) \
     || (analog_inputs[0][4] & (btn)) \
     || (((pad_status_groups[0].unk0[0].button & (btn)) || (analog_inputs[0][0] & (btn))) \
      && (analog_inputs[0][0] & 0x200)))

void smd_mini_ranking_init(void)
{
    int i;

    event_init(0x11);
    event_init(0x13);
    modeCtrl.unk10 = 0;
    fn_1_F8060(modeCtrl.unk10, 0);
    for (i = 0; i < 4; i++)
        lbl_1_bss_D2B88.unkB[i] = -1;
    g_fade_track_volume(0x30, 0);
    set_sub_mode_destination(fn_1_F1AD0);
    sub_mode_request = 0xB6;
}

void smd_mini_ranking_tick(void)
{
    struct RecTime race[8];
    struct RecScore target;
    struct RecBowl bowl1;
    struct RecBowl bowl2;
    struct RecGolf golf;
    struct RecTime boat[8];
    int i;

    debug_textdraw_set_pos(1, 1);
    debug_textdraw_vsprintf_buf_1("RANKING\n");
    debug_textdraw_vsprintf_buf_1(" [%d/%d]\n", modeCtrl.unk10, 15);
    if (!(g_some_other_flags & 0xA))
    {
        if (!(lbl_1_bss_D2B88.flags & 1))
        {
            i = modeCtrl.unk10;
            if (PAD_REPEAT(PAD_BUTTON_LEFT))
            {
                i--;
                if (i < 0)
                    i = 14;
            }
            if (PAD_REPEAT(PAD_BUTTON_RIGHT))
            {
                i++;
                if (i >= 15)
                    i = 0;
            }
            if (i != modeCtrl.unk10)
            {
                fn_1_F8060(i, 0);
                modeCtrl.unk10 = i;
            }
        }
        if (!(lbl_1_bss_D2B88.flags & 1))
        {
            int j = lbl_1_bss_D2B88.unkB[0];

            if (PAD_REPEAT(PAD_BUTTON_UP))
            {
                j--;
                if (j < 0)
                    j = 4;
            }
            if (PAD_REPEAT(PAD_BUTTON_DOWN))
            {
                j++;
                if (j >= 5)
                    j = 0;
            }
            lbl_1_bss_D2B88.unkB[0] = j;
        }
        if ((pad_status_groups[0].unk0[2].button & PAD_BUTTON_A)
         && !(lbl_1_bss_D2B88.flags & 1) && lbl_1_bss_D2B88.unkB[0] >= 0)
            fn_1_F8060(modeCtrl.unk10, 1);
        if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_Y)
        {
            switch (modeCtrl.unk10)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                fn_1_F8588(&race[0]);
                fn_1_F8588(&race[1]);
                fn_1_F8588(&race[2]);
                fn_1_F8588(&race[3]);
                fn_1_F8588(&race[4]);
                fn_1_F8588(&race[5]);
                fn_1_F8588(&race[6]);
                fn_1_F8588(&race[7]);
                if (fn_1_F9744(modeCtrl.unk10, race, 4) != 0)
                {
                    fn_1_F8060(modeCtrl.unk10, 1);
                    printf("entry: TRUE\n");
                }
                else
                {
                    fn_1_F8060(modeCtrl.unk10, 0);
                    printf("entry: FALSE\n");
                }
                break;
            case 6:
            case 7:
            case 8:
                fn_1_F86B0(&target);
                if (fn_1_F9744(modeCtrl.unk10, &target, 1) != 0)
                {
                    fn_1_F8060(modeCtrl.unk10, 1);
                    printf("entry: TRUE\n");
                }
                else
                {
                    fn_1_F8060(modeCtrl.unk10, 0);
                    printf("entry: FALSE\n");
                }
                break;
            case 9:
                fn_1_F8768(&bowl1);
                if (fn_1_F9744(9, &bowl1, 1) != 0)
                {
                    fn_1_F8060(9, 1);
                    printf("entry: TRUE\n");
                }
                else
                {
                    fn_1_F8060(9, 0);
                    printf("entry: FALSE\n");
                }
                break;
            case 10:
                fn_1_F8810(&bowl2);
                if (fn_1_F9744(10, &bowl2, 1) != 0)
                {
                    fn_1_F8060(10, 1);
                    printf("entry: TRUE\n");
                }
                else
                {
                    fn_1_F8060(10, 0);
                    printf("entry: FALSE\n");
                }
                break;
            case 11:
                fn_1_F88B8(&golf);
                if (fn_1_F9744(11, &golf, 1) != 0)
                {
                    fn_1_F8060(11, 1);
                    printf("entry: TRUE\n");
                }
                else
                {
                    fn_1_F8060(11, 0);
                    printf("entry: FALSE\n");
                }
                break;
            case 12:
            case 13:
            case 14:
                fn_1_F89A0(&boat[0]);
                fn_1_F89A0(&boat[1]);
                fn_1_F89A0(&boat[2]);
                fn_1_F89A0(&boat[3]);
                fn_1_F89A0(&boat[4]);
                fn_1_F89A0(&boat[5]);
                fn_1_F89A0(&boat[6]);
                fn_1_F89A0(&boat[7]);
                if (fn_1_F9744(modeCtrl.unk10, boat, 4) != 0)
                {
                    fn_1_F8060(modeCtrl.unk10, 1);
                    printf("entry: TRUE\n");
                }
                else
                {
                    fn_1_F8060(modeCtrl.unk10, 0);
                    printf("entry: FALSE\n");
                }
                break;
            }
        }
    }
}

void fn_1_F1AD0(void)
{
    switch (sub_mode)
    {
    case 0xB5:
    case 0xB6:
        set_sub_mode_destination(fn_1_F1AD0);
        break;
    default:
        fn_1_F828C();
        break;
    }
}

void fn_1_F1B20(int mode, u32 flags)
{
    int i;

    lbl_1_bss_D2B88.mode = (s8)mode;
    lbl_1_bss_D2B88.flags |= flags;
    lbl_1_bss_D2B88.unkF = 0;
    lbl_1_bss_D2B88.unk10 = 2;
    lbl_1_bss_D2B88.unk8 = 0;
    lbl_1_bss_D2B88.unk9 = 0;
    for (i = 0; i < 3; i++)
        lbl_1_bss_D2B88.unk11[i] = 0;
    for (i = 0; i < 4; i++)
    {
        if (lbl_1_bss_D2B88.unkB[i] >= 0)
        {
            lbl_1_bss_D2B88.unkA = (s8)i;
            break;
        }
    }
    if (lbl_1_bss_D2B88.flags & 1)
    {
        lbl_1_bss_D2B88.unk7 = 8;
        lbl_1_bss_D2B88.unk1C = 412.0f;
        lbl_1_bss_D2B88.unk20 = 118.0f;
        lbl_1_bss_D2B88.unk24 = 60.0f;
    }
    else
    {
        lbl_1_bss_D2B88.unk7 = 0;
        lbl_1_bss_D2B88.unk1C = 500.0f;
        lbl_1_bss_D2B88.unk20 = 122.0f;
        lbl_1_bss_D2B88.unk24 = 68.0f;
    }
    if ((lbl_1_bss_D2B88.flags & 1) && (lbl_1_bss_D2B88.flags & 6))
        lbl_1_bss_D2B88.unk28 = 43.5f;
    else
        lbl_1_bss_D2B88.unk28 = 60.0f;
}

void fn_1_F7794(int mode, int flags);
void fn_1_F7958(int mode, int flags);
void fn_1_F7ACC(int mode, int flags);
void fn_1_F7D28(int mode, int flags);
void fn_1_F7E80(int mode, int flags);

static struct RecTime lbl_1_data_61F18[5];
static struct RecTime lbl_1_data_61F48[5];
static struct RecTime lbl_1_data_61F78[5];
static struct RecTime lbl_1_data_61FA8[5];
static struct RecTime lbl_1_data_61FD8[5];
static struct RecTime lbl_1_data_62008[5];
static struct RecTime lbl_1_data_62114[5];
static struct RecTime lbl_1_data_62144[5];
static struct RecTime lbl_1_data_62174[5];
static struct RecScore lbl_1_data_62038[5];
static struct RecScore lbl_1_data_62060[5];
static struct RecScore lbl_1_data_62088[5];
static struct RecBowl lbl_1_data_620B0[2][5];
static struct RecGolf lbl_1_data_620EC[5];

struct RankingMode lbl_1_data_61E28[15] = {
    { fn_1_F7794, lbl_1_data_61F18, 8, "spr_bg/bg_rac.gct" },
    { fn_1_F7794, lbl_1_data_61F48, 8, "spr_bg/bg_rac.gct" },
    { fn_1_F7794, lbl_1_data_61F78, 8, "spr_bg/bg_rac.gct" },
    { fn_1_F7794, lbl_1_data_61FA8, 8, "spr_bg/bg_rac.gct" },
    { fn_1_F7794, lbl_1_data_61FD8, 8, "spr_bg/bg_rac.gct" },
    { fn_1_F7794, lbl_1_data_62008, 8, "spr_bg/bg_rac.gct" },
    { fn_1_F7958, lbl_1_data_62038, 8, "spr_bg/bg_tgt.gct" },
    { fn_1_F7958, lbl_1_data_62060, 8, "spr_bg/bg_tgt.gct" },
    { fn_1_F7958, lbl_1_data_62088, 8, "spr_bg/bg_tgt.gct" },
    { fn_1_F7ACC, lbl_1_data_620B0[0], 6, "spr_bg/bg_bwl.gct" },
    { fn_1_F7ACC, lbl_1_data_620B0[1], 6, "spr_bg/bg_bwl.gct" },
    { fn_1_F7D28, lbl_1_data_620EC, 8, "spr_bg/bg_glf.gct" },
    { fn_1_F7E80, lbl_1_data_62114, 8, "spr_bg/bg_glf.gct" },
    { fn_1_F7E80, lbl_1_data_62144, 8, "spr_bg/bg_glf.gct" },
    { fn_1_F7E80, lbl_1_data_62174, 8, "spr_bg/bg_glf.gct" },
};

static struct RecTime lbl_1_data_61F18[5] = {
    { "JAM", 0, 99, 59, 99, 0 },
    { "MKA", 1, 99, 59, 99, 0 },
    { "@RI", 2, 99, 59, 99, 0 },
    { "HIR", 3, 99, 59, 99, 0 },
    { "UED", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_61F40 = { "NAG", 1, 99, 59, 99, 0 };
static struct RecTime lbl_1_data_61F48[5] = {
    { "DAI", 0, 99, 59, 99, 0 },
    { "KMK", 1, 99, 59, 99, 0 },
    { "TBC", 2, 99, 59, 99, 0 },
    { "ODA", 3, 99, 59, 99, 0 },
    { "MAS", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_61F70 = { "NAG", 1, 99, 59, 99, 0 };
static struct RecTime lbl_1_data_61F78[5] = {
    { "IZA", 0, 99, 59, 99, 0 },
    { "MCI", 1, 99, 59, 99, 0 },
    { "MIC", 2, 99, 59, 99, 0 },
    { "M.S", 3, 99, 59, 99, 0 },
    { "NIN", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_61FA0 = { "NAG", 1, 99, 59, 99, 0 };
static struct RecTime lbl_1_data_61FA8[5] = {
    { "MA2", 0, 99, 59, 99, 0 },
    { "N.H", 1, 99, 59, 99, 0 },
    { "Y.S", 2, 99, 59, 99, 0 },
    { "AGE", 3, 99, 59, 99, 0 },
    { "SAK", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_61FD0 = { "NAG", 1, 99, 59, 99, 0 };
static struct RecTime lbl_1_data_61FD8[5] = {
    { "KAK", 0, 99, 59, 99, 0 },
    { "YSM", 1, 99, 59, 99, 0 },
    { "Y.M", 2, 99, 59, 99, 0 },
    { "MIE", 3, 99, 59, 99, 0 },
    { "TMY", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_62000 = { "NAG", 1, 99, 59, 99, 0 };
static struct RecTime lbl_1_data_62008[5] = {
    { "YAU", 0, 99, 59, 99, 0 },
    { "H.E", 1, 99, 59, 99, 0 },
    { "JUN", 2, 99, 59, 99, 0 },
    { "TOK", 3, 99, 59, 99, 0 },
    { "SHO", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_62030 = { "NAG", 1, 99, 59, 99, 0 };
static struct RecScore lbl_1_data_62038[5] = {
    { "SAK", 0, 50, 0 },
    { "M.S", 1, 40, 0 },
    { "ODA", 2, 30, 0 },
    { "MKA", 3, 20, 0 },
    { "JAM", 0, 10, 0 },
};
static struct RecScore lbl_1_data_62060[5] = {
    { "YSM", 0, 50, 0 },
    { "N.H", 1, 40, 0 },
    { "TBC", 2, 30, 0 },
    { "HIR", 3, 20, 0 },
    { "@RI", 0, 10, 0 },
};
static struct RecScore lbl_1_data_62088[5] = {
    { "SHO", 0, 50, 0 },
    { "AGE", 1, 40, 0 },
    { "Y.S", 2, 30, 0 },
    { "JUN", 3, 20, 0 },
    { "H.E", 0, 10, 0 },
};
static struct RecBowl lbl_1_data_620B0[2][5] = {
    {
        { "Y.M", 0, 50 },
        { "MA2", 1, 40 },
        { "MCI", 2, 30 },
        { "MIC", 3, 20 },
        { "NIN", 0, 10 },
    },
    {
        { "IZA", 0, 3 },
        { "KAK", 1, 2 },
        { "YAU", 2, 2 },
        { "MIE", 3, 1 },
        { "MAS", 0, 1 },
    },
};
static struct RecGolf lbl_1_data_620EC[5] = {
    { "TOK", 0, 99, 99 },
    { "DAI", 1, 99, 99 },
    { "KMK", 2, 99, 99 },
    { "TMY", 3, 99, 99 },
    { "UED", 0, 99, 99 },
};
static struct RecTime lbl_1_data_62114[5] = {
    { "JAM", 0, 99, 59, 99, 0 },
    { "MKA", 1, 99, 59, 99, 0 },
    { "@RI", 2, 99, 59, 99, 0 },
    { "HIR", 3, 99, 59, 99, 0 },
    { "UED", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_6213C = { "NAG", 1, 99, 59, 99, 0 };
static struct RecTime lbl_1_data_62144[5] = {
    { "DAI", 0, 99, 59, 99, 0 },
    { "KMK", 1, 99, 59, 99, 0 },
    { "TBC", 2, 99, 59, 99, 0 },
    { "ODA", 3, 99, 59, 99, 0 },
    { "MAS", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_6216C = { "NAG", 1, 99, 59, 99, 0 };
static struct RecTime lbl_1_data_62174[5] = {
    { "IZA", 0, 99, 59, 99, 0 },
    { "MCI", 1, 99, 59, 99, 0 },
    { "MIC", 2, 99, 59, 99, 0 },
    { "M.S", 3, 99, 59, 99, 0 },
    { "NIN", 0, 99, 59, 99, 0 },
};
static struct RecTime lbl_1_data_6219C = { "NAG", 1, 99, 59, 99, 0 };

struct RecTime *lbl_1_data_621A4[6] = { &lbl_1_data_61F40, &lbl_1_data_61F70, &lbl_1_data_61FA0, &lbl_1_data_61FD0, &lbl_1_data_62000, &lbl_1_data_62030 };
struct RecTime *lbl_1_data_621BC[3] = { &lbl_1_data_6213C, &lbl_1_data_6216C, &lbl_1_data_6219C };
static struct SpriteDrawRequest lbl_1_data_621C8 =
    { 0x319, 112.0f, 23.0f, 9.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62218 = { 0x31A, 76.0f, 0.0f, 9.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62268 = { 0x324, 53.0f, 91.0f, 1.0f, 0.491071433f, 0.160714284f, 0.0f, 0.0f, 0.491071433f, 0.160714284f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_622B8 = { 0x324, 196.0f, 91.0f, 1.0f, 0.5f, 0.160714284f, 0.0f, 0.160714284f, 0.5f, 0.321428567f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62308 = { 0x324, 425.0f, 91.0f, 1.0f, 0.571428597f, 0.160714284f, 0.0f, 0.321428567f, 0.571428597f, 0.482142866f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62358 = { 0x324, 372.0f, 91.0f, 1.0f, 0.580357134f, 0.160714284f, 0.0f, 0.482142866f, 0.580357134f, 0.642857134f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_623A8 = { 0x324, 372.0f, 91.0f, 1.0f, 0.696428597f, 0.160714284f, 0.0f, 0.803571403f, 0.696428597f, 0.964285731f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_623F8 = { 0x324, 513.0f, 91.0f, 1.0f, 0.696428597f, 0.160714284f, 0.0f, 0.803571403f, 0.696428597f, 0.964285731f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62448 = { 0x324, 376.0f, 91.0f, 1.0f, 0.446428567f, 0.160714284f, 0.491071433f, 0.0f, 0.9375f, 0.160714284f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62498 = { 0x324, 371.0f, 91.0f, 1.0f, 0.383928567f, 0.160714284f, 0.5f, 0.160714284f, 0.883928597f, 0.321428567f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_624E8 = { 0x324, 315.0f, 91.0f, 1.0f, 0.383928567f, 0.160714284f, 0.571428597f, 0.321428567f, 0.955357134f, 0.482142866f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62538 = { 0x314, 0.0f, 0.0f, 5.0f, 2.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static u32 lbl_1_data_62588[4] = { 0xFFEF0101, 0xFF4E7DFF, 0xFFF1DE21, 0xFF07A55B };
static struct SpriteDrawRequest lbl_1_data_62598 = { 0x325, 0.0f, 0.0f, 1.0f, 0.25f, 0.25f, 0.0f, 0.5f, 0.25f, 0.75f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_625E8 = { 0x325, 0.0f, 0.0f, 1.0f, 0.25f, 0.25f, 0.0f, 0.0f, 0.25f, 0.25f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62638 = { 0x325, 0.0f, 0.0f, 1.0f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f, 0.5f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62688 = { 0x325, 0.0f, 0.0f, 1.0f, 0.25f, 0.25f, 0.5f, 0.75f, 0.75f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_626D8 = { 0x325, 0.0f, 0.0f, 1.0f, 0.25f, 0.25f, 0.75f, 0.0f, 1.0f, 0.25f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62728 = { 0x316, 0.0f, 0.0f, 3.0f, 4.5f, 4.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFF0000FF, 0x0 };
static struct SpriteDrawRequest *lbl_1_data_62778[4] = { &lbl_1_data_62598, &lbl_1_data_62638, &lbl_1_data_62688, &lbl_1_data_626D8 };
static struct SpriteDrawRequest lbl_1_data_62788 = { 0x31B, 12.0f, 407.0f, 1.0f, 68.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_627D8 = { 0x318, 58.0f, 420.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62828 = { 0x317, 48.0f, 424.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62878 = { 0xF, 0.0f, 0.0f, 1.0f, 0.5f, 0.328125f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, 0xFFFFFFFF, 0x0 };
struct MiniRankingU16Tbl
{
    const u16 *ptr;
    u16 count;
};

static struct MiniRankingU16Tbl lbl_1_data_628C8[4] = {
    { lbl_1_rodata_64D90, 4 },
    { lbl_1_rodata_64D98, 9 },
    { lbl_1_rodata_64DAC, 8 },
    { lbl_1_rodata_64DBC, 5 },
};
static struct SpriteDrawRequest lbl_1_data_628E8 = { 0x31C, 0.0f, 0.0f, 0.100000001f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62938 = { 0x320, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static struct SpriteDrawRequest lbl_1_data_62988 = { 0x31F, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 5, 0xFFFFFFFF, 0x0 };
static char *lbl_1_data_629F0[6] = { "SEX", "XXX", "XX ", "XX", " XX", "X X" };

#define NEXT_PLAYER() \
    { \
        int found = 0; \
        int j; \
\
        for (j = lbl_1_bss_D2B88.unkA + 1; j < 4; j++) \
        { \
            if (lbl_1_bss_D2B88.unkB[j] >= 0) \
            { \
                int k; \
\
                lbl_1_bss_D2B88.unkA = (s8)j; \
                lbl_1_bss_D2B88.unkF = 0; \
                lbl_1_bss_D2B88.unk10 = 2; \
                lbl_1_bss_D2B88.unk8 = 0; \
                lbl_1_bss_D2B88.unk9 = 0; \
                for (k = 0; k < 3; k++) \
                    lbl_1_bss_D2B88.unk11[k] = 0; \
                found = 1; \
                break; \
            } \
        } \
        if (!found) \
            lbl_1_bss_D2B88.flags &= ~1; \
    }

void fn_1_F1CF4(s8 *status, struct Sprite *sprite)
{
    int ch;
    char *name;
    u8 *cursor;
    int pad;
    char tmp[4];

    if (lbl_1_bss_D2B88.flags & 1)
    {
        if (++lbl_1_bss_D2B88.unk7 > 8)
            lbl_1_bss_D2B88.unk7 = 8;
        lbl_1_bss_D2B88.unk1C += 0.2 * (412.0 - lbl_1_bss_D2B88.unk1C);
        lbl_1_bss_D2B88.unk20 += 0.2 * (118.0 - lbl_1_bss_D2B88.unk20);
        lbl_1_bss_D2B88.unk24 += 0.2 * (60.0 - lbl_1_bss_D2B88.unk24);
        lbl_1_bss_D2B88.unk9 = 0;
    }
    else
    {
        if (--lbl_1_bss_D2B88.unk7 < 0)
            lbl_1_bss_D2B88.unk7 = 0;
        lbl_1_bss_D2B88.unk1C += 0.2 * (500.0 - lbl_1_bss_D2B88.unk1C);
        lbl_1_bss_D2B88.unk20 += 0.2 * (122.0 - lbl_1_bss_D2B88.unk20);
        lbl_1_bss_D2B88.unk24 += 0.2 * (68.0 - lbl_1_bss_D2B88.unk24);
        if (lbl_1_bss_D2B88.unk9 < 60)
            lbl_1_bss_D2B88.unk9++;
    }
    if ((lbl_1_bss_D2B88.flags & 1) && (lbl_1_bss_D2B88.flags & 6))
        lbl_1_bss_D2B88.unk28 += 0.2 * (43.5 - lbl_1_bss_D2B88.unk28);
    else
        lbl_1_bss_D2B88.unk28 += 0.2 * (60.0 - lbl_1_bss_D2B88.unk28);
    if (lbl_1_bss_D2B88.unk8 < 8)
        lbl_1_bss_D2B88.unk8++;
    {
        u32 c = 255 - (u32)(127.0f * lbl_1_bss_D2B88.unk7 / 8.0f);

        lbl_1_bss_D2B88.unk18 = 0xFF000000 | (c << 16) | (c << 8) | c;
    }
    if (lbl_1_bss_D2B88.flags & 1)
    {
        pad = player_pad_map[lbl_1_bss_D2B88.unkA];
        if ((lbl_1_bss_D2B88.flags & 4) && lbl_1_bss_D2B88.unkA == lbl_1_bss_D2B88.unk5)
        {
            switch (lbl_1_bss_D2B88.mode)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                name = (char *)lbl_1_data_621A4[lbl_1_bss_D2B88.mode];
                break;
            case 12:
            case 13:
            case 14:
                name = (char *)lbl_1_data_621BC[lbl_1_bss_D2B88.mode - 12];
                break;
            }
        }
        else
        {
            name = lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entries;
            name += lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] * lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entrySize;
        }
        if (lbl_1_bss_D2B88.unkF < 3)
        {
            if ((lbl_1_bss_D2B88.flags & 4) && lbl_1_bss_D2B88.unkA == lbl_1_bss_D2B88.unk5)
                cursor = (u8 *)name + lbl_1_bss_D2B88.unkF;
            else
                cursor = (u8 *)name + lbl_1_bss_D2B88.unkF;
            ch = lbl_1_bss_D2B88.unk10;
            pad = player_pad_map[lbl_1_bss_D2B88.unkA];
            if ((pad_status_groups[pad].unk0[4].button & PAD_BUTTON_LEFT) || (analog_inputs[pad][4] & PAD_BUTTON_LEFT))
            {
                ch--;
                if (ch < 0)
                    ch = sizeof(lbl_1_data_61D70) - 1;
            }
            if ((pad_status_groups[pad].unk0[4].button & PAD_BUTTON_RIGHT) || (analog_inputs[pad][4] & PAD_BUTTON_RIGHT))
            {
                ch++;
                if (ch >= sizeof(lbl_1_data_61D70))
                    ch = 0;
            }
            if (ch != lbl_1_bss_D2B88.unk10)
                call_SoundReqID_arg_0(0x6C);
            lbl_1_bss_D2B88.unk10 = (s8)ch;
            *cursor = lbl_1_data_61D70[ch];
        }
        else
        {
            cursor = (u8 *)tmp;
            ch = lbl_1_bss_D2B88.unk10;
            if ((pad_status_groups[pad].unk0[4].button & PAD_BUTTON_LEFT) || (analog_inputs[pad][4] & PAD_BUTTON_LEFT))
            {
                ch--;
                if (ch < 0)
                    ch = 1;
            }
            if ((pad_status_groups[pad].unk0[4].button & PAD_BUTTON_RIGHT) || (analog_inputs[pad][4] & PAD_BUTTON_RIGHT))
            {
                ch++;
                if (ch >= 2)
                    ch = 0;
            }
            if (ch != lbl_1_bss_D2B88.unk10)
                call_SoundReqID_arg_0(0x6C);
            lbl_1_bss_D2B88.unk10 = (s8)ch;
            *cursor = lbl_1_data_61D70[ch];
        }
        if ((pad_status_groups[pad].unk0[2].button & PAD_BUTTON_A) && lbl_1_bss_D2B88.unk8 >= 8)
        {
            switch (*cursor)
            {
            case 0x5C:
                if (lbl_1_bss_D2B88.unkF > 0)
                {
                    if (lbl_1_bss_D2B88.unkF < 3)
                        *cursor = 0;
                    lbl_1_bss_D2B88.unkF--;
                    call_SoundReqID_arg_0(0x66);
                }
                break;
            case 0x5D:
                *cursor = 0;
                for (ch = 0; ch < sizeof(lbl_1_data_629F0) / sizeof(lbl_1_data_629F0[0]); ch++)
                {
                    if (strncmp(name, lbl_1_data_629F0[ch], 3) == 0)
                    {
                        strncpy(name, "---", 3);
                        break;
                    }
                }
                NEXT_PLAYER();
                call_SoundReqID_arg_0(0x6A);
                break;
            case 0x5B:
                *cursor = 0x20;
            default:
                lbl_1_bss_D2B88.unk11[lbl_1_bss_D2B88.unkF] = lbl_1_bss_D2B88.unk10;
                lbl_1_bss_D2B88.unkF++;
                if (lbl_1_bss_D2B88.unkF >= 3)
                    lbl_1_bss_D2B88.unk10 = 1;
                else if (lbl_1_bss_D2B88.unkF >= 4)
                {
                    for (ch = 0; ch < sizeof(lbl_1_data_629F0) / sizeof(lbl_1_data_629F0[0]); ch++)
                    {
                        if (strncmp(name, lbl_1_data_629F0[ch], 3) == 0)
                        {
                            strncpy(name, "---", 3);
                            break;
                        }
                    }
                    NEXT_PLAYER();
                }
                if (lbl_1_bss_D2B88.unkF < 4)
                    call_SoundReqID_arg_0(0x65);
                else
                    call_SoundReqID_arg_0(0x6A);
                break;
            }
        }
        if ((pad_status_groups[pad].unk0[2].button & PAD_BUTTON_B) && lbl_1_bss_D2B88.unk7 >= 8
         && lbl_1_bss_D2B88.unkF > 0)
        {
            if (lbl_1_bss_D2B88.unkF < 3)
                *cursor = 0;
            lbl_1_bss_D2B88.unkF--;
            lbl_1_bss_D2B88.unk10 = lbl_1_bss_D2B88.unk11[lbl_1_bss_D2B88.unkF];
            call_SoundReqID_arg_0(0x66);
        }
        if (lbl_1_bss_D2B88.flags & 2)
        {
            switch (lbl_1_bss_D2B88.mode)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                {
                    char *src = (char *)lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entries
                        + lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unk5] * lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entrySize;

                    memcpy_handler(lbl_1_data_621A4[lbl_1_bss_D2B88.mode], src, 3);
                }
                break;
            case 12:
            case 13:
            case 14:
                {
                    char *src = (char *)lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entries
                        + lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unk5] * lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entrySize;

                    memcpy_handler(lbl_1_data_621BC[lbl_1_bss_D2B88.mode - 12], src, 3);
                }
                break;
            }
        }
    }
}

static inline u32 fn_1_blink2(void)
{
    u32 t = g_font_char_var_3 % 60;

    if (t > 30)
        t = 60 - t;
    t <<= 2;
    t = (t << 16) | (t << 8) | t;
    return t;
}

static inline u32 fn_1_blink3(void)
{
    u32 t = g_font_char_var_3 % 60;

    if (t > 30)
        t = 60 - t;
    t <<= 3;
    t = (t << 16) | (t << 8) | t;
    return t;
}

static inline u32 fn_1_highlight(int player)
{
    if (lbl_1_bss_D2B88.flags & 1)
        return 0;
    if (!(lbl_1_bss_D2B88.unk14 & (1 << player)))
        return 0;
    return fn_1_blink3();
}

void fn_1_F2A4C(void)
{
    struct Sprite sprite;
    struct SpriteDrawRequest *req;

    fn_1_F9EAC(lbl_1_bss_D2B88.tex, 0.0f, 0.0f, 10.0f, 640.0f, 480.0f, -1, 0);
    draw_sprite_draw_request(&lbl_1_data_621C8);
    req = &lbl_1_data_62218;
    req->y = lbl_1_bss_D2B88.unk1C;
    draw_sprite_draw_request(req);
    sprite.x = 122.0f;
    sprite.y = 423.0 + lbl_1_bss_D2B88.unk1C - 412.0;
    sprite.depth = 0.1f;
    sprite.scaleX = 1.0f;
    sprite.scaleY = 1.0f;
    sprite.fontId = 0x8E;
    sprite.addR = 0;
    sprite.addG = 0;
    sprite.addB = 0;
    sprite.mulR = 0xFF;
    sprite.mulG = 0xFF;
    sprite.mulB = 0xFF;
    strcpy(sprite.text, "p/LEVER/         p/BUTTON_A/");
    fn_1_F35F4(&sprite);
    sprite.mulR = 0xFF;
    sprite.mulG = 0xFF;
    sprite.mulB = 0;
    strcpy(sprite.text, " h/DE   WOERANDE DEKIMETENE");
    fn_1_F35F4(&sprite);
    sprite.mulR = 0xFF;
    sprite.mulG = 0xA2;
    sprite.mulB = 0;
    strcpy(sprite.text, "  h/NAMAE");
    fn_1_F35F4(&sprite);
}

inline void fn_1_row_bg(int i, float x, float y)
{
    int found;
    int j;
    struct SpriteDrawRequest *req;

    found = 0;
    for (j = 0; j < 4; j++)
    {
        if (i == lbl_1_bss_D2B88.unkB[j])
        {
            found = 1;
            break;
        }
    }
    req = &lbl_1_data_62538;
    if (found)
    {
        u32 c;

        if (lbl_1_bss_D2B88.unk6 == 1)
            c = lbl_1_data_62588[3];
        else
            c = lbl_1_data_62588[j];
        req->bmpId = 0x326;
        if (i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA])
        {
            float scale = 1.0 - 0.5 * lbl_1_bss_D2B88.unk7 / 8.0;
            u8 a = (c >> 24) & 0xFF;
            u8 r = (c >> 16) & 0xFF;
            u8 g = (c >> 8) & 0xFF;
            u8 b = c & 0xFF;

            a = a * scale;
            r = r * scale;
            g = g * scale;
            b = b * scale;
            req->mulColor = (a << 24) | (r << 16) | (g << 8) | b;
        }
        else
            req->mulColor = c;
    }
    else
    {
        req->bmpId = 0x314;
        req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
    }
    req->x = x;
    req->y = y;
    draw_sprite_draw_request(req);
}

inline void fn_1_row_num(int i, float x, float y)
{
    int found;
    int j;

    textdraw_reset();
    textdraw_set_font(0x22);
    textdraw_set_mul_color(i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
    textdraw_set_add_color(fn_1_highlight(i));
    textdraw_set_pos(x, y);
    textdraw_printf("%d", i + 1);
    found = 0;
    for (j = 0; j < 4; j++)
    {
        if (i == lbl_1_bss_D2B88.unkB[j])
        {
            found = 1;
            break;
        }
    }
    if (found)
    {
        struct SpriteDrawRequest *req;

        if (lbl_1_bss_D2B88.unk6 == 1)
            req = &lbl_1_data_625E8;
        else
            req = lbl_1_data_62778[j];
        req->x = x - 26.0;
        req->y = y - 10.0;
        req->addColor = fn_1_blink2();
        draw_sprite_draw_request(req);
    }
}

void fn_1_dead1(u32 u)
{
    textdraw_printf("%d", (float)u);
}

void fn_1_dead2(float x)
{
    x = 1.0 + x;
    x = 0.5 + x;
    x = 0.125 + x;
    x = 26.0 + x;
    x = 10.0 + x;
    textdraw_set_depth(x);
}

void fn_1_F2BBC(float x, float y, u8 *name, int player)
{
    int i;

    textdraw_reset();
    textdraw_set_font(0x20);
    for (i = 0; *name != 0 && i < 3; i++, name++)
    {
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0xFF000000);
        if ((lbl_1_bss_D2B88.flags & 1)
            && player == lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA]
            && i == lbl_1_bss_D2B88.unkF)
            textdraw_set_add_color(fn_1_blink2());
        else if (!(lbl_1_bss_D2B88.flags & 1))
            textdraw_set_add_color(fn_1_highlight(player));
        textdraw_set_pos(2.0f + (x + i * 36), 2.0f + y);
        textdraw_printf("%c", *name);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(player != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
        textdraw_set_pos(x + i * 36, y);
        textdraw_printf("%c", *name);
    }
    if ((lbl_1_bss_D2B88.flags & 1) && player == lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA])
    {
        struct SpriteDrawRequest *req = &lbl_1_data_62728;

        req->x = x + lbl_1_bss_D2B88.unkF * 36 - 2.0f;
        req->y = y - 2.0f;
        req->addColor = fn_1_blink3();
        draw_sprite_draw_request(req);
        if (lbl_1_bss_D2B88.unkF >= 3)
        {
            textdraw_set_scale(1.0f, 1.0f);
            textdraw_set_depth(2.0f);
            textdraw_set_mul_color(0xFF000000);
            if (i == lbl_1_bss_D2B88.unkF)
                textdraw_set_add_color(fn_1_blink2());
            textdraw_set_pos(2.0f + (x + i * 36), 2.0f + y);
            textdraw_printf("%c", lbl_1_data_61D70[lbl_1_bss_D2B88.unk10]);
            textdraw_set_depth(1.0f);
            textdraw_set_mul_color(player != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
            textdraw_set_pos(x + i * 36, y);
            textdraw_printf("%c", lbl_1_data_61D70[lbl_1_bss_D2B88.unk10]);
        }
    }
}

void fn_1_F3184(float x, float y, u8 *time, int player)
{
    static float lbl_1_data_62A5C[3] = { 0.0f, 40.0f, 84.0f };
    int i;
    int v;

    textdraw_reset();
    textdraw_set_font(0x21);
    textdraw_set_add_color(fn_1_highlight(player));
    for (i = 0; i < 3; i++, time++)
    {
        v = *time;
        if (i == 1)
            v = v < 0 ? 0 : (v > 59 ? 59 : v);
        else
            v = v < 0 ? 0 : (v > 99 ? 99 : v);
        textdraw_set_depth(2.0f);
        textdraw_set_mul_color(0xFF000000);
        textdraw_set_pos(2.0 + (x + lbl_1_data_62A5C[i]), 2.0 + y);
        textdraw_printf("%d", v / 10);
        textdraw_set_pos(2.0 + (17.0 + (x + lbl_1_data_62A5C[i])), 2.0 + y);
        textdraw_printf("%d", v % 10);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(player != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
        textdraw_set_pos(x + lbl_1_data_62A5C[i], y);
        textdraw_printf("%d", v / 10);
        textdraw_set_pos(17.0 + (x + lbl_1_data_62A5C[i]), y);
        textdraw_printf("%d", v % 10);
    }
    textdraw_set_depth(2.0f);
    textdraw_set_mul_color(0xFF000000);
    textdraw_set_pos(2.0 + (380.0 + x - 346.0), y);
    textdraw_print(":");
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(player != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
    textdraw_set_pos(380.0 + x - 346.0, y - 2.0);
    textdraw_print(":");
    textdraw_set_depth(2.0f);
    textdraw_set_mul_color(0xFF000000);
    textdraw_set_pos(2.0 + (420.0 + x - 346.0), y);
    textdraw_print(";");
    textdraw_set_depth(1.0f);
    textdraw_set_mul_color(player != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
    textdraw_set_pos(420.0 + x - 346.0, y - 2.0);
    textdraw_print(";");
}

inline void fn_1_row_chara(int chara, int i, float x, float y)
{
    struct SpriteDrawRequest *req;
    struct MiniRankingU16Tbl *t;
    int a;
    int b;

    if (chara >= 0 && chara < 4)
    {
        t = &lbl_1_data_628C8[chara];
        a = (g_font_char_var_3 >> 3) % (t->count * 2 - 2);
        if (a >= t->count)
            a = (t->count * 2 - 2) - a;
        b = ((g_font_char_var_3 >> 3) - 1) % (t->count * 2 - 2);
        if (b >= t->count)
            b = (t->count * 2 - 2) - b;
        req = &lbl_1_data_62878;
        req->bmpId = t->ptr[a];
        req->x = 32.0 + x;
        req->y = 21.0 + y;
        req->z = 1.0f;
        req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
        req->opacity = 1.0f;
        req->rotation = 4096.0f * mathutil_cos(g_font_char_var_3 * 400);
        draw_sprite_draw_request(req);
        req = &lbl_1_data_62878;
        req->bmpId = t->ptr[b];
        req->z = 0.5f;
        req->opacity = 1.0 - (double)(g_font_char_var_3 & 7) / 8.0;
        draw_sprite_draw_request(req);
    }
}

void fn_1_dead3(float x)
{
    x = 32.0 + x;
    x = 21.0 + x;
    x = 4096.0f + x;
    x = 0.5f + x;
    textdraw_set_depth(x);
}

void fn_1_F35F4(struct Sprite *sprite)
{
    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_scale(sprite->scaleX, sprite->scaleY);
    textdraw_set_depth(0.1 + sprite->depth);
    textdraw_set_mul_color(0);
    textdraw_set_add_color(0);
    textdraw_set_pos(2.0 + sprite->x, 2.0 + sprite->y);
    textdraw_print(sprite->text);
    textdraw_set_depth(sprite->depth);
    textdraw_set_mul_color((sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB);
    textdraw_set_add_color((sprite->addR << 16) | (sprite->addG << 8) | sprite->addB);
    textdraw_set_pos(sprite->x, sprite->y);
    textdraw_print(sprite->text);
}

static inline void fn_1_row_icon(struct RecTime *rec, int i, float x, float y)
{
    if (rec->unk7 & 1)
    {
        struct SpriteDrawRequest *req = &lbl_1_data_628E8;

        req->bmpId = 0x31C;
        req->x = x;
        req->y = y;
        req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
        draw_sprite_draw_request(req);
        x += 16.0;
    }
}

void fn_1_F36F8(struct Sprite *sprite)
{
    int i;
    float _dummy_pool = 0.0f;
    float x;
    float y;
    float d;
    struct RecTime *rec;

    fn_1_F2A4C();
    draw_sprite_draw_request(&lbl_1_data_62268);
    draw_sprite_draw_request(&lbl_1_data_622B8);
    draw_sprite_draw_request(&lbl_1_data_62448);
    draw_sprite_draw_request(&lbl_1_data_623F8);
    x = 0.0f;
    y = 118.0f;
    rec = lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entries;
    for (i = 0; i < 5; i++, rec++)
    {
        fn_1_row_bg(i, x, y);
        fn_1_row_num(i, 50.0f + x, y - 1.0f);
        fn_1_F2BBC(172.0 + x, y - 1.0, (u8 *)rec, i);
        fn_1_F3184(346.0 + x, 2.0 + y, &rec->min, i);
        fn_1_row_chara(rec->chara, i, 520.0 + x, y - 5.0);
        fn_1_row_icon(rec, i, 568.0 + x, y - 5.0);
        y += lbl_1_bss_D2B88.unk28;
    }
    if (lbl_1_bss_D2B88.unk7 < 8 || (lbl_1_bss_D2B88.flags & 6))
    {
        rec = lbl_1_data_621A4[lbl_1_bss_D2B88.mode];
        d = 4.0 * (60.0 - lbl_1_bss_D2B88.unk28);
        x = 0.0f;
        y = 424.0 - d;
        if (!(lbl_1_bss_D2B88.flags & 6))
        {
            mtxa_from_identity();
            mtxa_translate_xyz(320.0f, 224.0f, 0.0f);
            mtxa_scale_xyz(1.0 - lbl_1_bss_D2B88.unk7 / 8.0f, 1.0f, 1.0f);
            mtxa_translate_xyz(-320.0f, -224.0f, 0.0f);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
        }
        {
            struct SpriteDrawRequest *req = &lbl_1_data_62788;

            req->y = 407.0 - d;
            draw_sprite_draw_request(req);
        }
        {
            struct SpriteDrawRequest *req = &lbl_1_data_62828;

            req->x = 48.0f;
            req->y = 423.0 - d;
            req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
            draw_sprite_draw_request(req);
        }
        if (lbl_1_bss_D2B88.flags & 6)
        {
            struct SpriteDrawRequest *req = lbl_1_data_62778[lbl_1_bss_D2B88.unk5];

            req->x = 22.0f;
            req->y = 423.0 - d - 10.0;
            req->addColor = fn_1_blink2();
            draw_sprite_draw_request(req);
        }
        fn_1_F2BBC(172.0 + x, y - 1.0, (u8 *)rec, i);
        fn_1_F3184(346.0 + x, 2.0 + y, &rec->min, i);
        fn_1_row_chara(rec->chara, i, 520.0 + x, y - 6.0);
        fn_1_row_icon(rec, i, 568.0 + x, y - 5.0);
        if (!(lbl_1_bss_D2B88.flags & 6))
        {
            mtxa_from_identity();
            GXLoadPosMtxImm(mtxa->mtxA, 0);
        }
    }
}

void fn_1_F43EC(struct Sprite *sprite)
{
    float x;
    float y;
    float d;
    float e;
    int i;
    struct RecScore *rec;

    fn_1_F2A4C();
    draw_sprite_draw_request(&lbl_1_data_62268);
    draw_sprite_draw_request(&lbl_1_data_622B8);
    draw_sprite_draw_request(&lbl_1_data_62358);
    draw_sprite_draw_request(&lbl_1_data_623F8);
    x = 0.0f;
    y = lbl_1_bss_D2B88.unk20;
    rec = lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entries;
    for (i = 0; i < 5; i++, rec++)
    {
        fn_1_row_bg(i, x, y);
        fn_1_row_num(i, 50.0f + x, y - 1.0f);
        fn_1_F2BBC(172.0 + x, y - 1.0, (u8 *)rec, i);
        {
            u32 score = rec->score;
            int div;
            int n;
            int nz;

            textdraw_reset();
            textdraw_set_font(0x23);
            textdraw_set_add_color(fn_1_highlight(i));
            nz = 0;
            for (div = 10000, n = 0; div > 0; div /= 10, n++)
            {
                if (nz != 0 || div < 10 || score / div != 0)
                {
                    textdraw_set_depth(2.0f);
                    textdraw_set_mul_color(0xFF000000);
                    textdraw_set_pos(2.0f + (327.0f + x + (float)(n * 22)), 2.0f + (2.0f + y));
                    textdraw_printf("%d", score / div);
                    textdraw_set_depth(1.0f);
                    textdraw_set_mul_color(i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
                    textdraw_set_pos(327.0f + x + (float)(n * 22), 2.0f + y);
                    textdraw_printf("%d", score / div);
                    score %= div;
                    nz = 1;
                }
            }
        }
        {
            struct SpriteDrawRequest *req = &lbl_1_data_62938;

            req->x = 438.0 + x;
            req->y = 10.0 + y;
            req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
            req->addColor = fn_1_highlight(i);
            draw_sprite_draw_request(req);
        }
        fn_1_row_chara(rec->chara, i, 520.0 + x, y - 5.0);
        d = 568.0 + x;
        e = y - 5.0;
        if (rec->unk6 & 1)
        {
            struct SpriteDrawRequest *req = &lbl_1_data_628E8;

            req->bmpId = 0x327;
            req->x = d;
            req->y = e;
            req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
            draw_sprite_draw_request(req);
            d += 16.0;
        }
        if (rec->unk6 & 2)
        {
            struct SpriteDrawRequest *req = &lbl_1_data_628E8;

            req->bmpId = 0x328;
            req->x = d;
            req->y = e;
            req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
            draw_sprite_draw_request(req);
        }
        y += lbl_1_bss_D2B88.unk24;
    }
}

void fn_1_F4F28(struct Sprite *sprite)
{
    float x;
    float y;
    int i;
    struct RecBowl *rec;

    fn_1_F2A4C();
    draw_sprite_draw_request(&lbl_1_data_62268);
    draw_sprite_draw_request(&lbl_1_data_622B8);
    draw_sprite_draw_request(&lbl_1_data_62358);
    draw_sprite_draw_request(&lbl_1_data_623F8);
    x = 0.0f;
    y = lbl_1_bss_D2B88.unk20;
    rec = lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entries;
    for (i = 0; i < 5; i++, rec++)
    {
        fn_1_row_bg(i, x, y);
        fn_1_row_num(i, 50.0f + x, y - 1.0f);
        fn_1_F2BBC(172.0 + x, y - 1.0, (u8 *)rec, i);
        switch (lbl_1_bss_D2B88.mode)
        {
        case 9:
            {
                u32 score = rec->score;
                int div;
                int n;
                int nz;

                score = score > 300 ? 300 : score;
                textdraw_reset();
                textdraw_set_font(0x23);
                textdraw_set_add_color(fn_1_highlight(i));
                nz = 0;
                for (div = 100, n = 0; div > 0; div /= 10, n++)
                {
                    if (nz != 0 || div < 10 || score / div != 0)
                    {
                        textdraw_set_depth(2.0f);
                        textdraw_set_mul_color(0xFF000000);
                        textdraw_set_pos(2.0f + (357.0f + x + (float)(n * 22)), 2.0f + (2.0f + y));
                        textdraw_printf("%d", score / div);
                        textdraw_set_depth(1.0f);
                        textdraw_set_mul_color(i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
                        textdraw_set_pos(357.0f + x + (float)(n * 22), 2.0f + y);
                        textdraw_printf("%d", score / div);
                        score %= div;
                        nz = 1;
                    }
                }
            }
            {
                struct SpriteDrawRequest *req = &lbl_1_data_62938;

                req->x = 424.0 + x;
                req->y = 12.0 + y;
                req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
                req->addColor = fn_1_highlight(i);
                draw_sprite_draw_request(req);
            }
            break;
        case 10:
            {
                u32 score = rec->score;
                int div;
                int n;
                int nz;

                score = score > 99 ? 99 : score;
                textdraw_reset();
                textdraw_set_font(0x23);
                textdraw_set_add_color(fn_1_highlight(i));
                nz = 0;
                for (div = 10, n = 0; div > 0; div /= 10, n++)
                {
                    if (nz != 0 || div < 10 || score / div != 0)
                    {
                        textdraw_set_depth(2.0f);
                        textdraw_set_mul_color(0xFF000000);
                        textdraw_set_pos(2.0f + (367.0f + x + (float)(n * 22)), 2.0f + (2.0f + y));
                        textdraw_printf("%d", score / div);
                        textdraw_set_depth(1.0f);
                        textdraw_set_mul_color(i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
                        textdraw_set_pos(367.0f + x + (float)(n * 22), 2.0f + y);
                        textdraw_printf("%d", score / div);
                        score %= div;
                        nz = 1;
                    }
                }
            }
            {
                struct SpriteDrawRequest *req = &lbl_1_data_62988;

                req->x = 414.0 + x;
                req->y = 12.0 + y;
                req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
                req->addColor = fn_1_highlight(i);
                draw_sprite_draw_request(req);
            }
            break;
        }
        fn_1_row_chara(rec->chara, i, 520.0 + x, y - 5.0);
        y += lbl_1_bss_D2B88.unk24;
    }
}

void fn_1_F5C8C(struct Sprite *sprite)
{
    float x;
    float y;
    float off;
    int i;
    struct RecGolf *rec;

    fn_1_F2A4C();
    draw_sprite_draw_request(&lbl_1_data_62268);
    draw_sprite_draw_request(&lbl_1_data_622B8);
    draw_sprite_draw_request(&lbl_1_data_624E8);
    draw_sprite_draw_request(&lbl_1_data_62498);
    draw_sprite_draw_request(&lbl_1_data_62308);
    draw_sprite_draw_request(&lbl_1_data_623F8);
    x = 0.0f;
    y = lbl_1_bss_D2B88.unk20;
    rec = lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entries;
    for (i = 0; i < 5; i++, rec++)
    {
        fn_1_row_bg(i, x, y);
        fn_1_row_num(i, 50.0f + x, y - 1.0f);
        fn_1_F2BBC(172.0 + x, y - 1.0, (u8 *)rec, i);
        {
            u32 score = rec->a;
            int div;
            int n;
            int nz;

            score = score > 999 ? 999 : score;
            if (score >= 100)
                off = 0.0f;
            else if (score >= 10)
                off = -9.0f;
            else
                off = -17.0f;
            textdraw_reset();
            textdraw_set_font(0x23);
            textdraw_set_add_color(fn_1_highlight(i));
            textdraw_set_scale(0.8f, 0.78571427f);
            nz = 0;
            for (div = 100, n = 0; div > 0; div /= 10, n++)
            {
                if (nz != 0 || div < 10 || score / div != 0)
                {
                    textdraw_set_depth(2.0f);
                    textdraw_set_mul_color(0xFF000000);
                    textdraw_set_pos(2.0f + (311.0f + x + off + (float)(n * 17)), 2.0f + (5.0f + y));
                    textdraw_printf("%d", score / div);
                    textdraw_set_depth(1.0f);
                    textdraw_set_mul_color(i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
                    textdraw_set_pos(311.0f + x + off + (float)(n * 17), 5.0f + y);
                    textdraw_printf("%d", score / div);
                    score %= div;
                    nz = 1;
                }
            }
        }
        {
            u32 score = rec->b;
            int div;
            int n;
            int nz;

            score = score > 999 ? 999 : score;
            if (score >= 100)
                off = 0.0f;
            else if (score >= 10)
                off = -9.0f;
            else
                off = -17.0f;
            textdraw_reset();
            textdraw_set_font(0x23);
            textdraw_set_add_color(fn_1_highlight(i));
            textdraw_set_scale(0.8f, 0.78571427f);
            nz = 0;
            for (div = 100, n = 0; div > 0; div /= 10, n++)
            {
                if (nz != 0 || div < 10 || score / div != 0)
                {
                    textdraw_set_depth(2.0f);
                    textdraw_set_mul_color(0xFF000000);
                    textdraw_set_pos(2.0f + (367.0f + x + off + (float)(n * 17)), 2.0f + (5.0f + y));
                    textdraw_printf("%d", score / div);
                    textdraw_set_depth(1.0f);
                    textdraw_set_mul_color(i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
                    textdraw_set_pos(367.0f + x + off + (float)(n * 17), 5.0f + y);
                    textdraw_printf("%d", score / div);
                    score %= div;
                    nz = 1;
                }
            }
        }
        {
            u32 score = rec->a + rec->b;
            int div;
            int n;
            int nz;

            score = score > 999 ? 999 : score;
            if (score >= 100)
                off = 0.0f;
            else if (score >= 10)
                off = -9.0f;
            else
                off = -17.0f;
            textdraw_reset();
            textdraw_set_font(0x23);
            textdraw_set_add_color(fn_1_highlight(i));
            nz = 0;
            for (div = 100, n = 0; div > 0; div /= 10, n++)
            {
                if (nz != 0 || div < 10 || score / div != 0)
                {
                    textdraw_set_depth(2.0f);
                    textdraw_set_mul_color(0xFF000000);
                    textdraw_set_pos(2.0f + (424.0f + x + off + (float)(n * 21)), 2.0f + (2.0f + y));
                    textdraw_printf("%d", score / div);
                    textdraw_set_depth(1.0f);
                    textdraw_set_mul_color(i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1);
                    textdraw_set_pos(424.0f + x + off + (float)(n * 21), 2.0f + y);
                    textdraw_printf("%d", score / div);
                    score %= div;
                    nz = 1;
                }
            }
        }
        fn_1_row_chara(rec->chara, i, 520.0 + x, y - 5.0);
        y += lbl_1_bss_D2B88.unk24;
    }
}

void fn_1_F6A9C(struct Sprite *sprite)
{
    int i;
    float _dummy_pool = 0.0f;
    float x;
    float y;
    float d;
    struct RecTime *rec;

    fn_1_F2A4C();
    draw_sprite_draw_request(&lbl_1_data_62268);
    draw_sprite_draw_request(&lbl_1_data_622B8);
    draw_sprite_draw_request(&lbl_1_data_62448);
    draw_sprite_draw_request(&lbl_1_data_623F8);
    x = 0.0f;
    y = 118.0f;
    rec = lbl_1_data_61E28[lbl_1_bss_D2B88.mode].entries;
    for (i = 0; i < 5; i++, rec++)
    {
        fn_1_row_bg(i, x, y);
        fn_1_row_num(i, 50.0f + x, y - 1.0f);
        fn_1_F2BBC(172.0 + x, y - 1.0, (u8 *)rec, i);
        fn_1_F3184(346.0 + x, 2.0 + y, &rec->min, i);
        fn_1_row_chara(rec->chara, i, 520.0 + x, y - 5.0);
        fn_1_row_icon(rec, i, 568.0 + x, y - 5.0);
        y += lbl_1_bss_D2B88.unk28;
    }
    if (lbl_1_bss_D2B88.unk7 < 8 || (lbl_1_bss_D2B88.flags & 6))
    {
        rec = lbl_1_data_621BC[lbl_1_bss_D2B88.mode - 12];
        d = 4.0 * (60.0 - lbl_1_bss_D2B88.unk28);
        x = 0.0f;
        y = 424.0 - d;
        if (!(lbl_1_bss_D2B88.flags & 6))
        {
            mtxa_from_identity();
            mtxa_translate_xyz(320.0f, 224.0f, 0.0f);
            mtxa_scale_xyz(1.0 - lbl_1_bss_D2B88.unk7 / 8.0f, 1.0f, 1.0f);
            mtxa_translate_xyz(-320.0f, -224.0f, 0.0f);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
        }
        {
            struct SpriteDrawRequest *req = &lbl_1_data_62788;

            req->y = 407.0 - d;
            draw_sprite_draw_request(req);
        }
        {
            struct SpriteDrawRequest *req = &lbl_1_data_62828;

            req->x = 48.0f;
            req->y = 423.0 - d;
            req->mulColor = i != lbl_1_bss_D2B88.unkB[lbl_1_bss_D2B88.unkA] ? lbl_1_bss_D2B88.unk18 : -1;
            draw_sprite_draw_request(req);
        }
        if (lbl_1_bss_D2B88.flags & 6)
        {
            struct SpriteDrawRequest *req = lbl_1_data_62778[lbl_1_bss_D2B88.unk5];

            req->x = 22.0f;
            req->y = 423.0 - d - 10.0;
            req->addColor = fn_1_blink2();
            draw_sprite_draw_request(req);
        }
        fn_1_F2BBC(172.0 + x, y - 1.0, (u8 *)rec, i);
        fn_1_F3184(346.0 + x, 2.0 + y, &rec->min, i);
        fn_1_row_chara(rec->chara, i, 520.0 + x, y - 6.0);
        fn_1_row_icon(rec, i, 568.0 + x, y - 5.0);
        if (!(lbl_1_bss_D2B88.flags & 6))
        {
            mtxa_from_identity();
            GXLoadPosMtxImm(mtxa->mtxA, 0);
        }
    }
}

struct MiniRankingName
{
    char *name;
    int len;
};

void fn_1_F7794(int mode, int flags)
{
    static struct MiniRankingName lbl_1_data_62AF4[6] = {
        { "k/~JYANNGURUSA-KITTO~", 10 },
        { "k/~AKUAOHURO-DO~", 8 },
        { "k/~HURO-ZUNNHAIUXEI~", 10 },
        { "k/~SUKAIDAUNNTAUNN~", 9 },
        { "k/~PAIPUWA-PUTONNNERU~", 10 },
        { "k/~SUPI-DODEZA-TO~", 8 },
    };
    struct Sprite *sprite;

    fn_1_F1B20(mode, flags);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->x = 160.0f;
        sprite->y = 34.0f;
        sprite->textAlign = 0;
        sprite->fontId = 0x8f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->drawFunc = fn_1_F35F4;
        strcpy(sprite->text, "k/RE-SU");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->x = 358.0 - 19.2 * (0.5 * (double)lbl_1_data_62AF4[lbl_1_bss_D2B88.mode].len);
        sprite->y = 37.0f;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->textAlign = 0;
        sprite->fontId = 0x8f;
        sprite->mulR = 0xEF;
        sprite->mulG = 0x9A;
        sprite->mulB = 0;
        sprite->drawFunc = fn_1_F35F4;
        strcpy(sprite->text, lbl_1_data_62AF4[lbl_1_bss_D2B88.mode].name);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->depth = 1.0f;
        sprite->mainFunc = fn_1_F1CF4;
        sprite->drawFunc = fn_1_F36F8;
        strcpy(sprite->text, "RANK");
    }
}

void fn_1_F7958(int mode, int flags)
{
    static char *lbl_1_data_62B64[3] = { "a/~5k/RAUNNDO~", "a/~10k/RAUNNDO~", "a/~15k/RAUNNDO~" };
    struct Sprite *sprite;

    fn_1_F1B20(mode, flags);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->x = 188.0f;
        sprite->y = 34.0f;
        sprite->textAlign = 0;
        sprite->fontId = 0x8f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->drawFunc = fn_1_F35F4;
        strcpy(sprite->text, "k/TA-GETTO");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->x = 328.0f;
        sprite->y = 37.0f;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->textAlign = 0;
        sprite->fontId = 0x8f;
        sprite->mulR = 0xEF;
        sprite->mulG = 0x9A;
        sprite->mulB = 0;
        sprite->drawFunc = fn_1_F35F4;
        strcpy(sprite->text, lbl_1_data_62B64[lbl_1_bss_D2B88.mode - 6]);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->depth = 1.0f;
        sprite->mainFunc = fn_1_F1CF4;
        sprite->drawFunc = fn_1_F43EC;
        strcpy(sprite->text, "RANK");
    }
}

void fn_1_F7ACC(int mode, int flags)
{
    struct Sprite *sprite;

    fn_1_F1B20(mode, flags);
    switch (lbl_1_bss_D2B88.mode)
    {
    case 9:
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 0x65;
            sprite->x = 152.0f;
            sprite->y = 34.0f;
            sprite->textAlign = 0;
            sprite->fontId = 0x8f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0;
            sprite->drawFunc = fn_1_F35F4;
            strcpy(sprite->text, "k/BOURINGU ");
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 0x65;
            sprite->x = 304.0f;
            sprite->y = 37.0f;
            sprite->scaleX = 0.8f;
            sprite->scaleY = 0.8f;
            sprite->fontId = 0x8f;
            sprite->textAlign = 4;
            sprite->mulR = 0xEF;
            sprite->mulG = 0x9A;
            sprite->mulB = 0;
            sprite->drawFunc = fn_1_F35F4;
            strcpy(sprite->text, "k/~NO-MARU MO-DO~");
        }
        break;
    case 10:
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 0x65;
            sprite->x = 152.0f;
            sprite->y = 34.0f;
            sprite->textAlign = 0;
            sprite->fontId = 0x8f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0;
            sprite->drawFunc = fn_1_F35F4;
            strcpy(sprite->text, "k/BOURINGU ");
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 0x65;
            sprite->x = 294.0f;
            sprite->y = 37.0f;
            sprite->scaleX = 0.8f;
            sprite->scaleY = 0.8f;
            sprite->fontId = 0x8f;
            sprite->textAlign = 4;
            sprite->mulR = 0xEF;
            sprite->mulG = 0x9A;
            sprite->mulB = 0;
            sprite->drawFunc = fn_1_F35F4;
            strcpy(sprite->text, "k/~CHARENJI MO-DO~");
        }
        break;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->depth = 1.0f;
        sprite->mainFunc = fn_1_F1CF4;
        sprite->drawFunc = fn_1_F4F28;
        strcpy(sprite->text, "RANK");
    }
}

void fn_1_F7D28(int mode, int flags)
{
    struct Sprite *sprite;

    fn_1_F1B20(mode, flags);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->x = 222.0f;
        sprite->y = 34.0f;
        sprite->textAlign = 0;
        sprite->fontId = 0x8f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->drawFunc = fn_1_F35F4;
        strcpy(sprite->text, "k/GORUHU");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->x = 316.0f;
        sprite->y = 37.0f;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->textAlign = 0;
        sprite->fontId = 0x8f;
        sprite->mulR = 0xEF;
        sprite->mulG = 0x9A;
        sprite->mulB = 0;
        sprite->drawFunc = fn_1_F35F4;
        strcpy(sprite->text, "a/~18k/HO-RU~");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->depth = 1.0f;
        sprite->mainFunc = fn_1_F1CF4;
        sprite->drawFunc = fn_1_F5C8C;
        strcpy(sprite->text, "RANK");
    }
}

void fn_1_F7E80(int mode, int flags)
{
    static char *lbl_1_data_62C04[3] = { "\x81\x60\x82\xB5\x82\xE5\x82\xAB\x82\xE3\x82\xA4\x81\x60", "\x81\x60\x82\xBF\x82\xE3\x82\xA4\x82\xAB\x82\xE3\x82\xA4\x81\x60", "\x81\x60\x82\xB6\x82\xE5\x82\xA4\x82\xAB\x82\xE3\x82\xA4\x81\x60" };
    struct Sprite *sprite;

    fn_1_F1B20(mode, flags);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->x = 160.0f;
        sprite->y = 34.0f;
        sprite->textAlign = 0;
        sprite->fontId = 0x3;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->drawFunc = fn_1_F35F4;
        strcpy(sprite->text, "\x83\x7B\x81\x5B\x83\x67");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->x = 358.0 - 24.0 * (0.5 * (float)(strlen(lbl_1_data_62C04[lbl_1_bss_D2B88.mode - 12]) >> 1));
        sprite->y = 37.0f;
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->textAlign = 0;
        sprite->fontId = 0x3;
        sprite->mulR = 0xEF;
        sprite->mulG = 0x9A;
        sprite->mulB = 0;
        sprite->drawFunc = fn_1_F35F4;
        strcpy(sprite->text, lbl_1_data_62C04[lbl_1_bss_D2B88.mode - 12]);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->depth = 1.0f;
        sprite->mainFunc = fn_1_F1CF4;
        sprite->drawFunc = fn_1_F6A9C;
        strcpy(sprite->text, "RANK");
    }
}

/* draws */

void fn_1_F8060(int mode, int flag)
{
    load_bmp_by_id(3);
    if (main_mode != 5)
        g_fade_track_volume(0x30, 0);
    destroy_sprite_with_unique_id(0x65);
    lbl_1_data_61E28[mode].init(mode, flag);
    fn_1_F9B04(lbl_1_bss_D2B88.tex, lbl_1_data_61E28[mode].bgPath, 0, 0x280, 0x1E0, 0xE);
    fn_1_F9CBC(lbl_1_bss_D2B88.tex);
}

void fn_1_F812C(int mode, int flag)
{
    char *path = lbl_1_data_61E28[lbl_1_bss_D2B88.mode].bgPath;

    destroy_sprite_with_unique_id(0x65);
    lbl_1_data_61E28[mode].init(mode, flag);
    if (strcmp(path, lbl_1_data_61E28[mode].bgPath) != 0)
    {
        fn_1_F9E1C(lbl_1_bss_D2B88.tex);
        fn_1_F9B04(lbl_1_bss_D2B88.tex, lbl_1_data_61E28[mode].bgPath, 0, 0x280, 0x1E0, 0xE);
        fn_1_F9CBC(lbl_1_bss_D2B88.tex);
    }
}

void fn_1_F8230(void)
{
    int i;

    lbl_1_bss_D2B88.flags = 0;
    for (i = 0; i < 4; i++)
        lbl_1_bss_D2B88.unkB[i] = -1;
    lbl_1_bss_D2B88.unk14 = 0;
    lbl_1_bss_D2B88.unk5 = -1;
}

void fn_1_F828C(void)
{
    destroy_sprite_with_unique_id(0x65);
    call_free_bmp_by_id(3);
    fn_1_F9E1C(lbl_1_bss_D2B88.tex);
}

void fn_1_F82C8(s8 *status, struct Sprite *sprite)
{
    if (sprite->userVar > 0)
        sprite->x = 544.0 + sprite->counter;
    else
        sprite->x = 46.0 - sprite->counter;
    sprite->counter = (s16)(30.0f * math_sin((g_font_char_var_3 << 10) + 0x4000));
}

void fn_1_F83A4(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->type = 1;
        sprite->bmpId = 0x32E;
        sprite->x = 46.0f;
        sprite->y = 24.0f;
        sprite->depth = 0.05f;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
        sprite->textAlign = 0;
        sprite->userVar = -1;
        sprite->mainFunc = fn_1_F82C8;
        sprintf(sprite->text, "work");
    }
}

void fn_1_F844C(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->type = 1;
        sprite->bmpId = 0x32E;
        sprite->flags |= 0x80000;
        sprite->x = 544.0f;
        sprite->y = 24.0f;
        sprite->depth = 0.05f;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
        sprite->textAlign = 0;
        sprite->fontId = 0x8E;
        sprite->userVar = 1;
        sprite->mainFunc = fn_1_F82C8;
        sprintf(sprite->text, "work");
    }
}

void fn_1_F8508(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0x65;
        sprite->type = 1;
        sprite->bmpId = 0x315;
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->depth = 10.0f;
        sprite->textAlign = 4;
        strcpy(sprite->text, "BG");
    }
}

static char *lbl_1_data_62C34[4] = { "BOY", "GAL", "KID", "GOR" };

void fn_1_F8588(struct RecTime *rec)
{
    rec->chara = (rand() & 0x7FFF) % 4;
    rec->min = (rand() & 0x7FFF) % 100;
    rec->sec = (rand() & 0x7FFF) % 60;
    rec->cs = (rand() & 0x7FFF) % 100;
    rec->unk7 = rand() & 3;
    printf("race: %02d'%02d\"%02d %s(%d)\n", rec->min, rec->sec, rec->cs, lbl_1_data_62C34[rec->chara], rec->unk7);
}

void fn_1_F86B0(struct RecScore *rec)
{
    rec->chara = (rand() & 0x7FFF) % 4;
    rec->score = (rand() & 0x7FFF) % 10000;
    rec->unk6 = rand() & 3;
    printf("target:%d %s\n", rec->score, lbl_1_data_62C34[rec->chara]);
}

void fn_1_F8768(struct RecBowl *rec)
{
    rec->chara = (rand() & 0x7FFF) % 4;
    rec->score = (rand() & 0x7FFF) % 300;
    printf("bowl_nml:%d %s\n", rec->score, lbl_1_data_62C34[rec->chara]);
}

void fn_1_F8810(struct RecBowl *rec)
{
    rec->chara = (rand() & 0x7FFF) % 4;
    rec->score = (rand() & 0x7FFF) % 100;
    printf("bowl_cha:%d %s\n", rec->score, lbl_1_data_62C34[rec->chara]);
}

void fn_1_F88B8(struct RecGolf *rec)
{
    rec->chara = (rand() & 0x7FFF) % 4;
    rec->a = (rand() & 0x7FFF) % 100;
    rec->b = (rand() & 0x7FFF) % 100;
    printf("golf_cha:%d %d %d %s\n", rec->a, rec->b, rec->a + rec->b, lbl_1_data_62C34[rec->chara]);
}

void fn_1_F89A0(struct RecTime *rec)
{
    rec->chara = (rand() & 0x7FFF) % 4;
    rec->min = (rand() & 0x7FFF) % 100;
    rec->sec = (rand() & 0x7FFF) % 60;
    rec->cs = (rand() & 0x7FFF) % 100;
    rec->unk7 = rand() & 3;
    printf("boat: %02d'%02d\"%02d %s(%d)\n", rec->min, rec->sec, rec->cs, lbl_1_data_62C34[rec->chara], rec->unk7);
}

#define REC_TIME(r) ((r)->sec * 100 + (r)->min * 6000 + (r)->cs)

int fn_1_F8AC8(int mode, struct RecTime *rec, int idx, int query)
{
    int j;
    int ret;
    int time;
    struct RecTime *table;
    int i;
    int other;

    ret = 0;
    time = REC_TIME(rec);
    table = lbl_1_data_61E28[mode].entries;
    for (i = 0; i < 5; i++)
    {
        other = REC_TIME(&table[i]);
        if (time < other)
        {
            if (query)
                return i;
            for (j = 4; j > i; j--)
                table[j] = table[j - 1];
            memset(rec, 0, 3);
            table[i] = *rec;
            lbl_1_bss_D2B88.unkB[idx] = (s8)i;
            for (j = idx - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D2B88.unkB[j] >= i)
                {
                    if (lbl_1_bss_D2B88.unkB[j] < 4)
                        lbl_1_bss_D2B88.unkB[j]++;
                    else
                        lbl_1_bss_D2B88.unkB[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (query)
        return -1;
    return ret;
}

#pragma auto_inline off
int fn_1_F8C9C(int mode, struct RecTime *recs, int count, int query)
{
    struct RecTime *best;
    int worst;
    int ret;
    int a;
    int b;
    int i;

    ret = 0;
    worst = 0;
    a = REC_TIME(recs);
    for (i = 1; i < count; i++)
    {
        b = REC_TIME(&recs[i]);
        if (a > b)
            worst = i;
    }
    best = lbl_1_data_621A4[mode];
    a = REC_TIME(&recs[worst]);
    b = REC_TIME(best);
    if (a < b)
    {
        if (query)
            return 5;
        memset(recs, 0, 3);
        *best = recs[0];
        if (lbl_1_bss_D2B88.unkB[worst] >= 0)
        {
            lbl_1_bss_D2B88.flags |= 2;
            lbl_1_bss_D2B88.unk5 = (s8)worst;
        }
        else
        {
            lbl_1_bss_D2B88.flags |= 4;
            lbl_1_bss_D2B88.unk5 = (s8)worst;
            lbl_1_bss_D2B88.unkB[worst] = 5;
        }
        ret = 1;
    }
    if (query)
        return -1;
    return ret;
}
#pragma auto_inline on

int fn_1_F8E98(int mode, struct RecScore *rec, int idx, int query)
{
    int i;
    struct RecScore *table;
    int j;
    int ret;

    ret = 0;
    table = lbl_1_data_61E28[mode].entries;
    for (i = 0; i < 5; i++)
    {
        if ((u16)rec->score > table[i].score)
        {
            if (query)
                return i;
            for (j = 4; j > i; j--)
                table[j] = table[j - 1];
            memset(rec, 0, 3);
            table[i] = *rec;
            lbl_1_bss_D2B88.unkB[idx] = (s8)i;
            for (j = idx - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D2B88.unkB[j] >= i)
                {
                    if (lbl_1_bss_D2B88.unkB[j] < 4)
                        lbl_1_bss_D2B88.unkB[j]++;
                    else
                        lbl_1_bss_D2B88.unkB[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (query)
        return -1;
    return ret;
}

int fn_1_F9030(int mode, struct RecBowl *rec, int idx, int query)
{
    int i;
    struct RecBowl *table;
    int j;
    int ret;

    ret = 0;
    table = lbl_1_data_61E28[mode].entries;
    for (i = 0; i < 5; i++)
    {
        if ((u16)rec->score > table[i].score)
        {
            if (query)
                return i;
            for (j = 4; j > i; j--)
                table[j] = table[j - 1];
            memset(rec, 0, 3);
            table[i] = *rec;
            lbl_1_bss_D2B88.unkB[idx] = (s8)i;
            for (j = idx - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D2B88.unkB[j] >= i)
                {
                    if (lbl_1_bss_D2B88.unkB[j] < 4)
                        lbl_1_bss_D2B88.unkB[j]++;
                    else
                        lbl_1_bss_D2B88.unkB[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (query)
        return -1;
    return ret;
}

int fn_1_F91C8(int mode, struct RecGolf *rec, int idx, int query)
{
    int ret;
    struct RecGolf *table;
    int i;
    int j;

    ret = 0;
    table = lbl_1_data_61E28[mode].entries;
    for (i = 0; i < 5; i++)
    {
        if (rec->a + rec->b < table[i].a + table[i].b)
        {
            if (query)
                return i;
            for (j = 4; j > i; j--)
                table[j] = table[j - 1];
            memset(rec, 0, 3);
            table[i] = *rec;
            lbl_1_bss_D2B88.unkB[idx] = (s8)i;
            for (j = idx - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D2B88.unkB[j] >= i)
                {
                    if (lbl_1_bss_D2B88.unkB[j] < 4)
                        lbl_1_bss_D2B88.unkB[j]++;
                    else
                        lbl_1_bss_D2B88.unkB[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (query)
        return -1;
    return ret;
}

int fn_1_F9374(int mode, struct RecTime *rec, int idx, int query)
{
    int j;
    int ret;
    struct RecTime *table;
    int time;
    int other;
    int i;

    ret = 0;
    time = REC_TIME(rec);
    table = lbl_1_data_61E28[mode].entries;
    for (i = 0; i < 5; i++)
    {
        other = REC_TIME(&table[i]);
        if (time < other)
        {
            if (query)
                return i;
            for (j = 4; j > i; j--)
                table[j] = table[j - 1];
            memset(rec, 0, 3);
            table[i] = *rec;
            lbl_1_bss_D2B88.unkB[idx] = (s8)i;
            for (j = idx - 1; j >= 0; j--)
            {
                if (lbl_1_bss_D2B88.unkB[j] >= i)
                {
                    if (lbl_1_bss_D2B88.unkB[j] < 4)
                        lbl_1_bss_D2B88.unkB[j]++;
                    else
                        lbl_1_bss_D2B88.unkB[j] = -1;
                }
            }
            ret = 1;
            break;
        }
    }
    if (query)
        return -1;
    return ret;
}

#pragma auto_inline off
int fn_1_F9548(int mode, struct RecTime *recs, int count, int query)
{
    struct RecTime *best;
    int worst;
    int ret;
    int a;
    int b;
    int i;

    ret = 0;
    worst = 0;
    a = REC_TIME(recs);
    for (i = 1; i < count; i++)
    {
        b = REC_TIME(&recs[i]);
        if (a > b)
            worst = i;
    }
    best = lbl_1_data_621BC[mode - 12];
    a = REC_TIME(&recs[worst]);
    b = REC_TIME(best);
    if (a < b)
    {
        if (query)
            return 5;
        memset(recs, 0, 3);
        *best = recs[0];
        if (lbl_1_bss_D2B88.unkB[worst] >= 0)
        {
            lbl_1_bss_D2B88.flags |= 2;
            lbl_1_bss_D2B88.unk5 = (s8)worst;
        }
        else
        {
            lbl_1_bss_D2B88.flags |= 4;
            lbl_1_bss_D2B88.unk5 = (s8)worst;
            lbl_1_bss_D2B88.unkB[worst] = 5;
        }
        ret = 1;
    }
    if (query)
        return -1;
    return ret;
}
#pragma auto_inline on

int fn_1_F9744(int mode, void *rec, int count)
{
    int ret;
    int i;

    fn_1_F8230();
    lbl_1_bss_D2B88.unk6 = (s8)count;
    ret = 0;
    for (i = 0; i < count; i++)
    {
        switch (mode)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            ret |= fn_1_F8AC8(mode, rec, i, 0);
            rec = (u8 *)rec + 8;
            break;
        case 6:
        case 7:
        case 8:
            ret |= fn_1_F8E98(mode, rec, i, 0);
            rec = (u8 *)rec + 8;
            break;
        case 9:
        case 10:
            ret |= fn_1_F9030(mode, rec, i, 0);
            rec = (u8 *)rec + 6;
            break;
        case 11:
            ret |= fn_1_F91C8(mode, rec, i, 0);
            rec = (u8 *)rec + 8;
            break;
        case 12:
        case 13:
        case 14:
            ret |= fn_1_F9374(mode, rec, i, 0);
            rec = (u8 *)rec + 8;
            break;
        }
    }
    for (i = 0; i < count; i++)
    {
        if (lbl_1_bss_D2B88.unkB[i] >= 0)
            lbl_1_bss_D2B88.unk14 |= 1 << lbl_1_bss_D2B88.unkB[i];
    }
    switch (mode)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        if (fn_1_F8C9C(mode, rec, count, 0) != 0)
        {
            lbl_1_bss_D2B88.unk14 |= 0x20;
            ret = 1;
        }
        break;
    case 12:
    case 13:
    case 14:
        if (fn_1_F9548(mode, rec, count, 0) != 0)
        {
            lbl_1_bss_D2B88.unk14 |= 0x20;
            ret = 1;
        }
        break;
    }
    return ret;
}
