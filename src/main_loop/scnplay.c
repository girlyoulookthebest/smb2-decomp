/* Cutscene player: loads scndat_*.bin and animates and draws its instances. */
#include "types.h"
#include "libc.h"
#include "ball.h"
#include "mathutil.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "bitmap.h"
#include "dip.h"
#include "event.h"
#include "file.h"
#include "light.h"
#include "mode.h"
#include "mot_joint.h"
#include "stage.h"
#include "macros.h"

struct SceneJointFlags
{
    /*0x00*/ u8 joint;
    /*0x02*/ u16 mask;
};  /* size = 0x4 */

struct SceneAnim
{
    /*0x00*/ u32 unk0;
    /*0x04*/ u16 numJoints;
    /*0x08*/ struct SceneJointFlags * DPTR jointFlags;
    /*0x0C*/ u16 * DPTR keyframeCounts;
    /*0x10*/ u16 * DPTR times;
    /*0x14*/ u8 * DPTR valueTypes;
    /*0x18*/ float * DPTR values;
};  /* size = 0x1C */

struct SceneObj
{
    /*0x00*/ u16 type;
    /*0x04*/ struct SceneAnim * DPTR anim;
    /*0x08*/ char * DPTR name;
    /*0x0C*/ u16 unkC;
    /*0x0E*/ u16 unkE;
};  /* size = 0x10 */

struct Scene
{
    /*0x00*/ struct SceneObj * DPTR objs;
    /*0x04*/ char * DPTR name;
    /*0x08*/ u16 numObjs;
    /*0x0A*/ u16 duration;
    /*0x0C*/ s32 stage;
    /*0x10*/ s32 unk10;
};  /* size = 0x14 */

struct Instance
{
    /*0x00*/ u16 type;
    /*0x02*/ u16 idx;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ char *name;
    /*0x0C*/ u32 flags;
    /*0x10*/ struct AnimJoint *joints;
    /*0x14*/ Mtx mtx;
    /*0x44*/ void *dialog;
    /*0x48*/ struct Ape *ape;
    /*0x4C*/ struct Instance *parent;
    /*0x50*/ void *lightBuf;
    /*0x54*/ s32 lightIdx;
    /*0x58*/ Vec pos;
    /*0x64*/ Quat rot;
};  /* size = 0x74 */

struct ScenePlayer
{
    /*0x00*/ struct Instance *instances;
    /*0x04*/ s32 count;
    /*0x08*/ s32 stage;
    /*0x0C*/ s32 unkC;
    /*0x10*/ u16 sceneId;
    /*0x12*/ u16 frame;
    /*0x14*/ u16 duration;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 unk18;
    /*0x1C*/ Vec unk1C;
    /*0x28*/ Vec unk28;
    /*0x34*/ u8 lightCount;
    /*0x35*/ u8 done;
};  /* size = 0x38 */

struct DialogSwap
{
    /*0x00*/ char *from;
    /*0x04*/ char *to;
    /*0x08*/ u32 flags;
};  /* size = 0xC */

#define OFFSET_TO_PTR(base, offset) (void *)((uptr)(offset) + (uptr)(base))

extern u8 s_keyframeValueCounts[];
extern struct GMA *g_bg_gma;

void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
void g_math_unk8(Quat *q, float t, Vec *a, Vec *b);
void quat_mult(Quat *out, Quat *a, Quat *b);
void mtxa_to_quat(Quat *q);
void mtxa_from_mtxb_mult_mtx(Mtx m);
struct GMAModel *get_GmaBuffer_entry(struct GMA *gma, char *name);
void fn_1_4EAFC(Mtx m, int color, int c);
void draw_ape(struct Ape *ape);
void fn_1_1141D8(struct Ape *ape, int b, char *name);
struct GMAModel *fn_1_10F79C(struct ApeModelTable *table, char *name);
struct Ape *g_load_ape_variant0(int chara, int lod, int scene);
void g_something_with_freeing_chara_heap_ape(struct Ape *ape);
void load_stage(int stage);
void enable_camera(int a);
void camera_set_state(int state);
void event_init(int id);
void event_dest(int id);
void destroy_sprite_with_unique_id(int id);
#ifdef __MWERKS__
int strncmp(const char *a, const char *b, u32 n);
#else
#include <string.h> /* size_t-correct prototypes (64-bit PC) */
#endif
#ifdef __MWERKS__
int strlen(const char *s);
#endif
void PSVECSubtract(Vec *a, Vec *b, Vec *out);
void DVDChangeDir(char *path);
void g_something_to_do_with_soft_fx(struct Instance *inst, int b, struct Instance *parent);
void load_scene_sound(int a);
void *load_dialog_tbl(int a);
void fn_1_143CC0(void);
void fn_1_143E68(void);
void fn_1_143F34(int a);
void fn_1_144020(void);
void fn_1_1440B0(struct Instance *inst);
void fn_1_152BE8(void);
void fn_1_152D70(void);
void fn_1_162C8C(void);
void fn_1_162CD0(void);
void fn_1_162CD4(struct ScenePlayer *p);
void fn_1_162D28(struct ScenePlayer *p);
int fn_1_162D2C(struct ScenePlayer *p);
void fn_1_162978(void *tbl);
void *fn_1_1629D0(void *tbl, char *name);
void fn_1_1607F4(void *dlg, int a);
void fn_1_15FB34(void *dlg, int a);
void fn_1_15FB70(void *dlg);
void fn_1_15FB94(void *dlg);
void fn_1_160518(void *dlg, void *data);
void fn_1_1607DC(void *dlg, struct Ape *ape);
void fn_1_1607E4(void *dlg);
void fn_1_160870(void *dlg);

