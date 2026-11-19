/* fight_3_11300: functions from 0x11300 (included by src/fight/fight_3.c in address order) */

/* Runs code for each camera, with g_current_camera pointing at it. */
#define CAMERA_FOREACH(code) \
    { \
        struct Camera *camera; \
        struct Camera *cameraBackup_; \
        int i_; \
        cameraBackup_ = g_current_camera; \
        camera = cameras; \
        for (i_ = 0; i_ < 5; i_++, camera++) \
        { \
            g_current_camera = camera; \
            { code } \
        } \
        g_current_camera = cameraBackup_; \
    }

/* Runs code for each active player's world tilt state. */
#define WORLD_FOREACH(code) \
    { \
        struct World *world; \
        s8 *status_; \
        int i_; \
        world = world_infos; \
        status_ = g_poolInfo.playerPool.statusList; \
        for (i_ = 0; i_ < g_poolInfo.playerPool.count; i_++, world++, status_++) \
        { \
            if (*status_ == 2) \
            { \
                code \
            } \
        } \
    }

/* defined below */
void fn_12_11C0C(void);
void fn_12_11E74(void);
void fn_12_13234(void);

/* defined in fight_3_14914.inc */
int fn_12_14F4C(struct Ball *ball);
void fn_12_150E0(struct Ball *ball);
void fn_12_150EC(void);
void fn_12_152C8(void);
void fn_12_156AC(void);
void fn_12_15D00(struct Ball *ball);
void fn_12_15D24(struct Ball *ball);
void fn_12_15D48(void);
void fn_12_15E20(void);
void fn_12_15EFC(void);
void fn_12_15F30(void);
void fn_12_15FC8(struct Ball *ball);
void fn_12_16368(void);
void fn_12_165A4(void);
void fn_12_16630(void);
void fn_12_169DC(struct Ball *ball);
void fn_12_16A3C(struct Ball *ball, float a);
void fn_12_16B44(void);
void fn_12_16BC4(void);
void fn_12_16C54(void);
void fn_12_17264(void);
void fn_12_172A0(void);
s32 fn_12_174E8(void);
void fn_12_175CC(void);
void fn_12_17650(void);
void fn_12_176B8(void);

/* main_loop pause.c struct PauseMenuState (only its flags are used here) */
extern struct { s32 timer; u32 flags; } g_related_to_pause_menu_input;

/* defined in other fight TUs */
extern u32 lbl_12_bss_1F644;  /* fight_9.c */
void fn_12_14C(void);
void fn_12_18C(void);
void fn_12_5050(struct FightGlove *glove, float a);
void fn_12_640C(void);
void fn_12_8ED0(void);
void fn_12_8FB0(void);
void fn_12_17714(void);
void fn_12_17844(void);
void fn_12_17910(int type);
void fn_12_1D21C(void);
void fn_12_1D2A0(void);
void fn_12_1D378(void);
void fn_12_1F744(void);
void fn_12_20518(void);
void fn_12_2301C(void);
void fn_12_23048(void);
void fn_12_23858(void);
void fn_12_24050(int playerId);
void fn_12_25FA8(void);
void fn_12_26798(void);
void fn_12_26B04(void);
void fn_12_27168(int delay);
void fn_12_29238(void);
void fn_12_292B0(int a, int b, float *c, float *d);
void fn_12_2939C(int a);
void fn_12_29F68(void);
void fn_12_2A0CC(void);
void fn_12_2ABC8(void);
void fn_12_2ABCC(void);
void fn_12_2B350(int frames);
void fn_12_2B374(int frames);
void fn_12_2B398(int frames);
void fn_12_2B3BC(void);
void fn_12_2B428(void);
void fn_12_2B688(void);
void fn_12_2BF54(void);

/* Mode init: debug parameter defaults, saves the player setup, loads assets, fills empty slots
   with CPU players and runs every module's init. */
