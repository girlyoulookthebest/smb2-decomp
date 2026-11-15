/* golf_4.c functions 0x1D260-0x20858; see golf_4.c */

/* Character portrait animation (ping-pong over the frame list) plus a fading trail copy.
 * mode 0: idle wobble, mode 1: spins faster the faster the current ball moves. */
void fn_11_1D260(float x, float y, s32 chara, u8 mode)
{
    struct SpriteDrawRequest *req;
    struct GolfFrameList *anim;
    int prevFrame;
    int frame;

    if (chara < 0 || chara >= 4)
        return;
    anim = &lbl_11_data_49E4[chara];
    frame = (g_font_char_var_3 >> 3) % (anim->count * 2 - 2);
    if (frame >= anim->count)
        frame = (anim->count * 2 - 2) - frame;
    prevFrame = ((g_font_char_var_3 >> 3) - 1) % (anim->count * 2 - 2);
    if (prevFrame >= anim->count)
        prevFrame = (anim->count * 2 - 2) - prevFrame;
    req = &lbl_11_data_48F4;
    req->bmpId = anim->frames[frame];
    if (mode == 0) {
        req->x = 21.5 + x;
        req->y = 14.0 + y;
        req->scaleX = 0.3359375f;
        req->scaleY = 0.21875f;
        if (fn_11_EEFC())
            req->rotation = 4096.0f * math_sin(g_font_char_var_3 * 0x190 + 0x4000);
        else
            req->rotation = 0;
    } else if (mode == 1 && !(g_some_other_flags & 0xA)) {
        req->x = 27.0 + x;
        req->y = 18.0 + y;
        req->scaleX = 0.421875f;
        req->scaleY = 0.28125f;
        if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.2)
            req->rotation += 0x2000;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.15)
            req->rotation += 0x1000;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.1)
            req->rotation += 0x800;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.06)
            req->rotation += 0x400;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.02)
            req->rotation += 0x200;
        else if (req->rotation <= 0x100 || 0xFF00 <= req->rotation)
            req->rotation = 0;
        else if (req->rotation > 0x8000)
            req->rotation += 0x100;
        else if (req->rotation <= 0x8000)
            req->rotation -= 0x100;
        if (req->rotation > 0x10000)
            req->rotation -= 0x10000;
    }
    req->z = 0.06f;
    req->opacity = 1.0f;
    draw_sprite_draw_request(req);
    req = &lbl_11_data_48F4;
    req->bmpId = anim->frames[prevFrame];
    req->z = 0.05f;
    req->opacity = 1.0 - (g_font_char_var_3 & 7) / 8.0;
    draw_sprite_draw_request(req);
}

/* Same as fn_11_1D260 with the second frame set (no idle wobble). */
void fn_11_1D71C(float x, float y, s32 chara, u8 mode)
{
    struct SpriteDrawRequest *req;
    struct GolfFrameList *anim;
    int prevFrame;
    int frame;

    if (chara < 0 || chara >= 4)
        return;
    anim = &lbl_11_data_4A04[chara];
    frame = (g_font_char_var_3 >> 3) % (anim->count * 2 - 2);
    if (frame >= anim->count)
        frame = (anim->count * 2 - 2) - frame;
    prevFrame = ((g_font_char_var_3 >> 3) - 1) % (anim->count * 2 - 2);
    if (prevFrame >= anim->count)
        prevFrame = (anim->count * 2 - 2) - prevFrame;
    req = &lbl_11_data_4944;
    req->bmpId = anim->frames[frame];
    if (mode == 0) {
        req->x = 21.5 + x;
        req->y = 14.0 + y;
        req->scaleX = 0.3359375f;
        req->scaleY = 0.21875f;
        req->rotation = 0;
    } else if (mode == 1 && !(g_some_other_flags & 0xA)) {
        req->x = 27.0 + x;
        req->y = 18.0 + y;
        req->scaleX = 0.421875f;
        req->scaleY = 0.28125f;
        if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.2)
            req->rotation += 0x2000;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.15)
            req->rotation += 0x1000;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.1)
            req->rotation += 0x800;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.06)
            req->rotation += 0x400;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.02)
            req->rotation += 0x200;
        else if (req->rotation <= 0x100 || 0xFF00 <= req->rotation)
            req->rotation = 0;
        else if (req->rotation > 0x8000)
            req->rotation += 0x100;
        else if (req->rotation <= 0x8000)
            req->rotation -= 0x100;
        if (req->rotation > 0x10000)
            req->rotation -= 0x10000;
    }
    req->z = 0.06f;
    req->opacity = 1.0f;
    draw_sprite_draw_request(req);
    req = &lbl_11_data_4944;
    req->bmpId = anim->frames[prevFrame];
    req->z = 0.05f;
    req->opacity = 1.0 - (g_font_char_var_3 & 7) / 8.0;
    draw_sprite_draw_request(req);
}

