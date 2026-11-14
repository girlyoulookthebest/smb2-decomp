#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "mathutil.h"
#include "mode.h"
#include "stage.h"
#include "stcoli.h"
#include "ape_internal.h"

void fn_2_7C30(struct Camera *camera, struct Ball *ball);
void g_camera_func90(struct Camera *camera, struct Ball *ball);

void fn_2_7BD0(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    fn_2_7C30(camera, ball);
    camera->timerCurr = 0;
    camera->unk26 = 7;
    camera->mode = 0x1A;
}

void fn_2_7C30(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec lookAt;
    float dx;
    float dy;
    float dz;

    if ((g_some_other_flags & 0xA) != 0 && camera->mode == 0x1A)
        return;
    mtxa_from_identity();
    if (stagedef->unkD4 != NULL)
    {
        mtxa_translate(stagedef->unkD4);
        mtxa_rotate_z(((struct StartPos *)stagedef->unkD4)->zrot);
        mtxa_rotate_y(((struct StartPos *)stagedef->unkD4)->yrot);
        mtxa_rotate_x(((struct StartPos *)stagedef->unkD4)->xrot);
    }
    eye.x = 0.0f;
    eye.y = 0.2f;
    eye.z = -1.4 - 0.005 * camera->timerCurr;
    lookAt.x = 0.0f;
    lookAt.y = 0.1f;
    lookAt.z = -(0.005 * camera->timerCurr);
    if (camera->mode == 0x1A)
    {
        if ((modeCtrl.courseFlags & 4) != 0 && modeCtrl.unk10 == 1)
        {
            eye.z = -2.8f;
            lookAt.z = -1.4f;
        }
        mtxa_tf_point(&eye, &eye);
        mtxa_tf_point(&lookAt, &lookAt);
        camera->eye.x += 0.1 * (eye.x - camera->eye.x);
        camera->eye.y += 0.1 * (eye.y - camera->eye.y);
        camera->eye.z += 0.1 * (eye.z - camera->eye.z);
        camera->lookAt.x += 0.1 * (lookAt.x - camera->lookAt.x);
        camera->lookAt.y += 0.1 * (lookAt.y - camera->lookAt.y);
        camera->lookAt.z += 0.1 * (lookAt.z - camera->lookAt.z);
    }
    else
    {
        mtxa_tf_point(&eye, &camera->eye);
        mtxa_tf_point(&lookAt, &camera->lookAt);
    }
    dx = camera->lookAt.x - camera->eye.x;
    dy = camera->lookAt.y - camera->eye.y;
    dz = camera->lookAt.z - camera->eye.z;
    camera->rotY = (s16)(math_atan2(dx, dz) - 0x8000);
    dx = dx * dx;
    dx = __fmadds(dz, dz, dx);
    camera->rotX = math_atan2(dy, math_sqrt(dx));
    camera->rotZ = 0;
    if (++camera->timerCurr > 0x293)
        camera->timerCurr = 0x293;
}

void g_camera_func89(struct Camera *camera, struct Ball *ball)
{
    camera->timerCurr = 0;
    camera->unk26 = 0;
    g_camera_func90(camera, ball);
    camera->eyeVel.x *= 0.25f;
    camera->eyeVel.y *= 0.25f;
    camera->eyeVel.z *= 0.25f;
    camera->mode = 0x5A;
}

