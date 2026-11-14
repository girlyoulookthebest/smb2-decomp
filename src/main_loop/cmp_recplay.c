/* Records the current attempt into one variable-length replay, stored as byte planes so that
 * it compresses well with the RLE scheme below. SMB1's recplay_2.c. */
#include "types.h"
#include "ball.h"
#include "course.h"
#include "libc.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "recplay.h"
#include "stage.h"
#include "world.h"

typedef int OSHeapHandle;

#define ALIGN4(x) (((x) + 3) & ~3)

struct ReplayItemGroupTrack
{
    /*0x00*/ u16 id;
    /*0x02*/ s16 animFrame;
    /*0x04*/ u8 * DPTR frames;
}; /* size = 0x8 */

struct ReplaySeesawTrack
{
    /*0x00*/ u16 id;
    /*0x02*/ u16 stateSize;
    /*0x04*/ u8 * DPTR frames;
}; /* size = 0x8 */

struct ReplayTracks
{
    /*0x00*/ s32 startTimer;
    /*0x04*/ u16 itemGroupCount;
    /*0x06*/ u16 seesawCount;
    /*0x08*/ struct ReplayItemGroupTrack * DPTR itemGroups;
    /*0x0C*/ struct ReplaySeesawTrack * DPTR seesaws;
}; /* size = 0x10 */

struct Replay
{
    /*0x00*/ struct ReplayHeader header;
    /*0x18*/ s16 timerStart;
    /*0x1A*/ s16 frameCount;
    /*0x1C*/ s16 timerEnd;
    /*0x1E*/ s16 goalEntered;
    /*0x20*/ s16 goalItemGroup;
    u8 unk22[2];
    /*0x24*/ Vec goalVel;
    /*0x30*/ u32 unk30;
    /*0x34*/ u32 size;
    /*0x38*/ s16 currFrame;
    /*0x3A*/ s16 maxFrames;
    /*0x3C*/ Vec startPos;
    /*0x48*/ u8 * DPTR posXLo;
    /*0x4C*/ u8 * DPTR posXHi;
    /*0x50*/ u8 * DPTR posYLo;
    /*0x54*/ u8 * DPTR posYHi;
    /*0x58*/ u8 * DPTR posZLo;
    /*0x5C*/ u8 * DPTR posZHi;
    /*0x60*/ u8 * DPTR rotXLo;
    /*0x64*/ u8 * DPTR rotXHi;
    /*0x68*/ u8 * DPTR rotYLo;
    /*0x6C*/ u8 * DPTR rotYHi;
    /*0x70*/ u8 * DPTR rotZLo;
    /*0x74*/ u8 * DPTR rotZHi;
    /*0x78*/ u8 * DPTR normX;
    /*0x7C*/ u8 * DPTR normY;
    /*0x80*/ u8 * DPTR normZ;
    /*0x84*/ s8 * DPTR speed;
    /*0x88*/ u8 * DPTR flags0;
    /*0x8C*/ u8 * DPTR flags1;
    /*0x90*/ u8 * DPTR flags2;
    /*0x94*/ u8 * DPTR flags3;
    /*0x98*/ u8 * DPTR tiltXLo;
    /*0x9C*/ u8 * DPTR tiltXHi;
    /*0xA0*/ u8 * DPTR tiltZLo;
    /*0xA4*/ u8 * DPTR tiltZHi;
    /*0xA8*/ struct ReplayTracks tracks;
}; /* size = 0xB8 */

struct ReplayWorldFrame
{
    s16 rotX;
    s16 rotZ;
};

struct RLEHeader
{
    /*0x00*/ u16 version;
    /*0x02*/ u16 flags;
    /*0x04*/ u32 size;
};

struct ReplayItemGroupState
{
    /*0x00*/ u32 playbackState;
    /*0x04*/ float animFrame;
};

#define NO_COMPRESSION 1

struct
{
    /*0x00*/ s16 state;
    /*0x02*/ u16 flags;
    /*0x04*/ Vec ballPos;
} lbl_1_bss_71EE0;
struct Replay *replay;
s32 g_loaded_player_score;
struct
{
    s16 unk0;
    u16 unk2;
    u8 filler4[4];
} lbl_1_bss_71EF8 __attribute__((aligned(8)));

extern OSHeapHandle stage_heap;
extern void *g_some_buffer_ptr;
extern struct { s8 isStoryMode; } stageselect_is_storymode;
extern s32 active_monkey_id[];
extern s32 practice_best_score;
extern s32 is_practice_score_new_best;

void *OSAllocFromHeap(OSHeapHandle heap, u32 size);
void OSFreeToHeap(OSHeapHandle heap, void *ptr);
void *alloc_from_heap_or_panic(OSHeapHandle heap, u32 size, char *file, int line);
int get_seesaw_replay_state_size(struct Seesaw *seesaw);
void fn_1_58768(struct Seesaw *seesaw, void *out);
BOOL is_itemgroup_playable(int itemGroupIdx);
int get_playable_itemgroup_count(void);
void create_postgoal_score_sprites(int a, int b, int c);

void fn_1_64A7C(int save);
void fn_1_654C8(int frame, struct ReplayBallFrame *out);
void fn_1_65B1C(int frame, struct ReplayWorldFrame *out);
struct Replay *create_replay(int frames);
struct Replay *init_replay(u8 *mem, int frameCount);
BOOL init_replay_playable_ig_and_seesaw_state(struct Replay *r, u8 *data, u32 tracksSize, int frames);
void fn_1_66E94(struct Replay *r);
BOOL fn_1_66EC4(struct Replay *r);
BOOL fn_1_67368(struct Replay *dst, struct Replay *src);
void fn_1_676B0(struct Replay *r);
void convert_replay_ptrs_to_offsets(struct Replay *replay, struct Replay *out);
void convert_replay_offsets_to_ptrs(struct Replay *replay);
int get_goal_score(u32 *flags, int *timeScore);

static inline int replay_track_count(u16 *count)
{
    return *count;
}

void fn_1_63F8C(void)
{
    lbl_1_bss_71EE0.state = 0;
}

