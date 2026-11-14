/* Monkey Target 2: flow of a game, the players and the flying balls. */
#include "types.h"
#include "macros.h"
#include "avdisp.h"
#include "ball.h"
#include "background.h"
#include "camera.h"
#include "effect.h"
#include "event.h"
#include "frustum.h"
#include "input.h"
#include "item.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "pilot2.h"
#include "pool.h"
#include "rand.h"
#include "shadow.h"
#include "stage.h"
#include "stcoli.h"
#include "world.h"
#include "ape_internal.h"
#include "dip.h"
#ifndef __MWERKS__
/* pc_port: undeclared in the original; an implicit declaration passes float args as double on x86 */
int fn_1_A74E4(Vec *a, Vec *b, float ra, Vec *c, Vec *d, float rb);
#endif

struct Viewport
{
    float x;
    float y;
    float w;
    float h;
};

struct PilotSpawn
{
    Vec pos;
    s16 type;
    u16 pad;
};

extern void (*lbl_1_bss_D49B8)(void);
extern void (*lbl_1_bss_70E94)(void);
extern u32 lbl_1_bss_D4A10;
extern u32 g_some_other_flags;
extern float lbl_80198B34;
extern s32 active_monkey_id[];
extern u8 lbl_1_bss_CF22C[];
struct PilotSave { u8 unk0; u8 unk1; u32 unk4; };
extern struct PilotSave lbl_1_bss_16E8;

static inline struct PilotSave *get_pilot_save(void)
{
    return &lbl_1_bss_16E8;
}
extern struct { u32 unk0; u32 unk4; } g_related_to_pause_menu_input;
extern s32 g_player_id_for_sound;
extern s32 g_smth_with_sound;
extern float lbl_1_bss_70E70[8];

void g_set_minigame_specific_funcs(void (*tick)(void), void (*draw)(void), void *a, void *b);
void g_very_similar_to_puts(const char *);
u32 *OSGetStackPointer(void);
void dest_all_events(void);
void g_something_with_iteratively_freeing_memory(void);
void unload_stage(void);
void SoundGroupLoad(int);
void fn_1_358E8(void);
s16 g_SoftStreamStart_with_some_defaults(int track);
void fn_1_11C5E4(void);
void event_commend_dest(void);
void g_something_calls_sndPopGroup(void);
void g_load_minigame_gma_tpl(int idx);
void free_minigame_gma_tpl(void);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void debug_textdraw_set_color(u8 color);
void event_init(int id);
void event_ball_tick(void);
void g_SoftStreamStart_with_some_defaults_2(int id);
void load_stage(int stage);
void load_commend_stage(void);
BOOL SoundOffID(int soundId);
void g_fade_track_volume(int a, int b);
void event_freeze(int id);
void fn_1_590AC(int a);
void call_SoundReqID_arg_0(int soundId);
int call_SoundReqID_arg_1_copy(u32 soundId);
void SoundRev(u16 id, u8 val);
void SoundCho(u16 id, u8 val);
void PSVECScale(Vec *src, Vec *dst, float scale);
void PSVECAdd(Vec *a, Vec *b, Vec *c);
void PSVECSubtract(Vec *a, Vec *b, Vec *c);
void g_load_mal_files_from_disc(int chara, int lod, int mesh, int scene);
void g_something_freeing_chara_heap_4(int chara, int lod, int mesh);
void create_stage_player(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod, void (*draw)(struct Ape *), int mesh);
void create_player_num_and_ready_sprites(int frames);
void event_restart(int id);
void create_go_sprite(int frames);
int fn_1_11E1C0(int a, void *b);
void queue_stage_load(int stageId);
void fade_screen_to_color(int flags, u32 color, int len);
void set_camera_view_settings(int, float, float, float, float);

void fn_13_128(void);
void fn_13_290(void);
void fn_13_304(void);
void fn_13_444(void);
void fn_13_4C4(void);
void fn_13_4E8(void);
void fn_13_518(int mode);
void fn_13_1B98(void);
void fn_13_1D80(void);
void fn_13_1DEC(void);
void g_load_pilot(void);
void fn_13_28C8(void);
void fn_13_2934(int idx);
void fn_13_380C(void);
void fn_13_3810(int a);
void fn_13_3DDC(void);
s32 fn_13_3E1C(void);
void fn_13_48D0(void);
void fn_13_4990(struct Ball *ball);
void fn_13_8478(struct Ball *ball);
s32 fn_13_8678(void);
void fn_13_8728(void);
void fn_13_86EC(void);

void fn_13_6C8(void);
void fn_13_74C(void);
void fn_13_930(void);
void fn_13_BE8(void);
void fn_13_1288(void);
void fn_13_1364(void);
void fn_13_1530(void);
void fn_13_1F10(void);
void fn_13_1FFC(void);
void fn_13_2058(void);
void fn_13_2194(void);
void fn_13_2260(void);
void fn_13_24F0(void);
void fn_13_257C(void);
void fn_13_2A00(void);
void fn_13_2BFC(void);
void fn_13_2F74(void);
void fn_13_3054(void);
void fn_13_330C(void);
void fn_13_3458(void);
void fn_13_3538(void);
void fn_13_3740(void);

void fn_13_89DC(void);
void fn_13_8A44(void);
void fn_13_8A88(struct Camera *camera);
void fn_13_A88C(void);
void fn_13_B020(void);
void fn_13_B174(void);
void fn_13_D408(int player);
void fn_13_DCA0(void);
void fn_13_E1B4(int player);
void fn_13_F288(void);
void fn_13_11730(void);
void fn_13_11780(void);
void fn_13_11CD4(void);
void fn_13_12A6C(void);

int lbl_13_bss_0;
u8 *lbl_13_bss_4;
u8 *lbl_13_bss_8;
s16 lbl_13_bss_C;
s16 lbl_13_bss_E;
s16 lbl_13_bss_10;
s16 lbl_13_bss_12;
float lbl_13_bss_14;
Vec lbl_13_bss_18;
int lbl_13_bss_24;
s8 lbl_13_bss_28;
s16 lbl_13_bss_2A;
s16 lbl_13_bss_2C;
int lbl_13_bss_30[4];
int lbl_13_bss_40[4];
int lbl_13_bss_50[4];
s16 lbl_13_bss_60[4];
s16 lbl_13_bss_68[4];
struct Pilot2Player lbl_13_bss_70[5];
int lbl_13_bss_200[4];
s8 lbl_13_bss_210[4];
s8 lbl_13_bss_214;
s16 lbl_13_bss_216;
s16 lbl_13_bss_218[4];
int lbl_13_bss_220;
s8 lbl_13_bss_224[8];
int lbl_13_bss_22C;
int lbl_13_bss_230;
s8 lbl_13_bss_234[4];
int lbl_13_bss_238;
s16 lbl_13_bss_23C;
int lbl_13_bss_240;
int lbl_13_bss_244[4];
s16 lbl_13_bss_254[4];
s8 lbl_13_bss_25C;
Vec *lbl_13_bss_260[4];
Vec lbl_13_bss_270[5];
Vec lbl_13_bss_2AC;
s16 lbl_13_bss_2B8;
Vec lbl_13_bss_2BC;
Vec lbl_13_bss_2C8;
FORCE_BSS_ORDER(lbl_13_bss_0)
FORCE_BSS_ORDER(lbl_13_bss_4)
FORCE_BSS_ORDER(lbl_13_bss_8)
FORCE_BSS_ORDER(lbl_13_bss_C)
FORCE_BSS_ORDER(lbl_13_bss_E)
FORCE_BSS_ORDER(lbl_13_bss_10)
FORCE_BSS_ORDER(lbl_13_bss_12)
FORCE_BSS_ORDER(lbl_13_bss_14)
FORCE_BSS_ORDER(lbl_13_bss_18)

