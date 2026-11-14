#include "macros.h"
#include "mathutil.h"
/* This caller uses the full integer return value; the shared declaration is u8. */
#define fn_18_C278 fn_18_C278_u8
#include "baseball.h"
#undef fn_18_C278
extern s32 fn_18_C278(void);

struct BBFielderState
{
    void (*init)();
    void (*update)();
    void (*destroy)();
    void (*input)();
};

struct BBGame88
{
    u8 filler0[0x17];
    u8 unk17;
    u8 unk18;
};

struct BBFielderState lbl_18_data_BF8[5] = {
    { fn_18_C640, fn_18_CA04, fn_18_C900, fn_18_C904 },
    { fn_18_CBD8, fn_18_CC6C, fn_18_CBE4, fn_18_CBE8 },
    { fn_18_D178, fn_18_D1AC, fn_18_D1A4, fn_18_D1A8 },
    { fn_18_D3D0, fn_18_D404, fn_18_D3FC, fn_18_D400 },
    { fn_18_DA58, fn_18_DA8C, fn_18_DA84, fn_18_DA88 },
};

struct BBFieldPos lbl_18_data_C48[8][4] = {
    {
        { { 6.5f, 0.0f, -18.0f }, 3.5f, 17.0f },
        { { -6.5f, 0.0f, -18.0f }, -17.0f, -3.5f },
        { { 15.0f, 0.0f, -26.0f }, 2.5f, 16.0f },
        { { -15.0f, 0.0f, -26.0f }, -16.0f, -2.5f },
    },
    {
        { { -1e+01f, 0.0f, -15.0f }, -10.5f, 3.5f },
        { { 0.0f, 0.0f, -18.0f }, -7.0f, 7.0f },
        { { -15.0f, 0.0f, -26.0f }, -16.0f, -2.5f },
        { { -9.0f, 0.0f, -4e+01f }, -12.0f, 2.0f },
    },
    {
        { { 1e+01f, 0.0f, -15.0f }, -3.5f, 10.5f },
        { { 0.0f, 0.0f, -22.0f }, -7.0f, 7.0f },
        { { 15.0f, 0.0f, -26.0f }, 2.5f, 16.0f },
        { { 9.0f, 0.0f, -4e+01f }, -2.0f, 11.5f },
    },
    {
        { { -1e+01f, 0.0f, -15.0f }, -10.5f, 3.5f },
        { { 12.0f, 0.0f, -18.0f }, 3.0f, 17.0f },
        { { 8.0f, 0.0f, -35.0f }, 1.0f, 14.5f },
        { { -8.0f, 0.0f, -26.0f }, -14.0f, 0.5f },
    },
    {
        { { 13.0f, 0.0f, -16.5f }, 1.5f, 15.0f },
        { { -6.5f, 0.0f, -18.0f }, -16.5f, -3.0f },
        { { 6.5f, 0.0f, -26.0f }, 2.5f, 16.5f },
        { { 0.0f, 0.0f, -4e+01f }, -7.0f, 7.0f },
    },
    {
        { { 3.0f, 0.0f, -15.0f }, -3.0f, 10.5f },
        { { -3.0f, 0.0f, -18.0f }, -12.0f, -1.5f },
        { { 3.0f, 0.0f, -26.0f }, -4.5f, 9.0f },
        { { -1.5f, 0.0f, -4e+01f }, -7.0f, 6.5f },
    },
    {
        { { 8.0f, 0.0f, -15.0f }, -3.0f, 10.5f },
        { { -12.0f, 0.0f, -18.0f }, -17.0f, -3.0f },
        { { 6.5f, 0.0f, -26.0f }, 2.5f, 16.5f },
        { { -6.0f, 0.0f, -26.0f }, -11.5f, 2.0f },
    },
    {
        { { 3.0f, 0.0f, -15.0f }, -3.0f, 10.5f },
        { { -15.0f, 0.0f, -18.0f }, -17.0f, -3.5f },
        { { 15.0f, 0.0f, -26.0f }, 2.0f, 16.0f },
        { { -5.0f, 0.0f, -4e+01f }, -12.0f, 2.0f },
    },
};

