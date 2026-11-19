/* Monkey Fight 2: module entry, and the punching glove stage object (stobj type 7):
 * init, tick (punch state machine), draw, collision with balls, spring arm and effects
 * (.text 0x0-0x5AF8). Original file name (OSPanic): mini_fight2.c */
#include "fight.h"

/* ---- main_loop ---- */
u32 *OSGetStackPointer(void);
void g_set_minigame_specific_funcs(void (*tick)(void), void (*draw)(void), void *menu, void *camera);
void g_very_similar_to_puts(const char *);
void fn_1_A8034(int type, struct StobjFuncs *funcs);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
void g_stores_doubles2(float r, float g, float b, float a);
void SoundPan(u16 soundId, s8 pan, s8 span);
void call_SoundReqID_arg_0(int soundId);
int call_SoundReqID_arg_1_copy(u32 soundId);
s8 fn_1_2F5B0(int cameraId, Vec *pos, s8 *pan, s8 *span);
void g_rumble_controller(int pad, int cmd, int dur);
void g_something_with_item_coli(Vec *a, Vec *b, float d, Vec *c, float e);
int fn_1_A74E4(Vec *a, Vec *b, float ra, Vec *c, Vec *d, float rb);
extern struct GMA *minigameGma;
extern u32 g_some_render_flag;
extern s32 active_monkey_id[];
extern u32 g_smth_with_sound;
extern u32 g_player_id_for_sound;

/* ---- .bss 0x0-0x1ACA0 ---- */

struct GMAModel *lbl_12_bss_0[35];          /* 0x00000 models listed by lbl_12_data_304 */
struct GMAModel *lbl_12_bss_8C[35];         /* 0x0008C models listed by lbl_12_data_2E0 */
struct FightBox lbl_12_bss_118[144];  /* 0x00118 per stage object (stobj pool index) */
struct GMAModel *lbl_12_bss_1AC98;          /* 0x1AC98 */

/* ---- .data 0x0-0x328 ---- */

