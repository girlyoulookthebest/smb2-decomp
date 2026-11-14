/* Sprite management and text drawing (SMB1 sprite.c). */
#include "types.h"
#include "bitmap.h"
#include "va.h"
#include "dip.h"
#include "event.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "sprite.h"
#include "textdraw.h"

#pragma unsigned_char on

#define MAX_SPRITES 0x50
#define RGBA(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

struct SpriteListNode
{
    /*0x00*/ struct Sprite *sprite;
    /*0x04*/ struct SpriteListNode *prev;
    /*0x08*/ struct SpriteListNode *next;
}; /* size = 0xC */

struct SpriteListNodePool
{
    /*0x000*/ struct SpriteListNode nodes[0x52];
    /*0x3D8*/ u32 unk3D8;
}; /* size = 0x3DC */

/* Current screen fade. */
struct ScreenFadeInfo
{
    /*0x00*/ s32 type;
    /*0x04*/ u32 color;
    /*0x08*/ s32 timer;
    /*0x0C*/ s32 timerMax;
};

struct StringParseState
{
    /*0x00*/ u32 mode;
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;
    /*0x0C*/ float unkC;
};

struct GlyphDesc
{
    /*0x00*/ char *text;
    /*0x04*/ s32 unk4;
};

struct DoubleKanjiGlyph
{
    /*0x00*/ char *text;
    /*0x04*/ s16 unk4;
    /*0x06*/ s16 unk6;
};

extern char **bmp_tex_names[];
extern s32 g_smth_with_sprite_draw_reqs_widescreen;
extern float g_font_char_var_1;
extern float g_font_char_var_2;
extern u32 g_font_char_var_3;

extern s32 active_sprite_draw_req_count;
extern s32 has_last_widescreen_scale;
extern s16 last_widescreen_scale_pivot_x;

void OSReport(const char *fmt, ...);
void mtxa_from_translate_xyz(float x, float y, float z);
void g_draw_sprite_draw_request_unbuffered(struct SpriteDrawRequest *req);
void g_read_something_from_prev_GX_settings(u8 *compareEnable, int *func, u8 *updateEnable);
void GXSetTevColor(int id, GXColor color);
void GXLoadPosMtxImm(Mtx mtx, int id);
void fn_1_B7760(void);
void fn_1_B7828(void);
void fn_1_B7F68(void);
void fn_1_B7F98(void);
void fn_1_D5FB0(void);
int fn_1_B2608(int fontId, char *str);
float textdraw_get_pixel_width_of_string_as_f32(char *str);
void set_ui_widescreen_scale_mtx(int arg);
void reset_ui_widescreen_scale_mtx(void);
void queue_bmp_by_id_read(int id);
void load_bmp_by_id_child(int id);
void free_bmp_by_id(int id);
void draw_sprite(struct Sprite *sprite);
void draw_text_sprite(struct Sprite *sprite);
void draw_bmp_sprite(struct Sprite *sprite);
void draw_texture_sprite(struct Sprite *sprite);
void calc_sprite_bounds(struct Sprite *sprite, s32 *left, s32 *top, s32 *right, s32 *bottom);
int g_get_font_char_width(char *chr, int fontId, struct FontParams *font);
float g_get_font_char_width_scaling(char *chr, int fontId);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_flags(u32 flags);
void textdraw_print(char *str);
void textdraw_print_internal_func_1(char *str);
float textdraw_print_internal_func_2(struct TextDrawInfo *info, float a, int b, float c);
float textdraw_print_internal_func_3(struct TextDrawInfo *info, struct FontParams *font, int a, int b);
float textdraw_chara_load(char *str, int a, int b);
float textdraw_get_pixel_width_of_string(char *str);
float fn_1_B74CC(char *str);
float g_get_string_sprite_width_3(char *str);
float textdraw_get_pixel_width_of_string_as_f32_child(char *str);
float textdraw_get_pixel_width_of_string_child(char *str);
int g_smth_with_fonts_chara_load_wrapper(char *str);
int textdraw_get_line_count_of_string(char *str);
int textdraw_get_line_count_of_string_child(char *str);
void fn_1_B75F4(void);
void fn_1_B7650(void);
void GXInitTexObjWrapMode(GXTexObj *obj, int s, int t);
void GXLoadTexObj_cached(GXTexObj *obj, int id);
#ifndef __MWERKS__
/* PC port: PlayStation/Xbox button words (pc_port button_prompts.c) */
char *pc_button_prompts_text(char *str);
#endif

/* .bss */
u32 lbl_1_bss_9C7C8;
struct Sprite sprites[MAX_SPRITES];
struct SpriteListNodePool g_some_sprite_structs;
u32 g_some_sprite_width;
u32 g_some_sprite_height;
struct ScreenFadeInfo g_screenfade_flags;

const s32 lbl_1_rodata_8FD0[] =
{
    0x16, 0x14, 0x16, 0x14, 0x14, 0x14, 0x14, 0x14,
    0x4, 0x12, 0x12, 0x12, 0x16, 0x12, 0x14, 0x14,
    0x16, 0x14, 0x14, 0x14, 0x12, 0x14, 0x16, 0x16,
    0x16, 0x14, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    0x10, 0x10, 0x10, 0x10, 0x10, 0xc, 0x10, 0x10,
    0x4, 0xc, 0x10, 0x4, 0x14, 0x10, 0x10, 0x10,
    0x10, 0xc, 0x10, 0xe, 0x10, 0x10, 0x14, 0x10,
    0x10, 0x10, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    0x12, 0xe, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0xe, 0xe, 0x12, 0x12, -1, 0xa, 0xa, 0xe,
    0xe, -1, 0x12, 0xa, -1, 0x14, 0xa, 0xa,
    0xc, 0xc, 0xc, 0xc, 0xa, 0xa, 0x11, 0x11,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 0x12, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
};

const u8 lbl_1_rodata_9390[] =
{
    0xe, 0xe, 0x8, 0xf, 0xe, 0xe, 0xf, 0x8, 0xe, 0xe, 0xb, 0xf,
    0x8, 0xe, 0x8, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe,
    0xe, 0xe, 0xe, 0xe, 0xf, 0xf, 0xf, 0xe, 0xf, 0xf, 0xf, 0xf,
    0xf, 0xf, 0xf, 0xf, 0xf, 0x8, 0xe, 0xf, 0xe, 0x10, 0xf, 0x10,
    0xf, 0x10, 0xf, 0xf, 0xf, 0xf, 0xf, 0x10, 0xf, 0xf, 0xf, 0xf,
};

const s32 charGlyphTable[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 0x133, 0x13f, -1, -1, 0x14c, 0x14b, 0x13d,
    0x136, 0x137, 0x124, 0x122, 0x12d, 0x123, 0x12e, 0x135,
    0x118, 0x119, 0x11a, 0x11b, 0x11c, 0x11d, 0x11e, 0x11f,
    0x120, 0x121, 0x130, -1, 0x128, 0x126, 0x129, 0x132,
    0x14a, 0x0, 0x3c, 0x24, 0x28, 0x3, 0x39, 0x14,
    0x37, 0x1, 0x1f, 0xf, 0x55, 0x46, 0x32, 0x4,
    0x41, -1, 0x55, 0x19, 0x23, 0x2, 0x7, 0x5a,
    -2, 0x4b, 0x1e, 0x138, -1, 0x139, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, 0x134, -1,
};

const s32 charVowelTable[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 0x0, -1, -1, -1, 0x3, -1, -1,
    -1, 0x1, -1, -1, -1, -1, -1, 0x4,
    -1, -1, -1, -2, -1, 0x2, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
};

const s32 lbl_1_rodata_97CC[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 0x50, -1, -1, -1, 0xd, -1, -1,
    -1, -2, -1, -1, -1, -1, -1, 0x54,
    -1, -1, -1, -1, -1, 0x52, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
};

const s32 lbl_1_rodata_99CC[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 0x0, -1, -1, -1, -1, 0x0,
    0x0, -1, -1, 0x0, -1, 0x0, 0x0, -1,
    0x0, -1, 0x0, 0x0, 0x0, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
};

const s32 lbl_1_rodata_9BCC[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 0x151, -1, -1, 0x12c, -1, 0x125,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 0x13a, -1, 0x13b, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
};

/* .data */
u32 lbl_1_data_38528[] =
{
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};

struct GlyphDesc symbolGlyphs[] =
{
    {"MARU", 0x140},
    {"SANKAKU", 0x141},
    {"SANNKAKU", 0x141},
    {"SIKAKU", 0x142},
    {"HOSI", 0x143},
    {"SUPE-DO", 0x144},
    {"KURO-BA-", 0x145},
    {"HA-TO", 0x146},
    {"DAIYA", 0x147},
    {"NIJYU-MARU", 0x148},
    {"ONNPU", 0x149},
    {"ONPU", 0x149},
    {"ATTOMA-KU", 0x14a},
    {"ANDO", 0x14b},
    {"ANNDO", 0x14b},
    {"SANNKAKU_U", 0x14d},
    {"SANNKAKU_B", 0x14e},
    {"SANNKAKU_L", 0x14f},
    {"SANNKAKU_R", 0x150},
    {"!=", 0x127},
    {"LE", 0x12a},
    {"GE", 0x12b},
    {"TENNTENN", 0x131},
    {"!'", 0x13c},
    {"!\"", 0x13e},
    {"KAKERU", 0x151},
    {"KOME", 0x152},
    {"*", 0x152},
    {"MUGENN", 0x153},
    {".", 0x12f},
    {"DMY", -1},
};

