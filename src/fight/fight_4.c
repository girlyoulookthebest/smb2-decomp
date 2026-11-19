/* Monkey Fight 2: stage gimmicks (.text 0x17714-0x1B40C). Each stage type has init/tick/exit
 * functions in lbl_12_data_850: timed item box/item drops (types 1-3), the swinging platforms with
 * corner balloons (type 4), type 5 (fight_5.c) and the collapsing floor (type 6). */
#include "fight.h"
#include "stobj.h"

/* DOL / main_loop */
void *fn_1_58118(void *callback);  /* stage.c: sets the per-frame stage callback, returns the old one */
struct GMAModel *fn_1_58280(struct StageModelRef *ref);
void get_curr_stage_fly_in_position(struct Sphere *sphere);
int spawn_stobj(struct Stobj *stobj);
struct GMAModel *get_GmaBuffer_entry(struct GMA *gma, char *name);
void call_SoundReqID_arg_0(int soundId);
void g_stores_doubles2(float r, float g, float b, float a);
void mtxa_from_rotate_x(s16 angle);
extern void *g_some_buffer_ptr;
extern u32 g_some_other_flags;
extern u32 g_font_char_var_3;
extern struct PoolInfo g_poolInfo;
extern struct ModeInfo mode_info;
extern s16 current_stage_id;
extern struct GMA *stage_gma;

/* other fight TUs */
void fn_12_205E8(struct FightStartSet *sets, int count);  /* fight_7.c: camera targets */
void fn_12_1B40C(void);  /* fight_5.c: stage type 5 */
void fn_12_1B684(void);
void fn_12_1B998(void);


/* Stage type 4: one of the three swinging platforms (lbl_12_bss_1EE38.work->parts). */
struct FightPendulum
{
    /*0x00*/ s16 animGroupId;  /* item group anim id this platform drives (1..3) */
    /*0x02*/ u16 flags;        /* 1: shake the camera on the next bounce, 2: drop items on the next bounce */
    /*0x04*/ s32 state;        /* 0 hanging, 1 dropped */
    /*0x08*/ s32 hitPoints;
    /*0x0C*/ u8 playerCooldown[4];
    /*0x10*/ Vec dir;          /* rope direction (y, z) */
    /*0x1C*/ Vec vel;
    /*0x28*/ Mtx mtx;
    /*0x58*/ Mtx baseMtx;
    /*0x88*/ Vec color;
    /*0x94*/ Vec addColor;
    /*0xA0*/ u8 *itemGroups;   /* item group indices, 0xFF terminated */
};  /* size 0xA4 on GC */

struct FightStage4Work
{
    /*0x000*/ struct FightStageSpawner spawner;
    /*0x00C*/ s32 timer;
    /*0x010*/ float swing;
    /*0x014*/ s16 swingAngle;
    /*0x016*/ s16 tiltAngle;
    /*0x018*/ Mtx mtx;
    /*0x048*/ Mtx prevMtx;
    /*0x078*/ u32 dropPending;
    /*0x07C*/ struct FightPendulum parts[3];
    /*0x268*/ u8 itemGroupLists[0x80];
    /*0x2E8*/ struct FightPendulum *camTarget;
};  /* size 0x2EC on GC */

/* Stage type 6: one falling floor piece (an item group). */
struct FightDebris
{
    /*0x00*/ s16 state;
    /*0x02*/ s16 timer;
    /*0x04*/ s16 timer2;
    /*0x06*/ u8 filler6[2];
    /*0x08*/ Vec pos;
    /*0x14*/ Vec prevPos;
    /*0x20*/ Vec vel;
    /*0x2C*/ S16Vec rot;
    /*0x32*/ S16Vec prevRot;
    /*0x38*/ S16Vec rotVel;
    /*0x3E*/ u8 filler3E[2];
    /*0x40*/ Vec center;
    /*0x4C*/ float radius;
};  /* size 0x50 */

struct FightStage6Work
{
    /*0x0000*/ s32 frame;
    /*0x0004*/ struct FightDebris pieces[0x80];
};  /* size 0x2804 */

/* scratch in g_some_buffer_ptr (stage type 6 init) */
struct FightDebrisDist
{
    Vec pos;
    float dist;
};

struct FightStageFuncs
{
    void (*init)(void);
    void (*tick)(void);
    void (*exit)(void);
};

void fn_12_177D4(void);
void fn_12_17BA0(struct FightStageSpawner *spawner);
int fn_12_182E8(void);
int fn_12_1804C(void);
void fn_12_19030(struct FightStageHit *hit);
void fn_12_19300(void);
void fn_12_19730(struct FightPendulum *part);
void fn_12_19AD4(struct Camera *camera, struct Ball *ball);
void fn_12_19D74(struct Camera *camera, struct Ball *ball);
void fn_12_1A234(void);
void fn_12_1A238(struct FightBalloon *balloon);
void fn_12_1A2C0(struct GMAModel *model, int count, struct FightRibbonJoint *joints);
void fn_12_18194(void);
void fn_12_181C0(void);
void fn_12_181EC(void);
void fn_12_181F0(void);
void fn_12_1823C(void);
void fn_12_18268(void);
void fn_12_1826C(void);
void fn_12_182B8(void);
void fn_12_182E4(void);
void fn_12_1832C(void);
void fn_12_1869C(void);
void fn_12_1902C(void);
void fn_12_1A2C4(void);
void fn_12_1A990(void);
void fn_12_1B404(void);

/* ---- .data 0x6E0 ---- */
/* the same unreferenced table every minigame TU starts its .data with */
static u8 lbl_12_data_6E0[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

struct FightStageFuncs lbl_12_data_850[7] = {
    { NULL, NULL, NULL },
    { fn_12_18194, fn_12_181C0, fn_12_181EC },
    { fn_12_181F0, fn_12_1823C, fn_12_18268 },
    { fn_12_1826C, fn_12_182B8, fn_12_182E4 },
    { fn_12_1832C, fn_12_1869C, fn_12_1902C },
    { fn_12_1B40C, fn_12_1B684, fn_12_1B998 },
    { fn_12_1A2C4, fn_12_1A990, fn_12_1B404 },
};

/* item box drops of stage types 1-3 (by remaining time and player count) */
struct FightStageSpawn lbl_12_data_8A4[16] = {
    { 0.8333333f, 1, 0xC, { 0, 0, 0 }, { 0 }, { -4.0f, 4.0f, 0.0f } },
    { 0.8333333f, 1, 0xC, { 0, 0, 0 }, { 0 }, { 4.0f, 6.0f, 0.0f } },
    { 0.8333333f, 1, 0x2, { 0, 0, 0 }, { 0 }, { -6.0f, 4.0f, 0.0f } },
    { 0.8333333f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 6.0f, 0.0f } },
    { 0.8333333f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 6.0f, 8.0f, 0.0f } },
    { 0.5f, 1, 0xC, { 0, 0, 0 }, { 0 }, { 0.0f, 4.0f, -6.0f } },
    { 0.5f, 1, 0xC, { 0, 0, 0 }, { 0 }, { 0.0f, 6.0f, 6.0f } },
    { 0.5f, 1, 0x2, { 0, 0, 0 }, { 0 }, { -0.0f, 4.0f, -6.0f } },
    { 0.5f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 6.0f, 0.0f } },
    { 0.5f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 8.0f, 6.0f } },
    { 0.25f, 1, 0xC, { 0, 0, 0 }, { 0 }, { -4.0f, 4.0f, 0.0f } },
    { 0.25f, 1, 0xC, { 0, 0, 0 }, { 0 }, { 4.0f, 6.0f, 0.0f } },
    { 0.25f, 1, 0x2, { 0, 0, 0 }, { 0 }, { -6.0f, 4.0f, 0.0f } },
    { 0.25f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.01f, 6.0f, 0.01f } },
    { 0.25f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 6.0f, 8.0f, 0.0f } },
    { -100.0f, 0, 0x0, { 0, 0, 0 }, { 0 }, { 0.0f, 0.0f, 0.0f } },
};

