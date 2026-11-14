/* Goal tapes and goal bags (SMB1 stobj_goal.c). */
#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "effect.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "pool.h"
#include "rand.h"
#include "recplay.h"
#include "stage.h"
#include "stcoli.h"
#include "stobj.h"
#include "world.h"

float fn_1_629C4(int id, float time);
void debug_textdraw_set_pos(int x, int y);
int debug_textdraw_vsprintf_buf_1(char *fmt, ...);
int spawn_stobj(struct Stobj *template);
extern void *g_some_buffer_ptr;
extern struct NlObj *g_init_common_gma_buf;

struct GoalTape_sub
{
    Vec unk0;
    Vec unkC;
    float unk18;
    Vec unk1C;
    u32 unk28;
    float unk2C;
};  /* size = 0x30 */

struct GoalTape
{
    s32 u_flags;
    float unk4;
    float unk8;
    float unkC;
    s32 unk10;
    struct Stobj *unk14;
    struct GoalTape_sub unk18[8];
};  /* size = 0x198 */

/* The "party ball", known as a "goal bag" internally */
struct GoalBag
{
    s32 u_flags;
    /*0x04*/ float openness;
    float unk8;
    /*0x0C*/ struct Stobj *stobj;
    /*0x10*/ struct StageGoal goal;
    s32 unk24;
};  /* size = 0x28 */

struct GoalTape goaltapes[0x10];
struct GoalBag goalbags[0x10];

s16 goal_LCD_small_common_gmatpl_idxes[] =
{
    0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
};

s16 goal_LCD_large_common_gmatpl_idxes[] =
{
    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
};

void fn_1_ABE84(struct GoalTape_sub *arg0, int faceCount, struct NlModel *model1, struct NlModel *model2);
void fn_1_AE074(struct GoalTape *tape);
float g_get_sphere_camera_zdist_clamped(Vec *pos, float radius);
void g_something_with_translating_items(int, Vec *, float);
void call_SoundReqID_arg_0(int soundId);
void fn_1_375A4(void);
void fn_1_ADB14(int goalId, struct PhysicsBall *ball);
void fn_1_ADF3C(int arg0);

void load_goaltape_and_goalbag_stobjs(struct StageCollHeader *group, int count)
{
    Vec sp8;
    struct Stobj stobj;
    struct StageCollHeader *stageAg;
    int i;
    int j;
    int totalGoals;
    struct StageGoal *goal;
    struct GoalTape *tape;
    struct GoalBag *bag;

    /* Spawn goal tapes */
    memset(goaltapes, 0, sizeof(goaltapes));
    memset(&stobj, 0, sizeof(stobj));

    stobj.type = 2;

    totalGoals = 0;
    tape = goaltapes;
    stageAg = group;
    for (i = 0; i < count; i++, stageAg++)
    {
        goal = stageAg->goals;
        for (j = 0; j < stageAg->goalCount; j++, goal++, tape++)
        {
            if (totalGoals >= 0x10)
            {
                debug_textdraw_set_pos(16, 16);
                debug_textdraw_vsprintf_buf_1("Warning!!! Goal Tape Max(%d) Over!!!\n", 0x10);
                break;
            }
            stobj.u_some_pos = goal->pos;
            stobj.rotX = goal->rotX;
            stobj.rotY = goal->rotY;
            stobj.rotZ = goal->rotZ;
            stobj.animGroupId = i;
            tape->u_flags = 0;
            stobj.extraData = tape;
            spawn_stobj(&stobj);
            totalGoals++;
        }
    }

    /* Spawn goal bags */
    memset(goalbags, 0, sizeof(goalbags));
    memset(&stobj, 0, sizeof(stobj));

    if (modeCtrl.courseFlags & 0x10)
        stobj.type = 4;
    else
        stobj.type = 3;

    sp8.x = 0.0f;
    sp8.y = 2.8f;
    sp8.z = 0.0f;

    totalGoals = 0;
    bag = goalbags;
    stageAg = group;
    for (i = 0; i < count; i++, stageAg++)
    {
        goal = stageAg->goals;
        for (j = 0; j < stageAg->goalCount; j++, goal++, bag++)
        {
            if (totalGoals >= 0x10)
                break;
            mtxa_from_translate(&goal->pos);
            mtxa_rotate_z(goal->rotZ);
            mtxa_rotate_y(goal->rotY);
            mtxa_rotate_x(goal->rotX);
            mtxa_tf_point(&sp8, &stobj.unkA8);
            stobj.rotX = goal->rotX;
            stobj.rotY = goal->rotY;
            stobj.rotZ = goal->rotZ;
            stobj.animGroupId = i;
            bag->u_flags = 0;
            bag->goal = *goal;
            stobj.extraData = bag;
            spawn_stobj(&stobj);
            totalGoals++;
        }
    }
}

struct GMAModel *goal_LCD_small_numbers[10];
struct GMAModel *goal_LCD_large_numbers[10];

void stobj_goaltape_init(struct Stobj *stobj)
{
    struct RaycastHit hit;
    Vec sp8;
    struct GoalTape *tape;
    struct GoalTape_sub *sub;
    int i;
    float f;
    float y;
    s16 *idx;
    struct GMAModel **model;

    stobj->state = 0;
    stobj->unk8 |= 2;
    stobj->model = (struct GMAModel *)g_init_common_gma_buf->models[3];
    stobj->boundSphereRadius = 1.3125f;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    tape = stobj->extraData;
    tape->unk14 = stobj;
    tape->unk10 = -1;
    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_tf_point(&stobj->u_model_origin, &sp8);
    mtxa_push();
    raycast_stage_down(&sp8, &hit, NULL);
    mtxa_pop();
    if (hit.flags & 1)
    {
        mtxa_rigid_inv_tf_point(&hit.pos, &sp8);
        if (sp8.y < stobj->u_model_origin.y)
            tape->unk4 = 0.002 + sp8.y;
        else
            tape->unk4 = 0.002f;
    }
    else
        tape->unk4 = 0.002f;
    y = stobj->u_model_origin.y;
    sub = tape->unk18;
    for (i = 8; i > 0; i--, sub++)
    {
        f = (i - 1) / 7.0f;
        sub->unk0.x = (1.75 * f) - 0.875;
        sub->unk0.y = y;
        sub->unk0.z = 0.0f;
        sub->unk18 = f;
        sub->unkC.x = 0.0f;
        sub->unkC.y = 0.0f;
        sub->unkC.z = 1.0f;
        sub->unk1C.x = 0.0f;
        sub->unk1C.y = 0.0f;
        sub->unk1C.z = 0.0f;
        sub->unk28 = 6;
        sub->unk2C = 0.225f;
    }
    tape->unk18[0].unk28 &= ~2;
    tape->unk18[0].unk28 |= 1;
    tape->unk18[7].unk28 &= ~4;
    tape->unk18[7].unk28 |= 1;
    tape->unk8 = y;
    tape->unkC = y;

    idx = goal_LCD_small_common_gmatpl_idxes;
    model = goal_LCD_small_numbers;
    for (i = 10; i > 0; i--, idx++, model++)
        *model = init_common_gma->modelEntries[*idx].model;

    idx = goal_LCD_large_common_gmatpl_idxes;
    model = goal_LCD_large_numbers;
    for (i = 10; i > 0; i--, idx++, model++)
        *model = init_common_gma->modelEntries[*idx].model;
}

