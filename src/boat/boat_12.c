#include "boat.h"
#include "macros.h"
#include "stage.h"
#include "mathutil.h"

extern u8 lbl_14_bss_24;
extern float lbl_1_data_657B0;
extern int lbl_80198B38;
s16 *tri_coli_grid_lookup(struct StageCollHeader *coll, float x, float z);
void collide_ball_with_tri_edges(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_tri_vertices(struct PhysicsBall *ball, struct StageColiTri *tri);
s32 fn_1_A74E4(Vec *a, Vec *b, float e, Vec *c, Vec *d, float f);

#define dumb_dot(a, b, c, d) (nx = (a), ny = (b), dx = (c), dy = (d), nx * dx + ny * dy)

void fn_14_21A04(void)
{
    struct PhysicsBall physA;
    struct PhysicsBall physB;
    Vec diffPrev;
    Vec diffVel;
    Vec push;
    Vec diff;
    float len;
    float dist;
    float sqA;
    float sqB;
    struct Ball *prev;
    struct Ball *ballA;
    struct Ball *ballB;
    struct BoatPlayer *plA;
    struct BoatPlayer *plB;
    s32 i;
    s32 j;

    prev = current_ball;
    i = 0;
    ballA = balls;
    plA = lbl_14_bss_208;
    while (i < lbl_14_bss_24 - 1) {
        fn_14_22024(ballA, plA, &physA);
        physA.flags |= (ballA->flags & 1) != 0 ? 1 : 0;
        j = i + 1;
        ballB = ballA + 1;
        plB = plA + 1;
        while (j < lbl_14_bss_24) {
            fn_14_22024(ballB, plB, &physB);
            physB.flags |= (ballB->flags & 1) != 0 ? 1 : 0;
            PSVECSubtract(&physB.prevPos, &physA.prevPos, &diffPrev);
            PSVECSubtract(&physA.vel, &physB.vel, &diffVel);
            PSVECSubtract(&physA.pos, &physB.pos, &diff);
            if (mathutil_vec_dot_prod(&diffPrev, &diffVel) > 0.0f
                && fn_1_A74E4(&physA.prevPos, &physA.pos, physA.radius, &physB.prevPos, &physB.pos, physB.radius) != 0) {
                sqA = mathutil_vec_sq_len(&physA.vel);
                sqB = mathutil_vec_sq_len(&physB.vel);
                fn_14_220C0(&diff, &physA.vel, 1.0f, &physB.vel, 1.0f);
                sqA = fabs(mathutil_vec_sq_len(&physA.vel) - sqA);
                sqB = fabs(mathutil_vec_sq_len(&physB.vel) - sqB);
                sqB = sqA + sqB;
                PSVECAdd(&physA.pos, &physA.vel, &physA.pos);
                PSVECAdd(&physB.pos, &physB.vel, &physB.pos);
                if (sqB > 0.001) {
                    g_rumble_controller(plA->unk1, 1, 0.3 * lbl_80198B34);
                    g_rumble_controller(plB->unk1, 1, 0.3 * lbl_80198B34);
                    current_ball = ballA;
                    call_SoundReqID_arg_0(0x17);
                    current_ball = ballB;
                    call_SoundReqID_arg_0(0x18);
                    fn_14_12B24(0x262, &current_ball->pos);
                }
            }
            dist = vec_normalize_len(&diff) - 0.01;
            len = (physA.radius + physB.radius) - dist;
            if (len > 0.0) {
                PSVECScale(&diff, &push, (float)(0.5 * len));
                PSVECAdd(&physA.pos, &push, &physA.pos);
                PSVECSubtract(&physB.pos, &push, &physB.pos);
            }
            apply_physicsball_to_ball(ballB, &physB);
            j++;
            ballB++;
            plB++;
        }
        apply_physicsball_to_ball(ballA, &physA);
        i++;
        ballA++;
        plA++;
    }
    i = 0;
    ballA = balls;
    plA = lbl_14_bss_208;
    while (i < lbl_14_bss_24) {
        current_ball = ballA;
        ballA->flags &= 0xFFFFFFFF07FFFFFFLL;
        fn_14_22024(ballA, plA, &physA);
        fn_14_22254(&physA, stagedef);
        apply_physicsball_to_ball(ballA, &physA);
        if ((ballA->flags & 1) != 0 && physA.hardestColiSpeed < -0.030000000000000002) {
            if (g_some_frame_counter.frameCounter < (s32)(lbl_80198B38 * 4)) {
                fn_14_12B24(0x69, &ballA->pos);
            } else if (physA.hardestColiSpeed < -0.10200000000000001) {
                fn_14_12B24(0x2F, &ballA->pos);
            } else if (physA.hardestColiSpeed < -0.07800000000000001) {
                fn_14_12B24(0x14, &ballA->pos);
            } else if (physA.hardestColiSpeed < -0.054000000000000006) {
                fn_14_12B24(0x13, &ballA->pos);
            } else {
                fn_14_12B24(0x12, &ballA->pos);
            }
        }
        if ((ballA->flags & 0x80000000000LL) == 0 && physA.hardestColiSpeed < -0.060000000000000005) {
            g_rumble_controller(plA->unk1, 1, 0.3 * lbl_80198B34);
        }
        ballA->unk80++;
        i++;
        ballA++;
        plA++;
    }
    current_ball = prev;
}

void fn_14_22024(struct Ball *ball, struct BoatPlayer *pl, struct PhysicsBall *phys)
{
    phys->flags = 0;
    phys->pos = ball->pos;
    phys->prevPos = ball->prevPos;
    phys->vel = ball->vel;
    phys->radius = pl->unk3B4;
    phys->gravityAccel = ball->accel;
    phys->restitution = ball->restitution;
    phys->hardestColiSpeed = 0.0f;
    phys->itemGroupId = 0;
    phys->hardestColiItemGroupId = 0;
    phys->friction = lbl_1_data_657B0;
}

void fn_14_220C0(Vec *normal, Vec *velA, float massA, Vec *velB, float massB)
{
    Vec a;
    Vec b;
    Vec n;
    float dot;
    float dot2;
    float impulseA;
    float impulseB;

    a = *velA;
    b = *velB;
    n = *normal;
    dot = mathutil_vec_dot_prod(&n, &a);
    dot2 = mathutil_vec_dot_prod(&n, &b);
    impulseA = 0.7 * ((dot2 - dot) * ((massA * massB) / (massA * massB + massA * massA)));
    impulseB = impulseA * -(massA / massB);
    a.x += impulseA * n.x;
    a.y += impulseA * n.y;
    a.z += impulseA * n.z;
    b.x += impulseB * n.x;
    b.y += impulseB * n.y;
    b.z += impulseB * n.z;
    *velA = a;
    *velB = b;
}

void fn_14_22254(struct PhysicsBall *ball, struct StageDef *stagedef)
{
    float restitution;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    s32 i;
    s16 *cellTris;

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++) {
        if (i != ball->itemGroupId) {
            tf_physball_to_itemgroup_space(ball, i);
        }
        cellTris = tri_coli_grid_lookup(coll, ball->prevPos.x, ball->prevPos.z);
        if (cellTris != NULL) {
            s16 *tris = cellTris;
            while (*tris >= 0) {
                struct StageColiTri *tri = &coll->triangles[*tris];
                if ((tri->flags & 0x10) == 0) {
                    fn_14_224A8(ball, tri);
                }
                tris++;
            }
        }
    }
    restitution = ball->restitution;
    ball->restitution = 0.0f;
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++) {
        if (i != ball->itemGroupId) {
            tf_physball_to_itemgroup_space(ball, i);
        }
        cellTris = tri_coli_grid_lookup(coll, ball->prevPos.x, ball->prevPos.z);
        if (cellTris != NULL) {
            s16 *tris = cellTris;
            while (*tris >= 0) {
                struct StageColiTri *tri = &coll->triangles[*tris];
                if ((tri->flags & 0x10) == 0) {
                    collide_ball_with_tri_edges(ball, tri);
                }
                tris++;
            }
        }
    }
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++) {
        if (i != ball->itemGroupId) {
            tf_physball_to_itemgroup_space(ball, i);
        }
        cellTris = tri_coli_grid_lookup(coll, ball->prevPos.x, ball->prevPos.z);
        if (cellTris != NULL) {
            s16 *tris = cellTris;
            while (*tris >= 0) {
                struct StageColiTri *tri = &coll->triangles[*tris];
                if ((tri->flags & 0x10) == 0) {
                    collide_ball_with_tri_vertices(ball, tri);
                }
                tris++;
            }
        }
    }
    ball->restitution = restitution;
    if (ball->itemGroupId != 0) {
        tf_physball_to_itemgroup_space(ball, 0);
    }
}