/* Same as fn_11_1D260 with the third frame set (no idle wobble). */
void fn_11_1DB90(float x, float y, s32 chara, u8 mode)
{
    struct SpriteDrawRequest *req;
    struct GolfFrameList *anim;
    int prevFrame;
    int frame;

    if (chara < 0 || chara >= 4)
        return;
    anim = &lbl_11_data_4A24[chara];
    frame = (g_font_char_var_3 >> 3) % (anim->count * 2 - 2);
    if (frame >= anim->count)
        frame = (anim->count * 2 - 2) - frame;
    prevFrame = ((g_font_char_var_3 >> 3) - 1) % (anim->count * 2 - 2);
    if (prevFrame >= anim->count)
        prevFrame = (anim->count * 2 - 2) - prevFrame;
    req = &lbl_11_data_4994;
    req->bmpId = anim->frames[frame];
    if (mode == 0) {
        req->x = 21.5 + x;
        req->y = 14.0 + y;
        req->scaleX = 0.3359375f;
        req->scaleY = 0.21875f;
        req->rotation = 0;
    } else if (mode == 1 && !(g_some_other_flags & 0xA)) {
        req->x = 27.0 + x;
        req->y = 18.0 + y;
        req->scaleX = 0.421875f;
        req->scaleY = 0.28125f;
        if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.2)
            req->rotation += 0x2000;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.15)
            req->rotation += 0x1000;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.1)
            req->rotation += 0x800;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.06)
            req->rotation += 0x400;
        else if (mathutil_vec_len(&balls[modeCtrl.currPlayer].vel) > 0.02)
            req->rotation += 0x200;
        else if (req->rotation <= 0x100 || 0xFF00 <= req->rotation)
            req->rotation = 0;
        else if (req->rotation > 0x8000)
            req->rotation += 0x100;
        else if (req->rotation <= 0x8000)
            req->rotation -= 0x100;
        if (req->rotation > 0x10000)
            req->rotation -= 0x10000;
    }
    req->z = 0.06f;
    req->opacity = 1.0f;
    draw_sprite_draw_request(req);
    req = &lbl_11_data_4994;
    req->bmpId = anim->frames[prevFrame];
    req->z = 0.05f;
    req->opacity = 1.0 - (g_font_char_var_3 & 7) / 8.0;
    draw_sprite_draw_request(req);
}

/* Minigame per-frame 2D callback (registered by _prolog): HUD for the current shot. */
void fn_11_1E004(void)
{
    s8 savedLayer;

    set_ui_widescreen_scale_mtx(320);
    savedLayer = -1;
    switch (lbl_11_bss_2AC8) {
    case 1:
        if (lbl_11_bss_2AC0 == 0)
            lbl_11_bss_2AC0 = g_font_char_var_3;
        if (fn_11_EC54() != -1) {
            savedLayer = fn_11_EC30();
            fn_11_EC64(fn_11_EC54());
        }
        fn_11_2DBC0();
        fn_11_252A8();
        if (savedLayer != -1)
            fn_11_EC64(savedLayer);
        if (fn_11_E9A8() && fn_11_EF34() && modeCtrl.playerCount != 1)
            fn_11_1FD20();
        break;
    default:
        lbl_11_bss_2AC0 = 0;
        break;
    }
    reset_ui_widescreen_scale_mtx();
}

