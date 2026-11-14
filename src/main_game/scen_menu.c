/* Story mode name entry and data select (load game) menus. */
#include "types.h"
#include "bitmap.h"
#include "gx.h"
#include "input.h"
#include "libc.h"
#include "mathutil.h"
#include "sprite.h"

extern s32 locale;

void GXSetZMode_cached(u8 compareEnable, int func, u8 updateEnable);
void set_ui_widescreen_scale_mtx(int w);
void reset_ui_widescreen_scale_mtx(void);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_alignment(int alignment);
void textdraw_set_mul_color(u32 color);
void textdraw_set_add_color(u32 color);
void textdraw_set_drop_shadow(void);
void textdraw_set_spacing(float x, float y);
void textdraw_clear_drop_shadow(void);
void textdraw_set_drop_shadow_with_params(int x, int y, float a);
void textdraw_set_font_style(int style);
void textdraw_set_pos(float x, float y);
void textdraw_print(char *text);
void textdraw_printf(char *format, ...);
void textdraw_set_flags(u32 flags);
void textdraw_set_scale(float x, float y);
void textdraw_set_alpha(float alpha);
void textdraw_set_border(void);
void fade_screen_to_color(int flags, u32 color, int len);
void call_SoundReqID_arg_0(int soundId);
void destroy_sprite_with_unique_id(int id);
#ifdef __MWERKS__
unsigned long strlen(const char *s);
#endif
int g_get_bmp_width(int bmpId);
int g_get_bmp_height(int bmpId);
void init_ui_element_sprite_with_defaults(void);
void set_ui_element_sprite_pos(float x, float y);
void set_ui_element_sprite_scale(float x, float y);
void set_ui_element_sprite_mult_color(u8 r, u8 g, u8 b, u8 a);
void set_ui_element_sprite_add_color(u8 r, u8 g, u8 b, u8 a);
void set_ui_element_sprite_alpha(float alpha);
void draw_ui_element(s32 bmpId);
void g_set_ui_element_sprite_flags(int flags);
void set_ui_element_sprite_add_color_from_u32(u32 color);
float g_call_get_string_sprite_width_3_discard_result(char *str);
char *fn_2_F06C(int loc, int world);
void draw_ui_box(s32 bmpId);

struct NamePage
{
    char *label;
    char **chars;
};

struct NameLocale
{
    struct NamePage *pages;
    int count;
};

struct DefaultName
{
    char **chars;
    int count;
};

struct StoryFileName
{
    /*0x00*/ char *chars[6];
    /*0x18*/ s8 len;
    /*0x19*/ s8 done;
    /*0x1A*/ s8 page;
    /*0x1B*/ s8 state;
    /*0x1C*/ s8 focus;
    /*0x1D*/ s8 row;
    /*0x1E*/ s8 col;
    /*0x1F*/ s8 confirmSel;
    /*0x20*/ int timer;
    /*0x24*/ float alpha;
    /*0x28*/ s8 tabBlink[8];
    /*0x30*/ s8 btnBlink[3];
    /*0x33*/ s8 fade;
};

struct StorySaveFile
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u32 flags;
    /*0x08*/ char name[13];
    /*0x15*/ s8 world;
    /*0x16*/ u8 filler16[2];
    /*0x18*/ int time;
    /*0x1C*/ int score;
    /*0x20*/ u8 filler20[0x64];
};

struct DataSelectEntry
{
    /*0x00*/ float x;
    /*0x04*/ float y;
    /*0x08*/ float destX;
    /*0x0C*/ float destY;
    /*0x10*/ float velX;
    /*0x14*/ float velY;
    /*0x18*/ u8 filler18[0xC];
    /*0x24*/ float scale;
    /*0x28*/ float destScale;
};

struct DataSelectMenuState
{
    /*0x00*/ s8 state;
    /*0x01*/ s8 sub;
    /*0x02*/ s8 cursor;
    /*0x03*/ s8 copySrc;
    /*0x04*/ s8 copyDst;
    /*0x05*/ s8 delSlot;
    /*0x06*/ u8 timer;
    /*0x07*/ s8 result;
    /*0x08*/ u32 frame;
    /*0x0C*/ struct DataSelectEntry entries[3];
    /*0x90*/ int unk90;
    /*0x94*/ s8 slot;
    /*0x95*/ s8 substate;
};

extern struct StorySaveFile storymode_save_files[];
extern s32 lbl_1_bss_CF3D8;
extern s8 g_last_used_memcard_slot_2;
struct MemcardSaveRequest
{
    u8 unk0;
    u32 flags;
    char *msg;
    void *data;
    u8 filler10[8];
};
void g_save_game_data(void);
void g_sprintf_memcard_error(int a, char *buf);
void g_some_replay_func4(int a, int slot, struct MemcardSaveRequest *req);
u8 g_check_some_memcard_field(void);
int g_get_some_memcard_var(void);
void smd_game_over_save_child(void);
void fn_2_8A40(struct StorySaveFile *file);
s16 play_track_and_fade_out_other_tracks(int track, int fadeTime, s8 volume);
void g_change_music_volume(int track, u32 time, s8 volume);
void queue_bmp_by_id_read(int grpId);
void preload_ape(int chara, int lod, int mesh, int scene);
int g_swapDiscQueueGroup(int group);
void fn_1_EFFD4(int file);
int are_all_party_games_unlocked(void);
int rand(void);
char *strcpy(char *dst, const char *src);
#ifdef __MWERKS__
void *memcpy(void *dst, const void *src, unsigned long n);
void *memset(void *dst, int c, unsigned long n);
#endif

struct StoryFileName story_file_name;
struct DataSelectMenuState data_select_menu_state;
float data_select_x;
float data_select_y;
float data_select_scale_x;
float data_select_scale_y;
float data_select_alpha;
u32 data_select_color;

static inline void hud_f32_to_u8(register float *in, register u8 *out)
{
#ifdef __MWERKS__
    asm
    {
        lfs f1, 0(in)
        psq_st f1, 0(out), 1, 2
    }
#else
    *out = pc_psq_st_u8(*in);
#endif
}

static u32 pulse_color(int timer)
{
    float v;
    u8 b;

    v = (timer % 60) / 60.0f;
    v = 1.0 - __fabs((float)(2.0 * v - 1.0));
    v = __fabs(v);
    v *= 255.0;
    hud_f32_to_u8(&v, &b);
    return b | ((b << 16) | (b << 8));
}