void stobj_goaltape_tick(struct Stobj *stobj)
{
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float sumZ;
    float sumY;
    float sumX;
    int i;
    struct GoalTape *tape;
    struct GoalTape_sub *sub;
    struct GoalTape_sub *other;
    struct ItemGroup *group;
    int animated;
    float len;
    float t;

    tape = stobj->extraData;
    switch (stobj->state)
    {
    case 1:
    case 2:
        switch (main_mode)
        {
        case 2:
            switch (modeCtrl.gameType)
            {
            case 1:
                stobj->state = 3;
                stobj->counter = 90;
                break;
            default:
                stobj->state = 0;
                break;
            }
            break;
        default:
            stobj->state = 0;
            break;
        }
        break;
    case 3:
        stobj->counter--;
        if (stobj->counter < 0)
        {
            if (!(mode_info.flags & 0x20))
                stobj->state = 4;
            else
                stobj->state = 0;
        }
        break;
    case 4:
    case 5:
        stobj->state = 0;
        tape->unk8 = -0.25f;
        tape->unkC = stobj->u_model_origin.y;
        fn_1_AE074(stobj->extraData);
        break;
    }

    if (stobj->animGroupId == 0)
        animated = 0;
    else
    {
        animated = 1;
        group = &itemgroups[stobj->animGroupId];
        mtxa_from_mtx(group->prevTransform);
        mtxa_tf_point(&stobj->u_some_pos, &sp8);
        mtxa_from_mtx(group->transform);
        mtxa_tf_point(&stobj->u_some_pos, &sp20);
        sp8.x -= sp20.x;
        sp8.y -= sp20.y;
        sp8.z -= sp20.z;
        mtxa_rotate_z(stobj->rotZ);
        mtxa_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX);
        mtxa_rigid_inv_tf_vec(&sp8, &sp8);
    }
    mtxa_from_mtx(itemgroups[stobj->animGroupId].transform);
    mtxa_translate(&stobj->u_some_pos);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    sp14.x = 0.004 * g_gravity_dir.x;
    sp14.y = 0.004 * g_gravity_dir.y;
    sp14.z = 0.004 * g_gravity_dir.z;
    mtxa_rigid_inv_tf_vec(&sp14, &sp14);

    sub = tape->unk18;
    for (i = 8; i > 0; i--, sub++)
    {
        if (!(sub->unk28 & 1))
        {
            sumX = 0.0f;
            sumY = 0.0f;
            sumZ = 0.0f;
            if (sub->unk28 & 2)
            {
                other = sub - 1;
                sumX += other->unkC.x;
                sumY += other->unkC.y;
                sumZ += other->unkC.z;
                sp20.x = other->unk0.x - sub->unk0.x;
                sp20.y = other->unk0.y - sub->unk0.y;
                sp20.z = other->unk0.z - sub->unk0.z;
                len = vec_normalize_len(&sp20);
                if (len > other->unk2C)
                {
                    t = 1.05 * (len - other->unk2C);
                    sub->unk1C.x += sp20.x * t;
                    sub->unk1C.y += sp20.y * t;
                    sub->unk1C.z += sp20.z * t;
                }
                else
                {
                    t = -other->unk2C;
                    sub->unk0.x = other->unk0.x + (sp20.x * t);
                    sub->unk0.y = other->unk0.y + (sp20.y * t);
                    sub->unk0.z = other->unk0.z + (sp20.z * t);
                    t = -mathutil_vec_dot_prod(&sub->unk1C, &sp20);
                    sub->unk1C.x += sp20.x * t;
                    sub->unk1C.y += sp20.y * t;
                    sub->unk1C.z += sp20.z * t;
                }
            }
            if (sub->unk28 & 4)
            {
                other = sub + 1;
                sumX += other->unkC.x;
                sumY += other->unkC.y;
                sumZ += other->unkC.z;
                sp20.x = other->unk0.x - sub->unk0.x;
                sp20.y = other->unk0.y - sub->unk0.y;
                sp20.z = other->unk0.z - sub->unk0.z;
                len = vec_normalize_len(&sp20);
                if (len > sub->unk2C)
                {
                    t = 1.05 * (len - sub->unk2C);
                    sub->unk1C.x += sp20.x * t;
                    sub->unk1C.y += sp20.y * t;
                    sub->unk1C.z += sp20.z * t;
                }
                else
                {
                    t = -sub->unk2C;
                    sub->unk0.x = other->unk0.x + (sp20.x * t);
                    sub->unk0.y = other->unk0.y + (sp20.y * t);
                    sub->unk0.z = other->unk0.z + (sp20.z * t);
                    t = -mathutil_vec_dot_prod(&sub->unk1C, &sp20);
                    sub->unk1C.x += sp20.x * t;
                    sub->unk1C.y += sp20.y * t;
                    sub->unk1C.z += sp20.z * t;
                }
            }
            sub->unk1C.x += sp14.x;
            sub->unk1C.y += sp14.y;
            sub->unk1C.z += sp14.z;
            mtxa_from_rotate_y(49152.0 * (sub->unk1C.x + sub->unk1C.z));
            mtxa_rotate_x(98304.0 * sub->unk1C.y);
            mtxa_tf_vec(&sub->unkC, &sub->unkC);
            sub->unkC.x += 0.075 * (sumX - sub->unkC.x);
            sub->unkC.y += 0.075 * (sumY - sub->unkC.y);
            sub->unkC.z += 0.075 * (sumZ - sub->unkC.z);
            vec_normalize_len(&sub->unkC);
        }
    }

    if (tape->unk8 > tape->unkC)
    {
        tape->unk8 -= 0.016666668f;
        if (tape->unk8 < tape->unkC)
            tape->unk8 = tape->unkC;
    }
    else if (tape->unk8 < tape->unkC)
    {
        tape->unk8 += 0.016666668f;
        if (tape->unk8 > tape->unkC)
            tape->unk8 = tape->unkC;
    }

    tape->unk18[0].unk0.y = tape->unk8;
    tape->unk18[7].unk0.y = tape->unk8;

    sub = tape->unk18;
    for (i = 8; i > 0; i--, sub++)
    {
        if (!(sub->unk28 & 1))
        {
            if (animated && ((sub->unk28 & 6) != 6))
            {
                sub->unk1C.x += 0.15000000000000002 * (sp8.x - sub->unk1C.x);
                sub->unk1C.y += 0.15000000000000002 * (sp8.y - sub->unk1C.y);
                sub->unk1C.z += 0.15000000000000002 * (sp8.z - sub->unk1C.z);
            }
            else
            {
                sub->unk1C.x *= 0.85;
                sub->unk1C.y *= 0.85;
                sub->unk1C.z *= 0.85;
            }
            sub->unk0.x += sub->unk1C.x;
            sub->unk0.y += sub->unk1C.y;
            sub->unk0.z += sub->unk1C.z;
            if (sub->unk0.y < tape->unk4)
            {
                sub->unk0.y = tape->unk4;
                sub->unk1C.y = 0.0f;
                sub->unkC.x *= 0.9;
                sub->unkC.y += 0.1 * (1.0 - sub->unkC.y);
                sub->unkC.z *= 0.9;
                vec_normalize_len(&sub->unkC);
            }
        }
    }
}

