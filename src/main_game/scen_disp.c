/* Story mode stage select screen, stage preview ball and NOW LOADING text. */
#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "course.h"
#include "event.h"
#include "frustum.h"
#include "gx.h"
#include "input.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "os.h"
#include "rand.h"
#include "scen.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "ape_internal.h"

extern s32 locale;

#define RAND_FLOAT() (rand() / 32767.0f)
#define CLAMP(a, lo, hi) ((a) < (lo) ? (lo) : ((a) > (hi) ? (hi) : (a)))

struct StageSelBall
{
    /*0x00*/ s32 beaten;
    /*0x04*/ float unk4;
    /*0x08*/ s16 rotX;
    /*0x0A*/ s16 rotY;
    /*0x0C*/ s16 rotZ;
    /*0x0E*/ s16 velX;
    /*0x10*/ s16 velY;
    /*0x12*/ s16 velZ;
}; /* size = 0x14 */

struct StageSelApe
{
    /*0x00*/ struct Ape *ape;
    /*0x04*/ float x;
    /*0x08*/ float y;
};

struct StageSel
{
    /*0x00*/ s16 state;
    /*0x02*/ s16 timer;
    /*0x04*/ float x;
    /*0x08*/ float y;
    /*0x0C*/ float vx;
    /*0x10*/ float vy;
    /*0x14*/ float ax;
    /*0x18*/ float ay;
    /*0x1C*/ s16 rot;
    /*0x1E*/ s16 rotVel;
    /*0x20*/ s32 dir;
    /*0x24*/ float rotOff;
    /*0x28*/ u32 flags;
    /*0x2C*/ float scale;
    /*0x30*/ struct StageSelBall balls[10];
    /*0xF8*/ struct StageSelApe apeInfo;
    u8 pad104[0x10C - 0x104];
}; /* size = 0x10C */

struct ClearEntry
{
    /*0x00*/ s16 id;
    /*0x02*/ s16 delay;
    /*0x04*/ float unk4;
    /*0x08*/ float scale;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    u8 pad12[2];
}; /* size = 0x14 */

struct ClearInfo
{
    /*0x00*/ s32 count;
    /*0x04*/ struct ClearEntry entries[10];
}; /* size = 0xCC */

struct SlideAnim
{
    /*0x00*/ s16 state;
    /*0x02*/ s16 timer;
    /*0x04*/ float offset;
};

struct PreviewBall
{
    /*0x00*/ s16 state;
    /*0x02*/ s16 timer;
    /*0x04*/ float x;
    /*0x08*/ float y;
    /*0x0C*/ s16 unkC;
    u8 padE[2];
    /*0x10*/ Vec pos;
    /*0x1C*/ float vel;
    u8 pad20[8];
    /*0x28*/ float size;
    /*0x2C*/ float rotX;
    /*0x30*/ float rotY;
    /*0x34*/ float rotZ;
    u8 pad38[0xC];
    /*0x44*/ s32 stage;
    /*0x48*/ float unk48;
    /*0x4C*/ float unk4C;
    /*0x50*/ float unk50;
    /*0x54*/ float unk54;
    /*0x58*/ float unk58;
    /*0x5C*/ s16 unk5C;
    /*0x60*/ float unk60;
    /*0x64*/ float unk64;
    /*0x68*/ float unk68;
    /*0x6C*/ s32 unbeaten;
}; /* size = 0x70 */

struct PreviewTex
{
    /*0x00*/ u32 flags;
    /*0x04*/ u8 tex[0x7C - 4];
}; /* size = 0x7C */

struct StorySelect
{
    /*0x000*/ s32 visible;
    /*0x004*/ s16 stageCount;
    /*0x006*/ s16 unbeatenCount;
    /*0x008*/ struct SlideAnim anim;
    /*0x010*/ struct StageSel sel;
    /*0x11C*/ struct ClearInfo clear;
    /*0x1E8*/ struct PreviewBall preview;
    /*0x258*/ struct PreviewTex previews[10];
}; /* size = 0x730 */

struct LoadingChar
{
    /*0x00*/ s32 on;
    /*0x04*/ float angle;
    /*0x08*/ float scaleA;
    /*0x0C*/ float scaleB;
    /*0x10*/ float x;
    /*0x14*/ float y;
    /*0x18*/ float y2;
    /*0x1C*/ float vy;
    /*0x20*/ float unk20;
    /*0x24*/ s32 bmpId;
    /*0x28*/ s32 nextBmpId;
}; /* size = 0x2C */

struct NowLoading
{
    /*0x00*/ s32 active;
    /*0x04*/ s32 counter;
    /*0x08*/ s32 timer;
    /*0x0C*/ struct LoadingChar chars[24];
    u8 pad42C[4];
}; /* size = 0x430 */

struct PreviewParams
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ float unk4;
};

struct Point2 { float x, y; };

struct PreviewArg
{
    s16 unk0;
    s16 unk2;
};

extern int main_heap;
extern void *lbl_801988E0;

