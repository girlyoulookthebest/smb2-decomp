/* Bowser 2 "author" background: AUT_PIPE_ models whose skin bones hang as rope chains. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "frustum.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"

struct BgBow2AuthorBone
{
    /*0x00*/ u32 flags;  /* 1: not linked to a parent, 2: chain end, 4: collides with the stage */
    /*0x04*/ Vec pos;
    /*0x10*/ Vec vel;
    /*0x1C*/ Vec dir;
    /*0x28*/ float len;
    /*0x2C*/ Vec parentDir;
    /*0x38*/ Vec childDir;
    /*0x44*/ float parentLen;
    /*0x48*/ float childLen;
    /*0x4C*/ struct BgBow2AuthorBone *parent;
    /*0x50*/ struct BgBow2AuthorBone *child;
    /*0x54*/ Mtx mtx;
};  /* size = 0x84 */

struct BgBow2AuthorPipe
{
    /*0x00*/ struct StageBgModel *bg;
    /*0x04*/ s32 boneCount;
    /*0x08*/ struct BgBow2AuthorBone *bones;
    /*0x0C*/ s32 rotZ;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 timer;
};  /* size = 0x18 */

struct BgBow2AuthorWork
{
    /*0x00*/ s32 pipeCount;
    /*0x04*/ struct BgBow2AuthorPipe pipes[16];
};

struct BgBow2AuthorState
{
    s32 paused;
    s32 reset;
};

extern void *g_some_buffer_ptr;
extern void *bg_heap;

void *OSFreeToHeap(void *heap, void *ptr);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void g_something_with_locked_cache_2(void *dst, int value, u32 size);
void draw_poly(struct GMAModel *model, Mtx **mtxs);
void pointer_range_advance(struct GMAModel *model, void *mtxs);
s16 *fn_80069964(struct GMAModel *model);
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *dst);
void fn_1_15F984(struct BgBow2AuthorState *out);

static struct BgModelSearch s_bgModelSearch[] =
{
    { BG_MDL_CMP_PREFIX, "AUT_PIPE_" },
    { BG_MDL_CMP_END,    NULL },
};

void fn_1_A1D28(struct BgBow2AuthorWork *work);
void fn_1_A1DD8(struct BgBow2AuthorWork *work);
void fn_1_A1E28(struct BgBow2AuthorWork *work);
void fn_1_A1E78(struct BgBow2AuthorWork *work, u32 mask);
void fn_1_A1F64(struct BgBow2AuthorPipe *pipe);
void fn_1_A2338(struct BgBow2AuthorPipe *pipe);
void fn_1_A2998(struct BgBow2AuthorBone *bone);
void fn_1_A2CD0(struct BgBow2AuthorPipe *pipe);
void fn_1_A2D28(struct BgBow2AuthorPipe *pipe);
void fn_1_A2E9C(struct BgBow2AuthorPipe *pipe);
int fn_1_A307C(int, struct StageBgModel *);

void bg_bow2_author_init(void)
{
    struct BgBow2AuthorWork *work = (void *)world_theme.work;

    bg_init_base();
    work->pipeCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, s_bgModelSearch, fn_1_A307C);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, s_bgModelSearch, fn_1_A307C);
    fn_1_A1D28(work);
}

void bg_bow2_author_tick(void)
{
    struct BgBow2AuthorWork *work = (void *)world_theme.work;

    g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
        fn_1_A1DD8(work);
}

void bg_bow2_author_dest(void)
{
    fn_1_A1E28((void *)world_theme.work);
}

void bg_bow2_author_disp(void)
{
    struct BgBow2AuthorWork *work = (void *)world_theme.work;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();

    mtxa_from_mtxb();
    g_set_active_light_group(world_theme.unkA8);
    fn_1_A1E78(work, mask);
    bg_disp_base();
}

void bg_bow2_author_item_coin_coli(int unused) {}

