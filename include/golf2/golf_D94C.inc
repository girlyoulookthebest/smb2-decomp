/* golf.c .text 0xD94C-0x129FC: turn/hole changes, shot setup, score queries, trail drawing (w1) */

void disable_all_cameras(void);
void g_create_stage_player_wrapper(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod,
                                   void (*drawFunc)(struct Ape *));
struct GMAModel *fn_1_5D6F4(char *name);
extern s32 active_monkey_id[];
extern s32 locale;

void fn_11_C628(void);
void fn_11_19F24(struct Ball *ball);
void fn_11_1A610(struct Ape *ape);
void fn_11_1CB74(void);
void fn_11_1CCF0(void);

void fn_11_D94C(void);
void fn_11_E164(void);
void fn_11_E2A4(void);
void fn_11_E5AC(void);
void fn_11_E778(void);
u8 fn_11_E9F0(void);
u8 fn_11_EF64(void);
float fn_11_F2A8(Vec *pos);
void fn_11_103B0(void);
void fn_11_11388(void);

/* Next player's turn: reset the ball and cameras, pick the lie and a club for the distance. */
void fn_11_D94C(void)
{
    s8 prevPlayer;
    int prevHole;
    int i;
    struct RaycastHit hit;
    float dist;
    float power;

    lbl_11_bss_30 = 2;
    lbl_11_bss_17C = 0;
    lbl_11_bss_B18 = 0;
    lbl_11_bss_54 = 0;
    lbl_11_bss_50 = 0;
    lbl_11_bss_60 = 0;
    lbl_11_bss_20->state = 4;
    lbl_11_bss_20->flags |= 0x10;
    lbl_11_bss_20->flags &= ~0x4000;
    if (lbl_11_bss_20->ape != NULL)
        lbl_11_bss_20->ape->flags |= 0x20;
    lbl_11_bss_20->flags &= ~0x200;
    if (lbl_11_bss_20->ape != NULL)
        lbl_11_bss_20->ape->flags &= ~0x4000;
    prevPlayer = modeCtrl.currPlayer;
    prevHole = lbl_11_bss_C8;
    fn_11_C628();
    if (prevPlayer != modeCtrl.currPlayer && prevHole == lbl_11_bss_C8)
    {
        lbl_11_bss_74 = 1;
        lbl_11_bss_78 = 120;
        lbl_11_bss_6C = 1;
    }
    else
    {
        lbl_11_bss_74 = 0;
        lbl_11_bss_78 = 0;
        lbl_11_bss_6C = 0;
    }
    lbl_11_bss_20->state = 0x1A;
    if (lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8] == 0)
        lbl_11_bss_20->unk154 = 1;
    else
        lbl_11_bss_20->unk154 = 4;
    if (lbl_11_bss_20->ape != NULL)
        lbl_11_bss_20->ape->ballIdx = modeCtrl.currPlayer;
    lbl_11_bss_20->flags &= ~0x10;
    lbl_11_bss_20->flags |= 0x4000;
    if (lbl_11_bss_20->ape != NULL)
        lbl_11_bss_20->ape->flags &= ~0x20;
    lbl_11_bss_24->unk1F = 2;
    lbl_11_bss_24->eye = cameras[prevPlayer].eye;
    lbl_11_bss_24->unk26 = 0;
    lbl_11_bss_24->flags &= ~0x10;
    disable_all_cameras();
    enable_camera(modeCtrl.currPlayer);
    lbl_11_bss_28->unk1F = 9;
    lbl_11_bss_28->unk26 = 11;
    lbl_11_bss_28->flags |= 0x10;
    for (i = 0; i < 5; i++)
        set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
    set_camera_view_settings(modeCtrl.currPlayer, 0.0f, 0.0f, 1.0f, 1.0f);
    set_camera_view_settings((modeCtrl.currPlayer + 1) % 5, 0.775f, 0.0f, 0.225f, 0.2875f);
    raycast_stage_down(&lbl_11_bss_20->pos, &hit, NULL);
    lbl_11_bss_7C = 0;
    if (hit.flags & 2)
    {
        lbl_11_bss_7C = 1;
        lbl_11_bss_CA = 3;
        printf("shot_on_green %d\n", modeCtrl.currPlayer);
    }
    else if (hit.flags & 4)
    {
        lbl_11_bss_CA = 1;
        printf("shot_on_rough %d\n", modeCtrl.currPlayer);
    }
    else if (hit.flags & 8)
    {
        lbl_11_bss_CA = 2;
        printf("shot_on_bunk %d\n", modeCtrl.currPlayer);
    }
    else
    {
        lbl_11_bss_CA = 0;
        printf("shot_on_fairway %d\n", modeCtrl.currPlayer);
    }
    dist = fn_11_F2A8(&lbl_11_bss_20->pos) / lbl_11_bss_94;
    if (lbl_11_bss_CA == 3)
    {
        if (dist > 30.0)
            lbl_11_bss_4C = 12;
        else if (dist > 10.0)
            lbl_11_bss_4C = 13;
        else
            lbl_11_bss_4C = 14;
    }
    else
    {
        if (dist > 250.0)
            lbl_11_bss_4C = 0;
        else if (dist > 225.0)
            lbl_11_bss_4C = 1;
        else if (dist > 210.0)
            lbl_11_bss_4C = 2;
        else if (dist > 195.0)
            lbl_11_bss_4C = 3;
        else if (dist > 180.0)
            lbl_11_bss_4C = 4;
        else if (dist > 165.0)
            lbl_11_bss_4C = 5;
        else if (dist > 155.0)
            lbl_11_bss_4C = 6;
        else if (dist > 145.0)
            lbl_11_bss_4C = 7;
        else if (dist > 135.0)
            lbl_11_bss_4C = 8;
        else if (dist > 120.0)
            lbl_11_bss_4C = 9;
        else if (dist > 105.0)
            lbl_11_bss_4C = 10;
        else
            lbl_11_bss_4C = 11;
        printf("Select Club: %d\n", lbl_11_bss_4C);
    }
    if (lbl_11_bss_4C >= 12 || lbl_11_bss_CA == 3)
        lbl_11_bss_58 = lbl_11_data_2FC[12];
    else
        lbl_11_bss_58 = &lbl_11_data_2FC[lbl_11_bss_4C][lbl_11_bss_CA];
    lbl_11_bss_8 = 24;
    lbl_11_bss_188 = 0;
    if (lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14)
    {
        power = (float)lbl_11_bss_94 * lbl_11_data_1A0[lbl_11_bss_4C] * lbl_11_data_2EC[lbl_11_bss_CA];
        lbl_11_bss_20->vel.x = power * math_sin(lbl_11_data_180[lbl_11_bss_4C] + 0x4000) * -math_sin(lbl_11_bss_20->unk92);
        lbl_11_bss_20->vel.y = power * math_sin(lbl_11_data_180[lbl_11_bss_4C]);
        lbl_11_bss_20->vel.z = power * math_sin(lbl_11_data_180[lbl_11_bss_4C] + 0x4000) * -math_sin(lbl_11_bss_20->unk92 + 0x4000);
        fn_11_19F24(lbl_11_bss_20);
        lbl_11_bss_20->vel.x = 0.0f;
        lbl_11_bss_20->vel.y = 0.0f;
        lbl_11_bss_20->vel.z = 0.0f;
    }
    lbl_11_bss_18C = lbl_11_bss_198[lbl_11_bss_50 - 1];
}

