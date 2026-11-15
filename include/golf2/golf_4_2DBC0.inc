/* golf_4.c functions; see golf_4.c */

void fn_11_2DD44(char *str, s32 x, s32 y);
void fn_11_2E944(void);
void fn_11_2EA5C(s16 x, s16 y);
void fn_11_2EB0C(s16 x, s16 y);
void fn_11_2FDDC(s32 yOffset);
void fn_11_31040(s32 yOffset, s32 state);
void fn_11_311C0(s32 yOffset, s32 state);

/* Title bar: "MONKEY GOLF 2", the play mode and the course length. */
void fn_11_2DBC0(void)
{
    struct SpriteDrawRequest req = { 0x103C, 320.0f, 12.0f, 0.2f, 20.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x6, -1, 0 };

    draw_sprite_draw_request(&req);
    textdraw_set_font(0x8F);
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_font_style(2);
    textdraw_set_mul_color(0xFFFFFFFF);
    textdraw_set_add_color(0);
    textdraw_set_depth(0.1f);
    textdraw_set_alignment(0);
    textdraw_set_spacing(1.0f, 0.0f);
    fn_11_2DD44("MONKEY GOLF 2", 36, 25);
    textdraw_set_scale(0.5f, 0.6f);
    if (lbl_1_bss_1704.unk0 == 0)
        fn_11_2DD44("STROKE PLAY", 301, 35);
    else if (lbl_1_bss_1704.unk0 == 1)
        fn_11_2DD44("MATCH PLAY", 313, 35);
    fn_11_2DD44("-1 ROUND", 435, 35);
    if (lbl_1_bss_1704.unk1 == 0)
        fn_11_2DD44("18 HOLES-", 523, 35);
    else if (lbl_1_bss_1704.unk1 != 0)
        fn_11_2DD44("9 HOLES-", 523, 35);
}

/* Text with a black drop shadow. */
void fn_11_2DD44(char *str, s32 x, s32 y)
{
    textdraw_set_mul_color(0xFF000000);
    textdraw_set_pos(x + 2, y + 2);
    textdraw_set_depth(0.15f);
    textdraw_print(str);
    textdraw_set_mul_color(0xFFFFFFFF);
    textdraw_set_pos(x, y);
    textdraw_set_depth(0.1f);
    textdraw_print(str);
}

#define GOLF4_PORTRAIT(x, y)                                                   \
    player = modeCtrl.currPlayer;                                              \
    if (fn_11_F220(player) == 0)                                               \
        fn_11_1D260(x, y, active_monkey_id[player], 1);                        \
    else if (fn_11_F220(player) == 1)                                          \
        fn_11_1DB90(x, y, active_monkey_id[player], 1);                        \
    else if (fn_11_F220(player) == 2)                                          \
        fn_11_1D71C(x, y, active_monkey_id[player], 1);

/* Shot HUD while aiming: panels, portrait, wind, power gauge. power is the shot power (0.2 .. 1). */
void fn_11_2DE64(s16 *sel, float power)
{
    s32 player;

    draw_sprite_draw_request(&lbl_11_data_1594[0]);
    draw_sprite_draw_request(&lbl_11_data_12C4);
    draw_sprite_draw_request(&lbl_11_data_1314[lbl_11_bss_CA]);
    draw_sprite_draw_request(&lbl_11_data_1454[modeCtrl.currPlayer]);
    GOLF4_PORTRAIT(96.0f, 402.0f)
    fn_11_31040(0, lbl_11_bss_4C);
    fn_11_311C0(0, lbl_11_bss_4C);
    fn_11_2E944();
    fn_11_2EA5C(306.0 + 314.0 * (power - 0.2), 432);
    if (lbl_11_bss_D0 == 1)
        fn_11_2EB0C(306.0 + 314.0 * (0.8 * lbl_11_bss_D8), 432);
    fn_11_2FDDC(0);
}

/* Shot HUD sliding out (t 0 -> 1 moves it down by 120px). */
void fn_11_2E068(s16 *sel, float unused, float t)
{
    struct SpriteDrawRequest req;
    s32 player;

    req = lbl_11_data_1594[0];
    req.y += 120.0f * t;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_12C4;
    req.y += 120.0f * t;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_1314[lbl_11_bss_CA];
    req.y += 120.0f * t;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_1454[modeCtrl.currPlayer];
    req.y += 120.0f * t;
    draw_sprite_draw_request(&req);
    GOLF4_PORTRAIT(96.0f, 402.0f + 120.0f * t)
    fn_11_31040(120.0f * t, lbl_11_bss_4C);
    fn_11_311C0(120.0f * t, lbl_11_bss_4C);
    fn_11_311C0(120.0f * t, lbl_11_bss_4C);
    fn_11_2FDDC(120.0f * t);
}

