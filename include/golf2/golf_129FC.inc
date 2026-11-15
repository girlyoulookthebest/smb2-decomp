/* golf.c functions 0x129FC-0x171E8: shot arrow and wind gauge drawing,
 * screen-space helpers, the screen copy texture and its wipe effects,
 * the rings over the cup, and score helpers. */

/* from other golf.c fragments */
void g_smth_with_golf_wind_velocity(float velocity);

/* GX and libc prototypes not in the shared headers */
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetCullMode_cached(int mode);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void g_something_with_GX_vtx_desc(u32 attrs);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void set_ui_widescreen_scale_mtx(int width);
void reset_ui_widescreen_scale_mtx(void);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int attr, int type);
void GXSetVtxAttrFmt(int vtxfmt, int attr, int cnt, int type, u8 frac);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTevColor(int id, GXColor color);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObjLOD(GXTexObj *obj, int minFilter, int magFilter, float minLod, float maxLod, float lodBias, u8 biasClamp, u8 doEdgeLod, int maxAniso);
void exit(int status);
extern struct GMA *minigameGma;


/* Draws the aiming arrow: a rainbow ribbon from the ball along the shot direction. */
void fn_11_129FC(void)
{
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    Vec toEye;
    Vec dir;
    Vec eye;
    float power;
    float alphaScale0;
    float alphaScale1;
    int i;
    int t;
    int alpha0;
    int alpha1;
    s8 r, g, b;

    mtxa_from_mtx(lbl_11_bss_24->unk14C);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    GXSetVtxAttrFmt(2, 11, 1, 5, 0);
    GXSetVtxAttrFmt(2, 9, 1, 4, 0);
    GXSetNumChans_cached(1);
    GXSetNumTexGens_cached(0);
    GXSetNumTevStages_cached(1);
    g_GXSetTevIndirect_zero_if_different(0);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 2);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTevColorIn_cached(0, 15, 10, 2, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 5, 1, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetBlendMode_cached(1, 4, 1, 0);
    GXSetCullMode_cached(0);
    GXSetTevColor(1, white);

    power = lbl_11_data_2EC[lbl_11_bss_CA] * (lbl_11_bss_94 * (lbl_11_data_1A0[lbl_11_bss_4C] * fabs(lbl_11_bss_D4)));
    lbl_11_bss_198[0].x = power * mathutil_cos(lbl_11_data_180[lbl_11_bss_4C]) * -math_sin(lbl_11_bss_20->unk92);
    lbl_11_bss_198[0].y = power * math_sin(lbl_11_data_180[lbl_11_bss_4C]);
    lbl_11_bss_198[0].z = power * mathutil_cos(lbl_11_data_180[lbl_11_bss_4C]) * -mathutil_cos(lbl_11_bss_20->unk92);
    lbl_11_bss_198[0].x += lbl_11_bss_20->pos.x;
    lbl_11_bss_198[0].y += lbl_11_bss_20->pos.y;
    lbl_11_bss_198[0].z += lbl_11_bss_20->pos.z;
    lbl_11_bss_198[1] = lbl_11_bss_20->pos;

    eye = lbl_11_bss_24->eye;
    toEye.x = eye.x - lbl_11_bss_198[1].x;
    toEye.y = eye.y - lbl_11_bss_198[1].y;
    toEye.z = eye.z - lbl_11_bss_198[1].z;
    dir.x = lbl_11_bss_198[0].x - lbl_11_bss_198[1].x;
    dir.y = lbl_11_bss_198[0].y - lbl_11_bss_198[1].y;
    dir.z = lbl_11_bss_198[0].z - lbl_11_bss_198[1].z;
    vec_normalize_len(&toEye);
    vec_normalize_len(&dir);
    mathutil_vec_cross_prod(&toEye, &dir, &lbl_11_bss_CBC[1]);
    vec_normalize_len(&lbl_11_bss_CBC[1]);

    for (i = 0; i < 9; i++)
    {
        if (i <= 4)
        {
            alphaScale0 = (float)i / 5.0f;
            alphaScale0 *= alphaScale0;
        }
        else
        {
            alphaScale0 = (float)(9 - i) / 5.0f;
            alphaScale0 *= alphaScale0;
        }
        if (i + 1 <= 4)
        {
            alphaScale1 = (float)(i + 1) / 5.0f;
            alphaScale1 *= alphaScale1;
        }
        else
        {
            alphaScale1 = (float)(9 - (i + 1)) / 5.0f;
            alphaScale1 *= alphaScale1;
        }
        t = g_font_char_var_3 - lbl_11_bss_F14;
        GXBegin(0x98, 2, 4);
        {
            float widths[10] = {-0.612f, -0.54f, -0.456f, -0.33f, -0.18f, 0.18f, 0.33f, 0.456f, 0.54f, 0.612f};

            alpha0 = alphaScale0 * (208.0f * (50 - t) * (50 - t) / 2500.0f);
            alpha1 = alphaScale1 * (208.0f * (50 - t) * (50 - t) / 2500.0f);
            fn_11_1338C((u8 *)&r, (u8 *)&g, (u8 *)&b, t);
            GXPosition3f32(lbl_11_bss_198[1].x + lbl_11_bss_CBC[1].x * widths[i],
                           lbl_11_bss_198[1].y + lbl_11_bss_CBC[1].y * widths[i],
                           lbl_11_bss_198[1].z + lbl_11_bss_CBC[1].z * widths[i]);
            GXColor4u8(r, g, b, alpha0);
            GXPosition3f32(lbl_11_bss_198[1].x + lbl_11_bss_CBC[1].x * widths[i + 1],
                           lbl_11_bss_198[1].y + lbl_11_bss_CBC[1].y * widths[i + 1],
                           lbl_11_bss_198[1].z + lbl_11_bss_CBC[1].z * widths[i + 1]);
            GXColor4u8(r, g, b, alpha1);
            fn_11_1338C((u8 *)&r, (u8 *)&g, (u8 *)&b, t - 1);
            GXPosition3f32(lbl_11_bss_198[0].x + lbl_11_bss_CBC[1].x * widths[i],
                           lbl_11_bss_198[0].y + lbl_11_bss_CBC[1].y * widths[i],
                           lbl_11_bss_198[0].z + lbl_11_bss_CBC[1].z * widths[i]);
            GXColor4u8(r, g, b, alpha0);
            GXPosition3f32(lbl_11_bss_198[0].x + lbl_11_bss_CBC[1].x * widths[i + 1],
                           lbl_11_bss_198[0].y + lbl_11_bss_CBC[1].y * widths[i + 1],
                           lbl_11_bss_198[0].z + lbl_11_bss_CBC[1].z * widths[i + 1]);
            GXColor4u8(r, g, b, alpha1);
        }
    }
}

