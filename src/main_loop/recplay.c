/* Ball and stage recording: per-player ring buffers of frames in replay_heap, kept (best first) as
 * the demo replays shown between games. SMB1's recplay.c, with variable-length replays. */
#include "types.h"
#include "ball.h"
#include "course.h"
#include "dip.h"
#include "file.h"
#include "libc.h"
#include "macros.h"
#include "mode.h"
#include "rand.h"
#include "recplay.h"
#include "stage.h"
#include "world.h"

typedef int OSHeapHandle;

#define ALIGN4(x) (((x) + 3) & ~3)

#define REPLAY_MAX_FRAMES 720
#define REPLAY_SLOT_COUNT 17
#define REPLAY_SLOT_CMP 17  /* the slot kept in compressed form (cmp_recplay.c) */
#define REPLAY_EVICT_SLOTS 12

struct ReplayItemGroupTrack
{
    /*0x00*/ u32 itemGroupId;
    /*0x04*/ s16 unk4;
    /*0x08*/ u8 * DPTR frames;
}; /* size = 0xC */

struct ReplaySeesawTrack
{
    /*0x00*/ u32 itemGroupId;
    /*0x04*/ u32 stateSize;
    /*0x08*/ u8 * DPTR frames;
}; /* size = 0xC */

struct ReplayWorldFrame
{
    s16 rotX;
    s16 rotZ;
};

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
    /*0x18*/ u32 size;
    /*0x1C*/ s16 timerStart;
    /*0x1E*/ s16 frameCount;
    /*0x20*/ s16 timerEnd;
    /*0x22*/ s16 goalEntered;
    /*0x24*/ s16 goalItemGroup;
    /*0x26*/ u16 maxFrames;
    /*0x28*/ Vec goalVel;
    /*0x34*/ s16 writePos;
    /*0x36*/ s16 storedFrames;
    /*0x38*/ struct ReplayBallFrame * DPTR ballFrames;
    /*0x3C*/ struct ReplayWorldFrame * DPTR worldFrames;
    /*0x40*/ struct ReplayTracks tracks;
}; /* size = 0x50 */

struct PlayerRecording
{
    /*0x00*/ s32 replayId;
    /*0x04*/ s16 recording;
    /*0x06*/ u16 flags;
}; /* size = 8 */

extern OSHeapHandle replay_heap;
extern s32 active_monkey_id[];
extern void *g_some_buffer_ptr;
extern struct { s8 isStoryMode; } stageselect_is_storymode;

long OSCheckHeap(OSHeapHandle heap);
void *OSAllocFromHeap(OSHeapHandle heap, u32 size);
void OSFreeToHeap(OSHeapHandle heap, void *ptr);
void OSPanic(char *file, int line, char *msg, ...);
int get_seesaw_replay_state_size(struct Seesaw *seesaw);
#ifdef __MWERKS__
char *strncpy(char *dst, const char *src, unsigned long n);
#endif
void g_advance_replay2(float time, struct ReplayBallFrame *frame);

typedef struct DVDDir
{
    u32 entryNum;
    u32 location;
    u32 next;
} DVDDir;

typedef struct DVDDirEntry
{
    u32 entryNum;
    BOOL isDir;
    char *name;
} DVDDirEntry;

struct ReplayItemGroupFrame
{
    /*0x00*/ u32 playbackState;
    /*0x04*/ float time;
};

struct BundledReplay
{
    /*0x00*/ s32 entryNum;
    /*0x04*/ s16 age;
    /*0x06*/ u16 attr;
}; /* size = 8 */

struct ReplayScoreState
{
    /*0x00*/ float score;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec vel;
    /*0x1C*/ s32 bounces;
}; /* size = 0x20 */

extern u8 STAGE_WORLD_THEMES[];
extern OSHeapHandle currentHeap;
BOOL DVDOpenDir(char *path, DVDDir *dir);
BOOL DVDReadDir(DVDDir *dir, DVDDirEntry *entry);
s32 DVDConvertPathToEntrynum(char *path);
BOOL fn_80018FF4(DVDDir *dir);
BOOL g_something_with_dvd(s32 entryNum, struct File *file);
void g_something_with_replays(int id, void *buf);
int g_LoadRecFile(int bundledId, int id);
void fn_1_58768(struct Seesaw *seesaw, void *out);
void fn_1_658BC(float time, struct ReplayWorldFrame *out);
void fn_1_65D60(float time, int itemGroupId, struct ReplayItemGroupFrame *out);
void fn_1_65FD0(int frame, int itemGroupId, void *out);
float fn_1_65BC4(float time);
void fn_1_65CEC(void *header);
float fn_1_65CA8(float time);

void fn_1_5FE5C(int id);
void fn_1_63450(struct Replay *replay);
void g_something_with_recplay3(int param_1);
int g_smth_with_replay_playback(int id);
BOOL fn_1_60E08(struct Replay *dst, struct Replay *src);
struct Replay *fn_1_60F7C(int frames);
void fn_1_610C4(struct Replay *replay);
struct Replay *fn_1_6112C(u8 *buf, int frames);
BOOL fn_1_612A0(struct Replay *replay, u8 *data, u32 trackDataSize, int frames);
BOOL is_itemgroup_playable(int itemGroupIdx);
int get_playable_itemgroup_count(void);
int compute_playable_itemgroup_count(void);
void fn_1_63234(struct Replay *replay);
void g_something_with_replays4(struct Replay *replay);
float g_something_with_replays6(struct Replay *replay);
void g_something_with_bundled_replays(void);
void fn_1_63BC4(void);
int g_something_with_replays2();
int fn_1_631BC(void);
void fn_1_6306C(void);
void fn_1_62C04(int player);
float fn_1_6305C(void);
void fn_1_62B8C(void);
void fn_1_62E20(void);
void g_something_with_stage_world_themes(struct Replay *replay);
void fn_1_63F8C(void);
void fn_1_63FA0(void);
void fn_1_63FA4(void);
void fn_1_63FA8(void);
void fn_1_6467C(void);
void fn_1_646A0(void);
void fn_1_64A7C(int param_1);
void fn_1_64AE8(void);
void fn_1_64BA8(void);
void fn_1_64C1C(void);
void fn_1_64CC0(char *name);
void fn_1_64D0C(void);
void fn_1_64D3C(void);
float fn_1_65010(void);
void fn_1_67B68(int param_1);

static inline int replay_track_count(u16 *count)
{
    return *count;
}

struct { s16 stageId; s32 count; } g_cached_button_itemgroup_count_stage = { -1 };

static s32 lbl_1_bss_71900;
static struct Replay *g_some_replay_data4[REPLAY_SLOT_COUNT];
static struct PlayerRecording lbl_1_bss_71948[4];
static s32 lbl_1_bss_71968;
static s32 lbl_1_bss_7196C;
static struct ReplayScoreState lbl_1_bss_71970;
static u8 lbl_1_bss_71990[0x2A];
static s16 lbl_1_bss_719BA;
static struct BundledReplay lbl_1_bss_719BC[0xA0];
static s16 lbl_1_bss_71EBC;
static s16 lbl_1_bss_71EBE;
static u8 lbl_1_bss_71EC0[8];
struct ReplayInfo lbl_1_bss_71EC8;

