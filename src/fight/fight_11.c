/* Monkey Fight 2: the CPU opponents (.text 0x2B3BC-0x2DF18).
 * Each CPU player has a struct FightCpu (lbl_12_bss_1F9D8[playerId]). Every frame fn_12_2B428 runs
 * the AI (fn_12_2B68C), which picks a target, then runs the current state's check and act functions
 * from lbl_12_data_A470 (0 approach, 1 retreat to the ring centre, 2 fight). The act functions write a
 * stick direction, a punch direction and the punch/guard buttons, which fn_12_2B428 turns into a fake
 * PADStatus in cpu->pad (read by fight.c instead of the real pad).
 * The 36 behaviour parameters (struct FightCpuParams) start random within lbl_12_data_A2C0's ranges and
 * evolve between rounds (fn_12_2BF54): the losers move towards the average of the leaders, the leaders
 * mutate. */
#include "fight.h"

/* main_loop globals (debug window settings, see window_menu.inc) */
extern float lbl_1_bss_6FA9C;  /* "KILL DECIDE SEC" */
extern float lbl_1_bss_6FAB4;  /* "BLT RANGE" */
BOOL g_some_item_collision_check(Vec *a, Vec *b, float e, Vec *c, Vec *d, float f);

/* fight.c */
float fn_12_457C(struct Stobj *stobj);
float fn_12_4658(struct Stobj *stobj);


/* Range of each CPU parameter: the initial value is random in [min, max]; flags bit 0/1 keep an
 * evolved value from crossing max/min. */
struct FightCpuParamRange
{
    float max;
    float min;
    u32 flags;
};

/* .data */
struct FightCpuParamRange lbl_12_data_A2C0[36] =
{
    { 1.0f, -1.0f, 0 },  /* 0x00 */
    { 7.27f, 3.27f, 0 },  /* 0x04 */
    { 1.75f, 0.75f, 0 },  /* 0x08 */
    { 2.8f, 1.2f, 0 },  /* 0x0C */
    { 7.0f, 3.0f, 0 },  /* 0x10 */
    { 0.0f, -2.0f, 0 },  /* 0x14 */
    { 2.8f, 1.2f, 0 },  /* 0x18 */
    { 1260.0f, 540.0f, 2 },  /* 0x1C */
    { 0.11666667f, 0.050000004f, 2 },  /* 0x20 */
    { 4.2f, 1.8000001f, 2 },  /* 0x24 */
    { 9.24f, 3.96f, 2 },  /* 0x28 */
    { 0.56f, 0.24000001f, 2 },  /* 0x2C */
    { 16.8f, 7.2000003f, 2 },  /* 0x30 */
    { 31.08f, 13.320001f, 2 },  /* 0x34 */
    { 0.35f, 0.15f, 2 },  /* 0x38 */
    { 1.26f, 0.54f, 2 },  /* 0x3C */
    { 0.7f, 0.3f, 0 },  /* 0x40 */
    { 0.093333334f, 0.040000003f, 2 },  /* 0x44 */
    { -2.8f, -1.2f, 2 },  /* 0x48 */
    { 0.022540001f, 0.009660001f, 2 },  /* 0x4C */
    { 33.6f, 14.400001f, 2 },  /* 0x50 */
    { 73.64f, 31.56f, 2 },  /* 0x54 */
    { 2.9399998f, 1.26f, 2 },  /* 0x58 */
    { 21.0f, 9.0f, 2 },  /* 0x5C */
    { 7.994f, 3.426f, 2 },  /* 0x60 */
    { 16.8f, 7.2000003f, 2 },  /* 0x64 */
    { 1.5120001f, 0.64800006f, 2 },  /* 0x68 */
    { 5.124f, 2.196f, 2 },  /* 0x6C */
    { 3.066f, 1.3140001f, 2 },  /* 0x70 */
    { 1.26f, 0.54f, 2 },  /* 0x74 */
    { 10.429999f, 4.4700003f, 2 },  /* 0x78 */
    { 168.0f, 72.0f, 2 },  /* 0x7C */
    { 2.1f, 0.90000004f, 2 },  /* 0x80 */
    { 16.8f, 7.2000003f, 2 },  /* 0x84 */
    { 2.1f, 0.90000004f, 2 },  /* 0x88 */
    { 0.35f, 0.15f, 2 },  /* 0x8C */
};

void fn_12_2C388(struct Ball *ball);
void fn_12_2C5B0(struct Ball *ball);
void fn_12_2CC54(struct Ball *ball);
void fn_12_2CDB4(struct Ball *ball);
void fn_12_2D62C(struct Ball *ball);
void fn_12_2D7EC(struct Ball *ball);

