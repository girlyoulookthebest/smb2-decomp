/* golf.c functions; see golf.c */

/* fn_11_900 (.text 0x900-0x9460): Monkey Golf main mode tick, registered by _prolog as the minigame
 * callback. A state machine over lbl_11_bss_30 (GOLF_STATE_*): course setup, per-player tee-off,
 * aiming (club and camera selection), the power/impact shot meter, ball flight, cup-in and score
 * jingles, the end-of-hole scoreboard, replays of the last shot and the results/exit screens. */

/* other golf.c fragments */
void fn_11_D94C(void);
void fn_11_E2A4(void);
float fn_11_F2A8(Vec *pos);
void fn_11_11388(void);

/* golf_1.c / golf_2.c. The u8 returns are as this caller sees them (it truncates the results). */
void fn_11_19F24(struct Ball *ball);
void fn_11_1A868(void);
u8 fn_11_1AA9C(void);
u8 fn_11_1B040(void);
u8 fn_11_1B42C(float *power);
void fn_11_1C08C(void);
void fn_11_1C2FC(void);
void fn_11_1C538(int result);

#define GOLF_PAD(i) pad_status_groups[player_pad_map[i]]
#define GOLF_OTHER_PLAYER ((modeCtrl.currPlayer + 1) % 2)
#define GOLF_SCORE(player, hole) lbl_11_data_29C[player][hole]
#define GOLF_SET_VOICE_PLAYER(player) \
    do { \
        g_player_id_for_sound = (player); \
        g_smth_with_sound = active_monkey_id[g_player_id_for_sound]; \
    } while (0)

/* Re-detect the lie under the ball (debug hole skip). */
#define GOLF_DETECT_LIE(hit) \
    do { \
        raycast_stage_down(&lbl_11_bss_20->pos, &hit, NULL); \
        if (hit.flags & 2) { \
            lbl_11_bss_CA = 3; \
            printf("shot_on_green %d\n", modeCtrl.currPlayer); \
        } else if (hit.flags & 4) { \
            lbl_11_bss_CA = 1; \
            printf("shot_on_rough %d\n", modeCtrl.currPlayer); \
        } else if (hit.flags & 8) { \
            lbl_11_bss_CA = 2; \
            printf("shot_on_bunk %d\n", modeCtrl.currPlayer); \
        } else { \
            lbl_11_bss_CA = 0; \
            printf("shot_on_fairway %d\n", modeCtrl.currPlayer); \
        } \
    } while (0)

#define GOLF_SET_CLUB_TABLE() \
    do { \
        if (lbl_11_bss_4C >= 12 || lbl_11_bss_CA == 3) \
            lbl_11_bss_58 = &lbl_11_data_2FC[12][0]; \
        else \
            lbl_11_bss_58 = &lbl_11_data_2FC[lbl_11_bss_4C][lbl_11_bss_CA]; \
    } while (0)

#define GOLF_DEBUG_SKIP_HOLE(hit) \
    do { \
        int j; \
        for (j = 0; j < modeCtrl.playerCount; j++) \
            lbl_11_data_258[j] = 1; \
        lbl_11_bss_CC = lbl_11_bss_C8; \
        fn_11_D94C(); \
        GOLF_DETECT_LIE(hit); \
        GOLF_SET_CLUB_TABLE(); \
    } while (0)

#define GOLF_FACE_DIR(v, angle) \
    do { \
        (v).x = -math_sin(angle); \
        (v).y = 0.0f; \
        (v).z = -math_sin((angle) + 0x4000); \
        if (lbl_11_bss_20->ape != NULL) \
            g_smth_with_ape_rotation(lbl_11_bss_20->ape, &(v)); \
    } while (0)

#define GOLF_FACE_CUP(v) \
    do { \
        (v).x = lbl_11_bss_38.x - lbl_11_bss_20->pos.x; \
        (v).y = 0.0f; \
        (v).z = lbl_11_bss_38.z - lbl_11_bss_20->pos.z; \
        if (lbl_11_bss_20->ape != NULL) \
            g_smth_with_ape_rotation(lbl_11_bss_20->ape, &(v)); \
    } while (0)

#define GOLF_RESET_AIM() \
    do { \
        lbl_11_bss_B18 = 0; \
        lbl_11_bss_50 = 0; \
        lbl_11_bss_54 = 0; \
        lbl_11_bss_60 = 0; \
    } while (0)

/* Club choice from the distance to the cup */
#define GOLF_PICK_CLUB(dist) \
    do { \
        if (lbl_11_bss_CA == 3) { \
            if (dist > 30.0) \
                lbl_11_bss_4C = 12; \
            else if (dist > 10.0) \
                lbl_11_bss_4C = 13; \
            else \
                lbl_11_bss_4C = 14; \
        } else if (dist > 250.0) \
            lbl_11_bss_4C = 0; \
        else if (dist > 225.0) \
            lbl_11_bss_4C = 1; \
        else if (dist > 210.0) \
            lbl_11_bss_4C = 2; \
        else if (dist > 195.0) \
            lbl_11_bss_4C = 3; \
        else if (dist > 180.0) \
            lbl_11_bss_4C = 4; \
        else if (dist > 165.0) \
            lbl_11_bss_4C = 5; \
        else if (dist > 155.0) \
            lbl_11_bss_4C = 6; \
        else if (dist > 145.0) \
            lbl_11_bss_4C = 7; \
        else if (dist > 135.0) \
            lbl_11_bss_4C = 8; \
        else if (dist > 120.0) \
            lbl_11_bss_4C = 9; \
        else if (dist > 105.0) \
            lbl_11_bss_4C = 10; \
        else \
            lbl_11_bss_4C = 11; \
    } while (0)

/* Simulate the shot with the current club to draw the aim trajectory. */
#define GOLF_PREVIEW_SHOT() \
    do { \
        float power = lbl_11_bss_94 * lbl_11_data_1A0[lbl_11_bss_4C] * lbl_11_data_2EC[lbl_11_bss_CA]; \
        lbl_11_bss_20->vel.x = power * math_sin(lbl_11_data_180[lbl_11_bss_4C] + 0x4000) * -math_sin(lbl_11_bss_20->unk92); \
        lbl_11_bss_20->vel.y = power * math_sin(lbl_11_data_180[lbl_11_bss_4C]); \
        lbl_11_bss_20->vel.z = power * math_sin(lbl_11_data_180[lbl_11_bss_4C] + 0x4000) * -math_sin(lbl_11_bss_20->unk92 + 0x4000); \
        fn_11_19F24(lbl_11_bss_20); \
        lbl_11_bss_20->vel.x = 0.0f; \
        lbl_11_bss_20->vel.y = 0.0f; \
        lbl_11_bss_20->vel.z = 0.0f; \
    } while (0)

#define GOLF_STICK_X(pad) (5.0 * -(pad).unk0[0].stickX)

