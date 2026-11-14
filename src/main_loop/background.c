/* Stage backgrounds: per-theme init/tick/draw dispatch, model scan and shared animation. */

#include "types.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#define NULL2 ((void *)0)
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mode.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "rand.h"
#include "background.h"
#include "world.h"
#include "avdisp.h"
#include "frustum.h"

extern struct
{
    u8 filler0[0x4];
    u32 bgTime;
} perfInfo;
extern u32 monkey_flags;
struct ShadowTexInfo
{
    /*0x00*/ u8 unk0;
    /*0x04*/ GXTexObj texObj;
};  /* size = 0x24 */

struct ShadowCastReq
{
    /*0x00*/ void *obj;
    /*0x04*/ Vec lightPos;
    /*0x10*/ float unk10;
    /*0x14*/ Vec pos;
    /*0x20*/ float radius;
    /*0x24*/ Vec up;
    /*0x30*/ s16 unk30;
    /*0x32*/ u16 cameraMask;
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ GXColor color;
    /*0x40*/ struct ShadowTexInfo tex;
};  /* size = 0x64 */

/* A group of background models rendered into one shadow texture */
char *strstr(const char *a, const char *b);
void g_something_with_translating_items(int a, Vec *b, float c);
struct BgModelFlagEntry
{
    u8 *name;
    s8 flag;
};

extern struct BgModelFlagEntry lbl_1_data_11008[];
extern s32 lbl_1_bss_6F00;
void g_load_ape_bg(void);
void fn_1_A00D0(void);
void fn_1_A01C8(void);
void fn_1_A0298(void);
void fn_1_72A34(void);
s8 lbl_1_bss_87F4C;
extern struct Light s_lightPool[];
extern struct
{
    /*0x00*/ u32 flags;
    u8 filler4[0x58 - 0x4];
    /*0x58*/ GXColor bgColor;
} g_some_render_flag;
void GXCopyTex(void *dest, int clear);
u16 GXGetTexObjWidth(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);

void GXSetCopyClear_cached(GXColor color, u32 z);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 wd, u16 ht);
void GXSetTexCopyDst_cached(u16 wd, u16 ht, u32 fmt, int mipmap);
void g_smth_with_bg_color_drawing(GXColor *color);
void g_something_with_raster_state(int a);
void g_GXSetTevIndirect_zero_if_different(int stage);
void C_MTXLookAt(Mtx m, Vec *pos, Vec *up, Vec *target);
void C_MTXOrtho(Mtx44 m, float t, float b, float l, float r, float n, float f);
void fn_1_10A69C(void *func);
void fn_1_E2E18(void);
void fn_1_70DAC(void);
int fn_1_70E90(void);
void fn_1_707FC(void);
extern u32 g_font_char_var_3;
static u32 lbl_1_bss_87F10;
static u16 lbl_1_bss_87F14;
static u16 lbl_1_bss_87F16;
static u16 lbl_1_bss_87F18;
static u16 lbl_1_bss_87F1A;
static Mtx g_some_bg_mtx;
void g_handle_bg_fg_model_anim(float, struct StageBgModel *, int);
float fn_1_8034(Vec *pos);
void fn_1_71844(Mtx mtx, struct StageEffectHeader *effect);
void g_something_with_texture_scroll(float *scroll);
void g_yet_another_unk_draw_func(void *func);
void g_render_foreground_objects(Mtx, struct StageBgModel *, int);

extern u32 frame_counter;
extern int sub_mode;
typedef int OSHeapHandle;
extern OSHeapHandle bg_heap;
extern OSHeapHandle currentHeap;
extern struct GMA *g_bg_gma;
extern struct TPL *g_bg_tpl;
extern struct NlObj *g_some_nl_model_buffer_2;
extern struct TPL *g_some_tpl_buffer;
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);
void OSFreeToHeap(OSHeapHandle heap, void *ptr);
static struct BgShadowGroup *lbl_1_bss_87F08;
static struct BgShadowGroup **lbl_1_bss_87F0C;
void *alloc_from_heap_or_panic(OSHeapHandle heap, u32 size, char *file, int line);
void fn_1_371C8(void);
void fn_1_70268(void);
void fn_1_70E98(void);
void g_something_freeing_heap(void *);
void g_something_freeing_heap_2(void *);
void gp_wait_wrapper(void);
struct TPL *g_load_tpl(char *name);
struct GMA *g_load_gma(char *name, struct TPL *tpl);
void queue_disc_read(char *);

void bg_bil2_dest(void);
void bg_bil2_disp(void);
void bg_bil2_init(void);
void bg_bil2_item_coin_coli(int);
void bg_bil2_tick(void);
void bg_bil_dest(void);
void bg_bil_disp(void);
void bg_bil_init(void);
void bg_bil_item_coin_coli(int);
void bg_bil_tick(void);
void bg_bns2_dest(void);
void bg_bns2_disp(void);
void bg_bns2_init(void);
void bg_bns2_item_coin_coli(int);
void bg_bns2_tick(void);
void bg_bns_dest(void);
void bg_bns_disp(void);
void bg_bns_init(void);
void bg_bns_item_coin_coli(int);
void bg_bns_tick(void);
void bg_boat_dest(void);
void bg_boat_disp(void);
void bg_boat_init(void);
void bg_boat_item_coin_coli(int);
void bg_boat_tick(void);
void bg_bow2_author_dest(void);
void bg_bow2_author_disp(void);
void bg_bow2_author_init(void);
void bg_bow2_author_item_coin_coli(int);
void bg_bow2_author_tick(void);
void bg_bow_dest(void);
void bg_bow_disp(void);
void bg_bow_init(void);
void bg_bow_item_coin_coli(int);
void bg_bow_tick(void);
void bg_bubble_dest(void);
void bg_bubble_disp(void);
void bg_bubble_init(void);
void bg_bubble_item_coin_coli(int);
void bg_bubble_tick(void);
void bg_dest_base(void);
void bg_disp_base(void);
void bg_electric_dest(void);
void bg_electric_disp(void);
void bg_electric_init(void);
void bg_electric_item_coin_coli(int);
void bg_electric_tick(void);
void bg_end_dest(void);
void bg_end_disp(void);
void bg_end_init(void);
void bg_end_item_coin_coli(int);
void bg_end_tick(void);
void bg_futsal_dest(void);
void bg_futsal_disp(void);
void bg_futsal_init(void);
void bg_futsal_item_coin_coli(int);
void bg_futsal_tick(void);
void bg_gear_author_dest(void);
void bg_gear_author_disp(void);
void bg_gear_author_tick(void);
void bg_gear_init(void);
void bg_gear_item_coin_coli(int);
void bg_gol_dest(void);
void bg_gol_disp(void);
void bg_gol_init(void);
void bg_gol_item_coin_coli(int);
void bg_gol_tick(void);
void bg_ice2_dest(void);
void bg_ice2_disp(void);
void bg_ice2_init(void);
void bg_ice2_item_coin_coli(int);
void bg_ice2_tick(void);
void bg_init_base(void);
void bg_item_coin_coli_base(int);
void bg_jun2_dest(void);
void bg_jun2_disp(void);
void bg_jun2_init(void);
void bg_jun2_item_coin_coli(int);
void bg_jun2_tick(void);
void bg_jun_dest(void);
void bg_jun_disp(void);
void bg_jun_init(void);
void bg_jun_item_coin_coli(int);
void bg_jun_tick(void);
void bg_lava_dest(void);
void bg_lava_disp(void);
void bg_lava_init(void);
void bg_lava_item_coin_coli(int);
void bg_lava_tick(void);
void bg_mst_dest(void);
void bg_mst_disp(void);
void bg_mst_init(void);
void bg_mst_item_coin_coli(int);
void bg_mst_tick(void);
void bg_nig_dest(void);
void bg_nig_disp(void);
void bg_nig_init(void);
void bg_nig_item_coin_coli(int);
void bg_nig_tick(void);
void bg_park_dest(void);
void bg_park_disp(void);
void bg_park_init(void);
void bg_park_item_coin_coli(int);
void bg_park_tick(void);
void bg_pil2_dest(void);
void bg_pil2_disp(void);
void bg_pil2_init(void);
void bg_pil2_item_coin_coli(int);
void bg_pil2_tick(void);
void bg_pil_dest(void);
void bg_pil_disp(void);
void bg_pil_init(void);
void bg_pil_item_coin_coli(int);
void bg_pil_tick(void);
void bg_pot_dest(void);
void bg_pot_disp(void);
void bg_pot_fgt_dest(void);
void bg_pot_fgt_disp(void);
void bg_pot_fgt_init(void);
void bg_pot_fgt_item_coin_coli(int);
void bg_pot_fgt_tick(void);
void bg_pot_init(void);
void bg_pot_item_coin_coli(int);
void bg_pot_tick(void);
void bg_snd_dest(void);
void bg_snd_disp(void);
void bg_snd_init(void);
void bg_snd_item_coin_coli(int);
void bg_snd_tick(void);
void bg_spa2_dest(void);
void bg_spa2_disp(void);
void bg_spa2_init(void);
void bg_spa2_item_coin_coli(int);
void bg_spa2_tick(void);
void bg_spa_dest(void);
void bg_spa_disp(void);
void bg_spa_init(void);
void bg_spa_item_coin_coli(int);
void bg_spa_tick(void);
void bg_stm_dest(void);
void bg_stm_disp(void);
void bg_stm_init(void);
void bg_stm_item_coin_coli(int);
void bg_stm_tick(void);
void bg_sun_dest(void);
void bg_sun_disp(void);
void bg_sun_init(void);
void bg_sun_item_coin_coli(int);
void bg_sun_tick(void);
void bg_tick_base(void);
void bg_wat2_dest(void);
void bg_wat2_disp(void);
void bg_wat2_init(void);
void bg_wat2_item_coin_coli(int);
void bg_wat2_tick(void);
void bg_wat_dest(void);
void bg_wat_disp(void);
void bg_wat_init(void);
void bg_wat_item_coin_coli(int);
void bg_wat_tick(void);
void bg_whale_dest(void);
void bg_whale_disp(void);
void bg_whale_init(void);
void bg_whale_item_coin_coli(int);
void bg_whale_tick(void);
void bg_park2_init(void);
void bg_park2_tick(void);
void bg_park2_dest(void);
void bg_park2_disp(void);
void bg_park2_item_coin_coli(int);