/* Shot HUD sliding in/out by 360px. */
void fn_11_2E350(s16 *sel, float t)
{
    struct SpriteDrawRequest req;
    s32 player;

    req = lbl_11_data_1594[0];
    req.y += 360.0f * t;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_12C4;
    req.y += 360.0f * t;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_1314[lbl_11_bss_CA];
    req.y += 360.0f * t;
    draw_sprite_draw_request(&req);
    fn_11_31040(360.0f * t, lbl_11_bss_4C);
    req = lbl_11_data_1454[modeCtrl.currPlayer];
    req.y += 360.0f * t;
    draw_sprite_draw_request(&req);
    GOLF4_PORTRAIT(96.0f, 402.0f + 360.0f * t)
    fn_11_311C0(360.0f * t, lbl_11_bss_4C);
    fn_11_2EA5C(306, 432.0f + 360.0f * t);
    fn_11_2FDDC(360.0f * t);
}

/* fn_11_2E638's state, .bss 0x2AB8-0x2AC0. File statics: as function-local statics MWCC lays them
 * out in first-use order (0x2ABA first); golf4_pin_bss (unreferenced, dead-stripped) fixes the order. */
static s16 lbl_11_bss_2AB8;  /* club scroll timer */
static s8 lbl_11_bss_2ABA;   /* displayed club */
static s8 lbl_11_bss_2ABB;   /* scroll direction */
static u32 lbl_11_bss_2ABC;  /* unreferenced */

void golf4_pin_bss(void)
{
    void *p;

    p = &lbl_11_bss_2AB8;
    p = &lbl_11_bss_2ABA;
    p = &lbl_11_bss_2ABB;
    p = &lbl_11_bss_2ABC;
}

/* Shot HUD with the club selector; *sel is the wanted club, the shown one scrolls towards it. */
void fn_11_2E638(s16 *sel)
{
    s32 player;

    draw_sprite_draw_request(&lbl_11_data_1594[0]);
    draw_sprite_draw_request(&lbl_11_data_12C4);
    draw_sprite_draw_request(&lbl_11_data_1314[lbl_11_bss_CA]);
    draw_sprite_draw_request(&lbl_11_data_1454[modeCtrl.currPlayer]);
    GOLF4_PORTRAIT(96.0f, 402.0f)
    if (*sel != lbl_11_bss_2ABA) {
        if (lbl_11_bss_2AB8 > 0) {
            *sel = (lbl_11_bss_2ABA + lbl_11_bss_2ABB + 4) % 4;
        } else {
            lbl_11_bss_2AB8 = 16;
            if (lbl_11_bss_2ABA == 0 && *sel == 3)
                lbl_11_bss_2ABB = -1;
            else if (lbl_11_bss_2ABA == 3 && *sel == 0)
                lbl_11_bss_2ABB = 1;
            else if (lbl_11_bss_2ABA > *sel)
                lbl_11_bss_2ABB = -1;
            else if (lbl_11_bss_2ABA < *sel)
                lbl_11_bss_2ABB = 1;
        }
    }
    if (lbl_11_bss_2AB8 > 0) {
        lbl_11_bss_2AB8--;
        if (lbl_11_bss_2AB8 == 0) {
            call_SoundReqID_arg_0(0x112);
            lbl_11_bss_2ABA = (lbl_11_bss_2ABA + lbl_11_bss_2ABB + 4) % 4;
        }
    }
    fn_11_31040(0, lbl_11_bss_4C);
    fn_11_311C0(0, lbl_11_bss_4C);
    fn_11_2E944();
    fn_11_2EA5C(306, 432);
    fn_11_2FDDC(0);
}