/* the original inlines the mathutil helpers into this very large function */
#pragma push
#pragma inline_max_total_size(100000)
void fn_11_900(void)
{
    struct PhysicsBall physBall;
    struct RaycastHit hit1;
    struct RaycastHit hit2;
    struct RaycastHit hit3;
    struct RaycastHit hit4;
    struct RaycastHit hit5;
    struct RaycastHit hit6;
    struct RaycastHit hit7;
    /* one scratch vector per use: the original's stack layout */
    Vec dir1;
    Vec dir2;
    Vec dir3;
    Vec dir4;
    Vec dir5;
    Vec dir6;
    Vec dir7;
    Vec dir8;
    Vec dir9;
    Vec dir10;
    Vec dir11;
    Vec dir12;
    Vec dir13;
    Vec spinAxis;
    Vec up;
    Quat spinQuat;
    Vec dir14;
    s16 totals[4];
    s8 charas[4];
    s8 order[4];
    s8 ranks[4];
    float absPower;
    int i;
    Vec *ballPos;
    int n0, n1, n2, n3, n4, n5, n6, n7;  /* counters of loops without calls (volatile registers) */
    float f;
    float dist;

    if (events[EVENT_BALL].state == EV_STATE_RUNNING)
        lbl_11_bss_20 = &balls[modeCtrl.currPlayer];
    else
        lbl_11_bss_20 = NULL;
    lbl_11_bss_24 = &cameras[modeCtrl.currPlayer];
    lbl_11_bss_28 = &cameras[(modeCtrl.currPlayer + 1) % 5];

    if (g_some_other_flags & 0xA)
        return;

    if (lbl_11_bss_8C != lbl_11_bss_4C && lbl_11_bss_30 == GOLF_STATE_AIM)
        call_SoundReqID_arg_0(0x112);
    lbl_11_bss_8C = lbl_11_bss_4C;

    if (lbl_11_bss_20 != NULL && lbl_11_bss_20->pos.x == stagedef->startPos->pos.x
     && lbl_11_bss_20->pos.z == stagedef->startPos->pos.z && lbl_11_bss_90 == 0 && lbl_11_bss_84 == 0)
        lbl_11_bss_80 = 60;
    if (lbl_11_bss_80 > -1)
        lbl_11_bss_80--;

    lbl_11_bss_94 = 4;
    current_ball = &balls[modeCtrl.currPlayer];
    g_current_camera = &cameras[modeCtrl.currPlayer];
    mtxa_from_translate_xyz(0.0f, -500.0f, 0.0f);
    mtxa_to_mtx(lbl_11_bss_98);

    if (dip_switches & DIP_TEST_TEAM)
    {
        if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_TRIGGER_Z)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_BUTTON_B))
        {
            GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = 0;
            lbl_11_bss_20->pos = lbl_11_bss_20->prevPos = lbl_11_bss_38;
            lbl_11_bss_20->pos.x += 2.0f;
            lbl_11_bss_20->pos.y += 3.0f;
            {
                Vec v = {0.0f, 0.01f, 0.0f};
                Vec *src = &v;
                lbl_11_bss_20->vel = *src;
            }
            lbl_11_bss_CA = 3;
        }
        if ((GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_RIGHT))
        {
            GOLF_DEBUG_SKIP_HOLE(hit1);
        }
        if ((GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_LEFT))
        {
            if (lbl_11_bss_C8 == 0)
                lbl_11_bss_C8 = -1;
            else
                lbl_11_bss_C8 -= 2;
            GOLF_DEBUG_SKIP_HOLE(hit2);
        }
        if ((GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A))
        {
            GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8)++;
            if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) > 8)
                GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = 8;
            return;
        }
        if ((GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_B))
        {
            GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8)--;
            return;
        }
    }
    else
    {
        if ((dip_switches & DIP_DEBUG) && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_B)
         && lbl_11_bss_D0 == 1)
        {
            lbl_11_bss_D4 = 1.0f;
            lbl_11_bss_D8 = 1.0f;
            lbl_11_bss_5C = 0.0f;
        }
        if ((dip_switches & DIP_DEBUG) && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_TRIGGER_Z)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_BUTTON_B))
        {
            GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = 0;
            lbl_11_bss_20->pos = lbl_11_bss_20->prevPos = lbl_11_bss_38;
            lbl_11_bss_20->pos.x += 2.0f;
            lbl_11_bss_20->pos.y += 3.0f;
            {
                Vec v = {0.0f, 0.01f, 0.0f};
                Vec *src = &v;
                lbl_11_bss_20->vel = *src;
            }
            lbl_11_bss_CA = 3;
        }
        if ((dip_switches & DIP_DEBUG) && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_R)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_BUTTON_UP))
        {
            dir1.x = lbl_11_bss_38.x - lbl_11_bss_20->pos.x;
            dir1.y = lbl_11_bss_38.y - lbl_11_bss_20->pos.y;
            dir1.z = lbl_11_bss_38.z - lbl_11_bss_20->pos.z;
            vec_normalize_len(&dir1);
            lbl_11_bss_20->vel.x += dir1.x / 10.0f;
            lbl_11_bss_20->vel.y += dir1.y / 10.0f;
            lbl_11_bss_20->vel.z += dir1.z / 10.0f;
        }
        if ((dip_switches & DIP_DEBUG) && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_R)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L))
        {
            if (lbl_11_bss_20->ape != NULL)
                lbl_11_bss_20->unk92 = 0;
            GOLF_FACE_DIR(dir2, lbl_11_bss_20->unk92);
        }
        if ((dip_switches & DIP_DEBUG) && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_RIGHT))
        {
            GOLF_DEBUG_SKIP_HOLE(hit3);
        }
        if ((dip_switches & DIP_DEBUG) && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_LEFT))
        {
            if (lbl_11_bss_C8 == 0)
                lbl_11_bss_C8 = -1;
            else
                lbl_11_bss_C8 -= 2;
            GOLF_DEBUG_SKIP_HOLE(hit4);
        }
        if ((dip_switches & DIP_DEBUG) && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A))
        {
            GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = 8;
        }
        if ((dip_switches & DIP_DEBUG) && (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_TRIGGER_L)
         && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_B))
        {
            lbl_11_bss_C8 = lbl_11_bss_2E;
            lbl_11_bss_30 = GOLF_STATE_HOLE_END;
        }
    }

    if (lbl_11_bss_DC >= 0)
        lbl_11_bss_DC--;

    /* A skips a replay */
    if ((lbl_11_bss_30 == GOLF_STATE_REPLAY_INIT || lbl_11_bss_30 == GOLF_STATE_REPLAY_CAMERA
      || lbl_11_bss_30 == GOLF_STATE_REPLAY_INTRO || lbl_11_bss_30 == GOLF_STATE_REPLAY_SHOT
      || lbl_11_bss_30 == GOLF_STATE_REPLAY_FLIGHT)
     && (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A))
    {
        lbl_11_bss_DE = 1000;
        SoundIcsReq(lbl_11_bss_20->playerId, 0, 0);
        goto replay_next;
    }

    if (lbl_11_bss_E0 > 0)
    {
        /* cup-in jingle and voices */
        fn_11_1C538(fn_11_16FD8());
        lbl_11_bss_E0--;
        lbl_11_bss_E4++;
        if (lbl_11_bss_E4 == 1 && lbl_1_bss_1704.unk0 == 0)
        {
            if (lbl_11_bss_30 == GOLF_STATE_CUP_IN)
            {
                switch (fn_11_16FD8())
                {
                case -10: call_SoundReqID_arg_0(0x1D5); break;
                case -3: call_SoundReqID_arg_0(0x2E7); break;
                case -2: call_SoundReqID_arg_0(0x2E8); break;
                case -1: call_SoundReqID_arg_0(0x1D2); break;
                case 0: call_SoundReqID_arg_0(0x1D9); break;
                case 1: call_SoundReqID_arg_0(0x1D3); break;
                case 2: call_SoundReqID_arg_0(0x3EB); break;
                default: call_SoundReqID_arg_0(0x1DA); break;
                }
            }
            else
                call_SoundReqID_arg_0(0x1D4);
        }
        if (lbl_11_bss_E4 == 1 && lbl_1_bss_1704.unk0 == 1)
        {
            if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) == 1 || lbl_11_bss_E8 == 0
             || (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) > lbl_11_data_23C[lbl_11_bss_C8] - 1
                 && lbl_11_data_23C[lbl_11_bss_C8] == GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8))
             || (lbl_11_bss_E8 == 2
                 && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) == GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8))
             || (lbl_11_bss_E8 == 1 && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) >= lbl_11_data_23C[lbl_11_bss_C8] - 1
                 && lbl_11_data_23C[lbl_11_bss_C8] == GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8)))
            {
                if (lbl_11_bss_30 == GOLF_STATE_CUP_IN)
                {
                    switch (fn_11_16FD8())
                    {
                    case -10: call_SoundReqID_arg_0(0x1D5); break;
                    case -3: call_SoundReqID_arg_0(0x2E7); break;
                    case -2: call_SoundReqID_arg_0(0x2E8); break;
                    case -1: call_SoundReqID_arg_0(0x1D2); break;
                    case 0: call_SoundReqID_arg_0(0x1D9); break;
                    case 1: call_SoundReqID_arg_0(0x1D3); break;
                    case 2: call_SoundReqID_arg_0(0x3EB); break;
                    default: call_SoundReqID_arg_0(0x1DA); break;
                    }
                }
                else
                    call_SoundReqID_arg_0(0x1D4);
            }
            else if (lbl_11_bss_E8 == 1 || lbl_11_bss_E8 == 2)
            {
                if (modeCtrl.currPlayer == 0)
                    call_SoundReqID_arg_0(0x1EA);
                else
                    call_SoundReqID_arg_0(0x1EB);
            }
        }
        if (lbl_11_bss_E4 == 61 && lbl_1_bss_1704.unk0 == 1)
        {
            if (lbl_11_bss_E8 == 1
             && (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) <= lbl_11_data_23C[lbl_11_bss_C8] - 1
                 || lbl_11_data_23C[lbl_11_bss_C8] != GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8))
             && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) != 1)
                call_SoundReqID_arg_0(0x1EF);
            else if (lbl_11_bss_E8 == 2
                  && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) > GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8))
                call_SoundReqID_arg_0(0x25C);
        }
        if (lbl_11_bss_E4 == 10)
        {
            GOLF_SET_VOICE_PLAYER(modeCtrl.currPlayer);
            if (lbl_11_bss_30 == GOLF_STATE_CUP_IN)
            {
                switch (fn_11_16FD8())
                {
                case -10:
                case -3: call_SoundReqID_arg_0(0x130); break;
                case -2:
                case -1: call_SoundReqID_arg_0(0x12F); break;
                case 0: call_SoundReqID_arg_0(0x12E); break;
                case 1:
                case 2:
                default: call_SoundReqID_arg_0(0x12C); break;
                }
            }
            else
                call_SoundReqID_arg_0(0x12C);
        }
        if (lbl_11_bss_E4 == 60)
        {
            GOLF_SET_VOICE_PLAYER(modeCtrl.currPlayer);
            if (lbl_11_bss_30 == GOLF_STATE_CUP_IN)
            {
                switch (fn_11_16FD8())
                {
                case -10:
                case -3: call_SoundReqID_arg_0(0x130); break;
                case -2:
                case -1: call_SoundReqID_arg_0(0x12F); break;
                case 0: call_SoundReqID_arg_0(0x12E); break;
                case 1:
                case 2:
                default: call_SoundReqID_arg_0(0x12C); break;
                }
            }
            else
                call_SoundReqID_arg_0(0x12C);
        }
        if (lbl_11_bss_E4 == 110)
        {
            GOLF_SET_VOICE_PLAYER(modeCtrl.currPlayer);
            if (lbl_11_bss_30 == GOLF_STATE_CUP_IN)
            {
                switch (fn_11_16FD8())
                {
                case -10:
                case -3: call_SoundReqID_arg_0(0x130); break;
                case -2:
                case -1: call_SoundReqID_arg_0(0x12F); break;
                case 0: call_SoundReqID_arg_0(0x12E); break;
                case 1:
                case 2:
                default: call_SoundReqID_arg_0(0x12C); break;
                }
            }
            else
                call_SoundReqID_arg_0(0x12C);
        }
        if (lbl_11_bss_E4 == 20)
        {
            GOLF_SET_VOICE_PLAYER(modeCtrl.currPlayer);
            if (lbl_11_bss_30 == GOLF_STATE_CUP_IN)
            {
                switch (fn_11_16FD8())
                {
                case -10:
                case -3:
                case -2:
                case -1:
                case 0: call_SoundReqID_arg_0(0xC6); break;
                default: call_SoundReqID_arg_0(0xC5); break;
                }
            }
            else
                call_SoundReqID_arg_0(0xC5);
        }
        if (lbl_11_bss_E4 == 1 && lbl_11_bss_30 == GOLF_STATE_CUP_IN)
        {
            switch (fn_11_16FD8())
            {
            case -3: g_SoftStreamStart_with_some_defaults_2(0x59); break;
            case -2: g_SoftStreamStart_with_some_defaults_2(0x59); break;
            }
        }
        if (lbl_11_bss_E4 == 25)
        {
            if (lbl_11_bss_30 == GOLF_STATE_CUP_IN)
            {
                switch (fn_11_16FD8())
                {
                case -10: fn_1_3061C(0x146, 0x147); break;
                case -3:
                case -2: break;
                case -1: fn_1_3061C(0x13E, 0x13F); break;
                case 0: fn_1_3061C(0x14A, 0x14B); break;
                case 1:
                case 2:
                default: fn_1_3061C(0x140, 0x141); break;
                }
            }
            else
            {
                fn_1_3061C(0x142, 0x143);
                GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = lbl_11_data_23C[lbl_11_bss_C8];
            }
        }
        g_rumble_controller(player_pad_map[modeCtrl.currPlayer], 4, -1);
        goto lbl_C;
    }
    else
    {
        lbl_11_bss_E4 = 0;
        /* random ambient bird call */
        if ((rand() & 0x7FFF) % 1200 == 0)
        {
            call_SoundReqID_arg_0(0xC3 | ((((rand() & 0x7FFF) % 126) - 63) & 0x7F) << 18
                                | ((((rand() & 0x7FFF) % 126) - 63) & 0x7F) << 25);
        }

        switch (lbl_11_bss_30)
        {
        case GOLF_STATE_INIT:
            if (lbl_1_bss_1704.unk1 == 0)
            {
                lbl_11_bss_2C = 0;
                lbl_11_bss_2E = 17;
            }
            else if (lbl_1_bss_1704.unk1 == 1)
            {
                lbl_11_bss_2C = 0;
                lbl_11_bss_2E = 8;
            }
            else if (lbl_1_bss_1704.unk1 == 2)
            {
                lbl_11_bss_2C = 9;
                lbl_11_bss_2E = 17;
            }
            lbl_11_bss_30 = GOLF_STATE_TEE_SETUP;
            lbl_11_bss_C8 = lbl_11_bss_CC = lbl_11_bss_2C - 1;
            modeCtrl.gameType = 8;  /* golf */
            fn_11_E2A4();
            fn_11_1A868();
            goto lbl_A;

        case GOLF_STATE_RESULTS:
            fn_11_13BAC();
            g_load_stage_for_menu_bg(5, 1);
            if (lbl_11_bss_45 == 0)
            {
                g_poolInfo.playerPool.statusList[0] = 2;
                g_poolInfo.playerPool.statusList[1] = 0;
                g_poolInfo.playerPool.statusList[2] = 0;
                g_poolInfo.playerPool.statusList[3] = 0;
                modeCtrl.currPlayer = 0;
                event_dest(EVENT_BALL);
                event_dest(EVENT_APE);
                lbl_11_bss_24 = &cameras[modeCtrl.currPlayer];
                lbl_11_bss_28 = &cameras[(modeCtrl.currPlayer + 1) % 5];
                camera_set_state(70);
                if (lbl_1_bss_1704.unk0 == 1)
                {
                    charas[0] = active_monkey_id[0];
                    charas[1] = active_monkey_id[1];
                    charas[2] = charas[3] = -1;
                    if (lbl_11_data_2E4[0] > lbl_11_data_2E4[1])
                    {
                        ranks[0] = 0;
                        ranks[1] = 1;
                    }
                    else if (lbl_11_data_2E4[0] < lbl_11_data_2E4[1])
                    {
                        ranks[0] = 1;
                        ranks[1] = 0;
                    }
                    else
                    {
                        ranks[0] = 0;
                        ranks[1] = 0;
                    }
                }
                else
                {
                    s8 p;
                    s8 h;

                    for (p = 0; p < 4; p++)
                    {
                        totals[p] = 0;
                        lbl_11_bss_EC[p].unk8 = 0;
                        lbl_11_bss_EC[p].unkA = 0;
                        lbl_11_bss_11C[p].unk4 = 0;
                        lbl_11_bss_11C[p].unk6 = 0;
                        order[p] = p;
                        for (h = lbl_11_bss_2C; h <= lbl_11_bss_2E; h++)
                            totals[p] += GOLF_SCORE(p, h);
                        for (h = 0; h < 9; h++)
                        {
                            lbl_11_bss_EC[p].unk8 += GOLF_SCORE(p, h);
                            lbl_11_bss_11C[p].unk4 += GOLF_SCORE(p, h);
                        }
                        for (h = 9; h < 18; h++)
                        {
                            lbl_11_bss_EC[p].unkA += GOLF_SCORE(p, h);
                            lbl_11_bss_11C[p].unk6 += GOLF_SCORE(p, h);
                        }
                    }
                    for (p = 0; p < modeCtrl.playerCount - 1; p++)
                    {
                        for (h = p + 1; h < modeCtrl.playerCount; h++)
                        {
                            if (totals[p] > totals[h])
                            {
                                s16 t = totals[p];
                                u8 t8;
                                totals[p] = totals[h];
                                totals[h] = t;
                                t8 = order[p];
                                order[p] = order[h];
                                order[h] = t8;
                            }
                        }
                    }
                    for (p = 0; p < 4; p++)
                    {
                        lbl_11_bss_EC[p].unk7 = charas[p] = -1;
                        lbl_11_bss_11C[p].unk3 = charas[p] = -1;
                    }
                    for (p = 0; p < modeCtrl.playerCount; p++)
                    {
                        lbl_11_bss_EC[p].unk7 = charas[p] = active_monkey_id[p];
                        lbl_11_bss_11C[p].unk3 = charas[p] = active_monkey_id[p];
                    }
                    for (p = 0; p < modeCtrl.playerCount; p++)
                        ranks[order[p]] = p;
                    for (p = 0; p < modeCtrl.playerCount; p++)
                    {
                        if (totals[p] == totals[p + 1])
                            ranks[order[p + 1]] = ranks[order[p]];
                    }
                }
                if (modeCtrl.playerCount == 1)
                {
                    ranks[0] = fn_1_11E1C0(0x17, lbl_11_bss_EC);
                    if (lbl_1_bss_1704.unk0 != 0 || lbl_1_bss_1704.unk1 != 0)
                    {
                        int h;
                        int par = 0;

                        for (h = lbl_11_bss_2C; h <= lbl_11_bss_2E; h++)
                            par += lbl_11_data_228[h];
                        if (totals[0] <= par)
                            lbl_11_bss_30 = GOLF_STATE_CLEAR_INIT;
                        else
                        {
                            modeCtrl.submodeTimer = 60;
                            lbl_11_bss_45 = 2;
                        }
                    }
                    else if (ranks[0] < 0)
                    {
                        lbl_11_bss_30 = GOLF_STATE_RANKING;
                        lbl_11_bss_46 = 0;
                        modeCtrl.submodeTimer = 0;
                    }
                    else
                    {
                        struct Light *light;

                        fn_1_1118BC();
                        for (n0 = 0; n0 < modeCtrl.playerCount; n0++)
                            balls[n0].ape = NULL;
                        event_init(EVENT_COMMEND);
                        fn_1_E4708(charas, ranks);
                        lbl_11_bss_45 = 1;
                        modeCtrl.submodeTimer = 0;
                        light = alloc_pool_light(1, 0);
                        if (light != NULL)
                        {
                            light->rotX = 0x6580;
                            light->rotY = 0x6000;
                        }
                    }
                }
                else
                {
                    struct Light *light;

                    fn_1_1118BC();
                    for (n1 = 0; n1 < modeCtrl.playerCount; n1++)
                        balls[n1].ape = NULL;
                    event_init(EVENT_COMMEND);
                    fn_1_E4708(charas, ranks);
                    lbl_11_bss_45 = 1;
                    modeCtrl.submodeTimer = 0;
                    light = alloc_pool_light(1, 0);
                    if (light != NULL)
                    {
                        light->rotX = 0x6580;
                        light->rotY = 0x6000;
                    }
                }
            }
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if ((GOLF_PAD(i).unk0[2].button & PAD_BUTTON_A) && fn_1_E5DC8() != 0 && lbl_11_bss_45 == 1)
                {
                    call_SoundReqID_arg_0(0xD1);
                    modeCtrl.submodeTimer = 60;
                    lbl_11_bss_45 = 2;
                }
            }
            if (modeCtrl.submodeTimer != 0)
            {
                if (modeCtrl.submodeTimer == 60)
                {
                    fade_screen_to_color(0x101, 0xFFFFFF, 60);
                    g_fade_track_volume(60, 2);
                }
                if (--modeCtrl.submodeTimer == 0)
                {
                    if (lbl_1_bss_1704.unk0 == 0 && lbl_1_bss_1704.unk1 == 0)
                    {
                        dest_all_events();
                        g_something_with_iteratively_freeing_memory();
                        free_minigame_gma_tpl();
                        g_something_calls_sndPopGroup();
                        event_init(EVENT_SPRITE);
                        event_init(EVENT_SOUND);
                        modeCtrl.submodeTimer = 0;
                        lbl_11_bss_30 = GOLF_STATE_RANKING;
                        lbl_11_bss_46 = 0;
                        fade_screen_to_color(0x100, 0, 60);
                    }
                    else
                        g_return_to_sel_ngc(modeCtrl.gameType);
                }
            }
            break;

        case GOLF_STATE_RANKING:
            if (lbl_11_bss_46 == 0)
            {
                dest_all_events();
                g_something_with_iteratively_freeing_memory();
                free_minigame_gma_tpl();
                g_something_calls_sndPopGroup();
                event_init(EVENT_SOUND);
                event_init(EVENT_SPRITE);
                event_init(EVENT_STAGE);
                event_init(EVENT_BACKGROUND);
                event_init(EVENT_CAMERA);
                lbl_11_bss_1C = 1;
                if (fn_1_11DF64(0x17, (u8 *)lbl_11_bss_EC, modeCtrl.playerCount) != 0)
                    fn_1_11DE54(0x17, 1);
                else
                    fn_1_11DE54(0x17, 0);
                lbl_11_bss_46 = 1;
                modeCtrl.submodeTimer = 0;
            }
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (GOLF_PAD(i).unk0[2].button & PAD_BUTTON_A)
                {
                    if (fn_1_11E3A4() == 0 && lbl_11_bss_46 == 1)
                    {
                        modeCtrl.submodeTimer = 60;
                        lbl_11_bss_46 = 2;
                        printf("entry_end\n");
                    }
                }
            }
            if (modeCtrl.submodeTimer != 0)
            {
                if (modeCtrl.submodeTimer == 60)
                {
                    fade_screen_to_color(0x101, 0xFFFFFF, 60);
                    g_fade_track_volume(60, 2);
                }
                if (--modeCtrl.submodeTimer == 0)
                    g_return_to_sel_ngc(modeCtrl.gameType);
            }
            break;

        case GOLF_STATE_HOLE_END:
            g_maybe_smth_with_music(0x42, 50);
            lbl_11_bss_30 = GOLF_STATE_SCOREBOARD;
            if (lbl_1_bss_1704.unk0 == 1)
            {
                int left;

                /* match play: count the hole for the winner */
                if (GOLF_SCORE(0, lbl_11_bss_C8) < GOLF_SCORE(1, lbl_11_bss_C8))
                    lbl_11_data_2E4[0]++;
                else if (GOLF_SCORE(0, lbl_11_bss_C8) > GOLF_SCORE(1, lbl_11_bss_C8))
                    lbl_11_data_2E4[1]++;
                else
                    lbl_11_data_2E4[4]++;
                if (lbl_1_bss_1704.unk1 == 0)
                    left = 18 - (lbl_11_bss_C8 + 1);
                else if (lbl_1_bss_1704.unk1 == 2)
                    left = 18 - (lbl_11_bss_C8 + 1);
                else
                    left = 9 - (lbl_11_bss_C8 + 1);
                /* the match is decided */
                if (fabs((float)(lbl_11_data_2E4[0] - lbl_11_data_2E4[1])) > left)
                {
                    lbl_11_data_254 = lbl_11_bss_C8;
                    lbl_11_bss_C8 = lbl_11_bss_2E;
                    lbl_11_data_258[0] = 1;
                    lbl_11_data_258[1] = 1;
                }
            }
            lbl_11_bss_13C = 0;
            lbl_11_bss_140 = 0;
            break;

        case GOLF_STATE_SCOREBOARD:
            if (lbl_11_bss_13C == 0)
                fade_screen_to_color(0x101, 0xFFFFFF, 60);
            if (lbl_11_bss_13C == 60)
            {
                lbl_11_bss_24->unk1F = 14;
                fade_screen_to_color(0x100, 0, 60);
                for (n2 = 0; n2 < modeCtrl.playerCount; n2++)
                    balls[n2].unk154 = 0;
            }
            lbl_11_bss_13C++;
            if (lbl_11_bss_13C > 120 && lbl_11_bss_140 == 0)
            {
                for (n3 = 0; n3 < modeCtrl.playerCount; n3++)
                {
                    if (GOLF_PAD(n3).unk0[2].button & PAD_BUTTON_A)
                        lbl_11_bss_140 = 1;
                }
            }
            if (lbl_11_bss_140 != 0)
            {
                if (lbl_11_bss_140 == 1)
                    fade_screen_to_color(0x101, 0xFFFFFF, 60);
                if (lbl_11_bss_140 == 61)
                {
                    fn_11_D94C();
                    fade_screen_to_color(0x100, 0, 60);
                }
                lbl_11_bss_140++;
            }
            if (lbl_11_bss_47 != 0)
            {
                fn_11_D94C();
                fade_screen_to_color(0x100, 0, 30);
            }
            break;

        case GOLF_STATE_CLEAR_INIT:
            lbl_11_bss_30 = GOLF_STATE_CLEAR;
            lbl_11_bss_13C = 0;
            lbl_11_bss_140 = 0;
            break;

        case GOLF_STATE_CLEAR:
            if (lbl_11_bss_13C == 0)
                fade_screen_to_color(0x101, 0xFFFFFF, 60);
            if (lbl_11_bss_13C == 60)
            {
                dest_all_events();
                free_minigame_gma_tpl();
                g_something_with_iteratively_freeing_memory();
                load_stage(lbl_11_data_204[17]);
                event_init(EVENT_STAGE);
                fn_11_11388();
                event_init(EVENT_CAMERA);
                event_init(EVENT_SOUND);
                event_init(EVENT_BACKGROUND);
                event_init(EVENT_SPRITE);
                event_init(EVENT_APE);
                lbl_11_bss_18 = 1;
                g_poolInfo.playerPool.statusList[0] = 2;
                g_poolInfo.playerPool.statusList[1] = 0;
                g_poolInfo.playerPool.statusList[2] = 0;
                g_poolInfo.playerPool.statusList[3] = 0;
                modeCtrl.currPlayer = 0;
                lbl_11_bss_20 = &balls[modeCtrl.currPlayer];
                lbl_11_bss_24 = &cameras[modeCtrl.currPlayer];
                lbl_11_bss_28 = &cameras[(modeCtrl.currPlayer + 1) % 5];
                disable_all_cameras();
                enable_camera(modeCtrl.currPlayer);
                g_load_minigame_gma_tpl(5);
                load_bmp_by_id(0x10);
                camera_set_state(64);
                textdraw_set_font(0x8F);
                g_fade_track_volume(-1, 1);
                fn_1_36140();
                g_SoftStreamStart_with_some_defaults_2(0x44);
                lbl_11_bss_20->accel = 0.0f;
                lbl_11_bss_20->pos.x = stagedef->startPos->pos.x;
                lbl_11_bss_20->pos.y = 1.1759990399999998 + stagedef->startPos->pos.y;
                lbl_11_bss_20->pos.z = stagedef->startPos->pos.z;
                lbl_11_bss_20->prevPos = lbl_11_bss_20->pos;
                lbl_11_bss_20->colorId = lbl_11_bss_20->playerId;
                lbl_11_bss_20->state = 26;
                lbl_11_bss_20->unk154 = 4;
                lbl_11_bss_20->flags = 0;
                lbl_11_bss_20->flags |= 0x4000;
                {
                    Quat q = {0.0f, 0.0f, 0.0f, 1.0f};
                    Quat *src = &q;
                    lbl_11_bss_20->unkA0 = lbl_11_bss_20->unkB0 = *src;
                }
                lbl_11_bss_24->unk1F = 19;
                lbl_11_bss_24->flags &= ~0x10;
                set_camera_view_settings(modeCtrl.currPlayer, 0.0f, 0.0f, 1.0f, 1.0f);
                fade_screen_to_color(0x100, 0, 60);
            }
            lbl_11_bss_13C++;
            if (lbl_11_bss_13C < 240 && lbl_11_bss_13C > 61)
            {
                lbl_11_bss_20->state = 26;
                lbl_11_bss_20->unk154 = 4;
                fn_11_1E794(120);
                set_ui_widescreen_scale_mtx(320);
                fn_11_2F0F8();
                reset_ui_widescreen_scale_mtx();
            }
            else if (lbl_11_bss_13C >= 240)
            {
                lbl_11_bss_20->state = 26;
                lbl_11_bss_20->unk154 = 0;
                fn_11_1E794(120);
                set_ui_widescreen_scale_mtx(320);
                fn_11_2F0F8();
                reset_ui_widescreen_scale_mtx();
            }
            if (lbl_11_bss_13C > 120 && lbl_11_bss_140 == 0)
            {
                for (n4 = 0; n4 < modeCtrl.playerCount; n4++)
                {
                    if (GOLF_PAD(n4).unk0[2].button & PAD_BUTTON_A)
                        lbl_11_bss_140 = 1;
                }
            }
            if (lbl_11_bss_140 != 0)
            {
                if (lbl_11_bss_140 == 1)
                    fade_screen_to_color(0x101, 0xFFFFFF, 60);
                if (lbl_11_bss_140 == 61)
                    g_return_to_sel_ngc(modeCtrl.gameType);
                lbl_11_bss_140++;
            }
            break;

        case GOLF_STATE_TEE_SETUP:
            event_vibration_dest();
            g_maybe_smth_with_music(0x42, 100);
            if (lbl_11_bss_4A != 0)
            {
                call_SoundReqID_arg_0(0xC4);
                lbl_11_bss_4A = 0;
            }
            lbl_11_bss_144 = 0;
            lbl_11_bss_145 = 1;
            lbl_11_bss_154 = lbl_11_bss_20->pos;
            lbl_11_bss_148 = lbl_11_bss_154;
            lbl_11_bss_20->vel.x = lbl_11_bss_20->vel.z = 0.0f;
            lbl_11_bss_160 = 0.0f;
            lbl_11_bss_164 = 0;
            lbl_11_bss_168 = 0.0f;
            lbl_11_bss_16C = 1;
            lbl_11_bss_170.x = lbl_11_bss_170.y = lbl_11_bss_170.z = 0.0f;
            lbl_11_bss_17C = 0;
            lbl_11_bss_90 = 0;
            lbl_11_bss_DE = 0;
            lbl_11_bss_180 = 0.0f;
            lbl_11_bss_184 = 0.0f;
            lbl_11_bss_E8 = 0;
            if ((lbl_11_data_258[0] != 0 || lbl_11_data_258[1] != 0)
             && (lbl_11_data_258[0] == 0 || lbl_11_data_258[1] == 0) && lbl_1_bss_1704.unk0 == 1)
            {
                /* match play, one player has holed out: can the other still win or tie? */
                int s0 = GOLF_SCORE(0, lbl_11_bss_C8);
                int s1 = GOLF_SCORE(1, lbl_11_bss_C8);

                if (modeCtrl.currPlayer == 0 && s0 == s1 - 1)
                    lbl_11_bss_E8 = 2;
                else if (modeCtrl.currPlayer == 0 && s0 < s1 - 1)
                    lbl_11_bss_E8 = 1;
                else if (modeCtrl.currPlayer == 1 && s1 == s0 - 1)
                    lbl_11_bss_E8 = 2;
                else if (modeCtrl.currPlayer == 1 && s1 < s0 - 1)
                    lbl_11_bss_E8 = 1;
                else
                    lbl_11_bss_E8 = 0;
                printf("match_flag1: %d  [%d  %d]\n", lbl_11_bss_E8, s0, s1);
            }
            else
            {
                int s0 = GOLF_SCORE(0, lbl_11_bss_C8);
                int s1 = GOLF_SCORE(1, lbl_11_bss_C8);

                if (modeCtrl.currPlayer == 0 && s0 < s1)
                    lbl_11_bss_E8 = 1;
                else if (modeCtrl.currPlayer == 1 && s1 < s0)
                    lbl_11_bss_E8 = 1;
                else
                    lbl_11_bss_E8 = 0;
                printf("match_flag2: %d\n", lbl_11_bss_E8);
            }
            f = fn_11_F2A8(&lbl_11_bss_20->pos) / lbl_11_bss_94;
            GOLF_PICK_CLUB(f);
            lbl_11_bss_8 = 24;
            lbl_11_bss_188 = 0;
            if (lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14)
                GOLF_PREVIEW_SHOT();
            lbl_11_bss_18C = lbl_11_bss_198[lbl_11_bss_50 - 1];
            lbl_11_bss_AF8 = 180;
            lbl_11_bss_AFC = 180;
            if (lbl_11_bss_C8 == lbl_11_bss_CC)
                lbl_11_bss_B00 = 0;
            else
            {
                g_fade_track_volume(-1, 1);
                fn_1_36140();
                g_SoftStreamStart_with_some_defaults_2(0x39);
                lbl_11_bss_B00 = 900;
            }
            lbl_11_bss_B04 = 0;
            lbl_11_bss_B08 = 0;
            lbl_11_bss_DC = 0;
            lbl_11_bss_E4 = 0;
            lbl_11_bss_30 = GOLF_STATE_TEE_INTRO;
            for (n5 = 0; n5 < modeCtrl.playerCount; n5++)
            {
                if (modeCtrl.currPlayer != n5)
                {
                    if (balls[n5].ape != NULL)
                        balls[n5].ape->flags |= 0x20;
                    balls[n5].colorId = balls[n5].playerId;
                }
                else
                {
                    if (balls[n5].ape != NULL)
                        balls[n5].ape->flags &= ~0x20;
                    balls[n5].colorId = balls[n5].playerId;
                }
            }
            if (lbl_11_bss_C8 == lbl_11_bss_CC)
                goto lbl_A;
            /* fall through */
        case GOLF_STATE_TEE_INTRO:
            lbl_11_bss_5C = 0.0f;
            if (lbl_11_bss_C8 == lbl_11_bss_CC)
            {
                lbl_11_bss_30 = GOLF_STATE_AIM;
                lbl_11_bss_B00 = 0;
                if (lbl_11_bss_20->ape != NULL)
                    fn_1_1132D0(lbl_11_bss_20->ape, 0, 1, 0);
                if (lbl_11_bss_20->ape != NULL)
                    fn_1_113768(lbl_11_bss_20->ape, 0.0f);
                fn_11_1C08C();
                break;
            }
            if (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A)
                lbl_11_bss_B00 = 0;
            if (lbl_11_bss_B00 > 0)
            {
                /* hole flyover */
                for (i = 0; i < 5; i++)
                    set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
                set_camera_view_settings(modeCtrl.currPlayer, 0.0f, 0.0f, 1.0f, 1.0f);
                set_camera_view_settings((modeCtrl.currPlayer + 1) % 5, 0.0f, 0.0f, 0.0001f, 0.0001f);
                lbl_11_bss_24->unk1F = 11;
                lbl_11_bss_20->flags |= 0x10;
                if (lbl_11_bss_20->ape != NULL)
                    lbl_11_bss_20->ape->flags |= 0x20;
                lbl_11_bss_20->unk154 = 0;
                lbl_11_bss_B00 -= 2;
            }
            else
            {
                g_SoftStreamStart_with_some_defaults_2(0x42);
                for (i = 0; i < 5; i++)
                    set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
                set_camera_view_settings(modeCtrl.currPlayer, 0.0f, 0.0f, 1.0f, 1.0f);
                set_camera_view_settings((modeCtrl.currPlayer + 1) % 5, 0.775f, 0.0f, 0.225f, 0.2875f);
                lbl_11_bss_24->unk1F = 2;
                lbl_11_bss_24->unk26 = 0;
                lbl_11_bss_24->flags &= ~0x10;
                disable_all_cameras();
                enable_camera(modeCtrl.currPlayer);
                lbl_11_bss_28->unk1F = 9;
                lbl_11_bss_28->unk26 = 11;
                lbl_11_bss_28->flags |= 0x10;
                lbl_11_bss_20->flags &= ~0x10;
                if (lbl_11_bss_20->ape != NULL)
                    lbl_11_bss_20->ape->flags &= ~0x20;
                if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) == 0)
                    lbl_11_bss_20->unk154 = 1;
                else
                    lbl_11_bss_20->unk154 = 4;
                for (n6 = 0; n6 < modeCtrl.playerCount; n6++)
                {
                    if (modeCtrl.currPlayer != n6)
                    {
                        balls[n6].flags |= 0x10;
                        if (balls[n6].ape != NULL)
                            balls[n6].ape->flags |= 0x20;
                        balls[n6].colorId = balls[n6].playerId;
                    }
                    else
                    {
                        balls[n6].flags &= ~0x10;
                        if (balls[n6].ape != NULL)
                            balls[n6].ape->flags &= ~0x20;
                        balls[n6].colorId = balls[n6].playerId;
                    }
                }
                lbl_11_bss_30 = GOLF_STATE_AIM;
                if (lbl_11_bss_CC != lbl_11_bss_C8)
                {
                    GOLF_SET_VOICE_PLAYER(modeCtrl.currPlayer);
                    call_SoundReqID_arg_0(0x1E);
                }
                lbl_11_bss_CC = lbl_11_bss_C8;
                if (lbl_11_bss_20->ape != NULL)
                {
                    fn_1_1132D0(lbl_11_bss_20->ape, 0, 1, 0);
                    fn_1_113600(lbl_11_bss_20->ape, 0.0f);
                    fn_1_113768(lbl_11_bss_20->ape, 0.0f);
                    fn_11_1C08C();
                }
                goto lbl_A;
            }
            break;

        case GOLF_STATE_AIM:
            if (g_maybe_related_to_music_crossfading(0x39) != -1)
                g_something_with_stopping_music_or_sfx(0x39);
            lbl_11_bss_B0C = mathutil_vec_len(&lbl_11_bss_20->vel);
            if (lbl_11_bss_B0C > 0.0f && lbl_11_bss_88 < 10)
            {
                /* the ball is still rolling */
                lbl_11_bss_49 = 1;
                if (lbl_11_bss_145 != 0)
                    lbl_11_bss_49 = 0;
                lbl_11_bss_20->state = 26;
                lbl_11_bss_20->unk154 = 4;
                lbl_11_bss_148 = lbl_11_bss_20->pos;
                if (lbl_11_bss_B0C != 0.0f)
                {
                    if (lbl_11_bss_B0C < 0.001 && lbl_11_bss_B08 <= 0)
                    {
                        lbl_11_bss_B08 = 30;
                        lbl_11_bss_88++;
                    }
                    else if (lbl_11_bss_B0C < 0.001)
                        lbl_11_bss_B08--;
                }
                if (did_ball_fallout(&balls[modeCtrl.currPlayer]))
                {
                    /* out of bounds or in the water: one stroke penalty */
                    if (lbl_11_bss_B04 == 0)
                    {
                        GOLF_SET_VOICE_PLAYER(modeCtrl.currPlayer);
                        call_SoundReqID_arg_0(0x51);
                        call_SoundReqID_arg_0(0x15);
                        GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8)++;
                    }
                    if (lbl_11_bss_B04 == 10)
                        call_SoundReqID_arg_0(0x1D8);
                    if (lbl_11_bss_B04 == 15)
                        call_SoundReqID_arg_0(0xC5);
                    if (lbl_11_bss_B04 == 29)
                    {
                        g_maybe_smth_with_music(0x42, 0);
                        fn_1_3061C(0x148, 0x149);
                    }
                    if (lbl_11_bss_B04 < 180)
                    {
                        lbl_11_bss_B04++;
                        lbl_11_bss_24->unk1F = 13;
                        break;
                    }
                    if (lbl_1_bss_1704.unk0 == 1 && lbl_11_data_258[GOLF_OTHER_PLAYER] == 1
                     && GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8) <= GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8)
                     && lbl_11_data_23C[lbl_11_bss_C8] != GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8))
                    {
                        /* match play: the hole is lost */
                        GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = lbl_11_data_23C[lbl_11_bss_C8];
                        lbl_11_bss_E8 = 2;
                        lbl_11_data_258[modeCtrl.currPlayer] = 1;
                    }
                    lbl_11_bss_B04 = 0;
                    g_smth_related_to_music(0x42, lbl_80198B38, lbl_80198B38, 100, 0);
                    if (math_sqrt(mathutil_sum_of_sq_3(stagedef->startPos->pos.x - lbl_11_bss_154.x, 0.0f,
                                                       stagedef->startPos->pos.z - lbl_11_bss_154.z)) < 0.01
                     && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) < lbl_11_data_23C[lbl_11_bss_C8] - 1)
                    {
                        /* shot from the tee: tee off again */
                        lbl_11_bss_20->vel.x = 0.0f;
                        lbl_11_bss_20->vel.y = -1e-07f;
                        lbl_11_bss_20->vel.z = 0.0f;
                        lbl_11_bss_20->unk80 = 0;
                        lbl_11_bss_20->unkA0.w = 1.0f;
                        lbl_11_bss_20->unkA0.x = 0.0f;
                        lbl_11_bss_20->unkA0.y = 0.0f;
                        lbl_11_bss_20->unkA0.z = 0.0f;
                        fn_11_EB80(&lbl_11_bss_20->pos);
                        lbl_11_bss_7C = 0;
                        lbl_11_bss_20->accel = 0.0f;
                        lbl_11_bss_20->pos.y += 2.8223976212319952;
                        lbl_11_bss_20->prevPos = lbl_11_bss_20->pos;
                        fn_11_EB38(&lbl_11_bss_20->pos);
                        GOLF_FACE_CUP(dir3);
                        lbl_11_bss_B10 = lbl_11_bss_B12 = lbl_11_bss_20->unk92;
                        g_move_and_collide(&balls[modeCtrl.currPlayer], &physBall);
                        fn_1_4AF48(&balls[modeCtrl.currPlayer], &physBall);
                        position_ball(&balls[modeCtrl.currPlayer], &physBall);
                        lbl_11_bss_20->unk80++;
                        lbl_11_data_25C[modeCtrl.currPlayer] = 1;
                        fn_11_D94C();
                        lbl_11_bss_145 = 1;
                    }
                    else
                    {
                        lbl_11_bss_20->unk154 = 3;
                        lbl_11_bss_24->unk1F = 2;
                        lbl_11_bss_145 = 1;
                    }
                    break;
                }
                lbl_11_bss_AFC = 180;
                if (lbl_11_bss_90 != 0)
                    fn_11_1C2FC();
                else
                    fn_11_1C08C();
                goto lbl_A;
            }

            /* the ball stopped */
            if (lbl_1_bss_1704.unk0 == 1 && lbl_11_bss_E8 == 2)
                lbl_11_bss_49 = 0;
            if (lbl_11_bss_49 != 0 && fn_11_F2A8(&lbl_11_bss_20->pos) > 0.8
             && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) < lbl_11_data_23C[lbl_11_bss_C8] - 2)
            {
                /* comment on the shot */
                int said = 0;

                raycast_stage_down(&lbl_11_bss_20->pos, &hit5, NULL);
                if (hit5.flags & 2)
                {
                    if (lbl_11_bss_B14 != 3)
                    {
                        call_SoundReqID_arg_0(0x1D6);
                        call_SoundReqID_arg_0(0xD8C2);
                        said = 1;
                    }
                }
                else if (!(hit5.flags & 4))
                {
                    if (hit5.flags & 8)
                    {
                        call_SoundReqID_arg_0(0xCF);
                        said = 1;
                    }
                    else if (lbl_11_bss_B14 == 3 && fn_11_F2A8(&lbl_11_bss_148) < 2.5f)
                    {
                        if (fabs(fn_11_F2A8(&lbl_11_bss_148) - fn_11_F2A8(&lbl_11_bss_154)) > 10.0)
                        {
                            call_SoundReqID_arg_0(0x1D7);
                            call_SoundReqID_arg_0(0xD8C2);
                            said = 1;
                        }
                    }
                }
                if (said == 0)
                {
                    if (lbl_11_bss_4C >= 12 && fn_11_F2A8(&lbl_11_bss_148) < 2.5f)
                        call_SoundReqID_arg_0(0xCF);
                    else if (fn_11_F2A8(&lbl_11_bss_148) < 30.0f)
                        call_SoundReqID_arg_0(0xD8C2);
                    else
                    {
                        dist = math_sqrt(mathutil_sum_of_sq_3(lbl_11_bss_148.x - lbl_11_bss_154.x,
                                                              lbl_11_bss_148.y - lbl_11_bss_154.y,
                                                              lbl_11_bss_148.z - lbl_11_bss_154.z));
                        if (dist > fn_11_F2A8(&lbl_11_bss_154))
                            call_SoundReqID_arg_0(0xD0);
                        else if (lbl_11_bss_49 == 1 && fn_11_F2A8(&lbl_11_bss_148) < fn_11_F2A8(&lbl_11_bss_154))
                            call_SoundReqID_arg_0(0xD8C2);
                        else if (lbl_11_bss_49 == 2 && fn_11_F2A8(&lbl_11_bss_148) < fn_11_F2A8(&lbl_11_bss_154))
                            call_SoundReqID_arg_0(0xD0);
                        else
                            call_SoundReqID_arg_0(0xD8C2);
                    }
                }
                lbl_11_bss_49 = 0;
            }
            if (lbl_11_bss_90 != 0 && lbl_11_bss_AFC == 180 && lbl_1_bss_1704.unk0 == 1)
            {
                /* match play: decide the hole once the ball is at rest */
                if (fn_11_F2A8(&lbl_11_bss_20->pos) < 0.8)
                {
                    if (lbl_11_data_258[GOLF_OTHER_PLAYER] != 1
                     && GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8) >= GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8))
                    {
                        GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8) = lbl_11_data_23C[lbl_11_bss_C8];
                        lbl_11_bss_E8 = 1;
                        lbl_11_data_258[GOLF_OTHER_PLAYER] = 1;
                    }
                    if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) > -1 && lbl_11_bss_E8 == 1)
                    {
                        GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = 0xFF;
                        if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) >= GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8))
                            GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8) - 1;
                    }
                }
                else if (lbl_11_data_258[GOLF_OTHER_PLAYER] == 1
                      && GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8) <= GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8)
                      && lbl_11_data_23C[lbl_11_bss_C8] != GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8))
                {
                    GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = lbl_11_data_23C[lbl_11_bss_C8];
                    lbl_11_bss_E8 = 2;
                    lbl_11_data_258[modeCtrl.currPlayer] = 1;
                }
            }
            if (lbl_11_bss_AFC > 0)
            {
                lbl_11_bss_4 = 0;
                if (lbl_1_bss_6FB6C != 2 || !(lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14))
                    lbl_11_bss_24->unk1F = 2;
                if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) == 0)
                {
                    lbl_11_bss_B10 = lbl_11_bss_20->unk92 = stagedef->startPos->yrot;
                    GOLF_FACE_DIR(dir4, lbl_11_bss_20->unk92);
                }
                else
                {
                    GOLF_FACE_CUP(dir4);
                    lbl_11_bss_B10 = lbl_11_bss_20->unk92;
                }
                lbl_11_bss_AFC--;
            }
            if (lbl_11_bss_AF8 > 0)
                lbl_11_bss_AF8--;
            if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A)
             && (lbl_11_bss_AF8 > 0 || lbl_11_bss_AFC > 0) && lbl_11_bss_DC < 0)
            {
                /* A skips the wait */
                lbl_11_bss_AFC = 1;
                lbl_11_bss_AF8 = 1;
                lbl_11_bss_DC = 60;
                GOLF_RESET_AIM();
                lbl_11_bss_20->unk92--;
                GOLF_FACE_DIR(dir5, lbl_11_bss_20->unk92);
            }
            if (lbl_11_bss_90 != 0)
                fn_11_1C08C();
            else if (!fn_11_1AA9C())
                goto lbl_C;
            if (lbl_11_bss_AFC > 0 || lbl_11_bss_AF8 > 0)
                goto lbl_B;

            /* aiming */
            lbl_11_bss_B0C = 0.0f;
            if (lbl_11_bss_145 != 0)
            {
                lbl_11_bss_145 = 0;
                lbl_11_data_25C[modeCtrl.currPlayer] = 0;
                GOLF_RESET_AIM();
                lbl_11_bss_20->unk92++;
                GOLF_FACE_DIR(dir6, lbl_11_bss_20->unk92);
                lbl_11_bss_8 = 24;
            }
            if (lbl_11_bss_16C == 1)
            {
                if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) == 0)
                    GOLF_FACE_DIR(dir7, lbl_11_bss_20->unk92);
                else
                    GOLF_FACE_CUP(dir7);
                if (lbl_11_bss_B10 == lbl_11_bss_20->unk92)
                {
                    lbl_11_bss_16C = 0;
                    lbl_11_bss_17C = 1;
                }
                lbl_11_bss_20->unk154 = 0;
                if (lbl_1_bss_6FB6C != 2 || lbl_11_bss_4C == 12 || lbl_11_bss_4C == 13 || lbl_11_bss_4C == 14)
                    lbl_11_bss_24->unk1F = 2;
                lbl_11_bss_148 = lbl_11_bss_20->pos;
                GOLF_RESET_AIM();
            }
            lbl_11_bss_B12 = lbl_11_bss_B10;
            lbl_11_bss_B10 = lbl_11_bss_20->unk92;
            if (lbl_11_bss_48 == 0)
            {
                /* up: previous club */
                if (((-GOLF_PAD(modeCtrl.currPlayer).unk0[0].stickY > 30
                      && fabs((float)(GOLF_STICK_X(GOLF_PAD(modeCtrl.currPlayer)) / 5.0)) < 20.0
                      && fabs((float)(GOLF_STICK_X(GOLF_PAD(modeCtrl.currPlayer)) / 5.0))
                          < fabs((float)-GOLF_PAD(modeCtrl.currPlayer).unk0[0].stickY))
                     || (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_DOWN))
                 && lbl_11_bss_24->unk1F != 12)
                {
                    if (lbl_11_bss_10 == 0)
                    {
                        lbl_11_bss_4C--;
                        if (lbl_11_bss_4C < 0)
                            lbl_11_bss_4C = 14;
                        lbl_11_bss_10 = 15;
                    }
                    GOLF_FACE_DIR(dir8, lbl_11_bss_20->unk92);
                    GOLF_RESET_AIM();
                    if (lbl_11_bss_CA == 3)
                    {
                        if (lbl_11_bss_4C < 12)
                            lbl_11_bss_4C = 14;
                        else if (lbl_11_bss_4C > 14)
                            lbl_11_bss_4C = 12;
                    }
                    if (lbl_11_bss_4C >= 12)
                        lbl_11_bss_58 = &lbl_11_data_2FC[12][0];
                    else
                        lbl_11_bss_58 = &lbl_11_data_2FC[lbl_11_bss_4C][lbl_11_bss_CA];
                }
                /* down: next club */
                if (((-GOLF_PAD(modeCtrl.currPlayer).unk0[0].stickY < -30
                      && fabs((float)(GOLF_STICK_X(GOLF_PAD(modeCtrl.currPlayer)) / 5.0)) < 20.0
                      && fabs((float)(GOLF_STICK_X(GOLF_PAD(modeCtrl.currPlayer)) / 5.0))
                          < fabs((float)-GOLF_PAD(modeCtrl.currPlayer).unk0[0].stickY))
                     || (GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_UP))
                 && lbl_11_bss_24->unk1F != 12)
                {
                    if (lbl_11_bss_10 == 0)
                    {
                        lbl_11_bss_4C++;
                        if (lbl_11_bss_CA == 3)
                        {
                            if (lbl_11_bss_4C < 12)
                                lbl_11_bss_4C = 14;
                            else if (lbl_11_bss_4C > 14)
                                lbl_11_bss_4C = 12;
                        }
                        if (lbl_11_bss_4C > 14)
                            lbl_11_bss_4C = 0;
                        lbl_11_bss_10 = 15;
                    }
                    GOLF_FACE_DIR(dir9, lbl_11_bss_20->unk92);
                    GOLF_RESET_AIM();
                    if (lbl_11_bss_CA == 3 && lbl_11_bss_4C < 12)
                    {
                        lbl_11_bss_4C = 12;
                        f = fn_11_F2A8(&lbl_11_bss_20->pos);
                        if (f > 30.0)
                            lbl_11_bss_4C = 12;
                        else if (f > 10.0)
                            lbl_11_bss_4C = 13;
                        else if (f > 0.0)
                            lbl_11_bss_4C = 14;
                    }
                    if (lbl_11_bss_4C >= 12)
                        lbl_11_bss_58 = &lbl_11_data_2FC[12][0];
                    else
                        lbl_11_bss_58 = &lbl_11_data_2FC[lbl_11_bss_4C][lbl_11_bss_CA];
                }
                /* camera views */
                if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_B) && lbl_11_bss_17C != 0)
                {
                    if (lbl_11_bss_24->unk1F == 2 || lbl_11_bss_24->unk1F == 21 || lbl_11_bss_24->unk1F == 22)
                    {
                        lbl_11_bss_24->unk1F = 20;
                        lbl_11_bss_20->modelScale = 1.0f;
                        call_SoundReqID_arg_0(0x10C);
                    }
                    else if (lbl_11_bss_24->unk1F == 20)
                    {
                        lbl_11_bss_24->unk1F = 2;
                        call_SoundReqID_arg_0(0x10C);
                    }
                }
                if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_Y) && lbl_11_bss_17C != 0)
                {
                    if (lbl_11_bss_24->unk1F == 2 || lbl_11_bss_24->unk1F == 20 || lbl_11_bss_24->unk1F == 22)
                    {
                        lbl_11_bss_24->unk1F = 21;
                        call_SoundReqID_arg_0(0x10C);
                    }
                    else if (lbl_11_bss_24->unk1F == 21)
                    {
                        lbl_11_bss_24->unk1F = 2;
                        lbl_11_bss_20->modelScale = 1.0f;
                        call_SoundReqID_arg_0(0x10C);
                    }
                }
                if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_X) && lbl_11_bss_17C != 0)
                {
                    if (lbl_11_bss_24->unk1F == 2 || lbl_11_bss_24->unk1F == 20 || lbl_11_bss_24->unk1F == 21)
                    {
                        lbl_11_bss_24->unk1F = 22;
                        lbl_11_bss_20->modelScale = 1.0f;
                        call_SoundReqID_arg_0(0x10C);
                    }
                    else if (lbl_11_bss_24->unk1F == 22)
                    {
                        lbl_11_bss_24->unk1F = 2;
                        lbl_11_bss_20->modelScale = 1.0f;
                        call_SoundReqID_arg_0(0x10C);
                    }
                }
                /* turn with the stick */
                if (fabs((float)GOLF_STICK_X(GOLF_PAD(modeCtrl.currPlayer))) > 25.0 && lbl_11_bss_17C != 0)
                {
                    if (lbl_11_bss_24->unk1F != 12)
                    {
                        if (lbl_11_bss_24->unk1F == 22)
                            lbl_11_bss_20->unk92 += GOLF_STICK_X(GOLF_PAD(modeCtrl.currPlayer)) / 10.0;
                        else
                            lbl_11_bss_20->unk92 += GOLF_STICK_X(GOLF_PAD(modeCtrl.currPlayer));
                        GOLF_FACE_DIR(dir10, lbl_11_bss_20->unk92);
                    }
                    GOLF_RESET_AIM();
                }
                /* fine turn with the d-pad */
                if (((GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_BUTTON_LEFT)
                  || (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_BUTTON_RIGHT))
                 && lbl_11_bss_17C != 0)
                {
                    if (lbl_11_bss_24->unk1F != 12)
                    {
                        if (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_BUTTON_LEFT)
                            lbl_11_bss_20->unk92 += 16;
                        if (GOLF_PAD(modeCtrl.currPlayer).unk0[0].button & PAD_BUTTON_RIGHT)
                            lbl_11_bss_20->unk92 -= 16;
                        GOLF_FACE_DIR(dir11, lbl_11_bss_20->unk92);
                    }
                    GOLF_RESET_AIM();
                }
                lbl_11_bss_188 = 0;
                if (lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14)
                    GOLF_PREVIEW_SHOT();
                else if (lbl_11_bss_7C != 0)
                {
                    lbl_11_bss_20->vel.x = lbl_11_bss_20->vel.y = lbl_11_bss_20->vel.z = 0.0f;
                    lbl_11_bss_B18 = 0;
                    lbl_11_bss_50 = 0;
                    lbl_11_bss_60 = 0;
                    lbl_11_bss_50 = 0;
                    lbl_11_bss_20->vel.x = lbl_11_bss_20->vel.y = lbl_11_bss_20->vel.z = 0.0f;
                }
                lbl_11_bss_18C = lbl_11_bss_198[lbl_11_bss_50 - 1];
                /* A: start the swing */
                if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A) && lbl_11_bss_24->unk1F != 12
                 && lbl_11_bss_DC < 0)
                {
                    if (lbl_11_bss_24->unk1F == 21)
                    {
                        lbl_11_bss_20->modelScale = 1.0f;
                        lbl_11_bss_24->unk1F = 2;
                        call_SoundReqID_arg_0(0x10C);
                        lbl_11_bss_DC = 60;
                    }
                    else if (lbl_11_bss_24->unk1F == 20)
                    {
                        lbl_11_bss_24->unk1F = 2;
                        call_SoundReqID_arg_0(0x10C);
                        lbl_11_bss_DC = 60;
                    }
                    else if (lbl_11_bss_24->unk1F == 22)
                    {
                        lbl_11_bss_24->unk1F = 2;
                        call_SoundReqID_arg_0(0x10C);
                        lbl_11_bss_DC = 60;
                    }
                    else
                    {
                        lbl_11_bss_160 = 0.0f;
                        lbl_11_bss_164 = 0;
                        lbl_11_bss_30 = GOLF_STATE_POWER;
                        lbl_11_bss_24->unk1F = 5;
                        lbl_11_bss_DC = 20;
                        lbl_11_bss_B1C = 0.2f;
                        lbl_11_bss_164 = 16;
                        lbl_11_bss_B18 = 0;
                        lbl_11_bss_D0 = 0;
                        g_change_music_volume(0x42, lbl_80198B38, 50);
                        lbl_11_bss_18C = lbl_11_bss_198[lbl_11_bss_50 - 1];
                        dir12 = lbl_11_bss_18C;
                        dir12.y = 100.0f;
                        if ((int)raycast_stage_down(&dir12, &hit6, NULL) != 0)
                        {
                            dir12 = hit6.pos;
                            dir12.y += 50.0f;
                        }
                        lbl_11_bss_18C = dir12;
                        lbl_11_bss_54 = 0;
                        lbl_11_bss_50 = 0;
                        lbl_11_bss_60 = 1;
                        lbl_11_bss_198[0] = lbl_11_bss_20->pos;
                    }
                }
            }
            if (lbl_11_bss_47 != 0)
            {
                /* demo: swing at a random angle */
                float rot = (int)(8192.0f * (float)((rand() & 0x7FFF) % 1000) / 1000.0f) - 0x1000;

                if (lbl_11_bss_34 == 0)
                    rot *= 0.1;
                if (lbl_11_bss_34 == 1)
                    rot *= 0.2;
                if (lbl_11_bss_34 == 2)
                    rot *= 0.5;
                lbl_11_bss_20->unk92 += rot;
                GOLF_FACE_DIR(dir13, lbl_11_bss_20->unk92);
                lbl_11_bss_160 = 0.0f;
                lbl_11_bss_164 = 0;
                lbl_11_bss_30 = GOLF_STATE_POWER;
                lbl_11_bss_24->unk1F = 5;
                lbl_11_bss_34 = 3;
            }
            goto lbl_A;

        case GOLF_STATE_POWER:
            if (lbl_11_bss_4C >= 12)
            {
                lbl_11_bss_180 = 0.0f;
                lbl_11_bss_184 = -100.0f;
            }
            else
            {
                lbl_11_bss_180 = (float)-GOLF_PAD(modeCtrl.currPlayer).unk0[0].stickX / 60.0f;
                lbl_11_bss_184 = (float)GOLF_PAD(modeCtrl.currPlayer).unk0[0].stickY / 60.0f;
            }
            f = math_sqrt(lbl_11_bss_180 * lbl_11_bss_180 + lbl_11_bss_184 * lbl_11_bss_184);
            if (f > 1.0)
            {
                lbl_11_bss_180 /= f;
                lbl_11_bss_184 /= f;
            }
            if (f > 1e-07)
            {
                lbl_11_bss_180 *= 0.3f;
                lbl_11_bss_184 *= 0.3f;
            }
            else
                lbl_11_bss_180 = lbl_11_bss_184 = 0.0f;
            if (!fn_11_1B040())
                goto lbl_C;
            lbl_11_bss_17C = 0;
            /* power meter: up for 80 frames, then back down */
            if (lbl_11_bss_164 % 160 > 80)
            {
                lbl_11_bss_B1C = (float)(160 - lbl_11_bss_164 % 160) / 80.0f;
                lbl_11_bss_160 = lbl_11_bss_B1C * lbl_11_data_170[lbl_11_bss_34];
            }
            else
            {
                lbl_11_bss_B1C = (float)(lbl_11_bss_164 % 160) / 80.0f;
                lbl_11_bss_160 = lbl_11_bss_B1C * lbl_11_data_170[lbl_11_bss_34];
            }
            if (lbl_11_bss_160 > 2.0f * lbl_11_data_170[lbl_11_bss_34])
                lbl_11_bss_160 = lbl_11_bss_168 = 0.0f;
            lbl_11_bss_168 = lbl_11_bss_160;
            if (!(GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A) || lbl_11_bss_D0 != 1)
                lbl_11_bss_164++;
            if (lbl_11_bss_164 % 160 == 0 && lbl_11_bss_D0 == 0)
            {
                lbl_11_bss_30 = GOLF_STATE_AIM;
                lbl_11_bss_24->unk1F = 2;
                lbl_11_bss_17C = 1;
                lbl_11_bss_160 = 0.0f;
                lbl_11_bss_164 = 0;
                lbl_11_bss_168 = 0.0f;
                lbl_11_bss_180 = 0.0f;
                lbl_11_bss_184 = 0.0f;
                g_change_music_volume(0x42, lbl_80198B38, 100);
            }
            else if (lbl_11_bss_164 % 160 == 0 && lbl_11_bss_D0 == 1)
            {
                lbl_11_bss_5C = -0.2f;
                lbl_11_bss_30 = GOLF_STATE_SWING;
            }
            if (lbl_11_bss_47 != 0 && (rand() & 0x7FFF) % 60 == 0)
            {
                if (lbl_11_data_250 != -1)
                {
                    SoundOff(lbl_11_data_250);
                    lbl_11_data_250 = -1;
                }
                lbl_11_bss_30 = GOLF_STATE_SWING;
            }
            lbl_11_bss_168 /= 100.0;
            lbl_11_bss_168 *= 1.3;
            if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A) && lbl_11_bss_DC < 0 && lbl_11_bss_D0 == 0)
            {
                /* first press: power */
                lbl_11_bss_D4 = lbl_11_bss_B1C;
                lbl_11_bss_D4 = (lbl_11_bss_D4 - 0.2) / 0.8f;
                if (1.0 == lbl_11_bss_D4 && lbl_11_bss_4C >= 12)
                    lbl_11_bss_D4 = 0.999f;
                if (fabs(lbl_11_bss_D4) < 1.1920928955078125e-07)
                    lbl_11_bss_D4 = 0.01f;
                lbl_11_bss_D8 = lbl_11_bss_D4;
                lbl_11_bss_D0 = 1;
                printf("power: %lf\n", lbl_11_bss_D4);
                if (lbl_11_bss_4C >= 12 || lbl_11_bss_CA == 3)
                {
                    printf("always safe shot\n");
                    lbl_11_bss_5C = 0.0f;
                    lbl_11_bss_30 = GOLF_STATE_SWING;
                }
                call_SoundReqID_arg_0(0x339);
                if (1.0 == lbl_11_bss_D4)
                {
                    call_SoundReqID_arg_0(10);
                    GOLF_SET_VOICE_PLAYER(modeCtrl.currPlayer);
                    call_SoundReqID_arg_0(0x12D);
                }
            }
            else if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_A) && lbl_11_bss_DC < 0
                  && lbl_11_bss_D0 == 1)
            {
                /* second press: impact */
                lbl_11_bss_5C = (lbl_11_bss_B1C - 0.2f) / 0.8f;
                if (0.0 == lbl_11_bss_5C && 1.0 == lbl_11_bss_D4)
                    printf("### nice shot\n");
                else if (fabs(lbl_11_bss_5C) < *lbl_11_bss_58 / 2.0f)
                    printf("### safe shot %lf\n", lbl_11_bss_5C);
                else
                    printf("### miss shot %lf\n", lbl_11_bss_5C);
                lbl_11_bss_30 = GOLF_STATE_SWING;
                for (i = 0; i < 4; i++)
                {
                    if (i != active_monkey_id[modeCtrl.currPlayer])
                        preload_ape(i, 0, 0, 15);
                }
                call_SoundReqID_arg_0(0x339);
                if (0.0 == lbl_11_bss_5C)
                {
                    call_SoundReqID_arg_0(10);
                    GOLF_SET_VOICE_PLAYER(modeCtrl.currPlayer);
                    call_SoundReqID_arg_0(0x12D);
                }
            }
            if ((GOLF_PAD(modeCtrl.currPlayer).unk0[2].button & PAD_BUTTON_B) && lbl_11_bss_D0 == 0)
            {
                /* B cancels */
                lbl_11_bss_30 = GOLF_STATE_AIM;
                lbl_11_bss_24->unk1F = 2;
                lbl_11_bss_17C = 1;
                lbl_11_bss_160 = 0.0f;
                lbl_11_bss_164 = 0;
                lbl_11_bss_D8 = 0.0f;
                lbl_11_bss_168 = 0.0f;
                lbl_11_bss_180 = 0.0f;
                lbl_11_bss_184 = 0.0f;
                call_SoundReqID_arg_0(0x119);
                g_change_music_volume(0x42, lbl_80198B38, 100);
            }
            goto lbl_A;

        case GOLF_STATE_SWING:
            lbl_11_bss_B20 = lbl_11_bss_168;
            lbl_11_bss_B24 = lbl_11_bss_20->unk92;
            lbl_11_bss_B28 = lbl_11_bss_20->pos;
            /* keep a copy of the ball and ape for the replay */
            balls[4] = balls[modeCtrl.currPlayer];
            balls[4].status = 0;
            /* balls[4].ape is ball->ape again here (casts: DPTR fields on x64) */
