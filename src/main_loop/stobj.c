/* Stage objects: bumpers, jamabars and buttons (SMB1 stobj.c). */
#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "frustum.h"
#include "input.h"
#include "item.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "stage.h"
#include "stobj.h"
#include "world.h"
#include "pc_hfr.h"

void g_something_with_translating_items(int type, Vec *pos, float weight);
void call_SoundReqID_arg_0(int soundId);
void *g_yet_another_unk_draw_func(void *func);
void mtxa_sq_from_mtx(Mtx mtx);
void mtxa_from_mtxb_translate(Vec *vec);
BOOL g_smth_with_buttons(int animGroupId, int type);
void g_smth_with_stage_anim_groups(int animGroupId, int type);
void g_stores_doubles2(float r, float g, float b, float a);
void collide_ball_with_cylinder(struct PhysicsBall *ball, struct StageColiCylinder *cylinder);
void GXLoadPosMtxImm(Mtx m, int id);
void GXLoadNrmMtxImm(Mtx m, int id);
int debug_window_printf(int buf, char *fmt, ...);
void debug_textdraw_put(int buf, char *str);
void g_rumble_controller(int pad, int a, int b);
float g_lava_bumper_func1(void);
void stobj_jamabar_child_coli(struct PhysicsBall *physBall, struct Stobj *stobj);
BOOL g_is_ball_in_ig_coli_range(struct ItemGroup *group, struct StageCollHeader *coll, int id, struct Sphere *sphere);
int strcmp(const char *a, const char *b);
#ifdef __MWERKS__
unsigned long strlen(const char *s);
#endif
extern struct
{
    /*0x00*/ u32 flags;
} g_some_render_flag;
extern u32 monkey_flags;
extern struct GMA *stage_gma;
extern struct GMA *g_bg_gma;
extern u32 g_font_char_var_3;
extern s32 g_smth_with_sound;
extern struct
{
    u8 unk0[0xC];
    /*0x0C*/ u32 stobjTime;
} perfInfo;

struct StageBumper
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    u8 unk12[2];
    /*0x14*/ Vec scale;
};  /* size = 0x20 */

struct StageJamabar
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    u8 unk12[2];
    /*0x14*/ Vec scale;
};  /* size = 0x20 */

struct StageButton
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    /*0x12*/ s16 type;
    /*0x14*/ s16 animGroupId;
    u8 unk16[2];
};  /* size = 0x18 */

struct BumperLODs
{
    /*0x00*/ float lods[4];
    /*0x10*/ int modelCount;
    /*0x14*/ struct GMAModel *models[4];
};  /* size = 0x24 */

struct StobjFuncs
{
    void (*init)(struct Stobj *);
    void (*tick)(struct Stobj *);
    void (*disp)(struct Stobj *);
    void (*coli)(struct Stobj *, struct PhysicsBall *);
    void (*dest)(struct Stobj *);
    void (*cbF)(struct Stobj *);
};

void event_stobj_collision_child_tick(void);
BOOL g_some_item_collision_check(Vec *a, Vec *b, float e, Vec *c, Vec *d, float f);
BOOL fn_1_A74E4(Vec *a, Vec *b, float e, Vec *c, Vec *d, float f);
void g_something_with_item_coli(Vec *a, Vec *b, float d, Vec *c, float e);
void g_draw_stobjs(void);
void fn_1_A7E30(void);
void g_stobj_sound_something(void);
int spawn_stobj(struct Stobj *template);
void fn_1_A81A0(struct Stobj *stobj);
void g_something_with_bumper_LODs(void);
void fn_1_A8628(void);
void load_bumper_stobjs(struct StageCollHeader *group, int count);
void load_jamabar_stobjs(struct StageCollHeader *group, int count);
void load_button_stobjs(struct StageCollHeader *group, int count);
void load_goaltape_and_goalbag_stobjs(struct StageCollHeader *group, int count);
void load_returngate_stobjs(struct StageCollHeader *group, int count);
#define DECLARE_STOBJ(name) \
    void stobj_##name##_init(struct Stobj *stobj); \
    void stobj_##name##_tick(struct Stobj *stobj); \
    void stobj_##name##_disp(struct Stobj *stobj); \
    void stobj_##name##_coli(struct Stobj *stobj, struct PhysicsBall *physBall); \
    void stobj_##name##_dest(struct Stobj *stobj); \
    void stobj_##name##_cb_f(struct Stobj *stobj);
DECLARE_STOBJ(bumper)
DECLARE_STOBJ(bumper_bgspecial)
DECLARE_STOBJ(jamabar)
DECLARE_STOBJ(default)
DECLARE_STOBJ(nameent_btn)
DECLARE_STOBJ(button)
DECLARE_STOBJ(goaltape)
DECLARE_STOBJ(goalbag)
DECLARE_STOBJ(goalbag_exmaster)
DECLARE_STOBJ(returngate)

s16 next_stobj_id;
struct Stobj stobjs[0x90];

u32 lbl_1_data_37ED0[0x5C] __attribute__((aligned(8))) =
{
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};

#pragma force_active on
char *stobjTypeNames[] =
{
    "SOT_BUMPER",
    "SOT_JAMABAR",
    "SOT_GOALTAPE",
    "SOT_GOALBAG",
    "SOT_GOALBAG_EXMASTER",
    "SOT_MF_PNL_BUMPER",
    "SOT_MF_PNL_ELECTRAP",
    "SOT_MF_BULLET_TEST",
    "SOT_MF_BOX",
    "SOT_BUMPER_BGSPECIAL",
    "SOT_NAMEENT_BTN",
    "SOT_STAGE_SW000",
    "SOT_RETURNGATE",
};
#pragma force_active reset