Vec lbl_18_data_EC8[7][3] = {
    {
        { 12.3647f, 0.0f, -14.4991f },
        { 11.4462f, 0.0f, -13.5806f },
        { 10.4839f, 0.0f, -12.6184f },
    },
    {
        { 18.2499f, 0.0f, -25.1036f },
        { 17.3313f, 0.0f, -24.1851f },
        { 16.3691f, 0.0f, -22.2229f },
    },
    {
        { 10.6416f, 0.0f, -32.573f },
        { 10.1445f, 0.0f, -31.3729f },
        { 9.6237f, 0.0f, -30.1157f },
    },
    {
        { 0.0f, 0.0f, -35.2756f },
        { 0.0f, 0.0f, -33.9767f },
        { 0.0f, 0.0f, -32.6158f },
    },
    {
        { -10.6416f, 0.0f, -32.573f },
        { -10.1445f, 0.0f, -31.3729f },
        { -9.6237f, 0.0f, -30.1157f },
    },
    {
        { -18.2499f, 0.0f, -25.1036f },
        { -17.3313f, 0.0f, -24.1851f },
        { -16.3691f, 0.0f, -22.2229f },
    },
    {
        { -12.3647f, 0.0f, -14.4991f },
        { -11.4462f, 0.0f, -13.5806f },
        { -10.4839f, 0.0f, -12.6184f },
    },
};

static inline void bb_set_c0(struct Ball *ball, Vec v)
{
    ball->unkC0 = v;
}

static inline void bb_set_rot(struct Ball *ball, Quat q)
{
    ball->unkB0 = q;
    ball->unkA0 = ball->unkB0;
}

static inline void bb_fielder_set_state(s32 id, u8 state)
{
    struct BBGamePl *pl = &lbl_18_bss_970.unkBC[id];
    struct Ball *ball = &balls[id];

    if (lbl_18_data_BF8[pl->unk5].destroy != NULL) {
        lbl_18_data_BF8[pl->unk5].destroy(ball, pl);
    }
    pl->unk5 = state;
    if (lbl_18_data_BF8[pl->unk5].init != NULL) {
        lbl_18_data_BF8[pl->unk5].init(ball, pl);
    }
}

u8 fn_18_C394(void)
{
    struct BBGame88 *p = (struct BBGame88 *) &lbl_18_bss_970.unk88;

    return p->unk17;
}

u8 fn_18_C3A8(void)
{
    struct BBGame88 *p = (struct BBGame88 *) &lbl_18_bss_970.unk88;

    return p->unk18;
}

void fn_18_C3BC(void)
{
    struct BBGame88 *p = (struct BBGame88 *) &lbl_18_bss_970.unk88;

    p->unk18 = 2;
}

void fn_18_C3D4(struct Ball *arg0)
{
    struct BBGamePl *tr31 = &lbl_18_bss_970.unkBC[arg0->playerId];

    fn_18_E06C((u8) arg0->playerId);
    tr31->unk5 = 0;
    if (lbl_18_data_BF8[tr31->unk5].init != NULL) {
        lbl_18_data_BF8[tr31->unk5].init(arg0, tr31);
    }
}

void fn_18_C47C(void)
{
    float dead = 0.0f;
}

void fn_18_C480(struct Ball *arg0)
{
    struct PhysicsBall sp8;
    struct BBGamePl *tr31 = &lbl_18_bss_970.unkBC[arg0->playerId];

    fn_18_84D4(arg0);
    fn_18_C518(arg0);
    fn_18_DD38(arg0);
    fn_18_C610(arg0, &sp8);
    fn_18_8558(arg0);
    fn_18_8784(arg0);
    fn_18_15218(arg0, tr31);
    arg0->unk80++;
}

void fn_18_C518(struct Ball *arg0)
{
    struct BBGamePl *tr31 = &lbl_18_bss_970.unkBC[arg0->playerId];

    if (lbl_18_data_BF8[tr31->unk5].input != NULL) {
        lbl_18_data_BF8[tr31->unk5].input(arg0, tr31);
    }
    if (lbl_18_data_BF8[tr31->unk5].update != NULL) {
        lbl_18_data_BF8[tr31->unk5].update(arg0, tr31);
    }
}