/* Per state: a check that may change cpu->state, then the state's action. */
struct FightCpuStateFuncs
{
    void (*check)(struct Ball *ball);
    void (*act)(struct Ball *ball);
};

struct FightCpuStateFuncs lbl_12_data_A470[3] =
{
    { fn_12_2C388, fn_12_2C5B0 },
    { fn_12_2CC54, fn_12_2CDB4 },
    { fn_12_2D62C, fn_12_2D7EC },
};

/* .bss */
struct FightCpu lbl_12_bss_1F9D8[4];

void fn_12_2B68C(struct Ball *ball);
void fn_12_2B7BC(struct Ball *ball);
void fn_12_2BE10(struct FightCpu *cpu);

/* Clears the CPU state and gives every CPU random parameters. */
void fn_12_2B3BC(void)
{
    int i;
    struct FightCpu *cpu;

    memset(lbl_12_bss_1F9D8, 0, sizeof(lbl_12_bss_1F9D8));
    for (cpu = lbl_12_bss_1F9D8, i = 4; i > 0; i--, cpu++)
        fn_12_2BE10(cpu);
}

/* Runs the AI of every CPU player and turns its output into a fake controller state. */
void fn_12_2B428(void)
{
    PADStatus pad;
    Vec v;
    int i;
    struct Ball *ball;
    struct FightPlayer *player;
    struct FightCpu *cpu;

    ball = balls;
    player = lbl_12_bss_1ACA0.players;
    cpu = lbl_12_bss_1F9D8;
    for (i = modeCtrl.playerCount; i > 0; i--, ball++, player++, cpu++)
    {
        if (!(player->flags & 1) || ball->status == 0)
            continue;
        memset(&pad, 0, sizeof(pad));
        cpu->pad.unk0[1] = cpu->pad.unk0[0];
        fn_12_2B68C(ball);
        pad.stickX = 60.0f * cpu->stick.x;
        pad.stickY = -60.0f * cpu->stick.z;
        pad.substickX = 0;
        pad.substickY = 0;
        if (cpu->punch)
        {
            if (cpu->aim.x != 0.0f || cpu->aim.y != 0.0f || cpu->aim.z != 0.0f)
            {
                mtxa_from_mtxb();
                mtxa_tf_vec(&cpu->aim, &v);
                pad.substickX = 60.0f * v.x;
                pad.substickY = 60.0f * v.y;
            }
            else if (lbl_12_bss_1ACA0.gloves[ball->playerId].flags & 8)
                pad.button |= PAD_BUTTON_A;
            else if (frame_counter & 4)
                pad.button |= PAD_BUTTON_A;
        }
        if (cpu->guard)
            pad.button |= PAD_BUTTON_B;
        cpu->pad.unk0[0] = pad;
        cpu->pad.unk0[2].button = cpu->pad.unk0[0].button & ~cpu->pad.unk0[1].button;
        cpu->pad.unk0[3].button = cpu->pad.unk0[1].button & ~cpu->pad.unk0[0].button;
    }
}

/* Unused. */
void fn_12_2B688(void) {}

/* One AI step for the CPU player `ball`: pick a target, then run the current state. */
void fn_12_2B68C(struct Ball *ball)
{
    struct FightCpu *cpu = &lbl_12_bss_1F9D8[ball->playerId];

    if (lbl_12_bss_1ACA0.gloves[ball->playerId].stobj == NULL)
        return;
    if (lbl_12_bss_1ACA0.flags & 0x80)
    {
        cpu->stick.x = 0.0f;
        cpu->stick.y = 0.0f;
        cpu->stick.z = 0.0f;
        cpu->punch = 0;
        cpu->guard = 0;
        return;
    }
    if (cpu->timer > 0)
        cpu->timer--;
    fn_12_2B7BC(ball);
    lbl_12_data_A470[cpu->state].check(ball);
    lbl_12_data_A470[cpu->state].act(ball);
}