int g_get_scen_stgname_buffer_size(void);
void g_queue_stage_name_load(void);
void g_read_storymode_select_stage_names_from_dvd(char **buffer);
struct Ape *g_load_ape_gameplay_variant0(int, int);
void fn_1_1132D0(struct Ape *ape, int a, int b, int c);
void fn_1_4E504(void *mtx, int a, int b);
void draw_storysel_ball_clear_half(void *mtx, int a);
void fn_1_4DDEC(void *mtx, int a, int b);
void fn_1_4E2FC(void *mtx, int a, int b);
void fn_1_4E12C(void *mtx, int a);
void g_stores_doubles2(float a, float b, float c, float d);
void g_math_unk13(Vec *eye, Vec *up, Vec *target);
void g_smth_with_projection_mtx(float *proj);
void g_load_preview_texture(void *tex, char *file, int id, int w, int h, int fmt);
void fn_1_F9E1C(void *tex);
void fn_1_F9CBC(void *tex);
void fn_1_F9EAC(void *tex, float a, float b, float c, float d, float e, int f, int g);
void g_draw_stobjs(void);
void g_draw_stage(void);
void GXCopyTex(void *dest, u8 clear);
void GXGetProjectionv(float *proj);
void GXGetViewportv(float *vp);
void GXInitTexObj(void *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInvalidateTexAll(void);
void GXLoadTexObj_cached(void *obj, int map);
void GXPixModeSync(void);
void GXSetCullMode_cached(int mode);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetNumChans_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_something_with_GX_vtx_desc(u32 attrs);
void g_something_with_raster_state(int);
void call_SoundReqID_arg_0(int soundId);
void draw_ape(struct Ape *ape);
void memcpy2(void *dest, void *src, u32 size);
void mtxa_to_quat(Quat *q);
void ord_tbl_draw_nodes(void);
void get_curr_stage_fly_in_position(struct Sphere *);
#ifdef __MWERKS__
u32 strlen(const char *s);
#else
#include <string.h> /* size_t-correct prototypes (64-bit PC) */

#ifndef __MWERKS__
int pc_text_is_lead(unsigned char c); /* PC: language-aware SJIS lead test (textdraw.c) */
#endif
#endif
float g_get_string_sprite_width_2(char *str);
int g_smth_with_fonts_chara_load_wrapper_discard_result(char *str);
float textdraw_get_pixel_width_of_string(char *str);
float textdraw_get_pixel_width_of_string_as_f32(char *str);
void textdraw_print(char *text);
void textdraw_printf(char *format, ...);
void textdraw_reset(void);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_font_style(int style);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
BOOL fn_2_106FC(void);
int fn_2_FED8(void);

void g_init_scen_stage_name_buffer(void);
void g_free_scen_stage_name_buffer(void);
void fn_2_A9EC(void);
void fn_2_A9F0(void);
void fn_2_AA20(void);
void g_create_storymode_select_sprites(int t);
void fn_2_AD74(void);
void fn_2_ADA8(void);
void g_related_to_loading_story_stageselect(int t);
void sprite_story_stage_select_tick(s8 *status, struct Sprite *sprite);
void sprite_story_stage_select_disp(struct Sprite *sprite);
void fn_2_BD58(void);
void g_handle_story_clear_stage_balls(int t);
void sprite_clear_floors_tick(s8 *status, struct Sprite *sprite);
void sprite_clear_floors_disp(struct Sprite *sprite);
void sprite_decoration_bar_tick(s8 *status, struct Sprite *sprite);
void sprite_decoration_bar_disp(struct Sprite *sprite);
void sprite_world_tick(s8 *status, struct Sprite *sprite);
void sprite_world_disp(struct Sprite *sprite);
void g_animate_story_select_sprites(int t);
void g_sprite_world_info_tick(s8 *status, struct Sprite *sprite);
void g_sprite_world_info_disp(struct Sprite *sprite);
void sprite_scen_stagesel_stage_name_tick(s8 *status, struct Sprite *sprite);
void sprite_scen_stagesel_stage_name_disp(struct Sprite *sprite);
void sprite_story_difficulty_tick(s8 *status, struct Sprite *sprite);
void sprite_story_difficulty_disp(struct Sprite *sprite);
void g_draw_story_stage_preview_ball(int t);
void g_sprite_story_stage_info_tick(s8 *status, struct Sprite *sprite);
void g_sprite_story_stage_info_disp(struct Sprite *sprite);
void g_preload_all_story_preview_images(void);
void fn_2_E198(void);
void fn_2_E20C(int stage);
void fn_2_E2E4(struct PreviewArg *arg);
void fn_2_E3B4(void);
void fn_2_E418(u8 *color, int fmt);
void g_draw_now_loading_text(void);
void fn_2_EA58(int unused);
void fn_2_EB6C(void);
void fn_2_EBBC(s8 *status, struct Sprite *sprite);
char *fn_2_F06C(int loc, int world);

u32 lbl_2_data_10B8[] = {
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

struct PreviewParams MAIN_GAME_STORY_STAGE_PREVIEW_PARAMS[100] = {
    { 0xC9, 4096, 1.0f },
    { 0xCA, 4096, 1.0f },
    { 0xCB, 4096, 1.0f },
    { 0xCC, 4096, 1.0f },
    { 0x1, 4096, 1.0f },
    { 0x2, 4096, 1.0f },
    { 0x3, 4096, 1.0f },
    { 0x4, 6144, 1.0f },
    { 0x5, 4096, 1.0f },
    { 0x6, 4096, 1.0f },
    { 0x7, 6144, 1.0f },
    { 0x8, 6144, 1.0f },
    { 0x9, 6144, 1.0f },
    { 0xA, 6144, 1.0f },
    { 0xB, 6144, 1.0f },
    { 0xC, 6144, 1.0f },
    { 0xD, 6144, 1.0f },
    { 0xE, 6144, 1.0f },
    { 0xF, 6144, 1.0f },
    { 0x10, 6144, 1.0f },
    { 0xE7, 4096, 1.0f },
    { 0xE8, 6144, 1.0f },
    { 0xE9, 6144, 1.0f },
    { 0xEA, 6144, 1.0f },
    { 0xEB, 4096, 1.0f },
    { 0xEC, 6144, 1.0f },
    { 0xED, 4096, 1.0f },
    { 0xEE, 4096, 1.0f },
    { 0xEF, 4096, 1.0f },
    { 0x11, 6144, 1.0f },
    { 0x12, 4096, 1.0f },
    { 0x13, 6144, 1.0f },
    { 0x14, 6144, 1.0f },
    { 0x15, 4096, 1.0f },
    { 0x16, 4096, 1.0f },
    { 0x17, 6144, 1.0f },
    { 0x18, 6144, 1.0f },
    { 0x19, 6144, 1.0f },
    { 0x1A, 6144, 1.0f },
    { 0x1B, 6144, 1.0f },
    { 0x1C, 6144, 1.0f },
    { 0x1D, 6144, 1.0f },
    { 0x1E, 4096, 1.0f },
    { 0x1F, 6144, 1.0f },
    { 0x20, 4096, 1.0f },
    { 0x21, 10240, 1.0f },
    { 0x22, 6144, 1.0f },
    { 0x23, 6144, 1.0f },
    { 0x24, 4096, 1.0f },
    { 0x25, 4096, 1.0f },
    { 0x26, 0, 1.0f },
    { 0x27, 6144, 1.0f },
    { 0x28, 6144, 1.0f },
    { 0x29, 6144, 1.0f },
    { 0x2A, 6144, 1.0f },
    { 0x2B, 2048, 1.0f },
    { 0x2C, 4096, 1.0f },
    { 0x2D, 6144, 1.0f },
    { 0x2E, 6144, 1.0f },
    { 0x2F, 6144, 1.0f },
    { 0x119, 4096, 1.0f },
    { 0x11A, 6144, 1.0f },
    { 0x11B, 4096, 1.0f },
    { 0x11C, 6144, 1.0f },
    { 0x11D, 6144, 1.0f },
    { 0x11E, 4096, 1.0f },
    { 0x11F, 4096, 1.0f },
    { 0x120, 6144, 1.0f },
    { 0x121, 6144, 1.0f },
    { 0x30, 4096, 1.0f },
    { 0x31, 5120, 1.0f },
    { 0x32, 5120, 1.0f },
    { 0x33, 6144, 1.0f },
    { 0x34, -24576, 0.300000012f },
    { 0x35, 5120, 1.0f },
    { 0x36, 6144, 1.0f },
    { 0x37, 6144, 1.0f },
    { 0x38, 4608, 1.0f },
    { 0x39, 6144, 1.0f },
    { 0x3A, 5120, 1.0f },
    { 0x3B, 6144, 1.0f },
    { 0x3C, 6144, 1.0f },
    { 0x3D, 4096, 1.0f },
    { 0x3E, 4096, 1.0f },
    { 0x3F, 4096, 1.0f },
    { 0x40, 6144, 1.0f },
    { 0x41, 6144, 1.0f },
    { 0x42, 4096, 1.0f },
    { 0x43, 5120, 1.0f },
    { 0x44, 6144, 1.0f },
    { 0x155, 6144, 1.0f },
    { 0x156, 6144, 1.0f },
    { 0x157, 6144, 1.0f },
    { 0x158, 6144, 1.0f },
    { 0x159, 5120, 1.0f },
    { 0x15A, 6144, 1.0f },
    { 0x15B, 6144, 1.0f },
    { 0x15C, 6144, 1.0f },
    { 0x15D, 4096, 1.0f },
    { 0x15E, 6144, 1.0f },
};

char *lbl_2_data_16C0[10][6] = {
    { "Jungle Island", "---", "---", "---", "---", "\x83\x57\x83\x83\x83\x93\x83\x4F\x83\x8B\x82\xB6\x82\xDC" },
    { "Volcanic Magma", "---", "---", "---", "---", "\x82\xAB\x82\xE3\x82\xA4\x82\xA9\x82\xB4\x82\xF1\x82\xCC\x20\x83\x7D\x83\x4F\x83\x7D" },
    { "Under the Ocean", "---", "---", "---", "---", "\x82\xA4\x82\xDD\x82\xCC\x20\x82\xBB\x82\xB1" },
    { "Inside a Whale", "---", "---", "---", "---", "\x82\xAB\x82\xE5\x82\xBE\x82\xA2\x83\x4E\x83\x57\x83\x89\x82\xCC\x20\x82\xA8\x82\xC8\x82\xA9" },
    { "Amusement Park", "---", "---", "---", "---", "\x82\xBD\x82\xCC\x82\xB5\x82\xA2\x20\x82\xE4\x82\xA4\x82\xA6\x82\xF1\x82\xBF" },
    { "Boiling Pot", "---", "---", "---", "---", "\x82\xA8\x82\xC8\x82\xD7\x20\x83\x4F\x83\x63\x83\x4F\x83\x63" },
    { "Bubbly Washing Machine", "---", "---", "---", "---", "\x83\x41\x83\x8F\x83\x41\x83\x8F\x20\x82\xB9\x82\xF1\x82\xBD\x82\xAD\x82\xAB" },
    { "Clock Tower Factory", "---", "---", "---", "---", "\x82\xC6\x82\xAF\x82\xA2\x82\xBE\x82\xA2\x82\xCC\x20\x82\xB1\x82\xA4\x82\xB6\x82\xE5\x82\xA4" },
    { "Space Colony", "---", "---", "---", "---", "\x83\x58\x83\x79\x81\x5B\x83\x58\x83\x52\x83\x8D\x83\x6A\x81\x5B" },
    { "Dr. BAD-BOON's Base", "---", "---", "---", "---", "\x83\x7D\x83\x62\x83\x68\x83\x71\x83\x71\x82\xCD\x82\xA9\x82\xB9\x82\xCC\x20\x82\xAB\x82\xBF" },
};

struct Point2 lbl_2_data_17B0[10] = {
    { 60.0f, 178.0f },
    { 126.0f, 161.0f },
    { 191.0f, 142.0f },
    { 257.0f, 126.0f },
    { 109.0f, 228.0f },
    { 174.0f, 210.0f },
    { 240.0f, 193.0f },
    { 157.0f, 277.0f },
    { 223.0f, 259.0f },
    { 205.0f, 326.0f },
};

u8 ape_story_select_anim_table[8] = { 0, 2, 4, 6, 8 };

char **scen_stgname_buffer;
s32 g_scen_stage_names_loaded;
struct StorySelect g_are_story_select_sprites_visible;
struct NowLoading lbl_2_bss_788;

void g_init_scen_stage_name_buffer(void)
{
    scen_stgname_buffer = alloc_from_heap_or_panic(main_heap, g_get_scen_stgname_buffer_size(), "scen_disp.c", 0x1b9);
    g_scen_stage_names_loaded = 0;
    g_queue_stage_name_load();
    fn_2_A9F0();
}

void g_free_scen_stage_name_buffer(void)
{
    fn_2_AA20();
    g_scen_stage_names_loaded = 0;
    if (scen_stgname_buffer != NULL)
    {
        OSFreeToHeap(main_heap, scen_stgname_buffer);
        scen_stgname_buffer = NULL;
    }
}

void fn_2_A9EC(void) {}

void fn_2_A9F0(void)
{
    memset(&g_are_story_select_sprites_visible, 0, sizeof(g_are_story_select_sprites_visible));
}

void fn_2_AA20(void) {}

void g_create_storymode_select_sprites(int t)
{
    struct Sprite *sprite;
    int world;

    g_preload_all_story_preview_images();
    world = scen_info.world;
    g_are_story_select_sprites_visible.visible = 1;
    g_are_story_select_sprites_visible.stageCount = (s16)get_world_stage_count(world);
    g_are_story_select_sprites_visible.unbeatenCount = (s16)get_world_unbeaten_stage_count(world);
    g_animate_story_select_sprites(t);
    g_related_to_loading_story_stageselect(t);
    g_handle_story_clear_stage_balls(t);
    g_draw_story_stage_preview_ball(t);
    textdraw_reset();
    textdraw_set_font(3);

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 414.0f;
        sprite->y = 56.0f;
        sprite->mainFunc = sprite_world_tick;
        sprite->drawFunc = sprite_world_disp;
        sprintf(sprite->text, "WORLD");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->depth = 128.0f;
        sprite->mainFunc = sprite_story_stage_select_tick;
        sprite->drawFunc = sprite_story_stage_select_disp;
        sprintf(sprite->text, "FLOOR SELECT");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->depth = 128.0f;
        sprite->mainFunc = sprite_clear_floors_tick;
        sprite->drawFunc = sprite_clear_floors_disp;
        sprintf(sprite->text, "CLEAR FLOORS");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->depth = 224.0f;
        sprite->userVar = 0;
        sprite->flags |= 0x40000;
        sprite->mainFunc = sprite_decoration_bar_tick;
        sprite->drawFunc = sprite_decoration_bar_disp;
        sprintf(sprite->text, "DECORATION BAR UP");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->depth = 128.1f;
        sprite->userVar = 1;
        sprite->mainFunc = sprite_decoration_bar_tick;
        sprite->drawFunc = sprite_decoration_bar_disp;
        sprintf(sprite->text, "DECORATION BAR DOWN");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 132.0f;
        sprite->y = 56.0f;
        sprite->mainFunc = g_sprite_world_info_tick;
        sprite->drawFunc = g_sprite_world_info_disp;
        sprintf(sprite->text, "TTITLE");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 430.0f;
        sprite->y = 305.0f;
        sprite->fontId = 3;
        sprite->flags = 0xA0000000;
        sprite->textAlign = 4;
        sprite->counter = 0;
        sprite->userVar = 1;
        sprite->unk50 = scen_info.unk6;
        sprite->mainFunc = sprite_scen_stagesel_stage_name_tick;
        sprite->drawFunc = sprite_scen_stagesel_stage_name_disp;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 322.0f;
        sprite->y = 412.0f;
        sprite->fontId = 3;
        sprite->flags = 0xC0000000;
        sprite->textAlign = 4;
        sprite->mainFunc = sprite_story_difficulty_tick;
        sprite->drawFunc = sprite_story_difficulty_disp;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->depth = 160.0f;
        sprite->flags |= 0x40000;
        sprite->mainFunc = g_sprite_story_stage_info_tick;
        sprite->drawFunc = g_sprite_story_stage_info_disp;
    }
}

void fn_2_AD74(void)
{
    g_are_story_select_sprites_visible.visible = 0;
    fn_2_E198();
    fn_2_BD58();
}

void fn_2_ADA8(void)
{
    g_are_story_select_sprites_visible.visible = 1;
}

void g_related_to_loading_story_stageselect(int t)
{
    float a = 0.8f;
    float b = 1.4633332f;
    float c = -0.005729167f;
    float d = 0.0f;
    int i;
    struct StageSel *sel = &g_are_story_select_sprites_visible.sel;
    struct StageSelBall *ball;
    int world = scen_info.world;
    struct StageSelApe *ape;
    float ft;
    float inv;

    sel->state = 1;
    sel->timer = (s16)t;
    sel->x = 320.0f;
    sel->y = 240.0f;
    sel->vx = 16.0f * (1.0f + 0.5f * RAND_FLOAT());
    sel->vy = 48.0f * (RAND_FLOAT() - 0.5f);
    ft = t;
    inv = 1.0f / (ft * ft);
    sel->ax = inv * (2.0f * (182.0f - sel->x - sel->vx * ft));
    sel->ay = inv * (2.0f * (239.0f - sel->y - sel->vy * ft));
    sel->rot = 0;
    sel->rotVel = 2560.0f * (1.0f + 0.2f * RAND_FLOAT());
    if (RAND_FLOAT() < 0.2f)
        sel->rotVel = (s16)-sel->rotVel;
    sel->scale = 0.0f;
    sel->flags = 0;
    ball = sel->balls;
    for (i = 0; i < g_are_story_select_sprites_visible.stageCount; i++, ball++)
    {
        if (is_storymode_stage_beaten(world, i))
            ball->beaten = 1;
        else
            ball->beaten = 0;
        ball->unk4 = 1.0f;
        ball->rotX = (s16)((rand() & 0x7FFF) - 0x7FFF);
        ball->rotY = (s16)((rand() & 0x7FFF) - 0x7FFF);
        ball->rotZ = (s16)((rand() & 0x7FFF) - 0x7FFF);
        ball->velX = 0;
        ball->velY = 0;
        ball->velZ = 0;
    }
    ape = &sel->apeInfo;
    ape->ape = g_load_ape_gameplay_variant0(0, 0);
}

void sprite_story_stage_select_tick(s8 *status, struct Sprite *sprite)
{
    int i;
    struct StageSel *sel = &g_are_story_select_sprites_visible.sel;
    struct StageSelBall *ball;
    struct StageSelApe *ape;
    struct Point2 p;
    float f;

    switch (sel->state)
    {
    case 1:
        sel->state = 2;
        call_SoundReqID_arg_0(0x318);
        /* fall through */
    case 2:
        sel->rot = (s16)(sel->timer * sel->rotVel);
        sel->rotOff = 640.0f * sel->timer / 30.0f;
        sel->vx += sel->ax;
        sel->vy += sel->ay;
        sel->x += sel->vx;
        sel->y += sel->vy;
        sel->timer--;
        if (sel->timer > 0)
            break;
        sel->state = 3;
        sel->x = 182.0f;
        sel->y = 239.0f;
        break;
    case 3:
        sel->state = 4;
        sel->flags |= 1;
        sel->rot = 0;
        sel->rotVel = -0.6f * sel->rotVel;
        if (sel->rotVel < 0)
            sel->dir = 1;
        else
            sel->dir = 0;
        sel->rotOff = 0.0f;
        sel->x = 182.0f;
        sel->y = 239.0f;
        f = 8.0f * sel->rotVel;
        ball = sel->balls;
        for (i = 0; i < g_are_story_select_sprites_visible.stageCount; i++, ball++)
        {
            ball->velX += (s16)(f * (RAND_FLOAT() - 0.5f));
            ball->velY += (s16)(f * (RAND_FLOAT() - 0.5f));
            ball->velZ += (s16)(f * (RAND_FLOAT() - 0.5f));
        }
        /* fall through */
    case 4:
        if (sel->dir)
        {
            sel->rotVel = 0.98f * sel->rotVel;
            sel->rotVel += 0x60;
            sel->rot += sel->rotVel;
            if (sel->rot > 0)
            {
                sel->rot = 0;
                if (sel->rotVel > 0)
                    sel->rotVel = -0.7f * sel->rotVel;
            }
        }
        else
        {
            sel->rotVel -= 0x60;
            sel->rot += sel->rotVel;
            if (sel->rot <= 0)
            {
                sel->rot = 0;
                if (sel->rotVel < 0)
                    sel->rotVel = -0.7f * sel->rotVel;
            }
        }
        if (scen_info.flags & 8)
            sel->state = 5;
        break;
    case 5:
        sel->state = 6;
        sel->flags &= ~1;
        sel->rotVel += (s16)(2048.0f * (1.0f + RAND_FLOAT()));
        sel->vx = -8.0f * (1.0f + RAND_FLOAT());
        sel->vy = -8.0f * (1.0f + RAND_FLOAT());
        /* fall through */
    case 6:
        sel->vx *= 0.98f;
        sel->vy *= 0.98f;
        sel->vy += 0.8f;
        sel->x += sel->vx;
        sel->y += sel->vy;
        sel->rotVel = 0.98f * sel->rotVel;
        sel->rot += sel->rotVel;
        sel->rotOff += 21.333334f;
        break;
    case 7:
        sel->state = 0;
        break;
    case 0:
        break;
    }

    if (sel->scale < 1.0f)
    {
        sel->scale += 0.05f * (1.05f - sel->scale);
        if (sel->scale > 1.0f)
            sel->scale = 1.0f;
    }

    ball = sel->balls;
    for (i = 0; i < g_are_story_select_sprites_visible.stageCount; i++, ball++)
    {
        /* float -> s32 -> s8: the GC (fctiwz + extsb) and GCC wrap the
         * out-of-range values; a direct float -> s8 cast is UB in C and clang
         * drops the 8-bit wrap (x64 stage-select balls spun differently). */
        ball->velY = (s8)(s32)(0.98f * ball->velY);
        ball->velZ = (s8)(s32)(0.98f * ball->velZ);
        if (i == scen_info.unk6)
            ball->velX += (s16)(0.15f * (4096.0f - ball->velX));
        else
            ball->velX = 0.96f * ball->velX;
        ball->rotX += ball->velX;
        ball->rotY += ball->velY;
        ball->rotZ += ball->velZ;
    }

    ape = &sel->apeInfo;
    p = lbl_2_data_17B0[scen_info.unk6];
    ape->x += 0.3f * (p.x - ape->x);
    ape->y += 0.3f * (p.y - ape->y);
    if (ape->ape != NULL)
    {
        fn_1_1132D0(ape->ape, 0, 7, ape_story_select_anim_table[(u32)scen_info.unk6 % 5]);
        g_ape_anim_body(ape->ape);
    }
}

void sprite_story_stage_select_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    Vec v;
    struct StageSel *sel = &g_are_story_select_sprites_visible.sel;
    struct Ape *ape;
    int i;
    struct Point2 *p;
    int rot = sel->rot;
    float scale = sel->scale;
    struct StageSelBall *ball;
    struct StageSelApe *info;

    req.bmpId = 0xF08;
    req.x = sel->x;
    req.y = sel->y;
    req.z = 0.01f + sprite->depth;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = scale;
    req.scaleY = scale;
    req.rotation = (s16)rot + 0xAAA;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    draw_sprite_draw_request(&req);

    mtxa_from_translate_xyz(sel->x, sel->y, 0.0f);
    mtxa_rotate_z((s16)-rot);
    mtxa_scale_s(scale);
    mtxa_translate_xyz(-182.0f, -239.0f, 0.0f);

    v.z = 0.0f;
    req.bmpId = 0xF05;
    req.z = sprite->depth;
    req.rotation = 0;
    p = lbl_2_data_17B0;
    for (i = 10; i > 0; i--, p++)
    {
        v.x = p->x;
        v.y = p->y;
        mtxa_tf_point(&v, &v);
        req.x = v.x;
        req.y = v.y;
        mtxa_push();
        draw_sprite_draw_request(&req);
        mtxa_pop();
    }

    g_set_light_color_maybe(8, 0.8f, 0.8f, 0.8f);
    mtxa_push();
    mtxa_from_identity();
    mtxa_scale_xyz(1.0f, -1.0f, 1.0f);
    g_set_active_light_group(8);
    mtxa_pop();

    ball = sel->balls;
    p = lbl_2_data_17B0;
    for (i = 0; i < 10; i++, p++, ball++)
    {
        if (ball->beaten == 0)
        {
            v.x = p->x;
            v.y = p->y;
            v.z = 0.0f;
            mtxa_tf_point(&v, &v);
            v.z = -(sprite->depth - 28.0f);
            mtxa_push();
            mtxa_from_translate(&v);
            mtxa_scale_s(-56.0f);
            mtxa_rotate_y(ball->rotY);
            mtxa_rotate_x(ball->rotX);
            mtxa_rotate_z(ball->rotZ);
            mtxa_scale_s(scale);
            mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &v);
            if (v.z > 0.0f)
            {
                g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.3f);
                fn_1_4E504(mtxa, i, 0);
                g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.025f);
                draw_storysel_ball_clear_half(mtxa, 0);
            }
            else
            {
                g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.025f);
                draw_storysel_ball_clear_half(mtxa, 0);
                g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.3f);
                fn_1_4E504(mtxa, i, 0);
            }
            mtxa_pop();
        }
    }
    g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);

    info = &sel->apeInfo;
    if (info->ape != NULL)
    {
        ape = info->ape;
        v.x = info->x;
        v.y = info->y;
        v.z = 0.0f;
        mtxa_tf_point(&v, &v);
        v.z = -(sprite->depth - 98.0f);
        mtxa_from_translate(&v);
        mtxa_to_quat(&ape->rot);
        mathutil_mtxA_get_translate(&ape->pos);
        ape->scale = -84.0f * scale;
        mtxa_push();
        mtxa_from_mtxb();
        mtxa_push();
        mtxa_from_identity();
        mtxa_to_mtx(mtxa->mtxB);
        ape->flags &= ~0x100000;
        ape->flags |= 0x4000000;
        draw_ape(ape);
        mtxa_pop();
        mtxa_pop();
    }

    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    g_some_sprite_reset_gx_state_func();
}

