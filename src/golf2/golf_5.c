/* Monkey Golf 2: the golf camera (.text 0x312F4-0x383A0).
 * fn_11_312F4 is the minigame camera callback (registered by _prolog through fn_1_D5D7C);
 * it dispatches on camera->unk1F to the per-mode helpers below. */
#include "golf.h"

/* main_loop globals not declared in the shared headers golf.h pulls in */
extern s32 lbl_1_bss_6FB68;
extern s32 lbl_1_bss_6FB6C;
extern struct Sphere lbl_1_bss_711B0;  /* the stage's bounding sphere */
extern s32 active_monkey_id[];
void call_SoundReqID_arg_0(int soundId);
void g_smth_with_ape_rotation(struct Ape *ape, Vec *dir);

/* .data */
float lbl_11_data_4E48 = 1.0f;  /* debug camera zoom */

/* .bss (defined after the zoom helper, which uses them by name) */
extern u32 lbl_11_bss_2B00;
extern s32 lbl_11_bss_2B04;
extern float lbl_11_bss_2B08;
extern s32 lbl_11_bss_2B0C;
extern float lbl_11_bss_2B10;
extern s32 lbl_11_bss_2B14;
extern Vec lbl_11_bss_2B18;
extern u8 lbl_11_bss_2B24;
extern u8 lbl_11_bss_2B25;
extern u8 lbl_11_bss_2B26;
extern Vec lbl_11_bss_2B28;
extern s32 lbl_11_bss_2B34;
extern u32 lbl_11_bss_2B38;
extern float lbl_11_bss_2B3C;
extern u8 lbl_11_bss_2B40[8];

void fn_11_33C24(struct Camera *camera, struct Ball *ball);
void fn_11_33DC4(struct Camera *camera, struct Ball *ball);
void fn_11_34400(struct Camera *camera, struct Ball *ball);
void fn_11_347B0(struct Camera *camera, struct Ball *ball);
void fn_11_348D0(struct Camera *camera, struct Ball *ball);
void fn_11_349F4(struct Camera *camera, struct Ball *ball);
void fn_11_36030(struct Camera *camera, struct Ball *ball);
void fn_11_37874(struct Camera *camera, struct Ball *ball);
void fn_11_37A34(struct Camera *camera, struct Ball *ball);
void fn_11_37A38(struct Camera *camera, struct Ball *ball);
void fn_11_38324(struct StageItemGroupAnim *anim, float time, Vec *out);

#define BALL_LOOK_AT_HEIGHT() (0.8 + (current_stage_id == 346 ? 0.0 : 0.18))
#define BALL_CAMERA_PITCH() (0x10000 - 0x900 - (current_stage_id == 346 ? 0 : 0x200))
#define CURR_PAD() (pad_status_groups[player_pad_map[modeCtrl.currPlayer]])

/* Aims the camera from eye at lookAt (yaw, then pitch). */
static inline void golf_cam_aim(struct Camera *camera)
{
    camera->rotY = math_atan2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z) - 0x8000;
    camera->rotX = math_atan2(camera->lookAt.y - camera->eye.y,
        math_sqrt(mathutil_sum_of_sq_2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z)));
}

/* Same, pitch first. */
static inline void golf_cam_aim_pitch_first(struct Camera *camera)
{
    camera->rotX = math_atan2(camera->lookAt.y - camera->eye.y,
        math_sqrt(mathutil_sum_of_sq_2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z)));
    camera->rotY = math_atan2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z) - 0x8000;
}

/* Aims along dir (dir = lookAt - eye). */
static inline void golf_cam_face_dir(struct Camera *camera, Vec *dir)
{
    camera->rotY = math_atan2(dir->x, dir->z) - 0x8000;
    camera->rotX = math_atan2(dir->y, math_sqrt(mathutil_sum_of_sq_2(dir->x, dir->z)));
    camera->rotZ = 0;
}

/* Length of v ignoring its y component. */
static inline float golf_vec_len_xz(Vec *v)
{
    return math_sqrt(mathutil_sum_of_sq_3(v->x, 0.0f, v->z));
}

/* Unreferenced (stripped at link): an out-of-line copy of the zoom below. Only its literals
 * survive, at the start of .rodata. */
void fn_11_golf_5_stripped(struct Camera *camera, struct Ball *ball, u32 timer, s32 *savedFov)
{
    Vec sp8;

    if (timer > 18.0f)
    {
        *savedFov = camera->sub28.view.fov;
        if (mathutil_vec_len(&ball->vel) > 0.0)
        {
            float dist = math_sqrt(mathutil_sum_of_sq_3(ball->pos.x - camera->eye.x, ball->pos.y - camera->eye.y,
                ball->pos.z - camera->eye.z));

            camera->sub28.view.fov = math_atan(100.0 / dist);
            if (camera->sub28.view.fov < 0.98f * *savedFov)
                camera->sub28.view.fov = 0.98f * *savedFov;
            if (camera->eye.y < ball->pos.y)
            {
                sp8.x = (float)(timer * timer) / 14400.0f;
                sp8.y = 0.0f;
                sp8.z = 0.0f;
                mtxa_from_identity();
                mtxa_rotate_y(ball->unk92);
                mtxa_tf_point(&sp8, &sp8);
                camera->eye.x += sp8.x;
                camera->eye.y += sp8.y;
                camera->eye.z += sp8.z;
                if (ball->pos.y - camera->eye.y > 1.0f)
                    camera->eye.y = camera->eye.y + (ball->pos.y - camera->eye.y) / 100.0;
                else
                    camera->eye.y = ball->pos.y;
            }
        }
        else
        {
            camera->sub28.view.fov = *savedFov;
            camera->sub28.view.fov *= 1.02;
            if (camera->sub28.view.fov > 0x2AAA)
                camera->sub28.view.fov = 0x2AAA;
        }
        golf_cam_aim_pitch_first(camera);
    }
}

/* Shot camera zoom: narrows the field of view as the ball flies away and lifts the eye.
 * Inlined into fn_11_312F4. */
static inline void fn_11_golf_5_zoom(struct Camera *camera, struct Ball *ball)
{
    Vec sp8;

    if (lbl_1_bss_6FB6C == 2 && lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14
     && lbl_11_bss_2B00 > 18.0f)
    {
        lbl_11_bss_2B0C = camera->sub28.view.fov;
        if (mathutil_vec_len(&ball->vel) > 0.0)
        {
            float dist = math_sqrt(mathutil_sum_of_sq_3(ball->pos.x - camera->eye.x, ball->pos.y - camera->eye.y,
                ball->pos.z - camera->eye.z));

            camera->sub28.view.fov = math_atan(100.0 / dist);
            if (camera->sub28.view.fov < 0.98f * lbl_11_bss_2B0C)
                camera->sub28.view.fov = 0.98f * lbl_11_bss_2B0C;
            if (camera->eye.y < ball->pos.y)
            {
                sp8.x = (float)(lbl_11_bss_2B00 * lbl_11_bss_2B00) / 14400.0f;
                sp8.y = 0.0f;
                sp8.z = 0.0f;
                mtxa_from_identity();
                mtxa_rotate_y(ball->unk92);
                mtxa_tf_point(&sp8, &sp8);
                camera->eye.x += sp8.x;
                camera->eye.y += sp8.y;
                camera->eye.z += sp8.z;
                if (ball->pos.y - camera->eye.y > 1.0f)
                    camera->eye.y = camera->eye.y + (ball->pos.y - camera->eye.y) / 100.0;
                else
                    camera->eye.y = ball->pos.y;
            }
        }
        else
        {
            camera->sub28.view.fov = lbl_11_bss_2B0C;
            camera->sub28.view.fov *= 1.02;
            if (camera->sub28.view.fov > 0x2AAA)
                camera->sub28.view.fov = 0x2AAA;
        }
        golf_cam_aim_pitch_first(camera);
    }
}

/* .bss */
u32 lbl_11_bss_2B00;    /* frames in the current shot */
s32 lbl_11_bss_2B04;    /* free-look camera: frames since the last button */
float lbl_11_bss_2B08;  /* debug cameras (modes 20/22): C-stick yaw */
s32 lbl_11_bss_2B0C;    /* saved field of view */
float lbl_11_bss_2B10;
s32 lbl_11_bss_2B14;    /* frame counter when the shot started */
Vec lbl_11_bss_2B18;    /* saved eye position / direction */
u8 lbl_11_bss_2B24;
u8 lbl_11_bss_2B25;
u8 lbl_11_bss_2B26;     /* set once the cup-overview direction is chosen */
Vec lbl_11_bss_2B28;    /* cup-overview direction */
s32 lbl_11_bss_2B34;    /* free-look camera: aim yaw */
u32 lbl_11_bss_2B38;    /* free-look camera: frames since start */
float lbl_11_bss_2B3C;  /* free-look camera: ground height under the look-at point */
u8 lbl_11_bss_2B40[8];

/* Unreferenced (stripped at link); pins the .bss order, which mwcc assigns by first
 * reference. */
void fn_11_golf_5_bss_order(void **out)
{
    out[0] = &lbl_11_bss_2B00;
    out[1] = &lbl_11_bss_2B04;
    out[2] = &lbl_11_bss_2B08;
    out[3] = &lbl_11_bss_2B0C;
    out[4] = &lbl_11_bss_2B10;
    out[5] = &lbl_11_bss_2B14;
    out[6] = &lbl_11_bss_2B18;
    out[7] = &lbl_11_bss_2B24;
    out[8] = &lbl_11_bss_2B25;
    out[9] = &lbl_11_bss_2B26;
    out[10] = &lbl_11_bss_2B28;
    out[11] = &lbl_11_bss_2B34;
    out[12] = &lbl_11_bss_2B38;
    out[13] = &lbl_11_bss_2B3C;
    out[14] = &lbl_11_bss_2B40;
}