/* Picks the most attractive target for `ball`'s CPU among the other players and the stage objects. */
void fn_12_2B7BC(struct Ball *ball)
{
    Vec pos;
    Vec vel;
    float score;
    float best;
    float nearest;
    float radius;
    float timeScale;
    float t;
    float dist;
    int i;
    struct FightCpu *cpu;
    struct FightPlayer *me;
    struct FightCpuParams *prm;
    int flags;
    struct FightPlayer *player;
    struct FightGlove *fd8;
    struct Ball *other;
    s8 *status;
    struct FightBox *info;
    struct Stobj *stobj;
    s8 *stobjStatus;
    int myStobjId;

    cpu = &lbl_12_bss_1F9D8[ball->playerId];
    prm = &cpu->params;
    if (lbl_12_bss_1ACA0.gloves[ball->playerId].stobj == NULL)
    {
        cpu->targetPos.x = 0.0f;
        cpu->targetPos.y = 0.0f;
        cpu->targetPos.z = 0.0f;
        cpu->targetVel.x = 0.0f;
        cpu->targetVel.y = 0.0f;
        cpu->targetVel.z = 0.0f;
        cpu->targetRadius = 0.0f;
        cpu->targetFlags = 0;
        return;
    }
    me = &lbl_12_bss_1ACA0.players[ball->playerId];
    best = -10000.0f;
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    vel.x = 0.0f;
    vel.y = 0.0f;
    vel.z = 0.0f;
    radius = 0.0f;
    nearest = 10000.0f;
    flags = 0;
    timeScale = prm->unk6C + (1.0f - prm->unk6C) * (float)mode_info.timerCurr / (float)mode_info.timerMax;

    status = g_poolInfo.playerPool.statusList;
    other = balls;
    player = lbl_12_bss_1ACA0.players;
    fd8 = lbl_12_bss_1ACA0.gloves;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, other++, player++, fd8++)
    {
        if (*status == 0 || ball == other)
            continue;
        if (other->flags & ((u64)8 << 32))
            continue;
        if (player->unk6 > 0)
            continue;
        score = prm->unk68 * (float)(1 << (4 - player->rank));
        score *= timeScale;
        if (player->wins <= me->wins)
            t = 0.0f;
        else
            t = prm->unk74 * (float)(1 << (player->wins - me->wins));
        score += t;
        dist = mathutil_vec_distance(&ball->pos, &other->pos);
        if (nearest > dist - other->currRadius)
            nearest = dist - other->currRadius;
        t = 1.0f / (1.0f + dist * (1.0f / prm->unk70));
        score *= t;
        if (player->hitBy[ball->playerId] != 0)
        {
            t = (float)player->hitBy[ball->playerId] * ((prm->unk78 - 1.0f) / (60.0f * lbl_1_bss_6FA9C));
            score *= 1.0f + t;
        }
        if (fd8->flags & 0x10)
        {
            t = 1.0f / (1.0f + dist * (1.0f / prm->unk88));
            score += prm->unk84 * t;
        }
        if (best < score)
        {
            best = score;
            pos = other->pos;
            vel = other->vel;
            radius = other->currRadius;
            flags = 0;
            if (fd8->flags & 8)
                flags |= 1;
            if (fd8->flags & 0x10)
                flags |= 2;
            if (fd8->flags & 0x20)
                flags |= 4;
        }
    }

    myStobjId = lbl_12_bss_1ACA0.gloves[ball->playerId].stobj->id;
    stobjStatus = g_poolInfo.stobjPool.statusList;
    info = lbl_12_bss_118;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, stobjStatus++, info++)
    {
        if (*stobjStatus == 0)
            continue;
        stobj = info->stobj;
        if (stobj == NULL || info->stobjCooldown[myStobjId] > 0)
            continue;
        score = prm->unk7C;
        dist = mathutil_vec_distance(&ball->pos, &stobj->position);
        if (nearest > dist - stobj->boundSphereRadius)
            nearest = dist - stobj->boundSphereRadius;
        score *= 1.0f / (1.0f + dist * (1.0f / prm->unk70));
        if (best < score)
        {
            best = score;
            pos = stobj->position;
            vel = stobj->unk64;
            radius = stobj->boundSphereRadius;
            flags = 0;
        }
    }

    cpu->targetPos = pos;
    cpu->targetVel = vel;
    cpu->targetRadius = radius;
    cpu->nearestDist = nearest;
    cpu->targetFlags = flags;
}

/* Gives `cpu` random parameters within lbl_12_data_A2C0's ranges, and random mutation steps. */
void fn_12_2BE10(struct FightCpu *cpu)
{
    struct FightCpuParams *prm = &cpu->params;
    int i;
    float *param;
    float *step;
    struct FightCpuParamRange *range;
    float size;
    float r;

    for (param = (float *)prm, step = (float *)&cpu->paramSteps, range = lbl_12_data_A2C0, i = 36;
         i > 0; i--, param++, step++, range++)
    {
        size = range->max - range->min;
        r = RAND_FLOAT();
        *step = size * (r - 0.5f) * RAND_FLOAT();
        *param += range->min + size * r;
    }
}

/* Evolves the CPU parameters at the end of a round: the CPUs of the best rank mutate, the others move
 * halfway towards the leaders' average. */