FORCE_BSS_ORDER(lbl_1_bss_71900)
FORCE_BSS_ORDER(g_some_replay_data4)
FORCE_BSS_ORDER(lbl_1_bss_71948)
FORCE_BSS_ORDER(lbl_1_bss_71968)
FORCE_BSS_ORDER(lbl_1_bss_7196C)
FORCE_BSS_ORDER(lbl_1_bss_71970)
FORCE_BSS_ORDER(lbl_1_bss_71990)
FORCE_BSS_ORDER(lbl_1_bss_719BA)
FORCE_BSS_ORDER(lbl_1_bss_719BC)
FORCE_BSS_ORDER(lbl_1_bss_71EBC)
FORCE_BSS_ORDER(lbl_1_bss_71EBE)
FORCE_BSS_ORDER(lbl_1_bss_71EC0)
FORCE_BSS_ORDER(lbl_1_bss_71EC8)

void g_something_with_replays5(void)
{
    u32 i;
    struct PlayerRecording *rec;

    memset(g_some_replay_data4, 0, sizeof(g_some_replay_data4));
    g_something_with_bundled_replays();
    fn_1_6306C();
    rec = lbl_1_bss_71948;
    for (i = 4; i != 0; i--, rec++)
    {
        rec->replayId = -1;
        rec->recording = 0;
    }
    fn_1_63F8C();
}

int fn_1_5FC54(void)
{
    int id;

    if (replay_heap < 0 || OSCheckHeap(replay_heap) < 0)
        return 0;
    if (g_some_replay_data4[12] != NULL)
        fn_1_5FE5C(12);
    fn_1_63BC4();
    if (g_something_with_replays2() == 0)
        return 0;
    id = fn_1_631BC();
    if (id >= 0)
    {
        g_some_replay_data4[12] = g_some_replay_data4[id];
        g_some_replay_data4[id] = NULL;
    }
    return 1;
}

void fn_1_5FD20(void)
{
    int i;
    struct Replay **p;
    struct Replay *replay;

    p = g_some_replay_data4;
    for (i = REPLAY_SLOT_COUNT; i > 0; i--, p++)
    {
        replay = *p;
        if (replay != NULL)
        {
            if (replay->header.flags & 0x8000)
                replay->header.flags &= ~0x8000;
            if (!(dip_switches & 1))
                replay->header.grade *= 0.92f;
        }
    }
    fn_1_63FA0();
}

void fn_1_5FDC0(void)
{
    int i;
    struct Replay **p;

    p = g_some_replay_data4;
    for (i = REPLAY_SLOT_COUNT; i > 0; i--, p++)
    {
        if (*p != NULL)
        {
            g_something_with_replays4(*p);
            fn_1_610C4(*p);
            *p = NULL;
        }
    }
    fn_1_67B68(0);
}

void fn_1_5FE3C(int param_1)
{
    fn_1_67B68(param_1);
}

void fn_1_5FE5C(int id)
{
    if (id == REPLAY_SLOT_CMP)
        fn_1_67B68(0);
    else
    {
        struct Replay **p = &g_some_replay_data4[id];

        if (*p != NULL)
        {
            g_something_with_replays4(*p);
            fn_1_610C4(*p);
            *p = NULL;
        }
    }
}

void event_recplay_init(void)
{
    lbl_1_bss_71968 = 0;
    fn_1_63FA4();
}

void event_recplay_tick(void)
{
    struct Ball *ball;
    struct World *world;
    struct Replay *replay;
    int pos;
    int i;
    struct PlayerRecording *rec;

    if (g_some_other_flags & 0xA)
        return;
    if (lbl_1_bss_71968 > 0)
    {
        lbl_1_bss_71968--;
        if (lbl_1_bss_71968 == 0)
            g_something_with_recplay3(lbl_1_bss_7196C);
    }
    rec = lbl_1_bss_71948;
    ball = balls;
    world = world_infos;
    for (i = 4; i > 0; i--, rec++, ball++, world++)
    {
        if (rec->recording != 0)
        {
            replay = g_some_replay_data4[rec->replayId];
            if (replay != NULL)
            {
                struct ReplayBallFrame *frame;
                struct ReplayWorldFrame *wframe;
                u32 max;

                pos = replay->writePos;
                frame = &replay->ballFrames[pos];
                frame->pos = ball->pos;
                frame->rotX = ball->rotX;
                frame->rotY = ball->rotY;
                frame->rotZ = ball->rotZ;
                frame->unk12 = 32767.0f * ball->unk11C.x;
                frame->unk14 = 32767.0f * ball->unk11C.y;
                frame->unk16 = 32767.0f * ball->unk11C.z;
                frame->flags = ball->flags;
                frame->unk1C = ball->unk13C;
                if (rec->flags & 1)
                {
                    frame->flags |= 0x10000;
                    rec->flags &= ~1;
                }
                wframe = &replay->worldFrames[pos];
                wframe->rotX = world->xrot;
                wframe->rotZ = world->zrot;
                replay->frameCount++;
                fn_1_63234(replay);
                max = replay->maxFrames;
                pos++;
                if (pos >= max)
                    pos -= max;
                replay->writePos = pos;
                if (replay->storedFrames < max)
                    replay->storedFrames++;
                fn_1_62E20();
            }
        }
    }
    if (modeCtrl.gameType == 0 || modeCtrl.gameType == 2 || modeCtrl.gameType == 0xE)
        fn_1_63FA8();
}

void event_recplay_dest(void)
{
    int i;
    struct PlayerRecording *rec;

    if (lbl_1_bss_71968 > 0)
    {
        g_something_with_recplay3(lbl_1_bss_7196C);
        lbl_1_bss_71968 = 0;
    }
    fn_1_6467C();
    rec = lbl_1_bss_71948;
    for (i = 4; i > 0; i--, rec++)
    {
        rec->replayId = -1;
        rec->recording = 0;
    }
}

int fn_1_601D8(void)
{
    int i;
    struct Replay **p;
    int id;
    float best;

    p = g_some_replay_data4;
    id = -1;
    best = 20000.0f;
    for (i = REPLAY_EVICT_SLOTS; i > 0; i--, p++)
    {
        struct Replay *replay = *p;
        float score;

        if (replay == NULL)
        {
            id = REPLAY_EVICT_SLOTS - i;
            break;
        }
        if (replay->header.grade == 0.0)
        {
            id = REPLAY_EVICT_SLOTS - i;
            break;
        }
        score = replay->header.grade;
        score *= g_something_with_replays6(replay);
        if (id < 0 || score < best)
        {
            id = REPLAY_EVICT_SLOTS - i;
            best = score;
        }
    }
    return id;
}

