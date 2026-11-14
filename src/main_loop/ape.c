/* Ape models and animation: loading MTA/MAL archives, per-ape setup and the
 * animation state machine. */
#include "types.h"
#include "libc.h"
#include "ball.h"
#include "mathutil.h"
#include "skeleton.h"
#include "avdisp.h"
#include "camera.h"
#include "file.h"
#include "mode.h"
#include "background.h"
#include "dip.h"
#include "frustum.h"
#include "light.h"
#include "macros.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"

#include "ape_internal.h"

struct ApeSlot
{
    void *unk0;
    /*0x04*/ void *skl;
    /*0x08*/ void *mta;
};

struct ArcHandle
{
    u8 unk0[0x1C];
};

struct ArcFile
{
    u8 unk0[0xC];
};

int lbl_1_bss_D4D38;
static int ape_eye_frame_count;
static int ape_eye_timer;
static s32 *ape_eye_frames;
static struct ApeSlot ape_body_slots[17];
static struct ApeSlot ape_face_slots[17];
static struct ApeSlot ape_rhand_slots[17];
static struct ApeSlot ape_tail_slot[1];
static struct ApeSlot ape_lhand_slots[17];
static void *ape_common_mals[17];
static void *ape_scene_mals[17];
static void *ape_skel_arc;
static u32 ape_common_mal_sizes[17];
static u32 ape_scene_mal_sizes[17];
struct TPL *ape_tpls[17][4];
static void *ape_rep_tpls[17];
struct GMA *ape_gma_table[17][4];
static struct ArcHandle ape_arc_handle;
static void *ape_arc_data;
static Mtx ape_mtx;
float g_ape_alphas[0x40];
static int ape_refcounts[17][4][2];
struct Ape *registered_apes[0x40];
int total_apes_registered;

#include "ape_data.inc"

#ifndef __MWERKS__
/* PC port: ape_data.inc was generated as big-endian u32 words. Pointers and
 * floats survive that, but the byte/halfword fields of ApePartEntry, the s8
 * boneMap arrays and the BoneHandle tables end up byte-reversed on a
 * little-endian host. Rewrite them once into native layout. */
static void pc_fix_be_word_bytes(u32 *w)
{
    u32 v = *w;
    u8 *b = (u8 *)w;

    b[0] = v >> 24; b[1] = v >> 16; b[2] = v >> 8; b[3] = v;
}

/* BoneHandle: u8 flags, s8 rot[3] | u16 mirrorBone, u8 unk6[2] */
static void pc_fix_bone_handles(u32 *w, int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        u32 w1 = w[i * 2 + 1];
        u8 *b = (u8 *)&w[i * 2 + 1];

        pc_fix_be_word_bytes(&w[i * 2]);
        *(u16 *)b = w1 >> 16;
        b[2] = w1 >> 8;
        b[3] = w1;
    }
}

#if PC_ADDR32_FIXUPS
/* 64-bit PC port: the u32 tables above hold their pointer words as 0; the
 * generated fixups store the (below-4GiB) addresses.  ApePartEntry and
 * ApePartList use DPTR fields, so they still overlay the 10-word records and
 * g_component_defs exactly.  See pc_port/docs/x64.md. */
#include "ape_data_fixups.inc"
#endif

#define PC_MAX_FIXED_MAPS 256
static void pc_fix_ape_static_data(void)
{
    static int done = 0;
    static u32 *fixedMaps[PC_MAX_FIXED_MAPS];
    static u32 *fixedLists[17 * 4];
    int nMaps = 0, nLists = 0;
    int c, l, e, k;

    if (done)
        return;
    done = 1;
#if PC_ADDR32_FIXUPS
    pc_apply_addr32_fixups(pc_ape_data_fixups,
                           sizeof(pc_ape_data_fixups) / sizeof(pc_ape_data_fixups[0]), "ape_data");
#endif

    for (c = 0; c < 17; c++)
    {
        for (l = 0; l < 4; l++)
        {
            u32 *list = (u32 *)g_component_defs[c][l][0];
            int count = g_component_defs[c][l][1];

            if (list == NULL)
                continue;
            for (k = 0; k < nLists; k++)
                if (fixedLists[k] == list)
                    break;
            if (k < nLists)
                continue;
            fixedLists[nLists++] = list;

            for (e = 0; e < count; e++)
            {
                u32 *ent = list + e * 10; /* sizeof(struct ApePartEntry) == 0x28 */
                u32 w0 = ent[0], w1 = ent[1];
                u8 *b0 = (u8 *)&ent[0];
                u8 *b1 = (u8 *)&ent[1];
                u32 *map = (u32 *)ent[3];

                b0[0] = w0 >> 24;      /* tag */
                b0[1] = w0 >> 16;      /* unk1 */
                *(u16 *)(b0 + 2) = w0; /* flags */
                *(u16 *)b1 = w1 >> 16; /* bone */
                b1[2] = w1 >> 8;
                b1[3] = w1;

                if (map == NULL)
                    continue;
                for (k = 0; k < nMaps; k++)
                    if (fixedMaps[k] == map)
                        break;
                if (k < nMaps || nMaps >= PC_MAX_FIXED_MAPS)
                    continue;
                fixedMaps[nMaps++] = map;
                /* s8 list terminated by -1 */
                for (k = 0;; k++)
                {
                    u32 v = map[k];

                    pc_fix_be_word_bytes(&map[k]);
                    if ((v >> 24) == 0xFF || ((v >> 16) & 0xFF) == 0xFF
                        || ((v >> 8) & 0xFF) == 0xFF || (v & 0xFF) == 0xFF)
                        break;
                }
            }
        }
    }

    /* hand skeleton handles */
    pc_fix_bone_handles(lbl_1_data_65A78, 16);
    /* handle table following the HANDL/HANDR boneMap (bytes 0x14..) */
    pc_fix_bone_handles(&lbl_1_data_6599C[5], 25);
}
#endif

#ifndef __MWERKS__
/* PC port: .mta data is big-endian on disc */
void pc_swap_mta(void *archive);
void pc_swap_ska(void *archive);
void pc_swap_skl(void *data);
void pc_swap_mal(void *data);
#define APE_READ_FILE(dest, line) { u32 size; \
    size = ALIGN32(file_size(&file)); \
    dest = alloc_from_heap_or_panic(currentHeap, size, "new_ape.c", line); \
    DCFlushRange(dest, size + 0x20); \
    file_read(&file, dest, size, 0); \
    file_close(&file); \
    pc_swap_mta(dest); }
#else
#define APE_READ_FILE(dest, line) { u32 size; \
    size = ALIGN32(file_size(&file)); \
    dest = alloc_from_heap_or_panic(currentHeap, size, "new_ape.c", line); \
    DCFlushRange(dest, size + 0x20); \
    file_read(&file, dest, size, 0); \
    file_close(&file); }
#endif

void ape_data_order(void)
{
    lbl_1_bss_D4D38 = (u32)&lbl_1_data_65988;
    lbl_1_bss_D4D38 = (u32)&lbl_1_data_6599C;
    lbl_1_bss_D4D38 = (u32)&lbl_1_data_65A78;
    lbl_1_bss_D4D38 = (u32)&chara_names;
}

/* Nothing calls this and the linker strips it, but compiling it first fixes the
 * layout of the file's BSS pool. */
void ape_bss_order(void)
{
    lbl_1_bss_D4D38 = 0;
    ape_eye_frame_count = 0;
    ape_eye_timer = 0;
    ape_eye_frames = NULL;
    lbl_1_bss_D4D38 = (u32)ape_body_slots;
    lbl_1_bss_D4D38 = (u32)ape_face_slots;
    lbl_1_bss_D4D38 = (u32)ape_rhand_slots;
    lbl_1_bss_D4D38 = (u32)ape_tail_slot;
    lbl_1_bss_D4D38 = (u32)ape_lhand_slots;
    lbl_1_bss_D4D38 = (u32)ape_common_mals;
    lbl_1_bss_D4D38 = (u32)ape_scene_mals;
    lbl_1_bss_D4D38 = (u32)&ape_skel_arc;
    lbl_1_bss_D4D38 = (u32)ape_common_mal_sizes;
    lbl_1_bss_D4D38 = (u32)ape_scene_mal_sizes;
    lbl_1_bss_D4D38 = (u32)ape_tpls;
    lbl_1_bss_D4D38 = (u32)ape_rep_tpls;
    lbl_1_bss_D4D38 = (u32)ape_gma_table;
    lbl_1_bss_D4D38 = (u32)&ape_arc_handle;
    lbl_1_bss_D4D38 = (u32)&ape_arc_data;
    lbl_1_bss_D4D38 = (u32)ape_mtx;
    lbl_1_bss_D4D38 = (u32)g_ape_alphas;
    lbl_1_bss_D4D38 = (u32)ape_refcounts;
    lbl_1_bss_D4D38 = (u32)registered_apes;
    lbl_1_bss_D4D38 = (u32)&total_apes_registered;
}

/* The rodata this stripped function leaves behind comes first in the pool. */
void ape_rodata_order(struct Ape *ape)
{
    struct
    {
        u8 r, g, b, a;
    } color = {0xFF, 0xFF, 0xFF, 0x80};

    ape->unk254 = *(u32 *)&color;
    ape->scale = 1.0f;
}

void ape_rodata_order2(struct Ape *ape)
{
    Vec v1 = {0.0f, 0.0f, 0.0f};
    Vec v2 = {0.0f, 0.0f, 0.0f};
    Vec v3 = {0.0f, 0.0f, 0.0f};
    Quat q1 = {0.0f, 0.0f, -1.0f, 0.0f};
    Quat q2 = {0.0f, 0.0f, 0.0f, 1.0f};

    ape->unk23C = 0.0f;
    ape->pos = v1;
    ape->unk264 = v2;
    ape->unk270 = v3;
    ape->unk2A8 = q1;
    ape->rot = q2;
}

/* Also stripped; its strings stay in the data pool. */
void ape_unused_strings(void)
{
    OSReport("%s_%s.mta");
    OSReport("new_ape.c");
    OSReport("File Not Found");
}

