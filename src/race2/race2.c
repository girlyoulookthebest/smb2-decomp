#include "background.h"
#include "race2.h"
#include "item.h"
#include "libc.h"
#include "event.h"
#include "input.h"
#include "sprite.h"
#include "sound.h"
#include "mathutil.h"
#include "camera.h"
#include "mode.h"
#include "ball.h"
#include "stage.h"

struct RaceInputRec
{
    s8 stickX;
    s8 stickY;
    u16 button;
    s16 cam;
};  /* size = 0x6 */

struct Race2Item
{
    s32 unk0;
    s32 unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    float unk10;
};  /* size = 0x14 */

struct Race2Rect
{
    float left;
    float top;
    float width;
    float height;
};

struct Race2ViewLayout
{
    s32 count;
    struct Race2Rect *rects;
};

struct Race2CupEntry
{
    s16 unk0;
    s16 unk2;
};

/* selection screen settings */
struct SelSettings8
{
    /*0x00*/ s8 unk0;
    /*0x01*/ s8 unk1[6];
    /*0x08*/ u32 unk8;
}; /* size = 0xC */

struct Race2CpuEntry
{
    float unk0;
    float unk4;
    s16 unk8;
    float unkC;
};  /* size = 0x10 */

struct Race2RecordEntry
{
    u8 unk0[7];
    u8 unk7;
    u8 time[3];
    u8 unkB;
};  /* size = 0xC */

struct Race2Record
{
    struct Race2RecordEntry a;
    struct Race2RecordEntry b;
};  /* size = 0x18 */

extern struct SelSettings8 lbl_1_bss_16D0;
extern s8 lbl_1_data_6B70[6];
extern u8 lbl_1_bss_A87E0[];
extern u32 lbl_1_bss_D3360;
extern struct Race2CpuEntry lbl_1_bss_D3388[];
extern u32 lbl_1_bss_D4188;
extern s8 lbl_1_bss_243C;
extern void (*lbl_1_bss_70E94)(void);
extern s32 active_monkey_id[];
extern s16 lbl_1_data_65768;
extern s16 lbl_1_data_6576A;
struct PauseMenuState
{
    u8 filler0[4];
    u32 flags;
};
extern struct PauseMenuState g_related_to_pause_menu_input;

static inline void OSs8tof32(register s8 *in, register volatile float *out)
{
#ifdef __MWERKS__
    asm {
        psq_l fp1, 0(in), 1, 4
        stfs fp1, 0(out)
    }
#else
    *out = (float)*in;  /* GQR4: s8, scale 0 */
#endif
}

static inline void OSf32tos8(register volatile float *in, register s8 *out)
{
#ifdef __MWERKS__
    asm {
        lfs fp1, 0(in)
        psq_st fp1, 0(out), 1, 4
    }
#else
    { float v = *in; *out = (s8)(v > 127.0f ? 127 : v < -128.0f ? -128 : (int)v); }
#endif
}


#ifdef __MWERKS__
void fn_1_CCAA0(void *, int, float, float, float);
#else
/* real prototype (ord_tbl.c); PPC passes floats in FPRs so the retail
 * call order differs from the C one */
void fn_1_CCAA0(float, float, float, void *, int);
#define fn_1_CCAA0(buf, n, ofs, mn, mx) (fn_1_CCAA0)(ofs, mn, mx, buf, n)
#endif
void *OSGetStackPointer(void);
void fn_10_0(void);
void fn_10_408(void);
void fn_10_7D08(void);
void fn_10_7C5C(void);
void fn_10_7DD0(void);
void fn_10_7FCC(void);
void fn_10_821C(void);
void fn_10_83E0(void);
void fn_10_A78C(void);
void fn_10_AA68(void);
void fn_10_D288(void);
void fn_10_D4BC(void);
void fn_10_DD4C(void);
void fn_10_DDA8(struct Effect *, struct Race2EffectLink *);
void fn_10_10764(void);
void fn_10_128F8(void);
void fn_10_12D34(void);
void fn_10_12EB8(void);
void fn_10_19420(float);
void fn_10_19AB0(void);
void fn_10_19DD0(void);
void fn_10_1CEA4(void);
void fn_10_1F040(void);
void fn_10_20334(void);
void fn_10_2078C(u8);
void fn_10_20CA0(void);
void fn_10_21114(void);
void fn_10_398(struct Ball *ball, s16 arg1);
void fn_1_113190(struct Ape *, int, int, s16, float);
void fn_1_1132D0(struct Ape *, int, int, int);
void fn_1_11355C(struct Ape *, float);
void fn_1_11C5E4(void);
void fn_1_11DE54();
int fn_1_11DF64();
int fn_1_11E1C0();
int fn_1_11E3A4(void);
void fn_1_358E8(void);
void fn_1_36140(void);
float fn_1_45A8C();
void fn_1_46958();
void fn_1_590AC(int);
void fn_1_E4708();
int fn_1_E5DC8(void);
void fn_1_E6760();
void fn_1_E6BD0(int);
void free_minigame_gma_tpl(void);
void g_SoftStreamStart_with_some_defaults();
void g_ape_anim_body();
void g_ape_anim_head();
void g_ape_upright();
void g_change_music_volume();
void g_fade_track_volume();
void g_load_mal_files_from_disc();
void g_load_minigame_gma_tpl();
void g_load_stage_for_menu_bg();
void g_maybe_smth_with_music();
void g_move_ape_into_ball();
void g_return_to_sel_ngc(int);
void g_set_minigame_specific_funcs();
void g_something_calls_sndPopGroup(void);
void g_something_freeing_chara_heap_4();
void g_very_similar_to_puts();
void item_replace_type_funcs();
void load_bmp_by_id();
void load_commend_stage(void);
void load_stage();
void mtxa_to_quat();
void play_track_and_fade_out_other_tracks();
void preload_ape();
#ifdef __MWERKS__
void qsort(void *, int, int, void *);
#endif
void queue_stage_load();


struct Race2Course lbl_10_bss_1208;
s16 lbl_10_bss_121C;
s16 lbl_10_bss_121E;
s16 lbl_10_bss_1220;
s32 lbl_10_bss_1224;
struct RaceInputRec *lbl_10_bss_1228;
s16 lbl_10_bss_122C;
struct RacePlayer lbl_10_bss_1230[8];
s32 lbl_10_bss_2D30;
s32 lbl_10_bss_2D34;
u32 lbl_10_bss_2D38;
s16 lbl_10_bss_2D3C;
s32 lbl_10_bss_2D40;
s16 player_count;
u8 lbl_10_bss_2D48[8];
s16 lbl_10_bss_2D50[8];
s32 lbl_10_bss_2D60;
s16 lbl_10_bss_2D64;
struct RacePlayer *lbl_10_bss_2D68[8];
struct Race2Item lbl_10_bss_2D88[256];
s16 lbl_10_bss_4188;
s32 lbl_10_bss_418C;
s32 race_frames_remaining;
struct Race2Record lbl_10_bss_4194;
struct Race2Record lbl_10_bss_41AC;
s32 lbl_10_bss_41C4;
u32 lbl_10_bss_41C8[8];
u16 lbl_10_bss_41E8[8];
struct RaceCheckpoint lbl_10_bss_41F8[8];
struct RacePlayer *lbl_10_bss_4238[8];
s16 lbl_10_bss_4258;
s16 lbl_10_bss_425A;

