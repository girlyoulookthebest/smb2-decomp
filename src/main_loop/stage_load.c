/* Stagedef loading: reads STAGE%03d.lz, turns its file offsets into pointers and binds model names
 * to GMA models. */
#include "types.h"
#include "avdisp.h"
#include "dip.h"
#include "file.h"
#include "libc.h"
#include "os.h"
#include "stage.h"

extern void *currentHeap;
extern struct GMA *worldGma;
extern struct GMA *g_bg_gma;
extern void *g_stage_nl_buf;
extern void *g_init_common_gma_buf;
extern struct GMAModel *worm_surface_gma_model;
extern struct GMAModel *wormhole_gma_model;
extern struct GMAModel *continue_gma_model;
extern struct GMAModel *goal_gma_models[3];
extern struct ItemGroup *itemgroups_copy;

#ifdef __MWERKS__
u32 strlen(const char *s);
#else
#include <string.h> /* size_t-correct prototypes (64-bit PC) */
#endif
int strcmp(const char *a, const char *b);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void OSFreeToHeap(void *heap, void *ptr);
void decompress_lz(void *src, void *dst);
void init_itemgroups(void);
void g_init_shadows(void);
void fn_1_5BC10(void);
void init_seesaws(void);

struct GMA **g_loaded_gmas_list[] = { &stage_gma, &worldGma, &g_bg_gma, &init_common_gma, NULL };

char *goal_gma_model_names[3] = { "GOAL", "GOAL_G", "GOAL_R" };

/* Same as fn_1_5CF50 (stage_name.c), which the original inlines here. */
static inline int string_match_len(u8 *a, u8 *b)
{
    int n = 0;

    while (*a == *b)
    {
        n++;
        if (*a == 0 || *b == 0)
            break;
        a++;
        b++;
    }
    return n;
}

static inline struct GMAModel *find_model_in_gma_list(char *name)
{
    struct GMA ***list;
    struct GMAModel *model;
    int i;

    list = g_loaded_gmas_list;
    model = NULL;
    while (*list != NULL)
    {
        if (**list != NULL)
        {
            for (i = 0; i < (int)(**list)->numModels; i++)
            {
                if (strcmp(name, (**list)->modelEntries[i].name) == 0)
                    model = (**list)->modelEntries[i].model;
            }
        }
        list++;
    }
    return model;
}

/* Picks the model whose name after its 4 character prefix shares the most with name. */
static inline struct GMAModel *find_model_in_gma_list_2(char *name, int len)
{
    struct GMA ***list;
    struct GMAModel *model;
    int i;

    list = g_loaded_gmas_list;
    model = NULL;
    while (*list != NULL)
    {
        if (**list != NULL)
        {
            for (i = 0; i < (int)(**list)->numModels; i++)
            {
                if (strlen((**list)->modelEntries[i].name) >= 4)
                {
                    int match = string_match_len((u8 *)name, (u8 *)(**list)->modelEntries[i].name + 4);

                    if (match > len)
                    {
                        len = match;
                        model = (**list)->modelEntries[i].model;
                    }
                }
            }
        }
        list++;
    }
    return model;
}

