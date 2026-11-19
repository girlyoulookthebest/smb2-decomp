/* fight_3_14914: functions from 0x14914 (included by src/fight/fight_3.c in address order) */
/* Results screen, player markers above the balls, scoring, glove resets, ball shadows. */

/* Functions in other fight TUs */
void fn_12_5050(struct FightGlove *glove, float arg1);
void fn_12_56B8(struct FightGlove *glove);
void fn_12_17714(void);
void fn_12_17910(int arg0);
void fn_12_1B408(void);  /* fight_4.c */
void fn_12_27654(void);

/* Functions in this fragment called before their definition */
void fn_12_1623C(struct Ball *victim, struct Ball *scorer, int points, int subtract, int index);
void fn_12_1674C(struct Ball *ball);
int fn_12_1755C(struct Ball *attacker, struct Ball *victim, int points);

/* DOL / main_loop */
void event_init(int id);
void event_dest(int id);
void dest_all_events(void);
void load_stage(int stage);
void fade_screen_to_color(s32 type, u32 color, int duration);
void fn_1_56C18(void);
void fn_1_E4708(s8 *charas, s8 *ranks);
int fn_1_E5DC8(void);
void fn_1_E6760(Mtx mtx);
void fn_1_E75E0(void);
void g_fade_track_volume(int a, int b);
void g_init_rendefc_for_stage(void);
void g_load_stage_for_menu_bg(s8 a, int b);
void g_return_to_sel_ngc(int a);
void g_stores_doubles2(float r, float g, float b, float a);
int spawn_stobj(struct Stobj *template);
void call_SoundReqID_arg_0(int soundId);
void preload_ape(int chara, int lod, int mesh, int scene);
void fn_1_D2AC4(struct NlModel *model, u32 mask, u32 value);
extern s32 active_monkey_id[];
extern struct GMA *minigameGma;
extern u32 g_smth_with_sound;
extern u32 g_player_id_for_sound;
extern struct NlObj *g_init_common_gma_buf;
extern Vec lbl_1_data_2E418[];  /* ball colours */

/* player and score, sorted by score */
struct FightRankSort
{
    s32 score;
    s32 player;
};

/* State 0xE: set up the results screen (stage, lights, podium camera, ranks) */
void fn_12_14914(void)
{
    s8 charas[4];
    s8 ranks[4];
    struct FightRankSort tmp;
    Vec dir;
    struct FightRankSort sorted[4];
    struct FightRoundStage *stage;
    int p;
    struct FightPlayer *player;
    int i;
    struct FightRankSort *entry;
    int j;
    int rank;
    int prevScore;
    int draw;
    s8 lowest;

    lbl_12_bss_1ACA0.nextState = 15;
    modeCtrl.submodeTimer = 216000;
    lbl_12_bss_1ACA0.flags |= 0x100;
    fade_screen_to_color(0x100, 0, 30);
    dest_all_events();
    fn_1_56C18();
    stage = &lbl_12_data_668[lbl_12_bss_1C410[(u32)lbl_12_bss_1ACA0.round % 3]];
    load_stage(stage->stageId);
    event_init(0);
    event_init(4);
    event_init(5);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    event_init(9);
    event_init(0xB);
    event_init(0xC);
    light_init(current_stage_id);
    g_init_rendefc_for_stage();
    fn_12_17910(stage->unk0);
    fn_12_17714();

    player = lbl_12_bss_1ACA0.players;
    entry = sorted;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, player++, entry++)
    {
        entry->score = player->wins;
        entry->player = i;
    }
    for (i = 0; i < g_poolInfo.playerPool.count - 1; i++)
    {
        entry = sorted;
        for (j = 0; j < g_poolInfo.playerPool.count - 1 - i; j++, entry++)
        {
            if (entry[1].score > entry[0].score)
            {
                tmp = entry[1];
                entry[1] = entry[0];
                entry[0] = tmp;
            }
        }
    }
    lowest = sorted[g_poolInfo.playerPool.count - 1].score;
    if (lowest == sorted[0].score)
        draw = TRUE;  /* never read */
    enable_cameras_for_active_players();
    camera_set_state(0x46);

    for (i = 0; i < 4; i++)
        charas[i] = -1;
    entry = sorted;
    rank = 0;
    prevScore = entry->score;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, entry++)
    {
        p = entry->player;

        player = &lbl_12_bss_1ACA0.players[p];
        if (g_poolInfo.playerPool.statusList[p] == 0)
            charas[p] = -1;
        else
        {
            charas[p] = active_monkey_id[p];
            if (player->flags & 1)
                charas[p] |= 0x40;
            if (prevScore != entry->score)
            {
                rank++;
                prevScore = entry->score;
            }
            if (rank == 0)
                ranks[p] = 0;
            else
                ranks[p] = 3;
        }
    }
    fn_1_E4708(charas, ranks);
    fn_1_E75E0();
    lbl_12_bss_1F644 |= 1;

    if (current_stage_id == 0x8F || current_stage_id == 0x90)
    {
        struct Light *light = alloc_pool_light(1, 0);

        if (light != NULL)
        {
            if (current_stage_id == 0x8F)
            {
                light->rotX = 0x1080;
                light->rotY = 0x5C00;
            }
            else if (current_stage_id == 0x90)
            {
                light->rotX = 0x1F00;
                light->rotY = 0x4500;
                set_bg_ambient(0.6f, 0.65f, 0.8f);
            }
        }
    }
    switch (world_theme.bgId)
    {
    case 2:
        mtxa_from_translate_xyz(-3.75f, 0.15f, -0.5f);
        mtxa_rotate_y(DEGREES_TO_S16(90));
        break;
    case 6:
        mtxa_from_translate_xyz(4.25f, 0.15f, 0.5f);
#ifdef __MWERKS__
        mtxa_rotate_y(DEGREES_TO_S16(270));  /* MWCC passes this as the int 0xC000 */
#else
        mtxa_rotate_y(-0x4000);  /* (s16)49152.0f is out of range: GCC would saturate it */
#endif
        break;
    default:
        mtxa_from_rotate_y(lbl_1_bss_2F48.infLightRotY);
        mtxa_rotate_x(lbl_1_bss_2F48.infLightRotX);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
        mtxa_from_rotate_y((s16)(math_atan2(dir.x, dir.z) - 0xA000));
        mtxa_translate_xyz(0.0f, 1.2f, -1.0f);
        break;
    }
    fn_1_E6760(mtxa->mtxA);
    fn_12_27654();
    g_load_stage_for_menu_bg(1, 1);
}

