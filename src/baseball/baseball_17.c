#include "input.h"
#include "macros.h"
#include "mathutil.h"
#include "baseball.h"

struct BBCamState
{
    void (*unk0)();
    void (*unk4)();
    void (*unk8)();
};

struct BBCamMove
{
    /*0x00*/ Vec pos;
    /*0x0C*/ S16Vec rot;
    /*0x14*/ Vec target;
    /*0x20*/ S16Vec targetRot;
    /*0x28*/ Vec vel;
    /*0x34*/ S16Vec rotVel;
};

struct BBCamPath
{
    Vec pos;
    Vec lookAt;
};

u32 lbl_18_data_FC8[0x5C] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

void fn_18_EBF4();
void fn_18_EC80();
void fn_18_EC7C();
void fn_18_EC84();
void fn_18_EC8C();
void fn_18_EC88();
void fn_18_ED94();
void fn_18_EE20();
void fn_18_EE1C();
void fn_18_EE24();
void fn_18_EE90();
void fn_18_EE8C();
void fn_18_F024();
void fn_18_F02C();
void fn_18_F028();
void fn_18_F294();
void fn_18_F320();
void fn_18_F31C();
void fn_18_F420();
void fn_18_F428();
void fn_18_F424();
void fn_18_F58C();
void fn_18_F594();
void fn_18_F590();
void fn_18_F85C();
void fn_18_F994();
void fn_18_F990();
void fn_18_FC7C();
void fn_18_FC84();
void fn_18_FC80();
void fn_18_FC88();
void fn_18_FD14();
void fn_18_FD10();
void fn_18_1016C();
void fn_18_101F8();
void fn_18_101F4();
void fn_18_FAC8();
void fn_18_FB20();
void fn_18_FB1C();
void fn_18_101FC();
void fn_18_10204();
void fn_18_10200();
void fn_18_102C0();
void fn_18_10370();
void fn_18_1036C();
void fn_18_108B0();
void fn_18_108B8();
void fn_18_108B4();
void fn_18_10A14();
void fn_18_10A1C();
void fn_18_10A18();
void fn_18_FD18();
void fn_18_FD98();
void fn_18_FD94();
void fn_18_FF58();
void fn_18_FF90();
void fn_18_FF8C();
void fn_18_10BFC();
void fn_18_10C04();
void fn_18_10C00();

struct BBCamState lbl_18_data_1138[20] = {
    { fn_18_EBF4, fn_18_EC80, fn_18_EC7C },
    { fn_18_EC84, fn_18_EC8C, fn_18_EC88 },
    { fn_18_ED94, fn_18_EE20, fn_18_EE1C },
    { fn_18_EE24, fn_18_EE90, fn_18_EE8C },
    { fn_18_F024, fn_18_F02C, fn_18_F028 },
    { fn_18_F294, fn_18_F320, fn_18_F31C },
    { fn_18_F420, fn_18_F428, fn_18_F424 },
    { fn_18_F58C, fn_18_F594, fn_18_F590 },
    { fn_18_F85C, fn_18_F994, fn_18_F990 },
    { fn_18_FC7C, fn_18_FC84, fn_18_FC80 },
    { fn_18_FC88, fn_18_FD14, fn_18_FD10 },
    { fn_18_1016C, fn_18_101F8, fn_18_101F4 },
    { fn_18_FAC8, fn_18_FB20, fn_18_FB1C },
    { fn_18_101FC, fn_18_10204, fn_18_10200 },
    { fn_18_102C0, fn_18_10370, fn_18_1036C },
    { fn_18_108B0, fn_18_108B8, fn_18_108B4 },
    { fn_18_10A14, fn_18_10A1C, fn_18_10A18 },
    { fn_18_FD18, fn_18_FD98, fn_18_FD94 },
    { fn_18_FF58, fn_18_FF90, fn_18_FF8C },
    { fn_18_10BFC, fn_18_10C04, fn_18_10C00 },
};

struct BBCamPath lbl_18_data_1228[9] = {
    { { 18.1019993f, 1.0f, -25.1879997f }, { 23.0f, 1.5f, -25.7999992f } },
    { { 15.7259998f, 2.0f, -31.0580006f }, { 21.7689991f, 2.5f, -34.3829994f } },
    { { 11.8789997f, 2.0f, -35.5040016f }, { 16.184f, 1.5f, -41.0f } },
    { { 6.15700006f, 2.0f, -38.0019989f }, { 8.72999954f, 2.5f, -45.0999985f } },
    { { 0.0f, 2.5f, -39.5960007f }, { 0.0f, 2.0f, -45.0f } },
    { { -6.15700006f, 2.0f, -38.0019989f }, { -8.72999954f, 2.5f, -45.0999985f } },
    { { -11.8789997f, 2.0f, -35.5040016f }, { -16.184f, 1.5f, -41.0f } },
    { { -15.7259998f, 2.0f, -31.0580006f }, { -21.7689991f, 2.5f, -34.3829994f } },
    { { -18.1019993f, 1.0f, -25.1879997f }, { -23.0f, 1.5f, -25.7999992f } },
};

void fn_18_103DC();
void fn_18_10558();
void fn_18_10594();
void fn_18_105D0();
void fn_18_1060C();
void fn_18_10648();
void fn_18_10684();
void fn_18_106C0();