void fn_1_63FA0(void) {}

void fn_1_63FA4(void) {}

void fn_1_63FA8(void)
{
    struct Ball *ball = &balls[modeCtrl.currPlayer];
    struct World *world = &world_infos[modeCtrl.currPlayer];
    s32 x, y, z;
    s32 speed;
    u32 temp;
    u32 flags;
    u32 f;
    u32 frameNum;
    s16 var;
    float scale;

    switch (lbl_1_bss_71EE0.state)
    {
    case 1:
    frameNum = replay->currFrame;
    if (frameNum < (u32)replay->maxFrames)
    {
        flags = ball->flags;
        if (lbl_1_bss_71EE0.flags & 1)
        {
            flags |= 0x10000;
            lbl_1_bss_71EE0.flags &= ~1;
        }
        if (!(flags & 0x10000))
        {
            x = 10922.333f * (ball->pos.x - lbl_1_bss_71EE0.ballPos.x);
            y = 10922.333f * (ball->pos.y - lbl_1_bss_71EE0.ballPos.y);
            z = 10922.333f * (ball->pos.z - lbl_1_bss_71EE0.ballPos.z);
        }
        else
        {
            x = 127.99609375f * (ball->pos.x - lbl_1_bss_71EE0.ballPos.x);
            y = 127.99609375f * (ball->pos.y - lbl_1_bss_71EE0.ballPos.y);
            z = 127.99609375f * (ball->pos.z - lbl_1_bss_71EE0.ballPos.z);
        }
        if (x < -32767)
            x = -32767;
        else if (x > 32767)
            x = 32767;
        if (y < -32767)
            y = -32767;
        else if (y > 32767)
            y = 32767;
        if (z < -32767)
            z = -32767;
        else if (z > 32767)
            z = 32767;

        replay->posXLo[frameNum] = x & 0xFF;
        x >>= 8;
        replay->posXHi[frameNum] = x & 0xFF;

        replay->posYLo[frameNum] = y & 0xFF;
        y >>= 8;
        replay->posYHi[frameNum] = y & 0xFF;

        replay->posZLo[frameNum] = z & 0xFF;
        z >>= 8;
        replay->posZHi[frameNum] = z & 0xFF;

        temp = (u16)ball->rotX;
        replay->rotXLo[frameNum] = temp & 0xFF;
        temp >>= 8;
        replay->rotXHi[frameNum] = temp & 0xFF;

        temp = (u16)ball->rotY;
        replay->rotYLo[frameNum] = temp & 0xFF;
        temp >>= 8;
        replay->rotYHi[frameNum] = temp & 0xFF;

        temp = (u16)ball->rotZ;
        replay->rotZLo[frameNum] = temp & 0xFF;
        temp >>= 8;
        replay->rotZHi[frameNum] = temp & 0xFF;

        replay->normX[frameNum] = 127.0f * ball->unk11C.x;
        replay->normY[frameNum] = 127.0f * ball->unk11C.y;
        replay->normZ[frameNum] = 127.0f * ball->unk11C.z;

        speed = 127.0f * ball->unk13C;
        if (speed < -127)
            speed = -127;
        else if (speed > 127)
            speed = 127;
        replay->speed[frameNum] = speed;

        f = flags;
        replay->flags0[frameNum] = f & 0xFF;
        f >>= 8;
        replay->flags1[frameNum] = f & 0xFF;
        f >>= 8;
        replay->flags2[frameNum] = f & 0xFF;
        f >>= 8;
        replay->flags3[frameNum] = f & 0xFF;

        temp = (u16)world->xrot;
        replay->tiltXLo[frameNum] = temp;
        temp >>= 8;
        replay->tiltXHi[frameNum] = temp;

        temp = (u16)world->zrot;
        replay->tiltZLo[frameNum] = temp;
        temp >>= 8;
        replay->tiltZHi[frameNum] = temp;

        if (flags & 0x10000)
            scale = 0.0078127f;
        else
            scale = 9.1556e-5f;
        var = (replay->posXHi[frameNum] << 8) | replay->posXLo[frameNum];
        lbl_1_bss_71EE0.ballPos.x += var * scale;
        var = (replay->posYHi[frameNum] << 8) | replay->posYLo[frameNum];
        lbl_1_bss_71EE0.ballPos.y += var * scale;
        var = (replay->posZHi[frameNum] << 8) | replay->posZLo[frameNum];
        lbl_1_bss_71EE0.ballPos.z += var * scale;

        replay->frameCount++;
        fn_1_676B0(replay);
        replay->currFrame = ++frameNum;
    }
        break;
    }
}

void fn_1_6467C(void)
{
    fn_1_64A7C(1);
}

void fn_1_646A0(void)
{
    struct Ball *ball = &balls[modeCtrl.currPlayer];
    u32 flags = 0;
    int difficulty;
    int floor;

    if (modeCtrl.gameType == 0xE || (modeCtrl.gameType == 2 && stageselect_is_storymode.isStoryMode == 1))
        flags |= 0x1000;
    if (mode_info.flags & INFO_FLAG_BONUS_STAGE)
        flags |= 8;
    switch (modeCtrl.gameType)
    {
    case 2:
        if (g_stage_id_in_practice_mode[0].courseFlags & 8)
            flags |= 0x20;
        if (g_stage_id_in_practice_mode[0].courseFlags & 0x10)
            flags |= 0x40;
        if (g_stage_id_in_practice_mode[0].courseFlags & 0x800000)
            flags |= 0x800;
        break;
    default:
        if (modeCtrl.courseFlags & 8)
            flags |= 0x20;
        if (modeCtrl.courseFlags & 0x10)
            flags |= 0x40;
        if (modeCtrl.courseFlags & 0x800000)
            flags |= 0x800;
        break;
    }
    switch (modeCtrl.gameType)
    {
    case 2:
        switch (stageselect_is_storymode.isStoryMode)
        {
        case 1:
            difficulty = mode_info.storyWorld;
            floor = mode_info.storyStage;
            break;
        default:
            difficulty = g_stage_id_in_practice_mode[0].difficulty;
            floor = g_stage_id_in_practice_mode[0].floor;
            break;
        }
        break;
    case 0xE:
        difficulty = mode_info.storyWorld;
        floor = mode_info.storyStage;
        break;
    default:
        difficulty = modeCtrl.difficulty;
        floor = mode_info.currFloor;
        break;
    }
    if (replay != NULL)
        fn_1_66E94(replay);
    replay = create_replay(mode_info.timerMax);
    lbl_1_bss_71EE0.ballPos = ball->pos;
    replay->startPos = ball->pos;
    replay->unk30 = 1;
    replay->currFrame = 0;
    replay->timerStart = mode_info.timerCurr;
    replay->frameCount = 0;
    replay->header.flags = (u16)flags;
    replay->header.stageId = current_stage_id;
    replay->header.difficulty = difficulty;
    replay->header.floor = floor;
    replay->header.monkey = active_monkey_id[ball->playerId];
    replay->header.name[0] = 0;
    fn_1_676B0(replay);
    replay->header.score = -1;
    lbl_1_bss_71EE0.state = 1;
}