u32 lbl_13_data_0[] =
{
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

s16 lbl_13_data_170[] = {0x9B, 0x9C, 0x9D};

struct Viewport lbl_13_data_178[4] =
{
    {0.0f, 0.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

struct Viewport lbl_13_data_1B8[4] =
{
    {0.0f, 0.0f, 0.5f, 0.5f},
    {0.5f, 0.0f, 0.5f, 0.5f},
    {0.0f, 0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f, 0.5f},
};

struct Viewport lbl_13_data_1F8[4] =
{
    {0.0f, 0.0f, 1.0f, 0.5f},
    {0.0f, 0.5f, 1.0f, 0.5f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

struct Viewport lbl_13_data_238[4] =
{
    {0.0f, 0.0f, 0.5f, 1.0f},
    {0.5f, 0.0f, 0.5f, 1.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

struct Viewport *lbl_13_data_278[] = {lbl_13_data_1F8, lbl_13_data_238, lbl_13_data_238};

struct Viewport lbl_13_data_284[4] =
{
    {0.0f, 0.0f, 1.0f, 0.5f},
    {0.0f, 0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f, 0.5f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

struct Viewport lbl_13_data_2C4[4] =
{
    {0.0f, 0.5f, 0.5f, 0.5f},
    {0.0f, 0.0f, 1.0f, 0.5f},
    {0.5f, 0.5f, 0.5f, 0.5f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

struct Viewport lbl_13_data_304[4] =
{
    {0.0f, 0.0f, 0.5f, 0.5f},
    {0.5f, 0.0f, 0.5f, 0.5f},
    {0.0f, 0.5f, 1.0f, 0.5f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

struct Viewport *lbl_13_data_344[] = {lbl_13_data_284, lbl_13_data_2C4, lbl_13_data_304, lbl_13_data_1B8};

struct Viewport lbl_13_data_354[4] =
{
    {0.0f, 0.0f, 1.0f, 1.0f},
    {0.66f, 0.66f, 0.32f, 0.32f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

struct Viewport lbl_13_data_394[4] =
{
    {0.0f, 0.0f, 1.0f, 0.5f},
    {0.0f, 0.5f, 1.0f, 0.5f},
    {0.7f, 0.2f, 0.28f, 0.28f},
    {0.7f, 0.7f, 0.28f, 0.28f},
};

struct Viewport lbl_13_data_3D4[4] =
{
    {0.0f, 0.0f, 0.5f, 1.0f},
    {0.5f, 0.0f, 0.5f, 1.0f},
    {0.31f, 0.6f, 0.17f, 0.28f},
    {0.81f, 0.6f, 0.17f, 0.28f},
};


void (*lbl_13_data_414[])(void) =
{
    fn_13_6C8, fn_13_74C, fn_13_930, fn_13_BE8, fn_13_1288, fn_13_1364, fn_13_1530, fn_13_1F10,
    fn_13_1FFC, fn_13_2058, fn_13_2194, fn_13_2260, fn_13_24F0, fn_13_257C, fn_13_2A00, fn_13_2BFC,
    fn_13_2F74, fn_13_3054, fn_13_330C, fn_13_3458, fn_13_3538, fn_13_3740,
};

void fn_13_49D0(struct Ball *ball);
void fn_13_4ECC(struct Ball *ball);
void fn_13_4F18(struct Ball *ball);
void fn_13_5174(struct Ball *ball);
void fn_13_55C8(struct Ball *ball);
void fn_13_565C(struct Ball *ball);
void fn_13_57C0(struct Ball *ball);
void fn_13_584C(struct Ball *ball);
void fn_13_5AC0(struct Ball *ball);
void fn_13_5B2C(struct Ball *ball);
void fn_13_5C78(struct Ball *ball);
void fn_13_5E68(struct Ball *ball);
void fn_13_60A8(struct Ball *ball);
void fn_13_618C(struct Ape *ape, int arg1);
void fn_13_6524(struct Ape *ape, float speed);
void fn_13_7890(struct Ball *ball, struct PhysicsBall *physBall);
void fn_13_84CC(struct Ball *ball);
void fn_13_5334(struct Ball *ball);
void fn_13_6910(struct Ball *ball);
void fn_13_71B0(struct Ball *ball);
void fn_13_7234(struct Ball *ball, struct PhysicsBall *physBall);
void fn_13_762C(struct Ball *ball, struct PhysicsBall *physBall);
void fn_13_7A60(struct Ball *ball, struct PhysicsBall *physBall);
void fn_13_7B70(struct Ball *ball, struct PhysicsBall *physBall);
void fn_13_7C9C(struct PhysicsBall *physBall, struct StageDef *stagedef);
void fn_13_7CBC(void);
void fn_13_84CC(struct Ball *ball);
void fn_13_8258(Vec *n, Vec *a, float m1, Vec *b, float m2);
void SoundPitch(u16 id, u16 pitch);
void fn_13_83EC(struct Ball *ball);
float vec_normalize_len(Vec *v);
extern Vec lbl_13_bss_338;
s8 fn_1_2F8F0(int a, Vec *pos, s8 *pan, s8 *span);
void fn_1_113190(struct Ape *ape, int a, float speed, int b, int c);
void fn_1_D1F28(struct Ape *ape);
void fn_1_45CA8(struct Ape *ape);
void fn_13_55C8(struct Ball *ball);
void fn_13_565C(struct Ball *ball);
void fn_13_57C0(struct Ball *ball);
void fn_13_584C(struct Ball *ball);
void fn_13_5AC0(struct Ball *ball);
void fn_13_5B2C(struct Ball *ball);
void fn_13_5C78(struct Ball *ball);
void fn_13_5E68(struct Ball *ball);

void (*lbl_13_data_46C[])(struct Ball *) =
{
    fn_13_49D0, fn_13_4ECC, fn_13_4F18, fn_13_5174, fn_13_5334, fn_13_55C8, fn_13_565C, fn_13_57C0,
    fn_13_584C, fn_13_5AC0, fn_13_5B2C, fn_13_5C78, fn_13_5E68,
};

Vec lbl_13_data_4A0[] =
{
    {-0.75f, 0.0f, 0.0f},
    {0.75f, 0.0f, 0.0f},
};

Vec lbl_13_data_4B8[] =
{
    {-0.9f, 0.0f, 0.0f},
    {0.9f, 0.0f, 0.0f},
};

Vec lbl_13_data_4D0[] =
{
    {-1.5f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {1.5f, 0.0f, 0.0f},
};

Vec lbl_13_data_4F4[] =
{
    {-2.25f, 0.0f, 0.0f},
    {-0.75f, 0.0f, 0.0f},
    {0.75f, 0.0f, 0.0f},
    {2.25f, 0.0f, 0.0f},
};

Vec lbl_13_data_524[] =
{
    {0.0f, 0.0f, 0.0f},
    {-1.5f, 0.1f, 0.7f},
    {1.5f, 0.1f, 0.7f},
    {-3.0f, 0.2f, 1.4f},
    {3.0f, 0.2f, 1.4f},
};

Vec lbl_13_data_560[] =
{
    {0.0f, 0.0f, 0.0f},
    {-0.85f, 0.1f, 0.95f},
    {0.85f, 0.1f, 0.95f},
    {-1.7f, 0.2f, 1.9f},
    {1.7f, 0.2f, 1.9f},
};

Vec lbl_13_data_59C[] =
{
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.5f},
    {0.0f, 0.0f, -1.5f},
    {0.0f, 0.0f, 3.0f},
    {0.0f, 0.0f, -3.0f},
};

Vec lbl_13_data_5D8[] =
{
    {0.0f, 0.0f, 0.0f},
    {0.0f, 1.5f, 0.0f},
    {0.0f, -1.5f, 0.0f},
    {0.0f, 3.0f, 0.0f},
    {0.0f, -3.0f, 0.0f},
};

Vec lbl_13_data_614[] =
{
    {0.0f, 0.0f, 0.0f},
    {2.0f, 0.0f, 0.0f},
};

Vec *lbl_13_data_62C[] = {lbl_13_data_560, lbl_13_data_524, lbl_13_data_59C, lbl_13_data_5D8, lbl_13_data_614};

const s16 lbl_13_rodata_0[] = {0x3A, 0x3B, 0x3F, 0x53, 0x54, 0x55, 0x56, 0x57};
const s16 lbl_13_rodata_10[] = {0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208, 0x209, 0x20A, 0x20B, 0x20C, 0x20D, 0x20E};

extern s16 lbl_13_bss_378[];
extern s16 lbl_13_bss_36C[][1];
extern s16 lbl_13_bss_34C[][3];

void _prolog(void)
{
    g_set_minigame_specific_funcs(fn_13_304, fn_13_3DDC, fn_13_4990, fn_13_8A88);
    g_very_similar_to_puts("RELOCATION PILOT2 LOADED");
    fn_13_128();
}

void _epilog(void)
{
    fn_13_290();
    g_very_similar_to_puts("RELOCATION PILOT2 PURGED");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    sp = OSGetStackPointer();
    while (sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 16)
    {
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
        sp = (u32 *)sp[0];
    }
    OSPanic("pilot2.c", 0x198, "\n");
}

struct PilotSpawn lbl_13_data_6F4[] =
{
    {{5.8f, 54.0f, 286.0f}, 0, 0x0},
    {{-5.8f, 54.0f, 286.0f}, 0, 0x0},
    {{0.0f, 8e+01f, 2e+02f}, 2, 0x0},
    {{0.0f, 75.0f, 1.6e+02f}, 2, 0x0},
    {{0.0f, 65.0f, 1.2e+02f}, 2, 0x0},
    {{0.0f, 107.5937f, 29.18009f}, 2, 0x0},
    {{0.0f, 108.6562f, 7.100487f}, 2, 0x0},
    {{0.0f, 109.7512f, -18.13877f}, 2, 0x0},
    {{0.0f, 110.5605f, -43.3886f}, 2, 0x0},
    {{0.0f, 110.855f, -68.64919f}, 2, 0x0},
    {{0.0f, 110.4852f, -93.9087f}, 2, 0x0},
    {{0.0f, 109.5073f, -119.1522f}, 2, 0x0},
    {{0.0f, 108.0495f, -144.3727f}, 2, 0x0},
    {{0.0f, 106.001f, -169.5515f}, 2, 0x0},
    {{0.0f, 103.2142f, -194.6595f}, 2, 0x0},
    {{0.0f, 99.81624f, -219.6925f}, 2, 0x0},
    {{0.0f, 95.9297f, -244.6545f}, 2, 0x0},
    {{0.0f, 91.6085f, -269.5447f}, 2, 0x0},
    {{0.0f, 86.5f, -296.509f}, 6, 0x0},
    {{0.0f, 68.31971f, 32.5537f}, 2, 0x0},
    {{0.541798f, 72.84176f, 11.60208f}, 2, 0x0},
    {{0.256536f, 76.28011f, -9.527932f}, 2, 0x0},
    {{0.0f, 77.48203f, -30.84716f}, 6, 0x0},
    {{-8.301256f, 77.48203f, -30.84716f}, 2, 0x0},
    {{-11.75644f, 75.5571f, -52.04737f}, 2, 0x0},
    {{-15.53655f, 71.1351f, -72.82547f}, 2, 0x0},
    {{-19.55349f, 65.26956f, -93.20366f}, 2, 0x0},
    {{-23.7488f, 58.79947f, -113.3638f}, 2, 0x0},
    {{-27.8609f, 52.30811f, -133.5343f}, 2, 0x0},
    {{-31.49347f, 46.21225f, -153.9177f}, 2, 0x0},
    {{-34.31579f, 40.8601f, -174.6334f}, 2, 0x0},
    {{-36.11445f, 36.45842f, -195.6841f}, 2, 0x0},
    {{-36.87588f, 32.87568f, -216.9527f}, 2, 0x0},
    {{-36.55728f, 29.96179f, -238.3344f}, 2, 0x0},
    {{-35.00808f, 27.62819f, -259.7328f}, 2, 0x0},
    {{-32.06483f, 25.83089f, -281.0363f}, 2, 0x0},
    {{-28.07143f, 24.5362f, -302.2074f}, 2, 0x0},
    {{-23.70468f, 23.68552f, -323.3287f}, 2, 0x0},
    {{8.301256f, 77.48203f, -30.84716f}, 2, 0x0},
    {{11.75644f, 75.5571f, -52.04737f}, 2, 0x0},
    {{15.53655f, 71.1351f, -72.82547f}, 2, 0x0},
    {{19.55349f, 65.26956f, -93.20366f}, 2, 0x0},
    {{23.7488f, 58.79947f, -113.3638f}, 2, 0x0},
    {{27.8609f, 52.30811f, -133.5343f}, 2, 0x0},
    {{31.49347f, 46.21225f, -153.9177f}, 2, 0x0},
    {{34.31579f, 40.8601f, -174.6334f}, 2, 0x0},
    {{36.11445f, 36.45842f, -195.6841f}, 2, 0x0},
    {{36.87588f, 32.87568f, -216.9527f}, 2, 0x0},
    {{36.55728f, 29.96179f, -238.3344f}, 2, 0x0},
    {{35.00808f, 27.62819f, -259.7328f}, 2, 0x0},
    {{32.06483f, 25.83089f, -281.0363f}, 2, 0x0},
    {{28.07143f, 24.5362f, -302.2074f}, 2, 0x0},
    {{23.70468f, 23.68552f, -323.3287f}, 2, 0x0},
    {{0.0f, 0.0f, 0.0f}, -1, 0x0},
};

struct PilotSpawn lbl_13_data_A54[] =
{
    {{3e+01f, 3e+01f, -4.5e+02f}, 6, 0x0},
    {{-3e+01f, 3e+01f, -4.5e+02f}, 6, 0x0},
    {{0.0f, 4e+01f, -4e+02f}, 7, 0x0},
    {{0.0f, 6e+01f, -5e+02f}, 6, 0x0},
    {{0.0f, 8e+01f, -5e+02f}, 8, 0x0},
    {{0.0f, 3e+01f, -1e+03f}, 6, 0x0},
    {{0.0f, 59.17567f, -672.3141f}, 2, 0x0},
    {{0.0f, 60.47882f, -704.647f}, 2, 0x0},
    {{0.0f, 60.46866f, -737.0015f}, 2, 0x0},
    {{0.0f, 57.72766f, -769.2299f}, 2, 0x0},
    {{0.0f, 52.04792f, -801.08f}, 2, 0x0},
    {{0.0f, 44.830353f, -832.62354f}, 2, 0x0},
    {{0.0f, 32.6235f, -1192.582f}, 2, 0x0},
    {{0.0f, 31.87532f, -1228.679f}, 2, 0x0},
    {{0.0f, 30.62992f, -1264.762f}, 2, 0x0},
    {{0.0f, 28.44606f, -1300.8f}, 2, 0x0},
    {{-3e+01f, 6e+01f, -1.4e+03f}, 8, 0x0},
    {{3e+01f, 6e+01f, -1.4e+03f}, 8, 0x0},
    {{0.0f, 0.0f, 0.0f}, -1, 0x0},
};

struct PilotSpawn lbl_13_data_B84[] =
{
    {{7e+01f, 3e+01f, -4.6e+02f}, 6, 0x0},
    {{6e+01f, 3e+01f, -4.7e+02f}, 6, 0x0},
    {{-7e+01f, 3e+01f, -4.6e+02f}, 6, 0x0},
    {{-6e+01f, 3e+01f, -4.7e+02f}, 6, 0x0},
    {{0.0f, 6e+01f, -3.7e+02f}, 7, 0x0},
    {{0.0f, 9e+01f, -3.7e+02f}, 8, 0x0},
    {{0.0f, 0.0f, 0.0f}, -1, 0x0},
};

struct PilotSpawn lbl_13_data_BF4[] =
{
    {{3e+01f, 3e+01f, -4.7e+02f}, 6, 0x0},
    {{-3e+01f, 3e+01f, -4.7e+02f}, 6, 0x0},
    {{3e+01f, 3e+01f, -5.3e+02f}, 6, 0x0},
    {{-3e+01f, 3e+01f, -5.3e+02f}, 6, 0x0},
    {{0.0f, 4e+01f, -4e+02f}, 7, 0x0},
    {{0.0f, 2e+01f, -5e+02f}, 6, 0x0},
    {{0.0f, 35.0f, -5e+02f}, 6, 0x0},
    {{0.0f, 5e+01f, -5e+02f}, 6, 0x0},
    {{0.0f, 7e+01f, -5e+02f}, 8, 0x0},
    {{0.0f, 0.0f, 0.0f}, -1, 0x0},
};

void fn_13_128(void)
{
    struct PilotSave *save = get_pilot_save();

    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    unload_stage();
    SoundGroupLoad(0xE);
    fn_1_358E8();
    g_SoftStreamStart_with_some_defaults(0x24);
    g_SoftStreamStart_with_some_defaults(0x25);
    g_SoftStreamStart_with_some_defaults(0x26);
    g_SoftStreamStart_with_some_defaults(0x27);
    load_bmp_by_id_child(0xB);
    lbl_13_bss_4 = alloc_from_heap_or_panic(currentHeap, 0x4830, "pilot2.c", 0x1B1);
    lbl_13_bss_8 = alloc_from_heap_or_panic(currentHeap, 0x12030, "pilot2.c", 0x1B2);
    fn_13_4C4();
    lbl_1_bss_D49B8 = fn_13_12A6C;
    lbl_13_bss_C = (s8)save->unk0;
    sub_mode_request = 0xC0;
    if (pad_status_groups[0].unk0[0].button & 0x10)
    {
        lbl_1_bss_D4A10 = 0;
        lbl_13_bss_E = 0;
    }
    else
    {
        struct PilotSave *save2 = get_pilot_save();

        lbl_1_bss_D4A10 = save2->unk4;
        lbl_13_bss_10 = (s8)save2->unk1;
        lbl_13_bss_E = 2;
    }
    fn_13_444();
}

void fn_13_290(void)
{
    fn_13_1DEC();
    dest_all_events();
    fn_1_11C5E4();
    event_commend_dest();
    fn_13_4E8();
    OSFreeToHeap(currentHeap, lbl_13_bss_8);
    OSFreeToHeap(currentHeap, lbl_13_bss_4);
    free_bmp_by_id(0xB);
    g_something_calls_sndPopGroup();
}

void fn_13_304(void)
{
    if (!(g_some_other_flags & 0xA))
    {
        fn_13_444();
        fn_13_11780();
        fn_13_8728();
        if (dip_switches & 1)
        {
            struct Ball *ball = current_ball;

            if ((merged_digital_inputs[0] & 0xD00) == 0xD00 && lbl_13_bss_12 < 0xE)
            {
                lbl_13_bss_E = 0xE;
                return;
            }
            if ((pad_status_groups[player_pad_map[ball->playerId]].unk0[0].button & 0x20)
             && (pad_status_groups[player_pad_map[ball->playerId]].unk0[2].button & 0x100)
             && lbl_13_bss_12 < 0xE)
                lbl_13_bss_E = 4;
        }
    }
}

void fn_13_444(void)
{
    if (lbl_13_bss_E != -1)
    {
        lbl_13_bss_12 = lbl_13_bss_E;
        lbl_13_bss_E = -1;
    }
    lbl_13_data_414[lbl_13_bss_12]();
}

void fn_13_4C4(void)
{
    g_load_minigame_gma_tpl(3);
}

void fn_13_4E8(void)
{
    free_minigame_gma_tpl();
    lbl_1_bss_D49B8 = NULL;
}

static void init_wind(void)
{
    Vec v = {0.0f, 0.0f, -1.0f};
    s16 angle = (rand() & 0x7FFF) * 2;

    lbl_13_bss_14 = (1.0f / lbl_80198B34) + ((3.0f / lbl_80198B34) * (rand() / 32767.0f));
    mtxa_from_identity();
    mtxa_rotate_y(angle);
    mtxa_scale_s(lbl_13_bss_14);
    mtxa_tf_vec(&v, &lbl_13_bss_18);
}

void fn_13_518(int mode)
{
    int i;
    struct Viewport *p;

    lbl_13_bss_24 = mode;
    switch (lbl_13_bss_24)
    {
    case 0:
        p = lbl_13_data_178;
        break;
    case 1:
    case 6:
        p = lbl_13_data_278[modeCtrl.wideMode];
        break;
    case 2:
        p = lbl_13_data_344[modeCtrl.splitscreenMode];
        break;
    default:
        p = lbl_13_data_1B8;
        break;
    case 4:
        p = lbl_13_data_354;
        break;
    case 5:
        p = lbl_13_data_394;
        break;
    case 7:
        p = lbl_13_data_3D4;
        break;
    }
    for (i = 0; i < 4; i++, p++)
    {
        set_camera_view_settings(i, p->x, p->y, p->w, p->h);
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f)
            enable_camera(i);
    }
    if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == 3)
    {
        cameras[3].mode = 0x43;
        cameras[3].unk1F = 7;
        cameras[3].flags |= 0x60;
    }
}

void fn_13_6C8(void)
{
    lbl_13_bss_24 = 0;
    camera_setup_singleplayer_viewport();
    lbl_13_bss_10 = 1;
    lbl_13_bss_E = 1;
    fn_13_444();
}

void fn_13_74C(void)
{
    int max;

    debug_textdraw_set_pos(0, 0);
    debug_textdraw_vsprintf_buf_1("MONKEY TARGET2\n");
    debug_textdraw_set_pos(1, 1);
    debug_textdraw_vsprintf_buf_1("SELECTOR\n");
    debug_textdraw_set_pos(1, 5);
    debug_textdraw_set_color(0x57);
    debug_textdraw_vsprintf_buf_1("%d PLAYER(S)\n", modeCtrl.playerCount);
    debug_textdraw_vsprintf_buf_1("%d PLAYING PLAYER(S)\n", modeCtrl.unk30);
    debug_textdraw_set_pos(1, 8);
    debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("%d BALL NUM\n", lbl_13_bss_10);
    debug_textdraw_set_pos(1, 0x14);
    debug_textdraw_vsprintf_buf_1("PRESS A BUTTON\n");
    switch (modeCtrl.playerCount)
    {
    case 1:
        max = 5;
        break;
    default:
        max = 1;
        break;
    }
    if (pad_status_groups[0].unk0[2].button & 1)
    {
        lbl_13_bss_10 -= 1;
        if (lbl_13_bss_10 <= 0)
            lbl_13_bss_10 = max;
    }
    else if (pad_status_groups[0].unk0[2].button & 2)
    {
        lbl_13_bss_10 += 1;
        if (lbl_13_bss_10 > max)
            lbl_13_bss_10 = 1;
    }
    if (pad_status_groups[0].unk0[2].button & 0x100)
        lbl_13_bss_E = 2;
}

void fn_13_930(void)
{
    int i;
    int j;

    if (lbl_13_bss_10 > 1)
        lbl_1_bss_D4A10 |= 1;
    lbl_13_bss_28 = lbl_13_bss_10 * modeCtrl.unk30;
    switch (modeCtrl.unk30)
    {
    case 1:
        lbl_13_bss_24 = 0;
        break;
    case 2:
        if (modeCtrl.wideMode)
            lbl_13_bss_24 = 6;
        else
            lbl_13_bss_24 = 1;
        break;
    case 3:
        lbl_13_bss_24 = 2;
        break;
    default:
        lbl_13_bss_24 = 3;
        break;
    }
    lbl_13_bss_2A = 1;
    lbl_13_bss_2C = 0;
    for (i = 0; i < 4; i++)
    {
        lbl_13_bss_30[i] = 0;
        lbl_13_bss_40[i] = 0;
        lbl_13_bss_50[i] = 0;
        lbl_13_bss_60[i] = lbl_13_bss_68[i] = i;
    }
    for (i = 0; i < 5; i++)
    {
        lbl_13_bss_378[i] = 0;
        for (j = 0; j < 1; j++)
            lbl_13_bss_36C[i][j] = 0;
        for (j = 0; j < 3; j++)
            lbl_13_bss_34C[i][j] = 0;
    }
    fn_13_A88C();
    fn_13_F288();
    event_init(2);
    {
        struct Ball *ball;
        struct Ball *oldBall;
        int k;

        oldBall = current_ball;
        ball = balls;
        for (k = 0; k < 8; k++, ball++)
        {
            if (ball->status == 2)
            {
                current_ball = ball;
                ball->state = 1;
            }
        }
        current_ball = oldBall;
    }
    event_ball_tick();
    fn_13_1B98();
    fn_13_1D80();
    g_SoftStreamStart_with_some_defaults_2(0x3C);
    lbl_13_bss_E = 3;
    fn_13_444();
}

void fn_13_BE8(void)
{
    int i;

    dest_all_events();
    lbl_1_bss_CF22C[0xF] = 0;
    load_stage(lbl_13_data_170[lbl_13_bss_2C]);
    if (lbl_13_bss_2A < lbl_13_bss_C)
        queue_stage_load(lbl_13_data_170[(u32)(lbl_13_bss_2C + 1) % 3]);
    else
    {
        load_commend_stage();
        fn_13_89DC();
        if (modeCtrl.unk30 == 1)
            fn_13_8A44();
    }
    for (i = 0; i < 5; i++)
    {
        memset(&lbl_13_bss_70[i], 0, sizeof(lbl_13_bss_70[i]));
        lbl_13_bss_70[i].unk3C = 1.0f;
    }
    for (i = 0; i < 4; i++)
    {
        lbl_13_bss_40[i] = lbl_13_bss_30[i];
        lbl_13_bss_50[i] = 0;
        lbl_13_bss_200[i] = 0;
        lbl_13_bss_68[i] = lbl_13_bss_60[i];
        lbl_13_bss_210[i] = 0;
    }
    lbl_1_bss_D4A10 &= ~8;
    switch (lbl_13_bss_24)
    {
    case 6:
        break;
    case 4:
        fn_13_518(0);
        break;
    case 5:
        fn_13_518(1);
        break;
    case 7:
        fn_13_518(6);
        break;
    }
    fn_13_11730();
    fn_13_86EC();
    switch (current_stage_id)
    {
    case 0x9B:
    case 0x9C:
    case 0x9D:
        lbl_13_bss_214 = 1;
        break;
    default:
        lbl_13_bss_214 = 0;
        break;
    }
    lbl_13_bss_216 = 4;
    fade_screen_to_color(0x100, 0, 0.5f * lbl_80198B34);
    lbl_13_bss_E = 6;
    fn_13_444();
}

void fn_13_1288(void)
{
    modeCtrl.unk18 = 0.5 * lbl_80198B34;
    fade_screen_to_color(0x101, 0, modeCtrl.unk18);
    lbl_13_bss_E = 5;
    fn_13_444();
}

void fn_13_1364(void)
{
    int i;
    int j;

    if (--modeCtrl.unk18 <= 0)
    {
        for (i = 0; i < 4; i++)
        {
            lbl_13_bss_30[i] += lbl_13_bss_50[i];
            lbl_13_bss_50[i] = 0;
            lbl_13_bss_218[i] = 0;
        }
        for (i = 0; i < 5; i++)
        {
            for (j = 0; j < 1; j++)
                lbl_13_bss_36C[i][j] = 0;
            for (j = 0; j < 3; j++)
                lbl_13_bss_34C[i][j] = 0;
        }
        SoundOffID(0xEF);
        SoundOffID(0xF9);
        SoundOffID(0xF3);
        g_fade_track_volume(0x64, 8);
        if (lbl_13_bss_2A >= lbl_13_bss_C)
        {
            lbl_13_bss_E = 0xE;
            return;
        }
        if ((u32)++lbl_13_bss_2C >= 3)
            lbl_13_bss_2C = 0;
        lbl_13_bss_2A++;
        lbl_13_bss_E = 3;
        fn_13_444();
    }
}

void fn_13_1530(void)
{
    struct Item item;
    struct PilotSpawn *tbl;
    int i;
    int count;

    event_init(0);
    event_init(1);
    event_init(2);
    event_init(3);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0xB);
    lbl_1_bss_243C = 0;
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    if (modeCtrl.unk30 == 1)
    {
        switch (current_stage_id)
        {
        default:
            event_init(0x15);
        case 0x9A:
            break;
        }
    }
    event_init(0x14);
    fn_13_518(lbl_13_bss_24);
    event_freeze(1);
    if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == 3)
        count = 4;
    else
        count = modeCtrl.unk30;
    fn_1_590AC(count);
    g_load_pilot();
    fn_13_B020();
    init_wind();
    lbl_13_bss_338 = lbl_13_bss_18;
    memset(&item, 0, sizeof(item));
    item.type = 5;
    item.animGroupId = 0;
    tbl = lbl_13_data_6F4;
    for (i = 0; tbl[i].type != -1; i++)
    {
        item.pos = tbl[i].pos;
        item.subType = tbl[i].type;
        if (!(lbl_1_bss_D4A10 & 2))
        {
            switch (item.subType)
            {
            case 1:
            default:
                continue;
            case 0:
            case 2:
                break;
            }
        }
        spawn_item(&item);
    }
    switch (current_stage_id)
    {
    case 0x9B:
        tbl = lbl_13_data_A54;
        break;
    case 0x9C:
        tbl = lbl_13_data_B84;
        break;
    case 0x9D:
        tbl = lbl_13_data_BF4;
        break;
    default:
        tbl = NULL;
        break;
    }
    if (tbl != NULL)
    {
        for (i = 0; tbl[i].type != -1; i++)
        {
            item.pos = tbl[i].pos;
            item.subType = tbl[i].type;
            if (!(lbl_1_bss_D4A10 & 2))
            {
                switch (item.subType)
                {
                case 1:
                default:
                    continue;
                case 0:
                case 2:
                    break;
                }
            }
            spawn_item(&item);
        }
    }
    lbl_1_bss_70E94 = fn_13_48D0;
    {
        struct Ball *ball;
        struct Ball *oldBall;
        int k;

        oldBall = current_ball;
        ball = balls;
        for (k = 0; k < 8; k++, ball++)
        {
            if (ball->status == 2)
            {
                current_ball = ball;
                ball->state = 0x17;
                ball->unk154 = 0;
            }
        }
        current_ball = oldBall;
    }
    event_ball_tick();
    {
        struct World *world;
        s8 *status;
        int k;

        world = world_infos;
        status = g_poolInfo.playerPool.statusList;
        for (k = 0; k < g_poolInfo.playerPool.count; k++, world++, status++)
        {
            if (*status == 2)
                world->state = 1;
        }
    }
    camera_set_state(0x43);
    if (lbl_13_bss_2A >= lbl_13_bss_C)
        call_SoundReqID_arg_0(0x20F);
    else
        call_SoundReqID_arg_0(lbl_13_rodata_10[lbl_13_bss_2A - 1]);
    lbl_13_bss_220 = 0;
    lbl_13_bss_E = 7;
    fn_13_444();
}

void fn_13_1B98(void)
{
    int i;

    if (lbl_1_bss_D4A10 & 1)
    {
        switch (modeCtrl.unk30)
        {
        case 1:
            for (i = 0; i < lbl_13_bss_28; i++)
                lbl_13_bss_224[i] = (active_monkey_id[0] + i) % 4;
            break;
        case 2:
            for (i = 0; i < lbl_13_bss_28; i++)
                lbl_13_bss_224[i * 2] = (active_monkey_id[0] + i) % 4;
            for (i = 0; i < lbl_13_bss_28; i++)
                lbl_13_bss_224[i * 2 + 1] = (active_monkey_id[1] + i) % 4;
            break;
        }
    }
    else
    {
        for (i = 0; i < lbl_13_bss_28; i++)
            lbl_13_bss_224[i] = active_monkey_id[i];
    }
    if (lbl_13_bss_28 >= 4)
    {
        lbl_13_bss_22C = 1;
        lbl_13_bss_230 = 3;
        return;
    }
    lbl_13_bss_22C = 0;
    if (lbl_13_bss_28 >= 3)
    {
        lbl_13_bss_230 = 2;
        return;
    }
    if (lbl_13_bss_28 == 2)
    {
        lbl_13_bss_230 = 1;
        return;
    }
    lbl_13_bss_230 = 0;
}

void fn_13_1D80(void)
{
    int i;

    for (i = 0; i < lbl_13_bss_28; i++)
        g_load_mal_files_from_disc(lbl_13_bss_224[i], lbl_13_bss_230, lbl_13_bss_22C, 0xE);
}

void fn_13_1DEC(void)
{
    int i;

    for (i = 0; i < lbl_13_bss_28; i++)
        g_something_freeing_chara_heap_4(lbl_13_bss_224[i], lbl_13_bss_230, lbl_13_bss_22C);
}

void g_load_pilot(void)
{
    int i;

    for (i = 0; i < lbl_13_bss_28; i++)
        create_stage_player(i, 0x17, lbl_13_bss_224[i], i, lbl_13_bss_230, (void (*)(struct Ape *))fn_13_618C, lbl_13_bss_22C);
    if (!(lbl_1_bss_D4A10 & 1) && modeCtrl.playerCount == 1)
        balls[0].colorId = 3;
}

void fn_13_1F10(void)
{
    int i;
    struct Ball *ball;

    if (lbl_13_bss_220++ == 0)
    {
        for (i = 0; i < 5; i++)
        {
            ball = &balls[i];
            if (ball->status == 2)
            {
                ball->unk154 = 2;
                ball->flags &= ~0x10;
                ball->ape->flags &= ~0x20;
                ball->flags |= 0x4000;
            }
        }
        return;
    }
    if (lbl_13_bss_220 >= lbl_80198B38 * 2)
        lbl_13_bss_E = 8;
}

void fn_13_1FFC(void)
{
    g_fade_track_volume(0x64, 8);
    create_player_num_and_ready_sprites(lbl_80198B38 * 2);
    lbl_13_bss_220 = 0;
    lbl_13_bss_E = 9;
}

void fn_13_2058(void)
{
    int i;
    struct Ball *ball;
    struct Pilot2Player *pl;

    fn_13_380C();
    if (lbl_13_bss_220 == 1.6f * lbl_80198B34)
    {
        for (i = 0; i < 4; i++)
        {
            ball = &balls[i];
            pl = &lbl_13_bss_70[ball->playerId];
            if (ball->status == 2)
            {
                if (pl->unk42 > 0)
                    break;
                g_player_id_for_sound = ball->playerId;
                g_smth_with_sound = ball->ape->charaId;
                call_SoundReqID_arg_0(0x1E);
            }
        }
    }
    if (lbl_13_bss_220 > lbl_80198B38 * 2)
        lbl_13_bss_E = 0xA;
    lbl_13_bss_220++;
}

void fn_13_2194(void)
{
    event_restart(1);
    if ((lbl_1_bss_D4A10 & 1) && modeCtrl.unk30 == 1)
        lbl_13_bss_234[0] = 1;
    create_go_sprite(lbl_80198B38);
    lbl_13_bss_238 = -1;
    lbl_13_bss_220 = 0;
    lbl_13_bss_E = 0xB;
    fn_13_444();
}

void fn_13_2260(void)
{
    int done[5];
    int i;
    int all;

    all = 1;
    fn_13_380C();
    fn_13_3E1C();
    for (i = 0; i < 5; i++)
    {
        struct Ball *ball = &balls[i];
        struct Pilot2Player *pl = &lbl_13_bss_70[i];

        done[i] = 0;
        if (ball->status == 2)
        {
            if ((ball->unk154 == 0xA && pl->unkC > (lbl_80198B38 * 2) / 3) || pl->unk1A > lbl_80198B38 * 2)
                done[i] = 1;
            else
                all = 0;
        }
    }
    if (all)
    {
        if (!(g_some_frame_counter.flags & 1))
        {
            g_some_frame_counter.flags |= 1;
            for (i = 0; i < 5; i++)
            {
                struct Ball *ball = &balls[i];
                struct Pilot2Player *pl = &lbl_13_bss_70[i];

                if (ball->status == 2)
                {
                    ball->vel.x = 0.0f;
                    ball->vel.y = 0.0f;
                    ball->vel.z = 0.0f;
                    ball->unk60.x = 0;
                    ball->unk60.y = 0;
                    ball->unk60.z = 0;
                    {
                        Quat q = {0.0f, 0.0f, 0.0f, 1.0f};
                        Quat *p = &q;

                        ball->unkB0 = *p;
                    }
                    pl->flags |= 0x800;
                    fn_13_8478(ball);
                }
            }
        }
        else
            lbl_13_bss_E = 0xC;
    }
    if (lbl_13_bss_10 == 2 && ((done[0] && done[2]) || (done[1] && done[3])))
        g_some_frame_counter.flags |= 1;
    fn_13_28C8();
    lbl_13_bss_220++;
}

void fn_13_24F0(void)
{
    fn_13_B174();
    lbl_13_bss_23C = 0;
    lbl_13_bss_220 = 0;
    lbl_13_bss_240 = 0;
    lbl_13_bss_E = 0xD;
    fn_13_444();
}

void fn_13_257C(void)
{
    s32 ret;
    int done;
    int i;
    struct Ball *ball;
    struct Pilot2Player *pl;

    fn_13_380C();
    ret = fn_13_3E1C();
    done = 1;
    for (i = 0; i < 5; i++)
    {
        ball = &balls[i];
        pl = &lbl_13_bss_70[ball->playerId];
        if (ball->status == 2)
        {
            if (lbl_13_bss_23C == i * lbl_80198B34)
            {
                fn_13_2934(i);
                if (!(lbl_1_bss_D4A10 & 1) && (pl->flags & 0x200))
                    lbl_13_bss_23C += 0.5f * lbl_80198B34;
            }
            if (ball->unk154 == 0xA)
                done = 0;
        }
    }
    if (lbl_1_bss_D4A10 & 1)
    {
        if (lbl_13_bss_220 > lbl_80198B34 * (lbl_13_bss_10 * modeCtrl.unk30))
            lbl_1_bss_D4A10 |= 8;
        else
            done = 0;
    }
    if (done)
    {
        if (ret == 0)
        {
            if (lbl_13_bss_238 <= 0)
                lbl_13_bss_240++;
        }
        else if (fn_13_8678())
        {
            for (i = 0; i < 4; i++)
            {
                lbl_13_bss_30[i] += lbl_13_bss_50[i];
                lbl_13_bss_50[i] = 0;
                lbl_13_bss_218[i] = 0;
            }
        }
        if (lbl_13_bss_240 > lbl_80198B38 && lbl_13_bss_220 > lbl_80198B38 * 2
         && (fn_13_8678() || lbl_13_bss_240 > lbl_80198B38 * 4))
            lbl_13_bss_E = 4;
    }
    fn_13_28C8();
    lbl_13_bss_23C++;
    lbl_13_bss_220++;
}

void fn_13_28C8(void)
{
    if (lbl_13_bss_238 >= 0)
    {
        if (lbl_13_bss_238 == 0)
            g_fade_track_volume(0x64, 8);
        lbl_13_bss_238 -= 1;
    }
}

void fn_13_2934(int idx)
{
    struct Ball *ball;
    struct Pilot2Player *pl;

    ball = &balls[idx];
    pl = &lbl_13_bss_70[ball->playerId];
    if (ball->unk154 == 0xA)
        ball->unk154 = 0xB;
    if (!(pl->flags & 4))
    {
        pl->flags |= 4;
        if ((lbl_1_bss_D4A10 & 1) && !(lbl_13_bss_70[lbl_13_bss_70[idx].unk41].flags & 0x400))
            fn_13_3810(idx);
    }
}

void fn_13_2A00(void)
{
    int i;

    fade_screen_to_color(0x100, 0, 0.5f * lbl_80198B34);
    fn_13_1DEC();
    dest_all_events();
    lbl_13_bss_24 = 0;
    camera_setup_singleplayer_viewport();
    event_init(0);
    event_init(4);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0x15);
    event_init(0x14);
    enable_camera(0);
    camera_set_state(0x43);
    cameras[0].unk1F = 5;
    for (i = 0; i < 4; i++)
    {
        lbl_13_bss_244[i] = 0;
        lbl_13_bss_254[i] = (int)(1.5f * lbl_80198B34) + (int)math_sqrt(lbl_13_bss_30[i]);
    }
    fn_13_DCA0();
    lbl_13_bss_220 = 0;
    lbl_13_bss_240 = 0;
    lbl_13_bss_E = 0xF;
    fn_13_444();
}

void fn_13_2BFC(void)
{
    struct { u8 unk0[7]; u8 chara; int score; u8 flag; } rec;
    int i;
    int step;
    u8 flag;

    lbl_13_bss_220++;
    for (i = 0; i < 4; i++)
    {
        if (lbl_13_bss_244[i] < lbl_13_bss_30[i])
        {
            step = lbl_13_bss_30[i] / lbl_13_bss_254[i];
            if (step + lbl_13_bss_244[i] > lbl_13_bss_30[i])
                step = lbl_13_bss_30[i] - lbl_13_bss_244[i];
            else if (step < 1)
                step = 1;
            lbl_13_bss_244[i] += step;
            if (lbl_13_bss_254[i] > 0)
                lbl_13_bss_254[i] -= 1;
        }
        else
            lbl_13_bss_254[i] = 0;
    }
    if (fn_13_8678() && lbl_13_bss_220 > 0x32)
    {
        for (i = 0; i < 4; i++)
            lbl_13_bss_244[i] = lbl_13_bss_30[i];
    }
    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != 0 && lbl_13_bss_244[i] != lbl_13_bss_30[i])
            break;
    }
    if (i == 4)
        lbl_13_bss_240++;
    if (fn_13_8678() && lbl_13_bss_240 > 0x14)
    {
        lbl_13_bss_25C = -1;
        if (modeCtrl.playerCount == 1)
        {
            flag = 0;
            if (lbl_1_bss_D4A10 & 2)
                flag |= 1;
            switch (lbl_13_bss_C)
            {
            case 3:
                if (lbl_13_bss_10 == 1)
                    i = 0xC;
                else
                    i = 0xE;
                break;
            default:
            case 6:
                if (lbl_13_bss_10 == 1)
                    i = 0xD;
                else
                    i = 0xF;
                break;
            }
            rec.chara = active_monkey_id[0];
            rec.score = lbl_13_bss_30[0];
            rec.flag = flag;
            lbl_13_bss_25C = fn_1_11E1C0(i, &rec);
            if (lbl_13_bss_25C < 0)
            {
                lbl_13_bss_E = 0x13;
                modeCtrl.unk18 = 0.5f * lbl_80198B34;
                fade_screen_to_color(0x101, 0, modeCtrl.unk18);
                return;
            }
        }
        g_related_to_pause_menu_input.unk4 |= 2;
        lbl_13_bss_E = 0x10;
        modeCtrl.unk18 = 0.5f * lbl_80198B34;
        fade_screen_to_color(0x101, 0, modeCtrl.unk18);
    }
}

void fn_13_2F74(void)
{
    if (--modeCtrl.unk18 <= 0)
    {
        fade_screen_to_color(0x100, 0, 0.5f * lbl_80198B34);
        lbl_13_bss_E = 0x11;
        fn_13_444();
    }
}


void fn_13_3054(void)
{
    s8 chara[4];
    s8 rank[4];
    int i;
    int j;
    int n;

    fn_13_1DEC();
    fn_13_4E8();
    dest_all_events();
    event_init(0);
    event_init(4);
    event_init(0x15);
    event_init(0x14);
    event_init(0x13);
    event_init(0x10);
    event_init(0xE);
    event_init(0x11);
    event_init(0xC);
    if (modeCtrl.playerCount == 1)
    {
        chara[0] = active_monkey_id[0];
        chara[1] = -1;
        chara[2] = -1;
        chara[3] = -1;
        rank[0] = lbl_13_bss_25C;
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            if (g_poolInfo.playerPool.statusList[i] == 0)
                chara[i] = -1;
            else
            {
                n = 0;
                for (j = 0; j < 4; j++)
                {
                    if (i != j && g_poolInfo.playerPool.statusList[j] != 0
                     && lbl_13_bss_30[j] > lbl_13_bss_30[i])
                        n += 1;
                }
                chara[i] = active_monkey_id[i];
                rank[i] = n;
            }
        }
    }
    fn_1_E4708(chara, rank);
    enable_camera(0);
    camera_set_state(0x46);
    mtxa_from_translate_xyz(0.0f, 147.5f, 394.5f);
    mtxa_rotate_y(-0x8000);
    fn_1_E6760(mtxa);
    modeCtrl.submodeTimer = 0;
    lbl_13_bss_0 = 0;
    lbl_13_bss_220 = 0;
    lbl_13_bss_E = 0x12;
    fn_13_444();
    if (modeCtrl.unk30 > 1)
        g_load_stage_for_menu_bg(2, 1);
}

void fn_13_330C(void)
{
    lbl_13_bss_220++;
    if (lbl_13_bss_220 >= lbl_80198B38)
    {
        if (lbl_13_bss_0 == 0)
        {
            if (fn_1_E5DC8() && fn_13_8678())
                lbl_13_bss_0 = 1;
        }
        else if (modeCtrl.submodeTimer == 0)
        {
            modeCtrl.submodeTimer = lbl_80198B38;
            fade_screen_to_color(0x101, 0, lbl_80198B38);
            g_fade_track_volume(lbl_80198B38, 2);
            if (modeCtrl.unk30 > 1)
                call_SoundReqID_arg_0(0xD1);
        }
        else if (--modeCtrl.submodeTimer == 0)
        {
            if (modeCtrl.unk30 > 1)
            {
                g_return_to_sel_ngc(modeCtrl.gameType);
                return;
            }
            lbl_13_bss_E = 0x14;
        }
    }
}

void fn_13_3458(void)
{
    if (--modeCtrl.unk18 <= 0)
    {
        fade_screen_to_color(0x100, 0, 0.5f * lbl_80198B34);
        lbl_13_bss_E = 0x14;
        fn_13_444();
    }
}

void fn_13_3538(void)
{
    struct { u8 unk0[7]; u8 chara; int score; u8 flag; } recs[4];
    u8 flag;
    int id;
    int i;

    flag = 0;
    event_commend_dest();
    dest_all_events();
    fn_13_4E8();
    event_init(0x11);
    event_init(0x13);
    event_init(0);
    event_init(0x14);
    event_init(0x10);
    if (lbl_1_bss_D4A10 & 2)
        flag |= 1;
    switch (lbl_13_bss_C)
    {
    case 3:
        if (lbl_13_bss_10 == 1)
            id = 0xC;
        else
            id = 0xE;
        break;
    default:
    case 6:
        if (lbl_13_bss_10 == 1)
            id = 0xD;
        else
            id = 0xF;
        break;
    }
    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] == 0)
            break;
        recs[i].chara = active_monkey_id[i];
        recs[i].score = lbl_13_bss_30[i];
        recs[i].flag = flag;
    }
    if (fn_1_11DF64(id, recs, modeCtrl.playerCount))
        fn_1_11DE54(id, 1);
    else
        fn_1_11DE54(id, 0);
    modeCtrl.submodeTimer = 0;
    lbl_13_bss_E = 0x15;
    fn_13_444();
    g_load_stage_for_menu_bg(2, 1);
}

void fn_13_3740(void)
{
    if (modeCtrl.submodeTimer == 0)
    {
        if (fn_13_8678() && fn_1_11E3A4() == 0)
        {
            call_SoundReqID_arg_0(0xD1);
            modeCtrl.submodeTimer = lbl_80198B38;
            fade_screen_to_color(0x101, 0, lbl_80198B38);
            g_fade_track_volume(lbl_80198B38, 2);
        }
    }
    else if (--modeCtrl.submodeTimer == 0)
        g_return_to_sel_ngc(modeCtrl.gameType);
}

void fn_13_380C(void) {}

void fn_13_3810(int arg0)
{
    if (modeCtrl.unk30 == 1)
    {
        if (lbl_13_bss_24 != 4)
            fn_13_518(4);
        lbl_13_bss_210[0] = arg0;
        cameras[1].unk1F = 3;
    }
    else if (modeCtrl.unk30 == 2)
    {
        if (lbl_13_bss_24 == 1)
        {
            fn_13_518(5);
            lbl_13_bss_210[0] = 2;
            lbl_13_bss_210[1] = 3;
            cameras[2].unk1F = 3;
            cameras[3].unk1F = 3;
        }
        else if (lbl_13_bss_24 == 6)
        {
            fn_13_518(7);
            lbl_13_bss_210[0] = 2;
            lbl_13_bss_210[1] = 3;
            cameras[2].unk1F = 3;
            cameras[3].unk1F = 3;
        }
    }
}

void fn_13_3DDC(void)
{
    switch (lbl_13_bss_12)
    {
    default:
        fn_13_11CD4();
        break;
    case 0x15:
        fn_1_120AD8();
        break;
    }
}

s32 fn_13_3E1C(void)
{
    int result;
    int i;
    int step;

    result = 0;
    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != 0)
        {
            if (lbl_13_bss_50[i] > 0)
            {
                step = lbl_13_bss_50[i] / lbl_13_bss_218[i];
                if (step < 1)
                    step = 1;
                lbl_13_bss_50[i] -= step;
                lbl_13_bss_30[i] += step;
                result = 1;
            }
            if (lbl_13_bss_218[i] > 0)
                lbl_13_bss_218[i] -= 1;
        }
    }
    return result;
}

void fn_13_3EEC(struct Ball *ball)
{
    Vec sp20;
    Vec sp14;
    Vec sp8;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    float t;
    int i;
    int other;

    t = (float)(lbl_80198B38 - pl->unk28) / (float)lbl_80198B38;
    if (pl->unk28 > 0)
        pl->unk28--;
    if (pl->flags & 8)
    {
        if (pl->unk46 > 0)
            pl->unk46--;
        if (pad_status_groups[pl->unk40].unk0[2].button & 0x200)
        {
            if (ball->unk154 == 4)
                lbl_13_bss_234[ball->playerId]++;
            if (modeCtrl.unk30 == 1)
            {
                if (lbl_13_bss_234[ball->playerId] >= 4)
                    lbl_13_bss_234[ball->playerId] = 1;
            }
            else if (lbl_13_bss_234[ball->playerId] >= 5)
                lbl_13_bss_234[ball->playerId] = 2;
            pl->unk46 = lbl_80198B38;
        }
        lbl_13_bss_260[pl->unk41] = lbl_13_data_62C[lbl_13_bss_234[ball->playerId]];
        t *= (float)(lbl_80198B38 - pl->unk46) / (float)lbl_80198B38;
        if (ball->unk154 == 4)
            pl->unk44 = ball->rotY;
        else if (mathutil_sum_of_sq_2(ball->vel.x, ball->vel.z) > 8.5733882e-05f)
        {
            s16 angle = math_atan2(-ball->vel.x, -ball->vel.z);

            pl->unk44 += (s16)(0.03f * (s16)(angle - pl->unk44));
        }
        mtxa_from_identity();
        mtxa_rotate_y(pl->unk44);
        for (i = ball->playerId + 1; i < 5; i++)
        {
            if (pl->unk41 == lbl_13_bss_70[i].unk41)
            {
                mtxa_tf_vec(&lbl_13_bss_260[pl->unk41][lbl_13_bss_70[i].unk42], &sp20);
                PSVECAdd(&ball->pos, &sp20, &lbl_13_bss_270[i]);
            }
        }
        if (ball->unk154 == 2 && lbl_13_bss_220 > lbl_80198B38 && !(pl->flags & 0x220))
        {
            for (i = ball->playerId + 1; i < 5; i++)
            {
                struct Ball *b = &balls[i];

                if (b->status == 2 && pl->unk41 == lbl_13_bss_70[i].unk41)
                {
                    PSVECSubtract(&b->pos, &lbl_13_bss_270[i], &sp14);
                    if (mathutil_vec_len(&sp14) < 3.0f)
                    {
                        sp14.x *= 0.1f * t;
                        sp14.z *= 0.1f * t;
                        sp14.y = 0.0f;
                        PSVECAdd(&ball->pos, &sp14, &ball->pos);
                    }
                }
            }
        }
    }
    else
    {
        other = pl->unk41;
        pl->flags |= lbl_13_bss_70[other].flags & 0x20;
        PSVECSubtract(&lbl_13_bss_270[ball->playerId], &ball->pos, &sp8);
        if (mathutil_vec_sq_len(&sp8) < 225.0f
         && !(lbl_13_bss_70[other].flags & 0x220) && !(pl->flags & 0x220))
        {
            if (ball->unk154 == 2)
            {
                sp8.x *= 0.1f * t;
                sp8.z *= 0.1f * t;
                sp8.y = 0.0f;
            }
            else
                PSVECScale(&sp8, &sp8, 0.02f * t);
            PSVECAdd(&ball->pos, &sp8, &ball->pos);
            if (pl->unk18 == 0)
            {
                PSVECSubtract(&balls[other].vel, &ball->vel, &sp8);
                PSVECScale(&sp8, &sp8, 0.02f);
                PSVECAdd(&ball->vel, &sp8, &ball->vel);
            }
            if (ball->unk154 == 4)
            {
                mtxa_sq_from_mtx(balls[other].unk30);
                mtxa_sq_to_mtx(ball->unk30);
            }
        }
    }
}

void fn_13_452C(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    if (ball->pos.y < -0.2f || (pl->flags & 0x200))
    {
        if (pl->unk18 == 0)
        {
            SoundOffID(0xEF);
            if (ball->unk154 == 4)
                call_SoundReqID_arg_0(0xF6);
            else
                call_SoundReqID_arg_0(0xF5);
            g_rumble_controller(pl->unk40, 1, 0x14);
        }
        else if (pl->unk18 == 0xA)
        {
            int id;

            switch (ball->ape->charaId)
            {
            case 1:
                call_SoundReqID_arg_0(0xEE);
                break;
            case 2:
                call_SoundReqID_arg_0(0xEA);
                break;
            case 3:
                id = call_SoundReqID_arg_1_copy(0x161);
                SoundRev(id, 0x60);
                SoundCho(id, 0x40);
                break;
            default:
                call_SoundReqID_arg_0(0xEC);
                break;
            }
        }
        if (pl->unk18 < 0x708)
            pl->unk18 += 1;
        if (!(pl->flags & 0x200)
         && ((math_sqrt(mathutil_sum_of_sq_2(ball->vel.x, ball->vel.z)) < 0.09259259f && ball->pos.y < -5.0f)
          || ball->pos.y < -60.0f))
        {
            pl->flags |= 0x200;
            if (!(lbl_1_bss_D4A10 & 1))
                fn_13_E1B4(ball->playerId);
            if (pl->unk42 == 0)
            {
                fn_1_3061C(0x16A, 0x16B);
                if (modeCtrl.unk30 == 1)
                    g_maybe_smth_with_music(0x3C, 0x1E);
                else
                    g_maybe_smth_with_music(0x3C, 0x32);
                if (lbl_13_bss_238 < lbl_80198B38 * 3)
                    lbl_13_bss_238 = lbl_80198B38 * 3;
            }
        }
        if (pl->unk1A == 0x50)
            call_SoundReqID_arg_0(0xC);
        if ((pl->flags & 0x200) && pl->unk1A < 0x708)
            pl->unk1A += 1;
        if (pl->unk10 > 0.0f)
            PSVECScale(&ball->vel, &ball->vel, 0.98f);
        else
            PSVECScale(&ball->vel, &ball->vel, 0.95f);
        if (ball->vel.y < 0.0f)
            ball->vel.y *= 0.85f;
        if (ball->pos.y < -90.0f)
            ball->pos.y = -90.0f;
    }
    else
    {
        if (pl->unk18 != 0)
        {
            SoundOffID(0xEE);
            SoundOffID(0xEA);
            SoundOffID(0x161);
            SoundOffID(0xEC);
            call_SoundReqID_arg_0(0xEF);
            pl->unk18 = 0;
        }
        pl->unk1A = 0;
    }
    if (ball->pos.y < 5.0f && pl->unk18 == 0 && ball->unk154 < 8)
    {
        call_SoundReqID_arg_0(0xF9);
        return;
    }
    SoundOffID(0xF9);
}

void fn_13_48D0(void)
{
    struct PhysicsBall sp8;
    int i;
    struct Ball *ball;

    fn_13_7CBC();
    for (i = 0; i < 5; i++)
    {
        ball = &balls[i];
        if (ball->status == 2)
        {
            if (lbl_13_bss_214 == 0)
            {
                fn_13_7A60(ball, &sp8);
                fn_13_7C9C(&sp8, stagedef);
                apply_physicsball_to_ball(ball, &sp8);
            }
            ball->unk30[0][3] = ball->pos.x;
            ball->unk30[1][3] = ball->pos.y;
            ball->unk30[2][3] = ball->pos.z;
        }
    }
}

void fn_13_4990(struct Ball *ball)
{
    lbl_13_data_46C[ball->unk154](ball);
}

void fn_13_49D0(struct Ball *ball)
{
    Vec sp24;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    g_reset_ball(ball);
    if (lbl_1_bss_D4A10 & 1)
    {
        ball->colorId = ball->playerId;
        switch (modeCtrl.playerCount)
        {
        case 1:
            pl->unk41 = 0;
            pl->unk42 = ball->playerId;
            break;
        case 2:
            switch (ball->playerId)
            {
            case 0:
                pl->unk41 = 0;
                pl->unk42 = 0;
                break;
            case 2:
                pl->unk41 = 0;
                pl->unk42 = 1;
                break;
            case 1:
                pl->unk41 = 1;
                pl->unk42 = 0;
                break;
            case 3:
                pl->unk41 = 1;
                pl->unk42 = 1;
                break;
            }
            break;
        }
        if (pl->unk42 == 0)
            pl->flags |= 8;
        if (pl->unk42 == 0)
        {
            if (modeCtrl.unk30 == 1)
                lbl_13_bss_234[ball->playerId] = 0;
            else
                lbl_13_bss_234[ball->playerId] = 2;
            lbl_13_bss_260[pl->unk41] = lbl_13_data_62C[lbl_13_bss_234[ball->playerId]];
        }
    }
    else
    {
        pl->unk41 = ball->playerId;
        pl->unk42 = 0;
    }
    pl->unk40 = player_pad_map[pl->unk41];
    ball->pos = stagedef->startPos->pos;
    if (lbl_1_bss_D4A10 & 1)
    {
        if (modeCtrl.playerCount == 1)
        {
            PSVECAdd(&ball->pos, &lbl_13_bss_260[pl->unk41][ball->playerId], &ball->pos);
            ball->pos.z -= 0.5f;
        }
        else if (modeCtrl.playerCount == 2)
        {
            if (pl->unk42 == 0)
                PSVECAdd(&ball->pos, &lbl_13_data_4B8[ball->playerId], &ball->pos);
            else
            {
                PSVECAdd(&ball->pos, &lbl_13_data_4B8[pl->unk41], &sp24);
                PSVECAdd(&sp24, &lbl_13_bss_260[pl->unk41][pl->unk42], &ball->pos);
            }
        }
    }
    else
    {
        switch (modeCtrl.unk30)
        {
        case 4:
            PSVECAdd(&ball->pos, &lbl_13_data_4F4[ball->playerId], &ball->pos);
            break;
        case 3:
            PSVECAdd(&ball->pos, &lbl_13_data_4D0[ball->playerId], &ball->pos);
            break;
        case 2:
            PSVECAdd(&ball->pos, &lbl_13_data_4A0[ball->playerId], &ball->pos);
            break;
        }
    }
    ball->prevPos = ball->pos;
    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);
    ball->flags |= 0x10;
    ball->ape->flags |= 0x20;
    ball->speed = ball->unkCC = 0.0f;
    {
        Vec v = {0.0f, 0.0f, 0.0f};
        Vec *p = &v;

        ball->unkC0 = *p;
    }
    ball->ape->flags &= ~0x4000;
    {
        Quat q = {0.0f, 0.0f, 0.0f, 1.0f};
        Quat *p = &q;

        ball->unkB0 = *p;
    }
    ball->unkA0 = ball->unkB0;
    ball->state = 0x17;
    ball->unk154 = 1;
}

void fn_13_4ECC(struct Ball *ball)
{
    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;
    ball->unk60.x = 0;
    ball->unk60.y = 0;
    ball->unk60.z = 0;
}

void fn_13_4F18(struct Ball *ball)
{
    struct PhysicsBall sp8;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    fn_13_452C(ball);
    if (pl->unk18 == 0 && (ball->ape->flags & 2) && pl->unkC % 30 == 1)
    {
        g_player_id_for_sound = ball->playerId;
        g_smth_with_sound = ball->ape->charaId;
        call_SoundReqID_arg_0(lbl_13_rodata_0[(rand() & 0x7FFF) >> 0xC]);
    }
    fn_13_71B0(ball);
    if (lbl_1_bss_D4A10 & 1)
        fn_13_3EEC(ball);
    fn_13_7234(ball, &sp8);
    fn_1_4AF48(ball, &sp8);
    fn_13_7B70(ball, &sp8);
    if (pl->unk42 == 0)
    {
        if ((pad_status_groups[pl->unk40].unk0[2].button & 0x100)
         && (!(ball->flags & 1) || !(mathutil_vec_len(&ball->vel) < 0.1388889f))
         && lbl_13_bss_12 == 0xB && lbl_13_bss_220 > lbl_80198B38)
            ball->unk154 = 3;
    }
    else if (balls[pl->unk41].unk154 == 3)
        ball->unk154 = 3;
    ball->unk80 += 1;
    pl->unkC += 1;
}

void fn_13_5174(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    pl->unk10 = 1.0f;
    pl->unk8 = 0.46296296f;
    {
        Quat q = {0.0f, 0.0f, 0.0f, 1.0f};
        Quat *p = &q;

        ball->unkA0 = *p;
    }
    mtxa_sq_from_identity();
    if (mathutil_sum_of_sq_2(ball->vel.x, ball->vel.z) > 0.034293551f || (ball->prevFlags & 1))
        mtxa_rotate_y(math_atan2(-ball->vel.x, -ball->vel.z));
    mtxa_sq_to_mtx(ball->unk30);
    ball->unk154 = 3;
    cameras[ball->playerId].unk1F = 2;
    ball->flags |= 0x90;
    if (pl->unk42 == 0)
    {
        call_SoundReqID_arg_0(0xF0);
        if (pl->unk18 == 0)
            call_SoundReqID_arg_0(0xEF);
    }
    g_rumble_controller(pl->unk40, 1, 0xA);
    pl->unkC = 0;
    ball->unk154 = 4;
    fn_13_5334(ball);
}

void fn_13_5334(struct Ball *ball)
{
    struct PhysicsBall sp8;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    fn_13_452C(ball);
    if (pl->unkC == 0x11)
    {
        g_player_id_for_sound = ball->playerId;
        g_smth_with_sound = ball->ape->charaId;
        call_SoundReqID_arg_0(0x1F);
    }
    if ((pl->flags & 2) && (ball->ape->flags & 2) && pl->unkC % 30 == 1)
    {
        g_player_id_for_sound = ball->playerId;
        g_smth_with_sound = ball->ape->charaId;
        call_SoundReqID_arg_0(lbl_13_rodata_0[(rand() & 0x7FFF) >> 0xC]);
    }
    if (!(pl->flags & 0x20))
    {
        if ((pad_status_groups[pl->unk40].unk0[2].button & 0x100) && pl->unk10 >= 13.333333333333332)
        {
            ball->unk154 = 5;
            return;
        }
        if (pl->unk10 < 20.0f)
            pl->unk10 += 1.0f;
        if (pl->unk14 > 0.0f)
            pl->unk14 -= 1.0f;
    }
    fn_13_71B0(ball);
    if (lbl_1_bss_D4A10 & 1)
        fn_13_3EEC(ball);
    fn_13_6910(ball);
    fn_13_762C(ball, &sp8);
    fn_13_7B70(ball, &sp8);
    if (pl->unk8 < 0.18518518f && pl->unk18 == 0 && !(pl->flags & 0x20))
    {
        if (!(pl->flags & 0x1000))
        {
            pl->flags |= 0x1000;
            call_SoundReqID_arg_0(0xF3);
        }
    }
    else
    {
        pl->flags &= ~0x1000;
        SoundOffID(0xF3);
    }
    ball->unk80 += 1;
    pl->unkC += 1;
}

void fn_13_55C8(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    pl->flags &= ~0x1000;
    SoundOffID(0xEF);
    SoundOffID(0xF3);
    if (ball->restitution > 0.24f)
        ball->restitution = 0.24f;
    ball->unk154 = 6;
    fn_13_565C(ball);
}

void fn_13_565C(struct Ball *ball)
{
    struct PhysicsBall sp8;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    fn_13_452C(ball);
    fn_13_60A8(ball);
    if (pl->unk18 == 0 && pl->unk10 == 0.0f && pl->unkC % 30 == 1)
    {
        g_player_id_for_sound = ball->playerId;
        g_smth_with_sound = ball->ape->charaId;
        call_SoundReqID_arg_0(lbl_13_rodata_0[(rand() & 0x7FFF) >> 0xC]);
    }
    fn_13_71B0(ball);
    fn_13_7890(ball, &sp8);
    if ((sp8.flags & 1) && ball->pos.y > 0.0)
        ball->unk154 = 7;
    fn_1_4AF48(ball, &sp8);
    fn_13_7B70(ball, &sp8);
    ball->unk80 += 1;
    pl->unkC += 1;
}

void fn_13_57C0(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    if (pl->unk4C != 0)
        call_SoundReqID_arg_0(0x138);
    else if (pl->unk4A != 0)
        call_SoundReqID_arg_0(0x135);
    pl->unkC = 0;
    ball->unk154 = 8;
    fn_13_584C(ball);
}

void fn_13_584C(struct Ball *ball)
{
    struct PhysicsBall sp14;
    Vec sp8;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    float len;

    if (ball->pos.y > 0.0 && pl->unk24 == 1)
        call_SoundReqID_arg_0(0x135);
    fn_13_452C(ball);
    fn_13_60A8(ball);
    fn_13_71B0(ball);
    fn_13_7890(ball, &sp14);
    if ((sp14.flags & 1) && pl->unk18 == 0)
    {
        sp8 = ball->vel;
        fn_1_4ADC8(&sp14, &sp8);
        len = mathutil_vec_len(&sp8);
        if (len < 0.01f)
            pl->unk1C += 1;
        if (pl->unk1C > 2.5f * lbl_80198B34 || (pl->unk4C != 0 && pl->unk1C > 0.5f * lbl_80198B34))
        {
            ball->unk154 = 9;
            if (lbl_13_bss_10 == 1)
                g_some_frame_counter.flags |= 1;
        }
        else if (len < 0.0005f && (pl->unk1C > 0x1C || (pl->unk4C != 0 && pl->unk1C > 8)) && ball->unk154 < 9)
        {
            ball->unk154 = 9;
            if (lbl_13_bss_10 == 1)
                g_some_frame_counter.flags |= 1;
        }
    }
    else
        pl->unk1C = 0;
    fn_1_4AF48(ball, &sp14);
    fn_13_7B70(ball, &sp14);
    ball->unk80 += 1;
    pl->unkC += 1;
}

void fn_13_5AC0(struct Ball *ball)
{
    if (lbl_13_bss_70[ball->playerId].flags & 0x20)
        fn_13_D408(ball->playerId);
    ball->unk154 = 0xA;
    fn_13_5B2C(ball);
}

void fn_13_5B2C(struct Ball *ball)
{
    struct PhysicsBall sp18;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    fn_13_452C(ball);
    fn_13_60A8(ball);
    if (g_some_frame_counter.flags & 1)
    {
        ball->vel.x = 0.0f;
        ball->vel.y = 0.0f;
        ball->vel.z = 0.0f;
        ball->unk60.x = 0;
        ball->unk60.y = 0;
        ball->unk60.z = 0;
        {
            Quat q = {0.0f, 0.0f, 0.0f, 1.0f};
            Quat *p = &q;

            ball->unkB0 = *p;
        }
    }
    else
    {
        fn_13_71B0(ball);
        fn_13_7890(ball, &sp18);
        fn_1_4AF48(ball, &sp18);
        fn_13_7B70(ball, &sp18);
    }
    ball->unk80 += 1;
    pl->unkC += 1;
}

void fn_13_5C78(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;
    ball->unk60.x = 0;
    ball->unk60.y = 0;
    ball->unk60.z = 0;
    {
        Quat q = {0.0f, 0.0f, 0.0f, 1.0f};
        Quat *p = &q;

        ball->unkB0 = *p;
    }
    if (!(pl->flags & 0x20))
    {
        fn_13_84CC(ball);
        if (lbl_13_bss_70[ball->playerId].unk42 == 0)
            cameras[ball->playerId].unk1F = 3;
    }
    if (pl->unk2C >= 5 && !(pl->flags & 2))
        pl->flags |= 1;
    if (pl->unk42 == 0)
    {
        if (pl->unk2C >= 0x2EE)
            call_SoundReqID_arg_0(0x1AE);
        else if (pl->unk2C >= 0x190)
            call_SoundReqID_arg_0(0x1AD);
        else if (pl->unk2C >= 0xA)
            call_SoundReqID_arg_0(0x1A0);
        else
            call_SoundReqID_arg_0(0x1A4);
        g_maybe_smth_with_music(0x3C, 0x1E);
        SoundOffID(0xF9);
    }
    pl->unkC = 0;
    ball->unk154 = 0xC;
    fn_13_5E68(ball);
}

void fn_13_5E68(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    int var;

    if (pl->unkC == 0xF && !(pl->flags & 0x40))
    {
        g_player_id_for_sound = ball->playerId;
        g_smth_with_sound = ball->ape->charaId;
        if (pl->unk2C >= 0x2EE)
            call_SoundReqID_arg_0(0x12D);
        else if (pl->unk2C >= 0x190)
            call_SoundReqID_arg_0(0x130);
        else if (pl->unk2C >= 0xC8)
            call_SoundReqID_arg_0(0x12F);
        else if (pl->unk2C >= 0xA)
            call_SoundReqID_arg_0(0x12E);
        else
            call_SoundReqID_arg_0(0x12C);
    }
    if (pl->unkC == 0x1E)
    {
        var = 0;
        if (pl->unk2C >= 0x2EE)
        {
            fn_1_3061C(0x15F, 0x160);
            var = 8.5f * lbl_80198B34;
        }
        else if (pl->unk2C >= 0x190)
        {
            fn_1_3061C(0x163, 0x15C);
            var = 4.5f * lbl_80198B34;
        }
        else if (pl->unk2C >= 0xA)
        {
            fn_1_3061C(0x15D, 0x15E);
            var = 4.0f * lbl_80198B34;
        }
        else if (pl->unk42 == 0)
        {
            fn_1_3061C(0x16A, 0x16B);
            var = 3.5f * lbl_80198B34;
        }
        if (var > 0)
            g_maybe_smth_with_music(0x3C, 0x1E);
        if (var > lbl_13_bss_238)
            lbl_13_bss_238 = var;
    }
    pl->unkC += 1;
}

void fn_13_60A8(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    if (pl->unk10 > 0.0f && !(pl->flags & 0x20))
    {
        if (pl->unk18 == 0)
            pl->unk10 -= 1.0f;
        else
            pl->unk10 -= 0.4f;
        if (pl->unk10 <= 0.0f)
        {
            pl->unk10 = 0.0f;
            call_SoundReqID_arg_0(0xF1);
            g_rumble_controller(pl->unk40, 1, 0xA);
        }
    }
    if (pl->unk24 > 0)
        pl->unk24 -= 1;
}

void fn_13_618C(struct Ape *ape, int arg1)
{
    struct RaycastHit sp8;
    struct Ball *ball = &balls[ape->ballIdx];
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    int var;
    int r27;
    float speed;

    switch (arg1)
    {
    case 3:
        fn_1_D1F28(ape);
        return;
    }
    if (g_some_other_flags & 0xA)
        return;
    ape->flags &= ~0x13;
    if (ball->vel.y < -0.1388889f)
    {
        raycast_stage_down(&ball->pos, &sp8, 0);
        if (lbl_13_bss_214 != 0)
            sp8.flags |= 1;
        if (!(sp8.flags & 1) || (!(ball->flags & 1) && ball->unk154 == 6))
            ape->flags |= 2;
    }
    if (pl->unk10 == 0.0f && pl->unk18 != 0)
        ape->flags |= 2;
    if (!(ape->flags & 2))
    {
        if (mathutil_vec_len(&ball->unkC0) < 0.00027777778f)
            ape->flags |= 1;
    }
    if (pl->unk10 > 0.0f && (ball->unk154 < 8 || (pl->flags & 0x20)))
        mtxa_from_mtx(ball->unk30);
    else
    {
        r27 = 0;
        var = 0;
        if (ball->unk154 >= 8 && pl->unkC < lbl_80198B38)
            var = 1;
        r27 |= var;
        r27 |= (ape->flags & 3) == 0;
        g_ape_upright(ape);
        if (r27 != 0)
            speed = fn_1_45A8C(ape);
        else
        {
            speed = 0.0f;
            mtxa_from_quat(&ape->rot);
            mtxa_normalize_basis();
            if (ape->flags & 2)
                fn_1_46958(ape);
        }
    }
    mtxa_to_quat(&ape->rot);
    if (ball->flags & 0x20)
        speed = mathutil_vec_len(&ball->vel);
    if (ball->unk154 == 2 && lbl_13_bss_214 == 0)
        fn_1_45CA8(ape);
    else
        ball->flags &= ~2;
    fn_13_6524(ape, speed);
    g_ape_anim_body(ape);
    if (!(ape->flags & 8))
        g_move_ape_into_ball(ape);
    g_ape_anim_head(ape, &ball->unk10C);
    ball->unk108 = 0;
    ball->unk118 = 0.0f;
}

void fn_13_6524(struct Ape *ape, float speed)
{
    struct Ball *ball = &balls[ape->ballIdx];
    struct Pilot2Player *pl = &lbl_13_bss_70[ape->ballIdx];
    int i;
    int a;
    int b;
    int c;

    if (pl->unk10 > 0.0f)
    {
        a = 0xE;
        b = 0;
        speed = 0.0f;
        if (pl->flags & 0x20)
        {
            if ((ball->flags & 1) || ball->unk154 == 0xA || ball->unk154 == 0xC)
                c = 5;
            else
                c = 2;
        }
        else if (pl->unk18 != 0 || (pl->flags & 0x22))
            c = 2;
        else if (pad_status_groups[pl->unk40].unk0[0].stickX < -0x1E)
            c = 7;
        else if (pad_status_groups[pl->unk40].unk0[0].stickX > 0x1E)
            c = 8;
        else
            c = 0;
    }
    else
    {
        if (ball->unk154 == 0xC && (pl->flags & 1))
        {
            int score = pl->unk2C * pl->unk34;

            a = 0;
            b = 7;
            if (score >= 0xBB8)
                c = 8;
            else if (score >= 0x3E8)
                c = 6;
            else if (score >= 0x1F4)
                c = 4;
            else if (score >= 0x64)
                c = 2;
            else
                c = 0;
        }
        else if (ape->flags & 2)
        {
            a = 0;
            b = 5;
            c = 0;
        }
        else if (ball->flags & 0x20)
        {
            a = 0;
            b = 3;
            c = 0;
            speed = mathutil_vec_len(&ball->vel);
            lbl_1_bss_70E70[ball->playerId] = speed;
        }
        else if (ball->flags & 2)
        {
            a = 0;
            b = 4;
            c = 1;
        }
        else if (ape->flags & 1)
        {
            a = 0;
            b = 1;
            c = 0;
        }
        else
        {
            float v = 216.0f * mathutil_vec_len(&ball->vel);

            a = 0;
            b = 2;
            c = 0;
            for (i = 0xC; i >= 0; i--)
            {
                if (v > fn_1_1156F0((struct ApeData *)ball->ape->unk0, 2, i)->unk14)
                {
                    c = i;
                    i = -1;
                }
            }
        }
        if (b != 1 || sub_mode == 0x31)
            ape->unk27C = 0;
    }
    fn_1_113190(ape, a, speed, b, c);
}

void fn_13_6910(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    Vec spC4 = {1.0f, 0.0f, 0.0f};
    Vec spB8 = {0.0f, 1.0f, 0.0f};
    Vec spAC = {0.0f, 0.0f, 1.0f};
    Vec spA0 = {0.0f, 0.0f, -1.0f};
    Vec sp94;
    Vec sp88;
    Vec sp7C;
    Vec sp70;
    Vec sp64;
    Vec sp58;
    Vec sp4C;
    Quat sp3C;
    Quat sp2C;
    float x;
    float y;
    float s;
    float t;
    float q;
    float ax;
    s16 angle;
    s16 angle2;

    if (pl->flags & 0x20)
    {
        x = 0.0f;
        y = 0.0f;
    }
    else
    {
        x = pad_status_groups[pl->unk40].unk0[0].stickX;
        y = pad_status_groups[pl->unk40].unk0[0].stickY;
    }
    ax = __fabs(x);
    angle = math_atan2(__fabs(y), ax);
    if (angle > 0x2000)
        angle = 0x4000 - angle;
    s = 1.0f - (0.29289323f * angle) / 8192.0f;
    if (y > 0.0f)
    {
        x *= s;
        y *= s;
    }
    t = y - 0.25 * __fabs(x);
    mtxa_from_mtx(ball->unk30);
    mtxa_tf_vec(&spA0, &sp88);
    mtxa_tf_vec(&spB8, &sp70);
    mtxa_tf_vec(&spC4, &sp64);
    mtxa_rigid_inv_tf_vec(&ball->vel, &sp7C);
    PSVECSubtract(&ball->vel, &lbl_13_bss_18, &sp58);
    pl->unk8 = mathutil_vec_dot_prod(&sp58, &sp88);
    s = math_sqrt(pl->unk8 < 0.20833333f ? 0.20833333f : (pl->unk8 > 1.2962962f ? 1.2962962f : pl->unk8));
    x *= s;
    t *= s;
    quat_from_axis_angle(&sp3C, &spC4, (s16)(-3.5f * t));
    quat_from_axis_angle(&sp2C, &spAC, (s16)(-9.0f * x));
    quat_mult(&sp3C, &sp3C, &sp2C);
    quat_from_axis_angle(&sp2C, &spB8, (s16)(-2.0f * x));
    quat_mult(&sp3C, &sp3C, &sp2C);
    mtxa_from_quat(&sp3C);
    mtxa_mult_left(ball->unk30);
    mtxa_sq_to_mtx(ball->unk30);
    if (pl->unk38 > 0.0f)
    {
        Vec sp20 = {1.0f, 1.0f, 1.0f};

        pl->unk38 *= 0.985f;
        g_math_unk9_smth_w_quats(&sp3C, &sp20, pl->unk38);
        mtxa_from_quat(&sp3C);
        mtxa_mult_left(ball->unk30);
        mtxa_sq_to_mtx(ball->unk30);
    }
    q = -mathutil_vec_dot_prod(&sp7C, &spC4);
    quat_from_axis_angle(&sp3C, &spB8, (s16)(1800.0f * q));
    q = mathutil_vec_dot_prod(&sp7C, &spB8);
    q *= 0.05 * (21.0 - pl->unk10);
    quat_from_axis_angle(&sp2C, &spC4, (s16)(5000.0f * q));
    quat_mult(&sp3C, &sp3C, &sp2C);
    q = -mathutil_vec_dot_prod(&spB8, &sp64);
    quat_from_axis_angle(&sp2C, &spAC, (s16)(400.0f * q));
    quat_mult(&sp3C, &sp3C, &sp2C);
    q = -mathutil_vec_dot_prod(&spB8, &sp88);
    if (sp70.y < -0.0f)
    {
        if (q > 0.0f)
            q = 0.0f;
        else if (sp70.y < -0.5f)
            q *= sp70.y;
        else
            q *= 1.0f + 3.0f * sp70.y;
    }
    quat_from_axis_angle(&sp2C, &spC4, (s16)(425.0f * q));
    quat_mult(&sp3C, &sp3C, &sp2C);
    mtxa_from_quat(&sp3C);
    mtxa_mult_left(ball->unk30);
    mtxa_sq_to_mtx(ball->unk30);
    mtxa_from_mtx(ball->unk30);
    mtxa_rigid_inv_tf_vec(&sp58, &sp4C);
    angle2 = math_atan2(sp4C.y, -sp4C.z);
    pl->unk4 = -math_sin((s16)(angle2 * 2));
    if (angle2 > 0x4000 || angle2 < -0x4000)
        pl->unk4 *= -1.0f;
    pl->unk4 = 0.11f * ((pl->unk10 / 20.0f) * (pl->unk4 * (pl->unk8 * pl->unk8)));
    PSVECScale(&sp70, &sp70, pl->unk4);
    sp94.x = 0.0f;
    sp94.y = -(9.8f / (lbl_80198B34 * lbl_80198B34));
    sp94.z = 0.0f;
    if (ball->flags & 0x100)
        sp94.y = 0.0f;
    if (!(pl->flags & 0x20))
    {
        PSVECSubtract(&lbl_13_bss_18, &ball->vel, &sp4C);
        mtxa_from_mtx(ball->unk30);
        mtxa_rigid_inv_tf_vec(&sp4C, &sp4C);
        sp4C.y *= 0.01;
        sp4C.z *= 0.01;
        mtxa_tf_vec(&sp4C, &sp4C);
        PSVECScale(&sp4C, &sp4C, 0.0175f);
        PSVECAdd(&sp94, &sp4C, &sp94);
        PSVECAdd(&sp94, &sp70, &sp94);
    }
    {
        Vec sp14;

        sp14 = ball->pos;
        sp14.y = 0.0f;
        t = mathutil_vec_len(&sp14) - 2800.0;
        if (t > 0.0f)
        {
            Vec sp8;

            if (mathutil_vec_dot_prod(&ball->pos, &ball->vel) < 0.0f)
                t *= 0.3f;
            vec_normalize_len(&sp14);
            PSVECScale(&sp14, &sp8, 8e-05 * -t);
            PSVECAdd(&ball->vel, &sp8, &ball->vel);
            PSVECScale(&sp14, &lbl_13_bss_18, -lbl_13_bss_14);
            lbl_13_bss_338 = lbl_13_bss_18;
        }
    }
    if (pl->flags & 0x20)
    {
        ball->vel.x *= 0.96f;
        ball->vel.z *= 0.96f;
        sp94.y = -ball->accel;
    }
    PSVECAdd(&ball->vel, &sp94, &ball->vel);
}

void fn_13_71B0(struct Ball *ball)
{
    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20;
}

void fn_13_7234(struct Ball *ball, struct PhysicsBall *physBall)
{
    Vec sp20;
    Vec sp14;
    Vec sp8;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    float f;
    float g;

    sp20.x = 0.0f;
    sp20.y = -ball->accel;
    sp20.z = 0.0f;
    if (ball->flags & 0x100)
        sp20.y = 0.0f;
    if (!(ball->flags & 0x100000000ULL) && (ball->prevFlags & 1))
    {
        sp14 = ball->vel;
        f = mathutil_sum_of_sq_2(sp14.x, sp14.z);
        if (f > 1.1920929e-07f)
        {
            f = 1.0 / math_sqrt(f);
            sp14.x *= f;
            sp14.z *= f;
            g = sp14.x * -math_sin(cameras[ball->playerId].rotY)
              + sp14.z * -math_sin(cameras[ball->playerId].rotY + 0x4000);
            if (g < 0.0f)
            {
                f = 1.0 + -0.65 * -g;
                sp20.x *= f;
                sp20.y *= f;
                sp20.z *= f;
            }
        }
    }
    mtxa_from_identity();
    mtxa_rotate_x(world_infos[pl->unk41].xrot);
    mtxa_rotate_z(world_infos[pl->unk41].zrot);
    mtxa_rigid_inv_tf_vec(&sp20, &sp20);
    PSVECAdd(&ball->vel, &sp20, &ball->vel);
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    init_physicsball_from_ball(ball, physBall);
    physBall->friction = 0.0055f;
    if (lbl_13_bss_214 == 0)
        fn_13_7C9C(physBall, stagedef);
    else
    {
        physBall->pos.y = 0.5f + lbl_13_bss_2AC.y;
        physBall->vel.x = 0.0f;
        physBall->vel.y = 0.0f;
        physBall->vel.z = 0.0f;
        physBall->flags |= 1;
    }
    apply_physicsball_to_ball(ball, physBall);
    if (physBall->flags & 1)
    {
        if (physBall->hardestColiItemGroupId == 0)
        {
            ball->unk11C.x = -physBall->hardestColiNormal.x;
            ball->unk11C.y = -physBall->hardestColiNormal.y;
            ball->unk11C.z = -physBall->hardestColiNormal.z;
            return;
        }
        mtxa_from_mtx(itemgroups[physBall->hardestColiItemGroupId].transform);
        mtxa_tf_vec(&physBall->hardestColiNormal, &ball->unk11C);
        ball->unk11C.x = -ball->unk11C.x;
        ball->unk11C.y = -ball->unk11C.y;
        ball->unk11C.z = -ball->unk11C.z;
        return;
    }
    if (lbl_13_bss_12 == 0xB)
    {
        PSVECSubtract(&lbl_13_bss_18, &ball->vel, &sp8);
        PSVECScale(&sp8, &sp8, 0.004f);
        PSVECAdd(&ball->vel, &sp8, &ball->vel);
    }
}

void fn_13_762C(struct Ball *ball, struct PhysicsBall *physBall)
{
    Vec sp8;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    float len;

    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    fn_13_7A60(ball, physBall);
    fn_13_7C9C(physBall, stagedef);
    apply_physicsball_to_ball(ball, physBall);
    if (physBall->flags & 1)
    {
        if (physBall->hardestColiItemGroupId == 0)
        {
            ball->unk11C.x = -physBall->hardestColiNormal.x;
            ball->unk11C.y = -physBall->hardestColiNormal.y;
            ball->unk11C.z = -physBall->hardestColiNormal.z;
        }
        else
        {
            mtxa_from_mtx(itemgroups[physBall->hardestColiItemGroupId].transform);
            mtxa_tf_vec(&physBall->hardestColiNormal, &ball->unk11C);
            ball->unk11C.x = -ball->unk11C.x;
            ball->unk11C.y = -ball->unk11C.y;
            ball->unk11C.z = -ball->unk11C.z;
        }
        if (pl->flags & 0x20)
        {
            sp8 = ball->vel;
            fn_1_4ADC8(physBall, &sp8);
            len = mathutil_vec_len(&sp8);
            if (len < 0.01f)
                pl->unk1C += 1;
            if (len < 0.001f || pl->unk1C > lbl_80198B38)
            {
                ball->unk154 = 9;
                g_some_frame_counter.flags |= 1;
            }
        }
        else if (pl->unk10 >= 20.0f && (ball->unk11C.y <= -0.8f || ball->pos.y <= 25.0f))
        {
            pl->flags |= 0x20;
            if (pl->unk10 >= 20.0f)
                pl->unk10 = 16.0f;
            ball->restitution = 0.24f;
            SoundOffID(0xEF);
        }
    }
    else
        pl->unk1C = 0;
}

void fn_13_7890(struct Ball *ball, struct PhysicsBall *physBall)
{
    Vec sp14;
    Vec sp8;

    sp14.x = 0.0f;
    sp14.y = -ball->accel;
    sp14.z = 0.0f;
    if (ball->flags & 0x100)
        sp14.y = 0.0f;
    PSVECAdd(&ball->vel, &sp14, &ball->vel);
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    fn_13_7A60(ball, physBall);
    fn_13_7C9C(physBall, stagedef);
    apply_physicsball_to_ball(ball, physBall);
    if (!(physBall->flags & 1))
    {
        PSVECSubtract(&lbl_13_bss_18, &ball->vel, &sp8);
        PSVECScale(&sp8, &sp8, 0.011f);
        PSVECAdd(&ball->vel, &sp8, &ball->vel);
    }
    if (physBall->flags & 1)
    {
        if (physBall->hardestColiItemGroupId == 0)
        {
            ball->unk11C.x = -physBall->hardestColiNormal.x;
            ball->unk11C.y = -physBall->hardestColiNormal.y;
            ball->unk11C.z = -physBall->hardestColiNormal.z;
            return;
        }
        mtxa_from_mtx(itemgroups[physBall->hardestColiItemGroupId].transform);
        mtxa_tf_vec(&physBall->hardestColiNormal, &ball->unk11C);
        ball->unk11C.x = -ball->unk11C.x;
        ball->unk11C.y = -ball->unk11C.y;
        ball->unk11C.z = -ball->unk11C.z;
    }
}

void fn_13_7A60(struct Ball *ball, struct PhysicsBall *physBall)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    init_physicsball_from_ball(ball, physBall);
    physBall->friction = 0.025f;
    if (ball->unk154 == 0xA || (pl->flags & 0x800))
    {
        physBall->friction = 1.0f;
        return;
    }
    if (ball->unk154 >= 6)
    {
        if (pl->unk24 == 0)
        {
            if (pl->unk4C != 0)
            {
                physBall->friction = 0.9f;
                return;
            }
            if (pl->unk4A != 0)
                physBall->friction *= 4.0f;
        }
    }
    else if (pl->flags & 0x20)
        physBall->friction = 0.1f;
}