void g_load_ape_mtas(int chara, int scene)
{
    struct File file;
    char path[0x100];
    u32 size;

    if (ape_body_slots[chara].mta == NULL)
    {
        sprintf(path, "%s_%s.mta", chara_names[chara], scene_names[scene]);
        if (file_open(path, &file))
        {
            APE_READ_FILE(ape_body_slots[chara].mta, 0x2FF)
        }
        else
        {
            sprintf(path, "%s_main.mta", chara_names[chara]);
            file_open(path, &file);
            APE_READ_FILE(ape_body_slots[chara].mta, 0x309)
        }
    }
    if (ape_face_slots[chara].mta == NULL)
    {
        sprintf(path, "%s_face.mta", chara_names[chara]);
        if (file_open(path, &file))
        {
            APE_READ_FILE(ape_face_slots[chara].mta, 0x315)
        }
        else
        {
            sprintf(path, "File not found : %s\n", path);
            OSPanic("new_ape.c", 0x31D, path);
        }
    }
    if (ape_rhand_slots[chara].mta == NULL)
    {
        sprintf(path, "%s_hand.mta", chara_names[chara]);
        if (file_open(path, &file))
        {
            APE_READ_FILE(ape_rhand_slots[chara].mta, 0x324)
            ape_archive_offsets_to_pointers(ape_rhand_slots[chara].mta);
        }
        else
        {
            sprintf(path, "File not found : %s\n", path);
            OSPanic("new_ape.c", 0x32E, path);
        }
    }
    if (chara == 4 || chara == 12)
    {
        if (ape_tail_slot[0].mta == NULL)
        {
            sprintf(path, "%s_tail.mta", chara_names[chara]);
            if (file_open(path, &file))
            {
                APE_READ_FILE(ape_tail_slot[0].mta, 0x336)
            }
            else
            {
                sprintf(path, "File not found : %s\n", path);
                OSPanic("new_ape.c", 0x33E, path);
            }
        }
    }
}

void g_init_mtas(struct Ape *ape, int chara, int scene)
{
    g_load_ape_mals(ape, chara, scene);
    ape->unk20 = ape_face_slots[chara].mta;
    g_init_mta(APE_FACE(ape), ape->unk20);
    g_init_skeleton_animation(APE_FACE(ape), NULL, 0);
    ape->bodyMta = ape_rhand_slots[chara].mta;
    g_init_mta((struct Skeleton *)ape->rhandAnim, ape->bodyMta);
    g_init_skeleton_animation((struct Skeleton *)ape->rhandAnim, NULL, 0);
    ape->unk38 = ape_rhand_slots[chara].mta;
    g_init_mta((struct Skeleton *)ape->lhandAnim, ape->unk38);
    g_init_skeleton_animation((struct Skeleton *)ape->lhandAnim, NULL, 0);
    g_init_some_handl_component_stuff((struct Skeleton *)ape->lhandAnim, lbl_1_data_65A78);
    ((struct Skeleton *)ape->lhandAnim)->flags |= 2;
    if (ape->charaId == 4 || ape->charaId == 12)
    {
        ape->handMta = ape_tail_slot[0].mta;
        g_init_mta((struct Skeleton *)ape->unk3C, ape->handMta);
        g_init_skeleton_animation((struct Skeleton *)ape->unk3C, NULL, 0);
    }
    if (ape->charaId == 4 || ape->charaId == 12)
        fn_1_1172C8(APE_DATA(ape), 3, (void *)fn_1_113CEC, ape);
    if (ape->charaId == 4 || ape->charaId == 10 || ape->charaId == 11 || ape->charaId == 12
     || ape->charaId == 15 || ape->charaId == 5 || ape->charaId == 16)
        fn_1_1172C8(APE_DATA(ape), 0, (void *)fn_1_113BDC, ape);
    else if (ape->charaId == 13)
        fn_1_1172C8(APE_DATA(ape), 0, (void *)g_something_with_comparing_strings, ape);
    else
        fn_1_1172C8(APE_DATA(ape), 0, (void *)fn_1_113AF4, ape);
    fn_1_1172C8(APE_DATA(ape), 1, (void *)fn_1_113C50, ape);
    fn_1_1172C8(APE_DATA(ape), 2, (void *)fn_1_113C50, ape);
    if (ape->charaId != 4 && ape->charaId != 10 && ape->charaId != 11 && ape->charaId != 12
     && ape->charaId != 15)
    {
        fn_1_1172C8(APE_DATA(ape), 3, (void *)fn_1_113D68, ape);
        fn_1_1172C8(APE_DATA(ape), 4, (void *)fn_1_113D68, ape);
    }
}

void g_something_freeing_chara_heap_3(struct Ape *ape)
{
    ape->unk14 = 0;
    ape->unk20 = NULL;
    ape->bodyMta = NULL;
    ape->unk38 = NULL;
    ape->handMta = NULL;
    ape->unk10 = NULL;
    ape->unk1C = NULL;
    ape->unk28 = NULL;
    ape->unk34 = NULL;
    ape->unk40 = NULL;
    if (ape->bodyAnim != NULL)
    {
        OSFreeToHeap(currentHeap, ape->bodyAnim);
        ape->bodyAnim = NULL;
    }
    if (ape->faceAnim != NULL)
    {
        OSFreeToHeap(currentHeap, ape->faceAnim);
        ape->faceAnim = NULL;
    }
    if (ape->rhandAnim != NULL)
    {
        OSFreeToHeap(currentHeap, ape->rhandAnim);
        ape->rhandAnim = NULL;
    }
    if (ape->lhandAnim != NULL)
    {
        OSFreeToHeap(currentHeap, ape->lhandAnim);
        ape->lhandAnim = NULL;
    }
    if (ape->unk3C != NULL && (ape->charaId == 4 || ape->charaId == 12))
    {
        OSFreeToHeap(currentHeap, ape->unk3C);
        ape->unk3C = NULL;
    }
    if (ape->unk8 != NULL)
    {
        OSFreeToHeap(currentHeap, ape->unk8);
        ape->unk8 = NULL;
    }
    if (ape->unk4 != NULL)
    {
        OSFreeToHeap(currentHeap, ape->unk4);
        ape->unk4 = NULL;
    }
    if (APE_DATA(ape) != NULL)
    {
        g_something_freeing_heap_3(APE_DATA(ape));
        ape->unk0 = 0;
    }
}

void g_load_ape_mals(struct Ape *ape, int chara, int scene)
{
    struct File file;
    char path[0x108];
    u32 size;
    int heap;

    heap = OSSetCurrentHeap(chara_heap);
    ape->unk14 = (sptr)ape_body_slots[chara].mta;
    if (ape_common_mals[chara] == NULL)
    {
        sprintf(path, "%s_common.mal", chara_names[chara]);
        file_open(path, &file);
        size = ALIGN32(file_size(&file));
        ape_common_mal_sizes[chara] = size;
        ape_common_mals[chara] = alloc_from_heap_or_panic(currentHeap, size, "new_ape.c", 0x41A);
        DCFlushRange(ape_common_mals[chara], size + 0x20);
        file_read(&file, ape_common_mals[chara], size, 0);
        file_close(&file);
#ifndef __MWERKS__
        pc_swap_mal(ape_common_mals[chara]); /* PC port: big-endian on disc */
#endif
    }
    if (ape->unk4 == NULL)
    {
        ape->unk4 = alloc_from_heap_or_panic(currentHeap, ape_common_mal_sizes[chara], "new_ape.c", 0x421);
        DCFlushRange(ape->unk4, ape_common_mal_sizes[chara] + 0x20);
    }
    memcpy(ape->unk4, ape_common_mals[chara], ape_common_mal_sizes[chara]);
    ape->unk0 = (sptr)g_get_structure_from_ape_common_mal_struct(ape->unk4);
    fn_1_11589C(APE_DATA(ape), ape->unk10);
    ape->bodyAnim = g_alloc_some_ape_state_0x8(APE_DATA(ape), chara);
    APE_DATA(ape)->unk0 = chara;
    fn_1_115960(APE_DATA(ape), (struct Mta *)ape->unk14);
    g_ape_anim_unk3(APE_DATA(ape));
    if (ape_scene_mals[chara] == NULL)
    {
        sprintf(path, "%s_%s.mal", chara_names[chara], scene_names[scene]);
        file_open(path, &file);
        size = ALIGN32(file_size(&file));
        ape_scene_mal_sizes[chara] = size;
        ape_scene_mals[chara] = alloc_from_heap_or_panic(currentHeap, size, "new_ape.c", 0x434);
        DCFlushRange(ape_scene_mals[chara], size + 0x20);
        file_read(&file, ape_scene_mals[chara], size, 0);
        file_close(&file);
#ifndef __MWERKS__
        pc_swap_mal(ape_scene_mals[chara]); /* PC port: big-endian on disc */
#endif
    }
    if (ape->unk8 == NULL)
    {
        ape->unk8 = alloc_from_heap_or_panic(currentHeap, ape_scene_mal_sizes[chara], "new_ape.c", 0x43B);
        DCFlushRange(ape->unk8, ape_scene_mal_sizes[chara] + 0x20);
    }
    memcpy(ape->unk8, ape_scene_mals[chara], ape_scene_mal_sizes[chara]);
    fn_1_1152D8(APE_DATA(ape), 0x64);
    fn_1_115094(APE_DATA(ape), ape->unk8);
    OSSetCurrentHeap(heap);
}

/* Stripped as well. */
void ape_unused_strings2(void)
{
    OSReport("%s_%s.tpl");
    OSReport("%s_rep.tpl");
    OSReport("%s_%s.gma");
    OSReport("Start:%d\n");
    OSReport("End  :%d\n");
    OSReport("Max  :%0.2f\n");
    OSReport("Frame:%0.2f\n");
    OSReport("Intrp:%0.2f\n");
}

void *fn_1_10F79C(struct ApeModelTable *table, char *name)
{
    struct ApeModelEntry *entry;
    void *model;
    int best;
    int i;
    int len;

    if (table == NULL)
        return NULL;
    best = 0;
    model = NULL;
    entry = table->entries;
    for (i = table->count; i > 0; i--, entry++)
    {
        if (strstr(entry->name, name) != NULL)
        {
            len = strlen(name);
            if (*(u8 *)(entry->name + len) == 0)
                len++;
            if (len > best)
            {
                model = entry->model;
                best = len;
            }
        }
    }
    return model;
}