#define STOBJ_TABLE(field, suffix) \
    { \
        stobj_bumper_##suffix, \
        stobj_jamabar_##suffix, \
        stobj_goaltape_##suffix, \
        stobj_goalbag_##suffix, \
        stobj_goalbag_exmaster_##suffix, \
        stobj_default_##suffix, \
        stobj_default_##suffix, \
        stobj_default_##suffix, \
        stobj_default_##suffix, \
        stobj_bumper_bgspecial_##suffix, \
        stobj_nameent_btn_##suffix, \
        stobj_button_##suffix, \
        stobj_returngate_##suffix, \
        NULL, \
    }

static void (*stobj_init_funcs[])(struct Stobj *) = STOBJ_TABLE(init, init);
static void (*stobj_tick_funcs[])(struct Stobj *) = STOBJ_TABLE(tick, tick);
static void (*stobj_disp_funcs[])(struct Stobj *) = STOBJ_TABLE(disp, disp);
static void (*stobj_coli_funcs[])(struct Stobj *, struct PhysicsBall *) = STOBJ_TABLE(coli, coli);
static void (*stobj_dest_funcs[])(struct Stobj *) = STOBJ_TABLE(dest, dest);
static void (*stobj_cb_f_funcs[])(struct Stobj *) = STOBJ_TABLE(cb_f, cb_f);

static int lbl_1_data_382A0[] = { 0x316 };

#pragma force_active on
char lbl_1_data_382A4[] = "STATUS: %s\n";
char lbl_1_data_382B0[] =
{
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x0A,
    0x00
};
char lbl_1_data_382CC[] = "TYPE: %s\n";
char lbl_1_data_382D8[] = "MODEL: ";
char lbl_1_data_382E0[] = "\n";
char lbl_1_data_382E4[] = "PART ID: %d\n";
char lbl_1_data_382F4[] = "POS: X,%7.3f\n";
char lbl_1_data_38304[] = "     Y,%7.3f\n";
char lbl_1_data_38314[] = "     Z,%7.3f\n";
char lbl_1_data_38324[] = "SPD: X,%7.3f\n";
char lbl_1_data_38334[] = "ROT: X,0x%04X\n";
char lbl_1_data_38344[] = "     Y,0x%04X\n";
char lbl_1_data_38354[] = "     Z,0x%04X\n";
char lbl_1_data_38364[] = "ROTSPD: X,0x%04X\n";
char lbl_1_data_38378[] = "        Y,0x%04X\n";
char lbl_1_data_3838C[] = "        Z,0x%04X\n";
char lbl_1_data_383A0[] = "COLI RAD: %7.3f\n";
char lbl_1_data_383B4[] = "Flag: 0x%08X\n";
#pragma force_active reset

void event_stobj_init(void)
{
    int i;
    struct Stobj *stobj;

    next_stobj_id = 0;
    memset(stobjs, 0, sizeof(stobjs));
    stobj = stobjs;
    for (i = 0; i < 0x90; i++, stobj++)
    {
        stobj->id = i;
        stobj->unk2 = -1;
    }
    pool_clear(&g_poolInfo.stobjPool);
    g_something_with_bumper_LODs();
    load_bumper_stobjs(stagedef->collHeaders, stagedef->itemGroupCount);
    load_jamabar_stobjs(stagedef->collHeaders, stagedef->itemGroupCount);
    load_button_stobjs(stagedef->collHeaders, stagedef->itemGroupCount);
    load_goaltape_and_goalbag_stobjs(stagedef->collHeaders, stagedef->itemGroupCount);
    load_returngate_stobjs(stagedef->collHeaders, stagedef->itemGroupCount);
    fn_1_A7E30();
}

void event_stobj_tick(void)
{
    int i;
    struct Stobj *stobj;
    s8 *status;

    if (g_some_other_flags & 0xA)
        return;

    status = g_poolInfo.stobjPool.statusList;
    stobj = stobjs;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, status++, stobj++)
    {
        if (*status != 0)
        {
            if (*status == 3)
            {
                stobj_dest_funcs[stobj->type](stobj);
                *status = 0;
            }
            else
            {
                stobj->unk7C = stobj->u_some_pos;
                stobj->unk88 = stobj->rotX;
                stobj->unk8A = stobj->rotY;
                stobj->unk8C = stobj->rotZ;
                stobj_tick_funcs[stobj->type](stobj);
                stobj->position_2 = stobj->position;
                fn_1_A81A0(stobj);
                if (stobj->unk8 & 8)
                {
                    Vec sp8;

                    if (stobj->animGroupId != 0)
                    {
                        mtxa_from_mtx(itemgroups[stobj->animGroupId].transform);
                        mtxa_translate(&stobj->u_some_pos);
                    }
                    else
                        mtxa_from_translate(&stobj->u_some_pos);
                    if (stobj->unk8 & 0x10)
                    {
                        mtxa_rotate_y(stobj->rotY);
                        mtxa_rotate_x(stobj->rotX);
                        mtxa_rotate_z(stobj->rotZ);
                    }
                    else
                    {
                        mtxa_rotate_z(stobj->rotZ);
                        mtxa_rotate_y(stobj->rotY);
                        mtxa_rotate_x(stobj->rotX);
                    }
                    mtxa_tf_point(&stobj->unk90, &sp8);
                    g_something_with_translating_items(5, &sp8, stobj->unk9C);
                }
            }
        }
    }
    g_stobj_sound_something();
}

void event_stobj_dest(void)
{
    int i;
    struct Stobj *stobj;
    s8 *status;

    status = g_poolInfo.stobjPool.statusList;
    stobj = stobjs;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, status++, stobj++)
    {
        if (*status != 0)
        {
            stobj_dest_funcs[stobj->type](stobj);
            *status = 0;
        }
    }
}

