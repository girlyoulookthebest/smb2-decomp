#include "baseball.h"
#include "input.h"

struct BBStateFuncs
{
    void (*unk0)(struct BBSub *);
    void (*unk4)(struct BBSub *);
    void (*unk8)(struct BBSub *);
};

u8 lbl_18_data_700[0x170] = {
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

struct BBStateFuncs lbl_18_data_870[20] = {
    { fn_18_3F1C, fn_18_3F68, fn_18_3F64 },
    { fn_18_4038, fn_18_405C, fn_18_4058 },
    { fn_18_423C, fn_18_4258, fn_18_4254 },
    { fn_18_4A7C, fn_18_4B60, fn_18_4B40 },
    { fn_18_4318, fn_18_4340, fn_18_433C },
    { fn_18_44BC, fn_18_4500, fn_18_44E0 },
    { fn_18_5294, fn_18_52E0, fn_18_52DC },
    { fn_18_4848, fn_18_48DC, fn_18_48BC },
    { fn_18_45D4, fn_18_45FC, fn_18_45F8 },
    { fn_18_4D78, fn_18_4E24, fn_18_4E00 },
    { fn_18_4F48, fn_18_502C, fn_18_5004 },
    { fn_18_5720, fn_18_5794, fn_18_5774 },
    { fn_18_5BAC, fn_18_5DCC, fn_18_5CF8 },
    { fn_18_72B8, fn_18_731C, fn_18_7318 },
    { fn_18_7AA8, fn_18_7CE8, fn_18_7CD4 },
    { fn_18_7070, fn_18_70D4, fn_18_70D0 },
    { fn_18_5950, fn_18_5A28, fn_18_5A08 },
    { fn_18_6C98, fn_18_6CC4, fn_18_6CC0 },
    { fn_18_6DBC, fn_18_6DC4, fn_18_6DC0 },
    { fn_18_560C, fn_18_5610, fn_18_571C },
};

Vec lbl_18_data_960[2] = {
    { 0.0f, 0.5f, 0.0f },
    { -9.6f, 0.5f, -10.0f },
};

static Vec lbl_18_data_978[4] = {
    { -8.2f, 0.5f, -4.0f },
    { -7.2f, 0.5f, -3.0f },
    { -6.2f, 0.5f, -2.0f },
    { -5.2f, 0.5f, -1.0f },
};

static float lbl_18_data_9A8[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
static float lbl_18_data_9B8[4] = { 90.0f, -5.2f, 0.0f, 0.0f };
static float lbl_18_data_9C8[4] = { 0.0f, 0.0f, 0.0f, 1.5f };
static float lbl_18_data_9D8[4] = { 90.0f, 1.0f, -1.5f, 0.0f };

Vec lbl_18_data_9E8[10] = {
    { 0.0f, 0.5f, -20.5f },
    { -5.0f, 0.5f, -15.5f },
    { -5.0f, 0.5f, -5.5f },
    { 5.0f, 0.5f, -5.5f },
    { 0.0f, 0.5f, -15.5f },
    { 0.0f, 0.5f, -10.5f },
    { -1.0f, 0.5f, -9.5f },
    { -2.0f, 0.5f, -8.5f },
    { 2.0f, 0.5f, -8.5f },
    { 1.0f, 0.5f, -9.5f },
};

static float lbl_18_bss_BF0;
static float lbl_18_bss_BF4;
static float lbl_18_bss_BF8;
struct BBBFC lbl_18_bss_BFC;
struct BBC08 lbl_18_bss_C08[4];
struct BBC68 lbl_18_bss_C68;

static void set_phase(u32 n)
{
    struct BBSub *s = &lbl_18_bss_970.unk210;

    s->unkC = n;
}

static void set_state(u32 n)
{
    struct BBSub *s = &lbl_18_bss_970.unk210;

    if (lbl_18_data_870[s->unk0].unk8 != NULL) {
        lbl_18_data_870[s->unk0].unk8(s);
    }
    s->unk0 = n;
    if (lbl_18_data_870[s->unk0].unk0 != NULL) {
        lbl_18_data_870[s->unk0].unk0(s);
    }
}

void fn_18_3BB0(void)
{
    fn_18_264C(3);
}

#pragma auto_inline off

s32 fn_18_3BD4(void)
{
    struct BBLift *s = &lbl_18_bss_970.unk1F8;

    return s->unk0 == 0;
}

#pragma auto_inline reset

void fn_18_3BF4(void)
{
    struct BBSub *s = &lbl_18_bss_970.unk210;

    if (fn_18_1798() == 0) {
        fn_18_1914C(3);
    } else {
        fn_18_1914C(4);
        fn_18_1914C(5);
    }
    memset(s, 0, 0x10);
    camera_setup_singleplayer_viewport();
    camera_set_state(0x56);
    fn_18_6DC();
    fn_18_EAE4(0, 0);
    g_load_baseball(0, 0x23, 2, fn_18_1818(0), fn_18_19A7C(), 0, fn_18_9F4, 0);
    g_load_baseball(1, 0x23, 3, fn_18_1818(1), fn_18_19AC4(), 0, fn_18_9F4, 0);
    g_load_baseball(2, 0x23, 4, fn_18_1818(1), fn_18_19AC4(), 2, fn_18_C4C, 1);
    g_load_baseball(3, 0x23, 4, fn_18_1818(1), fn_18_19AC4(), 2, fn_18_C4C, 1);
    g_load_baseball(4, 0x23, 4, fn_18_1818(1), fn_18_19AC4(), 2, fn_18_C4C, 1);
    g_load_baseball(5, 0x23, 4, fn_18_1818(1), fn_18_19AC4(), 2, fn_18_C4C, 1);
    fn_18_138C0();
    fn_18_15200();
    fn_18_157A8();
    fn_18_260BC();
    fn_18_15E88(0x1E);
    fn_18_2828(0x1E, 0);
    s->unk0 = 0;
    if (lbl_18_data_870[s->unk0].unk0 != NULL) {
        lbl_18_data_870[s->unk0].unk0(s);
    }
}

void fn_18_3E44(void)
{
    fn_18_19184();
    fn_18_134EC();
    fn_18_15EE8();
    fn_18_15A38();
    fn_1_474D0();
    fn_18_19D98();
}

void fn_18_3E78(void)
{
    struct BBSub *s = &lbl_18_bss_970.unk210;

    fn_18_10C4();
    fn_18_1274();
    if (lbl_18_data_870[s->unk0].unk4 != NULL) {
        lbl_18_data_870[s->unk0].unk4(s);
    }
}

u32 fn_18_3EF4(void)
{
    struct BBSub *s = &lbl_18_bss_970.unk210;

    return s->unkC;
}

u8 fn_18_3F08(void)
{
    struct BBSub *s = &lbl_18_bss_970.unk210;

    return s->unk0;
}

void fn_18_3F1C(struct BBSub *arg0)
{
    fn_18_EAE4(0, 0);
    fn_18_26400();
    fn_18_15F2C();
    fn_18_19B48();
    set_phase(1);
}

void fn_18_3F64(struct BBSub *arg0)
{

}

void fn_18_3F68(struct BBSub *arg0)
{
    if (fn_18_1A14() == 1) {
        set_state(1);
    }
    fn_18_15E60();
}

void fn_18_4038(struct BBSub *arg0)
{
    set_phase(2);
    arg0->unk8 = 0xA;
}

void fn_18_4058(struct BBSub *arg0)
{

}

void fn_18_405C(struct BBSub *arg0)
{
    s16 tr3;

    if (fn_18_C0F4() == 1) {
        arg0->unk8--;
    }
    if (arg0->unk8 == 0) {
        if (fn_18_EBDC() != 1) {
            fn_18_EAE4(0, 1);
        }
        fn_18_15EE8();
        fn_18_15E38();
    }
    fn_18_15E60();
    if (fn_18_17E40() == 1) {
        tr3 = fn_18_18B2C();
        if (tr3 != -1) {
            set_state((u8) (s8) tr3);
        } else {
            set_state(2);
        }
    }
}

void fn_18_423C(struct BBSub *arg0)
{
    set_phase(3);
}

void fn_18_4254(struct BBSub *arg0)
{

}

void fn_18_4258(struct BBSub *arg0)
{
    set_state(0);
}

void fn_18_4318(struct BBSub *arg0)
{
    fn_18_15E88(0x20);
}

void fn_18_433C(struct BBSub *arg0)
{

}

void fn_18_4340(struct BBSub *arg0)
{
    if (fn_18_15EBC(0x20) == 1) {
        if (fn_18_1942C() == 0) {
            set_state(0xF);
        } else {
            set_state(2);
        }
    }
}

void fn_18_44BC(struct BBSub *arg0)
{
    fn_18_15E88(0x23);
}

void fn_18_44E0(struct BBSub *arg0)
{
    fn_18_19404();
}

void fn_18_4500(struct BBSub *arg0)
{
    fn_18_15E60();
    if (fn_18_15EBC(0x23) == 1) {
        set_state(2);
    }
}

void fn_18_45D4(struct BBSub *arg0)
{
    fn_18_15E88(0x21);
}

void fn_18_45F8(struct BBSub *arg0)
{

}

void fn_18_45FC(struct BBSub *arg0)
{
    u8 tr31;

    if (fn_18_15EBC(0x21) == 1) {
        if (fn_18_193C8() == 0) {
            tr31 = fn_18_18A70();
            fn_18_194B4();
            if (tr31 != 0) {
                fn_18_15EE8();
                lbl_18_bss_11F0.unkE = tr31;
                set_state(0xB);
            } else {
                set_state(2);
            }
            fn_18_197EC();
            fn_18_19730();
            return;
        }
        set_state(2);
    }
}

void fn_18_4848(struct BBSub *arg0)
{
    if (fn_18_D390() == 1) {
        fn_18_EAE4(0, 0xD);
    } else {
        fn_18_EAE4(0, 4);
    }
    fn_18_191C4(0, 0xF, 1);
    lbl_18_bss_1218 = fn_18_18E30(0x79);
    fn_18_15EE8();
    fn_18_15E88(0x22);
}

void fn_18_48BC(struct BBSub *arg0)
{
    fn_18_19730();
}

void fn_18_48DC(struct BBSub *arg0)
{
    s8 tr30;

    if (fn_18_15EBC(0x22) == 1) {
        fn_18_191C4(0xFF, 0xF, 0);
        if (fn_18_19468() == 0) {
            tr30 = fn_18_7270();
            set_state(tr30);
        } else {
            set_state(2);
        }
    }
}

void fn_18_4A7C(struct BBSub *arg0)
{
    fn_18_EAE4(0, 7);
    fn_18_191C4(0, 0xF, 1);
    lbl_18_bss_1218 = fn_18_18E30(0x7B);
    arg0->unk4 = fn_18_18B00();
    fn_18_15E88(arg0->unk4);
    switch (arg0->unk4) {
    case 36:
        arg0->unk6 = 1;
        break;
    case 37:
        arg0->unk6 = 2;
        break;
    case 38:
        arg0->unk6 = 3;
        break;
    default:
        arg0->unk6 = 1;
        break;
    }
    arg0->unk8 = 0x3C;
}

void fn_18_4B40(struct BBSub *arg0)
{
    fn_18_19798();
}

void fn_18_4B60(struct BBSub *arg0)
{
    s32 tr0;
    u8 tr31;

    if (arg0->unk8 != 0) {
        arg0->unk8--;
        if (arg0->unk8 == 0x28) {
            fn_18_224AC();
        }
        if (arg0->unk8 == 0) {
            switch (arg0->unk4) {
            case 0x24:
                tr0 = 0x74;
                break;
            case 0x25:
                tr0 = 0x75;
                break;
            case 0x26:
                tr0 = 0x60;
                break;
            default:
                tr0 = 0x74;
                break;
            }
            fn_18_18E30(tr0);
        }
    }
    if (fn_18_15EBC(arg0->unk4) == 1) {
        tr31 = fn_18_18B58();
        fn_18_191C4(0xFF, 0xF, 0);
        if (tr31 != 0) {
            set_state(0xC);
        } else {
            set_state(0xA);
        }
    }
}

void fn_18_4D78(struct BBSub *arg0)
{
    fn_18_EAE4(0, 6);
    set_phase(7);
    fn_18_191C4(0, 0xF, 1);
    lbl_18_bss_1218 = fn_18_18E30(0x78);
    arg0->unk6 = 4;
    fn_18_15EE8();
    fn_18_15E88(0x27);
    arg0->unk8 = 0x12C;
}

void fn_18_4E00(struct BBSub *arg0)
{
    fn_18_15EE8();
    fn_18_19798();
}

void fn_18_4E24(struct BBSub *arg0)
{
    if (fn_18_15EBC(0x27) == 1) {
        if (arg0->unk8 == 0) {
            fn_18_191C4(0xFF, 0xF, 0);
            set_state(0xC);
        } else {
            if (arg0->unk8 == 0xE6) {
                fn_18_15E88(0x28);
                arg0->unk8--;
                return;
            }
            arg0->unk8--;
        }
    }
}

void fn_18_4F48(struct BBSub *arg0)
{
    fn_18_15E88(0x36);
    lbl_18_bss_BF0 = 0.0f;
    lbl_18_bss_BF4 = 0.0f;
    arg0->unk1 = 0;
    switch (arg0->unk4) {
    case 36:
        lbl_18_bss_BF8 = 1.0f;
        return;
    case 37:
        lbl_18_bss_BF8 = 2.0f;
        return;
    case 38:
        lbl_18_bss_BF8 = 3.0f;
        return;
    default:
        lbl_18_bss_BF8 = 1.0f;
        return;
    }
}

void fn_18_5004(struct BBSub *arg0)
{
    fn_18_197EC();
    fn_18_19730();
    fn_18_15E60();
}

void fn_18_502C(struct BBSub *arg0)
{
    switch (arg0->unk1) {
    case 0:
        if (fn_18_15EBC(0x36) == 1) {
            if (lbl_18_bss_BF4 < lbl_18_bss_BF8) {
                if (lbl_18_bss_BF0 <= 1.0) {
                    fn_18_1687C(lbl_18_bss_BF0);
                    fn_18_15E88(0x35);
                    lbl_18_bss_BF0 += 0.015;
                    if ((lbl_18_bss_BF0 >= 0.5) && (fn_18_19BE0() == 1)) {
                        fn_18_15E88(0x37);
                        set_state(0xC);
                    }
                } else {
                    fn_18_19588(1);
                    lbl_18_bss_BF4 += 1.0f;
                    lbl_18_bss_BF0 = 0.0f;
                }
            } else {
                arg0->unk1 = 1;
                fn_18_15E88(0x37);
            }
        }
        break;
    case 1:
        if (fn_18_15EBC(0x37) == 1) {
            set_state(2);
        }
        break;
    }
}

void fn_18_5294(struct BBSub *arg0)
{
    set_phase(8);
    fn_18_DF60();
    arg0->unk8 = 0x258;
}

void fn_18_52DC(struct BBSub *arg0)
{

}

void fn_18_52E0(struct BBSub *arg0)
{
    struct Ball *tr29 = &balls[1];
    struct PhysicsBall sp8;
    s16 tr3;

    fn_18_15E60();
    if (fn_18_D390() == 1) {
        fn_18_18BCC();
        set_state(7);
    } else if (arg0->unk8 != 0) {
        arg0->unk8--;
        init_physicsball_from_ball(tr29, &sp8);
        if ((fn_18_16B1C((struct PhysicsBall *) &sp8) != -1) && (fn_18_17E40() == 1)) {
            tr3 = fn_18_18B2C();
            if (tr3 != -1) {
                set_state((u8) (s8) tr3);
            } else {
                set_state(7);
            }
        }
    } else {
        fn_18_18BCC();
        set_state(7);
    }
}

void fn_18_560C(struct BBSub *arg0)
{

}

void fn_18_5610(struct BBSub *arg0)
{
    s32 r3;
    struct BBGamePl *r4;

    r4 = lbl_18_bss_970.unkBC;
    if (pad_status_groups[2].unk0[2].button & 0x100) {
        for (r3 = 0; r3 < 0xA; r3++, r4++) {
            r4->unk7 = 0;
            r4->unk8 = 0;
        }
        set_state(6);
    }
}

void fn_18_571C(struct BBSub *arg0)
{

}

void fn_18_5720(struct BBSub *arg0)
{
    set_phase(5);
    fn_18_15E88(0x2A);
    arg0->unk3 = fn_18_18B58();
    arg0->unk8 = 0x3C;
}

void fn_18_5774(struct BBSub *arg0)
{
    fn_18_15EE8();
}

void fn_18_5794(struct BBSub *arg0)
{
    if (arg0->unk8 != 0) {
        arg0->unk8--;
        return;
    }
    if (arg0->unk3 != 0) {
        fn_18_15E88(0x2B);
        arg0->unk3--;
        arg0->unk8 = 0x3C;
        return;
    }
    fn_18_19318();
    if (fn_18_18198() == 1) {
        set_state(0xD);
    } else {
        set_state(2);
    }
}

void fn_18_5950(struct BBSub *arg0)
{
    set_phase(6);
    fn_18_EAE4(0, 5);
    lbl_18_bss_2E0.unk44 &= 0xFFFFFFFE;
    lbl_18_bss_2E0.unk11C &= 0xFFFFFFFE;
    fn_1_47444(0);
    fn_1_47444(1);
    fn_1_47444(2);
    fn_1_47444(3);
    fn_1_47444(4);
    fn_1_47444(5);
    fn_18_15EE8();
    fn_18_196A4();
    fn_18_16158();
    arg0->unk8 = 0xF;
}

void fn_18_5A08(struct BBSub *arg0)
{
    fn_18_26440();
}

void fn_18_5A28(struct BBSub *arg0)
{
    u16 spA;
    u16 sp8;
    u8 *p = &lbl_18_bss_970.unkA;

    spA = 0;
    sp8 = 0;
    if (fn_18_1A7C() == 1) {
        fn_18_1040(0, p[0]);
        fn_18_10A0(0, &spA);
    } else {
        fn_18_1040(0, p[0]);
        fn_18_1040(1, p[1]);
        fn_18_10A0(0, &spA);
        fn_18_10A0(1, &sp8);
    }
    if (arg0->unk8 == 0) {
        if ((spA & 0x100) || (sp8 & 0x100)) {
            set_state(0x11);
            fn_18_15E88(0x2C);
        }
    } else {
        arg0->unk8--;
    }
}

void fn_18_5BAC(struct BBSub *arg0)
{
    struct BBBFC *s = &lbl_18_bss_BFC;
    struct BBC08 *r29;
    u8 tr25;
    u8 tr27;
    s32 r30;

    fn_18_A248(0);
    fn_18_C1C8(0);
    fn_18_E0C0(0);
    fn_18_C244(1);
    lbl_18_bss_2E0.unk638 &= 0xFFFFFFFE;
    fn_18_191C4(0, 0xF, 1);
    lbl_18_bss_1218 = fn_18_18E30(0x7C);
    fn_18_15EE8();
    fn_18_15E88(0x2A);
    memset(s, 0, 0xC);
    s->unk7 = fn_18_18B58();
    r29 = lbl_18_bss_C08;
    tr25 = fn_18_199EC();
    tr27 = fn_18_19B34();
    memset(r29, 0, 0x60);
    s->unk6 = tr27;
    for (r30 = 0; r30 < tr27; r30++, r29++) {
        fn_18_8A34(r30, fn_18_19A7C(), tr25, 0, fn_18_6288);
        r29->unk2 = fn_18_6B94(r30);
        r29->unk4 = arg0->unk6;
    }
    fn_18_EAE4(0, 0xE);
}

void fn_18_5CF8(struct BBSub *arg0)
{
    fn_18_15EE8();
    fn_18_197EC();
    fn_18_19730();
    fn_18_191C4(0xFF, 0xF, 0);
    if (fn_18_18E98(lbl_18_bss_1218) != 0xFFFFFFFF) {
        fn_18_18E50(lbl_18_bss_1218);
    }
}

void fn_18_5D5C(struct BBPlay *arg0)
{
    struct BBC08 *tr31;

    tr31 = &lbl_18_bss_C08[arg0->unk82];
    fn_18_87F8(arg0);
    fn_18_89C0(arg0);
    mtxa_from_translate(&arg0->pos);
    mtxa_rotate_y(tr31->unkC);
    mtxa_to_quat(&arg0->unk64->rot);
}

void fn_18_5DCC(struct BBSub *arg0)
{
    u16 spA;
    u16 sp8;
    s32 i;
    struct BBBFC *s = &lbl_18_bss_BFC;
    struct BBC08 *c = lbl_18_bss_C08;
    u8 tr28;
    u8 *p = &lbl_18_bss_970.unkA;
    u8 r3;
    u8 r5;
    struct BBPlay *tr4;
    struct BBPlay *tr4_2;

    tr28 = fn_18_19B34();
    spA = 0;
    sp8 = 0;
    if (fn_18_1A7C() == 1) {
        fn_18_1040(0, p[0]);
        fn_18_10A0(0, &spA);
    } else {
        fn_18_1040(0, p[0]);
        fn_18_1040(1, p[1]);
        fn_18_10A0(0, &spA);
        fn_18_10A0(1, &sp8);
    }
    switch (s->unk4) {            /* irregular */
    case 0:
        r5 = 0;
        if ((spA & 0x100) || (sp8 & 0x100)) {
            r5 = 1;
        } else {
            if (c->unk8 >= 0.7) {
                r5 = 2;
            }
        }
        if (r5 != 0) {
            for (i = 0; i < (s32) tr28; i++, c++) {
                tr4 = &lbl_18_bss_D68[i];
                tr4->unk0 &= 0xFFFFFFFE;
                tr4->unk4 = fn_18_5D5C;
                s->unk4 = 1;
                if (r5 == 1) {
                    s->unk0 |= 2;
                }
            }
            fn_18_8B70();
            fn_18_EAE4(0, 0xC);
        }
        break;
    case 1:
        if ((u32) (s->unk0 & 1) == 0) {
            if ((u8) s->unk7 != s->unk8) {
                tr4_2 = &lbl_18_bss_D68[s->unk8];
                tr4_2->unk0 |= 1;
                tr4_2->unk4 = fn_18_6504;
                c[s->unk8].unk2 = 3;
                c[s->unk8].unk3 = 0;
                if ((u32) (s->unk0 & 2) != 0) {
                    c[s->unk8].unk8 = 0.6f;
                } else {
                    c[s->unk8].unk8 = 0.0f;
                }
                s->unk0 |= 1;
                s->unk8++;
            } else {
                i = 0;
                r3 = 0;
                while (i < s->unk7) {
                    if ((s32) (c->unk0 & 2) != 0) {
                        r3 = (u8) r3 + 1;
                    }
                    i++;
                    c++;
                }
                if ((u8) r3 == s->unk7) {
                    fn_18_EAE4(0, 0x10);
                    s->unk5 = 0x94;
                    s->unk4 = 2;
                }
            }
        }
        break;
    case 2:
        if (s->unk5 == 0) {
            for (i = 0; i < s->unk6; i++) {
                fn_18_8CD4((u8) i);
            }
            fn_18_19318();
            fn_18_194B4();
            if (fn_18_18198() == 1) {
                set_state(0xD);
            } else {
                fn_18_A248(1);
                fn_18_C1C8(1);
                fn_18_E0C0(1);
                fn_18_C244(0);
                lbl_18_bss_2E0.unk638 |= 1;
                set_state(2);
            }
        } else {
            s->unk5--;
        }
        break;
    }
    fn_18_8B70();
}

void fn_18_6288(struct BBPlay *arg0)
{
    Vec sp2C;
    Vec sp20;
    S16Vec rot;
    s16 r;
    struct Ape *tr4_3;
    struct BBC08 *tr30;
    u8 tr0;

    tr30 = &lbl_18_bss_C08[arg0->unk82];
    tr0 = tr30->unk2 + tr30->unk3;
    if (tr0 == 3) {
        sp2C = lbl_18_data_960[1];
        sp20 = lbl_18_data_960[0];
    } else {
        sp2C = lbl_18_data_368[tr0];
        sp20 = lbl_18_data_368[tr0 + 1];
    }
    fn_18_1A24(&sp2C, &sp20, &arg0->pos, 0.0f);
    arg0->pos.y = 0.5f;
    tr4_3 = arg0->unk64;
    tr4_3->pos = arg0->pos;
    arg0->prevPos = arg0->pos;
    fn_18_87F8(arg0);
    fn_18_89C0(arg0);
    ray_to_euler_xy((Vec *) &sp20, (Vec *) &sp2C, &rot.x, &rot.y);
    rot.y -= 0x8000;
    tr30->unkC = rot.y;
    mtxa_from_translate(&arg0->pos);
    mtxa_rotate_y(tr30->unkC);
    mtxa_to_quat(&arg0->unk64->rot);
    {
        u16 sp8[2] = { 0, 1 };
        u16 sp14[6] = { 4, 5, 6, 7, 8, 9 };

        r = rand() & 0x7FFF;
        r = __abs(r) % 2;
        tr30->unk6 = sp8[r];
        r = rand() & 0x7FFF;
        r = __abs(r) % 6;
        tr30->unk5 = sp14[r];
    }
    fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, tr30->unk6);
    arg0->unk4 = fn_18_6504;
}

void fn_18_6504(struct BBPlay *arg0)
{
    Vec sp1C;
    Vec sp10;
    S16Vec rot;
    s16 r;
    struct Ape *tr4_3;
    struct BBC08 *tr30;
    u8 tr0;
    void *tr4;
    void *tr4_2;

    tr30 = &lbl_18_bss_C08[arg0->unk82];
    tr0 = tr30->unk2 + tr30->unk3;
    tr30->unk8 += 0.005;
    if (tr30->unk8 > 1.0) {
        tr30->unk8 = 1.0f;
    }
    if (tr0 == 3) {
        sp1C = lbl_18_data_960[1];
        sp10 = lbl_18_data_960[0];
    } else {
        sp1C = lbl_18_data_368[tr0];
        sp10 = lbl_18_data_368[tr0 + 1];
    }
    arg0->prevPos = arg0->pos;
    fn_18_1A24(&sp1C, &sp10, &arg0->pos, tr30->unk8);
    arg0->pos.y = 0.5f;
    tr4_3 = arg0->unk64;
    tr4_3->pos = arg0->pos;
    fn_18_87F8(arg0);
    fn_18_89C0(arg0);
    mtxa_from_translate(&arg0->pos);
    ray_to_euler_xy((Vec *) &sp10, (Vec *) &sp1C, &rot.x, &rot.y);
    rot.y -= 0x8000;
    tr30->unkC += fn_18_1F40(rot.y, tr30->unkC, 0x100, 0xA);
    mtxa_rotate_y(tr30->unkC);
    mtxa_to_quat(&arg0->unk64->rot);
    if (tr30->unk8 >= 1.0) {
        struct BBBFC *s = &lbl_18_bss_BFC;

        fn_18_16A94();
        fn_18_15E88(0x2B);
        fn_18_18E30(0x76);
        s->unk0 &= 0xFFFFFFFE;
        arg0->unk4 = fn_18_6828;
        tr30->unk7 = s->unk9;
        s->unk9++;
    }
    if (fn_18_1584(arg0->unk64) != 0) {
        if (tr30->unk6 == 1) {
            r = rand() & 0x7FFF;
            r = __abs(r) % 2;
            if (r != 0) {
                tr30->unk6 = 2;
            } else {
                tr30->unk6 = 1;
            }
            fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, tr30->unk6);
            return;
        }
        if (tr30->unk6 == 2) {
            tr30->unk6 = 1;
            fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, tr30->unk6);
            return;
        }
        fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, 0);
    }
}