void fn_14_224A8(struct PhysicsBall *ball, struct StageColiTri *tri)
{
    struct ColiPlane plane;
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float dx;
    float dy;
    Vec pos;
    Vec prevPos;

    prevPos = ball->prevPos;
    pos = ball->pos;

    x = prevPos.x - tri->pos.x;
    y = prevPos.y - tri->pos.y;
    z = prevPos.z - tri->pos.z;
    if (x * tri->normal.x + y * tri->normal.y + z * tri->normal.z < 0.0)
        return;
    x = pos.x - tri->pos.x;
    y = pos.y - tri->pos.y;
    z = pos.z - tri->pos.z;
    if (x * tri->normal.x + y * tri->normal.y + z * tri->normal.z > ball->radius)
        return;

    mtxa_from_translate(&tri->pos);
    mtxa_rotate_y(tri->rot.y);
    mtxa_rotate_x(tri->rot.x);
    mtxa_rotate_z(tri->rot.z);
    mtxa_rigid_inv_tf_point(&prevPos, &prevPos);
    mtxa_rigid_inv_tf_point(&pos, &pos);

    if (!((dumb_dot(0, 1, prevPos.x, prevPos.y) < -FLT_EPSILON
           || dumb_dot(tri->edge2Normal.x, tri->edge2Normal.y, prevPos.x - tri->vert2.x, prevPos.y - tri->vert2.y) < -FLT_EPSILON
           || dumb_dot(tri->edge3Normal.x, tri->edge3Normal.y, prevPos.x - tri->vert3.x, prevPos.y - tri->vert3.y) < -FLT_EPSILON)
          && (dumb_dot(0, 1, pos.x, pos.y) < -FLT_EPSILON
           || dumb_dot(tri->edge2Normal.x, tri->edge2Normal.y, pos.x - tri->vert2.x, pos.y - tri->vert2.y) < -FLT_EPSILON
           || dumb_dot(tri->edge3Normal.x, tri->edge3Normal.y, pos.x - tri->vert3.x, pos.y - tri->vert3.y) < -FLT_EPSILON)))
    {
        plane.point = tri->pos;
        plane.normal = tri->normal;
        fn_14_22760(ball, &plane);
    }
}