int load_scene_data(int category);
void g_parse_scene_data(struct Scene *scenes);
void fn_1_142EFC(struct Scene *scene);
void g_something_freeing_heap_4(void);
int fn_1_1418CC();
void fn_1_142018(struct ScenePlayer *player, struct Instance *inst);
void fn_1_142720(struct Instance *inst);
void fn_1_1431EC(struct AnimJoint *joints, u16 count);
void fn_1_14325C(struct AnimJoint *joints);
void fn_1_1432A0(struct AnimJoint *joints, struct SceneAnim *anim);
void fn_1_143428(struct SceneAnim *anim, struct MotionChannel *chan);
int fn_1_1434D4(char *name, char **list, u8 count);
void fn_1_143558(void *dlg, char *name, struct Ape *ape);
void fn_1_143614(void);
char *fn_1_1436D4(char *name);
void fn_1_143798(int stage);

struct Scene *scene_data;
s32 lbl_1_bss_DD4EC;
void *dialog_tbl;
struct ScenePlayer lbl_1_bss_DD4F4;
u8 lbl_1_bss_DD52C[0x110];
u32 lbl_1_bss_DD63C;

u32 lbl_1_data_6ECD8[] = {
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

int g_active_scene_category = 0x10;

char *s_charaIds[] = {
    "APE",
    "GAL",
    "KID",
    "GOR",
    "MAD",
    "KOB",
    "YGM",
    "YGW",
    "ODM",
    "ODW",
    "MADSPA",
    "MADFRG",
    "MADNKD",
    "ROB",
    "WHALE",
    "MADCRS",
    "KOBSPA",
};

char *lbl_1_data_6EF28[] = {
    "RED",
    "BLUE",
    "YELLOW",
    "GREEN",
    "CYAN",
    "MAGENTA",
    "BLACK",
    "WHITE",
};

char *s_partIds[] = {
    "DEFAULT",
    "EYEGRASS",
    "BUTTON",
    "COIL00",
    "COIL01",
    "BACKPACKFUTA",
    "SCREW",
    "PROP",
    "BACKPERA",
    "BACKPACK",
    "HELICAP",
    "HERIBAR",
};

u32 s_partMasks[] = {0x0, 0x2, 0x4, 0x10, 0x10, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

char *s_texIds[] = {
    "DEFAULT",
    "COLONY",
    "JUNGLE",
    "SPACE",
};

u32 s_texMasks[] = {0x0, 0x2, 0x4, 0x8};

struct DialogSwap lbl_1_data_6F11C[] = {
    {"123", "165", 2},
    {"125", "166", 2},
    {"127", "167", 2},
    {"129", "168", 2},
    {"131", "165", 2},
    {"133", "166", 2},
    {"135", "167", 2},
    {"137", "168", 2},
    {"139", "165", 2},
    {"141", "166", 2},
    {"143", "167", 2},
    {"145", "168", 2},
    {"147", "169", 4},
    {"148", "170", 4},
    {"149", "171", 4},
    {"150", "172", 4},
    {"123", "156", 1},
    {"124", "157", 1},
    {"125", "158", 1},
    {"126", "159", 1},
    {"127", "160", 1},
    {"128", "161", 1},
    {"129", "162", 1},
    {"130", "163", 1},
    {"131", "156", 1},
    {"132", "157", 1},
    {"133", "158", 1},
    {"134", "159", 1},
    {"135", "160", 1},
    {"136", "161", 1},
    {"137", "162", 1},
    {"138", "163", 1},
    {"139", "156", 1},
    {"140", "157", 1},
    {"141", "158", 1},
    {"142", "159", 1},
    {"143", "160", 1},
    {"144", "161", 1},
    {"145", "162", 1},
    {"146", "163", 1},
    {"147", "156", 1},
    {"148", "158", 1},
    {"149", "160", 1},
    {"150", "162", 1},
    {"151", "164", 1},
    {"152", "157", 1},
    {"153", "159", 1},
    {"154", "161", 1},
    {"155", "163", 1},
    {NULL, NULL, 0},
};


void fn_scnplay_order0(void) { scene_data = NULL; }
void fn_scnplay_order1(void) { lbl_1_bss_DD4EC = 0; }
void fn_scnplay_order2(void) { dialog_tbl = NULL; }
void fn_scnplay_order3(void) { lbl_1_bss_DD4F4.count = 0; }
void fn_scnplay_order4(void) { lbl_1_bss_DD52C[0] = 0; }
void fn_scnplay_order5(void) { lbl_1_bss_DD63C = 0; }

void load_scene_files(int category)
{
    dialog_tbl = load_dialog_tbl(category + 1);
    OSReport("dialog_tbl Loaded     : 0x%08x\n", dialog_tbl);
    DVDChangeDir("scnplay");
    load_scene_data(category);
    DVDChangeDir("..");
    load_scene_sound((s8)category);
}

void fn_1_141288(void)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;

    memset(player, 0, sizeof(*player));
    player->stage = -1;
    player->unkC = -1;
    {
        Vec v = {10.0f, 10.0f, 10.0f};
        Vec *p = &v;
        player->unk1C = *p;
    }
    {
        Vec v = {0.0f, 0.0f, 0.0f};
        Vec *p = &v;
        player->unk28 = *p;
    }
    player->unk16 = 0;
    player->unk18 = 0x2AAA;
    fn_1_143CC0();
    fn_1_152BE8();
    fn_1_162CD0();
    fn_1_143614();
}

void fn_1_141370(u16 sceneId, u16 frame)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    struct Scene *scene = scene_data + sceneId - 1;
    u16 count = scene->numObjs;
    struct SceneObj *obj;
    struct Instance *inst;
    int idx;
    u16 i;

    g_something_freeing_heap_4();
    if (sceneId == 0 || sceneId > lbl_1_bss_DD4EC)
        return;
    fn_1_141288();
    fn_1_142EFC(scene);
    player->stage = scene->stage;
    fn_1_143798(scene->stage);
    player->unkC = scene->unk10;
    player->count = count;
    player->sceneId = sceneId;
    player->frame = frame;
    player->duration = scene->duration;
    player->instances = alloc_from_heap_or_panic(currentHeap, ALIGN32(count * sizeof(struct Instance)), "scnplay.c", 0x1AD);
    inst = player->instances;
    idx = 0;
    for (i = 0, obj = scene->objs; i < count; i++, obj++)
    {
        if (obj->type == 9)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 10)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 11)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
    }
    for (i = 0, obj = scene->objs; i < count; i++, obj++)
    {
        if (obj->type == 6)
        {
            fn_1_1418CC(inst, obj, idx);
            player->lightCount++;
            inst++;
            idx++;
        }
        else if (obj->type == 7)
        {
            fn_1_1418CC(inst, obj, idx);
            player->lightCount++;
            inst++;
            idx++;
        }
        else if (obj->type == 8)
        {
            fn_1_1418CC(inst, obj, idx);
            player->lightCount++;
            inst++;
            idx++;
        }
        else if (obj->type == 16)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 2)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 14)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 15)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
    }
    for (i = 0, obj = scene->objs; i < count; i++, obj++)
    {
        if (obj->type == 1)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
    }
    for (i = 0, obj = scene->objs; i < count; i++, obj++)
    {
        if (obj->type == 3)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 17)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 4)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 13)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
        else if (obj->type == 0)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
    }
    for (i = 0, obj = scene->objs; i < count; i++, obj++)
    {
        if (obj->type == 12)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
    }
    for (i = 0, obj = scene->objs; i < count; i++, obj++)
    {
        if (obj->type == 5)
        {
            fn_1_1418CC(inst, obj, idx);
            inst++;
            idx++;
        }
    }
}

