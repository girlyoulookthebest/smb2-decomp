/* Monkey Fight 2: the fight cameras (.text 0x205E8-0x2301C).
 * lbl_12_data_9DD0 is the camera mode table (indexed by camera->unk1F, dispatched by fight_6.c):
 * each even mode initialises and switches to the next odd one, which runs per frame.
 *   0/1 fly-in from the stage start positions, 2/3 follow all gloves (spectator view),
 *   4/5 keep every fighter, item and stage point in view, 6/7 stage orbit (stage hooks may
 *   replace it), 8/9 the winner close-up with the camera bouncing around them. */
#include "fight.h"

/* main_loop globals not declared in the shared headers */
extern u16 lbl_1_bss_6FAE4;  /* stage view yaw */
extern u16 lbl_1_bss_6FAE6;  /* stage view pitch */
void get_curr_stage_fly_in_position(struct Sphere *sphere);
void mtxa_from_rotate_x(s16 angle);

/* What the camera keeps in view (lbl_12_bss_1EE90). */
struct FightCameraTargets
{
    /*0x00*/ Vec startCenter;  /* average of the stage start positions */
    /*0x0C*/ s32 setCount;
    /*0x10*/ struct FightStartSet *sets;  /* extra points kept in view (fn_12_205E8) */
};

/* An ape bone the winner camera aims at (lbl_12_data_9DFC). */
struct FightCameraBone
{
    /*0x00*/ s32 bone;
    /*0x04*/ Vec offset;
};

void fn_12_20604(struct Camera *camera, struct Ball *ball);
void fn_12_20688(struct Camera *camera, struct Ball *ball);
void fn_12_21134(struct Camera *camera, struct Ball *ball);
void fn_12_21420(struct Camera *camera, struct Ball *ball);
void fn_12_2170C(struct Camera *camera, struct Ball *ball);
void fn_12_2188C(struct Camera *camera, struct Ball *ball);
void fn_12_21AB8(struct Camera *camera, struct Ball *ball);
void fn_12_21B00(struct Camera *camera, struct Ball *ball);
void fn_12_22250(struct Camera *camera, struct Ball *ball);
void fn_12_22708(struct Camera *camera, struct Ball *ball);

/* .data */
void (*lbl_12_data_9DD0[])(struct Camera *, struct Ball *) = {
    fn_12_21134,
    fn_12_21420,
    fn_12_21AB8,
    fn_12_21B00,
    fn_12_20604,
    fn_12_20688,
    fn_12_2170C,
    fn_12_2188C,
    fn_12_22250,
    fn_12_22708,
    NULL,
};

struct FightCameraBone lbl_12_data_9DFC[2] = {
    {8, {0.0f, 0.0f, 0.0f}},
    {12, {0.0f, 0.0f, 0.0f}},
};

/* .bss */
struct FightCameraTargets lbl_12_bss_1EE90;
Vec lbl_12_bss_1EEA4[4];  /* per camera: smoothed view-space eye offset (mode 5) */

/* Sets the extra points the camera keeps in view. */
void fn_12_205E8(struct FightStartSet *sets, s32 count)
{
    lbl_12_bss_1EE90.setCount = count;
    lbl_12_bss_1EE90.sets = sets;
}

/* Mode 4: starts the keep-everything-in-view camera. */
void fn_12_20604(struct Camera *camera, struct Ball *ball)
{
    Vec *smooth;

    if (g_some_other_flags & 0xA)
        return;
    smooth = &lbl_12_bss_1EEA4[camera->idx];
    smooth->x = 0.0f;
    smooth->y = 0.0f;
    smooth->z = 0.0f;
    camera->unk1F = 5;
    fn_12_20688(camera, ball);
}