void fn_13_7B70(struct Ball *ball, struct PhysicsBall *physBall)
{
    position_ball(ball, physBall);
    if (!(ball->flags & 0x80000000000ULL) && !(ball->flags & 0x20000000000ULL) && ball->unk13C < -0.11f)
    {
        g_rumble_controller(lbl_13_bss_70[ball->playerId].unk40, 1, (int)((0.2f - ball->unk13C) * lbl_80198B38));
    }
}

void fn_13_7C9C(struct PhysicsBall *physBall, struct StageDef *stagedef)
{
    collide_ball_with_stage(physBall, stagedef);
}

void fn_13_7CBC(void)
{
    struct PhysicsBall a;
    struct PhysicsBall b;
    Vec sp3C;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    s8 pan;
    s8 span;
    int i;
    int k;
    struct Ball *old = current_ball;
    struct Ball *ballA;
    struct Ball *ballB;
    struct Pilot2Player *plA;
    struct Pilot2Player *plB;
    float e1;
    float e2;
    float l;
    float d;

    for (i = 0; i < 4; i++)
    {
        ballA = &balls[i];
        if (ballA->status == 2)
        {
            plA = &lbl_13_bss_70[i];
            init_physicsball_from_ball(ballA, &a);
            a.flags |= (ballA->flags & 1) ? 1 : 0;
            for (k = i + 1; k < 5; k++)
            {
                ballB = &balls[k];
                if (ballB->status == 2)
                {
                    plB = &lbl_13_bss_70[k];
                    if (ballA->unk154 == 4 && ballB->unk154 == 4 && plA->unk41 == plB->unk41)
                        continue;
                    init_physicsball_from_ball(ballB, &b);
                    b.flags |= (ballB->flags & 1) ? 1 : 0;
                    PSVECSubtract(&b.prevPos, &a.prevPos, &sp3C);
                    PSVECSubtract(&a.vel, &b.vel, &sp30);
                    PSVECSubtract(&a.pos, &b.pos, &sp24);
                    if (mathutil_vec_dot_prod(&sp3C, &sp30) > 0.0f
                     && fn_1_A74E4(&a.prevPos, &a.pos, a.radius, &b.prevPos, &b.pos, b.radius) != 0)
                    {
                        e1 = mathutil_vec_sq_len(&a.vel);
                        e2 = mathutil_vec_sq_len(&b.vel);
                        fn_13_8258(&sp24, &a.vel, plA->unk3C, &b.vel, plB->unk3C);
                        e1 = __fabs(mathutil_vec_sq_len(&a.vel) - e1);
                        e2 = __fabs(mathutil_vec_sq_len(&b.vel) - e2);
                        e2 = e1 + e2;
                        PSVECAdd(&a.pos, &a.vel, &a.pos);
                        PSVECAdd(&b.pos, &b.vel, &b.pos);
                        if (e2 > 0.001f)
                        {
                            int id;
                            s8 ret;
                            s16 pitch;

                            g_rumble_controller(plA->unk40, 1, 0xA);
                            g_rumble_controller(plB->unk40, 1, 0xA);
                            current_ball = ballA;
                            call_SoundReqID_arg_0(0x17);
                            current_ball = ballB;
                            call_SoundReqID_arg_0(0x18);
                            ret = fn_1_2F8F0(modeCtrl.playerCount, &current_ball->pos, &pan, &span);
                            id = call_SoundReqID_arg_1_copy(((span & 0x7F) << 25) | (0x15B | ((ret & 0x7F) << 11) | ((pan & 0x7F) << 18)));
                            e2 *= 6.6666665f;
                            if (e2 > 1.0f)
                                e2 = 1.0f;
                            pitch = (int)(131071.0f * e2) + 0x2000;
                            SoundPitch(id, pitch);
                        }
                        if (plA->unk41 != plB->unk41)
                        {
                            plA->unk28 = lbl_80198B38;
                            plB->unk28 = lbl_80198B38;
                        }
                        fn_13_83EC(ballA);
                        fn_13_83EC(ballB);
                    }
                    l = vec_normalize_len(&sp24);
                    l -= 0.005f;
                    d = (a.radius + b.radius) - l;
                    if (d > 0.0f)
                    {
                        PSVECScale(&sp24, &sp18, 0.5f * d);
                        PSVECScale(&sp24, &spC, 0.5f * d);
                        if (a.pos.y > b.pos.y)
                        {
                            sp18.y = sp24.y * d;
                            spC.y = 0.0f;
                        }
                        else if (a.pos.y < b.pos.y)
                        {
                            sp18.y = 0.0f;
                            spC.y = sp24.y * d;
                        }
                        PSVECAdd(&a.pos, &sp18, &a.pos);
                        PSVECSubtract(&b.pos, &spC, &b.pos);
                        if (plA->unk41 != plB->unk41)
                        {
                            plA->unk28 = lbl_80198B38;
                            plB->unk28 = lbl_80198B38;
                        }
                    }
                    apply_physicsball_to_ball(ballB, &b);
                }
            }
            apply_physicsball_to_ball(ballA, &a);
        }
    }
    current_ball = old;
}