/* State 0xF: results screen; leave to the menu when the timer runs out (A skips) */
void fn_12_14E28(void)
{
    if ((g_some_other_flags & 0xA) == 0)
    {
        modeCtrl.submodeTimer--;
        if (modeCtrl.submodeTimer > 60 && (merged_digital_inputs[2] & PAD_BUTTON_A) && fn_1_E5DC8())
            modeCtrl.submodeTimer = 60;
        if (modeCtrl.submodeTimer == 60)
        {
            fade_screen_to_color(0x101, 0xFFFFFF, 60);
            g_fade_track_volume(60, 2);
        }
        if (modeCtrl.submodeTimer < 0)
        {
            g_return_to_sel_ngc(modeCtrl.gameType);
            lbl_12_bss_1F644 &= ~1;
            lbl_12_bss_1ACA0.flags &= ~0x100;
        }
    }
}

/* State 0x16: nothing */
void fn_12_14F48(void)
{
}

/* Whether a ball's ape stops waiting (CPU: random, more likely over time; human: any input) */
int fn_12_14F4C(struct Ball *ball)
{
    int ret;
    struct FightPlayer *player;

    if (ball->unk80 > 180)
        return 1;
    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    ret = 0;
    if (player->flags & 1)
    {
        if (rand() / 32767.0f < ball->unk80 / 720.0f)
            ret = 1;
    }
    else
    {
        struct ControllerInfo *pad = &pad_status_groups[player_pad_map[ball->playerId]];

        if ((pad->unk0[2].button & (PAD_BUTTON_A | PAD_BUTTON_B)) || pad->unk0[0].triggerRight > 20
         || math_sqrt(mathutil_sum_of_sq_2(pad->unk0[0].substickX, pad->unk0[0].substickY)))
            ret = 1;
    }
    return ret;
}

/* Make fn_12_14F4C return 1 */
void fn_12_150E0(struct Ball *ball)
{
    ball->unk80 = 180;
}

/* Load the player marker models and reset the markers */
void fn_12_150EC(void)
{
    struct GMAModel **model;
    struct FightPlayer *player;
    u16 *modelId;
    int i;
    struct FightMarker *marker;
    s8 *status;

    lbl_12_bss_1C414 = minigameGma->modelEntries[88].model;
    modelId = lbl_12_data_6C4;
    model = lbl_12_bss_1C418;
    player = lbl_12_bss_1ACA0.players;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, status++, model++, modelId++, player++)
    {
        if (*status != 0)
        {
            if (player->flags & 1)
                *model = init_common_gma->modelEntries[9].model;
            else
                *model = init_common_gma->modelEntries[*modelId].model;
        }
    }
    marker = lbl_12_bss_1C428;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, status++, marker++)
    {
        if (*status != 0)
        {
            marker->state = 0;
            marker->scale = 0.0f;
            marker->crownKicked = 0;
            marker->crownScale = 0.0f;
            marker->crownTilt = 0.0f;
            marker->crownTiltVel = 0.0f;
            marker->crownLift = 0.0f;
            memset(marker->rotZ, 0, sizeof(marker->rotZ));
            marker->rotY = 0.0f;
            marker->rotYVel = 0.0f;
        }
    }
}

