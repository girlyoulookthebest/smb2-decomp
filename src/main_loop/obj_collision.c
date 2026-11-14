/* Ball vs. item and stage object collision (SMB1 obj_collision.c). */
#include "types.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "item.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "stage.h"
#include "stobj.h"

BOOL g_is_ball_in_ig_coli_range(struct ItemGroup *group, struct StageCollHeader *coll, int id, struct Sphere *sphere);
extern struct Stobj stobjs[0x90];
void event_stobj_collision_child_tick(void);
BOOL g_some_item_collision_check(Vec *a, Vec *b, float e, Vec *c, Vec *d, float f);
BOOL fn_1_A74E4(Vec *a, Vec *b, float e, Vec *c, Vec *d, float f);
void g_something_with_item_coli(Vec *a, Vec *b, float d, Vec *c, float e);

void event_stobj_collision_init(void) {}

void event_stobj_collision_tick(void)
{
    if (!(g_some_other_flags & 0xA))
        event_stobj_collision_child_tick();
}

void event_stobj_collision_dest(void) {}

void event_stobj_collision_child_tick(void)
{
    struct Ball *ball;
    int i;
    int j;
    s8 *status;
    struct Ball *ballBackup;
    struct PhysicsBall physBall;
    Vec itemPos;
    struct Sphere sphere;

    ballBackup = current_ball;
    i = 0;
    ball = balls;
    for (; i < 8; i++, ball++)
    {
        current_ball = ball;
        if (ball->status != 0 && ball->status != 4)
        {
            init_physicsball_from_ball(ball, &physBall);
            sphere.pos = physBall.pos;
            sphere.radius = physBall.radius;
            if (events[EVENT_STOBJ].state == 2)
            {
                struct Stobj *stobj;

                status = g_poolInfo.stobjPool.statusList;
                stobj = stobjs;
                for (j = g_poolInfo.stobjPool.count; j > 0; j--, stobj++, status++)
                {
                    if (*status != 0 && (stobj->unk8 & 2))
                    {
                        if (physBall.itemGroupId != stobj->animGroupId)
                            tf_physball_to_itemgroup_space(&physBall, stobj->animGroupId);
                        if (g_some_item_collision_check(&physBall.prevPos, &physBall.pos, physBall.radius, &stobj->position_2, &stobj->position, stobj->boundSphereRadius))
                            stobj->coliFunc(stobj, &physBall);
                    }
                }
            }
            if (!(ball->flags & 0x1000000000ULL) && events[EVENT_ITEM].state == 2)
            {
                struct Item *item;

                status = g_poolInfo.itemPool.statusList;
                item = items;
                for (j = g_poolInfo.itemPool.count; j > 0; j--, item++, status++)
                {
                    if (*status != 0
                     && (item->flags & 2)
                     && item->unkC == 0
                     && (modeCtrl.gameType != 5
                      || current_stage_id != 0x97
                      || item->animGroupId == 0
                      || g_is_ball_in_ig_coli_range(&itemgroups[item->animGroupId], &stagedef->collHeaders[item->animGroupId], item->animGroupId, &sphere)))
                    {
                        BOOL (*func)(Vec *, Vec *, float, Vec *, Vec *, float);

                        if (physBall.itemGroupId != item->animGroupId)
                            tf_physball_to_itemgroup_space(&physBall, item->animGroupId);
                        if (item->flags & 8)
                            func = fn_1_A74E4;
                        else
                            func = g_some_item_collision_check;

                        itemPos = item->pos;
                        if (func(&physBall.prevPos, &physBall.pos, physBall.radius, &item->prevPos, &itemPos, item->unk14))
                        {
                            item->unkC = 8;
                            /* cast: LLVM can't call through a __ptr32 (DPTR) fn pointer; no-op on GC */
                            ((void (*)(struct Item *, struct PhysicsBall *))item->coliFunc)(item, &physBall);
                            if (item->flags & 0x10)
                            {
                                Vec normal = itemPos;

                                normal.x -= physBall.pos.x;
                                normal.y -= physBall.pos.y;
                                normal.z -= physBall.pos.z;
                                vec_normalize_len(&normal);
                                g_something_with_item_coli(&normal, &physBall.vel, 1.0f, &item->vel, item->unk18);
                            }
                            item->pos = itemPos;
                        }
                    }
                }
            }
            if (!(mode_info.flags & 0x10))
            {
                if (physBall.itemGroupId != 0)
                    tf_physball_to_itemgroup_space(&physBall, 0);
                apply_physicsball_to_ball(ball, &physBall);
            }
        }
    }
    current_ball = ballBackup;
}

