/* Ball vs. stage collision (SMB1 stcoli.c), per item group with a bounding-sphere cull. */
#include "types.h"
#include "macros.h"
#include "mathutil.h"
#include "ball.h"
#include "dip.h"
#include "stage.h"
#include "stcoli.h"
#include "stobj.h"
#include "avdisp.h"
#include "camera.h"



int g_some_printf_function_2(int unused, char *fmt, ...);

s16 *tri_coli_grid_lookup(struct StageCollHeader *coll, float x, float z);
void collide_ball_with_tri_face(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_tri_edges(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_tri_vertices(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_tri_edge(struct PhysicsBall *ball, Vec *ballPos, Vec *ballPrevPos, struct ColiEdge *edge);
void collide_ball_with_tri_vertex(struct PhysicsBall *ball, Vec *ballPos, Vec2d *vert);
void collide_ball_with_cylinder(struct PhysicsBall *ball, struct StageColiCylinder *cylinder);
void collide_ball_with_circle(struct PhysicsBall *ball, struct ColiCircle *circle);
void collide_ball_with_sphere(struct PhysicsBall *ball, struct StageColiSphere *sphere);
void collide_ball_with_cone(struct PhysicsBall *ball, struct StageColiCone *cone);
void collide_ball_with_goal(struct PhysicsBall *ball, struct StageGoal *goal);
void collide_ball_with_plane(struct PhysicsBall *ball, struct ColiPlane *plane);
void g_some_jamabar_coli_func(struct PhysicsBall *ball, struct ColiRect *rect);
void tf_physicsball_by_mtxa(struct PhysicsBall *src, struct PhysicsBall *dest);
void inv_tf_physicsball_by_mtxa(struct PhysicsBall *src, struct PhysicsBall *dest);
void stcoli_sub25(struct PhysicsBall *ball, struct DynamicStagePart *dynStageParts);
BOOL g_is_ball_in_ig_coli_range(struct ItemGroup *ig, struct StageCollHeader *coll, int igId, struct Sphere *sphere);
BOOL g_ball_ig_bound_sphere_overlap(struct ItemGroup *ig, struct StageCollHeader *coll, struct Sphere *sphere);

void collide_ball_with_stage(struct PhysicsBall *ball, struct StageDef *stagedef)
{
    int j;
    s16 *cellTris;
    s16 *cellTriIdx;
    struct StageCollHeader *coll;
    struct StageColiCylinder *cylinder;
    struct StageColiSphere *coliSphere;
    struct StageGoal *goal;
    struct Sphere sphere;
    int i;
    struct ItemGroup *ig;
    struct StageColiTri tri;
    struct StageColiCone *cone;

    if (dip_switches & DIP_TRIANGLE)
    {
        tri.pos.x = 0.0f;
        tri.pos.y = 0.0f;
        tri.pos.z = 0.0f;
        tri.normal.x = 0.0f;
        tri.normal.y = 1.0f;
        tri.normal.z = 0.0f;
        tri.rot.x = -0x4000;
        tri.rot.y = 0;
        tri.rot.z = 0;
        tri.vert2.x = 10.0f;
        tri.vert2.y = 0.0f;
        tri.vert3.x = 0.0f;
        tri.vert3.y = 10.0f;
        tri.edge2Normal.x = -0.7071f;
        tri.edge2Normal.y = -0.7071f;
        tri.edge3Normal.x = 1.0f;
        tri.edge3Normal.y = 0.0f;
        ball->itemGroupId = 0;
        collide_ball_with_tri_face(ball, &tri);
        collide_ball_with_tri_edges(ball, &tri);
        collide_ball_with_tri_vertices(ball, &tri);
        return;
    }

    sphere.pos = ball->pos;
    sphere.radius = ball->radius;

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (!g_is_ball_in_ig_coli_range(ig, coll, i, &sphere))
            continue;
        if (i != ball->itemGroupId)
            tf_physball_to_itemgroup_space(ball, i);
        cellTris = tri_coli_grid_lookup(coll, ball->pos.x, ball->pos.z);
        if (cellTris != NULL)
        {
            for (cellTriIdx = cellTris; *cellTriIdx >= 0; cellTriIdx++)
                collide_ball_with_tri_face(ball, &coll->triangles[*cellTriIdx]);
        }
    }

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (!g_is_ball_in_ig_coli_range(ig, coll, i, &sphere))
            continue;
        if (i != ball->itemGroupId)
            tf_physball_to_itemgroup_space(ball, i);
        cellTris = tri_coli_grid_lookup(coll, ball->pos.x, ball->pos.z);
        if (cellTris != NULL)
        {
            for (cellTriIdx = cellTris; *cellTriIdx >= 0; cellTriIdx++)
                collide_ball_with_tri_edges(ball, &coll->triangles[*cellTriIdx]);
        }
    }

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (!g_is_ball_in_ig_coli_range(ig, coll, i, &sphere))
            continue;
        if (i != ball->itemGroupId)
            tf_physball_to_itemgroup_space(ball, i);
        cellTris = tri_coli_grid_lookup(coll, ball->pos.x, ball->pos.z);
        if (cellTris != NULL)
        {
            for (cellTriIdx = cellTris; *cellTriIdx >= 0; cellTriIdx++)
                collide_ball_with_tri_vertices(ball, &coll->triangles[*cellTriIdx]);
        }
    }

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (coll->coliCones == NULL && coll->coliSpheres == NULL && coll->coliCylinders == NULL
         && coll->goals == NULL)
            continue;
        if (!g_is_ball_in_ig_coli_range(ig, coll, i, &sphere))
            continue;
        if ((g_some_frame_counter.flags & (1 << 1))
         && (coll->unkA6 & (1 << (g_some_frame_counter.unkC - 1))) != 0)
            continue;
        if (!g_ball_ig_bound_sphere_overlap(ig, coll, &sphere))
            continue;
        if (i != ball->itemGroupId)
            tf_physball_to_itemgroup_space(ball, i);

        cone = coll->coliCones;
        for (j = coll->coliConeCount; j > 0; j--, cone++)
            collide_ball_with_cone(ball, cone);

        coliSphere = coll->coliSpheres;
        for (j = coll->coliSphereCount; j > 0; j--, coliSphere++)
            collide_ball_with_sphere(ball, coliSphere);

        cylinder = coll->coliCylinders;
        for (j = coll->coliCylinderCount; j > 0; j--, cylinder++)
            collide_ball_with_cylinder(ball, cylinder);

        goal = coll->goals;
        for (j = coll->goalCount; j > 0; j--, goal++)
            collide_ball_with_goal(ball, goal);
    }

    if (ball->itemGroupId != 0)
        tf_physball_to_itemgroup_space(ball, 0);
    if (g_some_draw_var != NULL)
        stcoli_sub25(ball, g_some_draw_var);
}

s16 *tri_coli_grid_lookup(struct StageCollHeader *coll, float x, float z)
{
    int cellX;
    int cellZ;

    if (coll->gridCellTris == NULL)
        return NULL;

    cellX = mathutil_floor((x - coll->gridOriginX) / coll->gridStepX);
    cellZ = mathutil_floor((z - coll->gridOriginZ) / coll->gridStepZ);

    if (cellX < 0 || cellX >= coll->gridCellCountX)
        return NULL;
    if (cellZ < 0 || cellZ >= coll->gridCellCountZ)
        return NULL;
    return coll->gridCellTris[cellZ * coll->gridCellCountX + cellX];
}

static inline float dumb_dot_r(float y2, float x2, float y1, float x1)
{
    return x1 * x2 + y1 * y2;
}
#define dumb_dot(a, b, c, d) dumb_dot_r(d, c, b, a)

#pragma auto_inline off
void collide_ball_with_tri_face(struct PhysicsBall *ball, struct StageColiTri *tri)
{
    struct ColiPlane plane;
    float x;
    float y;
    float z;
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
        plane.flags = tri->flags;
        collide_ball_with_plane(ball, &plane);
    }
}
#pragma auto_inline reset

void collide_ball_with_tri_edges(struct PhysicsBall *ball, struct StageColiTri *tri)
{
    float x;
    struct ColiEdge edge;
    float z;
    float y;
    float ny;
    Vec pos;
    float nx;
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

    nx = 0;
    ny = 1;
    if (dumb_dot(nx, ny, pos.x, pos.y) < -ball->radius
     && dumb_dot(nx, ny, prevPos.x, prevPos.y) < -ball->radius)
        return;

    nx = tri->edge2Normal.x;
    ny = tri->edge2Normal.y;
    if (dumb_dot(nx, ny, pos.x - tri->vert2.x, pos.y - tri->vert2.y) < -ball->radius
     && dumb_dot(nx, ny, prevPos.x - tri->vert2.x, prevPos.y - tri->vert2.y) < -ball->radius)
        return;

    nx = tri->edge3Normal.x;
    ny = tri->edge3Normal.y;
    if (dumb_dot(nx, ny, pos.x - tri->vert3.x, pos.y - tri->vert3.y) < -ball->radius
     && dumb_dot(nx, ny, prevPos.x - tri->vert3.x, prevPos.y - tri->vert3.y) < -ball->radius)
        return;

    edge.start.x = 0;
    edge.start.y = 0;
    edge.end.x = tri->vert2.x;
    edge.end.y = tri->vert2.y;
    edge.normal.x = 0;
    edge.normal.y = 1;
    collide_ball_with_tri_edge(ball, &pos, &prevPos, &edge);

    edge.start.x = tri->vert2.x;
    edge.start.y = tri->vert2.y;
    edge.end.x = tri->vert3.x;
    edge.end.y = tri->vert3.y;
    edge.normal.x = tri->edge2Normal.x;
    edge.normal.y = tri->edge2Normal.y;
    collide_ball_with_tri_edge(ball, &pos, &prevPos, &edge);

    edge.start.x = tri->vert3.x;
    edge.start.y = tri->vert3.y;
    edge.end.x = 0;
    edge.end.y = 0;
    edge.normal.x = tri->edge3Normal.x;
    edge.normal.y = tri->edge3Normal.y;
    collide_ball_with_tri_edge(ball, &pos, &prevPos, &edge);
}

