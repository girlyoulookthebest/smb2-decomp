#include "baseball.h"



struct BBCpu
{
    u8 unk0;
    u8 unk1;
    u8 filler2[2];
    u32 unk4;
    u8 unk8;
    u8 unk9;
    u16 unkA;
    u8 unkC;
    u8 unkD;
    u8 fillerE[3];
    u8 unk11;
    u8 unk12;
    u8 unk13;
    u8 unk14;
    u8 filler15[3];
    u32 unk18;
    u8 unk1C;
    u8 unk1D;
    u8 unk1E;
    u8 filler1F;
    s16 unk20;
    u8 filler22[2];
    float unk24;
    float unk28;
}; /* size = 0x2C */

struct BBFlags
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
}; /* size = 0x3 */

struct BBCmd
{
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
}; /* size = 0x8 */

struct BBScript
{
    s32 unk0;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u16 unk8;
    s8 unkA;
    s8 unkB;
    u16 unkC;
}; /* size = 0x10 */

static inline void set_sub0(u8 a, u8 b)
{
    u8 *p = (u8 *) &lbl_18_bss_970 + 0xA;
    p[4] = a;
    p[6] = b;
}

static inline void set_sub1(u8 a, u8 b)
{
    u8 *p = (u8 *) &lbl_18_bss_970 + 0xA;
    p[5] = a;
    p[7] = b;
}

static inline void set_sub2(u16 a)
{
    u16 *p = (u16 *) ((u8 *) &lbl_18_bss_970 + 0xA);
    p[4] = a;
}

static inline void set_sub3(u16 a)
{
    u16 *p = (u16 *) ((u8 *) &lbl_18_bss_970 + 0xA);
    p[5] = a;
}

void fn_18_26458(struct BBCpu *cpu);
void fn_18_26800(struct BBCpu *cpu);
void fn_18_26878(struct BBCpu *cpu);
void fn_18_26A6C(struct BBCpu *cpu);
void fn_18_26D20(struct BBCpu *cpu);
void fn_18_26EC0(struct BBCpu *cpu);
void fn_18_263E4(struct BBCpu *cpu);
void fn_18_26368(struct BBCpu *cpu, u8 arg1);
void fn_18_26504(struct BBCpu *cpu);
void fn_18_26564(struct BBCpu *cpu);
void fn_18_2675C(struct BBCpu *cpu);
void fn_18_268BC(struct BBCpu *cpu);
void fn_18_275D4(struct BBCpu *cpu);
void fn_18_277CC(struct BBCpu *cpu);
void fn_18_27968(struct BBCpu *cpu);
void fn_18_2717C(s16 *arg0);
void fn_18_271A4(s16 *arg0, s16 arg1, s16 arg2, s16 arg3);

struct BBCpu lbl_18_bss_1A330[3];
struct BBFlags lbl_18_bss_1A3B4;
s16 lbl_18_bss_1A3B8[16];
struct BBScript lbl_18_bss_1A3D8;


u32 lbl_18_data_82680[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000, 0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000, 0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000, 0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

void (*lbl_18_data_827F0[8])(struct BBCpu *) = {
    NULL, NULL, fn_18_26A6C, fn_18_26D20,
    NULL, NULL, NULL, NULL,
};

void (*lbl_18_data_82810[8])(struct BBCpu *) = {
    fn_18_26458, fn_18_26800, fn_18_26878, NULL,
    NULL, NULL, NULL, NULL,
};

void (*lbl_18_data_82830[5])(struct BBCpu *) = {
    NULL, fn_18_26EC0, NULL, NULL,
    NULL,
};

u8 lbl_18_data_82844[12] = {
    0x01, 0x06, 0x02, 0x07, 0x03, 0x08, 0x04, 0x09,
    0x05, 0x0A, 0x00, 0x00,
};

float lbl_18_data_82850[5][3] = {
    {0.300000012f, 0.400000006f, 0.300000012f},
    {0.300000012f, 0.5f, 0.200000003f},
    {0.400000006f, 0.5f, 0.100000001f},
    {0.5f, 0.400000006f, 0.100000001f},
    {0.600000024f, 0.300000012f, 0.100000001f},
};

u8 lbl_18_data_8288C[8] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
};

u8 lbl_18_data_82894[4] = {
    0x00, 0x04, 0x07, 0x00,
};

u8 lbl_18_data_82898[12] = {
    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
    0x13, 0x00, 0x00, 0x00,
};

u8 lbl_18_data_828A4[4] = {
    0x15, 0x16, 0x17, 0x18,
};

u8 lbl_18_data_828A8[8] = {
    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x00, 0x00, 0x00,
};