void fn_1_64A7C(int save)
{
    if (replay != NULL && lbl_1_bss_71EE0.state == 1)
    {
        lbl_1_bss_71EE0.state = 0;
        if (save)
            fn_1_66EC4(replay);
    }
}

void fn_1_64AE8(void)
{
    if (lbl_1_bss_71EE0.state == 1)
    {
        replay->header.flags |= 1;
        replay->timerEnd = mode_info.unk1C;
        replay->goalVel = mode_info.unk10;
        replay->goalEntered = mode_info.goalEntered;
        replay->goalItemGroup = mode_info.goalItemGroup;
    }
}

void fn_1_64BA8(void)
{
    if (lbl_1_bss_71EE0.state == 1)
    {
        replay->header.flags |= 2;
        replay->header.score = balls[modeCtrl.currPlayer].score - g_loaded_player_score;
        replay->timerEnd = mode_info.timerCurr;
    }
}

void fn_1_64C1C(void)
{
    if (lbl_1_bss_71EE0.state == 1)
    {
        replay->header.flags |= 4;
        replay->header.score = balls[modeCtrl.currPlayer].score - g_loaded_player_score;
    }
}

void g_something_with_score(void)
{
    replay->header.score = balls[modeCtrl.currPlayer].score - g_loaded_player_score;
}

void fn_1_64CC0(char *name)
{
    strncpy(replay->header.name, name, 4);
    replay->header.name[3] = 0;
}

void fn_1_64D0C(void)
{
    if (lbl_1_bss_71EE0.state == 1)
        lbl_1_bss_71EE0.flags |= 1;
}

void fn_1_64D3C(void)
{
    mode_info.timerMax = replay->timerStart;
    mode_info.timerCurr = replay->timerStart;
    mode_info.flags &= ~0x667;
    if (replay->header.flags & 1)
    {
        mode_info.flags |= INFO_FLAG_TIMER_PAUSED | INFO_FLAG_05;
        mode_info.timerCurr = replay->timerEnd;
        mode_info.unk1C = replay->timerEnd;
        mode_info.unk10 = replay->goalVel;
        mode_info.goalEntered = replay->goalEntered;
        mode_info.goalItemGroup = replay->goalItemGroup;
    }
    if (replay->header.flags & 8)
        mode_info.flags |= INFO_FLAG_BONUS_STAGE;
    active_monkey_id[lbl_1_bss_71EC8.unk14] = replay->header.monkey;
    if (!(replay->header.flags & 0x1000))
    {
        modeCtrl.difficulty = replay->header.difficulty;
        mode_info.currFloor = replay->header.floor;
    }
    else
    {
        mode_info.storyWorld = replay->header.difficulty;
        mode_info.storyStage = replay->header.floor;
    }
    modeCtrl.courseFlags &= ~0x800018;
    if (replay->header.flags & 0x20)
        modeCtrl.courseFlags |= 8;
    if (replay->header.flags & 0x40)
        modeCtrl.courseFlags |= 0x10;
    if (replay->header.flags & 0x800)
        modeCtrl.courseFlags |= 0x800000;
}

float fn_1_65010(void)
{
    return replay->frameCount;
}

void g_advance_replay2(float time, struct ReplayBallFrame *out)
{
    struct ReplayBallFrame sp64;
    struct ReplayBallFrame sp44;
    struct ReplayBallFrame sp24;
    Vec sp18;
    float temp_f2;
    float t;
    float s;
    int frame;
    struct Ball *ball;
    u8 unused[4];

    ball = current_ball;
    if (replay->frameCount == 0)
    {
        out->pos = ball->pos;
        out->rotX = ball->rotX;
        out->rotY = ball->rotY;
        out->rotZ = ball->rotZ;
        return;
    }
    if (time < 0.0f)
        time = 0.0f;
    else if (replay->frameCount <= time)
        time = replay->frameCount;

    frame = time;
    temp_f2 = frame;
    t = time - temp_f2;
    s = 1.0 - t;
    fn_1_654C8(frame, &sp64);
    fn_1_654C8(frame + 1, &sp44);

    sp24.pos.x = (sp64.pos.x * s) + (sp44.pos.x * t);
    sp24.pos.y = (sp64.pos.y * s) + (sp44.pos.y * t);
    sp24.pos.z = (sp64.pos.z * s) + (sp44.pos.z * t);
    sp24.rotX = (sp64.rotX * s) + (sp44.rotX * t);
    sp24.rotY = (sp64.rotY * s) + (sp44.rotY * t);
    sp24.rotZ = (sp64.rotZ * s) + (sp44.rotZ * t);
    sp18.x = (sp64.unk12 * s) + (sp44.unk12 * t);
    sp18.y = (sp64.unk14 * s) + (sp44.unk14 * t);
    sp18.z = (sp64.unk16 * s) + (sp44.unk16 * t);
    vec_set_len(32767.0f, &sp18, &sp18);
    sp24.unk12 = sp18.x;
    sp24.unk14 = sp18.y;
    sp24.unk16 = sp18.z;
    sp24.flags = sp64.flags;
    sp24.unk1C = (sp64.unk1C * s) + (sp44.unk1C * t);
    *out = sp24;
}

