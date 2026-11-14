/* CPU/GPU performance meter (the SDK's PERF library) and profiling timers. */
#include "types.h"
#include "gx.h"
#include "dip.h"
#include "input.h"
#include "libc.h"
#include "futsal_data.h"

u32 monkey_flags = 0xFFFFFFFF;

u32 g_profile_timer_start_times[9] __attribute__((aligned(32)));
void *perfDispLists[2];
u32 currDispList;
u32 g_enable_perf;
u32 perfDispListSizes[2];
s8 zTrigTimer;
/* Per-frame draw statistics (SMB1's perfInfo); the first ten words are cleared each frame. */
struct PerfInfo
{
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 drawCount;  /* g_some_draw_perf_var */
    u32 unk10;
    u32 timeElapsed;
    u32 time;
    u32 unk1C;
    u32 totalElapsed;
    u32 unk24;
    u32 unk28;
} perfInfo;

/* Profiling timers read and written by other modules. */
u32 g_some_perf_timer1;
u32 g_some_perf_timer2;
u32 g_some_perf_timer3;
u32 g_some_perf_timer4;
u32 g_some_perf_timer5;
u32 g_some_perf_timer6;
u32 g_some_perf_timer7;
u32 g_some_perf_timer8;
u32 g_some_perf_timer9;
u32 g_some_perf_timer10;
u32 g_some_perf_timer11;
u32 g_some_perf_timer12;
u32 lbl_1_bss_15EFC;
u32 g_some_perf_timer14;
u32 g_some_perf_timer13;
u32 g_some_perf_timer15;
u32 g_some_perf_timer17;
u32 g_some_perf_timer18;
u32 g_some_perf_timer16[3];  /* size from the gap before the next file; maybe padding */


extern void *currentHeap;
extern s32 is_more_than_24_mib;

u32 OSGetTick(void);
void OSFreeToHeap(void *heap, void *ptr);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void *fn_1_1588(u32 size, u32 align, char *file, int line);
void PERFInit(u32 numSamples, u32 numFramesHistory, u32 numTypes, void *(*allocator)(u32),
              void (*deallocator)(void *), void (*initDraw)(void));
void PERFSetEvent(u8 id, char *name, int type);
void g_set_something_with_perf(int a);
void PERFEndFrame(void);
void PERFStopAutoSampling(void);
void PERFDumpScreen(void);
void PERFPreDraw(void);
void PERFPostDraw(void);
void PERFStartFrame(void);
void PERFStartAutoSampling(float msInterval);
void gx_begin_display_list_wrapper(void *list, u32 size);
u32 gx_end_display_list_wrapper(void);
void GXCallDisplayList(void *list, u32 size);
void g_clear_prev_GX_settings_something2(void);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void GXSetChanAmbColor(int chan, GXColor color);
void GXSetChanMatColor(int chan, GXColor color);
void g_something_with_raster_state(int a);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumChans_cached(u8 n);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);

void perf_init_timer(int timerId)
{
    g_profile_timer_start_times[timerId] = OSGetTick();
}

u32 perf_stop_timer(volatile int timerId)
{
    /* OSTicksToMicroseconds */
    return ((OSGetTick() - g_profile_timer_start_times[timerId]) * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
}

void perf_free(void *ptr)
{
    OSFreeToHeap(currentHeap, ptr);
}

void *perf_alloc(u32 size)
{
    return alloc_from_heap_or_panic(currentHeap, size, "perf.c", 74);
}

void perf_init_draw(void)
{
    GXColor matColor = { 255, 255, 255, 255 };
    GXColor ambColor = { 0, 0, 0, 255 };

    opti_GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetChanAmbColor(4, ambColor);
    GXSetChanMatColor(4, matColor);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 3, 1);
    g_something_with_raster_state(0);
    GXSetZCompLoc_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumChans_cached(1);
    g_GXSetTevOrder_wrapper(0, 0, 0, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 1);
}

