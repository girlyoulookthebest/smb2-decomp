#include "background.h"
#include "boat.h"
#include "macros.h"
#include "bitmap.h"
#include "event.h"
#include "input.h"
#include "sound.h"
#include "sprite.h"

void *OSGetStackPointer(void);
#ifdef __MWERKS__
void fn_1_CCAA0(void *, int, float, float, float);
#else
/* real prototype (ord_tbl.c); PPC passes floats in FPRs so the retail
 * call order differs from the C one */
void fn_1_CCAA0(float, float, float, void *, int);
#define fn_1_CCAA0(buf, n, ofs, mn, mx) (fn_1_CCAA0)(ofs, mn, mx, buf, n)
#endif
void fn_1_113768(struct Ape *ape, float x);
void fn_1_11360C(struct Ape *ape, float x);
void fn_1_11C5E4(void);
void fn_1_11DE54();
int fn_1_11DF64();
int fn_1_11E1C0();
int fn_1_11E3A4(void);
void fn_1_358E8(void);
void fn_1_36140(void);
void fn_1_E4708();
int fn_1_E5DC8(void);
void fn_1_E6760();
void g_change_music_volume();
int fn_1_B2FB0(void);

extern struct { u8 unk0; u8 unk1[3]; u16 unk4; } lbl_1_bss_1708;
extern void (*lbl_1_bss_70E94)(void);
extern u8 lbl_1_bss_A87E0[];
extern u32 lbl_1_bss_D432C;
extern u32 lbl_1_bss_D4334[];
extern float lbl_1_bss_D43B0;
extern struct BoatCpuPoint lbl_1_bss_D43B4[];
extern s32 active_monkey_id[];
extern s32 bg_heap;
extern s32 stage_heap;
extern s32 lbl_1_data_6581C;
extern s32 lbl_1_data_6592C[];
extern u8 lbl_1_data_65970[];