void fn_18_6828(struct BBPlay *arg0)
{
    Vec sp14;
    Vec sp8;
    s16 tr0;
    s16 r4;
    struct Ape *tr4;
    struct BBBFC *s = &lbl_18_bss_BFC;
    struct BBC08 *tr29;
    char *dbg = "stop jingle\n";

    tr29 = &lbl_18_bss_C08[arg0->unk82];
    if (tr29->unk10 < 90.0) {
        tr29->unk10 += 1.0;
        lbl_18_data_9B8[1] = lbl_18_data_978[(4 - s->unk7) + tr29->unk7].x;
        lbl_18_data_9D8[1] = lbl_18_data_978[(4 - s->unk7) + tr29->unk7].z;
        sp14.x = fn_18_15BC(tr29->unk10, lbl_18_data_9A8, lbl_18_data_9B8);
        sp14.z = fn_18_15BC(tr29->unk10, lbl_18_data_9C8, lbl_18_data_9D8);
        sp14.y = 0.5f;
        arg0->prevPos = arg0->pos;
        arg0->pos = sp14;
        tr4 = arg0->unk64;
        tr4->pos = arg0->pos;
        fn_18_87F8(arg0);
        fn_18_89C0(arg0);
        mtxa_from_translate(&arg0->pos);
        PSVECSubtract(&arg0->pos, &arg0->prevPos, &sp8);
        tr0 = math_atan2(sp8.x, sp8.z) - 0x8000;
        r4 = tr0 - tr29->unkC;
        if (__abs(r4) > 0x100) {
            r4 = r4 / 10;
            if (r4 < 0) {
                tr29->unkC += r4;
            } else {
                tr29->unkC -= r4;
            }
        }
        mtxa_rotate_y(tr29->unkC);
        mtxa_to_quat(&arg0->unk64->rot);
        if (fn_18_1584(arg0->unk64) != 0) {
            if (tr29->unk6 == 1) {
                fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, 1);
                return;
            }
            if (tr29->unk6 == 2) {
                fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, 1);
                return;
            }
            fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, 0);
        }
    } else {
        arg0->prevPos = arg0->pos;
        fn_18_87F8(arg0);
        fn_18_89C0(arg0);
        mtxa_from_translate(&arg0->pos);
        PSVECSubtract(&arg0->pos, &arg0->prevPos, &sp8);
        r4 = -tr29->unkC;
        if (__abs(r4) > 0x100) {
            r4 = r4 / 10;
            if (r4 < 0) {
                tr29->unkC += r4;
            } else {
                tr29->unkC -= r4;
            }
        }
        mtxa_rotate_y(tr29->unkC);
        mtxa_to_quat(&arg0->unk64->rot);
        fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, tr29->unk5);
        tr29->unk0 |= 2;
    }
}