u8 lbl_18_data_828B0[4] = {
    0x1E, 0x1F, 0x20, 0x21,
};

u8 lbl_18_data_828B4[4] = {
    0x22, 0x23, 0x24, 0x00,
};

float lbl_18_data_828B8[5][4] = {
    {0.600000024f, 0.300000012f, 0.100000001f, 0.0f},
    {0.5f, 0.400000006f, 0.100000001f, 0.0f},
    {0.400000006f, 0.5f, 0.100000001f, 0.0f},
    {0.100000001f, 0.300000012f, 0.5f, 0.100000001f},
    {0.100000001f, 0.200000003f, 0.5f, 0.200000003f},
};

u8 lbl_18_data_82908[4] = {
    0x04, 0x05, 0x04, 0x03,
};

u8 lbl_18_data_8290C[8][9] = {
    {0x03, 0x00, 0x01, 0x01, 0x02, 0x01, 0x01, 0x00, 0x03},
    {0x03, 0x03, 0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02},
    {0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x03, 0x03},
    {0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {0x02, 0x00, 0x01, 0x03, 0x02, 0x03, 0x01, 0x00, 0x02},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x03, 0x01, 0x02, 0x02, 0x03, 0x02, 0x02, 0x01, 0x03},
};

u8 lbl_18_data_82954[8][9] = {
    {0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01},
    {0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01},
    {0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00},
    {0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00},
    {0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00},
    {0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00},
    {0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00},
};

float lbl_18_data_8299C[23] = {
    0.200000003f, 0.25f, 0.300000012f, 0.349999994f, 0.400000006f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 2.0f, 1.89999998f, 1.75f, 1.60000002f, 1.70000005f, 1.79999995f,
    2.0f, 2.04999995f, 0.5f, 0.0f, 0.0f, 0.0f, -0.0500000007f,
};

s16 lbl_18_data_829F8[] = {1, -40, 0, 30, 0, 0};

s16 lbl_18_data_82A04[] = {1, -40, 0, 25, 0, 0};

s16 lbl_18_data_82A10[] = {1, -40, 0, 20, 0, 0};

s16 lbl_18_data_82A1C[] = {1, -40, 0, 15, 0, 0};

s16 lbl_18_data_82A28[] = {1, -40, 0, 10, 0, 0};

s16 lbl_18_data_82A34[] = {1, 40, 0, 30, 0, 0};

s16 lbl_18_data_82A40[] = {1, 40, 0, 25, 0, 0};

s16 lbl_18_data_82A4C[] = {1, 40, 0, 20, 0, 0};

s16 lbl_18_data_82A58[] = {1, 40, 0, 15, 0, 0};

s16 lbl_18_data_82A64[] = {1, 40, 0, 10, 0, 0};

s16 lbl_18_data_82A70[] = {1, 0, -60, 60, 0, 0};

s16 lbl_18_data_82A7C[] = {1, 60, -60, 60, 0, 0};

s16 lbl_18_data_82A88[] = {1, -60, -60, 60, 0, 0};

s16 lbl_18_data_82A94[] = {1, 0, 60, 60, 0, 0};

s16 lbl_18_data_82AA0[] = {1, 60, 60, 60, 0, 0};

s16 lbl_18_data_82AAC[] = {1, -60, 60, 60, 0, 0};

s16 lbl_18_data_82AB8[] = {1, 0, 0, 60, 0, 0};

s16 lbl_18_data_82AC4[] = {1, 60, 0, 60, 0, 0};

s16 lbl_18_data_82AD0[] = {1, -60, 0, 60, 0, 0};

s16 lbl_18_data_82ADC[] = {3, 1, 0, 0};

s16 lbl_18_data_82AE4[] = {1, 0, -60, 30, 0, 0};

s16 lbl_18_data_82AF0[] = {1, 0, 60, 300, 0, 0};

s16 lbl_18_data_82AFC[] = {1, 60, -60, 30, 0, 0};

s16 lbl_18_data_82B08[] = {1, -60, -60, 30, 0, 0};

s16 lbl_18_data_82B14[] = {1, 0, -60, 8, 1, 0, 60, 60, 1, 0, -60, 10, 0, 0};

s16 lbl_18_data_82B30[] = {1, 0, -60, 15, 1, 0, 60, 20, 1, 0, -60, 10, 0, 0};

s16 lbl_18_data_82B4C[] = {1, 0, 60, 50, 1, 0, -60, 20, 0, 0};

s16 lbl_18_data_82B60[] = {1, 0, -60, 5, 1, 0, 60, 5, 1, 0, -60, 5, 1, 0, 60, 5, 1, 0, -60, 20, 0, 0};

