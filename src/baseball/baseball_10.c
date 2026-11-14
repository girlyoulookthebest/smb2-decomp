#include "input.h"
#include "macros.h"
#include "mathutil.h"
#include "baseball.h"

struct BBBallCtl
{
    /*0x00*/ s32 flags;
    /*0x04*/ s32 unk4;
    /*0x08*/ u8 mode;
    u8 filler9[3];
    /*0x0C*/ u32 unkC;
    /*0x10*/ u8 unk10;
    /*0x11*/ u8 unk11;
    /*0x12*/ u8 unk12;
    /*0x13*/ u8 unk13;
    /*0x14*/ u8 unk14;
    /*0x15*/ u8 unk15;
    /*0x16*/ u8 unk16;
    /*0x17*/ u8 unk17;
    /*0x18*/ u8 unk18;
    u8 filler19[3];
    /*0x1C*/ float unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ float unk2C;
    /*0x30*/ float unk30;
};

struct BBBallState
{
    void (*init)();
    void (*update)();
    void (*destroy)();
    void (*input)();
};

struct BBBallState lbl_18_data_B50[8] = {
    { fn_18_A960, fn_18_AC08, fn_18_AAC0, fn_18_AAC4 },
    { fn_18_AC0C, fn_18_ACA8, fn_18_AC4C, fn_18_AC50 },
    { fn_18_ADD8, fn_18_AE60, fn_18_AE04, fn_18_AE08 },
    { fn_18_B190, fn_18_B1E8, fn_18_B1E0, fn_18_B1E4 },
    { fn_18_B5C0, fn_18_B624, fn_18_B61C, fn_18_B620 },
    { fn_18_B70C, fn_18_B718, fn_18_B710, fn_18_B714 },
    { fn_18_B8DC, fn_18_B924, fn_18_B91C, fn_18_B920 },
    { fn_18_BA1C, fn_18_BA48, fn_18_BA40, fn_18_BA44 },
};

struct BBWall lbl_18_data_BD0 = { { 0.0f, 0.5f, 0.0f }, { 0, 0, 0 }, 0, { 0.0f, 1.0f, 0.0f }, 0.6f, 0.32f };

void fn_18_A368(struct Ball *arg0)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    ctl->mode = 0;
    if (lbl_18_data_B50[ctl->mode].init != NULL) {
        lbl_18_data_B50[ctl->mode].init(arg0, ctl);
    }
}

void fn_18_A3D8(void)
{

}

void fn_18_A3DC(struct Ball *arg0)
{
    struct PhysicsBall sp8;

    fn_18_84D4(arg0);
    fn_18_A784(arg0);
    fn_18_A458(arg0, &sp8);
    fn_1_4AF48(arg0, &sp8);
    position_ball(arg0, &sp8);
    if (fn_18_C0C8() == 1) {
        fn_18_22434(&arg0->pos);
    }
    arg0->unk80++;
}