/* "nP START" banner that bounces in (timer counts down from 180), plus "USE CONTROLLER n". */
void fn_11_1E104(s32 timer)
{
    char buf[256];
    float scaleX;
    float scaleY;

    if (timer == 180) {
        lbl_11_bss_2ACC = 0;
        scaleX = 0.0f;
        scaleY = 0.0f;
    } else if (timer > 30.0) {
        scaleY = 1.0 + math_sin(65535.0f * (lbl_11_bss_2ACC / 36.0f) - 32768.0f)
                     / (1.0f + ((float)lbl_11_bss_2ACC * (float)lbl_11_bss_2ACC) / 25.0f);
        scaleX = scaleY;
        lbl_11_bss_2ACC++;
    } else {
        scaleX = 1.0f;
        scaleY = 1.0 / (float)(1.0 + (30.0 - timer) * (30.0 - timer));
    }
    textdraw_reset();
    textdraw_set_font(9);
    textdraw_set_scale(scaleX, scaleY);
    textdraw_set_pos(320.0f - 72.0f * scaleX, 240.0f - 96.0f * scaleY);
    if (modeCtrl.playerCount != 1) {
        switch (modeCtrl.currPlayer) {
        case 0:
            textdraw_set_mul_color(0xFF0000);
            textdraw_print("1P");
            break;
        case 1:
            textdraw_set_mul_color(0x2C65FF);
            textdraw_print("2P");
            break;
        case 2:
            textdraw_set_mul_color(0xFBFF2C);
            textdraw_print("3P");
            break;
        case 3:
            textdraw_set_mul_color(0x2CFF57);
            textdraw_print("4P");
            break;
        }
    }
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_pos(320.0 - 2.5 * (72.0f * scaleX), 240.0f - 32.0f * scaleY);
    textdraw_print("START");
    if (modeCtrl.playerCount != 1) {
        textdraw_set_pos(320.0 - 7.0 * (0.3 * (72.0f * scaleX)), 240.0f + 96.0f * scaleY);
        textdraw_set_mul_color(0xDFAFFF);
        textdraw_set_scale(0.3 * scaleX, 0.3 * scaleY);
        sprintf(buf, "USE CONTROLLER %d", player_pad_map[modeCtrl.currPlayer] + 1);
        textdraw_print(buf);
    }
}

/* Speech-bubble style label: left cap, right cap and n middle tiles of 24px, then the text. */
void fn_11_1E4C0(s32 x, s32 y, s32 n, char *text)
{
    struct SpriteDrawRequest req;
    s32 i;

    req = lbl_11_data_47E4;
    req.x = x - n * 12 - 16;
    req.y = y;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_4884;
    req.x = x + n * 12;
    req.y = y;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_4834;
    req.x = x - n * 12;
    req.y = y;
    for (i = 0; i < n; i++) {
        draw_sprite_draw_request(&req);
        req.x += 24.0f;
    }
    textdraw_reset();
    if (locale == 5) {
        textdraw_set_font(0x8E);
        textdraw_set_pos(x - n * 12 - 2, y + 9);
    } else if (locale == 0) {
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        textdraw_set_scale(0.7f, 1.0f);
        textdraw_set_pos(x - n * 12 - 6, y + 9);
    }
    textdraw_set_mul_color(0);
    textdraw_set_depth(0.11f);
    textdraw_print(text);
}