s16 lbl_18_data_82B8C[] = {1, 0, -60, 15, 1, 0, 60, 60, 0, 0};

s16 lbl_18_data_82BA0[] = {1, 0, -60, 21, 1, 0, 60, 30, 0, 0};

s16 lbl_18_data_82BB4[] = {1, 0, 60, 40, 1, 0, -60, 8, 1, 0, 60, 60, 0, 0};

s16 lbl_18_data_82BD0[] = {1, 0, 60, 100, 1, 0, -60, 60, 1, 0, -60, 20, 0, 0};

s16 lbl_18_data_82BEC[] = {1, 0, -60, 15, 1, 0, 60, 40, 1, 0, -60, 30, 0, 0};

s16 lbl_18_data_82C08[] = {1, 60, 60, 40, 1, -60, -60, 9, 1, -60, 60, 60, 0, 0};

s16 lbl_18_data_82C24[] = {1, -60, -60, 14, 1, -60, 60, 40, 1, 60, -60, 30, 0, 0};

s16 lbl_18_data_82C40[] = {1, 0, -70, 40, 0, 0};

s16 lbl_18_data_82C4C[] = {3, 1, 0, 0};

s16 lbl_18_data_82C54[] = {1, 60, 0, 1, 0, 0};

s16 lbl_18_data_82C60[] = {1, -60, 0, 1, 0, 0};

s16 lbl_18_data_82C6C[] = {1, 0, 60, 1, 0, 0};

s16 lbl_18_data_82C78[] = {1, 0, -60, 1, 0, 0};

s16 lbl_18_data_82C84[] = {1, 60, 60, 1, 0, 0};

s16 lbl_18_data_82C90[] = {1, 60, -60, 1, 0, 0};

s16 lbl_18_data_82C9C[] = {1, -60, 60, 1, 0, 0};

s16 lbl_18_data_82CA8[] = {1, -60, -60, 1, 0, 0};

s16 lbl_18_data_82CB4[] = {1, 30, 0, 1, 0, 0};

s16 lbl_18_data_82CC0[] = {1, 60, 0, 4, 0, 0};

s16 lbl_18_data_82CCC[] = {1, 60, 0, 4, 0, 0};

s16 lbl_18_data_82CD8[] = {1, -30, 0, 1, 0, 0};

s16 lbl_18_data_82CE4[] = {1, -60, 0, 4, 0, 0};

s16 lbl_18_data_82CF0[] = {1, -60, 0, 4, 0, 0};

s16 lbl_18_data_82CFC[] = {1, 60, 0, 1, 0, 0};

s16 lbl_18_data_82D08[] = {1, -60, 0, 1, 0, 0};

s16 lbl_18_data_82D14[] = {1539, 1, 0, 0};

s16 lbl_18_data_82D1C[] = {1283, 1, 0, 0};

s16 *lbl_18_data_82D24[] = {
    NULL, lbl_18_data_829F8, lbl_18_data_82A04,
    lbl_18_data_82A10, lbl_18_data_82A1C, lbl_18_data_82A28,
    lbl_18_data_82A34, lbl_18_data_82A40, lbl_18_data_82A4C,
    lbl_18_data_82A58, lbl_18_data_82A64, lbl_18_data_82A70,
    lbl_18_data_82A7C, lbl_18_data_82A88, lbl_18_data_82A94,
    lbl_18_data_82AA0, lbl_18_data_82AAC, lbl_18_data_82AB8,
    lbl_18_data_82AC4, lbl_18_data_82AD0, lbl_18_data_82ADC,
    lbl_18_data_82AE4, lbl_18_data_82AF0, lbl_18_data_82AFC,
    lbl_18_data_82B08, lbl_18_data_82B14, lbl_18_data_82B30,
    lbl_18_data_82B4C, lbl_18_data_82B60, lbl_18_data_82B8C,
    lbl_18_data_82BA0, lbl_18_data_82BB4, lbl_18_data_82BD0,
    lbl_18_data_82BEC, lbl_18_data_82C08, lbl_18_data_82C24,
    lbl_18_data_82C40, lbl_18_data_82C4C, lbl_18_data_82C54,
    lbl_18_data_82C60, lbl_18_data_82C6C, lbl_18_data_82C78,
    lbl_18_data_82C84, lbl_18_data_82C90, lbl_18_data_82C9C,
    lbl_18_data_82CA8, lbl_18_data_82CB4, lbl_18_data_82CC0,
    lbl_18_data_82CCC, lbl_18_data_82CD8, lbl_18_data_82CE4,
    lbl_18_data_82CF0, lbl_18_data_82CFC, lbl_18_data_82D08,
    lbl_18_data_82D14, lbl_18_data_82D1C, lbl_18_bss_1A3B8,
};