struct FightStageSpawn lbl_12_data_A64[16] = {
    { 0.8333333f, 1, 0xC, { 0, 0, 0 }, { 0 }, { 0.0f, 6.0f, 0.4f } },
    { 0.8283333f, 1, 0xC, { 0, 0x5555, 0 }, { 0 }, { 0.0f, 9.0f, -0.4f } },
    { 0.8333333f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 6.0f, 0.2f } },
    { 0.8283333f, 1, 0x2, { 0, 0x5555, 0 }, { 0 }, { 0.2f, 9.0f, -0.1f } },
    { 0.8233333f, 1, 0x2, { 0, -0x5555, 0 }, { 0 }, { -0.2f, 12.0f, -0.1f } },
    { 0.5f, 1, 0xC, { 0, 0, 0 }, { 0 }, { -0.4f, 6.0f, 0.0f } },
    { 0.495f, 1, 0xC, { 0, -0x8000, 0 }, { 0 }, { 0.4f, 9.0f, 0.0f } },
    { 0.5f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 6.0f, -0.2f } },
    { 0.495f, 1, 0x2, { 0, 0x5555, 0 }, { 0 }, { 0.2f, 9.0f, 0.1f } },
    { 0.49f, 1, 0x2, { 0, -0x5555, 0 }, { 0 }, { -0.2f, 12.0f, 0.1f } },
    { 0.25f, 1, 0xC, { 0, 0, 0 }, { 0 }, { 0.0f, 6.0f, 0.4f } },
    { 0.245f, 1, 0xC, { 0, -0x8000, 0 }, { 0 }, { 0.0f, 9.0f, -0.4f } },
    { 0.25f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 6.0f, 0.2f } },
    { 0.245f, 1, 0x2, { 0, 0x5555, 0 }, { 0 }, { 0.2f, 9.0f, -0.1f } },
    { 0.24f, 1, 0x2, { 0, -0x5555, 0 }, { 0 }, { -0.2f, 12.0f, -0.1f } },
    { -100.0f, 0, 0x0, { 0, 0, 0 }, { 0 }, { 0.0f, 0.0f, 0.0f } },
};

struct FightStageSpawn lbl_12_data_C24[16] = {
    { 0.8333333f, 1, 0xC, { 0, 0x2000, 0 }, { 0 }, { 4.5f, 4.0f, 0.0f } },
    { 0.8283333f, 1, 0xC, { 0, -0x2000, 0 }, { 0 }, { -4.5f, 4.0f, 0.0f } },
    { 0.8333333f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 4.0f, 2.0f } },
    { 0.8283333f, 1, 0x2, { 0, 0x5555, 0 }, { 0 }, { 2.0f, 4.0f, -1.0f } },
    { 0.8233333f, 1, 0x2, { 0, -0x5555, 0 }, { 0 }, { -2.0f, 4.0f, -1.0f } },
    { 0.5f, 1, 0xC, { 0, 0x2000, 0 }, { 0 }, { 0.0f, 4.0f, 5.0f } },
    { 0.495f, 1, 0xC, { 0, 0x2000, 0 }, { 0 }, { 0.0f, 4.0f, -5.0f } },
    { 0.5f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 4.0f, -5.0f } },
    { 0.495f, 1, 0x2, { 0, 0x5555, 0 }, { 0 }, { 5.0f, 4.0f, 2.5f } },
    { 0.49f, 1, 0x2, { 0, -0x5555, 0 }, { 0 }, { -5.0f, 4.0f, 2.5f } },
    { 0.25f, 1, 0xC, { 0, 0x2000, 0 }, { 0 }, { 5.0f, 4.0f, 0.0f } },
    { 0.245f, 1, 0xC, { 0, -0x2000, 0 }, { 0 }, { -5.0f, 4.0f, 0.0f } },
    { 0.25f, 1, 0x2, { 0, 0, 0 }, { 0 }, { 0.0f, 4.0f, 5.0f } },
    { 0.245f, 1, 0x2, { 0, 0x5555, 0 }, { 0 }, { 5.0f, 4.0f, -2.5f } },
    { 0.24f, 1, 0x2, { 0, -0x5555, 0 }, { 0 }, { -5.0f, 4.0f, -2.5f } },
    { -100.0f, 0, 0x0, { 0, 0, 0 }, { 0 }, { 0.0f, 0.0f, 0.0f } },
};

/* hit points of the type 4 platforms, by player count */
static s16 lbl_12_data_DE4[4] = { 0, 25, 28, 32 };

/* camera targets of the type 4 platforms */
static struct FightStartPos lbl_12_data_DEC[2] = {
    { 2.0f, { 10.0f, 5.0f, 0.0f } },
    { 2.0f, { -10.0f, 5.0f, 0.0f } },
};

static struct FightStartSet lbl_12_data_E0C[4] = {
    { 0, 0, NULL },
    { 2, 2, lbl_12_data_DEC },
    { 3, 2, lbl_12_data_DEC },
    { 4, 2, lbl_12_data_DEC },
};

/* type 4 drops nothing on a timer */
static struct FightStageSpawn lbl_12_data_E2C[1] = {
    { -100.0f, 0, 0, { 0, 0, 0 }, { 0 }, { 0.0f, 0.0f, 0.0f } },
};

/* corner balloon models */
static char *lbl_12_data_EB0[4][4] = {
    { "BALLOON_G", "RIBON_UP_skin", "RIBON_DOWN_R_skin", "RIBON_DOWN_L_skin" },
    { "BALLOON_P", "RIBON_UP_skin", "RIBON_DOWN_R_skin", "RIBON_DOWN_L_skin" },
    { "BALLOON_OR", "RIBON_UP_skin", "RIBON_DOWN_R_skin", "RIBON_DOWN_L_skin" },
    { "BALLOON_B", "RIBON_UP_skin", "RIBON_DOWN_R_skin", "RIBON_DOWN_L_skin" },
};

struct FightBalloon lbl_12_data_EF0[4] = {
    { { -10.0f, 10.0f, -10.0f }, 0, -0x6000, 0 },
    { { 10.0f, 10.0f, -10.0f }, 0, 0x6000, 0 },
    { { 10.0f, 10.0f, 10.0f }, 0, 0x2000, 0 },
    { { -10.0f, 10.0f, 10.0f }, 0, -0x2000, 0 },
};

struct FightStageCameraFuncs lbl_12_data_99A0 = { fn_12_19AD4, fn_12_19D74, fn_12_1A234 };