/* Animate the player markers and the leaders' crowns */
void fn_12_152C8(void)
{
    float crownTarget;
    int i;
    struct FightMarker *marker;
    s8 *status;
    int noLeader;
    struct FightGlove *glove;
    struct Ball *ball;
    int leaders;

    leaders = lbl_12_bss_1ACA0.rankCounts[0];
    if (leaders < modeCtrl.playerCount)
    {
        noLeader = FALSE;
        crownTarget = 1.0f - (float)(leaders - 1) / (float)modeCtrl.playerCount;
    }
    else
        noLeader = TRUE;
    ball = balls;
    glove = lbl_12_bss_1ACA0.gloves;
    marker = lbl_12_bss_1C428;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, status++, marker++, glove++, ball++)
    {
        if (*status == 0)
            continue;
        switch (marker->state)
        {
        case 1:
            marker->state = 2;
            marker->targetScale = 1.0f;
            if (marker->scale <= 0.0f)
            {
                marker->scale = 0.0f;
                memset(marker->rotZ, 0, sizeof(marker->rotZ));
            }
            break;
        case 3:
            marker->state = 2;
            marker->targetScale = -0.05f;
            break;
        }
        if (!(ball->flags & 0x10))
            marker->scale += 0.2f * (marker->targetScale - marker->scale);
        else
            marker->scale += 0.2f * -marker->scale;
        if (lbl_12_bss_1ACA0.players[i].rank == 0 && !noLeader)
        {
            if (marker->crownKicked == 0)
            {
                float kick;

                marker->crownKicked = 1;
                kick = 4096.0f + 4096.0f * (rand() / 32767.0f);
                if (frame_counter & 1)
                    kick *= -1.0f;
                marker->crownTiltVel += kick;
            }
            marker->crownScale += 0.15f * (crownTarget - marker->crownScale);
            marker->crownLift += 0.1f * (1.0f - marker->crownLift);
        }
        else
        {
            marker->crownKicked = 0;
            marker->crownScale += 0.2f * (-0.1f - marker->crownScale);
            marker->crownLift += -0.1f * marker->crownLift;
        }
        marker->crownTiltVel *= 0.9f;
        marker->crownTiltVel += -0.1f * marker->crownTilt;
        marker->crownTilt += marker->crownTiltVel;
        marker->rotYVel *= 0.96f;
        if (glove->flags & 4)
            marker->rotYVel += -245.76f;
        else
            marker->rotYVel += -1024.0f * math_sin(marker->rotY);
        marker->rotY += marker->rotYVel;
    }
}