#pragma force_active on

u16 lbl_1_data_2FF48[] =
{
    0xFE00, 0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C, 0xFE0D,
    0x0E0F, 0xFF00, 0x0001, 0x0203, 0x0405, 0x0607, 0x0809, 0x0A0B,
    0x0C0D, 0x0E0F, 0xFF00, 0x0000, 0x3100, 0x0000, 0x0000, 0x0000,
    0x3800, 0xB400, 0x0001, 0x0000, 0x0100, 0x0000, 0x0002, 0x0000,
    0x1000, 0x4C00, 0x0003, 0x0000, 0x0100, 0x0000, 0x0004, 0x0000,
    0x1800, 0x0000, 0x0009, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000,
    0x0100, 0x0000, 0x000B, 0x0000, 0x1800, 0x0000, 0x000C, 0x0000,
    0x1800, 0x0000, 0x0005, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000,
    0x0100, 0x0000, 0x0007, 0x0000, 0x1800, 0x0000, 0x0008, 0x0000,
    0x3800, 0xB400, 0x000D, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000,
    0x1800, 0x0000, 0x0014, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000,
    0x0100, 0x0000, 0x0016, 0x0000, 0x1800, 0x0000, 0x0017, 0x0000,
    0x1800, 0x0000, 0x0018, 0x0000, 0x1800, 0x0000, 0x000F, 0x0000,
    0x4000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0000, 0x0011, 0x0000,
    0x1800, 0x0000, 0x0012, 0x0000, 0x1800, 0x0000, 0x0013, 0x0000,
    0x1800, 0x0000, 0x0000, 0x0000, 0x1800, 0x0000, 0x0001, 0x0000,
    0x1800, 0x0000, 0x0002, 0x0000, 0x1800, 0x0000, 0x0003, 0x0000,
    0x1800, 0x0000, 0x0004, 0x0000, 0x1800, 0x0000, 0x0005, 0x0000,
    0x1800, 0x0000, 0x0006, 0x0000, 0x1800, 0x0000, 0x0007, 0x0000,
    0x1800, 0x0000, 0x0008, 0x0000, 0x1800, 0x0000, 0x0009, 0x0000,
    0x1800, 0x0000, 0x000A, 0x0000, 0x1800, 0x0000, 0x000B, 0x0000,
    0x1800, 0x0000, 0x000C, 0x0000, 0x1800, 0x0000, 0x000D, 0x0000,
    0x1800, 0x0000, 0x000E, 0x0000, 0x1800, 0x0000, 0x000F, 0x0000,
};

struct BackgroundInfo world_theme =
{
    0,
    8421504.0,
    0x80,
    { 128, 0, 0, 0 },
};

u32 lbl_1_data_30188 = 0xFF000000;

GXColor background_colors[] =
{
    { 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0xD8, 0xBC, 0x77, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF }, { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF }, { 0xFF, 0xFF, 0xCD, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF }, { 0x00, 0x00, 0x00, 0xFF },
};

char *bg_type_names[] =
{
    "NULL",
    "BG_TYPE_E3",
    "BG_TYPE_JUN",
    "BG_TYPE_WAT",
    "BG_TYPE_NIG",
    "BG_TYPE_SUN",
    "BG_TYPE_SPA",
    "BG_TYPE_SND",
    "BG_TYPE_ICE2",
    "BG_TYPE_STM",
    "BG_TYPE_BNS",
    "BG_TYPE_PIL",
    "BG_TYPE_BIL",
    "BG_TYPE_GOL",
    "BG_TYPE_BOW",
    "BG_TYPE_MST",
    "BG_TYPE_END",
    "BG_TYPE_LAVA",
    "BG_TYPE_WAT2",
    "BG_TYPE_JUN2",
    "BG_TYPE_PARK",
    "BG_TYPE_POT",
    "BG_TYPE_SPA2",
    "BG_TYPE_ELECTRIC",
    "BG_TYPE_GEAR",
    "BG_TYPE_BUBBLE",
    "BG_TYPE_BNS2",
    "BG_TYPE_FUTSAL",
    "BG_TYPE_BOW2",
    "BG_TYPE_PIL2",
    "BG_TYPE_BIL2",
    "BG_TYPE_BOAT",
    "BG_TYPE_WHALE",
    "BG_TYPE_GOL2",
    "BG_TYPE_POT_FGT",
    "BG_TYPE_AUTHOR",
    "BG_TYPE_BUBBLE_AUTHOR",
    "BG_TYPE_PARK_AUTHOR",
    "BG_TYPE_GEAR_AUTHOR",
    "BG_TYPE_WAT2_AUTHOR",
    "BG_TYPE_PIL2_AUTHOR",
    "BG_TYPE_BOW2_AUTHOR",
    NULL,
};

