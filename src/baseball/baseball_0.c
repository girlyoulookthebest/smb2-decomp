#include "baseball.h"
#include "event.h"

struct BBMain
{
    /*0x00*/ s32 unk0;
    /*0x04*/ u8 filler4[0x14];
    /*0x18*/ u8 unk18;
    /*0x19*/ u8 unk19;
    /*0x1A*/ u8 filler1A[6];
    /*0x20*/ u32 unk20;
    /*0x24*/ u8 filler24[0x2B4];
};  /* size = 0x2D8 */

char *lbl_18_data_18 = "//mkb2.main_loop.rel";

struct BBMain lbl_18_bss_0;
int (*lbl_18_bss_2D8[2])(void);
struct BBCam lbl_18_bss_2E0;

void _prolog(void)
{
    g_very_similar_to_puts("RELOCATION ASPECT0 LOADED\n");
    fn_18_130();
    set_console_reset_callback(fn_18_1FC);
    fn_18_394();
    fn_18_39C();
    fn_18_4A8();
    g_baseball_game_loop();
    fn_18_4D0();
    fn_18_3BC();
    fn_18_398();
    fn_18_130();
}

void _epilog(void)
{
    revert_console_reset_callback();
    g_very_similar_to_puts("RELOCATION ASPECT0 PURGED\n");
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
    OSPanic("aspect0.c", 0x81, "\n");
}

void baseball_dead_strings(void *p)
{
    printf("module: %08X\n", p);
    printf("cannot OSAlloc\n");
}

void fn_18_130(void)
{
    u32 size;

    size = OSCheckHeap(main_heap);
    printf("[baseball] MainHeap %x\n", size);
    size = OSCheckHeap(stage_heap);
    printf("[baseball] StageHeap %x\n", size);
    size = OSCheckHeap(bg_heap);
    printf("[baseball] BackgroundHeap %x\n", size);
    size = OSCheckHeap(chara_heap);
    printf("[baseball] CharaHeap %x\n", size);
    size = OSCheckHeap(replay_heap);
    printf("[baseball] ReplayHeap %x\n", size);
}

s32 fn_18_1FC(void)
{
    event_vibration_dest();
    if (fn_1_F004C() != 0) {
        return 0;
    }
    if (g_GTK_get_something2() != 0) {
        if (g_DTK_get_something1() == 3) {
            return 0;
        }
        DTKFlushTracks(NULL);
    }
    return 1;
}

void fn_18_25C(void)
{
    lbl_18_bss_0.unk18 = 0;
}

void fn_18_270(void)
{

}

void fn_18_274(void)
{
    lbl_18_bss_0.unk20 |= 0x40000000;
}

s32 fn_18_28C(void)
{
    return (lbl_18_bss_0.unk20 & 0x40000000) ? 1 : 0;
}

void fn_18_2B4(void)
{
    lbl_18_bss_0.unk20 &= 0xDFFFFFFF;
    lbl_18_bss_0.unk20 &= 0xEFFFFFFF;
    lbl_18_bss_0.unk20 &= 0xF7FFFFFF;
    lbl_18_bss_0.unk20 &= 0xFDFFFFFF;
    lbl_18_bss_0.unk20 &= 0xBFFFFFFF;
    lbl_18_bss_0.unk18 = lbl_18_bss_0.unk19;
}

s32 fn_18_334(void)
{
    return lbl_18_bss_0.unk0;
}

void fn_18_344(s32 arg0)
{
    lbl_18_bss_0.unk0 = arg0;
}

void fn_18_354(void)
{
    lbl_18_bss_2D8[0] = NULL;
    lbl_18_bss_2D8[0] = get_console_reset_callback();
}

void fn_18_390(void)
{

}

void fn_18_394(void)
{

}

void fn_18_398(void)
{

}

void fn_18_39C(void)
{
    fn_80061AF0();
}

void fn_18_3BC(void)
{

}