/* Draw the player markers (kept inside the screen, pointing at their ball) and the crowns */
void fn_12_156AC(void)
{
    float scale;
    float invTan;
    float invAspect;
    Vec pos;
    struct Ball *ball;
    s8 *status;
    int i;
    struct GMAModel **model;
    struct FightMarker *marker;
    struct FightPlayer *player;
    int angle;
    float k;
    float k1;
    float k2;
    float sx;
    float sy;
    float m;

    invTan = g_current_camera->sub28.view.invTanHalfFov;
    invAspect = 1.0f / g_current_camera->sub28.view.aspect;
    player = lbl_12_bss_1ACA0.players;
    model = lbl_12_bss_1C418;
    marker = lbl_12_bss_1C428;
    ball = balls;
    status = g_poolInfo.playerPool.statusList;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, ball++, status++, model++, marker++, player++)
    {
        if (*status == 2 && !(ball->flags & 0x10) && !(marker->scale <= 0.0f))
        {
            /* PC HFR: the markers share one mesh per type: pair them per player and camera */
            HFR_TAG(HFR_K_MINI, 0x464D0000 | ((u32)g_something_with_camera3 << 8) | (u32)(ball - balls));
            mtxa_from_mtxb_translate(&ball->pos);
            mtxa_sq_from_identity();
            mtxa_translate_xyz(0.0f, 0.25f + ball->currRadius, 0.0f);
            mathutil_mtxA_get_translate(&pos);
            if (pos.z > -1.0f)
                pos.z = -1.0f;
            {
                k1 = -1.0f / pos.z;
                sx = invAspect * (k1 * (pos.x * invTan));
                sy = k1 * (pos.y * invTan);
                m = (fabs(sx) > fabs(sy)) ? fabs(sx) : fabs(sy);
                if (m > 0.8f)
                {
                    int target;

                    k2 = 0.8f / m;
                    pos.x *= k2;
                    pos.y *= k2;
                    angle = marker->rotZ[g_something_with_camera3];
                    target = math_atan2(pos.y - (0.25f + ball->currRadius) * k2, pos.x) + 0x4000;
                    while (angle < target - 0x8000)
                        angle += 0x10000;
                    while (angle > target + 0x8000)
                        angle -= 0x10000;
                    angle += (target - angle) >> 4;
                    marker->rotZ[g_something_with_camera3] = angle;
                }
                else
                    marker->rotZ[g_something_with_camera3] -= marker->rotZ[g_something_with_camera3] >> 3;
            }
            k = -1.0f / pos.z;
            pos.x *= k;
            pos.y *= k;
            pos.z = -1.0f;
            mathutil_mtxA_set_translate(&pos);
            if (k > 0.1f * invTan)
                k = 0.1f * invTan;
            if (k < 0.05f * invTan)
                k = 0.05f * invTan;
            scale = k * marker->scale;
            mtxa_scale_s(scale);
            mtxa_rotate_z(marker->rotZ[g_something_with_camera3]);
            mtxa_rotate_y(marker->rotY);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            avdisp_set_scale_factor(scale);
            if (player->unk6 != 0 && !(player->unk6 & 8))
            {
                GXColor color = lbl_12_data_A218[ball->colorId];

                g_stores_doubles2((1.0f / 511.0f) * (128.0f + color.r),
                                  (1.0f / 511.0f) * (128.0f + color.g),
                                  (1.0f / 511.0f) * (128.0f + color.b), 0.0f);
                avdisp_draw_model_culled_sort_auto(*model);
                g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
            }
            else if (!(player->flags & 1))
                avdisp_draw_model_culled_sort_auto(*model);
            else
            {
                Vec color = lbl_1_data_2E418[ball->colorId];

                set_post_mult_color(color.x, color.y, color.z, 1.0f);
                avdisp_draw_model_culled_sort_auto(*model);
                fade_color_base_default();
            }
            if (marker->crownScale > 0.0f)
            {
                mtxa_translate_xyz(0.0f, 1.3f + 2.0f * (1.0f - marker->crownLift), 0.0f);
                mathutil_mtxA_get_translate(&pos);
                pos.x *= 1.1f;
                pos.y *= 1.1f;
                pos.z = -1.1f;
                mathutil_mtxA_set_translate(&pos);
                mtxa_sq_from_identity();
                scale *= 1.1f * (0.75f * marker->crownScale);
                mtxa_scale_s(scale);
                mtxa_rotate_z(marker->crownTilt);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                set_post_mult_color(1.0f, 0.9f, 0.5f, 1.0f);
                avdisp_set_scale_factor(scale);
                avdisp_draw_model_culled_sort_auto(lbl_12_bss_1C414);
                set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            }
            HFR_UNTAG();
        }
    }
}

/* Show a ball's player marker */
void fn_12_15D00(struct Ball *ball)
{
    struct FightMarker *marker = &lbl_12_bss_1C428[ball->playerId];

    marker->state = 1;
}

/* Hide a ball's player marker */
void fn_12_15D24(struct Ball *ball)
{
    struct FightMarker *marker = &lbl_12_bss_1C428[ball->playerId];

    marker->state = 3;
}

/* Reset the players for a new match (handicaps from the menu settings) */
void fn_12_15D48(void)
{
    int i;
    struct FightPlayer *player;
    u8 *handicap;
    struct FightSelSettings *sel;

    sel = &lbl_1_bss_16DC;
    handicap = sel->unk8;
    player = lbl_12_bss_1ACA0.players;
    for (i = 4; i > 0; i--, player++, handicap++)
    {
        player->score = 0;
        player->wins = 0;
        player->flags &= 1;
        memset(player->hitBy, 0, sizeof(player->hitBy));
        player->stunTimer = 0;
        player->unk14 = 0;
        player->unk1A = 0;
        player->handicap = (s8)*handicap - 2;
        memset(&player->trail, 0, sizeof(player->trail));
    }
}

/* Reset the players for a new round */
void fn_12_15E20(void)
{
    s8 *status;
    int i;
    struct FightPlayer *player;

    status = g_poolInfo.playerPool.statusList;
    player = lbl_12_bss_1ACA0.players;
    for (i = 4; i > 0; i--, player++, status++)
    {
        if (*status != 0)
            player->score = lbl_1_bss_6FAA0;
        else
            player->score = -1;
        player->unk6 = 0;
        memset(player->hitBy, 0, sizeof(player->hitBy));
        player->stunTimer = 0;
        player->unk14 = 0;
        player->unk1A = 0;
        player->unk1C = 0;
        player->flags &= ~0x1E;
    }
}

/* Clear the gloves */
void fn_12_15EFC(void)
{
    memset(lbl_12_bss_1ACA0.gloves, 0, sizeof(lbl_12_bss_1ACA0.gloves));
}

/* Reset the gloves of the balls in play */
void fn_12_15F30(void)
{
    int i;
    struct FightGlove *glove;
    struct Ball *ball;

    ball = balls;
    glove = lbl_12_bss_1ACA0.gloves;
    for (i = 4; i > 0; i--, glove++, ball++)
    {
        if (ball->status != 0)
        {
            glove->flags = 0;
            glove->prevPunch = 0;
            glove->prevCharge = 0;
            fn_12_1674C(ball);
        }
    }
}