void fn_1_654C8(int frame, struct ReplayBallFrame *out)
{
    struct ReplayBallFrame sp10;
    int i;
    int idx = replay->frameCount - 1 - frame;
    u8 *xLo;
    u8 *yLo;
    u8 *zLo;
    u8 *xHi;
    u8 *yHi;
    u8 *zHi;
    u8 *flags2;
    s16 var;
    float scale;

    if (idx < 0)
        idx = 0;
    sp10.pos = replay->startPos;
    xLo = replay->posXLo;
    yLo = replay->posYLo;
    zLo = replay->posZLo;
    xHi = replay->posXHi;
    yHi = replay->posYHi;
    zHi = replay->posZHi;
    flags2 = replay->flags2;
    for (i = idx + 1; i > 0; i--, xLo++, yLo++, zLo++, xHi++, yHi++, zHi++, flags2++)
    {
        if (*flags2 & 1)
            scale = 0.0078127f;
        else
            scale = 9.1556e-5f;
        var = (*xHi << 8) | *xLo;
        sp10.pos.x += var * scale;
        var = (*yHi << 8) | *yLo;
        sp10.pos.y += var * scale;
        var = (*zHi << 8) | *zLo;
        sp10.pos.z += var * scale;
    }

    sp10.rotX = replay->rotXLo[idx] | (replay->rotXHi[idx] << 8);
    sp10.rotY = replay->rotYLo[idx] | (replay->rotYHi[idx] << 8);
    sp10.rotZ = replay->rotZLo[idx] | (replay->rotZHi[idx] << 8);
    sp10.unk12 = (s8)replay->normX[idx] << 8;
    sp10.unk14 = (s8)replay->normY[idx] << 8;
    sp10.unk16 = (s8)replay->normZ[idx] << 8;
    sp10.flags = (replay->flags3[idx] << 24)
               | (replay->flags2[idx] << 16)
               | (replay->flags1[idx] << 8)
               | replay->flags0[idx];
    sp10.unk1C = 0.007874f * (float)replay->speed[idx];

    *out = sp10;
}

void fn_1_658BC(float time, struct ReplayWorldFrame *out)
{
    struct ReplayWorldFrame sp20;
    struct ReplayWorldFrame sp1C;
    struct ReplayWorldFrame sp18;
    float temp_f2;
    float t;
    float s;
    s32 frame;
    struct World *world;
    u8 unused[4];

    world = current_world_info;
    if (replay->frameCount == 0)
    {
        out->rotX = world->xrot;
        out->rotZ = world->zrot;
        return;
    }
    if (time < 0.0f)
        time = 0.0f;
    else if (replay->frameCount <= time)
        time = replay->frameCount;

    frame = time;
    temp_f2 = frame;
    t = time - temp_f2;
    s = 1.0 - t;
    fn_1_65B1C(frame, &sp20);
    fn_1_65B1C(frame + 1, &sp1C);
    sp18.rotX = (sp20.rotX * s) + (sp1C.rotX * t);
    sp18.rotZ = (sp20.rotZ * s) + (sp1C.rotZ * t);
    *out = sp18;
}

void fn_1_65B1C(int frame, struct ReplayWorldFrame *out)
{
    struct ReplayWorldFrame sp10;
    int i = replay->frameCount - 1 - frame;

    if (i < 0)
        i = 0;
    sp10.rotX = replay->tiltXLo[i] | (replay->tiltXHi[i] << 8);
    sp10.rotZ = replay->tiltZLo[i] | (replay->tiltZHi[i] << 8);
    *out = sp10;
}

float fn_1_65BC4(float time)
{
    float t;

    t = time + (replay->timerStart - replay->frameCount);
    if (replay->header.flags & 1)
    {
        if (t < replay->timerEnd)
            t = replay->timerEnd;
    }
    return t;
}

float fn_1_65CA8(float time)
{
    return replay->tracks.startTimer - time;
}

void fn_1_65CEC(struct ReplayHeader *header)
{
    if (replay == NULL)
        header->flags = 0x200;
    else
        *header = replay->header;
}

int g_get_replay_time(void)
{
    return replay->timerEnd;
}

void fn_1_65D60(float time, int id, struct ReplayItemGroupState *out)
{
    struct Replay *r = replay;
    struct ReplayItemGroupTrack *track;
    int frame;
    int anim;
    int i;
    u8 *state;
    BOOL found;

    if (id == 0)
    {
        out->playbackState = 0;
        out->animFrame = 0.0f;
        return;
    }
    if (r->tracks.itemGroups == NULL)
    {
        out->playbackState = 0;
        out->animFrame = r->tracks.startTimer - time;
        return;
    }
    found = FALSE;
    track = r->tracks.itemGroups;
    for (frame = r->tracks.itemGroupCount; frame > 0; frame--, track++)
    {
        if (track->id == id)
        {
            found = TRUE;
            break;
        }
        if (track->id > id)
            break;
    }
    if (!found)
    {
        out->playbackState = 0;
        out->animFrame = r->tracks.startTimer - time;
        return;
    }
    if (time < 0.0)
        frame = r->currFrame - 1;
    else if (replay->frameCount <= time)
        frame = 0;
    else
        frame = r->currFrame - 1 - (int)time;
    i = r->currFrame - 1;
    anim = track->animFrame;
    state = track->frames + i;
    if (i > frame)
    {
        for (i--, state--; i > frame; i--, state--)
        {
            switch (*state)
            {
            case 0:
                anim--;
                break;
            case 2:
                anim++;
                break;
            case 3:
                anim -= 2;
                break;
            case 4:
                anim += 2;
                break;
            }
        }
    }
    out->playbackState = *state;
    out->animFrame = anim - (time - (int)time);
}