void fn_12_11300(void)
{
    int i;
    s8 *status;
    struct FightPlayer *player;

    lbl_1_bss_6FAE6 = 0xA800;
    lbl_1_bss_6FAE4 = 0x80;
    lbl_1_bss_6FAE0 = 1.0f;
    lbl_1_bss_6FADC = 60;
    lbl_1_bss_6FAD8 = 0;
    lbl_1_bss_6FAD4 = 0;
    lbl_1_bss_6FAD0 = 90;
    lbl_1_bss_6FACC = 0.2f;
    lbl_1_bss_6FAC8 = 1.3f;
    lbl_1_bss_6FAC4 = 4.0f;
    lbl_1_bss_6FAC0 = 0.0f;
    lbl_1_bss_6FABC = 0.45f;
    lbl_1_bss_6FAB8 = 1.0f;
    lbl_1_bss_6FAB4 = 2.0f;
    lbl_1_bss_6FAB0 = 0.5f;
    lbl_1_bss_6FAAC = 0.8f;
    lbl_1_bss_6FAAA = 0;
    lbl_1_bss_6FAA8 = 20;
    lbl_1_bss_6FAA6 = 10;
    lbl_1_bss_6FAA4 = 5;
    lbl_1_bss_6FAA2 = 1;
    lbl_1_bss_6FAA0 = 0;
    lbl_1_bss_6FA9C = 1.5f;
    lbl_1_bss_6FA98 = 2.0f;
    lbl_1_bss_6FA96 = 1;
    lbl_1_bss_6FA94 = 1;
    lbl_1_bss_6FA90 = 2.45f;
    lbl_1_bss_6FA8C = 6;
    lbl_1_bss_6FA88 = -0.15f;
    lbl_1_bss_6FA84 = 0.1f;
    lbl_1_bss_6FA80 = 1.0f;
    lbl_1_bss_6FA78 = 1;
    lbl_1_bss_6FA76 = 1;
    lbl_1_bss_6FA74 = 1;
    if (lbl_1_bss_6FA7A <= 0 || lbl_1_bss_6FA7A > 4)
        lbl_1_bss_6FA7A = 4;

    memset(&lbl_12_bss_1ACA0, 0, sizeof(lbl_12_bss_1ACA0));
    lbl_12_bss_1ACA0.nextState = 0;
    modeCtrl.gameType = 4;
    modeCtrl.currPlayer = 0;
    g_something_with_iteratively_freeing_memory();
    load_bmp_by_id_child(5);
    load_bmp_by_id_child(0x13);
    {
        int heap = OSSetCurrentHeap(stage_heap);
        g_load_minigame_gma_tpl(0xF);
        OSSetCurrentHeap(heap);
    }
    SoundGroupLoad(0xB);
    fn_1_358E8();
    g_SoftStreamStart_with_some_defaults(0xE);
    g_SoftStreamStart_with_some_defaults(0x1B);
    dest_all_events();

    lbl_12_bss_1C404 = modeCtrl.playerCount;
    {
    int j;
    for (j = 0; j < 4; j++)
    {
        lbl_12_bss_1C408[j] = g_poolInfo.playerPool.statusList[j];
        lbl_12_bss_1C40C[j] = active_monkey_id[j];
    }
    }

    if ((lbl_1_bss_16DC.flags & 1) && modeCtrl.playerCount < 4)
    {
        u8 used[4];

        memset(used, 0, sizeof(used));
        modeCtrl.playerCount += lbl_1_bss_6FA7A;
        if (modeCtrl.playerCount > 4)
            modeCtrl.playerCount = 4;
        status = g_poolInfo.playerPool.statusList;
        player = lbl_12_bss_1ACA0.players;
        for (i = 0; i < modeCtrl.playerCount; i++, player++, status++)
        {
            if (*status == 0)
            {
                *status = 2;
                player_pad_map[i] = player_pad_map[0];
                player->flags |= 1;
            }
        }
        status = g_poolInfo.playerPool.statusList;
        player = lbl_12_bss_1ACA0.players;
        for (i = 0; i < modeCtrl.playerCount; i++, player++, status++)
        {
            if (*status != 0 && (player->flags & 1))
            {
                int monkey = (rand() & 0x7FFF) % 4;
                int n;

                for (n = 4; n > 0; n--)
                {
                    if (used[monkey] == 0)
                        break;
                    monkey++;
                    if (monkey >= 4)
                        monkey = 3;
                }
                active_monkey_id[i] = monkey;
                used[monkey]++;
            }
        }
    }
    status = g_poolInfo.playerPool.statusList;
    player = lbl_12_bss_1ACA0.players;
    for (i = 0; i < 4; i++, player++, status++)
    {
        if (*status != 0)
            player->chara = active_monkey_id[i] % 5;
    }
    fn_12_176B8();
    fn_12_1D21C();
    fn_12_8ED0();
    fn_12_14C();
    fn_12_20518();
    fn_12_2301C();
    fn_12_29F68();
    fn_12_2B3BC();
    fn_12_17264();
}

/* Mode exit: module teardown, frees assets, restores the player setup. */
void fn_12_11910(void)
{
    int i;

    fn_12_17844();
    fn_12_1D378();
    fn_12_8FB0();
    fn_12_18C();
    fn_12_23048();
    fn_12_2ABC8();
    fn_12_2B688();
    if (lbl_12_bss_1F644 & 1)
        lbl_12_bss_1F644 &= ~1;
    g_something_calls_sndPopGroup();
    {
        int heap = OSSetCurrentHeap(stage_heap);
        free_minigame_gma_tpl();
        OSSetCurrentHeap(heap);
    }
    free_bmp_by_id(5);
    free_bmp_by_id(0x13);
    modeCtrl.playerCount = lbl_12_bss_1C404;
    for (i = 0; i < 4; i++)
    {
        g_poolInfo.playerPool.statusList[i] = lbl_12_bss_1C408[i];
        active_monkey_id[i] = lbl_12_bss_1C40C[i];
    }
    dest_all_events();
    gp_wait_wrapper();
}