extern struct FightStartPos lbl_12_bss_1EDF8[4];

/* Restarts the current stage type and installs the per-frame stage callback. */
void fn_12_17714(void)
{
    if (lbl_12_bss_1EE38.type != 0)
        lbl_12_data_850[lbl_12_bss_1EE38.type].exit();
    lbl_12_bss_1EE38.cameraFuncs = NULL;
    lbl_12_bss_1EE38.unk30 = 0;
    if (lbl_12_bss_1EE38.type != 0)
        lbl_12_data_850[lbl_12_bss_1EE38.type].init();
    fn_1_58118(fn_12_177D4);
}

/* Per-frame stage callback: ticks the stage type unless the game is paused. */
void fn_12_177D4(void)
{
    if (!(g_some_other_flags & 0xA) && lbl_12_bss_1EE38.type != 0)
        lbl_12_data_850[lbl_12_bss_1EE38.type].tick();
}

/* Ends the stage gimmick. */
void fn_12_17844(void)
{
    if (lbl_12_bss_1EE38.type != 0)
    {
        lbl_12_data_850[lbl_12_bss_1EE38.type].exit();
        lbl_12_bss_1EE38.type = 0;
    }
    fn_1_58118(NULL);
    memset(lbl_12_bss_1EE38.hooks, 0, sizeof(lbl_12_bss_1EE38.hooks));
}

/* A punch hit a stage item group. */
void fn_12_178CC(struct FightStageHit *hit)
{
    if (lbl_12_bss_1EE38.hitFunc != NULL)
        lbl_12_bss_1EE38.hitFunc(hit);
}

/* Sets up the stage gimmick of the given type and the start position camera targets. */
void fn_12_17910(int type)
{
    int i;
    struct StartPos *startPos;
    struct FightStartPos *pos;

    if (lbl_12_bss_1EE38.type != 0)
        lbl_12_data_850[lbl_12_bss_1EE38.type].exit();
    get_curr_stage_fly_in_position(&lbl_12_bss_1EE38.flyIn);
    lbl_12_bss_1EDF0.unk0 = 0;
    lbl_12_bss_1EDF0.count = (*(s32 *)stagedef->unk6C < 4) ? *(s32 *)stagedef->unk6C : 4;
    lbl_12_bss_1EDF0.list = lbl_12_bss_1EDF8;
    pos = lbl_12_bss_1EDF8;
    startPos = stagedef->startPos;
    for (i = lbl_12_bss_1EDF0.count; i > 0; i--, startPos++, pos++)
    {
        pos->unk0 = 4.0f;
        pos->pos = startPos->pos;
    }
    fn_12_205E8(&lbl_12_bss_1EDF0, 1);
    lbl_12_bss_1EE38.type = type;
    lbl_12_bss_1EE38.hitFunc = NULL;
    memset(lbl_12_bss_1EE38.hooks, 0, sizeof(lbl_12_bss_1EE38.hooks));
    if (type != 0)
        lbl_12_data_850[type].init();
}

/* Which side an item group belongs to on the current stage (bit 0: it moves with the floor). */
u32 fn_12_17AB8(int itemGroup)
{
    struct StageCollHeader *coll = &stagedef->collHeaders[itemGroup];
    int side;

    switch (current_stage_id)
    {
    case 0x90:
    case 0x92:
        if (coll->animGroupId == 0)
            side = 1;
        else
            side = 2;
        break;
    case 0x91:
    case 0x93:
        if (coll->animGroupId == 2)
            side = 2;
        else if (coll->animGroupId == 1)
            side = 1;
        else
            side = 0;
        break;
    case 0x8F:
    case 0x94:
        if (coll->animGroupId == 0)
            side = 1;
        else if (coll->animGroupId == 2)
            side = 2;
        else
            side = 0;
        break;
    default:
        side = 1;
        break;
    }
    return side;
}

/* Rewinds a spawn list. */
void fn_12_17B94(struct FightStageSpawner *spawner)
{
    spawner->next = spawner->list;
}

/* Spawns the item boxes and items whose time has come, plus a random box between two start positions every 512 frames. */
void fn_12_17BA0(struct FightStageSpawner *spawner)
{
    float ratio;
    float cx, cy, cz;
    struct FightStageSpawn *spawn;
    u32 mask;
    int i;
    int j;
    struct Stobj stobj;
    struct Item item;
    struct RaycastHit hit;
    Vec v;
    Vec a;
    Vec b;

    mask = 1 << (modeCtrl.playerCount - 1);
    if (lbl_12_bss_1ACA0.flags & 0x100)
        return;
    ratio = (float)mode_info.timerCurr / (float)mode_info.timerMax;
    for (spawn = spawner->next; spawn->time > ratio; spawn++)
    {
        if (!(mask & spawn->playerMask))
            continue;
        switch (spawn->kind)
        {
        case 1:
            if (fn_12_182E8() >= spawner->maxObjs)
                break;
            memset(&stobj, 0, sizeof(stobj));
            stobj.type = 8;
            stobj.animGroupId = 0;
            stobj.rotX = spawn->rot.x;
            stobj.rotY = spawn->rot.y;
            stobj.rotZ = spawn->rot.z;
            stobj.u_some_pos = spawn->pos;
            spawn_stobj(&stobj);
            break;
        case 2:
            memset(&item, 0, sizeof(item));
            item.type = 4;
            item.subType = fn_12_1804C();
            item.animGroupId = 0;
            item.pos = spawn->pos;
            item.vel.y = 1.0f / 12.0f;
            if ((int)raycast_stage_down(&item.pos, &hit, &v) == 0)
            {
                cx = 0.0f;
                cy = 0.0f;
                cz = 0.0f;
                v.x = cx - item.pos.x;
                v.y = 0.0f;
                v.z = cz - item.pos.z;
                vec_set_len(1.0f / 12.0f, &v, &v);
                if (cy > item.pos.y)
                    v.y = (cy - item.pos.y) / 30.0f;
                item.vel.x += v.x;
                item.vel.y += v.y;
                item.vel.z += v.z;
            }
            spawn_item(&item);
            break;
        }
    }
    spawner->next = spawn;

    if ((lbl_12_bss_1ACA0.flags & 4) && !(g_font_char_var_3 & 0x1FF) && fn_12_182E8() < spawner->maxObjs)
    {
        i = (rand() & 0x7FFF) % *(s32 *)stagedef->unk6C;
        j = (rand() & 0x7FFF) % *(s32 *)stagedef->unk6C;
        if (i == j)
        {
            j++;
            j %= *(s32 *)stagedef->unk6C;
        }
        a = stagedef->startPos[i].pos;
        b = stagedef->startPos[j].pos;
        memset(&stobj, 0, sizeof(stobj));
        stobj.type = 8;
        stobj.animGroupId = 0;
        stobj.rotX = 0;
        stobj.rotY = rand() & 0x7FFF;
        stobj.rotZ = 0;
        stobj.u_some_pos.x = 0.5f * (a.x + b.x) + (RAND_FLOAT() - 0.5f);
        stobj.u_some_pos.y = 6.0f + 0.5f * (a.y + b.y);
        stobj.u_some_pos.z = 0.5f * (a.z + b.z) + (RAND_FLOAT() - 0.5f);
        spawn_stobj(&stobj);
    }
}