u32 lbl_2_data_1B48[92] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};
char *lbl_2_data_1CCC[6] = {
    "\x4E\x45\x58\x54",
    "",
    "",
    "",
    "",
    "\x82\xB7\x82\xB7\x82\xDE",
};
char *lbl_2_data_1CF4[6] = {
    "\x42\x41\x43\x4B",
    "",
    "",
    "",
    "",
    "\x82\xE0\x82\xC7\x82\xE9",
};
char *lbl_2_data_1D18[6] = {
    "\x45\x4E\x44",
    "",
    "",
    "",
    "",
    "\x82\xA8\x82\xED\x82\xE8",
};
char **lbl_2_data_1D30[3] = { lbl_2_data_1CCC, lbl_2_data_1CF4, lbl_2_data_1D18 };
char *lbl_2_data_1E7C[80] = {
    "\x82\xA0",
    "\x82\xA2",
    "\x82\xA4",
    "\x82\xA6",
    "\x82\xA8",
    "\x82\xA9",
    "\x82\xAB",
    "\x82\xAD",
    "\x82\xAF",
    "\x82\xB1",
    "\x82\xB3",
    "\x82\xB5",
    "\x82\xB7",
    "\x82\xB9",
    "\x82\xBB",
    "\x82\xBD",
    "\x82\xBF",
    "\x82\xC2",
    "\x82\xC4",
    "\x82\xC6",
    "\x82\xC8",
    "\x82\xC9",
    "\x82\xCA",
    "\x82\xCB",
    "\x82\xCC",
    "\x82\xCD",
    "\x82\xD0",
    "\x82\xD3",
    "\x82\xD6",
    "\x82\xD9",
    "\x82\xDC",
    "\x82\xDD",
    "\x82\xDE",
    "\x82\xDF",
    "\x82\xE0",
    "\x82\xE2",
    "\x82\xE4",
    "\x82\xE6",
    "\x82\xED",
    "\x82\xF0",
    "\x82\xE7",
    "\x82\xE8",
    "\x82\xE9",
    "\x82\xEA",
    "\x82\xEB",
    "\x82\xE1",
    "\x82\xE3",
    "\x82\xE5",
    "\x82\xC1",
    "\x82\xF1",
    "\x82\xAA",
    "\x82\xAC",
    "\x82\xAE",
    "\x82\xB0",
    "\x82\xB2",
    "\x82\x9F",
    "\x82\xA1",
    "\x82\xA3",
    "\x82\xA5",
    "\x82\xA7",
    "\x82\xB4",
    "\x82\xB6",
    "\x82\xB8",
    "\x82\xBA",
    "\x82\xBC",
    "\x82\xBE",
    "\x82\xC0",
    "\x82\xC3",
    "\x82\xC5",
    "\x82\xC7",
    "\x82\xCE",
    "\x82\xD1",
    "\x82\xD4",
    "\x82\xD7",
    "\x82\xDA",
    "\x82\xCF",
    "\x82\xD2",
    "\x82\xD5",
    "\x82\xD8",
    "\x82\xDB",
};
char *lbl_2_data_20FC[80] = {
    "\x83\x41",
    "\x83\x43",
    "\x83\x45",
    "\x83\x47",
    "\x83\x49",
    "\x83\x4A",
    "\x83\x4C",
    "\x83\x4E",
    "\x83\x50",
    "\x83\x52",
    "\x83\x54",
    "\x83\x56",
    "\x83\x58",
    "\x83\x5A",
    "\x83\x5C",
    "\x83\x5E",
    "\x83\x60",
    "\x83\x63",
    "\x83\x65",
    "\x83\x67",
    "\x83\x69",
    "\x83\x6A",
    "\x83\x6B",
    "\x83\x6C",
    "\x83\x6D",
    "\x83\x6E",
    "\x83\x71",
    "\x83\x74",
    "\x83\x77",
    "\x83\x7A",
    "\x83\x7D",
    "\x83\x7E",
    "\x83\x80",
    "\x83\x81",
    "\x83\x82",
    "\x83\x84",
    "\x83\x86",
    "\x83\x88",
    "\x83\x8F",
    "\x83\x92",
    "\x83\x89",
    "\x83\x8A",
    "\x83\x8B",
    "\x83\x8C",
    "\x83\x8D",
    "\x83\x83",
    "\x83\x85",
    "\x83\x87",
    "\x83\x62",
    "\x83\x93",
    "\x83\x4B",
    "\x83\x4D",
    "\x83\x4F",
    "\x83\x51",
    "\x83\x53",
    "\x83\x40",
    "\x83\x42",
    "\x83\x44",
    "\x83\x46",
    "\x83\x48",
    "\x83\x55",
    "\x83\x57",
    "\x83\x59",
    "\x83\x5B",
    "\x83\x5D",
    "\x83\x5F",
    "\x83\x61",
    "\x83\x64",
    "\x83\x66",
    "\x83\x68",
    "\x83\x6F",
    "\x83\x72",
    "\x83\x75",
    "\x83\x78",
    "\x83\x7B",
    "\x83\x70",
    "\x83\x73",
    "\x83\x76",
    "\x83\x79",
    "\x83\x7C",
};
char *nameentry_character_ptr_list[80] = {
    "\x41",
    "\x42",
    "\x43",
    "\x44",
    "\x45",
    "\x46",
    "\x47",
    "\x48",
    "\x49",
    "\x4A",
    "\x4B",
    "\x4C",
    "\x4D",
    "\x4E",
    "\x4F",
    "\x50",
    "\x51",
    "\x52",
    "\x53",
    "\x54",
    "\x55",
    "\x56",
    "\x57",
    "\x58",
    "\x59",
    "\x5A",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x61",
    "\x62",
    "\x63",
    "\x64",
    "\x65",
    "\x66",
    "\x67",
    "\x68",
    "\x69",
    "\x6A",
    "\x6B",
    "\x6C",
    "\x6D",
    "\x6E",
    "\x6F",
    "\x70",
    "\x71",
    "\x72",
    "\x73",
    "\x74",
    "\x75",
    "\x76",
    "\x74",
    "\x78",
    "\x79",
    "\x7A",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
};
char *lbl_2_data_256C[80] = {
    "\x30",
    "\x31",
    "\x32",
    "\x33",
    "\x34",
    "\x35",
    "\x36",
    "\x37",
    "\x38",
    "\x39",
    "\x81\x43",
    "\x81\x44",
    "\x81\x45",
    "\x81\x46",
    "\x81\x47",
    "\x81\x48",
    "\x81\x49",
    "\x81\x4D",
    "\x81\x4F",
    "\x81\x51",
    "\x81\x58",
    "\x81\x5B",
    "\x81\x5C",
    "\x81\x5E",
    "\x81\x5F",
    "\x81\x60",
    "\x81\x62",
    "\x81\x63",
    "\x81\x65",
    "\x81\x66",
    "\x81\x67",
    "\x81\x68",
    "\x81\x69",
    "\x81\x6A",
    "\x81\x6D",
    "\x81\x6E",
    "\x81\x77",
    "\x81\x78",
    "\x81\x7B",
    "\x81\x7C",
    "\x81\x7E",
    "\x81\x80",
    "\x81\x81",
    "\x81\x82",
    "\x81\x83",
    "\x81\x84",
    "\x81\x87",
    "\x81\x89",
    "\x81\x8A",
    "\x81\x8F",
    "\x81\x90",
    "\x81\x93",
    "\x81\x94",
    "\x81\x95",
    "\x81\x96",
    "\x81\x97",
    "\x81\x99",
    "\x81\x9A",
    "\x81\x9B",
    "\x81\x9C",
    "\x81\xA0",
    "\x81\xA1",
    "\x81\xA2",
    "\x81\xA3",
    "\x81\xA6",
    "\x81\xA8",
    "\x81\xCB",
    "\x81\xF4",
    "\x84\xD6",
    "\x84\xD2",
    "\x84\xD4",
    "\x84\xD0",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
};
char *lbl_2_data_26AC[80] = {
    "\x30",
    "\x31",
    "\x32",
    "\x33",
    "\x34",
    "\x35",
    "\x36",
    "\x37",
    "\x38",
    "\x39",
    "\x81\x43",
    "\x81\x44",
    "\x81\x45",
    "\x81\x46",
    "\x81\x47",
    "\x81\x48",
    "\x81\x49",
    "\x81\x4D",
    "\x81\x4F",
    "\x81\x51",
    "\x81\x5B",
    "\x81\x5C",
    "\x81\x5E",
    "\x81\x5F",
    "\x81\x60",
    "\x81\x62",
    "\x81\x63",
    "\x81\x65",
    "\x81\x66",
    "\x81\x67",
    "\x81\x68",
    "\x81\x69",
    "\x81\x6A",
    "\x81\x6D",
    "\x81\x6E",
    "\x81\x77",
    "\x81\x78",
    "\x81\x7B",
    "\x81\x7C",
    "\x81\x7E",
    "\x81\x80",
    "\x81\x81",
    "\x81\x82",
    "\x81\x83",
    "\x81\x84",
    "\x81\x87",
    "\x81\x89",
    "\x81\x8A",
    "\x81\x90",
    "\x81\x93",
    "\x81\x94",
    "\x81\x95",
    "\x81\x96",
    "\x81\x97",
    "\x81\x99",
    "\x81\x9A",
    "\x81\x9B",
    "\x81\x9C",
    "\x81\xA0",
    "\x81\xA1",
    "\x81\xA2",
    "\x81\xA3",
    "\x81\xA6",
    "\x81\xA8",
    "\x81\xCB",
    "\x81\xF4",
    "\x84\xD6",
    "\x84\xD2",
    "\x84\xD4",
    "\x84\xD0",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
    "\x20",
};
struct NamePage lbl_2_data_27F8[2] = {
    { "\x41\x42\x43", nameentry_character_ptr_list },
    { "\x4D\x61\x72\x6B", lbl_2_data_26AC },
};
struct NamePage lbl_2_data_2808[2] = {
    { "\x41\x42\x43", nameentry_character_ptr_list },
    { "\x4D\x61\x72\x6B", lbl_2_data_26AC },
};
struct NamePage lbl_2_data_2818[2] = {
    { "\x41\x42\x43", nameentry_character_ptr_list },
    { "\x4D\x61\x72\x6B", lbl_2_data_26AC },
};
struct NamePage lbl_2_data_2828[2] = {
    { "\x41\x42\x43", nameentry_character_ptr_list },
    { "\x4D\x61\x72\x6B", lbl_2_data_26AC },
};
struct NamePage lbl_2_data_2838[2] = {
    { "\x41\x42\x43", nameentry_character_ptr_list },
    { "\x4D\x61\x72\x6B", lbl_2_data_26AC },
};
struct NamePage lbl_2_data_2868[4] = {
    { "\x82\xD0\x82\xE7\x82\xAA\x82\xC8", lbl_2_data_1E7C },
    { "\x83\x4A\x83\x5E\x83\x4A\x83\x69", lbl_2_data_20FC },
    { "\x41\x42\x43", nameentry_character_ptr_list },
    { "\x82\xAB\x82\xB2\x82\xA4", lbl_2_data_256C },
};
struct NameLocale lbl_2_data_2888[6] = {
    { lbl_2_data_27F8, 2 },
    { lbl_2_data_2808, 2 },
    { lbl_2_data_2818, 2 },
    { lbl_2_data_2828, 2 },
    { lbl_2_data_2838, 2 },
    { lbl_2_data_2868, 4 },
};
char *lbl_2_data_28D8[6] = {
    "\x4E\x61\x6D\x65\x20\x45\x6E\x74\x72\x79",
    "",
    "",
    "",
    "",
    "\x83\x6C\x81\x5B\x83\x80\x83\x47\x83\x93\x83\x67\x83\x8A\x81\x5B",
};
char *lbl_2_data_2928[6] = {
    "\x50\x6C\x65\x61\x73\x65\x20\x65\x6E\x74\x65\x72\x20\x79\x6F\x75\x72\x20\x6E\x61\x6D\x65\x2E",
    "",
    "",
    "",
    "",
    "\x82\xE0\x82\xB6\x82\xF0\x83\xAD\x82\xC5\x82\xA6\x82\xE7\x82\xF1\x82\xC5\x83\x9F\x82\xC5\x82\xAF\x82\xC1\x82\xC4\x82\xA2",
};
char *lbl_2_data_2968[6] = {
    "\x41\x72\x65\x20\x79\x6F\x75\x20\x73\x75\x72\x65\x3F",
    "",
    "",
    "",
    "",
    "\x82\xB1\x82\xEA\x82\xC5\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48",
};
char *lbl_2_data_298C[6] = {
    "\x59\x65\x73",
    "",
    "",
    "",
    "",
    "\x82\xCD\x82\xA2",
};
char *lbl_2_data_29B0[6] = {
    "\x4E\x6F",
    "",
    "",
    "",
    "",
    "\x82\xA2\x82\xA2\x82\xA6",
};
char *lbl_2_data_29C8[4] = { "\x41", "\x49", "\x41", "\x49" };
char *lbl_2_data_29D8[4] = { "\x83\x41", "\x83\x43", "\x83\x41", "\x83\x43" };
struct DefaultName lbl_2_data_29E8 = { lbl_2_data_29C8, 4 };
struct DefaultName lbl_2_data_29F0 = { lbl_2_data_29D8, 4 };
struct DefaultName *lbl_2_data_29F8[6] = { &lbl_2_data_29E8, &lbl_2_data_29E8, &lbl_2_data_29E8, &lbl_2_data_29E8, &lbl_2_data_29E8, &lbl_2_data_29F0 };