void fn_1_A1D28(struct BgBow2AuthorWork *work)
{
    int n;
    struct BgBow2AuthorPipe *pipe;

    pipe = work->pipes;
    for (n = work->pipeCount; n > 0; n--, pipe++)
    {
        fn_1_A1F64(pipe);
        pipe->timer = 60.0f * (1.0f + 2.0f * RAND_FLOAT());
    }
}

void fn_1_A1DD8(struct BgBow2AuthorWork *work)
{
    int n;
    struct BgBow2AuthorPipe *pipe;

    pipe = work->pipes;
    for (n = work->pipeCount; n > 0; n--, pipe++)
        fn_1_A2338(pipe);
}

void fn_1_A1E28(struct BgBow2AuthorWork *work)
{
    int n;
    struct BgBow2AuthorPipe *pipe;

    pipe = work->pipes;
    for (n = work->pipeCount; n > 0; n--, pipe++)
        fn_1_A2CD0(pipe);
}

void fn_1_A1E78(struct BgBow2AuthorWork *work, u32 mask)
{
    int n;
    struct BgBow2AuthorPipe *pipe;
    struct GMAModel *model;
    struct StageBgModel *bg;

    pipe = work->pipes;
    for (n = work->pipeCount; n > 0; n--, pipe++)
    {
        bg = pipe->bg;
        if (!(bg->flags & mask))
            continue;
        model = bg->model;
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&bg->pos);
        mtxa_rotate_z(bg->rotZ);
        mtxa_rotate_y(bg->rotY);
        mtxa_rotate_x(bg->rotX);
        mtxa_rotate_z(pipe->rotZ);
        mtxa_scale(&bg->scale);
        if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, bg->scale.x))
        {
            mtxa_push();
            fn_1_A2D28(pipe);
            mtxa_pop();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
}

/* Build the bone chain from the model's skin matrices. */
void fn_1_A1F64(struct BgBow2AuthorPipe *pipe)
{
    int n;
    struct BgBow2AuthorBone *bone;
    Mtx *mtx;
    Mtx *mtxs;
    struct BgBow2AuthorBone *parent;
    int count;
    struct GMAModel *model;
    s16 *parents;
    struct BgBow2AuthorBone *child;
    Mtx **ptrs;
    Mtx **buf;

    model = pipe->bg->model;
    count = model->mtxCount;
    if (count == 0)
        return;
    pipe->bones = alloc_from_heap_or_panic(bg_heap, count * sizeof(struct BgBow2AuthorBone), "bg_bow2_au.c", 0x13A);
    g_something_with_locked_cache_2(pipe->bones, 0, count * sizeof(struct BgBow2AuthorBone));
    pipe->boneCount = count;
    buf = g_some_buffer_ptr;
    mtxs = (Mtx *)(buf + count);
    ptrs = buf;
    mtx = mtxs;
    for (n = count; n > 0; n--, ptrs++, mtx++)
        *ptrs = mtx;
    draw_poly(model, buf);

    mtx = mtxs;
    bone = pipe->bones;
    for (n = count; n > 0; n--, bone++, mtx++)
    {
        mtxa_from_mtx(*mtx);
        mathutil_mtxA_get_translate(&bone->pos);
        bone->flags = 1;
        bone->child = NULL;
        bone->parent = NULL;
        mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
        mtxa_to_mtx(bone->mtx);
    }

    parents = fn_80069964(model);
    bone = pipe->bones;
    for (n = count; n > 0; n--, bone++, parents++)
    {
        if (*parents >= 0)
        {
            parent = &pipe->bones[*parents];
            bone->parent = parent;
            bone->parentLen = mathutil_vec_distance(&parent->pos, &bone->pos);
            bone->parentDir.x = bone->pos.x - parent->pos.x;
            bone->parentDir.y = bone->pos.y - parent->pos.y;
            bone->parentDir.z = bone->pos.z - parent->pos.z;
            vec_normalize_len(&bone->parentDir);
            if (bone->parentLen > 0.0f)
                bone->flags &= ~1;
            parent->child = bone;
            parent->childDir = bone->parentDir;
            parent->childLen = bone->parentLen;
        }
    }

    /* Skip zero-length links so each bone's child is the next bone that actually moves. */
    bone = pipe->bones;
    for (n = count; n > 0; n--, bone++)
    {
        child = bone->child;
        if (child == NULL)
            bone->flags |= 2;
        else if (bone->childLen == 0.0f)
        {
            child = child->child;
            while (child->parentLen == 0.0f)
                child = child->child;
            bone->child = child;
            if (child == NULL)
                bone->flags |= 2;
            else
            {
                bone->childDir.x = child->pos.x - bone->pos.x;
                bone->childDir.y = child->pos.y - bone->pos.y;
                bone->childDir.z = child->pos.z - bone->pos.z;
                bone->childLen = vec_normalize_len(&bone->childDir);
            }
        }
    }

    bone = pipe->bones;
    for (n = count; n > 0; n--, bone++)
    {
        parent = bone->child;
        if (parent == NULL)
        {
            bone->dir = bone->childDir;
            bone->len = bone->childLen;
        }
        else
        {
            bone->dir = bone->parentDir;
            bone->len = bone->parentLen;
        }
        if (n == count >> 1 || (bone->flags & 2))
            bone->flags |= 4;
    }
}

