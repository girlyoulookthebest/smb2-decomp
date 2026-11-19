/* fight_9_23858: functions from 0x23858 (included by src/fight/fight_9.c in address order) */

/* fight_9_26798.inc */
void fn_12_28A6C(float x, float y);

void fn_12_2392C(int x, int y, int playerId);
void g_references_420_07(int playerId, float x, float y);
void fn_12_24108(s8 *status, struct Sprite *sprite);
void fn_12_24414(s8 *status, struct Sprite *sprite);
void fn_12_2445C(struct Sprite *sprite);
void fn_12_24550(s8 *status, struct Sprite *sprite);
void fn_12_24824(struct Sprite *sprite);
void fn_12_24FC8(s8 *status, struct Sprite *sprite);
void fn_12_25270(struct Sprite *sprite);
void fn_12_25900(s8 *status, struct Sprite *sprite);
void fn_12_25D84(struct Sprite *sprite);
void fn_12_2602C(void);
void fn_12_2612C(s8 *status, struct Sprite *sprite);
void fn_12_2660C(struct Sprite *sprite);

/* Creates the timer sprite and a HUD panel for every active player. */
void fn_12_23858(void)
{
    int i;
    int n;
    s8 *status;
    s16 (*pos)[2];

    fn_12_28A6C(64.0f, -124.0f);
    pos = lbl_12_data_9FE0[modeCtrl.playerCount - 1];
    n = 0;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, status++, pos++)
    {
        if (*status == 2)
        {
            fn_12_2392C((*pos)[0], (*pos)[1], i);
            n++;
        }
    }
}

/* Creates the HUD panel sprites of one player: face, player id, KO points, win bar, handicap gauge. */
void fn_12_2392C(int x, int y, int playerId)
{
    struct Sprite *sprite;
    float *rect;
    float v;

    g_references_420_07(playerId, x, y);

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->textAlign = 0;
        sprite->bmpId = lbl_12_data_A064[active_monkey_id[playerId]];
        sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
        sprite->userVar = playerId;
        sprite->mainFunc = fn_12_24108;
        sprite->drawFunc = fn_12_2445C;
        sprintf(sprite->text, "face");
        sprite->widescreenArg = 320;
        sprite->flags |= 0x01000000;
    }

    sprite = create_sprite();
    if (sprite != NULL)
    {
        if (lbl_12_bss_1ACA0.players[playerId].flags & 1)
            rect = lbl_12_data_A004[4];
        else
            rect = lbl_12_data_A004[playerId];
        v = lbl_12_data_A054[playerId];
        sprite->type = 1;
        sprite->x = x + 10;
        sprite->y = y - 15;
        sprite->textAlign = 0;
        sprite->bmpId = 0x130A;
        sprite->unk90 = rect[0];
        sprite->unk94 = rect[1] + v;
        sprite->unk98 = rect[2];
        sprite->unk9C = rect[3] + v;
        sprite->scaleX = 0.2f;
        sprite->scaleY = 0.25f;
        sprite->userVar = playerId;
        sprite->mainFunc = fn_12_24414;
        sprite->drawFunc = fn_12_2445C;
        sprintf(sprite->text, "player id");
        sprite->widescreenArg = 320;
        sprite->flags |= 0x01000000;
    }

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x + 50;
        sprite->y = y + 4;
        sprite->textAlign = 0;
        sprite->userVar = playerId;
        sprite->mainFunc = fn_12_24550;
        sprite->drawFunc = fn_12_24824;
        sprintf(sprite->text, "ko point");
        sprite->widescreenArg = 320;
        sprite->flags |= 0x01000000;
    }

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = lbl_12_data_9FF0[lbl_12_bss_1ACA0.winsNeeded - 1][0];
        sprite->y = lbl_12_data_9FF0[lbl_12_bss_1ACA0.winsNeeded - 1][1];
        sprite->textAlign = 0;
        sprite->userVar = playerId;
        sprite->mainFunc = fn_12_24FC8;
        sprite->drawFunc = fn_12_25270;
        sprintf(sprite->text, "win bar");
        sprite->widescreenArg = 320;
        sprite->flags |= 0x01000000;
    }

    if (lbl_12_bss_1ACA0.unk1760 == 0)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->type = 1;
            sprite->x = 10.0f;
            sprite->y = 34.0f;
            sprite->textAlign = 0;
            sprite->userVar = playerId;
            sprite->mainFunc = fn_12_25900;
            sprite->drawFunc = fn_12_25D84;
            sprintf(sprite->text, "handicap gauge");
        }
#ifdef __MWERKS__
        /* the original writes these outside the NULL check */
        sprite->widescreenArg = 320;
        sprite->flags |= 0x01000000;