void sprite_story_file_nameentry_disp(struct Sprite *sprite)
{
    struct NameLocale *nl;
    int i;
    u32 c;
    float x, y, alpha, x2, y2, a;

    GXSetZMode_cached(1, 3, 0);
    set_ui_widescreen_scale_mtx(0x140);
    nl = &lbl_2_data_2888[locale];
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_scale(1.0f, 0.84375f);
    set_ui_element_sprite_pos(320.0f, 54.0f);
    draw_ui_box(0x1808);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(-0x8000);
    textdraw_set_drop_shadow();
    textdraw_set_font_style(5);
    textdraw_set_pos(320.0f, 54.0f);
    textdraw_print(lbl_2_data_28D8[locale]);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(320.0f, 426.0f);
    set_ui_element_sprite_scale(1.0f, 0.84375f);
    draw_ui_box(0x1808);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(-0x8000);
    textdraw_set_drop_shadow();
    textdraw_set_flags(0x90000000);
    textdraw_set_pos(320.0f, 426.0f);
    textdraw_print(lbl_2_data_2928[locale]);

    x = 320.0f;
    y = 270.0f;
    alpha = story_file_name.alpha;
    c = 255.0 * (1.0 - alpha) + 127.0f * alpha;
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(x, y);
    set_ui_element_sprite_mult_color(0, c, c, c);
    draw_ui_element(0x1802);
    x = x - 0.5 * g_get_bmp_width(0x1802);
    y = y - 0.5 * g_get_bmp_height(0x1802);

    if (story_file_name.state == 0)
    {
        switch (story_file_name.focus)
        {
        case 0:
            x2 = 49.0 + x;
            y2 = 23.0 + y + 32.0 * story_file_name.page;
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_pos(x2, y2);
            set_ui_element_sprite_mult_color(0xFF, 0, 0xF0, 0xDD);
            set_ui_element_sprite_scale(2.375f, 1.0f);
            draw_ui_element(0x1804);
            break;
        case 1:
            x2 = 105.0 + x + 28.0 * story_file_name.col;
            y2 = 21.0 + y + 27.0 * story_file_name.row;
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_pos(x2, y2);
            set_ui_element_sprite_mult_color(0xFF, 0, 0xF0, 0xDD);
            draw_ui_element(0x1804);
            break;
        case 2:
            x2 = 415.0 + x;
            y2 = 23.0 + y + 32.0 * story_file_name.row;
            init_ui_element_sprite_with_defaults();
            set_ui_element_sprite_pos(x2, y2);
            set_ui_element_sprite_mult_color(0xFF, 0, 0xF0, 0xDD);
            set_ui_element_sprite_scale(2.375f, 1.0f);
            draw_ui_element(0x1804);
            break;
        }
    }

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0xFF603913);
    textdraw_set_scale(0.6666667f, 0.6666667f);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_mult_color(0xFF, 0x60, 0x39, 0x13);
    for (i = 0; i < nl->count; i++)
    {
        y2 = 49.0 + x;
        x2 = 23.0 + y + 32.0 * i;
        if (i == story_file_name.page)
            a = 1.0f;
        else
            a = 0.5f;
        c = 255.0f * (story_file_name.tabBlink[i] / 8.0f);
        set_ui_element_sprite_pos(y2, x2);
        set_ui_element_sprite_alpha(a);
        set_ui_element_sprite_add_color(0, c, c, c);
        draw_ui_element(0x1805);
        textdraw_set_pos(y2, x2);
        textdraw_set_alpha(a);
        textdraw_set_add_color(c | ((c << 16) | (c << 8)));
        textdraw_print(nl->pages[i].label);
    }
    set_ui_element_sprite_alpha(1.0f);
    textdraw_set_alpha(1.0f);
    for (i = 0; i < 3; i++)
    {
        a = 415.0 + x;
        y2 = 23.0 + y + 32.0 * i;
        c = 255.0f * (story_file_name.btnBlink[i] / 8.0f);
        set_ui_element_sprite_pos(a, y2);
        set_ui_element_sprite_add_color(0, c, c, c);
        draw_ui_element(0x1805);
        textdraw_set_pos(a, y2);
        textdraw_set_add_color(c | ((c << 16) | (c << 8)));
        textdraw_print(lbl_2_data_1D30[i][locale]);
    }

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0xFF603913);
    for (i = 0; i < 8; i++)
    {
        for (c = 0; (int)c < 10; c++)
        {
            float px = 105.0 + x + 28.0 * (int)c;
            float py = 21.0 + y + 27.0 * i;

            textdraw_set_pos(px, py);
            textdraw_print(nl->pages[story_file_name.page].chars[c + i * 10]);
        }
    }

    x = 320.0f;
    y = 120.0f;
    c = 32.0 * alpha;
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0xFF603913);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(x, y);
    set_ui_element_sprite_add_color(0, c, c, c);
    draw_ui_element(0x1803);
    x = x - 0.5 * g_get_bmp_width(0x1803);
    y = y - 0.5 * g_get_bmp_height(0x1803);

    if (story_file_name.len < 6 && story_file_name.state == 0)
    {
        a = 28.0 + x + 32.0 * story_file_name.len;
        y2 = 24.0 + y;
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_pos(a, y2);
        set_ui_element_sprite_mult_color(0xFF, 0, 0xF0, 0xDD);
        draw_ui_element(0x1804);
    }
    for (i = 0; i < story_file_name.len; i++)
    {
        if (story_file_name.chars[i] != NULL)
        {
            float px = 28.0 + x + 32.0 * i;
            float py = 24.0 + y;

            textdraw_set_pos(px, py);
            textdraw_print(story_file_name.chars[i]);
        }
    }

    x = 320.0 * alpha + 960.0 * (1.0 - alpha);
    y = 240.0f;
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_scale(1.0f, 1.6875f);
    set_ui_element_sprite_pos(x, y);
    draw_ui_box(0x1808);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(-0x8000);
    textdraw_set_drop_shadow();
    textdraw_set_flags(0x80000000);
    textdraw_set_pos(x, y - 20.0);
    textdraw_print(lbl_2_data_2968[locale]);
    textdraw_set_pos(x - 62.0, 6.0 + y);
    textdraw_set_alignment(3);
    textdraw_set_border();
    if (story_file_name.confirmSel == 0)
    {
        textdraw_set_mul_color(-0x100);
        textdraw_print("\x81\x6D\x20\x20\x20\x20\x20\x20\x81\x6E");
        textdraw_set_pos(x - 62.0, 6.0 + y);
        textdraw_set_add_color(pulse_color(story_file_name.timer));
    }
    else
    {
        textdraw_set_mul_color(0xFF7F7F00);
        textdraw_set_add_color(0);
    }
    textdraw_print(lbl_2_data_298C[locale]);
    textdraw_set_pos(62.0 + x, 6.0 + y);
    textdraw_set_alignment(3);
    if (story_file_name.confirmSel == 1)
    {
        textdraw_set_mul_color(-0x100);
        textdraw_print("\x81\x6D\x20\x20\x20\x20\x20\x20\x81\x6E");
        textdraw_set_pos(62.0 + x, 6.0 + y);
        textdraw_set_add_color(pulse_color(story_file_name.timer));
    }
    else
    {
        textdraw_set_mul_color(0xFF7F7F00);
        textdraw_set_add_color(0);
    }
    textdraw_print(lbl_2_data_29B0[locale]);
    reset_ui_widescreen_scale_mtx();
    GXSetZMode_cached(1, 3, 1);
}

void create_story_file_nameentry_sprite(void)
{
    struct Sprite *sprite;

    memset(&story_file_name, 0, sizeof(story_file_name));
    story_file_name.page = 0;
    story_file_name.focus = 1;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x28;
        sprite->depth = 1.0f;
        sprite->drawFunc = sprite_story_file_nameentry_disp;
        strcpy(sprite->text, "NAME INPUT");
    }
}
char *lbl_2_data_2A3C[6] = {
    "\x43\x6F\x70\x79",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x73\x81\x5B\x82\xB7\x82\xE9",
};
char *lbl_2_data_2A68[6] = {
    "\x44\x65\x6C\x65\x74\x65",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x82\xAF\x82\xB7",
};
char *lbl_2_data_2A88[6] = {
    "\x52\x65\x74\x75\x72\x6E",
    "",
    "",
    "",
    "",
    "\x82\xE0\x82\xC7\x82\xE9",
};
char **lbl_2_data_2AA0[3] = { lbl_2_data_2A3C, lbl_2_data_2A68, lbl_2_data_2A88 };