/* Exit of a spawn list (nothing to do). */
void fn_12_18048(struct FightStageSpawner *spawner) {}

/* Random punch item subtype; the third kind gets likelier as time runs out. */
int fn_12_1804C(void)
{
    float weights[4];
    float total;
    float r;
    float t;
    int i;
    float *w;

    t = 1.0f - (float)mode_info.timerCurr / (float)mode_info.timerMax;
    weights[0] = 0.0f;
    weights[1] = 2.0f;
    weights[2] = 2.0f * t;
    weights[3] = 2.0f;
    total = 0.0f;
    for (w = weights, i = 4; i > 0; i--, w++)
        total += *w;
    r = total * RAND_FLOAT();
    total = 0.0f;
    for (w = weights, i = 0; i < 3; i++, w++)
    {
        if (r < *w + total)
            break;
        total += *w;
    }
    return i;
}

/* Stage type 1: init. */
void fn_12_18194(void)
{
    struct FightStageSpawner *spawner = lbl_12_bss_1EE38.work;

    spawner->list = lbl_12_data_8A4;
    spawner->maxObjs = 16;
    spawner->next = spawner->list;
}

/* Stage type 1: tick. */
void fn_12_181C0(void)
{
    fn_12_17BA0(lbl_12_bss_1EE38.work);
}

/* Stage type 1: exit. */
void fn_12_181EC(void) {}

/* Stage type 2: init. */
void fn_12_181F0(void)
{
    struct FightStageSpawner *spawner = lbl_12_bss_1EE38.work;

    spawner->list = lbl_12_data_A64;
    if (modeCtrl.playerCount < 4)
        spawner->maxObjs = 4;
    else
        spawner->maxObjs = 3;
    spawner->next = spawner->list;
}

/* Stage type 2: tick. */
void fn_12_1823C(void)
{
    fn_12_17BA0(lbl_12_bss_1EE38.work);
}

/* Stage type 2: exit. */
void fn_12_18268(void) {}

/* Stage type 3: init. */
void fn_12_1826C(void)
{
    struct FightStageSpawner *spawner = lbl_12_bss_1EE38.work;

    spawner->list = lbl_12_data_C24;
    if (modeCtrl.playerCount < 4)
        spawner->maxObjs = 4;
    else
        spawner->maxObjs = 2;
    spawner->next = spawner->list;
}

/* Stage type 3: tick. */
void fn_12_182B8(void)
{
    fn_12_17BA0(lbl_12_bss_1EE38.work);
}

/* Stage type 3: exit. */
void fn_12_182E4(void) {}

/* Number of item boxes in play. */
int fn_12_182E8(void)
{
    int count = 0;
    struct FightBox *box = lbl_12_bss_118;
    int i;

    for (i = g_poolInfo.stobjPool.count; i > 0; i--, box++)
    {
        if (box->stobj != NULL)
            count++;
    }
    return count;
}

/* Stage type 4: init. Three platforms hang from the item groups 1-3, four balloons mark the corners. */
void fn_12_1832C(void)
{
    int i;
    struct FightStage4Work *work;
    struct FightPendulum *part;
    u8 *list;
    int left;
    s32 hitPoints;
    int id;
    int j;
    struct StageCollHeader *coll;
    int count;
    struct FightBalloon *balloon;
    char **names;
    struct Effect effect;

    work = lbl_12_bss_1EE38.work;
    memset(work, 0, sizeof(*work));
    lbl_12_bss_1EE38.hitFunc = fn_12_19030;
    work->spawner.list = lbl_12_data_E2C;
    work->spawner.maxObjs = 16;
    work->spawner.next = work->spawner.list;
    lbl_12_bss_1EE38.hooks[2] = fn_12_19300;
    lbl_12_data_E0C[0] = lbl_12_bss_1EDF0;
    lbl_12_data_E0C[0].unk0 = 1;
    fn_12_205E8(lbl_12_data_E0C, 4);
    lbl_12_bss_1EE38.cameraFuncs = &lbl_12_data_99A0;
    work->dropPending = 1;

    list = work->itemGroupLists;
    left = 0x7F;
    hitPoints = lbl_12_data_DE4[modeCtrl.playerCount - 1];
    part = work->parts;
    for (i = 0; i < 3; i++, part++)
    {
        id = i + 1;
        part->animGroupId = id;
        part->flags = 3;
        part->state = 0;
        part->hitPoints = hitPoints;
        part->dir.x = 0.0f;
        part->dir.y = 0.0f;
        part->dir.z = -1.0f;
        mtxa_from_rotate_x(1024.0f * (RAND_FLOAT() - 0.5f));
        mtxa_tf_vec(&part->dir, &part->dir);
        vec_normalize_len(&part->dir);
        mtxa_from_identity();
        mtxa_rotate_y((i + 1) << 14);
        mtxa_translate_xyz(0.0f, 0.0f, -10.0f);
        mtxa_to_mtx(part->baseMtx);
        mtxa_from_identity();
        mtxa_to_mtx(part->mtx);
        part->color.x = 1.0f;
        part->color.y = 1.0f;
        part->color.z = 1.0f;
        part->addColor.x = 0.0f;
        part->addColor.y = 0.0f;
        part->addColor.z = 0.0f;
        part->itemGroups = list;
        count = stagedef->itemGroupCount;
        coll = stagedef->collHeaders;
        if (left > 0)
        {
            for (j = 0; j < count; j++, coll++)
            {
                if (coll->animGroupId == id)
                {
                    *list++ = j;
                    if (--left <= 0)
                        break;
                }
            }
            *list++ = 0xFF;
            left--;
        }
    }

    fn_1_6869C(0x35);
    balloon = lbl_12_data_EF0;
    names = lbl_12_data_EB0[0];
    for (i = 4; i > 0; i--, balloon++, names += 4)
    {
        balloon->balloonModel = get_GmaBuffer_entry(stage_gma, names[0]);
        balloon->ribbonModels[0] = get_GmaBuffer_entry(stage_gma, names[1]);
        balloon->ribbonModels[1] = get_GmaBuffer_entry(stage_gma, names[2]);
        balloon->ribbonModels[2] = get_GmaBuffer_entry(stage_gma, names[3]);
        fn_12_1A238(balloon);
        memset(&effect, 0, sizeof(effect));
        effect.type = 0x35;
        effect.model = (struct GMAModel *)balloon;
        spawn_effect(&effect);
    }
    work->timer = 600;
    mtxa_from_identity();
    mtxa_to_mtx(work->mtx);
    mtxa_to_mtx(work->prevMtx);
}

