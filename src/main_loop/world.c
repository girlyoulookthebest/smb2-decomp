/* Stage tilting (SMB1 world.c): turns stick input into per-player stage rotation and gravity. */
#include "libc.h"
#include "types.h"
#include "mathutil.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "input.h"
#include "mode.h"
#include "pool.h"
#include "recplay.h"
#include "world.h"

/* A keyframe of a Hermite curve, as evaluated by fn_1_D2550. */
struct Keyframe
{
    s32 frame;
    float value;
    float tangentIn;
    float tangentOut;
};

/* SMB1's ReplayWorldFrame */
struct ReplayWorldFrame
{
    s16 rotX;
    s16 rotZ;
};

/* SMB1's advTutorialInfo: the scripted stick input of the tutorial demo. */
extern struct
{
    s32 stickXRot;
    s32 stickZRot;
} lbl_1_bss_1568;

extern float lbl_1_bss_6FA90;  /* "WLD SLANT COEF" */

float fn_1_D2550(float frame, struct Keyframe *curve);
void fn_1_10FA8(struct World *world);
void fn_1_621B4(int replay, float time, struct ReplayWorldFrame *frame);

struct World world_infos[4];
Vec g_gravity_dir;
struct World *current_world_info;

struct Keyframe lbl_1_data_2EA10[] =
{
    {     0,     0.0f,     0.0f,     0.0f },
    {   240,     0.0f,     0.0f,     0.0f },
    {   300,  -478.0f,     0.0f,     0.0f },
    {   480,  -382.0f,     0.0f,     0.0f },
    {   540,  -382.0f,     0.0f,     0.0f },
    {   720,  -510.0f,     0.0f,     0.0f },
    {   780,   -30.0f,     0.0f,     0.0f },
    {   960,   -30.0f,     0.0f,     0.0f },
    {  1020,    24.0f,     0.0f,     0.0f },
    {  1170,    24.0f,     0.0f,     0.0f },
    {  1200,     0.0f,     0.0f,     0.0f },
    {  2160,     0.0f,     0.0f,     0.0f },
    {  2220,  -416.0f,     0.0f,     0.0f },
    {  2700,     0.0f,     0.0f,     0.0f },
    {  2940,     0.0f,     0.0f,     0.0f },
    {  3030, -2560.0f,     0.0f,     0.0f },
    {  3060, -2816.0f,     0.0f,     0.0f },
    {  3090, -2560.0f,     0.0f,     0.0f },
    {  3360, -2304.0f,     0.0f,     0.0f },
    {  3390,     0.0f,     0.0f,     0.0f },
    {  3810,     0.0f,     0.0f,     0.0f },
    {  3840, -3695.0f,     0.0f,     0.0f },
    {  3870,     0.0f,     0.0f,     0.0f },
    {  3990,     0.0f,     0.0f,     0.0f },
    {  4050,  -938.0f,     0.0f,     0.0f },
    {  4420, -1024.0f,     0.0f,     0.0f },
    {  4450,  3072.0f,     0.0f,     0.0f },
    {  4510, -6144.0f,     0.0f,     0.0f },
    {  4520, -6144.0f,     0.0f,     0.0f },
    {  4550, -2048.0f,     0.0f,     0.0f },
    {  4580,     0.0f,     0.0f,     0.0f },
    {  4600,   512.0f,     0.0f,     0.0f },
    {  4630,     0.0f,     0.0f,     0.0f },
    {  4660,     0.0f,     0.0f,     0.0f },
    {  4690, -1024.0f,     0.0f,     0.0f },
    {  4870, -1024.0f,     0.0f,     0.0f },
    {  4900,     0.0f,     0.0f,     0.0f },
    {  5160,     0.0f,     0.0f,     0.0f },
};

