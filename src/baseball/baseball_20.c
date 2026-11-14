#include "baseball.h"
#include "avdisp.h"
#include "baseball_stage_data.inc"

extern struct TPL *stage_tpl;
extern const float lbl_18_rodata_5CC;

struct GMA **lbl_18_data_78B48[] = { &stage_gma, NULL };
s32 lbl_18_data_78B50[2] = { 0x12D, 0x12E };
static s32 lbl_18_data_78B58 = -1;
static s32 lbl_18_data_78B5C = -1;

struct BBGateSet lbl_18_bss_11A8;

void fn_18_12AD8(s32 arg0)
{
    char sp108[0x100];
    char sp8[0x100];
    s32 tr30;
    struct StageDef *r0;

    if (lbl_18_data_78B58 != arg0) {
        lbl_18_data_78B5C = arg0;
        DVDChangeDir("stage");
        tr30 = OSSetCurrentHeap(stage_heap);
        if (arg0 == 0x12D) {
            r0 = (struct StageDef *) lbl_18_data_16E0;
        } else {
            r0 = (struct StageDef *) lbl_18_data_3415C;
        }
#ifndef __MWERKS__
        {
            /* the embedded stagedefs are big-endian word tables; fix them
             * up for PC once (pc_port endian_fix.c) */
            extern void pc_fix_embedded_stagedefs(void *const *, const u32 *, int,
                                                  void *const *, int);
            static int fixed;
            static void *const arrays[] = {
                lbl_18_data_1570, lbl_18_data_16E0, lbl_18_data_17B4, lbl_18_data_2854,
                lbl_18_data_2870, lbl_18_data_288C, lbl_18_data_296C, lbl_18_data_2988,
                lbl_18_data_29A4, lbl_18_data_3415C,
            };
            static const u32 sizes[] = {
                sizeof(lbl_18_data_1570), sizeof(lbl_18_data_16E0), sizeof(lbl_18_data_17B4),
                sizeof(lbl_18_data_2854), sizeof(lbl_18_data_2870), sizeof(lbl_18_data_288C),
                sizeof(lbl_18_data_296C), sizeof(lbl_18_data_2988), sizeof(lbl_18_data_29A4),
                sizeof(lbl_18_data_3415C),
            };
            static void *const defs[] = { lbl_18_data_16E0, lbl_18_data_3415C };

            if (!fixed) {
                fixed = 1;
#if PC_ADDR32_FIXUPS
                /* x64: store the address words (PC_A32 = 0) first; the
                 * byte-swap pass below then treats them exactly as on 32-bit */
                pc_apply_addr32_fixups(pc_bb_stage_data_fixups,
                                       sizeof(pc_bb_stage_data_fixups) / sizeof(pc_bb_stage_data_fixups[0]),
                                       "baseball stage data");
#endif
                pc_fix_embedded_stagedefs(arrays, sizes, 10, defs, 2);
            }
        }
#endif
        stagedef = r0;
        itemgroups = alloc_from_heap_or_panic(currentHeap, stagedef->itemGroupCount * (s32)sizeof(struct ItemGroup), "bbstage.c", 0x1FB);
        memset(itemgroups, 0, stagedef->itemGroupCount * (s32)sizeof(struct ItemGroup));
        init_itemgroups();
        g_baseball_init_seesaws();
        g_init_shadows();
        if (arg0 != 0xBE) {
            if (arg0 == 0x12D) {
                sprintf(sp108, "mdl_banana_st.gma");
                sprintf(sp8, "mdl_banana_st.tpl");
            } else {
                sprintf(sp108, "mdl_monkey_dm.gma");
                sprintf(sp8, "mdl_monkey_dm.tpl");
            }
            stage_tpl = g_load_tpl(sp8);
            stage_gma = g_load_gma(sp108, stage_tpl);
        }
        OSSetCurrentHeap(tr30);
        DVDChangeDir("/");
    }
}

void g_baseball_init_seesaws(void)
{
    s32 r31;
    struct ItemGroup *r30;
    struct StageCollHeader *r29;
    s32 r28;
    struct Seesaw *tr3;
    struct SeesawFuncs *tr26;
    s32 r25;

    seesaw_count = 0;
    if (stagedef == NULL || itemgroups == NULL) {
        return;
    }
    r25 = 0;
    r29 = stagedef->collHeaders;
    r30 = itemgroups;
    r31 = 0;
    while (r31 < stagedef->itemGroupCount) {
        r28 = 2;
        if (r29->animLoopType & 2) {
            r28 = 0;
        }
        if (r28 != 2) {
            tr26 = &lbl_1_data_2EF80[r28];
            tr3 = alloc_from_heap_or_panic(currentHeap, sizeof(struct Seesaw), "bbstage.c", 0x251);
            r30->seesaw = tr3;
            memset(tr3, 0, sizeof(struct Seesaw));
            tr3->state = alloc_from_heap_or_panic(currentHeap, tr26->stateSize, "bbstage.c", 0x255);
            memset(tr3->state, 0, tr26->stateSize);
            tr3->itemGroupId = (s16) r31;
            tr3->type = r28;
            tr3->init = tr26->init;
            tr3->dest = tr26->dest;
            tr3->tick = tr26->tick;
            tr3->coli = tr26->coli;
            tr3->func5 = tr26->func5;
            tr3->replay = tr26->replay;
            tr3->init(tr3);
            r25++;
        }
        r31++;
        r30++;
        r29++;
    }
    if (r25 != 0) {
        seesaw_count = (s16) r25;
    }
}