void stobj_goaltape_disp(struct Stobj *stobj)
{
    Vec sp8;
    int i;
    struct GoalTape_sub *sub;
    struct GoalTape_sub *first;
    int count;
    int id;
    int time;
    int digit;

    mtxa_from_mtxb();
    mtxa_translate(&stobj->u_some_pos);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    g_smth_with_avdisp_globals(0.8f, 0.8f, 0.8f);

    sub = ((struct GoalTape *)stobj->extraData)->unk18;
    for (i = 8; i > 0; i--, sub++)
    {
        first = sub;
        count = 1;
        while (i > 0 && (sub->unk28 & 4) != 0)
        {
            i--;
            sub++;
            count++;
        }
        if (count > 1)
        {
            mtxa_push();
            fn_1_ABE84(first, count, (void *)stobj->model, g_some_buffer_ptr);
            mtxa_pop();
            nl2ngc_draw_model_sort_none(g_some_buffer_ptr);
        }
    }
    apply_curr_light_group_ambient();
    nl2ngc_set_material_color(1.0f, 1.0f, 1.0f);
    id = lbl_1_bss_71EC8.unk14;
    if (g_poolInfo.playerPool.statusList[id] == 2 && (balls[id].flags & 0x10000000000ULL) != 0)
        time = (100.0 * fn_1_629C4(lbl_1_bss_71EC8.unk0[id], lbl_1_bss_71EC8.unk10)) / 60.0;
    else
        time = mode_info.timerCurr * 100 / 60;
    if (time < 0)
        time = 0;

    sp8.x = -0.45f;
    sp8.y = 0.0f;
    sp8.z = 0.0f;

    /* Draw LCD display on goal */
    mtxa_push();

    digit = time % 10;
    time /= 10;
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(goal_LCD_small_numbers[digit]);

    mtxa_translate(&sp8);

    digit = time % 10;
    time /= 10;
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(goal_LCD_small_numbers[digit]);

    mtxa_pop();

    sp8.x = -0.6666f;

    digit = time % 10;
    time /= 10;
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(goal_LCD_large_numbers[digit]);

    mtxa_translate(&sp8);

    digit = time % 10;
    time /= 10;
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(goal_LCD_large_numbers[digit]);

    mtxa_translate(&sp8);

    digit = time % 10;
    time /= 10;
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(goal_LCD_large_numbers[digit]);

    fade_color_base_default();
}

void stobj_goaltape_coli(struct Stobj *stobj, struct PhysicsBall *ball)
{
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float radius;
    float radiusSq;
    float x;
    float y;
    float z;
    int i;
    struct GoalTape_sub *sub;
    int hit;

    sp14.x = 0.0f;
    sp14.y = 0.0f;
    sp14.z = 0.0f;
    hit = 0;
    mtxa_from_identity();
    mtxa_translate(&stobj->u_some_pos);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rigid_inv_tf_point(&ball->pos, &sp44);
    radius = ball->radius;
    radiusSq = radius * radius;

    sub = ((struct GoalTape *)stobj->extraData)->unk18;
    for (i = 8; i > 0; i--, sub++)
    {
        if (sub->unk28 & 1)
            continue;
        sp38.x = sub->unk0.x - sp44.x;
        if (__fabs(sp38.x) > radius)
            continue;
        sp38.z = sub->unk0.z - sp44.z;
        if (__fabs(sp38.z) > radius)
            continue;
        sp38.y = sub->unk0.y - sp44.y;
        if (__fabs(sp38.y) > radius)
            continue;
        if (mathutil_vec_sq_len(&sp38) > radiusSq)
            continue;
        vec_set_len(radius, &sp38, &sp38);
        x = sp44.x + sp38.x;
        y = sp44.y + sp38.y;
        z = sp44.z + sp38.z;
        sp20.x = x - sub->unk0.x;
        sp20.y = y - sub->unk0.y;
        sp20.z = z - sub->unk0.z;
        sub->unk0.x = x;
        sub->unk0.y = y;
        sub->unk0.z = z;
        sub->unk1C.x += sp20.x;
        sub->unk1C.y += sp20.y;
        sub->unk1C.z += sp20.z;
        hit = 1;
        vec_set_len(-0.05 * mathutil_vec_len(&sp20), &sp38, &sp8);
        sp14.x += sp8.x;
        sp14.y += sp8.y;
        sp14.z += sp8.z;
        sp2C.x = sub->unkC.x;
        sp2C.y = sub->unkC.y;
        sp2C.z = sub->unkC.z;
        vec_normalize_len(&sp38);
        if (mathutil_vec_dot_prod(&sp38, &sp2C) < 0.0)
        {
            sp38.x = -sp38.x;
            sp38.y = -sp38.y;
            sp38.z = -sp38.z;
        }
        sp2C.x += 0.125 * (sp38.x - sp2C.x);
        sp2C.y += 0.125 * (sp38.y - sp2C.y);
        sp2C.z += 0.125 * (sp38.z - sp2C.z);
        vec_normalize_len(&sp2C);
        sub->unkC.x = sp2C.x;
        sub->unkC.y = sp2C.y;
        sub->unkC.z = sp2C.z;
    }
    if (hit != 0)
    {
        mtxa_tf_vec(&sp14, &sp14);
        ball->vel.x += sp14.x;
        ball->vel.y += sp14.y;
        ball->vel.z += sp14.z;
    }
}