void collide_ball_with_tri_edge(struct PhysicsBall *ball, Vec *ballPos, Vec *ballPrevPos, struct ColiEdge *edge)
{
    float lenSq;
    float invLen;
    float halfChord;
    float hitX;
    float y;
    struct ColiPlane plane;
    Vec prevPosRtEdge;
    Vec posRtEdge;
    Vec edgeEnd;
    Vec vec;

    mtxa_push();
    mtxa_from_identity();
    mtxa_translate_xyz(edge->start.x, edge->start.y, 0);
    mtxa_rotate_z(-(s16)math_atan2(edge->normal.x, edge->normal.y));
    mtxa_rigid_inv_tf_point(ballPrevPos, &prevPosRtEdge);
    mtxa_rigid_inv_tf_point(ballPos, &posRtEdge);
    edgeEnd.x = edge->end.x;
    edgeEnd.y = edge->end.y;
    edgeEnd.z = 0;
    mtxa_rigid_inv_tf_point(&edgeEnd, &edgeEnd);

    vec.y = posRtEdge.y - prevPosRtEdge.y;
    vec.z = posRtEdge.z - prevPosRtEdge.z;
    mtxa_rotate_x(-(s16)math_atan2(vec.y, vec.z) - 0x8000);
    mtxa_rigid_inv_tf_point(ballPos, &posRtEdge);
    mtxa_rigid_inv_tf_point(ballPrevPos, &prevPosRtEdge);

    y = posRtEdge.y;
    if (fabs(y) > ball->radius)
    {
        mtxa_pop();
        return;
    }
    halfChord = math_sqrt(ball->radius * ball->radius - y * y);
    if (fabs(posRtEdge.z) > halfChord)
    {
        mtxa_pop();
        return;
    }
    if (prevPosRtEdge.z < 0.0)
        halfChord = -halfChord;
    vec.x = 0;
    vec.y = y;
    vec.z = halfChord;
    if (prevPosRtEdge.z > halfChord && posRtEdge.z < halfChord)
        hitX = prevPosRtEdge.x
             + (posRtEdge.x - prevPosRtEdge.x)
             * ((halfChord - prevPosRtEdge.z) / (posRtEdge.z - prevPosRtEdge.z));
    else
        hitX = posRtEdge.x;

    if (hitX < 0.0 || hitX > edgeEnd.x)
    {
        mtxa_pop();
        return;
    }
    lenSq = mathutil_sum_of_sq_2(vec.y, vec.z);
    if (lenSq <= FLT_EPSILON)
    {
        mtxa_pop();
        return;
    }

    invLen = math_rsqrt(lenSq);
    vec.y *= invLen;
    vec.z *= invLen;
    mtxa_tf_vec(&vec, &plane.normal);
    mathutil_mtxA_get_translate(&plane.point);
    mtxa_pop();
    mtxa_tf_vec(&plane.normal, &plane.normal);
    mtxa_tf_point(&plane.point, &plane.point);
    collide_ball_with_plane(ball, &plane);
}

void collide_ball_with_tri_vertices(struct PhysicsBall *ball, struct StageColiTri *tri)
{
    Vec pos;
    Vec prevPos;
    Vec2d vert;
    float x;
    float y;
    float z;

    prevPos = ball->prevPos;
    x = prevPos.x - tri->pos.x;
    y = prevPos.y - tri->pos.y;
    z = prevPos.z - tri->pos.z;
    if (x * tri->normal.x + y * tri->normal.y + z * tri->normal.z < 0.0)
        return;

    pos = ball->pos;
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

    vert.x = 0;
    vert.y = 0;
    collide_ball_with_tri_vertex(ball, &pos, &vert);
    vert.x = tri->vert2.x;
    vert.y = tri->vert2.y;
    collide_ball_with_tri_vertex(ball, &pos, &vert);
    vert.x = tri->vert3.x;
    vert.y = tri->vert3.y;
    collide_ball_with_tri_vertex(ball, &pos, &vert);
}

void collide_ball_with_tri_vertex(struct PhysicsBall *ball, Vec *ballPos, Vec2d *vert)
{
    struct ColiPlane plane;
    Vec tmpVec;
    float distSq;
    float invDist;

    tmpVec.x = ballPos->x - vert->x;
    tmpVec.y = ballPos->y - vert->y;
    tmpVec.z = ballPos->z;
    distSq = mathutil_sum_of_sq_3(tmpVec.x, tmpVec.y, tmpVec.z);
    if (!(distSq > ball->radius * ball->radius) && !(distSq <= FLT_EPSILON))
    {
        invDist = math_rsqrt(distSq);
        tmpVec.x *= invDist;
        tmpVec.y *= invDist;
        tmpVec.z *= invDist;
        mtxa_tf_vec(&tmpVec, &plane.normal);

        tmpVec.x = vert->x;
        tmpVec.y = vert->y;
        tmpVec.z = 0;
        mtxa_tf_point(&tmpVec, &plane.point);

        mtxa_push();
        collide_ball_with_plane(ball, &plane);
        mtxa_pop();
    }
}

/* Pushes the ball out of the plane and applies friction and restitution to its velocity.
 * Every other ball-vs-stage collision function ends up here. */
void g_some_jamabar_coli_func(struct PhysicsBall *ball, struct ColiRect *rect)
{
    struct ColiPlane plane;
    Vec pos;
    Vec prevPos;
    Vec tmpVec;
    Vec pos_rt_rect;
    float ballEdgeDistSq;
    float invBallEdgeDist;
    float halfWidth;
    float halfHeight;

    prevPos = ball->prevPos;
    tmpVec.x = prevPos.x - rect->pos.x;
    tmpVec.y = prevPos.y - rect->pos.y;
    tmpVec.z = prevPos.z - rect->pos.z;
    if (tmpVec.z * rect->normal.z + (tmpVec.x * rect->normal.x + tmpVec.y * rect->normal.y) < 0.0)
        return;

    pos = ball->pos;
    tmpVec.x = pos.x - rect->pos.x;
    tmpVec.y = pos.y - rect->pos.y;
    tmpVec.z = pos.z - rect->pos.z;
    if (tmpVec.z * rect->normal.z + (tmpVec.x * rect->normal.x + tmpVec.y * rect->normal.y) > ball->radius)
        return;

    mtxa_from_translate(&rect->pos);
    mtxa_rotate_y(math_atan2(rect->normal.x, rect->normal.z));
    mtxa_rotate_x(-math_atan2(rect->normal.y, math_sqrt(mathutil_sum_of_sq_2(rect->normal.x, rect->normal.z))));
    mtxa_rigid_inv_tf_point(&pos, &pos_rt_rect);

    halfWidth = 0.5 * rect->width;
    halfHeight = 0.5 * rect->height;

    if (pos_rt_rect.x < -halfWidth - ball->radius)
        return;
    if (pos_rt_rect.x > halfWidth + ball->radius)
        return;
    if (pos_rt_rect.y < -halfHeight - ball->radius)
        return;
    if (pos_rt_rect.y > halfHeight + ball->radius)
        return;

    if (pos_rt_rect.x < -halfWidth)
    {
        tmpVec.x = pos_rt_rect.x + halfWidth;
        tmpVec.z = pos_rt_rect.z;
        ballEdgeDistSq = mathutil_sum_of_sq_2(tmpVec.x, tmpVec.z);
        if (!(ballEdgeDistSq > (ball->radius * ball->radius)))
        {
            if (ballEdgeDistSq > FLT_EPSILON)
            {
                invBallEdgeDist = math_rsqrt(ballEdgeDistSq);
                tmpVec.x *= invBallEdgeDist;
                tmpVec.y = 0;
                tmpVec.z *= invBallEdgeDist;
            }
            else
            {
                tmpVec.x = -1;
                tmpVec.y = 0;
                tmpVec.z = 0;
            }
            mtxa_tf_vec(&tmpVec, &plane.normal);
            tmpVec.x = -halfWidth;
            tmpVec.y = 0;
            tmpVec.z = 0;
            mtxa_tf_point(&tmpVec, &plane.point);
            collide_ball_with_plane(ball, &plane);
        }
    }
    else if (pos_rt_rect.x > halfWidth)
    {
        tmpVec.x = pos_rt_rect.x - halfWidth;
        tmpVec.z = pos_rt_rect.z;
        ballEdgeDistSq = mathutil_sum_of_sq_2(tmpVec.x, tmpVec.z);
        if (!(ballEdgeDistSq > (ball->radius * ball->radius)))
        {
            if (ballEdgeDistSq > FLT_EPSILON)
            {
                invBallEdgeDist = math_rsqrt(ballEdgeDistSq);
                tmpVec.x *= invBallEdgeDist;
                tmpVec.y = 0;
                tmpVec.z *= invBallEdgeDist;
            }
            else
            {
                tmpVec.x = 1;
                tmpVec.y = 0;
                tmpVec.z = 0;
            }
            mtxa_tf_vec(&tmpVec, &plane.normal);
            tmpVec.x = halfWidth;
            tmpVec.y = 0;
            tmpVec.z = 0;
            mtxa_tf_point(&tmpVec, &plane.point);
            collide_ball_with_plane(ball, &plane);
        }
    }
    else if (pos_rt_rect.y < -halfHeight)
    {
        tmpVec.y = pos_rt_rect.y + halfHeight;
        tmpVec.z = pos_rt_rect.z;
        ballEdgeDistSq = mathutil_sum_of_sq_2(tmpVec.y, tmpVec.z);
        if (!(ballEdgeDistSq > (ball->radius * ball->radius)))
        {
            if (ballEdgeDistSq > FLT_EPSILON)
            {
                invBallEdgeDist = math_rsqrt(ballEdgeDistSq);
                tmpVec.x = 0;
                tmpVec.y *= invBallEdgeDist;
                tmpVec.z *= invBallEdgeDist;
            }
            else
            {
                tmpVec.x = 0;
                tmpVec.y = -1;
                tmpVec.z = 0;
            }
            mtxa_tf_vec(&tmpVec, &plane.normal);
            tmpVec.x = 0;
            tmpVec.y = -halfHeight;
            tmpVec.z = 0;
            mtxa_tf_point(&tmpVec, &plane.point);
            collide_ball_with_plane(ball, &plane);
        }
    }
    else if (pos_rt_rect.y > halfHeight)
    {
        tmpVec.y = pos_rt_rect.y - halfHeight;
        tmpVec.z = pos_rt_rect.z;
        ballEdgeDistSq = mathutil_sum_of_sq_2(tmpVec.y, tmpVec.z);
        if (!(ballEdgeDistSq > (ball->radius * ball->radius)))
        {
            if (ballEdgeDistSq > FLT_EPSILON)
            {
                invBallEdgeDist = math_rsqrt(ballEdgeDistSq);
                tmpVec.x = 0;
                tmpVec.y *= invBallEdgeDist;
                tmpVec.z *= invBallEdgeDist;
            }
            else
            {
                tmpVec.x = 0;
                tmpVec.y = 1;
                tmpVec.z = 0;
            }
            mtxa_tf_vec(&tmpVec, &plane.normal);
            tmpVec.x = 0;
            tmpVec.y = halfHeight;
            tmpVec.z = 0;
            mtxa_tf_point(&tmpVec, &plane.point);
            collide_ball_with_plane(ball, &plane);
        }
    }
    else if (pos_rt_rect.x < -halfWidth || pos_rt_rect.x > halfWidth)
        return;
    else if (pos_rt_rect.y < -halfHeight || pos_rt_rect.y > halfHeight)
        return;
    else
    {
        plane.point = rect->pos;
        plane.normal = rect->normal;
        plane.flags = rect->flags;
        collide_ball_with_plane(ball, &plane);
    }
}