void load_stage_models(void)
{
    struct GMAModel *model;
    struct StageModelRef *ref;
    struct StageBgModel *bg;
    struct StageModelInfo *lod;
    int j;
    int i;

    bg = stagedef->bgModels;
    for (i = 0; i < stagedef->bgModelCount; i++, bg++)
    {
        model = find_model_in_gma_list(bg->name);
        if (model == NULL && (g_some_other_flags & 4))
            printf("warning BG %s : no match\n", bg->name);
        bg->model = model;
    }

    bg = stagedef->fgModels;
    for (i = 0; i < stagedef->fgModelCount; i++, bg++)
    {
        model = find_model_in_gma_list(bg->name);
        if (model == NULL && (g_some_other_flags & 4))
            printf("warning MV %s : no match\n", bg->name);
        bg->model = model;
    }

    ref = stagedef->modelList;
    for (i = 0; i < stagedef->modelListCount; i++, ref++)
    {
        lod = ref->lods;
        for (j = 0; j < ref->lodCount; j++, lod++)
        {
            model = find_model_in_gma_list(lod->name);
            if (model == NULL && (g_some_other_flags & 4))
                printf("warning INSTANCE %s : no match\n", lod->name);
            lod->model = model;
        }
    }

    for (i = 0; i < 3; i++)
    {
        int len = strlen(goal_gma_model_names[i]) - 1;

        goal_gma_models[i] = find_model_in_gma_list_2(goal_gma_model_names[i], len);
        if (goal_gma_models[i] == NULL)
            goal_gma_models[i] = init_common_gma->modelEntries[0x29].model;
    }

    {
        int len = strlen("CONTINUE") - 1;

        continue_gma_model = find_model_in_gma_list_2("CONTINUE", len);
    }
    if (continue_gma_model == NULL)
        continue_gma_model = init_common_gma->modelEntries[0x20].model;

    wormhole_gma_model = find_model_in_gma_list("WORMHOLE");
    worm_surface_gma_model = find_model_in_gma_list("WORM_SURFACE");
    if (worm_surface_gma_model == NULL)
        worm_surface_gma_model = wormhole_gma_model;
}

void *g_loaded_nl_list[] = { &g_stage_nl_buf, &g_init_common_gma_buf, NULL };

struct GMAModel *fn_1_5D6F4(char *name)
{
    return find_model_in_gma_list(name);
}

void init_seesaws(void)
{
    int i;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int type;
    struct Seesaw *seesaw;
    struct SeesawFuncs *funcs;
    int count;

    seesaw_count = 0;
    if (stagedef == NULL || itemgroups == NULL)
        return;

    count = 0;
    coll = stagedef->collHeaders;
    ig = itemgroups;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        type = 2;
        if (coll->animLoopType & 2)
            type = 0;
        if (type == 2)
            continue;
        funcs = &lbl_1_data_2EF80[type];
        seesaw = alloc_from_heap_or_panic(currentHeap, sizeof(*seesaw), "stcoli.c", 0x139);
        ig->seesaw = seesaw;
        memset(seesaw, 0, sizeof(*seesaw));
        seesaw->state = alloc_from_heap_or_panic(currentHeap, funcs->stateSize, "stcoli.c", 0x13D);
        memset(seesaw->state, 0, funcs->stateSize);
        seesaw->itemGroupId = i;
        seesaw->type = type;
        seesaw->init = funcs->init;
        seesaw->dest = funcs->dest;
        seesaw->tick = funcs->tick;
        seesaw->coli = funcs->coli;
        seesaw->func5 = funcs->func5;
        seesaw->replay = funcs->replay;
        seesaw->init(seesaw);
        count++;
    }
    if (count != 0)
        seesaw_count = count;
}

u8 g_some_32byte_lz_buffer[0x20] __attribute__((aligned(32)));

#define OFFSET_TO_PTR(base, offset) (void *)((uptr)(offset) + (uptr)(base))
#define OFFSET_TO_PTR2(base, offset) (void *)((uptr)(base) + (uptr)(offset))
#define ADJUST(p) p = OFFSET_TO_PTR(stagedef, p)
#define ADJUST_OPT(p) \
    if (p != NULL)    \
    ADJUST(p)
#define ADJUST_BASE(p, base)  \
    if (p != NULL)            \
    p = OFFSET_TO_PTR(base, p)

static inline void adjust_stage_anim_ptrs(struct StageItemGroupAnim * DPTR *anim, struct StageDef *base)
{
    *anim = OFFSET_TO_PTR(base, *anim);
    ADJUST_BASE((*anim)->rotXKeyframes, base);
    ADJUST_BASE((*anim)->rotYKeyframes, base);
    ADJUST_BASE((*anim)->rotZKeyframes, base);
    ADJUST_BASE((*anim)->posXKeyframes, base);
    ADJUST_BASE((*anim)->posYKeyframes, base);
    ADJUST_BASE((*anim)->posZKeyframes, base);
}