void stobj_goaltape_dest(struct Stobj *stobj) {}

void stobj_goaltape_cb_f(struct Stobj *stobj) {}

void fn_1_ABE84(struct GoalTape_sub *arg0, int faceCount, struct NlModel *model1, struct NlModel *model2)
{
    Vec normal;
    Vec sp8;
    struct NlVtxTypeB *vtx;
    s16 rotX;
    s16 rotY;
    struct NlMesh *mesh2;
    struct NlMesh *mesh1;
    u32 flags;

    memcpy_handler(model2, model1, sizeof(*model2));

    mesh1 = (struct NlMesh *)model1->meshStart;
    mesh2 = (struct NlMesh *)model2->meshStart;
    memcpy_handler(mesh2, mesh1, sizeof(*mesh2));

    mesh2->dispListSize = (faceCount * 64) + 8;
    mesh1 = (void *)mesh1->dispListStart;
    mesh2 = (void *)mesh2->dispListStart;
    flags = ((struct NlDispList *)mesh1)->flags;
    flags &= ~0x14;
    flags |= 0x10;
    ((struct NlDispList *)mesh2)->flags = flags;
    ((struct NlDispList *)mesh2)->faceCount = faceCount * 2;

    mesh2 = (void *)((struct NlDispList *)mesh2)->vtxData;
    vtx = (void *)mesh2;
    for (; faceCount > 0; faceCount--, vtx++, arg0++)
    {
        normal.x = arg0->unkC.x;
        normal.y = arg0->unkC.y;
        normal.z = arg0->unkC.z;
        rotY = math_atan2(normal.x, normal.z) - 0x8000;
        rotX = math_atan2(normal.y, math_sqrt(mathutil_sum_of_sq_2(normal.x, normal.z)));
        mtxa_from_rotate_y(rotY);
        mtxa_rotate_x(rotX);
        sp8.x = 0.0f;
        sp8.y = 0.125f;
        sp8.z = 0.0f;
        mtxa_tf_vec(&sp8, &sp8);

        vtx->x = arg0->unk0.x + sp8.x;
        *(u32 *)&vtx->x |= 1;
        vtx->y = arg0->unk0.y + sp8.y;
        vtx->z = arg0->unk0.z + sp8.z;
        vtx->nx = normal.x;
        vtx->ny = normal.y;
        vtx->nz = normal.z;
        vtx->s = arg0->unk18;
        vtx->t = 1.0f;
        *(u32 *)&vtx->t |= 1;
        vtx++;

        vtx->x = arg0->unk0.x - sp8.x;
        *(u32 *)&vtx->x |= 1;
        vtx->y = arg0->unk0.y - sp8.y;
        vtx->z = arg0->unk0.z - sp8.z;
        vtx->nx = normal.x;
        vtx->ny = normal.y;
        vtx->nz = normal.z;
        vtx->s = arg0->unk18;
        vtx->t = 0.0f;
        *(u32 *)&vtx->t |= 1;
    }
    mesh2 = (void *)vtx;
    *(u32 *)mesh2 = 0;
}

void stobj_goalbag_init(struct Stobj *stobj)
{
    struct GoalBag *bag;

    stobj->state = 1;
    stobj->unk8 = 0x12;
    stobj->model = init_common_gma->modelEntries[0x3E].model;
    stobj->boundSphereRadius = stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->u_some_pos = stobj->unkA8;
    stobj->u_local_pos.x = 0.0f;
    stobj->u_local_pos.y = -1.0f;
    stobj->u_local_pos.z = 0.1f;
    vec_normalize_len(&stobj->u_local_pos);
    stobj->u_local_vel.x = 0.0f;
    stobj->u_local_vel.y = 0.0f;
    stobj->u_local_vel.z = 0.0f;
    bag = stobj->extraData;
    bag->stobj = stobj;
    bag->unk24 = -1;
    bag->openness = 0.0f;
    bag->unk8 = 0.0f;
}

static const Vec lbl_1_rodata_8EDC = { 0.0f, 3.55f, 0.0f };
static const Vec lbl_1_rodata_8EE8 = { 1.0f, 0.0f, 0.0f };

