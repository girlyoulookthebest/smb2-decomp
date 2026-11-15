#pragma peephole off

/* this file calls mtxa_rotate_y with an int argument */
#define mtxa_rotate_y mtxa_rotate_y_s16
#include "types.h"
#include "gx.h"
#include "avdisp.h"
#include "ball.h"
#include "background.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"
#include "stage.h"
#undef mtxa_rotate_y

/* Podium scene shown after mini games: state in g_some_flag_relating_to_pausing */
struct CommendWork
{
    /*0x00*/ u32 flags;
    /*0x04*/ struct Ape *apes[8];
    /*0x24*/ s8 count;
    /*0x25*/ s8 apeCount;
    /*0x26*/ s8 ranks[8];
    /*0x2E*/ s8 order[8];
    /*0x36*/ s8 isCpu[8];
    u8 unk3E[2];
    /*0x40*/ void *tpl;
    /*0x44*/ struct GMA *gma;
    /*0x48*/ Mtx mtx;
    /*0x78*/ s8 textId;
    u8 unk79[3];
    /*0x7C*/ int timer;
    /*0x80*/ void *heap;
    /*0x84*/ GXColor color;
    u8 unk88[0xA0 - 0x88];
}; /* size = 0xA0 */

/* Argument block for fn_1_8A70 */
struct CommendFontCtx
{
    /*0x000*/ s32 flags;
    /*0x004*/ s32 unk4;
    /*0x008*/ s32 unk8;
    /*0x00C*/ GXColor color;
    /*0x010*/ s32 unk10;
    /*0x014*/ float unk14;
    /*0x018*/ float unk18;
    /*0x01C*/ float unk1C;
    /*0x020*/ float unk20;
    /*0x024*/ float unk24;
    /*0x028*/ s32 unk28;
    /*0x02C*/ float unk2C;
    /*0x030*/ float unk30;
    /*0x034*/ s32 unk34;
    /*0x038*/ float unk38;
    /*0x03C*/ float unk3C;
    /*0x040*/ s32 unk40;
    /*0x044*/ s32 count;
    /*0x048*/ struct GMAModel *models[32];
    /*0x0C8*/ Mtx mtxs[32];
    /*0x6C8*/ Vec unk6C8;
}; /* size = 0x6D4 */

struct CommendSound
{
    int frame;
    int soundId;
};

struct CommendLabel
{
    /*0x00*/ char **text;
    /*0x04*/ float yOffset;
    /*0x08*/ float scale;
    /*0x0C*/ u32 flags;
    /*0x10*/ struct CommendSound *sounds;
    /*0x14*/ int soundCount;
}; /* size = 0x18 */

struct CommendAnimTrack
{
    int count;
    struct StageKeyframe * DPTR keyframes;  /* DPTR: u32 word tables (data_init_gen.c) */
};

struct CommendWork g_some_flag_relating_to_pausing;
#define work g_some_flag_relating_to_pausing

extern u32 g_font_char_var_3;
extern u32 locale;
extern s32 sub_mode_request;
extern void *currentHeap;
extern struct CommendAnimTrack * DPTR * DPTR lbl_1_data_5CAFC[];
extern float lbl_1_data_2E418[][3];

void mtxa_rotate_y(int angle);
void fn_1_E4F70(void);
void fn_1_E41A8(int textId);
void fn_1_E5E78(int idx, Vec *pos);
void fn_1_E6384(Vec *pos);
int fn_1_E5DC8(void);
int fn_1_E5E44(void);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_put_buf_1(char *str);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void fn_1_8A70(struct CommendFontCtx *ctx);
void *OSSetCurrentHeap(void *heap);
void g_something_with_sound7_and_game_heaps(int a);
void load_stage(int id);
void set_sub_mode_destination(void (*func)(void));
void ord_tbl_draw_nodes(void);
void g_draw_stage(void);
void g_draw_bg(void);
void *g_load_tpl(char *name);
struct GMA *g_load_gma(char *name, void *tpl);
void DVDChangeDir(char *dir);
struct Ape *load_ape(int chara, int a, int b, int c);
void g_some_set_ape_anim(struct Ape *ape, int a, int b);
void run_anim_funcs(struct Ape *ape);
void play_track_and_fade_out_other_tracks(int track, int a, int b);
void gp_wait_wrapper(void);
void g_something_freeing_heap(void *p);
void g_something_freeing_heap_2(void *p);
void g_something_with_freeing_chara_heap_ape(struct Ape *ape);
float fn_1_1135E8(struct Ape *ape);
float body_frame_add72(struct Ape *ape);
float (*g_get_ape_anim_smth(struct Ape *ape, int a))[4];
void draw_ape(struct Ape *ape);
void *g_yet_another_unk_draw_func(void *func);
void g_smth_with_gpu_and_locked_cache_mtx(void);
void *memset(void *p, int c, unsigned int n);
unsigned int strlen(char *s);

void fn_1_E4094(void);
void g_something_with_drawing_apes(void);
void fn_1_E41A8(int textId);
void fn_1_E4584(s8 *charas, s8 *ranks, s8 *order, int count);
void fn_1_E4708(s8 *charas, s8 *ranks);
void fn_1_E4DFC(s8 *charas, s8 *ranks, s8 id);
void fn_1_E4F70(void);
void g_something_with_rotation_not_aiai(struct Ape *ape);
int fn_1_E5DC8(void);
int fn_1_E5E44(void);
void fn_1_E5E78(int idx, Vec *pos);
void fn_1_E6384(Vec *pos);
void fn_1_E6760(Mtx mtx);
void fn_1_E67BC(int a);
void fn_1_E74AC(int a, int b);

static u32 lbl_1_data_53430[] =
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

/* keeps the table in .data; nothing in this file reads it directly */
static void *unused_data_53430(void)
{
    return lbl_1_data_53430;
}