static inline void ape_load_skl(char *path, struct ApeSlot *slot)
{
    struct ArcFile info;
    u32 size;

    ARCOpen(&ape_arc_handle, path, &info);
    size = ALIGN32(arcGetLength(&info));
    if (slot->skl == NULL)
        slot->skl = alloc_from_heap_or_panic(currentHeap, size, "new_ape.c", 0x49B);
    DCFlushRange(slot->skl, size + 0x20);
    slot->skl = ARCGetStartAddrInMem(&info);
    return_1(&info);
#ifndef __MWERKS__
    pc_swap_skl(slot->skl); /* PC port: big-endian on disc */
#endif
}

void event_ape_init(void)
{
    struct File file;
    struct ArcFile info;
    char path[0x108];
    u32 size;
    int i;
    struct Ball *ball;

    set_global_ape_LOD_1();
    ball = balls;
    for (i = 0; i < 8; i++, ball++)
    {
        if (ball->status == 0)
            ball->ape = NULL;
    }
    DVDChangeDir("ape");
    if (ape_arc_data == NULL)
    {
        file_open("skl.arc", &file);
        size = ALIGN32(file_size(&file));
        ape_arc_data = alloc_from_heap_or_panic(currentHeap, size, "new_ape.c", 0x52B);
        file_read(&file, ape_arc_data, size, 0);
        file_close(&file);
        ARCInitHandle(ape_arc_data, &ape_arc_handle);
    }
    if (ape_skel_arc == NULL)
    {
        sprintf(path, "skel.ska");
        ARCOpen(&ape_arc_handle, path, &info);
        arcGetLength(&info);
        ape_skel_arc = ARCGetStartAddrInMem(&info);
        return_1(&info);
#ifndef __MWERKS__
        pc_swap_ska(ape_skel_arc); /* PC port: big-endian on disc */
#endif
        ape_archive_offsets_to_pointers(ape_skel_arc);
    }
    for (i = 0; i < 17; i++)
    {
        if (ape_face_slots[i].skl == NULL)
        {
            sprintf(path, "%s_face.skl", chara_names[i]);
            ape_load_skl(path, &ape_face_slots[i]);
        }
        if (ape_rhand_slots[i].skl == NULL)
        {
            sprintf(path, "%s_handr.skl", chara_names[i]);
            ape_load_skl(path, &ape_rhand_slots[i]);
        }
        if (ape_lhand_slots[i].skl == NULL)
        {
            sprintf(path, "%s_handl.skl", chara_names[i]);
            ape_load_skl(path, &ape_lhand_slots[i]);
        }
    }
    if (ape_tail_slot[0].skl == NULL)
    {
        sprintf(path, "%s_tail.skl", chara_names[4]);
        ape_load_skl(path, &ape_tail_slot[0]);
    }
    DVDChangeDir("..");
    mtxa_push();
    mtxa_from_identity();
    mtxa_rotate_z(0x2FA4);
    mtxa_to_mtx(ape_mtx);
    mtxa_pop();
    for (i = 0; i < 0x40; i++)
        g_ape_alphas[i] = 1.0f;
}

void event_ape_tick(void)
{
    int i;
    struct Ball *ball;

    if ((g_some_other_flags & 0xA) == 0)
    {
        ball = balls;
        for (i = 0; i < 8; i++, ball++)
        {
            if (ball->status != 0 && ball->ape != NULL)
            {
                ball->ape->pos = ball->pos;
                if (ball->ape->drawFunc != NULL)
                    ball->ape->drawFunc(ball->ape, 0);
                else
                    ape_default_anim(ball->ape);
            }
        }
    }
}

void event_ape_dest(void)
{
    int i;
    struct Ball *ball;

    ball = balls;
    for (i = 0; i < 8; i++, ball++)
    {
        if (ball->status != 0 && ball->ape != NULL)
        {
            g_something_with_freeing_chara_heap_ape(ball->ape);
            ball->ape = NULL;
        }
    }
}

static inline void ape_init_skeletons(struct Ape *ape, int chara)
{
    ape->unk1C = ape_face_slots[chara].skl;
    if (ape->faceAnim == NULL)
        ape->faceAnim = alloc_from_heap_or_panic(currentHeap, ape_skeleton_size(*(u16 *)ape->unk1C), "new_ape.c", 0x374);
    init_component_state(APE_FACE(ape), (struct Bone *)(APE_FACE(ape) + 1), ape->unk1C);
    ape->unk28 = ape_rhand_slots[chara].skl;
    if (ape->rhandAnim == NULL)
        ape->rhandAnim = alloc_from_heap_or_panic(currentHeap, ape_skeleton_size(*(u16 *)ape->unk28), "new_ape.c", 0x379);
    init_component_state((struct Skeleton *)ape->rhandAnim, (struct Bone *)((struct Skeleton *)ape->rhandAnim + 1), ape->unk28);
    ape->unk34 = ape_lhand_slots[chara].skl;
    if (ape->lhandAnim == NULL)
        ape->lhandAnim = alloc_from_heap_or_panic(currentHeap, ape_skeleton_size(*(u16 *)ape->unk34), "new_ape.c", 0x37E);
    init_component_state((struct Skeleton *)ape->lhandAnim, (struct Bone *)((struct Skeleton *)ape->lhandAnim + 1), ape->unk34);
    g_init_some_handl_component_stuff((struct Skeleton *)ape->lhandAnim, lbl_1_data_65A78);
    ((struct Skeleton *)ape->lhandAnim)->flags |= 2;
    if (ape->charaId == 4 || ape->charaId == 12)
    {
        ape->unk40 = ape_tail_slot[0].skl;
        if (ape->unk3C == NULL)
            ape->unk3C = alloc_from_heap_or_panic(currentHeap, ape_skeleton_size(*(u16 *)ape->unk40), "new_ape.c", 0x386);
        init_component_state((struct Skeleton *)ape->unk3C, (struct Bone *)((struct Skeleton *)ape->unk3C + 1), ape->unk40);
    }
}

struct Ape *load_ape(int chara, int lod, int scene, int mesh)
{
    struct File file;
    char path[0x100];
    struct Ape *ape;
    int idx;
    int heap;
    char *lodName;
    u32 size;

    if (chara == 13)
        mesh = 1;
    heap = OSSetCurrentHeap(chara_heap);
    DVDChangeDir("ape");
    g_load_ape_mtas(chara, scene);
    ape = alloc_from_heap_or_panic(currentHeap, sizeof(*ape), "new_ape.c", 0x619);
    memset(ape, 0, sizeof(*ape));
    ape->charaId = chara;
#ifndef __MWERKS__
    pc_fix_ape_static_data();
#endif
    ape->partList = (struct ApePartList *)&g_component_defs[chara][lod];
    ape->unk10 = ape_skel_arc;
    ape_init_skeletons(ape, chara);
    g_init_mtas(ape, chara, scene);
    idx = lod + (chara << 2);
    lodName = ape_lod_names[mesh][lod];
    if (ape_tpls[0][idx] == NULL)
    {
        sprintf(path, "%s_%s.tpl", chara_names[chara], lodName);
        if (DVDConvertPathToEntrynum(path) != -1)
            ape_tpls[0][idx] = g_load_tpl(path);
    }
    if (ape_rep_tpls[chara] == NULL)
    {
        sprintf(path, "%s_rep.tpl", chara_names[chara]);
        if (file_open(path, &file))
        {
            size = ALIGN32(file_size(&file));
            ape_rep_tpls[chara] = OSAllocFromHeap(currentHeap, size);
            file_read(&file, ape_rep_tpls[chara], size, 0);
            file_close(&file);
        }
    }
    if (ape_gma_table[0][idx] == NULL)
    {
        sprintf(path, "%s_%s.gma", chara_names[chara], lodName);
        if (DVDConvertPathToEntrynum(path) != -1)
            ape_gma_table[0][idx] = g_load_gma(path, ape_tpls[0][idx]);
    }
    ape_refcounts[chara][lod][mesh]++;
    OSSetCurrentHeap(heap);
    ape->cameraVisibleMask = 0;
    ape->flags = 0;
    ape->scale = 1.0f;
    ape->unk29C = 0;
    ape->lod = lod;
    ape->charaId = chara;
    assign_ape_assets(ape, lod, mesh);
    ape->unk23C = 0.0f;
    ape->unk240 = 0.0f;
    ape->unk24C = 0;
    ape->unk250 = 1;
    ape->unk254 = 0;
    ape->unk2A4 = 0;
    ape->unk2B8 = 0;
    ape->unk27C = 0;
    {
        Vec v1 = {0.0f, 0.0f, 0.0f};
        Vec *p = &v1;
        ape->pos = *p;
    }
    {
        Vec v2 = {0.0f, 0.0f, 0.0f};
        Vec *p = &v2;
        ape->unk264 = *p;
    }
    {
        Vec v3 = {0.0f, 0.0f, 0.0f};
        Vec *p = &v3;
        ape->unk270 = *p;
    }
    {
        Quat q1 = {0.0f, 0.0f, -1.0f, 0.0f};
        Quat *p = &q1;
        ape->unk2A8 = *p;
    }
    {
        Quat q2 = {0.0f, 0.0f, 0.0f, 1.0f};
        Quat *p = &q2;
        ape->rot = *p;
    }
    ape->unk74 = scene;
    ape->unk78 = -1;
    ape->unk7C = -1;
    ape->unk60 = -1.0f;
    ape->unk214 = -1.0f;
    ape->rhandId = 0xFFFF;
    ape->lhandId = 0xFFFF;
    ape->faceExpr = 0xFFFF;
    ape->lhandBlend = 0.0f;
    ape->rhandBlend = 0.0f;
    ape->unkA2 = 0;
    ape->unkA0 = 0;
    ape->unkB0 = 0.0f;
    ape->unkB4 = 0.0f;
    ape->unkB8 = 0.0f;
    ape->unkBC = 0.0f;
    ape->meshType = mesh;
    memset(ape->unk2CC + 0, 0xFF, 0xC);
    memset(ape->unk2CC + 1, 0xFF, 0xC);
    memset(ape->unk2CC + 2, 0xFF, 0xC);
    register_ape(ape);
    DVDChangeDir("..");
    return ape;
}

