#include "baseball.h"
#include "input.h"

struct BBPadInput
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    s8 stickX[2];
    s8 stickY[2];
    u16 button[2];
};

struct BBInfo
{
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
    s8 unk4;
    s8 unk5;
    s8 unk6;
    u32 unk8;
};

extern struct BBInfo lbl_1_bss_172C;

u8 lbl_18_data_1F8[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};
Vec lbl_18_data_368[4] = {
    { 0.0f, 0.0f, 0.0f },
    { 9.6f, 0.0f, -10.0f },
    { 0.0f, 0.0f, -20.0f },
    { -9.6f, 0.0f, -10.0f },
};

struct BBGame lbl_18_bss_970;
struct BBSeries lbl_18_bss_BDC;

static void set_unk4(u8 v)
{
    struct BBGame *g = &lbl_18_bss_970;

    g->unk4 = v;
}

static void set_unk5(u8 v)
{
    struct BBGame *g = &lbl_18_bss_970;

    g->unk5 = v;
}

static void set_mode(s8 mode)
{
    switch (mode) {
    case 0:
    default:
        set_unk5(0);
        break;
    case 1:
        if (fn_18_16E8() == 0) {
            set_unk5(1);
        } else {
            set_unk5(2);
        }
        break;
    case 2: {
        struct BBGame *g = &lbl_18_bss_970;
        s16 r = rand() & 0x7FFF;
        u8 v;

        r = __abs(r) % 2;
        if (fn_18_16E8() == 0) {
            if (r == 0) {
                v = 0;
            } else {
                v = 1;
            }
        } else if (r == 0) {
            v = 0;
        } else {
            v = 2;
        }
        g->unk5 = v;
        break;
    }
    }
}

static void init_pads(u8 p0, u8 p1)
{
    struct BBPadInput *pad = (struct BBPadInput *)&lbl_18_bss_970.unkA;
    u8 mode = fn_18_19E0();
    s32 i;

    for (i = 0; i < 2; i++) {
        pad->stickX[i] = 0;
        pad->stickY[i] = 0;
        pad->button[i] = 0;
    }
    pad->unk0 = p0;
    pad->unk1 = p1;
    switch (mode) {
    case 0:
    default:
        pad->unk2 = pad->unk0;
        pad->unk3 = pad->unk1;
        break;
    case 1:
        pad->unk2 = pad->unk1;
        pad->unk3 = pad->unk0;
        break;
    case 2:
        pad->unk2 = pad->unk1;
        pad->unk3 = pad->unk0;
        break;
    }
}

void fn_18_16D4(u8 arg0)
{
    struct BBGame *g = &lbl_18_bss_970;

    g->unk0 = arg0;
}

u8 fn_18_16E8(void)
{
    struct BBGame *g = &lbl_18_bss_970;

    return g->unk0;
}

void fn_18_16FC(u8 arg0)
{
    struct BBGame *g = &lbl_18_bss_970;

    if (arg0 == 2) {
        s16 r = rand() & 0x7FFF;
        r = __abs(r) % 2;
        if (r == 0) {
            g->unk1 = 0;
        } else {
            g->unk1 = 1;
        }
    } else {
        g->unk1 = arg0;
    }
    lbl_18_bss_BDC.unk6 = g->unk1;
}

u8 fn_18_1798(void)
{
    struct BBGame *g = &lbl_18_bss_970;

    return g->unk1;
}

void fn_18_17AC(u8 arg0, u8 arg1)
{
    struct BBGame *g = &lbl_18_bss_970;

    g->unk8 = arg0;
    g->unk9 = arg1;
}

u8 fn_18_17C4(u8 arg0)
{
    struct BBGame *g = &lbl_18_bss_970;

    switch (arg0) {
    case 0:
        return g->unk8;
    case 1:
        return g->unk9;
    case 2:
        return g->unk9;
    default:
        return g->unk9;
    }
}

u8 fn_18_1818(u8 arg0)
{
    struct BBGame *g = &lbl_18_bss_970;
    u8 r;

    switch (fn_18_19E0()) {
    case 0:
    default:
        if (arg0 == 0) {
            r = g->unk8;
        } else {
            r = g->unk9;
        }
        break;
    case 1:
        if (arg0 == 0) {
            r = g->unk9;
        } else {
            r = g->unk8;
        }
        break;
    case 2:
        if (arg0 == 0) {
            r = g->unk9;
        } else {
            r = g->unk8;
        }
        break;
    }
    return r;
}