/* Power gauge bar; its length follows the current shot power (*lbl_11_bss_58). */
void fn_11_2E944(void)
{
    s32 dummy;  /* the per-state cases set nothing that is used */
    struct SpriteDrawRequest req = { 0x102A, 306.0f, 433.0f, 0.1f, 1.0f, 1.75f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.9f, -1, 0x5, (s32)0xFFFF00FF, 0 };

    switch (lbl_11_bss_4C) {
    case 0:
        dummy = 0;
        break;
    case 1:
        dummy = 1;
        break;
    case 2:
        dummy = 2;
        break;
    case 3:
        dummy = 3;
        break;
    case 4:
        dummy = 4;
        break;
    case 5:
        dummy = 5;
        break;
    case 6:
        dummy = 6;
        break;
    case 7:
        dummy = 7;
        break;
    case 8:
        dummy = 8;
        break;
    case 9:
        dummy = 9;
        break;
    case 10:
        dummy = 10;
        break;
    case 11:
        dummy = 11;
        break;
    default:
        dummy = 12;
        break;
    }
    req.opacity = 0.8f;
    if (lbl_11_bss_58 != NULL) {
        req.x -= 252.0f * *lbl_11_bss_58 / 2.0f;
        req.scaleX = 252.0f * *lbl_11_bss_58 / 8.0f;
    }
    draw_sprite_draw_request(&req);
}

/* Stand-in for a function the linker stripped between fn_11_2E944 and fn_11_2EA5C: its
 * sprite initializer stays at .rodata 0x4DC. Unreferenced, so it is stripped again. */
void golf_stripped_2EA5C_sprite(void)
{
    struct SpriteDrawRequest req = { 1, 322.0f, 447.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0xE000, 1.0f, -1, 0x5, -1, 0 };

    draw_sprite_draw_request(&req);
}

/* Power gauge cursor at (x, y). */
void fn_11_2EA5C(s16 x, s16 y)
{
    struct SpriteDrawRequest req = { 0x1010, 322.0f, 432.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };

    req.x = x - 12;
    req.y = y - 36;
    draw_sprite_draw_request(&req);
}

/* Marker for the previous shot's power on the gauge. */
void fn_11_2EB0C(s16 x, s16 y)
{
    struct SpriteDrawRequest req = { 0x100F, 322.0f, 432.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };

    req.x = x - 12;
    req.y = y - 36;
    draw_sprite_draw_request(&req);
}

/* Mini-map: ball marker of player `player` (blinks in turn every 30 frames). */
void fn_11_2EBBC(u8 player)
{
    struct SpriteDrawRequest req;
    Vec center;
    float half;
    float left;
    float top;
    float width;
    float height;
    float x;
    float y;
    s32 blink;

    half = 10.0 + lbl_11_bss_29F0 / 2.0f;
    center = lbl_11_bss_29E4;
    left = center.x - 4.0f * half / 3.0f;
    top = center.z - half;
    width = (center.x + 4.0f * half / 3.0f) - left;
    height = (center.z + half) - top;
    x = balls[player].pos.x - left;
    y = balls[player].pos.z - top;
    x = 496.0f + 144.0f * x / width;
    y = 138.0f * y / height;
    req = lbl_11_data_1454[player];
    req.flags = 0xA;
    if (x < 500.0f)
        req.x = 500.0f;
    else
        req.x = x;
    if (y > 120.0f)
        req.y = 120.0f;
    else
        req.y = y;
    blink = g_font_char_var_3 % 120;
    blink = blink / 30.0;
    if (blink == player)
        req.z -= 0.005;
    draw_sprite_draw_request(&req);
}

/* Mini-map frame piece at (x, y), clipped against the map's right edge. */
void fn_11_2EDB4(s32 x, s32 y)
{
    struct SpriteDrawRequest req;

    req = lbl_11_data_4794;
    req.x = x;
    req.y = y;
    req.z = 1.1f;
    if (lbl_11_bss_24->unk1F == 21) {
        if (x <= 496 || y >= 144)
            draw_sprite_draw_request(&req);
    } else {
        if (x >= 480 && x < 496) {
            req.u2 = (496 - x) / 16.0f;
            req.scaleX = (496 - x) / 16.0f;
        }
        if (x < 496)
            draw_sprite_draw_request(&req);
    }
}