/* Player name plate that slides in (timer counts down from 180): face, name and score vs par. */
void fn_11_1E794(u32 timer)
{
    char buf[256];
    struct SpriteDrawRequest plate;
    struct SpriteDrawRequest face;
    s32 hole;
    s32 score;
    u16 *faceBmp;

    plate = lbl_11_data_1904;
    if (timer > 150.0)
        plate.x = -680.0 + 680.0f * (float)(180 - timer) * (float)(180 - timer) / 30.0 / 30.0;
    else if (!(timer > 90.0)) {
        if (timer > 60)
            plate.x = 640.0f * (float)(90.0 - timer) * (90.0 - timer) / 30.0 / 30.0;
        else
            plate.x = 640.0f;
    }
    draw_sprite_draw_request(&plate);
    set_ui_widescreen_scale_mtx(320);
    face = lbl_11_data_18B4;
    if (active_monkey_id[modeCtrl.currPlayer] == 0)
        faceBmp = lbl_11_data_48D4;
    else if (active_monkey_id[modeCtrl.currPlayer] == 1)
        faceBmp = lbl_11_data_48DC;
    else if (active_monkey_id[modeCtrl.currPlayer] == 2)
        faceBmp = lbl_11_data_48E4;
    else if (active_monkey_id[modeCtrl.currPlayer] == 3)
        faceBmp = lbl_11_data_48EC;
    face.bmpId = faceBmp[0];
    face.x = 130.0 + plate.x;
    face.y = 3.0 + plate.y;
    face.z = 0.11f;
    face.scaleX = 0.46875f;
    face.scaleY = 0.3125f;
    draw_sprite_draw_request(&face);
    textdraw_reset();
    textdraw_set_font(0x8F);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_flags(0x200000);
    textdraw_set_depth(0.11f);
    textdraw_set_pos(200.0f + plate.x, 13.0f + plate.y);
    switch (active_monkey_id[modeCtrl.currPlayer]) {
    case 0:
        textdraw_print("AIAI");
        break;
    case 1:
        textdraw_print("MEEMEE");
        break;
    case 2:
        textdraw_print("BABY");
        break;
    case 3:
        textdraw_print("GONGON");
        break;
    }
    score = 0;
    if (lbl_1_bss_1704.unk1 == 0 || lbl_1_bss_1704.unk1 == 1) {
        for (hole = 0; hole < fn_11_EC30(); hole++)
            score += fn_11_EC00(modeCtrl.currPlayer, hole) - fn_11_16FC4(hole);
    } else {
        for (hole = 9; hole < fn_11_EC30(); hole++)
            score += fn_11_EC00(modeCtrl.currPlayer, hole) - fn_11_16FC4(hole);
    }
    textdraw_set_pos(400.0f + plate.x, 13.0f + plate.y);
    if (locale == 0) {
        if (score > 0)
            sprintf(buf, "a/Score +%d\0\0\0", score);  /* the original literal is 16 bytes */
        else
            sprintf(buf, "a/Score %d", score);
    } else if (locale == 5) {
        if (score > 0)
            sprintf(buf, "\x83\x58\x83\x52\x83\x41 +%d", score);
        else
            sprintf(buf, "\x83\x58\x83\x52\x83\x41 %d", score);
    }
    textdraw_print(buf);
    reset_ui_widescreen_scale_mtx();
}

/* Match play: "1P/2P WINS" replaces the hole result once the winner is decided. */
#define GOLF4_MATCH_WINS_BANNER(n, x0, x1, word)                \
    if (fn_11_EC98() == (n) && lbl_1_bss_1704.unk0 == 1) {      \
        textdraw_set_pos(x0, 80.0f);                            \
        switch (modeCtrl.currPlayer) {                          \
        case 0:                                                 \
            textdraw_set_mul_color(0xFF0000);                   \
            textdraw_print("1P");                               \
            break;                                              \
        case 1:                                                 \
            textdraw_set_mul_color(0x2C65FF);                   \
            textdraw_print("2P");                               \
            break;                                              \
        }                                                       \
        textdraw_set_pos(x1, 80.0f);                            \
        textdraw_print(word);                                   \
        return;                                                 \
    }

/* Hole result caption ("PAR", "BIRDIE", "HOLE IN ONE", "CUP IN n", ...), bouncing in.
 * result is strokes relative to par (-10 = hole in one, 50 = give up, 99 = OB). */
