/* Stage state (SMB1 stage.c): item group animation, stage loading, seesaws and fog. */
#include "libc.h"
#include "avdisp.h"
#include "camera.h"
#include "types.h"
#include "mathutil.h"
#include "dip.h"
#include "event.h"
#include "light.h"
#include "macros.h"
#include "mode.h"
#include "nl2ngc.h"
#include "recplay.h"
#include "stage.h"


int strcmp(const char *a, const char *b);
void event_dest(int id);

/* An item group's animation state, as stored in a replay. */
struct ReplayItemGroupFrame
{
    u32 playbackState;
    float animFrame;
};

/* What a seesaw is told about a ball that touched its item group. */
struct SeesawColiInfo
{
    /*0x00*/ s16 playerId;
    /*0x02*/ s16 unk2;
    /*0x04*/ Vec pos;  /* ball position in item group space */
    /*0x10*/ Vec impulse;  /* hardest collision normal scaled by its speed */
};

/* State of a seesaw item group; angles are s16 units held as floats. */
struct SeesawState
{
    /*0x00*/ float angle;
    /*0x04*/ float prevAngle;
    /*0x08*/ float angVel;
    /*0x0C*/ float sensitivity;
    /*0x10*/ float friction;
    /*0x14*/ float spring;
    /*0x18*/ Mtx invTransform;  /* item group space to seesaw pivot space */
    /*0x48*/ Mtx transform;
};  /* size = 0x78 */

struct StageDynamicPartsEntry
{
    s32 stageId;
    struct DynamicStagePart *parts;
};

struct StageState g_some_frame_counter;
struct DynamicStagePart *g_some_draw_var;
s8 lbl_1_bss_71098;
struct ItemGroup *itemgroups;
struct StageDef *stagedef;
s16 seesaw_count;
struct DynamicStagePart lbl_1_bss_710A8[5];
struct TPL *stage_tpl;
struct GMA *stage_gma;
s16 current_stage_id;
char current_stage_name[0x80];
struct Sphere lbl_1_bss_711B0;  /* the stage's bounding sphere */
u8 lbl_1_bss_711C0[0x14];  /* unreferenced */

/* Unreferenced; byte tables followed by 8-byte records. */
u32 lbl_1_data_2EE10[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

void seesaw_init(struct Seesaw *seesaw);
void seesaw_dest(struct Seesaw *seesaw);
void seesaw_tick(struct Seesaw *seesaw);
void seesaw_coli(struct Seesaw *seesaw, struct SeesawColiInfo *info);
void g_seesaw_func5(struct Seesaw *seesaw, void *state);
void g_seesaw_replay_smth(struct Seesaw *seesaw, int replay, float time);

struct SeesawFuncs lbl_1_data_2EF80[] = {
    { 0x78, 4, seesaw_init, seesaw_dest, seesaw_tick, seesaw_coli, g_seesaw_func5, g_seesaw_replay_smth },
};

/* Current fog; type < 0 means no fog. */
struct StageFogState
{
    /*0x00*/ s8 type;
    /*0x01*/ u8 red;
    /*0x02*/ u8 green;
    /*0x03*/ u8 blue;
    /*0x04*/ float start;
    /*0x08*/ float end;
};

struct StageFogState g_fog_type = { -1 };
struct StageDynamicPartsEntry lbl_1_data_2EFAC[] = { { 92, lbl_1_bss_710A8 } };
s32 g_current_stage_id = -1;  /* stage whose files are loaded */
s32 lbl_1_data_2EFB8 = -1;

extern s32 locale;
extern s32 stage_heap;
extern s32 bg_heap;
extern struct NlObj *g_stage_nl_buf;
extern struct TPL *lbl_1_bss_1288;
extern struct NlObj *g_init_common_gma_buf;
extern void *g_some_buffer_ptr;

/* Header in front of every NL model. */
struct NlModelHeader
{
    struct { u32 unk0; char name[]; } * DPTR unk0;
    struct { u32 modelSize; } * DPTR unk4;
};

#define NLMODEL_HEADER(model) ((struct NlModelHeader *)((u8 *)(model) - 8))

int OSSetCurrentHeap(int heap);
int DVDChangeDir(char *dir);
void queue_disc_read(char *file);
void fn_1_5FE3C(int);
void gp_wait_wrapper(void);
void g_something_freeing_heap(void *);
void g_something_freeing_heap_2(void *);
void g_free_stage(void);
int get_stage_world_theme(int stageId);
void g_something_with_world_themes(void);
void g_something_with_stage_world_themes2(int theme);
void free_world_gma_tpl(void);
void g_something_with_loading_wl_tpls(int theme);
void load_stage_models(void);
void fn_1_CB6CC(void);
void fn_1_57270(int stageId);
void fn_1_57B10(void);
void event_init(int id);
int fn_1_57A70(int stageId);
void g_something_with_init_or_maybe_leftover_from_smb1(void);
void g_queue_stage_name_load(void);
void read_stage_name_from_dvd(int stageId, char *buf, int size);
void load_stagedef(int stageId);
struct TPL *g_load_tpl(char *name);
struct GMA *g_load_gma(char *name, struct TPL *tpl);
void g_load_stage_gma_tpl_lz(int stageId);
int fn_1_5CF50(char *a, char *b);

void g_load_stgname_file(s32 locale);
void fn_1_573DC(struct NlVtxTypeB *);
void fn_1_57600(struct NlVtxTypeA *);
int g_handle_bonus_wave_collision(Vec *rayOrigin, Vec *outHitPos, Vec *outHitNormal);

float fn_1_62B20(int replay, float time);
void fn_1_624A8(int replay, float time, int itemGroupId, struct ReplayItemGroupFrame *out);
void fn_1_5F930(void *model, void *posNrmTexFunc, void *posColorTexFunc);
void fn_1_E2918(void);
void g_init_smth_with_stage_models(void);
void init_itemgroups(void);
void g_init_smth_with_seesaws(void);
void g_advance_stage_animation(void);
void g_transform_some_itemgroup_vec(void);
void g_smth_with_stage_fog(float time);
void fn_1_58948(float time);

void event_stage_init(void)
{
    memset(&g_some_frame_counter, 0, sizeof(g_some_frame_counter));
    g_init_smth_with_stage_models();
    init_itemgroups();
    g_init_smth_with_seesaws();
}

void event_stage_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (mode_info.flags & INFO_FLAG_08)
    {
        if (modeCtrl.submodeTimer > 360)
            g_some_frame_counter.timer = 0.0f;
        else
            g_some_frame_counter.timer = 360 - modeCtrl.submodeTimer;
        g_some_frame_counter.frameCounter = 359;
    }
    else if (mode_info.flags & INFO_FLAG_REPLAY)
    {
        g_some_frame_counter.timer = fn_1_62B20(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], lbl_1_bss_71EC8.unk10);
        g_some_frame_counter.frameCounter = g_some_frame_counter.timer;
    }
    else
        g_some_frame_counter.timer = g_some_frame_counter.frameCounter;
    g_advance_stage_animation();
    if (g_some_frame_counter.callback != NULL)
        g_some_frame_counter.callback();
    if (g_some_draw_var != NULL)
    {
        struct DynamicStagePart *dyn = g_some_draw_var;

        while (dyn->modelName != NULL)
        {
            if (dyn->tempModel != NULL)
            {
                memcpy(dyn->tempModel, dyn->origModel, **(u32 **)((u8 *)dyn->origModel - 4));
                fn_1_5F930(dyn->tempModel, dyn->posNrmTexFunc, dyn->posColorTexFunc);
            }
            dyn++;
        }
    }
    g_transform_some_itemgroup_vec();
    if (g_fog_type.type >= 0)
        g_smth_with_stage_fog(g_some_frame_counter.timer);
    if (lbl_1_bss_71098 != 0)
        fn_1_58948(g_some_frame_counter.timer);
    if (!(g_some_frame_counter.flags & 5))
        g_some_frame_counter.frameCounter++;
}