void fn_18_260BC(void)
{
    memset(lbl_18_bss_1A330, 0, 0x84);
    memset(&lbl_18_bss_1A3B4, 0, 3);
    fn_18_275A0();
}

void fn_18_26104(void)
{
    struct BBCpu *cpu1 = &lbl_18_bss_1A330[1];
    struct BBCpu *cpu2 = &lbl_18_bss_1A330[2];
    u8 tr29;
    u8 tr28;
    u16 spA;
    u8 sp9;
    u8 sp8;

    tr28 = fn_18_A94C();
    tr29 = fn_18_C5F4(2);
    fn_18_263E4(cpu1);
    fn_18_26368(cpu1, tr28);
    if (lbl_18_data_82810[tr28] != NULL) {
        lbl_18_data_82810[tr28](cpu1);
    }
    fn_18_263E4(cpu2);
    fn_18_26368(cpu2, tr29);
    if (lbl_18_data_82830[tr29] != NULL) {
        lbl_18_data_82830[tr29](cpu2);
    }
    fn_18_27234();
    fn_18_272F0();
    fn_18_274E0((s8 *) &sp9, (s8 *) &sp8, &spA);
    cpu1->unk4++;
    cpu2->unk4++;
    set_sub1(sp9, sp8);
    set_sub3(spA);
}

void fn_18_2627C(void)
{
    struct BBCpu *cpu = &lbl_18_bss_1A330[0];
    u8 tr30;

    tr30 = fn_18_8FAC();
    fn_18_263E4(cpu);
    fn_18_26368(cpu, tr30);
    if (lbl_18_data_827F0[tr30] != NULL) {
        lbl_18_data_827F0[tr30](cpu);
    }
    fn_18_27234();
    fn_18_272F0();
    fn_18_274E0((s8 *) &cpu->unk8, (s8 *) &cpu->unk9, &cpu->unkA);
    cpu->unk4++;
    set_sub0(cpu->unk8, cpu->unk9);
    set_sub2(cpu->unkA);
}

void fn_18_26368(struct BBCpu *cpu, u8 arg1)
{
    if (cpu->unk0 != arg1) {
        fn_18_275A0();
        cpu->unk0 = arg1;
        cpu->unk1 = arg1;
        cpu->unk4 = 0;
        cpu->unk13 = 0;
        cpu->unk18 = 0;
        cpu->unk14 = 0;
        cpu->unkC = 0;
    }
}

void fn_18_263E4(struct BBCpu *cpu)
{
    cpu->unk8 = 0;
    cpu->unk9 = 0;
    cpu->unkA = 0;
}

void fn_18_26400(void)
{
    struct BBFlags *p = &lbl_18_bss_1A3B4;

    p->unk2 = p->unk1;
    p->unk1 = fn_18_1988C();
}

void fn_18_26440(void)
{
    struct BBFlags *p = &lbl_18_bss_1A3B4;

    p->unk0 = 0;
}

void fn_18_26458(struct BBCpu *cpu)
{
    s16 r;

    if (cpu->unk4 >= 0x14) {
        fn_18_26504(cpu);
        if (cpu->unk4 == 0x19) {
            r = rand() & 0x7FFF;
            r = __abs(r) % 10;
            fn_18_27118(lbl_18_data_82844[r]);
        }
        if (cpu->unk4 == 0x64) {
            fn_18_27118(0x14);
        }
    }
}

void fn_18_26504(struct BBCpu *cpu)
{
    switch (cpu->unkC) {
    case 0:
        fn_18_26564(cpu);
        cpu->unkC = 1;
        break;
    case 1:
        fn_18_2675C(cpu);
        break;
    }
}

void fn_18_26564(struct BBCpu *cpu)
{
    float tf1;
    float f2;
    s32 r5;
    u8 tr29;
    u8 tr28;
    struct BBFlags *p = &lbl_18_bss_1A3B4;
    u8 r25;
    s16 r;

    tr29 = fn_18_14AF4();
    tr28 = fn_18_1980();
    r25 = 0;
    if (p->unk0 == 0) {
        p->unk0 = 1;
        r25 = 1;
    }
    if (p->unk1 != p->unk2) {
        r25 = 1;
    }
    tf1 = fabs((float) rand() / 32767.0f);
    if (tf1 < 0.1) {
        r25 = 1;
    }
    if (r25 != 0) {
        tf1 = fabs((float) rand() / 32767.0f);
        f2 = 0.0f;
        for (r5 = 0; r5 < 2; r5++) {
            f2 += lbl_18_data_82850[tr28][r5];
            if (tf1 < f2) {
                break;
            }
        }
        switch ((u8) r5) {
        case 0:
            cpu->unkD = lbl_18_data_8288C[(u8) tr29];
            return;
        case 1:
            r = rand() & 0x7FFF;
            r = __abs(r) % 3;
            cpu->unkD = lbl_18_data_82894[r];
            return;
        default:
            r = rand() & 0x7FFF;
            r = __abs(r) % 8;
            cpu->unkD = r;
            break;
        }
    }
}