/* Minigame camera callback: per-mode update selected by camera->unk1F. */
void fn_11_312F4(struct Camera *camera, struct Ball *ball)
{
    Vec prevEye;

    prevEye = camera->eye;
    camera->sub28.view.nearZ = 0.1f;
    if (g_some_other_flags & 0xA)
        return;
    if (fn_11_EC44() != 0)
        ball = &balls[modeCtrl.currPlayer];

    switch (camera->unk1F)
    {
    case 8:
        fn_1_D7110(0, 0x4000);
        lbl_11_bss_2B10 = 150.0f;
        camera->lookAt = ball->pos;
        golf_cam_aim(camera);
        camera->rotZ = 0;
        break;
    case 13:
        fn_1_D7110(0, 0x4000);
        lbl_11_bss_2B10 = 150.0f;
        break;
    case 11:
        lbl_11_bss_2B10 = 150.0f;
        fn_1_D7110(lbl_1_bss_2F48.infLightRotY, 0xC00);
        fn_11_37A38(camera, ball);
        break;
    case 0:
        lbl_11_bss_2B10 = 150.0f;
        fn_11_33C24(camera, ball);
        break;
    case 12:
        fn_1_D7110(0, 0x4000);
        fn_11_349F4(camera, ball);
        break;
    case 4:
        {
            s32 prevRotX;
            s32 prevRotZ;

            lbl_11_bss_2B14 = g_font_char_var_3 - 1;
            lbl_11_bss_2B00++;
            camera->eye.x += camera->eyeVel.x / lbl_11_bss_2B00;
            camera->eye.y += camera->eyeVel.y / lbl_11_bss_2B00;
            camera->eye.z += camera->eyeVel.z / lbl_11_bss_2B00;
            fn_11_golf_5_zoom(camera, ball);
            if (lbl_1_bss_6FB6C == 0 || lbl_11_bss_4C == 12 || lbl_11_bss_4C == 13 || lbl_11_bss_4C == 14)
            {
                if (lbl_11_bss_2B00 > 42.0f)
                {
                    Vec side;

                    if (lbl_11_bss_4C == 12 || lbl_11_bss_4C == 13 || lbl_11_bss_4C == 14)
                    {
                        camera->eye.x = ball->pos.x + 5.0 * ball->vel.x;
                        camera->eye.y = 10.0 + ball->pos.y;
                        camera->eye.z = ball->pos.z + 5.0 * ball->vel.z;
                    }
                    else
                    {
                        camera->eye.x = ball->pos.x + 30.0f * ball->vel.x;
                        camera->eye.y = 10.0f + (ball->pos.y + 30.0f * ball->vel.y);
                        camera->eye.z = ball->pos.z + 30.0f * ball->vel.z;
                    }
                    camera->lookAt = ball->pos;
                    side = ball->vel;
                    mtxa_from_identity();
                    mtxa_rotate_y(0x4000);
                    mtxa_tf_point(&side, &side);
                    vec_normalize_len(&side);
                    if (rand() / 32767.0f > 0.5)
                    {
                        camera->eye.x += 10.0f * side.x;
                        camera->eye.z += 10.0f * side.z;
                    }
                    else
                    {
                        camera->eye.x -= 10.0f * side.x;
                        camera->eye.z -= 10.0f * side.z;
                    }
                    camera->eyeVel = (Vec){0.0f, 0.0f, 0.0f};
                    camera->unk1F = 1;
                    lbl_11_bss_2B18 = camera->eye;
                    lbl_11_bss_2B10 = 0.0f;
                    golf_cam_aim_pitch_first(camera);
                }
                if (lbl_11_bss_2B00 == 1 && 1.0 == lbl_11_bss_D4 && 0.0 == lbl_11_bss_5C)
                    call_SoundReqID_arg_0(0x38B);
                if (lbl_11_bss_2B00 == 0.5f * lbl_80198B38 && 1.0 == lbl_11_bss_D4 && 0.0 == lbl_11_bss_5C)
                {
                    int r = (rand() & 0x7FFF) % 3;

                    g_player_id_for_sound = modeCtrl.currPlayer;
                    g_smth_with_sound = active_monkey_id[g_player_id_for_sound];
                    if (r == 0)
                        call_SoundReqID_arg_0(0x130);
                    else if (r == 1)
                        call_SoundReqID_arg_0(0x12F);
                    else
                        call_SoundReqID_arg_0(0x12E);
                }
                if (lbl_11_bss_2B00 == 0.7f * lbl_80198B38)
                {
                    lbl_11_bss_74 = 1;
                    lbl_11_bss_78 = 60;
                    if (lbl_11_bss_D4 > 0.8 && fabs(lbl_11_bss_5C) < 0.1 && lbl_11_bss_4C <= 2)
                        call_SoundReqID_arg_0(0x368);
                    lbl_11_bss_70 = 0;
                    lbl_11_bss_6C = 0;
                }
            }
            fn_1_D7110(0, 0x4000);
            camera->lookAt = balls[modeCtrl.currPlayer].pos;
            camera->lookAt.y += 1.0;
            prevRotX = camera->rotX;
            prevRotZ = camera->rotZ;
            golf_cam_aim_pitch_first(camera);
            camera->rotX = prevRotX + (camera->rotX - prevRotX) / 10.0f;
            camera->rotZ = prevRotZ + (camera->rotZ - prevRotZ) / 10.0f;
            if (lbl_1_bss_6FB6C == 1 && lbl_11_bss_2B00 > 90.0)
            {
                Vec dir;

                camera->unk1F = 1;
                dir.x = lbl_11_bss_38.x - ball->pos.x;
                dir.y = 0.0f;
                dir.z = lbl_11_bss_38.z - ball->pos.z;
                vec_normalize_len(&dir);
                mtxa_from_identity();
                mtxa_rotate_y(-16384.0f + 32768.0f * (rand() / 32767.0f));
                mtxa_tf_vec(&dir, &dir);
                camera->eye.x = ball->pos.x + 50.0 * (dir.x / mathutil_vec_len(&dir) * mathutil_vec_len(&ball->vel));
                camera->eye.y = 10.0 + ball->pos.y;
                camera->eye.z = ball->pos.z + 50.0 * (dir.z / mathutil_vec_len(&dir) * mathutil_vec_len(&ball->vel));
                camera->lookAt = balls[modeCtrl.currPlayer].pos;
                golf_cam_aim_pitch_first(camera);
                lbl_11_bss_2B18 = camera->eye;
                lbl_11_bss_2B10 = 0.0f;
            }
        }
        break;
    case 3:
        lbl_11_bss_2B14 = g_font_char_var_3 - 1;
        lbl_11_bss_2B00++;
        if (lbl_11_bss_2B00 > 30)
        {
            lbl_11_bss_2B00 = 0;
            camera->unk1F = 4;
        }
        fn_1_D7110(0, 0x4000);
        fn_11_36030(camera, ball);
        break;
    case 20:
        /* debug: orbit the ball with the C stick */
        lbl_11_bss_2B10 = 150.0f;
        camera->sub28.view.fov = 0x2AAA;
        {
            Vec eye = {0.0f, 0.0f, 0.0f};
            Vec lookAt = {0.0f, 0.0f, 0.0f};

            if (CURR_PAD().unk0[0].substickX != 0)
                lbl_11_bss_2B08 += CURR_PAD().unk0[0].substickX / 6000.0f;
            if (CURR_PAD().unk0[0].substickY != 0)
                lbl_11_data_4E48 -= CURR_PAD().unk0[0].substickY / 6000.0f;
            if (lbl_11_bss_2B08 > 1.0)
                lbl_11_bss_2B08 = 1.0f;
            if (lbl_11_bss_2B08 < -1.0)
                lbl_11_bss_2B08 = -1.0f;
            if (lbl_11_data_4E48 > 2.0)
                lbl_11_data_4E48 = 2.0f;
            if (lbl_11_data_4E48 < 0.1)
                lbl_11_data_4E48 = 0.1f;
            mtxa_from_translate(&ball->pos);
            mtxa_rotate_y(ball->unk92);
            mtxa_translate_xyz(30.0f * lbl_11_bss_2B08 * lbl_11_data_4E48, 20.0f * lbl_11_data_4E48, 35.0f * lbl_11_data_4E48);
            mtxa_tf_point(&eye, &eye);
            camera->eye.x = camera->eye.x + (eye.x - camera->eye.x) / 10.0;
            camera->eye.y = camera->eye.y + (eye.y - camera->eye.y) / 10.0;
            camera->eye.z = camera->eye.z + (eye.z - camera->eye.z) / 10.0;
            mtxa_rotate_y(4096.0f * lbl_11_bss_2B08);
            mtxa_translate_xyz(0.0f, -8.0f, -40.0f);
            mtxa_tf_point(&lookAt, &lookAt);
            camera->lookAt.x = camera->lookAt.x + (lookAt.x - camera->lookAt.x) / 10.0;
            camera->lookAt.y = camera->lookAt.y + (lookAt.y - camera->lookAt.y) / 10.0;
            camera->lookAt.z = camera->lookAt.z + (lookAt.z - camera->lookAt.z) / 10.0;
            golf_cam_aim(camera);
            camera->rotZ = 0;
        }
        break;
    case 22:
        lbl_11_bss_2B10 = 150.0f;
        camera->sub28.view.fov = 0x2AAA;
        {
            Vec eye = {0.0f, 0.0f, 0.0f};
            Vec target;
            Vec rayPos;
            struct RaycastHit hit2;

            if (CURR_PAD().unk0[0].substickX != 0)
                lbl_11_bss_2B08 += CURR_PAD().unk0[0].substickX / 6000.0f;
            if (CURR_PAD().unk0[0].substickY != 0)
                lbl_11_data_4E48 -= CURR_PAD().unk0[0].substickY / 6000.0f;
            if (lbl_11_bss_2B08 > 1.0)
                lbl_11_bss_2B08 = -1.0f;
            if (lbl_11_bss_2B08 < -1.0)
                lbl_11_bss_2B08 = 1.0f;
            if (lbl_11_data_4E48 > 1.0)
                lbl_11_data_4E48 = 1.0f;
            if (lbl_11_data_4E48 < 0.0)
                lbl_11_data_4E48 = 0.0f;
            if (lbl_11_bss_4C == 12 || lbl_11_bss_4C == 13 || lbl_11_bss_4C == 14)
            {
                target.x = -math_sin(ball->unk92);
                target.y = -200.0f;
                target.z = -math_sin(ball->unk92 + 0x4000);
                if (lbl_11_bss_4C == 12)
                {
                target.x = ball->pos.x + 50.0f * target.x * lbl_11_bss_94;
                target.y = -200.0f;
                target.z = ball->pos.z + 50.0f * target.z * lbl_11_bss_94;
                }
                else if (lbl_11_bss_4C == 13)
                {
                target.x = ball->pos.x + 30.0f * target.x * lbl_11_bss_94;
                target.y = -200.0f;
                target.z = ball->pos.z + 30.0f * target.z * lbl_11_bss_94;
                }
                else if (lbl_11_bss_4C == 14)
                {
                target.x = ball->pos.x + 10.0f * target.x * lbl_11_bss_94;
                target.y = -200.0f;
                target.z = ball->pos.z + 10.0f * target.z * lbl_11_bss_94;
                }
                rayPos = target;
                rayPos.y = 500.0f + ball->pos.y;
                if ((s32)raycast_stage_down(&rayPos, &hit2, NULL) != 0)
                    target = hit2.pos;
                else
                    target.y = 0.0f;
                lbl_11_bss_50 = 1;
                lbl_11_bss_198[0] = target;
                mtxa_push();
                mtxa_from_translate(&target);
                mtxa_rotate_y(32768.0f * lbl_11_bss_2B08);
                mtxa_translate_xyz(0.0f, 20.0f + CURR_PAD().unk0[0].substickY / 30.0f, -50.0f);
                mtxa_tf_point(&eye, &eye);
                mtxa_pop();
                camera->eye.x = camera->eye.x + (eye.x - camera->eye.x) / 10.0;
                camera->eye.y = camera->eye.y + (eye.y - camera->eye.y) / 10.0;
                camera->eye.z = camera->eye.z + (eye.z - camera->eye.z) / 10.0;
                camera->lookAt.x = camera->lookAt.x + (target.x - camera->lookAt.x) / 10.0;
                camera->lookAt.y = camera->lookAt.y + (target.y - camera->lookAt.y) / 10.0;
                camera->lookAt.z = camera->lookAt.z + (target.z - camera->lookAt.z) / 10.0;
            }
            else
            {
                mtxa_push();
                mtxa_from_translate(&lbl_11_bss_198[lbl_11_bss_50 - 1]);
                mtxa_rotate_y(32768.0f * lbl_11_bss_2B08);
                mtxa_translate_xyz(0.0f, 60.0f + CURR_PAD().unk0[0].substickY / 6.0f, -150.0f);
                mtxa_tf_point(&eye, &eye);
                mtxa_pop();
                camera->eye.x = camera->eye.x + (eye.x - camera->eye.x) / 10.0;
                camera->eye.y = camera->eye.y + (eye.y - camera->eye.y) / 10.0;
                camera->eye.z = camera->eye.z + (eye.z - camera->eye.z) / 10.0;
                camera->lookAt.x = camera->lookAt.x + (lbl_11_bss_198[lbl_11_bss_50 - 1].x - camera->lookAt.x) / 10.0;
                camera->lookAt.y = camera->lookAt.y + (lbl_11_bss_198[lbl_11_bss_50 - 1].y - camera->lookAt.y) / 10.0;
                camera->lookAt.z = camera->lookAt.z + (lbl_11_bss_198[lbl_11_bss_50 - 1].z - camera->lookAt.z) / 10.0;
            }
            golf_cam_aim(camera);
            camera->rotZ = 0;
        }
        break;
    case 21:
        {
            Vec to;
            Vec from;
            Vec target;
            Vec rayPos;
            struct RaycastHit hit2;
            float midX, midY, midZ;

            camera->sub28.view.nearZ = 1.0f;
            lbl_11_bss_2B08 = 0.0f;
            lbl_11_data_4E48 = 1.0f;
            lbl_11_bss_2B10 = 150.0f;
            if (lbl_11_bss_4C == 12 || lbl_11_bss_4C == 13 || lbl_11_bss_4C == 14)
            {
                target.x = -math_sin(ball->unk92);
                target.y = -200.0f;
                target.z = -math_sin(ball->unk92 + 0x4000);
                if (lbl_11_bss_4C == 12)
                {
                target.x = ball->pos.x + 50.0f * target.x * lbl_11_bss_94;
                target.y = -200.0f;
                target.z = ball->pos.z + 50.0f * target.z * lbl_11_bss_94;
                }
                else if (lbl_11_bss_4C == 13)
                {
                target.x = ball->pos.x + 30.0f * target.x * lbl_11_bss_94;
                target.y = -200.0f;
                target.z = ball->pos.z + 30.0f * target.z * lbl_11_bss_94;
                }
                else if (lbl_11_bss_4C == 14)
                {
                target.x = ball->pos.x + 10.0f * target.x * lbl_11_bss_94;
                target.y = -200.0f;
                target.z = ball->pos.z + 10.0f * target.z * lbl_11_bss_94;
                }
                rayPos = target;
                rayPos.y = 500.0f + ball->pos.y;
                if ((s32)raycast_stage_down(&rayPos, &hit2, NULL) != 0)
                    target = hit2.pos;
                else
                    target.y = 0.0f;
                lbl_11_bss_50 = 1;
                lbl_11_bss_198[0] = target;
                to = target;
                from = balls[modeCtrl.currPlayer].pos;
            }
            else
            {
                to = lbl_11_bss_198[lbl_11_bss_50 - 1];
                from = balls[modeCtrl.currPlayer].pos;
            }
            midX = 0.35 * to.x + 0.65 * from.x;
            midY = 0.35 * to.y + 0.65 * from.y;
            midZ = 0.35 * to.z + 0.65 * from.z;
            {
                float dist = math_sqrt(mathutil_sum_of_sq_3(to.x - midX, to.y - midY, to.z - midZ));
                from.y = from.y + 2.0 * dist;
            }
            from.x = from.x - 0.5 * (to.x - midX);
            from.z = from.z - 0.5 * (to.z - midZ);
            camera->lookAt.x = camera->lookAt.x + (midX - camera->lookAt.x) / 10.0;
            camera->lookAt.y = camera->lookAt.y + (midY - camera->lookAt.y) / 10.0;
            camera->lookAt.z = camera->lookAt.z + (midZ - camera->lookAt.z) / 10.0;
            camera->eye.x = camera->eye.x + (from.x - camera->eye.x) / 10.0;
            camera->eye.y = camera->eye.y + (from.y - camera->eye.y) / 10.0;
            camera->eye.z = camera->eye.z + (from.z - camera->eye.z) / 10.0;
            golf_cam_aim(camera);
            camera->rotZ = 0;
            camera->sub28.view.fov = math_atan(0.6f) * 2;
            camera->sub28.view.tanHalfFov = math_tan(camera->sub28.view.fov >> 1);
        }
        break;
    case 23:
        {
            Vec rayPos;
            struct RaycastHit hit2;
            Vec offset;

            lbl_11_bss_2B08 = 0.0f;
            lbl_11_data_4E48 = 1.0f;
            lbl_11_bss_2B10 = 150.0f;
            camera->lookAt = ball->pos;
            camera->eye = (Vec){0.0f, 0.0f, 0.0f};
            camera->eyeVel = (Vec){0.0f, 0.0f, 0.0f};
            if (lbl_11_bss_B40 == 0)
            {
                mtxa_from_identity();
                mtxa_translate(&ball->pos);
                mtxa_rotate_y(ball->unk92 - 0x2000);
                mtxa_translate_xyz(0.0f, -0.3f, -2.0f);
                mtxa_tf_point(&camera->eye, &camera->eye);
                rayPos = camera->eye;
                rayPos.y = 300.0f;
                raycast_stage_down(&rayPos, &hit2, NULL);
                if (hit2.flags != 0 && camera->eye.y < hit2.pos.y)
                    camera->eye.y = 0.5 + hit2.pos.y;
            }
            else if (lbl_11_bss_B40 == 1)
            {
                mtxa_from_identity();
                mtxa_translate(&ball->pos);
                mtxa_rotate_y(ball->unk92 - 0x6000);
                mtxa_translate_xyz(0.0f, 0.5f, -2.0f);
                mtxa_tf_point(&camera->eye, &camera->eye);
                rayPos = camera->eye;
                rayPos.y = 1000.0f;
                raycast_stage_down(&rayPos, &hit2, NULL);
                if (hit2.flags != 0 && camera->eye.y < hit2.pos.y)
                    camera->eye.y = 0.5 + hit2.pos.y;
            }
            else
            {
                camera->lookAt.x = ball->pos.x;
                camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
                camera->lookAt.z = ball->pos.z;
                mtxa_from_translate(&camera->lookAt);
                mtxa_rotate_y(ball->unk92);
                offset.x = 0.0f;
                offset.y = 0.0f;
                offset.z = 3.0f;
                mtxa_tf_point(&offset, &camera->eye);
                offset.x = 0.0f;
                offset.y = 0.0f;
                offset.z = 3.0f;
                mtxa_tf_point(&offset, &camera->unkB4);
            }
            golf_cam_aim(camera);
            camera->rotZ = 0;
        }
        break;
    case 2:
        lbl_11_bss_2B08 = 0.0f;
        lbl_11_data_4E48 = 1.0f;
        lbl_11_bss_2B10 = 150.0f;
        /* fall through */
    case 5:
        lbl_11_bss_2B14 = g_font_char_var_3 - 1;
        /* fall through */
    case 7:
    case 19:
        lbl_11_bss_2B00 = 0;
        fn_1_D7110(0, 0x4000);
        fn_11_36030(camera, ball);
        break;
    case 6:
        lbl_11_bss_2B10 = 150.0f;
        lbl_11_bss_2B00 = 0;
        /* fall through */
    case 1:
        lbl_11_bss_2B14 = g_font_char_var_3 - 1;
        fn_1_D7110(lbl_1_bss_2F48.infLightRotY, 0xC00);
        fn_11_36030(camera, ball);
        break;
    case 14:
        camera->sub28.view.nearZ = 5.0f;
        lbl_11_bss_2B00 = 0;
        fn_1_D7110(0, 0x4000);
        fn_11_348D0(camera, ball);
        break;
    case 9:
        fn_11_37874(camera, ball);
        break;
    case 10:
        fn_11_37A34(camera, ball);
        break;
    case 15:
        fn_11_33DC4(camera, ball);
        break;
    case 16:
    case 17:
        fn_11_34400(camera, ball);
        break;
    case 18:
        fn_11_347B0(camera, ball);
        break;
    }

    /* keep the eye above the ground */
    if (camera->unk1F != 6 && camera->unk1F != 7 && camera->unk1F != 8 && camera->unk1F != 19)
    {
        Vec sp38;
        struct RaycastHit hit;

        sp38 = camera->eye;
        sp38.y += 2.0f;
        if ((s32)raycast_stage_down(&sp38, &hit, NULL) != 0)
        {
            if (1.0f + hit.pos.y > camera->eye.y)
            {
                camera->eye.y = 1.0f + hit.pos.y;
                sp38.x = camera->lookAt.x - camera->eye.x;
                sp38.y = camera->lookAt.y - camera->eye.y;
                sp38.z = camera->lookAt.z - camera->eye.z;
                golf_cam_face_dir(camera, &sp38);
            }
        }
        else
        {
            sp38 = camera->eye;
            sp38.y += 5.0f;
            if ((s32)raycast_stage_down(&sp38, &hit, NULL) != 0)
            {
                if (1.0f + hit.pos.y > camera->eye.y)
                {
                    camera->eye.y = 1.0f + hit.pos.y;
                    sp38.x = camera->lookAt.x - camera->eye.x;
                    sp38.y = camera->lookAt.y - camera->eye.y;
                    sp38.z = camera->lookAt.z - camera->eye.z;
                    golf_cam_face_dir(camera, &sp38);
                }
            }
            else
            {
                sp38 = camera->eye;
                sp38.y += 25.0f;
                if ((s32)raycast_stage_down(&sp38, &hit, NULL) != 0 && 1.0f + hit.pos.y > camera->eye.y)
                {
                    camera->eye.y = 1.0f + hit.pos.y;
                    sp38.x = camera->lookAt.x - camera->eye.x;
                    sp38.y = camera->lookAt.y - camera->eye.y;
                    sp38.z = camera->lookAt.z - camera->eye.z;
                    golf_cam_face_dir(camera, &sp38);
                }
            }
        }
    }
}

