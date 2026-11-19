/* Monkey Fight 2: fighter ball modes (start, respawn, knock-out, ring-out, punch knock-back),
   the ball trail, ball-ball collisions, the fighter ape animation callback and the camera mode
   dispatch. */
#include "fight.h"

/* fight_3.c */
void fn_12_1674C(struct Ball *ball);
/* fight_4.c (w7) */
void fn_12_178CC(struct FightStageHit *hit);
/* fight_7.c (w9) */
void fn_12_205E8(struct FightStartSet *sets, int count);
extern void (*lbl_12_data_9DD0[])(struct Camera *camera, struct Ball *ball);

/* DOL / main_loop */
void g_reset_ball(struct Ball *ball);
void g_maybe_sets_number_of_starting_monkeys(struct Ball *ball);
void g_move_and_collide(struct Ball *ball, struct PhysicsBall *physBall);
void handle_ball_stage_coli(struct Ball *ball, struct PhysicsBall *physBall);
void fn_1_4AF48(struct Ball *ball, struct PhysicsBall *physBall);
void position_ball(struct Ball *ball, struct PhysicsBall *physBall);
void apply_physicsball_to_ball(struct Ball *ball, struct PhysicsBall *physBall);
void fn_1_48BCC(struct Ball *ball);
void ball_physics_g_something_w_postgoal_blast_up(struct Ball *ball);
void g_something_with_item_coli(Vec *a, Vec *b, float d, Vec *c, float e);
void call_SoundReqID_arg_0(int soundId);
int call_SoundReqID_arg_1_copy(u32 soundId);
void SoundPitch(u16 soundId, u16 pitch);
extern float lbl_1_bss_6FA98;  /* window.c debug value */
extern float lbl_1_bss_6FAE0;  /* window.c debug value */
extern s32 sub_mode;
void set_ape_anim(struct Ape *ape, int anim);
void g_some_set_ape_anim(struct Ape *ape, int a, int b);
void fn_1_1132D0(struct Ape *ape, int b, int c, int d);
float fn_1_1135E8(struct Ape *ape);
float fn_1_1136E0(struct Ape *ape);
void ape_default_anim(struct Ape *ape);

/* An ape animation: fn_1_1132D0(ape, unk0, unk2, unk4). */
struct FightApeAnim
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ s32 unk4;
};

void fn_12_1D970(struct Ball *ball);
void fn_12_1E2D8(struct Ball *ball);
void fn_12_1E704(struct Ball *ball);
struct FightApeAnim *fn_12_1F34C(struct Ape *ape);
struct FightApeAnim *fn_12_1F398(struct Ape *ape);
void fn_12_1FFEC(int playerId, Vec *out);
void fn_12_20120(struct Ball *ball, struct PhysicsBall *physBall, struct FightPlayer *player);

/* .data */
u8 lbl_12_data_9D28[4] = { 0, 1, 2, 3 };  /* ball colour per player */

struct FightApeAnim lbl_12_data_9D2C[6] = {
    { 0, 7, 0 },
    { 0, 7, 2 },
    { 0, 7, 4 },
    { 0, 7, 6 },
    { 0, 7, 8 },
    { 1, 0, 0 },
};

struct FightApeAnim lbl_12_data_9D5C[3] = {
    { 0, 8, 0 },
    { 0, 8, 1 },
    { 1, 1, 0 },
};


/* .bss */
struct FightApeState lbl_12_bss_1EE70[4];