void collide_ball_with_cylinder(struct PhysicsBall *ball, struct StageColiCylinder *cylinder)
{
    struct ColiPlane plane;
    struct ColiCircle topCircle;
    struct ColiCircle bottomCircle;
    Vec ballPos;
    Vec ballPrevPos;
    Vec tmpVec;
    float distSq;
    float radius;
    float invDist;

    tmpVec.x = ball->pos.x - cylinder->pos.x;
    tmpVec.y = ball->pos.y - cylinder->pos.y;
    tmpVec.z = ball->pos.z - cylinder->pos.z;
    distSq = mathutil_sum_of_sq_3(tmpVec.x, tmpVec.y, tmpVec.z);
    radius = ball->radius + math_sqrt(mathutil_sum_of_sq_2(cylinder->radius, cylinder->height));
    if (distSq > radius * radius)
        return;

    mtxa_from_translate(&cylinder->pos);
    mtxa_rotate_z(cylinder->rot.z);
    mtxa_rotate_y(cylinder->rot.y);
    mtxa_rotate_x(cylinder->rot.x);
    mtxa_rigid_inv_tf_point(&ball->pos, &ballPos);
    mtxa_rigid_inv_tf_point(&ball->prevPos, &ballPrevPos);
    distSq = mathutil_sum_of_sq_2(ballPos.x, ballPos.z);
    radius = cylinder->radius + ball->radius;
    if (distSq > radius * radius)
        return;

    if (ballPrevPos.y < 0.5 * -cylinder->height)
    {
        topCircle.pos.x = 0.0f;
        topCircle.pos.y = 0.5 * -cylinder->height;
        topCircle.pos.z = 0.0f;
        mtxa_tf_point(&topCircle.pos, &topCircle.pos);
        topCircle.radius = cylinder->radius;
        topCircle.rot.x = cylinder->rot.x - 0x8000;
        topCircle.rot.y = cylinder->rot.y;
        topCircle.rot.z = cylinder->rot.z;
        collide_ball_with_circle(ball, &topCircle);
        return;
    }
    if (ballPrevPos.y > 0.5 * cylinder->height)
    {
        bottomCircle.pos.x = 0.0f;
        bottomCircle.pos.y = 0.5 * cylinder->height;
        bottomCircle.pos.z = 0.0f;
        mtxa_tf_point(&bottomCircle.pos, &bottomCircle.pos);
        bottomCircle.radius = cylinder->radius;
        bottomCircle.rot.x = cylinder->rot.x;
        bottomCircle.rot.y = cylinder->rot.y;
        bottomCircle.rot.z = cylinder->rot.z;
        collide_ball_with_circle(ball, &bottomCircle);
        return;
    }

    invDist = math_rsqrt(distSq);
    plane.normal.x = ballPos.x * invDist;
    plane.normal.y = 0.0f;
    plane.normal.z = ballPos.z * invDist;
    plane.point.x = plane.normal.x * cylinder->radius;
    plane.point.y = ballPos.y + plane.normal.y * cylinder->radius;
    plane.point.z = plane.normal.z * cylinder->radius;
    mtxa_tf_vec(&plane.normal, &plane.normal);
    mtxa_tf_point(&plane.point, &plane.point);
    collide_ball_with_plane(ball, &plane);
}

void collide_ball_with_circle(struct PhysicsBall *ball, struct ColiCircle *circle)
{
    struct ColiPlane plane;
    Vec ballPos;
    Vec tmpVec;
    float distSq;
    float scale;

    tmpVec.x = ball->pos.x - circle->pos.x;
    tmpVec.y = ball->pos.y - circle->pos.y;
    tmpVec.z = ball->pos.z - circle->pos.z;
    distSq = mathutil_sum_of_sq_3(tmpVec.x, tmpVec.y, tmpVec.z);
    if (distSq > (ball->radius + circle->radius) * (ball->radius + circle->radius))
        return;

    mtxa_from_translate(&circle->pos);
    mtxa_rotate_z(circle->rot.z);
    mtxa_rotate_y(circle->rot.y);
    mtxa_rotate_x(circle->rot.x);
    mtxa_rigid_inv_tf_point(&ball->pos, &ballPos);
    distSq = mathutil_sum_of_sq_2(ballPos.x, ballPos.z);
    if (distSq < circle->radius * circle->radius)
    {
        mathutil_mtxA_get_translate(&plane.point);
        plane.normal.x = 0.0f;
        plane.normal.y = 1.0f;
        plane.normal.z = 0.0f;
        mtxa_tf_vec(&plane.normal, &plane.normal);
        collide_ball_with_plane(ball, &plane);
        return;
    }

    if (distSq < (ball->radius + circle->radius) * (ball->radius + circle->radius) && distSq > FLT_EPSILON)
    {
        scale = circle->radius * math_rsqrt(distSq);
        plane.point.x = ballPos.x * scale;
        plane.point.y = 0.0f;
        plane.point.z = ballPos.z * scale;
        tmpVec.x = ballPos.x - plane.point.x;
        tmpVec.y = ballPos.y - plane.point.y;
        tmpVec.z = ballPos.z - plane.point.z;
        distSq = mathutil_sum_of_sq_3(tmpVec.x, tmpVec.y, tmpVec.z);
        if (distSq > FLT_EPSILON)
        {
            scale = math_rsqrt(distSq);
            plane.normal.x = tmpVec.x * scale;
            plane.normal.y = tmpVec.y * scale;
            plane.normal.z = tmpVec.z * scale;
            mtxa_tf_point(&plane.point, &plane.point);
            mtxa_tf_vec(&plane.normal, &plane.normal);
            collide_ball_with_plane(ball, &plane);
        }
    }
}

void collide_ball_with_sphere(struct PhysicsBall *ball, struct StageColiSphere *sphere)
{
    struct ColiPlane plane;
    Vec tmpVec;
    float invDist;
    float distSq;

    tmpVec.x = ball->pos.x - sphere->pos.x;
    tmpVec.y = ball->pos.y - sphere->pos.y;
    tmpVec.z = ball->pos.z - sphere->pos.z;
    distSq = mathutil_sum_of_sq_3(tmpVec.x, tmpVec.y, tmpVec.z);
    if (distSq > (sphere->radius + ball->radius) * (sphere->radius + ball->radius))
        return;
    if (distSq <= FLT_EPSILON)
        return;

    invDist = math_rsqrt(distSq);
    tmpVec.x *= invDist;
    tmpVec.y *= invDist;
    tmpVec.z *= invDist;
    plane.point.x = sphere->pos.x + tmpVec.x * sphere->radius;
    plane.point.y = sphere->pos.y + tmpVec.y * sphere->radius;
    plane.point.z = sphere->pos.z + tmpVec.z * sphere->radius;
    plane.normal.x = tmpVec.x;
    plane.normal.y = tmpVec.y;
    plane.normal.z = tmpVec.z;
    collide_ball_with_plane(ball, &plane);
}

void collide_ball_with_cone(struct PhysicsBall *ball, struct StageColiCone *cone)
{
    struct ColiPlane plane;
    Vec tmpVec;
    Vec ballPos;
    float distSq;
    float maxScale;
    float invLen;

    tmpVec.x = ball->pos.x - cone->pos.x;
    tmpVec.y = ball->pos.y - cone->pos.y;
    tmpVec.z = ball->pos.z - cone->pos.z;
    distSq = mathutil_sum_of_sq_3(tmpVec.x, tmpVec.y, tmpVec.z);
    maxScale = (cone->scale.x > cone->scale.y) ? cone->scale.x : cone->scale.y;
    maxScale += ball->radius;
    if (distSq > maxScale * maxScale)
        return;

    mtxa_from_translate(&cone->pos);
    mtxa_rotate_z(cone->rot.z);
    mtxa_rotate_y(cone->rot.y);
    mtxa_rotate_x(cone->rot.x);
    mtxa_rigid_inv_tf_point(&ball->pos, &ballPos);
    if (ballPos.y < -ball->radius)
        return;
    if (ballPos.y > cone->scale.y + ball->radius)
        return;
    distSq = mathutil_sum_of_sq_2(ballPos.x, ballPos.z);
    if (distSq > (cone->scale.x + ball->radius) * (cone->scale.x + ball->radius))
        return;

    mtxa_translate_xyz(0.0f, cone->scale.y, 0.0f);
    mtxa_rotate_y(-math_atan2(ballPos.z, ballPos.x));
    mtxa_rotate_z(-math_atan2(cone->scale.y, cone->scale.x));
    mtxa_rigid_inv_tf_point(&ball->pos, &tmpVec);

    if (tmpVec.x < 0.0)
    {
        mathutil_mtxA_get_translate(&plane.point);
        distSq = mathutil_sum_of_sq_3(tmpVec.x, tmpVec.y, tmpVec.z);
        if (distSq <= FLT_EPSILON)
            return;
        invLen = 1.0 / math_sqrt(distSq);
        tmpVec.x *= invLen;
        tmpVec.y *= invLen;
        tmpVec.z *= invLen;
        mtxa_tf_vec(&tmpVec, &plane.normal);
        collide_ball_with_plane(ball, &plane);
    }
    else
    {
        invLen = math_sqrt(mathutil_sum_of_sq_2(cone->scale.x, cone->scale.y));
        if (tmpVec.x > ball->radius + invLen)
            return;
        mathutil_mtxA_get_translate(&plane.point);
        tmpVec.x = 0.0f;
        tmpVec.y = 1.0f;
        tmpVec.z = 0.0f;
        mtxa_tf_vec(&tmpVec, &plane.normal);
        collide_ball_with_plane(ball, &plane);
    }
}