void fn_18_2675C(struct BBCpu *cpu)
{
    s16 diff;
    u8 tr3;

    tr3 = fn_18_E0AC();
    if (tr3 != cpu->unkD) {
        diff = cpu->unkD - tr3;
        if (__abs(diff) > 4) {
            if (diff > 0) {
                fn_18_27118(0x37);
                return;
            }
            fn_18_27118(0x36);
            return;
        }
        if (diff > 0) {
            fn_18_27118(0x36);
            return;
        }
        fn_18_27118(0x37);
    }
}

void fn_18_26800(struct BBCpu *cpu)
{
    s16 r;

    if (cpu->unk4 == 0) {
        r = rand() & 0x7FFF;
        r = __abs(r) % 9;
        fn_18_27118(lbl_18_data_82898[r]);
    }
}

void fn_18_26878(struct BBCpu *cpu)
{
    if (cpu->unk4 == 5) {
        fn_18_268BC(cpu);
        fn_18_27118(cpu->unk11);
    }
}

void fn_18_268BC(struct BBCpu *cpu)
{
    float tf1;
    float f31;
    u8 tr30;
    s16 r29;
    s32 r5;
    u8 tr3;

    tr30 = fn_18_1980();
    r29 = -1;
    f31 = 0.0f;
    tf1 = fabs((float) rand() / 32767.0f);
    for (r5 = 0; r5 < 4; r5++) {
        f31 += lbl_18_data_828B8[tr30][r5];
        if (tf1 <= f31) {
            r29 = r5;
            break;
        }
    }
    if (r29 == -1) {
        r29 = 0;
    }
    tr3 = __abs((s16) (rand() & 0x7FFF)) % lbl_18_data_82908[r29];
    switch (r29) {
    default:
    case 0:
        cpu->unk11 = lbl_18_data_828A4[tr3];
        return;
    case 1:
        cpu->unk11 = lbl_18_data_828A8[tr3];
        return;
    case 2:
        cpu->unk11 = lbl_18_data_828B0[tr3];
        return;
    case 3:
        cpu->unk11 = lbl_18_data_828B4[tr3];
        return;
    }
}

void fn_18_26A6C(struct BBCpu *cpu)
{
    u8 sp10[6];
    u8 sp8[6];
    float tf1;
    u8 tr30;
    s32 r4;
    s32 r6;
    u8 r29;
    u8 tr28;
    u8 tr3;
    u8 t;
    s16 r;

    tr28 = fn_18_1980();
    tr30 = fn_18_14AF4();
    tr3 = fn_18_E0AC();
    r29 = 0;
    for (r4 = 0; r4 < 9; r4++) {
        if (lbl_18_data_8290C[(u8) tr30][r4] != 0 && lbl_18_data_82954[tr3][r4] == 0) {
            sp10[r29] = 8 - r4;
            sp8[r29] = lbl_18_data_8290C[(u8) tr30][r4];
            r29++;
            if (r29 == 5) {
                break;
            }
        }
    }
    for (r6 = 0; r6 < r29 - 1; r6++) {
        for (r4 = r6; r4 < r29 - 1; r4++) {
            if (sp8[r4] < sp8[r4 + 1]) {
                t = sp8[r4];
                sp8[r4] = sp8[r4 + 1];
                sp8[r4 + 1] = t;
                t = sp10[r4];
                sp10[r4] = sp10[r4 + 1];
                sp10[r4 + 1] = t;
            }
        }
    }
    if (r29 != 0) {
        tf1 = fabs((float) rand() / 32767.0f);
        for (r4 = 0; r4 < r29; r4++) {
            if (tf1 < lbl_18_data_8299C[tr28]) {
                break;
            }
        }
        if (r29 == r4) {
            r = rand() & 0x7FFF;
            r = __abs(r) % 9;
            cpu->unk12 = r;
        } else {
            cpu->unk12 = sp10[r4];
        }
    } else {
        r = rand() & 0x7FFF;
        r = __abs(r) % 9;
        cpu->unk12 = r;
    }
    rand();
    fn_18_275D4(cpu);
    fn_18_277CC(cpu);
    cpu->unk1C = 1;
}