/* unreferenced */
static u8 lbl_12_data_0[0x170] = {
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

/* glove (type 7) stage object callbacks and helpers */
void fn_12_1C0(struct Stobj *stobj);
void fn_12_4C8(struct Stobj *stobj);
void fn_12_235C(struct Stobj *stobj);
void fn_12_2538(struct Stobj *stobj, struct PhysicsBall *physBall);
void fn_12_3894(struct Stobj *stobj);
void fn_12_38FC(struct Stobj *stobj);
void fn_12_3B8C(struct FightGlove *glove, Vec *from, Vec *to);
void fn_12_3C78(struct FightGlove *glove, Vec *root, Vec *rootDir, Vec *tip, Vec *tipDir);
void fn_12_43E4(struct FightGlove *glove);
float fn_12_457C(struct Stobj *stobj);
float fn_12_4658(struct Stobj *stobj);
float fn_12_4710(struct Stobj *stobj);
float fn_12_474C(struct Stobj *stobj);
void fn_12_47C0(struct Stobj *stobj);
void fn_12_4AE4(struct Ball *ball, Vec *dir);
void fn_12_4EF8(struct FightGlove *glove, int level);
void fn_12_5050(struct FightGlove *glove, float speed);
void fn_12_56B8(struct FightGlove *glove);
void fn_12_5A10(void);

void _prolog(void)
{
    g_set_minigame_specific_funcs(fn_12_11A28, fn_12_11BC4, (void *)fn_12_1D37C, (void *)fn_12_20598);
    g_very_similar_to_puts("RELOCATION MINI_FIGHT2 LOADED");
    fn_12_11300();
    sub_mode_request = 0xBE;
}

void _epilog(void)
{
    fn_12_11910();
    g_set_minigame_specific_funcs(NULL, NULL, NULL, NULL);
    g_very_similar_to_puts("RELOCATION MINI_FIGHT2 PURGED");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    for (sp = OSGetStackPointer(); sp != NULL && (uptr)sp != 0xFFFFFFFF && i++ < 16; sp = (u32 *)(uptr)*sp)
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
    OSPanic("mini_fight2.c", 0x50, "\n");
}

/* punch hit sound per character (active_monkey_id) */
s16 lbl_12_data_230[6] = { 0x0102, 0x0103, 0x0100, 0x0101, 0x00FF, 0 };
/* sounds of the punched ball's ape */
static s32 lbl_12_data_23C[3] = { 0x17, 0x18, 0x1A };
static struct StobjFuncs lbl_12_data_248 = { fn_12_1C0, fn_12_4C8, fn_12_235C, fn_12_2538, fn_12_3894, NULL };
static struct StobjFuncs lbl_12_data_260 = { fn_12_6FB0, fn_12_70B8, fn_12_83F8, fn_12_8970, fn_12_8EC0, NULL };
/* glove model per ball colour (minigameGma) */
static u16 lbl_12_data_278[10] = { 0x8F, 0x8D, 0x90, 0x8E, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F };
/* power-up glove model per ball colour (minigameGma) */
static u16 lbl_12_data_28C[10] = { 0xAC, 0xAA, 0xAD, 0xAB, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC };

/* Registers the glove (type 7) and type 8 stage object callbacks and looks up the models. */
void fn_12_14C(void)
{
    fn_1_A8034(7, &lbl_12_data_248);
    fn_1_A8034(8, &lbl_12_data_260);
    fn_12_5A10();
}

/* Unregisters the type 7 and 8 stage object callbacks. */
void fn_12_18C(void)
{
    fn_1_A8034(7, NULL);
    fn_1_A8034(8, NULL);
}

/* Glove stobj init: model, scale, arm and glove parameters from the player's level. */
void fn_12_1C0(struct Stobj *stobj)
{
    struct Ball *ball;
    struct FightGlove *glove;
    struct FightPlayer *player;
    struct FightCharaParam *param;
    float scale;
    float interval;
    Vec offset;

    glove = stobj->extraData;
    ball = &balls[glove->playerIdx];
    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    stobj->state = 0;
    stobj->unk8 |= 0x12;
    stobj->model = minigameGma->modelEntries[lbl_12_data_278[ball->colorId]].model;
    stobj->boundSphereRadius = lbl_1_bss_6FABC;
    scale = lbl_1_bss_6FABC / stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->u_model_origin.x *= scale;
    stobj->u_model_origin.y *= scale;
    stobj->u_model_origin.z *= scale;
    stobj->unk48 = scale;
    stobj->unk4C = scale;
    stobj->unk50 = scale;
    stobj->u_local_pos.x = 0.0f;
    stobj->u_local_pos.y = 0.0f;
    stobj->u_local_pos.z = 1.0f;
    vec_set_len(ball->currRadius, &stobj->u_local_pos, &offset);
    stobj->u_some_pos.x = ball->pos.x + offset.x;
    stobj->u_some_pos.y = ball->pos.y + offset.y;
    stobj->u_some_pos.z = ball->pos.z + offset.z;
    glove->unkC = 0;
    glove->unk32 = 0;
    glove->unkA = 0;
    glove->unk30 = 0;
    glove->sound3C = -1;
    glove->unk44 = 0;
    glove->stobj = stobj;
    glove->armModel = minigameGma->modelEntries[0xA6].model;
    glove->unk54 = minigameGma->modelEntries[lbl_12_data_28C[ball->colorId]].model;
    stobj->unk8 |= 8;
    stobj->unk9C = 1.0f;
    stobj->unk90 = stobj->u_model_origin;
    param = &lbl_12_data_9CC4[player->chara];
    glove->unk10 = param->unk10;
    glove->unk14 = 1.0f - (1.0f - lbl_1_bss_6FAAC) * param->unk4;
    glove->unk18 = param->unk0;
    interval = lbl_1_bss_6FADC;
    glove->unk1C = interval * param->unkC;
    glove->unk20 = param->unk8;
    fn_12_3B8C(glove, &ball->pos, &stobj->u_some_pos);
}

/* Glove stobj tick: reads the punch/charge input (pad or CPU), runs the punch state machine
 * (stobj->state: 0-3 idle/aim/charge, 4-5 punch, 6-8 hold and spin, 9-13 charged punch),
 * moves the glove towards its target with a spring and updates the arm. */
void fn_12_4C8(struct Stobj *stobj)
{
    struct Ball *ball;
    int punch;
    float keep;
    int i;
    float scale;
    float range;
    int newPunch;
    float reach;
    float spring;
    int charge;
    struct Ball *savedBall;
    int newCharge;
    float push;
    int level;
    int swing;
    struct FightGlove *glove;
    int punchTime;
    s16 *cooldown;
    struct ControllerInfo *pad;
    float keepCharge;
    Vec target;
    Vec input;
    Vec delta;
    s8 pan;
    s8 span;

    glove = stobj->extraData;
    ball = &balls[glove->playerIdx];
    current_ball = ball;
    if (!(glove->flags & 0x40) && lbl_12_bss_1ACA0.players[ball->playerId].stunTimer <= 0)
    {

        glove->unk58 += 0.1f * ((float)glove->rangeLevel - glove->unk58);
        keep = glove->unk14;
        punchTime = glove->unk1C;
        keepCharge = 1.0 - 0.5 * (1.0 - keep);
        stobj->boundSphereRadius += 0.2f * (fn_12_457C(stobj) - stobj->boundSphereRadius);
        scale = stobj->boundSphereRadius / stobj->model->boundSphereRadius;
        stobj->u_model_origin = stobj->model->boundSphereCenter;
        stobj->u_model_origin.x *= scale;
        stobj->u_model_origin.y *= scale;
        stobj->u_model_origin.z *= scale;
        stobj->unk48 = scale;
        stobj->unk4C = scale;
        stobj->unk50 = scale;
        range = fn_12_4658(stobj);
        spring = 0.1f - 0.06f * (1.0f - lbl_1_bss_6FAB4 / range);
        reach = fn_12_4710(stobj);
        if (glove->unk2C > 0)
        {
            glove->unk2C--;
            if (glove->unk2C == 0)
                glove->flags &= ~4;
        }
        if (lbl_12_bss_1ACA0.players[ball->playerId].flags & 1)
            pad = &lbl_12_bss_1F9D8[ball->playerId].pad;
        else
            pad = NULL;

        if ((ball->flags & 0x2000000810LL) || (lbl_12_bss_1ACA0.flags & 0x40))
        {
            struct ControllerInfo *p;

            punch = 0;
            charge = 0;
            p = (pad != NULL) ? pad : &pad_status_groups[player_pad_map[ball->playerId]];
            input.x = p->unk0[0].substickX;
            input.y = p->unk0[0].substickY;
            input.z = 0.0f;
            if (mathutil_vec_len(&input) <= 30.0f)
            {
                if (pad == NULL)
                {
                    p = &pad_status_groups[player_pad_map[ball->playerId]];
                    input.x = p->unk0[0].stickX;
                    input.y = p->unk0[0].stickY;
                    input.z = 0.0f;
                    mtxa_from_mtxb();
                    mtxa_rigid_inv_tf_vec(&input, &input);
                    input.y = 0.0f;
                }
                else
                {
                    p = pad;
                    input.x = p->unk0[0].stickX;
                    input.y = 0.0f;
                    input.z = -(float)p->unk0[0].stickY;
                }
            }
        }
        else
        {
            struct ControllerInfo *p;

            punch = 0;
            swing = lbl_1_bss_6FAD4 % 2;
            p = (pad != NULL) ? pad : &pad_status_groups[player_pad_map[ball->playerId]];
            charge = (p->unk0[0].button & PAD_BUTTON_B) || p->unk0[0].triggerRight > 20;
            input.x = p->unk0[0].substickX;
            input.y = p->unk0[0].substickY;
            input.z = 0.0f;
            if (mathutil_vec_len(&input) > 30.0f)
                punch = 1;
            if (punch)
            {
                mtxa_from_mtxb();
                mtxa_rigid_inv_tf_vec(&input, &input);
                input.y = 0.0f;
            }
            else if (pad == NULL)
            {
                p = &pad_status_groups[player_pad_map[ball->playerId]];
                punch = p->unk0[0].button & PAD_BUTTON_A;
                input.x = p->unk0[0].stickX;
                input.y = p->unk0[0].stickY;
                input.z = 0.0f;
                mtxa_from_mtxb();
                mtxa_rigid_inv_tf_vec(&input, &input);
                input.y = 0.0f;
            }
            else
            {
                p = pad;
                punch = p->unk0[0].button & PAD_BUTTON_A;
                input.x = p->unk0[0].stickX;
                input.y = 0.0f;
                input.z = -(float)p->unk0[0].stickY;
            }
        }
        newPunch = punch && glove->prevPunch == 0;
        newCharge = charge && glove->prevCharge == 0;
        glove->prevPunch = punch;
        glove->prevCharge = charge;
        if (0.0f == input.x && 0.0f == input.z)
            input = stobj->u_local_pos;
        else
            vec_normalize_len(&input);

        switch (stobj->state)
        {
        case 0:
            stobj->state = 1;
            stobj->counter = punchTime >> 1;
            /* fall through */
        case 1:
            stobj->u_local_pos.x += 0.1f * (input.x - stobj->u_local_pos.x);
            stobj->u_local_pos.y += 0.1f * (input.y - stobj->u_local_pos.y);
            stobj->u_local_pos.z += 0.1f * (input.z - stobj->u_local_pos.z);
            vec_normalize_len(&stobj->u_local_pos);
            scale = ball->currRadius + 0.75f * stobj->boundSphereRadius;
            target.x = ball->pos.x + stobj->u_local_pos.x * (scale + reach);
            target.y = ball->pos.y + stobj->u_local_pos.y * (scale + reach);
            target.z = ball->pos.z + stobj->u_local_pos.z * (scale + reach);
            stobj->counter--;
            if (stobj->counter < 0)
                stobj->state = 2;
            break;
        case 2:
            stobj->state = 3;
            glove->chargeTimer = 0;
            glove->chargeLevel = 0;
            glove->flags &= ~0x11;
            /* fall through */
        case 3:
            if (!(glove->flags & 0x10) && newCharge)
            {
                glove->flags |= 0x10;
                glove->chargeTimer = 0;
                glove->chargeLevel = 0;
                fn_12_4EF8(glove, 0);
                glove->sound40 = call_SoundReqID_arg_1_copy(0x35B);
            }
            if (glove->flags & 0x10)
            {
                glove->chargeTimer++;
                if (glove->chargeLevel < 4)
                {
                    level = glove->chargeTimer / 24;
                    if (glove->chargeLevel != level)
                    {
                        fn_12_4EF8(glove, level);
                        glove->chargeLevel = level;
                    }
                }
                if ((glove->flags & 1) || glove->chargeTimer > 156 || (!charge && glove->chargeTimer < 24))
                {
                    glove->flags &= ~0x11;
                    glove->chargeTimer = 0;
                    glove->chargeLevel = 0;
                    if (glove->sound40 > 0)
                    {
                        SoundOff(glove->sound40);
                        glove->sound40 = -1;
                    }
                }
                if (ball->flags & 1)
                {
                    ball->vel.x *= keepCharge;
                    ball->vel.z *= keepCharge;
                }
            }
            stobj->u_local_pos.x += 0.2f * (input.x - stobj->u_local_pos.x);
            stobj->u_local_pos.y += 0.2f * (input.y - stobj->u_local_pos.y);
            stobj->u_local_pos.z += 0.2f * (input.z - stobj->u_local_pos.z);
            vec_normalize_len(&stobj->u_local_pos);
            scale = ball->currRadius + 0.75f * stobj->boundSphereRadius;
            target.x = ball->pos.x + stobj->u_local_pos.x * (scale + reach);
            target.y = ball->pos.y + stobj->u_local_pos.y * (scale + reach);
            target.z = ball->pos.z + stobj->u_local_pos.z * (scale + reach);
            if ((!charge || newPunch) && glove->chargeTimer >= 24)
                stobj->state = 9;
            else if (newPunch)
            {
                stobj->u_local_pos = input;
                fn_12_4AE4(ball, &stobj->u_local_pos);
                if (!(glove->flags & 6))
                    stobj->state = 4;
                else
                    stobj->state = 6;
            }
            glove->flags &= ~1;
            break;
        case 4:
            stobj->state = 5;
            stobj->unk8 |= 2;
            stobj->counter = punchTime;
            memset(glove->hitCooldown, 0, sizeof(glove->hitCooldown));
            glove->flags |= 8;
            glove->flags &= ~1;
            target.x = ball->pos.x + stobj->u_local_pos.x * scale;
            target.y = ball->pos.y + stobj->u_local_pos.y * scale;
            target.z = ball->pos.z + stobj->u_local_pos.z * scale;
            delta.x = target.x - stobj->u_some_pos.x;
            delta.y = target.y - stobj->u_some_pos.y;
            delta.z = target.z - stobj->u_some_pos.z;
            vec_set_len(lbl_1_bss_6FACC, &delta, &delta);
            stobj->unk64.x += delta.x;
            stobj->unk64.y += delta.y;
            stobj->unk64.z += delta.z;
            g_smth_with_sound = active_monkey_id[glove->playerIdx];
            g_player_id_for_sound = glove->playerIdx;
            if (glove->rangeLevel > 0)
                glove->sound3C = call_SoundReqID_arg_1_copy(0x104);
            else
            {
                call_SoundReqID_arg_0(0x106);
                glove->sound3C = -1;
            }
            /* fall through */
        case 5:
            if (ball->flags & 1)
            {
                ball->vel.x *= keep;
                ball->vel.z *= keep;
            }
            if (swing == 1)
            {
                stobj->u_local_pos.x += 0.2f * (input.x - stobj->u_local_pos.x);
                stobj->u_local_pos.y += 0.2f * (input.y - stobj->u_local_pos.y);
                stobj->u_local_pos.z += 0.2f * (input.z - stobj->u_local_pos.z);
                vec_normalize_len(&stobj->u_local_pos);
            }
            target.x = ball->pos.x + stobj->u_local_pos.x * range;
            target.y = ball->pos.y + stobj->u_local_pos.y * range;
            target.z = ball->pos.z + stobj->u_local_pos.z * range;
            if (glove->flags & 4)
                stobj->state = 6;
            if (stobj->counter >= 0)
                stobj->counter--;
            if ((!punch && stobj->counter < 0) || (glove->flags & 1))
            {
                stobj->state = 0;
                glove->flags &= ~9;
            }
            break;
        case 6:
            stobj->state = 7;
            stobj->counter = 3;
            stobj->unk8 |= 2;
            memset(glove->hitCooldown, 0, sizeof(glove->hitCooldown));
            glove->flags &= ~1;
            glove->flags |= 8;
            target.x = ball->pos.x + stobj->u_local_pos.x * range;
            target.y = ball->pos.y + stobj->u_local_pos.y * range;
            target.z = ball->pos.z + stobj->u_local_pos.z * range;
            delta.x = target.x - stobj->u_some_pos.x;
            delta.y = target.y - stobj->u_some_pos.y;
            delta.z = target.z - stobj->u_some_pos.z;
            vec_set_len(lbl_1_bss_6FACC, &delta, &delta);
            stobj->unk64.x += delta.x;
            stobj->unk64.y += delta.y;
            stobj->unk64.z += delta.z;
            g_smth_with_sound = active_monkey_id[glove->playerIdx];
            g_player_id_for_sound = glove->playerIdx;
            if (glove->rangeLevel > 0)
                glove->sound3C = call_SoundReqID_arg_1_copy(0x104);
            else
            {
                call_SoundReqID_arg_0(0x106);
                glove->sound3C = -1;
            }
            /* fall through */
        case 7:
            if (ball->flags & 1)
            {
                ball->vel.x *= keep;
                ball->vel.z *= keep;
            }
            target.x = ball->pos.x + stobj->u_local_pos.x * range;
            target.y = ball->pos.y + stobj->u_local_pos.y * range;
            target.z = ball->pos.z + stobj->u_local_pos.z * range;
            stobj->counter--;
            if (stobj->counter < 0)
            {
                stobj->state = 8;
                if (glove->sound38 >= 0)
                    SoundOff(glove->sound38);
                glove->sound38 = call_SoundReqID_arg_1_copy(0x107);
            }
            if (!punch || (glove->flags & 1) || !(glove->flags & 4))
            {
                stobj->state = 0;
                glove->flags &= ~9;
                if (glove->sound38 >= 0)
                {
                    SoundOff(glove->sound38);
                    glove->sound38 = -1;
                }
            }
            break;
        case 8:
            if (ball->flags & 1)
            {
                ball->vel.x *= keep;
                ball->vel.z *= keep;
            }
            spring = 0.1f;
            mtxa_from_rotate_y(0xE38);
            mtxa_tf_vec(&stobj->u_local_pos, &stobj->u_local_pos);
            vec_normalize_len(&stobj->u_local_pos);
            target.x = ball->pos.x + 0.6f * (stobj->u_local_pos.x * range);
            target.y = ball->pos.y + 0.6f * (stobj->u_local_pos.y * range);
            target.z = ball->pos.z + 0.6f * (stobj->u_local_pos.z * range);
            if ((glove->sound38 != -1 || glove->sound40 != -1) && g_font_char_var_3 % 5 == 0)
            {
                fn_1_2F5B0(0, &target, &pan, &span);
                if (glove->sound38 != -1)
                    SoundPan(glove->sound38, pan, span);
                if (glove->sound40 != -1)
                    SoundPan(glove->sound40, pan, span);
            }
            if (!punch || (glove->flags & 1) || !(glove->flags & 4))
            {
                stobj->state = 0;
                glove->flags &= ~9;
                if (glove->sound38 >= 0)
                {
                    SoundOff(glove->sound38);
                    glove->sound38 = -1;
                }
            }
            break;
        case 9:
            stobj->state = 10;
            stobj->counter = 60.0f * (0.15f + 0.025f * glove->chargeLevel);
            glove->flags &= ~0x10;
            glove->flags |= 0x20;
            stobj->u_local_pos = input;
            fn_12_4AE4(ball, &stobj->u_local_pos);
            /* fall through */
        case 10:
            if (ball->flags & 1)
            {
                ball->vel.x *= keep;
                ball->vel.z *= keep;
            }
            if (stobj->counter >= 6.0f)
            {
                stobj->u_local_pos.x += 0.2f * (input.x - stobj->u_local_pos.x);
                stobj->u_local_pos.y += 0.2f * (input.y - stobj->u_local_pos.y);
                stobj->u_local_pos.z += 0.2f * (input.z - stobj->u_local_pos.z);
                vec_normalize_len(&stobj->u_local_pos);
                if (6.0f == stobj->counter)
                    fn_12_4AE4(ball, &stobj->u_local_pos);
            }
            scale = ball->currRadius + 0.75f * stobj->boundSphereRadius;
            target.x = ball->pos.x + stobj->u_local_pos.x * (scale + reach);
            target.y = ball->pos.y + stobj->u_local_pos.y * (scale + reach);
            target.z = ball->pos.z + stobj->u_local_pos.z * (scale + reach);
            stobj->counter--;
            if (stobj->counter < 0)
            {
                if (!(glove->flags & 6))
                    stobj->state = 11;
                else
                    stobj->state = 13;
            }
            break;
        case 11:
            stobj->state = 12;
            stobj->unk8 |= 2;
            stobj->counter = 15;
            memset(glove->hitCooldown, 0, sizeof(glove->hitCooldown));
            glove->flags |= 8;
            glove->flags &= ~0x21;
            if (glove->sound40 > 0)
            {
                SoundOff(glove->sound40);
                glove->sound40 = -1;
            }
            lbl_12_bss_1ACA0.players[ball->playerId].flags |= 0x20;
            target.x = ball->pos.x + stobj->u_local_pos.x * range;
            target.y = ball->pos.y + stobj->u_local_pos.y * range;
            target.z = ball->pos.z + stobj->u_local_pos.z * range;
            delta.x = target.x - stobj->u_some_pos.x;
            delta.y = target.y - stobj->u_some_pos.y;
            delta.z = target.z - stobj->u_some_pos.z;
            vec_set_len(3.0f * lbl_1_bss_6FACC, &delta, &delta);
            stobj->unk64.x += delta.x;
            stobj->unk64.y += 1.1f * delta.y;
            stobj->unk64.z += delta.z;
            push = -0.1f * glove->chargeLevel;
            ball->vel.x += delta.x * push;
            ball->vel.y += delta.y * push;
            ball->vel.z += delta.z * push;
            g_smth_with_sound = active_monkey_id[glove->playerIdx];
            g_player_id_for_sound = glove->playerIdx;
            if (glove->rangeLevel > 0)
                glove->sound3C = call_SoundReqID_arg_1_copy(0x104);
            else
            {
                call_SoundReqID_arg_0(0x106);
                glove->sound3C = -1;
            }
            /* fall through */
        case 12:
            if (ball->flags & 1)
            {
                ball->vel.x *= keep;
                ball->vel.z *= keep;
            }
            if (swing == 1)
            {
                stobj->u_local_pos.x += 0.2f * (input.x - stobj->u_local_pos.x);
                stobj->u_local_pos.y += 0.2f * (input.y - stobj->u_local_pos.y);
                stobj->u_local_pos.z += 0.2f * (input.z - stobj->u_local_pos.z);
                vec_normalize_len(&stobj->u_local_pos);
            }
            target.x = ball->pos.x + stobj->u_local_pos.x * range;
            target.y = ball->pos.y + stobj->u_local_pos.y * range;
            target.z = ball->pos.z + stobj->u_local_pos.z * range;
            stobj->counter--;
            if (stobj->counter < 0 || (glove->flags & 1))
            {
                stobj->state = 0;
                glove->flags &= ~9;
            }
            break;
        case 13:
            stobj->state = 7;
            stobj->counter = 3;
            stobj->unk8 |= 2;
            memset(glove->hitCooldown, 0, sizeof(glove->hitCooldown));
            glove->flags |= 8;
            glove->flags &= ~0x21;
            lbl_12_bss_1ACA0.players[ball->playerId].flags |= 0x20;
            target.x = ball->pos.x + stobj->u_local_pos.x * range;
            target.y = ball->pos.y + stobj->u_local_pos.y * range;
            target.z = ball->pos.z + stobj->u_local_pos.z * range;
            delta.x = target.x - stobj->u_some_pos.x;
            delta.y = target.y - stobj->u_some_pos.y;
            delta.z = target.z - stobj->u_some_pos.z;
            vec_set_len(3.0f * lbl_1_bss_6FACC, &delta, &delta);
            stobj->unk64.x += delta.x;
            stobj->unk64.y += 1.1f * delta.y;
            stobj->unk64.z += delta.z;
            push = -0.1f * glove->chargeLevel;
            ball->vel.x += delta.x * push;
            ball->vel.y += delta.y * push;
            ball->vel.z += delta.z * push;
            g_smth_with_sound = active_monkey_id[glove->playerIdx];
            g_player_id_for_sound = glove->playerIdx;
            if (glove->rangeLevel > 0)
                glove->sound3C = call_SoundReqID_arg_1_copy(0x104);
            else
            {
                call_SoundReqID_arg_0(0x106);
                glove->sound3C = -1;
            }
            break;
        }

        if (glove->flags & 2)
        {
            glove->flags &= ~2;
            stobj->unk64.x = 0.0f;
            stobj->unk64.y = 0.0f;
            stobj->unk64.z = 0.0f;
            stobj->u_some_pos = target;
            stobj->unk7C = stobj->u_some_pos;
            fn_12_3B8C(glove, &ball->pos, &stobj->u_some_pos);
        }
        else
        {
            stobj->unk64.x *= 0.85f;
            stobj->unk64.y *= 0.85f;
            stobj->unk64.z *= 0.85f;
            stobj->unk64.x += spring * (target.x - stobj->u_some_pos.x);
            stobj->unk64.y += spring * ((target.y - 0.05f) - stobj->u_some_pos.y);
            stobj->unk64.z += spring * (target.z - stobj->u_some_pos.z);
            stobj->u_some_pos.x += stobj->unk64.x;
            stobj->u_some_pos.y += stobj->unk64.y;
            stobj->u_some_pos.z += stobj->unk64.z;
        }
        stobj->unk76 = stobj->rotX;
        stobj->unk78 = stobj->rotY;
        delta.x = stobj->u_some_pos.x - ball->pos.x;
        delta.y = stobj->u_some_pos.y - ball->pos.y;
        delta.z = stobj->u_some_pos.z - ball->pos.z;
        vec_to_euler_xy(&delta, &stobj->rotX, &stobj->rotY);
        stobj->unk76 = stobj->rotX - stobj->unk76;
        stobj->unk78 = stobj->rotY - stobj->unk78;
        for (cooldown = glove->hitCooldown, i = 4; i > 0; i--, cooldown++)
        {
            if (*cooldown > 0)
                (*cooldown)--;
        }
        if (glove->unkA != 0)
            glove->unkA--;
        if (glove->unk2A > 0)
            glove->unk2A--;
        if (glove->unk44 > 0)
        {
            glove->unk44--;
            if (glove->unk44 == 0 && glove->sound3C >= 0)
            {
                savedBall = current_ball;
                SoundOff(glove->sound3C);
                glove->sound3C = -1;
                current_ball = savedBall;
            }
        }
        fn_12_5AF8(stobj);
        if (cameras[0].unk1F == 3 || cameras[0].unk1F == 9)
            fn_12_47C0(stobj);
        if (!(ball->flags & 0x800))
            fn_12_38FC(stobj);
        if (glove->unk30 != 0)
        {
            glove->unk30--;
            if (glove->unk30 == 0)
            {
                g_smth_with_sound = active_monkey_id[glove->playerIdx];
                g_player_id_for_sound = glove->playerIdx;
                call_SoundReqID_arg_0(0x108);
            }
        }
        delta.x = ball->pos.x + stobj->u_local_pos.x * ball->currRadius;
        delta.y = (ball->pos.y + stobj->u_local_pos.y * ball->currRadius) - 0.05f;
        delta.z = ball->pos.z + stobj->u_local_pos.z * ball->currRadius;
        mtxa_from_translate(&stobj->u_some_pos);
        mtxa_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX);
        mtxa_tf_point_xyz(0.0f, 0.0f, 0.25f, &target);
        input.x = -mtxa->mtxA[0][2];
        input.y = -mtxa->mtxA[1][2];
        input.z = -mtxa->mtxA[2][2];
        fn_12_3C78(glove, &delta, &stobj->u_local_pos, &target, &input);
    }
}