void g_draw_stobjs(void)
{
    int i;
    struct Stobj *stobj;
    s8 *status;
    int group;
    void *func;
    Mtx mtx;

    if ((monkey_flags & 8) && !(dip_switches & 0x80000000))
    {
        perf_init_timer(8);
        func = world_theme.unk8C;
        if (func != NULL)
            g_yet_another_unk_draw_func(func);
        mtx_copy(mtxa->mtxB, mtx);

        group = 0;
        status = g_poolInfo.stobjPool.statusList;
        stobj = stobjs;
        for (i = g_poolInfo.stobjPool.count; i > 0; i--, status++, stobj++)
        {
            if (*status != 0)
            {
                if (group != stobj->animGroupId)
                {
                    mtxa_from_mtx(mtx);
                    mtxa_mult_right(itemgroups[stobj->animGroupId].transform);
                    mtxa_to_mtx(mtxa->mtxB);
                    group = stobj->animGroupId;
                }
                HFR_TAG(HFR_K_STOBJ, stobj - stobjs);
                stobj_disp_funcs[stobj->type](stobj);
                HFR_UNTAG();
            }
        }
        mtx_copy(mtx, mtxa->mtxB);
        if (func != NULL)
            g_yet_another_unk_draw_func(NULL);
        perfInfo.stobjTime += perf_stop_timer(8);
    }
}

static u32 lbl_1_bss_9AB34;

void fn_1_A7E30(void)
{
    lbl_1_bss_9AB34 = 0;
}

void g_stobj_sound_something(void)
{
    int count;
    u32 mask = lbl_1_bss_9AB34;
    int *sound;

    if (mask != 0)
    {
        sound = lbl_1_data_382A0;
        count = 1;

        for (; count > 0 && mask != 0; count--, mask >>= 1, sound++)
        {
            if (mask & 1)
                call_SoundReqID_arg_0(*sound);
        }
        lbl_1_bss_9AB34 = 0;
    }
}

int spawn_stobj(struct Stobj *template)
{
    int idx;
    struct Stobj *stobj;

    idx = pool_alloc(&g_poolInfo.stobjPool, 1);
    if (idx < 0)
        return -1;

    stobj = &stobjs[idx];
    memcpy_handler(stobj, template, sizeof(*stobj));
    stobj->id = idx;
    stobj_init_funcs[stobj->type](stobj);
    stobj->unk7C = stobj->u_some_pos;
    fn_1_A81A0(stobj);
    stobj->position_2 = stobj->position;
    stobj->coliFunc = stobj_coli_funcs[stobj->type];
    stobj->unk2 = next_stobj_id;
    next_stobj_id++;
    if (next_stobj_id < 0)
        next_stobj_id = 0;
    return stobj->unk2;
}

static struct StobjFuncs stobjDummyFuncs =
{
    stobj_default_init,
    stobj_default_tick,
    stobj_default_disp,
    stobj_default_coli,
    stobj_default_dest,
    stobj_default_cb_f,
};

#pragma force_active on
void fn_1_A8034(int type, struct StobjFuncs *funcs)
{
    struct StobjFuncs sp8;

    if (funcs == NULL)
        sp8 = stobjDummyFuncs;
    else
    {
        sp8 = *funcs;
        if (sp8.init == NULL)
            sp8.init = stobjDummyFuncs.init;
        if (sp8.tick == NULL)
            sp8.tick = stobjDummyFuncs.tick;
        if (sp8.disp == NULL)
            sp8.disp = stobjDummyFuncs.disp;
        if (sp8.coli == NULL)
            sp8.coli = stobjDummyFuncs.coli;
        if (sp8.dest == NULL)
            sp8.dest = stobjDummyFuncs.dest;
        if (sp8.cbF == NULL)
            sp8.cbF = stobjDummyFuncs.cbF;
    }

    stobj_init_funcs[type] = sp8.init;
    stobj_tick_funcs[type] = sp8.tick;
    stobj_disp_funcs[type] = sp8.disp;
    stobj_coli_funcs[type] = sp8.coli;
    stobj_dest_funcs[type] = sp8.dest;
    stobj_cb_f_funcs[type] = sp8.cbF;
}
#pragma force_active reset

void fn_1_A81A0(struct Stobj *stobj)
{
    mtxa_from_translate(&stobj->u_some_pos);
    if (stobj->unk8 & 0x10)
    {
        mtxa_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX);
        mtxa_rotate_z(stobj->rotZ);
    }
    else
    {
        mtxa_rotate_z(stobj->rotZ);
        mtxa_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX);
    }
    mtxa_tf_point(&stobj->u_model_origin, &stobj->position);
}

static struct GMA **lbl_1_data_383DC[] = { &stage_gma, &g_bg_gma, NULL };

struct BumperLODs lbl_1_bss_9AB38;
struct GMAModel *lbl_1_bss_9AB5C;
struct GMAModel *lbl_1_bss_9AB60[5];

