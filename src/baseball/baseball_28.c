#include "baseball.h"
#include "file.h"

u8 lbl_18_data_800E0 = 0xFF;

s32 lbl_18_bss_197B0;
s32 lbl_18_bss_197B4;
s32 lbl_18_bss_197B8;
s32 lbl_18_bss_197BC;
s32 lbl_18_bss_197C0;
s32 lbl_18_bss_197C4;
s32 lbl_18_bss_197C8;
s32 lbl_18_bss_197CC[0x20];
u8 lbl_18_bss_1984C[0x20][0x50];
s32 lbl_18_bss_1A24C;
s32 lbl_18_bss_1A250;
s32 lbl_18_bss_1A254;
s32 lbl_18_bss_1A258;
s32 lbl_18_bss_1A25C;
void *lbl_18_bss_1A260;
void *lbl_18_bss_1A264;
void *lbl_18_bss_1A268;
void *lbl_18_bss_1A26C;
s32 lbl_18_bss_1A270[4];
u8 lbl_18_bss_1A280[0x40];

void fn_18_21A34(s32 arg0)
{
    if (lbl_18_bss_197B0 < 0) {
        lbl_18_bss_197B0 = arg0;
    } else {
        lbl_18_bss_197B4 = arg0;
    }
    lbl_18_bss_197B8 = 1;
    lbl_18_bss_197BC = 0;
}

void fn_18_21A68(void)
{
    if ((lbl_18_bss_197C0 == 1) || (lbl_18_bss_197C0 == 2)) {
        lbl_18_bss_197C0 = 3;
        lbl_18_bss_197C4 = 0x20;
    }
    lbl_18_bss_197B0 = lbl_18_bss_197B4 = -1;
}

s32 fn_18_21AA8(char *arg0, s32 arg1)
{
    char sp8[0x100];

    if (lbl_18_bss_197C8 >= 0x20) {
        return -1;
    }
    lbl_18_bss_197CC[lbl_18_bss_197C8] = arg1;
    sprintf(sp8, "/snd/adp/%s.adp", arg0);
    if (DTKQueueTrack(sp8, lbl_18_bss_1984C[lbl_18_bss_197C8], 0, NULL) == 0) {
        return lbl_18_bss_197C8++;
    }
    return -1;
}

static void baseball_dead_strings_28(void)
{
    printf("BGM Length    0x%x");
    printf("Stream Length 0x%x");
    printf("Stream Length 0x%x\n");
}

void fn_18_21B4C(u8 arg0)
{
    lbl_18_data_800E0 = arg0;
    if ((lbl_18_bss_197C0 == 1) || (lbl_18_bss_197C0 == 2)) {
        DTKSetVolume(lbl_18_data_800E0, lbl_18_data_800E0);
    }
}

void fn_18_21BB8(u8 arg0, s32 arg1)
{
    lbl_18_bss_1A24C = arg0;
    lbl_18_bss_1A250 = lbl_18_data_800E0;
    lbl_18_bss_197C4 = arg1;
    lbl_18_bss_1A254 = arg1;
}

void fn_18_21BE4(void)
{
    u8 tr0;

    if (g_DTK_get_something1() != 3) {
        switch (lbl_18_bss_197C0) {     /* irregular */
        case 0:
            if (lbl_18_bss_197B0 >= 0) {
                if (lbl_18_bss_1A258 == lbl_18_bss_197B0) {
                    DTKSetVolume(lbl_18_data_800E0, lbl_18_data_800E0);
                    DTKSetState(1);
                    lbl_18_bss_197B0 = lbl_18_bss_197B4;
                    lbl_18_bss_197B4 = -1;
                    lbl_18_bss_197C0 = 1;
                    return;
                }
                if (lbl_18_bss_1A258 > lbl_18_bss_197B0) {
                    DTKPrevTrack();
                    lbl_18_bss_1A258--;
                    return;
                }
                DTKNextTrack();
                lbl_18_bss_1A258++;
                return;
            }
            break;
        case 1:
            if (lbl_18_bss_197BC != 0) {
                DTKSetState(2);
                lbl_18_bss_197BC = 0;
                lbl_18_bss_197C0 = 2;
                return;
            }
            if (lbl_18_bss_197C4 > 0) {
                lbl_18_bss_197C4--;
                lbl_18_data_800E0 = (u8) ((s32) ((lbl_18_bss_1A24C * (lbl_18_bss_1A254 - lbl_18_bss_197C4)) + (lbl_18_bss_1A250 * lbl_18_bss_197C4)) / lbl_18_bss_1A254);
                if (lbl_18_bss_1A25C == 0) {
                    DTKSetVolume(lbl_18_data_800E0, lbl_18_data_800E0);
                }
            }
            if ((lbl_18_bss_197CC[lbl_18_bss_1A258] > 0) && (AIGetStreamSampleCount() > (u32) lbl_18_bss_197CC[lbl_18_bss_1A258])) {
                if ((lbl_18_bss_197B0 < 0) && (lbl_18_bss_197B8 != 0)) {
                    lbl_18_bss_197B0 = lbl_18_bss_1A258;
                }
                DTKSetState(0);
                lbl_18_bss_197C0 = 0;
                return;
            }
            break;
        case 2:
            if (lbl_18_bss_197BC != 0) {
                DTKSetState(1);
                lbl_18_bss_197BC = 0;
                lbl_18_bss_197C0 = 1;
                return;
            }
            break;
        case 3:
            lbl_18_bss_197C4--;
            tr0 = (lbl_18_data_800E0 * lbl_18_bss_197C4) / 32;
            if (lbl_18_bss_1A25C == 0) {
                DTKSetVolume(tr0, tr0);
            }
            if (lbl_18_bss_197C4 == 0) {
                DTKSetState(0);
                lbl_18_bss_197C0 = 0;
            }
            break;
        }
    }
}