/* Copies the main camera (secondary cameras just mirror it). */
#define FIGHT7_COPY_MAIN_CAMERA(camera) \
    do \
    { \
        (camera)->rotY = cameras[0].rotY; \
        (camera)->rotX = cameras[0].rotX; \
        (camera)->rotZ = cameras[0].rotZ; \
        (camera)->eye = cameras[0].eye; \
        (camera)->eyeVel = cameras[0].eyeVel; \
        (camera)->lookAt = cameras[0].lookAt; \
        (camera)->lookAtVel = cameras[0].lookAtVel; \
    } while (0)

/* Mode 5: frames every fighter, item and extra point inside the view. */
void fn_12_20688(struct Camera *camera, struct Ball *ball)
{
    Vec p;
    Vec prevEye;
    Vec prevLookAt;
    Vec min;
    Vec max;
    Vec vel;
    Vec min2;
    Vec max2;
    struct Ball *b;
    Vec *smoothPtr;
    float tanX;
    float tanY;
    float r;
    float t;
    float x;
    float y;
    float z;
    int j;
    Vec smooth;
    struct Item *item;
    s8 *status;
    int i;
    struct FightPlayer *player;
    struct FightStartSet *set;
    struct FightStartPos *pos;
    int group;

    if (camera->idx != 0)
    {
        FIGHT7_COPY_MAIN_CAMERA(camera);
        return;
    }
    if (g_some_other_flags & 0xA)
        return;

    smoothPtr = &lbl_12_bss_1EEA4[camera->idx];
    prevEye = camera->eye;
    prevLookAt = camera->lookAt;
    mtxa_from_mtx(camera->unk14C);
    tanY = 0.75f * camera->sub28.view.tanHalfFov;
    tanX = tanY * camera->sub28.view.aspect;
    mtxa_tf_point(&lbl_12_bss_1EE38.flyIn.pos, &p);
    min.x = 1000000.0f + p.x;
    min.y = 1000000.0f + p.y;
    max.x = p.x - 1000000.0f;
    max.y = p.y - 1000000.0f;
    max.z = p.z - 1000000.0f;
    max2 = max;
    min2 = min;

    player = lbl_12_bss_1ACA0.players;
    b = balls;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, b++, status++, player++)
    {
        if (*status != 2 || (player->flags & 2))
            continue;
        r = 1.5f * b->currRadius;
        mtxa_tf_point_xyz(b->pos.x, b->pos.y + 1.5f * b->currRadius, b->pos.z, &p);
        mtxa_tf_vec(&b->vel, &vel);
        vel.x = 6.0 * fabs(vel.x);
        vel.y = 6.0 * fabs(vel.y);
        if (vel.z < 0.0f)
            vel.z = 0.0f;
        else
            vel.z *= 6.0f;
        if (max.z < vel.z + (p.z + r))
            max.z = vel.z + (p.z + r);
        t = r + (p.y + vel.y + tanY * p.z);
        if (max.y < t)
            max.y = t;
        t = p.y - vel.y - tanY * p.z - r;
        if (min.y > t)
            min.y = t;
        t = r + (p.x + vel.x + tanX * p.z);
        if (max.x < t)
            max.x = t;
        t = p.x - vel.x - tanX * p.z - r;
        if (min.x > t)
            min.x = t;
    }

    item = items;
    status = g_poolInfo.itemPool.statusList;
    for (i = 0; i < g_poolInfo.itemPool.count; i++, item++, status++)
    {
        if (*status == 0 || *stagedef->falloutY > item->pos.y)
            continue;
        r = 1.5f * item->unk14;
        mtxa_tf_point(&item->pos, &p);
        if (max.z < p.z + r)
            max.z = p.z + r;
        t = r + (p.y + tanY * p.z);
        if (max.y < t)
            max.y = t;
        t = p.y - tanY * p.z - r;
        if (min.y > t)
            min.y = t;
        t = r + (p.x + tanX * p.z);
        if (max.x < t)
            max.x = t;
        t = p.x - tanX * p.z - r;
        if (min.x > t)
            min.x = t;
    }

    r = 0.1f;
    mtxa_tf_point(&lbl_12_bss_1EE38.flyIn.pos, &p);
    t = r + (p.y + tanY * p.z);
    if (max.y < t)
        max.y = t;
    t = p.y - tanY * p.z - r;
    if (min.y > t)
        min.y = t;
    t = r + (p.x + tanX * p.z);
    if (max.x < t)
        max.x = t;
    t = p.x - tanX * p.z - r;
    if (min.x > t)
        min.x = t;

    if (lbl_12_bss_1EE90.setCount != 0)
    {
        group = 0;
        mtxa_push();
        set = lbl_12_bss_1EE90.sets;
        for (i = lbl_12_bss_1EE90.setCount; i > 0; i--, set++)
        {
            if (set->unk0 != group)
            {
                mtxa_pop();
                mtxa_push();
                group = set->unk0;
                mtxa_mult_right(itemgroups[group].transform);
            }
            pos = set->list;
            for (j = set->count; j > 0; j--, pos++)
            {
                r = pos->unk0;
                mtxa_tf_point(&pos->pos, &p);
                if (max2.z < p.z + r)
                    max2.z = p.z + r;
                t = r + (p.y + tanY * p.z);
                if (max2.y < t)
                    max2.y = t;
                t = p.y - tanY * p.z - r;
                if (min2.y > t)
                    min2.y = t;
                t = r + (p.x + tanX * p.z);
                if (max2.x < t)
                    max2.x = t;
                t = p.x - tanX * p.z - r;
                if (min2.x > t)
                    min2.x = t;
            }
        }
        mtxa_pop();
        if (max.x > max2.x)
            max.x = max2.x;
        if (max.y > max2.y)
            max.y = max2.y;
        if (max.z > max2.z)
            max.z = max2.z;
        if (min.x < min2.x)
            min.x = min2.x;
        if (min.y < min2.y)
            min.y = min2.y;
    }

    t = 0.5f * (max.x - min.x) / tanX;
    if (max.z < t)
        max.z = t;
    t = 0.5f * (max.y - min.y) / tanY;
    if (max.z < t)
        max.z = t;
    p.x = 0.5f * (max.x + min.x);
    p.y = 0.5f * (max.y + min.y);
    p.z = max.z;
    x = 0.1f * p.x;
    y = 0.1f * p.y;
    if (p.z < 0.0f)
        z = 0.025f * p.z;
    else
        z = 0.18f * p.z;
    smooth = *smoothPtr;
    smooth.x += 0.4f * (x - smooth.x);
    smooth.y += 0.4f * (y - smooth.y);
    smooth.z += 0.15f * (z - smooth.z);
    *smoothPtr = smooth;
    p = smooth;
    mtxa_rigid_inv_tf_point(&p, &p);
    camera->eye = p;

    p.x = 0.0f;
    p.y = 0.0f;
    p.z = -15.0f;
    mtxa_from_rotate_y(lbl_1_bss_6FAE4);
    mtxa_rotate_x(lbl_1_bss_6FAE6);
    mtxa_tf_vec(&p, &p);
    camera->lookAt.x = camera->eye.x + p.x;
    camera->lookAt.y = camera->eye.y + p.y;
    camera->lookAt.z = camera->eye.z + p.z;
    camera->eyeVel.x = camera->eye.x - prevEye.x;
    camera->eyeVel.y = camera->eye.y - prevEye.y;
    camera->eyeVel.z = camera->eye.z - prevEye.z;
    camera->lookAtVel.x = camera->lookAt.x - prevLookAt.x;
    camera->lookAtVel.y = camera->lookAt.y - prevLookAt.y;
    camera->lookAtVel.z = camera->lookAt.z - prevLookAt.z;

    p.x = camera->lookAt.x - camera->eye.x;
    p.y = camera->lookAt.y - camera->eye.y;
    p.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(p.x, p.z) - 0x8000;
    camera->rotX = math_atan2(p.y, math_sqrt(mathutil_sum_of_sq_2(p.x, p.z)));
    camera->rotZ = 0;
}