static inline void adjust_bg_anim_ptrs(struct StageBgAnim * DPTR *anim, struct StageDef *base)
{
    *anim = OFFSET_TO_PTR(base, *anim);
    ADJUST_BASE((*anim)->scaleX.frames, base);
    ADJUST_BASE((*anim)->scaleY.frames, base);
    ADJUST_BASE((*anim)->scaleZ.frames, base);
    ADJUST_BASE((*anim)->rotX.frames, base);
    ADJUST_BASE((*anim)->rotY.frames, base);
    ADJUST_BASE((*anim)->rotZ.frames, base);
    ADJUST_BASE((*anim)->posX.frames, base);
    ADJUST_BASE((*anim)->posY.frames, base);
    ADJUST_BASE((*anim)->posZ.frames, base);
    ADJUST_BASE((*anim)->visible.frames, base);
    ADJUST_BASE((*anim)->translucency.frames, base);
}

static inline void adjust_effect_ptrs(struct StageEffectHeader * DPTR *effect, struct StageDef *base)
{
    *effect = OFFSET_TO_PTR(base, *effect);
    ADJUST_BASE((*effect)->fx1Keyframes, base);
    ADJUST_BASE((*effect)->fx2Keyframes, base);
    ADJUST_BASE((*effect)->textureScroll, base);
}

#define ADJUST_BG_MODEL(bg)                                 \
    if ((bg)->flags & 0x8000)                               \
    {                                                       \
        u32 high = ((bg)->flags & 0xFFFF0000) >> 12;        \
                                                            \
        (bg)->flags &= 0xF;                                 \
        (bg)->flags |= high;                                \
    }                                                       \
    ADJUST((bg)->name);                                     \
    if ((bg)->anim != NULL)                                 \
        adjust_bg_anim_ptrs(&(bg)->anim, stagedef);         \
    if ((bg)->effect != NULL)                               \
        adjust_effect_ptrs(&(bg)->effect, stagedef);

static inline void adjust_extra_channels(struct StageExtraChannels *ch)
{
    ADJUST_OPT(ch->a.frames);
    ADJUST_OPT(ch->b.frames);
    ADJUST_OPT(ch->c.frames);
}