u32 lbl_10_data_1120[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000, 0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000, 0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000, 0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

void fn_10_14264(void);
void g_load_race(void);
void fn_10_14B04(void);
void fn_10_14C3C(void);
void fn_10_14D00(void);
void fn_10_14D7C(void);
void fn_10_14E54(void);
void fn_10_154A4(void);
void fn_10_15680(void);
void fn_10_159DC(void);
void fn_10_15B3C(void);
void fn_10_15F04(void);
void fn_10_1603C(void);
void fn_10_1610C(void);
void fn_10_1623C(void);
void fn_10_1627C(void);

void (*lbl_10_data_1290[])(void) = {
    fn_10_14264, g_load_race, fn_10_14B04, fn_10_14C3C,
    fn_10_14D00, fn_10_14D7C, fn_10_14E54, fn_10_154A4,
    fn_10_15680, fn_10_159DC, fn_10_15B3C, fn_10_15F04,
    fn_10_1603C, fn_10_1610C, fn_10_1623C, fn_10_1627C,
};

char *lbl_10_data_1403[] = {
    "RMD2_RACE_INIT_FIRST", "RMD2_RACE_INIT", "RMD2_RACE_INTRO_INIT", "RMD2_RACE_INTRO",
    "RMD2_RACE_READY_INIT", "RMD2_RACE_READY", "RMD2_RACE_MAIN", "RMD2_RESULT_INIT",
    "RMD2_RESULT_MAIN", "RMD2_REPLAY_SKIP", "RMD2_COMMEND_INIT", "RMD2_COMMEND_MAIN",
    "RMD2_RANKING_INIT", "RMD2_RANKING_MAIN", "RMD2_DEST_INIT", "RMD2_DEST_MAIN",
};

struct Race2Rect lbl_10_data_1444[] = {{0.0f, 0.0f, 1.0f, 1.0f}};
struct Race2Rect lbl_10_data_1454[] = {
    {0.0f, 0.0f, 0.5f, 0.5f}, {0.5f, 0.0f, 0.5f, 0.5f}, {0.0f, 0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f, 0.5f},
};
struct Race2ViewLayout lbl_10_data_1494[] = {
    {1, lbl_10_data_1444}, {-1, NULL}, {-1, NULL}, {4, lbl_10_data_1454},
};
struct Race2Rect lbl_10_data_14B4[] = {{0.0f, 0.0f, 1.0f, 0.5f}, {0.0f, 0.5f, 1.0f, 0.5f}};
struct Race2Rect lbl_10_data_14D4[] = {{0.0f, 0.0f, 0.5f, 1.0f}, {0.5f, 0.0f, 0.5f, 1.0f}};
struct Race2ViewLayout lbl_10_data_14F4[] = {{2, lbl_10_data_14B4}, {2, lbl_10_data_14D4}, {2, lbl_10_data_14D4}};
struct Race2Rect lbl_10_data_150C[] = {{0.0f, 0.0f, 1.0f, 0.5f}, {0.0f, 0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f, 0.5f}};
struct Race2Rect lbl_10_data_153C[] = {{0.0f, 0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 0.5f}, {0.5f, 0.5f, 0.5f, 0.5f}};
struct Race2Rect lbl_10_data_156C[] = {{0.0f, 0.0f, 0.5f, 0.5f}, {0.5f, 0.0f, 0.5f, 0.5f}, {0.0f, 0.5f, 1.0f, 0.5f}};
struct Race2Rect lbl_10_data_159C[] = {
    {0.0f, 0.0f, 0.5f, 0.5f}, {0.5f, 0.0f, 0.5f, 0.5f}, {0.0f, 0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f, 0.5f},
};
struct Race2ViewLayout lbl_10_data_15DC[] = {
    {3, lbl_10_data_150C}, {3, lbl_10_data_153C}, {3, lbl_10_data_156C}, {4, lbl_10_data_159C},
};
struct Race2CupEntry lbl_10_data_15FC[] = {{0, 5}, {1, 5}, {2, 4}, {3, 3}, {4, 2}, {5, 2}, {-1, 0}};
void fn_10_18CAC(struct Ape *ape, int);

void _prolog(void);
void _epilog(void);
void _unresolved(void);
void fn_10_13B94(void);
void fn_10_13CF0(void);
void race2_tick(void);
void fn_10_1410C(void);
void fn_10_1416C(void);
void fn_10_141EC(s16 arg0);
void fn_10_14264(void);
void g_load_race(void);
void fn_10_14B04(void);
void fn_10_14C3C(void);
void fn_10_14D00(void);
void fn_10_14D7C(void);
void fn_10_14E54(void);
void fn_10_154A4(void);
void fn_10_15680(void);
void fn_10_159DC(void);
void fn_10_15B3C(void);
void fn_10_15F04(void);
void fn_10_1603C(void);
void fn_10_1610C(void);
void fn_10_1623C(void);
void fn_10_1627C(void);
void fn_10_162BC(struct Ball *ball);
float fn_10_163A0(void);
void fn_10_164C8(struct Ball *ball);
void fn_10_17190(struct Ball *ball, struct StageExtraData *arg1, float *arg2);
void fn_10_17320(struct Ball *ball, struct StageExtraData *arg1, float *arg2);
void fn_10_176A0(struct Ball *ball);
void fn_10_17960(struct Ball *ball);
void fn_10_179E0(void);
s32 fn_10_17AE8(struct RacePlayer **arg0, struct RacePlayer **arg1);
void fn_10_17CD8(s32 arg0);
s32 fn_10_17E50(struct RacePlayer **arg0, struct RacePlayer **arg1);
void fn_10_17F04(void);
s32 fn_10_17F44(struct RacePlayer **arg0, struct RacePlayer **arg1);
s32 fn_10_17FA0(void);
void fn_10_180D8(struct Ball *ball);
s16 fn_10_181C8(u16 arg0);
void fn_10_181EC(struct StageExtraData *arg0, float farg0, Vec *arg1);
void fn_10_182C4(struct RaceCheckpoint *ball, u8 *arg1);
void fn_10_18430(void);
void fn_10_18474(void);
s32 fn_10_188A0(s32 arg0);
u16 fn_10_188C0(void);
float fn_10_18900(void);
void fn_10_1897C(void);
void fn_10_18A00(void);
void fn_10_18AA8(void);
void fn_10_18ACC(void);
void fn_10_18B30(void);
void fn_10_18B9C(void);
void fn_10_18C04(void);

void _prolog(void)
{
    if ((s32) modeCtrl.unk30 >= 2) {
        fn_1_CCAA0(lbl_1_bss_A87E0, 0x1000, 0.0f, 0.0f, 400.0f);
    } else {
        fn_1_CCAA0(lbl_1_bss_A87E0, 0x4000, 0.0f, 0.0f, 400.0f);
    }
    g_set_minigame_specific_funcs(race2_tick, fn_10_1416C, &fn_10_0, &fn_10_7D08);
    fn_10_13B94();
    sub_mode_request = 0xBA;
    if ((u32) (g_some_other_flags & 4) != 0) {
        g_very_similar_to_puts("[SUPER MONKEY RACE DX] LOADED\n");
    }
}

void _epilog(void)
{
    fn_10_13CF0();
    fn_1_CCAA0(lbl_1_bss_A87E0, 0x4000, 0.0f, 0.0f, 400.0f);
    if ((u32) (g_some_other_flags & 4) != 0) {
        g_very_similar_to_puts("[SUPER MONKEY RACE DX] PURGED\n");
    }
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    for (sp = OSGetStackPointer(); sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 16; sp = (u32 *)sp[0])
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
    OSPanic("race2_mode.c", 0x13A, "\n");
}

/* stripped at link; pins the .bss order, which mwcc assigns by first use */
void race2_bss_layout(void)
{
    lbl_10_bss_1208.unk0 = 0;
    lbl_10_bss_121C = 0;
    lbl_10_bss_121E = 0;
    lbl_10_bss_1220 = 0;
    lbl_10_bss_1224 = 0;
    lbl_10_bss_1228 = 0;
    lbl_10_bss_122C = 0;
    lbl_10_bss_1230[0].flags = 0;
    lbl_10_bss_2D30 = 0;
    lbl_10_bss_2D34 = 0;
    lbl_10_bss_2D38 = 0;
    lbl_10_bss_2D3C = 0;
    lbl_10_bss_2D40 = 0;
    player_count = 0;
    lbl_10_bss_2D48[0] = 0;
    lbl_10_bss_2D50[0] = 0;
    lbl_10_bss_2D60 = 0;
    lbl_10_bss_2D64 = 0;
    lbl_10_bss_2D68[0] = 0;
    lbl_10_bss_2D88[0].unk0 = 0;
    lbl_10_bss_4188 = 0;
    lbl_10_bss_418C = 0;
    race_frames_remaining = 0;
    lbl_10_bss_4194.a.unk7 = 0;
    lbl_10_bss_41AC.a.unk7 = 0;
    lbl_10_bss_41C4 = 0;
    lbl_10_bss_41C8[0] = 0;
    lbl_10_bss_41E8[0] = 0;
    lbl_10_bss_41F8[0].unk0 = 0;
    lbl_10_bss_4238[0] = 0;
    lbl_10_bss_4258 = 0;
    lbl_10_bss_425A = 0;
}

void fn_10_13B94(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    s16 r5;

    for (r5 = 0; r5 < 0x80; r5++) {
        lbl_1_bss_D3388[r5].unk0 = -1.0f;
        lbl_1_bss_D3388[r5].unk4 = 0.0f;
        lbl_1_bss_D3388[r5].unkC = 0.0f;
    }
    dest_all_events();
    SoundGroupLoad(0xD);
    fn_1_358E8();
    g_SoftStreamStart_with_some_defaults(0x22);
    g_SoftStreamStart_with_some_defaults(0x23);
    g_load_minigame_gma_tpl(1);
    load_bmp_by_id(7);
    fn_10_DD4C();
    lbl_10_bss_121C = 0;
    lbl_10_bss_121E = 0;
    lbl_10_bss_1220 = -1;
    lbl_10_bss_1224 = 0;
    course->unk0 = 0;
    course->unk2 = 0;
    course->unk4 = 3;
    course->unk8 = 0.0f;
    course->unk6 = 0;
    lbl_10_bss_1228 = 0;
    fn_10_7C5C();
    lbl_1_bss_D4188 = -1;
}

void fn_10_13CF0(void)
{
    dest_all_events();
    free_minigame_gma_tpl();
    camera_setup_singleplayer_viewport();
    call_free_bmp_by_id(7);
    g_something_calls_sndPopGroup();
    fn_10_18B9C();
    item_replace_type_funcs(2, &(struct ItemFuncs){0});
    if (lbl_10_bss_1228 != 0) {
        OSFreeToHeap(currentHeap, lbl_10_bss_1228);
    }
}

void race2_tick(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;

    if (((u32) (dip_switches & 1) != 0) && ((s32) (pad_status_groups->unk0[0].button & 0x40) != 0) && ((s32) (pad_status_groups->unk0[2].button & 0x200) != 0)) {
        s16 r28;

        printf("static RACE2_CPU ???[] = {\n");
        for (r28 = 0; r28 < 0x80; r28++) {
            if (lbl_1_bss_D3388[r28].unk0 < 0.0f) {
                printf("\tRACE2_CPU_END\n};\n");
                break;
            }
            printf("\t{ %5.1f,\t%3.1f,\t\t0x%04x,\t%5.2f\t},\n", lbl_1_bss_D3388[r28].unk0, lbl_1_bss_D3388[r28].unk4, (u16) lbl_1_bss_D3388[r28].unk8, lbl_1_bss_D3388[r28].unkC);
        }
    }
    if ((u32) (modeCtrl.courseFlags & 0x4000) != 0) {
        course->unk2 = course->unk2 & ~0x40;
        modeCtrl.courseFlags &= 0xFFFFBFFF;
        dest_all_events();
        fn_10_141EC(1);
    }
    if ((u32) (g_some_other_flags & 0xA) == 0) {
        fn_10_1410C();
        if (lbl_10_bss_122C == 0) {
            g_fade_track_volume(0x64, 8);
        }
        if (lbl_10_bss_1220 >= 0) {
            lbl_10_bss_121E = lbl_10_bss_121C;
            lbl_10_bss_121C = lbl_10_bss_1220;
            lbl_10_bss_1220 = -1;
        }
        if ((course->unk2 & 0x40) != 0) {
            struct RacePlayer *player = lbl_10_bss_1230;

            if (lbl_10_bss_121C == 4) {
                fn_10_18AA8();
            } else if ((lbl_10_bss_121C >= 5) && (lbl_10_bss_121C <= 6)) {
                fn_10_18ACC();
            }
            if ((lbl_10_bss_121C >= 5) && (lbl_10_bss_121C <= 8)) {
                if (((lbl_10_bss_2D30 < 0) && ((lbl_10_bss_121C != 8) || (lbl_10_bss_1224 >= 0x1E)) && ((s32) (pad_status_groups[player->unk14].unk0[2].button & 0x100) != 0)) || (lbl_10_bss_2D34 == 0x2A10)) {
                    fade_screen_to_color(0x101, 0, 0x1E);
                    lbl_10_bss_2D30 = 0x1E;
                }
                if (lbl_10_bss_2D30 == 0) {
                    fn_10_141EC(9);
                    if (((u32) (dip_switches & 1) != 0) && ((u32) (lbl_1_bss_D3360 & 0x40) != 0)) {
                        fn_10_141EC(1);
                    }
                }
            }
        } else if (lbl_10_bss_121C == 4) {
            fn_10_1897C();
        } else if ((lbl_10_bss_121C >= 5) && (lbl_10_bss_121C <= 6)) {
            fn_10_18A00();
        }
        lbl_10_data_1290[lbl_10_bss_121C]();
    }
}

void fn_10_1410C(void)
{
    lbl_10_bss_2D38++;
    if (lbl_10_bss_1224 > 0) {
        lbl_10_bss_1224--;
    }
    if (lbl_10_bss_2D3C > 0) {
        lbl_10_bss_2D3C--;
    }
    if (lbl_10_bss_122C >= 0) {
        lbl_10_bss_122C--;
    }
}

void fn_10_1416C(void)
{
    if (lbl_10_bss_121C >= 0 && lbl_10_bss_121C <= 10) {
        fn_10_AA68();
    } else if (lbl_10_bss_121C == 11) {
        fn_10_D288();
    } else if (lbl_10_bss_121C == 13) {
        fn_10_D4BC();
    }
}

void fn_10_141EC(s16 arg0)
{
    if ((arg0 < 0) || (arg0 >= 0x10)) {
        OSPanic("race2_mode.c", 0x239, "Illegal race mode.\n");
    }
    lbl_10_bss_1220 = arg0;
    lbl_10_bss_2D40 = 0;
}

void fn_10_14264(void)
{
    u8 sp8[4] = {0, 1, 2, 3};
    struct Race2Course *course = &lbl_10_bss_1208;
    struct SelSettings8 *sel = &lbl_1_bss_16D0;
    s16 r0;
    s32 i;
    s32 x;

    course->unk0 = sel->unk0;
    course->unk4 = sel->unk1[course->unk0];
    course->unk2 = (u16) sel->unk8;
    if ((course->unk2 & 0x10) != 0) {
        course->unk2 &= ~1;
        course->unk2 &= ~4;
        course->unk2 &= ~2;
        course->unk4 = lbl_1_data_6B70[course->unk0];
    }
    if (((s32) modeCtrl.unk30 == 3) && (modeCtrl.splitscreenMode == 3)) {
        course->unk2 |= 0x80;
    }
    if ((s32) modeCtrl.unk30 == 1) {
        course->unk2 = course->unk2 & ~0x20;
    } else {
        course->unk2 |= 0x20;
    }
    if ((course->unk2 & 1) != 0) {
        r0 = 8;
    } else {
        r0 = (s16) modeCtrl.unk30;
    }
    player_count = r0;
    for (i = 0; i < (s32) modeCtrl.unk30; i++) {
        lbl_10_bss_2D48[i] = (u8) active_monkey_id[i];
    }
    for (i = modeCtrl.unk30; i < player_count; i++) {
        x = rand() & 0x7FFF;
        lbl_10_bss_2D48[i] = sp8[x & 3];
    }
    fn_10_18B30();
    if ((course->unk2 & 8) != 0) {
        course->unk6 = 0;
        for (i = 0; i < 8; i++) {
            lbl_10_bss_2D50[i] = 0;
        }
    }
    lbl_10_bss_1220 = 1;
    lbl_10_bss_2D40 = 0;
}

void g_load_race(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    s32 b;
    struct Race2ViewLayout *r26;
    struct Race2CourseInfo *tr27;
    struct World *r4;
    s8 *r5;
    s8 r0;
    s32 r6;
    s32 a;
    struct Race2Item *tr3;
    s32 i;
    struct Race2Rect *r29;

    lbl_10_bss_2D38 = 0;
    if ((course->unk2 & 8) != 0) {
        course->unk0 = (u16) lbl_10_data_15FC[course->unk6].unk0;
        course->unk4 = (u16) lbl_10_data_15FC[course->unk6].unk2;
    }
    tr27 = &lbl_10_data_2ADC[course->unk0];
    load_stage(tr27->unk4);
    if ((course->unk2 & 0x40) == 0) {
        fn_10_21114();
        lbl_10_bss_2D60 = (OSGetTick() / 4);
    }
    fn_10_A78C();
    fn_10_188A0(lbl_10_bss_2D60);
    if ((course->unk2 & 8) != 0) {
        lbl_10_bss_2D64 = lbl_10_data_15FC[course->unk6 + 1].unk0;
        if (lbl_10_bss_2D64 >= 0) {
            queue_stage_load(lbl_10_data_2ADC[lbl_10_bss_2D64].unk4);
        } else {
            fn_10_18C04();
        }
    } else {
        load_commend_stage();
        fn_10_18C04();
        if ((course->unk2 & 0x10) != 0) {
            preload_ape(lbl_10_bss_2D48[0], 1, 0, 0);
            queue_stage_load(0xC8);
        }
    }
    course->unk8 = fn_10_163A0();
    course->unkC = (100.0 * (1.0 / course->unk8));
    for (i = 0; i < 8; i++) {
        lbl_10_bss_2D68[i] = 0;
    }
    lbl_10_bss_2D3C = (s16) (lbl_80198B38 * 0xA);
    for (i = 0; i < 0x100; i++) {
        tr3 = &lbl_10_bss_2D88[i];
        tr3->unk0 = 0;
        tr3->unk4 = 0;
        tr3->unk8 = -1;
        tr3->unkA = -1;
        tr3->unkC = 0;
        tr3->unkE = 0;
        tr3->unk10 = 0.0f;
    }
    lbl_10_bss_122C = -1;
    lbl_10_bss_4188 = 0;
    lbl_10_bss_418C = -1;
    fn_10_12D34();
    if ((s32) modeCtrl.unk30 >= 3) {
        r0 = 1;
    } else {
        r0 = 2;
    }
    lbl_1_bss_243C = r0;
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(3);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x14);
    if ((course->unk2 & 0x40) == 0) {
        event_init(0xB);
    }
    switch (current_stage_id) {               /* switch 1; irregular */
    case 0x86:                                      /* switch 1 */
        break;
    case 0x85:
    case 0x87:                                      /* switch 1 */
        fn_1_590AC(modeCtrl.unk30);
        break;
    }
    lbl_1_bss_70E94 = &fn_10_408;
    a = 1;
    b = 3;
    for (i = 0; i < player_count; i++) {
        create_stage_player((u8) i, 0x16, lbl_10_bss_2D48[i], (u8) i, b, fn_10_18CAC, a);
        if (i < (s32) modeCtrl.unk30) {
            enable_camera(i);
            cameras[i].mode = 0x3C;
            cameras[i].unk1F = 0;
        }
    }
    if ((course->unk2 & 0x80) != 0) {
        enable_camera(3);
        cameras[3].mode = 0x3C;
        cameras[3].unk1F = 0xE;
        cameras[3].flags |= 0x40;
        fn_10_20CA0();
    }
    r4 = world_infos;
    r5 = g_poolInfo.playerPool.statusList;
    r6 = 0;
    while (r6 < (s32) g_poolInfo.playerPool.count) {
        if (*r5 == 2) {
            r4->state = 1;
        }
        r6++;
        r4++;
        r5++;
    }
    fn_10_10764();
    race_frames_remaining = (tr27->unk58 * lbl_80198B38);
    lbl_10_bss_4194.a.unk7 = (u8) balls->ape->charaId;
    lbl_10_bss_4194.a.unkB = 0;
    lbl_10_bss_4194.a.time[0] = 0x63;
    lbl_10_bss_4194.a.time[1] = 0x3B;
    lbl_10_bss_4194.a.time[2] = 0x63;
    lbl_10_bss_4194.b = lbl_10_bss_4194.a;
    switch ((s32) modeCtrl.unk30) {                 /* switch 2; irregular */
    case 2:                                         /* switch 2 */
        r26 = &lbl_10_data_14F4[modeCtrl.wideMode];
        break;
    case 3:                                         /* switch 2 */
        r26 = &lbl_10_data_15DC[modeCtrl.splitscreenMode];
        break;
    default:                                        /* switch 2 */
        r26 = &lbl_10_data_1494[modeCtrl.unk30 - 1];
        break;
    }
    r29 = r26->rects;
    i = 0;
    while (i < r26->count) {
        set_camera_view_settings(i, r29->left, r29->top, r29->width, r29->height);
        i++;
        r29++;
    }
    if (((s32) modeCtrl.unk30 == 1) && ((course->unk2 & 8) == 0) && ((course->unk2 & 0x40) == 0) && (lbl_10_bss_1228 == 0)) {
        lbl_10_bss_1228 = alloc_from_heap_or_panic(currentHeap, 0xFD20, "race2_mode.c", 0x396);
    }
    fn_10_19DD0();
    g_maybe_smth_with_music(-1, 0x64);
    play_track_and_fade_out_other_tracks(tr27->unk6, 0x1E, 0x64);
    fade_screen_to_color(0x100, 0, 0x1E);
    lbl_10_bss_1220 = 2;
    lbl_10_bss_2D40 = 0;
}

