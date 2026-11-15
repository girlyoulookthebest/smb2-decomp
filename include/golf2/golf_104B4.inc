/* golf.c fn_11_104B4: shot ring at the end of the trail (w1) */

void ray_scale(Vec *a, Vec *b, Vec *out, float t);
void g_something_with_GX_vtx_desc(u32 attrs);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int attr, int type);
void GXSetVtxAttrFmt(int vtxfmt, int attr, int cnt, int type, u8 frac);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void g_GXSetTevIndirect_zero_if_different(int stage);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetCullMode_cached(int mode);
void GXSetTevColor(int id, GXColor color);

/* Draws a fading ring (5 bands of 33 points) around the trail end, or around the ball
 * while aiming (lbl_11_bss_14 != 0). */
void fn_11_104B4(void)
{
    Vec ring[33];
    Vec ring2[33];
    Vec dir;
    Vec side;
    Vec dir2;
    Vec side2;
    Vec toEye;
    Vec fwd;
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    s8 r, g, b;
    s8 r2, g2, b2;
    int i;
    int k;
    int alpha0;
    int alpha1;
    u8 rainbow;
    int alpha2;

    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    g_something_with_GX_vtx_desc(0xA00);
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
    if (lbl_11_bss_14 == 0)
    {
        float widths[10] = {0.612f, 0.54f, 0.456f, 0.33f, 0.18f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

        if (0.0 == lbl_11_bss_5C && 1.0 == lbl_11_bss_D4)
            rainbow = TRUE;
        else
            rainbow = FALSE;
        dir.x = lbl_11_bss_198[lbl_11_bss_60 - 1].x - lbl_11_bss_198[lbl_11_bss_60 - 2].x;
        dir.y = lbl_11_bss_198[lbl_11_bss_60 - 1].y - lbl_11_bss_198[lbl_11_bss_60 - 2].y;
        dir.z = lbl_11_bss_198[lbl_11_bss_60 - 1].z - lbl_11_bss_198[lbl_11_bss_60 - 2].z;
        side = lbl_11_bss_CBC[lbl_11_bss_60 - 1];
        vec_normalize_len(&dir);
        vec_normalize_len(&side);
        for (i = 0; i < 16; i++)
        {
            ray_scale(&side, &dir, &ring[i], i / 16.0f);
            vec_normalize_len(&ring[i]);
        }
        ring[16] = dir;
        side.x *= -1.0f;
        side.y *= -1.0f;
        side.z *= -1.0f;
        for (i = 17; i <= 32; i++)
        {
            ray_scale(&dir, &side, &ring[i], (i - 16) / 16.0f);
            vec_normalize_len(&ring[i]);
        }
        for (k = 0; k < 5; k++)
        {
            float a0;
            float a1;

            if (k <= 4)
            {
                a0 = (float)k / 5.0f;
                a0 *= a0;
            }
            else
            {
                a0 = (float)(9 - k) / 5.0f;
                a0 *= a0;
            }
            if (k + 1 <= 4)
            {
                a1 = (float)(k + 1) / 5.0f;
                a1 *= a1;
            }
            else
            {
                a1 = (float)(9 - (k + 1)) / 5.0f;
                a1 *= a1;
            }
            /* stored as bytes; read back as s8 (matches the original's stores and loads) */
            *(u8 *)&r = 0xD0;
            *(u8 *)&g = 0xD0;
            *(u8 *)&b = 0xFF;
            alpha0 = a0 * (208.0f * (50 - (lbl_11_bss_60 - 1)) * (50 - (lbl_11_bss_60 - 1)) / 2500.0f);
            alpha1 = a1 * (208.0f * (50 - (lbl_11_bss_60 - 1)) * (50 - (lbl_11_bss_60 - 1)) / 2500.0f);
            if (rainbow)
                fn_11_1338C((u8 *)&r, (u8 *)&g, (u8 *)&b, lbl_11_bss_60 - 1);
            GXBegin(0x98, 2, 66);
            for (i = 0; i <= 32; i++)
            {
                GXPosition3f32(lbl_11_bss_198[lbl_11_bss_60 - 1].x + ring[i].x * widths[k],
                               lbl_11_bss_198[lbl_11_bss_60 - 1].y + ring[i].y * widths[k],
                               lbl_11_bss_198[lbl_11_bss_60 - 1].z + ring[i].z * widths[k]);
                GXColor4u8(r, g, b, alpha0);
                GXPosition3f32(lbl_11_bss_198[lbl_11_bss_60 - 1].x + ring[i].x * widths[k + 1],
                               lbl_11_bss_198[lbl_11_bss_60 - 1].y + ring[i].y * widths[k + 1],
                               lbl_11_bss_198[lbl_11_bss_60 - 1].z + ring[i].z * widths[k + 1]);
                GXColor4u8(r, g, b, alpha1);
            }
        }
    }
    else
    {
    float widths2[10] = {0.612f, 0.54f, 0.456f, 0.33f, 0.18f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    /* computed but unused here */
    if (0.0 == lbl_11_bss_5C && 1.0 == lbl_11_bss_D4)
        rainbow = TRUE;
    else
        rainbow = FALSE;
    dir2.x = lbl_11_bss_198[1].x - lbl_11_bss_198[0].x;
    dir2.y = lbl_11_bss_198[1].y - lbl_11_bss_198[0].y;
    dir2.z = lbl_11_bss_198[1].z - lbl_11_bss_198[0].z;
    toEye.x = lbl_11_bss_24->eye.x - lbl_11_bss_198[0].x;
    toEye.y = lbl_11_bss_24->eye.y - lbl_11_bss_198[0].y;
    toEye.z = lbl_11_bss_24->eye.z - lbl_11_bss_198[0].z;
    fwd.x = lbl_11_bss_198[1].x - lbl_11_bss_198[0].x;
    fwd.y = lbl_11_bss_198[1].y - lbl_11_bss_198[0].y;
    fwd.z = lbl_11_bss_198[1].z - lbl_11_bss_198[0].z;
    vec_normalize_len(&toEye);
    vec_normalize_len(&fwd);
    mathutil_vec_cross_prod(&toEye, &fwd, &lbl_11_bss_CBC[0]);
    vec_normalize_len(&lbl_11_bss_CBC[0]);
    side2 = lbl_11_bss_CBC[0];
    vec_normalize_len(&dir2);
    vec_normalize_len(&side2);
    for (i = 0; i < 16; i++)
    {
        ray_scale(&side2, &dir2, &ring2[i], i / 16.0f);
        vec_normalize_len(&ring2[i]);
    }
    ring2[16] = dir2;
    side2.x *= -1.0f;
    side2.y *= -1.0f;
    side2.z *= -1.0f;
    for (i = 17; i <= 32; i++)
    {
        ray_scale(&dir2, &side2, &ring2[i], (i - 16) / 16.0f);
        vec_normalize_len(&ring2[i]);
    }
    for (k = 0; k < 5; k++)
    {
        float a0;
        float a1;

        if (k <= 4)
        {
            a0 = (float)k / 5.0f;
            a0 *= a0;
        }
        else
        {
            a0 = (float)(9 - k) / 5.0f;
            a0 *= a0;
        }
        if (k + 1 <= 4)
        {
            a1 = (float)(k + 1) / 5.0f;
            a1 *= a1;
        }
        else
        {
            a1 = (float)(9 - (k + 1)) / 5.0f;
            a1 *= a1;
        }
        *(u8 *)&r2 = 0xD0;
        *(u8 *)&g2 = 0xD0;
        *(u8 *)&b2 = 0xFF;
        alpha2 = a0 * (208.0f * (50 - (g_font_char_var_3 - lbl_11_bss_F14)) * (50 - (g_font_char_var_3 - lbl_11_bss_F14)) / 2500.0f);
        alpha1 = a1 * (208.0f * (50 - (g_font_char_var_3 - lbl_11_bss_F14)) * (50 - (g_font_char_var_3 - lbl_11_bss_F14)) / 2500.0f);
        fn_11_1338C((u8 *)&r2, (u8 *)&g2, (u8 *)&b2, g_font_char_var_3 - lbl_11_bss_F14);
        GXBegin(0x98, 2, 66);
        for (i = 0; i <= 32; i++)
        {
            GXPosition3f32(lbl_11_bss_198[1].x + ring2[i].x * widths2[k],
                           lbl_11_bss_198[1].y + ring2[i].y * widths2[k],
                           lbl_11_bss_198[1].z + ring2[i].z * widths2[k]);
            GXColor4u8(r2, g2, b2, alpha2);
            GXPosition3f32(lbl_11_bss_198[1].x + ring2[i].x * widths2[k + 1],
                           lbl_11_bss_198[1].y + ring2[i].y * widths2[k + 1],
                           lbl_11_bss_198[1].z + ring2[i].z * widths2[k + 1]);
            GXColor4u8(r2, g2, b2, alpha1);
        }
    }
    }
}
