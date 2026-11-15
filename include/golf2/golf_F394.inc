/* golf.c fn_11_F394: per-camera draw (w1) */

void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void g_something_with_rend_efc(int mask);
void draw_shadow_reqs(void);
void g_draw_stage(void);
void g_draw_bg(void);
void g_draw_stobjs(void);
void g_draw_ball_and_ape(void);
void ord_tbl_draw_nodes(void);
void handle_test_camera(void);
void set_ui_widescreen_scale_mtx(int width);
void reset_ui_widescreen_scale_mtx(void);
void GXSetZMode_cached(u8 compareEnable, int func, u8 updateEnable);
extern struct GMA *minigameGma;
extern struct Sphere lbl_1_bss_711B0;  /* the stage's bounding sphere */

void fn_11_104B4(void);
void fn_11_114D8(void);

#define GOLF_MODEL(n) (minigameGma->modelEntries[n].model)

/* Main draw: every active camera draws the stage, golf ball markers, shot trail and HUD. */
void fn_11_F394(void)
{
    int i;
    struct Ball *ballBackup;
    int j;
    s8 status;
    s8 state;
    float t;
    struct GolfGroundTexParams ground;
    Vec pos;
    Vec eye;
    Vec dir;
    Vec mid;

    ballBackup = current_ball;
    for (i = 0; i < 5; i++)
    {
        if (!(cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f))
            continue;
        if (!(((status = g_poolInfo.playerPool.statusList[i]) != 0 && status != 4 && !(cameras[i].flags & 0x40))
              || (cameras[i].flags & 0x80)))
            continue;
        current_ball = &balls[i];
        g_current_camera = &cameras[i];
        camera_apply_viewport_if_changed(i);
        fn_1_D4870();
        g_maybe_some_bg_effect_handler();
        u_reset_light_group_stack(i);
        if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
            g_something_with_rend_efc(4);
        if ((lbl_11_bss_80 > -1 && minigameGma != NULL) || lbl_11_bss_30 == 0x40 || lbl_11_bss_30 == 0x80)
        {
            mtxa_from_mtxb();
            mtxa_translate(&stagedef->startPos->pos);
            mtxa_translate_xyz(0.0f, -0.44f, 0.0f);
            if (lbl_11_bss_D4 > 0.5)
            {
                mtxa_translate_xyz(0.0f, 2.0f * math_sin(32768.0f * (59 - lbl_11_bss_80) / 59.0f), 0.0f);
                if (lbl_11_bss_20 != NULL)
                    mtxa_rotate_y(lbl_11_bss_20->unk92 - 16384.0f * lbl_11_bss_5C);
                if (lbl_11_bss_30 != 0x80)
                    mtxa_rotate_x((59 - lbl_11_bss_80) * -0x700);
            }
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(GOLF_MODEL(20));
        }
        state = events[EVENT_STAGE].state;
        if (state == EV_STATE_RUNNING || state == EV_STATE_SUSPENDED)
            g_draw_stage();
        if (lbl_11_bss_CA == 3 && lbl_11_bss_90 == 0 && 0.0 == lbl_11_bss_B0C && lbl_11_bss_AFC <= 0
            && lbl_11_bss_AF8 <= 0 && lbl_11_bss_30 == 8
            && (lbl_11_bss_24->unk1F == 2 || lbl_11_bss_24->unk1F == 20 || lbl_11_bss_24->unk1F == 21
                || lbl_11_bss_24->unk1F == 22))
        {
            ground.model1 = GOLF_MODEL(36);
            ground.model2 = GOLF_MODEL(25);
            ground.pos.x = lbl_1_bss_711B0.pos.x;
            ground.pos.y = 0.0f;
            ground.pos.z = lbl_1_bss_711B0.pos.z;
            ground.unk14 = 2.0f;
            ground.unk18 = 2.0f;
            if (lbl_11_bss_C8 == 0)
                ground.height = 9.0f;
            else if (lbl_11_bss_C8 == 1)
                ground.height = 25.0f;
            else if (lbl_11_bss_C8 == 2)
                ground.height = 0.32f;
            else if (lbl_11_bss_C8 == 3)
                ground.height = 4.0f;
            else if (lbl_11_bss_C8 == 4)
                ground.height = 15.0f;
            else if (lbl_11_bss_C8 == 5)
                ground.height = 2.0f;
            else if (lbl_11_bss_C8 == 6)
                ground.height = 8.0f;
            else if (lbl_11_bss_C8 == 7)
                ground.height = 10.0f;
            else if (lbl_11_bss_C8 == 8)
                ground.height = 10.0f;
            else if (lbl_11_bss_C8 == 9)
                ground.height = 10.0f;
            else if (lbl_11_bss_C8 == 10)
                ground.height = 10.0f;
            else if (lbl_11_bss_C8 == 11)
                ground.height = 17.0f;
            else if (lbl_11_bss_C8 == 12)
                ground.height = 30.0f;
            else if (lbl_11_bss_C8 == 13)
                ground.height = 7.0f;
            else if (lbl_11_bss_C8 == 14)
                ground.height = 15.0f;
            else if (lbl_11_bss_C8 == 15)
                ground.height = 25.0f;
            else if (lbl_11_bss_C8 == 16)
                ground.height = 28.0f;
            else if (lbl_11_bss_C8 == 17)
                ground.height = 7.5f;
            ground.pos = lbl_11_bss_20->pos;
            ground.height = 2.0f;
            fn_11_383A0(&ground);
            fn_11_38774(fn_11_F350);
        }
        draw_shadow_reqs();
        if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
        {
            g_smth_with_rendefc_reflective_height(400.0f);
            g_draw_bg();
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
            g_something_with_rend_efc(0x10);
        if (events[EVENT_STOBJ].state == EV_STATE_RUNNING)
            g_draw_stobjs();
        if (events[EVENT_EFFECT].state == EV_STATE_RUNNING)
            g_draw_effects();
        if (events[EVENT_BALL].state == EV_STATE_RUNNING)
            g_draw_ball_and_ape();
        /* aim marker next to the ball */
        if ((lbl_11_bss_30 == 0x20 || (lbl_11_bss_30 == 0x10 && lbl_11_bss_24->unk1F != 23))
            && lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14)
        {
            pos = lbl_11_bss_20->pos;
            eye = lbl_11_bss_24->eye;
            dir.x = eye.x - pos.x;
            dir.y = eye.y - pos.y;
            dir.z = eye.z - pos.z;
            vec_normalize_len(&dir);
            eye = dir;
            eye.y = -eye.z;
            eye.z = eye.x;
            eye.x = eye.y;
            dir.x += pos.x;
            dir.y += pos.y;
            dir.z += pos.z;
            eye.x = dir.x + eye.x * lbl_11_bss_180;
            eye.z = dir.z + eye.z * lbl_11_bss_180;
            eye.y = dir.y + lbl_11_bss_184;
            dir.x = eye.x - pos.x;
            dir.y = eye.y - pos.y;
            dir.z = eye.z - pos.z;
            vec_normalize_len(&dir);
            pos.x += dir.x;
            pos.y += dir.y;
            pos.z += dir.z;
            mtxa_from_mtx(cameras[i].unk14C);
            mtxa_translate(&pos);
            mtxa_scale_s(0.1f - 0.02 * math_sin(g_font_char_var_3 << 11));
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_always(GOLF_MODEL(39));
        }
        /* predicted shot trail */
        if (lbl_11_bss_30 == 0x20 || lbl_11_bss_30 == 0x10 || lbl_11_bss_30 == 8)
        {
            if (lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14)
            {
                if (lbl_11_bss_50 != 0 && lbl_11_bss_24->unk1F != 21)
                {
                    mid.x = (lbl_11_bss_198[0].x + lbl_11_bss_20->pos.x) / 2.0;
                    mid.y = (lbl_11_bss_198[0].y + lbl_11_bss_20->pos.y) / 2.0;
                    mid.z = (lbl_11_bss_198[0].z + lbl_11_bss_20->pos.z) / 2.0;
                    mtxa_from_mtxb();
                    t = (float)(int)(g_font_char_var_3 % (lbl_11_bss_50 / 2) * 2) / (float)lbl_11_bss_50;
                    t = math_sin(32768.0f * t);
                    set_post_mult_color(1.0f, t, 1.0f, 0.5f);
                    mtxa_translate(&mid);
                    g_smth_with_gpu_and_locked_cache_mtx();
                    avdisp_draw_model_culled_sort_always(GOLF_MODEL(39));
                }
                for (j = 0; j < lbl_11_bss_50; j++)
                {
                    t = (float)(int)((g_font_char_var_3 - j) % (lbl_11_bss_50 / 2) * 2) / (float)lbl_11_bss_50;
                    t = math_sin(32768.0f * t);
                    if ((lbl_11_bss_24->unk1F == 21 || lbl_11_bss_24->unk1F == 22) && j == lbl_11_bss_50 - 1)
                    {
                        ord_tbl_draw_nodes();
                        GXSetZMode_cached(1, 7, 1);
                        fn_11_16318();
                        GXSetZMode_cached(1, 3, 1);
                    }
                    if (lbl_11_bss_24->unk1F != 21)
                    {
                        mtxa_from_mtxb();
                        set_post_mult_color(1.0f, t, 1.0f, 0.5f);
                        mtxa_translate(&lbl_11_bss_198[j]);
                        mtxa_scale_s(1.0f);
                        g_smth_with_gpu_and_locked_cache_mtx();
                        avdisp_draw_model_culled_sort_always(GOLF_MODEL(39));
                    }
                }
            }
            else if (lbl_11_bss_24->unk1F == 21 || lbl_11_bss_24->unk1F == 22)
            {
                ord_tbl_draw_nodes();
                GXSetZMode_cached(1, 7, 1);
                fn_11_16318();
                GXSetZMode_cached(1, 3, 1);
            }
            set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        }
        ord_tbl_draw_nodes();
        avdisp_set_z_mode(1, 3, 1);
        if (lbl_11_bss_90 != 0 && lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14
            && lbl_11_bss_145 == 0 && g_current_camera->unk1F != 7 && g_current_camera->unk1F != 8)
        {
            lbl_11_bss_14 = 0;
            fn_11_114D8();
            fn_11_104B4();
        }
        if (lbl_11_bss_20 != NULL && (lbl_11_bss_20->flags & 0x10) && lbl_11_bss_30 == 0x10)
        {
            lbl_11_bss_14 = 1;
            fn_11_129FC();
            fn_11_104B4();
            lbl_11_bss_198[0] = lbl_11_bss_20->pos;
            lbl_11_bss_60 = 1;
            lbl_11_bss_50 = 0;
        }
        /* scoreboard view in the second camera */
        if (lbl_11_bss_48 == 0 && !fn_11_E9A8() && lbl_11_bss_30 != 0x800 && lbl_11_bss_30 != 0x1000
            && lbl_11_bss_30 != 0x20000 && lbl_11_bss_30 != 0x40000 && lbl_11_bss_30 != 0x40
            && lbl_11_bss_30 != 0x80 && lbl_11_bss_30 != 0x100 && lbl_11_bss_30 != 0x8000
            && lbl_11_bss_30 != 0x10000 && lbl_11_bss_30 != 0x200 && lbl_11_bss_30 != 0x400)
        {
            camera_apply_viewport_if_changed((modeCtrl.currPlayer + 1) % 5);
            push_light_group();
            mtxa_from_mtx(lbl_11_bss_28->unk14C);
            g_set_active_light_group(3);
            if (lbl_11_bss_B00 <= 0)
            {
                avdisp_set_z_mode(1, 7, 1);
                avdisp_set_alpha(0.5f);
                mtxa_from_identity();
                mtxa_translate_xyz(0.0f, 0.0f, -10.0f);
                mtxa_scale_s(4.0f);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[31].model);
                avdisp_set_alpha(1.0f);
                avdisp_set_z_mode(1, 3, 1);
            }
            fn_11_1CF04();
            pop_light_group();
            camera_apply_viewport_if_changed(modeCtrl.currPlayer);
            if (lbl_11_bss_B00 <= 0)
            {
                for (j = 0; j < modeCtrl.playerCount; j++)
                {
                    if (lbl_11_data_258[j] == 0 && (j != modeCtrl.currPlayer || lbl_11_bss_B04 == 0))
                    {
                        set_ui_widescreen_scale_mtx(0x238);
                        fn_11_2EBBC(j);
                        reset_ui_widescreen_scale_mtx();
                    }
                }
                fn_11_2EF2C();
            }
        }
        ord_tbl_draw_nodes();
        handle_test_camera();
        ord_tbl_draw_nodes();
        if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
            g_something_with_rend_efc(8);
    }
    current_ball = ballBackup;
    camera_setup_default_projection();
}