struct Keyframe lbl_1_data_2EC70[] =
{
    {     0,     0.0f,     0.0f,     0.0f },
    {   540,     0.0f,     0.0f,     0.0f },
    {   660,  -768.0f,     0.0f,     0.0f },
    {   780,     0.0f,     0.0f,     0.0f },
    {  2850,     0.0f,     0.0f,     0.0f },
    {  2910,   574.0f,     0.0f,     0.0f },
    {  2970,     0.0f,     0.0f,     0.0f },
    {  3240,     0.0f,     0.0f,     0.0f },
    {  3360,   -96.0f,     0.0f,     0.0f },
    {  3480,     0.0f,     0.0f,     0.0f },
    {  4200,     0.0f,     0.0f,     0.0f },
    {  4260,  1408.0f,     0.0f,     0.0f },
    {  4420,  1408.0f,     0.0f,     0.0f },
    {  4450, -3584.0f,     0.0f,     0.0f },
    {  4510, -3584.0f,     0.0f,     0.0f },
    {  4540,  2048.0f,     0.0f,     0.0f },
    {  4570, -1024.0f,     0.0f,     0.0f },
    {  4580, -1024.0f,     0.0f,     0.0f },
    {  4610,     0.0f,     0.0f,     0.0f },
    {  4640,  -640.0f,     0.0f,     0.0f },
    {  4700,  -640.0f,     0.0f,     0.0f },
    {  4730,     0.0f,     0.0f,     0.0f },
    {  5160,     0.0f,     0.0f,     0.0f },
};

void world_clear(struct World *world);
void world_sub_input_init(struct World *world);
void world_sub_input_main(struct World *world);
void world_sub_6(struct World *world);
void world_sub_7(struct World *world);
void world_sub_8(struct World *world);
void world_sub_9(struct World *world);
void world_sub_10(struct World *world);
void world_sub_11(struct World *world);

void event_world_init(void)
{
    struct World *world;
    int i;

    world = world_infos;
    for (i = 0; i < 4; i++, world++)
    {
        world_clear(world);
        world->playerId = i;
    }
    g_gravity_dir.x = 0.0f;
    g_gravity_dir.y = -1.0f;
    g_gravity_dir.z = 0.0f;
}

void event_world_tick(void)
{
    struct World *world = world_infos;
    s8 *status = g_poolInfo.playerPool.statusList;
    int i;
    Vec sum;

    for (i = 0; i < g_poolInfo.playerPool.count; i++, world++, status++)
    {
        if (*status == 0 || *status == 4)
            continue;
        world->playerId = i;
        switch (world->state)
        {
        case 1:
            world_sub_input_init(world);
            break;
        case 2:
            world_sub_input_main(world);
            break;
        case 6:
            world_sub_6(world);
            // fall through
        case 7:
            world_sub_7(world);
            break;
        case 8:
            world_sub_8(world);
            // fall through
        case 9:
            world_sub_9(world);
            break;
        case 10:
            world_sub_10(world);
            // fall through
        case 11:
            world_sub_11(world);
            break;
        }
        if (world->unk1C > 0)
            world->unk1C--;
    }

    switch (modeCtrl.gameType)
    {
    case 1:
    case 3:
    case 4:
        sum.x = 0.0f;
        sum.y = 0.0f;
        sum.z = 0.0f;
        world = world_infos;
        status = g_poolInfo.playerPool.statusList;
        for (i = 0; i < g_poolInfo.playerPool.count; i++, world++, status++)
        {
            if (*status == 0 || *status == 4)
                continue;
            sum.x += world->unk10.x;
            sum.y += world->unk10.y;
            sum.z += world->unk10.z;
        }
        if (sum.x != 0.0f || sum.y != 0.0f || sum.z != 0.0f)
        {
            g_gravity_dir = sum;
            vec_normalize_len(&g_gravity_dir);
        }
        else
        {
            g_gravity_dir.x = 0.0f;
            g_gravity_dir.y = 0.0f;
            g_gravity_dir.z = 0.0f;
        }
        break;
    default:
        g_gravity_dir = world_infos[modeCtrl.currPlayer].unk10;
        break;
    }
}

void event_world_dest(void)
{
    struct World *world = world_infos;
    s8 *status = g_poolInfo.playerPool.statusList;
    int i;

    for (i = 0; i < g_poolInfo.playerPool.count; i++, world++, status++)
    {
        if (*status == 2)
        {
            memset(world, 0, sizeof(*world));
            world->unk10.y = -1.0f;
        }
    }
    g_gravity_dir.x = 0.0f;
    g_gravity_dir.y = -1.0f;
    g_gravity_dir.z = 0.0f;
}

