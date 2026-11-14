#include "baseball.h"
#include "mathutil.h"

struct BBBallFuncs lbl_18_data_A70[5] = {
    {(void *) fn_18_E17C, (void *) fn_18_E3E8, (void *) fn_18_E3E4},
    {(void *) fn_18_E550, (void *) fn_18_E798, (void *) fn_18_E794},
    {(void *) fn_18_8DC0, (void *) fn_18_8E34, (void *) fn_18_8E30},
    {(void *) fn_18_A368, (void *) fn_18_A3DC, (void *) fn_18_A3D8},
    {(void *) fn_18_C3D4, (void *) fn_18_C480, (void *) fn_18_C47C},
};

u8 fn_18_81B0(void)
{
    u8 tr31;
    s8 tr30;
    u8 r29;

    tr31 = fn_18_19E0();
    tr30 = fn_18_19964();
    r29 = 0;
    if (fn_18_1A7C() == 1) {
        switch ((s32) tr31) {                   /* switch 1; irregular */
        case 0:                                     /* switch 1 */
            if (tr30 == 0) {
                r29 = 0;
            } else if (tr30 == 1) {
                r29 = 2;
            } else {
                r29 = 3;
            }
            break;
        case 1:                                     /* switch 1 */
            r29 = 3;
            break;
        case 2:                                     /* switch 1 */
            if (tr30 == 0) {
                r29 = 2;
            } else if (tr30 == 1) {
                r29 = 0;
            } else {
                r29 = 3;
            }
            break;
        }
    } else {
        switch ((s32) tr31) {                   /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            if (tr30 == 0) {
                r29 = 0;
            } else if (tr30 == 1) {
                r29 = 1;
            } else {
                r29 = 3;
            }
            break;
        case 1:                                     /* switch 2 */
            if (tr30 == 0) {
                r29 = 1;
            } else if (tr30 == 1) {
                r29 = 0;
            } else {
                r29 = 3;
            }
            break;
        case 2:                                     /* switch 2 */
            r29 = 3;
            break;
        }
    }
    return r29;
}

void fn_18_8324(void)
{
    fn_18_8D3C();
    fn_18_134EC();
    fn_18_15EE8();
    fn_18_15A38();
    fn_1_474D0();
    fn_18_19D98();
    fn_18_2239C();
    fn_18_11F48();
    fn_18_1214C();
    dest_all_events();
    fn_18_1B90();
    fn_18_2724(1);
}

void fn_18_8374(struct Ball *arg0)
{
    if ((fn_18_334() != 1) && (arg0->status != 0) && (lbl_18_data_A70[arg0->unk154].unk4 != NULL)) {
        lbl_18_data_A70[arg0->unk154].unk4(arg0);
    }
}

void g_load_baseball(u8 arg0, s32 arg1, s8 arg2, u8 arg3, u8 arg4, u8 arg5, void (*arg6)(struct Ape *), s32 arg7)
{
    struct Ball *tr31;

    tr31 = &balls[arg0];
    create_stage_player(arg0, arg1, arg3, arg4, arg5, arg6, arg7);
    g_reset_ball(tr31);
    tr31->state = (s8) arg1;
    tr31->unk154 = (s8) arg2;
    if (lbl_18_data_A70[tr31->unk154].unk0 != NULL) {
        lbl_18_data_A70[tr31->unk154].unk0(tr31);
    }
}

void fn_18_84D4(struct Ball *arg0)
{
    arg0->prevPos = arg0->pos;
    arg0->speed = mathutil_vec_len(&arg0->vel);
    arg0->flags &= ~0x20;
}

void fn_18_8558(struct Ball *arg0)
{
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8 = {0.0f, 1.0f, 0.0f};
    float tf31;
    float tf30;
    float tf29;
    float f2;
    float len;

    tf31 = arg0->pos.x - arg0->prevPos.x;
    tf30 = arg0->pos.y - arg0->prevPos.y;
    tf29 = arg0->pos.z - arg0->prevPos.z;
    sp20.x = -sp8.x * arg0->currRadius;
    sp20.y = -sp8.y * arg0->currRadius;
    sp20.z = -sp8.z * arg0->currRadius;
    mtxa_from_rotate_y(arg0->rotY);
    mtxa_rotate_x(arg0->rotX);
    mtxa_rotate_z(arg0->rotZ);
    mtxa_rigid_inv_tf_vec(&sp20, &sp20);
    sp2C.x = -tf31;
    sp2C.y = -tf30;
    sp2C.z = -tf29;
    mtxa_rigid_inv_tf_vec(&sp2C, &sp2C);
    mathutil_vec_cross_prod(&sp20, &sp2C, &sp14);
    f2 = 0.0f;
    if (arg0->currRadius > 1.1920929e-07f) {
        f2 = 10430.380520449504 / (arg0->currRadius * arg0->currRadius);
    }
    arg0->unk60.x = sp14.x * f2;
    arg0->unk60.y = sp14.y * f2;
    arg0->unk60.z = sp14.z * f2;
    len = vec_normalize_len(&sp14);
    quat_from_axis_angle(&arg0->unkA0, &sp14, (s16) (10430.3779296875 * ((2.0f * len) / (3.14159265 * (arg0->currRadius * arg0->currRadius)))));
}