struct GlyphDesc buttonGlyphs[] =
{
    {"BOTANN_A", 0x0},
    {"BUTTON_A", 0x0},
    {"BUTTON_B", 0x1},
    {"BUTTON_C", 0x2},
    {"BOTANN_L", 0x3},
    {"BUTTON_L", 0x3},
    {"BOTANN_R", 0x4},
    {"BUTTON_R", 0x4},
    {"BUTTON_X", 0x5},
    {"BUTTON_Y", 0x6},
    {"BUTTON_Z", 0x7},
    {"LEVER", 0x8},
    {"BUTTON_+", 0x9},
    {"DMY", -1},
};

struct GlyphDesc singleKanjiGlyphs[] =
{
    {"SHOkyuu", 0x168},
    {"CHUkyuu", 0x169},
    {"NAKA", 0x169},
    {"JYOkyuu", 0x16a},
    {"UE", 0x16a},
    {"KYUU", 0x16b},
    {"NANI", 0x16c},
    {"HONN", 0x16d},
    {"KAchi", 0x16e},
    {"MAke", 0x16f},
    {"SHIyou", 0x170},
    {"TSUKAu", 0x170},
    {"shiYOU", 0x171},
    {"YOU", 0x171},
    {"JIbunn", 0x172},
    {"jiBUNN", 0x173},
    {"HITO", 0x174},
    {"NINN", 0x174},
    {"GENzai", 0x175},
    {"genZAI", 0x176},
    {"KOsuu", 0x177},
    {"DOUji", 0x178},
    {"douJI", 0x179},
    {"TOKI", 0x179},
    {"TAIsenn", 0x17a},
    {"taiSENN", 0x17b},
    {"SENN", 0x17b},
    {"KAIsuu", 0x17c},
    {"KAI", 0x17c},
    {"TSUGI", 0x17d},
    {"RENNshuu", 0x17e},
    {"rennSHUU", 0x17f},
    {"KYOUsou", 0x180},
    {"kyouSOU", 0x181},
    {"SAKI", 0x182},
    {"HOKA", 0x183},
    {"KURAI", 0x184},
    {"KAZU", 0x185},
    {"SUU", 0x185},
    {"OSU", 0x186},
    {"HAI", 0x187},
    {"KEttei", 0x188},
    {"keTTEI", 0x189},
    {"Au", 0x18a},
    {"MIGI", 0x18b},
    {"SHITA", 0x18c},
    {"KUDA", 0x18c},
    {"HIDARI", 0x18d},
    {"TAberu", 0x18e},
    {"IMA", 0x18f},
    {"DOU", 0x190},
    {"UGOku", 0x190},
    {"DAI", 0x191},
    {"OOkii", 0x191},
    {"KIbunn", 0x192},
    {"HENN", 0x193},
    {"KAwaru", 0x193},
    {"CHIkei", 0x194},
    {"chiKEI", 0x195},
    {"chiTENN", 0x196},
    {"TENN", 0x196},
    {"SAKA", 0x197},
    {"MICHI", 0x198},
    {"Ochiru", 0x199},
    {"TOru", 0x19a},
    {"KIkann", 0x19b},
    {"MOUkeru", 0x19c},
    {"TUku", 0x19d},
    {"YOmu", 0x19e},
    {"KARADA", 0x19f},
    {"ATUKAu", 0x1a0},
    {"SETSU", 0x1a1},
    {"AKArui", 0x1a2},
    {"KAku", 0x1a3},
    {"DENN", 0x166},
    {"GENN", 0x167},
    {"NUku", 0x165},
    {"SOU", 0x164},
    {"TAMA", 0x163},
    {"DMY", -1},
};

struct DoubleKanjiGlyph doubleKanjiGlyphs[] =
{
    {"SHOKYUU", 0x168, 0x16b},
    {"CHUKYUU", 0x169, 0x16b},
    {"JYOKYUU", 0x16a, 0x16b},
    {"NANNBONN", 0x16c, 0x16d},
    {"SHOUBU", 0x16e, 0x16f},
    {"SYOUBU", 0x16e, 0x16f},
    {"SHIYOU", 0x170, 0x171},
    {"JIBUNN", 0x172, 0x173},
    {"GENZAI", 0x175, 0x176},
    {"DOUJI", 0x178, 0x179},
    {"TAISENN", 0x17a, 0x17b},
    {"RENNSHUU", 0x17e, 0x17f},
    {"KYOUSOU", 0x180, 0x181},
    {"HONNSUU", 0x16d, 0x185},
    {"KOSUU", 0x177, 0x185},
    {"KETTEI", 0x188, 0x189},
    {"CHIKEI", 0x194, 0x195},
    {"CHITENN", 0x194, 0x196},
    {"SAKAMICHI", 0x197, 0x198},
    {"RAKKA", 0x199, 0x18c},
    {"IRE", 0x187, 0x58},
    {"SENNSHU", 0x182, 0x19a},
    {"TORU", 0x19a, 0x57},
    {"TORE", 0x19a, 0x58},
    {"NANNKAI", 0x16c, 0x17c},
    {"KAISUU", 0x17c, 0x185},
    {"KATE", 0x16e, 0x26},
    {"SHOKI", 0x168, 0x19b},
    {"SETTEI", 0x19c, 0x189},
    {"CHAKUCHI", 0x19d, 0x194},
    {"HONNTAI", 0x16d, 0x19f},
    {"TORIATUKAI", 0x19a, 0x1a0},
    {"SETUMEI", 0x1a1, 0x1a2},
    {"DENNGENN", 0x166, 0x167},
    {"KACHI", 0x16e, 0x24},
    {"TAIKETSU", 0x17a, 0x188},
    {"TATAKAU", 0x17b, 0x2},
    {"TATAKAI", 0x17b, 0x1},
    {"SENNSOU", 0x17b, 0x181},
    {"NUKU", 0x165, 0x11},
    {"HASHIRU", 0x164, 0x57},
    {"KYOUSOU2", 0x180, 0x164},
    {"NUKI", 0x165, 0x10},
    {"TSUKAU", 0x170, 0x2},
    {"OTOSU", 0x199, 0x1b},
    {"OTO", 0x199, 0x27},
    {"TSUKAI", 0x170, 0x1},
    {"OCHI", 0x199, 0x24},
    {"KIME", 0x188, 0x49},
    {"JITENN", 0x179, 0x196},
    {"OSHI", 0x186, 0x1a},
    {"DOUTENN", 0x178, 0x196},
    {"HENNKEI", 0x193, 0x195},
    {"TSUKAE", 0x170, 0x3},
    {"SAYUU", 0x18d, 0x18b},
    {"KIMA", 0x188, 0x46},
    {"MAKE", 0x16f, 0x12},
    {"JYOUGE", 0x16a, 0x18c},
    {"DMY", -1, -1},
};

char *lbl_1_data_392A0[] =
{
    "ALIGN_LT",
    "ALIGN_LC",
    "ALIGN_LB",
    "ALIGN_CT",
    "ALIGN_CC",
    "ALIGN_CB",
    "ALIGN_RT",
    "ALIGN_RC",
    "ALIGN_RB",
    "ALIGN_KINTO",
};

void event_sprite_init(void)
{
    s8 *status = g_poolInfo.spritePool.statusList;
    int i;

    for (i = 0; i < g_poolInfo.spritePool.count; i++, status++)
        *status = 0;

    textdraw_reset();
    textdraw_pos_x.startX = 0.0f;
    textdraw_pos_x.x = 0.0f;
    textdraw_pos_x.y = 0.0f;
    fn_1_B7760();
}

void event_sprite_tick(void)
{
    struct Sprite *sprite;
    s8 *status;
    int i;

    if (!(g_some_other_flags & 0xA))
    {
        lbl_1_bss_9C7C8 = 0;
        sprite = sprites;
        status = g_poolInfo.spritePool.statusList;
        for (i = 0; i < g_poolInfo.spritePool.count; i++, sprite++, status++)
        {
            if (*status != 0)
            {
                lbl_1_bss_9C7C8 |= 1 << sprite->tag;
                if (sprite->mainFunc != NULL)
                    sprite->mainFunc(status, sprite);
            }
        }
    }
    if (!(g_some_other_flags & 2))
        fn_1_B7828();
}

void event_sprite_dest(void)
{
    struct Sprite *sprite;
    s8 *status;
    int i;

    sprite = sprites;
    status = g_poolInfo.spritePool.statusList;
    for (i = 0; i < 0x50; i++, sprite++, status++)
    {
        if (*status != 0)
        {
            if (sprite->destFunc != NULL)
                sprite->destFunc(sprite);
            *status = 0;
        }
    }

    g_poolInfo.spritePool.nextFree = 0;
    lbl_1_bss_9C7C8 = 0;
    fn_1_B7F68();
}

