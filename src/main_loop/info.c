/* Per-stage play state: timer, goals, fall-outs, bonus and ranking sprites. */
#include "types.h"
#include "mode.h"
#include "stage.h"
#include "libc.h"
#include "futsal_data.h"
#include "ball.h"
#include "input.h"
#include "sprite.h"
#include "camera.h"
#include "item.h"
#include "pool.h"
#include "recplay.h"
#include "dip.h"

struct ModeInfo mode_info;
s32 practice_best_score;
s32 is_practice_score_new_best;
s8 rankBonusPoints[8];  /* bonus points per finishing place */
u32 lbl_1_bss_70DC[4];
struct ModeInfo playerInfos[4];  /* per-player copies of mode_info */

extern struct StageSelectState stageselect_is_storymode;
extern s32 g_smth_with_sound;      /* character of the ball the next voice line is for */
extern s32 g_player_id_for_sound;
void call_SoundReqID_arg_0(int soundId);

/* Stage select state; the symbol name only describes its first byte. */
struct StageSelectState
{
    /*0x00*/ s8 isStoryMode;
    u8 unk1[0x17 - 0x1];
    /*0x17*/ u8 flags;
};

BOOL is_stage_id_not_for_party_game(int stageId);
void teleport_through_wormhole(int playerId, int wormholeId);
void fn_1_6094C(int a, int b);
void fn_1_6160C(int playerId);  /* replay: goal */
void fn_1_616C0(int playerId);  /* replay: fall-out */
void fn_1_61734(int playerId);  /* replay: time over */
void g_something_with_goals_and_physicsball(int goalId, struct PhysicsBall *physBall);
void fn_1_FC300(void);
void g_handle_goal(void);
int get_world_unbeaten_stage_count(int world);
void fn_1_16C720(void);
void g_handle_world_bgm(int a);
void g_some_set_ape_anim(struct Ape *ape, int a, int b);
void g_ball_ape_rotation(struct Ball *ball);

int get_current_cm_stage_time_limit(void);
int get_current_storymode_stage_time_limit(void);
int get_storymode_stage_time_limit_wrapper(int world, int stage);
BOOL is_bonus_stage(int stageId);
BOOL g_are_on_final_course_level(int difficulty, int floor, u32 courseFlags);

static void g_reset_mode_info_stuff(void);
BOOL did_ball_enter_goal(struct Ball *ball, u32 *outGoalId, s32 *outItemGroupId, s8 *outGoalType);
int u_find_goal_of_type(struct Ball *ball, s8 *goalType);
BOOL did_ball_enter_wormhole(struct Ball *ball, u32 *outWormholeId);
void g_set_goaled(void);
void set_ball_rank_bonus(struct Ball *ball);
void create_ranking_sprite(struct Ball *ball);
void g_something_with_goals_and_ball(struct Ball *ball, int goalId, int itemGroupId, s8 goalType);
BOOL did_ball_fallout(struct Ball *ball);
void g_something_playing_soundeffects(struct Ball *ball);

void g_reset_cm_course(void)
{
    mode_info.unk8 = 0;
    mode_info.attempts = 1;
    mode_info.currFloor = 1;
    mode_info.livesLost = 0;
    mode_info.continuesUsed = 0;
    if (modeCtrl.gameType == 2)  /* practice */
    {
        practice_best_score = 0;
        is_practice_score_new_best = 0;
    }
}

void event_info_init(void)
{
    g_reset_mode_info_stuff();

    mode_info.timerCurr = 60 * 60;
    if (modeCtrl.courseFlags & 1)
        mode_info.timerCurr = get_current_cm_stage_time_limit();
    if (modeCtrl.courseFlags & (1 << 22))
        mode_info.timerCurr = get_current_storymode_stage_time_limit();
    if (modeCtrl.gameType == 2 && stageselect_is_storymode.isStoryMode == 1)
        mode_info.timerCurr = get_storymode_stage_time_limit_wrapper(mode_info.storyWorld, mode_info.storyStage);
    if (sub_mode == 0xD)
        mode_info.timerCurr = 90 * 60;
    if (sub_mode == 0x44)
        mode_info.timerCurr = 60 * 60;
    mode_info.timerMax = mode_info.timerCurr;

    if (is_bonus_stage(current_stage_id))
        mode_info.flags |= INFO_FLAG_BONUS_STAGE;
    if (g_are_on_final_course_level(modeCtrl.difficulty, mode_info.currFloor, modeCtrl.courseFlags))
        mode_info.flags |= INFO_FLAG_FINAL_FLOOR;
}