void event_stage_dest(void)
{
    g_fog_type.type = -1;
    lbl_1_bss_71098 = 0;
    fn_1_E2918();
}

/* Item group animation driven by the mode timer (stage preview). */
static inline float get_preview_itemgroup_anim_time(struct ItemGroup *ig)
{
    int state = ig->playbackState & 7;
    float t;

    if (state == 1)
        return 0.0f;
    else
    {
        int frame;

        if (modeCtrl.submodeTimer > 360)
        {
            frame = 0;
            t = 0.0f;
        }
        else
        {
            frame = 360 - modeCtrl.submodeTimer;
            t = frame / 60.0f;
        }
        switch (state)
        {
        case 2:
            t = -t;
            break;
        case 3:
            t *= 2.0f;
            frame *= 2;
            break;
        case 4:
            t *= -2.0f;
            frame = -(frame * 2);
            break;
        }
        ig->animFrame = frame;
        return t;
    }
}

float advance_itemgroup_anim(struct ItemGroup *ig, struct StageCollHeader *coll)
{
    float t;

    if ((mode_info.flags & INFO_FLAG_08) && events[EVENT_VIEW].state == 0)
    {
        t = get_preview_itemgroup_anim_time(ig);
    }
    else
    {
        if (!(g_some_frame_counter.flags & 1))
        {
            int frame = ig->animFrame;
            int limit;

            if (!(g_some_frame_counter.flags & 4))
            {
                switch (ig->playbackState & 7)
                {
                case 0:
                    frame++;
                    break;
                case 2:
                    frame--;
                    break;
                case 3:
                    frame += 2;
                    break;
                case 4:
                    frame -= 2;
                    break;
                }
            }
            if (coll->animLoopType & 1)
            {
                switch (ig->playbackState & 7)
                {
                case 0:
                case 3:
                    limit = 60.0f * coll->animLoopEnd;
                    if (frame > limit)
                    {
                        frame = limit;
                        ig->playbackState = (ig->playbackState & ~7) | 1;
                    }
                    break;
                case 2:
                case 4:
                    limit = 60.0f * coll->animLoopStart;
                    if (frame < limit)
                    {
                        frame = limit;
                        ig->playbackState = (ig->playbackState & ~7) | 1;
                    }
                    break;
                }
            }
            ig->animFrame = frame;
            t = (float)ig->animFrame / 60.0;
        }
        else
            t = (float)ig->animFrame / 60.0;
    }
    if (!(coll->animLoopType & 1))
    {
        float len = coll->animLoopEnd - coll->animLoopStart;

        t -= len * mathutil_floor(t / len);
        t += coll->animLoopStart;
    }
    return t;
}