void g_something_with_bumper_LODs(void)
{
    int i;
    struct GMA ***gmaIter;
    struct GMA *gma;
    int found;
    int found2;
    int numModels;
    struct GMAModelEntry *entry;

    memset(&lbl_1_bss_9AB38, 0, sizeof(lbl_1_bss_9AB38));
    switch (world_theme.bgId)
    {
    case 10:
        lbl_1_bss_9AB38.lods[0] = 0.095f;
        lbl_1_bss_9AB38.lods[1] = 0.0565f;
        lbl_1_bss_9AB38.lods[2] = 0.00565f;
        lbl_1_bss_9AB38.lods[3] = 0.00056f;
        break;
    case 9:
        lbl_1_bss_9AB38.lods[0] = 0.103f;
        lbl_1_bss_9AB38.lods[1] = 0.085f;
        lbl_1_bss_9AB38.lods[2] = 0.0085f;
        lbl_1_bss_9AB38.lods[3] = 0.00085f;
        break;
    case 8:
        lbl_1_bss_9AB38.lods[0] = 0.028f;
        lbl_1_bss_9AB38.lods[1] = 0.0075f;
        lbl_1_bss_9AB38.lods[2] = 0.00075f;
        lbl_1_bss_9AB38.lods[3] = 0.000075f;
        break;
    default:
        lbl_1_bss_9AB38.lods[0] = 0.075f;
        lbl_1_bss_9AB38.lods[1] = 0.0075f;
        lbl_1_bss_9AB38.lods[2] = 0.00075f;
        lbl_1_bss_9AB38.lods[3] = 0.000075f;
        break;
    }

    found = FALSE;
    for (gmaIter = lbl_1_data_383DC; *gmaIter != NULL; gmaIter++)
    {
        gma = **gmaIter;
        if (gma != NULL)
        {
            for (i = 0; i < 4; i++)
            {
                if (lbl_1_bss_9AB38.models[i] == NULL)
                {
                    char name[32];

                    sprintf(name, "BUMPER_L%d", i + 1);
                    entry = gma->modelEntries;
                    numModels = gma->numModels;
                    while (numModels > 0)
                    {
                        if (strcmp(entry->name + 4, name) == 0)
                        {
                            lbl_1_bss_9AB38.models[i] = entry->model;
                            found = TRUE;
                        }
                        numModels--;
                        entry++;
                    }
                }
            }
        }
    }

    if (found)
    {
        struct GMAModel **iter;
        struct GMAModel **next;
        int count;

        for (iter = lbl_1_bss_9AB38.models, i = 0; i < 4; i++, iter++)
        {
            if (*iter == NULL)
            {
                next = iter;
                for (numModels = i; numModels < 3; numModels++, next++)
                    next[0] = next[1];
                *next = NULL;
            }
        }
        count = 0;
        for (iter = lbl_1_bss_9AB38.models, i = 4; i > 0 && *iter != NULL; i--, iter++)
            count++;
        lbl_1_bss_9AB38.modelCount = count;
    }
    else
    {
        lbl_1_bss_9AB38.models[0] = init_common_gma->modelEntries[0x64].model;
        lbl_1_bss_9AB38.models[1] = init_common_gma->modelEntries[0x65].model;
        lbl_1_bss_9AB38.modelCount = 2;
    }

    memset(&lbl_1_bss_9AB5C, 0, sizeof(lbl_1_bss_9AB5C));

    found2 = FALSE;
    for (gmaIter = lbl_1_data_383DC; *gmaIter != NULL; gmaIter++)
    {
        gma = **gmaIter;
        if (gma != NULL)
        {
            entry = gma->modelEntries;
            numModels = gma->numModels;
            while (numModels > 0)
            {
                if (strcmp(entry->name + 4, "JAMABAR") == 0)
                {
                    lbl_1_bss_9AB5C = entry->model;
                    found2 = TRUE;
                    break;
                }
                numModels--;
                entry++;
            }
            if (found2)
                break;
        }
    }
    if (!found2)
        lbl_1_bss_9AB5C = init_common_gma->modelEntries[0x66].model;
    fn_1_A8628();
}

static char *button_type_strs[] =
{
    "BUTTON_P",
    "BUTTON_S",
    "BUTTON_R",
    "BUTTON_FF",
    "BUTTON_FR",
};

void fn_1_A8628(void)
{
    struct GMAModel **models;
    char **strs;
    int count;
    int numModels;
    struct GMA ***gmaIter;
    struct GMAModelEntry *entry;
    int found;
    int diff;

    memset(lbl_1_bss_9AB60, 0, sizeof(lbl_1_bss_9AB60));
    models = lbl_1_bss_9AB60;
    strs = button_type_strs;
    for (count = 5; count > 0; count--, models++, strs++)
    {
        struct GMA *gma;

        found = FALSE;
        for (gmaIter = lbl_1_data_383DC; *gmaIter != NULL; gmaIter++)
        {
            gma = **gmaIter;
            if (gma != NULL)
            {
                entry = gma->modelEntries;
                numModels = gma->numModels;
                while (numModels > 0)
                {
                    diff = strlen(entry->name);
                    diff -= strlen(*strs);
                    if (diff >= 0 && strcmp(entry->name + diff, *strs) == 0)
                    {
                        *models = entry->model;
                        found = TRUE;
                        break;
                    }
                    numModels--;
                    entry++;
                }
                if (found)
                    break;
            }
        }
        if (!found)
            *models = init_common_gma->modelEntries[0x5C].model;
    }
}

void load_bumper_stobjs(struct StageCollHeader *group, int count)
{
    struct Stobj stobj;
    int i, j;

    memset(&stobj, 0, sizeof(stobj));
    switch (world_theme.bgId)
    {
    case 2:
    case 9:
    case 0x11:
        stobj.type = 9;
        break;
    default:
        stobj.type = 0;
        break;
    }

    for (i = 0; i < count; i++, group++)
    {
        struct StageBumper *bumper = group->bumpers;

        for (j = 0; j < group->bumperCount; j++, bumper++)
        {
            stobj.u_some_pos = bumper->pos;
            stobj.rotX = bumper->rotX;
            stobj.rotY = bumper->rotY;
            stobj.rotZ = bumper->rotZ;
            stobj.unk3C = bumper->scale;
            stobj.animGroupId = i;
            spawn_stobj(&stobj);
        }
    }
}

