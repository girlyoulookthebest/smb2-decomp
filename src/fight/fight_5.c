/* Monkey Fight 2: the hanging-panel ring stage (12 panels under the ring that swing, drop and
   change colour when punched, and a ring tilt in the last seconds), and the fighter ball mode
   table and per-frame ball/player helpers. */
#include "fight.h"

void fn_12_17BA0(struct FightStageSpawner *spawn);
int fn_12_1804C(void);

/* fight_6.c */
void fn_12_1D534(struct Ball *ball);
void fn_12_1D970(struct Ball *ball);
void fn_12_1DB30(struct Ball *ball);
void fn_12_1DE74(struct Ball *ball);
void fn_12_1E2D8(struct Ball *ball);
void fn_12_1E3B8(struct Ball *ball);
void fn_12_1E454(struct Ball *ball);
void fn_12_1E64C(struct Ball *ball);
void fn_12_1E704(struct Ball *ball);
void fn_12_1E768(struct Ball *ball);
void fn_12_1ECA0(struct Ball *ball);
void fn_12_1F0C4(struct Ball *ball);
void fn_12_1F110(struct Ape *ape);
void fn_12_1F3E4(struct Ball *ball);
void fn_12_1F5A8(struct Ball *ball);
/* fight_7.c */
void fn_12_205E8(struct FightStartSet *sets, int count);

/* DOL / main_loop */
struct GMAModel *fn_1_58280(struct StageModelRef *ref);
void g_stores_doubles2(float a, float b, float c, float d);
void call_SoundReqID_arg_0(int soundId);
void g_create_stage_player_wrapper(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod,
                                   void (*func)(struct Ape *));
extern s32 active_monkey_id[];
void vec_to_euler(Vec *v, S16Vec *rot);
void fn_1_48654(struct Ball *ball);
void fn_1_486A0(struct Ball *ball);
void fn_1_487C8(struct Ball *ball);
void g_competition_mode_respawn_start(struct Ball *ball);
void fn_1_48BCC(struct Ball *ball);
void ball_physics_g_something_w_postgoal_slowdown(struct Ball *ball);
void ball_physics_g_something_w_postgoal_blast_up(struct Ball *ball);
void fn_1_48ED8(struct Ball *ball);
void g_ball_mode_play_replay(struct Ball *ball);
void fn_1_49470(struct Ball *ball);
void fn_1_49670(struct Ball *ball);
void fn_1_49674(struct Ball *ball);
void fn_1_49878(struct Ball *ball);
void fn_1_4987C(struct Ball *ball);
void fn_1_49CCC(struct Ball *ball);
void fn_1_49BD4(struct Ball *ball);
void g_smth_with_mystery_3(struct Ball *ball);
void fn_1_49EDC(struct Ball *ball);
void g_handle_competition_mode_respawn(struct Ball *ball);
void fn_1_4A18C(struct Ball *ball);
void fn_1_4A1AC(struct Ball *ball);
void ball_physics_g_something_w_poastgoal_slowdown_blast_up(struct Ball *ball);
void ball_physics_g_something_w_postgoal_blast_up2(struct Ball *ball);
void fn_2_71BC(struct Ball *ball);
void g_challenge_mode_start(struct Ball *ball);
void fn_2_73B4(struct Ball *ball);
void challenge_mode_physics(struct Ball *ball);

/* One hanging panel. */
struct FightPanel
{
    /*0x00*/ s8 index;
    /*0x01*/ u8 flags;  /* 1: drops a banana when it swings back, 2: reset (skip the motion blur once) */
    /*0x02*/ s16 itemGroup;
    /*0x04*/ s16 state;
    /*0x06*/ s16 timer;
    /*0x08*/ u8 hitTimer[4];  /* per player */
    /*0x0C*/ Vec pos;
    /*0x18*/ Vec vel;
    /*0x24*/ s32 swing1;
    /*0x28*/ float angle1;
    /*0x2C*/ float angVel1;
    /*0x30*/ s32 swing2;
    /*0x34*/ float angle2;
    /*0x38*/ float angVel2;
    /*0x3C*/ Vec target;
    /*0x48*/ float drop;
    /*0x4C*/ float dropVel;
    /*0x50*/ Vec color;
    /*0x5C*/ Mtx mtx;
    /*0x8C*/ Mtx baseMtx;
};  /* size 0xBC */