void fn_11_1EC9C(s16 result, u32 timer)
{
    char buf[256];
    float pos;
    float v;

    textdraw_reset();
    textdraw_set_font(9);
    if (result == -10) {
        GOLF4_MATCH_WINS_BANNER(1, 115.0f, 288.0f, "WINS")
        textdraw_set_pos(104.0f, 48.0f);
        textdraw_print("HOLE");
        textdraw_set_pos(428.0f, 48.0f);
        textdraw_print("IN");
        textdraw_set_pos(212.0f, 112.0f);
        textdraw_print("ONE");
    } else if (result == -3) {
        GOLF4_MATCH_WINS_BANNER(1, 115.0f, 288.0f, "WINS")
        textdraw_set_pos(60.0f, 88.0f);
        textdraw_set_scale(0.8f, 0.8f);
        textdraw_print("ALBATROSS");
        textdraw_set_scale(1.0f, 1.0f);
    } else if (result == -2) {
        GOLF4_MATCH_WINS_BANNER(1, 115.0f, 288.0f, "WINS")
        textdraw_set_pos(142.0f, 80.0f);
        textdraw_print("EAGLE");
    } else if (result == -1) {
        GOLF4_MATCH_WINS_BANNER(1, 115.0f, 288.0f, "WINS")
        textdraw_set_pos(155.0f, 80.0f);
        textdraw_print("BIRDIE");
    } else if (result == 0) {
        GOLF4_MATCH_WINS_BANNER(1, 115.0f, 288.0f, "WINS")
        if (timer > 120) {
            v = 212.0 + (320.0f * math_sin(65535.0f * ((float)(240 - timer) / 36.0f) - 32768.0f))
                                  / (1.0f + ((float)(240 - timer) * (float)(240 - timer)) / 10.0f);
            if (212.0 > v)
                pos = v;
            else
                pos = 424.0 - v;
            textdraw_set_pos(pos, 80.0f);
            textdraw_print("PAR");
        } else {
            textdraw_set_pos(212.0f, 80.0f);
            textdraw_print("PAR");
        }
    } else if (result == 1) {
        GOLF4_MATCH_WINS_BANNER(1, 115.0f, 288.0f, "WINS")
        if (timer > 120) {
            v = 80.0f - (200.0f * math_sin(65535.0f * ((float)(240 - timer) / 36.0f) - 32768.0f))
                                  / (1.0f + ((float)(240 - timer) * (float)(240 - timer)) / 10.0f);
            if (80.0f > v)
                pos = v;
            else
                pos = 160.0f - v;
            textdraw_set_pos(140.0f, pos);
            textdraw_print("BOGEY");
        } else {
            textdraw_set_pos(140.0f, 80.0f);
            textdraw_print("BOGEY");
        }
    } else if (result == 2) {
        GOLF4_MATCH_WINS_BANNER(1, 115.0f, 288.0f, "WINS")
        textdraw_set_pos(320.0f, 88.0f);
        textdraw_set_alignment(4);
        textdraw_set_scale(0.7f, 0.7f);
        textdraw_print("DOUBLE BOGEY");
        textdraw_set_scale(1.0f, 1.0f);
    } else if (result == 50) {
        GOLF4_MATCH_WINS_BANNER(2, 32.0f, 248.0f, "LOSES")
        if (timer > 180) {
            v = 80.0f - (200.0f * math_sin(65535.0f * ((float)(300 - timer) / 36.0f) - 32768.0f))
                                  / (1.0f + ((float)(300 - timer) * (float)(300 - timer)) / 10.0f);
            if (80.0f > v)
                pos = v;
            else
                pos = 160.0f - v;
            textdraw_set_pos(111.2f, pos);
            textdraw_print("GIVE UP");
        } else {
            textdraw_set_pos(111.2f, 80.0f);
            textdraw_print("GIVE UP");
        }
    } else if (result == 99) {
        textdraw_set_pos(248.0f, 208.0f);
        textdraw_print("OB");
    } else {
        GOLF4_MATCH_WINS_BANNER(1, 115.0f, 288.0f, "WINS")
        if (timer > 120) {
            v = 80.0f - (200.0f * math_sin(65535.0f * ((float)(240 - timer) / 36.0f) - 32768.0f))
                              / (1.0f + ((float)(240 - timer) * (float)(240 - timer)) / 10.0f);
            if (80.0f > v)
                v = v;
            else
                v = 160.0f - v;
            textdraw_set_pos(104.0f, v);
            textdraw_print("CUP IN ");
            textdraw_set_pos(464.0f, v);
            sprintf(buf, "%d", result + fn_11_16FA4());
            textdraw_print(buf);
        } else {
            textdraw_set_pos(104.0f, 80.0f);
            textdraw_print("CUP IN ");
            textdraw_set_pos(464.0f, 80.0f);
            sprintf(buf, "%d", result + fn_11_16FA4());
            textdraw_print(buf);
        }
    }
}