void collide_ball_with_plane(struct PhysicsBall *ball, struct ColiPlane *coliPlane)
{
    float adjustedBallSpeed;
    Vec coliNormal;
    float penetrationDist;
    u32 isHardestColi;
    float planeDeltaY;
    float parallelVelX;
    Vec ballPos;
    float planeDeltaZ;
    float planeDist;
    float parallelVelY;
    Vec ballVel;
    float planeDeltaX;
    float normalSpeed;
    float parallelVelZ;

    ballPos = ball->pos;
    planeDeltaX = ballPos.x - coliPlane->point.x;
    planeDeltaY = ballPos.y - coliPlane->point.y;
    planeDeltaZ = ballPos.z - coliPlane->point.z;
    planeDist = planeDeltaX * coliPlane->normal.x + planeDeltaY * coliPlane->normal.y
              + planeDeltaZ * coliPlane->normal.z;
    if (planeDist > ball->radius)
        return;

    if (coliPlane->flags & (1 << 1))
        ball->flags |= (1 << 1);

    isHardestColi = -1;
    if (!(ball->flags & 1))
        isHardestColi = 0;

    penetrationDist = ball->radius - planeDist;
    ballPos.x += coliPlane->normal.x * penetrationDist;
    ballPos.y += coliPlane->normal.y * penetrationDist;
    ballPos.z += coliPlane->normal.z * penetrationDist;
    ball->pos = ballPos;

    coliNormal = coliPlane->normal;
    ballVel.x = ball->vel.x;
    ballVel.y = ball->vel.y;
    ballVel.z = ball->vel.z;
    normalSpeed = coliNormal.x * ballVel.x + coliNormal.y * ballVel.y + coliNormal.z * ballVel.z;

    if (normalSpeed < -FLT_EPSILON && ball->restitution > FLT_EPSILON)
    {
        if (normalSpeed < ball->hardestColiSpeed)
        {
            ball->hardestColiSpeed = normalSpeed;
            ball->hardestColiItemGroupId = ball->itemGroupId;
            isHardestColi = 0;
        }

        /* friction only on the first contact of the frame */
        if (!(ball->flags & 1))
        {
            parallelVelX = ballVel.x - coliNormal.x * normalSpeed;
            parallelVelY = ballVel.y - coliNormal.y * normalSpeed;
            parallelVelZ = ballVel.z - coliNormal.z * normalSpeed;
            ballVel.x -= parallelVelX * ball->friction;
            ballVel.y -= parallelVelY * ball->friction;
            ballVel.z -= parallelVelZ * ball->friction;
        }

        if (normalSpeed >= -5.0 * ball->gravityAccel)
        {
            ballVel.x -= coliNormal.x * normalSpeed;
            ballVel.y -= coliNormal.y * normalSpeed;
            ballVel.z -= coliNormal.z * normalSpeed;
        }
        else
        {
            adjustedBallSpeed = normalSpeed + 5.0 * ball->gravityAccel;
            ballVel.x -= coliNormal.x * (-5.0 * ball->gravityAccel);
            ballVel.y -= coliNormal.y * (-5.0 * ball->gravityAccel);
            ballVel.z -= coliNormal.z * (-5.0 * ball->gravityAccel);
            ballVel.x -= (1.0 + ball->restitution) * (coliNormal.x * adjustedBallSpeed);
            ballVel.y -= (1.0 + ball->restitution) * (coliNormal.y * adjustedBallSpeed);
            ballVel.z -= (1.0 + ball->restitution) * (coliNormal.z * adjustedBallSpeed);
        }

        ball->vel.x = ballVel.x;
        ball->vel.y = ballVel.y;
        ball->vel.z = ballVel.z;
    }

    if (isHardestColi == 0)
        *(struct ColiPlane *)&ball->hardestColiPoint = *coliPlane;

    ball->flags |= 1;
}

BOOL line_intersects_rect(Vec *lineStart, Vec *lineEnd, struct ColiRect *rect)
{
    Vec start;
    Vec end;
    Vec planePoint;
    float halfWidth;
    float halfHeight;
    BOOL intersects;

    start = *lineStart;
    end = *lineEnd;

    mtxa_from_translate(&rect->pos);
    mtxa_rotate_z(rect->rot.z);
    mtxa_rotate_y(rect->rot.y);
    mtxa_rotate_x(rect->rot.x);
    mtxa_rigid_inv_tf_point(&start, &start);
    mtxa_rigid_inv_tf_point(&end, &end);

    if ((end.z < 0.0 && start.z < 0.0) || (end.z > 0.0 && start.z > 0.0))
        intersects = FALSE;
    else
    {
        planePoint.x = start.x - end.x;
        planePoint.y = start.y - end.y;
        planePoint.z = start.z - end.z;

        if (planePoint.z > FLT_EPSILON)
        {
            planePoint.x = end.x - planePoint.x * (end.z / planePoint.z);
            planePoint.y = end.y - planePoint.y * (end.z / planePoint.z);
        }
        else
        {
            planePoint.x = end.x;
            planePoint.y = end.y;
        }

        halfWidth = 0.5 * rect->width;
        halfHeight = 0.5 * rect->height;
        if (planePoint.x < -halfWidth || planePoint.x > halfWidth)
            intersects = FALSE;
        else if (planePoint.y < -halfHeight || planePoint.y > halfHeight)
            intersects = FALSE;
        else
            intersects = TRUE;
    }
    return intersects;
}

static struct ColiRect jamabarColiRects[] =
{
    { {  0.0f, 0.5f,  1.75f }, { 0,       0, 0 }, 0, {  0.0f, 0.0f,  1.0f }, 1.0f, 1.0f },
    { { -0.5f, 0.5f,  0.0f  }, { 0, -0x4000, 0 }, 0, { -1.0f, 0.0f,  0.0f }, 3.5f, 1.0f },
    { {  0.0f, 0.5f, -1.75f }, { 0, -0x8000, 0 }, 0, {  0.0f, 0.0f, -1.0f }, 1.0f, 1.0f },
    { {  0.5f, 0.5f,  0.0f  }, { 0,  0x4000, 0 }, 0, {  1.0f, 0.0f,  0.0f }, 3.5f, 1.0f },
};

void stobj_jamabar_child_coli(struct PhysicsBall *ball, struct Stobj *stobj)
{
    Vec ballPos;
    float dz;
    float prevVel;
    struct ColiRect *coliRect;
    int i;

    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_z(stobj->rotZ);
    inv_tf_physicsball_by_mtxa(ball, ball);
    ballPos = ball->pos;

    coliRect = jamabarColiRects;
    for (i = 0; i < 4; i++, coliRect++)
        g_some_jamabar_coli_func(ball, coliRect);

    dz = 0.75 * (ball->pos.z - ballPos.z);
    stobj->u_local_pos.z -= dz;
    ball->pos.z += dz;
    if (dz * stobj->u_local_vel.z > 0.0)
    {
        prevVel = stobj->u_local_vel.z;
        stobj->u_local_vel.z *= 0.5;
        ball->vel.z = ball->vel.z + 2.5 * (prevVel - stobj->u_local_vel.z);
    }

    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_z(stobj->rotZ);
    tf_physicsball_by_mtxa(ball, ball);
}

int raycast_tri(Vec *rayOrigin, Vec *rayDir, struct StageColiTri *tri);
int raycast_cone(Vec *rayOrigin, Vec *rayDir, struct StageColiCone *cone, Vec *outHitPos, Vec *outHitNormal);
int raycast_sphere(Vec *rayOrigin, Vec *rayDir, struct StageColiSphere *sphere, Vec *outHitPos, Vec *outHitNormal);
int raycast_cylinder(Vec *rayOrigin, Vec *rayDir, struct StageColiCylinder *cylinder, Vec *outHitPos, Vec *outHitNormal);