/* Stage type 4: tick. Swings the floor, bounces the platforms on their ropes and moves their item groups. */
void fn_12_1869C(void)
{
    int i;
    struct FightStage4Work *work;
    float gravZ;
    float gravY;
    struct ItemGroup *ig;
    struct FightPendulum *part;
    s32 frame;
    u8 *p;
    int j;
    int flash;
    s32 hp;
    u8 *cooldown;
    float d;
    Vec up;
    S16Vec rot;
    Vec v;

    work = lbl_12_bss_1EE38.work;
    gravY = -1.0f;
    gravZ = 0.0f;
    fn_12_17BA0(&work->spawner);
    if (lbl_12_bss_1ACA0.flags & 4)
        work->timer--;
    if ((lbl_12_bss_1ACA0.flags & 4) && work->timer < 0)
    {
        work->swing += 4.551111f;
        work->swingAngle += 0xB6;
        work->tiltAngle += 0x6D;
    }
    else
        work->swing *= 0.95f;
    mtxa_from_rotate_z(work->tiltAngle);
    mtxa_rotate_x(work->swing * math_sin(work->swingAngle));
    mtxa_tf_vec_xyz(0.0f, 0.0f, -10.0f, &v);
    vec_to_euler(&v, &rot);
    mtxa_from_translate_xyz(0.0f, fabs(v.y), 0.0f);
    mtxa_rotate_y(rot.y);
    mtxa_rotate_x(rot.x);
    mtx_copy(work->mtx, work->prevMtx);
    mtxa_to_mtx(work->mtx);
    ig = &itemgroups[1];
    for (i = stagedef->itemGroupCount - 1; i > 0; i--, ig++)
    {
        mtx_copy(work->mtx, ig->transform);
        mtx_copy(work->prevMtx, ig->prevTransform);
    }

    /* near the end of the round all platforms drop */
    if (work->dropPending != 0 && !(lbl_12_bss_1ACA0.flags & 0x104)
     && (float)mode_info.timerCurr / (float)mode_info.timerMax < 0.13333334f)
    {
        work->dropPending = 0;
        part = work->parts;
        for (i = 3; i > 0; i--, part++)
        {
            if (part->state == 0)
            {
                part->state = 1;
                part->flags &= ~2;
                part->vel.z += -0.002f * RAND_FLOAT() - 0.002f;
            }
        }
    }

    frame = g_font_char_var_3;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = -0.3f;
    vec_normalize_len(&up);
    part = work->parts;
    for (i = 3; i > 0; i--, part++)
    {
        frame += 0x14;
        cooldown = part->playerCooldown;
        for (j = 4; j > 0; j--, cooldown++)
        {
            if (*cooldown != 0)
                (*cooldown)--;
        }
        part->addColor.x += 0.09f * (-0.05f - part->addColor.x);
        part->addColor.y += 0.092f * (-0.05f - part->addColor.y);
        part->addColor.z += 0.1f * (-0.05f - part->addColor.z);

        /* blink while low on hit points */
        flash = 0;
        if (part->state == 0)
        {
            hp = part->hitPoints;
            if (hp < 8 && !(frame & 0x1F))
                flash = 1;
            else if (hp < 16 && !(frame & 0x3F))
                flash = 1;
        }
        if (flash)
        {
            part->color.x = 1.0f;
            part->color.y = 0.8f;
            part->color.z = 0.75f;
            part->addColor.x += 0.2f;
            part->addColor.y += 0.05f;
            part->addColor.z += 0.05f;
        }
        else
        {
            if (part->color.x > 1.0f)
            {
                part->color.x += 0.1f * (0.95f - part->color.x);
                if (part->color.x < 1.0f)
                    part->color.x = 1.0f;
            }
            else if (part->color.x < 1.0f)
            {
                part->color.x += 0.1f * (1.05f - part->color.x);
                if (part->color.x > 1.0f)
                    part->color.x = 1.0f;
            }
            if (part->color.y > 1.0f)
            {
                part->color.y += 0.1f * (0.95f - part->color.y);
                if (part->color.y < 1.0f)
                    part->color.y = 1.0f;
            }
            else if (part->color.y < 1.0f)
            {
                part->color.y += 0.1f * (1.05f - part->color.y);
                if (part->color.y > 1.0f)
                    part->color.y = 1.0f;
            }
            if (part->color.z > 1.0f)
            {
                part->color.z += 0.1f * (0.95f - part->color.z);
                if (part->color.z < 1.0f)
                    part->color.z = 1.0f;
            }
            else if (part->color.z < 1.0f)
            {
                part->color.z += 0.1f * (1.05f - part->color.z);
                if (part->color.z > 1.0f)
                    part->color.z = 1.0f;
            }
        }

        /* pull a hanging platform back under its rope */
        if (part->state == 0)
        {
            v.x = 0.0f;
            v.y = 1.1f - part->dir.y;
            v.z = -part->dir.z;
            vec_set_len(0.012f, &v, &v);
            part->vel.x += v.x;
            part->vel.y += v.y;
            part->vel.z += v.z;
        }
        part->vel.y += 0.008f * gravY;
        part->vel.z += 0.008f * gravZ;
        part->vel.y *= 0.9f;
        part->vel.z *= 0.9f;
        d = -mathutil_vec_dot_prod(&part->dir, &part->vel);
        part->vel.y += d * part->dir.y;
        part->vel.z += d * part->dir.z;
        part->dir.y += part->vel.y;
        part->dir.z += part->vel.z;
        vec_normalize_len(&part->dir);

        /* bounce off the floor */
        d = mathutil_vec_dot_prod(&up, &part->dir);
        if (d < 0.0f)
        {
            part->dir.y = up.z;
            part->dir.z = -up.y;
            d = mathutil_vec_dot_prod(&up, &part->vel);
            if (d < 0.0f)
            {
                d *= -2.0f;
                part->vel.y += d * up.y;
                part->vel.z += d * up.z;
                if ((part->flags & 2) && part->state != 0)
                {
                    part->flags &= ~2;
                    fn_12_19730(part);
                    call_SoundReqID_arg_0(0x41);
                }
                if (part->flags & 1)
                {
                    part->flags &= ~1;
                    v.x = 0.0f;
                    v.y = -0.5f + -0.25f * RAND_FLOAT();
                    v.z = 0.0f;
                    camera_start_shake(0, 30, &v);
                    call_SoundReqID_arg_0(0x42);
                }
            }
        }
        if (part->dir.z > 0.0f)
        {
            if (part->flags & 4)
                part->flags &= ~4;
            part->dir.z = 0.0f;
            vec_normalize_len(&part->dir);
            if (part->vel.z > 0.0f)
                part->vel.z *= -1.0f;
        }

        mtxa_from_mtx(part->baseMtx);
        mtxa_rotate_x(math_atan2(part->dir.z, part->dir.y));
        mtxa_mult_left(work->mtx);
        for (p = part->itemGroups; *p != 0xFF; p++)
        {
            ig = &itemgroups[*p];
            mtxa_to_mtx(ig->transform);
            mtx_copy(part->mtx, ig->prevTransform);
        }
        mtxa_to_mtx(part->mtx);
    }
}

/* Stage type 4: exit. */
void fn_12_1902C(void) {}