/* Records the ball position into the player's trail and ages the trail points. */
void fn_12_1D534(struct Ball *ball)
{
    struct FightPlayer *player;
    struct FightTrail *trail;
    struct FightTrailPt *pt;
    Vec dv;
    int started;
    float speed;
    float accel;
    int idx;
    int n;

    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    trail = &player->trail;
    pt = &trail->pts[trail->head];
    dv.x = ball->vel.x - trail->prevVel.x;
    dv.y = ball->vel.y - trail->prevVel.y;
    dv.z = ball->vel.z - trail->prevVel.z;
    pt->pos = ball->pos;
    started = 0;
    speed = mathutil_vec_len(&ball->vel);
    accel = mathutil_vec_len(&dv);
    if (accel > 0.1f && speed > 0.4f)
    {
        if (!(ball->flags & 0x10) && !(player->flags & 0x10))
        {
            player->flags |= 0x10;
            trail->slowTimer = 0;
            started = 1;
        }
    }
    else if (player->flags & 0x10)
    {
        if (ball->flags & 1)
        {
            trail->slowTimer++;
            if (trail->slowTimer > 30 || speed < 0.2f)
                player->flags &= ~0x10;
        }
        else
            trail->slowTimer = 0;
        if (ball->flags & 0x10)
            player->flags &= ~0x10;
    }
    if (player->flags & 0x10)
    {
        pt->flags |= 1;
        if (started)
            pt->timer = 15;
        else
        {
            float t;

            if (10.0f * accel > 2.5f * speed)
                t = 10.0f * accel;
            else
                t = 2.5f * speed;
            if (t < 0.5f)
                t = 0.5f;
            else if (t > 1.0f)
                t = 1.0f;
            pt->timer = (s16)(int)(15.0f * t);
        }
    }
    else
    {
        pt->flags &= ~1;
        pt->timer = 0;
    }
    if (player->stunTimer == 0)
    {
        if (trail->count < 60)
            trail->count++;
        if (trail->head < 59)
            trail->head++;
        else
            trail->head = 0;
    }
    n = trail->count;
    idx = trail->head;
    if (idx > 0)
        pt = &trail->pts[idx - 1];
    else
        pt = &trail->pts[idx + 59];
    for (; n > 15; idx--, n--, pt--)
    {
        if (idx <= 0)
            pt = &trail->pts[idx + 59];
        if (pt->timer == 0)
            break;
    }
    for (; n > 0; idx--, n--, pt--)
    {
        if (idx <= 0)
            pt = &trail->pts[idx + 59];
        if (pt->timer != 0)
        {
            pt->timer--;
            if (pt->timer == 0)
                pt->flags &= ~1;
        }
    }
    trail->prevVel = ball->vel;
}

/* Ball mode 36: round start, waiting on the start position. */
void fn_12_1D970(struct Ball *ball)
{
    g_maybe_sets_number_of_starting_monkeys(ball);
    ball->colorId = lbl_12_data_9D28[ball->playerId];
    ball->unk2 = 3;
    if (lbl_12_bss_1ACA0.players[ball->playerId].flags & 1)
        ball->flags |= 0x20000000000ULL;
    ball->flags |= 0x10;
    ball->ape->flags |= 0x20;
    lbl_12_bss_1EE70[ball->playerId].state = 0;
    ball->state = 24;
    ball->unk154 = 0;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;
}

/* Ball mode 37: drops the ball onto its start position. */
void fn_12_1DB30(struct Ball *ball)
{
    Vec start;
    float t;

    g_reset_ball(ball);
    ball->colorId = lbl_12_data_9D28[ball->playerId];
    if (lbl_12_bss_1ACA0.players[ball->playerId].flags & 1)
        ball->flags |= 0x20000000000ULL;
    t = 24.0f;
    fn_12_1FFEC(ball->playerId, &start);
    ball->pos.x = start.x;
    ball->pos.y = start.y + ((ball->accel * t) * t) * 0.5;
    ball->pos.z = start.z;
    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    {
        float dy;

        if (stagedef != NULL)
            dy = stagedef->startPos->pos.y - ball->pos.y;
        else
            dy = 0.0f;
        ball->vel.x = 0.0f;
        ball->vel.y = dy / t + (ball->accel * t) * 0.5;
        ball->vel.z = 0.0f;
    }
    ball->rotX = 0x2000;
    if (stagedef != NULL)
        ball->rotY = stagedef->startPos->yrot - 0x4000;
    else
        ball->rotY = 0;
    ball->rotZ = 0;
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
    ball->flags &= ~0x10;
    ball->flags |= 0x4000;
    ball->flags |= 0x100000000ULL;
    if (ball->ape != NULL)
        ball->ape->flags &= ~(1 << 5);
    lbl_12_bss_1EE70[ball->playerId].state = 0;
    ball->state = 24;
    ball->unk154 = 44;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.1f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkA0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    mtxa_to_quat(&ball->unkB0);
}