/* Mini-map: pin/cup marker. */
void fn_11_2EF2C(void)
{
    struct SpriteDrawRequest req;
    Vec center;
    float half;
    float left;
    float top;
    float width;
    float height;
    float x;
    float y;

    half = lbl_11_bss_29F0 / 2.0f;
    center = lbl_11_bss_29E4;
    left = center.x - 4.0f * half / 3.0f;
    top = center.z - half * lbl_1_bss_2440 / math_tan(0x1555);
    width = 2.0f * (4.0f * half / 3.0f);
    height = 2.0f * half * lbl_1_bss_2440 / math_tan(0x1555);
    x = lbl_11_bss_38.x - left;
    y = lbl_11_bss_38.z - top;
    x = 496.0f + 144.0f * x / width;
    y = 138.0f * y / height;
    req = lbl_11_data_4794;
    req.flags = 0xD;
    if (x < 500.0f)
        req.x = 500.0f;
    else
        req.x = x;
    if (y > 120.0f)
        req.y = 120.0f;
    else
        req.y = y;
    draw_sprite_draw_request(&req);
}

void fn_11_2F6C8(struct SpriteDrawRequest *req, float x, float y);

/* End-of-round message for a score at or under par (front/back nine only): banner, a cheer
 * animation and "try all 18 holes" text. */
void fn_11_2F0F8(void)
{
    struct SpriteDrawRequest req;
    s32 score;
    s32 hole;

    req.bmpId = 0x44;
    req.x = 320.0f;
    req.y = 155.0f;
    req.z = 0.11f;
    req.scaleX = 1.3f;
    req.scaleY = 0.9f;
    req.u1 = req.v1 = 0.0f;
    req.u2 = req.v2 = 1.0f;
    req.rotation = 0;
    req.opacity = 1.0f;
    req.unk30 = -1;
    req.flags = 0xA;
    req.mulColor = -1;
    req.addColor = 0;
    fn_11_2F6C8(&req, 416.0f, 128.0f);
    score = 0;
    if (lbl_1_bss_1704.unk1 == 0 || lbl_1_bss_1704.unk1 == 1) {
        for (hole = 0; hole < fn_11_EC30(); hole++)
            score += fn_11_EC00(modeCtrl.currPlayer, hole) - fn_11_16FC4(hole);
    } else {
        for (hole = 9; hole < fn_11_EC30(); hole++)
            score += fn_11_EC00(modeCtrl.currPlayer, hole) - fn_11_16FC4(hole);
    }
    textdraw_reset();
    textdraw_set_mul_color(0xFFFF00);
    textdraw_set_depth(0.1f);
    if (score == 0) {
        if (APE_BODY(balls[modeCtrl.currPlayer].ape)->clock.loops >= 2
         || (fn_1_11465C(balls[modeCtrl.currPlayer].ape, 0, 0) && fn_1_1146A4(balls[modeCtrl.currPlayer].ape, 0, 0)))
            fn_1_1132D0(balls[modeCtrl.currPlayer].ape, 0, 7, lbl_11_data_1210[(u32)(rand() & 0x7FFF) % 5]);
        if (locale == 5) {
            textdraw_set_font(0x8F);
            textdraw_set_alignment(3);
            textdraw_set_pos(320.0f, 115.0f);
            textdraw_print("\x82\xA0\x82\xC8\x82\xBD\x82\xCD\x81" "A\x82\xC8\x82\xA9\x82\xC8\x82\xA9\x82\xCC\x82\xA4\x82\xC5\x82\xDC\x82\xA6\x82\xC5\x82\xB7\x81" "B");
            textdraw_set_pos(320.0f, 144.0f);
            textdraw_print("\x83z\x81[\x83\x8B\x82\xB7\x82\xA4\x82\xF0" "18\x83z\x81[\x83\x8B\x82\xC9\x82\xB5\x82\xC4\x83v\x83\x8C\x83" "C\x82\xB7\x82\xEA\x82\xCE");
            textdraw_set_pos(320.0f, 173.0f);
            textdraw_print("\x83\x89\x83\x93\x83L\x83\x93\x83O\x82\xC9\x83`\x83\x83\x83\x8C\x83\x93\x83W\x82\xB7\x82\xE9\x82\xB1\x82\xC6\x82\xAA\x82\xC5\x82\xAB\x82\xDC\x82\xB7\x81" "B");
        } else if (locale == 0) {
            textdraw_set_font(3);
            textdraw_set_flags(0x81000000);
            textdraw_set_scale(0.85f, 1.0f);
            textdraw_set_alignment(3);
            textdraw_set_pos(320.0f, 115.0f);
            textdraw_print("You're pretty good.");
            textdraw_set_pos(320.0f, 144.0f);
            textdraw_print("If you play all 18 holes, you can try");
            textdraw_set_pos(320.0f, 173.0f);
            textdraw_print("to get your name in the Rankings.");
            textdraw_set_alignment(0);
        }
        textdraw_reset();
        textdraw_set_font(9);
        textdraw_set_mul_color(0xFFA000);
        textdraw_set_depth(0.1f);
        textdraw_set_scale(0.75f, 0.75f);
        textdraw_set_pos(104.0f, 40.0f);
        textdraw_print("EVEN   PAR");
    } else {
        if (APE_BODY(balls[modeCtrl.currPlayer].ape)->clock.loops >= 2
         || (fn_1_11465C(balls[modeCtrl.currPlayer].ape, 0, 0) && fn_1_1146A4(balls[modeCtrl.currPlayer].ape, 0, 0)))
            fn_1_1132D0(balls[modeCtrl.currPlayer].ape, 0, 7, lbl_11_data_1210[(u32)(rand() & 0x7FFF) % 5]);
        if (locale == 5) {
            textdraw_set_font(0x8F);
            textdraw_set_alignment(3);
            textdraw_set_pos(320.0f, 115.0f);
            textdraw_print("\x82\xA8\x82\xDF\x82\xC5\x82\xC6\x82\xA4\x82\xB2\x82\xB4\x82\xA2\x82\xDC\x82\xB7! \x82\xA2\x82\xA2\x83X\x83R\x83" "A\x82\xC5\x82\xB5\x82\xBD!");
            textdraw_set_pos(320.0f, 144.0f);
            textdraw_print("\x82\xC2\x82\xAC\x82\xCD\x83z\x81[\x83\x8B\x82\xB7\x82\xA4\x82\xF0" "18\x83z\x81[\x83\x8B\x82\xC9\x82\xB5\x82\xC4\x83v\x83\x8C\x83" "C\x82\xB5\x82\xC4");
            textdraw_set_pos(320.0f, 173.0f);
            textdraw_print("\x83\x89\x83\x93\x83L\x83\x93\x83O\x82\xC9\x83`\x83\x83\x83\x8C\x83\x93\x83W\x82\xB5\x82\xC4\x82\xDD\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81" "B");
        } else if (locale == 0) {
            textdraw_set_font(3);
            textdraw_set_flags(0x81000000);
            textdraw_set_scale(0.77f, 1.0f);
            textdraw_set_alignment(3);
            textdraw_set_pos(320.0f, 115.0f);
            textdraw_print("Congratulations! That's a great score!");
            textdraw_set_pos(320.0f, 144.0f);
            textdraw_print("Now try playing all 18 holes");
            textdraw_set_pos(320.0f, 173.0f);
            textdraw_print("to get your name in the Rankings!");
            textdraw_set_alignment(0);
        }
        textdraw_reset();
        textdraw_set_font(9);
        textdraw_set_mul_color(0xFF4600);
        textdraw_set_depth(0.1f);
        textdraw_set_scale(0.75f, 0.75f);
        textdraw_set_pos(93.2f, 40.0f);
        textdraw_print("UNDER  PAR");
    }
}