void g_advance_stage_animation(void)
{
    struct ItemGroup *ig = itemgroups;
    struct StageCollHeader *coll = stagedef->collHeaders;
    int i;

    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        struct StageItemGroupAnim *anim = coll->anim;
        float t;

        if (anim == NULL)
            continue;
        if (mode_info.flags & INFO_FLAG_REPLAY)
        {
            struct ReplayItemGroupFrame frame;
            float replayTime;

            fn_1_624A8(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], lbl_1_bss_71EC8.unk10, i, &frame);
            ig->playbackState = frame.playbackState;
            ig->animFrame = frame.animFrame;
            replayTime = (1.0f / 60.0f) * frame.animFrame;
            if (!(coll->animLoopType & 1))
            {
                float len = coll->animLoopEnd - coll->animLoopStart;

                replayTime -= len * mathutil_floor(replayTime / len);
                replayTime += coll->animLoopStart;
            }
            t = replayTime;
        }
        else
        {
            if ((ig->playbackState & 7) == 1)
                continue;
            t = advance_itemgroup_anim(ig, coll);
        }
        if (anim->rotXKeyframes != NULL)
        {
            ig->prevRot.x = ig->rot.x;
            ig->rot.x = DEGREES_TO_S16(evaluate_stagedef_keyframe(t, anim->rotXKeyframeCount, anim->rotXKeyframes));
        }
        if (anim->rotYKeyframes != NULL)
        {
            ig->prevRot.y = ig->rot.y;
            ig->rot.y = DEGREES_TO_S16(evaluate_stagedef_keyframe(t, anim->rotYKeyframeCount, anim->rotYKeyframes));
        }
        if (anim->rotZKeyframes != NULL)
        {
            ig->prevRot.z = ig->rot.z;
            ig->rot.z = DEGREES_TO_S16(evaluate_stagedef_keyframe(t, anim->rotZKeyframeCount, anim->rotZKeyframes));
        }
        if (anim->posXKeyframes != NULL)
        {
            ig->prevPos.x = ig->pos.x - coll->conveyorVel.x;
            ig->pos.x = evaluate_stagedef_keyframe(t, anim->posXKeyframeCount, anim->posXKeyframes);
        }
        if (anim->posYKeyframes != NULL)
        {
            ig->prevPos.y = ig->pos.y - coll->conveyorVel.y;
            ig->pos.y = evaluate_stagedef_keyframe(t, anim->posYKeyframeCount, anim->posYKeyframes);
        }
        if (anim->posZKeyframes != NULL)
        {
            ig->prevPos.z = ig->pos.z - coll->conveyorVel.z;
            ig->pos.z = evaluate_stagedef_keyframe(t, anim->posZKeyframeCount, anim->posZKeyframes);
        }
        mtxa_from_translate(&ig->pos);
        mtxa_rotate_z(ig->rot.z);
        mtxa_rotate_y(ig->rot.y);
        mtxa_rotate_x(ig->rot.x - coll->initRot.x);
        mtxa_rotate_y(-coll->initRot.y);
        mtxa_rotate_z(-coll->initRot.z);
        mtxa_translate_neg(&coll->initPos);
        mtxa_to_mtx(ig->transform);
        mtxa_from_translate(&ig->prevPos);
        mtxa_rotate_z(ig->prevRot.z);
        mtxa_rotate_y(ig->prevRot.y);
        mtxa_rotate_x(ig->prevRot.x - coll->initRot.x);
        mtxa_rotate_y(-coll->initRot.y);
        mtxa_rotate_z(-coll->initRot.z);
        mtxa_translate_neg(&coll->initPos);
        mtxa_to_mtx(ig->prevTransform);
    }
    if (seesaw_count != 0)
    {
        ig = itemgroups;
        for (i = 0; i < stagedef->itemGroupCount; i++, ig++)
        {
            if (ig->seesaw)
            {
                if (!(mode_info.flags & INFO_FLAG_REPLAY))
                    ig->seesaw->tick(ig->seesaw);
                else
                    ig->seesaw->replay(ig->seesaw, lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], lbl_1_bss_71EC8.unk10);
            }
        }
    }
}

void g_transform_some_itemgroup_vec(void)
{
    struct ItemGroup *ig = itemgroups;
    struct StageCollHeader *coll = stagedef->collHeaders;
    int i;

    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (coll->boundSphere != NULL)
        {
            mtxa_from_mtx(ig->transform);
            mtxa_tf_point(&coll->boundSphere->pos, &ig->boundSphereCenter);
        }
    }
}

void *get_GmaBuffer_entry(struct GMA *gma, char *name)
{
    struct GMAModelEntry *entry;
    int count;

    if (gma == NULL)
        return NULL;
    entry = gma->modelEntries;
    count = gma->numModels;
    while (count > 0)
    {
        if (strcmp(entry->name, name) == 0)
            return entry->model;
        count--;
        entry++;
    }
    return NULL;
}

void g_stgname_init(void)
{
    struct
    {
        u8 unk0[0x5C];
        s8 count;
        u8 unk5D[0x1A5 - 0x5D];
    } sp8;
    struct DynamicStagePart *dyn;

    memset(&sp8, 0, sizeof(sp8));
    g_load_stgname_file(locale);
    dyn = &lbl_1_bss_710A8[sp8.count];
    dyn->modelName = "SHAPE_STAGE134";
    dyn->posNrmTexFunc = fn_1_573DC;
    dyn->posColorTexFunc = fn_1_57600;
    dyn->raycastDownFunc = g_handle_bonus_wave_collision;
    sp8.count++;
    lbl_1_bss_710A8[sp8.count].modelName = NULL;
}

void init_itemgroups(void)
{
    struct ItemGroup *ig = itemgroups;
    struct StageCollHeader *coll = stagedef->collHeaders;
    int i;

    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        struct Seesaw *seesaw = ig->seesaw;

        memset(ig, 0, sizeof(*ig));
        ig->seesaw = seesaw;
        ig->pos.x = coll->initPos.x;
        ig->pos.y = coll->initPos.y;
        ig->pos.z = coll->initPos.z;
        ig->prevPos.x = coll->initPos.x - coll->conveyorVel.x;
        ig->prevPos.y = coll->initPos.y - coll->conveyorVel.y;
        ig->prevPos.z = coll->initPos.z - coll->conveyorVel.z;
        ig->rot.x = coll->initRot.x;
        ig->rot.y = coll->initRot.y;
        ig->rot.z = coll->initRot.z;
        ig->prevRot.x = coll->initRot.x;
        ig->prevRot.y = coll->initRot.y;
        ig->prevRot.z = coll->initRot.z;
        mtxa_from_identity();
        mtxa_to_mtx(ig->transform);
        mtxa_translate_neg(&coll->conveyorVel);
        mtxa_to_mtx(ig->prevTransform);
        if (coll->boundSphere != NULL)
            ig->boundSphereRadius = coll->boundSphere->radius;
        ig->playbackState |= coll->initPlaybackState;
    }
}

void fn_1_56C18(void) {}


s8 lbl_1_bss_71120 = 0;  /* background theme whose models are loaded */