void fn_13_8258(Vec *n, Vec *a, float m1, Vec *b, float m2)
{
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float f3;
    float f2;
    float d;
    float e;

    sp20 = *a;
    sp14 = *b;
    sp8 = *n;
    d = mathutil_vec_dot_prod(&sp8, &sp20);
    e = mathutil_vec_dot_prod(&sp8, &sp14);
    f3 = 1.5 * ((e - d) * ((m1 * m2) / ((m1 * m2) + (m1 * m1))));
    f2 = f3 * -(m1 / m2);
    sp20.x += f3 * sp8.x;
    sp20.y += f3 * sp8.y;
    sp20.z += f3 * sp8.z;
    sp14.x += f2 * sp8.x;
    sp14.y += f2 * sp8.y;
    sp14.z += f2 * sp8.z;
    *a = sp20;
    *b = sp14;
}

void fn_13_83EC(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    if (ball->unk154 == 0xA && !(pl->flags & 0x20))
    {
        ball->unk154 = 8;
        pl->unkC = 0;
        pl->unk1C = 0;
        if (pl->unk4C != 0 || pl->unk4A != 0)
            pl->unk24 = lbl_80198B38 / 2;
    }
}

void fn_13_8478(struct Ball *ball)
{
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];

    if (ball->unk154 == 0xA && !(pl->flags & 0x20))
    {
        ball->unk154 = 8;
        pl->unkC = 0;
        pl->unk1C = 0;
    }
}