s16 bg_unk_pairs[][2] =
{
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { 6, 7 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { 6, 7 },
    { -1, -1 }, { -1, -1 }, { 6, 7 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { 6, 7 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { -1, -1 },
};

void (*bg_init_funcs[])(void) =
{
    NULL,
    bg_init_base,
    bg_jun_init,
    bg_wat_init,
    bg_nig_init,
    bg_sun_init,
    bg_spa_init,
    bg_snd_init,
    bg_ice2_init,
    bg_stm_init,
    bg_bns_init,
    bg_pil_init,
    bg_bil_init,
    bg_gol_init,
    bg_bow_init,
    bg_mst_init,
    bg_end_init,
    bg_lava_init,
    bg_wat2_init,
    bg_jun2_init,
    bg_park2_init,
    bg_pot_init,
    bg_spa2_init,
    bg_electric_init,
    bg_gear_init,
    bg_bubble_init,
    bg_bns2_init,
    bg_futsal_init,
    bg_init_base,
    bg_pil2_init,
    bg_bil2_init,
    bg_boat_init,
    bg_whale_init,
    bg_init_base,
    bg_pot_fgt_init,
    bg_park_init,
    bg_bubble_init,
    bg_park_init,
    bg_gear_init,
    bg_wat2_init,
    bg_pil2_init,
    bg_bow2_author_init,
    NULL,
};

void (*bg_tick_funcs[])(void) =
{
    NULL,
    bg_tick_base,
    bg_jun_tick,
    bg_wat_tick,
    bg_nig_tick,
    bg_sun_tick,
    bg_spa_tick,
    bg_snd_tick,
    bg_ice2_tick,
    bg_stm_tick,
    bg_bns_tick,
    bg_pil_tick,
    bg_bil_tick,
    bg_gol_tick,
    bg_bow_tick,
    bg_mst_tick,
    bg_end_tick,
    bg_lava_tick,
    bg_wat2_tick,
    bg_jun2_tick,
    bg_park2_tick,
    bg_pot_tick,
    bg_spa2_tick,
    bg_electric_tick,
    bg_gear_author_tick,
    bg_bubble_tick,
    bg_bns2_tick,
    bg_futsal_tick,
    bg_tick_base,
    bg_pil2_tick,
    bg_bil2_tick,
    bg_boat_tick,
    bg_whale_tick,
    bg_tick_base,
    bg_pot_fgt_tick,
    bg_park_tick,
    bg_bubble_tick,
    bg_park_tick,
    bg_gear_author_tick,
    bg_wat2_tick,
    bg_pil2_tick,
    bg_bow2_author_tick,
    NULL,
};

void (*bg_dest_funcs[])(void) =
{
    NULL,
    bg_dest_base,
    bg_jun_dest,
    bg_wat_dest,
    bg_nig_dest,
    bg_sun_dest,
    bg_spa_dest,
    bg_snd_dest,
    bg_ice2_dest,
    bg_stm_dest,
    bg_bns_dest,
    bg_pil_dest,
    bg_bil_dest,
    bg_gol_dest,
    bg_bow_dest,
    bg_mst_dest,
    bg_end_dest,
    bg_lava_dest,
    bg_wat2_dest,
    bg_jun2_dest,
    bg_park2_dest,
    bg_pot_dest,
    bg_spa2_dest,
    bg_electric_dest,
    bg_gear_author_dest,
    bg_bubble_dest,
    bg_bns2_dest,
    bg_futsal_dest,
    bg_dest_base,
    bg_pil2_dest,
    bg_bil2_dest,
    bg_boat_dest,
    bg_whale_dest,
    bg_dest_base,
    bg_pot_fgt_dest,
    bg_park_dest,
    bg_bubble_dest,
    bg_park_dest,
    bg_gear_author_dest,
    bg_wat2_dest,
    bg_pil2_dest,
    bg_bow2_author_dest,
    NULL,
};

void (*bg_disp_funcs[])(void) =
{
    NULL,
    bg_disp_base,
    bg_jun_disp,
    bg_wat_disp,
    bg_nig_disp,
    bg_sun_disp,
    bg_spa_disp,
    bg_snd_disp,
    bg_ice2_disp,
    bg_stm_disp,
    bg_bns_disp,
    bg_pil_disp,
    bg_bil_disp,
    bg_gol_disp,
    bg_bow_disp,
    bg_mst_disp,
    bg_end_disp,
    bg_lava_disp,
    bg_wat2_disp,
    bg_jun2_disp,
    bg_park2_disp,
    bg_pot_disp,
    bg_spa2_disp,
    bg_electric_disp,
    bg_gear_author_disp,
    bg_bubble_disp,
    bg_bns2_disp,
    bg_futsal_disp,
    bg_disp_base,
    bg_pil2_disp,
    bg_bil2_disp,
    bg_boat_disp,
    bg_whale_disp,
    bg_disp_base,
    bg_pot_fgt_disp,
    bg_park_disp,
    bg_bubble_disp,
    bg_park_disp,
    bg_gear_author_disp,
    bg_wat2_disp,
    bg_pil2_disp,
    bg_bow2_author_disp,
    NULL,
};

void (*bg_item_coin_coli_funcs[])(int) =
{
    NULL,
    bg_item_coin_coli_base,
    bg_jun_item_coin_coli,
    bg_wat_item_coin_coli,
    bg_nig_item_coin_coli,
    bg_sun_item_coin_coli,
    bg_spa_item_coin_coli,
    bg_snd_item_coin_coli,
    bg_ice2_item_coin_coli,
    bg_stm_item_coin_coli,
    bg_bns_item_coin_coli,
    bg_pil_item_coin_coli,
    bg_bil_item_coin_coli,
    bg_gol_item_coin_coli,
    bg_bow_item_coin_coli,
    bg_mst_item_coin_coli,
    bg_end_item_coin_coli,
    bg_lava_item_coin_coli,
    bg_wat2_item_coin_coli,
    bg_jun2_item_coin_coli,
    bg_park2_item_coin_coli,
    bg_pot_item_coin_coli,
    bg_spa2_item_coin_coli,
    bg_electric_item_coin_coli,
    bg_gear_item_coin_coli,
    bg_bubble_item_coin_coli,
    bg_bns2_item_coin_coli,
    bg_futsal_item_coin_coli,
    bg_item_coin_coli_base,
    bg_pil2_item_coin_coli,
    bg_bil2_item_coin_coli,
    bg_boat_item_coin_coli,
    bg_whale_item_coin_coli,
    bg_item_coin_coli_base,
    bg_pot_fgt_item_coin_coli,
    bg_park_item_coin_coli,
    bg_bubble_item_coin_coli,
    bg_park_item_coin_coli,
    bg_gear_item_coin_coli,
    bg_wat2_item_coin_coli,
    bg_pil2_item_coin_coli,
    bg_bow2_author_item_coin_coli,
    NULL,
};

void *bg_coli_funcs_2[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

void *bg_coli_funcs_3[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

char *bg_filename_list[] =
{
    NULL,
    NULL,
    "bg_jun",
    "bg_wat",
    "bg_nig",
    "bg_sun",
    "bg_spa",
    "bg_snd",
    "bg_ice",
    "bg_stm",
    "bg_bns",
    "bg_pil",
    NULL,
    "bg_gol",
    "bg_bow",
    "bg_mst",
    "bg_ending",
    "bg_lav2",
    "bg_wat2",
    "bg_jun2",
    "bg_par2",
    "bg_pot2",
    "bg_spa2",
    "bg_ele2",
    "bg_gea2",
    "bg_bub2",
    "bg_bns2",
    "bg_fut2",
    "bg_bow2",
    "bg_tar2",
    NULL,
    NULL,
    "bg_wha2",
    "bg_gol2",
    "bg_pot2",
    "bg_vil2",
    "bg_au_bub2",
    "bg_au_par2",
    "bg_au_gea2",
    "bg_au_wat2",
    "bg_au_tar2",
    "bg_bow2",
    NULL,
};

char *bg_old_filename_list[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

#ifndef __MWERKS__
void *pc_bg_work(void) { return world_theme.work; }
#endif

u32 bg_work_sizes[] =
{
    0x00000000, 0x00000000, 0x0000016C, 0x00000194, 0x00000000, 0x0000016C,
    0x000000AC, 0x0000056C, 0x00000000, 0x00000428, 0x0000080C, 0x00000330,
    0x00000000, 0x00000000, 0x00000000, 0x00000B90, 0x00000094, 0x00001914,
    0x000005E4, 0x000007D0, 0x00000008, 0x0000116C, 0x00000170, 0x00000000,
    0x00000190, 0x00001888, 0x00001ABC, 0x00000000, 0x00000000, 0x00000330,
    0x000000BC, 0x00000000, 0x000004E4, 0x00000000, 0x0000116C, 0x00000000,
    0x00001888, 0x00000000, 0x00000190, 0x000005E4, 0x00000330, 0x00000184,
    0x00000000,
};

u8 STAGE_WORLD_THEMES[] =
{
    0x00, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x12, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x14, 0x14, 0x14, 0x14,
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
    0x19, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x16, 0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16, 0x16, 0x01, 0x13, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x1B, 0x1B, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x13, 0x12, 0x16, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x13, 0x14, 0x18, 0x11, 0x16, 0x12, 0x04, 0x1F, 0x1F, 0x1F, 0x06, 0x03, 0x13,
    0x14, 0x22, 0x14, 0x15, 0x06, 0x06, 0x13, 0x0B, 0x1D, 0x1A, 0x17, 0x1D, 0x1D, 0x1D, 0x1D, 0x1C,
    0x1E, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
    0x21, 0x21, 0x21, 0x0E, 0x0C, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x13, 0x01, 0x0A, 0x10, 0x02, 0x14, 0x13, 0x13, 0x13, 0x13, 0x1A, 0x14, 0x14,
    0x14, 0x14, 0x14, 0x16, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 0x17, 0x13, 0x13, 0x13,
    0x13, 0x1A, 0x14, 0x14, 0x14, 0x14, 0x1A, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x1A, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x16,
    0x17, 0x17, 0x17, 0x17, 0x17, 0x13, 0x13, 0x13, 0x13, 0x1A, 0x15, 0x15, 0x15, 0x15, 0x1A, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1A, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19,
    0x19, 0x19, 0x1A, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1A, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
    0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1D, 0x17, 0x23,
    0x16, 0x1D, 0x23, 0x13, 0x1D, 0x11, 0x28, 0x27, 0x23, 0x20, 0x25, 0x25, 0x23, 0x26, 0x15, 0x24,
    0x29, 0x16, 0x17, 0x23, 0x20,
};

#pragma force_active reset
void event_background_init(void)
{
    int r30 = world_theme.bgId;
    void *r29 = world_theme.work;
    u32 r28 = world_theme.unkA0;

    memset(&world_theme, 0, sizeof(world_theme));  /* 0xD0 on GC */

    world_theme.bgId = r30;
    world_theme.work = r29;
    world_theme.unkA0 = r28;

    world_theme.animTimer = 0.0f;
    world_theme.unk8 = 0;

    world_theme.backdropColor = background_colors[world_theme.bgId];
    mtxa_from_identity();
    mtxa_to_mtx(world_theme.unk48);
    world_theme.unk78 = bg_coli_funcs_2[world_theme.bgId];
    world_theme.unk7C = bg_coli_funcs_3[world_theme.bgId];
    world_theme.unk8C = NULL;
    world_theme.unk90 = NULL;
    world_theme.ballEnvFunc = NULL;
    world_theme.unkA8 = bg_unk_pairs[world_theme.bgId][0];
    world_theme.unkAA = bg_unk_pairs[world_theme.bgId][1];
    if (world_theme.unkA8 < 0)
        world_theme.unkA8 = 0;
    if (world_theme.unkAA < 0)
        world_theme.unkAA = 4;
    fn_1_70268();
    if (world_theme.bgId > 0)
    {
        int temp = rand();
        srand(world_theme.unkA0);
        world_theme.unkA4 = rand();
        bg_init_funcs[world_theme.bgId]();
        world_theme.unkA0 = frame_counter + rand();
        srand(temp);
    }
}

void event_background_tick(void)
{
    if (!is_game_paused_and_in_view_stage())
    {
        world_theme.animTimer += 1.0f;
        world_theme.unkA4++;
        world_theme.timers[0] = world_theme.animTimer;
        if (mode_info.flags & 0x20)
            world_theme.timers[1] += 1.0;
        else
            world_theme.timers[1] = 0.0f;
        if (sub_mode == 0x3B)
            world_theme.timers[2] += 1.0;
        else
            world_theme.timers[2] = 0.0f;
        world_theme.timers[3] = world_theme.animTimer;
    }
    if (world_theme.bgId > 0)
        bg_tick_funcs[world_theme.bgId]();
}

void event_background_dest(void)
{
    if (world_theme.bgId > 0)
        bg_dest_funcs[world_theme.bgId]();
    world_theme.unk98 = NULL;
    world_theme.unk78 = NULL;
    world_theme.unk7C = NULL;
    world_theme.unk8C = NULL;
    world_theme.unk90 = NULL;
    world_theme.ballEnvFunc = NULL;
    fn_1_70E98();
}

void g_maybe_some_bg_effect_handler(void)
{
    if (world_theme.unk98 != NULL)
        world_theme.unk98();
}

void g_draw_bg(void)
{
    if (monkey_flags & 2)
    {
        perf_init_timer(8);
        if (world_theme.bgId > 0)
            bg_disp_funcs[world_theme.bgId]();
        perfInfo.bgTime += perf_stop_timer(8);
    }
}

void g_something_with_world_themes(void)
{
    if (events[0x14].state != 0)
        event_dest(0x14);
    if (world_theme.bgId > 0)
    {
        OSHeapHandle oldHeap = OSSetCurrentHeap(bg_heap);

        if (world_theme.work != NULL)
        {
            OSFreeToHeap(currentHeap, world_theme.work);
            world_theme.work = NULL;
        }
        if (g_bg_tpl != NULL || g_bg_gma != NULL)
            gp_wait_wrapper();
        if (g_bg_tpl != NULL)
        {
            g_something_freeing_heap_2(g_bg_tpl);
            g_bg_tpl = NULL;
        }
        if (g_bg_gma != NULL)
        {
            g_something_freeing_heap(g_bg_gma);
            g_bg_gma = NULL;
        }
        free_nlobj(&g_some_nl_model_buffer_2, &g_some_tpl_buffer);

        OSSetCurrentHeap(oldHeap);
        world_theme.bgId = -1;
    }
}

void g_something_with_init_or_maybe_leftover_from_smb1(int bgId)
{
    char bgDir[64];
    char gmaFileName[64];
    char tplFileName[64];
    char *bgName;

#ifndef __MWERKS__
    /* stage.c declares this (void) and calls it with no argument; on PPC
     * the stale r3 happened to be benign, on x86 the stack garbage would
     * index bg_filename_list wildly. */
    if (bgId < 0 || bgId >= (int)(sizeof(bg_filename_list) / sizeof(bg_filename_list[0])))
        return;
#endif
    if (world_theme.bgId != bgId)
    {
        bgName = bg_filename_list[bgId];
        if (bgName != NULL)
        {
            strcpy(bgDir, "bg/");
            DVDChangeDir(bgDir);
            sprintf(gmaFileName, "%s.gma", bgName);
            sprintf(tplFileName, "%s.tpl", bgName);
            queue_disc_read(gmaFileName);
            queue_disc_read(tplFileName);
            DVDChangeDir("/");
        }
        bgName = bg_old_filename_list[bgId];
        if (bgName != NULL)
        {
            strcpy(bgDir, "bg/");
            DVDChangeDir(bgDir);
            sprintf(gmaFileName, "%s_p.lz", bgName);
            sprintf(tplFileName, "%s.lz", bgName);
            queue_disc_read(gmaFileName);
            queue_disc_read(tplFileName);
            DVDChangeDir("/");
        }
    }
}

void g_something_with_stage_world_themes2(int themeId)
{
    char bgDir[64];
    char gmaFileName[64];
    char tplFileName[64];
    char *bgName;

    if (world_theme.bgId != themeId)
    {
        OSHeapHandle oldHeap = OSSetCurrentHeap(bg_heap);

        if (world_theme.bgId > 0)
        {
            if (world_theme.work != NULL)
            {
                OSFreeToHeap(currentHeap, world_theme.work);
                world_theme.work = NULL;
            }
            if (g_bg_tpl != NULL || g_bg_gma != NULL)
                gp_wait_wrapper();
            if (g_bg_tpl != NULL)
            {
                g_something_freeing_heap_2(g_bg_tpl);
                g_bg_tpl = NULL;
            }
            if (g_bg_gma != NULL)
            {
                g_something_freeing_heap(g_bg_gma);
                g_bg_gma = NULL;
            }
            free_nlobj(&g_some_nl_model_buffer_2, &g_some_tpl_buffer);
        }
        if (themeId > 0)
        {
            bgName = bg_filename_list[themeId];
            if (bgName != NULL)
            {
                strcpy(bgDir, "bg/");
                DVDChangeDir(bgDir);
                sprintf(gmaFileName, "%s.gma", bgName);
                sprintf(tplFileName, "%s.tpl", bgName);
                g_bg_tpl = g_load_tpl(tplFileName);
                g_bg_gma = g_load_gma(gmaFileName, g_bg_tpl);
                DVDChangeDir("/");
            }
            bgName = bg_old_filename_list[themeId];
            if (bgName != NULL)
            {
                strcpy(bgDir, "bg/");
                DVDChangeDir(bgDir);
                sprintf(gmaFileName, "%s_p.lz", bgName);
                sprintf(tplFileName, "%s.lz", bgName);
                load_nlobj(&g_some_nl_model_buffer_2, &g_some_tpl_buffer, gmaFileName, tplFileName);
                DVDChangeDir("/");
            }
            if (bg_work_sizes[themeId] != 0)
            {
                /* bg_work_sizes are the 32-bit sizes of the bg_*.c work structs,
                 * which hold pointers; on x64 they grow by up to 1.9x (measured:
                 * BgSpa2Work 0x170 -> 0x2B8), so scale by PC_PTR_SCALE. */
                world_theme.work = alloc_from_heap_or_panic(currentHeap, bg_work_sizes[themeId] * PC_PTR_SCALE, "background.c", 0x3D2);
                memset(world_theme.work, 0, bg_work_sizes[themeId] * PC_PTR_SCALE);
            }
        }
        OSSetCurrentHeap(oldHeap);
        world_theme.bgId = themeId;
        light_init(current_stage_id);
        fn_1_371C8();
    }
}

void fn_1_70268(void)
{
    lbl_1_bss_87F08 = NULL;
    lbl_1_bss_87F0C = &lbl_1_bss_87F08;
}

int fn_1_70290(struct BgShadowGroup *node)
{
    node->next = NULL;
    *lbl_1_bss_87F0C = node;
    lbl_1_bss_87F0C = &node->next;
    return 1;
}
struct ShadowTexInfo lbl_1_data_30FC8 = { 1 };

s16 s_nightWindowAModels[] =
{
    9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22,
};

s16 s_nightWindowBModels[] =
{
    23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 33,
};

s16 s_nightWindowCModels[] =
{
    34, 35, 36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 47, 48, 49,
    50, 51,
};

s16 s_nightWindowDModels[] =
{
    52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67,
    68, 69,
};

s16 s_nightWindowEModels[] =
{
    70, 71, 72, 73, 74, 75, 76, 77,
    78, 79, 80, 81, 82, 83, 84, 85,
    86, 87,
};

s16 s_nightWindowFModels[] =
{
    92, 93, 94, 95, 96, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107,
};

s16 s_nightWindowGModels[] =
{
    108, 109, 110, 111, 112, 113, 114, 115,
    116, 117, 118, 119, 120, 121, 122,
};

s16 s_stormFireModels[] =
{
    8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39,
};


void fn_1_70830(void);
int fn_1_709A8(struct BgShadowGroup *group, struct ShadowCastReq *req);
void fn_1_D4650(struct ShadowCastReq *req);
float fn_1_D5824(struct ShadowCastReq *req);

void g_draw_bg_model_shadows(void)
{
    struct ShadowCastReq req;
    Vec lightOfs;
    Vec center;
    Vec min;

    if (modeCtrl.unk30 <= 1)
    {
        struct BgShadowGroup *group;
        u32 mask;
        int i;
        struct Light *light = s_lightPool;
        struct StageBgModel **models;
        struct StageBgModel *model;
        struct GMAModel *gma;

        mtxa_from_rotate_y(light->rotY);
        mtxa_rotate_x(light->rotX);
        lightOfs.x = 0.0f;
        lightOfs.y = 0.0f;
        lightOfs.z = -15.0f;
        mtxa_tf_vec(&lightOfs, &lightOfs);
        req.unk30 = 0x46;
        req.unk34 = 0.075f;
        req.unk38 = 0.1f;
        req.up.x = 0.0f;
        req.up.y = 1.0f;
        req.up.z = 0.0f;
        mtxa_tf_vec(&req.up, &req.up);
        req.tex = lbl_1_data_30FC8;
        req.cameraMask = 0xFFFF;
        {
            float r = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.r));
            float g = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.g));
            float b = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.b));

            req.color.r = r;
            req.color.g = g;
            req.color.b = b;
        }
        mask = g_smth_with_challenge_mode_var_and_3p();
        fn_1_707FC();
        group = lbl_1_bss_87F08;
        while (group != NULL)
        {
            float maxX;
            float maxY;
            float maxZ;
            float radius;

            min.x = 20000.0f;
            min.y = 20000.0f;
            min.z = 20000.0f;
            maxX = -20000.0f;
            maxY = -20000.0f;
            maxZ = -20000.0f;
            models = group->models;
            for (i = group->count; i > 0; i--, models++)
            {
                model = *models;
                if ((model->flags & mask) != 0)
                {
                    mtxa_from_translate(&model->pos);
                    mtxa_rotate_z(model->rotZ);
                    mtxa_rotate_y(model->rotY);
                    mtxa_rotate_x(model->rotX);
                    mtxa_scale(&model->scale);
                    gma = model->model;
                    radius = model->scale.x;
                    radius = gma->boundSphereRadius * radius;
                    mtxa_tf_point(&gma->boundSphereCenter, &center);
                    if (min.x > center.x - radius)
                        min.x = center.x - radius;
                    if (min.y > center.y - radius)
                        min.y = center.y - radius;
                    if (min.z > center.z - radius)
                        min.z = center.z - radius;
                    if (maxX < center.x + radius)
                        maxX = center.x + radius;
                    if (maxY < center.y + radius)
                        maxY = center.y + radius;
                    if (maxZ < center.z + radius)
                        maxZ = center.z + radius;
                }
            }
            if (min.x > maxX)
            {
                group = group->next;
                continue;
            }
            center.x = 0.5f * (min.x + maxX);
            center.y = 0.5f * (min.y + maxY);
            center.z = 0.5f * (min.z + maxZ);
            maxZ = 0.0f;
            models = group->models;
            for (i = group->count; i > 0; i--, models++)
            {
                model = *models;
                mtxa_from_translate(&model->pos);
                mtxa_rotate_z(model->rotZ);
                mtxa_rotate_y(model->rotY);
                mtxa_rotate_x(model->rotX);
                mtxa_scale(&model->scale);
                gma = model->model;
                mtxa_tf_point(&gma->boundSphereCenter, &min);
                radius = model->scale.x;
                radius = gma->boundSphereRadius * radius + mathutil_vec_distance(&min, &center);
                if (maxZ < radius)
                    maxZ = radius;
            }
            radius = maxZ * group->scale;
            req.lightPos.x = lightOfs.x + center.x;
            req.lightPos.y = lightOfs.y + center.y;
            req.lightPos.z = lightOfs.z + center.z;
            req.unk10 = 2.0f * radius;
            req.pos = center;
            req.radius = radius;
            req.tex = lbl_1_data_30FC8;
            if (fn_1_709A8(group, &req) == 0)
            {
                group = group->next;
                continue;
            }
            fn_1_D4650(&req);
            group = group->next;
        }
        fn_1_70830();
    }
}