s8 fn_18_6B94(u8 arg0)
{
    u8 tr30;
    u8 tr29;
    u8 tr28;
    u8 tr4;
    s32 r5;

    tr30 = fn_18_19B68();
    tr29 = fn_18_19B90();
    tr28 = fn_18_19BB8();
    tr4 = fn_18_19BE0();
    r5 = 0;
    if (tr30 != 0) {
        if ((u8) r5 == arg0) {
            return 0;
        }
        r5++;
    }
    if (tr29 != 0) {
        if ((u8) r5 == arg0) {
            return 1;
        }
        r5++;
    }
    if (tr28 != 0) {
        if ((u8) r5 == arg0) {
            return 2;
        }
        r5++;
    }
    if (tr4 != 0 && (u8) r5 == arg0) {
        return 3;
    }
    return -1;
}

void fn_18_6C98(struct BBSub *arg0)
{
    fn_18_1424();
    fn_18_19184();
    fn_18_3430();
}

void fn_18_6CC0(struct BBSub *arg0)
{

}

void fn_18_6CC4(struct BBSub *arg0)
{
    fn_18_3510();
    fn_18_3728(1);
    if (fn_18_3BD4() == 1) {
        set_state(0x12);
        arg0->unk8 = 0x1E;
        fn_18_27A0(arg0->unk8, 0);
    }
}