void smd_mini_commend_init(void)
{
    s8 charas[4];
    s8 ranks[4];

    dest_all_events();
    g_something_with_sound7_and_game_heaps(1);
    load_stage(3);
    event_init(0);
    event_init(0x14);
    event_init(0x10);
    event_init(0x11);
    event_init(0xE);
    event_init(0x13);
    event_init(0xC);
    enable_cameras_for_active_players();
    camera_set_state(0x46);
    cameras[0].eye.x = -0.5f;
    cameras[0].eye.y = 1.3f;
    cameras[0].eye.z = -2.5f;
    cameras[0].lookAt.x = -0.5f;
    cameras[0].lookAt.y = 0.9f;
    cameras[0].lookAt.z = 0.0f;
    charas[0] = 0;
    ranks[0] = 0;
    charas[1] = 0x41;
    ranks[1] = 1;
    charas[2] = 0x42;
    ranks[2] = 2;
    charas[3] = 0x43;
    ranks[3] = 3;
    fn_1_E4708(charas, ranks);
    modeCtrl.unk10 = 0;
    set_sub_mode_destination(fn_1_E4094);
    sub_mode_request = 0xB4;
}

#define SET4(c0, r0, c1, r1, c2, r2, c3, r3) \
    charas[0] = c0;                          \
    ranks[0] = r0;                           \
    charas[1] = c1;                          \
    ranks[1] = r1;                           \
    charas[2] = c2;                          \
    ranks[2] = r2;                           \
    charas[3] = c3;                          \
    ranks[3] = r3;                           \
    fn_1_E4708(charas, ranks);

void smd_mini_commend_tick(void)
{
    s8 charas5a[5];
    s8 ranks5a[5];
    s8 charas5b[5];
    s8 ranks5b[5];
    s8 charas5c[5];
    s8 ranks5c[5];
    s8 charas8[8];
    s8 ranks8[8];
    s8 charas[4];
    s8 ranks[4];
    int i;
    int gameType;

    if (g_some_other_flags & 0xA)
        return;
    i = modeCtrl.unk10;
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_RIGHT)
    {
        i++;
        if (i >= 14)
            i = 0;
    }
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_LEFT)
    {
        i--;
        if (i < 0)
            i = 13;
    }
    if (i == modeCtrl.unk10)
        return;
    switch (i)
    {
    case 0:
        SET4(0, 0, 1, 1, 2, 2, 3, 3)
        break;
    case 1:
        SET4(0, 3, 1, 0, 2, 1, 3, 2)
        break;
    case 2:
        SET4(0, 2, 1, 3, 2, 0, 3, 1)
        break;
    case 3:
        SET4(0, 1, 1, 2, 2, 3, 3, 0)
        break;
    case 4:
        SET4(0, 3, 0x41, 0, 0x42, 1, 0x43, 2)
        break;
    case 5:
        SET4(0x40, 0, 1, 1, 2, 2, 3, 3)
        break;
    case 6:
        SET4(0, 0, 0x41, 1, 0x42, 2, 0x43, 3)
        break;
    case 7:
        SET4(0, 0, 1, 0, 2, 0, 3, 0)
        break;
    case 8:
        SET4(0, 3, 1, 3, 2, 3, 3, 3)
        break;
    case 9:
        charas[0] = 0;
        ranks[0] = 2;
        charas[1] = -1;
        charas[2] = -1;
        charas[3] = -1;
        fn_1_E4708(charas, ranks);
        break;
    case 10:
        charas5a[0] = 0x40;
        ranks5a[0] = 3;
        charas5a[1] = 0x41;
        ranks5a[1] = 3;
        charas5a[2] = 0x42;
        ranks5a[2] = 3;
        charas5a[3] = 0x43;
        ranks5a[3] = 3;
        charas5a[4] = 2;
        ranks5a[4] = 0;
        fn_1_E4DFC(charas5a, ranks5a, 0);
        fn_1_E67BC(0xC);
        break;
    case 11:
        charas5b[0] = 0;
        ranks5b[0] = 0;
        charas5b[1] = 1;
        ranks5b[1] = 0;
        charas5b[2] = 2;
        ranks5b[2] = 0;
        charas5b[3] = 3;
        ranks5b[3] = 0;
        charas5b[4] = 2;
        ranks5b[4] = 0;
        fn_1_E4DFC(charas5b, ranks5b, 0);
        fn_1_E74AC(0, 1);
        break;
    case 12:
        charas5c[0] = 0;
        ranks5c[0] = 3;
        charas5c[1] = 1;
        ranks5c[1] = 3;
        charas5c[2] = 2;
        ranks5c[2] = 3;
        charas5c[3] = 3;
        ranks5c[3] = 3;
        charas5c[4] = 2;
        ranks5c[4] = 3;
        fn_1_E4DFC(charas5c, ranks5c, 0);
        fn_1_E74AC(1, 0);
        fn_1_E67BC(2);
        break;
    case 13:
        gameType = modeCtrl.gameType;
        modeCtrl.gameType = 3;
        charas8[0] = 0;
        ranks8[0] = 7;
        charas8[1] = 1;
        ranks8[1] = 6;
        charas8[2] = 2;
        ranks8[2] = 5;
        charas8[3] = 3;
        ranks8[3] = 4;
        charas8[4] = 0x40;
        ranks8[4] = 3;
        charas8[5] = 0x41;
        ranks8[5] = 2;
        charas8[6] = 0x42;
        ranks8[6] = 1;
        charas8[7] = 0x43;
        ranks8[7] = 0;
        fn_1_E4708(charas8, ranks8);
        modeCtrl.gameType = gameType;
        break;
    }
    mtxa_from_translate_xyz(0.0f, 10.0f, 0.0f);
    fn_1_E6760(mtxa->mtxA);
    camera_set_state(0x46);
    modeCtrl.unk10 = i;
}

void fn_1_E4094(void)
{
    switch (sub_mode)
    {
    case 0xB3:
    case 0xB4:
        set_sub_mode_destination(fn_1_E4094);
        break;
    default:
        dest_all_events();
        g_something_with_sound7_and_game_heaps(0);
        break;
    }
}

