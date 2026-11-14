/* wormhole.c - Moves a ball and its camera from one wormhole to its
 * destination. The transform is built from a view matrix at each end. */
#pragma peephole off
#include "types.h"
#include "ball.h"
#include "camera.h"
#include "libc.h"
#include "mathutil.h"
#include "stage.h"

struct WormholeTf
{
    /*0x00*/ Mtx mtx;
    /*0x30*/ struct StageWormhole *src;
    /*0x34*/ struct ItemGroup *srcGroup;
    /*0x38*/ struct StageWormhole *dest;
    /*0x3C*/ struct ItemGroup *destGroup;
};

struct Unk_D4290
{
    /*0x00*/ s64 unk0;
    /*0x08*/ u8 unk8;
    /*0x09*/ u8 unk9;
    /*0x0A*/ u8 unkA;
};

extern u32 lbl_1_bss_CF220[2];

void C_MTXLookAt(Mtx m, Vec *pos, Vec *up, Vec *target);
void call_SoundReqID_arg_0(int);
void g_smth_with_wormhole_replays(int);
void quat_mult(Quat *dst, Quat *a, Quat *b);
void mtxa_to_quat(Quat *q);

struct WormholeTf *g_compute_wormhole_tf(int wormholeId, struct WormholeTf *out);
void g_mtx_mult_right(Mtx m, Mtx tf);
void apply_wormhole_tf_to_mtx(Mtx m, struct WormholeTf *tf);
void apply_wormhole_tf_to_vec(Vec *v, struct WormholeTf *tf);
void g_apply_wormhole_pos_to_camera(Vec *pos, Vec *vel, struct WormholeTf *tf);
void apply_wormhole_tf_to_quat(Quat *q, struct WormholeTf *tf);
void fn_1_10B288(Quat *q, struct WormholeTf *tf);

static const float s_zero = 0.0f;
static const float s_height = 2.2f;
static const float s_one = 1.0f;
static const float s_negOne = -1.0f;

static struct Unk_D4290 lbl_1_bss_D4290;
static u8 lbl_1_bss_D42A0[0x88];

#ifdef __MWERKS__
#define BALL ((struct Ball *)wormholeId)
#else
#define BALL (&balls[playerId])  /* PC: don't round-trip the pointer through int */
#endif

void teleport_through_wormhole(int playerId, int wormholeId)
{
    Vec pos;
    struct WormholeTf tf;
    struct Camera *camera;

    lbl_1_bss_CF220[0] |= 1;
    mtxa_push();
    g_compute_wormhole_tf(wormholeId, &tf);

    /* The ball pointer shares a variable with wormholeId here */
#ifdef __MWERKS__
    wormholeId = (int)&balls[playerId];
#endif
    pos = BALL->pos;
    g_apply_wormhole_pos_to_camera(&pos, &BALL->vel, &tf);
    g_mtx_mult_right(BALL->unk30, tf.mtx);
    mtxa_from_mtx(BALL->unk30);
    mathutil_mtxA_get_translate(&BALL->pos);
    mtxa_to_euler_yxz(&BALL->rotY, &BALL->rotX, &BALL->rotZ);
    g_mtx_mult_right(BALL->unkD0, tf.mtx);
    mtxa_from_mtx(BALL->unkD0);
    mathutil_mtxA_get_translate(&BALL->prevPos);
    fn_1_10B288(&BALL->unkA0, &tf);
    apply_wormhole_tf_to_quat(&BALL->unkB0, &tf);
    apply_wormhole_tf_to_vec(&BALL->unk11C, &tf);
    g_smth_with_wormhole_replays(playerId);

    camera = &cameras[playerId];
    apply_wormhole_tf_to_mtx(camera->unk14C, &tf);
    apply_wormhole_tf_to_mtx(camera->unk17C, &tf);
    apply_wormhole_tf_to_mtx(camera->unk1AC, &tf);
    apply_wormhole_tf_to_mtx(camera->unk1DC, &tf);
    g_apply_wormhole_pos_to_camera(&camera->eye, &camera->eyeVel, &tf);
    g_apply_wormhole_pos_to_camera(&camera->lookAt, &camera->lookAtVel, &tf);
    mtxa_push();
    mtxa_from_mtx(tf.mtx);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    mtxa_rotate_z(camera->rotZ);
    mtxa_to_euler_yxz(&camera->rotY, &camera->rotX, &camera->rotZ);
    mtxa_pop();
    camera->flags |= 0x100;
    call_SoundReqID_arg_0(0x317);
    mtxa_pop();
}

#undef BALL

struct WormholeTf *g_compute_wormhole_tf(int wormholeId, struct WormholeTf *out)
{
    Mtx view;
    struct WormholeTf tf;
    Vec eye;
    Vec target;
    Vec up;
    u32 destId;
    struct StageWormhole *src;
    struct StageWormhole *dest;
    struct StageCollHeader *hdr;
    int i;
    int count;
    int n;

    src = &stagedef->wormholes[wormholeId];
    dest = src->dest;
    destId = ((u32)dest - (u32)stagedef->wormholes) / sizeof(*dest);
    tf.src = src;
    tf.dest = dest;