void fn_10_14B04(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct Ball *r30;
    struct Ape *tr3;
    s16 r29;

    if ((course->unk2 & 0x40) != 0) {
        fn_10_7DD0();
    } else if ((course->unk2 & 0x80) != 0) {
        fn_10_821C();
    }
    r29 = 0;
    r30 = balls;
    while (r29 < 8) {
        if (r30->status == 2) {
            r30->flags &= ~0x10ULL;
            tr3 = r30->ape;
            tr3->flags &= 0xFFFFFFDF;
            fn_10_398(r30, 6);
            lbl_10_bss_1230[r30->playerId].unk3C = 0x1770;
        }
        r29++;
        r30++;
    }
    lbl_10_bss_1224 = (1.1 * lbl_80198B34);
    lbl_10_bss_1220 = 3;
    lbl_10_bss_2D40 = 0;
}

void fn_10_14C3C(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    if (lbl_10_bss_1224 == ((1.1 * lbl_80198B34) - 60.0)) {
        fn_10_19420(2.1f);
        if ((course->unk2 & 8) != 0) {
            call_SoundReqID_arg_0(course->unk6 + 0x1F8);
        }
    }
    if (lbl_10_bss_1224 == 0) {
        lbl_10_bss_1220 = 4;
        lbl_10_bss_2D40 = 0;
    }
}

