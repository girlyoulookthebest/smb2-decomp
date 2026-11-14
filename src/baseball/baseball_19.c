#include "background.h"
#include "baseball.h"

#define NULL2 ((void *)0)

struct BBLayerSet
{
    s16 *unk0;
    s16 *unk4;
    s16 *unk8;
};

extern s16 lbl_18_data_1408[10];
extern s16 lbl_18_data_141C[10];
extern s16 lbl_18_data_1430[10];
extern s16 lbl_18_data_1444[10];
extern s16 lbl_18_data_1458[10];
extern s16 lbl_18_data_146C[10];

s16 lbl_18_data_1480[10] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x0};
s16 lbl_18_data_1494[10] = {0x2, 0x3, 0x0, 0x0, 0x1, 0x0, 0x0, 0x3, 0x2, 0x0};
s16 lbl_18_data_14A8[10] = {0x1, 0x3, 0x0, 0x3, 0x0, 0x3, 0x1, 0x2, 0x2, 0x0};
s16 lbl_18_data_14BC[10] = {0x2, 0x2, 0x1, 0x3, 0x0, 0x3, 0x0, 0x3, 0x1, 0x0};
s16 lbl_18_data_14D0[10] = {0x3, 0x1, 0x3, 0x0, 0x3, 0x0, 0x3, 0x1, 0x3, 0x0};
s16 lbl_18_data_14E4[10] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
s16 lbl_18_data_14F8[10] = {0x1, 0x3, 0x0, 0x2, 0x1, 0x2, 0x0, 0x3, 0x1, 0x0};
s16 lbl_18_data_150C[10] = {0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x0};
s16 lbl_18_data_1520[10] = {0x2, 0x0, 0x1, 0x1, 0x2, 0x1, 0x1, 0x0, 0x2, 0x0};

struct BBLayerSet lbl_18_data_1534[2] = {
    {lbl_18_data_1408, lbl_18_data_141C, lbl_18_data_1430},
    {lbl_18_data_1444, lbl_18_data_1458, lbl_18_data_146C},
};

s16 *lbl_18_data_154C[9] = {
    lbl_18_data_1494, lbl_18_data_14A8, lbl_18_data_14BC,
    lbl_18_data_14D0, lbl_18_data_14E4, lbl_18_data_14F8,
    lbl_18_data_150C, lbl_18_data_1520, NULL,
};

struct BBTex lbl_18_bss_1090[3];

extern struct TPL *stage_tpl;
void *g_yet_another_unk_draw_func(void *func);
void fn_18_11C7C(u8 arg0)
{
    u8 tr3;
    s16 r30;
    s16 *tr29;
    s16 r31;
    struct BBTex *p;

    tr3 = fn_18_1798();
    if (tr3 == 0) {
        r30 = 0xA;
    } else {
        r30 = 0xF;
    }
    p = lbl_18_bss_1090;
    fn_18_11E54(p, lbl_18_data_1534[tr3].unk0, stage_gma->modelEntries[r30].model);
    p = &lbl_18_bss_1090[1];
    fn_18_11E54(p, lbl_18_data_1534[tr3].unk4, stage_gma->modelEntries[r30].model);
    p = &lbl_18_bss_1090[2];
    fn_18_11E54(p, lbl_18_data_1534[tr3].unk8, stage_gma->modelEntries[r30].model);
    tr29 = fn_18_11E38(arg0);
    for (r30 = 0; r30 < 3; r30++) {
        p = &lbl_18_bss_1090[r30];
        for (r31 = 0; r31 < 9; r31++) {
            fn_18_11EFC(p, lbl_18_data_1480[r31], tr29[r31]);
        }
    }
}

s16 *fn_18_11E38(u8 arg0)
{
    return lbl_18_data_154C[arg0];
}

void fn_18_11E54(struct BBTex *arg0, s16 *arg1, struct GMAModel *arg2)
{
    u16 r31;
    s32 r30;
    struct BBLayer *tr3;

    arg0->unk0 = arg1;
    r31 = arg0->unk0[0];
    for (r30 = 0; r30 < r31; r30++) {
        tr3 = fn_18_12048(arg2, arg0->unk0[r30 + 1]);
        if (tr3 != NULL) {
            arg0->unk4[r30] = tr3->unk8;
            arg0->unk30[r30] = tr3;
        }
    }
}

void fn_18_11EFC(struct BBTex *arg0, u16 arg1, u16 arg2)
{
    struct BBLayer *r6 = arg0->unk30[arg1];

    if ((arg0->unk0 != 0) && (arg0->unk4[arg2] != 0)) {
        r6->unk8 = arg0->unk4[arg2];
    }
}

static inline void bb_restore_textures(register struct BBTex *p)
{
    register struct BBLayer *layer;
    register u16 count;
    register s32 i;

    layer = p->unk30[0];
    if (p->unk0 != NULL) {
        count = p->unk0[0];
        for (i = 0; i < count; i++, layer++)
            layer->unk8 = p->unk4[i];
    }
}