/* Per-frame main: runs the current state, then the per-frame modules. */
void fn_12_11A28(void)
{
    lbl_12_bss_1ACA0.state = lbl_12_bss_1ACA0.nextState;
    lbl_12_data_608[lbl_12_bss_1ACA0.state]();
    modeCtrl.currPlayer = 0;
    if (lbl_1_bss_16DC.flags & 1)
        fn_12_2B428();
    if (!(g_some_other_flags & 0xA))
        fn_12_640C();
    fn_12_152C8();
    if (!(g_some_other_flags & 0xA))
        fn_12_2A0CC();
    if (!(g_some_other_flags & 0xA))
        fn_12_1F744();
    if (lbl_12_bss_1ACA0.flags & 2)
    {
        call_SoundReqID_arg_0(0x67);
        lbl_12_bss_1ACA0.flags &= ~2;
    }
    if (dip_switches & 1)
    {
        int i;
        struct FightPlayer *player = lbl_12_bss_1ACA0.players;

        for (i = 0; i < 4; i++, player++)
        {
            u16 *button = &pad_status_groups[i].unk0[2].button;

            if (*button & PAD_BUTTON_LEFT)
                player->handicap--;
            if (*button & PAD_BUTTON_RIGHT)
                player->handicap++;
        }
    }
}

/* Draw. */
void fn_12_11BC4(void)
{
    fn_12_16C54();
    if (lbl_12_bss_1F644 & 1)
        fn_12_11E74();
    else
        fn_12_11C0C();
    fn_12_2ABCC();
}

/* Draws the world once per active player viewport. */
void fn_12_11C0C(void)
{
    int i;
    struct Ball *ballBackup = current_ball;

    for (i = 0; i < 5; i++)
    {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f)
        {
            s8 status = g_poolInfo.playerPool.statusList[i];

            if (status != 0 && status != 4)
            {
                current_ball = &balls[i];
                camera_apply_viewport_if_changed(i);
                fn_1_D4870();
                g_maybe_some_bg_effect_handler();
                if (events[EVENT_REND_EFC].state == 2)
                    g_something_with_rend_efc(4);
                g_draw_func_init();
                status = events[EVENT_STAGE].state;
                if (status == 2 || status == 4)
                    g_draw_stage();
                if (events[EVENT_REND_EFC].state == 2)
                    g_something_with_rend_efc(0x10);
                draw_shadow_reqs();
                if (events[EVENT_BACKGROUND].state == 2)
                {
                    g_smth_with_rendefc_reflective_height(400.0f);
                    g_draw_bg();
                    g_smth_with_rendefc_reflective_height(0.0f);
                }
                if (events[EVENT_ITEM].state == 2)
                    draw_items();
                if (events[EVENT_STOBJ].state == 2)
                    g_draw_stobjs();
                if (events[EVENT_EFFECT].state == 2)
                    g_draw_effects();
                if (events[EVENT_BALL].state == 2)
                    g_draw_ball_and_ape();
                fn_12_156AC();
                fn_12_172A0();
                ord_tbl_draw_nodes();
                if (world_theme.unk8 & 1)
                    fn_1_D6D84(i);
                if (events[EVENT_REND_EFC].state == 2)
                    g_something_with_rend_efc(8);
            }
        }
    }
    current_ball = ballBackup;
    camera_setup_default_projection();
}

/* Single full-screen draw. */
void fn_12_11E74(void)
{
    s8 status;

    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(4);
    status = events[EVENT_STAGE].state;
    if (status == 2 || status == 4)
        g_draw_stage();
    draw_shadow_reqs();
    if (events[EVENT_BACKGROUND].state == 2)
    {
        g_smth_with_rendefc_reflective_height(400.0f);
        g_draw_bg();
        g_smth_with_rendefc_reflective_height(0.0f);
    }
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(0x10);
    if (events[EVENT_ITEM].state == 2)
        draw_items();
    if (events[EVENT_STOBJ].state == 2)
        g_draw_stobjs();
    if (events[EVENT_EFFECT].state == 2)
        g_draw_effects();
    if (events[EVENT_BALL].state == 2)
        g_draw_ball_and_ape();
    g_something_with_drawing_apes();
    ord_tbl_draw_nodes();
    if (world_theme.unk8 & 1)
        fn_1_D6D84(0);
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(8);
}