#ifndef __MWERKS__
            if (balls[4].ape != lbl_11_bss_20->ape)
#endif
            *(struct Ape *)balls[4].ape = *(struct Ape *)lbl_11_bss_20->ape;
            lbl_11_bss_B14 = lbl_11_bss_CA;
            lbl_11_bss_B34 = lbl_11_bss_4C;
            lbl_11_bss_B38 = 1;
            lbl_11_bss_B3C = absPower = fabs(lbl_11_bss_D4);
            if (0.0 == lbl_11_bss_5C && 1.0 == lbl_11_bss_D4)
            {
                lbl_11_bss_24->unk1F = 23;
                lbl_11_bss_20->unk60.y = lbl_11_bss_20->unk92;
            }
            if (fn_11_1B42C(&absPower) == 0)
            {
                lbl_11_bss_160 = 0.0f;
                lbl_11_bss_164 = 0;
                goto lbl_C;
            }
            g_change_music_volume(0x42, lbl_80198B38, 100);
            g_fade_track_volume(100, 8);
            if (lbl_11_bss_4C < 12 && lbl_11_bss_CA != 3 && (0.0 != lbl_11_bss_5C || 1.0 != lbl_11_bss_D4))
            {
                /* impact error: hook or slice */
                if (fabs(lbl_11_bss_5C) < *lbl_11_bss_58 / 2.0f)
                    lbl_11_bss_180 += lbl_11_bss_5C;
                else if (fabs(lbl_11_bss_5C) > 0.15)
                {
                    if (lbl_11_bss_5C > 0.2)
                        lbl_11_bss_5C = 0.2f;
                    lbl_11_bss_180 -= 2.0f * lbl_11_bss_5C;
                    lbl_11_bss_D4 *= 0.35 + (0.25 - fabs(lbl_11_bss_5C));
                }
                else
                {
                    lbl_11_bss_180 -= lbl_11_bss_5C;
                    lbl_11_bss_D4 *= 0.65 + (0.15 - fabs(lbl_11_bss_5C));
                }
            }
            lbl_11_bss_160 = 0.0f;
            lbl_11_bss_164 = 0;
            lbl_11_bss_20->state = 26;
            lbl_11_bss_20->unk154 = 4;
            {
                float power = lbl_11_data_2EC[lbl_11_bss_CA] * (lbl_11_bss_94 * (lbl_11_data_1A0[lbl_11_bss_4C] * fabs(lbl_11_bss_D4)));
                if (0.0 == lbl_11_bss_5C && 1.0 == lbl_11_bss_D4)
                    power *= 1.1;
                lbl_11_bss_20->vel.x = power * math_sin(lbl_11_data_180[lbl_11_bss_4C] + 0x4000) * -math_sin(lbl_11_bss_20->unk92);
                lbl_11_bss_20->vel.y = power * math_sin(lbl_11_data_180[lbl_11_bss_4C]);
                lbl_11_bss_20->vel.z = power * math_sin(lbl_11_data_180[lbl_11_bss_4C] + 0x4000) * -math_sin(lbl_11_bss_20->unk92 + 0x4000);
            }
            lbl_11_bss_D4 = fabs(lbl_11_bss_D4);
            lbl_11_bss_18C.x = lbl_11_bss_18C.x * lbl_11_bss_D4 + lbl_11_bss_20->pos.x * (1.0f - lbl_11_bss_D4);
            lbl_11_bss_18C.y = lbl_11_bss_18C.y * lbl_11_bss_D4 + lbl_11_bss_20->pos.y * (1.0f - lbl_11_bss_D4);
            lbl_11_bss_18C.z = lbl_11_bss_18C.z * lbl_11_bss_D4 + lbl_11_bss_20->pos.z * (1.0f - lbl_11_bss_D4);
            /* spin the ape around the axis perpendicular to the shot */
            spinAxis = lbl_11_bss_20->vel;
            {
                Vec v = {0.0f, 1.0f, 0.0f};
                Vec *src = &v;
                up = *src;
            }
            vec_normalize_len(&spinAxis);
            vec_normalize_len(&up);
            mathutil_vec_cross_prod(&spinAxis, &up, &spinAxis);
            mtxa_from_rotate_y(-0x4000);
            mtxa_tf_vec(&spinAxis, &spinAxis);
            quat_from_axis_angle(&spinQuat, &spinAxis, (int)(9029.0f * lbl_11_bss_D4));
            quat_mult(&lbl_11_bss_20->unkA0, &spinQuat, &lbl_11_bss_20->unkA0);
            if ((lbl_11_bss_4C == 0 || lbl_11_bss_4C == 1 || lbl_11_bss_4C == 2) && 0.0 == lbl_11_bss_5C && 1.0 == lbl_11_bss_D4)
                lbl_11_bss_70 = 1;
            lbl_11_bss_154 = lbl_11_bss_148;
            lbl_11_bss_30 = GOLF_STATE_AIM;
            lbl_11_bss_88 = 0;
            lbl_11_bss_24->unk1F = 4;
            lbl_11_bss_B40 = 0;
            lbl_11_bss_16C = 1;
            lbl_11_bss_B0C = mathutil_vec_len(&lbl_11_bss_20->vel);
            GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8)++;
            lbl_11_bss_90 = 1;
            lbl_11_bss_B44 = lbl_11_bss_20->vel;
            goto lbl_A;

        case GOLF_STATE_REPLAY_INIT:
            lbl_11_bss_B50 = 0;
            for (n7 = 0; n7 < modeCtrl.playerCount; n7++)
            {
                balls[n7].unk154 = 0;
                balls[n7].flags |= 0x10;
                balls[n7].flags &= ~0x4000;
                balls[n7].flags &= ~0x200;
                if (balls[n7].ape != NULL)
                {
                    balls[n7].ape->flags |= 0x20;
                    balls[n7].ape->flags &= ~0x4000;
                }
            }
            balls[modeCtrl.currPlayer] = balls[4];
            lbl_11_bss_20->status = 2;