void g_mini_commend_draw_func(void)
{
    debug_textdraw_set_pos(1, 1);
    debug_textdraw_put_buf_1("MINI COMMEND");
    debug_textdraw_set_pos(3, 3);
    debug_textdraw_vsprintf_buf_1("\x1C [ %d / %d ]", modeCtrl.unk10, 14);
    if (events[0].state == 2)
        g_draw_stage();
    if (events[20].state == 2)
        g_draw_bg();
    g_something_with_drawing_apes();
    if (events[14].state == 2)
        g_draw_effects();
    ord_tbl_draw_nodes();
}

/* model index by player colour and by rank */
static const s8 commendPlayerGlyphs[4] = { 1, 3, 5, 7 };
static const s8 commendPodiumRanks[5] = { 0, 1, 2, 3, 4 };
static const float commendPodiumHeights[8] = { 1.0f, 0.65f, 0.4f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

/* '0'..'Z' to commend.gma glyph model */
static s8 commendGlyphs[] =
{
    0x2A, 0x1A, 0x23, 0x22, 0x0F, 0x0E, 0x20, 0x1F, 0x0B, 0x18, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0D, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x19, 0x1B, 0x1C, 0x1D, 0x1E, 0x21, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x00,
};

#define TEXT1(s) { s, "", "", "", "", s }
#define TEXT2(s, t) { s, "", "", "", "", t }

static char *textCongratulations[] = TEXT1("CONGRATULATIONS:");
static char *textYouLose[] = TEXT1("YOU LOSE");
static char *textYouRetire[] = TEXT1("YOU RETIRE");
static char *textComWins[] = TEXT2("COM WINS", "COM WIN");
static char *text1PWins[] = TEXT2("1P WINS", "1P WIN");
static char *text2PWins[] = TEXT2("2P WINS", "2P WIN");
static char *text3PWins[] = TEXT2("3P WINS", "3P WIN");
static char *text4PWins[] = TEXT2("4P WINS", "4P WIN");
static char *textDrawGame[] = TEXT1("DRAW GAME");
static char *textRankIn[] = TEXT1("RANK IN");
static char *textNone[] = { "", "", "", "", "", "" };
static char *textYouWin[] = TEXT1("YOU WIN");

static struct CommendSound sounds0[] = { { -1, 0x1E9 } };
static struct CommendSound sounds1[] = { { 0, 0x1E9 } };
static struct CommendSound sounds2[] = { { 0, 0x1F0 } };
static struct CommendSound sounds3[] = { { 0, 0x1E9 }, { 0x82, 0x1F1 } };
static struct CommendSound sounds4[] = { { 0, 0x1E9 }, { 0x82, 0x1EA }, { 0xBE, 0x1EF } };
static struct CommendSound sounds5[] = { { 0, 0x1E9 }, { 0x82, 0x1EB }, { 0xBE, 0x1EF } };
static struct CommendSound sounds6[] = { { 0, 0x1E9 }, { 0x82, 0x1EC }, { 0xBE, 0x1EF } };
static struct CommendSound sounds7[] = { { 0, 0x1E9 }, { 0x82, 0x1ED }, { 0xBE, 0x1EF } };
static struct CommendSound sounds8[] = { { 0, 0x1DB } };
static struct CommendSound sounds9[] = { { 0, 0x1E9 }, { 0x82, 0x1EE } };

static struct CommendLabel commendLabels[] =
{
    { textCongratulations, 1.64999998f, 0.25f, 0, sounds1, 1 },
    { textCongratulations, 1.89999998f, 0.219999999f, 0, sounds0, 1 },
    { textYouLose, 1.54999995f, 0.319999993f, 0, sounds2, 1 },
    { textYouRetire, 1.04999995f, 0.319999993f, 0, sounds8, 1 },
    { textComWins, 1.42999995f, 0.319999993f, 1, sounds1, 1 },
    { text1PWins, 1.42999995f, 0.319999993f, 1, sounds4, 3 },
    { text2PWins, 1.42999995f, 0.319999993f, 1, sounds5, 3 },
    { text3PWins, 1.42999995f, 0.319999993f, 1, sounds6, 3 },
    { text4PWins, 1.42999995f, 0.319999993f, 1, sounds7, 3 },
    { textDrawGame, 1.23000002f, 0.319999993f, 1, sounds0, 1 },
    { textRankIn, 1.42999995f, 0.319999993f, 1, sounds9, 2 },
    { textNone, 0.0f, 0.0f, 0, sounds0, 1 },
    { textYouWin, 1.42999995f, 0.319999993f, 1, sounds3, 2 },
};

static struct StageKeyframe commendKeyframes[] =
{
    { 2, 0.0f, 0.0f, -1.0f, -0.782549977f },
    { 2, 18.3894176f, -62.6189117f, -2.58384991f, -2.58384991f },
    { 2, 35.5168228f, -74.2648697f, 0.187234998f, 0.187234998f },
    { 2, 49.6394196f, -50.9729538f, 3.22981095f, 3.22981095f },
    { 2, 79.3870163f, 337.225677f, 7.43835115f, 7.43835115f },
    { 2, 139.48317f, 329.4617f, 0.0f, 0.0f },
    { 2, 200.0f, 360.0f, 0.0f, -1.0f },
};

void fn_1_E41A8(int textId)
{
    struct CommendFontCtx ctx;
    Mtx mtx;
    Vec v;
    Vec pos;
    int i;
    s8 len;
    u8 *str;
    struct CommendLabel *label;
    struct GMAModel *model;

    if (work.count > 0)
    {
        fn_1_E6384(&pos);
        memset(&ctx, 0, sizeof(ctx));
        ctx.flags = 3;
        if (work.flags & 0x200)
            ctx.flags |= 4;
        ctx.unk4 = 4;
        ctx.unk8 = 4;
        ctx.color = work.color;
        ctx.unk10 = 0;
        ctx.unk14 = 0.5f;
        ctx.unk18 = 1.5f;
        ctx.unk1C = 0.0f;
        ctx.unk20 = 0.0f;
        ctx.unk24 = 0.0f;
        ctx.unk28 = 0;
        ctx.unk2C = -0.3f;
        ctx.unk30 = 0.025f;
        ctx.unk34 = 0;
        ctx.unk38 = 0.0f;
        ctx.unk3C = 0.0f;
        ctx.unk40 = 0;
        mtxa_from_identity();
        mtxa_rotate_x(0x1C00);
        mtxa_tf_point_xyz(0.0f, 0.0f, -1.0f, &v);
        mtxa_from_identity();
        mtxa_rotate_z((s16)-(g_font_char_var_3 << 9));
        mtxa_tf_vec(&v, &v);
        ctx.unk6C8 = v;
        label = &commendLabels[textId];
        mtxa_from_identity();
        mtxa_scale_s(2.0f);
        mtxa_to_mtx(mtx);
        if (textId == 1)
        {
            pos.y += label->yOffset;
            mtxa_from_mtxb_translate(&pos);
            mtxa_scale_s(label->scale);
            mtxa_mult_left(mtx);
            mtxa_to_mtx(mtx);
        }
        else
        {
            pos.y += label->yOffset;
            mtxa_from_mtxb_translate(&pos);
            mtxa_scale_s(label->scale);
            mtxa_mult_left(mtx);
            mtxa_to_mtx(mtx);
        }
        str = (u8 *)label->text[locale];
        len = strlen((char *)str);
        for (i = 0; i < len; i++, str++)
        {
            float f;

            v.x = 1.14 * (0.5 * (len - 1)) - 1.14 * i;
            v.y = 0.0f;
            v.z = 0.0f;
            mtxa_from_mtx(mtx);
            mtxa_translate(&v);
            mtxa_rotate_y(-0x8000);
#ifdef __MWERKS__
            model = work.gma->modelEntries[commendGlyphs[*str - '0']].model;
#else
            /* ' ' is below '0': on GC this read the byte 16 before the table
             * (0x1C, the end of the debug string "\x1C [ %d / %d ]"); spaces
             * are not drawn, but the model is used for the transform */
            model = work.gma->modelEntries[*str == ' ' ? 0x1C : commendGlyphs[*str - '0']].model;
#endif
            mtxa_translate(&model->boundSphereCenter);
            f = evaluate_stagedef_keyframe(g_font_char_var_3 % 200, 7, commendKeyframes);
            mtxa_rotate_y(182.04445f * f);
            mtxa_translate_neg(&model->boundSphereCenter);
            if (*str != ' ')
            {
                ctx.models[ctx.count] = model;
                mtx_copy(mtxa->mtxA, ctx.mtxs[ctx.count]);
                ctx.count++;
            }
        }
        if (len > 0)
            fn_1_8A70(&ctx);
    }
}

void fn_1_E4584(s8 *charas, s8 *ranks, s8 *order, int count)
{
    int humans = 0;
    int winners = 0;
    int id = 0;
    int first = -1;
    int i;

    for (i = 0; i < count; i++)
    {
        if (!(charas[order[i]] & 0x40))
        {
            humans++;
            if (first < 0)
                first = order[i];
            if (ranks[order[i]] == 0)
                winners++;
        }
        if (id < ranks[order[i]])
            id = ranks[order[i]];
    }
    if (work.flags & 0x20)
        id = 10;
    else if (winners > 1)
        id = 0;
    else if (work.flags & 4)
        id = 3;
    else if (humans == 1)
    {
        if (id == ranks[first] && id != 0)
            id = 2;
        else if (ranks[first] == 0)
            id = 12;
        else
            id = 4;
    }
    else if (humans > 1)
    {
        if (charas[order[0]] & 0x40)
            id = 4;
        else
            id = order[0] + 5;
    }
    else
        id = 0;
    work.textId = id;
}

void fn_1_E4708(s8 *charas, s8 *ranks)
{
    s8 order[8];
    int n;
    int x;
    int i;
    int j;
    struct Ape *ape;

    x = 0;
    if (work.flags & 1)
        fn_1_E4F70();
    work.flags = 1;
    work.textId = 0;
    work.timer = 0;
    work.heap = currentHeap;
    for (i = 0; i < 8; i++)
        work.apes[i] = NULL;
    DVDChangeDir("init");
    work.tpl = g_load_tpl("commend.tpl");
    if (work.tpl != NULL)
        work.gma = g_load_gma("commend.gma", work.tpl);
    DVDChangeDir("..");
    n = 0;
    if (modeCtrl.gameType == 3)
    {
        for (i = 0; i < 8; i++)
        {
            if (charas[i] >= 0)
                {
                order[n] = i;
                n++;
            }
        }
        if (n > 4)
            work.flags |= 0x400;
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            if (charas[i] >= 0)
                {
                order[n] = i;
                n++;
            }
        }
    }
    for (i = 0; i < n - 1; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (ranks[order[i]] > ranks[order[j]])
            {
                s8 temp = order[i];

                order[i] = order[j];
                order[j] = temp;
            }
        }
    }
    if (n <= 1)
        work.flags |= 0x20;
    if (n > 1)
    {
        int all = 1;

        for (i = 0; i < n; i++)
        {
            if (ranks[order[i]] != 0)
            {
                all = 0;
                break;
            }
        }
        if (all)
            work.flags |= 2;
    }
    if (!(work.flags & 2) && n > 1)
    {
        int all = 1;

        for (i = 0; i < n; i++)
        {
            if (n - 1 != ranks[order[i]])
            {
                all = 0;
                break;
            }
        }
        if (all)
            work.flags |= 4;
    }
    if (n == 2 && ranks[order[0]] < ranks[order[1]])
        ranks[order[1]] = 3;
    fn_1_E4584(charas, ranks, order, n);
    if (n > 2 && !(work.flags & 6))
    {
        s8 temp = order[0];

        order[0] = order[1];
        order[1] = temp;
    }
    if (work.flags & 0x400)
    {
        for (i = 0; i < 8; i++)
        {
            if (ranks[order[i]] >= 4)
                ranks[order[i]] = 3;
        }
    }
    work.count = n;
    for (i = 0; i < n; i++)
    {
        work.ranks[i] = (ranks[order[i]] < 0) ? 0 : ((ranks[order[i]] > 4) ? 4 : ranks[order[i]]);
        work.order[i] = order[i];
        work.isCpu[i] = (charas[order[i]] & 0x40) != 0;
        charas[order[i]] &= ~0x40;
    }
    work.apeCount = n;
    if (work.flags & 0x400)
        x = 1;
    for (i = 0; i < work.apeCount; i++)
    {
        if (charas[order[i]] >= 0)
        {
            int rank = work.ranks[i];

            ape = load_ape(charas[order[i]], x, 0x1A, 1);
            ape->flags |= 0x1000000;
            ape->unk29C = rank;
            if (work.flags & 0x20)
                ape->unk29C = 0;
            ape->unk29C |= order[i] << 8;
            ape->unk29C |= i << 16;
            ape->colorIndex = order[i];
            mtxa_from_quat(&ape->rot);
            mtxa_to_quat(&ape->rot);
            g_some_set_ape_anim(ape, 7, ape->unk29C & 0xFF);
            run_anim_funcs(ape);
            work.apes[i] = ape;
        }
    }
    mtxa_from_identity();
    mtxa_to_mtx(work.mtx);
    work.color.r = 0xC0;
    work.color.g = 0xC0;
    work.color.b = 0xC0;
    work.color.a = 0xFF;
    play_track_and_fade_out_other_tracks(0x31, 0x1E, 0x64);
}