/* Mode 0: starts the fly-in from above the stage start positions. */
void fn_12_21134(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    struct Sphere sphere;
    struct StartPos *start;
    int i;
    float scale;

    camera->flags |= CAMERA_FLAG_NO_STAGE_TILT;
    get_curr_stage_fly_in_position(&sphere);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    start = stagedef->startPos;
    for (i = *(s32 *)stagedef->unk6C; i > 0; i--, start++)
    {
        v.x += start->pos.x;
        v.y += start->pos.y;
        v.z += start->pos.z;
    }
    scale = 1.0f / *(s32 *)stagedef->unk6C;
    lbl_12_bss_1EE90.startCenter.x = v.x * scale;
    lbl_12_bss_1EE90.startCenter.y = v.y * scale;
    lbl_12_bss_1EE90.startCenter.z = v.z * scale;

    camera->unk5C = camera->lookAt;
    camera->unk68 = mathutil_vec_distance(&camera->eye, &camera->lookAt);
    camera->unk74 = camera->rotY;
    camera->unk70 = camera->rotX;
    camera->unk78 = camera->rotZ;
    camera->unk7C = lbl_12_bss_1EE90.startCenter;
    mtxa_from_rotate_x(lbl_1_bss_6FAE6);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 17.5f;
    mtxa_tf_vec(&v, &v);
    mtxa_from_translate(&camera->unk7C);
    mtxa_rotate_y(lbl_1_bss_6FAE4);
    mtxa_tf_point(&v, &v);
    v.x = camera->unk7C.x - v.x;
    v.y = camera->unk7C.y - v.y;
    v.z = camera->unk7C.z - v.z;
    camera->unk94 = (s16)(math_atan2(v.x, v.z) - 0x8000) + 0x10000;
    camera->unk90 = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->unk98 = 0;
    camera->flags |= CAMERA_FLAG_UNK1;
    camera->timerMax = camera->timerCurr = 120;
    camera->unk26 = 1;
    fn_12_21420(camera, ball);
    camera->unk1F = 1;
}