void g_smth_with_drawing_all_sprites(int a)
{
    struct Sprite *sprite;
    struct SpriteListNode *node;
    struct SpriteListNode *head;
    struct SpriteListNode *newNode;
    struct SpriteListNode *tail;
    s8 *status;
    int idx;
    int i;
    int viewStage = (events[EVENT_VIEW].state == (s8)EV_STATE_RUNNING);

    idx = 0;
    head = &g_some_sprite_structs.nodes[idx++];
    tail = &g_some_sprite_structs.nodes[idx++];
    head->sprite = NULL;
    head->prev = NULL;
    head->next = tail;
    tail->sprite = NULL;
    tail->prev = head;
    tail->next = NULL;

    status = g_poolInfo.spritePool.statusList;
    for (i = 0; i < g_poolInfo.spritePool.count; i++, status++)
    {
        if (*status == 0)
            continue;
        sprite = &sprites[i];
        if (viewStage && sprite->tag != 100)
            continue;
        if (a == 0)
        {
            if ((sprite->flags & (1 << 18)) == 0)
                continue;
        }
        else
        {
            if ((sprite->flags & (1 << 18)) != 0)
                continue;
        }
        if (sprite->prev != NULL)
            continue;

        node = head->next;
        while (node->sprite != NULL)
        {
            if (sprite->depth > node->sprite->depth)
                break;
            node = node->next;
        }
        newNode = &g_some_sprite_structs.nodes[idx++];
        newNode->sprite = sprite;
        newNode->prev = node->prev;
        newNode->next = node;
        node->prev->next = newNode;
        node->prev = newNode;
    }

    node = head->next;
    while (node->sprite != NULL)
    {
        sprite = node->sprite;
        textdraw_reset();
        draw_sprite(sprite);
        while (sprite->next != NULL)
        {
            draw_sprite(sprite->next);
            sprite = sprite->next;
        }
        node = node->next;
    }
    if (main_mode == 4)
        fn_1_D5FB0();
    fn_1_B7F98();
}

void fn_1_AE99C(struct Sprite *sprite)
{
    if (g_poolInfo.spritePool.statusList[sprite->poolIdx] != 0 && sprite->prev == NULL)
    {
        textdraw_reset();
        draw_sprite(sprite);
        while (sprite->next != NULL)
        {
            draw_sprite(sprite->next);
            sprite = sprite->next;
        }
    }
}

void draw_sprite(struct Sprite *sprite)
{
    u32 flags;

    if (sprite->unk8C & 1)
        return;
    calc_sprite_bounds(sprite, &sprite->left, &sprite->top, &sprite->right, &sprite->bottom);
    flags = sprite->flags;
    if (flags & 0x1000000)
    {
        set_ui_widescreen_scale_mtx(sprite->widescreenArg);
        sprite->flags &= ~0x1000000;
    }
    if (sprite->drawFunc != NULL)
    {
        sprite->drawFunc(sprite);
    }
    else
    {
        switch (sprite->type)
        {
        case SPRITE_TYPE_TEXT:
            draw_text_sprite(sprite);
            break;
        case SPRITE_TYPE_BITMAP:
            if (!bmp_infos[sprite->bmpId >> 8].isLoaded)
            {
                printf("SPRITE WARNING!! %s's category %s is not load\n",
                    bmp_tex_names[sprite->bmpId >> 8][sprite->bmpId & 0xFF],
                    bmp_infos[sprite->bmpId >> 8].name);
                break;
            }
            draw_bmp_sprite(sprite);
            break;
        case SPRITE_TYPE_TEXTURE:
            draw_texture_sprite(sprite);
            break;
        }
    }
    if (flags & 0x1000000)
    {
        reset_ui_widescreen_scale_mtx();
        sprite->flags |= 0x1000000;
    }
}

void fn_1_AEBA0(int id)
{
    queue_bmp_by_id_read(id);
}

void load_bmp_by_id(int id)
{
    load_bmp_by_id_child(id);
}

void call_free_bmp_by_id(int id)
{
    free_bmp_by_id(id);
}

void g_something_with_iteratively_freeing_memory(void)
{
    int i;

    for (i = 0; i < 25; i++)
    {
        if (i != 0)
            free_bmp_by_id(i);
    }
}

struct Sprite *create_sprite(void)
{
    struct Sprite *sprite;
    int index = pool_alloc(&g_poolInfo.spritePool, 2);

    if (index < 0)
        return NULL;
    else
    {
        sprite = &sprites[index];

        memset(sprite, 0, sizeof(*sprite));
        sprite->poolIdx = index;
        sprite->depth = 0.1f;
        sprite->mulR = 255;
        sprite->mulG = 255;
        sprite->mulB = 255;
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
        sprite->opacity = 1.0f;
        sprite->left = 0;
        sprite->top = 0;
        sprite->right = 1;
        sprite->bottom = 1;
        sprite->unk90 = 0.0f;
        sprite->unk94 = 0.0f;
        sprite->unk98 = 1.0f;
        sprite->unk9C = 1.0f;
        sprite->flags = 0x20000;
        return sprite;
    }
}

struct Sprite *fn_1_AED58(int tag)
{
    int i = g_poolInfo.spritePool.capacity - 1;
    struct Sprite *sprite = &sprites[i];
    s8 *status = g_poolInfo.spritePool.statusList + i;

    for (; i > 0; i--, sprite--, status--)
    {
        if (*status != 0 && sprite->tag == tag)
            break;
    }
    if (i == 0)
        return create_sprite();

    i++;
    sprite++;
    status++;
    for (; i < g_poolInfo.spritePool.capacity; i++, sprite++, status++)
    {
        if (*status == 0)
        {
            struct Sprite *newSprite;

            if (i + 1 > g_poolInfo.spritePool.count)
                g_poolInfo.spritePool.count = i + 1;
            g_poolInfo.spritePool.nextFree = i + 1;
            *status = 2;
            newSprite = &sprites[i];
            memset(newSprite, 0, sizeof(*newSprite));
            newSprite->poolIdx = i;
            newSprite->depth = 0.1f;
            newSprite->mulR = 255;
            newSprite->mulG = 255;
            newSprite->mulB = 255;
            newSprite->scaleX = 1.0f;
            newSprite->scaleY = 1.0f;
            newSprite->opacity = 1.0f;
            newSprite->left = 0;
            newSprite->top = 0;
            newSprite->right = 1;
            newSprite->bottom = 1;
            newSprite->unk90 = 0.0f;
            newSprite->unk94 = 0.0f;
            newSprite->unk98 = 1.0f;
            newSprite->unk9C = 1.0f;
            newSprite->flags = 0x20000;
            return newSprite;
        }
    }
    return NULL;
}

struct Sprite *create_linked_sprite(struct Sprite *sprite)
{
    struct Sprite *newSprite = create_sprite();

    if (newSprite != NULL)
    {
        sprite->next = newSprite;
        newSprite->prev = sprite;
        newSprite->flags |= sprite->flags & (1 << 18);
    }
    return newSprite;
}

void destroy_sprite_with_unique_id(int tag)
{
    struct Sprite *sprite = sprites;
    s8 *status = g_poolInfo.spritePool.statusList;
    int i;

    for (i = 0; i < 0x50; i++, sprite++, status++)
    {
        if (*status != 0 && sprite->tag == tag)
        {
            if (sprite->destFunc != NULL)
                sprite->destFunc(sprite);
            *status = 0;
        }
    }
}

void dest_all_sprites(void)
{
    struct Sprite *sprite = sprites;
    s8 *status = g_poolInfo.spritePool.statusList;
    int i;

    for (i = 0; i < 0x50; i++, sprite++, status++)
    {
        if (*status != 0)
        {
            if (sprite->destFunc != NULL)
                sprite->destFunc(sprite);
            *status = 0;
        }
    }
}

struct Sprite *get_sprite_with_unique_id(int tag)
{
    struct Sprite *sprite = sprites;
    s8 *status = g_poolInfo.spritePool.statusList;
    int i;

    for (i = 0; i < 0x50; i++, sprite++, status++)
    {
        if (*status != 0 && sprite->tag == tag)
            return sprite;
    }
    return NULL;
}

void calc_sprite_bounds(struct Sprite *sprite, s32 *left, s32 *top, s32 *right, s32 *bottom)
{
    int width = 0;
    int height = 0;
    struct FontParams *font;
    char *chr;
    int x;
    int y;
    int len;

    if (sprite->drawFunc != NULL)
    {
        *left   = sprite->x - 50.0f;
        *top    = sprite->y - 50.0f;
        *right  = 50.0f + sprite->x;
        *bottom = 50.0f + sprite->y;
        return;
    }

    font = &FONT_DEFINITIONS[sprite->fontId];
    switch (sprite->type)
    {
    case SPRITE_TYPE_TEXT:
        len = strlen(sprite->text);
        width = len * font->spaceWidth;
        height = font->lineHeight;
        if (sprite->fontId > 0x8C)
        {
            width = fn_1_B2608(sprite->fontId, sprite->text);
        }
        else if (sprite->fontId == 3)
        {
            textdraw_reset();
            textdraw_set_font(3);
            if (sprite->flags & 0x80000000)
                textdraw_set_flags(0x80000000);
            width = textdraw_get_pixel_width_of_string_as_f32(sprite->text);
        }
        else
        {
            chr = sprite->text;
            while (*chr != 0)
            {
                float ratio;

                width += g_get_font_char_width(chr, sprite->fontId, font) - font->spaceWidth;
                ratio = g_get_font_char_width_scaling(chr, sprite->fontId);
                if (ratio != 1.0)
                    width += ratio * font->spaceWidth - font->spaceWidth;
                chr++;
            }
        }
        break;
    case SPRITE_TYPE_BITMAP:
        if (!bmp_infos[sprite->bmpId >> 8].isLoaded)
        {
            printf("SPRITE WARNING!! %s's category %s is not load\n",
                bmp_tex_names[sprite->bmpId >> 8][sprite->bmpId & 0xFF],
                bmp_infos[sprite->bmpId >> 8].name);
            *left   = sprite->x - 50.0f;
            *top    = sprite->y - 50.0f;
            *right  = 50.0f + sprite->x;
            *bottom = 50.0f + sprite->y;
            return;
        }
        {
            struct TPLTextureHeader *tex = &bmp_infos[sprite->bmpId >> 8].tpl->texHeaders[sprite->bmpId & 0xFF];

            width = tex->width;
            height = tex->height;
        }
        break;
    case SPRITE_TYPE_TEXTURE:
        {
            struct SpriteTexture *tex = sprite->tex;

            width = tex->width;
            height = tex->height;
        }
        break;
    }

    width *= sprite->scaleX;
    height *= sprite->scaleY;
    if (sprite->prev == NULL)
    {
        x = sprite->x;
        y = sprite->y;
        switch (sprite->textAlign)
        {
        case ALIGN_CT:
        case ALIGN_CC:
        case ALIGN_CB:
            x -= width / 2;
            break;
        case ALIGN_RT:
        case ALIGN_RC:
        case ALIGN_RB:
            x -= width;
            break;
        }
        switch (sprite->textAlign)
        {
        case ALIGN_LT:
        case ALIGN_CT:
        case ALIGN_RT:
            break;
        case ALIGN_LC:
        case ALIGN_CC:
        case ALIGN_RC:
            y -= height / 2;
            break;
        case ALIGN_LB:
        case ALIGN_CB:
        case ALIGN_RB:
            y -= height;
            break;
        }
        *left   = x;
        *top    = y;
        *right  = x + width;
        *bottom = y + height;
    }
    else
    {
        x = sprite->prev->right;
        y = sprite->prev->top;
        switch (sprite->textAlign)
        {
        case ALIGN_LT:
        case ALIGN_CT:
        case ALIGN_RT:
            break;
        case ALIGN_LC:
        case ALIGN_CC:
        case ALIGN_RC:
            y = (sprite->prev->top + sprite->prev->bottom) / 2 - height / 2;
            break;
        case ALIGN_LB:
        case ALIGN_CB:
        case ALIGN_RB:
            y = sprite->prev->bottom - height;
            break;
        }
        x += sprite->x;
        y += sprite->y;
        *left   = x;
        *top    = y;
        *right  = x + width;
        *bottom = y + height;
    }
}