BOOL g_some_item_collision_check(Vec *arg0, Vec *arg1, float arg4, Vec *arg2, Vec *arg3, float arg5)
{
    float temp_f6 = arg0->x - arg2->x;
    float temp_f7 = arg0->y - arg2->y;
    float temp_f8 = arg0->z - arg2->z;
    float temp_f10;
    float temp_f11 = (arg1->x - arg3->x) - temp_f6;
    float temp_f12 = (arg1->y - arg3->y) - temp_f7;
    float temp_f13 = (arg1->z - arg3->z) - temp_f8;
    float temp_f9 = (temp_f11 * temp_f11) + (temp_f12 * temp_f12) + (temp_f13 * temp_f13);
    float temp_f3;
    float temp_f2;

    if (temp_f9 == 0.0f)
        return 0;
    temp_f10 = arg4 + arg5;
    temp_f3 = (temp_f6 * temp_f11) + (temp_f7 * temp_f12) + (temp_f8 * temp_f13);
    temp_f2 = (temp_f6 * temp_f6) + (temp_f7 * temp_f7) + (temp_f8 * temp_f8) - (temp_f10 * temp_f10);
    if (0.0f > (temp_f3 * temp_f3) - (temp_f9 * temp_f2))
        return 0;
    if (0.0f >= temp_f2)
        return 1;
    if (0.0f >= temp_f9 + temp_f3 + temp_f3 + temp_f2)
        return 1;
    if (0.0f <= temp_f3)
        return 0;
    return !(-temp_f3 >= temp_f9);
}

BOOL fn_1_A74E4(Vec *arg0, Vec *arg1, float arg4, Vec *arg2, Vec *arg3, float arg5)
{
    float temp_f10 = arg0->x - arg2->x;
    float temp_f11 = arg0->y - arg2->y;
    float temp_f8 = arg0->z - arg2->z;
    float temp_f9;
    float temp_f6 = (arg1->x - arg3->x) - temp_f10;
    float temp_f12 = (arg1->y - arg3->y) - temp_f11;
    float temp_f13 = (arg1->z - arg3->z) - temp_f8;
    float temp_f31 = (temp_f6 * temp_f6) + (temp_f12 * temp_f12) + (temp_f13 * temp_f13);
    float temp_f30;
    float temp_f3;
    float temp_f1;
    float temp_f3_2;

    if (temp_f31 == 0.0f)
        return 0;
    temp_f9 = arg4 + arg5;
    temp_f30 = (temp_f10 * temp_f6) + (temp_f11 * temp_f12) + (temp_f8 * temp_f13);
    temp_f3 = (temp_f10 * temp_f10) + (temp_f11 * temp_f11) + (temp_f8 * temp_f8) - (temp_f9 * temp_f9);
    temp_f1 = (temp_f30 * temp_f30) - (temp_f31 * temp_f3);
    if (0.0f > temp_f1)
        return 0;
    if (0.0f >= temp_f3)
    {
        arg1->x = arg0->x;
        arg1->y = arg0->y;
        arg1->z = arg0->z;
        arg3->x = arg2->x;
        arg3->y = arg2->y;
        arg3->z = arg2->z;
        return 1;
    }
    if (!(0.0f >= temp_f31 + temp_f30 + temp_f30 + temp_f3))
    {
        if (0.0f <= temp_f30)
            return 0;
        if (-temp_f30 >= temp_f31)
            return 0;
    }
    temp_f3_2 = -(temp_f30 + math_sqrt(temp_f1)) / temp_f31;
    arg1->x = arg0->x + temp_f3_2 * (arg1->x - arg0->x);
    arg1->y = arg0->y + temp_f3_2 * (arg1->y - arg0->y);
    arg1->z = arg0->z + temp_f3_2 * (arg1->z - arg0->z);
    arg3->x = arg2->x + temp_f3_2 * (arg3->x - arg2->x);
    arg3->y = arg2->y + temp_f3_2 * (arg3->y - arg2->y);
    arg3->z = arg2->z + temp_f3_2 * (arg3->z - arg2->z);
    return 1;
}

void g_something_with_item_coli(Vec *normal, Vec *velA, float massA, Vec *velB, float massB)
{
    float nx = normal->x;
    float ny = normal->y;
    float nz = normal->z;
    float ax = velA->x;
    float ay = velA->y;
    float az = velA->z;
    float bx = velB->x;
    float by = velB->y;
    float bz = velB->z;
    float dotA = nz * az + (nx * ax + ny * ay);
    /* the y term reads velA in both dot products */
    float dotB = nz * bz + (nx * bx + ny * ay);
    float impulseA = 2.0 * ((dotB - dotA) * ((massA * massB) / (massA * massB + massA * massA)));
    float impulseB = impulseA * -(massA / massB);

    ax += impulseA * nx;
    ay += impulseA * ny;
    az += impulseA * nz;
    bx += impulseB * nx;
    by += impulseB * ny;
    bz += impulseB * nz;
    velA->x = ax;
    velA->y = ay;
    velA->z = az;
    velB->x = bx;
    velB->y = by;
    velB->z = bz;
}