struct Ape *g_load_ape_variant0(int chara, int lod, int scene)
{
    return load_ape(chara, lod, scene, 0);
}

#pragma auto_inline off
struct Ape *load_ape_gameplay(int chara, int lod, int mesh)
{
    int scene;

    switch (modeCtrl.gameType)
    {
    case 3:
        scene = lbl_1_data_65AF8[12];
        break;
    case 5:
        scene = lbl_1_data_65AF8[14];
        break;
    case 6:
        scene = lbl_1_data_65AF8[17];
        break;
    case 7:
        scene = lbl_1_data_65AF8[16];
        break;
    case 8:
        scene = lbl_1_data_65AF8[15];
        break;
    case 9:
        scene = lbl_1_data_65AF8[23];
        break;
    case 10:
        scene = lbl_1_data_65AF8[18];
        break;
    case 12:
        scene = lbl_1_data_65AF8[21];
        break;
    case 13:
        scene = lbl_1_data_65AF8[19];
        break;
    case 15:
        scene = lbl_1_data_65AF8[22];
        break;
    case 16:
        scene = lbl_1_data_65AF8[20];
        break;
    default:
        scene = lbl_1_data_65AF8[1];
        break;
    }
    if (world_theme.bgId == 0x1E)
        scene = lbl_1_data_65AF8[17];
    if (g_check_flag_related_to_pausing() != 0)
        scene = lbl_1_data_65AF8[26];
    return load_ape(chara, lod, scene, mesh);
}

struct Ape *g_load_ape_gameplay_variant0(int chara, int lod)
{
    return load_ape_gameplay(chara, lod, 0);
}
#pragma auto_inline on

struct ApeNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ int lightGroup;
    /*0x0C*/ struct Ape *ape;
    /*0x10*/ float alpha;
};

struct ApeLinePoint
{
    /*0x00*/ Vec pos;
    /*0x0C*/ u32 color;
};

extern u32 monkey_flags;
extern u32 g_some_render_flag;
extern struct
{
    u32 unk0[6];
    u32 unk18;
} perfInfo;

void draw_ape_subroutine(struct OrdTblNode *arg)
{
    struct ApeNode *node = (struct ApeNode *)arg;

    g_draw_ape(node->ape, node->alpha);
}

static inline struct Bone *ape_part_bone(struct Skeleton *skel, struct ApePartEntry *part)
{
    return &skel->bones[part->bone];
}

static inline struct Bone *ape_part_joint(struct Skeleton *skel, struct ApePartEntry *part, int j)
{
    return &skel->bones[part->boneMap[j]];
}

void g_draw_ape(struct Ape *ape, float alpha)
{
    Mtx mtxs[50];
    Mtx apeMtx;
    struct ApeLinePoint line[2];
    Vec center;
    float *m;
    int i;
    struct ApePartList *list;
    struct ApePartEntry *part;
    void *model;
    struct ApeAnim *skel;
    int j;
    int count;
    void *alt;
    void *tex;

    list = ape->partList;
    if (list == NULL)
        return;
    if (!(monkey_flags & 0x40))
        return;

    perf_init_timer(8);
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(apeMtx);
    mtxa_from_mtxb();
    mtxa_translate(&ape->pos);
    mtxa_scale_s(ape->scale);
    mtxa_mult_right(apeMtx);
    if (!(ape->flags & 0x01000000) && ape->charaId <= 3)
        mtxa_translate_xyz(0.0f, -0.12f, 0.0f);
    mtxa_to_mtx(apeMtx);
    m = g_get_ape_anim_smth(ape, 0);
    center.x = m[3];
    center.y = m[7];
    center.z = m[11];
    if (!(ape->flags & 0x04000000) && ape->charaId != 14)
    {
        if (!g_is_sphere_visible_scaled(&center, 0.5f, ape->scale))
            return;
    }
    if (ape->colorIndex != 0 && ape_rep_tpls[ape->charaId] != NULL)
        tex = fn_80065C94(ape_rep_tpls[ape->charaId], ape->colorIndex - 1);
    else
        tex = NULL;
    fn_1_113F64(ape, tex);
    if (ape->flags & 0x00100000)
        set_post_mult_color(1.0f, 1.0f, 1.0f, alpha);
    if (dip_switches & 0x00200000)
    {
        line[0].pos = current_ball->unk10C;
        line[1].pos = current_ball->ape->pos;
        {
            struct { u8 r, g, b, a; } c = {0x00, 0xFF, 0xFF, 0xFF};
            u32 *p = (u32 *)&c;
            line[0].color = *p;
        }
        {
            struct { u8 r, g, b, a; } c = {0xFF, 0x00, 0xFF, 0xFF};
            u32 *p = (u32 *)&c;
            line[1].color = *p;
        }
        mtxa_from_mtxb();
        fn_1_E2FD0(0x18);
        fn_1_E2FE0(1, 1, 1, 0);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        fn_1_E349C(&line[0], &line[1]);
    }
    mtxa_push();
    for (i = 0; i < list->count; i++)
    {
        part = &list->entries[i];
        model = ape->partModels[i];
        if (model == NULL)
            continue;
        if (part->tag == 2 && (ape->flags & 0x02000000))
            continue;
        if (part->tag == 0xE && (ape->unk21C & 2))
            continue;
        if (part->tag == 0x11 && (ape->unk21C & 4))
            continue;
        if (part->tag == 0x13 && (ape->unk21C & 0x10))
            continue;
        if (part->tag == 0x12 && (ape->unk21C & 8))
            continue;
        if (part->tag == 0xA && ape->unk220 != 0)
            continue;
        if (part->tag == 0xB && !(ape->unk220 & 2))
            continue;
        if (part->tag == 0xC && !(ape->unk220 & 4))
            continue;
        if (part->tag == 0xD && !(ape->unk220 & 8))
            continue;
        if (part->tag == 0x14 && !(ape->unk224 & 2))
            continue;
        if (part->tag == 0x15 && !(ape->unk224 & 4))
            continue;
        if (part->tag == 0x16 && !(ape->unk224 & 8))
            continue;
        if (part->tag == 0x17 && !(ape->unk224 & 0x10))
            continue;
        if (part->tag == 0xF && !(ape->unk224 & 0x20))
            continue;
        if (part->tag == 0x10 && !(ape->unk224 & 0x40))
            continue;
        mtxa_from_mtx(apeMtx);
        if (part->bone != 0)
            mtxa_mult_right(ape_part_bone(APE_BODY(ape), part)->mtx);
        mtxa_translate(&part->pos);
        mtxa_rotate_z(182.04445f * part->rot.z);
        mtxa_rotate_y(182.04445f * part->rot.y);
        mtxa_rotate_x(182.04445f * part->rot.x);
        if (part->boneMap != NULL)
        {
            count = seek(part->boneMap);
            switch (part->tag)
            {
            case 0:
                skel = ape->bodyAnim;
                break;
            case 8:
                skel = ape->unk3C;
                break;
            case 1:
                skel = ape->faceAnim;
                break;
            case 6:
                skel = ape->lhandAnim;
                break;
            case 7:
                skel = ape->rhandAnim;
                break;
            default:
                skel = ape->bodyAnim;
                break;
            }
            for (j = 0; j < count; j++)
            {
                if (part->boneMap[j] >= 0)
                    mtx_copy(ape_part_joint((struct Skeleton *)skel, part, j)->mtx, mtxs[j]);
            }
            if (ape->lod == 2 || ape->lod == 3)
            {
                switch (part->tag)
                {
                case 1:
                    draw_poly(model, NULL);
                    break;
                case 6:
                    draw_poly(model, NULL);
                    break;
                case 7:
                    draw_poly(model, NULL);
                    break;
                default:
                    pointer_range_advance(model, mtxs);
                    break;
                }
            }
            else
                pointer_range_advance(model, mtxs);
        }
        else
        {
            if (part->tag == 1 && ape->lod >= 2)
            {
                if (ape->faceExpr < 9)
                {
                    alt = ape->faceModels[ape->faceExpr];
                    if (alt != NULL)
                        model = alt;
                }
            }
            else if (part->tag == 6 && ape->lod >= 2)
            {
                if ((u16)ape->unk218 == ape->lhandId && (ape->unkA0 & 2))
                    alt = ape->handModels[1];
                else if (ape->lhandBlend > 0.5 && !(ape->unkA0 & 2))
                    alt = ape->handModels[1];
                else
                    alt = ape->handModels[0];
                if (alt != NULL)
                    model = alt;
            }
            else if (part->tag == 7 && ape->lod >= 2)
            {
                if ((u16)ape->unk218 == ape->rhandId && (ape->unkA2 & 2))
                    alt = ape->handModels[3];
                else if (ape->rhandBlend > 0.5 && !(ape->unkA2 & 2))
                    alt = ape->handModels[3];
                else
                    alt = ape->handModels[2];
                if (alt != NULL)
                    model = alt;
            }
            else if (part->tag == 4)
            {
                mtxa_rotate_z(182.04445f * ape->unkB4);
                if (ape->unkA8 > 0)
                {
                    if (ape->unkA8 >= 8)
                        mtxa_rotate_z(1820.0 * (1.0 - (ape->unkA8 - 8) / 8.0f));
                    else
                        mtxa_rotate_z(1820.0f * ape->unkA8 / 8.0f);
                }
            }
            else if (part->tag == 5)
            {
                mtxa_rotate_z(182.04445f * ape->unkB8);
                if (ape->unkA8 > 0)
                {
                    if (ape->unkA8 >= 8)
                        mtxa_rotate_z(-1820.0 * (1.0 - (ape->unkA8 - 8) / 8.0f));
                    else
                        mtxa_rotate_z(-1820.0f * ape->unkA8 / 8.0f);
                }
            }
            else if (part->tag == 3)
            {
                if (ape->charaId == 0)
                {
                    if (ape->unkAC > 0)
                    {
                        alt = ape->eyeModels[ape->unkAC];
                        if (alt != NULL)
                            model = alt;
                    }
                }
                else if (ape->charaId == 13)
                {
                    int expr = ape->faceExpr;

                    if (expr != ape_baby_eye_expr || ape_baby_eye_expr == -1)
                    {
                        ape_eye_frame_count = *ape_baby_eye_counts[expr];
                        ape_eye_frames = ape_baby_eye_frames[expr];
                        ape_baby_eye_expr = expr;
                    }
                    alt = ape->eyeModelsAlt[ape_eye_frames[lbl_1_bss_D4D38]];
                    if (alt != NULL)
                        model = alt;
                }
                else if (ape->unkC0 != 0)
                {
                    int frame = (float)lbl_1_data_6A8CC[0] * ape->unkB0 / 100.0;

                    if (ape->unkC0 == 3)
                        frame = lbl_1_data_6A8CC[0];
                    alt = ape->eyeModelsAlt[frame];
                    if (alt != NULL)
                        model = alt;
                }
                else if (ape->unkAC > 0)
                {
                    alt = ape->eyeModels[ape->unkAC];
                    if (alt != NULL)
                        model = alt;
                }
            }
            draw_poly(model, NULL);
        }
        if (part->flags & 1)
            avdisp_set_z_mode(1, 3, 0);
        g_smth_with_gpu_and_locked_cache_mtx();
        if ((part->flags & 2) && !(ape->flags & 0x40000000))
        {
            float t = (float)((part->flags & 0xF000) >> 12) / 3.1;

            g_smth_with_rendefc_reflective_height(t * ape->scale);
            if (ape->flags & 0x00100000)
                avdisp_draw_model_unculled_sort_never(model);
            else
                avdisp_draw_model_unculled_sort_always(model);
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        else
            avdisp_draw_model_unculled_sort_never(model);
        if (part->flags & 1)
            avdisp_set_z_mode(1, 3, 1);
    }
    if (ape->flags & 0x00100000)
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    fn_1_113FF8(ape);
    if (dip_switches & 0x00800000)
        dip_apenum_draw(ape);
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(apeMtx);
    mtxa_pop();
    perfInfo.unk18 += perf_stop_timer(8);
}

void draw_ape(struct Ape *ape)
{
    struct ApeNode *node;
    struct OrdTblNode *entry;

    if (ape == NULL)
        return;
    if (ape->cameraVisibleMask & (1 << g_current_camera->idx))
        return;
    if (ape->flags & 0x20)
        return;
    if ((g_some_render_flag & 4) && fn_1_80D4(&ape->pos) < 0.0f)
        return;
    if (g_ape_alphas[ape->apeIdx] < 1.0f || (ape->flags & 0x00100000))
    {
        mtxa_from_mtxb();
        entry = ord_tbl_get_entry_for_pos(&ape->pos);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = draw_ape_subroutine;
            node->lightGroup = get_active_light_group();
            node->ape = ape;
            node->alpha = g_ape_alphas[ape->apeIdx];
            fn_1_CD418(entry, &node->node);
        }
    }
    else
        g_draw_ape(ape, 1.0f);
}

