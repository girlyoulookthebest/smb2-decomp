/* Ball physics, ape (monkey) orientation inside the ball, and ball drawing
 * (SMB1 ball.c). */
#include "types.h"
#include "macros.h"
#include "mathutil.h"
#include "ball.h"
#include "mode.h"
#include "stcoli.h"
#include "pool.h"
#include "libc.h"
#include "stage.h"
#include "dip.h"
#include "recplay.h"
#include "light.h"
#include "rand.h"
#include "effect.h"
#include "world.h"
#include "camera.h"
#include "input.h"

/* .data */
u8 lbl_1_data_2E1E8[0x170] =
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
float physical_ball_size[] = { 0.5f, 0.009799991734325886f, 0.5f, 0.5f, 0.021777760237455368f, 0.1f };
/* per game type ball flags (0x80: challenge mode) */
u16 lbl_1_data_2E370 = 0x01FE;
u16 lbl_1_data_2E372 = 0x017E;
u16 lbl_1_data_2E374 = 0x0036;
u16 lbl_1_data_2E376 = 0x017E;
u16 lbl_1_data_2E378 = 0x017E;
u16 lbl_1_data_2E37A = 0x01FE;
u16 lbl_1_data_2E37C = 0x017E;
u16 lbl_1_data_2E37E = 0x0060;
u16 lbl_1_data_2E380 = 0x0076;
u16 lbl_1_data_2E382 = 0x017E;
u16 lbl_1_data_2E384 = 0x017E;

/* .bss (0x700B8 - 0x70F28), addressed from one base register: keep complete and in order */
u16 *g_effect_flag_ptr;
u16 lbl_1_bss_700BC;
struct Ball balls[8];
Mtx lbl_1_bss_70E40;
float lbl_1_bss_70E70[8];
struct Ball *current_ball;
void (*lbl_1_bss_70E94)(void);
s32 lbl_1_bss_70E98[16];
s32 active_monkey_id[4];
s32 lbl_1_bss_70EE8;
void (*g_some_menu_func_1)(struct Ball *);
s32 player_pad_map[4];
s32 lbl_1_bss_70F00[10];  /* per-player idle-chatter counters */

FORCE_BSS_ORDER(g_effect_flag_ptr)
FORCE_BSS_ORDER(lbl_1_bss_700BC)
FORCE_BSS_ORDER(balls)
FORCE_BSS_ORDER(lbl_1_bss_70E40)
FORCE_BSS_ORDER(lbl_1_bss_70E70)
FORCE_BSS_ORDER(current_ball)
FORCE_BSS_ORDER(lbl_1_bss_70E94)
FORCE_BSS_ORDER(lbl_1_bss_70E98)
FORCE_BSS_ORDER(active_monkey_id)
FORCE_BSS_ORDER(lbl_1_bss_70EE8)
FORCE_BSS_ORDER(g_some_menu_func_1)
FORCE_BSS_ORDER(player_pad_map)
FORCE_BSS_ORDER(lbl_1_bss_70F00)

void g_something_with_translating_items(int a, Vec *b, float c);
void mtxa_to_quat(Quat *q);
void fn_1_D1FB8(struct Ape *ape, int b);
void fn_1_113190(struct Ape *ape, int a, float speed, int b, int c);
struct ApeFlagInfo *fn_1_1156F0(void *a, int b, int c);

void fn_1_D2510(struct Ape *ape);
void fn_1_67BE8(void);
void fn_1_D250C(float a);
void set_global_LOD(int a);
void challenge_mode_init(void);
void g_reset_ball(struct Ball *ball);
void g_ape_anim_head(struct Ape *ape, Vec *lookAt);
void g_maybe_sets_number_of_starting_monkeys(struct Ball *ball);
void fn_1_D1F7C(int a);
struct Ape *load_ape_gameplay(int chara, int lod, int variant);
void g_something_with_freeing_chara_heap_ape(struct Ape *ape);
struct Ape *load_ape_gameplay_id_only(int chara);
extern void (*ball_mode_funcs[])(struct Ball *);
void g_some_ballfunc(struct Ball *ball);
void ball_movement_sparks(struct Ball *ball);
void ball_collision_stars(struct Ball *ball);
void g_ball_ape_rotation(struct Ball *ball);
void spawn_postgoal_ball_sparkle(void);
void set_visual_scale(struct Ball *ball);
void ball_sounds_gameplay(struct Ball *ball);
void fn_1_67C18(void);
void fn_1_67C5C(void);
void fn_1_D29E0(int);
void g_banana_disp_efc_req(s8);
void create_1up_sprite(int);
void call_SoundReqID_arg_0(int);
void SoundIcsReq(u32 soundId, u8 intensity, s8 volume);
extern u32 g_font_char_var_3;  /* global frame counter */

static inline void vec_set(Vec *v, float x, float y, float z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

#define FLT_EPSILON 1.1920928955078125e-07f

extern u32 g_some_other_flags;
extern s8 number_of_starting_monkeys;
int get_storymode_score(void);
int get_storymode_banana_count(void);
void set_ball_properties(struct Ball *ball, int arg);
void fn_1_487C8(struct Ball *ball);
void set_ape_anim(struct Ape *ape, int anim);
void g_apply_ball_velocity(struct Ball *ball, struct PhysicsBall *physBall);
void handle_ball_stage_coli(struct Ball *ball, struct PhysicsBall *physBall);
void fn_1_4AF48(struct Ball *ball, struct PhysicsBall *physBall);
void position_ball(struct Ball *ball, struct PhysicsBall *physBall);
void g_some_set_ape_anim(struct Ape *ape, int a, int b);
int fn_1_4CD70(void);
void ball_physics_g_something_w_postgoal_blast_up(struct Ball *ball);
void g_ball_mode_play_replay(struct Ball *ball);
void g_smth_with_ape_rotation(struct Ape *ape, Vec *dir);
void fn_1_10D34(struct Ball *ball);
void g_rumble_controller(int pad, int a, int b);
extern s32 lbl_80198B38;
extern struct BgLightInfo lbl_1_bss_2F48;  /* light.c's s_bgLightInfo */

void fn_1_48654(struct Ball *ball);
void fn_1_486A0(struct Ball *ball);
void fn_1_487C8(struct Ball *ball);
void g_competition_mode_respawn_start(struct Ball *ball);
void fn_1_48BCC(struct Ball *ball);
void ball_physics_g_something_w_postgoal_slowdown(struct Ball *ball);
void ball_physics_g_something_w_postgoal_blast_up(struct Ball *ball);
void fn_1_48ED8(struct Ball *ball);
void g_ball_mode_play_replay(struct Ball *ball);
void fn_1_49470(struct Ball *ball);
void fn_1_49670(struct Ball *ball);
void fn_1_49674(struct Ball *ball);
void fn_1_49878(struct Ball *ball);
void fn_1_4987C(struct Ball *ball);
void fn_1_49CCC(struct Ball *ball);
void fn_1_49BD4(struct Ball *ball);
void g_smth_with_mystery_3(struct Ball *ball);
void fn_1_49EDC(struct Ball *ball);
void g_handle_competition_mode_respawn(struct Ball *ball);
void fn_1_4A18C(struct Ball *ball);
void fn_1_4A1AC(struct Ball *ball);
void ball_physics_g_something_w_poastgoal_slowdown_blast_up(struct Ball *ball);
void ball_physics_g_something_w_postgoal_blast_up2(struct Ball *ball);
void fn_2_71BC(struct Ball *ball);
void g_challenge_mode_start(struct Ball *ball);
void fn_2_73B4(struct Ball *ball);
void challenge_mode_physics(struct Ball *ball);

void (*ball_mode_funcs[])(struct Ball *) =
{
    fn_1_48654,
    fn_1_486A0,
    fn_1_487C8,
    g_competition_mode_respawn_start,
    fn_1_48BCC,
    ball_physics_g_something_w_postgoal_slowdown,
    ball_physics_g_something_w_postgoal_blast_up,
    fn_1_48ED8,
    g_ball_mode_play_replay,
    fn_1_48ED8,
    g_ball_mode_play_replay,
    fn_1_49470,
    fn_1_49670,
    fn_1_49674,
    fn_1_49878,
    fn_1_4987C,
    fn_1_49CCC,
    fn_1_49BD4,
    g_smth_with_mystery_3,
    fn_1_49EDC,
    g_handle_competition_mode_respawn,
    fn_1_4A18C,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_1_4A1AC,
    ball_physics_g_something_w_poastgoal_slowdown_blast_up,
    ball_physics_g_something_w_postgoal_blast_up2,
    fn_2_71BC,
    g_challenge_mode_start,
    fn_2_73B4,
    challenge_mode_physics,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_1_4A1AC,
};

Vec lbl_1_data_2E418[10] =
{
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.30000001192092896f, 1.0f },
    { 1.0f, 0.8999999761581421f, 0.0f },
    { 0.0f, 0.800000011920929f, 0.0f },
    { 0.0f, 0.800000011920929f, 0.800000011920929f },
    { 1.0f, 0.0f, 0.800000011920929f },
    { 0.20000000298023224f, 0.4000000059604645f, 0.4000000059604645f },
    { 1.0f, 1.0f, 0.8999999761581421f },
    { 0.699999988079071f, 0.0f, 1.0f },
    { 0.699999988079071f, 1.0f, 0.0f },
};