/* State 0x00: copies the menu settings, starts the persistent events. */
void fn_12_11FF8(void)
{
    u32 i;
    struct FightSelSettings *settings = &lbl_1_bss_16DC;

    lbl_12_bss_1ACA0.nextState = 1;
    lbl_12_bss_1ACA0.winsNeeded = settings->unk4;
    lbl_12_bss_1ACA0.unk1760 = settings->flags & 2;
    for (i = 0; i < 3; i++)
        lbl_12_bss_1C410[i] = settings->stageIdx[i];
    dest_all_events();
    event_init(EVENT_CAMERA);
    event_init(EVENT_BALL);
    event_init(EVENT_APE);
    fn_12_1D21C();
    fn_12_1D2A0();
    fn_12_20518();
    enable_cameras_for_active_players();
}

/* State 0x01 */
void fn_12_120B0(void)
{
    lbl_12_bss_1ACA0.nextState = 2;
}

/* State 0x02: match init. */
void fn_12_120C4(void)
{
    lbl_12_bss_1ACA0.nextState = 3;
    lbl_12_bss_1ACA0.round = 0;
    lbl_12_bss_1ACA0.unk1750 = -1;
    lbl_12_bss_1ACA0.flags = 0;
    fn_12_15D48();
    fn_12_15EFC();
    fn_12_16B44();
}

/* State 0x03 */
void fn_12_1212C(void)
{
    lbl_12_bss_1ACA0.nextState = 4;
}

/* State 0x04: round init: loads the round's stage, starts the round events, resets the balls,
   cameras and per-round modules. */
void fn_12_12140(void)
{
    struct FightRoundStage *stage;
    struct Effect effect;
    float colorA[4];
    float colorB[3];

    lbl_12_bss_1ACA0.nextState = 5;
    modeCtrl.submodeTimer = 600;
    modeCtrl.unk18 = 30;
    fn_12_175CC();
    fn_1_56C18();
    stage = &lbl_12_data_668[lbl_12_bss_1C410[lbl_12_bss_1ACA0.round % 3U]];
    load_stage(stage->stageId);
    event_init(EVENT_STAGE);
    event_init(EVENT_WORLD);
    event_init(EVENT_STOBJ);
    event_init(EVENT_ITEM);
    event_init(EVENT_OBJ_COLLISION);
    event_init(EVENT_CAMERA);
    event_init(EVENT_SPRITE);
    event_init(EVENT_SOUND);
    event_init(EVENT_EFFECT);
    event_init(EVENT_REND_EFC);
    event_init(EVENT_BACKGROUND);
    event_init(EVENT_INFO);
    event_init(EVENT_VIBRATION);
    fn_12_1D2A0();
    enable_cameras_for_active_players();
    light_init(current_stage_id);
    fade_screen_to_color(0x100, 0, 30);
    mode_info.flags |= 8;
    g_init_rendefc_for_stage();
    BALL_FOREACH( ball->state = 0x18; )
    BALL_FOREACH( ball->unk154 = 0x24; )
    BALL_FOREACH( ball->ape->flags |= 0x20; )
    memset(&effect, 0, sizeof(effect));
    effect.type = 0x27;
    spawn_effect(&effect);
    effect.type = 0x2C;
    BALL_FOREACH(
        effect.playerId = ball->playerId;
        spawn_effect(&effect);
    )
    fn_12_16630();
    camera_setup_singleplayer_viewport();
    enable_cameras_for_active_players();
    CAMERA_FOREACH( camera->unk1F = 6; )
    fn_12_17910(stage->unk0);
    fn_12_17714();
    fn_12_15E20();
    fn_12_15F30();
    fn_12_23858();
    fn_12_26B04();
    colorA[0] = 255.0f;
    colorA[1] = 255.0f;
    colorA[2] = 255.0f;
    colorA[3] = 255.0f;
    colorB[0] = 128.0f;
    colorB[1] = 128.0f;
    colorB[2] = 255.0f;
    fn_12_29238();
    fn_12_292B0(1, 1, colorA, colorB);
    {
        s16 track = 0x38;  /* sign-extended at the call in the original */
        g_fade_track_volume(track, 0);
    }
    lbl_12_bss_1ACA0.flags |= 0x40;
    stage = &lbl_12_data_668[lbl_12_bss_1C410[(lbl_12_bss_1ACA0.round + 1) % 3U]];
    if (stage->stageId != current_stage_id)
        queue_stage_load(stage->stageId);
}