void fn_18_A458(struct Ball *arg0, struct PhysicsBall *arg1)
{
    Vec sp20;
    Vec sp14;
    Vec sp8;
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;
    struct Camera *cam = cameras;
    float tf1;
    float tf2;
    float tf31;

    sp20.x = 0.0f;
    if ((arg0->flags & 0x100) != 0) {
        sp20.y = 0.0f;
    } else {
        sp20.y = -arg0->accel;
    }
    sp20.z = 0.0f;
    mtxa_from_identity();
    mtxa_rigid_inv_tf_vec(&sp20, &sp20);
    PSVECAdd(&arg0->vel, &sp20, &arg0->vel);
    PSVECAdd(&arg0->pos, &arg0->vel, &arg0->pos);
    init_physicsball_from_ball(arg0, arg1);
    collide_ball_with_stage(arg1, stagedef);
    apply_physicsball_to_ball(arg0, arg1);
    ctl->unk17 = 0;
    if ((u32) (arg1->flags & 1) != 0) {
        if (fn_18_C154() >= 4.0 && ctl->unk18 == 0) {
            ctl->unk18 = 1;
        }
        ctl->unk17 = 1;
        if (arg1->hardestColiItemGroupId == 0) {
            arg0->unk11C.x = -arg1->hardestColiNormal.x;
            arg0->unk11C.y = -arg1->hardestColiNormal.y;
            arg0->unk11C.z = -arg1->hardestColiNormal.z;
        } else {
            mtxa_from_mtx(itemgroups[arg1->hardestColiItemGroupId].transform);
            mtxa_tf_vec(&arg1->hardestColiNormal, &arg0->unk11C);
            arg0->unk11C.x = -arg0->unk11C.x;
            arg0->unk11C.y = -arg0->unk11C.y;
            arg0->unk11C.z = -arg0->unk11C.z;
        }
    }
    if ((u32) (arg1->flags & 1) != 0) {
        tf1 = math_sqrt(mathutil_sum_of_sq_3(arg0->pos.x - cam->eye.x, arg0->pos.y - cam->eye.y, arg0->pos.z - cam->eye.z));
        sp14.x = arg0->pos.x + (arg0->unk11C.x * arg0->currRadius);
        if (tf1 < 8.0) {
            tf2 = 5.0 / tf1;
            if (tf2 > 1.0) {
                tf2 = 1.0f;
            }
            sp14.y = arg0->pos.y + (0.3 * tf2);
        } else {
            sp14.y = arg0->pos.y + (arg0->unk11C.y * arg0->currRadius);
        }
        sp14.z = arg0->pos.z + (arg0->unk11C.z * arg0->currRadius);
        sp8.x = -arg0->vel.x;
        sp8.y = -arg0->vel.y;
        sp8.z = -arg0->vel.z;
        tf31 = math_sqrt(mathutil_vec_sq_len(&arg0->vel));
        if (fn_18_A94C() != 0 && tf31 > 0.05) {
            fn_18_2248C(&sp14, &sp8, &arg1->hardestColiNormal);
        }
    }
}

void fn_18_A784(struct Ball *arg0)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    if ((u32) (ctl->flags & 1) == 0) {
        if (lbl_18_data_B50[ctl->mode].input != NULL) {
            lbl_18_data_B50[ctl->mode].input(arg0, ctl);
        }
        if (lbl_18_data_B50[ctl->mode].update != NULL) {
            lbl_18_data_B50[ctl->mode].update(arg0, ctl);
        }
    }
}

void fn_18_A860(s32 arg0, u8 arg1)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;
    struct Ball *ball = &balls[arg0];

    if (lbl_18_data_B50[ctl->mode].destroy != NULL) {
        lbl_18_data_B50[ctl->mode].destroy(ball, ctl);
    }
    ctl->mode = (u8) arg1;
    if (lbl_18_data_B50[ctl->mode].init != NULL) {
        lbl_18_data_B50[ctl->mode].init(ball, ctl);
    }
}

u8 fn_18_A94C(void)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    return ctl->mode;
}

void fn_18_A960(struct Ball *arg0)
{
    fn_18_BF20(arg0);
    fn_18_BFE0();
    arg0->flags |= 4;
    mtxa_from_translate(&arg0->pos);
    mtxa_rotate_y(arg0->rotY);
    mtxa_rotate_x(arg0->rotX);
    mtxa_rotate_z(arg0->rotZ);
    mtxa_to_mtx(arg0->unk30);
    mtxa_to_mtx(arg0->unkD0);
    arg0->speed = arg0->unkCC = 0.0f;
    {
        Vec sp18 = { 0.0f, 0.0f, 0.0f };
        Vec *p = &sp18;

        arg0->unkC0 = *p;
    }
    arg0->ape->flags &= ~0x4000;
    {
        Quat sp8 = { 0.0f, 0.0f, 0.0f, 1.0f };
        Quat *p = &sp8;

        arg0->unkB0 = *p;
    }
    arg0->unkA0 = arg0->unkB0;
    fn_18_1A04(0);
    fn_18_14A0((u8) arg0->playerId, 0);
}

void fn_18_AAC0(void)
{

}

void fn_18_AAC4(struct Ball *arg0, struct BBBallCtl *arg1)
{
    u16 spA;
    u8 sp9;
    u8 sp8;

    fn_18_13F0((s8 *) &sp9, (s8 *) &sp8, &spA);
    fn_18_BB5C(arg0, arg1, sp9, sp8);
    if ((fn_18_C190(0) == 1) && ((s32) (spA & 0x100) != 0)) {
        arg1->unk10 = 1;
        fn_18_18FD8(3);
        fn_18_A860(arg0->playerId, 1);
    }
}