void load_stage(int stageId)
{
    s8 stageEvent;
    s8 bgEvent;
    BOOL themeChanged = FALSE;
    int oldHeap2;
    int bgTheme;
    int oldHeap;

    stageEvent = events[EVENT_STAGE].state;
    bgEvent = events[EVENT_BACKGROUND].state;
    if (stageEvent != 0)
        event_dest(EVENT_STAGE);
    if (bgEvent != 0)
        event_dest(EVENT_BACKGROUND);
    fn_1_5FE3C(1);
    if (g_current_stage_id != stageId)
    {
        oldHeap = OSSetCurrentHeap(stage_heap);
        if (stage_tpl != NULL || stage_gma != NULL)
            gp_wait_wrapper();
        if (stage_tpl != NULL)
        {
            g_something_freeing_heap_2(stage_tpl);
            stage_tpl = NULL;
        }
        if (stage_gma != NULL)
        {
            g_something_freeing_heap(stage_gma);
            stage_gma = NULL;
        }
        free_nlobj(&g_stage_nl_buf, &lbl_1_bss_1288);
        g_free_stage();
        OSSetCurrentHeap(oldHeap);
    }
    if (world_theme != get_stage_world_theme(stageId))
    {
        g_something_with_world_themes();
        themeChanged = TRUE;
    }
    g_load_stage_gma_tpl_lz(stageId);
    bgTheme = stagedef->bgTheme;
    if (bgTheme != lbl_1_bss_71120)
    {
        oldHeap2 = OSSetCurrentHeap(bg_heap);
        free_world_gma_tpl();
        g_something_with_loading_wl_tpls(bgTheme);
        OSSetCurrentHeap(oldHeap2);
        lbl_1_bss_71120 = bgTheme;
    }
    g_something_with_stage_world_themes2(get_stage_world_theme(stageId));
    if (g_current_stage_id != stageId || themeChanged)
    {
        if (g_some_other_flags & 4)
            printf("========== st%03d ============\n", stageId);
        load_stage_models();
        fn_1_CB6CC();
        fn_1_57270(stageId);
        fn_1_57B10();
        g_current_stage_id = stageId;
    }
    light_init(stageId);
    if (stagedef != NULL && stagedef->fogAnim != NULL)
    {
        g_fog_type.type = 4;
        if (stagedef->fog != NULL)
            g_fog_type.type = (s8)stagedef->fog->type;
        g_smth_with_stage_fog(0.0f);
    }
    else
        g_fog_type.type = -1;
    fn_1_E2918();
    if (stagedef != NULL && stagedef->lightAnim != NULL)
    {
        lbl_1_bss_71098 = 1;
        fn_1_58948(0.0f);
    }
    else
        lbl_1_bss_71098 = 0;
    current_stage_id = stageId;
    if (stageEvent != 0)
        event_init(EVENT_STAGE);
    if (bgEvent != 0)
        event_init(EVENT_BACKGROUND);
}

void unload_stage(void)
{
    int oldHeap;

    g_something_with_world_themes();
    if (g_current_stage_id != -1)
    {
        oldHeap = OSSetCurrentHeap(stage_heap);
        if (stage_tpl != NULL || stage_gma != NULL)
            gp_wait_wrapper();
        if (stage_tpl != NULL)
        {
            g_something_freeing_heap_2(stage_tpl);
            stage_tpl = NULL;
        }
        if (stage_gma != NULL)
        {
            g_something_freeing_heap(stage_gma);
            stage_gma = NULL;
        }
        free_nlobj(&g_stage_nl_buf, &lbl_1_bss_1288);
        g_free_stage();
        OSSetCurrentHeap(oldHeap);
        current_stage_id = 0;
        g_current_stage_id = -1;
    }
}

/* Stages with an NL (Naomi) model file */
static u8 nlStageIds[] = { 10, 19, 20, 30, 49, 50, 60, 70, 80, 92, 96, 97, 98, 99, 100, 114, 115, 116, 117, 118, 119, 120, 0xFF };

static inline BOOL stage_has_nl_model(int stageId)
{
    u8 *p;

    for (p = nlStageIds; *p != 0xFF; p++)
    {
        if (*p == stageId)
            return TRUE;
    }
    return FALSE;
}

void queue_stage_load(int stageId)
{
    char gmaName[0x100];
    char tplName[0x100];
    char lzName[0x100];

    fn_1_57A70(stageId);
    g_something_with_init_or_maybe_leftover_from_smb1();
    g_queue_stage_name_load();
    DVDChangeDir("stage");
    sprintf(lzName, "STAGE%03d.lz", stageId);
    queue_disc_read(lzName);
    sprintf(tplName, "st%03d.tpl", stageId);
    sprintf(gmaName, "st%03d.gma", stageId);
    if (stageId != 190)
    {
        queue_disc_read(gmaName);
        queue_disc_read(tplName);
    }
    if (stage_has_nl_model(stageId))
    {
        sprintf(gmaName, "st%03d_p.lz", stageId);
        sprintf(tplName, "st%03d.lz", stageId);
        queue_disc_read(gmaName);
        queue_disc_read(tplName);
    }
    DVDChangeDir("/");
}

void g_load_stage_gma_tpl_lz(int stageId)
{
    char gmaName[0x100];
    char tplName[0x100];
    int oldHeap;

    if (g_current_stage_id != stageId)
    {
        lbl_1_data_2EFB8 = stageId;
        DVDChangeDir("stage");
        oldHeap = OSSetCurrentHeap(stage_heap);
        read_stage_name_from_dvd(stageId, current_stage_name, sizeof(current_stage_name));
        load_stagedef(stageId);
        if (stageId != 190)
        {
            sprintf(gmaName, "st%03d.gma", stageId);
            sprintf(tplName, "st%03d.tpl", stageId);
            stage_tpl = g_load_tpl(tplName);
            stage_gma = g_load_gma(gmaName, stage_tpl);
        }
        if (stage_has_nl_model(stageId))
        {
            sprintf(gmaName, "st%03d_p.lz", stageId);
            sprintf(tplName, "st%03d.lz", stageId);
            load_nlobj(&g_stage_nl_buf, &lbl_1_bss_1288, gmaName, tplName);
        }
        OSSetCurrentHeap(oldHeap);
        DVDChangeDir("/");
    }
}

struct NlObj **lbl_1_data_2F050[] = { &g_stage_nl_buf, &g_init_common_gma_buf, NULL, NULL };

void fn_1_57270(int stageId)
{
    struct DynamicStagePart *dyn;
    int bestLen;
    struct NlModel *model;
    struct NlObj ***objIter;
    u8 *tempModelEnd;
    int i;
    struct NlModel * DPTR *models;
    struct StageDynamicPartsEntry *entry;

    tempModelEnd = (u8 *)g_some_buffer_ptr + 0x10000;
    entry = lbl_1_data_2EFAC;
    for (i = 1; i > 0; i--, entry++)
    {
        if (entry->stageId == stageId)
            break;
    }
    if (i <= 0)
    {
        g_some_draw_var = NULL;
        return;
    }
    g_some_draw_var = entry->parts;
    dyn = g_some_draw_var;
    while (dyn->modelName != NULL)
    {
        model = NULL;
        bestLen = 0;
        dyn->tempModel = NULL;
        for (objIter = lbl_1_data_2F050; *objIter != NULL; objIter++)
        {
            if (**objIter != NULL)
            {
                models = (**objIter)->models;
                for (i = 0; models[i] != NULL; i++)
                {
                    int len = fn_1_5CF50(dyn->modelName, NLMODEL_HEADER(models[i])->unk0->name);

                    if (len > bestLen)
                    {
                        bestLen = len;
                        {
                            int best = i;

                            model = models[best];
                        }
                    }
                }
                dyn->origModel = model;
                if (model != NULL)
                {
                    tempModelEnd -= NLMODEL_HEADER(model)->unk4->modelSize;
                    dyn->tempModel = tempModelEnd;
                }
            }
        }
        dyn++;
    }
}

