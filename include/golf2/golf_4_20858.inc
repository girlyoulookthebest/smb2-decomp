/* golf_4.c functions; see golf_4.c */

/* .text 0x20858-0x252A8 (worker w8) */

/* x of digit slot `pos` of the shot-info panel numbers */
#define HUD_SLOT_X(pos) ((pos) * 19 + 304)

/* Loads `req` with `load`, then draws it at (px, py) moved by (ox, oy): first a black drop shadow
 * 2px down-right and slightly behind, then the sprite itself in `color`. Every argument is
 * expanded twice (the original evaluates them twice too, see the '+'/'-' colour assignments). */
#define HUD_DRAW_SHADOWED_LOAD(load, px, py, ox, oy, color) \
    do { \
        load; \
        req.x = (px); \
        req.y = (py); \
        req.x += (ox); \
        req.y += (oy); \
        req.mulColor = (color); \
        req.x += 2.0f; \
        req.y += 2.0f; \
        req.z += 0.01; \
        req.mulColor = 0; \
        draw_sprite_draw_request(&req); \
        load; \
        req.x = (px); \
        req.y = (py); \
        req.x += (ox); \
        req.y += (oy); \
        req.mulColor = (color); \
        draw_sprite_draw_request(&req); \
    } while (0)
#define HUD_DRAW_SHADOWED(src, px, py, ox, oy, color) HUD_DRAW_SHADOWED_LOAD(req = (src), px, py, ox, oy, color)

/* the distance unit after the number */
#define HUD_DRAW_UNIT(px, py, ox, oy, color) \
    HUD_DRAW_SHADOWED_LOAD(if (lbl_11_bss_7C) req = lbl_11_data_3B64; else req = lbl_11_data_3BB4, px, py, ox, oy, color)

/* Shot-info panel: the distance to the pin (fn_11_ECA8) on the first line and the height difference
 * (fn_11_EDF8, signed, coloured by sign) on the second, each as 1-3 digits, '.', one decimal and the unit. */
void fn_11_20858(void)
{
    struct SpriteDrawRequest req;
    float f;
    s32 d;
    s32 pos;
    s32 color;
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;

    /* line offsets: the numbers are right-aligned by moving them left by their digit count */
    f = fn_11_ECA8();
    if (f >= 1000.0)
        f = 999.999f;
    if (f <= -1000.0)
        f = -999.999f;
    if (f >= 100.0)
        x1 = -44;
    else if (f >= 10.0)
        x1 = -25;
    else
        x1 = -6;
    y1 = 122;

    f = fn_11_EDF8();
    f *= 100.0f;
    f = mathutil_floor(f);
    f /= 100.0f;
    if (f >= 1000.0)
        f = 999.999f;
    if (f <= -1000.0)
        f = -999.999f;
    if (-0.01 < f && f < 0.01)
        x2 = -6;
    else if (-10.0 < f && f < 10.0)
        x2 = -25;
    else
        x2 = -44;
    y2 = 176;

    /* labels */
    req = lbl_11_data_3C54;
    req.x += -228.0f;
    req.y += 78.0f;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_3CA4;
    req.x += -228.0f;
    req.y += 106.0f;
    draw_sprite_draw_request(&req);

    /* distance */
    pos = 1;
    f = fn_11_ECA8();
    if (f >= 1000.0)
        f = 999.999f;
    if (f <= -1000.0)
        f = -999.999f;
    if (f >= 100.0)
    {
        d = f;
        d /= 100;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        f -= d * 100;
        d = f;
        d /= 10;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        f -= d * 10;
        d = f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        HUD_DRAW_SHADOWED(lbl_11_data_3C04, HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        f -= d;
        d = 10.0f * f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], 8.0 + HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        f -= d / 10.0f;
        d = 100.0f * f;
        HUD_DRAW_UNIT(8.0 + HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
    }
    else if (f >= 10.0)
    {
        d = f;
        d /= 10;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        f -= d * 10;
        d = f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        HUD_DRAW_SHADOWED(lbl_11_data_3C04, HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        f -= d;
        d = 10.0f * f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], 8.0 + HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        f -= d / 10.0f;
        d = 100.0f * f;
        HUD_DRAW_UNIT(8.0 + HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
    }
    else
    {
        d = f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        HUD_DRAW_SHADOWED(lbl_11_data_3C04, HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        f -= d;
        d = 10.0f * f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], 8.0 + HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
        pos++;
        f -= d / 10.0f;
        d = 100.0f * f;
        HUD_DRAW_UNIT(8.0 + HUD_SLOT_X(pos), 22.0f, x1 - 252, y1 - 22, 0xFFFFE300);
    }

    /* height difference */
    pos = 1;
    f = fn_11_EDF8();
    f *= 100.0f;
    f = mathutil_floor(f);
    f /= 100.0f;
    if (f >= 1000.0)
        f = 999.999f;
    if (f <= -1000.0)
        f = -999.999f;
    if (-0.01 < f && f < 0.01)
        f = 0.0f;
    if (fabs(f) >= 100.0)
    {
        /* no room for the sign: only the colour tells it */
        if (f > 0.0)
            color = 0xFF1F9CFF;
        else if (f == 0.0)
            color = 0xFFFFFFFF;
        else
            color = 0xFFFF0900;
        f = fabs(f);
        d = f;
        d /= 100;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        f -= d * 100;
        d = f;
        d /= 10;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        f -= d * 10;
        d = f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        HUD_DRAW_SHADOWED(lbl_11_data_3C04, HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        f -= d;
        d = 10.0f * f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], 8.0 + HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        f -= d / 10.0f;
        d = 100.0f * f;
        HUD_DRAW_UNIT(8.0 + HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
    }
    else if (fabs(f) >= 10.0)
    {
        if (f > 0.0)
        {
            HUD_DRAW_SHADOWED(lbl_11_data_3AC4[0], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color = 0xFF1F9CFF);
            pos++;
        }
        else if (f == 0.0)
            color = 0xFFFFFFFF;
        else
        {
            HUD_DRAW_SHADOWED(lbl_11_data_3AC4[1], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color = 0xFFFF0900);
            pos++;
        }
        f = fabs(f);
        d = f;
        d /= 10;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        f -= d * 10;
        d = f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        HUD_DRAW_SHADOWED(lbl_11_data_3C04, HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        f -= d;
        d = 10.0f * f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], 8.0 + HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        f -= d / 10.0f;
        d = 100.0f * f;
        HUD_DRAW_UNIT(8.0 + HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
    }
    else
    {
        if (f > 0.0)
        {
            HUD_DRAW_SHADOWED(lbl_11_data_3AC4[0], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color = 0xFF1F9CFF);
            pos++;
        }
        else if (f == 0.0)
            color = 0xFFFFFFFF;
        else
        {
            HUD_DRAW_SHADOWED(lbl_11_data_3AC4[1], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color = 0xFFFF0900);
            pos++;
        }
        HUD_DRAW_SHADOWED(lbl_11_data_3754[(d = (f = fabs(f)))], HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        HUD_DRAW_SHADOWED(lbl_11_data_3C04, HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        f -= d;
        d = 10.0f * f;
        HUD_DRAW_SHADOWED(lbl_11_data_3754[d], 8.0 + HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
        pos++;
        f -= d / 10.0f;
        d = 100.0f * f;
        HUD_DRAW_UNIT(8.0 + HUD_SLOT_X(pos), 44.0f, x2 - 252, y2 - 44, color);
    }
}

#undef HUD_SLOT_X
#undef HUD_DRAW_SHADOWED_LOAD
#undef HUD_DRAW_SHADOWED
#undef HUD_DRAW_UNIT