/* Stage type 4: a punch hit an item group; hitting a platform pushes it and wears it down. */
void fn_12_19030(struct FightStageHit *hitArg)
{
    struct FightStage4Work *work;
    int i;
    struct FightStageHit hit;
    Vec v;
    int damage;
    struct FightPendulum *part;
    s32 animGroupId;
    float power;
    float glow;

    if (hitArg->itemGroup == 0)
        return;
    hit = *hitArg;
    work = lbl_12_bss_1EE38.work;
    animGroupId = stagedef->collHeaders[hit.itemGroup].animGroupId;
    part = work->parts;
    for (i = 3; i > 0; i--, part++)
    {
        if (animGroupId != part->animGroupId)
            continue;
        if (hit.type == 1)
        {
            if (part->playerCooldown[hit.playerId] != 0)
            {
                part->playerCooldown[hit.playerId] = 2;
                continue;
            }
            part->playerCooldown[hit.playerId] = 2;
        }
        power = fabs(hit.power);
        if (part->state == 0)
        {
            damage = 0;
            switch (hit.type)
            {
            case 0:
                if (power > 0.01f)
                    damage = 4;
                break;
            default:
                if (power > 0.01f)
                    damage = 1;
                break;
            }
            if ((float)damage > 0.0f)
            {
                part->hitPoints -= damage;
                if (part->hitPoints < 0)
                {
                    part->state = 1;
                    part->hitPoints = 0;
                    v.x = 0.0f;
                    v.y = 0.125f + 0.05f * RAND_FLOAT();
                    v.z = 0.0f;
                    camera_start_shake(0, 15, &v);
                }
            }
            glow = 0.125f * (float)damage;
            part->addColor.x += 0.5f * (1.1f * glow - part->addColor.x);
            part->addColor.y += 0.5f * (glow - part->addColor.y);
            part->addColor.z += 0.5f * (glow - part->addColor.z);
        }
        part->vel.z += -0.15f * (part->dir.y * power);
    }
}

/* Stage type 4 hook: draws the platforms again with their hit tint. */
void fn_12_19300(void)
{
    Vec color;
    int i;
    struct FightPendulum *part;
    struct GMAModel *model;
    u8 *p;
    Vec add;

    part = ((struct FightStage4Work *)lbl_12_bss_1EE38.work)->parts;
    for (i = 3; i > 0; i--, part++)
    {
        color = part->color;
        if (color.x > 1.0f)
            color.x = 1.0f;
        else if (color.x < 0.0f)
            color.x = 0.0f;
        if (color.y > 1.0f)
            color.y = 1.0f;
        else if (color.y < 0.0f)
            color.y = 0.0f;
        if (color.z > 1.0f)
            color.z = 1.0f;
        else if (color.z < 0.0f)
            color.z = 0.0f;
        add = part->addColor;
        if (add.x > 1.0f)
            add.x = 1.0f;
        else if (add.x < 0.0f)
            add.x = 0.0f;
        if (add.y > 1.0f)
            add.y = 1.0f;
        else if (add.y < 0.0f)
            add.y = 0.0f;
        if (add.z > 1.0f)
            add.z = 1.0f;
        else if (add.z < 0.0f)
            add.z = 0.0f;
        if (1.0f == color.x && 1.0f == color.y && 1.0f == color.z
         && 0.0f == add.x && 0.0f == add.y && 0.0f == add.z)
            continue;
        HFR_TAG(HFR_K_MINI, 0x46500000 | (u32)(3 - i));  /* PC HFR: tinted redraw per platform */
        mtxa_from_mtxb();
        mtxa_mult_right(itemgroups[*part->itemGroups].transform);
        p = part->itemGroups;
        while (*p != 0xFF)
        {
            model = fn_1_58280(*stagedef->collHeaders[*p].modelRefs);
            if (model != NULL)
            {
                if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
                {
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    set_post_mult_color(color.x, color.y, color.z, 1.0f);
                    g_stores_doubles2(add.x, add.y, add.z, 0.0f);
                    avdisp_draw_model_unculled_sort_never(model);
                }
                p++;
            }
        }
        HFR_UNTAG();
    }
    fade_color_base_default();
    g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
}

/* Stage type 4: a dropped platform landed; three punch items jump off it. */
void fn_12_19730(struct FightPendulum *part)
{
    struct Item item;
    Vec v;
    float cx, cy, cz;
    int i;

    memset(&item, 0, sizeof(item));
    item.type = 4;
    item.animGroupId = 0;
    mtxa_from_mtx(part->mtx);
    for (i = 3; i > 0; i--)
    {
        item.subType = fn_12_1804C();
        v.x = 5.0f * (float)i - 10.0f;
        v.y = 5.0f * (0.5f + 0.5f * RAND_FLOAT());
        v.z = 0.0f;
        mtxa_tf_point(&v, &item.pos);
        item.vel.x = 0.0f;
        item.vel.y = (1.0f + 0.5f * RAND_FLOAT()) / 12.0f;
        item.vel.z = 0.0f;
        item.unk10 = 60.0f * (0.36f + 0.4f * RAND_FLOAT());
        item.unk12 = item.unk10 + 0x1B8;
        cx = 0.0f;
        cy = 0.0f;
        cz = 0.0f;
        v.x = cx - item.pos.x;
        v.y = 0.0f;
        v.z = cz - item.pos.z;
        vec_set_len((0.25f + 1.25f * RAND_FLOAT()) / 12.0f, &v, &v);
        if (cy > item.pos.y)
            v.y = (cy - item.pos.y) / 30.0f;
        item.vel.x += v.x;
        item.vel.y += v.y;
        item.vel.z += v.z;
        spawn_item(&item);
    }
}

/* Stage type 4 camera, init: looks at a platform from a random side of the fly-in point. */
void fn_12_19AD4(struct Camera *camera, struct Ball *ball)
{
    struct FightStage4Work *work;
    struct Sphere flyIn;
    struct FightPendulum *part;
    Vec v;
    float len;

    work = lbl_12_bss_1EE38.work;
    flyIn = lbl_12_bss_1EE38.flyIn;
    if (work->camTarget == NULL)
    {
        part = &work->parts[1];
        work->camTarget = part;
    }
    else
        part = work->camTarget;
    mtxa_from_mtx(part->baseMtx);
    mtxa_rotate_x(math_atan2(part->dir.z, part->dir.y));
    mtxa_tf_point_xyz(0.0f, 5.0f, 0.0f, &camera->lookAt);
    camera->lookAtVel.x = 0.0f;
    camera->lookAtVel.y = 0.0f;
    camera->lookAtVel.z = 0.0f;
    v.x = 0.25f + 1.25f * RAND_FLOAT();
    if (rand() & 1)
        v.x = -v.x;
    v.x += flyIn.pos.x;
    v.y = 1.0f;
    v.z = flyIn.pos.z - (10.0f + 5.0f * RAND_FLOAT());
    v.x -= camera->lookAt.x;
    v.y -= camera->lookAt.y;
    v.z -= camera->lookAt.z;
    vec_normalize_len(&v);
    len = 3.0f + 2.0f * RAND_FLOAT();
    camera->eye.x = camera->lookAt.x + v.x * len;
    camera->eye.y = camera->lookAt.y + v.y * len;
    camera->eye.z = camera->lookAt.z + v.z * len;
    len = 0.1f + 0.1f * RAND_FLOAT();
    camera->eyeVel.x = v.x * len;
    camera->eyeVel.y = v.y * len;
    camera->eyeVel.z = v.z * len;
    camera->flags |= 1;
}