u32 lbl_14_data_0[0x5C] = {
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
void (*lbl_14_data_170[])(void) = {
    g_load_boat, fn_14_16C0, fn_14_18B4, fn_14_193C, fn_14_1A24, fn_14_1A84, fn_14_1C54, fn_14_23C0,
    fn_14_2438, fn_14_2620, fn_14_2890, fn_14_2A48, fn_14_2AA4, fn_14_2BE4, fn_14_2F38, fn_14_3084,
    fn_14_31A4,
};
s16 lbl_14_data_1B4 = -1;
u8 lbl_14_data_1B8[4] = {1, 2, 4, 7};

s16 lbl_14_bss_0;
s32 lbl_14_bss_4;
u32 lbl_14_bss_8;
u32 lbl_14_bss_C;
u32 lbl_14_bss_10;
s32 lbl_14_bss_14;
void *lbl_14_bss_18;
u32 lbl_14_bss_1C;
struct BoatInfo20 lbl_14_bss_20;
u8 lbl_14_bss_24;
u8 lbl_14_bss_25;
u8 lbl_14_bss_28[4];
u8 lbl_14_bss_2C[4];
s32 lbl_14_bss_30[0xC];
s32 lbl_14_bss_60;
u32 lbl_14_bss_64;
s8 lbl_14_bss_68;
u32 lbl_14_bss_6C;
void *lbl_14_bss_70;
void *lbl_14_bss_74;
void *lbl_14_bss_78;
void *lbl_14_bss_7C;
s32 lbl_14_bss_80;
s8 lbl_14_bss_84[8];
s8 lbl_14_bss_8C[8];
struct BoatRecord lbl_14_bss_94[2];
struct BoatInfo20 *lbl_14_bss_AC;
s32 lbl_14_bss_B0;
struct GMAModel *lbl_14_bss_B4[0x4D];
s32 (*lbl_14_bss_1E8)(float);
float lbl_14_bss_1EC;
struct GMAModel *lbl_14_bss_1F0[4];
struct GMAModel *lbl_14_bss_200;
struct GMAModel *lbl_14_bss_204;

static inline int is_race_over(void)
{
    if (modeCtrl.unk30 <= 1) {
        if ((lbl_14_bss_208[0].unk8 & 8) != 0) {
            return 1;
        }
    } else {
        struct Ball *ball;
        struct BoatPlayer *pl;
        s16 count;
        s16 i;

        if ((lbl_14_bss_2988[lbl_14_bss_24 - 2]->unk8 & 8) != 0) {
            return 1;
        }
        ball = balls;
        pl = lbl_14_bss_208;
        count = 0;
        i = 0;
        while (i < lbl_14_bss_24) {
            if (ball->status == 2 && !(ball->flags & 0x80000000000LL) && (pl->unk8 & 8) != 0) {
                count++;
            }
            i++;
            ball++;
            pl++;
        }
        if (count >= modeCtrl.unk30) {
            return 1;
        }
    }
    return 0;
}

/* pin .bss definition order one symbol at a time, so no pool anchor is created; stripped at link */
void boat_bss_pin_0(void) { lbl_14_bss_0 = 0; }
void boat_bss_pin_1(void) { lbl_14_bss_4 = 0; }
void boat_bss_pin_2(void) { lbl_14_bss_8 = 0; }
void boat_bss_pin_3(void) { lbl_14_bss_C = 0; }
void boat_bss_pin_4(void) { lbl_14_bss_10 = 0; }
void boat_bss_pin_5(void) { lbl_14_bss_14 = 0; }
void boat_bss_pin_6(void) { lbl_14_bss_18 = NULL; }
void boat_bss_pin_7(void) { lbl_14_bss_1C = 0; }
void boat_bss_pin_8(void) { lbl_14_bss_20.unk0 = 0; }
void boat_bss_pin_9(void) { lbl_14_bss_24 = 0; }
void boat_bss_pin_10(void) { lbl_14_bss_25 = 0; }
void boat_bss_pin_11(void) { lbl_14_bss_28[0] = 0; }
void boat_bss_pin_12(void) { lbl_14_bss_2C[0] = 0; }
void boat_bss_pin_13(void) { lbl_14_bss_30[0] = 0; }
void boat_bss_pin_14(void) { lbl_14_bss_60 = 0; }
void boat_bss_pin_15(void) { lbl_14_bss_64 = 0; }
void boat_bss_pin_16(void) { lbl_14_bss_68 = 0; }
void boat_bss_pin_17(void) { lbl_14_bss_6C = 0; }
void boat_bss_pin_18(void) { lbl_14_bss_70 = NULL; }
void boat_bss_pin_19(void) { lbl_14_bss_74 = NULL; }
void boat_bss_pin_20(void) { lbl_14_bss_78 = NULL; }
void boat_bss_pin_21(void) { lbl_14_bss_7C = NULL; }
void boat_bss_pin_22(void) { lbl_14_bss_80 = 0; }
void boat_bss_pin_23(void) { lbl_14_bss_84[0] = 0; }
void boat_bss_pin_24(void) { lbl_14_bss_8C[0] = 0; }
void boat_bss_pin_25(void) { lbl_14_bss_94[0].chara = 0; }
void boat_bss_pin_26(void) { lbl_14_bss_AC = NULL; }
void boat_bss_pin_27(void) { lbl_14_bss_B0 = 0; }
void boat_bss_pin_28(void) { lbl_14_bss_B4[0] = NULL; }
void boat_bss_pin_29(void) { lbl_14_bss_1E8 = 0; }
void boat_bss_pin_30(void) { *(s32 *) &lbl_14_bss_1EC = 0; }
void boat_bss_pin_31(void) { lbl_14_bss_1F0[0] = NULL; }
void boat_bss_pin_32(void) { lbl_14_bss_200 = NULL; }
void boat_bss_pin_33(void) { lbl_14_bss_204 = NULL; }

void fn_14_0(void)
{
}

void _prolog(void)
{
    if ((u32) (g_some_other_flags & 4) != 0) {
        g_very_similar_to_puts("[MONKEY BOAT] LOADED\n");
    }
    fn_14_138();
}

void _epilog(void)
{
    fn_14_AF0();
    if ((u32) (g_some_other_flags & 4) != 0) {
        g_very_similar_to_puts("[MONKEY BOAT] PURGED\n");
    }
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
    OSPanic("boat.c", 0xB0, "\n");
}

void fn_14_138(void)
{
    u8 monkeys[4] = {0, 1, 2, 3};
    s16 i;

    dest_all_events();
    sub_mode_request = 0xC2;
    g_set_minigame_specific_funcs(fn_14_674, fn_14_AD0, fn_14_66DC, fn_14_E148);
    bg_heap = stage_heap;
    if ((u32) (dip_switches & 1) != 0) {
        for (i = 0; i < 0x80; i++) {
            lbl_1_bss_D43B4[i] = (struct BoatCpuPoint){-1.0f, 0.0f, 0};
        }
        lbl_1_bss_D43B4[0] = (struct BoatCpuPoint){0.0f, 0.5f, 0};
        lbl_1_bss_D43B4[1] = (struct BoatCpuPoint){100.0f, 0.5f, 0};
    }
    if (modeCtrl.unk30 >= 2) {
        fn_1_CCAA0(lbl_1_bss_A87E0, 0x1000, 0.0f, 0.0f, 400.0f);
    } else {
        fn_1_CCAA0(lbl_1_bss_A87E0, 0x4000, 0.0f, 0.0f, 400.0f);
    }
    SoundGroupLoad(0x12);
    fn_1_358E8();
    g_SoftStreamStart_with_some_defaults(0x4D);
    g_SoftStreamStart_with_some_defaults(0x4C);
    g_load_minigame_gma_tpl(8);
    load_bmp_by_id(0xE);
    lbl_14_data_1B4 = -1;
    lbl_14_bss_8 = 0;
    lbl_14_bss_C = 0;
    lbl_14_bss_10 = 0;
    lbl_14_bss_14 = -1;
    lbl_14_bss_18 = 0;
    lbl_14_bss_1C = OSGetTick() / 4;
    fn_14_65E4();
    fn_14_E0FC();
    fn_14_32FC();
    fn_14_15624();
    fn_14_1864C();
    lbl_14_bss_20.unk0 = lbl_1_bss_1708.unk0;
    lbl_14_bss_20.unk1 = lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0];
    lbl_14_bss_20.unk2 = lbl_1_bss_1708.unk4;
    if (lbl_14_bss_20.unk2 & 1) {
        lbl_14_bss_24 = 1;
        lbl_14_bss_20.unk2 &= ~2;
        lbl_14_bss_20.unk2 &= ~4;
        lbl_14_bss_20.unk2 &= ~8;
        lbl_14_bss_20.unk1 = lbl_1_data_65970[lbl_14_bss_20.unk0];
    } else {
        if (lbl_14_bss_20.unk2 & 0x10) {
            lbl_14_bss_25 = 0;
        }
        if (modeCtrl.unk30 == 1) {
            lbl_14_bss_20.unk2 |= 2;
        }
        if (lbl_14_bss_20.unk2 & 2) {
            lbl_14_bss_24 = 4;
        } else {
            lbl_14_bss_24 = modeCtrl.unk30;
        }
    }
    if (!(lbl_14_bss_20.unk2 & 0x10) && modeCtrl.unk30 == 1) {
        lbl_14_bss_18 = alloc_from_heap_or_panic(currentHeap, lbl_80198B38 * 0xB40, "boat.c", 0x120);
    }
    lbl_14_bss_0 = 0;
    for (i = 0; i < modeCtrl.unk30; i++) {
        lbl_14_bss_28[i] = active_monkey_id[i];
    }
    for (i = modeCtrl.unk30; i < lbl_14_bss_24; i++) {
        lbl_14_bss_28[i] = monkeys[(u32) (rand() & 0x7FFF) % 4];
    }
    for (i = 0; i < lbl_14_bss_24; i++) {
        if (i >= modeCtrl.unk30) {
            if (modeCtrl.unk30 == 1) {
                lbl_14_bss_2C[i] = 2;
            } else {
                lbl_14_bss_2C[i] = 3;
            }
        } else if (modeCtrl.unk30 == 1) {
            lbl_14_bss_2C[i] = 1;
        } else {
            lbl_14_bss_2C[i] = 3;
        }
    }
    fn_14_DB0();
}