/* Advance to the next hole, or finish the round after the last one. */
void fn_11_E164(void)
{
    lbl_11_bss_C8++;
    lbl_11_bss_CA = 0;
    lbl_11_bss_4C = 0;
    lbl_11_bss_58 = lbl_11_data_2FC[0];
    if (lbl_11_bss_C8 > lbl_11_bss_2E)
    {
        lbl_11_bss_30 = 0x2000;
        fn_11_E778();
        return;
    }
    lbl_11_bss_30 = 2;
    lbl_11_bss_17C = 0;
    fn_11_E5AC();
    if (stagedef->extraAnims != NULL)
    {
        lbl_11_bss_38 = *(Vec *)stagedef->extraAnims;
        printf("other_data->cup\n");
        printf("x : %lf\n", lbl_11_bss_38.x);
        printf("y : %lf\n", lbl_11_bss_38.y);
        printf("z : %lf\n", lbl_11_bss_38.z);
        if (fn_11_EF64() == 0)
        {
            fn_11_1CB74();
            fn_11_1CCF0();
            mtxa_tf_point(&lbl_11_bss_38, &lbl_11_bss_38);
        }
    }
}

/* Restart the course at its first hole. */
void fn_11_E2A4(void)
{
    int i;

    lbl_11_bss_C8 = lbl_11_bss_2C;
    fn_11_E5AC();
    if (stagedef->extraAnims != NULL)
    {
        lbl_11_bss_38 = *(Vec *)stagedef->extraAnims;
        printf("other_data->cup\n");
        printf("x : %lf\n", lbl_11_bss_38.x);
        printf("y : %lf\n", lbl_11_bss_38.y);
        printf("z : %lf\n", lbl_11_bss_38.z);
        if (fn_11_EF64() == 0)
        {
            fn_11_1CB74();
            fn_11_1CCF0();
            mtxa_tf_point(&lbl_11_bss_38, &lbl_11_bss_38);
        }
    }
    lbl_11_bss_20->state = 0x1A;
    lbl_11_bss_20->unk154 = 0;
    if (lbl_11_bss_20->ape != NULL)
        lbl_11_bss_20->ape->ballIdx = modeCtrl.currPlayer;
    lbl_11_bss_20->flags &= ~0x10;
    lbl_11_bss_20->flags |= 0x4000;
    if (lbl_11_bss_20->ape != NULL)
        lbl_11_bss_20->ape->flags &= ~0x20;
    lbl_11_bss_24->unk1F = 2;
    lbl_11_bss_24->eye = cameras[(modeCtrl.currPlayer + 4) % 5].eye;
    lbl_11_bss_24->unk26 = 0;
    lbl_11_bss_24->flags &= ~0x10;
    disable_all_cameras();
    enable_camera(modeCtrl.currPlayer);
    lbl_11_bss_28->unk1F = 9;
    lbl_11_bss_28->unk26 = 11;
    lbl_11_bss_28->flags |= 0x10;
    for (i = 0; i < 5; i++)
        set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
    set_camera_view_settings(modeCtrl.currPlayer, 0.0f, 0.0f, 1.0f, 1.0f);
    set_camera_view_settings((modeCtrl.currPlayer + 1) % 5, 0.775f, 0.0f, 0.225f, 0.2875f);
}