void load_jamabar_stobjs(struct StageCollHeader *group, int count)
{
    struct Stobj stobj;
    int i, j;

    memset(&stobj, 0, sizeof(stobj));
    stobj.type = 1;

    for (i = 0; i < count; i++, group++)
    {
        struct StageJamabar *jamabar = group->jamabars;

        for (j = 0; j < group->jamabarCount; j++, jamabar++)
        {
            stobj.unkA8 = jamabar->pos;
            stobj.rotX = jamabar->rotX;
            stobj.rotY = jamabar->rotY;
            stobj.rotZ = jamabar->rotZ;
            stobj.unk3C = jamabar->scale;
            stobj.animGroupId = i;
            spawn_stobj(&stobj);
        }
    }
}

void load_button_stobjs(struct StageCollHeader *group, int count)
{
    struct Stobj stobj;
    int i, j;

    memset(&stobj, 0, sizeof(stobj));
    stobj.type = 0xB;

    for (i = 0; i < count; i++, group++)
    {
        struct StageButton *button = group->buttons;

        for (j = 0; j < group->buttonCount; j++, button++)
        {
            stobj.unkA8 = button->pos;
            stobj.unk3C.x = 1.0f;
            stobj.unk3C.y = 1.0f;
            stobj.unk3C.z = 1.0f;
            stobj.rotX = button->rotX;
            stobj.rotY = button->rotY;
            stobj.rotZ = button->rotZ;
            stobj.animGroupId = i;
            stobj.extraData = button;
            spawn_stobj(&stobj);
        }
    }
}

void stobj_bumper_init(struct Stobj *stobj)
{
    stobj->state = 0;
    stobj->unk8 |= 0xA;
    stobj->model = lbl_1_bss_9AB38.models[0];
    stobj->boundSphereRadius = 0.75f * stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->unk48 = 1.0f;
    stobj->unk4C = 1.0f;
    stobj->unk50 = 1.0f;
    stobj->unk76 = 0;
    stobj->unk9C = 0.75f;
    stobj->unk90 = stobj->u_model_origin;
}

void stobj_bumper_tick(struct Stobj *stobj)
{
    switch (stobj->state)
    {
    case 0:
        stobj->unk78 += (0x100 - stobj->unk78) >> 6;
        if (stobj->unk48 > 1.0)
        {
            stobj->unk48 -= 0.06666666666666667;
            if (stobj->unk48 < 1.0)
                stobj->unk48 = 1.0f;
            stobj->unk50 = stobj->unk48;
        }
        break;
    case 1:
        stobj->state = 2;
        stobj->counter = 7;
        // fall through
    case 2:
        stobj->counter--;
        if (stobj->counter < 0)
            stobj->state = 0;
        stobj->unk78 += 0x100;
        stobj->unk48 += 0.5 * (2.0 - stobj->unk48);
        stobj->unk50 = stobj->unk48;
        break;
    }
    stobj->unk76 += stobj->unk78;
}

static inline struct GMAModel *get_bumper_lod_model(float size)
{
    float *lods = lbl_1_bss_9AB38.lods;
    struct GMAModel **models = lbl_1_bss_9AB38.models;
    int i;

    for (i = lbl_1_bss_9AB38.modelCount - 1; i > 0; i--, models++, lods++)
    {
        if (size > *lods)
            break;
    }
    return *models;
}

void stobj_bumper_disp(struct Stobj *stobj)
{
    Vec sp18;
    Vec spC;
    void *temp_r31;
    struct GMAModel *model;
    float radius;

    mtxa_from_mtxb();
    mtxa_translate(&stobj->u_some_pos);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_y(stobj->unk76);
    spC.x = stobj->unk48 * stobj->unk3C.x;
    spC.y = stobj->unk3C.y;
    spC.z = stobj->unk50 * stobj->unk3C.z;
    mtxa_scale_xyz(spC.x, spC.y, spC.z);
    mathutil_mtxA_get_translate(&sp18);
    if (sp18.z > 0.0)
        return;

    model = stobj->model;
    radius = model->boundSphereRadius;
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, radius, spC.x))
    {
        float temp_f1;

        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);

        temp_f1 = -((g_current_camera->sub28.viewportH * (radius * g_current_camera->sub28.view.invTanHalfFov)) / sp18.z);
        model = get_bumper_lod_model(temp_f1);
        if (temp_f1 > 0.03f)
            avdisp_draw_model_unculled_sort_auto(model);
        else
        {
            temp_r31 = g_yet_another_unk_draw_func(NULL);
            avdisp_draw_model_unculled_sort_auto(model);
            g_yet_another_unk_draw_func(temp_r31);
        }
    }
    if (stobj->unk48 > 1.0f)
    {
        float temp_f31_2;
        float phi_f30;

        mtxa_from_mtxb();
        mtxa_translate_xyz(stobj->u_some_pos.x, 0.05f + stobj->u_some_pos.y, stobj->u_some_pos.z);
        mtxa_rotate_z(stobj->rotZ);
        mtxa_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX - 0x4000);
        temp_f31_2 = 2.0f * spC.x;
        phi_f30 = 2.0f * (spC.x - 1.0f);
        if (phi_f30 > 1.0f)
            phi_f30 = 1.0f;
        mtxa_scale_s(temp_f31_2);
        avdisp_set_scale_factor(temp_f31_2);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_z_mode(1, 3, 0);
        set_post_mult_color(phi_f30, phi_f30, phi_f30, 1.0f);
        avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[0x5E].model);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void stobj_bumper_coli(struct Stobj *stobj, struct PhysicsBall *physBall)
{
    Vec sp24;
    Vec sp18;
    float temp_f1;
    float temp_f2;
    struct Ball *ball;

    ball = current_ball;
    stobj->state = 1;
    sp24 = stobj->position;
    fn_1_A74E4(&physBall->prevPos, &physBall->pos, physBall->radius, &stobj->position_2, &sp24, stobj->model->boundSphereRadius);
    sp18 = physBall->pos;
    sp18.x -= stobj->position.x;
    sp18.y -= stobj->position.y;
    sp18.z -= stobj->position.z;
    vec_normalize_len(&sp18);
    temp_f2 = -1.5 * (sp18.x * physBall->vel.x + sp18.y * physBall->vel.y + sp18.z * physBall->vel.z);
    if (temp_f2 > 0.0)
    {
        physBall->vel.x += temp_f2 * sp18.x;
        physBall->vel.y += temp_f2 * sp18.y;
        physBall->vel.z += temp_f2 * sp18.z;
    }
    physBall->vel.x += 0.05 * sp18.x;
    physBall->vel.y += 0.05 * sp18.y;
    physBall->vel.z += 0.05 * sp18.z;
    temp_f1 = stobj->boundSphereRadius + physBall->radius + 0.01;
    sp18.x *= temp_f1;
    sp18.y *= temp_f1;
    sp18.z *= temp_f1;
    physBall->pos.x = stobj->position.x + sp18.x;
    physBall->pos.y = stobj->position.y + sp18.y;
    physBall->pos.z = stobj->position.z + sp18.z;
    call_SoundReqID_arg_0(0x5011);
    ball->flags |= 0x20;

    {
        s16 sp8[] =
        {
            0x005A, 0x005B,
            0x005C, 0x005D,
            0x005E, 0x005F,
            0x0060, 0x0061,
        };

        g_smth_with_sound = ball->ape->charaId;
        call_SoundReqID_arg_0(sp8[g_font_char_var_3 & 7]);
        if (!(ball->flags & 0x80000000000ULL))
            g_rumble_controller(player_pad_map[ball->playerId], 1, 0x1E);
    }
}