/* Mode 1: eases from the fly-in start to the stage view, then switches to mode 4. */
void fn_12_21420(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    float t;

    if (g_some_other_flags & 0xA)
        return;
    camera->timerCurr--;
    if (camera->timerCurr == 0)
        camera->unk1F = 4;
    t = (float)camera->timerCurr / (float)camera->timerMax;
    t = (3.0 + -2.0 * t) * (t * t);
    camera->lookAt.x = camera->unk7C.x * (1.0 - t) + camera->unk5C.x * t;
    camera->lookAt.y = camera->unk7C.y * (1.0 - t) + camera->unk5C.y * t;
    camera->lookAt.z = camera->unk7C.z * (1.0 - t) + camera->unk5C.z * t;
    camera->rotX = camera->unk70 + (1.0 - t) * (s16)(camera->unk90 - camera->unk70);
    camera->rotY = camera->unk74 + (1.0 - t) * (camera->unk94 - camera->unk74);
    camera->rotZ = camera->unk78 + (1.0 - t) * (s16)(camera->unk98 - camera->unk78);
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    mtxa_rotate_z(camera->rotZ);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 10.0 * (1.0 - t) + camera->unk68 * t;
    mtxa_tf_point(&v, &camera->eye);
}

/* Mode 6: starts the stage orbit (or the stage gimmick's own camera). */
void fn_12_2170C(struct Camera *camera, struct Ball *ball)
{
    struct Sphere sphere;
    struct FightStageCameraFuncs *funcs;

    camera_clear(camera);
    camera->flags |= CAMERA_FLAG_NO_STAGE_TILT;
    funcs = lbl_12_bss_1EE38.cameraFuncs;
    if (funcs != NULL && funcs->init != NULL)
    {
        funcs->init(camera, ball);
    }
    else
    {
        sphere = lbl_12_bss_1EE38.flyIn;
        camera->unk5C = sphere.pos;
        camera->unk68 = 0.0125f * sphere.radius;
        camera->unk6C = 0.005f * sphere.radius;
        camera->unk7C = sphere.pos;
        camera->unk88 = 0.8f * sphere.radius;
        camera->unk8C = 0.7f * sphere.radius;
        camera->lookAt = camera->unk5C;
        camera->rotY = rand() & 0x7FFF;
        camera->flags |= CAMERA_FLAG_UNK1;
    }
    camera->unk26 = 1;
    fn_12_2188C(camera, ball);
    camera->unk1F = 7;
}