void fn_1_602BC(void)
{
    int i;
    struct PlayerRecording *rec;

    g_something_with_recplay3(0);
    rec = lbl_1_bss_71948;
    for (i = 4; i > 0; i--, rec++)
        rec->replayId = -1;
}

void fn_1_6030C(int player, int replayId)
{
    lbl_1_bss_71948[player].replayId = replayId;
}

/* Whether the replay is the one being recorded by some player. */
static inline BOOL is_replay_recording(int id)
{
    int i;
    struct PlayerRecording *rec;

    rec = lbl_1_bss_71948;
    for (i = 4; i > 0; i--, rec++)
    {
        if (rec->replayId == id)
            return TRUE;
    }
    return FALSE;
}

/* The worst idle replay: an ungraded one if any, else the lowest score. */
static inline int pick_replay_to_evict(void)
{
    int i;
    struct Replay **p;
    int id;
    float best;

    p = g_some_replay_data4;
    id = -1;
    best = 20000.0f;
    for (i = 0; i < REPLAY_SLOT_COUNT; i++, p++)
    {
        struct Replay *replay = *p;
        float score;
        int n;
        struct PlayerRecording *rec;

        if (replay != NULL)
        {
            rec = lbl_1_bss_71948;
            for (n = 4; n > 0; n--, rec++)
            {
                if (rec->replayId == i)
                    break;
            }
            if (n > 0)
                continue;
            if (replay->header.grade == 0.0)
            {
                id = i;
                break;
            }
            score = replay->header.grade;
            score *= g_something_with_replays6(replay);
            if (id < 0 || score < best)
            {
                id = i;
                best = score;
            }
        }
    }
    return id;
}

static inline BOOL evict_replay(void)
{
    int id = pick_replay_to_evict();

    if (id < 0)
        return FALSE;
    fn_1_5FE5C(id);
    return TRUE;
}

void g_something_with_recplay2(void)
{
    int i;
    struct Replay *replay;
    struct PlayerRecording *rec;
    u32 flags;
    int difficulty;
    int floor;

    flags = 0x8000;
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
    rec = lbl_1_bss_71948;
    for (i = 4; i > 0; i--, rec++)
    {
        if (rec->replayId >= 0)
        {
            replay = g_some_replay_data4[rec->replayId];
            if (replay != NULL)
                fn_1_5FE5C(rec->replayId);
            replay = fn_1_60F7C(REPLAY_MAX_FRAMES);
            while (replay == NULL)
            {
                int id = pick_replay_to_evict();

                if (id < 0)
                    OSPanic("recplay.c", 0x328, "Can't alloc the memory for RECPLAY!\n");
                else
                {
                    fn_1_5FE5C(id);
                    replay = fn_1_60F7C(REPLAY_MAX_FRAMES);
                }
            }
            g_some_replay_data4[rec->replayId] = replay;
            replay->writePos = 0;
            replay->storedFrames = 0;
            replay->timerStart = mode_info.timerCurr;
            replay->frameCount = 0;
            replay->header.flags = flags;
            replay->header.stageId = current_stage_id;
            replay->header.difficulty = difficulty;
            replay->header.floor = floor;
            replay->header.monkey = active_monkey_id[4 - i];
            replay->header.name[0] = 0;
            g_something_with_stage_world_themes(replay);
            rec->recording = 1;
            fn_1_62B8C();
            fn_1_63234(replay);
        }
    }
    if (modeCtrl.gameType == 0 || modeCtrl.gameType == 2 || modeCtrl.gameType == 0xE)
        fn_1_646A0();
}

/* Bytes needed for the item group and seesaw tracks of a replay. */
static inline int track_data_size(int frames)
{
    int total = 0;
    int count = get_playable_itemgroup_count();
    int size = sizeof(struct ReplayItemGroupTrack);

    total += ALIGN4(size * count);
    size = ALIGN4(frames);
    total += ALIGN4(size * count);
    count = seesaw_count;
    if (count != 0)
    {
        int i;
        struct ItemGroup *ig;
        struct StageCollHeader *coli;

        size = sizeof(struct ReplaySeesawTrack);
        total += ALIGN4(size * count);
        coli = stagedef->collHeaders;
        ig = itemgroups;
        for (i = stagedef->itemGroupCount; i > 0; i--, ig++, coli++)
        {
            struct Seesaw *seesaw = ig->seesaw;

            if (seesaw != NULL)
            {
                size = frames * get_seesaw_replay_state_size(seesaw);
                size = ALIGN4(size);
                total += size;
            }
        }
    }
    return total;
}

static inline u32 replay_alloc_size(int frames)
{
    int worldSize;
    int ballSize;
    int headerSize = sizeof(struct Replay);

    ballSize = ALIGN4(frames * sizeof(struct ReplayBallFrame));
    worldSize = ALIGN4(frames * sizeof(struct ReplayWorldFrame));
    return headerSize + ballSize + worldSize + track_data_size(frames);
}

void g_something_with_recplay3(int param_1)
{
    int i;
    struct PlayerRecording *rec;

    lbl_1_bss_71968 = 0;
    rec = lbl_1_bss_71948;
    for (i = 4; i > 0; i--, rec++)
    {
        if (rec->replayId >= 0)
        {
            if (rec->recording == 1)
            {
                struct Replay *replay = g_some_replay_data4[rec->replayId];

                if (replay == NULL)
                    continue;
                if (param_1 != 0)
                {
                    if (replay->frameCount < replay->maxFrames)
                        replay->header.flags |= 0x10;
                    else
                        replay->header.flags &= ~0x10;
                    fn_1_62C04(4 - i);
                    replay->header.grade = fn_1_6305C();
                    g_smth_with_replay_playback(rec->replayId);
                }
            }
            rec->recording = 0;
        }
    }
    if (modeCtrl.gameType == 0 || modeCtrl.gameType == 2 || modeCtrl.gameType == 0xE)
        fn_1_64A7C(param_1);
}

void fn_1_6094C(int param_1, int param_2)
{
    lbl_1_bss_7196C = param_1;
    lbl_1_bss_71968 = param_2;
    if (param_2 == 0)
        g_something_with_recplay3(lbl_1_bss_7196C);
}

int g_smth_with_replay_playback(int id)
{
    struct Replay *replay = g_some_replay_data4[id];
    struct Replay *dst;
    BOOL heap;
    u32 size;

    if (replay == NULL)
        return 0;
    if (replay->storedFrames >= replay->maxFrames)
        return 0;
    size = replay_alloc_size(replay->storedFrames);
    if (0x10000 > size)
    {
        heap = FALSE;
        dst = fn_1_6112C((u8 *)g_some_buffer_ptr, replay->storedFrames);
    }
    else
    {
        heap = TRUE;
        dst = fn_1_60F7C(replay->storedFrames);
        if (dst == NULL)
            return 0;
    }
    fn_1_60E08(dst, replay);
    fn_1_610C4(replay);
    if (heap)
        g_some_replay_data4[id] = dst;
    else
    {
        replay = fn_1_60F7C(dst->storedFrames);
        while (replay == NULL)
        {
            if (!evict_replay())
                break;
            replay = fn_1_60F7C(dst->storedFrames);
        }
        if (replay == NULL)
            OSPanic("recplay.c", 0x464, "Fatal Error");
        fn_1_60E08(replay, dst);
        g_some_replay_data4[id] = replay;
    }
    return 1;
}