void fn_14_22760(struct PhysicsBall *ball, struct ColiPlane *coliPlane)
{
    float speed;
    Vec coliNormal;
    float penetrationDist;
    u32 isHardestColi;
    float y;
    float x;
    Vec ballPos;
    float z;
    Vec ballVel;

    ballPos = ball->pos;
    x = ballPos.x - coliPlane->point.x;
    y = ballPos.y - coliPlane->point.y;
    z = ballPos.z - coliPlane->point.z;
    speed = x * coliPlane->normal.x + y * coliPlane->normal.y
              + z * coliPlane->normal.z;
    if (speed > ball->radius)
        return;

    isHardestColi = -1;
    if (!(ball->flags & 1))
        isHardestColi = 0;

    penetrationDist = ball->radius - speed;
    ballPos.x += coliPlane->normal.x * penetrationDist;
    ballPos.y += coliPlane->normal.y * penetrationDist;
    ballPos.z += coliPlane->normal.z * penetrationDist;
    ball->pos = ballPos;

    coliNormal = coliPlane->normal;
    ballVel.x = ball->vel.x;
    ballVel.y = ball->vel.y;
    ballVel.z = ball->vel.z;
    speed = coliNormal.x * ballVel.x + coliNormal.y * ballVel.y + coliNormal.z * ballVel.z;

    if (speed < 0.0 && ball->restitution > FLT_EPSILON)
    {
        if (speed < ball->hardestColiSpeed)
        {
            ball->hardestColiSpeed = speed;
            ball->hardestColiItemGroupId = ball->itemGroupId;
            isHardestColi = 0;
        }

        x = ballVel.x - coliNormal.x * speed;
        y = ballVel.y - coliNormal.y * speed;
        z = ballVel.z - coliNormal.z * speed;
        ballVel.x -= x * ball->friction;
        if (ballVel.y > 0.0)
            ballVel.y -= 10.0 * (y * ball->friction);
        else
            ballVel.y -= y * ball->friction;
        ballVel.z -= z * ball->friction;

        if (speed >= -5.0 * ball->gravityAccel)
        {
            ballVel.x -= coliNormal.x * speed;
            ballVel.y -= coliNormal.y * speed;
            ballVel.z -= coliNormal.z * speed;
        }
        else
        {
            speed += 5.0 * ball->gravityAccel;
            ballVel.x -= coliNormal.x * (-5.0 * ball->gravityAccel);
            ballVel.y -= coliNormal.y * (-5.0 * ball->gravityAccel);
            ballVel.z -= coliNormal.z * (-5.0 * ball->gravityAccel);
            ballVel.x -= (1.0 + ball->restitution) * (coliNormal.x * speed);
            ballVel.y -= (1.0 + ball->restitution) * (coliNormal.y * speed);
            ballVel.z -= (1.0 + ball->restitution) * (coliNormal.z * speed);
        }

        ball->vel.x = ballVel.x;
        ball->vel.y = ballVel.y;
        ball->vel.z = ballVel.z;
    }

    if (isHardestColi == 0)
        *(struct ColiPlane *)&ball->hardestColiPoint = *coliPlane;

    ball->flags |= 1;
}