/* Rainbow color for the arrow: t cycles through 30 steps. */
void fn_11_1338C(u8 *r, u8 *g, u8 *b, int t)
{
    t %= 30;
    if (t >= 0 && t < 5)
    {
        *r = 0xFF;
        *g = 255.0f * ((5.0f - t) / 5.0f);
        *b = 0;
        return;
    }
    if (t >= 5 && t < 10)
    {
        *r = 0xFF;
        *g = 0;
        *b = 255.0 * ((t - 5.0) / 5.0);
        return;
    }
    if (t >= 10 && t < 15)
    {
        *r = 255.0f * ((15.0f - t) / 5.0f);
        *g = 0;
        *b = 0xFF;
        return;
    }
    if (t >= 15 && t < 20)
    {
        *r = 0;
        *g = 255.0 * ((t - 15.0) / 5.0);
        *b = 0xFF;
        return;
    }
    if (t >= 20 && t < 25)
    {
        *r = 0;
        *g = 0xFF;
        *b = 255.0f * ((25.0f - t) / 5.0f);
        return;
    }
    if (t >= 25 && t < 30)
    {
        *r = 255.0 * ((t - 25.0) / 5.0);
        *g = 0xFF;
        *b = 0;
    }
}

/* Draws the wind gauge arrow next to the HUD wind display. */
void fn_11_13630(void)
{
    float sx;
    float sy;
    float scale;

    set_ui_widescreen_scale_mtx(0x41);
    g_smth_with_golf_wind_velocity(golf_wind_velocity);
    reset_ui_widescreen_scale_mtx();
    if (golf_wind_velocity < 0.001)
        return;

    fn_11_138C8(&sx, &sy, 0.1 + lbl_11_bss_24->sub28.view.nearZ,
                0.1015625 * lbl_1_bss_2434,
                0.5208333 * (lbl_1_bss_2434 / g_camera_aspect_ratio),
                lbl_11_bss_24->sub28.view.tanHalfFov);
    scale = 0.008f;
    u_reset_light_group_stack(modeCtrl.currPlayer);
    mtxa_from_mtxb();
    mathutil_mtxA_set_translate_xyz(sx, sy, -(0.1 + lbl_11_bss_24->sub28.view.nearZ));
    mtxa_rotate_y(lbl_11_bss_24->rotY + 0x2000);
    mtxa_rotate_x(0x800);
    mtxa_rotate_y(-(lbl_11_bss_24->rotY + 0x2000));
    mtxa_rotate_y(golf_wind_angle);
    if (lbl_11_bss_24->sub28.view.nearZ > 0.9)
        mtxa_scale_xyz(0.165f, 0.825f, 0.165f);
    else
        mtxa_scale_xyz(0.03f, 0.15f, 0.03f);
    mtxa_scale_s((float)lbl_11_bss_24->sub28.view.fov / 10922.0f);
    mtxa_translate_xyz(0.0f, 0.0125f, 1.187f);
    if (golf_wind_velocity > 6.666666f)
        set_post_mult_color(1.0f, 0.5f, 0.5f, 1.0f);
    else if (golf_wind_velocity > 3.333333f)
        set_post_mult_color(1.0f, 1.0f, 0.5f, 1.0f);
    else
        set_post_mult_color(0.5f, 1.0f, 1.0f, 1.0f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_set_scale_factor(scale);
    avdisp_draw_model_unculled_sort_never(minigameGma->modelEntries[23].model);
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
}

/* Screen position (x, y) at view depth z to view-space x/y. */
void fn_11_138C8(float *outX, float *outY, float z, float x, float y, float unused)
{
    float h = lbl_11_bss_24->sub28.viewportH * (0.5f * (lbl_1_bss_2434 / g_camera_aspect_ratio));
    float cx = 0.5f * lbl_1_bss_2434 * lbl_11_bss_24->sub28.viewportW;
    float s = (z * lbl_11_bss_24->sub28.view.tanHalfFov) / h;

    *outX = s * (x - cx);
    *outY = s * -(y - h);
    *outX *= lbl_1_bss_2440 / lbl_11_bss_24->sub28.view.tanHalfFov;
    *outY *= lbl_1_bss_2440 / lbl_11_bss_24->sub28.view.tanHalfFov;
}

/* Inverse of fn_11_138C8: view-space x/y at depth z to a screen position. */
void fn_11_139C8(float x, float y, float z, float *outX, float *outY, float unused)
{
    float s;
    float cx;
    float h;

    h = lbl_11_bss_24->sub28.viewportH * (0.5f * (lbl_1_bss_2434 / g_camera_aspect_ratio));
    cx = 0.5f * lbl_1_bss_2434 * lbl_11_bss_24->sub28.viewportW;
    s = (1.0f / h) * (z * lbl_11_bss_24->sub28.view.tanHalfFov);

    *outX = cx + x / (lbl_1_bss_2440 / lbl_11_bss_24->sub28.view.tanHalfFov) / s;
    *outY = h - y / (lbl_1_bss_2440 / lbl_11_bss_24->sub28.view.tanHalfFov) / s;
}

/* Allocates the screen copy texture (module init). */
void fn_11_13AC4(void)
{
    u32 size = GXGetTexBufferSize(640, current_render_mode->efbHeight, 6, 0, 0);

    if (lbl_11_bss_2818 == NULL)
        lbl_11_bss_2818 = alloc_from_heap_or_panic(chara_heap, size, "golf2_mode.c", 7754);
    if (lbl_11_bss_2818 == NULL)
    {
        printf("error: over_heap_tex_buff\n");
        exit(1);
    }
    GXInitTexObj(&lbl_11_bss_281C, lbl_11_bss_2818, 640, current_render_mode->efbHeight, 6, 0, 0, 0);
}

/* Frees the screen copy texture. */
void fn_11_13BAC(void)
{
    if (lbl_11_bss_2818 != NULL)
    {
        OSFreeToHeap(chara_heap, lbl_11_bss_2818);
        lbl_11_bss_2818 = NULL;
    }
}

/* Copies the EFB into the screen copy texture. */
void fn_11_13C08(void)
{
    GXSetTexCopySrc_cached(0, 0, 640, current_render_mode->efbHeight);
    GXSetTexCopyDst_cached(640, current_render_mode->efbHeight, 6, 0);
    GXCopyTex(lbl_11_bss_2818, 0);
    GXInitTexObjLOD(&lbl_11_bss_281C, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
}

/* Sets up GX to draw the screen copy texture over the screen. */
void fn_11_13CBC(void)
{
    mtxa_push();
    mtxa_from_translate_xyz(0.5f, 0.5f, 0.0f);
    mtxa_scale_xyz(1.0f / g_camera_aspect_ratio, -1.0f, 0.0f);
    mtxa_rotate_x(0x4000);
    GXLoadTexMtxImm(mtxa->mtxA, 30, 1);
    mtxa_pop();
    GXSetTexCoordGen2_cached(0, 1, 0, 30, 0, 125);
    GXLoadTexObj_cached(&lbl_11_bss_281C, 0);
    g_something_with_GX_vtx_desc(0x2A00);
    GXSetVtxAttrFmt(2, 13, 1, 4, 0);
    GXSetVtxAttrFmt(2, 11, 1, 5, 0);
    GXSetVtxAttrFmt(2, 9, 1, 4, 0);
    GXSetNumChans_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    g_GXSetTevIndirect_zero_if_different(0);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevColorIn_cached(0, 15, 15, 15, 8);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 1);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetBlendMode_cached(1, 4, 5, 0);
}

/* Distance of grid point (i, j) from the grid center, for the ripple wipe. */
static inline float ripple_dist(int i, int j)
{
    return math_sqrt(mathutil_sum_of_sq_3(fabs((float)(20 - i)), 0.0f, fabs((float)(20 - j))));
}
#define RIPPLE_DIST(i, j) ripple_dist(i, j)
#define RIPPLE_OUTER (4.0 * (2.5 * (float)(lbl_11_bss_6C + 60)) / 60.0)
#define RIPPLE_INNER (4.0 * (2.0 * (float)(lbl_11_bss_6C + 60)) / 60.0)

/* Ripple wipe: the screen copy on a 40x40 grid with a growing ring wave. */
void fn_11_13E6C(void)
{
    GXColor color = {0xFF, 0xFF, 0xFF, 0x7F};
    float left;
    float top;
    int i;
    int j;

    GXSetCullMode_cached(0);
    if (120.0f >= 4.0f * lbl_11_bss_6C)
        color.a = 255.0f * (120.0f - 4.0f * lbl_11_bss_6C) / 120.0f;
    else
        color.a = 0;
    GXSetTevColor(1, color);
    left = -0.5f * g_camera_aspect_ratio;
    top = -0.5f;
    for (i = 0; i < 39; i++)
    {
        mtxa_from_identity();
        mtxa_translate_xyz(0.0f, 0.0f, -0.5 / lbl_1_bss_2440);
        mtxa_rotate_x(0x4000);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        GXBegin(0x98, 2, 80);
        for (j = 0; j < 40; j++)
        {
            if (RIPPLE_DIST(i, j) > RIPPLE_OUTER)
            {
                GXPosition3f32(left + j / 40.0f * g_camera_aspect_ratio, 0.0f, top + i / 40.0f);
                GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
                GXTexCoord2f32(j / 40.0f, i / 40.0f);
            }
            else if (RIPPLE_DIST(i, j) > RIPPLE_INNER)
            {
                GXPosition3f32(left + j / 40.0f * g_camera_aspect_ratio,
                               0.5f * math_sqrt(RIPPLE_OUTER * RIPPLE_OUTER - RIPPLE_DIST(i, j) * RIPPLE_DIST(i, j)) / 40.0f,
                               top + i / 40.0f);
                GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
                GXTexCoord2f32(j / 40.0f, i / 40.0f);
            }
            else
            {
                GXPosition3f32(left + j / 40.0f * g_camera_aspect_ratio,
                               0.5f * math_sqrt(RIPPLE_OUTER * RIPPLE_OUTER - RIPPLE_DIST(i, j) * RIPPLE_DIST(i, j)) / 40.0f
                               - math_sqrt(RIPPLE_INNER * RIPPLE_INNER - RIPPLE_DIST(i, j) * RIPPLE_DIST(i, j)) / 40.0f,
                               top + i / 40.0f);
                GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
                GXTexCoord2f32(j / 40.0f, i / 40.0f);
            }

            if (RIPPLE_DIST(i + 1, j) > RIPPLE_OUTER)
            {
                GXPosition3f32(left + j / 40.0f * g_camera_aspect_ratio, 0.0f, top + (i + 1) / 40.0f);
                GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
                GXTexCoord2f32(j / 40.0f, (i + 1) / 40.0f);
            }
            else if (RIPPLE_DIST(i + 1, j) > RIPPLE_INNER)
            {
                GXPosition3f32(left + j / 40.0f * g_camera_aspect_ratio,
                               0.5f * math_sqrt(RIPPLE_OUTER * RIPPLE_OUTER - RIPPLE_DIST(i + 1, j) * RIPPLE_DIST(i + 1, j)) / 40.0f,
                               top + (i + 1) / 40.0f);
                GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
                GXTexCoord2f32(j / 40.0f, (i + 1) / 40.0f);
            }
            else
            {
                GXPosition3f32(left + j / 40.0f * g_camera_aspect_ratio,
                               0.5f * math_sqrt(RIPPLE_OUTER * RIPPLE_OUTER - RIPPLE_DIST(i + 1, j) * RIPPLE_DIST(i + 1, j)) / 40.0f
                               - math_sqrt(RIPPLE_INNER * RIPPLE_INNER - RIPPLE_DIST(i + 1, j) * RIPPLE_DIST(i + 1, j)) / 40.0f,
                               top + (i + 1) / 40.0f);
                GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
                GXTexCoord2f32(j / 40.0f, (i + 1) / 40.0f);
            }
        }
    }
}

/* Tile wipe: the screen copy split into 8x8 tiles that flip over. */
void fn_11_15248(void)
{
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
    Vec center;
    float x2, y2, z2;
    float x3, y3, z3;
    float left;
    float top;
    float step;
    float x0, y0, z0;
    float x1, y1, z1;
    int i;
    int j;

    GXSetCullMode_cached(2);
    GXSetTevColor(1, color);
    left = -0.5f * g_camera_aspect_ratio;
    top = -0.5f;
    step = 0.125f;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            x0 = left + g_camera_aspect_ratio * (j * step);
            y0 = 0.0f;
            z0 = top + i * step;
            x1 = left + g_camera_aspect_ratio * ((j + 1) * step);
            y1 = 0.0f;
            z1 = top + i * step;
            x2 = left + g_camera_aspect_ratio * ((j + 1) * step);
            y2 = 0.0f;
            z2 = top + (i + 1) * step;
            x3 = left + g_camera_aspect_ratio * (j * step);
            y3 = 0.0f;
            z3 = top + (i + 1) * step;
            center.x = x0 + step / 2.0f;
            center.y = 0.0f;
            center.z = z0 + step / 2.0f;
            mtxa_from_identity();
            mtxa_translate_xyz(0.0f, 0.0f, -0.5 / lbl_1_bss_2440);
            mtxa_rotate_x(0x4000);
            mtxa_translate(&center);
            mtxa_rotate_y(0x2000);
            mtxa_rotate_z((lbl_11_bss_6C / lbl_11_bss_78) << 15);
            mtxa_rotate_y(-0x2000);
            mtxa_translate_xyz(-center.x, -center.y, -center.z);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            GXBegin(0x80, 2, 4);
            GXPosition3f32(x0, y0, z0);
            GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
            GXTexCoord2f32(j * step, i * step);
            GXPosition3f32(x1, y1, z1);
            GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
            GXTexCoord2f32((j + 1) * step, i * step);
            GXPosition3f32(x2, y2, z2);
            GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
            GXTexCoord2f32((j + 1) * step, (i + 1) * step);
            GXPosition3f32(x3, y3, z3);
            GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
            GXTexCoord2f32(j * step, (i + 1) * step);
        }
    }
}