void fn_1_65FD0(int t, int id, void *out)
{
    struct Replay *r = replay;
    struct ReplaySeesawTrack *track;
    int i;
    int frame;

    if (r->tracks.seesaws == NULL)
        return;
    track = r->tracks.seesaws;
    for (i = replay_track_count(&r->tracks.seesawCount); i > 0; i--, track++)
    {
        if (track->id == id)
            break;
    }
    if (i == 0)
        return;
    if (t < 0.0)
        frame = r->currFrame - 1;
    else if ((float)replay->frameCount <= (float)t)
        frame = 0;
    else
        frame = r->currFrame - 1 - t;
    memcpy_handler(out, track->frames + frame * (track->stateSize >> 2) * 4, track->stateSize);
}

u32 rle_encode(u8 *src, u8 *dest, u32 srcSize)
{
    u32 destPos;
    int i;
    u8 r6;
    u32 r5;
    u8 *srcp;
    int r9;
    int tooLarge;
    u8 *destp;

    srcp = src;
    destPos = sizeof(struct RLEHeader);
    destp = dest + sizeof(struct RLEHeader);

    r6 = src[0];
    if (dest != NULL)
        destp[1] = r6;
    r5 = 1;
    if (r6 == srcp[1])
        r9 = 1;
    else
        r9 = 0;
    tooLarge = FALSE;
    srcp++;
    for (i = srcSize - 1; i > 0; i--, srcp++)
    {
        if (r9)
        {
            if (*srcp == r6 && r5 < 0x7F)
            {
                r5++;
                continue;
            }
            if (dest != NULL)
                *destp = r5 | 0x80;
            destp += 2;
            destPos += 2;
            if (destPos >= srcSize)
            {
                tooLarge = TRUE;
                break;
            }
            r6 = *srcp;
            if (dest != 0)
                destp[1] = r6;
            r5 = 1;
            if (r6 != srcp[1])
                r9 = 0;
        }
        else if (*srcp == r6 || r5 == 0x7F)
        {
            if (dest != NULL)
                *destp = r5;
            destp = (u8 *)(uptr)r5 + (uptr)destp;
            destp++;
            destPos = r5 + destPos;
            destPos++;
            if (destPos >= srcSize)
            {
                tooLarge = TRUE;
                break;
            }
            r6 = *srcp;
            if (dest != NULL)
                destp[1] = r6;
            r5 = 1;
            if (r6 == srcp[1])
                r9 = 1;
        }
        else
        {
            r5++;
            if (destPos + (r5 + 1) >= srcSize)
            {
                tooLarge = TRUE;
                break;
            }
            r6 = *srcp;
            if (dest != NULL)
                destp[r5] = r6;
        }
    }

    if (!tooLarge)
    {
        if (r9 != 0)
        {
            if (dest != NULL)
                *destp = r5 | 0x80;
            destPos += 2;
            if (destPos >= srcSize)
                tooLarge = 1;
        }
        else
        {
            if (dest != NULL)
                *destp = r5;
            destPos = r5 + destPos;
            destPos++;
            if (destPos >= srcSize)
                tooLarge = 1;
        }
    }

    if (!tooLarge)
    {
        struct RLEHeader header;

        header.version = 1;
        header.flags = 0;
        header.size = srcSize;
        if (dest != NULL)
            memcpy_handler(dest, &header, sizeof(header));
    }
    else
    {
        struct RLEHeader header;

        destPos = srcSize + sizeof(struct RLEHeader);
        header.version = 1;
        header.flags = NO_COMPRESSION;
        header.size = srcSize;
        if (dest != NULL)
        {
            memcpy_handler(dest, &header, sizeof(header));
            memcpy_handler(dest + sizeof(header), src, srcSize);
        }
    }
    return destPos;
}

int fn_1_663BC(u8 *src, u8 *dest, u32 destSize)
{
    struct RLEHeader header;
    u32 n;
    u8 *destp;
    u8 *srcp;
    int isRun;
    int remaining;
    u8 val;

    memcpy_handler(&header, src, sizeof(header));
    if (header.version != 1)
        return FALSE;
    if (header.size > destSize)
        return FALSE;
    if (header.flags & NO_COMPRESSION)
    {
        memcpy_handler(dest, src + sizeof(struct RLEHeader), header.size);
        return TRUE;
    }

    srcp = src + sizeof(struct RLEHeader);
    destp = dest;
    remaining = header.size;
    while (remaining != 0)
    {
        isRun = *srcp & 0x80;
        n = *srcp & 0x7F;
        srcp++;
        if (isRun)
        {
            val = *srcp;
            while (n != 0 && remaining > 0)
            {
                *destp = val;
                n--;
                destp++;
                remaining--;
            }
            srcp++;
        }
        else
        {
            while (n != 0 && remaining > 0)
            {
                *destp = *srcp;
                n--;
                destp++;
                remaining--;
                srcp++;
            }
        }
    }
    return TRUE;
}

/* Bytes needed for a replay of the current stage with the time limit's frame count. */
static inline int replay_tracks_size(int frames)
{
    int tracks;
    int i;
    struct ItemGroup *ig;
    struct StageCollHeader *stageIg;
    int count;
    int planes;

    tracks = 0;
    count = get_playable_itemgroup_count();
    planes = 8;
    planes *= count;
    tracks += ALIGN4(planes);
    planes = ALIGN4(frames);
    tracks += ALIGN4(planes * count);
    count = seesaw_count;
    if (count != 0)
    {
        planes = 8;
        tracks += ALIGN4(planes * count);
        stageIg = stagedef->collHeaders;
        ig = itemgroups;
        for (i = stagedef->itemGroupCount; i > 0; i--, ig++, stageIg++)
        {
            void *seesaw = ig->seesaw;

            if (seesaw != NULL)
            {
                planes = frames * get_seesaw_replay_state_size(seesaw);
                planes = ALIGN4(planes);
                tracks += planes;
            }
        }
    }
    return tracks;
}

static inline int replay_size(int frames)
{
    int size = 0xB8;
    int n;

    n = 6;
    n += 6;
    n += 4;
    n += 4;
    n *= frames;
    n = ALIGN4(n);
    size += n;
    n = 4;
    n *= frames;
    n = ALIGN4(n);
    size += n;
    size += replay_tracks_size(frames);
    return size;
}

