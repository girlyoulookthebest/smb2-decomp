/* golf.c functions 0x9460-0xD94C: the main draw callback and the next-player choice. */

/* Functions defined in other golf.c fragments (the golf_129FC.inc ones are in golf.h) */
void fn_11_F394(void);
u8 fn_11_E9F0(void);
float fn_11_F2A8(Vec *pos);
void fn_11_E164(void);

/* Functions in the other golf2 TUs */
void fn_11_1A610(struct Ape *ape);
void fn_11_20858(void);

/* DOL / main_loop */
void g_draw_bg(void);
void g_draw_stage(void);
void g_something_with_drawing_apes(void);
void ord_tbl_draw_nodes(void);
void fn_1_120AD8(void);
void *fn_1_1135A4(struct Ape *ape);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_scale(float x, float y);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_alpha(float alpha);
void textdraw_print(char *str);
void set_ui_widescreen_scale_mtx(int width);
void reset_ui_widescreen_scale_mtx(void);
void call_SoundReqID_arg_0(int soundId);
void g_draw_effects(void);
void set_post_mult_color(float r, float g, float b, float a);
void camera_apply_viewport_if_changed(int cameraId);
void camera_setup_default_projection(void);
void set_camera_view_settings(int cameraId, float left, float top, float width, float height);
void u_reset_light_group_stack(int unused);
void mtxa_tf_point(Vec *src, Vec *dst);
float vec_normalize_len(Vec *v);
void mtxa_rotate_x(s16 angle);
void mtxa_to_quat(Quat *q);
/* projects a point through mtxa to screen coordinates (out[0], out[1]) */
void g_math_unk18(Vec *pos, float *out, float tanHalfFov, float aspect);
struct Ape *g_load_ape_gameplay_variant0(int a, int lod);
void g_something_with_freeing_chara_heap_ape(struct Ape *ape);
int sprintf(char *buf, const char *fmt, ...);
int strcmp(const char *, const char *);

extern struct GMA *minigameGma;
extern u32 g_font_char_var_3;
extern u32 g_some_other_flags;
extern s32 locale;
extern s32 active_monkey_id[];
extern u32 g_player_id_for_sound;
extern u32 g_smth_with_sound;

/* XZ distance of a position from the tee */
#define GOLF_TEE_DIST(v) \
    math_sqrt(mathutil_sum_of_sq_3((v).x - stagedef->startPos->pos.x, 0.0f, (v).z - stagedef->startPos->pos.z))

/* Makes player `p` current: their ball, their camera and the next camera. */
#define GOLF_SET_PLAYER(p) \
    do { \
        modeCtrl.currPlayer = (p); \
        lbl_11_bss_20 = &balls[modeCtrl.currPlayer]; \
        lbl_11_bss_24 = &cameras[modeCtrl.currPlayer]; \
        lbl_11_bss_28 = &cameras[(modeCtrl.currPlayer + 1) % 5]; \
    } while (0)

/* Draws the hole: the stage, the aim markers, the club, the shot preview, the
 * other players' markers and the HUD, depending on the golf state. */