void fn_1_573DC(struct NlVtxTypeB *vtxp)
{
    struct NlVtxTypeB vtx = *vtxp;
    float dist;
    float amplitude;
    int angle;
    Vec dir;

    dist = math_sqrt(vtx.x * vtx.x + vtx.z * vtx.z);
    amplitude = 0.5 + -0.030833333333333333 * dist;
    angle = -1092.0f * (g_some_frame_counter.timer - 30.0f) + (int)(16384.0 * dist);
    if (angle > 0)
        return;
    dir.x = vtx.x;
    dir.y = 0.0f;
    dir.z = vtx.z;
    vtx.y += amplitude * math_sin(angle);
    vec_set_len(-(amplitude * mathutil_cos(angle)), &dir, &dir);
    if (angle > -16384)
    {
        float scale = -6.103515625e-05f * angle;

        dir.x *= scale;
        dir.z *= scale;
    }
    vtx.nx += dir.x;
    vtx.nz += dir.z;
    vec_normalize_len((Vec *)&vtx.nx);
    *vtxp = vtx;
}

void fn_1_57600(struct NlVtxTypeA *vtxp)
{
    struct NlVtxTypeA vtx = *vtxp;
    float dist;
    float amplitude;
    int angle;

    dist = math_sqrt(vtx.x * vtx.x + vtx.z * vtx.z);
    amplitude = 0.5 + -0.030833333333333333 * dist;
    angle = -1092.0f * (g_some_frame_counter.timer - 30.0f) + (int)(16384.0 * dist);
    if (angle > 0)
        return;
    vtx.y += amplitude * math_sin(angle);
    *vtxp = vtx;
}

int g_handle_bonus_wave_collision(Vec *rayOrigin, Vec *outHitPos, Vec *outHitNormal)
{
    float dist;
    float amplitude;
    int angle;

    if (rayOrigin->x < -10.01 || rayOrigin->x > 10.01)
        return 0;
    if (rayOrigin->z < -10.01 || rayOrigin->z > 10.01)
        return 0;
    dist = math_sqrt(rayOrigin->x * rayOrigin->x + rayOrigin->z * rayOrigin->z);
    amplitude = 0.5 + -0.030833333333333333 * dist;
    angle = -1092.0f * (g_some_frame_counter.timer - 30.0f) + (int)(16384.0 * dist);
    if (angle > 0)
    {
        outHitPos->x = rayOrigin->x;
        outHitPos->y = 0.0f;
        outHitPos->z = rayOrigin->z;
        if (outHitNormal != NULL)
        {
            outHitNormal->x = 0.0f;
            outHitNormal->y = 1.0f;
            outHitNormal->z = 0.0f;
        }
        return 1;
    }
    outHitPos->x = rayOrigin->x;
    outHitPos->y = amplitude * math_sin(angle);
    outHitPos->z = rayOrigin->z;
    if (outHitNormal != NULL)
    {
        Vec n;
        float lenSq;
        float scale;

        outHitNormal->x = 0.0f;
        outHitNormal->y = 1.0f;
        outHitNormal->z = 0.0f;
        n.x = rayOrigin->x;
        n.z = rayOrigin->z;
        lenSq = mathutil_sum_of_sq_2(n.x, n.z);
        if (lenSq <= FLT_EPSILON)
            return 1;
        scale = -(amplitude * mathutil_cos(angle)) * math_rsqrt(lenSq);
        n.x *= scale;
        n.z *= scale;
        n.y = 1.0f;
        lenSq = mathutil_sum_of_sq_3(n.x, n.y, n.z);
        if (lenSq <= FLT_EPSILON)
            return 1;
        scale = math_rsqrt(lenSq);
        n.x *= scale;
        n.y *= scale;
        n.z *= scale;
        outHitNormal->x = n.x;
        outHitNormal->y = n.y;
        outHitNormal->z = n.z;
    }
    return 1;
}

extern u8 STAGE_WORLD_THEMES[];

int get_stage_world_theme(int stageId)
{
    if (sub_mode == 0x48)
    {
        switch (modeCtrl.difficulty)
        {
        case 0:
            return 0;
        case 1:
            return 0;
        case 2:
            break;
        }
        return 0;
    }
    return STAGE_WORLD_THEMES[stageId];
}

int fn_1_57A70(int stageId)
{
    int floor = mode_info.currFloor;
    int theme;

    mode_info.currFloor++;
    theme = get_stage_world_theme(stageId);
    mode_info.currFloor = (s16)floor;
    return theme;
}