int fn_1_664EC(void)
{
    return replay_size(mode_info.timerMax) + 8;
}

u32 get_compressed_replay_size(void)
{
    u32 size;

    convert_replay_ptrs_to_offsets(replay, replay);
    size = rle_encode((u8 *)replay, NULL, replay->size);
    convert_replay_offsets_to_ptrs(replay);
    return size;
}

u32 compress_replay(void *dest)
{
    u32 size;

    if (dest == NULL)
        return 0;
    convert_replay_ptrs_to_offsets(replay, replay);
    size = rle_encode((u8 *)replay, dest, replay->size);
    convert_replay_offsets_to_ptrs(replay);
    return size;
}

int g_smth_with_cmp_recplay(void *src)
{
    int ret;
    u32 size;

    if (replay != NULL)
    {
        fn_1_66E94(replay);
        replay = NULL;
    }
    size = ((struct RLEHeader *)src)->size;
    replay = alloc_from_heap_or_panic(stage_heap, size, "cmp_recplay.c", 0x610);
    ret = fn_1_663BC(src, (u8 *)replay, size);
    if (ret != 0)
        convert_replay_offsets_to_ptrs(replay);
    replay->header.flags |= 0x400;
    return ret;
}

int g_some_replay_func5(void)
{
    int ret = 0;

    if (lbl_1_bss_71EE0.state == 0 && replay != NULL && replay->frameCount != 0 && replay->header.score >= 0)
        ret = 1;
    return ret;
}

struct Replay *init_replay(u8 *p, int frames)
{
    int size;
    struct Replay *r;

    size = replay_size(frames);
    r = (void *)p;
    p += sizeof(struct Replay);
    r->size = size;
    r->maxFrames = frames;
    r->posXLo = p;
    p += frames;
    r->posXHi = p;
    p += frames;
    r->posYLo = p;
    p += frames;
    r->posYHi = p;
    p += frames;
    r->posZLo = p;
    p += frames;
    r->posZHi = p;
    p += frames;
    r->rotXLo = p;
    p += frames;
    r->rotXHi = p;
    p += frames;
    r->rotYLo = p;
    p += frames;
    r->rotYHi = p;
    p += frames;
    r->rotZLo = p;
    p += frames;
    r->rotZHi = p;
    p += frames;
    r->normX = p;
    p += frames;
    r->normY = p;
    p += frames;
    r->normZ = p;
    p += frames;
    r->speed = (s8 *)p;
    p += frames;
    r->flags0 = p;
    p += frames;
    r->flags1 = p;
    p += frames;
    r->flags2 = p;
    p += frames;
    r->flags3 = p;
    p += frames;
    r->tiltXLo = p;
    p += frames;
    r->tiltXHi = p;
    p += frames;
    r->tiltZLo = p;
    p += frames;
    r->tiltZHi = p;
    p += frames;
    init_replay_playable_ig_and_seesaw_state(r, p, replay_tracks_size(frames), frames);
    return r;
}

BOOL init_replay_playable_ig_and_seesaw_state(struct Replay *r, u8 *data, u32 tracksSize, int frames)
{
    int i;
    struct ReplayTracks *tracks;
    struct ReplayItemGroupTrack *igTrack;
    struct ReplaySeesawTrack *ssTrack;
    int n;
    struct ItemGroup *ig;
    struct StageCollHeader *stageIg;
    int count;

    if (tracksSize == 0)
        return TRUE;
    tracks = &r->tracks;
    count = get_playable_itemgroup_count();
    tracks->itemGroupCount = count;
    if (count == 0)
        tracks->itemGroups = NULL;
    else
    {
        n = 8;
        igTrack = (void *)data;
        tracks->itemGroups = igTrack;
        data += ALIGN4(n * count);
        n = frames;
        for (i = 0; i < stagedef->itemGroupCount; i++)
        {
            if (is_itemgroup_playable(i + 1))
            {
                igTrack->id = i + 1;
                igTrack->frames = data;
                data += ALIGN4(n);
                igTrack++;
            }
        }
    }
    count = seesaw_count;
    tracks->seesawCount = count;
    if (count == 0)
        tracks->seesaws = NULL;
    else
    {
        n = 8;
        ssTrack = (void *)data;
        tracks->seesaws = ssTrack;
        data += ALIGN4(n * count);
        stageIg = stagedef->collHeaders;
        ig = itemgroups;
        for (i = 0; i < stagedef->itemGroupCount; i++, ig++, stageIg++)
        {
            struct Seesaw *seesaw = ig->seesaw;

            if (seesaw != NULL)
            {
                u32 size;

                ssTrack->id = i;
                size = get_seesaw_replay_state_size(seesaw);
                ssTrack->stateSize = size;
                if (size == 0)
                    ssTrack->frames = NULL;
                else
                {
                    ssTrack->frames = data;
                    n = size * frames;
                    data += ALIGN4(n);
                }
                ssTrack++;
            }
        }
    }
    return TRUE;
}

struct Replay *create_replay(int frames)
{
    u32 size = replay_size(frames);
    struct Replay *r = OSAllocFromHeap(stage_heap, size);

    if (r == NULL)
        return NULL;
    memset(r, 0, size);
    return init_replay((u8 *)r, frames);
}

void fn_1_66E94(struct Replay *r)
{
    OSFreeToHeap(stage_heap, r);
}

BOOL fn_1_66EC4(struct Replay *r)
{
    BOOL allocated;
    u32 size;
    struct Replay *temp;

    if (r == NULL)
        return FALSE;
    if (r->currFrame >= r->maxFrames)
        return FALSE;
    size = replay_size(r->currFrame);
    if (0x10000 > size)
    {
        allocated = FALSE;
        temp = init_replay(g_some_buffer_ptr, r->currFrame);
    }
    else
    {
        allocated = TRUE;
        temp = create_replay(r->currFrame);
        if (temp == NULL)
            return FALSE;
    }
    fn_1_67368(temp, r);
    fn_1_66E94(r);
    if (allocated)
    {
        replay = temp;
        return TRUE;
    }
    r = create_replay(temp->currFrame);
    if (r == NULL)
    {
        replay = NULL;
        return FALSE;
    }
    fn_1_67368(r, temp);
    replay = r;
    return TRUE;
}