/* Glove stobj draw: the arm, then the glove (power-up model when powered, white while
 * punching, blinking while charging). */
void fn_12_235C(struct Stobj *stobj)
{
    struct FightGlove *glove;
    struct Ball *ball;
    struct GMAModel *model;

    glove = stobj->extraData;
    ball = &balls[glove->playerIdx];
    if (!(glove->flags & 0x40) && !(ball->flags & 0x10))
    {
        fn_12_43E4(glove);
        if (!(g_some_render_flag & 4) || !(stobj->u_some_pos.y < -stobj->boundSphereRadius))
        {
            if (glove->powerLevel != 0)
                model = glove->unk54;
            else
                model = stobj->model;
            mtxa_from_mtxb_translate(&stobj->u_some_pos);
            mtxa_rotate_y(stobj->rotY);
            mtxa_rotate_x(stobj->rotX);
            mtxa_rotate_z(stobj->rotZ);
            mtxa_scale_s(stobj->unk48);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_set_scale_factor(stobj->unk48);
            if (glove->flags & 8)
            {
                g_stores_doubles2(0.5f, 0.5f, 0.5f, 0.0f);
                avdisp_draw_model_culled_sort_auto(model);
                g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
            }
            else if (glove->flags & 0x10)
            {
                if (g_font_char_var_3 & 1)
                {
                    set_post_mult_color(0.5f, 0.5f, 0.5f, 1.0f);
                    avdisp_draw_model_culled_sort_auto(model);
                    fade_color_base_default();
                }
                else
                    avdisp_draw_model_culled_sort_auto(model);
            }
            else
                avdisp_draw_model_culled_sort_auto(model);
        }
    }
}