/* Mode 0: start behind the ball, facing along the start position's heading. */
void fn_11_33C24(struct Camera *camera, struct Ball *ball)
{
    Vec sp8;

    camera_clear(camera);
    lbl_11_bss_2B24 = 0;
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(stagedef->startPos->yrot);
    sp8.x = 0.0f;
    sp8.y = 1.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &camera->eye);
    sp8.x = 0.0f;
    sp8.y = 0.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &camera->unkB4);
    sp8.x = camera->lookAt.x - camera->eye.x;
    sp8.y = camera->lookAt.y - camera->eye.y;
    sp8.z = camera->lookAt.z - camera->eye.z;
    golf_cam_face_dir(camera, &sp8);
    lbl_11_bss_2B25 = 2;
    lbl_11_bss_2B26 = 0;
    camera->unk1F = 1;
}

/* Mode 15: follow camera behind the ball (main_loop's g_camera_func1, restarting from
 * behind the ball every frame). */
void fn_11_33DC4(struct Camera *camera, struct Ball *ball)
{
    s16 pitch;
    s16 yaw;
    Vec sp14;
    Vec sp8;
    Vec prevEye;
    Vec prevLookAt;
    float f1;
    int r3;
    int r4;

    if (g_some_other_flags & 0xA)
        return;

    prevEye.x = camera->eye.x;
    prevEye.y = camera->eye.y;
    prevEye.z = camera->eye.z;

    prevLookAt.x = camera->lookAt.x;
    prevLookAt.y = camera->lookAt.y;
    prevLookAt.z = camera->lookAt.z;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(ball->unk92);
    sp14.x = 0.0f;
    sp14.y = 1.0f;
    sp14.z = 3.0f;
    mtxa_tf_point(&sp14, &camera->eye);
    sp14.x = 0.0f;
    sp14.y = 0.0f;
    sp14.z = 3.0f;
    mtxa_tf_point(&sp14, &camera->unkB4);

    sp14.x = camera->unkB4.x - prevLookAt.x;
    sp14.y = camera->unkB4.y - prevLookAt.y;
    sp14.z = camera->unkB4.z - prevLookAt.z;

    f1 = mathutil_sum_of_sq_3(sp14.x, sp14.y, sp14.z);
    if (f1 > FLT_EPSILON)
    {
        f1 = math_rsqrt(f1);
        sp14.x *= f1;
        sp14.y *= f1;
        sp14.z *= f1;
    }
    else
    {
        sp14.x = 1.0f;
        sp14.y = 0.0f;
        sp14.z = 0.0f;
    }

    sp14.x = sp14.x * 0.75 + prevLookAt.x;
    sp14.y = sp14.y * 0.75 + prevLookAt.y;
    sp14.z = sp14.z * 0.75 + prevLookAt.z;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;

    sp14.x = camera->lookAt.x - sp14.x;
    sp14.y = camera->lookAt.y - sp14.y;
    sp14.z = camera->lookAt.z - sp14.z;

    if (ball->unk80 < 60)
        pitch = 0;
    else
        pitch = math_atan2(sp14.y, math_sqrt(mathutil_sum_of_sq_2(sp14.x, sp14.z)));

    yaw = math_atan2(sp14.x, sp14.z) - 32768;
    r3 = (s16)(yaw - camera->rotY);
    r3 = CLAMP(r3, -512, 512);
    yaw = camera->rotY + r3;
    if (!(camera->flags & (1 << 1)) && !(ball->flags & BALL_FLAG_GOAL))
    {
        r3 = (s16)(ball->unk92 - yaw);
        if (r3 > 0x800)
            r3 -= 0x800;
        else if (r3 < -0x800)
            r3 += 0x800;
        else
            r3 = 0;
        r3 >>= 7;
        r4 = camera->unk114;
        if (r3 == 0)
            r4 = 0;
        else if ((r4 < 0 && r3 > 0) || (r4 > 0 && r3 < 0))
            r4 = 0;
        else if (r3 < 0)
        {
            if (r3 < r4 - 4)
                r4 -= 4;
            else
                r4 = r3;
        }
        else
        {
            if (r3 > r4 + 4)
                r4 += 4;
            else
                r4 = r3;
        }

        yaw += r4;
        r3 = (s16)(yaw - camera->rotY);
        r3 = CLAMP(r3, -768, 768);
        yaw = camera->rotY + r3;
    }

    if (pitch < -6144)
        pitch = -6144;
    else if (pitch > 6144)
        pitch = 6144;
    pitch = camera->unkC0 = camera->unkC0 + 0.2 * (pitch - camera->unkC0);

    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(yaw);
    mtxa_rotate_x(pitch);
    sp14.x = 0.0f;
    sp14.y = 0.0f;
    sp14.z = 3.0f;
    sp8 = camera->unkB4;
    mtxa_tf_point(&sp14, &camera->unkB4);
    camera->unk114 = yaw - camera->rotY;
    camera->rotY = yaw;
    camera->rotX = pitch + BALL_CAMERA_PITCH();
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    sp14.x = 0.0f;
    sp14.y = 0.0f;
    sp14.z = math_sqrt(mathutil_sum_of_sq_2(3.0f, 1.0f));
    mtxa_tf_point(&sp14, &camera->eye);

    camera->eyeVel.x = camera->eye.x - prevEye.x;
    camera->eyeVel.y = camera->eye.y - prevEye.y;
    camera->eyeVel.z = camera->eye.z - prevEye.z;

    camera->lookAtVel.x = camera->lookAt.x - prevLookAt.x;
    camera->lookAtVel.y = camera->lookAt.y - prevLookAt.y;
    camera->lookAtVel.z = camera->lookAt.z - prevLookAt.z;
}

