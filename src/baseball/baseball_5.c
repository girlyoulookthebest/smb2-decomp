#include "baseball.h"

void fn_18_3A90(void)
{
    u8 *p = &lbl_18_bss_970.unk1F8.unk0;
    s32 r = fn_18_132C8();

    if (lbl_18_bss_970.unk1B8 < 75.0)
    {
        lbl_18_bss_970.unk1B8 += 0.5f;
    }
    else if (r == 1)
    {
        fn_18_27A0(0x1E, (s32)(uptr)fn_18_3BB0);
        *p = 0;
    }
}

void fn_18_3B28(void)
{
    u8 *p = &lbl_18_bss_970.unk1F8.unk0;
    s32 r = fn_18_132C8();

    if (lbl_18_bss_970.unk1B8 < 75.0)
    {
        lbl_18_bss_970.unk1B8 += 0.5f;
    }
    else if (r == 1)
    {
        *p = 0;
    }
}