/* A ball was knocked out: score it for the players who hit it last (or for all the others) */
void fn_12_15FC8(struct Ball *ball)
{
    int i;
    struct FightPlayer *victim;
    struct FightPlayer *other;
    int points;
    int total;
    s16 *lastHit;
    s8 *status;
    int index;
    struct Ball *scorer;
    int noAttacker;

    index = 0;
    total = 0;
    if (lbl_12_bss_1ACA0.flags & 4)
        return;
    victim = &lbl_12_bss_1ACA0.players[ball->playerId];
    if (victim->rank == 0 && lbl_12_bss_1ACA0.rankCounts[0] < modeCtrl.playerCount)
        points = lbl_1_bss_6FAA8;
    else
        points = lbl_1_bss_6FAA6;
    noAttacker = TRUE;
    other = lbl_12_bss_1ACA0.players;
    lastHit = victim->hitBy;
    for (i = 0; i < 4; i++, other++, lastHit++)
    {
        if (other != victim && *lastHit > 0)
        {
            int p = fn_12_1755C(&balls[i], ball, points);

            other->score += p;
            total += p;
            fn_12_1623C(ball, &balls[i], fn_12_1755C(&balls[i], ball, points), lbl_1_bss_6FAAA, index);
            index++;
            noAttacker = FALSE;
        }
    }
    if (noAttacker)
    {
        points = lbl_1_bss_6FAA4;
        status = g_poolInfo.playerPool.statusList;
        other = lbl_12_bss_1ACA0.players;
        scorer = balls;
        for (i = 4; i > 0; i--, other++, status++, scorer++)
        {
            if (*status != 0 && other != victim)
            {
                int p = points;

                other->score += p;
                total += p;
                fn_12_1623C(ball, scorer, points, lbl_1_bss_6FAAA, index);
                index++;
            }
        }
    }
    lbl_12_bss_1ACA0.unk1748 = 1;
    memset(victim->hitBy, 0, sizeof(victim->hitBy));
    if (lbl_1_bss_6FAAA != 0)
    {
        victim->score -= total;
        if (victim->score < 0)
            victim->score = 0;
    }
}

/* Spawn the floating "+points" effect over a knocked-out ball */
void fn_12_1623C(struct Ball *victim, struct Ball *scorer, int points, int subtract, int index)
{
    struct Effect effect;

    if (!(lbl_1_bss_6FA96 & 1))
        return;
    memset(&effect, 0, sizeof(effect));
    effect.type = 0x28;
    effect.playerId = scorer->playerId;
    effect.pos = victim->pos;
    effect.unk18 = lbl_1_data_2E418[victim->colorId].x;
    effect.unk1C = lbl_1_data_2E418[victim->colorId].y;
    effect.unk20 = lbl_1_data_2E418[victim->colorId].z;
    effect.unkA4 = points;
    effect.unkA6 = index;
    if (subtract)
        effect.unk98.x = -1.0f;
    else
        effect.unk98.x = 1.0f;
    spawn_effect(&effect);
}

/* Rank the players by score (round winners first), count the players per rank, pick a leader */
void fn_12_16368(void)
{
    struct FightRankSort tmp;
    struct FightRankSort sorted[4];
    struct FightPlayer *player;
    int i;
    struct FightRankSort *entry;
    int j;
    int rank;
    int prevScore;

    player = lbl_12_bss_1ACA0.players;
    entry = sorted;
    for (i = 0; i < 4; i++, player++, entry++)
    {
        entry->score = player->score;
        entry->player = i;
        if (player->flags & 4)
            entry->score += 100;
    }
    for (i = 0; i < 3; i++)
    {
        entry = sorted;
        for (j = 0; j < 3 - i; j++, entry++)
        {
            if (entry[1].score > entry[0].score)
            {
                tmp = entry[1];
                entry[1] = entry[0];
                entry[0] = tmp;
            }
        }
    }
    memset(lbl_12_bss_1ACA0.rankCounts, 0, sizeof(lbl_12_bss_1ACA0.rankCounts));
    entry = sorted;
    prevScore = entry->score;
    rank = 0;
    player = &lbl_12_bss_1ACA0.players[entry->player];
    player->rank = rank;
    lbl_12_bss_1ACA0.rankCounts[rank]++;
    entry++;
    for (i = 1; i < 4; i++, entry++)
    {
        if (prevScore != entry->score)
        {
            rank = i;
            prevScore = entry->score;
        }
        player = &lbl_12_bss_1ACA0.players[entry->player];
        player->rank = rank;
        lbl_12_bss_1ACA0.rankCounts[rank]++;
    }
    if (lbl_12_bss_1ACA0.rankCounts[0] > 0)
        lbl_12_bss_1ACA0.unk1750 = sorted[(rand() & 0x7FFF) % lbl_12_bss_1ACA0.rankCounts[0]].player;
    if (lbl_12_bss_1ACA0.rankCounts[0] > 1)
        lbl_12_bss_1ACA0.unk174C = 0;
    else
        lbl_12_bss_1ACA0.unk174C = 1;
}