/* Story mode and single-player bookkeeping when a goal is reached. */
static inline void handle_story_goal(void)
{
    if (main_mode == 2)
    {
        if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1)
            g_handle_goal();
        else if (modeCtrl.gameType == 14)
        {
            g_handle_goal();
            if (get_world_unbeaten_stage_count(mode_info.storyWorld) + 1 == 10)
                fn_1_16C720();
        }
    }
}

void event_info_tick(void)
{
    struct Ball *ball;
    s8 *status;
    int i;
    struct Ball *ballBackup;
    int goaledBalls;
    struct ReplayHeader replayHeader;
    struct PhysicsBall physBall;

    if (g_some_other_flags & 0xA)
        return;

    is_stage_id_not_for_party_game(current_stage_id);

    /* wormholes */
    ballBackup = current_ball;
    ball = balls;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++)
    {
        u32 wormholeId;

        if (*status == 0 || *status == 4)
            continue;
        current_ball = ball;
        if (did_ball_enter_wormhole(ball, &wormholeId))
            teleport_through_wormhole(ball->playerId, wormholeId);
    }
    current_ball = ballBackup;

    /* goals */
    goaledBalls = 0;
    ballBackup = current_ball;
    ball = balls;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++)
    {
        u32 goalId;
        s32 itemGroupId;
        s8 goalType;

        if (*status == 0 || *status == 4)
            continue;
        current_ball = ball;
        if (!did_ball_enter_goal(ball, &goalId, &itemGroupId, &goalType))
            continue;
        mode_info.playerId = ball->playerId;
        switch (modeCtrl.gameType)
        {
        case 1:  /* competition */
            if (ball->flags & BALL_FLAG_REPLAY)
            {
                fn_1_62AA0(lbl_1_bss_71EC8.unk0[ball->playerId], &replayHeader);
                if (!(replayHeader.flags & 1))
                    continue;
            }
            if (ball->flags & BALL_FLAG_REPLAY)
            {
                mode_info.flags &= ~(INFO_FLAG_REPLAY | INFO_FLAG_11);
                ball->flags |= BALL_FLAG_GOAL;
                ball->state = 5;
                ball->unk15C = ball->pos;
            }
            if (!(ball->flags & BALL_FLAG_GOAL) && !(mode_info.flags & INFO_FLAG_05))
            {
                fn_1_6094C(1, 0x5A);
                mode_info.flags &= ~(INFO_FLAG_REPLAY | INFO_FLAG_11);
                ball->flags |= BALL_FLAG_GOAL;
                ball->state = 5;
                ball->unk15C = ball->pos;
                cameras[i].mode = 14;
                if (++goaledBalls == 1)
                    mode_info.unk2C++;
                if (!(mode_info.flags & INFO_FLAG_REPLAY) && !(mode_info.flags & INFO_FLAG_BONUS_STAGE))
                {
                    ball->rank = mode_info.unk2C;
                    if (ball->rank == 1)  /* first place */
                    {
                        ball->winStreak++;
                        ball->unk134 = 0;
                    }
                    else if (ball->rank == modeCtrl.playerCount)  /* last place */
                    {
                        ball->winStreak = 0;
                        ball->unk134++;
                    }
                    else
                    {
                        ball->winStreak = 0;
                        ball->unk134 = 0;
                    }
                    set_ball_rank_bonus(ball);
                    create_ranking_sprite(ball);
                }
                if (mode_info.unk2C == 1)
                    g_something_with_goals_and_ball(ball, goalId, itemGroupId, goalType);
                fn_1_6160C(ball->playerId);
            }
            init_physicsball_from_ball(ball, &physBall);
            if (itemGroupId != physBall.itemGroupId)
                tf_physball_to_itemgroup_space(&physBall, itemGroupId);
            g_something_with_goals_and_physicsball(goalId, &physBall);
            ball->goalTime = mode_info.timerCurr;
            g_something_playing_soundeffects(ball);
            break;
        case 4:
            break;
        default:
            if (ball->flags & BALL_FLAG_11)
                break;
            if (!(mode_info.flags & INFO_FLAG_REPLAY) && (ball->flags & BALL_FLAG_GOAL))
                break;
            if (ball->flags & BALL_FLAG_REPLAY)
            {
                fn_1_62AA0(lbl_1_bss_71EC8.unk0[ball->playerId], &replayHeader);
                if (!(replayHeader.flags & 1))
                    break;
            }
            mode_info.flags &= ~(INFO_FLAG_REPLAY | INFO_FLAG_11);
            ball->flags |= BALL_FLAG_GOAL;
            ball->state = 5;
            if (!(mode_info.flags & INFO_FLAG_05))
            {
                g_set_goaled();
                g_something_with_goals_and_ball(ball, goalId, itemGroupId, goalType);
                if (modeCtrl.gameType == 0)
                    fn_1_FC300();
                handle_story_goal();
            }
            fn_1_6160C(ball->playerId);
            if (sub_mode == 0x12)
                mode_info.flags |= INFO_FLAG_GOAL;
            init_physicsball_from_ball(ball, &physBall);
            if (itemGroupId != physBall.itemGroupId)
                tf_physball_to_itemgroup_space(&physBall, itemGroupId);
            g_something_with_goals_and_physicsball(goalId, &physBall);
            ball->goalTime = mode_info.timerCurr;
            g_something_playing_soundeffects(ball);
            break;
        }
    }
    if (goaledBalls > 1)
        mode_info.unk2C += goaledBalls - 1;
    if (goaledBalls > 0 && mode_info.unk2C >= modeCtrl.playerCount - 1)
        g_set_goaled();
    current_ball = ballBackup;

    /* bananas left */
    mode_info.bananasLeft = 0;
    {
        struct Item *item = items;
        s8 *itemStatus = g_poolInfo.itemPool.statusList;

        for (i = 0; i < g_poolInfo.itemPool.count; i++, item++, itemStatus++)
        {
            if (*itemStatus != 0 && *itemStatus != 3 && item->type == 0 && (item->flags & (1 << 1)))
                mode_info.bananasLeft++;
        }
    }

    /* debug: X+Y finishes the stage (D-pad picks the goal colour) */
    if (!(mode_info.flags & INFO_FLAG_05)
     && !(mode_info.flags & INFO_FLAG_TIMER_PAUSED)
     && ((dip_switches & DIP_DEBUG) || (dip_switches & DIP_TEST_TEAM))
     && stagedef->goals != NULL
     && (merged_digital_inputs[0] & PAD_BUTTON_X) && (merged_digital_inputs[0] & PAD_BUTTON_Y))
    {
        if (modeCtrl.gameType == 0)
            mode_info.playerId = modeCtrl.currPlayer;
        else
            mode_info.playerId = 0;
        handle_story_goal();
        mode_info.goalEntered = u_find_goal_of_type(&balls[modeCtrl.currPlayer], &mode_info.goalType);
        mode_info.goalItemGroup = 0;
        mode_info.unk10 = balls[modeCtrl.currPlayer].vel;
        mode_info.unk1C = mode_info.timerCurr;
        if (merged_digital_inputs[0] & PAD_BUTTON_LEFT)
            mode_info.unk22 = 10;

        BALL_FOREACH(
            if (!(ball->flags & BALL_FLAG_GOAL))
            {
                ball->flags |= BALL_FLAG_GOAL;
                ball->state = 5;
                ball->unk15C = ball->pos;
            }
        )

        g_set_goaled();
        {
            struct PhysicsBall debugPhysBall;

            init_physicsball_from_ball(&balls[modeCtrl.currPlayer], &debugPhysBall);
            g_something_with_goals_and_physicsball(mode_info.goalEntered, &debugPhysBall);
            balls[modeCtrl.currPlayer].goalTime = mode_info.timerCurr;
        }
    }

    /* fall-outs */
    if (!(mode_info.flags & INFO_FLAG_05))
    {
        ball = balls;
        status = g_poolInfo.playerPool.statusList;
        for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++)
        {
            if (*status == 0 || *status == 4)
                continue;
            if (ball->flags & BALL_FLAG_11)
                continue;
            if (!did_ball_fallout(ball))
                continue;
            switch (modeCtrl.gameType)
            {
            case 1:
                ball->state = 19;
                ball->flags |= BALL_FLAG_11;
                ball->unk15C = ball->pos;
                break;
            case 4:
                ball->flags |= BALL_FLAG_11;
                break;
            default:
                mode_info.flags |= INFO_FLAG_FALLOUT | INFO_FLAG_TIMER_PAUSED;
                ball->flags |= BALL_FLAG_11;
                fn_1_616C0(ball->playerId);
                break;
            }
        }
    }

    /* clock and time over */
    if (!(mode_info.flags & INFO_FLAG_TIMER_PAUSED) && !(dip_switches & DIP_TIME_STOP))
    {
        if (mode_info.unk8 < 0xFFFFFFFF)
            mode_info.unk8++;
        if (!(mode_info.flags & INFO_FLAG_11))
            mode_info.timerCurr--;

        if (mode_info.timerCurr <= 0)
        {
            switch (modeCtrl.gameType)
            {
            case 1:
                if (mode_info.unk2C > 0)
                {
                    g_set_goaled();
                    mode_info.flags |= INFO_FLAG_13;
                    break;
                }
                mode_info.flags |= INFO_FLAG_TIMER_PAUSED | INFO_FLAG_TIMEOVER;
                fn_1_61734(ball->playerId);
                if (!(mode_info.flags & INFO_FLAG_BONUS_STAGE))
                    BALL_FOREACH( ball->winStreak = 0; ball->unk134++; )
                BALL_FOREACH( ball->flags |= BALL_FLAG_TIMEOVER; )
                break;
            case 4:
                mode_info.flags |= INFO_FLAG_TIMER_PAUSED | INFO_FLAG_TIMEOVER;
                break;
            default:
                if (modeCtrl.gameType == 2 && (stageselect_is_storymode.flags & 2))
                {
                    mode_info.flags |= INFO_FLAG_TIMER_PAUSED;
                    g_handle_world_bgm(100);
                    break;
                }
                ball = current_ball;
                mode_info.flags |= INFO_FLAG_TIMER_PAUSED | INFO_FLAG_TIMEOVER;
                fn_1_61734(ball->playerId);
                ball->flags |= BALL_FLAG_TIMEOVER;
                if (mode_info.flags & INFO_FLAG_BONUS_STAGE)
                    g_some_set_ape_anim(ball->ape, 2, 4);
                else
                    g_some_set_ape_anim(ball->ape, 2, ball->unk2 & 1);
                break;
            }
        }
    }

    BALL_FOREACH( g_ball_ape_rotation(ball); )
}