void fn_14_674(void)
{
    if ((u32) (modeCtrl.courseFlags & 0x4000) != 0) {
        lbl_14_bss_8 &= ~4;
        modeCtrl.courseFlags &= ~0x4000;
        lbl_14_bss_1C = OSGetTick() / 4;
        if (lbl_14_bss_20.unk0 == 2) {
            SoundOffID(0x273);
            SoundOffID(0x274);
        }
        dest_all_events();
        fn_14_BC8(0);
    }
    if ((u32) (g_some_other_flags & 0xA) != 0) {
        if ((u32) (dip_switches & 1) != 0 && (pad_status_groups[0].unk0[0].button & 0x40) && (pad_status_groups[0].unk0[0].button & 0x20) && (pad_status_groups[0].unk0[2].button & 0x200)) {
            struct BoatCpuPoint *p;
            s16 i;

            printf("static BOAT_CPU cpu_crs??_path??[] = {\n");
            i = 0;
            p = lbl_1_bss_D43B4;
            while (i < 0x80) {
                if (p->unk0 < 0.0) {
                    printf("\tBOAT_CPU_END\n");
                    break;
                }
                printf("\t{ %6.2f,\t%4.2f,\t\t% d\t},\n", p->unk0, p->unk4, p->unk8);
                i++;
                p++;
            }
            printf("};\n");
        }
        return;
    }
    if ((u32) (dip_switches & 1) != 0) {
        s16 i;

        for (i = 0; i < 0xC; i++) {
            lbl_1_bss_D4334[i] = 0;
        }
    }
    lbl_14_bss_30[0] = OSGetTick();
    if (lbl_14_data_1B4 >= 0 && lbl_14_data_1B4 < 0x11) {
        lbl_14_bss_0 = lbl_14_data_1B4;
        lbl_1_data_6581C = lbl_1_data_6592C[lbl_14_bss_0];
        lbl_14_data_1B4 = -1;
    }
    if ((u32) (lbl_14_bss_8 & 4) != 0) {
        if ((u32) (lbl_14_bss_8 & 8) == 0) {
            struct BoatPlayer *pl = lbl_14_bss_208;

            if (lbl_14_bss_60 >= lbl_80198B38 && ((pad_status_groups[pl->unk1].unk0[2].button & 0x100) || lbl_14_bss_60 >= (lbl_80198B38 * 0xB4) - (0.6 * lbl_80198B34))) {
                lbl_14_bss_8 |= 8;
                lbl_14_bss_64 = 0.5 * lbl_80198B34;
                if (fn_1_B2FB0() != 0) {
                    fade_screen_to_color(0x101, 0, (s32) (0.5 * lbl_80198B34));
                }
            }
        } else if (lbl_14_bss_64 != 0) {
            lbl_14_bss_64--;
            if (lbl_14_bss_64 == 0) {
                if ((lbl_14_bss_20.unk2 & 1) && lbl_14_bss_68 < 0) {
                    fn_14_BC8(0xF);
                } else {
                    fn_14_BC8(0xD);
                }
            }
        }
    }
    lbl_14_data_170[lbl_14_bss_0]();
    fn_14_BF8();
    lbl_1_bss_D4334[0] += BOAT_TICKS(lbl_14_bss_30[0]);
}

void fn_14_AD0(void)
{
    fn_14_3418();
}

void fn_14_AF0(void)
{
    if (lbl_14_bss_18 != 0) {
        OSFreeToHeap(currentHeap, lbl_14_bss_18);
        lbl_14_bss_18 = 0;
    }
    dest_all_events();
    camera_setup_singleplayer_viewport();
    fn_14_13B94();
    fn_14_23E2C();
    fn_14_213D4();
    fn_14_15834();
    fn_14_186A8();
    call_free_bmp_by_id(0xE);
    free_minigame_gma_tpl();
    g_something_calls_sndPopGroup();
    fn_1_36140();
    unload_stage();
    fn_14_E24();
    bg_heap = -1;
    fn_1_CCAA0(lbl_1_bss_A87E0, 0x4000, 0.0f, 0.0f, 400.0f);
}

void fn_14_BC8(s16 arg0)
{
    lbl_14_data_1B4 = arg0;
    lbl_14_bss_4 = 0;
}

s16 fn_14_BE8(void)
{
    return lbl_14_bss_0;
}