struct GMAModel *fn_1_58280(struct StageModelRef *ref);
void mtxa_from_rotate_x(s16 angle);
void g_smth_with_seesaws_and_replays(int replay, int frame, int itemGroupId, float *out);
s16 *tri_coli_grid_lookup(struct StageCollHeader *coll, float x, float z);
void collide_ball_with_tri_face(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_tri_edges(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_tri_vertices(struct PhysicsBall *ball, struct StageColiTri *tri);
void collide_ball_with_cylinder(struct PhysicsBall *ball, struct StageColiCylinder *cylinder);
void collide_ball_with_sphere(struct PhysicsBall *ball, struct StageColiSphere *sphere);
void collide_ball_with_cone(struct PhysicsBall *ball, struct StageColiCone *cone);
void collide_ball_with_goal(struct PhysicsBall *ball, struct StageGoal *goal);
BOOL g_is_ball_in_ig_coli_range(struct ItemGroup *ig, struct StageCollHeader *coll, int igId, struct Sphere *sphere);

void fn_1_57B10(void)
{
    struct StageCollHeader *coll;
    int i;
    BOOL found;
    struct StageModelRef * DPTR *refs;
    int j;
    float minX, minY, minZ;
    float maxX, maxY, maxZ;

    found = FALSE;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, coll++)
    {
        refs = coll->modelRefs;
        for (j = 0; j < coll->modelRefCount; j++, refs++)
        {
            struct GMAModel *model = fn_1_58280(*refs);

            if (model == NULL)
                continue;
            if (!found)
            {
                minX = model->boundSphereCenter.x - model->boundSphereRadius;
                minY = model->boundSphereCenter.y - model->boundSphereRadius;
                minZ = model->boundSphereCenter.z - model->boundSphereRadius;
                maxX = model->boundSphereCenter.x + model->boundSphereRadius;
                maxY = model->boundSphereCenter.y + model->boundSphereRadius;
                maxZ = model->boundSphereCenter.z + model->boundSphereRadius;
                found = TRUE;
            }
            else
            {
                if (model->boundSphereCenter.x - model->boundSphereRadius < minX)
                    minX = model->boundSphereCenter.x - model->boundSphereRadius;
                if (model->boundSphereCenter.y - model->boundSphereRadius < minY)
                    minY = model->boundSphereCenter.y - model->boundSphereRadius;
                if (model->boundSphereCenter.z - model->boundSphereRadius < minZ)
                    minZ = model->boundSphereCenter.z - model->boundSphereRadius;
                if (model->boundSphereCenter.x + model->boundSphereRadius > maxX)
                    maxX = model->boundSphereCenter.x + model->boundSphereRadius;
                if (model->boundSphereCenter.y + model->boundSphereRadius > maxY)
                    maxY = model->boundSphereCenter.y + model->boundSphereRadius;
                if (model->boundSphereCenter.z + model->boundSphereRadius > maxZ)
                    maxZ = model->boundSphereCenter.z + model->boundSphereRadius;
            }
        }
    }
    if (found)
    {
        Vec halfSize;

        lbl_1_bss_711B0.pos.x = 0.5 * (maxX + minX);
        lbl_1_bss_711B0.pos.y = 0.5 * (maxY + minY);
        lbl_1_bss_711B0.pos.z = 0.5 * (maxZ + minZ);
        halfSize.x = 0.5 * (maxX - minX);
        halfSize.y = 0.5 * (maxY - minY);
        halfSize.z = 0.5 * (maxZ - minZ);
        lbl_1_bss_711B0.radius = math_sqrt(mathutil_sum_of_sq_3(halfSize.x, halfSize.y, halfSize.z));
    }
    else
    {
        lbl_1_bss_711B0.pos.x = 0.0f;
        lbl_1_bss_711B0.pos.y = 0.0f;
        lbl_1_bss_711B0.pos.z = 0.0f;
        lbl_1_bss_711B0.radius = 50.0f;
    }
}

void fn_1_57E14(Vec *outCenter, float *outRadius)
{
    float maxZ;
    float minX;
    float minY;
    float minZ;
    float maxX;
    float maxY;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int j;
    int i;
    struct StageModelRef * DPTR *refs;
    struct GMAModel *model;
    struct StageModelRef * DPTR *refs2;
    struct GMAModel *model2;
    float radius;
    float r;
    float t;
    Vec center;
    Vec pos;
    Vec pos2;

    minX = 0.0f;
    minY = 0.0f;
    minZ = 0.0f;
    maxX = 0.0f;
    maxY = 0.0f;
    maxZ = 0.0f;
    if (stage_gma != NULL)
    {
        ig = itemgroups;
        coll = stagedef->collHeaders;
        for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
        {
            mtxa_from_mtx(ig->transform);
            refs = coll->modelRefs;
            for (j = 0; j < coll->modelRefCount; j++, refs++)
            {
                float x, y, z, r1;

                model = fn_1_58280(*refs);
                if (model == NULL)
                    continue;
                mtxa_tf_point(&model->boundSphereCenter, &pos);
                r1 = model->boundSphereRadius;
                x = pos.x - r1;
                y = pos.y - r1;
                z = pos.z - r1;
                if (minX > x)
                    minX = x;
                if (minY > y)
                    minY = y;
                if (minZ > z)
                    minZ = z;
                x = pos.x + r1;
                y = pos.y + r1;
                z = pos.z + r1;
                if (maxX < x)
                    maxX = x;
                if (maxY < y)
                    maxY = y;
                if (maxZ < z)
                    maxZ = z;
            }
        }
        center.x = 0.5f * (minX + maxX);
        center.y = 0.5f * (minY + maxY);
        center.z = 0.5f * (minZ + maxZ);

        radius = 0.0f;
        ig = itemgroups;
        coll = stagedef->collHeaders;
        for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
        {
            mtxa_from_mtx(ig->transform);
            refs2 = coll->modelRefs;
            for (j = 0; j < coll->modelRefCount; j++, refs2++)
            {
                model2 = fn_1_58280(*refs2);
                if (model2 == NULL)
                    continue;
                mtxa_tf_point(&model2->boundSphereCenter, &pos2);
                r = model2->boundSphereRadius;
                t = r + math_sqrt((center.x - pos2.x) * (center.x - pos2.x) + (center.z - pos2.z) * (center.z - pos2.z));
                if (radius < t)
                    radius = t;
            }
        }
        radius *= 0.75f;
        *outCenter = center;
        *outRadius = radius;
    }
    else
    {
        outCenter->x = 0.0f;
        outCenter->y = 0.0f;
        outCenter->z = 0.0f;
        *outRadius = 50.0f;
    }
}

void *fn_1_58118(void *callback)
{
    void *old = g_some_frame_counter.callback;

    g_some_frame_counter.callback = callback;
    return old;
}

struct GMAModel *g_some_draw_func2(struct StageModelRef *ref)
{
    return g_some_draw_func4(ref, 1.0f);
}

struct GMAModel *g_some_draw_func4(struct StageModelRef *ref, float scale)
{
    struct GMAModel *model;
    struct StageModelInfo *lod;
    float size;
    Vec pos;
    int i;

    lod = ref->lods;
    model = lod->model;
    if (model == NULL)
        return NULL;
    size = 2.0 * model->boundSphereRadius * scale;
    mtxa_tf_point(&model->boundSphereCenter, &pos);
    if (pos.z > -FLT_EPSILON)
        return model;
    size = g_current_camera->sub28.viewportH
         * (240.0 * (size / (-pos.z * g_current_camera->sub28.view.tanHalfFov)));
    for (i = 0; i < ref->lodCount; i++, lod++)
    {
        if (size > lod->minSize)
            return lod->model;
    }
    return NULL;
}

struct GMAModel *fn_1_58280(struct StageModelRef *ref)
{
    return ref->lods->model;
}

void g_smth_with_stage_anim_groups(int animGroupId, int state)
{
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int copyMtx;
    int i;

    if (state == 1)
        copyMtx = TRUE;
    else
        copyMtx = FALSE;
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = stagedef->itemGroupCount; i > 0; i--, ig++, coll++)
    {
        if (coll->animGroupId != animGroupId)
            continue;
        if ((coll->animLoopType & 1) && (ig->playbackState & 7) == 1)
        {
            switch (state)
            {
            case 0:
            case 3:
                ig->animFrame = 60.0f * coll->animLoopStart;
                break;
            }
        }
        ig->playbackState = (ig->playbackState & ~7) | state;
        if (copyMtx)
            mtx_copy(ig->transform, ig->prevTransform);
    }
}