void fn_18_8784(struct Ball *arg0)
{
    mtxa_from_quat(&arg0->unkA0);
    mtxa_mult_left(arg0->unk30);
    mtxa_normalize_basis();
    mathutil_mtxA_set_translate(&arg0->pos);
    mtxa_to_mtx(arg0->unk30);
    mtxa_to_euler_yxz(&arg0->rotY, &arg0->rotX, &arg0->rotZ);
}

void fn_18_87F8(struct BBPlay *arg0)
{
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8 = {0.0f, 1.0f, 0.0f};
    float tf31;
    float tf30;
    float tf29;
    float f2;
    float len;

    tf31 = arg0->pos.x - arg0->prevPos.x;
    tf30 = arg0->pos.y - arg0->prevPos.y;
    tf29 = arg0->pos.z - arg0->prevPos.z;
    sp20.x = -sp8.x * arg0->unk78;
    sp20.y = -sp8.y * arg0->unk78;
    sp20.z = -sp8.z * arg0->unk78;
    mtxa_from_rotate_y(arg0->unk7E);
    mtxa_rotate_x(arg0->unk7C);
    mtxa_rotate_z(arg0->unk80);
    mtxa_rigid_inv_tf_vec(&sp20, &sp20);
    sp2C.x = -tf31;
    sp2C.y = -tf30;
    sp2C.z = -tf29;
    mtxa_rigid_inv_tf_vec(&sp2C, &sp2C);
    mathutil_vec_cross_prod(&sp20, &sp2C, &sp14);
    f2 = 0.0f;
    if (arg0->unk78 > 1.1920929e-07f) {
        f2 = 10430.380520449504 / (arg0->unk78 * arg0->unk78);
    }
    len = vec_normalize_len(&sp14);
    quat_from_axis_angle(&arg0->unk68, &sp14, (s16) (10430.3779296875 * ((2.0f * len) / (3.14159265 * (arg0->unk78 * arg0->unk78)))));
}

void fn_18_89C0(struct BBPlay *arg0)
{
    mtxa_from_quat(&arg0->unk68);
    mtxa_mult_left(arg0->unk8);
    mtxa_normalize_basis();
    mathutil_mtxA_set_translate(&arg0->pos);
    mtxa_to_mtx(arg0->unk8);
    mtxa_to_euler_yxz(&arg0->unk7E, &arg0->unk7C, &arg0->unk80);
}

void fn_18_8A34(u8 arg0, s32 arg1, s32 arg2, s32 arg3, BBPlayFn arg4)
{
    struct BBPlay *tr31;

    if (arg0 < 6) {
        tr31 = &lbl_18_bss_D68[arg0];
        memset(tr31, 0, sizeof(struct BBPlay));
        tr31->unk0 |= 1;
        tr31->unk82 = arg0;
        tr31->unk4 = arg4;
        tr31->colorId = arg1;
        tr31->unk50 = arg3;
        tr31->unk78 = 0.5f;
        tr31->unk58 = 1.0f;
        {
            Quat sp8 = {0.0f, 0.0f, 0.0f, 1.0f};
            Quat *q = &sp8;

            tr31->unk68 = *q;
        }
        mtxa_from_identity();
        mtxa_to_mtx(tr31->unk8);
        if (arg2 != 0xFF) {
            tr31->unk64 = g_load_ape_gameplay_variant0(arg2, arg3);
            tr31->unk64->colorIndex = tr31->colorId;
            if (tr31->unk4 != NULL) {
                tr31->unk4(tr31);
                g_ape_anim_body(tr31->unk64);
            }
        }
    }
}

void fn_18_8B70(void)
{
    struct BBPlay *tr31;
    s32 r30;

    for (r30 = 0; r30 < 6; r30++) {
        if ((u8) r30 < 6) {
            tr31 = &lbl_18_bss_D68[(u8) r30];
            if (((tr31->unk0 & 1) != 0) && (tr31->unk4 != NULL)) {
                tr31->unk4(tr31);
                g_ape_anim_body(tr31->unk64);
            }
        }
    }
}

void fn_18_8C04(void)
{
    Mtx sp8;
    struct BBPlay *tr31;
    s32 r30;

    for (r30 = 0; r30 < 6; r30++) {
        if ((u8) r30 < 6) {
            tr31 = &lbl_18_bss_D68[(u8) r30];
            if (((tr31->unk0 & 1) != 0) && (tr31->unk64 != NULL)) {
                draw_ape(tr31->unk64);
                g_smth_with_rendefc_reflective_height(-1.0f);
                mtxa_from_mtxb();
                mtxa_mult_right(tr31->unk8);
                mtxa_scale_s(tr31->unk58);
                mtxa_to_mtx(sp8);
                fn_1_4EAFC(sp8, tr31->colorId, tr31->unk50);
                g_smth_with_rendefc_reflective_height(0.0f);
            }
        }
    }
}