void fn_10_14D00(void)
{
    struct Ball *r6;
    s16 r7;

    lbl_10_bss_1224 = 0x12C;
    r7 = 0;
    r6 = balls;
    while (r7 < 8) {
        if (r6->status == 2) {
            lbl_10_bss_1230[r6->playerId].unk3C = lbl_10_bss_1224;
        }
        r7++;
        r6++;
    }
    lbl_10_bss_1220 = 5;
    lbl_10_bss_2D40 = 0;
}

void fn_10_14D7C(void)
{
    if (lbl_10_bss_1224 == 0xB3) {
        fn_10_19AB0();
    }
    if (lbl_10_bss_1224 == 0xA) {
        create_go_sprite(0x3C);
        call_SoundReqID_arg_0(0x1E7);
    }
    if (lbl_10_bss_1224 == (lbl_1_data_65768 + lbl_1_data_6576A)) {
        fn_10_18430();
    }
    if (lbl_10_bss_1224 <= (lbl_1_data_65768 + lbl_1_data_6576A)) {
        fn_10_18474();
    }
    if (lbl_10_bss_1224 == 0) {
        call_SoundReqID_arg_0(0x3B0DF);
        lbl_10_bss_1220 = 6;
        lbl_10_bss_2D40 = 0;
    }
}

void fn_10_14E54(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    Vec sp8;
    float tf1_2;
    struct Ball *r29;
    struct RacePlayer *tr28;
    struct Ball *r28;
    struct RacePlayer *tr27;
    struct RacePlayer *tr26;
    s16 r25;
    s16 r24;
    struct RaceCheckpoint *tr4_2;
    struct RaceCheckpoint *tr5;

    fn_10_18474();
    if (((course->unk2 & 0x20) == 0) && (lbl_10_bss_2D40 == 0) && (race_frames_remaining > 0) && ((u32) (dip_switches & 0x20) == 0)) {
        race_frames_remaining--;
    }
    r25 = 0;
    r29 = balls;
    while (r25 < 8) {
        if (r29->status == 2) {
            tr28 = (struct RacePlayer *)r29->unk150;
            fn_10_162BC(r29);
            if ((u32) (tr28->flags & 2) == 0) {
                if ((u32) (tr28->flags & 0x20) != 0) {
                    tr28->unk48[tr28->unk44].unk0++;
                    tr28->unk1E0++;
                } else {
                    fn_10_164C8(r29);
                    fn_10_176A0(r29);
                    fn_10_17960(r29);
                    if ((u32) (tr28->flags & 0x800) == 0) {
                        if ((tr28->unk204 > tr28->unk200) || (tr28->unk1F4 > tr28->unk1F0)) {
                            tr28->flags |= 1;
                            tr28->unkC = 0;
                            if (tr28->unkE < 0x7FFE) {
                                tr28->unkE++;
                            }
                        } else {
                            tr28->flags &= 0xFFFFFFFE;
                            tr28->unkE = 0;
                            if (tr28->unkC < 0x7FFE) {
                                tr28->unkC++;
                            }
                        }
                    }
                    if ((tr28->unkE <= (lbl_80198B38 * 2)) && (tr28->unkC < (s32) (lbl_80198B38 * 2))) {
                        if (mathutil_vec_len(&r29->vel) < 0.046296296296296294) {
                            tr28->flags = ((tr28->flags & 0xFFFFFFFE) | (tr28->unk38 & 1));
                        }
                    }
                    if ((u32) (tr28->flags & 0x800) != 0) {
                        tr4_2 = &tr28->unk48[tr28->unk46];
                        tr5 = (struct RaceCheckpoint *)&tr28->unk1D8;
                        if ((tr4_2->unk0 < tr5->unk0) || ((tr4_2->unk0 == tr5->unk0) && (tr4_2->unk4 < tr5->unk4))) {
                            *tr5 = *tr4_2;
                            if ((u32) (tr28->flags & 0x100000) != 0) {
                                lbl_10_bss_4194.b.unkB = 1;
                            } else {
                                lbl_10_bss_4194.b.unkB = 0;
                            }
                        }
                    }
                }
            }
            if ((u32) (tr28->flags & 0x800) != 0) {
                tr28->flags &= 0xFFFFF7FF;
                tr28->flags &= 0xFFEFFFFF;
                if (((r29->flags & 0x80000000000ULL) == 0) && ((u32) (tr28->flags & 2) == 0)) {
                    if ((s32) modeCtrl.unk30 == 1) {
                        fn_10_20334();
                    } else {
                        fn_10_2078C((u8) r29->playerId);
                    }
                    if ((lbl_10_bss_2D40 == 0) && ((s32) modeCtrl.unk30 == 1)) {
                        fn_10_180D8(r29);
                    }
                }
                if (((u32) (tr28->flags & 2) != 0) && ((r29->flags & 0x80000000000ULL) == 0) && ((course->unk2 & 0x10) != 0)) {
                    fn_10_182C4((struct RaceCheckpoint *)&tr28->unk1E0, lbl_10_bss_4194.a.time);
                    fn_10_182C4((struct RaceCheckpoint *)&tr28->unk1D8, lbl_10_bss_4194.b.time);
                    lbl_10_bss_418C = fn_1_11E1C0(course->unk0, &lbl_10_bss_4194);
                }
            }
            if (((u32) (tr28->unk38 & 1) != 0) && ((u32) (tr28->flags & 1) != 0)) {
                if (tr28->unk10 <= 0x7FFE) {
                    tr28->unk10++;
                }
            } else {
                tr28->unk10 = 0;
            }
            g_ape_anim_head(r29->ape, &r29->vel);
        }
        r25++;
        r29++;
    }
    if (player_count >= 2) {
        fn_10_179E0();
    }
    if ((course->unk2 & 4) != 0) {
        fn_10_128F8();
        fn_10_12EB8();
    }
    r25 = 0;
    r29 = balls;
    while (r25 < 7) {
        if (r29->status == 2) {
            tr27 = (struct RacePlayer *)r29->unk150;
            r24 = r25 + 1;
            r28 = &balls[r25 + 1];
            while (r24 < 8) {
                if (r28->status == 2) {
                    tr26 = (struct RacePlayer *)r28->unk150;
                    PSVECSubtract(&r28->pos, &r29->pos, &sp8);
                    tf1_2 = vec_normalize_len(&sp8);
                    tr26->unk280[r25] = tf1_2;
                    tr27->unk280[r24] = tf1_2;
                    tr27->unk220[r24] = sp8;
                    sp8.x = -sp8.x;
                    sp8.y = -sp8.y;
                    sp8.z = -sp8.z;
                    tr26->unk220[r25] = sp8;
                }
                r24++;
                r28++;
            }
        }
        r25++;
        r29++;
    }
    if ((course->unk2 & 0x40) != 0) {
        fn_10_7FCC();
    } else if ((course->unk2 & 0x80) != 0) {
        fn_10_83E0();
    }
    if ((lbl_10_bss_2D40 == 0) && ((fn_10_17FA0() != 0) || (race_frames_remaining == 0))) {
        lbl_10_bss_2D40 = 1;
        if (race_frames_remaining == 0) {
            lbl_10_bss_1224 = 0x1E;
        } else {
            lbl_10_bss_1224 = 0x12C;
        }
    }
    if ((lbl_10_bss_2D40 != 0) && (lbl_10_bss_1224 <= 0)) {
        lbl_10_bss_1220 = 7;
        lbl_10_bss_2D40 = 0;
    }
}