/* State 0x05: round intro (round announcement), skippable with A after 30 frames. */
void fn_12_12604(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (modeCtrl.submodeTimer > 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.unk18 > 0)
        modeCtrl.unk18--;
    if (modeCtrl.submodeTimer == 585)
        call_SoundReqID_arg_0(fn_12_174E8() ? 0x184 : lbl_12_data_674[lbl_12_bss_1ACA0.round]);
    if (modeCtrl.submodeTimer == 585)
        fn_12_2939C(0);
    if (modeCtrl.submodeTimer == 0 || (modeCtrl.unk18 == 0 && (merged_digital_inputs[2] & PAD_BUTTON_A)))
    {
        if (lbl_12_bss_1ACA0.unk1760 == 0)
            lbl_12_bss_1ACA0.nextState = 6;
        else
            lbl_12_bss_1ACA0.nextState = 0x14;
        if (modeCtrl.submodeTimer > 585)
            call_SoundReqID_arg_0(fn_12_174E8() ? 0x184 : lbl_12_data_674[lbl_12_bss_1ACA0.round]);
        if (modeCtrl.submodeTimer > 585)
            fn_12_2939C(0);
        {
            s16 track = 0xE;
            g_fade_track_volume(track, 0);
        }
    }
}

/* State 0x06: "ready" countdown setup. */
void fn_12_127E8(void)
{
    lbl_12_bss_1ACA0.nextState = 7;
    modeCtrl.submodeTimer = 180;
    fn_12_150EC();
    CAMERA_FOREACH( camera->unk1F = 0; )
    fn_12_16368();
    lbl_12_bss_1ACA0.unk1748 = 0;
    lbl_12_bss_1F644 &= ~4;
    fn_12_25FA8();
    fn_12_2B350(modeCtrl.submodeTimer);
    fn_12_2939C(0);
    WORLD_FOREACH(
        world->state = 4;
        world->xrot = 0;
        world->zrot = 0;
    )
}

/* State 0x07: countdown, starts the balls after 45 frames. */
void fn_12_1292C(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 135.0)
    {
        BALL_FOREACH(
            ball->unk154 = 0x25;
            fn_12_15D00(ball);
        )
    }
    if (modeCtrl.submodeTimer == 30.0)
        fn_12_2B398(modeCtrl.submodeTimer);
    if (modeCtrl.submodeTimer == 0)
        lbl_12_bss_1ACA0.nextState = 8;
}

/* State 0x08: "go!": round starts. */
void fn_12_12AB8(void)
{
    lbl_12_bss_1ACA0.nextState = 9;
    modeCtrl.submodeTimer = 60;
    WORLD_FOREACH( world->state = 1; )
    BALL_FOREACH( ball->flags |= 0x100000000ULL; )
    mode_info.flags &= ~8;
    call_SoundReqID_arg_0(0xD8FC);
    call_SoundReqID_arg_0(0x183);
    lbl_12_bss_1ACA0.flags |= 0x20;
    lbl_12_bss_1ACA0.flags &= ~0x40;
    BALL_FOREACH(
        g_smth_with_sound = ball->ape->charaId;
        g_player_id_for_sound = ball->playerId;
        call_SoundReqID_arg_0(0x1E);
    )
}

/* State 0x09: the fight. Handles per-ball fight states (fly in, ring out), the end of the round. */
void fn_12_12CBC(void)
{
    int done;

    if (g_some_other_flags & 0xA)
        return;
    {
        struct Ball *ball;
        struct Ball *ballBackup = current_ball;
        s8 *status;
        int i;
        struct FightPlayer *player;

        player = lbl_12_bss_1ACA0.players;
        ball = balls;
        status = g_poolInfo.playerPool.statusList;
        for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, player++, status++)
        {
            if (*status != 2)
                continue;
            current_ball = ball;
            switch (ball->unk154)
            {
            case 0x28:
                if (fn_12_14F4C(ball))
                {
                    ball->unk154 = 0x29;
                    world_infos[ball->playerId].xrot = 0;
                    world_infos[ball->playerId].zrot = 0;
                }
                break;
            case 0x2B:
                if (ball->unk130 > 30.0)
                {
                    ball->flags &= ~0x800ULL;
                    fn_1_48384(ball);
                    fn_12_169DC(ball);
                    ball->unk154 = 0x26;
                    world_infos[ball->playerId].state = 4;
                    fn_12_15D00(ball);
                    fn_12_5050(&lbl_12_bss_1ACA0.gloves[ball->playerId], 0.0f);
                }
                break;
            case 0x2C:
                if (mathutil_vec_distance(&ball->pos, &lbl_12_bss_1EE38.flyIn.pos) > 2.0f * lbl_12_bss_1EE38.flyIn.radius)
                    ball->flags |= 0x800ULL;
                if (ball->flags & 0x800ULL)
                {
                    ball->unk154 = 0x2A;
                    fn_1_48384(ball);
                    lbl_12_bss_1ACA0.players[ball->playerId].unk6 = 0;
                    world_infos[ball->playerId].state = 4;
                    fn_12_15FC8(ball);
                    fn_12_15D24(ball);
                    fn_12_24050(ball->playerId);
                }
                break;
            }
            if (ball->flags & 0x1000ULL)
                ball->flags &= ~0x1000ULL;
            if (player->unk1A > 0)
                player->unk1A--;
        }
        current_ball = ballBackup;
    }
    if (lbl_12_bss_1ACA0.unk1748 != 0)
    {
        fn_12_16368();
        lbl_12_bss_1ACA0.unk1748 = 0;
    }
    done = FALSE;
    if (mode_info.flags & 2)
        done = TRUE;
    if ((dip_switches & 1) && (merged_digital_inputs[0] & PAD_BUTTON_X) && (merged_digital_inputs[0] & PAD_BUTTON_Y))
    {
        done = TRUE;
        mode_info.flags |= 8;
    }
    if (done)
    {
        if (lbl_12_bss_1ACA0.rankCounts[0] > 1)
            lbl_12_bss_1ACA0.nextState = 0x10;
        else
            lbl_12_bss_1ACA0.nextState = 0xA;
        lbl_12_bss_1ACA0.flags &= ~0x20;
    }
}