void fn_18_26D20(struct BBCpu *cpu)
{
    float tf2;
    float f3;
    struct Ball *tr29 = &balls[1];
    struct Ball *ball0 = balls;
    s16 r27;
    s16 r26;
    u8 r25;

    r25 = 0;
    r27 = 0;
    if (cpu->unk1E != 0) {
        cpu->unk1E--;
        r27 = cpu->unk20;
        r25 = 1;
    }
    if (fn_18_A000() == 0) {
        f3 = 1.2f;
    } else {
        f3 = -1.2f;
    }
    tf2 = (cpu->unk24 + (ball0->pos.x + f3)) - tr29->pos.x;
    if (fabs(tf2) > cpu->unk28) {
        cpu->unk1C = 1;
    }
    r26 = 0;
    if (cpu->unk1C == 1) {
        if (fabs(tf2) >= 0.01) {
            if ((cpu->unk24 + (ball0->pos.x + f3)) > tr29->pos.x) {
                r26 = -0x28;
            } else {
                r26 = 0x28;
            }
            r25 = 1;
        } else {
            cpu->unk1C = 0;
        }
    }
    if (r25 == 1) {
        fn_18_2717C(lbl_18_bss_1A3B8);
        fn_18_271A4(lbl_18_bss_1A3B8, r26, r27, 1);
        fn_18_27118(0x38);
    }
    fn_18_27968(cpu);
    if (cpu->unk13 != 0 && cpu->unk18 != 0) {
        cpu->unk18--;
        if (cpu->unk18 == 0) {
            cpu->unk13 = 0;
            fn_18_27148(0x25);
        }
    }
}

void fn_18_26EC0(struct BBCpu *cpu)
{
    float sp10[4];
    u8 spC[4];
    u8 sp8[4];
    register float rx;
    register float rz;
    struct Ball *tr30 = &balls[1];
    s32 r29;
    struct Ball *tr3;
    u8 r28;

    if (fn_18_3F08() == 1) {
        r28 = 0;
        for (r29 = 0; r29 < 4; r29++) {
            tr3 = &balls[r29 + 2];
            if (tr30->pos.z > tr3->pos.z - 0.2) {
                sp8[r28] = r29;
                r28++;
            }
        }
        if (r28 == 0) {
            for (r29 = 0; r29 < 4; r29++) {
                tr3 = &balls[r29 + 2];
                {
                    rx = tr30->pos.x - tr3->pos.x;
                    rz = tr30->pos.z - tr3->pos.z;
#ifdef __MWERKS__
                    asm
                    {
                        fmuls rx, rx, rx
                        fmadds rx, rz, rz, rx
                    }
#else
                    rx = rx * rx + rz * rz;
#endif
                    sp10[r29] = math_sqrt(rx);
                }
                spC[r29] = r29;
                sp8[r28] = r29;
                r28++;
            }
        } else {
            for (r29 = 0; r29 < r28; r29++) {
                tr3 = &balls[sp8[r29] + 2];
                {
                    rx = tr30->pos.x - tr3->pos.x;
                    rz = tr30->pos.z - tr3->pos.z;
#ifdef __MWERKS__
                    asm
                    {
                        fmuls rx, rx, rx
                        fmadds rx, rz, rz, rx
                    }
#else
                    rx = rx * rx + rz * rz;
#endif
                    sp10[r29] = math_sqrt(rx);
                }
                spC[r29] = r29;
            }
        }
        for (r29 = 0; r29 < r28; r29++) {
            if (sp10[0] > sp10[r29 + 1]) {
                sp10[0] = sp10[r29 + 1];
                spC[0] = spC[r29 + 1];
            }
        }
        tr3 = &balls[sp8[spC[0]] + 2];
        {
            rx = tr30->pos.x - tr3->pos.x;

            if (fabs(rx) > 0.4) {
                if (rx > 0.0) {
                    fn_18_27118(0x34);
                    return;
                }
                fn_18_27118(0x35);
            }
        }
    }
}

void fn_18_27118(u8 arg0)
{
    struct BBScript *cmd = &lbl_18_bss_1A3D8;

    if (!(cmd->unk0 & 8)) {
        cmd->unk0 |= 1;
        cmd->unk6 = arg0;
    }
}

void fn_18_27148(u8 arg0)
{
    struct BBScript *cmd = &lbl_18_bss_1A3D8;

    cmd->unk0 |= 8;
    cmd->unkC = 0;
    cmd->unk0 &= ~2;
    cmd->unk6 = arg0;
}

void fn_18_2717C(s16 *arg0)
{
    memset(arg0, 0, 0x10);
}