void fn_2_BD58(void)
{
    struct StageSel *sel = &g_are_story_select_sprites_visible.sel;
    struct StageSelApe *ape = &sel->apeInfo;

    if (ape->ape != NULL)
    {
        g_something_with_freeing_chara_heap_ape(ape->ape);
        ape->ape = NULL;
    }
}

void g_handle_story_clear_stage_balls(int t)
{
    int i;
    struct ClearInfo *clear = &g_are_story_select_sprites_visible.clear;
    struct ClearEntry *e;
    int world;

    clear->count = g_are_story_select_sprites_visible.unbeatenCount;
    world = scen_info.world;
    e = clear->entries;
    for (i = 0; i < 10; i++, e++)
    {
        e->id = (s16)fn_1_A4CA8(world, i);
        e->delay = (s16)(t + i * 8);
        e->unk4 = 1.0f;
        e->scale = 0.0f;
    }
}

void sprite_clear_floors_tick(s8 *status, struct Sprite *sprite)
{
    struct ClearInfo *clear = &g_are_story_select_sprites_visible.clear;
    int count = clear->count;
    int i;
    struct ClearEntry *e = clear->entries;

    for (i = count; i > 0; i--, e++)
    {
        if (e->delay > 0)
        {
            e->delay--;
            if (e->delay == 0)
                e->scale = 1.5f;
        }
        else
            e->scale += 0.05f * (1.0f - e->scale);
        e->rotX += 0x100;
        e->rotY += 0x900;
    }
}