void fn_1_707FC(void)
{
    lbl_1_bss_87F10 = 0;
    lbl_1_bss_87F14 = 0;
    lbl_1_bss_87F16 = 0;
    lbl_1_bss_87F18 = 0;
    lbl_1_bss_87F1A = 0;
}
void fn_1_70830(void)
{
    if ((lbl_1_bss_87F10 & 1) != 0)
    {
        fn_1_10A69C(NULL);
        fn_1_E2E18();
    }
    if ((lbl_1_bss_87F10 & 4) != 0)
    {
        if ((u16)lbl_1_bss_87F14 != lbl_1_bss_87F18 || (u16)lbl_1_bss_87F16 != lbl_1_bss_87F1A)
        {
            GXSetViewport(0.0f, 0.0f, lbl_1_bss_87F18, lbl_1_bss_87F1A, 0.0f, 1.0f);
            GXSetScissor_cached(0, 0, lbl_1_bss_87F18, lbl_1_bss_87F1A);
            lbl_1_bss_87F10 |= 2;
        }
        g_smth_with_bg_color_drawing(&g_some_render_flag.bgColor);
        GXSetCopyClear_cached(g_some_render_flag.bgColor, 0xFFFFFF);
        lbl_1_bss_87F10 |= 2;
    }
    if ((lbl_1_bss_87F10 & 2) != 0)
        camera_apply_viewport(g_something_with_camera3);
}