void fn_18_AC08(void)
{

}

void fn_18_AC0C(struct Ball *arg0)
{
    fn_18_1A04(1);
    fn_18_14A0((u8) arg0->playerId, 5);
}

void fn_18_AC4C(void)
{

}

void fn_18_AC50(struct Ball *arg0, struct BBBallCtl *arg1)
{
    u16 spA;
    u8 sp9;
    u8 sp8;

    fn_18_13F0((s8 *) &sp9, (s8 *) &sp8, &spA);
    fn_18_BCA4(arg0, arg1, sp9, sp8);
}

void fn_18_ACA8(struct Ball *arg0, struct BBBallCtl *arg1)
{

    if ((fn_18_C190(5) == 1) && (fn_18_1584(arg0->ape) != 0)) {
        arg0->vel.x = arg1->unk1C;
        arg0->vel.y = 0.0f;
        arg0->vel.z = arg1->unk24;
        fn_18_A860(arg0->playerId, 2);
    }
}

void fn_18_ADD8(struct Ball *arg0)
{
    fn_18_14A0((u8) arg0->playerId, 1);
}

void fn_18_AE04(void)
{

}

void fn_18_AE08(struct Ball *arg0, struct BBBallCtl *arg1)
{
    u16 spA;
    u8 sp9;
    u8 sp8;

    fn_18_13F0((s8 *) &sp9, (s8 *) &sp8, &spA);
    fn_18_BDF0(arg0, arg1, sp9, sp8);
}

void fn_18_AE60(struct Ball *arg0, struct BBBallCtl *arg1)
{
    struct PhysicsBall sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;

    if (arg0->vel.z < 0.1f) {
        arg0->vel.z = 0.1f;
    }
    arg0->vel.z += arg1->unk30;
    if (fabs(arg0->vel.z) > 0.3) {
        if (arg0->vel.z > 0.0) {
            arg0->vel.z -= 0.03;
        } else {
            arg0->vel.z += 0.03;
        }
    }
    if (fabs(arg0->vel.x) > 0.01) {
        if (arg0->vel.x > 0.0) {
            arg0->vel.x -= 0.001;
        } else {
            arg0->vel.x += 0.001;
        }
    } else {
        arg0->vel.x += arg1->unk28;
    }
    init_physicsball_from_ball(arg0, &sp2C);
    fn_18_C038(&sp2C);
    if ((fn_18_13CF4(&sp20, &sp14, &sp8) == 1) && (fn_18_141F8() == 1)) {
        if (fn_18_199EC() == 2) {
            sp20.y *= 0.5;
        }
        arg0->vel = sp20;
        fn_18_223E4(&sp14, &sp8, 3.0f);
        fn_18_18E30(0x5F);
        fn_18_13CD4();
        fn_18_A860(arg0->playerId, 3);
    } else if (arg0->pos.z > 3.5f) {
        fn_18_A860(arg0->playerId, 4);
    }
}

void fn_18_B190(struct Ball *arg0, struct BBBallCtl *arg1)
{
    fn_18_1A04(4);
    fn_18_18ED8(0);
    arg1->unk18 = 0;
    arg1->unk14 = 1;
    arg1->unk16 = 0;
}

void fn_18_B1E0(void)
{

}

void fn_18_B1E4(void)
{

}

