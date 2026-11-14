/* Water spray and wake thrown up by the balls near the arena floor. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "mathutil.h"
#include "rand.h"
#ifndef __MWERKS__
/* pc_port: undeclared in the original; an implicit declaration passes float args as double on x86 */
void mtxa_rotate_z_sin_cos(float sin, float cos);
#endif

struct DropParticle
{
    Vec pos;
    Vec vel;
    float scale;
    float alpha;
    float timer;
};

struct SprayPool
{
    u32 count;
    float accum;
    struct DropParticle particles[0x80];
    u32 pad;
};

struct WakePool
{
    u32 count;
    float accum;
    struct DropParticle particles[0x200];
    u32 pad;
};

struct LineVert
{
    Vec pos;
    GXColor color;
};

extern struct GMA *minigameGma[];
extern u32 g_font_char_var_3;
extern struct SprayPool *lbl_17_bss_4;
extern struct WakePool *lbl_17_bss_8;
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void fn_1_E326C(struct LineVert *a, struct LineVert *b);

void fn_17_F474(struct SprayPool *pool, u32 count);
void fn_17_F4BC(Vec *pos, Vec *vel, struct SprayPool *pool);
void fn_17_FA08(Vec *pos, Vec *vel, struct WakePool *pool);
void fn_17_10170(Vec *pos, Vec *vel, int frame);

static s32 lbl_17_data_1190[] =
{
    0x5B, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6A, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62,
};

float lbl_17_bss_3C0[4];

void fn_17_F404(void)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        fn_17_F474(&lbl_17_bss_4[i], 0x80);
        fn_17_F474((struct SprayPool *)&lbl_17_bss_8[i], 0x200);
    }
}

void fn_17_F474(struct SprayPool *pool, u32 count)
{
    struct DropParticle *p = pool->particles;
    u32 i;

    pool->count = count;
    pool->accum = 0.0f;
    for (i = 0; i < count; i++)
        p[i].timer = 0.0f;
}

static void spray_update(struct DropParticle *p, u32 i)
{
    struct GMAModel *model;

    model = minigameGma[0]->modelEntries[0x2D].model;
    if (!(g_some_other_flags & 0xA))
    {
        p[i].pos.x += p[i].vel.x;
        p[i].pos.y += p[i].vel.y;
        p[i].pos.z += p[i].vel.z;
        p[i].vel.x *= 0.98f;
        p[i].vel.z *= 0.98f;
        if (p[i].vel.y < -0.1f)
            p[i].vel.y *= 0.3f;
        else
            p[i].vel.y += 0.001f;
        p[i].scale *= 1.01f;
        if (p[i].timer < 60.0f)
            p[i].alpha -= 0.016666668f;
        if (p[i].pos.y > 0.0f)
            p[i].timer = 0.0f;
        else
            p[i].timer -= 1.0f;
    }
    mtxa_from_mtxb_translate(&p[i].pos);
    mtxa_sq_from_identity();
    mtxa_rotate_x(0x4000);
    mtxa_scale_s(p[i].scale);
    avdisp_set_alpha(p[i].alpha);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_always(model);
}

void fn_17_F4BC(Vec *pos, Vec *vel, struct SprayPool *arg)
{
    u32 i;
    struct DropParticle *p = arg->particles;
    struct SprayPool *pool = arg;
    float depth;
    float size;

    if (pos->y > 0.0f)
        depth = 0.0f;
    else
    {
        depth = (100.0f + pos->y) / 100.0f;
        if (depth < 0.0f)
            depth = 0.0f;
    }
    if (!(g_some_other_flags & 0xA))
        pool->accum += 1.5f * depth;
    size = 0.15f * depth;
    for (i = 0; i < pool->count; i++)
    {
        if (p[i].timer > 0.0f)
        {
            spray_update(p, i);
        }
        else if (!(g_some_other_flags & 0xA) && pool->accum > 1.0f)
        {
            p[i].timer = 60.0f + 60.0f * (0.5f * ((float)rand() / 32767.0f));
            p[i].scale = size * (0.2f + (float)rand() / 32767.0f);
            p[i].pos = *pos;
            p[i].vel.x = 0.9f * vel->x;
            p[i].vel.y = vel->y;
            p[i].vel.z = 0.9f * vel->z;
            p[i].vel.x += 0.2f * ((float)rand() / 32767.0f - 0.5f);
            p[i].vel.z += 0.2f * ((float)rand() / 32767.0f - 0.5f);
            p[i].alpha = 1.0f;
            pool->accum -= 1.0f;
        }
    }
}