/* Ball mode 38: respawn after a ring-out (moves the ball back over its start position). */
void fn_12_1DE74(struct Ball *ball)
{
    struct Effect effect;
    Vec start;
    Vec oldPos;
    struct FightPlayer *player;

    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    oldPos = ball->pos;
    g_reset_ball(ball);
    ball->colorId = lbl_12_data_9D28[ball->playerId];
    if (player->flags & 1)
        ball->flags |= 0x20000000000ULL;
    fn_12_1FFEC(ball->playerId, &start);
    ball->pos.x = start.x;
    ball->pos.y = 1.0f + start.y;
    ball->pos.z = start.z;
    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;
    ball->rotX = 0;
    if (stagedef != NULL)
        ball->rotY = stagedef->startPos->yrot - 0x4000;
    else
        ball->rotY = 0;
    ball->rotZ = 0;
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
    ball->flags |= 0x500;
    ball->flags |= 0x1800000000ULL;
    ball->flags |= 0x10;
    if (ball->ape != NULL)
        ball->ape->flags |= 0x20;
    lbl_12_bss_1EE70[ball->playerId].state = 0;
    ball->unk80 = mathutil_vec_distance(&oldPos, &ball->pos);
    if ((float)ball->unk80 < 15.0f)
        ball->unk80 = 15;
    else if ((float)ball->unk80 > 45.0f)
        ball->unk80 = 45;
    ball->flags |= 0x2000000000ULL;
    memset(&effect, 0, sizeof(effect));
    effect.type = 41;
    effect.playerId = ball->playerId;
    effect.u_otherTimer = ball->unk80;
    effect.pos = oldPos;
    spawn_effect(&effect);
    fn_12_1674C(ball);
    player->flags &= ~0x10;
    ball->state = 24;
    ball->unk154 = 39;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkA0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    mtxa_from_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    if (ball->ape != NULL)
        mtxa_to_quat(&ball->ape->rot);
    mtxa_to_quat(&ball->unkB0);
    fn_12_1E2D8(ball);
}

