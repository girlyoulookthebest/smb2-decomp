/* Dogfight game flow: rounds, player physics and input, CPU steering, sounds and results. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "input.h"
#include "item.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "sprite.h"
#include "stage.h"
#include "textdraw.h"
#include "va.h"

struct DogfightPlayer
{
    u32 flags;
    float unk4;
    float unk8;
    s32 unkC;
    float unk10;
    s16 unk14;
    s16 unk16;
    u16 unk18;
    u8 filler1A[2];
    s16 unk1C;
    s8 unk1E[3];
    u8 filler21[0x32 - 0x21];
    s16 unk32;
    s16 unk34;
    s16 unk36;
    s16 unk38;
    s8 unk3A;
    s8 unk3B;
    s16 unk3C;
    u8 filler3E[0x44 - 0x3E];
    s32 unk44;
    float unk48;
    float unk4C;
    float unk50;
    u8 filler54[0x5C - 0x54];
    s16 unk5C;
    s8 unk5E;
    s8 unk5F;
    s8 unk60;
    s8 unk61;
    s8 unk62;
    s8 unk63;
    s8 unk64;
    u8 unk65;
    u8 unk66;
    s8 unk67;
    u8 filler68[4];
    s32 unk6C;
    u8 filler70[4];
    s32 unk74;
    s32 unk78;
    u8 unk7C;
    u8 unk7D;
    u8 unk7E;
    u8 unk7F;
    u8 unk80;
    u8 unk81;
    u8 filler82[2];
    float unk84;
    float unk88;
    float unk8C;
    s8 unk90;
    s8 unk91;
    u8 filler92[2];
    u32 unk94;
    float unk98;
    float unk9C;
    s8 unkA0;
    s8 unkA1;
    s16 unkA2;
    s16 unkA4;
    s16 unkA6;
    u8 fillerA8[2];
    s16 unkAA;
    s16 unkAC;
    s8 unkAE;
    u8 fillerAF;
};

struct DogfightItem
{
    Vec pos;
    s16 type;
};

struct DogfightSpawn
{
    Vec pos;
    s16 rotY;
};

struct DogfightResult
{
    u8 filler0[7];
    u8 unk7;
    s32 unk8;
    s8 unkC;
};

struct SelSettingsG
{
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
    s32 unk4;
};

void g_very_similar_to_puts(char *str);
void unload_stage(void);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_set_color(u8 color);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
int rand(void);
void create_stage_player(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod, void (*drawFunc)(struct Ape *), int variant);
void PSVECScale(Vec *src, Vec *dst, float scale);
void PSVECSubtract(Vec *a, Vec *b, Vec *out);
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
u32 raycast_stage_down(Vec *pos, struct RaycastHit *hit, Vec *c);

void fn_17_154(void);
void fn_17_478(void);
void fn_17_518(void);
void fn_17_650(void);
void fn_17_6D0(void);
void fn_17_6F4(void);
void fn_17_724(void);
void fn_17_8A0(int mode);
void fn_17_B28(void);
void fn_17_BBC(void);
void fn_17_F1C(void);
void fn_17_132C(void);
void fn_17_1530(void);
void fn_17_160C(void);
void g_load_dog(void);
void fn_17_1B74(void);
void fn_17_1BEC(void);
void fn_17_1CA8(void);
void fn_17_1CFC(void);
void fn_17_1D58(void);
void fn_17_1E84(void);
void fn_17_1F0C(void);
void fn_17_26BC(void);
void fn_17_27A0(void);
void fn_17_2950(void);
void fn_17_2C50(void);
void fn_17_2CE0(void);
void fn_17_2F7C(void);
void fn_17_33B4(void);
void fn_17_3508(void);
void fn_17_36E0(void);
void fn_17_3794(void);
void fn_17_37D4(void);
int fn_17_3834(void);
void fn_17_396C(struct Ball *ball);
void fn_17_3BC8(void);
void fn_17_3C5C(struct Ball *ball);
void fn_17_3C9C(struct Ball *ball);
void fn_17_3DD0(struct Ball *ball);
void fn_17_3E1C(struct Ball *ball);
void fn_17_4108(struct Ball *ball);
void fn_17_48AC(struct Ape *ape, int arg1);
void fn_17_4B00(struct Ape *ape);
void fn_17_4C68(struct Ball *ball);
void fn_17_4CC4(struct Ball *ball);
void fn_17_57A0(struct Ball *ball);
void fn_17_6820(struct Ball *ball);
float fn_17_6B58(struct Ball *ball);
void fn_17_6BE0(struct Ball *ball);
void fn_17_6C64(struct Ball *ball, struct PhysicsBall *physBall);
void fn_17_6D68(struct Ball *ball, struct PhysicsBall *physBall);
void fn_17_6DB8(struct PhysicsBall *physBall, struct StageDef *stage);
void fn_17_6DD8(void);
void fn_17_724C(Vec *normal, Vec *vel1, float m1, Vec *vel2, float m2);
int fn_17_73E0(void);
void fn_17_7454(struct Ball *ball, struct DogfightPlayer *player);
void fn_17_76A0(void);
void fn_17_770C(void);

void fn_17_7750(void);
void fn_17_9554(void);
void fn_17_A8AC(void);
void fn_17_D520(int);
void fn_17_D64C(int);
void fn_17_D768(int);
void fn_17_D89C(void);
void fn_17_E9FC(void);
void fn_17_ED48(void);
void fn_17_F404(void);
void fn_17_116EC(void);
void fn_17_117E8(void);
void fn_17_12520(struct Ball *ball);
void fn_17_14938(void);
void fn_17_155B4(void);

extern float lbl_80198B34;
extern int lbl_80198B38;
extern u32 lbl_1_bss_D4CD8;
extern u32 lbl_1_bss_D4CDC;
extern s8 lbl_1_bss_D4D2C;
extern s8 lbl_1_bss_D4D2D;
extern u32 lbl_1_bss_D4D30;
extern void (*lbl_1_bss_D4CE0)(void);
extern struct DogfightPlayer lbl_1_bss_D4A18[];
extern struct SelSettingsG lbl_1_bss_1718;
extern s8 lbl_1_bss_243C;
extern void (*lbl_1_bss_70E94)(void);
extern u8 lbl_1_bss_A87E0[];
extern u8 lbl_1_bss_CF22C[];
extern u8 lbl_1_data_65980[];
extern s32 active_monkey_id[];
extern u32 g_font_char_var_3;
extern u32 g_some_other_flags;
extern s32 g_player_id_for_sound;
extern s32 g_smth_with_sound;
extern struct { u32 unk0; u32 unk4; } g_related_to_pause_menu_input;
extern void *bg_heap;
extern void *stage_heap;

u32 *OSGetStackPointer(void);
void g_set_minigame_specific_funcs(void (*)(void), void (*)(void), void *, void *);
void dest_all_events(void);
void SoundGroupLoad(int);
void load_bmp_by_id_child(int);
void free_bmp_by_id(int);
void *alloc_from_heap_or_panic(int, u32, char *, int);
void event_init(int);
void event_ball_tick(void);
void g_SoftStreamStart_with_some_defaults_2(int);
void load_stage(int);
void load_commend_stage(void);
void fade_screen_to_color(int, u32, int);
void SoundOffID(int);
void g_fade_track_volume(int, int);
void fn_1_590AC(int);
void fn_1_590F4(int);
void call_SoundReqID_arg_0(int);
int call_SoundReqID_arg_1_copy(u32);
int call_SoundReqID_arg_2(u32);
int fn_1_11E1C0(int, void *);
void g_rumble_controller(int, int, int);
void fn_1_D1F28(struct Ape *);
s8 fn_1_2F8F0(int, Vec *, s8 *, s8 *);
void SoundPitch(u16, u16);
void SoundDop(u16, u16);
void queue_stage_load(int);
void set_camera_view_settings(int, float, float, float, float);
void camera_set_state(int);
void fn_1_46958(struct Ape *);
void g_ape_anim_body(struct Ape *);
void g_move_ape_into_ball(struct Ape *);
void g_ape_upright(struct Ape *);
void quat_from_axis_angle(Quat *, Vec *, s16);
void mtxa_sq_to_mtx(Mtx);
void g_math_unk9_smth_w_quats(Quat *, Vec *, float);
void vec_set_len(float, Vec *, Vec *);
float vec_normalize_len(Vec *);
void init_physicsball_from_ball(struct Ball *, struct PhysicsBall *);
void collide_ball_with_stage(struct PhysicsBall *, struct StageDef *);
void apply_physicsball_to_ball(struct Ball *, struct PhysicsBall *);
void fn_1_113768(struct Ape *, float);
#ifdef __MWERKS__
void fn_1_CCAA0(void *, int, float, float, float);
#else
/* real prototype (ord_tbl.c); PPC passes floats in FPRs so the retail
 * call order differs from the C one */
void fn_1_CCAA0(float, float, float, void *, int);
#define fn_1_CCAA0(buf, n, ofs, mn, mx) (fn_1_CCAA0)(ofs, mn, mx, buf, n)
#endif
void fn_1_E4708(s8 *, s8 *);
void fn_1_E7600(void);
void fn_1_E7620(GXColor *);
void fn_1_E6760(Mtx);
void g_load_stage_for_menu_bg(s8, int);
int fn_1_E5DC8(void);
void g_return_to_sel_ngc(int);
int fn_1_11DF64(int, u8 *, int);
void fn_1_11DE54(int, int);
int fn_1_11E3A4(void);
void fn_1_120AD8(void);
void g_reset_ball(struct Ball *);
void position_ball(struct Ball *, struct PhysicsBall *);
void fn_1_1132D0(struct Ape *, int, int, int);
void quat_mult(Quat *, Quat *, Quat *);
int fn_1_A74E4(Vec *, Vec *, float, Vec *, Vec *, float);
void preload_ape(int, int, int, int);
void g_maybe_smth_with_music(int, s8);
void create_player_num_and_ready_sprites(int);
void create_go_sprite(int);
void g_something_with_iteratively_freeing_memory(void);
void fn_1_11C5E4(void);
void g_something_calls_sndPopGroup(void);
void camera_setup_singleplayer_viewport(void);
void g_load_minigame_gma_tpl(int);
void free_minigame_gma_tpl(void);
void mtxa_normalize_basis(void);
void mtxa_to_quat(Quat *);
void mtxa_rigid_inv_tf_vec(Vec *, Vec *);
s16 math_atan(float);

s16 lbl_17_bss_374[6];
s32 lbl_17_bss_36C;
s8 lbl_17_bss_370;
s16 lbl_17_bss_364[4];
s32 lbl_17_bss_354[4];
s32 lbl_17_bss_350;
s32 lbl_17_bss_34C;
s32 lbl_17_bss_348;
struct DogfightSpawn *lbl_17_bss_344;
s32 lbl_17_bss_334[4];
s32 lbl_17_bss_324[4];
s32 lbl_17_bss_314[4];
s16 lbl_17_bss_30C[4];
s16 lbl_17_bss_308;
struct DogfightPlayer lbl_17_bss_48[4];
s16 lbl_17_bss_44;
s8 lbl_17_bss_40[4];
s8 lbl_17_bss_3C;
s32 lbl_17_bss_2C[4];
s8 lbl_17_bss_28;
s32 lbl_17_bss_24;
Vec lbl_17_bss_18;
s16 lbl_17_bss_14;
s16 lbl_17_bss_12;
s16 lbl_17_bss_10;
u8 lbl_17_bss_E;
s16 lbl_17_bss_C;
void *lbl_17_bss_8;
void *lbl_17_bss_4;
s32 lbl_17_bss_0 = 0;