void fn_18_B1E8(struct Ball *arg0, struct BBBallCtl *arg1)
{
    struct PhysicsBall sp14;
    Vec sp8;

    init_physicsball_from_ball(arg0, &sp14);
    if ((fn_18_175A4(&sp8, &sp14) == 1) && (fn_18_1888C() == 0)) {
        arg1->unk13 = 1;
        arg1->unk12 = 1;
        PSVECScale(&sp8, &sp8, 0.8f);
        arg0->vel = sp8;
        fn_18_18E30(0x64);
        fn_18_A860(arg0->playerId, 7);
    }
    if (arg1->unk16 == 0) {
        init_physicsball_from_ball(arg0, &sp14);
        if ((s16) fn_18_16B1C(&sp14) != -1) {
            fn_18_18E30(0x77);
            arg1->unk16 = 1;
        }
    }
    fn_18_C28C(arg0, arg1);
    if (math_sqrt(mathutil_vec_sq_len(&arg0->vel)) < 0.01) {
        arg1->unkC++;
    } else {
        arg1->unkC = 0;
    }
    if (arg1->unkC > 0x28) {
        arg0->vel.x = 0.0f;
        arg0->vel.y = 0.0f;
        arg0->vel.z = 0.0f;
        arg0->unk60.x = 0;
        arg0->unk60.y = 0;
        arg0->unk60.z = 0;
        arg0->prevPos = arg0->pos;
        fn_18_1A04(6);
        fn_18_A860(arg0->playerId, 5);
    }
    if (fn_18_3EF4() == 1) {
        fn_18_1A04(6);
        fn_18_A860(arg0->playerId, 5);
    }
}

void fn_18_B5C0(struct Ball *arg0, struct BBBallCtl *arg1)
{
    fn_18_1A04(5);
    arg1->unk15 = 1;
    arg0->vel.x = arg0->vel.y = arg0->vel.z = 0.0f;
}

void fn_18_B61C(void)
{

}

void fn_18_B620(void)
{

}

void fn_18_B624(struct Ball *arg0)
{

    fn_18_A860(arg0->playerId, 5);
}

void fn_18_B70C(void)
{

}

void fn_18_B710(void)
{

}

void fn_18_B714(void)
{

}

void fn_18_B718(struct Ball *arg0)
{

    if (fn_18_3EF4() == 1) {
        fn_18_A860(arg0->playerId, 0);
    } else if (fn_18_3EF4() == 4) {
        fn_18_A860(arg0->playerId, 6);
    }
}

void fn_18_B8DC(struct Ball *arg0)
{
    fn_18_BF20(arg0);
    fn_18_14A0((u8) arg0->playerId, 5);
}

void fn_18_B91C(void)
{

}

void fn_18_B920(void)
{

}

void fn_18_B924(struct Ball *arg0)
{

    if (fn_18_3EF4() == 1) {
        fn_18_A860(arg0->playerId, 0);
    }
}

void fn_18_BA1C(void)
{
    fn_18_1A04(7);
}

void fn_18_BA40(void)
{

}

void fn_18_BA44(void)
{

}

void fn_18_BA48(struct Ball *arg0)
{

    arg0->vel.y += 0.05;
    if (fn_18_3EF4() == 1) {
        fn_18_A860(arg0->playerId, 0);
    }
}

void fn_18_BB5C(struct Ball *arg0, struct BBBallCtl *arg1, u8 arg2, u8 arg3)
{
    float sp8[2];
    float tf2;
    float tf31;

    tf31 = mathutil_sum_of_sq_2((s8) arg2 / 60.0f, (s8) arg3 / 60.0f);
    math_sin_cos_v(math_atan2((float) (s8) arg2, (float) (s8) arg3), sp8);
    tf2 = 0.025f * tf31 * sp8[0];
    if ((__abs((s8) arg2) > 0xF) && (fabs(arg0->pos.x + tf2) < 0.4)) {
        arg0->pos.x += tf2;
    }
}

void fn_18_BCA4(struct Ball *arg0, struct BBBallCtl *arg1, u8 arg2, u8 arg3)
{
    float sp8[2];
    float tf1;
    float tf2;
    float tf3;
    float tf4;

    math_sin_cos_v(math_atan2((float) (s8) arg2, (float) (s8) arg3), sp8);
    tf2 = (float) fabs((s8) arg2 / 60.0f);
    tf4 = (float) fabs((s8) arg3 / 60.0f);
    tf3 = 0.02f * tf2 * sp8[0];
    tf1 = 0.1f + (0.3f * tf4 * -sp8[1]);
    arg1->unk1C = tf3;
    if (tf1 >= 0.1f) {
        arg1->unk24 = tf1;
    } else {
        arg1->unk24 = 0.1f;
    }
}