u32 lbl_1_data_2E490[110] =
{
    0xFFFF3C3C, 0xFF5454F0, 0xFFECEC14, 0xFF14C814, 0xFF14C8DC, 0xFFE614C8,
    0xFFA0A0A0, 0xFFE6E6E6, 0x0000001A, 0x40000000, 0x41700000, 0x3DCCCCCD,
    0x00000004, 0x00000002, 0x00000015, 0x40000000, 0x41700000, 0x3DCCCCCD,
    0x00000005, 0x00000003, 0x0000001B, 0x3F800000, 0x40A00000, 0x3D4CCCCD,
    0x00000000, 0x00000000, 0x00000016, 0x3F800000, 0x40A00000, 0x3D4CCCCD,
    0x00000000, 0x00000000, 0x00000018, 0x40400000, 0x428C0000, 0x3D4CCCCD,
    0x00000006, 0x00000000, 0x00000013, 0x40400000, 0x428C0000, 0x3D4CCCCD,
    0x00000007, 0x00000000, 0x00000017, 0x40000000, 0x42480000, 0x3D4CCCCD,
    0x00000000, 0x00000000, 0x00000012, 0x40000000, 0x42480000, 0x3D4CCCCD,
    0x00000000, 0x00000000, 0x00000005, 0x41000000, 0x41A00000, 0x3E8F5C29,
    0x00000009, 0x00000000, 0x00000003, 0x40000000, 0x41A00000, 0x3DCCCCCD,
    0x00000000, 0x00000000, 0x0000000A, 0x3F800000, 0x42200000, 0x3D4CCCCD,
    0x0000000B, 0x00000000, 0x00000008, 0x40000000, 0x42200000, 0x3D4CCCCD,
    0x0000000C, 0x00000000, 0x00000007, 0x40000000, 0x41A00000, 0x3D4CCCCD,
    0x00000000, 0x00000000, 0x0000000F, 0x3F800000, 0x42200000, 0x3D4CCCCD,
    0x0000000E, 0x00000000, 0x0000000D, 0x40000000, 0x42200000, 0x3D4CCCCD,
    0x0000000F, 0x00000000, 0x0000000C, 0x40000000, 0x41A00000, 0x3D4CCCCD,
    0x00000000, 0x00000000, 0x00000010, 0x40800000, 0x42A00000, 0x3E19999A,
    0x00000000, 0x00000000,
};

void g_ape_upright(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    Quat quat;
    Vec sp44;
    Vec sp38 = {0.0f, 1.0f, 0.0f};
    float f1;

    ball->unkB0.w /= 0.65f;
    ball->unkB0.x *= 0.65f;
    ball->unkB0.y *= 0.65f;
    ball->unkB0.z *= 0.65f;

    quat_normalize(&ball->unkB0);
    mtxa_from_quat(&ball->unkB0);
    mtxa_to_mtx(lbl_1_bss_70E40);
    mtxa_from_quat(&ape->rot);
    mtxa_normalize_basis();
    mtxa_mult_left(lbl_1_bss_70E40);
    mtxa_normalize_basis();

    if (!(ball->flags & 0x20))
    {
        float f31 = mathutil_vec_len(&ball->vel);
        float f1 = mathutil_vec_len(&ball->unkC0);

        if (f31 > 0.032407406717538834f)
        {
            if (f1 * 100.0f < ball->unkCC)
                ball->flags |= 0x20;
            else if (f1 * 3.0f < ball->unkCC && f31 * 1.5f < ball->speed)
                ball->flags |= 0x20;
        }
    }

    if (!(ape->flags & (1 << 17)) && ball->unkB0.w < 0.9941)
        return;

    mtxa_to_mtx(lbl_1_bss_70E40);

    sp44 = (Vec){0.0f, 1.0f, 0.0f};
    mtxa_rigid_inv_tf_vec(&sp44, &sp44);

    f1 = 1.0f - vec_dot_normalized(&sp44, &sp38);
    if (!(ape->flags & (1 << 17)) && f1 > 0.01f)
    {
        Vec sp2C = {0.0f, 1.0f, 0.0f};
        if (f1 > 1.999f)
            sp44 = (Vec){1.0f, 0.0f, 0.0f};
        else
            mathutil_vec_cross_prod(&sp2C, &sp44, &sp44);
        quat_from_axis_angle(&quat, &sp44, 0x38E);
    }
    else
    {
        quat_from_dirs(&quat, &(Vec){0.0f, 1.0f, 0.0f}, &sp44);
    }

    quat_normalize(&quat);
    mtxa_from_quat(&quat);
    mtxa_normalize_basis();
    mtxa_mult_left(lbl_1_bss_70E40);
}

float fn_1_45A8C(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    Vec sp48;
    Vec sp3C;
    Quat sp2C;

    sp48 = ball->vel;
    sp48.y = 0.0f;

    if (mathutil_vec_len(&sp48) < 0.00027777777f)
        return 0.0f;

    vec_normalize_len(&sp48);
    mtxa_rigid_inv_tf_vec(&sp48, &sp48);

    sp3C = (Vec){0.0f, 0.0f, 0.0f};

    if (sp48.z > -0.992f)
    {
        Vec sp20 = {0.0f, 0.0f, -1.0f};

        if (1.0f - sp48.z > 9.9999999392252903e-09f)
            mathutil_vec_cross_prod(&sp20, &sp48, &sp3C);
        else
        {
            sp3C.x = 0.0f;
            sp3C.y = 1.0f;
            sp3C.z = 0.0f;
        }
        quat_from_axis_angle(&sp2C, &sp3C, 0x2D8);
    }
    else
    {
        quat_from_dirs(&sp2C, &(Vec){0.0f, 0.0f, -1.0f}, &sp48);
    }

    mtxa_push();
    mtxa_from_quat(&sp2C);
    mtxa_normalize_basis();
    mtxa_to_mtx(lbl_1_bss_70E40);
    mtxa_pop();
    mtxa_mult_right(lbl_1_bss_70E40);
    return mathutil_vec_len(&ball->unkC0) * 1.5f;
}

void fn_1_45CA8(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    struct RaycastHit sp3C;
    struct RaycastHit sp20;
    Vec sp14;
    Vec sp8;
    Vec sp58[] =
    {
        {0.3f,   0.0f, 0.0f},
        {-0.25f, 0.0f, 0.0f},
        {0.1f,   0.0f, -0.2f},
        {0.1f,   0.0f, 0.2f},
    };
    int r28;
    u32 i;

    mtxa_push();
    sp14.x = 1.0f;
    sp14.y = 0.0f;
    sp14.z = 0.0f;
    mtxa_tf_vec(&sp14, &sp14);
    r28 = -math_atan2(sp14.z, sp14.x) - 32768;
    raycast_stage_down(&ball->pos, &sp3C, NULL);
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(r28);
    ball->flags &= ~2;

    for (i = 0; i < 4; i++)
    {
        mtxa_push();
        mtxa_tf_point(&sp58[i], &sp14);
        raycast_stage_down(&sp14, &sp20, NULL);
        mtxa_pop();
        if (sp20.flags == 0 || sp20.pos.y < sp3C.pos.y - 1.0)
        {
            ball->flags |= 2;
            sp8.x = sp14.x;
            sp8.y = ball->pos.y - 1.0;
            sp8.z = sp14.z;
            ape->unk27C = i;
            g_something_with_translating_items(3, &sp8, 0.5f);
            break;
        }
    }
    mtxa_pop();
}

int fn_1_45EAC(struct Ape *ape, struct Ball *ball)
{
    int ret;
    int var1;
    float var2;

    if (mode_info.unk22 != 1)
        ret = 8;
    else if (mode_info.timerCurr > 0.8 * mode_info.timerMax)
        ret = 8;
    else if (mode_info.timerCurr > 0.6 * mode_info.timerMax)
        ret = 6;
    else if (mode_info.timerCurr > 0.4 * mode_info.timerMax)
        ret = 4;
    else if (mode_info.timerCurr > 0.2 * mode_info.timerMax)
        ret = 2;
    else
        ret = 0;

    if (ape->unk6C != 7)
        return ret;

    switch (modeCtrl.gameType)
    {
    case 1:
        var1 = ball->winStreak;
        var2 = var1 * 0.2f;
        if (var2 > 3.0f)
            var2 = 3.0f;
        break;
    default:
        var1 = 0;
        var2 = 0.0f;
        break;
    }

    if (!(ape->flags & (1 << 13)))
    {
        if (ape->unk2CA > var1 || (ape->flags & (1 << 14)))
        {
            ape->flags |= 0x5000;
            ret++;
        }
    }
    else
    {
        ape->flags |= 0x1000;
        mtxa_push();
        mtxa_from_quat(&ape->rot);
        mtxa_rotate_y(DEGREES_TO_S16(20.0f + 12.0f * var2));
        mtxa_to_quat(&ape->rot);
        mtxa_pop();
        ret++;
    }
    return ret;
}

static inline int fn_1_461C0_inline(struct Ball *ball)
{
    int i;

    if (!(ball->flags & 0x1000))
        return 0;

    for (i = 0; i < g_poolInfo.playerPool.count; i++)
    {
        if (ball->rank - 1 == balls[i].rank
         && (u16)abs(ball->goalTime - balls[i].goalTime) < 30)
            return 1;
    }

    return 0;
}

void fn_1_461C0(struct Ape *ape, float speed)
{
    struct Ball *ball = &balls[ape->ballIdx];
    int a;
    int b;
    int c = 2;

    if (modeCtrl.gameType == 8)
        return;

    if (sub_mode == 0x4D || sub_mode == 0x4E)
    {
        speed = ape->unk27C++;
        if (ball->rank == 0)
        {
            b = 1;
            a = 4;
        }
        else if (ball->rank == 4)
        {
            b = 8;
            a = 1;
        }
        else
        {
            b = 1;
            a = 1;
        }
    }
    else if (ape->flags & (1 << 8))
    {
        a = 1;
        b = 2;
        c = 0;
        fn_1_D1FB8(ape, 0);
    }
    else if (ape->flags & (1 << 11))
    {
        a = 1;
        b = 2;
        c = 1;
        fn_1_D1FB8(ape, 0);
    }
    else if (ape->flags & (1 << 18))
    {
        a = 1;
        b = 2;
        c = 2;
    }
    else if (sub_mode == 0x4B)
    {
        a = 0;
        b = 7;
        c = 6;
    }
    else if (ape->flags & (1 << 1))
    {
        a = 0;
        b = 5;
        c = 0;
    }
    else if ((ball->flags & 0x3000) && !(mode_info.flags & 0x10))
    {
        a = 0;
        b = 7;
        if (main_mode == 0 && sub_mode == 0xE)
        {
            if (modeCtrl.submodeTimer > 90)
                c = 0;
            else
            {
                c = 1;
                ape->flags |= 0x1000;
                mtxa_push();
                mtxa_from_quat(&ape->rot);
                mtxa_rotate_y(0xE38);
                mtxa_to_quat(&ape->rot);
                mtxa_pop();
            }
        }
        else if (sub_mode == 0x40)
            c = 2;
        else if (mode_info.flags & 0x40)
        {
            a = 1;
            b = 0;
            c = 0;
        }
        else
            c = fn_1_45EAC(ape, ball);
    }
    else if (ball->flags & 0x4000)
    {
        a = 0;
        b = 0;
        c = 0;
        ape->flags |= 0x1000;
        if (ape->flags & (1 << 13))
            ball->flags &= ~0x4000;
    }
    else if (ball->flags & 0x8000)
    {
        a = 0;
        b = 8;
        if (mode_info.currFloor & 1)
            c = 0;
        else
            c = 1;
        if (modeCtrl.gameType == 1)
            c = fn_1_461C0_inline(ball);
        else if (!(mode_info.flags & 0x40))
        {
            if (mode_info.currFloor & 1)
                c = 0;
            else
                c = 1;
        }
        else
            c = 1;
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
        if (sub_mode == 0xE && modeCtrl.submodeTimer > 0x1338)
            c = 6;
    }
    else
    {
        float f31 = 216.0f * mathutil_vec_len(&ball->vel);
        int i;

        a = 0;
        b = 2;
        c = 0;
        for (i = 12; i >= 0; i--)
        {
            if (f31 > fn_1_1156F0((void *)ball->ape->unk0, 2, i)->unk14)
            {
                c = i;
                i = -1;
            }
        }
    }
    if (b != 1 || sub_mode == 0x31)
        ape->unk27C = 0;
    fn_1_113190(ape, a, speed, b, c);
}