void sprite_clear_floors_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    Vec v;
    struct ClearInfo *clear = &g_are_story_select_sprites_visible.clear;
    float x = 268.0f + 3.0f * g_are_story_select_sprites_visible.anim.offset;
    float x2;
    int i;
    struct ClearEntry *e;

    req.bmpId = 0xF07;
    req.x = x;
    req.y = 340.0f;
    req.z = 0.02f + sprite->depth;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0x603413;
    req.addColor = 0;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;

    x2 = x;
    e = clear->entries;
    for (i = 9; i > 0; i--, e++)
    {
        req.bmpId = 0xF07;
        if (e->scale > 1.0f)
        {
            req.scaleX = e->scale;
            req.scaleY = e->scale;
        }
        else
        {
            req.scaleX = 1.0f;
            req.scaleY = 1.0f;
        }
        req.x = x2;
        draw_sprite_draw_request(&req);
        req.bmpId = 0xF02;
        req.x = 18.0f + x2;
        req.scaleX = 1.0f;
        req.scaleY = 1.0f;
        draw_sprite_draw_request(&req);
        x2 += 36.0f;
    }
    req.bmpId = 0xF07;
    req.x = x2;
    draw_sprite_draw_request(&req);

    req.bmpId = 0xF01;
    req.z = 0.01f + sprite->depth;
    req.mulColor = 0xFFFFFF;
    x2 = 18.0f + x;
    e = &clear->entries[1];
    for (i = 9; i > 0; i--, e++)
    {
        if (e->id < 0)
            req.mulColor = 0x808080;
        else
            req.mulColor = 0xFFFFFF;
        req.x = x2;
        draw_sprite_draw_request(&req);
        x2 += 36.0f;
    }

    req.bmpId = 0xF06;
    req.z = sprite->depth;
    req.mulColor = 0xFFFFFF;
    x2 = x;
    e = clear->entries;
    for (i = 10; i > 0; i--, e++)
    {
        if (e->id < 0)
            req.mulColor = 0x808080;
        else
            req.mulColor = 0xFFFFFF;
        req.x = x2;
        if (e->scale > 1.0f)
        {
            req.scaleX = e->scale;
            req.scaleY = e->scale;
        }
        else
        {
            req.scaleX = 1.0f;
            req.scaleY = 1.0f;
        }
        draw_sprite_draw_request(&req);
        x2 += 36.0f;
    }

    g_set_light_color_maybe(8, 0.8f, 0.8f, 0.8f);
    mtxa_push();
    mtxa_from_identity();
    mtxa_scale_xyz(1.0f, -1.0f, 1.0f);
    g_set_active_light_group(8);
    mtxa_pop();

    v.y = 340.0f;
    x2 = x;
    e = clear->entries;
    for (i = 0; i < 10; i++, e++)
    {
        v.x = x2;
        x = e->scale;
        if (e->id < 0 || 0.0f == x)
        {
            x2 += 36.0f;
            continue;
        }
        v.z = -(sprite->depth - 12.0f * x);
        mtxa_from_translate(&v);
        mtxa_scale_s(-24.0f);
        mtxa_rotate_y(e->rotY);
        mtxa_rotate_x(e->rotX);
        mtxa_rotate_z(e->rotZ);
        mtxa_scale_s(x);
        g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.25f);
        fn_1_4DDEC(mtxa, e->id, 2);
        g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
        x2 += 36.0f;
    }

    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    g_some_sprite_reset_gx_state_func();
}