void fn_12_2BF54(void)
{
    struct FightCpuParams avg;
    struct FightCpuParams old;
    int i;
    int j;
    s8 *status;
    struct FightPlayer *player;
    struct FightCpu *cpu;
    int bestRank;
    int count;
    int cpuCount;
    float *a;
    float *b;
    float *c;
    float prev;
    float scale;
    struct FightCpuParamRange *range;
    float limit;

    bestRank = 4;
    cpuCount = 0;
    status = g_poolInfo.playerPool.statusList;
    player = lbl_12_bss_1ACA0.players;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, player++)
    {
        if (*status == 0 || !(player->flags & 1))
            continue;
        cpuCount++;
        if (bestRank > player->rank)
        {
            bestRank = player->rank;
            if (bestRank == 0)
                break;
        }
    }
    if (bestRank > 3)
        return;

    /* average of the leaders */
    count = 0;
    memset(&avg, 0, sizeof(avg));
    cpu = lbl_12_bss_1F9D8;
    status = g_poolInfo.playerPool.statusList;
    player = lbl_12_bss_1ACA0.players;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, player++, cpu++)
    {
        if (*status == 0 || !(player->flags & 1) || bestRank != player->rank)
            continue;
        count++;
        for (a = (float *)&avg, b = (float *)&cpu->params, j = 36; j > 0; j--, a++, b++)
            *a += *b;
    }
    scale = 1.0f / (float)count;
    for (a = (float *)&avg, j = 36; j > 0; j--, a++)
        *a *= scale;

    if (cpuCount == 1 && bestRank != 0)
        bestRank--;

    cpu = lbl_12_bss_1F9D8;
    status = g_poolInfo.playerPool.statusList;
    player = lbl_12_bss_1ACA0.players;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, player++, cpu++)
    {
        if (*status == 0 || !(player->flags & 1))
            continue;
        old = cpu->params;
        if (bestRank == player->rank)
        {
            /* mutate */
            for (a = (float *)&cpu->params, b = (float *)&cpu->paramSteps, j = 36; j > 0; j--, a++, b++)
            {
                *b *= 0.5f;
                *a += *b;
            }
        }
        else
        {
            /* move halfway towards the leaders */
            for (a = (float *)&avg, b = (float *)&cpu->params, c = (float *)&cpu->paramSteps, j = 36; j > 0; j--, a++, b++, c++)
            {
                prev = *b;
                *b = 0.5f * (*a + *b);
                *c = 0.5f * (*c + (prev - *b) * RAND_FLOAT());
            }
        }
        /* don't let a parameter cross its range limits */
        for (a = (float *)&cpu->params, b = (float *)&old, range = lbl_12_data_A2C0, j = 36; j > 0; j--, a++, b++, range++)
        {
            if (range->flags == 0)
                continue;
            if (range->flags & 1)
            {
                limit = range->max;
                if ((*b > limit && *a < limit) || (*b < limit && *a > limit))
                    *a = limit;
            }
            if (range->flags & 2)
            {
                limit = range->min;
                if ((*b > limit && *a < limit) || (*b < limit && *a > limit))
                    *a = limit;
            }
        }
    }
}

/* Returns whether a ball at `pos` would be over the stage (not over the fallout plane). */
static inline int fight_cpu_ground_below(Vec *pos)
{
    struct RaycastHit hit;
    int onGround = raycast_stage_down(pos, &hit, NULL);

    if (onGround && hit.pos.y < *stagedef->falloutY)
        onGround = 0;
    return onGround;
}

/* State 0 (approach) check: retreat when touching the ring edge, too far out or about to fall off. */
void fn_12_2C388(struct Ball *ball)
{
    Vec pos;
    Vec ahead;
    struct FightCpu *cpu = &lbl_12_bss_1F9D8[ball->playerId];
    struct FightCpuParams *prm = &cpu->params;
    float dot;

    if (!(ball->flags & 1))
        cpu->state = 1;
    if (mathutil_vec_len(&cpu->targetPos) - mathutil_vec_len(&ball->pos) > prm->unk04)
        cpu->state = 1;
    dot = vec_dot_normalized_safe(&ball->pos, &ball->vel);
    if (dot < 0.0f)
    {
        ahead.x = 0.0f;
        ahead.y = 0.0f;
        ahead.z = 0.0f;
    }
    else
    {
        ahead = ball->pos;
        vec_set_len(prm->unk2C * dot, &ahead, &ahead);
    }
    pos.x = ahead.x + ball->pos.x + prm->unk50 * ball->vel.x;
    pos.y = ahead.y + ball->pos.y + prm->unk50 * ball->vel.y;
    pos.z = ahead.z + ball->pos.z + prm->unk50 * ball->vel.z;
    if (!fight_cpu_ground_below(&pos))
        cpu->state = 1;
}