int g_get_font_char_width(char *chr, int fontId, struct FontParams *font)
{
    switch (fontId)
    {
    case 9:
        switch (*chr)
        {
        case ' ':
            return 18;
        case 'I':
        case '[':
            return 30;
        }
        break;
    case 0x4F:
    case 0x72:
        switch (*chr)
        {
        case ';':
            return 6;
        case '<':
            return 10;
        }
        break;
    case 0x53:
    case 0x80:
        if (*chr == '1')
            return 15;
        break;
    case 0x54:
    case 0x81:
        if (*chr == '1')
            return 15;
        break;
    case 0x49:
    case 0x78:
        switch (*chr)
        {
        case '1':
        case ':':
            return 19;
        }
        break;
    }
    return font->spaceWidth;
}

float g_get_font_char_width_scaling(char *chr, int fontId)
{
    switch (fontId)
    {
    case 9:
        switch (chr[0])
        {
        case 'O':
            switch (chr[1])
            {
            case 'A':
                return 0.95f;
            case 'V':
                return 0.9f;
            }
            break;
        case 'D':
            switch (chr[1])
            {
            case 'Y':
                return 0.9f;
            }
            break;
        }
        break;
    case 4:
        if ((chr[0] == 'A' && chr[1] == 'T')
         || (chr[0] == 'T' && chr[1] == 'A')
         || (chr[0] == 'A' && chr[1] == 'Y')
         || (chr[0] == 'Y' && chr[1] == 'A'))
            return 0.8125f;
        return lbl_1_rodata_9390[chr[0] - 0x20] / 16.0;
    case 0x4B:
    case 0x74:
        return 0.9f;
    case 0x4F:
    case 0x72:
        return 0.875f;
    case 0x53:
    case 0x80:
        if (chr[0] == '1')
            return 1.1875f;
        return 1.125f;
    case 0x56:
    case 0x7E:
        return 0.9f;
    case 0x57:
    case 0x84:
        return 0.8333333f;
    case 0x60:
        switch (chr[0])
        {
        case ':':
            return 0.5f;
        }
        break;
    case 0x54:
    case 0x81:
        if (chr[0] == '1')
            return 1.0625f;
        break;
    case 0x49:
    case 0x78:
        switch (chr[0])
        {
        case '1':
        case ':':
            return 1.0526316f;
        }
        break;
    }
    return 1.0f;
}

int fn_1_AFCA4(struct StringParseState *state, char *str, s32 *color, s32 *skip, s32 *special)
{
    s32 glyph;

    state->unk8 = 0;

    /* "h/" - switch to hiragana mode */
    if (str[0] == 'h' && str[1] == '/')
    {
        state->mode = (state->mode & 0xFFFF0000) | 1;
        *skip = 1;
        return -2;
    }

    /* "k/" - switch to katakana mode */
    if (str[0] == 'k' && str[1] == '/')
    {
        state->mode = (state->mode & 0xFFFF0000) | 2;
        *skip = 1;
        return -2;
    }

    /* "a/" - switch to ASCII mode */
    if (str[0] == 'a' && str[1] == '/')
    {
        state->mode = (state->mode & 0xFFFF0000) | 0;
        *skip = 1;
        return -2;
    }

    /* "p/" - switch to picture mode */
    if (str[0] == 'p' && str[1] == '/')
    {
        state->mode = (state->mode & 0xFFFF0000) | 3;
        *skip = 1;
        return -2;
    }

    /* "c/0xrrggbb/" - change text color */
    if (str[0] == 'c' && str[1] == '/')
    {
        int shift;

        str += 4;
        *skip += 4;
        shift = 20;
        *color = 0;
        while (*str != '/')
        {
            int digit;

            switch (*str)
            {
            case '0': digit = 0; break;
            case '1': digit = 1; break;
            case '2': digit = 2; break;
            case '3': digit = 3; break;
            case '4': digit = 4; break;
            case '5': digit = 5; break;
            case '6': digit = 6; break;
            case '7': digit = 7; break;
            case '8': digit = 8; break;
            case '9': digit = 9; break;
            case 'a': digit = 10; break;
            case 'b': digit = 11; break;
            case 'c': digit = 12; break;
            case 'd': digit = 13; break;
            case 'e': digit = 14; break;
            case 'f': digit = 15; break;
            default:
                digit = 0;
                break;
            }
            *color |= digit << shift;
            str++;
            shift -= 4;
            *skip += 1;
        }
        return -2;
    }

    /* "b/" "/b" - blinking text */
    if (str[0] == 'b' && str[1] == '/')
    {
        state->mode |= 0x10000;
        *skip = 1;
        return -2;
    }
    if (str[0] == '/' && str[1] == 'b')
    {
        state->mode &= ~0x10000;
        *skip = 1;
        return -2;
    }

    if (str[0] == 'f' && str[1] == 't' && str[2] == '/')
    {
        *special = 1;
        *skip = 2;
        return -2;
    }

    if (str[0] == 'f' && str[1] == 'p' && str[2] == '/')
    {
        *special = 2;
        *skip = 2;
        return -2;
    }

    state->mode &= ~0x20000;
    glyph = -1;

    /* ascii character */
    if ((u16)state->mode == 0)
    {
        if (str[0] >= 'A' && str[0] <= 'Z')
            glyph = str[0] + 0x87;
        else if (str[0] >= 'a' && str[0] <= 'z')
            glyph = str[0] + 0x8F;
        else if (str[0] >= '0' && str[0] <= '9')
            glyph = str[0] + 0xE8;
        else
            glyph = charGlyphTable[str[0]];
    }
    /* hiragana/katakana character */
    else if ((u16)state->mode == 1 || (u16)state->mode == 2)
    {
        glyph = charGlyphTable[str[0]];
        if (glyph != -1)
        {
            if (glyph >= 0xC8)
            {
                s32 alt = lbl_1_rodata_9BCC[str[0]];

                if (alt != -1)
                    glyph = alt;
            }
            else if (str[0] == 'X')
            {
                int vowel = charVowelTable[str[1]];

                if (vowel >= 0)
                {
                    glyph = vowel + 10;
                    state->unk8 = 1;
                }
                else if (str[1] == 'T')
                {
                    glyph = 0x2F;
                    if (str[2] == 'S')
                        state->unk8 = 3;
                    else
                        state->unk8 = 2;
                }
            }
            else if (glyph >= 15)
            {
                if (str[0] == str[1])
                {
                    if (str[0] == 'N')
                    {
                        glyph = 0x5F;
                        state->unk8 = 1;
                    }
                    else
                        glyph = 0x2F;
                }
                else if (state->unk4 != -1)
                {
                    glyph = lbl_1_rodata_97CC[str[1]];
                    state->unk4 = -1;
                    if (glyph == -2)
                    {
                        *skip = 1;
                        return -2;
                    }
                    state->unk8 = 1;
                }
                else
                {
                    s32 vowel = charVowelTable[str[1]];

                    if (vowel == -2)
                    {
                        vowel = charVowelTable[str[2]];
                        state->unk8 = 2;
                    }
                    if (vowel != -1)
                    {
                        if (str[0] == 'J' && str[1] == 'I')
                            state->unk8 = 1;
                        else if (str[0] == 'F' || str[0] == 'J')
                            state->unk4 = glyph;
                        else if (str[0] == 'C' && str[1] != 'H')
                        {
                            glyph = vowel + 15;
                            state->unk8 = 1;
                        }
                        else
                        {
                            glyph += vowel;
                            if (state->unk8 == 0)
                                state->unk8 = 1;
                        }
                    }
                    else
                    {
                        vowel = lbl_1_rodata_99CC[str[0]];
                        if (vowel != -1)
                            glyph++;
                        if (str[0] == 'N' && str[1] != 'Y')
                            glyph = 0x5F;
                        else if (str[0] == 'S' && str[1] == 'H' && str[2] == 'I')
                            state->unk8 = 2;
                        else
                            state->unk4 = glyph;
                    }
                }
            }
            else if (str[0] == 'V')
                state->unk4 = glyph;
            else if (state->unk4 != -1)
            {
                state->unk4 = -1;
                glyph += 10;
            }
            if (glyph < 100 && (u16)state->mode == 2)
                glyph += 100;
        }
    }
    /* icon or kanji */
    else if ((u16)state->mode == 3)
    {
        if (state->unk4 != -1)
        {
            glyph = state->unk4;
            state->unk4 = -1;
            state->unk8 = 0;
        }
        else
        {
            char *p;
            int j;
            int len;
            struct GlyphDesc *desc;

            glyph = -2;

            for (p = str, len = 0; *p != '/'; len++, p++)
                ;

            /* check symbol glyphs */
            desc = symbolGlyphs;
            while (desc->unk4 != -1)
            {
                for (p = str, j = 0; *p != '/'; j++, p++)
                {
                    if (*p != desc->text[j])
                        break;
                }
                if (len == j && len == (u32)strlen(desc->text))
                {
                    glyph = desc->unk4;
                    state->unk8 = j;
                    break;
                }
                desc++;
            }
            if (glyph == -2)
            {
                /* check button glyphs */
                desc = buttonGlyphs;
                while (desc->unk4 != -1)
                {
                    for (p = str, j = 0; *p != '/'; j++, p++)
                    {
                        if (*p != desc->text[j])
                            break;
                    }
                    if (len == j && len == (u32)strlen(desc->text))
                    {
                        glyph = desc->unk4;
                        state->unk8 = j;
                        state->mode |= 0x20000;
                        break;
                    }
                    desc++;
                }
            }
            if (glyph == -2)
            {
                /* check single kanji */
                desc = singleKanjiGlyphs;
                while (desc->unk4 != -1)
                {
                    for (p = str, j = 0; *p != '/'; j++, p++)
                    {
                        if (*p != desc->text[j])
                            break;
                    }
                    if (len == j && len == (u32)strlen(desc->text))
                    {
                        glyph = desc->unk4;
                        state->unk8 = j;
                        break;
                    }
                    desc++;
                }
            }
            if (glyph == -2)
            {
                /* check double kanji */
                struct DoubleKanjiGlyph *dbl = doubleKanjiGlyphs;

                while (dbl->unk4 != -1)
                {
                    for (p = str, j = 0; *p != '/'; j++, p++)
                    {
                        if (*p != dbl->text[j])
                            break;
                    }
                    if (len == j && len == (u32)strlen(dbl->text))
                    {
                        glyph = dbl->unk4;
                        state->unk8 = j - 1;
                        state->unk4 = dbl->unk6;
                        break;
                    }
                    dbl++;
                }
            }
        }
    }

    return glyph;
}