void fn_11_9460(void)
{
    u8 n;
    char buf[0x100];
    char buf2[0x100];
    Vec arc0;
    Vec arc1;
    Vec cup;
    Vec cup2;
    Vec cupScreen;
    Vec ballView;
    float screenX;
    float screenY;
    float alpha;
    float scale;
    float sy;
    float sx;
    float adz;
    float adx;
    float dx;
    float dz;
    float dist;
    struct GMAModel *model;
    s32 i;
    s32 x;

    camera_apply_viewport_if_changed(modeCtrl.currPlayer);
    if (lbl_11_bss_47)
    {
        debug_textdraw_set_pos(1, 35);
        debug_textdraw_vsprintf_buf_1("AUTO");
    }

    if (lbl_11_bss_30 == 0x2000)
    {
        if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
            g_draw_bg();
        g_something_with_drawing_apes();
        if (events[EVENT_EFFECT].state == EV_STATE_RUNNING)
            g_draw_effects();
        ord_tbl_draw_nodes();
        return;
    }
    if (lbl_11_bss_30 == 0x4000)
    {
        if (lbl_11_bss_1C != 0)
            fn_1_120AD8();
        return;
    }

    /* replay */
    if (lbl_11_bss_30 == 0x100 || lbl_11_bss_30 == 0x8000 || lbl_11_bss_30 == 0x10000 || lbl_11_bss_30 == 0x200
     || lbl_11_bss_30 == 0x400)
    {
        if (lbl_11_bss_DE == 1)
        {
            set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
            set_camera_view_settings(1, 0.0f, 0.0f, 0.0f, 0.0f);
            set_camera_view_settings(2, 0.0f, 0.0f, 0.0f, 0.0f);
            set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
            cameras[0].flags |= 0x80;
        }
        else if (lbl_11_bss_DE == 2)
        {
            if (lbl_11_bss_30 != 0x400)
            {
                if (!(g_some_other_flags & 8))
                    lbl_11_bss_B50++;
            }
            else if (lbl_11_bss_AFC > 180 && lbl_11_bss_AFC < 240 && !(g_some_other_flags & 8))
                lbl_11_bss_B50--;
            else if (lbl_11_bss_AFC <= 180 && !(g_some_other_flags & 8))
                lbl_11_bss_B50 = 0;
        }
        camera_apply_viewport_if_changed(0);
        if (lbl_11_bss_20->ape != NULL && GOLF_TEE_DIST(lbl_11_bss_20->pos) <= 0.1 && !lbl_11_bss_90)
        {
            mtxa_from_mtx(cameras[0].unk14C);
            mtxa_translate_xyz(lbl_11_bss_20->ape->pos.x, lbl_11_bss_20->ape->pos.y - 0.12, lbl_11_bss_20->ape->pos.z);
            mtxa_rotate_y(lbl_11_bss_20->unk92);
            mtxa_mult_right(fn_1_1135A4(lbl_11_bss_20->ape));
            mtxa_scale_s(0.1f);
            mtxa_mult_right(lbl_11_bss_98);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            if ((lbl_11_bss_20->flags & 0x10) == 0)
                avdisp_draw_model_unculled_sort_never(minigameGma->modelEntries[lbl_11_data_260[lbl_11_bss_B34]].model);
        }
        fn_11_F394();
        camera_apply_viewport_if_changed(0);
        camera_setup_default_projection();
        textdraw_reset();
        textdraw_set_font(9);
        textdraw_set_scale(0.4f, 0.4f);
        textdraw_set_mul_color(0xFFFF);
        if (lbl_11_bss_DE != 2 || lbl_11_bss_30 != 0x400 || lbl_11_bss_AFC > 240)
        {
            textdraw_set_pos(440.0f, 430.0f);
            if (g_font_char_var_3 % 60 > 30)
                textdraw_print("REPLAY");
        }
        ord_tbl_draw_nodes();
        return;
    }

    /* hole intro: "HOLE n" and "PAR n" */
    if (lbl_11_bss_24->unk1F == 11)
    {
        if (lbl_11_bss_B00 > 720)
        {
            sy = 1.0 + math_sin(65535.0f * ((float)(900 - lbl_11_bss_B00) / 36.0f) - 32768.0f)
                / (1.0f + (float)(900 - lbl_11_bss_B00) * (float)(900 - lbl_11_bss_B00) / 25.0f);
            sx = sy;
            textdraw_reset();
            textdraw_set_font(9);
            textdraw_set_scale(sx, sy);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(320.0f - 3.0f * (72.0f * sx), 240.0f - 96.0f * sy);
            textdraw_print("HOLE");
            if (lbl_11_bss_C8 + 1 >= 10)
            {
                sprintf(buf, "%d", lbl_11_bss_C8 + 1);
                textdraw_set_mul_color(0xFFFF00);
                textdraw_set_pos(320.0f + 72.0f * sx, 240.0f - 96.0f * sy);
                textdraw_print(buf);
            }
            else
            {
                sprintf(buf, "%d", lbl_11_bss_C8 + 1);
                textdraw_set_mul_color(0xFFFF00);
                textdraw_set_pos(320.0f + 2.0f * (72.0f * sx), 240.0f - 96.0f * sy);
                textdraw_print(buf);
            }
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(320.0f - 2.0f * (72.0f * sx), 240.0f + 32.0f * sy);
            sprintf(buf, "PAR %d", (s8)fn_11_16FA4());
            textdraw_print(buf);
        }
        else if (lbl_11_bss_B00 > 600)
        {
            sy = 1.0 / (1.0 + (720 - lbl_11_bss_B00) / 120.0);
            sx = sy;
            textdraw_reset();
            textdraw_set_font(9);
            textdraw_set_scale(sx, sy);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(104.0f * sx, 144.0f * (sy * sy));
            textdraw_print("HOLE");
            if (lbl_11_bss_C8 + 1 >= 10)
            {
                sprintf(buf, "%d", lbl_11_bss_C8 + 1);
                textdraw_set_mul_color(0xFFFF00);
                textdraw_set_pos(392.0f * sx, 144.0f * (sy * sy));
                textdraw_print(buf);
            }
            else
            {
                sprintf(buf, "%d", lbl_11_bss_C8 + 1);
                textdraw_set_mul_color(0xFFFF00);
                textdraw_set_pos(464.0f * sx, 144.0f * (sy * sy));
                textdraw_print(buf);
            }
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(176.0f * sx, 272.0f * (sy * sy));
            sprintf(buf, "PAR %d", (s8)fn_11_16FA4());
            textdraw_print(buf);
        }
        else
        {
            sx = 0.5f;
            sy = 0.5f;
            textdraw_reset();
            textdraw_set_font(9);
            textdraw_set_scale(sx, sy);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(104.0f * sx, 144.0f * (sy * sy));
            textdraw_print("HOLE");
            if (lbl_11_bss_C8 + 1 >= 10)
            {
                sprintf(buf, "%d", lbl_11_bss_C8 + 1);
                textdraw_set_mul_color(0xFFFF00);
                textdraw_set_pos(392.0f * sx, 144.0f * (sy * sy));
                textdraw_print(buf);
            }
            else
            {
                sprintf(buf, "%d", lbl_11_bss_C8 + 1);
                textdraw_set_mul_color(0xFFFF00);
                textdraw_set_pos(464.0f * sx, 144.0f * (sy * sy));
                textdraw_print(buf);
            }
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_pos(176.0f * sx, 272.0f * (sy * sy));
            sprintf(buf, "PAR %d", (s8)fn_11_16FA4());
            textdraw_print(buf);
        }
    }

    /* course flyover: the back wall models are drawn on their own */
    if (lbl_11_bss_30 == 0x1000 && lbl_11_bss_13C > 60 && lbl_11_bss_140 < 60)
    {
        if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
            g_draw_bg();
        g_draw_stage();
        for (n = 0; n < 20; n++)
        {
            sprintf(buf2, "NODISP_BACK_WALL_N%d", n + 1);
            model = fn_11_0(stage_gma, buf2);
            if (model != NULL)
            {
                mtxa_from_mtx(lbl_11_bss_24->unk14C);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_always(model);
            }
        }
        ord_tbl_draw_nodes();
        fn_11_1D250(1);
        ord_tbl_draw_nodes();
        return;
    }

    fn_11_1D250(0);
    if (lbl_11_bss_30 == 0x20000 || lbl_11_bss_30 == 0x40000)
    {
        fn_11_F394();
        ord_tbl_draw_nodes();
        return;
    }

    /* Z toggles the overhead map while aiming */
    if (lbl_11_bss_30 == 8 && 0.0 == lbl_11_bss_B0C && lbl_11_bss_AFC <= 0 && lbl_11_bss_AF8 <= 0)
    {
        if (!(g_some_other_flags & 0xA))
        {
            if (!lbl_11_bss_48 && (pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x10)
             && lbl_11_bss_24->unk1F != 12)
            {
                call_SoundReqID_arg_0(0xCE);
                lbl_11_bss_48 = 1;
                fn_11_1D250(1);
            }
            else if (lbl_11_bss_48
                  && ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x100)
                   || (pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x200)
                   || (pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x400)
                   || (pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x800)
                   || (pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[2].button & 0x10)))
            {
                call_SoundReqID_arg_0(0xCE);
                fn_11_1D250(0);
                lbl_11_bss_48 = 0;
            }
        }
        if (lbl_11_bss_48)
            fn_11_1D250(1);
    }

    /* the club in the ape's hands */
    if ((lbl_11_bss_B0C <= 0.0f || lbl_11_bss_88 >= 10) && !lbl_11_bss_90 && lbl_11_bss_20->ape != NULL)
    {
        mtxa_from_mtxb();
        mtxa_translate_xyz(lbl_11_bss_20->ape->pos.x, lbl_11_bss_20->ape->pos.y - 0.12, lbl_11_bss_20->ape->pos.z);
        mtxa_rotate_y(lbl_11_bss_20->unk92);
        mtxa_mult_right(fn_1_1135A4(lbl_11_bss_20->ape));
        mtxa_scale_s(0.1f);
        mtxa_mult_right(lbl_11_bss_98);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        if ((lbl_11_bss_20->flags & 0x10) == 0)
            avdisp_draw_model_unculled_sort_never(minigameGma->modelEntries[lbl_11_data_260[lbl_11_bss_4C]].model);
    }

    /* the flag marker with the hole number */
    if (lbl_11_bss_24->unk1F == 2 || lbl_11_bss_24->unk1F == 20 || lbl_11_bss_24->unk1F == 21
     || lbl_11_bss_24->unk1F == 22 || lbl_11_bss_24->unk1F == 5 || lbl_11_bss_24->unk1F == 3
     || lbl_11_bss_24->unk1F == 4 || lbl_11_bss_24->unk1F == 11)
    {
        dx = lbl_11_bss_24->eye.x - lbl_11_bss_38.x;
        dz = lbl_11_bss_24->eye.z - lbl_11_bss_38.z;
        avdisp_set_z_mode(1, 3, 1);
        mtxa_from_mtxb();
        mtxa_translate(&lbl_11_bss_38);
        mtxa_rotate_y(math_atan2(dx, dz));
        if (lbl_11_bss_CA != 3)
        {
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            switch (lbl_11_bss_C8)
            {
            case 0: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[2].model); break;
            case 1: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[3].model); break;
            case 2: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[4].model); break;
            case 3: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[5].model); break;
            case 4: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[6].model); break;
            case 5: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[7].model); break;
            case 6: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[8].model); break;
            case 7: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[9].model); break;
            case 8: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[10].model); break;
            case 9: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[11].model); break;
            case 10: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[12].model); break;
            case 11: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[13].model); break;
            case 12: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[14].model); break;
            case 13: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[15].model); break;
            case 14: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[16].model); break;
            case 15: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[17].model); break;
            case 16: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[18].model); break;
            case 17: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[19].model); break;
            }
        }
        else if (lbl_11_bss_24->unk1F == 2 || lbl_11_bss_24->unk1F == 20)
        {
            avdisp_set_alpha(0.5f);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            switch (lbl_11_bss_C8)
            {
            case 0: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[2].model); break;
            case 1: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[3].model); break;
            case 2: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[4].model); break;
            case 3: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[5].model); break;
            case 4: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[6].model); break;
            case 5: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[7].model); break;
            case 6: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[8].model); break;
            case 7: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[9].model); break;
            case 8: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[10].model); break;
            case 9: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[11].model); break;
            case 10: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[12].model); break;
            case 11: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[13].model); break;
            case 12: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[14].model); break;
            case 13: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[15].model); break;
            case 14: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[16].model); break;
            case 15: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[17].model); break;
            case 16: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[18].model); break;
            case 17: avdisp_draw_model_unculled_sort_always(minigameGma->modelEntries[19].model); break;
            }
        }
    }

    /* the aim arrow at the ball, coloured by club */
    if ((lbl_11_bss_24->unk1F == 2 || lbl_11_bss_24->unk1F == 20 || lbl_11_bss_24->unk1F == 21
      || lbl_11_bss_24->unk1F == 22)
     && !lbl_11_bss_145 && !lbl_11_bss_90 && lbl_11_bss_30 == 8)
    {
        u_reset_light_group_stack(modeCtrl.currPlayer);
        mtxa_from_mtxb();
        mtxa_translate(&lbl_11_bss_20->pos);
        mtxa_rotate_y(lbl_11_bss_20->unk92);
        mtxa_rotate_x(lbl_11_data_180[lbl_11_bss_4C]);
        if (lbl_11_bss_4C < 3)
        {
            mtxa_translate_xyz(0.0f, 0.0f, -0.5f - 1.5f * (float)(g_font_char_var_3 % 30) / 30.0f);
            set_post_mult_color(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else if (lbl_11_bss_4C < 6)
        {
            mtxa_translate_xyz(0.0f, 0.0f, -0.5f - 0.8f * (float)(g_font_char_var_3 % 40) / 40.0f);
            set_post_mult_color(1.0f, 0.7f, 0.0f, 1.0f);
        }
        else if (lbl_11_bss_4C < 9)
        {
            mtxa_translate_xyz(0.0f, 0.0f, -0.5f - 0.6f * (float)(g_font_char_var_3 % 50) / 50.0f);
            set_post_mult_color(0.4f, 1.0f, 0.4f, 1.0f);
        }
        else
        {
            mtxa_translate_xyz(0.0f, 0.0f, -0.5f - 0.4f * (float)(g_font_char_var_3 % 60) / 60.0f);
            set_post_mult_color(0.6f, 0.6f, 1.0f, 1.0f);
        }
        switch (lbl_11_bss_4C)
        {
        case 0: set_post_mult_color(1.0f, 0.0f, 0.0f, 1.0f); break;
        case 1: set_post_mult_color(1.0f, 0.9f, 0.0f, 1.0f); break;
        case 2: set_post_mult_color(1.0f, 0.8f, 0.0f, 1.0f); break;
        case 3: set_post_mult_color(1.0f, 0.7f, 0.0f, 1.0f); break;
        case 4: set_post_mult_color(0.8f, 0.8f, 0.1f, 1.0f); break;
        case 5: set_post_mult_color(0.6f, 0.9f, 0.2f, 1.0f); break;
        case 6: set_post_mult_color(0.4f, 1.0f, 0.4f, 1.0f); break;
        case 7: set_post_mult_color(0.46f, 0.86f, 0.6f, 1.0f); break;
        case 8: set_post_mult_color(0.52f, 0.73f, 0.8f, 1.0f); break;
        case 9: set_post_mult_color(0.6f, 0.6f, 1.0f, 1.0f); break;
        case 10: set_post_mult_color(0.5f, 0.6f, 1.0f, 1.0f); break;
        case 11: set_post_mult_color(0.4f, 0.6f, 1.0f, 1.0f); break;
        default: set_post_mult_color(0.3f, 0.6f, 1.0f, 1.0f); break;
        }
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(minigameGma->modelEntries[23].model);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    /* dots from the ball back to where the last shot started */
    if (lbl_11_bss_24->unk1F == 21 && !lbl_11_bss_145 && !lbl_11_bss_90 && lbl_11_bss_CA != 3
     && lbl_11_bss_4C != 12 && lbl_11_bss_4C != 13 && lbl_11_bss_4C != 14 && lbl_11_bss_30 == 8
     && lbl_11_bss_C >= 0)
    {
        for (i = 0; i < 8.0; i++)
        {
            mtxa_from_mtxb();
            mtxa_translate_xyz(
                (i * lbl_11_bss_198[lbl_11_bss_50 - 1].x) / 10.0 + (lbl_11_bss_20->pos.x * (float)(10.0 - i)) / 10.0,
                (i * lbl_11_bss_198[lbl_11_bss_50 - 1].y) / 10.0 + (lbl_11_bss_20->pos.y * (float)(10.0 - i)) / 10.0,
                (i * lbl_11_bss_198[lbl_11_bss_50 - 1].z) / 10.0 + (lbl_11_bss_20->pos.z * (float)(10.0 - i)) / 10.0);
            alpha = math_sin(16384.0 + 32768.0 * ((float)i / 10.0 - 2.0f * lbl_11_bss_C / 120.0f));
            alpha = fn_11_16ACC(alpha, 20);
            set_post_mult_color(1.0f, 0.5f, 0.8f, alpha);
            scale = math_sqrt(mathutil_sum_of_sq_3(
                cameras[modeCtrl.currPlayer].eye.x - cameras[modeCtrl.currPlayer].lookAt.x,
                cameras[modeCtrl.currPlayer].eye.y - cameras[modeCtrl.currPlayer].lookAt.y,
                cameras[modeCtrl.currPlayer].eye.z - cameras[modeCtrl.currPlayer].lookAt.z)) / 15.0;
            mtxa_scale_s(scale);
            mtxa_translate_xyz(0.0f, 0.5f, 0.0f);
            mtxa_rotate_y(lbl_11_bss_20->unk92);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_always(minigameGma->modelEntries[23].model);
            set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);

    /* the marker running along the putting line */
    if ((lbl_11_bss_24->unk1F == 21 || lbl_11_bss_24->unk1F == 20) && !lbl_11_bss_145 && !lbl_11_bss_90
     && lbl_11_bss_CA == 3 && lbl_11_bss_30 == 8)
    {
        fn_11_15FB4(&arc0, lbl_11_bss_54, lbl_11_bss_B54, 120, lbl_11_bss_8);
        fn_11_15FB4(&arc1, lbl_11_bss_54, lbl_11_bss_B54, 120, lbl_11_bss_8 + 1);
        adx = arc0.x - arc1.x;
        adz = arc0.z - arc1.z;
        mtxa_from_mtxb();
        mtxa_translate(&arc0);
        set_post_mult_color(1.0f, 0.5f, 0.8f, fn_11_16B2C(2, math_sin(32768.0f * lbl_11_bss_8 / 120.0f)));
        scale = math_sqrt(mathutil_sum_of_sq_3(
            cameras[modeCtrl.currPlayer].eye.x - cameras[modeCtrl.currPlayer].lookAt.x,
            cameras[modeCtrl.currPlayer].eye.y - cameras[modeCtrl.currPlayer].lookAt.y,
            cameras[modeCtrl.currPlayer].eye.z - cameras[modeCtrl.currPlayer].lookAt.z)) / 15.0;
        mtxa_scale_s(scale);
        mtxa_translate_xyz(0.0f, 0.5f, 0.0f);
        mtxa_rotate_y(math_atan2(adx, adz));
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_always(minigameGma->modelEntries[23].model);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    fn_11_F394();
    camera_apply_viewport_if_changed(modeCtrl.currPlayer);
    avdisp_set_z_mode(1, 3, 1);

    if (lbl_11_bss_B00 <= 0 && lbl_11_bss_30 != 0x40 && lbl_11_bss_30 != 0x80 && !lbl_11_bss_48 && !fn_11_E9A8())
        fn_11_1F7FC(lbl_11_bss_C8 + 1);

    /* "Player n" banner and, in match play, the win/lose caption */
    if (lbl_11_bss_AF8 > 0 && lbl_11_bss_B00 <= 0)
    {
        if (lbl_11_bss_AF8 == 178 && modeCtrl.playerCount != 1)
        {
            if (modeCtrl.currPlayer == 0)
                call_SoundReqID_arg_0(0x1EA);
            else if (modeCtrl.currPlayer == 1)
                call_SoundReqID_arg_0(0x1EB);
            else if (modeCtrl.currPlayer == 2)
                call_SoundReqID_arg_0(0x1EC);
            else if (modeCtrl.currPlayer == 3)
                call_SoundReqID_arg_0(0x1ED);
        }
        if (lbl_11_bss_AF8 == 118)
            call_SoundReqID_arg_0(0x1F5);
        set_ui_widescreen_scale_mtx(320);
        fn_11_1E104(lbl_11_bss_AF8);
        reset_ui_widescreen_scale_mtx();
        if (lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8] == 0 && lbl_1_bss_1704.unk0 == 0)
            fn_11_1E794(lbl_11_bss_AF8);
        else if (lbl_1_bss_1704.unk0 == 1)
        {
            if (lbl_11_bss_AF8 > 150.0)
                x = -320.0 + 640.0f * (180 - lbl_11_bss_AF8) * (180 - lbl_11_bss_AF8) / 30.0 / 30.0;
            else if (lbl_11_bss_AF8 > 90.0)
                x = 320;
            else if (lbl_11_bss_AF8 > 60)
                x = 320.0 + 640.0f * (float)(90.0 - lbl_11_bss_AF8) * (90.0 - lbl_11_bss_AF8) / 30.0 / 30.0;
            else
                x = 960;
            set_ui_widescreen_scale_mtx(320);
            if (locale == 5)
            {
                if (lbl_11_bss_E8 == 1)
                    /* "Sink this and you win" */
                    fn_11_1E4C0(x, 400, 11, "\x82\xb1\x82\xea\x82\xf0\x82\xa2\x82\xea\x82\xe9\x82\xc6\x82\xa9\x82\xbf\x82\xc5\x82\xb7");
                else if (lbl_11_bss_E8 == 2)
                    /* "Miss this and you lose" */
                    fn_11_1E4C0(x, 400, 12, "\x82\xb1\x82\xea\x82\xf0\x82\xa2\x82\xea\x82\xc8\x82\xa2\x82\xc6\x82\xdc\x82\xaf\x82\xc5\x82\xb7");
            }
            else if (locale == 0)
            {
                if (lbl_11_bss_E8 == 1)
                    fn_11_1E4C0(x, 400, 11, "If you put this in, you win.");
                else if (lbl_11_bss_E8 == 2)
                    fn_11_1E4C0(x, 400, 14, "If you can't put this in, you lose.");
            }
            reset_ui_widescreen_scale_mtx();
        }
    }

    if (lbl_11_bss_30 != 0x40 && lbl_11_bss_30 != 0x80)
    {
        /* stroke counter */
        if ((lbl_11_bss_30 == 8 || lbl_11_bss_30 == 0x10 || lbl_11_bss_30 == 0x20 || lbl_11_bss_30 == 2
          || lbl_11_bss_30 == 4)
         && lbl_11_bss_24->unk1F != 12 && lbl_11_bss_24->unk1F != 11 && !lbl_11_bss_48)
        {
            set_ui_widescreen_scale_mtx(65);
            if (lbl_11_bss_90)
                fn_11_1FA7C(lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8]);
            else
                fn_11_1FA7C(lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8] + 1);
            reset_ui_widescreen_scale_mtx();
        }
        /* power gauge */
        if (lbl_11_bss_30 == 8 && 0.0 == lbl_11_bss_B0C && lbl_11_bss_24->unk1F != 12)
        {
            if (lbl_11_bss_AFC <= 0 && lbl_11_bss_AF8 <= 0)
            {
                set_ui_widescreen_scale_mtx(320);
                if (!lbl_11_bss_48)
                    fn_11_2E638(&lbl_11_bss_34);
                reset_ui_widescreen_scale_mtx();
            }
            else if (!lbl_11_bss_90)
            {
                set_ui_widescreen_scale_mtx(320);
                fn_11_2E350(&lbl_11_bss_34, lbl_11_bss_AFC / 180.0f);
                reset_ui_widescreen_scale_mtx();
            }
        }
        if (lbl_11_bss_30 == 0x20 && lbl_11_bss_AFC <= 0 && lbl_11_bss_AF8 <= 0)
        {
            set_ui_widescreen_scale_mtx(320);
            fn_11_2DE64(&lbl_11_bss_34, lbl_11_bss_B1C);
            reset_ui_widescreen_scale_mtx();
        }
        if (lbl_11_bss_B0C <= 0.0f && lbl_11_bss_90)
        {
            set_ui_widescreen_scale_mtx(320);
            fn_11_2E068(&lbl_11_bss_34, lbl_11_bss_B1C, (180 - lbl_11_bss_AFC) / 180.0f);
            reset_ui_widescreen_scale_mtx();
        }
        else if (lbl_11_bss_30 == 0x10 || lbl_11_bss_24->unk1F == 3 || lbl_11_bss_24->unk1F == 4
              || lbl_11_bss_24->unk1F == 6 || lbl_11_bss_24->unk1F == 1)
        {
            set_ui_widescreen_scale_mtx(320);
            fn_11_2DE64(&lbl_11_bss_34, lbl_11_bss_B1C);
            reset_ui_widescreen_scale_mtx();
        }
        if ((lbl_11_bss_30 == 0x20 || lbl_11_bss_30 == 8 || lbl_11_bss_30 == 0x10 || lbl_11_bss_30 == 2
          || lbl_11_bss_30 == 4)
         && lbl_11_bss_24->unk1F != 12 && lbl_11_bss_24->unk1F != 11 && !lbl_11_bss_48)
        {
            set_ui_widescreen_scale_mtx(65);
            fn_11_20858();
            reset_ui_widescreen_scale_mtx();
        }
    }
    else
    {
        textdraw_set_scale(2.0f, 2.0f);
        textdraw_set_alpha(1.0f);
        textdraw_set_mul_color(0xFFFF00);
        if (lbl_11_bss_30 == 0x40)
        {
            set_ui_widescreen_scale_mtx(320);
            fn_11_1EC9C(fn_11_16FD8(), lbl_11_bss_E0);
            reset_ui_widescreen_scale_mtx();
        }
        else
        {
            set_ui_widescreen_scale_mtx(320);
            fn_11_1EC9C(50, lbl_11_bss_E0);
            reset_ui_widescreen_scale_mtx();
        }
    }
    if (lbl_11_bss_B04 != 0)
    {
        set_ui_widescreen_scale_mtx(320);
        fn_11_1EC9C(99, lbl_11_bss_E0);
        reset_ui_widescreen_scale_mtx();
    }

    /* markers over the other players' balls */
    if (lbl_11_bss_24->unk1F != 11 && lbl_11_bss_24->unk1F != 7 && lbl_11_bss_24->unk1F != 8
     && lbl_11_bss_30 != 0x100 && lbl_11_bss_30 != 0x8000 && lbl_11_bss_30 != 0x10000 && lbl_11_bss_30 != 0x200
     && lbl_11_bss_30 != 0x400)
    {
        avdisp_set_z_mode(1, 3, 1);
        for (i = 0; i < modeCtrl.playerCount; i++)
        {
            if (((i != modeCtrl.currPlayer && lbl_11_data_258[i] == 0 && GOLF_TEE_DIST(balls[i].pos) > 0.1)
              || (i == modeCtrl.currPlayer && lbl_11_bss_24->unk1F == 22))
             && cameras[modeCtrl.currPlayer].unk1F != 21)
            {
                dist = math_sqrt(mathutil_sum_of_sq_3(balls[i].pos.x - lbl_11_bss_24->eye.x, 0.0f,
                                                      balls[i].pos.z - lbl_11_bss_24->eye.z));
                mtxa_from_mtx(lbl_11_bss_24->unk14C);
                mtxa_translate_xyz(balls[i].pos.x,
                                   balls[i].pos.y + dist * math_tan(lbl_11_bss_24->sub28.view.fov) / 8.0f,
                                   balls[i].pos.z);
                mtxa_rotate_y(g_font_char_var_3 << 8);
                mtxa_scale_s(0.096225 * dist * math_tan(lbl_11_bss_24->sub28.view.fov) / 4.0);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                if (i == 0)
                    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[1].model);
                else if (i == 1)
                    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[3].model);
                else if (i == 2)
                    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[5].model);
                else if (i == 3)
                    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[7].model);
                mtxa_from_mtx(lbl_11_bss_24->unk14C);
                mtxa_translate_xyz(balls[i].pos.x,
                                   balls[i].pos.y + dist * math_tan(lbl_11_bss_24->sub28.view.fov) / 8.0f,
                                   balls[i].pos.z);
                mtxa_rotate_y(g_font_char_var_3 << 8);
                mtxa_rotate_y(-0x8000);
                mtxa_scale_s(0.096225 * dist * math_tan(lbl_11_bss_24->sub28.view.fov) / 4.0);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                if (i == 0)
                    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[1].model);
                else if (i == 1)
                    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[3].model);
                else if (i == 2)
                    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[5].model);
                else if (i == 3)
                    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[7].model);
            }
        }
    }

    if (lbl_11_bss_30 != 0x40 && lbl_11_bss_30 != 0x80
     && (lbl_11_bss_30 == 8 || lbl_11_bss_30 == 0x10 || lbl_11_bss_30 == 0x20 || lbl_11_bss_30 == 2
      || lbl_11_bss_30 == 4)
     && lbl_11_bss_24->unk1F != 12 && lbl_11_bss_24->unk1F != 11 && !lbl_11_bss_48)
        fn_11_13630();

    /* the pointer to the cup when it is behind the camera */
    if (lbl_11_bss_30 == 8 && 0.0 == lbl_11_bss_B0C && lbl_11_bss_24->unk1F == 2
     && (fn_11_F2A8(&lbl_11_bss_20->pos) > 100.0f || lbl_11_bss_7C != 0) && lbl_11_bss_AFC <= 0
     && lbl_11_bss_AF8 <= 0)
    {
        cup.x = lbl_11_bss_38.x;
        cup.y = 0.0f;
        cup.z = lbl_11_bss_38.z;
        mtxa_from_mtxb();
        mtxa_tf_point(&cup, &cup);
        vec_normalize_len(&cup);
        if (cup.z < -0.1)
        {
            fn_11_139C8(cup.x, 0.0f, -cup.z, &screenX, &screenY, lbl_1_bss_2440);
            screenX = screenX / (lbl_1_bss_2434 / 640.0f);
            screenY = screenY / ((lbl_1_bss_2434 / g_camera_aspect_ratio) / 480.0f);
            fn_11_2EDB4((s32)screenX - 8, 50.0f + (float)(g_font_char_var_3 & 0x1F) / 5.0f);
        }
    }

    /* the pointer to the cup on screen */
    if (lbl_11_bss_30 == 8 && 0.0 == lbl_11_bss_B0C && lbl_11_bss_24->unk1F == 21
     && fn_11_F2A8(&lbl_11_bss_20->pos) > 100.0f && lbl_11_bss_AFC <= 0 && lbl_11_bss_AF8 <= 0)
    {
        mtxa_push();
        cup2.x = lbl_11_bss_38.x;
        cup2.y = lbl_11_bss_38.y;
        cup2.z = lbl_11_bss_38.z;
        mtxa_from_mtxb();
        g_math_unk18(&cup2, &cupScreen.x, lbl_1_bss_2440, lbl_11_bss_24->sub28.view.aspect);
        if (10000.0f != cupScreen.x && 10000.0f != cupScreen.y)
        {
            set_ui_widescreen_scale_mtx(cupScreen.x);
            fn_11_2EDB4(cupScreen.x, (s32)cupScreen.y - 16);
            reset_ui_widescreen_scale_mtx();
        }
        mtxa_pop();
    }

    if (lbl_11_bss_30 == 8 && 0.0 == lbl_11_bss_B0C && (lbl_11_bss_24->unk1F == 21 || lbl_11_bss_24->unk1F == 22))
    {
        if (math_sqrt(mathutil_sum_of_sq_3(lbl_11_bss_20->pos.x - lbl_11_bss_24->eye.x,
                                           lbl_11_bss_20->pos.y - lbl_11_bss_24->eye.y,
                                           lbl_11_bss_20->pos.z - lbl_11_bss_24->eye.z)) > 50.0f
         && !lbl_11_bss_48
         && lbl_11_bss_AFC <= 0 && lbl_11_bss_AF8 <= 0)
        {
            ballView.x = lbl_11_bss_20->pos.x;
            ballView.y = 0.0f;
            ballView.z = lbl_11_bss_20->pos.z;
            mtxa_from_mtxb();
            mtxa_tf_point(&ballView, &ballView);
            vec_normalize_len(&ballView);
            if (0.0 != ballView.z)
                fn_11_3071C();
        }
    }

    if ((lbl_11_bss_90 && lbl_11_bss_B0C < 0.1) || lbl_11_bss_B04 != 0)
    {
        fn_11_13C08();
        lbl_11_bss_6C = 1;
    }
    avdisp_set_z_mode(1, 3, 1);

    if (lbl_11_bss_6C == 0 && lbl_11_bss_74 != 0)
    {
        fn_11_13C08();
        lbl_11_bss_6C++;
        if (lbl_11_bss_78 == 0)
            lbl_11_bss_78 = 120;
    }
    else if (lbl_11_bss_78 > lbl_11_bss_6C && lbl_11_bss_6C > 0 && lbl_11_bss_74 != 0)
    {
        fn_11_13CBC();
        if (lbl_11_bss_74 == 1)
            fn_11_158B8();
        else if (lbl_11_bss_74 == 2)
            fn_11_15248();
        if (!(g_some_other_flags & 0xA))
            lbl_11_bss_6C++;
    }
    else if (lbl_11_bss_6C == lbl_11_bss_78 && lbl_11_bss_74 != 0)
    {
        lbl_11_bss_74 = 0;
        lbl_11_bss_6C = 0;
        lbl_11_bss_78 = 0;
    }

    if (lbl_11_bss_6C == 0 && lbl_11_bss_70 != 0)
        lbl_11_bss_6C++;
    else if (90.0f > lbl_11_bss_6C && lbl_11_bss_6C > 0 && lbl_11_bss_70 != 0)
    {
        fn_11_13C08();
        fn_11_13CBC();
        fn_11_13E6C();
        if (!(g_some_other_flags & 0xA))
            lbl_11_bss_6C++;
    }
    else if (90.0f == lbl_11_bss_6C && lbl_11_bss_70 != 0)
    {
        lbl_11_bss_70 = 0;
        lbl_11_bss_6C = 0;
    }
    ord_tbl_draw_nodes();
}