void dmd_scen_entry_main_child(void)
{
    struct NameLocale *nl;
    u32 sound;
    int flags;
    char *ch;
    int i;
    int row;
    int col;

    sound = 0;
    flags = 0;
    ch = "\x20";
    nl = &lbl_2_data_2888[locale];
    for (i = 0; i < 8; i++)
    {
        if (story_file_name.tabBlink[i] > 0)
            story_file_name.tabBlink[i]--;
    }
    for (i = 0; i < 3; i++)
    {
        if (story_file_name.btnBlink[i] > 0)
            story_file_name.btnBlink[i]--;
    }

    switch (story_file_name.state)
    {
    case 0:
        switch (story_file_name.focus)
        {
        case 0:
            if ((merged_digital_inputs[4] & PAD_BUTTON_UP) || (merged_analog_inputs[4] & PAD_BUTTON_UP))
                flags |= 0x400;
            if ((merged_digital_inputs[4] & PAD_BUTTON_DOWN) || (merged_analog_inputs[4] & PAD_BUTTON_DOWN))
                flags |= 0x200;
            if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
                flags |= 0x10;
            if (merged_digital_inputs[2] & PAD_BUTTON_B)
                flags |= 2;
            if (merged_digital_inputs[2] & 0x1000)
                flags |= 4;
            break;
        case 1:
        {
            col = story_file_name.col;
            row = story_file_name.row;

            if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
            {
                col--;
                if (col < 0)
                {
                    flags |= 8;
                    break;
                }
            }
            else if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
            {
                col++;
                if (col >= 10)
                {
                    flags |= 0x20;
                    break;
                }
            }
            if ((merged_digital_inputs[4] & PAD_BUTTON_UP) || (merged_analog_inputs[4] & PAD_BUTTON_UP))
            {
                row--;
                if (row <= 0)
                    row = 0;
            }
            if ((merged_digital_inputs[4] & PAD_BUTTON_DOWN) || (merged_analog_inputs[4] & PAD_BUTTON_DOWN))
            {
                row++;
                if (row >= 8)
                    row = 7;
            }
            if (col != story_file_name.col)
            {
                sound |= 2;
                story_file_name.col = col;
            }
            if (row != story_file_name.row)
            {
                sound |= 2;
                story_file_name.row = row;
            }
            if (merged_digital_inputs[2] & PAD_BUTTON_A)
                flags |= 0x800;
            if (merged_digital_inputs[2] & PAD_BUTTON_B)
                flags |= 2;
            if (merged_digital_inputs[2] & 0x1000)
                flags |= 4;
            ch = nl->pages[story_file_name.page].chars[col + row * 10];
            break;
        }
        case 2:
        {
            row = story_file_name.row;

            if ((merged_digital_inputs[4] & PAD_BUTTON_UP) || (merged_analog_inputs[4] & PAD_BUTTON_UP))
            {
                row--;
                if (row < 0)
                    row = 0;
            }
            if ((merged_digital_inputs[4] & PAD_BUTTON_DOWN) || (merged_analog_inputs[4] & PAD_BUTTON_DOWN))
            {
                row++;
                if (row >= 3)
                    row = 2;
            }
            if (row != story_file_name.row)
            {
                sound |= 2;
                story_file_name.row = row;
            }
            if (story_file_name.len < 6
             && ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT)))
                flags |= 0x10;
            if (merged_digital_inputs[2] & PAD_BUTTON_A)
            {
                switch (story_file_name.row)
                {
                case 0:
                    flags |= 1;
                    story_file_name.btnBlink[0] = 8;
                    break;
                case 1:
                    flags |= 2;
                    break;
                case 2:
                    flags |= 4;
                    break;
                }
            }
            if (merged_digital_inputs[2] & PAD_BUTTON_B)
                flags |= 2;
            if (merged_digital_inputs[2] & 0x1000)
                flags |= 4;
            break;
        }
        }
        if (merged_digital_inputs[2] & PAD_TRIGGER_L)
            flags |= 0x200;
        else if (merged_digital_inputs[2] & PAD_TRIGGER_R)
            flags |= 0x400;
        break;
    case 1:
    {
        int sel = story_file_name.confirmSel;

        if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
            sel = 0;
        else if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
            sel = 1;
        if (sel != story_file_name.confirmSel)
        {
            sound |= 2;
            story_file_name.confirmSel = sel;
        }
        if (merged_digital_inputs[2] & PAD_BUTTON_A)
        {
            if (story_file_name.confirmSel == 0)
                flags |= 0x80;
            else
                flags |= 0x100;
        }
        else if (merged_digital_inputs[2] & PAD_BUTTON_B)
            flags |= 0x100;
        break;
    }
    case 2:
        story_file_name.fade = 30;
        fade_screen_to_color(0x101, 0, story_file_name.fade);
        story_file_name.state = 3;
        break;
    case 3:
        story_file_name.fade--;
        if (story_file_name.fade <= 0)
            story_file_name.done = 2;
        break;
    }

    if (flags & 0x801)
    {
        int n;

        if (flags & 1)
            ch = "\x20";
        n = story_file_name.len;
        if (n < 6)
        {
            story_file_name.chars[n] = ch;
            n++;
            if (n >= 6)
            {
                flags |= 0x20;
                story_file_name.row = 2;
            }
        }
        if (n != story_file_name.len)
        {
            sound |= 8;
            story_file_name.len = n;
        }
    }
    else if (flags & 2)
    {
        if (story_file_name.len > 0)
        {
            story_file_name.len--;
            story_file_name.chars[story_file_name.len] = NULL;
            sound |= 0x10;
            story_file_name.btnBlink[1] = 8;
        }
        else
            story_file_name.state = 2;
    }
    else if (flags & 4)
    {
        sound |= 1;
        story_file_name.btnBlink[2] = 8;
        flags |= 0x40;
    }

    {
        int page = story_file_name.page;

        if (flags & 0x400)
        {
            page--;
            if (page < 0)
                page = nl->count - 1;
        }
        else if (flags & 0x200)
        {
            page++;
            if (page >= nl->count)
                page = 0;
        }
        if (page != story_file_name.page)
        {
            sound |= 0x10;
            story_file_name.tabBlink[page] = 8;
            story_file_name.page = page;
        }
    }

    if (flags & 8)
    {
        sound |= 2;
        story_file_name.focus = 0;
    }
    else if (flags & 0x10)
    {
        sound |= 2;
        switch (story_file_name.focus)
        {
        case 0:
            story_file_name.col = 0;
            story_file_name.row = story_file_name.page;
            break;
        case 2:
            story_file_name.col = 9;
            break;
        case 1:
            break;
        }
        story_file_name.focus = 1;
    }
    else if (flags & 0x20)
    {
        sound |= 2;
        story_file_name.row = story_file_name.row < 2 ? story_file_name.row : 2;
        story_file_name.focus = 2;
    }

    if (flags & 0x40)
    {
        int allSpaces = 1;

        for (i = 0; i < story_file_name.len; i++)
        {
            if (((u8 *)story_file_name.chars[i])[0] != ' ')
            {
                allSpaces = 0;
                break;
            }
        }
        if (allSpaces)
        {
            struct DefaultName *def = lbl_2_data_29F8[locale];

            for (i = 0; i < def->count; i++)
                story_file_name.chars[i] = def->chars[i];
            story_file_name.len = i;
        }
        story_file_name.state = 1;
    }
    else if (flags & 0x100)
        story_file_name.state = 0;
    else if (flags & 0x80)
    {
        sound |= 1;
        story_file_name.done = 1;
    }

    if (sound & 1)
        call_SoundReqID_arg_0(0x6A);
    else if (sound & 2)
        call_SoundReqID_arg_0(0x6C);
    else if (sound & 4)
        call_SoundReqID_arg_0(0x6B);
    else if (sound & 8)
        call_SoundReqID_arg_0(0x65);
    else if (sound & 0x10)
        call_SoundReqID_arg_0(0x66);
    else if (sound & 0x20)
        call_SoundReqID_arg_0(0x16);

    story_file_name.timer++;
    if (story_file_name.state == 1)
        story_file_name.alpha += 0.1 * (1.0 - story_file_name.alpha);
    else
        story_file_name.alpha += 0.1 * -story_file_name.alpha;
}

void fn_2_12468(void)
{
    destroy_sprite_with_unique_id(0x28);
}

int fn_2_1248C(void)
{
    return story_file_name.done;
}

void g_get_story_file_name(char *dst)
{
    int i;

    for (i = 0; i < story_file_name.len; i++)
    {
        strcpy(dst, story_file_name.chars[i]);
        dst += strlen(dst);
    }
}

static u32 pulse_color_u(u32 timer)
{
    float v;
    u8 b;

    v = (timer % 60) / 60.0f;
    v = 1.0 - __fabs((float)(2.0 * v - 1.0));
    v = __fabs(v);
    v *= 255.0;
    hud_f32_to_u8(&v, &b);
    return b | ((b << 16) | (b << 8));
}

static void print_no_cursor(void)
{
    textdraw_print("\x81\x6D\x20\x20\x20\x20\x20\x20\x20\x81\x6E");
}