void sprite_decoration_bar_tick(s8 *status, struct Sprite *sprite) {}

void sprite_decoration_bar_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    Vec v;
    int top = sprite->userVar == 0;
    float rotOff = g_are_story_select_sprites_visible.sel.rotOff;
    float x;
    float y;

    if (top)
    {
        x = 257.0f + rotOff;
        y = 126.0f;
    }
    else
    {
        x = 205.0f - rotOff;
        y = 330.0f;
    }
    if (!(g_are_story_select_sprites_visible.sel.flags & 1))
    {
        v.x = x;
        v.y = y;
    }
    else
    {
        mtxa_from_translate_xyz(g_are_story_select_sprites_visible.sel.x, g_are_story_select_sprites_visible.sel.y, 0.0f);
        mtxa_rotate_z((s16)-g_are_story_select_sprites_visible.sel.rot);
        mtxa_scale_s(g_are_story_select_sprites_visible.sel.scale);
        mtxa_translate_xyz(-182.0f, -239.0f, 0.0f);
        mtxa_tf_point_xyz(x, y, 0.0f, &v);
    }
    req.bmpId = 0xF03;
    req.x = v.x;
    req.y = v.y;
    req.z = sprite->depth;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    y = v.x;
    req.y = v.y;
    if (top)
    {
        req.flags |= 0x40000;
        req.mulColor = 0x908080;
        while (y < 656.0f)
        {
            req.x = y;
            draw_sprite_draw_request(&req);
            y += 38.0f;
        }
    }
    else
    {
        req.mulColor = 0xFFFFFF;
        while (y > -16.0f)
        {
            req.x = y;
            draw_sprite_draw_request(&req);
            y -= 38.0f;
        }
    }
}

void sprite_world_tick(s8 *status, struct Sprite *sprite) {}

void sprite_world_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x = sprite->x;
    float y = sprite->y - g_are_story_select_sprites_visible.anim.offset;
    int world = scen_info.world;
    float tx;
    float ty;
    float w;
    char *str;

    req.bmpId = 0xF0A;
    req.x = x;
    req.y = y;
    req.z = 0.01f + sprite->depth;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 0.6764706f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.u1 = 0.31617647f;
    req.v1 = 0.0f;
    req.u2 = 0.99264705f;
    req.v2 = 1.0f;
    draw_sprite_draw_request(&req);

    tx = -183.0f + x;
    ty = -12.0f + y;
    req.bmpId = 0xF0C;
    req.x = tx;
    req.y = ty;
    req.z = sprite->depth;
    req.flags = 5;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    if (world < 10)
        req.x += 5.0f;
    draw_sprite_draw_request(&req);

    tx = -122.0f + x;
    ty = -30.0f + y;
    req.bmpId = 0xF09;
    req.x = tx;
    req.y = ty;
    req.scaleX = 0.2f;
    req.scaleY = 0.5f;
    req.u1 = 0.2f * (world % 5);
    req.v1 = 0.5f * (world / 5);
    req.u2 = 0.2f + req.u1;
    req.v2 = 0.5f + req.v1;
    draw_sprite_draw_request(&req);

    str = fn_2_F06C(locale, world);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0xC0000000);
    textdraw_set_mul_color(0xFFFF00);
    tx = -74.0f + x;
    ty = -12.0f + y;
    w = textdraw_get_pixel_width_of_string_as_f32(str);
    if (w > 242.0f)
        textdraw_set_scale(242.0f / w, 1.0f);
    textdraw_set_depth(sprite->depth);
    textdraw_set_pos(tx, ty);
    textdraw_printf(str);
}

void g_animate_story_select_sprites(int t)
{
    struct SlideAnim *anim = &g_are_story_select_sprites_visible.anim;

    anim->state = 1;
    anim->timer = (s16)t;
    anim->offset = 0.0f;
}

void g_sprite_world_info_tick(s8 *status, struct Sprite *sprite)
{
    struct SlideAnim *anim = &g_are_story_select_sprites_visible.anim;

    switch (anim->state)
    {
    case 1:
        anim->state = 2;
        /* fall through */
    case 2:
        anim->offset = 8.0f * anim->timer;
        anim->timer--;
        if (anim->timer > 0)
            break;
        anim->state = 3;
        break;
    case 3:
        anim->state = 4;
        anim->offset = 0.0f;
        /* fall through */
    case 4:
        if (scen_info.flags & 8)
            anim->state = 5;
        break;
    case 5:
        anim->state = 6;
        /* fall through */
    case 6:
        anim->offset += 8.0f;
        break;
    case 7:
    case 0:
        break;
    }
}

char *lbl_2_data_18BC = "Stage Select";

void g_sprite_world_info_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x = sprite->x;
    float y = sprite->y - g_are_story_select_sprites_visible.anim.offset;
    float w;

    req.bmpId = 0xF0A;
    req.x = x;
    req.y = y;
    req.z = 0.01f + sprite->depth;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 0.30147058f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 0.30147058f;
    req.v2 = 1.0f;
    draw_sprite_draw_request(&req);

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0xC0000000);
    textdraw_set_mul_color(0xFFFF00);
    y -= 12.0f;
    w = textdraw_get_pixel_width_of_string_as_f32(lbl_2_data_18BC);
    if (w > 140.0f)
    {
        textdraw_set_scale(140.0f / w, 1.0f);
        w = 140.0f;
    }
    x -= 0.5f * w;
    textdraw_set_depth(sprite->depth);
    textdraw_set_pos(x, y);
    textdraw_printf(lbl_2_data_18BC);
}

void sprite_scen_stagesel_stage_name_tick(s8 *status, struct Sprite *sprite)
{
    switch (sprite->userVar)
    {
    case 1:
        if ((scen_info.flags & 8) || 0.0f == g_are_story_select_sprites_visible.anim.offset)
        {
            if (sprite->counter < 600)
                sprite->counter++;
        }
        if (scen_info.unk6 != sprite->unk50)
        {
            sprite->counter = 0;
            sprite->unk50 = scen_info.unk6;
        }
        if (scen_info.flags & 8)
            sprite->opacity += 0.2f * -sprite->opacity;
        if (is_storymode_stage_beaten(scen_info.world, scen_info.unk6))
            sprite->counter = 0;
        break;
    case 2:
        sprite->opacity += 0.1f * -sprite->opacity;
        break;
    }
}