void fn_18_271A4(s16 *arg0, s16 arg1, s16 arg2, s16 arg3)
{
    s16 r8;

    for (r8 = 0; r8 < 0x10; r8++) {
        if (arg0[r8] == 0) {
            break;
        }
    }
    if (r8 <= 0xB) {
        arg0[r8] = 1;
        arg0[r8 + 1] = arg1;
        arg0[r8 + 2] = arg2;
        arg0[r8 + 3] = arg3;
        arg0[r8 + 4] = 0;
    }
}

void fn_18_27234(void)
{
    struct BBScript *cmd = &lbl_18_bss_1A3D8;

    if (cmd->unk0 & 8) {
        cmd->unk0 &= ~4;
        cmd->unk0 &= ~2;
        cmd->unk5 = cmd->unk4;
        cmd->unk4 = cmd->unk6;
        cmd->unk7 = 0;
        cmd->unkC = 0;
        cmd->unkA = 0;
        cmd->unkB = 0;
        cmd->unk8 = 0;
        return;
    }
    if (cmd->unkC == 0 && !(cmd->unk0 & 2) && (cmd->unk0 & 1)) {
        cmd->unk0 &= ~1;
        cmd->unk4 = cmd->unk6;
        cmd->unk7 = 0;
    }
}

void fn_18_272F0(void)
{
    struct BBScript *cmd = &lbl_18_bss_1A3D8;
    s16 op;
    s16 arg;
    s16 *p;

    if (cmd->unk4 != 0) {
        p = lbl_18_data_82D24[cmd->unk4] + cmd->unk7;
        op = *p & 0xFF;
        arg = (*p & 0xFF00) >> 8;
        if (cmd->unkC == 0) {
            cmd->unk0 |= 2;
            switch (op) {
            case 2:
                break;
            case 0:
                cmd->unk4 = 0;
                cmd->unk7 = 0;
                break;
            case 1:
                cmd->unkA = p[1];
                cmd->unkB = p[2];
                cmd->unkC = p[3];
                cmd->unk7 += 4;
                break;
            case 3:
                switch (arg) {
                case 0:
                    cmd->unk8 |= 0x100;
                    break;
                case 1:
                    cmd->unk8 |= 0x200;
                    break;
                case 2:
                    cmd->unk8 |= 0x400;
                    break;
                case 3:
                    cmd->unk8 |= 0x800;
                    break;
                case 4:
                    cmd->unk8 |= 0x10;
                    break;
                case 5:
                    cmd->unk8 |= 0x40;
                    break;
                case 6:
                    cmd->unk8 |= 0x20;
                    break;
                }
                cmd->unkC = p[1];
                cmd->unk7 += 2;
                break;
            }
            p = lbl_18_data_82D24[cmd->unk4] + cmd->unk7;
            arg = *p & 0xFF;
            if (arg == 0) {
                cmd->unk0 |= 4;
            }
        }
    }
}

float lbl_18_data_82E24[5][2] = {
    {0.333299994f, 0.333299994f},
    {0.333299994f, 0.333299994f},
    {0.333299994f, 0.333299994f},
    {0.333299994f, 0.333299994f},
    {0.333299994f, 0.333299994f},
};

u8 lbl_18_data_82E4C[4] = {
    0x3C, 0x3C, 0x0A, 0x00,
};

s16 lbl_18_data_82E50[4] = {40, -40, 40, 0};

float lbl_18_data_82E58[5][2] = {
    {0.333299994f, 0.333299994f},
    {0.333299994f, 0.333299994f},
    {0.333299994f, 0.333299994f},
    {0.333299994f, 0.333299994f},
    {0.333299994f, 0.333299994f},
};

float lbl_18_data_82E80[3] = {
    0.200000003f, -0.200000003f, 0.0f,
};

float lbl_18_data_82E8C[5] = {
    0.150000006f, 0.100000001f, 0.0500000007f, 0.0299999993f, 0.0f,
};

float lbl_18_data_82EA0[5] = {
    0.150000006f, 0.119999997f, 0.100000001f, 0.0500000007f, 0.0299999993f,
};

float lbl_18_data_82EB4[2][9] = {
    {0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 2.0f, 1.89999998f, 1.75f, 1.60000002f},
    {1.70000005f, 1.79999995f, 2.0f, 2.04999995f, 0.5f, 0.0f, 0.0f, 0.0f, -0.0500000007f},
};

float lbl_18_data_82EFC[5][3] = {
    {7.0f, 6.0f, 5.0f},
    {6.0f, 6.5f, 4.80000019f},
    {5.0f, 4.5f, 4.0f},
    {4.5f, 4.0f, 3.79999995f},
    {4.0f, 3.79999995f, 3.5f},
};