static void *load_snd_file(char *path)
{
    struct DiscFileInfo file;
    u32 size;
    void *buf;

    buf = NULL;
    if (DVDOpen(path, &file) != 0 && (size = file.length) != 0) {
        size = (size + 0x1F) & ~0x1F;
        if (size < 0x10000) {
            buf = OSAllocFromHeap(currentHeap, size);
        } else {
            buf = OSAllocFromHeap(chara_heap, size);
        }
        if (buf != NULL) {
            memset(buf, 0xE0, size);
            if (DVDReadPrio(&file, buf, size, 0, 2) > 0) {
                DVDClose(&file);
            } else {
                buf = NULL;
            }
        }
    }
    return buf;
}

void fn_18_21E6C(char *arg0)
{
    char path[0x100];
    s32 heap;
    s32 i;

    heap = OSSetCurrentHeap(chara_heap);
    DVDChangeDir("snd/mkb");
    sprintf(path, "%s.pool", arg0);
    lbl_18_bss_1A260 = load_snd_file(path);
    sprintf(path, "%s.proj", arg0);
    lbl_18_bss_1A264 = load_snd_file(path);
    sprintf(path, "%s.samp", arg0);
    lbl_18_bss_1A268 = load_snd_file(path);
    sprintf(path, "%s.sdir", arg0);
    lbl_18_bss_1A26C = load_snd_file(path);
    DVDChangeDir("/");
    for (i = 0; i < 3; i++) {
        sndPushGroup(lbl_18_bss_1A264, (u16) i, lbl_18_bss_1A268, lbl_18_bss_1A26C, lbl_18_bss_1A260);
    }
    OSFreeToHeap(chara_heap, lbl_18_bss_1A268);
    OSSetCurrentHeap(heap);
    DTKInit();
    g_DTK_set_something(2);
    lbl_18_bss_197C0 = 0;
    lbl_18_bss_1A258 = 0;
    lbl_18_bss_197B0 = lbl_18_bss_197B4 = -1;
    lbl_18_bss_1A25C = 0;
}

static void baseball_dead_strings_28b(void)
{
    printf("");
}

void fn_18_22268(void)
{
    lbl_18_bss_1A270[0] = 1;
}

void fn_18_2227C(void)
{
    s32 r30;

    if (g_GTK_get_something2() != 0) {
        do {

        } while (g_DTK_get_something1() == 3);
        lbl_18_bss_1A270[0] = 0;
        DTKFlushTracks(fn_18_22268);
        do {

        } while (lbl_18_bss_1A270[0] == 0);
    }
    for (r30 = 0; r30 < 3; r30++) {
        sndPopGroup();
    }
    OSFreeToHeap(chara_heap, lbl_18_bss_1A260);
    OSFreeToHeap(chara_heap, lbl_18_bss_1A264);
    OSFreeToHeap(chara_heap, lbl_18_bss_1A26C);
}

void fn_18_2233C(void)
{
    Vec sp8 = {5000.0f, 12000.0f, -10000.0f};

    memset(lbl_18_bss_1A280, 0, 0x3C);
    fn_1_169660();
    fn_1_16373C((Vec *) &sp8);
    fn_18_19C08();
}