int fn_1_1418CC(inst, obj, idx)
    struct Instance *inst;
    struct SceneObj *obj;
    u16 idx;
{
    struct Light light;
    struct SceneAnim anim;
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    u32 size;

    anim = *obj->anim;
    inst->type = obj->type;
    inst->idx = idx;
    inst->unk4 = obj->unkC;
    inst->unk6 = obj->unkE;
    inst->name = obj->name;
    inst->flags = 0;
    size = ALIGN32((anim.numJoints + 1) * sizeof(struct AnimJoint));
    inst->joints = alloc_from_heap_or_panic(currentHeap, size, "scnplay.c", 0x2C8);
    if (inst->joints == NULL)
        return 0;
    fn_1_1431EC(inst->joints, anim.numJoints);
    fn_1_14325C(inst->joints);
    fn_1_1432A0(inst->joints, &anim);
    inst->dialog = NULL;
    inst->ape = NULL;
    inst->parent = NULL;
    inst->lightBuf = NULL;
    switch (inst->type)
    {
    case 4:
    {
        struct Instance *other;
        int i;

        other = player->instances;
        if (strncmp(inst->name, "SARU", 4) == 0)
        {
            for (i = 0; i < idx; i++, other++)
            {
                if (other->type == 1 && other->unk6 == inst->unk6)
                {
                    if (other->ape != NULL)
                        inst->parent = other;
                    break;
                }
            }
        }
        break;
    }
    case 3:
    case 13:
    {
        int i;
        struct Instance *other;

        other = player->instances;
        for (i = 0; i < idx; i++, other++)
        {
            if (other->type == 1 && other->unk6 == inst->unk6)
            {
                if (other->ape != NULL)
                    inst->parent = other;
                break;
            }
        }
        break;
    }
    case 1:
    {
        int i;
        struct Instance *other;
        int id;

        other = player->instances;
        for (i = 0; i < idx; i++, other++)
        {
            if ((other->type == 2 || other->type == 14 || other->type == 15) && other->unk6 == inst->unk6)
                other->parent = inst;
        }
        id = fn_1_1434D4(inst->name, s_charaIds, 17);
        if (id == -1)
        {
            id = 0;
            OSReport("saru_id error!! %s %d\n", obj->name, inst->idx);
        }
        inst->ape = g_load_ape_variant0(id, inst->unk4, player->unkC + 27);
        break;
    }
    case 5:
    {
        int i;
        struct Instance *other;

        other = player->instances;
        if (strcmp(inst->name, "SARU") != 0)
        {
            for (i = 0; i < idx; i++, other++)
            {
                if (other->type == 0 && strcmp(other->name, inst->name) == 0)
                    inst->parent = other;
            }
        }
        else
        {
            for (i = 0; i < idx; i++, other++)
            {
                if (other->type == 1 && other->unk6 == inst->unk6)
                    inst->parent = other;
            }
        }
        if (inst->parent == NULL)
            OSReport("INSTANCE ERROR\n");
        break;
    }
    case 14:
    {
        int id;

        id = fn_1_1434D4(inst->name, s_partIds, 12);
        if (id == -1)
        {
            id = 0;
            OSReport("parts_id error!! %s %d\n", obj->name, inst->idx);
        }
        inst->unk4 = (u16)id;
        break;
    }
    case 15:
    {
        int id;

        id = fn_1_1434D4(inst->name, s_texIds, 4);
        if (id == -1)
        {
            id = 0;
            OSReport("tex_id error!! %s %d\n", obj->name, inst->idx);
        }
        inst->unk4 = (u16)id;
        break;
    }
    case 12:
    {
        int i;
        struct Instance *other;

        other = player->instances;
        if (strcmp(inst->name, "SARU") == 0)
        {
            for (i = 0; i < idx; i++, other++)
            {
                if (other->type == 1 && other->unk6 == inst->unk6)
                {
                    if (other->ape != NULL)
                        inst->parent = other;
                    break;
                }
            }
        }
        else if (strcmp(inst->name, "SOFTFX") != 0)
        {
            for (i = 0; i < idx; i++, other++)
            {
                if (other->type == 0 && strcmp(other->name, inst->name) == 0)
                    inst->parent = other;
            }
        }
        break;
    }
    case 6:
    case 7:
    case 8:
        inst->lightIdx = player->lightCount;
        size = 0x40;
        inst->lightBuf = alloc_from_heap_or_panic(currentHeap, size, "scnplay.c", 0x39D);
        memset(inst->lightBuf, 0, 0x40);
        memset(&light, 0, sizeof(light));
        light.id = 1;
        light.inst = inst->lightIdx;
        switch (inst->type)
        {
        case 6:
            light.type = 0;
            break;
        case 7:
            light.type = 1;
            break;
        case 8:
            light.type = 2;
            break;
        }
        add_light_to_pool(&light);
        break;
    case 16:
    case 17:
        break;
    }
    return 1;
}