BOOL fn_1_60E08(struct Replay *dst, struct Replay *src)
{
    int i;
    u32 count;
    struct Replay saved;
    struct ReplayItemGroupTrack *dstIg;
    struct ReplayItemGroupTrack *srcIg;
    struct ReplaySeesawTrack *dstSs;
    struct ReplaySeesawTrack *srcSs;

    if (src->storedFrames >= src->maxFrames)
        count = src->maxFrames;
    else
        count = src->writePos;
    memcpy_handler(&saved, dst, sizeof(saved));
    memcpy_handler(dst, src, sizeof(saved));
    dst->maxFrames = saved.maxFrames;
    dst->ballFrames = saved.ballFrames;
    dst->worldFrames = saved.worldFrames;
    dst->tracks.itemGroups = saved.tracks.itemGroups;
    dst->tracks.seesaws = saved.tracks.seesaws;
    memcpy_handler(dst->ballFrames, src->ballFrames, count << 5);
    memcpy_handler(dst->worldFrames, src->worldFrames, count << 2);
    dstIg = dst->tracks.itemGroups;
    srcIg = src->tracks.itemGroups;
    for (i = dst->tracks.itemGroupCount; i > 0; i--, dstIg++, srcIg++)
    {
        dstIg->itemGroupId = srcIg->itemGroupId;
        dstIg->unk4 = srcIg->unk4;
        memcpy_handler(dstIg->frames, srcIg->frames, count);
    }
    dstSs = dst->tracks.seesaws;
    srcSs = src->tracks.seesaws;
    for (i = dst->tracks.seesawCount; i > 0; i--, dstSs++, srcSs++)
    {
        dstSs->itemGroupId = srcSs->itemGroupId;
        dstSs->stateSize = srcSs->stateSize;
        memcpy_handler(dstSs->frames, srcSs->frames, dstSs->stateSize * count);
    }
    count = dst->maxFrames;
    dst->storedFrames = count;
    while (dst->writePos >= count)
        dst->writePos -= count;
    return TRUE;
}

struct Replay *fn_1_60F7C(int frames)
{
    u32 size = replay_alloc_size(frames);
    void *buf = OSAllocFromHeap(replay_heap, size + 0x20);

    if (buf == NULL)
        return NULL;
    return fn_1_6112C((u8 *)buf, frames);
}

void fn_1_610C4(struct Replay *replay)
{
    if (replay_heap >= 0 && OSCheckHeap(replay_heap) >= 0)
        OSFreeToHeap(replay_heap, replay);
}

struct Replay *fn_1_6112C(u8 *buf, int frames)
{
    int headerSize = sizeof(struct Replay);
    int temp;
    int ballSize;
    int worldSize;
    struct Replay *replay;
    u32 size;
    int trackSize;

    temp = ALIGN4(frames * sizeof(struct ReplayBallFrame));
    ballSize = temp;
    temp = ALIGN4(frames * sizeof(struct ReplayWorldFrame));
    worldSize = temp;
    trackSize = track_data_size(frames);
    size = headerSize + ballSize + worldSize + trackSize;
    memset(buf, 0, size);
    replay = (struct Replay *)buf;
    buf += headerSize;
    replay->size = size;
    replay->maxFrames = frames;
    replay->ballFrames = (struct ReplayBallFrame *)buf;
    buf += ballSize;
    replay->worldFrames = (struct ReplayWorldFrame *)buf;
    buf += worldSize;
    fn_1_612A0(replay, buf, trackSize, frames);
    return replay;
}