int fn_1_709A8(struct BgShadowGroup *group, struct ShadowCastReq *req)
{
    Vec eye;
    Mtx view;
    Mtx44 proj;
    int texW;
    int texH;
    int w;
    int h;
    void *data;
    float radius;

    mtxa_push();
    radius = fn_1_D5824(req);
    mtxa_pop();
    if (radius <= 0.0f)
        return 0;
    texW = GXGetTexObjWidth(group->tex);
    texH = GXGetTexObjHeight(group->tex);
    w = texW * 2;
    h = texH * 2;
    if ((lbl_1_bss_87F10 & 1) == 0)
    {
        fn_1_70DAC();
        lbl_1_bss_87F10 |= 1;
    }
    req->radius *= (float)(w + 4) / (float)w;
    radius = req->radius;
    eye = req->lightPos;
    eye.x -= req->pos.x;
    eye.y -= req->pos.y;
    eye.z -= req->pos.z;
    vec_set_len(2.0f * radius, &eye, &eye);
    eye.x += req->pos.x;
    eye.y += req->pos.y;
    eye.z += req->pos.z;
    C_MTXLookAt(view, &eye, &req->up, &req->pos);
    C_MTXOrtho(proj, -radius, radius, radius, -radius, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(proj, 1);
    if (lbl_1_bss_87F14 != w || lbl_1_bss_87F16 != h)
    {
        GXSetViewport(0.0f, 0.0f, w, h, 0.0f, 1.0f);
        GXSetScissor_cached(0, 0, w, h);
        lbl_1_bss_87F14 = w;
        lbl_1_bss_87F16 = h;
    }
    lbl_1_bss_87F10 |= 2;
    if (lbl_1_bss_87F18 < w || lbl_1_bss_87F1A < h)
    {
        GXColor clearColor = { 0, 0, 0, 0 };

        g_smth_with_bg_color_drawing(&clearColor);
        if (lbl_1_bss_87F18 < w)
            lbl_1_bss_87F18 = w;
        if (lbl_1_bss_87F1A < h)
            lbl_1_bss_87F1A = h;
        if ((lbl_1_bss_87F10 & 4) == 0)
        {
            GXSetCopyClear_cached(clearColor, 0xFFFFFF);
            lbl_1_bss_87F10 |= 4;
        }
    }
    mtxa_push();
    {
        int i;
        struct StageBgModel **models;
        GXColor white;

        white.r = 0xFF;
        white.g = 0xFF;
        white.b = 0xFF;
        GXSetTevKColor_cached(0, white);
        models = group->models;
        for (i = group->count; i > 0; i--, models++)
        {
            struct StageBgModel *model = *models;

            mtxa_from_mtx(view);
            mtxa_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_scale(&model->scale);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model->model);
        }
    }
    mtxa_pop();
    data = GXGetTexObjData(group->tex);
    GXSetTexCopySrc_cached(0, 0, w, h);
    GXSetTexCopyDst_cached(texW, texH, group->texFmt, 1);
    GXCopyTex(data, 1);
    GXInitTexObj(&req->tex.texObj, data, (u16)texW, (u16)texH, GXGetTexObjFmt(group->tex), 0, 0, 0);
    return 1;
}