u8 fn_18_C5F4(s32 arg0)
{
    struct BBGamePl *pl = &lbl_18_bss_970.unkBC[arg0];

    return pl->unk5;
}

void fn_18_C610(struct Ball *arg0, struct PhysicsBall *arg1)
{
    PSVECAdd(&arg0->pos, &arg0->vel, &arg0->pos);
}

void fn_18_C640(struct Ball *arg0, struct BBGamePl *arg1)
{
    Vec sp24 = { 0.0f, 1.0f, 0.0f };
    Mtx sp30;

    quat_from_axis_angle(&arg0->ape->rot, &sp24, -0x8000);
    mtxa_from_quat(&arg0->ape->rot);
    mtxa_to_mtx(sp30);
    arg0->pos.x = arg0->pos.y = arg0->pos.z = 0.0f;
    arg0->prevPos = arg0->pos;
    arg0->vel.x = arg0->vel.y = arg0->vel.z = 0.0f;
    arg0->rotX = 0x2000;
    arg0->rotY = 0x4000;
    arg0->rotZ = 0;
    arg0->unk60.x = arg0->unk60.y = arg0->unk60.z = 0;
    arg0->pos.x = lbl_18_data_C48[arg1->unk6][arg0->playerId - 2].pos.x;
    arg0->pos.y = 0.5f;
    arg0->pos.z = lbl_18_data_C48[arg1->unk6][arg0->playerId - 2].pos.z;
    arg0->prevPos = arg0->pos;
    arg1->unk10 = lbl_18_data_C48[arg1->unk6][arg0->playerId - 2].unkC;
    arg1->unk14 = lbl_18_data_C48[arg1->unk6][arg0->playerId - 2].unk10;
    mtxa_from_translate(&arg0->pos);
    mtxa_mult_right(sp30);
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
        arg0->unkA0 = arg0->unkB0;
    }
    arg1->unkC = 0;
    fn_18_14D8((u8) arg0->playerId, 1);
    fn_18_151B8(arg0);
}

void fn_18_C900(void)
{

}

void fn_18_C904(struct Ball *arg0, struct BBGamePl *arg1)
{
    u16 spA;
    u8 sp9;
    u8 sp8;

    if (arg1->unkC == 0) {
        if (fn_18_C278() == 0) {
            fn_18_13F0((s8 *) &sp9, (s8 *) &sp8, &spA);
            if ((s32) (spA & 0x20) != 0) {
                fn_18_18E30(0x67);
                arg1->unk6 = (arg1->unk6 < 7) ? arg1->unk6 + 1 : 0;
            } else if ((s32) (spA & 0x40) != 0) {
                fn_18_18E30(0x67);
                arg1->unk6 = (arg1->unk6 != 0) ? arg1->unk6 - 1 : 7;
            }
            arg1->unkC = 0xA;
        }
    } else {
        arg1->unkC = (u32) (arg1->unkC - 1);
    }
    fn_18_DE1C(arg0, arg1->unk6, (struct BBAim *) arg1);
}

void fn_18_CA04(struct Ball *arg0)
{

    if (fn_18_1A14() == 4) {
        bb_fielder_set_state(arg0->playerId, 1);
    } else if (fn_18_1A14() == 5) {
        bb_fielder_set_state(arg0->playerId, 1);
    }
}

void fn_18_CBD8(struct Ball *arg0, struct BBGamePl *arg1)
{
    arg1->unk7 = 0;
}

void fn_18_CBE4(void)
{

}

void fn_18_CBE8(struct Ball *arg0, struct BBGamePl *arg1)
{
    u16 spA;
    u8 sp9;
    u8 sp8;

    fn_18_13F0((s8 *) &sp9, (s8 *) &sp8, &spA);
    if (fn_18_DB8C(arg0, arg1, sp9, sp8) == 1) {
        fn_18_14D8((u8) arg0->playerId, 2);
        return;
    }
    fn_18_14D8((u8) arg0->playerId, 1);
}