void fn_18_11F48(void)
{
    struct BBTex *p;

    p = lbl_18_bss_1090;
    bb_restore_textures(p);
    p = &lbl_18_bss_1090[1];
    bb_restore_textures(p);
    p = &lbl_18_bss_1090[2];
    bb_restore_textures(p);
}

struct BBLayer *fn_18_12048(struct GMAModel *arg0, u32 arg1)
{
    struct BBLayer *tr6 = (struct BBLayer *) ((u8 *) arg0 + 0x40);
    s32 r7;

    for (r7 = 0; r7 < arg0->tevLayerCount; r7++) {
        if (arg1 == tr6[r7].unk4) {
            tr6[r7].unk8 = &arg0->texObjs[r7];
            tr6[r7].unk0 &= 0xFFFEFFFF;
            return &tr6[r7];
        }
    }
    return NULL;
}

void fn_18_120B4(void)
{
    void *p = &lbl_18_bss_2E0.unk1C;
    s32 tr30;

    tr30 = lbl_18_data_78B50[fn_18_1798()];
    fn_18_130F0();
    memset(p, 0, 0x18);
    lbl_18_bss_2E0.unk1C |= 1;
    fn_18_12AD8(tr30);
    load_stage_models();
    fn_18_12E70();
    fn_18_12E3C(tr30);
}

void fn_18_1214C(void)
{
    s32 tr31;
    struct ItemGroup *r30;
    s32 r29;

    lbl_18_bss_2E0.unk1C &= 0xFFFFFFFE;
    tr31 = OSSetCurrentHeap(stage_heap);
    r30 = itemgroups;
    r29 = 0;
    while (r29 < (s32) stagedef->itemGroupCount) {
        if ((struct Seesaw *) r30->seesaw != NULL) {
            if ((void *) r30->seesaw->state != NULL) {
                OSFreeToHeap(currentHeap, r30->seesaw->state);
                r30->seesaw->state = NULL;
            }
            OSFreeToHeap(currentHeap, r30->seesaw);
            r30->seesaw = NULL;
        }
        r29++;
        r30++;
    }
    if ((struct ItemGroup *) itemgroups != NULL) {
        OSFreeToHeap(stage_heap, itemgroups);
        itemgroups = NULL;
    }
    if (((struct GMA *) stage_gma != NULL) || (stage_tpl != NULL)) {
        gp_wait_wrapper();
    }
    if (stage_tpl != NULL) {
        g_something_freeing_heap_2(stage_tpl);
        stage_tpl = NULL;
    }
    if ((struct GMA *) stage_gma != NULL) {
        g_something_freeing_heap(stage_gma);
        stage_gma = NULL;
    }
    stagedef = NULL;
    fn_1_5BC10();
    OSSetCurrentHeap(tr31);
}

void fn_18_1232C(void)
{
    float f31;
    struct StageCollHeader *r31;
    void *tr30;
    struct ItemGroup *r29;
    s32 r28;
    s32 r27;
    s32 r27_2;
    struct StageModelRef * DPTR *r26;
    struct StageModelInstance *r26_2;
    struct GMAModel *tr3;
    struct GMAModel *tr3_2;

    tr30 = world_theme.unk8C;
    if (tr30 != 0) {
        g_yet_another_unk_draw_func(tr30);
    }
    g_check_some_condition(6);
    r29 = itemgroups;
    r31 = stagedef->collHeaders;
    r28 = 0;
    while (r28 < (s32) stagedef->itemGroupCount) {
        mtxa_from_mtxb();
        if (r28 > 0) {
            mtxa_mult_right(r29->transform);
        }
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        r26 = r31->modelRefs;
        r27 = 0;
        while (r27 < (s32) r31->modelRefCount) {
            tr3 = g_some_draw_func2(*r26);
            if (tr3 != NULL) {
                fn_1_10A2C0(tr3);
                fn_1_10A300(tr3);
            }
            r27++;
            r26++;
        }
        r26_2 = stagedef->collHeaders[r28].instances;
        mtxa_push();
        r27_2 = 0;
        while (r27_2 < (s32) stagedef->collHeaders[r28].instanceCount) {
            mtxa_peek();
            mtxa_translate(&r26_2->pos);
            mtxa_rotate_z(r26_2->rotZ);
            mtxa_rotate_y(r26_2->rotY);
            mtxa_rotate_x(r26_2->rotX);
            mtxa_scale(&r26_2->scale);
            if (r26_2->scale.x > r26_2->scale.y) {
                f31 = r26_2->scale.x;
            } else {
                f31 = r26_2->scale.y;
            }
            f31 = (r26_2->scale.z > f31) ? r26_2->scale.z : f31;
            avdisp_set_scale_factor(f31);
            tr3_2 = g_some_draw_func4(r26_2->modelRef, f31);
            if (tr3_2 != NULL) {
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_never(tr3_2);
            }
            r27_2++;
            r26_2++;
        }
        mtxa_pop();
        r28++;
        r29++;
        r31++;
    }
    if (tr30 != 0) {
        g_yet_another_unk_draw_func(0);
    }
}