u8 lbl_17_data_0[0x170] =
{
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

s16 lbl_17_data_170[4] = {0x98, 0x99, 0x9A, 0};

float lbl_17_data_178[16] =
{
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
};

float lbl_17_data_1B8[16] =
{
    0.0f, 0.0f, 0.5f, 0.5f,
    0.5f, 0.0f, 0.5f, 0.5f,
    0.0f, 0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f,
};

float lbl_17_data_1F8[16] =
{
    0.0f, 0.0f, 1.0f, 0.5f,
    0.0f, 0.5f, 1.0f, 0.5f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
};

float lbl_17_data_238[16] =
{
    0.0f, 0.0f, 0.5f, 1.0f,
    0.5f, 0.0f, 0.5f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
};

float *lbl_17_data_278[3] = {lbl_17_data_1F8, lbl_17_data_238, lbl_17_data_238};

float lbl_17_data_284[16] =
{
    0.0f, 0.0f, 1.0f, 0.5f,
    0.0f, 0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f,
    0.0f, 0.0f, 0.0f, 0.0f,
};

float lbl_17_data_2C4[16] =
{
    0.0f, 0.5f, 0.5f, 0.5f,
    0.0f, 0.0f, 1.0f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f,
    0.0f, 0.0f, 0.0f, 0.0f,
};

float lbl_17_data_304[16] =
{
    0.0f, 0.0f, 0.5f, 0.5f,
    0.5f, 0.0f, 0.5f, 0.5f,
    0.0f, 0.5f, 1.0f, 0.5f,
    0.0f, 0.0f, 0.0f, 0.0f,
};

float *lbl_17_data_344[4] = {lbl_17_data_284, lbl_17_data_2C4, lbl_17_data_304, lbl_17_data_1B8};

void (*lbl_17_data_354[])(void) =
{
    fn_17_B28,
    fn_17_BBC,
    fn_17_F1C,
    fn_17_132C,
    fn_17_1530,
    fn_17_160C,
    g_load_dog,
    fn_17_1B74,
    fn_17_1BEC,
    fn_17_1CA8,
    fn_17_1CFC,
    fn_17_1D58,
    fn_17_1E84,
    fn_17_1F0C,
    fn_17_26BC,
    fn_17_27A0,
    fn_17_2C50,
    fn_17_2CE0,
    fn_17_2F7C,
    fn_17_33B4,
    fn_17_3508,
    fn_17_36E0,
};

void (*lbl_17_data_3AC[])(struct Ball *) = {fn_17_3C9C, fn_17_3DD0, fn_17_3E1C, fn_17_4108};

struct DogfightSpawn lbl_17_data_3BC[3][4] =
{
    {
        {{-1.0f, 250.0f, 95.0f}, 0},
        {{95.0f, 251.5f, 1.0f}, 16384},
        {{1.0f, 250.0f, -95.0f}, -32768},
        {{-95.0f, 251.5f, -1.0f}, -16384},
    },
    {
        {{0.0f, 560.0f, 450.0f}, 0},
        {{450.0f, 560.0f, 0.0f}, 16384},
        {{0.0f, 560.0f, -450.0f}, -32768},
        {{-450.0f, 560.0f, 0.0f}, -16384},
    },
    {
        {{-1.0f, 634.0f, 95.0f}, 0},
        {{95.0f, 635.5f, 1.0f}, 16384},
        {{1.0f, 634.0f, -95.0f}, -32768},
        {{-95.0f, 635.5f, -1.0f}, -16384},
    },
};


const s16 lbl_17_rodata_0[8] = {0x3A, 0x3B, 0x3F, 0x53, 0x54, 0x55, 0x56, 0x57};
const s16 lbl_17_rodata_10[4] = {0x2BC, 0x2BD, 0x2BB, 0x2BE};
const s16 lbl_17_rodata_18[4] = {0x2A7, 0x2A8, 0x2A9, 0x2AA};
const s16 lbl_17_rodata_20[4] = {0x2A3, 0x2A4, 0x2A5, 0x2A6};

void _prolog(void)
{
    g_set_minigame_specific_funcs(fn_17_518, fn_17_3794, fn_17_3C5C, fn_17_7750);
    g_very_similar_to_puts("RELOCATION DOGFIGHT LOADED");
    bg_heap = stage_heap;
    fn_17_154();
}

void _epilog(void)
{
    fn_17_478();
    unload_stage();
    bg_heap = (void *)-1;
    g_very_similar_to_puts("RELOCATION DOGFIGHT PURGED");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    for (sp = OSGetStackPointer(); sp != NULL && (uptr)sp != 0xFFFFFFFF && i++ < 16; sp = (u32 *)(uptr)sp[0])
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
    OSPanic("dogfight.c", 0x179, "\n");
}

void fn_17_154(void)
{
    struct SelSettingsG *sel = &lbl_1_bss_1718;

    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    unload_stage();
    switch (modeCtrl.unk30)
    {
    case 1:
        fn_1_CCAA0(lbl_1_bss_A87E0, 0x4000, 0.0f, 0.0f, 400.0f);
        break;
    case 2:
        fn_1_CCAA0(lbl_1_bss_A87E0, 0x2000, 0.0f, 0.0f, 400.0f);
        break;
    default:
        fn_1_CCAA0(lbl_1_bss_A87E0, 0x1000, 0.0f, 0.0f, 400.0f);
        break;
    }
    SoundGroupLoad(0x13);
    load_bmp_by_id_child(0x12);
    fn_17_6D0();
    lbl_1_bss_D4CE0 = fn_17_155B4;
    lbl_17_bss_4 = alloc_from_heap_or_panic(currentHeap, 0x4830, "dogfight.c", 0x199);
    lbl_17_bss_8 = alloc_from_heap_or_panic(currentHeap, 0x12030, "dogfight.c", 0x19A);
    lbl_17_bss_C = 10;
    lbl_1_bss_D4CD8 = 0;
    sub_mode_request = 200;
    if (pad_status_groups[0].unk0[0].button & PAD_TRIGGER_Z)
    {
        lbl_17_bss_E = lbl_1_data_65980[sel->unk0];
        lbl_1_bss_D4D2D = sel->unk1;
        lbl_1_bss_D4D30 = 0;
        lbl_1_bss_D4D2C = 1;
        lbl_17_bss_10 = 0;
        lbl_17_bss_12 = 0;
    }
    else
    {
        lbl_17_bss_E = lbl_1_data_65980[sel->unk0];
        lbl_1_bss_D4D2D = sel->unk1;
        lbl_1_bss_D4D30 = sel->unk4;
        lbl_1_bss_D4D2C = 1;
        lbl_17_bss_10 = sel->unk3;
        if (modeCtrl.playerCount <= 1)
            lbl_1_bss_D4D30 |= 1;
        else if (modeCtrl.playerCount == 4)
            lbl_1_bss_D4D30 &= ~1;
        if (lbl_1_bss_D4D2D != 0)
            lbl_1_bss_D4D30 &= ~0x20;
        lbl_17_bss_12 = 2;
    }
    fn_17_650();
}

void fn_17_478(void)
{
    dest_all_events();
    fn_1_11C5E4();
    OSFreeToHeap(currentHeap, lbl_17_bss_8);
    OSFreeToHeap(currentHeap, lbl_17_bss_4);
    fn_17_6F4();
    free_bmp_by_id(0x12);
    g_something_calls_sndPopGroup();
    fn_1_CCAA0(lbl_1_bss_A87E0, 0x4000, 0.0f, 0.0f, 400.0f);
}

void fn_17_518(void)
{
    struct Ball *ball;

    if (!(g_some_other_flags & 0xA))
    {
        fn_17_650();
        if (dip_switches & 1)
        {
            ball = current_ball;
            if ((merged_digital_inputs[0] & 0xD00) == 0xD00 && lbl_17_bss_14 < 0x10)
            {
                lbl_17_bss_12 = 0x10;
                return;
            }
            if ((pad_status_groups[player_pad_map[ball->playerId]].unk0[0].button & PAD_TRIGGER_R)
             && (pad_status_groups[player_pad_map[ball->playerId]].unk0[2].button & PAD_BUTTON_X)
             && lbl_17_bss_14 < 0x10)
                lbl_17_bss_12 = 4;
        }
    }
}

void fn_17_650(void)
{
    if (lbl_17_bss_12 != -1)
    {
        lbl_17_bss_14 = lbl_17_bss_12;
        lbl_17_bss_12 = -1;
    }
    lbl_17_data_354[lbl_17_bss_14]();
}

void fn_17_6D0(void)
{
    Vec *unused = &lbl_17_bss_18;

    g_load_minigame_gma_tpl(9);
}

void fn_17_6F4(void)
{
    free_minigame_gma_tpl();
    lbl_1_bss_D4CE0 = NULL;
}

struct DogfightItem lbl_17_data_530[] =
{
    {{0.0f, 0.0f, 0.0f}, -1},
};

struct DogfightItem lbl_17_data_540[] =
{
    {{36.274601f, 136.0f, 36.274609f}, 8},
    {{-3.6000001e-05f, 136.0f, 51.300041f}, 8},
    {{-36.274651f, 136.0f, 36.274559f}, 8},
    {{-51.300041f, 136.0f, -0.00012f}, 8},
    {{-36.274559f, 136.0f, -36.274651f}, 8},
    {{7.0000001e-06f, 136.0f, -51.300041f}, 8},
    {{36.274559f, 136.0f, -36.274651f}, 8},
    {{51.300041f, 136.0f, -3.8999999e-05f}, 8},
    {{0.0f, 150.0f, 0.0f}, 2},
    {{700.0f, 120.0f, -20.0f}, 2},
    {{700.0f, 120.0f, -40.0f}, 2},
    {{700.0f, 120.0f, -60.0f}, 2},
    {{700.0f, 120.0f, 0.0f}, 2},
    {{700.0f, 120.0f, 20.0f}, 2},
    {{700.0f, 120.0f, 40.0f}, 2},
    {{700.0f, 120.0f, 60.0f}, 2},
    {{-700.0f, 120.0f, -20.0f}, 2},
    {{-700.0f, 120.0f, -40.0f}, 2},
    {{-700.0f, 120.0f, -60.0f}, 2},
    {{-700.0f, 120.0f, 0.0f}, 2},
    {{-700.0f, 120.0f, 20.0f}, 2},
    {{-700.0f, 120.0f, 40.0f}, 2},
    {{-700.0f, 120.0f, 60.0f}, 2},
    {{-20.0f, 120.0f, 700.0f}, 2},
    {{-40.0f, 120.0f, 700.0f}, 2},
    {{-60.0f, 120.0f, 700.0f}, 2},
    {{0.0f, 120.0f, 700.0f}, 2},
    {{20.0f, 120.0f, 700.0f}, 2},
    {{40.0f, 120.0f, 700.0f}, 2},
    {{60.0f, 120.0f, 700.0f}, 2},
    {{-20.0f, 120.0f, -700.0f}, 2},
    {{-40.0f, 120.0f, -700.0f}, 2},
    {{-60.0f, 120.0f, -700.0f}, 2},
    {{0.0f, 120.0f, -700.0f}, 2},
    {{20.0f, 120.0f, -700.0f}, 2},
    {{40.0f, 120.0f, -700.0f}, 2},
    {{60.0f, 120.0f, -700.0f}, 2},
    {{0.0f, 0.0f, 0.0f}, -1},
};

struct DogfightItem lbl_17_data_7A0[] =
{
    {{36.274601f, 500.0f, 36.274609f}, 8},
    {{-3.6000001e-05f, 500.0f, 51.300041f}, 8},
    {{-36.274651f, 500.0f, 36.274559f}, 8},
    {{-51.300041f, 500.0f, -0.00012f}, 8},
    {{-36.274559f, 500.0f, -36.274651f}, 8},
    {{7.0000001e-06f, 500.0f, -51.300041f}, 8},
    {{36.274559f, 500.0f, -36.274651f}, 8},
    {{51.300041f, 500.0f, -3.8999999e-05f}, 8},
    {{200.0f, 430.0f, -20.0f}, 2},
    {{200.0f, 430.0f, -40.0f}, 2},
    {{200.0f, 430.0f, -60.0f}, 2},
    {{200.0f, 430.0f, 0.0f}, 2},
    {{200.0f, 430.0f, 20.0f}, 2},
    {{200.0f, 430.0f, 40.0f}, 2},
    {{200.0f, 430.0f, 60.0f}, 2},
    {{-200.0f, 430.0f, -20.0f}, 2},
    {{-200.0f, 430.0f, -40.0f}, 2},
    {{-200.0f, 430.0f, -60.0f}, 2},
    {{-200.0f, 430.0f, 0.0f}, 2},
    {{-200.0f, 430.0f, 20.0f}, 2},
    {{-200.0f, 430.0f, 40.0f}, 2},
    {{-200.0f, 430.0f, 60.0f}, 2},
    {{0.0f, 0.0f, 0.0f}, -1},
};

struct DogfightItem lbl_17_data_910[] =
{
    {{36.274601f, 600.0f, 36.274609f}, 8},
    {{-3.6000001e-05f, 600.0f, 51.300041f}, 8},
    {{-36.274651f, 600.0f, 36.274559f}, 8},
    {{-51.300041f, 600.0f, -0.00012f}, 8},
    {{-36.274559f, 600.0f, -36.274651f}, 8},
    {{7.0000001e-06f, 600.0f, -51.300041f}, 8},
    {{36.274559f, 600.0f, -36.274651f}, 8},
    {{51.300041f, 600.0f, -3.8999999e-05f}, 8},
    {{0.0f, 600.0f, 0.0f}, 2},
    {{200.0f, 530.0f, -20.0f}, 2},
    {{200.0f, 530.0f, -40.0f}, 2},
    {{200.0f, 530.0f, -60.0f}, 2},
    {{200.0f, 530.0f, 0.0f}, 2},
    {{200.0f, 530.0f, 20.0f}, 2},
    {{200.0f, 530.0f, 40.0f}, 2},
    {{200.0f, 530.0f, 60.0f}, 2},
    {{-200.0f, 530.0f, -20.0f}, 2},
    {{-200.0f, 530.0f, -40.0f}, 2},
    {{-200.0f, 530.0f, -60.0f}, 2},
    {{-200.0f, 530.0f, 0.0f}, 2},
    {{-200.0f, 530.0f, 20.0f}, 2},
    {{-200.0f, 530.0f, 40.0f}, 2},
    {{-200.0f, 530.0f, 60.0f}, 2},
    {{0.0f, 0.0f, 0.0f}, -1},
};


void fn_17_724(void)
{
    struct Item item;
    int i;
    struct DogfightItem *entry;

    memset(&item, 0, sizeof(item));
    item.type = 6;
    item.animGroupId = 0;
    entry = lbl_17_data_530;
    for (i = 0; entry[i].type != -1; i++)
    {
        item.pos = entry[i].pos;
        item.subType = entry[i].type;
        item.stageBanana = (void *)&entry[i];
        spawn_item(&item);
    }
    switch (current_stage_id)
    {
    case 0x98:
        entry = lbl_17_data_540;
        break;
    case 0x99:
        entry = lbl_17_data_7A0;
        break;
    case 0x9A:
        entry = lbl_17_data_910;
        break;
    default:
        entry = NULL;
        break;
    }
    if (entry != NULL)
    {
        for (i = 0; entry[i].type != -1; i++)
        {
            item.pos = entry[i].pos;
            item.subType = entry[i].type;
            item.stageBanana = (void *)&entry[i];
            spawn_item(&item);
        }
    }
}

const Vec lbl_17_rodata_30 = {0.0f, 0.0f, -1.0f};

void fn_17_8A0(int mode)
{
    int i;
    float *layout;

    lbl_17_bss_24 = mode;
    switch (lbl_17_bss_24)
    {
    case 0:
        layout = lbl_17_data_178;
        break;
    case 4:
    case 1:
        layout = lbl_17_data_278[modeCtrl.wideMode];
        break;
    case 2:
        layout = lbl_17_data_344[modeCtrl.splitscreenMode];
        break;
    default:
        layout = lbl_17_data_1B8;
        break;
    }
    if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == 3)
    {
        set_camera_view_settings(4, layout[12], layout[13], layout[14], layout[15]);
        enable_camera(4);
        cameras[4].flags |= 0x60;
    }
    for (i = 0; i < lbl_17_bss_28; i++, layout += 4)
    {
        set_camera_view_settings(i, layout[0], layout[1], layout[2], layout[3]);
        if (layout[2] / layout[3] > 1.0f)
            cameras[i].sub28.view.nextFov = 0x20B6;
        else if (layout[2] / layout[3] <= 0.5f)
            cameras[i].sub28.view.nextFov = 0x3333;
        else
            cameras[i].sub28.view.nextFov = 0x25B0;
        cameras[i].sub28.view.fov = cameras[i].sub28.view.nextFov;
        enable_camera(i);
    }
    for (i = 0; i < modeCtrl.unk30; i++)
        lbl_17_bss_2C[i] = 1;
    for (i = modeCtrl.unk30; i < 4; i++)
    {
        set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
        lbl_17_bss_2C[i] = 0;
    }
}