void fn_18_CC6C(struct Ball *arg0, struct BBGamePl *arg1)
{
    struct PhysicsBall sp6C;
    struct PhysicsBall spC;
    s16 spA;
    s16 sp8;

    init_physicsball_from_ball(arg0, &sp6C);
    init_physicsball_from_ball(&balls[1], &spC);
    if (fn_18_1722C(&sp6C, &spC, 0.1f) == 1) {
        fn_18_18E30(0x73);
        arg1->unk7 = 1;
        balls[1].vel.x = 0.0f;
        balls[1].vel.y = 0.0f;
        balls[1].vel.z = 0.0f;
        balls[1].unk60.x = 0;
        balls[1].unk60.y = 0;
        balls[1].unk60.z = 0;
        balls[1].prevPos = balls[1].pos;
        fn_18_8558(&balls[1]);
        init_physicsball_from_ball(arg0, &sp6C);
        init_physicsball_from_ball(&balls[1], &spC);
        fn_18_1722C(&sp6C, &spC, 0.1f);
        apply_physicsball_to_ball(&balls[1], &spC);
        ray_to_euler_xy(&balls[1].pos, &arg0->pos, &spA, &sp8);
        mtxa_from_identity();
        mtxa_rotate_z(0);
        mtxa_rotate_y(sp8);
        mtxa_rotate_x(spA);
        mtxa_to_quat(&balls[1].ape->rot);
        bb_fielder_set_state(arg0->playerId, 2);
    } else if (fn_18_1A14() == 6) {
        bb_fielder_set_state(arg0->playerId, 2);
    } else if (fn_18_1A14() == 7) {
        bb_fielder_set_state(arg0->playerId, 2);
    } else if (fn_18_1A14() == 5) {
        bb_fielder_set_state(arg0->playerId, 2);
    }
}

void fn_18_D178(struct Ball *arg0)
{
    fn_18_14D8((u8) arg0->playerId, 0);
}

void fn_18_D1A4(void)
{

}

void fn_18_D1A8(void)
{

}

void fn_18_D1AC(struct Ball *arg0, struct BBGamePl *arg1)
{

    if (fn_18_3EF4() == 1) {
        bb_fielder_set_state(arg0->playerId, 0);
    } else if ((fn_18_3EF4() == 8) && (arg1->unk8 == 1)) {
        bb_fielder_set_state(arg0->playerId, 3);
    }
}

s32 fn_18_D390(void)
{
    struct BBGamePl *pl = &lbl_18_bss_970.unkBC[2];
    s32 i;

    for (i = 2; i <= 5; i++, pl++) {
        if (pl->unk7 == 1) {
            return 1;
        }
    }
    return 0;
}

void fn_18_D3D0(struct Ball *arg0)
{
    fn_18_14D8((u8) arg0->playerId, 2);
}

void fn_18_D3FC(void)
{

}

void fn_18_D400(void)
{

}

void fn_18_D404(struct Ball *arg0, struct BBGamePl *arg1)
{
    struct Ball *tr3;
    struct PhysicsBall sp68;
    struct PhysicsBall sp8;

    init_physicsball_from_ball(arg0, &sp68);
    init_physicsball_from_ball(&balls[1], &sp8);
    if (fn_18_1722C(&sp68, &sp8, 0.1f) == 1) {
        arg1->unk7 = 1;
        balls[1].vel.x = 0.0f;
        balls[1].vel.y = 0.0f;
        balls[1].vel.z = 0.0f;
        bb_fielder_set_state(arg0->playerId, 4);
    } else {
        fn_18_D758(arg0, arg1);
        fn_18_D7F0(arg0);
    }
    tr3 = &balls[1];
    if (__fabs(tr3->pos.y - arg0->pos.y) > 1.0) {
        bb_fielder_set_state(arg0->playerId, 4);
    }
    if (fn_18_3EF4() != 8) {
        bb_fielder_set_state(arg0->playerId, 4);
    }
}