void fn_1_141E48(void)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    struct Instance *inst;
    int i;

    if (player->sceneId == 0)
        return;
    if (!(g_some_other_flags & 0xA) && player->done)
        player->frame++;
    if (player->frame > player->duration)
    {
        g_something_freeing_heap_4();
        return;
    }
    i = player->count;
    inst = player->instances;
    for (; i > 0; i--, inst++)
    {
        if (!(g_some_other_flags & 0xA) || !player->done)
            fn_1_142018(player, inst);
    }
    if (!(g_some_other_flags & 0xA))
    {
        fn_1_162CD4(player);
        fn_1_162D28(player);
    }
    if (player->done || fn_1_162D2C(player))
    {
        player->done = 1;
        fn_1_143E68();
    }
}

void fn_1_141F7C(u32 mask)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    int i;
    struct Instance *inst;

    if (player->sceneId != 0)
    {
        fn_1_143F34(1);
        i = player->count;
        inst = player->instances;
        for (; i > 0; i--, inst++)
        {
            if (mask & (1 << inst->type))
                fn_1_142720(inst);
        }
        fn_1_143F34(2);
    }
}

void fn_1_142018(struct ScenePlayer *player, struct Instance *inst)
{
    struct AnimJoint *joint = inst->joints;
    float frame = player->frame;

    u_interpolate_joint_motion(joint, NULL, NULL, frame, 0);
    inst->flags &= ~6;
    if (__fabs(joint->unk20C - 1.0f) < 1.1920928955078125e-07)
    {
        if (!(inst->flags & 1))
            inst->flags |= 0xB;
    }
    else
    {
        if (inst->flags & 1)
        {
            inst->flags &= ~1;
            inst->flags |= 4;
        }
    }
    switch (inst->type)
    {
    case 6:
    {
        struct Light *l;
        struct AnimJoint *j = inst->joints;
        Vec dir;
        s16 rotX, rotY;

        l = alloc_pool_light(1, inst->lightIdx);
        if (l != NULL)
        {
            l->red = j->scale.x;
            l->green = j->scale.y;
            l->blue = j->scale.z;
            dir = j->pos;
            vec_normalize_len(&dir);
            vec_to_euler_xy(&dir, &rotX, &rotY);
            l->rotX = rotX;
            l->rotY = rotY;
        }
        break;
    }
    case 7:
    {
        struct Light *l;
        struct AnimJoint *j = inst->joints;
        struct AnimJoint *j2 = j + 1;

        l = alloc_pool_light(1, inst->lightIdx);
        if (l != NULL)
        {
            l->red = j->scale.x;
            l->green = j->scale.y;
            l->blue = j->scale.z;
            l->pos = j->pos;
            l->refDist = j2->scale.y;
            l->k0 = 1.0f;
            l->k1 = 0.5f;
        }
        break;
    }
    case 8:
    {
        struct Light *l;
        struct AnimJoint *j = inst->joints;
        struct AnimJoint *j2 = j + 1;
        Vec pos;
        Vec dir;
        s16 rotX, rotY;

        l = alloc_pool_light(1, inst->lightIdx);
        if (l != NULL)
        {
            l->red = j->scale.x;
            l->green = j->scale.y;
            l->blue = j->scale.z;
            pos = j->pos;
            dir.x = j2->pos.x - pos.x;
            dir.y = j2->pos.y - pos.y;
            dir.z = j2->pos.z - pos.z;
            vec_normalize_len(&dir);
            vec_to_euler_xy(&dir, &rotX, &rotY);
            l->pos = pos;
            l->rotX = rotX;
            l->rotY = rotY;
            l->spotCutoff = 57.295776f * (j2->rot.x + 0.5f * j2->rot.y);
            l->refDist = j2->scale.y;
            l->k0 = 1.0f;
            l->k1 = 0.0f;
        }
        break;
    }
    case 10:
        player->unk28 = joint->pos;
        break;
    case 9:
        player->unk1C = joint->pos;
        break;
    case 11:
        player->unk18 = 10430.378f * joint->rot.x;
        player->unk16 = 10430.378f * joint->rot.z;
        break;
    case 2:
        if (inst->flags & 2)
            fn_1_1141D8(inst->parent->ape, player->unkC + 27, inst->name);
        break;
    case 14:
        if (inst->flags & 2)
            inst->parent->ape->unk21C |= s_partMasks[inst->unk4];
        else if (inst->flags & 4)
            inst->parent->ape->unk21C &= ~s_partMasks[inst->unk4];
        mtxa_from_translate(&joint->pos);
        mtxa_mult_right(joint->rotateMtx);
        mtxa_scale(&joint->scale);
        mtxa_to_mtx(inst->mtx);
        break;
    case 15:
        if (inst->flags & 2)
            inst->parent->ape->unk220 |= s_texMasks[inst->unk4];
        else if (inst->flags & 4)
            inst->parent->ape->unk220 &= ~s_texMasks[inst->unk4];
        break;
    case 1:
        g_ape_anim_body(inst->ape);
        break;
    case 4:
        if (inst->flags & 2)
        {
            if (inst->dialog == NULL)
                inst->dialog = alloc_from_heap_or_panic(currentHeap, 0xA0, "scnplay.c", 0x4D0);
            if (inst->parent != NULL)
                fn_1_143558(inst->dialog, inst->name + strlen("SARU"), inst->parent->ape);
            else
                fn_1_143558(inst->dialog, inst->name + strlen("SERIFU"), NULL);
        }
        else if (inst->flags & 4)
            fn_1_1607F4(inst->dialog, 0);
        break;
    case 12:
        if (inst->flags & 2)
            g_something_to_do_with_soft_fx(inst, inst->unk4, inst->parent);
        else if (inst->flags & 4)
            fn_1_1440B0(inst);
        break;
    case 3:
        if (inst->parent != NULL)
        {
            struct AnimJoint *parentJoint = inst->parent->joints;

            if (player->done && inst->unk4 != 0)
            {
                Vec up = {0.0f, 1.0f, 0.0f};
                Vec delta;
                Quat q;

                PSVECSubtract(&parentJoint->pos, &inst->pos, &delta);
                g_math_unk8(&q, 0.5f * joint->scale.x, &delta, &up);
                quat_mult(&q, &q, &inst->rot);
                quat_normalize(&q);
                mtxa_from_quat(&q);
                mathutil_mtxA_set_translate(&parentJoint->pos);
                inst->rot = q;
            }
            else
            {
                mtxa_from_translate(&parentJoint->pos);
                mtxa_mult_right(joint->rotateMtx);
                mtxa_to_quat(&inst->rot);
            }
            inst->pos = parentJoint->pos;
            mtxa_scale(&joint->scale);
            mtxa_to_mtx(inst->mtx);
        }
        break;
    case 13:
        if (inst->flags & 1)
            g_ape_anim_head(inst->parent->ape, &joint->pos);
        break;
    case 5:
        mtxa_from_translate(&joint->pos);
        mtxa_mult_right(joint->rotateMtx);
        mtxa_scale(&joint->scale);
        mtxa_to_mtx(inst->mtx);
        break;
    default:
        mtxa_from_translate(&joint->pos);
        mtxa_mult_right(joint->rotateMtx);
        mtxa_scale(&joint->scale);
        mtxa_to_mtx(inst->mtx);
        break;
    case 16:
        break;
    }
}