void stobj_goalbag_tick(struct Stobj *stobj)
{
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float temp_f12;
    float temp_f2_4;
    float temp_f2_6;
    float radius;
    float x;
    float y;
    float z;
    struct ItemGroup *group;
    struct GoalBag *bag = stobj->extraData;

    switch (stobj->state)
    {
    case 1:
        break;
    case 2:
    case 3:
        stobj->state = 4;
        switch (main_mode)
        {
        case 2:
            switch (modeCtrl.gameType)
            {
            case 1:
                stobj->counter = 120;
                break;
            default:
                stobj->counter = -1;
                break;
            }
            break;
        default:
            stobj->counter = -1;
            break;
        }
        bag->unk8 = 0.05 + 0.1 * RAND_FLOAT();
        /* fall through */
    case 4:
        if (stobj->counter > 0)
        {
            stobj->counter--;
            if (stobj->counter == 0 && !(mode_info.flags & 0x20))
                stobj->state = 5;
        }
        bag->unk8 += 0.005;
        bag->unk8 *= 0.99;
        bag->openness += bag->unk8;
        if (bag->openness < 0.0)
        {
            bag->openness = 0.0f;
            if (bag->unk8 < 0.0)
                bag->unk8 = 0.5 * -bag->unk8;
        }
        else if (bag->openness > 1.0)
        {
            bag->openness = 1.0f;
            if (bag->unk8 > 0.0)
                bag->unk8 = 0.5 * -bag->unk8;
        }
        break;
    case 5:
    case 6:
        stobj->state = 7;
        stobj->counter = 60;
        bag->unk8 = 0.05 + 0.1 * RAND_FLOAT();
        /* fall through */
    case 7:
        stobj->counter--;
        bag->unk8 -= 0.005;
        bag->unk8 *= 0.99;
        bag->openness += bag->unk8;
        if (bag->openness < 0.0)
        {
            bag->openness = 0.0f;
            bag->u_flags = 0;
            bag->unk24 = -1;
            if (stobj->counter < 0)
            {
                stobj->state = 1;
                bag->unk8 = 0.0f;
            }
            else
            {
                if (bag->unk8 < 0.0)
                    bag->unk8 = 0.5 * -bag->unk8;
            }
        }
        else if (bag->openness > 1.0)
        {
            bag->openness = 1.0f;
            if (bag->unk8 > 0.0)
                bag->unk8 = 0.5 * -bag->unk8;
        }
        break;
    }

    sp2C.x = 0.008f * g_gravity_dir.x;
    sp2C.y = 0.008f * g_gravity_dir.y;
    sp2C.z = 0.008f * g_gravity_dir.z;
    if (stobj->animGroupId > 0)
    {
        mtxa_from_mtx(itemgroups[stobj->animGroupId].transform);
        mtxa_rigid_inv_tf_vec(&sp2C, &sp2C);
    }
    stobj->u_local_vel.x += sp2C.x;
    stobj->u_local_vel.y += sp2C.y;
    stobj->u_local_vel.z += sp2C.z;
    if (stobj->animGroupId > 0)
    {
        group = &itemgroups[stobj->animGroupId];
        mtxa_from_mtx(group->prevTransform);
        mtxa_tf_point(&stobj->position, &sp2C);
        mtxa_from_mtx(group->transform);
        mtxa_tf_point(&stobj->position, &sp38);
        sp2C.x -= sp38.x;
        sp2C.y -= sp38.y;
        sp2C.z -= sp38.z;
        mtxa_rigid_inv_tf_vec(&sp2C, &sp2C);
        stobj->u_local_vel.x += 0.020000000000000018 * (sp2C.x - stobj->u_local_vel.x);
        stobj->u_local_vel.y += 0.020000000000000018 * (sp2C.y - stobj->u_local_vel.y);
        stobj->u_local_vel.z += 0.020000000000000018 * (sp2C.z - stobj->u_local_vel.z);
    }
    else
    {
        stobj->u_local_vel.x *= 0.98;
        stobj->u_local_vel.y *= 0.98;
        stobj->u_local_vel.z *= 0.98;
    }
    sp20 = lbl_1_rodata_8EDC;
    sp14 = lbl_1_rodata_8EE8;
    mtxa_from_translate(&bag->goal.pos);
    mtxa_rotate_z(bag->goal.rotZ);
    mtxa_rotate_y(bag->goal.rotY);
    mtxa_rotate_x(bag->goal.rotX);
    mtxa_tf_point(&sp20, &sp20);
    mtxa_tf_vec(&sp14, &sp14);
    radius = stobj->boundSphereRadius;
    x = stobj->u_some_pos.x + (radius * stobj->u_local_pos.x);
    y = stobj->u_some_pos.y + (radius * stobj->u_local_pos.y);
    z = stobj->u_some_pos.z + (radius * stobj->u_local_pos.z);
    sp2C.x = x - sp20.x;
    sp2C.y = y - sp20.y;
    sp2C.z = z - sp20.z;

    temp_f12 = sp2C.x * sp14.x + sp2C.y * sp14.y + sp2C.z * sp14.z;
    if (mathutil_vec_sq_len(&sp2C) - temp_f12 * temp_f12 < radius * radius)
    {
        sp2C.x = sp20.x + sp14.x * temp_f12;
        sp2C.y = sp20.y + sp14.y * temp_f12;
        sp2C.z = sp20.z + sp14.z * temp_f12;
        sp8.x = x - sp2C.x;
        sp8.y = y - sp2C.y;
        sp8.z = z - sp2C.z;
        vec_normalize_len(&sp8);
        stobj->u_local_pos.x = sp2C.x + sp8.x * radius - stobj->u_some_pos.x;
        stobj->u_local_pos.y = sp2C.y + sp8.y * radius - stobj->u_some_pos.y;
        stobj->u_local_pos.z = sp2C.z + sp8.z * radius - stobj->u_some_pos.z;
        vec_normalize_len(&stobj->u_local_pos);
        temp_f2_4 = sp8.x * stobj->u_local_vel.x + sp8.y * stobj->u_local_vel.y + sp8.z * stobj->u_local_vel.z;
        if (temp_f2_4 < 0.0)
        {
            temp_f2_4 *= -1.5;
            stobj->u_local_vel.x += temp_f2_4 * sp8.x;
            stobj->u_local_vel.y += temp_f2_4 * sp8.y;
            stobj->u_local_vel.z += temp_f2_4 * sp8.z;
        }
    }
    temp_f2_6 = -mathutil_vec_dot_prod(&stobj->u_local_pos, &stobj->u_local_vel);
    stobj->u_local_vel.x += temp_f2_6 * stobj->u_local_pos.x;
    stobj->u_local_vel.y += temp_f2_6 * stobj->u_local_pos.y;
    stobj->u_local_vel.z += temp_f2_6 * stobj->u_local_pos.z;
    stobj->u_local_pos.x += stobj->u_local_vel.x;
    stobj->u_local_pos.y += stobj->u_local_vel.y;
    stobj->u_local_pos.z += stobj->u_local_vel.z;
    vec_normalize_len(&stobj->u_local_pos);
    mtxa_from_rotate_y(-stobj->rotY);
    mtxa_rotate_x(0);
    mtxa_tf_vec(&stobj->u_local_pos, &sp38);
    stobj->rotX = math_atan2(sp38.z, sp38.y) - 0x8000;
    stobj->rotZ = math_atan2(sp38.x, math_sqrt(mathutil_sum_of_sq_2(sp38.z, sp38.y)));
    sp38.x = 2.0 * stobj->position.x - stobj->u_some_pos.x;
    sp38.y = 2.0 * stobj->position.y - stobj->u_some_pos.y;
    sp38.z = 2.0 * stobj->position.z - stobj->u_some_pos.z;
    if (stobj->animGroupId > 0)
    {
        mtxa_from_mtx(itemgroups[stobj->animGroupId].transform);
        mtxa_tf_point(&sp38, &sp38);
    }
    g_something_with_translating_items(5, &sp38, 1.0f);
}

