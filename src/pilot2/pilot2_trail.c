/* Monkey Target 2 trails: smoke from the ape and spray on the water surface. */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "mathutil.h"
#include "pilot2.h"
#include "rand.h"

struct TrailParticle
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ float scale;
    /*0x1C*/ float alpha;
    /*0x20*/ float life;
};  /* size = 0x24 */

struct Trail
{
    /*0x00*/ u32 count;
    /*0x04*/ float accum;
    /*0x08*/ struct TrailParticle parts[1];
};

struct TrailVertex
{
    /*0x00*/ Vec pos;
    /*0x0C*/ GXColor color;
};

extern struct GMA *minigameGma[];
extern u32 g_some_other_flags;
extern u32 g_font_char_var_3;
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void fn_1_E2FD0(int width);
void fn_1_E2FE0(int a, int b, int c, int d);
void fn_1_E326C(struct TrailVertex *a, struct TrailVertex *b);
void mtxa_rotate_z_sin_cos(float sin, float cos);

void fn_13_F2F8(struct Trail *trail, u32 count);
void fn_13_F340(Vec *pos, Vec *vel, struct Trail *trail);
void fn_13_F88C(Vec *pos, Vec *vel, struct Trail *trail);
void fn_13_FFF4(Vec *pos, Vec *vel, int frame);

int lbl_13_data_1230[] =
{
    0x83, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91,
    0x92, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A,
};

Vec lbl_13_bss_338;
u32 lbl_13_bss_344;

void fn_13_F288(void)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        fn_13_F2F8((struct Trail *)(lbl_13_bss_4 + i * 0x120C), 0x80);
        fn_13_F2F8((struct Trail *)(lbl_13_bss_8 + i * 0x480C), 0x200);
    }
}

void fn_13_F2F8(struct Trail *trail, u32 count)
{
    struct TrailParticle *parts = trail->parts;
    u32 i;

    trail->count = count;
    trail->accum = 0.0f;
    for (i = 0; i < count; i++)
        parts[i].life = 0.0f;
}

static inline void fn_update_smoke(struct TrailParticle *parts, u32 i)
{
    parts[i].pos.x += parts[i].vel.x;
    parts[i].pos.y += parts[i].vel.y;
    parts[i].pos.z += parts[i].vel.z;
    parts[i].vel.x *= 0.98f;
    parts[i].vel.z *= 0.98f;
    if (parts[i].vel.y < -0.1f)
        parts[i].vel.y *= 0.3f;
    else
        parts[i].vel.y += 0.001f;
    parts[i].scale *= 1.01f;
    if (parts[i].life < 60.0f)
        parts[i].alpha -= 0.01666666753590107f;
    if (parts[i].pos.y > 0.0f)
        parts[i].life = 0.0f;
    else
        parts[i].life -= 1.0f;
}