void fn_1_E4DFC(s8 *charas, s8 *ranks, s8 id)
{
    struct Ape *ape;
    int i;

    fn_1_E4708(charas, ranks);
    if (charas[4] >= 0)
    {
        ape = load_ape(charas[4], 0, 0x1A, 1);
        ape->flags |= 0x1000000;
        ape->unk29C = ranks[4];
        ape->unk29C |= id << 8;
        ape->colorIndex = id;
        mtxa_from_quat(&ape->rot);
        mtxa_to_quat(&ape->rot);
        g_some_set_ape_anim(ape, 7, ape->unk29C & 0xFF);
        run_anim_funcs(ape);
        work.apes[4] = ape;
        work.flags |= 8;
        work.textId = id + 5;
        work.apeCount++;
        for (i = 0; i < 4; i++)
        {
            if (work.apes[i] != NULL)
                work.apes[i]->colorIndex = 1;
        }
    }
}

void fn_1_E4F70(void)
{
    void *heap;
    int i;

    if (work.flags & 1)
    {
        heap = OSSetCurrentHeap(work.heap);
        if (work.tpl != NULL || work.gma != NULL)
            gp_wait_wrapper();
        if (work.tpl != NULL)
        {
            g_something_freeing_heap_2(work.tpl);
            work.tpl = NULL;
        }
        if (work.gma != NULL)
        {
            g_something_freeing_heap(work.gma);
            work.gma = NULL;
        }
        for (i = 7; i >= 0; i--)
        {
            if (work.apes[i] != NULL)
            {
                g_something_with_freeing_chara_heap_ape(work.apes[i]);
                work.apes[i] = NULL;
            }
        }
        OSSetCurrentHeap(heap);
        work.flags &= ~1;
    }
}