/* Modes 16/17: overview of the hole from beside the line between ball and cup. */
void fn_11_34400(struct Camera *camera, struct Ball *ball)
{
    float dist;
    Vec dir;

    camera->unk26 = 0;
    camera->lookAt.x = (lbl_11_bss_38.x + ball->pos.x) / 2.0;
    camera->lookAt.y = (1.46 + lbl_11_bss_38.y + ball->pos.y) / 2.0;
    camera->lookAt.z = (lbl_11_bss_38.z + ball->pos.z) / 2.0;
    dist = math_sqrt(mathutil_sum_of_sq_3(camera->lookAt.x - ball->pos.x, camera->lookAt.y - ball->pos.y,
        camera->lookAt.z - ball->pos.z));
    dist = dist / math_tan(0x1555);
    if (dist < 9.0)
        dist = 9.0f;
    if (camera->unk1F == 16)
    {
        dir.x = lbl_11_bss_38.x - ball->pos.x;
        dir.y = 0.0f;
        dir.z = lbl_11_bss_38.z - ball->pos.z;
        vec_normalize_len(&dir);
        mtxa_from_identity();
        if (fn_11_EC30() == 15)
            mtxa_rotate_y(0x6000);
        else
            mtxa_rotate_y(0x2000);
        mtxa_tf_vec(&dir, &dir);
        if (stagedef->extraAnims != NULL)
        {
            camera->eye.x = lbl_11_bss_38.x + dist * (dir.x / mathutil_vec_len(&dir));
            camera->eye.y = lbl_11_bss_38.y + 0.5 * dist;
            camera->eye.z = lbl_11_bss_38.z + dist * (dir.z / mathutil_vec_len(&dir));
        }
        lbl_11_bss_2B28 = dir;
    }
    else
    {
        camera->eye.x = lbl_11_bss_38.x + dist * (lbl_11_bss_2B28.x / mathutil_vec_len(&lbl_11_bss_2B28));
        camera->eye.y = lbl_11_bss_38.y + 0.5 * dist;
        camera->eye.z = lbl_11_bss_38.z + dist * (lbl_11_bss_2B28.z / mathutil_vec_len(&lbl_11_bss_2B28));
    }
    golf_cam_aim_pitch_first(camera);
    camera->unk114 = 0;
    camera->unk1F = 17;
}