void stobj_bumper_dest(struct Stobj *stobj) {}

void stobj_bumper_cb_f(struct Stobj *stobj) {}

void stobj_bumper_bgspecial_init(struct Stobj *stobj)
{
    stobj->state = 0;
    stobj->unk8 |= 0xA;
    stobj->model = lbl_1_bss_9AB38.models[0];
    stobj->boundSphereRadius = 0.75f * stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->unk48 = 1.0f;
    stobj->unk4C = 1.0f;
    stobj->unk50 = 1.0f;
    stobj->unk76 = 0;
    stobj->unk9C = 0.75f;
    stobj->unk90 = stobj->u_model_origin;
}

void stobj_bumper_bgspecial_tick(struct Stobj *stobj)
{
    stobj_bumper_tick(stobj);
}

static s16 lbl_1_data_3844C[] =
{
    8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
};

void stobj_bumper_bgspecial_disp(struct Stobj *stobj)
{
    Mtx sp20;
    Vec sp14;
    Vec sp8;
    float scale;
    float temp;
    struct GMAModel *model;
    struct GMAModel *bumperModel;
    int modelId;

    stobj_bumper_disp(stobj);

    switch (world_theme.bgId)
    {
    case 0x11:
        model = init_common_gma->modelEntries[0x5E].model;
        bumperModel = stobj->model;
        mtxa_from_mtxb_translate(&stobj->u_some_pos);
        mtxa_rotate_z(stobj->rotZ);
        mtxa_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX);
        mtxa_translate_xyz(1.5f * bumperModel->boundSphereCenter.x, 1.5f * bumperModel->boundSphereCenter.y, 1.5f * bumperModel->boundSphereCenter.z);
        mtxa_sq_from_identity();
        scale = 2.0f * (stobj->unk48 * stobj->unk3C.x) * (bumperModel->boundSphereRadius / model->boundSphereRadius);
        if (g_some_render_flag.flags & 0x400)
            scale *= 1.0f + 0.8f * (g_lava_bumper_func1() - 1.0f);
        if ((g_font_char_var_3 + stobj->id) & 1)
            scale *= 0.95f;
        mtxa_scale_s(scale);
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * scale))
        {
            scale *= model->boundSphereRadius;
            mathutil_mtxA_get_translate(&sp14);
            temp = mathutil_vec_len(&sp14);
            if (temp > 0.01f + scale)
            {
                mtxa_to_mtx(sp20);
                temp = (temp - scale) / temp;
                mtxa_from_identity();
                mtxa_scale_s(temp);
                mtxa_mult_right(sp20);
            }
            set_post_mult_color(1.0f, 0.95f, 0.8f, 1.0f);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_draw_model_unculled_sort_always(model);
            avdisp_set_z_mode(1, 3, 1);
            fade_color_base_default();
        }
        break;
    case 2:
        if (stobj->unk48 > 1.0f)
        {
            float var_f1;
            float radius;

            model = g_bg_gma->modelEntries[0x19].model;
            var_f1 = 1.1f * (stobj->unk48 - 1.0f);
            if (var_f1 > 1.0f)
                var_f1 = 1.0f;
            var_f1 = math_sin(16384.0f * var_f1);
            radius = stobj->model->boundSphereRadius;
            temp = radius + radius * var_f1;
            mtxa_from_mtxb_translate(&stobj->u_some_pos);
            mtxa_rotate_z(stobj->rotZ);
            mtxa_rotate_y(stobj->rotY);
            mtxa_rotate_x(stobj->rotX);
            mtxa_rotate_y(stobj->id << 11);
            mtxa_translate_xyz(0.0f, temp, 0.0f);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(model);
        }
        break;
    case 9:
        modelId = lbl_1_data_3844C[g_font_char_var_3 & 0x1F];
        model = g_bg_gma->modelEntries[modelId].model;
        mtxa_from_mtxb_translate(&stobj->u_some_pos);
        mathutil_mtxA_get_translate(&sp8);
        temp = sp8.z + 8.0f * g_current_camera->sub28.view.invTanHalfFov * g_current_camera->sub28.viewportH;
        if (temp > 0.0f)
        {
            temp *= 0.5f;
            mtxa_rigid_inv_tf_tl(&sp8);
            mtxa_rotate_y(math_atan2(sp8.x, sp8.z));
            mtxa_scale_xyz(0.025f * stobj->unk48, 0.035f, 1.0f);
            if (temp < 1.0f)
                avdisp_set_alpha(temp);
            avdisp_set_scale_factor(0.035f);
            avdisp_draw_model_culled_sort_always(model);
        }
        break;
    }
}