static inline void ape_free_chara_anims(int arg)
{
    int chara = arg;

    if (ape_rhand_slots[chara].mta != NULL)
    {
        OSFreeToHeap(currentHeap, ape_rhand_slots[chara].mta);
        ape_rhand_slots[chara].mta = NULL;
    }
    if (ape_lhand_slots[chara].mta != NULL)
    {
        OSFreeToHeap(currentHeap, ape_lhand_slots[chara].mta);
        ape_lhand_slots[chara].mta = NULL;
    }
    if (ape_face_slots[chara].mta != NULL)
    {
        OSFreeToHeap(currentHeap, ape_face_slots[chara].mta);
        ape_face_slots[chara].mta = NULL;
    }
    if (ape_body_slots[chara].mta != NULL)
    {
        OSFreeToHeap(currentHeap, ape_body_slots[chara].mta);
        ape_body_slots[chara].mta = NULL;
    }
    if (chara == 4 || chara == 12)
    {
        if (ape_tail_slot->mta != NULL)
            OSFreeToHeap(currentHeap, ape_tail_slot->mta);
        ape_tail_slot->mta = NULL;
    }
    if (ape_common_mals[chara] != NULL)
        OSFreeToHeap(currentHeap, ape_common_mals[chara]);
    if (ape_scene_mals[chara] != NULL)
        OSFreeToHeap(currentHeap, ape_scene_mals[chara]);
    ape_common_mals[chara] = NULL;
    ape_scene_mals[chara] = NULL;
}

void g_something_with_freeing_chara_heap_ape(struct Ape *ape)
{
    int heap;
    int total;
    int i;

    if (ape == NULL)
        return;
    unregister_ape(ape);
    if (ape_refcounts[ape->charaId][ape->lod][ape->meshType] <= 0)
    {
        heap = OSSetCurrentHeap(chara_heap);
        ape->partList = NULL;
        g_something_freeing_chara_heap_3(ape);
        OSFreeToHeap(currentHeap, ape);
        OSSetCurrentHeap(heap);
        return;
    }
    ape_refcounts[ape->charaId][ape->lod][ape->meshType]--;
    total = 0;
    for (i = 0; i < 4; i++)
    {
        total += ape_refcounts[ape->charaId][i][0];
        total += ape_refcounts[ape->charaId][i][1];
    }
    heap = OSSetCurrentHeap(chara_heap);
    ape->partList = NULL;
    if (ape_refcounts[ape->charaId][ape->lod][ape->meshType] <= 0)
        g_something_freeing_chara_heap_2(ape->charaId, ape->lod, ape->meshType);
    if (total <= 0)
        ape_free_chara_anims(ape->charaId);
    g_something_freeing_chara_heap_3(ape);
    OSFreeToHeap(currentHeap, ape);
    OSSetCurrentHeap(heap);
}

void fn_1_1118BC(void)
{
    int i;

    for (i = 0; i < 0x40; i++)
    {
        struct Ape *ape = get_ape_by_idx(i);

        if (ape != NULL)
            g_something_with_freeing_chara_heap_ape(ape);
    }
}

void ape_default_anim(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    int free;

    if (!(g_some_other_flags & 0xA) && ape != NULL && stagedef != NULL)
    {
        ape->flags &= ~0x13;
        run_anim_funcs(ape);
        free = (ball->flags & 0x1000) != 0;
        free |= (ape->flags & 3) == 0;
        g_ape_upright(ape);
        if (free)
            fn_1_45A8C(ape);
        else
        {
            mtxa_from_quat(&ape->rot);
            mtxa_normalize_basis();
            if (ape->flags & 2)
                fn_1_46958(ape);
        }
        mtxa_to_quat(&ape->rot);
        g_ape_anim_body(ape);
        if (!(ape->flags & 8))
            g_move_ape_into_ball(ape);
        g_ape_anim_head(ape, &ball->unk10C);
        ball->unk108 = 0;
        ball->unk118 = 0.0f;
    }
}

#define APE_FIT_OUT(field, out) \
    if (APE_BODY(ape)->blendFrame < APE_BODY(ape)->blendLength \
     && __fabs(prev - ape->field) > 3.0) \
    { \
        if (prev > ape->field) \
            ape->field = prev - 3.0f; \
        else \
            ape->field = 3.0f + prev; \
    }