/* State 0 (approach): steer towards the target, keep away from items, punch when it comes in reach. */
void fn_12_2C5B0(struct Ball *ball)
{
    Vec move;
    Vec aim;
    Vec start;
    Vec end;
    Vec dir;
    Vec rel;
    Vec rv;
    struct FightCpu *cpu;
    int punch;
    struct FightCpuParams *prm;
    struct Stobj *stobj;
    struct FightGlove *fd8;
    s8 *status;
    int i;
    struct Item *item;
    float x;
    float z;
    float y;
    float reach;
    float dot;
    float len;

    cpu = &lbl_12_bss_1F9D8[ball->playerId];
    prm = &cpu->params;
    move.x = 0.0f;
    move.y = 0.0f;
    move.z = 0.0f;
    aim.x = 0.0f;
    aim.y = 0.0f;
    aim.z = 0.0f;
    punch = 0;
    aim.x = cpu->targetPos.x - ball->pos.x;
    aim.y = 0.0f;
    aim.z = cpu->targetPos.z - ball->pos.z;
    vec_normalize_len(&aim);
    fd8 = &lbl_12_bss_1ACA0.gloves[ball->playerId];
    stobj = fd8->stobj;
    reach = fn_12_4658(stobj);
    if (!(fd8->flags & 8))
    {
        start = aim;
        vec_set_len(reach, &start, &start);
        start.x += ball->pos.x;
        start.y += ball->pos.y;
        start.z += ball->pos.z;
    }
    else
    {
        start = stobj->u_local_pos;
        vec_set_len(reach, &start, &start);
        start.x += ball->pos.x + ball->vel.x * prm->unk30;
        start.y += ball->pos.y + ball->vel.y * prm->unk30;
        start.z += ball->pos.z + ball->vel.z * prm->unk30;
    }
    end.x = cpu->targetPos.x + cpu->targetVel.x * prm->unk34;
    end.y = cpu->targetPos.y + cpu->targetVel.y * prm->unk34;
    end.z = cpu->targetPos.z + cpu->targetVel.z * prm->unk34;
    if (g_some_item_collision_check(&stobj->position, &start, stobj->boundSphereRadius, &cpu->targetPos, &end, cpu->targetRadius))
        cpu->punchTimer = 60.0f * prm->unk8C;
    if ((fd8->flags & 4) && cpu->nearestDist < prm->unk08 * (stobj->boundSphereRadius + reach))
        cpu->punchTimer = 60.0f * prm->unk8C;

    rel.x = cpu->targetPos.x - ball->pos.x;
    rel.y = 0.0f;
    rel.z = cpu->targetPos.z - ball->pos.z;
    dir = rel;
    vec_normalize_len(&dir);
    rv.x = cpu->targetVel.x * prm->unk28 - ball->vel.x * prm->unk24;
    rv.y = 0.0f;
    rv.z = cpu->targetVel.z * prm->unk28 - ball->vel.z * prm->unk24;
    dot = mathutil_vec_dot_prod(&rv, &dir);
    if (dot < 0.0f)
    {
        dot *= -0.8f;
        rv.x += dot * dir.x;
        rv.z += dot * dir.z;
    }
    x = rel.x - rv.x;
    z = rel.z - rv.z;
    move.x += x;
    move.z += z;
    x = -ball->vel.x;
    z = -ball->vel.z;
    move.x += prm->unk38 * x;
    move.z += prm->unk38 * z;

    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    status = g_poolInfo.itemPool.statusList;
    item = items;
    for (i = g_poolInfo.itemPool.count; i > 0; i--, status++, item++)
    {
        if (*status == 0)
            continue;
        dir.x = item->pos.x - ball->pos.x;
        dir.y = item->pos.y - ball->pos.y;
        dir.z = item->pos.z - ball->pos.z;
        len = vec_normalize_len(&dir);
        if (len < ball->currRadius + item->unk14)
            continue;
        rel.x = ball->vel.x - item->vel.x;
        rel.y = ball->vel.y - item->vel.y;
        rel.z = ball->vel.z - item->vel.z;
        dot = mathutil_vec_dot_prod(&dir, &rel);
        if (dot <= 0.0f)
            continue;
        len /= dot * prm->unk64;
        len += 1.0f;
        len = 1.0f / len;
        x += len * dir.x;
        y += len * dir.y;
        z += len * dir.z;
    }
    move.x += prm->unk60 * x;
    move.y += prm->unk60 * y;
    move.z += prm->unk60 * z;

    if (cpu->punchTimer > 0)
    {
        cpu->punchTimer--;
        punch = 1;
    }
    if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f)
        vec_normalize_len(&move);
    cpu->stick.x += 0.2f * (move.x - cpu->stick.x);
    cpu->stick.y += 0.2f * (move.y - cpu->stick.y);
    cpu->stick.z += 0.2f * (move.z - cpu->stick.z);
    cpu->aim = aim;
    cpu->punch = punch;
    cpu->guard = 0;
}