float lbl_18_data_82F38[5] = {
    0.100000001f, 0.200000003f, 0.300000012f, 0.400000006f, 0.5f,
};

void fn_18_274E0(s8 *arg0, s8 *arg1, u16 *arg2)
{
    struct BBScript *cmd = &lbl_18_bss_1A3D8;

    *arg0 = 0;
    *arg1 = 0;
    *arg2 = 0;
    if (cmd->unk4 != 0 && cmd->unkC != 0) {
        *arg0 = cmd->unkA;
        *arg1 = cmd->unkB;
        *arg2 = cmd->unk8;
        cmd->unkC--;
        if (cmd->unkC == 0) {
            cmd->unkA = 0;
            cmd->unkB = 0;
            cmd->unk8 = 0;
            if (cmd->unk0 & 4) {
                cmd->unk0 &= ~4;
                cmd->unk5 = cmd->unk4;
                cmd->unk4 = 0;
                cmd->unk0 &= ~2;
            }
        }
    }
}

void fn_18_275A0(void)
{
    struct BBScript *cmd = &lbl_18_bss_1A3D8;

    memset(cmd, 0, 0x10);
}

void fn_18_275D4(struct BBCpu *cpu)
{
    float tf2;
    float f1;
    u8 tr29;
    s32 r5;

    tr29 = fn_18_1980();
    tf2 = fabs((float) rand() / 32767.0f);
    for (r5 = 0; r5 < 3; r5++) {
        f1 = 0.0f;
        if (r5 == 2) {
            cpu->unk1D = r5;
            break;
        }
        f1 += lbl_18_data_82E24[tr29][r5];
        if (tf2 <= f1) {
            cpu->unk1D = r5;
            break;
        }
    }
    tf2 = fabs((float) rand() / 32767.0f);
    cpu->unk1E = (float) lbl_18_data_82E4C[cpu->unk1D] * tf2;
    tf2 = fabs((float) rand() / 32767.0f);
    cpu->unk20 = tf2 * (float) lbl_18_data_82E50[cpu->unk1D];
}

void fn_18_277CC(struct BBCpu *cpu)
{
    float tf1;
    float f31;
    s32 r5;
    u8 tr29;
    u8 r28;

    tr29 = fn_18_1980();
    f31 = 0.0f;
    r28 = 0;
    tf1 = fabs((float) rand() / 32767.0f);
    for (r5 = 0; r5 < 3; r5++) {
        if (r5 == 2) {
            r28 = r5;
            break;
        }
        f31 += lbl_18_data_82E58[tr29][r5];
        if (tf1 <= f31) {
            r28 = r5;
            break;
        }
    }
    tf1 = fabs((float) rand() / 32767.0f);
    f31 = tf1 * lbl_18_data_82E8C[tr29];
    tf1 = fabs((float) rand() / 32767.0f);
    if (tf1 < 0.5) {
        f31 *= -1.0;
    }
    cpu->unk24 = f31 + lbl_18_data_82E80[r28];
    cpu->unk28 = lbl_18_data_82EA0[tr29];
}

void fn_18_27968(struct BBCpu *cpu)
{
    float f1;
    float f2;
    s32 r5;
    struct Ball *tr29 = &balls[1];
    struct Ball *ball0 = balls;
    u8 tr26;
    u8 r6;

    tr26 = fn_18_1980();
    if (0.0 != tr29->vel.z) {
        if (fabs(ball0->pos.z - tr29->pos.z) < 2.3 && fabs(tr29->pos.x) > 0.8500000000000001) {
            cpu->unk13 = 0;
            return;
        }
        if (cpu->unk14 < 3 && (ball0->pos.z - tr29->pos.z) < lbl_18_data_82EFC[tr26][cpu->unk14]) {
            if (cpu->unk14 != 0) {
                f1 = fabs((float) rand() / 32767.0f);
                if (lbl_18_data_82F38[tr26] < f1) {
                    cpu->unk14++;
                    return;
                }
            }
            cpu->unk14++;
            if (fn_18_A000() == 0) {
                r6 = 0;
            } else {
                r6 = 1;
            }
            r5 = 0;
            f2 = tr29->pos.z;
            do {
                f2 += tr29->vel.z;
                r5++;
            } while (!((ball0->pos.z - f2) < lbl_18_data_82EB4[r6][cpu->unk12]));
            cpu->unk13 = 1;
            if (r5 - 7 <= 0) {
                cpu->unk18 = 1;
                return;
            }
            cpu->unk18 = r5 - 7;
        }
    }
}