void fn_17_F964(struct Ball *ball)
{
    Vec v;

    if (ball->pos.y >= 0.0f)
        return;
    mtxa_from_quat(&ball->ape->rot);
    mtxa_tf_point_xyz(0.15f, 0.0f, 0.1f, &v);
    PSVECAdd(&v, &ball->pos, &v);
    fn_17_F4BC(&v, &ball->vel, &lbl_17_bss_4[ball->playerId]);
}

static void wake_update(struct DropParticle *p, u32 i)
{
    struct LineVert va;
    struct LineVert vb;
    Vec tail;

    mtxa_push();
    if (!(g_some_other_flags & 0xA))
    {
        p[i].pos.x += p[i].vel.x - -(0.1f * lbl_17_bss_3C0[0]);
        p[i].pos.y += p[i].vel.y - -(0.1f * lbl_17_bss_3C0[1]);
        p[i].pos.z += p[i].vel.z - -(0.1f * lbl_17_bss_3C0[2]);
    }
    va.color.r = va.color.g = va.color.b = 0xFF;
    va.color.a = 230.0f * p[i].alpha;
    va.pos = p[i].pos;
    vb.color.r = vb.color.g = vb.color.b = 0xFF;
    vb.color.a = 0;
    vec_set_len(0.05f, &p[i].vel, &tail);
    vb.pos.x = p[i].pos.x - tail.x;
    vb.pos.y = p[i].pos.y - tail.y;
    vb.pos.z = p[i].pos.z - tail.z;
    if (!(g_some_other_flags & 0xA))
    {
        p[i].vel.x *= 0.999f;
        p[i].vel.z *= 0.999f;
        p[i].vel.y -= 0.01f;
        p[i].alpha *= 0.9f;
        if (p[i].pos.y < 0.0f)
            p[i].timer = 0.0f;
        else
            p[i].timer -= 1.0f;
    }
    mtxa_from_mtxb();
    fn_1_E326C(&va, &vb);
    mtxa_pop();
}

void fn_17_FA08(Vec *pos, Vec *vel, struct WakePool *arg)
{
    u32 i;
    struct DropParticle *p = arg->particles;
    struct WakePool *pool = arg;
    int count;
    Vec up = {0.0f, 0.0f, -1.0f};
    Vec dir;
    Quat q;
    Vec off;
    float r;
    int flag;
    float k;

    count = 0;
    dir = *vel;
    vec_normalize_len(&dir);
    quat_from_dirs(&q, &up, &dir);
    mtxa_from_quat(&q);
    mtxa_normalize_basis();
    k = 7.2f * mathutil_vec_len(vel);
    pool->accum += k;
    fn_1_E2FD0(20);
    fn_1_E2FE0(1, 2, 1, 15);
    for (i = 0; i < pool->count; i++)
    {
        if (p[i].timer > 0.0f)
        {
            wake_update(p, i);
            count++;
        }
        else if (pool->accum > 1.0f)
        {
            r = (float)rand() / 32767.0f - 0.5f;
            if (!(g_some_other_flags & 0xA))
                p[i].timer = 60.0f + 1.5f * (60.0f * ((float)rand() / 32767.0f));
            p[i].scale = 0.2f;
            flag = (float)rand() / 32767.0f > 0.2f;
            if (flag != 0)
            {
                if (r < 0.0f)
                    r -= 0.4f;
                else
                    r += 0.4f;
                off.x = r;
                off.y = 0.0f;
                off.z = 1.8f * ((float)rand() / 32767.0f - 0.1f);
            }
            else
            {
                off.x = (float)rand() / 32767.0f - 0.5f;
                off.y = 0.0f;
                off.z = 0.4f * r - 0.7f;
            }
            mtxa_tf_vec(&off, &off);
            p[i].pos.x = pos->x + off.x;
            p[i].pos.y = 0.0f;
            p[i].pos.z = pos->z + off.z;
            if (flag != 0)
            {
                p[i].vel.x = r * (0.04f * ((float)rand() / 32767.0f));
                p[i].vel.y = 0.25f * ((float)rand() / 32767.0f);
                p[i].vel.z = -1.0f * mathutil_vec_len(vel);
            }
            else
            {
                p[i].vel.x = 0.2f * ((float)rand() / 32767.0f) - 0.1f;
                p[i].vel.y = 0.18f * ((float)rand() / 32767.0f);
                p[i].vel.z = mathutil_vec_len(vel) * (-0.5f * ((float)rand() / 32767.0f) - 0.7f);
            }
            mtxa_tf_vec(&p[i].vel, &p[i].vel);
            p[i].alpha = 1.0f;
            pool->accum -= 1.0f;
        }
    }
}