/* Stage type 4 camera, tick: drifts around the fly-in point and follows the platform while low. */
void fn_12_19D74(struct Camera *camera, struct Ball *ball)
{
    struct FightPendulum *part;
    Vec t;
    Vec eye;
    Vec lookAt;
    struct Sphere flyIn;
    Vec v;
    float len;
    float d;
    float f;

    part = ((struct FightStage4Work *)lbl_12_bss_1EE38.work)->camTarget;
    flyIn = lbl_12_bss_1EE38.flyIn;
    eye = camera->eye;
    lookAt = camera->lookAt;
    if (part->flags & 4)
    {
        camera->eye.x += camera->eyeVel.x;
        camera->eye.y += camera->eyeVel.y;
        camera->eye.z += camera->eyeVel.z;
    }
    else
    {
        v.x = camera->eye.x - flyIn.pos.x;
        v.y = 0.0f;
        v.z = camera->eye.z - flyIn.pos.z;
        if (vec_normalize_len(&v) > 11.0f)
        {
            t.x = camera->eyeVel.x;
            t.y = 0.0f;
            t.z = camera->eyeVel.z;
            len = mathutil_vec_len(&t);
            d = mathutil_vec_dot_prod(&v, &t);
            t.x -= 0.05f * d * v.x;
            t.z -= 0.05f * d * v.z;
            vec_set_len(len, &t, &t);
            camera->eyeVel.x = t.x;
            camera->eyeVel.z = t.z;
            camera->eyeVel.y += fabs(0.025f * d);
            camera->eyeVel.y *= 0.98f;
        }
        camera->eye.x += camera->eyeVel.x;
        camera->eye.y += camera->eyeVel.y;
        camera->eye.z += camera->eyeVel.z;
    }
    if (camera->eye.y < 5.0f)
    {
        mtxa_from_mtx(part->baseMtx);
        mtxa_rotate_x(math_atan2(part->dir.z, part->dir.y));
        mtxa_tf_point_xyz(0.0f, 5.0f, 0.0f, &t);
    }
    else if (camera->eye.y < 15.0f)
    {
        f = 0.1f * (camera->eye.y - 5.0f);
        mtxa_from_mtx(part->baseMtx);
        mtxa_rotate_x(math_atan2(part->dir.z, part->dir.y));
        mtxa_tf_point_xyz(0.0f, 5.0f, 0.0f, &t);
        t.x = t.x * (1.0f - f) + flyIn.pos.x * f;
        t.y = t.y * (1.0f - f) + flyIn.pos.y * f;
        t.z = t.z * (1.0f - f) + flyIn.pos.z * f;
    }
    else
        t = flyIn.pos;
    camera->lookAtVel.x += 0.02f * (0.02f * (t.x - camera->lookAt.x) - camera->lookAtVel.x);
    camera->lookAtVel.y += 0.02f * (0.02f * (t.y - camera->lookAt.y) - camera->lookAtVel.y);
    camera->lookAtVel.z += 0.02f * (0.02f * (t.z - camera->lookAt.z) - camera->lookAtVel.z);
    camera->lookAtVel.x *= 0.99f;
    camera->lookAtVel.y *= 0.99f;
    camera->lookAtVel.z *= 0.99f;
    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;
    t.x = camera->lookAt.x - camera->eye.x;
    t.y = camera->lookAt.y - camera->eye.y;
    t.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(t.x, t.z) - 0x8000;
    camera->rotX = math_atan2(t.y, math_sqrt(mathutil_sum_of_sq_2(t.x, t.z)));
    camera->rotZ = 0;
}

/* Stage type 4 camera (unused hook). */
void fn_12_1A234(void) {}

/* Sets up the ribbon joints of a corner balloon. */
void fn_12_1A238(struct FightBalloon *balloon)
{
    struct GMAModel *model;

    model = balloon->ribbonModels[0];
    if (model != NULL)
    {
        balloon->jointCounts[0] = model->mtxCount;
        fn_12_1A2C0(model, balloon->jointCounts[0], balloon->joints[0]);
    }
    model = balloon->ribbonModels[1];
    if (model != NULL)
    {
        balloon->jointCounts[1] = model->mtxCount;
        fn_12_1A2C0(model, balloon->jointCounts[1], balloon->joints[1]);
    }
    model = balloon->ribbonModels[2];
    if (model != NULL)
    {
        balloon->jointCounts[2] = model->mtxCount;
        fn_12_1A2C0(model, balloon->jointCounts[2], balloon->joints[2]);
    }
}

/* Ribbon joint setup (empty). */
void fn_12_1A2C0(struct GMAModel *model, int count, struct FightRibbonJoint *joints) {}

/* Stage type 6: init. Every item group becomes a floor piece that falls off in order, outside in. */
void fn_12_1A2C4(void)
{
    struct FightDebris *piece;
    int i;
    int count;
    struct FightStage6Work *work;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    struct FightDebrisDist *dists;
    struct FightDebrisDist *dist;
    struct GMAModel *model;
    float d;
    float invMax;
    float spin;
    float delay;
    float maxDist;
    float turns;
    Vec pos;

    work = lbl_12_bss_1EE38.work;
    work->frame = 0;
    if (stagedef->itemGroupCount - 1 > 0x80)
        count = 0x80;
    else
        count = stagedef->itemGroupCount - 1;
    ig = &itemgroups[1];
    coll = &stagedef->collHeaders[1];
    piece = work->pieces;
    for (i = 0; i < count; i++, piece++, ig++, coll++)
    {
        piece->state = 0;
        if (coll->modelRefCount != 0)
            model = fn_1_58280(*coll->modelRefs);
        else
            model = NULL;
        if (model != NULL)
        {
            piece->center = model->boundSphereCenter;
            piece->radius = model->boundSphereRadius;
        }
        else
        {
            piece->center.x = 0.0f;
            piece->center.y = 0.0f;
            piece->center.z = 0.0f;
            piece->radius = 1.0f;
        }
        mtxa_from_mtx(ig->transform);
        mtxa_translate(&piece->center);
        mathutil_mtxA_get_translate(&piece->pos);
        mtxa_to_euler(&piece->rot);
        mtxa_from_mtx(ig->prevTransform);
        mtxa_translate(&piece->center);
        mathutil_mtxA_get_translate(&piece->prevPos);
        mtxa_to_euler(&piece->prevRot);
        piece->vel.x = piece->pos.x - piece->prevPos.x;
        piece->vel.y = piece->pos.y - piece->prevPos.y;
        piece->vel.z = piece->pos.z - piece->prevPos.z;
        piece->rotVel.x = piece->rot.x - piece->prevRot.x;
        piece->rotVel.y = piece->rot.y - piece->prevRot.y;
        piece->rotVel.z = piece->rot.z - piece->prevRot.z;
    }

    dists = g_some_buffer_ptr;
    maxDist = 0.0f;
    ig = &itemgroups[1];
    piece = work->pieces;
    dist = dists;
    for (i = 0; i < count; i++, piece++, ig++, dist++)
    {
        mtxa_from_mtx(ig->transform);
        mtxa_tf_point(&piece->center, &pos);
        d = math_sqrt(mathutil_sum_of_sq_2(pos.x, pos.z)) + 0.25f * piece->radius;
        if ((fn_12_17AB8(i + 1) & 1) && maxDist < d)
            maxDist = d;
        dist->pos = pos;
        dist->dist = d;
    }

    turns = 6.0f * (1.0f + RAND_FLOAT());
    invMax = 1.0f / maxDist;
    spin = 2700.0f / (65536.0f * turns);
    dist = dists;
    piece = work->pieces;
    for (i = 0; i < count; i++, dist++, piece++)
    {
        if (!(fn_12_17AB8(i + 1) & 1))
        {
            piece->state = 1;
            delay = 1.0f + 60.0f * RAND_FLOAT();
            piece->timer = (s32)delay;
        }
        else
        {
            pos = dist->pos;
            d = dist->dist;
            delay = 2700.0f * (1.0f - d * invMax);
            delay += spin * (float)math_atan2(pos.x, pos.z);
            delay += 30.0f * RAND_FLOAT();
            if (delay < 0.0f)
                delay = 1.0f + 60.0f * RAND_FLOAT();
            piece->timer = (s32)delay;
            delay = 90.0f + 60.0f * (1.0f - d * invMax);
            piece->timer2 = (s32)delay;
        }
    }
}

