#ifndef MATHUTIL_H
#define MATHUTIL_H

#include "types.h"

typedef struct { float x, y, z; } Vec;
typedef struct { s16 x, y, z; } S16Vec;
typedef struct { float x, y; } Vec2d;
typedef float Mtx[3][4];
typedef float Mtx44[4][4];
typedef struct { float x, y, z, w; } Quat;

#ifdef __MWERKS__
#define fabs(x) __fabs(x)  /* compiler intrinsic */
#else
#include <math.h>
#endif

#define S16_TO_DEGREES(angle) ((angle) * (360.0f / 65536.0f))

/* mathutil's state; mtxA is the top of the locked-cache matrix stack. */
struct MathutilData
{
    /*0x00*/ Mtx mtxA;
    /*0x30*/ Mtx mtxB;
};

extern struct MathutilData *mtxa;

#define DEGREES_TO_S16(angle) ((s16)((angle) * (65536.0f / 360.0f)))

/* Sega's math library in main.dol (hand-written asm, matrix stack in locked cache).
 * "mtxa" is the current matrix at the top of that stack. */
float math_sqrt(float x);
float math_rsqrt(float x);
float math_tan(s16 angle);
void math_sin_cos_v(s16 angle, float *sincos);  /* sincos[0] = sin, sincos[1] = cos */
s16 math_atan2(float y, float x);
s16 math_atan(float x);

void mtxa_from_identity(void);
void mtxa_from_mtxb(void);
void mtxa_from_mtxb_translate(Vec *v);
void mtxa_from_quat(Quat *q);
void mtxa_mult_left(Mtx m);
void mtxa_mult_right(Mtx m);
void mtxa_rigid_invert(void);
void mtxa_to_euler(S16Vec *rot);
void g_math_unk9_smth_w_quats(Quat *out, Vec *axis, float angle);  /* quaternion from axis and angle */
void mtxa_sq_from_identity(void);
void mtxa_from_rotate_y(s16 angle);
void mtxa_from_rotate_z(s16 angle);
void mtxa_from_translate(Vec *v);
void mtxa_from_translate_xyz(float x, float y, float z);
void mtxa_from_mtx(Mtx m);
void mtxa_to_mtx(Mtx m);
void mtx_copy(Mtx src, Mtx dst);
void mtxa_push(void);
void mtxa_pop(void);
void mtxa_translate(Vec *v);
void mtxa_scale_s(float s);
void mtxa_scale_xyz(float x, float y, float z);
void mtxa_scale(Vec *s);
void mtxa_peek(void);
void mtxa_invert(void);
void mtxa_translate_neg(Vec *v);
void mtxa_translate_xyz(float x, float y, float z);
void mtxa_to_euler_yxz(s16 *rotY, s16 *rotX, s16 *rotZ);
void mtxa_rotate_x(s16 angle);
void mtxa_rotate_y(s16 angle);
void mtxa_rotate_z(s16 angle);
void mtxa_sq_to_mtx(Mtx m);
void mtxa_tf_point(Vec *src, Vec *dst);
void mtxa_rigid_inv_tf_point(Vec *src, Vec *dst);
void mtxa_tf_vec(Vec *src, Vec *dst);
/* Float parameters first (ABI-identical) for the original argument evaluation order. */
void mtxa_tf_vec_xyz(float x, float y, float z, Vec *dst);
/* Float parameters first (ABI-identical) for the original argument evaluation order. */
void mtxa_tf_point_xyz(float x, float y, float z, Vec *dst);
void mtxa_rigid_inv_tf_vec(Vec *src, Vec *dst);
void mtxa_rigid_inv_tf_point_xyz(float x, float y, float z, Vec *dst);
void mtxa_normalize_basis(void);
float vec_dot_normalized(Vec *a, Vec *b);
void quat_from_axis_angle(Quat *out, Vec *axis, s16 angle);
void quat_normalize(Quat *q);
void quat_from_dirs(Quat *out, Vec *a, Vec *b);
void mtxa_rigid_inv_tf_tl(Vec *dst);
/* Parameter order picked to reproduce the original argument evaluation order; the
 * ABI is unchanged because float and pointer arguments use separate registers. */
void vec_set_len(float len, Vec *src, Vec *dst);

/* The original uses inline asm for these (as in SMB1), so the multiplies fuse exactly as
 * in the shipped code. Other compilers (GCC for PC/PSP ports) get the plain C version. */
static inline float mathutil_sum_of_sq_2(register float a, register float b)
{
#ifdef __MWERKS__
    asm
    {
        fmuls a, a, a
        fmadds a, b, b, a
    }
    return a;
#else
    return a * a + b * b;
#endif
}

static inline float mathutil_sum_of_sq_3(register float a, register float b, register float c)
{
#ifdef __MWERKS__
    asm
    {
        fmuls a, a, a
        fmadds a, b, b, a
        fmadds a, c, c, a
    }
    return a;
#else
    return a * a + b * b + c * c;
#endif
}

