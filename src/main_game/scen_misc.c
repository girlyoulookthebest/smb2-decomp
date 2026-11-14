/* Story mode debug world select. */
#include "types.h"
#include "course.h"
#include "input.h"
#include "libc.h"
#include "scen.h"

struct WorldSelect
{
    /*0x00*/ int unk0;
    /*0x04*/ u8 filler4[2];
    /*0x06*/ s16 state;
    /*0x08*/ int unk8;
    /*0x0C*/ u8 fillerC[0x14 - 0xC];
}; /* size = 0x14 */

struct WorldSelect lbl_2_bss_C00;

void g_some_scenario_init_func_2(void)
{
    memset(&lbl_2_bss_C00, 0, sizeof(lbl_2_bss_C00));
}

void fn_2_10754(void)
{
}

void g_smth_with_storymode(void)
{
    int world;

    switch (lbl_2_bss_C00.state)
    {
    case 1:
        lbl_2_bss_C00.state = 2;
        lbl_2_bss_C00.unk8 = 1;
        /* fall through */
    case 2:
        lbl_2_bss_C00.state = 3;
        break;
    case 3:
        lbl_2_bss_C00.state = 4;
        /* fall through */
    case 4:
        world = scen_info.world;
        if (merged_digital_inputs[2] & PAD_BUTTON_UP)
            world--;
        if (merged_digital_inputs[2] & PAD_BUTTON_DOWN)
            world++;
        if (world < 0)
            world = 0;
        else if (world > 9)
            world = 9;
        scen_info.world = world;
        if ((merged_digital_inputs[2] & PAD_BUTTON_A) && !fn_1_A505C(world))
            lbl_2_bss_C00.state = 5;
        break;
    case 5:
        lbl_2_bss_C00.state = 6;
        lbl_2_bss_C00.unk0 = 2;
        scen_info.unk6 = 0;
        /* fall through */
    case 6:
        lbl_2_bss_C00.state = 7;
        break;
    case 7:
        lbl_2_bss_C00.state = 8;
        lbl_2_bss_C00.unk8 = 0;
        /* fall through */
    case 8:
        lbl_2_bss_C00.state = 0;
        break;
    case 0:
        break;
    }
}

void fn_2_10900(void)
{
    int unused;

    switch (scen_info.unk0)
    {
    default:
        unused = 0;
        break;
    case 8:
    case 9:
        unused = 1;
        break;
    }
}