/* Mode 18: slow orbit around the whole stage. */
void fn_11_347B0(struct Camera *camera, struct Ball *ball)
{
    camera->lookAt = lbl_1_bss_711B0.pos;
    camera->eye.x = lbl_1_bss_711B0.radius;
    camera->eye.y = 0.0f;
    camera->eye.z = 0.0f;
    mtxa_from_identity();
    mtxa_translate(&lbl_1_bss_711B0.pos);
    mtxa_rotate_y(g_font_char_var_3 << 6);
    mtxa_rotate_z(0x1000);
    mtxa_tf_point(&camera->eye, &camera->eye);
    camera->rotY = math_atan2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z) - 0x8000;
    camera->rotX = math_atan2(camera->lookAt.y - camera->eye.y,
        math_sqrt(mathutil_sum_of_sq_2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z)));
    camera->rotZ = 0;
}

/* Mode 14: the same orbit (shown with a nearer clip plane). */
void fn_11_348D0(struct Camera *camera, struct Ball *ball)
{
    camera->lookAt = lbl_1_bss_711B0.pos;
    camera->eye.x = lbl_1_bss_711B0.radius;
    camera->eye.y = 0.0f;
    camera->eye.z = 0.0f;
    mtxa_from_identity();
    mtxa_translate(&lbl_1_bss_711B0.pos);
    mtxa_rotate_y((s16)g_font_char_var_3 << 6);
    mtxa_rotate_z(0x1000);
    mtxa_tf_point(&camera->eye, &camera->eye);
    camera->rotY = math_atan2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z) - 0x8000;
    camera->rotX = math_atan2(camera->lookAt.y - camera->eye.y,
        math_sqrt(mathutil_sum_of_sq_2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z)));
    camera->rotZ = 0;
}

/* Mode 12: free-look camera around the ball (stick orbits/zooms, C stick pans, X/Y raise and
 * lower the eye; B or A leaves). */