#else
        if (sprite != NULL)
        {
            sprite->widescreenArg = 320;
            sprite->flags |= 0x01000000;
        }
#endif
    }
}

/* Stand-in for a function the linker stripped between fn_12_2392C and g_references_420_07: only
 * its literals (64/15, 4, 128/15) stay in .rodata. Unreferenced, so it is stripped again. */
void fight9_stripped_23DB8_literals(float *f)
{
    f[0] = 4.2666667f;
    f[1] = 4.0f;
    f[2] = 8.533334f;
}

/* Initializes the HUD panel state of one player. */
void g_references_420_07(int playerId, float x, float y)
{
    struct FightHud *hud;
    struct FightPlayer *player;
    int i;
    struct FightHudWin *win;

    hud = &lbl_12_bss_1EED8[playerId];
    player = &lbl_12_bss_1ACA0.players[playerId];
    hud->color = lbl_1_data_2E418[balls[playerId].colorId];
    hud->color.x *= 255.0f;
    hud->color.y *= 255.0f;
    hud->color.z *= 255.0f;
    hud->hidden = lbl_12_bss_1ACA0.unk1760;
    hud->baseX = x;
    hud->baseY = y;
    hud->x = x;
    hud->y = y;
    hud->velX = 0.0f;
    hud->velY = 0.0f;
    hud->angle = 0.0f;
    hud->angleVel = 0.0f;
    hud->koDisp = 0;
    hud->koVel = 0;
    if (hud->hidden)
        hud->koScaleY = 0.0f;
    else
        hud->koScaleY = 1.0f;
    hud->koScaleVel = 0.0f;
    if (hud->hidden)
        hud->winOffsetVel = 0.0f;
    else
        hud->winOffsetVel = 28.0f;
    memset(hud->wins, 0, sizeof(hud->wins));
    hud->winCount = player->wins;
    win = hud->wins;
    for (i = hud->winCount; i > 0; i--, win++)
    {
        win->scale = 1.0f;
        win->scaleTarget = win->scale;
    }
}

/* Kicks the face of a player's HUD panel sideways (random strength). */
void fn_12_23F60(int playerId)
{
    struct FightHud *hud = &lbl_12_bss_1EED8[playerId];

    if (rand() & 1)
        hud->faceVelX += 8.0f * (1.0f + 0.5f * RAND_FLOAT());
    else
        hud->faceVelY += 8.0f * (1.0f + 0.5f * RAND_FLOAT());
}

/* Gives a player's HUD panel a random spin. */
void fn_12_24050(int playerId)
{
    struct FightHud *hud = &lbl_12_bss_1EED8[playerId];
    float spin;

    spin = 1536.0f * (1.0f + 0.5f * RAND_FLOAT());
    if (rand() & 1)
        spin *= -1.0f;
    hud->angleVel += spin;
}

/* Face sprite main: panel sway/drop and face wobble, builds the panel matrix. */
void fn_12_24108(s8 *status, struct Sprite *sprite)
{
    struct FightHud *hud;
    struct FightPlayer *player;
    float shake;

    hud = &lbl_12_bss_1EED8[sprite->userVar];
    player = &lbl_12_bss_1ACA0.players[sprite->userVar];
    if (lbl_12_bss_1ACA0.unk1760 == 0)
        hud->hidden = lbl_12_bss_1ACA0.flags & 4;
    if (!(player->flags & 2))
    {
        if (hud->y > hud->baseY)
        {
            hud->y -= 7.4666667f;
            if (hud->y < hud->baseY)
                hud->y = hud->baseY;
        }
        hud->angleVel += -2048.0f * math_sin(hud->angle);
        hud->angleVel *= 0.96f;
        hud->angle += hud->angleVel;
    }
    else
    {
        float bottom = 160.0f + hud->baseY;

        if (hud->y < bottom)
        {
            hud->velY += 0.24f;
            hud->y += hud->velY;
            if (hud->y > bottom)
            {
                hud->y = bottom;
                hud->velY = 0.0f;
            }
        }
        hud->angleVel *= 0.98f;
        hud->angle += hud->angleVel;
    }
    mtxa_from_translate_xyz(72.0f + hud->x, 24.0f + hud->y, 0.0f);
    if (modeCtrl.wideMode)
        mtxa_scale_xyz(0.75f, 1.0f, 1.0f);
    mtxa_rotate_z(hud->angle);
    mtxa_translate_xyz(-(72.0f + hud->x), -(24.0f + hud->y), 0.0f);
    mtxa_to_mtx(hud->mtx);
    hud->faceVelX += 0.5f * -hud->faceX;
    hud->faceVelY += 0.5f * -hud->faceY;
    hud->faceVelX *= 0.8f;
    hud->faceVelY *= 0.8f;
    hud->faceX += hud->faceVelX;
    hud->faceY += hud->faceVelY;
    shake = 0.1f * hud->faceVelY - 0.1f * hud->faceVelX;
    sprite->scaleX = 1.0f - shake;
    sprite->scaleY = 1.0f + shake;
    sprite->x = hud->x + -24.0f * (sprite->scaleX - 1.0f);
    sprite->y = hud->y + -32.0f * (sprite->scaleY - 1.0f);
}