void fn_17_B28(void)
{
    lbl_17_bss_24 = 0;
    camera_setup_singleplayer_viewport();
    lbl_17_bss_3C = 0;
    modeCtrl.playerCount = 2;
    lbl_17_bss_12 = 1;
    fn_17_650();
}

void fn_17_BBC(void)
{
    int i;

    debug_textdraw_set_pos(1, 1);
    debug_textdraw_vsprintf_buf_1("SELECTOR\n");
    if (pad_status_groups[0].unk0[2].button & (PAD_BUTTON_DOWN | PAD_BUTTON_UP))
    {
        lbl_17_bss_3C++;
        if (lbl_17_bss_3C >= 2)
            lbl_17_bss_3C = 0;
    }
    if (lbl_17_bss_3C == 0)
        debug_textdraw_set_color(0xFF);
    else
        debug_textdraw_set_color(0x57);
    debug_textdraw_set_pos(1, 5);
    debug_textdraw_vsprintf_buf_1("%d PLAYER(S)\n", modeCtrl.playerCount);
    if (lbl_17_bss_3C == 1)
        debug_textdraw_set_color(0xFF);
    else
        debug_textdraw_set_color(0x57);
    if (lbl_1_bss_D4D30 & 1)
        debug_textdraw_vsprintf_buf_1("COM ON\n");
    else
        debug_textdraw_vsprintf_buf_1("COM OFF\n");
    debug_textdraw_set_color(0xFF);
    debug_textdraw_set_pos(1, 20);
    debug_textdraw_vsprintf_buf_1("PRESS A BUTTON TO START\n");
    switch (lbl_17_bss_3C)
    {
    case 0:
        if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_LEFT)
        {
            modeCtrl.playerCount--;
            if (modeCtrl.playerCount <= 0)
                modeCtrl.playerCount = 4;
        }
        else if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_RIGHT)
        {
            modeCtrl.playerCount++;
            if (modeCtrl.playerCount > 4)
                modeCtrl.playerCount = 1;
        }
        break;
    case 1:
        if (pad_status_groups[0].unk0[2].button & (PAD_BUTTON_LEFT | PAD_BUTTON_RIGHT))
            lbl_1_bss_D4D30 ^= 1;
        break;
    }
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_A)
    {
        modeCtrl.unk30 = modeCtrl.playerCount;
        if (modeCtrl.playerCount <= 1)
            lbl_1_bss_D4D30 |= 1;
        for (i = 0; i < 4; i++)
        {
            if (i < modeCtrl.playerCount)
            {
                g_poolInfo.playerPool.statusList[i] = 2;
                player_pad_map[i] = i;
                active_monkey_id[i] = i;
            }
            else
                g_poolInfo.playerPool.statusList[i] = 0;
        }
        lbl_17_bss_12 = 2;
    }
}

void fn_17_F1C(void)
{
    struct Ball *ball;
    struct Ball *ballBackup;
    int chara;
    int j;
    int found;
    int i;
    int k;

    lbl_17_bss_28 = 0;
    if (modeCtrl.playerCount == 1 && lbl_1_bss_D4D2D == 0)
        lbl_1_bss_D4D30 |= 0x10;
    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] == 2)
        {
            lbl_17_bss_28++;
            lbl_17_bss_40[i] = active_monkey_id[i];
        }
        else if (lbl_1_bss_D4D30 & 1)
        {
            lbl_1_bss_D4CD8 |= 1 << i;
            lbl_17_bss_28++;
            lbl_17_bss_40[i] = 0;
            for (chara = 0; chara < 4; chara++)
            {
                found = 0;
                for (j = 0; j < i; j++)
                {
                    if (lbl_17_bss_40[j] == chara)
                    {
                        found = 1;
                        break;
                    }
                }
                if (found == 0)
                {
                    lbl_17_bss_40[i] = chara;
                    break;
                }
            }
        }
        else
            lbl_17_bss_40[i] = -1;
    }
    switch (modeCtrl.unk30)
    {
    case 1:
        lbl_17_bss_24 = 0;
        break;
    case 2:
        if (modeCtrl.wideMode != 0)
            lbl_17_bss_24 = 4;
        else
            lbl_17_bss_24 = 1;
        break;
    case 3:
        if (modeCtrl.splitscreenMode == 3)
            lbl_17_bss_24 = 3;
        else
            lbl_17_bss_24 = 2;
        break;
    default:
        lbl_17_bss_24 = 3;
        break;
    }
    lbl_17_bss_44 = 1;
    for (i = 0; i < 4; i++)
        lbl_17_bss_48[i].unk5F = 0;
    if (lbl_1_bss_D4D2D == 1)
        lbl_17_bss_308 = 200;
    else
        lbl_17_bss_308 = 100;
    fn_17_9554();
    fn_17_F404();
    for (i = 0; i < 4; i++)
        lbl_17_bss_30C[i] = lbl_80198B34 * (0.1f + 0.1f * ((float)rand() / 32767.0f));
    event_init(2);
    ballBackup = current_ball;
    ball = balls;
    for (k = 0; k < 8; k++, ball++)
    {
        if (ball->status == 2)
        {
            current_ball = ball;
            ball->state = 1;
        }
    }
    current_ball = ballBackup;
    event_ball_tick();
    if (!(lbl_1_bss_D4CDC & 8))
        g_SoftStreamStart_with_some_defaults_2(0x51);
    camera_set_state(0x43);
    lbl_17_bss_12 = 3;
    fn_17_650();
}

void fn_17_132C(void)
{
    int i;
    struct DogfightPlayer *player;

    dest_all_events();
    lbl_1_bss_D4D30 &= 0x35;
    for (i = 0; i < 4; i++)
    {
        lbl_17_bss_314[i] = 0;
        lbl_17_bss_324[i] = 0;
        lbl_17_bss_334[i] = 0;
    }
    lbl_1_bss_CF22C[0xF] = 0;
    if (current_stage_id != lbl_17_data_170[lbl_17_bss_10])
        unload_stage();
    load_stage(lbl_17_data_170[lbl_17_bss_10]);
    load_commend_stage();
    fn_17_76A0();
    if (lbl_1_bss_D4D30 & 0x10)
        fn_17_770C();
    lbl_17_bss_344 = lbl_17_data_3BC[lbl_17_bss_10];
    for (i = 0; i < 4; i++)
    {
        player = &lbl_17_bss_48[i];
        player->flags = 0;
    }
    for (i = 0; i < 4; i++)
    {
        lbl_17_bss_324[i] = lbl_17_bss_314[i];
        lbl_17_bss_334[i] = 0;
    }
    fn_17_116EC();
    lbl_17_bss_348 = lbl_80198B38 * ((s8)lbl_17_bss_E * 60);
    lbl_17_bss_34C = 0;
    fade_screen_to_color(0x100, 0, 30);
    lbl_17_bss_12 = 6;
    fn_17_650();
}

void fn_17_1530(void)
{
    modeCtrl.unk18 = 0.5 * (double)lbl_80198B34;
    fade_screen_to_color(0x101, 0, modeCtrl.unk18);
    lbl_17_bss_12 = 5;
    fn_17_650();
}

void fn_17_160C(void)
{
    int i;

    if (--modeCtrl.unk18 <= 0)
    {
        fn_17_37D4();
        for (i = 0; i < lbl_17_bss_28; i++)
        {
            SoundOffID(lbl_17_rodata_18[i]);
            SoundOffID(lbl_17_rodata_20[i]);
        }
        g_fade_track_volume(100, 8);
        if (lbl_1_bss_D4D30 & 4)
            lbl_17_bss_12 = 3;
        else if (lbl_1_bss_D4D30 & 8)
        {
            lbl_17_bss_12 = 0x10;
            fn_17_650();
        }
        else
        {
            if (++lbl_17_bss_10 >= 3U)
                lbl_17_bss_10 = 0;
            lbl_17_bss_44++;
            lbl_17_bss_12 = 3;
        }
        fade_screen_to_color(0x100, 0, 0.5 * (double)lbl_80198B34);
    }
}

void g_load_dog(void)
{
    int mode[4];
    Vec pos;
    int i;
    int count;
    struct Ball *ball;
    struct Ball *ballBackup;
    int k;

    event_init(0);
    event_init(2);
    event_init(3);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(11);
    lbl_1_bss_243C = 0;
    event_init(16);
    event_init(17);
    event_init(19);
    event_init(14);
    event_init(20);
    fn_17_8A0(lbl_17_bss_24);
    camera_set_state(0x43);
    for (i = 0; i < lbl_17_bss_28; i++)
        cameras[i].unk1F = 1;
    cameras[4].unk1F = 1;
    if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == 3)
        count = 4;
    else
        count = modeCtrl.unk30;
    fn_1_590AC(count);
    fn_1_590F4(count);
    fn_17_A8AC();
    pos = lbl_17_rodata_30;
    lbl_17_bss_18.x = 0.0f;
    lbl_17_bss_18.y = 0.0f;
    lbl_17_bss_18.z = 0.0f;
    fn_17_724();
    if (modeCtrl.unk30 >= 4)
    {
        mode[0] = 3;
        mode[1] = 3;
        mode[2] = 3;
        mode[3] = 3;
    }
    else if (modeCtrl.unk30 == 3)
    {
        mode[0] = 2;
        mode[1] = 2;
        mode[2] = 2;
        mode[3] = 3;
    }
    else if (modeCtrl.unk30 == 2)
    {
        if (lbl_17_bss_28 == 2)
        {
            mode[0] = 1;
            mode[1] = 1;
        }
        else
        {
            mode[0] = 2;
            mode[1] = 2;
        }
        mode[2] = 3;
        mode[3] = 3;
    }
    else
    {
        mode[0] = 0;
        mode[1] = 3;
        mode[2] = 3;
        mode[3] = 3;
    }
    for (i = 0; i < 4; i++)
    {
        if (lbl_17_bss_40[i] >= 0)
            create_stage_player(i, 0x17, lbl_17_bss_40[i], i, mode[i], (void *)fn_17_48AC, mode[i] == 3 ? 1 : 0);
    }
    lbl_1_bss_70E94 = fn_17_3BC8;
    ballBackup = current_ball;
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
    current_ball = ballBackup;
    event_ball_tick();
    lbl_17_bss_350 = 0;
    lbl_17_bss_12 = 7;
    fn_17_650();
}