/* State 1 (retreat) check: back to approaching once far enough inside the ring, or fighting when the
 * target is close. */
void fn_12_2CC54(struct Ball *ball)
{
    struct FightCpu *cpu = &lbl_12_bss_1F9D8[ball->playerId];
    struct FightCpuParams *prm = &cpu->params;
    float dist;
    float reach;

    cpu->punchTimer = 0;
    if (mathutil_vec_len(&cpu->targetPos) - mathutil_vec_len(&ball->pos) > prm->unk00)
    {
        cpu->state = 0;
        if (cpu->timer <= 0)
        {
            dist = mathutil_vec_distance(&cpu->targetPos, &ball->pos);
            reach = prm->unk0C * fn_12_4658(lbl_12_bss_1ACA0.gloves[ball->playerId].stobj);
            if (cpu->nearestDist > reach && dist < reach + prm->unk10)
                cpu->state = 2;
        }
    }
}

/* State 1 (retreat): head for the ring centre, away from the edge and from fast players and items. */
void fn_12_2CDB4(struct Ball *ball)
{
    struct RaycastHit hit;
    Vec t;
    Vec move;
    Vec aim;
    Vec pos;
    Vec ahead;
    Vec away;
    Vec side;
    Vec rel;
    Vec dir;
    int i;
    struct FightCpu *cpu;
    struct Ball *other;
    struct Item *item;
    s8 *status;
    int punch;
    struct FightCpuParams *prm;
    int onGround;
    float speed;
    float dot;
    float len;
    float dist;
    float d;
    float fwd;

    cpu = &lbl_12_bss_1F9D8[ball->playerId];
    prm = &cpu->params;
    move.x = 0.0f;
    move.y = 0.0f;
    move.z = 0.0f;
    aim.x = 0.0f;
    aim.y = 0.0f;
    aim.z = 0.0f;
    punch = 0;
    t.x = -ball->vel.x;
    t.y = 0.0f;
    t.z = -ball->vel.z;
    move.x += prm->unk3C * t.x;
    move.z += prm->unk3C * t.z;
    t.x = -ball->pos.x;
    t.y = 0.0f;
    t.z = -ball->pos.z;
    move.x += prm->unk4C * t.x;
    move.z += prm->unk4C * t.z;
    fwd = vec_dot_normalized_safe(&ball->pos, &ball->vel);
    if (fwd < 0.0f)
    {
        ahead.x = 0.0f;
        ahead.y = 0.0f;
        ahead.z = 0.0f;
    }
    else
    {
        ahead = ball->pos;
        vec_set_len(prm->unk58 * fwd, &ahead, &ahead);
    }
    pos.x = ahead.x + ball->pos.x + prm->unk50 * ball->vel.x;
    pos.y = ahead.y + ball->pos.y + prm->unk50 * ball->vel.y;
    pos.z = ahead.z + ball->pos.z + prm->unk50 * ball->vel.z;
    onGround = raycast_stage_down(&pos, &hit, NULL);
    if (onGround && hit.pos.y < *stagedef->falloutY)
        onGround = 0;
    if (!onGround)
    {
        t.x = -ball->pos.x;
        t.y = 0.0f;
        t.z = -ball->pos.z;
        vec_set_len(0.01f + mathutil_vec_len(&ball->vel), &t, &t);
        move.x += prm->unk54 * t.x;
        move.z += prm->unk54 * t.z;
        pos.x = ahead.x + ball->pos.x + prm->unk5C * ball->vel.x;
        pos.y = ahead.y + ball->pos.y + prm->unk5C * ball->vel.y;
        pos.z = ahead.z + ball->pos.z + prm->unk5C * ball->vel.z;
        onGround = raycast_stage_down(&pos, &hit, NULL);
        if (onGround && hit.pos.y < *stagedef->falloutY)
            onGround = 0;
        if (!onGround && (ball->flags & 1))
            punch = 1;
    }

    t.x = 0.0f;
    t.y = 0.0f;
    t.z = 0.0f;
    status = g_poolInfo.playerPool.statusList;
    other = balls;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, other++)
    {
        if (*status == 0 || ball == other)
            continue;
        speed = mathutil_vec_len(&other->vel);
        if (speed < 0.01f)
            continue;
        away.x = ball->pos.x - other->pos.x;
        away.y = ball->pos.y - other->pos.y;
        away.z = ball->pos.z - other->pos.z;
        dist = vec_normalize_len(&away);
        if (mathutil_vec_dot_prod(&away, &other->vel) < 0.0f)
            continue;
        speed /= prm->unk44 * dist;
        side = ball->vel;
        if (side.x == 0.0f && side.y == 0.0f && side.z == 0.0f)
        {
            side.x = -away.z;
            side.y = away.y;
            side.z = away.x;
        }
        vec_normalize_len(&side);
        d = prm->unk48 * mathutil_vec_dot_prod(&away, &side);
        side.x += d * away.x;
        side.y = 0.0f;
        side.z += d * away.z;
        vec_set_len(speed, &side, &side);
        t.x += side.x;
        t.z += side.z;
    }
    move.x += prm->unk40 * t.x;
    move.y += prm->unk40 * t.y;
    move.z += prm->unk40 * t.z;

    t.x = 0.0f;
    t.y = 0.0f;
    t.z = 0.0f;
    status = g_poolInfo.itemPool.statusList;
    item = items;
    for (i = g_poolInfo.itemPool.count; i > 0; i--, status++, item++)
    {
        if (*status == 0)
            continue;
        dir.x = item->pos.x - ball->pos.x;
        dir.y = item->pos.y - ball->pos.y;
        dir.z = item->pos.z - ball->pos.z;
        len = vec_normalize_len(&dir);
        if (len < ball->currRadius + item->unk14)
            continue;
        rel.x = ball->vel.x - item->vel.x;
        rel.y = ball->vel.y - item->vel.y;
        rel.z = ball->vel.z - item->vel.z;
        dot = mathutil_vec_dot_prod(&dir, &rel);
        if (dot <= 0.0f)
            continue;
        len /= dot * prm->unk64;
        len += 1.0f;
        len = 1.0f / len;
        t.x += len * dir.x;
        t.y += len * dir.y;
        t.z += len * dir.z;
    }
    move.x += prm->unk60 * t.x;
    move.y += prm->unk60 * t.y;
    move.z += prm->unk60 * t.z;

    if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f)
        vec_normalize_len(&move);
    cpu->stick.x += 0.2f * (move.x - cpu->stick.x);
    cpu->stick.y += 0.2f * (move.y - cpu->stick.y);
    cpu->stick.z += 0.2f * (move.z - cpu->stick.z);
    cpu->aim = aim;
    cpu->punch = punch;
    cpu->guard = 0;
}