void event_info_dest(void)
{
    g_reset_mode_info_stuff();
}

/* Clear mode_info, keeping the fields that last across stages. */
static void g_reset_mode_info_stuff(void)
{
    u32 unk8 = mode_info.unk8;
    s16 attempts = mode_info.attempts;
    s16 currFloor = mode_info.currFloor;
    s16 livesLost = mode_info.livesLost;
    s16 continuesUsed = mode_info.continuesUsed;
    s16 currStageId = mode_info.currStageId;
    s16 storyWorld = mode_info.storyWorld;
    s16 storyStage = mode_info.storyStage;

    memset(&mode_info, 0, sizeof(mode_info));
    mode_info.unk8 = unk8;
    mode_info.attempts = attempts;
    mode_info.currFloor = currFloor;
    mode_info.livesLost = livesLost;
    mode_info.continuesUsed = continuesUsed;
    mode_info.currStageId = currStageId;
    mode_info.storyWorld = storyWorld;
    mode_info.storyStage = storyStage;
    mode_info.unk22 = 1;
    if (modeCtrl.gameType == 2)
        is_practice_score_new_best = 0;
}

/* Did the ball cross a goal trigger this frame? Returns the goal's index (counting through
 * all item groups), its item group and its type. */
BOOL did_ball_enter_goal(struct Ball *ball, u32 *outGoalId, s32 *outItemGroupId, s8 *outGoalType)
{
    struct ItemGroup *itemGroup;
    struct StageCollHeader *coll;
    int goalId;
    int itemGroupId;
    struct PhysicsBall physBall;

    init_physicsball_from_ball(ball, &physBall);
    goalId = 0;
    itemGroup = itemgroups;
    coll = stagedef->collHeaders;
    for (itemGroupId = 0; itemGroupId < stagedef->itemGroupCount; itemGroupId++, itemGroup++, coll++)
    {
        if (coll->goalCount > 0)
        {
            struct StageGoal *goal;
            int i;

            if (itemGroupId != physBall.itemGroupId)
                tf_physball_to_itemgroup_space(&physBall, itemGroupId);
            goal = coll->goals;
            for (i = 0; i < coll->goalCount; i++, goal++)
            {
                struct ColiRect goalTrigger;

                mtxa_from_translate(&goal->pos);
                mtxa_rotate_z(goal->rotZ);
                mtxa_rotate_y(goal->rotY);
                mtxa_rotate_x(goal->rotX);
                goalTrigger.pos.x = 0.0f;
                goalTrigger.pos.y = 1.5f;
                goalTrigger.pos.z = 0.0f;
                mtxa_tf_point(&goalTrigger.pos, &goalTrigger.pos);
                goalTrigger.rot.x = goal->rotX;
                goalTrigger.rot.y = goal->rotY;
                goalTrigger.rot.z = goal->rotZ;
                goalTrigger.width = 3.0f;
                goalTrigger.height = 3.0f;
                if (line_intersects_rect(&physBall.pos, &physBall.prevPos, &goalTrigger))
                {
                    *outGoalId = goalId;
                    *outItemGroupId = itemGroupId;
                    *outGoalType = goal->type;
                    return TRUE;
                }
                goalId++;
            }
        }
    }
    return FALSE;
}