void fn_1_142720(struct Instance *inst)
{
    if (!(inst->flags & 1) && (inst->type != 4 || !(inst->flags & 8)))
        return;
    switch (inst->type)
    {
    case 16:
    {
        struct AnimJoint *joint = inst->joints;

        g_smth_with_avdisp_globals(joint->scale.x, joint->scale.y, joint->scale.z);
        break;
    }
    case 17:
    {
        struct SpriteDrawRequest req;
        struct AnimJoint *joint = inst->joints;
        u8 r = 255.0f * joint->scale.x;
        u8 g = 255.0f * joint->scale.y;
        int b = (u8)(255.0f * joint->scale.z);

        req.mulColor = (r << 16) + ((g << 8) + b);
        req.opacity = joint->pos.y;
        req.bmpId = 0x4B;
        req.x = 320.0f;
        req.y = 240.1f;
        req.z = 0.25f;
        req.scaleX = 128.0f;
        req.scaleY = 60.0f;
        req.u1 = 0.0f;
        req.v1 = 0.0f;
        req.u2 = 1.0f;
        req.v2 = 1.0f;
        req.rotation = 0;
        req.unk30 = -1;
        req.flags = 0x2000A;
        req.addColor = 0;
        draw_sprite_draw_request(&req);
        break;
    }
    case 3:
    {
        Vec v;
        int id;

        id = fn_1_1434D4(inst->name, lbl_1_data_6EF28, 8);
        if (inst->parent != NULL)
        {
            if (id == -1)
                id = 0;
            mtxa_from_mtxb_mult_mtx(inst->mtx);
            v.x = inst->mtx[0][0];
            v.y = inst->mtx[1][0];
            v.z = inst->mtx[2][0];
            g_smth_with_rendefc_reflective_height(-mathutil_vec_len(&v));
            fn_1_4EAFC(mtxa->mtxA, id, 3);
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        break;
    }
    case 1:
        if (inst->ape != NULL)
        {
            struct AnimJoint *joint = inst->joints;

            inst->ape->pos = joint->pos;
            if (inst->ape->charaId == 0 || inst->ape->charaId == 1 || inst->ape->charaId == 2 || inst->ape->charaId == 3)
                inst->ape->pos.y += -0.012f * joint->scale.y;
            inst->ape->scale = joint->scale.x;
            mtxa_from_identity();
            mtxa_mult_right(joint->rotateMtx);
            mtxa_to_quat(&inst->ape->rot);
            draw_ape(inst->ape);
        }
        break;
    case 14:
    {
        struct GMAModel *model;
        struct AnimJoint *joint = inst->joints;

        model = fn_1_10F79C(inst->parent->ape->modelTable, inst->name);
        if (model == NULL)
            model = init_common_gma->modelEntries[90].model;
        mtxa_from_mtxb_mult_mtx(inst->mtx);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_scale_factor(MAX(joint->scale.x, MAX(joint->scale.y, joint->scale.z)));
        avdisp_draw_model_culled_sort_auto(model);
        break;
    }
    case 5:
        if (strcmp(inst->name, "SARU") == 0)
        {
            struct AnimJoint *joint = inst->joints;

            inst->parent->ape->pos = joint->pos;
            if (inst->parent->ape->charaId == 0 || inst->parent->ape->charaId == 1
             || inst->parent->ape->charaId == 2 || inst->parent->ape->charaId == 3)
                inst->parent->ape->pos.y += -0.012f * joint->scale.y;
            inst->parent->ape->scale = joint->scale.x;
            mtxa_from_identity();
            mtxa_mult_right(joint->rotateMtx);
            mtxa_to_quat(&inst->parent->ape->rot);
            draw_ape(inst->parent->ape);
            break;
        }
    case 0:
    {
        struct AnimJoint *joint = inst->joints;
        struct GMAModel *model;

        model = get_GmaBuffer_entry(stage_gma, inst->name);
        if (model == NULL)
            model = get_GmaBuffer_entry(g_bg_gma, inst->name);
        if (model == NULL)
            model = get_GmaBuffer_entry(init_common_gma, inst->name);
        if (model == NULL && (dip_switches & 0x04000000))
            model = init_common_gma->modelEntries[90].model;
        if (model != NULL)
        {
            mtxa_from_mtxb_mult_mtx(inst->mtx);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_set_scale_factor(MAX(joint->scale.x, MAX(joint->scale.y, joint->scale.z)));
            avdisp_draw_model_culled_sort_auto(model);
        }
        break;
    }
    case 4:
        fn_1_15FB94(inst->dialog);
        break;
    }
}

int load_scene_data(int category)
{
    static char *files[] = {

        "scndat_SCENE1A.bin",
        "scndat_SCENE1B.bin",
        "scndat_SCENE2.bin",
        "scndat_SCENE3.bin",
        "scndat_SCENE4.bin",
        "scndat_SCENE5.bin",
        "scndat_SCENE6.bin",
        "scndat_SCENE7.bin",
        "scndat_SCENE8.bin",
        "scndat_SCENE9.bin",
        "scndat_SCENE10.bin",
        "scndat_SCENELAST.bin",
        "scndat_ENDING1.bin",
        "scndat_ENDING2.bin",
        "scndat_ENDING3.bin",
        "scndat_ENDING4.bin",
    };
    struct File file;
    int total = 0;
    u32 size;

    if (category < 0 || category >= 16)
    {
        OSReport("Scene Category Error\n");
        return 0;
    }
    if (!file_open(files[category], &file))
        return 0;
    if (scene_data != NULL)
        return 0;
    size = ALIGN32(file_size(&file));
    scene_data = alloc_from_heap_or_panic(currentHeap, size, "scnplay.c", 0x67D);
    file_read(&file, scene_data, size, 0);
    file_close(&file);
#ifndef __MWERKS__
    {
        /* pc_port: scndat_*.bin is a big-endian Scene array image */
        extern void pc_swap_scene_data(void *data, u32 size);
        pc_swap_scene_data(scene_data, size);
    }
#endif
    g_parse_scene_data(scene_data);
    total += size;
    g_active_scene_category = category;
    OSReport("%s Loaded     : 0x%08x\n", files[category], scene_data);
    return total;
}

void g_parse_scene_data(struct Scene *scenes)
{
    struct Scene *scene;
    struct SceneObj *obj;
    int n;
    int i;

    n = 0;
    scene = scenes;
    for (; scene->objs != NULL; n++, scene++)
    {
        scene->objs = OFFSET_TO_PTR(scenes, scene->objs);
        scene->name = OFFSET_TO_PTR(scenes, scene->name);
        obj = scene->objs;
        i = scene->numObjs;
        for (; i > 0; i--, obj++)
        {
            struct SceneAnim *anim;

            obj->anim = OFFSET_TO_PTR(scenes, obj->anim);
            obj->name = OFFSET_TO_PTR(scenes, obj->name);
            anim = obj->anim;
            anim->jointFlags = OFFSET_TO_PTR(scenes, anim->jointFlags);
            anim->keyframeCounts = OFFSET_TO_PTR(scenes, anim->keyframeCounts);
            anim->times = OFFSET_TO_PTR(scenes, anim->times);
            anim->valueTypes = OFFSET_TO_PTR(scenes, anim->valueTypes);
            anim->values = OFFSET_TO_PTR(scenes, anim->values);
        }
    }
    lbl_1_bss_DD4EC = n;
}

void fn_1_142ECC(void)
{
    memset(lbl_1_bss_DD52C, 0, sizeof(lbl_1_bss_DD52C));
}

void fn_1_142EFC(struct Scene *scene) {}

void fn_1_142F00(void) {}

void g_something_freeing_heap_4(void)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    struct Instance *inst = player->instances;
    int i = player->count;

    for (; i > 0; i--, inst++)
    {
        if (inst->joints != NULL)
        {
            OSFreeToHeap(currentHeap, inst->joints);
            inst->joints = NULL;
        }
        if (inst->lightBuf != NULL)
        {
            OSFreeToHeap(currentHeap, inst->lightBuf);
            inst->lightBuf = NULL;
        }
        if (inst->dialog != NULL)
        {
            fn_1_1607F4(inst->dialog, 1);
            fn_1_15FB70(inst->dialog);
            OSFreeToHeap(currentHeap, inst->dialog);
            inst->dialog = NULL;
        }
        if (inst->ape != NULL)
        {
            g_something_with_freeing_chara_heap_ape(inst->ape);
            inst->ape = NULL;
        }
    }
    if (player->instances != NULL)
    {
        OSFreeToHeap(currentHeap, player->instances);
        player->count = 0;
        player->instances = NULL;
    }
    player->sceneId = 0;
    fn_1_144020();
    fn_1_152D70();
}