/* Ball mode 39: respawn fly-in; ends in mode 40 (play) with a sound. */
void fn_12_1E2D8(struct Ball *ball)
{
    struct PhysicsBall physBall;

    g_move_and_collide(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80--;
    if (ball->unk80 <= 0)
    {
        ball->state = 24;
        ball->unk154 = 40;
        ball->unk80 = 0;
        ball->flags &= ~0x10;
        ball->flags |= 0x4000;
        if (ball->ape != NULL)
            ball->ape->flags &= ~(1 << 5);
        call_SoundReqID_arg_0(0x131);
    }
}

/* Ball mode 40: bobbing in place. */
void fn_12_1E3B8(struct Ball *ball)
{
    struct PhysicsBall physBall;

    ball->vel.x = 0.0f;
    ball->vel.y = 0.04f * math_sin(ball->unk80 << 9);
    ball->vel.z = 0.0f;
    g_move_and_collide(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
}

/* Ball mode 41: released into play (hop, sound, then mode 44). */
void fn_12_1E454(struct Ball *ball)
{
    struct FightPlayer *player;

    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    ball->vel.y += 0.15f;
    ball->flags &= ~0x10;
    ball->flags |= 0x4000;
    if (ball->ape != NULL)
        ball->ape->flags &= ~(1 << 5);
    if (player->unk1C != 0)
    {
        g_smth_with_sound = ball->ape->charaId;
        call_SoundReqID_arg_0(30);
    }
    player->unk1C = 15;
    ball->flags |= 0x4000;
    ball->flags &= ~0x500;
    ball->flags |= 0x100000000ULL;
    ball->flags &= ~0x2000000000ULL;
    ball->flags &= ~0x1800000000ULL;
    player->unk6 = (s16)(int)(60.0f * lbl_1_bss_6FA98);
    player->flags |= 8;
    call_SoundReqID_arg_0(0x129);
    ball->state = 24;
    ball->unk154 = 44;
    ball->unk80 = 0;
    fn_1_48BCC(ball);
}

/* Ball mode 42: knocked out (voice, then mode 43). */
void fn_12_1E64C(struct Ball *ball)
{
    ball->unk130 = 0;
    g_smth_with_sound = ball->ape->charaId;
    g_player_id_for_sound = ball->playerId;
    call_SoundReqID_arg_0((rand() & 1) ? 0x51 : 0x1D);
    call_SoundReqID_arg_0(0x15);
    ball->flags |= 0x1800000000ULL;
    ball->state = 24;
    ball->unk154 = 43;
    fn_12_1E704(ball);
}

/* Ball mode 43: knocked out, falling. */
void fn_12_1E704(struct Ball *ball)
{
    struct PhysicsBall physBall;

    g_move_and_collide(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
    ball->unk130++;
}

/* Ball mode 44: normal play: movement, ground damping, stage hits. */
void fn_12_1E768(struct Ball *ball)
{
    Vec g;
    Vec n;
    Vec v;
    struct FightStageHit hit;
    struct PhysicsBall physBall;
    struct FightPlayer *player;

    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    fn_12_20120(ball, &physBall, player);
    handle_ball_stage_coli(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
    if (player->stunTimer == 0)
    {
        if (!(ball->flags & 1))
        {
            ball->vel.x *= 0.99f;
            ball->vel.y *= 0.99f;
            ball->vel.z *= 0.99f;
        }
        else if (player->unk14 < 42)
        {
            float d;

            g = world_infos[ball->playerId].unk10;
            n.x = ball->unk11C.x;
            n.y = ball->unk11C.y;
            n.z = ball->unk11C.z;
            d = -mathutil_vec_dot_prod(&g, &n);
            g.x += d * n.x;
            g.y += d * n.y;
            g.z += d * n.z;
            if (0.0f != g.x || 0.0f != g.y || 0.0f != g.z)
            {
                float f;
                float s;

                vec_normalize_len(&g);
                if (player->unk14 <= 0)
                    f = 1.0f;
                else
                    f = 1.0f - 0.023809524f * player->unk14;
                f *= lbl_12_data_9CC4[player->chara].unk4;
                v = ball->vel;
                d = mathutil_vec_dot_prod(&g, &v);
                v.x -= d * g.x;
                v.y -= d * g.y;
                v.z -= d * g.z;
                s = 1.0f - 0.05f * (f * d);
                v.x *= s;
                v.y *= s;
                v.z *= s;
                v.x += d * g.x;
                v.y += d * g.y;
                v.z += d * g.z;
                if (d < 0.0f)
                {
                    s = -0.1f * (f * d);
                    v.x += s * g.x;
                    v.y += s * g.y;
                    v.z += s * g.z;
                }
                ball->vel = v;
            }
        }
    }
    if (ball->flags & 1)
    {
        hit.type = 0;
        hit.playerId = ball->playerId;
        hit.itemGroup = physBall.hardestColiItemGroupId;
        hit.power = physBall.hardestColiSpeed;
        hit.pos = physBall.pos;
        mtxa_from_mtx(itemgroups[physBall.hardestColiItemGroupId].transform);
        mtxa_tf_vec(&physBall.hardestColiNormal, &hit.dir);
        fn_12_178CC(&hit);
    }
    if ((player->flags & 8) && (ball->flags & 1))
    {
        world_infos[ball->playerId].state = 1;
        player->flags &= ~8;
    }
    if (player->unk6 > 0)
        player->unk6--;
    if (player->unk1C > 0)
    {
        player->unk1C--;
        if (player->unk1C == 0)
        {
            g_smth_with_sound = ball->ape->charaId;
            call_SoundReqID_arg_0(30);
        }
    }
}

/* Ball mode 45: ring-out: respawns over the start position and flies back in (mode 46). */
void fn_12_1ECA0(struct Ball *ball)
{
    struct Effect effect;
    struct PhysicsBall physBall;
    Vec start;
    Vec oldPos;

    oldPos = ball->pos;
    g_reset_ball(ball);
    ball->colorId = lbl_12_data_9D28[ball->playerId];
    if (lbl_12_bss_1ACA0.players[ball->playerId].flags & 1)
        ball->flags |= 0x20000000000ULL;
    fn_12_1FFEC(ball->playerId, &start);
    ball->pos.x = oldPos.x;
    ball->pos.y = 10.0f + oldPos.y;
    ball->pos.z = oldPos.z;
    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;
    ball->rotX = 0x2000;
    if (stagedef != NULL)
        ball->rotY = stagedef->startPos->yrot - 0x4000;
    else
        ball->rotY = 0;
    ball->rotZ = 0;
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
    ball->flags |= 0x500;
    ball->flags |= 0x1800000000ULL;
    ball->flags |= 0x10;
    if (ball->ape != NULL)
        ball->ape->flags |= 0x20;
    ball->unk80 = 3.0f * mathutil_vec_distance(&oldPos, &ball->pos);
    memset(&effect, 0, sizeof(effect));
    effect.type = 41;
    effect.playerId = ball->playerId;
    effect.u_otherTimer = ball->unk80;
    effect.pos = oldPos;
    spawn_effect(&effect);
    fn_12_1674C(ball);
    ball->state = 24;
    ball->unk154 = 46;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.1f };
    ball->ape->flags &= ~(1 << 14);
    ball->unkA0 = (Quat){ 0.0f, 0.0f, 0.0f, 0.05f };
    mtxa_to_quat(&ball->unkB0);
    g_move_and_collide(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80--;
    if (ball->unk80 <= 0)
    {
        ball->state = 24;
        ball->unk154 = 40;
        ball->unk80 = 0;
        ball->flags &= ~0x10;
        ball->flags |= 0x4000;
        if (ball->ape != NULL)
            ball->ape->flags &= ~(1 << 5);
        call_SoundReqID_arg_0(0x131);
    }
}

/* Ball mode 46: flying back in. */
void fn_12_1F0C4(struct Ball *ball)
{
    struct PhysicsBall physBall;

    g_move_and_collide(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
}

/* Fighter ape animation callback (g_create_stage_player_wrapper). */
void fn_12_1F110(struct Ape *ape)
{
    struct Ball *ball;
    struct FightPlayer *player;
    struct FightApeState *st;
    struct FightApeAnim *anim;

    ball = &balls[ape->ballIdx];
    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    st = &lbl_12_bss_1EE70[ball->playerId];
    ape->flags &= ~0x13;
    switch (st->state)
    {
    case 0:
        st->state = 1;
        ape->flags |= 0x8000000;
        set_ape_anim(ape, 1);
        break;
    case 1:
        break;
    case 2:
        st->state = 3;
        st->anim = fn_12_1F34C(ape);
        set_ape_anim(ape, 0);
        /* fall through */
    case 3:
        if (ape->flags & 0x2000)
        {
            set_ape_anim(ape, 0);
            st->anim = fn_12_1F34C(ape);
        }
        anim = st->anim;
        fn_1_1132D0(ape, anim->unk0, anim->unk2, anim->unk4);
        if ((float)modeCtrl.submodeTimer < 270.0f)
        {
            float t = fn_1_1135E8(ape);

            if (fn_1_1136E0(ape) - t < 2.0f)
                st->state = 6;
        }
        break;
    case 4:
        st->state = 5;
        st->anim = fn_12_1F398(ape);
        set_ape_anim(ape, 1);
        /* fall through */
    case 5:
        if (ape->flags & 0x2000)
            st->anim = fn_12_1F398(ape);
        anim = st->anim;
        fn_1_1132D0(ape, anim->unk0, anim->unk2, anim->unk4);
        break;
    case 6:
        st->state = 7;
        g_some_set_ape_anim(ape, 3, player->wins - 1);
        break;
    }
    ape_default_anim(ape);
}

/* Random idle animation. */
struct FightApeAnim *fn_12_1F34C(struct Ape *ape)
{
    return &lbl_12_data_9D2C[(rand() & 0x7FFF) % 6u];
}

/* Random win animation. */
struct FightApeAnim *fn_12_1F398(struct Ape *ape)
{
    return &lbl_12_data_9D5C[(rand() & 0x7FFF) % 3u];
}

/* Ball mode 47: round over (the main_loop post-goal mode with the fighter animation). */
void fn_12_1F3E4(struct Ball *ball)
{
    ball->state = 6;
    ball->flags |= 0x500;
    if (ball->flags & 0x3000)
        ball->flags |= 0x40;
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){ 0.0f, 0.0f, 0.0f };
    ball->unkB0 = (Quat){ 0.0f, 0.0f, 0.0f, 1.0f };
    ball->unkA0 = ball->unkB0;
    if (ball->flags & 0x8000)
        lbl_12_bss_1EE70[ball->playerId].state = 4;
    else
        lbl_12_bss_1EE70[ball->playerId].state = 2;
    ball_physics_g_something_w_postgoal_blast_up(ball);
}

/* Tilts the player's world (gravity) from the stick. */
void fn_12_1F5A8(struct Ball *ball)
{
    struct FightCpu *cpu;
    struct World *world;
    Vec down;

    cpu = &lbl_12_bss_1F9D8[ball->playerId];
    world = &world_infos[ball->playerId];
    if (world->state == 2)
    {
        float stickX = cpu->pad.unk0[0].stickX / 60.0f;
        float stickY = -(float)cpu->pad.unk0[0].stickY / 60.0f;
        float inpXRot = stickY * DEGREES_TO_S16(world->maxTilt);
        float inpZRot = -stickX * DEGREES_TO_S16(world->maxTilt);

        world->xrot = inpXRot;
        world->zrot = inpZRot;
        /* dead in the original; only its literals stay in .rodata */
        {
            float u1 = 20.0f;
            float u2 = 12.0f;
        }
        down.x = 0.0f;
        down.y = -1.0f;
        down.z = 0.0f;
        mtxa_from_identity();
        mtxa_rotate_x(world->xrot);
        mtxa_rotate_z(world->zrot);
        mtxa_rigid_inv_tf_vec(&down, &world->unk10);
    }
}

/* Ball-ball hit sounds by impact strength. */
s32 lbl_12_data_9D90[3] = { 0x17, 0x18, 0x1A };

/* Start positions around the ring when the stage has none (players 0, 1, 3, 2). */
Vec lbl_12_data_9D9C[4] = {
    { 1.5f, 0.0f, 0.0f },
    { -1.5f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.5f },
    { 0.0f, 0.0f, -1.5f },
};

/* Dot product of (x1, y1, z1) and (x2, y2, z2) (as in main_loop/camera.c). */
static inline float mathutil_dot_xyz(register float x1, register float y1, register float z1, register float x2, register float y2, register float z2)
{
#ifdef __MWERKS__
    asm
    {
        fmuls x1, x1, x2
        fmadds x1, y1, y2, x1
        fmadds x1, z1, z2, x1
    }
    return x1;
#else
    return x1 * x2 + y1 * y2 + z1 * z2;
#endif
}

/* Ball-ball collisions between the gloves. */
void fn_12_1F744(void)
{
    struct PhysicsBall a;
    struct PhysicsBall b;
    Vec d;
    Vec v;
    Vec n1;
    Vec n2;
    int i;
    int j;
    struct Ball *ballA;
    s8 *statusA;
    struct Ball *ballB;
    s8 *statusB;
    struct FightPlayer *playerA;
    struct FightPlayer *playerB;
    float r;
    float massB;
    float massA;
    float f;
    int snd;

    playerA = lbl_12_bss_1ACA0.players;
    ballA = balls;
    statusA = g_poolInfo.playerPool.statusList;
    for (i = g_poolInfo.playerPool.count; i > 1; i--, ballA++, statusA++, playerA++)
    {
        if (*statusA == 0 || (ballA->flags & 0x800000000ULL))
            continue;
        init_physicsball_from_ball(ballA, &a);
        if (ballA->flags & 1)
            a.flags |= 1;
        massA = ballA->modelScale * (ballA->modelScale * ballA->modelScale);
        playerB = playerA + 1;
        ballB = ballA + 1;
        statusB = statusA + 1;
        for (j = i - 1; j > 0; j--, ballB++, statusB++, playerB++)
        {
            if (*statusB != 2 || (ballB->flags & 0x800000000ULL))
                continue;
            init_physicsball_from_ball(ballB, &b);
            if (ballB->flags & 1)
                b.flags |= 1;
            d.x = b.prevPos.x - a.prevPos.x;
            d.y = b.prevPos.y - a.prevPos.y;
            d.z = b.prevPos.z - a.prevPos.z;
            v.x = a.vel.x - b.vel.x;
            v.y = a.vel.y - b.vel.y;
            v.z = a.vel.z - b.vel.z;
            if (mathutil_vec_dot_prod(&d, &v) > 0.0f
             && fn_1_A74E4(&a.prevPos, &a.pos, a.radius, &b.prevPos, &b.pos, b.radius))
            {
                d.x = a.pos.x - b.pos.x;
                d.y = a.pos.y - b.pos.y;
                d.z = a.pos.z - b.pos.z;
                vec_set_len(lbl_1_bss_6FAE0, &d, &d);
                massB = ballB->modelScale * (ballB->modelScale * ballB->modelScale);
                f = fabs(mathutil_dot_xyz(d.x, d.y, d.z, a.vel.x - b.vel.x, a.vel.y - b.vel.y, a.vel.z - b.vel.z));
                if (f > 0.05f)
                {
                    if (playerA->unk1A == 0 || playerB->unk1A == 0)
                    {
                        int id;
                        float pitch;

                        g_player_id_for_sound = ballA->playerId;
                        id = call_SoundReqID_arg_1_copy(0x114);
                        pitch = 163830.0f * (f - 0.05f);
                        if (pitch > 16383.0f)
                            pitch = 16383.0f;
                        g_player_id_for_sound = ballA->playerId;
                        SoundPitch(id, (int)pitch & 0x3FFF);
                    }
                    snd = f / 0.2f;
                    if (snd > 2)
                        snd = 2;
                    snd = lbl_12_data_9D90[snd];
                    if (playerA->unk1A == 0)
                    {
                        ballA->flags |= 0x20;
                        g_player_id_for_sound = ballA->playerId;
                        g_smth_with_sound = ballA->ape->charaId;
                        call_SoundReqID_arg_0(snd);
                        playerA->unk1A = 15;
                    }
                    if (playerB->unk1A == 0)
                    {
                        ballB->flags |= 0x20;
                        g_player_id_for_sound = ballB->playerId;
                        g_smth_with_sound = ballB->ape->charaId;
                        call_SoundReqID_arg_0(snd);
                        playerB->unk1A = 15;
                    }
                }
                g_something_with_item_coli(&d, &a.vel, massA, &b.vel, massB);
                if (a.flags & 1)
                {
                    v = a.vel;
                    n1.x = -ballA->unk11C.x;
                    n1.y = -ballA->unk11C.y;
                    n1.z = -ballA->unk11C.z;
                    f = mathutil_vec_dot_prod(&n1, &v);
                    if (f < 0.0f)
                    {
                        v.x -= f * n1.x;
                        v.y -= f * n1.y;
                        v.z -= f * n1.z;
                        a.vel = v;
                        f *= -massA / massB;
                        b.vel.x += f * n1.x;
                        b.vel.y += f * n1.y;
                        b.vel.z += f * n1.z;
                    }
                }
                if (b.flags & 1)
                {
                    v = b.vel;
                    n2.x = -ballB->unk11C.x;
                    n2.y = -ballB->unk11C.y;
                    n2.z = -ballB->unk11C.z;
                    f = mathutil_vec_dot_prod(&n2, &v);
                    if (f < 0.0f)
                    {
                        v.x -= f * n2.x;
                        v.y -= f * n2.y;
                        v.z -= f * n2.z;
                        b.vel = v;
                        f *= -massB / massA;
                        a.vel.x += f * n2.x;
                        a.vel.y += f * n2.y;
                        a.vel.z += f * n2.z;
                    }
                }
                a.pos.x += a.vel.x;
                a.pos.y += a.vel.y;
                a.pos.z += a.vel.z;
                b.pos.x += b.vel.x;
                b.pos.y += b.vel.y;
                b.pos.z += b.vel.z;
                r = a.radius + b.radius;
                d.x = b.pos.x - a.pos.x;
                d.y = b.pos.y - a.pos.y;
                d.z = b.pos.z - a.pos.z;
                if (vec_normalize_len(&d) < r)
                {
                    if (a.flags & 1)
                    {
                        b.pos.x = a.pos.x + d.x * r;
                        b.pos.y = a.pos.y + d.y * r;
                        b.pos.z = a.pos.z + d.z * r;
                    }
                    else
                    {
                        a.pos.x = b.pos.x - d.x * r;
                        a.pos.y = b.pos.y - d.y * r;
                        a.pos.z = b.pos.z - d.z * r;
                    }
                }
            }
            apply_physicsball_to_ball(ballB, &b);
        }
        apply_physicsball_to_ball(ballA, &a);
    }
}

/* Start position of a player (players 2 and 3 swapped). */
void fn_12_1FFEC(int playerId, Vec *out)
{
    if (playerId == 2)
        playerId = 3;
    else if (playerId == 3)
        playerId = 2;
    if (stagedef == NULL)
    {
        *out = lbl_12_data_9D9C[playerId];
        return;
    }
    if (*(s32 *)stagedef->unk6C > 1)
    {
        *out = stagedef->startPos[playerId].pos;
        return;
    }
    *out = lbl_12_data_9D9C[playerId];
    out->x += stagedef->startPos->pos.x;
    out->y += stagedef->startPos->pos.y;
    out->z += stagedef->startPos->pos.z;
}

/* Ball physics step of a fighter: gravity along the tilted world, extra push while rolling uphill. */
void fn_12_20120(struct Ball *ball, struct PhysicsBall *physBall, struct FightPlayer *player)
{
    Vec up;
    Vec accel;
    Vec vel;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20;
    mtxa_from_identity();
    mtxa_rotate_x(world_infos[ball->playerId].xrot);
    mtxa_rotate_z(world_infos[ball->playerId].zrot);
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    mtxa_tf_vec(&up, &up);
    accel.x = 0.0f;
    accel.y = -ball->accel;
    accel.z = 0.0f;
    if (ball->flags & 0x200)
        accel.y = -accel.y;
    else if (ball->flags & 0x100)
        accel.y = 0.0f;
    if (!(ball->flags & 0x100000000ULL) && sub_mode != 14 && (ball->prevFlags & 1))
    {
        float sq;

        vel = ball->vel;
        sq = mathutil_sum_of_sq_2(vel.x, vel.z);
        if (sq > 1.1920929e-07f)
        {
            float s = 1.0 / math_sqrt(sq);
            float t;

            vel.x *= s;
            vel.z *= s;
            t = vel.x * -math_sin(cameras[ball->playerId].rotY) + vel.z * -math_sin(cameras[ball->playerId].rotY + 0x4000);
            if (t < 0.0)
            {
                s = 1.0 + -0.65 * -t;
                accel.x *= s;
                accel.y *= s;
                accel.z *= s;
            }
        }
    }
    mtxa_from_identity();
    mtxa_rotate_x(world_infos[ball->playerId].xrot);
    mtxa_rotate_z(world_infos[ball->playerId].zrot);
    mtxa_rigid_inv_tf_vec(&accel, &accel);
    ball->vel.x += accel.x;
    ball->vel.y += accel.y;
    ball->vel.z += accel.z;
    if (player->stunTimer == 0)
    {
        ball->pos.x += ball->vel.x;
        ball->pos.y += ball->vel.y;
        ball->pos.z += ball->vel.z;
    }
}

/* Sets every camera to the fighter camera mode (59) with no sub-mode, clears the start sets. */
void fn_12_20518(void)
{
    struct Camera *camera;
    struct Camera *oldCamera;
    int i;

    oldCamera = g_current_camera;
    camera = cameras;
    for (i = 0; i < 5; i++, camera++)
    {
        g_current_camera = camera;
        camera->mode = 59;
        camera->unk1F = -1;
    }
    g_current_camera = oldCamera;
    fn_12_205E8(NULL, 0);
}

/* Fighter camera mode: runs the sub-mode in camera->unk1F. */
void fn_12_20598(struct Camera *camera, struct Ball *ball)
{
    if (camera->unk1F >= 0)
        lbl_12_data_9DD0[camera->unk1F](camera, ball);
}