struct BBCamState lbl_18_data_1300[7] = {
    { NULL, fn_18_103DC, NULL },
    { NULL, fn_18_10558, NULL },
    { NULL, fn_18_10594, NULL },
    { NULL, fn_18_105D0, NULL },
    { NULL, fn_18_1060C, NULL },
    { NULL, fn_18_10648, NULL },
    { fn_18_10684, fn_18_106C0, NULL },
};

struct BBPlay lbl_18_bss_D68[6];
u8 lbl_18_bss_1080 __attribute__((aligned(4)));
u8 lbl_18_bss_1088 __attribute__((aligned(8)));
u8 lbl_18_bss_108C __attribute__((aligned(4)));

/* stripped at link; pins the .bss order, which mwcc assigns by first reference */
void baseball_17_bss_layout(void)
{
    *(u8 *) lbl_18_bss_D68 = 0;
    lbl_18_bss_1080 = 0;
    lbl_18_bss_1088 = 0;
    lbl_18_bss_108C = 0;
}

void fn_18_E8C8(struct Ball *arg0)
{
    s16 spA;
    s16 sp8;
    struct Camera *cam = cameras;
    struct BBLift *lift = &lbl_18_bss_970.unk1F8;

    mtxa_from_translate(&cam->eye);
    mtxa_rotate_z(cam->rotZ);
    mtxa_rotate_y(cam->rotY);
    mtxa_rotate_x(cam->rotX);
    mtxa_tf_point(&lift->unkC, &arg0->pos);
    arg0->prevPos = arg0->pos;
    ray_to_euler_xy(&cam->eye, &arg0->pos, &spA, &sp8);
    mtxa_from_identity();
    mtxa_rotate_y(sp8);
    mtxa_rotate_x(spA);
    mtxa_rotate_z(0);
    mtxa_rotate_y(-0x8000);
    mtxa_to_quat(&arg0->ape->rot);
}

void fn_18_E9A0(s32 arg0, s8 arg1)
{
    struct Camera *cam = cameras;
    struct Ball *ball = balls;

    camera_setup_singleplayer_viewport();
    camera_set_state(0x56);
    fn_18_1143C(arg0);
    enable_camera(arg0);
    cam->unk1F = arg1;
    lbl_18_data_1138[cam->unk1F].unk0(cam, ball);
    event_camera_tick();
    camera_apply_viewport(arg0);
}

void fn_18_EA54(struct Camera *arg0, struct Ball *arg1)
{
    if ((fn_18_334() != 1) && (lbl_18_data_1138[arg0->unk1F].unk4 != NULL)) {
        lbl_18_data_1138[arg0->unk1F].unk4(arg0, arg1);
    }
}

void fn_18_EAE4(s32 arg0, s8 arg1)
{
    struct Camera *cam = &cameras[arg0];
    struct Ball *ball = balls;

    if (lbl_18_data_1138[cam->unk1F].unk8 != NULL) {
        lbl_18_data_1138[cam->unk1F].unk8(cam, ball);
    }
    cam->unk1F = arg1;
    if (lbl_18_data_1138[cam->unk1F].unk0 != NULL) {
        lbl_18_data_1138[cam->unk1F].unk0(cam, ball);
    }
}

u8 fn_18_EBDC(void)
{
    struct Camera *cam = cameras;

    return cam->unk1F;
}

void fn_18_EBF4(struct Camera *arg0)
{
    camera_clear(arg0);
    arg0->eye.x = 0.0f;
    arg0->eye.y = 2.6f;
    arg0->eye.z = 5.0f;
    arg0->unkB4 = arg0->eye;
    arg0->rotX = -0x6F8;
    arg0->rotY = 0;
    arg0->rotZ = 0;
}

void fn_18_EC7C(void)
{

}

void fn_18_EC80(void)
{

}

void fn_18_EC84(void)
{

}

void fn_18_EC88(void)
{

}

void fn_18_EC8C(struct Camera *arg0, struct Ball *arg1)
{
    struct BBCamMove sp8;
    struct Ball *ball = arg1 + 1;

    fn_18_11380(&sp8, arg0, ball);
    fn_18_11190(&sp8, 10.0f, 4.5f, 0x2000);
    if (fabs(arg0->eye.x + sp8.vel.x) < 26.5) {
        arg0->eye.x += sp8.vel.x;
    }
    if ((arg0->eye.y + sp8.vel.y) > 1.0) {
        arg0->eye.y += sp8.vel.y;
    }
    if ((arg0->eye.z + sp8.vel.z) < 5.0) {
        arg0->eye.z += sp8.vel.z;
    }
    arg0->rotX += sp8.rotVel.x;
    arg0->rotY += sp8.rotVel.y;
    arg0->rotZ += sp8.rotVel.z;
}

void fn_18_ED94(struct Camera *arg0)
{
    camera_clear(arg0);
    arg0->eye.x = -15.0f;
    arg0->eye.y = 11.0f;
    arg0->eye.z = 8.65f;
    arg0->unkB4 = arg0->eye;
    arg0->rotX = -0xD27;
    arg0->rotY = -0x160B;
    arg0->rotZ = 0;
}

void fn_18_EE1C(void)
{

}