/* Fade wipe: the screen copy as one quad that fades out. */
void fn_11_158B8(void)
{
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
    float x2, y2, z2;
    float x3, y3, z3;
    float left;
    float top;
    float step;
    float x0, y0, z0;
    float x1, y1, z1;
    int i;
    int j;

    GXSetCullMode_cached(2);
    if (lbl_11_bss_78 >= 4.0 * lbl_11_bss_6C)
        color.a = 255.0 * (lbl_11_bss_78 - 4.0 * lbl_11_bss_6C) / lbl_11_bss_78;
    else
        color.a = 0;
    GXSetTevColor(1, color);
    left = -0.5f * g_camera_aspect_ratio;
    top = -0.5f;
    step = 1.0f;
    for (i = 0; i < 1; i++)
    {
        for (j = 0; j < 1; j++)
        {
            x0 = left + g_camera_aspect_ratio * (j * step);
            y0 = 0.0f;
            z0 = top + i * step;
            x1 = left + g_camera_aspect_ratio * ((j + 1) * step);
            y1 = 0.0f;
            z1 = top + i * step;
            x2 = left + g_camera_aspect_ratio * ((j + 1) * step);
            y2 = 0.0f;
            z2 = top + (i + 1) * step;
            x3 = left + g_camera_aspect_ratio * (j * step);
            y3 = 0.0f;
            z3 = top + (i + 1) * step;
            mtxa_from_identity();
            mtxa_translate_xyz(0.0f, 0.0f, -0.5 / lbl_1_bss_2440);
            mtxa_rotate_x(0x4000);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            GXBegin(0x80, 2, 4);
            GXPosition3f32(x0, y0, z0);
            GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
            GXTexCoord2f32(j * step, i * step);
            GXPosition3f32(x1, y1, z1);
            GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
            GXTexCoord2f32((j + 1) * step, i * step);
            GXPosition3f32(x2, y2, z2);
            GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
            GXTexCoord2f32((j + 1) * step, (i + 1) * step);
            GXPosition3f32(x3, y3, z3);
            GXColor4u8(0xFF, 0xFF, 0xFF, 0x80);
            GXTexCoord2f32(j * step, (i + 1) * step);
        }
    }
}