/* sqrt((a - c)^2 + (b - d)^2); SMB1's mathutil_unk */
static inline float mathutil_distance_2(register float a, register float b, register float c, register float d)
{
#ifdef __MWERKS__
    asm
    {
        fsubs a, a, c
        fsubs b, b, d
        fmuls a, a, a
        fmadds a, b, b, a
    }
    return math_sqrt(a);
#else
    a -= c;
    b -= d;
    return math_sqrt(a * a + b * b);
#endif
}

static inline float mathutil_vec_len(register Vec *v)
{
#ifdef __MWERKS__
    register float x, z, y;
    asm
    {
        lfs x, v->x
        lfs y, v->y
        lfs z, v->z
        fmuls x, x, x
        fmadds x, y, y, x
        fmadds x, z, z, x
    }
    return math_sqrt(x);
#else
    return math_sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
#endif
}

static inline float mathutil_vec_sq_len(register Vec *v)
{
#ifdef __MWERKS__
    register float z, y, x;
    asm
    {
        lfs x, v->x
        lfs y, v->y
        lfs z, v->z
        fmuls x, x, x
        fmadds x, y, y, x
        fmadds x, z, z, x
    }
    return x;
#else
    return v->x * v->x + v->y * v->y + v->z * v->z;
#endif
}

static inline float mathutil_vec_distance(register Vec *a, register Vec *b)
{
#ifdef __MWERKS__
    /* Declaration order picks the registers; this order matches the original. */
    register float x1, z1, y1, x2, y2, z2;
    asm
    {
        lfs x1, a->x
        lfs x2, b->x
        lfs y1, a->y
        lfs y2, b->y
        lfs z1, a->z
        lfs z2, b->z
        fsubs x1, x1, x2
        fsubs y1, y1, y2
        fsubs z1, z1, z2
        fmuls x1, x1, x1
        fmadds x1, y1, y1, x1
        fmadds x1, z1, z1, x1
    }
    return math_sqrt(x1);
#else
    return math_sqrt(mathutil_sum_of_sq_3(a->x - b->x, a->y - b->y, a->z - b->z));
#endif
}

static inline float mathutil_vec_sq_distance(register Vec *a, register Vec *b)
{
#ifdef __MWERKS__
    /* Declaration order picks the registers; this order matches the original. */
    register float z1, y1, x1, x2, y2, z2;
    asm
    {
        lfs x1, a->x
        lfs x2, b->x
        lfs y1, a->y
        lfs y2, b->y
        lfs z1, a->z
        lfs z2, b->z
        fsubs x1, x1, x2
        fsubs y1, y1, y2
        fsubs z1, z1, z2
        fmuls x1, x1, x1
        fmadds x1, y1, y1, x1
        fmadds x1, z1, z1, x1
    }
    return x1;
#else
    return mathutil_sum_of_sq_3(a->x - b->x, a->y - b->y, a->z - b->z);
#endif
}

static inline float mathutil_vec_dot_prod(register Vec *a, register Vec *b)
{
#ifdef __MWERKS__
    /* Registers are handed out in reverse declaration order; this order matches the original. */
    register float z2, y2, x2, z1, y1, x1;
    asm
    {
        lfs x1, a->x
        lfs x2, b->x
        lfs y1, a->y
        lfs y2, b->y
        lfs z1, a->z
        lfs z2, b->z
        fmuls x1, x1, x2
        fmadds x1, y1, y2, x1
        fmadds x1, z1, z2, x1
    }
    return x1;
#else
    return a->x * b->x + a->y * b->y + a->z * b->z;
#endif
}

float vec_normalize_len(Vec *v);
float vec_dot_normalized_safe(Vec *a, Vec *b);
float math_sin(s16 angle);

#define LC_CACHE_BASE 0xE0000000  /* locked cache, where mathutil keeps mtxA */

static inline void mathutil_mtxA_get_col2_scaled(register float scale, register Vec *v)
{
#ifdef __MWERKS__
    register float *mtxA;
    register float x, y, z;

    asm
    {
        lis mtxA, LC_CACHE_BASE@ha
        lfs x, 0x08(mtxA)  /* mtxA[0][2] */
        lfs y, 0x18(mtxA)  /* mtxA[1][2] */
        lfs z, 0x28(mtxA)  /* mtxA[2][2] */
        fmuls x, x, scale
        stfs x, v->x
        fmuls y, y, scale
        stfs y, v->y
        fmuls z, z, scale
        stfs z, v->z
    }
#else
    v->x = mtxa->mtxA[0][2] * scale;
    v->y = mtxa->mtxA[1][2] * scale;
    v->z = mtxa->mtxA[2][2] * scale;
#endif
}