/* Stroke counter box: frame, shadow and the number (one or two digits, n < 20). */
void fn_11_1F7FC(s32 n)
{
    struct SpriteDrawRequest req;

    req = lbl_11_data_4334;
    req.rotation = -0x4000;
    req.x = 150.0f + (lbl_11_data_4384.x - 2.0f);
    req.y = lbl_11_data_4384.y;
    req.z += 0.01;
    draw_sprite_draw_request(&req);
    draw_sprite_draw_request(&lbl_11_data_4334);
    draw_sprite_draw_request(&lbl_11_data_4384);
    if (n < 10) {
        req = lbl_11_data_4474[n];
        req.x += 8.0f;
        req.y += 4.0f;
        draw_sprite_draw_request(&req);
        req = lbl_11_data_4424;
        req.x += 26.0f;
        req.y += 4.0f;
        draw_sprite_draw_request(&req);
    } else {
        req = lbl_11_data_4474[1];
        req.x += 5.0f;
        req.y += 4.0f;
        req.z += 0.005;
        draw_sprite_draw_request(&req);
        req = lbl_11_data_4474[n - 10];
        req.x += 14.0f;
        req.y += 4.0f;
        draw_sprite_draw_request(&req);
        req = lbl_11_data_4424;
        req.x += 29.0f;
        req.y += 4.0f;
        draw_sprite_draw_request(&req);
    }
}

/* Hole number box (current hole n, colour by hole) and the par digit, each with a drop shadow. */
void fn_11_1FA7C(s32 n)
{
    struct SpriteDrawRequest req;

    draw_sprite_draw_request(&lbl_11_data_3F24);
    draw_sprite_draw_request(&lbl_11_data_3F74);
    draw_sprite_draw_request(&lbl_11_data_3FC4);
    if (n > 9)
        n = 9;
    req = lbl_11_data_4014[n];
    req.mulColor = lbl_11_data_4A44[n - 1];
    req.x += 2.0f;
    req.y += 2.0f;
    req.z += 0.005;
    req.mulColor = 0;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_4014[n];
    req.mulColor = lbl_11_data_4A44[n - 1];
    draw_sprite_draw_request(&req);
    req = lbl_11_data_4014[fn_11_16FA4()];
    req.x += 59.0f;
    req.y += 12.0f;
    req.scaleX *= 0.75;
    req.scaleY *= 0.75;
    req.mulColor = 0xFB5302;
    req.x += 2.0f;
    req.y += 2.0f;
    req.z += 0.005;
    req.mulColor = 0;
    draw_sprite_draw_request(&req);
    req = lbl_11_data_4014[fn_11_16FA4()];
    req.x += 59.0f;
    req.y += 12.0f;
    req.scaleX *= 0.75;
    req.scaleY *= 0.75;
    req.mulColor = 0xFB5302;
    draw_sprite_draw_request(&req);
}

/* Scale a sprite by a 1 s triangle pulse (1.0 .. 1.25). */
#define GOLF4_PULSE(req)                                                                  \
    if (g_font_char_var_3 % 60 < 30) {                                                    \
        (req).scaleX *= 1.0 + (float)(30 - g_font_char_var_3 % 60) / 120.0;               \
        (req).scaleY *= 1.0 + (float)(30 - g_font_char_var_3 % 60) / 120.0;               \
    } else {                                                                              \
        (req).scaleX *= 1.0 + (float)(g_font_char_var_3 % 60 - 30) / 120.0;               \
        (req).scaleY *= 1.0 + (float)(g_font_char_var_3 % 60 - 30) / 120.0;               \
    }