/* Round over: one more win for the leaders; flag the match over once someone has enough */
void fn_12_165A4(void)
{
    int i;
    struct FightPlayer *player;
    s8 *status;

    player = lbl_12_bss_1ACA0.players;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < 4; i++, player++, status++)
    {
        if (*status == 0)
            break;
        if (player->rank == 0)
        {
            player->wins++;
            if (player->wins >= lbl_12_bss_1ACA0.winsNeeded)
                lbl_12_bss_1ACA0.flags |= 1;
        }
    }
}

/* Spawn the glove stobj (type 7) of every ball in play */
void fn_12_16630(void)
{
    struct Stobj stobj;
    struct Ball *ball;
    struct Ball *ballBackup;
    int i;
    struct FightGlove *glove;

    memset(&stobj, 0, sizeof(stobj));
    memset(lbl_12_bss_1ACA0.gloves, 0, sizeof(lbl_12_bss_1ACA0.gloves));
    stobj.type = 7;
    stobj.animGroupId = 0;
    ballBackup = current_ball;
    ball = balls;
    for (i = 0; i < 8; i++, ball++)
    {
        if (ball->status == 2)
        {
            current_ball = ball;
            glove = &lbl_12_bss_1ACA0.gloves[ball->playerId];
            glove->playerIdx = ball->playerId;
            glove->sound38 = -1;
            glove->sound3C = -1;
            glove->sound40 = -1;
            stobj.unk6 = ball->playerId;
            stobj.extraData = glove;
            spawn_stobj(&stobj);
        }
    }
    current_ball = ballBackup;
}

/* Reset a ball's glove: stop its sounds and put the glove stobj on the ball */
void fn_12_1674C(struct Ball *ball)
{
    struct Ball *ballBackup;
    struct FightGlove *glove;
    struct Stobj *stobj;

    ballBackup = current_ball;
    current_ball = ball;
    glove = &lbl_12_bss_1ACA0.gloves[ball->playerId];
    glove->flags |= 2;
    if (glove->sound38 >= 0)
    {
        SoundOff(glove->sound38);
        glove->sound38 = -1;
    }
    if (glove->sound40 > 0)
    {
        SoundOff(glove->sound40);
        glove->sound40 = -1;
    }
    stobj = glove->stobj;
    if (stobj != NULL)
    {
        stobj->u_some_pos = ball->pos;
        stobj->unk7C = stobj->u_some_pos;
        stobj->position = stobj->u_some_pos;
        stobj->position_2 = stobj->u_some_pos;
    }
    current_ball = ballBackup;
}

/* A punch item was picked up: raise the glove's level of that kind */
void fn_12_16880(struct Ball *ball, int itemSubType)
{
    struct FightGlove *glove;

    glove = &lbl_12_bss_1ACA0.gloves[ball->playerId];
    switch (itemSubType)
    {
    case 0:
        if (glove->radLevel < lbl_1_bss_6FA78)
            glove->radLevel++;
        if (glove->unk30 != 0)
        {
            g_smth_with_sound = active_monkey_id[glove->playerIdx];
            g_player_id_for_sound = glove->playerIdx;
            call_SoundReqID_arg_0(0x108);
        }
        glove->unk30 = 15;
        break;
    case 1:
        if (glove->rangeLevel < lbl_1_bss_6FA76)
            glove->rangeLevel++;
        break;
    case 2:
        glove->flags |= 4;
        glove->unk2C = 600;
        break;
    case 3:
        if (glove->powerLevel < lbl_1_bss_6FA74)
        {
            fn_12_56B8(glove);
            glove->powerLevel++;
        }
        break;
    }
}

/* Clear a ball's glove levels */
void fn_12_169DC(struct Ball *ball)
{
    struct FightGlove *glove;

    glove = &lbl_12_bss_1ACA0.gloves[ball->playerId];
    glove->radLevel = 0;
    glove->rangeLevel = 0;
    glove->powerLevel = 0;
    glove->unk58 = 0.0f;
    glove->flags &= ~4;
    glove->unk2C = 0;
}