char *lbl_2_data_2B00[6] = {
    "\x43\x6F\x6E\x74\x69\x6E\x75\x65\x20\x74\x68\x65\x20\x67\x61\x6D\x65\x20\x66\x72\x6F\x6D\x20\x74\x68\x65\x20\x73\x61\x76\x65\x64\x20\x70\x6F\x69\x6E\x74\x2E",
    "",
    "",
    "",
    "",
    "\x83\x51\x81\x5B\x83\x80\x82\xF0\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7\x82\xA0\x82\xBB\x82\xD1\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_2_data_2B5C[6] = {
    "\x53\x74\x61\x72\x74\x20\x74\x68\x65\x20\x67\x61\x6D\x65\x20\x66\x72\x6F\x6D\x20\x74\x68\x65\x20\x62\x65\x67\x69\x6E\x6E\x69\x6E\x67\x2E",
    "",
    "",
    "",
    "",
    "\x83\x51\x81\x5B\x83\x80\x82\xF0\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7\x82\xA0\x82\xBB\x82\xD1\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_2_data_2BA0[6] = {
    "\x57\x69\x6C\x6C\x20\x63\x6F\x70\x79\x20\x74\x68\x65\x20\x64\x61\x74\x61\x2E",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x82\xF0\x83\x52\x83\x73\x81\x5B\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_2_data_2BE0[6] = {
    "\x44\x65\x6C\x65\x74\x65\x20\x74\x68\x65\x20\x64\x61\x74\x61\x2E",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x82\xF0\x82\xAF\x82\xB5\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_2_data_2C2C[6] = {
    "\x42\x61\x63\x6B\x20\x74\x6F\x20\x74\x68\x65\x20\x70\x72\x65\x76\x69\x6F\x75\x73\x2E",
    "",
    "",
    "",
    "",
    "\x82\xD0\x82\xC6\x82\xC2\x82\xDC\x82\xA6\x82\xC9\x82\xE0\x82\xC7\x82\xE8\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_2_data_2C84[6] = {
    "\x50\x6C\x65\x61\x73\x65\x20\x63\x68\x6F\x6F\x73\x65\x20\x74\x68\x65\x20\x64\x61\x74\x61\x20\x74\x6F\x20\x63\x6F\x70\x79\x2E",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x73\x81\x5B\x82\xE0\x82\xC6\x82\xF0\x82\xA6\x82\xE7\x82\xF1\x82\xC5\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42",
};
char *lbl_2_data_2CDC[6] = {
    "\x50\x6C\x65\x61\x73\x65\x20\x63\x68\x6F\x6F\x73\x65\x20\x77\x68\x65\x72\x65\x20\x74\x6F\x20\x63\x6F\x70\x79\x20\x74\x6F\x2E",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x73\x81\x5B\x82\xB3\x82\xAB\x82\xF0\x82\xA6\x82\xE7\x82\xF1\x82\xC5\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42",
};
char *lbl_2_data_2D54[6] = {
    "\x44\x6F\x20\x79\x6F\x75\x20\x77\x61\x6E\x74\x20\x74\x6F\x20\x63\x6F\x70\x79\x20\x74\x68\x65\x20\x64\x61\x74\x61\x0A\x66\x72\x6F\x6D\x20\x25\x64\x20\x74\x6F\x20\x25\x64\x3F",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x82\xF0\x25\x64\x82\xA9\x82\xE7\x25\x64\x82\xD6\x83\x52\x83\x73\x81\x5B\x82\xB5\x82\xDC\x82\xB7\x81\x42\x0A\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48",
};
char *lbl_2_data_2D90[6] = {
    "\x43\x61\x6E\x63\x65\x6C\x6C\x65\x64\x2E",
    "",
    "",
    "",
    "",
    "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x42",
};
char *lbl_2_data_2DD8[6] = {
    "\x43\x6F\x70\x79\x69\x6E\x67\x20\x74\x68\x65\x20\x64\x61\x74\x61\x2E",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x82\xF0\x83\x52\x83\x73\x81\x5B\x82\xB5\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_2_data_2E28[6] = {
    "\x46\x69\x6E\x69\x73\x68\x65\x64\x20\x63\x6F\x70\x79\x69\x6E\x67\x20\x74\x68\x65\x20\x64\x61\x74\x61\x2E",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x82\xF0\x83\x52\x83\x73\x81\x5B\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x42",
};
char *lbl_2_data_2E7C[6] = {
    "\x50\x6C\x65\x61\x73\x65\x20\x63\x68\x6F\x6F\x73\x65\x20\x61\x20\x64\x61\x74\x61\x20\x74\x6F\x20\x64\x65\x6C\x65\x74\x65\x2E",
    "",
    "",
    "",
    "",
    "\x82\xAF\x82\xB7\x83\x66\x81\x5B\x83\x5E\x82\xF0\x82\xE7\x82\xF1\x82\xC5\x89\xBA\x82\xB3\x82\xA2\x81\x42",
};
char *lbl_2_data_2EE0[6] = {
    "\x44\x6F\x20\x79\x6F\x75\x20\x77\x61\x6E\x74\x20\x74\x6F\x20\x64\x65\x6C\x65\x74\x65\x20\x74\x68\x65\x20\x64\x61\x74\x61\x20\x25\x64\x3F",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x25\x64\x82\xF0\x82\xAF\x82\xB5\x82\xC4\x82\xE0\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48\x81\x42",
};
char *lbl_2_data_2F2C[6] = {
    "\x44\x65\x6C\x65\x74\x69\x6E\x67\x20\x74\x68\x65\x20\x64\x61\x74\x61\x20\x25\x64\x2E",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x25\x64\x82\xF0\x82\xAF\x82\xB5\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x81\x42",
};
char *lbl_2_data_2F74[6] = {
    "\x44\x65\x6C\x65\x74\x65\x64\x20\x74\x68\x65\x20\x64\x61\x74\x61\x20\x25\x64\x2E",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x25\x64\x82\xF0\x82\xAF\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x42",
};

void fn_2_1251C(float x, float y)
{
    struct StorySaveFile *file;

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_flags(0x80000000);
    textdraw_set_mul_color(-0x100);
    textdraw_set_drop_shadow();
    textdraw_set_spacing(0.0f, 4.0f);
    textdraw_set_scale(0.7f, 1.0f);
    textdraw_set_pos(x, y);
    file = &storymode_save_files[data_select_menu_state.cursor];
    switch (data_select_menu_state.state)
    {
        case 0:
            switch (data_select_menu_state.sub)
            {
                case 0:
                    if (file->flags & 1)
                        textdraw_print(lbl_2_data_2B00[locale]);
                    else
                        textdraw_print(lbl_2_data_2B5C[locale]);
                    break;
                case 1:
                    switch (data_select_menu_state.cursor)
                    {
                        case 0:
                            textdraw_print(lbl_2_data_2BA0[locale]);
                            break;
                        case 1:
                            textdraw_print(lbl_2_data_2BE0[locale]);
                            break;
                        case 2:
                            textdraw_print(lbl_2_data_2C2C[locale]);
                            break;
                    }
                    break;
            }
            break;
        case 1:
            textdraw_print(lbl_2_data_2C84[locale]);
            break;
        case 2:
            textdraw_print(lbl_2_data_2CDC[locale]);
            break;
        case 3:
            textdraw_set_mul_color(0xFFEF9A00);
            textdraw_set_pos(x, y - 20.0);
            textdraw_printf(lbl_2_data_2D54[locale], data_select_menu_state.copySrc + 1,
                            data_select_menu_state.copyDst + 1);
            textdraw_set_pos(x - 62.0, 16.0 + y);
            textdraw_set_alignment(3);
            textdraw_set_flags(0x80000000);
            textdraw_set_scale(1.0f, 1.0f);
            textdraw_set_border();
            if (data_select_menu_state.cursor == 0)
            {
                textdraw_set_mul_color(-0x100);
                textdraw_print("\x81\x6D\x20\x20\x20\x20\x20\x20\x81\x6E");
                textdraw_set_pos(x - 62.0, 16.0 + y);
                textdraw_set_add_color(pulse_color_u(data_select_menu_state.frame));
            }
            else
            {
                textdraw_set_mul_color(0xFF7F7F00);
                textdraw_set_add_color(0);
            }
            textdraw_print(lbl_2_data_298C[locale]);
            textdraw_set_pos(62.0 + x, 16.0 + y);
            textdraw_set_alignment(3);
            if (data_select_menu_state.cursor == 1)
            {
                textdraw_set_mul_color(-0x100);
                print_no_cursor();
                textdraw_set_pos(62.0 + x, 16.0 + y);
                textdraw_set_add_color(pulse_color_u(data_select_menu_state.frame));
            }
            else
            {
                textdraw_set_mul_color(0xFF7F7F00);
                textdraw_set_add_color(0);
            }
            textdraw_print(lbl_2_data_29B0[locale]);
            break;
        case 4:
            textdraw_print(lbl_2_data_2D90[locale]);
            break;
        case 5:
            if (data_select_menu_state.timer < 120)
                textdraw_print(lbl_2_data_2DD8[locale]);
            else
                textdraw_print(lbl_2_data_2E28[locale]);
            break;
        case 6:
            textdraw_print(lbl_2_data_2E7C[locale]);
            break;
        case 7:
            textdraw_set_mul_color(0xFFEF9A00);
            textdraw_set_pos(x, y - 20.0);
            textdraw_printf(lbl_2_data_2EE0[locale], data_select_menu_state.delSlot + 1);
            textdraw_set_pos(x - 62.0, 16.0 + y);
            textdraw_set_alignment(3);
            textdraw_set_flags(0x80000000);
            textdraw_set_scale(1.0f, 1.0f);
            textdraw_set_border();
            if (data_select_menu_state.cursor == 0)
            {
                textdraw_set_mul_color(-0x100);
                textdraw_print("\x81\x6D\x20\x20\x20\x20\x20\x20\x81\x6E");
                textdraw_set_pos(x - 62.0, 16.0 + y);
                textdraw_set_add_color(pulse_color_u(data_select_menu_state.frame));
            }
            else
            {
                textdraw_set_mul_color(0xFF7F7F00);
                textdraw_set_add_color(0);
            }
            textdraw_print(lbl_2_data_298C[locale]);
            textdraw_set_pos(62.0 + x, 16.0 + y);
            textdraw_set_alignment(3);
            if (data_select_menu_state.cursor == 1)
            {
                textdraw_set_mul_color(-0x100);
                print_no_cursor();
                textdraw_set_pos(62.0 + x, 16.0 + y);
                textdraw_set_add_color(pulse_color_u(data_select_menu_state.frame));
            }
            else
            {
                textdraw_set_mul_color(0xFF7F7F00);
                textdraw_set_add_color(0);
            }
            textdraw_print(lbl_2_data_29B0[locale]);
            break;
        case 8:
            textdraw_print(lbl_2_data_2D90[locale]);
            break;
        case 9:
            if (data_select_menu_state.timer < 120)
                textdraw_printf(lbl_2_data_2F2C[locale], data_select_menu_state.delSlot + 1);
            else
                textdraw_printf(lbl_2_data_2F74[locale], data_select_menu_state.delSlot + 1);
            break;
        case 10:
            if (file->flags & 1)
                textdraw_print(lbl_2_data_2B00[locale]);
            else
                textdraw_print(lbl_2_data_2B5C[locale]);
            break;
    }
}
char *lbl_2_data_2FCC[6] = {
    "\x46\x72\x6F\x6D",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x73\x81\x5B\x82\xE0\x82\xC6",
};
char *lbl_2_data_2FF4[6] = {
    "\x54\x6F",
    "",
    "",
    "",
    "",
    "\x83\x52\x83\x73\x81\x5B\x82\xB3\x82\xAB",
};
char *lbl_2_data_300C[6] = {
    "\x44\x65\x6C\x65\x74\x65",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x82\xAF\x82\xB7",
};
char *lbl_2_data_3038[6] = {
    "\x4F\x70\x65\x6E",
    "",
    "",
    "",
    "",
    "\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7",
};
char *lbl_2_data_3064[6] = {
    "\x57\x6F\x72\x6C\x64",
    "",
    "",
    "",
    "",
    "\x83\x8F\x81\x5B\x83\x8B\x83\x68",
};
char *lbl_2_data_3094[6] = {
    "\x54\x69\x6D\x65",
    "",
    "",
    "",
    "",
    "\x83\x76\x83\x8C\x83\x43\x82\xB6\x82\xA9\x82\xF1",
};
char *lbl_2_data_30C0[6] = {
    "\x53\x63\x6F\x72\x65",
    "",
    "",
    "",
    "",
    "\x82\xC6\x82\xAD\x82\xC4\x82\xF1",
};

static inline int is_file_highlighted(int idx)
{
    if (data_select_menu_state.sub == 0 && data_select_menu_state.cursor == idx)
        return 1;
    switch (data_select_menu_state.state)
    {
        case 1:
        case 2:
        case 3:
        case 5:
            if (idx == data_select_menu_state.copySrc
             || (idx == data_select_menu_state.copyDst && data_select_menu_state.state != 1))
                return 1;
            break;
        case 6:
        case 7:
        case 9:
            if (idx == data_select_menu_state.delSlot)
                return 1;
            break;
    }
    return 0;
}

static void load_file_mtx(void)
{
    mtxa_peek();
    mtxa_translate_xyz(data_select_x, data_select_y, 0.0f);
    mtxa_scale_xyz(data_select_scale_x, data_select_scale_y, 1.0f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

static u32 get_color(void)
{
    return data_select_color;
}

void draw_storymode_save_file(struct StorySaveFile *file, int idx)
{
    float x;
    float y;
    float a;
    float b;
    char *str;

    x = -76.0f;
    y = -96.0f;
    load_file_mtx();
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(x, y);
    g_set_ui_element_sprite_flags(5);
    set_ui_element_sprite_alpha(data_select_alpha);
    set_ui_element_sprite_add_color_from_u32(get_color());
    if (is_file_highlighted(idx))
        set_ui_element_sprite_mult_color(0xFF, 0xFF, 0xFF, 0xFF);
    else
        set_ui_element_sprite_mult_color(0xFF, 0x7F, 0x7F, 0x7F);
    draw_ui_element(0x1800);
    textdraw_set_scale(1.25f, 1.25f);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0xFF603913);
    textdraw_set_add_color(data_select_color);
    textdraw_set_alpha(data_select_alpha);
    textdraw_set_pos(24.0 + x, 24.0 + y);
    textdraw_printf("%d", idx + 1);
    textdraw_set_scale(0.6666667f, 0.6666667f);
    textdraw_set_alignment(0);
    textdraw_set_drop_shadow();
    textdraw_set_drop_shadow_with_params(2, 2, 0.3 * data_select_alpha);
    textdraw_set_pos(48.0 + x, 17.0 + y);
    if (file->flags & 1)
    {
        if (is_file_highlighted(idx))
            textdraw_set_mul_color(0xFF223FE6);
        else
            textdraw_set_mul_color(0xFF111F73);
        str = file->name;
        a = 1.0f;
    }
    else
    {
        textdraw_set_mul_color(0xFF603913);
        str = lbl_2_data_3038[locale];
        a = 0.5f;
    }
    textdraw_set_alpha(a * data_select_alpha);
    textdraw_print(str);
    textdraw_clear_drop_shadow();
    textdraw_set_alpha(a * data_select_alpha);
    textdraw_set_mul_color(0xFF603913);
    textdraw_set_pos(25.0 + x, 50.0 + y);
    textdraw_print(lbl_2_data_3064[locale]);
    textdraw_set_pos(25.0 + x, 96.0 + y);
    textdraw_print(lbl_2_data_3094[locale]);
    textdraw_set_pos(25.0 + x, 142.0 + y);
    textdraw_print(lbl_2_data_30C0[locale]);
    if (file->flags & 1)
    {
        int t;
        int hour;
        int min;
        int sec;

        textdraw_set_alignment(6);
        textdraw_set_pos(141.0 + x, 50.0 + y);
        textdraw_printf("\x81\x63%d", file->world + 1);
        textdraw_set_scale(0.5833333f, 0.6666667f);
        textdraw_set_mul_color(0xFF005826);
        textdraw_set_pos(141.0 + x, 69.0 + y);
        {
        char *name;
        float w;

        name = fn_2_F06C(locale, file->world);
        w = g_call_get_string_sprite_width_3_discard_result(name);
        if (w > 116.0)
            textdraw_set_scale(0.58333333333333337 * (116.0 / w), 0.6666667f);
        textdraw_printf("%s", name);
        }
        textdraw_set_scale(0.5833333f, 0.6666667f);
        t = file->time;
        t /= 60;
        sec = t % 60;
        t /= 60;
        min = t % 60;
        t /= 60;
        hour = t;
        textdraw_set_pos(141.0 + x, 115.0 + y);
        textdraw_printf("%d:%02d:%02d", hour, min, sec);
        textdraw_set_pos(141.0 + x, 161.0 + y);
        textdraw_printf("%d", file->score);
    }
    switch (data_select_menu_state.state)
    {
        case 2:
        case 3:
            if (idx == data_select_menu_state.copyDst)
            {
                a = 80.0 + x;
                b = y - 8.0;
                init_ui_element_sprite_with_defaults();
                set_ui_element_sprite_pos(a, b);
                draw_ui_element(0x1801);
                textdraw_reset();
                textdraw_set_font(3);
                textdraw_set_alignment(4);
                textdraw_set_flags(0x80000000);
                textdraw_set_mul_color(0xFF603913);
                textdraw_set_scale(0.6666667f, 0.6666667f);
                textdraw_set_pos(a, b);
                textdraw_print(lbl_2_data_2FF4[locale]);
            }
        case 1:
            if (idx == data_select_menu_state.copySrc)
            {
                b = 80.0 + x;
                a = y - 8.0;
                init_ui_element_sprite_with_defaults();
                set_ui_element_sprite_pos(b, a);
                draw_ui_element(0x1801);
                textdraw_reset();
                textdraw_set_font(3);
                textdraw_set_alignment(4);
                textdraw_set_flags(0x80000000);
                textdraw_set_mul_color(0xFF603913);
                textdraw_set_scale(0.6666667f, 0.6666667f);
                textdraw_set_pos(b, a);
                textdraw_print(lbl_2_data_2FCC[locale]);
            }
            break;
        case 6:
        case 7:
            if (idx == data_select_menu_state.delSlot)
            {
                b = 80.0 + x;
                a = y - 8.0;
                init_ui_element_sprite_with_defaults();
                set_ui_element_sprite_pos(b, a);
                draw_ui_element(0x1801);
                textdraw_reset();
                textdraw_set_font(3);
                textdraw_set_alignment(4);
                textdraw_set_flags(0x80000000);
                textdraw_set_mul_color(0xFF603913);
                textdraw_set_scale(0.6666667f, 0.6666667f);
                textdraw_set_pos(b, a);
                textdraw_print(lbl_2_data_300C[locale]);
            }
            break;
    }
    mtxa_peek();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}
char *lbl_2_data_3114[6] = {
    "\x44\x61\x74\x61\x20\x53\x65\x6C\x65\x63\x74",
    "",
    "",
    "",
    "",
    "\x83\x66\x81\x5B\x83\x5E\x83\x5A\x83\x8C\x83\x4E\x83\x67",
};

static inline int is_action_highlighted(int idx)
{
    if (data_select_menu_state.sub == 1 && data_select_menu_state.cursor == idx)
        return 1;
    switch (data_select_menu_state.state)
    {
        case 1:
        case 2:
        case 3:
        case 5:
            if (idx == 0)
                return 1;
            break;
        case 6:
        case 7:
        case 9:
            if (idx == 1)
                return 1;
            break;
    }
    return 0;
}

static inline u32 make_color(u8 b, u8 g, u8 r)
{
    return r | ((g << 8) | (b << 16));
}

static inline void reset_data_select_view(void)
{
    data_select_x = 0.0f;
    data_select_y = 0.0f;
    data_select_scale_x = 1.0f;
    data_select_scale_y = 1.0f;
    data_select_color = 0;
    data_select_alpha = 1.0f;
}

void fn_2_1385C(struct Sprite *sprite)
{
    struct StorySaveFile *file;
    struct DataSelectEntry *entry;
    int empty;
    int i;
    float ty;
    float x;
    float y;

    GXSetZMode_cached(1, 3, 0);
    mtxa_from_identity();
    set_ui_widescreen_scale_mtx(320);
    mtxa_push();
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(320.0f, 70.0f);
    draw_ui_element(0x1808);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(-0x8000);
    textdraw_set_drop_shadow();
    textdraw_set_flags(0x80000000);
    textdraw_set_font_style(5);
    textdraw_set_pos(320.0f, 70.0f);
    textdraw_print(lbl_2_data_3114[locale]);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(320.0f, 380.0f);
    draw_ui_element(0x1807);
    fn_2_1251C(320.0f, 380.0f);
    empty = 1;
    for (i = 0; i < 3; i++)
    {
        if (storymode_save_files[i].flags & 1)
        {
            empty = 0;
            break;
        }
    }
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_flags(0x80000000);
    textdraw_set_mul_color(0xFF603913);
    textdraw_set_scale(0.6666667f, 0.6666667f);
    x = 502.0f;
    y = 110.0f;
    for (i = 0; i < 3; i++)
    {
        switch (i)
        {
            case 0:
            case 1:
                if (empty != 0)
                    continue;
                break;
        }
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_pos(x, y);
        g_set_ui_element_sprite_flags(5);
        if (is_action_highlighted(i))
            set_ui_element_sprite_mult_color(0xFF, 0xFF, 0xFF, 0xFF);
        else
            set_ui_element_sprite_mult_color(0xFF, 0x7F, 0x7F, 0x7F);
        draw_ui_element(0x1801);
        {
            float tx;
            double d;

            d = y + 0.5 * g_get_bmp_height(0x1801);
            ty = d;
            tx = x + 0.5 * g_get_bmp_width(0x1801);
            textdraw_set_pos(tx, ty);
        }
        textdraw_print(lbl_2_data_2AA0[i][locale]);
        y += 52.0;
    }
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(0);
    file = storymode_save_files;
    entry = data_select_menu_state.entries;
    for (i = 0; i < 3; i++, file++, entry++)
    {
        if (data_select_menu_state.state == 5 && i == data_select_menu_state.copyDst)
            continue;
        if (data_select_menu_state.state == 9 && i == data_select_menu_state.delSlot)
            continue;
        if (data_select_menu_state.state == 10 && i == data_select_menu_state.slot)
            continue;
        reset_data_select_view();
        data_select_x = 106.0 + 156.0 * i;
        data_select_y = 203.0f;
        draw_storymode_save_file(file, i);
    }
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    if (data_select_menu_state.state == 5)
    {
        float t;
        u8 b;
        int src = data_select_menu_state.copySrc;
        int dst = data_select_menu_state.copyDst;

        if (data_select_menu_state.timer < 60)
        {
            file = &storymode_save_files[dst];
            t = data_select_menu_state.timer / 60.0f;
            t = 1.0 - t;
            t = t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);
            reset_data_select_view();
            data_select_alpha = t;
            data_select_x = 106.0 + 156.0 * dst;
            data_select_y = 203.0f;
            draw_storymode_save_file(file, dst);
        }
        else if (data_select_menu_state.timer < 120)
        {
            file = &storymode_save_files[src];
            t = (data_select_menu_state.timer - 60) / 60.0f;
            t = t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);
            reset_data_select_view();
            data_select_alpha = t;
            data_select_x = 106.0 + 156.0 * (dst * t + src * (1.0 - t));
            data_select_y = 203.0f;
            draw_storymode_save_file(file, dst);
        }
        else
        {
            file = &storymode_save_files[src];
            t = (data_select_menu_state.timer - 120) / 60.0f;
            t = 2.0 * t - 1.0;
            t = __fabs(t);
            t = 1.0 - t;
            t *= 255.0;
            hud_f32_to_u8(&t, &b);
            reset_data_select_view();
            data_select_color = make_color(b, b, b);
            data_select_x = 106.0 + 156.0 * dst;
            data_select_y = 203.0f;
            draw_storymode_save_file(file, dst);
        }
    }
    else if (data_select_menu_state.state == 9)
    {
        float t;
        u8 b;
        int slot = data_select_menu_state.delSlot;

        file = &storymode_save_files[slot];
        reset_data_select_view();
        if (data_select_menu_state.timer < 60)
        {
            t = data_select_menu_state.timer / 60.0f;
            t = 1.0 - t;
            t = t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);
            data_select_alpha = t;
        }
        else if (data_select_menu_state.timer < 120)
        {
            t = (data_select_menu_state.timer - 60) / 60.0f;
            t = t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);
            data_select_alpha = t;
        }
        else
        {
            t = (data_select_menu_state.timer - 120) / 60.0f;
            t = 2.0 * t - 1.0;
            t = __fabs(t);
            t = 1.0 - t;
            t *= 255.0;
            hud_f32_to_u8(&t, &b);
            data_select_color = make_color(b, b, b);
        }
        data_select_x = 106.0 + 156.0 * slot;
        data_select_y = 203.0f;
        draw_storymode_save_file(file, slot);
    }
    else if (data_select_menu_state.state == 10)
    {
        float t;
        u8 b;
        float px;
        float py;
        float sy;
        float sx;
        int slot = data_select_menu_state.slot;

        file = &storymode_save_files[slot];
        reset_data_select_view();
        t = ((int)data_select_menu_state.timer % 15) / 15.0f;
        t = 1.0 - __fabs((float)(2.0 * t - 1.0));
        t = __fabs(t);
        t *= 127.5;
        hud_f32_to_u8(&t, &b);
        data_select_color = make_color(b, b, b);
        t = data_select_menu_state.timer > 60 ? (data_select_menu_state.timer - 60) / 60.0f : 0.0f;
        px = 320.0 * t + (106.0 + 156.0 * slot) * (1.0 - t);
        py = 203.0 * (1.0 - t) + 240.0 * t;
        data_select_x = px;
        data_select_y = py;
        sy = 1.0 / (1.001 - t);
        sx = 1.001 / (1.001 - t);
        data_select_scale_x = sx;
        data_select_scale_y = sy;
        draw_storymode_save_file(file, slot);
    }
    mtxa_pop();
    reset_ui_widescreen_scale_mtx();
    GXSetZMode_cached(1, 3, 1);
}

