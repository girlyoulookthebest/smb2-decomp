/* Per-world sound hooks: looping ambience started and faded for specific worlds. */
#include "types.h"
#include "mode.h"
#include "stage.h"
#include "dip.h"

void fn_1_34828(void);
void call_SoftStreamSEReq_arg_0(int id, s8 volume, s8 pan, s8 span);
void fn_1_34B48(int id, s8 volume);

void fn_1_37320(void);
void fn_1_373AC(void);
void fn_1_3754C(void);
void fn_1_37550(void);
void fn_1_37554(void);
void fn_1_37558(void);
void fn_1_3755C(void);
void fn_1_37560(void);

struct WorldSoundHandler
{
    s8 worldTheme;
    void (*init)(void);
    void (*tick)(void);
};

struct WorldSoundHandler lbl_1_data_24138[] =
{
    { 31, fn_1_37320, fn_1_373AC },
    { 24, fn_1_3754C, fn_1_37550 },
    { 17, fn_1_37554, fn_1_37558 },
    { 25, fn_1_3755C, fn_1_37560 },
};

void (*lbl_1_bss_25C30)(void);  /* current world's sound tick */
float lbl_1_bss_25C34;  /* ambience volume */
float lbl_1_bss_25C38;  /* ambience target volume */

/* Stop streamed sound effects and start the current world's sound hooks. */
void fn_1_371C8(void)
{
    struct WorldSoundHandler *handler = lbl_1_data_24138;
    s8 i;

    fn_1_34828();
    if (sub_mode == 3 || sub_mode == 4 || sub_mode == 79 || sub_mode == 80)
        return;
    if (current_stage_id == 0)
        return;
    lbl_1_bss_25C30 = NULL;
    for (i = 0; i < sizeof(lbl_1_data_24138) / sizeof(lbl_1_data_24138[0]); i++, handler++)
    {
        if (handler->worldTheme == world_theme)
        {
            handler->init();
            lbl_1_bss_25C30 = handler->tick;
            return;
        }
    }
}

void fn_1_372BC(void)
{
    if (lbl_1_bss_25C30 != NULL)
        lbl_1_bss_25C30();
}

void fn_1_37300(void)
{
    fn_1_34828();
}

/* Never called and stripped from the game; only its constants remain. */
void world_sound_stripped_literals(float *f, s8 x)
{
    *f = x * 0.0039;
}

void fn_1_37320(void)
{
    lbl_1_bss_25C34 = 1.0f;
    lbl_1_bss_25C38 = 1.0f;
    call_SoftStreamSEReq_arg_0(0, 100, -64, 63);
    call_SoftStreamSEReq_arg_0(1, 100, 63, 63);
    fn_1_34B48(0, 1);
    fn_1_34B48(1, 1);
}

void fn_1_373AC(void)
{
    if (modeCtrl.gameType != 10)
        return;
    if (g_some_other_flags & 0xA)
        return;
    lbl_1_bss_25C34 = lbl_1_bss_25C34
                    + 0.05f * (lbl_1_bss_25C38 * ((sub_mode == 31 || sub_mode == 32 || sub_mode == 87 || sub_mode == 88)
                                                  ? 0.01f : 1.0f)
                               - lbl_1_bss_25C34);
    lbl_1_bss_25C34 = (lbl_1_bss_25C34 > 1.0f) ? lbl_1_bss_25C34 : 1.0f;
    fn_1_34B48(0, lbl_1_bss_25C34);
    fn_1_34B48(1, lbl_1_bss_25C34);
}

void fn_1_3754C(void) {}
void fn_1_37550(void) {}
void fn_1_37554(void) {}
void fn_1_37558(void) {}
void fn_1_3755C(void) {}
void fn_1_37560(void) {}

void fn_1_37564(s8 volume)
{
    lbl_1_bss_25C38 = volume;
}

void fn_1_375A4(void) {}