void fn_11_349F4(struct Camera *camera, struct Ball *ball)
{
    Vec v2C;
    Vec v20;
    Vec v14;
    Vec v8;
    struct RaycastHit hit;
    float y;

    if (CURR_PAD().unk0[2].button & PAD_BUTTON_B)
    {
        v8.x = -math_sin(lbl_11_bss_2B34);
        v8.y = 0.0f;
        v8.z = -math_sin(lbl_11_bss_2B34 + 0x4000);
        if (balls[modeCtrl.currPlayer].ape != NULL)
            g_smth_with_ape_rotation(balls[modeCtrl.currPlayer].ape, &v8);
        call_SoundReqID_arg_0(0x10D);
        camera->unk1F = 2;
        return;
    }
    if (CURR_PAD().unk0[2].button & PAD_BUTTON_A)
    {
        call_SoundReqID_arg_0(0x10);
        camera->unk1F = 2;
        return;
    }
    if (lbl_11_bss_2B38 > 30.0)
    {
        if (CURR_PAD().unk0[0].stickX != 0)
        {
            mtxa_from_identity();
            mtxa_translate_xyz(camera->lookAt.x, camera->lookAt.y, camera->lookAt.z);
            mtxa_rotate_y((CURR_PAD().unk0[0].stickX << 8) / 60.0);
            mtxa_translate_xyz(-camera->lookAt.x, -camera->lookAt.y, -camera->lookAt.z);
            mtxa_tf_point(&camera->eye, &camera->eye);
            camera->rotX = math_atan2(camera->lookAt.y - camera->eye.y,
                math_sqrt(mathutil_sum_of_sq_2(camera->lookAt.x - camera->eye.x, camera->lookAt.z - camera->eye.z)));
        }
        if (CURR_PAD().unk0[0].stickY != 0)
        {
            v20.x = camera->lookAt.x - camera->eye.x;
            v20.y = 0.0f;
            v20.z = camera->lookAt.z - camera->eye.z;
            v2C = v20;
            v20.x = -CURR_PAD().unk0[0].stickY * (4.0 * (v2C.x / mathutil_vec_len(&v2C))) / 60.0 / 4.0;
            v20.z = -CURR_PAD().unk0[0].stickY * (4.0 * (v2C.z / mathutil_vec_len(&v2C))) / 60.0 / 4.0;
            camera->eye.x -= v20.x;
            camera->eye.z -= v20.z;
            v2C.x = -camera->lookAt.x + camera->eye.x;
            v2C.y = 0.0f;
            v2C.z = -camera->lookAt.z + camera->eye.z;
            if (mathutil_vec_len(&v2C) < 3.0)
            {
                camera->eye.x = camera->lookAt.x + 3.0 * v2C.x / mathutil_vec_len(&v2C);
                camera->eye.z = camera->lookAt.z + 3.0 * v2C.z / mathutil_vec_len(&v2C);
            }
        }
        if (CURR_PAD().unk0[0].stickX != 0 || CURR_PAD().unk0[0].stickY != 0)
        {
            golf_cam_aim(camera);
            camera->rotZ = 0;
        }
        if (CURR_PAD().unk0[0].substickX != 0)
        {
            v2C.x = camera->lookAt.x - camera->eye.x;
            v2C.z = camera->lookAt.z - camera->eye.z;
            v20.x = -v2C.z;
            v20.y = 0.0f;
            v20.z = v2C.x;
            v2C = v20;
            v20.x = -CURR_PAD().unk0[0].substickX * (4.0 * (v2C.x / mathutil_vec_len(&v2C))) / 60.0 / 4.0;
            v20.z = -CURR_PAD().unk0[0].substickX * (4.0 * (v2C.z / mathutil_vec_len(&v2C))) / 60.0 / 4.0;
            v2C.x = camera->eye.x - camera->lookAt.x;
            v2C.y = camera->eye.y - camera->lookAt.y;
            v2C.z = camera->eye.z - camera->lookAt.z;
            y = camera->lookAt.y;
            camera->lookAt.x -= v20.x;
            camera->lookAt.z -= v20.z;
            camera->lookAt.y = 100.0f;
            raycast_stage_down(&camera->lookAt, &hit, NULL);
            if (hit.flags != 0)
            {
                lbl_11_bss_2B3C = 1.0 + hit.pos.y;
                camera->lookAt.y = y + (lbl_11_bss_2B3C - y) / 15.0;
                camera->eye.x = v2C.x + camera->lookAt.x;
                camera->eye.y = v2C.y + camera->lookAt.y;
                camera->eye.z = v2C.z + camera->lookAt.z;
            }
            else
            {
                camera->eye.x -= v20.x;
                camera->eye.z -= v20.z;
                camera->lookAt.x = camera->eye.x - v2C.x;
                camera->lookAt.y = camera->eye.y - v2C.y;
                camera->lookAt.z = camera->eye.z - v2C.z;
            }
        }
        if (CURR_PAD().unk0[0].substickY != 0)
        {
            v20.x = camera->lookAt.x - camera->eye.x;
            v20.y = 0.0f;
            v20.z = camera->lookAt.z - camera->eye.z;
            v2C = v20;
            v20.x = -CURR_PAD().unk0[0].substickY * (4.0 * (v2C.x / mathutil_vec_len(&v2C))) / 60.0 / 4.0;
            v20.z = -CURR_PAD().unk0[0].substickY * (4.0 * (v2C.z / mathutil_vec_len(&v2C))) / 60.0 / 4.0;
            v2C.x = camera->eye.x - camera->lookAt.x;
            v2C.y = camera->eye.y - camera->lookAt.y;
            v2C.z = camera->eye.z - camera->lookAt.z;
            y = camera->lookAt.y;
            camera->lookAt.x -= v20.x;
            camera->lookAt.z -= v20.z;
            camera->lookAt.y = 100.0f;
            raycast_stage_down(&camera->lookAt, &hit, NULL);
            if (hit.flags != 0)
            {
                lbl_11_bss_2B3C = 1.0 + hit.pos.y;
                camera->lookAt.y = y + (lbl_11_bss_2B3C - y) / 15.0;
                camera->eye.x = v2C.x + camera->lookAt.x;
                camera->eye.y = v2C.y + camera->lookAt.y;
                camera->eye.z = v2C.z + camera->lookAt.z;
            }
            else
            {
                camera->eye.x -= v20.x;
                camera->eye.z -= v20.z;
                camera->lookAt.x = camera->eye.x - v2C.x;
                camera->lookAt.y = camera->eye.y - v2C.y;
                camera->lookAt.z = camera->eye.z - v2C.z;
            }
        }
        v2C.x = camera->lookAt.x - camera->eye.x;
        v2C.y = camera->lookAt.y - camera->eye.y;
        v2C.z = camera->lookAt.z - camera->eye.z;
        camera->lookAt.y = camera->lookAt.y + (lbl_11_bss_2B3C - camera->lookAt.y) / 15.0;
        camera->eye.x = camera->lookAt.x - v2C.x;
        camera->eye.y = camera->lookAt.y - v2C.y;
        camera->eye.z = camera->lookAt.z - v2C.z;
        lbl_11_bss_2B04++;
        if (CURR_PAD().unk0[2].button & PAD_BUTTON_X)
        {
            call_SoundReqID_arg_0(0x10C);
            lbl_11_bss_2B04 = 0;
        }
        if (CURR_PAD().unk0[2].button & PAD_BUTTON_Y)
        {
            call_SoundReqID_arg_0(0x10D);
            lbl_11_bss_2B04 = 0;
        }
        if (CURR_PAD().unk0[0].button & PAD_BUTTON_X)
        {
            camera->eye.y += 0.2;
            if (camera->eye.y > 20.0 + camera->lookAt.y)
                camera->eye.y = 20.0 + camera->lookAt.y;
            golf_cam_aim(camera);
        }
        if (CURR_PAD().unk0[0].button & PAD_BUTTON_Y)
        {
            camera->eye.y -= 0.2;
            if (camera->eye.y < 1.0 + camera->lookAt.y)
                camera->eye.y = 1.0 + camera->lookAt.y;
            golf_cam_aim(camera);
        }
        if (camera->eye.y - 1.0 < camera->lookAt.y)
        {
            camera->eye.y = 1.0 + camera->lookAt.y;
            golf_cam_aim(camera);
        }
    }
    else
    {
        lbl_11_bss_2B3C = camera->lookAt.y;
        v14.x = -math_sin(balls[modeCtrl.currPlayer].unk92);
        v14.y = 0.0f;
        v14.z = -math_sin(balls[modeCtrl.currPlayer].unk92 + 0x4000);
        camera->lookAt.x = balls[modeCtrl.currPlayer].pos.x + 0.01 * v14.x / mathutil_vec_len(&v14);
        camera->lookAt.z = balls[modeCtrl.currPlayer].pos.z + 0.01 * v14.z / mathutil_vec_len(&v14);
        lbl_11_bss_2B38++;
    }

    /* keep the look-at point and the eye inside the stage's bounding sphere */
    v20.x = camera->eye.x - camera->lookAt.x;
    v20.y = camera->eye.y - camera->lookAt.y;
    v20.z = camera->eye.z - camera->lookAt.z;
    v2C.x = camera->lookAt.x - lbl_1_bss_711B0.pos.x;
    v2C.y = camera->lookAt.y - lbl_1_bss_711B0.pos.y;
    v2C.z = camera->lookAt.z - lbl_1_bss_711B0.pos.z;
    if (mathutil_vec_len(&v2C) > 1.2 * (lbl_1_bss_711B0.radius / 2.0f))
    {
        camera->lookAt.x = lbl_1_bss_711B0.pos.x + v2C.x * (1.2 * (lbl_1_bss_711B0.radius / 2.0f) / mathutil_vec_len(&v2C));
        camera->lookAt.y = lbl_1_bss_711B0.pos.y + v2C.y * (1.2 * (lbl_1_bss_711B0.radius / 2.0f) / mathutil_vec_len(&v2C));
        camera->lookAt.z = lbl_1_bss_711B0.pos.z + v2C.z * (1.2 * (lbl_1_bss_711B0.radius / 2.0f) / mathutil_vec_len(&v2C));
        camera->eye.x = v20.x + camera->lookAt.x;
        camera->eye.y = v20.y + camera->lookAt.y;
        camera->eye.z = v20.z + camera->lookAt.z;
    }
    v20.x = camera->eye.x - camera->lookAt.x;
    v20.y = camera->eye.y - camera->lookAt.y;
    v20.z = camera->eye.z - camera->lookAt.z;
    v2C.x = camera->eye.x - lbl_1_bss_711B0.pos.x;
    v2C.y = camera->eye.y - lbl_1_bss_711B0.pos.y;
    v2C.z = camera->eye.z - lbl_1_bss_711B0.pos.z;
    if (mathutil_vec_len(&v2C) > 1.5 * (lbl_1_bss_711B0.radius / 2.0f))
    {
        camera->eye.x = lbl_1_bss_711B0.pos.x + v2C.x * (1.5 * (lbl_1_bss_711B0.radius / 2.0f) / mathutil_vec_len(&v2C));
        camera->eye.y = lbl_1_bss_711B0.pos.y + v2C.y * (1.5 * (lbl_1_bss_711B0.radius / 2.0f) / mathutil_vec_len(&v2C));
        camera->eye.z = lbl_1_bss_711B0.pos.z + v2C.z * (1.5 * (lbl_1_bss_711B0.radius / 2.0f) / mathutil_vec_len(&v2C));
        golf_cam_aim(camera);
    }
    v2C.x = camera->lookAt.x - balls[modeCtrl.currPlayer].pos.x;
    v2C.y = 0.0f;
    v2C.z = camera->lookAt.z - balls[modeCtrl.currPlayer].pos.z;
    if (balls[modeCtrl.currPlayer].ape != NULL)
        g_smth_with_ape_rotation(balls[modeCtrl.currPlayer].ape, &v2C);
}

/* Shot camera (modes 1-7, 19): follows the ball like main_loop's g_camera_func1, then adds the
 * per-mode framing (behind the putter, overview of the cup, flight follow). */