void stobj_goalbag_disp(struct Stobj *stobj)
{
    Vec sp8;
    float alpha;
    struct GMAModel *model;
    struct GoalBag *bag = stobj->extraData;
    s16 rotZ;

    mtxa_from_mtxb();
    mtxa_translate(&stobj->u_some_pos);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);

    if (bag->openness == 0.0)
    {
        model = stobj->model;
        alpha = g_get_sphere_camera_zdist_clamped(&model->boundSphereCenter, model->boundSphereRadius);
        if (alpha > 0.0f)
        {
            if (alpha < 1.0f)
            {
                avdisp_set_alpha(alpha);
                avdisp_draw_model_culled_sort_always(model);
            }
            else
            {
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(model);
            }
        }
    }
    else
    {
        rotZ = 9102.0f * bag->openness;
        sp8.x = 0.0f;
        sp8.y = -0.5 * bag->openness;
        sp8.z = 0.0f;
        mtxa_translate(&sp8);
        mtxa_push();
        mtxa_rotate_z(-rotZ);
        model = init_common_gma->modelEntries[0x3C].model;
        alpha = g_get_sphere_camera_zdist_clamped(&model->boundSphereCenter, model->boundSphereRadius);
        if (alpha > 0.0f)
        {
            if (alpha < 1.0f)
            {
                avdisp_set_alpha(alpha);
                avdisp_draw_model_culled_sort_always(model);
            }
            else
            {
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(model);
            }
        }
        mtxa_pop();
        mtxa_rotate_z(rotZ);
        model = init_common_gma->modelEntries[0x3D].model;
        alpha = g_get_sphere_camera_zdist_clamped(&model->boundSphereCenter, model->boundSphereRadius);
        if (alpha > 0.0f)
        {
            if (alpha < 1.0f)
            {
                avdisp_set_alpha(alpha);
                avdisp_draw_model_culled_sort_always(model);
            }
            else
            {
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(model);
            }
        }
    }
}

static const Vec lbl_1_rodata_8F58 = { 0.0f, 0.0f, -1.0f };

void stobj_goalbag_coli(struct Stobj *stobj, struct PhysicsBall *ball)
{
    float _dummy = 0.0f;  /* force pool base register allocation */
    Vec sp50;
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    Vec delta;
    float temp_f0;
    float impulse = 0.0f;
    float radius;
    struct GoalBag *bag;

    sp50.x = ball->pos.x - stobj->position.x;
    sp50.y = ball->pos.y - stobj->position.y;
    sp50.z = ball->pos.z - stobj->position.z;
    vec_normalize_len(&sp50);
    sp44.x = stobj->position.z - stobj->position_2.z;
    sp44.y = stobj->position.y - stobj->position_2.y;
    sp44.z = stobj->position.z - stobj->position_2.z;
    sp2C.x = ball->vel.x;
    sp2C.y = ball->vel.y;
    sp2C.z = ball->vel.z;
    sp38.x = sp2C.x - sp44.x;
    sp38.y = sp2C.y - sp44.y;
    sp38.z = sp2C.z - sp44.z;
    temp_f0 = -1.5 * mathutil_vec_dot_prod(&sp50, &ball->vel);
    if (temp_f0 > 0.0)
    {
        sp38.x += temp_f0 * sp50.x;
        sp38.y += temp_f0 * sp50.y;
        sp38.z += temp_f0 * sp50.z;
        ball->vel.x = sp38.x + sp44.x;
        ball->vel.y = sp38.y + sp44.y;
        ball->vel.z = sp38.z + sp44.z;
        impulse = temp_f0;
    }
    radius = stobj->boundSphereRadius + ball->radius;
    sp50.x *= radius;
    sp50.y *= radius;
    sp50.z *= radius;
    delta.x = stobj->position.x + sp50.x - ball->pos.x;
    delta.y = stobj->position.y + sp50.y - ball->pos.y;
    delta.z = stobj->position.z + sp50.z - ball->pos.z;
    ball->pos.x += delta.x;
    ball->pos.y += delta.y;
    ball->pos.z += delta.z;
    temp_f0 = -mathutil_vec_dot_prod(&sp50, &stobj->u_local_pos);
    sp50.x += temp_f0 * stobj->u_local_pos.x;
    sp50.y += temp_f0 * stobj->u_local_pos.y;
    sp50.z += temp_f0 * stobj->u_local_pos.z;
    vec_normalize_len(&sp50);
    sp38.x = sp44.x + stobj->u_local_vel.x - sp2C.x;
    sp38.y = sp44.y + stobj->u_local_vel.y - sp2C.y;
    sp38.z = sp44.z + stobj->u_local_vel.z - sp2C.z;
    temp_f0 = -2.0 * mathutil_vec_dot_prod(&sp50, &sp38);
    if (temp_f0 < 0.0)
    {
        sp38.x += temp_f0 * sp50.x;
        sp38.y += temp_f0 * sp50.y;
        sp38.z += temp_f0 * sp50.z;
        stobj->u_local_vel.x = sp2C.x + (sp38.x - sp44.x);
        stobj->u_local_vel.y = sp2C.y + (sp38.y - sp44.y);
        stobj->u_local_vel.z = sp2C.z + (sp38.z - sp44.z);
    }
    temp_f0 = -mathutil_vec_dot_prod(&stobj->u_local_pos, &stobj->u_local_vel);
    stobj->u_local_vel.x += temp_f0 * stobj->u_local_pos.x;
    stobj->u_local_vel.y += temp_f0 * stobj->u_local_pos.y;
    stobj->u_local_vel.z += temp_f0 * stobj->u_local_pos.z;
    bag = stobj->extraData;
    if (bag->u_flags != 0)
    {
        temp_f0 = -2.0 * mathutil_vec_dot_prod(&stobj->u_local_pos, &sp2C);
        if (temp_f0 < 0.0)
            bag->unk8 += temp_f0;
    }
    if (impulse > 0.1f)
    {
        float _dummy = 0.0f;  /* force pool base register allocation for inner loop */
        struct Effect effect;
        Vec sp20 = lbl_1_rodata_8F58;
        Vec sp14;
        Vec sp8;
        int i;
        int step;
        int count;
        int count2;
        int step2;
        float speed;
        float baseSpeed;

        mtxa_push();
        radius = stobj->boundSphereRadius;
        sp8 = stobj->position;
        if (stobj->animGroupId > 0)
        {
            mtxa_from_mtx(itemgroups[stobj->animGroupId].transform);
            mtxa_tf_point(&sp8, &sp8);
            mtxa_tf_vec(&sp44, &sp44);
            mtxa_tf_vec(&sp2C, &sp2C);
        }
        sp38.x = 0.25f * (sp44.x + sp2C.x);
        sp38.y = 0.25f * (sp44.y + sp2C.y);
        sp38.z = 0.25f * (sp44.z + sp2C.z);
        memset(&effect, 0, sizeof(effect));
        effect.playerId = current_ball->playerId;
        baseSpeed = 0.1f;
        for (i = 2; i > 0; i--)
        {
            effect.type = 5;
            count = 16.0f + (16.0f * impulse);
            if (count > 24)
                count = 24;
            step = 65536.0f / count;
            mtxa_from_rotate_y(rand() & 0x7FFF);
            mtxa_rotate_x(rand() & 0x7FFF);
            mtxa_rotate_z(rand() & 0x7FFF);
            while (count > 0)
            {
                mtxa_rotate_y(step);
                mtxa_tf_vec(&sp20, &sp14);
                effect.pos.x = sp8.x + (radius * sp14.x);
                effect.pos.y = sp8.y + (radius * sp14.y);
                effect.pos.z = sp8.z + (radius * sp14.z);
                speed = baseSpeed + 0.02f * RAND_FLOAT();
                effect.vel.x = sp38.x + (speed * sp14.x);
                effect.vel.y = sp38.y + (speed * sp14.y);
                effect.vel.z = sp38.z + (speed * sp14.z);
                spawn_effect(&effect);
                count--;
            }
            effect.type = 2;
            count2 = 12.0f + (12.0f * impulse);
            if (count2 > 18)
                count2 = 18;
            step2 = 65536.0f / count2;
            while (count2 > 0)
            {
                mtxa_rotate_y(step2);
                mtxa_tf_vec(&sp20, &sp14);
                effect.pos.x = sp8.x + (radius * sp14.x);
                effect.pos.y = sp8.y + (radius * sp14.y);
                effect.pos.z = sp8.z + (radius * sp14.z);
                speed = baseSpeed + 0.02f * RAND_FLOAT();
                effect.vel.x = sp38.x + (speed * sp14.x);
                effect.vel.y = sp38.y + (speed * sp14.y);
                effect.vel.z = sp38.z + (speed * sp14.z);
                spawn_effect(&effect);
                count2--;
            }
            baseSpeed *= 0.5f;
            sp38.x *= 2.0f;
            sp38.y *= 2.0f;
            sp38.z *= 2.0f;
        }
        mtxa_pop();
    }
}