/* State 2 (fight) check: retreat when not punching or about to fall off. */
void fn_12_2D62C(struct Ball *ball)
{
    Vec pos;
    Vec ahead;
    struct FightCpu *cpu = &lbl_12_bss_1F9D8[ball->playerId];
    struct FightCpuParams *prm = &cpu->params;
    struct FightGlove *fd8 = &lbl_12_bss_1ACA0.gloves[ball->playerId];
    float dot;

    if (!(fd8->flags & 0x30))
        cpu->state = 1;
    if (fd8->chargeLevel > 0)
    {
        dot = vec_dot_normalized_safe(&ball->pos, &ball->vel);
        if (dot < 0.0f)
        {
            ahead.x = 0.0f;
            ahead.y = 0.0f;
            ahead.z = 0.0f;
        }
        else
        {
            ahead = ball->pos;
            vec_set_len(prm->unk2C * dot, &ahead, &ahead);
        }
        pos.x = ahead.x + ball->pos.x + prm->unk50 * ball->vel.x;
        pos.y = ahead.y + ball->pos.y + prm->unk50 * ball->vel.y;
        pos.z = ahead.z + ball->pos.z + prm->unk50 * ball->vel.z;
        if (!fight_cpu_ground_below(&pos))
            cpu->state = 1;
    }
}