void fn_11_36030(struct Camera *camera, struct Ball *ball)
{
    s16 pitch;
    s16 yaw;
    Vec sp5C;
    Vec sp50;
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    struct RaycastHit hit;
    struct RaycastHit hit2;
    Vec prevEye;
    Vec prevLookAt;
    float f1;
    int r3;
    int r4;

    if (g_some_other_flags & 0xA)
        return;

    prevEye.x = camera->eye.x;
    prevEye.y = camera->eye.y;
    prevEye.z = camera->eye.z;

    prevLookAt.x = camera->lookAt.x;
    prevLookAt.y = camera->lookAt.y;
    prevLookAt.z = camera->lookAt.z;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(ball->unk92);
    sp5C.x = 0.0f;
    sp5C.y = 1.0f;
    sp5C.z = 3.0f;
    mtxa_tf_point(&sp5C, &camera->eye);
    sp5C.x = 0.0f;
    sp5C.y = 0.0f;
    sp5C.z = 3.0f;
    mtxa_tf_point(&sp5C, &camera->unkB4);

    sp5C.x = camera->unkB4.x - prevLookAt.x;
    sp5C.y = camera->unkB4.y - prevLookAt.y;
    sp5C.z = camera->unkB4.z - prevLookAt.z;

    f1 = mathutil_sum_of_sq_3(sp5C.x, sp5C.y, sp5C.z);
    if (f1 > FLT_EPSILON)
    {
        f1 = math_rsqrt(f1);
        sp5C.x *= f1;
        sp5C.y *= f1;
        sp5C.z *= f1;
    }
    else
    {
        sp5C.x = 1.0f;
        sp5C.y = 0.0f;
        sp5C.z = 0.0f;
    }

    sp5C.x = sp5C.x * 0.75 + prevLookAt.x;
    sp5C.y = sp5C.y * 0.75 + prevLookAt.y;
    sp5C.z = sp5C.z * 0.75 + prevLookAt.z;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;

    sp5C.x = camera->lookAt.x - sp5C.x;
    sp5C.y = camera->lookAt.y - sp5C.y;
    sp5C.z = camera->lookAt.z - sp5C.z;

    if (ball->unk80 < 60)
        pitch = 0;
    else
        pitch = math_atan2(sp5C.y, math_sqrt(mathutil_sum_of_sq_2(sp5C.x, sp5C.z)));

    yaw = math_atan2(sp5C.x, sp5C.z) - 32768;
    r3 = (s16)(yaw - camera->rotY);
    r3 = CLAMP(r3, -512, 512);
    yaw = camera->rotY + r3;
    if (!(camera->flags & (1 << 1)) && !(ball->flags & BALL_FLAG_GOAL))
    {
        if (camera->unk1F == 7 || camera->unk1F == 19)
            r3 = (s16)(ball->unk92 - yaw - 0x8000);
        else
            r3 = (s16)(ball->unk92 - yaw);
        if (r3 > 0x800)
            r3 -= 0x800;
        else if (r3 < -0x800)
            r3 += 0x800;
        else
            r3 = 0;
        r3 >>= 7;
        r4 = camera->unk114;
        if (r3 == 0)
            r4 = 0;
        else if ((r4 < 0 && r3 > 0) || (r4 > 0 && r3 < 0))
            r4 = 0;
        else if (r3 < 0)
        {
            if (r3 < r4 - 4)
                r4 -= 4;
            else
                r4 = r3;
        }
        else
        {
            if (r3 > r4 + 4)
                r4 += 4;
            else
                r4 = r3;
        }

        yaw += r4;
        r3 = (s16)(yaw - camera->rotY);
        r3 = CLAMP(r3, -768, 768);
        yaw = camera->rotY + r3;
        if (camera->unk1F == 2 || camera->unk1F == 5)
        {
            if ((u32)fn_11_EBE0() == 0)
                yaw = ball->unk92;
            else
                yaw = yaw + (ball->unk92 - yaw) / 80.0;
        }
    }
    if (camera->unk1F == 7 || camera->unk1F == 19)
        yaw = ball->unk92 + 0x8000;

    if (pitch < -6144)
        pitch = -6144;
    else if (pitch > 6144)
        pitch = 6144;
    pitch = camera->unkC0 = camera->unkC0 + 0.2 * (pitch - camera->unkC0);

    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(yaw);
    mtxa_rotate_x(pitch);
    sp5C.x = 0.0f;
    sp5C.y = 0.0f;
    sp5C.z = 3.0f;
    sp44 = camera->unkB4;
    mtxa_tf_point(&sp5C, &camera->unkB4);
    camera->unk114 = yaw - camera->rotY;
    camera->rotY = yaw;
    camera->rotX = pitch + BALL_CAMERA_PITCH();
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    sp5C.x = 0.0f;
    sp5C.y = 0.0f;
    sp5C.z = math_sqrt(mathutil_sum_of_sq_2(3.0f, 1.0f));
    mtxa_tf_point(&sp5C, &camera->eye);

    if (camera->unk1F == 3 || camera->unk1F == 4)
    {
        if (mathutil_vec_len(&ball->vel) < 1.0)
        {
            camera->eye.x = prevEye.x + (camera->eye.x - prevEye.x) * mathutil_vec_len(&ball->vel);
            camera->eye.y = prevEye.y + (camera->eye.y - prevEye.y) * mathutil_vec_len(&ball->vel);
            camera->eye.z = prevEye.z + (camera->eye.z - prevEye.z) * mathutil_vec_len(&ball->vel);
        }
        sp50 = camera->eye;
        sp50.y += 100.0f;
        raycast_stage_down(&sp50, &hit, NULL);
        if (hit.flags != 0 && 0.5 + hit.pos.y > camera->eye.y)
            camera->eye.y = 0.5 + hit.pos.y;
        golf_cam_aim(camera);
        camera->rotZ = 0;
    }
    else if (camera->unk1F == 2 || camera->unk1F == 5)
    {
        if (lbl_11_bss_74 == 0)
        {
            camera->eye.x = prevEye.x + (camera->eye.x - prevEye.x) / 10.0;
            camera->eye.y = prevEye.y + (camera->eye.y - prevEye.y) / 10.0;
            camera->eye.z = prevEye.z + (camera->eye.z - prevEye.z) / 10.0;
        }
        golf_cam_aim(camera);
        camera->rotZ = 0;
    }

    camera->eyeVel.x = camera->eye.x - prevEye.x;
    camera->eyeVel.y = camera->eye.y - prevEye.y;
    camera->eyeVel.z = camera->eye.z - prevEye.z;

    camera->lookAtVel.x = camera->lookAt.x - prevLookAt.x;
    camera->lookAtVel.y = camera->lookAt.y - prevLookAt.y;
    camera->lookAtVel.z = camera->lookAt.z - prevLookAt.z;

    camera->sub28.view.fov = 0x2AAA;

    switch (camera->unk1F)
    {
    case 7:
    case 19:
        {
            int par = fn_11_16FA4();

            if (fn_11_EC00(modeCtrl.currPlayer, fn_11_EC30()) <= par)
            {
                if (camera->unk1F == 7)
                    camera->lookAt.y -= 0.5;
                camera->eye.y += 0.5;
                golf_cam_aim(camera);
                camera->rotZ = 0;
                break;
            }
            sp38.x = 0.0f;
            sp38.y = 4.0 + (double)math_sin((g_font_char_var_3 - lbl_11_bss_2B14) << 11)
                / ((g_font_char_var_3 - lbl_11_bss_2B14) / 30.0f);
            sp38.z = 1.0 + 0.25 * math_sin((g_font_char_var_3 - lbl_11_bss_2B14) << 11)
                / ((g_font_char_var_3 - lbl_11_bss_2B14) / 30.0f);
            mtxa_from_identity();
            mtxa_translate(&balls[modeCtrl.currPlayer].pos);
            mtxa_rotate_y(balls[modeCtrl.currPlayer].unk92 + 0x8000);
            mtxa_tf_point(&sp38, &sp38);
            camera->eye = sp38;
            golf_cam_aim(camera);
            camera->rotZ = 0;
        }
        break;
    case 5:
        lbl_11_bss_2B26 = 0;
        break;
    case 2:
        lbl_11_bss_2B26 = 0;
        break;
    case 6:
        /* prevEye is dead here; the original reuses its z as the zoom distance (keeps f31) */
        {
            camera->unk26 = 0;
            camera->lookAt.x = (lbl_11_bss_38.x + ball->pos.x) / 2.0;
            camera->lookAt.y = (1.46 + lbl_11_bss_38.y + ball->pos.y) / 2.0;
            camera->lookAt.z = (lbl_11_bss_38.z + ball->pos.z) / 2.0;
            prevEye.z = math_sqrt(mathutil_sum_of_sq_3(camera->lookAt.x - ball->pos.x, camera->lookAt.y - ball->pos.y,
                camera->lookAt.z - ball->pos.z));
            prevEye.z *= 3.0f;
            if (prevEye.z < 9.0)
                prevEye.z = 9.0f;
            if (lbl_11_bss_2B26 == 0)
            {
                sp2C.x = lbl_11_bss_38.x - ball->pos.x;
                sp2C.y = 0.0f;
                sp2C.z = lbl_11_bss_38.z - ball->pos.z;
                vec_normalize_len(&sp2C);
                mtxa_from_identity();
                mtxa_rotate_y(0x4000);
                mtxa_tf_vec(&sp2C, &sp2C);
                if (stagedef->extraAnims != NULL)
                {
                    camera->eye.x = lbl_11_bss_38.x + prevEye.z * (sp2C.x / mathutil_vec_len(&sp2C));
                    camera->eye.y = 10.0 + lbl_11_bss_38.y;
                    camera->eye.z = lbl_11_bss_38.z + prevEye.z * (sp2C.z / mathutil_vec_len(&sp2C));
                }
                lbl_11_bss_2B18 = sp2C;
                lbl_11_bss_2B26 = 1;
            }
            else
            {
                camera->eye.x = lbl_11_bss_38.x + prevEye.z * (lbl_11_bss_2B18.x / mathutil_vec_len(&lbl_11_bss_2B18));
                camera->eye.y = 10.0 + lbl_11_bss_38.y;
                camera->eye.z = lbl_11_bss_38.z + prevEye.z * (lbl_11_bss_2B18.z / mathutil_vec_len(&lbl_11_bss_2B18));
            }
            golf_cam_aim_pitch_first(camera);
            camera->unk114 = 0;
            prevEye.z = math_sqrt(mathutil_sum_of_sq_3(camera->lookAt.x - camera->eye.x, camera->lookAt.y - camera->eye.y,
                camera->lookAt.z - camera->eye.z));
            camera->sub28.view.fov = 2.5 * math_atan(math_sqrt(mathutil_sum_of_sq_3(camera->lookAt.x - ball->pos.x,
                camera->lookAt.y - ball->pos.y, camera->lookAt.z - ball->pos.z)) / prevEye.z);
            if (camera->sub28.view.fov < 0x2800)
                camera->sub28.view.fov = 0x2800;
            if (camera->sub28.view.fov > 0x3800)
                camera->sub28.view.fov = 0x3800;
        }
        break;
    case 1:
        if (!(CURR_PAD().unk0[0].button & PAD_BUTTON_X) && lbl_11_bss_2B10 >= 150.0)
        {
            if (lbl_1_bss_6FB6C == 1)
            {
                sp20.x = lbl_11_bss_38.x - ball->pos.x;
                sp20.y = 0.0f;
                sp20.z = lbl_11_bss_38.z - ball->pos.z;
                vec_normalize_len(&sp20);
                mtxa_from_identity();
                mtxa_rotate_y(-16384.0f + 32768.0f * (rand() / 32767.0f));
                mtxa_tf_vec(&sp20, &sp20);
                camera->eye.x = ball->pos.x + 50.0 * (sp20.x / mathutil_vec_len(&sp20));
                camera->eye.y = 10.0 + ball->pos.y;
                camera->eye.z = ball->pos.z + 50.0 * (sp20.z / mathutil_vec_len(&sp20));
                lbl_11_bss_2B18 = camera->eye;
                lbl_11_bss_2B10 = -600.0f;
                printf("new camera pos1\n");
            }
            else if (lbl_1_bss_6FB6C == 0)
            {
                if (lbl_11_bss_4C == 12 || lbl_11_bss_4C == 13 || lbl_11_bss_4C == 14)
                {
                    camera->eye.x = ball->pos.x + 15.0 * (rand() / 32767.0f);
                    camera->eye.z = ball->pos.z + 15.0 * (rand() / 32767.0f);
                    mtxa_from_identity();
                    mtxa_translate(&ball->pos);
                    mtxa_rotate_y(65536.0f * (rand() / 32767.0f));
                    mtxa_translate_xyz(-ball->pos.x, -ball->pos.y, -ball->pos.z);
                    mtxa_tf_point(&camera->eye, &camera->eye);
                }
                else
                {
                    camera->eye = lbl_11_bss_18C;
                    camera->eye.z += 100.0f;
                    mtxa_from_identity();
                    mtxa_translate(&lbl_11_bss_18C);
                    mtxa_rotate_y(65536.0f * (rand() / 32767.0f));
                    mtxa_translate_xyz(-lbl_11_bss_18C.x, -lbl_11_bss_18C.y, -lbl_11_bss_18C.z);
                    mtxa_tf_point(&camera->eye, &camera->eye);
                }
                camera->eye.y += 30.0f;
                camera->lookAt = ball->pos;
                lbl_11_bss_2B18 = camera->eye;
                lbl_11_bss_2B10 = -600.0f;
                camera->sub28.view.fov = 0x2AAA;
                printf("new camera pos2\n");
            }
        }
        else
        {
            s32 found;

            sp14 = ball->pos;
            found = raycast_stage_down(&sp14, &hit2, NULL);
            camera->eye = lbl_11_bss_2B18;
            lbl_11_bss_2B10 += 1.0f;
            if (ball->pos.y < 30.0f + hit2.pos.y)
                lbl_11_bss_B38 = 0;
            if ((CURR_PAD().unk0[0].button & PAD_BUTTON_X) && mathutil_vec_len(&ball->vel) > 0.01f
             && ball->pos.y > 30.0f + hit2.pos.y && found != 0 && lbl_11_bss_B38 != 0)
            {
                sp8 = ball->vel;
                vec_normalize_len(&sp8);
                camera->eye.x = camera->lookAt.x - 5.0 * sp8.x;
                camera->eye.y = 1.0 + (camera->lookAt.y - 5.0 * sp8.y);
                camera->eye.z = camera->lookAt.z - 5.0 * sp8.z;
            }
            else if (lbl_11_bss_2B10 >= 150.0)
            {
                lbl_11_bss_74 = 1;
                lbl_11_bss_78 = 60;
                lbl_11_bss_6C = 0;
            }
        }
        camera->lookAt = ball->pos;
        golf_cam_aim_pitch_first(camera);
        if (lbl_1_bss_6FB68 == 1)
        {
            if (mathutil_vec_len(&ball->vel) > 0.8)
            {
                float dist = math_sqrt(mathutil_sum_of_sq_3(ball->pos.x - camera->eye.x, ball->pos.y - camera->eye.y,
                    ball->pos.z - camera->eye.z));

                camera->sub28.view.fov = math_atan(40.0 / dist);
            }
            else
            {
                camera->sub28.view.fov = lbl_11_bss_2B0C;
                camera->sub28.view.fov *= 1.02;
                if (camera->sub28.view.fov > 0x2AAA)
                    camera->sub28.view.fov = 0x2AAA;
            }
            lbl_11_bss_2B0C = camera->sub28.view.fov;
        }
        lbl_11_bss_2B26 = 0;
        break;
    }
}