void textdraw_reset(void)
{
    textdraw_pos_x.fontId = 0;
    textdraw_pos_x.mulColor = 0xFFFFFF;
    textdraw_pos_x.addColor = 0;
    textdraw_pos_x.rotZ = 0;
    textdraw_pos_x.depth = 0.1f;
    textdraw_pos_x.scaleX = 1.0f;
    textdraw_pos_x.scaleY = 1.0f;
    textdraw_pos_x.alpha = 1.0f;
    textdraw_pos_x.flags = 0;
    textdraw_pos_x.alignment = 0;
    textdraw_pos_x.unk32 = 0;
    textdraw_pos_x.shadowX = 2;
    textdraw_pos_x.shadowY = 2;
    textdraw_pos_x.shadowAlpha = 0.75f;
    textdraw_pos_x.unk3C = 0;
    textdraw_pos_x.unk40 = 1.0f;
    textdraw_pos_x.unk44 = 0.0f;
    textdraw_pos_x.fontStyle = 0;
    textdraw_pos_x.spacingX = 0.0f;
    textdraw_pos_x.spacingY = 0.0f;
    textdraw_pos_x.counter = 0xFFFF;
    g_textdraw_unk8.unk0 = 0;
}

void textdraw_set_font(int fontId)
{
    if (fontId > 0x8C)
    {
        if (fontId == 0x8F)
            textdraw_set_flags(0x80000000);
        fontId = 3;
    }
    textdraw_pos_x.fontId = fontId;
}

void textdraw_set_mul_color(u32 color)
{
    textdraw_pos_x.mulColor = color & 0xFFFFFF;
}

void textdraw_set_add_color(u32 color)
{
    textdraw_pos_x.addColor = color;
}

void textdraw_set_rot_z(int rotZ)
{
    textdraw_pos_x.rotZ = rotZ;
}

void textdraw_set_depth(float depth)
{
    textdraw_pos_x.depth = depth;
}

void textdraw_set_scale(float x, float y)
{
    textdraw_pos_x.scaleX = x;
    textdraw_pos_x.scaleY = y;
}

void textdraw_set_alpha(float alpha)
{
    textdraw_pos_x.alpha = alpha;
}

void textdraw_set_flags(u32 flags)
{
    textdraw_pos_x.flags |= flags;
}

void textdraw_clear_flags(u32 flags)
{
    textdraw_pos_x.flags &= ~flags;
}

void textdraw_set_alignment(s8 alignment)
{
    textdraw_pos_x.alignment = alignment;
}

void textdraw_set_drop_shadow_with_params(s16 x, s16 y, float alpha)
{
    textdraw_pos_x.flags |= 0x40000000;
    textdraw_pos_x.flags &= ~0x20000000;
    textdraw_pos_x.shadowX = x;
    textdraw_pos_x.shadowY = y;
    textdraw_pos_x.shadowAlpha = alpha;
    textdraw_pos_x.unk3C = 0;
}

void textdraw_set_drop_shadow(void)
{
    textdraw_pos_x.flags |= 0x40000000;
    textdraw_pos_x.flags &= ~0x20000000;
}

void textdraw_clear_drop_shadow(void)
{
    textdraw_pos_x.flags &= ~0x40000000;
}

void textdraw_set_border(void)
{
    textdraw_pos_x.flags |= 0x20000000;
    textdraw_pos_x.flags &= ~0x40000000;
}

void textdraw_clear_border(void)
{
    textdraw_pos_x.flags &= ~0x20000000;
}

void g_textdraw_set_unk6(float val)
{
    textdraw_pos_x.unk40 = val;
}

void g_textdraw_set_unk7(u32 val)
{
    textdraw_pos_x.unk44 = val;
}

void textdraw_set_font_style(u8 style)
{
    textdraw_pos_x.fontStyle = style;
}

void textdraw_set_spacing(float x, float y)
{
    textdraw_pos_x.spacingX = x;
    textdraw_pos_x.spacingY = y;
}

void g_textdraw_set_counter(u16 counter)
{
    textdraw_pos_x.counter = counter;
}

void textdraw_set_pos_for_aram_font_test(float x, float y)
{
    struct FontParams *font = &FONT_DEFINITIONS[textdraw_pos_x.fontId];

    textdraw_pos_x.startX = textdraw_pos_x.x = font->unk34 * (x * font->spaceWidth);
    textdraw_pos_x.y = font->unk34 * (y * font->lineHeight);
}

void textdraw_set_pos(float x, float y)
{
    textdraw_pos_x.startX = textdraw_pos_x.x = x;
    textdraw_pos_x.y = y;
}

void textdraw_put_char(char chr)
{
    struct SpriteDrawRequest params;
    struct FontParams *font = &FONT_DEFINITIONS[textdraw_pos_x.fontId];
    int glyphIndex = chr - font->firstChar;
    int div = glyphIndex / font->charsPerRow;
    int mod = glyphIndex % font->charsPerRow;
    float f4;
    float f5;

    params.bmpId = font->bmpId;
    params.x = textdraw_pos_x.x + font->unk18 * font->unk20;
    params.y = textdraw_pos_x.y + font->unk1C * font->unk22;
    params.z = textdraw_pos_x.depth;
    f4 = font->unk10 * mod;
    f5 = font->unk14 * div;
    params.u1 = f4 + font->unk18 * font->unk20;
    params.v1 = f5 + font->unk1C * font->unk22;
    params.u2 = font->unk10 + (f4 - font->unk18 * font->unk21);
    params.v2 = font->unk14 + (f5 - font->unk1C * font->unk23);
    params.scaleX = textdraw_pos_x.scaleX * (font->unk10 - font->unk18 * font->unk20 - font->unk18 * font->unk21);
    params.scaleY = textdraw_pos_x.scaleY * (font->unk14 - font->unk1C * font->unk22 - font->unk18 * font->unk23);
    params.rotation = textdraw_pos_x.rotZ;
    params.opacity = textdraw_pos_x.alpha;
    params.unk30 = -1;
    params.flags = (textdraw_pos_x.flags & ~0xF) | 5;
    params.mulColor = ((u8)(255.0f * textdraw_pos_x.alpha) << 24) | textdraw_pos_x.mulColor;
    params.addColor = textdraw_pos_x.addColor;
    draw_sprite_draw_request(&params);
}

static inline int get_jpn_char_width(int chr, int fontId, struct FontParams *font)
{
    switch (fontId)
    {
    case 0x8F:
        switch (chr)
        {
        case ' ':
            return 12;
        }
    default:
        return font->spaceWidth;
    }
}