/* Turn a ball's glove off: stop its sounds, fn_12_5050, flags = 0x40 */
void fn_12_16A3C(struct Ball *ball, float a)
{
    struct Ball *ballBackup;
    struct FightGlove *glove;

    ballBackup = current_ball;
    current_ball = ball;
    glove = &lbl_12_bss_1ACA0.gloves[ball->playerId];
    if (glove->sound38 >= 0)
    {
        SoundOff(glove->sound38);
        glove->sound38 = -1;
    }
    if (glove->sound40 > 0)
    {
        SoundOff(glove->sound40);
        glove->sound40 = -1;
    }
    fn_12_5050(&lbl_12_bss_1ACA0.gloves[ball->playerId], a);
    lbl_12_bss_1ACA0.gloves[ball->playerId].flags = 0x40;
    current_ball = ballBackup;
}

/* Clear the per round rank/score log */
void fn_12_16B44(void)
{
    int n;
    int i;
    struct Fight1C518 *entry;

    memset(lbl_12_bss_1C518, 0, sizeof(lbl_12_bss_1C518));
    entry = lbl_12_bss_1C518;
    for (n = 17; n > 0; n--, entry++)
    {
        for (i = 0; i < 4; i++)
        {
            entry->unk0[i] = -1;
            entry->unk4[i] = -1;
        }
    }
}

/* Log the ranks and scores of this round */
void fn_12_16BC4(void)
{
    int i;
    struct Fight1C518 *entry;
    s8 *status;
    struct FightPlayer *player;

    entry = &lbl_12_bss_1C518[lbl_12_bss_1ACA0.round];
    player = lbl_12_bss_1ACA0.players;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, player++, status++)
    {
        if (*status != 0)
        {
            entry->unk0[i] = player->rank;
            entry->unk4[i] = player->score;
        }
    }
}

/* Blob shadows of the gloves, the item boxes and (world theme 6) the balls */
void fn_12_16C54(void)
{
    Vec tmp;
    Vec pos;
    struct RaycastHit hit;
    struct ShadowReq req;
    int i;
    s8 *status;
    struct Stobj *stobj;
    struct FightGlove *glove;
    struct Ball *ball;
    float t;
    struct FightBox *box;

#ifndef __MWERKS__
    memset(&req, 0, sizeof(req));  /* the box loop reuses color.a from the glove loop */
#endif
    req.model = init_common_gma->modelEntries[103].model;
    req.unk20 = 0.025f;
    req.unk24 = 0.075f;
    glove = lbl_12_bss_1ACA0.gloves;
    ball = balls;
    status = g_poolInfo.playerPool.statusList;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, glove++, ball++)
    {
        stobj = glove->stobj;
        if (stobj != NULL && *status != 0 && !(ball->flags & 0x10) && !(glove->flags & 0x40)
         && (s32)raycast_stage_down(&stobj->position, &hit, &tmp) != 0)
        {
            t = 0.2f * (stobj->u_some_pos.y - hit.pos.y);
            if (t < 1.0f)
            {
                vec_to_euler(&hit.normal, &req.rot);
                req.pos = hit.pos;
                req.scale.x = stobj->boundSphereRadius * (1.25f + t);
                req.scale.y = req.scale.x;
                req.scale.z = req.scale.x;
                t = 1.0f - t;
                req.color = lbl_12_data_6CC[ball->colorId];
                req.color.r = req.color.r * t;
                req.color.g = req.color.g * t;
                req.color.b = req.color.b * t;
                submit_shadow_req(&req);
            }
        }
    }

    req.model = init_common_gma->modelEntries[103].model;
    req.unk20 = 0.025f;
    req.unk24 = 0.075f;
    box = lbl_12_bss_118;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, box++)
    {
        stobj = box->stobj;
        if (stobj != NULL && !(stobj->position.y < 0.0f))
        {
            pos.x = stobj->position.x;
            pos.y = 1.0f;
            pos.z = stobj->position.z;
            if ((s32)raycast_stage_down(&pos, &hit, &tmp) != 0)
            {
                t = 0.1f * (stobj->u_some_pos.y - hit.pos.y);
                if (t < 1.0f)
                {
                    vec_to_euler(&hit.normal, &req.rot);
                    req.pos = hit.pos;
                    req.scale.x = stobj->boundSphereRadius * (1.5f + 3.0f * t);
                    req.scale.y = req.scale.x;
                    req.scale.z = req.scale.x;
                    /* color.a stays what the glove loop left (as in the original) */
                    t = ((float)box->pieceCount / 35.0f) * (box->alpha * (1.0f - t));
                    req.color.r = 144.0f * t;
                    req.color.g = 128.0f * t;
                    req.color.b = 144.0f * t;
                    submit_shadow_req(&req);
                }
            }
        }
    }

    if (world_theme.bgId == 6)
    {
        req.model = minigameGma->modelEntries[169].model;
        req.unk20 = 0.025f;
        req.unk24 = 0.075f;
        ball = balls;
        status = g_poolInfo.playerPool.statusList;
        for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, ball++)
        {
            if (*status != 0 && !(ball->flags & 0x10) && (s32)raycast_stage_down(&ball->pos, &hit, &tmp) != 0)
            {
                t = 0.33333334f * (ball->pos.y - hit.pos.y);
                if (t < 1.0f)
                {
                    vec_to_euler(&hit.normal, &req.rot);
                    req.pos = hit.pos;
                    req.scale.x = balls[0].currRadius * (1.25f + t);
                    req.scale.y = req.scale.x;
                    req.scale.z = req.scale.x;
                    t = 0.25f * (1.0f - t);
                    req.color = lbl_12_data_A218[ball->colorId];
                    req.color.r = (512.0f + req.color.r) * t;
                    req.color.g = (512.0f + req.color.g) * t;
                    req.color.b = (512.0f + req.color.b) * t;
                    submit_shadow_req(&req);
                }
            }
        }
    }
}