BOOL g_smth_with_buttons(int animGroupId, u32 state)
{
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int i;
    BOOL result;

    result = TRUE;
    ig = itemgroups;
    coll = stagedef->collHeaders;

    for (i = stagedef->itemGroupCount; i > 0; i--, ig++, coll++)
    {
        if (coll->animGroupId == animGroupId && (ig->playbackState & 7) != state)
        {
            result = FALSE;
            break;
        }
    }
    return result;
}

void g_init_smth_with_seesaws(void)
{
    int i;
    struct ItemGroup *ig;

    if (stagedef == NULL || itemgroups == NULL)
        return;
    ig = itemgroups;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++)
    {
        struct Seesaw *seesaw = ig->seesaw;

        if (seesaw != NULL)
            seesaw->init(seesaw);
    }
}

void collide_ball_with_seesaws(int a, int playerId, struct PhysicsBall *physBall)
{
    struct PhysicsBall ball;
    struct SeesawColiInfo info;
    struct Sphere sphere;
    int left;
    struct Seesaw *seesaw;
    struct ItemGroup *ig;
    int i;
    struct StageCollHeader *coll;
    struct StageColiTri *tri;
    s16 *cellTriIdx;
    s16 *cellTris;

    left = seesaw_count;
    if (left == 0)
        return;
    sphere.pos = physBall->pos;
    sphere.radius = physBall->radius;
    info.playerId = playerId;
    info.unk2 = a;
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount && left > 0; i++, ig++, coll++)
    {
        seesaw = ig->seesaw;
        if (seesaw == NULL)
            continue;
        left--;
        ball = *physBall;
        tf_physball_to_itemgroup_space(&ball, i);
        if (g_is_ball_in_ig_coli_range(ig, coll, i, &sphere))
        {
            cellTris = tri_coli_grid_lookup(coll, ball.pos.x, ball.pos.z);
            if (cellTris != NULL)
            {
                for (cellTriIdx = cellTris; *cellTriIdx >= 0; cellTriIdx++)
                {
                    tri = &coll->triangles[*cellTriIdx];
                    collide_ball_with_tri_face(&ball, tri);
                    collide_ball_with_tri_edges(&ball, tri);
                    collide_ball_with_tri_vertices(&ball, tri);
                }
            }
        }
        if (coll->coliConeCount != 0)
        {
            struct StageColiCone *cone = coll->coliCones;
            int j;

            for (j = coll->coliConeCount; j > 0; j--, cone++)
                collide_ball_with_cone(&ball, cone);
        }
        if (coll->coliSphereCount != 0)
        {
            struct StageColiSphere *coliSphere = coll->coliSpheres;
            int j;

            for (j = coll->coliSphereCount; j > 0; j--, coliSphere++)
                collide_ball_with_sphere(&ball, coliSphere);
        }
        if (coll->coliCylinderCount != 0)
        {
            struct StageColiCylinder *cylinder = coll->coliCylinders;
            int j;

            for (j = coll->coliCylinderCount; j > 0; j--, cylinder++)
                collide_ball_with_cylinder(&ball, cylinder);
        }
        if (coll->goalCount != 0)
        {
            struct StageGoal *goal = coll->goals;
            int j;

            for (j = coll->goalCount; j > 0; j--, goal++)
                collide_ball_with_goal(&ball, goal);
        }
        if (ball.flags & 1)
        {
            mtxa_from_mtx(ig->transform);
            info.pos = ball.pos;
            vec_set_len(ball.hardestColiSpeed, &ball.hardestColiNormal, &info.impulse);
            seesaw->coli(seesaw, &info);
        }
    }
}

int get_seesaw_replay_state_size(struct Seesaw *seesaw)
{
    return lbl_1_data_2EF80[seesaw->type].replayStateSize;
}

void *fn_1_58768(struct Seesaw *seesaw, void *state)
{
    seesaw->func5(seesaw, state);
    return state;
}

void g_smth_with_stage_fog(float t)
{
    struct StageFogAnim *anim = stagedef->fogAnim;
    float start = 60.0f * stagedef->loopStart;
    float len = 60.0f * stagedef->loopEnd - start;

    if (len > 0.0f)
    {
        t -= len * (int)((t - start) / len);
        t = (1.0f / 60.0f) * (t + start);
    }
    g_fog_type.start = evaluate_stagedef_keyframe(t, anim->start.count, anim->start.frames);
    g_fog_type.end = evaluate_stagedef_keyframe(t, anim->end.count, anim->end.frames);
    g_fog_type.red = 255.0f * evaluate_stagedef_keyframe(t, anim->red.count, anim->red.frames);
    g_fog_type.green = 255.0f * evaluate_stagedef_keyframe(t, anim->green.count, anim->green.frames);
    g_fog_type.blue = 255.0f * evaluate_stagedef_keyframe(t, anim->blue.count, anim->blue.frames);
}