/* State 2 (fight): guard, keep the punching distance and decide when to throw the punch. */
void fn_12_2D7EC(struct Ball *ball)
{
    Vec move;
    Vec aim;
    Vec rel;
    Vec dir;
    Vec rv;
    struct FightCpu *cpu;
    struct FightCpuParams *prm;
    struct FightGlove *fd8;
    int punch;
    int guard;
    int doPunch;
    int i;
    struct Ball *other;
    struct FightPlayer *player;
    float dist;
    float reach;
    float chance;
    float timer;
    float x;
    float z;
    float dot;
    float unused;
    int maxScore;

    move.x = 0.0f;
    move.y = 0.0f;
    move.z = 0.0f;
    aim.x = 0.0f;
    aim.y = 0.0f;
    aim.z = 0.0f;
    punch = 0;
    guard = 1;
    cpu = &lbl_12_bss_1F9D8[ball->playerId];
    prm = &cpu->params;
    fd8 = &lbl_12_bss_1ACA0.gloves[ball->playerId];
    reach = ball->currRadius + (fn_12_4658(fd8->stobj) + fn_12_457C(fd8->stobj));
    rel.x = cpu->targetPos.x - ball->pos.x;
    rel.y = 0.0f;
    rel.z = cpu->targetPos.z - ball->pos.z;
    dist = mathutil_vec_len(&rel);
    if ((fd8->flags & 0x10) && fd8->chargeLevel > 0)
    {
        rel.x = cpu->targetPos.x - ball->pos.x;
        rel.y = 0.0f;
        rel.z = cpu->targetPos.z - ball->pos.z;
        doPunch = 0;
        if (dist < lbl_1_bss_6FAB4 * prm->unk0C)
            doPunch = 1;
        else if (dist < reach)
        {
            if ((cpu->targetFlags & 6) && RAND_FLOAT() < prm->unk20)
                doPunch = 1;
            else
            {
                chance = prm->unk14 + (float)fd8->chargeLevel * (1.0f - prm->unk14) / 4.0f;
                chance *= 0.041666668f;  /* 1/24 */
                if ((float)fd8->chargeTimer > 126.0f || RAND_FLOAT() < chance)
                    doPunch = 1;
            }
        }
        if (doPunch)
        {
            punch = 1;
            aim = rel;
            vec_normalize_len(&aim);
        }
    }
    if (fd8->flags & 0x20)
    {
        punch = 1;
        aim.x = cpu->targetPos.x - ball->pos.x;
        aim.y = 0.0f;
        aim.z = cpu->targetPos.z - ball->pos.z;
        vec_normalize_len(&aim);
    }
    if (dist > reach * prm->unk18)
    {
        rel.x = cpu->targetPos.x - ball->pos.x;
        rel.y = 0.0f;
        rel.z = cpu->targetPos.z - ball->pos.z;
        dir = rel;
        vec_normalize_len(&dir);
        rv.x = cpu->targetVel.x * prm->unk28 - ball->vel.x * prm->unk24;
        rv.y = 0.0f;
        rv.z = cpu->targetVel.z * prm->unk28 - ball->vel.z * prm->unk24;
        dot = mathutil_vec_dot_prod(&rv, &dir);
        if (dot < 0.0f)
        {
            dot *= -0.8f;
            rv.x += dot * dir.x;
            rv.z += dot * dir.z;
        }
        x = rel.x - rv.x;
        z = rel.z - rv.z;
        move.x += x;
        move.z += z;
    }
    rel.x = cpu->targetPos.x - ball->pos.x;
    rel.y = 0.0f;
    rel.z = cpu->targetPos.z - ball->pos.z;
    dir = rel;
    vec_normalize_len(&dir);
    dot = -0.5f * (mathutil_vec_dot_prod(&fd8->stobj->u_local_pos, &dir) - 1.0f) - 0.1f;
    if (dot > 0.0f)
    {
        x = rel.x * dot;
        z = rel.z * dot;
        move.x += x;
        move.z += z;
    }
    if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f)
        vec_normalize_len(&move);
    cpu->stick.x += 0.2f * (move.x - cpu->stick.x);
    cpu->stick.y += 0.2f * (move.y - cpu->stick.y);
    cpu->stick.z += 0.2f * (move.z - cpu->stick.z);
    cpu->aim = aim;
    cpu->punch = punch;
    cpu->guard = guard;

    timer = prm->unk1C * (0.5f + 0.5f * RAND_FLOAT());
    maxScore = 0;
    other = balls;
    player = lbl_12_bss_1ACA0.players;
    for (i = modeCtrl.playerCount; i > 0; i--, other++, player++)
    {
        if (ball != other && other->status != 0 && maxScore < player->wins)
            maxScore = player->wins;
    }
    if (lbl_12_bss_1ACA0.players[ball->playerId].wins < maxScore)
    {
        /* The original computes the score gap ratio but then scales by the leader's score instead
         * (the ratio is dead; its int-to-float conversions are still emitted). */
        unused = (float)(maxScore - lbl_12_bss_1ACA0.players[ball->playerId].wins) / (float)lbl_12_bss_1ACA0.winsNeeded;
        timer *= 1.0f - 0.5f * (float)maxScore;
    }
    cpu->timer = timer;
}