void g_camera_func90(struct Camera *camera, struct Ball *ball)
{
    Mtx mtx;
    struct RaycastHit hit;
    Vec prevEye;
    Vec prevLookAt;
    Vec vel;
    Vec correction;
    Vec dir;
    Vec pos;
    Vec offs;
    struct Sphere sphere;
    float x;
    float y;
    float dist;
    float dot;
    struct Ape *ape;
    int flag;
    int timer;

    if ((g_some_other_flags & 2) != 0)
        return;
    timer = modeCtrl.submodeTimer > 0;
    ape = ball->ape;
    flag = (ape != NULL) ? (ape->flags & 2) : 0;
    if (camera->timerCurr < 0x7FFF)
        camera->timerCurr++;
    prevEye = camera->eye;
    prevLookAt = camera->lookAt;
    if (timer != 0)
    {
        camera->eyeVel.x *= 0.98f;
        camera->eyeVel.y *= 0.98f;
        camera->eyeVel.z *= 0.98f;
    }
    else if (ape == NULL)
    {
        camera->eyeVel.x *= 0.98f;
        camera->eyeVel.y *= 0.98f;
        camera->eyeVel.z *= 0.98f;
    }
    else
    {
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(mtx);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(mtx);
        mtxa_mult_right(g_get_ape_anim_smth(ape, 3));
        mathutil_mtxA_get_translate(&pos);
        if (flag == 0)
        {
            mtxa_tf_vec_xyz(1.0f, 0.0f, 0.0f, &dir);
        }
        else
        {
            get_curr_stage_fly_in_position(&sphere);
            dir.x = pos.x - sphere.pos.x;
            dir.y = pos.y - (sphere.pos.y + 0.1f * sphere.radius);
            dir.z = pos.z - sphere.pos.z;
        }
        mtxa_from_rotate_y(g_font_char_var_3 << 5);
        mtxa_tf_vec(&dir, &dir);
        offs.x = camera->eye.x - pos.x;
        offs.y = camera->eye.y - pos.y;
        offs.z = camera->eye.z - pos.z;
        dist = vec_normalize_len(&offs);
        if ((int)raycast_stage_down(&pos, &hit, NULL) != 0 && pos.y - hit.pos.y < 1.5f * dist)
        {
            dot = -mathutil_vec_dot_prod(&dir, &hit.normal);
            if (dot > 0.0f)
            {
                dir.x += dot * hit.normal.x;
                dir.y += dot * hit.normal.y;
                dir.z += dot * hit.normal.z;
            }
        }
        vec_normalize_len(&dir);
        dot = 0.025f * (5.0f * ball->currRadius - dist);
        correction.x = dot * offs.x;
        correction.y = dot * offs.y;
        correction.z = dot * offs.z;
        if (__fabs(dot) > 0.2f)
            vec_set_len(0.2f, &correction, &correction);
        x = pos.x + dir.x * dist;
        y = pos.y + dir.y * dist;
        dist = pos.z + dir.z * dist;
        switch (world_theme)
        {
        case 0x15:
        case 0x19:
            if (y < *stagedef->falloutY)
            {
                y = *stagedef->falloutY;
                dir.x = x - pos.x;
                dir.y = y - pos.y;
                dir.z = dist - pos.z;
                vec_normalize_len(&dir);
                dot = -mathutil_vec_dot_prod(&correction, &dir);
                correction.x += dot * dir.x;
                correction.y += dot * dir.y;
                correction.z += dot * dir.z;
            }
            break;
        }
        vel.x = x - camera->eye.x;
        vel.y = y - camera->eye.y;
        vel.z = dist - camera->eye.z;
        dot = vec_dot_normalized_safe(&dir, &offs);
        if (dot < 0.99f)
        {
            dot = mathutil_vec_dot_prod(&vel, &offs);
            vel.x -= offs.x * dot;
            vel.y -= offs.y * dot;
            vel.z -= offs.z * dot;
        }
        else
        {
            dot = mathutil_vec_dot_prod(&vel, &offs);
            vel.x = dot * offs.x;
            vel.y = dot * offs.y;
            vel.z = dot * offs.z;
        }
        if (flag != 0)
            dot = 0.02f;
        else
            dot = 0.01f;
        vec_set_len(dot, &vel, &vel);
        vel.x += correction.x;
        vel.y += correction.y;
        vel.z += correction.z;
        camera->eyeVel.x += 0.03f * (vel.x - camera->eyeVel.x);
        camera->eyeVel.y += 0.03f * (vel.y - camera->eyeVel.y);
        camera->eyeVel.z += 0.03f * (vel.z - camera->eyeVel.z);
    }
    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;
    camera->lookAt.x += 0.4f * (ball->pos.x - camera->lookAt.x);
    camera->lookAt.y += 0.4f * (ball->pos.y - camera->lookAt.y);
    camera->lookAt.z += 0.4f * (ball->pos.z - camera->lookAt.z);
    camera->lookAtVel.x = camera->lookAt.x - prevLookAt.x;
    camera->lookAtVel.y = camera->lookAt.y - prevLookAt.y;
    camera->lookAtVel.z = camera->lookAt.z - prevLookAt.z;
    vel.x = camera->lookAt.x - camera->eye.x;
    vel.y = camera->lookAt.y - camera->eye.y;
    vel.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = (s16)(math_atan2(vel.x, vel.z) - 0x8000);
    dot = mathutil_sum_of_sq_2(vel.x, vel.z);
    camera->rotX = math_atan2(vel.y, math_sqrt(dot));
    camera->rotZ = 0;
}