/* Glove stobj collision with a ball (current_ball): pushes the glove out of its own ball and
 * of balls it is not punching; on a punch hit plays the hit sounds, spawns the hit effects,
 * rumbles, scores the hit, stuns the target and knocks its ball away. */
void fn_12_2538(struct Stobj *stobj, struct PhysicsBall *physBall)
{
    struct Effect effect;
    Vec pos;
    Vec dir;
    Vec v;
    Vec prevVel;
    Vec hitPos;
    GXColor color;
    float spread;
    int sound;
    struct Ball *ball;
    int j;
    struct FightGlove *glove;
    float len;
    float power;
    int stun;
    int i;
    int playerId;
    struct FightPlayer *other;
    float velY;
    float radius;
    float frac;
    struct FightPlayer *player;
    float t;
    float count;

    ball = current_ball;
    glove = stobj->extraData;
    playerId = ball->playerId;
    if (!(glove->flags & 0x40) && !(ball->flags & 0x800000000LL)
     && lbl_12_bss_1ACA0.players[glove->playerIdx].stunTimer <= 0)
    {
        if (glove->unk2A <= 0 && glove->playerIdx != playerId && (glove->flags & 8))
        {
            if (glove->powerLevel > 0)
                sound = 0x35C;
            else if (glove->radLevel > 0)
                sound = 0xFF;
            else
                sound = lbl_12_data_230[active_monkey_id[glove->playerIdx]];
            if (glove->rangeLevel > 0)
                sound |= 0xD800;
            g_smth_with_sound = active_monkey_id[glove->playerIdx];
            g_player_id_for_sound = glove->playerIdx;
            call_SoundReqID_arg_0(sound);
            ball->flags |= 0x20;
            g_smth_with_sound = ball->ape->charaId;
            g_player_id_for_sound = ball->playerId;
            call_SoundReqID_arg_0(lbl_12_data_23C[g_font_char_var_3 % 3]);
            glove->unk2A = 15;
        }
        if (glove->playerIdx == playerId || !(glove->flags & 8)
         || lbl_12_bss_1ACA0.players[playerId].unk6 > 0)
        {
            dir.x = stobj->position.x - physBall->pos.x;
            dir.y = stobj->position.y - physBall->pos.y;
            dir.z = stobj->position.z - physBall->pos.z;
            radius = physBall->radius + stobj->boundSphereRadius;
            if (mathutil_vec_len(&dir) < radius)
            {
                v = dir;
                vec_set_len(radius, &v, &v);
                pos.x = physBall->pos.x + v.x;
                pos.y = physBall->pos.y + v.y;
                pos.z = physBall->pos.z + v.z;
                stobj->u_some_pos.x += pos.x - stobj->position.x;
                stobj->u_some_pos.y += pos.y - stobj->position.y;
                stobj->u_some_pos.z += pos.z - stobj->position.z;
                stobj->position.x = pos.x;
                stobj->position.y = pos.y;
                stobj->position.z = pos.z;
                vec_normalize_len(&dir);
                power = mathutil_vec_dot_prod(&dir, &stobj->unk64);
                if (power < 0.0f)
                {
                    power *= -1.2f;
                    stobj->unk64.x += dir.x * power;
                    stobj->unk64.y += dir.y * power;
                    stobj->unk64.z += dir.z * power;
                }
            }
            return;
        }
        if (glove->hitCooldown[playerId] > 0)
        {
            dir.x = stobj->position.x - physBall->pos.x;
            dir.y = stobj->position.y - physBall->pos.y;
            dir.z = stobj->position.z - physBall->pos.z;
            radius = physBall->radius + stobj->boundSphereRadius;
            if (mathutil_vec_len(&dir) < radius)
            {
                v = dir;
                vec_set_len(radius, &v, &v);
                pos.x = physBall->pos.x + v.x;
                pos.y = physBall->pos.y + v.y;
                pos.z = physBall->pos.z + v.z;
                stobj->u_some_pos.x += pos.x - stobj->position.x;
                stobj->u_some_pos.y += pos.y - stobj->position.y;
                stobj->u_some_pos.z += pos.z - stobj->position.z;
                stobj->position.x = pos.x;
                stobj->position.y = pos.y;
                stobj->position.z = pos.z;
            }
            return;
        }

        if (glove->sound3C >= 0)
            glove->unk44 = 15;
        color = lbl_12_data_A218[balls[glove->playerIdx].colorId];
        memset(&effect, 0, sizeof(effect));
        effect.playerId = glove->playerIdx;
        effect.type = 2;
        frac = stobj->boundSphereRadius / (stobj->boundSphereRadius + ball->currRadius);
        hitPos.x = stobj->position.x * (1.0f - frac) + ball->pos.x * frac;
        hitPos.y = stobj->position.y * (1.0f - frac) + ball->pos.y * frac;
        hitPos.z = stobj->position.z * (1.0f - frac) + ball->pos.z * frac;
        effect.pos = hitPos;
        effect.unk18 = 0.002610966f * (128.0f + color.r);
        effect.unk1C = 0.002610966f * (128.0f + color.g);
        effect.unk20 = 0.002610966f * (128.0f + color.b);
        dir.x = ball->pos.x - stobj->position.x;
        dir.y = ball->pos.y - stobj->position.y;
        dir.z = ball->pos.z - stobj->position.z;
        vec_normalize_len(&dir);
        vec_to_euler_xy(&dir, &effect.rotX, &effect.rotY);
        mtxa_from_rotate_y(effect.rotY);
        mtxa_rotate_x(effect.rotX);
        power = 0.5f * mathutil_vec_dot_prod(&dir, &stobj->unk64);
        v.x = power * dir.x;
        v.y = power * dir.y;
        v.z = power * dir.z;
        for (i = 8; i > 0; i--)
        {
            mtxa_rotate_z(0x2000);
            mtxa_tf_vec_xyz(0.0f, power * (1.0f + 0.25f * (rand() / 32767.0f)), 0.0f, &effect.vel);
            effect.vel.x += v.x;
            effect.vel.y += v.y;
            effect.vel.z += v.z;
            spawn_effect(&effect);
        }
        effect.type = 0x2F;
        effect.playerId = ball->playerId;
        vec_to_euler_xy(&ball->vel, &effect.rotX, &effect.rotY);
        mtxa_from_rotate_y(effect.rotY);
        mtxa_rotate_x(effect.rotX);
        count = fabs(75.0f * power);
        for (i = count; i > 0; i--)
        {
            mtxa_rotate_z(rand() & 0x7FFF);
            mtxa_tf_vec_xyz(0.0f, 1.2f * ball->currRadius, 0.0f, &effect.pos);
            effect.pos.x += hitPos.x;
            effect.pos.y += hitPos.y;
            effect.pos.z += hitPos.z;
            velY = power * (power * (rand() / 32767.0f - 0.5f));
            mtxa_tf_vec_xyz(power * (power * (rand() / 32767.0f - 0.5f)), velY, 0.0f, &effect.vel);
            spread = (i - 1) * (2.5f / count);
            effect.vel.x += spread * v.x;
            effect.vel.y += spread * v.y;
            effect.vel.z += spread * v.z;
            effect.vel.y *= 0.75f;
            spawn_effect(&effect);
        }
        effect.type = 0x2E;
        effect.playerId = ball->playerId;
        effect.unk8C = dir;
        effect.pos = hitPos;
        effect.vel.x = 2.0f * power * dir.x;
        effect.vel.y = 2.0f * power * dir.y;
        effect.vel.z = 2.0f * power * dir.z;
        v.x = ball->vel.x - stobj->unk64.x;
        v.y = ball->vel.y - stobj->unk64.y;
        v.z = ball->vel.z - stobj->unk64.z;
        effect.colorFactor = 2.0 * fabs(mathutil_vec_dot_prod(&dir, &v));
        spawn_effect(&effect);

        v.x = ball->vel.x - stobj->unk64.x;
        v.y = ball->vel.y - stobj->unk64.y;
        v.z = ball->vel.z - stobj->unk64.z;
        dir.x = ball->pos.x - stobj->u_some_pos.x;
        dir.z = ball->pos.y - stobj->u_some_pos.y;
        dir.y = ball->pos.z - stobj->u_some_pos.z;
        vec_normalize_len(&dir);
        power = fabs(mathutil_vec_dot_prod(&dir, &v)) - 0.02f;
        if (power > 0.0f)
        {
            struct Ball *attacker = &balls[glove->playerIdx];

            if (!(attacker->flags & 0x20000000000LL))
                g_rumble_controller(player_pad_map[attacker->playerId], 1, 60.0f * (0.2f + 0.5f * power));
            if (!(ball->flags & 0x20000000000LL))
                g_rumble_controller(player_pad_map[playerId], 1, 60.0f * (0.2f + power));
        }
        if (lbl_1_bss_6FAA2 != 0 && (lbl_12_bss_1ACA0.flags & 0x20))
        {
            lbl_12_bss_1ACA0.unk1748 = 1;
            lbl_12_bss_1ACA0.players[glove->playerIdx].score += lbl_1_bss_6FAA2;
            fn_12_1623C(ball, &balls[glove->playerIdx], lbl_1_bss_6FAA2, 0, 1);
        }
        player = &lbl_12_bss_1ACA0.players[playerId];
        for (j = 0; j < g_poolInfo.playerPool.count; j++)
            player->hitBy[j] >>= 3;
        player->hitBy[glove->playerIdx] = 60.0f * lbl_1_bss_6FA9C;
        player->unk14 += 0x30;
        lbl_12_bss_1ACA0.gloves[playerId].flags |= 1;
        fn_12_23F60(playerId);

        prevVel = physBall->vel;
        pos = stobj->position;
        fn_1_A74E4(&physBall->prevPos, &physBall->pos, physBall->radius, &stobj->position_2, &pos, stobj->boundSphereRadius);
        stobj->u_some_pos.x += pos.x - stobj->position.x;
        stobj->u_some_pos.y += pos.y - stobj->position.y;
        stobj->u_some_pos.z += pos.z - stobj->position.z;
        stobj->position = pos;
        dir.x = physBall->pos.x - pos.x;
        dir.y = physBall->pos.y - pos.y;
        dir.z = physBall->pos.z - pos.z;
        vec_normalize_len(&dir);
        if (lbl_12_bss_1ACA0.gloves[playerId].flags & 0x10)
            power = 0.3f;
        else
            power = 1.0f;
        g_something_with_item_coli(&dir, &stobj->unk64, fn_12_474C(stobj), &physBall->vel,
            power * ((2.0f * ball->currRadius) * (2.0f * ball->currRadius)));
        t = 0.08f;
        if (glove->powerLevel != 0)
            t += glove->powerLevel * (0.4f / lbl_1_bss_6FA74);
        if (glove->chargeLevel != 0)
            t += 0.1f * glove->chargeLevel;
        v = dir;
        power = mathutil_vec_dot_prod(&stobj->u_local_pos, &v);
        if (power < 0.0f)
        {
            power = -power;
            v.x += power * stobj->u_local_pos.x;
            v.y += power * stobj->u_local_pos.y;
            v.z += power * stobj->u_local_pos.z;
        }
        physBall->vel.x += t * v.x;
        physBall->vel.y += t * v.y;
        physBall->vel.z += t * v.z;
        glove->hitCooldown[playerId] = 12;
        v = physBall->vel;
        if (ball->flags & 1)
        {
            dir = ball->unk11C;
            power = mathutil_vec_dot_prod(&v, &dir);
            if (power > 0.0f)
            {
                power = -power;
                v.x += power * dir.x;
                v.y += power * dir.y;
                v.z += power * dir.z;
            }
            power = mathutil_vec_dot_prod(&prevVel, &dir);
            if (power > 0.0f)
            {
                power = -power;
                prevVel.x += power * dir.x;
                prevVel.y += power * dir.y;
                prevVel.z += power * dir.z;
            }
        }
        len = math_sqrt(mathutil_sum_of_sq_3(prevVel.x - physBall->vel.x, prevVel.y - physBall->vel.y, prevVel.z - physBall->vel.z));
        if (len > lbl_1_bss_6FA84)
        {
            len -= lbl_1_bss_6FA84;
            len *= 1.0f / (lbl_1_bss_6FA80 - lbl_1_bss_6FA84);
            if (len > 1.0f)
                len = 1.0f;
            stun = lbl_1_bss_6FA8C * len;
            if (player->stunTimer < stun)
                player->stunTimer = stun;
            other = &lbl_12_bss_1ACA0.players[stobj->id];
            if (other->stunTimer < stun)
                other->stunTimer = stun;
            memset(&effect, 0, sizeof(effect));
            effect.playerId = ball->playerId;
            effect.type = 0x33;
            effect.pos = physBall->pos;
            effect.vel = physBall->vel;
            spawn_effect(&effect);
        }
    }
}

