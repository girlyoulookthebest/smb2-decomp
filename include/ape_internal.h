#ifndef APE_INTERNAL_H
#define APE_INTERNAL_H

#include "types.h"
#include "ball.h"
#include "skeleton.h"

struct ApeTrack
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 count;
    /*0x02*/ u16 root;
};  /* size = 0x4 */

/* Node of the per-bone binary search tree of events, keyed by frame. */
struct ApeEvent
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 key;
    /*0x04*/ s16 value;
    /*0x06*/ u8 type;
    u8 unk7;
    /*0x08*/ u32 kind;
    /*0x0C*/ u16 left;
    /*0x0E*/ u16 right;
};  /* size = 0x10 */

/* Run-time animation clip; either one packed array or a linked list. */
struct ApeClipInfo
{
    /*0x00*/ u16 flags;
    /*0x02*/ u16 unk2;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 unkC;
    /*0x0E*/ u16 unkE;
    /*0x10*/ u16 unk10;
    u8 unk12[2];
    /*0x14*/ float unk14;
    /*0x18*/ float unk18;
    /*0x1C*/ u32 nameOff;
    /*0x20*/ u32 unk20;
    /*0x24*/ u32 flagsOff;
    /*0x28*/ struct ApeTrack * DPTR tracks;
    /*0x2C*/ struct ApeClipInfo * DPTR prev;
    /*0x30*/ struct ApeClipInfo * DPTR next;
};  /* size = 0x34 */

struct ApeClipGroup
{
    /*0x00*/ char * DPTR name;
    /*0x04*/ struct ApeClipInfo * DPTR clips;
    /*0x08*/ u16 count;
    /*0x0A*/ u16 current;
};  /* size = 0xC */

/* The object at Ape::unk0: per-character animation tables. */
struct ApeData
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 boneCount;
    /*0x02*/ u16 flags;
    /*0x04*/ struct Mta * DPTR mta;
    /*0x08*/ struct Skeleton * DPTR skel;
    /*0x0C*/ struct ApeClipInfo * DPTR clips;
    /*0x10*/ u16 clipCount;
    /*0x12*/ u16 curGroup;
    /*0x14*/ char * DPTR names;
    /*0x18*/ u32 unk18;
    /*0x1C*/ struct Mta * DPTR skelData;
    /*0x20*/ struct ApeEvent * DPTR events;
    /*0x24*/ u16 eventCount;
    /*0x26*/ u8 count;
    /*0x27*/ u8 countAlt;
    /*0x28*/ struct ApeClipGroup * DPTR groups;
    /*0x2C*/ struct ApeEvent * DPTR unk2C;
    /*0x30*/ struct ApeEvent * DPTR unk30;
    /*0x34*/ struct ApeClipGroup * DPTR groupsAlt;
    /*0x38*/ struct ApeClipInfo * DPTR clipsAlt;
    /*0x3C*/ struct ApeEvent * DPTR unk3C;
    /*0x40*/ char * DPTR namesAlt;
    /*0x44*/ u32 unk44;
    /*0x48*/ u16 clipCountAlt;
    /*0x4A*/ u16 unk4A;
    /*0x4C*/ u8 unk4C;
    u8 unk4D[0x60 - 0x4D];
};  /* size = 0x60 */

/* On-disc MAL archive. */
struct ApeMalClip
{
    /*0x00*/ u16 unk0;
    u8 unk2[2];
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 unkC;
    /*0x0E*/ u16 unkE;
    /*0x10*/ float unk10;
    /*0x14*/ float unk14;
    /*0x18*/ u32 unk18;
    /*0x1C*/ u32 nameOff;
    /*0x20*/ u32 flagsOff;
};  /* size = 0x24 */

struct ApeMalGroup
{
    /*0x00*/ u16 firstClip;
    /*0x02*/ u16 count;
    /*0x04*/ u32 nameOff;
};  /* size = 0x8 */

