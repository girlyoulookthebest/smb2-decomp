#include "macros.h"
#include "mathutil.h"
/* This caller uses the full integer return value; the shared declaration is u8. */
#define fn_18_C394 fn_18_C394_u8
#include "baseball.h"
#undef fn_18_C394
extern s32 fn_18_C394(void);

struct BBPoleData
{
    Vec pos;
    float radius;
    float height;
    S16Vec rot;
};

struct BBPole
{
    u8 filler0[0x48];
    Mtx mtx;
    struct BBPoleData data;
};

struct BBWall lbl_18_data_78E40[12] = {
    { { 23.58f, -1.0f, -25.82f }, { 0x0000, 0xC5B1, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { 20.8f, -1.0f, -33.86f }, { 0x0000, 0xD556, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { 15.49f, -1.0f, -40.1f }, { 0x0000, 0xE4FB, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { 8.33f, -1.0f, -44.125f }, { 0x0000, 0xF163, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { 0.0f, -1.0f, -45.65f }, { 0x0000, 0x0000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { -8.33f, -1.0f, -44.125f }, { 0x0000, 0x0E9D, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { -15.49f, -1.0f, -40.1f }, { 0x0000, 0x1B05, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { -20.8f, -1.0f, -33.86f }, { 0x0000, 0x2AAA, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { -23.58f, -1.0f, -25.82f }, { 0x0000, 0x3A4F, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 7.5f, 1.5f },
    { { 14.7f, -1.0f, -6.2f }, { 0x0000, 0xE9D1, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.783f, 34.5f },
    { { 0.0f, -1.0f, 9.2f }, { 0x0000, 0x0000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.9f, 9.5f },
    { { -14.7f, -1.0f, -6.2f }, { 0x0000, 0x162F, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.783f, 34.5f },
};

struct BBWall lbl_18_data_79020[7] = {
    { { 11.23f, 0.86f, -13.4f }, { 0x1000, 0xE000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.1f, 2.2f },
    { { 17.1229f, 0.8604f, -23.9767f }, { 0x1000, 0xE000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.1f, 2.2f },
    { { 10.0317f, 0.8604f, -31.1006f }, { 0x1000, 0xF000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.1f, 2.2f },
    { { 0.0f, 0.8604f, -33.682f }, { 0x1000, 0x0000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.1f, 2.2f },
    { { -10.032f, 0.8604f, -31.1005f }, { 0x1000, 0x1000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.1f, 2.2f },
    { { -17.1231f, 0.8604f, -23.9765f }, { 0x1000, 0x2000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.1f, 2.2f },
    { { -11.2377f, 0.8604f, -13.3722f }, { 0x1000, 0x2000, 0x0000 }, 0, { 0.0f, 1.0f, 0.0f }, 1.1f, 2.2f },
};

Vec lbl_18_data_79138[7] = {
    { 2.939f, 1.721f, -2.9393f },
    { 2.9391f, 1.721f, -2.9393f },
    { 1.5905f, 1.721f, -3.8403f },
    { -0.0002f, 1.721f, -4.1566f },
    { -1.5908f, 1.721f, -3.8401f },
    { -2.9393f, 1.721f, -2.9391f },
    { -2.939f, 1.721f, -2.9393f },
};

Vec lbl_18_data_7918C[10] = {
    { 25.03304f, 2.01109f, -25.11858f },
    { 23.70674f, 2.01109f, -30.31561f },
    { 19.20808f, 2.01109f, -38.0955f },
    { 12.71443f, 2.01109f, -43.2054f },
    { 4.43113f, 2.01109f, -46.38945f },
    { -4.43113f, 2.01109f, -46.38945f },
    { -12.71443f, 2.01109f, -43.2054f },
    { -19.20808f, 2.01109f, -38.0955f },
    { -23.70673f, 2.01109f, -30.31561f },
    { -25.03304f, 2.01109f, -25.11858f },
};

void fn_18_16740(void)
{
    u8 tr31;

    tr31 = fn_18_19E0();
    if (fn_18_198C8() == 0) {
        switch (tr31) {
        case 0:
            lbl_18_bss_11E8->unk14C = 0;
            break;
        case 1:
            lbl_18_bss_11E8->unk14C = 1;
            break;
        case 2:
            lbl_18_bss_11E8->unk14C = 2;
            break;
        }
    } else {
        switch (tr31) {
        case 0:
            if (fn_18_1A7C() == 1) {
                lbl_18_bss_11E8->unk14C = 2;
            } else {
                lbl_18_bss_11E8->unk14C = 1;
            }
            break;
        case 1:
            lbl_18_bss_11E8->unk14C = 0;
            break;
        case 2:
            lbl_18_bss_11E8->unk14C = 0;
            break;
        }
    }
}

void fn_18_1687C(float farg0)
{
    Vec sp8;
    u8 r31;

    r31 = 0;
    if (fn_18_19B68() == 1) {
        fn_18_1A24(&lbl_18_data_368[0], &lbl_18_data_368[1], &sp8, farg0);
        lbl_18_bss_11E8->unk124[r31].x = sp8.x;
        lbl_18_bss_11E8->unk124[r31].z = sp8.z;
        r31++;
    }
    if (fn_18_19B90() == 1) {
        fn_18_1A24(&lbl_18_data_368[1], &lbl_18_data_368[2], &sp8, farg0);
        lbl_18_bss_11E8->unk124[r31].x = sp8.x;
        lbl_18_bss_11E8->unk124[r31].z = sp8.z;
        r31++;
    }
    if (fn_18_19BB8() == 1) {
        fn_18_1A24(&lbl_18_data_368[2], &lbl_18_data_368[3], &sp8, farg0);
        lbl_18_bss_11E8->unk124[r31].x = sp8.x;
        lbl_18_bss_11E8->unk124[r31].z = sp8.z;
        r31++;
    }
    if (fn_18_19BE0() == 1) {
        fn_18_1A24(&lbl_18_data_368[3], &lbl_18_data_368[0], &sp8, farg0);
        lbl_18_bss_11E8->unk124[r31].x = sp8.x;
        lbl_18_bss_11E8->unk124[r31].z = sp8.z;
        r31++;
    }
    lbl_18_bss_11E8->unk144 = r31;
}

void fn_18_16A94(void)
{
    if (fn_18_198B4() == 1) {
        if (lbl_18_bss_11E8->unk8[0][0] >= 0x63) {
            lbl_18_bss_11E8->unk8[0][0] = 0x62;
        }
    } else if (lbl_18_bss_11E8->unk8[0][1] >= 0x63) {
        lbl_18_bss_11E8->unk8[0][1] = 0x62;
    }
}

s16 fn_18_16B1C(struct PhysicsBall *arg0)
{
    s32 r31;

    for (r31 = 0; r31 < 12; r31++) {
        fn_18_16C48(arg0, &lbl_18_data_78E40[r31]);
        if (arg0->flags & 1) {
            return r31;
        }
    }
    return -1;
}

s32 fn_18_16B94(u8 arg0, s16 arg1)
{
    s16 *tr3;
    s32 i;

    tr3 = fn_18_11E38(arg0);
    if (arg1 < 9) {
        for (i = 0; i < 9; i++) {
            switch (tr3[arg1]) {
            case 0:
                return 0x24;
            case 1:
                return 0x25;
            case 2:
                return 0x26;
            case 3:
                return 0x22;
            default:
                return 0x23;
            }
        }
    } else {
        return 0x23;
    }
    return 0x23;
}

void fn_18_16C48(struct PhysicsBall *arg0, struct BBWall *arg1)
{
    struct ColiPlane sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float tf0;
    float tf1;
    float tf31;
    float tf30;

    sp20 = arg0->prevPos;
    sp14.x = sp20.x - arg1->pos.x;
    sp14.y = sp20.y - arg1->pos.y;
    sp14.z = sp20.z - arg1->pos.z;
    if (sp14.z * arg1->normal.z + (sp14.x * arg1->normal.x + sp14.y * arg1->normal.y) < 0.0) {
        return;
    }
    sp2C = arg0->pos;
    sp14.x = sp2C.x - arg1->pos.x;
    sp14.y = sp2C.y - arg1->pos.y;
    sp14.z = sp2C.z - arg1->pos.z;
    if (sp14.z * arg1->normal.z + (sp14.x * arg1->normal.x + sp14.y * arg1->normal.y) > arg0->radius) {
        return;
    }
    mtxa_from_translate(&arg1->pos);
    mtxa_rotate_y(arg1->rot.y);
    mtxa_rotate_x((s16)-math_atan2(arg1->normal.y, math_sqrt(mathutil_sum_of_sq_2(arg1->normal.x, arg1->normal.z))));
    mtxa_rigid_inv_tf_point(&sp2C, &sp8);
    tf31 = 0.5 * arg1->w;
    tf30 = 0.5 * arg1->h;
    if (sp8.x < -tf31 - arg0->radius) {
        return;
    }
    if (sp8.x > tf31 + arg0->radius) {
        return;
    }
    if (sp8.y < -tf30 - arg0->radius) {
        return;
    }
    if (sp8.y > tf30 + arg0->radius) {
        return;
    }
    if (sp8.x < -tf31) {
        sp14.x = sp8.x + tf31;
        sp14.z = sp8.z;
        tf0 = mathutil_sum_of_sq_2(sp14.x, sp14.z);
        if (tf0 > arg0->radius * arg0->radius) {
            return;
        }
        if (tf0 > FLT_EPSILON) {
            tf1 = math_rsqrt(tf0);
            sp14.x *= tf1;
            sp14.y = 0.0f;
            sp14.z *= tf1;
        } else {
            sp14.x = -1.0f;
            sp14.y = 0.0f;
            sp14.z = 0.0f;
        }
        mtxa_tf_vec(&sp14, &sp38.normal);
        sp14.x = -tf31;
        sp14.y = 0.0f;
        sp14.z = 0.0f;
        mtxa_tf_point(&sp14, &sp38.point);
        collide_ball_with_plane(arg0, &sp38);
    } else if (sp8.x > tf31) {
        sp14.x = sp8.x - tf31;
        sp14.z = sp8.z;
        tf0 = mathutil_sum_of_sq_2(sp14.x, sp14.z);
        if (tf0 > arg0->radius * arg0->radius) {
            return;
        }
        if (tf0 > FLT_EPSILON) {
            tf1 = math_rsqrt(tf0);
            sp14.x *= tf1;
            sp14.y = 0.0f;
            sp14.z *= tf1;
        } else {
            sp14.x = 1.0f;
            sp14.y = 0.0f;
            sp14.z = 0.0f;
        }
        mtxa_tf_vec(&sp14, &sp38.normal);
        sp14.x = tf31;
        sp14.y = 0.0f;
        sp14.z = 0.0f;
        mtxa_tf_point(&sp14, &sp38.point);
        collide_ball_with_plane(arg0, &sp38);
    } else if (sp8.y < -tf30) {
        sp14.y = sp8.y + tf30;
        sp14.z = sp8.z;
        tf0 = mathutil_sum_of_sq_2(sp14.y, sp14.z);
        if (tf0 > arg0->radius * arg0->radius) {
            return;
        }
        if (tf0 > FLT_EPSILON) {
            tf1 = math_rsqrt(tf0);
            sp14.x = 0.0f;
            sp14.y *= tf1;
            sp14.z *= tf1;
        } else {
            sp14.x = 0.0f;
            sp14.y = -1.0f;
            sp14.z = 0.0f;
        }
        mtxa_tf_vec(&sp14, &sp38.normal);
        sp14.x = 0.0f;
        sp14.y = -tf30;
        sp14.z = 0.0f;
        mtxa_tf_point(&sp14, &sp38.point);
        collide_ball_with_plane(arg0, &sp38);
    } else if (sp8.y > tf30) {
        sp14.y = sp8.y - tf30;
        sp14.z = sp8.z;
        tf0 = mathutil_sum_of_sq_2(sp14.y, sp14.z);
        if (tf0 > arg0->radius * arg0->radius) {
            return;
        }
        if (tf0 > FLT_EPSILON) {
            tf1 = math_rsqrt(tf0);
            sp14.x = 0.0f;
            sp14.y *= tf1;
            sp14.z *= tf1;
        } else {
            sp14.x = 0.0f;
            sp14.y = 1.0f;
            sp14.z = 0.0f;
        }
        mtxa_tf_vec(&sp14, &sp38.normal);
        sp14.x = 0.0f;
        sp14.y = tf30;
        sp14.z = 0.0f;
        mtxa_tf_point(&sp14, &sp38.point);
        collide_ball_with_plane(arg0, &sp38);
    } else {
        if (sp8.x < -tf31 || sp8.x > tf31) {
            return;
        }
        if (sp8.y < -tf30 || sp8.y > tf30) {
            return;
        }
        sp38.point = arg1->pos;
        sp38.normal = arg1->normal;
        collide_ball_with_plane(arg0, &sp38);
    }
}

s32 fn_18_1722C(struct PhysicsBall *arg0, struct PhysicsBall *arg1, float farg0)
{
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float tf30;
    float tf29;
    float tf28;
    float tf2;
    float tf1;

    PSVECSubtract(&arg1->prevPos, &arg0->prevPos, &sp2C);
    PSVECSubtract(&arg0->vel, &arg1->vel, &sp20);
    PSVECSubtract(&arg0->pos, &arg1->pos, &sp14);
    if (mathutil_vec_dot_prod(&sp2C, &sp20) > 0.0f
        && fn_1_A74E4(&arg0->prevPos, &arg0->pos, arg0->radius, &arg1->prevPos, &arg1->pos, arg1->radius) != 0) {
        tf30 = mathutil_vec_sq_len(&arg0->vel);
        tf29 = mathutil_vec_sq_len(&arg1->vel);
        fn_18_17410(&sp14, &arg0->vel, 1.0f, &arg1->vel, 1.0f);
        tf28 = tf30 + tf29;
        PSVECAdd(&arg0->pos, &arg0->vel, &arg0->pos);
        PSVECAdd(&arg1->pos, &arg1->vel, &arg1->pos);
    }
    tf1 = vec_normalize_len(&sp14);
    tf1 -= farg0;
    tf2 = arg0->radius + arg1->radius;
    tf1 = tf2 - tf1;
    if (tf1 > 0.0f) {
        PSVECScale(&sp14, &sp8, 0.5f * tf1);
        PSVECAdd(&arg0->pos, &sp8, &arg0->pos);
        PSVECSubtract(&arg1->pos, &sp8, &arg1->pos);
        return 1;
    }
    return 0;
}

void fn_18_17410(Vec *arg0, Vec *arg1, float farg0, Vec *arg2, float farg1)
{
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float tf8;
    float tf0;
    float tf2;
    float tf3;

    sp20 = *arg1;
    sp14 = *arg2;
    sp8 = *arg0;
    tf8 = mathutil_vec_dot_prod(&sp8, &sp20);
    tf0 = mathutil_vec_dot_prod(&sp8, &sp14);
    tf3 = 1.5 * ((tf0 - tf8) * ((farg0 * farg1) / (farg0 * farg1 + farg0 * farg0)));
    tf2 = tf3 * -(farg0 / farg1);
    sp20.x += tf3 * sp8.x;
    sp20.y += tf3 * sp8.y;
    sp20.z += tf3 * sp8.z;
    sp14.x += tf2 * sp8.x;
    sp14.y += tf2 * sp8.y;
    sp14.z += tf2 * sp8.z;
    *arg1 = sp20;
    *arg2 = sp14;
}

s32 fn_18_175A4(Vec *arg0, struct PhysicsBall *arg1)
{
    s32 r31;
    u8 tr29;

    tr29 = fn_18_14AF4();
    for (r31 = 0; r31 < 7; r31++) {
        if (lbl_18_data_78BC8[tr29][r31] != 0) {
            fn_18_16C48(arg1, &lbl_18_data_79020[r31]);
            if (arg1->flags & 1) {
                arg0->x = lbl_18_data_79138[r31].x;
                arg0->y = lbl_18_data_79138[r31].y;
                arg0->z = lbl_18_data_79138[r31].z;
                vec_set_len(0.8f, arg0, arg0);
                return 1;
            }
        }
    }
    return 0;
}

/* Named assembly operands preserve the original cross-product instruction order
 * while allowing the loop to retain its origin and endpoint pointers. */
#ifdef __MWERKS__
#define BB_CROSS_Y_ASM(a, b, c, load0, load1, delta, product) \
    asm { \
        lfs load1, 0(b); \
        lfs load0, 0(a); \
        fsubs delta, load1, load0; \
        lfs load1, 8(c); \
        lfs load0, 8(a); \
        fsubs load0, load1, load0; \
        fmuls product, delta, load0; \
        lfs load1, 0(c); \
        lfs load0, 0(a); \
        fsubs delta, load1, load0; \
        lfs load1, 8(b); \
        lfs load0, 8(a); \
        fsubs load0, load1, load0; \
        fmuls load0, delta, load0; \
        fsubs load0, product, load0; \
        fmr load1, load0; \
    }
#else
#define BB_CROSS_Y_ASM(a, b, c, load0, load1, delta, product) \
    ((load1) = ((b)->x - (a)->x) * ((c)->z - (a)->z) \
        - ((c)->x - (a)->x) * ((b)->z - (a)->z))
#endif

static inline float bb_cross_y(Vec *aa, Vec *bb, Vec *cc)
{
    register Vec *c, *b, *a;
    register float product, delta, load1, load0;

    a = aa;
    b = bb;
    c = cc;
    BB_CROSS_Y_ASM(a, b, c, load0, load1, delta, product);
    return load1;
}

static inline float bb_dist_xz(Vec *a, Vec *b)
{
    float dy;
    float dx;
    float dz;
    dx = a->x - b->x;
    dy = 0.0f;
    dz = a->z - b->z;

    return math_sqrt(dz * dz + (dx * dx + dy * dy));
}

s32 fn_18_176BC(Vec *arg0)
{
    Vec sp8[3] = {
        { 0.0f, 0.0f, 0.0f },
        { 9.7752f, 0.0f, -9.9543f },
        { -9.7752f, 0.0f, -9.9541f },
    };
    float tf31;

    if (fn_18_C3A8() == 1) {
        fn_18_C3BC();
        if (bb_cross_y(&sp8[0], &sp8[1], arg0) <= 0.0f && bb_cross_y(&sp8[0], &sp8[2], arg0) >= 0.0f) {
            return 0;
        }
        return 1;
    }
    tf31 = fn_18_C154();
    if (arg0->y < -7.0) {
        return 1;
    }
    if (tf31 <= 4.0) {
        return 0;
    }
    if (arg0->z > 0.0) {
        return 1;
    }
    if (fn_18_C394() == 0) {
        return 0;
    }
    if (tf31 >= 13.812043f) {
        return 0;
    }
    if (bb_cross_y(&sp8[0], &sp8[1], arg0) <= 0.0f && bb_cross_y(&sp8[0], &sp8[2], arg0) >= 0.0f) {
        return 0;
    }
    return 1;
}

s32 fn_18_17968(register Vec *arg0)
{
    Vec sp8 = { 0.0f, 0.0f, 0.0f };
    register Vec *origin;
    register Vec *tr5;
    register Vec *tr28;
    register float load0, load1, delta, product;
    float tf31;
    s32 index;

    if (arg0->y < 2.01109) {
        return 0;
    }
    origin = &sp8;
    for (index = 0; index < 9; index++) {
        tr5 = &lbl_18_data_7918C[index];
        BB_CROSS_Y_ASM(origin, tr5, arg0, load0, load1, delta, product);
        if (load1 <= 0.0f) {
            tr28 = &lbl_18_data_7918C[index + 1];
            BB_CROSS_Y_ASM(origin, tr28, arg0, load0, load1, delta, product);
            if (load1 >= 0.0f) {
                tf31 = bb_dist_xz(&sp8, tr5);
                tf31 = (tf31 + bb_dist_xz(&sp8, tr28)) / 2.0f;
                if (tf31 < bb_dist_xz(&sp8, arg0)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

s32 fn_18_17B7C(struct PhysicsBall *arg0, Mtx arg1)
{
    struct ColiPlane sp50;
    struct ColiCircle sp38;
    struct ColiCircle sp20;
    Vec sp14;
    Vec sp8;
    float tf0_2;
    float tf0_3;
    float tf1;
    struct BBPole *tr30;
    struct BBPoleData *tr29;

    tr30 = (struct BBPole *)&lbl_18_bss_2E0.unk44;
    tr29 = &tr30->data;
    mtxa_from_mtx(arg1);
    mtxa_push();
    mtxa_from_translate(&tr29->pos);
    mtxa_rotate_z(tr29->rot.z);
    mtxa_rotate_y(tr29->rot.y);
    mtxa_rotate_x(tr29->rot.x);
    mtxa_to_mtx(tr30->mtx);
    mtxa_pop();
    mtxa_mult_right(tr30->mtx);
    mtxa_to_mtx(tr30->mtx);
    mtxa_from_mtx(tr30->mtx);
    mtxa_rigid_inv_tf_point(&arg0->pos, &sp14);
    mtxa_rigid_inv_tf_point(&arg0->prevPos, &sp8);
    tf0_2 = mathutil_sum_of_sq_2(sp14.x, sp14.z);
    tf0_3 = tr29->radius + arg0->radius;
    if (tf0_2 > tf0_3 * tf0_3) {
        return 0;
    }
    if (tf0_2 < FLT_EPSILON) {
        return 0;
    }
    if (sp8.y < 0.5 * -tr29->height) {
        sp38.pos.x = 0.0f;
        sp38.pos.y = 0.5 * -tr29->height;
        sp38.pos.z = 0.0f;
        mtxa_tf_point(&sp38.pos, &sp38.pos);
        sp38.radius = tr29->radius;
        sp38.rot.x = tr29->rot.x - 0x8000;
        sp38.rot.y = tr29->rot.y;
        sp38.rot.z = tr29->rot.z;
        collide_ball_with_circle(arg0, &sp38);
        return (arg0->flags & 1) != 0;
    }
    if (sp8.y > 0.5 * tr29->height) {
        sp20.pos.x = 0.0f;
        sp20.pos.y = 0.5 * tr29->height;
        sp20.pos.z = 0.0f;
        mtxa_tf_point(&sp20.pos, &sp20.pos);
        sp20.radius = tr29->radius;
        sp20.rot.x = tr29->rot.x;
        sp20.rot.y = tr29->rot.y;
        sp20.rot.z = tr29->rot.z;
        collide_ball_with_circle(arg0, &sp20);
        return (arg0->flags & 1) != 0;
    }
    tf1 = math_rsqrt(tf0_2);
    sp50.normal.x = sp14.x * tf1;
    sp50.normal.y = 0.0f;
    sp50.normal.z = sp14.z * tf1;
    sp50.point.x = sp50.normal.x * tr29->radius;
    sp50.point.y = sp14.y + sp50.normal.y * tr29->radius;
    sp50.point.z = sp50.normal.z * tr29->radius;
    mtxa_tf_vec(&sp50.normal, &sp50.normal);
    mtxa_tf_point(&sp50.point, &sp50.point);
    collide_ball_with_plane(arg0, &sp50);
    return (arg0->flags & 1) != 0;
}

/* Unreferenced tentative definitions are allocated in reverse order by MWCC. */
s32 lbl_18_bss_1220[8];
s32 lbl_18_bss_121C;
u32 lbl_18_bss_1218;
struct BBRules lbl_18_bss_11F0;