void stobj_bumper_bgspecial_coli(struct Stobj *stobj, struct PhysicsBall *physBall)
{
    stobj_bumper_coli(stobj, physBall);
}

void stobj_bumper_bgspecial_dest(struct Stobj *stobj) {}

void stobj_bumper_bgspecial_cb_f(struct Stobj *stobj) {}

void stobj_jamabar_init(struct Stobj *stobj)
{
    stobj->state = 0;
    stobj->unk8 |= 0xA;
    stobj->model = lbl_1_bss_9AB5C;
    stobj->boundSphereRadius = stobj->model->boundSphereRadius * stobj->unk3C.x;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->u_some_pos = stobj->unkA8;
    stobj->unk90.x = 0.0f;
    stobj->unk90.y = 0.5f;
    stobj->unk90.z = 1.75f;
    stobj->unk9C = 1.0f;
}

void stobj_jamabar_tick(struct Stobj *stobj)
{
    Vec spC;

    mtxa_from_translate(&stobj->unkA8);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_push();
    mtxa_mult_left(itemgroups[stobj->animGroupId].transform);
    mtxa_rigid_inv_tf_vec(&g_gravity_dir, &spC);
    mtxa_pop();
    spC.z *= 0.016;
    stobj->u_local_vel.z += spC.z;
    stobj->u_local_vel.z *= 0.97;
    stobj->u_local_pos.z += stobj->u_local_vel.z;
    if (stobj->u_local_pos.z < -2.5)
    {
        stobj->u_local_pos.z = -2.5f;
        if (stobj->u_local_vel.z < 0.0)
            stobj->u_local_vel.z = -stobj->u_local_vel.z;
    }
    else if (stobj->u_local_pos.z > 0.0)
    {
        stobj->u_local_pos.z = 0.0f;
        if (stobj->u_local_vel.z > 0.0)
            stobj->u_local_vel.z = -stobj->u_local_vel.z;
    }
    mtxa_tf_point(&stobj->u_local_pos, &stobj->u_some_pos);
    stobj->unk64.x = stobj->u_some_pos.x - stobj->unk7C.x;
    stobj->unk64.y = stobj->u_some_pos.y - stobj->unk7C.y;
    stobj->unk64.z = stobj->u_some_pos.z - stobj->unk7C.z;
}

void stobj_jamabar_disp(struct Stobj *stobj)
{
    Vec spC;

    mtxa_from_mtxb();
    mtxa_translate(&stobj->u_some_pos);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    spC = stobj->unk3C;
    mtxa_scale_xyz(spC.x, spC.y, spC.z);
    avdisp_set_scale_factor(spC.z);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(stobj->model);
}

void stobj_jamabar_coli(struct Stobj *stobj, struct PhysicsBall *physBall)
{
    stobj_jamabar_child_coli(physBall, stobj);
}

void stobj_jamabar_dest(struct Stobj *stobj) {}

void stobj_jamabar_cb_f(struct Stobj *stobj)
{
    debug_window_printf(2, lbl_1_data_382B0);
    debug_window_printf(2, "OFS: X,%7.3f\n", stobj->u_local_pos.x);
    debug_window_printf(2, lbl_1_data_38304, stobj->u_local_pos.y);
    debug_window_printf(2, lbl_1_data_38314, stobj->u_local_pos.z);
    debug_textdraw_put(2, lbl_1_data_382E0);
    debug_window_printf(2, "OFS SPD: X,%7.3f\n", stobj->u_local_vel.x);
    debug_window_printf(2, "         Y,%7.3f\n", stobj->u_local_vel.y);
    debug_window_printf(2, "         Z,%7.3f\n", stobj->u_local_vel.z);
    debug_textdraw_put(2, lbl_1_data_382E0);
}

void stobj_default_init(struct Stobj *stobj) {}

void stobj_default_tick(struct Stobj *stobj) {}

void stobj_default_disp(struct Stobj *stobj) {}

void stobj_default_coli(struct Stobj *stobj, struct PhysicsBall *physBall) {}

void stobj_default_dest(struct Stobj *stobj) {}

void stobj_default_cb_f(struct Stobj *stobj) {}

void stobj_button_init(struct Stobj *stobj)
{
    struct StageButton *button = stobj->extraData;

    stobj->state = 0;
    stobj->unk8 |= 0xA;
    stobj->counter = 0x3C;
    stobj->model = lbl_1_bss_9AB60[button->type];
    stobj->boundSphereRadius = stobj->model->boundSphereRadius * stobj->unk3C.x;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->u_some_pos = stobj->unkA8;
    *(Vec *)&stobj->unk48 = stobj->unk3C;
    stobj->u_local_pos.x = 0.0f;
    stobj->u_local_pos.y = 0.0f;
    stobj->u_local_pos.z = 0.0f;
    stobj->u_local_vel.x = 0.0f;
    stobj->u_local_vel.y = 0.0f;
    stobj->u_local_vel.z = 0.0f;
    stobj->unk90.x = 0.0f;
    stobj->unk90.y = 0.3f;
    stobj->unk90.z = 0.0f;
    stobj->unk9C = 1.0f;
}