/* lbl_12_bss_1EE38.work on this stage */
struct FightPanelStage
{
    /*0x00*/ struct FightStageSpawner spawn;
    /*0x0C*/ s32 timer;
    /*0x10*/ float tilt;
    /*0x14*/ s16 tiltPhase;
    /*0x16*/ s16 tiltDir;
    /*0x18*/ Mtx mtx;
    /*0x48*/ Mtx prevMtx;
    /*0x78*/ u32 flags;  /* 1: a panel moved (sound) */
    /*0x7C*/ s16 mode;
    /*0x7E*/ s16 modeIndex;
    /*0x80*/ s16 modeTimer;
    /*0x82*/ s16 unk82;
    /*0x84*/ s16 wavePhase;
    /*0x86*/ s16 unk86;
    /*0x88*/ struct FightPanel panels[12];
};  /* size 0x958 on GC */

/* Panel mode schedule: `mode` starts once the remaining time fraction drops below `time`. */
struct FightPanelMode
{
    /*0x00*/ float time;
    /*0x04*/ s16 mode;
};

/* .data: the same unreferenced table several fight TUs start their .data with */
u8 lbl_12_data_99B0[0x170] = {
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

/* 0 panels down, 1 panels up, 2 drop every third panel, 3 alternating, 4 wave */
struct FightPanelMode lbl_12_data_9B20[8] = {
    { 1.0f, 0 },
    { 0.95f, 3 },
    { 0.53333336f, 2 },
    { 0.5f, 1 },
    { 0.46666667f, 4 },
    { 0.09166667f, 2 },
    { 0.058333334f, 1 },
    { -1.0f, 1 },
};

/* The start position on each panel. */
struct FightStartPos lbl_12_data_9B60 = { 2.0f, { 0.0f, 0.0f, -3.0f } };

/* Start sets: [0] is the ring's own (copied from lbl_12_bss_1EDF0), then one per panel. */
struct FightStartSet lbl_12_data_9B70[13] = {
    { 0, 0, NULL },
    { 100, 1, &lbl_12_data_9B60 },
    { 101, 1, &lbl_12_data_9B60 },
    { 102, 1, &lbl_12_data_9B60 },
    { 103, 1, &lbl_12_data_9B60 },
    { 104, 1, &lbl_12_data_9B60 },
    { 105, 1, &lbl_12_data_9B60 },
    { 106, 1, &lbl_12_data_9B60 },
    { 107, 1, &lbl_12_data_9B60 },
    { 108, 1, &lbl_12_data_9B60 },
    { 109, 1, &lbl_12_data_9B60 },
    { 110, 1, &lbl_12_data_9B60 },
    { 111, 1, &lbl_12_data_9B60 },
};

/* No timed spawns on this stage. */
struct FightStageSpawn lbl_12_data_9BD8[1] = {
    { -100.0f },
};

void fn_12_1D074(struct Camera *camera, struct Ball *ball);

struct FightStageCameraFuncs lbl_12_data_9BF4 = { fn_12_1D074, NULL, NULL };

/* Ball modes (ball->unk154): main_loop's, then the fighter modes from 36 on. */
void (*lbl_12_data_9C00[])(struct Ball *) = {
    fn_1_48654,
    fn_1_486A0,
    fn_1_487C8,
    g_competition_mode_respawn_start,
    fn_1_48BCC,
    ball_physics_g_something_w_postgoal_slowdown,
    ball_physics_g_something_w_postgoal_blast_up,
    fn_1_48ED8,
    g_ball_mode_play_replay,
    fn_1_48ED8,
    g_ball_mode_play_replay,
    fn_1_49470,
    fn_1_49670,
    fn_1_49674,
    fn_1_49878,
    fn_1_4987C,
    fn_1_49CCC,
    fn_1_49BD4,
    g_smth_with_mystery_3,
    fn_1_49EDC,
    g_handle_competition_mode_respawn,
    fn_1_4A18C,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_1_4A1AC,
    ball_physics_g_something_w_poastgoal_slowdown_blast_up,
    ball_physics_g_something_w_postgoal_blast_up2,
    fn_2_71BC,
    g_challenge_mode_start,
    fn_2_73B4,
    challenge_mode_physics,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_1_4A1AC,
    fn_12_1D970,
    fn_12_1DB30,
    fn_12_1DE74,
    fn_12_1E2D8,
    fn_12_1E3B8,
    fn_12_1E454,
    fn_12_1E64C,
    fn_12_1E704,
    fn_12_1E768,
    fn_12_1ECA0,
    fn_12_1F0C4,
    fn_12_1F3E4,
    NULL,
};

struct FightCharaParam lbl_12_data_9CC4[5] = {
    { 1.0f, 1.0f, 1.0f, 0.0f, 0 },
    { 0.75f, 1.0f, 1.25f, 0.2f, 0 },
    { 0.75f, 1.5f, 1.0f, 0.0f, 0 },
    { 3.0f, 0.25f, 1.0f, 0.15f, 0 },
    { 0.5f, 1.0f, 1.0f, 0.2f, 1 },
};

void fn_12_1B9C4(struct FightStageHit *hit);
void fn_12_1BEF4(struct FightPanel *panel);
void fn_12_1C98C(int index, Vec *out);
void fn_12_1CB78(struct FightPanelStage *stage);
void fn_12_1CDD4(void);
void fn_12_1D440(struct Ball *ball);

/* Panel stage init: clears the work, hangs a panel on each flagged item group, sets the start sets. */
void fn_12_1B40C(void)
{
    int i;
    struct FightPanelStage *stage;
    struct FightPanel *panel;
    s16 *setGroup;
    int n;
    float dist;

    stage = lbl_12_bss_1EE38.work;
    memset(stage, 0, sizeof(*stage));
    lbl_12_bss_1EE38.hitFunc = fn_12_1B9C4;
    stage->spawn.list = lbl_12_data_9BD8;
    stage->spawn.maxObjs = 16;
    fn_12_17B94(&stage->spawn);
    lbl_12_bss_1EE38.hooks[2] = fn_12_1CDD4;
    lbl_12_data_9B70[0] = lbl_12_bss_1EDF0;
    lbl_12_data_9B70[0].unk0 = 1;
    fn_12_205E8(lbl_12_data_9B70, 13);
    lbl_12_bss_1EE38.cameraFuncs = &lbl_12_data_9BF4;
    stage->mode = 1;
    stage->modeIndex = 0;
    dist = -10.0f * math_sin(0x4AAA);
    setGroup = &lbl_12_data_9B70[1].unk0;
    panel = stage->panels;
    n = 12;
    for (i = 0; i < stagedef->itemGroupCount && n > 0; i++)
    {
        if (fn_12_17AB8(i) & 2)
        {
            panel->index = 12 - n;
            panel->itemGroup = (s16)i;
            panel->swing1 = 1;
            panel->angle1 = -28672.0f * ((float)rand() / 32767.0f);
            panel->angVel1 = 0.0f;
            panel->swing2 = 1;
            panel->angle2 = 0.0f;
            panel->angVel2 = 0.0f;
            mtxa_from_identity();
            mtxa_rotate_y((i << 16) / 12 + 0xAAA);
            mtxa_translate_xyz(0.0f, 0.0f, dist);
            mtxa_rotate_x(0x4000);
            mtxa_to_mtx(panel->baseMtx);
            mtxa_from_identity();
            mtxa_to_mtx(panel->mtx);
            *setGroup = panel->itemGroup;
            panel++;
            setGroup += 4;
            n--;
        }
    }
    lbl_12_data_9B70[0] = lbl_12_bss_1EDF0;
    lbl_12_data_9B70[0].unk0 = 1;
    fn_12_205E8(lbl_12_data_9B70, 13);
    stage->timer = 600;
    mtxa_from_identity();
    mtxa_to_mtx(stage->mtx);
    mtxa_to_mtx(stage->prevMtx);
}

/* Stands in for a function the linker stripped from this file: only its -1.0f literal stays in
   the .rodata pool. Unreferenced, so it is stripped again. */
void fn_12_fight5_stripped(float *out)
{
    *out = -1.0f;
}

/* Panel stage tick: spawns, tilts the ring once the timer runs out, moves the panels. */
void fn_12_1B684(void)
{
    struct FightPanelStage *stage;
    Vec v;
    S16Vec rot;
    int i;
    struct FightPanel *panel;
    struct ItemGroup *ig;

    stage = lbl_12_bss_1EE38.work;
    fn_12_17BA0(&stage->spawn);
    if (lbl_12_bss_1ACA0.flags & 4)
        stage->timer--;
    if ((lbl_12_bss_1ACA0.flags & 4) && stage->timer < 0)
    {
        stage->tilt += 4.551111f;
        stage->tiltPhase += 0xB6;
        stage->tiltDir += 0x6D;
    }
    else
        stage->tilt *= 0.95f;
    mtxa_from_rotate_z(stage->tiltDir);
    mtxa_rotate_x(stage->tilt * math_sin(stage->tiltPhase));
    mtxa_tf_vec_xyz(0.0f, 0.0f, -10.0f, &v);
    vec_to_euler(&v, &rot);
    mtxa_from_translate_xyz(0.0f, fabs(v.y), 0.0f);
    mtxa_rotate_y(rot.y);
    mtxa_rotate_x(rot.x);
    mtx_copy(stage->mtx, stage->prevMtx);
    mtxa_to_mtx(stage->mtx);

    ig = &itemgroups[1];
    for (i = stagedef->itemGroupCount - 1; i > 0; i--, ig++)
    {
        mtx_copy(stage->mtx, ig->transform);
        mtx_copy(stage->prevMtx, ig->prevTransform);
    }
    fn_12_1CB78(stage);

    panel = stage->panels;
    for (i = 12; i > 0; i--, panel++)
    {
        fn_12_1BEF4(panel);
        mtxa_from_mtx(panel->baseMtx);
        mtxa_translate_xyz(panel->pos.x, panel->pos.y, panel->pos.z + panel->drop);
        mtxa_rotate_x(panel->angle1);
        mtxa_translate_xyz(0.0f, 0.0f, -panel->pos.z);
        mtxa_rotate_x(panel->angle2);
        mtxa_translate_xyz(0.0f, 0.0f, panel->pos.z);
        ig = &itemgroups[panel->itemGroup];
        mtxa_mult_left(stage->mtx);
        mtxa_to_mtx(ig->transform);
        if (!(panel->flags & 2))
            mtx_copy(panel->mtx, ig->prevTransform);
        else
        {
            panel->flags &= ~2;
            mtx_copy(panel->mtx, ig->prevTransform);
        }
        mtxa_to_mtx(panel->mtx);
    }

    if (stage->flags != 0)
    {
        if ((stage->flags & 1) && (float)g_some_frame_counter.frameCounter > 30.0f)
        {
            call_SoundReqID_arg_0(0x3F841);
            call_SoundReqID_arg_0(0x3F843);
        }
        stage->flags = 0;
    }
}

/* Panel stage destroy. */
void fn_12_1B998(void)
{
    fn_12_18048(lbl_12_bss_1EE38.work);
}

/* Hit on the stage: knocks the hit panel back, or flips it down on a hard punch (banana + sparks). */
void fn_12_1B9C4(struct FightStageHit *hit)
{
    struct Effect effect;
    struct Item item;
    struct FightStageHit h;
    Vec push;
    int i;
    struct FightPanel *panel;
    struct FightPanelStage *stage;
    float speed;
    float force;
    struct GMAModel *model;

    if (hit->itemGroup == 0)
        return;
    h = *hit;
    stage = lbl_12_bss_1EE38.work;
    mtxa_push();
    panel = stage->panels;
    for (i = 12; i > 0; i--, panel++)
    {
        if (h.itemGroup != panel->itemGroup)
            continue;
        if (h.type == 1)
        {
            if (panel->hitTimer[h.playerId] != 0)
            {
                panel->hitTimer[h.playerId] = 2;
                continue;
            }
            panel->hitTimer[h.playerId] = 2;
        }
        speed = fabs(h.power);
        if (h.type == 1)
            speed *= 0.1f;
        if (speed > 0.5f)
            speed = 0.5f;
        mtxa_from_mtx(panel->baseMtx);
        force = speed * (1.0f - panel->pos.z / 3.2f);
        if (force > 0.0f)
        {
            mtxa_rigid_inv_tf_vec(&h.dir, &push);
            vec_set_len(force, &push, &push);
            panel->dropVel -= push.y;
        }
        if (speed > 0.4f && panel->pos.z < 0.2f)
        {
            panel->flags |= 2;
            panel->pos.z = 3.3f;
            panel->target.z = 3.3f;
            panel->vel.x = 0.0f;
            panel->vel.y = 0.0f;
            panel->vel.z = 0.0f;
            panel->swing1 = 0;
            panel->angle1 = -28672.0f;
            panel->angVel1 = 0.0f;
            panel->swing2 = 1;
            panel->angle2 = 0.0f;
            panel->angVel2 = 0.0f;
            panel->drop = 0.0f;
            panel->dropVel = 0.0f;
            panel->state = 5;
            if (panel->flags & 1)
            {
                Vec center;

                panel->flags &= ~1;
                memset(&item, 0, sizeof(item));
                item.type = 4;
                item.subType = (s16)fn_12_1804C();
                item.animGroupId = 0;
                mtxa_from_mtx(panel->mtx);
                mathutil_mtxA_get_translate(&item.pos);
                item.vel.y = 0.083333336f;
                center.x = 0.0f;
                center.y = 0.0f;
                center.z = 0.0f;
                push.x = center.x - item.pos.x;
                push.y = 0.0f;
                push.z = center.z - item.pos.z;
                vec_set_len(0.083333336f, &push, &push);
                if (center.y > item.pos.y)
                    push.y = (center.y - item.pos.y) / 30.0f;
                item.vel.x += push.x;
                item.vel.y += push.y;
                item.vel.z += push.z;
                spawn_item(&item);
            }
            memset(&effect, 0, sizeof(effect));
            effect.type = 50;
            effect.model = (struct GMAModel *)stagedef->collHeaders[h.itemGroup].modelRefs[0];
            model = fn_1_58280((struct StageModelRef *)effect.model);
            mtxa_from_mtx(itemgroups[h.itemGroup].transform);
            mtxa_tf_point(&model->boundSphereCenter, &effect.pos);
            mtxa_to_euler_yxz(&effect.rotY, &effect.rotX, &effect.rotZ);
            effect.vel.x = h.power * h.dir.x;
            effect.vel.y = h.power * h.dir.y;
            effect.vel.z = h.power * h.dir.z;
            effect.unk98.x = 0.97f;
            effect.unk98.y = 0.93f;
            effect.unk98.z = 0.97f;
            effect.unk52 = 2048.0f * ((float)rand() / 32767.0f - 0.5f);
            effect.unk54 = 2048.0f * ((float)rand() / 32767.0f - 0.5f);
            effect.unk56 = 2048.0f * ((float)rand() / 32767.0f - 0.5f);
            spawn_effect(&effect);
        }
    }
    mtxa_pop();
}

/* Panel tick: follows the target position set by the stage mode, swings, fades the hit colour. */
void fn_12_1BEF4(struct FightPanel *panel)
{
    struct Item item;
    Vec t;
    Vec d;
    int i;
    u8 *timer;

    timer = panel->hitTimer;
    for (i = 4; i > 0; i--, timer++)
    {
        if (*timer != 0)
            (*timer)--;
    }
    panel->dropVel *= 0.9f;
    panel->dropVel += -0.4f * panel->drop;
    panel->drop += panel->dropVel;
    fn_12_1C98C(panel->index, &t);
    switch (panel->state)
    {
    case 0:
        if (t.z != panel->target.z || t.y != panel->target.y || t.x != panel->target.x)
        {
            struct FightPanelStage *stage = lbl_12_bss_1EE38.work;

            if (t.z > panel->target.z)
            {
                panel->state = 1;
                stage->flags |= 1;
            }
            else if (t.z < panel->target.z)
            {
                panel->state = 3;
                stage->flags |= 1;
            }
        }
        break;
    case 1:
        panel->state = 2;
        panel->timer = 15;
        panel->swing2 = 0;
        /* fall through */
    case 2:
        panel->timer--;
        if ((float)panel->timer > 6.0f)
        {
            panel->color.x += 0.3f * (2.0f - panel->color.x);
            panel->color.y += 0.3f * (1.0f - panel->color.y);
            panel->color.z += 0.3f * (1.0f - panel->color.z);
        }
        if (t.z == panel->target.z && t.y == panel->target.y && t.x == panel->target.x)
            panel->state = 0;
        if (panel->timer == 0)
        {
            panel->state = 0;
            panel->target = t;
            if (panel->flags & 1)
            {
                panel->flags &= ~1;
                memset(&item, 0, sizeof(item));
                item.type = 4;
                item.subType = (s16)fn_12_1804C();
                item.animGroupId = 0;
                mtxa_from_mtx(panel->mtx);
                mathutil_mtxA_get_translate(&item.pos);
                item.vel.y = 0.083333336f;
                t.x = 0.0f;
                t.y = 0.0f;
                t.z = 0.0f;
                d.x = t.x - item.pos.x;
                d.y = 0.0f;
                d.z = t.z - item.pos.z;
                vec_set_len(0.083333336f, &d, &d);
                if (t.y > item.pos.y)
                    d.y = (t.y - item.pos.y) / 30.0f;
                item.vel.x += d.x;
                item.vel.y += d.y;
                item.vel.z += d.z;
                spawn_item(&item);
            }
        }
        break;
    case 3:
        panel->state = 4;
        panel->timer = 15;
        panel->swing1 = 1;
        panel->swing2 = 1;
        /* fall through */
    case 4:
        panel->color.x += 0.3f * (2.5f - panel->color.x);
        panel->color.y += 0.3f * (2.4f - panel->color.y);
        panel->color.z += 0.3f * (1.0f - panel->color.z);
        if (t.z == panel->target.z && t.y == panel->target.y && t.x == panel->target.x)
            panel->state = 0;
        panel->timer--;
        if (panel->timer == 0)
        {
            panel->state = 0;
            panel->target = t;
        }
        break;
    case 5:
        if (3.3f == t.z)
            panel->state = 0;
        break;
    }

    t = panel->target;
    panel->vel.x *= 0.95f;
    panel->vel.y *= 0.98f;
    panel->vel.z *= 0.95f;
    d.x = t.x - panel->pos.x;
    d.y = t.y - panel->pos.y;
    d.z = t.z - panel->pos.z;
    if (d.x < 0.0f)
        panel->vel.x += -0.016f;
    else if (d.x > 0.0f)
        panel->vel.x += 0.016f;
    if (d.y < 0.0f)
        panel->vel.y += -0.016f;
    else if (d.y > 0.0f)
        panel->vel.y += 0.016f;
    if (d.z < 0.0f)
        panel->vel.z += -0.016f;
    else if (d.z > 0.0f)
        panel->vel.z += 0.016f;
    panel->pos.x += panel->vel.x;
    panel->pos.y += panel->vel.y;
    panel->pos.z += panel->vel.z;
    if (d.x * (t.x - panel->pos.x) < 0.0f)
    {
        panel->pos.x = t.x;
        if (panel->vel.x * d.x > 0.0f)
        {
            if (fabs(panel->vel.x) > 0.03f)
                panel->vel.x *= -0.5f;
            else
                panel->vel.x = 0.0f;
        }
    }
    if (d.y * (t.y - panel->pos.y) < 0.0f)
    {
        panel->pos.y = t.y;
        if (panel->vel.y * d.y > 0.0f)
        {
            if (fabs(panel->vel.y) > 0.03f)
                panel->vel.y *= -0.5f;
            else
                panel->vel.y = 0.0f;
        }
    }
    if (d.z * (t.z - panel->pos.z) < 0.0f)
    {
        panel->pos.z = t.z;
        if (panel->vel.z * d.z > 0.0f)
        {
            if (fabs(panel->vel.z) > 0.03f)
                panel->vel.z *= -0.5f;
            else
                panel->vel.z = 0.0f;
        }
    }

    if (panel->swing1 != 0)
    {
        panel->angVel1 += -64.0f * math_sin(panel->angle1);
        panel->angVel1 *= 0.96f;
        panel->angle1 += panel->angVel1;
        if (panel->angle1 > 0.0f)
        {
            panel->angle1 = 0.0f;
            if (panel->angVel1 > 0.0f)
                panel->angVel1 *= -(0.5f + 0.2f * ((float)rand() / 32767.0f));
        }
    }
    if (panel->swing2 != 0)
    {
        panel->angVel2 += -64.0f * math_sin(panel->angle2);
        panel->angVel2 *= 0.96f;
        panel->angle2 += panel->angVel2;
        if (panel->angle2 > 0.0f)
        {
            panel->angle2 = 0.0f;
            if (panel->angVel2 > 0.0f)
                panel->angVel2 *= -(0.3f + 0.2f * ((float)rand() / 32767.0f));
        }
    }
    else
    {
        panel->angVel2 += -64.0f;
        panel->angVel2 *= 0.96f;
        panel->angle2 += panel->angVel2;
        if (panel->angle2 < -9216.0f)
        {
            panel->angle2 = -9216.0f;
            if (panel->angVel2 < 0.0f)
                panel->angVel2 *= -(0.3f + 0.2f * ((float)rand() / 32767.0f));
        }
    }

    if (panel->color.x > 0.0f)
    {
        panel->color.x += 0.1f * (-0.01f - panel->color.x);
        if (panel->color.x < 0.0f)
            panel->color.x = 0.0f;
    }
    if (panel->color.y > 0.0f)
    {
        panel->color.y += 0.1f * (-0.01f - panel->color.y);
        if (panel->color.y < 0.0f)
            panel->color.y = 0.0f;
    }
    if (panel->color.z > 0.0f)
    {
        panel->color.z += 0.1f * (-0.01f - panel->color.z);
        if (panel->color.z < 0.0f)
            panel->color.z = 0.0f;
    }
}

/* Target offset of panel `index` for the current stage mode (z: 0.1 up, 3.3 down). */
void fn_12_1C98C(int index, Vec *out)
{
    Vec v;
    struct FightPanelStage *stage;
    int timer;

    if (lbl_12_bss_1ACA0.flags & 0x100)
    {
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 3.3f;
        *out = v;
        return;
    }
    stage = lbl_12_bss_1EE38.work;
    timer = stage->modeTimer;
    switch (stage->mode)
    {
    case 2:
        break;
    case 0:
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 0.1f;
        break;
    case 1:
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 3.3f;
        break;
    case 3:
        v.x = 0.0f;
        v.y = 0.0f;
        if (((timer << 6) + (index << 13)) & 0x2000)
            v.z = 0.1f;
        else
            v.z = 3.3f;
        break;
    case 4:
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = math_sin(stage->wavePhase + 65536.0f * (0.083333336f * index));
        if (v.z < 0.0f)
            v.z = 0.1f;
        else
            v.z = 3.3f;
        break;
    }
    *out = v;
}

/* Stage mode: advances the wave and switches modes on the round-time schedule. */
void fn_12_1CB78(struct FightPanelStage *stage)
{
    int mode;

    switch (stage->mode)
    {
    case 4:
        stage->wavePhase += (int)(256.0f * (0.0011111111f * stage->modeTimer)) + 0x40;
        break;
    }
    stage->modeTimer++;
    if (!(lbl_12_bss_1ACA0.flags & 4))
    {
        float t = (float)mode_info.timerCurr / (float)mode_info.timerMax;
        struct FightPanelMode *sched = &lbl_12_data_9B20[stage->modeIndex];

        if (sched->time > t)
        {
            mode = sched->mode;
            if (stage->mode != mode)
            {
                switch (mode)
                {
                case 2:
                    {
                        int i;
                        struct FightPanel *panel = stage->panels;

                        for (i = 12; i > 0; i--, panel++)
                        {
                            if (i % 3 == 0)
                                panel->flags |= 1;
                        }
                    }
                    mode = 0;
                    break;
                case 4:
                    stage->wavePhase = rand() & 0x7FFF;
                    break;
                default:
                case 3:
                    stage->unk82 = 0;
                    stage->wavePhase = 0;
                    stage->unk86 = 0;
                    break;
                }
                stage->mode = (s16)mode;
                stage->modeTimer = 0;
            }
            stage->modeIndex++;
        }
    }
    else if (stage->mode != 1)
    {
        stage->mode = 1;
        stage->modeTimer = 0;
        stage->unk82 = 0;
        stage->wavePhase = 0;
        stage->unk86 = 0;
    }
}

/* Panel stage draw: redraws the panels with their hit colour added. */
void fn_12_1CDD4(void)
{
    int i;
    struct FightPanel *panel;
    struct GMAModel *model;
    Vec c;

    panel = ((struct FightPanelStage *)lbl_12_bss_1EE38.work)->panels;
    for (i = 12; i > 0; i--, panel++)
    {
        c = panel->color;
        if (c.x <= 0.0f && c.y <= 0.0f && c.z <= 0.0f)
            continue;
        mtxa_from_mtxb();
        mtxa_mult_right(itemgroups[panel->itemGroup].transform);
        model = fn_1_58280(stagedef->collHeaders[panel->itemGroup].modelRefs[0]);
        if (model != NULL && g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
        {
            if (c.x > 1.0f)
                c.x = 1.0f;
            else if (c.x < 0.0f)
                c.x = 0.0f;
            if (c.y > 1.0f)
                c.y = 1.0f;
            else if (c.y < 0.0f)
                c.y = 0.0f;
            if (c.z > 1.0f)
                c.z = 1.0f;
            else if (c.z < 0.0f)
                c.z = 0.0f;
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            g_stores_doubles2(c.x, c.y, c.z, 0.0f);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    fade_color_base_default();
    g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
}

/* Intro camera init: starts above the ring at a random heading. */
void fn_12_1D074(struct Camera *camera, struct Ball *ball)
{
    struct Sphere s;

    camera_clear(camera);
    camera->flags |= 8;
    s = lbl_12_bss_1EE38.flyIn;
    camera->unk5C.x = s.pos.x;
    camera->unk5C.y = s.pos.y + 3.0f * s.radius;
    camera->unk5C.z = s.pos.z;
    camera->unk68 = s.radius * (0.5f + 0.5f * ((float)rand() / 32767.0f));
    camera->unk6C = s.radius * (-2.0f + -2.0f * ((float)rand() / 32767.0f));
    camera->unk7C = s.pos;
    camera->unk88 = 0.8f * s.radius;
    camera->unk8C = 0.7f * s.radius;
    camera->lookAt = camera->unk5C;
    camera->rotY = rand() & 0x7FFF;
    camera->flags |= 1;
    camera->unk26 = 1;
    camera->unk1F = 7;
}

/* Resets the ball mode of every ball in play (and the player's lbl_12_bss_1EE70 entry). */
void fn_12_1D21C(void)
{
    struct Ball *ball;
    struct Ball *oldBall;
    int i;

    oldBall = current_ball;
    ball = balls;
    for (i = 0; i < 8; i++, ball++)
    {
        if (ball->status == 2)
        {
            current_ball = ball;
            ball->unk154 = -1;
            lbl_12_bss_1EE70[ball->playerId].state = 0;
        }
    }
    current_ball = oldBall;
}

/* Creates the gloves' apes. */
void fn_12_1D2A0(void)
{
    int lod;
    struct Ball *ball;
    int i;
    int count;
    s8 *status;

    if (modeCtrl.playerCount > 2)
        lod = 2;
    else
        lod = 1;
    ball = balls;
    status = g_poolInfo.playerPool.statusList;
    count = modeCtrl.playerCount;
    for (i = 0; i < count; i++, ball++, status++)
    {
        if (*status != 0)
        {
            g_create_stage_player_wrapper(i, 24, active_monkey_id[i], i, lod, fn_12_1F110);
            ball->unk154 = 36;
        }
    }
}

void fn_12_1D378(void)
{
}

/* Fighter ball tick: runs the ball mode, applies a pending mode change, then the timers. */
void fn_12_1D37C(struct Ball *ball)
{
    struct FightPlayer *player = &lbl_12_bss_1ACA0.players[ball->playerId];

    if (player->flags & 1)
        fn_12_1F5A8(ball);
    if (ball->unk154 >= 0)
        lbl_12_data_9C00[ball->unk154](ball);
    if (ball->state != 24)
    {
        ball->unk154 = ball->state;
        ball->state = 24;
    }
    fn_12_1D440(ball);
    fn_12_1D534(ball);
}

/* Counts down the player's timers. */
void fn_12_1D440(struct Ball *ball)
{
    int i;
    struct FightPlayer *player;
    s16 *timer;

    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    if (ball->flags & 1)
    {
        player = &lbl_12_bss_1ACA0.players[ball->playerId];
        timer = player->hitBy;
        for (i = 4; i > 0; i--, timer++)
        {
            if (*timer > 0)
                (*timer)--;
        }
    }
    if (player->stunTimer > 0)
    {
        ball->flags &= ~1;
        player->stunTimer--;
    }
    if (player->unk14 > 0)
        player->unk14--;
}