void fn_18_EE20(void)
{

}

void fn_18_EE24(struct Camera *arg0, struct Ball *arg1)
{
    Vec sp8;
    struct Ball *ball = arg1 + 1;

    mtxa_from_translate(&ball->pos);
    sp8.x = 0.0f;
    sp8.y = 1.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &arg0->eye);
}

void fn_18_EE8C(void)
{

}

void fn_18_EE90(struct Camera *arg0, struct Ball *arg1)
{
    struct BBCamMove sp8;
    struct Ball *ball = arg1 + 1;

    fn_18_11380(&sp8, arg0, ball);
    fn_18_11190(&sp8, 60.0f, 2.0f, 0x71C);
    arg0->eye.x += sp8.vel.x;
    arg0->eye.y += sp8.vel.y;
    arg0->eye.z += sp8.vel.z;
    arg0->rotX += sp8.rotVel.x;
    arg0->rotY += sp8.rotVel.y;
    arg0->rotZ += sp8.rotVel.z;
    if (fn_18_3EF4() == 1) {
        fn_18_EAE4(arg0->idx, 0);
    }
}

void fn_18_F024(void)
{

}

void fn_18_F028(void)
{

}

void fn_18_F02C(struct Camera *arg0, struct Ball *arg1)
{
    struct BBCamMove sp8;
    struct Ball *ball = arg1 + 1;

    fn_18_11380(&sp8, arg0, ball);
    fn_18_11190(&sp8, 60.0f, 7.5f, 0x2E38);
    arg0->eye.x += sp8.vel.x;
    arg0->eye.y += sp8.vel.y;
    arg0->eye.z += sp8.vel.z;
    arg0->rotX += sp8.rotVel.x;
    arg0->rotY += sp8.rotVel.y;
    arg0->rotZ += sp8.rotVel.z;
    if (fn_18_3EF4() == 1) {
        fn_18_EAE4(arg0->idx, 0);
    } else if (fn_18_3EF4() == 6) {
        fn_18_EAE4(arg0->idx, 5);
    }
}

void fn_18_F294(struct Camera *arg0)
{
    camera_clear(arg0);
    arg0->eye.x = 0.0f;
    arg0->eye.y = 7.3f;
    arg0->eye.z = -23.0f;
    arg0->unkB4 = arg0->eye;
    arg0->rotX = 0x48D;
    arg0->rotY = 0;
    arg0->rotZ = 0;
}

void fn_18_F31C(void)
{

}

void fn_18_F320(struct Camera *arg0)
{

    if (fn_18_3EF4() == 1) {
        fn_18_EAE4(arg0->idx, 0);
    }
}

void fn_18_F420(void)
{

}

void fn_18_F424(void)
{

}

void fn_18_F428(struct Camera *arg0, struct Ball *arg1)
{
    struct BBCamMove sp8;
    struct Ball *ball = arg1 + 1;

    fn_18_11380(&sp8, arg0, ball);
    fn_18_11190(&sp8, 10.0f, 3.0f, 0);
    arg0->rotX += sp8.rotVel.x;
    arg0->rotY += sp8.rotVel.y;
    arg0->rotZ += sp8.rotVel.z;
    if (fn_18_3EF4() == 1) {
        fn_18_EAE4(arg0->idx, 0);
    }
}

void fn_18_F58C(void)
{

}

void fn_18_F590(void)
{

}

void fn_18_F594(struct Camera *arg0)
{
    struct BBCamMove sp28;
    Vec sp1C;
    Vec sp10;
    S16Vec sp8;
    struct BBCamPath *tr5;
    u8 tr3;

    tr3 = fn_18_18B68();
    tr5 = &lbl_18_data_1228[tr3];
    sp1C = tr5->pos;
    sp10 = lbl_18_data_1228[tr3].lookAt;
    ray_to_euler_xy(&sp10, &arg0->eye, &sp8.x, &sp8.y);
    sp8.y -= 0x8000;
    fn_18_10FE8(&sp28, arg0, &sp1C, &sp8);
    fn_18_11094(&sp28, 0x1E);
    arg0->eye.x += sp28.vel.x;
    arg0->eye.y += sp28.vel.y;
    arg0->eye.z += sp28.vel.z;
    arg0->rotX += sp28.rotVel.x;
    arg0->rotY += sp28.rotVel.y;
    arg0->rotZ = 0;
    if (fn_18_3EF4() == 1) {
        fn_18_EAE4(arg0->idx, 0);
    } else if (fn_18_3EF4() == 6) {
        fn_18_EAE4(arg0->idx, 5);
    }
}

void fn_18_F85C(struct Camera *arg0)
{
    Vec sp14;
    Vec sp8;
    register float tf31;
    float tf30;
    register float tf29;

    tf29 = (&lbl_18_bss_970.unk1B8)[arg0->idx * 4];
    fn_18_15664((struct BBTrack *) lbl_18_data_7F334, (float *) &sp14, tf29);
    fn_18_15664((struct BBTrack *) lbl_18_data_7F440, (float *) &sp8, tf29);
    camera_clear(arg0);
    arg0->eye.x = sp14.x;
    arg0->eye.y = sp14.y;
    arg0->eye.z = sp14.z;
    arg0->unkB4 = arg0->eye;
    tf31 = sp8.x - sp14.x;
    tf30 = sp8.y - sp14.y;
    tf29 = sp8.z - sp14.z;
    arg0->rotY = math_atan2(tf31, tf29) - 0x8000;
    tf31 = tf31 * tf31;
    tf31 = __fmadds(tf29, tf29, tf31);
    arg0->rotX = math_atan2(tf30, math_sqrt(tf31));
    arg0->rotZ = 0;
}