/* Nine-slice box: draws the 6px-border texture of *base stretched to (texW, texH) texels at base's
 * position and scale (centre, 4 edges, 4 corners). */
void fn_11_2F6C8(struct SpriteDrawRequest *base, float texW, float texH)
{
    struct SpriteDrawRequest req;
    float w = texW * base->scaleX;
    float h = texH * base->scaleY;

    /* centre */
    req = *base;
    req.u1 = 6.0 / texW;
    req.v1 = 6.0 / texH;
    req.u2 = 1.0 - 6.0 / texW;
    req.v2 = 1.0 - 6.0 / texH;
    req.scaleX = (w - 12.0) / texW;
    req.scaleY = (h - 12.0) / texH;
    draw_sprite_draw_request(&req);
    /* top edge */
    req = *base;
    req.y = 3.0 + (base->y - 0.5 * h);
    req.u1 = 6.0 / texW;
    req.v1 = 0.0f;
    req.u2 = 1.0 - 6.0 / texW;
    req.v2 = 6.0 / texH;
    req.scaleX = (w - 12.0) / texW;
    req.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&req);
    /* bottom edge */
    req = *base;
    req.y = (base->y + 0.5 * h) - 3.0;
    req.u1 = 6.0 / texW;
    req.v1 = 1.0 - 6.0 / texH;
    req.u2 = 1.0 - 6.0 / texW;
    req.v2 = 1.0f;
    req.scaleX = (w - 12.0) / texW;
    req.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&req);
    /* left edge */
    req = *base;
    req.x = 3.0 + (base->x - 0.5 * w);
    req.u1 = 0.0f;
    req.v1 = 6.0 / texH;
    req.u2 = 6.0 / texW;
    req.v2 = 1.0 - 6.0 / texH;
    req.scaleX = 6.0 / texW;
    req.scaleY = (h - 12.0) / texH;
    draw_sprite_draw_request(&req);
    /* right edge */
    req = *base;
    req.x = (base->x + 0.5 * w) - 3.0;
    req.u1 = 1.0 - 6.0 / texW;
    req.v1 = 6.0 / texH;
    req.u2 = 1.0f;
    req.v2 = 1.0 - 6.0 / texH;
    req.scaleX = 6.0 / texW;
    req.scaleY = (h - 12.0) / texH;
    draw_sprite_draw_request(&req);
    /* top-left */
    req = *base;
    req.x = 3.0 + (base->x - 0.5 * w);
    req.y = 3.0 + (base->y - 0.5 * h);
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 6.0 / texW;
    req.v2 = 6.0 / texH;
    req.scaleX = 6.0 / texW;
    req.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&req);
    /* top-right */
    req = *base;
    req.x = (base->x + 0.5 * w) - 3.0;
    req.y = 3.0 + (base->y - 0.5 * h);
    req.u1 = 1.0 - 6.0 / texW;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 6.0 / texH;
    req.scaleX = 6.0 / texW;
    req.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&req);
    /* bottom-left */
    req = *base;
    req.x = 3.0 + (base->x - 0.5 * w);
    req.y = (base->y + 0.5 * h) - 3.0;
    req.u1 = 0.0f;
    req.v1 = 1.0 - 6.0 / texH;
    req.u2 = 6.0 / texW;
    req.v2 = 1.0f;
    req.scaleX = 6.0 / texW;
    req.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&req);
    /* bottom-right */
    req = *base;
    req.x = (base->x + 0.5 * w) - 3.0;
    req.y = (base->y + 0.5 * h) - 3.0;
    req.u1 = 1.0 - 6.0 / texW;
    req.v1 = 1.0 - 6.0 / texH;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = 6.0 / texW;
    req.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&req);
}