/* Game-over standings badges next to each player's panel: WIN/LOSE/DRAW in match play,
 * 1st-4th place (ties share a place) in stroke play. */
void fn_11_1FD20(void)
{
    struct SpriteDrawRequest req;
    u16 totals[4];
    u8 rank[4];
    u8 order[4];
    s32 i;
    s32 j;

    if (lbl_1_bss_1704.unk0 == 1) {
        if (fn_11_EC1C(0) == fn_11_EC1C(1)) {
            req = lbl_11_data_3E34;
            req.x = 102.4 + lbl_11_bss_2AD0[0].x;
            req.y = 15.0 + lbl_11_bss_2AD0[0].y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_3E34;
            req.x = 102.4 + lbl_11_bss_2AD0[1].x;
            req.y = 15.0 + lbl_11_bss_2AD0[1].y;
            draw_sprite_draw_request(&req);
        } else if (fn_11_EC1C(0) > fn_11_EC1C(1)) {
            req = lbl_11_data_3E84;
            req.x = 102.4 + lbl_11_bss_2AD0[0].x;
            req.y = 15.0 + lbl_11_bss_2AD0[0].y;
            GOLF4_PULSE(req)
            draw_sprite_draw_request(&req);
            req = lbl_11_data_3ED4;
            req.x = 102.4 + lbl_11_bss_2AD0[1].x;
            req.y = 15.0 + lbl_11_bss_2AD0[1].y;
            draw_sprite_draw_request(&req);
        } else if (fn_11_EC1C(0) < fn_11_EC1C(1)) {
            req = lbl_11_data_3ED4;
            req.x = 102.4 + lbl_11_bss_2AD0[0].x;
            req.y = 15.0 + lbl_11_bss_2AD0[0].y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_3E84;
            req.x = 102.4 + lbl_11_bss_2AD0[1].x;
            req.y = 15.0 + lbl_11_bss_2AD0[1].y;
            GOLF4_PULSE(req)
            draw_sprite_draw_request(&req);
        }
    } else if (lbl_1_bss_1704.unk0 == 0) {
        s32 place;
        s32 same;

        for (i = 0; i < modeCtrl.playerCount; i++) {
            totals[i] = 0;
            order[i] = i;
            for (j = 0; j < 18; j++)
                totals[i] += fn_11_EC00(i, j);
        }
        for (i = 0; i < modeCtrl.playerCount; i++) {
            for (j = i; j < modeCtrl.playerCount; j++) {
                if (totals[i] > totals[j]) {
                    u16 t = totals[i];
                    totals[i] = totals[j];
                    totals[j] = t;
                    t = order[i];
                    order[i] = order[j];
                    order[j] = t;
                }
            }
        }
        place = 0;
        same = 1;
        for (i = 0; i < modeCtrl.playerCount; i++) {
            rank[order[i]] = place;
            if (totals[i] != totals[i + 1]) {
                place += same;
                same = 1;
            } else {
                same++;
            }
        }
        for (i = 0; i < modeCtrl.playerCount; i++) {
            if (rank[i] == 0) {
                req = lbl_11_data_3CF4;
                GOLF4_PULSE(req)
            } else if (rank[i] == 1) {
                req = lbl_11_data_3D44;
            } else if (rank[i] == 2) {
                req = lbl_11_data_3D94;
            } else if (rank[i] == 3) {
                req = lbl_11_data_3DE4;
            }
            req.x = 80.0 + lbl_11_bss_2AD0[i].x;
            req.y = 15.0 + lbl_11_bss_2AD0[i].y;
            draw_sprite_draw_request(&req);
        }
    }
}

/* Stand-in for a function the linker stripped between fn_11_1FD20 and fn_11_20858: its
 * literals (100.0, 10.0, 22.0f, 8.0, 100.0f) stay at .rodata 0x2D8-0x2FC ahead of
 * fn_11_20858's. Unreferenced, so it is stripped again. */
void golf_stripped_20858_literals(double *d, float *f)
{
    d[0] = 100.0;
    d[1] = 10.0;
    f[0] = 22.0f;
    d[2] = 8.0;
    f[1] = 100.0f;
}