void fn_18_F990(void)
{

}

void fn_18_F994(struct Camera *arg0)
{
    Vec sp14;
    Vec sp8;
    register float tf31;
    float tf30;
    register float tf29;

    tf29 = (&lbl_18_bss_970.unk1B8)[arg0->idx * 4];
    fn_18_15664((struct BBTrack *) lbl_18_data_7F334, (float *) &sp14, tf29);
    fn_18_15664((struct BBTrack *) lbl_18_data_7F440, (float *) &sp8, tf29);
    camera_clear(arg0);
    arg0->eye.x = sp14.x;
    arg0->eye.y = sp14.y;
    arg0->eye.z = sp14.z;
    arg0->unkB4 = arg0->eye;
    tf31 = sp8.x - sp14.x;
    tf30 = sp8.y - sp14.y;
    tf29 = sp8.z - sp14.z;
    arg0->rotY = math_atan2(tf31, tf29) - 0x8000;
    tf31 = tf31 * tf31;
    tf31 = __fmadds(tf29, tf29, tf31);
    arg0->rotX = math_atan2(tf30, math_sqrt(tf31));
    arg0->rotZ = 0;
}

void fn_18_FAC8(struct Camera *arg0)
{
    arg0->eye.x = 3.2f;
    arg0->eye.y = 2.1f;
    arg0->eye.z = -4.7f;
    arg0->unkB4 = arg0->eye;
    arg0->rotX = -0x999;
    arg0->rotY = 0x58E3;
    arg0->rotZ = 0;
}

void fn_18_FB1C(void)
{

}

void fn_18_FB20(struct Camera *arg0)
{
    struct BBCamMove sp20;
    struct BBBFC *p = &lbl_18_bss_BFC;
    Vec sp14 = { 0.0f, 0.0f, 0.0f };
    S16Vec spC;
    s16 spA;
    s16 sp8;
    struct BBPlay *r4;

    if (p->unk0 & 2) {
        r4 = lbl_18_bss_D68;
    } else if ((u8) p->unk9 >= p->unk7) {
        r4 = &lbl_18_bss_D68[p->unk7 - 1];
    } else {
        r4 = &lbl_18_bss_D68[p->unk9];
    }
    arg0->eye.x = 3.2f;
    arg0->eye.y = 2.1f;
    arg0->eye.z = -4.7f;
    ray_to_euler_xy(&arg0->eye, &r4->pos, &spA, &sp8);
    spC.x = spA;
    spC.y = sp8;
    spC.z = 0;
    fn_18_10FE8(&sp20, arg0, &arg0->eye, &spC);
    fn_18_11094(&sp20, 0x1E);
    sp20.rotVel.y = fn_18_1F40((s32) sp8, (s32) arg0->rotY, 0x80, 0x1E);
    arg0->rotX += sp20.rotVel.x;
    arg0->rotY += sp20.rotVel.y;
    arg0->rotZ = 0;
}

void fn_18_FC7C(void)
{

}

void fn_18_FC80(void)
{

}

void fn_18_FC84(void)
{

}

void fn_18_FC88(struct Camera *arg0)
{
    camera_clear(arg0);
    arg0->eye.x = 0.0f;
    arg0->eye.y = 8.5f;
    arg0->eye.z = -1.5f;
    arg0->unkB4 = arg0->eye;
    arg0->rotX = -0x1C71;
    arg0->rotY = 0;
    arg0->rotZ = 0;
}

void fn_18_FD10(void)
{

}

void fn_18_FD14(void)
{

}

void fn_18_FD18(struct Camera *arg0)
{
    if (fn_18_7CD8() == 3) {
        arg0->lookAt.x = 0.0f;
        arg0->lookAt.y = 0.7f;
        arg0->lookAt.z = -15.5f;
        return;
    }
    arg0->lookAt.x = 0.0f;
    arg0->lookAt.y = 0.7f;
    arg0->lookAt.z = -10.5f;
}

void fn_18_FD94(void)
{

}

void fn_18_FD98(struct Camera *arg0)
{
    struct BBCamMove sp34;
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    S16Vec sp8;
    register float tf31;
    float tf30;
    register float tf29;

    if (fn_18_7CD8() == 3) {
        {
            Vec sp1C = { 0.0f, 0.5f, -10.5f };
            Vec *p = &sp1C;
            sp28 = *p;
        }
    } else {
        {
            Vec sp10 = { 0.0f, 0.5f, -7.0f };
            Vec *p = &sp10;
            sp28 = *p;
        }
    }
    tf31 = arg0->lookAt.x - arg0->eye.x;
    tf30 = arg0->lookAt.y - arg0->eye.y;
    tf29 = arg0->lookAt.z - arg0->eye.z;
    sp8.y = math_atan2(tf31, tf29) - 0x8000;
    tf31 = tf31 * tf31;
    tf31 = __fmadds(tf29, tf29, tf31);
    sp8.x = math_atan2(tf30, math_sqrt(tf31));
    sp8.z = 0;
    fn_18_10FE8(&sp34, arg0, &sp28, &sp8);
    fn_18_11094(&sp34, 0x1E);
    arg0->eye.x += sp34.vel.x;
    arg0->eye.y += sp34.vel.y;
    arg0->eye.z += sp34.vel.z;
    arg0->rotX += sp34.rotVel.x;
    arg0->rotY += sp34.rotVel.y;
    arg0->rotZ = 0;
}