void fn_14_BF8(void)
{
    lbl_14_bss_C++;
    if (lbl_14_bss_10 != 0) {
        lbl_14_bss_10--;
    }
    if (lbl_14_bss_14 >= 0) {
        lbl_14_bss_14--;
        if (lbl_14_bss_14 == 0) {
            g_maybe_smth_with_music(-1, 0x64);
        }
    }
    if ((u32) (lbl_14_bss_8 & 1) != 0 && lbl_14_bss_6C != 0 && (u32) (dip_switches & 0x20) == 0) {
        lbl_14_bss_6C--;
    }
}

s32 fn_14_CB8(const void *arg0, const void *arg1)
{
    struct BoatPlayer *a = *(struct BoatPlayer **) arg0;
    struct BoatPlayer *b = *(struct BoatPlayer **) arg1;
    struct Ball *ballA = &balls[a->unk0];
    struct Ball *ballB = &balls[b->unk0];

    if (ballA->status != 2 && ballB->status != 2) {
        return a->unk0 > b->unk0 ? 1 : -1;
    }
    if (ballA->status != 2 && ballB->status == 2) {
        return 1;
    }
    if (a->unk5 == b->unk5) {
        return a->unk0 > b->unk0 ? 1 : -1;
    }
    if (a->unk5 < b->unk5) {
        return 1;
    }
    return -1;
}

void fn_14_DB0(void)
{
    s16 i;

    for (i = 0; i < lbl_14_bss_24; i++) {
        g_load_mal_files_from_disc(lbl_14_bss_28[i], lbl_14_bss_2C[i], 1, 0x12);
    }
}

void fn_14_E24(void)
{
    s16 i;

    for (i = 0; i < lbl_14_bss_24; i++) {
        g_something_freeing_chara_heap_4(lbl_14_bss_28[i], lbl_14_bss_2C[i], 1);
    }
}

void g_load_boat(void)
{
    struct Ball *ball;
    s16 i;

    lbl_14_bss_C = 0;
    fn_14_128E4(lbl_14_bss_1C);
    world_theme.backdropColor = (GXColor){0x41, 0x87, 0x41, 0xFF};
    lbl_14_bss_8 &= ~1;
    lbl_14_bss_8 &= ~4;
    if (lbl_14_bss_20.unk2 & 0x10) {
        fn_14_153B4(lbl_14_bss_25);
        lbl_14_bss_20.unk1 = lbl_1_data_65970[lbl_14_bss_25];
        lbl_14_bss_25++;
        if (lbl_14_bss_25 < 3) {
            queue_stage_load(fn_14_155F0(lbl_14_bss_25));
        } else {
            for (i = 0; i < lbl_14_bss_24; i++) {
                preload_ape(lbl_14_bss_28[i], 0, 1, 0x1A);
            }
        }
    } else {
        fn_14_153B4(lbl_14_bss_20.unk0);
        load_commend_stage();
        for (i = 0; i < lbl_14_bss_24; i++) {
            preload_ape(lbl_14_bss_28[i], 0, 1, 0x1A);
        }
        if (lbl_14_bss_20.unk2 & 1) {
            preload_ape(lbl_14_bss_28[0], 1, 0, 0);
            queue_stage_load(0xC8);
        }
    }
    if (lbl_14_bss_20.unk0 == 2) {
        lbl_14_bss_2A7C = (sptr) fn_14_11CE8;
        lbl_14_bss_2A78 = (sptr) fn_14_11F0C;
    } else {
        lbl_14_bss_2A7C = (sptr) fn_14_115F4;
        lbl_14_bss_2A78 = (sptr) fn_14_11818;
    }
    lbl_14_bss_70 = stagedef->extraData;
    if (stagedef->extraData->listCount >= 5) {
        lbl_14_bss_74 = (u8 *) stagedef->extraData->list + 0x48;
        lbl_14_bss_78 = (u8 *) stagedef->extraData->list + 0x60;
    } else {
        lbl_14_bss_74 = lbl_14_bss_70;
        lbl_14_bss_78 = lbl_14_bss_70;
    }
    if (stagedef->extraData->listCount >= 6) {
        lbl_14_bss_7C = (u8 *) stagedef->extraData->list + 0x78;
        lbl_14_bss_80 = stagedef->extraData->listCount - 5;
    } else {
        lbl_14_bss_7C = lbl_14_bss_70;
        lbl_14_bss_80 = 1;
    }
    fn_14_33E8(lbl_14_bss_20.unk0);
    lbl_1_bss_D43B0 = lbl_14_bss_2AE8->unk18;
    event_init(0);
    event_init(2);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x14);
    event_init(0xB);
    fn_1_37564(1);
    g_some_frame_counter.flags |= 4;
    if (modeCtrl.unk30 <= 1) {
        event_init(0x15);
        g_init_rendefc_for_stage();
    }
    fn_14_E2C4();
    lbl_1_bss_70E94 = fn_14_78F0;
    for (i = 0; i < 8; i++) {
        lbl_14_bss_84[i] = -1;
        lbl_14_bss_8C[i] = -1;
    }
    i = 0;
    ball = balls;
    while (i < lbl_14_bss_24) {
        create_stage_player((u8) i, 0x21, lbl_14_bss_28[i], (u8) i, lbl_14_bss_2C[i], fn_14_0, 1);
        switch (modeCtrl.unk30) {
        case 1:
            balls[i].unk179 = 0;
            break;
        case 2:
            balls[i].unk179 = 1;
            break;
        case 3:
        case 4:
            balls[i].unk179 = 2;
            break;
        }
        lbl_14_bss_84[i] = balls[i].ape->charaId;
        if (i >= modeCtrl.unk30) {
            ball->flags |= 0x80000000000LL;
            lbl_14_bss_84[i] |= 0x40;
        }
        fn_14_78A8(&balls[i], 0);
        i++;
        ball++;
    }
    if (modeCtrl.unk30 >= 3) {
        for (i = 0; i < lbl_14_bss_24; i++) {
            balls[i].unk179 = 2;
        }
    }
    for (i = 0; i < modeCtrl.unk30; i++) {
        enable_camera(i);
        cameras[i].mode = 0x4D;
        cameras[i].unk1F = 0;
    }
    if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == 3) {
        lbl_14_bss_8 |= 0x20;
        enable_camera(3);
        cameras[3].mode = 0x4D;
        cameras[3].unk1F = 0xC;
        fn_14_20C24();
        cameras[3].flags |= 0x40;
    }
    fn_14_156C0();
    fn_14_12C7C();
    fn_14_239C0();
    fn_14_21098();
    fn_14_22A9C();
    fn_14_234F4();
    lbl_14_bss_6C = lbl_14_bss_2AEC;
    if (modeCtrl.unk30 == 1) {
        fn_14_1A918();
    } else {
        fn_14_1AC0C();
    }
    if (!(lbl_14_bss_20.unk2 & 0x10) && modeCtrl.unk30 == 1) {
        lbl_14_bss_60 = 0;
        lbl_14_bss_8 |= 2;
    }
    lbl_14_bss_68 = -1;
    lbl_14_bss_94[0].chara = balls[0].ape->charaId;
    lbl_14_bss_94[0].time[0] = 0x63;
    lbl_14_bss_94[0].time[1] = 0x3B;
    lbl_14_bss_94[0].time[2] = 0x63;
    lbl_14_bss_94[1] = lbl_14_bss_94[0];
    lbl_1_bss_D432C = -1;
    lbl_1_bss_D432C &= ~0x44000;
    g_maybe_smth_with_music(-1, 0x64);
    lbl_14_bss_14 = -1;
    g_SoftStreamStart_with_some_defaults_2(lbl_14_bss_2AE8->unk6);
    fade_screen_to_color(0x100, 0, (s32) (0.5 * lbl_80198B34));
    fn_14_BC8(2);
}