/* State 0x12: start of sudden death. */
void fn_12_1315C(void)
{
    lbl_12_bss_1ACA0.flags &= ~0x40;
    fn_12_2939C(0);
    call_SoundReqID_arg_0(0xD8FC);
    call_SoundReqID_arg_0(0x183);
    BALL_FOREACH( fn_12_150E0(ball); )
    lbl_12_bss_1ACA0.nextState = 0x13;
    fn_12_13234();
}

/* State 0x13: sudden death: the round ends when one player is left in the ring. */
void fn_12_13234(void)
{
    int remaining;
    int winner;
    int done;

    if (g_some_other_flags & 0xA)
        return;
    {
        struct Ball *ball;
        struct Ball *ballBackup = current_ball;
        s8 *status;
        int i;
        struct FightPlayer *player;

        winner = 0;
        remaining = 0;
        player = lbl_12_bss_1ACA0.players;
        ball = balls;
        status = g_poolInfo.playerPool.statusList;
        for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++, player++)
        {
            if (*status != 2)
                continue;
            current_ball = ball;
            if (!(player->flags & 2))
                winner = i;
            switch (ball->unk154)
            {
            case 0x28:
                if (fn_12_14F4C(ball))
                {
                    ball->unk154 = 0x29;
                    world_infos[ball->playerId].xrot = 0;
                    world_infos[ball->playerId].zrot = 0;
                }
                break;
            case 0x2B:
                if (ball->unk130 > 30.0)
                {
                    ball->unk154 = 0x2D;
                    player->flags |= 2;
                    ball->flags &= ~0x800ULL;
                    fn_1_48384(ball);
                    fn_12_169DC(ball);
                    call_SoundReqID_arg_0(0x185);
                    world_infos[ball->playerId].state = 4;
                }
                break;
            case 0x2C:
                if (ball->pos.y > 12.0f + *stagedef->falloutY
                 || mathutil_vec_distance(&ball->pos, &lbl_12_bss_1EE38.flyIn.pos) > 2.0f * lbl_12_bss_1EE38.flyIn.radius)
                    ball->flags |= 0x800ULL;
                if (ball->flags & 0x800ULL)
                {
                    ball->unk154 = 0x2A;
                    fn_1_48384(ball);
                    lbl_12_bss_1ACA0.players[ball->playerId].unk6 = 0;
                    world_infos[ball->playerId].state = 4;
                    fn_12_15FC8(ball);
                    fn_12_15D24(ball);
                    fn_12_24050(ball->playerId);
                }
                break;
            }
            if (ball->flags & 0x1000ULL)
                ball->flags &= ~0x1000ULL;
            if (player->unk1A > 0)
                player->unk1A--;
            if (!(player->flags & 2))
                remaining++;
        }
        current_ball = ballBackup;
        if (remaining == 1)
        {
            player = lbl_12_bss_1ACA0.players;
            status = g_poolInfo.playerPool.statusList;
            for (i = 0; i < g_poolInfo.playerPool.count; i++, status++, player++)
            {
                if (*status == 2 && !(player->flags & 2))
                {
                    winner = i;
                    break;
                }
            }
        }
    }
    if (lbl_12_bss_1ACA0.unk1748 != 0)
    {
        fn_12_16368();
        lbl_12_bss_1ACA0.unk1748 = 0;
    }
    done = FALSE;
    if (remaining <= 1)
    {
        done = TRUE;
        lbl_12_bss_1ACA0.players[winner].flags |= 4;
        fn_12_16368();
    }
    if (done)
    {
        lbl_12_bss_1ACA0.nextState = 0xA;
        lbl_12_bss_1ACA0.flags &= ~4;
    }
}