BOOL fn_1_67368(struct Replay *dst, struct Replay *src)
{
    struct Replay saved;
    int count = src->currFrame;
    int i;
    struct ReplayItemGroupTrack *dstIg;
    struct ReplayItemGroupTrack *srcIg;
    struct ReplaySeesawTrack *dstSs;
    struct ReplaySeesawTrack *srcSs;

    memcpy_handler(&saved, dst, sizeof(saved));
    memcpy_handler(dst, src, sizeof(saved));
    dst->size = saved.size;
    dst->maxFrames = saved.maxFrames;
    dst->posXLo = saved.posXLo;
    dst->posXHi = saved.posXHi;
    dst->posYLo = saved.posYLo;
    dst->posYHi = saved.posYHi;
    dst->posZLo = saved.posZLo;
    dst->posZHi = saved.posZHi;
    dst->rotXLo = saved.rotXLo;
    dst->rotXHi = saved.rotXHi;
    dst->rotYLo = saved.rotYLo;
    dst->rotYHi = saved.rotYHi;
    dst->rotZLo = saved.rotZLo;
    dst->rotZHi = saved.rotZHi;
    dst->normX = saved.normX;
    dst->normY = saved.normY;
    dst->normZ = saved.normZ;
    dst->speed = saved.speed;
    dst->flags0 = saved.flags0;
    dst->flags1 = saved.flags1;
    dst->flags2 = saved.flags2;
    dst->flags3 = saved.flags3;
    dst->tiltXLo = saved.tiltXLo;
    dst->tiltXHi = saved.tiltXHi;
    dst->tiltZLo = saved.tiltZLo;
    dst->tiltZHi = saved.tiltZHi;
    dst->tracks.itemGroups = saved.tracks.itemGroups;
    dst->tracks.seesaws = saved.tracks.seesaws;
    memcpy(dst->posXLo, src->posXLo, count);
    memcpy(dst->posXHi, src->posXHi, count);
    memcpy(dst->posYLo, src->posYLo, count);
    memcpy(dst->posYHi, src->posYHi, count);
    memcpy(dst->posZLo, src->posZLo, count);
    memcpy(dst->posZHi, src->posZHi, count);
    memcpy(dst->rotXLo, src->rotXLo, count);
    memcpy(dst->rotXHi, src->rotXHi, count);
    memcpy(dst->rotYLo, src->rotYLo, count);
    memcpy(dst->rotYHi, src->rotYHi, count);
    memcpy(dst->rotZLo, src->rotZLo, count);
    memcpy(dst->rotZHi, src->rotZHi, count);
    memcpy(dst->normX, src->normX, count);
    memcpy(dst->normY, src->normY, count);
    memcpy(dst->normZ, src->normZ, count);
    memcpy(dst->speed, src->speed, count);
    memcpy(dst->flags0, src->flags0, count);
    memcpy(dst->flags1, src->flags1, count);
    memcpy(dst->flags2, src->flags2, count);
    memcpy(dst->flags3, src->flags3, count);
    memcpy(dst->tiltXLo, src->tiltXLo, count);
    memcpy(dst->tiltXHi, src->tiltXHi, count);
    memcpy(dst->tiltZLo, src->tiltZLo, count);
    memcpy(dst->tiltZHi, src->tiltZHi, count);
    dstIg = dst->tracks.itemGroups;
    srcIg = src->tracks.itemGroups;
    for (i = dst->tracks.itemGroupCount; i > 0; i--, dstIg++, srcIg++)
    {
        dstIg->id = srcIg->id;
        dstIg->animFrame = srcIg->animFrame;
        memcpy_handler(dstIg->frames, srcIg->frames, count);
    }
    dstSs = dst->tracks.seesaws;
    srcSs = src->tracks.seesaws;
    for (i = dst->tracks.seesawCount; i > 0; i--, dstSs++, srcSs++)
    {
        dstSs->id = srcSs->id;
        dstSs->stateSize = srcSs->stateSize;
        memcpy_handler(dstSs->frames, srcSs->frames, dstSs->stateSize * count);
    }
    return TRUE;
}

void fn_1_676B0(struct Replay *r)
{
    struct ReplayTracks *tracks = &r->tracks;
    struct ReplayItemGroupTrack *igTrack;
    struct ReplaySeesawTrack *ssTrack;
    int i;
    int pos;

    tracks->startTimer = g_some_frame_counter.frameCounter;
    pos = r->currFrame;
    igTrack = tracks->itemGroups;
    if (igTrack != NULL)
    {
        for (i = replay_track_count(&tracks->itemGroupCount); i > 0; i--, igTrack++)
        {
            struct ItemGroup *ig = &itemgroups[igTrack->id];

            igTrack->animFrame = ig->animFrame;
            igTrack->frames[pos] = ig->playbackState;
        }
    }
    ssTrack = tracks->seesaws;
    if (ssTrack != NULL)
    {
        for (i = replay_track_count(&tracks->seesawCount); i > 0; i--, ssTrack++)
            fn_1_58768(itemgroups[ssTrack->id].seesaw,
                       (u32 *)ssTrack->frames + (ssTrack->stateSize >> 2) * pos);
    }
}