void sprite_scen_stagesel_stage_name_disp(struct Sprite *sprite)
{
    u8 text[256];
    u8 c[4];
    int i;
    int j;
    int len;
    int width;
    char *name;
    int stage;
    int t;
    float scale;
    float alpha;

    if (sprite->counter > 0)
    {
        stage = scen_info.unk6;
        if (g_scen_stage_names_loaded == 0)
        {
            g_read_storymode_select_stage_names_from_dvd(scen_stgname_buffer);
            g_scen_stage_names_loaded = 1;
        }
        {
            char *n = scen_stgname_buffer[get_story_mode_stage_id(scen_info.world, stage)];

            name = n;
        }
        textdraw_reset();
        textdraw_set_font(sprite->fontId);
        textdraw_set_flags(0xA0000000);
        textdraw_set_mul_color(0xFFFF00);
        textdraw_set_font_style(5);
        sprintf((char *)text, "\x81\x60 %s \x81\x60", name);
        len = g_smth_with_fonts_chara_load_wrapper_discard_result((char *)text);
        width = textdraw_get_pixel_width_of_string((char *)text);
        textdraw_set_alignment(1);
        textdraw_set_pos(sprite->x - 0.5f * width, sprite->y);
        for (i = 0, j = 0; i < len; i++, j++)
        {
            c[0] = text[j];
#ifdef __MWERKS__
            if (c[0] >= 0x80)
#else
            if (pc_text_is_lead(c[0]))
#endif
            {
                j++;
                c[1] = text[j];
                c[2] = 0;
            }
            else
                c[1] = 0;
            if (sprite->userVar == 1)
            {
                t = sprite->counter - i * 4;
                scale = CLAMP((16 - t) / 8.0f, 1.0f, 2.0f);
                alpha = CLAMP(t / 8.0f, 0.0f, 1.0f);
                if (scale > 1.9f)
                    alpha = 0.0f;
                scale *= sprite->scaleX;
                textdraw_set_scale(scale, scale);
                textdraw_set_alpha(alpha * sprite->opacity);
            }
            else
                textdraw_set_alpha(sprite->opacity);
            textdraw_print((char *)c);
        }
    }
}

void sprite_story_difficulty_tick(s8 *status, struct Sprite *sprite) {}

char *lbl_2_data_18D8 = "Difficulty";
char *g_unk_difficulty1[2] = {
    "/bcff8000/Select a stage with /bcffffff/\x83\xAD/bcff8000/ and decide with /bcffffff/\x83\x9F/bcff8000/.",
    "/bcff8000/For other options, press START/PAUSE.",
};

void sprite_story_difficulty_disp(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    float x = sprite->x;
    float y = sprite->y + g_are_story_select_sprites_visible.anim.offset;
    float w;
    float tx;
    float ty;
    int i;
    char **p;
    int diff;

    req.bmpId = 0xF0B;
    req.x = x;
    req.y = y;
    req.z = 0.01f + sprite->depth;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    draw_sprite_draw_request(&req);

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0xC0000000);
    textdraw_set_scale(0.75f, 0.75f);
    ty = 26.0f + (4.0f + (y - 40.0f));
    p = g_unk_difficulty1;
    for (i = 2; i > 0; i--, p++)
    {
        w = 0.75f * textdraw_get_pixel_width_of_string_as_f32(*p);
        if (w > 524.0f)
        {
            textdraw_set_scale(393.0f / w, 0.75f);
            w = 524.0f;
        }
        else
            textdraw_set_scale(0.75f, 0.75f);
        tx = x - 0.5f * w;
        textdraw_set_depth(sprite->depth);
        textdraw_set_pos(tx, ty);
        textdraw_printf(*p);
        ty += 24.0f;
    }

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0xC0000000);
    w = textdraw_get_pixel_width_of_string_as_f32(lbl_2_data_18D8);
    tx = x - 0.5f * (240.0f + (20.0f + w));
    ty = 4.0f + (y - 40.0f);
    textdraw_set_mul_color(0xFFFF00);
    textdraw_set_depth(sprite->depth);
    textdraw_set_pos(tx, ty);
    textdraw_printf(lbl_2_data_18D8);

    req.bmpId = 0xF00;
    req.x = x;
    req.y = y;
    req.z = sprite->depth;
    req.opacity = 1.0f;
    req.flags = 6;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    tx += 20.0f + w;
    ty = 4.0f + (y - 40.0f);
    diff = get_story_mode_stage_difficulty(scen_info.world, scen_info.unk6);
    if (diff > 10)
        diff = 10;
    for (i = 0; i < diff; i++)
    {
        req.x = tx;
        req.y = ty;
        draw_sprite_draw_request(&req);
        tx += 24.0f;
    }
    req.mulColor = 0x808080;
    for (; i < 10; i++)
    {
        req.x = tx;
        req.y = ty;
        draw_sprite_draw_request(&req);
        tx += 24.0f;
    }
}

void g_draw_story_stage_preview_ball(int t)
{
    struct PreviewBall *p = &g_are_story_select_sprites_visible.preview;
    struct PreviewParams *params;

    memset(p, 0, sizeof(*p));
    p->state = 1;
    p->timer = (s16)t;
    p->x = 437.0f;
    p->y = 239.0f;
    p->size = 24.0f;
    p->stage = scen_info.unk6;
    p->unk4C = p->size;
    p->unk54 = 0.0f;
    params = &MAIN_GAME_STORY_STAGE_PREVIEW_PARAMS[p->stage + scen_info.world * 10];
    p->unk5C = params->unk2;
    p->unk60 = params->unk4;
    p->unk64 = 1.0f;
    p->unk68 = 0.0f;
    p->unbeaten = is_storymode_stage_beaten(scen_info.world, p->stage) == 0;
}

void g_sprite_story_stage_info_tick(s8 *status, struct Sprite *sprite)
{
    struct PreviewBall *p = &g_are_story_select_sprites_visible.preview;
    float target = 437.0f;
    struct PreviewParams *params;
    float f;
    float g;
    float t;
    int stage;
    int prev;

    switch (p->state)
    {
    case 1:
        p->state = 2;
        p->vel = -0.06666667f * p->size * (1.0f + 2.0f * RAND_FLOAT());
        p->pos.y = p->size;
        /* fall through */
    case 2:
        p->pos.x = -p->vel * p->timer;
        p->rotZ = -(s16)(32768.0f * p->pos.x / (3.1415927f * p->size));
        p->timer--;
        if (p->timer >= 0)
            break;
        p->state = 3;
        break;
    case 3:
        p->state = 4;
        p->vel *= -0.5f;
        p->unk48 = 0.0f;
        p->unk58 = p->size * (0.041666668f * (1.0f + 0.25f * RAND_FLOAT()));
        /* fall through */
    case 4:
        prev = p->stage;
        stage = scen_info.unk6;
        if (stage != prev)
        {
            p->stage = stage;
            p->unkC = 0;
            p->pos.x = 0.0f;
            p->vel = 0.1f * (p->size * (RAND_FLOAT() - 0.5f));
            p->unk48 = 0.0f;
            p->unk4C = p->size;
            p->unk50 = p->size * (0.0033333334f * (1.0f + RAND_FLOAT()));
            p->unk54 = 0.0f;
            p->unk58 = p->size * (0.041666668f * (1.0f + 0.25f * RAND_FLOAT()));
            params = &MAIN_GAME_STORY_STAGE_PREVIEW_PARAMS[stage + scen_info.world * 10];
            p->unk5C = params->unk2;
            p->unk60 = params->unk4;
            if (!is_storymode_stage_beaten(scen_info.world, stage))
            {
                p->unbeaten = 1;
                call_SoundReqID_arg_0(0x319);
            }
            else
                p->unbeaten = 0;
        }
        if (p->unbeaten)
            p->unkC = (s16)(p->unkC + ((-0x3000 - p->unkC) >> 4));
        p->vel += 0.025f * -p->pos.x;
        p->vel *= 0.96f;
        p->pos.x += p->vel;
        p->rotZ = -(s16)(32768.0f * p->pos.x / (3.1415927f * p->size));
        f = 0.2f * p->size;
        if (p->unk54 < f)
            p->unk54 += p->unk58;
        else
        {
            p->unk58 += -0.08f;
            p->unk58 *= 0.98f;
            p->unk54 += p->unk58;
            if (p->unk54 < f)
            {
                p->unk54 = f;
                if (p->unk58 < 0.0f)
                {
                    if (!fn_2_106FC())
                        p->unk58 *= -1.0f;
                    else
                        p->unk58 *= -0.5f;
                    p->unk48 = 1.0f;
                }
            }
        }
        if (p->unk48 && fn_2_106FC())
        {
            f = 1.5f * p->size;
            p->unk50 += 0.125f * (f - p->unk4C);
            p->unk50 *= 0.9f;
            p->unk4C += p->unk50;
            p->unk5C += 0x7F;
        }
        if (scen_info.flags & 8)
            p->state = 5;
        break;
    case 5:
        p->state = 6;
        p->timer = 0;
        p->vel += 0.075f * (p->size * (1.0f + 0.5f * RAND_FLOAT()));
        /* fall through */
    case 6:
        if (p->timer == 0)
        {
            if (fn_2_FED8() == 2)
                p->timer = 0x21;
        }
        else
            p->timer--;
        if (p->timer != 0)
        {
            t = p->timer;
            g = (-p->unk64 - p->unk68 * t) / (t * t);
            p->unk68 += g;
            p->unk64 += p->unk68;
        }
        else
        {
            if (p->unk64 > 0.8f)
            {
                g = -0.00011111111f;
                p->unk68 += g;
            }
            else
                p->unk68 *= 0.96f;
            p->unk64 += p->unk68;
        }
        target = 320.0f;
        p->unkC = (s16)(p->unkC + ((-0x3000 - p->unkC) >> 4));
        p->vel += 0.0125f * -p->pos.x;
        p->vel *= 0.96f;
        p->pos.x += p->vel;
        p->rotZ = -(s16)(32768.0f * p->pos.x / (3.1415927f * p->size));
        f = 0.2f * p->size;
        if (p->unk54 < f)
            p->unk54 += p->unk58;
        else
        {
            p->unk58 += -0.08f;
            p->unk58 *= 0.98f;
            p->unk54 += p->unk58;
            if (p->unk54 < f)
            {
                p->unk54 = f;
                if (p->unk58 < 0.0f)
                {
                    if (!fn_2_106FC())
                        p->unk58 *= -1.0f;
                    else
                        p->unk58 *= -0.5f;
                    p->unk48 = 1.0f;
                }
            }
        }
        if (p->unk48 && fn_2_106FC())
        {
            f = 1.5f * p->size;
            p->unk50 += 0.125f * (f - p->unk4C);
            p->unk50 *= 0.9f;
            p->unk4C += p->unk50;
            p->unk5C += 0x7F;
        }
        break;
    case 7:
    case 0:
        break;
    }
    p->x += 0.2f * (target - p->x);
}