/* Debug: choose a goal type with the D-pad (up: 2, right: 1, down: 0) and return the
 * index of the first goal of that type. */
int u_find_goal_of_type(struct Ball *ball, s8 *goalType)
{
    int goalId;
    int found;
    struct ItemGroup *itemGroup;
    struct StageCollHeader *coll;
    int itemGroupId;

    goalId = 0;
    found = 0;
    itemGroup = itemgroups;
    coll = stagedef->collHeaders;
    for (itemGroupId = 0; itemGroupId < stagedef->itemGroupCount; itemGroupId++, itemGroup++, coll++)
    {
        if (coll->goalCount > 0)
        {
            struct StageGoal *goal = coll->goals;
            int i;

            for (i = 0; i < coll->goalCount; i++, goal++)
            {
                if (goal->type == 0)
                {
                    found = 1;
                    break;
                }
                goalId++;
            }
            if (found)
                break;
        }
    }

    if ((merged_digital_inputs[0] & PAD_BUTTON_UP)
     || (merged_digital_inputs[0] & PAD_BUTTON_RIGHT)
     || (merged_digital_inputs[0] & PAD_BUTTON_DOWN))
    {
        if (merged_digital_inputs[0] & PAD_BUTTON_UP)
            *goalType = 2;
        if (merged_digital_inputs[0] & PAD_BUTTON_RIGHT)
            *goalType = 1;
        if (merged_digital_inputs[0] & PAD_BUTTON_DOWN)
            *goalType = 0;

        goalId = 0;
        found = 0;
        itemGroup = itemgroups;
        coll = stagedef->collHeaders;
        for (itemGroupId = 0; itemGroupId < stagedef->itemGroupCount; itemGroupId++, itemGroup++, coll++)
        {
            if (coll->goalCount > 0)
            {
                struct StageGoal *goal = coll->goals;
                int i;

                for (i = 0; i < coll->goalCount; i++, goal++)
                {
                    if (goal->type == *goalType)
                    {
                        found = 1;
                        break;
                    }
                    goalId++;
                }
                if (found)
                    break;
            }
        }
    }
    return goalId;
}