/* (points[i].c - points[i + 1].c)^2; the length below squares these again (as in the original). */
#define SEG_SQ(p, i, c) (((p)[i].c - (p)[(i) + 1].c) * ((p)[i].c - (p)[(i) + 1].c))

/* Point at fraction num/den along the polyline points[0..count-1]. */
void fn_11_15FB4(Vec *out, int count, Vec *points, int den, int num)
{
    Vec pos;
    float total = 0.0f;
    float sum = 0.0f;
    float segLen = 0.0f;
    float target;
    int i;

    for (i = 0; i < count - 1; i++)
        total += math_sqrt(mathutil_sum_of_sq_3(SEG_SQ(points, i, x), SEG_SQ(points, i, y), SEG_SQ(points, i, z)));
    target = total * num / den;
    for (i = 0; i < count - 1; i++)
    {
        segLen = math_sqrt(mathutil_sum_of_sq_3(SEG_SQ(points, i, x), SEG_SQ(points, i, y), SEG_SQ(points, i, z)));
        if (sum + segLen > target)
            break;
        sum += segLen;
    }
    target -= sum;
    pos = points[i];
    target /= segLen;
    pos.x += target * (points[i + 1].x - points[i].x);
    pos.y += target * (points[i + 1].y - points[i].y);
    pos.z += target * (points[i + 1].z - points[i].z);
    *out = pos;
}