#ifndef __MWERKS__
            if (lbl_11_bss_20->ape != balls[4].ape)
#endif
            *(struct Ape *)lbl_11_bss_20->ape = *(struct Ape *)balls[4].ape;
            lbl_11_bss_30 = GOLF_STATE_REPLAY_CAMERA;
            fn_11_1B040();
            lbl_11_bss_144 = 0;
            lbl_11_bss_CA = lbl_11_bss_B14;
            lbl_11_bss_4C = lbl_11_bss_B34;
            break;

        case GOLF_STATE_REPLAY_CAMERA:
            lbl_11_bss_20->state = 26;
            lbl_11_bss_20->unk154 = 0;
            if (lbl_11_bss_20->ape != NULL)
                lbl_11_bss_20->ape->ballIdx = modeCtrl.currPlayer;
            lbl_11_bss_20->flags &= ~0x10;
            lbl_11_bss_20->flags |= 0x4000;
            if (lbl_11_bss_20->ape != NULL)
                lbl_11_bss_20->ape->flags &= ~0x20;
            if (lbl_11_bss_DE == 1)
            {
                camera_clear(&cameras[0]);
                set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
                set_camera_view_settings(1, 0.0f, 0.0f, 0.0f, 0.0f);
                set_camera_view_settings(2, 0.0f, 0.0f, 0.0f, 0.0f);
                set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
            }
            else if (lbl_11_bss_DE == 2)
            {
                set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
                set_camera_view_settings(1, 0.0f, 0.0f, 0.0f, 0.0f);
                set_camera_view_settings(2, 0.0f, 0.0f, 0.0f, 0.0f);
                set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
            }
            else if (lbl_11_bss_DE == 3)
            {
                set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
                set_camera_view_settings(1, 0.0f, 0.0f, 0.0f, 0.0f);
                set_camera_view_settings(2, 0.0f, 0.0f, 0.0f, 0.0f);
                set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
            }
            cameras[0].unk26 = 0;
            cameras[1].unk26 = 0;
            cameras[2].unk26 = 0;
            cameras[3].unk26 = 0;
            cameras[0].mode = 64;
            cameras[1].mode = 64;
            cameras[2].mode = 64;
            cameras[3].mode = 64;
            if (lbl_11_bss_DE == 1)
                cameras[0].unk1F = 1;
            else
                cameras[0].unk1F = 16;
            cameras[1].unk1F = 16;
            cameras[2].unk1F = 18;
            cameras[3].unk1F = 1;
            lbl_11_bss_30 = GOLF_STATE_REPLAY_INTRO;
            lbl_11_bss_AFC = 120;
            fn_11_1B040();
            break;

        case GOLF_STATE_REPLAY_INTRO:
            if (lbl_11_bss_AFC > 0)
            {
                lbl_11_bss_AFC--;
                GOLF_FACE_DIR(dir14, lbl_11_bss_B24);
                if (lbl_11_bss_DE == 1)
                    cameras[0].unk1F = 2;
                cameras[2].unk1F = 18;
                cameras[3].unk1F = 15;
                fn_11_1B040();
            }
            else
            {
                lbl_11_bss_30 = GOLF_STATE_REPLAY_SHOT;
                GOLF_RESET_AIM();
            }
            break;

        case GOLF_STATE_REPLAY_SHOT:
            lbl_11_bss_160 = (100.0 * lbl_11_bss_B20) / 1.3;
            lbl_11_bss_160 = lbl_11_bss_160 * ((float)lbl_11_data_178[lbl_11_bss_34] / lbl_11_data_170[lbl_11_bss_34]);
            lbl_11_bss_B40 = 3;
            if (fn_11_1B42C(&lbl_11_bss_B3C) == 0)
            {
                lbl_11_bss_160 = 0.0f;
                lbl_11_bss_164 = 0;
                goto lbl_C;
            }
            lbl_11_bss_B40 = 0;
            lbl_11_bss_160 = 0.0f;
            lbl_11_bss_164 = 0;
            if (lbl_11_bss_4C < 12 && lbl_11_bss_CA != 3 && (0.0 != lbl_11_bss_5C || 1.0 != lbl_11_bss_D4))
            {
                if (fabs(lbl_11_bss_5C) < *lbl_11_bss_58 / 2.0f)
                    lbl_11_bss_180 += lbl_11_bss_5C;
                else if (fabs(lbl_11_bss_5C) > 0.15)
                {
                    if (lbl_11_bss_5C > 0.2)
                        lbl_11_bss_5C = 0.2f;
                    lbl_11_bss_180 -= 2.0f * lbl_11_bss_5C;
                    lbl_11_bss_D4 *= 0.35 + (0.25 - fabs(lbl_11_bss_5C));
                }
                else
                {
                    lbl_11_bss_180 -= lbl_11_bss_5C;
                    lbl_11_bss_D4 *= 0.65 + (0.15 - fabs(lbl_11_bss_5C));
                }
            }
            lbl_11_bss_20->state = 26;
            lbl_11_bss_20->unk154 = 4;
            printf("save_angle_id: %d\n", lbl_11_bss_B34);
            lbl_11_bss_4C = lbl_11_bss_B34;
            lbl_11_bss_CA = lbl_11_bss_B14;
            lbl_11_bss_D4 = lbl_11_bss_B3C;
            {
                float power = lbl_11_data_2EC[lbl_11_bss_CA] * (lbl_11_bss_94 * (lbl_11_data_1A0[lbl_11_bss_4C] * fabs(lbl_11_bss_D4)));
                if (0.0 == lbl_11_bss_5C && 1.0 == lbl_11_bss_D4)
                    power *= 1.1;
                lbl_11_bss_20->vel.x = power * math_sin(lbl_11_data_180[lbl_11_bss_4C] + 0x4000) * -math_sin(lbl_11_bss_20->unk92);
                lbl_11_bss_20->vel.y = power * math_sin(lbl_11_data_180[lbl_11_bss_4C]);
                lbl_11_bss_20->vel.z = power * math_sin(lbl_11_data_180[lbl_11_bss_4C] + 0x4000) * -math_sin(lbl_11_bss_20->unk92 + 0x4000);
            }
            lbl_11_bss_20->vel.x = lbl_11_bss_B44.x;
            lbl_11_bss_20->vel.y = lbl_11_bss_B44.y;
            lbl_11_bss_20->vel.z = lbl_11_bss_B44.z;
            lbl_11_bss_30 = GOLF_STATE_REPLAY_FLIGHT;
            lbl_11_bss_AFC = (lbl_11_bss_DE == 2) ? 240 : 120;
            if (lbl_11_bss_DE == 1)
                cameras[0].unk1F = 4;
            cameras[2].unk1F = 18;
            cameras[3].unk1F = 15;
            lbl_11_bss_16C = 1;
            lbl_11_bss_B0C = mathutil_vec_len(&lbl_11_bss_20->vel);
            lbl_11_bss_90 = 1;
            break;

        case GOLF_STATE_REPLAY_FLIGHT:
            lbl_11_bss_B0C = f = mathutil_vec_len(&lbl_11_bss_20->vel);
            if (f > 0.0f && lbl_11_bss_20->unk154 != 6)
            {
                lbl_11_bss_20->state = 26;
                lbl_11_bss_20->unk154 = 4;
                lbl_11_bss_148 = lbl_11_bss_20->pos;
                cameras[2].unk1F = 18;
                cameras[3].unk1F = 15;
                lbl_11_bss_AFC = (lbl_11_bss_DE == 2) ? 360 : 120;
                if (lbl_11_bss_90 != 0)
                    fn_11_1C2FC();
                else
                    fn_11_1C08C();
                if (fn_11_F2A8(&lbl_11_bss_20->pos) < 0.8 && lbl_11_bss_144 == 0)
                {
                    call_SoundReqID_arg_0(0xB7);
                    lbl_11_bss_144 = 1;
                    lbl_11_bss_20->vel.y = 0.0f;
                }
            }
            else if (lbl_11_bss_AFC > 0)
            {
                lbl_11_bss_AFC--;
                if (lbl_11_bss_AFC == 240 && lbl_11_bss_DE == 2)
                {
                    lbl_11_bss_B50 = 60;
                    lbl_11_bss_20->pos.y += 1.0;
                    lbl_11_bss_20->prevPos.y += 1.0;
                    lbl_11_bss_20->unk154 = 6;
                    cameras[0].unk1F = 7;
                }
                if (lbl_11_bss_AFC <= 240 && lbl_11_bss_DE == 2)
                {
                    fn_11_1C538(fn_11_16FD8());
                }
            }
            else
            {
            replay_next:
                lbl_11_bss_90 = 0;
                lbl_11_bss_DE++;
                if (lbl_11_bss_DE > 2)
                {
                    /* replay finished */
                    for (i = 0; i < 5; i++)
                    {
                        set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
                        cameras[i].flags &= ~0x80;
                        cameras[i].flags &= ~0x10;
                        cameras[i].unk1F = 13;
                    }
                    set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
                    if (fn_11_E9A8())
                        lbl_11_bss_30 = GOLF_STATE_HOLE_END;
                    else
                    {
                        fn_11_D94C();
                        lbl_11_bss_AFC = lbl_11_bss_AF8 = 0;
                    }
                    lbl_11_bss_DE = 0;
                    g_change_music_volume(0x44, 30, 0);
                    g_SoftStreamStart_with_some_defaults_2(0x42);
                }
                else
                {
                    lbl_11_bss_30 = GOLF_STATE_REPLAY_INIT;
                    lbl_11_bss_B50 = 0;
                    cameras[0].unk1F = 1;
                }
            }
            break;

        case GOLF_STATE_GIVE_UP:
        case GOLF_STATE_CUP_IN:
            lbl_11_bss_17C = 0;
            if (lbl_11_bss_AFC > 0)
                lbl_11_bss_AFC--;
            if (lbl_11_bss_AF8 > 0)
                lbl_11_bss_AF8--;
            if (lbl_11_bss_AFC > 0 && lbl_11_bss_AF8 > 0)
                goto lbl_B;
            /* fall through */
        default:
        lbl_A:
            if (lbl_11_bss_90 != 0 && 0.0f == lbl_11_bss_B0C)
            {
                /* the shot is over */
                lbl_11_bss_90 = 0;
                if (fn_11_E9A8())
                {
                    /* everyone holed out */
                    if (lbl_11_data_258[modeCtrl.currPlayer] == 1 && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) == 1)
                    {
                        /* hole in one: show the replay */
                        lbl_11_bss_DE = 1;
                        g_SoftStreamStart_with_some_defaults_2(0x44);
                        lbl_11_bss_30 = GOLF_STATE_REPLAY_INIT;
                        lbl_11_bss_B50 = 0;
                        goto lbl_lie;
                    }
                    for (i = 0; i < 5; i++)
                    {
                        set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
                        cameras[i].flags &= ~0x80;
                        cameras[i].flags &= ~0x10;
                        cameras[i].unk1F = 13;
                    }
                    set_camera_view_settings(modeCtrl.currPlayer, 0.0f, 0.0f, 1.0f, 1.0f);
                    lbl_11_bss_30 = GOLF_STATE_HOLE_END;
                    break;
                }
                if (lbl_11_data_258[modeCtrl.currPlayer] == 1 && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) == 1)
                {
                    lbl_11_bss_DE = 1;
                    g_SoftStreamStart_with_some_defaults_2(0x44);
                    lbl_11_bss_30 = GOLF_STATE_REPLAY_INIT;
                    lbl_11_bss_B50 = 0;
                }
                else
                {
                    fn_11_D94C();
                    lbl_11_bss_AFC = lbl_11_bss_AF8 = 0;
                }
            lbl_lie:
                raycast_stage_down(&lbl_11_bss_20->pos, &hit7, NULL);
                if (hit7.flags & 2)
                {
                    lbl_11_bss_CA = 3;
                    printf("shot_on_green %d\n", modeCtrl.currPlayer);
                }
                else if (hit7.flags & 4)
                {
                    lbl_11_bss_CA = 1;
                    printf("shot_on_rough %d\n", modeCtrl.currPlayer);
                }
                else if (hit7.flags & 8)
                {
                    lbl_11_bss_CA = 2;
                    printf("shot_on_bunk %d\n", modeCtrl.currPlayer);
                }
                else
                {
                    lbl_11_bss_CA = 0;
                    printf("shot_on_fairway %d\n", modeCtrl.currPlayer);
                }
                f = fn_11_F2A8(&lbl_11_bss_20->pos) / lbl_11_bss_94;
                GOLF_PICK_CLUB(f);
                GOLF_SET_CLUB_TABLE();
            }
        lbl_B:
            ballPos = &lbl_11_bss_20->pos;
            if (lbl_11_bss_90 != 0 && lbl_11_bss_B0C <= 0.0f)
            {
                if (lbl_1_bss_6FB6C != 2 || lbl_11_bss_4C >= 12)
                    lbl_11_bss_24->unk1F = 2;
            }
            else if (fn_11_F2A8(ballPos) < 25.0f && lbl_11_bss_90 != 0)
            {
                lbl_11_bss_24->unk1F = 6;
                lbl_11_bss_4 = 60;
            }
            else if (lbl_11_bss_4 > 0 && lbl_11_bss_90 != 0)
            {
                lbl_11_bss_24->unk1F = 6;
                lbl_11_bss_4--;
            }
            else if (lbl_11_bss_24->unk1F != 3 && lbl_11_bss_24->unk1F != 4 && lbl_11_bss_90 != 0)
                lbl_11_bss_24->unk1F = 1;
            if (fn_11_F2A8(&lbl_11_bss_20->pos) < 0.8 && 0.0f == lbl_11_bss_B0C)
            {
                /* in the cup */
                lbl_11_data_258[modeCtrl.currPlayer] = 1;
                event_vibration_dest();
                lbl_11_bss_30 = GOLF_STATE_CUP_IN;
                lbl_11_bss_24->unk1F = 7;
                if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) <= lbl_11_data_228[lbl_11_bss_C8])
                {
                    lbl_11_bss_20->pos.y += 1.0;
                    lbl_11_bss_20->prevPos.y += 1.0;
                }
                lbl_11_bss_E4 = 0;
                lbl_11_bss_49 = 0;
                if (lbl_1_bss_1704.unk0 == 1 && lbl_11_bss_E8 == 1 && GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) != 1)
                {
                    GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = lbl_11_data_228[lbl_11_bss_C8] - 1;
                    if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) >= GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8))
                        GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) = GOLF_SCORE(GOLF_OTHER_PLAYER, lbl_11_bss_C8) - 1;
                }
                for (i = 0; i < 4; i++)
                    preload_ape(i, 0, 0, 15);
                switch (fn_11_16FD8())
                {
                case -10:
                    lbl_11_bss_E0 = 510;
                    g_fade_track_volume(0, 8);
                    lbl_11_bss_20->state = 26;
                    lbl_11_bss_20->unk154 = 5;
                    break;
                case -3:
                    lbl_11_bss_E0 = 480;
                    g_fade_track_volume(0, 8);
                    lbl_11_bss_20->state = 26;
                    lbl_11_bss_20->unk154 = 5;
                    break;
                case -2:
                    lbl_11_bss_E0 = 480;
                    g_fade_track_volume(0, 8);
                    lbl_11_bss_20->state = 26;
                    lbl_11_bss_20->unk154 = 5;
                    break;
                case -1:
                    lbl_11_bss_E0 = 360;
                    g_fade_track_volume(0, 8);
                    lbl_11_bss_20->state = 26;
                    lbl_11_bss_20->unk154 = 5;
                    break;
                case 0:
                    lbl_11_bss_E0 = 240;
                    g_fade_track_volume(0, 8);
                    lbl_11_bss_20->state = 26;
                    lbl_11_bss_20->unk154 = 5;
                    break;
                default:
                    lbl_11_bss_E0 = 240;
                    g_fade_track_volume(0, 8);
                    lbl_11_bss_20->state = 26;
                    lbl_11_bss_20->unk154 = 4;
                    break;
                }
                lbl_11_bss_AFC = lbl_11_bss_AF8 = 0;
            }
            else if (GOLF_SCORE(modeCtrl.currPlayer, lbl_11_bss_C8) >= lbl_11_data_23C[lbl_11_bss_C8] - 1
                  && 0.0f == lbl_11_bss_B0C)
            {
                /* stroke limit reached: give up the hole */
                g_fade_track_volume(0, 8);
                lbl_11_data_258[modeCtrl.currPlayer] = 1;
                lbl_11_bss_30 = GOLF_STATE_GIVE_UP;
                lbl_11_bss_E0 = 300;
                lbl_11_bss_AFC = lbl_11_bss_AF8 = 0;
                lbl_11_bss_20->state = 26;
                lbl_11_bss_20->unk154 = 4;
                lbl_11_bss_24->unk1F = 7;
                lbl_11_bss_E4 = 0;
            }
        lbl_C:
            if (fn_11_F2A8(&lbl_11_bss_20->pos) < 0.8 && lbl_11_bss_144 == 0)
            {
                g_rumble_controller(player_pad_map[modeCtrl.currPlayer], 1, 30);
                call_SoundReqID_arg_0(0xB7);
                lbl_11_bss_144 = 1;
                lbl_11_bss_20->vel.y = 0.0f;
            }
            break;
        }
    }

    if (GOLF_STICK_X(GOLF_PAD(modeCtrl.currPlayer)) || (pad_status_groups[0].unk0[2].button & PAD_BUTTON_LEFT)
     || (pad_status_groups[0].unk0[2].button & PAD_BUTTON_RIGHT))
        lbl_11_bss_8 = 24;
    lbl_11_bss_8++;
    if (lbl_11_bss_8 >= 120)
        lbl_11_bss_8 = -10;
    lbl_11_bss_C++;
    if (lbl_11_bss_C >= 120)
        lbl_11_bss_C = 0;
    if (lbl_11_bss_10 > 0)
        lbl_11_bss_10--;
}
#pragma pop