void g_related_to_loading_dataselect_menu(void)
{
    struct DataSelectEntry *entry;
    int i;
    struct Sprite *sprite;
    int group;

    memset(&data_select_menu_state, 0, sizeof(data_select_menu_state));
    entry = data_select_menu_state.entries;
    for (i = 0; i < 3; i++, entry++)
    {
        entry->destX = 76.0 + (30.0 + 156.0 * i);
        entry->destY = 203.0f;
        entry->x = 640.0 * (rand() / 32767.0f);
        entry->y = 480.0 * (rand() / 32767.0f);
        entry->velX = 20.0 * (rand() / 32767.0f) - 10.0;
        entry->velY = 20.0 * (rand() / 32767.0f) - 10.0;
        entry->scale = .1f;
        entry->destScale = .1f;
    }
    data_select_menu_state.unk90 = 0x50;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x28;
        sprite->depth = 1.0f;
        sprite->drawFunc = fn_2_1385C;
        strcpy(sprite->text, "LOAD GAME");
    }
    group = g_swapDiscQueueGroup(4);
    if (((u8 *)storymode_save_files)[0x31B] != 0)
        fn_1_EFFD4(0);
    queue_bmp_by_id_read(0xF);
    preload_ape(0, 0, 0, 1);
    g_swapDiscQueueGroup(group);
    g_change_music_volume(are_all_party_games_unlocked() ? 0x5A : 5, 0x3C, 0x64);
}

