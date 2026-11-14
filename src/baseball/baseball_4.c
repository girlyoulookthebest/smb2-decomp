#include "baseball.h"
#include "input.h"

u8 lbl_18_data_468[0x170] = {
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

static u8 dbgCursor;
static u8 dbgGameMode;
static u8 dbgStadium;
static u8 dbgChar1;
static u8 dbgChar2;
static u8 dbgRepeat;
static u8 dbgPort1;
static u8 dbgPort2;
void fn_18_28B0(void)
{
    struct BBTimer *t = (struct BBTimer *) &lbl_18_bss_970.unk220;

    if ((u32) t->unk0 != 0) {
        if ((u32) t->unk4 != 0) {
            t->unk4--;
        }
        if ((u32) t->unk4 == 0) {
            if ((u32) t->unk8 != 0) {
                ((void (*)(void))t->unk8)();   /* native pointer for the call (x64 __ptr32) */
            }
            t->unk0 = 0;
        }
    }
}

void fn_18_2930(void)
{
    dbgCursor = 0;
    dbgGameMode = 0;
    dbgStadium = 0;
    dbgChar1 = 0;
    dbgChar2 = 0;
    dbgRepeat = 0;
    dbgPort1 = 0;
    dbgPort2 = 1;
}

void fn_18_297C(void)
{

}

void fn_18_2980(void)
{
    debug_textdraw_set_pos(1, dbgCursor + 2);
    debug_textdraw_vsprintf_buf_1(">");
    debug_textdraw_set_pos(2, 2);
    debug_textdraw_vsprintf_buf_1("stadium   >> ");
    if (dbgStadium == 0) {
        debug_textdraw_vsprintf_buf_1("banana stadium\n");
    } else {
        debug_textdraw_vsprintf_buf_1("monkey dome\n");
    }
    debug_textdraw_set_pos(2, 3);
    debug_textdraw_vsprintf_buf_1("game mode >> ");
    if (dbgGameMode == 0) {
        debug_textdraw_vsprintf_buf_1("1p vs 2p\n");
    } else if (dbgGameMode == 1) {
        debug_textdraw_vsprintf_buf_1("1p vs com\n");
    } else if (dbgGameMode == 2) {
        debug_textdraw_vsprintf_buf_1("tournament\n");
    }
    debug_textdraw_set_pos(2, 4);
    debug_textdraw_vsprintf_buf_1("1p char   >> ");
    switch (dbgChar1) {
    case 0:
        debug_textdraw_vsprintf_buf_1("aiai\n");
        break;
    case 1:
        debug_textdraw_vsprintf_buf_1("meemee\n");
        break;
    case 2:
        debug_textdraw_vsprintf_buf_1("baby\n");
        break;
    case 3:
        debug_textdraw_vsprintf_buf_1("gongon\n");
        break;
    }
    debug_textdraw_set_pos(2, 5);
    debug_textdraw_vsprintf_buf_1("2p char   >> ");
    switch (dbgChar2) {
    case 0:
        debug_textdraw_vsprintf_buf_1("aiai\n");
        break;
    case 1:
        debug_textdraw_vsprintf_buf_1("meemee\n");
        break;
    case 2:
        debug_textdraw_vsprintf_buf_1("baby\n");
        break;
    case 3:
        debug_textdraw_vsprintf_buf_1("gongon\n");
        break;
    }
    debug_textdraw_set_pos(2, 6);
    debug_textdraw_vsprintf_buf_1("1P port >> ");
    switch (dbgPort1) {
    case 0:
        debug_textdraw_vsprintf_buf_1("1P\n");
        break;
    case 1:
        debug_textdraw_vsprintf_buf_1("2P\n");
        break;
    case 2:
        debug_textdraw_vsprintf_buf_1("3P\n");
        break;
    case 3:
        debug_textdraw_vsprintf_buf_1("4P\n");
        break;
    }
    debug_textdraw_set_pos(2, 7);
    debug_textdraw_vsprintf_buf_1("2P port >> ");
    switch (dbgPort2) {
    case 0:
        debug_textdraw_vsprintf_buf_1("1P\n");
        break;
    case 1:
        debug_textdraw_vsprintf_buf_1("2P\n");
        break;
    case 2:
        debug_textdraw_vsprintf_buf_1("3P\n");
        break;
    case 3:
        debug_textdraw_vsprintf_buf_1("4P\n");
        break;
    }
    if (dbgRepeat != 0) {
        dbgRepeat--;
        return;
    }
    dbgRepeat = 0x14;
    if ((pad_status_groups->unk0[0].button & 8) != 0) {
        dbgCursor = (dbgCursor == 0) ? 5 : dbgCursor - 1;
    } else if ((pad_status_groups->unk0[0].button & 4) != 0) {
        dbgCursor = (dbgCursor < 5) ? dbgCursor + 1 : 0;
    } else if ((pad_status_groups->unk0[0].button & 2) != 0) {
        switch (dbgCursor) {
        case 0:
            dbgStadium = (dbgStadium < 2) ? dbgStadium + 1 : 0;
            break;
        case 1:
            dbgGameMode = (dbgGameMode < 2) ? dbgGameMode + 1 : 0;
            break;
        case 2:
            dbgChar1 = (dbgChar1 < 3) ? dbgChar1 + 1 : 0;
            break;
        case 3:
            dbgChar2 = (dbgChar2 < 3) ? dbgChar2 + 1 : 0;
            break;
        case 4:
            dbgPort1 = (dbgPort1 < 3) ? dbgPort1 + 1 : 0;
            break;
        case 5:
            dbgPort2 = (dbgPort1 < 3) ? dbgPort2 + 1 : 0;
            break;
        }
    } else if ((pad_status_groups->unk0[0].button & 1) != 0) {
        switch (dbgCursor) {
        case 0:
            dbgStadium = (dbgStadium == 0) ? 2 : dbgStadium - 1;
            break;
        case 1:
            dbgGameMode = (dbgGameMode == 0) ? 2 : dbgGameMode - 1;
            break;
        case 2:
            dbgChar1 = (dbgChar1 == 0) ? 3 : dbgChar1 - 1;
            break;
        case 3:
            dbgChar2 = (dbgChar2 == 0) ? 3 : dbgChar2 - 1;
            break;
        case 4:
            dbgPort1 = (dbgPort1 == 0) ? 3 : dbgPort1 - 1;
            break;
        case 5:
            dbgPort2 = (dbgPort2 == 0) ? 3 : dbgPort2 - 1;
            break;
        }
    } else {
        dbgRepeat = 0;
    }
    if ((pad_status_groups->unk0[0].button & 0x100) != 0) {
        fn_18_16D4(dbgGameMode);
        fn_18_16FC(dbgStadium);
        fn_18_17AC(dbgChar1, dbgChar2);
        fn_18_ECC(dbgPort1, dbgPort2);
        fn_18_1994(9);
        fn_18_196C(0);
        fn_18_264C(1);
    }
}

void fn_18_3004(void)
{
    struct BBSeq *s = (struct BBSeq *) &lbl_18_bss_970.unk1EC;
    u8 tr31;

    fn_18_191A4(0xFF);
    fn_18_1914C(0);
    memset(s, 0, 0xC);
    fn_18_120B4();
    fn_18_134B0();
    fn_18_15A08();
    fn_18_2233C();
    fn_18_11C7C(0);
    event_init(0);
    event_init(2);
    event_init(3);
    event_init(0x10);
    event_init(0x11);
    event_init(0xE);
    fn_18_E9A0(0, 2);
    fn_18_6DC();
    tr31 = fn_18_18CC(0);
    g_load_baseball(0, 0x23, 0, fn_18_1818(0), tr31, 0, fn_18_9F4, 0);
    tr31 = fn_18_18CC(1);
    g_load_baseball(1, 0x23, 0, fn_18_1818(1), tr31, 0, fn_18_9F4, 0);
    fade_screen_to_color(0x100, 0, 0x1E);
}

void fn_18_3134(void)
{
    fn_1_474D0();
}

void fn_18_3154(void)
{
    fn_18_3178();
    fn_18_329C();
}

void fn_18_3178(void)
{
    struct BBSeq *s;
    u8 *m = &lbl_18_bss_970.unkA;
    u16 spA;
    u16 sp8;

    s = (struct BBSeq *) &lbl_18_bss_970.unk1EC;
    if (s->unk1 != 0) {
        s->unk4++;
        spA = 0;
        sp8 = 0;
        if (fn_18_1A7C() == 1) {
            fn_18_1040(0, m[0]);
            fn_18_10A0(0, &spA);
        } else {
            fn_18_1040(0, m[0]);
            fn_18_1040(1, m[1]);
            fn_18_10A0(0, &spA);
            fn_18_10A0(1, &sp8);
        }
        if (((spA & 0x100) != 0) || ((sp8 & 0x100) != 0) || ((u32) s->unk4 > (u32) s->unk8)) {
            fn_18_19184();
            fn_18_15E88(0x2F);
            fn_18_E540(1);
            s->unk4 = 1;
            s->unk1 = 0;
            s->unk0++;
        }
    }
}

void fn_18_329C(void)
{
    struct BBSeq *s = (struct BBSeq *) &lbl_18_bss_970.unk1EC;

    if ((lbl_18_data_6A8[s->unk0]) != NULL) {
        lbl_18_data_6A8[s->unk0]();
    }
}

void fn_18_3304(void)
{
    struct BBSeq *s = (struct BBSeq *) &lbl_18_bss_970.unk1EC;

    fn_18_163FC();
    s->unk0++;
}

void fn_18_3344(void)
{
    struct BBSeq *s = (struct BBSeq *) &lbl_18_bss_970.unk1EC;

    if (fn_18_16E8() == 2) {
        if (fn_18_15EBC(0x2E) == 1) {
            s->unk1 = 1;
            s->unk8 = 0x10E;
        }
    } else if (fn_18_15EBC(0x2D) == 1) {
        s->unk1 = 1;
        s->unk8 = 0x190;
    }
}

void fn_18_33C8(void)
{
    struct BBSeq *s = (struct BBSeq *) &lbl_18_bss_970.unk1EC;

    if (fn_18_15EBC(0x2F) == 1) {
        if ((u32) s->unk4 == 1) {
            s->unk4--;
            fn_18_15EE8();
            return;
        }
        fn_18_264C(2);
    }
}

void fn_18_3430(void)
{
    struct BBLift *s = &lbl_18_bss_970.unk1F8;
    u8 tr31;

    memset(s, 0, 0x18);
    s->unk4 = 0x2D;
    s->unk1 = 0;
    s->unk0 = 1;
    fn_18_14288();
    tr31 = fn_18_19A7C();
    g_load_baseball(7, 0x23, 1, fn_18_199EC(), tr31, 0, fn_18_9F4, 0);
}

void fn_18_34C0(void)
{
    fn_18_142C8();
    fn_1_47444(7);
}

void fn_18_34E8(void)
{
    fn_18_3510();
    fn_18_3728(0);
}

void fn_18_3510(void)
{
    struct BBLift *s = &lbl_18_bss_970.unk1F8;

    if ((lbl_18_data_6B8[s->unk1]) != NULL) {
        lbl_18_data_6B8[s->unk1]();
    }
}

void fn_18_3578(void)
{
    struct BBLift *s = &lbl_18_bss_970.unk1F8;
    u16 spA;
    s8 sp9;
    s8 sp8;

    spA = 0;
    if (fn_18_1A7C() == 1) {
        if (fn_18_1A9C() == 1) {
            if ((u32) s->unk8 > 0xA) {
                spA |= 0x100;
            }
            s->unk8++;
        } else {
            fn_18_10C4();
            fn_18_1240(&sp9, &sp8, &spA);
        }
    } else {
        fn_18_10C4();
        fn_18_1240(&sp9, &sp8, &spA);
    }
    if ((spA & 0x100) != 0) {
        fn_18_18E30(0x6A);
        fn_18_148F0();
        s->unk1 = 0;
        s->unk4 = 0;
    }
}

void fn_18_364C(void)
{
    u8 *m = &lbl_18_bss_970.unkA;
    struct BBLift *s = &lbl_18_bss_970.unk1F8;
    u16 spA;
    u16 sp8;

    spA = 0;
    sp8 = 0;
    if (fn_18_1A7C() == 1) {
        fn_18_1040(0, m[0]);
        fn_18_10A0(0, &spA);
    } else {
        fn_18_1040(0, m[0]);
        fn_18_1040(1, m[1]);
        fn_18_10A0(0, &spA);
        fn_18_10A0(1, &sp8);
    }
    if (((spA & 0x100) != 0) || ((sp8 & 0x100) != 0)) {
        fn_18_15E88(0x3A);
        s->unk1 = 0;
    }
}

void fn_18_3728(u8 arg0)
{
    struct BBLift *s = &lbl_18_bss_970.unk1F8;

    if (arg0 == 0) {
        if ((lbl_18_data_6C4[s->unk0]) != NULL) {
            lbl_18_data_6C4[s->unk0]();
        }
    } else if ((lbl_18_data_6D8[s->unk0]) != NULL) {
        lbl_18_data_6D8[s->unk0]();
    }
}

void fn_18_37E0(void)
{
    struct BBLift *s = &lbl_18_bss_970.unk1F8;
    s32 tr30;
    s32 tr29;
    s32 tr28;
    s32 tr27;

    tr30 = fn_18_14394();
    tr29 = fn_18_14608();
    tr28 = fn_18_14C78();
    tr27 = fn_18_E7EC();
    if ((u32) s->unk4 != 0) {
        s->unk4--;
        if ((u32) s->unk4 == 0) {
            fn_18_16740();
            fn_18_15E88(0x38);
        }
    }
    if ((tr30 != 0) && (tr29 != 0) && (tr28 != 0) && (tr27 != 0)) {
        lbl_18_bss_1218 = fn_18_18E30(0x7E);
        s->unk0++;
        s->unk4 = 0x1E;
    }
}

void fn_18_38B0(void)
{
    struct BBLift *s = &lbl_18_bss_970.unk1F8;
    u8 tr30;

    if ((u32) s->unk4 != 0) {
        s->unk4--;
        if ((u32) s->unk4 == 0) {
            s->unk1 = 1;
            s->unk8 = 0;
        } else {
            return;
        }
    }
    fn_18_147C8();
    if (fn_18_14A20() != 0) {
        fn_18_14A4C();
        tr30 = fn_18_14AF4();
        fn_18_18E30(lbl_18_data_6EC[tr30]);
        fn_18_166E8(tr30);
        fn_18_15E88(0x39);
        s->unk4 = 0x2D;
        s->unk0++;
        s->unk1 = 2;
    }
}

void fn_18_3988(void)
{
    struct BBLift *s = &lbl_18_bss_970.unk1F8;
    s32 tr30;
    s32 tr29;
    s32 tr28;
    s32 tr3;

    if (s->unk1 == 0) {
        tr30 = fn_18_143E0();
        tr29 = fn_18_14654();
        tr28 = fn_18_14D78();
        tr3 = fn_18_E838();
        if ((tr30 != 0) && (tr29 != 0) && (tr28 != 0) && (tr3 != 0)) {
            lbl_18_bss_970.unk1B8 = 0.0f;
            fn_18_11C7C(fn_18_14AF4());
            fn_18_EAE4(0, 8);
            s->unk4 = 0;
            s->unk0++;
            fn_18_13120(fn_18_14AF4());
            fn_18_131B8();
            if (fn_18_18E98(lbl_18_bss_1218) != 0xFFFFFFFF) {
                fn_18_18E50(lbl_18_bss_1218);
            }
        }
    }
}

void (*lbl_18_data_6A8[4])(void) = {fn_18_3304, fn_18_3344, fn_18_33C8, NULL};
void (*lbl_18_data_6B8[3])(void) = {NULL, fn_18_3578, fn_18_364C};
void (*lbl_18_data_6C4[5])(void) = {NULL, fn_18_37E0, fn_18_38B0, fn_18_3988, fn_18_3A90};
void (*lbl_18_data_6D8[5])(void) = {NULL, fn_18_37E0, fn_18_38B0, fn_18_3988, fn_18_3B28};
u16 lbl_18_data_6EC[10] = {0x4A, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0, 0};