void fn_14_16C0(void)
{
    s16 i;
    struct Ball *ball;
    struct BoatPlayer *pl;

    lbl_14_bss_8 |= 4;
    lbl_14_bss_60 = 0;
    event_dest(0xB);
    lbl_14_bss_C = 0;
    fn_14_128E4(lbl_14_bss_1C);
    dest_all_sprites();
    for (i = 0; i < 8; i++) {
        lbl_14_bss_29E4[i] = lbl_14_bss_2988[i];
    }
    i = 0;
    ball = balls;
    pl = lbl_14_bss_208;
    while (i < lbl_14_bss_24) {
        fn_1_113768(ball->ape, 0.0f);
        fn_1_11360C(ball->ape, 0.0f);
        pl->unk3 = pl->unk2;
        pl->unk10 = pl->unk8;
        *(Vec *) &pl->unk2C0 = *(Vec *) &pl->unk2A8;
        *(Vec *) &pl->unk2CC = *(Vec *) &pl->unk2B4;
        fn_14_78A8(ball, 0);
        i++;
        ball++;
        pl++;
    }
    cameras[0].unk1F = 0;
    fn_14_1A918();
    lbl_14_bss_8 &= ~1;
    fn_14_156C0();
    fn_14_12F3C();
    fn_14_239C0();
    fn_14_21098();
    fn_14_22A9C();
    fn_14_234F4();
    lbl_14_bss_6C = lbl_14_bss_2AEC;
    fn_14_1F358(470.0f, 430.0f);
    fade_screen_to_color(0x100, 0, (s32) (0.5 * lbl_80198B34));
    fn_1_37564(1);
    fn_14_BC8(2);
}

void fn_14_18B4(void)
{
    lbl_14_bss_10 = lbl_80198B38 * 3;
    if ((u32) (lbl_14_bss_8 & 4) != 0) {
        fn_14_E3E4();
    } else if ((u32) (lbl_14_bss_8 & 0x20) != 0) {
        fn_14_E638();
    }
    fn_14_BC8(3);
    fn_14_193C();
}

void fn_14_193C(void)
{
    if (lbl_14_bss_10 == 2.5 * lbl_80198B34) {
        fn_14_19618(2.1f);
        if (lbl_14_bss_20.unk2 & 0x10) {
            s16 sounds[3] = {0x268, 0x269, 0x26B};

            call_SoundReqID_arg_0(sounds[lbl_14_bss_25 - 1]);
        }
    }
    fn_14_13144();
    fn_14_23B78();
    fn_14_211EC();
    fn_14_22B80();
    if (lbl_14_bss_10 == 0) {
        fn_14_BC8(4);
    }
}

void fn_14_1A24(void)
{
    lbl_14_bss_10 = lbl_80198B38 * 3;
    fn_14_1A5C8();
    fn_14_BC8(5);
    fn_14_1A84();
}