/* Club/shot-type indicator panel under the HUD (three stacked tiles plus a label tile), by shot
 * state lbl_11_bss_4C; yOffset slides it with the HUD. */
void fn_11_2FDDC(s32 yOffset)
{
    struct SpriteDrawRequest a;
    struct SpriteDrawRequest b;
    struct SpriteDrawRequest c;
    struct SpriteDrawRequest d;
    u32 state;

    b = lbl_11_data_3754[0];
    c = lbl_11_data_3754[0];
    d = lbl_11_data_3BB4;
    if (lbl_11_bss_4C == 12 || lbl_11_bss_4C == 13 || lbl_11_bss_4C == 14)
        d = lbl_11_data_3B64;
    state = lbl_11_bss_4C;
    a.bmpId = -1;
    switch (state) {
    case 0:
        a = lbl_11_data_3754[2];
        b = lbl_11_data_3754[5];
        break;
    case 1:
        a = lbl_11_data_3754[2];
        b = lbl_11_data_3754[2];
        c = lbl_11_data_3754[5];
        break;
    case 2:
        a = lbl_11_data_3754[2];
        b = lbl_11_data_3754[1];
        break;
    case 3:
        a = lbl_11_data_3754[1];
        b = lbl_11_data_3754[9];
        c = lbl_11_data_3754[5];
        break;
    case 4:
        a = lbl_11_data_3754[1];
        b = lbl_11_data_3754[8];
        break;
    case 5:
        a = lbl_11_data_3754[1];
        b = lbl_11_data_3754[6];
        c = lbl_11_data_3754[5];
        break;
    case 6:
        a = lbl_11_data_3754[1];
        b = lbl_11_data_3754[5];
        c = lbl_11_data_3754[5];
        break;
    case 7:
        a = lbl_11_data_3754[1];
        b = lbl_11_data_3754[4];
        c = lbl_11_data_3754[5];
        break;
    case 8:
        a = lbl_11_data_3754[1];
        b = lbl_11_data_3754[3];
        c = lbl_11_data_3754[5];
        break;
    case 9:
        a = lbl_11_data_3754[1];
        b = lbl_11_data_3754[2];
        break;
    case 10:
        a = lbl_11_data_3754[1];
        b = lbl_11_data_3754[0];
        c = lbl_11_data_3754[5];
        break;
    case 11:
        b = lbl_11_data_3754[7];
        break;
    case 12:
        if (locale == 0) {
            a = lbl_11_data_3754[1];
            b = lbl_11_data_3754[5];
        } else if (locale == 5) {
            b = lbl_11_data_3754[5];
        }
        break;
    case 13:
        if (locale == 0)
            b = lbl_11_data_3754[9];
        else if (locale == 5)
            b = lbl_11_data_3754[3];
        break;
    case 14:
        if (locale == 0)
            b = lbl_11_data_3754[3];
        else if (locale == 5)
            b = lbl_11_data_3754[1];
        break;
    }
    if (a.bmpId != -1) {
        a.scaleX *= 0.7f;
        a.scaleY *= 0.8f;
    }
    b.scaleX *= 0.7f;
    b.scaleY *= 0.8f;
    c.scaleX *= 0.7f;
    c.scaleY *= 0.8f;
    d.scaleX *= 0.7f;
    d.scaleY *= 0.8f;
    if (a.bmpId != -1) {
        a.x = 171.0f;
        a.y = yOffset + 431;
        a.mulColor = (s32)0xFF2238A7;
    }
    b.x = 185.0f;
    b.y = yOffset + 431;
    b.mulColor = (s32)0xFF2238A7;
    c.x = 199.0f;
    c.y = yOffset + 431;
    c.mulColor = (s32)0xFF2238A7;
    d.x = 213.0f;
    d.y = yOffset + 431;
    d.mulColor = (s32)0xFF2238A7;
    if (a.bmpId != -1)
        draw_sprite_draw_request(&a);
    draw_sprite_draw_request(&b);
    draw_sprite_draw_request(&c);
    draw_sprite_draw_request(&d);
}