/* Mode 9: top-down view of the whole hole (or the stage), then mode 10. */
void fn_11_37874(struct Camera *camera, struct Ball *ball)
{
    Vec center;
    float radius;
    s8 found;

    fn_11_16B68(&found, &radius, &center);
    if (found)
    {
        if (radius < 100.0f)
            radius = 100.0f;
        lbl_11_bss_29F0 = radius;
        lbl_11_bss_29E4 = center;
    }
    else
    {
        lbl_11_bss_29F0 = lbl_1_bss_711B0.radius;
        lbl_11_bss_29E4 = lbl_1_bss_711B0.pos;
    }
    camera_clear(camera);
    camera->unk1F = 10;
    camera->lookAt.x = 0.00044444f * lbl_11_bss_29E4.x;
    camera->lookAt.y = 0.00044444f * lbl_11_bss_29E4.y;
    camera->lookAt.z = 0.00044444f * lbl_11_bss_29E4.z;
    camera->eye.x = 0.00044444f * lbl_11_bss_29E4.x;
    camera->eye.z = 0.00044444f * lbl_11_bss_29E4.z;
    camera->eye.y = 3.99996f;
    camera->rotX = -0x4000;
    camera->rotY = 0;
    camera->rotZ = 0;
    camera->sub28.view.fov = math_atan(lbl_11_bss_29F0 / 2.0f / 9000.0) * 2;
}

/* Mode 10: hold the top-down view. */
void fn_11_37A34(struct Camera *camera, struct Ball *ball) {}

/* Mode 11: hole fly-by before the shot: follows the stage's camera animation if it has one,
 * otherwise flies from the start position to the cup and back. */
void fn_11_37A38(struct Camera *camera, struct Ball *ball)
{
    Vec dir;
    float dx;
    float dz;

    if (stagedef->extraAnims != NULL && stagedef->extraAnims->animA != NULL && stagedef->extraAnims->animB != NULL)
    {
        float t = 900 - fn_11_EBF0();

        t /= 900.0f;
        t *= 100.0;

        fn_11_38324(stagedef->extraAnims->animB, t, &camera->eye);
        fn_11_38324(stagedef->extraAnims->animA, t, &camera->lookAt);
        golf_cam_aim(camera);
        camera->rotZ = 0;
        return;
    }
    if (fn_11_EBF0() > 450)
    {
        dir.x = lbl_11_bss_38.x - camera->eye.x;
        dir.y = 0.0f;
        dir.z = lbl_11_bss_38.z - camera->eye.z;
        dx = dir.x / mathutil_vec_len(&dir);
        dz = dir.z / mathutil_vec_len(&dir);
        camera->eye.x = (stagedef->startPos->pos.x - 20.0 * dx)
            + ((lbl_11_bss_38.x - 20.0 * dx) - (stagedef->startPos->pos.x - 20.0 * dx)) * (900 - fn_11_EBF0()) / 450.0;
        camera->eye.y = 20.0f;
        camera->eye.z = (stagedef->startPos->pos.z - 20.0 * dz)
            + ((lbl_11_bss_38.z - 20.0 * dz) - (stagedef->startPos->pos.z - 20.0 * dz)) * (900 - fn_11_EBF0()) / 450.0;
        dir.x = lbl_11_bss_38.x - camera->eye.x;
        dir.y = lbl_11_bss_38.y - camera->eye.y;
        dir.z = lbl_11_bss_38.z - camera->eye.z;
        camera->lookAt.x = camera->eye.x + 5.0 * (dir.x / golf_vec_len_xz(&dir));
        camera->lookAt.y = camera->eye.y - 2.5;
        camera->lookAt.z = camera->eye.z + 5.0 * (dir.z / golf_vec_len_xz(&dir));
        golf_cam_aim(camera);
        camera->rotZ = 0;
        return;
    }
    dir.x = lbl_11_bss_38.x - camera->eye.x;
    dir.y = 0.0f;
    dir.z = lbl_11_bss_38.z - camera->eye.z;
    dx = dir.x / mathutil_vec_len(&dir);
    dz = dir.z / mathutil_vec_len(&dir);
    camera->eye.x = (stagedef->startPos->pos.x - 20.0 * dx)
        + ((lbl_11_bss_38.x - 20.0 * dx) - (stagedef->startPos->pos.x - 20.0 * dx)) * (float)fn_11_EBF0() / 450.0;
    camera->eye.y = 20.0 + 80.0 * math_sin((int)(32768.0f * fn_11_EBF0() / 450.0f));
    camera->eye.z = (stagedef->startPos->pos.z - 20.0 * dz)
        + ((lbl_11_bss_38.z - 20.0 * dz) - (stagedef->startPos->pos.z - 20.0 * dz)) * (float)fn_11_EBF0() / 450.0;
    dir.x = lbl_11_bss_38.x - camera->eye.x;
    dir.y = lbl_11_bss_38.y - camera->eye.y;
    dir.z = lbl_11_bss_38.z - camera->eye.z;
    camera->lookAt.x = lbl_11_bss_38.x * fn_11_EBF0() / 450.0f + stagedef->startPos->pos.x * (450 - fn_11_EBF0()) / 450.0f;
    camera->lookAt.y = lbl_11_bss_38.y * fn_11_EBF0() / 450.0f + stagedef->startPos->pos.y * (450 - fn_11_EBF0()) / 450.0f;
    camera->lookAt.z = lbl_11_bss_38.z * fn_11_EBF0() / 450.0f + stagedef->startPos->pos.z * (450 - fn_11_EBF0()) / 450.0f;
    golf_cam_aim(camera);
    camera->rotZ = 0;
}

/* Evaluates the position keyframes of a stage animation at the given time. */
void fn_11_38324(struct StageItemGroupAnim *anim, float time, Vec *out)
{
    out->x = evaluate_stagedef_keyframe(time, anim->posXKeyframeCount, anim->posXKeyframes);
    out->y = evaluate_stagedef_keyframe(time, anim->posYKeyframeCount, anim->posYKeyframes);
    out->z = evaluate_stagedef_keyframe(time, anim->posZKeyframeCount, anim->posZKeyframes);
}