void g_move_ape_into_ball(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];

    mtxa_push();
    mtxa_from_quat(&ape->rot);
    mtxa_tf_vec_xyz(0.0f, -0.12f, 0.0f, &ape->unk264);
    mtxa_pop();

    ape->pos = ball->pos;
    ape->unk270 = (Vec){0.0f, 0.0f, 0.0f};
}

// needed here due to float constant ordering
static float get_0_05(void) {return 0.05f;}

void fn_1_46958(void)
{
    Quat sp2C;
    Vec sp20;

    mtxa_to_mtx(lbl_1_bss_70E40);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp20);
    if (sp20.y < 0.99f)
    {
        Vec sp14 = {0.0f, 1.0f, 0.0f};
        mathutil_vec_cross_prod(&sp20, &sp14, &sp20);
        quat_from_axis_angle(&sp2C, &sp20, 0x38E);
    }
    else
    {
        Vec sp8 = {0.0f, 1.0f, 0.0f};
        quat_from_dirs(&sp2C, &sp20, &sp8);
    }
    mtxa_from_quat(&sp2C);
    mtxa_normalize_basis();
    mtxa_mult_right(lbl_1_bss_70E40);
}

void g_default_ape_draw(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    struct RaycastHit sp94;
    int r30;
    float speed;

    if (g_some_other_flags & 0xA)
        return;

    raycast_stage_down(&ball->pos, &sp94, NULL);
    ape->flags &= -20;
    if (!(sp94.flags & 1) && ball->vel.y < -0.16203702986240387f)
        ape->flags |= 2;
    else if (mathutil_vec_len(&ball->unkC0) < 0.00027777777f)
        ape->flags |= 1;

    r30 = (ball->flags & 0x1000) != 0;
    r30 |= !(ape->flags & 3);
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
            fn_1_46958();
    }

    if (ball->flags & 0x20)
        speed = mathutil_vec_len(&ball->vel);

    fn_1_45CA8(ape);
    mtxa_to_quat(&ape->rot);
    fn_1_461C0(ape, speed);
    fn_1_D2510(ape);
    if (!(ape->flags & (1 << 3)))
        g_move_ape_into_ball(ape);
    g_ape_anim_head(ape, &ball->unk10C);
    ball->unk108 = 0;
    ball->unk118 = 0.0f;
}

void fn_1_46FF8(struct Ball *ball) {}

void fn_1_46FFC(void)
{
    BALL_FOREACH( ball->ape->flags &= ~(1 << 14); )
}

void event_ball_init(void)
{
    struct Ball *ball;
    int i;

    switch (modeCtrl.gameType)
    {
    case 0:
    case 1:
    case 2:
    case 14:
        g_effect_flag_ptr = &lbl_1_data_2E372;
        break;
    case 3:
        g_effect_flag_ptr = &lbl_1_data_2E374;
        break;
    case 4:
        g_effect_flag_ptr = &lbl_1_data_2E376;
        break;
    case 5:
    case 13:
        g_effect_flag_ptr = &lbl_1_data_2E378;
        break;
    case 6:
        g_effect_flag_ptr = &lbl_1_data_2E37A;
        break;
    case 7:
        g_effect_flag_ptr = &lbl_1_data_2E37C;
        break;
    case 8:
        g_effect_flag_ptr = &lbl_1_data_2E37E;
        break;
    case 10:
        g_effect_flag_ptr = &lbl_1_bss_700BC;
        break;
    case 9:
        g_effect_flag_ptr = &lbl_1_data_2E380;
        break;
    case 15:
        g_effect_flag_ptr = &lbl_1_data_2E382;
        break;
    case 16:
        g_effect_flag_ptr = &lbl_1_data_2E384;
        break;
    default:
        g_effect_flag_ptr = &lbl_1_data_2E370;
        break;
    }
    lbl_1_bss_70E94 = 0;
    fn_1_67BE8();
    for (i = 0; i < 16; i++)
        lbl_1_bss_70E98[i] = -1;
    fn_1_D250C(1.0f);
    set_global_LOD(1);
    for (i = 0, ball = balls; i < 8; i++, ball++)
    {
        ball->playerId = i;
        ball->status = 0;
        ball->ape = NULL;
        g_reset_ball(ball);
        ball->state = 0;
        ball->unk178 = -1;
        ball->unk154 = 0;
        ball->unk155 = -1;
    }
    if (*g_effect_flag_ptr & 0x80)
        challenge_mode_init();
}

void g_call_maybe_sets_number_of_starting_monkeys(struct Ball *ball)
{
    g_maybe_sets_number_of_starting_monkeys(ball);
}

void create_stage_player(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod, void (*drawFunc)(struct Ape *), int variant)
{
    struct Ball *ball = &balls[playerIdx];
    int ballColor;
    struct Ape *ape;
    int apeColor;

    switch (colorId)
    {
    default:
        ballColor = colorId;
        apeColor = colorId;
        break;
    case 0xFF:
        ballColor = 3;
        apeColor = 0;
        break;
    }
    if (ball->status != 0)
        return;
    ball->status = 2;
    ball->colorId = (u8)ballColor;
    ball->state = (s8)mode;
    ball->unk154 = 0;
    ball->unk157 = 0;
    lbl_1_bss_70E70[playerIdx] = 0.0f;
    set_global_LOD(lod);
    fn_1_D1F7C(1);
    if (apeChara == 0xFF)
        return;
    ape = load_ape_gameplay(apeChara, lod, variant);
    ball->ape = ape;
    ape->unk29C = 0;
    ape->ballIdx = playerIdx;
    ape->colorIndex = apeColor;
    if (stagedef != NULL)
    {
        ape->pos = stagedef->startPos->pos;
        mtxa_from_identity();
        mtxa_rotate_y(stagedef->startPos->yrot);
        mtxa_to_quat(&ape->rot);
    }
    else
    {
        ape->pos.x = 0.0f;
        ape->pos.y = 0.0f;
        ape->pos.z = 0.0f;
        mtxa_from_identity();
        mtxa_to_quat(&ape->rot);
    }
    if (drawFunc != NULL)
        ape->drawFunc = drawFunc;
    else
        ape->drawFunc = g_default_ape_draw;
    set_global_LOD(1);
}

void g_create_stage_player_wrapper(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod, void (*drawFunc)(struct Ape *))
{
    create_stage_player(playerIdx, mode, apeChara, colorId, lod, drawFunc, 0);
}

void fn_1_47444(u8 playerIdx)
{
    struct Ball *ball = &balls[playerIdx];

    if (ball->status != 0)
    {
        if (ball->ape != NULL)
        {
            g_something_with_freeing_chara_heap_ape(ball->ape);
            ball->ape = NULL;
        }
        ball->flags &= ~(1ULL << 40);
        ball->status = 0;
    }
}

void fn_1_474D0(void)
{
    int i;

    for (i = 0; i < 8; i++)
        fn_1_47444(i);
}

void challenge_mode_init(void)
{
    struct Ball *ball;
    struct Ape *ape;
    int i;

    set_global_LOD(1);
    switch (modeCtrl.gameType)
    {
    case 1:
    case 4:
        if (modeCtrl.playerCount > 2)
            set_global_LOD(2);
        break;
    case 3:
        if (modeCtrl.playerCount >= 2)
            set_global_LOD(2);
        break;
    }

    for (i = 0, ball = &balls[0]; i < 8; i++, ball++)
    {
        if (i >= 4)
            continue;
        if (g_poolInfo.playerPool.statusList[i] == 0)
            continue;
        if (modeCtrl.gameType != 8 && g_poolInfo.playerPool.statusList[i] == 4)
            continue;
        current_ball = ball;
        g_reset_ball(ball);
        ball->status = 2;
        ball->state = 0;
        fn_1_D1F7C(1);
        ape = load_ape_gameplay_id_only(active_monkey_id[i]);
        ball->ape = ape;
        ape->unk29C = 0;
        if (modeCtrl.unk30 > 1)
            ape->flags |= 0x100000;
        ball->unk157 = 0;
        ape->ballIdx = i;
        ape->colorIndex = i;
        ape->pos = stagedef->startPos->pos;
        mtxa_from_identity();
        mtxa_rotate_y(stagedef->startPos->yrot);
        mtxa_to_quat(&ape->rot);
        lbl_1_bss_70EE8 = 0;
        lbl_1_bss_70E70[i] = 0.0f;
        switch (modeCtrl.gameType)
        {
        case 1:
        case 3:
        case 4:
        case 5:
        case 8:
        case 13:
            break;
        default:
            ape->cameraVisibleMask = ~(1 << i);
            break;
        }
        switch (modeCtrl.gameType)
        {
        case 1:
        case 4:
            if (modeCtrl.playerCount > 2)
                fn_1_D1FB8(ape, 2);
            else
                fn_1_D1FB8(ape, 1);
            break;
        case 3:
        case 5:
        case 13:
            switch (modeCtrl.playerCount)
            {
            case 1:
                fn_1_D1FB8(ape, 1);
                break;
            case 2:
            case 3:
            case 4:
            default:
                fn_1_D1FB8(ape, 2);
                break;
            }
            break;
        default:
            fn_1_D1FB8(ape, 0);
            break;
        }
    }

    switch (modeCtrl.gameType)
    {
    case 0:
    case 7:
        current_ball = &balls[modeCtrl.currPlayer];
        break;
    default:
        current_ball = &balls[0];
        break;
    }
    set_global_LOD(1);
}