void fn_10_154A4(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct Ball *r29;
    struct RacePlayer *tr28;
    s16 r27;

    r27 = 0;
    r29 = balls;
    while (r27 < 8) {
        if (r29->status == 2) {
            tr28 = (struct RacePlayer *)r29->unk150;
            if ((u32) (tr28->flags & 2) == 0) {
                fn_10_398(r29, 0xF);
            }
            if (((course->unk2 & 8) != 0) && ((r29->flags & 0x80000000000ULL) != 0) && ((u32) (tr28->flags & 2) == 0)) {
                lbl_10_bss_2D50[r29->playerId] += fn_10_181C8(tr28->unk3E);
            }
        }
        r27++;
        r29++;
    }
    if ((course->unk2 & 8) != 0) {
        fn_10_17CD8(0);
        if (lbl_10_bss_2D64 >= 0) {
            lbl_10_bss_1224 = 0x1E0;
        } else {
            lbl_10_bss_1224 = 0x12C;
        }
    } else {
        lbl_10_bss_1224 = 0x12C;
    }
    if ((course->unk2 & 0x40) != 0) {
        set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
        set_camera_view_settings(1, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    lbl_10_bss_1220 = 8;
    lbl_10_bss_2D40 = 0;
}

void fn_10_15680(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct RacePlayer *r5;
    s16 r6;
    struct Race2CourseInfo *tr29;

    tr29 = &lbl_10_data_2ADC[course->unk0];
    if ((course->unk2 & 0x80) != 0) {
        fn_10_83E0();
    }
    if (((course->unk2 & 8) != 0) && (lbl_10_bss_2D64 >= 0)) {
        if (lbl_10_bss_1224 == (lbl_80198B38 * 5)) {
            fn_10_1F040();
        } else if (lbl_10_bss_1224 == lbl_80198B38) {
            g_change_music_volume(tr29->unk6, lbl_80198B38, 0);
        }
    }
    if (lbl_10_bss_1224 == 30) {
        if (((course->unk2 & 0x40) == 0) || (lbl_10_bss_2D30 < 0)) {
            fade_screen_to_color(0x101, 0, 0x1E);
        }
    } else if (lbl_10_bss_1224 == 0) {
        if ((course->unk2 & 8) != 0) {
            course->unk6++;
            if (lbl_10_data_15FC[course->unk6].unk0 >= 0) {
                dest_all_events();
                lbl_10_bss_1220 = 1;
                lbl_10_bss_2D40 = 0;
                return;
            }
            g_related_to_pause_menu_input.flags |= 2;
            lbl_10_bss_1220 = 0xA;
            lbl_10_bss_2D40 = 0;
            return;
        }
        if (((s32) modeCtrl.unk30 == 1) && ((u32) (lbl_10_bss_1230[0].flags & 2) != 0) && ((course->unk2 & 0x40) == 0)) {
            lbl_10_bss_41AC.a = lbl_10_bss_4194.a;
            lbl_10_bss_41AC.b = lbl_10_bss_4194.b;
            lbl_10_bss_41C4 = lbl_10_bss_418C;
            r6 = 0;
            r5 = lbl_10_bss_1230;
            while (r6 < 8) {
                lbl_10_bss_41C8[r6] = r5->flags;
                lbl_10_bss_41E8[r6] = r5->unk3E;
                lbl_10_bss_41F8[r6] = *(struct RaceCheckpoint *)&r5->unk1E0;
                r6++;
                r5++;
            }
            course->unk2 |= 0x40;
            dest_all_events();
            lbl_10_bss_1220 = 1;
            lbl_10_bss_2D40 = 0;
            return;
        }
        if ((course->unk2 & 0x40) != 0) {
            lbl_10_bss_1220 = 9;
            lbl_10_bss_2D40 = 0;
            if (((u32) (dip_switches & 1) != 0) && ((u32) (lbl_1_bss_D3360 & 0x40) != 0)) {
                lbl_10_bss_1220 = 1;
                lbl_10_bss_2D40 = 0;
                return;
            }
        } else {
            g_related_to_pause_menu_input.flags |= 2;
            if (((course->unk2 & 0x10) != 0) && (lbl_10_bss_418C < 0)) {
                lbl_10_bss_1220 = 0xC;
                lbl_10_bss_2D40 = 0;
                return;
            }
            lbl_10_bss_1220 = 0xA;
            lbl_10_bss_2D40 = 0;
        }
    }
}

void fn_10_159DC(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct RacePlayer *r29;
    s16 r28;

    g_related_to_pause_menu_input.flags |= 2;
    lbl_10_bss_4194.a = lbl_10_bss_41AC.a;
    lbl_10_bss_4194.b = lbl_10_bss_41AC.b;
    lbl_10_bss_418C = lbl_10_bss_41C4;
    r28 = 0;
    r29 = lbl_10_bss_1230;
    while (r28 < 8) {
        r29->flags = lbl_10_bss_41C8[r28];
        r29->unk3E = lbl_10_bss_41E8[r28];
        *(struct RaceCheckpoint *)&r29->unk1E0 = lbl_10_bss_41F8[r28];
        fn_10_182C4((struct RaceCheckpoint *)&r29->unk1E0, &r29->unk1E8);
        r28++;
        r29++;
    }
    fn_10_17F04();
    lbl_10_bss_122C = -1;
    fn_1_36140();
    if (((course->unk2 & 0x10) != 0) && (lbl_10_bss_418C < 0)) {
        lbl_10_bss_1220 = 0xC;
        lbl_10_bss_2D40 = 0;
        return;
    }
    lbl_10_bss_1220 = 0xA;
    lbl_10_bss_2D40 = 0;
}

void fn_10_15B3C(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    s8 sp10[8];
    s8 sp8[8];
    struct Race2CourseInfo *tr29;
    s16 r28;
    s32 r0;
    struct Ball *r3;
    struct RacePlayer *r4;
    s16 r5;
    s32 dead;

    tr29 = &lbl_10_data_2ADC[course->unk0];
    r28 = 0;
    if ((course->unk2 & 8) != 0) {
        fn_10_17CD8(1);
    } else if ((course->unk2 & 0x10) != 0) {
        dead = 1;
    } else {
        r3 = balls;
        r4 = lbl_10_bss_1230;
        r5 = 0;
        while (r5 < player_count) {
            if ((u32) (r4->flags & 2) == 0) {
                r4->unk3E = (u16) player_count;
            }
            r5++;
            r3++;
            r4++;
        }
    }
    r5 = 0;
    r3 = balls;
    while (r5 < 8) {
        sp10[r5] = -1;
        if (r3->status == 2) {
            r4 = (struct RacePlayer *)r3->unk150;
            sp10[r5] = (s8) r3->ape->charaId;
            if ((r3->flags & 0x80000000000ULL) != 0) {
                sp10[r5] |= 0x40;
            }
            if ((course->unk2 & 0x10) != 0) {
                sp8[r5] = (s8) lbl_10_bss_418C;
            } else if ((course->unk2 & 8) != 0) {
                sp8[r5] = r4->unk3E - 1;
                if (sp8[r5] >= 3) {
                    sp8[r5] = 3;
                }
            } else if ((u32) (r4->flags & 2) != 0) {
                sp8[r5] = r4->unk3E - 1;
            } else {
                sp8[r5] = player_count - 1;
                r28++;
            }
        }
        r5++;
        r3++;
    }
    dest_all_events();
    fn_10_18B9C();
    if (lbl_10_bss_1228 != 0) {
        OSFreeToHeap(currentHeap, lbl_10_bss_1228);
        lbl_10_bss_1228 = 0;
    }
    camera_setup_singleplayer_viewport();
    load_stage(tr29->unk4);
    event_init(0x10);
    event_init(0);
    event_init(0x14);
    event_init(0xE);
    event_init(4);
    event_init(0x11);
    event_init(0x13);
    event_init(0xC);
    fn_1_E4708(sp10, sp8);
    fn_1_E6BD0(2);
    enable_cameras_for_active_players();
    camera_set_state(0x46);
    mtxa_from_translate(&tr29->unk64);
    mtxa_rotate_y(tr29->unk60);
    fn_1_E6760(mtxa);
    if (((course->unk2 & 0x10) == 0) && (r28 < player_count)) {
        fn_10_1CEA4();
    }
    if ((course->unk2 & 0x10) == 0) {
        g_load_stage_for_menu_bg(0, 1);
    }
    fade_screen_to_color(0x100, 0, (s32) (0.5 * lbl_80198B34));
    lbl_10_bss_1220 = 0xB;
    lbl_10_bss_2D40 = 0;
    if (player_count == 8) {
        r0 = lbl_80198B38 * 5;
    } else {
        r0 = lbl_80198B38;
    }
    lbl_10_bss_1224 = r0;
}

void fn_10_15F04(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    s32 r30;

    if ((course->unk2 & 0x10) != 0) {
        r30 = 1;
    } else {
        r30 = 0;
    }
    if (lbl_10_bss_2D40 == 0) {
        if ((lbl_10_bss_1224 == 0) && (fn_1_E5DC8() != 0) && (((s32) (merged_digital_inputs[2] & 0x100) != 0) || ((s32) (merged_digital_inputs[2] & 0x1000) != 0))) {
            fade_screen_to_color(0x101, 0, lbl_80198B38);
            lbl_10_bss_1224 = lbl_80198B38;
            g_fade_track_volume(lbl_80198B38, 2);
            if (r30 == 0) {
                call_SoundReqID_arg_0(0xD1);
            }
            lbl_10_bss_2D40 = 1;
        }
    } else if (lbl_10_bss_1224 <= 0) {
        if (r30 != 0) {
            lbl_10_bss_1220 = 0xC;
            lbl_10_bss_2D40 = 0;
            return;
        }
        lbl_10_bss_1220 = 0xE;
        lbl_10_bss_2D40 = 0;
    }
}

void fn_10_1603C(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    dest_all_events();
    event_init(0x11);
    event_init(0x13);
    event_init(0);
    event_init(0x14);
    event_init(0x10);
    if (fn_1_11DF64(course->unk0, &lbl_10_bss_4194, 1) != 0) {
        fn_1_11DE54(course->unk0, 1);
    } else {
        fn_1_11DE54(course->unk0, 0);
    }
    g_load_stage_for_menu_bg(0, 1);
    fade_screen_to_color(0x100, 0xFFFFFF, lbl_80198B38);
    lbl_10_bss_1220 = 0xD;
    lbl_10_bss_2D40 = 0;
}

void fn_10_1610C(void)
{
    s16 r6;

    if (lbl_10_bss_2D40 == 0) {
        if (fn_1_11E3A4() == 0) {
            for (r6 = 0; r6 < (s32) modeCtrl.unk30; r6++) {
                if ((s32) (pad_status_groups[player_pad_map[r6]].unk0[2].button & 0x100) != 0) {
                    lbl_10_bss_2D40 = 1;
                }
            }
        }
        if (lbl_10_bss_2D40 != 0) {
            fade_screen_to_color(0x101, 0, lbl_80198B38);
            lbl_10_bss_1224 = lbl_80198B38;
            g_fade_track_volume(0x3C, 2);
            call_SoundReqID_arg_0(0xD1);
            lbl_10_bss_2D40 = 1;
        }
    } else if (lbl_10_bss_1224 <= 0) {
        dest_all_events();
        fn_1_11C5E4();
        lbl_10_bss_121E = lbl_10_bss_121C;
        lbl_10_bss_121C = 0xE;
        lbl_10_bss_1220 = -1;
        lbl_10_bss_2D40 = 0;
    }
}

void fn_10_1623C(void)
{
    dest_all_events();
    lbl_10_bss_1220 = 0xF;
    lbl_10_bss_2D40 = 0;
}

void fn_10_1627C(void)
{
    if (lbl_10_bss_1224 <= 0) {
        g_return_to_sel_ngc(modeCtrl.gameType);
    }
}

void fn_10_162BC(struct Ball *ball)
{
    s32 r5;
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    if (((ball->prevFlags & 0x1ULL) != 0) || ((ball->flags & 0x1ULL) != 0)) {
        r5 = 1;
    } else {
        r5 = 0;
    }
    if (player->unk8 <= 0x7FFE) {
        player->unk8++;
    }
    if (player->unkA <= 0x7FFE) {
        player->unkA++;
    }
    if ((r5 != 0) && (player->unk8 > 5)) {
        player->unkA = 0;
    }
    if ((r5 == 0) && (player->unkA > 5)) {
        player->unk8 = 0;
    }
}

float fn_10_163A0(void)
{
    Vec sp14;
    Vec sp8;
    float f31;
    float f30;
    struct StageExtraData *tr30;
    s32 r29;

    tr30 = stagedef->extraData;
    f31 = 0.0f;
    r29 = 0;
    f30 = 0.0f;
    while (f30 < 100.0) {
        fn_10_181EC(tr30, f30, &sp14);
        fn_10_181EC(tr30, 0.01f + f30, &sp8);
        f31 += 10000.000223517423 * mathutil_vec_distance(&sp14, &sp8);
        r29++;
        f30 += 1.0;
    }
    return f31 / (float) r29;
}

void fn_10_164C8(struct Ball *ball)
{
    struct RacePlayer *tr30 = &lbl_10_bss_1230[ball->playerId];
    struct Race2Course *course = &lbl_10_bss_1208;
    Vec sp8;
    struct Race2CourseInfo *tr28;
    struct StageExtraData *tr27;
    struct StageExtraData *tr26;
    float *tr25;
    float *tr24;
    float tf31;
    float tf30;
    float f2;
    struct RacePlayer *tr3;

    tr28 = &lbl_10_data_2ADC[course->unk0];
    tr27 = stagedef->extraData;
    tr26 = (struct StageExtraData *)&stagedef->extraData->list[tr30->unk2A0];
    tf31 = (3.7037037037037037 * course->unkC);
    tr25 = &tr30->unk20C;
    tr24 = &tr30->unk208;
    tf30 = tr28->unk8 * tr28->unk8;
    if ((tr30->unk44 < 0x32) && (tr30->unk44 < lbl_10_bss_1208.unk4)) {
        tr30->unk20C = tr30->unk208;
        fn_10_17320(ball, tr27, &tr30->unk208);
        if ((ball->flags & 0x80000000000ULL) != 0) {
            tr30->unk2A8 = tr30->unk2A4;
            fn_10_17320(ball, tr26, &tr30->unk2A4);
        }
        switch (tr28->unk4) {                          /* irregular */
        case 0x86:
            fn_10_181EC(tr27, *tr24, &sp8);
            if (((0.0 <= *tr24) && (*tr24 <= 1.1)) || ((92.5 <= *tr24) && (*tr24 <= 94.9)) || ((98.1 <= *tr24) && (*tr24 <= 100.0))) {
                if (mathutil_vec_sq_distance(&sp8, &ball->pos) >= 100.0) {
                    fn_10_17190(ball, tr27, tr24);
                    if ((ball->flags & 0x80000000000ULL) != 0) {
                        fn_10_17190(ball, tr26, &tr30->unk2A4);
                    }
                }
            } else if (tr30->unk8 > 5) {
                if (((45.5 < *tr24) && (*tr24 < 50.0) && (ball->pos.y < (sp8.y - 0.5))) || ((55.0 < *tr24) && (*tr24 < 58.5) && (ball->pos.y < (sp8.y - 0.5))) || ((69.0 < *tr24) && (*tr24 < 79.0) && (ball->pos.y < (sp8.y - 0.5)))) {
                    fn_10_17190(ball, tr27, tr24);
                    if ((ball->flags & 0x80000000000ULL) != 0) {
                        fn_10_17190(ball, tr26, &tr30->unk2A4);
                    }
                } else {
                    if (mathutil_vec_sq_distance(&sp8, &ball->pos) > tf30) {
                        fn_10_17190(ball, tr27, tr24);
                        if ((ball->flags & 0x80000000000ULL) != 0) {
                            fn_10_17190(ball, tr26, &tr30->unk2A4);
                        }
                    }
                }
                if ((ball->flags & 0x80000000000ULL) != 0) {
                    if (fabs(*tr24 - tr30->unk2A4) > 3.0) {
                        fn_10_17190(ball, tr26, &tr30->unk2A4);
                    } else {
                        fn_10_181EC(tr26, tr30->unk2A4, &sp8);
                        if (mathutil_vec_sq_distance(&sp8, &ball->pos) > tf30) {
                            fn_10_17190(ball, tr26, &tr30->unk2A4);
                        }
                    }
                }
            }
            break;
        case 0x88:
            if ((15.0 < tr30->unk208) && (tr30->unk208 < 30.0)) {
                if ((tr30->unk8 > 5) && (ball->pos.y > 5.5)) {
                    fn_10_17190(ball, tr27, &tr30->unk208);
                    if ((ball->flags & 0x80000000000ULL) != 0) {
                        fn_10_17190(ball, tr26, &tr30->unk2A4);
                    }
                }
            } else if ((62.0 < tr30->unk208) && (tr30->unk208 < 69.0)) {
                if ((tr30->unk8 > 5) && (ball->pos.y < -4.0)) {
                    tr30->unk208 = 51.0f;
                    fn_10_17320(ball, tr27, &tr30->unk208);
                    if ((ball->flags & 0x80000000000ULL) != 0) {
                        fn_10_17190(ball, tr26, &tr30->unk2A4);
                    }
                }
            } else if ((69.0 < tr30->unk208) && (tr30->unk208 < 74.5) && (tr30->unk8 > 5) && (ball->pos.y < 5.0)) {
                fn_10_17190(ball, tr27, &tr30->unk208);
                if ((ball->flags & 0x80000000000ULL) != 0) {
                    fn_10_17190(ball, tr26, &tr30->unk2A4);
                }
            }
            if (((ball->flags & 0x80000000000ULL) != 0) && (tr30->unk8 > 5) && (fabs(*tr24 - tr30->unk2A4) > 3.0)) {
                if ((51.0 < *tr24) && (*tr24 < 60.0)) {
                    tr30->unk2A4 = 51.0f;
                    fn_10_17320(ball, tr26, &tr30->unk2A4);
                } else {
                    fn_10_17190(ball, tr26, &tr30->unk2A4);
                }
            }
            break;
        default:
        case 0x87:
            fn_10_181EC(tr27, tr30->unk208, &sp8);
            if (mathutil_vec_sq_distance(&sp8, &ball->pos) > tf30) {
                fn_10_17190(ball, tr27, &tr30->unk208);
            }
            if ((ball->flags & 0x80000000000ULL) != 0) {
                fn_10_181EC(tr26, tr30->unk2A4, &sp8);
                if (mathutil_vec_sq_distance(&sp8, &ball->pos) > tf30) {
                    fn_10_17190(ball, tr26, &tr30->unk2A4);
                }
            }
            break;
        }
        if (tr30->unkA > 0) {
            if (((100.0 - tf31) < *tr25) && (*tr24 < tf31)) {
                f2 = ((100.0 - *tr25) + *tr24);
            } else if (((100.0 - tf31) < *tr24) && (*tr25 < tf31)) {
                f2 = ((100.0 - *tr24) + *tr25);
            } else {
                f2 = fabs(*tr24 - *tr25);
            }
            if (f2 > (10.0 * course->unkC)) {
                tr30->unk208 = tr30->unk20C;
                tr30->unk2A4 = tr30->unk2A8;
            }
        }
        if ((ball->flags & 0x80000000000ULL) != 0) {
            fn_10_181EC(tr26, tr30->unk2A4, &tr30->unk2AC);
        }
        tr30->unk214 = tr30->unk210;
        if (((u32) (dip_switches & 1) != 0) || ((u32) (dip_switches & 0x20000000) != 0)) {
            tr3 = &lbl_10_bss_1230[ball->playerId];
            if (((ball->flags & 0x80000000000ULL) == 0) && ((s32) (pad_status_groups[tr3->unk14].unk0[0].button & 0x40) != 0) && ((s32) (pad_status_groups[tr3->unk14].unk0[0].button & 0x20) != 0) && ((s32) (pad_status_groups[tr3->unk14].unk0[2].button & 0x400) != 0)) {
                tr30->unk1F4 = tr30->unk20C = 99.99f;
                tr30->unk1F0 = tr30->unk208 = 0.01f;
                tr30->unk1FC = tr30->unk214 = 99.99f;
                tr30->unk1F8 = tr30->unk210 = 100.01f;
            }
        }
        if ((100.0 - tf31) <= *tr25) {
            if (*tr25 <= 100.0) {
                if (0.0 <= *tr24) {
                    if (*tr24 <= tf31) {
                        tr30->unk210 += (100.0 + (*tr24 - *tr25));
                        return;
                    }
                }
            }
        }
        if (0.0f <= *tr25) {
            if (*tr25 <= tf31) {
                if ((100.0 - tf31) <= *tr24) {
                    if (*tr24 <= 100.0) {
                        tr30->unk210 += ((*tr24 - *tr25) - 100.0);
                        return;
                    }
                }
            }
        }
        tr30->unk210 += *tr24 - *tr25;
        if (tr30->unk210 >= 0.0) {
            tr30->unk210 = *tr24;
            if (current_stage_id == 0x86) {
                if ((tr30->unk214 >= 0.0) && ((tr30->unk210 - tr30->unk214) > 60.0)) {
                    tr30->unk210 -= 100.0;
                }
            }
        } else {
            tr30->unk210 = (*tr24 - 100.0);
            if ((tr30->unk214 < 0.0) && ((tr30->unk210 - tr30->unk214) < -25.0)) {
                if (current_stage_id == 0x85) {
                    if (*tr24 > 50.0) {
                        tr30->unk210 = (*tr24 - 100.0);
                        tr30->unk214 = tr30->unk210;
                        return;
                    }
                    tr30->unk210 = *tr24;
                    tr30->unk214 = tr30->unk210;
                    return;
                }
                tr30->unk210 = (100.0 + tr30->unk210);
            }
        }
    }
}

void fn_10_17190(struct Ball *ball, struct StageExtraData *arg1, float *arg2)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    Vec sp8;
    float f31;
    float f30;
    float f29;
    float f28;
    float f27;
    float tf3;

    f31 = 0.0f;
    fn_10_181EC(arg1, f31, &sp8);
    f30 = mathutil_vec_sq_distance(&ball->pos, &sp8);
    f28 = 100.0 / course->unk8;
    f29 = 0.0f;
    while (f29 < course->unk8) {
        f27 = f29 * f28;
        fn_10_181EC(arg1, f27, &sp8);
        tf3 = mathutil_vec_sq_distance(&ball->pos, &sp8);
        if (tf3 < f30) {
            f31 = f27;
            f30 = tf3;
        }
        f29 += 10.0;
    }
    *arg2 = f31;
    fn_10_17320(ball, arg1, arg2);
}

void fn_10_17320(struct Ball *ball, struct StageExtraData *arg1, float *arg2)
{
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float f31;
    float tf0;
    s16 r30;
    s16 r29;
    s16 r28;
    s16 r27;

    r29 = __abs((s32) *arg2);
    r30 = r29;
    fn_10_181EC(arg1, (float) r30, &sp14);
    fn_10_181EC(arg1, 0.01f + (float) r30, &sp8);
    PSVECSubtract(&sp8, &sp14, &sp2C);
    vec_normalize_len(&sp2C);
    PSVECSubtract(&ball->pos, &sp14, &sp20);
    f31 = mathutil_vec_dot_prod(&sp2C, &sp20);
    if (f31 >= 0.0) {
        r28 = 1;
    } else {
        r28 = -1;
    }
    r27 = 0;
    while (r27 < 0x64) {
        r29 += r28;
        if (r29 == 0x64) {
            r29 = 0;
        } else if (r29 == -1) {
            r29 = 0x63;
        }
        fn_10_181EC(arg1, (float) r29, &sp14);
        fn_10_181EC(arg1, 0.01f + (float) r29, &sp8);
        PSVECSubtract(&sp8, &sp14, &sp2C);
        vec_normalize_len(&sp2C);
        PSVECSubtract(&ball->pos, &sp14, &sp20);
        tf0 = mathutil_vec_dot_prod(&sp2C, &sp20);
        if ((r28 > 0) && (f31 >= 0.0) && (tf0 < 0.0)) {
            tf0 = fabs(tf0);
            *arg2 = r30 + (f31 / (f31 + tf0));
            break;
        }
        if ((r28 < 0) && (f31 < 0.0) && (tf0 >= 0.0)) {
            f31 = fabs(f31);
            *arg2 = r30 - (f31 / (f31 + tf0));
            if (*arg2 < 0.0) {
                *arg2 += 100.0;
            }
            break;
        }
        r30 = r29;
        f31 = tf0;
        r27++;
    }
    if (*arg2 < 0.0) {
        *arg2 += 100.0;
    }
    if (*arg2 >= 100.0) {
        *arg2 -= 100.0;
    }
}

void fn_10_176A0(struct Ball *ball)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    float tf2;
    float tf2_2;
    float tf3;
    float tf3_2;
    struct RacePlayer *player;
    struct RaceCheckpoint *tr7;
    struct RaceCheckpoint *tr8;

    player = (struct RacePlayer *)ball->unk150;
    player->unk46 = player->unk44;
    tr7 = &player->unk48[player->unk46];
    tr8 = &player->unk48[player->unk44];
    if (fabs(player->unk210) >= 100.0) {
        player->unk44++;
        player->flags |= 0x800;
        player->unk1EC = (s16) (lbl_80198B38 * 2);
        if (player->unk210 >= 100.0) {
            if (player->unk208 <= player->unk20C) {
                tf2 = player->unk208;
                tf3 = (100.0 - player->unk20C);
                tr7->unk4 += (tf3 / (tf2 + tf3));
                tr8->unk4 += (tf2 / (tf2 + tf3));
            } else {
                tr8->unk0++;
            }
            player->unk210 = player->unk208;
            player->unk1F0 = course->unkC;
            player->unk1F4 = 0.0f;
            player->unk1F8 = course->unkC;
            player->unk1FC = 0.0f;
        } else {
            if ((course->unk2 & 0x40) == 0) {
                lbl_10_bss_4194.a.unkB = 1;
            }
            player->flags |= 0x100000;
            if (player->unk208 >= player->unk20C) {
                tf2_2 = (100.0 - player->unk208);
                tf3_2 = player->unk20C;
                tr7->unk4 += (tf2_2 / (tf2_2 + tf3_2));
                tr8->unk4 += (tf3_2 / (tf2_2 + tf3_2));
            } else {
                tr8->unk0++;
            }
            player->unk210 = (player->unk208 - 100.0);
            player->unk1F0 = -course->unkC;
            player->unk1F4 = 0.0f;
            player->unk1F8 = -course->unkC;
            player->unk1FC = 0.0f;
        }
    } else {
        tr8->unk0++;
    }
    player->unk204 = player->unk200;
    player->unk200 = (0.01 * (course->unk8 * ((100.0 * player->unk44) + player->unk1F8)));
    if (player->unk44 == lbl_10_bss_1208.unk4) {
        player->unk1E4 = player->unk48[player->unk44 - 1].unk4;
        player->flags |= 2;
        fn_10_182C4((struct RaceCheckpoint *)&player->unk1E0, &player->unk1E8);
        return;
    }
    player->unk1E0++;
}