BOOL fn_1_612A0(struct Replay *replay, u8 *data, u32 trackDataSize, int frames)
{
    int i;
    struct ReplayTracks *tracks;
    int count;
    int size;
    struct ReplayItemGroupTrack *igTrack;
    struct ReplaySeesawTrack *ssTrack;
    struct StageCollHeader *coli;
    struct ItemGroup *ig;

    if (trackDataSize == 0)
        return TRUE;
    tracks = &replay->tracks;
    count = get_playable_itemgroup_count();
    tracks->itemGroupCount = count;
    if (count == 0)
        tracks->itemGroups = NULL;
    else
    {
        size = sizeof(struct ReplayItemGroupTrack);
        igTrack = (struct ReplayItemGroupTrack *)data;
        tracks->itemGroups = igTrack;
        data += ALIGN4(size * count);
        size = frames;
        for (i = 0; i < stagedef->itemGroupCount; i++)
        {
            if (is_itemgroup_playable(i + 1))
            {
                igTrack->itemGroupId = i + 1;
                igTrack->frames = data;
                data += ALIGN4(size);
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
        size = sizeof(struct ReplaySeesawTrack);
        ssTrack = (struct ReplaySeesawTrack *)data;
        tracks->seesaws = ssTrack;
        data += ALIGN4(size * count);
        coli = stagedef->collHeaders;
        ig = itemgroups;
        for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coli++)
        {
            struct Seesaw *seesaw = ig->seesaw;

            if (seesaw != NULL)
            {
                u32 stateSize;

                ssTrack->itemGroupId = i;
                stateSize = get_seesaw_replay_state_size(seesaw);
                ssTrack->stateSize = stateSize;
                if (stateSize == 0)
                    ssTrack->frames = NULL;
                else
                {
                    ssTrack->frames = data;
                    size = stateSize * frames;
                    data += ALIGN4(size);
                }
                ssTrack++;
            }
        }
    }
    return TRUE;
}

struct StageButton
{
    u8 unk0[0x14];
    s16 animGroupId;
    u8 unk16[2];
}; /* size = 0x18 */

BOOL is_itemgroup_playable(int itemGroupIdx)
{
    struct StageCollHeader *coli;
    int animGroupId;
    BOOL playable;
    int i;
    struct StageButton *button;

    if (itemGroupIdx == 0)
        return FALSE;
    coli = &stagedef->collHeaders[itemGroupIdx];
    playable = FALSE;
    if (coli->anim == NULL)
        return FALSE;
    animGroupId = coli->animGroupId;
    button = stagedef->buttons;
    for (i = stagedef->buttonCount; i > 0; i--, button++)
    {
        if (button->animGroupId == animGroupId)
            playable = TRUE;
    }
    return playable;
}

int get_playable_itemgroup_count(void)
{
    if (g_cached_button_itemgroup_count_stage.stageId != current_stage_id)
    {
        g_cached_button_itemgroup_count_stage.count = compute_playable_itemgroup_count();
        g_cached_button_itemgroup_count_stage.stageId = current_stage_id;
    }
    return g_cached_button_itemgroup_count_stage.count;
}

int compute_playable_itemgroup_count(void)
{
    int count;
    int i;
    int groupCount;

    count = 0;
    groupCount = stagedef->itemGroupCount;
    for (i = 0; i < groupCount; i++)
    {
        if (is_itemgroup_playable(i))
            count++;
    }
    return count;
}

void fn_1_6160C(int player)
{
    struct PlayerRecording *rec = &lbl_1_bss_71948[player];

    if (rec->recording == 1)
    {
        struct Replay *replay = g_some_replay_data4[rec->replayId];

        replay->header.flags |= 1;
        replay->timerEnd = mode_info.unk1C;
        replay->goalVel = mode_info.unk10;
        replay->goalEntered = mode_info.goalEntered;
        replay->goalItemGroup = mode_info.goalItemGroup;
        fn_1_64AE8();
    }
}

void fn_1_616C0(int player)
{
    struct PlayerRecording *rec = &lbl_1_bss_71948[player];

    if (rec->recording == 1)
    {
        struct Replay *replay = g_some_replay_data4[rec->replayId];

        replay->header.flags |= 2;
        replay->timerEnd = mode_info.timerCurr;
        fn_1_64BA8();
    }
}

void fn_1_61734(int player)
{
    struct PlayerRecording *rec = &lbl_1_bss_71948[player];

    if (rec->recording == 1)
    {
        struct Replay *replay = g_some_replay_data4[rec->replayId];

        replay->header.flags |= 4;
        fn_1_64C1C();
    }
}

static char lbl_1_data_2F3E8[] = "#$%@^";
static char lbl_1_data_2F3F0[] = "";

void fn_1_61798(char *name)
{
    int i;
    struct Replay **p;
    struct Replay *replay;
    BOOL bad;
    s8 *c;
    u8 *n;

    bad = FALSE;
    n = (u8 *)name;
    for (; *n != 0; n++)
    {
        for (c = (s8 *)lbl_1_data_2F3E8; *c != 0; c++)
        {
            if (*c == *n)
            {
                bad = TRUE;
                name = lbl_1_data_2F3F0;
                break;
            }
        }
        if (bad)
            break;
    }
    p = g_some_replay_data4;
    for (i = REPLAY_SLOT_COUNT; i > 0; i--, p++)
    {
        replay = *p;
        if (replay != NULL && (replay->header.flags & 0x8000))
        {
            strncpy(replay->header.name, name, 4);
            replay->header.name[3] = 0;
        }
    }
    fn_1_64CC0(name);
}

void g_smth_with_wormhole_replays(int player)
{
    struct PlayerRecording *rec = &lbl_1_bss_71948[player];

    if (rec->recording == 1)
    {
        rec->flags |= 1;
        fn_1_64D0C();
    }
}

void fn_1_618F4(int id)
{
    struct Replay *replay;

    if (id == REPLAY_SLOT_CMP)
    {
        fn_1_64D3C();
        return;
    }
    replay = g_some_replay_data4[id];
    mode_info.timerMax = replay->timerStart;
    mode_info.timerCurr = replay->timerStart;
    mode_info.flags &= ~0x667;
    if (replay->header.flags & 1)
    {
        mode_info.flags |= 0x28;
        mode_info.timerCurr = replay->timerEnd;
        mode_info.unk1C = replay->timerEnd;
        mode_info.unk10 = replay->goalVel;
        mode_info.goalEntered = replay->goalEntered;
        mode_info.goalItemGroup = replay->goalItemGroup;
    }
    if (replay->header.flags & 8)
        mode_info.flags |= 0x40;
    active_monkey_id[0] = replay->header.monkey;
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

float fn_1_61B18(int id)
{
    struct Replay *replay;
    int n;

    if (id == REPLAY_SLOT_CMP)
        return fn_1_65010();
    replay = g_some_replay_data4[id];
    if (replay->storedFrames != 0)
        n = replay->storedFrames - 1;
    else
        n = 0;
    return n;
}

void g_get_replay_frame_data(int id, float time, struct ReplayBallFrame *frame)
{
    struct Replay *replay;
    u32 maxFrames;
    int pos;
    struct Ball *ball = current_ball;

    if (id == REPLAY_SLOT_CMP)
    {
        g_advance_replay2(time, frame);
        return;
    }
    replay = g_some_replay_data4[id];
    if (replay->storedFrames == 0)
    {
        frame->pos = ball->pos;
        frame->rotX = ball->rotX;
        frame->rotY = ball->rotY;
        frame->rotZ = ball->rotZ;
        return;
    }
    maxFrames = replay->maxFrames;
    if (time < 0.0)
    {
        pos = replay->writePos - 1;
        if (pos < 0)
            pos += maxFrames;
        *frame = replay->ballFrames[pos];
    }
    else if (fn_1_61B18(id) <= time)
    {
        pos = replay->writePos - replay->storedFrames;
        while (pos < 0)
            pos += maxFrames;
        *frame = replay->ballFrames[pos];
    }
    else
    {
        int whole = time;
        float t = time - whole;
        float u = 1.0 - t;
        int pos2;
        struct ReplayBallFrame a;
        struct ReplayBallFrame b;
        struct ReplayBallFrame out;
        Vec normal;

        pos = replay->writePos - 1 - whole;
        if (pos < 0)
            pos += maxFrames;
        pos2 = pos - 1;
        if (pos2 < 0)
            pos2 += maxFrames;
        a = replay->ballFrames[pos];
        b = replay->ballFrames[pos2];
        out.pos.x = a.pos.x * u + b.pos.x * t;
        out.pos.y = a.pos.y * u + b.pos.y * t;
        out.pos.z = a.pos.z * u + b.pos.z * t;
        out.rotX = a.rotX * u + b.rotX * t;
        out.rotY = a.rotY * u + b.rotY * t;
        out.rotZ = a.rotZ * u + b.rotZ * t;
        normal.x = a.unk12 * u + b.unk12 * t;
        normal.y = a.unk14 * u + b.unk14 * t;
        normal.z = a.unk16 * u + b.unk16 * t;
        vec_set_len(32767.0f, &normal, &normal);
        out.unk12 = normal.x;
        out.unk14 = normal.y;
        out.unk16 = normal.z;
        out.flags = a.flags;
        out.unk1C = a.unk1C * u + b.unk1C * t;
        *frame = out;
    }
}

void fn_1_621B4(int id, float time, struct ReplayWorldFrame *out)
{
    struct World *world = current_world_info;
    struct Replay *replay;
    u32 maxFrames;
    int pos;

    if (id == REPLAY_SLOT_CMP)
    {
        fn_1_658BC(time, out);
        return;
    }
    replay = g_some_replay_data4[id];
    if (replay->storedFrames == 0)
    {
        out->rotX = world->xrot;
        out->rotZ = world->zrot;
        return;
    }
    maxFrames = replay->maxFrames;
    if (time < 0.0)
    {
        pos = replay->writePos - 1;
        if (pos < 0)
            pos += maxFrames;
        *out = replay->worldFrames[pos];
    }
    else if (fn_1_61B18(id) <= time)
    {
        pos = replay->writePos - replay->storedFrames;
        while (pos < 0)
            pos += maxFrames;
        *out = replay->worldFrames[pos];
    }
    else
    {
        int whole = time;
        float t = time - whole;
        int pos2;
        float inv = 1.0 - t;
        struct ReplayWorldFrame a;
        struct ReplayWorldFrame b;
        struct ReplayWorldFrame result;

        pos = replay->writePos - 1 - whole;
        if (pos < 0)
            pos += maxFrames;
        pos2 = pos - 1;
        if (pos2 < 0)
            pos2 += maxFrames;
        a = replay->worldFrames[pos];
        b = replay->worldFrames[pos2];
        result.rotX = a.rotX * inv + b.rotX * t;
        result.rotZ = a.rotZ * inv + b.rotZ * t;
        *out = result;
    }
}

void fn_1_624A8(int id, float time, int itemGroupId, struct ReplayItemGroupFrame *out)
{
    struct Replay *replay;
    struct ReplayItemGroupTrack *track;
    u32 maxFrames;
    int pos;
    int cur;
    int frame;
    int i;
    BOOL found;
    u8 *p;

    if (id == REPLAY_SLOT_CMP)
    {
        fn_1_65D60(time, itemGroupId, out);
        return;
    }
    replay = g_some_replay_data4[id];
    if (itemGroupId == 0)
    {
        out->playbackState = 0;
        out->time = 0.0f;
        return;
    }
    if (replay->tracks.itemGroups == NULL)
    {
        out->playbackState = 0;
        out->time = replay->tracks.startTimer - time;
        return;
    }
    found = FALSE;
    track = replay->tracks.itemGroups;
    for (i = replay->tracks.itemGroupCount; i > 0; i--, track++)
    {
        if (track->itemGroupId == itemGroupId)
        {
            found = TRUE;
            break;
        }
        if (track->itemGroupId > itemGroupId)
            break;
    }
    if (!found)
    {
        out->playbackState = 0;
        out->time = replay->tracks.startTimer - time;
        return;
    }
    maxFrames = replay->maxFrames;
    if (time < 0.0)
    {
        pos = replay->writePos - 1;
        if (pos < 0)
            pos += maxFrames;
    }
    else if (fn_1_61B18(id) <= time)
    {
        pos = replay->writePos - replay->storedFrames;
        while (pos < 0)
            pos += maxFrames;
    }
    else
    {
        pos = replay->writePos - 1 - (int)time;
        while (pos < 0)
            pos += maxFrames;
    }
    cur = replay->writePos - 2;
    if (cur < 0)
        cur += replay->maxFrames;
    frame = track->unk4;
    p = &track->frames[cur];
    while (cur != pos)
    {
        switch (*p)
        {
        case 0:
            frame--;
            break;
        case 2:
            frame++;
            break;
        case 3:
            frame -= 2;
            break;
        case 4:
            frame += 2;
            break;
        }
        if (cur > 0)
        {
            cur--;
            p--;
        }
        else
        {
            cur = maxFrames - 1;
            p = (u8 *)track->frames + (maxFrames - 1);
        }
    }
    out->playbackState = *p;
    out->time = frame - (float)(time - (int)time);
}

void g_smth_with_seesaws_and_replays(int id, int frame, int itemGroupId, void *out)
{
    struct Replay *replay;
    struct ReplaySeesawTrack *track;
    int i;
    u32 maxFrames;
    int pos;

    if (id == REPLAY_SLOT_CMP)
    {
        fn_1_65FD0(frame, itemGroupId, out);
        return;
    }
    replay = g_some_replay_data4[id];
    if (replay->tracks.seesaws == NULL)
        return;
    track = replay->tracks.seesaws;
    for (i = replay_track_count(&replay->tracks.seesawCount); i > 0; i--, track++)
    {
        if (track->itemGroupId == itemGroupId)
            break;
    }
    if (i == 0)
        return;
    maxFrames = replay->maxFrames;
    if (frame < 0)
    {
        pos = replay->writePos - 1;
        if (pos < 0)
            pos += maxFrames;
    }
    else if (fn_1_61B18(id) <= frame)
    {
        pos = replay->writePos - replay->storedFrames;
        while (pos < 0)
            pos += maxFrames;
    }
    else
    {
        pos = replay->writePos - 1 - frame;
        while (pos < 0)
            pos += maxFrames;
    }
    memcpy_handler(out, (u32 *)track->frames + pos * (track->stateSize >> 2), track->stateSize);
}

float fn_1_629C4(int id, float time)
{
    struct Replay *replay;
    float t;

    if (id == REPLAY_SLOT_CMP)
        return fn_1_65BC4(time);
    replay = g_some_replay_data4[id];
    t = time + (replay->timerStart - replay->frameCount);
    if ((replay->header.flags & 1) && t < replay->timerEnd)
        t = replay->timerEnd;
    return t;
}

void fn_1_62AA0(int id, void *header)
{
    if (id == REPLAY_SLOT_CMP)
        fn_1_65CEC(header);
    else
    {
        struct Replay *replay = g_some_replay_data4[id];

        if (replay != NULL)
            memcpy_handler(header, replay, sizeof(struct ReplayHeader));
        else
        {
            memset(header, 0, sizeof(struct ReplayHeader));
            ((struct ReplayHeader *)header)->flags = 0x200;
        }
    }
}

float fn_1_62B20(int id, float time)
{
    if (id == REPLAY_SLOT_CMP)
        return fn_1_65CA8(time);
    return g_some_replay_data4[id]->tracks.startTimer - time;
}

void fn_1_62B8C(void)
{
    struct Ball *ball = current_ball;

    lbl_1_bss_71970.score = 0.0f;
    lbl_1_bss_71970.pos = ball->pos;
    lbl_1_bss_71970.vel = ball->vel;
    lbl_1_bss_71970.bounces = 0;
}

void fn_1_62C04(int player)
{
    struct PlayerRecording *rec = &lbl_1_bss_71948[player];
    struct Replay *replay = g_some_replay_data4[rec->replayId];
    float ratio;
    float score = lbl_1_bss_71970.score;

    ratio = (float)lbl_1_bss_71970.bounces / replay->frameCount;
    score *= 1.0 / (float)(replay->frameCount + 1);

    if (replay->header.flags & 1)
    {
        score *= 2.0;
        score *= 1.0 + (float)(replay->timerStart - replay->timerEnd) / replay->timerStart;
        if (replay->header.flags & 0x10)
            score *= 1.0 + (replay->frameCount - 360.0) / 720.0;
        score *= 1.0 + 0.5 * (1.0 - ratio);
        score *= 1.0 + 0.25 * (0.02 * replay->header.floor);
    }
    if (replay->header.flags & 2)
    {
        score *= 0.25;
        score *= ratio;
    }
    if (replay->header.flags & 4)
        score *= 0.1;
    lbl_1_bss_71970.score = score;
}

void fn_1_62E20(void)
{
    struct Ball *ball = current_ball;
    Vec pos;
    Vec delta;
    float inc;
    float dot;
    float len;
    Vec a;
    Vec b;

    pos = ball->pos;
    delta.x = ball->pos.x - lbl_1_bss_71970.pos.x;
    delta.y = ball->pos.y - lbl_1_bss_71970.pos.y;
    delta.z = ball->pos.z - lbl_1_bss_71970.pos.z;
    inc = 0.0f;
    a = delta;
    len = mathutil_vec_len(&a);
    b = lbl_1_bss_71970.vel;
    inc += 2.0 * (float)(len - mathutil_vec_len(&b));
    dot = a.x * b.x + a.y * b.y + a.z * b.z;
    if (dot > 1.1920929e-7f)
        inc += (double)math_sqrt(dot);
    if (ball->flags & 1)
        lbl_1_bss_71970.bounces++;
    lbl_1_bss_71970.score += inc;
    lbl_1_bss_71970.pos = pos;
    lbl_1_bss_71970.vel = delta;
}

float fn_1_6305C(void)
{
    return lbl_1_bss_71970.score;
}

void fn_1_6306C(void)
{
    int i;
    struct Replay **p;

    memset(lbl_1_bss_71990, 0, 0x2A);
    p = g_some_replay_data4;
    for (i = REPLAY_SLOT_COUNT; i > 0; i--, p++)
    {
        struct Replay *replay = *p;

        if (replay != NULL && replay->header.floor != 0)
            g_something_with_stage_world_themes(replay);
    }
}

void g_something_with_stage_world_themes(struct Replay *replay)
{
    lbl_1_bss_71990[STAGE_WORLD_THEMES[replay->header.stageId]]++;
}

void g_something_with_replays4(struct Replay *replay)
{
    u8 *count = &lbl_1_bss_71990[STAGE_WORLD_THEMES[replay->header.stageId]];

    if (*count != 0)
    {
        u8 n = *count;
    }
}

float g_something_with_replays6(struct Replay *replay)
{
    float count = lbl_1_bss_71990[STAGE_WORLD_THEMES[replay->header.stageId]];

    if (count > 0.0)
        return 1.0 / count;
    return 1.0f;
}

int fn_1_631BC(void)
{
    int id = -1;
    float best = -1.0f;
    int i;
    struct Replay **p = g_some_replay_data4;

    for (i = REPLAY_EVICT_SLOTS; i > 0; i--, p++)
    {
        struct Replay *replay = *p;

        if (replay != NULL && (replay->header.flags & 1) && (replay->header.flags & 0x10)
         && replay->header.grade > best)
        {
            id = REPLAY_EVICT_SLOTS - i;
            best = replay->header.grade;
        }
    }
    return id;
}

void fn_1_63234(struct Replay *replay)
{
    struct ReplayTracks *tracks = &replay->tracks;
    struct ReplayItemGroupTrack *igTrack;
    struct ReplaySeesawTrack *ssTrack;
    int i;
    int pos;

    tracks->startTimer = g_some_frame_counter.frameCounter;
    pos = replay->writePos;
    igTrack = tracks->itemGroups;
    if (igTrack != NULL)
    {
        for (i = replay_track_count(&tracks->itemGroupCount); i > 0; i--, igTrack++)
        {
            struct ItemGroup *ig = &itemgroups[igTrack->itemGroupId];

            igTrack->unk4 = ig->animFrame;
            igTrack->frames[pos] = ig->playbackState;
        }
    }
    ssTrack = tracks->seesaws;
    if (ssTrack != NULL)
    {
        for (i = replay_track_count(&tracks->seesawCount); i > 0; i--, ssTrack++)
            fn_1_58768(itemgroups[ssTrack->itemGroupId].seesaw,
                       (u32 *)ssTrack->frames + (ssTrack->stateSize >> 2) * pos);
    }
}

struct Replay *fn_1_6333C(int id)
{
    return g_some_replay_data4[id];
}

u32 fn_1_63354(struct Replay *replay)
{
    return replay->size;
}

void fn_1_6335C(struct Replay *replay, void *dst)
{
    struct Replay *r = replay;
    int i;
    uptr base = (uptr)replay;
    int j;
    struct ReplayItemGroupTrack *igTrack;
    struct ReplaySeesawTrack *ssTrack;

    r->ballFrames = (struct ReplayBallFrame *)((u8 *)r->ballFrames - (u8 *)base);
    r->worldFrames = (struct ReplayWorldFrame *)((u8 *)r->worldFrames - (u8 *)base);
    if (r->tracks.itemGroups != NULL)
    {
        igTrack = r->tracks.itemGroups;
        for (i = r->tracks.itemGroupCount; i > 0; i--, igTrack++)
            igTrack->frames -= base;
        r->tracks.itemGroups = (struct ReplayItemGroupTrack *)((u8 *)r->tracks.itemGroups - (u8 *)base);
    }
    if (r->tracks.seesaws != NULL)
    {
        ssTrack = r->tracks.seesaws;
        for (j = r->tracks.seesawCount; j > 0; j--, ssTrack++)
        {
            if (ssTrack->frames != NULL)
                ssTrack->frames -= base;
        }
        r->tracks.seesaws = (struct ReplaySeesawTrack *)((u8 *)r->tracks.seesaws - (u8 *)base);
    }
    if (dst != r)
    {
        memcpy_handler(dst, r, r->size);
        fn_1_63450(replay);
    }
}

void fn_1_63450(struct Replay *replay)
{
    int i;
    struct Replay *r = replay;
    uptr base = (uptr)replay;
    struct ReplayItemGroupTrack *igTrack;
    struct ReplaySeesawTrack *ssTrack;

    r->ballFrames = (struct ReplayBallFrame *)((u8 *)r->ballFrames + base);
    r->worldFrames = (struct ReplayWorldFrame *)((u8 *)r->worldFrames + base);
    if (r->tracks.itemGroups != NULL)
    {
        r->tracks.itemGroups = (struct ReplayItemGroupTrack *)((u8 *)r->tracks.itemGroups + base);
        igTrack = r->tracks.itemGroups;
        for (i = r->tracks.itemGroupCount; i > 0; i--, igTrack++)
            igTrack->frames += base;
    }
    if (r->tracks.seesaws != NULL)
    {
        r->tracks.seesaws = (struct ReplaySeesawTrack *)((u8 *)r->tracks.seesaws + base);
        ssTrack = r->tracks.seesaws;
        for (i = r->tracks.seesawCount; i > 0; i--, ssTrack++)
        {
            if (ssTrack->frames != NULL)
                ssTrack->frames += base;
        }
    }
}

void g_something_with_replays(int id, void *buf)
{
    u32 size = ((struct Replay *)buf)->size;
    struct Replay *replay;

    fn_1_5FE5C(id);
    replay = OSAllocFromHeap(replay_heap, size);
    while (replay == NULL)
    {
        int evict = pick_replay_to_evict();

        if (evict < 0)
            OSPanic("recplay.c", 0xA8A, "Can't alloc the memory for RECPLAY!\n");
        else
        {
            fn_1_5FE5C(evict);
            replay = OSAllocFromHeap(replay_heap, size);
        }
    }
    memcpy_handler(replay, buf, size);
    fn_1_63450(replay);
    g_something_with_stage_world_themes(replay);
    replay->header.flags |= 0x400;
    g_some_replay_data4[id] = replay;
}

void g_something_with_bundled_replays(void)
{
    DVDDir dir;
    DVDDirEntry entry;
    char path[0x80];
    int i;
    struct BundledReplay *bundled;

    memset(lbl_1_bss_719BC, 0, sizeof(lbl_1_bss_719BC));
    DVDOpenDir("/replay/", &dir);
    i = 0;
    bundled = lbl_1_bss_719BC;
    while (i < 0xA0 && DVDReadDir(&dir, &entry))
    {
        int n;
        u8 *p;
        u32 value;

        if (entry.isDir)
            continue;
        sprintf(path, "/replay/%s", entry.name);
        bundled->entryNum = DVDConvertPathToEntrynum(path);
        p = (u8 *)entry.name + (strlen(entry.name) - 4);
        value = 0;
        for (n = 4; n > 0; n--, p++)
        {
            value <<= 4;
            if (*p >= '0' && *p <= '9')
                value |= *p - '0';
            else if (*p >= 'A' && *p <= 'F')
                value |= *p - ('A' - 10);
        }
        bundled->attr = value;
        bundled->age = i & 1;
        i++;
        bundled++;
    }
    lbl_1_bss_71EBC = i;
    fn_80018FF4(&dir);
}

int g_LoadRecFile(int bundledId, int id)
{
    struct File file;
    void *buf;
    u32 size;
    OSHeapHandle heap;
    struct BundledReplay *bundled;
    struct Replay *replay;

    printf("LoadRecFile(%d, %d)\n", bundledId, id);
    if (replay_heap < 0 || OSCheckHeap(replay_heap) < 0)
        return 0;
    bundled = &lbl_1_bss_719BC[bundledId];
    g_something_with_dvd(bundled->entryNum, &file);
    size = OSRoundUp32B(file_size(&file));
    if (size <= 0x10000)
    {
        heap = -1;
        buf = g_some_buffer_ptr;
    }
    else
    {
        heap = currentHeap;
        buf = OSAllocFromHeap(heap, size);
    }
    if (buf != NULL)
        file_read(&file, buf, size, 0);
    file_close(&file);
    if (buf == NULL)
        return 1;
#ifndef __MWERKS__
    {
        /* pc_port: .rec files are big-endian struct Replay images */
        extern void pc_swap_replay(void *buf, u32 fileSize);
        pc_swap_replay(buf, size);
    }
#endif
    g_something_with_replays(id, buf);
    if (heap >= 0)
        OSFreeToHeap(heap, buf);
    replay = g_some_replay_data4[id];
    if (replay != NULL)
    {
        replay->header.monkey = lbl_1_bss_71900;
        lbl_1_bss_71900++;
        if (lbl_1_bss_71900 >= 4)
            lbl_1_bss_71900 = 0;
    }
    return 1;
}

void fn_1_63BC4(void)
{
    if (lbl_1_bss_719BA == 0)
    {
        int best;
        int maxAge;
        int picks = __abs(rand() & 0x7FFF) % 4 + 4;
        u8 *out = lbl_1_bss_71EC0;
        struct BundledReplay *bundled;
        int i;

        best = -1;
        bundled = lbl_1_bss_719BC;
        for (i = 0; i < lbl_1_bss_71EBC; i++, bundled++)
        {
            bundled->age++;
            if ((bundled->attr & 0x11) == 0x11 && (best < 0 || maxAge < bundled->age))
            {
                maxAge = bundled->age;
                best = i;
            }
        }
#ifndef __MWERKS__
        /* pc_port: an empty or non-matching /replay/ scan would index
         * lbl_1_bss_719BC[-1] and divide by lbl_1_bss_71EBC == 0 below */
        if (best < 0 || lbl_1_bss_71EBC <= 0)
            return;
#endif
        lbl_1_bss_719BC[best].age = (rand() & 0x7FFF) % ((maxAge >> 1) + 1);
        *out = best;
        out++;
        for (best = picks - 1; best > 0; best--, out++)
        {
            int age;
            struct BundledReplay *b = lbl_1_bss_719BC;
            int sum;
            int j;

            b->age++;
            age = b->age;
            maxAge = 0;
            sum = b->age;
            b++;
            for (j = 1; j < lbl_1_bss_71EBC; j++, b++)
            {
                if (b->age < 0xFFFF)
                    b->age++;
                if (age < b->age)
                {
                    age = b->age;
                    maxAge = j;
                }
                sum += b->age;
            }
            sum /= lbl_1_bss_71EBC;
            sum >>= 2;
            if (sum > 0)
                lbl_1_bss_719BC[maxAge].age = (rand() & 0x7FFF) % sum;
            else
                lbl_1_bss_719BC[maxAge].age = 0;
            *out = maxAge;
        }
        lbl_1_bss_719BA = picks;
    }
}

int g_something_with_replays2(int arg)
{
    int i;
    u8 *pick = &lbl_1_bss_71EC0[lbl_1_bss_719BA - 1];

    for (i = lbl_1_bss_719BA; i > 0; i--, pick--)
    {
        struct Replay **p = g_some_replay_data4;
        int id = -1;
        float score;
        float best = 20000.0f;
        int n;

        for (n = REPLAY_EVICT_SLOTS; n > 0; n--, p++)
        {
            struct Replay *replay = *p;

            if (replay == NULL)
            {
                id = REPLAY_EVICT_SLOTS - n;
                break;
            }
            if (replay->header.grade == 0.0)
            {
                id = REPLAY_EVICT_SLOTS - n;
                break;
            }
            score = replay->header.grade;
            score *= g_something_with_replays6(replay);
            if (id < 0 || score < best)
            {
                id = REPLAY_EVICT_SLOTS - n;
                best = score;
            }
        }
        arg = g_LoadRecFile(*pick, id);
        if (arg == 0)
            break;
    }
    if (arg == 0)
        return 0;
    if (replay_heap < 0 || OSCheckHeap(replay_heap) < 0)
        return 0;
    lbl_1_bss_719BA = 0;
    return 1;
}

void fn_1_63F54(int id)
{
    struct Replay *replay = g_some_replay_data4[id];

    if (replay != NULL)
        replay->header.grade *= 0.5f;
}