void g_something_with_rotation_not_aiai(struct Ape *ape)
{
    Vec pos;
    Mtx mtx2;
    Mtx mtx1;
    int rx;
    int ry;
    int rz;
    struct CommendAnimTrack *tracks;
    struct CommendAnimTrack * DPTR *table;
    float t;
    float f;

    if (ape->charaId == 0 && ((ape->unk7C == 1 && fn_1_1135E8(ape) > 150.5) || ape->unk7C == 2))
    {
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(mtx1);
        mtxa_from_mtxb_translate(&ape->pos);
        mtxa_mult_right(mtx1);
        mtxa_scale_s(ape->scale);
        mtxa_mult_right(g_get_ape_anim_smth(ape, 3));
        mtxa_scale_s(0.105f);
        mtxa_translate_xyz(-0.4235f, 0.0f, 5.9531f);
        mtxa_rotate_z(-0x1136);
        mtxa_rotate_y(-0x82FD);
        mtxa_rotate_x(0x3E86);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_never(work.gma->modelEntries[47].model);
        return;
    }
    pos.x = pos.y = pos.z = 0.0f;
    rz = 0;
    ry = rz;
    rx = ry;
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(mtx2);
    mtxa_from_mtxb_translate(&ape->pos);
    mtxa_scale_xyz(ape->scale, ape->scale, ape->scale);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(mtx2);
    table = lbl_1_data_5CAFC[ape->charaId];
    if (table != NULL)
    {
        tracks = table[ape->unk7C];
        t = body_frame_add72(ape);
        t = (fn_1_1135E8(ape) - t) / 30.0;
        if (tracks[0].keyframes != NULL)
        {
            f = evaluate_stagedef_keyframe(t, tracks[0].count, tracks[0].keyframes);
            rx = (s16)(182.04445f * f);
        }
        if (tracks[1].keyframes != NULL)
        {
            f = evaluate_stagedef_keyframe(t, tracks[1].count, tracks[1].keyframes);
            ry = (s16)(182.04445f * f);
        }
        if (tracks[2].keyframes != NULL)
        {
            f = evaluate_stagedef_keyframe(t, tracks[2].count, tracks[2].keyframes);
            rz = (s16)(182.04445f * f);
        }
        if (tracks[3].keyframes != NULL)
            pos.x = evaluate_stagedef_keyframe(t, tracks[3].count, tracks[3].keyframes) / 10.0;
        if (tracks[4].keyframes != NULL)
            pos.y = evaluate_stagedef_keyframe(t, tracks[4].count, tracks[4].keyframes) / 10.0;
        if (tracks[5].keyframes != NULL)
            pos.z = evaluate_stagedef_keyframe(t, tracks[5].count, tracks[5].keyframes) / 10.0;
    }
    else
    {
        pos.z += 0.1;
        ry = 0x8000;
    }
    mtxa_translate(&pos);
    mtxa_rotate_z((s16)rz);
    mtxa_rotate_y((s16)ry);
    mtxa_rotate_x((s16)rx);
    mtxa_scale_s(0.1f);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(work.gma->modelEntries[47].model);
}