u8 fn_18_18CC(u8 arg0)
{
    u8 r;

    switch (fn_18_19E0()) {
    case 0:
    default:
        if (arg0 == 0) {
            r = 0;
        } else {
            r = 1;
        }
        break;
    case 1:
        if (arg0 == 0) {
            r = 1;
        } else {
            r = 0;
        }
        break;
    case 2:
        if (arg0 == 0) {
            r = 1;
        } else {
            r = 0;
        }
        break;
    }
    return r;
}

void fn_18_196C(u8 arg0)
{
    struct BBGame *g = &lbl_18_bss_970;

    g->unk3 = arg0;
}

u8 fn_18_1980(void)
{
    struct BBGame *g = &lbl_18_bss_970;

    return g->unk3;
}

void fn_18_1994(u8 arg0)
{
    struct BBGame *g = &lbl_18_bss_970;

    if (arg0 > 9) {
        arg0 = 9;
    }
    g->unk2 = arg0;
}

u8 fn_18_19B8(void)
{
    struct BBGame *g = &lbl_18_bss_970;

    return g->unk2;
}

u8 fn_18_19CC(void)
{
    struct BBGame *g = &lbl_18_bss_970;

    return g->unk4;
}

u8 fn_18_19E0(void)
{
    struct BBGame *g = &lbl_18_bss_970;

    return g->unk5;
}

void fn_18_19F4(s32 arg0)
{
    *(s32 *)lbl_18_bss_970.unk78 = arg0;
}

void fn_18_1A04(s32 arg0)
{
    lbl_18_bss_970.unk8C = arg0;
}

u32 fn_18_1A14(void)
{
    return lbl_18_bss_970.unk8C;
}

void fn_18_1A24(void *arg0, void *arg1, void *arg2, float farg0)
{
    Vec *a = arg0;
    Vec *b = arg1;
    Vec *out = arg2;

    out->x = a->x + farg0 * (b->x - a->x);
    out->y = a->y + farg0 * (b->y - a->y);
    out->z = a->z + farg0 * (b->z - a->z);
}

s32 fn_18_1A7C(void)
{
    return lbl_18_bss_970.unk0 != 0;
}

s32 fn_18_1A9C(void)
{
    u8 mode = fn_18_19E0();

    if ((fn_18_16E8() != 0 ? 1 : 0) == 1) {
        switch (mode) {
        case 0:
            if (fn_18_198C8() == 1) {
                return 1;
            }
            break;
        case 1:
            break;
        case 2:
            if (fn_18_198C8() == 0) {
                return 1;
            }
            break;
        }
    }
    return 0;
}

s32 fn_18_1B50(void)
{
    return lbl_18_bss_BDC.unk4 >= 5;
}

u8 fn_18_1B80(void)
{
    return lbl_18_bss_BDC.unk4;
}

void fn_18_1B90(void)
{
    struct BBInfo *info = &lbl_1_bss_172C;

    memset(&lbl_18_bss_970, 0, 0x26C);
    fn_18_16D4(info->unk0);
    if (lbl_18_bss_BDC.unk4 >= 2) {
        lbl_18_bss_BDC.unk4 = 5;
        lbl_18_bss_BDC.unk5++;
    } else {
        lbl_18_bss_BDC.unk5++;
        lbl_18_bss_BDC.unk4++;
    }
    if (lbl_18_bss_BDC.unk6 == 0) {
        lbl_18_bss_BDC.unk6 = 1;
        fn_18_16FC(1);
    } else {
        lbl_18_bss_BDC.unk6 = 0;
        fn_18_16FC(0);
    }
    fn_18_1994(info->unk5);
    fn_18_196C(info->unk6);
    if (info->unk8 & 1) {
        set_unk4(1);
    } else {
        set_unk4(0);
    }
    fn_18_17AC(info->unk1, lbl_18_bss_BDC.unk0[lbl_18_bss_BDC.unk5]);
    set_mode(info->unk4);
    init_pads(player_pad_map[0], player_pad_map[1]);
    fn_18_192B0();
}

s16 fn_18_1F40(s32 arg0, s32 arg1, s16 arg2, s16 arg3)
{
    s16 r;

    arg0 = (s16)(u16)(arg0 - arg1);
    if (__abs(arg0) > arg2) {
        if ((s16)arg0 & 0x8000) {
            r = -((s16)(0x10000 - arg0) / arg3);
        } else {
            r = (s16)arg0 / arg3;
        }
    } else {
        r = 0;
    }
    return r;
}

