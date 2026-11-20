/* Monkey Bowling 2: main game flow, scoring, UI and ball handling. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "gx.h"
#include "input.h"
#include "item.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "rand.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "textdraw.h"
#include "world.h"

#ifndef __MWERKS__
/* pc_port: undeclared in the original; the float radius must not be
 * promoted to double on x86. */
#include "frustum.h"
#endif

#define STRUCT_COPY(d, s, n) memcpy(&(d), &(s), n)
extern struct GMA *minigameGma[];
#define MODEL(n) minigameGma[0]->modelEntries[n].model

struct BowlPlayer
{
    char *u0;
    s16 u4;
    s8 u6;
    s8 u7;
    s8 u8;
    s8 u9;
    s16 frameScore[11];
    s8 u20[0x15];
    s8 u35[0x15];
};

struct BowlLane
{
    s16 u0;
    s8 u2;
    s8 u3;
    s8 u4[0x14];
};

struct Bss1A8
{
    u16 u0;
    s8 u2;
    s8 u3;
    s8 u4;
};

u32 *OSGetStackPointer(void);
void SoundDop(u16 soundId, u16 pitch);
void SoundGroupLoad(int);
void SoundIcsReq(u32 soundId, u8 intensity, s8 volume);
void SoundPan(u16 soundId, s8 pan, s8 span);
void SoundVol(u16 soundId, u8 volume);
void call_SoundReqID_arg_0(int soundId);
int call_SoundReqID_arg_1_copy(u32 soundId);
void dest_all_events(void);
void destroy_sprite_with_unique_id(int tag);
BOOL did_ball_fallout(struct Ball *ball);
void fade_screen_to_color(s32 type, u32 color, int duration);
void fn_1_113190(struct Ape *ape, int a, float speed, int b, int c);
void *fn_1_1135A4(struct Ape *ape);
void fn_1_11C5E4(void);
void fn_1_11DE54(int a, int b);
int fn_1_11DF64(int a, u8 *b, int c);
int fn_1_11E1C0(int a, void *b);
int fn_1_11E3A4(void);
void fn_1_120AD8(void);
void fn_1_16C424(int val);
u8 fn_1_16C474(void);
void fn_1_2810(void);
void fn_1_296C(void);
void fn_1_3061C(int a, int b);
void fn_1_32338(s8 reverb, s8 chorus);
void fn_1_358E8(void);
void fn_1_56C18(void);
void fn_1_D1F28(struct Ape *ape);
void fn_1_D2510(struct Ape *ape);
void fn_1_D29E0(int id);
void fn_1_D4870(void);
void fn_1_E4708(s8 *charas, s8 *ranks);
int fn_1_E5DC8(void);
void fn_1_E6760(Mtx mtx);
void fn_1_E6BD0(int state);
void fn_1_E7600(void);
#ifdef __MWERKS__
void fn_1_E7620(GXColor color);
#else
/* pc_port: MWCC passes structs by invisible reference, so this by-value
 * declaration and mini_commend.c's GXColor* definition agree on GC; on x86
 * they do not, so route the call through the address. */
void fn_1_E7620(GXColor *color);
#define fn_1_E7620(c) fn_1_E7620(&(c))
#endif
struct BowlRecord { char name[7]; u8 chara; u16 score; };
void fn_5_10B98(void);
void fn_5_10D28(void);
void fn_5_10F40(void);
void fn_5_111E4(int type);
void fn_5_11ADC(void);
void fn_5_12194(int slot);
void fn_5_121E4(int slot);
void fn_5_12234(void);
void fn_5_12264(struct Ball *ball);
void fn_5_12448(void);
void fn_5_1256C(Vec *pos, s8 index);
void fn_5_1266C(void);
int fn_5_126C8(s8 *outCount);
void fn_5_127C8(void);
void fn_5_1280C(void);
void fn_5_12860(float y);
void fn_5_128C4(float z);
int fn_5_12A68(void);
void fn_5_12BAC(int slot);
void fn_5_12C28(int slot);
void fn_5_14B74(int counter, int playerNum);
void fn_5_14E3C(int counter, char *text);
void fn_5_15108(int counter, char *text);
void fn_5_153D4(int counter, int pins);
void fn_5_1570C(int counter);
void fn_5_15918(int counter);
void fn_5_15B24(void);
void fn_5_15E3C(int counter);
void fn_5_15F2C(void);
void fn_5_16358(void);
void fn_5_164E4(void);
void fn_5_165B4(int slot);
void fn_5_165E8(int slot);
void fn_5_1661C(s8 lane, s8 variant);
void fn_5_169B4(s8 lane);
void fn_5_169F0(s8 variant);
float fn_5_16B64(Vec *pos);
void fn_5_C734(struct Ball *ball);
void fn_5_C850(int slot, struct Ball *ball);
void fn_5_C8CC(int slot, struct Ball *ball);
void fn_5_D710(struct Camera *camera, struct Ball *ball);
void free_minigame_gma_tpl(void);
s16 g_SoftStreamStart_with_some_defaults(int track);
int g_check_some_condition(int a);
void g_create_stage_player_wrapper(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod,
                                   void (*drawFunc)(struct Ape *));
void g_draw_ball_and_ape(void);
void g_draw_bg(void);
void g_fade_track_volume(int a, int b);
float g_get_string_sprite_width_2(char *str);
void g_init_rendefc_for_stage(void);
void g_load_minigame_gma_tpl(int idx);
void g_load_stage_for_menu_bg(s8 a, int b);
void g_maybe_some_bg_effect_handler(void);
void g_reset_cm_course(void);
void g_return_to_sel_ngc(int a);
void g_set_minigame_specific_funcs(void (*tick)(void), void (*draw)(void), void *menu, void *camera);
void g_set_up_debugtext_buffer(void);
struct ShadowReceive
{
    /*0x00*/ void *obj;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ void *model;
};
void g_some_shadow_draw_func(struct ShadowReceive *s);
void g_something_calls_sndPopGroup(void);
void g_something_with_drawing_apes(void);
void g_something_with_iteratively_freeing_memory(void);
void g_something_with_rend_efc(int mask);
void g_something_with_translating_items(int a, Vec *b, float c);
void g_very_similar_to_puts(const char *);
void *g_yet_another_unk_draw_func(void *func);
int get_next_player_idx(void);
void handle_test_camera(void);
extern s8 lbl_1_bss_1700[];
extern s32 active_monkey_id[];
extern s32 lbl_1_bss_70E98[];
void load_bmp_by_id(int id);
void load_commend_stage(void);
void load_stage(int stage);
void mtxa_to_quat(Quat *q);
void ord_tbl_draw_nodes(void);
void preload_ape(int chara, int lod, int mesh, int scene);
void queue_disc_read(char *file);
void queue_stage_load(int stageId);
void reset_ui_widescreen_scale_mtx(void);
void set_ui_widescreen_scale_mtx(int width);
void textdraw_clear_flags(u32 flags);
void textdraw_printf(char *fmt, ...);
void textdraw_put_char(u8 c);
void textdraw_reset(void);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_rot_z(int rot);
void textdraw_set_scale(float x, float y);
int DVDChangeDir(char *dir);
void PSVECScale(Vec *in, Vec *out, float scale);
void event_dest(int id);
void event_init(int id);
void fn_1_461C0(struct Ape *ape, float speed);
void textdraw_print(char *str);
extern s32 lbl_5_bss_1BB94;
extern u32 g_some_render_flag;
/* SMB1's backgroundInfo; only the field used here is named. */
struct WorldTheme
{
#ifdef __MWERKS__
    u8 unk0[0x8C];
#else
    /* struct BackgroundInfo's layout up to unk8C (as in stage_draw.c): it
     * has native pointers, so on 64-bit drawFunc is at 0x98, not 0x8C */
    u8 unk0[0x78];
    void *unk78;
    void *unk7C;
    void *unk80;
    float unk84;
    u8 filler88[4];
#endif
    /*0x8C*/ void *drawFunc;
};
#ifndef __MWERKS__
/* must match background.h's struct BackgroundInfo unk8C */
_Static_assert(__builtin_offsetof(struct WorldTheme, drawFunc) == (sizeof(void *) == 8 ? 0x98 : 0x8C), "WorldTheme");
#endif
int g_check_some_condition(int a);
void g_something_with_texture_scroll(void *scroll);
extern u32 monkey_flags;
extern u32 perfInfo[];
extern s32 locale;
extern s32 g_related_to_pause_menu_input[];

void g_init_bowling(void);
void fn_5_7DC(void);
void fn_5_D34(void);
void fn_5_16DC(void);
void fn_5_1B7C(struct Sprite *arg0);
void fn_5_1B9C(void);
void fn_5_2FF0(void);
void fn_5_30A0(void);
void fn_5_3380(void);
void fn_5_34CC(void);
void fn_5_3798(void);
void fn_5_39E8(void);
void fn_5_3CA0(void);
void fn_5_4184(void);
void fn_5_4518(void);
void fn_5_490C(void);
void fn_5_533C(void);
void fn_5_592C(void);
void fn_5_5CDC(void);
void fn_5_5D94(void);
void fn_5_6250(void);
void fn_5_638C(void);
void fn_5_65A4(void);
void fn_5_6AE8(void);
void fn_5_6C00(void);
void fn_5_70C4(void);
void fn_5_7238(void);
void fn_5_751C(void);
void fn_5_7A5C(void);
void fn_5_7BEC(void);
void fn_5_7DB8(struct BowlPlayer *player);
void fn_5_830C(s32 player);
void fn_5_83D4(struct BowlPlayer *player);
void fn_5_84B4(struct BowlPlayer *player);
char *fn_5_87E0(void);
void fn_5_88CC(float a);
void fn_5_8F38(float a, float b);
void fn_5_9C90(float a, float b);
s8 fn_5_A580(struct BowlPlayer *player, s8 index);
s8 fn_5_A608(s8 pins);
void g_bowl_drawFunc(struct Ape *ape, int n);
void fn_5_A89C(struct Ape *ape, float speed);
void fn_5_B30C(float x, float y, u32 color, char *str);
void fn_5_B3C8(void);
void fn_5_BC78(void);
void fn_5_BE18(s32 a, s32 b, int n);
void fn_5_BEB4(void);
void fn_5_BF50(void);
int fn_5_BF88(void);
void fn_5_C0D0(s8 a);
void fn_5_C0F4(void);
void fn_5_C1D4(struct Ape *ape);
int fn_5_C3C8(void);
void g_load_missing_apes(void);
void g_load_commend_apes(void);
void g_load_selthing(void);
s32 ballid_matches_chara(s32 arg0);

