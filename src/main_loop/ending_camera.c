/* Ending scene camera: eases towards fixed points or points attached to the
 * characters. */
#include "types.h"
#include "camera.h"
#include "dip.h"
#include "ending.h"
#include "mathutil.h"

void fn_1_108398(struct Camera *camera, struct Ball *ball);
void fn_1_10860C(struct Camera *camera, struct Ball *ball);
void fn_1_108348(struct Camera *camera, struct Ball *ball);

void *lbl_1_data_63D40[4] = { fn_1_108398, fn_1_10860C };

void fn_1_1082D4(void)
{
    struct Camera *camera;
    struct Camera *curr;
    int i;

    g_minigame_camera_func = fn_1_108348;
    curr = g_current_camera;
    camera = cameras;
    for (i = 0; i < 5; i++, camera++)
    {
        g_current_camera = camera;
        camera->mode = 0x4A;
        camera->unk1F = -1;
    }
    g_current_camera = curr;
}

void fn_1_108344(void) {}

void fn_1_108348(struct Camera *camera, struct Ball *ball)
{
    if (camera->unk1F >= 0)
        ((void (*)(struct Camera *, struct Ball *))lbl_1_data_63D40[camera->unk1F])(camera, ball);
}

void fn_1_108398(struct Camera *camera, struct Ball *ball)
{
    Mtx mtx;
    struct EndingCamera *endCam;
    struct EndingChara *chara;
    struct Ape *ape;

    if (!(g_some_other_flags & 0xA) && lbl_1_bss_D3354.work != NULL)
    {
        endCam = &lbl_1_bss_D3354.work->camera;
        camera_clear(camera);
        camera->flags |= 8;
        if (lbl_1_bss_D3354.work == NULL)
            return;
        switch (endCam->eyeMode)
        {
        case 2:
            chara = &lbl_1_bss_D3354.work->charas[endCam->eyeChara];
            ape = chara->ape;
            mtxa_from_quat(&ape->rot);
            mtxa_to_mtx(mtx);
            mtxa_from_translate(&ape->pos);
            mtxa_scale_s(ape->scale);
            mtxa_translate(&ape->unk264);
            mtxa_mult_right(mtx);
            mtxa_tf_point(&endCam->eyeOffset, &camera->eye);
            camera->eyeVel.x = 0.0f;
            camera->eyeVel.y = 0.0f;
            camera->eyeVel.z = 0.0f;
            break;
        default:
            camera->eye = endCam->eye;
            camera->eyeVel = endCam->eyeVel;
            break;
        }
        switch (endCam->lookAtMode)
        {
        case 2:
            chara = &lbl_1_bss_D3354.work->charas[endCam->lookAtChara];
            ape = chara->ape;
            mtxa_from_quat(&ape->rot);
            mtxa_to_mtx(mtx);
            mtxa_from_translate(&ape->pos);
            mtxa_scale_s(ape->scale);
            mtxa_translate(&ape->unk264);
            mtxa_mult_right(mtx);
            mtxa_tf_point(&endCam->lookAtOffset, &camera->lookAt);
            camera->eyeVel.x = 0.0f;
            camera->eyeVel.y = 0.0f;
            camera->eyeVel.z = 0.0f;
            break;
        default:
            camera->lookAt = endCam->lookAt;
            camera->lookAtVel = endCam->lookAtVel;
            break;
        }
        camera->unk1F = 1;
        camera->unk26 = 0;
        fn_1_10860C(camera, ball);
    }
}