void g_sprite_story_stage_info_disp(struct Sprite *sprite)
{
    Mtx mtx;
    float proj[7];
    float proj2[7];
    struct Sphere sphere;
    Vec eye;
    Vec target;
    Vec up;
    Vec v;
    struct PreviewArg arg;
    struct PreviewBall *p = &g_are_story_select_sprites_visible.preview;
    int stage = scen_info.unk6;
    float x = p->x;
    float y = p->y;
    int drawTex = 0;
    float f = p->unk64;

    eye.x = 0.0f;
    eye.y = 32.0f * (0.4f + 0.6f * f) + p->size;
    eye.z = 64.0f * f;
    target.x = 0.0f;
    target.y = p->size;
    target.z = 0.0f;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    g_math_unk13(&eye, &up, &target);
    mtxa_to_mtx(mtx);
    GXGetProjectionv(proj);
    memcpy2(proj2, proj, sizeof(proj));
    proj2[2] = 1.0f - 0.003125f * x;
    proj2[4] = 0.004166667f * y - 1.0f;
    g_smth_with_projection_mtx(proj2);

    mtxa_push();
    mtxa_from_mtx(mtx);
    g_set_active_light_group(0);
    mtxa_pop();

    if (p->unbeaten)
    {
        if (!fn_2_106FC())
            drawTex = 1;
        else
        {
            get_curr_stage_fly_in_position(&sphere);
            mtxa_from_mtxb();
            mtxa_push();
            mtxa_from_mtx(mtx);
            mtxa_translate_xyz(p->pos.x, p->pos.y + p->unk54 * p->unk60, p->pos.z);
            mtxa_rotate_y(p->rotY);
            mtxa_rotate_x(p->rotX);
            mtxa_rotate_z(p->rotZ);
            mtxa_rotate_y(p->unk5C);
            mtxa_scale_s(p->unk4C * p->unk60 / sphere.radius);
            mtxa_translate_neg(&sphere.pos);
            mtxa_to_mtx(mtxa->mtxB);
            fn_2_E3B4();
            mtxa_pop();
            mtxa_to_mtx(mtxa->mtxB);
        }
    }

    g_set_light_color_maybe(8, 0.4f, 0.4f, 0.4f);
    mtxa_push();
    mtxa_from_mtx(mtx);
    g_set_active_light_group(8);
    mtxa_pop();

    memset(&arg, 0, sizeof(arg));
    mtxa_from_mtx(mtx);
    mtxa_translate(&p->pos);
    mtxa_scale_s(p->size / 0.5f);
    mtxa_rotate_y(p->rotY);
    mtxa_rotate_x(p->rotX);
    mtxa_rotate_z(p->rotZ);
    mtxa_rotate_x(0x4000);
    arg.unk0 = p->unkC;
    arg.unk2 = (s16)p->stage;
    fn_2_E2E4(&arg);

    if (drawTex && p->unbeaten)
    {
        mtxa_from_mtx(mtx);
        mtxa_translate_xyz(p->pos.x, p->pos.y + p->unk54, p->pos.z);
        mtxa_rotate_y(p->rotY);
        mtxa_rotate_x(p->rotX);
        mtxa_rotate_z(p->rotZ);
        mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &v);
        mtxa_sq_from_identity();
        mtxa_rotate_z((s16)(math_atan2(v.y, v.x) - 0x4000));
        mtxa_scale_s(p->unk4C / 24.0f);
        fn_2_E20C(stage);
    }

    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    g_some_sprite_reset_gx_state_func();
    g_smth_with_projection_mtx(proj);
}

void g_preload_all_story_preview_images(void)
{
    int i;
    struct PreviewTex *pv;
    int world;
    int oldHeap;

    oldHeap = OSSetCurrentHeap(chara_heap);
    world = scen_info.world;
    pv = g_are_story_select_sprites_visible.previews;

    for (i = 0; i < 10; i++, pv++)
    {
        g_load_preview_texture(pv->tex, "preview/w256x256.tpl", i + world * 10, 0x100, 0x100, 5);
        pv->flags &= ~2;
        pv->flags |= 1;
    }
    OSSetCurrentHeap(oldHeap);
}

void fn_2_E198(void)
{
    int i;
    struct PreviewTex *pv = g_are_story_select_sprites_visible.previews;

    for (i = 0; i < 10; i++, pv++)
    {
        if (pv->flags & 1)
        {
            fn_1_F9E1C(pv->tex);
            pv->flags &= ~3;
        }
    }
}

void fn_2_E20C(int stage)
{
    struct PreviewTex *pv = &g_are_story_select_sprites_visible.previews[stage];
    float w;
    float h;

    if (pv->flags & 1)
    {
        if (!(pv->flags & 2))
        {
            fn_1_F9CBC(pv->tex);
            pv->flags |= 2;
        }
        w = 24.0f;
        h = 24.0f;
        g_some_sprite_reset_gx_state_func();
        g_smth_with_gpu_and_locked_cache_mtx();
        fn_1_F9EAC(pv->tex, -0.5f * w, 0.5f * h, 0.0f, w, -h, -1, 0);
    }
}

void fn_2_E2E4(struct PreviewArg *arg)
{
    Mtx m2;
    Mtx m1;

    mtxa_push();
    mtxa_to_mtx(m1);
    mtxa_translate_xyz(0.0f, -0.5f, 0.0f);
    mtxa_rotate_x(arg->unk0);
    mtxa_translate_xyz(0.0f, 0.5f, 0.0f);
    mtxa_to_mtx(m2);
    g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.5f);
    fn_1_4E2FC(m1, arg->unk2, 3);
    g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.25f);
    fn_1_4E12C(m2, 3);
    g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
    mtxa_pop();
}

void fn_2_E3B4(void)
{
    s8 state;

    if (events[4].state == 2)
        g_draw_stobjs();
    state = events[0].state;
    if (state == 2 || state == 4)
        g_draw_stage();
    ord_tbl_draw_nodes();
}