void fn_13_84CC(struct Ball *ball)
{
    Vec sp8;
    struct RaycastHit hit;
    struct Pilot2Player *pl = &lbl_13_bss_70[ball->playerId];
    int var;
    int score;

    if (lbl_1_bss_D4A10 & 1)
        var = pl->unk41;
    else
        var = ball->playerId;
    sp8.x = 10.0f * ball->unk11C.x;
    sp8.y = 0.0f;
    sp8.z = 10.0f * ball->unk11C.z;
    {
        float f = mathutil_vec_sq_len(&sp8);

        if (f > 0.25f)
            PSVECScale(&sp8, &sp8, 0.5f * math_rsqrt(f));
    }
    PSVECAdd(&ball->pos, &sp8, &sp8);
    if ((int)raycast_stage_down(&sp8, &hit, 0) != 0)
        pl->unk2C = fn_1_54A60(hit.flags);
    else if (pl->unk18 == 0)
        pl->unk2C = 0xF;
    else
        pl->unk2C = 0;
    pl->unk34 = pl->unk48 + 1;
    score = pl->unk2C * pl->unk34;
    lbl_13_bss_50[var] += score;
    lbl_13_bss_218[var] = lbl_80198B38 * 3;
    lbl_13_bss_200[var] += score;
}

s32 fn_13_8678(void)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != 0 && (pad_status_groups[player_pad_map[i]].unk0[2].button & 0x100))
            return 1;
    }
    return 0;
}