void fn_18_6DBC(struct BBSub *arg0)
{

}

void fn_18_6DC0(struct BBSub *arg0)
{

}

void fn_18_6DC4(struct BBSub *arg0)
{
    if (arg0->unk8 != 0) {
        arg0->unk8--;
        if (arg0->unk8 == 0) {
            fn_18_6EB8();
            fn_18_2828(0x1E, 0);
            set_state(2);
        }
    }
}

void fn_18_6EB8(void)
{
    if (fn_18_1798() == 0) {
        fn_18_1914C(3);
    } else {
        fn_18_1914C(4);
        fn_18_1914C(5);
    }
    fn_18_34C0();
    g_load_baseball(0, 0x23, 2, fn_18_199EC(), fn_18_19A7C(), 0, fn_18_9F4, 0);
    g_load_baseball(1, 0x23, 3, fn_18_19A34(), fn_18_19AC4(), 0, fn_18_9F4, 0);
    g_load_baseball(2, 0x23, 4, fn_18_19A34(), fn_18_19AC4(), 2, fn_18_C4C, 1);
    g_load_baseball(3, 0x23, 4, fn_18_19A34(), fn_18_19AC4(), 2, fn_18_C4C, 1);
    g_load_baseball(4, 0x23, 4, fn_18_19A34(), fn_18_19AC4(), 2, fn_18_C4C, 1);
    g_load_baseball(5, 0x23, 4, fn_18_19A34(), fn_18_19AC4(), 2, fn_18_C4C, 1);
    lbl_18_bss_2E0.unk44 |= 1;
    lbl_18_bss_2E0.unk11C |= 1;
}