/* Stage type 6: tick. Shakes the floor pieces loose and lets them fall. */
void fn_12_1A990(void)
{
    struct FightStage6Work *work;
    int i;
    struct FightDebris *piece;
    struct ItemGroup *ig;
    float s;
    Vec v;

    work = lbl_12_bss_1EE38.work;
    work->frame++;
    ig = &itemgroups[1];
    piece = work->pieces;
    for (i = (stagedef->itemGroupCount - 1 > 0x80) ? 0x80 : stagedef->itemGroupCount - 1; i > 0;
         i--, piece++, ig++)
    {
        switch (piece->state)
        {
        case 0:
            piece->vel.x *= 0.8f;
            piece->vel.y *= 0.8f;
            piece->vel.z *= 0.8f;
            if (piece->rotVel.x != 0)
                piece->rotVel.x = 0.8f * piece->rotVel.x;
            if (piece->rotVel.y != 0)
                piece->rotVel.y = 0.8f * piece->rotVel.y;
            if (piece->rotVel.z != 0)
                piece->rotVel.z = 0.8f * piece->rotVel.z;
            piece->timer2--;
            if (piece->timer2 < 0 || !(lbl_12_bss_1ACA0.flags & 0x40))
            {
                piece->state = 1;
                s = 0.01f;
                v.x = piece->pos.x * s;
                v.y = -(0.02f * RAND_FLOAT());
                v.z = piece->pos.z * s;
                piece->vel.x += v.x;
                piece->vel.y += v.y;
                piece->vel.z += v.z;
            }
            break;
        case 1:
            piece->vel.x *= 0.8f;
            piece->vel.y *= 0.8f;
            piece->vel.z *= 0.8f;
            if (piece->rotVel.x != 0)
                piece->rotVel.x = 0.8f * piece->rotVel.x;
            if (piece->rotVel.y != 0)
                piece->rotVel.y = 0.8f * piece->rotVel.y;
            if (piece->rotVel.z != 0)
                piece->rotVel.z = 0.8f * piece->rotVel.z;
            piece->timer--;
            if (piece->timer < 0)
                piece->state = 2;
            break;
        case 2:
            piece->state = 3;
            piece->timer = 60.0f * (0.25f + 0.25f * RAND_FLOAT());
            s = 1.0f / piece->radius;
            v.x = piece->pos.x;
            v.y = 0.0f;
            v.z = piece->pos.z;
            vec_set_len(s * (0.03f * RAND_FLOAT()), &v, &v);
            piece->vel.x += v.x;
            piece->vel.y += -0.1f * (s * RAND_FLOAT());
            piece->vel.z += v.z;
            s = 1024.0f / piece->radius;
            piece->rotVel.x += (s16)(s * (RAND_FLOAT() - 0.5f));
            piece->rotVel.y += (s16)(s * (RAND_FLOAT() - 0.5f));
            piece->rotVel.z += (s16)(s * (RAND_FLOAT() - 0.5f));
            /* fall through */
        case 3:
            piece->vel.x *= 0.8f;
            piece->vel.y *= 0.8f;
            piece->vel.z *= 0.8f;
            if (piece->rotVel.x != 0)
                piece->rotVel.x = 0.82f * piece->rotVel.x;
            if (piece->rotVel.y != 0)
                piece->rotVel.y = 0.82f * piece->rotVel.y;
            if (piece->rotVel.z != 0)
                piece->rotVel.z = 0.82f * piece->rotVel.z;
            piece->timer--;
            if (piece->timer < 0)
                piece->state = 4;
            break;
        case 4:
            piece->state = 5;
            s = (0.3f * RAND_FLOAT()) / (10.0f * piece->radius);
            v.x = piece->pos.x * s;
            v.z = piece->pos.z * s;
            piece->vel.x += v.x;
            piece->vel.z += v.z;
            s = 1280.0f / piece->radius;
            piece->rotVel.x += (s16)(s * (RAND_FLOAT() - 0.5f));
            piece->rotVel.y += (s16)(s * (RAND_FLOAT() - 0.5f));
            piece->rotVel.z += (s16)(s * (RAND_FLOAT() - 0.5f));
            /* fall through */
        case 5:
            piece->vel.y += -0.016f;
            piece->vel.x *= 0.98f;
            piece->vel.y *= 0.98f;
            piece->vel.z *= 0.98f;
            if (piece->rotVel.x != 0)
                piece->rotVel.x = 0.99f * piece->rotVel.x;
            if (piece->rotVel.y != 0)
                piece->rotVel.y = 0.99f * piece->rotVel.y;
            if (piece->rotVel.z != 0)
                piece->rotVel.z = 0.99f * piece->rotVel.z;
            break;
        }
        piece->prevPos = piece->pos;
        piece->pos.x += piece->vel.x;
        piece->pos.y += piece->vel.y;
        piece->pos.z += piece->vel.z;
        piece->prevRot = piece->rot;
        piece->rot.x += piece->rotVel.x;
        piece->rot.y += piece->rotVel.y;
        piece->rot.z += piece->rotVel.z;
        mtxa_from_translate(&piece->pos);
        mtxa_rotate_y(piece->rot.y);
        mtxa_rotate_x(piece->rot.x);
        mtxa_rotate_z(piece->rot.z);
        mtxa_translate_neg(&piece->center);
        mtxa_to_mtx(ig->transform);
        mtxa_from_translate(&piece->prevPos);
        mtxa_rotate_y(piece->prevRot.y);
        mtxa_rotate_x(piece->prevRot.x);
        mtxa_rotate_z(piece->prevRot.z);
        mtxa_translate_neg(&piece->center);
        mtxa_to_mtx(ig->prevTransform);
    }
}

/* Stage type 6: exit. */
void fn_12_1B404(void) {}

/* Called once by fight_3.c fn_12_176B8 after clearing the stage state (empty). */
void fn_12_1B408(void) {}

/* ---- .bss 0x1EDF0 (defined after the code: the functions address it by symbol; MWCC emits
 * .bss in reverse definition order, so these are defined last-to-first) ---- */
struct FightStage lbl_12_bss_1EE38;
struct FightStartPos lbl_12_bss_1EDF8[4];
struct FightStartSet lbl_12_bss_1EDF0;
