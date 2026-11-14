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

void fn_18_E9C(void)
{
    memset(&lbl_18_bss_970, 0, 0x26C);
}

void fn_18_ECC(u8 arg0, u8 arg1)
{
    struct BBPadInput *pad = (struct BBPadInput *) &lbl_18_bss_970.unkA;
    s32 r6;
    u8 tr3;

    tr3 = fn_18_19E0();
    for (r6 = 0; r6 < 2; r6++) {
        pad->stickX[r6] = 0;
        pad->stickY[r6] = 0;
        pad->button[r6] = 0;
    }
    pad->unk0 = arg0;
    pad->unk1 = arg1;
    switch (tr3) {
    case 0:
    default:
        pad->unk2 = pad->unk0;
        pad->unk3 = pad->unk1;
        return;
    case 1:
        pad->unk2 = pad->unk1;
        pad->unk3 = pad->unk0;
        return;
    case 2:
        pad->unk2 = pad->unk1;
        pad->unk3 = pad->unk0;
        return;
    }
}

void fn_18_FB8(u8 arg0, u8 arg1)
{
    struct BBPadInput *pad = (struct BBPadInput *) &lbl_18_bss_970.unkA;

    pad->stickX[arg0] = pad_status_groups[arg1].unk0[0].stickX;
    pad->stickY[arg0] = pad_status_groups[arg1].unk0[0].stickY;
}

void fn_18_1010(u8 arg0, s8 *arg1, s8 *arg2)
{
    struct BBPadInput *pad = (struct BBPadInput *) &lbl_18_bss_970.unkA;

    *arg1 = pad->stickX[arg0];
    *arg2 = pad->stickY[arg0];
}

void fn_18_1040(u8 arg0, u8 arg1)
{
    struct BBPadInput *pad = (struct BBPadInput *) &lbl_18_bss_970.unkA;

    pad->button[arg0] = (u16) ((u16) (pad_status_groups[arg1].unk0[0].button & 0x60) | pad_status_groups[arg1].unk0[2].button);
}

void fn_18_10A0(u8 arg0, u16 *arg1)
{
    struct BBPadInput *pad = (struct BBPadInput *) &lbl_18_bss_970.unkA;

    *arg1 = pad->button[arg0];
}

void fn_18_10C4(void)
{
    u8 tr31;

    tr31 = lbl_18_bss_970.unkC;
    if (fn_18_1A7C() == 1) {
        if (fn_18_1A9C() == 0) {
            fn_18_FB8(0, tr31);
            fn_18_1040(0, tr31);
            return;
        }
        fn_18_2627C();
        return;
    }
    fn_18_FB8(0, tr31);
    fn_18_1040(0, tr31);
}

void fn_18_1240(s8 *arg0, s8 *arg1, u16 *arg2)
{
    fn_18_1010(0, arg0, arg1);
    fn_18_10A0(0, arg2);
}

void fn_18_1274(void)
{
    u8 tr31;

    tr31 = lbl_18_bss_970.unkD;
    if (fn_18_1A7C() == 1) {
        if (fn_18_1A9C() == 1) {
            fn_18_FB8(1, tr31);
            fn_18_1040(1, tr31);
            return;
        }
        fn_18_26104();
        return;
    }
    fn_18_FB8(1, tr31);
    fn_18_1040(1, tr31);
}

void fn_18_13F0(s8 *arg0, s8 *arg1, u16 *arg2)
{
    fn_18_1010(1, arg0, arg1);
    fn_18_10A0(1, arg2);
}

void fn_18_1424(void)
{
    struct BBPadInput *pad = (struct BBPadInput *) &lbl_18_bss_970.unkA;

    if (pad->unk2 == pad->unk0) {
        pad->unk2 = pad->unk1;
        pad->unk3 = pad->unk0;
        return;
    }
    pad->unk2 = pad->unk0;
    pad->unk3 = pad->unk1;
}

void fn_18_146C(u8 arg0, u16 arg1, u16 arg2)
{
    lbl_18_bss_970.unk22E[arg0].unk0 = arg1;
    lbl_18_bss_970.unk22E[arg0].unk2 = arg2;
}

void fn_18_14A0(u8 arg0, u16 arg1)
{
    lbl_18_bss_970.unk22E[arg0].unk0 = 0;
    lbl_18_bss_970.unk22E[arg0].unk2 = arg1;
}

void fn_18_14D8(u8 arg0, u16 arg1)
{
    lbl_18_bss_970.unk22E[arg0].unk0 = 2;
    lbl_18_bss_970.unk22E[arg0].unk2 = arg1;
}

void fn_18_1510(u8 arg0, u16 arg1)
{
    lbl_18_bss_970.unk22E[arg0].unk0 = 1;
    lbl_18_bss_970.unk22E[arg0].unk2 = arg1;
}

void fn_18_1548(u8 arg0, u16 *arg1, u16 *arg2)
{
    *arg1 = lbl_18_bss_970.unk22E[arg0].unk0;
    *arg2 = lbl_18_bss_970.unk22E[arg0].unk2;
}

s32 fn_18_1584(struct Ape *arg0)
{
    float *p;

    if (arg0 == NULL) {
        return 0;
    }
    /* bodyAnim is the ape's Skeleton; +0x40 is its AnimClock (speed, frame,
     * length at p[2..4]).  Skeleton grows on x64, so use the field. */
    p = (float *) &((struct Skeleton *) arg0->bodyAnim)->clock;
    return p[3] > p[4] - p[2];
}

float fn_18_15BC(float farg0, float *arg0, float *arg1)
{
    float sp8[7];

    sp8[0] = arg0[0];
    sp8[1] = arg1[0];
    sp8[2] = farg0;
    sp8[3] = arg0[1];
    sp8[4] = arg1[1];
    sp8[5] = arg0[3];
    sp8[6] = arg1[2];
    return fn_18_1614(sp8);
}

float fn_18_1614(float *arg0)
{
    float h00;
    float h10;
    float h11;
    float h01;
    float dt;
    float t;
    float u;

    dt = 0.033333335f * (arg0[1] - arg0[0]);
    t = (1.0f / dt) * (0.033333335f * (arg0[2] - arg0[0]));
    u = 1.0f - t;
    h00 = (1.0f + (2.0f * t)) * (u * u);
    h10 = t * (u * u);
    h11 = -(u * (t * t));
    h01 = (3.0f - (2.0f * t)) * (t * t);
    return (arg0[4] * h01) + ((h11 * (arg0[6] * dt)) + ((arg0[3] * h00) + (h10 * (arg0[5] * dt))));
}