void world_clear(struct World *world)
{
    memset(world, 0, sizeof(*world));
    world->unk10.y = -1.0f;
    world->unk20.y = 1.0f;
}

void world_sub_input_init(struct World *world)
{
    world_clear(world);
    switch (modeCtrl.gameType)
    {
    case 4:
        world->maxTilt = 23.0 * lbl_1_bss_6FA90;
        break;
    default:
        world->maxTilt = 23.0f;
        break;
    }
    world->state = 2;
}

void world_sub_input_main(struct World *world)
{
    int inpXRot;
    int inpYRot;
    Vec spC;
    Quat quat;

    if (g_some_other_flags & 0xA)
        return;

    if (main_mode == 0 && sub_mode != 8 && sub_mode != 14)
        return;

    if (sub_mode == 8)
    {
        fn_1_10FA8(world);
    }
    else if (sub_mode == 14)
    {
        float t = 5160 - modeCtrl.submodeTimer;

        world->xrotPrev = world->xrot;
        world->zrotPrev = world->zrot;
        inpXRot = lbl_1_bss_1568.stickXRot = fn_1_D2550(t, lbl_1_data_2EA10);
        inpYRot = lbl_1_bss_1568.stickZRot = fn_1_D2550(t, lbl_1_data_2EC70);

        mtxa_from_identity();
        mtxa_rotate_y(cameras[world->playerId].rotY);
        mtxa_rotate_x(inpXRot);
        mtxa_rotate_z(inpYRot);
        spC.x = 0.0f;
        spC.y = 1.0f;
        spC.z = 0.0f;
        mtxa_tf_vec(&spC, &spC);
        inpXRot = math_atan2(spC.z, spC.y);
        inpYRot = -(s16)math_atan2(spC.x, math_sqrt(mathutil_sum_of_sq_2(spC.z, spC.y)));

        inpXRot = (s16)(inpXRot - world->xrot);
        inpYRot = (s16)(inpYRot - world->zrot);
        world->xrot += inpXRot * 0.2;
        world->zrot += inpYRot * 0.2;
        spC.x = 0.0f;
        spC.y = -1.0f;
        spC.z = 0.0f;
        mtxa_from_identity();
        mtxa_rotate_x(world->xrot);
        mtxa_rotate_z(world->zrot);
        mtxa_rigid_inv_tf_vec(&spC, &world->unk10);
    }
    else if ((balls[world->playerId].flags & 0x1000)
     || world->unk1C > 0
     || (balls[world->playerId].state == 23 && balls[world->playerId].unk154 >= 4)
     || balls[world->playerId].unk154 == 1)
    {
        inpXRot = 0;
        inpYRot = 0;

        inpXRot = (s16)(inpXRot - world->xrot);
        inpYRot = (s16)(inpYRot - world->zrot);
        world->xrot += inpXRot * 0.2;
        world->zrot += inpYRot * 0.2;
        spC.x = 0.0f;
        spC.y = -1.0f;
        spC.z = 0.0f;
        mtxa_from_identity();
        mtxa_rotate_x(world->xrot);
        mtxa_rotate_z(world->zrot);
        mtxa_rigid_inv_tf_vec(&spC, &world->unk10);
    }
    else  // normal controller input
    {
        float stickX;
        float stickY;

        world->xrotPrev = world->xrot;
        world->zrotPrev = world->zrot;
        stickX = pad_status_groups[player_pad_map[world->playerId]].unk0[0].stickX / 60.0;
        stickY = -(float)pad_status_groups[player_pad_map[world->playerId]].unk0[0].stickY / 60.0;

        if (stickX < -1.0)
            stickX = -1.0f;
        else if (stickX > 1.0)
            stickX = 1.0f;

        if (stickY < -1.0)
            stickY = -1.0f;
        else if (stickY > 1.0)
            stickY = 1.0f;

        inpXRot = stickY * DEGREES_TO_S16(world->maxTilt);
        inpYRot = -stickX * DEGREES_TO_S16(world->maxTilt);

        mtxa_from_identity();
        mtxa_rotate_y(cameras[world->playerId].rotY);
        mtxa_rotate_x(inpXRot);
        mtxa_rotate_z(inpYRot);
        spC.x = 0.0f;
        spC.y = 1.0f;
        spC.z = 0.0f;
        mtxa_tf_vec(&spC, &spC);

        // smooth the "up" vector
        spC.x = world->unk20.x + 0.2 * (spC.x - world->unk20.x);
        spC.y = world->unk20.y + 0.2 * (spC.y - world->unk20.y);
        spC.z = world->unk20.z + 0.2 * (spC.z - world->unk20.z);
        world->xrot = math_atan2(spC.z, spC.y);
        world->zrot = -math_atan2(spC.x, math_sqrt(mathutil_sum_of_sq_2(spC.z, spC.y)));
        vec_normalize_len(&spC);
        world->unk20 = spC;
        {
            Vec up = {0.0f, 1.0f, 0.0f};

            quat_from_dirs(&quat, &up, &spC);
        }
        mtxa_from_quat(&quat);
        mtxa_rigid_inv_tf_point_xyz(0.0f, -1.0f, 0.0f, &world->unk10);
    }
}