/* Did the ball pass through a wormhole this frame? Returns the wormhole's index. */
BOOL did_ball_enter_wormhole(struct Ball *ball, u32 *outWormholeId)
{
    struct PhysicsBall physBall;
    struct ItemGroup *itemGroup;
    struct StageCollHeader *coll;
    int wormholeId;
    int itemGroupId;

    init_physicsball_from_ball(ball, &physBall);
    wormholeId = 0;
    itemGroup = itemgroups;
    coll = stagedef->collHeaders;
    for (itemGroupId = 0; itemGroupId < stagedef->itemGroupCount; itemGroupId++, itemGroup++, coll++)
    {
        if (coll->wormholeCount > 0)
        {
            int i;
            struct StageWormhole *wormhole;

            if (itemGroupId != physBall.itemGroupId)
                tf_physball_to_itemgroup_space(&physBall, itemGroupId);
            wormhole = coll->wormholes;
            for (i = 0; i < coll->wormholeCount; i++, wormhole++)
            {
                struct ColiRect trigger;

                mtxa_from_translate(&wormhole->pos);
                mtxa_rotate_z(wormhole->rotZ);
                mtxa_rotate_y(wormhole->rotY);
                mtxa_rotate_x(wormhole->rotX);
                trigger.pos.x = 0.0f;
                trigger.pos.y = 2.0f;
                trigger.pos.z = 0.0f;
                mtxa_tf_point(&trigger.pos, &trigger.pos);
                trigger.rot.x = wormhole->rotX;
                trigger.rot.y = wormhole->rotY;
                trigger.rot.z = wormhole->rotZ;
                trigger.width = 4.0f;
                trigger.height = 4.0f;
                if (line_intersects_rect(&physBall.pos, &physBall.prevPos, &trigger))
                {
                    *outWormholeId = wormholeId;
                    return TRUE;
                }
                wormholeId++;
            }
        }
    }
    return FALSE;
}