/* Load the stage of the current hole (if not loaded yet) and re-roll the wind. */
void fn_11_E5AC(void)
{
    if (current_stage_id != lbl_11_data_204[lbl_11_bss_C8])
    {
        dest_all_events();
        golf_wind_angle = 65536.0f * ((f32)rand() / 32767.0f) - 32768.0f;
        golf_wind_velocity = (f32)((rand() & 0x7FFF) % 600) / 60.0f;
#ifndef __MWERKS__
        pc_golf_test_wind(); /* test hook: SMB2_PC_GOLF_WIND (pc_launcher.c) */
#endif
        load_stage(lbl_11_data_204[lbl_11_bss_C8]);
        if (lbl_11_bss_C8 < 17)
            queue_stage_load(lbl_11_data_204[lbl_11_bss_C8 + 1]);
        else
            queue_stage_load(lbl_11_data_204[17]);
        event_init(0);
        fn_11_11388();
        event_init(0x10);
        event_init(0x11);
        event_init(0x13);
        event_init(0xE);
        event_init(0x14);
        event_init(0xB);
        event_init(3);
        lbl_11_bss_18 = 1;
        enable_camera(modeCtrl.currPlayer);
        camera_set_state(0x40);
        textdraw_set_font(0x8F);
    }
}

/* End of the round: load the results stage with player 1 in front. */
void fn_11_E778(void)
{
    int i;

    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    free_minigame_gma_tpl();
    g_something_calls_sndPopGroup();
    load_stage(lbl_11_data_204[17]);
    event_init(0x10);
    event_init(0x13);
    event_init(0x14);
    event_init(0xE);
    event_init(0x11);
    fn_11_11388();
    event_init(3);
    lbl_11_bss_18 = 1;
    g_poolInfo.playerPool.statusList[0] = balls[0].status = 2;
    g_poolInfo.playerPool.statusList[1] = balls[1].status = 0;
    g_poolInfo.playerPool.statusList[2] = balls[2].status = 0;
    g_poolInfo.playerPool.statusList[3] = balls[3].status = 0;
    modeCtrl.currPlayer = 0;
    lbl_11_bss_20 = &balls[modeCtrl.currPlayer];
    lbl_11_bss_24 = &cameras[modeCtrl.currPlayer];
    lbl_11_bss_28 = &cameras[(modeCtrl.currPlayer + 1) % 5];
    enable_camera(modeCtrl.currPlayer);
    for (i = 0; i < 5; i++)
        set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
    set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
}

/* TRUE if every player has its lbl_11_data_258 flag set. */
u8 fn_11_E9A8(void)
{
    int i;
    int all = TRUE;

    for (i = 0; i < modeCtrl.playerCount; i++)
    {
        if (lbl_11_data_258[i] == 0)
            all = FALSE;
    }
    return all;
}