void fn_18_FF58(struct Camera *arg0)
{
    arg0->lookAt.x = 0.0f;
    arg0->lookAt.y = 0.7f;
    arg0->lookAt.z = -10.5f;
    lbl_18_bss_1088 = 0;
}

void fn_18_FF8C(void)
{

}

void fn_18_FF90(struct Camera *arg0)
{
    struct BBCamMove sp1C;
    Vec sp10 = { 0.0f, 0.5f, -7.0f };
    S16Vec sp8;
    register float tf31;
    float tf30;
    register float tf29;

    if (fn_18_7CD8() == 3) {
        arg0->rotY += 0x10;
        return;
    }
    switch ((s32) lbl_18_bss_1088) {                /* irregular */
    case 0:
        arg0->lookAt.x += 0.01;
        if (arg0->lookAt.x > 3.0) {
            lbl_18_bss_1088 = 1;
        }
        break;
    case 1:
        arg0->lookAt.x -= 0.01;
        if (arg0->lookAt.x < -3.0) {
            lbl_18_bss_1088 = 0;
        }
        break;
    }
    tf31 = arg0->lookAt.x - arg0->eye.x;
    tf30 = arg0->lookAt.y - arg0->eye.y;
    tf29 = arg0->lookAt.z - arg0->eye.z;
    sp8.y = math_atan2(tf31, tf29) - 0x8000;
    tf31 = tf31 * tf31;
    tf31 = __fmadds(tf29, tf29, tf31);
    sp8.x = math_atan2(tf30, math_sqrt(tf31));
    sp8.z = 0;
    fn_18_10FE8(&sp1C, arg0, &sp10, &sp8);
    fn_18_11094(&sp1C, 0x1E);
    arg0->rotX += sp1C.rotVel.x;
    arg0->rotY += sp1C.rotVel.y;
    arg0->rotZ = 0;
}

void fn_18_1016C(struct Camera *arg0)
{
    camera_clear(arg0);
    arg0->eye.x = 0.0f;
    arg0->eye.y = 0.5f;
    arg0->eye.z = 1.0f;
    arg0->unkB4 = arg0->eye;
    arg0->rotX = 0;
    arg0->rotY = 0;
    arg0->rotZ = 0;
}

void fn_18_101F4(void)
{

}

void fn_18_101F8(void)
{

}

void fn_18_101FC(void)
{

}

void fn_18_10200(void)
{

}

void fn_18_10204(struct Camera *arg0, struct Ball *arg1)
{
    struct BBCamMove sp8;
    struct Ball *ball = arg1 + 1;

    fn_18_11380(&sp8, arg0, ball);
    fn_18_11190(&sp8, 30.0f, 3.0f, 0x2000);
    arg0->eye.x += sp8.vel.x;
    arg0->eye.y += sp8.vel.y;
    arg0->eye.z += sp8.vel.z;
    arg0->rotX += sp8.rotVel.x;
    arg0->rotY += sp8.rotVel.y;
    arg0->rotZ += sp8.rotVel.z;
}

void fn_18_102C0(struct Camera *arg0, struct Ball *arg1)
{
    u8 *mode = &lbl_18_bss_108C;

    memset(mode, 0, 2);
    *mode = 6;
    if (lbl_18_data_1300[*mode].unk0 != NULL) {
        lbl_18_data_1300[*mode].unk0(arg0, mode, arg1);
    }
}

void fn_18_1036C(void)
{

}

void fn_18_10370(struct Camera *arg0, struct Ball *arg1)
{
    u8 *mode = &lbl_18_bss_108C;

    if (lbl_18_data_1300[*mode].unk4 != NULL) {
        lbl_18_data_1300[*mode].unk4(arg0, mode, arg1);
    }
}

void fn_18_103DC(struct Camera *arg0)
{
    struct BBCamMove sp2C;
    struct BBPlay *play = lbl_18_bss_D68;
    Vec sp20 = { 0.0f, 0.0f, 0.0f };
    Vec sp14;
    s16 sp10;
    s16 spE;
    s16 spC;
    s16 spA;
    s16 sp8;

    if ((uptr) play->unk64 != 0) {
        mtxa_from_quat(&play->unk64->rot);
        g_math_unk14(&sp10, &spE, &spC);
        mtxa_from_translate(&play->pos);
        mtxa_rotate_z(sp10);
        mtxa_rotate_y(spE);
        mtxa_rotate_x(spC);
        sp14.x = 0.0f;
        sp14.y = 1.0f;
        sp14.z = -2.5f;
        mtxa_tf_point_xyz(sp14.x, sp14.y, sp14.z, &sp14);
        ray_to_euler_xy(&sp14, &play->pos, &spA, &sp8);
        sp2C.pos = arg0->eye;
        sp2C.target = sp14;
        sp2C.vel = sp20;
        sp2C.rot.x = arg0->rotX;
        sp2C.rot.y = arg0->rotY;
        sp2C.rot.z = arg0->rotZ;
        arg0->eye = sp14;
        arg0->rotX = spA;
        arg0->rotY = sp8;
        arg0->rotZ = 0;
    }
}