void world_sub_6(struct World *world)
{
    world->state = 7;
}

void world_sub_7(struct World *world)
{
    s16 var1;
    s16 var2;
    Vec sp10;
    struct ReplayWorldFrame spC;

    if (g_some_other_flags & 0xA)
        return;

    fn_1_621B4(lbl_1_bss_71EC8.unk0[world->playerId], lbl_1_bss_71EC8.unk10, &spC);
    var1 = -world->xrot;
    var2 = -world->zrot;
    world->xrot += var1 >> 2;
    world->zrot += var2 >> 2;
    sp10.x = 0.0f;
    sp10.y = -1.0f;
    sp10.z = 0.0f;
    if (mode_info.flags & INFO_FLAG_REPLAY)
    {
        mtxa_from_identity();
        mtxa_rotate_x(spC.rotX);
        mtxa_rotate_z(spC.rotZ);
        mtxa_rigid_inv_tf_vec(&sp10, &world->unk10);
    }
    else
        world->unk10 = sp10;
}

void world_sub_8(struct World *world)
{
    world->state = 9;
}

void world_sub_9(struct World *world)
{
    s16 var1;
    s16 var2;
    Vec sp10;
    struct ReplayWorldFrame spC;

    if (g_some_other_flags & 0xA)
        return;

    fn_1_621B4(lbl_1_bss_71EC8.unk0[world->playerId], lbl_1_bss_71EC8.unk10, &spC);
    var1 = spC.rotX - world->xrot;
    var2 = spC.rotZ - world->zrot;
    world->xrot += var1 >> 2;
    world->zrot += var2 >> 2;
    sp10.x = 0.0f;
    sp10.y = -1.0f;
    sp10.z = 0.0f;
    mtxa_from_identity();
    mtxa_rotate_x(world->xrot);
    mtxa_rotate_z(world->zrot);
    mtxa_rigid_inv_tf_vec(&sp10, &world->unk10);
}

void world_sub_10(struct World *world)
{
    world->state = 11;
}

void world_sub_11(struct World *world)
{
    float f1;
    Vec spC;
    s16 var;

    if (g_some_other_flags & 0xA)
        return;

    f1 = pad_status_groups[player_pad_map[world->playerId]].unk0[0].stickX / 60.0;
    if (f1 < -1.0)
        f1 = -1.0f;
    else if (f1 > 1.0)
        f1 = 1.0f;
    var = -f1 * 4096;
    var = var - world->zrot;
    world->zrot += var * 0.2;
    world->zrot *= 0.8;
    spC.x = 0.0f;
    spC.y = -1.0f;
    spC.z = 0.0f;
    mtxa_from_identity();
    mtxa_rotate_z(world->zrot);
    mtxa_rotate_x(world->xrot);
    mtxa_rigid_inv_tf_vec(&spC, &world->unk10);
}