struct Ape *fn_1_478C4(int unused, int chara)
{
    struct Ape *ape = load_ape_gameplay_id_only(chara);

    ape->unk29C = 0;
    mtxa_from_identity();
    mtxa_rotate_y(-0x4000);
    fn_1_D1FB8(ape, 0);
    mtxa_to_quat(&ape->rot);
    lbl_1_bss_70EE8 = 0;
    return ape;
}

#pragma force_active on
const Vec lbl_1_rodata_7364 = { 0.0f, 0.0f, 0.0f };
#pragma force_active off

void event_ball_tick(void)
{
    struct Ball *ball;
    int i;
    Vec sp60;
    Vec sp54;
    Vec sp48;
    Vec sp3C;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    float minDot;
    float dot;

    if (g_some_other_flags & 0xA)
        return;

    for (ball = &balls[0], i = 0; i < 8; i++, ball++)
    {
        if (ball->status == 0 || ball->status == 4)
            continue;
        current_ball = ball;
        mtx_copy(ball->unk30, ball->unkD0);
        ball->prevFlags = ball->flags;
        ball->flags &= ~(1 | 4 | 0x10000);
        if (*g_effect_flag_ptr & 2)
            g_some_ballfunc(ball);
        if (*g_effect_flag_ptr & 4)
            g_something_with_translating_items(4, &ball->pos, 0.75f);
        if (!(ball->unk178 & 0x80))
        {
            ball->state = ball->unk178;
            ball->unk178 |= 0x80;
        }
        if (!(ball->unk155 & 0x80))
        {
            ball->unk154 = ball->unk155;
            ball->unk155 |= 0x80;
        }
        ball_mode_funcs[ball->state](ball);
        if (*g_effect_flag_ptr & 0x100)
            raycast_stage_down(&ball->pos, &ball->unk17C, NULL);
    }

    if (lbl_1_bss_70E94 != NULL)
        lbl_1_bss_70E94();

    for (ball = &balls[0], i = 0; i < 8; i++, ball++)
    {
        if (ball->status == 0 || ball->status == 4)
            continue;
        current_ball = ball;
        if ((*g_effect_flag_ptr & 8) && !(ball->flags & (1ULL << 43)))
            ball_movement_sparks(ball);
        if ((*g_effect_flag_ptr & 0x10) && (ball->flags & 4) && !(ball->flags & (1ULL << 43)))
            ball_collision_stars(ball);

        mtxa_from_mtx(ball->unk30);
        ball->unkCC = mathutil_vec_len(&ball->unkC0);
        mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &sp54);
        mtxa_tf_vec_xyz(1.0f, 0.0f, 0.0f, &sp3C);
        mtxa_tf_vec_xyz(0.0f, 0.0f, 1.0f, &sp24);
        vec_set(&sp18, 0.0f, -ball->currRadius, 0.0f);
        mtxa_rigid_inv_tf_vec(&sp18, &sp18);
        mtxa_from_mtx(ball->unkD0);
        mtxa_tf_vec(&sp18, &ball->unkC0);
        ball->unkC0.y += ball->currRadius;
        mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &sp60);
        mtxa_tf_vec_xyz(1.0f, 0.0f, 0.0f, &sp48);
        mtxa_tf_vec_xyz(0.0f, 0.0f, 1.0f, &sp30);
        minDot = vec_dot_normalized(&sp60, &sp54);
        dot = vec_dot_normalized(&sp48, &sp3C);
        if (minDot > dot)
        {
            minDot = dot;
            sp60 = sp48;
            sp54 = sp3C;
        }
        dot = vec_dot_normalized(&sp30, &sp24);
        if (minDot > dot)
        {
            minDot = dot;
            sp60 = sp30;
            sp54 = sp24;
        }
        if (minDot > -0.9998f && minDot < 0.9998f)
            quat_from_dirs(&ball->unkB0, &sp60, &sp54);
        else
            ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
        g_ball_ape_rotation(ball);
        if (*g_effect_flag_ptr & 0x20)
            spawn_postgoal_ball_sparkle();
        if (*g_effect_flag_ptr & 0x40)
            set_visual_scale(ball);
        if (*g_effect_flag_ptr & 2)
            ball_sounds_gameplay(ball);
        if (ball->unk15A > 0)
            ball->unk15A--;
    }

    switch (modeCtrl.gameType)
    {
    case 0:
    case 7:
        current_ball = &balls[modeCtrl.currPlayer];
        break;
    default:
        current_ball = &balls[0];
        break;
    }
    fn_1_67C18();
}

void event_ball_dest(void)
{
    int i;
    struct Ball *ball;
    int j;

    fn_1_67C5C();
    for (i = 0; i < 8; i++)
    {
        ball = &balls[(u8)i];
        if (ball->status != 0)
        {
            if (ball->ape != NULL)
            {
                g_something_with_freeing_chara_heap_ape(ball->ape);
                ball->ape = NULL;
            }
            ball->flags &= ~(1ULL << 40);
            ball->status = 0;
        }
    }
    for (j = 15; j >= 0; j--)
    {
        if (lbl_1_bss_70E98[j] != -1)
        {
            fn_1_D29E0(lbl_1_bss_70E98[j]);
            lbl_1_bss_70E98[j] = -1;
        }
    }
    g_effect_flag_ptr = NULL;
    lbl_1_bss_70E94 = NULL;
}

void add_bananas(int count)
{
    struct Ball *ball = current_ball;

    switch (modeCtrl.gameType)
    {
    case 0:
        ball->bananas += count;
        g_banana_disp_efc_req(count);
        if (ball->bananas >= 100)
        {
            ball->unk2++;
            ball->bananas -= 100;
            create_1up_sprite(120);
            call_SoundReqID_arg_0(0x2852);
        }
        break;
    case 1:
    case 2:
    case 14:
        ball->bananas += count;
        if (ball->bananas > 999)
            ball->bananas = 999;
        break;
    default:
        ball->bananas += count;
        break;
    }
}

void g_something_with_translating_items(int type, Vec *pos, float weight)
{
    int i;
    struct Ball *ball;
    int playerId;
    Vec diff;
    Vec dir;
    Mtx mtx;
    float len;

    playerId = current_ball->playerId;
    ball = &balls[0];
    for (i = 0; i < 8; i++, ball++)
    {
        if (ball->status == 0)
            continue;
        if (type == 4 && playerId == i)
            continue;
        diff.x = pos->x - ball->pos.x;
        diff.y = pos->y - ball->pos.y;
        diff.z = pos->z - ball->pos.z;
        len = vec_normalize_len(&diff);
        if (len > FLT_EPSILON)
            weight /= len;
        if (ball->unk118 > weight)
            return;
        if ((ball->flags & 1) != 0)
        {
            dir.x = -ball->unk11C.x;
            dir.y = -ball->unk11C.y;
            dir.z = -ball->unk11C.z;
        }
        else
        {
            dir.x = ball->vel.x;
            dir.y = ball->vel.y;
            dir.z = ball->vel.z;
            if (0.0 == dir.y)
                dir.y = FLT_EPSILON;
            vec_normalize_len(&dir);
        }
        weight *= 0.75 * (0.5 + mathutil_vec_dot_prod(&dir, &diff));
        if (ball->unk118 > weight)
            return;
        if (ball->ape != NULL)
            mtxa_from_quat(&ball->ape->rot);
        mtxa_to_mtx(mtx);
        dir.x = -mtx[0][0];
        dir.y = -mtx[1][0];
        dir.z = -mtx[2][0];
        mtxa_from_identity();
        mtxa_rotate_y(-0x4000);
        mtxa_tf_vec(&dir, &dir);
        weight *= 0.5 + mathutil_vec_dot_prod(&dir, &diff);
        if (ball->unk118 < weight)
        {
            ball->unk108 = type;
            ball->unk10C = *pos;
            ball->unk118 = weight;
        }
    }
}

void fn_1_48384(struct Ball *ball)
{
    ball->flags &= ~((1ULL << 33) | (1ULL << 34));
    ball->unk148 = 0;
}

void g_maybe_sets_number_of_starting_monkeys(struct Ball *ball)
{
    struct Ape *ape = ball->ape;
    s8 playerId = ball->playerId;
    s8 status = ball->status;
    u8 colorId = ball->colorId;

    memset(ball, 0, sizeof(*ball));
    ball->bananas = 0;
    ball->unk140 = 0;
    ball->score = 0;
    ball->ape = ape;
    ball->playerId = playerId;
    ball->status = status;
    ball->winStreak = 0;
    ball->unk134 = 0;
    ball->unk178 = -1;
    ball->unk155 = -1;
    ball->colorId = colorId;

    switch (modeCtrl.gameType)
    {
    default:
        if (dip_switches & DIP_SARU_0)
            ball->unk2 = 0;
        else
            ball->unk2 = number_of_starting_monkeys;
        break;
    case 1:
    case 2:
        ball->unk2 = 2;
        break;
    case 14:
        ball->unk2 = 2;
        ball->score = get_storymode_score();
        ball->bananas = get_storymode_banana_count();
        break;
    }
}