void load_stagedef(int stageId)
{
    char filename[32];
    struct File file;
    u32 compSize;
    u32 uncompSize;
    void *uncompData;
    void *compData;
    struct StageCollHeader *coll;
    struct StageBgModel *bg;
    struct StageModelRef *ref;
    int i;

    sprintf(filename, "STAGE%03d.lz", stageId);
    if (!file_open(filename, &file) && !file_open("STAGE000.lz", &file))
        OSPanic("stcoli.c", 0x189, "cannot Open");
    if (file_read(&file, g_some_32byte_lz_buffer, 0x20, 0) < 0)
        OSPanic("stcoli.c", 0x18D, "cannot Read");
    compSize = OSRoundUp32B(LWBRX(g_some_32byte_lz_buffer, 0));
    uncompSize = OSRoundUp32B(LWBRX(g_some_32byte_lz_buffer, 4));
    uncompData = alloc_from_heap_or_panic(currentHeap, uncompSize, "stcoli.c", 0x191);
    compData = alloc_from_heap_or_panic(currentHeap, compSize, "stcoli.c", 0x192);
    if (file_read(&file, compData, compSize, 0) < 0)
        OSPanic("stcoli.c", 0x194, "cannot Read");
    if (file_close(&file) != 1)
        OSPanic("stcoli.c", 0x195, "cannot Close");
    decompress_lz(compData, uncompData);
    OSFreeToHeap(currentHeap, compData);
    stagedef = uncompData;
    if (stagedef == NULL)
        OSPanic("stcoli.c", 0x19B, "cannot open stcoli\n");

#ifndef __MWERKS__
    { extern void pc_swap_stagedef(void *); pc_swap_stagedef(stagedef); }
#endif

    ADJUST(stagedef->collHeaders);
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, coll++)
    {
        if (0.0f == coll->animLoopStart && 0.0f == coll->animLoopEnd)
        {
            coll->animLoopStart = stagedef->loopStart;
            coll->animLoopEnd = stagedef->loopEnd;
        }
        if (coll->anim != NULL)
            adjust_stage_anim_ptrs(&coll->anim, stagedef);
        ADJUST_OPT(coll->triangles);
        if (coll->gridCellTris != NULL)
        {
            int j;
            s16 * DPTR *cell;

            ADJUST(coll->gridCellTris);
            cell = coll->gridCellTris;
            for (j = 0; j < coll->gridCellCountX * coll->gridCellCountZ; j++, cell++)
                ADJUST_OPT(*cell);
        }
        ADJUST_OPT(coll->goals);
        ADJUST_OPT(coll->bumpers);
        ADJUST_OPT(coll->jamabars);
        ADJUST_OPT(coll->bananas);
        ADJUST_OPT(coll->coliCones);
        ADJUST_OPT(coll->coliSpheres);
        ADJUST_OPT(coll->coliCylinders);
        ADJUST_OPT(coll->falloutVolumes);
        ADJUST_OPT(coll->reflectives);
        ADJUST_OPT(coll->instances);
        ADJUST_OPT(coll->modelRefs);
        ADJUST_OPT(coll->buttons);
        ADJUST_OPT(coll->boundSphere);
        ADJUST_OPT(coll->wormholes);
        ADJUST_OPT(coll->generators);
        ADJUST_OPT(coll->textureScroll);
    }

    ADJUST_OPT(stagedef->startPos);
    ADJUST_OPT(stagedef->falloutY);
    ADJUST_OPT(stagedef->goals);
    ADJUST_OPT(stagedef->bumpers);
    ADJUST_OPT(stagedef->jamabars);
    ADJUST_OPT(stagedef->bananas);
    ADJUST_OPT(stagedef->coliCones);
    ADJUST_OPT(stagedef->coliCylinders);
    ADJUST_OPT(stagedef->falloutVolumes);
    ADJUST_OPT(stagedef->reflectives);
    for (i = 0; i < stagedef->reflectiveCount; i++)
        stagedef->reflectives[i].modelName = OFFSET_TO_PTR2(stagedef, stagedef->reflectives[i].modelName);
    ADJUST_OPT(stagedef->unk80);
    ADJUST_OPT(stagedef->instances);
    for (i = 0; i < stagedef->instanceCount; i++)
        stagedef->instances[i].modelRef = OFFSET_TO_PTR2(stagedef, stagedef->instances[i].modelRef);
    ADJUST_OPT(stagedef->modelRefs);
    for (i = 0; i < stagedef->modelRefCount; i++)
        stagedef->modelRefs[i] = OFFSET_TO_PTR2(stagedef, stagedef->modelRefs[i]);
    ADJUST_OPT(stagedef->buttons);
    if (stagedef->fogAnim != NULL)
    {
        struct StageFogAnim *fogAnim;

        ADJUST(stagedef->fogAnim);
        fogAnim = stagedef->fogAnim;
        ADJUST(fogAnim->start.frames);
        ADJUST(fogAnim->end.frames);
        ADJUST(fogAnim->red.frames);
        ADJUST(fogAnim->green.frames);
        ADJUST(fogAnim->blue.frames);
        ADJUST(fogAnim->unk28.frames);
    }
    ADJUST_OPT(stagedef->fog);
    ADJUST_OPT(stagedef->lightAnim);
    for (i = 0; i < stagedef->lightAnimCount; i++)
    {
        struct StageLightAnim *light = &stagedef->lightAnim[i];

        ADJUST_OPT(light->dirX.frames);
        ADJUST_OPT(light->dirY.frames);
        ADJUST_OPT(light->dirZ.frames);
    }
    ADJUST_OPT(stagedef->wormholes);
    for (i = 0; i < stagedef->wormholeCount; i++)
        stagedef->wormholes[i].dest = OFFSET_TO_PTR2(stagedef, stagedef->wormholes[i].dest);
    ADJUST_OPT(stagedef->bgGuests);
    ADJUST_OPT(stagedef->dynamicReflectionPlane);
    ADJUST_OPT(stagedef->generators);
    for (i = 0; i < stagedef->generatorCount; i++)
    {
        struct StageGenerator *gen = &stagedef->generators[i];

        ADJUST(gen->name);
        ADJUST_OPT(gen->scaleX.frames);
        ADJUST_OPT(gen->scaleY.frames);
        ADJUST_OPT(gen->scaleZ.frames);
        ADJUST_OPT(gen->unk38.frames);
        ADJUST_OPT(gen->red.frames);
        ADJUST_OPT(gen->green.frames);
        ADJUST_OPT(gen->blue.frames);
        /* tests green rather than alpha */
        if (gen->green.frames != NULL)
            ADJUST(gen->alpha.frames);
    }
    ADJUST_OPT(stagedef->unkD4);

    if (stagedef->bgModels != NULL)
    {
        ADJUST(stagedef->bgModels);
        bg = stagedef->bgModels;
        for (i = 0; i < stagedef->bgModelCount; i++, bg++)
        {
            ADJUST_BG_MODEL(bg);
        }
    }
    if (stagedef->fgModels != NULL)
    {
        ADJUST(stagedef->fgModels);
        bg = stagedef->fgModels;
        for (i = 0; i < stagedef->fgModelCount; i++, bg++)
        {
            ADJUST_BG_MODEL(bg);
        }
    }

    if (stagedef->extraData != NULL)
    {
        struct StageExtraData *extra;
        struct StageExtraChannels *ch;

        extra = stagedef->extraData = OFFSET_TO_PTR(stagedef, stagedef->extraData);
        ch = (struct StageExtraChannels *)extra;
        adjust_extra_channels(ch);
        ADJUST_OPT(extra->list);
        ch = extra->list;
        for (i = 0; i < extra->listCount; i++, ch++)
            adjust_extra_channels(ch);
        ADJUST_OPT(extra->unk20.frames);
        ADJUST_OPT(extra->unk28.frames);
        ADJUST_OPT(extra->unk30.frames);
        ADJUST_OPT(extra->unk38.frames);
        ADJUST_OPT(extra->unk40.frames);
        ADJUST_OPT(extra->unk48.frames);
        ADJUST_OPT(extra->unk50.frames);
        ADJUST_OPT(extra->unk58.frames);
        ADJUST_OPT(extra->unk60.frames);
    }

    if (stagedef->modelListCount > 0)
    {
        ADJUST(stagedef->modelList);
        ref = stagedef->modelList;
        for (i = 0; i < stagedef->modelListCount; i++, ref++)
        {
            struct StageModelInfo *lod;
            int j;

            ADJUST(ref->lods);
            lod = ref->lods;
            for (j = 0; j < ref->lodCount; j++, lod++)
                ADJUST(lod->name);
        }
    }

    if (stagedef->extraAnims != NULL)
    {
        ADJUST(stagedef->extraAnims);
        if (stagedef->extraAnims->animA != NULL)
            adjust_stage_anim_ptrs(&stagedef->extraAnims->animA, stagedef);
        if (stagedef->extraAnims->animB != NULL)
            adjust_stage_anim_ptrs(&stagedef->extraAnims->animB, stagedef);
    }

    itemgroups = alloc_from_heap_or_panic(currentHeap, stagedef->itemGroupCount * sizeof(struct ItemGroup), "stcoli.c", 0x2BE);
    memset(itemgroups, 0, stagedef->itemGroupCount * sizeof(struct ItemGroup));
    itemgroups_copy = alloc_from_heap_or_panic(currentHeap, stagedef->itemGroupCount * sizeof(struct ItemGroup), "stcoli.c", 0x2C0);
    memset(itemgroups_copy, 0, stagedef->itemGroupCount * sizeof(struct ItemGroup));
    init_itemgroups();
    init_seesaws();
    g_init_shadows();
}