void fn_14_1A84(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    s16 i;

    fn_14_13144();
    fn_14_23B78();
    fn_14_211EC();
    fn_14_22B80();
    if (lbl_14_bss_10 == (s32) ((10.0 * lbl_80198B34) / 60.0)) {
        create_go_sprite(lbl_80198B38);
    }
    if (lbl_14_bss_10 == 0.1 * lbl_80198B34) {
        i = 0;
        ball = balls;
        pl = lbl_14_bss_208;
        while (i < 8) {
            if (ball->status == 2) {
                pl->unk8 &= ~4;
                fn_14_789C(ball, 2);
            }
            i++;
            ball++;
            pl++;
        }
    }
    if (lbl_14_bss_10 == 0) {
        if (modeCtrl.unk30 == 1) {
            lbl_14_bss_8 |= 1;
        }
        if (lbl_14_bss_20.unk0 == 2) {
            fn_14_12B24(0x273, &(Vec){65.171f, -56.97f, -213.22f});
            fn_14_12B24(0x274, &(Vec){4.583f, -88.29f, -308.583f});
        }
        call_SoundReqID_arg_0(0x26F);
        fn_1_37564(0x46);
        fn_14_BC8(6);
    }
}

void fn_14_1C54(void)
{
    struct Ball *ball;
    struct BoatPlayer *pl;
    s16 i;

    lbl_14_bss_30[1] = OSGetTick();
    fn_14_13144();
    lbl_1_bss_D4334[1] += BOAT_TICKS(lbl_14_bss_30[1]);
    lbl_14_bss_30[2] = OSGetTick();
    fn_14_23B78();
    lbl_1_bss_D4334[2] += BOAT_TICKS(lbl_14_bss_30[2]);
    lbl_14_bss_30[3] = OSGetTick();
    fn_14_211EC();
    lbl_1_bss_D4334[3] += BOAT_TICKS(lbl_14_bss_30[3]);
    lbl_14_bss_30[4] = OSGetTick();
    fn_14_22B80();
    lbl_1_bss_D4334[4] += BOAT_TICKS(lbl_14_bss_30[4]);
    lbl_14_bss_30[5] = OSGetTick();
    if (lbl_14_bss_20.unk0 == 2) {
        fn_14_12BF4(0x273, &(Vec){65.171f, -56.97f, -213.22f});
        fn_14_12BF4(0x274, &(Vec){4.583f, -88.29f, -308.583f});
    }
    if ((u32) (lbl_14_bss_8 & 4) != 0 && (lbl_14_bss_208[0].unk8 & 8) == 0) {
        fn_14_E50C();
    } else if ((u32) (lbl_14_bss_8 & 0x20) != 0) {
        fn_14_E760();
    }
    if ((u32) (lbl_14_bss_8 & 1) != 0) {
        struct BoatPlayer *p0 = lbl_14_bss_208;

        if ((p0->unk8 & 0x10) != 0 || (p0->unk8 & 0x8000) != 0) {
            lbl_14_bss_6C += lbl_14_bss_2AEE;
            if (lbl_14_bss_6C > lbl_80198B38 * 0x258) {
                lbl_14_bss_6C = lbl_80198B38 * 0x258;
            }
        } else if ((p0->unk8 & 8) != 0) {
            lbl_14_bss_8 &= ~1;
        }
        if (lbl_14_bss_6C == 0) {
            lbl_14_bss_4 = 1;
        }
    }
    if (modeCtrl.unk30 == 1) {
        struct BoatPlayer *p0 = lbl_14_bss_208;

        if ((p0->unk8 & 0x10) != 0) {
            if (p0->unk7 == lbl_14_bss_20.unk1) {
                fn_14_19F48();
            } else {
                fn_14_19CDC();
            }
        } else if ((p0->unk8 & 0x8000) != 0) {
            fn_14_19CDC();
        }
    } else {
        i = 0;
        ball = balls;
        pl = lbl_14_bss_208;
        while (i < lbl_14_bss_24) {
            if (ball->status == 2 && !(ball->flags & 0x80000000000LL) && (pl->unk8 & 0x10) != 0) {
                fn_14_1A1BC(i);
            }
            i++;
            ball++;
            pl++;
        }
    }
    if (lbl_14_bss_20.unk2 & 4) {
        fn_14_159CC();
    }
    if (lbl_14_bss_4 == 0) {
        if (is_race_over()) {
            lbl_14_bss_10 = lbl_80198B38 * 5;
            lbl_14_bss_4 = 1;
            if ((lbl_14_bss_20.unk2 & 1) && (u32) (lbl_14_bss_8 & 4) == 0 && (lbl_14_bss_29A8->unk8 & 8) != 0) {
                struct BoatPlayer *p0 = lbl_14_bss_208;
                s16 j;

                for (j = 0; j < 3; j++) {
                    lbl_14_bss_94[0].time[j] = ((u8 *) &p0->unk2A8.unk8)[j] & 0xFF;
                    lbl_14_bss_94[1].time[j] = ((u8 *) &p0->unk2B4.unk8)[j] & 0xFF;
                }
                lbl_14_bss_68 = fn_1_11E1C0(lbl_14_bss_20.unk0 + 0x18, &lbl_14_bss_94[0]);
            }
        }
    } else if (lbl_14_bss_10 == 0) {
        struct Ball *b = balls;
        s16 j;

        pl = lbl_14_bss_208;
        j = 0;
        while (j < lbl_14_bss_24) {
            if (b->status == 2 && (pl->unk8 & 8) == 0) {
                fn_14_789C(b, 0xD);
            }
            j++;
            b++;
            pl++;
        }
        if (lbl_14_bss_20.unk2 & 0x10) {
            fn_14_BC8(9);
        } else if (lbl_14_bss_20.unk2 & 1) {
            fn_14_BC8(0xB);
        } else {
            fn_14_BC8(7);
        }
    }
    lbl_1_bss_D4334[5] += BOAT_TICKS(lbl_14_bss_30[5]);
}

