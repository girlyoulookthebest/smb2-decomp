/* Return gates. */
#include "types.h"
#include "mathutil.h"
#include "background.h"
#include "ball.h"
#include "libc.h"
#include "stage.h"
#include "stobj.h"

int spawn_stobj(struct Stobj *stobj);

void load_returngate_stobjs(struct StageCollHeader *group, int count)
{
    struct Stobj stobj;
    struct StageCollHeader *stageAg;
    int i;
    int j;
    struct StageWormhole *wormhole;

    memset(&stobj, 0, sizeof(stobj));
    stobj.type = 12;
    stageAg = group;
    for (i = 0; i < count; i++, stageAg++)
    {
        wormhole = stageAg->wormholes;
        for (j = stageAg->wormholeCount; j > 0; j--, wormhole++)
        {
            if (wormhole->unk0 & 1)
            {
                stobj.u_some_pos = wormhole->pos;
                stobj.rotX = wormhole->rotX;
                stobj.rotY = wormhole->rotY;
                stobj.rotZ = wormhole->rotZ;
                stobj.animGroupId = i;
                stobj.extraData = wormhole;
                spawn_stobj(&stobj);
            }
        }
    }
}

void stobj_returngate_init(struct Stobj *stobj)
{
    struct StageWormhole *wormhole;

    stobj->unk8 |= 0xA;
    stobj->boundSphereRadius = 2.0f;
    wormhole = stobj->extraData;
    stobj->u_some_pos = wormhole->pos;
    stobj->u_model_origin.x = 0.0f;
    stobj->u_model_origin.y = 0.0f;
    stobj->u_model_origin.z = 0.0f;
    stobj->rotX = wormhole->rotX;
    stobj->rotY = wormhole->rotY;
    stobj->rotZ = wormhole->rotZ;
    stobj->unk9C = 0.75f;
    stobj->unk90 = stobj->u_model_origin;
    stobj->unk90.y += 4.0f;
}

void stobj_returngate_tick(struct Stobj *stobj) {}

void stobj_returngate_disp(struct Stobj *stobj) {}

void stobj_returngate_coli(struct Stobj *stobj, struct PhysicsBall *ball)
{
    Vec delta;
    Vec pos;
    Vec normal;
    Vec ballPos;
    Vec force;
    float dist;
    float radius;

    mtxa_push();
    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    pos = stobj->u_some_pos;
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &normal);
    ballPos = ball->pos;
    radius = ball->radius;
    delta.x = ballPos.x - pos.x;
    delta.y = ballPos.y - pos.y;
    delta.z = ballPos.z - pos.z;
    dist = mathutil_vec_dot_prod(&normal, &delta);
    if (__fabs(dist) > radius)
    {
        force.x = 0.0f;
        force.y = 0.0f;
        force.z = 0.0f;
    }
    else
    {
        double strength = 0.006f;

        if (dist > 0.0f)
            dist = 0.006f - dist * (0.006f / radius);
        else
            dist = -0.006f - dist * (0.006f / radius);
        force.x = normal.x * dist;
        force.y = normal.y * dist;
        force.z = normal.z * dist;
    }
    ball->vel.x += force.x;
    ball->vel.y += force.y;
    ball->vel.z += force.z;
    mtxa_pop();
}

void stobj_returngate_dest(struct Stobj *stobj) {}

void stobj_returngate_cb_f(struct Stobj *stobj) {}