void g_ape_anim_body(struct Ape *ape)
{
    struct ApeAnimOut out;
    struct ApeAnimOut out2;
    float len;
    float prev;
    float cur;
    int expr;
    struct ApeClipInfo *clip;
    struct Skeleton *body;

    clip = g_get_ape_flag(APE_DATA(ape));
    body = APE_BODY(ape);
    if (ape != NULL && APE_DATA(ape) != NULL && APE_DATA(ape)->skel != NULL && body != NULL
     && !(ape->flags & 0x10000000))
    {
        len = fn_1_1136E0(ape);
        prev = body_frame_add72(ape);
        cur = fn_1_1135E8(ape);
        cur += prev;
        if (FLT_EPSILON + cur >= len)
            ape->flags |= 0x2000;
        if (!(APE_BODY(ape)->blendFrame < APE_BODY(ape)->blendLength && -1.0f == ape->unk60
              && !(ape->flags & 0x20000000)))
        {
            if (ape->unk6C != ape->unk78 || ape->unk70 != ape->unk7C)
            {
                ape->rhandId = 0xFFFF;
                ape->lhandId = 0xFFFF;
                ape->faceExpr = 0xFFFF;
                ape->unk92 = 0xFFFF;
                ape->unkA2 = 0;
                ape->unkA0 = 0;
                ape->unkA4 = 0;
                ape->flags &= ~0x20000000;
                g_set_ape_dance_state2(APE_DATA(ape), ape->unk6C, ape->unk70);
                if (ape->unk78 == -1 && ape->unk7C == -1)
                    fn_1_113768(ape, 0.0f);
                ape->flags &= ~0x2000;
                ape->unk74 = ape->unk68;
                ape->unk78 = APE_DATA(ape)->curGroup;
                ape->unk7C = (APE_DATA(ape)->curGroup < APE_DATA(ape)->count
                                  ? &APE_DATA(ape)->groups[APE_DATA(ape)->curGroup]
                                  : &APE_DATA(ape)->groupsAlt[APE_DATA(ape)->curGroup - APE_DATA(ape)->unk4C])->current;
            }
        }
        if (ape->unk60 >= 0.0)
        {
            APE_BODY(ape)->blendFrame = 0.0f;
            APE_BODY(ape)->blendLength = ape->unk60;
            ape->unk60 = -1.0f;
        }
        if (ape->unk214 >= 0.0)
        {
            APE_BODY(ape)->clock.frame = ape->unk214;
            ape->unk214 = -1.0f;
        }
        APE_DATA(ape)->skel->flags |= 0x10;
        fn_1_117018(APE_DATA(ape));
        APE_DATA(ape)->skel->flags &= ~0x10;
        expr = 0;
        if (ape->lod < 2)
        {
            if (ape->faceAnim != NULL)
            {
                struct ApeAnimOut *last;

                g_ape_anim_unk4(APE_DATA(ape), 0, &out);
                last = &ape->unk2CC[0];
                if ((u16)last->a != out.a || (u16)last->b != out.b || last->t != out.t)
                {
                    if ((u16)out.a == out.b)
                        fn_1_10BFE8(APE_FACE(ape), 0, out.b * 10, out.t);
                    else
                        fn_1_10BFE8(APE_FACE(ape), out.a * 10, out.b * 10, out.t);
                }
                expr = out.b;
                ape->faceExpr = out.b;
                ape->unkB0 = 100.0f * out.t;
                ape->unk2CC[0] = out;
            }
            if ((ape->charaId == 4 || ape->charaId == 12) && ape->unk3C != NULL)
            {
                g_ape_anim_unk4(APE_DATA(ape), 3, &out);
                if ((u16)ape->unk92 != out.b)
                {
                    g_init_skeleton_animation((struct Skeleton *)ape->unk3C, NULL, out.b != 0 ? (u16)(out.b - 1) : ape->unk66);
                    ape->unk92 = out.b;
                    ape->unkA4 = out.flags;
                }
                if (ape->unkA4 & 2)
                    fn_1_10BE3C((struct Skeleton *)ape->unk3C);
                else
                    fn_1_10BFE8((struct Skeleton *)ape->unk3C, 0, 100, out.t);
            }
            if (ape->lhandAnim != NULL)
            {
                struct ApeAnimOut *last;

                g_ape_anim_unk4(APE_DATA(ape), 1, &out);
                last = &ape->unk2CC[1];
                if ((u16)last->b != out.b || (u16)last->flags != out.flags || last->t != out.t
                 || (ape->unkA0 & 2))
                {
                    if ((u16)ape->lhandId != out.b)
                    {
                        g_init_skeleton_animation((struct Skeleton *)ape->lhandAnim, NULL, out.b != 0 ? (u16)(out.b - 1) : ape->unk64);
                        ape->lhandId = out.b;
                        ape->unkA0 = out.flags;
                    }
                    if (ape->unkA0 & 2)
                        fn_1_10BE3C((struct Skeleton *)ape->lhandAnim);
                    else
                        fn_1_10BFE8((struct Skeleton *)ape->lhandAnim, 0, 100, out.t);
                }
                ape->unk2CC[1] = out;
                ape->lhandId = out.b;
                ape->unkA0 = out.flags;
                ape->lhandBlend = out.t;
            }
            if (ape->rhandAnim != NULL)
            {
                struct ApeAnimOut *last;

                g_ape_anim_unk4(APE_DATA(ape), 2, &out);
                last = &ape->unk2CC[2];
                if ((u16)last->b != out.b || (u16)last->flags != out.flags || last->t != out.t
                 || (ape->unkA2 & 2))
                {
                    if ((u16)ape->rhandId != out.b)
                    {
                        g_init_skeleton_animation((struct Skeleton *)ape->rhandAnim, NULL, out.b != 0 ? (u16)(out.b - 1) : ape->unk64);
                        ape->rhandId = out.b;
                        ape->unkA2 = out.flags;
                    }
                    if (ape->unkA2 & 2)
                        fn_1_10BE3C((struct Skeleton *)ape->rhandAnim);
                    else
                        fn_1_10BFE8((struct Skeleton *)ape->rhandAnim, 0, 100, out.t);
                }
                ape->unk2CC[2] = out;
                ape->rhandId = out.b;
                ape->unkA2 = out.flags;
                ape->rhandBlend = out.t;
            }
            if (ape->charaId != 4 && ape->charaId != 10 && ape->charaId != 11
             && ape->charaId != 12 && ape->charaId != 15)
            {
                prev = ape->unkB4;
                g_ape_anim_unk4(APE_DATA(ape), 3, &out);
                ape->unkB4 = 100.0f * out.t;
                APE_FIT_OUT(unkB4, out)
                prev = ape->unkB8;
                g_ape_anim_unk4(APE_DATA(ape), 4, &out);
                ape->unkB8 = 100.0f * out.t;
                APE_FIT_OUT(unkB8, out)
                if (out.b == 0 && ape->unkA8 == 0 && RAND_FLOAT() < 1.0 / (lbl_80198B38 * 3))
                    ape->unkA8 = lbl_1_data_6A8CC[0];
            }
        }
        else if (ape->lod >= 2)
        {
            if (ape->faceAnim != NULL)
            {
                ape->faceExpr = 0;
                g_ape_anim_unk4(APE_DATA(ape), 0, &out2);
                expr = out2.b;
                ape->faceExpr = out2.b;
                ape->unkB0 = 100.0f * out2.t;
                if (ape->charaId == 13 && ape_eye_timer != g_font_char_var_3)
                {
                    lbl_1_bss_D4D38 = (lbl_1_bss_D4D38 + 1) % ape_eye_frame_count;
                    ape_eye_timer = g_font_char_var_3;
                }
            }
            if (ape->lhandAnim != NULL)
            {
                g_ape_anim_unk4(APE_DATA(ape), 1, &out2);
                ape->lhandId = out2.b;
                ape->unkA0 = out2.flags;
                ape->lhandBlend = out2.t;
            }
            if (ape->rhandAnim != NULL)
            {
                g_ape_anim_unk4(APE_DATA(ape), 2, &out2);
                ape->rhandId = out2.b;
                ape->unkA2 = out2.flags;
                ape->rhandBlend = out2.t;
            }
            prev = ape->unkB4;
            g_ape_anim_unk4(APE_DATA(ape), 3, &out2);
            ape->unkB4 = 100.0f * out2.t;
            APE_FIT_OUT(unkB4, out2)
            prev = ape->unkB8;
            g_ape_anim_unk4(APE_DATA(ape), 4, &out2);
            ape->unkB8 = 100.0f * out2.t;
            APE_FIT_OUT(unkB8, out2)
            if (out2.b == 0 && ape->unkA8 == 0 && RAND_FLOAT() < 1.0 / (lbl_80198B38 * 3))
                ape->unkA8 = lbl_1_data_6A8CC[0];
        }
        if ((ape->lod == 2 || ape->lod == 3) && ape->unkA8 == 0
         && RAND_FLOAT() < 1.0 / (lbl_80198B38 * 3))
            ape->unkA8 = lbl_1_data_6A8CC[0];
        if (ape->unkAC == 0 && RAND_FLOAT() < 1.0 / (lbl_80198B38 * 2))
            ape->unkAC = lbl_1_data_6A8CC[0];
        if (expr == 1 && (ape->charaId == 1 || ape->charaId == 2 || ape->charaId == 3))
        {
            if (ape->unkC0 == 2)
                ape->unkC0 = 3;
            else if (ape->unkC0 == 3 && ape->unkB0 > 70.0f)
            {
                ape->unkC0 = 1;
                ape->unkAC = lbl_1_data_6A8CC[0];
            }
            else if (ape->unkC0 == 0)
                ape->unkC0 = 1;
        }
        else if (expr == 6 && (ape->charaId == 1 || ape->charaId == 2 || ape->charaId == 3))
        {
            if (ape->unkC0 == 1)
                ape->unkC0 = 3;
            else if (ape->unkC0 == 3 && ape->unkB0 > 70.0f)
            {
                ape->unkC0 = 2;
                ape->unkAC = lbl_1_data_6A8CC[0];
            }
            else if (ape->unkC0 == 0)
                ape->unkC0 = 1;
        }
        else
        {
            if (ape->unkC0 == 1)
                ape->unkAC = 7;
            ape->unkC0 = 0;
        }
        if (ape->unkAC == 0 && RAND_FLOAT() < 1.0 / (lbl_80198B38 * 2))
            ape->unkAC = lbl_1_data_6A8CC[0];
        if (ape->unkC0 != 0)
        {
            if (ape->unkAC < lbl_1_data_6A8CC[0])
                ape->unkAC++;
        }
        else if (ape->unkAC > 0)
            ape->unkAC--;
        if (ape->unkA8 > 0)
            ape->unkA8--;
        fn_1_10BF98(APE_DATA(ape)->skel);
    }
}

/* Stripped too; only its literal stays in the pool. */
float ape_unused_literal(void)
{
    return 216.0f;
}

void g_load_mal_files_from_disc(int chara, int lod, int mesh, int scene)
{
    char malPath[0x100];
    char path[0x100];
    struct File malFile;
    struct File file;
    int heap;
    char *lodName;
    int idx;
    int idx2;
    u32 size;

    idx2 = lod + (chara << 2);
    if (ape_gma_table[0][idx2] == NULL || ape_tpls[0][idx2] == NULL)
    {
        heap = OSSetCurrentHeap(chara_heap);
        DVDChangeDir("ape");
        g_load_ape_mtas(chara, scene);
        idx = lod + (chara << 2);
        lodName = ape_lod_names[mesh][lod];
        if (ape_tpls[0][idx] == NULL)
        {
            sprintf(path, "%s_%s.tpl", chara_names[chara], lodName);
            if (DVDConvertPathToEntrynum(path) != -1)
                ape_tpls[0][idx] = g_load_tpl(path);
        }
        if (ape_rep_tpls[chara] == NULL)
        {
            sprintf(path, "%s_rep.tpl", chara_names[chara]);
            if (file_open(path, &file))
            {
                u32 repSize = ALIGN32(file_size(&file));

                ape_rep_tpls[chara] = OSAllocFromHeap(currentHeap, repSize);
                file_read(&file, ape_rep_tpls[chara], repSize, 0);
                file_close(&file);
            }
        }
        if (ape_gma_table[0][idx] == NULL)
        {
            sprintf(path, "%s_%s.gma", chara_names[chara], lodName);
            if (DVDConvertPathToEntrynum(path) != -1)
                ape_gma_table[0][idx] = g_load_gma(path, ape_tpls[0][idx]);
        }
        if (ape_common_mals[chara] == NULL)
        {
            sprintf(malPath, "%s_common.mal", chara_names[chara]);
            file_open(malPath, &malFile);
            size = ALIGN32(file_size(&malFile));
            ape_common_mal_sizes[chara] = size;
            ape_common_mals[chara] = alloc_from_heap_or_panic(currentHeap, size, "new_ape.c", 0xA64);
            DCFlushRange(ape_common_mals[chara], size + 0x20);
            file_read(&malFile, ape_common_mals[chara], size, 0);
            file_close(&malFile);
#ifndef __MWERKS__
            pc_swap_mal(ape_common_mals[chara]); /* PC port: big-endian on disc */
#endif
        }
        if (ape_scene_mals[chara] == NULL)
        {
            sprintf(malPath, "%s_%s.mal", chara_names[chara], scene_names[scene]);
            file_open(malPath, &malFile);
            size = ALIGN32(file_size(&malFile));
            ape_scene_mal_sizes[chara] = size;
            ape_scene_mals[chara] = alloc_from_heap_or_panic(currentHeap, size, "new_ape.c", 0xA6E);
            DCFlushRange(ape_scene_mals[chara], size + 0x20);
            file_read(&malFile, ape_scene_mals[chara], size, 0);
            file_close(&malFile);
#ifndef __MWERKS__
            pc_swap_mal(ape_scene_mals[chara]); /* PC port: big-endian on disc */
#endif
        }
        ape_refcounts[chara][lod][mesh]++;
        DVDChangeDir("..");
        OSSetCurrentHeap(heap);
    }
}