/* Rope physics: gravity, stiffness toward the parent's direction, length constraint, stage collision. */
void fn_1_A2338(struct BgBow2AuthorPipe *pipe)
{
    struct StageBgModel *bg;
    int paused;
    int n;
    struct BgBow2AuthorBone *bone;
    struct BgBow2AuthorBone *parent;
    struct RaycastHit hit;
    Vec grav;
    Vec dir;
    Vec newPos;
    Vec delta;
    Vec wpos;
    Vec wvel;
    struct BgBow2AuthorState state;
    float f;
    float len;

    bg = pipe->bg;
    if (pipe->timer > 0)
        pipe->timer--;
    if (!(modeCtrl.courseFlags & (1 << 26)))
        paused = 0;
    else
    {
        fn_1_15F984(&state);
        paused = state.paused;
        if (state.reset)
        {
            fn_1_A2E9C(pipe);
            pipe->timer = 0;
        }
    }
    if (paused)
        return;

    mtxa_from_translate(&bg->pos);
    mtxa_rotate_z(bg->rotZ);
    mtxa_rotate_y(bg->rotY);
    mtxa_rotate_x(bg->rotX);
    mtxa_rotate_z(pipe->rotZ);
    mtxa_rigid_inv_tf_vec_xyz(0.0f, -0.016f, 0.0f, &grav);
    bone = pipe->bones;
    for (n = pipe->boneCount; n > 0; n--, bone++)
    {
        if (bone->flags & 1)
            continue;
        parent = bone->parent;
        bone->vel.x += grav.x;
        bone->vel.y += grav.y;
        bone->vel.z += grav.z;
        if (parent != NULL)
        {
            dir = parent->dir;
            delta.x = bone->pos.x - parent->pos.x;
            delta.y = bone->pos.y - parent->pos.y;
            delta.z = bone->pos.z - parent->pos.z;
            vec_normalize_len(&delta);
            f = 0.9f + 0.1f * (0.5f * (1.0f + mathutil_vec_dot_prod(&delta, &dir)));
            bone->vel.x *= f;
            bone->vel.y *= f;
            bone->vel.z *= f;
        }
        if ((bone->flags & 2) && pipe->timer > 0)
        {
            dir.x = bone->pos.x - parent->pos.x;
            dir.y = bone->pos.y - parent->pos.y;
            dir.z = bone->pos.z - parent->pos.z;
            vec_normalize_len(&dir);
            f = -0.064f * (1.0f + (float)pipe->timer / 180.0f);
            bone->vel.x += dir.x * f;
            bone->vel.y += dir.y * f;
            bone->vel.z += dir.z * f;
            fn_1_A2998(bone);
        }
        bone->vel.x *= 0.98f;
        bone->vel.y *= 0.98f;
        bone->vel.z *= 0.98f;
        bone->pos.x += bone->vel.x;
        bone->pos.y += bone->vel.y;
        bone->pos.z += bone->vel.z;
        if (parent != NULL)
        {
            dir.x = bone->pos.x - parent->pos.x;
            dir.y = bone->pos.y - parent->pos.y;
            dir.z = bone->pos.z - parent->pos.z;
            len = vec_normalize_len(&dir);
            f = bone->parentLen;
            newPos.x = parent->pos.x + dir.x * f;
            newPos.y = parent->pos.y + dir.y * f;
            newPos.z = parent->pos.z + dir.z * f;
            bone->dir = dir;
            bone->len = len;
            delta.x = newPos.x - bone->pos.x;
            delta.y = newPos.y - bone->pos.y;
            delta.z = newPos.z - bone->pos.z;
            bone->vel.x += delta.x;
            bone->vel.y += delta.y;
            bone->vel.z += delta.z;
            parent->vel.x -= delta.x;
            parent->vel.y -= delta.y;
            parent->vel.z -= delta.z;
            bone->pos = newPos;
        }
        if (bone->flags & 4)
        {
            mtxa_tf_point(&bone->pos, &wpos);
            mtxa_tf_vec(&bone->vel, &wvel);
            mtxa_push();
            raycast_stage_down(&wpos, &hit, NULL);
            mtxa_pop();
            if (hit.flags & 1)
            {
                delta.x = wpos.x - hit.pos.x;
                delta.y = wpos.y - hit.pos.y;
                delta.z = wpos.z - hit.pos.z;
                f = mathutil_vec_dot_prod(&delta, &hit.normal);
                if (f < 0.5f)
                {
                    f -= 0.5f;
                    wpos.x -= hit.normal.x * f;
                    wpos.y -= hit.normal.y * f;
                    wpos.z -= hit.normal.z * f;
                    f = mathutil_vec_dot_prod(&wvel, &hit.normal);
                    if (f < 0.0f)
                    {
                        f *= -(1.5f + 0.2f * RAND_FLOAT());
                        wvel.x += f * hit.normal.x;
                        wvel.y += f * hit.normal.y;
                        wvel.z += f * hit.normal.z;
                    }
                    mtxa_rigid_inv_tf_point(&wpos, &bone->pos);
                    mtxa_rigid_inv_tf_vec(&wvel, &bone->vel);
                }
            }
        }
    }
}