void stobj_goalbag_dest(struct Stobj *stobj) {}

void stobj_goalbag_cb_f(struct Stobj *stobj) {}

void stobj_goalbag_exmaster_init(struct Stobj *stobj)
{
    stobj_goalbag_init(stobj);
}

void stobj_goalbag_exmaster_tick(struct Stobj *stobj)
{
    stobj_goalbag_tick(stobj);
}

void stobj_goalbag_exmaster_disp(struct Stobj *stobj)
{
    stobj_goalbag_disp(stobj);
}

void stobj_goalbag_exmaster_coli(struct Stobj *stobj, struct PhysicsBall *ball)
{
    stobj_goalbag_coli(stobj, ball);
}

void stobj_goalbag_exmaster_dest(struct Stobj *stobj) {}

void stobj_goalbag_exmaster_cb_f(struct Stobj *stobj) {}

void g_something_with_goals_and_physicsball(int goalId, struct PhysicsBall *ball)
{
    Vec sp14;
    Vec sp8;
    float dist;
    float minDist;
    struct GoalTape *tape;
    struct Stobj *stobj;
    int i;
    struct GoalTape_sub *sub;
    int closest;

    fn_1_ADB14(goalId, ball);
    fn_1_375A4();
    if (goalId < 0x10)
    {
        tape = &goaltapes[goalId];
        if (tape->unk14 == NULL || tape->u_flags != 0)
            return;
        stobj = tape->unk14;
        mtxa_from_identity();
        mtxa_translate(&stobj->u_some_pos);
        mtxa_rotate_z(stobj->rotZ);
        mtxa_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX);
        mtxa_rigid_inv_tf_point(&ball->pos, &sp14);
        mtxa_rigid_inv_tf_vec(&ball->vel, &sp8);
        if (tape->unk10 < 0)
            tape->unk10 = mode_info.timerCurr;
        closest = -1;
        minDist = 17.5f;
        sub = tape->unk18;
        for (i = 0; i < 8; i++, sub++)
        {
            if (sub->unk28 & 4)
            {
                dist = mathutil_vec_distance(&sub->unk0, &sp14);
                if (minDist > dist)
                {
                    minDist = dist;
                    closest = i;
                }
            }
        }
        if (closest >= 0)
        {
            sub = &tape->unk18[closest];
            sub[0].unk28 &= 0xFFFFFFFB;
            sub[1].unk28 &= 0xFFFFFFFD;
            sub[0].unk1C.x += sp8.x;
            sub[0].unk1C.y += sp8.y;
            sub[0].unk1C.z += sp8.z;
            sub[1].unk1C.x += sp8.x;
            sub[1].unk1C.y += sp8.y;
            sub[1].unk1C.z += sp8.z;
            stobj->state = 1;
            tape->u_flags = 1;
        }
    }
}