static const struct SpriteDrawRequest spr0 = { 75, 0.0f, 0.0f, 0.30000001f, 80.0f, 60.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.5f, -1, 0x5, 0, 0 };
static const struct SpriteDrawRequest spr1 = { 1550, 76.0f, 398.0f, 0.13f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr2 = { 1555, 116.0f, 411.0f, 0.1123f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr3 = { 1554, 210.0f, 417.0f, 0.1012f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr4 = { 1553, 221.0f, 423.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
static const struct SpriteDrawRequest spr5 = { 1544, 71.0f, 420.0f, 0.13f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0, 1.0f, -1, 0xB, -1, 0 };
static const struct SpriteDrawRequest spr6 = { 1544, 71.0f, 420.0f, 0.13f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x9, -1, 0 };
static const struct SpriteDrawRequest spr7 = { 1539, 71.0f, 420.0f, 0.14f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x100009, -1, 0 };
static const struct SpriteDrawRequest spr8 = { 1539, 71.0f, 420.0f, 0.14f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0, 1.0f, -1, 0x10000B, -1, 0 };
static const struct SpriteDrawRequest spr9 = { 1539, 71.0f, 420.0f, 0.14f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x18000B, -1, 0 };
static const struct SpriteDrawRequest sprA = { 1539, 71.0f, 420.0f, 0.14f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0, 1.0f, -1, 0x180009, -1, 0 };
static const struct SpriteDrawRequest sprB = { 1539, 71.0f, 420.0f, 0.14f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x100009, -1, 0 };
static const struct SpriteDrawRequest sprC = { 1539, 71.0f, 420.0f, 0.14f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0, 1.0f, -1, 0x10000B, -1, 0 };
static const struct SpriteDrawRequest sprD[4] = {
    { 1542, 71.0f, 420.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1541, 71.0f, 420.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1543, 71.0f, 420.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1540, 71.0f, 420.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
};
static const struct SpriteDrawRequest spr11[4] = {
    { 1546, 71.0f, 420.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1549, 71.0f, 420.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1547, 71.0f, 420.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1548, 71.0f, 420.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
};
static const struct SpriteDrawRequest spr15[5] = {
    { 1545, 60.0f, 390.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -12743, 1.0f, -1, 0xA, -1, 0 },
    { 1545, 45.0f, 402.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -6371, 1.0f, -1, 0xA, -1, 0 },
    { 1545, 39.0f, 420.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1545, 45.0f, 438.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 6371, 1.0f, -1, 0xA, -1, 0 },
    { 1545, 60.0f, 450.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 12743, 1.0f, -1, 0xA, -1, 0 },
};
static const struct SpriteDrawRequest spr1A[5] = {
    { 1545, 82.0f, 390.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 12743, 1.0f, -1, 0xA, -1, 0 },
    { 1545, 97.0f, 402.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 6371, 1.0f, -1, 0xA, -1, 0 },
    { 1545, 103.0f, 420.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1545, 97.0f, 438.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -6371, 1.0f, -1, 0xA, -1, 0 },
    { 1545, 82.0f, 450.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -12743, 1.0f, -1, 0xA, -1, 0 },
};
static const struct SpriteDrawRequest spr1F = { 1551, 505.0f, 370.0f, 0.1123f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr20[10] = {
    { 1552, 560.0f, 436.0f, 0.1f, 0.2f, 0.5f, 0.0f, 0.0f, 0.2f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 549.0f, 418.0f, 0.1f, 0.2f, 0.5f, 0.2f, 0.0f, 0.40000001f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 571.0f, 418.0f, 0.1f, 0.2f, 0.5f, 0.40000001f, 0.0f, 0.60000002f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 537.0f, 400.0f, 0.1f, 0.2f, 0.5f, 0.60000002f, 0.0f, 0.80000001f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 560.0f, 400.0f, 0.1f, 0.2f, 0.5f, 0.80000001f, 0.0f, 1.0f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 583.0f, 400.0f, 0.1f, 0.2f, 0.5f, 0.0f, 0.5f, 0.2f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 526.0f, 382.0f, 0.1f, 0.2f, 0.5f, 0.2f, 0.5f, 0.40000001f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 549.0f, 382.0f, 0.1f, 0.2f, 0.5f, 0.40000001f, 0.5f, 0.60000002f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 571.0f, 382.0f, 0.1f, 0.2f, 0.5f, 0.60000002f, 0.5f, 0.80000001f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 594.0f, 382.0f, 0.1f, 0.2f, 0.5f, 0.80000001f, 0.5f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
};
static const struct SpriteDrawRequest spr2A[] = {
    { 1536, 0.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    { 1536, 104.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 49152, 1.0f, -1, 0xD, -1, 0 },
    { 1536, 0.0f, 88.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 16384, 1.0f, -1, 0x7, -1, 0 },
    { 1536, 104.0f, 88.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 32768, 1.0f, -1, 0xF, -1, 0 },
    { 1537, 8.0f, 0.0f, 0.1f, 12.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
    { 1537, 8.0f, 88.0f, 0.1f, 12.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 32768, 1.0f, -1, 0xF, -1, 0 },
    { 1537, 0.0f, 8.0f, 0.1f, 10.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 16384, 1.0f, -1, 0x7, -1, 0 },
#ifndef __MWERKS__
    /* spr31: the GC code reads it as spr2A[7], the next object */
    { 1537, 104.0f, 8.0f, 0.1f, 10.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 49152, 1.0f, -1, 0xD, -1, 0 },
#endif
};
#ifdef __MWERKS__
static const struct SpriteDrawRequest spr31 = { 1537, 104.0f, 8.0f, 0.1f, 10.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 49152, 1.0f, -1, 0xD, -1, 0 };
#endif
static const struct SpriteDrawRequest spr32 = { 1551, 0.0f, 0.0f, 0.12f, 0.80000001f, 0.80000001f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr33[] = {
    { 1552, 44.0f, 52.799999f, 0.11f, 0.16f, 0.40000001f, 0.0f, 0.0f, 0.2f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 35.200001f, 38.400002f, 0.11f, 0.16f, 0.40000001f, 0.2f, 0.0f, 0.40000001f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 52.799999f, 38.400002f, 0.11f, 0.16f, 0.40000001f, 0.40000001f, 0.0f, 0.60000002f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 25.6f, 24.0f, 0.11f, 0.16f, 0.40000001f, 0.60000002f, 0.0f, 0.80000001f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 44.0f, 24.0f, 0.11f, 0.16f, 0.40000001f, 0.80000001f, 0.0f, 1.0f, 0.5f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 62.400002f, 24.0f, 0.11f, 0.16f, 0.40000001f, 0.0f, 0.5f, 0.2f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 16.799999f, 9.6000004f, 0.11f, 0.16f, 0.40000001f, 0.2f, 0.5f, 0.40000001f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 35.200001f, 9.6000004f, 0.11f, 0.16f, 0.40000001f, 0.40000001f, 0.5f, 0.60000002f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
    { 1552, 52.799999f, 9.6000004f, 0.11f, 0.16f, 0.40000001f, 0.60000002f, 0.5f, 0.80000001f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
#ifndef __MWERKS__
    /* spr3C: the GC code reads it as spr33[9], the next object */
    { 1552, 71.199997f, 9.6000004f, 0.11f, 0.16f, 0.40000001f, 0.80000001f, 0.5f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 },
#endif
};
#ifdef __MWERKS__
static const struct SpriteDrawRequest spr3C = { 1552, 71.199997f, 9.6000004f, 0.11f, 0.16f, 0.40000001f, 0.80000001f, 0.5f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, -1, 0 };
#endif
static const struct SpriteDrawRequest spr3D = { 1560, 2.0f, 2.0f, 0.12f, 1.25f, 0.875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -96, 0 };
static const struct SpriteDrawRequest spr3E = { 1561, 0.0f, 0.0f, 0.13f, 0.375f, 0.359375f, 0.0f, 0.0f, 0.375f, 0.359375f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr3F = { 1561, 57.0f, 0.0f, 0.13f, 0.26249999f, 0.359375f, 0.35624999f, 0.0f, 0.61874998f, 0.359375f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr40 = { 1561, 57.0f, 0.0f, 0.13f, 0.35624999f, 0.359375f, 0.61874998f, 0.0f, 0.97500002f, 0.359375f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr41 = { 1561, 57.0f, 0.0f, 0.13f, 0.025f, 0.359375f, 0.97500002f, 0.0f, 1.0f, 0.359375f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr42 = { 1561, 0.0f, 23.0f, 0.13f, 0.35624999f, 0.46875f, 0.0f, 0.359375f, 0.35624999f, 0.828125f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr43[] = {
    { 1562, 0.0f, 23.0f, 0.13f, 0.890625f, 0.25f, 0.0f, 0.0f, 0.890625f, 0.25f, 0, 1.0f, -1, 0x5, -1, 0 },
    { 1562, 0.0f, 23.0f, 0.13f, 0.890625f, 0.25f, 0.0f, 0.25f, 0.890625f, 0.5f, 0, 1.0f, -1, 0x5, -1, 0 },
    { 1562, 0.0f, 23.0f, 0.13f, 0.890625f, 0.25f, 0.0f, 0.5f, 0.890625f, 0.75f, 0, 1.0f, -1, 0x5, -1, 0 },
#ifndef __MWERKS__
    /* spr46: the GC code reads it as spr43[3], the next object */
    { 1562, 0.0f, 23.0f, 0.13f, 0.890625f, 0.25f, 0.0f, 0.75f, 0.890625f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 },
#endif
};
#ifdef __MWERKS__
static const struct SpriteDrawRequest spr46 = { 1562, 0.0f, 23.0f, 0.13f, 0.890625f, 0.25f, 0.0f, 0.75f, 0.890625f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
#endif
static const struct SpriteDrawRequest spr47 = { 1561, 57.0f, 23.0f, 0.13f, 0.26249999f, 0.46875f, 0.35624999f, 0.359375f, 0.61874998f, 0.828125f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr48 = { 1561, 57.0f, 23.0f, 0.13f, 0.35624999f, 0.46875f, 0.61874998f, 0.359375f, 0.97500002f, 0.828125f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr49 = { 1561, 57.0f, 23.0f, 0.13f, 0.025f, 0.46875f, 0.97500002f, 0.359375f, 1.0f, 0.828125f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr4A = { 1561, 0.0f, 53.0f, 0.13f, 0.35624999f, 0.171875f, 0.0f, 0.828125f, 0.35624999f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr4B = { 1561, 57.0f, 53.0f, 0.13f, 0.26249999f, 0.171875f, 0.35624999f, 0.828125f, 0.61874998f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr4C = { 1561, 57.0f, 53.0f, 0.13f, 0.35624999f, 0.171875f, 0.61874998f, 0.828125f, 0.97500002f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr4D = { 1561, 57.0f, 53.0f, 0.13f, 0.025f, 0.171875f, 0.97500002f, 0.828125f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr4E = { 1563, 2.0f, 2.0f, 0.11f, 0.1923077f, 0.16249999f, 0.76923078f, 0.5f, 0.96153843f, 0.67500001f, 0, 1.0f, -1, 0x5, -9221598, 0 };
static const struct SpriteDrawRequest spr4F[] = {
    { 1563, 8.0f, 6.0f, 0.1f, 0.38461539f, 0.25f, 0.0f, 0.5f, 0.38461539f, 0.75f, 0, 1.0f, -1, 0x5, 6502930, 0 },
    { 1563, 8.0f, 6.0f, 0.1f, 0.38461539f, 0.25f, 0.38461539f, 0.5f, 0.76923078f, 0.75f, 0, 1.0f, -1, 0x5, 6502930, 0 },
    { 1563, 8.0f, 6.0f, 0.1f, 0.38461539f, 0.25f, 0.0f, 0.75f, 0.38461539f, 1.0f, 0, 1.0f, -1, 0x5, 6502930, 0 },
#ifndef __MWERKS__
    /* spr52: the GC code reads it as spr4F[3], the next object */
    { 1563, 8.0f, 6.0f, 0.1f, 0.38461539f, 0.25f, 0.38461539f, 0.75f, 0.76923078f, 1.0f, 0, 1.0f, -1, 0x5, 6502930, 0 },
#endif
};
#ifdef __MWERKS__
static const struct SpriteDrawRequest spr52 = { 1563, 8.0f, 6.0f, 0.1f, 0.38461539f, 0.25f, 0.38461539f, 0.75f, 0.76923078f, 1.0f, 0, 1.0f, -1, 0x5, 6502930, 0 };
#endif
static const struct SpriteDrawRequest spr53 = { 1560, 2.0f, 2.0f, 0.12f, 0.59375f, 0.875f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -96, 0 };
static const struct SpriteDrawRequest spr54 = { 1561, 57.0f, 0.0f, 0.13f, 0.14375f, 0.359375f, 0.35624999f, 0.0f, 0.48750001f, 0.359375f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr55 = { 1561, 57.0f, 23.0f, 0.13f, 0.14375f, 0.46875f, 0.35624999f, 0.609375f, 0.48750001f, 0.828125f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr56 = { 1561, 57.0f, 53.0f, 0.13f, 0.14375f, 0.171875f, 0.35624999f, 0.828125f, 0.48750001f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr57 = { 1561, 0.0f, 0.0f, 0.13f, 0.23125f, 0.359375f, 0.33125001f, 0.0f, 0.5625f, 0.359375f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr58 = { 1561, 0.0f, 23.0f, 0.13f, 0.025f, 0.46875f, 0.33125001f, 0.359375f, 0.35624999f, 0.828125f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr59 = { 1561, 0.0f, 23.0f, 0.13f, 0.20625f, 0.46875f, 0.61874998f, 0.609375f, 0.82499999f, 0.828125f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr5A = { 1561, 0.0f, 53.0f, 0.13f, 0.23125f, 0.171875f, 0.33125001f, 0.828125f, 0.5625f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr5B = { 1565, 0.0f, 24.0f, 0.12f, 12.307693f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
static const struct SpriteDrawRequest spr5C = { 1538, 515.0f, 446.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xB, -1, 0 };
static const struct SpriteDrawRequest spr5D = { 1556, 547.0f, 446.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xB, -1, 0 };
static const struct SpriteDrawRequest spr5E = { 1556, 579.0f, 446.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xB, -1, 0 };
static const struct SpriteDrawRequest spr5F = { 1556, 611.0f, 446.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xB, -1, 0 };
static const struct SpriteDrawRequest spr60 = { 75, 0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.40000001f, -1, 0x5, 0, 0 };

static u8 data_0[0x170] = {
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

static char *txtStandard[6] = {
    "Standard",
    "Standard",
    "Standard",
    "Est\xE1ndar",
    "Standard",
    "Standard",
};
static char *txtBucket[6] = {
    "Bucket",
    "Eimer",
    "Seau",
    "Cubo",
    "Cestino",
    "Bucket",
};
static char *txtBabySplit[6] = {
    "Baby split",
    "Baby-Split",
    "Petite s\xE9paration",
    "Mini divisi\xF3n",
    "Piccolo split",
    "Baby split",
};
static char *txtFitSplit[6] = {
    "Fit split",
    "Fit-Split",
    "S\xE9paration \xE9quilibr\xE9\x65",
    "Divisi\xF3n correcta",
    "Split giusto",
    "Fit split",
};
static char *txtClothesline[6] = {
    "Clothesline",
    "****",
    "****",
    "****",
    "****",
    "Clothesline",
};
static char *txtWashOut[6] = {
    "Wash out",
    "Washout",
    "Lessive",
    "Fracaso",
    "Ripulita",
    "Wash out",
};
static char *txtChristmasTree[6] = {
    "Christmas tree",
    "Weihnachtsbaum",
    "Sapin de No\xEBl",
    "\xC1rbol Navidad",
    "Albero di natale",
    "Christmas tree",
};
static char *txtDoubleFit[6] = {
    "Double fit",
    "****",
    "****",
    "****",
    "****",
    "Double fit",
};
static char *txtBigThree[6] = {
    "Big three",
    "Gro\xDF\x65 Drei",
    "Grand trois",
    "El gran tres",
    "Il grande tre",
    "Big three",
};
static char *txtBabySplitWithCompany[6] = {
    "Baby split with company",
    "****",
    "****",
    "****",
    "****",
    "Baby split with company",
};
static char *txtLightSeven[6] = {
    "Light seven",
    "****",
    "****",
    "****",
    "****",
    "Light seven",
};
static char *txtTriangle[6] = {
    "Triangle",
    "****",
    "****",
    "****",
    "****",
    "Triangle",
};
static u32 data_488 = 1;
static u32 data_48C = 0x61;
static s8 data_490 = 1;
static s8 data_491 = 3;
static s8 data_492 = 2;
static s32 data_494 = -1;

static char *txtMove[6] = {
    "Move",
    "Bewegen",
    "D\xE9pl.",
    "Mover",
    "Muovi",
    "\x82\xA2\x82\xC7\x82\xA4",
};
static char *txtSelect[6] = {
    "Select",
    "****",
    "****",
    "****",
    "****",
    "\x82\xA2\x82\xBF",
};
static char *txtPosition[6] = {
    "Position",
    "****",
    "****",
    "****",
    "****",
    "\x82\xAB\x82\xDF\x82\xE9",
};
static char *txtChange[6] = {
    "Change",
    "****",
    "****",
    "****",
    "****",
    "\x82\xB5\x82\xC4\x82\xF1",
};
static char *txtView[6] = {
    "View",
    "****",
    "****",
    "****",
    "****",
    "\x82\xAB\x82\xE8\x82\xA9\x82\xA6",
};
static char *txtSelect2[6] = {
    "Select",
    "****",
    "****",
    "****",
    "****",
    "\x82\xD9\x82\xA4\x82\xB1\x82\xA4",
};
static char *txtAngle[6] = {
    "Angle",
    "****",
    "****",
    "****",
    "****",
    "\x82\xAB\x82\xDF\x82\xE9",
};
static char *txtCancel[6] = {
    "Cancel",
    "****",
    "****",
    "****",
    "****",
    "\x82\xE0\x82\xC7\x82\xE9",
};
static char *txtEmpty[6] = {
    "",
    "****",
    "****",
    "****",
    "****",
    "",
};
static char *txtSelect3[6] = {
    "Select",
    "****",
    "****",
    "****",
    "****",
    "\x82\xC2\x82\xE6\x82\xB3",
};
static char *txtPower[6] = {
    "Power",
    "****",
    "****",
    "****",
    "****",
    "\x82\xAB\x82\xDF\x82\xE9",
};
static char *txtSpin[6] = {
    "Spin",
    "Taste",
    "Effet",
    "Efecto",
    "Effetto",
    "\x82\xA9\x82\xA2\x82\xC4\x82\xF1",
};
static char *txtWhichWillYouChallenge[6] = {
    "Which will you challenge",
    "Welches Muster spielst du",
    "Lequel vas-tu jouer ",
    "\xBF\x43u\xE1l desafiar\xE1s?",
    "Quale sfiderai",
    "\x82\xC7\x82\xEA\x82\xC9\x83`\x83\x83\x83\x8C\x83\x93\x83W\x82\xB7\x82\xE9",
};
static char *txtNumberLeft[6] = {
    "Number left",
    "\xDC\x62rig",
    "Restantes",
    "N\xFAmero izq.",
    "N. rimaste:",
    "\x82\xCC\x82\xB1\x82\xE8",
};
static char *txtEmpty2[6] = {
    "",
    "",
    "",
    "",
    "",
    "\x82\xA0\x82\xC6",
};
static char *txtPattern[6] = {
    "Pattern",
    "Muster",
    "Dispo.",
    "Patr\xF3n",
    "schema",
    "\x83p\x83^\x81[\x83\x93",
};
static char *txtPatterns[6] = {
    "Patterns",
    "Muster",
    "Dispositions",
    "Patr\xF3n",
    "Schema",
    "\x83p\x83^\x81[\x83\x93",
};
static char *txtPattern2[6] = {
    "Pattern",
    "Muster",
    "Dispo.",
    "Patr\xF3n",
    "Schema",
    "\x83p\x83^\x81[\x83\x93",
};
static char *txtNumberOfBallsLeft[6] = {
    "Number of balls left",
    "Zahl \xFC\x62riger Kugeln",
    "Boules restantes",
    "N\xB0 bolas restantes",
    "N. palle rimaste",
    "\x82\xCC\x82\xB1\x82\xE8\x83{\x81[\x83\x8B",
};
static char *txtNumberOfPatternsCleared[6] = {
    "Number of patterns cleared",
    "Abger\xE4umte Muster",
    "Nombre de dispo. r\xE9ussies",
    "N\xB0 de patrones superados",
    "N. schemi superati",
    "\x83N\x83\x8A\x83\x41\x82\xB5\x82\xBD\x83p\x83^\x81[\x83\x93\x82\xB7\x82\xA4",
};
static char *txtFallOut[6] = {
    "FALL OUT",
    "STURZ",
    "CHUTE",
    "CA\xCD\x44\x41",
    "CADUTA",
    "FALL OUT",
};
static char *txtClear[6] = {
    "CLEAR[",
    "GESCHAFFT[",
    "R\xC9USSI [",
    "\xA1SPERADO[",
    "SUPERATO[",
    "CLEAR[",
};
static char *txtTryAgain[6] = {
    "TRY AGAIN",
    "NOCH EINMAL",
    "ESSAYE ENCORE",
    "VUELVE A INTENTARLO",
    "RIPROVA",
    "TRY AGAIN",
};
static char *txtStrikes[13][6] = {
    {
        "\\",
        "\\",
        "\\",
        "\\",
        "\\",
        "\\",
    },
    {
        "STRIKE[[",
        "STRIKE[",
        "STRIKE [",
        "\xA1\xA1PLENO[[",
        "STRIKE[[",
        "STRIKE[[",
    },
    {
        "DOUBLE[[",
        "DOPPEL[",
        "DOUBLE [",
        "\xA1\xA1\x44OBLE[[",
        "DOPPIO[[",
        "DOUBLE[[",
    },
    {
        "TURKEY[[",
        "TURKEY[",
        "TURKEY [",
        "\xA1\xA1\x33\xB0 PLENO[[",
        "TRIPLO[[",
        "TURKEY[[",
    },
    {
        "4TH STRIKE[[",
        "4. STRIKE[",
        "4E STRIKE",
        "\xA1\xA1\x34\xB0 PLENO[[",
        "4\xC5 STRIKE[[",
        "4TH STRIKE[[",
    },
    {
        "5TH STRIKE[[",
        "5. STRIKE[",
        "5E STRIKE",
        "\xA1\xA1\x35\xB0 PLENO[[",
        "5\xB0 STRIKE[[",
        "5TH STRIKE[[",
    },
    {
        "6TH STRIKE[[",
        "6. STRIKE[",
        "6E STRIKE",
        "\xA1\xA1\x36\xB0 PLENO[[",
        "6\xB0 STRIKE[[",
        "6TH STRIKE[[",
    },
    {
        "7TH STRIKE[[",
        "7. STRIKE[",
        "7E STRIKE",
        "\xA1\xA1\x37\xB0 PLENO[[",
        "7\xB0 STRIKE[[",
        "7TH STRIKE[[",
    },
    {
        "8TH STRIKE[[",
        "8. STRIKE[",
        "8E STRIKE",
        "\xA1\xA1\x38\xB0 PLENO[[",
        "8\xB0 STRIKE[[",
        "8TH STRIKE[[",
    },
    {
        "9TH STRIKE[[",
        "9. STRIKE[",
        "9E STRIKE",
        "\xA1\xA1\x39\xB0 PLENO[[",
        "9\xB0 STRIKE[[",
        "9TH STRIKE[[",
    },
    {
        "10TH STRIKE[[",
        "10. STRIKE[",
        "10E STRIKE",
        "\xA1\xA1\x31\x30\xB0 PLENO[[",
        "10\xB0 STRIKE[[",
        "10TH STRIKE[[",
    },
    {
        "11TH STRIKE[[",
        "11. STRIKE[",
        "11E STRIKE",
        "\xA1\xA1\x31\x31\xB0 PLENO[[",
        "11\xB0 STRIKE[[",
        "11TH STRIKE[[",
    },
    {
        "PERFECT[[",
        "PERFEKT[",
        "PARFAIT [[",
        "\xA1\xA1PERFECTO[[",
        "PERFETTO[[",
        "PERFECT[[",
    },
};
static char *txtPunchOut[6] = {
    "PUNCH OUT[[",
    "FERTIG[",
    "PUNCH OUT [",
    "\xA1\xA1PUNCH OUT[[",
    "PUNCH OUT[",
    "PUNCH OUT[[",
};
static char *txtSpare[6] = {
    "SPARE[",
    "SPARE[",
    "SPARE [",
    "\xA1SEMI]PLENO[",
    "SPARE[",
    "SPARE[",
};
static char *txtMiss[6] = {
    "MISS",
    "DANEBEN",
    "MANQU\xC9",
    "FALLO",
    "MANCATA",
    "MISS",
};
static char *txtReplay[6] = {
    "REPLAY",
    "WIEDERH.",
    "REPLAY",
    "REPETIR",
    "REPLAY",
    "REPLAY",
};
static char *txtNowYouCanPlay[6] = {
    "Now you can play level ",
    "****",
    "****",
    "****",
    "****",
    "\x83\x8C\x83x\x83\x8B",
};
static char *txtEmpty4[6] = {
    "!",
    "****",
    "****",
    "****",
    "****",
    "\x82\xAA \x82\xA6\x82\xE7\x82\xD7\x82\xE9\x82\xE6\x82\xA4\x82\xC9\x82\xC8\x82\xE8\x82\xDC\x82\xB5\x82\xBD!",
};
static char *txtMonkeyBowling2[6] = {
    "MONKEY BOWLING 2",
    "****",
    "****",
    "****",
    "****",
    "MONKEY BOWLING 2",
};
static char *txtNormalMode[6] = {
    "NORMAL MODE",
    "****",
    "****",
    "****",
    "****",
    "NORMAL MODE",
};
static char *txtChallengeMode[6] = {
    "CHALLENGE MODE",
    "HERAUSF.-MODUS",
    "MODE CHALLENGE",
    "MODO DESAF\xCDO",
    "MOD. SFIDA",
    "CHALLENGE MODE",
};
static char *txtSpecialMode[6] = {
    "SPECIAL MODE",
    "****",
    "****",
    "****",
    "****",
    "SPECIAL MODE",
};
static char *txtNormal[6] = {
    "Normal",
    "****",
    "****",
    "****",
    "****",
    "Normal",
};
static char *txt9Pins[6] = {
    "9 pins",
    "****",
    "****",
    "****",
    "****",
    "9 pins",
};
static char *txtStrike2[6] = {
    "Strike",
    "****",
    "****",
    "****",
    "****",
    "Strike",
};
static char *txtLevels[3][6] = {
    {
    "LEVEL 1",
    "****",
    "****",
    "****",
    "****",
    "LEVEL 1",
},
    {
    "LEVEL 2",
    "****",
    "****",
    "****",
    "****",
    "LEVEL 2",
},
    {
    "LEVEL 3",
    "****",
    "****",
    "****",
    "****",
    "LEVEL 3",
},
};

struct BallParams { float a, b, c, d; s16 u10, u12, u14, u16; };

const struct BallParams ballParams[4] = {
    { 0.5f, 0.25f, 0.050000001f, 2.5f, 0xE000, 0xE800, 0xED36, 0xF000 },
    { 0.44f, 0.23f, 0.050000001f, 2.5f, 0xE000, 0xD800, 0xEAC9, 0xF000 },
    { 0.40000001f, 0.22f, 0.025f, 2.5f, 0xFC00, 0x0000, 0xFDA7, 0xF800 },
    { 0.69999999f, 0.30000001f, 0.050000001f, 10.0f, 0xE000, 0xF800, 0xF6A3, 0xF200 },
};

struct PinSplit { u16 mask; char **names; };

const struct PinSplit pinSplits[10] = {
    { 0x3FF, txtStandard },
    { 0x134, txtBucket },
    { 0x204, txtBabySplit },
    { 0x04B, txtClothesline },
    { 0x300, txtFitSplit },
    { 0x20B, txtWashOut },
    { 0x242, txtChristmasTree },
    { 0x0C2, txtBabySplitWithCompany },
    { 0x1C0, txtDoubleFit },
    { 0x248, txtBigThree },
};

const Vec pinPositions[10] = {
    { 0.0f, 0.41350001f, -99.752289f },
    { -0.69999999f, 0.41350001f, -100.96229f },
    { 0.69999999f, 0.41350001f, -100.96229f },
    { -1.4f, 0.41350001f, -102.17229f },
    { 0.0f, 0.41350001f, -102.17229f },
    { 1.4f, 0.41350001f, -102.17229f },
    { -2.0999999f, 0.41350001f, -103.39229f },
    { -0.69999999f, 0.41350001f, -103.39229f },
    { 0.69999999f, 0.41350001f, -103.39229f },
    { 2.0999999f, 0.41350001f, -103.39229f },
};



static s32 bss_0;
static s16 bss_4;
static s8 bss_6;
static s8 bss_7;
static s8 bss_8;
static s16 bss_A;
static s32 bss_C;
static s8 bss_10;
static struct BowlPlayer players[4];
static struct BowlLane lanes[4];
static u16 bss_1A4;
static s8 bss_1A6;
static struct Bss1A8 bss_1A8;
static s8 bss_1B4[4];
static f32 bss_1B8;
static f32 bss_1BC;
static f32 bss_1C0;
static s16 bss_1C4;
static f32 bss_1C8;
static f32 bss_1CC;
static struct { int a; int b; s16 timer; s16 trigger; u8 pad[4]; } bss_1D0;
/* Fix data and bss layout; stripped by the linker. */
void dev_pin_scal(void) { data_488=0; data_48C=0; data_490=0; data_491=0; }
void dev_pin_bss(void) { void *p; p=&bss_0; p=&bss_4; p=&bss_6; p=&bss_7; p=&bss_8; p=&bss_A; p=&bss_C; p=&bss_10; p=players; p=lanes; p=&bss_1A4; p=&bss_1A6; p=&bss_1A8; }
s16 lbl_5_bss_1AE = 0;
s16 lbl_5_bss_1B0 = 0;
void dev_pin_bss2(void) { void *p; p=bss_1B4; p=&bss_1B8; p=&bss_1BC; p=&bss_1C0; p=&bss_1C4; p=&bss_1C8; p=&bss_1CC; p=&bss_1D0; }

void _prolog(void)
{
    g_set_minigame_specific_funcs(fn_5_7DC, fn_5_D34, fn_5_C734, fn_5_D710);
    g_init_bowling();
    sub_mode_request = 0xAC;
    g_very_similar_to_puts("RELOCATION BOWLING LOADED\n");
}

void _epilog(void)
{
    fn_5_2FF0();
    g_very_similar_to_puts("RELOCATION BOWLING PURGED\n");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    for (sp = OSGetStackPointer(); sp != NULL && (uptr)sp != 0xFFFFFFFF && i++ < 16; sp = (u32 *)(uptr)*sp)
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
    OSPanic("mini_bowling.c", 0x44E, "\n");
}

void g_init_bowling(void)
{
    int i;
    int j;
    struct Sprite *sprite;

    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    SoundGroupLoad(0xA);
    fn_1_358E8();
    g_SoftStreamStart_with_some_defaults(0x15);
    g_SoftStreamStart_with_some_defaults(0x16);
    g_SoftStreamStart_with_some_defaults(0x17);
    g_SoftStreamStart_with_some_defaults(0x18);
    g_SoftStreamStart_with_some_defaults(0x19);
    g_SoftStreamStart_with_some_defaults(0x1A);
    fn_1_56C18();
    load_stage(0x9F);
    g_load_minigame_gma_tpl(0xE);
    load_bmp_by_id(6);
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(3);
    event_init(9);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x14);
    event_init(0x15);
    event_init(0xB);
    light_init(0x9F);
    g_init_rendefc_for_stage();
    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] == 2)
        {
            g_create_stage_player_wrapper(i, 0, active_monkey_id[i], i, 0, (void (*)(struct Ape *))g_bowl_drawFunc);
            balls[i].ape->cameraVisibleMask = ~((1 << i) | (1 << i));
        }
    }
    g_some_frame_counter.flags |= 4;
    g_some_frame_counter.frameCounter = 0;
    itemgroups[1].animFrame = 0;
    itemgroups[2].animFrame = 0;
    fn_1_2810();
    for (i = 0; i < 5; i++)
    {
        if (i == modeCtrl.currPlayer)
            enable_camera(i);
        else
            disable_camera(i);
    }
    {
        struct Ball *ball;
        struct Ball *savedBall = current_ball;
        int k;

        ball = balls;
        for (k = 0; k < 8; k++, ball++)
        {
            if (ball->status == 2)
            {
                current_ball = ball;
                current_ball->state = 1;
            }
        }
        current_ball = savedBall;
    }
    {
        struct World *world = world_infos;
        s8 *status = g_poolInfo.playerPool.statusList;
        int k;

        for (k = 0; k < g_poolInfo.playerPool.count; k++, world++, status++)
        {
            if (*status == 2)
                world->state = 1;
        }
    }
    camera_set_state(0x3D);
    fn_5_C0D0(0);
    for (i = 0; i < 4; i++)
        set_camera_view_settings(i, 0.0f, 0.0f, 1.0f, 1.0f);
    g_reset_cm_course();
    for (i = 0; i < 4; i++)
    {
        players[i].u0 = 0;
        players[i].u4 = 0;
        players[i].u6 = 1;
        players[i].u7 = 1;
        players[i].u8 = 0;
        players[i].u9 = 0;
        players[i].frameScore[0] = 0;
        for (j = 1; j < 11; j++)
            players[i].frameScore[j] = -1;
        for (j = 0; j < 21; j++)
        {
            players[i].u20[j] = 0;
            players[i].u35[j] = 0;
        }
    }
    for (i = 0; i < 4; i++)
    {
        lanes[i].u0 = 0;
        lanes[i].u2 = 1;
        lanes[i].u3 = 0;
        for (j = 0; j < 20; j++)
            lanes[i].u4[j] = 0;
    }
    bss_1A4 = 0;
    bss_1A6 = 0;
    data_490 = 1;
    bss_6 = 0;
    bss_1A8.u0 = 0;
    bss_1A8.u2 = 0;
    bss_1A8.u3 = 0;
    bss_1A8.u4 = 0xF;
    bss_C = 0;
    fade_screen_to_color(0x100, 0, 0x1E);
    data_494 = -1;
    g_fade_track_volume(0x2B, 0);
    fn_5_12234();
    fn_5_15F2C();
    switch (lbl_1_bss_1700[0])
    {
    case 2:
        fn_5_169F0(lbl_1_bss_1700[1]);
    case 0:
    default:
        data_491 = 3;
        data_488 = 1;
        data_48C = 0x21;
        bss_0 = 0x2710;
        break;
    case 1:
        data_488 = 0x1000;
        data_48C = 0x20;
        bss_0 = 0x2710;
        break;
    case 4:
        data_491 = 4;
        data_488 = 0x8000;
        data_48C = 0x21;
        bss_0 = 0x2710;
        break;
    }
    for (i = 0; i < 3; i++)
    {
        fn_5_C850(i, balls);
        fn_5_12BAC(i);
        fn_5_12194(i);
        fn_5_165B4(i);
    }
    bss_7 = 0;
    bss_8 = 0;
    lbl_5_bss_1AE = -0x400;
    lbl_5_bss_1B0 = 0;
    sprite = create_sprite();
    if (sprite != NULL)
        sprite->drawFunc = fn_5_1B7C;
    fn_5_10B98();
    for (i = 0; i < 4; i++)
        bss_1B4[i] = -1;
    bss_10 = 0;
    g_load_missing_apes();
}

void fn_5_7DC(void)
{
    s8 a;
    s8 b;

    bss_4 = (u16)g_font_char_var_3;
    fn_5_BEB4();
    if ((g_some_other_flags & 0xA) != 0)
        return;
    bss_0--;
    lbl_5_bss_1BB94++;
    if ((dip_switches & 0x20000000) && (merged_digital_inputs[2] & 0x10))
        fn_5_127C8();
    switch (data_488)
    {
    case 0x1:
        fn_5_30A0();
        break;
    case 0x2:
        fn_5_3380();
        break;
    case 0x4:
        fn_5_34CC();
        break;
    case 0x8:
        fn_5_3798();
        break;
    case 0x10:
        fn_5_39E8();
        break;
    case 0x20:
        fn_5_3CA0();
        break;
    case 0x40:
        fn_5_4184();
        break;
    case 0x80:
        fn_5_490C();
        break;
    case 0x100:
        fn_5_533C();
        break;
    case 0x200:
        fn_5_592C();
        break;
    case 0x400:
        fn_5_5CDC();
        break;
    case 0x800:
        fn_5_5D94();
        break;
    case 0x1000:
        fn_5_6250();
        break;
    case 0x2000:
        fn_5_65A4();
        break;
    case 0x4000:
        fn_5_6C00();
        break;
    case 0x8000:
        fn_5_70C4();
        break;
    case 0x10000:
        fn_5_751C();
        break;
    default:
        data_488 = 1;
        break;
    }
    if (modeCtrl.courseFlags & 0x4000)
    {
        modeCtrl.courseFlags &= ~0x4000;
        g_init_bowling();
        return;
    }
    a = -0.9795918367346939 * current_ball->pos.z;
    b = -0.6530612244897959 * current_ball->pos.z;
    a = a < 0 ? 0 : (a > 127 ? 127 : a);
    b = b < 0 ? 0 : (b > 127 ? 127 : b);
    fn_1_32338(a, b);
    if (bss_C > 0)
        bss_C--;
    if (data_488 != 0x200)
    {
        switch (lbl_1_bss_1700[0])
        {
        case 0:
        default:
            if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x400) && (data_48C & 1) && !(data_488 & 0x100))
            {
                data_490 = (data_490 == 0);
                call_SoundReqID_arg_0(0x98);
            }
            if (data_490 != 0 && data_491 > 3)
                data_491--;
            else if (data_490 == 0 && data_491 < 10)
                data_491++;
            break;
        case 4:
            if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x400) && (data_48C & 1) && !(data_488 & 0x100))
            {
                data_490 = (data_490 == 0);
                call_SoundReqID_arg_0(0x98);
            }
            if (data_490 != 0 && data_491 > 4)
                data_491--;
            else if (data_490 == 0 && data_491 < 20)
                data_491++;
            break;
        case 1:
            break;
        }
        fn_5_12264(current_ball);
    }
}

void fn_5_D34(void)
{
    s16 angle;
    Mtx mtx;
    Vec dir;
    Vec pos;
    float frac;
    float y;

    if (data_48C & 0x2000)
    {
        fn_1_120AD8();
        return;
    }
    mtxa_from_mtxb();
    g_set_active_light_group(0);
    if (bss_C == 0)
        angle = 0;
    else if (bss_C < 60)
        angle = 243.2f * bss_C;
    else if (bss_C > 348.0)
        angle = 1216.0 * (360.0 - bss_C);
    else
        angle = 0x3900;
    mtxa_from_mtxb();
    mtxa_translate_xyz(0.0f, 3.5f, -100.896f);
    mtxa_rotate_y(angle);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(MODEL(0));
    if (data_48C & 0x1000)
    {
        if (g_some_render_flag & 4)
            fn_5_C0F4();
        else
        {
            s8 status;

            status = g_poolInfo.playerPool.statusList[0];
            fn_5_16358();
            if (cameras[0].sub28.viewportW > 0.0f && cameras[0].sub28.viewportH > 0.0f && status != 0 && status != 4)
            {
                camera_apply_viewport_if_changed(0);
                fn_1_D4870();
                g_maybe_some_bg_effect_handler();
                u_reset_light_group_stack(0);
                g_something_with_drawing_apes();
                status = events[0].state;
                if (status == 2 || status == 4)
                    fn_5_16DC();
                if (events[20].state == 2)
                {
                    g_smth_with_rendefc_reflective_height(400.0f);
                    g_draw_bg();
                    g_smth_with_rendefc_reflective_height(0.0f);
                }
                if (events[21].state == 2)
                    g_something_with_rend_efc(0x10);
                handle_test_camera();
                ord_tbl_draw_nodes();
                if (events[21].state == 2)
                    g_something_with_rend_efc(8);
                fn_5_10F40();
            }
            camera_setup_default_projection();
        }
        return;
    }
    if (!(cameras[modeCtrl.currPlayer].sub28.viewportW > 0.0f) || !(cameras[modeCtrl.currPlayer].sub28.viewportH > 0.0f))
        return;
    if (g_poolInfo.playerPool.statusList[modeCtrl.currPlayer] != 0 && g_poolInfo.playerPool.statusList[modeCtrl.currPlayer] != 4)
    {
        camera_apply_viewport_if_changed(modeCtrl.currPlayer);
        if (data_48C & 0x4000)
        {
            mtxa_from_quat(&current_ball->ape->rot);
            mtxa_to_mtx(mtx);
            mtxa_from_mtxb();
            mtxa_translate(&current_ball->ape->pos);
            mtxa_mult_right(mtx);
            if (current_ball->ape->charaId == 3)
            {
                mtxa_translate_xyz(0.0f, -0.1f, 0.0f);
                mtxa_mult_right(fn_1_1135A4(current_ball->ape));
                mtxa_translate_xyz(0.079999998f, 0.0099999998f, -0.079999998f);
            }
            else
            {
                mtxa_translate_xyz(0.0f, -0.12f, 0.0f);
                mtxa_mult_right(fn_1_1135A4(current_ball->ape));
                mtxa_translate_xyz(0.039999999f, 0.0099999998f, -0.079999998f);
            }
            mtxa_scale_s(0.1f);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(MODEL(1));
        }
        if (data_48C & 4)
        {
            int j;

            frac = (bss_4 & 0xF) / 16.0f;
            dir.x = -math_sin(lbl_5_bss_1B0 + lbl_5_bss_1AE * bss_1B8);
            dir.y = 0.0f;
            dir.z = -math_sin(16384.0f + (lbl_5_bss_1B0 + lbl_5_bss_1AE * bss_1B8));
            PSVECScale(&dir, &dir, 3.3f);
            pos.x = current_ball->pos.x;
            pos.y = 0.0f;
            pos.z = -15.86f;
            mtxa_from_mtxb();
            mtxa_translate(&pos);
            mtxa_scale_s(0.30000001f);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(MODEL(3));
            for (j = 0; j < 20; j++)
            {
                if (j == 0)
                {
                    pos.x += dir.x * frac;
                    pos.z += dir.z * frac;
                }
                else
                {
                    pos.x += dir.x;
                    pos.z += dir.z;
                }
                pos.y = 2.0f;
                y = 0.15 + fn_5_16B64(&pos);
                if (y > 0.0f)
                    pos.y = y;
                else
                    pos.y = 0.0f;
                mtxa_from_mtxb();
                mtxa_translate(&pos);
                mtxa_scale_s(0.30000001f);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_set_alpha(1 - 0.05 * j);
                avdisp_draw_model_culled_sort_auto(MODEL(4));
            }
        }
        fn_5_12448();
        if (g_some_render_flag & 4)
        {
            fn_5_C0F4();
            return;
        }
        fn_5_16358();
        {
        int i;
        struct Ball *savedBall;

        savedBall = current_ball;
        for (i = 0; i < 5; i++)
        {
            if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f)
            {
                s8 status2;
                s8 state;

                status2 = g_poolInfo.playerPool.statusList[i];
                if (status2 != 0 && status2 != 4)
                {
                    current_ball = &balls[i];
                    camera_apply_viewport_if_changed(i);
                    fn_1_D4870();
                    g_maybe_some_bg_effect_handler();
                    u_reset_light_group_stack(i);
                    if (events[21].state == 2)
                        g_something_with_rend_efc(4);
                    state = events[0].state;
                    if (state == 2 || state == 4)
                        fn_5_16DC();
                    if (events[20].state == 2)
                    {
                        g_smth_with_rendefc_reflective_height(400.0f);
                        g_draw_bg();
                        g_smth_with_rendefc_reflective_height(0.0f);
                    }
                    if (events[21].state == 2)
                        g_something_with_rend_efc(0x10);
                    if (events[14].state == 2)
                        g_draw_effects();
                    if (events[2].state == 2)
                        g_draw_ball_and_ape();
                    handle_test_camera();
                    ord_tbl_draw_nodes();
                    if (data_48C & 0x80000)
                        fn_5_11ADC();
                    if (events[21].state == 2)
                        g_something_with_rend_efc(8);
                }
            }
        }
        current_ball = savedBall;
        }
        camera_setup_default_projection();
    }
}

void fn_5_16DC(void)
{
    struct StageCollHeader *coll;
    int shadow;
    void *drawFunc;
    struct ItemGroup *ig;
    struct StageModelRef * DPTR *refs;
    int j;
    int i;
    int loaded;
    struct GMAModel *model;
    struct ShadowReceive s;
    Mtx mtx;

    if (!(g_some_render_flag & 4) && !(dip_switches & DIP_NO_STAGE) && (monkey_flags & 1))
    {
        perf_init_timer(8);
        drawFunc = ((struct WorldTheme *)&world_theme)->drawFunc;
        if (drawFunc != NULL)
        {
            if (!(g_some_render_flag & 0x80))
                g_yet_another_unk_draw_func(drawFunc);
            else
                drawFunc = NULL;
        }
        s.unk6 = 6;
        if (dip_switches & DIP_TRIANGLE)
        {
            mtxa_from_mtxb();
            mtxa_rotate_x(-0x4000);
            mtxa_scale_xyz(10.0f, 10.0f, 10.0f);
            avdisp_set_scale_factor(10.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[89].model);
        }
        else
        {
            s.unk6 = 6;
            shadow = g_check_some_condition(6);
            ig = itemgroups;
            coll = stagedef->collHeaders;
            for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
            {
                if (!(g_some_frame_counter.flags & 0x10) || !(coll->unkA7 & (1 << (g_some_frame_counter.unkD - 1))))
                {
                    mtxa_from_mtxb();
                    if (i > 0)
                        mtxa_mult_right(ig->transform);
                    loaded = 0;
                    refs = coll->modelRefs;
                    for (j = 0; j < coll->modelRefCount; j++, refs++)
                    {
                        if (!((*refs)->flags & 8))
                        {
                            model = g_some_draw_func2(*refs);
                            if (model != NULL && g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
                            {
                                if (coll->textureScroll != NULL)
                                    g_something_with_texture_scroll(coll->textureScroll);
                                if (!loaded)
                                {
                                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                                    loaded = 1;
                                }
                                avdisp_draw_model_unculled_sort_never(model);
                                if (coll->textureScroll != NULL)
                                    g_something_with_texture_scroll_2(0);
                                if (shadow && i == 0 && j == 2)
                                {
                                    s.obj = refs;
                                    if ((*refs)->flags & 2)
                                        s.unk4 = 4;
                                    else
                                        s.unk4 = 2;
                                    s.model = model;
                                    g_some_shadow_draw_func(&s);
                                }
                            }
                        }
                    }
                }
            }
        }
        if (drawFunc != NULL)
            g_yet_another_unk_draw_func(NULL);
        if (dip_switches & DIP_STCOLI)
        {
            struct ItemGroup *ig;
            struct StageCollHeader *coll;
            int i;
            struct FalloutVolume *vol;
            int j;

            mtx_copy(mtxa->mtxB, mtx);
            ig = itemgroups;
            coll = stagedef->collHeaders;
            for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
            {
                mtxa_from_mtx(mtx);
                mtxa_mult_right(ig->transform);
                mtxa_to_mtx(mtxa->mtxB);
                vol = coll->falloutVolumes;
                for (j = 0; j < coll->falloutVolumeCount; j++, vol++)
                {
                    float scale;

                    mtxa_from_mtxb();
                    mtxa_translate(&vol->pos);
                    mtxa_rotate_z(vol->rotZ);
                    mtxa_rotate_y(vol->rotY);
                    mtxa_rotate_x(vol->rotX);
                    mtxa_scale(&vol->scale);
                    scale = MAX(vol->scale.x, vol->scale.y);
                    scale = MAX(scale, vol->scale.z);
                    avdisp_set_scale_factor(scale);
                    avdisp_set_alpha(0.5f);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[39].model);
                }
            }
            mtx_copy(mtx, mtxa->mtxB);
        }
        perfInfo[0] += perf_stop_timer(8);
    }
}

void fn_5_1B7C(struct Sprite *sprite)
{
    fn_5_1B9C();
}

static inline float maxf(float a, float b)
{
    if (a > b)
        return a;
    return b;
}

void fn_5_1B9C(void)
{
    int i;

    if (data_48C & 0x3000)
        return;
    if (data_48C & 0x8000)
    {
        set_ui_widescreen_scale_mtx(0x140);
        fn_5_B3C8();
    }
    switch (lbl_1_bss_1700[0])
    {
    case 1:
        if (!(data_488 & 0x6100))
        {
            struct SpriteDrawRequest ballReq = sprD[current_ball->colorId];
            struct SpriteDrawRequest iconReq = spr11[current_ball->ape->charaId];
            char *text = txtNumberLeft[locale];
            float w;
            u8 fade;
            char buf[4];

            set_ui_widescreen_scale_mtx(0x60);
            textdraw_reset();
            textdraw_set_font(3);
            textdraw_set_flags(0x80000000);
            fn_5_B30C(16.0f, 16.0f, 0xFFFF00, text);
            w = 32.0f + g_get_string_sprite_width_2(text);
            ballReq.scaleX *= 2.0 / 3.0;
            ballReq.scaleY *= 2.0 / 3.0;
            iconReq.scaleX *= 2.0 / 3.0;
            iconReq.scaleY *= 2.0 / 3.0;
            ballReq.x = iconReq.x = w + bss_1A8.u4 * 16;
            ballReq.y = iconReq.y = 28.0f;
            ballReq.z += 0.01 * bss_1A8.u4;
            iconReq.z = ballReq.z - 0.005;
            fade = bss_1A8.u4 * 12;
            ballReq.mulColor -= (fade << 16) + (fade << 8) + fade;
            iconReq.mulColor = ballReq.mulColor;
            for (i = 0; i < bss_1A8.u4; i++)
            {
                ballReq.x -= 16.0f;
                ballReq.z -= 0.01;
                ballReq.mulColor += 0xC0C0C;
                draw_sprite_draw_request(&ballReq);
                iconReq.x -= 16.0f;
                iconReq.z -= 0.01;
                iconReq.mulColor += 0xC0C0C;
                draw_sprite_draw_request(&iconReq);
            }
            sprintf(buf, "%d", bss_1A8.u4);
            fn_5_B30C(w + bss_1A8.u4 * 16, 16.0f, 0xFFFF00, buf);
        }
        break;
    case 4:
        if (data_48C & 1)
        {
            if (data_48C & 0x800)
            {
                set_ui_widescreen_scale_mtx(0x140);
                fn_5_9C90(41.0f, 238 - modeCtrl.playerCount * 15);
            }
            else
            {
                set_ui_widescreen_scale_mtx(0x60);
                fn_5_9C90(16.0f, 16.0f);
            }
        }
        break;
    default:
        if (data_48C & 1)
        {
            if (data_48C & 0x800)
            {
                set_ui_widescreen_scale_mtx(0x140);
                fn_5_8F38(72.0f, 238 - modeCtrl.playerCount * 15);
            }
            else
            {
                set_ui_widescreen_scale_mtx(0x60);
                fn_5_8F38(16.0f, 16.0f);
            }
        }
        break;
    }
    if ((data_48C & 0x20) && bss_1A6 > 0)
    {
        set_ui_widescreen_scale_mtx(0x220);
        draw_sprite_draw_request((struct SpriteDrawRequest *)&spr1F);
        for (i = 0; i < sizeof(spr20) / sizeof(spr20[0]); i++)
        {
            if (bss_1A4 & (1 << i))
                draw_sprite_draw_request((struct SpriteDrawRequest *)&spr20[i]);
        }
    }
    if (data_48C & 8)
    {
        set_ui_widescreen_scale_mtx(0x60);
        fn_5_88CC(bss_1BC);
    }
    if (data_48C & 0x10)
    {
        struct SpriteDrawRequest req;

        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request((struct SpriteDrawRequest *)&spr2);
        req = spr3;
        req.u2 = bss_1C0;
        req.scaleX = bss_1C0;
        draw_sprite_draw_request(&req);
        for (i = 0; i < 22; i++)
        {
            if (bss_1C0 < 0.045454545454545456 * i)
                break;
            req = spr4;
            if (bss_1C0 < 0.045454545454545456 * (i + 1))
            {
                req.scaleX = 22.0 * bss_1C0 - i;
                req.scaleY = 22.0 * bss_1C0 - i;
            }
            req.x += i * 14;
            draw_sprite_draw_request(&req);
        }
    }
    if (data_48C & 0x390)
    {
        int pulse = 255.0 * (0.65 + 0.35 * math_sin((s16)(1092.2666666666667 * bss_4)));
        char *texts[7];
        float widths[7];
        float maxWidth;
        int i;

        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        textdraw_set_scale(0.8f, 0.8f);
        texts[0] = txtMove[locale];
        texts[1] = txtSelect[locale];
        texts[2] = txtPosition[locale];
        texts[3] = txtSelect2[locale];
        texts[4] = txtAngle[locale];
        texts[5] = txtSelect3[locale];
        texts[6] = txtPower[locale];
        maxWidth = 0.0f;
        for (i = 0; i < 7; i++)
        {
            widths[i] = g_get_string_sprite_width_2(texts[i]);
            if (maxWidth < widths[i])
                maxWidth = widths[i];
        }
        maxWidth *= 0.8f;
        if (data_48C & 0x80)
        {
            struct SpriteDrawRequest req;
            char *a = txtChange[locale];
            char *b = txtView[locale];
            float wa = g_get_string_sprite_width_2(a);
            float w = 0.8f * maxf(wa, g_get_string_sprite_width_2(b));
            float arrowOfs = (bss_4 & 0x1F) >> 1;
            float arrow = math_sin((s16)(1024.0f * bss_4));
            float x;

            req = spr60;
            set_ui_widescreen_scale_mtx(0x60);
            req.x = 21.0f;
            req.y = 256.0f;
            req.scaleX = 6 + 0.125 * w;
            if (b == "")
                req.scaleY = 5.0f;
            else
                req.scaleY = 8.0f;
            draw_sprite_draw_request(&req);
            pulse = ((u8)pulse << 16) + 0xFFFF;
            fn_5_B30C(29.0f, 264.0f, 0xFFFFFF, "\x83\xA6");
            fn_5_B30C(61.0f, 264.0f, pulse, a);
            fn_5_B30C(61.0f, 288.0f, pulse, b);
            set_ui_widescreen_scale_mtx(0x220);
            x = 611.0f - maxWidth;
            req.x = x - 40.0f;
            req.y = 192.0f;
            req.scaleX = 6 + 0.125 * maxWidth;
            req.scaleY = 16.0f;
            draw_sprite_draw_request(&req);
            fn_5_B30C(x - 32.0f, 200.0f, 0xFFFFFF, "\x83\xAD");
            fn_5_B30C(x, 200.0f, pulse, texts[0]);
            fn_5_B30C(x - 32.0f, 264.0f, 0xFFFFFF, "\x83\x9F");
            fn_5_B30C(x, 264.0f, pulse, texts[1]);
            fn_5_B30C(x, 288.0f, pulse, texts[2]);
            if (arrow > 0.0)
            {
                textdraw_set_alpha(arrow);
                textdraw_set_flags(0x100000);
                x += 0.5f * maxWidth - 32.0f;
                fn_5_B30C(x, 224.0f + arrowOfs, pulse, "\x81\xA3");
                textdraw_clear_flags(0x100000);
            }
        }
        else if (data_48C & 0x100)
        {
            struct SpriteDrawRequest req;
            char *b;
            char *a;
            float wa;
            float w;
            float x;

            a = txtCancel[locale];
            b = txtEmpty[locale];
            wa = g_get_string_sprite_width_2(a);
            w = 0.8f * maxf(wa, g_get_string_sprite_width_2(b));
            x = 611.0f - maxWidth;
            req = spr60;
            set_ui_widescreen_scale_mtx(0x60);
            req.x = 21.0f;
            req.y = 280.0f;
            req.scaleX = 6 + 0.125 * w;
            if (b == "")
                req.scaleY = 5.0f;
            else
                req.scaleY = 8.0f;
            draw_sprite_draw_request(&req);
            pulse = (u8)pulse + 0xFFFF00;
            fn_5_B30C(29.0f, 288.0f, 0xFFFFFF, "\x83\xA0");
            fn_5_B30C(61.0f, 288.0f, pulse, a);
            fn_5_B30C(61.0f, 312.0f, pulse, b);
            set_ui_widescreen_scale_mtx(0x220);
            req.x = x - 40.0f;
            req.y = 280.0f;
            req.scaleX = 6 + 0.125 * maxWidth;
            req.scaleY = 8.0f;
            draw_sprite_draw_request(&req);
            fn_5_B30C(x - 32.0f, 288.0f, 0xFFFFFF, "\x83\x9F");
            fn_5_B30C(x, 288.0f, pulse, texts[3]);
            fn_5_B30C(x, 312.0f, pulse, texts[4]);
        }
        else if (data_48C & 0x400)
        {
            struct SpriteDrawRequest req;
            float x = 611.0f - maxWidth;

            req = spr60;
            set_ui_widescreen_scale_mtx(0x220);
            req.x = x - 40.0f;
            req.y = 304.0f;
            req.scaleX = 6 + 0.125 * maxWidth;
            req.scaleY = 8.0f;
            draw_sprite_draw_request(&req);
            pulse = ((u8)pulse << 8) + 0xFF0000 + (u8)pulse;
            fn_5_B30C(x - 32.0f, 312.0f, 0xFFFFFF, "\x83\x9F");
            fn_5_B30C(x, 312.0f, pulse, texts[5]);
            fn_5_B30C(x, 336.0f, pulse, texts[6]);
        }
        else if (data_48C & 0x200)
        {
            struct SpriteDrawRequest req;
            char *a = txtSpin[locale];

            req = spr60;
            set_ui_widescreen_scale_mtx(0x60);
            req.x = 21.0f;
            req.y = 320.0f;
            req.scaleX = 9 + 0.800000011920929 * (0.125 * g_get_string_sprite_width_2(a));
            req.scaleY = 5.0f;
            draw_sprite_draw_request(&req);
            pulse = (((u8)pulse << 16) + 0x10000 + (u8)pulse) - 0x100;
            fn_5_B30C(29.0f, 328.0f, 0xFFFFFF, "\x83\xA1\x83\xAF");
            fn_5_B30C(77.0f, 328.0f, pulse, txtSpin[locale]);
        }
    }
    if (data_48C & 0x800)
    {
        char *s;

        reset_ui_widescreen_scale_mtx();
        draw_sprite_draw_request((struct SpriteDrawRequest *)&spr0);
        draw_sprite_draw_request((struct SpriteDrawRequest *)&spr5B);
        set_ui_widescreen_scale_mtx(0x60);
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        fn_5_B30C(47.0f, 37.0f, 0xFFFFFF, txtMonkeyBowling2[locale]);
        set_ui_widescreen_scale_mtx(0x220);
        textdraw_set_scale(0.7f, 0.7f);
        switch (lbl_1_bss_1700[0])
        {
        case 0:
            s = txtNormalMode[locale];
            fn_5_B30C(611.0f - 0.7f * g_get_string_sprite_width_2(s), 44.2f, 0xFFFFFF, s);
            break;
        case 3:
            s = txtNormalMode[locale];
            fn_5_B30C(611.0f - 0.7f * g_get_string_sprite_width_2(s), 33.0f, 0xFFFFFF, s);
            s = txt9Pins[locale];
            fn_5_B30C(611.0f - 0.7f * g_get_string_sprite_width_2(s), 50.0f, 0xFFFFFF, s);
            break;
        case 4:
            s = txtNormalMode[locale];
            fn_5_B30C(611.0f - 0.7f * g_get_string_sprite_width_2(s), 33.0f, 0xFFFFFF, s);
            s = txtStrike2[locale];
            fn_5_B30C(611.0f - 0.7f * g_get_string_sprite_width_2(s), 50.0f, 0xFFFFFF, s);
            break;
        default:
        case 2:
            s = txtSpecialMode[locale];
            fn_5_B30C(611.0f - 0.7f * g_get_string_sprite_width_2(s), 33.0f, 0xFFFFFF, s);
            s = txtLevels[lbl_1_bss_1700[1]][locale];
            fn_5_B30C(611.0f - 0.7f * g_get_string_sprite_width_2(s), 50.0f, 0xFFFFFF, s);
            break;
        }
        textdraw_set_scale(1.0f, 1.0f);
        set_ui_widescreen_scale_mtx(0x140);
        if (bss_6 != 0)
        {
            char buf[0x34];

            draw_sprite_draw_request((struct SpriteDrawRequest *)&spr1);
            sprintf(buf, "%s%d%s", txtNowYouCanPlay[locale], lbl_1_bss_1700[1] + 2, txtEmpty4[locale]);
            fn_5_B30C(0.5f * (640.0f - g_get_string_sprite_width_2(buf)), 408.0f, 0xFFFF00, buf);
        }
    }
    if (data_48C & 0x30000)
    {
        char *s = txtChallengeMode[locale];
        char buf[0x20];

        reset_ui_widescreen_scale_mtx();
        draw_sprite_draw_request((struct SpriteDrawRequest *)&spr5B);
        set_ui_widescreen_scale_mtx(0x60);
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        fn_5_B30C(47.0f, 37.0f, 0xFFFFFF, txtMonkeyBowling2[locale]);
        set_ui_widescreen_scale_mtx(0x220);
        textdraw_set_scale(0.7f, 0.7f);
        fn_5_B30C(611.0f - 0.7f * g_get_string_sprite_width_2(s), 46.0f, 0xFFFFFF, s);
        textdraw_set_scale(1.0f, 1.0f);
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        sprintf(buf, "%s %d", txtNumberOfBallsLeft[locale], bss_1A8.u4 > 0 ? bss_1A8.u4 : 0);
        fn_5_B30C(595.0f - g_get_string_sprite_width_2(buf), 384.0f, 0xFFFF00, buf);
        sprintf(buf, "%s %d", txtNumberOfPatternsCleared[locale], bss_1A8.u2);
        fn_5_B30C(595.0f - g_get_string_sprite_width_2(buf), 408.0f, 0xFFFF00, buf);
    }
    if (data_48C & 0x40000)
    {
        int n = (bss_0 >> 5) & 3;

        set_ui_widescreen_scale_mtx(0x220);
        draw_sprite_draw_request((struct SpriteDrawRequest *)&spr5C);
        if (n < 3)
            draw_sprite_draw_request((struct SpriteDrawRequest *)&spr5D);
        if (n < 2)
            draw_sprite_draw_request((struct SpriteDrawRequest *)&spr5E);
        if (n < 1)
            draw_sprite_draw_request((struct SpriteDrawRequest *)&spr5F);
    }
    reset_ui_widescreen_scale_mtx();
}

void fn_5_2FF0(void)
{
    int i;

    g_set_up_debugtext_buffer();
    dest_all_events();
    g_some_render_flag &= 0xFFFFFFDF;
    fn_1_11C5E4();
    load_stage(0x9F);
    fn_5_164E4();
    free_minigame_gma_tpl();
    free_bmp_by_id(6);
    g_something_calls_sndPopGroup();
    for (i = 0xF; i >= 0; i--)
    {
        if (lbl_1_bss_70E98[i] != -1)
            fn_1_D29E0(lbl_1_bss_70E98[i]);
    }
}

void fn_5_30A0(void)
{
    if (lbl_1_bss_1700[0] == 2)
        fn_5_169B4(players[modeCtrl.currPlayer].u6);
    if (lbl_1_bss_1700[0] == 4)
    {
        g_some_frame_counter.frameCounter = 0x1770;
        itemgroups[1].animFrame = 0x1770;
        itemgroups[2].animFrame = 0x1770;
    }
    else if (players[modeCtrl.currPlayer].u7 == 1 || (players[modeCtrl.currPlayer].u7 == 2 && players[modeCtrl.currPlayer].u6 == 0xA))
    {
        g_some_frame_counter.frameCounter = 0;
        itemgroups[1].animFrame = 0;
        itemgroups[2].animFrame = 0;
    }
    else
    {
        g_some_frame_counter.frameCounter = 0x1770;
        itemgroups[1].animFrame = 0x1770;
        itemgroups[2].animFrame = 0x1770;
    }
    fn_5_12860(pinPositions[0].y);
    bss_1A4 = fn_5_126C8(&bss_1A6);
    fn_5_1266C();
    if (bss_1A4 == 0 || lbl_1_bss_1700[0] == 4)
    {
        fn_5_7A5C();
        bss_1A4 = fn_5_126C8(&bss_1A6);
    }
    bss_7 = 0;
    bss_8 = 0;
    bss_1B8 = 0.0f;
    bss_1C0 = 0.0f;
    bss_1BC = 0.0f;
    bss_1C4 = 0;
    current_ball->state = 0x19;
    current_ball->unk154 = 0;
    if (modeCtrl.playerCount >= 2)
        fn_5_14B74(0x3C, modeCtrl.currPlayer + 1);
    else
        fn_5_14B74(0x3C, 0);
    bss_1C8 = 0.0f;
    bss_1CC = 0.016666668f;
    bss_0 = 0x3C;
    data_488 = 2;
    data_48C = 0x4023;
    fn_5_C0D0(2);
}

void fn_5_3380(void)
{
    s32 sounds[4] = {0x1EA, 0x1EB, 0x1EC, 0x1ED};

    if (lbl_1_bss_1700[0] != 1 && modeCtrl.playerCount >= 2)
    {
        if (bss_0 == 0x3B)
            call_SoundReqID_arg_0(sounds[modeCtrl.currPlayer]);
        else if (bss_0 == 1)
            call_SoundReqID_arg_0(0x1F5);
    }
    else if (bss_0 == 0x3B)
        call_SoundReqID_arg_0(0x1F5);
    if (bss_0 < 0)
    {
        if (modeCtrl.playerCount == 1)
            call_SoundReqID_arg_0(0xD81E);
        bss_1C8 = 0.0f;
        bss_1CC = 0.016666668f;
        bss_0 = 0x2710;
        data_488 = 4;
        data_48C = 0x40A1;
        fn_5_C0D0(3);
    }
}

void fn_5_34CC(void)
{
    if (9964.0 == bss_0 && modeCtrl.playerCount >= 2)
        call_SoundReqID_arg_0(0xD81E);
    current_ball->vel.x = 0.00050000000000000001 * pad_status_groups[player_pad_map[current_ball->playerId]].unk0[0].stickX;
    if ((current_ball->pos.x > 2.2100000953674317 && current_ball->vel.x > 0.0) || (current_ball->pos.x < -2.2100000953674317 && current_ball->vel.x < 0.0))
        current_ball->vel.x = 0.0f;
    fn_5_BC78();
    if (pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x100)
    {
        if (bss_0 > 9964.0 && modeCtrl.playerCount >= 2)
            call_SoundReqID_arg_0(0xD81E);
        call_SoundReqID_arg_0(0x11A);
        SoundIcsReq(current_ball->playerId, 0, 0);
        current_ball->vel.x = 0.0f;
        current_ball->unk60.z = 0;
        bss_1C8 = 0.0f;
        bss_1CC = ballParams[current_ball->ape->charaId].c;
        bss_0 = 0x258;
        data_488 = 8;
        data_48C = 0x4125;
        fn_5_C0D0(7);
    }
}

void fn_5_3798(void)
{
    if (bss_0 == 0x12C)
        fn_5_15B24();
    bss_1C8 += bss_1CC;
    if (bss_1C8 >= 1.0)
    {
        bss_1C8 = 1.0f;
        bss_1CC = -bss_1CC;
    }
    else if (bss_1C8 <= 0.0)
    {
        bss_1C8 = 0.0f;
        bss_1CC = -bss_1CC;
    }
    if (bss_1C8 <= 0.0)
        call_SoundReqID_arg_0(0x0104009D);
    if (bss_1C8 >= 1.0)
        call_SoundReqID_arg_0(0xFC009E);
    bss_1B8 = 2.0 * bss_1C8 - 1.0;
    if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x100) || bss_0 < 0)
    {
        destroy_sprite_with_unique_id(0x6B);
        call_SoundReqID_arg_0(0x11A);
        bss_1C8 = 0.0f;
        bss_1CC = 0.016666668f;
        bss_1C0 = 0.0f;
        bss_0 = 0x24C;
        data_488 = 0x10;
        data_48C = 0x4435;
        fn_5_C0D0(8);
    }
    if (pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x200)
    {
        destroy_sprite_with_unique_id(0x6B);
        call_SoundReqID_arg_0(0x119);
        bss_1C8 = 0.0f;
        bss_1CC = 0.016666668f;
        bss_0 = 0x2710;
        data_488 = 4;
        data_48C = 0x40A1;
        fn_5_C0D0(3);
    }
}

void fn_5_39E8(void)
{
    if (bss_0 == 0x12C)
        fn_5_15B24();
    bss_1C0 = 1.0 - fabs(math_sin((s16)(546.13336f * bss_0)));
    if (data_494 == -1)
        data_494 = call_SoundReqID_arg_1_copy(0x11C);
    if (data_494 != -1)
    {
        u8 vol = 60.0 + 9.5714285714285712 * (u8)(7.0f * bss_1C0);
        u16 dop = 2340.4285714285716 * (u16)(7.0f * bss_1C0);

        SoundVol((u16)data_494, vol);
        SoundPan((u16)data_494, 0x40, 127.0f - 127.0f * bss_1C0);
        SoundDop((u16)data_494, dop);
    }
    if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x100) || bss_0 < 0)
    {
        destroy_sprite_with_unique_id(0x6B);
        if (data_494 != -1)
        {
            SoundOff((u16)data_494);
            data_494 = -1;
        }
        call_SoundReqID_arg_0(0x11A);
        bss_0 = 120.0 + 60.0 * (1.0 - bss_1C0);
        data_488 = 0x20;
        data_48C = 0x423D;
        fn_5_C0D0(8);
    }
}

void fn_5_3CA0(void)
{
    float frames;

    bss_1BC += 0.00035714285714285714 * (pad_status_groups[player_pad_map[current_ball->playerId]].unk0[0].triggerRight - pad_status_groups[player_pad_map[current_ball->playerId]].unk0[0].triggerLeft);
    if (bss_1BC > 1.0)
        bss_1BC = 1.0f;
    if (bss_1BC < -1.0)
        bss_1BC = -1.0f;
    frames = 120.0 + 60.0 * (1.0 - bss_1C0);
    current_ball->vel.z = -15.86f / frames;
    current_ball->unk60.x = -0x3E8;
    fn_5_BC78();
    if (bss_0 < 0xC)
        data_48C &= 0xFFFFBFFF;
    if (bss_0 < 0)
    {
        int chara = current_ball->ape->charaId;
        float maxSpeed = ballParams[chara].a;
        float speed = ballParams[chara].b;
        Vec vel;

        SoundIcsReq(current_ball->playerId, 0, 0);
        speed = speed + bss_1C0 * (maxSpeed - speed);
        vel.x = 0.0f;
        vel.y = current_ball->accel * (12.25 + bss_1C0 * ballParams[chara].d);
        vel.z = -speed;
        mtxa_from_rotate_y((s16)(lbl_5_bss_1B0 + lbl_5_bss_1AE * bss_1B8));
        mtxa_tf_point(&vel, &vel);
        current_ball->state = 0x19;
        current_ball->unk154 = 1;
        current_ball->pos.y = 0.0f;
        current_ball->pos.z = -15.86f;
        current_ball->vel = vel;
        current_ball->rotX = 0;
        current_ball->rotY = (s16)(bss_1BC * ballParams[chara].u10);
        current_ball->rotZ = (s16)(bss_1BC * ballParams[chara].u12);
        current_ball->unk60.x = (s16)((1.0f / maxSpeed) * (speed * ballParams[chara].u16));
        current_ball->unk60.y = 0;
        current_ball->unk60.z = 0;
        fn_5_C850(2, current_ball);
        fn_5_12BAC(2);
        fn_5_12194(2);
        fn_5_165B4(2);
        bss_0 = 0x3FC;
        data_488 = 0x40;
        data_48C = 0x80079;
        fn_5_C0D0(1);
    }
}

void fn_5_4184(void)
{
    struct BowlPlayer *player;
    s8 result;

    if (current_ball->pos.z < -95.0 && bss_8 <= 0)
    {
        fn_5_C850(1, current_ball);
        fn_5_12BAC(1);
        fn_5_12194(1);
        fn_5_165B4(1);
        bss_8 = 1;
    }
    if (fn_5_C3C8() != 0)
    {
        switch (lbl_1_bss_1700[0])
        {
        case 0:
        default:
            player = &players[modeCtrl.currPlayer];
            fn_5_7DB8(player);
            if ((current_ball->flags & 0x1000) == 0)
            {
                fn_5_4518();
                break;
            }
            result = player->u35[player->u8];
            if (result == 2 && bss_8 > 0)
            {
                bss_8 = 3;
                bss_7 = 3;
            }
            else if (result == 3 && bss_8 > 0)
            {
                bss_8 = 2;
                bss_7 = 2;
            }
            else
            {
                bss_8 = 1;
                bss_7 = 1;
            }
            fn_5_C850(0, current_ball);
            fn_5_12BAC(0);
            fn_5_12194(0);
            fn_5_165B4(0);
            fn_5_6AE8();
            break;
        case 1:
            bss_1A4 = fn_5_126C8(&bss_1A6);
            fn_5_1280C();
            if ((current_ball->flags & 0x1000) == 0)
            {
                fn_5_638C();
                break;
            }
            if (bss_1A6 == 0 && bss_8 > 0)
            {
                bss_8 = 3;
                bss_7 = 3;
            }
            else
            {
                bss_8 = 1;
                bss_7 = 1;
            }
            fn_5_C850(0, current_ball);
            fn_5_12BAC(0);
            fn_5_12194(0);
            fn_5_165B4(0);
            fn_5_6AE8();
            break;
        case 4:
            fn_5_830C(modeCtrl.currPlayer);
            if ((current_ball->flags & 0x1000) == 0)
            {
                fn_5_7238();
                break;
            }
            if (bss_1A6 == 0 && bss_8 > 0)
            {
                bss_8 = 3;
                bss_7 = 3;
            }
            else
            {
                bss_8 = 1;
                bss_7 = 1;
            }
            fn_5_C850(0, current_ball);
            fn_5_12BAC(0);
            fn_5_12194(0);
            fn_5_165B4(0);
            fn_5_6AE8();
            break;
        }
    }
}

void fn_5_4518(void)
{
    s32 sounds[13] = {0x1CC, 0x1CC, 0x1C6, 0x1CE, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1D0};
    struct BowlPlayer *player = &players[modeCtrl.currPlayer];

    if ((current_ball->flags & 0x1000) == 0)
    {
        fn_5_C0D0(9);
        fn_5_15108(lbl_80198B38 * 2, txtFallOut[locale]);
    }
    else
    {
        switch (player->u35[player->u8])
        {
        case 2:
            if (player->u6 == 0xA && player->u7 == 3 && player->u9 == 3)
            {
                fn_5_14E3C(0xB4, txtPunchOut[locale]);
                call_SoundReqID_arg_0(0x1CA);
            }
            else
            {
                fn_5_14E3C(0xB4, txtStrikes[player->u9][locale]);
                call_SoundReqID_arg_0(sounds[player->u9]);
            }
            if (player->u9 < 3)
                fn_5_BE18(0xA3, 0xA4, 0x14A);
            else if (player->u9 < 0xC)
                fn_5_BE18(0xA9, 0xAA, 0x12C);
            else
                fn_5_BE18(0xA7, 0xA8, 0x1FE);
            break;
        case 3:
            fn_5_14E3C(0xB4, txtSpare[locale]);
            call_SoundReqID_arg_0(0x1CB);
            fn_5_BE18(0xA1, 0xA2, 0xF0);
            break;
        case 4:
        case 1:
            fn_5_153D4(0xB4, player->u20[player->u8]);
            fn_5_BE18(0xA5, 0xA6, 0xD2);
            break;
        default:
            fn_5_15108(0xB4, txtMiss[locale]);
            call_SoundReqID_arg_0(0x1C9);
            fn_5_BE18(0x9F, 0xA0, 0xB4);
            break;
        }
        current_ball->unk154 = 3;
        current_ball->flags |= 0x40;
        fn_5_C0D0(0xA);
        bss_C = 0x168;
    }
    if (player->u9 == 0xC)
        bss_0 = 0x21C;
    else if ((current_ball->flags & 0x1000) == 0)
        bss_0 = 0xF0;
    else
        bss_0 = 0x168;
    data_488 = 0x80;
    data_48C = 0x61;
}

void fn_5_490C(void)
{
    int done;

    if ((current_ball->flags & 0x1000) == 0 && 210.0 == bss_0)
        call_SoundReqID_arg_0(0xC);
    if (players[modeCtrl.currPlayer].u7 == 1 || (players[modeCtrl.currPlayer].u7 == 2 && players[modeCtrl.currPlayer].u6 == 0xA))
    {
        if (g_some_frame_counter.frameCounter < 0x1770)
        {
            g_some_frame_counter.frameCounter += 0x14;
            itemgroups[1].animFrame += 0x14;
            itemgroups[2].animFrame += 0x14;
        }
        if (g_some_frame_counter.frameCounter == 0xBB8 && bss_1A4 == 0)
            fn_5_7BEC();
        if (g_some_frame_counter.frameCounter > 0x528 && g_some_frame_counter.frameCounter < 0x12C0)
        {
            if (bss_1A4 == 0 && g_some_frame_counter.frameCounter < 0xE10)
                fn_5_12860((itemgroups[1].pos.y - 2.37) + (0xE10 - g_some_frame_counter.frameCounter) / 600.0);
            else
                fn_5_12860(itemgroups[1].pos.y - 2.37);
        }
        else
            fn_5_12860(pinPositions[0].y);
        if (g_some_frame_counter.frameCounter > 0x258 && g_some_frame_counter.frameCounter < 0xBB8)
            fn_5_128C4(itemgroups[2].pos.z - 0.2);
    }
    else
    {
        if (g_some_frame_counter.frameCounter < 0x2EE0)
        {
            g_some_frame_counter.frameCounter += 0x14;
            itemgroups[1].animFrame += 0x14;
            itemgroups[2].animFrame += 0x14;
        }
        if (g_some_frame_counter.frameCounter == 0x2580)
            fn_5_7BEC();
        if (g_some_frame_counter.frameCounter >= 0x2580 && g_some_frame_counter.frameCounter < 0x2A30)
        {
            float y;

            if (g_some_frame_counter.frameCounter < 0x27D8)
                y = (itemgroups[1].pos.y - 2.37) + (0x27D8 - g_some_frame_counter.frameCounter) / 600.0;
            else
                y = itemgroups[1].pos.y - 2.37;
            fn_5_12860(y);
        }
        if (g_some_frame_counter.frameCounter == 0x1C20)
            fn_5_127C8();
        if (g_some_frame_counter.frameCounter > 0x1C20 && g_some_frame_counter.frameCounter < 0x2328)
            fn_5_128C4(itemgroups[2].pos.z - 0.2);
    }
    if (bss_0 < 0)
        done = 1;
    else if (!(pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x100))
        done = 0;
    else if ((current_ball->flags & 0x1000) != 0)
    {
        if (bss_0 < 240.0)
            done = 1;
        else
            done = 0;
    }
    else if (bss_0 < 180.0)
        done = 1;
    else
        done = 0;
    if (done != 0)
    {
        struct BowlPlayer *player = &players[modeCtrl.currPlayer];
        s8 nextFrame = 0;

        fn_5_BF50();
        event_dest(0x13);
        data_494 = -1;
        event_init(0x13);
        fn_5_84B4(player);
        fn_5_84B4(player);
        fn_5_83D4(player);
        if (player->u6 == 0xA)
        {
            if (player->u7 >= 3)
                nextFrame = 1;
            else if (player->u7 == 2 && player->u35[0x12] != 2 && player->u35[0x13] != 2 && player->u35[0x13] != 3)
                nextFrame = 1;
        }
        else if (player->u7 >= 2)
            nextFrame = 1;
        else if (player->u35[player->u8] == 2)
            nextFrame = 1;
        bss_C = 0;
        destroy_sprite_with_unique_id(0x6B);
        event_effect_init();
        if (nextFrame != 0)
        {
            int i;

            fn_1_296C();
            for (i = 0; i < 5; i++)
            {
                if (i == modeCtrl.currPlayer)
                    enable_camera(i);
                else
                    disable_camera(i);
            }
            player->u7 = 1;
            player->u6++;
            fn_5_7A5C();
            bss_1A4 = fn_5_126C8(&bss_1A6);
        }
        else
            player->u7++;
        player->u8 = player->u7 - 1 + (player->u6 - 1) * 2;
        if (modeCtrl.currPlayer == 0 && players[0].u6 > 0xA)
        {
            data_490 = 0;
            {
                struct Ball *ball;
                struct Ball *savedBall = current_ball;
                int k;

                ball = balls;
                for (k = 0; k < 8; k++, ball++)
                {
                    if (ball->status == 2)
                    {
                        current_ball = ball;
                        current_ball->state = 1;
                    }
                }
                current_ball = savedBall;
            }
            {
                struct World *world = world_infos;
                s8 *status = g_poolInfo.playerPool.statusList;
                int k;

                for (k = 0; k < g_poolInfo.playerPool.count; k++, world++, status++)
                {
                    if (*status == 2)
                        world->state = 1;
                }
            }
            fn_5_C0D0(0);
            bss_0 = 0x2710;
            data_488 = 0x100;
            data_48C = 0x801;
            if ((dip_switches & 0x20000000) && (merged_digital_inputs[0] & 0x10))
                players[0].u4 = 0x78;
            if (lbl_1_bss_1700[0] == 2)
            {
                int p;
                int best = 0;

                for (p = 0; p < 4; p++)
                {
                    if (best < players[p].u4)
                        best = players[p].u4;
                }
                if (lbl_1_bss_1700[1] < 2 && best >= 0x78 && lbl_1_bss_1700[1] + 1 > fn_1_16C474())
                {
                    bss_6 = 1;
                    fn_1_16C424(lbl_1_bss_1700[1] + 1);
                }
            }
        }
        else
        {
            {
                struct Ball *ball;
                struct Ball *savedBall = current_ball;
                int k;

                ball = balls;
                for (k = 0; k < 8; k++, ball++)
                {
                    if (ball->status == 2)
                    {
                        current_ball = ball;
                        current_ball->state = 1;
                    }
                }
                current_ball = savedBall;
            }
            {
                struct World *world = world_infos;
                s8 *status = g_poolInfo.playerPool.statusList;
                int k;

                for (k = 0; k < g_poolInfo.playerPool.count; k++, world++, status++)
                {
                    if (*status == 2)
                        world->state = 1;
                }
            }
            fn_5_C0D0(0);
            bss_0 = 0x2710;
            data_488 = 1;
        }
    }
}

void fn_5_533C(void)
{
    struct BowlRecord rec[4];
    s16 scores[4];
    s8 charas[4];
    s8 ranks[4];
    int i;
    int m;
    int board;
    int bossCount;

    if (fn_5_BF88() == 0)
        return;
    if (bss_0 >= 0x26D4)
        return;
    if (modeCtrl.playerCount == 1)
    {
        int rank;
        int k;

        for (k = 0; k < 4; k++)
        {
            rec[k].chara = 0;
            rec[k].score = 0;
        }
        rec[0].chara = balls->ape->charaId;
        switch (lbl_1_bss_1700[0])
        {
        case 0:
        default:
            rec[0].score = players->u4;
            board = 0x10;
            break;
        case 2:
            rec[0].score = players->u4;
            switch (lbl_1_bss_1700[1])
            {
            case 0:
            default:
                board = 0x14;
                break;
            case 1:
                board = 0x15;
                break;
            case 2:
                board = 0x16;
                break;
            }
            break;
        case 3:
            rec[0].score = players->u4;
            board = 0x11;
            break;
        case 1:
            rec[0].score = bss_1A8.u2;
            board = 0x13;
            break;
        case 4:
            rec[0].score = lanes[0].u0;
            board = 0x12;
            break;
        }
        rank = fn_1_11E1C0(board, rec);
        if (rank == -1)
        {
            dest_all_events();
            event_init(0x11);
            event_init(0x13);
            event_init(0);
            event_init(0x14);
            event_init(0x10);
            g_some_render_flag &= ~0x20;
            fn_1_11DF64(board, (u8 *)rec, modeCtrl.playerCount);
            fn_1_11DE54(board, 0);
            g_related_to_pause_menu_input[1] |= 2;
            modeCtrl.submodeTimer = 0;
            bss_0 = 0x2710;
            data_488 = 0x400;
            data_48C = 0x2000;
            g_load_selthing();
            return;
        }
        charas[0] = balls->ape->charaId;
        charas[1] = -1;
        charas[2] = -1;
        charas[3] = -1;
        ranks[0] = rank;
        ranks[1] = 3;
        ranks[2] = 3;
        ranks[3] = 3;
    }
    else
    {
        if (lbl_1_bss_1700[0] == 4)
        {
            for (i = 0; i < 4; i++)
                scores[i] = lanes[i].u0;
        }
        else
        {
            for (i = 0; i < 4; i++)
                scores[i] = players[i].u4;
        }
        for (i = 0; i < 4; i++)
        {
            s8 n = 0;
            int j;

            if (g_poolInfo.playerBuf[i] == 0)
                charas[i] = -1;
            else
                charas[i] = balls[i].ape->charaId;
            for (j = 0; j < 4; j++)
            {
                if (j != i && g_poolInfo.playerBuf[j] != 0 && (s16)scores[i] < scores[j])
                    n++;
            }
            ranks[i] = n;
        }
    }
    dest_all_events();
    event_init(0x14);
    event_init(0x10);
    event_init(0x13);
    event_init(0x11);
    event_init(0);
    event_init(0x15);
    light_init(0x9F);
    g_init_rendefc_for_stage();
    data_492 = 2;
    bss_A = 0;
    bossCount = 0;
    if (modeCtrl.playerCount == 1)
        bss_1B4[0] = charas[0];
    else
    {
        for (m = 0; m < 4; m++)
        {
            if (charas[m] != -1 && ranks[m] == 0)
            {
                bss_1B4[bossCount] = charas[m];
                bossCount++;
            }
        }
    }
    bss_10 = 0;
    g_related_to_pause_menu_input[1] |= 2;
    event_init(0xC);
    fn_1_E4708(charas, ranks);
    fn_1_E6BD0(0);
    fn_1_E7600();
    {
        GXColor color = {0x40, 0x40, 0x40, 0xFF};

        fn_1_E7620(color);
    }
    mtxa_from_rotate_y(-0x8000);
    mtxa_translate_xyz(0.5f, 0.809f, 5.0f);
    fn_1_E6760(mtxa->mtxA);
    enable_camera(0);
    fn_5_C0D0(0x11);
    g_some_frame_counter.frameCounter = 0;
    itemgroups[1].animFrame = 0;
    itemgroups[2].animFrame = 0;
    if (lbl_1_bss_1700[0] == 2)
        fn_5_1661C(0, -1);
    modeCtrl.submodeTimer = 0;
    bss_0 = 0x2710;
    data_488 = 0x200;
    data_48C = 0x1000;
    g_load_commend_apes();
}

void fn_5_592C(void)
{
    bss_A--;
    if (bss_A <= 0)
    {
        bss_A = (rand() & 0x3F) + 0x2D;
        data_492--;
        if (data_492 <= 0)
        {
            switch (bss_1B4[bss_10])
            {
            case 0:
                fn_5_111E4(0);
                break;
            case 1:
                fn_5_111E4(1);
                break;
            case 2:
                fn_5_111E4(2);
                break;
            case 3:
                fn_5_111E4(3);
                break;
            }
            bss_10++;
            if (bss_1B4[bss_10] == -1 || bss_10 >= 4)
                bss_10 = 0;
            data_492 = 8;
        }
        else
            fn_5_111E4((rand() & 1) + 4);
    }
    fn_5_10D28();
    if (fn_5_BF88() != 0 && fn_1_E5DC8() != 0 && bss_0 < 0x2530)
    {
        struct BowlRecord rec[4];
        int i;
        int board;

        dest_all_events();
        event_init(0x11);
        event_init(0x13);
        event_init(0);
        event_init(0x14);
        event_init(0x10);
        g_some_render_flag &= ~0x20;
        for (i = 0; i < 4; i++)
        {
            if (g_poolInfo.playerPool.statusList[i] == 0)
                break;
            rec[i].chara = active_monkey_id[i];
            switch (lbl_1_bss_1700[0])
            {
            case 0:
            default:
                board = 0x10;
                rec[i].score = players[i].u4;
                break;
            case 2:
                switch (lbl_1_bss_1700[1])
                {
                case 0:
                default:
                    board = 0x14;
                    break;
                case 1:
                    board = 0x15;
                    break;
                case 2:
                    board = 0x16;
                    break;
                }
                rec[i].score = players[i].u4;
                break;
            case 3:
                board = 0x11;
                rec[i].score = players[i].u4;
                break;
            case 1:
                board = 0x13;
                rec[i].score = bss_1A8.u2;
                break;
            case 4:
                board = 0x12;
                rec[i].score = lanes[i].u0;
                break;
            }
        }
        if (fn_1_11DF64(board, (u8 *)rec, modeCtrl.playerCount) != 0)
            fn_1_11DE54(board, 1);
        else
            fn_1_11DE54(board, 0);
        modeCtrl.submodeTimer = 0;
        bss_0 = 0x2710;
        data_488 = 0x400;
        data_48C = 0x2000;
        g_load_selthing();
    }
}

void fn_5_5CDC(void)
{
    if (modeCtrl.submodeTimer == 0)
    {
        if (fn_5_BF88() != 0 && fn_1_11E3A4() == 0)
        {
            call_SoundReqID_arg_0(0xD1);
            modeCtrl.submodeTimer = 0x3C;
            fade_screen_to_color(0x101, 0xFFFFFF, 0x3C);
            g_fade_track_volume(0x3C, 2);
        }
    }
    else if (--modeCtrl.submodeTimer == 0)
        g_return_to_sel_ngc(modeCtrl.gameType);
}

void fn_5_5D94(void)
{
    u32 i;
    int pin;

    for (i = 0; i < 10; i++)
    {
        pin = bss_1A8.u3;
        if (!(bss_1A8.u0 & (1 << pin)))
            break;
        pin++;
        if (pin >= sizeof(pinPositions) / sizeof(pinPositions[0]))
            pin = 0;
    }
    bss_1A8.u3 = pin;
    if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[4].button & 2)
        || (analog_inputs[player_pad_map[current_ball->playerId]][4] & 2))
    {
        for (i = 0; i < 10; i++)
        {
            pin++;
            if (pin >= sizeof(pinPositions) / sizeof(pinPositions[0]))
                pin = 0;
            if (!(bss_1A8.u0 & (1 << pin)))
                break;
        }
        bss_1A8.u3 = pin;
        call_SoundReqID_arg_0(0x6C);
    }
    else if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[4].button & 1)
             || (analog_inputs[player_pad_map[current_ball->playerId]][4] & 1))
    {
        for (i = 0; i < 10; i++)
        {
            pin--;
            if (pin < 0)
                pin = 9;
            if (!(bss_1A8.u0 & (1 << pin)))
                break;
        }
        bss_1A8.u3 = pin;
        call_SoundReqID_arg_0(0x6C);
    }
    else if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[4].button & 8)
             || (analog_inputs[player_pad_map[current_ball->playerId]][4] & 8))
    {
        pin -= 5;
        if (pin < 0)
            pin += 10;
        if (!(bss_1A8.u0 & (1 << pin)))
            bss_1A8.u3 = pin;
        call_SoundReqID_arg_0(0x6C);
    }
    else if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[4].button & 4)
             || (analog_inputs[player_pad_map[current_ball->playerId]][4] & 4))
    {
        pin += 5;
        if (pin >= sizeof(pinPositions) / sizeof(pinPositions[0]))
            pin -= 10;
        if (!(bss_1A8.u0 & (1 << pin)))
            bss_1A8.u3 = pin;
        call_SoundReqID_arg_0(0x6C);
    }
    fn_5_7A5C();
    bss_1A4 = fn_5_126C8(&bss_1A6);
    if (pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x100)
    {
        bss_1A8.u4--;
        if (bss_1A8.u0 & (1 << bss_1A8.u3))
            call_SoundReqID_arg_0(0x6B);
        else
        {
            call_SoundReqID_arg_0(0x6A);
            fn_5_14B74(0x3C, 0);
            bss_0 = 0x3C;
            data_488 = 2;
            data_48C = 0x4022;
        }
    }
}

void fn_5_6250(void)
{
    g_some_frame_counter.frameCounter = 0x1770;
    itemgroups[1].animFrame = 0x1770;
    itemgroups[2].animFrame = 0x1770;
    fn_5_7A5C();
    bss_1A4 = fn_5_126C8(&bss_1A6);
    bss_7 = 0;
    bss_8 = 0;
    current_ball->state = 0x19;
    current_ball->unk154 = 0;
    bss_1B8 = 0.0f;
    bss_1C0 = 0.0f;
    bss_1BC = 0.0f;
    bss_1C4 = 0;
    bss_1C8 = 0.0f;
    bss_1CC = 1.0f / 60.0f;
    bss_0 = 0x2710;
    data_488 = 0x800;
    data_48C = 0x8000;
    fn_5_C0D0(2);
}

void fn_5_638C(void)
{
    if ((current_ball->flags & 0x1000) == 0)
    {
        fn_5_C0D0(9);
        fn_5_15108(0xB4, "FALL OUT");
        bss_0 = 0xF0;
    }
    else
    {
        if (bss_1A6 == 0)
        {
            fn_5_BE18(0xA9, 0xAA, 5.0 * lbl_80198B38);
            fn_5_14E3C(0xB4, txtClear[locale]);
            call_SoundReqID_arg_0(0x1CF);
        }
        else
        {
            fn_5_BE18(0x9F, 0xA0, 3.0 * lbl_80198B38);
            fn_5_15108(0xB4, txtTryAgain[locale]);
            call_SoundReqID_arg_0(0x1CD);
        }
        current_ball->unk154 = 3;
        current_ball->flags |= 0x40;
        fn_5_C0D0(0xA);
        bss_C = 0x168;
        bss_0 = 0x168;
    }
    data_488 = 0x2000;
    data_48C = 0x60;
}

void fn_5_65A4(void)
{
    int done;

    if ((current_ball->flags & 0x1000) == 0 && 210.0 == bss_0)
        call_SoundReqID_arg_0(0xC);
    if (g_some_frame_counter.frameCounter < 0x2EE0)
    {
        g_some_frame_counter.frameCounter += 0x14;
        itemgroups[1].animFrame += 0x14;
        itemgroups[2].animFrame += 0x14;
    }
    if (g_some_frame_counter.frameCounter == 0x2580)
        fn_5_7A5C();
    if (g_some_frame_counter.frameCounter >= 0x2580 && g_some_frame_counter.frameCounter < 0x2A30)
    {
        float y;

        if (g_some_frame_counter.frameCounter < 0x27D8)
            y = (itemgroups[1].pos.y - 2.37) + (0x27D8 - g_some_frame_counter.frameCounter) / 600.0;
        else
            y = itemgroups[1].pos.y - 2.37;
        fn_5_12860(y);
    }
    if (g_some_frame_counter.frameCounter == 0x1C20)
        fn_5_127C8();
    if (g_some_frame_counter.frameCounter > 0x1C20 && g_some_frame_counter.frameCounter < 0x2328)
        fn_5_128C4(itemgroups[2].pos.z - 0.2);
    if (bss_0 < 0)
        done = 1;
    else if (!(pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x100))
        done = 0;
    else if ((current_ball->flags & 0x1000) != 0)
    {
        if (bss_0 < 240.0)
            done = 1;
        else
            done = 0;
    }
    else if (bss_0 < 180.0)
        done = 1;
    else
        done = 0;
    if (done != 0)
    {
        fn_5_BF50();
        event_dest(0x13);
        data_494 = -1;
        event_init(0x13);
        if (bss_1A6 == 0)
        {
            bss_1A8.u0 |= 1 << bss_1A8.u3;
            bss_1A8.u2++;
        }
        bss_C = 0;
        destroy_sprite_with_unique_id(0x6B);
        event_effect_init();
        if (bss_1A8.u2 >= sizeof(pinPositions) / sizeof(pinPositions[0]))
        {
            fn_5_15918(0x2710);
            fn_5_15E3C(0x2710);
            bss_0 = 0x2710;
            data_488 = 0x100;
            data_48C = 0x20000;
        }
        else if (bss_1A8.u4 <= 0)
        {
            fn_5_1570C(0x2710);
            fn_5_15E3C(0x2710);
            bss_0 = 0x2710;
            data_488 = 0x100;
            data_48C = 0x10000;
        }
        else
        {
            bss_0 = 0x2710;
            data_488 = 0x1000;
            data_48C = 0x20;
        }
        {
            struct Ball *ball;
            struct Ball *savedBall = current_ball;
            int k;

            ball = balls;
            for (k = 0; k < 8; k++, ball++)
            {
                if (ball->status == 2)
                {
                    current_ball = ball;
                    current_ball->state = 1;
                }
            }
            current_ball = savedBall;
        }
        {
            struct World *world = world_infos;
            s8 *status = g_poolInfo.playerPool.statusList;
            int k;

            for (k = 0; k < g_poolInfo.playerPool.count; k++, world++, status++)
            {
                if (*status == 2)
                    world->state = 1;
            }
        }
        fn_5_C0D0(0);
    }
}

void fn_5_6AE8(void)
{
    if (bss_7 == 1)
    {
        fn_5_C8CC(2, current_ball);
        fn_5_12C28(2);
        fn_5_121E4(2);
        fn_5_165E8(2);
        fn_5_C0D0(0xE);
    }
    else
    {
        fn_5_C8CC(1, current_ball);
        fn_5_12C28(1);
        fn_5_121E4(1);
        fn_5_165E8(1);
        if (bss_7 == 2)
            fn_5_C0D0(0xC);
        else if (bss_7 == 3)
            fn_5_C0D0(0xD);
        else
            fn_5_C0D0(1);
    }
    bss_0 = 0x3FC;
    data_488 = 0x4000;
    data_48C = 0xC0000;
}

void fn_5_6C00(void)
{
    int done = 0;

    if (pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x200)
    {
        if (cameras[modeCtrl.currPlayer].unk1F == 0xF)
            fn_5_C0D0(0x10);
        else if (cameras[modeCtrl.currPlayer].unk1F == 0x10)
            fn_5_C0D0(0xF);
    }
    if ((pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x100) && bss_0 < 0x3C0)
        done = 1;
    else if (bss_0 < 0 || (current_ball->pos.y < -5.0 && bss_7 > 1) || fn_5_C3C8() != 0)
    {
        bss_7--;
        if (bss_7 > 0)
            fn_5_6AE8();
        else
            done = 1;
    }
    if (done != 0)
    {
        fn_5_C8CC(0, current_ball);
        fn_5_12C28(0);
        fn_5_121E4(0);
        fn_5_165E8(0);
        switch (lbl_1_bss_1700[0])
        {
        default:
            fn_5_4518();
            break;
        case 1:
            fn_5_638C();
            break;
        case 4:
            fn_5_7238();
            break;
        }
    }
}

void fn_5_70C4(void)
{
    g_some_frame_counter.frameCounter = 0x1770;
    itemgroups[1].animFrame = 0x1770;
    itemgroups[2].animFrame = 0x1770;
    fn_5_12860(pinPositions[0].y);
    bss_1A4 = fn_5_126C8(&bss_1A6);
    fn_5_1266C();
    fn_5_7A5C();
    bss_1A4 = fn_5_126C8(&bss_1A6);
    bss_7 = 0;
    bss_8 = 0;
    bss_1B8 = 0.0f;
    bss_1C0 = 0.0f;
    bss_1BC = 0.0f;
    bss_1C4 = 0;
    current_ball->state = 0x19;
    current_ball->unk154 = 0;
    if (modeCtrl.playerCount >= 2)
        fn_5_14B74(0x3C, modeCtrl.currPlayer + 1);
    else
        fn_5_14B74(0x3C, 0);
    bss_1C8 = 0.0f;
    bss_1CC = 1.0f / 60.0f;
    bss_0 = 0x3C;
    data_488 = 2;
    data_48C = 0x4023;
}

void fn_5_7238(void)
{
    char *msgs[] = {
        "\\", "STRIKE\\", "DOUBLE\\", "TURKEY\\", "4TH STRIKE\\", "5TH STRIKE\\", "6TH STRIKE\\",
        "7TH STRIKE\\", "8TH STRIKE\\", "9TH STRIKE\\", "10TH STRIKE\\", "11TH STRIKE\\", "12TH STRIKE\\",
        "13TH STRIKE\\", "14TH STRIKE\\", "15TH STRIKE\\", "16TH STRIKE\\", "17TH STRIKE\\", "18TH STRIKE\\",
        "19TH STRIKE\\", "PERFECT\\",
    };
    int sounds[] = {
        0x1CC, 0x1CC, 0x1C6, 0x1CE, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7,
        0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1D0,
    };
    struct BowlLane *lane = &lanes[modeCtrl.currPlayer];

    if ((current_ball->flags & 0x1000) == 0)
    {
        fn_5_C0D0(9);
        fn_5_15108(0x78, "FALL OUT");
    }
    else
    {
        s8 pins = lane->u4[lane->u2 - 1];

        if (pins == 10)
        {
            s8 streak = lane->u3;

            fn_5_14E3C(0xB4, msgs[streak]);
            call_SoundReqID_arg_0(sounds[streak]);
            if (streak < 3)
                fn_5_BE18(0xA3, 0xA4, 0x14A);
            else if (streak < 0x14)
                fn_5_BE18(0xA9, 0xAA, 0x12C);
            else
                fn_5_BE18(0xA7, 0xA8, 0x1FE);
        }
        else if (pins > 0)
        {
            fn_5_153D4(0xB4, pins);
            fn_5_BE18(0xA5, 0xA6, 0xD2);
        }
        else
        {
            fn_5_15108(0xB4, "MISS");
            call_SoundReqID_arg_0(0x1C9);
            fn_5_BE18(0x9F, 0xA0, 0xB4);
        }
        current_ball->unk154 = 3;
        current_ball->flags |= 0x40;
        fn_5_C0D0(0xA);
        bss_C = 0x168;
    }
    if (lane->u3 == 0xC)
        bss_0 = 0x21C;
    else if ((current_ball->flags & 0x1000) == 0)
        bss_0 = 0xF0;
    else
        bss_0 = 0x168;
    data_488 = 0x10000;
    data_48C = 0x61;
}

void fn_5_751C(void)
{
    int done;

    if ((current_ball->flags & 0x1000) == 0 && 210.0 == bss_0)
        call_SoundReqID_arg_0(0xC);
    if (g_some_frame_counter.frameCounter < 0x2EE0)
    {
        g_some_frame_counter.frameCounter += 0x14;
        itemgroups[1].animFrame += 0x14;
        itemgroups[2].animFrame += 0x14;
    }
    if (g_some_frame_counter.frameCounter == 0x2580)
        fn_5_7A5C();
    if (g_some_frame_counter.frameCounter >= 0x2580 && g_some_frame_counter.frameCounter < 0x2A30)
    {
        float y;

        if (g_some_frame_counter.frameCounter < 0x27D8)
            y = (itemgroups[1].pos.y - 2.37) + (0x27D8 - g_some_frame_counter.frameCounter) / 600.0;
        else
            y = itemgroups[1].pos.y - 2.37;
        fn_5_12860(y);
    }
    if (g_some_frame_counter.frameCounter == 0x1C20)
        fn_5_127C8();
    if (g_some_frame_counter.frameCounter > 0x1C20 && g_some_frame_counter.frameCounter < 0x2328)
        fn_5_128C4(itemgroups[2].pos.z - 0.2);
    if (bss_0 < 0)
        done = 1;
    else if (!(pad_status_groups[player_pad_map[current_ball->playerId]].unk0[2].button & 0x100))
        done = 0;
    else if ((current_ball->flags & 0x1000) != 0)
    {
        if (bss_0 < 240.0)
            done = 1;
        else
            done = 0;
    }
    else if (bss_0 < 180.0)
        done = 1;
    else
        done = 0;
    if (done != 0)
    {
        {
            struct BowlLane *lane = &lanes[modeCtrl.currPlayer];
            int i;

            fn_5_BF50();
            event_dest(0x13);
            data_494 = -1;
            event_init(0x13);
            bss_C = 0;
            destroy_sprite_with_unique_id(0x6B);
            event_effect_init();
            lane->u2++;
            fn_1_296C();
            for (i = 0; i < 5; i++)
            {
                if (i == modeCtrl.currPlayer)
                    enable_camera(i);
                else
                    disable_camera(i);
            }
        }
        fn_5_7A5C();
        bss_1A4 = fn_5_126C8(&bss_1A6);
        {
            struct Ball *ball;
            struct Ball *savedBall = current_ball;
            int k;

            ball = balls;
            for (k = 0; k < 8; k++, ball++)
            {
                if (ball->status == 2)
                {
                    current_ball = ball;
                    current_ball->state = 1;
                }
            }
            current_ball = savedBall;
        }
        {
            struct World *world = world_infos;
            s8 *status = g_poolInfo.playerPool.statusList;
            int k;

            for (k = 0; k < g_poolInfo.playerPool.count; k++, world++, status++)
            {
                if (*status == 2)
                    world->state = 1;
            }
        }
        fn_5_C0D0(0);
        if (modeCtrl.currPlayer == 0 && lanes->u2 > 0x14)
        {
            data_490 = 0;
            bss_0 = 0x2710;
            data_488 = 0x100;
            data_48C = 0x801;
        }
        else
        {
            bss_0 = 0x2710;
            data_488 = 0x8000;
        }
    }
}

void fn_5_7A5C(void)
{
    int i;

    fn_5_12234();
    switch (lbl_1_bss_1700[0])
    {
    case 0:
    case 2:
    case 4:
    default:
        for (i = 0; i < sizeof(pinPositions) / sizeof(pinPositions[0]); i++)
            fn_5_1256C((Vec *)&pinPositions[i], i);
        break;
    case 1:
        {
            u16 mask = pinSplits[bss_1A8.u3].mask;

            for (i = 0; i < sizeof(pinPositions) / sizeof(pinPositions[0]); i++)
            {
                if (mask & (1 << i))
                    fn_5_1256C((Vec *)&pinPositions[i], i);
            }
        }
        break;
    case 3:
        {
            s8 order[] = { 1, 7, 10, 8, 6, 2, 9, 4, 3, 5 };
            s8 idx = players[modeCtrl.currPlayer].u6 - 1;

            for (i = 0; i < sizeof(pinPositions) / sizeof(pinPositions[0]); i++)
            {
                if (i != order[idx] - 1)
                    fn_5_1256C((Vec *)&pinPositions[i], i);
            }
        }
        break;
    }
}

void fn_5_7BEC(void)
{
    int i;

    fn_5_12234();
    switch (lbl_1_bss_1700[0])
    {
    case 0:
    case 2:
    case 4:
    default:
        for (i = 0; i < sizeof(pinPositions) / sizeof(pinPositions[0]); i++)
            fn_5_1256C((Vec *)&pinPositions[i], i);
        break;
    case 1:
        {
            u16 mask = pinSplits[bss_1A8.u3].mask;

            for (i = 0; i < sizeof(pinPositions) / sizeof(pinPositions[0]); i++)
            {
                if (mask & (1 << i))
                    fn_5_1256C((Vec *)&pinPositions[i], i);
            }
        }
        break;
    case 3:
        {
            s8 order[] = { 1, 7, 10, 8, 6, 2, 9, 4, 3, 5 };
            s8 idx = players[modeCtrl.currPlayer].u6 - 1;

            if (players[modeCtrl.currPlayer].u6 != 10 && get_next_player_idx() == 0)
                idx++;
            for (i = 0; i < sizeof(pinPositions) / sizeof(pinPositions[0]); i++)
            {
                if (i != order[idx] - 1)
                    fn_5_1256C((Vec *)&pinPositions[i], i);
            }
        }
        break;
    }
}

void fn_5_7DB8(struct BowlPlayer *player)
{
    s8 idx = player->u8;

    bss_1A4 = fn_5_126C8(&bss_1A6);
    fn_5_1280C();
    player->u0 = NULL;
    if (player->u7 == 1)
    {
        player->u20[idx] = (s8)(10 - bss_1A6);
        if (bss_1A6 == 0)
            player->u35[idx] = 2;
        else if (bss_1A6 == 10)
            player->u35[idx] = 5;
        else
        {
            player->u0 = fn_5_87E0();
            if (player->u0 == NULL)
                player->u35[idx] = 1;
            else
                player->u35[idx] = 4;
        }
    }
    else if (player->u6 == 10 && player->u7 == 2)
    {
        int n = 10 - bss_1A6;

        if (player->u35[idx - 1] == 2)
            player->u20[idx] = (s8)n;
        else
            {
                int d = n - player->u20[idx - 1];

                player->u20[idx] = (s8)d;
            }
        if (bss_1A6 == 0)
        {
            if (player->u35[idx - 1] == 2)
                player->u35[idx] = 2;
            else
                player->u35[idx] = 3;
        }
        else if (n == player->u20[idx - 1] || n == 0)
        {
            if (player->u35[idx - 1] == 2)
                player->u35[idx] = 5;
            else
                player->u35[idx] = 7;
        }
        else if (player->u35[idx - 1] == 2)
        {
            player->u0 = fn_5_87E0();
            if (player->u0 == NULL)
                player->u35[idx] = 1;
            else
                player->u35[idx] = 4;
        }
        else
            player->u35[idx] = 1;
    }
    else if (player->u6 == 10 && player->u7 == 3)
    {
        int n = 10 - bss_1A6;

        if (player->u35[idx - 1] == 2 || player->u35[idx - 1] == 3)
            player->u20[idx] = (s8)n;
        else
            {
                int d = n - player->u20[idx - 1];

                player->u20[idx] = (s8)d;
            }
        if (bss_1A6 == 0)
        {
            if (player->u35[idx - 1] == 2 || player->u35[idx - 1] == 3)
                player->u35[idx] = 2;
            else
                player->u35[idx] = 3;
        }
        else if (bss_1A6 == 10 && (player->u35[idx - 1] == 2 || player->u35[idx - 1] == 3))
            player->u35[idx] = 5;
        else if (player->u35[idx - 1] != 3 && n == player->u20[idx - 1])
            player->u35[idx] = 7;
        else
            player->u35[idx] = 1;
    }
    else
    {
        int n = 10 - bss_1A6;

        {
                int d = n - player->u20[idx - 1];

                player->u20[idx] = (s8)d;
            }
        if (bss_1A6 == 0)
            player->u35[idx] = 3;
        else if (n == player->u20[idx - 1])
            player->u35[idx] = 7;
        else
            player->u35[idx] = 1;
    }
    if (player->u35[idx] == 2)
        player->u9++;
    else
        player->u9 = 0;
}

void fn_5_830C(s32 player)
{
    struct BowlLane *lane = &lanes[player];
    s8 n;

    bss_1A4 = fn_5_126C8(&bss_1A6);
    fn_5_1280C();
    n = 10 - bss_1A6;
    if (n == 0)
        lane->u4[lane->u2 - 1] = -1;
    else
    {
        lane->u4[lane->u2 - 1] = n;
        lane->u0 += n;
    }
    if (bss_1A6 == 0)
        lane->u3++;
    else
        lane->u3 = 0;
}

void fn_5_83D4(struct BowlPlayer *player)
{
    int i;
    int total;

    for (i = 0, total = 0; i < 0x15; i++)
    {
        total += player->u20[i];
        if (i < 0x12)
        {
            if (player->u35[i] == 2)
            {
                int j = i + 1;
                int n = 0;

                while (n < 2)
                {
                    if (player->u35[j] != 0)
                    {
                        total += player->u20[j];
                        n++;
                    }
                    j++;
                }
            }
            else if (player->u35[i] == 3)
            {
                if (player->u35[i + 1] != 0)
                    total += player->u20[i + 1];
                else
                    total += player->u20[i + 2];
            }
        }
    }
    player->u4 = (s16)total;
}

void fn_5_84B4(struct BowlPlayer *player)
{
    int frame;

    for (frame = 1; frame <= 10; frame++)
    {
        if (player->frameScore[frame] == -1)
            break;
    }
    if (frame == 10)
    {
        if (player->u35[0x12] != 0 && player->u35[0x13] != 0
            && ((player->u35[0x12] != 2 && player->u35[0x13] != 2 && player->u35[0x13] != 3) || player->u35[0x14] != 0))
            player->frameScore[frame] = player->frameScore[frame - 1] + player->u20[0x12] + player->u20[0x13] + player->u20[0x14];
    }
    else
    {
        int j = (frame - 1) * 2;

        if (player->u35[j] == 2)
        {
            if (frame == 9)
            {
                if (player->u35[j + 2] != 0 && player->u35[j + 3] != 0)
                    player->frameScore[frame] = player->frameScore[frame - 1] + player->u20[j] + player->u20[j + 2] + player->u20[j + 3];
            }
            else if (player->u35[j + 2] != 0)
            {
                if (player->u35[j + 2] == 2)
                {
                    if (player->u35[j + 4] != 0)
                        player->frameScore[frame] = player->frameScore[frame - 1] + player->u20[j] + player->u20[j + 2] + player->u20[j + 4];
                }
                else if (player->u35[j + 3] != 0)
                    player->frameScore[frame] = player->frameScore[frame - 1] + player->u20[j] + player->u20[j + 2] + player->u20[j + 3];
            }
        }
        else if (player->u35[j + 1] == 3)
        {
            if (player->u35[j + 2] != 0)
                player->frameScore[frame] = player->frameScore[frame - 1] + player->u20[j] + player->u20[j + 1] + player->u20[j + 2];
        }
        else if (player->u35[j + 1] != 0)
            player->frameScore[frame] = player->frameScore[frame - 1] + player->u20[j] + player->u20[j + 1];
    }
}

char *fn_5_87E0(void)
{
    static const u16 nonSplits[] = {
        0xA, 0x12, 0x14, 0x24, 0x48, 0x88, 0x90, 0x110, 0x120, 0x220,
        0x1A, 0x4A, 0x8A, 0x92, 0x112, 0x34, 0x94, 0x114, 0x124, 0x224,
        0xC8, 0x190, 0x320, 0x5A, 0x9A, 0x11A, 0xCA, 0x192, 0xB4, 0x134,
        0x234, 0x194, 0x324, 0xDA, 0x15A, 0x19A, 0x1B4, 0x2B4, 0x334, 0x1DA,
        0x3B4,
    };
    static const struct
    {
        char *name;
        u16 mask;
    } splits[] = {
        { "split \"making it fit\"", 0x6 },
        { "split \"making it fit\"", 0x18 },
        { "split \"making it fit\"", 0x30 },
        { "split \"making it fit\"", 0xC0 },
        { "split \"making it fit\"", 0x180 },
        { "split \"making it fit\"", 0x300 },
        { "split \"rail road\"", 0x28 },
        { "split \"rail road\"", 0x140 },
        { "split \"cincinnati\"", 0x280 },
        { "split \"snake eye\"", 0x240 },
        { "\"baby split\"", 0x42 },
        { "\"baby split\"", 0x204 },
        { "\"baby split\"", 0x102 },
        { "\"baby split\"", 0x84 },
        { "split \"one in the dark\"", 0x82 },
        { "split \"one in the dark\"", 0x104 },
        { "split \"dime store\"", 0x210 },
        { "split \"6-7\"", 0x60 },
        { "split \"christmas tree\"", 0x242 },
        { "split \"christmas tree\"", 0x244 },
        { "split \"big three\"", 0x248 },
        { "split \"lily\"", 0x250 },
        { "split \"big three\"", 0x260 },
        { "split \"big four\"", 0x268 },
        { "split \"4-7-9-10\"", 0x348 },
        { "split \"big five\"", 0x2E8 },
        { "split \"big five\"", 0x368 },
    };
    u32 i;

    if (bss_1A6 < 2)
        return NULL;
    if (bss_1A4 & 1)
        return NULL;
    for (i = 0; i < sizeof(nonSplits) / sizeof(nonSplits[0]); i++)
    {
        if (bss_1A4 == nonSplits[i])
            return NULL;
    }
    for (i = 0; i < sizeof(splits) / sizeof(splits[0]); i++)
    {
        if (bss_1A4 == splits[i].mask)
            return splits[i].name;
    }
    return "split";
}

void fn_5_88CC(float x)
{
    struct SpriteDrawRequest barReq = spr5;
    struct SpriteDrawRequest markReq = spr6;
    struct SpriteDrawRequest ballReq = sprD[current_ball->colorId];
    struct SpriteDrawRequest iconReq = spr11[current_ball->ape->charaId];
    struct SpriteDrawRequest reqF8;
    struct SpriteDrawRequest reqA8;
    struct SpriteDrawRequest req58;
    struct SpriteDrawRequest req8;
    int i;

    if ((g_some_other_flags & 0xA) == 0)
    {
        s16 prev = bss_1C4;
        float delta = x * ballParams[current_ball->ape->charaId].u14;

        bss_1C4 += delta;
        if (0.0 != x && bss_1C4 >= 0 && prev < 0)
        {
            float opacity = __fabs(delta) / 5431.0;

            if (opacity > 1.0)
                opacity = 1.0f;
            i = call_SoundReqID_arg_1_copy(0x9C);
            if (i != -1)
            {
                SoundVol(i, 95.0 + 10.0 * __fabs(bss_1BC));
                SoundDop(i, 8192.0f + 8191.0f * opacity);
            }
        }
    }
    ballReq.rotation = bss_1C4;
    iconReq.rotation = bss_1C4;
    if (x >= 0.0)
    {
        reqF8 = spr9;
        reqA8 = sprA;
    }
    else
    {
        reqF8 = sprB;
        reqA8 = sprC;
    }
    reqF8.rotation = reqA8.rotation = -65536.0f * x;
    if (reqF8.rotation < -0x8000)
    {
        markReq.z = 0.15000001f;
        reqA8.z = 0.14f;
        barReq.z = 0.13f;
        reqF8.z = 0.12f;
        draw_sprite_draw_request(&markReq);
        draw_sprite_draw_request(&reqA8);
        draw_sprite_draw_request(&barReq);
        draw_sprite_draw_request(&reqF8);
    }
    else if (reqF8.rotation < 0)
    {
        markReq.z = 0.15000001f;
        reqF8.z = 0.14f;
        barReq.z = 0.13f;
        draw_sprite_draw_request(&markReq);
        draw_sprite_draw_request(&reqF8);
        draw_sprite_draw_request(&barReq);
    }
    else if (reqF8.rotation == 0)
    {
        draw_sprite_draw_request(&barReq);
        draw_sprite_draw_request(&markReq);
    }
    else if (reqF8.rotation < 0x8000)
    {
        barReq.z = 0.15000001f;
        reqF8.z = 0.14f;
        markReq.z = 0.13f;
        draw_sprite_draw_request(&barReq);
        draw_sprite_draw_request(&reqF8);
        draw_sprite_draw_request(&markReq);
    }
    else
    {
        barReq.z = 0.15000001f;
        reqA8.z = 0.14f;
        markReq.z = 0.13f;
        reqF8.z = 0.12f;
        draw_sprite_draw_request(&barReq);
        draw_sprite_draw_request(&reqA8);
        draw_sprite_draw_request(&markReq);
        draw_sprite_draw_request(&reqF8);
    }
    draw_sprite_draw_request(&ballReq);
    draw_sprite_draw_request(&iconReq);
    for (i = 0; i < 5; i++)
    {
        float opacity = 1.0 - 0.032258064f * (bss_4 & 0x1F) + 0.2 * i;

        if (opacity > 1.0)
            opacity -= 1.0;
        if (x <= 0.0)
            req58 = spr15[i];
        else
        {
            req58 = spr15[4 - i];
            req58.rotation += 0x8000;
        }
        req58.z = barReq.z - 0.005;
        req58.opacity = opacity * opacity;
        draw_sprite_draw_request(&req58);
        if (x >= 0.0)
            req8 = spr1A[i];
        else
        {
            req8 = spr1A[4 - i];
            req8.rotation += 0x8000;
        }
        req8.z = markReq.z - 0.005;
        req8.opacity = opacity * opacity;
        draw_sprite_draw_request(&req8);
    }
}

void fn_5_8F38(float x, float y)
{
    s8 i;
    s8 p;
    s8 frame = players[modeCtrl.currPlayer].u6 < 1 ? 1 : (players[modeCtrl.currPlayer].u6 > 10 ? 10 : players[modeCtrl.currPlayer].u6);
    s8 first = (frame - data_491 + 1) > 1 ? (frame - data_491 + 1) : 1;
    s8 end = first + data_491;
    struct SpriteDrawRequest req;
    float px;

    textdraw_reset();
    textdraw_set_font(0x3C);
    textdraw_set_mul_color(0x532A02);
    req = spr3E;
    req.x += x;
    req.y += y;
    px = req.x;
    y = req.y;
    draw_sprite_draw_request(&req);
    px += 57.0f;
    for (i = first; i < end; i++)
    {
        if (i != 10)
        {
            req = spr3F;
            req.x = px;
            req.y = y;
            draw_sprite_draw_request(&req);
            textdraw_set_pos(13.0f + px, 4.0f + y);
            textdraw_put_char(i);
            px += 42.0f;
        }
        else
        {
            req = spr40;
            req.x = px;
            req.y = y;
            draw_sprite_draw_request(&req);
            textdraw_set_pos(14.0f + px, 4.0f + y);
            textdraw_put_char(1);
            textdraw_set_pos(25.0f + px, 4.0f + y);
            textdraw_put_char(0);
            px += 57.0f;
        }
    }
    req = spr41;
    req.x = px;
    req.y = y;
    draw_sprite_draw_request(&req);
    y += 23.0f;
    for (p = 0; p < modeCtrl.playerCount; p++)
    {
        u8 colors[4] = { 2, 1, 3, 0 };

        req = spr43[colors[balls[p].colorId]];
        req.x += x;
        req.y = y;
        draw_sprite_draw_request(&req);
        px = req.x;
        req = spr4F[p];
        req.x += px;
        req.y += y;
        draw_sprite_draw_request(&req);
        px += 57.0f;
        for (i = first; i < end; i++)
        {
            s8 mark;
            s16 score = players[p].frameScore[i];

            if (i != 10)
            {
                req = spr47;
                req.x = px;
                req.y = y;
                draw_sprite_draw_request(&req);
                if (modeCtrl.currPlayer == p && i == frame && (data_488 & 0x300) == 0)
                {
                    req = spr3D;
                    req.x += px;
                    req.y += y;
                    req.opacity = 0.4 + 0.25 * math_sin((u16)(bss_4 * 0x400));
                    draw_sprite_draw_request(&req);
                }
                mark = fn_5_A580(&players[p], (i - 1) * 2);
                if (mark > 0)
                {
                    textdraw_set_pos(2.0f + px, 2.0f + y);
                    textdraw_put_char(mark);
                }
                else if (mark < 0)
                {
                    req = spr4E;
                    req.x += px;
                    req.y += y;
                    draw_sprite_draw_request(&req);
                    textdraw_set_pos(2.0f + px, 2.0f + y);
                    textdraw_put_char(-mark);
                }
                mark = fn_5_A580(&players[p], (i - 1) * 2 + 1);
                if (mark > 0)
                {
                    textdraw_set_pos(23.0f + px, 2.0f + y);
                    textdraw_put_char(mark);
                }
                if (score != -1)
                {
                    if (score < 10)
                    {
                        textdraw_set_pos(11.0f + px, 17.0f + y);
                        textdraw_put_char(score);
                    }
                    else if (score < 100)
                    {
                        s16 tens = score / 10;

                        score = score - tens * 10;
                        textdraw_set_pos(5.0f + px, 17.0f + y);
                        textdraw_put_char(tens);
                        textdraw_set_pos(16.0f + px, 17.0f + y);
                        textdraw_put_char(score);
                    }
                    else
                    {
                        s16 hundreds = score / 100;
                        s16 tens = (score - hundreds * 100) / 10;

                        score = score - hundreds * 100 - tens * 10;
                        textdraw_set_pos(px, 17.0f + y);
                        textdraw_put_char(hundreds);
                        textdraw_set_pos(11.0f + px, 17.0f + y);
                        textdraw_put_char(tens);
                        textdraw_set_pos(22.0f + px, 17.0f + y);
                        textdraw_put_char(score);
                    }
                }
                px += 42.0f;
            }
            else
            {
                req = spr48;
                req.x = px;
                req.y = y;
                draw_sprite_draw_request(&req);
                if (modeCtrl.currPlayer == p && i == frame && (data_488 & 0x300) == 0)
                {
                    req = spr3D;
                    req.x += px;
                    req.y += y;
                    req.scaleX *= 1.375;
                    req.opacity = 0.4 + 0.25 * math_sin((u16)(bss_4 * 0x400));
                    draw_sprite_draw_request(&req);
                }
                mark = fn_5_A580(&players[p], (i - 1) * 2);
                if (mark > 0)
                {
                    textdraw_set_pos(1.0f + px, 2.0f + y);
                    textdraw_put_char(mark);
                }
                else if (mark < 0)
                {
                    req = spr4E;
                    req.x += px;
                    req.y += y;
                    draw_sprite_draw_request(&req);
                    textdraw_set_pos(1.0f + px, 2.0f + y);
                    textdraw_put_char(-mark);
                }
                mark = fn_5_A580(&players[p], (i - 1) * 2 + 1);
                if (mark > 0)
                {
                    textdraw_set_pos(20.0f + px, 2.0f + y);
                    textdraw_put_char(mark);
                }
                else if (mark < 0)
                {
                    req = spr4E;
                    req.x += 19.0f + px;
                    req.y += y;
                    draw_sprite_draw_request(&req);
                    textdraw_set_pos(20.0f + px, 2.0f + y);
                    textdraw_put_char(-mark);
                }
                mark = fn_5_A580(&players[p], (i - 1) * 2 + 2);
                if (mark > 0)
                {
                    textdraw_set_pos(39.0f + px, 2.0f + y);
                    textdraw_put_char(mark);
                }
                if (score != -1)
                {
                    if (score < 10)
                    {
                        textdraw_set_pos(18.0f + px, 17.0f + y);
                        textdraw_put_char(score);
                    }
                    else if (score < 100)
                    {
                        s16 tens = score / 10;

                        score = score - tens * 10;
                        textdraw_set_pos(12.0f + px, 17.0f + y);
                        textdraw_put_char(tens);
                        textdraw_set_pos(23.0f + px, 17.0f + y);
                        textdraw_put_char(score);
                    }
                    else
                    {
                        s16 hundreds = score / 100;
                        s16 tens = (score - hundreds * 100) / 10;

                        score = score - hundreds * 100 - tens * 10;
                        textdraw_set_pos(7.0f + px, 17.0f + y);
                        textdraw_put_char(hundreds);
                        textdraw_set_pos(18.0f + px, 17.0f + y);
                        textdraw_put_char(tens);
                        textdraw_set_pos(29.0f + px, 17.0f + y);
                        textdraw_put_char(score);
                    }
                }
                px += 57.0f;
            }
        }
        req = spr49;
        req.x = px;
        req.y = y;
        draw_sprite_draw_request(&req);
        y += 30.0f;
    }
    req = spr4A;
    req.x += x;
    req.y = y;
    draw_sprite_draw_request(&req);
    px = 57.0f + req.x;
    for (i = first; i < end; i++)
    {
        if (i != 10)
        {
            req = spr4B;
            req.x = px;
            req.y = y;
            px += 42.0f;
        }
        else
        {
            req = spr4C;
            req.x = px;
            req.y = y;
            px += 57.0f;
        }
        draw_sprite_draw_request(&req);
    }
    req = spr4D;
    req.x = px;
    req.y = y;
    draw_sprite_draw_request(&req);
}

void fn_5_9C90(float x, float y)
{
    s8 i;
    s8 p;
    s8 frame = lanes[modeCtrl.currPlayer].u2 < 1 ? 1 : (lanes[modeCtrl.currPlayer].u2 > 20 ? 20 : lanes[modeCtrl.currPlayer].u2);
    s8 first = (frame - data_491 + 1) > 1 ? (frame - data_491 + 1) : 1;
    s8 end = first + data_491;
    struct SpriteDrawRequest req;
    float px;

    textdraw_reset();
    textdraw_set_font(0x3C);
    textdraw_set_mul_color(0x532A02);
    req = spr3E;
    req.x += x;
    req.y += y;
    px = req.x;
    y = req.y;
    draw_sprite_draw_request(&req);
    px += 57.0f;
    for (i = first; i < end; i++)
    {
        req = spr54;
        req.x = px;
        req.y = y;
        draw_sprite_draw_request(&req);
        if (i < 10)
        {
            textdraw_set_pos(3.0f + px, 4.0f + y);
            textdraw_put_char(i);
        }
        else
        {
            textdraw_set_pos(px - 2.0f, 4.0f + y);
            textdraw_put_char(i / 10);
            textdraw_set_pos(9.0f + (px - 2.0f), 4.0f + y);
            textdraw_put_char(i % 10);
        }
        px += 23.0f;
    }
    req = spr57;
    req.x = px;
    req.y = y;
    draw_sprite_draw_request(&req);
    px += 37.0f;
    req = spr41;
    req.x = px;
    req.y = y;
    draw_sprite_draw_request(&req);
    y += 23.0f;
    for (p = 0; p < modeCtrl.playerCount; p++)
    {
        u8 colors[4] = { 2, 1, 3, 0 };
        s16 total = lanes[p].u0;

        req = spr43[colors[balls[p].colorId]];
        req.x += x;
        req.y = y;
        draw_sprite_draw_request(&req);
        px = req.x;
        req = spr4F[p];
        req.x += px;
        req.y += y;
        draw_sprite_draw_request(&req);
        px += 57.0f;
        for (i = first; i < end; i++)
        {
            s8 pins = lanes[p].u4[i - 1];
            s8 mark;

            req = spr55;
            req.x = px;
            req.y = y;
            draw_sprite_draw_request(&req);
            if (modeCtrl.currPlayer == p && i == frame && (data_488 & 0x300) == 0)
            {
                req = spr53;
                req.x += px;
                req.y += y;
                req.opacity = 0.4 + 0.25 * math_sin((u16)(bss_4 * 0x400));
                draw_sprite_draw_request(&req);
            }
            mark = fn_5_A608(pins);
            if (mark != 0)
            {
                textdraw_set_pos(3.0f + px, 9.0f + y);
                textdraw_put_char(mark);
            }
            px += 23.0f;
        }
        req = spr58;
        req.x = px;
        req.y = y;
        draw_sprite_draw_request(&req);
        px += 4.0f;
        req = spr59;
        req.x = px;
        req.y = y;
        draw_sprite_draw_request(&req);
        if (total < 10)
        {
            textdraw_set_pos(9.0f + (px - 1.0f), 9.0f + y);
            textdraw_put_char(total);
        }
        else if (total < 100)
        {
            s16 tens = total / 10;

            total = total - tens * 10;
            textdraw_set_pos(5.0f + (px - 1.0f), 9.0f + y);
            textdraw_put_char(tens);
            textdraw_set_pos(14.0f + (px - 1.0f), 9.0f + y);
            textdraw_put_char(total);
        }
        else
        {
            s16 hundreds = total / 100;
            s16 tens = (total - hundreds * 100) / 10;

            total = total - hundreds * 100 - tens * 10;
            textdraw_set_pos(px - 1.0f, 9.0f + y);
            textdraw_put_char(hundreds);
            textdraw_set_pos(9.0f + (px - 1.0f), 9.0f + y);
            textdraw_put_char(tens);
            textdraw_set_pos(18.0f + (px - 1.0f), 9.0f + y);
            textdraw_put_char(total);
        }
        px += 33.0f;
        req = spr49;
        req.x = px;
        req.y = y;
        draw_sprite_draw_request(&req);
        y += 30.0f;
    }
    req = spr4A;
    req.x += x;
    req.y = y;
    draw_sprite_draw_request(&req);
    px = 57.0f + req.x;
    for (i = first; i < end; i++)
    {
        req = spr56;
        req.x = px;
        req.y = y;
        draw_sprite_draw_request(&req);
        px += 23.0f;
    }
    req = spr5A;
    req.x = px;
    req.y = y;
    draw_sprite_draw_request(&req);
    px += 37.0f;
    req = spr4D;
    req.x = px;
    req.y = y;
    draw_sprite_draw_request(&req);
}

s8 fn_5_A580(struct BowlPlayer *player, s8 index)
{
    switch (player->u35[index])
    {
    case 0:
        return 0;
    case 2:
        return 11;
    case 3:
        return 10;
    case 5:
        return 13;
    case 6:
        return 14;
    case 7:
        return 12;
    case 4:
        return -player->u20[index];
    case 1:
    default:
        return player->u20[index];
    }
}

s8 fn_5_A608(s8 pins)
{
    switch (pins)
    {
    case 0:
        return 0;
    case 10:
        return 11;
    case -1:
        return 13;
    }
#ifndef __MWERKS__
    /* pc_port: no return in the original; on GC r3 still holds pins. */
    return pins;
#endif
}

void g_bowl_drawFunc(struct Ape *ape, int n)
{
    struct Ball *ball = &balls[ape->ballIdx];
    struct RaycastHit hit;
    int r30;
    float speed;

    switch (n)
    {
    case 3:
        fn_1_D1F28(ape);
        return;
    default:
        break;
    }
    if (g_some_other_flags & 0xA)
        return;
    if (ape->ballIdx != modeCtrl.currPlayer)
        return;

    raycast_stage_down(&ball->pos, &hit, NULL);
    ape->flags &= -20;
    if (!(hit.flags & 1) && ball->vel.y < -0.16203702986240387f)
        ape->flags |= 2;
    else if (mathutil_vec_len(&ball->unkC0) < 0.00027777777f)
        ape->flags |= 1;

    r30 = !(ape->flags & 3);
    g_ape_upright(ape);
    if (r30)
    {
        speed = fn_1_45A8C(ape);
    }
    else
    {
        speed = 0.0f;
        mtxa_from_quat(&ape->rot);
        mtxa_normalize_basis();
        if (ape->flags & (1 << 1))
            fn_1_46958(ape);
    }

    if (ball->flags & 0x20)
        speed = mathutil_vec_len(&ball->vel);

    fn_5_C1D4(ape);
    mtxa_to_quat(&ape->rot);
    fn_5_A89C(ape, speed);
    fn_1_D2510(ape);
    if (!(ape->flags & (1 << 3)))
        g_move_ape_into_ball(ape);
    g_ape_anim_head(ape, &ball->unk10C);
    ball->unk108 = 0;
    ball->unk118 = 0.0f;
}

void fn_5_A89C(struct Ape *ape, float speed)
{
    if (data_488 & 0x804)
    {
        if (current_ball->vel.x > 0.0)
            fn_1_113190(ape, 0x10, speed, 0, 1);
        else if (current_ball->vel.x < 0.0)
            fn_1_113190(ape, 0x10, speed, 0, 2);
        else
            fn_1_113190(ape, 0x10, speed, 0, 0);
    }
    else if (data_488 & 0x1A)
        fn_1_113190(ape, 0x10, speed, 0, 0);
    else if (data_488 & 0x20)
    {
        if (bss_0 > 0x46)
            fn_1_113190(ape, 0x10, speed, 1, 4);
        else if (bss_0 > 1)
        {
            if (1.0f == bss_1C0)
            {
                fn_1_113190(ape, 0x10, speed, 1, 0);
                if (bss_0 == 0x46)
                    call_SoundReqID_arg_0(0x3B12D);
            }
            else if (bss_1C0 > 0.66)
            {
                fn_1_113190(ape, 0x10, speed, 1, 1);
                if (bss_0 == 0x46)
                    call_SoundReqID_arg_0(0x3B12D);
            }
            else if (bss_1C0 > 0.33)
            {
                fn_1_113190(ape, 0x10, speed, 1, 2);
                if (bss_0 == 0x46)
                    call_SoundReqID_arg_0(0x3B020);
            }
            else
            {
                fn_1_113190(ape, 0x10, speed, 1, 3);
                if (bss_0 == 0x46)
                    call_SoundReqID_arg_0(0x3B01F);
            }
        }
        else
            fn_1_113190(ape, 0, speed, 6, 0);
    }
    else if ((data_488 & 0x40) && bss_0 > 0x3DE)
        fn_1_113190(ape, 0, speed, 6, 0);
    else if (data_488 & 0x80)
    {
        struct BowlPlayer *player = &players[modeCtrl.currPlayer];

        switch (player->u35[player->u8])
        {
        case 2:
            if (player->u9 < 3)
                fn_1_113190(ape, 0, speed, 7, 4);
            else if (player->u9 < 12)
                fn_1_113190(ape, 0, speed, 7, 8);
            else
                fn_1_113190(ape, 0x10, speed, 2, 0);
            if (246.0 == bss_0 || 120.0 == bss_0)
                call_SoundReqID_arg_0(0x59);
            break;
        case 3:
            fn_1_113190(ape, 0, speed, 7, 2);
            if (246.0 == bss_0 || 120.0 == bss_0)
                call_SoundReqID_arg_0(0x58);
            break;
        case 5:
        case 6:
        case 7:
            fn_1_113190(ape, 0x10, speed, 3, 0);
            if ((246.0 == bss_0 || 120.0 == bss_0) && (current_ball->flags & 0x1000) != 0)
                call_SoundReqID_arg_0(0x1C);
            break;
        case 1:
        default:
            if (player->u20[player->u8] >= 6
             && (player->u7 == 1 || (player->u6 == 10 && player->u7 == 2 && player->u35[player->u8 - 1] == 2)))
            {
                fn_1_113190(ape, 0, speed, 7, 0);
                if (246.0 == bss_0 || 120.0 == bss_0)
                    call_SoundReqID_arg_0(0x1B);
            }
            else
            {
                fn_1_113190(ape, 0, speed, 8, 0);
                if (246.0 == bss_0 || 120.0 == bss_0)
                    call_SoundReqID_arg_0(0x1C);
            }
            break;
        }
    }
    else if (data_488 & 0x2000)
    {
        if (bss_1A6 == 0)
        {
            if ((u32)bss_1A8.u2 >= 9)
                fn_1_113190(ape, 0x10, speed, 2, 0);
            else
                fn_1_113190(ape, 0, speed, 7, 6);
            if (246.0 == bss_0 || 120.0 == bss_0)
                call_SoundReqID_arg_0(0x59);
        }
        else
        {
            if (bss_1A8.u4 < 1)
                fn_1_113190(ape, 0x10, speed, 3, 0);
            else
                fn_1_113190(ape, 0, speed, 8, 0);
            if ((246.0 == bss_0 || 120.0 == bss_0) && (current_ball->flags & 0x1000) != 0)
                call_SoundReqID_arg_0(0x1C);
        }
    }
    else if (data_488 & 0x10000)
    {
        struct BowlLane *lane = &lanes[modeCtrl.currPlayer];
        s8 pins = lane->u4[lane->u2 - 1];

        if (pins == 10)
        {
            if (lane->u3 < 3)
                fn_1_113190(ape, 0, speed, 7, 4);
            else if (lane->u3 < 20)
                fn_1_113190(ape, 0, speed, 7, 8);
            else
                fn_1_113190(ape, 0x10, speed, 2, 0);
            if (246.0 == bss_0 || 120.0 == bss_0)
                call_SoundReqID_arg_0(0x59);
        }
        else if (pins >= 6)
        {
            fn_1_113190(ape, 0, speed, 7, 0);
            if (246.0 == bss_0 || 120.0 == bss_0)
                call_SoundReqID_arg_0(0x1B);
        }
        else if (pins > 0)
        {
            fn_1_113190(ape, 0, speed, 8, 0);
            if (246.0 == bss_0 || 120.0 == bss_0)
                call_SoundReqID_arg_0(0x1C);
        }
        else
        {
            fn_1_113190(ape, 0x10, speed, 3, 0);
            if ((246.0 == bss_0 || 120.0 == bss_0) && (current_ball->flags & 0x1000) != 0)
                call_SoundReqID_arg_0(0x1C);
        }
    }
    else
    {
        fn_1_461C0(ape, speed);
        return;
    }
}

void fn_5_B30C(float x, float y, u32 color, char *str)
{
    textdraw_set_depth(0.11f);
    textdraw_set_pos(2.0f + x, 2.0f + y);
    textdraw_set_mul_color(0);
    textdraw_print(str);
    textdraw_set_depth(0.1f);
    textdraw_set_pos(x, y);
    textdraw_set_mul_color(color);
    textdraw_print(str);
}

void fn_5_B3C8(void)
{
    struct SpriteDrawRequest reqs[8];
    struct SpriteDrawRequest req;
    float xs[10] = { 36.0f, 156.0f, 276.0f, 396.0f, 516.0f, 36.0f, 156.0f, 276.0f, 396.0f, 516.0f };
    float ys[10] = { 120.0f, 120.0f, 120.0f, 120.0f, 120.0f, 214.0f, 214.0f, 214.0f, 214.0f, 214.0f };
    char buf[0x14];
    int i;
    int j;
    float t;
    u8 c;
    u32 color;
    int k;
    float width;
    char *str;
    float dx;
    float px;
    float dy;

    t = 0.8 + 0.2 * math_sin(1092.2666666666667 * bss_4);
    c = 255.0f * t;
    {
        int b = c;

        color = (c << 16) + (c << 8) + b;
    }
    reset_ui_widescreen_scale_mtx();
    draw_sprite_draw_request((struct SpriteDrawRequest *)&spr0);
    set_ui_widescreen_scale_mtx(0x140);
    for (i = 0; i < sizeof(xs) / sizeof(xs[0]); i++)
    {
        req = spr32;
        req.x += xs[i];
        req.y += ys[i];
        if (bss_1A8.u0 & (1 << i))
            req.mulColor = 0x606060;
        else if (bss_1A8.u3 == i)
            req.mulColor = color;
        draw_sprite_draw_request(&req);
        for (j = 0; j < sizeof(xs) / sizeof(xs[0]); j++)
        {
            if ((1 << j) & pinSplits[i].mask)
            {
                req = spr33[j];
                req.x += xs[i];
                req.y += ys[i];
                if (bss_1A8.u0 & (1 << i))
                    req.mulColor = 0x606060;
                else if (bss_1A8.u3 == i)
                    req.mulColor = color;
                draw_sprite_draw_request(&req);
            }
        }
    }
    for (k = 0; k < 8; k++)
    {
        reqs[k] = spr2A[k];
        reqs[k].x += xs[bss_1A8.u3] - 11.0f;
        reqs[k].y += ys[bss_1A8.u3] - 13.0f;
        reqs[k].opacity = t;
        reqs[k].mulColor = 0xFF0000 + (c << 8) + c;
        draw_sprite_draw_request(&reqs[k]);
    }
    set_ui_widescreen_scale_mtx(0x60);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    textdraw_set_depth(0.11f);
    textdraw_set_pos(18.0f, 330.0f);
    textdraw_set_mul_color(0);
    textdraw_printf("%s%d\n%s", txtPattern2[locale], bss_1A8.u3 + 1, pinSplits[bss_1A8.u3].names[locale]);
    textdraw_set_depth(0.1f);
    textdraw_set_pos(16.0f, 328.0f);
    textdraw_set_mul_color(0xF0C0C0);
    textdraw_printf("%s%d\n%s", txtPattern2[locale], bss_1A8.u3 + 1, pinSplits[bss_1A8.u3].names[locale]);
    set_ui_widescreen_scale_mtx(0x220);
    {
        int n = 10 - bss_1A8.u2;

        if (n == 1)
            sprintf(buf, "%s%d%s", txtEmpty2[locale], n, txtPattern[locale]);
        else
            sprintf(buf, "%s%d%s", txtEmpty2[locale], n, txtPatterns[locale]);
    }
    dx = 611.0f - g_get_string_sprite_width_2(buf);
    fn_5_B30C(dx, 328.0f, 0xF0C0C0, buf);
    set_ui_widescreen_scale_mtx(0x140);
    str = txtWhichWillYouChallenge[locale];
    width = g_get_string_sprite_width_2(str);
    draw_sprite_draw_request((struct SpriteDrawRequest *)&spr1);
    if (locale != 3)
    {
        float s = math_sin(bss_0 * (65536.0 / (2.0 * lbl_80198B38)));
        int rot = 2048.0f * s;
        s16 ang = math_atan(0.5f);

        px = math_sqrt(720.0f);
        dx = 12.0f - px * math_sin(rot + ang);
        dy = 24.0f - px * math_sin(rot + ang + 0x4000);
        px = 0.5f * (640.0f - width) - 16.0f;
        fn_5_B30C(px, 408.0f, 0xFFFF00, str);
        px += 8.0f + width;
        textdraw_set_rot_z((s16)rot);
        fn_5_B30C(px + dx, 408.0f + dy, 0xFFFF00, "?");
    }
    else
    {
        px = 0.5f * (640.0f - width);
        fn_5_B30C(px, 408.0f, 0xFFFF00, str);
    }
}

void fn_5_BC78(void)
{
    float speed = 216.0 * mathutil_vec_len(&current_ball->vel);

    if ((current_ball->flags & 1) != 0 && speed > 1.0)
    {
        if (bss_4 % 8 == 0)
        {
            int vol = MIN(1.5 * (speed - 1.0), 127.0);
            s8 pitch = 0.85 * MIN(15.0 * (speed - 1.0), 80.0);

            SoundIcsReq(current_ball->playerId, vol, pitch);
        }
    }
    else
        SoundIcsReq(current_ball->playerId, 0, 0);
}

void fn_5_BE18(s32 a, s32 b, int n)
{
    bss_1D0.a = a;
    bss_1D0.b = b;
    bss_1D0.timer = (s16)n;
    bss_1D0.trigger = n - 30.0;
    g_fade_track_volume(0, 8);
}

void fn_5_BEB4(void)
{
    if (bss_1D0.timer > 0)
    {
        if ((s16)bss_1D0.timer == bss_1D0.trigger)
            fn_1_3061C(bss_1D0.a, bss_1D0.b);
        bss_1D0.timer--;
        if (bss_1D0.timer <= 0)
            fn_5_BF50();
    }
}

void fn_5_BF50(void)
{
    bss_1D0.timer = 0;
    g_fade_track_volume(0x64, 8);
}

int fn_5_BF88(void)
{
    if (g_poolInfo.playerPool.statusList[0] != 0 && (pad_status_groups[player_pad_map[0]].unk0[2].button & 0x100) != 0)
        return 1;
    if (g_poolInfo.playerPool.statusList[1] != 0 && (pad_status_groups[player_pad_map[1]].unk0[2].button & 0x100) != 0)
        return 1;
    if (g_poolInfo.playerPool.statusList[2] != 0 && (pad_status_groups[player_pad_map[2]].unk0[2].button & 0x100) != 0)
        return 1;
    if (g_poolInfo.playerPool.statusList[3] != 0 && (pad_status_groups[player_pad_map[3]].unk0[2].button & 0x100) != 0)
        return 1;
    return 0;
}


void fn_5_C0D0(s8 a)
{
    cameras[modeCtrl.currPlayer].unk1F = a;
}

void fn_5_C0F4(void)
{
    struct StageCollHeader *hdr;
    struct ItemGroup *group;
    struct StageModelRef * DPTR *ref;
    int j;
    int i;

    group = itemgroups;
    hdr = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, group++, hdr++)
    {
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(group->transform);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        ref = hdr->modelRefs;
        for (j = 0; j < hdr->modelRefCount; j++, ref++)
        {
            if (i != 0 || j != 2)
            {
                struct GMAModel *model = g_some_draw_func2(*ref);

                if (model != NULL)
                    avdisp_draw_model_culled_sort_never(model);
            }
        }
    }
}

void fn_5_C1D4(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    Vec points[] =
    {
        {0.3f,   0.0f, 0.0f},
        {-0.25f, 0.0f, 0.0f},
        {0.1f,   0.0f, -0.2f},
        {0.1f,   0.0f, 0.2f},
    };
    Vec tmp;
    Vec v;
    int rot;
    float ground;
    u32 i;

    mtxa_push();
    tmp.x = 1.0f;
    tmp.y = 0.0f;
    tmp.z = 0.0f;
    mtxa_tf_vec(&tmp, &tmp);
    rot = -math_atan2(tmp.z, tmp.x) - 32768;
    ground = fn_5_16B64(&ball->pos);
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(rot);
    ball->flags &= ~2;
    fn_5_16B64(&v);
    for (i = 0; i < 4; i++)
    {
        mtxa_tf_point(&points[i], &tmp);
        if (fn_5_16B64(&tmp) < ground - 1.0)
        {
            ball->flags |= 2;
            v.x = tmp.x;
            v.y = ball->pos.y - 1.0;
            v.z = tmp.z;
            ape->unk27C = i;
            g_something_with_translating_items(3, &v, 0.5f);
            break;
        }
    }
    mtxa_pop();
}

int fn_5_C3C8(void)
{
    if (bss_0 < 0)
        return 1;
    if (fn_5_12A68() == 0)
        return 0;
    if (current_ball->pos.y < -20.0f)
        return 1;
    if ((current_ball->flags & BALL_FLAG_GOAL) == 0)
        return 0;
    return did_ball_fallout(current_ball) != 0;
}


void g_load_missing_apes(void)
{
    int apes[] = {0, 1, 2, 3};
    int i;

    for (i = 0; i < 4; i++)
    {
        if (ballid_matches_chara(apes[i]) != 0)
            preload_ape(apes[i], 0, 1, 0x1A);
    }
    DVDChangeDir("init");
    load_commend_stage();
    DVDChangeDir("..");
}

void g_load_commend_apes(void)
{
    int apes[] = {0, 1, 2, 3};
    int i;

    queue_stage_load(200);
    DVDChangeDir("bmp");
    queue_disc_read("bmp_rnk.tpl");
    DVDChangeDir("..");
    for (i = 0; i < 4; i++)
    {
        if (ballid_matches_chara(apes[i]) != 0)
            preload_ape(apes[i], 1, 0, 0);
    }
    DVDChangeDir("ape");
    queue_disc_read("aiai_main.mta");
    queue_disc_read("meemee_main.mta");
    queue_disc_read("baby_main.mta");
    queue_disc_read("gongon_main.mta");
    DVDChangeDir("..");
}

void g_load_selthing(void)
{
    g_load_stage_for_menu_bg(4, 1);
    DVDChangeDir("bmp");
    queue_disc_read("bmp_sel2.tpl");
    queue_disc_read("bmp_how.tpl");
    queue_disc_read("bmp_nml.tpl");
    DVDChangeDir("..");
    DVDChangeDir("snd/mkb");
    queue_disc_read("sel.pool");
    queue_disc_read("sel.proj");
    queue_disc_read("sel.sdir");
    queue_disc_read("sel.samp");
    DVDChangeDir("../..");
}

int ballid_matches_chara(int chara)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != 0 && chara == active_monkey_id[i])
            return 1;
    }
    return 0;
}

/* Keeps otherwise unused data in the build; stripped by the linker. */
void unreferenced_data(void)
{
    void *p;

    p = (void *)&spr6;
    p = (void *)&spr7;
    p = (void *)&sprC;
    p = (void *)&spr32;
    p = (void *)&spr41;
    p = (void *)&spr42;
    p = (void *)&spr4E;
    p = (void *)&spr5A;
    p = (void *)&spr60;
    data_0[0] = 0;
}