/* Stand-in for a function the linker stripped (it sat between fn_11_15FB4 and fn_11_16318);
 * only its constants remain in the .rodata pool (0x4E0-0x4FB). */
void golf_stripped_16318_literals(GXColor *color, float *f)
{
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};

    *color = white;
    f[0] = 29.0f;
    f[1] = 160.0f;
    f[2] = 27.0f;
    f[3] = 128.0f;
    f[4] = 23.0f;
    f[5] = 112.0f;
}

/* Draws the rising rings over the cup after the ball is holed. */
void fn_11_16318(void)
{
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    Vec pos0;
    Vec pos1;
    int j;
    int i;
    int a;
    int r, g, b;
    float dist;

    if (lbl_11_bss_C < 0)
        return;

    mtxa_from_mtx(lbl_11_bss_24->unk14C);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    g_something_with_GX_vtx_desc(0xA00);
    GXSetVtxAttrFmt(2, 11, 1, 5, 0);
    GXSetVtxAttrFmt(2, 9, 1, 4, 0);
    GXSetNumChans_cached(1);
    GXSetNumTexGens_cached(0);
    GXSetNumTevStages_cached(1);
    g_GXSetTevIndirect_zero_if_different(0);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 2);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTevColorIn_cached(0, 15, 10, 2, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 5, 1, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetCullMode_cached(0);
    GXSetTevColor(1, white);

    for (i = 0; i < 15; i++)
    {
        GXBegin(0x98, 2, 66);
        for (j = 0; j <= 32; j++)
        {
            mtxa_from_identity();
            if (lbl_11_bss_24->unk1F == 21)
            {
                if (lbl_11_bss_4C == 12 || lbl_11_bss_4C == 13 || lbl_11_bss_4C == 14)
                    mtxa_translate_xyz(0.0f, 1.0f, 0.0f);
                else
                    mtxa_translate_xyz(0.0f, 20.0f, 0.0f);
            }
            mtxa_translate(&lbl_11_bss_198[lbl_11_bss_50 - 1]);
            mtxa_rotate_y((int)((float)(j << 16) / 32.0f));
            dist = math_sqrt(mathutil_sum_of_sq_3(
                cameras[modeCtrl.currPlayer].eye.x - cameras[modeCtrl.currPlayer].lookAt.x,
                cameras[modeCtrl.currPlayer].eye.y - cameras[modeCtrl.currPlayer].lookAt.y,
                cameras[modeCtrl.currPlayer].eye.z - cameras[modeCtrl.currPlayer].lookAt.z));
            mtxa_scale_s(dist / 15.0);

            if (lbl_11_bss_4C >= 12)
            {
                r = 255.0f * math_sqrt(i / 15.0f);
                g = 0xFF;
                b = 0xFF;
            }
            else
            {
                r = 0xFF;
                g = 255.0f * math_sqrt(i / 15.0f);
                b = 0xFF;
            }
            a = (u8)(208.0f * (1.0f - i / 15.0f)) * (1.0f - lbl_11_bss_C / 120.0f);
            pos0.x = 0.0f;
            pos0.y = 0.0f;
            pos0.z = 2.0f * lbl_11_bss_C / 120.0f - 0.1f * i;
            if (pos0.z <= 0.0f)
                a = 0;
            mtxa_tf_point(&pos0, &pos0);
            GXPosition3f32(pos0.x, pos0.y, pos0.z);
            GXColor4u8(r, g, b, a);

            if (lbl_11_bss_4C >= 12)
            {
                r = 255.0f * math_sqrt((i + 1) / 15.0f);
                g = 0xFF;
                b = 0xFF;
            }
            else
            {
                r = 0xFF;
                g = 255.0f * math_sqrt((i + 1) / 15.0f);
                b = 0xFF;
            }
            a = (u8)(208.0f * (1.0f - (i + 1) / 15.0f)) * (1.0f - lbl_11_bss_C / 120.0f);
            pos1.x = 0.0f;
            pos1.y = 0.0f;
            pos1.z = 2.0f * lbl_11_bss_C / 120.0f - 0.1f * (i + 1);
            if (pos1.z <= 0.0)
                a = 0;
            mtxa_tf_point(&pos1, &pos1);
            GXPosition3f32(pos1.x, pos1.y, pos1.z);
            GXColor4u8(r, g, b, a);
        }
    }
}