static inline void mathutil_mtxA_get_translate(register Vec *v)
{
#ifdef __MWERKS__
    /* Declaration order picks the registers; this order matches the original. */
    register float *_z;
    register float *_y;
    register float *_x;
    register float *mtxA;
    register float x, y, z;

    _x = &v->x;
    _y = _x + 1;
    _z = _x + 2;

    asm
    {
        lis mtxA, LC_CACHE_BASE@ha
        lfs x, 0x0C(mtxA)  /* mtxA[0][3] */
        lfs y, 0x1C(mtxA)  /* mtxA[1][3] */
        lfs z, 0x2C(mtxA)  /* mtxA[2][3] */
        stfs x, 0(_x)
        stfs y, 0(_y)
        stfs z, 0(_z)
    }
#else
    v->x = mtxa->mtxA[0][3];
    v->y = mtxa->mtxA[1][3];
    v->z = mtxa->mtxA[2][3];
#endif
}

static inline void mathutil_mtxA_set_translate(register Vec *v)
{
#ifdef __MWERKS__
    register float *mtxA;
    register float z, y, x;

    asm
    {
        lfs x, v->x
        lfs y, v->y
        lfs z, v->z
        lis mtxA, LC_CACHE_BASE@ha
        stfs x, 0x0C(mtxA)  /* mtxA[0][3] */
        stfs y, 0x1C(mtxA)  /* mtxA[1][3] */
        stfs z, 0x2C(mtxA)  /* mtxA[2][3] */
    }
#else
    mtxa->mtxA[0][3] = v->x;
    mtxa->mtxA[1][3] = v->y;
    mtxa->mtxA[2][3] = v->z;
#endif
}

static inline void mathutil_mtxA_set_translate_xyz(register float x, register float y, register float z)
{
#ifdef __MWERKS__
    register float *mtxA;

    asm
    {
        lis mtxA, LC_CACHE_BASE@ha
        stfs x, 0x0C(mtxA)  /* mtxA[0][3] */
        stfs y, 0x1C(mtxA)  /* mtxA[1][3] */
        stfs z, 0x2C(mtxA)  /* mtxA[2][3] */
    }
#else
    mtxa->mtxA[0][3] = x;
    mtxa->mtxA[1][3] = y;
    mtxa->mtxA[2][3] = z;
#endif
}

static inline void mathutil_vec_cross_prod(register Vec *a, register Vec *b, register Vec *result)
{
#ifdef __MWERKS__
    register float x1, y1, z1, x2, y2, z2;
    register float x, y, z;

    asm
    {
        lfs y1, a->y
        lfs z2, b->z
        lfs z1, a->z
        lfs x2, b->x
        lfs x1, a->x
        lfs y2, b->y

        fmuls x, y1, z2
        fmuls y, z1, x2
        fmuls z, x1, y2
        fnmsubs x, z1, y2, x
        stfs x, result->x
        fnmsubs y, x1, z2, y
        stfs y, result->y
        fnmsubs z, y1, x2, z
        stfs z, result->z
    }
#else
    float x = a->y * b->z - a->z * b->y;
    float y = a->z * b->x - a->x * b->z;
    float z = a->x * b->y - a->y * b->x;

    result->x = x;
    result->y = y;
    result->z = z;
#endif
}

/* Cosine as a quarter-turn-shifted sine. A macro: the shift is done in int, unextended. */
#define mathutil_cos(angle) math_sin((angle) + 0x4000)

/* Round toward -inf (SMB1's mathutil_floor): switches the FPSCR rounding mode around fctiw. */
static inline float mathutil_floor(register float n)
{
#ifdef __MWERKS__
    s32 buf[2];
    register float savedFlags;
    asm
    {
        mffs savedFlags
        mtfsb1 30
        mtfsb1 31
        fctiw n, n
        stfd n, buf[0]
        mtfsf 0xFF, savedFlags
    }
    return buf[1];
#else
    s32 i = (s32)n;
    if (n < i)
        i--;
    return i;
#endif
}

/* Round toward zero: same dance with the other rounding mode. */
static inline float mathutil_trunc(register float n)
{
#ifdef __MWERKS__
    s32 buf[2];
    register float savedFlags;
    register float result;
    asm
    {
        mffs savedFlags
        mtfsb0 30
        mtfsb1 31
        fctiw result, n
        stfd result, buf[0]
        mtfsf 0xFF, savedFlags
    }
    return buf[1];
#else
    return (s32)n;
#endif
}

/* Round toward +inf (SMB1's mathutil_ceil): same dance with the other rounding mode. */
static inline float mathutil_ceil(register float n)
{
#ifdef __MWERKS__
    s32 buf[2];
    register float savedFlags;
    asm
    {
        mffs savedFlags
        mtfsb1 30
        mtfsb0 31
        fctiw n, n
        stfd n, buf[0]
        mtfsf 0xFF, savedFlags
    }
    return buf[1];
#else
    s32 i = (s32)n;
    if (n > i)
        i++;
    return i;
#endif
}

#endif