/* Pin flag marker with the current character's face, projected to the screen over the hole,
 * when the pin is farther from the ball than the camera. */
void fn_11_3071C(void)
{
    struct SpriteDrawRequest req;
    Vec toCam;
    Vec v;

    toCam.x = cameras[modeCtrl.currPlayer].eye.x - balls[modeCtrl.currPlayer].pos.x;
    toCam.y = cameras[modeCtrl.currPlayer].eye.y - balls[modeCtrl.currPlayer].pos.y;
    toCam.z = cameras[modeCtrl.currPlayer].eye.z - balls[modeCtrl.currPlayer].pos.z;
    v.x = lbl_11_bss_198[lbl_11_bss_50 - 1].x - balls[modeCtrl.currPlayer].pos.x;
    v.y = lbl_11_bss_198[lbl_11_bss_50 - 1].y - balls[modeCtrl.currPlayer].pos.y;
    v.z = lbl_11_bss_198[lbl_11_bss_50 - 1].z - balls[modeCtrl.currPlayer].pos.z;
    if (mathutil_vec_len(&toCam) < mathutil_vec_len(&v))
        return;
    mtxa_from_mtxb();
    mtxa_tf_point(&balls[modeCtrl.currPlayer].pos, &v);
    fn_11_139C8(v.x, v.y, -v.z, &toCam.x, &toCam.y, lbl_11_bss_24->sub28.view.tanHalfFov);
    toCam.x /= lbl_1_bss_2434 / 640.0f;
    toCam.y /= (lbl_1_bss_2434 / g_camera_aspect_ratio) / 480.0f;
    if (cameras[modeCtrl.currPlayer].unk1F == 22 && (toCam.x < 110.0f || toCam.x > 480.0f))
        return;
    if (v.z < 0.0) {
        set_ui_widescreen_scale_mtx(toCam.x);
        req = lbl_11_data_1274;
        req.x = 21.0f + toCam.x;
        req.y = 25.0f + toCam.y;
        draw_sprite_draw_request(&req);
        req = lbl_11_data_3614[active_monkey_id[modeCtrl.currPlayer]];
        req.x = 21.0f + (toCam.x - 17.0f);
        req.y = 25.0f + (toCam.y - 66.0f);
        draw_sprite_draw_request(&req);
        reset_ui_widescreen_scale_mtx();
    }
}