void g_something_freeing_chara_heap_4(int chara, int lod, int mesh)
{
    int heap;
    int total;
    int i;

    if (ape_refcounts[chara][lod][mesh] <= 0)
        return;
    heap = OSSetCurrentHeap(chara_heap);
    ape_refcounts[chara][lod][mesh]--;
    total = 0;
    for (i = 0; i < 4; i++)
    {
        total += ape_refcounts[chara][i][0];
        total += ape_refcounts[chara][i][1];
    }
    if (ape_refcounts[chara][lod][mesh] <= 0)
        g_something_freeing_chara_heap_2(chara, lod, mesh);
    if (total <= 0)
        ape_free_chara_anims(chara);
    OSSetCurrentHeap(heap);
}

void g_something_freeing_chara_heap_2(int chara, int lod, int mesh)
{
    int idx = lod + (chara << 2);
    int heap;

    heap = OSSetCurrentHeap(chara_heap);
    gp_wait_wrapper();
    if (ape_gma_table[0][idx] != NULL)
    {
        g_something_freeing_heap(ape_gma_table[0][idx]);
        ape_gma_table[0][idx] = NULL;
    }
    if (ape_rep_tpls[chara] != NULL)
    {
        OSFreeToHeap(currentHeap, ape_rep_tpls[chara]);
        ape_rep_tpls[chara] = NULL;
    }
    if (ape_tpls[0][idx] != NULL)
    {
        g_something_freeing_heap_2(ape_tpls[0][idx]);
        ape_tpls[0][idx] = NULL;
    }
    OSSetCurrentHeap(heap);
}

void assign_ape_assets(struct Ape *ape, int lod, int mesh)
{
    int chara = ape->charaId;

    if (lod < 0 || lod >= 4)
        return;
    ape->modelTable = (struct ApeModelTable *)ape_gma_table[0][lod + (chara << 2)];
    ape->tpl = ape_tpls[0][lod + (chara << 2)];
#ifndef __MWERKS__
    pc_fix_ape_static_data();
#endif
    ape->partList = (struct ApePartList *)&g_component_defs[chara][lod];
    assign_ape_models(ape);
    ape->lod = lod;
}

#ifdef __MWERKS__
void fn_1_113190(struct Ape *ape, int b, int c, int d)
#else
/* Every caller outside ape.c (ball.c, baseball, boat) passes an unused
 * float between b and c (fn_1_113190(ape, b, speed, c, d)).  On PPC the
 * float travels in f1 and the ints keep their GPRs, so the 4-int definition
 * works; on x86 every argument is on the stack, so take the float here. */
static void ape_set_anim(struct Ape *ape, int b, int c, int d);

void fn_1_113190(struct Ape *ape, int b, float unused, int c, int d)
{
    (void)unused;
    ape_set_anim(ape, b, c, d);
}

static void ape_set_anim(struct Ape *ape, int b, int c, int d)
#endif
{
    struct ApeClipInfo *info;

    switch (lbl_1_data_65AF8[b])
    {
    case 0:
        ape->unk68 = 0;
        ape->unk6C = c;
        ape->unk70 = d;
        info = (struct ApeClipInfo *)fn_1_1157E4(APE_DATA(ape), ape->unk6C);
        if (APE_DATA(ape)->count <= ape->unk6C)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        else if (info->unk8 <= ape->unk70)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        break;
    default:
        ape->unk68 = (s16)b;
        ape->unk6C = c + 100;
        ape->unk70 = d;
        info = (struct ApeClipInfo *)fn_1_1157E4(APE_DATA(ape), ape->unk6C);
        if (APE_DATA(ape)->countAlt <= c)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        else if (info->unk8 <= ape->unk70)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        break;
    }
}

void fn_1_1132D0(struct Ape *ape, int b, int c, int d)
{
    struct ApeClipInfo *info;

    switch (lbl_1_data_65AF8[b])
    {
    case 0:
        ape->unk68 = 0;
        ape->unk6C = c;
        ape->unk70 = d;
        info = (struct ApeClipInfo *)fn_1_1157E4(APE_DATA(ape), ape->unk6C);
        if (APE_DATA(ape)->count <= ape->unk6C)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        else if (info->unk8 <= ape->unk70)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        break;
    default:
        ape->unk68 = (s16)b;
        ape->unk6C = c + 100;
        ape->unk70 = d;
        info = (struct ApeClipInfo *)fn_1_1157E4(APE_DATA(ape), ape->unk6C);
        if (APE_DATA(ape)->countAlt <= c)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        else if (info->unk8 <= ape->unk70)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        break;
    }
}

void fn_1_113410(struct Ape *ape, int b, int c, int d)
{
    struct ApeClipInfo *info;

    ape->unk68 = (s16)b;
    ape->unk6C = c;
    ape->unk70 = d;
    if (lbl_1_data_65AF8[b] != 0)
        ape->unk6C += 100;
    info = (struct ApeClipInfo *)fn_1_1157E4(APE_DATA(ape), ape->unk6C);
    if (lbl_1_data_65AF8[b] != 0)
    {
        if (APE_DATA(ape)->countAlt <= c)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        else if (info->unk8 <= ape->unk70)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
    }
    else
    {
        if (APE_DATA(ape)->count <= c)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 2;
        }
        else if (info->unk8 <= ape->unk70)
        {
            ape->unk68 = 0;
            ape->unk6C = 7;
            ape->unk70 = 4;
        }
    }
}

void fn_1_11355C(struct Ape *ape, float speed)
{
    if (lbl_80198B3C == 1)
        speed *= 1.2;
    APE_BODY(ape)->speed = speed;
}

void *fn_1_113590(struct Ape *ape)
{
    return ape_bone(APE_BODY(ape), 8)->mtx;
}

void *fn_1_1135A4(struct Ape *ape)
{
    return ape_bone(APE_BODY(ape), 12)->mtx;
}

void *g_get_ape_anim_smth(struct Ape *ape, int bone)
{
    if (bone == 14)
        return APE_FACE(ape)->bones->mtx;
    else
        return ape_bone(APE_BODY(ape), bone)->mtx;
}

float fn_1_1135E8(struct Ape *ape)
{
    return APE_BODY(ape)->clock.frame;
}

float body_frame_add72(struct Ape *ape)
{
    return APE_BODY(ape)->clock.speed;
}

void fn_1_113600(struct Ape *ape, float frame)
{
    APE_BODY(ape)->clock.frame = frame;
}

void fn_1_11360C(struct Ape *ape, float x)
{
    ape->unk214 = x;
}

float fn_1_113614(struct Ape *ape, int alt, int a, int b)
{
    struct ApeData *data = APE_DATA(ape);
    struct ApeClipInfo *info;
    int idx;
    struct MtaClip *clip;

    if (alt != 0)
        a += 100;
    info = fn_1_1156F0(data, a, b);
    idx = g_table_index(APE_BODY(ape)->mta,
                        alt != 0 ? data->namesAlt + info->nameOff : data->names + info->nameOff);
    {
        struct MtaClip *c = APE_BODY(ape)->mta->clips[idx];

        clip = c;
    }
    return clip->length;
}

float fn_1_1136E0(struct Ape *ape)
{
    return APE_BODY(ape)->clock.length;
}

int fn_1_1136EC(struct Ape *ape)
{
    float frame = APE_BODY(ape)->clock.frame;
    float length = APE_BODY(ape)->clock.length;
    float speed = APE_BODY(ape)->clock.speed;

    return length - frame - speed < 1.1920929e-7f;
}

int fn_1_113728(struct Ape *ape)
{
    if (APE_BODY(ape)->blendFrame < APE_BODY(ape)->blendLength && -1.0f == ape->unk60)
        return 0;
    return 1;
}

void fn_1_113768(struct Ape *ape, float x)
{
    ape->unk60 = x;
}

char *get_ape_scene_name(struct Ape *ape)
{
    if (ape == NULL)
        return NULL;
    if (ape->unk74 == -1 || ape->unk74 >= 0x2B)
        return NULL;
    if (ape->unk78 >= 100)
        return scene_names[ape->unk74];
    return scene_names[0];
}

int fn_1_1137D8(struct Ape *ape)
{
    struct ApeClipInfo *info;

    if (ape == NULL)
        return 0;
    if (APE_DATA(ape) == NULL)
        return 0;
    info = (struct ApeClipInfo *)fn_1_1157E4(APE_DATA(ape), ape->unk78);
    if (info != NULL)
        return *(int *)info;
    return 0;
}

char *get_ape_anim_flags(struct Ape *ape)
{
    struct ApeClipInfo *info;

    if (ape == NULL)
        return NULL;
    if (APE_DATA(ape) == NULL)
        return NULL;
    info = g_get_ape_flag(APE_DATA(ape));
    if (info != NULL)
    {
        if (ape->unk78 >= 100)
            return APE_DATA(ape)->namesAlt + info->flagsOff;
        else
            return APE_DATA(ape)->names + info->flagsOff;
    }
    return NULL;
}

/* Stripped too. */
void ape_unused_strings3(void)
{
    OSReport("The memory for apes cannot be allocated.");
    OSReport("madh_tail.mta");
}

void queue_disc_read(char *file);
void g_queue_ape_mta_mal_load(int scene, int chara);

void preload_ape(int chara, int lod, int mesh, int scene)
{
    char path[0x100];

    DVDChangeDir("ape");
    g_queue_ape_mta_mal_load(scene, chara);
    sprintf(path, "%s_%s.gma", chara_names[chara], ape_lod_names[mesh][lod]);
    queue_disc_read(path);
    sprintf(path, "%s_%s.tpl", chara_names[chara], ape_lod_names[mesh][lod]);
    queue_disc_read(path);
    sprintf(path, "%s_rep.tpl", chara_names[chara]);
    queue_disc_read(path);
    DVDChangeDir("..");
}