struct ApeMal
{
    /*0x00*/ float version;
    /*0x04*/ u16 clipCount;
    /*0x06*/ u8 groupCount;
    /*0x08*/ u32 clipsOff;
    /*0x0C*/ u32 framesOff;
    /*0x10*/ u16 unk10;
    /*0x14*/ u32 unk14;
    /*0x18*/ u32 unk18;
    /*0x1C*/ u32 namesOff;
    /*0x20*/ u8 unk20;
    /*0x21*/ u8 boneCount;
    /*0x22*/ u16 flags;
    /*0x24*/ u32 groupsOff;
};

struct ApePartEntry
{
    /*0x00*/ s8 tag;
    u8 unk1;
    /*0x02*/ u16 flags;  /* 1: no z update, 2: reflective, 0xF000: reflection height */
    /*0x04*/ u16 bone;
    u8 unk6[2];
    /*0x08*/ char * DPTR name;
    /*0x0C*/ s8 * DPTR boneMap;
    /*0x10*/ Vec rot;
    /*0x1C*/ Vec pos;
};

struct ApePartList
{
    /*0x00*/ struct ApePartEntry * DPTR entries;
    /*0x04*/ int count;
};

struct ApeModelEntry
{
    /*0x00*/ void * DPTR model;
    /*0x04*/ char * DPTR name;
};

struct ApeModelTable
{
    /*0x00*/ int count;
    u8 unk4[4];
    /*0x08*/ struct ApeModelEntry * DPTR entries;
};

#define APE_DATA(ape) ((struct ApeData *)(ape)->unk0)
#define APE_BODY(ape) ((struct Skeleton *)(ape)->bodyAnim)
#define APE_FACE(ape) ((struct Skeleton *)(ape)->faceAnim)

extern u32 lbl_80198B3C;