/* Glove stobj destroy: stops the hold sound. */
void fn_12_3894(struct Stobj *stobj)
{
    struct FightGlove *glove;

    glove = stobj->extraData;
    current_ball = &balls[glove->playerIdx];
    if (glove->sound38 >= 0)
    {
        SoundOff(glove->sound38);
        glove->sound38 = -1;
    }
}

/* Collides the glove with the stage; a punching glove reports the hit (fn_12_178CC) and
 * bounces back off walls that the item group flags as solid. */
void fn_12_38FC(struct Stobj *stobj)
{
    struct PhysicsBall physBall;
    Vec normal;
    Vec dir;
    struct FightStageHit hit;
    struct FightGlove *glove;
    struct Ball *ball;

    physBall.flags = 0;
    physBall.pos = stobj->u_some_pos;
    physBall.prevPos = stobj->unk7C;
    physBall.vel = stobj->unk64;
    physBall.radius = stobj->boundSphereRadius;
    physBall.gravityAccel = 0.0f;
    physBall.restitution = 0.5f;
    physBall.hardestColiSpeed = 0.0f;
    physBall.friction = 0.01f;
    physBall.itemGroupId = 0;
    physBall.hardestColiItemGroupId = 0;
    collide_ball_with_stage(&physBall, stagedef);
    if (physBall.flags & 1)
    {
        mtxa_from_mtx(itemgroups[physBall.hardestColiItemGroupId].transform);
        mtxa_tf_vec(&physBall.hardestColiNormal, &normal);
        glove = stobj->extraData;
        ball = &balls[glove->playerIdx];
        dir.x = stobj->position.x - ball->pos.x;
        dir.y = stobj->position.y - ball->pos.y;
        dir.z = stobj->position.z - ball->pos.z;
        vec_normalize_len(&dir);
        if (mathutil_vec_dot_prod(&normal, &dir) < 0.8f)
        {
            if (glove->flags & 8)
            {
                hit.type = 1;
                hit.playerId = glove->playerIdx;
                hit.itemGroup = physBall.hardestColiItemGroupId;
                hit.power = physBall.hardestColiSpeed;
                hit.pos = physBall.pos;
                hit.dir = normal;
                fn_12_178CC(&hit);
            }
            if (!(glove->flags & 8))
            {
                stobj->u_some_pos = physBall.pos;
                stobj->unk64 = physBall.vel;
            }
            else if (fn_12_17AB8(physBall.hardestColiItemGroupId) & 2)
            {
                stobj->u_some_pos = physBall.pos;
                stobj->unk64 = physBall.vel;
            }
        }
    }
}

