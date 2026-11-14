/* Frame setup, GX FIFO usage tracking and video mode selection. */
#include "types.h"
#include "gx.h"
#include "os.h"
#include "dip.h"
#include "libc.h"

s8 ntscOverscanAdjust[4] = { 16, 16, 72, 0 };
s8 palOverscanAdjust[4] = { 0, 0, 56, 0 };

s32 next_fifo_use_array_idx;
u32 last_60_gx_fifo_use_sizes[60];
u32 tick_at_gx_finish_frame;
float lbl_1_bss_7080;  /* GPU time of the last frame, in 60 Hz frames */
u32 gx_fifo_use_size;
s32 g_video_mode_change_requested;

void fn_1_E767C(void);
void gx_start_new_frame(void);
void gp_wait(void);
void g_perf_finish_frame(void);
void g_perf_start_frame(void);
u32 get_gx_fifo_use_size2(void);
void video_finish_frame(void);
u32 OSGetTick(void);
u32 OSGetProgressiveMode(void);

void g_something_with_gx_modes_and_compare(void)
{
    fn_1_E767C();
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 3, 1);
    GXSetZCompLoc_cached(0);
    GXSetAlphaCompare(4, 0, 0, 3, 255);
    next_fifo_use_array_idx = 0;
    memset(last_60_gx_fifo_use_sizes, 0, sizeof(last_60_gx_fifo_use_sizes));
}

void gx_finish_frame_and_reset_z_and_alpha_compare_modes(void)
{
    gx_start_new_frame();
    tick_at_gx_finish_frame = OSGetTick();
    fn_1_E767C();
    GXSetZMode_cached(1, 3, 1);
    GXSetZCompLoc_cached(0);
    GXSetAlphaCompare(4, 0, 0, 3, 255);
}

/* Wait for the GPU and record how long the frame took it, in 60 Hz frames. */
void g_something_with_graphics_processor2(void)
{
    u32 now;

    gp_wait();
    now = OSGetTick();
    lbl_1_bss_7080 = 60.0 * (now - tick_at_gx_finish_frame) / (*(u32 *)0x800000F8 / 4);  /* bus clock / 4 = ticks per second */
}

void g_something_with_video_this_also_finishes_a_frame(void)
{
    g_perf_finish_frame();
    if (dip_switches & 1)
    {
        gx_fifo_use_size = get_gx_fifo_use_size2();
        last_60_gx_fifo_use_sizes[next_fifo_use_array_idx] = gx_fifo_use_size;
        next_fifo_use_array_idx--;
        if (next_fifo_use_array_idx < 0)
            next_fifo_use_array_idx = 59;
    }
    video_finish_frame();
    g_perf_start_frame();
    if (g_video_mode_change_requested)
    {
        VIConfigure(current_render_mode);
        g_video_mode_change_requested = 0;
    }
}

void gp_wait_wrapper(void)
{
    gp_wait();
}

u32 get_gx_fifo_use_size(void)
{
    return gx_fifo_use_size;
}

/* Largest GX FIFO use over the last 60 frames. */
u32 get_max_gx_fifo_use_size(void)
{
    u32 max = gx_fifo_use_size;
    int i;
    u32 *size = last_60_gx_fifo_use_sizes;

    for (i = 60; i > 0; i--, size++)
    {
        if (max < *size)
            max = *size;
    }
    return max;
}

float get_gpu_frame_time(void)
{
    return lbl_1_bss_7080;
}

/* Pick the render mode for the console's TV format (shrunk by an overscan margin chosen
 * by overscanIdx) and set up the viewport and copy parameters for it. */
void set_render_mode(int overscanIdx)
{
    s8 *overscanAdjust;

    switch (VIGetTvFormat())
    {
    case 0:  /* NTSC */
        if (OSGetProgressiveMode() == 1)
            current_render_mode = &ntsc_progressive_render_mode;
        else
            current_render_mode = &ntsc_interlaced_render_mode;
        overscanAdjust = ntscOverscanAdjust;
        break;
    case 1:  /* PAL */
        current_render_mode = &pal_interlaced_render_mode;
        current_render_mode->efbHeight = 448;
        overscanAdjust = palOverscanAdjust;
        break;
    case 2:  /* MPAL */
        current_render_mode = &mpal_interlaced_render_mode;
        overscanAdjust = ntscOverscanAdjust;
        break;
    default:
        OSPanic("render.c", 175, "init_system: invalid TV format\n");
        break;
    }
    GXAdjustForOverscan(current_render_mode, &overscan_adjusted_render_mode, 0, overscanAdjust[overscanIdx]);
    current_render_mode = &overscan_adjusted_render_mode;
    GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->efbHeight, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
    GXSetDispCopySrc_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
    GXSetDispCopyDst_cached(current_render_mode->fbWidth, current_render_mode->xfbHeight);
    GXSetDispCopyYScale((float)current_render_mode->xfbHeight / (float)current_render_mode->efbHeight);
    GXSetCopyFilter(current_render_mode->aa, current_render_mode->sample_pattern, 1, current_render_mode->vfilter);
    g_video_mode_change_requested = 1;
}
