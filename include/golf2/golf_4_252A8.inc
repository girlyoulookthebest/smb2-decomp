/* golf_4.c functions 0x252A8-0x2DBC0 (fn_11_252A8: scorecard draw); see golf_4.c */

/* golf_4_1D260.inc: character portrait at (x, y). The calls here evaluate x and y before chara,
 * so the parameter order is (x, y, chara, mode) (same registers as (chara, mode, x, y)). */

/*
 * Scorecard sprite templates in golf_4.c .data (defined by the TU data block):
 * frame pieces, header/row cells, two 10-glyph digit sets (2944, 2C64),
 * per-player name tags/icons (1454, 3254, 3394, 34D4), win/draw/lose marks.
 * lbl_11_bss_2AD0[player] receives the position of the player's total cell.
 */

/* Pulsing highlight level 0..230 from the global frame counter */
#define GOLF_BLINK(v) \
    v = g_font_char_var_3 * 8; \
    v &= 0x1FF; \
    v = v - 0xFF; \
    if (v < 0) { \
        v *= -1; \
    } \
    v = (230.0f * v) / 255.0f

/* Orange add-color for the blinking current-hole digits */
#define GOLF_BLINK_RGB(v) ((v / 4) + ((v / 4) << 8) + (v << 16))

/*
 * Draws the Monkey Golf scorecard: per-hole par row, each player's strokes per
 * hole with OUT/IN/total columns (stroke play), or the hole-by-hole win/lose
 * marks and wins-needed track (match play, lbl_1_bss_1704.unk0 == 1).
 * lbl_1_bss_1704.unk1 selects the course: 0 = 18 holes, 1 = front 9, 2 = back 9.
 */