void convert_replay_ptrs_to_offsets(struct Replay *replay, struct Replay *out)
{
    struct Replay *r = replay;
    uptr base = (uptr)replay;
    int i;
    struct ReplayItemGroupTrack *igTrack;
    struct ReplaySeesawTrack *ssTrack;

    r->posXLo -= base;
    r->posXHi -= base;
    r->posYLo -= base;
    r->posYHi -= base;
    r->posZLo -= base;
    r->posZHi -= base;
    r->rotXLo -= base;
    r->rotXHi -= base;
    r->rotYLo -= base;
    r->rotYHi -= base;
    r->rotZLo -= base;
    r->rotZHi -= base;
    r->normX -= base;
    r->normY -= base;
    r->normZ -= base;
    r->speed -= base;
    r->flags0 -= base;
    r->flags1 -= base;
    r->flags2 -= base;
    r->flags3 -= base;
    r->tiltXLo -= base;
    r->tiltXHi -= base;
    r->tiltZLo -= base;
    r->tiltZHi -= base;
    if (r->tracks.itemGroups != NULL)
    {
        igTrack = r->tracks.itemGroups;
        for (i = r->tracks.itemGroupCount; i > 0; i--, igTrack++)
            igTrack->frames -= base;
        r->tracks.itemGroups = (void *)((u8 *)r->tracks.itemGroups - base);
    }
    if (r->tracks.seesaws != NULL)
    {
        ssTrack = r->tracks.seesaws;
        for (i = r->tracks.seesawCount; i > 0; i--, ssTrack++)
        {
            if (ssTrack->frames != NULL)
                ssTrack->frames -= base;
        }
        r->tracks.seesaws = (void *)((u8 *)r->tracks.seesaws - base);
    }
    if (out != r)
    {
        memcpy_handler(out, r, r->size);
        convert_replay_offsets_to_ptrs(replay);
    }
}

void convert_replay_offsets_to_ptrs(struct Replay *replay)
{
    struct Replay *r = replay;
    uptr base = (uptr)replay;
    int i;
    struct ReplayItemGroupTrack *igTrack;
    struct ReplaySeesawTrack *ssTrack;

    r->posXLo += base;
    r->posXHi += base;
    r->posYLo += base;
    r->posYHi += base;
    r->posZLo += base;
    r->posZHi += base;
    r->rotXLo += base;
    r->rotXHi += base;
    r->rotYLo += base;
    r->rotYHi += base;
    r->rotZLo += base;
    r->rotZHi += base;
    r->normX += base;
    r->normY += base;
    r->normZ += base;
    r->speed += base;
    r->flags0 += base;
    r->flags1 += base;
    r->flags2 += base;
    r->flags3 += base;
    r->tiltXLo += base;
    r->tiltXHi += base;
    r->tiltZLo += base;
    r->tiltZHi += base;
    if (r->tracks.itemGroups != NULL)
    {
        r->tracks.itemGroups = (void *)((u8 *)r->tracks.itemGroups + base);
        igTrack = r->tracks.itemGroups;
        for (i = r->tracks.itemGroupCount; i > 0; i--, igTrack++)
            igTrack->frames += base;
    }
    if (r->tracks.seesaws != NULL)
    {
        r->tracks.seesaws = (void *)((u8 *)r->tracks.seesaws + base);
        ssTrack = r->tracks.seesaws;
        for (i = r->tracks.seesawCount; i > 0; i--, ssTrack++)
        {
            if (ssTrack->frames != NULL)
                ssTrack->frames += base;
        }
    }
}

void fn_1_67B68(int keepLoaded)
{
    if (replay != NULL && (keepLoaded == 0 || !(replay->header.flags & 0x400)))
    {
        fn_1_66E94(replay);
        replay = NULL;
    }
}

void fn_1_67BE8(void)
{
    memset(&lbl_1_bss_71EF8, 0, sizeof(lbl_1_bss_71EF8));
}

void fn_1_67C18(void)
{
    lbl_1_bss_71EF8.unk0--;
    if (lbl_1_bss_71EF8.unk0 <= 0)
        lbl_1_bss_71EF8.unk2 = 0;
}

void fn_1_67C5C(void)
{
    memset(&lbl_1_bss_71EF8, 0, sizeof(lbl_1_bss_71EF8));
}

void increment_score(int type, int score)
{
    struct Ball *ball;

    switch (type)
    {
    case 2:
    case 3:
    {
        int timeScore;
        u32 flags = 0;
        int total = get_goal_score(&flags, &timeScore);

        if (type == 3)
        {
            create_postgoal_score_sprites(timeScore, total, flags);
            break;
        }
        switch (modeCtrl.gameType)
        {
        case 1:
        {
            int i;
            s8 *status;

            ball = balls;
            status = g_poolInfo.playerPool.statusList;
            for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++)
            {
                if (*status != 0)
                {
                    switch (ball->rank)
                    {
                    case 1:
                        ball->score += total;
                        break;
                    case 2:
                        ball->score += total / 2;
                        break;
                    case 3:
                        ball->score += total / 3;
                        break;
                    }
                    if (ball->score > 999999999)
                        ball->score = 999999999;
                }
            }
            break;
        }
        case 0:
            ball = &balls[modeCtrl.currPlayer];
            ball->score += total;
            if (ball->score > 999999999)
                ball->score = 999999999;
            break;
        case 2:
            ball = current_ball;
            ball->score += total;
            if (ball->score > 999999999)
                ball->score = 999999999;
            if (ball->score > practice_best_score)
            {
                if (practice_best_score > 0)
                    is_practice_score_new_best = 1;
                practice_best_score = ball->score;
            }
            break;
        default:
            ball = current_ball;
            ball->score += total;
            if (ball->score > 999999999)
                ball->score = 999999999;
            break;
        }
        break;
    }
    default:
        ball = current_ball;
        ball->score += score;
        if (ball->score > 999999999)
            ball->score = 999999999;
        break;
    }
}

int get_goal_score(u32 *flags, int *timeScore)
{
    int mult;
    int i;
    int t;
    u32 f;
    struct StageGoal *goal;
    int base;

    mult = mode_info.unk22;
    f = 0;
    t = mode_info.unk1C * 100 / 60;
    goal = stagedef->collHeaders->goals;
    for (i = 0; i < 3; i++, goal++)
    {
        if (i == mode_info.goalEntered)
            break;
    }
    if (goal->type == 1)
    {
        t += 10000;
        f |= 4;
    }
    else if (goal->type == 2)
    {
        t += 20000;
        f |= 8;
    }
    base = t;
    if (mode_info.unk22 != 1)
        f |= 2;
    if (mode_info.timerCurr > mode_info.timerMax >> 1)
    {
        f |= 1;
        mult <<= 1;
    }
    t *= mult;
    if (flags != NULL)
        *flags = f;
    if (timeScore != NULL)
        *timeScore = base;
    return t;
}
