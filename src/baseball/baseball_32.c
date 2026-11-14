#include "baseball.h"
#include "bitmap.h"
#include "input.h"

#define PAD_BUTTON_B_ 0x200
#define PAD_REPEAT(pad, btn) \
( \
    (pad_status_groups[pad].unk0[4].button & (btn)) \
 || (analog_inputs[pad][4] & (btn)) \
 || ( \
        ((pad_status_groups[pad].unk0[0].button & (btn)) || (analog_inputs[pad][0] & (btn))) \
     && (analog_inputs[pad][0] & PAD_BUTTON_B_) \
    ) \
)

struct HowToLabel
{
    s32 bmpId;
    s16 lineY[18];
};

struct HowToLine
{
    s8 unk0;
    s16 unk2;
    s16 unk4;
    char **unk8;
};

struct HowToArrow
{
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    u32 unk10;
};

u32 lbl_18_data_80310[] = {
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

char lbl_18_data_80480[] = "how to";

s8 lbl_18_data_80488[2][2] = {{5, 4}, {4, 2}};

char lbl_18_data_8048C[] = "Monkey Baseball controls";

char lbl_18_data_804A8[] = "";

char lbl_18_data_804AC[] = "\203\202\203\223\203L\201[\203x\201[\203X\203{\201[\203\213\202\314\202\273\202\244\202\263";

char lbl_18_data_804CC[] = "Exhibition Mode Rules";

char lbl_18_data_804E4[] = "\203G\203L\203V\203r\203W\203\207\203\223\203\202\201[\203h\202\314\203\213\201[\203\213";

char lbl_18_data_80504[] = "Tournament Mode Rules";

char lbl_18_data_8051C[] = "\203g\201[\203i\203\201\203\223\203g\203\202\201[\203h\202\314\203\213\201[\203\213";

char lbl_18_data_80538[] = "About the Wheel (Part1)";

char lbl_18_data_80550[] = "\203\213\201[\203\214\203b\203g\202\311\202\302\202\242\202\304 (\202\273\202\314\202P)";

char lbl_18_data_8056C[] = "About the Wheel (Part2)";

char lbl_18_data_80584[] = "\203\213\201[\203\214\203b\203g\202\311\202\302\202\242\202\304 (\202\273\202\314\202Q)";

char *lbl_18_data_805A0[5][6] = {
    {lbl_18_data_8048C, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804AC},
    {lbl_18_data_804CC, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804E4},
    {lbl_18_data_80504, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_8051C},
    {lbl_18_data_80538, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_80550},
    {lbl_18_data_8056C, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_80584},
};

char lbl_18_data_80618[] = "Monkey Tennis Controls";

char lbl_18_data_80630[] = "\203\202\203\223\203L\201[\203e\203j\203X\202\314\202\273\202\244\202\263";

char lbl_18_data_80648[] = "Singles Mode Rules";

char lbl_18_data_8065C[] = "\203V\203\223\203O\203\213\203X\203\202\201[\203h\202\314\203\213\201[\203\213";

char lbl_18_data_80678[] = "Doubles Mode Rules";

char lbl_18_data_8068C[] = "\203_\203u\203\213\203X\203\202\201[\203h\202\314\203\213\201[\203\213";

char *lbl_18_data_806A4[4][6] = {
    {lbl_18_data_80618, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_80630},
    {lbl_18_data_80648, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_8065C},
    {lbl_18_data_80678, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_8068C},
    {lbl_18_data_80504, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_8051C},
};

char *(*lbl_18_data_80704[2])[6] = {lbl_18_data_805A0, lbl_18_data_806A4};

struct HowToLabel lbl_18_data_8070C[22] = {
    {0xC24, {5, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC23, {5, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC26, {5, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC22, {5, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC25, {5, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC19, {5, 26, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC1A, {5, 26, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC1C, {5, 26, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC1B, {5, 26, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC1E, {5, 26, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC1D, {5, 26, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC29, {5, 26, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC20, {5, 26, 49, 67, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC1F, {5, 26, 44, 67, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC21, {5, 26, 44, 67, 85, 103, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC43, {5, 26, 44, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC29, {5, 22, 35, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC1F, {5, 22, 35, 48, 63, 76, 89, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC21, {5, 22, 35, 48, 63, 78, 93, 108, 123, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC36, {5, 26, 44, 67, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0xC44, {5, 27, 44, 61, 87, 104, 121, 138, 155, 172, 189, 206, 236, 253, 270, 287, 304, 321}},
    {0xC32, {5, 26, 44, 67, 85, 103, 121, 139, 162, 180, 198, 0, 0, 0, 0, 0, 0, 0}},
};

char lbl_18_data_80A7C[] = "START/PAUSE";

char lbl_18_data_80A88[] = "Pause Menu";

char *lbl_18_data_80A94[] = {lbl_18_data_80A7C, lbl_18_data_80A88, NULL};

char lbl_18_data_80AA0[] = "Pausenmen\374";

char *lbl_18_data_80AAC[] = {lbl_18_data_80A7C, lbl_18_data_80AA0, NULL};

char lbl_18_data_80AB8[] = "Menu pause";

char *lbl_18_data_80AC4[] = {lbl_18_data_80A7C, lbl_18_data_80AB8, NULL};

char lbl_18_data_80AD0[] = "Men\372 Pausa";

char *lbl_18_data_80ADC[] = {lbl_18_data_80A7C, lbl_18_data_80AD0, NULL};

char lbl_18_data_80AE8[] = "Menu pausa";

char *lbl_18_data_80AF4[] = {lbl_18_data_80A7C, lbl_18_data_80AE8, NULL};

char lbl_18_data_80B00[] = "\203X\203^\201[\203g\203{\203^\203\223";

char lbl_18_data_80B10[] = "\203|\201[\203Y\203\201\203j\203\205\201[";

char *lbl_18_data_80B20[] = {lbl_18_data_80B00, lbl_18_data_80B10, NULL};

char lbl_18_data_80B2C[] = "\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N";

char lbl_18_data_80B44[] = "/bcff0000/\202\310\202\260\202\351\202\334\202\246";

char lbl_18_data_80B5C[] = "\203|\203W\203V\203\207\203\223\202\360\202\244\202\262\202\251\202\267";

char lbl_18_data_80B74[] = "/bcff0000/\202\310\202\260\202\275\202\240\202\306";

char lbl_18_data_80B8C[] = "\201`\203s\203b\203`\203\203\201[\202\314\203{\201[\203\213\201`";

char lbl_18_data_80BA4[] = "\202\244\202\246\202\305\203X\203\215\201[\203{\201[\203\213";

char lbl_18_data_80BB8[] = "\202\265\202\275\202\305\203X\203s\201[\203h\203{\201[\203\213";

char lbl_18_data_80BD0[] = "\202\263\202\344\202\244\202\305\203R\203\223\203g\203\215\201[\203\213";

char lbl_18_data_80BE8[] = "/bcff0000/\202\244\202\275\202\352\202\275\202\240\202\306";

char lbl_18_data_80C00[] = "\201`\203s\203b\203`\203\203\201[\202\252\202\355\201`";

char lbl_18_data_80C14[] = "\202\265\202\343\202\321\202\360\202\244\202\262\202\251\202\265\202\304\203{\201[\203\213\202\360\202\306\202\351";

char *lbl_18_data_80C34[] = {lbl_18_data_80B2C, lbl_18_data_80B44, lbl_18_data_80B5C, lbl_18_data_80B74, lbl_18_data_80B8C, lbl_18_data_80BA4, lbl_18_data_80BB8, lbl_18_data_80BD0, lbl_18_data_80BE8, lbl_18_data_80C00, lbl_18_data_80C14, NULL};

char lbl_18_data_80C64[] = "Control Stick";

char lbl_18_data_80C74[] = "/bcff0000/Before throwing";

char lbl_18_data_80C90[] = "/bcff0000/the ball";

char lbl_18_data_80CA4[] = "Position change";

char lbl_18_data_80CB4[] = "/bcff0000/After throwing";

char lbl_18_data_80CD0[] = "-Pitch ball-";

char lbl_18_data_80CE0[] = "Push up for Slow ball";

char lbl_18_data_80CF8[] = "Push down for";

char lbl_18_data_80D08[] = "Speed ball";

char lbl_18_data_80D14[] = "Push right or left for";

char lbl_18_data_80D2C[] = "Direction controls";

char lbl_18_data_80D40[] = "/bcff0000/After the ball";

char lbl_18_data_80D5C[] = "/bcff0000/gets hit";

char lbl_18_data_80D70[] = "-Defense side-";

char lbl_18_data_80D80[] = "Change Defense";

char lbl_18_data_80D90[] = "position to";

char lbl_18_data_80D9C[] = "catch the ball";

char *lbl_18_data_80DAC[] = {lbl_18_data_80C64, lbl_18_data_80C74, lbl_18_data_80C90, lbl_18_data_80CA4, lbl_18_data_80CB4, lbl_18_data_80C90, lbl_18_data_80CD0, lbl_18_data_80CE0, lbl_18_data_80CF8, lbl_18_data_80D08, lbl_18_data_80D14, lbl_18_data_80D2C, lbl_18_data_80D40, lbl_18_data_80D5C, lbl_18_data_80D70, lbl_18_data_80D80, lbl_18_data_80D90, lbl_18_data_80D9C, NULL};

char lbl_18_data_80DF8[] = "\202`\203{\203^\203\223";

char lbl_18_data_80E04[] = "/bcff0000/\203s\203b\203`\203\203\201[";

char lbl_18_data_80E1C[] = "\203{\201[\203\213\202\360\202\310\202\260\202\351";

char lbl_18_data_80E2C[] = "/bcff0000/\203o\203b\203^\201[";

char lbl_18_data_80E40[] = "\203{\201[\203\213\202\360\202\244\202\302";

char *lbl_18_data_80E50[] = {lbl_18_data_80DF8, lbl_18_data_80E04, lbl_18_data_80E1C, lbl_18_data_80E2C, lbl_18_data_80E40, NULL};

char lbl_18_data_80E68[] = "A Button";

char lbl_18_data_80E74[] = "/bcff0000/Pitcher";

char lbl_18_data_80E88[] = "Throw the ball.";

char lbl_18_data_80E98[] = "/bcff0000/Batter";

char lbl_18_data_80EAC[] = "Hit the ball.";

char *lbl_18_data_80EBC[] = {lbl_18_data_80E68, lbl_18_data_80E74, lbl_18_data_80E88, lbl_18_data_80E98, lbl_18_data_80EAC, NULL};

char lbl_18_data_80ED4[] = "\202k\201E\202q\203g\203\212\203K\201[\203{\203^\203\223";

char lbl_18_data_80EEC[] = "/bcff0000/\203s\203b\203`\203\203\201[\202\252\202\355";

char lbl_18_data_80F08[] = "\202\265\202\343\202\321\202\242\202\277\202\360\202\244\202\262\202\251\202\267";

char *lbl_18_data_80F20[] = {lbl_18_data_80ED4, lbl_18_data_80EEC, lbl_18_data_80F08, NULL};

char lbl_18_data_80F30[] = "L,R Button";

char lbl_18_data_80F3C[] = "/bcff0000/Pitcher Side";

char lbl_18_data_80F54[] = "Change your defense position";

char *lbl_18_data_80F74[] = {lbl_18_data_80F30, lbl_18_data_80F3C, lbl_18_data_80F54, NULL};

char lbl_18_data_80F84[] = "\203L\203\203\203\211\202\360\202\244\202\262\202\251\202\267";

char lbl_18_data_80F98[] = "\202\242\202\352\202\275\202\331\202\244\202\261\202\244\202\311";

char *lbl_18_data_80FAC[] = {lbl_18_data_80B2C, lbl_18_data_80F84, lbl_18_data_804A8, lbl_18_data_80F98, lbl_18_data_80E40, NULL};

char lbl_18_data_80FC4[] = "Character";

char lbl_18_data_80FD0[] = "movement";

char lbl_18_data_80FDC[] = "Hit the ball in";

char lbl_18_data_80FEC[] = "the direction";

char lbl_18_data_80FFC[] = "you press the";

char lbl_18_data_8100C[] = "Control Stick.";

char *lbl_18_data_8101C[] = {lbl_18_data_80C64, lbl_18_data_80FC4, lbl_18_data_80FD0, lbl_18_data_80FDC, lbl_18_data_80FEC, lbl_18_data_80FFC, lbl_18_data_8100C, NULL};

char lbl_18_data_8103C[] = "\203g\203b\203v\203X\203s\203\223";

char lbl_18_data_8104C[] = "\201i\202\315\202\342\202\255\202\304\201A\202\275\202\251\202\242\201j";

char *lbl_18_data_81064[] = {lbl_18_data_80DF8, lbl_18_data_8103C, lbl_18_data_8104C, NULL};

char lbl_18_data_81074[] = "Top Spin";

char lbl_18_data_81080[] = "(Fast and High)";

char *lbl_18_data_81090[] = {lbl_18_data_80E68, lbl_18_data_81074, lbl_18_data_81080, NULL};

char lbl_18_data_810A0[] = "\202a\203{\203^\203\223";

char lbl_18_data_810AC[] = "\203X\203\211\203C\203X\203X\203s\203\223";

char lbl_18_data_810BC[] = "\201i\202\250\202\273\202\255\202\304\201A\202\320\202\255\202\242\201j";

char *lbl_18_data_810D4[] = {lbl_18_data_810A0, lbl_18_data_810AC, lbl_18_data_810BC, NULL};

char lbl_18_data_810E4[] = "B Button";

char lbl_18_data_810F0[] = "Slice Spin";

char lbl_18_data_810FC[] = "(Slow and Low)";

char *lbl_18_data_8110C[] = {lbl_18_data_810E4, lbl_18_data_810F0, lbl_18_data_810FC, NULL};

char lbl_18_data_8111C[] = "\202w\203{\203^\203\223";

char lbl_18_data_81128[] = "\203\215\203u";

char lbl_18_data_81130[] = "\201i\202\342\202\334\202\310\202\350\202\314\203{\201[\203\213\201j";

char *lbl_18_data_81148[] = {lbl_18_data_8111C, lbl_18_data_81128, lbl_18_data_81130, NULL};

char lbl_18_data_81158[] = "X Button";

char lbl_18_data_81164[] = "Lob";

char *lbl_18_data_81168[] = {lbl_18_data_81158, lbl_18_data_81164, NULL};

char lbl_18_data_81174[] = "\202x\203{\203^\203\223";

char lbl_18_data_81180[] = "\203X\203R\203A\202\360\202\335\202\351";

char *lbl_18_data_81190[] = {lbl_18_data_81174, lbl_18_data_81180, NULL};

char lbl_18_data_8119C[] = "Y Button";

char lbl_18_data_811A8[] = "View score";

char *lbl_18_data_811B4[] = {lbl_18_data_8119C, lbl_18_data_811A8, NULL};

struct HowToLine lbl_18_data_811C0[11][6] = {
    {
        {0, 243, 343, lbl_18_data_80A94},
        {0, 243, 343, lbl_18_data_80AAC},
        {0, 243, 343, lbl_18_data_80AC4},
        {0, 243, 343, lbl_18_data_80ADC},
        {0, 243, 343, lbl_18_data_80AF4},
        {0, 243, 343, lbl_18_data_80B20},
    },
    {
        {14, 15, 135, lbl_18_data_8101C},
        {14, 15, 135, lbl_18_data_8101C},
        {14, 15, 135, lbl_18_data_8101C},
        {14, 15, 135, lbl_18_data_8101C},
        {14, 15, 135, lbl_18_data_8101C},
        {13, 15, 135, lbl_18_data_80FAC},
    },
    {
        {8, 471, 247, lbl_18_data_81090},
        {8, 471, 247, lbl_18_data_81090},
        {8, 471, 247, lbl_18_data_81090},
        {8, 471, 247, lbl_18_data_81090},
        {8, 471, 247, lbl_18_data_81090},
        {8, 471, 247, lbl_18_data_81064},
    },
    {
        {9, 471, 335, lbl_18_data_8110C},
        {9, 471, 335, lbl_18_data_8110C},
        {9, 471, 335, lbl_18_data_8110C},
        {9, 471, 335, lbl_18_data_8110C},
        {9, 471, 335, lbl_18_data_8110C},
        {9, 471, 335, lbl_18_data_810D4},
    },
    {
        {0, 471, 159, lbl_18_data_81168},
        {0, 471, 159, lbl_18_data_81168},
        {0, 471, 159, lbl_18_data_81168},
        {0, 471, 159, lbl_18_data_81168},
        {0, 471, 159, lbl_18_data_81168},
        {7, 471, 159, lbl_18_data_81148},
    },
    {
        {11, 15, 100, lbl_18_data_80C34},
        {11, 15, 100, lbl_18_data_80C34},
        {11, 15, 100, lbl_18_data_80C34},
        {11, 15, 100, lbl_18_data_80C34},
        {11, 15, 100, lbl_18_data_80C34},
        {11, 15, 100, lbl_18_data_80C34},
    },
    {
        {20, 15, 82, lbl_18_data_80DAC},
        {20, 15, 82, lbl_18_data_80DAC},
        {20, 15, 82, lbl_18_data_80DAC},
        {20, 15, 82, lbl_18_data_80DAC},
        {20, 15, 82, lbl_18_data_80DAC},
        {21, 15, 120, lbl_18_data_80C34},
    },
    {
        {13, 15, 328, lbl_18_data_80C34},
        {13, 15, 328, lbl_18_data_80C34},
        {13, 15, 328, lbl_18_data_80C34},
        {13, 15, 328, lbl_18_data_80C34},
        {13, 15, 328, lbl_18_data_80C34},
        {13, 15, 328, lbl_18_data_80C34},
    },
    {
        {19, 471, 208, lbl_18_data_80EBC},
        {19, 471, 208, lbl_18_data_80EBC},
        {19, 471, 208, lbl_18_data_80EBC},
        {19, 471, 208, lbl_18_data_80EBC},
        {19, 471, 208, lbl_18_data_80EBC},
        {19, 471, 208, lbl_18_data_80E50},
    },
    {
        {7, 243, 79, lbl_18_data_80F74},
        {7, 243, 79, lbl_18_data_80F74},
        {7, 243, 79, lbl_18_data_80F74},
        {7, 243, 79, lbl_18_data_80F74},
        {7, 243, 79, lbl_18_data_80F74},
        {7, 243, 79, lbl_18_data_80F20},
    },
    {
        {0, 243, 79, lbl_18_data_811B4},
        {0, 243, 79, lbl_18_data_811B4},
        {0, 243, 79, lbl_18_data_811B4},
        {0, 243, 79, lbl_18_data_811B4},
        {0, 243, 79, lbl_18_data_811B4},
        {0, 243, 79, lbl_18_data_81190},
    },
};

s8 lbl_18_data_814D8[] = {34, 40, 42, 43, -1, 0, 0, 0};

s8 lbl_18_data_814E0[] = {35, 34, 36, 37, 38, 44, -1, 0};

s8 *lbl_18_data_814E8[2] = {lbl_18_data_814D8, lbl_18_data_814E0};

struct HowToArrow lbl_18_data_814F0[12] = {
    {320.0f, 346.0f, 320.0f, 238.0f, 0xA3ABBA},
    {163.0f, 161.0f, 222.0f, 211.0f, 0x00B2EB},
    {476.0f, 282.0f, 411.0f, 235.0f, 0x009F62},
    {476.0f, 339.0f, 381.0f, 250.0f, 0xDF0029},
    {476.0f, 183.0f, 434.0f, 213.0f, 0xA3ABBA},
    {163.0f, 128.0f, 222.0f, 211.0f, 0x00B2EB},
    {163.0f, 160.0f, 222.0f, 211.0f, 0x00B2EB},
    {163.0f, 348.0f, 218.0f, 246.0f, 0x00B2EB},
    {476.0f, 240.0f, 411.0f, 235.0f, 0x009F62},
    {320.0f, 125.0f, 250.0f, 161.0f, 0xA3ABBA},
    {320.0f, 115.0f, 381.0f, 190.0f, 0xA3ABBA},
    {163.0f, 128.0f, 222.0f, 211.0f, 0x00B2EB},
};

u32 lbl_18_data_815E0[5] = {0x3761E2, 0x19A871, 0xFACE19, 0xE2193E, 0x962897};

u32 lbl_18_data_815F4[] = {0x7E421F, 0xE2193E, 0xFACE19, 0x962897, 0x19A871, 0x3761E2};

u32 lbl_18_data_8160C[] = {0x3761E2, 0x19A871, 0xFACE19, 0xE2193E};

u32 lbl_18_data_8161C[] = {0x7E421F, 0xE2193E, 0xFACE19, 0x962897, 0x19A871, 0x3761E2};

u32 lbl_18_data_81634[] = {0x3761E2, 0x19A871, 0xFACE19, 0xE2193E, 0x962897};

u32 *lbl_18_data_81648[5] = {NULL, lbl_18_data_815F4, lbl_18_data_8160C, lbl_18_data_8161C, lbl_18_data_81634};

float lbl_18_data_8165C[] = {112.0f, 194.0f, 276.0f, 358.0f};

float lbl_18_data_8166C[] = {112.0f, 176.0f, 240.0f, 304.0f, 368.0f};

float lbl_18_data_81680[] = {98.0f, 154.0f, 210.0f, 266.0f, 322.0f, 378.0f};

float *lbl_18_data_81698[7] = {NULL, NULL, NULL, NULL, lbl_18_data_8165C, lbl_18_data_8166C, lbl_18_data_81680};

s32 lbl_18_data_816B4[] = {0xF, 0x20, 0x33, 0x3, 0xF};

s32 lbl_18_data_816C8[] = {0x45, 0x2E, 0x40, 0x57, 0x45};

s32 lbl_18_data_816DC[] = {0x46, 0x2F, 0x41, 0x5B, 0x46};

s32 lbl_18_data_816F0[] = {0x47, 0x30, 0x41, 0x5C, 0x47, 0xF, 0x20, 0x33, 0x3, 0x45, 0x2E, 0x34, 0x4, 0x46, 0x2F, 0x35, 0x7, 0x47, 0x30, 0x36, 0x8};

s32 lbl_18_data_81744[] = {0x0, 0x0, 0x0, 0x0, 0xF};

s32 lbl_18_data_81758[] = {0x0, 0x0, 0x0, 0x0, 0x45};

s32 lbl_18_data_8176C[] = {0x0, 0x0, 0x0, 0x0, 0x46};

s32 lbl_18_data_81780[] = {0x0, 0x0, 0x0, 0x0, 0x47};

s32 *lbl_18_data_81794[3] = {lbl_18_data_816B4, lbl_18_data_816B4, lbl_18_data_81744};
s32 *lbl_18_data_817A0[3] = {lbl_18_data_816C8, lbl_18_data_816C8, lbl_18_data_81758};
s32 *lbl_18_data_817AC[3] = {lbl_18_data_816DC, lbl_18_data_816DC, lbl_18_data_8176C};
s32 *lbl_18_data_817B8[3] = {lbl_18_data_816F0, lbl_18_data_816F0, lbl_18_data_81780};

s32 lbl_18_data_817C4[8] = {0x0, 0xC3C, 0xC3F, 0xC3D, 0xC39, 0xC3B, 0xC3E, 0xC3A};

char lbl_18_data_817E4[] = "\203s\203b\203`\203\203\201[\202\315 \202\240\202\242\202\304\203o\203b\203^\201[\202\311 \203q\203b\203g\202\342\203z\201[\203\200\203\211\203\223\202\360";

char lbl_18_data_81818[] = "\202\244\202\275\202\352\202\310\202\242\202\346\202\244\202\311\201A\202\244\202\334\202\255 \203R\203\215\202\252\202\350\202\334\202\265\202\345\202\244\201I";

char lbl_18_data_81844[] = "\203o\203b\203^\201[\202\315 \203^\203C\203~\203\223\203O\202\346\202\255 \203o\203b\203g\202\360 \202\323\202\301\202\304\202\255\202\276\202\263\202\242\201I";

char lbl_18_data_81878[] = "\203R\203\215\202\252\202\301\202\304\202\255\202\351\203{\201[\203\213\202\360 \202\244\202\277\202\334\202\265\202\345\202\244\201I";

char lbl_18_data_818A0[] = "\203q\203b\203g\201E\203c\201[\203x\201[\203X\201E\203X\203\212\201[\203x\201[\203X\202\314\203|\203P\203b\203g\202\311";

char lbl_18_data_818D0[] = "\202\315\202\242\202\352\202\316\201A \202\302\202\254\202\314\203o\203b\203^\201[\202\311 \202\276\202\266\202\343\202\361\202\252 \202\334\202\355\202\350\202\334\202\267\201I";

char lbl_18_data_81904[] = "\202\244\202\301\202\275\203{\201[\203\213\202\252\203W\203\203\203\223\203v\202\276\202\242\202\314 \202\244\202\246\202\360 \202\306\202\250\202\351\202\306";

char lbl_18_data_81934[] = "\203{\201[\203\213\202\315 \203X\203^\203\223\203h\202\334\202\305 \203W\203\203\203\223\203v\202\265\202\304 \203z\201[\203\200\203\211\203\223\202\305\202\267\201I";

char lbl_18_data_81968[] = "\203A\203E\203g\203|\203P\203b\203g\202\311\202\250\202\277\202\275\202\350\201A\202\240\202\242\202\304\202\311\203L\203\203\203b\203`\202\263\202\352\202\351\202\306";

char lbl_18_data_8199C[] = "\203A\203E\203g\202\311\202\310\202\350\201A\202R\203A\203E\203g\202\305 \203`\203F\203\223\203W\202\265\202\334\202\267\201B";

char lbl_18_data_819C8[] = "\203C\203j\203\223\203O\202\262\202\306\202\311 \203\213\201[\203\214\203b\203g\202\360\202\334\202\355\202\265\201A\202\305\202\275\202\337\202\311\202\346\202\301\202\304";

char lbl_18_data_819FC[] = "\203|\203P\203b\203g\202\342 \203W\203\203\203\223\203v\202\276\202\242\202\314\202\310\202\347\202\321\202\251\202\275\202\252\201A\202\251\202\355\202\350\202\334\202\267\201B";

char lbl_18_data_81A30[] = "\203m\201[\203}\203\213\203V\203t\203g\201F";

char lbl_18_data_81A44[] = "\202\310\202\311\202\340\202\310\202\242 \202\337\202\305 \202\306\202\334\202\301\202\275\202\306\202\253\202\314 \202\253\202\331\202\361\202\304\202\253\202\310\203V\203t\203g\202\305\202\267\201B";

char lbl_18_data_81A7C[] = "\202\242\202\301\202\317\202\302\203V\203t\203g\201F";

char lbl_18_data_81A90[] = "\202\267\202\327\202\304\202\314\203W\203\203\203\223\203v\202\276\202\242\202\252\202\305\202\304\201A\203|\203P\203b\203g\202\315\202\272\202\361\202\324\203A\203E\203g\202\305\202\267\201B";

char lbl_18_data_81ACC[] = "\202\335\202\254\203V\203t\203g\201F";

char lbl_18_data_81ADC[] = "\203\211\203C\203g\202\252\202\355\202\311\201A\203q\203b\203g\202\314\203|\203P\203b\203g\202\252 \202\240\202\302\202\334\202\301\202\304\202\242\202\334\202\267\201B";

char lbl_18_data_81B10[] = "\202\320\202\276\202\350\203V\203t\203g\201F";

char lbl_18_data_81B20[] = "\203\214\203t\203g\202\252\202\355\202\311\201A\203q\203b\203g\202\314\203|\203P\203b\203g\202\252 \202\240\202\302\202\334\202\301\202\304\202\242\202\334\202\267\201B";

char lbl_18_data_81B54[] = "\203R\203c\203R\203c\203V\203t\203g\201F";

char lbl_18_data_81B68[] = "\203|\203P\203b\203g\202\252 \202\272\202\361\202\324 \202P\203x\201[\203X\203q\203b\203g\202\311\202\310\202\350\202\334\202\267\201B";

char lbl_18_data_81B98[] = "\202\334\202\361\202\310\202\251\203V\203t\203g\201F";

char lbl_18_data_81BAC[] = "\203Z\203\223\203^\201[\202\311\201A\203q\203b\203g\202\314\203|\203P\203b\203g\202\252 \202\240\202\302\202\334\202\301\202\304\202\242\202\334\202\267\201B";

char lbl_18_data_81BE0[] = "\202\315\202\361\202\324\202\361\203A\203E\203g\201F";

char lbl_18_data_81BF4[] = "\203|\203P\203b\203g\202\314\202\315\202\361\202\324\202\361\202\252\201A\203A\203E\203g\202\314\203|\203P\203b\203g\202\311\202\310\202\350\202\334\202\267\201B";

char lbl_18_data_81C28[] = "\203\211\203b\203L\201[\203Z\203u\203\223\201F";

char lbl_18_data_81C3C[] = "\202V\202\251\202\242\202\314\202\335\201A\202\336\202\266\202\351\202\265\202\314\202\337\202\252 \203\211\203b\203L\201[\203]\201[\203\223\202\311\202\310\202\350\202\334\202\267\201B";

char *lbl_18_data_81C74[15][2] = {
    {lbl_18_data_817E4, lbl_18_data_81818},
    {lbl_18_data_81844, lbl_18_data_81878},
    {lbl_18_data_818A0, lbl_18_data_818D0},
    {lbl_18_data_81904, lbl_18_data_81934},
    {lbl_18_data_81968, lbl_18_data_8199C},
    {lbl_18_data_819C8, lbl_18_data_819FC},
    {lbl_18_data_81A30, lbl_18_data_81A44},
    {lbl_18_data_81A7C, lbl_18_data_81A90},
    {lbl_18_data_81ACC, lbl_18_data_81ADC},
    {lbl_18_data_81B10, lbl_18_data_81B20},
    {lbl_18_data_819C8, lbl_18_data_819FC},
    {lbl_18_data_81B54, lbl_18_data_81B68},
    {lbl_18_data_81B98, lbl_18_data_81BAC},
    {lbl_18_data_81BE0, lbl_18_data_81BF4},
    {lbl_18_data_81C28, lbl_18_data_81C3C},
};

char lbl_18_data_81CEC[] = "The pitcher should perform a nice roll so the";

char lbl_18_data_81D1C[] = "batter doesn't make a hit or a homer!";

char lbl_18_data_81D44[] = "Batter must swing at the right time to hit the";

char lbl_18_data_81D74[] = "ball! Hit the rolling ball!";

char lbl_18_data_81D90[] = "If the ball falls into one base, two base, or three base";

char lbl_18_data_81DCC[] = "pockets, the next batter is up!";

char lbl_18_data_81DEC[] = "If the ball hits the jump board, it will jump to the";

char lbl_18_data_81E24[] = "stands and become a homerun!";

char lbl_18_data_81E44[] = "If the ball falls into the Out pocket or is caught by";

char lbl_18_data_81E7C[] = "the other team, it is Out. After 3 Outs your turn is over.";

char lbl_18_data_81EB8[] = "Spin the wheel each inning. Depending on what number you get,";

char lbl_18_data_81EF8[] = "the position of the pockets and the jump boards change.";

char lbl_18_data_81F30[] = "Normal Shift:";

char lbl_18_data_81F40[] = "A basic shift occurs when the wheel stops at the No-symbol outcome.";

char lbl_18_data_81F84[] = "Homerun Shift:";

char lbl_18_data_81F94[] = "All jump boards appear, and all pockets become Out pockets.";

char lbl_18_data_81FD0[] = "Right Shift:";

char lbl_18_data_81FE0[] = "Hit pockets move to the right side.";

char lbl_18_data_82004[] = "Left Shift:";

char lbl_18_data_82010[] = "Hit pockets move to the left side.";

char lbl_18_data_82034[] = "Hit Shift:";

char lbl_18_data_82040[] = "All pockets become One Base hits.";

char lbl_18_data_82064[] = "Center Shift:";

char lbl_18_data_82074[] = "Hit pockets move to the center.";

char lbl_18_data_82094[] = "Half Out:";

char lbl_18_data_820A0[] = "Half of the pockets become Out pockets.";

char lbl_18_data_820C8[] = "Lucky Seven:";

char lbl_18_data_820D8[] = "On the seventh match, Non-symbol outcomes become lucky zones.";

char *lbl_18_data_82118[15][2] = {
    {lbl_18_data_81CEC, lbl_18_data_81D1C},
    {lbl_18_data_81D44, lbl_18_data_81D74},
    {lbl_18_data_81D90, lbl_18_data_81DCC},
    {lbl_18_data_81DEC, lbl_18_data_81E24},
    {lbl_18_data_81E44, lbl_18_data_81E7C},
    {lbl_18_data_81EB8, lbl_18_data_81EF8},
    {lbl_18_data_81F30, lbl_18_data_81F40},
    {lbl_18_data_81F84, lbl_18_data_81F94},
    {lbl_18_data_81FD0, lbl_18_data_81FE0},
    {lbl_18_data_82004, lbl_18_data_82010},
    {lbl_18_data_81EB8, lbl_18_data_81EF8},
    {lbl_18_data_82034, lbl_18_data_82040},
    {lbl_18_data_82064, lbl_18_data_82074},
    {lbl_18_data_82094, lbl_18_data_820A0},
    {lbl_18_data_820C8, lbl_18_data_820D8},
};

char lbl_18_data_82190[] = "15\201A30\201A40\202\306\203|\203C\203\223\203g\202\360\202\306\202\301\202\304\202\242\202\253\201A\202\263\202\253\202\311\202S\203|\203C\203\223\203g\202\360";

char lbl_18_data_821C8[] = "\202\306\202\352\202\316\201A\202\273\202\314\203v\203\214\203C\203\204\201[\202\314\203Q\201[\203\200\202\306\202\310\202\350\202\334\202\267\201B";

char lbl_18_data_821F4[] = "\202\250\202\275\202\252\202\242\202\311\202R\203|\203C\203\223\203g\202\360\202\306\202\351\202\306\201A\203X\203R\203A\202\315\203f\203\205\201[\203X\202\306\202\310\202\350\201A";

char lbl_18_data_8222C[] = "\202\273\202\314\202\240\202\306 \202Q\203|\203C\203\223\203g\202\314 \202\263\202\252\202\305\202\351\202\334\202\305 \202\302\202\303\202\257\202\334\202\267\201B";

char lbl_18_data_8225C[] = "\202\253\202\337\202\347\202\352\202\275\203Q\201[\203\200\202\267\202\244\202\360 \202\263\202\253\202\311\202\306\202\301\202\275\203v\203\214\203C\203\204\201[\202\252\201A";

char lbl_18_data_82290[] = "\202\273\202\314\203Z\203b\203g\202\360 \202\306\202\301\202\275\202\261\202\306\202\311\202\310\202\350\202\334\202\267\201B";

char lbl_18_data_822B4[] = "\202\253\202\337\202\347\202\352\202\275\203Z\203b\203g\202\267\202\244\202\360 \202\263\202\253\202\311\202\306\202\301\202\275\203v\203\214\203C\203\204\201[\202\252\201A";

char lbl_18_data_822E8[] = "\202\273\202\314\202\265\202\240\202\242\202\311 \202\251\202\301\202\275\202\261\202\306\202\311\202\310\202\350\202\334\202\267\201B";

char lbl_18_data_8230C[] = "\202\266\202\337\202\361\202\311 \202\240\202\347\202\355\202\352\202\351 \203X\203}\203b\203V\203\205\203}\201[\203N\202\314 \202\244\202\246\202\305";

char lbl_18_data_8233C[] = "\203^\203C\203~\203\223\203O\202\346\202\255 \203{\201[\203\213\202\360\202\244\202\302\202\306\201A\203X\203}\203b\203V\203\205\202\252 \202\253\202\334\202\350\202\334\202\267\201I";

char *lbl_18_data_82378[6][2] = {
    {lbl_18_data_82190, lbl_18_data_821C8},
    {lbl_18_data_821F4, lbl_18_data_8222C},
    {lbl_18_data_8225C, lbl_18_data_82290},
    {lbl_18_data_822B4, lbl_18_data_822E8},
    {NULL, NULL},
    {lbl_18_data_8230C, lbl_18_data_8233C},
};

char lbl_18_data_823A8[] = "Points are named 15,30,40. The Player who";

char lbl_18_data_823D4[] = "gets 4 points first wins the game.";

char lbl_18_data_823F8[] = "If the score is tied at 3 points, a Deuce occurs.";

char lbl_18_data_8242C[] = "You continue to play until the difference is 2.";

char lbl_18_data_8245C[] = "The player who wins the determined number of";

char lbl_18_data_8248C[] = "games wins the set.";

char lbl_18_data_824A0[] = "The first player to win the majority of the";

char lbl_18_data_824CC[] = "determined number of sets wins the match.";

char lbl_18_data_824F8[] = "You can smash if you hit the ball just on the Smash";

char lbl_18_data_8252C[] = "Mark which appears on the ground!";

char *lbl_18_data_82550[6][2] = {
    {lbl_18_data_823A8, lbl_18_data_823D4},
    {lbl_18_data_823F8, lbl_18_data_8242C},
    {lbl_18_data_8245C, lbl_18_data_8248C},
    {lbl_18_data_824A0, lbl_18_data_824CC},
    {NULL, NULL},
    {lbl_18_data_824F8, lbl_18_data_8252C},
};

char *(*lbl_18_data_82580[2][6])[2] = {
    {lbl_18_data_82118, lbl_18_data_82118, lbl_18_data_82118, lbl_18_data_82118, lbl_18_data_82118, lbl_18_data_81C74},
    {lbl_18_data_82550, lbl_18_data_82550, lbl_18_data_82550, lbl_18_data_82550, lbl_18_data_82550, lbl_18_data_82378},
};

char lbl_18_data_825B0[] = "pages";

char lbl_18_data_825B8[] = " ";

char lbl_18_data_825BC[] = "\203y\201[\203W";

char lbl_18_data_825C4[] = "Back";

char lbl_18_data_825CC[] = "Zur\374ck";

char lbl_18_data_825D4[] = "Retour";

char lbl_18_data_825DC[] = "Atr\341s";

char lbl_18_data_825E4[] = "Indietro";

char lbl_18_data_825F0[] = "\202\334\202\246";

char lbl_18_data_825F8[] = "Next";

char lbl_18_data_82600[] = "Weiter";

char lbl_18_data_82608[] = "Suiv.";

char lbl_18_data_82610[] = "Sig.";

char lbl_18_data_82618[] = "Succ.";

char lbl_18_data_82620[] = "\202\302\202\254";

char lbl_18_data_82628[] = "Movement";

char lbl_18_data_82634[] = "Bewegen";

char lbl_18_data_8263C[] = "D\351pl.";

char lbl_18_data_82644[] = "Mov.";

char lbl_18_data_8264C[] = "Muovi";

char lbl_18_data_82654[] = "\202\242\202\307\202\244";

char lbl_18_data_8265C[] = "Return";

char lbl_18_data_82664[] = "\202\340\202\307\202\351";

char lbl_18_data_8266C[] = "%d/%d\n%s";

char lbl_18_data_82678[] = "\203\255";

char lbl_18_data_8267C[4] = "\203\240";

s32 lbl_18_bss_1A2D0;
s8 lbl_18_bss_1A2D4;
s8 lbl_18_bss_1A2D5;
s8 lbl_18_bss_1A2D6;
s8 lbl_18_bss_1A2D7;
s8 lbl_18_bss_1A2D8;
s8 lbl_18_bss_1A2D9;
s8 lbl_18_bss_1A2DA;
float lbl_18_bss_1A2DC;
float lbl_18_bss_1A2E0;
float lbl_18_bss_1A2E4;
float lbl_18_bss_1A2E8;
float lbl_18_bss_1A2EC;
float lbl_18_bss_1A2F0;
float lbl_18_bss_1A2F4;
float lbl_18_bss_1A2F8;
float lbl_18_bss_1A2FC;
float lbl_18_bss_1A300;
float lbl_18_bss_1A304;
float lbl_18_bss_1A308;
float lbl_18_bss_1A30C;
s8 lbl_18_bss_1A310;
float lbl_18_bss_1A314;
float lbl_18_bss_1A318;
float lbl_18_bss_1A31C;
void fn_18_22CE0(void)
{
    struct Sprite *tr3;

    lbl_18_bss_1A2D0 = (modeCtrl.gameType - 0xF);
    tr3 = create_sprite();
    if (tr3 != NULL) {
        tr3->tag = 0xA;
        tr3->x = 320.0f;
        tr3->y = 240.0f;
        tr3->textAlign = 0;
        tr3->depth = 0.002f;
        tr3->userVar = 0;
        tr3->fontId = 3;
        if ((s32) main_mode == 1) {
            tr3->mainFunc = fn_18_22E7C;
        }
        tr3->drawFunc = fn_18_2489C;
        tr3->destFunc = fn_18_25188;
        strcpy(tr3->text, lbl_18_data_80480);
        lbl_18_bss_1A2D4 = 0xF;
        lbl_18_bss_1A2D5 = 0;
        lbl_18_bss_1A2D6 = 0;
        lbl_18_bss_1A2D7 = 0;
        lbl_18_bss_1A2D8 = 0;
        lbl_18_bss_1A2D9 = 0;
        lbl_18_bss_1A2DA = 0;
        lbl_18_bss_1A2DC = 1.0f;
        lbl_18_bss_1A2E0 = 0.0f;
        lbl_18_bss_1A2E4 = 1.0f;
        lbl_18_bss_1A2E8 = 0.0f;
        lbl_18_bss_1A2EC = 0.0f;
        lbl_18_bss_1A2F0 = 0.0f;
        lbl_18_bss_1A2F4 = 0.0f;
        lbl_18_bss_1A2F8 = 0.0f;
        lbl_18_bss_1A2FC = 0.0f;
        lbl_18_bss_1A300 = 1.0f;
        lbl_18_bss_1A304 = 0.0f;
        lbl_18_bss_1A308 = 1.0f;
        lbl_18_bss_1A30C = 1.0f;
        lbl_18_bss_1A310 = 0;
        lbl_18_bss_1A314 = 1.0f;
        lbl_18_bss_1A318 = 0.0f;
        lbl_18_bss_1A31C = 0.0f;
    }
}

void fn_18_22E7C(s8 *arg0, struct Sprite *arg1)
{
    float f2;
    float f2_2;
    s32 tr0_5;
    s32 r0;
    s32 r0_2;
    s32 r3;
    s32 r3_2;
    s32 r4;

    r0 = 0;
    if (lbl_18_bss_1A2D4 > 0) {
        lbl_18_bss_1A2D4--;
    }
    if (PAD_REPEAT((s8) g_related_to_pause_menu_input.padId, 1) && lbl_18_bss_1A2D4 == 0) {
        if (--lbl_18_bss_1A2D6 < 0) {
            lbl_18_bss_1A2D6 = (lbl_18_data_80488[0][lbl_18_bss_1A2D0] - 1);
        }
        if (fn_18_16E8() == 2) {
            if (lbl_18_bss_1A2D6 == 1) {
                lbl_18_bss_1A2D6 = 0;
            }
        } else if (lbl_18_bss_1A2D6 == 2) {
            lbl_18_bss_1A2D6 = 1;
        }
        if (--lbl_18_bss_1A2D5 < 0) {
            lbl_18_bss_1A2D5 = (lbl_18_data_80488[1][lbl_18_bss_1A2D0] - 1);
        }
        fn_18_218EC(0x6E);
        lbl_18_bss_1A2D4 = 0xF;
        r0 = 1;
    } else if (PAD_REPEAT((s8) g_related_to_pause_menu_input.padId, 2) && lbl_18_bss_1A2D4 == 0) {
        if (++lbl_18_bss_1A2D6 == (s8) lbl_18_data_80488[0][lbl_18_bss_1A2D0]) {
            lbl_18_bss_1A2D6 = 0;
        }
        if (fn_18_16E8() == 2) {
            if (lbl_18_bss_1A2D6 == 1) {
                lbl_18_bss_1A2D6 = 2;
            }
        } else if (lbl_18_bss_1A2D6 == 2) {
            lbl_18_bss_1A2D6 = 3;
        }
        if (++lbl_18_bss_1A2D5 >= (s8) lbl_18_data_80488[1][lbl_18_bss_1A2D0]) {
            lbl_18_bss_1A2D5 = 0;
        }
        fn_18_218EC(0x6E);
        lbl_18_bss_1A2D4 = 0xF;
        r0 = 1;
    }
    lbl_18_bss_1A2D7 = lbl_18_bss_1A2D6;
    lbl_18_bss_1A2DA = lbl_18_bss_1A2D6;
    switch ((s32) modeCtrl.gameType) {              /* switch 1; irregular */
    case 15:                                        /* switch 1 */
        if (lbl_18_bss_1A2D6 > 0) {
            if (lbl_18_bss_1A2D6 > 2) {
                r3 = 3;
            } else {
                r3 = 1;
            }
            lbl_18_bss_1A2D7 = r3;
            if (r0 != 0) {
                lbl_18_bss_1A2D8 = 0x2D;
            }
        }
        break;
    case 16:                                        /* switch 1 */
        if (lbl_18_bss_1A2D6 > 0) {
            lbl_18_bss_1A2D7 = 1;
            if (r0 != 0) {
                lbl_18_bss_1A2D8 = 0x2D;
            }
        }
        break;
    }
    if (lbl_18_bss_1A2D8 > 0) {
        lbl_18_bss_1A2D8--;
    }
    if (lbl_18_bss_1A2D8 == 0) {
        lbl_18_bss_1A2D9 = lbl_18_bss_1A2D6;
    }
    tr0_5 = arg1->userVar;
    switch (tr0_5) {                            /* switch 2; irregular */
    case 0:                                         /* switch 2 */
        lbl_18_bss_1A2DC += (0.1 * -lbl_18_bss_1A2DC);
        lbl_18_bss_1A2E0 += (0.1 * (100.0 - lbl_18_bss_1A2E0));
        if (lbl_18_bss_1A2E0 > 79.0) {
            lbl_18_bss_1A2E4 += (0.1 * (80.0 - lbl_18_bss_1A2E4));
        }
        if ((lbl_18_bss_1A2D7 == 0) || (lbl_18_bss_1A2D7 == 4)) {
            lbl_18_bss_1A2E8 += (0.1 * (1.0 - lbl_18_bss_1A2E8));
        } else if ((lbl_18_bss_1A2EC < 0.1) && (lbl_18_bss_1A2F4 < 0.1)) {
            lbl_18_bss_1A2E8 += (0.1 * -lbl_18_bss_1A2E8);
        }
        if (lbl_18_bss_1A2D7 == 0) {
            if ((lbl_18_bss_1A2E8 > 0.95) && (lbl_18_bss_1A2F4 < 0.01)) {
                lbl_18_bss_1A2EC += (0.2 * (1.0 - lbl_18_bss_1A2EC));
            }
            if (lbl_18_bss_1A2EC > 0.99) {
                lbl_18_bss_1A2F0 += (0.1 * (1.0 - lbl_18_bss_1A2F0));
            }
        } else {
            lbl_18_bss_1A2F0 += (0.3 * -lbl_18_bss_1A2F0);
            if (lbl_18_bss_1A2F0 < 0.01) {
                lbl_18_bss_1A2EC += (0.3 * -lbl_18_bss_1A2EC);
            }
            if (lbl_18_bss_1A2EC < 0.05f) {
                lbl_18_bss_1A2EC = 0.0f;
            }
        }
        if (lbl_18_bss_1A2D7 == 4) {
            if ((lbl_18_bss_1A2E8 > 0.95) && (lbl_18_bss_1A2EC < 0.01)) {
                lbl_18_bss_1A2F4 += (0.2 * (1.0 - lbl_18_bss_1A2F4));
            }
            if (lbl_18_bss_1A2F4 > 0.99) {
                lbl_18_bss_1A2F8 += (0.1 * (1.0 - lbl_18_bss_1A2F8));
            }
        } else {
            lbl_18_bss_1A2F8 += (0.3 * -lbl_18_bss_1A2F8);
            if (lbl_18_bss_1A2F8 < 0.01) {
                lbl_18_bss_1A2F4 += (0.3 * -lbl_18_bss_1A2F4);
            }
        }
        if ((lbl_18_bss_1A2D7 == 1) && (lbl_18_bss_1A2D8 == 0)) {
            if ((lbl_18_bss_1A2EC < 0.01) && (lbl_18_bss_1A2F4 < 0.01)) {
                lbl_18_bss_1A300 += (0.2 * -lbl_18_bss_1A300);
            }
            if (lbl_18_bss_1A300 < 0.01) {
                lbl_18_bss_1A2FC += (0.2 * (1.0 - lbl_18_bss_1A2FC));
            }
            if (lbl_18_bss_1A2FC > 0.99) {
                lbl_18_bss_1A304 += (0.1 * (1.0 - lbl_18_bss_1A304));
            }
        } else {
            lbl_18_bss_1A304 += (0.3 * -lbl_18_bss_1A304);
            if (lbl_18_bss_1A304 < 0.01) {
                lbl_18_bss_1A2FC += (0.3 * -lbl_18_bss_1A2FC);
            }
            if (lbl_18_bss_1A2FC < 0.01) {
                lbl_18_bss_1A300 += (0.1 * (1.0 - lbl_18_bss_1A300));
            }
        }
        if (lbl_18_bss_1A2D7 == 2) {
            if ((lbl_18_bss_1A2FC < 0.01) && (lbl_18_bss_1A2E8 < 0.1)) {
                lbl_18_bss_1A308 += (0.1 * -lbl_18_bss_1A308);
            }
        } else {
            lbl_18_bss_1A308 += (0.1 * (1.0 - lbl_18_bss_1A308));
        }
        if ((lbl_18_bss_1A2D7 == 3) && (lbl_18_bss_1A2D8 == 0)) {
            if (lbl_18_bss_1A2E8 < 0.1) {
                lbl_18_bss_1A30C += (0.1 * -lbl_18_bss_1A30C);
            }
        } else {
            lbl_18_bss_1A30C += (0.1 * (1.0 - lbl_18_bss_1A30C));
        }
        break;
    case 1:                                         /* switch 2 */
        lbl_18_bss_1A2DC += (0.1 * (1.0 - lbl_18_bss_1A2DC));
        break;
    }
    r0_2 = 0;
    r3_2 = lbl_18_bss_1A310;
    if (((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x10) != 0) || ((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x20) != 0) || ((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x80) != 0) || ((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x40) != 0) || ((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x100) != 0)) {
        r3_2++;
        if (r3_2 > 0x1E) {
            r3_2 = 0x1E;
        }
        r0_2 = 1;
    } else {
        r3_2--;
        if (r3_2 < 0) {
            r3_2 = 0;
        }
        if (r3_2 >= 0x1E) {
            r0_2 = 1;
        }
    }
    lbl_18_bss_1A310 = (s8) r3_2;
    if (r3_2 < 0x1E) {
        r4 = r3_2;
    } else {
        r4 = 0x1E;
    }
    lbl_18_bss_1A314 = ((float) r4 / 30.0f);
    f2 = lbl_18_bss_1A318;
    if ((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x10) != 0) {
        f2 -= 1.0;
        if (f2 < -30.0) {
            f2 = -30.0f;
        }
    } else if ((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x20) != 0) {
        f2 += 1.0;
        if (f2 > 30.0) {
            f2 = 30.0f;
        }
    } else if (r0_2 == 0) {
        f2 -= f2 / (float) (r3_2 + 1);
    }
    lbl_18_bss_1A318 = f2;
    f2_2 = lbl_18_bss_1A31C;
    if ((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x80) != 0) {
        f2_2 -= 1.0;
        if (f2_2 < -30.0) {
            f2_2 = -30.0f;
        }
    } else if ((s32) (*analog_inputs[(s8) g_related_to_pause_menu_input.padId] & 0x40) != 0) {
        f2_2 += 1.0;
        if (f2_2 > 30.0) {
            f2_2 = 30.0f;
        }
    } else if (r0_2 == 0) {
        f2_2 -= f2_2 / (float) (r3_2 + 1);
    }
    lbl_18_bss_1A31C = f2_2;
}

void fn_18_23BC8(s32 arg0, struct Sprite *arg1, struct SpriteDrawRequest *arg2)
{
    float tf31;
    float tf31_2;
    float tf30;
    float tf29;
    float tf28;
    float tf27;
    float tf26;
    double f26;
    float tf26_2;
    float f0;
    s32 r25;
    s32 tr24;
    s32 r24;
    struct HowToLine *tr23;
    struct HowToArrow *tr22;
    struct HowToLabel *tr22_2;
    s8 *tr21;
    s32 tr20;
    u32 r20;

    tr21 = lbl_18_data_814E8[lbl_18_bss_1A2D0];
    if (tr21 != 0) {
        for (r25 = 0; tr21[r25] != -1; r25++) {
            tr23 = &lbl_18_data_811C0[tr21[r25] - 0x22][locale];
            textdraw_set_depth((float) (arg1->depth - 0.002));
            tf29 = arg2->z;
            tf28 = arg2->scaleX;
            tf27 = arg2->scaleY;
            tr24 = arg2->flags;
            tr20 = arg2->mulColor;
            tr22 = &lbl_18_data_814F0[tr21[r25] - 0x22];
            tf26 = tr22->unk0 + (0.15f * (320.0f - tr22->unk0));
            tf31 = tr22->unk8 - tf26;
            tf30 = tr22->unkC - tr22->unk4;
            arg2->z += 0.0005;
            if (arg0 != 0) {
                f0 = lbl_18_bss_1A2F8;
            } else {
                f0 = lbl_18_bss_1A2F0;
            }
            arg2->opacity = f0;
            arg2->mulColor = tr22->unk10;
            arg2->bmpId = 0xC2B;
            arg2->x = ((0.5 * tf31) + tf26);
            arg2->y = ((0.5 * tf30) + tr22->unk4);
            arg2->scaleX = 1.0f;
            arg2->scaleY = (0.125 * math_sqrt((tf31 * tf31) + (tf30 * tf30)));
            arg2->rotation = math_atan2(tf31, tf30);
            arg2->flags = (arg1->flags & 0xFFFFFFF0) | 0xA;
            arg2->y += 10.0f;
            draw_sprite_draw_request(arg2);
            arg2->bmpId = 0xC2A;
            arg2->x = tr22->unk8;
            arg2->y = tr22->unkC;
            arg2->z -= 0.001;
            arg2->scaleX = 1.0f;
            arg2->scaleY = 1.0f;
            arg2->rotation += 0x8000;
            arg2->y += 10.0f;
            draw_sprite_draw_request(arg2);
            if (tr21[r25] == 0x2B) {
                tf31_2 = 140.0f + (tr22->unk8 - tr22->unk0);
                arg2->bmpId = 0xC2B;
                arg2->x = ((0.5 * tf31_2) + tf26);
                arg2->y = ((0.5 * tf30) + tr22->unk4);
                arg2->z = tf29;
                arg2->scaleX = 1.0f;
                arg2->scaleY = (0.125 * math_sqrt((tf31_2 * tf31_2) + (tf30 * tf30)));
                arg2->rotation = math_atan2(tf31_2, tf30);
                arg2->y += 10.0f;
                draw_sprite_draw_request(arg2);
                arg2->bmpId = 0xC2A;
                arg2->x = 140.0f + tr22->unk8;
                arg2->y = tr22->unkC;
                arg2->z -= 0.001;
                arg2->scaleX = 1.0f;
                arg2->scaleY = 1.0f;
                arg2->rotation += 0x8000;
                arg2->y += 10.0f;
                draw_sprite_draw_request(arg2);
            }
            arg2->z = tf29;
            arg2->scaleX = tf28;
            arg2->scaleY = tf27;
            arg2->rotation = 0;
            arg2->opacity = 1.0f;
            arg2->flags = tr24;
            arg2->mulColor = tr20;
            tr22_2 = &lbl_18_data_8070C[tr23->unk0];
            arg2->bmpId = tr22_2->bmpId;
            arg2->x = tr23->unk2;
            arg2->y = tr23->unk4;
            arg2->x += 0.05f * (244.0f - arg2->x);
            arg2->y += 10.0f;
            arg2->x -= 11.4;
            arg2->scaleX = 1.15f;
            draw_sprite_draw_request(arg2);
            arg2->x += 11.4f;
            arg2->scaleX = 1.0f;
            if (lbl_18_bss_1A2D7 == 0) {
                for (r24 = 0; tr23->unk8[r24] != 0; r24++) {
                    if (r24 == 0) {
                        f26 = 0.541666;
                    } else {
                        f26 = 0.58;
                    }
                    tf26_2 = f26;
                    if (r24 == 0) {
                        r20 = 0xFFFFFF;
                    } else {
                        r20 = 0;
                    }
                    textdraw_set_alignment(3);
                    textdraw_set_mul_color(r20);
                    textdraw_set_scale(0.8f * tf26_2, tf26_2);
                    textdraw_set_pos(76.0f + arg2->x, arg2->y + (float) tr22_2->lineY[r24]);
                    textdraw_print_and_fit_to_width(165.0f, tr23->unk8[r24]);
                    textdraw_set_alignment(0);
                }
            }
        }
    }
}

void fn_18_24164(s8 arg0, struct Sprite *arg1, struct SpriteDrawRequest *arg2, s8 arg3, s8 arg4, s8 arg5, s8 arg6)
{
    float f31;
    float tf30;
    float tf29;
    double f29;
    s8 r28;
    char *tr27;
    char *tr19;
    s32 tr0;
    s32 r0;
    s32 r0_2;

    for (r28 = 0; r28 < arg3; r28++) {
        if (arg0 == 0) {
            f31 = 42.0f;
            arg2->y = (400.0f * lbl_18_bss_1A300) + lbl_18_data_81698[arg3][r28];
        } else {
            f31 = 42.0f - (640.0f * lbl_18_bss_1A30C);
            arg2->y = lbl_18_data_81698[arg3][r28];
        }
        if (arg0 == 0) {
            arg2->mulColor = lbl_18_data_815E0[r28];
        } else {
            arg2->mulColor = lbl_18_data_81648[arg0][r28];
        }
        if (arg3 == 6) {
            f29 = 0.86666;
        } else {
            f29 = 1.0;
        }
        tf29 = f29;
        arg2->scaleY = tf29;
        arg2->bmpId = 0xC01;
        arg2->x = f31;
        if ((arg0 == 0) || ((arg0 >= 3) && (r28 == 0))) {
            arg2->u1 = 0.0f;
            arg2->u2 = 0.333333f;
        } else {
            arg2->u1 = 0.333333f;
            arg2->u2 = 0.666666f;
        }
        arg2->scaleX = arg2->u2 - arg2->u1;
        draw_sprite_draw_request(arg2);
        arg2->bmpId = 0xC01;
        arg2->x = 32.0f + f31;
        arg2->u1 = 0.333333f;
        arg2->u2 = 0.666666f;
        if (arg0 == 0) {
            arg2->scaleX = (lbl_18_bss_1A2FC * (14.775 * (0.99401f * (1.046875f * (arg2->u2 - arg2->u1)))));
        } else {
            arg2->scaleX = (14.775 * (0.99401f * (1.046875f * (arg2->u2 - arg2->u1))));
        }
        draw_sprite_draw_request(arg2);
        arg2->scaleX = 1.0f;
        arg2->bmpId = 0xC01;
        if (arg0 == 0) {
            arg2->x = 32.0f + f31 + (491.99704f * lbl_18_bss_1A2FC);
        } else {
            arg2->x = 492.0f + (32.0f + f31);
        }
        arg2->u1 = 0.666666f;
        arg2->u2 = 1.0f;
        arg2->scaleX = arg2->u2 - arg2->u1;
        draw_sprite_draw_request(arg2);
        arg2->bmpId = 0xC02;
        arg2->x = 3.0f + f31;
        arg2->y += 2.0f;
        arg2->z = (arg1->depth - 0.001);
        arg2->u1 = 0.0f;
        arg2->u2 = 1.0f;
        arg2->scaleX = arg2->u2 - arg2->u1;
        arg2->mulColor = 0xFFFFFF;
        if ((arg0 == 0) || ((arg0 >= 3) && (r28 == 0))) {
            draw_sprite_draw_request(arg2);
        }
        if ((arg0 == 0) || (r28 == 0)) {
            arg2->bmpId = lbl_18_data_81794[arg5][r28];
        } else {
            arg2->bmpId = lbl_18_data_817C4[r28 + (arg0 - 3) * 4 - 1];
        }
        arg2->x = 3.0f + f31;
        if (arg3 == 6) {
            r0 = 6;
        } else {
            r0 = 7;
        }
        arg2->y += r0;
        tf30 = arg2->y;
        arg2->z = (arg1->depth - 0.002);
        arg2->u1 = 0.0f;
        arg2->u2 = 1.0f;
        if ((arg0 == 0) || ((s32) arg2->bmpId == 0xF)) {
            arg2->scaleX = 0.4f;
            arg2->scaleY = (0.26 * tf29);
        } else {
            arg2->x += 3.0f;
            switch (arg0) {
            case 3:
            case 4:
                arg2->scaleX = 0.6875f;
                arg2->scaleY = 0.6875f;
                break;
            }
        }
        arg2->mulColor = 0xFFFFFF;
        if ((s32) arg2->bmpId != 0) {
            if ((arg0 == 0) || ((s32) arg2->bmpId == 0xF)) {
                tr0 = (((u32) g_font_char_var_3 >> 2) + (r28 * 4)) & 0xF;
                switch (tr0) {                  /* irregular */
                case 10:
                case 15:
                    arg2->bmpId = lbl_18_data_817A0[arg5][r28];
                    break;
                case 11:
                case 14:
                    arg2->bmpId = lbl_18_data_817AC[arg5][r28];
                    break;
                case 12:
                case 13:
                    arg2->bmpId = lbl_18_data_817B8[arg5][r28];
                    break;
                }
            }
            draw_sprite_draw_request(arg2);
        }
        arg2->z = arg1->depth;
        arg2->scaleY = 1.0f;
        if ((lbl_18_bss_1A2D7 == 1) || (lbl_18_bss_1A2D7 == 3)) {
            arg2->x = 3.0f + f31;
            arg2->y = tf30;
            textdraw_set_alpha(arg0 == 0 ? lbl_18_bss_1A304 : 1.0);
            tr27 = lbl_18_data_82580[lbl_18_bss_1A2D0][locale][r28 + arg4 * arg6][0];
            tr19 = lbl_18_data_82580[lbl_18_bss_1A2D0][locale][r28 + arg4 * arg6][1];
            textdraw_set_pos(56.0f + arg2->x, arg2->y - 1.0f);
            textdraw_print_and_fit_to_width(482.0f, tr27);
            if (arg3 == 6) {
                r0_2 = 0x10;
            } else {
                r0_2 = 0x15;
            }
            textdraw_set_pos(56.0f + arg2->x, arg2->y + (float) r0_2);
            textdraw_print_and_fit_to_width(482.0f, tr19);
            textdraw_set_alpha(1.0f);
        }
    }
}

void fn_18_2489C(struct Sprite *arg0)
{
    char *tr27;
    struct SpriteDrawRequest req;
    float tf31;
    float tf31_2;
    s32 r5;
    s8 r0;
    s8 r6;
    s8 r7;
    s8 r9;
    s8 r3;

    fn_18_251AC();
    req.z = arg0->depth;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.opacity = 1.0f;
    req.rotation = 0;
    req.mulColor = ((u8) (s32) (255.0f * arg0->opacity) << 0x18) | 0xFF0000 | 0xFFFF;
    req.addColor = 0;
    req.unk30 = -1;
    req.flags = (arg0->flags & 0xFFFFFFF0) | 5;
    textdraw_reset();
    textdraw_set_font((s32) arg0->fontId);
    textdraw_set_depth(arg0->depth);
    textdraw_set_flags(0x80000000);
    GXSetZMode_cached(1, 7, 1);
    req.bmpId = 0x4B;
    req.x = 320.1f;
    req.y = 240.1f;
    req.z = (0.003 + arg0->depth);
    req.scaleX = lbl_18_bss_1A2E0;
    req.scaleY = lbl_18_bss_1A2E4;
    req.opacity = 0.6666f;
    req.mulColor = 0xC0C030;
    req.flags = (arg0->flags & 0xFFFFFFF0) | 0xA;
    draw_sprite_draw_request(&req);
    req.z = arg0->depth;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.opacity = 1.0f;
    req.mulColor = ((u8) (s32) (255.0f * arg0->opacity) << 0x18) | 0xFF0000 | 0xFFFF;
    req.flags = (arg0->flags & 0xFFFFFFF0) | 5;
    GXSetZMode_cached(1, 3, 1);
    req.bmpId = 0xC00;
    req.x = 112.0f;
    req.y = 22.0f - (56.0f * lbl_18_bss_1A2DC);
    draw_sprite_draw_request(&req);
    tr27 = lbl_18_data_80704[lbl_18_bss_1A2D0][lbl_18_bss_1A2DA][locale];
    textdraw_set_drop_shadow();
    textdraw_set_alignment(3);
    textdraw_set_depth((float) (arg0->depth - 0.002));
    textdraw_set_mul_color(0xFF8000);
    textdraw_set_pos(320.0f, 15.0f + req.y);
    textdraw_print_and_fit_to_width(384.0f, tr27);
    {
        char *sp68[6] = {lbl_18_data_825B0, lbl_18_data_825B8, lbl_18_data_825B8, lbl_18_data_825B8, lbl_18_data_825B8, lbl_18_data_825BC};

        textdraw_clear_drop_shadow();
        textdraw_set_alignment(3);
        textdraw_set_scale(0.5f, 0.5f);
        textdraw_set_mul_color(0);
        textdraw_set_pos(550.0f, 15.0f + req.y);
        textdraw_printf_and_fit_to_width(34.0f, lbl_18_data_8266C, lbl_18_bss_1A2D5 + 1, lbl_18_data_80488[1][lbl_18_bss_1A2D0], sp68[locale]);
        textdraw_set_depth(arg0->depth);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_alignment(0);
    }
    textdraw_set_depth(arg0->depth);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_mul_color(0);
    textdraw_set_scale(0.6f, 0.6f);
    textdraw_set_alignment(4);
    {
        char *sp50[6] = {lbl_18_data_825C4, lbl_18_data_825CC, lbl_18_data_825D4, lbl_18_data_825DC, lbl_18_data_825E4, lbl_18_data_825F0};
        char *tr27_2 = sp50[locale];

        req.bmpId = 0xC27;
        req.x = 44.0f - (40.0f * lbl_18_bss_1A2DC);
        req.y = 46.0f;
        req.flags = (arg0->flags & 0xFFFFFFF0) | 0xA;
        draw_sprite_draw_request(&req);
        textdraw_set_pos(req.x - 1.0f, 32.0f + req.y);
        textdraw_print_and_fit_to_width(45.0f, tr27_2);
    }
    {
        char *sp38[6] = {lbl_18_data_825F8, lbl_18_data_82600, lbl_18_data_82608, lbl_18_data_82610, lbl_18_data_82618, lbl_18_data_82620};
        char *tr27_3 = sp38[locale];

        req.x = 596.0f + (40.0f * lbl_18_bss_1A2DC);
        req.y = 46.0f;
        req.flags = (arg0->flags & 0xFFFFFFF0) | 0x80000 | 0xA;
        draw_sprite_draw_request(&req);
        textdraw_set_pos(req.x - 1.0f, 32.0f + req.y);
        textdraw_print_and_fit_to_width(45.0f, tr27_3);
    }
    req.flags = (arg0->flags & 0xFFFFFFF0) | 5;
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_alignment(0);
    {
        char *sp20[6] = {lbl_18_data_82628, lbl_18_data_82634, lbl_18_data_8263C, lbl_18_data_82644, lbl_18_data_8264C, lbl_18_data_82654};
        char *tr27_4 = sp20[locale];

        textdraw_set_scale(0.8f, 1.0f);
        tf31 = textdraw_get_pixel_width_of_string(tr27_4);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(20.0f - ((20.0f + tf31) * lbl_18_bss_1A2DC), 440.0f);
        textdraw_print(lbl_18_data_82678);
        textdraw_set_mul_color(0);
        textdraw_set_pos(30.0f + (20.0f - ((20.0f + tf31) * lbl_18_bss_1A2DC)), 440.0f);
        textdraw_print(tr27_4);
    }
    {
        char *sp8[6] = {lbl_18_data_8265C, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_804A8, lbl_18_data_82664};
        char *tr27_5 = sp8[locale];

        textdraw_set_scale(0.8f, 1.0f);
        tf31_2 = textdraw_get_pixel_width_of_string(tr27_5);
        textdraw_set_mul_color(0);
        textdraw_set_pos((620.0f - tf31_2) + ((20.0f + tf31_2) * lbl_18_bss_1A2DC), 440.0f);
        textdraw_print(tr27_5);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_pos(((620.0f - tf31_2) + ((20.0f + tf31_2) * lbl_18_bss_1A2DC)) - 28.0f, 440.0f);
        textdraw_print(lbl_18_data_8267C);
    }
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_mul_color(0xFFFFFF);
    req.bmpId = 0xC28;
    req.x = 320.0f;
    req.y = 264.0f;
    req.scaleX = lbl_18_bss_1A2E8;
    req.scaleY = lbl_18_bss_1A2E8;
    req.rotation = (65535.0 * (1.0 - lbl_18_bss_1A2E8));
    req.flags = (arg0->flags & 0xFFFFFFF0) | 0xA;
    draw_sprite_draw_request(&req);
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.flags = (arg0->flags & 0xFFFFFFF0) | 5;
    textdraw_set_font_style(2);
    req.scaleX = 1.0f;
    req.scaleY = lbl_18_bss_1A2EC;
    req.z = (arg0->depth - 0.001);
    textdraw_set_alpha(lbl_18_bss_1A2F0);
    fn_18_23BC8(0, arg0, &req);
    req.z = arg0->depth;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.rotation = 0;
    req.flags = (arg0->flags & 0xFFFFFFF0) | 5;
    textdraw_set_font((s32) arg0->fontId);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_depth(arg0->depth);
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_alpha(1.0f);
    textdraw_set_scale(0.68f, 0.7f);
    textdraw_set_mul_color(0xFFFFC0);
    textdraw_set_depth((float) (arg0->depth - 0.002));
    r3 = 0;
    r6 = 4;
    r7 = 4;
    r9 = 0;
    r5 = 0;
    if (((s32) modeCtrl.gameType == 0) || ((s32) modeCtrl.gameType == 8)) {
        r5 = 1;
    }
    r0 = r5 ? 1 : 0;
    switch ((s32) modeCtrl.gameType) {              /* irregular */
    case 16:
        break;
    case 15:
        r6 = 5;
        r7 = 5;
        if (lbl_18_bss_1A2D9 >= 3) {
            r9 = lbl_18_bss_1A2D9 - 2;
            r3 = lbl_18_bss_1A2D9;
        }
        break;
    }
    fn_18_24164(r3, arg0, &req, r6, r7, r0, r9);
    fn_18_25298();
}

void fn_18_25188(struct Sprite *arg0)
{
    call_free_bmp_by_id(0xC);
}

void fn_18_251AC(void)
{
    float inv;
    float nx;
    float ny;
    float dx;
    float dy;

    mtxa_from_identity();
    mtxa_translate_xyz(320.0f, 240.0f, 0.0f);
    inv = lbl_18_bss_1A314;
    inv = (1.0 / ((1.0 - inv) + (0.625 * inv)));
    mtxa_scale_xyz(inv, inv, 1.0f);
    mtxa_translate_neg_xyz(320.0f, 240.0f, 0.0f);
    nx = lbl_18_bss_1A318 / 30.0;
    dx = 120.0 * nx;
    ny = lbl_18_bss_1A31C / 30.0;
    dy = 90.0 * ny;
    mtxa_translate_xyz(-dx, -dy, 0.0f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