void fn_17_1B74(void)
{
    if (lbl_17_bss_350 >= lbl_80198B38 * 4 || fn_17_73E0() != 0)
        lbl_17_bss_12 = 8;
    lbl_17_bss_350++;
}

void fn_17_1BEC(void)
{
    int i;
    struct Ball *ball;

    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        if (ball->status == 2)
            ball->unk154 = 2;
    }
    cameras[4].unk1F = 9;
    lbl_17_bss_350 = 0;
    lbl_17_bss_12 = 9;
    fn_17_650();
}

void fn_17_1CA8(void)
{
    if (lbl_17_bss_350 >= lbl_80198B38 * 2)
        lbl_17_bss_12 = 10;
    lbl_17_bss_350++;
}

void fn_17_1CFC(void)
{
    g_fade_track_volume(100, 8);
    create_player_num_and_ready_sprites(lbl_80198B38 * 2);
    lbl_17_bss_350 = 0;
    lbl_17_bss_12 = 11;
}

void fn_17_1D58(void)
{
    int i;
    struct Ball *ball;

    if ((float)lbl_17_bss_350 == 1.6f * lbl_80198B34)
    {
        for (i = 0; i < 4; i++)
        {
            ball = &balls[i];
            if (ball->status == 2)
            {
                g_player_id_for_sound = ball->playerId;
                g_smth_with_sound = ball->ape->charaId;
                call_SoundReqID_arg_0(0x1E);
            }
        }
    }
    if (lbl_17_bss_350 > lbl_80198B38 * 2)
        lbl_17_bss_12 = 12;
    lbl_17_bss_350++;
}

void fn_17_1E84(void)
{
    create_go_sprite(lbl_80198B38);
    lbl_17_bss_350 = 0;
    lbl_17_bss_12 = 13;
    fn_17_650();
}

void fn_17_1F0C(void)
{
    int i;
    int retired;
    int eliminated;
    int alive;
    int cpu;
    int max;

    fn_17_3834();
    if (lbl_1_bss_D4D2D == 1)
    {
        retired = 0;
        eliminated = 0;
        alive = 0;
        cpu = 0;
        lbl_17_bss_34C++;
        for (i = 0; i < 4; i++)
        {
            struct Ball *ball = &balls[i];
            struct DogfightPlayer *player = &lbl_17_bss_48[i];

            if (ball->status == 2)
            {
                if (player->flags & 0x40000)
                    eliminated++;
                if (player->flags & 0x20000)
                    retired++;
                else if (!(player->flags & 0x800))
                    alive++;
                else
                    cpu++;
            }
        }
        if (eliminated == lbl_17_bss_28 - 1)
        {
            for (i = 0; i < lbl_17_bss_28; i++)
            {
                struct DogfightPlayer *player = &lbl_17_bss_48[i];

                if (!(player->flags & 0x40000))
                {
                    player->flags |= 0x4000;
                    break;
                }
            }
        }
        if (retired >= lbl_17_bss_28 - 1)
        {
            if (retired == lbl_17_bss_28)
            {
                retired = 0;
                for (i = 0; i < lbl_17_bss_28; i++)
                {
                    struct DogfightPlayer *player = &lbl_17_bss_48[i];

                    if (player->unk6C > retired)
                        retired = player->unk6C;
                }
                for (i = 0; i < lbl_17_bss_28; i++)
                {
                    struct DogfightPlayer *player = &lbl_17_bss_48[i];

                    if (player->unk6C == retired)
                        player->flags &= ~0x20000;
                }
            }
            lbl_1_bss_D4D30 |= 0x40;
            lbl_17_bss_12 = 14;
        }
        else if (!(lbl_1_bss_D4CDC & 0x40) && alive == 0)
        {
            alive = 0;
            for (i = 0; i < lbl_17_bss_28; i++)
            {
                struct DogfightPlayer *player = &lbl_17_bss_48[i];

                if ((player->flags & 0x800) && !(player->flags & 0x20000) && player->unk5C > alive)
                    alive = player->unk5C;
            }
            for (i = 0; i < lbl_17_bss_28; i++)
            {
                struct DogfightPlayer *player = &lbl_17_bss_48[i];

                if ((player->flags & 0x800) && !(player->flags & 0x20000))
                {
                    if (player->unk5C < alive)
                    {
                        player->flags |= 0x20000;
                        player->unk6C = lbl_17_bss_34C + player->unk5C;
                        cpu--;
                    }
                    if (cpu == 1)
                        break;
                }
            }
            if (cpu > 1)
            {
                max = 0;
                for (i = 0; i < lbl_17_bss_28; i++)
                {
                    struct DogfightPlayer *player = &lbl_17_bss_48[i];

                    if ((player->flags & 0x800) && !(player->flags & 0x20000) && lbl_17_bss_314[i] > max)
                        max = lbl_17_bss_314[i];
                }
                for (i = 0; i < lbl_17_bss_28; i++)
                {
                    struct DogfightPlayer *player = &lbl_17_bss_48[i];

                    if ((player->flags & 0x800) && !(player->flags & 0x20000))
                    {
                        if (lbl_17_bss_314[i] < max)
                        {
                            player->flags |= 0x20000;
                            player->unk6C = lbl_17_bss_34C + player->unk5C + lbl_17_bss_314[i];
                            cpu--;
                        }
                        if (cpu == 1)
                            break;
                    }
                }
            }
            if (cpu > 1)
            {
                for (i = 0; i < lbl_17_bss_28; i++)
                {
                    struct DogfightPlayer *player = &lbl_17_bss_48[i];

                    if ((player->flags & 0x800) && !(player->flags & 0x20000))
                    {
                        player->flags |= 0x20000;
                        player->unk6C = lbl_17_bss_34C + player->unk5C + lbl_17_bss_314[i] + 1;
                        cpu--;
                        if (cpu == 1)
                            break;
                    }
                }
            }
            lbl_1_bss_D4D30 |= 0x40;
            lbl_17_bss_12 = 14;
        }
        for (i = 0; i < lbl_17_bss_28; i++)
        {
            struct DogfightPlayer *player = &lbl_17_bss_48[i];

            if ((player->flags & 0x20000) && !(player->flags & 0x80000))
            {
                player->flags |= 0x80000;
                fn_17_D64C(i);
            }
        }
    }
    else if (lbl_17_bss_348 > 0)
    {
        if (!(lbl_1_bss_D4CDC & 4))
            lbl_17_bss_348--;
        if ((lbl_1_bss_D4D30 & 0x20) && (float)lbl_17_bss_348 == 11.5f * lbl_80198B34)
            fn_17_ED48();
        if (lbl_17_bss_348 == lbl_80198B38 * 6 - 1)
            fn_17_E9FC();
        if (lbl_17_bss_348 == lbl_80198B38 * 6 - 10)
            call_SoundReqID_arg_0(0x28);
        else if (lbl_17_bss_348 == lbl_80198B38 * 5 - 10)
            call_SoundReqID_arg_0(0x27);
        else if (lbl_17_bss_348 == lbl_80198B38 * 4 - 10)
            call_SoundReqID_arg_0(0x26);
        else if (lbl_17_bss_348 == lbl_80198B38 * 3 - 10)
            call_SoundReqID_arg_0(0x25);
        else if (lbl_17_bss_348 == lbl_80198B38 * 2 - 10)
            call_SoundReqID_arg_0(0x24);
        else if ((float)lbl_17_bss_348 == 0.1f * lbl_80198B34)
            call_SoundReqID_arg_0(0xB);
        if ((dip_switches & 1) && (pad_status_groups[0].unk0[0].button & PAD_BUTTON_Y)
         && (pad_status_groups[0].unk0[0].button & PAD_BUTTON_X))
        {
            lbl_17_bss_348 -= 100;
            if (lbl_17_bss_348 < 0)
                lbl_17_bss_348 = 0;
        }
    }
    else
    {
        lbl_1_bss_D4D30 |= 2;
        lbl_17_bss_12 = 14;
    }
    lbl_17_bss_350++;
}

void fn_17_26BC(void)
{
    int i;
    struct Ball *ball;
    struct DogfightPlayer *player;

    lbl_1_bss_D4D30 &= ~4;
    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        player = &lbl_17_bss_48[i];
        if (ball->status == 2)
        {
            player->flags &= ~0x400;
            player->flags |= 0x4000;
        }
    }
    lbl_17_bss_350 = 0;
    lbl_17_bss_12 = 15;
    fn_17_650();
}

void fn_17_27A0(void)
{
    if (lbl_1_bss_D4D2D == 1)
    {
        if ((float)lbl_17_bss_350 == lbl_80198B34)
            fn_17_2950();
        else if (lbl_17_bss_350 == lbl_80198B38 * 6)
            lbl_17_bss_12 = 4;
    }
    else if ((float)lbl_17_bss_350 == lbl_80198B34)
        fn_17_D89C();
    else if ((float)lbl_17_bss_350 == 6.0f * lbl_80198B34)
        fn_17_2950();
    else if (lbl_17_bss_350 == lbl_80198B38 * 12)
        lbl_17_bss_12 = 4;
    lbl_17_bss_350++;
}

void fn_17_2950(void)
{
    int i;
    int winners;
    int human;
    int j;
    int best;

    winners = 0;
    human = 0;
    fn_17_37D4();
    for (i = 0; i < lbl_17_bss_28; i++)
        cameras[i].unk1F = 5;
    if (lbl_1_bss_D4D2D == 1)
    {
        for (i = 0; i < 4; i++)
        {
            struct Ball *ball = &balls[i];
            struct DogfightPlayer *player = &lbl_17_bss_48[i];

            if (ball->status == 2 && !(player->flags & 0x20000))
                player->flags |= 0x10000;
        }
    }
    else
    {
        best = -1;
        for (j = 0; j < 4; j++)
        {
            struct Ball *ball = &balls[j];

            if (ball->status == 2 && lbl_17_bss_314[j] >= best)
                best = lbl_17_bss_314[j];
        }
        for (j = 0; j < 4; j++)
        {
            struct Ball *ball = &balls[j];
            struct DogfightPlayer *player = &lbl_17_bss_48[j];

            if (ball->status == 2)
            {
                if (best == lbl_17_bss_314[j])
                    player->flags |= 0x10000;
                else
                    player->flags |= 0x20000;
            }
        }
    }
    for (i = 0; i < 4; i++)
    {
        struct Ball *ball = &balls[i];
        struct DogfightPlayer *player = &lbl_17_bss_48[i];

        if (ball->status == 2 && (player->flags & 0x10000))
        {
            winners++;
            if (!(player->flags & 0x800))
                human = 1;
        }
    }
    for (i = 0; i < 4; i++)
    {
        struct Ball *ball = &balls[i];
        struct DogfightPlayer *player = &lbl_17_bss_48[i];

        if (ball->status == 2)
        {
            if (player->flags & 0x10000)
            {
                player->unk5F++;
                if (winners > 1)
                {
                    player->flags |= 0x100000;
                    fn_17_D768(i);
                }
                else
                    fn_17_D520(i);
            }
            else
                fn_17_D64C(i);
        }
    }
    lbl_1_bss_D4D30 |= 8;
    if (human != 0)
        g_SoftStreamStart_with_some_defaults_2(0x54);
    else
        g_SoftStreamStart_with_some_defaults_2(0x55);
    g_maybe_smth_with_music(0x51, 30);
}

void fn_17_2C50(void)
{
    lbl_17_bss_24 = 0;
    camera_setup_singleplayer_viewport();
    g_related_to_pause_menu_input.unk4 |= 2;
    lbl_17_bss_12 = 18;
    fn_17_650();
}