s32 fn_18_12E34(void)
{
    return 0;
}

struct BBModels
{
    u8 unk0;
    s32 (* DPTR unk4)(void);   /* DPTR: overlays lbl_18_bss_2E0.unk1C (BBCam) */
    struct GMAModel * DPTR unk8;
    struct GMAModel * DPTR unkC;
    struct GMAModel * DPTR unk10;
    struct GMAModel * DPTR unk14;
};

void fn_18_12E3C(u32 arg0)
{
    struct BBModels *p = (struct BBModels *) &lbl_18_bss_2E0.unk1C;

    if (arg0 == 0x12D) {
        p->unk4 = fn_18_12E34;
    } else {
        p->unk4 = fn_18_12E34;
    }
}

static inline struct GMAModel *find_model(char *name) {
    struct GMA ***r30 = lbl_18_data_78B48;
    struct GMAModel *r27 = NULL;
    s32 r28;

    while (*r30 != NULL) {
        if (**r30 != NULL) {
            for (r28 = 0; r28 < (s32) (**r30)->numModels; r28++) {
                if (strcmp(name, (**r30)->modelEntries[r28].name) == 0) {
                    r27 = (**r30)->modelEntries[r28].model;
                }
            }
        }
        r30++;
    }
    return r27;
}

void fn_18_12E70(void)
{
    struct BBModels *r29 = (struct BBModels *) &lbl_18_bss_2E0.unk1C;

    r29->unk8 = find_model((char *) lbl_18_data_2988);
    r29->unkC = find_model((char *) lbl_18_data_2854);
    r29->unk10 = find_model((char *) lbl_18_data_296C);
    r29->unk14 = find_model((char *) lbl_18_data_2870);
}

u8 lbl_18_data_78BC8[8][7] = {
    0, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 0, 0,
    1, 1, 1, 1, 1, 1, 1,
    0, 1, 0, 1, 0, 1, 0,
};
s16 lbl_18_data_78C00[8] = { 1, 2, 3, 4, 5, 6, 7, 0 };

void fn_18_130F0(void)
{
    memset(&lbl_18_bss_11A8, 0, 0x3C);
}

void fn_18_13120(u8 arg0)
{
    struct BBGateSet *p = &lbl_18_bss_11A8;

    switch (arg0) {
    case 4:
        p->unk0 = 4;
        break;
    case 5:
        p->unk0 = 5;
        break;
    case 6:
        p->unk0 = 6;
        break;
    case 3:
        p->unk0 = 3;
        break;
    case 1:
        p->unk0 = 1;
        break;
    case 2:
        p->unk0 = 2;
        break;
    default:
        p->unk0 = 0;
        break;
    }
}

void fn_18_131B8(void)
{
    struct BBGateSet *p = &lbl_18_bss_11A8;
    s32 tr0;
    s32 tr7;
    s32 r4;

    for (r4 = 0; r4 < 7; r4++) {
        if (lbl_18_data_78BC8[p->unk0][r4] == 1) {
            if (p->gates[r4].unk1 != 4) {
                tr0 = lbl_18_data_78C00[r4];
                itemgroups[tr0].playbackState &= ~7;
                p->gates[r4].unk0 = 4;
            }
        } else if (p->gates[r4].unk1 != 0) {
            tr7 = lbl_18_data_78C00[r4];
            itemgroups[tr7].playbackState &= ~7;
            itemgroups[tr7].playbackState |= 2;
            p->gates[r4].unk0 = 0;
        }
    }
}

static inline s32 gate_open(s32 id) {
    struct ItemGroup *ig = &itemgroups[id];
    struct StageCollHeader *c = &stagedef->collHeaders[id];

    if (ig->animFrame >= (s32) (lbl_18_rodata_5CC * c->animLoopEnd) - 1) {
        ig->playbackState &= ~7;
        ig->playbackState |= 1;
        return 1;
    }
    return 0;
}

static inline s32 gate_close(s32 id) {
    struct ItemGroup *ig;
    s32 frame;
    struct StageCollHeader *c;
    s32 lim;

    ig = &itemgroups[id];
    c = &stagedef->collHeaders[id];
    frame = ig->animFrame;
#ifdef __MWERKS__
    /* The original reuses the header register for the converted frame. */
    c = (struct StageCollHeader *)(s32) (lbl_18_rodata_5CC * c->animLoopStart);
    lim = (s32)c + 1;
#else
    lim = (s32) (lbl_18_rodata_5CC * c->animLoopStart) + 1;
#endif

    if (frame <= lim) {
        ig->playbackState = (ig->playbackState & ~7) | 1;
        return 1;
    }
    return 0;
}

s32 fn_18_132C8(void)
{
    struct BBGateSet *p = &lbl_18_bss_11A8;
    s32 r3;
    u8 r4;

    r4 = 0;
    for (r3 = 0; r3 < 7; r3++) {
        if ((u8) p->gates[r3].unk0 != p->gates[r3].unk1) {
            r4++;
            switch (p->gates[r3].unk0) {
            case 4:
                if (gate_open(lbl_18_data_78C00[r3]) == 1) {
                    p->gates[r3].unk1 = p->gates[r3].unk0;
                }
                break;
            case 0:
                if (gate_close(lbl_18_data_78C00[r3]) == 1) {
                    p->gates[r3].unk1 = p->gates[r3].unk0;
                }
                break;
            }
        }
    }
    return r4 == 0;
}

