#include "baseball.h"

struct BBEnding lbl_18_bss_1A2C0;

void fn_18_22664(void)
{
    Vec pos = {0.0f, 1.0f, -30.0f};
    s8 charas[4];
    s8 ranks[2];
    Mtx mtx;

    camera_set_state(0x46);
    event_init(0xC);
    charas[0] = lbl_18_bss_1A2C0.unk0;
    charas[1] = lbl_18_bss_1A2C0.unk1;
    charas[2] = -1;
    charas[3] = -1;
    ranks[0] = lbl_18_bss_1A2C0.unk2;
    ranks[1] = lbl_18_bss_1A2C0.unk3;
    fn_1_E4708(charas, ranks);
    switch (lbl_18_bss_1A2C0.unk5) {
    case 0:
        if (lbl_18_bss_1A2C0.unk6 == 1) {
            fn_1_E67BC(0xC);
        } else {
            fn_1_E67BC(5);
        }
        break;
    case 1:
        fn_1_E67BC(6);
        break;
    case 2:
        fn_1_E67BC(2);
        break;
    case 3:
        fn_1_E67BC(0);
        break;
    }
    mtxa_from_identity();
    mtxa_translate(&pos);
    mtxa_rotate_y(-0x8000);
    mtxa_to_mtx(mtx);
    fn_1_E6760(mtx);
    fn_18_EAE4(0, 0x13);
    g_font_char_var_3 = 0;
    lbl_18_bss_1A2C0.unk4 = 0;
    lbl_18_bss_1A2C0.unk8 = 0;
    lbl_18_bss_1A2C0.unkC = 0;
}

void fn_18_227FC(void)
{
    event_dest(0xC);
    memset(&lbl_18_bss_1A2C0, 0, 0x10);
}

void fn_18_22834(void)
{
    u8 *teams = (u8 *) &lbl_18_bss_970 + 0xA;
    u16 spA;
    u16 sp8;

    spA = 0;
    sp8 = 0;
    if (fn_18_1A7C() == 1) {
        fn_18_1040(0, teams[0]);
        fn_18_10A0(0, &spA);
    } else {
        fn_18_1040(0, teams[0]);
        fn_18_1040(1, teams[1]);
        fn_18_10A0(0, &spA);
        fn_18_10A0(1, &sp8);
    }
    spA |= sp8;
    switch (lbl_18_bss_1A2C0.unk4) {
    case 0:
        fn_18_22B00();
        if (fn_1_E5DC8() == 0) {
            break;
        }
        if ((spA & 0x100) != 0) {
            lbl_18_bss_1A2C0.unk8 = 0x1E;
            fade_screen_to_color(0x101, 0, lbl_18_bss_1A2C0.unk8);
            g_fade_track_volume(lbl_18_bss_1A2C0.unk8, 2);
            g_return_to_sel_ngc(modeCtrl.gameType);
            lbl_18_bss_1A2C0.unk4 = 1;
        }
        if (lbl_18_bss_1A2C0.unkC < 300) {
            lbl_18_bss_1A2C0.unkC++;
        }
        if (lbl_18_bss_1A2C0.unkC == 1) {
            switch (lbl_18_bss_1A2C0.unk5) {
            case 0:
            case 1:
            case 3:
                fn_18_18E30(0x57);
                break;
            case 2:
                fn_18_18E30(0x5A);
                break;
            }
        }
        if (lbl_18_bss_1A2C0.unkC == 0x78) {
            switch (lbl_18_bss_1A2C0.unk5) {
            case 0:
                if (lbl_18_bss_1A2C0.unk6 == 1) {
                    fn_18_18E30(0x58);
                } else {
                    fn_18_18E30(0x53);
                }
                break;
            case 1:
                fn_18_18E30(0x54);
                break;
            case 2:
            case 3:
                break;
            }
        }
        break;
    case 1:
        if (lbl_18_bss_1A2C0.unk8 == 0) {
            event_dest(0xC);
            memset(&lbl_18_bss_1A2C0, 0, 0x10);
            fn_18_274();
        } else {
            lbl_18_bss_1A2C0.unk8--;
        }
        break;
    }
}

void fn_18_22AC4(s8 arg0, s8 arg1, s8 arg2, s8 arg3)
{
    struct BBEnding *ending = &lbl_18_bss_1A2C0;

    ending->unk0 = arg0;
    ending->unk1 = arg2;
    ending->unk2 = arg1;
    ending->unk3 = arg3;
}

void fn_18_22AE4(u8 arg0, u8 arg1)
{
    lbl_18_bss_1A2C0.unk5 = arg0;
    lbl_18_bss_1A2C0.unk6 = arg1;
}

void fn_18_22B00(void)
{
    float frame;
    struct CommendWork *work;
    s32 i;
    s32 flag;
    u32 mask;

    work = fn_1_E7668();
    for (i = 0; i < work->unk25; i++) {
        struct Ape *ape = work->apes[i];
        s32 sounds[4][2] = {{0x3D, 0x3F}, {0x49, 0x48}, {0x33, 0x37}, {0x2A, 0x2E}};

        if (ape != NULL) {
        mask = fn_1_E5CD0();
        if (mask == 1 << (((u32) ape->unk29C >> 8) & 0xFF) ||
            ((mask & (1 << (((u32) ape->unk29C >> 8) & 0xFF))) != 0 && (rand() & 0x7FFF) % 3 == 0)) {
            flag = 0;
            frame = body_frame_add72(ape);
            if (fn_1_1135E8(ape) - 1.0 < frame) {
                flag = 1;
            }
            if (flag != 0) {
                g_smth_with_sound = ape->charaId;
                g_player_id_for_sound = 0;
                switch ((u8) ape->unk29C) {
                case 0:
                    fn_18_18EB8(sounds[ape->charaId][0], &ape->pos);
                    break;
                case 3:
                    fn_18_18EB8(sounds[ape->charaId][1], &ape->pos);
                    break;
                }
            }
        }
        }
    }
}