void dmd_scen_loadgame_main_child(void)
{
    u32 sound;
    int flags;
    int c;
    int i;
    int empty;
    struct MemcardSaveRequest req1;
    char buf1[32];
    struct MemcardSaveRequest req2;
    char buf2[32];

    sound = 0;
    flags = 0;
    switch (data_select_menu_state.state)
    {
    case 0:
        switch (data_select_menu_state.sub)
        {
        case 0:
            c = data_select_menu_state.cursor;
            if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
            {
                c--;
                if (c < 0)
                    c = 0;
            }
            if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
            {
                c++;
                if (c >= 3)
                {
                    sound |= 2;
                    data_select_menu_state.sub = 1;
                    empty = 1;
                    for (i = 0; i < 3; i++)
                    {
                        if (storymode_save_files[i].flags & 1)
                        {
                            empty = 0;
                            break;
                        }
                    }
                    if (empty)
                        data_select_menu_state.cursor = 2;
                    else
                        data_select_menu_state.cursor = 0;
                    break;
                }
            }
            if (c != data_select_menu_state.cursor)
            {
                sound |= 2;
                data_select_menu_state.cursor = (s8)c;
            }
            if (merged_digital_inputs[2] & PAD_BUTTON_A)
            {
                data_select_menu_state.slot = data_select_menu_state.cursor;
                sound |= 0x20;
                data_select_menu_state.state = 10;
                data_select_menu_state.timer = 0;
                if (storymode_save_files[data_select_menu_state.slot].flags & 1)
                    fn_2_8A40(&storymode_save_files[data_select_menu_state.slot]);
            }
            else if (merged_digital_inputs[2] & PAD_BUTTON_B)
            {
                sound |= 4;
                data_select_menu_state.state = 11;
            }
            break;
        case 1:
            empty = 1;
            for (i = 0; i < 3; i++)
            {
                if (storymode_save_files[i].flags & 1)
                {
                    empty = 0;
                    break;
                }
            }
            c = data_select_menu_state.cursor;
            if (empty)
                c = 2;
            else
            {
                if ((merged_digital_inputs[4] & PAD_BUTTON_UP) || (merged_analog_inputs[4] & PAD_BUTTON_UP))
                {
                    c--;
                    if (c < 0)
                        c = 0;
                }
                if ((merged_digital_inputs[4] & PAD_BUTTON_DOWN) || (merged_analog_inputs[4] & PAD_BUTTON_DOWN))
                {
                    c++;
                    if (c >= 3)
                        c = 2;
                }
            }
            if (c != data_select_menu_state.cursor)
            {
                sound |= 2;
                data_select_menu_state.cursor = (s8)c;
            }
            if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
            {
                sound |= 2;
                data_select_menu_state.sub = 0;
                data_select_menu_state.cursor = 2;
            }
            else if (merged_digital_inputs[2] & PAD_BUTTON_A)
            {
                switch (data_select_menu_state.cursor)
                {
            case 0:
                for (i = 0; i < 3; i++)
                {
                    if (storymode_save_files[i].flags & 1)
                        break;
                }
                if (i < 3)
                {
                    sound |= 1;
                    data_select_menu_state.state = 1;
                    data_select_menu_state.sub = 0;
                    data_select_menu_state.cursor = data_select_menu_state.copySrc = (s8)i;
                }
                break;
            case 1:
                for (i = 0; i < 3; i++)
                {
                    if (storymode_save_files[i].flags & 1)
                        break;
                }
                if (i < 3)
                {
                    sound |= 1;
                    data_select_menu_state.state = 6;
                    data_select_menu_state.sub = 0;
                    data_select_menu_state.cursor = data_select_menu_state.delSlot = (s8)i;
                }
                break;
            case 2:
                    sound |= 4;
                    data_select_menu_state.state = 11;
                    break;
                }
            }
            else if (merged_digital_inputs[2] & PAD_BUTTON_B)
            {
                sound |= 4;
                data_select_menu_state.state = 11;
            }
            break;
        }
        break;
    case 1:
        c = data_select_menu_state.cursor;
        if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
        {
            for (i = c - 1; i >= 0; i--)
            {
                if (storymode_save_files[i].flags & 1)
                    break;
            }
            if (i >= 0)
                c = i;
        }
        if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
        {
            for (i = c + 1; i < 3; i++)
            {
                if (storymode_save_files[i].flags & 1)
                    break;
            }
            if (i < 3)
                c = i;
        }
        if (c != data_select_menu_state.cursor)
        {
            sound |= 2;
            data_select_menu_state.cursor = data_select_menu_state.copySrc = (s8)c;
        }
        if (merged_digital_inputs[2] & PAD_BUTTON_A)
        {
            sound |= 1;
            data_select_menu_state.state = 2;
            c = data_select_menu_state.cursor;
            data_select_menu_state.cursor = data_select_menu_state.copyDst = (c == 0) ? 1 : 0;
        }
        else if (merged_digital_inputs[2] & PAD_BUTTON_B)
        {
            sound |= 4;
            data_select_menu_state.state = 0;
            data_select_menu_state.sub = 1;
            data_select_menu_state.cursor = 0;
        }
        break;
    case 2:
        c = data_select_menu_state.cursor;
        if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
        {
            do
            {
                c--;
            } while (c == data_select_menu_state.copySrc);
            if (c < 0)
                c = data_select_menu_state.cursor;
        }
        if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
        {
            do
            {
                c++;
            } while (c == data_select_menu_state.copySrc);
            if (c >= 3)
                c = data_select_menu_state.cursor;
        }
        if (c != data_select_menu_state.cursor)
        {
            sound |= 2;
            data_select_menu_state.cursor = data_select_menu_state.copyDst = (s8)c;
        }
        if (merged_digital_inputs[2] & PAD_BUTTON_A)
        {
            sound |= 1;
            data_select_menu_state.state = 3;
            data_select_menu_state.sub = 2;
            data_select_menu_state.copyDst = data_select_menu_state.cursor;
            data_select_menu_state.cursor = 0;
        }
        else if (merged_digital_inputs[2] & PAD_BUTTON_B)
        {
            sound |= 4;
            data_select_menu_state.state = 1;
            data_select_menu_state.cursor = data_select_menu_state.copySrc;
        }
        break;
    case 3:
        c = data_select_menu_state.cursor;
        if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
            c = 0;
        if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
            c = 1;
        if (c != data_select_menu_state.cursor)
        {
            sound |= 2;
            data_select_menu_state.cursor = (s8)c;
        }
        if (merged_digital_inputs[2] & PAD_BUTTON_A)
        {
            if (data_select_menu_state.cursor == 0)
                flags |= 1;
            else
                flags |= 2;
        }
        if (merged_digital_inputs[2] & PAD_BUTTON_B)
            flags |= 2;
        if (flags & 1)
        {
            data_select_menu_state.state = 5;
            data_select_menu_state.substate = 0;
        }
        else if (flags & 2)
        {
            data_select_menu_state.state = 4;
            data_select_menu_state.timer = 0;
        }
        break;
    case 4:
        if (data_select_menu_state.timer >= 60)
        {
            data_select_menu_state.state = 0;
            data_select_menu_state.sub = 1;
            data_select_menu_state.cursor = 0;
        }
        data_select_menu_state.timer++;
        break;
    case 5:
        switch (data_select_menu_state.substate)
        {
        case 0:
            data_select_menu_state.timer = 0;
            data_select_menu_state.substate++;
            break;
        case 1:
            data_select_menu_state.timer++;
            if (data_select_menu_state.timer == 60)
                memcpy(&storymode_save_files[data_select_menu_state.copyDst], &storymode_save_files[data_select_menu_state.copySrc], sizeof(storymode_save_files[0]));
            else if (data_select_menu_state.timer >= 180)
            {
                if (((u8 *)storymode_save_files)[0x31B] != 0)
                    data_select_menu_state.substate++;
                else
                    data_select_menu_state.substate = 4;
            }
            break;
        case 2:
            g_save_game_data();
            memset(&req1, 0, sizeof(req1));
            req1.unk0 = 0;
            g_sprintf_memcard_error(0, buf1);
            req1.msg = buf1;
            if (lbl_1_bss_CF3D8 != 0)
                req1.flags |= 0x20000000;
            req1.data = storymode_save_files;
            g_some_replay_func4(0, (u8)g_last_used_memcard_slot_2, &req1);
            data_select_menu_state.substate++;
            break;
        case 3:
            if (g_check_some_memcard_field() != 0)
                break;
            if (g_get_some_memcard_var() != 0)
                smd_game_over_save_child();
        case 4:
            data_select_menu_state.state = 0;
            data_select_menu_state.sub = 1;
            data_select_menu_state.cursor = 0;
            break;
        }
        break;
    case 6:
        c = data_select_menu_state.cursor;
        if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
        {
            for (i = c - 1; i >= 0; i--)
            {
                if (storymode_save_files[i].flags & 1)
                    break;
            }
            if (i >= 0)
                c = i;
        }
        if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
        {
            for (i = c + 1; i < 3; i++)
            {
                if (storymode_save_files[i].flags & 1)
                    break;
            }
            if (i < 3)
                c = i;
        }
        if (c != data_select_menu_state.cursor)
        {
            sound |= 2;
            data_select_menu_state.cursor = data_select_menu_state.delSlot = (s8)c;
        }
        if (merged_digital_inputs[2] & PAD_BUTTON_A)
        {
            sound |= 1;
            data_select_menu_state.state = 7;
            data_select_menu_state.sub = 2;
            data_select_menu_state.cursor = 0;
        }
        else if (merged_digital_inputs[2] & PAD_BUTTON_B)
        {
            sound |= 4;
            data_select_menu_state.state = 0;
            data_select_menu_state.sub = 1;
            data_select_menu_state.cursor = 1;
        }
        break;
    case 7:
        c = data_select_menu_state.cursor;
        if ((merged_digital_inputs[4] & PAD_BUTTON_LEFT) || (merged_analog_inputs[4] & PAD_BUTTON_LEFT))
            c = 0;
        if ((merged_digital_inputs[4] & PAD_BUTTON_RIGHT) || (merged_analog_inputs[4] & PAD_BUTTON_RIGHT))
            c = 1;
        if (c != data_select_menu_state.cursor)
        {
            sound |= 2;
            data_select_menu_state.cursor = (s8)c;
        }
        if (merged_digital_inputs[2] & PAD_BUTTON_A)
        {
            if (data_select_menu_state.cursor == 0)
                flags |= 1;
            else
                flags |= 2;
        }
        if (merged_digital_inputs[2] & PAD_BUTTON_B)
            flags |= 2;
        if (flags & 1)
        {
            data_select_menu_state.state = 9;
            data_select_menu_state.substate = 0;
        }
        else if (flags & 2)
        {
            data_select_menu_state.state = 8;
            data_select_menu_state.timer = 0;
        }
        break;
    case 8:
        if (data_select_menu_state.timer >= 60)
        {
            data_select_menu_state.state = 0;
            data_select_menu_state.sub = 1;
            data_select_menu_state.cursor = 1;
        }
        data_select_menu_state.timer++;
        break;
    case 9:
        switch (data_select_menu_state.substate)
        {
        case 0:
            data_select_menu_state.timer = 0;
            data_select_menu_state.substate++;
            break;
        case 1:
            data_select_menu_state.timer++;
            if (data_select_menu_state.timer == 60)
                memset(&storymode_save_files[data_select_menu_state.delSlot], 0, sizeof(storymode_save_files[0]));
            else if (data_select_menu_state.timer >= 180)
            {
                if (((u8 *)storymode_save_files)[0x31B] != 0)
                    data_select_menu_state.substate++;
                else
                    data_select_menu_state.substate = 4;
            }
            break;
        case 2:
            g_save_game_data();
            memset(&req2, 0, sizeof(req2));
            req2.unk0 = 0;
            g_sprintf_memcard_error(0, buf2);
            req2.msg = buf2;
            if (lbl_1_bss_CF3D8 != 0)
                req2.flags |= 0x20000000;
            req2.data = storymode_save_files;
            g_some_replay_func4(0, (u8)g_last_used_memcard_slot_2, &req2);
            data_select_menu_state.substate++;
            break;
        case 3:
            if (g_check_some_memcard_field() != 0)
                break;
            if (g_get_some_memcard_var() != 0)
                smd_game_over_save_child();
        case 4:
            data_select_menu_state.state = 0;
            data_select_menu_state.sub = 1;
            data_select_menu_state.cursor = 1;
            break;
        }
        break;
    case 10:
        if (data_select_menu_state.timer == 60)
        {
            fade_screen_to_color(0x101, 0, 0x3C);
            if (storymode_save_files[data_select_menu_state.slot].flags & 1)
                play_track_and_fade_out_other_tracks(0, 0x3C, 0x64);
        }
        else if (data_select_menu_state.timer >= 120)
        {
            if (!(storymode_save_files[data_select_menu_state.slot].flags & 1))
                data_select_menu_state.result = 1;
            else
                data_select_menu_state.result = 2;
        }
        data_select_menu_state.timer++;
        break;
    case 11:
        data_select_menu_state.timer = 0x1E;
        data_select_menu_state.state = 12;
        fade_screen_to_color(0x101, 0, data_select_menu_state.timer);
        g_change_music_volume(-1, data_select_menu_state.timer, 1);
        break;
    case 12:
        data_select_menu_state.timer--;
        if (data_select_menu_state.timer == 0)
            data_select_menu_state.result = 3;
        break;
    }
    if (sound & 1)
        call_SoundReqID_arg_0(0x6A);
    else if (sound & 2)
        call_SoundReqID_arg_0(0x6C);
    else if (sound & 4)
        call_SoundReqID_arg_0(0x6B);
    else if (sound & 8)
        call_SoundReqID_arg_0(0x65);
    else if (sound & 0x10)
        call_SoundReqID_arg_0(0x66);
    else if (sound & 0x20)
        call_SoundReqID_arg_0(0x16);
    data_select_menu_state.frame++;
}

void fn_2_15CC4(void)
{
    destroy_sprite_with_unique_id(0x28);
}

int get_storymode_menu_state(void)
{
    return data_select_menu_state.result;
}

int fn_2_15CFC(void)
{
    return data_select_menu_state.slot;
}