/* Player id sprite main: follows the panel. */
void fn_12_24414(s8 *status, struct Sprite *sprite)
{
    struct FightHud *hud = &lbl_12_bss_1EED8[sprite->userVar];

    sprite->x = 10.0f + hud->x;
    sprite->y = hud->y - 15.0f;
}

/* Draws a bitmap sprite in its panel's matrix (face, player id). */
void fn_12_2445C(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;

    GXLoadPosMtxImm(lbl_12_bss_1EED8[sprite->userVar].mtx, 0);
    req.bmpId = sprite->bmpId;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 5;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = sprite->scaleX;
    req.scaleY = sprite->scaleY;
    req.u1 = sprite->unk90;
    req.v1 = sprite->unk94;
    req.u2 = sprite->unk98;
    req.v2 = sprite->unk9C;
    req.x = sprite->x;
    req.y = sprite->y;
    req.z = sprite->depth;
    req.rotation = 0;
    draw_sprite_draw_request(&req);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

/* KO point sprite main: counter scale bounce and the rolling KO digits. */
void fn_12_24550(s8 *status, struct Sprite *sprite)
{
    struct FightHud *hud;
    struct FightPlayer *player;
    int prev;
    int target;

    hud = &lbl_12_bss_1EED8[sprite->userVar];
    player = &lbl_12_bss_1ACA0.players[sprite->userVar];
    if (hud->hidden)
    {
        float t = 1.0f - 0.035714287f * hud->winOffsetY;

        if (hud->koScaleY > t)
        {
            hud->koScaleVel = t - hud->koScaleY;
            hud->koScaleY = t;
        }
        else
            hud->koScaleVel = 0.0f;
    }
    else if (hud->koScaleY < 1.0f)
    {
        hud->koScaleVel += 0.05f * (1.0f - hud->koScaleY);
        hud->koScaleY += hud->koScaleVel;
    }
    else
    {
        hud->koScaleVel -= 0.024f;
        hud->koScaleVel *= 0.998f;
        hud->koScaleY += hud->koScaleVel;
        if (hud->koScaleY < 1.0f)
        {
            hud->koScaleY = 1.0f;
            if (hud->koScaleVel < 0.0f)
                hud->koScaleVel *= -0.8f + 0.2f * RAND_FLOAT();
        }
    }
    prev = (hud->koDisp + 0x2000) >> 16;
    target = player->score << 16;
    hud->koVel -= hud->koVel >> 6;
    if (hud->koDisp > target)
    {
        hud->koVel -= 0x444;
        hud->koDisp += hud->koVel;
        if (hud->koDisp < target)
        {
            hud->koDisp = target;
            if (hud->koVel < 0)
                hud->koVel = -0.375f * hud->koVel;
        }
    }
    else
    {
        hud->koVel += 0x444;
        hud->koDisp += hud->koVel;
        if (hud->koDisp > target)
        {
            hud->koDisp = target;
            if (hud->koVel > 0)
                hud->koVel = -0.375f * hud->koVel;
        }
    }
    if (prev != (hud->koDisp + 0x2000) >> 16)
        lbl_12_bss_1ACA0.flags |= 2;
}

/* Texture rect of digit d in the 5x2 KO digit sheet. */
#define KO_DIGIT_UV(req, d, off) \
    do { \
        (req).u1 = 0.2f * ((d) % 5) + (off); \
        (req).v1 = 0.5f * ((d) / 5); \
        (req).u2 = 0.2f + (req).u1 - (off); \
        (req).v2 = 0.5f + (req).v1; \
    } while (0)

/* KO point sprite draw: the rolling KO counter digits and the "KO" label. */
void fn_12_24824(struct Sprite *sprite)
{
    struct FightHud *hud;
    int ko;
    float x;
    float y;
    float digitX;
    float frac;
    float off;
    int d;
    int carry;
    struct SpriteDrawRequest req;

    hud = &lbl_12_bss_1EED8[sprite->userVar];
    ko = hud->koDisp;
    if (hud->koScaleY <= 0.0f)
        return;
    x = 50.0f + hud->x;
    y = 4.0f + hud->y;
    if (modeCtrl.wideMode)
        off = 0.01f;
    else
        off = 0.0f;
    mtxa_from_mtx(hud->mtx);
    mtxa_translate_xyz(0.0f, 28.0f + y, 0.0f);
    mtxa_scale_xyz(1.0f, hud->koScaleY, 1.0f);
    mtxa_translate_xyz(0.0f, -(28.0f + y), 0.0f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    digitX = 40.0f + x;
    req.bmpId = 0x130C;
    req.z = 0.1f;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 5;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 0.2f - off;
    req.rotation = 0;
    frac = (ko & 0xFFFF) / 65536.0f;
    ko >>= 16;
    carry = 0;
    d = ko % 10;
    ko = ko / 10;
    KO_DIGIT_UV(req, d, off);
    req.x = digitX;
    req.y = y;
    req.scaleY = 0.5f * (1.0f - frac);
    draw_sprite_draw_request(&req);
    if (frac > 0.0f)
    {
        d++;
        if (d > 9)
        {
            d -= 10;
            carry = 1;
        }
        KO_DIGIT_UV(req, d, off);
        req.y = y + 28.0f * (1.0f - frac);
        req.scaleY = 0.5f * frac;
        draw_sprite_draw_request(&req);
    }
    digitX -= 20.0f;
    while (ko > 0 || carry)
    {
        d = ko % 10;
        ko = ko / 10;
        if (!carry)
        {
            KO_DIGIT_UV(req, d, off);
            req.x = digitX;
            req.y = y;
            req.scaleY = 0.5f;
            draw_sprite_draw_request(&req);
        }
        else
        {
            carry = 0;
            if (ko != 0 || d != 0)
            {
                KO_DIGIT_UV(req, d, off);
                req.x = digitX;
                req.y = y;
                req.scaleY = 0.5f * (1.0f - frac);
                draw_sprite_draw_request(&req);
            }
            if (frac > 0.0f)
            {
                d++;
                if (d > 9)
                {
                    d -= 10;
                    carry = 1;
                }
                KO_DIGIT_UV(req, d, off);
                req.x = digitX;
                req.y = y + 28.0f * (1.0f - frac);
                req.scaleY = 0.5f * frac;
                draw_sprite_draw_request(&req);
            }
        }
        digitX -= 20.0f;
    }
    digitX = 56.0f + x;
    y = 11.0f + y;
    req.bmpId = 0x1309;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 5;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.x = digitX;
    req.y = y;
    req.z = 0.09f;
    req.rotation = 0;
    draw_sprite_draw_request(&req);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

/* Win bar sprite main: bar drop when hidden, new win stars pop in, star springs. */
void fn_12_24FC8(s8 *status, struct Sprite *sprite)
{
    struct FightHud *hud;
    int i;
    struct FightHudWin *win;
    struct FightPlayer *player;

    hud = &lbl_12_bss_1EED8[sprite->userVar];
    player = &lbl_12_bss_1ACA0.players[sprite->userVar];
    if (hud->hidden == 0)
    {
        float prev = hud->winOffsetY;

        hud->winOffsetY += 0.1f * -hud->winOffsetY;
        hud->winOffsetVel = hud->winOffsetY - prev;
    }
    else
    {
        hud->winOffsetVel += 0.32f;
        hud->winOffsetVel *= 0.99f;
        hud->winOffsetY += hud->winOffsetVel;
        if (hud->winOffsetY > 28.0f)
        {
            hud->winOffsetY = 28.0f;
            if (hud->winOffsetVel > 0.0f)
                hud->winOffsetVel *= -0.9f + 0.2f * RAND_FLOAT();
        }
    }
    if (hud->winCount != player->wins)
    {
        hud->winCount = (player->wins > 5) ? 5 : player->wins;
        win = hud->wins;
        for (i = hud->winCount; i > 0; i--, win++)
        {
            if (0.0f == win->scaleTarget)
            {
                win->scaleTarget = 1.5f;
                win->rotVel += 8192.0f * (RAND_FLOAT() - 0.5f);
                win->glowTarget = 1.0f;
            }
        }
    }
    win = hud->wins;
    for (i = 5; i > 0; i--, win++)
    {
        win->rotVel *= 0.98f;
        win->rotVel += -0.025f * win->rot;
        win->rot += win->rotVel;
        win->scaleVel *= 0.9f;
        win->scaleVel += 0.1f * (win->scaleTarget - win->scale);
        win->scale += win->scaleVel;
        win->glow += 0.05f * (win->glowTarget - win->glow);
    }
}

/* .data used by fn_12_25270 and fn_12_285E0. Defined here, after fn_12_24FC8, for the .data order
 * (mwcc emits string literals at the function that uses them, between the objects around it). */
/* win bar: star spacing per lbl_12_bss_1ACA0.winsNeeded (index - 1) */
static u8 lbl_12_data_A0A4[5] = { 0, 16, 16, 12, 10 };


/* Win bar sprite draw: the bar, the empty slots, the win stars and the sparkle of new stars. */
void fn_12_25270(struct Sprite *sprite)
{
    int i;
    int spacing;
    int n;
    int j;
    struct FightHudWin *win;
    struct FightHud *hud;
    u32 t;
    float x;
    float y;
    float px;
    float py;
    float z;
    float r;
    float u;
    Vec v;
    struct SpriteDrawRequest req;

    hud = &lbl_12_bss_1EED8[sprite->userVar];
    GXLoadPosMtxImm(hud->mtx, 0);
    x = hud->x + sprite->x;
    y = hud->winOffsetY + (hud->y + sprite->y);
    n = lbl_12_bss_1ACA0.winsNeeded;
    spacing = lbl_12_data_A0A4[n - 1];
    if (n > 0)
    {
        float w;

        px = 8.0f + x;
        py = y;
        w = (float)spacing * (float)(n - 1);
        if (n > 2)
            w += 8.0f * (n - 2);
        req.bmpId = 0x1300;
        req.opacity = 1.0f;
        req.unk30 = 2;
        req.flags = 5;
        req.mulColor = 0xFFFFFF;
        req.addColor = 0;
        req.scaleX = 0.125f * w;
        req.scaleY = 1.0f;
        req.u1 = 0.0f;
        req.v1 = 0.0f;
        req.u2 = 1.0f;
        req.v2 = 1.0f;
        req.x = px;
        req.y = py;
        req.z = 0.1f;
        req.rotation = 0;
        draw_sprite_draw_request(&req);
    }

    px = x - 2.0f;
    py = y - 2.0f;
    req.bmpId = 0x1301;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 5;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.z = 0.09f;
    req.rotation = 0;
    for (i = n; i > 0; i--)
    {
        req.x = px;
        req.y = py;
        draw_sprite_draw_request(&req);
        px += spacing + 8;
    }

    px = 10.0f + (x - 10.0f);
    py = 14.0f + (y - 13.0f);
    req.bmpId = 0x1304;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    win = hud->wins;
    for (i = (n < 5) ? n : 5; i > 0; i--, win++)
    {
        u = 0.2f * i;  /* depth by star index */
        req.z = 0.1 - 0.03f + (double)0.01f * u;
        if (win->scale > 0.0f)
        {
            req.x = px;
            req.y = py;
            req.scaleX = win->scale;
            req.scaleY = win->scale;
            req.rotation = win->rot;
            draw_sprite_draw_request(&req);
        }
        px += spacing + 8;
    }

    px = 10.0f + (x - 10.0f);
    py = 14.0f + (y - 13.0f);
    req.bmpId = 0x130B;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    win = hud->wins;
    for (i = (n < 5) ? n : 5; i > 0; i--, win++)
    {
        t = g_font_char_var_3 + (sprite->userVar << 11);
        if (win->scale <= 1.0f)
        {
            px += spacing + 8;
        }
        else
        {
            u = 0.2f * i;
            z = 0.1 - 0.03f + (double)0.01f * u;
            r = 32.0f * win->glow;
            mtxa_from_rotate_y(t << 6);
            mtxa_rotate_x(0x2000);
            mtxa_rotate_z(t << 9);
            for (j = 9; j > 0; j--)
            {
                mtxa_rotate_z(0x1C71);
                mtxa_tf_vec_xyz(0.0f, r, 0.0f, &v);
                req.x = px + v.x;
                req.y = py + v.y;
                req.z = z + 3.125e-05f * v.z;
                req.rotation = (t << 5) + (j << 9);
                mtxa_push();
                draw_sprite_draw_request(&req);
                mtxa_pop();
            }
            px += spacing + 8;
        }
    }

    /* set up for another pass that draws nothing (dead in the original too) */
    px = 10.0f + (x - 10.0f);
    py = 14.0f + (y - 13.0f);
    req.bmpId = 0x130B;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 10;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.z = 0.07f;
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

/* Handicap gauge sprite main: highlights the player's handicap segment; while the handicap can be
 * changed (lbl_12_bss_1ACA0 flag 0x80, player not ready) the other segments blink. */
void fn_12_25900(s8 *status, struct Sprite *sprite)
{
    int handicap;
    float f;
    struct FightHud *hud;
    int mode;
    Vec c;
    int sel;
    struct FightPlayer *player;
    int k;
    int blink;
    u32 frame;
    struct FightHudGauge *g;

    hud = &lbl_12_bss_1EED8[sprite->userVar];
    player = &lbl_12_bss_1ACA0.players[sprite->userVar];
    if ((lbl_12_bss_1ACA0.flags & 0x80) == 0)
        mode = 0;
    else if (player->rank == 0)
        mode = 1;
    else
        mode = 2;
    handicap = player->handicap;
    frame = g_font_char_var_3;
    switch (mode)
    {
    case 0:
    case 2:
        g = hud->gauge;
        for (k = -2; k <= 2; k++, g++)
        {
            if (handicap == k)
            {
                g->mul.x += 0.1f * (255.0f - g->mul.x);
                g->mul.y += 0.1f * (255.0f - g->mul.y);
                g->mul.z += 0.1f * (255.0f - g->mul.z);
            }
            else
            {
                g->mul.x += 0.1f * (136.0f - g->mul.x);
                g->mul.y += 0.1f * (128.0f - g->mul.y);
                g->mul.z += 0.1f * (128.0f - g->mul.z);
            }
            g->add.x *= 0.9f;
            g->add.y *= 0.9f;
            g->add.z *= 0.9f;
        }
        break;
    case 1:
        c = hud->color;
        c.x = 0.5f * (256.0f + c.x);
        c.y = 0.5f * (256.0f + c.y);
        c.z = 0.5f * (256.0f + c.z);
        blink = lbl_12_bss_1ACA0.round % 2;
#ifndef __MWERKS__
        f = 0.0f;  /* blink is -1 for a negative odd round: f stays unset in the original */
#endif
        switch (blink)
        {
        case 1:
            sel = (g_font_char_var_3 >> 4) & 7;
            if (sel >= 5)
                sel = 5 - (sel - 3);
            sel -= 2;
            break;
        }
        g = hud->gauge;
        for (k = -2; k <= 2; k++, g++)
        {
            if (handicap == k)
            {
                g->mul.x += 0.1f * (255.0f - g->mul.x);
                g->mul.y += 0.1f * (255.0f - g->mul.y);
                g->mul.z += 0.1f * (255.0f - g->mul.z);
            }
            else
            {
                g->mul.x += 0.1f * (136.0f - g->mul.x);
                g->mul.y += 0.1f * (128.0f - g->mul.y);
                g->mul.z += 0.1f * (128.0f - g->mul.z);
            }
            if (k == handicap)
            {
                g->add.x += 0.1f * (255.0f - g->add.x);
                g->add.y += 0.1f * (255.0f - g->add.y);
                g->add.z += 0.1f * (255.0f - g->add.z);
            }
            else
            {
                switch (blink)
                {
                case 0:
                    if (((frame >> 5) + k) & 1)
                        f = 0.0f;
                    else
                        f = 1.0f;
                    break;
                case 1:
                    if (k == sel)
                        f = 1.0f;
                    else
                        f = 0.0f;
                    break;
                }
                g->add.x += 0.1f * (c.x * f - g->add.x);
                g->add.y += 0.1f * (c.y * f - g->add.y);
                g->add.z += 0.1f * (c.z * f - g->add.z);
            }
        }
        break;
    }
}

/* Sega's float -> u8 quantized store (GQR2), as in main_loop/hud.c. */
static inline void fight9_f32_to_u8(register float *in, register u8 *out)
{
#ifdef __MWERKS__
    asm
    {
        lfs f1, 0(in)
        psq_st f1, 0(out), 1, 2
    }
#else
    *out = pc_psq_st_u8(*in);
#endif
}

/* Handicap gauge sprite draw: frame and the five coloured segments. */
void fn_12_25D84(struct Sprite *sprite)
{
    struct FightHud *hud;
    float x;
    float y;
    float py;
    struct SpriteDrawRequest req;
    int k;
    GXColor c;
    struct FightHudGauge *g;

    hud = &lbl_12_bss_1EED8[sprite->userVar];
    GXLoadPosMtxImm(hud->mtx, 0);
    x = hud->x + sprite->x;
    y = hud->y + sprite->y;
    py = y;
    req.bmpId = 0x1303;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 5;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 1.0f;
    req.scaleY = 1.0f;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.x = x;
    req.y = py;
    req.z = 0.1f;
    req.rotation = 0;
    draw_sprite_draw_request(&req);
    x = 40.0f + x;
    py = 2.0f + y;
    req.bmpId = 0x1302;
    req.y = py;
    g = hud->gauge;
    for (k = -2; k <= 2; k++, g++)
    {
        fight9_f32_to_u8(&g->mul.x, &c.r);
        fight9_f32_to_u8(&g->mul.y, &c.g);
        fight9_f32_to_u8(&g->mul.z, &c.b);
        req.mulColor = (c.r << 16) | (c.g << 8) | c.b;
        fight9_f32_to_u8(&g->add.x, &c.r);
        fight9_f32_to_u8(&g->add.y, &c.g);
        fight9_f32_to_u8(&g->add.z, &c.b);
        req.addColor = (c.r << 16) | (c.g << 8) | c.b;
        req.x = x;
        draw_sprite_draw_request(&req);
        x += 17.0f;
    }
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

/* Creates the "FIGHT COUNT DOWN" sprite (3, 2, 1, FIGHT). */
void fn_12_25FA8(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        fn_12_2602C();
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->mainFunc = fn_12_2612C;
        sprite->drawFunc = fn_12_2660C;
        sprintf(sprite->text, "FIGHT COUNT DOWN");
    }
}

/* Resets the countdown state. */
void fn_12_2602C(void)
{
    int i;
    struct FightCountdownDigit *d;

    lbl_12_bss_1F568.state = 0;
    lbl_12_bss_1F568.digit = -1;
    d = lbl_12_bss_1F568.digits;
    for (i = 0; i < 4; i++, d++)
    {
        d->scale = 0.0f;
        d->vel = 0.0f;
        d->target = 0.0f;
    }
    lbl_12_bss_1F568.fightScale.x = 0.0f;
    lbl_12_bss_1F568.fightScale.y = 0.0f;
    lbl_12_bss_1F568.fightTarget = lbl_12_bss_1F568.fightScale;
    lbl_12_bss_1F568.fightVel.x = 0.0f;
    lbl_12_bss_1F568.fightVel.y = 0.0f;
}

/* Countdown sprite main: pops the seconds digits while modeCtrl.submodeTimer runs down, then
 * shows "FIGHT" for a second and throws it off screen. */
void fn_12_2612C(s8 *status, struct Sprite *sprite)
{
    s16 secs;
    struct FightCountdownDigit *d;
    int i;

    switch (lbl_12_bss_1F568.state)
    {
    case 0:
        lbl_12_bss_1F568.state = 1;
        /* fall through */
    case 1:
        secs = (modeCtrl.submodeTimer + 59) / 60;
        if (lbl_12_bss_1F568.digit != secs)
        {
            if (lbl_12_bss_1F568.digit >= 0)
            {
                d = &lbl_12_bss_1F568.digits[lbl_12_bss_1F568.digit];
                d->scale = 0.0f;
                d->vel = 0.0f;
                d->target = 0.0f;
            }
            lbl_12_bss_1F568.digit = secs;
            d = &lbl_12_bss_1F568.digits[secs];
            d->target = 1.0f;
            d->vel += 0.3f * RAND_FLOAT();
            if (secs > 0)
                call_SoundReqID_arg_0(0x3D806);
        }
        if (modeCtrl.submodeTimer <= 0)
            lbl_12_bss_1F568.state = 2;
        break;
    case 2:
        lbl_12_bss_1F568.state = 3;
        lbl_12_bss_1F568.timer = 60;
        if (lbl_12_bss_1F568.digit >= 0)
        {
            d = &lbl_12_bss_1F568.digits[lbl_12_bss_1F568.digit];
            d->scale = 0.0f;
            d->target = 0.0f;
            d->vel = 0.0f;
            lbl_12_bss_1F568.digit = -1;
        }
        lbl_12_bss_1F568.fightTarget.x = 1.0f;
        lbl_12_bss_1F568.fightTarget.y = 1.0f;
        lbl_12_bss_1F568.fightVel.x = 0.5f * RAND_FLOAT();
        lbl_12_bss_1F568.fightVel.y = 0.5f * RAND_FLOAT();
        /* fall through */
    case 3:
        lbl_12_bss_1F568.timer--;
        if (lbl_12_bss_1F568.timer < 0)
            lbl_12_bss_1F568.state = 4;
        break;
    case 4:
        lbl_12_bss_1F568.state = 5;
        lbl_12_bss_1F568.fightTarget.x = 3.0f;
        lbl_12_bss_1F568.fightTarget.y = -0.2f;
        lbl_12_bss_1F568.fightVel.x -= 0.5f;
        lbl_12_bss_1F568.fightVel.y += 0.4f;
        /* fall through */
    case 5:
        if (lbl_12_bss_1F568.fightScale.y < 0.0f)
            *status = 0;
        break;
    }
    d = lbl_12_bss_1F568.digits;
    for (i = 4; i > 0; i--, d++)
    {
        d->vel *= 0.8f;
        d->vel += 0.2f * (d->target - d->scale);
        d->scale += d->vel;
    }
    lbl_12_bss_1F568.fightVel.x *= 0.8f;
    lbl_12_bss_1F568.fightVel.y *= 0.8f;
    lbl_12_bss_1F568.fightVel.x += 0.1f * (lbl_12_bss_1F568.fightTarget.x - lbl_12_bss_1F568.fightScale.x);
    lbl_12_bss_1F568.fightVel.y += 0.025f * (lbl_12_bss_1F568.fightTarget.y - lbl_12_bss_1F568.fightScale.y);
    lbl_12_bss_1F568.fightScale.x += lbl_12_bss_1F568.fightVel.x;
    lbl_12_bss_1F568.fightScale.y += lbl_12_bss_1F568.fightVel.y;
}

/* Countdown sprite draw: the digits and "FIGHT". */
void fn_12_2660C(struct Sprite *sprite)
{
    int i;
    struct FightCountdownDigit *d;
    float sx;
    float sy;

    set_ui_widescreen_scale_mtx(320);
    textdraw_reset();
    textdraw_set_font(9);
    d = lbl_12_bss_1F568.digits;
    for (i = 0; i < 4; i++, d++)
    {
        if (d->scale > 0.0f)
        {
            sx = d->scale + d->vel;
            sy = d->scale - d->vel;
            textdraw_set_pos(320.0f - 72.0f * sx, 240.0f - 64.0f * sy);
            textdraw_set_scale(2.0f * sx, 2.0f * sy);
            textdraw_put_char(i + '0');
        }
    }
    textdraw_reset();
    textdraw_set_font(9);
    if (lbl_12_bss_1F568.fightScale.y > 0.0f)
    {
        textdraw_set_pos(320.0f - 144.0f * lbl_12_bss_1F568.fightScale.x,
                         240.0f - 32.0f * lbl_12_bss_1F568.fightScale.y);
        textdraw_set_scale(lbl_12_bss_1F568.fightScale.x, lbl_12_bss_1F568.fightScale.y);
        textdraw_print("FIGHT");
    }
    reset_ui_widescreen_scale_mtx();
}

/* .data used by the 0x26798 fragment, placed after fn_12_2660C (its "FIGHT" string comes first). */
/* round banner paths per number of banners */
static struct FightBannerPath lbl_12_data_A0C8[1] = {
    { { 320.0f, 240.0f, 0.0f }, { 320.0f, 120.0f, 0.0f }, 0.0f, 1.0f },
};
static struct FightBannerPath lbl_12_data_A0E8[2] = {
    { { -160.0f, 240.0f, 0.0f }, { 160.0f, 240.0f, 0.0f }, 0.75f, 0.75f },
    { { 800.0f, 240.0f, 0.0f }, { 480.0f, 240.0f, 0.0f }, 0.75f, 0.75f },
};
static struct FightBannerPath lbl_12_data_A128[3] = {
    { { 320.0f, 180.0f, 0.0f }, { 320.0f, 140.0f, 0.0f }, 0.0f, 0.75f },
    { { -160.0f, 300.0f, 0.0f }, { 160.0f, 300.0f, 0.0f }, 0.75f, 0.75f },
    { { 800.0f, 300.0f, 0.0f }, { 480.0f, 300.0f, 0.0f }, 0.75f, 0.75f },
};
static struct FightBannerPath lbl_12_data_A188[4] = {
    { { -160.0f, 180.0f, 0.0f }, { 160.0f, 140.0f, 0.0f }, 0.6f, 0.6f },
    { { 800.0f, 180.0f, 0.0f }, { 480.0f, 140.0f, 0.0f }, 0.6f, 0.6f },
    { { -160.0f, 300.0f, 0.0f }, { 160.0f, 300.0f, 0.0f }, 0.6f, 0.6f },
    { { 800.0f, 300.0f, 0.0f }, { 480.0f, 300.0f, 0.0f }, 0.6f, 0.6f },
};
static struct FightBannerPath *lbl_12_data_A208[4] = {
    lbl_12_data_A0C8, lbl_12_data_A0E8, lbl_12_data_A128, lbl_12_data_A188,
};
/* colour per ball colorId (fight.c, fight_2.c, fight_3.c, fight_8.c, fight_9.c) */
GXColor lbl_12_data_A218[10] = {
    { 0xFF, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0x00, 0xFF },
    { 0x00, 0xFF, 0x00, 0xFF },
    { 0xFF, 0x00, 0xFF, 0xFF },
    { 0x00, 0xFF, 0xFF, 0xFF },
    { 0xFF, 0x80, 0x00, 0xFF },
    { 0x80, 0x00, 0xFF, 0xFF },
    { 0xFF, 0x00, 0x80, 0xFF },
    { 0x80, 0xFF, 0x00, 0xFF },
};