struct ApeClipGroup *fn_1_1157E4(struct ApeData *data, int a);
struct ApeClipInfo *fn_1_1156F0(struct ApeData *data, int a, int b);
struct ApeClipInfo *fn_1_117C64(struct ApeClipInfo *clip, u8 b);
u16 fn_1_1177A4(struct ApeData *d, u16 root, u16 n, u16 count, struct ApeEvent *out);
u16 fn_1_117868(struct ApeEvent *events, u16 count);
char *fn_1_10DFDC(struct Mta *mta, int idx);
int fn_1_117D44(struct ApeData *d, char *name);
void init_component_state(struct Skeleton *skel, struct Bone *bones, void *data);
void g_init_mta(struct Skeleton *skel, struct Mta *mta);
void g_init_skeleton_animation(struct Skeleton *skel, void *info, u16 clipIdx);
void g_set_ape_stuff_in_chara_heap(struct ApeEvent *dst, struct ApeEvent *src);
struct ApeEvent *g_some_ape_anim_func2(struct ApeData *d, int bone);
struct ApeEvent *fn_1_1175A0(struct ApeData *d, int bone);
void g_unk_ape_anim1(struct Skeleton *skel, struct AnimClock *info, u16 clipIdx, u16 blendLength, u16 flags);
void fn_1_10BE3C(struct Skeleton *skel);
void ape_archive_offsets_to_pointers(void *);
extern int currentHeap;
void *OSAllocFromHeap(int heap, u32 size);
void OSFreeToHeap(int heap, void *ptr);
struct ApeClipInfo *g_get_ape_flag(struct ApeData *data);
int g_table_index(void *, char *);
int strcmp(const char *, const char *);
void OSReport(const char *, ...);
char *strstr(const char *, const char *);
void g_smth_with_gpu_and_locked_cache_mtx(void);
void mtxa_push(void);
void mtxa_pop(void);
void fn_1_113F64(struct Ape *ape, void *x);
void fn_1_113FF8(struct Ape *ape);
void dip_apenum_draw(struct Ape *ape);
void *g_get_ape_anim_smth(struct Ape *ape, int bone);
int seek(s8 *p);
void *fn_80065C94(void *tpl, int idx);
void draw_poly(void *model, void *mtxs);
void pointer_range_advance(void *model, void *mtxs);
void g_smth_with_rendefc_reflective_height(float h);
void GXLoadPosMtxImm(Mtx m, u32 id);
void GXLoadNrmMtxImm(Mtx m, u32 id);
void fn_1_E2FD0(int width);
void fn_1_E2FE0(int a, int b, int c, int d);
void fn_1_E349C(void *a, void *b);
float fn_1_80D4(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void g_draw_ape(struct Ape *ape, float alpha);
void fn_80066C98(void *model, int a, void *b);
int fn_1_1145B8(struct ApeData *data, int unused, char *name);
void OSPanic(const char *, int, const char *, ...);
void g_load_ape_mals(struct Ape *ape, int chara, int scene);
void *alloc_from_heap_or_panic(int heap, u32 size, char *file, int line);
void DCFlushRange(void *addr, u32 size);
int OSSetCurrentHeap(int heap);
extern int chara_heap;
void ARCInitHandle(void *data, void *handle);
int ARCOpen(void *handle, char *path, void *info);
u32 arcGetLength(void *info);
void *ARCGetStartAddrInMem(void *info);
int return_1(void *info);
void g_init_some_handl_component_stuff(struct Skeleton *skel, void *a);
struct TPL *g_load_tpl(char *file);
struct GMA *g_load_gma(char *file, struct TPL *tpl);
s32 DVDConvertPathToEntrynum(char *path);
void set_global_ape_LOD_1(void);
void ape_default_anim(struct Ape *ape);
void assign_ape_assets(struct Ape *ape, int lod, int mesh);
void register_ape(struct Ape *ape);
void g_something_freeing_heap_3(struct ApeData *d);
struct ApeData *g_get_structure_from_ape_common_mal_struct(struct ApeMal *mal);
void fn_1_11589C(struct ApeData *d, void *a);
void *g_alloc_some_ape_state_0x8(struct ApeData *d, int idx);
void fn_1_115960(struct ApeData *d, struct Mta *mta);
void g_ape_anim_unk3(struct ApeData *d);
void fn_1_1152D8(struct ApeData *d, u8 a);
void fn_1_115094(struct ApeData *d, struct ApeMal *mal);
void fn_1_1172C8(struct ApeData *d, int type, int (*func)(void *, char *), void *arg);
u16 fn_1_113AF4(struct Ape *ape, char *name);
u16 g_something_with_comparing_strings(struct Ape *ape, char *name);
u16 fn_1_113BDC(struct Ape *ape, char *name);
u16 fn_1_113C50(struct Ape *a, char *name);
u16 fn_1_113CEC(struct Ape *a, char *name);
int fn_1_113D68(void);
void g_something_with_freeing_chara_heap_ape(struct Ape *ape);
void assign_ape_models(struct Ape *ape);
void g_something_freeing_chara_heap_2(int chara, int lod, int mesh);
void g_something_freeing_chara_heap_3(struct Ape *ape);
void unregister_ape(struct Ape *ape);
struct Ape *get_ape_by_idx(int idx);
void run_anim_funcs(struct Ape *ape);
void g_ape_upright(struct Ape *ape);
float fn_1_45A8C(struct Ape *ape);
void fn_1_46958(struct Ape *ape);
void g_ape_anim_body(struct Ape *ape);
void g_move_ape_into_ball(struct Ape *ape);
void g_ape_anim_head(struct Ape *ape, Vec *v);
float fn_1_1136E0(struct Ape *ape);
float body_frame_add72(struct Ape *ape);
float fn_1_1135E8(struct Ape *ape);
void fn_1_113768(struct Ape *ape, float x);
void g_set_ape_dance_state2(struct ApeData *d, int group, int b);
void fn_1_117018(struct ApeData *d);
void g_ape_anim_unk4(struct ApeData *d, int bone, struct ApeAnimOut *out);
void fn_1_10BFE8(struct Skeleton *skel, u16 a, u16 b, float t);
void fn_1_10BF98(struct Skeleton *skel);
void gp_wait_wrapper(void);
void g_something_freeing_heap(void *p);
void g_something_freeing_heap_2(void *p);
extern u32 g_font_char_var_3;
extern int lbl_80198B38;

static inline struct Bone *ape_bone(struct Skeleton *skel, int idx)
{
    return &skel->bones[idx];
}

static inline u32 ape_skeleton_size(u16 boneCount)
{
    return boneCount * sizeof(struct Bone) + sizeof(struct Skeleton);
}

#define ALIGN32(x) (((x) + 0x1F) & ~0x1F)

#endif