/* Mode 7: slowly orbits the stage while zooming out. */
void fn_12_2188C(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    struct FightStageCameraFuncs *funcs;

    if (g_some_other_flags & 0xA)
        return;
    funcs = lbl_12_bss_1EE38.cameraFuncs;
    if (funcs != NULL && funcs->tick != NULL)
    {
        funcs->tick(camera, ball);
        return;
    }
    camera->unk5C.x += 0.01f * (camera->unk7C.x - camera->unk5C.x);
    camera->unk5C.y += 0.01f * (camera->unk7C.y - camera->unk5C.y);
    camera->unk5C.z += 0.01f * (camera->unk7C.z - camera->unk5C.z);
    camera->unk6C += 0.005f * (camera->unk8C - camera->unk6C);
    camera->unk68 += 0.005f * (camera->unk88 - camera->unk68);
    camera->lookAt = camera->unk5C;
    camera->rotY += 9830.4f / (6.2831855f * camera->unk68);
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    v.x = 0.0f;
    v.y = camera->unk6C;
    v.z = camera->unk68;
    mtxa_tf_point(&v, &camera->eye);
    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(v.x, v.z) - 0x8000;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = 0;
}

/* Mode 2: starts the spectator camera. */
void fn_12_21AB8(struct Camera *camera, struct Ball *ball)
{
    if (g_some_other_flags & 0xA)
        return;
    camera->unk1F = 3;
    camera->unk26 = 0;
    fn_12_21B00(camera, ball);
}