/* Spawn a spark effect flying off a bone. */
void fn_1_A2998(struct BgBow2AuthorBone *bone)
{
    struct Effect effect;
    Vec vel;
    float f;

    mtxa_push();
    memset(&effect, 0, sizeof(effect));
    effect.type = 0x38;
    effect.timer = 60.0f * (0.5f + 0.25f * RAND_FLOAT());
    mtxa_tf_point(&bone->pos, &effect.pos);
    mtxa_tf_vec(&bone->dir, &effect.vel);
    f = 0.2f * (1.0f + 0.3f * RAND_FLOAT());
    effect.vel.x *= f;
    effect.vel.y *= f;
    effect.vel.z *= f;
    mtxa_tf_vec(&bone->vel, &vel);
    effect.vel.x += 0.5f * vel.x;
    effect.vel.y += 0.5f * vel.y;
    effect.vel.z += 0.5f * vel.z;
    f = 0.05f * mathutil_vec_len(&effect.vel);
    effect.vel.x += f * (RAND_FLOAT() - 0.5);
    effect.vel.y += f * (RAND_FLOAT() - 0.5);
    effect.vel.z += f * (RAND_FLOAT() - 0.5);
    effect.scale.x = 0.5f * (1.0f + 0.5f * RAND_FLOAT());
    effect.scale.y = effect.scale.x * ((1.0f + RAND_FLOAT()) / 30.0f);
    effect.unk18 = 1.0f;
    effect.unk1C = 1.2f;
    effect.unk20 = 1.4f;
    effect.rotZ = (rand() & 0x7FFF) - 0x7FFF;
    spawn_effect(&effect);
    mtxa_pop();
}