u32 raycast_stage_down(Vec *rayOrigin, struct RaycastHit *outHit, Vec *outVelAtPoint)
{
    struct Sphere sphere;
    Vec rayOrigin_rt_ig;
    Vec hitPos;
    Vec rayDir;
    Vec coneHitNormal;
    Vec sphereHitNormal;
    Vec cylinderHitNormal;
    Vec dynStagePartHitNormal;
    Vec currHitPos;
    Vec prevHitPos;
    struct ItemGroup *ig;
    struct StageColiTri *tri;
    struct StageCollHeader *coll;
    s16 *cellTris;
    s16 *cellTriIdx;
    struct StageColiCone *cone;
    struct StageColiSphere *coliSphere;
    struct StageColiCylinder *cylinder;
    int j;
    int i;
    int hitIgId;

    outHit->flags = 0;
    hitIgId = 0;
    sphere.pos = *rayOrigin;
    sphere.radius = 0.0f;
    if (!stagedef)
        return 0;

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        rayOrigin_rt_ig.x = rayOrigin->x;
        rayOrigin_rt_ig.y = rayOrigin->y;
        rayOrigin_rt_ig.z = rayOrigin->z;
        rayDir.x = 0.0f;
        rayDir.y = -1.0f;
        rayDir.z = 0.0f;
        if ((g_some_frame_counter.flags & (1 << 1))
         && (coll->unkA6 & (1 << (g_some_frame_counter.unkC - 1))) != 0)
            continue;
        if (i > 0)
        {
            mtxa_from_mtx(ig->transform);
            mtxa_rigid_inv_tf_point(&rayOrigin_rt_ig, &rayOrigin_rt_ig);
            mtxa_rigid_inv_tf_vec(&rayDir, &rayDir);
        }

        cellTris = tri_coli_grid_lookup(coll, rayOrigin_rt_ig.x, rayOrigin_rt_ig.z);
        if (cellTris != NULL)
        {
            for (cellTriIdx = cellTris; *cellTriIdx >= 0; cellTriIdx++)
            {
                hitPos.x = rayOrigin_rt_ig.x;
                hitPos.y = rayOrigin_rt_ig.y;
                hitPos.z = rayOrigin_rt_ig.z;
                tri = &coll->triangles[*cellTriIdx];
                if (raycast_tri(&hitPos, &rayDir, tri))
                {
                    mtxa_from_mtx(ig->transform);
                    mtxa_tf_point(&hitPos, &hitPos);
                    if (!(outHit->flags & 1) || hitPos.y > outHit->pos.y)
                    {
                        outHit->flags = tri->flags | 1;
                        outHit->pos.x = hitPos.x;
                        outHit->pos.y = hitPos.y;
                        outHit->pos.z = hitPos.z;
                        mtxa_tf_vec(&tri->normal, &outHit->normal);
                        hitIgId = i;
                    }
                }
            }
        }

        cone = coll->coliCones;
        for (j = coll->coliConeCount; j > 0; j--, cone++)
        {
            if (raycast_cone(&rayOrigin_rt_ig, &rayDir, cone, &hitPos, &coneHitNormal))
            {
                mtxa_from_mtx(ig->transform);
                mtxa_tf_point(&hitPos, &hitPos);
                if (!(outHit->flags & 1) || hitPos.y > outHit->pos.y)
                {
                    outHit->flags = cone->flags | 1;
                    outHit->pos.x = hitPos.x;
                    outHit->pos.y = hitPos.y;
                    outHit->pos.z = hitPos.z;
                    mtxa_tf_vec(&coneHitNormal, &outHit->normal);
                    hitIgId = i;
                }
            }
        }

        coliSphere = coll->coliSpheres;
        for (j = coll->coliSphereCount; j > 0; j--, coliSphere++)
        {
            if (raycast_sphere(&rayOrigin_rt_ig, &rayDir, coliSphere, &hitPos, &sphereHitNormal))
            {
                mtxa_from_mtx(ig->transform);
                mtxa_tf_point(&hitPos, &hitPos);
                if (!(outHit->flags & 1) || hitPos.y > outHit->pos.y)
                {
                    outHit->flags = coliSphere->flags | 1;
                    outHit->pos.x = hitPos.x;
                    outHit->pos.y = hitPos.y;
                    outHit->pos.z = hitPos.z;
                    mtxa_tf_vec(&sphereHitNormal, &outHit->normal);
                    hitIgId = i;
                }
            }
        }

        cylinder = coll->coliCylinders;
        for (j = coll->coliCylinderCount; j > 0; j--, cylinder++)
        {
            if (raycast_cylinder(&rayOrigin_rt_ig, &rayDir, cylinder, &hitPos, &cylinderHitNormal))
            {
                mtxa_from_mtx(ig->transform);
                mtxa_tf_point(&hitPos, &hitPos);
                if (!(outHit->flags & 1) || hitPos.y > outHit->pos.y)
                {
                    outHit->flags = cylinder->flags | 1;
                    outHit->pos.x = hitPos.x;
                    outHit->pos.y = hitPos.y;
                    outHit->pos.z = hitPos.z;
                    mtxa_tf_vec(&cylinderHitNormal, &outHit->normal);
                    hitIgId = i;
                }
            }
        }
    }

    if (g_some_draw_var)
    {
        hitPos.x = rayOrigin_rt_ig.x;
        hitPos.y = rayOrigin_rt_ig.y;
        hitPos.z = rayOrigin_rt_ig.z;
        if (g_some_draw_var[0].raycastDownFunc(&hitPos, &hitPos, &dynStagePartHitNormal))
        {
            if (!(outHit->flags & 1) || hitPos.y > outHit->pos.y)
            {
                outHit->flags = 1;
                outHit->pos.x = hitPos.x;
                outHit->pos.y = hitPos.y;
                outHit->pos.z = hitPos.z;
                outHit->normal.x = dynStagePartHitNormal.x;
                outHit->normal.y = dynStagePartHitNormal.y;
                outHit->normal.z = dynStagePartHitNormal.z;
                hitIgId = 0;
            }
        }
    }

    if (outVelAtPoint)
    {
        if ((outHit->flags & 1) && hitIgId > 0)
        {
            ig = &itemgroups[hitIgId];
            currHitPos.x = outHit->pos.x;
            currHitPos.y = outHit->pos.y;
            currHitPos.z = outHit->pos.z;
            mtxa_from_mtx(ig->transform);
            mtxa_rigid_inv_tf_point(&currHitPos, &hitPos);
            mtxa_from_mtx(ig->prevTransform);
            mtxa_tf_point(&hitPos, &prevHitPos);
            outVelAtPoint->x = currHitPos.x - prevHitPos.x;
            outVelAtPoint->y = currHitPos.y - prevHitPos.y;
            outVelAtPoint->z = currHitPos.z - prevHitPos.z;
        }
        else
        {
            outVelAtPoint->x = 0.0f;
            outVelAtPoint->y = 0.0f;
            outVelAtPoint->z = 0.0f;
        }
    }

    return (outHit->flags & 1) != 0;
}

static inline float edge_dot(float nx, float ny, float x, float y)
{
    return nx * x + ny * y;
}

int raycast_tri(Vec *rayOrigin, Vec *rayDir, struct StageColiTri *tri)
{
    Vec rayOrigin_rt_tri;
    Vec rayDir_rt_tri;
    float scale;
    float x;
    float y;
    float z;

    // If ray origin is not in front of triangle, no hit
    x = rayOrigin->x - tri->pos.x;
    y = rayOrigin->y - tri->pos.y;
    z = rayOrigin->z - tri->pos.z;
    if (z * tri->normal.z + (x * tri->normal.x + y * tri->normal.y) < 0.0)
        return 0;

    // If ray direction is not towards triangle, no hit
    if (rayDir->z * tri->normal.z + (rayDir->x * tri->normal.x + rayDir->y * tri->normal.y) > 0.0)
        return 0;

    // Transform ray origin and direction into triangle space
    mtxa_from_translate(&tri->pos);
    mtxa_rotate_y(tri->rot.y);
    mtxa_rotate_x(tri->rot.x);
    mtxa_rotate_z(tri->rot.z);
    mtxa_rigid_inv_tf_point(rayOrigin, &rayOrigin_rt_tri);
    mtxa_rigid_inv_tf_vec(rayDir, &rayDir_rt_tri);

    // If ray is nearly parallel to plane, no hit
    if (fabs(rayDir_rt_tri.z) <= FLT_EPSILON)
        return 0;

    // Find the ray-plane intersection point
    scale = -rayOrigin_rt_tri.z / rayDir_rt_tri.z;
    rayOrigin_rt_tri.x += rayDir_rt_tri.x * scale;
    rayOrigin_rt_tri.y += rayDir_rt_tri.y * scale;
    rayOrigin_rt_tri.z = 0.0f;

    // Check if ray-plane point is on the inner side of each triangle edge
    if (/*V*/edge_dot(0, 1, rayOrigin_rt_tri.x, rayOrigin_rt_tri.y) < -0.01)
        return 0;
    if (dumb_dot(tri->edge2Normal.x, tri->edge2Normal.y,
                 rayOrigin_rt_tri.x - tri->vert2.x, rayOrigin_rt_tri.y - tri->vert2.y) < -0.01)
        return 0;
    if (dumb_dot(tri->edge3Normal.x, tri->edge3Normal.y,
                 rayOrigin_rt_tri.x - tri->vert3.x, rayOrigin_rt_tri.y - tri->vert3.y) < -0.01)
        return 0;

    mtxa_tf_point(&rayOrigin_rt_tri, rayOrigin);
    return 1;
}

int raycast_cone(Vec *rayOrigin, Vec *rayDir, struct StageColiCone *cone, Vec *outHitPos, Vec *outHitNormal)
{
    Vec rayOrigin_rt_cone;
    Vec hitPos;
    Vec rayDir_rt_cone;
    float yRadiusRatio;
    float invLen;
    float a;
    float b;
    float c;
    float discrim;
    float t;

    mtxa_from_translate(&cone->pos);
    mtxa_rotate_z(cone->rot.z);
    mtxa_rotate_y(cone->rot.y);
    mtxa_rotate_x(cone->rot.x);
    mtxa_translate_xyz(0.0f, cone->scale.y, 0.0f);
    mtxa_rigid_inv_tf_point(rayOrigin, &rayOrigin_rt_cone);
    mtxa_rigid_inv_tf_vec(rayDir, &rayDir_rt_cone);

    yRadiusRatio = cone->scale.x / cone->scale.y;

    a = rayDir_rt_cone.x * rayDir_rt_cone.x
      + rayDir_rt_cone.z * rayDir_rt_cone.z
      - yRadiusRatio * yRadiusRatio * (rayDir_rt_cone.y * rayDir_rt_cone.y);
    if (fabs(a) < FLT_EPSILON)
        return FALSE;

    b = rayDir_rt_cone.x * rayOrigin_rt_cone.x
      + rayDir_rt_cone.z * rayOrigin_rt_cone.z
      - rayOrigin_rt_cone.y * (rayDir_rt_cone.y * (yRadiusRatio * yRadiusRatio));
    c = rayOrigin_rt_cone.x * rayOrigin_rt_cone.x
      + rayOrigin_rt_cone.z * rayOrigin_rt_cone.z
      - yRadiusRatio * yRadiusRatio * (rayOrigin_rt_cone.y * rayOrigin_rt_cone.y);
    discrim = b * b - a * c;
    if (discrim < FLT_EPSILON)
        return FALSE;

    t = -(b + math_sqrt(discrim)) / a;
    hitPos.x = rayOrigin_rt_cone.x + rayDir_rt_cone.x * t;
    hitPos.y = rayOrigin_rt_cone.y + rayDir_rt_cone.y * t;
    hitPos.z = rayOrigin_rt_cone.z + rayDir_rt_cone.z * t;
    if (hitPos.y > 0.0 || hitPos.y < -cone->scale.y)
    {
        t = -(b - math_sqrt(discrim)) / a;
        hitPos.x = rayOrigin_rt_cone.x + rayDir_rt_cone.x * t;
        hitPos.y = rayOrigin_rt_cone.y + rayDir_rt_cone.y * t;
        hitPos.z = rayOrigin_rt_cone.z + rayDir_rt_cone.z * t;
        if (hitPos.y > 0.0 || hitPos.y < -cone->scale.y)
            return FALSE;
    }

    mtxa_tf_point(&hitPos, outHitPos);
    rayDir_rt_cone.x = hitPos.x;
    rayDir_rt_cone.z = hitPos.z;
    discrim = mathutil_sum_of_sq_2(rayDir_rt_cone.x, rayDir_rt_cone.z);
    if (discrim > FLT_EPSILON)
    {
        rayDir_rt_cone.y = cone->scale.x * math_sqrt(discrim) / cone->scale.y;
        discrim += rayDir_rt_cone.y * rayDir_rt_cone.y;
        invLen = math_rsqrt(discrim);
        rayDir_rt_cone.x *= invLen;
        rayDir_rt_cone.y *= invLen;
        rayDir_rt_cone.z *= invLen;
        mtxa_tf_vec(&rayDir_rt_cone, outHitNormal);
    }
    else
    {
        rayDir_rt_cone.x = 0.0f;
        rayDir_rt_cone.y = 1.0f;
        rayDir_rt_cone.z = 0.0f;
        mtxa_tf_vec(&rayDir_rt_cone, outHitNormal);
    }
    return TRUE;
}