/* Mode 3: follows the centre of all gloves still in the round, from behind them. */
void fn_12_21B00(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    Vec center;
    float maxDist;
    float dist;
    float minX;
    float dirX;
    float dirY;
    float dirZ;
    float minY;
    float minZ;
    float maxX;
    float maxY;
    float maxZ;
    int i;
    int allOnGround;
    struct FightPlayer *player;
    s8 *status;
    struct Ball *b;

    if (g_some_other_flags & 0xA)
        return;
    if (camera->idx != 0)
    {
        FIGHT7_COPY_MAIN_CAMERA(camera);
        return;
    }

    dirX = 0.0f;
    dirY = 0.0f;
    dirZ = 0.0f;
    minX = 10000.0f;
    minY = 10000.0f;
    minZ = 10000.0f;
    maxX = -10000.0f;
    maxY = -10000.0f;
    maxZ = -10000.0f;
    allOnGround = TRUE;
    status = g_poolInfo.playerPool.statusList;
    player = lbl_12_bss_1ACA0.players;
    b = balls;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, b++, player++, status++)
    {
        if (*status == 0 || player->rank != 0)
            continue;
        if (b->flags & 0x200)
        {
            allOnGround = FALSE;
        }
        else
        {
            mtxa_from_rotate_y(b->unk92);
            mtxa_tf_vec_xyz(0.0f, 0.0f, -5.0f, &v);
            dirX += v.x;
            dirY += v.y;
            dirZ += v.z;
        }
        v = b->pos;
        if (maxX < v.x)
            maxX = v.x;
        if (maxY < v.y)
            maxY = v.y;
        if (maxZ < v.z)
            maxZ = v.z;
        if (minX > v.x)
            minX = v.x;
        if (minY > v.y)
            minY = v.y;
        if (minZ > v.z)
            minZ = v.z;
    }
    center.x = 0.5f * (maxX + minX);
    center.y = 0.5f * (maxY + minY);
    center.z = 0.5f * (maxZ + minZ);

    maxDist = 0.0f;
    status = g_poolInfo.playerPool.statusList;
    player = lbl_12_bss_1ACA0.players;
    b = balls;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, b++, player++, status++)
    {
        if (*status == 0 || player->rank != 0)
            continue;
        dist = 3.0f * b->currRadius + mathutil_vec_distance(&b->pos, &center);
        if (maxDist < dist)
            maxDist = dist;
    }

    v.x = dirX + (camera->eye.x - center.x);
    v.y = dirY + (camera->eye.y - center.y);
    v.z = dirZ + (camera->eye.z - center.z);
    dist = mathutil_vec_distance(&camera->eye, &center);
    dist += 0.75f * (maxDist / math_sin(camera->sub28.view.fov >> 2) - dist);
    vec_set_len(dist, &v, &v);
    v.x += center.x;
    v.y += center.y;
    v.z += center.z;
    if (!allOnGround)
        v.y = camera->eye.y;

    camera->eyeVel.x *= 0.92f;
    camera->eyeVel.y *= 0.92f;
    camera->eyeVel.z *= 0.92f;
    camera->eyeVel.x += 0.02f * (0.1f * (v.x - camera->eye.x) - camera->eyeVel.x);
    camera->eyeVel.y += 0.02f * (0.1f * (v.y - camera->eye.y) - camera->eyeVel.y);
    camera->eyeVel.z += 0.02f * (0.1f * (v.z - camera->eye.z) - camera->eyeVel.z);
    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;
    camera->lookAtVel.x *= 0.95f;
    camera->lookAtVel.y *= 0.95f;
    camera->lookAtVel.z *= 0.95f;
    camera->lookAtVel.x += 0.1f * (0.1f * (center.x - camera->lookAt.x) - camera->lookAtVel.x);
    camera->lookAtVel.y += 0.2f * (0.1f * (center.y - camera->lookAt.y) - camera->lookAtVel.y);
    camera->lookAtVel.z += 0.1f * (0.1f * (center.z - camera->lookAt.z) - camera->lookAtVel.z);
    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(v.x, v.z) - 0x8000;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = 0;
}