void g_free_stage(void)
{
    struct Seesaw *seesaw;
    struct ItemGroup *ig;
    int i;

    if (stagedef == NULL)
        return;
    fn_1_5BC10();
    if (stagedef != NULL && itemgroups != NULL)
    {
        ig = &itemgroups[stagedef->itemGroupCount - 1];
        for (i = stagedef->itemGroupCount; i > 0; i--, ig--)
        {
            seesaw = ig->seesaw;
            if (seesaw != NULL)
            {
                seesaw->dest(seesaw);
                OSFreeToHeap(currentHeap, seesaw->state);
                OSFreeToHeap(currentHeap, seesaw);
                ig->seesaw = NULL;
            }
        }
        seesaw_count = 0;
    }
    if (itemgroups != NULL)
    {
        OSFreeToHeap(currentHeap, itemgroups);
        itemgroups = NULL;
    }
    if (itemgroups_copy != NULL)
    {
        OSFreeToHeap(currentHeap, itemgroups_copy);
        itemgroups_copy = NULL;
    }
    itemgroups = NULL;
    OSFreeToHeap(currentHeap, stagedef);
    stagedef = NULL;
}

static void fn_1_5F9EC(u32 *dl, u32 *end, void (*func)(u32 *));
static void fn_1_5FAE8(u32 *dl, u32 *end, void (*func)(u32 *));