int raycast_sphere(Vec *rayOrigin, Vec *rayDir, struct StageColiSphere *sphere, Vec *outHitPos, Vec *outHitNormal)
{
    Vec delta;
    Vec dir;
    float dirLenSq;
    float dot;
    float c;
    float discrim;
    float t;

    dir.x = rayDir->x;
    dir.y = rayDir->y;
    dir.z = rayDir->z;
    delta.x = rayOrigin->x - sphere->pos.x;
    delta.y = rayOrigin->y - sphere->pos.y;
    delta.z = rayOrigin->z - sphere->pos.z;
    dirLenSq = mathutil_sum_of_sq_3(dir.x, dir.y, dir.z);
    if (dirLenSq < FLT_EPSILON)
        return 0;
    dot = dir.z * delta.z + (dir.x * delta.x + dir.y * delta.y);
    c = mathutil_sum_of_sq_3(delta.x, delta.y, delta.z) - sphere->radius * sphere->radius;
    discrim = dot * dot - dirLenSq * c;
    if (discrim < FLT_EPSILON)
        return 0;
    if (c < 0.0f)
        return 0;
    if (dot > 0.0f)
        return 0;
    t = -(dot + math_sqrt(discrim)) / dirLenSq;
    outHitPos->x = rayOrigin->x + dir.x * t;
    outHitPos->y = rayOrigin->y + dir.y * t;
    outHitPos->z = rayOrigin->z + dir.z * t;
    outHitNormal->x = (outHitPos->x - sphere->pos.x) / sphere->radius;
    outHitNormal->y = (outHitPos->y - sphere->pos.y) / sphere->radius;
    outHitNormal->z = (outHitPos->z - sphere->pos.z) / sphere->radius;
    return 1;
}

int raycast_cylinder(Vec *rayOrigin, Vec *rayDir, struct StageColiCylinder *cylinder, Vec *outHitPos, Vec *outHitNormal)
{
    Vec hitPos;
    Vec rayOrigin_rt_cyl;
    Vec rayDir_rt_cyl;
    float discrim;
    float c;
    float a;
    float b;
    float t;
    float capY;

    mtxa_from_translate(&cylinder->pos);
    mtxa_rotate_z(cylinder->rot.z);
    mtxa_rotate_y(cylinder->rot.y);
    mtxa_rotate_x(cylinder->rot.x);
    mtxa_rigid_inv_tf_point(rayOrigin, &rayOrigin_rt_cyl);
    mtxa_rigid_inv_tf_vec(rayDir, &rayDir_rt_cyl);

    // Test against the cylinder's side
    a = rayDir_rt_cyl.x * rayDir_rt_cyl.x + rayDir_rt_cyl.z * rayDir_rt_cyl.z;
    if (!(a < FLT_EPSILON))
    {
        b = rayDir_rt_cyl.x * rayOrigin_rt_cyl.x + rayDir_rt_cyl.z * rayOrigin_rt_cyl.z;
        c = rayOrigin_rt_cyl.x * rayOrigin_rt_cyl.x + rayOrigin_rt_cyl.z * rayOrigin_rt_cyl.z
          - cylinder->radius * cylinder->radius;
        discrim = b * b - a * c;
        if (discrim < FLT_EPSILON)
            return 0;
        if (!(c < 0.0f))
        {
            if (b > 0.0f)
                return 0;
            t = -(b + math_sqrt(discrim)) / a;
            hitPos.x = rayOrigin_rt_cyl.x + rayDir_rt_cyl.x * t;
            hitPos.y = rayOrigin_rt_cyl.y + rayDir_rt_cyl.y * t;
            hitPos.z = rayOrigin_rt_cyl.z + rayDir_rt_cyl.z * t;
            if (!(fabs(hitPos.y) > 0.5 * cylinder->height))
            {
                mtxa_tf_point(&hitPos, outHitPos);
                outHitNormal->x = hitPos.x / cylinder->radius;
                outHitNormal->y = 0.0f;
                outHitNormal->z = hitPos.z / cylinder->radius;
                mtxa_tf_vec(outHitNormal, outHitNormal);
                return 1;
            }
        }
    }

    // Test against the top cap
    if (rayDir_rt_cyl.y < FLT_EPSILON)
    {
        capY = 0.5 * cylinder->height;
        if (rayOrigin_rt_cyl.y < capY)
            return 0;
        t = (capY - rayOrigin_rt_cyl.y) / rayDir_rt_cyl.y;
        hitPos.x = rayOrigin_rt_cyl.x + rayDir_rt_cyl.x * t;
        hitPos.y = capY;
        hitPos.z = rayOrigin_rt_cyl.z + rayDir_rt_cyl.z * t;
        if (mathutil_sum_of_sq_2(hitPos.x, hitPos.z) > cylinder->radius * cylinder->radius)
            return 0;
        mtxa_tf_point(&hitPos, outHitPos);
        outHitNormal->x = 0.0f;
        outHitNormal->y = 1.0f;
        outHitNormal->z = 0.0f;
        mtxa_tf_vec(outHitNormal, outHitNormal);
        return 1;
    }
    // Test against the bottom cap
    if (rayDir_rt_cyl.y > FLT_EPSILON)
    {
        capY = 0.5 * -cylinder->height;
        if (rayOrigin_rt_cyl.y > capY)
            return 0;
        t = (capY - rayOrigin_rt_cyl.y) / rayDir_rt_cyl.y;
        hitPos.x = rayOrigin_rt_cyl.x + rayDir_rt_cyl.x * t;
        hitPos.y = capY;
        hitPos.z = rayOrigin_rt_cyl.z + rayDir_rt_cyl.z * t;
        if (mathutil_sum_of_sq_2(hitPos.x, hitPos.z) > cylinder->radius * cylinder->radius)
            return 0;
        mtxa_tf_point(&hitPos, outHitPos);
        outHitNormal->x = 0.0f;
        outHitNormal->y = 1.0f;
        outHitNormal->z = 0.0f;
        mtxa_tf_vec(outHitNormal, outHitNormal);
        return 1;
    }
    return 0;
}

/* Ring-shaped collider (the goal's arch), centered at pos and lying in its local XY plane */
struct ColiRing
{
    /*0x00*/ Vec pos;
    /*0x0C*/ float radius;      /* radius of the ring */
    /*0x10*/ float thickness;   /* radius of the ring's tube */
    /*0x14*/ s16 rotX;
    /*0x16*/ s16 rotY;
    /*0x18*/ s16 rotZ;
};  /* size = 0x1C */

/* Parameter order chosen for evaluation order; ABI-identical to (Vec *, Vec *, Vec *, Vec *, float, float). */
int g_some_item_collision_check(Vec *prevPos, Vec *pos, float radius, Vec *a, Vec *b, float c);
void g_goal_coli_func1(struct PhysicsBall *ball, struct ColiRing *ring);
void g_goal_coli_func2(struct PhysicsBall *ball, struct ColiRing *ring);

#define GOAL_MODEL_ID 41

void collide_ball_with_goal(struct PhysicsBall *ball, struct StageGoal *goal)
{
    struct StageColiCylinder cylinder;
    Vec pos;
    struct ColiRing ring1;
    struct ColiRing ring2;
    float radius;

    mtxa_from_translate(&goal->pos);
    mtxa_rotate_z(goal->rotZ);
    mtxa_rotate_y(goal->rotY);
    mtxa_rotate_x(goal->rotX);
    radius = init_common_gma->modelEntries[GOAL_MODEL_ID].model->boundSphereRadius;
    mtxa_tf_point(&init_common_gma->modelEntries[GOAL_MODEL_ID].model->boundSphereCenter, &pos);
    if (mathutil_vec_distance(&pos, &ball->pos) > radius)
        return;

    mtxa_push();
    pos.x = 0.0f;
    pos.y = 3.2f;
    pos.z = 0.0f;
    mtxa_tf_point(&pos, &ring1.pos);
    ring1.radius = 2.25f;
    ring1.thickness = 0.1f;
    ring1.rotX = goal->rotX;
    ring1.rotY = goal->rotY;
    ring1.rotZ = goal->rotZ;
    g_goal_coli_func1(ball, &ring1);
    mtxa_pop();

    mtxa_push();
    pos.x = 0.0f;
    pos.y = 3.2f;
    pos.z = 0.0f;
    mtxa_tf_point(&pos, &ring2.pos);
    ring2.radius = 2.25f;
    ring2.thickness = 0.1f;
    ring2.rotX = goal->rotX;
    ring2.rotY = goal->rotY;
    ring2.rotZ = goal->rotZ;
    g_goal_coli_func2(ball, &ring2);
    mtxa_pop();

    mtxa_push();
    pos.x = 1.1f;
    pos.y = 0.75f;
    pos.z = 0.0f;
    mtxa_tf_point(&pos, &cylinder.pos);
    cylinder.rot.x = goal->rotX;
    cylinder.rot.y = goal->rotY;
    cylinder.rot.z = goal->rotZ;
    cylinder.radius = 0.2f;
    cylinder.height = 1.5f;
    collide_ball_with_cylinder(ball, &cylinder);
    mtxa_pop();

    mtxa_push();
    pos.x = -1.1f;
    pos.y = 0.75f;
    pos.z = 0.0f;
    mtxa_tf_point(&pos, &cylinder.pos);
    cylinder.rot.x = goal->rotX;
    cylinder.rot.y = goal->rotY;
    cylinder.rot.z = goal->rotZ;
    cylinder.radius = 0.2f;
    cylinder.height = 1.5f;
    collide_ball_with_cylinder(ball, &cylinder);
    mtxa_pop();
}