/* Mode 8: starts the winner camera: placed near the winner, then thrown towards them. */
void fn_12_22250(struct Camera *camera, struct Ball *ball)
{
    Vec target;
    Vec eye;
    struct PhysicsBall physBall;
    struct Ape *ape;
    struct Ball *winner;
    float dist;
    float minDist;

    if (g_some_other_flags & 0xA)
        return;
    winner = &balls[lbl_12_bss_1ACA0.unk1750];
    ape = winner->ape;
    dist = mathutil_vec_distance(&winner->pos, &camera->eye);
    dist = 1.5f * winner->currRadius + 0.25f * (dist - 1.5f * winner->currRadius);
    minDist = 0.25f + 1.5f * ((float)modeCtrl.submodeTimer / (float)modeCtrl.unk3C);
    if (dist < minDist)
        dist = minDist;
    mtxa_from_rotate_y(winner->unk92);
    mtxa_tf_vec_xyz(dist * (-1.5f * (RAND_FLOAT() - 0.5f)), dist * (-1.0f * (RAND_FLOAT() - 0.5f)), -dist, &eye);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -dist, &target);
    eye.x += winner->pos.x;
    eye.y += winner->pos.y;
    eye.z += winner->pos.z;
    target.x += winner->pos.x;
    target.y += winner->pos.y;
    target.z += winner->pos.z;

    physBall.flags = 0;
    physBall.pos = eye;
    physBall.prevPos = winner->pos;
    physBall.vel.x = physBall.pos.x - physBall.prevPos.x;
    physBall.vel.y = physBall.pos.y - physBall.prevPos.y;
    physBall.vel.z = physBall.pos.z - physBall.prevPos.z;
    physBall.radius = 0.15f;
    physBall.gravityAccel = winner->accel;
    physBall.restitution = winner->restitution;
    physBall.hardestColiSpeed = 0.0f;
    physBall.itemGroupId = 0;
    physBall.hardestColiItemGroupId = 0;
    physBall.friction = 0.01f;
    collide_ball_with_stage(&physBall, stagedef);
    eye = physBall.pos;
    camera->eye = eye;

    camera->eyeVel.x = (target.x - eye.x) * ((2.0f * RAND_FLOAT()) / 60.0f);
    camera->eyeVel.y = (target.y - eye.y) * (RAND_FLOAT() / 60.0f);
    camera->eyeVel.z = (target.z - eye.z) * ((2.0f * RAND_FLOAT()) / 60.0f);
    camera->lookAt.x = winner->pos.x;
    camera->lookAt.y = 0.25f + winner->pos.y;
    camera->lookAt.z = winner->pos.z;
    camera->lookAtVel.x = 0.0f;
    camera->lookAtVel.y = 0.0f;
    camera->lookAtVel.z = 0.0f;
    camera->timerCurr = fn_1_1136E0(ape);
    camera->unk1F = 9;
    camera->unk26 = 0;
    camera->flags |= CAMERA_FLAG_UNK1;
    fn_12_22708(camera, winner);
}

/* Mode 9: the camera bounces around the winner (off the stage and the gloves),
 * looking between two of the winner's bones; back to mode 8 when the pose ends. */