/* Set the material flags of the init_common NL model 0 (ball ring) */
void fn_12_17264(void)
{
    fn_1_D2AC4(g_init_common_gma_buf->models[0], 0x1BFFFFFF, 0x24000000);
}

/* Draw the spinning ring around every ball in play (init_common NL model 0, ball colour) */
void fn_12_172A0(void)
{
    Vec pos;
    Vec color;
    int i;
    struct Ball *ball;
    s8 *status;
    struct NlModel *model;
    u32 frame;
    float scale;
    float radius;
    float t;
    float f;
    float g;

    frame = g_font_char_var_3;
    model = g_init_common_gma_buf->models[0];
    ball = balls;
    status = g_poolInfo.playerPool.statusList;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, ball++)
    {
        if (*status != 0 && !(ball->flags & 0x10))
        {
            mtxa_from_mtxb_translate(&ball->pos);
            mtxa_sq_from_identity();
            mathutil_mtxA_get_translate(&pos);
            radius = ball->currRadius;
            if (!(pos.z >= -radius))
            {
                t = (pos.z + radius) / pos.z;
                pos.x *= t;
                pos.y *= t;
                pos.z *= t;
                mathutil_mtxA_set_translate(&pos);
                scale = t;
                mtxa_scale_s(scale);
                mtxa_rotate_z(frame << 7);
                f = 0.5f * (1.0f + math_sin((frame << 11) + (i << 12)));
                g = 1.0f - f;
                color = lbl_1_data_2E418[ball->colorId];
                color.x = f + g * color.x;
                color.y = f + g * color.y;
                color.z = f + g * color.z;
                nl2ngc_set_material_color(color.x, color.y, color.z);
                nl2ngc_set_scale(scale);
                HFR_TAG(HFR_K_MINI, 0x46520000 | (u32)(ball - balls));  /* PC HFR: ring per ball */
                nl2ngc_draw_model_alpha_sort_none(model, 0.75f);
                HFR_UNTAG();
            }
        }
    }
    fade_color_base_default();
}

/* TRUE if every active player is one win from the match (final round) */
s32 fn_12_174E8(void)
{
    struct FightPlayer *player;
    s8 *status;
    int wins;
    int i;

    wins = lbl_12_bss_1ACA0.winsNeeded - 1;
    player = lbl_12_bss_1ACA0.players;
    status = g_poolInfo.playerPool.statusList;
    for (i = g_poolInfo.playerPool.count; i > 0; i--, status++, player++)
    {
        if (*status != 0 && player->wins < wins)
            break;
    }
    return i == 0;
}

/* Points of a hit after the handicaps (2 per handicap step, at least 1) */
int fn_12_1755C(struct Ball *attacker, struct Ball *victim, int points)
{
    struct FightPlayer *a;
    struct FightPlayer *v;

    if (points <= 0)
        return 0;
    a = &lbl_12_bss_1ACA0.players[attacker->playerId];
    v = &lbl_12_bss_1ACA0.players[victim->playerId];
    points += (v->handicap - a->handicap) * 2;
    if (points <= 0)
        points = 1;
    return points;
}

/* Stop the fight's events */
void fn_12_175CC(void)
{
    event_dest(0);
    event_dest(1);
    event_dest(4);
    event_dest(5);
    event_dest(7);
    event_dest(0x10);
    event_dest(0x11);
    event_dest(0x13);
    event_dest(0xE);
    event_dest(0x15);
    event_dest(0x14);
    event_dest(9);
    event_dest(0xB);
}

/* Preload the players' apes */
void fn_12_17650(void)
{
    int i;

    for (i = 0; i < modeCtrl.playerCount; i++)
        preload_ape(active_monkey_id[i], 0, 1, 0x1A);
}

/* Clear the stage gimmick state and its work buffer, then fn_12_1B408 */
void fn_12_176B8(void)
{
    memset(&lbl_12_bss_1EE38, 0, sizeof(lbl_12_bss_1EE38));
    memset(lbl_12_bss_1C5E8, 0, sizeof(lbl_12_bss_1C5E8));
    lbl_12_bss_1EE38.work = lbl_12_bss_1C5E8;
    fn_12_1B408();
}