/* Picks who plays next: in stroke play the player furthest from the cup, in
 * match play the other player. Once everyone has holed out it moves to the next
 * hole (the best score on the last hole tees off first) and puts every ball on the
 * tee. Reloads the ape of the new current player and resets its ball. */
void fn_11_C628(void)
{
    u32 score[4];
    u16 score2[4];
    u8 rank[4];
    u8 order[4];
    u8 rank2[4];
    u8 order2[4];
    s32 reloadApe;
    u32 next;
    float best;
    s32 i;
    s32 j;
    s32 r;
    s32 cnt;
    s32 min;
    u32 tmp;
    s32 i2;
    s32 j2;
    s32 r2;
    s32 cnt2;
    s8 bestRank;
    u16 tmp16;
    u32 k;
    s8 *status;

    reloadApe = TRUE;
    lbl_11_bss_84 = 0;
    status = &g_poolInfo.playerPool.statusList[modeCtrl.currPlayer];
    if (*status == 2)
    {
        *status = 4;
        lbl_11_bss_20->status = 4;
    }
    best = 0.0f;
    next = 99;
    if (fn_11_E9F0())
    {
        if (lbl_1_bss_1704.unk0 == 1)
        {
            if (modeCtrl.currPlayer == 0)
                next = 1;
            else
                next = 0;
            if (GOLF_TEE_DIST(balls[next].pos) > 0.01 && lbl_11_data_29C[next][lbl_11_bss_C8] != 0)
                next = (next + 1) & 1;
        }
        else if (lbl_1_bss_1704.unk0 == 0)
        {
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                score[i] = 0;
                order[i] = i;
                if (lbl_11_bss_C8 > 0)
                    score[i] = lbl_11_data_29C[i][lbl_11_bss_C8 - 1];
                else
                    score[i] = 0;
                if (GOLF_TEE_DIST(balls[i].pos) > 0.01 || lbl_11_data_258[i] != 0)
                    score[i] += 1000;
            }
            /* j: someone has not played this hole yet */
            j = FALSE;
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (lbl_11_data_29C[i][lbl_11_bss_C8] == 0)
                    j = TRUE;
            }
            if (!j)
            {
                for (i = 0; i < modeCtrl.playerCount; i++)
                {
                    if (score[i] < 1000)
                        score[i] = lbl_11_data_29C[i][lbl_11_bss_C8];
                }
            }
            else
            {
                for (i = 0; i < modeCtrl.playerCount; i++)
                {
                    if (lbl_11_data_29C[i][lbl_11_bss_C8] != 0)
                        score[i] += 1000;
                }
            }
            min = 1000;
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (score[i] < 1000 && score[i] < min)
                    min = score[i];
            }
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (min == score[i])
#ifdef __MWERKS__
                    score[i] = lbl_11_data_29C[i][lbl_11_bss_C8 - 1];