static inline float get_jpn_glyph_ratio(int glyph, int fontId)
{
    switch (fontId)
    {
    case 0x8F:
        if (glyph >= 0xC8 && lbl_1_rodata_8FD0[glyph - 0xC8] != -1)
        {
            return (float)(lbl_1_rodata_8FD0[glyph - 0xC8] + 1 < 24 ? lbl_1_rodata_8FD0[glyph - 0xC8] + 1 : 24)
                 / (float)FONT_DEFINITIONS[fontId].spaceWidth;
        }
    default:
        return 1.0f;
    }
}

static inline int get_font5_offset(char *str, int fontId)
{
    switch (fontId)
    {
    case 5:
        return 0.5f * (16 - lbl_1_rodata_9390[*str - 0x20]);
    default:
        return 0;
    }
}

void textdraw_print(char *str)
{
    struct TextDrawInfo *drawInfo = &textdraw_pos_x;
    struct FontParams *font = &FONT_DEFINITIONS[drawInfo->fontId];
    int div;
    int mod;
    int charWidth;
    int glyph;
    int count;
    int fontIdBackup;
    int lineLen;
    int lineCount;
    int newLine;
    float charPad;
    float ratio;
    float lineWidth;
    float extraSpacing;
    float spacingBackup;
    struct SpriteDrawRequest params;
    struct StringParseState parseState;
    s32 skip;
    s32 special;
    s32 color;

#ifndef __MWERKS__
    str = pc_button_prompts_text(str);
#endif
    if (font->bmpId == -1)
    {
        textdraw_print_internal_func_1(str);
        return;
    }

    fontIdBackup = drawInfo->fontId;
    params.bmpId = font->bmpId;
    params.z = drawInfo->depth;
    params.opacity = drawInfo->alpha;
    params.rotation = drawInfo->rotZ;
    params.mulColor = ((u8)(255.0f * textdraw_pos_x.alpha) << 24) | drawInfo->mulColor;
    params.addColor = drawInfo->addColor;
    params.unk30 = -1;
    params.flags = (drawInfo->flags & ~0xF) | 5;
    parseState.mode = 0;
    parseState.unk4 = -1;
    parseState.unk8 = 0;
    parseState.unkC = 0.0f;
    count = 0;
    newLine = 1;
    lineCount = textdraw_chara_load(str, 0, 3);
    spacingBackup = drawInfo->spacingX;

    for (; *str != 0; str++)
    {
        if (g_smth_with_sprite_draw_reqs_widescreen != 4 && -1.0f != g_font_char_var_1)
        {
            if (g_font_char_var_1 <= (float)(u16)count)
                break;
        }
        if ((u16)count >= drawInfo->counter)
            break;
        if (newLine != 0)
        {
            newLine = 0;
            lineWidth = (1.0 / font->unk34) * (drawInfo->scaleX * textdraw_chara_load(str, 1, 0));
            lineLen = textdraw_chara_load(str, 1, 1);
            if ((drawInfo->alignment & 0x10) && drawInfo->unk32 > lineWidth)
            {
                extraSpacing = (drawInfo->unk32 - lineWidth) / (float)((s16)lineLen - 1);
                drawInfo->spacingX = 0.0f;
            }
            else
                extraSpacing = 0.0f;
        }
        if (drawInfo->fontId > 0x8C)
            charWidth = get_jpn_char_width(*str, drawInfo->fontId, font);
        else
            charWidth = g_get_font_char_width(str, drawInfo->fontId, font);
        charPad = font->unk10 * (0.5 * (font->spaceWidth - charWidth) / font->spaceWidth);
        ratio = g_get_font_char_width_scaling(str, drawInfo->fontId);
        if (*str == '\n')
        {
            drawInfo->x = drawInfo->startX;
            drawInfo->y += drawInfo->spacingY + font->lineHeight * drawInfo->scaleY;
            newLine = 1;
            continue;
        }
        else if (*str == ' ' || *str < font->firstChar || *str > font->lastChar)
        {
            drawInfo->x += drawInfo->spacingX + ratio * (charWidth * drawInfo->scaleX);
            parseState.unkC += charWidth;
            count++;
            continue;
        }
        if (drawInfo->fontId > 0x8C)
        {
            skip = 0;
            special = 0;
            color = params.mulColor;
            glyph = fn_1_AFCA4(&parseState, str, &color, &skip, &special);
            str += skip;
            params.mulColor = color;
            if (special == 1)
            {
                drawInfo->fontId = 0x8E;
                font = &FONT_DEFINITIONS[drawInfo->fontId];
            }
            else if (special == 2)
            {
                drawInfo->fontId = 0x8F;
                font = &FONT_DEFINITIONS[drawInfo->fontId];
            }
            if (glyph == -1)
            {
                drawInfo->x += charWidth * drawInfo->scaleX;
                continue;
            }
            else if (glyph == -2)
                continue;
            ratio = get_jpn_glyph_ratio(glyph, drawInfo->fontId);
            if (parseState.mode & 0x20000)
                font = &FONT_DEFINITIONS[0x90];
            else
                font = &FONT_DEFINITIONS[drawInfo->fontId];
            params.bmpId = font->bmpId;
        }
        else
            glyph = *str;
        if (drawInfo->fontId < 0x8C
         || g_font_char_var_1 < g_font_char_var_2
         || !(parseState.mode & 0x10000)
         || (g_font_char_var_3 % 60) < 45)
        {
            float pad0 = font->unk20;
            float pad1 = font->unk21;
            float pad2 = font->unk22;
            float pad3 = font->unk23;
            float f0;
            float f1;
            int index;

            index = glyph - font->firstChar;
            div = index / font->charsPerRow;
            mod = index % font->charsPerRow;
            params.x = textdraw_print_internal_func_2(drawInfo, lineWidth, lineLen, extraSpacing);
            params.y = textdraw_print_internal_func_3(drawInfo, font, lineCount, 0);
            params.x += get_font5_offset(str, drawInfo->fontId);
            f0 = font->unk10 * mod;
            f1 = font->unk14 * div;
            params.u1 = charPad + (f0 + font->unk18 * pad0);
            params.v1 = f1 + font->unk1C * pad2;
            params.u2 = (font->unk10 + (f0 - font->unk18 * pad1)) - charPad;
            params.v2 = font->unk14 + (f1 - font->unk1C * pad3);
            params.scaleX = drawInfo->scaleX * (font->unk10 - 2.0 * charPad);
            params.scaleY = drawInfo->scaleY * font->unk14;
            draw_sprite_draw_request(&params);
        }
        drawInfo->x += extraSpacing + (drawInfo->spacingX + ratio * (charWidth * drawInfo->scaleX));
        str += parseState.unk8;
        parseState.unkC += charWidth;
        count++;
    }

    if (glyph != '\n')
        drawInfo->y = textdraw_print_internal_func_3(drawInfo, font, lineCount, 1);
    drawInfo->fontId = (s16)fontIdBackup;
    drawInfo->spacingX = spacingBackup;
}

void textdraw_printf(char *fmt, ...)
{
    va_list args;
    char buf[0x200];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    textdraw_print(buf);
}

void draw_text_sprite(struct Sprite *sprite)
{
    struct TextDrawInfo *info = &textdraw_pos_x;

    info->startX = sprite->left;
    info->x = sprite->left;
    info->y = sprite->top;
    info->fontId = sprite->fontId;
    info->mulColor = RGBA(sprite->mulR, sprite->mulG, sprite->mulB, (u8)(255.0f * sprite->opacity));
    info->addColor = RGBA(sprite->addR, sprite->addG, sprite->addB, 0);
    info->rotZ = sprite->rotation;
    info->depth = sprite->depth;
    info->scaleX = sprite->scaleX;
    info->scaleY = sprite->scaleY;
    info->alpha = sprite->opacity;
    info->flags = sprite->flags | 0x800000;
    info->alignment = sprite->textAlign;
    info->unk32 = 0;
    info->shadowX = sprite->shadowX;
    info->shadowY = sprite->shadowY;
    info->shadowAlpha = sprite->shadowAlpha;
    info->unk3C = sprite->unk1C;
    info->unk40 = sprite->unk20;
    info->unk44 = sprite->unk24;
    info->fontStyle = sprite->fontStyle;
    info->spacingX = sprite->spacingX;
    info->spacingY = sprite->spacingY;
    info->counter = 0xFFFF;
    textdraw_print(sprite->text);
}

void draw_bmp_sprite(struct Sprite *sprite)
{
    struct SpriteDrawRequest params;

    params.bmpId = sprite->bmpId;
    params.x = (sprite->left + sprite->right) / 2;
    params.y = (sprite->top + sprite->bottom) / 2;
    params.z = sprite->depth;
    params.scaleX = sprite->scaleX;
    params.scaleY = sprite->scaleY;
    params.u1 = sprite->unk90;
    params.v1 = sprite->unk94;
    params.u2 = sprite->unk98;
    params.v2 = sprite->unk9C;
    params.rotation = sprite->rotation;
    params.opacity = sprite->opacity;
    params.unk30 = -1;
    params.flags = (sprite->flags & ~0xF) | 10;
    params.mulColor = RGBA(sprite->mulR, sprite->mulG, sprite->mulB, (u8)(255.0f * sprite->opacity));
    params.addColor = RGBA(sprite->addR, sprite->addG, sprite->addB, 0);
    draw_sprite_draw_request(&params);
}