void fn_18_7070(struct BBSub *arg0)
{
    set_phase(4);
    fn_18_A860(1, 6);
    fn_18_EAE4(0, 3);
    fn_18_19730();
    arg0->unk8 = 0xB4;
}

void fn_18_70D0(struct BBSub *arg0)
{

}

void fn_18_70D4(struct BBSub *arg0)
{
    s8 tr30;

    if (arg0->unk8 != 0) {
        arg0->unk8--;
        return;
    }
    if (fn_18_19468() == 0) {
        tr30 = fn_18_7270();
        set_state(tr30);
    } else {
        set_state(2);
    }
}

s8 fn_18_7270(void)
{
    if (fn_18_18B78() == 1) {
        return 0xD;
    }
    if (fn_18_1833C() == 1) {
        return 0xD;
    }
    return 0x10;
}

void fn_18_72B8(struct BBSub *arg0)
{
    fn_18_1970C();
    fn_18_15EE8();
    if (fn_18_18B78() == 1) {
        arg0->unk4 = 0x31;
    } else {
        arg0->unk4 = 0x30;
    }
    fn_18_15E88(arg0->unk4);
    arg0->unk8 = 0x78;
}

void fn_18_7318(struct BBSub *arg0)
{

}

void fn_18_731C(struct BBSub *arg0)
{
    if (arg0->unk8 == 0) {
        if (fn_18_15EBC(arg0->unk4) == 1) {
            set_state(0xE);
        }
    } else {
        arg0->unk8--;
    }
}