/* x raised to the integer power n. */
float fn_11_16ACC(float x, int n)
{
    int k = abs(n);
    float result = 1.0f;

    while (k != 0)
    {
        if (k & 1)
            result *= x;
        x *= x;
        k >>= 1;
    }
    if (n >= 0)
        return result;
    return 1.0f / result;
}

/* Square root applied n times. */
float fn_11_16B2C(int n, float x)
{
    while (n > 0)
    {
        x = math_sqrt(x);
        n--;
    }
    return x;
}

/* Distance unit for the cup distance: yards, or meters for clubs 12+ (putting). */
#define DIST_UNIT(var) \
    var = 0.914f; \
    if (lbl_11_bss_4C >= 12) \
        var = 1.0f;

/* Finds the widest spread between the cup and the players' balls, for the camera. */
void fn_11_16B68(s8 *flag, float *maxDist, Vec *center)
{
    float unit;
    float d;
    int i;
    int j;

    *flag = 1;
    *maxDist = 0.0f;
    center->x = 0.0f;
    center->y = 0.0f;
    center->z = 0.0f;
    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != 0)
        {
            DIST_UNIT(unit)
            if (math_sqrt(mathutil_sum_of_sq_3(lbl_11_bss_38.x - balls[i].pos.x,
                                               lbl_11_bss_38.y - balls[i].pos.y,
                                               lbl_11_bss_38.z - balls[i].pos.z)) / unit > *maxDist)
            {
                DIST_UNIT(unit)
                *maxDist = math_sqrt(mathutil_sum_of_sq_3(lbl_11_bss_38.x - balls[i].pos.x,
                                                          lbl_11_bss_38.y - balls[i].pos.y,
                                                          lbl_11_bss_38.z - balls[i].pos.z)) / unit;
                center->x = (lbl_11_bss_38.x + balls[i].pos.x) / 2.0f;
                center->y = (lbl_11_bss_38.y + balls[i].pos.y) / 2.0f;
                center->z = (lbl_11_bss_38.z + balls[i].pos.z) / 2.0f;
            }
        }
    }
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < i; j++)
        {
            if (g_poolInfo.playerPool.statusList[i] != 0 && g_poolInfo.playerPool.statusList[j] != 0)
            {
                d = math_sqrt(mathutil_sum_of_sq_3(balls[i].pos.x - balls[j].pos.x,
                                                   balls[i].pos.y - balls[j].pos.y,
                                                   balls[i].pos.z - balls[j].pos.z));
                if (d > *maxDist)
                {
                    *maxDist = d;
                    center->x = (balls[j].pos.x + balls[i].pos.x) / 2.0f;
                    center->y = (balls[j].pos.y + balls[i].pos.y) / 2.0f;
                    center->z = (balls[j].pos.z + balls[i].pos.z) / 2.0f;
                }
            }
        }
    }
    *maxDist = 1.2 * *maxDist;
}