void g_init_perf_profiling(void)
{
    GXColor unused = { 200, 200, 0, 200 };

    PERFInit(100, 1, 3, perf_alloc, perf_free, perf_init_draw);
    PERFSetEvent(0, "MODE", 0);
    PERFSetEvent(1, "EVENT", 0);
    PERFSetEvent(2, "DISP", 1);
    g_set_something_with_perf(1);
    if (!(dip_switches & DIP_ONLY24MB) && is_more_than_24_mib)
    {
        perfDispLists[0] = fn_1_1588(0x10000, 32, "perf.c", 116);
        perfDispLists[1] = fn_1_1588(0x10000, 32, "perf.c", 117);
    }
    currDispList = 0;
    g_enable_perf = 0;
}

void g_perf_finish_frame(void)
{
    if (g_enable_perf)
    {
        PERFEndFrame();
        PERFStopAutoSampling();
        if (perfDispLists[currDispList & 1] != NULL)
        {
            gx_begin_display_list_wrapper(perfDispLists[currDispList & 1], 0x10000);
            PERFDumpScreen();
            perfDispListSizes[currDispList & 1] = gx_end_display_list_wrapper();
        }
    }
    if (currDispList != 0 && g_enable_perf && perfDispLists[currDispList & 1] != NULL)
    {
        PERFPreDraw();
        GXCallDisplayList(perfDispLists[currDispList & 1], perfDispListSizes[currDispList & 1]);
        PERFPostDraw();
        g_clear_prev_GX_settings_something2();
        GXSetZCompLoc_cached(0);
    }
    currDispList++;
}

/* With DIP_DEBUG, Z shows the meter: toggled by a press with DIP_PERF_ALWAYS, otherwise
 * while held for more than 30 frames. */
void g_perf_start_frame(void)
{
    if (dip_switches & DIP_DEBUG)
    {
        if (dip_switches & DIP_PERF_ALWAYS)
        {
            if (pad_status_groups[0].unk0[2].button & PAD_TRIGGER_Z)
            {
                g_enable_perf = (u8)(g_enable_perf ? FALSE : TRUE);
                printf("change\n");
            }
        }
        else if (pad_status_groups[0].unk0[0].button & PAD_TRIGGER_Z)
        {
            if (zTrigTimer < 127)
                zTrigTimer++;
            if (zTrigTimer > 30)
                g_enable_perf = TRUE;
        }
        else
        {
            zTrigTimer = 0;
            g_enable_perf = FALSE;
        }
    }
    else
    {
        zTrigTimer = 0;
        g_enable_perf = FALSE;
    }
    if (g_enable_perf)
    {
        PERFStartFrame();
        PERFStartAutoSampling(1.0f);
    }
}

void g_reset_draw_perf_counters(void)
{
    memset(&perfInfo, 0, 10 * sizeof(u32));
}

/* Unreferenced (stripped at link); the timers are unused here, so this fixes their order. */
void perf_force_bss_order(void **p)
{
    p[0] = &g_some_perf_timer1;
    p[1] = &g_some_perf_timer2;
    p[2] = &g_some_perf_timer3;
    p[3] = &g_some_perf_timer4;
    p[4] = &g_some_perf_timer5;
    p[5] = &g_some_perf_timer6;
    p[6] = &g_some_perf_timer7;
    p[7] = &g_some_perf_timer8;
    p[8] = &g_some_perf_timer9;
    p[9] = &g_some_perf_timer10;
    p[10] = &g_some_perf_timer11;
    p[11] = &g_some_perf_timer12;
    p[12] = &lbl_1_bss_15EFC;
    p[13] = &g_some_perf_timer14;
    p[14] = &g_some_perf_timer13;
    p[15] = &g_some_perf_timer15;
    p[16] = &g_some_perf_timer17;
    p[17] = &g_some_perf_timer18;
    p[18] = g_some_perf_timer16;
}