void draw_texture_sprite(struct Sprite *sprite)
{
    struct SpriteDrawRequest params;
    struct SpriteTexture *tex = sprite->tex;

    if (tex->type == 3)
    {
        params.bmpId = 0x1A00;
        params.x = (sprite->left + sprite->right) / 2;
        params.y = (sprite->top + sprite->bottom) / 2;
        params.z = sprite->depth;
        params.scaleX = sprite->scaleX;
        params.scaleY = sprite->scaleY;
        params.u1 = sprite->unk90;
        params.v1 = sprite->unk94;
        params.u2 = sprite->unk98;
        params.v2 = sprite->unk9C;
        params.rotation = sprite->rotation;
        params.opacity = sprite->opacity;
        params.unk30 = -1;
        params.flags = (sprite->flags & ~0xF) | 10;
        params.mulColor = RGBA(sprite->mulR, sprite->mulG, sprite->mulB, (u8)(255.0f * sprite->opacity));
        params.addColor = RGBA(sprite->addR, sprite->addG, sprite->addB, 0);
        g_some_sprite_width = tex->width;
        g_some_sprite_height = tex->height;
        GXLoadTexObj_cached(&tex->texObj, 0);
        draw_sprite_draw_request(&params);
    }
}

void textdraw_print_and_fit_to_width(float maxWidth, char *str)
{
    float scaleBackup = textdraw_pos_x.scaleX;
    float width = textdraw_get_pixel_width_of_string(str);

    if (width > maxWidth)
        textdraw_pos_x.scaleX *= maxWidth / width;
    textdraw_print(str);
    textdraw_pos_x.scaleX = scaleBackup;
}

void textdraw_printf_and_fit_to_width(float maxWidth, char *fmt, ...)
{
    va_list args;
    char buf[0x200];
    float scaleBackup;
    float width;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    scaleBackup = textdraw_pos_x.scaleX;
    width = textdraw_get_pixel_width_of_string(buf);
    if (width > maxWidth)
        textdraw_pos_x.scaleX *= maxWidth / width;
    textdraw_print(buf);
    textdraw_pos_x.scaleX = scaleBackup;
}

int g_get_font_def_aram_flag(int fontId)
{
    return FONT_DEFINITIONS[fontId].bmpId;
}

float g_get_string_sprite_width_2(char *str)
{
    return fn_1_B74CC(str);
}

float g_call_get_string_sprite_width_3_discard_result(char *str)
{
    return g_get_string_sprite_width_3(str);
}

float textdraw_get_pixel_width_of_string_as_f32(char *str)
{
#ifndef __MWERKS__
    str = pc_button_prompts_text(str);
#endif
    return textdraw_get_pixel_width_of_string_as_f32_child(str);
}

float textdraw_get_pixel_width_of_string(char *str)
{
#ifndef __MWERKS__
    str = pc_button_prompts_text(str);
#endif
    return textdraw_get_pixel_width_of_string_child(str);
}

float textdraw_get_pixel_height_of_string(char *str)
{
    float lines = textdraw_get_line_count_of_string(str);

    return lines * FONT_DEFINITIONS[textdraw_pos_x.fontId].lineHeight + (lines - 1.0) * textdraw_pos_x.spacingY;
}

void fn_1_B2250(void)
{
    fn_1_B75F4();
}

void fn_1_B2270(void)
{
    fn_1_B7650();
}

float fn_1_B2290(int index)
{
    switch (index)
    {
    case 0:
        return g_textdraw_unk8.unk4;
    case 1:
        return g_textdraw_unk8.unk8;
    case 2:
        return g_textdraw_unk8.unkC;
    case 3:
        return g_textdraw_unk8.unk10;
    default:
        return 0.0f;
    }
}

int g_smth_with_fonts_chara_load_wrapper_discard_result(char *str)
{
    return g_smth_with_fonts_chara_load_wrapper(str);
}

int textdraw_get_line_count_of_string(char *str)
{
    return textdraw_get_line_count_of_string_child(str);
}

float fn_1_B2348(int fontId, char *str, int mode)
{
    struct StringParseState parseState;
    s32 skip;
    s32 special;
    s32 color;
    int count = 0;
    struct FontParams *font;

    parseState.mode = 0;
    parseState.unk4 = -1;
    parseState.unk8 = 0;
    parseState.unkC = 0.0f;
    font = &FONT_DEFINITIONS[fontId];

    for (; *str != 0; str++)
    {
        int glyph;

        if (*str == ' ')
        {
            parseState.unkC += get_jpn_char_width(*str, fontId, font);
            count++;
            continue;
        }
        skip = 0;
        color = 0;
        special = 0;
        glyph = fn_1_AFCA4(&parseState, str, &color, &skip, &special);
        if (special == 1)
        {
            fontId = 0x8E;
            font = &FONT_DEFINITIONS[fontId];
        }
        else if (special == 2)
        {
            fontId = 0x8F;
            font = &FONT_DEFINITIONS[fontId];
        }
        str += skip;
        if (glyph == -1 || glyph == -2)
            continue;
        parseState.unkC += font->spaceWidth * get_jpn_glyph_ratio(glyph, fontId);
        str += parseState.unk8;
        count++;
    }
    if (mode == 0)
        return count;
    else
        return parseState.unkC;
}

float fn_1_B25DC(char *str)
{
    return fn_1_B2348(0, str, 0);
}

int fn_1_B2608(int fontId, char *str)
{
    return fn_1_B2348(fontId, str, 1);
}

float fn_1_B2638(char *str)
{
    struct FontParams *font = &FONT_DEFINITIONS[9];
    float width = 0.0f;

    for (; *str != 0; str++)
    {
        if (*str == ' ')
            width += g_get_font_char_width(str, 9, font);
        else
            width += font->spaceWidth * g_get_font_char_width_scaling(str, 9);
    }
    return width;
}

void g_smth_with_screen_fading(void)
{
    struct SpriteDrawRequest params;

    if (g_screenfade_flags.timer == 0)
        return;
    if (main_mode == 5)
    {
        if ((g_screenfade_flags.type & 0xFF) != 2)
            g_screenfade_flags.timer--;
    }
    else
    {
        if (!(g_some_other_flags & 0xA) && (g_screenfade_flags.type & 0xFF) != 2)
            g_screenfade_flags.timer--;
    }
    switch (g_screenfade_flags.type & 0xFF)
    {
    case 0:
        params.opacity = (float)g_screenfade_flags.timer / (float)g_screenfade_flags.timerMax;
        break;
    case 1:
        params.opacity = 1.0 - (float)g_screenfade_flags.timer / (float)g_screenfade_flags.timerMax;
        break;
    case 2:
        params.opacity = 1.0f;
        break;
    }
    params.bmpId = 0x4B;
    params.x = 320.0f;
    params.y = 240.1f;
    params.z = (g_screenfade_flags.type & 0x100) ? 0.009 : 0.25;
    params.scaleX = 128.0f;
    params.scaleY = 60.0f;
    params.u1 = 0.0f;
    params.v1 = 0.0f;
    params.u2 = 1.0f;
    params.v2 = 1.0f;
    params.rotation = 0;
    params.unk30 = -1;
    params.flags = 0x2000A;
    params.mulColor = g_screenfade_flags.color;
    params.addColor = RGBA(0, 0, 0, 0);
    draw_sprite_draw_request(&params);
    if ((g_screenfade_flags.type & 0xFF) == 1 && g_screenfade_flags.timer == 0)
    {
        g_screenfade_flags.type = (g_screenfade_flags.type & 0x100) ? 0x102 : 2;
        g_screenfade_flags.timer = 1;
        g_screenfade_flags.timerMax = 0;
    }
}

void fade_screen_to_color(s32 type, u32 color, int duration)
{
    if ((type & 0xFF) != 2 && duration > 3)
        duration -= 3;

    if ((g_screenfade_flags.type & 0xFF) == (type & 0xFF)
     && (g_screenfade_flags.type & 0xFF) != 2
     && g_screenfade_flags.timer > 0)
        return;

    if ((type & 0xFF) == 1 && (g_screenfade_flags.type & 0xFF) == 0 && g_screenfade_flags.timer > 0)
    {
        g_screenfade_flags.type = (g_screenfade_flags.type & 0xFF00) | 1;
        g_screenfade_flags.timer = duration - g_screenfade_flags.timer * ((float)duration / (float)g_screenfade_flags.timerMax);
        g_screenfade_flags.timerMax = duration;
    }
    else if ((type & 0xFF) == 1 && (g_screenfade_flags.type & 0xFF) == 1 && g_screenfade_flags.timer > 0)
    {
        g_screenfade_flags.type = (g_screenfade_flags.type & 0xFF00) | 1;
        g_screenfade_flags.timer = duration - g_screenfade_flags.timer * ((float)duration / (float)g_screenfade_flags.timerMax);
        g_screenfade_flags.timerMax = duration;
    }
    else if ((type & 0xFF) == 0 && (g_screenfade_flags.type & 0xFF) == 0 && g_screenfade_flags.timer > 0)
    {
        g_screenfade_flags.type = (g_screenfade_flags.type & 0xFF00) | 0;
        g_screenfade_flags.timer = duration - g_screenfade_flags.timer * ((float)duration / (float)g_screenfade_flags.timerMax);
        g_screenfade_flags.timerMax = duration;
    }
    else if ((type & 0xFF) == 0 && (g_screenfade_flags.type & 0xFF) == 1 && g_screenfade_flags.timer > 0)
    {
        g_screenfade_flags.type = (g_screenfade_flags.type & 0xFF00) | 0;
        g_screenfade_flags.timer = duration - g_screenfade_flags.timer * ((float)duration / (float)g_screenfade_flags.timerMax);
        g_screenfade_flags.timerMax = duration;
    }
    else
    {
        g_screenfade_flags.type = type;
        if ((type & 0xFF) != 0)
            g_screenfade_flags.color = color;
        g_screenfade_flags.timer = duration;
        g_screenfade_flags.timerMax = duration;
    }
}