void fn_2_E418(u8 *color, int fmt)
{
    GXTexObj tex;
    float vp[6];
    float proj[7];
    int x;
    int y;
    int w;
    int h;
    int coord;
    int tev;
    float a;
    float b;
    float x0;
    float x1;
    float y0;
    float y1;

    GXGetViewportv(vp);
    x = vp[0];
    y = vp[1];
    w = vp[2];
    h = vp[3];
    coord = w >> 1;
    tev = h >> 1;
    GXSetTexCopySrc_cached(x, y, w, h);
    GXSetTexCopyDst_cached(coord, tev, fmt, 1);
    GXCopyTex(lbl_801988E0, 0);
    GXInitTexObj(&tex, lbl_801988E0, coord, tev, fmt, 0, 0, 0);
    GXPixModeSync();
    GXInvalidateTexAll();
    GXLoadTexObj_cached(&tex, 0);
    g_something_with_GX_vtx_desc(0x2200);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 7, 1);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    GXSetTevKColor_cached(0, *(GXColor *)color);

    tev = 0;
    coord = 0;
    GXSetTexCoordGen2_cached(coord, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(tev, coord, 0, 0xFF);
    GXSetTevKColorSel_cached(tev, 0xC);
    GXSetTevKAlphaSel_cached(tev, 0x1C);
    GXSetTevColorIn_cached(tev, 0xF, 0xE, 8, 0xF);
    GXSetTevColorOp_cached(tev, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tev, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(tev, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(tev);
    tev++;
    coord++;
    GXSetNumTevStages_cached(tev);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(0);

    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();

    GXGetProjectionv(proj);
    a = 1.0f / proj[1];
    b = 1.0f / proj[3];
    x0 = -1000.0f * (-1.0f - proj[2]) * a;
    x1 = -1000.0f * (1.0f - proj[2]) * a;
    y0 = -1000.0f * (-1.0f - proj[4]) * b;
    y1 = -1000.0f * (1.0f - proj[4]) * b;
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x0, y0, -1000.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x1, y0, -1000.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y1, -1000.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x0, y1, -1000.0f);
    GXTexCoord2f32(1.0f, 1.0f);
}

char *lbl_2_data_19B8 = "NOW LOADING";

void g_draw_now_loading_text(void)
{
    struct Sprite *sprite;
    u8 *s;
    int i;
    struct LoadingChar *c;
    float x;
    float y;
    float scaleX;
    float scaleY;
    float w;
    float dx;

    fn_2_EB6C();
    textdraw_reset();
    textdraw_set_font(9);
    scaleX = 0.375f;
    scaleY = 0.375f;
    s = (u8 *)lbl_2_data_19B8;
    x = 616.0f - 72.0f * scaleX * strlen((char *)s);
    y = 456.0f;
    i = 0;
    c = lbl_2_bss_788.chars;
    while (*s != 0)
    {
        if (*s != ' ')
        {
            sprite = create_sprite();
            if (sprite != NULL)
            {
                sprintf(sprite->text, "%c", *s);
                w = g_get_string_sprite_width_2(sprite->text);
                dx = 0.5f * ((72.0f - w) * scaleX);
                sprite->tag = 0x26;
                sprite->x = x + dx;
                sprite->y = y;
                sprite->depth = 0.2f;
                sprite->scaleX = scaleX;
                sprite->scaleY = scaleY;
                sprite->fontId = 9;
                sprite->textAlign = 5;
                sprite->userVar = i;
                sprite->mainFunc = fn_2_EBBC;
                sprite->flags |= 0x1000000;
                sprite->widescreenArg = 0x220;
                c->on = 1;
                c->angle = i << 15;
                c->scaleA = scaleX;
                c->scaleB = scaleY;
                c->x = x;
                c->y = y;
                c->y2 = y;
                c->vy = -20.0f * RAND_FLOAT();
                c->unk20 = scaleX;
                c->bmpId = -1;
                c->nextBmpId = -2;
                c++;
                i++;
            }
        }
        x += 72.0f * scaleX;
        s++;
        if (i >= 24)
            break;
    }
}

void fn_2_EA58(int unused)
{
    int i;
    struct LoadingChar *c;

    if (lbl_2_bss_788.active != 0)
    {
        lbl_2_bss_788.active = 0;
        c = lbl_2_bss_788.chars;
        for (i = 24; i > 0; i--, c++)
        {
            if (c->on != 0 && __fabs(c->vy) < 1.0)
            {
                c->vy += -20.0f * RAND_FLOAT();
                if (rand() & 1)
                    c->angle += 65536.0f;
                else
                    c->angle -= 65536.0f;
            }
        }
    }
}

void fn_2_EB6C(void)
{
    memset(&lbl_2_bss_788, 0xFF, 0x42C);
    lbl_2_bss_788.active = 1;
    lbl_2_bss_788.timer = 180;
}

s32 lbl_2_data_19C0[16] = {
    0x31, 0x48, 0x5D, 0x41, 0xA, 0x14, 0x24, 0x36,
    0x31, 0x48, 0x5D, 0x41, 0x4D, 0x17, 0x27, 0x39,
};

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

void fn_2_EBBC(s8 *status, struct Sprite *sprite)
{
    float rgb[3];
    struct LoadingChar *c;
    s16 ang;
    float scale;
    float s;

    if (sprite->userVar == 0 && lbl_2_bss_788.active != 0)
    {
        if (lbl_2_bss_788.timer > 24)
        {
            c = 0;
        }
        else
        {
            c = &lbl_2_bss_788.chars[lbl_2_bss_788.timer];
            if (c->on != 0)
            {
                c->angle += 65536.0f * (1 << (rand() & 3));
                c->vy += -10.0f * (1.0f + RAND_FLOAT());
                if (lbl_2_bss_788.counter & 1)
                    c->nextBmpId = lbl_2_data_19C0[(u32)(rand() & 0x7FFF) % 16];
                else
                    c->nextBmpId = -1;
            }
        }
        lbl_2_bss_788.timer--;
        if (lbl_2_bss_788.timer < 0)
        {
            lbl_2_bss_788.counter++;
            lbl_2_bss_788.timer = 180;
        }
    }

    c = &lbl_2_bss_788.chars[sprite->userVar];
    c->angle *= 0.97f;
    {
        int raw = (u16)c->angle;
        ang = raw;
        raw = 0;
    }
    scale = c->scaleA * mathutil_cos(ang);
    if (c->nextBmpId != -2 && c->unk20 * scale < 0.0f)
    {
        c->bmpId = c->nextBmpId;
        c->nextBmpId = -2;
    }
    c->unk20 = scale;
    c->vy += 0.48f;
    c->vy *= 0.98f;
    c->y2 += c->vy;
    if (lbl_2_bss_788.active != 0)
    {
        if (c->y2 > c->y)
        {
            c->y2 = c->y;
            if (c->vy > 0.0f)
            {
                if (c->vy > 8.0f)
                {
                    if (rand() & 1)
                        c->angle += 65536.0f;
                    else
                        c->angle += -65536.0f;
                }
                c->vy *= -(0.6f + 0.3f * RAND_FLOAT());
            }
        }
    }
    else if (c->y2 > 544.0f)
    {
        c->on = 0;
        *status = 0;
    }

    if (c->bmpId < 0)
    {
        s = math_sin(ang + 0x7800);
        s = s * (s * (s * s));
        rgb[0] = 192.0f * s;
        rgb[1] = 192.0f * s;
        rgb[2] = 224.0f * s;
        hud_f32_to_u8(&rgb[0], &sprite->addR);
        hud_f32_to_u8(&rgb[1], &sprite->addG);
        hud_f32_to_u8(&rgb[2], &sprite->addB);
    }
    else
    {
        sprite->addR = 0;
        sprite->addG = 0;
        sprite->addB = 0;
    }

    if (c->bmpId >= 0)
    {
        sprite->type = 1;
        sprite->bmpId = (u16)c->bmpId;
        sprite->scaleX = scale;
        if (sprite->scaleX < 0.0f)
            sprite->scaleX = -sprite->scaleX;
        sprite->scaleY = c->scaleA;
        sprite->scaleX *= 0.84375f;
        sprite->scaleY *= 0.5484375f;
    }
    else
    {
        sprite->type = 0;
        sprite->scaleX = scale;
        if (sprite->scaleX < 0.0f)
            sprite->scaleX = -sprite->scaleX;
        sprite->scaleY = c->scaleB;
    }
    sprite->x = c->x;
    sprite->y = c->y2;
}

char *fn_2_F06C(int loc, int world)
{
    return lbl_2_data_16C0[world][loc];
}

int lbl_2_data_1A00[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