void g_queue_ape_mta_mal_load(int scene, int chara)
{
    char path[0x100];

    sprintf(path, "%s_%s.mta", chara_names[chara], scene_names[scene]);
    queue_disc_read(path);
    sprintf(path, "%s_face.mta", chara_names[chara]);
    queue_disc_read(path);
    sprintf(path, "%s_hand.mta", chara_names[chara]);
    queue_disc_read(path);
    switch (chara)
    {
    case 4:
    case 12:
        queue_disc_read("madh_tail.mta");
        break;
    }
    sprintf(path, "%s_common.mal", chara_names[chara]);
    queue_disc_read(path);
    sprintf(path, "%s_%s.mal", chara_names[chara], scene_names[scene]);
    queue_disc_read(path);
}

u16 fn_1_113AF4(struct Ape *ape, char *name)
{
    u32 i;

    for (i = 0; i < 9; i++)
    {
        if (strcmp(name, ape_face_enum_alt[i]) == 0)
            return i;
    }
    return 0;
}

u16 g_something_with_comparing_strings(struct Ape *ape, char *name)
{
    u32 i;

    for (i = 0; i < 6; i++)
    {
        if (strcmp(name, lbl_1_data_6AFC4[i]) == 0)
            return i;
    }
    return 0;
}

u16 fn_1_113BDC(struct Ape *ape, char *name)
{
    u32 i;

    for (i = 0; i < 10; i++)
    {
        if (strcmp(name, ape_face_expr_names[i]) == 0)
            return i;
    }
    return 0;
}

u16 fn_1_113C50(struct Ape *a, char *name)
{
    struct Ape *ape = a;

    ape->unk64 = g_table_index(ape->bodyMta, lbl_1_data_6B094[ape->charaId]);
    ape->unk218 = g_table_index(ape->bodyMta, lbl_1_data_6B178[ape->charaId]) + 1;
    return g_table_index(ape->bodyMta, name) + 1;
}


u16 fn_1_113CEC(struct Ape *a, char *name)
{
    struct Ape *ape = a;

    ape->unk66 = g_table_index(ape->handMta, "MAD_TAIL_Default");
    if (ape->unk66 == 0xFFFF)
        OSReport("not fount default tail: TAIL_Default\n");
    return g_table_index(ape->handMta, name) + 1;
}

int fn_1_113D68(void)
{
    return 1;
}

void register_ape(struct Ape *ape)
{
    struct Ape **p = registered_apes;
    int i;

    for (i = 0; i < 0x40; i++, p++)
    {
        if (*p == NULL)
        {
            *p = ape;
            ape->apeIdx = i;
            ape->unk88 = total_apes_registered++;
            return;
        }
    }
    OSPanic("new_ape.c", 0xD93, "ERROR: ape registration over");
}

void unregister_ape(struct Ape *ape)
{
    registered_apes[ape->apeIdx] = NULL;
}

struct Ape *get_ape_by_idx(int idx)
{
    return registered_apes[idx];
}


void dip_apenum_draw(struct Ape *ape)
{
    static s16 digitModels[10] = {78, 79, 80, 81, 82, 83, 84, 85, 86, 87};
    u8 buf[16];

    sprintf((char *)buf, "%02d", ape->apeIdx);
    mtxa_from_mtxb_translate(&ape->pos);
    mtxa_translate_xyz(0.0f, ape->scale, 0.0f);
    mtxa_rotate_y(g_current_camera->rotY);
    mtxa_scale_s(0.5 * ape->scale);
    mtxa_push();
    mtxa_translate_xyz(-1.595f, -3.9f, 0.0f);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[digitModels[buf[0] - '0']].model);
    mtxa_pop();
    mtxa_translate_xyz(-1.155f, -3.9f, 0.0f);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[digitModels[buf[1] - '0']].model);
}

static inline void *ape_find_tag0(struct Ape *ape)
{
    struct ApePartList *list;
    void *model = NULL;
    int i;

    list = ape->partList;
    for (i = 0; i < list->count; i++)
    {
        struct ApePartEntry *e = &list->entries[i];

        if (e->tag == 0)
        {
            model = ape->partModels[i];
            break;
        }
    }
    return model;
}

void fn_1_113F64(struct Ape *ape, void *x)
{
    if (ape->colorIndex > 0 && x != NULL)
    {
        void *model = ape_find_tag0(ape);

        if (model != NULL)
            fn_80066C98(model, 5, x);
    }
}

void fn_1_113FF8(struct Ape *ape)
{
    if (ape->colorIndex > 0)
    {
        void *model = ape_find_tag0(ape);

        if (model != NULL)
            fn_80066C98(model, 5, NULL);
    }
}

char *get_ape_chara_name(struct Ape *ape)
{
    int id = ape->charaId;

    if (id < 0 || id >= sizeof(chara_names) / sizeof(chara_names[0]))
        return "null";
    return chara_names[id];
}

char *get_ape_lod_string(struct Ape *ape)
{
    int id = ape->lod;

    if (id < 0 || id >= sizeof(ape_lod_enum) / sizeof(ape_lod_enum[0]))
        return "null";
    return ape_lod_enum[id];
}

char *g_get_ape_skinning_or_stitching_name(struct Ape *ape)
{
    int id = ape->meshType;

    if (id < 0 || id >= sizeof(g_mesh_type_name) / sizeof(g_mesh_type_name[0]))
        return "null";
    return g_mesh_type_name[id];
}

char *get_ape_face_expr_name(struct Ape *ape)
{
    int expr = ape->faceExpr;

    if (expr == 0xFFFF)
        expr = 0;
    switch (ape->charaId)
    {
    default:
        return ape_face_enum_alt[expr];
    case 4:
    case 5:
    case 10:
    case 11:
    case 12:
    case 15:
    case 16:
        return ape_face_expr_names[expr];
    }
}

int seek(s8 *p)
{
    int i = 0;

    while (p[i++] != -1)
        ;
    return i - 1;
}

void fn_1_1141D8(struct Ape *ape, int b, char *name)
{
    int r = fn_1_1145B8(APE_DATA(ape), ape->unk14, name);

#ifdef __MWERKS__
    if (r == -1)
        fn_1_113190(ape, b, 0, 0);
    else
        fn_1_113190(ape, b, 0, r);
#else
    if (r == -1)
        ape_set_anim(ape, b, 0, 0);
    else
        ape_set_anim(ape, b, 0, r);
#endif
    if (b == 0x1E && r == 3 && ape->charaId == 0)
    {
        Quat q = {0.00923200045f, -0.338063985f, -0.941078007f, 0.0f};
        Quat *p = &q;

        ape->unk2A8 = *p;
        printf("FORCE_FACE_DIR\n");
    }
    if (b == 0x1E && r == 1 && ape->charaId == 4)
    {
        Quat q = {0.480069011f, -0.351745009f, -0.803623021f, 0.0f};
        Quat *p = &q;

        ape->unk2A8 = *p;
        printf("FORCE_FACE_DIR\n");
    }
    if (b == 0x1F && r == 0xC && ape->charaId == 0)
    {
        Quat q = {0.315030009f, -0.243231997f, -0.917384982f, 0.0f};
        Quat *p = &q;

        ape->unk2A8 = *p;
        printf("FORCE_FACE_DIR\n");
    }
}

int fn_1_1145B8(struct ApeData *data, int unused, char *name)
{
    int i;
    struct ApeClipGroup *info = fn_1_1157E4(data, 100);

    for (i = 0; i < info->count; i++)
    {
        struct ApeClipInfo *c = fn_1_1156F0(data, 100, i);

        if (strcmp(data->namesAlt + c->nameOff, name) == 0)
            return i;
    }
    return -1;
}

int fn_1_11465C(struct Ape *ape, int a, int b)
{
    if (lbl_1_data_65AF8[a] != 0)
        return ape->unk78 == b + 100;
    return ape->unk78 == b;
}

int fn_1_1146A4(struct Ape *ape, int a, int b)
{
    return ape->unk7C == b;
}

static inline void *ape_find_model(struct ApeModelTable *table, char *name)
{
    struct ApeModelEntry *e;
    void *found;
    int best;
    int n;

    if (table == NULL)
        return NULL;
    best = 0;
    found = NULL;
    e = table->entries;
    n = table->count;
    for (; n > 0; n--, e++)
    {
        if (strstr(e->name, name) != NULL)
        {
            int len = strlen(name);

            if (*(u8 *)(e->name + len) == 0)
                len++;
            if (len > best)
            {
                found = e->model;
                best = len;
            }
        }
    }
    return found;
}

void assign_ape_models(struct Ape *ape)
{
    int i;

    for (i = 0; i < 0x28; i++)
        ape->partModels[i] = NULL;
    for (i = 0; i < ape->partList->count; i++)
        ape->partModels[i] = ape_find_model(ape->modelTable, ape->partList->entries[i].name);
    for (i = 0; i < 0xF; i++)
        ape->eyeModels[i] = NULL;
    for (i = 0; i < 0xF; i++)
        ape->eyeModels[i] = ape_find_model(ape->modelTable, eye_string_table[i]);
    for (i = 0; i < 0xF; i++)
        ape->eyeModelsAlt[i] = NULL;
    for (i = 0; i < 0xF; i++)
        ape->eyeModelsAlt[i] = ape_find_model(ape->modelTable, lbl_1_data_6AC10[ape->charaId][i]);
    for (i = 0; i < 0xA; i++)
        ape->faceModels[i] = NULL;
    for (i = 0; i < 9; i++)
        ape->faceModels[i] = ape_find_model(ape->modelTable, ape_face_names[i]);
    ape->handModels[0] = ape_find_model(ape->modelTable, "HANDL_GHA");
    ape->handModels[1] = ape_find_model(ape->modelTable, "HANDL_PHA");
    ape->handModels[2] = ape_find_model(ape->modelTable, "HANDR_GHA");
    ape->handModels[3] = ape_find_model(ape->modelTable, "HANDR_PHA");
}