void fn_1_70DAC(void)
{
    GXSetCullMode_cached(2);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 0xFF);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xE);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    g_something_with_raster_state(0);
    fn_1_10A69C(fn_1_70E90);
}


int fn_1_70E90(void)
{
    return 1;
}

void fn_1_70E98(void)
{
    lbl_1_bss_87F08 = NULL;
    lbl_1_bss_87F0C = &lbl_1_bss_87F08;
}

void call_item_coin_coli_func_for_cur_world_theme(int a)
{
    if (world_theme.bgId > 0)
        bg_item_coin_coli_funcs[world_theme.bgId](a);
}

void bg_init_base(void)
{
    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
}

void bg_tick_base(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
}

void bg_dest_base(void) {}

void bg_disp_base(void)
{
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_item_coin_coli_base(int a) {}

void g_handle_bg_fg_model_anim(float time, struct StageBgModel *bgObj, int bgObjCount)
{
    int i;
    int r29;
    Vec boundSphereCenter;

    if (bgObj == NULL2)
        return;
    r29 = g_smth_with_challenge_mode_var_and_3p();
    for (i = 0; i < bgObjCount; i++, bgObj++)
    {
        float timeSecondsLooped;
        float loopDurationSeconds;
        struct StageBgAnim *anim;
        int timer;

        bgObj->flags &= ~(1 << 16);
        if (!(bgObj->flags & r29))
            continue;
        if (bgObj->model == NULL2)
            continue;
        bgObj->flags |= (1 << 16);
        anim = bgObj->anim;
        if (anim == NULL2)
            continue;
        timer = (bgObj->animFlags & 0xC000) >> 14;
        timeSecondsLooped = world_theme.timers[timer] / 60.0;
        if (bgObj->flags & (1 << 6))
            timeSecondsLooped = g_some_frame_counter.timer / 60.0;
        if (bgObj->animFlags & 1)
        {
            timeSecondsLooped += anim->loopStart;
            loopDurationSeconds = anim->loopEnd - anim->loopStart;
            if (timeSecondsLooped > loopDurationSeconds)
                timeSecondsLooped = loopDurationSeconds;
            timeSecondsLooped += anim->loopStart;
        }
        else
        {
            timeSecondsLooped += anim->loopStart;
            loopDurationSeconds = anim->loopEnd - anim->loopStart;
            timeSecondsLooped -=
                loopDurationSeconds * mathutil_floor(timeSecondsLooped / loopDurationSeconds);
            timeSecondsLooped += anim->loopStart;
        }
        if (anim->visible.frames != NULL2 &&
            evaluate_stagedef_keyframe(timeSecondsLooped, anim->visible.count, anim->visible.frames) < 0.5)
        {
            bgObj->flags &= ~(1 << 16);
            continue;
        }
        if (anim->translucency.frames != NULL2)
        {
            bgObj->translucency = evaluate_stagedef_keyframe(
                timeSecondsLooped, anim->translucency.count, anim->translucency.frames);
            if (bgObj->translucency >= 1.0)
                continue;
        }
        if (anim->scaleX.frames != NULL2)
            bgObj->scale.x = evaluate_stagedef_keyframe(timeSecondsLooped, anim->scaleX.count, anim->scaleX.frames);
        if (anim->scaleY.frames != NULL2)
            bgObj->scale.y = evaluate_stagedef_keyframe(timeSecondsLooped, anim->scaleY.count, anim->scaleY.frames);
        if (anim->scaleZ.frames != NULL2)
            bgObj->scale.z = evaluate_stagedef_keyframe(timeSecondsLooped, anim->scaleZ.count, anim->scaleZ.frames);
        if (anim->rotX.frames != NULL2)
            bgObj->rotX = DEGREES_TO_S16(evaluate_stagedef_keyframe(timeSecondsLooped, anim->rotX.count, anim->rotX.frames));
        if (anim->rotY.frames != NULL2)
            bgObj->rotY = DEGREES_TO_S16(evaluate_stagedef_keyframe(timeSecondsLooped, anim->rotY.count, anim->rotY.frames));
        if (anim->rotZ.frames != NULL2)
            bgObj->rotZ = DEGREES_TO_S16(evaluate_stagedef_keyframe(timeSecondsLooped, anim->rotZ.count, anim->rotZ.frames));
        if (anim->posX.frames != NULL2)
            bgObj->pos.x = evaluate_stagedef_keyframe(timeSecondsLooped, anim->posX.count, anim->posX.frames);
        if (anim->posY.frames != NULL2)
            bgObj->pos.y = evaluate_stagedef_keyframe(timeSecondsLooped, anim->posY.count, anim->posY.frames);
        if (anim->posZ.frames != NULL2)
            bgObj->pos.z = evaluate_stagedef_keyframe(timeSecondsLooped, anim->posZ.count, anim->posZ.frames);
        if ((bgObj->flags & (1 << 5)) && sub_mode != 0xE)
        {
            mtxa_from_translate(&bgObj->pos);
            mtxa_rotate_z(bgObj->rotZ);
            mtxa_rotate_y(bgObj->rotY);
            mtxa_rotate_x(bgObj->rotX);
            mtxa_tf_point(&bgObj->model->boundSphereCenter, &boundSphereCenter);
            g_something_with_translating_items(5, &boundSphereCenter, 1.0f);
        }
    }
}

void g_render_foreground_objects(Mtx viewFromWorld, struct StageBgModel *bgObj, int bgObjCount)
{
    struct GMAModel *model;
    int i;
    int r29;
    int scrollActive;
    void *r27;
    struct StageEffectHeader *effect;
    int customLightGroup;
    float scale;

    if (bgObj == NULL)
        return;
    r29 = g_smth_with_challenge_mode_var_and_3p();
    scrollActive = 0;
    r27 = world_theme.unk90;
    for (i = 0; i < bgObjCount; i++, bgObj++)
    {
        if (!(bgObj->flags & r29))
            continue;
        if ((g_some_render_flag.flags & (1 << 2)) && (bgObj->flags & (1 << 7)))
            continue;
        if (!(bgObj->flags & (1 << 16)))
            continue;
        if (bgObj->flags & (1 << 17))
            continue;
        if (bgObj->translucency >= 1.0)
            continue;
        if (bgObj->model == NULL)
            continue;
        model = bgObj->model;
        if (model->mtxCount != 0)
            continue;
        mtxa_from_mtx(viewFromWorld);
        mtxa_translate(&bgObj->pos);
        mtxa_rotate_z(bgObj->rotZ);
        mtxa_rotate_y(bgObj->rotY);
        mtxa_rotate_x(bgObj->rotX);
        mtxa_scale(&bgObj->scale);
        scale = MAX(bgObj->scale.x, bgObj->scale.y);
        scale = MAX(bgObj->scale.z, scale);
        if ((g_some_render_flag.flags & (1 << 2))
         && fn_1_8034(&model->boundSphereCenter) < -(scale * model->boundSphereRadius))
            continue;
        if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, scale) == 0)
            continue;
        customLightGroup = bgObj->flags >> 28;
        effect = bgObj->effect;
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        if (customLightGroup > 0)
        {
            push_light_group();
            g_set_active_light_group(customLightGroup + 11);
        }
        if (r27 != NULL && (bgObj->flags & (1 << 24)))
            g_yet_another_unk_draw_func(r27);
        if (effect != NULL && effect->textureScroll != NULL)
        {
            if (scrollActive == 0)
            {
                scrollActive = 1;
                g_something_with_texture_scroll_2(1);
            }
            g_something_with_texture_scroll(effect->textureScroll);
        }
        else if (scrollActive != 0)
        {
            scrollActive = 0;
            g_something_with_texture_scroll_2(0);
        }
        avdisp_set_scale_factor(scale);
        if (bgObj->translucency < FLT_EPSILON)
            avdisp_draw_model_unculled_sort_auto(model);
        else
        {
            avdisp_set_alpha(1.0 - bgObj->translucency);
            avdisp_draw_model_unculled_sort_always(model);
        }
        if (effect != NULL)
            fn_1_71844(viewFromWorld, effect);
        if (r27 != NULL)
            g_yet_another_unk_draw_func(NULL);
        if (customLightGroup > 0)
            pop_light_group();
    }
    if (scrollActive != 0)
        g_something_with_texture_scroll_2(0);
}
void fn_1_71844(Mtx viewFromWorld, struct StageEffectHeader *effect)
{
    u32 t;

    if (effect->fx1Keyframes != NULL && world_theme.bgId == 4)
    {
        struct StageNightWindowAnim *nightFlipbook = effect->fx1Keyframes;
        int i;

        for (i = 0; i < effect->nightWindowAnimCount; i++, nightFlipbook++)
        {
            int modelId;

            mtxa_from_mtx(viewFromWorld);
            mtxa_translate(&nightFlipbook->pos);
            mtxa_rotate_z(nightFlipbook->rotZ);
            mtxa_rotate_y(nightFlipbook->rotY);
            mtxa_rotate_x(nightFlipbook->rotX);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            t = g_font_char_var_3 / 2;
            switch (nightFlipbook->id)
            {
            default:
            case 65:
                modelId = s_nightWindowAModels[t % ARRAY_COUNT(s_nightWindowAModels)];
                break;
            case 66:
                modelId = s_nightWindowBModels[t % ARRAY_COUNT(s_nightWindowBModels)];
                break;
            case 67:
                modelId = s_nightWindowCModels[t % ARRAY_COUNT(s_nightWindowCModels)];
                break;
            case 68:
                modelId = s_nightWindowDModels[t % ARRAY_COUNT(s_nightWindowDModels)];
                break;
            case 69:
                modelId = s_nightWindowEModels[t % ARRAY_COUNT(s_nightWindowEModels)];
                break;
            case 70:
                modelId = s_nightWindowFModels[t % ARRAY_COUNT(s_nightWindowFModels)];
                break;
            case 71:
                modelId = s_nightWindowGModels[t % ARRAY_COUNT(s_nightWindowGModels)];
                break;
            }
            avdisp_draw_model_unculled_sort_never(g_bg_gma->modelEntries[modelId].model);
        }
    }
    if (effect->fx2Keyframes != NULL)
    {
        struct StageStormFireAnim *stormFlipbook = effect->fx2Keyframes;
        int i;

        for (i = 0; i < effect->stormFireAnimCount; i++, stormFlipbook++)
        {
            int modelId;

            mtxa_from_mtx(viewFromWorld);
            mtxa_translate(&stormFlipbook->pos);
            mtxa_rotate_y(g_current_camera->rotY);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            modelId = s_stormFireModels[(g_font_char_var_3 + stormFlipbook->frameOffset * 4)
                                        % ARRAY_COUNT(s_stormFireModels)];
            avdisp_draw_model_unculled_sort_auto(g_bg_gma->modelEntries[modelId].model);
        }
    }
}