/* Lays the arm segments out on the straight line from `from` (ball) to `to` (glove), at rest. */
void fn_12_3B8C(struct FightGlove *glove, Vec *from, Vec *to)
{
    struct FightGloveSeg *seg;
    int i;
    float t;
    float u;
    Vec a;
    Vec b;

    a = *from;
    b = *to;
    for (seg = glove->segs, i = 12; i > 0; i--, seg++)
    {
        seg->vel.x = 0.0f;
        seg->vel.y = 0.0f;
        seg->vel.z = 0.0f;
        t = i / 12.0f;
        u = 1.0f - t;
        seg->pos.x = a.x * t + b.x * u;
        seg->pos.y = a.y * t + b.y * u;
        seg->pos.z = a.z * t + b.z * u;
    }
}

/* Spring arm: pulls each segment towards its neighbours and the root-tip line, orients it
 * along the arm, and pushes every other segment out of the other balls. */
void fn_12_3C78(struct FightGlove *glove, Vec *root, Vec *rootDir, Vec *tip, Vec *tipDir)
{
    Vec a;
    Vec b;
    Vec aDir;
    Vec bDir;
    Vec prev;
    Vec cur;
    Vec dir;
    float d0x;
    float t;
    float d0y;
    int n;
    float d1x;
    float d0z;
    float dist;
    float lx;
    float d1y;
    float ly;
    struct FightGloveSeg *seg;
    float d1z;
    float lz;
    float u;
    int i;
    struct Ball *ball;
    s8 *status;
    int playerIdx;
    float radius;

    a = *root;
    b = *tip;
    dist = mathutil_vec_distance(&a, &b);
    aDir = *rootDir;
    bDir = *tipDir;
    aDir.x *= dist;
    aDir.y *= dist;
    aDir.z *= dist;
    bDir.x *= dist;
    bDir.y *= dist;
    bDir.z *= dist;
    seg = glove->segs;
    cur = seg->pos;
    d0x = a.x - cur.x;
    d0y = a.y - cur.y;
    d0z = a.z - cur.z;
    prev = a;
    for (i = 12; i > 1; i--, seg++)
    {
        cur = seg->pos;
        t = i / 12.0f;
        u = 1.0f - t;
        d1x = seg[1].pos.x - cur.x;
        d1y = seg[1].pos.y - cur.y;
        d1z = seg[1].pos.z - cur.z;
        lx = t * a.x + u * b.x;
        ly = t * a.y + u * b.y;
        lz = t * a.z + u * b.z;
        seg->vel.x *= 0.8f;
        seg->vel.y *= 0.8f;
        seg->vel.z *= 0.8f;
        seg->vel.x += 0.1f * (d0x + d1x) + 0.2f * (lx - cur.x);
        seg->vel.y += 0.1f * (d0y + d1y) + 0.2f * (ly - cur.y);
        seg->vel.z += 0.1f * (d0z + d1z) + 0.2f * (lz - cur.z);
        seg->pos.x += seg->vel.x;
        seg->pos.y += seg->vel.y;
        seg->pos.z += seg->vel.z;
        dir.x = ((d1x - d0x) + t * aDir.x) + u * bDir.x;
        dir.y = ((d1y - d0y) + t * aDir.y) + u * bDir.y;
        dir.z = ((d1z - d0z) + t * aDir.z) + u * bDir.z;
        vec_to_euler_xy(&dir, &seg->rotX, &seg->rotY);
        d0x = -d1x;
        d0y = -d1y;
        d0z = -d1z;
        prev = cur;
    }
    cur = seg->pos;
    t = 1.0f / 12.0f;
    u = 1.0f - t;
    d1x = b.x - cur.x;
    d1y = b.y - cur.y;
    d1z = b.z - cur.z;
    lx = t * a.x + u * b.x;
    ly = t * a.y + u * b.y;
    lz = t * a.z + u * b.z;
    seg->vel.x *= 0.8f;
    seg->vel.y *= 0.8f;
    seg->vel.z *= 0.8f;
    seg->vel.x += 0.1f * (d0x + d1x) + 0.2f * (lx - cur.x);
    seg->vel.y += 0.1f * (d0y + d1y) + 0.2f * (ly - cur.y);
    seg->vel.z += 0.1f * (d0z + d1z) + 0.2f * (lz - cur.z);
    seg->pos.x += seg->vel.x;
    seg->pos.y += seg->vel.y;
    seg->pos.z += seg->vel.z;
    dir.x = bDir.x + (d1x - d0x);
    dir.y = bDir.y + (d1y - d0y);
    dir.z = bDir.z + (d1z - d0z);
    vec_to_euler_xy(&dir, &seg->rotX, &seg->rotY);

    playerIdx = glove->playerIdx;
    ball = balls;
    status = g_poolInfo.playerPool.statusList;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, ball++, status++)
    {
        if (ball->playerId != playerIdx && *status != 0 && !(ball->flags & 0x800000000LL))
        {
            a = ball->pos;
            radius = ball->currRadius;
            for (seg = glove->segs, n = 12; n > 0; n -= 2, seg += 2)
            {
                b.x = seg->pos.x - a.x;
                b.y = seg->pos.y - a.y;
                b.z = seg->pos.z - a.z;
                if (vec_normalize_len(&b) < 0.2f + radius)
                {
                    seg->pos.x = a.x + (0.2f + radius) * b.x;
                    seg->pos.y = a.y + (0.2f + radius) * b.y;
                    seg->pos.z = a.z + (0.2f + radius) * b.z;
                    dist = mathutil_vec_dot_prod(&b, &seg->vel);
                    if (dist < 0.0f)
                    {
                        dist *= -1.5f;
                        seg->vel.x += dist * b.x;
                        seg->vel.y += dist * b.y;
                        seg->vel.z += dist * b.z;
                    }
                }
            }
        }
    }
}