void fn_13_F340(Vec *pos, Vec *vel, struct Trail *arg)
{
    struct GMAModel *model;
    u32 i;
    struct TrailParticle *parts = arg->parts;
    struct Trail *trail = arg;
    float dead1 = 0.98f;
    float dead2 = -0.1f;
    float dead3 = 0.3f;
    float dead4 = 0.001f;
    float dead5 = 1.01f;
    float dead6 = 60.0f;
    float dead7 = 0.01666666753590107f;
    float dead8 = 1.0f;
    float depth;
    float scale;

    if (pos->y > 0.0f)
        depth = 0.0f;
    else
    {
        depth = (100.0f + pos->y) / 100.0f;
        if (depth < 0.0f)
            depth = 0.0f;
    }
    if ((g_some_other_flags & 0xA) == 0)
        trail->accum += 1.5f * depth;
    scale = 0.15f * depth;
    for (i = 0; i < trail->count; i++)
    {
        if (parts[i].life > 0.0f)
        {
            model = minigameGma[0]->modelEntries[2].model;
            if ((g_some_other_flags & 0xA) == 0)
            {
                fn_update_smoke(parts, i);
            }
            mtxa_from_mtxb_translate(&parts[i].pos);
            mtxa_sq_from_identity();
            mtxa_rotate_x(0x4000);
            mtxa_scale_s(parts[i].scale);
            avdisp_set_alpha(parts[i].alpha);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
        else if ((g_some_other_flags & 0xA) == 0 && trail->accum > 1.0f)
        {
            parts[i].life = 60.0f + 60.0f * (0.5f * ((float)rand() / 32767.0f));
            parts[i].scale = scale * (0.2f + ((float)rand() / 32767.0f));
            parts[i].pos = *pos;
            parts[i].vel.x = 0.9f * vel->x;
            parts[i].vel.y = vel->y;
            parts[i].vel.z = 0.9f * vel->z;
            parts[i].vel.x += 0.2f * (((float)rand() / 32767.0f) - 0.5f);
            parts[i].vel.z += 0.2f * (((float)rand() / 32767.0f) - 0.5f);
            parts[i].alpha = 1.0f;
            trail->accum -= 1.0f;
        }
    }
}

void fn_13_F7E8(struct Ball *ball)
{
    float dead1, dead2, dead3, dead4;
    Vec sp8;

    if (ball->pos.y >= 0.0f)
        return;
    mtxa_from_quat(&ball->ape->rot);
    mtxa_tf_point_xyz(0.15f, 0.0f, 0.1f, &sp8);
    PSVECAdd(&sp8, &ball->pos, &sp8);
    fn_13_F340(&sp8, &ball->vel, (struct Trail *)(lbl_13_bss_4 + ball->playerId * 0x120C));
    dead1 = 230.0f;
    dead2 = 0.05f;
    dead3 = 0.999f;
    dead4 = 0.01f;
}

static inline void fn_draw_spray(struct TrailParticle *parts, u32 i)
{
    struct TrailVertex start;
    struct TrailVertex end;
    Vec back;

    mtxa_push();
    if ((g_some_other_flags & 0xA) == 0)
    {
        parts[i].pos.x += parts[i].vel.x - -(0.1f * lbl_13_bss_338.x);
        parts[i].pos.y += parts[i].vel.y - -(0.1f * lbl_13_bss_338.y);
        parts[i].pos.z += parts[i].vel.z - -(0.1f * lbl_13_bss_338.z);
    }
    start.color.r = start.color.g = start.color.b = 0xFF;
    start.color.a = 230.0f * parts[i].alpha;
    start.pos = parts[i].pos;
    end.color.r = end.color.g = end.color.b = 0xFF;
    end.color.a = 0;
    vec_set_len(0.05f, &parts[i].vel, &back);
    end.pos.x = parts[i].pos.x - back.x;
    end.pos.y = parts[i].pos.y - back.y;
    end.pos.z = parts[i].pos.z - back.z;
    if ((g_some_other_flags & 0xA) == 0)
    {
        parts[i].vel.x *= 0.999f;
        parts[i].vel.z *= 0.999f;
        parts[i].vel.y -= 0.01f;
        parts[i].alpha *= 0.9f;
        if (parts[i].pos.y < 0.0f)
            parts[i].life = 0.0f;
        else
            parts[i].life -= 1.0f;
    }
    mtxa_from_mtxb();
    fn_1_E326C(&start, &end);
    mtxa_pop();
}

void fn_13_F88C(Vec *pos, Vec *vel, struct Trail *arg)
{
    u32 i;
    struct TrailParticle *parts = arg->parts;
    struct Trail *trail = arg;
    Vec up = {0.0f, 0.0f, -1.0f};
    int n = 0;
    Vec dir;
    Quat quat;
    Vec spawn;
    float t;
    float len;
    int side;

    dir = *vel;
    vec_normalize_len(&dir);
    quat_from_dirs(&quat, &up, &dir);
    mtxa_from_quat(&quat);
    mtxa_normalize_basis();
    len = 7.2f * mathutil_vec_len(vel);
    trail->accum += len;
    fn_1_E2FD0(20);
    fn_1_E2FE0(1, 2, 1, 15);
    for (i = 0; i < trail->count; i++)
    {
        if (parts[i].life > 0.0f)
        {
            fn_draw_spray(parts, i);
            n++;
        }
        else if (trail->accum > 1.0f)
        {
            t = ((float)rand() / 32767.0f) - 0.5f;
            if ((g_some_other_flags & 0xA) == 0)
                parts[i].life = 60.0f + 1.5f * (60.0f * ((float)rand() / 32767.0f));
            parts[i].scale = 0.2f;
            side = ((float)rand() / 32767.0f) > 0.2f;
            if (side)
            {
                if (t < 0.0f)
                    t -= 0.4f;
                else
                    t += 0.4f;
                spawn.x = t;
                spawn.y = 0.0f;
                spawn.z = 1.8f * (((float)rand() / 32767.0f) - 0.1f);
            }
            else
            {
                spawn.x = ((float)rand() / 32767.0f) - 0.5f;
                spawn.y = 0.0f;
                spawn.z = 0.4f * t - 0.7f;
            }
            mtxa_tf_vec(&spawn, &spawn);
            parts[i].pos.x = pos->x + spawn.x;
            parts[i].pos.y = 0.0f;
            parts[i].pos.z = pos->z + spawn.z;
            if (side)
            {
                parts[i].vel.x = t * (0.04f * ((float)rand() / 32767.0f));
                parts[i].vel.y = 0.25f * ((float)rand() / 32767.0f);
                parts[i].vel.z = -1.0f * mathutil_vec_len(vel);
            }
            else
            {
                parts[i].vel.x = 0.2f * ((float)rand() / 32767.0f) - 0.1f;
                parts[i].vel.y = 0.18f * ((float)rand() / 32767.0f);
                parts[i].vel.z = mathutil_vec_len(vel) * (-0.5f * ((float)rand() / 32767.0f) - 0.7f);
            }
            mtxa_tf_vec(&parts[i].vel, &parts[i].vel);
            parts[i].alpha = 1.0f;
            trail->accum -= 1.0f;
        }
    }
}

void fn_13_FFF4(Vec *pos, Vec *vel, int frame)
{
    Mtx mtx;
    Vec dir1;
    Vec translate1;
    Vec dir2;
    Vec translate2;
    struct GMAModelEntry *entries = minigameGma[0]->modelEntries;
    float alpha;
    float z;
    float width;
    float width2;
    int i;
    struct GMAModel *model;

    mtxa_push();
    mtxa_to_mtx(mtx);
    mtxa_from_mtxb();
    mtxa_mult_right(mtx);
    alpha = 216.0f * mathutil_vec_len(vel);
    alpha /= 120.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    avdisp_set_alpha(alpha);
    mtxa_push();
    mathutil_mtxA_get_translate(&translate1);
    mtxa_tf_vec_xyz(1.0f, 0.0f, 0.0f, &dir1);
    z = dir1.z;
    dir1.x = math_sqrt(dir1.x * dir1.x + dir1.z * dir1.z);
    dir1.z = 0.0f;
    vec_normalize_len(&dir1);
    mtxa_sq_from_identity();
    mtxa_rotate_z_sin_cos(dir1.y, dir1.x);
    width = 3.0 * fabs(z);
    mathutil_mtxA_set_translate_xyz(translate1.x - 0.5f, translate1.y - 0.02f, translate1.z - 0.5f);
    mtxa_scale_xyz(width, 1.0f + (width + alpha), 1.0f);
    for (i = 0; i < 5; i++)
    {
        mtxa_translate_xyz(0.02f, 0.0f, 0.01f);
        mtxa_rotate_z(-0x400);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha(1.0f - 0.8f * ((float)i / 5.0f));
        model = entries[lbl_13_data_1230[frame & 0xF]].model;
        frame++;
        avdisp_draw_model_unculled_sort_never(model);
    }
    mtxa_pop();
    mtxa_push();
    mathutil_mtxA_get_translate(&translate2);
    mtxa_tf_vec_xyz(-1.0f, 0.0f, 0.0f, &dir2);
    z = dir2.z;
    dir2.x = math_sqrt(dir2.x * dir2.x + dir2.z * dir2.z);
    dir2.z = 0.0f;
    vec_normalize_len(&dir2);
    mtxa_sq_from_identity();
    mtxa_rotate_z_sin_cos(dir2.y, dir2.x);
    width2 = 3.0 * fabs(z);
    mathutil_mtxA_set_translate_xyz(0.5f + translate2.x, translate2.y - 0.02f, translate2.z - 0.5f);
    mtxa_scale_xyz(-width2, 1.0f + (width2 + alpha), 1.0f);
    for (i = 0; i < 5; i++)
    {
        mtxa_translate_xyz(0.02f, 0.0f, 0.01f);
        mtxa_rotate_z(-0x400);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha(1.0f - 0.8f * ((float)i / 5.0f));
        model = entries[lbl_13_data_1230[frame & 0xF]].model;
        frame++;
        avdisp_draw_model_unculled_sort_never(model);
    }
    mtxa_pop();
    mtxa_pop();
}

void fn_13_103E0(struct Ball *ball)
{
    Vec pos;
    Quat quat;

    pos.x = ball->pos.x;
    pos.y = 0.0f;
    pos.z = ball->pos.z;
    fn_13_F88C(&pos, &ball->vel, (struct Trail *)(lbl_13_bss_8 + ball->playerId * 0x480C));
    quat = ball->ape->rot;
    quat.x = 0.0f;
    quat.z = 0.0f;
    quat_normalize(&quat);
    mtxa_from_quat(&quat);
    mtxa_rotate_y(0x4000);
    mathutil_mtxA_set_translate(&pos);
    fn_13_FFF4(&pos, &ball->vel, g_font_char_var_3);
}