void fn_10_17960(struct Ball *ball)
{
    struct RacePlayer *player;
    struct StageExtraData *r3;

    player = (struct RacePlayer *)ball->unk150;
    r3 = stagedef->extraData;
    if (player->unk8 > 5) {
        player->unk1F4 = player->unk1F0;
        player->unk1FC = player->unk1F8;
        player->unk1F0 = player->unk208;
        player->unk1F8 = player->unk210;
        player->unk24 = player->unk18;
        fn_10_181EC(r3, player->unk1F0, &player->unk18);
    }
}

void fn_10_179E0(void)
{
    struct RacePlayer *r5;
    s16 r6;
    struct RacePlayer *tr4;

    qsort(lbl_10_bss_2D68, player_count, sizeof(lbl_10_bss_2D68[0]), fn_10_17AE8);
    r5 = *lbl_10_bss_2D68;
    r5->unk3E = 1;
    for (r6 = 1; r6 < player_count; r6++) {
        tr4 = lbl_10_bss_2D68[r6];
        if (((u32) (r5->flags & 2) != 0) && ((u32) (tr4->flags & 2) != 0) && ((u8) r5->unk1E8 == tr4->unk1E8) && ((u8) r5->unk1E9 == tr4->unk1E9) && ((u8) r5->unk1EA == tr4->unk1EA)) {
            tr4->unk3E = r5->unk3E;
        } else {
            tr4->unk3E = (u16) (r6 + 1);
        }
        r5 = tr4;
    }
}