static float commendShadowScales[5] = { 0.25f, 0.25f, 0.15f, 0.35f, 0.0f };

void g_something_with_drawing_apes(void)
{
    Vec v;
    Vec pos2;
    Vec pos1;
    Mtx saved;
    Mtx mtx;
    int i;
    void *bg;
    struct Ape *ape;
    float s;
    float y;
    int idx;
    int n;
    int c;
    float *p;

    if (work.flags & 1)
    {
        mtx_copy(mtxa->mtxB, saved);
        mtxa_from_mtxb();
        mtxa_mult_right(work.mtx);
        mtxa_to_mtx(mtxa->mtxB);
        bg = world_theme.unk8C;
        if (bg != NULL)
            g_yet_another_unk_draw_func(bg);
        mtxa_from_mtxb();
        mtxa_rotate_y(-0x8000);
        if (work.flags & 0x400)
        {
            mtxa_push();
            mtxa_translate_xyz(1.9f, 0.0f, 0.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(work.gma->modelEntries[48].model);
            mtxa_peek();
            mtxa_translate_xyz(-2.1f, 0.0f, 0.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(work.gma->modelEntries[48].model);
            mtxa_pop();
        }
        else
        {
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(work.gma->modelEntries[48].model);
        }
        if (work.flags & 0x100)
        {
            mtxa_from_mtxb();
            mtxa_rotate_y(-0x8000);
            if (work.flags & 0x800)
                mtxa_translate_xyz(0.5f, 0.5f, 0.0f);
            else
                mtxa_translate_xyz(0.5f, 0.0f, 0.0f);
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_culled_sort_auto(work.gma->modelEntries[52].model);
            avdisp_draw_model_culled_sort_auto(work.gma->modelEntries[commendPodiumRanks[0]].model);
        }
        else
        {
            for (i = 0; i < work.count; i++)
            {
                fn_1_E5E78(i, &pos1);
                mtxa_from_mtxb_translate(&pos1);
                mtxa_rotate_y(-0x8000);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(work.gma->modelEntries[43].model);
                if ((u32)work.ranks[i] < 5)
                {
                    idx = commendPodiumRanks[work.ranks[i]];
                    if (work.count == 2 && work.ranks[i] == 0 && !(work.flags & 2))
                        idx = 5;
                    else if (work.ranks[i] == 0 && (work.flags & 0x1000))
                        idx = 5;
                    avdisp_draw_model_culled_sort_auto(work.gma->modelEntries[idx].model);
                }
            }
            if (work.flags & 8)
            {
                mtxa_from_mtxb_translate(&work.apes[4]->pos);
                mtxa_rotate_y(-0x8000);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(work.gma->modelEntries[43].model);
                avdisp_draw_model_culled_sort_auto(work.gma->modelEntries[5].model);
            }
        }
        if (bg != NULL)
            g_yet_another_unk_draw_func(NULL);
        set_post_mult_color(0.25f, 0.25f, 0.25f, 0.25f);
        avdisp_set_z_mode(1, 3, 0);
        for (i = 0; i < work.count; i++)
        {
            ape = work.apes[i];
            if (ape != NULL)
            {
                float (*anim)[4] = g_get_ape_anim_smth(ape, 0);

                v.x = anim[0][3];
                v.y = 0.0f;
                v.z = anim[2][3];
                mtxa_from_quat(&ape->rot);
                mtxa_to_mtx(mtx);
                mtxa_from_translate(&ape->pos);
                mtxa_scale_xyz(ape->scale, ape->scale, ape->scale);
                mtxa_translate(&ape->unk264);
                mtxa_mult_right(mtx);
                mtxa_tf_point(&v, &v);
                mtxa_from_identity();
                mtxa_scale_s(0.95f);
                mtxa_to_mtx(mtx);
                mtxa_from_mtxb_translate(&v);
                mtxa_mult_left(mtx);
                mtxa_rotate_x(0x4000);
                p = &commendShadowScales[ape->charaId];
                s = *p;
                y = g_get_ape_anim_smth(ape, 0)[1][3];
                if (!(y > 9.0 * s))
                {
                    if (y > s)
                        s *= 1.0 - (y - s) / (8.0 * s);
                    mtxa_scale_s(s);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[103].model);
                }
            }
        }
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        avdisp_set_z_mode(1, 3, 1);
        for (i = 0; i < work.apeCount; i++)
        {
            struct Ape *a = work.apes[i];

            if (a != NULL && (u8)a->unk29C == 0)
                g_something_with_rotation_not_aiai(a);
        }
        for (i = 0; i < work.apeCount; i++)
        {
            struct Ape *a = work.apes[i];

            fn_1_E5E78(i, &a->pos);
            draw_ape(work.apes[i]);
        }
        if (work.count >= 2 && !(work.flags & 0x100))
        {
            n = work.count;
            if (work.flags & 8)
                n = 5;
            for (i = 0; i < n; i++)
            {
                fn_1_E5E78(i, &pos2);
                mtxa_from_mtxb_translate(&pos2);
                mtxa_translate_xyz(0.3f, -0.125f, -0.4f);
                mtxa_rotate_y(-0x8000);
                mtxa_scale_s(0.25f);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                if (work.flags & 8)
                    c = work.apes[i]->colorIndex;
                else
                    c = work.order[i];
                if (work.isCpu[i])
                {
                    float *color = lbl_1_data_2E418[c];

                    set_post_mult_color(color[0], color[1], color[2], 1.0f);
                    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[9].model);
                    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
                }
                else
                    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[commendPlayerGlyphs[c]].model);
            }
        }
        if (fn_1_E5DC8())
        {
            fn_1_E41A8(work.textId);
            if (commendLabels[work.textId].flags & 1)
                fn_1_E41A8(1);
        }
        mtx_copy(saved, mtxa->mtxB);
    }
}