/* Stand-in for a function the linker stripped; only its constants remain. */
void info_stripped_literals_1(float *f, double *d)
{
    f[0] = 3.0969208e-08f;
    d[0] = 0.0;
}

/* Everyone's done: stop the clock, and in competition mode time out the balls still
 * playing. */
void g_set_goaled(void)
{
    mode_info.flags |= INFO_FLAG_TIMER_PAUSED | INFO_FLAG_05 | INFO_FLAG_GOAL;
    switch (modeCtrl.gameType)
    {
    case 1:  /* competition */
        {
            struct Ball *ballBackup = current_ball;
            struct Ball *ball = balls;
            int i;

            for (i = 0; i < 8; i++, ball++)
            {
                if (ball->status == 2)
                {
                    current_ball = ball;
                    if (!(ball->flags & BALL_FLAG_GOAL))
                    {
                        ball->flags |= BALL_FLAG_08 | BALL_FLAG_10;
                        ball->winStreak = 0;
                        ball->unk134++;
                        ball->flags |= BALL_FLAG_TIMEOVER;
                    }
                }
            }
            current_ball = ballBackup;
        }
        break;
    }
}

/* Competition mode: the bonus for finishing in the top three, multiplied by the win
 * streak if that rule is on. */
void set_ball_rank_bonus(struct Ball *ball)
{
    int bonus;

    if (ball->rank < 1 || ball->rank > 3)
        return;

    bonus = rankBonusPoints[ball->rank];
    if ((modeCtrl.courseFlags & (1 << 11)) && ball->winStreak > 0)
        bonus *= ball->winStreak;
    ball->bonus = bonus;
}

struct RankTexOffset
{
    float u1;
    float v1;
    float u2;
    float v2;
};

static struct RankTexOffset rankTexOffsets[4] =
{
    {   0.0f,  0.0f, 160.0f, 48.0f },
    {   0.0f,  0.0f, 160.0f, 48.0f },
    {   0.0f, 48.0f, 128.0f, 48.0f },
    { 160.0f,  0.0f,  88.0f, 48.0f },
};

void sprite_bonus_plus000_tick(s8 *dummy, struct Sprite *sprite)
{
    struct Ball *ball = &balls[sprite->userVar];

    sprite->opacity += (1.0f - sprite->opacity) * 0.1;
    if (sprite->bmpId >= 100)
        sprintf(sprite->text, "BONUS  +%3d", ball->bonus);
    else if (sprite->bmpId >= 10)
        sprintf(sprite->text, "BONUS  +%2d", ball->bonus);
    else
        sprintf(sprite->text, "BONUS  +%1d", ball->bonus);
}