/* Runs a callback on the vertices of each mesh in a model, by mesh type.
 * Display lists hold strips (flag 0x10) of `count` vertices or lists (flag 8) of
 * `count` triangles. A vertex is 8 words if bit 0 of its first word is set, else 2. */
void fn_1_5F930(int *model, void (*posNrmTexFunc)(u32 *), void (*posColorTexFunc)(u32 *))
{
    int *mesh;
    u32 next;

    if (model[0] == -1)
        return;
    mesh = model + 6;
    while (mesh[0] != 0)
    {
        next = mesh[0x13] + 0x50;
        next = (u32)mesh + next;

        switch (mesh[9])
        {
        case -2:
            break;
        case -3:
            if (posColorTexFunc != NULL)
                fn_1_5FAE8((u32 *)(mesh + 0x14), (u32 *)next, posColorTexFunc);
            break;
        default:
            if (posNrmTexFunc != NULL)
                fn_1_5F9EC((u32 *)(mesh + 0x14), (u32 *)next, posNrmTexFunc);
            break;
        }
        mesh = (int *)next;
    }
}

static void fn_1_5F9EC(u32 *dl, u32 *end, void (*func)(u32 *))
{
    int i;
    int count;
    u32 *p;

    while (dl < end)
    {
        count = dl[1];
        p = dl + 2;
        if (dl[0] & 0x10)
        {
            for (; count > 0; count--)
            {
                if (((int *)p)[0] & 1)
                {
                    func(p);
                    p += 8;
                }
                else
                    p += 2;
            }
        }
        else if (dl[0] & 8)
        {
            for (; count > 0; count--)
            {
                for (i = 3; i > 0; i--)
                {
                    if (((int *)p)[0] & 1)
                    {
                        func(p);
                        p += 8;
                    }
                    else
                        p += 2;
                }
            }
        }
        dl = p;
    }
}

static void fn_1_5FAE8(u32 *dl, u32 *end, void (*func)(u32 *))
{
    int i;
    int count;
    u32 *p;

    while (dl < end)
    {
        count = dl[1];
        p = dl + 2;
        if (dl[0] & 0x10)
        {
            for (; count > 0; count--)
            {
                if (((int *)p)[0] & 1)
                {
                    func(p);
                    p += 8;
                }
                else
                    p += 2;
            }
        }
        else if (dl[0] & 8)
        {
            for (; count > 0; count--)
            {
                for (i = 3; i > 0; i--)
                {
                    if (((int *)p)[0] & 1)
                    {
                        func(p);
                        p += 8;
                    }
                    else
                        p += 2;
                }
            }
        }
        dl = p;
    }
}