void fn_18_12540(void)
{
    if (fn_18_334() != 1) {
        world_theme.animTimer += 1.0f;
        fn_18_126A0(world_theme.animTimer / 60.0, stagedef->bgModels, stagedef->bgModelCount);
    }
}

void fn_18_125CC(void)
{
    if ((struct StageBgModel *) stagedef->bgModels != NULL) {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    }
    fn_18_129F0(g_ord_tbl_stuff->matrices[0], stagedef->bgModels, stagedef->bgModelCount);
    if ((struct StageBgModel *) stagedef->fgModels != NULL) {
        mtxa_from_mtx(mtxa->mtxB);
    }
    fn_18_129F0(mtxa->mtxB, stagedef->fgModels, stagedef->fgModelCount);
}

void fn_18_126A0(float farg0, struct StageBgModel *arg0, s32 arg1)
{
    float f31;
    float tf2;
    s32 r30;
    struct StageBgAnim *tr29;
    s32 timer;

    if (arg0 == NULL2) {
        return;
    }
    for (r30 = 0; r30 < arg1; r30++, arg0++) {
        if (arg0->model == NULL2) {
            continue;
        }
        arg0->flags |= 0x10000;
        tr29 = arg0->anim;
        if (tr29 == NULL2) {
            continue;
        }
        timer = (arg0->animFlags & 0xC000) >> 14;
        f31 = farg0;
        if (arg0->animFlags & 1) {
            f31 += tr29->loopStart;
            tf2 = tr29->loopEnd - tr29->loopStart;
            if (f31 > tf2) {
                f31 = tf2;
            }
            f31 += tr29->loopStart;
        } else {
            f31 += tr29->loopStart;
            tf2 = tr29->loopEnd - tr29->loopStart;
            f31 -= tf2 * mathutil_floor(f31 / tf2);
            f31 += tr29->loopStart;
        }
        if (tr29->visible.frames != NULL2 && evaluate_stagedef_keyframe(f31, tr29->visible.count, tr29->visible.frames) < 0.5) {
            arg0->flags &= ~(1 << 16);
            continue;
        }
        if (tr29->translucency.frames != NULL2) {
            arg0->translucency = evaluate_stagedef_keyframe(f31, tr29->translucency.count, tr29->translucency.frames);
            if (arg0->translucency >= 1.0) {
                continue;
            }
        }
        if (tr29->scaleX.frames != NULL2) {
            arg0->scale.x = evaluate_stagedef_keyframe(f31, tr29->scaleX.count, tr29->scaleX.frames);
        }
        if (tr29->scaleY.frames != NULL2) {
            arg0->scale.y = evaluate_stagedef_keyframe(f31, tr29->scaleY.count, tr29->scaleY.frames);
        }
        if (tr29->scaleZ.frames != NULL2) {
            arg0->scale.z = evaluate_stagedef_keyframe(f31, tr29->scaleZ.count, tr29->scaleZ.frames);
        }
        if (tr29->rotX.frames != NULL2) {
            arg0->rotX = DEGREES_TO_S16(evaluate_stagedef_keyframe(f31, tr29->rotX.count, tr29->rotX.frames));
        }
        if (tr29->rotY.frames != NULL2) {
            arg0->rotY = DEGREES_TO_S16(evaluate_stagedef_keyframe(f31, tr29->rotY.count, tr29->rotY.frames));
        }
        if (tr29->rotZ.frames != NULL2) {
            arg0->rotZ = DEGREES_TO_S16(evaluate_stagedef_keyframe(f31, tr29->rotZ.count, tr29->rotZ.frames));
        }
        if (tr29->posX.frames != NULL2) {
            arg0->pos.x = evaluate_stagedef_keyframe(f31, tr29->posX.count, tr29->posX.frames);
        }
        if (tr29->posY.frames != NULL2) {
            arg0->pos.y = evaluate_stagedef_keyframe(f31, tr29->posY.count, tr29->posY.frames);
        }
        if (tr29->posZ.frames != NULL2) {
            arg0->pos.z = evaluate_stagedef_keyframe(f31, tr29->posZ.count, tr29->posZ.frames);
        }
    }
}

void fn_18_129F0(Mtx arg0, struct StageBgModel *arg1, s32 arg2)
{
    struct GMAModel *tr31;
    s32 r30;

    if (arg1 != NULL) {
        r30 = 0;
        while (r30 < arg2) {
            tr31 = arg1->model;
            mtxa_from_mtx(arg0);
            mtxa_translate(&arg1->pos);
            mtxa_rotate_z(arg1->rotZ);
            mtxa_rotate_y(arg1->rotY);
            mtxa_rotate_x(arg1->rotX);
            mtxa_scale(&arg1->scale);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            if (arg1->translucency < 1.1920929e-7f) {
                avdisp_draw_model_unculled_sort_auto(tr31);
            } else {
                avdisp_set_alpha((float) (1.0 - arg1->translucency));
                avdisp_draw_model_unculled_sort_always(tr31);
            }
            r30++;
            arg1++;
        }
    }
}


/* Shared with the gate animation code in baseball_20.c. */
const float lbl_18_rodata_5CC = 60.0f;