void g_something_with_cutscenes_func2(void)
{
    if (scene_data != NULL)
    {
        OSFreeToHeap(currentHeap, scene_data);
        OSReport("scndat Free !!    : 0x%08x\n", scene_data);
        scene_data = NULL;
        lbl_1_bss_DD4EC = 0;
        g_active_scene_category = 16;
    }
    fn_1_144020();
    fn_1_152D70();
    fn_1_162C8C();
    if (dialog_tbl != NULL)
    {
        fn_1_162978(dialog_tbl);
        OSReport("dialog_tbl Free !!   : 0x%08x\n", dialog_tbl);
        dialog_tbl = NULL;
    }
}

int fn_1_143104(u16 sceneId)
{
    if (sceneId == 0 || sceneId > lbl_1_bss_DD4EC)
        return -1;
    return scene_data[sceneId - 1].stage;
}

int g_something_with_cutscenes_func(u16 sceneId)
{
    if (sceneId == 0 || sceneId > lbl_1_bss_DD4EC)
        return -1;
    return scene_data[sceneId - 1].unk10;
}

char *fn_1_14319C(u16 sceneId)
{
    if (sceneId == 0 || sceneId > lbl_1_bss_DD4EC)
        return "SN_NULL";
    return scene_data[sceneId - 1].name;
}