void fn_18_7408(struct BBPlay *arg0)
{
    s16 spA;
    s16 sp8;
    struct Ape *tr4_3;
            struct BBC68Item *tr31;

    tr31 = &lbl_18_bss_C68.unk8[arg0->unk82];
    tr31->unk10 = lbl_18_data_9E8[arg0->unk82];
    tr31->unk1C = lbl_18_data_9E8[arg0->unk82 + 5];
    arg0->pos = tr31->unk10;
    arg0->prevPos = arg0->pos;
    tr4_3 = arg0->unk64;
    tr4_3->pos = arg0->pos;
    ray_to_euler_xy(&tr31->unk1C, &tr31->unk10, &sp8, &spA);
    spA -= 0x8000;
    tr31->unk8 = spA;
    fn_18_87F8(arg0);
    fn_18_89C0(arg0);
    mtxa_from_translate(&arg0->pos);
    mtxa_rotate_y(tr31->unk8);
    mtxa_to_quat(&arg0->unk64->rot);
    fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, 1);
    arg0->unk4 = fn_18_7574;
}

void fn_18_7574(struct BBPlay *arg0)
{
    Vec sp30;
    Vec sp24;
    S16Vec rot;
    s16 r;
    struct Ape *tr4;
    struct BBC68 *s = &lbl_18_bss_C68;
    struct BBC68Item *tr29;

    tr29 = &lbl_18_bss_C68.unk8[arg0->unk82];
    if (tr29->unk4 < 1.0) {
        tr29->unk4 += 0.007;
    }
    if (tr29->unk4 > 1.0) {
        tr29->unk4 = 1.0f;
    }
    sp30 = tr29->unk10;
    sp24 = tr29->unk1C;
    arg0->prevPos = arg0->pos;
    fn_18_1A24(&sp30, &sp24, &arg0->pos, tr29->unk4);
    tr4 = arg0->unk64;
    tr4->pos = arg0->pos;
    ray_to_euler_xy((Vec *) &sp24, (Vec *) &sp30, &rot.x, &rot.y);
    rot.y -= 0x8000;
    tr29->unk8 += fn_18_1F40(rot.y, tr29->unk8, 0x800, 0xA);
    fn_18_87F8(arg0);
    fn_18_89C0(arg0);
    mtxa_from_translate(&arg0->pos);
    mtxa_rotate_y(tr29->unk8);
    mtxa_to_quat(&arg0->unk64->rot);
    fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, 1);
    if (1.0 == tr29->unk4) {
        u16 sp18[6] = { 4, 5, 6, 7, 8, 9 };
        u16 sp8[3] = { 0, 1, 2 };

        r = rand() & 0x7FFF;
        if (s->unk1 != 3) {
            r = __abs(r) % 6;
            tr29->unkA = 3;
            tr29->unkC = sp18[r];
        } else {
            r = __abs(r) % 3;
            tr29->unkA = 4;
            tr29->unkC = sp8[r];
        }
        arg0->unk4 = fn_18_7818;
    }
}