Vec lbl_18_data_80160[9][3] = {
    {
        {25.6844997f, 2.5f, -23.0837002f},
        {25.3854008f, 2.5f, -26.3533001f},
        {24.6620007f, 2.5f, -29.5559998f},
    },
    {
        {23.8614998f, 2.5f, -31.7852993f},
        {23.3533993f, 2.5f, -34.7016983f},
        {20.4776001f, 2.5f, -37.3964005f},
    },
    {
        {18.9867001f, 2.5f, -39.1722984f},
        {16.5568008f, 2.5f, -41.3802986f},
        {13.8593998f, 2.5f, -43.2522011f},
    },
    {
        {11.8965998f, 2.5f, -44.2897987f},
        {8.91419983f, 2.5f, -45.662899f},
        {5.77810001f, 2.5f, -46.6348991f},
    },
    {
        {3.27629995f, 2.5f, -47.0419006f},
        {0.0f, 2.5f, -47.2565994f},
        {-3.27629995f, 2.5f, -47.0419006f},
    },
    {
        {-5.77810001f, 2.5f, -46.6348991f},
        {-8.91419983f, 2.5f, -45.662899f},
        {-11.8965998f, 2.5f, -44.2897987f},
    },
    {
        {-13.8593998f, 2.5f, -43.2522011f},
        {-16.5568008f, 2.5f, -41.3802986f},
        {-18.9867001f, 2.5f, -39.1722984f},
    },
    {
        {-20.4776001f, 2.5f, -37.3964005f},
        {-23.3533993f, 2.5f, -34.7016983f},
        {-23.8614998f, 2.5f, -31.7852993f},
    },
    {
        {-24.6620007f, 2.5f, -29.5559998f},
        {-25.3854008f, 2.5f, -26.3533001f},
        {-25.6844997f, 2.5f, -23.0837002f},
    },
};

Vec lbl_18_data_802A4[9] = {
    {24.3976002f, 2.45000005f, -26.1973f},
    {21.4971008f, 2.45000005f, -34.1852989f},
    {15.9341002f, 2.45000005f, -40.5978012f},
    {8.55630016f, 2.45000005f, -44.7290993f},
    {0.0f, 2.45000005f, -46.2565994f},
    {-8.55630016f, 2.45000005f, -44.7290993f},
    {-15.9341002f, 2.45000005f, -40.5978012f},
    {-21.4971008f, 2.45000005f, -34.1852989f},
    {-24.3976002f, 2.45000005f, -26.1973f},
};

void fn_18_2239C(void)
{
    fn_1_1639BC();
    fn_18_19D98();
    fn_1_169940();
}

void fn_18_223C4(Vec *arg0)
{
    fn_1_163DAC(arg0);
}

void fn_18_223E4(Vec *arg0, Vec *arg1, float farg0)
{
    fn_1_164B90(0, arg0, arg1, farg0);
}

void fn_18_22414(Vec *arg0, Vec *arg1)
{
    fn_1_1641E4(arg0, arg1);
}

void fn_18_22434(Vec *arg0)
{
    fn_1_167E74(arg0);
}

void fn_18_22454(Vec *arg0)
{

}

void fn_18_22458(void)
{
    struct Camera *cam = cameras;

    fn_1_1637B4(0, cam->unk14C);
}

void fn_18_2248C(Vec *arg0, Vec *arg1, Vec *arg2)
{
    fn_1_168968(arg0, arg1, arg2);
}

void fn_18_224AC(void)
{
    Vec sp50;
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    s32 r31;
    u8 tr3;

    tr3 = fn_18_18B68();
    sp50 = lbl_18_data_80160[tr3][0];
    sp44 = lbl_18_data_80160[tr3][1];
    sp38 = lbl_18_data_80160[tr3][2];
    sp8 = lbl_18_data_802A4[tr3];
    PSVECSubtract(&sp8, &sp50, &sp2C);
    PSVECSubtract(&sp8, &sp44, &sp20);
    PSVECSubtract(&sp8, &sp38, &sp14);
    vec_set_len(0.4f, &sp2C, &sp2C);
    vec_set_len(0.4f, &sp20, &sp20);
    vec_set_len(0.4f, &sp14, &sp14);
    fn_18_18E30(0x6F);
    for (r31 = 0; r31 < 0x32; r31++) {
        fn_1_167028(&sp50, &sp2C);
    }
    for (r31 = 0; r31 < 0x32; r31++) {
        fn_1_167028(&sp44, &sp20);
    }
    for (r31 = 0; r31 < 0x32; r31++) {
        fn_1_167028(&sp38, &sp14);
    }
}