void fn_1_1431EC(struct AnimJoint *joints, u16 count)
{
    mtxa_from_identity();
    for (; count != 0; count--, joints++)
    {
        joints->flags = 0x20000;
        mtxa_to_mtx(joints->rotateMtx);
        mtxa_to_mtx(joints->unk270);
    }
    joints->flags = 0;
}

void fn_1_14325C(struct AnimJoint *joints)
{
    u16 zero = 0;
    struct MotionChannel *chan;
    u8 i;

    for (; joints->flags != 0; joints++)
    {
        chan = joints->channels;
        i = 10;
        for (; i != 0; i--, chan++)
        {
            chan->currKeyframe = zero;
            chan->keyframeCount = zero;
        }
    }
}

void fn_1_1432A0(struct AnimJoint *joints, struct SceneAnim *anim)
{
    struct SceneJointFlags *flags = anim->jointFlags;
    u8 i = 0;
    u16 mask;

    for (; joints->flags != 0; i++, joints++)
    {
        if (flags->joint == i)
        {
            mask = flags->mask;
            if (mask & 0x200)
                fn_1_143428(anim, &joints->channels[0]);
            if (mask & 0x100)
                fn_1_143428(anim, &joints->channels[1]);
            if (mask & 0x80)
                fn_1_143428(anim, &joints->channels[2]);
            if (mask & 0x40)
                fn_1_143428(anim, &joints->channels[3]);
            if (mask & 0x20)
                fn_1_143428(anim, &joints->channels[4]);
            if (mask & 0x10)
                fn_1_143428(anim, &joints->channels[5]);
            if (mask & 8)
                fn_1_143428(anim, &joints->channels[6]);
            if (mask & 4)
                fn_1_143428(anim, &joints->channels[7]);
            if (mask & 2)
                fn_1_143428(anim, &joints->channels[8]);
            if (mask & 1)
                fn_1_143428(anim, &joints->channels[9]);
            flags++;
        }
    }
}