void fn_17_2CE0(void)
{
    s8 zero;
    struct DogfightResult result;
    int i;
    int step;
    int rec;

    lbl_17_bss_350++;
    for (i = 0; i < 4; i++)
    {
        if (lbl_17_bss_354[i] < lbl_17_bss_314[i])
        {
            step = lbl_17_bss_314[i] / lbl_17_bss_364[i];
            if (step + lbl_17_bss_354[i] > lbl_17_bss_314[i])
                step = lbl_17_bss_314[i] - lbl_17_bss_354[i];
            else if (step < 1)
                step = 1;
            lbl_17_bss_354[i] += step;
            if (lbl_17_bss_364[i] > 0)
                lbl_17_bss_364[i]--;
        }
        else
            lbl_17_bss_364[i] = 0;
    }
    if (fn_17_73E0() != 0 && lbl_17_bss_350 > 50)
    {
        for (i = 0; i < 4; i++)
            lbl_17_bss_354[i] = lbl_17_bss_314[i];
    }
    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != 0 && lbl_17_bss_354[i] != lbl_17_bss_314[i])
            break;
    }
    if (i == 4)
        lbl_17_bss_36C++;
    if (fn_17_73E0() != 0 && lbl_17_bss_36C > 20)
    {
        lbl_17_bss_370 = -1;
        if (lbl_1_bss_D4D30 & 0x10)
        {
            zero = 0;
            switch (lbl_1_bss_1718.unk0)
            {
            case 0:
            default:
                rec = 0x1E;
                break;
            case 1:
                rec = 0x1F;
                break;
            case 2:
                rec = 0x20;
                break;
            }
            result.unk7 = active_monkey_id[0];
            result.unk8 = lbl_17_bss_314[0];
            result.unkC = zero;
            lbl_17_bss_370 = fn_1_11E1C0(rec, &result);
        }
        g_related_to_pause_menu_input.unk4 |= 2;
        lbl_17_bss_12 = 18;
    }
}

void fn_17_2F7C(void)
{
    Vec pos;
    s8 charas[4];
    s8 ranks[4];
    s16 rotY;
    int i;
    int j;
    int rank;

    fn_17_6F4();
    dest_all_events();
    event_init(0);
    event_init(4);
    event_init(21);
    event_init(20);
    event_init(19);
    event_init(16);
    event_init(14);
    event_init(17);
    event_init(12);
    for (i = 0; i < 4; i++)
    {
        if (i >= lbl_17_bss_28)
            charas[i] = -1;
        else
        {
            if (lbl_1_bss_D4D2D == 0)
            {
                rank = 0;
                for (j = 0; j < lbl_17_bss_28; j++)
                {
                    if (i != j && lbl_17_bss_314[j] > lbl_17_bss_314[i])
                        rank++;
                }
            }
            else if (lbl_1_bss_D4D2C == 1)
            {
                rank = 0;
                for (j = 0; j < lbl_17_bss_28; j++)
                {
                    if (i != j && lbl_17_bss_48[j].unk6C > lbl_17_bss_48[i].unk6C)
                        rank++;
                }
            }
            else if (lbl_17_bss_48[i].unk5F == 0)
                rank = 3;
            else
            {
                rank = 0;
                for (j = 0; j < lbl_17_bss_28; j++)
                {
                    if (i != j && lbl_17_bss_48[j].unk5F > lbl_17_bss_48[i].unk5F)
                        rank++;
                }
            }
            charas[i] = lbl_17_bss_40[i];
            if (lbl_17_bss_48[i].flags & 0x800)
                charas[i] |= 0x40;
            ranks[i] = rank;
        }
    }
    fn_1_E4708(charas, ranks);
    if (current_stage_id == 0x99)
    {
        GXColor color = {0x1E, 0x1E, 0x1E, 0xFF};
        GXColor color2;

        fn_1_E7600();
        color2 = color;
        fn_1_E7620(&color2);
    }
    enable_camera(0);
    camera_set_state(0x46);
    switch (current_stage_id)
    {
    case 0x98:
        {
            Vec p = {24.0f, 70.0f, 66.0f};
            Vec *q = &p;

            pos = *q;
            rotY = -0x7000;
        }
        break;
    case 0x99:
        {
            Vec p = {0.0f, 525.0f, 394.5f};
            Vec *q = &p;

            pos = *q;
            rotY = -0x8000;
        }
        break;
    default:
        {
            Vec p = {0.0f, 147.5f, 394.5f};
            Vec *q = &p;

            pos = *q;
            rotY = -0x8000;
        }
        break;
    }
    mtxa_from_translate(&pos);
    mtxa_rotate_y(rotY);
    fn_1_E6760(mtxa->mtxA);
    modeCtrl.submodeTimer = 0;
    lbl_17_bss_0 = 0;
    lbl_17_bss_350 = 0;
    lbl_17_bss_12 = 19;
    fn_17_650();
    if (!(lbl_1_bss_D4D30 & 0x10))
        g_load_stage_for_menu_bg(8, 1);
}

void fn_17_33B4(void)
{
    lbl_17_bss_350++;
    if (lbl_17_bss_350 >= lbl_80198B38)
    {
        if (lbl_17_bss_0 == 0)
        {
            if (fn_1_E5DC8() != 0 && fn_17_73E0() != 0)
                lbl_17_bss_0 = 1;
        }
        else if (modeCtrl.submodeTimer == 0)
        {
            modeCtrl.submodeTimer = lbl_80198B38;
            fade_screen_to_color(0x101, 0, lbl_80198B38);
            g_fade_track_volume(lbl_80198B38, 2);
            if (!(lbl_1_bss_D4D30 & 0x10))
                call_SoundReqID_arg_0(0xD1);
        }
        else if (--modeCtrl.submodeTimer == 0)
        {
            if (!(lbl_1_bss_D4D30 & 0x10))
            {
                g_return_to_sel_ngc(modeCtrl.gameType);
                return;
            }
            lbl_17_bss_12 = 20;
        }
    }
}

void fn_17_3508(void)
{
    struct DogfightResult results[4];
    s8 zero;
    int rec;
    int i;

    zero = 0;
    fn_17_6F4();
    dest_all_events();
    event_init(17);
    event_init(19);
    event_init(0);
    event_init(20);
    event_init(16);
    switch (lbl_1_bss_1718.unk0)
    {
    case 0:
    default:
        rec = 0x1E;
        break;
    case 1:
        rec = 0x1F;
        break;
    case 2:
        rec = 0x20;
        break;
    }
    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] == 0)
            break;
        results[i].unk7 = active_monkey_id[i];
        results[i].unk8 = lbl_17_bss_314[i];
        results[i].unkC = zero;
    }
    if (fn_1_11DF64(rec, (void *)results, modeCtrl.playerCount) != 0)
        fn_1_11DE54(rec, 1);
    else
        fn_1_11DE54(rec, 0);
    modeCtrl.submodeTimer = 0;
    lbl_17_bss_12 = 21;
    fn_17_650();
    g_load_stage_for_menu_bg(8, 1);
}

void fn_17_36E0(void)
{
    if (modeCtrl.submodeTimer == 0)
    {
        if (fn_17_73E0() != 0 && fn_1_11E3A4() == 0)
        {
            call_SoundReqID_arg_0(0xD1);
            modeCtrl.submodeTimer = 60;
            fade_screen_to_color(0x101, 0, 60);
            g_fade_track_volume(60, 2);
        }
    }
    else if (--modeCtrl.submodeTimer == 0)
        g_return_to_sel_ngc(modeCtrl.gameType);
}

void fn_17_3794(void)
{
    switch (lbl_17_bss_14)
    {
    default:
        fn_17_14938();
        break;
    case 21:
        fn_1_120AD8();
        break;
    }
}

void fn_17_37D4(void)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        lbl_17_bss_314[i] += lbl_17_bss_334[i];
        lbl_17_bss_334[i] = 0;
        lbl_17_bss_374[i] = 0;
    }
}

int fn_17_3834(void)
{
    int ret;
    int i;
    int step;

    ret = 0;
    for (i = 0; i < 4; i++)
    {
        if (balls[i].status != 0)
        {
            if (lbl_17_bss_334[i] > 0)
            {
                if (lbl_17_bss_374[i] <= 0)
                    step = lbl_17_bss_334[i];
                else
                {
                    step = lbl_17_bss_334[i] / lbl_17_bss_374[i];
                    if (step < 1)
                        step = 1;
                }
                lbl_17_bss_334[i] -= step;
                lbl_17_bss_314[i] += step;
                if (lbl_17_bss_314[i] > 9999999)
                {
                    lbl_17_bss_314[i] = 9999999;
                    lbl_17_bss_334[i] = 0;
                }
                ret = 1;
            }
            if (lbl_17_bss_374[i] > 0)
                lbl_17_bss_374[i]--;
        }
    }
    return ret;
}

void fn_17_396C(struct Ball *ball)
{
    struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];

    if (ball->pos.y < -0.05f || (player->flags & 0x200))
    {
        if (player->unk14 == 0)
        {
            SoundOffID(lbl_17_rodata_18[ball->playerId]);
            player->unk74 = -1;
            SoundOffID(lbl_17_rodata_20[ball->playerId]);
            player->unk78 = -1;
            g_player_id_for_sound = ball->playerId;
            call_SoundReqID_arg_0(0x2C6);
            g_rumble_controller(player->unk5E, 1, 20);
        }
        else if (player->unk14 == 10 && lbl_17_bss_2C[ball->playerId] != 0)
            call_SoundReqID_arg_0(lbl_17_rodata_10[ball->ape->charaId]);
        player->unk14++;
        if (!(player->flags & 0x200)
         && ((math_sqrt(mathutil_sum_of_sq_2(ball->vel.x, ball->vel.z)) < 0.09259259f && ball->pos.y < -5.0f)
          || ball->pos.y < -60.0f))
        {
            player->flags |= 0x200;
            if (lbl_1_bss_D4D2D == 1)
            {
                player->flags |= 0x40000;
                player->unk6C = lbl_17_bss_34C;
            }
        }
        if (player->flags & 0x200)
            player->unk16++;
        PSVECScale(&ball->vel, &ball->vel, 0.96f);
        if (ball->vel.y < 0.0f)
            ball->vel.y *= 0.9f;
    }
    else
    {
        if (player->unk14 != 0)
        {
            SoundOffID(lbl_17_rodata_10[ball->ape->charaId]);
            player->unk14 = 0;
        }
        player->unk16 = 0;
    }
}

void fn_17_3BC8(void)
{
    int i;

    fn_17_117E8();
    fn_17_6DD8();
    if (dip_switches & 1)
    {
        for (i = 0; i < 4; i++)
            lbl_1_bss_D4A18[i] = lbl_17_bss_48[i];
    }
}

void fn_17_3C5C(struct Ball *ball)
{
    lbl_17_data_3AC[ball->unk154](ball);
}

void fn_17_3C9C(struct Ball *ball)
{
    g_reset_ball(ball);
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

void fn_17_3DD0(struct Ball *ball)
{
    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;
    ball->unk60.x = 0;
    ball->unk60.y = 0;
    ball->unk60.z = 0;
}

void fn_17_3E1C(struct Ball *ball)
{
    struct DogfightPlayer *p;
    struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];
    int keep;
    s8 wins;
    int i;

    keep = player->flags & 0x8000;
    if (lbl_17_bss_14 == 15)
        keep |= player->flags & 0x110000;
    p = &lbl_17_bss_48[ball->playerId];
    wins = p->unk5F;
    memset(p, 0, sizeof(*p));
    p->unk5F = wins;
    p->unk5E = player_pad_map[ball->playerId];
    p->unk50 = 1.0f;
    p->unk61 = -1;
    p->unk60 = 10;
    p->unk34 = -1;
    p->unk38 = -1;
    p->unk67 = -1;
    p->unk74 = -1;
    p->unk78 = -1;
    p->unkA1 = -1;
    p->unkA0 = ball->playerId % 3;
    p->unk5C = lbl_17_bss_308;
    p->unk6C = 360000;
    for (i = 0; i < 3; i++)
        p->unk1E[i] = -1;
    player->flags |= keep;
    fn_17_4C68(ball);
    ball->pos = lbl_17_bss_344[ball->playerId].pos;
    ball->rotY = lbl_17_bss_344[ball->playerId].rotY;
    ball->prevPos = ball->pos;
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_to_mtx(ball->unk30);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -0.32407406f, &ball->vel);
    player->unk8 = 0.32407406f;
    ball->flags |= 0x90;
    ball->ape->flags &= ~0x20;
    {
        Quat q = {0.0f, 0.0f, 0.0f, 1.0f};
        Quat *src = &q;

        ball->unkA0 = *src;
    }
    player->unk10 = 20.0f;
    player->flags |= 0x4000;
    fn_1_113768(ball->ape, 0.0f);
    if (lbl_17_bss_14 != 15)
        cameras[ball->playerId].unk1F = 3;
    player->unkC = 0;
    ball->unk154 = 3;
    fn_17_4108(ball);
}