/* TRUE if some player has not teed off on this hole yet. */
u8 fn_11_E9F0(void)
{
    int i;
    int ret = FALSE;

    for (i = 0; i < modeCtrl.playerCount; i++)
    {
        if (lbl_11_data_29C[i][lbl_11_bss_C8] == 0)
            ret = TRUE;
        if (math_sqrt(mathutil_sum_of_sq_3(balls[i].pos.x - stagedef->startPos->pos.x, 0.0f,
                                           balls[i].pos.z - stagedef->startPos->pos.z)) < 0.01
            && lbl_11_data_258[i] == 0)
            ret = TRUE;
    }
    return ret;
}

s16 fn_11_EB18(void)
{
    return lbl_11_bss_C8;
}

u8 fn_11_EB28(void)
{
    return lbl_11_bss_145;
}

void fn_11_EB38(Vec *v)
{
    lbl_11_bss_148 = *v;
}

void fn_11_EB5C(Vec *v)
{
    lbl_11_bss_154 = *v;
}

void fn_11_EB80(Vec *v)
{
    *v = lbl_11_bss_154;
}

void fn_11_EBA4(s16 *p)
{
    lbl_11_bss_B10 = *p;
}

void fn_11_EBB8(s16 *p)
{
    *p = lbl_11_bss_B10;
}

void fn_11_EBCC(s16 *p)
{
    lbl_11_bss_B12 = *p;
}

s32 fn_11_EBE0(void)
{
    return lbl_11_bss_AFC;
}

s32 fn_11_EBF0(void)
{
    return lbl_11_bss_B00;
}

/* strokes of a player on a hole */
u8 fn_11_EC00(int player, int idx)
{
    return lbl_11_data_29C[player][idx];
}

u8 fn_11_EC1C(int player)
{
    return lbl_11_data_2E4[player];
}

u8 fn_11_EC30(void)
{
    return lbl_11_bss_C8;
}

u16 fn_11_EC44(void)
{
    return lbl_11_bss_DE;
}

s8 fn_11_EC54(void)
{
    return lbl_11_data_254;
}

void fn_11_EC64(s8 hole)
{
    lbl_11_bss_C8 = hole;
}

u8 fn_11_EC78(void)
{
    return lbl_11_data_25C[modeCtrl.currPlayer];
}

u8 fn_11_EC98(void)
{
    return lbl_11_bss_E8;
}

/* Distance left to the cup in yards (or metres), per club scale. */
float fn_11_ECA8(void)
{
    float unit = 1.0f;
    float scale = 1.0f;

    if (lbl_11_bss_4C >= 12)
        scale = 0.914f;
    if (locale == 0 && lbl_11_bss_7C != 0)
        unit = 0.3048f;
    if (lbl_11_bss_145 != 0 && lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8] == 0)
        return fn_11_F2A8(&stagedef->startPos->pos) / lbl_11_bss_94 / unit / scale;
    return fn_11_F2A8(&lbl_11_bss_148) / lbl_11_bss_94 / unit / scale;
}

/* Height difference between the ball and the cup. */
float fn_11_EDF8(void)
{
    float scale = 0.914f;
    float unit = 1.0f;

    if (locale == 0 && lbl_11_bss_7C != 0)
    {
        scale = 1.0f;
        unit = 0.3048f;
    }
    if (fn_11_F2A8(&lbl_11_bss_20->pos) < 2.0)
        return 0.0f;
    return (lbl_11_bss_148.y - 0.5 - (1.0 + lbl_11_bss_38.y)) / lbl_11_bss_94 / scale / unit;
}

u8 fn_11_EEFC(void)
{
    if (lbl_11_bss_30 == 0x800 || lbl_11_bss_30 == 0x1000)
        return TRUE;
    return FALSE;
}

/* TRUE on the last hole of the course */
u8 fn_11_EF34(void)
{
    return lbl_11_bss_C8 == lbl_11_bss_2E;
}

/* TRUE if the current hole's stage is in the lbl_11_data_1DC list. */
u8 fn_11_EF64(void)
{
    int i;

    for (i = 0; i < lbl_11_data_200; i++)
    {
        if (lbl_11_data_1DC[i] == lbl_11_data_204[lbl_11_bss_C8])
            return TRUE;
    }
    return FALSE;
}