/* Par of the current hole. */
s8 fn_11_16FA4(void)
{
    return lbl_11_data_228[lbl_11_bss_C8];
}

/* Par of the given hole. */
s8 fn_11_16FC4(int hole)
{
    return lbl_11_data_228[hole];
}

/* Current player's score on the current hole relative to par
 * (-10 hole in one, 99 not played, 50 stroke limit reached). */
int fn_11_16FD8(void)
{
    if (lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8] == 1)
        return -10;
    if (lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8] == 99)
        return 99;
    if ((s8)lbl_11_data_23C[lbl_11_bss_C8] == lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8])
        return 50;
    return lbl_11_data_29C[modeCtrl.currPlayer][lbl_11_bss_C8] - (s8)lbl_11_data_228[lbl_11_bss_C8];
}

/* Same as fn_11_16FD8 for the given player. */
int fn_11_170E8(int player)
{
    if (lbl_11_data_29C[player][lbl_11_bss_C8] == 1)
        return -10;
    if (lbl_11_data_29C[player][lbl_11_bss_C8] == 99)
        return 99;
    if ((s8)lbl_11_data_23C[lbl_11_bss_C8] == lbl_11_data_29C[player][lbl_11_bss_C8])
        return 50;
    return lbl_11_data_29C[player][lbl_11_bss_C8] - (s8)lbl_11_data_228[lbl_11_bss_C8];
}

BOOL fn_11_171C8(void)
{
    return lbl_11_bss_30 == 0x40000;
}