void fn_18_10558(struct Camera *arg0)
{
    arg0->eye.x = 13.4f;
    arg0->eye.y = 2.7f;
    arg0->eye.z = -9.7f;
    arg0->rotX = -0x1000;
    arg0->rotY = 0x3D27;
    arg0->rotZ = 0;
}

void fn_18_10594(struct Camera *arg0)
{
    arg0->eye.x = -3.5f;
    arg0->eye.y = 3.35f;
    arg0->eye.z = -20.5f;
    arg0->rotX = -0x1369;
    arg0->rotY = -0x4E38;
    arg0->rotZ = 0;
}

void fn_18_105D0(struct Camera *arg0)
{
    arg0->eye.x = -9.6f;
    arg0->eye.y = 1.37f;
    arg0->eye.z = -7.35f;
    arg0->rotX = -0x999;
    arg0->rotY = -0x7D2;
    arg0->rotZ = 0;
}

void fn_18_1060C(struct Camera *arg0)
{
    arg0->eye.x = 3.5f;
    arg0->eye.y = 2.6f;
    arg0->eye.z = -1.7f;
    arg0->rotX = -0xD82;
    arg0->rotY = 0x471C;
    arg0->rotZ = 0;
}

void fn_18_10648(struct Camera *arg0)
{
    arg0->eye.x = 14.5f;
    arg0->eye.y = 7.4f;
    arg0->eye.z = -12.0f;
    arg0->rotX = -0x16C1;
    arg0->rotY = 0x45B0;
    arg0->rotZ = 0;
}

void fn_18_10684(struct Camera *arg0)
{
    arg0->eye.x = 6.8f;
    arg0->eye.y = 3.7f;
    arg0->eye.z = -15.0f;
    arg0->rotX = -0xAAA;
    arg0->rotY = 0x7B05;
    arg0->rotZ = 0;
}

void fn_18_106C0(struct Camera *arg0)
{
    struct BBCamMove sp2C;
    struct BBPlay *play = lbl_18_bss_D68;
    Vec sp20 = { 0.0f, 0.0f, 0.0f };
    Vec sp14;
    s16 sp10;
    s16 spE;
    s16 spC;
    s16 spA;
    s16 sp8;

    if ((uptr) play->unk64 != 0) {
        mtxa_from_quat(&play->unk64->rot);
        g_math_unk14(&sp10, &spE, &spC);
        mtxa_from_translate(&play->pos);
        mtxa_rotate_z(sp10);
        mtxa_rotate_y(spE);
        mtxa_rotate_x(spC);
        sp14.x = 0.0f;
        sp14.y = 0.1f;
        sp14.z = -3.0f;
        mtxa_tf_point_xyz(sp14.x, sp14.y, sp14.z, &sp14);
        ray_to_euler_xy(&sp14, &play->pos, &spA, &sp8);
        sp2C.pos = arg0->eye;
        sp2C.target = sp14;
        sp2C.vel = sp20;
        sp2C.rot.x = arg0->rotX;
        sp2C.rot.y = arg0->rotY;
        sp2C.rot.z = arg0->rotZ;
        sp2C.targetRot.x = spA;
        sp2C.targetRot.y = sp8;
        sp2C.targetRot.z = 0;
        fn_18_11094(&sp2C, 0x1E);
        arg0->eye.x += sp2C.vel.x;
        arg0->eye.y += sp2C.vel.y;
        arg0->eye.z += sp2C.vel.z;
        arg0->rotX += sp2C.rotVel.x;
        if (sp2C.rotVel.y < 0) {
            arg0->rotY += sp2C.rotVel.y;
        } else {
            arg0->rotY -= sp2C.rotVel.y;
        }
        arg0->rotZ += sp2C.rotVel.z;
    }
}

void fn_18_108B0(void)
{

}

void fn_18_108B4(void)
{

}

void fn_18_108B8(struct Camera *arg0)
{
    struct BBCamMove sp28;
    Vec sp1C = { 0.0f, 0.0f, 0.0f };
    struct BBPlay *play = lbl_18_bss_D68;
    Vec sp10;
    s16 spC;
    s16 spA;
    s16 sp8;

    mtxa_from_quat(&play->unk64->rot);
    g_math_unk14(&spC, &spA, &sp8);
    mtxa_from_translate(&play->pos);
    mtxa_rotate_z(spC);
    mtxa_rotate_y(spA);
    mtxa_rotate_x(sp8);
    sp10.x = 0.0f;
    sp10.y = 0.0f;
    sp10.z = -3.0f;
    mtxa_tf_point_xyz(sp10.x, sp10.y, sp10.z, &sp10);
    sp28.pos = arg0->eye;
    sp28.target = sp10;
    sp28.vel = sp1C;
    sp28.rot.x = arg0->rotX;
    sp28.rot.y = arg0->rotY;
    sp28.rot.z = arg0->rotZ;
    arg0->eye = sp10;
    arg0->rotX = sp8;
    arg0->rotY = spA;
    arg0->rotZ = spC;
}