int g_check_flag_related_to_pausing(void)
{
    return (work.flags & 1) != 0;
}

int fn_1_E5CD0(void)
{
    u32 mask;
    int i;

    for (i = 0; i < fn_1_E5E44(); i++)
    {
        if (cameras[0].timerCurr < (i + 1) * 0x110)
        {
            int j = i;

            if (work.count > 2)
            {
                if (j == 0)
                    j = 1;
                else if (j == 1)
                    j = 0;
            }
            return 1 << work.order[j];
        }
    }
    mask = 0;
    for (i = 0; i < fn_1_E5E44(); i++)
        mask |= 1 << work.order[i];
    return mask;
}

int fn_1_E5DC8(void)
{
    return cameras[0].timerCurr >= fn_1_E5E44() * 0x110;
}

int fn_1_E5E08(void)
{
    if (work.flags & 0x500)
        return 0;
    return !(work.flags & 0x26);
}

int fn_1_E5E44(void)
{
    if (work.flags & 0x400)
        return 4;
    return work.count;
}

struct PodiumRow
{
    float v[8];
};

static const struct PodiumRow podiumXs = { { 1.0f, 0.0f, -1.0f, -2.0f, 2.0f, -3.0f, 3.0f, -4.0f } };
static const struct PodiumRow podiumZs = { { 0.0f, 0.0f, 0.0f, 0.0f, 0.3f, 0.3f, 0.6f, 0.6f } };

/* podium position of slot i */
#define PODIUM_POS(i, X, Y, Z, END)                             \
    if (work.flags & 0x100)                                     \
    {                                                           \
        X = -(0.8 * (i - 2)) - 0.5;                             \
        if (work.flags & 0x800)                                 \
            Y = 0.5f;                                           \
        else                                                    \
            Y = 0.0f;                                           \
        Z = 0.0f;                                               \
        END                                                     \
    }                                                           \
    else if (work.flags & 0x400)                                \
    {                                                           \
        xs = podiumXs;                                          \
        zs = podiumZs;                                          \
        X = xs.v[i];                                            \
        Y = commendPodiumHeights[work.ranks[i]];                \
        Z = zs.v[i];                                            \
        END                                                     \
    }                                                           \
    else if (i < 4)                                             \
    {                                                           \
        X = 0.5 * (work.count - 1) - i - 0.5;                   \
        Y = commendPodiumHeights[work.ranks[i]];                \
        if (work.flags & 0x20)                                  \
            Y = commendPodiumHeights[0];                        \
        Z = 0.0f;                                               \
        END                                                     \
    }                                                           \
    else                                                        \
    {                                                           \
        X = -0.5f;                                              \
        Y = commendPodiumHeights[0];                            \
        Z = 1.0f;                                               \
        END                                                     \
    }

void fn_1_E5E78(int idx, Vec *pos)
{
    struct PodiumRow xs;
    struct PodiumRow zs;

    PODIUM_POS(idx, pos->x, pos->y, pos->z, )
}

void fn_1_E60D0(int id, Vec *pos)
{
    struct PodiumRow zs;
    struct PodiumRow xs;
    int i;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;
    for (i = 0; i < work.count; i++)
    {
        if (id == work.order[i])
        {
            PODIUM_POS(i, pos->x, pos->y, pos->z, return;)
        }
    }
}

void fn_1_E6384(Vec *pos)
{
    struct PodiumRow zs;
    struct PodiumRow xs;
    float x;
    float y;
    float z;
    float maxX;
    float minX;
    float maxY;
    float minY;
    float maxZ;
    float minZ;
    int i;
    int n;

    n = fn_1_E5E44();
    for (i = 0; i < n; i++)
    {
        PODIUM_POS(i, x, y, z, )
        if (i == 0)
        {
            maxX = x;
            minX = maxX;
            maxY = y;
            minY = maxY;
            maxZ = z;
            minZ = maxZ;
            if (minY >= 0.0)
                minY = 0.0f;
        }
        else
        {
            minX = (minX < x) ? minX : x;
            minY = (minY < y) ? minY : y;
            minZ = (minZ < z) ? minZ : z;
            maxX = (maxX > x) ? maxX : x;
            maxY = (maxY > y) ? maxY : y;
            maxZ = (maxZ > z) ? maxZ : z;
        }
    }
    if (work.flags & 8)
    {
        minY = (minY < work.apes[4]->pos.y) ? minY : work.apes[4]->pos.y;
        maxY = (maxY > work.apes[4]->pos.y) ? maxY : work.apes[4]->pos.y;
    }
    pos->x = -0.5f;
    pos->y = 0.5 * (minY + maxY);
    pos->z = 0.5 * (minZ + maxZ);
    if (work.flags & 0x400)
    {
        pos->y += 0.3;
        pos->z -= 1.6;
    }
}

void fn_1_E6760(Mtx mtx)
{
    mtx_copy(mtx, work.mtx);
}

void fn_1_E678C(Mtx mtx)
{
    mtx_copy(work.mtx, mtx);
}

void fn_1_E67BC(int id)
{
    work.textId = id;
}