/* State 0x10: sudden death intro: players not leading (rank > 0) are out. */
void fn_12_13708(void)
{
    float colorA[4];
    float colorB[3];

    lbl_12_bss_1ACA0.nextState = 0x11;
    modeCtrl.submodeTimer = 180;
    lbl_12_bss_1ACA0.flags |= 4;
    lbl_12_bss_1ACA0.flags |= 0x40;
    CAMERA_FOREACH( camera->unk1F = 0; )
    fn_12_2B350(modeCtrl.submodeTimer);
    call_SoundReqID_arg_0(0x19A);
    fn_12_27168(modeCtrl.submodeTimer - 60);
    colorA[0] = 76.0f;
    colorA[1] = 76.0f;
    colorA[2] = 112.0f;
    colorA[3] = 192.0f;
    colorB[0] = 0.0f;
    colorB[1] = 0.0f;
    colorB[2] = 0.0f;
    fn_12_292B0(0, 2, colorA, colorB);
    fn_12_17910(6);
    {
        int i;
        struct Ball *ball;
        s8 *status;
        struct FightPlayer *player;

        player = lbl_12_bss_1ACA0.players;
        ball = balls;
        status = g_poolInfo.playerPool.statusList;
        for (i = g_poolInfo.playerPool.count; i > 0; i--, ball++, status++, player++)
        {
            if (player->rank > 0)
            {
                ball->unk154 = 0x2D;
                player->flags |= 2;
                fn_1_48384(ball);
                fn_12_169DC(ball);
                lbl_12_bss_1ACA0.players[ball->playerId].unk6 = 0;
                world_infos[ball->playerId].state = 4;
                fn_12_15D24(ball);
            }
            else
            {
                ball->unk154 = 0x26;
                ball->flags &= ~0x800ULL;
                fn_1_48384(ball);
                fn_12_169DC(ball);
                world_infos[ball->playerId].state = 4;
                fn_12_15D00(ball);
            }
        }
    }
}

/* State 0x11: sudden death intro countdown. */
void fn_12_13990(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 60.0)
        fn_12_2939C(0);
    if (modeCtrl.submodeTimer == 30.0)
        fn_12_2B398(modeCtrl.submodeTimer);
    if (modeCtrl.submodeTimer == 0)
        lbl_12_bss_1ACA0.nextState = 0x12;
}

/* State 0x14: intro of a sudden-death-only match. */
void fn_12_13AA4(void)
{
    float colorA[4];
    float colorB[3];

    lbl_12_bss_1ACA0.nextState = 0x15;
    modeCtrl.submodeTimer = 180;
    lbl_12_bss_1ACA0.flags |= 4;
    fn_12_150EC();
    lbl_12_bss_1ACA0.flags |= 0x40;
    CAMERA_FOREACH( camera->unk1F = 0; )
    fn_12_16368();
    lbl_12_bss_1ACA0.unk1748 = 0;
    lbl_12_bss_1F644 &= ~4;
    fn_12_2B350(modeCtrl.submodeTimer);
    call_SoundReqID_arg_0(0x19A);
    fn_12_27168(modeCtrl.submodeTimer - 60);
    colorA[0] = 76.0f;
    colorA[1] = 76.0f;
    colorA[2] = 112.0f;
    colorA[3] = 192.0f;
    colorB[0] = 0.0f;
    colorB[1] = 0.0f;
    colorB[2] = 0.0f;
    fn_12_292B0(0, 2, colorA, colorB);
    fn_12_17910(6);
    WORLD_FOREACH(
        world->state = 4;
        world->xrot = 0;
        world->zrot = 0;
    )
}

/* State 0x15: countdown of a sudden-death-only match. */
void fn_12_13C84(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 135.0)
    {
        BALL_FOREACH(
            ball->unk154 = 0x26;
            ball->flags &= ~0x800ULL;
            fn_1_48384(ball);
            fn_12_169DC(ball);
            world_infos[ball->playerId].state = 4;
            fn_12_15D00(ball);
        )
    }
    if (modeCtrl.submodeTimer == 60.0)
        fn_12_2939C(0);
    if (modeCtrl.submodeTimer == 30.0)
        fn_12_2B398(modeCtrl.submodeTimer);
    if (modeCtrl.submodeTimer == 0)
    {
        lbl_12_bss_1ACA0.nextState = 0x12;
        WORLD_FOREACH( world->state = 1; )
        BALL_FOREACH( ball->flags |= 0x100000000ULL; )
        call_SoundReqID_arg_0(0x183);
        BALL_FOREACH(
            g_smth_with_sound = ball->ape->charaId;
            g_player_id_for_sound = ball->playerId;
            call_SoundReqID_arg_0(0x1E);
        )
    }
}