void g_reset_ball(struct Ball *ball)
{
    s8 playerId;
    s8 status;
    s8 lives;
    s32 bananas;
    s32 unk140;
    s32 score;
    struct Ape *ape;
    s16 winStreak;
    s16 unk134;
    s8 unk178;
    s8 unk155;
    u8 colorId;
    u8 lod;

    switch (modeCtrl.gameType)
    {
    case 2:
        ball->bananas = 0;
        ball->score = 0;
        break;
    case 1:
        if ((ball->flags & (1ULL << 39)) != 0)
        {
            ball->bananas = ball->unk140;
            ball->flags &= ~(1ULL << 39);
        }
        ball->unk140 = ball->bananas;
        break;
    case 14:
        ball->score = get_storymode_score();
        ball->bananas = get_storymode_banana_count();
        break;
    }

    playerId = ball->playerId;
    status = ball->status;
    lives = ball->unk2;
    bananas = ball->bananas;
    unk140 = ball->unk140;
    score = ball->score;
    ape = ball->ape;
    winStreak = ball->winStreak;
    unk134 = ball->unk134;
    unk178 = ball->unk178;
    unk155 = ball->unk155;
    colorId = ball->colorId;
    lod = ball->unk179;
    memset(ball, 0, sizeof(*ball));
    ball->playerId = playerId;
    ball->status = status;
    ball->unk2 = lives;
    ball->bananas = bananas;
    ball->unk140 = unk140;
    ball->score = score;
    ball->ape = ape;
    ball->winStreak = winStreak;
    ball->unk134 = unk134;
    ball->unk178 = unk178;
    ball->unk155 = unk155;
    ball->colorId = colorId;
    ball->unk179 = lod;
    set_ball_properties(ball, 0);
    mtxa_from_identity();
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
}

void fn_1_48654(struct Ball *ball)
{
    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;
    ball->unk60.x = 0;
    ball->unk60.y = 0;
    ball->unk60.z = 0;
}

void fn_1_486A0(struct Ball *ball)
{
    struct Ape *ape = ball->ape;
    s8 playerId = ball->playerId;
    s8 status = ball->status;
    u8 colorId = ball->colorId;

    memset(ball, 0, sizeof(*ball));
    ball->bananas = 0;
    ball->unk140 = 0;
    ball->score = 0;
    ball->ape = ape;
    ball->playerId = playerId;
    ball->status = status;
    ball->winStreak = 0;
    ball->unk134 = 0;
    ball->unk178 = -1;
    ball->unk155 = -1;
    ball->colorId = colorId;

    switch (modeCtrl.gameType)
    {
    default:
        if (dip_switches & DIP_SARU_0)
            ball->unk2 = 0;
        else
            ball->unk2 = number_of_starting_monkeys;
        break;
    case 1:
    case 2:
        ball->unk2 = 2;
        break;
    case 14:
        ball->unk2 = 2;
        ball->score = get_storymode_score();
        ball->bananas = get_storymode_banana_count();
        break;
    }
    fn_1_487C8(ball);
}

#pragma force_active on
const float lbl_1_rodata_73A8 = 0.25f;
const float lbl_1_rodata_73AC = 0.125f;
#pragma force_active off

void fn_1_487C8(struct Ball *ball)
{
    g_reset_ball(ball);

    ball->pos.x = stagedef->startPos->pos.x;
    ball->pos.y = stagedef->startPos->pos.y + ((ball->accel * 24.0) * 24.0) * 0.5;
    ball->pos.z = stagedef->startPos->pos.z;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;

    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);

    ball->flags |= 0x10;
    ball->ape->flags |= 0x20;
    ball->state = 0;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;
    set_ape_anim(ball->ape, 5);
}

void g_competition_mode_respawn_start(struct Ball *ball)
{
    float f2;
    float f4;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;

    f4 = modeCtrl.submodeTimer;
    f2 = stagedef->startPos->pos.y - ball->pos.y;

    ball->vel.x = 0.0f;
    ball->vel.y = f2 / f4 + (ball->accel * f4) * 0.5;
    ball->vel.z = 0.0f;

    ball->rotX = 0x2000;
    ball->rotY = stagedef->startPos->yrot - 0x4000;
    ball->rotZ = 0;

    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);

    ball->flags &= ~0x10;
    ball->flags |= 0x4000;
    if (ball->ape != NULL)
        ball->ape->flags &= ~(1 << 5);
    call_SoundReqID_arg_0(30);
    ball->state = 4;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkA0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    mtxa_to_quat(&ball->unkB0);
    set_ape_anim(ball->ape, 5);
}

void fn_1_48BCC(struct Ball *ball)
{
    struct PhysicsBall physBall;

    g_apply_ball_velocity(ball, &physBall);
    handle_ball_stage_coli(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
}

void ball_physics_g_something_w_postgoal_slowdown(struct Ball *ball)
{
    ball->state = 6;
    ball->flags |= 0x500;
    if ((ball->flags & 0x3000) != 0)
        ball->flags |= 0x40;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;
    if ((ball->flags & 0x2000) != 0)
        g_some_set_ape_anim(ball->ape, 3, 8);
    else
        g_some_set_ape_anim(ball->ape, 3, fn_1_4CD70());
    ball_physics_g_something_w_postgoal_blast_up(ball);
}

void ball_physics_g_something_w_postgoal_blast_up(struct Ball *ball)
{
    struct PhysicsBall physBall;

    if ((ball->flags & 0x200) == 0 && (ball->ape->flags & (1 << 14)))
    {
        ball->flags &= ~0x500;
        ball->flags |= 0x200;
        call_SoundReqID_arg_0(0x126);
    }
    g_apply_ball_velocity(ball, &physBall);
    handle_ball_stage_coli(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
}

void fn_1_48ED8(struct Ball *ball)
{
    struct ReplayBallFrame frame;
    Vec dir;

    ball->unk80 = 0;
    ball->state = 10;
    ball->flags &= ~0x700;
    ball->flags &= ~0x10;
    if (ball->ape != NULL)
        ball->ape->flags &= ~(1 << 5);
    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], lbl_1_bss_71EC8.unk10, &frame);
    ball->pos.x = frame.pos.x;
    ball->pos.y = frame.pos.y;
    ball->pos.z = frame.pos.z;
    ball->ape->pos = ball->pos;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;
    ball->ape->rot = ball->unkA0;
    g_ball_mode_play_replay(ball);
    dir.x = ball->vel.x;
    dir.y = 0.0f;
    dir.z = ball->vel.z;
    if (dir.x == 0.0 && dir.z == 0.0)
    {
        mtxa_from_identity();
        mtxa_rotate_y(stagedef->startPos->yrot);
        dir.x = 0.0f;
        dir.y = 0.0f;
        dir.z = -1.0f;
        mtxa_tf_vec(&dir, &dir);
    }
    g_smth_with_ape_rotation(ball->ape, &dir);
    set_ape_anim(ball->ape, 1);
    mtx_copy(ball->unk30, ball->unkD0);
}

void g_ball_mode_play_replay(struct Ball *ball)
{
    struct ReplayBallFrame frame;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;

    if (ball->playerId == lbl_1_bss_71EC8.unk14)
        lbl_1_bss_71EC8.unk10 -= 1.0f;

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], lbl_1_bss_71EC8.unk10, &frame);

    ball->pos.x = frame.pos.x;
    ball->pos.y = frame.pos.y;
    ball->pos.z = frame.pos.z;

    ball->vel.x = ball->pos.x - ball->prevPos.x;
    ball->vel.y = ball->pos.y - ball->prevPos.y;
    ball->vel.z = ball->pos.z - ball->prevPos.z;

    ball->unk60.x = frame.rotX - ball->rotX;
    ball->unk60.y = frame.rotY - ball->rotY;
    ball->unk60.z = frame.rotZ - ball->rotZ;

    ball->rotX = frame.rotX;
    ball->rotY = frame.rotY;
    ball->rotZ = frame.rotZ;

    ball->unk11C.x = frame.unk12 * 3.0518509475997192e-05;
    ball->unk11C.y = frame.unk14 * 3.0518509475997192e-05;
    ball->unk11C.z = frame.unk16 * 3.0518509475997192e-05;

    ball->flags = (ball->flags & 0xFFFFFFFF00000000ULL) | frame.flags | (1ULL << 40);
    ball->unk13C = frame.unk1C;

    if ((ball->prevFlags & 0x1000) == 0 && (ball->flags & 0x1000) != 0)
        g_some_set_ape_anim(ball->ape, 3, fn_1_4CD70());

    if (lbl_1_bss_71EC8.unk10 <= 0.0 || !(mode_info.flags & 0x10))
        ball->state = 4;

    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    ball->unk80++;
}

void fn_1_49470(struct Ball *ball)
{
    g_reset_ball(ball);

    ball->pos.x = stagedef->startPos->pos.x;
    ball->pos.y = stagedef->startPos->pos.y;
    ball->pos.z = stagedef->startPos->pos.z;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;

    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);

    ball->ape->pos = ball->pos;
    ball->ape->unk270.x = 0.0f;
    ball->ape->unk270.y = 0.0f;
    ball->ape->unk270.z = 0.0f;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;
    ball->ape->rot = ball->unkA0;
    ball->rotY = lbl_1_bss_2F48.infLightRotY + 0x2000;

    mtxa_from_identity();
    mtxa_rotate_y(ball->rotY);
    mtxa_to_quat(&ball->ape->rot);
    ball->state = 12;
}

void fn_1_49670(struct Ball *ball) {}

void fn_1_49674(struct Ball *ball)
{
    g_reset_ball(ball);

    ball->pos.x = stagedef->startPos->pos.x;
    ball->pos.y = stagedef->startPos->pos.y + 10.0;
    ball->pos.z = stagedef->startPos->pos.z;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;

    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);

    ball->ape->pos = ball->pos;
    ball->ape->unk270.x = 0.0f;
    ball->ape->unk270.y = 0.0f;
    ball->ape->unk270.z = 0.0f;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;
    ball->ape->rot = ball->unkA0;

    mtxa_from_identity();
    mtxa_rotate_y(stagedef->startPos->yrot);
    mtxa_to_quat(&ball->ape->rot);
    ball->state = 14;
}

void fn_1_49878(struct Ball *ball) {}