void fn_1_10860C(struct Camera *camera, struct Ball *ball)
{
    Mtx mtx;
    Vec prevEye;
    Vec prevLookAt;
    Vec target;
    Vec curr;
    Vec vel;
    struct EndingCamera *endCam;
    struct EndingChara *chara;
    struct Ape *ape;
    register float dx, dy, dz;

    if (!(g_some_other_flags & 0xA) && lbl_1_bss_D3354.work != NULL)
    {
        endCam = &lbl_1_bss_D3354.work->camera;
        prevEye = camera->eye;
        prevLookAt = camera->lookAt;
        switch (endCam->eyeMode)
        {
        case 0:
            camera->eye = endCam->eye;
            break;
        case 1:
            target = endCam->eye;
            curr = camera->eye;
            vel = camera->eyeVel;
            vel.x += 0.5f * (0.05f * (target.x - curr.x) - vel.x);
            vel.y += 0.5f * (0.05f * (target.y - curr.y) - vel.y);
            vel.z += 0.5f * (0.05f * (target.z - curr.z) - vel.z);
            camera->eye.x += vel.x;
            camera->eye.y += vel.y;
            camera->eye.z += vel.z;
            break;
        case 2:
            chara = &lbl_1_bss_D3354.work->charas[endCam->eyeChara];
            ape = chara->ape;
            mtxa_from_quat(&ape->rot);
            mtxa_to_mtx(mtx);
            mtxa_from_translate(&ape->pos);
            mtxa_scale_s(ape->scale);
            mtxa_translate(&ape->unk264);
            mtxa_mult_right(mtx);
            mtxa_tf_point(&endCam->eyeOffset, &target);
            curr = camera->eye;
            vel = camera->eyeVel;
            vel.x += 0.5f * (0.05f * (target.x - curr.x) - vel.x);
            vel.y += 0.5f * (0.05f * (target.y - curr.y) - vel.y);
            vel.z += 0.5f * (0.05f * (target.z - curr.z) - vel.z);
            camera->eye.x += vel.x;
            camera->eye.y += vel.y;
            camera->eye.z += vel.z;
            break;
        }
        switch (endCam->lookAtMode)
        {
        case 0:
            camera->lookAt = endCam->lookAt;
            break;
        case 1:
            target = endCam->lookAt;
            curr = camera->lookAt;
            vel = camera->lookAtVel;
            vel.x += 0.2f * (0.1f * (target.x - curr.x) - vel.x);
            vel.y += 0.2f * (0.1f * (target.y - curr.y) - vel.y);
            vel.z += 0.2f * (0.1f * (target.z - curr.z) - vel.z);
            camera->lookAt.x += vel.x;
            camera->lookAt.y += vel.y;
            camera->lookAt.z += vel.z;
            break;
        case 2:
            chara = &lbl_1_bss_D3354.work->charas[endCam->lookAtChara];
            ape = chara->ape;
            mtxa_from_quat(&ape->rot);
            mtxa_to_mtx(mtx);
            mtxa_from_translate(&ape->pos);
            mtxa_scale_s(ape->scale);
            mtxa_translate(&ape->unk264);
            mtxa_mult_right(mtx);
            mtxa_tf_point(&endCam->lookAtOffset, &target);
            curr = camera->lookAt;
            vel = camera->lookAtVel;
            vel.x += 0.2f * (0.1f * (target.x - curr.x) - vel.x);
            vel.y += 0.2f * (0.1f * (target.y - curr.y) - vel.y);
            vel.z += 0.2f * (0.1f * (target.z - curr.z) - vel.z);
            camera->lookAt.x += vel.x;
            camera->lookAt.y += vel.y;
            camera->lookAt.z += vel.z;
            break;
        }
        camera->eyeVel.x = camera->eye.x - prevEye.x;
        camera->eyeVel.y = camera->eye.y - prevEye.y;
        camera->eyeVel.z = camera->eye.z - prevEye.z;
        camera->lookAtVel.x = camera->lookAt.x - prevLookAt.x;
        camera->lookAtVel.y = camera->lookAt.y - prevLookAt.y;
        camera->lookAtVel.z = camera->lookAt.z - prevLookAt.z;
        dx = camera->lookAt.x - camera->eye.x;
        dy = camera->lookAt.y - camera->eye.y;
        dz = camera->lookAt.z - camera->eye.z;
        camera->rotY = math_atan2(dx, dz) - 0x8000;
#ifdef __MWERKS__
        asm
        {
            fmuls dx, dx, dx
            fmadds dx, dz, dz, dx
        }
#else
        dx = dx * dx + dz * dz;
#endif
        camera->rotX = math_atan2(dy, math_sqrt(dx));
        camera->rotZ = 0;
    }
}