void fn_17_10170(Vec *pos, Vec *vel, int frame)
{
    struct GMAModelEntry *entries = minigameGma[0]->modelEntries;
    Mtx m;
    Vec dir1;
    Vec trans1;
    Vec dir2;
    Vec trans2;
    float alpha;
    float z;
    float s;
    float s2;
    int i;
    struct GMAModel *model;

    mtxa_push();
    mtxa_to_mtx(m);
    mtxa_from_mtxb();
    mtxa_mult_right(m);
    alpha = 216.0f * mathutil_vec_len(vel);
    alpha /= 120.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    avdisp_set_alpha(alpha);
    mtxa_push();
    mathutil_mtxA_get_translate(&trans1);
    mtxa_tf_vec_xyz(1.0f, 0.0f, 0.0f, &dir1);
    z = dir1.z;
    dir1.x = math_sqrt(dir1.x * dir1.x + dir1.z * dir1.z);
    dir1.z = 0.0f;
    vec_normalize_len(&dir1);
    mtxa_sq_from_identity();
    mtxa_rotate_z_sin_cos(dir1.y, dir1.x);
    s = 3.0 * __fabs(z);
    mathutil_mtxA_set_translate_xyz(trans1.x - 0.5f, trans1.y - 0.02f, trans1.z - 0.5f);
    mtxa_scale_xyz(s, 1.0f + (s + alpha), 1.0f);
    for (i = 0; i < 5; i++)
    {
        mtxa_translate_xyz(0.02f, 0.0f, 0.01f);
        mtxa_rotate_z(-0x400);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha(1.0f - 0.8f * ((float)i / 5.0f));
        model = entries[lbl_17_data_1190[frame & 0xF]].model;
        frame++;
        avdisp_draw_model_unculled_sort_never(model);
    }
    mtxa_pop();
    mtxa_push();
    mathutil_mtxA_get_translate(&trans2);
    mtxa_tf_vec_xyz(-1.0f, 0.0f, 0.0f, &dir2);
    z = dir2.z;
    dir2.x = math_sqrt(dir2.x * dir2.x + dir2.z * dir2.z);
    dir2.z = 0.0f;
    vec_normalize_len(&dir2);
    mtxa_sq_from_identity();
    mtxa_rotate_z_sin_cos(dir2.y, dir2.x);
    s2 = 3.0 * __fabs(z);
    mathutil_mtxA_set_translate_xyz(0.5f + trans2.x, trans2.y - 0.02f, trans2.z - 0.5f);
    mtxa_scale_xyz(-s2, 1.0f + (s2 + alpha), 1.0f);
    for (i = 0; i < 5; i++)
    {
        mtxa_translate_xyz(0.02f, 0.0f, 0.01f);
        mtxa_rotate_z(-0x400);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha(1.0f - 0.8f * ((float)i / 5.0f));
        model = entries[lbl_17_data_1190[frame & 0xF]].model;
        frame++;
        avdisp_draw_model_unculled_sort_never(model);
    }
    mtxa_pop();
    mtxa_pop();
}

void fn_17_1055C(struct Ball *ball)
{
    Vec pos;
    Quat q;

    pos.x = ball->pos.x;
    pos.y = 0.0f;
    pos.z = ball->pos.z;
    fn_17_FA08(&pos, &ball->vel, &lbl_17_bss_8[ball->playerId]);
    q = ball->ape->rot;
    q.x = 0.0f;
    q.z = 0.0f;
    quat_normalize(&q);
    mtxa_from_quat(&q);
    mtxa_rotate_y(0x4000);
    mathutil_mtxA_set_translate(&pos);
    fn_17_10170(&pos, &ball->vel, g_font_char_var_3);
}