int fn_1_E67D0(Vec *pos, struct RaycastHit *hit, Vec *normal)
{
    struct PodiumRow zs;
    struct PodiumRow xs;
    float x;
    float y;
    float z;
    int i;
    int n;

    hit->flags = 0;
    normal->x = 0.0f;
    normal->y = 0.0f;
    normal->z = 0.0f;
    hit->pos.x = pos->x;
    hit->pos.z = pos->z;
    hit->normal.x = 0.0f;
    hit->normal.y = 1.0f;
    hit->normal.z = 0.0f;
    if (-2.5 <= pos->x && pos->x <= 1.5 && -0.5 <= pos->z && pos->z <= 0.5)
    {
        hit->flags = 1;
        hit->pos.y = -0.05f;
    }
    n = work.count;
    for (i = 0; i < n; i++)
    {
        PODIUM_POS(i, x, y, z, )
        if (pos->x < x - 0.5)
            continue;
        if (pos->z < z - 0.5)
            continue;
        if (pos->x > x + 0.5)
            continue;
        if (pos->z > z + 0.5)
            continue;
        hit->flags = 1;
        hit->pos.y = y;
    }
    if (work.flags & 8)
    {
        struct Ape *ape = work.apes[4];

        if (pos->x >= ape->pos.x - 0.5 && pos->z >= ape->pos.z - 0.5
         && pos->x <= 0.5 + ape->pos.x && pos->z <= 0.5 + ape->pos.z)
        {
            hit->flags = 1;
            hit->pos.y = ape->pos.y;
        }
    }
    return (hit->flags & 1) != 0;
}

void fn_1_E6BD0(int state)
{
    switch (state)
    {
    case 0:
        work.flags |= 0x40;
        work.flags &= ~0x80;
        break;
    case 1:
        work.flags &= ~0x40;
        work.flags &= ~0x80;
        break;
    case 2:
        work.flags &= ~0x40;
        work.flags |= 0x80;
        break;
    }
}

void event_commend_init(void)
{
    memset(&work, 0, 0x88);
}

void event_commend_tick(void)
{
    struct Effect e;
    struct PodiumRow zs;
    struct PodiumRow xs;
    Vec v;
    Vec pos;
    float maxX;
    float maxZ;
    float minX;
    float minZ;
    float z;
    float x;
    int n;
    u16 mask;
    struct CommendSound *sound;
    int i;
    struct Ape *ape;

    if (g_some_other_flags & 0xA)
        return;
    for (i = 0; i < work.apeCount; i++)
    {
        ape = work.apes[i];
        run_anim_funcs(ape);
        g_ape_anim_body(ape);
    }
    if (cameras[0].timerCurr >= fn_1_E5E44() * 0x110)
    {
        sound = commendLabels[work.textId].sounds;
        for (i = 0; i < commendLabels[work.textId].soundCount; i++, sound++)
        {
            if (sound->frame == work.timer)
                call_SoundReqID_arg_0(sound->soundId);
        }
        if (work.timer < 600)
            work.timer++;
    }
    if (events[14].state != 2)
        return;
    if (work.flags & 0x40)
        return;
    if (work.flags & 0x100)
    {
        fn_1_E6384(&pos);
        mtxa_from_translate_xyz(pos.x, 3.8f, pos.z);
        z = 3.6f;
    }
    else if (work.flags & 8)
    {
        mtxa_from_translate_xyz(work.apes[4]->pos.x, 3.8f, work.apes[4]->pos.z);
        z = 0.4f;
    }
    else
    {
        int k = 0;
        int j;

        for (j = 0; j < work.count; j++)
        {
            if (work.ranks[j] == 0)
            {
                float y;

                PODIUM_POS(j, x, y, z, )
                if (k <= 0)
                {
                    maxX = x;
                    maxZ = z;
                    minX = x;
                    minZ = z;
                }
                else
                {
                    if (x > maxX)
                        maxX = x;
                    if (z > maxZ)
                        maxZ = z;
                    if (x < minX)
                        minX = x;
                    if (z < minZ)
                        minZ = z;
                }
                k++;
            }
        }
        if (k <= 0)
            return;
        x = 0.5 * (maxX + minX);
        mtxa_from_translate_xyz(x, 3.8f, z);
        z = 0.4 + (maxX - minX);
    }
    mask = 1 << current_ball->playerId;
    memset(&e, 0, sizeof(e));
    e.type = 0x12;
    e.vel.x = 0.0f;
    e.vel.y = 0.0f;
    e.vel.z = 0.0f;
    n = 2;
    if (work.flags & 0x80)
        n = 1;
    v.x = 0.0f;
    v.y = 0.0f;
    for (; n > 0; n--)
    {
        v.z = 0.5 * (z * (1.0 + RAND_FLOAT()));
        mtxa_rotate_y((s16)(rand() & 0x7FFF));
        mtxa_rotate_x((s16)(rand() & 0x7FFF));
        mtxa_tf_point(&v, &e.pos);
        e.rotX = rand() & 0x7FFF;
        e.rotY = rand() & 0x7FFF;
        e.rotZ = rand() & 0x7FFF;
        if (n & 1)
            e.cameras = mask;
        else
            e.cameras = 0;
        spawn_effect(&e);
    }
}

void event_commend_dest(void)
{
    fn_1_E4F70();
}

void fn_1_E74AC(int a, int b)
{
    int i;
    struct Ape *ape;

    work.flags |= 0x100;
    work.flags = b ? (work.flags | 0x800) : (work.flags | 0x40);
    work.count = 5;
    for (i = 0; i < work.count; i++)
    {
        ape = work.apes[i];
        ape->colorIndex = a;
        if (b)
        {
            switch (i)
            {
            case 2:
                ape->unk29C &= ~0xFF;
                break;
            default:
                ape->unk29C = (ape->unk29C & ~0xFF) | 1;
                break;
            }
        }
        else
            ape->unk29C = (ape->unk29C & ~0xFF) | 3;
        g_some_set_ape_anim(ape, 7, ape->unk29C & 0xFF);
    }
}

void fn_1_E75E0(void)
{
    work.flags |= 0x1000;
}

void fn_1_E7600(void)
{
    work.flags |= 0x200;
}

void fn_1_E7620(GXColor *color)
{
    work.color = *color;
}

void load_commend_stage(void)
{
    queue_disc_read("commend.tpl");
    queue_disc_read("commend.gma");
}

struct CommendWork *fn_1_E7668(void)
{
    return &work;
}

int fn_1_E7674(void)
{
    return 1;
}

void fn_1_E767C(void)
{
}