void g_something_with_texture_scroll(float *scroll)
{
    float t = world_theme.animTimer;

    g_some_bg_mtx[0][3] = 0.016666668f * (scroll[0] * t);
    g_some_bg_mtx[1][3] = 0.016666668f * (scroll[1] * t);
    fn_1_10A4CC(0, g_some_bg_mtx);
}

void bg_nig_init(void)
{
    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
}

void bg_nig_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
}

void bg_nig_dest(void) {}

void bg_nig_disp(void)
{
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_nig_item_coin_coli(int a) {}

void bg_ice2_init(void)
{
    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
}

void bg_ice2_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
}

void bg_ice2_dest(void) {}

void bg_ice2_disp(void)
{
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_ice2_item_coin_coli(int a) {}

void bg_bil_init(void)
{
    int i;
    struct StageBgModel *model;
    struct BgModelFlagEntry *entry;
    int len;

    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
    model = stagedef->bgModels;
    for (i = 0; i < stagedef->bgModelCount; i++, model++)
    {
        entry = lbl_1_data_11008;
        while (entry->flag != -1)
        {
            int j;
            int last;
            int found;

            len = strlen((char *)entry->name);
            last = strlen(model->name) - 1;
            found = FALSE;
            for (j = 0; j < len; j++)
            {
                if ((u8)model->name[j] != entry->name[j])
                    break;
                if (last == j)
                {
                    found = TRUE;
                    break;
                }
            }
            if (found)
            {
                model->flags |= (entry->flag & 0xF) << 28;
                break;
            }
            entry++;
        }
    }
}

void bg_bil_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
}

void bg_bil_dest(void) {}

void bg_bil_disp(void)
{
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_bil_item_coin_coli(int a) {}

void bg_gol_init(void)
{
    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
}

void bg_gol_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
}

void bg_gol_dest(void) {}

void bg_gol_disp(void)
{
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_gol_item_coin_coli(int a) {}

void bg_bow_init(void)
{
    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
}

void bg_bow_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
}

void bg_bow_dest(void) {}

void bg_bow_disp(void)
{
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_bow_item_coin_coli(int a) {}

void bg_futsal_init(void)
{
    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
    g_load_ape_bg();
}

void bg_futsal_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
    fn_1_A00D0();
}

void bg_futsal_dest(void)
{
    fn_1_A01C8();
}

void bg_futsal_disp(void)
{
    fn_1_72A34();
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
    fn_1_A0298();
}

void fn_1_72A34(void)
{
    int i;

    for (i = 0; i < stagedef->bgModelCount; i++)
    {
        if (strstr(stagedef->bgModels[i].name, "CLIPOBJ") != NULL)
        {
            if (lbl_1_bss_87F4C != 0)
                stagedef->bgModels[i].flags &= ~(1 << 16);
            else
                stagedef->bgModels[i].flags |= (1 << 16);
        }
    }
}