void fn_14_23C0(void)
{
    lbl_14_bss_10 = lbl_80198B38 * 5;
    if (lbl_14_bss_20.unk0 == 2) {
        SoundOffID(0x273);
        SoundOffID(0x274);
    }
    fn_14_BC8(8);
    fn_14_2438();
}

void fn_14_2438(void)
{
    struct BoatPlayer *pl;
    s16 count;
    s16 i;

    fn_14_13144();
    fn_14_23B78();
    fn_14_211EC();
    fn_14_22B80();
    if ((u32) (lbl_14_bss_8 & 0x20) != 0) {
        fn_14_E760();
    }
    if (lbl_14_bss_10 == (s32) (0.5 * lbl_80198B34) && fn_1_B2FB0() != 0) {
        fade_screen_to_color(0x101, 0, (s32) (0.5 * lbl_80198B34));
    }
    if (lbl_14_bss_10 == 0) {
        if ((u32) (lbl_14_bss_8 & 4) == 0) {
            count = 0;
            i = 0;
            pl = lbl_14_bss_208;
            while (i < lbl_14_bss_24) {
                if ((pl->unk8 & 8) != 0) {
                    lbl_14_bss_8C[i] = pl->unk2 - 1;
                } else {
                    lbl_14_bss_8C[i] = lbl_14_bss_24 - 1;
                    count++;
                }
                i++;
                pl++;
            }
            if (count == lbl_14_bss_24) {
                lbl_14_bss_8 |= 0x10;
            }
            if (modeCtrl.unk30 == 1 && (lbl_14_bss_208[0].unk8 & 8) != 0 && !(lbl_14_bss_20.unk2 & 0x10)) {
                fn_14_BC8(1);
                return;
            }
        }
        fn_14_BC8(0xD);
    }
}

static inline void boat_assign_ranks(void *ranks)
{
    s16 i;
    struct BoatPlayer **list = ranks;

    for (i = 0; i < 8; i++) {
        lbl_14_bss_29C4[i] = &lbl_14_bss_208[i];
    }
    qsort(list, 8, sizeof(*list), fn_14_CB8);
    list[0]->unk6 = 1;
    for (i = 1; i < lbl_14_bss_24; i++) {
        if (list[i]->unk5 == list[i - 1]->unk5) {
            list[i]->unk6 = list[i - 1]->unk6;
        } else {
            list[i]->unk6 = i + 1;
        }
    }
    for (i = 0; i < lbl_14_bss_24; i++) {
        if (list[i]->unk5 == 0) {
            list[i]->unk6 = lbl_14_bss_24;
        }
    }
}

void fn_14_2620(void)
{
    s16 i;
    struct Ball *ball;
    struct BoatPlayer *pl;

    i = 0;
    ball = balls;
    pl = lbl_14_bss_208;
    while (i < lbl_14_bss_24) {
        if ((ball->flags & 0x80000000000LL) && (pl->unk8 & 0x20) != 0) {
            pl->unk5 += lbl_14_data_1B8[lbl_14_bss_24 - pl->unk2];
        }
        i++;
        ball++;
        pl++;
    }
    boat_assign_ranks(lbl_14_bss_29C4);
    if (lbl_14_bss_25 < 3) {
        lbl_14_bss_10 = lbl_80198B38 * 8;
    } else {
        lbl_14_bss_10 = lbl_80198B38 * 4;
    }
    fn_14_BC8(0xA);
    fn_14_2890();
}

void fn_14_2890(void)
{
    struct BoatPlayer *pl;
    s16 i;

    fn_14_13144();
    fn_14_23B78();
    fn_14_211EC();
    fn_14_22B80();
    if (lbl_14_bss_10 == lbl_80198B38 * 6) {
        if (lbl_14_bss_25 < 3) {
            fn_14_1EAE0();
        }
    } else if (lbl_14_bss_10 == lbl_80198B38) {
        g_change_music_volume(lbl_14_bss_2AE8->unk6, lbl_80198B38, 0);
    } else if (lbl_14_bss_10 == (s32) (0.5 * lbl_80198B34)) {
        fade_screen_to_color(0x101, 0, (s32) (0.5 * lbl_80198B34));
    } else if (lbl_14_bss_10 == 0) {
        if (lbl_14_bss_25 >= 3) {
            pl = lbl_14_bss_208;
            i = 0;
            while (i < lbl_14_bss_24) {
                lbl_14_bss_8C[i] = pl->unk6 - 1;
                i++;
                pl++;
            }
            fn_14_BC8(0xD);
        } else {
            fn_14_BC8(0);
            fn_14_13B94();
            fn_14_23E2C();
            fn_14_213D4();
        }
        dest_all_events();
    }
}

void fn_14_2A48(void)
{
    lbl_14_bss_10 = lbl_80198B38 * 5;
    fn_14_BC8(0xC);
    fn_14_2AA4();
}

void fn_14_2AA4(void)
{
    fn_14_13144();
    fn_14_23B78();
    fn_14_211EC();
    fn_14_22B80();
    if (lbl_14_bss_10 == (s32) (0.5 * lbl_80198B34)) {
        fade_screen_to_color(0x101, 0, (s32) (0.5 * lbl_80198B34));
    }
    if (lbl_14_bss_10 == 0) {
        if ((lbl_14_bss_208[0].unk8 & 8) != 0 && (u32) (lbl_14_bss_8 & 4) == 0) {
            lbl_14_bss_8C[0] = lbl_14_bss_68;
            fn_14_BC8(1);
            return;
        }
        if (lbl_14_bss_68 < 0) {
            fn_14_BC8(0xF);
            return;
        }
        fn_14_BC8(0xD);
    }
}