static inline void ring_outer_coli(struct PhysicsBall *ball, struct ColiRing *ring)
{
    Vec normal;
    Vec closest;
    Vec pos;
    Vec prevPos;
    struct ColiPlane plane;
    float maxDist = ball->radius + (ring->radius + ring->thickness);
    float minDist = ring->radius - ring->thickness - ball->radius;
    float dist = mathutil_vec_distance(&ball->pos, &ring->pos);

    if (dist > maxDist || dist < minDist)
        return;

    mtxa_from_translate(&ring->pos);
    mtxa_rotate_z(ring->rotZ);
    mtxa_rotate_y(ring->rotY);
    mtxa_rotate_x(ring->rotX);
    mtxa_rigid_inv_tf_point(&ball->pos, &pos);
    mtxa_rigid_inv_tf_point(&ball->prevPos, &prevPos);
    closest.x = pos.x + prevPos.x;
    closest.y = pos.y + prevPos.y;
    closest.z = 0.0f;
    vec_set_len(ring->radius, &closest, &closest);
    if (g_some_item_collision_check(&prevPos, &pos, ball->radius, &closest, &closest, ring->thickness))
    {
        normal.x = prevPos.x - closest.x;
        normal.y = prevPos.y - closest.y;
        normal.z = prevPos.z - closest.z;
        vec_normalize_len(&normal);
        mtxa_tf_vec(&normal, &plane.normal);

        maxDist = ring->thickness;
        closest.x += normal.x * maxDist;
        closest.y += normal.y * maxDist;
        closest.z += normal.z * maxDist;
        mtxa_tf_point(&closest, &plane.point);

        collide_ball_with_plane(ball, &plane);
    }
}

// Collides with the ring, except for the part of its underside facing the ball
void g_goal_coli_func1(struct PhysicsBall *ball, struct ColiRing *ring)
{
    Vec down;
    Vec dir;

    down.x = 0.0f;
    down.y = -1.0f;
    down.z = 0.0f;
    mtxa_tf_vec(&down, &down);
    dir.x = ball->pos.x - ring->pos.x;
    dir.y = ball->pos.y - ring->pos.y;
    dir.z = ball->pos.z - ring->pos.z;
    vec_normalize_len(&dir);
    if (!(mathutil_vec_dot_prod(&down, &dir) > 0.7732404444))
        ring_outer_coli(ball, ring);
}

// Collides with the inner part of the ring
void g_goal_coli_func2(struct PhysicsBall *ball, struct ColiRing *ring)
{
    Vec normal;
    Vec closest;
    Vec pos;
    Vec prevPos;
    struct ColiPlane plane;
    float maxDist;
    float len;
    float scale;

    maxDist = ball->radius + (ring->radius + ring->thickness);
    if (mathutil_vec_distance(&ball->pos, &ring->pos) > maxDist)
        return;

    mtxa_from_translate(&ring->pos);
    mtxa_rotate_z(ring->rotZ);
    mtxa_rotate_y(ring->rotY);
    mtxa_rotate_x(ring->rotX);
    mtxa_rigid_inv_tf_point(&ball->pos, &pos);
    mtxa_rigid_inv_tf_point(&ball->prevPos, &prevPos);
    closest.x = 0.5 * (pos.x + prevPos.x);
    closest.y = 0.5 * (pos.y + prevPos.y);
    closest.z = 0.0f;
    if (closest.y < -0.5f)
        closest.y = -0.5f;
    len = math_sqrt(mathutil_sum_of_sq_2(closest.x, closest.y));
    if (len > ring->radius)
    {
        scale = ring->radius / len;
        closest.x *= scale;
        closest.y *= scale;
        closest.z *= scale;
    }
    if (g_some_item_collision_check(&prevPos, &pos, ball->radius, &closest, &closest, ring->thickness))
    {
        normal.x = prevPos.x - closest.x;
        normal.y = prevPos.y - closest.y;
        normal.z = prevPos.z - closest.z;
        vec_normalize_len(&normal);
        mtxa_tf_vec(&normal, &plane.normal);

        maxDist = ring->thickness;
        closest.x += normal.x * maxDist;
        closest.y += normal.y * maxDist;
        closest.z += normal.z * maxDist;
        mtxa_tf_point(&closest, &plane.point);

        collide_ball_with_plane(ball, &plane);
    }
}

struct RaycastSample
{
    Vec pos;
    s32 hit;
};

void stcoli_sub29(struct StageColiTri *tri, Vec *vert1, Vec *vert2, Vec *vert3);

// Collides the ball with dynamic stage parts by sampling the surface below the ball on
// a 4x4 grid and building temporary triangles from the samples.
void stcoli_sub25(struct PhysicsBall *ball, struct DynamicStagePart *dynStageParts)
{
    struct StageColiTri *tri;
    struct RaycastSample *s00;
    struct RaycastSample *s10;
    struct RaycastSample *s11;
    struct RaycastSample *s01;
    float baseX;
    float baseZ;
    int (*raycastDown)(Vec *, Vec *, Vec *);
    int j;
    int i;
    struct StageColiTri triangles[32];
    struct RaycastSample samples[16];

    raycastDown = dynStageParts[0].raycastDownFunc;
    baseX = (double)mathutil_floor(ball->pos.x - 1.0);
    baseZ = (double)mathutil_floor(ball->pos.z - 1.0);

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            s00 = &samples[i * 4 + j];
            s00->pos.x = baseX + (double)j;
            s00->pos.y = 0.0f;
            s00->pos.z = baseZ + (double)i;
            s00->hit = raycastDown(&s00->pos, &s00->pos, NULL);
        }
    }

    tri = triangles;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++, tri++)
        {
            s00 = &samples[(i + 0) * 4 + j + 0];
            s10 = &samples[(i + 1) * 4 + j + 0];
            s11 = &samples[(i + 1) * 4 + j + 1];
            s01 = &samples[(i + 0) * 4 + j + 1];
            if (s00->hit != 0 && s10->hit != 0 && s11->hit != 0)
                stcoli_sub29(tri, &s00->pos, &s10->pos, &s11->pos);
            else
                tri->flags = 0xFFFF;
            tri++;
            if (s00->hit != 0 && s11->hit != 0 && s01->hit != 0)
                stcoli_sub29(tri, &s00->pos, &s11->pos, &s01->pos);
            else
                tri->flags = 0xFFFF;
        }
    }

    tri = triangles;
    for (i = 3; i > 0; i--)
    {
        for (j = 3; j > 0; j--, tri++)
        {
            if (tri->flags == 0)
                collide_ball_with_tri_face(ball, tri);
            tri++;
            if (tri->flags == 0)
                collide_ball_with_tri_face(ball, tri);
        }
    }

    tri = triangles;
    for (i = 3; i > 0; i--)
    {
        for (j = 3; j > 0; j--, tri++)
        {
            if (tri->flags == 0)
                collide_ball_with_tri_edges(ball, tri);
            tri++;
            if (tri->flags == 0)
                collide_ball_with_tri_edges(ball, tri);
        }
    }

    tri = triangles;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++, tri++)
        {
            if (tri->flags == 0)
                collide_ball_with_tri_vertices(ball, tri);
            tri++;
            if (tri->flags == 0)
                collide_ball_with_tri_vertices(ball, tri);
        }
    }
}

#define COLI_CONE_MODEL_ID 33
#define COLI_CYLINDER_MODEL_ID 34
#define COLI_SPHERE_MODEL_ID 35
#define COLI_TRIANGLE_MODEL_ID 89

void stcoli_sub27(struct DynamicStagePart *dynStageParts);
void stcoli_sub28(struct StageColiTri *tri);
void stcoli_sub29(struct StageColiTri *tri, Vec *a, Vec *b, Vec *c);

void g_draw_stage_collision(void)
{
    Mtx mtxB;
    Vec pos;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int i;
    s16 *cellTris;
    struct StageColiCone *cone;
    int j;
    struct StageColiSphere *sphere;
    struct StageColiCylinder *cylinder;
    int k;

    mtx_copy(mtxa->mtxB, mtxB);

    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (i > 0)
        {
            mtxa_from_mtx(mtxB);
            mtxa_mult_right(ig->transform);
            mtxa_to_mtx(mtxa->mtxB);
        }
        pos.x = g_current_camera->lookAt.x;
        pos.y = g_current_camera->lookAt.y;
        pos.z = g_current_camera->lookAt.z;
        if (i > 0)
        {
            mtxa_from_mtx(ig->transform);
            mtxa_rigid_inv_tf_point(&pos, &pos);
        }
        cellTris = tri_coli_grid_lookup(coll, pos.x, pos.z);
        if (cellTris != NULL)
        {
            while (*cellTris >= 0)
            {
                stcoli_sub28(&coll->triangles[*cellTris]);
                cellTris++;
            }
        }

        cone = coll->coliCones;
        for (k = coll->coliConeCount; k > 0; k--, cone++)
        {
            float scale;


            mtxa_from_mtx(mtxa->mtxB);
            mtxa_translate(&cone->pos);
            mtxa_rotate_z(cone->rot.z);
            mtxa_rotate_y(cone->rot.y);
            mtxa_rotate_x(cone->rot.x);
            mtxa_scale(&cone->scale);
            scale = MAX(cone->scale.x, cone->scale.y);
            scale = MAX(scale, cone->scale.z);
            avdisp_set_scale_factor(scale);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[COLI_CONE_MODEL_ID].model);
        }

        sphere = coll->coliSpheres;
        for (j = coll->coliSphereCount; j > 0; j--, sphere++)
        {
            mtxa_from_mtx(mtxa->mtxB);
            mtxa_translate(&sphere->pos);
            mtxa_scale_xyz(sphere->radius, sphere->radius, sphere->radius);
            avdisp_set_scale_factor(sphere->radius);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[COLI_SPHERE_MODEL_ID].model);
        }

        cylinder = coll->coliCylinders;
        for (j = coll->coliCylinderCount; j > 0; j--, cylinder++)
        {
            float scale;

            mtxa_from_mtx(mtxa->mtxB);
            mtxa_translate(&cylinder->pos);
            mtxa_rotate_z(cylinder->rot.z);
            mtxa_rotate_y(cylinder->rot.y);
            mtxa_rotate_x(cylinder->rot.x);
            mtxa_scale_xyz(cylinder->radius, cylinder->height, cylinder->radius);
            scale = MAX(cylinder->radius, cylinder->height);
            avdisp_set_scale_factor(scale);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[COLI_CYLINDER_MODEL_ID].model);
        }
    }
    mtx_copy(mtxB, mtxa->mtxB);
    if (g_some_draw_var)
        stcoli_sub27(g_some_draw_var);
}

