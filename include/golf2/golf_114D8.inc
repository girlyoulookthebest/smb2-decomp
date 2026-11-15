/* golf.c fn_11_114D8: shot trail ribbon (w1) */

/* Advances and draws the ribbon that follows the ball in flight (9 fading bands along
 * the lbl_11_bss_198 points, oriented to the camera; lbl_11_bss_CBC holds the side vectors). */
void fn_11_114D8(void)
{
    int i;
    struct Camera *camera;
    Vec toEye;
    Vec fwd;
    Vec eye;
    Vec toBall;
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    s8 r, g, b;
    int k;
    u8 alpha0;
    u8 alpha1;
    u8 rainbow;
    float scale;
    float dist;

    if (lbl_11_bss_DE != 0)
        camera = &cameras[0];
    else
        camera = lbl_11_bss_24;
    mtxa_from_mtx(camera->unk14C);
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
    GXSetBlendMode_cached(1, 4, 1, 0);
    GXSetCullMode_cached(0);
    GXSetTevColor(1, white);
    /* first frame after the shot: spread 30 points between the start and the first point */
    if (lbl_11_bss_60 == 2)
    {
        int j;

        lbl_11_bss_60 = 30;
        lbl_11_bss_198[30] = lbl_11_bss_198[1];
        for (j = 1; j < 30; j++)
        {
            lbl_11_bss_198[j].x = lbl_11_bss_198[0].x * (29 - j) / 30.0f + lbl_11_bss_198[30].x * (j + 1) / 30.0f;
            lbl_11_bss_198[j].y = lbl_11_bss_198[0].y * (29 - j) / 30.0f + lbl_11_bss_198[30].y * (j + 1) / 30.0f;
            lbl_11_bss_198[j].z = lbl_11_bss_198[0].z * (29 - j) / 30.0f + lbl_11_bss_198[30].z * (j + 1) / 30.0f;
        }
    }
    eye = camera->eye;
    for (i = lbl_11_bss_60 - 1; i >= 0; i--)
    {
        toEye.x = eye.x - lbl_11_bss_198[i].x;
        toEye.y = eye.y - lbl_11_bss_198[i].y;
        toEye.z = eye.z - lbl_11_bss_198[i].z;
        fwd.x = lbl_11_bss_198[i + 1].x - lbl_11_bss_198[i].x;
        fwd.y = lbl_11_bss_198[i + 1].y - lbl_11_bss_198[i].y;
        fwd.z = lbl_11_bss_198[i + 1].z - lbl_11_bss_198[i].z;
        vec_normalize_len(&toEye);
        vec_normalize_len(&fwd);
        mathutil_vec_cross_prod(&toEye, &fwd, &lbl_11_bss_CBC[i + 1]);
        vec_normalize_len(&lbl_11_bss_CBC[i + 1]);
        if (!(g_some_other_flags & 0xA))
        {
            lbl_11_bss_198[i + 2] = lbl_11_bss_198[i + 1];
            lbl_11_bss_198[i + 1] = lbl_11_bss_198[i];
        }
    }
    if (lbl_11_bss_60 >= 2)
        lbl_11_bss_CBC[lbl_11_bss_60 - 1] = lbl_11_bss_CBC[lbl_11_bss_60 - 2];
    if (!(g_some_other_flags & 0xA))
    {
        lbl_11_bss_198[0] = lbl_11_bss_20->pos;
        lbl_11_bss_60++;
        if (lbl_11_bss_60 > 49)
            lbl_11_bss_60 = 49;
    }
    if (lbl_11_bss_60 > 1)
    {
        toEye.x = eye.x - lbl_11_bss_198[0].x;
        toEye.y = eye.y - lbl_11_bss_198[0].y;
        toEye.z = eye.z - lbl_11_bss_198[0].z;
        fwd.x = lbl_11_bss_198[1].x - lbl_11_bss_198[0].x;
        fwd.y = lbl_11_bss_198[1].y - lbl_11_bss_198[0].y;
        fwd.z = lbl_11_bss_198[1].z - lbl_11_bss_198[0].z;
        vec_normalize_len(&toEye);
        vec_normalize_len(&fwd);
        mathutil_vec_cross_prod(&toEye, &fwd, &lbl_11_bss_CBC[0]);
        vec_normalize_len(&lbl_11_bss_CBC[0]);
    }
    if (lbl_11_bss_60 > 1)
    {
        float widths[10] = {-0.612f, -0.54f, -0.456f, -0.33f, -0.18f, 0.18f, 0.33f, 0.456f, 0.54f, 0.612f};

        if (0.0 == lbl_11_bss_5C && 1.0 == lbl_11_bss_D4)
            rainbow = TRUE;
        else
            rainbow = FALSE;
        /* stored as bytes; read back as s8 (matches the original's stores and loads) */
        *(u8 *)&r = 0xD0;
        *(u8 *)&g = 0xD0;
        *(u8 *)&b = 0xFF;
        scale = 1.0f;
        if (lbl_11_bss_DE != 0)
        {
            toBall.x = camera->eye.x - balls[modeCtrl.currPlayer].pos.x;
            toBall.y = camera->eye.y - balls[modeCtrl.currPlayer].pos.y;
            toBall.z = camera->eye.z - balls[modeCtrl.currPlayer].pos.z;
            dist = mathutil_vec_len(&toBall);
            if (dist < 50.0f)
                scale = 1.0f;
            else if (dist < 100.0)
                scale = 1.0 + (dist - 50.0f) / 100.0f;
            else
                scale = 1.0 + (dist - 100.0f) / 80.0f;
        }
        if (lbl_11_bss_60 != 49)
        {
            for (k = 0; k < 9; k++)
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
                GXBegin(0x98, 2, lbl_11_bss_60 * 2);
                for (i = lbl_11_bss_60 - 1; i >= 0; i--)
                {
                    alpha0 = a0 * (208.0f * (50 - i) * (50 - i) / 2500.0f);
                    alpha1 = a1 * (208.0f * (50 - i) * (50 - i) / 2500.0f);
                    if ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[0].button & 0x400) && i == 0)
                    {
                        alpha0 *= 0.05;
                        alpha1 *= 0.05;
                    }
                    if ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[0].button & 0x400) && i == 1)
                    {
                        alpha0 *= 0.1;
                        alpha1 *= 0.1;
                    }
                    if ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[0].button & 0x400) && i == 2)
                    {
                        alpha0 *= 0.2;
                        alpha1 *= 0.2;
                    }
                    if (rainbow || lbl_11_bss_DE != 0)
                        fn_11_1338C((u8 *)&r, (u8 *)&g, (u8 *)&b, i);
                    GXPosition3f32(lbl_11_bss_198[i].x + scale * lbl_11_bss_CBC[i].x * widths[k],
                                   lbl_11_bss_198[i].y + scale * lbl_11_bss_CBC[i].y * widths[k],
                                   lbl_11_bss_198[i].z + scale * lbl_11_bss_CBC[i].z * widths[k]);
                    GXColor4u8(r, g, b, alpha0);
                    GXPosition3f32(lbl_11_bss_198[i].x + scale * lbl_11_bss_CBC[i].x * widths[k + 1],
                                   lbl_11_bss_198[i].y + scale * lbl_11_bss_CBC[i].y * widths[k + 1],
                                   lbl_11_bss_198[i].z + scale * lbl_11_bss_CBC[i].z * widths[k + 1]);
                    GXColor4u8(r, g, b, alpha1);
                }
            }
        }
        else
        {
            /* full trail: the ribbon also narrows toward its tail */
            for (k = 0; k < 9; k++)
            {
                float b0;
                float b1;
                float taper;

                if (k <= 4)
                {
                    b0 = (float)k / 5.0f;
                    b0 *= b0;
                }
                else
                {
                    b0 = (float)(9 - k) / 5.0f;
                    b0 *= b0;
                }
                if (k + 1 <= 4)
                {
                    b1 = (float)(k + 1) / 5.0f;
                    b1 *= b1;
                }
                else
                {
                    b1 = (float)(9 - (k + 1)) / 5.0f;
                    b1 *= b1;
                }
                GXBegin(0x98, 2, lbl_11_bss_60 * 2);
                for (i = lbl_11_bss_60 - 1; i >= 0; i--)
                {
                    taper = 1.0 - 0.5 * (float)i / (lbl_11_bss_60 - 1);
                    alpha0 = b0 * (208.0f * (50 - i) * (50 - i) / 2500.0f);
                    alpha1 = b1 * (208.0f * (50 - i) * (50 - i) / 2500.0f);
                    if ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[0].button & 0x400) && i == 0)
                    {
                        alpha0 *= 0.2;
                        alpha1 *= 0.2;
                    }
                    if ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[0].button & 0x400) && i == 1)
                    {
                        alpha0 *= 0.6;
                        alpha1 *= 0.6;
                    }
                    if ((pad_status_groups[player_pad_map[modeCtrl.currPlayer]].unk0[0].button & 0x400) && i == 2)
                    {
                        alpha0 *= 0.7;
                        alpha1 *= 0.7;
                    }
                    if (rainbow || lbl_11_bss_DE != 0)
                        fn_11_1338C((u8 *)&r, (u8 *)&g, (u8 *)&b, i);
                    GXPosition3f32(lbl_11_bss_198[i].x + taper * (scale * lbl_11_bss_CBC[i].x * widths[k]),
                                   lbl_11_bss_198[i].y + taper * (scale * lbl_11_bss_CBC[i].y * widths[k]),
                                   lbl_11_bss_198[i].z + taper * (scale * lbl_11_bss_CBC[i].z * widths[k]));
                    GXColor4u8(r, g, b, alpha0);
                    GXPosition3f32(lbl_11_bss_198[i].x + taper * (scale * lbl_11_bss_CBC[i].x * widths[k + 1]),
                                   lbl_11_bss_198[i].y + taper * (scale * lbl_11_bss_CBC[i].y * widths[k + 1]),
                                   lbl_11_bss_198[i].z + taper * (scale * lbl_11_bss_CBC[i].z * widths[k + 1]));
                    GXColor4u8(r, g, b, alpha1);
                }
            }
        }
    }
}