/* Glove stobj draw helper: the 12 arm segments (in the reflection pass only those above
 * the water line). */
void fn_12_43E4(struct FightGlove *glove)
{
    struct FightGloveSeg *seg;
    int i;
    struct GMAModel *model;
    float reach;
    float width;
    float length;

    model = minigameGma->modelEntries[0xA6].model;
    seg = glove->segs;
    reach = fn_12_4710(glove->stobj);
    width = 1.0f + (1.0f / 12.0f) * reach;
    length = 1.0f + 2.0f * reach;
    if (!(g_some_render_flag & 4))
    {
        for (i = 12; i > 0; i--, seg++)
        {
            mtxa_from_mtxb_translate(&seg->pos);
            mtxa_rotate_y(seg->rotY);
            mtxa_rotate_x(seg->rotX);
            mtxa_rotate_z(seg->rotZ);
            mtxa_scale_xyz(width, width, length);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(model);
        }
    }
    else
    {
        for (i = 12; i > 0; i--, seg++)
        {
            if (seg->pos.y < -0.2f)
                continue;
            mtxa_from_mtxb_translate(&seg->pos);
            mtxa_rotate_y(seg->rotY);
            mtxa_rotate_x(seg->rotX);
            mtxa_rotate_z(seg->rotZ);
            mtxa_scale_xyz(width, width, length);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(model);
        }
    }
}

/* Collision radius of a glove stobj from its radius/power/charge levels. */
float fn_12_457C(struct Stobj *stobj)
{
    struct FightGlove *glove = stobj->extraData;
    float level;

    level = glove->radLevel;
    if (glove->powerLevel != 0)
        level += 0.25f * glove->powerLevel;
    if (glove->chargeLevel != 0)
        level += 0.5f * glove->chargeLevel;
    return lbl_1_bss_6FABC * (1.0f + lbl_1_bss_6FAB8 * level);
}

/* Punch reach of a glove stobj from its range/charge levels. */
float fn_12_4658(struct Stobj *stobj)
{
    struct FightGlove *glove = stobj->extraData;
    float level;

    level = glove->rangeLevel;
    if (glove->chargeLevel != 0)
        level += 1.25f * glove->chargeLevel;
    return (lbl_1_bss_6FAB4 * glove->unk20) * (1.0f + lbl_1_bss_6FAB0 * level);
}

/* Current (smoothed) range extension of a glove stobj, used to stretch the arm. */
float fn_12_4710(struct Stobj *stobj)
{
    struct FightGlove *glove = stobj->extraData;
    float range = glove->unk58;

    return 0.5f * (range * (lbl_1_bss_6FAB4 * lbl_1_bss_6FAB0));
}

/* Punch mass of a glove stobj from its power level. */
float fn_12_474C(struct Stobj *stobj)
{
    struct FightGlove *glove = stobj->extraData;
    float level;

    level = glove->powerLevel;
    return (lbl_1_bss_6FAC8 * glove->unk18) * (1.0f + lbl_1_bss_6FAC4 * level);
}

/* Keeps the glove out of the camera (camera modes 3/9): pushes it off a sphere around the
 * eye and turns its rest direction away from the camera. */
void fn_12_47C0(struct Stobj *stobj)
{
    Vec pos;
    Vec eye2;
    Vec d;
    Vec pushed;
    Vec eye;
    Vec prevEye;
    float radius;
    float dot;

    mtxa_from_mtxb();
    mtxa_rigid_inv_tf_tl(&eye);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[3]);
    mtxa_rigid_inv_tf_tl(&prevEye);
    pos = stobj->position;
    eye2 = eye;
    if (fn_1_A74E4(&stobj->position_2, &pos, stobj->boundSphereRadius, &prevEye, &eye2, 0.5f) != 0)
    {
        d.x = stobj->position.x - eye.x;
        d.y = stobj->position.y - eye.y;
        d.z = stobj->position.z - eye.z;
        radius = 0.5f + stobj->boundSphereRadius;
        if (mathutil_vec_len(&d) < radius)
        {
            pushed = d;
            vec_set_len(radius, &pushed, &pushed);
            pushed.x += eye.x;
            pushed.y += eye.y;
            pushed.z += eye.z;
            stobj->u_some_pos.x += pushed.x - stobj->position.x;
            stobj->u_some_pos.y += pushed.y - stobj->position.y;
            stobj->u_some_pos.z += pushed.z - stobj->position.z;
            stobj->position.x = pushed.x;
            stobj->position.y = pushed.y;
            stobj->position.z = pushed.z;
            vec_normalize_len(&d);
            dot = mathutil_vec_dot_prod(&d, &stobj->unk64);
            if (dot < 0.0f)
            {
                dot *= -1.2f;
                stobj->unk64.x += d.x * dot;
                stobj->unk64.y += d.y * dot;
                stobj->unk64.z += d.z * dot;
            }
            d.x = stobj->u_some_pos.x - eye2.x;
            d.y = 0.0f;
            d.z = stobj->u_some_pos.z - eye2.z;
            if (0.0f == d.x && 0.0f == d.z)
                d.z = -1.0f;
            else
                vec_normalize_len(&d);
            stobj->u_local_pos.x += 0.1f * (d.x - stobj->u_local_pos.x);
            stobj->u_local_pos.y += 0.1f * (d.y - stobj->u_local_pos.y);
            stobj->u_local_pos.z += 0.1f * (d.z - stobj->u_local_pos.z);
        }
    }
}