/* Wind speed readout: unit label and up to three digits (tens, units, tenths).
 * speed is in m/s; the English version shows mph. */
/* uv of digit n in the 5x2 digit sheet */
static void golf4_wind_digit_uv(struct SpriteDrawRequest *req, s32 n)
{
    switch (n % 5) {
    case 0:
        req->u1 = 0.0f;
        req->u2 = 0.2f;
        break;
    case 1:
        req->u1 = 0.2f;
        req->u2 = 0.4f;
        break;
    case 2:
        req->u1 = 0.4f;
        req->u2 = 0.6f;
        break;
    case 3:
        req->u1 = 0.6f;
        req->u2 = 0.8f;
        break;
    case 4:
        req->u1 = 0.8f;
        req->u2 = 1.0f;
        break;
    }
    if (n < 5) {
        req->v1 = 0.0f;
        req->v2 = 0.5f;
    } else {
        req->v1 = 0.5f;
        req->v2 = 1.0f;
    }
}

void g_smth_with_golf_wind_velocity(float speed)
{
    struct SpriteDrawRequest label = { 0x103A, 0.0f, 0.0f, 0.11f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, -1, 0 };
    struct SpriteDrawRequest digit = { 0x103B, 0.0f, 0.0f, 0.1f, 0.2f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x5, (s32)0xFF5A401B, 0 };
    struct SpriteDrawRequest req;
    s32 n;

    if (locale == 0)
        speed = 3600.0f * (speed / 1609.0f);
    label.x = 25.0f;
    label.y = 210.0f;
    draw_sprite_draw_request(&label);
    n = speed / 10.0;
    speed -= n * 10;
    if (n != 0) {
        req = digit;
        req.x = 42.0f;
        if (locale == 0)
            req.x -= 6.0f;
        req.y = 276.0f;
        golf4_wind_digit_uv(&req, n);
        draw_sprite_draw_request(&req);
    }
    n = speed;
    speed -= n;
    req = digit;
    req.x = 51.0f;
    if (locale == 0)
        req.x -= 6.0f;
    req.y = 276.0f;
    golf4_wind_digit_uv(&req, n);
    draw_sprite_draw_request(&req);
    n = 10.0 * speed;
    req = digit;
    req.x = 64.0f;
    if (locale == 0)
        req.x -= 6.0f;
    req.y = 276.0f;
    golf4_wind_digit_uv(&req, n);
    draw_sprite_draw_request(&req);
}

/* Shot-state icon (7 rows x 2 columns of a sheet, picked by state) at the HUD's left. */
void fn_11_31040(s32 yOffset, s32 state)
{
    struct SpriteDrawRequest req;

    req = lbl_11_data_1224;
    if (state % 2 == 0 || state > 12) {
        req.u1 = 0.0f;
        req.u2 = 0.5f;
    } else {
        req.u1 = 0.5f;
        req.u2 = 1.0f;
    }
    if (state < 2) {
        req.v1 = 0.0f;
        req.v2 = 0.14285715f;
    } else if (state < 4) {
        req.v1 = 0.14285715f;
        req.v2 = 0.2857143f;
    } else if (state < 6) {
        req.v1 = 0.2857143f;
        req.v2 = 0.42857143f;
    } else if (state < 8) {
        req.v1 = 0.42857143f;
        req.v2 = 0.5714286f;
    } else if (state < 10) {
        req.v1 = 0.5714286f;
        req.v2 = 0.71428573f;
    } else if (state < 12) {
        req.v1 = 0.71428573f;
        req.v2 = 0.85714287f;
    } else {
        req.v1 = 0.85714287f;
        req.v2 = 1.0f;
    }
    req.x = 103.0f;
    req.y = yOffset + 441;
    draw_sprite_draw_request(&req);
}

/* Shot-phase caption tile (aim / power / ...) by state. */
void fn_11_311C0(s32 yOffset, s32 state)
{
    struct SpriteDrawRequest req;

    req = lbl_11_data_1224;
    if (state < 3)
        req = lbl_11_data_1774;
    else if (state < 6)
        req = lbl_11_data_17C4;
    else if (state < 9)
        req = lbl_11_data_1814;
    else
        req = lbl_11_data_1864;
    req.y += yOffset;
    draw_sprite_draw_request(&req);
}