void fn_18_10A14(void)
{

}

void fn_18_10A18(void)
{

}

void fn_18_10A1C(struct Camera *arg0)
{
    struct BBCamMove sp2C;
    struct BBPlay *play = lbl_18_bss_D68;
    Vec sp20 = { 0.0f, 0.0f, 0.0f };
    Vec sp14;
    s16 sp10;
    s16 spE;
    s16 spC;
    s16 spA;
    s16 sp8;

    if ((uptr) play->unk64 != 0) {
        mtxa_from_quat(&play->unk64->rot);
        g_math_unk14(&sp10, &spE, &spC);
        mtxa_from_translate(&play->pos);
        mtxa_rotate_z(sp10);
        mtxa_rotate_y(spE);
        mtxa_rotate_x(spC);
        sp14.x = 0.0f;
        sp14.y = 0.1f;
        sp14.z = -3.0f;
        mtxa_tf_point_xyz(sp14.x, sp14.y, sp14.z, &sp14);
        ray_to_euler_xy(&arg0->eye, &play->pos, &spA, &sp8);
        sp2C.pos = arg0->eye;
        sp2C.target = sp14;
        sp2C.vel = sp20;
        sp2C.rot.x = arg0->rotX;
        sp2C.rot.y = arg0->rotY;
        sp2C.rot.z = arg0->rotZ;
        sp2C.targetRot.x = spA;
        sp2C.targetRot.y = sp8;
        sp2C.targetRot.z = 0;
        fn_18_11094(&sp2C, 0x14);
        sp2C.rotVel.y = fn_18_1F40((s32) sp8, (s32) arg0->rotY, 0x80, 0x14);
        arg0->eye.x += sp2C.vel.x;
        arg0->eye.y += sp2C.vel.y;
        arg0->eye.z += sp2C.vel.z;
        arg0->rotX += sp2C.rotVel.x;
        arg0->rotY += sp2C.rotVel.y;
        arg0->rotZ = 0;
    }
}

void fn_18_10BFC(void)
{

}

void fn_18_10C00(void)
{

}

void fn_18_10C04(struct Camera *arg0)
{
    float spC[2];
    s8 sp9;
    s8 sp8;
    float tf1;
    float tf2;
    float tf31;
    u16 r29;

    fn_18_FB8(0, 0);
    fn_18_1010(0, &sp9, &sp8);
    if (__abs(pad_status_groups->unk0[0].stickX) > 0xF) {
        sp9 = pad_status_groups->unk0[0].stickX;
    }
    if (__abs(pad_status_groups->unk0[0].stickY) > 0xF) {
        sp8 = pad_status_groups->unk0[0].stickY;
    }
    r29 = 0;
    if ((s32) (pad_status_groups->unk0[0].button & 0x100) != 0) {
        r29 |= 0x100;
    }
    if ((s32) (pad_status_groups->unk0[0].button & 0x200) != 0) {
        r29 |= 0x200;
    }
    tf31 = mathutil_sum_of_sq_2(sp9 / 60.0f, sp8 / 60.0f);
    math_sin_cos_v(math_atan2((float) sp9, (float) sp8), spC);
    tf1 = tf31 * spC[0];
    tf2 = tf31 * spC[1];
    if ((s32) (r29 & 0x100) != 0) {
        if (__abs(sp8) > 0xF) {
            arg0->eye.y += tf2;
        }
    } else if ((s32) (r29 & 0x200) != 0) {
        arg0->rotX += (s16) (182.04445f * tf2);
        arg0->rotY -= (s16) (182.04445f * tf1);
    } else {
        if (__abs(sp9) > 0xF) {
            arg0->eye.x += tf1;
        }
        if (__abs(sp8) > 0xF) {
            arg0->eye.z -= tf2;
        }
    }
    debug_textdraw_set_pos(1, 2);
    debug_textdraw_vsprintf_buf_1("pos x >> %f", arg0->eye.x);
    debug_textdraw_set_pos(1, 3);
    debug_textdraw_vsprintf_buf_1("pos y >> %f", arg0->eye.y);
    debug_textdraw_set_pos(1, 4);
    debug_textdraw_vsprintf_buf_1("pos z >> %f", arg0->eye.z);
    debug_textdraw_set_pos(1, 5);
    debug_textdraw_vsprintf_buf_1("rot x >> %f", 0.005493164f * (float) arg0->rotX);
    debug_textdraw_set_pos(1, 6);
    debug_textdraw_vsprintf_buf_1("rot y >> %f", 0.005493164f * (float) arg0->rotY);
    debug_textdraw_set_pos(1, 7);
    debug_textdraw_vsprintf_buf_1("rot z >> %f", 0.005493164f * (float) arg0->rotZ);
}