/* State 0x0A: round end: winner celebration / loser animations. */
void fn_12_13FEC(void)
{
    lbl_12_bss_1ACA0.nextState = 0xB;
    modeCtrl.submodeTimer = 480;
    modeCtrl.unk3C = modeCtrl.submodeTimer;
    modeCtrl.courseFlags &= ~0x400;
    lbl_12_bss_1ACA0.flags |= 0x80;
    fn_12_165A4();
    fn_12_16BC4();
    if (lbl_1_bss_16DC.flags & 1)
        fn_12_2BF54();
    modeCtrl.unk18 = 104;
    fn_12_2B398(7);
    lbl_12_bss_1ACA0.flags &= ~0x18;
    BALL_FOREACH( cameras[ball->playerId].unk1F = 2; )
    BALL_FOREACH(
        fn_1_48384(ball);
        lbl_12_bss_1ACA0.players[ball->playerId].unk6 = 0;
        ball->flags |= 0x400ULL;
        if (lbl_12_bss_1ACA0.players[ball->playerId].rank==0)
        {
            ball->unk154 = 0x2F;
            ball->flags &= ~0x1800000010ULL;
            ball->flags |= 0x1100ULL;
            fn_12_15D00(ball);
            if (ball->ape != NULL)
                ball->ape->flags &= ~0x20;
            g_smth_with_sound = ball->ape->charaId;
            g_player_id_for_sound = ball->playerId;
            call_SoundReqID_arg_0(0x2859);
            ball->winStreak = 3;
        }
        else
        {
            if (!(ball->flags & 0x10ULL))
            {
                ball->unk154 = 0x2F;
                ball->flags &= ~0x1800000010ULL;
                if (ball->ape != NULL)
                    ball->ape->flags &= ~0x20;
                fn_12_16A3C(ball, 0.0f);
            }
            ball->flags |= 0x8000ULL;
            fn_12_15D24(ball);
        }
    )
    WORLD_FOREACH(
        world->state = 4;
        world->xrot = 0;
        world->zrot = 0;
        world->unk10.x = 0.0f;
        world->unk10.y = -1.0f;
        world->unk10.z = 0.0f;
    )
    fn_12_26798();
    fn_12_2B374(modeCtrl.unk18 + 19);
    g_fade_track_volume(0, 8);
    if (lbl_12_bss_1ACA0.flags & 1)
    {
        load_commend_stage();
        fn_12_17650();
    }
}

/* State 0x0B: round end countdown ("time up" / results), skippable with A. */
void fn_12_14448(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (!(lbl_12_bss_1ACA0.flags & 8))
    {
        modeCtrl.unk18--;
        if (modeCtrl.unk18 % 19 == 0)
        {
            call_SoundReqID_arg_0(0xFC);
            if (modeCtrl.unk18 != 0)
                fn_12_2B398(19);
        }
        if (modeCtrl.unk18 == 0)
        {
            modeCtrl.unk18 = 19;
            lbl_12_bss_1ACA0.flags |= 8;
            BALL_FOREACH( cameras[ball->playerId].unk1F = 8; )
        }
    }
    else if (!(lbl_12_bss_1ACA0.flags & 0x10))
    {
        modeCtrl.unk18--;
        if (modeCtrl.unk18 == 0)
        {
            lbl_12_bss_1ACA0.flags |= 0x10;
            call_SoundReqID_arg_0(0x1FE);
            fn_1_3061C(0x139, 0x13A);
        }
    }
    BALL_FOREACH(
        if (!(ball->flags & 0x200ULL) && (ball->ape->flags & 0x4000))
        {
            ball->flags &= ~0x500ULL;
            ball->flags |= 0x200ULL;
            modeCtrl.courseFlags |= 0x400;
            g_smth_with_sound = ball->ape->charaId;
            g_player_id_for_sound = ball->playerId;
            call_SoundReqID_arg_0(0x2859);
            call_SoundReqID_arg_0(0x126);
            fn_12_16A3C(ball, 0.1f);
        }
    )
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer > 60 && modeCtrl.submodeTimer < 180 && (modeCtrl.courseFlags & 0x400)
     && (merged_digital_inputs[2] & PAD_BUTTON_A))
        modeCtrl.submodeTimer = 60;
    if (modeCtrl.submodeTimer == 60)
    {
        fade_screen_to_color(0x101, -1, 60);
        if (lbl_12_bss_1ACA0.flags & 1)
            g_fade_track_volume(60, 2);
    }
    if (modeCtrl.submodeTimer < 0)
    {
        lbl_12_bss_1ACA0.nextState = 0xC;
        lbl_12_bss_1ACA0.flags &= ~0x80;
    }
}

/* State 0x0C: next round, or (story mode) leave through the pause menu flag. */
void fn_12_14878(void)
{
    if (!(lbl_12_bss_1ACA0.flags & 1))
    {
        lbl_12_bss_1ACA0.nextState = 4;
        lbl_12_bss_1ACA0.round++;
        g_fade_track_volume(60, 2);
    }
    else
    {
        g_related_to_pause_menu_input.flags |= 2;
        lbl_12_bss_1ACA0.nextState = 0xE;
    }
    g_fade_track_volume(60, 2);
}

/* State 0x0D: idle. */
void fn_12_14910(void)
{
}