void fn_12_22708(struct Camera *camera, struct Ball *ball)
{
    struct PhysicsBall physBall;
    Mtx mtx;
    Vec target;
    Vec dir;
    Vec prevEye;
    Vec rel;
    struct Ape *ape;
    int frozen;
    int holding;
    struct Ball *winner;
    int i;
    struct Ball *b;
    int j;
    struct FightCameraBone *bone;
    float f;  /* scratch: factor, push-out distance, dot product */
    float len;

    if (g_some_other_flags & 0xA)
        return;
    if (camera->idx != 0)
    {
        FIGHT7_COPY_MAIN_CAMERA(camera);
        return;
    }

    winner = &balls[lbl_12_bss_1ACA0.unk1750];
    ape = winner->ape;
    frozen = ape->flags & 0x4000;
    holding = FALSE;
    if ((ape->unk86 == 3 && 0.5f * fn_1_1136E0(ape) < fn_1_1135E8(ape)) || modeCtrl.submodeTimer < 120.0f)
        holding = TRUE;
    prevEye = camera->eye;
    if (frozen)
    {
        if (winner->pos.y - camera->eye.y < 0.0f)
            camera->eyeVel.y += 0.006f;
        else
            camera->eyeVel.y += -0.003f;
        camera->eyeVel.x *= 0.98f;
        camera->eyeVel.y *= 0.98f;
        camera->eyeVel.z *= 0.98f;
    }
    else
    {
        if (holding)
        {
            f = 1.0f;
            camera->eyeVel.y += 0.0035f * f;
        }
        dir.x = winner->pos.x - camera->eye.x;
        dir.y = 0.0f;
        dir.z = winner->pos.z - camera->eye.z;
        if (0.0f != dir.x || 0.0f != dir.z)
            vec_normalize_len(&dir);
        else
            dir.z = -1.0f;
        f = mathutil_vec_dot_prod(&dir, &camera->eyeVel);
        camera->eyeVel.x -= f * dir.x;
        camera->eyeVel.y -= f * dir.y;
        camera->eyeVel.z -= f * dir.z;
        f *= 0.92f;
        camera->eyeVel.x += f * dir.x;
        camera->eyeVel.y += f * dir.y;
        camera->eyeVel.z += f * dir.z;
    }
    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    b = balls;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, b++)
    {
        dir.x = b->pos.x - camera->eye.x;
        dir.y = b->pos.y - camera->eye.y;
        dir.z = b->pos.z - camera->eye.z;
        len = vec_normalize_len(&dir);
        if (len < (f = 0.15f + b->currRadius))
        {
            camera->eye.x -= f * dir.x;
            camera->eye.y -= f * dir.y;
            camera->eye.z -= f * dir.z;
            rel.x = camera->eyeVel.x - b->vel.x;
            rel.y = camera->eyeVel.y - b->vel.y;
            rel.z = camera->eyeVel.z - b->vel.z;
            f = mathutil_vec_dot_prod(&dir, &rel);
            if (f > 0.0f)
            {
                f *= -1.9f;
                rel.x += f * dir.x;
                rel.y += f * dir.y;
                rel.z += f * dir.z;
                camera->eyeVel.x = rel.x + b->vel.x;
                camera->eyeVel.y = rel.y + b->vel.y;
                camera->eyeVel.z = rel.z + b->vel.z;
            }
        }
    }

    physBall.flags = 0;
    physBall.pos = camera->eye;
    physBall.prevPos = prevEye;
    physBall.vel = camera->eyeVel;
    physBall.radius = 0.15f;
    physBall.gravityAccel = winner->accel;
    physBall.restitution = winner->restitution;
    physBall.hardestColiSpeed = 0.0f;
    physBall.itemGroupId = 0;
    physBall.hardestColiItemGroupId = 0;
    physBall.friction = 0.01f;
    collide_ball_with_stage(&physBall, stagedef);
    if (0.0f != physBall.hardestColiSpeed)
    {
        camera_start_shake(camera->idx, 15.0f * (1.0f + RAND_FLOAT()), &physBall.vel);
        camera->eye = physBall.pos;
        camera->eyeVel = physBall.vel;
    }

    if (frozen)
    {
        target = winner->pos;
    }
    else
    {
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(mtx);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(mtx);
        target.x = 0.0f;
        target.y = 0.0f;
        target.z = 0.0f;
        bone = lbl_12_data_9DFC;
        for (j = 2; j > 0; j--, bone++)
        {
            mtxa_push();
            mtxa_mult_right(g_get_ape_anim_smth(ape, bone->bone));
            mtxa_tf_point(&bone->offset, &dir);
            mtxa_pop();
            target.x += dir.x;
            target.y += dir.y;
            target.z += dir.z;
        }
        target.x *= 0.5f;
        target.y *= 0.5f;
        target.z *= 0.5f;
        target.y += 0.25f;
    }
    camera->lookAtVel.x += 0.06f * (0.25f * (target.x - camera->lookAt.x) - camera->lookAtVel.x);
    camera->lookAtVel.y += 0.06f * (0.25f * (target.y - camera->lookAt.y) - camera->lookAtVel.y);
    camera->lookAtVel.z += 0.06f * (0.25f * (target.z - camera->lookAt.z) - camera->lookAtVel.z);
    camera->lookAtVel.x *= 0.99f;
    camera->lookAtVel.y *= 0.99f;
    camera->lookAtVel.z *= 0.99f;
    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;
    if (!holding && !frozen)
    {
        camera->timerCurr--;
        if (camera->timerCurr <= 0)
            camera->unk1F = 8;
    }

    dir.x = camera->lookAt.x - camera->eye.x;
    dir.y = camera->lookAt.y - camera->eye.y;
    dir.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(dir.x, dir.z) - 0x8000;
    camera->rotX = math_atan2(dir.y, math_sqrt(mathutil_sum_of_sq_2(dir.x, dir.z)));
    camera->rotZ = 0;
}