/* Punch auto-aim: bends `dir` towards the ball in reach that lies most in front of `ball`. */
void fn_12_4AE4(struct Ball *ball, Vec *dir)
{
    Vec fwd;
    Vec best;
    Vec pos;
    Vec d;
    int n;
    s8 *status;
    int found;
    struct Ball *target;
    struct Ball *other;
    struct Stobj *stobj;
    float bestScore;
    float reach;
    float radius;
    float len;
    float score;

    fwd = *dir;
    vec_normalize_len(&fwd);
    best = fwd;
    pos = ball->pos;
    stobj = lbl_12_bss_1ACA0.gloves[ball->playerId].stobj;
    radius = fn_12_457C(stobj);
    reach = fn_12_4658(stobj);
    found = 0;
    bestScore = -100.0f;
    target = NULL;
    status = g_poolInfo.playerPool.statusList;
    other = balls;
    for (n = g_poolInfo.playerPool.count; n > 0; n--, other++)
    {
        if (*status == 0)
            continue;
        d.x = other->pos.x - pos.x;
        d.y = 0.0f;
        d.z = other->pos.z - pos.z;
        len = mathutil_vec_len(&d);
        if (len > reach + 2.0f * radius)
            continue;
        if (0.0f != len)
        {
            score = mathutil_vec_dot_prod(&d, &fwd);
            score *= 1.0f / len;
            score *= 1.0f / (0.5f * len);
        }
        else
            score = 1.0f + bestScore;
        if (bestScore < score)
        {
            bestScore = score;
            target = other;
            best = d;
        }
    }
    if (bestScore > 0.0f && target != NULL)
    {
        found = 1;
        best.x = 0.9f * best.x + 0.1f * fwd.x;
        best.y = 0.9f * best.y + 0.1f * fwd.y;
        best.z = 0.9f * best.z + 0.1f * fwd.z;
        vec_normalize_len(&best);
    }
    if (found)
        *dir = best;
}

/* charge level effect model per level 1-4 (init_common_gma) */
s16 lbl_12_data_2D8[4] = { 0x2B, 0x2B, 0x2B, 0x2C };
/* item box piece models (minigameGma) */
u8 lbl_12_data_2E0[35] = {
    0x26, 0x31, 0x3C, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
    0x2E, 0x2F, 0x30, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42,
};
/* item box piece models, reflection pass (minigameGma) */
u8 lbl_12_data_304[35] = {
    0x03, 0x0E, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x1A, 0x1B, 0x1C,
    0x1D, 0x1E, 0x1F,
};

/* Spawns the glove's charge effect for charge level `level` (0: start, 4: full). */
void fn_12_4EF8(struct FightGlove *glove, int level)
{
    struct Effect effect;

    memset(&effect, 0, sizeof(effect));
    effect.playerId = glove->playerIdx;
    switch (level)
    {
    case 0:
        effect.type = 0x30;
        effect.colorFactor = 1.0f;
        effect.model = init_common_gma->modelEntries[0x2F].model;
        break;
    case 1:
    case 2:
    case 3:
        effect.type = 0x31;
        effect.colorFactor = 1.0f + 0.25f * level;
        effect.model = init_common_gma->modelEntries[lbl_12_data_2D8[level - 1]].model;
        break;
    case 4:
    default:
        effect.type = 0x30;
        effect.colorFactor = 2.0f;
        effect.model = init_common_gma->modelEntries[lbl_12_data_2D8[level - 1]].model;
        break;
    }
    spawn_effect(&effect);
}

/* Breaks the glove apart: spawns flying pieces of the glove and of each arm segment
 * (`speed` adds to their random spread). */
void fn_12_5050(struct FightGlove *glove, float arg1)
{
    float speed = arg1;
    float spread;
    struct Effect effect;
    int i;
    struct Stobj *stobj;
    struct FightGloveSeg *seg;

    stobj = glove->stobj;
    memset(&effect, 0, sizeof(effect));
    effect.type = 0x34;
    effect.playerId = glove->playerIdx;
    if (glove->powerLevel == 0)
        effect.model = stobj->model;
    else
        effect.model = glove->unk54;
    effect.pos = stobj->u_some_pos;
    spread = speed + 0.4f * mathutil_vec_len(&stobj->unk64);
    effect.vel.x = stobj->unk64.x + spread * (RAND_FLOAT() - 0.5f);
    effect.vel.y = stobj->unk64.y + spread * (RAND_FLOAT() - 0.5f);
    effect.vel.z = stobj->unk64.z + spread * (RAND_FLOAT() - 0.5f);
    effect.rotX = stobj->rotX;
    effect.rotY = stobj->rotY;
    effect.rotZ = stobj->rotZ;
    spread = mathutil_vec_len(&effect.vel);
    spread *= 12288.0f;
    effect.unk52 = stobj->unk76 + spread * (RAND_FLOAT() - 0.5f);
    effect.unk54 = stobj->unk78 + spread * (RAND_FLOAT() - 0.5f);
    effect.unk56 = stobj->unk7A + spread * (RAND_FLOAT() - 0.5f);
    effect.scale.x = stobj->unk48;
    effect.scale.y = stobj->unk48;
    effect.scale.z = stobj->unk48;
    effect.unk98.x = 0.97f;
    effect.unk98.y = 0.97f;
    effect.unk98.z = 0.97f;
    spawn_effect(&effect);

    effect.model = minigameGma->modelEntries[0xA6].model;
    effect.scale.x = 1.0f;
    effect.scale.y = 1.0f;
    effect.scale.z = 1.0f;
    effect.unk98.x = 0.98f;
    effect.unk98.y = 0.98f;
    effect.unk98.z = 0.98f;
    for (seg = glove->segs, i = 12; i > 0; i--, seg++)
    {
        effect.pos = seg->pos;
        spread = speed + 0.5f * mathutil_vec_len(&seg->vel);
        effect.vel.x = 0.5f * seg->vel.x + spread * (RAND_FLOAT() - 0.5f);
        effect.vel.y = 0.5f * seg->vel.y + spread * (RAND_FLOAT() - 0.5f);
        effect.vel.z = 0.5f * seg->vel.z + spread * (RAND_FLOAT() - 0.5f);
        effect.rotX = seg->rotX;
        effect.rotY = seg->rotY;
        effect.rotZ = seg->rotZ;
        spread = mathutil_vec_len(&effect.vel);
        spread *= 12288.0f;
        effect.unk52 += spread * (RAND_FLOAT() - 0.5f);
        effect.unk54 += spread * (RAND_FLOAT() - 0.5f);
        effect.unk56 += spread * (RAND_FLOAT() - 0.5f);
        spawn_effect(&effect);
    }
}

/* Unpowered glove hit: throws off a copy of the glove as an effect and shrinks it. */
void fn_12_56B8(struct FightGlove *glove)
{
    struct Effect effect;
    struct Stobj *stobj;
    float spread;

    if (glove->powerLevel > 0)
        return;
    stobj = glove->stobj;
    memset(&effect, 0, sizeof(effect));
    effect.type = 0x34;
    effect.playerId = glove->playerIdx;
    effect.model = stobj->model;
    effect.pos = stobj->u_some_pos;
    spread = mathutil_vec_len(&stobj->unk64);
    spread *= 0.5f * RAND_FLOAT();
    effect.vel.x = 0.8f * stobj->unk64.x + stobj->u_local_pos.x * spread;
    effect.vel.y = 0.8f * stobj->unk64.y + stobj->u_local_pos.y * spread + 0.3f * (0.5f + 0.5f * RAND_FLOAT());
    effect.vel.z = 0.8f * stobj->unk64.z + stobj->u_local_pos.z * spread;
    effect.rotX = stobj->rotX;
    effect.rotY = stobj->rotY;
    effect.rotZ = stobj->rotZ;
    spread = mathutil_vec_len(&effect.vel);
    spread *= 12288.0f;
    effect.unk52 = stobj->unk76 + spread * (RAND_FLOAT() - 0.5f);
    effect.unk54 = stobj->unk78 + spread * (RAND_FLOAT() - 0.5f);
    effect.unk56 = stobj->unk7A + spread * (RAND_FLOAT() - 0.5f);
    effect.scale.x = stobj->unk48;
    effect.scale.y = stobj->unk48;
    effect.scale.z = stobj->unk48;
    effect.unk98.x = 0.97f;
    effect.unk98.y = 0.99f;
    effect.unk98.z = 0.97f;
    spawn_effect(&effect);
    stobj->boundSphereRadius *= 0.25f;
}

/* Clears the item boxes and looks up the item box models. */
void fn_12_5A10(void)
{
    int i;
    u8 *ids;
    u8 *reflIds;
    struct GMAModel **models;
    struct GMAModel **reflModels;

    /* dead stores: MWCC lays out .bss in first-reference order, these put lbl_12_bss_0/8C first */
    reflModels = lbl_12_bss_0;
    models = lbl_12_bss_8C;
    memset(lbl_12_bss_118, 0, sizeof(lbl_12_bss_118));
    lbl_12_bss_1AC98 = minigameGma->modelEntries[0x96].model;
    ids = lbl_12_data_2E0;
    models = lbl_12_bss_8C;
    reflIds = lbl_12_data_304;
    reflModels = lbl_12_bss_0;
    for (i = 35; i > 0; i--, ids++, models++, reflIds++, reflModels++)
    {
        *models = minigameGma->modelEntries[*ids].model;
        *reflModels = minigameGma->modelEntries[*reflIds].model;
    }
}