/* Result of a player on this hole: 0 under par .. 2 over, 3 unknown (match play compares players 0/1). */
u8 fn_11_EFC4(u8 player)
{
    if (lbl_1_bss_1704.unk0 == 0)
    {
        if (fn_11_170E8(player) <= -1)
            return 0;
        if (fn_11_170E8(player) <= 0)
            return 1;
        return 2;
    }
    if (player == 0)
    {
        if (lbl_11_data_29C[0][lbl_11_bss_C8] < lbl_11_data_29C[1][lbl_11_bss_C8])
            return 0;
        if (lbl_11_data_29C[0][lbl_11_bss_C8] == lbl_11_data_29C[1][lbl_11_bss_C8])
            return 1;
        if (lbl_11_data_29C[0][lbl_11_bss_C8] > lbl_11_data_29C[1][lbl_11_bss_C8])
            return 2;
    }
    else if (player == 1)
    {
        if (lbl_11_data_29C[1][lbl_11_bss_C8] < lbl_11_data_29C[0][lbl_11_bss_C8])
            return 0;
        if (lbl_11_data_29C[1][lbl_11_bss_C8] == lbl_11_data_29C[0][lbl_11_bss_C8])
            return 1;
        if (lbl_11_data_29C[1][lbl_11_bss_C8] > lbl_11_data_29C[0][lbl_11_bss_C8])
            return 2;
    }
    return 3;
}

u8 fn_11_F220(u8 player)
{
    if (fn_11_170E8(player) < -1)
        return 0;
    if (fn_11_170E8(player) < 1)
        return 1;
    return 2;
}

void fn_11_F27C(Mtx m)
{
    mtxa_from_mtx(m);
    mtxa_to_mtx(lbl_11_bss_98);
}

/* Distance from pos to the cup (scaled for the club). */
float fn_11_F2A8(Vec *pos)
{
    float scale = 0.914f;

    if (lbl_11_bss_4C >= 12)
        scale = 1.0f;
    return math_sqrt(mathutil_sum_of_sq_3(lbl_11_bss_38.x - pos->x, lbl_11_bss_38.y - pos->y, lbl_11_bss_38.z - pos->z)) / scale;
}

void fn_11_F350(void)
{
    s8 state = events[EVENT_STAGE].state;

    if (state == EV_STATE_RUNNING || state == EV_STATE_SUSPENDED)
        fn_11_103B0();
}

/* Stand-in for a function the linker dead-stripped from the REL: only its float literal
 * (9999.0f, .rodata 0x338) survives, between fn_11_F2A8's and fn_11_F394's literals. */
void golf_stripped_F394_literal(float *out)
{
    *out = 9999.0f;
}

#include "golf2/golf_F394.inc"

/* Draw the green's MOT_GREEN_CLASSIZ model in every item group. */
void fn_11_103B0(void)
{
    int i;
    int j;
    struct ItemGroup *itemgroup;
    struct StageCollHeader *coll;
    struct GMAModel *green;
    struct StageModelRef * DPTR *refs;
    struct GMAModel *model;

    if (stage_gma == NULL)
        return;
    itemgroup = itemgroups;
    coll = stagedef->collHeaders;
    green = fn_1_5D6F4("MOT_GREEN_CLASSIZ");
    for (i = 0; i < stagedef->itemGroupCount; i++, itemgroup++, coll++)
    {
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(itemgroup->transform);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        refs = coll->modelRefs;
        for (j = 0; j < coll->modelRefCount; j++, refs++)
        {
            model = g_some_draw_func2(*refs);
            if (model != NULL && green != NULL && model == green)
                avdisp_draw_model_culled_sort_never(model);
        }
    }
}

/* Stand-in for a dead-stripped stage-polygon debug function: only its strings survive
 * (.data 0x818-0x860, right after fn_11_103B0's "MOT_GREEN_CLASSIZ"). */
void golf_stripped_103B0_strings(Vec *v)
{
    if (fn_1_5D6F4("MOT_LARGE_GREEN") == NULL)
    {
        printf("error : stage_pol\n");
        return;
    }
    printf("Success\n");
    printf("%lf %lf %lf\n", v->x, v->y, v->z);
    printf("false\n");
}

#include "golf2/golf_104B4.inc"

/* Stand-in for a dead-stripped function: only its float literals survive
 * (20.0f, 1.187f at .rodata 0x3E8, between fn_11_104B4's and fn_11_114D8's). */
void golf_stripped_104B4_literals(float *out)
{
    out[0] = 20.0f;
    out[1] = 1.187f;
}

/* Create the golfer apes (the current player's with its character). */
void fn_11_11388(void)
{
    u8 i;

    event_init(2);
    for (i = 0; i < 8; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != 0 && modeCtrl.currPlayer == i)
        {
            g_create_stage_player_wrapper(i, 0, active_monkey_id[i], i, 0, fn_11_1A610);
            balls[i].unk179 = 3;
        }
        else if (g_poolInfo.playerPool.statusList[i] != 0)
        {
            g_create_stage_player_wrapper(i, 0, 0xFF, i, 0, fn_11_1A610);
            balls[i].unk179 = 3;
        }
        else
            balls[i].status = 0;
    }
}

#include "golf2/golf_114D8.inc"