void sprite_bonus_banana_tick(s8 *dummy, struct Sprite *sprite)
{
    sprite->opacity += (1.0f - sprite->opacity) * 0.1;
}

/* Stand-in for a stripped sprite tick (SMB1's win_streak_sprite_main); only its
 * constants remain. */
void info_stripped_literals_2(float *f, int i)
{
    f[0] = 32767.0f;
    f[1] = 1.035f;
    f[2] = 89128.96f;
    f[3] = 255.0f;
    f[4] = 0.0f;
    f[5] = i;
}

/* A camera's viewport, as fractions of the screen. */
struct Viewport
{
    float left;
    float top;
    float width;
    float height;
};

/* The rank icon fades and shrinks in, drifts up, then (if a bonus was earned) spawns the
 * bonus count with a banana icon. */
void sprite_ranking_tick(s8 *dummy, struct Sprite *sprite)
{
    sprite->counter++;
    if (sprite->counter <= 15)
    {
        sprite->scaleX = 1.0 + 0.2f * (15.0f - sprite->counter);
        sprite->scaleY = sprite->scaleX;
        sprite->opacity = sprite->counter * 0.066666;
    }
    if (sprite->counter > 60 && sprite->counter < 0x69)
        sprite->y -= 1.0f;
    if (sprite->userVar != 0 && sprite->counter == 0x78)
    {
        struct Sprite *countSprite;
        struct Ball *ball = &balls[sprite->bmpId];
        struct Viewport *vp = (struct Viewport *)&cameras[ball->playerId].sub28.viewportX;

        countSprite = create_sprite();
        if (countSprite != NULL)
        {
            countSprite->x = (vp->left + vp->width * 0.5) * 640.0;
            countSprite->y = (vp->top + vp->height * 0.5) * 480.0;
            countSprite->fontId = 142;
            countSprite->textAlign = ALIGN_CC;
            countSprite->userVar = ball->playerId;
            countSprite->mulR = 255;
            countSprite->mulG = 255;
            countSprite->mulB = 0;
            countSprite->opacity = 0.0f;
            countSprite->bmpId = ball->bonus;  /* used as the digit count by the tick */
            countSprite->mainFunc = sprite_bonus_plus000_tick;
            sprintf(countSprite->text, "BONUS  +000", ball->bonus);
            countSprite = create_linked_sprite(countSprite);
            if (countSprite != NULL)
            {
                countSprite->type = SPRITE_TYPE_BITMAP;
                countSprite->x = -140.0f;
                if (ball->bonus < 10)
                    countSprite->x += 48.0f;
                else if (ball->bonus < 100)
                    countSprite->x += 24.0f;
                countSprite->y = -2.0f;
                countSprite->bmpId = 12;
                countSprite->textAlign = ALIGN_CC;
                countSprite->scaleX = 0.3f;
                countSprite->scaleY = 0.3f;
                countSprite->opacity = 0.0f;
                countSprite->mainFunc = sprite_bonus_banana_tick;
                sprintf(countSprite->text, "bonus banana.pic");
            }
        }
    }
}

void sprite_ranking_disp(struct Sprite *sprite) {}

void create_ranking_sprite(struct Ball *ball)
{
    struct Viewport *vp;
    struct Sprite *rankIcon;

    if (ball->rank == 0)
        return;
    vp = (struct Viewport *)&cameras[ball->playerId].sub28.viewportX;
    rankIcon = create_sprite();
    if (rankIcon == NULL)
        return;
    rankIcon->x = (vp->left + vp->width * 0.5) * 640.0;
    rankIcon->y = (vp->top + vp->height * 0.5) * 480.0;
    rankIcon->type = SPRITE_TYPE_BITMAP;
    rankIcon->textAlign = ALIGN_CC;
    rankIcon->userVar = ball->rank;
    rankIcon->bmpId = ball->playerId;
    rankIcon->counter = 0;
    rankIcon->mainFunc = sprite_ranking_tick;
    rankIcon->drawFunc = sprite_ranking_disp;
    strcpy(rankIcon->text, "ranking");
}