int fn_1_B2FB0(void)
{
    int ret;

    if (g_screenfade_flags.timer != 0)
        ret = 0;
    else
        ret = 1;
    return ret;
}

int fn_1_B2FD4(void)
{
    if (g_screenfade_flags.timer == 0)
        return -1;
    else
        return (s8)(g_screenfade_flags.type & 0xFF);
}

static inline int append_sprite_draw_req(struct SpriteDrawRequest *req)
{
    if (active_sprite_draw_req_count == 0x280)
    {
        if (!(req->flags & (1 << 22)))
            OSReport("nlSprPut : SPRITE BUFFER OVER !! %s\n", bmp_tex_names[req->bmpId >> 8][req->bmpId & 0xFF]);
        else
            OSReport("nlSprPut : SPRITE BUFFER OVER !! ARAM FONT %d\n", req->bmpId);
        return 0;
    }

    memcpy_handler(&sprite_draw_req_buffer[active_sprite_draw_req_count], req, sizeof(struct SpriteDrawRequest));
    if (has_last_widescreen_scale != 0)
    {
        sprite_draw_req_buffer[active_sprite_draw_req_count].flags |= (1 << 24);
        sprite_draw_req_buffer[active_sprite_draw_req_count].unk40 = last_widescreen_scale_pivot_x;
    }
    active_sprite_draw_req_count++;
    return 1;
}

static inline int add_sprite_draw_req(struct SpriteDrawRequest *req)
{
    struct Sprite *sprite;

    switch (g_smth_with_sprite_draw_reqs_widescreen)
    {
    case 0:
        if (append_sprite_draw_req(req))
            return 1;
        else
            return 1;
        break;
    case 1:
        if (!(req->flags & (1 << 18))
         && g_screenfade_flags.timer > 0
         && ((g_screenfade_flags.type & 0x100) ? 0.009 : 0.25) > req->z
         && append_sprite_draw_req(req))
            return 1;
        sprite = get_sprite_with_unique_id(4);
        if (sprite != NULL && sprite->unk58 >= req->z
         && get_sprite_with_unique_id(10) == NULL
         && append_sprite_draw_req(req))
            return 1;
        break;
    }
    return 0;
}

int draw_sprite_draw_request(struct SpriteDrawRequest *req)
{
    Vec topLeft = {0};
    Vec topRight = {0};
    Vec bottomLeft = {0};
    Vec bottomRight = {0};
    int zFunc;
    GXColor color;
    u8 zCompare;
    u8 zUpdate;
    float z;
    float u1;
    float u2;
    float v1;
    float v2;
    float x;
    float y;
    float texWidth;
    float texHeight;
    float zero1;
    float zero2;
    int group;
    int width;
    int height;
    struct TPLTextureHeader *tex;

    if (add_sprite_draw_req(req))
        return 0;

    if (req->flags & (1 << 22))
    {
        g_draw_sprite_draw_request_unbuffered(req);
        return 0;
    }

    group = req->bmpId >> 8;
    if (group != 0x1A)
    {
        if (!bmp_infos[group].isLoaded)
        {
            printf("SPRITE WARNING!! bmp_%s's category %s is not load\n",
                bmp_tex_names[group][req->bmpId & 0xFF],
                bmp_infos[group].name);
            return 0;
        }
        tex = &bmp_infos[req->bmpId >> 8].tpl->texHeaders[req->bmpId & 0xFF];
        width = tex->width;
        height = tex->height;
    }
    else
    {
        width = g_some_sprite_width;
        height = g_some_sprite_height;
    }

    if (req->flags & (1 << 24))
        set_ui_widescreen_scale_mtx(req->unk40);

    x = req->x;
    y = req->y;
    z = -req->z;
    texWidth = width;
    texHeight = height;
    u1 = req->u1;
    u2 = req->u2;
    v1 = req->v1;
    v2 = req->v2;

    switch (req->flags & 3)
    {
    default:
    case 1:
        topLeft.x = bottomLeft.x = 0.0f;
        topRight.x = bottomRight.x = texWidth;
        break;
    case 2:
        topLeft.x = bottomLeft.x = -texWidth / 2.0;
        topRight.x = bottomRight.x = texWidth / 2.0;
        break;
    case 3:
        topLeft.x = bottomLeft.x = -texWidth;
        topRight.x = bottomRight.x = 0.0f;
        break;
    }

    switch ((req->flags & 0xC) >> 2)
    {
    default:
    case 1:
        topLeft.y = topRight.y = 0.0f;
        bottomLeft.y = bottomRight.y = texHeight;
        break;
    case 2:
        topLeft.y = topRight.y = -texHeight / 2.0;
        bottomLeft.y = bottomRight.y = texHeight / 2.0;
        break;
    case 3:
        topLeft.y = topRight.y = -texHeight;
        bottomLeft.y = bottomRight.y = 0.0f;
        break;
    }

    mtxa_from_translate_xyz(x, y, 0.0f);
    mtxa_rotate_z((s16)-req->rotation);
    mtxa_scale_xyz(req->scaleX, req->scaleY, 1.0f);
    mtxa_tf_point(&topLeft, &topLeft);
    mtxa_tf_point(&topRight, &topRight);
    mtxa_tf_point(&bottomLeft, &bottomLeft);
    mtxa_tf_point(&bottomRight, &bottomRight);

    if (req->flags & (1 << 18))
    {
        float f0 = 0.002405626 * req->z;

        topLeft.x = (topLeft.x - 320.0f) * f0;
        topLeft.y = -((topLeft.y - 240.0f) * f0);
        topRight.x = (topRight.x - 320.0f) * f0;
        topRight.y = -((topRight.y - 240.0f) * f0);
        bottomLeft.x = (bottomLeft.x - 320.0f) * f0;
        bottomLeft.y = -((bottomLeft.y - 240.0f) * f0);
        bottomRight.x = (bottomRight.x - 320.0f) * f0;
        bottomRight.y = -((bottomRight.y - 240.0f) * f0);
    }

    if (req->flags & (1 << 19))
    {
        u1 = req->u2;
        u2 = req->u1;
    }
    if (req->flags & (1 << 20))
    {
        v1 = req->v2;
        v2 = req->v1;
    }

    if (group != 0x1A)
        GXLoadTexObj_cached(&bmp_infos[req->bmpId >> 8].tpl->texObjs[req->bmpId & 0xFF], 0);

    color.r = (req->mulColor & 0xFF0000) >> 16;
    color.g = (req->mulColor & 0xFF00) >> 8;
    color.b = req->mulColor & 0xFF;
    color.a = 255.0f * req->opacity;
    GXSetTevColor(1, color);
    color.r = (req->addColor & 0xFF0000) >> 16;
    color.g = (req->addColor & 0xFF00) >> 8;
    color.b = req->addColor & 0xFF;
    color.a = (req->addColor & 0xFF000000) >> 24;
    GXSetTevColor(2, color);

    if (req->flags & (1 << 21))
    {
        g_read_something_from_prev_GX_settings(&zCompare, &zFunc, &zUpdate);
        GXSetZMode_cached(1, 7, 1);
    }

    GXBegin(0x80, 7, 4);
    zero1 = 0.0f;
    zero2 = 0.0f;
    GXPosition3f32(topLeft.x, topLeft.y, z);
    GXTexCoord2f32(u1 + zero1, v1 + zero2);
    GXPosition3f32(topRight.x, topRight.y, z);
    GXTexCoord2f32(u2 - zero1, v1 + zero2);
    GXPosition3f32(bottomRight.x, bottomRight.y, z);
    GXTexCoord2f32(u2 - zero1, v2 - zero2);
    GXPosition3f32(bottomLeft.x, bottomLeft.y, z);
    GXTexCoord2f32(u1 + zero1, v2 - zero2);

    if (req->flags & (1 << 21))
        GXSetZMode_cached(zCompare, zFunc, zUpdate);
    if (req->flags & (1 << 24))
        reset_ui_widescreen_scale_mtx();
    return 0;
}

void fn_1_B3C14(u32 bmpId, int s, int t)
{
    u32 group = (bmpId & 0xFF00) >> 8;
    u32 index = bmpId & 0xFF;

    GXInitTexObjWrapMode(&bmp_infos[group].tpl->texObjs[index], s, t);
}

void set_ui_widescreen_scale_mtx(int pivot)
{
    if (modeCtrl.wideMode != 0)
    {
        if (has_last_widescreen_scale == 0 || last_widescreen_scale_pivot_x != pivot)
        {
            if (g_smth_with_sprite_draw_reqs_widescreen != 0)
            {
                mtxa_from_identity();
                mtxa_translate_xyz(pivot, 0.0f, 0.0f);
                mtxa_scale_xyz(0.75f, 1.0f, 1.0f);
                mtxa_translate_xyz(-pivot, 0.0f, 0.0f);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
            }
            has_last_widescreen_scale = 1;
            last_widescreen_scale_pivot_x = pivot;
        }
    }
}

void reset_ui_widescreen_scale_mtx(void)
{
    if (modeCtrl.wideMode != 0)
    {
        if (has_last_widescreen_scale != 0)
        {
            if (g_smth_with_sprite_draw_reqs_widescreen != 0)
            {
                mtxa_from_identity();
                GXLoadPosMtxImm(mtxa->mtxA, 0);
            }
            has_last_widescreen_scale = 0;
            last_widescreen_scale_pivot_x = 0;
        }
    }
}