static Vec lbl_13_data_DF8 = {0.0f, 145.0f, 394.0f};

void fn_13_86EC(void)
{
    lbl_13_bss_2B8 = 0;
    lbl_13_bss_2AC = lbl_13_data_DF8;
}

void fn_13_8728(void)
{
    Vec sp8 = {0.0f, 146.5f, 394.0f};
    float f;

    switch (current_stage_id)
    {
    default:
        return;
    case 0x9B:
    case 0x9C:
    case 0x9D:
        if ((float)lbl_13_bss_2B8 > 240.0f)
            return;
        if (!(g_some_other_flags & 0xA))
        {
            if (lbl_13_bss_2B8 == 0)
                call_SoundReqID_arg_0(0x2FF);
            else if (150.0f == (float)lbl_13_bss_2B8)
                call_SoundReqID_arg_0(0x301);
            else if (240.0f == (float)lbl_13_bss_2B8)
                call_SoundReqID_arg_0(0x300);
            if (240.0f == (float)lbl_13_bss_2B8++)
                lbl_13_bss_214 = 0;
        }
        lbl_13_bss_2BC = sp8;
        (&lbl_13_bss_2BC)->y -= 0.2f;
        f = (float)lbl_13_bss_2B8 / 120.0f;
        if (f < 0.0f)
            f = 0.0f;
        if (f > 1.0f)
            f = 1.0f;
        f *= 3.2f;
        lbl_13_bss_2C8 = lbl_13_bss_2BC;
        lbl_13_bss_2BC.x -= f;
        lbl_13_bss_2C8.x += f;
        f = ((float)lbl_13_bss_2B8 - 150.0f) / 90.0f;
        if (f < 0.0f)
            f = 0.0f;
        if (f > 1.0f)
            f = 1.0f;
        lbl_13_bss_2AC = lbl_13_data_DF8;
        lbl_13_bss_2AC.y += 1.5f * f;
        break;
    }
}