void fn_1_58948(float t)
{
    struct StageLightAnim *anim = stagedef->lightAnim;
    float start = 60.0f * stagedef->loopStart;
    float len = 60.0f * stagedef->loopEnd - start;
    Vec dir;
    s16 rotY;
    s16 rotX;
    struct Light light;

    if (len > 0.0f)
    {
        t -= len * (int)((t - start) / len);
        t = (1.0f / 60.0f) * (t + start);
    }
    dir.x = evaluate_stagedef_keyframe(t, anim->dirX.count, anim->dirX.frames);
    dir.y = evaluate_stagedef_keyframe(t, anim->dirY.count, anim->dirY.frames);
    dir.z = evaluate_stagedef_keyframe(t, anim->dirZ.count, anim->dirZ.frames);
    rotY = math_atan2(dir.x, dir.z) - 0x8000;
    rotX = math_atan2(dir.y, math_sqrt(mathutil_sum_of_sq_2(dir.x, dir.z)));
    light.id = 1;
    light.inst = 0;
    light.type = 0;
    {
        struct Color3f color = { 1.0f, 1.0f, 1.0f };
        struct Color3f *src = &color;

        *(struct Color3f *)&light.red = *src;
    }
    light.rotX = rotX;
    light.rotY = rotY;
    add_light_to_pool(&light);
}

BOOL is_stage_id_not_for_party_game(int stageId)
{
    BOOL result = FALSE;

    if (stageId <= 130)
        result = TRUE;
    else if (stageId >= 181 && stageId != 199)
        result = TRUE;
    return result;
}

void seesaw_init(struct Seesaw *seesaw)
{
    struct StageCollHeader *coll = &stagedef->collHeaders[seesaw->itemGroupId];
    struct SeesawState *st = seesaw->state;

    st->angle = 0.0f;
    st->prevAngle = st->angle;
    st->angVel = 0.0f;
    mtxa_from_translate(&coll->initPos);
    mtxa_rotate_z(coll->initRot.z);
    mtxa_rotate_y(coll->initRot.y);
    mtxa_rotate_x(coll->initRot.x);
    mtxa_to_mtx(st->transform);
    mtxa_from_rotate_x(-coll->initRot.x);
    mtxa_rotate_y(-coll->initRot.y);
    mtxa_rotate_z(-coll->initRot.z);
    mtxa_translate_neg(&coll->initPos);
    mtxa_to_mtx(st->invTransform);
    st->sensitivity = 40.0f * coll->seesawSensitivity;
    st->friction = 1.0f - 0.02f * coll->seesawFriction;
    st->spring = -2.0f * coll->seesawSpring;
}

void seesaw_dest(struct Seesaw *seesaw) {}

void seesaw_tick(struct Seesaw *seesaw)
{
    struct ItemGroup *ig = &itemgroups[seesaw->itemGroupId];
    struct StageCollHeader *coll = &stagedef->collHeaders[seesaw->itemGroupId];
    struct SeesawState *st = seesaw->state;

    st->angVel += st->spring * math_sin(st->angle);
    st->angVel *= st->friction;
    st->angle += st->angVel;

    if (coll->anim != NULL)
        mtxa_from_mtx(ig->transform);
    else
        mtxa_from_identity();
    mtxa_mult_right(st->transform);
    mtxa_rotate_z(st->angle);
    mtxa_mult_right(st->invTransform);
    mtxa_to_mtx(ig->transform);

    if (coll->anim != NULL)
        mtxa_from_mtx(ig->prevTransform);
    else
        mtxa_from_identity();
    mtxa_mult_right(st->transform);
    mtxa_rotate_z(st->prevAngle);
    mtxa_mult_right(st->invTransform);
    mtxa_to_mtx(ig->prevTransform);

    st->prevAngle = st->angle;
}

void seesaw_coli(struct Seesaw *seesaw, struct SeesawColiInfo *info)
{
    struct SeesawState *st = seesaw->state;
    Vec pos;
    Vec imp;
    float nx;
    float ny;
    float len;
    float inv;
    float d;
    float mag;

    mtxa_from_mtx(st->invTransform);
    mtxa_tf_point(&info->pos, &pos);
    mtxa_tf_vec(&info->impulse, &imp);
    len = math_sqrt(mathutil_sum_of_sq_2(pos.x, pos.y));
    inv = 1.0f / len;
    nx = pos.x * inv;
    ny = pos.y * inv;
    d = -(nx * imp.x + ny * imp.y);
    imp.x += d * nx;
    imp.y += d * ny;
    mag = math_sqrt(mathutil_sum_of_sq_2(imp.x, imp.y));
    if (-(ny * imp.x) + nx * imp.y < 0.0f)
        mag = -mag;
    mag = st->sensitivity * (len * mag);
    st->angVel += mag;
}

void g_seesaw_func5(struct Seesaw *seesaw, void *state)
{
    *(float *)state = ((struct SeesawState *)seesaw->state)->angle;
}

void g_seesaw_replay_smth(struct Seesaw *seesaw, int replay, float time)
{
    struct ItemGroup *ig = &itemgroups[seesaw->itemGroupId];
    struct StageCollHeader *coll = &stagedef->collHeaders[seesaw->itemGroupId];
    struct SeesawState *st = seesaw->state;
    int frame;
    float a;
    float b;
    float t;

    frame = time;
    g_smth_with_seesaws_and_replays(replay, frame, seesaw->itemGroupId, &a);
    g_smth_with_seesaws_and_replays(replay, frame + 1, seesaw->itemGroupId, &b);
    t = time - frame;
    st->angle = a * (1.0f - t) + b * t;
    st->angVel = st->angle - st->prevAngle;

    if (coll->anim != NULL)
        mtxa_from_mtx(ig->transform);
    else
        mtxa_from_identity();
    mtxa_mult_right(st->transform);
    mtxa_rotate_z(st->angle);
    mtxa_mult_right(st->invTransform);
    mtxa_to_mtx(ig->transform);

    if (coll->anim != NULL)
        mtxa_from_mtx(ig->prevTransform);
    else
        mtxa_from_identity();
    mtxa_mult_right(st->transform);
    mtxa_rotate_z(st->prevAngle);
    mtxa_mult_right(st->invTransform);
    mtxa_to_mtx(ig->prevTransform);

    st->prevAngle = st->angle;
}

void fn_1_590AC(int a)
{
    if (a > 0)
        g_some_frame_counter.flags |= 2;
    else
        g_some_frame_counter.flags &= ~2;
    g_some_frame_counter.unkC = a;
}

void fn_1_590F4(int a)
{
    if (a > 0)
        g_some_frame_counter.flags |= 0x10;
    else
        g_some_frame_counter.flags &= ~0x10;
    g_some_frame_counter.unkD = a;
}