    mtxa_push();
    n = wormholeId;
    count = stagedef->itemGroupCount;
    hdr = stagedef->collHeaders;
    for (i = 0; i < count; i++, hdr++)
    {
        n -= hdr->wormholeCount;
        if (n < 0)
        {
            tf.srcGroup = &itemgroups[i];
            mtxa_from_mtx(tf.srcGroup->transform);
            break;
        }
    }
    mtxa_translate(&src->pos);
    mtxa_rotate_z(src->rotZ);
    mtxa_rotate_y(src->rotY);
    mtxa_rotate_x(src->rotX);
    mtxa_translate_xyz(s_zero, s_height, s_zero);
    mathutil_mtxA_get_translate(&eye);
    mtxa_tf_vec_xyz(s_zero, s_one, s_zero, &up);
    mtxa_tf_point_xyz(s_zero, s_zero, s_negOne, &target);
    C_MTXLookAt(mtxa->mtxA, &eye, &up, &target);
    mtxa_to_mtx(view);

    n = destId;
    count = stagedef->itemGroupCount;
    hdr = stagedef->collHeaders;
    for (i = 0; i < count; i++, hdr++)
    {
        n -= hdr->wormholeCount;
        if (n < 0)
        {
            tf.destGroup = &itemgroups[i];
            mtxa_from_mtx(tf.destGroup->transform);
            break;
        }
    }
    mtxa_translate(&dest->pos);
    mtxa_rotate_z(dest->rotZ);
    mtxa_rotate_y(dest->rotY);
    mtxa_rotate_x(dest->rotX);
    mtxa_translate_xyz(s_zero, s_height, s_zero);
    mtxa_tf_vec_xyz(s_zero, s_one, s_zero, &up);
    mtxa_tf_point_xyz(s_zero, s_zero, s_one, &target);
    mathutil_mtxA_get_translate(&eye);
    C_MTXLookAt(mtxa->mtxA, &eye, &up, &target);
    mtxa_rigid_invert();
    mtxa_mult_right(view);
    mtxa_to_mtx(tf.mtx);
    mtxa_pop();
    *out = tf;
    return out;
}

void g_mtx_mult_right(Mtx m, Mtx tf)
{
    mtxa_from_mtx(tf);
    mtxa_mult_right(m);
    mtxa_to_mtx(m);
}

void apply_wormhole_tf_to_mtx(Mtx m, struct WormholeTf *tf)
{
    Mtx inv;

    mtxa_from_mtx(m);
    mtxa_rigid_invert();
    mtxa_to_mtx(inv);
    mtxa_from_mtx(m);
    mtxa_mult_right(tf->mtx);
    mtxa_mult_right(inv);
    mtxa_to_mtx(m);
}

void apply_wormhole_tf_to_vec(Vec *v, struct WormholeTf *tf)
{
    mtxa_from_mtx(tf->mtx);
    mtxa_tf_vec(v, v);
}

void g_apply_wormhole_pos_to_camera(Vec *pos, Vec *vel, struct WormholeTf *tf)
{
    Vec b;
    Vec a;
    Vec delta;
    Vec tfDelta;
    struct ItemGroup *group;

    delta = *vel;

    group = tf->srcGroup;
    mtxa_from_mtx(group->prevTransform);
    mtxa_rigid_inv_tf_point(pos, &a);
    mtxa_from_mtx(group->transform);
    mtxa_rigid_inv_tf_point(pos, &b);
    delta.x = b.x - a.x;
    delta.y = b.y - a.y;
    delta.z = b.z - a.z;
    mtxa_tf_vec(&delta, &tfDelta);

    mtxa_from_mtx(tf->mtx);
    mtxa_tf_point(pos, pos);
    vel->x += tfDelta.x;
    vel->y += tfDelta.y;
    vel->z += tfDelta.z;
    mtxa_tf_vec(vel, vel);

    group = tf->destGroup;
    mtxa_from_mtx(group->prevTransform);
    mtxa_rigid_inv_tf_point(pos, &a);
    mtxa_from_mtx(group->transform);
    mtxa_rigid_inv_tf_point(pos, &b);
    delta.x = b.x - a.x;
    delta.y = b.y - a.y;
    delta.z = b.z - a.z;
    mtxa_tf_vec(&delta, &tfDelta);
    vel->x -= tfDelta.x;
    vel->y -= tfDelta.y;
    vel->z -= tfDelta.z;
}

void apply_wormhole_tf_to_quat(Quat *q, struct WormholeTf *tf)
{
    Quat tfQuat;

    mtxa_from_mtx(tf->mtx);
    mtxa_to_quat(&tfQuat);
    quat_mult(q, q, &tfQuat);
}

void fn_1_10B288(Quat *q, struct WormholeTf *tf)
{
}

void fn_1_10B28C(void)
{
    lbl_1_bss_D4290.unk0 = 0;
    lbl_1_bss_D4290.unk8 = 1;
    lbl_1_bss_D4290.unk9 = 0;
    lbl_1_bss_D4290.unkA = 0;
    memset(lbl_1_bss_D42A0, 0, 0x84);
}