void fn_18_7818(struct BBPlay *arg0)
{
    s16 spA;
    s16 sp8;
    struct BBC68Item *tr31;
    struct Camera *cam;

    tr31 = &lbl_18_bss_C68.unk8[arg0->unk82];
    cam = cameras;
    ray_to_euler_xy(&cam->eye, &arg0->pos, &sp8, &spA);
    spA -= 0x8000;
    tr31->unk8 += fn_18_1F40(spA, tr31->unk8, 0x800, 0x1E);
    mtxa_from_translate(&arg0->pos);
    mtxa_rotate_y(tr31->unk8);
    mtxa_to_quat(&arg0->unk64->rot);
    fn_1_113190(arg0->unk64, 0x16, 0.0f, tr31->unkA, tr31->unkC);
}

void fn_18_78E4(struct BBPlay *arg0)
{
    float sp8[2];
    struct Ape *tr4;
    struct BBC68Item *tr30;

    tr30 = &lbl_18_bss_C68.unk8[arg0->unk82];
    math_sin_cos_v((s16)(0xAAAA - (s32)(182.04444885253906 * (60.0 * arg0->unk82))), sp8);
    tr30->unk10.x = 6.0 * sp8[1];
    tr30->unk10.z = -10.5 + (6.0 * -sp8[0]);
    tr30->unk10.y = 0.5f;
    tr30->unk1C.x = 1.8 * sp8[1];
    tr30->unk1C.z = -10.5 + (1.8 * -sp8[0]);
    tr30->unk1C.y = 0.5f;
    arg0->pos = tr30->unk10;
    arg0->prevPos = arg0->pos;
    tr4 = arg0->unk64;
    tr4->pos = arg0->pos;
    tr30->unk8 = -0x8000;
    fn_18_87F8(arg0);
    fn_18_89C0(arg0);
    mtxa_from_translate(&arg0->pos);
    mtxa_rotate_y(tr30->unk8);
    mtxa_to_quat(&arg0->unk64->rot);
    fn_1_113190(arg0->unk64, 0x16, 0.0f, 3, 1);
    arg0->unk4 = fn_18_7574;
}