#else
                    /* on the first hole this reads [i][-1]: the previous
                     * player's row, or for player 0 the byte before the
                     * table on GC, the low byte of big-endian
                     * lbl_11_data_260[14] */
                    score[i] = (lbl_11_bss_C8 == 0 && i == 0) ? (u8)lbl_11_data_260[14]
                                                              : (&lbl_11_data_29C[0][0])[i * 18 + lbl_11_bss_C8 - 1];
#endif
                else
                    score[i] = 1000;
            }
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                for (j = i; j < modeCtrl.playerCount; j++)
                {
                    if (score[i] > score[j])
                    {
                        tmp = score[i];
                        score[i] = score[j];
                        score[j] = tmp;
                        tmp = order[i];
                        order[i] = order[j];
                        order[j] = tmp;
                    }
                }
            }
            r = 0;
            cnt = 1;
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                rank[order[i]] = r;
                if (score[i] != score[i + 1])
                {
                    r += cnt;
                    cnt = 1;
                }
                else
                    cnt++;
            }
            min = 9;
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (rank[i] == 0 && min > i)
                {
                    next = i;
                    min = i;
                }
            }
        }
    }
    else
    {
        for (k = 0; k < modeCtrl.playerCount; k++)
        {
            if (GOLF_TEE_DIST(balls[k].pos) < 0.01)
            {
                balls[k].pos.x = stagedef->startPos->pos.x;
                balls[k].pos.z = stagedef->startPos->pos.z;
            }
            if (fn_11_F2A8(&balls[k].pos) - best > 0.01 && lbl_11_data_258[k] == 0)
            {
                best = fn_11_F2A8(&balls[k].pos);
                next = k;
            }
        }
    }
    if (next == 99)
        next = 0;

    if (!fn_11_E9A8() && next != modeCtrl.currPlayer)
    {
        GOLF_SET_PLAYER(next);
        g_player_id_for_sound = modeCtrl.currPlayer;
        g_smth_with_sound = active_monkey_id[g_player_id_for_sound];
        call_SoundReqID_arg_0(0x1E);
    }
    else
    {
        reloadApe = FALSE;
        GOLF_SET_PLAYER(next);
    }

    /* everyone has holed out: next hole */
    if (fn_11_E9A8())
    {
        for (k = 0; k < modeCtrl.playerCount; k++)
            lbl_11_data_258[k] = 0;
        lbl_11_bss_C8++;
        if (lbl_1_bss_1704.unk0 == 1)
        {
            /* match play: whoever won the last decided hole goes first */
            if (lbl_11_bss_C8 == 0)
                GOLF_SET_PLAYER(0);
            else
            {
                for (i2 = lbl_11_bss_C8 - 1; i2 >= 0; i2--)
                {
                    if (lbl_11_data_29C[0][i2] > lbl_11_data_29C[1][i2])
                    {
                        GOLF_SET_PLAYER(1);
                        break;
                    }
                    if (lbl_11_data_29C[0][i2] < lbl_11_data_29C[1][i2])
                    {
                        GOLF_SET_PLAYER(0);
                        break;
                    }
                    if (i2 == 0)
                    {
                        GOLF_SET_PLAYER(0);
                        break;
                    }
                }
            }
        }
        else if (lbl_1_bss_1704.unk0 == 0)
        {
            /* stroke play: the best score on the last hole goes first */
            for (i2 = 0; i2 < modeCtrl.playerCount; i2++)
            {
                score2[i2] = 0;
                order2[i2] = i2;
                if (lbl_11_bss_C8 > 0)
                    score2[i2] = lbl_11_data_29C[i2][lbl_11_bss_C8 - 1];
                else
                    score2[i2] = 0;
            }
            for (i2 = 0; i2 < modeCtrl.playerCount; i2++)
            {
                for (j2 = i2; j2 < modeCtrl.playerCount; j2++)
                {
                    if (score2[i2] > score2[j2])
                    {
                        tmp16 = score2[i2];
                        score2[i2] = score2[j2];
                        score2[j2] = tmp16;
                        tmp16 = order2[i2];
                        order2[i2] = order2[j2];
                        order2[j2] = tmp16;
                    }
                }
            }
            r2 = 0;
            cnt2 = 1;
            for (i2 = 0; i2 < modeCtrl.playerCount; i2++)
            {
                rank2[order2[i2]] = r2;
                if (score2[i2] != score2[i2 + 1])
                {
                    r2 += cnt2;
                    cnt2 = 1;
                }
                else
                    cnt2++;
            }
            bestRank = 9;
            for (i2 = 0; i2 < modeCtrl.playerCount; i2++)
            {
                if (rank2[i2] == 0 && bestRank > i2)
                {
                    GOLF_SET_PLAYER(i2);
                    bestRank = i2;
                }
            }
        }
        lbl_11_bss_C8--;
        fn_11_E164();
        for (k = 0; k < modeCtrl.playerCount; k++)
        {
            balls[k].pos = stagedef->startPos->pos;
            balls[k].prevPos = stagedef->startPos->pos;
        }
    }

    for (k = 0; k < modeCtrl.playerCount; k++)
    {
        if (balls[k].status != 0)
        {
            g_poolInfo.playerPool.statusList[k] = 4;
            balls[k].status = 4;
            if (reloadApe)
            {
                g_something_with_freeing_chara_heap_ape(balls[k].ape);
                balls[k].ape = NULL;
            }
        }
    }
    if (reloadApe)
    {
        balls[modeCtrl.currPlayer].ape = g_load_ape_gameplay_variant0(active_monkey_id[modeCtrl.currPlayer], 0);
        balls[modeCtrl.currPlayer].ape->unk29C = 0;
        balls[modeCtrl.currPlayer].ape->ballIdx = modeCtrl.currPlayer;
        balls[modeCtrl.currPlayer].ape->colorIndex = modeCtrl.currPlayer;
        balls[modeCtrl.currPlayer].ape->pos = balls[modeCtrl.currPlayer].pos;
        balls[modeCtrl.currPlayer].ape->drawFunc = fn_11_1A610;
        mtxa_from_identity();
        mtxa_rotate_y(balls[modeCtrl.currPlayer].unk92);
        mtxa_to_quat(&balls[modeCtrl.currPlayer].ape->rot);
    }
    g_poolInfo.playerPool.statusList[modeCtrl.currPlayer] = 2;
    lbl_11_bss_20->status = 2;
    lbl_11_bss_20->vel = (Vec){0.0f, 0.0f, 0.0f};
    lbl_11_bss_20->unkC0 = (Vec){0.0f, 0.0f, 0.0f};
    lbl_11_bss_20->unkA0 = (Quat){0.0f, 0.0f, 0.0f, 0.0f};
    lbl_11_bss_20->unkB0 = (Quat){0.0f, 0.0f, 0.0f, 0.0f};
    lbl_11_bss_20->unkCC = 0.0f;
    lbl_11_bss_88 = 0;
    mtx_copy(lbl_11_bss_20->unk30, lbl_11_bss_20->unkD0);
}