/* Record the goal the ball entered and the ball's velocity relative to it (including the
 * goal's own motion when it is on a moving item group). */
void g_something_with_goals_and_ball(struct Ball *ball, int goalId, int itemGroupId, s8 goalType)
{
    mode_info.goalEntered = goalId;
    mode_info.goalItemGroup = itemGroupId;
    mode_info.unk10 = ball->vel;
    mode_info.unk1C = mode_info.timerCurr;
    mode_info.goalType = goalType;
    if (itemGroupId > 0)
    {
        struct ItemGroup *itemGroup = &itemgroups[itemGroupId];
        struct StageGoal *goal = &stagedef->goals[goalId];
        Vec sp20;
        Vec sp14;

        mtxa_from_mtx(itemGroup->prevTransform);
        mtxa_tf_point(&goal->pos, &sp14);
        mtxa_from_mtx(itemGroup->transform);
        mtxa_tf_point(&goal->pos, &sp20);

        mode_info.unk10.x += sp14.x - sp20.x;
        mode_info.unk10.y += sp14.y - sp20.y;
        mode_info.unk10.z += sp14.z - sp20.z;
    }
}

/* Is the ball below the stage's fall-out height or inside a fall-out volume? */
BOOL did_ball_fallout(struct Ball *ball)
{
    struct PhysicsBall physBall;
    struct ItemGroup *itemGroup;
    struct StageCollHeader *coll;
    int itemGroupId;

    if (ball->pos.y < *stagedef->falloutY)
        return TRUE;
    init_physicsball_from_ball(ball, &physBall);
    itemGroup = itemgroups;
    coll = stagedef->collHeaders;
    for (itemGroupId = 0; itemGroupId < stagedef->itemGroupCount; itemGroupId++, itemGroup++, coll++)
    {
        struct FalloutVolume *volume;
        int i;

        if (itemGroupId != physBall.itemGroupId)
            tf_physball_to_itemgroup_space(&physBall, itemGroupId);
        volume = coll->falloutVolumes;
        for (i = 0; i < coll->falloutVolumeCount; i++, volume++)
        {
            Vec spC;

            mtxa_from_translate(&volume->pos);
            mtxa_rotate_z(volume->rotZ);
            mtxa_rotate_y(volume->rotY);
            mtxa_rotate_x(volume->rotX);
            mtxa_rigid_inv_tf_point(&physBall.pos, &spC);
            spC.x /= volume->scale.x;
            spC.y /= volume->scale.y;
            spC.z /= volume->scale.z;
            if (spC.x < -0.5 || spC.x > 0.5)
                continue;
            if (spC.y < -0.5 || spC.y > 0.5)
                continue;
            if (spC.z < -0.5 || spC.z > 0.5)
                continue;
            return TRUE;
        }
    }
    return FALSE;
}

/* Goal voice line: an early finish gets a cheer (another on a first try), otherwise a
 * plain "goal". */
void g_something_playing_soundeffects(struct Ball *ball)
{
    g_smth_with_sound = ball->ape->charaId;
    g_player_id_for_sound = ball->playerId;
    if (mode_info.timerCurr > (mode_info.timerMax >> 1))
    {
        if (mode_info.unk22 != 1)
            call_SoundReqID_arg_0(0x2859);
        else
            call_SoundReqID_arg_0(0x2858);
    }
    else
        call_SoundReqID_arg_0(0x281B);
}

/* Add time to the clock (capped at 5:60.95), keeping timerMax at least as large. */
void add_stage_time(int time)
{
    if (0x549B - mode_info.timerCurr > time)
        mode_info.timerCurr += time;
    else
        mode_info.timerCurr = 0x549B;
    if (mode_info.timerMax < mode_info.timerCurr)
        mode_info.timerMax = mode_info.timerCurr;
    if (mode_info.timerCurr < 0)
        mode_info.timerCurr = 0;
}

/* Unreferenced (stripped at link): places these unused arrays in .bss order. */
void info_force_bss_order(void **p)
{
    p[0] = lbl_1_bss_70DC;
    p[1] = playerInfos;
}