void fn_1_4987C(struct Ball *ball)
{
    Vec axis;

    g_reset_ball(ball);

    ball->pos.x = stagedef->startPos->pos.x;
    ball->pos.y = stagedef->startPos->pos.y + 10.0;
    ball->pos.z = stagedef->startPos->pos.z;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;

    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);

    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;

    axis.x = (RAND_FLOAT() - 0.5) * 0.5;
    axis.y = (RAND_FLOAT() - 0.5) * 0.5;
    axis.z = (RAND_FLOAT() - 0.5) * 0.5;

    if (ball->currRadius > FLT_EPSILON && mathutil_vec_len(&axis) != 0.0f)
    {
        float len = vec_normalize_len(&axis);

        quat_from_axis_angle(
            &ball->unkA0,
            &axis,
            (len * 2.0f) / (ball->currRadius * ball->currRadius * 3.14159265) * 10430.3779296875);
    }

    ball->flags |= 0x40;
    ball->state = 4;
    ball->ape->pos = ball->pos;
    ball->ape->unk270.x = 0.0f;
    ball->ape->unk270.y = 0.0f;
    ball->ape->unk270.z = 0.0f;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;
    ball->ape->rot = ball->unkA0;
}

void fn_1_49BD4(struct Ball *ball)
{
    Vec v;
    struct PhysicsBall physBall;

    mtxa_from_mtx(mtxa->mtxB);
    mtxa_rigid_inv_tf_tl(&v);
    v.x -= ball->pos.x;
    v.y = 0.0f;
    v.z -= ball->pos.z;
    vec_set_len(0.00064f, &v, &v);

    ball->vel.x += v.x;
    ball->vel.y += v.y;
    ball->vel.z += v.z;

    g_apply_ball_velocity(ball, &physBall);
    handle_ball_stage_coli(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
}

void fn_1_49CCC(struct Ball *ball)
{
    g_reset_ball(ball);

    if (modeCtrl.submodeTimer == 0x1428)
    {
        ball->pos.x = stagedef->startPos->pos.x;
        ball->pos.y = stagedef->startPos->pos.y;
        ball->pos.z = stagedef->startPos->pos.z;
    }
    else
    {
        ball->pos.x = 14.5f;
        ball->pos.y = 2.5f;
        ball->pos.z = -20.0f;
    }

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;

    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);

    ball->ape->pos = ball->pos;
    ball->ape->unk270.x = 0.0f;
    ball->ape->unk270.y = 0.0f;
    ball->ape->unk270.z = 0.0f;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;

    if (modeCtrl.submodeTimer != 0x1428)
    {
        mtxa_from_identity();
        mtxa_rotate_y(stagedef->startPos->yrot);
        mtxa_to_quat(&ball->ape->rot);
    }
}

void fn_1_49EDC(struct Ball *ball)
{
    ball->unk130 = 60;
    world_infos[ball->playerId].unk1C = 90;
    cameras[ball->playerId].mode = 4;
    call_SoundReqID_arg_0(ball->unk2 == 1 ? 0x51 : 0x1D);
    call_SoundReqID_arg_0(0x15);
    ball->state = 20;
}