void fn_17_4108(struct Ball *ball)
{
    struct Effect effect;
    struct PhysicsBall physBall;
    float t;
    struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];

    fn_17_396C(ball);
    if (lbl_17_bss_14 == 13 && lbl_17_bss_350 == 1)
        player->flags |= 0x400;
    if ((float)player->unkC == 2.5f * lbl_80198B34)
    {
        player->flags &= ~0xC000;
        ball->flags |= 0x90;
        ball->ape->flags &= ~0x20;
    }
    else if (player->flags & 0x8000)
    {
        if (player->unkC == 0)
            player->flags |= 0x400;
        if (g_font_char_var_3 & 1)
        {
            ball->flags |= 0x80;
            ball->ape->flags &= ~0x20;
        }
        else
        {
            ball->flags &= ~0x80ULL;
            ball->ape->flags |= 0x20;
        }
    }
    if (lbl_17_bss_2C[ball->playerId] != 0 && player->unk14 == 0)
    {
        if (player->unk74 == -1)
        {
            g_player_id_for_sound = ball->playerId;
            player->unk74 = call_SoundReqID_arg_2(lbl_17_rodata_18[ball->playerId] | 0x30000 | 0xD800);
        }
        if (player->unk78 == -1)
        {
            g_player_id_for_sound = ball->playerId;
            player->unk78 = call_SoundReqID_arg_2(lbl_17_rodata_20[ball->playerId] | 0x30000 | 0x8800);
        }
    }
    if (player->unk74 != -1)
    {
        t = player->unk8 / 1.1111112f;
        if (t > 1.0f)
            t = 1.0f;
        else if (t < 0.2f)
            t = 0.2f;
        SoundDop(player->unk74, 16383.0f * t);
    }
    if (player->unk3C > 0)
        player->unk3C--;
    if (player->unk34 >= 0)
    {
        if (player->unk34 < lbl_80198B38 * 3)
        {
            player->unk34++;
            if (player->unk34 == (int)(0.5f * lbl_80198B34) && lbl_17_bss_2C[ball->playerId] != 0)
            {
                g_player_id_for_sound = ball->playerId;
                call_SoundReqID_arg_0(0x29D);
            }
        }
        else
            player->unk34 = -1;
    }
    if (player->unk38 >= 0)
    {
        if (player->unk38 < lbl_80198B38 * 3)
            player->unk38++;
        else
            player->unk38 = -1;
    }
    if (player->flags & 8)
    {
        if (player->unk32 < 1800)
            player->unk32++;
        if (lbl_17_bss_2C[player->unk3B] != 0 && player->unk32 == (int)(1.3f * lbl_80198B34))
            call_SoundReqID_arg_0(0x2BF);
    }
    if (player->flags & 0x208)
    {
        if ((player->flags & 0x10000) && player->unk36 > lbl_80198B38 * 2
         && (lbl_1_bss_D4D2D != 1 || !(player->flags & 0x200)))
            ball->unk154 = 2;
        else
        {
            if (player->unk36 < 1800)
                player->unk36++;
            if (player->unk36 == lbl_80198B38 * 3)
                player->flags |= 0x10;
        }
    }
    if (lbl_1_bss_D4D2D != 1)
    {
        if (player->unk36 >= lbl_80198B38 * 5 && lbl_17_bss_14 == 13)
        {
            SoundOffID(lbl_17_rodata_10[ball->ape->charaId]);
            player->flags |= 0x8000;
            ball->unk154 = 2;
        }
    }
    else if (!(lbl_1_bss_D4D30 & 0x40) && player->unk36 == lbl_80198B38)
    {
        player->flags |= 0x20000;
        player->flags &= ~0x400;
    }
    if ((player->flags & 2) && (ball->ape->flags & 2) && player->unkC % 30 == 1)
    {
        g_player_id_for_sound = ball->playerId;
        g_smth_with_sound = ball->ape->charaId;
        call_SoundReqID_arg_0(lbl_17_rodata_0[(rand() & 0x7FFF) >> 12]);
    }
    fn_17_4CC4(ball);
    fn_17_6BE0(ball);
    fn_17_57A0(ball);
    fn_17_6C64(ball, &physBall);
    position_ball(ball, &physBall);
    fn_17_12520(ball);
    if (player->unk8 < 0.18518518f && player->unk14 == 0)
    {
        if (!(player->flags & 0x1000))
            player->flags |= 0x1000;
    }
    else
        player->flags &= ~0x1000;
    if (player->flags & 8)
    {
        lbl_17_bss_30C[ball->playerId]--;
        if (lbl_17_bss_30C[ball->playerId] == 0)
        {
            memset(&effect, 0, sizeof(effect));
            effect.type = 0x2C;
            effect.playerId = ball->playerId;
            spawn_effect(&effect);
            lbl_17_bss_30C[ball->playerId] = lbl_80198B34 * (0.1f + 0.1f * ((float)rand() / 32767.0f));
        }
    }
    ball->unk80++;
    player->unkC++;
}

void fn_17_48AC(struct Ape *ape, int arg1)
{
    struct RaycastHit hit;
    struct Ball *ball = &balls[ape->ballIdx];
    struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];
    int flag;

    switch (arg1)
    {
    case 3:
        fn_1_D1F28(ape);
        break;
    default:
        if (!(g_some_other_flags & 0xA))
        {
            raycast_stage_down(&ball->pos, &hit, NULL);
            ape->flags &= ~0x13;
            if ((!(hit.flags & 1) && ball->vel.y < -0.1388889f) || (0.0f == player->unk10 && player->unk14 != 0))
                ape->flags |= 2;
            else if (mathutil_vec_len(&ball->unkC0) < 0.00027777778f)
                ape->flags |= 1;
            if (player->unk10 > 0.0f)
                mtxa_from_mtx(ball->unk30);
            else
            {
                flag = 0;
                flag |= (ape->flags & 3) == 0;
                g_ape_upright(ape);
                if (flag != 0)
                    fn_1_45A8C(ape);
                else
                {
                    mtxa_from_quat(&ape->rot);
                    mtxa_normalize_basis();
                    if (ape->flags & 2)
                        fn_1_46958(ape);
                }
            }
            mtxa_to_quat(&ape->rot);
            if (ball->flags & 0x20)
                mathutil_vec_len(&ball->vel);
            fn_17_4B00(ape);
            g_ape_anim_body(ape);
            if (!(ape->flags & 8))
                g_move_ape_into_ball(ape);
            ball->unk108 = 0;
            ball->unk118 = 0.0f;
        }
        break;
    }
}

void fn_17_4B00(struct Ape *ape)
{
    int a;
    int b;
    int c;
    struct Ball *ball = &balls[ape->ballIdx];
    struct DogfightPlayer *player = &lbl_17_bss_48[ape->ballIdx];

    if (ball->unk154 == 3)
    {
        a = 0x13;
        b = 0;
        if (player->flags & 0x208)
            c = 1;
        else if ((float)player->unk3C > (float)(lbl_80198B38 * 2) - 0.8f * lbl_80198B34)
            c = 1;
        else if ((player->flags & 0x10000) && !(player->flags & 0x100000))
        {
            b = 1;
            c = 0;
        }
        else if (player->flags & 0x20000)
        {
            b = 1;
            c = 1;
        }
        else
            c = 0;
    }
    else
    {
        a = 0;
        b = 0;
        c = 0;
    }
    fn_1_1132D0(ape, a, b, c);
}

void fn_17_4C68(struct Ball *ball)
{
    struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];

    if ((lbl_1_bss_D4CD8 >> ball->playerId) & 1)
    {
        player->flags |= 0x800;
        return;
    }
    player->flags &= ~0x800;
}

float lbl_17_data_ADC[3] = {0.5f, 0.69999999f, 0.94999999f};
float lbl_17_data_AE8[3] = {4.0f, 1.0f, 0.5f};
float lbl_17_data_AF4[3] = {0.5f, 0.75f, 1.2f};
Vec lbl_17_data_B00[3] =
{
    {0.0f, 140.0f, 0.0f},
    {0.0f, 500.0f, 0.0f},
    {0.0f, 600.0f, 0.0f},
};

void fn_17_4CC4(struct Ball *ball)
{
    struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];
    Vec up = {0.0f, 1.0f, 0.0f};
    Vec worldUp;
    Vec localDir;
    Vec dir;

    if ((lbl_1_bss_D4CD8 >> ball->playerId) & 1)
        player->flags |= 0x800;
    else
        player->flags &= ~0x800;
    player->unk84 = 0.0f;
    player->unk88 = 0.0f;
    player->unk8C = 0.0f;
    player->unk90 = 0;
    player->unk91 = 0;
    if (player->flags & 0x800)
    {
        s16 pitchErr;
        s16 yawErr;
        s16 pitch;
        struct Ball *target;
        Vec *targetPos;
        float speed;

        if (player->unkA2 > 0)
            player->unkA2--;
        if (player->unkA4 < 0)
            player->unkA4++;
        if (player->unkA6 > 0)
            player->unkA6--;
        if (player->unkA2 == 0)
        {
            player->unkA0++;
            if (player->unkA0 >= 3)
                player->unkA0 = 0;
            player->unkA2 = (lbl_80198B38 * 3) + (int)(2.0f * ((float)rand() / 32767.0f));
        }
        if (player->unkAA <= 0)
        {
            int i;
            int r = (rand() & 0x7FFF) % 6;

            player->unkA1 = -1;
            player->unkAE = 0;
            if (r < 4)
            {
                for (i = r; i < r + 4; i++)
                {
                    struct DogfightPlayer *other;
                    struct Ball *otherBall;
                    int idx = i % 4;

                    otherBall = &balls[idx];
                    other = &lbl_17_bss_48[idx];

                    if (otherBall->status == 2 && otherBall != ball && !(other->flags & 8))
                    {
                        player->unkA1 = (s8)idx;
                        if (other->flags & 0x800)
                            player->unkAA = lbl_80198B38 * 3;
                        else
                            player->unkAA = lbl_80198B38 * 6;
                        break;
                    }
                }
            }
            else
                player->unkAA = lbl_80198B34 * (7.0f * ((float)rand() / 32767.0f));
        }
        else
        {
            if (player->unkA1 != -1 && (lbl_17_bss_48[player->unkA1].flags & 8))
                player->unkAA = 0;
            player->unkAA--;
        }
        pitch = math_atan(-ball->unk30[1][2]);
        if (player->unkA1 != -1)
            target = &balls[player->unkA1];
        else
            target = NULL;
        if (player->unkA1 != -1)
            targetPos = &target->pos;
        else
            targetPos = &lbl_17_data_B00[lbl_17_bss_10];
        PSVECSubtract(targetPos, &ball->pos, &dir);
        mtxa_from_identity();
        mtxa_rotate_y(-ball->rotY);
        mtxa_tf_vec(&dir, &localDir);
        yawErr = math_atan2(-localDir.x, -localDir.z);
        pitchErr = math_atan2(localDir.y, math_sqrt(mathutil_sum_of_sq_2(localDir.x, localDir.z)));
        pitchErr -= pitch;
        speed = mathutil_vec_len(&ball->vel);
        mtxa_from_mtx(ball->unk30);
        mtxa_tf_vec(&up, &worldUp);
        if (ball->pos.y < 40.0f)
        {
            player->unk94 |= 4;
            player->unk94 &= ~8;
            player->unk94 &= ~3;
        }
        else
        {
            player->unk94 &= ~0xC;
            player->unk94 &= ~3;
            player->unk94 &= ~3;
            if (yawErr < -0x3A0)
            {
                player->unk94 |= 2;
                player->unk9C = (float)(-0x3A0 - yawErr) / 128.0f;
                if (player->unk9C > 1.0f)
                    player->unk9C = 1.0f;
            }
            else if (yawErr > 0x3A0)
            {
                player->unk94 |= 1;
                player->unk9C = (float)(yawErr - 0x3A0) / 128.0f;
                if (player->unk9C > 1.0f)
                    player->unk9C = 1.0f;
            }
            if (abs(yawErr) < 0x5800)
            {
                player->unk94 &= ~0xC;
                if (pitchErr < -0x1A0)
                {
                    player->unk94 |= 8;
                    player->unk98 = (float)(-0x1A0 - pitchErr) / 128.0f;
                    if (player->unk98 > 1.0f)
                        player->unk98 = 1.0f;
                }
                else if (pitchErr > 0x360)
                {
                    player->unk94 |= 4;
                    player->unk98 = (float)(pitchErr - 0x360) / 128.0f;
                    if (player->unk98 > 1.0f)
                        player->unk98 = 1.0f;
                }
            }
        }
        if (player->unk94 & 4)
        {
            if (ball->rotX < 0x2000)
            {
                float t = (float)(0x2000 - ball->rotX) / 128.0f;

                if (t > 1.0f)
                    t = 1.0f;
                player->unk88 = -0.9f * player->unk98 * t;
            }
            player->unk8C = 1.0f;
        }
        else if (player->unk94 & 8)
            player->unk88 = 0.9f * player->unk98;
        if (player->unk94 & 3)
        {
            float steer = player->unk9C * lbl_17_data_ADC[player->unkA0];

            player->unk84 = steer;
            if (player->unk94 & 4)
                player->unk84 *= 0.75f;
            else if ((player->unk94 & 8) && worldUp.y > 0.866f)
                player->unk84 *= 0.75f;
            else
            {
                float len = math_sqrt(mathutil_sum_of_sq_2(worldUp.x, worldUp.z));

                if (len > 0.5f)
                {
                    float k = 0.5f * steer;

                    player->unk88 = -len * k;
                }
                else
                {
                    player->unk88 = 0.3f * len;
                    player->unk88 -= 0.1f;
                }
            }
            if (player->unk94 & 1)
                player->unk84 *= -1.0f;
            if (worldUp.y < 0.0f && (player->unk94 & 4))
                player->unk84 = 0.0f;
        }
        if (speed < 0.27777779f)
            player->unk8C = 1.0f;
        else if (speed > 0.69444442f)
            player->unk8C = -0.8f;
        else if (!(player->unk94 & 8))
        {
            if (speed < 0.46296296f)
                player->unk8C = 0.75f;
            else
                player->unk8C = 0.0f;
        }
        else
            player->unk8C = -0.3f;
        if (player->unk61 >= 0 && !(lbl_17_bss_48[player->unk61].flags & 0x18)
         && (player->flags & 0x2000) && player->unkA4 >= 0)
        {
            player->unk90 = 1;
            player->unkA4++;
            if ((float)player->unkA4 > lbl_80198B34 * lbl_17_data_AF4[player->unkA0])
                player->unkA4 = (int)(-0.8f * lbl_80198B34);
        }
        if (player->unk1E[0] >= 0 && !(lbl_17_bss_48[player->unk1E[0]].flags & 0x18))
        {
            player->unkAC++;
            if ((float)player->unkAC >= 0.4f * lbl_80198B34 && player->unkA6 == 0)
            {
                player->unk91 = 1;
                player->unkAE++;
                if (player->unkAE >= 2)
                    player->unkA6 = lbl_80198B34 * (2.0f * lbl_17_data_AE8[player->unkA0]);
                else
                    player->unkA6 = lbl_80198B34 * lbl_17_data_AE8[player->unkA0];
            }
        }
        else
            player->unkAC = 0;
    }
}