void fn_18_1FB0(void)
{
    s8 order[4];
    u8 count;
    struct BBInfo *info = &lbl_1_bss_172C;
    u8 dup;
    s32 i;
    s16 r;

    printf("info->mode      >> %d\n", info->unk0);
    printf("info->stadium   >> %d\n", info->unk3);
    printf("info->inning    >> %d\n", info->unk5);
    printf("info->com_level >> %d\n", info->unk6);
    printf("info->team[0]   >> %d\n", info->unk1);
    printf("info->team[1]   >> %d\n", info->unk2);
    fn_18_16D4(info->unk0);
    if (fn_18_16E8() == 2) {
        memset(&lbl_18_bss_BDC, 0, 7);
        lbl_18_bss_BDC.unk4 = 0;
        memset(order, -1, 4);
        count = 0;
        do {
            r = rand() & 0x7FFF;
            r = __abs(r) % 4;
            dup = 0;
            for (i = 0; i <= count; i++) {
                if (order[i] == r) {
                    dup = 1;
                    break;
                }
            }
            if (dup == 0) {
                order[count] = r;
                count++;
            }
        } while (count < 4);
        for (i = 0; i < 4; i++) {
            lbl_18_bss_BDC.unk0[i] = order[i];
        }
    }
    fn_18_16FC(info->unk3);
    fn_18_1994(info->unk5);
    fn_18_196C(info->unk6);
    if (info->unk8 & 1) {
        set_unk4(1);
    } else {
        set_unk4(0);
    }
    if (fn_18_16E8() == 2) {
        fn_18_17AC(info->unk1, lbl_18_bss_BDC.unk0[0]);
    } else {
        fn_18_17AC(info->unk1, info->unk2);
    }
    set_mode(info->unk4);
    init_pads(player_pad_map[0], player_pad_map[1]);
}

void fn_18_24B4(u8 arg0)
{
    lbl_18_bss_970.unk268 = arg0;
    lbl_18_data_428[lbl_18_bss_970.unk268].unk0();
}

void fn_18_2504(void)
{
    g_font_char_var_3++;
    fn_18_25A80();
    if (lbl_18_bss_970.unk268 != 4 && (merged_digital_inputs[2] & 0x1000)) {
        fn_18_344(1);
        fn_18_18E70(lbl_18_bss_1218, 0x50);
    }
    if (fn_18_334() == 1) {
        switch (fn_18_25AB8()) {
        case 0:
            fn_18_344(0);
            fn_18_18E70(lbl_18_bss_1218, 0x7F);
            fn_18_19230();
            break;
        case 1:
            fn_18_344(0);
            fn_18_18E70(lbl_18_bss_1218, 0x7F);
            fn_18_274();
            break;
        }
        return;
    }
    fn_18_28B0();
    if (lbl_18_data_428[lbl_18_bss_970.unk268].unk4 != NULL) {
        lbl_18_data_428[lbl_18_bss_970.unk268].unk4();
    }
}

void fn_18_264C(u8 arg0)
{
    if (lbl_18_data_428[lbl_18_bss_970.unk268].unk8 != NULL) {
        lbl_18_data_428[lbl_18_bss_970.unk268].unk8();
    }
    lbl_18_bss_970.unk268 = (u8)arg0;
    if (lbl_18_data_428[lbl_18_bss_970.unk268].unk0 != NULL) {
        lbl_18_data_428[lbl_18_bss_970.unk268].unk0();
    }
}

void fn_18_2724(u8 arg0)
{
    lbl_18_bss_970.unk268 = (u8)arg0;
    if (lbl_18_data_428[lbl_18_bss_970.unk268].unk0 != NULL) {
        lbl_18_data_428[lbl_18_bss_970.unk268].unk0();
    }
}

void fn_18_27A0(u32 arg0, s32 arg1)
{
    fade_screen_to_color(0x101, 0, (s32)(float)arg0);
    {
        s32 *t = &lbl_18_bss_970.unk220;

        t[0] = 1;
        t[1] = arg0;
        t[2] = arg1;
    }
}

void fn_18_2828(u32 arg0, s32 arg1)
{
    fade_screen_to_color(0x100, 0, (s32)(float)arg0);
    {
        s32 *t = &lbl_18_bss_970.unk220;

        t[0] = 1;
        t[1] = arg0;
        t[2] = arg1;
    }
}


struct BBPlayState lbl_18_data_428[5] = {
    { fn_18_2930, fn_18_2980, fn_18_297C },
    { fn_18_3004, fn_18_3154, fn_18_3134 },
    { fn_18_3430, fn_18_34E8, fn_18_34C0 },
    { fn_18_3BF4, fn_18_3E78, fn_18_3E44 },
    { fn_18_22664, fn_18_22834, fn_18_227FC },
};