void stobj_button_tick(struct Stobj *stobj)
{
    Vec sp8;
    struct StageButton *button = stobj->extraData;

    if (stobj->counter > 0)
        stobj->counter--;
    switch (stobj->state & 0xFF)
    {
    case 0:
        stobj->state &= ~0xFF;
        stobj->state |= 1;
        stobj->state &= ~0x100;
        stobj->state &= ~0x200;
        stobj->unk9C = 1.0f;
        /* fall through */
    case 1:
        stobj->u_local_vel.y += 0.1f * -stobj->u_local_pos.y;
        stobj->u_local_vel.y *= 0.95f;
        stobj->u_local_pos.y += stobj->u_local_vel.y;
        if (stobj->u_local_pos.y < -0.1f)
        {
            stobj->u_local_pos.y = -0.1f;
            if (stobj->u_local_vel.y < 0.0f)
                stobj->u_local_vel.y *= -0.8f;
        }
        else if (stobj->u_local_pos.y > 0.1f)
        {
            stobj->u_local_pos.y = 0.1f;
            if (stobj->u_local_vel.y > 0.0f)
                stobj->u_local_vel.y *= -0.8f;
        }
        if (g_smth_with_buttons(button->animGroupId, button->type))
        {
            stobj->state &= ~0xFF;
            stobj->state |= 2;
        }
        if ((stobj->state & 0x200) && stobj->u_local_pos.y < -0.025f)
        {
            stobj->state &= ~0xFF;
            stobj->state |= 2;
        }
        break;
    case 2:
        stobj->state &= ~0xFF;
        stobj->state |= 3;
        stobj->unk9C = 0.01f;
        if (!(stobj->state & 0x100))
        {
            stobj->state |= 0x100;
            g_smth_with_stage_anim_groups(button->animGroupId, button->type);
        }
        if (stobj->counter <= 0)
            lbl_1_bss_9AB34 |= 1;
        /* fall through */
    case 3:
        if (!g_smth_with_buttons(button->animGroupId, button->type))
            stobj->state &= ~0xFF;
        stobj->u_local_vel.y += 0.1f * (-0.1f - stobj->u_local_pos.y);
        stobj->u_local_vel.y *= 0.9f;
        stobj->u_local_pos.y += stobj->u_local_vel.y;
        if (stobj->u_local_pos.y < -0.1f)
        {
            stobj->u_local_pos.y = -0.1f;
            if (stobj->u_local_vel.y < 0.0f)
                stobj->u_local_vel.y *= -0.8f;
        }
        else if (stobj->u_local_pos.y > 0.0f)
        {
            stobj->u_local_pos.y = 0.0f;
            if (stobj->u_local_vel.y > 0.0f)
                stobj->u_local_vel.y *= -0.8f;
        }
        break;
    }
    mtxa_from_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_tf_vec(&stobj->u_local_pos, &sp8);
    stobj->u_some_pos.x = stobj->unkA8.x + sp8.x;
    stobj->u_some_pos.y = stobj->unkA8.y + sp8.y;
    stobj->u_some_pos.z = stobj->unkA8.z + sp8.z;
}

void stobj_button_disp(struct Stobj *stobj)
{
    Vec sp8;
    float r;
    float g;
    float b;
    float temp;
    float t;
    struct GMAModel *model;
    int flag;

    t = 0.1f * math_sin(g_font_char_var_3 << 10);
    temp = 7.5000005f * (-0.02f - stobj->u_local_pos.y) + t;
    r = temp;
    g = 0.8f * temp;
    b = 0.8f * temp;
    if (r < 0.0f)
        r = 0.0f;
    else if (r > 1.0f)
        r = 1.0f;
    if (g < 0.0f)
        g = 0.0f;
    else if (g > 1.0f)
        g = 1.0f;
    if (b < 0.0f)
        b = 0.0f;
    else if (b > 1.0f)
        b = 1.0f;
    if (0.0f == r && 0.0f == g && 0.0f == b)
        flag = 0;
    else
        flag = 1;
    sp8 = stobj->unk3C;
    mtxa_from_mtxb();
    mtxa_translate(&stobj->u_some_pos);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_scale(&sp8);
    model = stobj->model;
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, sp8.z))
    {
        avdisp_set_scale_factor(sp8.z);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        if (flag)
            g_stores_doubles2(r, g, b, 0.0f);
        avdisp_draw_model_unculled_sort_auto(model);
        if (flag)
            g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void stobj_button_coli(struct Stobj *stobj, struct PhysicsBall *physBall)
{
    Vec sp8;
    struct StageColiCylinder cylinder;
    struct PhysicsBall sp30;
    float radius;

    radius = stobj->boundSphereRadius;
    cylinder.pos = stobj->position;
    cylinder.height = 0.3f * stobj->unk3C.y;
    cylinder.radius = math_sqrt(radius * radius - cylinder.height * cylinder.height);
    cylinder.rot.x = stobj->rotX;
    cylinder.rot.y = stobj->rotY;
    cylinder.rot.z = stobj->rotZ;
    cylinder.flags = 0;
    sp30 = *physBall;
    sp30.hardestColiSpeed = 0.0f;
    collide_ball_with_cylinder(&sp30, &cylinder);
    if (0.0f != sp30.hardestColiSpeed)
    {
        mtxa_from_rotate_z(stobj->rotZ);
        mtxa_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX);
        mtxa_tf_vec(&sp30.hardestColiNormal, &sp8);
        stobj->u_local_vel.y = 0.5f * (sp30.hardestColiSpeed * sp8.y);
        stobj->state |= 0x200;
        if (__fabs(physBall->hardestColiSpeed) < __fabs(sp30.hardestColiSpeed))
            *physBall = sp30;
    }
}

void stobj_button_dest(struct Stobj *stobj) {}

void stobj_button_cb_f(struct Stobj *stobj) {}