void stcoli_sub27(struct DynamicStagePart *dynStageParts)
{
    struct RaycastSample *s00;
    struct RaycastSample *s10;
    struct RaycastSample *s11;
    struct RaycastSample *s01;
    struct Ball *ball;
    float baseX;
    float baseZ;
    int j;
    int i;
    int (*raycastDown)(Vec *, Vec *, Vec *);
    struct StageColiTri tri;
    struct RaycastSample samples[16];

    ball = current_ball;
    raycastDown = dynStageParts[0].raycastDownFunc;
    baseX = (double)mathutil_floor(ball->pos.x - 1.0);
    baseZ = (double)mathutil_floor(ball->pos.z - 1.0);

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            s00 = &samples[i * 4 + j];
            s00->pos.x = baseX + (double)j;
            s00->pos.y = 0.0f;
            s00->pos.z = baseZ + (double)i;
            s00->hit = raycastDown(&s00->pos, &s00->pos, NULL);
        }
    }

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            s00 = &samples[(i + 0) * 4 + j + 0];
            s10 = &samples[(i + 1) * 4 + j + 0];
            s11 = &samples[(i + 1) * 4 + j + 1];
            s01 = &samples[(i + 0) * 4 + j + 1];
            if (s00->hit != 0 && s10->hit != 0 && s11->hit != 0)
            {
                stcoli_sub29(&tri, &s00->pos, &s10->pos, &s11->pos);
                stcoli_sub28(&tri);
            }
            if (s00->hit != 0 && s11->hit != 0 && s01->hit != 0)
            {
                stcoli_sub29(&tri, &s00->pos, &s11->pos, &s01->pos);
                stcoli_sub28(&tri);
            }
        }
    }
}

/* Draws a collision triangle using the common triangle model, sheared onto its vertices. */
void stcoli_sub28(struct StageColiTri *tri)
{
    Vec v;
    Mtx mtx;
    float f0, f1;

    mtxa_from_translate(&tri->pos);
    mtxa_rotate_y(tri->rot.y);
    mtxa_rotate_x(tri->rot.x);
    mtxa_rotate_z(tri->rot.z);
    mtxa_to_mtx(mtx);
    v.x = tri->vert2.x;
    v.y = tri->vert2.y;
    v.z = 0.0f;
    mtxa_tf_vec(&v, &v);
    mtx[0][0] = v.x;
    mtx[1][0] = v.y;
    mtx[2][0] = v.z;
    v.x = tri->vert3.x;
    v.y = tri->vert3.y;
    v.z = 0.0f;
    mtxa_tf_vec(&v, &v);
    mtx[0][1] = v.x;
    mtx[1][1] = v.y;
    mtx[2][1] = v.z;
    mtxa_from_mtx(mtxa->mtxB);
    mtxa_mult_right(mtx);
    f0 = mathutil_sum_of_sq_2(tri->vert2.x, tri->vert2.y);
    f1 = mathutil_sum_of_sq_2(tri->vert3.x, tri->vert3.y);
    avdisp_set_scale_factor(MAX(f0, f1));
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_alpha(0.5f);
    avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[COLI_TRIANGLE_MODEL_ID].model);
}

/* Builds a collision triangle from three points. */
void stcoli_sub29(struct StageColiTri *tri, Vec *a, Vec *b, Vec *c)
{
    float lenSq;
    float x;
    float y;
    Vec normal;
    Vec tmp;
    Vec edge1;
    Vec edge2;

    edge1.x = b->x - a->x;
    edge1.y = b->y - a->y;
    edge1.z = b->z - a->z;
    edge2.x = c->x - a->x;
    edge2.y = c->y - a->y;
    edge2.z = c->z - a->z;

    mathutil_vec_cross_prod(&edge1, &edge2, &normal);
    tri->pos = *a;
    tri->rot.y = math_atan2(normal.x, normal.z);
    tri->rot.x = -math_atan2(normal.y, math_sqrt(normal.x * normal.x + normal.z * normal.z));
    mtxa_from_identity();
    mtxa_rotate_y(tri->rot.y);
    mtxa_rotate_x(tri->rot.x);
    mtxa_rigid_inv_tf_vec(&edge1, &tmp);
    tri->rot.z = math_atan2(tmp.y, tmp.x);
    mtxa_rotate_z(tri->rot.z);
    mtxa_rigid_inv_tf_point(&edge1, &edge1);
    mtxa_rigid_inv_tf_point(&edge2, &edge2);
    tri->normal.x = 0.0f;
    tri->normal.y = 0.0f;
    tri->normal.z = 1.0f;
    mtxa_tf_vec(&tri->normal, &tri->normal);
    tri->vert2.x = edge1.x;
    tri->vert2.y = edge1.y;
    tri->vert3.x = edge2.x;
    tri->vert3.y = edge2.y;
    x = -(edge2.y - edge1.y);
    y = edge2.x - edge1.x;
    lenSq = x * x + y * y;
    if (lenSq <= FLT_EPSILON)
    {
        g_some_printf_function_2(2, "edge length is too short\n");
        return;
    }
    lenSq = math_rsqrt(lenSq);
    tri->edge2Normal.x = x * lenSq;
    tri->edge2Normal.y = y * lenSq;
    x = (double)edge2.y;
    y = (double)-edge2.x;
    lenSq = x * x + y * y;
    if (lenSq <= FLT_EPSILON)
    {
        g_some_printf_function_2(2, "edge length is too short\n");
        return;
    }
    lenSq = math_rsqrt(lenSq);
    tri->edge3Normal.x = x * lenSq;
    tri->edge3Normal.y = y * lenSq;
    tri->flags = 0;
}

void tf_physicsball_by_mtxa(struct PhysicsBall *src, struct PhysicsBall *dest)
{
    mtxa_tf_point(&src->pos, &dest->pos);
    mtxa_tf_point(&src->prevPos, &dest->prevPos);
    mtxa_tf_vec(&src->vel, &dest->vel);
}

void inv_tf_physicsball_by_mtxa(struct PhysicsBall *src, struct PhysicsBall *dest)
{
    mtxa_rigid_inv_tf_point(&src->pos, &dest->pos);
    mtxa_rigid_inv_tf_point(&src->prevPos, &dest->prevPos);
    mtxa_rigid_inv_tf_vec(&src->vel, &dest->vel);
}

void tf_physball_to_itemgroup_space(struct PhysicsBall *ball, int igId)
{
    Vec move;
    struct ItemGroup *ig;

    if (ball->itemGroupId > 0)
    {
        ig = &itemgroups[ball->itemGroupId];
        move.x = ball->pos.x - ball->prevPos.x;
        move.y = ball->pos.y - ball->prevPos.y;
        move.z = ball->pos.z - ball->prevPos.z;
        mtxa_from_mtx(ig->transform);
        mtxa_tf_point(&ball->pos, &ball->pos);
        mtxa_tf_vec(&ball->vel, &ball->vel);
        mtxa_tf_vec(&move, &move);
        mtxa_from_mtx(ig->prevTransform);
        mtxa_tf_point(&ball->prevPos, &ball->prevPos);
        ball->vel.x += ball->pos.x - ball->prevPos.x - move.x;
        ball->vel.y += ball->pos.y - ball->prevPos.y - move.y;
        ball->vel.z += ball->pos.z - ball->prevPos.z - move.z;
    }
    if (igId > 0)
    {
        move.x = ball->pos.x - ball->prevPos.x;
        move.y = ball->pos.y - ball->prevPos.y;
        move.z = ball->pos.z - ball->prevPos.z;
        ig = &itemgroups[igId];
        mtxa_from_mtx(ig->transform);
        mtxa_rigid_inv_tf_point(&ball->pos, &ball->pos);
        mtxa_rigid_inv_tf_vec(&ball->vel, &ball->vel);
        mtxa_rigid_inv_tf_vec(&move, &move);
        mtxa_from_mtx(ig->prevTransform);
        mtxa_rigid_inv_tf_point(&ball->prevPos, &ball->prevPos);
        ball->vel.x += ball->pos.x - ball->prevPos.x - move.x;
        ball->vel.y += ball->pos.y - ball->prevPos.y - move.y;
        ball->vel.z += ball->pos.z - ball->prevPos.z - move.z;
    }
    ball->itemGroupId = igId;
}

int fn_1_54A60(u32 a)
{
    if (!(a & 0x8000))
        return 0;
    else
        return (a & 0x7FFE) >> 1;
}

BOOL g_is_ball_in_ig_coli_range(struct ItemGroup *ig, struct StageCollHeader *coll, int igId, struct Sphere *sphere)
{
    if ((g_some_frame_counter.flags & (1 << 1))
     && (coll->unkA6 & (1 << (g_some_frame_counter.unkC - 1))) != 0)
        return FALSE;
    return g_ball_ig_bound_sphere_overlap(ig, coll, sphere) != 0;
}

BOOL g_ball_ig_bound_sphere_overlap(struct ItemGroup *ig, struct StageCollHeader *coll, struct Sphere *sphere)
{
    if (coll->boundSphere != NULL)
    {
        float dx = ig->boundSphereCenter.x - sphere->pos.x;
        float dy = ig->boundSphereCenter.y - sphere->pos.y;
        float dz = ig->boundSphereCenter.z - sphere->pos.z;
        float r = ig->boundSphereRadius + sphere->radius;

        if (dz * dz + (dx * dx + dy * dy) > r * r)
            return FALSE;
    }
    return TRUE;
}