s32 fn_10_17AE8(struct RacePlayer **arg0, struct RacePlayer **arg1)
{
    s32 r3;
    s32 r3_2;
    struct RacePlayer *tr3;
    struct RacePlayer *tr4;

    tr3 = *arg0;
    tr4 = *arg1;
    if (((u32) (tr3->flags & 2) == 0) && ((u32) (tr4->flags & 2) != 0)) {
        return 1;
    }
    if (((u32) (tr3->flags & 2) != 0) && ((u32) (tr4->flags & 2) == 0)) {
        return -1;
    }
    if (((u32) (tr3->flags & 2) != 0) && ((u32) (tr4->flags & 2) != 0)) {
        if (((u8) tr3->unk1E8 == tr4->unk1E8) && ((u8) tr3->unk1E9 == tr4->unk1E9) && ((u8) tr3->unk1EA == tr4->unk1EA)) {
            if ((u16) tr3->unk0 > tr4->unk0) {
                r3_2 = 1;
            } else {
                r3_2 = -1;
            }
            return r3_2;
        }
        if (((float) tr3->unk1E0 + tr3->unk1E4) > ((float) tr4->unk1E0 + tr4->unk1E4)) {
            r3 = 1;
        } else {
            r3 = -1;
        }
        return r3;
    }
    if (((100.0 * tr3->unk44) + tr3->unk1F8) < ((100.0 * tr4->unk44) + tr4->unk1F8)) {
        return 1;
    }
    return -1;
}

void fn_10_17CD8(s32 arg0)
{
    struct RacePlayer *r6;
    struct RacePlayer *tr7;
    s16 r3;

    for (r3 = 0; r3 < player_count; r3++) {
        lbl_10_bss_4238[r3] = lbl_10_bss_2D68[r3];
    }
    qsort(lbl_10_bss_4238, player_count, sizeof(lbl_10_bss_4238[0]), fn_10_17E50);
    r6 = lbl_10_bss_4238[0];
    r6->unk40 = 1;
    for (r3 = 1; r3 < player_count; r3++) {
        tr7 = lbl_10_bss_4238[r3];
        if (lbl_10_bss_2D50[r6->unk0] == lbl_10_bss_2D50[tr7->unk0]) {
            tr7->unk40 = r6->unk40;
        } else {
            tr7->unk40 = (u16) (r3 + 1);
        }
        r6 = tr7;
    }
    if (arg0 != 0) {
        for (r3 = 0; r3 < player_count; r3++) {
            lbl_10_bss_2D68[r3] = lbl_10_bss_4238[r3];
            (lbl_10_bss_2D68[r3])->unk3E = (lbl_10_bss_2D68[r3])->unk40;
        }
    }
}

s32 fn_10_17E50(struct RacePlayer **arg0, struct RacePlayer **arg1)
{
    s32 r3;
    struct RacePlayer *tr6;
    struct RacePlayer *tr7;

    tr6 = *arg0;
    tr7 = *arg1;
    if (lbl_10_bss_2D50[tr6->unk0] == lbl_10_bss_2D50[tr7->unk0]) {
        if ((u16) tr6->unk0 > tr7->unk0) {
            r3 = 1;
        } else {
            r3 = -1;
        }
        return r3;
    }
    if (lbl_10_bss_2D50[tr6->unk0] < lbl_10_bss_2D50[tr7->unk0]) {
        return 1;
    }
    return -1;
}

void fn_10_17F04(void)
{
    qsort(lbl_10_bss_2D68, player_count, sizeof(lbl_10_bss_2D68[0]), fn_10_17F44);
}