void g_handle_competition_mode_respawn(struct Ball *ball)
{
    Vec dir;
    struct PhysicsBall physBall;

    g_apply_ball_velocity(ball, &physBall);
    handle_ball_stage_coli(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;

    if (--ball->unk130 <= 0)
    {
        ball->pos.x = ball->prevPos.x = stagedef->startPos->pos.x;
        ball->pos.y = ball->prevPos.y = stagedef->startPos->pos.y + 0.5 * (24.0 * (24.0 * ball->accel));
        ball->pos.z = ball->prevPos.z = stagedef->startPos->pos.z;

        ball->vel.x = 0.0f;
        ball->vel.y = 0.0f;
        ball->vel.z = 0.0f;
        ball->unk80 = 0;

        ball->unkA0.w = 1.0f;
        ball->unkA0.x = 0.0f;
        ball->unkA0.y = 0.0f;
        ball->unkA0.z = 0.0f;

        ball->unk15A = 30;
        world_infos[ball->playerId].unk1C = 30;

        dir.x = -math_sin(stagedef->startPos->yrot);
        dir.y = 0.0f;
        dir.z = -math_sin(stagedef->startPos->yrot + 0x4000);
        g_smth_with_ape_rotation(ball->ape, &dir);

        cameras[ball->playerId].mode = 0;
        ball->flags &= ~(u64)0x800;
        ball->state = 4;
    }
}

void fn_1_4A18C(struct Ball *ball)
{
    fn_1_10D34(ball);
}

void fn_1_4A1AC(struct Ball *ball)
{
    if (g_some_menu_func_1 != NULL)
        g_some_menu_func_1(ball);
}

void ball_physics_g_something_w_poastgoal_slowdown_blast_up(struct Ball *ball)
{
    struct PhysicsBall physBall;

    ball->state = 28;
    ball->flags |= 0x500;
    if (ball->flags & 0x3000)
        ball->flags |= 0x40;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;

    if ((ball->flags & 0x200) == 0 && (ball->ape->flags & (1 << 14)))
    {
        ball->flags &= ~0x500;
        ball->flags |= 0x200;
        call_SoundReqID_arg_0(0x126);
    }
    g_apply_ball_velocity(ball, &physBall);
    handle_ball_stage_coli(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
}

void ball_physics_g_something_w_postgoal_blast_up2(struct Ball *ball)
{
    struct PhysicsBall physBall;

    if ((ball->flags & 0x200) == 0 && (ball->ape->flags & (1 << 14)))
    {
        ball->flags &= ~0x500;
        ball->flags |= 0x200;
        call_SoundReqID_arg_0(0x126);
    }
    g_apply_ball_velocity(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
}

void g_move_and_collide(struct Ball *ball, struct PhysicsBall *physBall)
{
    g_apply_ball_velocity(ball, physBall);
    handle_ball_stage_coli(ball, physBall);
}

void g_apply_ball_velocity(struct Ball *ball, struct PhysicsBall *physBall)
{
    Vec accel;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;

    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20;

    accel.x = 0.0f;
    accel.y = -ball->accel;
    accel.z = 0.0f;
    if (ball->flags & 0x200)
        accel.y = -accel.y;
    else if (ball->flags & 0x100)
        accel.y = 0.0f;

    if (!(ball->flags & (1ULL << 32))
     && sub_mode != 14
     && (ball->prevFlags & 1))
    {
        Vec vel = ball->vel;
        float f1 = mathutil_sum_of_sq_2(vel.x, vel.z);
        float dot;

        if (f1 > FLT_EPSILON)
        {
            f1 = 1.0 / math_sqrt(f1);
            vel.x *= f1;
            vel.z *= f1;
            dot = -math_sin(cameras[ball->playerId].rotY) * vel.x
                      + -mathutil_cos(cameras[ball->playerId].rotY) * vel.z;
            if (dot < 0.0)
            {
                f1 = -dot * -0.65 + 1.0;
                accel.x *= f1;
                accel.y *= f1;
                accel.z *= f1;
            }
        }
    }

    mtxa_from_identity();
    mtxa_rotate_x(world_infos[ball->playerId].xrot);
    mtxa_rotate_z(world_infos[ball->playerId].zrot);
    mtxa_rigid_inv_tf_vec(&accel, &accel);

    ball->vel.x += accel.x;
    ball->vel.y += accel.y;
    ball->vel.z += accel.z;

    ball->pos.x += ball->vel.x;
    ball->pos.y += ball->vel.y;
    ball->pos.z += ball->vel.z;
}

void handle_ball_stage_coli(struct Ball *ball, struct PhysicsBall *physBall)
{
    init_physicsball_from_ball(ball, physBall);
    collide_ball_with_seesaws(0, ball->playerId, physBall);
    collide_ball_with_stage(physBall, stagedef);
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
    }
}

void position_ball(struct Ball *ball, struct PhysicsBall *physBall)
{
    mtxa_from_quat(&ball->unkA0);
    mtxa_mult_left(ball->unk30);
    mtxa_to_euler_yxz(&ball->rotY, &ball->rotX, &ball->rotZ);
    mtxa_from_identity();
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mathutil_mtxA_set_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);

    ball->unk13C = physBall->hardestColiSpeed;
    if ((ball->flags & 1) && physBall->hardestColiSpeed < -0.11f)
        ball->flags |= 4;
    if (physBall->hardestColiSpeed < -0.15f)
        ball->flags |= 0x20;

    if ((ball->flags & 1) && physBall->hardestColiSpeed < -0.054999999701976776)
    {
        int r4 = FALSE;

        if (modeCtrl.gameType != 4 && g_some_frame_counter.frameCounter < 0xF0)
            r4 = TRUE;
        if (r4)
            ball->flags |= 0x08000000;
        else if (physBall->hardestColiSpeed < -0.18699999898672104)
            ball->flags |= 0x10000000;
        else if (physBall->hardestColiSpeed < -0.14299999922513962)
            ball->flags |= 0x20000000;
        else if (physBall->hardestColiSpeed < -0.098999999463558197)
            ball->flags |= 0x40000000;
        else
            ball->flags |= 0x80000000;
    }

    if (!(ball->flags & (0x800ULL << 32))
     && !(ball->flags & (0x200ULL << 32))
     && ball->unk13C < -0.11f)
    {
        switch (modeCtrl.gameType)
        {
        case 8:
            g_rumble_controller(player_pad_map[ball->playerId], 1, 0.5 * lbl_80198B38);
            break;
        case 5:
            break;
        default:
            g_rumble_controller(player_pad_map[ball->playerId], 1, (0.2f - ball->unk13C) * lbl_80198B38);
            break;
        }
    }
}

void fn_1_4ADC8(struct PhysicsBall *physBall, Vec *b)
{
    struct Ball *ball = current_ball;
    struct ItemGroup *itemGroup = &itemgroups[physBall->hardestColiItemGroupId];
    Vec sp44;
    Vec sp38;

    mtxa_from_mtx(itemGroup->transform);
    mtxa_tf_point(&physBall->hardestColiPoint, &sp38);
    mtxa_tf_vec(&physBall->hardestColiNormal, &sp44);
    {
    Vec sp2C;
    Vec sp20;
    Vec sp14;

    sp14.x = sp38.x - ball->pos.x;
    sp14.y = sp38.y - ball->pos.y;
    sp14.z = sp38.z - ball->pos.z;

    sp2C.x = ball->pos.x + sp14.x * sp44.x;
    sp2C.y = ball->pos.y + sp14.y * sp44.y;
    sp2C.z = ball->pos.z + sp14.z * sp44.z;

    mtxa_rigid_inv_tf_point(&sp2C, &sp14);

    mtxa_from_mtx(itemGroup->prevTransform);
    mtxa_tf_point(&sp14, &sp20);

    sp14.x = sp2C.x - sp20.x;
    sp14.y = sp2C.y - sp20.y;
    sp14.z = sp2C.z - sp20.z;

    b->x -= sp14.x;
    b->y -= sp14.y;
    b->z -= sp14.z;
    }
}

static void apply_coli_friction(struct PhysicsBall *physBall, Vec *b, Vec *c)
{
    struct Ball *ball = current_ball;
    float f2 = mathutil_vec_dot_prod(b, &physBall->hardestColiNormal);
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    float f4;

    sp2C.x = f2 * physBall->hardestColiNormal.x;
    sp2C.y = f2 * physBall->hardestColiNormal.y;
    sp2C.z = f2 * physBall->hardestColiNormal.z;

    sp38.x = sp2C.x + (b->x - sp2C.x) * 0.92;
    sp38.y = sp2C.y + (b->y - sp2C.y) * 0.92;
    sp38.z = sp2C.z + (b->z - sp2C.z) * 0.92;

    sp44.x = sp38.x - b->x;
    sp44.y = sp38.y - b->y;
    sp44.z = sp38.z - b->z;

    ball->vel.x += sp44.x;
    ball->vel.y += sp44.y;
    ball->vel.z += sp44.z;

    sp14.x = sp44.x * -50.0;
    sp14.y = sp44.y * -50.0;
    sp14.z = sp44.z * -50.0;

    mtxa_rigid_inv_tf_vec(&sp14, &sp14);
    mathutil_vec_cross_prod(c, &sp14, &sp20);

    f4 = 0.0f;
    if (ball->currRadius > FLT_EPSILON)
        f4 = (32768.0 / 3.141592) / (ball->currRadius * ball->currRadius);

    ball->unk60.x = ball->unk60.x + (sp20.x * f4 - ball->unk60.x) * 0.15;
    ball->unk60.y = ball->unk60.y + (sp20.y * f4 - ball->unk60.y) * 0.15;
    ball->unk60.z = ball->unk60.z + (sp20.z * f4 - ball->unk60.z) * 0.15;
}

void fn_1_4AF48(struct Ball *ball, struct PhysicsBall *physBall)
{
    /* Preserve the work slot before the compiler conversion temporaries. */
    union
    {
        Vec value;
        float workspace[4];
    } normalSlot;
    Vec tfPoint;
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    float f2;
    float f1;

    if (physBall->hardestColiSpeed < 0.0)
    {
        sp20.x = ball->pos.x - ball->prevPos.x;
        sp20.y = ball->pos.y - ball->prevPos.y;
        sp20.z = ball->pos.z - ball->prevPos.z;
        if (physBall->hardestColiItemGroupId > 0)
        {
            struct ItemGroup *itemGroup;
            struct Ball *cball = current_ball;
            Vec d14;
            Vec d20;
            Vec d2C;

            itemGroup = &itemgroups[physBall->hardestColiItemGroupId];

            mtxa_from_mtx(itemGroup->transform);
            mtxa_tf_point(&physBall->hardestColiPoint, &tfPoint);
            mtxa_tf_vec(&physBall->hardestColiNormal, &normalSlot.value);

            d14.x = tfPoint.x - cball->pos.x;
            d14.y = tfPoint.y - cball->pos.y;
            d14.z = tfPoint.z - cball->pos.z;

            d2C.x = cball->pos.x + d14.x * normalSlot.value.x;
            d2C.y = cball->pos.y + d14.y * normalSlot.value.y;
            d2C.z = cball->pos.z + d14.z * normalSlot.value.z;

            mtxa_rigid_inv_tf_point(&d2C, &d14);

            mtxa_from_mtx(itemGroup->prevTransform);
            mtxa_tf_point(&d14, &d20);

            d14.x = d2C.x - d20.x;
            d14.y = d2C.y - d20.y;
            d14.z = d2C.z - d20.z;

            sp20.x -= d14.x;
            sp20.y -= d14.y;
            sp20.z -= d14.z;
        }

        mtxa_from_mtx(itemgroups[physBall->hardestColiItemGroupId].transform);
        mtxa_tf_vec(&physBall->hardestColiNormal, &sp14);
        sp38.x = -sp14.x * ball->currRadius;
        sp38.y = -sp14.y * ball->currRadius;
        sp38.z = -sp14.z * ball->currRadius;

        mtxa_from_rotate_y(ball->rotY);
        mtxa_rotate_x(ball->rotX);
        mtxa_rotate_z(ball->rotZ);
        mtxa_rigid_inv_tf_vec(&sp38, &sp38);

        sp44.x = -sp20.x;
        sp44.y = -sp20.y;
        sp44.z = -sp20.z;
        mtxa_rigid_inv_tf_vec(&sp44, &sp44);
        mathutil_vec_cross_prod(&sp38, &sp44, &sp2C);

        f2 = 0.0f;
        if (ball->currRadius > FLT_EPSILON)
            f2 = (32768.0 / 3.141592) / (ball->currRadius * ball->currRadius);
        ball->unk60.x = sp2C.x * f2;
        ball->unk60.y = sp2C.y * f2;
        ball->unk60.z = sp2C.z * f2;

        if (ball->flags & 0x400)
            apply_coli_friction(physBall, &sp20, &sp38);

        if (ball->currRadius > FLT_EPSILON)
        {
            f1 = mathutil_vec_len(&sp2C);
            if (f1 != 0.0f)
            {
                f1 = vec_normalize_len(&sp2C);
                quat_from_axis_angle(
                    &ball->unkA0,
                    &sp2C,
                    (f1 * 2.0f) / (ball->currRadius * ball->currRadius * 3.14159265) * 10430.3779296875);
            }
        }
    }
    else if (ball->flags & 0x400)
    {
        ball->vel.x *= 0.95;
        ball->vel.y *= 0.95;
        ball->vel.z *= 0.95;

        if (ball->unkA0.w > 0.99971997737884521f)
        {
            ball->unkA0.w = 1.0f;
            ball->unkA0.x = ball->unkA0.y = ball->unkA0.z = 0.0f;
        }
        else
        {
            ball->unkA0.w /= 0.95f;
            ball->unkA0.x *= 0.95f;
            ball->unkA0.y *= 0.95f;
            ball->unkA0.z *= 0.95f;
        }
    }
}

void set_ball_properties(struct Ball *ball, int arg)
{
    float *params = &physical_ball_size[arg * 3];

    ball->currRadius = params[0];
    ball->unk14C = params[0];
    ball->modelScale = 1.0f;
    ball->accel = params[1];
    ball->restitution = params[2];
    ball->unk1 = arg;
    if (g_effect_flag_ptr != NULL && (*g_effect_flag_ptr & 0x80))
    {
        if (modeCtrl.playerCount > 1)
            ball->colorId = ball->playerId;
        else
            ball->colorId = 3;
    }
}

void ball_collision_stars(struct Ball *ball)
{
    struct Effect effect;
    Vec sp24;
    Vec sp18;
    Vec spC;
    int r31;
    int r30;
    float f25;
    float f0;

    sp24.x = -ball->unk11C.x;
    sp24.y = -ball->unk11C.y;
    sp24.z = -ball->unk11C.z;

    sp18.x = ball->vel.x * 0.5f;
    sp18.y = ball->vel.y * 0.5f;
    sp18.z = ball->vel.z * 0.5f;

    spC.x = ball->pos.x + ball->unk11C.x * ball->currRadius;
    spC.y = ball->pos.y + ball->unk11C.y * ball->currRadius;
    spC.z = ball->pos.z + ball->unk11C.z * ball->currRadius;

    f0 = -mathutil_vec_dot_prod(&sp24, &sp18);
    sp18.x += f0 * sp24.x;
    sp18.y += f0 * sp24.y;
    sp18.z += f0 * sp24.z;

    r30 = fabs(ball->unk13C / 0.0165f);
    if (r30 > 32)
        r30 = 32;

    f25 = fabs((float)(ball->unk13C / 0.32999999821186066)) + 1.0;

    memset(&effect, 0, sizeof(effect));

    effect.playerId = ball->playerId;
    effect.type = ET_COLIFLASH;
    effect.pos = spC;
    effect.unk8C = sp24;
    effect.colorFactor = ball->unk13C;
    spawn_effect(&effect);

    memset(&effect, 0, sizeof(effect));

    effect.playerId = ball->playerId;
    effect.type = ET_COLISTAR_PARTICLE;
    effect.pos = spC;

    for (r31 = r30 >> 1; r31 > 0; r31--)
    {
        effect.vel.x = sp18.x + f25 * (RAND_FLOAT() * 0.05 - 0.025);
        effect.vel.y = sp18.y + f25 * (RAND_FLOAT() * 0.05 - 0.025);
        effect.vel.z = sp18.z + f25 * (RAND_FLOAT() * 0.05 - 0.025);

        f0 = f25 * (RAND_FLOAT() * 0.055f + 0.015f);
        effect.vel.x += f0 * sp24.x;
        effect.vel.y += f0 * sp24.y;
        effect.vel.z += f0 * sp24.z;

        spawn_effect(&effect);
    }
    r30 -= r30 >> 1;

    memset(&effect, 0, sizeof(effect));

    effect.playerId = ball->playerId;
    effect.type = ET_COLI_PARTICLE;
    effect.pos = spC;
    sp18.x *= 0.5f;
    sp18.y *= 0.5f;
    sp18.z *= 0.5f;

    for (r31 = r30; r31 > 0; r31--)
    {
        effect.vel.x = sp18.x + f25 * (RAND_FLOAT() * 0.05 - 0.025);
        effect.vel.y = sp18.y + f25 * (RAND_FLOAT() * 0.05 - 0.025);
        effect.vel.z = sp18.z + f25 * (RAND_FLOAT() * 0.05 - 0.025);

        f0 = f25 * (RAND_FLOAT() * 0.05f + 0.06f);
        effect.vel.x += f0 * sp24.x;
        effect.vel.y += f0 * sp24.y;
        effect.vel.z += f0 * sp24.z;

        spawn_effect(&effect);
    }
}

void init_physicsball_from_ball(struct Ball *ball, struct PhysicsBall *physBall)
{
    physBall->flags = 0;
    physBall->pos.x = ball->pos.x;
    physBall->pos.y = ball->pos.y;
    physBall->pos.z = ball->pos.z;
    physBall->prevPos.x = ball->prevPos.x;
    physBall->prevPos.y = ball->prevPos.y;
    physBall->prevPos.z = ball->prevPos.z;
    physBall->vel.x = ball->vel.x;
    physBall->vel.y = ball->vel.y;
    physBall->vel.z = ball->vel.z;
    physBall->radius = ball->currRadius;
    physBall->gravityAccel = ball->accel;
    physBall->restitution = ball->restitution;
    physBall->hardestColiSpeed = 0.0f;
    physBall->itemGroupId = 0;
    physBall->hardestColiItemGroupId = 0;
    physBall->friction = 0.01f;
}

void apply_physicsball_to_ball(struct Ball *ball, struct PhysicsBall *physBall)
{
    if (physBall->flags & 1)
        ball->flags |= 1;
    ball->pos.x = physBall->pos.x;
    ball->pos.y = physBall->pos.y;
    ball->pos.z = physBall->pos.z;
    ball->vel.x = physBall->vel.x;
    ball->vel.y = physBall->vel.y;
    ball->vel.z = physBall->vel.z;
}

void g_ball_ape_rotation(struct Ball *ball)
{
    Vec spC;
    int r31;
    int r30;
    float f1;
    float f3;
    int var;

    if (ball->ape == NULL)
    {
        ball->unk92 = 0;
        return;
    }
    mtxa_from_quat(&ball->ape->rot);
    spC.x = 0.0f;
    spC.y = 0.0f;
    spC.z = -1.0f;
    mtxa_tf_vec(&spC, &spC);

    r31 = math_atan2(spC.x, spC.z) - 32768;
    r30 = math_atan2(ball->vel.x, ball->vel.z) - 32768;

    f1 = math_sqrt(mathutil_sum_of_sq_2(ball->vel.x, ball->vel.z));
    if (f1 < 0.23148148148148145)
        f3 = 0.0f;
    else if (f1 < 0.37037037037037035)
        f3 = (f1 - 0.23148148148148145) / 0.1388888888888889;
    else
        f3 = 1.0f;

    var = (s16)(r30 - r31);

    ball->unk92 = r31 + var * f3;
}

void spawn_postgoal_ball_sparkle(void)
{
    struct Ball *ball = current_ball;
    int bvar;

    if (ball->flags & (1ULL << 43))
        return;
    if (!(ball->flags & 0x40))
        return;

    if (modeCtrl.gameType != 1)
        bvar = ((ball->flags & 0x200) || (ball->unk80 & 1));
    else if (ball->flags & 0x200)
        bvar = ball->unk80 & 1;
    else
        bvar = (ball->unk80 & 3) == 1;

    if (bvar)
    {
        struct Effect effect;

        memset(&effect, 0, sizeof(effect));
        effect.type = ET_LEVITATE;
        effect.playerId = ball->playerId;
        effect.pos = ball->pos;
        spawn_effect(&effect);
    }
}

void g_some_ballfunc(struct Ball *ball)
{
    ball->flags &= ~0xF8000000ULL;
}

static const s16 s_idleChatterSounds[] = { 0x3A, 0x3B, 0x3F, 0x53, 0x54, 0x55, 0x56, 0x57 };

static inline void idle_chatter_ape_idle(struct Ball *ball)
{
    if (ball->ape->unk74 == 0 && ball->ape->unk78 == 4)
        lbl_1_bss_70F00[ball->playerId]++;
    else
        lbl_1_bss_70F00[ball->playerId] = 0;

    if ((lbl_1_bss_70F00[ball->playerId] % 30) == 1)
        call_SoundReqID_arg_0(s_idleChatterSounds[(rand() & 0x7FFF) >> 12]);
}

static inline void idle_chatter_ape_state(struct Ball *ball)
{
    if (ball->ape->unk250 == 2)
        lbl_1_bss_70F00[ball->playerId]++;
    else
        lbl_1_bss_70F00[ball->playerId] = 0;

    if ((lbl_1_bss_70F00[ball->playerId] % 30) == 1)
        call_SoundReqID_arg_0(s_idleChatterSounds[(rand() & 0x7FFF) >> 12]);
}

void ball_sounds_gameplay(struct Ball *ball)
{
    switch (modeCtrl.gameType)
    {
    case 4:
        if (!(ball->flags & (1ULL << 37)) && ball->unk80 > 0x78)
            idle_chatter_ape_idle(ball);
        break;
    case 7:
        idle_chatter_ape_state(ball);
        break;
    case 8:
        break;
    default:
        if (g_some_frame_counter.frameCounter > 0xF0
         && (u32)(sub_mode - 0x41) > 1
         && (u32)(sub_mode - 0x3D) > 1
         && (u32)(sub_mode - 0x51) > 1)
        {
            if ((!(mode_info.flags & 0x20) && !(mode_info.flags & 8))
             || (mode_info.flags & 0x10))
            {
                if (!(ball->flags & 0x100) && !(ball->flags & 0x200))
                    idle_chatter_ape_idle(ball);
            }
        }
        break;
    }

    if (ball->state != 6
     && g_some_frame_counter.frameCounter > 0xF0
     && ball->ape->unk250 == 4)
    {
        float f1 = lbl_1_bss_70E70[ball->playerId] * 216.0f;
        if (f1 >= 35.0f)
            call_SoundReqID_arg_0(0x1A);
        else if (f1 >= 20.0f)
            call_SoundReqID_arg_0(0x18);
        else
            call_SoundReqID_arg_0(0x17);
    }
    if (ball->flags & (1 << 27))
        call_SoundReqID_arg_0(0x69);
    else if (ball->flags & (1 << 28))
        call_SoundReqID_arg_0(0x2F);
    else if (ball->flags & (1 << 29))
        call_SoundReqID_arg_0(0x14);
    else if (ball->flags & (1 << 30))
        call_SoundReqID_arg_0(0x13);
    else if (ball->flags & 0x80000000U)
        call_SoundReqID_arg_0(0x12);

    if (sub_mode == 0x54)
        return;

    if (modeCtrl.gameType != 3 && modeCtrl.gameType != 13 && modeCtrl.gameType != 7)
    {
        s8 r4;
        s8 r5;
        float f4 = mathutil_vec_len(&ball->vel) * 216.0f;

        if ((ball->flags & 1) && f4 > 10.0)
        {
            r4 = MIN((f4 - 10.0) * 1.5, 127.0);
            r5 = MIN((f4 - 10.0) * 15.0, 80.0) * 0.85000002384185791;
        }
        else
        {
            r4 = 0;
            r5 = 0;
        }
        if ((g_font_char_var_3 & 7) == 0 || r4 == 0)
            SoundIcsReq(ball->playerId, r4, r5);
    }
}

void ball_movement_sparks(struct Ball *ball)
{
    float f26;

    if (!(ball->flags & 1))
        return;

    f26 = mathutil_vec_len(&ball->vel);
    f26 *= 5.0;
    if (f26 > 1.5f)
    {
        Vec spC4;
        struct Effect effect;
        Vec spC;
        float f2;
        int r29;

        spC.x = -ball->unk11C.x;
        spC.y = -ball->unk11C.y;
        spC.z = -ball->unk11C.z;

        memset(&effect, 0, sizeof(effect));

        effect.type = ET_COLI_PARTICLE;
        effect.playerId = ball->playerId;

        effect.pos.x = ball->pos.x + ball->unk11C.x * ball->currRadius;
        effect.pos.y = ball->pos.y + ball->unk11C.y * ball->currRadius;
        effect.pos.z = ball->pos.z + ball->unk11C.z * ball->currRadius;

        if (!(mode_info.flags & 0x10) || (mode_info.flags & 0x800))
            f2 = 0.85f;
        else
            f2 = 0.1f;

        spC4.x = ball->vel.x * f2;
        spC4.y = ball->vel.y * f2;
        spC4.z = ball->vel.z * f2;

        for (r29 = f26; r29 > 0; r29--)
        {
            float f25 = RAND_FLOAT() * f26 * 0.1;

            effect.vel.x = (spC.x + (RAND_FLOAT() * 1.5 - 0.75)) * f25 + spC4.x;
            effect.vel.y = (spC.y + (RAND_FLOAT() * 1.5 - 0.75)) * f25 + spC4.y;
            effect.vel.z = (spC.z + (RAND_FLOAT() * 1.5 - 0.75)) * f25 + spC4.z;

            spawn_effect(&effect);
        }
    }
}

void set_visual_scale(struct Ball *ball)
{
    if (!(ball->flags & (3ULL << 33)))
    {
        if (ball->currRadius != ball->unk14C)
        {
            ball->currRadius += (ball->unk14C - ball->currRadius) * 0.2f;
            ball->modelScale = ball->currRadius / ball->unk14C;
        }
    }
    else
    {
        ball->unk148--;
        if (ball->unk148 < 0)
            ball->flags &= ~(3ULL << 33);
        else if (ball->flags & (1ULL << 33))
        {
            float target = ball->unk14C * 0.5f;
            ball->currRadius += (target - ball->currRadius) * 0.3f;
        }
        else
        {
            float target = ball->unk14C * 2.0f;
            ball->currRadius += (target - ball->currRadius) * 0.3f;
        }
        ball->modelScale = ball->currRadius / ball->unk14C;
    }

    if (ball->ape != NULL)
        ball->ape->scale = ball->modelScale;
}

/* Rates the remaining time: 0 (more than 80% left) to 4 (20% or less) */
int fn_1_4CD70(void)
{
    int curr;
    int max;
    int ret;

    max = mode_info.timerMax;
    curr = mode_info.timerCurr;
    ret = 4;

    if (curr > 0.8 * max)
        ret = 0;
    else if (curr > 0.6 * max)
        ret = 1;
    else if (curr > 0.4 * max)
        ret = 2;
    else if (curr > 0.2 * max)
        ret = 3;
    return ret;
}
