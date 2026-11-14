#include "baseball.h"

struct BBScreen *lbl_18_bss_11E8;

void fn_18_15E38(void)
{
    fn_18_15AD0();
    fn_18_15E88(0x33);
}

void fn_18_15E60(void)
{
    fn_18_15AD0();
    fn_18_15E88(0x35);
}

void fn_18_15E88(s16 arg0)
{
    g_get_sprite_work(lbl_18_bss_11E8, (u32) arg0);
}

s32 fn_18_15EBC(s16 arg0)
{
    return lbl_18_bss_11E8->unk24C[arg0] <= 0;
}

void fn_18_15EE8(void)
{
    if (lbl_18_bss_11E8 != NULL) {
        g_get_sprite_work(lbl_18_bss_11E8, 1);
    }
}

void fn_18_15F2C(void)
{
    u8 *game = (u8 *) &lbl_18_bss_970 + 0x18;
    u8 tr30;

    tr30 = fn_18_19E0();
    lbl_18_bss_11E8->unkCC[0] = game[0x9];
    lbl_18_bss_11E8->unkCC[1] = game[0xA];
    lbl_18_bss_11E8->unkCC[2] = game[0xB];
    g_get_sprite_work(lbl_18_bss_11E8, 0x1F);
    lbl_18_bss_11E8->unk9C[0] = fn_18_1818(0);
    lbl_18_bss_11E8->unk9C[1] = fn_18_1818(1);
    lbl_18_bss_11E8->unk148 = (fn_18_19B0C() - 1);
    switch ((s32) tr30) {                       /* irregular */
    case 0:
        lbl_18_bss_11E8->unk8C[0] = 1;
        if (fn_18_1A7C() == 1) {
            lbl_18_bss_11E8->unk8C[1] = 0;
        } else {
            lbl_18_bss_11E8->unk8C[1] = 2;
        }
        break;
    case 1:
        lbl_18_bss_11E8->unk8C[0] = 2;
        lbl_18_bss_11E8->unk8C[1] = 1;
        break;
    case 2:
        lbl_18_bss_11E8->unk8C[0] = 0;
        lbl_18_bss_11E8->unk8C[1] = 1;
        break;
    }
    lbl_18_bss_11E8->unk8[0][0] = game[0x12];
    lbl_18_bss_11E8->unk8[0][1] = game[0x13];
    lbl_18_bss_11E8->unkD8 = game[0xD];
    lbl_18_bss_11E8->unk80 = game[0x10];
    g_get_sprite_work(lbl_18_bss_11E8, 0x29);
    fn_18_15AD0();
    g_get_sprite_work(lbl_18_bss_11E8, 0x33);
}

void fn_18_16158(void)
{
    s32 r31;
    u8 tr31;
    u8 tr30;
    u8 tr29;
    u8 tr3;

    tr31 = fn_18_19B8();
    tr30 = fn_18_19928();
    tr29 = fn_18_198B4();
    tr3 = fn_18_19E0();
    switch ((s32) tr3) {                        /* irregular */
    case 0:
        lbl_18_bss_11E8->unk8C[0] = 1;
        if (fn_18_1A7C() == 1) {
            lbl_18_bss_11E8->unk8C[1] = 0;
        } else {
            lbl_18_bss_11E8->unk8C[1] = 2;
        }
        break;
    case 1:
        lbl_18_bss_11E8->unk8C[0] = 2;
        lbl_18_bss_11E8->unk8C[1] = 1;
        break;
    case 2:
        lbl_18_bss_11E8->unk8C[0] = 0;
        lbl_18_bss_11E8->unk8C[1] = 1;
        break;
    }
    lbl_18_bss_11E8->unk8[0][0] = fn_18_1993C();
    lbl_18_bss_11E8->unk8[0][1] = fn_18_19950();
    if (tr30 > tr31) {
        lbl_18_bss_11E8->unkDC = tr30;
    } else {
        lbl_18_bss_11E8->unkDC = tr31;
    }
    for (r31 = 1; r31 <= 0xC; r31++) {
        if (r31 <= tr30) {
            if ((r31 == tr30) && (tr29 == 1)) {
                lbl_18_bss_11E8->unk8[r31][0] = -1;
            } else {
                lbl_18_bss_11E8->unk8[r31][0] = fn_18_199A0(0, (u8) r31);
            }
        } else {
            lbl_18_bss_11E8->unk8[r31][0] = -1;
        }
    }
    for (r31 = 1; r31 <= 0xC; r31++) {
        if (r31 < (s32) tr30) {
            lbl_18_bss_11E8->unk8[r31][1] = fn_18_199A0(1, (u8) r31);
        } else {
            lbl_18_bss_11E8->unk8[r31][1] = -1;
        }
    }
    g_get_sprite_work(lbl_18_bss_11E8, 0x1D);
}

void fn_18_163FC(void)
{
    u8 tr31;
    u8 tr3;

    tr31 = fn_18_1798();
    tr3 = fn_18_19E0();
    if (tr31 == 0) {
        lbl_18_bss_11E8->unkE0 = 1;
    } else {
        lbl_18_bss_11E8->unkE0 = 0;
    }
    switch ((s32) tr3) {                        /* irregular */
    case 0:
    default:
        lbl_18_bss_11E8->unk8C[0] = 1;
        if (fn_18_1A7C() == 1) {
            lbl_18_bss_11E8->unk8C[1] = 0;
        } else {
            lbl_18_bss_11E8->unk8C[1] = 2;
        }
        break;
    case 1:
        lbl_18_bss_11E8->unk8C[0] = 2;
        lbl_18_bss_11E8->unk8C[1] = 1;
        break;
    case 2:
        lbl_18_bss_11E8->unk8C[0] = 0;
        lbl_18_bss_11E8->unk8C[1] = 1;
        break;
    }
    lbl_18_bss_11E8->unk9C[0] = fn_18_1818(0);
    lbl_18_bss_11E8->unk9C[1] = fn_18_1818(1);
    if (fn_18_16E8() == 2) {
        lbl_18_bss_11E8->unk150 = fn_18_1B80();
        g_get_sprite_work(lbl_18_bss_11E8, 0x2E);
        return;
    }
    g_get_sprite_work(lbl_18_bss_11E8, 0x2D);
}

void fn_18_165B8(void)
{
    u8 tr3;

    tr3 = fn_18_19E0();
    switch ((s32) tr3) {                        /* irregular */
    case 0:
        lbl_18_bss_11E8->unk8C[0] = 1;
        if (fn_18_1A7C() == 1) {
            lbl_18_bss_11E8->unk8C[1] = 0;
        } else {
            lbl_18_bss_11E8->unk8C[1] = 2;
        }
        break;
    case 1:
        lbl_18_bss_11E8->unk8C[0] = 2;
        lbl_18_bss_11E8->unk8C[1] = 1;
        break;
    case 2:
        lbl_18_bss_11E8->unk8C[0] = 0;
        lbl_18_bss_11E8->unk8C[1] = 1;
        break;
    }
    lbl_18_bss_11E8->unk8[0][0] = fn_18_1993C();
    lbl_18_bss_11E8->unk8[0][1] = fn_18_19950();
    g_get_sprite_work(lbl_18_bss_11E8, 0x32);
}

void fn_18_166E8(u8 arg0)
{
    u8 table[9] = {3, 6, 5, 9, 7, 8, 4, 10, 0};

    if (arg0 > 8) {
        arg0 = 0;
    }
    lbl_18_bss_11E8->unk14C = table[arg0];
}