void fn_17_57A0(struct Ball *ball)
{
    struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];
    Vec axisX = {1.0f, 0.0f, 0.0f};
    Vec axisY = {0.0f, 1.0f, 0.0f};
    Vec axisZ = {0.0f, 0.0f, 1.0f};
    Vec axisNegZ = {0.0f, 0.0f, -1.0f};
    Vec gravity;
    Vec forward;
    Vec localVel;
    Vec up;
    Vec right;
    Vec relVel;
    Vec localRelVel;
    Quat q;
    Quat q2;
    u8 accel = 0;
    u8 brake = 0;
    float stickY;
    float stickX;
    float triggerDiff = 0.0f;
    float dash;
    s16 angle;
    float k;
    float clamped;
    float speedScale;
    float yaw;
    s16 pathAngle;

    if (player->flags & 0x800)
    {
        stickX = 60.0f * player->unk84;
        stickY = 60.0f * player->unk88;
        if (player->unk8C > 0.0f)
            accel = 140.0f * player->unk8C;
        else if (player->unk8C < 0.0f)
            brake = 140.0f * -player->unk8C;
    }
    else
    {
        stickX = pad_status_groups[player->unk5E].unk0[0].stickX;
        stickY = pad_status_groups[player->unk5E].unk0[0].stickY;
        if (!(lbl_1_bss_D4CDC & 0x10))
        {
            brake = pad_status_groups[player->unk5E].unk0[0].triggerLeft;
            accel = pad_status_groups[player->unk5E].unk0[0].triggerRight;
        }
        else
        {
            if (pad_status_groups[player->unk5E].unk0[0].button & 0x200)
                brake = 140;
            if (pad_status_groups[player->unk5E].unk0[0].button & 0x100)
                accel = 140;
            {
                u8 left = pad_status_groups[player->unk5E].unk0[0].triggerLeft;
                u8 right = pad_status_groups[player->unk5E].unk0[0].triggerRight;

                triggerDiff = right - left;
            }
        }
    }
    if ((player->flags & 8) || !(player->flags & 0x400))
    {
        stickX = 0.0f;
        stickY = 0.0f;
        accel = 0;
        brake = 0;
        triggerDiff = 0.0f;
        player->unk80 = 0;
    }
    else if (player->flags & 0x200)
    {
        stickX *= 0.5f;
        stickY *= 0.5f;
    }
    if (player->unk80 != 0)
    {
        player->unk80--;
        if (stickX >= 0.0f)
            player->unk81 &= ~4;
        if (stickX <= 0.0f)
            player->unk81 &= ~8;
        if (player->unk81 == 0)
            player->unk80 = 0;
    }
    else
        player->unk81 = 0;
    if (stickX > 50.0f)
    {
        if (player->unk7C > 8)
            player->unk7E = 0;
        else
        {
            player->unk7C++;
            player->unk7E = 1;
        }
    }
    else
        player->unk7C = 0;
    if (stickX < -50.0f)
    {
        if (player->unk7D > 8)
            player->unk7F = 0;
        else
        {
            player->unk7D++;
            player->unk7F = 1;
        }
    }
    else
        player->unk7D = 0;
    if (player->unk7E != 0)
    {
        player->unk7E++;
        if (player->unk7E > 10)
            player->unk7E = 0;
    }
    if (player->unk7F != 0)
    {
        player->unk7F++;
        if (player->unk7F > 10)
            player->unk7F = 0;
    }
    if (player->unk7E != 0 && player->unk7F != 0)
    {
        player->unk7E = 0;
        player->unk7F = 0;
        if (stickX > 0.0f)
            player->unk81 |= 8;
        else
            player->unk81 |= 4;
        player->unk80 = lbl_80198B34;
    }
    dash = (float)player->unk80 / 20.0f;
    if (dash > 1.0f)
        dash = 1.0f;
    stickX *= 1.0f + 1.3f * dash;
    {
        float absX = __fabs(stickX);
        float absY = __fabs(stickY);

        angle = math_atan2(absY, absX);
    }
    if (angle > 0x2000)
        angle = 0x4000 - angle;
    k = 1.0f - (0.29289323f * (float)angle) / 8192.0f;
    if (stickY > 0.0f)
    {
        stickX *= k;
        stickY *= k;
    }
    stickY -= 9.0f;
    mtxa_from_mtx(ball->unk30);
    mtxa_tf_vec(&axisNegZ, &forward);
    mtxa_tf_vec(&axisY, &up);
    mtxa_tf_vec(&axisX, &right);
    mtxa_rigid_inv_tf_vec(&ball->vel, &localVel);
    PSVECSubtract(&ball->vel, &lbl_17_bss_18, &relVel);
    player->unk8 = mathutil_vec_dot_prod(&relVel, &forward);
    if ((ball->rotZ > 0 && stickX < 0.0f) || (ball->rotZ < 0 && stickX > 0.0f))
        stickY -= math_sqrt(mathutil_sum_of_sq_2(up.x, up.z)) * (0.5 * __fabs(stickX));
    if (player->unk8 < 0.09259259f)
        clamped = 0.09259259f;
    else if (player->unk8 > 1.3888888f)
        clamped = 1.3888888f;
    else
        clamped = player->unk8;
    speedScale = 0.46296296f / clamped;
    speedScale *= math_sqrt(speedScale);
    if (speedScale > 1.0f)
        speedScale = 1.0f;
    stickY *= speedScale;
    stickX *= 0.9f;
    triggerDiff *= 2.8f;
    quat_from_axis_angle(&q, &axisX, -5.0f * stickY);
    if (!(lbl_1_bss_D4CDC & 0x10) || (player->flags & 0x800))
    {
        yaw = -9.0f * stickX;
        triggerDiff = -2.0f * stickX;
    }
    else
    {
        yaw = (-7.0f * stickX) - triggerDiff;
        triggerDiff = -2.0f * stickX;
    }
    quat_from_axis_angle(&q2, &axisZ, yaw);
    quat_mult(&q, &q, &q2);
    quat_from_axis_angle(&q2, &axisY, triggerDiff);
    quat_mult(&q, &q, &q2);
    mtxa_from_quat(&q);
    mtxa_mult_left(ball->unk30);
    mtxa_sq_to_mtx(ball->unk30);
    if (player->unk4C > 0.0f)
    {
        Vec spinAxis = {0.0f, 1.0f, 1.0f};

        player->unk4C *= 0.999f;
        g_math_unk9_smth_w_quats(&q, &spinAxis, player->unk4C);
        mtxa_from_quat(&q);
        mtxa_mult_left(ball->unk30);
        mtxa_sq_to_mtx(ball->unk30);
    }
    if (!(player->flags & 8))
    {
        float a;

        a = -mathutil_vec_dot_prod(&localVel, &axisX);
        quat_from_axis_angle(&q, &axisY, 1800.0f * a);
        a = mathutil_vec_dot_prod(&localVel, &axisY);
        a *= 0.5f;
        quat_from_axis_angle(&q2, &axisX, 5000.0f * a);
        quat_mult(&q, &q, &q2);
        a = -mathutil_vec_dot_prod(&axisY, &right);
        if (!(player->unk81 & 0xC))
            a *= 1.0f - dash;
        quat_from_axis_angle(&q2, &axisZ, 500.0f * a);
        quat_mult(&q, &q, &q2);
        a = -mathutil_vec_dot_prod(&axisY, &forward);
        if (stickY > 0.0f)
            a *= 1.0 - (0.5 * __fabs(stickY)) / 60.0;
        if (up.y < -0.0f)
        {
            if (a > 0.0f)
                a = 0.0f;
            else if (up.y < -0.5f)
                a *= up.y;
            else
                a *= 1.0f + 3.0f * up.y;
        }
        quat_from_axis_angle(&q2, &axisX, 210.0f * a);
        quat_mult(&q, &q, &q2);
        mtxa_from_quat(&q);
        mtxa_mult_left(ball->unk30);
        mtxa_sq_to_mtx(ball->unk30);
    }
    mtxa_from_mtx(ball->unk30);
    mtxa_rigid_inv_tf_vec(&relVel, &localRelVel);
    pathAngle = math_atan2(localRelVel.y, -localRelVel.z);
    player->unk4 = -math_sin(pathAngle * 2);
    if (pathAngle > 0x4000 || pathAngle < -0x4000)
        player->unk4 *= -1.0f;
    player->unk4 = 0.2f * (player->unk10 / 20.0f * (player->unk4 * (player->unk8 * player->unk8)));
    PSVECScale(&up, &up, player->unk4);
    if (brake != 0)
    {
        Vec hVel;

        hVel.x = ball->vel.x;
        hVel.y = 0.0f;
        hVel.z = ball->vel.z;
        if (mathutil_vec_len(&hVel) > 0.18518518f)
        {
            float f = (float)brake;

            f = 1.0f - 0.008f * (f / 140.0f);
            ball->vel.x *= f;
            ball->vel.z *= f;
        }
    }
    if (accel != 0 && !(player->flags & 0x200))
    {
        if (mathutil_vec_len(&ball->vel) < 0.87962961f)
        {
            Vec thrust;
            float f = (float)accel;
            float t;

            f = 0.85f * (f / 140.0f);
            t = (0.8425926f - player->unk8) / 0.37037036f;

            if (t > 1.0f)
                t = 1.0f;
            f *= t;
            PSVECScale(&forward, &thrust, f / 216.0f);
            PSVECScale(&thrust, &thrust, fn_17_6B58(ball));
            PSVECAdd(&ball->vel, &thrust, &ball->vel);
        }
        if (accel > brake && player->unk14 == 0)
            fn_17_7454(ball, player);
    }
    if (!(player->flags & 8) && player->unk14 == 0 && player->unk8 < 0.46296296f)
    {
        Vec thrust;
        float f = (0.46296296f - player->unk8) / 0.46296296f;

        f *= 0.8f;
        f *= (140.0f - (float)brake) / 140.0f;
        PSVECScale(&forward, &thrust, f / 216.0f);
        PSVECScale(&thrust, &thrust, fn_17_6B58(ball));
        PSVECAdd(&ball->vel, &thrust, &ball->vel);
    }
    if (player->unk78 != -1)
    {
        float f = (float)(accel - brake) / 140.0f;

        f = 8192.0f + 8192.0f * (0.9f * f);
        SoundDop(player->unk78, f);
    }
    if (player->unk18 != 0)
    {
        Vec boost;

        player->unk18--;
        PSVECScale(&forward, &boost, 0.0092592593f);
        PSVECAdd(&ball->vel, &boost, &ball->vel);
    }
    gravity.x = 0.0f;
    gravity.y = -(9.8f / (lbl_80198B34 * lbl_80198B34));
    gravity.z = 0.0f;
    if (ball->flags & 0x100)
        gravity.y = 0.0f;
    fn_17_6820(ball);
    PSVECScale(&up, &up, fn_17_6B58(ball));
    if (!(player->flags & 8))
        PSVECAdd(&gravity, &up, &gravity);
    if (mathutil_vec_sq_len(&ball->vel) > 0.69830775f)
    {
        PSVECScale(&ball->vel, &ball->vel, 0.9991f);
        if (mathutil_vec_sq_len(&ball->vel) > 9.852623f)
            vec_set_len(3.1388888f, &ball->vel, &ball->vel);
    }
    PSVECAdd(&ball->vel, &gravity, &ball->vel);
    if (lbl_17_bss_14 == 13 && !(player->flags & 0x8208))
    {
        player->unk48 += 0.33333334f * mathutil_vec_len(&ball->vel);
        if (player->unk48 > 1.0f)
        {
            int i = (int)player->unk48;

            player->unk48 -= (float)i;
            lbl_17_bss_314[ball->playerId]++;
            if (lbl_17_bss_314[ball->playerId] > 9999999)
                lbl_17_bss_314[ball->playerId] = 9999999;
        }
    }
}