void bg_futsal_item_coin_coli(int a) {}

GXColor lbl_1_data_31130[] = { { 0x00, 0x99, 0x7F, 0x00 }, { 0x00, 0x59, 0x4C, 0x00 } };

void bg_electric_init(void)
{
    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
}

void bg_electric_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
    {
        struct Light *light = alloc_pool_light(4, 0);

        if (light != NULL)
            light->rotY = 109.22667f * world_theme.animTimer;
    }
}

void bg_electric_dest(void) {}

void bg_electric_disp(void)
{
    if (lbl_1_bss_6F00 == 0)
    {
        float r;
        float g;
        float b;
        float t = 0.5f * (1.0f + math_sin(364.0889f * world_theme.animTimer));
        float u = 1.0f - t;

        r = 0.003921569f * (lbl_1_data_31130[1].r * t + lbl_1_data_31130[0].r * u);
        g = 0.003921569f * (lbl_1_data_31130[1].g * t + lbl_1_data_31130[0].g * u);
        b = 0.003921569f * (lbl_1_data_31130[1].b * t + lbl_1_data_31130[0].b * u);
        g_set_light_color_maybe(world_theme.unkAA, r, g, b);
        g_set_light_color_maybe(world_theme.unkA8, r, g, b);
    }
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_electric_item_coin_coli(int a) {}

void bg_boat_init(void)
{
    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
}

void bg_boat_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
}

void bg_boat_dest(void)
{
    fn_1_A01C8();
}

void bg_boat_disp(void)
{
    if (current_stage_id == 0x8C)
    {
        float camY = g_current_camera->eye.y;
        s16 i;

        for (i = stagedef->bgModelCount - 1; i >= 0; i--)
        {
            stagedef->bgModels[i].flags &= ~(1 << 16);
            if (camY < -3.0)
            {
                if (stagedef->bgModels[i].pos.y < -3.0)
                    stagedef->bgModels[i].flags |= (1 << 16);
            }
            else
            {
                if (stagedef->bgModels[i].pos.y >= -3.0)
                    stagedef->bgModels[i].flags |= (1 << 16);
            }
        }
    }
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_boat_item_coin_coli(int a) {}

struct BgParkAmbient
{
    s16 stageId;
    float r, g, b;
};

struct BgParkAmbient lbl_1_data_31138[] =
{
    { 402, 0.3f, 0.3f, 0.3f },
    { 408, 0.6f, 0.6f, 0.6f },
    { 411, 0.4f, 0.4f, 0.4f },
    { 412, 0.2f, 0.2f, 0.2f },
    { 399, 0.2f, 0.2f, 0.2f },
    { 419, 0.6f, 0.6f, 0.6f },
    { 420, 0.5f, 0.5f, 0.5f },
    { -1, 0.0f, 0.0f, 0.0f },
};

void bg_park_init(void)
{
    struct BgParkAmbient *entry;

    mtxa_from_identity();
    mtxa_to_mtx(g_some_bg_mtx);
    entry = lbl_1_data_31138;
    while (entry->stageId >= 0)
    {
        if (entry->stageId == current_stage_id)
            break;
        entry++;
    }
    if (entry->stageId < 0)
        entry = NULL;
    if (entry != NULL)
        set_bg_ambient(entry->r, entry->g, entry->b);
}

void bg_park_tick(void)
{
    float timeSeconds = world_theme.animTimer / 60.0;

    g_handle_bg_fg_model_anim(timeSeconds, stagedef->bgModels, stagedef->bgModelCount);
    g_handle_bg_fg_model_anim(timeSeconds, stagedef->fgModels, stagedef->fgModelCount);
}

void bg_park_dest(void) {}

void bg_park_disp(void)
{
    push_light_group();
    if ((stagedef->bgModels != NULL || stagedef->fgModels != NULL)
     && (g_some_render_flag.flags & 1))
        call_g_avdisp_set_ambient(0.5f, 0.5f, 0.5f);
    if (stagedef->bgModels != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        g_set_active_light_group(world_theme.unkAA);
    }
    g_render_foreground_objects(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if (stagedef->fgModels != NULL)
    {
        mtxa_from_mtx(mtxa->mtxB);
        g_set_active_light_group(world_theme.unkA8);
    }
    g_render_foreground_objects(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
    pop_light_group();
}

void bg_park_item_coin_coli(int a) {}

void g_smth_with_bg_model_names(struct BgModelSearch *searchList, int (*proc)(int, struct GMAModelEntry *))
{
    int i;
    int j;
    struct GMAModelEntry *gmaEntry;
    struct BgModelSearch *search;
    char *modelName;
    int len1;
    int r25;
    struct GMA *gma = g_bg_gma;

    if (gma != NULL)
    {
        r25 = 1;
        gmaEntry = gma->modelEntries;
        for (i = gma->numModels; i > 0; i--, gmaEntry++)
        {
            modelName = gmaEntry->name;
            len1 = strlen(modelName);
            for (search = searchList, j = 0; search->name != NULL; j++, search++)
            {
                int matched;

                switch (search->cmpType)
                {
                case 0:
                    matched = !strncmp(modelName, search->name, strlen(search->name));
                    break;
                case 1:
                    matched = !strcmp(modelName, search->name);
                    break;
                case 2:
                    {
                        int len2 = strlen(search->name);

                        if (len2 > len1)
                            matched = FALSE;
                        else
                            matched = !strncmp(modelName + (len1 - len2), search->name, len2);
                    }
                    break;
                default:
                    matched = FALSE;
                    break;
                }
                if (matched)
                {
                    r25 = proc(j, gmaEntry);
                    if (r25 == 0)
                        break;
                }
                if (r25 == 0)
                    break;
            }
            if (r25 == 0)
                break;
        }
    }
}

void g_init_stagedef_bg_fg_models(struct StageBgModel *bgObj, int bgObjCount, struct BgModelSearch *searchList, int (*proc)(int, struct StageBgModel *))
{
    int i;
    int j;
    int r25 = 1;
    struct BgModelSearch *search;
    char *modelName;
    int len1;

    for (i = bgObjCount; i > 0; i--, bgObj++)
    {
        if (bgObj->model != NULL)
        {
            modelName = bgObj->name;
            len1 = strlen(modelName);
            for (search = searchList, j = 0; search->name != NULL; j++, search++)
            {
                int matched;

                switch (search->cmpType)
                {
                case 0:
                    matched = !strncmp(modelName, search->name, strlen(search->name));
                    break;
                case 1:
                    matched = !strcmp(modelName, search->name);
                    break;
                case 2:
                    {
                        int len2 = strlen(search->name);

                        if (len2 > len1)
                            matched = FALSE;
                        else
                            matched = !strncmp(modelName + (len1 - len2), search->name, len2);
                    }
                    break;
                default:
                    matched = FALSE;
                    break;
                }
                if (matched)
                {
                    r25 = proc(j, bgObj);
                    if (r25 == 0)
                        break;
                }
                if (r25 == 0)
                    break;
            }
            if (r25 == 0)
                break;
        }
    }
}

void fn_1_7385C(void)
{
    int i;
    struct StageBgModel *model;

    model = stagedef->bgModels;
    for (i = 0; i < stagedef->bgModelCount; i++, model++)
    {
        if (model->model != NULL)
            model->flags &= 0xFFFFFF;
    }
    model = stagedef->fgModels;
    for (i = 0; i < stagedef->fgModelCount; i++, model++)
    {
        if (model->model != NULL)
            model->flags &= 0xFFFFFF;
    }
}

void fn_1_73900(u32 a)
{
    world_theme.unkA0 = a;
}


int is_game_paused_and_in_view_stage(void)
{
    int paused = FALSE;

    if ((g_some_other_flags & 0xA) && events[13].state != 2)
        paused = TRUE;
    return paused;
}

int g_smth_with_challenge_mode_var_and_3p(void)
{
    if (g_some_render_flag.flags & 0x11)
        return 0x10;
    if (main_mode == 2 || main_mode == 4)
    {
        if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == 3)
            return 8;
        return 1 << (modeCtrl.unk30 - 1);
    }
    return 1;
}