void fn_11_252A8(void)
{
    char buf[256];
    char buf2[256];
    struct SpriteDrawRequest req;
    struct SpriteDrawRequest req2;
    struct SpriteDrawRequest req3;
    struct SpriteDrawRequest req4;
    struct SpriteDrawRequest req5;
    struct SpriteDrawRequest req6;
    struct SpriteDrawRequest req7;
    struct SpriteDrawRequest req8;
    struct SpriteDrawRequest req9;
    float baseX;
    float x;
    float baseY;
    float y;
    int i;
    int player;
    int parSum;
    int total;
    int diff;
    int total2;
    int diff2;
    int wins;
    int strokes;

#ifndef __MWERKS__
    strokes = 0;
#endif
    if (lbl_1_bss_1704.unk1 == 0) {
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_1E04;
        } else if (lbl_1_bss_1704.unk0 == 1) {
            req = lbl_11_data_1EF4;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            if (modeCtrl.playerCount == 1) {
                req.y += 62.0f;
            } else if (modeCtrl.playerCount == 2) {
                req.y += 40.0f;
            } else if (modeCtrl.playerCount == 3) {
                req.y += 18.0f;
            } else if (modeCtrl.playerCount == 4) {
                req.y += -2.0f;
            }
        }
        draw_sprite_draw_request(&req);
        baseX = req.x;
        baseY = req.y;
        x = 98.0 + baseX;
        y = baseY;
        for (i = 0; i < 9; i++) {
            if ((i % 2) == 0) {
                req = lbl_11_data_1F94;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req2 = lbl_11_data_2174;
                req2.x += x;
                req2.y = 32.0f + (req2.y + y);
                req2.z -= 0.005;
                draw_sprite_draw_request(&req2);
                req3 = lbl_11_data_2944[fn_11_16FC4(i)];
                req3.x = 8.0f + req2.x;
                req3.y = 4.0f + req2.y;
                req3.mulColor = 0xFFDF0029;
                draw_sprite_draw_request(&req3);
            } else {
                req = lbl_11_data_1FE4;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req2 = lbl_11_data_21C4;
                req2.x += x;
                req2.y = 32.0f + (req2.y + y);
                req2.z -= 0.005;
                draw_sprite_draw_request(&req2);
                req3 = lbl_11_data_2944[fn_11_16FC4(i)];
                req3.x = 8.0f + req2.x;
                req3.y = 4.0f + req2.y;
                req3.mulColor = 0xFFDF0029;
                draw_sprite_draw_request(&req3);
            }
            req2 = lbl_11_data_2944[i + 1];
            req2.x = 8.0f + req.x;
            req2.y = 6.0f + req.y;
            draw_sprite_draw_request(&req2);
            x += 30.0;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_2034;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_3024;
            req2.x = 4.0f + req.x;
            req2.y = 6.0f + req.y;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_2214;
            req2.x = req.x;
            req2.y = 32.0f + req.y;
            req2.z -= 0.005;
            draw_sprite_draw_request(&req2);
            parSum = 0;
            for (i = 0; i < 9; i++) {
                parSum += fn_11_16FC4(i);
            }
            req2 = lbl_11_data_2944[(parSum / 10)];
            req2.x = 8.0f + (8.0f + req.x);
            req2.y = 4.0f + (32.0f + req.y);
            req2.mulColor = 0xFFDF0029;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_2944[(parSum % 10)];
            req2.x = 8.0f + (20.0f + (8.0f + req.x));
            req2.y = 4.0f + (32.0f + req.y);
            req2.mulColor = 0xFFDF0029;
            draw_sprite_draw_request(&req2);
            x += 66.0;
        }
        req = lbl_11_data_20D4;
        req.x += x;
        req.y += y;
        draw_sprite_draw_request(&req);
        for (player = 0; player < modeCtrl.playerCount; player++) {
            x = baseX;
            y = 66.0 + baseY + 30.0 * player;
            req = lbl_11_data_2124;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_3254[player];
            req2.x = x;
            req2.y = y;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_1454[player];
            req2.x = 12.0f + x;
            req2.y = 9.0f + y;
            req2.z = 0.06f;
            draw_sprite_draw_request(&req2);
            if (fn_11_EFC4(player) == 0) {
                fn_11_1D260(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            } else if (fn_11_EFC4(player) == 1) {
                fn_11_1DB90(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            } else if (fn_11_EFC4(player) == 2) {
                fn_11_1D71C(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            }
            x += 98.0;
            for (i = 0; i < 9; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_2174;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_21C4;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (lbl_1_bss_1704.unk0 == 0) {
                    if (i < fn_11_EC30()) {
                        if (fn_11_EC00(player, i) < 10) {
                            req2 = lbl_11_data_2944[fn_11_EC00(player, i)];
                            req2.x = 8.0f + x;
                            req2.y = 4.0f + y;
                            draw_sprite_draw_request(&req2);
                        } else {
                            req2 = lbl_11_data_2C64[1];
                            req2.x = 3.0f + x;
                            req2.y = 4.0f + y;
                            draw_sprite_draw_request(&req2);
                            req2 = lbl_11_data_2C64[0];
                            req2.x = 16.0f + x;
                            req2.y = 4.0f + y;
                            draw_sprite_draw_request(&req2);
                        }
                    }
                    if ((fn_11_EEFC() != 0) && (i == fn_11_EC30())) {
                        if (fn_11_EC00(player, i) < 10) {
                            req2 = lbl_11_data_2944[fn_11_EC00(player, i)];
                            req2.x = 8.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                        } else {
                            req2 = lbl_11_data_2C64[1];
                            req2.x = 3.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                            req2 = lbl_11_data_2C64[0];
                            req2.x = 16.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                        }
                    }
                } else if (lbl_1_bss_1704.unk0 == 1) {
                    if (i < fn_11_EC30()) {
                        if (player == 0) {
                                                        if (fn_11_EC00(0, i) < fn_11_EC00(1, i)) {
                                req2 = lbl_11_data_3164;
                            } else {
                                                                if (fn_11_EC00(0, i) == fn_11_EC00(1, i)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i) > fn_11_EC00(1, i)) {
                                        req2 = lbl_11_data_3204;
                                    }
                                }
                            }
                        } else if (player == 1) {
                                                        if (fn_11_EC00(0, i) < fn_11_EC00(1, i)) {
                                req2 = lbl_11_data_3204;
                            } else {
                                                                if (fn_11_EC00(0, i) == fn_11_EC00(1, i)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i) > fn_11_EC00(1, i)) {
                                        req2 = lbl_11_data_3164;
                                    }
                                }
                            }
                        }
                        req2.x = 4.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                    }
                    if ((fn_11_EEFC() != 0) && (i == fn_11_EC30())) {
                        if (player == 0) {
                                                        if (fn_11_EC00(0, i) < fn_11_EC00(1, i)) {
                                req2 = lbl_11_data_3164;
                            } else {
                                                                if (fn_11_EC00(0, i) == fn_11_EC00(1, i)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i) > fn_11_EC00(1, i)) {
                                        req2 = lbl_11_data_3204;
                                    }
                                }
                            }
                        } else if (player == 1) {
                                                        if (fn_11_EC00(0, i) < fn_11_EC00(1, i)) {
                                req2 = lbl_11_data_3204;
                            } else {
                                                                if (fn_11_EC00(0, i) == fn_11_EC00(1, i)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i) > fn_11_EC00(1, i)) {
                                        req2 = lbl_11_data_3164;
                                    }
                                }
                            }
                        }
                        req2.x = 4.0f + x;
                        req2.y = 4.0f + y;
                        {
                            s16 blink;

                            GOLF_BLINK(blink);
                            req2.addColor = GOLF_BLINK_RGB(blink);
                        }
                        draw_sprite_draw_request(&req2);
                    }
                }
                x += 30.0;
            }
            if (lbl_1_bss_1704.unk0 == 0) {
                req = lbl_11_data_2214;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req = lbl_11_data_3394[player];
                req.x = x;
                req.y = y;
                draw_sprite_draw_request(&req);
                total = 0;
                for (i = 0; i < 9; i++) {
                    total += fn_11_EC00(player, i);
                }
                if (total < 10) {
                    req2 = lbl_11_data_2944[total];
                    req2.x = 36.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                } else {
                    req2 = lbl_11_data_2944[(total / 10)];
                    req2.x = 16.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[(total % 10)];
                    req2.x = 36.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                }
                x += 66.0;
            }
            req = lbl_11_data_22B4;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
        }
        x = baseX;
        y = 66.0 + baseY + 30.0 * modeCtrl.playerCount;
        req = lbl_11_data_2304;
        req.x = x;
        req.y = y;
        draw_sprite_draw_request(&req);
        x += 98.0;
        for (i = 0; i < 9; i++) {
            if ((i % 2) == 0) {
                req = lbl_11_data_2354;
                req.x += x;
                req.y += y;
            } else {
                req = lbl_11_data_23A4;
                req.x += x;
                req.y += y;
            }
            draw_sprite_draw_request(&req);
            x += 30.0;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_23F4;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            x += 66.0;
        }
        req = lbl_11_data_2494;
        req.x += x;
        req.y += y;
        draw_sprite_draw_request(&req);
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_1E54;
        } else if (lbl_1_bss_1704.unk0 == 1) {
            req = lbl_11_data_1F44;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            if (modeCtrl.playerCount == 1) {
                req.y += 22.0f;
            } else if (modeCtrl.playerCount == 2) {
                req.y += 18.0f;
            } else if (modeCtrl.playerCount == 3) {
                req.y += 12.0f;
            } else if (modeCtrl.playerCount == 4) {
                req.y += 2.0f;
            }
        }
        draw_sprite_draw_request(&req);
        baseX = req.x;
        baseY = req.y;
        x = 98.0 + baseX;
        y = baseY;
        for (i = 0; i < 9; i++) {
            if ((i % 2) == 0) {
                req = lbl_11_data_1F94;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req2 = lbl_11_data_2174;
                req2.x += x;
                req2.y = 32.0f + (req2.y + y);
                req2.z -= 0.005;
                draw_sprite_draw_request(&req2);
                req4 = lbl_11_data_2944[fn_11_16FC4(i + 9)];
                req4.x = 8.0f + req2.x;
                req4.y = 4.0f + req2.y;
                req4.mulColor = 0xFFDF0029;
                draw_sprite_draw_request(&req4);
            } else {
                req = lbl_11_data_1FE4;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req2 = lbl_11_data_21C4;
                req2.x += x;
                req2.y = 32.0f + (req2.y + y);
                req2.z -= 0.005;
                draw_sprite_draw_request(&req2);
                req4 = lbl_11_data_2944[fn_11_16FC4(i + 9)];
                req4.x = 8.0f + req2.x;
                req4.y = 4.0f + req2.y;
                req4.mulColor = 0xFFDF0029;
                draw_sprite_draw_request(&req4);
            }
            req2 = lbl_11_data_2C64[1];
            req2.x = 3.0f + req.x;
            req2.y = 6.0f + req.y;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_2C64[i];
            req2.x = 15.0f + req.x;
            req2.y = 6.0f + req.y;
            draw_sprite_draw_request(&req2);
            x += 30.0;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_2034;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_2FD4;
            req2.x = 20.0f + req.x;
            req2.y = 6.0f + req.y;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_2214;
            req2.x = req.x;
            req2.y = 32.0f + req.y;
            req2.z -= 0.005;
            draw_sprite_draw_request(&req2);
            parSum = 0;
            for (i = 9; i < 18; i++) {
                parSum += fn_11_16FC4(i);
            }
            req2 = lbl_11_data_2944[(parSum / 10)];
            req2.x = 8.0f + (8.0f + req.x);
            req2.y = 4.0f + (32.0f + req.y);
            req2.mulColor = 0xFFDF0029;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_2944[(parSum % 10)];
            req2.x = 8.0f + (20.0f + (8.0f + req.x));
            req2.y = 4.0f + (32.0f + req.y);
            req2.mulColor = 0xFFDF0029;
            draw_sprite_draw_request(&req2);
            x += 66.0;
        }
        req = lbl_11_data_2084;
        req.x += x;
        req.y += y;
        if (lbl_1_bss_1704.unk0 == 1) {
            req.scaleX = 0.3902439f;
        }
        draw_sprite_draw_request(&req);
        req2 = lbl_11_data_2F84;
        if (lbl_1_bss_1704.unk0 == 1) {
            req2.x = 20.0 + req.x;
        } else {
            req2.x = 9.0 + req.x;
        }
        req2.y = 6.0 + req.y;
        draw_sprite_draw_request(&req2);
        if (lbl_1_bss_1704.unk0 == 1) {
            x += 128.0;
        } else {
            x += 100.0;
        }
        req = lbl_11_data_20D4;
        req.x += x;
        req.y += y;
        draw_sprite_draw_request(&req);
        for (player = 0; player < modeCtrl.playerCount; player++) {
            x = baseX;
            y = 66.0 + baseY + 30.0 * player;
            req = lbl_11_data_2124;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_3254[player];
            req2.x = x;
            req2.y = y;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_1454[player];
            req2.x = 12.0f + x;
            req2.y = 9.0f + y;
            req2.z = 0.06f;
            draw_sprite_draw_request(&req2);
            if (fn_11_EFC4(player) == 0) {
                fn_11_1D260(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            } else if (fn_11_EFC4(player) == 1) {
                fn_11_1DB90(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            } else if (fn_11_EFC4(player) == 2) {
                fn_11_1D71C(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            }
            x += 98.0;
            for (i = 0; i < 9; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_2174;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_21C4;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (lbl_1_bss_1704.unk0 == 0) {
                    if ((i + 9) < fn_11_EC30()) {
                        if (fn_11_EC00(player, i + 9) < 10) {
                            req2 = lbl_11_data_2944[fn_11_EC00(player, i + 9)];
                            req2.x = 8.0f + x;
                            req2.y = 4.0f + y;
                            draw_sprite_draw_request(&req2);
                        } else {
                            req2 = lbl_11_data_2C64[1];
                            req2.x = 3.0f + x;
                            req2.y = 4.0f + y;
                            draw_sprite_draw_request(&req2);
                            req2 = lbl_11_data_2C64[0];
                            req2.x = 16.0f + x;
                            req2.y = 4.0f + y;
                            draw_sprite_draw_request(&req2);
                        }
                    }
                    if ((fn_11_EEFC() != 0) && ((i + 9) == fn_11_EC30())) {
                        if (fn_11_EC00(player, i + 9) < 10) {
                            req2 = lbl_11_data_2944[fn_11_EC00(player, i + 9)];
                            req2.x = 8.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                        } else {
                            req2 = lbl_11_data_2C64[1];
                            req2.x = 3.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                            req2 = lbl_11_data_2C64[0];
                            req2.x = 16.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                        }
                    }
                } else if (lbl_1_bss_1704.unk0 == 1) {
                    if ((i + 9) < fn_11_EC30()) {
                        if (player == 0) {
                                                        if (fn_11_EC00(0, i + 9) < fn_11_EC00(1, i + 9)) {
                                req2 = lbl_11_data_3164;
                            } else {
                                                                if (fn_11_EC00(0, i + 9) == fn_11_EC00(1, i + 9)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i + 9) > fn_11_EC00(1, i + 9)) {
                                        req2 = lbl_11_data_3204;
                                    }
                                }
                            }
                        } else if (player == 1) {
                                                        if (fn_11_EC00(0, i + 9) < fn_11_EC00(1, i + 9)) {
                                req2 = lbl_11_data_3204;
                            } else {
                                                                if (fn_11_EC00(0, i + 9) == fn_11_EC00(1, i + 9)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i + 9) > fn_11_EC00(1, i + 9)) {
                                        req2 = lbl_11_data_3164;
                                    }
                                }
                            }
                        }
                        req2.x = 4.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                    }
                    if ((fn_11_EEFC() != 0) && ((i + 9) == fn_11_EC30())) {
                        if (player == 0) {
                                                        if (fn_11_EC00(0, i + 9) < fn_11_EC00(1, i + 9)) {
                                req2 = lbl_11_data_3164;
                            } else {
                                                                if (fn_11_EC00(0, i + 9) == fn_11_EC00(1, i + 9)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i + 9) > fn_11_EC00(1, i + 9)) {
                                        req2 = lbl_11_data_3204;
                                    }
                                }
                            }
                        } else if (player == 1) {
                                                        if (fn_11_EC00(0, i + 9) < fn_11_EC00(1, i + 9)) {
                                req2 = lbl_11_data_3204;
                            } else {
                                                                if (fn_11_EC00(0, i + 9) == fn_11_EC00(1, i + 9)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i + 9) > fn_11_EC00(1, i + 9)) {
                                        req2 = lbl_11_data_3164;
                                    }
                                }
                            }
                        }
                        req2.x = 4.0f + x;
                        req2.y = 4.0f + y;
                        {
                            s16 blink;

                            GOLF_BLINK(blink);
                            req2.addColor = GOLF_BLINK_RGB(blink);
                        }
                        draw_sprite_draw_request(&req2);
                    }
                }
                x += 30.0;
            }
            if (lbl_1_bss_1704.unk0 == 0) {
                req = lbl_11_data_2214;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req = lbl_11_data_3394[player];
                req.x = x;
                req.y = y;
                draw_sprite_draw_request(&req);
                total = 0;
                for (i = 0; i < 9; i++) {
                    total += fn_11_EC00(player, i + 9);
                }
                if (total < 10) {
                    req2 = lbl_11_data_2944[total];
                    req2.x = 36.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                } else {
                    req2 = lbl_11_data_2944[(total / 10)];
                    req2.x = 16.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[(total % 10)];
                    req2.x = 36.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                }
                x += 66.0;
            }
            req = lbl_11_data_2264;
            req.x += x;
            req.y += y;
            lbl_11_bss_2AD0[player].x = req.x;
            lbl_11_bss_2AD0[player].y = req.y;
            if (lbl_1_bss_1704.unk0 == 1) {
                req.scaleX = 0.3902439f;
            }
            draw_sprite_draw_request(&req);
            req = lbl_11_data_34D4[player];
            req.x += x;
            req.y += y;
            if (lbl_1_bss_1704.unk0 == 1) {
                req.scaleX *= 1.28;
            }
            draw_sprite_draw_request(&req);
            if (lbl_1_bss_1704.unk0 == 1) {
                if (player == 0) {
                    diff = fn_11_EC1C(0) - fn_11_EC1C(1);
                } else {
                    diff = fn_11_EC1C(1) - fn_11_EC1C(0);
                }
                if (diff > 0) {
                    if (fabs((float) diff) >= 10.0) {
                        req2 = lbl_11_data_2944[(s32) (fabs((float) diff) / 10.0)];
                        req2.x = 10.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_2944[((s32) fabs((float) diff) % 10)];
                        req2.x = 30.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_3074;
                        req2.x = 50.0f + x;
                        req2.y = 9.0f + y;
                        draw_sprite_draw_request(&req2);
                    } else {
                        req2 = lbl_11_data_2944[(s32) fabs((float) diff)];
                        req2.x = 10.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_3074;
                        req2.x = 30.0f + x;
                        req2.y = 9.0f + y;
                        draw_sprite_draw_request(&req2);
                    }
                } else if (diff == 0) {
                    req2 = lbl_11_data_30C4;
                    req2.x = 10.0f + x;
                    req2.y = 9.0f + y;
                    draw_sprite_draw_request(&req2);
                } else if (diff < 0) {
                    if (fabs((float) diff) >= 10.0) {
                        req2 = lbl_11_data_2944[(s32) (fabs((float) diff) / 10.0)];
                        req2.x = 10.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_2944[((s32) fabs((float) diff) % 10)];
                        req2.x = 30.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_3114;
                        req2.x = 50.0f + x;
                        req2.y = 9.0f + y;
                        draw_sprite_draw_request(&req2);
                    } else {
                        req2 = lbl_11_data_2944[(s32) fabs((float) diff)];
                        req2.x = 10.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_3114;
                        req2.x = 30.0f + x;
                        req2.y = 9.0f + y;
                        draw_sprite_draw_request(&req2);
                    }
                }
            } else if (lbl_1_bss_1704.unk0 == 0) {
                total = 0;
                if (lbl_1_bss_1704.unk1 == 0) {
                for (i = 0; i < 18; i++) {
                    total += fn_11_EC00(player, i);
                }
                } else if (lbl_1_bss_1704.unk1 == 1) {
                for (i = 0; i < 9; i++) {
                    total += fn_11_EC00(player, i);
                }
                } else if (lbl_1_bss_1704.unk1 == 2) {
                for (i = 9; i < 18; i++) {
                    total += fn_11_EC00(player, i);
                }
                }
                if (total >= 100) {
                    req2 = lbl_11_data_2944[(total / 100)];
                    req2.x = 10.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[((total % 100) / 10)];
                    req2.x = 30.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[(total % 10)];
                    req2.x = 50.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                } else if (total >= 10) {
                    req2 = lbl_11_data_2944[((total % 100) / 10)];
                    req2.x = 10.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[(total % 10)];
                    req2.x = 30.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                } else {
                    req2 = lbl_11_data_2944[(total % 10)];
                    req2.x = 10.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                }
            }
            if (lbl_1_bss_1704.unk0 == 1) {
                x += 128.0;
            } else {
                x += 100.0;
            }
            req = lbl_11_data_22B4;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
        }
        x = baseX;
        y = 66.0 + baseY + 30.0 * modeCtrl.playerCount;
        req = lbl_11_data_2304;
        req.x = x;
        req.y = y;
        draw_sprite_draw_request(&req);
        x += 98.0;
        for (i = 0; i < 9; i++) {
            if ((i % 2) == 0) {
                req = lbl_11_data_2354;
                req.x += x;
                req.y += y;
            } else {
                req = lbl_11_data_23A4;
                req.x += x;
                req.y += y;
            }
            draw_sprite_draw_request(&req);
            x += 30.0;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_23F4;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            x += 66.0;
        }
        req = lbl_11_data_2444;
        req.x += x;
        req.y += y;
        if (lbl_1_bss_1704.unk0 == 1) {
            req.scaleX = 0.3902439f;
        }
        draw_sprite_draw_request(&req);
        if (lbl_1_bss_1704.unk0 == 1) {
            x += 128.0;
        } else {
            x += 100.0;
        }
        req = lbl_11_data_2494;
        req.x += x;
        req.y += y;
        draw_sprite_draw_request(&req);
        if (lbl_1_bss_1704.unk0 == 1) {
            if (lbl_1_bss_1704.unk1 == 0) {
                wins = 18;
            } else if (lbl_1_bss_1704.unk1 == 2) {
                wins = 18;
            } else {
                wins = 9;
            }
            if (fn_11_EEFC() != 0) {
                wins -= 1;
            }
            wins -= fn_11_EC30();
            {
                int won1 = fn_11_EC1C(1);
                int won0 = fn_11_EC1C(0);

                /* holes left plus holes won by both = holes counted; a majority wins */
                wins = won1 + wins;
                wins = won0 + wins;
            }
            wins = wins / 2 + 1;
            req = lbl_11_data_2534;
            draw_sprite_draw_request(&req);
            baseX = req.x;
            baseY = req.y;
            x = 98.0 + baseX;
            y = baseY;
            for (i = 0; i < 10; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_2584;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_25D4;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (i < 9) {
                    req = lbl_11_data_2944[i + 1];
                    req.x = 9.0f + (req.x + x);
                    req.y = 36.0f + (req.y + y);
                    req.mulColor = 0xFF69372E;
                    draw_sprite_draw_request(&req);
                } else {
                    req = lbl_11_data_2C64[1];
                    req.x = 3.0f + (req.x + x);
                    req.y = 36.0f + (req.y + y);
                    req.mulColor = 0xFF69372E;
                    draw_sprite_draw_request(&req);
                    req = lbl_11_data_2C64[0];
                    req.x = 16.0f + (req.x + x);
                    req.y = 36.0f + (req.y + y);
                    req.mulColor = 0xFF69372E;
                    draw_sprite_draw_request(&req);
                }
                x += 30.0;
            }
            req = lbl_11_data_2624;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_2674;
            x = baseX;
            y = 66.0 + baseY;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_3254[0];
            x = baseX;
            y = 66.0 + baseY;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_1454[0];
            req2.x = 12.0f + x;
            req2.y = 9.0f + y;
            req2.z = 0.06f;
            draw_sprite_draw_request(&req2);
            if (fn_11_EFC4(0) == 0) {
                fn_11_1D260(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[0], 0);
            } else if (fn_11_EFC4(0) == 1) {
                fn_11_1DB90(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[0], 0);
            } else if (fn_11_EFC4(0) == 2) {
                fn_11_1D71C(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[0], 0);
            }
            x += 98.0;
            for (i = 0; i < 10; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_26C4;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_2714;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (wins < (i + 1)) {
                    req5 = lbl_11_data_28F4;
                    req5.x = req.x;
                    req5.y = req.y;
                    draw_sprite_draw_request(&req5);
                    req5.y -= 32.0;
                    req5.z -= 0.02;
                    draw_sprite_draw_request(&req5);
                }
                if (fn_11_EC1C(0) > (i + 1)) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    draw_sprite_draw_request(&req);
                }
                if (((i + 1) == fn_11_EC1C(0)) && (fn_11_EEFC() != 0) && (fn_11_EC00(0, fn_11_EC30()) < fn_11_EC00(1, fn_11_EC30()))) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    {
                        s16 blink;

                        GOLF_BLINK(blink);
                        req.addColor = blink + (blink << 8) + (blink << 16);
                    }
                    draw_sprite_draw_request(&req);
                } else if ((i + 1) == fn_11_EC1C(0)) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    draw_sprite_draw_request(&req);
                }
                x += 30.0;
            }
            req = lbl_11_data_2764;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_2674;
            x = baseX;
            y = 96.0 + baseY;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_3254[1];
            x = baseX;
            y = 96.0 + baseY;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_1454[1];
            req2.x = 12.0f + x;
            req2.y = 9.0f + y;
            req2.z = 0.06f;
            draw_sprite_draw_request(&req2);
            if (fn_11_EFC4(1) == 0) {
                fn_11_1D260(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[1], 0);
            } else if (fn_11_EFC4(1) == 1) {
                fn_11_1DB90(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[1], 0);
            } else if (fn_11_EFC4(1) == 2) {
                fn_11_1D71C(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[1], 0);
            }
            x += 98.0;
            for (i = 0; i < 10; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_26C4;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_2714;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (wins < (i + 1)) {
                    req6 = lbl_11_data_28F4;
                    req6.x = req.x;
                    req6.y = req.y;
                    draw_sprite_draw_request(&req6);
                }
                if (fn_11_EC1C(1) > (i + 1)) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    draw_sprite_draw_request(&req);
                }
                if (((i + 1) == fn_11_EC1C(1)) && (fn_11_EEFC() != 0) && (fn_11_EC00(0, fn_11_EC30()) > fn_11_EC00(1, fn_11_EC30()))) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    {
                        s16 blink;

                        GOLF_BLINK(blink);
                        req.addColor = blink + (blink << 8) + (blink << 16);
                    }
                    draw_sprite_draw_request(&req);
                } else if ((i + 1) == fn_11_EC1C(1)) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    draw_sprite_draw_request(&req);
                }
                x += 30.0;
            }
            req = lbl_11_data_2764;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_27B4;
            x = baseX;
            y = 126.0 + baseY;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            x += 98.0;
            for (i = 0; i < 10; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_2804;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_2854;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                x += 30.0;
            }
            req = lbl_11_data_28A4;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            if ((fn_11_E9A8() == 0) || (fn_11_EF34() == 0)) {
                textdraw_reset();
                if (locale == 5) {
                    textdraw_set_font(0x8F);
                    textdraw_set_depth(0.08f);
                    textdraw_set_mul_color(0xFF69372E);
                    textdraw_set_pos(72.0f, 345.0f);
                    sprintf(buf, "\x82\xB3\x82\xAB\x82\xC9%d\x82\xB5\x82\xE5\x82\xA4\x82\xB5\x82\xBD\x82\xD9\x82\xA4\x82\xCC\x82\xA9\x82\xBF\x82\xC5\x82\xB7", wins);
                } else if (locale == 0) {
                    textdraw_set_font(3);
                    textdraw_set_flags(0x80000000);
                    textdraw_set_depth(0.08f);
                    textdraw_set_mul_color(0xFF69372E);
                    textdraw_set_pos(78.0f, 348.0f);
                    sprintf(buf, "The first person to get %d wins will win the game.", wins);
                    textdraw_set_scale(0.5f, 0.7f);
                }
                textdraw_print(buf);
            }
        }
    } else {
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_1DB4;
        } else if (lbl_1_bss_1704.unk0 == 1) {
            req = lbl_11_data_1EA4;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            if (modeCtrl.playerCount == 1) {
                req.y += 45.0f;
            } else if (modeCtrl.playerCount == 2) {
                req.y += 30.0f;
            } else if (modeCtrl.playerCount == 3) {
                req.y += 15.0f;
            }
        }
        draw_sprite_draw_request(&req);
        baseX = req.x;
        baseY = req.y;
        x = 98.0 + baseX;
        y = baseY;
        for (i = 0; i < 9; i++) {
            if ((i % 2) == 0) {
                req = lbl_11_data_1F94;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req2 = lbl_11_data_2174;
                req2.x += x;
                req2.y = 32.0f + (req2.y + y);
                req2.z -= 0.005;
                draw_sprite_draw_request(&req2);
                if (lbl_1_bss_1704.unk1 == 1) {
                    req7 = lbl_11_data_2944[fn_11_16FC4(i)];
                } else {
                    req7 = lbl_11_data_2944[fn_11_16FC4(i + 9)];
                }
                req7.x = 8.0f + req2.x;
                req7.y = 4.0f + req2.y;
                req7.mulColor = 0xFFDF0029;
                draw_sprite_draw_request(&req7);
            } else {
                req = lbl_11_data_1FE4;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req2 = lbl_11_data_21C4;
                req2.x += x;
                req2.y = 32.0f + (req2.y + y);
                req2.z -= 0.005;
                draw_sprite_draw_request(&req2);
                if (lbl_1_bss_1704.unk1 == 1) {
                    req7 = lbl_11_data_2944[fn_11_16FC4(i)];
                } else {
                    req7 = lbl_11_data_2944[fn_11_16FC4(i + 9)];
                }
                req7.x = 8.0f + req2.x;
                req7.y = 4.0f + req2.y;
                req7.mulColor = 0xFFDF0029;
                draw_sprite_draw_request(&req7);
            }
            if (lbl_1_bss_1704.unk1 == 1) {
                req2 = lbl_11_data_2944[i + 1];
                req2.x = 8.0f + req.x;
                req2.y = 6.0f + req.y;
            } else {
                req2 = lbl_11_data_2C64[1];
                req2.x = 3.0f + req.x;
                req2.y = 6.0f + req.y;
                draw_sprite_draw_request(&req2);
                req2 = lbl_11_data_2C64[i];
                req2.x = 15.0f + req.x;
                req2.y = 6.0f + req.y;
                draw_sprite_draw_request(&req2);
            }
            draw_sprite_draw_request(&req2);
            x += 30.0;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_2034;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_2214;
            req2.x = req.x;
            req2.y = 32.0f + req.y;
            req2.z -= 0.005;
            draw_sprite_draw_request(&req2);
            if (lbl_1_bss_1704.unk1 == 1) {
                req2 = lbl_11_data_3024;
                req2.x = 4.0f + req.x;
                req2.y = 6.0f + req.y;
                draw_sprite_draw_request(&req2);
                parSum = 0;
                for (i = 0; i < 9; i++) {
                    parSum += fn_11_16FC4(i);
                }
            } else {
                req2 = lbl_11_data_2FD4;
                req2.x = 20.0f + req.x;
                req2.y = 6.0f + req.y;
                draw_sprite_draw_request(&req2);
                parSum = 0;
                for (i = 9; i < 18; i++) {
                    parSum += fn_11_16FC4(i);
                }
            }
            req2 = lbl_11_data_2944[(parSum / 10)];
            req2.x = 8.0f + (8.0f + req.x);
            req2.y = 4.0f + (32.0f + req.y);
            req2.mulColor = 0xFFDF0029;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_2944[(parSum % 10)];
            req2.x = 8.0f + (20.0f + (8.0f + req.x));
            req2.y = 4.0f + (32.0f + req.y);
            req2.mulColor = 0xFFDF0029;
            draw_sprite_draw_request(&req2);
            x += 66.0;
        }
        req = lbl_11_data_2084;
        req.x += x;
        req.y += y;
        if (lbl_1_bss_1704.unk0 == 1) {
            req.scaleX *= 1.28;
        }
        draw_sprite_draw_request(&req);
        req2 = lbl_11_data_2F84;
        if (lbl_1_bss_1704.unk0 == 1) {
            req2.x = 20.0 + req.x;
        } else {
            req2.x = 9.0 + req.x;
        }
        req2.y = 6.0 + req.y;
        draw_sprite_draw_request(&req2);
        if (lbl_1_bss_1704.unk0 == 1) {
            x += 128.0;
        } else {
            x += 100.0;
        }
        req = lbl_11_data_20D4;
        req.x += x;
        req.y += y;
        draw_sprite_draw_request(&req);
        for (player = 0; player < modeCtrl.playerCount; player++) {
            x = baseX;
            y = 66.0 + baseY + 30.0 * player;
            req = lbl_11_data_2124;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_3254[player];
            req2.x = x;
            req2.y = y;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_1454[player];
            req2.x = 12.0f + x;
            req2.y = 9.0f + y;
            req2.z = 0.06f;
            draw_sprite_draw_request(&req2);
            if (fn_11_EFC4(player) == 0) {
                fn_11_1D260(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            } else if (fn_11_EFC4(player) == 1) {
                fn_11_1DB90(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            } else if (fn_11_EFC4(player) == 2) {
                fn_11_1D71C(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[player], 0);
            }
            x += 98.0;
            for (i = 0; i < 9; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_2174;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_21C4;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (lbl_1_bss_1704.unk0 == 0) {
                    if (lbl_1_bss_1704.unk1 == 1) {
                        strokes = fn_11_EC00(player, i);
                    } else if (lbl_1_bss_1704.unk1 == 2) {
                        strokes = fn_11_EC00(player, i + 9);
                    }
                    if (strokes < 10) {
                        req2 = lbl_11_data_2944[strokes];
                        req2.x = 8.0f + x;
                        req2.y = 4.0f + y;
                        if ((lbl_1_bss_1704.unk1 == 1) && (i < fn_11_EC30())) {
                            draw_sprite_draw_request(&req2);
                        } else if ((lbl_1_bss_1704.unk1 == 2) && ((i + 9) < fn_11_EC30())) {
                            draw_sprite_draw_request(&req2);
                        }
                    } else {
                        req2 = lbl_11_data_2C64[1];
                        req2.x = 3.0f + x;
                        req2.y = 4.0f + y;
                        if ((lbl_1_bss_1704.unk1 == 1) && (i < fn_11_EC30())) {
                            draw_sprite_draw_request(&req2);
                        } else if ((lbl_1_bss_1704.unk1 == 2) && ((i + 9) < fn_11_EC30())) {
                            draw_sprite_draw_request(&req2);
                        }
                        req2 = lbl_11_data_2C64[0];
                        req2.x = 16.0f + x;
                        req2.y = 4.0f + y;
                        if ((lbl_1_bss_1704.unk1 == 1) && (i < fn_11_EC30())) {
                            draw_sprite_draw_request(&req2);
                        } else if ((lbl_1_bss_1704.unk1 == 2) && ((i + 9) < fn_11_EC30())) {
                            draw_sprite_draw_request(&req2);
                        }
                    }
                    if ((fn_11_EEFC() != 0) && (lbl_1_bss_1704.unk1 == 1) && (i == fn_11_EC30())) {
                        if (fn_11_EC00(player, i) < 10) {
                            req2 = lbl_11_data_2944[fn_11_EC00(player, i)];
                            req2.x = 8.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                        } else {
                            req2 = lbl_11_data_2C64[1];
                            req2.x = 3.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                            req2 = lbl_11_data_2C64[0];
                            req2.x = 16.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                        }
                    } else if ((fn_11_EEFC() != 0) && (lbl_1_bss_1704.unk1 == 2) && ((i + 9) == fn_11_EC30())) {
                        if (fn_11_EC00(player, i + 9) < 10) {
                            req2 = lbl_11_data_2944[fn_11_EC00(player, i + 9)];
                            req2.x = 8.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                        } else {
                            req2 = lbl_11_data_2C64[1];
                            req2.x = 3.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                            req2 = lbl_11_data_2C64[0];
                            req2.x = 16.0f + x;
                            req2.y = 4.0f + y;
                            {
                                s16 blink;

                                GOLF_BLINK(blink);
                                req2.addColor = GOLF_BLINK_RGB(blink);
                            }
                            draw_sprite_draw_request(&req2);
                        }
                    }
                } else if (lbl_1_bss_1704.unk0 == 1) {
                    if (lbl_1_bss_1704.unk1 == 1) {
                        if (player == 0) {
                                                        if (fn_11_EC00(0, i) < fn_11_EC00(1, i)) {
                                req2 = lbl_11_data_3164;
                            } else {
                                                                if (fn_11_EC00(0, i) == fn_11_EC00(1, i)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i) > fn_11_EC00(1, i)) {
                                        req2 = lbl_11_data_3204;
                                    }
                                }
                            }
                        } else if (player == 1) {
                                                        if (fn_11_EC00(0, i) < fn_11_EC00(1, i)) {
                                req2 = lbl_11_data_3204;
                            } else {
                                                                if (fn_11_EC00(0, i) == fn_11_EC00(1, i)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i) > fn_11_EC00(1, i)) {
                                        req2 = lbl_11_data_3164;
                                    }
                                }
                            }
                        }
                    } else if (lbl_1_bss_1704.unk1 == 2) {
                        if (player == 0) {
                                                        if (fn_11_EC00(0, i + 9) < fn_11_EC00(1, i + 9)) {
                                req2 = lbl_11_data_3164;
                            } else {
                                                                if (fn_11_EC00(0, i + 9) == fn_11_EC00(1, i + 9)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i + 9) > fn_11_EC00(1, i + 9)) {
                                        req2 = lbl_11_data_3204;
                                    }
                                }
                            }
                        } else if (player == 1) {
                                                        if (fn_11_EC00(0, i + 9) < fn_11_EC00(1, i + 9)) {
                                req2 = lbl_11_data_3204;
                            } else {
                                                                if (fn_11_EC00(0, i + 9) == fn_11_EC00(1, i + 9)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i + 9) > fn_11_EC00(1, i + 9)) {
                                        req2 = lbl_11_data_3164;
                                    }
                                }
                            }
                        }
                    }
                    req2.x = 4.0f + x;
                    req2.y = 4.0f + y;
                    if ((lbl_1_bss_1704.unk1 == 1) && (i < fn_11_EC30())) {
                        draw_sprite_draw_request(&req2);
                    } else if ((lbl_1_bss_1704.unk1 == 2) && ((i + 9) < fn_11_EC30())) {
                        draw_sprite_draw_request(&req2);
                    }
                    if ((fn_11_EEFC() != 0) && (lbl_1_bss_1704.unk1 == 1) && (i == fn_11_EC30())) {
                        if (player == 0) {
                                                        if (fn_11_EC00(0, i) < fn_11_EC00(1, i)) {
                                req2 = lbl_11_data_3164;
                            } else {
                                                                if (fn_11_EC00(0, i) == fn_11_EC00(1, i)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i) > fn_11_EC00(1, i)) {
                                        req2 = lbl_11_data_3204;
                                    }
                                }
                            }
                        } else if (player == 1) {
                                                        if (fn_11_EC00(0, i) < fn_11_EC00(1, i)) {
                                req2 = lbl_11_data_3204;
                            } else {
                                                                if (fn_11_EC00(0, i) == fn_11_EC00(1, i)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i) > fn_11_EC00(1, i)) {
                                        req2 = lbl_11_data_3164;
                                    }
                                }
                            }
                        }
                        req2.x = 4.0f + x;
                        req2.y = 4.0f + y;
                        {
                            s16 blink;

                            GOLF_BLINK(blink);
                            req2.addColor = GOLF_BLINK_RGB(blink);
                        }
                        draw_sprite_draw_request(&req2);
                    } else if ((fn_11_EEFC() != 0) && (lbl_1_bss_1704.unk1 == 2) && ((i + 9) == fn_11_EC30())) {
                        if (player == 0) {
                                                        if (fn_11_EC00(0, i + 9) < fn_11_EC00(1, i + 9)) {
                                req2 = lbl_11_data_3164;
                            } else {
                                                                if (fn_11_EC00(0, i + 9) == fn_11_EC00(1, i + 9)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i + 9) > fn_11_EC00(1, i + 9)) {
                                        req2 = lbl_11_data_3204;
                                    }
                                }
                            }
                        } else if (player == 1) {
                                                        if (fn_11_EC00(0, i + 9) < fn_11_EC00(1, i + 9)) {
                                req2 = lbl_11_data_3204;
                            } else {
                                                                if (fn_11_EC00(0, i + 9) == fn_11_EC00(1, i + 9)) {
                                    req2 = lbl_11_data_31B4;
                                } else {
                                                                        if (fn_11_EC00(0, i + 9) > fn_11_EC00(1, i + 9)) {
                                        req2 = lbl_11_data_3164;
                                    }
                                }
                            }
                        }
                        req2.x = 4.0f + x;
                        req2.y = 4.0f + y;
                        {
                            s16 blink;

                            GOLF_BLINK(blink);
                            req2.addColor = GOLF_BLINK_RGB(blink);
                        }
                        draw_sprite_draw_request(&req2);
                    }
                }
                x += 30.0;
            }
            if (lbl_1_bss_1704.unk0 == 0) {
                req = lbl_11_data_2214;
                req.x += x;
                req.y += y;
                draw_sprite_draw_request(&req);
                req = lbl_11_data_3394[player];
                req.x = x;
                req.y = y;
                draw_sprite_draw_request(&req);
                total2 = 0;
                if (lbl_1_bss_1704.unk1 == 1) {
                    for (i = 0; i < 9; i++) {
                        total2 += fn_11_EC00(player, i);
                    }
                } else {
                    for (i = 0; i < 9; i++) {
                        total2 += fn_11_EC00(player, i + 9);
                    }
                }
                if (total2 < 10) {
                    req2 = lbl_11_data_2944[total2];
                    req2.x = 16.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                } else {
                    req2 = lbl_11_data_2944[(total2 / 10)];
                    req2.x = 16.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[(total2 % 10)];
                    req2.x = 36.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                }
                x += 66.0;
            }
            req = lbl_11_data_2264;
            req.x += x;
            req.y += y;
            lbl_11_bss_2AD0[player].x = req.x;
            lbl_11_bss_2AD0[player].y = req.y;
            if (lbl_1_bss_1704.unk0 == 1) {
                req.scaleX *= 1.28;
            }
            draw_sprite_draw_request(&req);
            req = lbl_11_data_34D4[player];
            req.x += x;
            req.y += y;
            if (lbl_1_bss_1704.unk0 == 1) {
                req.scaleX *= 1.28;
            }
            draw_sprite_draw_request(&req);
            if (lbl_1_bss_1704.unk0 == 1) {
                if (player == 0) {
                                        diff2 = fn_11_EC1C(0) - fn_11_EC1C(1);
                } else {
                                        diff2 = fn_11_EC1C(1) - fn_11_EC1C(0);
                }
                if (diff2 > 0) {
                    if (fabs((float) diff2) >= 10.0) {
                        req2 = lbl_11_data_2944[(s32) (fabs((float) diff2) / 10.0)];
                        req2.x = 10.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_2944[((s32) fabs((float) diff2) % 10)];
                        req2.x = 30.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_3074;
                        req2.x = 50.0f + x;
                        req2.y = 9.0f + y;
                        draw_sprite_draw_request(&req2);
                    } else {
                        req2 = lbl_11_data_2944[(s32) fabs((float) diff2)];
                        req2.x = 10.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_3074;
                        req2.x = 30.0f + x;
                        req2.y = 9.0f + y;
                        draw_sprite_draw_request(&req2);
                    }
                } else if (diff2 == 0) {
                    req2 = lbl_11_data_30C4;
                    req2.x = 12.0f + x;
                    req2.y = 9.0f + y;
                    draw_sprite_draw_request(&req2);
                } else if (diff2 < 0) {
                    if (fabs((float) diff2) >= 10.0) {
                        req2 = lbl_11_data_2944[(s32) (fabs((float) diff2) / 10.0)];
                        req2.x = 10.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_2944[((s32) fabs((float) diff2) % 10)];
                        req2.x = 30.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_3114;
                        req2.x = 50.0f + x;
                        req2.y = 9.0f + y;
                        draw_sprite_draw_request(&req2);
                    } else {
                        req2 = lbl_11_data_2944[(s32) fabs((float) diff2)];
                        req2.x = 10.0f + x;
                        req2.y = 4.0f + y;
                        draw_sprite_draw_request(&req2);
                        req2 = lbl_11_data_3114;
                        req2.x = 30.0f + x;
                        req2.y = 9.0f + y;
                        draw_sprite_draw_request(&req2);
                    }
                }
            } else if (lbl_1_bss_1704.unk0 == 0) {
                total2 = 0;
                if (lbl_1_bss_1704.unk1 == 0) {
                for (i = 0; i < 18; i++) {
                    total2 += fn_11_EC00(player, i);
                }
                } else if (lbl_1_bss_1704.unk1 == 1) {
                for (i = 0; i < 9; i++) {
                    total2 += fn_11_EC00(player, i);
                }
                } else if (lbl_1_bss_1704.unk1 == 2) {
                for (i = 9; i < 18; i++) {
                    total2 += fn_11_EC00(player, i);
                }
                }
                if (total2 >= 100) {
                    req2 = lbl_11_data_2944[(total2 / 100)];
                    req2.x = 10.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[((total2 % 100) / 10)];
                    req2.x = 30.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[(total2 % 10)];
                    req2.x = 50.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                } else if (total2 >= 10) {
                    req2 = lbl_11_data_2944[((total2 % 100) / 10)];
                    req2.x = 10.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                    req2 = lbl_11_data_2944[(total2 % 10)];
                    req2.x = 30.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                } else {
                    req2 = lbl_11_data_2944[(total2 % 10)];
                    req2.x = 10.0f + x;
                    req2.y = 4.0f + y;
                    draw_sprite_draw_request(&req2);
                }
            }
            if (lbl_1_bss_1704.unk0 == 1) {
                x += 128.0;
            } else {
                x += 100.0;
            }
            req = lbl_11_data_22B4;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
        }
        x = baseX;
        y = 66.0 + baseY + 30.0 * modeCtrl.playerCount;
        req = lbl_11_data_2304;
        req.x = x;
        req.y = y;
        draw_sprite_draw_request(&req);
        x += 98.0;
        for (i = 0; i < 9; i++) {
            if ((i % 2) == 0) {
                req = lbl_11_data_2354;
                req.x += x;
                req.y += y;
            } else {
                req = lbl_11_data_23A4;
                req.x += x;
                req.y += y;
            }
            draw_sprite_draw_request(&req);
            x += 30.0;
        }
        if (lbl_1_bss_1704.unk0 == 0) {
            req = lbl_11_data_23F4;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            x += 66.0;
        }
        req = lbl_11_data_2444;
        req.x += x;
        req.y += y;
        if (lbl_1_bss_1704.unk0 == 1) {
            req.scaleX *= 1.28;
        }
        draw_sprite_draw_request(&req);
        if (lbl_1_bss_1704.unk0 == 1) {
            x += 128.0;
        } else {
            x += 100.0;
        }
        req = lbl_11_data_2494;
        req.x += x;
        req.y += y;
        draw_sprite_draw_request(&req);
        if (lbl_1_bss_1704.unk0 == 1) {
            if (lbl_1_bss_1704.unk1 == 0) {
                wins = 18;
            } else if (lbl_1_bss_1704.unk1 == 2) {
                wins = 18;
            } else {
                wins = 9;
            }
            if (fn_11_EEFC() != 0) {
                wins -= 1;
            }
            wins -= fn_11_EC30();
            {
                int won1 = fn_11_EC1C(1);
                int won0 = fn_11_EC1C(0);

                /* holes left plus holes won by both = holes counted; a majority wins */
                wins = won1 + wins;
                wins = won0 + wins;
            }
            wins = wins / 2 + 1;
            req = lbl_11_data_24E4;
            draw_sprite_draw_request(&req);
            baseX = req.x;
            baseY = req.y;
            x = 98.0 + baseX;
            y = baseY;
            for (i = 0; i < 10; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_2584;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_25D4;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (i < 9) {
                    req = lbl_11_data_2944[i + 1];
                    req.x = 9.0f + (req.x + x);
                    req.y = 36.0f + (req.y + y);
                    req.mulColor = 0xFF69372E;
                    draw_sprite_draw_request(&req);
                } else {
                    req = lbl_11_data_2C64[1];
                    req.x = 3.0f + (req.x + x);
                    req.y = 36.0f + (req.y + y);
                    req.mulColor = 0xFF69372E;
                    draw_sprite_draw_request(&req);
                    req = lbl_11_data_2C64[0];
                    req.x = 16.0f + (req.x + x);
                    req.y = 36.0f + (req.y + y);
                    req.mulColor = 0xFF69372E;
                    draw_sprite_draw_request(&req);
                }
                x += 30.0;
            }
            req = lbl_11_data_2624;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_2674;
            x = baseX;
            y = 66.0 + baseY;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_3254[0];
            req2.x = x;
            req2.y = y;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_1454[0];
            req2.x = 12.0f + x;
            req2.y = 9.0f + y;
            req2.z = 0.06f;
            draw_sprite_draw_request(&req2);
            if (fn_11_EFC4(0) == 0) {
                fn_11_1D260(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[0], 0);
            } else if (fn_11_EFC4(0) == 1) {
                fn_11_1DB90(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[0], 0);
            } else if (fn_11_EFC4(0) == 2) {
                fn_11_1D71C(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[0], 0);
            }
            x += 98.0;
            for (i = 0; i < 10; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_26C4;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_2714;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (wins < (i + 1)) {
                    req8 = lbl_11_data_28F4;
                    req8.x = req.x;
                    req8.y = req.y;
                    draw_sprite_draw_request(&req8);
                    req8.y -= 32.0;
                    req8.z -= 0.02;
                    draw_sprite_draw_request(&req8);
                }
                if (fn_11_EC1C(0) > (i + 1)) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    draw_sprite_draw_request(&req);
                }
                if (((i + 1) == fn_11_EC1C(0)) && (fn_11_EEFC() != 0) && (fn_11_EC00(0, fn_11_EC30()) < fn_11_EC00(1, fn_11_EC30()))) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    {
                        s16 blink;

                        GOLF_BLINK(blink);
                        req.addColor = blink + (blink << 8) + (blink << 16);
                    }
                    draw_sprite_draw_request(&req);
                } else if ((i + 1) == fn_11_EC1C(0)) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    draw_sprite_draw_request(&req);
                }
                x += 30.0;
            }
            req = lbl_11_data_2764;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_2674;
            x = baseX;
            y = 96.0 + baseY;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            req2 = lbl_11_data_3254[1];
            req2.x = x;
            req2.y = y;
            draw_sprite_draw_request(&req2);
            req2 = lbl_11_data_1454[1];
            req2.x = 12.0f + x;
            req2.y = 9.0f + y;
            req2.z = 0.06f;
            draw_sprite_draw_request(&req2);
            if (fn_11_EFC4(1) == 0) {
                fn_11_1D260(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[1], 0);
            } else if (fn_11_EFC4(1) == 1) {
                fn_11_1DB90(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[1], 0);
            } else if (fn_11_EFC4(1) == 2) {
                fn_11_1D71C(6.0f + (44.0f + x), 2.0f + y, active_monkey_id[1], 0);
            }
            x += 98.0;
            for (i = 0; i < 10; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_26C4;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_2714;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                if (wins < (i + 1)) {
                    req9 = lbl_11_data_28F4;
                    req9.x = req.x;
                    req9.y = req.y;
                    draw_sprite_draw_request(&req9);
                }
                if (fn_11_EC1C(1) > (i + 1)) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    draw_sprite_draw_request(&req);
                }
                if (((i + 1) == fn_11_EC1C(1)) && (fn_11_EEFC() != 0) && (fn_11_EC00(0, fn_11_EC30()) > fn_11_EC00(1, fn_11_EC30()))) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    {
                        s16 blink;

                        GOLF_BLINK(blink);
                        req.addColor = blink + (blink << 8) + (blink << 16);
                    }
                    draw_sprite_draw_request(&req);
                } else if ((i + 1) == fn_11_EC1C(1)) {
                    req = lbl_11_data_3A74;
                    req.x = 4.0f + x;
                    req.y = 4.0f + y;
                    draw_sprite_draw_request(&req);
                }
                x += 30.0;
            }
            req = lbl_11_data_2764;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            req = lbl_11_data_27B4;
            x = baseX;
            y = 126.0 + baseY;
            req.x = x;
            req.y = y;
            draw_sprite_draw_request(&req);
            x += 98.0;
            for (i = 0; i < 10; i++) {
                if ((i % 2) == 0) {
                    req = lbl_11_data_2804;
                    req.x += x;
                    req.y += y;
                } else {
                    req = lbl_11_data_2854;
                    req.x += x;
                    req.y += y;
                }
                draw_sprite_draw_request(&req);
                x += 30.0;
            }
            req = lbl_11_data_28A4;
            req.x += x;
            req.y += y;
            draw_sprite_draw_request(&req);
            if ((fn_11_E9A8() == 0) || (fn_11_EF34() == 0)) {
                textdraw_reset();
                if (locale == 5) {
                    textdraw_set_font(0x8F);
                    textdraw_set_depth(0.08f);
                    textdraw_set_mul_color(0xFF69372E);
                    textdraw_set_pos(72.0f, 283.0f);
                    sprintf(buf2, "\x82\xB3\x82\xAB\x82\xC9%d\x82\xB5\x82\xE5\x82\xA4\x82\xB5\x82\xBD\x82\xD9\x82\xA4\x82\xCC\x82\xA9\x82\xBF\x82\xC5\x82\xB7", wins);
                } else if (locale == 0) {
                    textdraw_set_font(3);
                    textdraw_set_flags(0x80000000);
                    textdraw_set_depth(0.08f);
                    textdraw_set_mul_color(0xFF69372E);
                    textdraw_set_pos(78.0f, 286.0f);
                    sprintf(buf2, "The first person to get %d wins will win the game.", wins);
                    textdraw_set_scale(0.5f, 0.7f);
                }
                textdraw_print(buf2);
            }
        }
    }
}