void fn_1_AD9B0(int arg0)
{
    int i;
    int j;
    struct GoalTape *tape;
    struct GoalTape_sub *sub;
    float temp;

    fn_1_ADF3C(arg0);
    tape = goaltapes;
    for (i = 8; i > 0; i--, tape++)
    {
        if (tape->unk10 <= arg0 && tape->u_flags != 0)
        {
            tape->u_flags = 0;
            sub = tape->unk18;
            for (j = 8; j > 0; j--, sub++)
            {
                temp = (j - 1) / 7.0f;
                sub->unk0.x = 1.75 * temp - 0.875;
                sub->unk0.y = tape->unk14->u_model_origin.y;
                sub->unk0.z = 0.0f;
                sub->unkC.x *= 0.25;
                sub->unkC.y *= 0.25;
                sub->unkC.z = 0.75 + 0.25 * sub->unkC.z;
                vec_normalize_len(&sub->unkC);
                sub->unk28 |= 6;
            }
            tape->unk18[0].unk28 &= 0xFFFFFFFD;
            tape->unk18[7].unk28 &= 0xFFFFFFFB;
        }
    }
}

void fn_1_ADB14(int goalId, struct PhysicsBall *ball)
{
    struct Effect effect;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float radius;
    int confettiCount;
    struct GoalBag *bag;
    struct Stobj *stobj;
    u16 cameraMask;

    if (goalId < 0x10)
    {
        bag = &goalbags[goalId];
        stobj = bag->stobj;
        if (stobj == NULL || bag->u_flags != 0)
            return;
        bag->u_flags = 1;
        stobj->state = 2;
        stobj->boundSphereRadius = 0.5 * stobj->model->boundSphereRadius;
        stobj->u_model_origin = stobj->model->boundSphereCenter;
        stobj->u_model_origin.x *= 0.5;
        stobj->u_model_origin.y *= 0.5;
        stobj->u_model_origin.z *= 0.5;
        stobj->u_local_vel.x += 0.5 * ball->vel.x;
        stobj->u_local_vel.y += 0.5 * ball->vel.y;
        stobj->u_local_vel.z += 0.5 * ball->vel.z;
        call_SoundReqID_arg_0(0x16);
        call_SoundReqID_arg_0(0x127);
        if (bag->unk24 < 0)
            bag->unk24 = mode_info.timerCurr;
        cameraMask = 1 << current_ball->playerId;
        memset(&effect, 0, sizeof(effect));
        effect.type = 0;
        mtxa_from_mtx(itemgroups[stobj->animGroupId].transform);
        mtxa_tf_vec(&ball->vel, &effect.vel);
        mtxa_tf_point(&stobj->position, &sp14);
        mtxa_from_mtx(itemgroups[stobj->animGroupId].prevTransform);
        mtxa_tf_point(&stobj->position_2, &sp8);
        effect.vel.x += sp14.x - sp8.x;
        effect.vel.y += sp14.y - sp8.y;
        effect.vel.z += sp14.z - sp8.z;
        radius = stobj->model->boundSphereRadius - init_common_gma->modelEntries[0x46].model->boundSphereRadius;
        mtxa_from_mtx(itemgroups[stobj->animGroupId].transform);
        mtxa_translate(&stobj->position);
        if (main_mode == 2 && modeCtrl.gameType == 1)
        {
            switch (modeCtrl.playerCount)
            {
            case 2:
                confettiCount = 130;
                break;
            case 3:
                confettiCount = 100;
                break;
            case 4:
                confettiCount = 70;
                break;
            default:
                confettiCount = 160;
                break;
            }
        }
        else
            confettiCount = 160;
        sp20.x = 0.0f;
        sp20.y = 0.0f;

        while (confettiCount > 0)
        {
            sp20.z = 0.5 * (radius * (1.0 + RAND_FLOAT()));
            mtxa_rotate_y(rand() & 0x7FFF);
            mtxa_rotate_x(rand() & 0x7FFF);
            mtxa_tf_point(&sp20, &effect.pos);
            effect.rotX = rand() & 0x7FFF;
            effect.rotY = rand() & 0x7FFF;
            effect.rotZ = rand() & 0x7FFF;
            if (confettiCount & 1)
                effect.cameras = cameraMask;
            else
                effect.cameras = 0;
            spawn_effect(&effect);
            confettiCount--;
        }
    }
}

void fn_1_ADF3C(int arg0)
{
    int i;
    struct GoalBag *bag;
    struct Stobj *stobj;

    bag = goalbags;
    for (i = 0x10; i > 0; i--, bag++)
    {
        if (bag->unk24 <= arg0 && bag->u_flags != 0)
        {
            bag->u_flags = 0;
            bag->openness = 0.0f;
            bag->unk8 = 0.0f;
            stobj = bag->stobj;
            stobj->boundSphereRadius = stobj->model->boundSphereRadius;
            stobj->u_model_origin = stobj->model->boundSphereCenter;
            stobj->state = 1;
        }
    }
}

float g_get_sphere_camera_zdist_clamped(Vec *pos, float radius)
{
    Vec sp8;

    mtxa_tf_point(pos, &sp8);
    sp8.z += 0.1f + radius;
    sp8.z = -sp8.z;
    if (sp8.z > 1.0f)
        return 1.0f;
    if (sp8.z > 0.0f)
        return sp8.z;
    return 0.0f;
}

void fn_1_AE074(struct GoalTape *tape)
{
    int i;
    struct GoalTape_sub *sub;
    float y;

    tape->u_flags = 0;
    tape->unk10 = -1;
    y = tape->unk8;
    sub = tape->unk18;
    for (i = 8; i > 0; i--, sub++)
    {
        sub->unk0.x = 1.75 * (((i - 1) / 7.0f) - 0.5f);
        sub->unk0.y = y;
        sub->unk0.z = 0.0f;
        sub->unk1C.x *= 0.05f;
        sub->unk1C.y *= 0.05f;
        sub->unk1C.z *= 0.05f;
        sub->unkC.x *= 0.25f;
        sub->unkC.y *= 0.25f;
        sub->unkC.z = 0.75f + 0.25f * sub->unkC.z;
        vec_normalize_len(&sub->unkC);
        sub->unk28 = 6;
    }
    tape->unk18[0].unk28 &= 0xFFFFFFFD;
    tape->unk18[0].unk28 |= 1;
    tape->unk18[7].unk28 &= 0xFFFFFFFB;
    tape->unk18[7].unk28 |= 1;
}