void fn_17_6820(struct Ball *ball)
{
    Vec center;
    Vec cross;
    Vec dir;
    Vec push;
    Vec yAxis;
    Vec localAxis;
    Quat q;
    Vec bounce;
    float radius;
    float d;
    float vn;
    s16 rot = 0;

    switch (current_stage_id)
    {
    case 0x98:
    case 0x9B:
        radius = 1100.0f;
        {
            Vec c = {0.0f, 0.0f, 0.0f};
            Vec *p = &c;

            center = *p;
        }
        break;
    case 0x99:
        radius = 600.0f;
        {
            Vec c = {0.0f, 650.0f, 0.0f};
            Vec *p = &c;

            center = *p;
        }
        break;
    case 0x9A:
    default:
        return;
    }
    PSVECSubtract(&ball->pos, &center, &dir);
    switch (current_stage_id)
    {
    case 0x98:
    case 0x9B:
        dir.y = 0.0f;
        break;
    }
    d = mathutil_vec_len(&dir) - radius;
    if (d <= 0.0f)
        return;
    vec_normalize_len(&dir);
    if (d > 5.0f)
        d = 5.0f;
    PSVECScale(&dir, &push, -d);
    PSVECAdd(&ball->pos, &push, &ball->pos);
    vn = mathutil_vec_dot_prod(&dir, &ball->vel);
    if (vn > 0.0f)
    {
        rot = 512.0f * vn;
        mathutil_vec_cross_prod(&dir, &ball->vel, &cross);
        if (cross.y < 0.0f)
            rot *= -1;
    }
    if (rot != 0)
    {
        static const Vec uy = {0.0f, 1.0f, 0.0f};

        yAxis = uy;
        mtxa_from_mtx(ball->unk30);
        mtxa_rigid_inv_tf_vec(&yAxis, &localAxis);
        quat_from_axis_angle(&q, &localAxis, rot);
        mtxa_from_quat(&q);
        mtxa_mult_left(ball->unk30);
        mtxa_sq_to_mtx(ball->unk30);
        mtxa_from_identity();
        mtxa_rotate_y(rot);
        mtxa_tf_vec(&ball->vel, &ball->vel);
    }
    vn = mathutil_vec_dot_prod(&dir, &ball->vel);
    if (vn > 0.0f)
    {
        PSVECScale(&dir, &bounce, 0.5f * -vn);
        PSVECAdd(&ball->vel, &bounce, &ball->vel);
    }
}

float fn_17_6B58(struct Ball *ball)
{
    float k;
    float h;
    float ratio;

    switch (current_stage_id)
    {
    case 0x98:
    case 0x9B:
        h = 400.0f;
        k = 100.0f;
        break;
    case 0x99:
    case 0x9A:
        h = 1000.0f;
        k = 200.0f;
        break;
    default:
        return 1.0f;
    }
    h -= ball->pos.y;
    if (h < 0.0f)
        h = 0.0f;
    ratio = h / k;
    if (ratio >= 1.0f)
        return 1.0f;
    return ratio;
}

void fn_17_6BE0(struct Ball *ball)
{
    float unused = 0.011f;

    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ULL;
}

void fn_17_6C64(struct Ball *ball, struct PhysicsBall *physBall)
{
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    fn_17_6D68(ball, physBall);
    fn_17_6DB8(physBall, stagedef);
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
    }
}

void fn_17_6D68(struct Ball *ball, struct PhysicsBall *physBall)
{
    init_physicsball_from_ball(ball, physBall);
    physBall->friction = 0.025f;
    physBall->radius = 0.9f;
}

void fn_17_6DB8(struct PhysicsBall *physBall, struct StageDef *stage)
{
    collide_ball_with_stage(physBall, stage);
}

void fn_17_6DD8(void)
{
    struct PhysicsBall a;
    struct PhysicsBall b;
    Vec prevDiff;
    Vec velDiff;
    Vec sep;
    Vec normal;
    s8 vol;
    s8 pan;
    int i;
    int j;
    struct Ball *saved;
    struct Ball *ballA;
    struct Ball *ballB;
    struct DogfightPlayer *playerA;
    struct DogfightPlayer *playerB;

    saved = current_ball;
    for (i = 0; i < 3; i++)
    {
        ballA = &balls[i];
        if (ballA->status == 2)
        {
            playerA = &lbl_17_bss_48[i];
            init_physicsball_from_ball(ballA, &a);
            a.flags |= (ballA->flags & 1) ? 1 : 0;
            for (j = i + 1; j < 4; j++)
            {
                ballB = &balls[j];
                if (ballB->status == 2)
                {
                    float overlap;

                    playerB = &lbl_17_bss_48[j];
                    init_physicsball_from_ball(ballB, &b);
                    b.flags |= (ballB->flags & 1) ? 1 : 0;
                    PSVECSubtract(&b.prevPos, &a.prevPos, &prevDiff);
                    PSVECSubtract(&a.vel, &b.vel, &velDiff);
                    PSVECSubtract(&a.pos, &b.pos, &normal);
                    if (mathutil_vec_dot_prod(&prevDiff, &velDiff) > 0.0f
                     && fn_1_A74E4(&a.prevPos, &a.pos, a.radius, &b.prevPos, &b.pos, b.radius) != 0)
                    {
                        float e1 = mathutil_vec_sq_len(&a.vel);
                        float e2 = mathutil_vec_sq_len(&b.vel);

                        fn_17_724C(&normal, &a.vel, playerA->unk50, &b.vel, playerB->unk50);
                        e1 = (float)__fabs(mathutil_vec_sq_len(&a.vel) - e1);
                        e2 = (float)__fabs(mathutil_vec_sq_len(&b.vel) - e2);
                        e2 = e1 + e2;
                        PSVECAdd(&a.pos, &a.vel, &a.pos);
                        PSVECAdd(&b.pos, &b.vel, &b.pos);
                        if (e2 > 0.001f)
                        {
                            s8 span;
                            int id;
                            s16 pitch;

                            g_rumble_controller(playerA->unk5E, 1, 10);
                            g_rumble_controller(playerB->unk5E, 1, 10);
                            current_ball = ballA;
                            call_SoundReqID_arg_0(0x17);
                            current_ball = ballB;
                            call_SoundReqID_arg_0(0x18);
                            span = fn_1_2F8F0(modeCtrl.playerCount, &current_ball->pos, &vol, &pan);
                            id = call_SoundReqID_arg_1_copy(((pan & 0x7F) << 25) | (((span & 0x7F) << 11) | 0x15B | ((vol & 0x7F) << 18)));
                            e2 *= 6.6666665f;
                            if (e2 > 1.0f)
                                e2 = 1.0f;
                            pitch = (int)(131071.0f * e2) + 0x2000;
                            SoundPitch(id, pitch);
                        }
                    }
                    overlap = vec_normalize_len(&normal);
                    overlap -= 0.01f;
                    overlap = (a.radius + b.radius) - overlap;
                    if (overlap > 0.0f)
                    {
                        PSVECScale(&normal, &sep, 0.5f * overlap);
                        PSVECAdd(&a.pos, &sep, &a.pos);
                        PSVECSubtract(&b.pos, &sep, &b.pos);
                    }
                    apply_physicsball_to_ball(ballB, &b);
                }
            }
            apply_physicsball_to_ball(ballA, &a);
        }
    }
    current_ball = saved;
}

void fn_17_724C(Vec *normal, Vec *vel1, float m1, Vec *vel2, float m2)
{
    Vec v1;
    Vec v2;
    Vec n;
    float d;
    float e;
    float f;
    float g;

    v1 = *vel1;
    v2 = *vel2;
    n = *normal;
    d = mathutil_vec_dot_prod(&n, &v1);
    e = mathutil_vec_dot_prod(&n, &v2);
    f = 1.5 * ((e - d) * ((m1 * m2) / ((m1 * m2) + (m1 * m1))));
    g = f * -(m1 / m2);
    v1.x += f * n.x;
    v1.y += f * n.y;
    v1.z += f * n.z;
    v2.x += g * n.x;
    v2.y += g * n.y;
    v2.z += g * n.z;
    *vel1 = v1;
    *vel2 = v2;
}

int fn_17_73E0(void)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != 0
         && (pad_status_groups[player_pad_map[i]].unk0[2].button & 0x100))
            return 1;
    }
    return 0;
}

void fn_17_7454(struct Ball *ball, struct DogfightPlayer *player)
{
    struct Effect effect;
    Vec pos;
    Vec vel;
    Vec tmp;
    s16 i;

    if (!(player->flags & 0x800))
    {
        memset(&effect, 0, sizeof(effect));
        effect.type = 0x2B;
        effect.playerId = ball->playerId;
        effect.unk18 = 0.1f;
        effect.unk1C = 0.1f;
        effect.unk20 = 0.1f;
        mtxa_from_quat(&ball->ape->rot);
        mtxa_tf_vec_xyz(0.0f, -0.5f, -1.0f, &pos);
        PSVECAdd(&pos, &ball->pos, &pos);
        PSVECScale(&ball->vel, &vel, 0.75f);
        for (i = 5; i > 0; i--)
        {
            {
                Vec r = {0.0f, 0.0f, 0.0f};
                Vec *p;

                r.x = (float)rand() / 32767.0f;
                r.y = (float)rand() / 32767.0f;
                r.z = (float)rand() / 32767.0f;
                p = &r;
                effect.vel = *p;
            }
            PSVECScale(&effect.vel, &effect.vel, 1.5f);
            {
                Vec half = {0.75f, 0.75f, 0.75f};
                Vec *q = &half;

                PSVECSubtract(&effect.vel, q, &effect.vel);
            }
            PSVECScale(&effect.vel, &effect.vel, 0.1 * (5.0 * ((float)rand() / 32767.0f)));
            PSVECAdd(&effect.vel, &vel, &effect.vel);
            PSVECScale(&effect.vel, &tmp, 2.0f);
            PSVECAdd(&pos, &tmp, &effect.pos);
            spawn_effect(&effect);
        }
    }
}

void fn_17_76A0(void)
{
    int i;

    for (i = 0; i < lbl_17_bss_28; i++)
        preload_ape(lbl_17_bss_40[i], 0, 1, 0x1A);
}

void fn_17_770C(void)
{
    preload_ape(lbl_17_bss_40[0], 1, 0, 0);
    queue_stage_load(200);
}