s32 fn_10_17F44(struct RacePlayer **arg0, struct RacePlayer **arg1)
{
    struct RacePlayer *tr4;
    struct RacePlayer *tr5;

    tr5 = *arg0;
    tr4 = *arg1;
    if ((u16) tr5->unk3E > tr4->unk3E) {
        return 1;
    }
    if (((u16) tr5->unk3E == tr4->unk3E) && ((u16) tr5->unk0 > tr4->unk0)) {
        return 1;
    }
    return -1;
}

s32 fn_10_17FA0(void)
{
    s32 r3;
    struct Ball *r8;
    s8 *r9;
    struct RacePlayer *r10;
    s16 r11;
    s16 r12;

    r8 = balls;
    r9 = g_poolInfo.playerBuf;
    r11 = 0;
    r3 = 0;
    r12 = 0;
    while (r12 < 8) {
        if (*r9 == 2) {
            r10 = &lbl_10_bss_1230[r8->playerId];
            if (((r8->flags & 0x80000000000ULL) == 0) && ((u32) (r10->flags & 2) != 0)) {
                r11++;
            }
        }
        r12++;
        r8++;
        r10++;
        r9++;
    }
    if ((s32) modeCtrl.unk30 == 1) {
        if (r11 == 1) {
            r3 = 1;
        }
    } else if (r11 >= modeCtrl.unk30) {
        r3 = 1;
    } else if ((player_count >= 2) && ((u32) (lbl_10_bss_2D68[player_count - 2]->flags & 2) != 0)) {
        r3 = 1;
    }
    return r3;
}

void fn_10_180D8(struct Ball *ball)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct Race2CourseInfo *info = &lbl_10_data_2ADC[course->unk0];
    struct RacePlayer *player;

    player = (struct RacePlayer *)ball->unk150;
    if (((ball->flags & 0x80000000000ULL) == 0) && ((u32) (player->flags & 2) == 0) && (player->unk44 > player->unk46)) {
        race_frames_remaining += info->unk5A * lbl_80198B38;
        if (race_frames_remaining >= (lbl_80198B38 * 0x258)) {
            race_frames_remaining = lbl_80198B38 * 0x258;
        }
    }
}

s16 fn_10_181C8(u16 arg0)
{
    return (s16) (((player_count - arg0) + 1) * 2);
}

void fn_10_181EC(struct StageExtraData *arg0, float farg0, Vec *arg1)
{
    if (farg0 < 0.0) {
        farg0 += 100.0;
    }
    if (farg0 >= 100.0) {
        farg0 -= 100.0;
    }
    arg1->x = evaluate_stagedef_keyframe(farg0, arg0->channels.a.count, arg0->channels.a.frames);
    arg1->y = evaluate_stagedef_keyframe(farg0, arg0->channels.b.count, arg0->channels.b.frames);
    arg1->z = evaluate_stagedef_keyframe(farg0, arg0->channels.c.count, arg0->channels.c.frames);
}

void fn_10_182C4(struct RaceCheckpoint *ball, u8 *arg1)
{
    s32 r0;
    s32 r5;
    s32 r7;

    r5 = ball->unk0 / 60;
    r0 = r5 / 60;
    r5 = r5 % 60;
    r7 = (1000.0 * ((float) (ball->unk0 % 60) + ball->unk4)) / 60.0;
    r5 += r7 / 1000;
    r7 = r7 % 1000;
    r7 = r7 / 10;
    if (r0 >= 0x64) {
        r0 = 0x63;
        r5 = 0x3B;
        r7 = 0x63;
    }
    arg1[0] = r0;
    arg1[1] = r5;
    arg1[2] = r7;
}

void fn_10_18430(void)
{
    lbl_10_bss_4258 = (lbl_1_data_65768 + lbl_1_data_6576A) * 2;
    lbl_10_bss_425A = -1;
}

void fn_10_18474(void)
{
    struct Effect spC;
    float sp8;
    struct Ball *r31;
    struct RacePlayer *r30;
    s16 r29;
    struct Ball *tr28;
    s16 r5;
    s32 r0;

    if (lbl_10_bss_4258 > 0) {
        lbl_10_bss_4258--;
        if (((lbl_10_bss_4258 >= 0) && (lbl_10_bss_4258 < lbl_1_data_65768)) || (((lbl_1_data_6576A + (lbl_1_data_65768 + lbl_1_data_6576A)) <= lbl_10_bss_4258) && (lbl_10_bss_4258 < ((lbl_1_data_65768 + lbl_1_data_6576A) * 2)))) {
            lbl_10_bss_425A++;
        } else {
            return;
        }
        r29 = 0;
        r31 = balls;
        r30 = lbl_10_bss_1230;
        while (r29 < 8) {
            if ((r31->status == 2) && ((r31->flags & 0x80000000000ULL) == 0)) {
                if ((r30->unk2 <= -5) || (r30->unk2 >= 5)) {
                    r30->unk33A[lbl_10_bss_425A] = 0;
                } else {
                    OSs8tof32(&r30->unk3, &sp8);
                    sp8 /= 60.0;
                    if (sp8 > 1.0) {
                        sp8 = 1.0f;
                    } else if (sp8 < -1.0) {
                        sp8 = -1.0f;
                    }
                    sp8 *= 127.0;
                    OSf32tos8(&sp8, (s8 *)&r30->unk33A[lbl_10_bss_425A]);
                }
            }
            r29++;
            r31++;
            r30++;
        }
        if (lbl_10_bss_4258 <= 0) {
            r29 = 0;
            r31 = balls;
            r30 = lbl_10_bss_1230;
            while (r29 < 8) {
                if (r31->status == 2) {
                    if ((r31->flags & 0x80000000000ULL) == 0) {
                        r0 = 0;
                        for (r5 = 0; r5 < lbl_1_data_65768; r5++) {
                            if ((s8) r30->unk33A[r5] != -0x7F) {
                                r0 = 1;
                                break;
                            }
                        }
                        if (((u32) (dip_switches & 1) != 0) && ((u32) (lbl_1_bss_D3360 & 8) != 0)) {
                            r0 = 0;
                        }
                        if (r0 != 0) {
                            goto next;
                        }
                        for (r5 = lbl_1_data_65768; r5 < (s32) (lbl_1_data_65768 * 2); r5++) {
                            if ((s8) r30->unk33A[r5] != 0x7F) {
                                r0 = 1;
                                break;
                            }
                        }
                        if (((u32) (dip_switches & 1) != 0) && ((u32) (lbl_1_bss_D3360 & 8) != 0)) {
                            r0 = 0;
                        }
                        if (r0 != 0) {
                            goto next;
                        }
                    }
                    r30->unk336 = 0xA;
                    tr28 = current_ball;
                    current_ball = r31;
                    memset(&spC, 0, sizeof(spC));
                    spC.type = 9;
                    spC.playerId = r31->playerId;
                    fn_10_DDA8(&spC, NULL);
                    call_SoundReqID_arg_0(0x2852);
                    call_SoundReqID_arg_0(0xD3);
                    current_ball = tr28;
                }
            next:
                r29++;
                r31++;
                r30++;
            }
        }
    }
}

s32 lbl_10_data_1740 = 1;

s32 fn_10_188A0(s32 arg0)
{
    s32 tr0;

    tr0 = lbl_10_data_1740;
    lbl_10_data_1740 = arg0;
    return tr0;
}

u16 fn_10_188C0(void)
{
    lbl_10_data_1740 = (lbl_10_data_1740 * 0x41C64E6D) + 0x3039;
    return (u16) (((u32) lbl_10_data_1740 >> 0x10) & 0x7FFF);
}

float fn_10_18900(void)
{
    lbl_10_data_1740 = (lbl_10_data_1740 * 0x41C64E6D) + 0x3039;
    return (float) ((u16) (((u32) lbl_10_data_1740 >> 0x10) & 0x7FFF) / 32767.0);
}

void fn_10_1897C(void)
{
    struct RaceInputRec *p;
    s32 r5;

    if (lbl_10_bss_1228 != NULL) {
        lbl_10_bss_2D34 = -1;
        for (r5 = 0; r5 < 0x2A30; r5++) {
            struct RaceInputRec sp8 = {0, 0, 0, 0};

            p = &sp8;
            lbl_10_bss_1228[r5] = *p;
        }
    }
}

void fn_10_18A00(void)
{
    struct RacePlayer *player = lbl_10_bss_1230;
    struct RaceInputRec *tr4;

    if ((lbl_10_bss_1228 != NULL) && (lbl_10_bss_2D34 < 0x2A2F)) {
        lbl_10_bss_2D34++;
        tr4 = &lbl_10_bss_1228[lbl_10_bss_2D34];
        tr4->stickX = pad_status_groups[player->unk14].unk0[0].stickX;
        tr4->stickY = pad_status_groups[player->unk14].unk0[0].stickY;
        tr4->button = pad_status_groups[player->unk14].unk0[2].button;
        tr4->cam = cameras->rotY;
    }
}

void fn_10_18AA8(void)
{
    lbl_10_bss_2D34 = -1;
    lbl_10_bss_2D30 = -1;
}

void fn_10_18ACC(void)
{
    if (lbl_10_bss_2D34 < 0x2A2F) {
        lbl_10_bss_2D34++;
        if (lbl_10_bss_2D30 > 0) {
            lbl_10_bss_2D30--;
        }
    }
}

void fn_10_18B30(void)
{
    s16 r31;

    for (r31 = 0; r31 < player_count; r31++) {
        g_load_mal_files_from_disc(lbl_10_bss_2D48[r31], 3, 1, 0xC);
    }
}

void fn_10_18B9C(void)
{
    s16 r31;

    for (r31 = 0; r31 < player_count; r31++) {
        g_something_freeing_chara_heap_4(lbl_10_bss_2D48[r31], 3, 1);
    }
}

void fn_10_18C04(void)
{
    s16 r31;

    for (r31 = 0; r31 < player_count; r31++) {
        if (player_count <= 4) {
            preload_ape(lbl_10_bss_2D48[r31], 0, 1, 0x1A);
        } else {
            preload_ape(lbl_10_bss_2D48[r31], 1, 1, 0x1A);
        }
    }
}