void fn_14_2BE4(void)
{
    s16 i;

    fn_14_E24();
    if ((u32) (lbl_14_bss_8 & 4) != 0) {
        struct BoatPlayer *pl;

        for (i = 0; i < 8; i++) {
            lbl_14_bss_2988[i] = lbl_14_bss_29E4[i];
        }
        i = 0;
        pl = lbl_14_bss_208;
        while (i < lbl_14_bss_24) {
            pl->unk2 = pl->unk3;
            pl->unk8 = pl->unk10;
            *(Vec *) &pl->unk2A8 = *(Vec *) &pl->unk2C0;
            *(Vec *) &pl->unk2B4 = *(Vec *) &pl->unk2CC;
            i++;
            pl++;
        }
        if (lbl_14_bss_18 != 0) {
            OSFreeToHeap(currentHeap, lbl_14_bss_18);
            lbl_14_bss_18 = 0;
        }
        lbl_14_bss_8 &= ~0xC;
    }
    dest_all_events();
    fn_14_23E2C();
    fn_14_213D4();
    camera_setup_singleplayer_viewport();
    event_init(0x10);
    event_init(0xE);
    event_init(0);
    event_init(0x11);
    event_init(0x13);
    event_init(0xC);
    fn_1_37564(1);
    fn_1_E4708(lbl_14_bss_84, lbl_14_bss_8C);
    if (!(lbl_14_bss_20.unk2 & 1)) {
        for (i = 0; i < lbl_14_bss_24; i++) {
            if (lbl_14_bss_8C[i] > lbl_14_bss_24 - 1) {
                lbl_14_bss_8C[i] = lbl_14_bss_24 - 1;
            }
        }
    }
    enable_cameras_for_active_players();
    camera_set_state(0x46);
    mtxa_from_translate_xyz(0.0f, 0.5f, -10.0f);
    mtxa_rotate_y(-0x8000);
    fn_1_E6760(mtxa);
    lbl_1_bss_D432C = -1;
    lbl_1_bss_D432C &= 0xFFF877FD;
    if (!(lbl_14_bss_20.unk2 & 1) && (u32) (lbl_14_bss_8 & 0x10) == 0) {
        fn_14_1F64C();
    }
    if (!(lbl_14_bss_20.unk2 & 1)) {
        g_load_stage_for_menu_bg(6, 1);
    }
    fade_screen_to_color(0x100, 0, (s32) (0.5 * lbl_80198B38));
    lbl_14_bss_10 = lbl_80198B38;
    fn_14_BC8(0xE);
}

void fn_14_2F38(void)
{
    fn_14_13144();
    if (lbl_14_bss_10 == 0) {
        if (lbl_14_bss_4 == 0) {
            if (fn_1_E5DC8() != 0 && ((merged_digital_inputs[2] & 0x100) || (merged_digital_inputs[2] & 0x1000))) {
                fade_screen_to_color(0x101, 0, lbl_80198B38);
                lbl_14_bss_10 = lbl_80198B38;
                g_change_music_volume(lbl_14_bss_2AE8->unk6, lbl_80198B38, 0);
                if (lbl_14_bss_20.unk2 & 1) {
                    call_SoundReqID_arg_0(0xD1);
                }
                lbl_14_bss_4 = 1;
            }
        } else {
            dest_all_events();
            lbl_1_bss_D432C = 0;
            if (lbl_14_bss_20.unk2 & 1) {
                fn_14_BC8(0xF);
                return;
            }
            g_return_to_sel_ngc(modeCtrl.gameType);
        }
    }
}

void fn_14_3084(void)
{
    fn_14_13B94();
    fn_14_23E2C();
    fn_14_213D4();
    dest_all_events();
    event_init(0x11);
    event_init(0x13);
    event_init(0);
    event_init(0x14);
    event_init(0x10);
    lbl_1_bss_D432C = -1;
    lbl_1_bss_D432C &= 0xFFFC37F9;
    fn_1_37564(1);
    if (fn_1_11DF64(lbl_14_bss_20.unk0 + 0x18, &lbl_14_bss_94[0], 1) != 0) {
        fn_1_11DE54(lbl_14_bss_20.unk0 + 0x18, 1);
    } else {
        fn_1_11DE54(lbl_14_bss_20.unk0 + 0x18, 0);
    }
    g_load_stage_for_menu_bg(6, 1);
    fade_screen_to_color(0x100, 0xFFFFFF, lbl_80198B38);
    fn_14_BC8(0x10);
}

void fn_14_31A4(void)
{
    s16 i;

    if (lbl_14_bss_4 == 0) {
        if (fn_1_11E3A4() == 0) {
            for (i = 0; i < modeCtrl.unk30; i++) {
                if (pad_status_groups[player_pad_map[i]].unk0[2].button & 0x100) {
                    lbl_14_bss_4 = 1;
                }
            }
        }
        if (lbl_14_bss_4 != 0) {
            fade_screen_to_color(0x101, 0, lbl_80198B38);
            lbl_14_bss_10 = lbl_80198B38;
            g_fade_track_volume(lbl_80198B38, 2);
            call_SoundReqID_arg_0(0xD1);
            lbl_14_bss_4 = 1;
        }
    } else if (lbl_14_bss_10 == 0) {
        dest_all_events();
        lbl_1_bss_D432C = 0;
        fn_1_11C5E4();
        g_return_to_sel_ngc(modeCtrl.gameType);
    }
}