void fn_18_BDF0(struct Ball *arg0, struct BBBallCtl *arg1, u8 arg2, u8 arg3)
{
    float sp8[2];
    float tf0;
    float tf2;
    float tf3;

    math_sin_cos_v(math_atan2((float) (s8) arg2, (float) (s8) arg3), sp8);
    tf2 = (float) fabs((s8) arg2 / 60.0f);
    tf3 = (float) fabs((s8) arg3 / 60.0f);
    tf2 = 0.0019999999552965165 * sp8[0] * tf2;
    tf0 = 0.030000001192092896 * -sp8[1] * tf3;
    arg1->unk28 = tf2;
    arg1->unk30 = tf0;
}

void fn_18_BF20(struct Ball *arg0)
{
    arg0->vel.x = 0.0f;
    arg0->vel.y = 0.0f;
    arg0->vel.z = 0.0f;
    arg0->rotX = 0x2000;
    arg0->rotY = 0x4000;
    arg0->rotZ = 0;
    arg0->unk60.x = 0;
    arg0->unk60.y = 0;
    arg0->unk60.z = 0;
    arg0->pos.x = 0.0f;
    arg0->pos.y = 0.5f;
    arg0->pos.z = -10.5f;
    arg0->prevPos.x = 0.0f;
    arg0->prevPos.y = 0.5f;
    arg0->prevPos.z = -10.5f;
    mtxa_from_identity();
    mtxa_rotate_y(-0x8000);
    mtxa_to_quat(&arg0->ape->rot);
}

void fn_18_BFE0(void)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    ctl->unk11 = 0;
    ctl->unk12 = 0;
    ctl->unk13 = 0;
    ctl->unk14 = 0;
    ctl->unk15 = 0;
    ctl->unk10 = 0;
    ctl->unkC = 0;
    ctl->unk17 = 0;
    ctl->unk18 = 0;
}

void fn_18_C038(struct PhysicsBall *arg0)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    fn_18_16C48(arg0, &lbl_18_data_BD0);
    if ((u32) (arg0->flags & 1) != 0) {
        ctl->unk11 = 1;
    }
}

s32 fn_18_C09C(void)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    return (u8) ctl->unk11 == 1;
}

s32 fn_18_C0C8(void)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    return (u8) ctl->unk12 == 1;
}

u8 fn_18_C0F4(void)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    return (u8) ((u8) ctl->unk14 == 1);
}

u8 fn_18_C124(void)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    return (u8) ((u8) ctl->unk15 == 1);
}

float fn_18_C154(void)
{
    struct Ball *ball = &balls[1];
    register float x = ball->pos.x;
    register float z = ball->pos.z;

    x = x * x;
    x = __fmadds(z, z, x);
    return math_sqrt(x);
}

s32 fn_18_C190(s16 arg0)
{
    struct Ape *ape = balls[1].ape;

    if ((ape->unk6C == 0x64) && (ape->unk70 == arg0)) {
        return 1;
    }
    return 0;
}

void fn_18_C1C8(s32 arg0)
{
    struct Ball *ball = &balls[1];

    if (arg0 == 1) {
        ball->flags &= ~0x10;
        ball->ape->flags &= ~0x20;
    } else {
        ball->flags |= 0x10;
        ball->ape->flags |= 0x20;
    }
}

void fn_18_C244(s32 arg0)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    if (arg0 == 1) {
        ctl->flags |= 1;
    } else {
        ctl->flags &= ~1;
    }
}

u8 fn_18_C278(void)
{
    struct BBBallCtl *ctl = (struct BBBallCtl *) &lbl_18_bss_970.unk88;

    return ctl->unk10;
}

void fn_18_C28C(struct Ball *arg0, struct BBBallCtl *arg1)
{
    float tf1;
    u16 r30;
    u8 r0;

    tf1 = math_sqrt(mathutil_vec_sq_len(&arg0->vel));
    if (tf1 < 0.01) {
        r0 = 0;
    } else if (tf1 < 0.03) {
        r0 = 1;
    } else if (tf1 < 0.06) {
        r0 = 2;
    } else {
        r0 = 3;
    }
    switch (r0) {
    case 0:
        r30 = 4;
        break;
    case 1:
        r30 = 3;
        break;
    case 2:
        r30 = 2;
        break;
    case 3:
        r30 = 1;
        break;
    }
    if (fn_18_1584(arg0->ape) != 0) {
        fn_18_14A0((u8) arg0->playerId, r30);
    }
}