void fn_18_D758(struct Ball *arg0, struct BBGamePl *arg1)
{
    float sp8[2];
    struct Ball *tr3;

    tr3 = &balls[1];
    math_sin_cos_v(math_atan2(tr3->pos.x - arg0->pos.x, tr3->pos.z - arg0->pos.z), sp8);
    arg0->pos.x += 0.15f * sp8[0];
    arg0->pos.z += 0.15f * sp8[1];
}

void fn_18_D7F0(struct Ball *arg0)
{
    struct PhysicsBall sp68;
    struct PhysicsBall sp8;
    float tf31;
    s32 gate;
    u8 collided;
    u8 layout;
    s32 endpoint;
    struct Ball *other;

    collided = 0;
    other = &balls[1];
    layout = fn_18_14AF4();
    {
        register float z;
        register float x;

        x = other->pos.x - arg0->pos.x;
        z = other->pos.z - arg0->pos.z;

#ifdef __MWERKS__
        asm {
            fmuls x, x, x
            fmadds x, z, z, x
        }
#else
        x = x * x + z * z;
#endif
        tf31 = math_sqrt(x);
    }
    init_physicsball_from_ball(arg0, &sp68);
    for (gate = 0; gate < 7; gate++) {
        if (lbl_18_data_78BC8[layout][gate] != 0) {
            if (tf31 > 1.5) {
                fn_18_D9A4(&sp8, &lbl_18_data_EC8[gate][1], 2.5395f, arg0->accel);
            } else {
                for (endpoint = 0; endpoint < 3; endpoint++) {
                    if (endpoint == 1) {
                        fn_18_D9A4(&sp8, &lbl_18_data_EC8[gate][endpoint], 2.0f, arg0->accel);
                    } else {
                        fn_18_D9A4(&sp8, &lbl_18_data_EC8[gate][endpoint], 1.5f, arg0->accel);
                    }
                }
            }
            if (fn_18_1722C(&sp68, &sp8, 0.1f) == 1) {
                sp68.pos.y = 0.5f;
                sp68.vel.y = 0.0f;
                collided = 1;
            }
        }
    }
    if (collided != 0) {
        apply_physicsball_to_ball(arg0, &sp68);
    }
}

void fn_18_D9A4(struct PhysicsBall *arg0, Vec *arg1, float farg0, float farg1)
{
    arg0->flags = 0;
    arg0->pos.x = arg1->x;
    arg0->pos.y = arg1->y;
    arg0->pos.z = arg1->z;
    arg0->prevPos.x = arg1->x;
    arg0->prevPos.y = arg1->y;
    arg0->prevPos.z = arg1->z;
    arg0->vel.x = 0.0f;
    arg0->vel.y = 0.0f;
    arg0->vel.z = 0.0f;
    arg0->radius = farg0;
    arg0->gravityAccel = farg1;
    arg0->restitution = 0.0f;
    arg0->hardestColiSpeed = 0.0f;
    arg0->itemGroupId = 0;
    arg0->hardestColiItemGroupId = 0;
    arg0->friction = 0.01f;
}

void fn_18_DA58(struct Ball *arg0)
{
    fn_18_14D8((u8) arg0->playerId, 0);
}

void fn_18_DA84(void)
{

}

void fn_18_DA88(void)
{

}

void fn_18_DA8C(struct Ball *arg0)
{

    if (fn_18_3EF4() == 1) {
        bb_fielder_set_state(arg0->playerId, 0);
    }
}

s32 fn_18_DB8C(struct Ball *arg0, struct BBGamePl *arg1, s32 arg2, s32 arg3)
{
    float sp8[2];
    float tf31;
    float tf30;
    float tf2;
    float tf3;

    tf31 = __fabs((float) (s8) arg2 / 60.0f);
    tf30 = __fabs((float) (s8) arg3 / 60.0f);
    math_sin_cos_v(math_atan2((float) (s8) arg2, (float) (s8) arg3), sp8);
    tf2 = 0.15f * tf31 * sp8[0];
    tf3 = 0.15f * tf30 * sp8[1];
    if (__abs((s8) arg2) > 0xF && arg1->unk10 < arg0->pos.x + tf2 && arg1->unk14 > arg0->pos.x + tf2) {
        arg0->pos.x += tf2;
        return 1;
    }
    return 0;
}