void fn_18_7AA8(struct BBSub *arg0)
{
    u8 tr30;
    u8 r29;
    u8 r28;
    s32 r31;

    lbl_18_bss_2E0.unk638 &= 0xFFFFFFFE;
    fn_18_EAE4(0, 0xA);
    fn_18_165B8();
    arg0->unk1 = 0;
    fn_18_191C4(0, 0xF, 1);
    lbl_18_bss_1218 = fn_18_18E30(0x7A);
    arg0->unk8 = 0x384;
    fn_18_134EC();
    fn_1_47444(0);
    fn_1_47444(1);
    fn_1_47444(2);
    fn_1_47444(3);
    fn_1_47444(4);
    fn_1_47444(5);
    tr30 = fn_18_81B0();
    if ((s8) fn_18_19964() == 0) {
        r29 = fn_18_1818(0);
        r28 = fn_18_18CC(0);
    } else {
        r29 = fn_18_1818(1);
        r28 = fn_18_18CC(1);
    }
    memset(&lbl_18_bss_C68, 0, 0xF8);
    lbl_18_bss_C68.unk1 = tr30;
    switch (lbl_18_bss_C68.unk1) {
    case 0:
    case 1:
    case 2:
        for (r31 = 0; r31 < 5; r31++) {
            fn_18_8A34(r31, r28, r29, 0, fn_18_7408);
        }
        break;
    case 3:
        for (r31 = 1; r31 <= 6; r31++) {
            if (r31 % 2 != 0) {
                fn_18_8A34(r31 - 1, fn_18_18CC(0), fn_18_1818(0), 0, fn_18_78E4);
            } else {
                fn_18_8A34(r31 - 1, fn_18_18CC(1), fn_18_1818(1), 0, fn_18_78E4);
            }
        }
        break;
    }
}

void fn_18_7CD4(struct BBSub *arg0)
{

}

u8 fn_18_7CD8(void)
{
    return lbl_18_bss_C68.unk1;
}

void fn_18_7CE8(struct BBSub *arg0)
{
    u16 spA;
    u16 sp8;
    s8 tr26;
    s8 r31;
    s8 r30;
    struct BBC68 *s = &lbl_18_bss_C68;
    struct BBC68Item *it = &lbl_18_bss_C68.unk8[0];
    u8 *p = &lbl_18_bss_970.unkA;
    u8 tr27;
    u8 tr28;
    s8 tr29;
    u8 r25;

    tr27 = fn_18_19E0();
    fn_18_19964();
    switch (s->unk3) {            /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        if (it->unk4 > 0.7) {
            fn_18_EAE4(0, 0x11);
            if (s->unk1 == 3) {
                s->unk4 = 0x78;
            } else {
                s->unk4 = 0x168;
            }
            s->unk3 = 1;
        }
        break;
    case 1:                                         /* switch 1 */
        if (s->unk4 != 0) {
            s->unk4--;
            if (s->unk4 == 0) {
                fn_18_EAE4(0, 0x12);
                s->unk3 = 2;
            }
        }
        break;
    case 2:                                         /* switch 1 */
        if (arg0->unk8 != 0) {
            arg0->unk8--;
        }
        break;
    }
    fn_18_8B70();
    if (arg0->unk1 == 0) {
        spA = 0;
        sp8 = 0;
        if (fn_18_1A7C() == 1) {
            fn_18_1040(0, p[0]);
            fn_18_10A0(0, &spA);
        } else {
            fn_18_1040(0, p[0]);
            fn_18_1040(1, p[1]);
            fn_18_10A0(0, &spA);
            fn_18_10A0(1, &sp8);
        }
        tr28 = fn_18_81B0();
        r25 = 0;
        if (fn_18_16E8() == 2) {
            switch (tr28) {
            case 0:
                r25 = 1;
                break;
            case 1:
            case 2:
            case 3:
                r25 = 0;
                break;
            }
        }
        if ((r25 != 0) && (fn_18_1B50() == 0)) {
            if ((fn_18_15EBC(0x32) == 1) && ((spA & 0x100) || (sp8 & 0x100) || (arg0->unk8 == 0))) {
                if (fn_18_18E98(lbl_18_bss_1218) != 0xFFFFFFFF) {
                    fn_18_18E70(lbl_18_bss_1218, 0);
                    fn_18_18E50(lbl_18_bss_1218);
                    lbl_18_bss_1218 = -1;
                }
                arg0->unk1 = 1;
                fn_18_19260(0);
                fn_18_19184();
                fn_18_27A0(0x1E, (s32)(uptr)fn_18_8324);
            }
        } else if ((fn_18_15EBC(0x32) == 1) && ((spA & 0x100) || (sp8 & 0x100) || (arg0->unk8 == 0))) {
            tr29 = fn_18_19964();
            if (fn_18_18E98(lbl_18_bss_1218) != 0xFFFFFFFF) {
                fn_18_18E70(lbl_18_bss_1218, 0);
                fn_18_18E50(lbl_18_bss_1218);
                lbl_18_bss_1218 = -1;
                printf("stop jingle\n");
            }
            switch (tr27) {               /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                if (tr29 == 0) {
                    r31 = 0;
                    r30 = 3;
                } else if (tr29 == 1) {
                    r31 = 3;
                    r30 = 0;
                } else {
                    r31 = 0;
                    r30 = 0;
                }
                break;
            case 1:                                 /* switch 2 */
                if (tr29 == 0) {
                    r30 = 0;
                    r31 = 3;
                } else if (tr29 == 1) {
                    r30 = 3;
                    r31 = 0;
                } else {
                    r31 = 0;
                    r30 = 0;
                }
                break;
            case 2:                                 /* switch 2 */
                if (tr29 == 0) {
                    r30 = 0;
                    r31 = 3;
                } else if (tr29 == 1) {
                    r30 = 3;
                    r31 = 0;
                } else {
                    r31 = 0;
                    r30 = 0;
                }
                break;
            }
            if (fn_18_1A7C() == 1) {
                tr26 = fn_18_17C4(1) | 0x40;
                fn_18_22AC4(fn_18_17C4(0), r31, tr26, r30);
                fn_18_22AE4(tr28, 1);
            } else {
                tr26 = fn_18_17C4(1);
                fn_18_22AC4(fn_18_17C4(0), r31, tr26, r30);
                fn_18_22AE4(tr28, 0);
            }
            fn_18_191C4(0xFF, 0xF, 0);
            fn_18_8D3C();
            fn_18_264C(4);
        }
    }
}