void fn_1_A2CD0(struct BgBow2AuthorPipe *pipe)
{
    if (pipe->bones != NULL)
    {
        OSFreeToHeap(bg_heap, pipe->bones);
        pipe->bones = NULL;
        pipe->boneCount = 0;
    }
}

/* Write the bone matrices for skinning. */
void fn_1_A2D28(struct BgBow2AuthorPipe *pipe)
{
    struct GMAModel *model = pipe->bg->model;
    struct BgBow2AuthorBone *bone;
    Mtx *mtxs;
    Mtx *mtx;
    int n;
    struct BgBow2AuthorBone *child;
    Vec d;
    Quat q;
    float len;

    if (pipe->boneCount != 0)
    {
        bone = pipe->bones;
        mtxs = g_some_buffer_ptr;
        mtx = mtxs;
        for (n = pipe->boneCount; n > 0; n--, mtx++, bone++)
        {
            child = bone->child;
            if (!(bone->flags & 2))
            {
                d.x = child->pos.x - bone->pos.x;
                d.y = child->pos.y - bone->pos.y;
                d.z = child->pos.z - bone->pos.z;
                len = vec_normalize_len(&d);
            }
            if (bone->flags & 3)
                mtxa_from_mtx(bone->mtx);
            else
            {
                quat_from_dirs(&q, &bone->childDir, &d);
                mtxa_from_quat(&q);
                mtxa_mult_right(bone->mtx);
            }
            if (!(bone->flags & 2))
                mtxa_scale_xyz(len / bone->childLen, 1.0f, 1.0f);
            mathutil_mtxA_set_translate(&bone->pos);
            mtxa_to_mtx(*mtx);
        }
        pointer_range_advance(model, mtxs);
    }
}

/* Reset: straighten the chain along gravity and jitter the velocities. */
void fn_1_A2E9C(struct BgBow2AuthorPipe *pipe)
{
    int n;
    struct BgBow2AuthorBone *bone;
    struct StageBgModel *bg;
    struct BgBow2AuthorBone *parent;
    Vec d;
    Vec down;
    float len;

    bg = pipe->bg;
    mtxa_from_translate(&bg->pos);
    mtxa_rotate_z(bg->rotZ);
    mtxa_rotate_y(bg->rotY);
    mtxa_rotate_x(bg->rotX);
    mtxa_rotate_z(pipe->rotZ);
    mtxa_rigid_inv_tf_vec_xyz(0.0f, -1.0f, 0.0f, &down);
    bone = pipe->bones;
    for (n = pipe->boneCount; n > 0; n--, bone++)
    {
        if (bone->flags & 1)
            continue;
        parent = bone->parent;
        if (parent != NULL)
        {
            d = down;
            len = bone->parentLen;
            bone->pos.x = parent->pos.x + d.x * len;
            bone->pos.y = parent->pos.y + d.y * len;
            bone->pos.z = parent->pos.z + d.z * len;
        }
        bone->vel.x += 0.1f * (RAND_FLOAT() - 0.5f);
        bone->vel.y += 0.1f * (RAND_FLOAT() - 0.5f);
        bone->vel.z += 0.1f * (RAND_FLOAT() - 0.5f);
    }
}

int fn_1_A307C(int index, struct StageBgModel *bg)
{
    struct BgBow2AuthorWork *work = (void *)world_theme.work;

    switch (index)
    {
    case 0:
        if (work->pipeCount < 16)
        {
            work->pipes[work->pipeCount].bg = bg;
            work->pipeCount++;
        }
        break;
    }
    return 1;
}