void fn_18_10FE8(struct BBCamMove *arg0, struct Camera *arg1, Vec *arg2, S16Vec *arg3)
{
    arg0->pos = arg1->eye;
    arg0->rot.x = arg1->rotX;
    arg0->rot.y = arg1->rotY;
    arg0->rot.z = arg1->rotZ;
    arg0->target = *arg2;
    arg0->targetRot.x = arg3->x;
    arg0->targetRot.y = arg3->y;
    arg0->targetRot.z = arg3->z;
    arg0->vel.x = 0.0f;
    arg0->vel.y = 0.0f;
    arg0->vel.z = 0.0f;
    arg0->rotVel.x = 0;
    arg0->rotVel.y = 0;
    arg0->rotVel.z = 0;
}

void fn_18_11094(struct BBCamMove *arg0, s32 arg1)
{
    arg0->vel.x = ((arg0->target.x - arg0->pos.x) / (float) arg1);
    arg0->vel.y = ((arg0->target.y - arg0->pos.y) / (float) arg1);
    arg0->vel.z = ((arg0->target.z - arg0->pos.z) / (float) arg1);
    arg0->rotVel.x = (s16) ((s32) (arg0->targetRot.x - arg0->rot.x) / arg1);
    arg0->rotVel.y = (s16) ((s32) (arg0->targetRot.y - arg0->rot.y) / arg1);
    arg0->rotVel.z = (s16) ((s32) (arg0->targetRot.z - arg0->rot.z) / arg1);
}

void fn_18_11190(struct BBCamMove *arg0, float farg0, float farg1, s16 arg1)
{
    float sp10[2];
    S16Vec sp8;
    float tf0;
    float tf1;
    float tf2;
    float tf3;
    float tf29;

    tf1 = arg0->target.x - arg0->pos.x;
    tf3 = arg0->target.y - arg0->pos.y;
    tf0 = arg0->target.z - arg0->pos.z;
    tf1 = tf1 * tf1;
    tf1 = __fmadds(tf3, tf3, tf1);
    tf1 = __fmadds(tf0, tf0, tf1);
    if (fabs(farg1 - math_sqrt(tf1)) > 0.05f) {
        math_sin_cos_v(arg1, sp10);
        tf29 = farg1 * sp10[0];
        ray_to_euler_xy(&arg0->target, &arg0->pos, &sp8.x, &sp8.y);
        sp8.y -= 0x8000;
        math_sin_cos_v(sp8.y, sp10);
        tf2 = farg1 * sp10[0];
        tf3 = farg1 * sp10[1];
        tf2 = (arg0->target.x + tf2) - arg0->pos.x;
        tf29 = (arg0->target.y + tf29) - arg0->pos.y;
        tf3 = (arg0->target.z + tf3) - arg0->pos.z;
        tf2 /= farg0;
        tf29 /= farg0;
        tf3 /= farg0;
        arg0->vel.x = tf2;
        arg0->vel.y = tf29;
        arg0->vel.z = tf3;
    } else {
        arg0->vel.x = 0.0f;
        arg0->vel.y = 0.0f;
        arg0->vel.z = 0.0f;
    }
    ray_to_euler_xy(&arg0->target, &arg0->pos, &sp8.x, &sp8.y);
    sp8.y -= 0x8000;
    arg0->rotVel.y = fn_18_1F40((s32) sp8.y, (s32) arg0->rot.y, 0x80, (s16) farg0);
    arg0->rotVel.x = fn_18_1F40((s32) (s16) -sp8.x, (s32) arg0->rot.x, 0x80, (s16) farg0);
}

void fn_18_11380(struct BBCamMove *arg0, struct Camera *arg1, struct Ball *arg2)
{
    Vec sp8 = { 0.0f, 0.0f, 0.0f };

    arg0->pos = arg1->eye;
    arg0->target = arg2->pos;
    arg0->vel = sp8;
    arg0->rot.x = arg1->rotX;
    arg0->rot.y = arg1->rotY;
    arg0->rot.z = arg1->rotZ;
    arg0->targetRot.x = arg2->rotX;
    arg0->targetRot.y = arg2->rotY;
    arg0->targetRot.z = arg2->rotZ;
    arg0->rotVel.x = 0;
    arg0->rotVel.y = 0;
    arg0->rotVel.z = 0;
}

float lbl_18_data_139C[17] = { 0.0f, 0.0f, 1.0f, 1.0f };

void fn_18_1143C(s32 arg0)
{
    float *vp = lbl_18_data_139C;

    fn_18_1147C(0, vp[0], vp[1], vp[2], vp[3]);
}

void fn_18_1147C(s32 arg0, float farg0, float farg1, float farg2, float farg3)
{
    struct Camera *tr30;

    tr30 = &cameras[arg0];
    tr30->sub28.viewportX = farg0;
    tr30->sub28.viewportY = farg1;
    tr30->sub28.viewportW = farg2;
    tr30->sub28.viewportH = farg3;
    if ((farg2 > 0.0f) && (farg3 > 0.0f)) {
        tr30->sub28.view.aspect = g_camera_aspect_ratio * (farg2 / farg3);
        if (1.0f == farg3) {
            tr30->sub28.view.nextFov = 0x2AAA;
        } else {
            tr30->sub28.view.nextFov = math_atan2((0.5f + (0.5f * farg3)) * math_tan(0x1555), 1.0f) * 2;
            tr30->sub28.view.fov = tr30->sub28.view.nextFov;
        }
    }
    camera_apply_viewport(arg0);
    camera_apply_viewport_2(arg0);
}