void fn_1_143428(struct SceneAnim *anim, struct MotionChannel *chan)
{
    u16 count = *anim->keyframeCounts;
    u16 total;
    u8 *types;

    if (chan != NULL)
    {
        chan->keyframeCount = count;
        chan->times = anim->times;
        chan->valueTypes = anim->valueTypes;
        chan->values = anim->values;
    }
    anim->keyframeCounts++;
    anim->times += count;
    types = anim->valueTypes;
    anim->valueTypes += count;
    total = 0;
    for (; count != 0; count--, types++)
        total += s_keyframeValueCounts[*types];
    anim->values += total;
}

int fn_1_1434D4(char *name, char **list, u8 count)
{
    int i;

    for (i = 0; i < count; i++, list++)
    {
        if (strcmp(name, *list) == 0)
            return i;
    }
    return -1;
}

void fn_1_143558(void *dlg, char *name, struct Ape *ape)
{
    char *str;
    void *data;

    fn_1_15FB34(dlg, 0x100);
    str = fn_1_1436D4(name);
    if (str != NULL)
    {
        data = fn_1_1629D0(dialog_tbl, str);
        if (data != NULL)
        {
            fn_1_160518(dlg, data);
            fn_1_1607DC(dlg, ape);
            fn_1_1607E4(dlg);
            fn_1_160870(dlg);
        }
        else
            OSReport("No Dialog Data %s\n", str);
    }
}

void fn_1_143614(void)
{
    u32 flags = 0;

    if ((modeCtrl.courseFlags & 0x800008) && !(modeCtrl.courseFlags & 0x40))
        flags |= 1;
    if ((modeCtrl.courseFlags & 0x800008) && (modeCtrl.courseFlags & 0x40) && mode_info.livesLost == 0)
    {
        if (modeCtrl.courseFlags & 0x800000)
            flags |= 4;
        else
            flags |= 2;
    }
    lbl_1_bss_DD63C = flags;
}

char *fn_1_1436D4(char *name)
{
    struct DialogSwap *swap;
    int found;
    u32 flags = lbl_1_bss_DD63C;

    if (flags == 0)
        return name;
    found = 0;
    swap = lbl_1_data_6F11C;
    for (; swap->from != NULL; swap++)
    {
        if (strcmp(name, swap->from) == 0)
        {
            if (swap->flags & flags)
                found = 1;
            if (found)
                break;
        }
    }
    if (!found)
        return name;
    return swap->to;
}

void fn_1_143798(int stage)
{
    destroy_sprite_with_unique_id(50);
    event_dest(EVENT_CAMERA);
    event_dest(EVENT_EFFECT);
    event_init(EVENT_EFFECT);
    event_init(EVENT_REND_EFC);
    load_stage(stage);
    if (events[EVENT_STAGE].state == EV_STATE_INACTIVE)
        event_init(EVENT_STAGE);
    if (events[EVENT_BACKGROUND].state == EV_STATE_INACTIVE)
        event_init(EVENT_BACKGROUND);
    if (events[EVENT_SPRITE].state == EV_STATE_INACTIVE)
        event_init(EVENT_SPRITE);
    event_init(EVENT_CAMERA);
    if (events[EVENT_SOUND].state == EV_STATE_INACTIVE)
        event_init(EVENT_SOUND);
    enable_camera(0);
    camera_set_state(0x54);
}

void g_dump_scene_info(int sceneId)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    struct Scene *scene = scene_data + sceneId - 1;
    u16 count = scene->numObjs;
    struct SceneObj *obj;
    int id;
    u16 i;

    if (sceneId == 0 || sceneId > lbl_1_bss_DD4EC)
        return;
    OSReport("================== Scene_Info_Dump( %d ) =====================\n", sceneId);
    OSReport("STAGE:%d\n", fn_1_143104(sceneId));
    i = 0;
    obj = scene->objs;
    for (; i < count; i++, obj++)
    {
        if (obj->type == 1)
        {
            id = fn_1_1434D4(obj->name, s_charaIds, 17);
            if (id == -1)
                OSReport("CHARA NUM Error : %s\n", obj->name);
            else
                OSReport("CHARA:%s LOD:%d Motion:%s\n", s_charaIds[id], obj->unkC, obj->name);
        }
    }
}
