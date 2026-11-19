/* fight_9_26798: functions from 0x26798 (included by src/fight/fight_9.c in address order) */

static void fn_12_2699C(s8 *status, struct Sprite *sprite);
static void fn_12_26CC4(s8 *status, struct Sprite *sprite);
static void fn_12_26F30(s8 *status, struct Sprite *sprite);

/* Create the "nP WIN" / "COM WIN" labels of the round winners */
void fn_12_26798(void)
{
    int i;
    struct Sprite *sprite;
    struct FightPlayer *player;
    struct Ball *ball;
    s8 *status;
    int n;
    struct FightBannerPath *layout;
    GXColor color;
    int count;

    count = lbl_12_bss_1ACA0.rankCounts[0];
    if (count > 0)
    {
        layout = lbl_12_data_A208[count - 1];
        n = 0;
        player = lbl_12_bss_1ACA0.players;
        ball = balls;
        status = g_poolInfo.playerPool.statusList;
        for (i = 0; i < 4; i++, player++, ball++, status++)
        {
            if (*status != 0 && player->rank == 0)
            {
                lbl_12_bss_1F5C4[i] = *layout;
                color = lbl_12_data_A218[ball->colorId];
                sprite = create_sprite();
                if (sprite != NULL)
                {
                    sprite->x = layout->start.x;
                    sprite->y = layout->start.y;
                    sprite->fontId = 9;
                    sprite->textAlign = ALIGN_CB;
                    sprite->depth = 0.1f;
                    sprite->mulR = (color.r + 0xFF) >> 1;
                    sprite->mulG = (color.g + 0xFF) >> 1;
                    sprite->mulB = (color.b + 0xFF) >> 1;
                    sprite->userVar = i;
                    sprite->mainFunc = fn_12_2699C;
                    if (player->flags & 1)
                        sprintf(sprite->text, "COM WIN");
                    else
                        sprintf(sprite->text, "%dP WIN", i + 1);
                    sprite->widescreenArg = 0x140;
                    sprite->flags |= 0x01000000;
                }
                n++;
                layout++;
            }
        }
    }
}

/* "nP WIN" label: slide to the end position, grow to the end scale */
static void fn_12_2699C(s8 *status, struct Sprite *sprite)
{
    struct FightBannerPath *anim = &lbl_12_bss_1F5C4[sprite->userVar];

    if (anim->start.x < anim->end.x)
    {
        anim->start.x += 10.666667f;
        if (anim->start.x > anim->end.x)
            anim->start.x = anim->end.x;
    }
    else if (anim->start.x > anim->end.x)
    {
        anim->start.x -= 10.666667f;
        if (anim->start.x < anim->end.x)
            anim->start.x = anim->end.x;
    }
    if (anim->start.y < anim->end.y)
    {
        anim->start.y += 10.666667f;
        if (anim->start.y > anim->end.y)
            anim->start.y = anim->end.y;
    }
    else if (anim->start.y > anim->end.y)
    {
        anim->start.y -= 10.666667f;
        if (anim->start.y < anim->end.y)
            anim->start.y = anim->end.y;
    }
    anim->unk18 += 0.15f * (anim->unk1C - anim->unk18);
    sprite->x = anim->start.x;
    sprite->y = anim->start.y;
    sprite->scaleX = anim->unk1C;
    sprite->scaleY = anim->unk18;
}

/* Create the "ROUND n" / "FINAL ROUND" banner, one sprite per letter */
void fn_12_26B04(void)
{
    /* letter animation, alternating per round */
    static void (*lbl_12_data_A250[2])(s8 *, struct Sprite *) = { fn_12_26CC4, fn_12_26F30 };
    char text[256];
    int len;
    u8 r, g, b;
    u8 *p;
    int counter;
    void (*mainFunc)(s8 *, struct Sprite *);
    float x;
    struct Sprite *sprite;

    lbl_12_bss_1F644 |= 4;
    r = 0xFF;
    g = 0xFF;
    b = 0xFF;
    if (fn_12_174E8())
        sprintf(text, "FINAL ROUND");
    else
        sprintf(text, "ROUND %d", lbl_12_bss_1ACA0.round + 1);
    mainFunc = lbl_12_data_A250[lbl_12_bss_1ACA0.round & 1];
    len = strlen(text);
    x = 320.0f - 18.0f * (float)(len - 1);
    p = (u8 *)text;
    counter = 0;
    while (len > 0)
    {
        if (*p != ' ')
        {
            sprite = create_sprite();
            if (sprite != NULL)
            {
                sprite->x = x;
                sprite->y = 256.0f;
                sprite->fontId = 9;
                sprite->textAlign = ALIGN_CB;
                sprite->depth = 0.1f;
                sprite->scaleX = 0.0f;
                sprite->scaleY = 0.0f;
                sprite->addR = r;
                sprite->addG = g;
                sprite->addB = b;
                sprite->counter = counter;
                sprite->userVar = len;
                sprite->mainFunc = mainFunc;
                ((u8 *)sprite->text)[0] = *p;
                sprite->text[1] = 0;
                sprite->widescreenArg = 0x140;
                sprite->flags |= 0x01000000;
            }
        }
        len--;
        p++;
        x += 36.0f;
        counter += 6;
    }
}

/* fade the additive white of a banner letter */
#define FADE_ADD(sprite) \
    if ((sprite)->addR != 0) \
        (sprite)->addR = 0.9f * (sprite)->addR; \
    if ((sprite)->addG != 0) \
        (sprite)->addG = 0.9f * (sprite)->addG; \
    if ((sprite)->addB != 0) \
        (sprite)->addB = 0.9f * (sprite)->addB;

/* Round banner letter (even rounds): drop in, then squash away vertically */
static void fn_12_26CC4(s8 *status, struct Sprite *sprite)
{
    if (!(lbl_12_bss_1F644 & 4))
    {
        if (sprite->userVar & 1)
            sprite->y += 2.1333334f;
        else
            sprite->y -= 2.1333334f;
        sprite->scaleY += 0.2f * (-0.05f - sprite->scaleY);
        if (sprite->scaleY <= 0.0f)
        {
            sprite->scaleY = 0.0f;
            *status = 0;
        }
        FADE_ADD(sprite)
    }
    else
    {
        if (sprite->counter != 0)
        {
            sprite->counter--;
            return;
        }
        sprite->scaleX = 0.5f;
        sprite->scaleY += 0.2f * (0.5f - sprite->scaleY);
        FADE_ADD(sprite)
    }
}

/* Round banner letter (odd rounds): grow in horizontally, then squash away */
static void fn_12_26F30(s8 *status, struct Sprite *sprite)
{
    if (!(lbl_12_bss_1F644 & 4))
    {
        sprite->scaleX += 0.2f * (-0.05f - sprite->scaleX);
        if (sprite->scaleX <= 0.0f)
        {
            sprite->scaleX = 0.0f;
            *status = 0;
        }
        FADE_ADD(sprite)
    }
    else
    {
        if (sprite->counter != 0)
        {
            sprite->counter--;
            return;
        }
        sprite->scaleX += 0.2f * (0.5f - sprite->scaleX);
        sprite->scaleY = 0.5f;
        FADE_ADD(sprite)
    }
}

static void fn_12_272CC(s8 *status, struct Sprite *sprite);
static void fn_12_27568(int i, Vec *out);
static void fn_12_275B8(int i, Vec *out);

/* Create the "SUDDEN DEATH" banner, one sprite per letter, starting after `delay` frames */
void fn_12_27168(int delay)
{
    char text[64];
    float x;
    float y;
    int i;
    u8 *p;
    struct Sprite *sprite;

    strcpy(text, "SUDDEN DEATH");
    x = 320.0f - 43.2f * (0.5f * (strlen(text) - 1));
    y = 224.0f;
    p = (u8 *)text;
    i = 0;
    while (*p != 0)
    {
        if (*p != ' ')
        {
            sprite = create_sprite();
            if (sprite != NULL)
            {
                sprite->x = x;
                sprite->y = y;
                sprite->fontId = 9;
                sprite->textAlign = ALIGN_CC;
                sprite->scaleX = 0.0f;
                sprite->scaleY = 0.0f;
                sprite->userVar = i;
                sprite->mainFunc = fn_12_272CC;
                sprite->counter = delay + i * 4;
                ((u8 *)sprite->text)[0] = *p;
                sprite->text[1] = 0;
                sprite->widescreenArg = 0x140;
                sprite->flags |= 0x01000000;
            }
        }
        i++;
        p++;
        x += 43.2f;
    }
}

/* "SUDDEN DEATH" letter: fly in from the HUD, then squash away when the round starts */
static void fn_12_272CC(s8 *status, struct Sprite *sprite)
{
    Vec pos;
    float t;

    if (sprite->counter == 0)
    {
        if (!(lbl_12_bss_1ACA0.flags & 4))
        {
            sprite->scaleX *= 1.1f;
            sprite->scaleY += 0.15f * (-0.05f - sprite->scaleY);
            if (sprite->scaleY <= 0.0f)
                *status = 0;
        }
    }
    else
    {
        sprite->counter--;
        if (sprite->counter == 0)
        {
            fn_12_275B8(sprite->userVar, &pos);
            sprite->x = pos.x;
            sprite->y = pos.y;
            sprite->scaleX = 0.4f;
            sprite->scaleY = 0.4f;
        }
        else if (sprite->counter < 15.0f)
        {
            fn_12_275B8(sprite->userVar, &pos);
            t = 1.0f / (sprite->counter + 1);
            sprite->x += t * (pos.x - sprite->x);
            sprite->y += t * (pos.y - sprite->y);
            sprite->scaleX += (0.4f - sprite->scaleX) * t;
            sprite->scaleY += (0.4f - sprite->scaleY) * t;
        }
        else if (sprite->counter < 120.0f)
        {
            fn_12_27568(sprite->userVar, &pos);
            if (sprite->counter > 112.5f)
            {
                t = (120.0f - sprite->counter) / 7.5f;
                sprite->scaleX = 0.6f * t;
                sprite->scaleY = 0.6f * t;
            }
            else
            {
                sprite->scaleX = 0.6f;
                sprite->scaleY = 0.6f;
            }
            sprite->x = pos.x;
            sprite->y = pos.y;
        }
    }
}

/* screen position of letter i of the big "SUDDEN DEATH" banner */
static void fn_12_27568(int i, Vec *out)
{
    out->x = 82.39999055862427 + 43.20000171661377 * (float)i;
    out->y = 224.0f;
}

/* screen position of letter i of the small "SUDDEN DEATH" HUD label (two lines) */
static void fn_12_275B8(int i, Vec *out)
{
    if (i > 6)
    {
        out->x = 54.399999141693115 + 28.800000429153442 * (float)(i - 7);
        out->y = 60.8f;
    }
    else
    {
        out->x = 39.999998927116394 + 28.800000429153442 * (float)i;
        out->y = 35.2f;
    }
}

static void fn_12_27898(s8 *status, struct Sprite *sprite);
static void fn_12_27A40(struct Sprite *sprite);
static void fn_12_27BA4(struct Sprite *sprite);
static void fn_12_2837C(s8 *status, struct Sprite *sprite);
static void fn_12_285E0(struct Sprite *sprite);

/* KO points of player i over the rounds played, capped at 9999 */
static inline int fight9_ko_total(int i)
{
    int n;
    struct Fight1C518 *log;
    int sum;

    sum = 0;
    log = lbl_12_bss_1C518;
    for (n = lbl_12_bss_1ACA0.round + 1; n > 0; n--, log++)
    {
        sum += log->unk4[i];
        if (sum > 9999)
        {
            sum = 9999;
            break;
        }
    }
    return sum;
}

/* Create the KO points and win bar shown over each fighter */
void fn_12_27654(void)
{
    int i;
    s8 *status;
    void (*drawFunc)(struct Sprite *);
    struct FightTag *tag;
    struct Sprite *sprite;

    if (lbl_12_bss_1ACA0.unk1760 != 0)
        drawFunc = fn_12_27A40;
    else
        drawFunc = fn_12_27BA4;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, status++)
    {
        if (*status == 0)
            continue;
        tag = &lbl_12_bss_1F648[i];
        memset(tag, 0, sizeof(*tag));
        tag->koTarget = fight9_ko_total(i) << 16;

        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->type = 1;
            sprite->x = 0.0f;
            sprite->y = 0.0f;
            sprite->textAlign = ALIGN_LT;
            sprite->userVar = i;
            sprite->mainFunc = fn_12_27898;
            sprite->drawFunc = drawFunc;
            sprintf(sprite->text, "ko point");
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->type = 1;
            sprite->x = -50.0f + lbl_12_data_9FF0[lbl_12_bss_1ACA0.winsNeeded - 1][0];
            sprite->y = -8.0f + lbl_12_data_9FF0[lbl_12_bss_1ACA0.winsNeeded - 1][1];
            sprite->textAlign = ALIGN_LT;
            sprite->userVar = i;
            sprite->mainFunc = fn_12_2837C;
            sprite->drawFunc = fn_12_285E0;
            sprintf(sprite->text, "win bar");
        }
    }
}

/* KO points over a fighter: roll the displayed value towards the total */
static void fn_12_27898(s8 *status, struct Sprite *sprite)
{
    struct FightTag *tag;
    int prev;
    int target;

    tag = &lbl_12_bss_1F648[sprite->userVar];
    prev = (tag->koDisp + 0x2000) >> 16;
    if (tag->flags & 1)
        target = tag->koTarget;
    else
        target = 0;
    tag->koVel -= tag->koVel >> 6;
    if (tag->koDisp > target)
    {
        tag->koVel -= 0x444;
        tag->koDisp += tag->koVel;
        if (tag->koDisp < target)
        {
            tag->koDisp = target;
            if (tag->koVel < 0)
                tag->koVel = -0.375f * tag->koVel;
        }
    }
    else
    {
        tag->koVel += 0x444;
        tag->koDisp += tag->koVel;
        if (tag->koDisp > target)
        {
            tag->koDisp = target;
            if (tag->koVel > 0)
                tag->koVel = -0.375f * tag->koVel;
        }
    }
    if (lbl_12_bss_1ACA0.unk1760 == 0 && prev != (tag->koDisp + 0x2000) >> 16)
        lbl_12_bss_1ACA0.flags |= 2;
}

/* KO points over a fighter, no drawing (lbl_12_bss_1ACA0.unk1760): only track whether it is on screen */
static void fn_12_27A40(struct Sprite *sprite)
{
    struct FightTag *tag = &lbl_12_bss_1F648[sprite->userVar];
    Vec pos;
    float x;
    float y;

    fn_1_E678C(mtxa->mtxA);
    mtxa_mult_left(mtxa->mtxB);
    fn_1_E60D0(sprite->userVar, &pos);
    pos.z -= 0.25f;
    g_math_unk15(&pos, &pos, g_current_camera->sub28.view.tanHalfFov);
    x = (int)(-50.0f + (50.0f + pos.x));
    y = (int)(-8.0f + (4.0f + pos.y));
    if (fabs(pos.x - 320.0f) > 288.0 || fabs(pos.y - 240.0f) > 208.0)
        tag->flags &= ~2;
    else
        tag->flags |= 3;
}

/* Texture rect of digit d in the 5x2 KO digit sheet (no wide-mode inset). */
#define TAG_DIGIT_UV(req, d) \
    do { \
        (req).u1 = 0.2f * ((d) % 5); \
        (req).v1 = 0.5f * ((d) / 5); \
        (req).u2 = 0.2f + (req).u1; \
        (req).v2 = 0.5f + (req).v1; \
    } while (0)

/* KO points over a fighter: draw the rolling KO counter digits and the "KO" label at the
 * fighter's projected screen position (and track whether it is on screen). */
static void fn_12_27BA4(struct Sprite *sprite)
{
    int ko;
    int d;
    Vec pos;
    float x;
    float y;
    float digitX;
    float frac;
    int carry;
    struct FightTag *tag;
    struct SpriteDrawRequest req;

    tag = &lbl_12_bss_1F648[sprite->userVar];
    ko = tag->koDisp;
    fn_1_E678C(mtxa->mtxA);
    mtxa_mult_left(mtxa->mtxB);
    fn_1_E60D0(sprite->userVar, &pos);
    pos.z -= 0.25f;
    g_math_unk15(&pos, &pos, g_current_camera->sub28.view.tanHalfFov);
    x = (int)(-50.0f + (50.0f + pos.x));
    y = (int)(-8.0f + (4.0f + pos.y));
    if (fabs(pos.x - 320.0f) > 288.0 || fabs(pos.y - 240.0f) > 208.0)
        tag->flags &= ~2;
    else
        tag->flags |= 3;
    digitX = 40.0f + x;
    req.bmpId = 0x130C;
    req.z = 0.1f;
    req.opacity = 1.0f;
    req.unk30 = 2;
    req.flags = 5;
    req.mulColor = 0xFFFFFF;
    req.addColor = 0;
    req.scaleX = 0.2f;
    req.rotation = 0;
    frac = (ko & 0xFFFF) / 65536.0f;
    ko >>= 16;
    carry = 0;
    d = ko % 10;
    ko = ko / 10;
    TAG_DIGIT_UV(req, d);
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
        TAG_DIGIT_UV(req, d);
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
            TAG_DIGIT_UV(req, d);
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
                TAG_DIGIT_UV(req, d);
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
                TAG_DIGIT_UV(req, d);
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
}

/* Win bar over a fighter: pop in the stars of the rounds won one by one, star springs */
static void fn_12_2837C(s8 *status, struct Sprite *sprite)
{
    struct FightTag *tag;
    struct FightPlayer *player;
    int n;
    int i;
    struct FightTagWin *win;

    tag = &lbl_12_bss_1F648[sprite->userVar];
    if (!(tag->flags & 1))
        n = 0;
    else
    {
        player = &lbl_12_bss_1ACA0.players[sprite->userVar];
        if (tag->winCount < player->wins)
        {
            tag->winTimer++;
            n = (player->wins < tag->winTimer >> 4) ? player->wins : tag->winTimer >> 4;
        }
        else
            n = player->wins;
    }
    if (tag->winCount != n)
    {
        tag->winCount = (n > 5) ? 5 : n;
        win = tag->wins;
        for (i = tag->winCount; i > 0; i--, win++)
        {
            if (0.0f == win->scaleTarget)
            {
                win->scaleTarget = 1.5f;
                win->timer = 15;
                win->rotVel += 8192.0f * (RAND_FLOAT() - 0.5f);
                win->glowTarget = 1.0f;
            }
        }
    }
    win = tag->wins;
    for (i = 5; i > 0; i--, win++)
    {
        win->rotVel *= 0.98f;
        win->rotVel += -0.025f * win->rot;
        win->rot += win->rotVel;
        if (win->timer != 0)
        {
            win->timer--;
            if (win->timer == 0)
                win->scaleTarget = 1.0f;
        }
        win->scaleVel *= 0.9f;
        win->scaleVel += 0.1f * (win->scaleTarget - win->scale);
        win->scale += win->scaleVel;
        win->glow += 0.05f * (win->glowTarget - win->glow);
    }
}

/* Win bar over a fighter: the bar, the empty slots and the win stars at the fighter's projected
 * screen position */
static void fn_12_285E0(struct Sprite *sprite)
{
    int spacing;
    int n;
    struct FightTag *tag;
    int i;
    struct FightTagWin *win;
    Vec pos;
    float x;
    float y;
    float px;
    float py;
    float u;
    struct SpriteDrawRequest req;

    tag = &lbl_12_bss_1F648[sprite->userVar];
    n = lbl_12_bss_1ACA0.winsNeeded;
    fn_1_E678C(mtxa->mtxA);
    mtxa_mult_left(mtxa->mtxB);
    fn_1_E60D0(sprite->userVar, &pos);
    pos.z -= 0.25f;
    g_math_unk15(&pos, &pos, g_current_camera->sub28.view.tanHalfFov);
    x = (int)(pos.x + sprite->x);
    y = (int)(pos.y + sprite->y);
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
    win = tag->wins;
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
}

static void fn_12_28C88(s8 *status, struct Sprite *sprite);
static void fn_12_28CBC(s8 *status, struct Sprite *sprite);
static void fn_12_28CE0(struct Sprite *sprite);
static void fn_12_28E30(s8 *status, struct Sprite *sprite);
static void fn_12_29140(s8 *status, struct Sprite *sprite);

/* Create the round timer at (x, y): frame, swinging ball, digits and the clock face */
void fn_12_28A6C(float x, float y)
{
    struct Sprite *sprite;

    lbl_12_bss_1F918[0] = x;
    lbl_12_bss_1F918[1] = y;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->textAlign = ALIGN_CT;
        sprite->depth = 0.15f;
        sprite->bmpId = 0x50F;
        sprite->mainFunc = fn_12_29140;
        sprintf(sprite->text, "fgt_time_waku");
        sprite->widescreenArg = 0x60;
        sprite->flags |= 0x01000000;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = 7.0f + y;
        sprite->textAlign = ALIGN_CT;
        sprite->depth = 0.12f;
        sprite->bmpId = 0x50C;
        sprite->userVar = -0x40;   /* swing angle */
        sprite->unk50 = 0x88;      /* swing speed */
        sprite->unk54 = 0x4000;
        sprite->mainFunc = fn_12_28E30;
        sprintf(sprite->text, "fgt_time_ball");
        sprite->widescreenArg = 0x60;
        sprite->flags |= 0x01000000;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = x;
        sprite->y = 27.0f + y;
        sprite->fontId = 0x5E;     /* seconds font */
        sprite->userVar = 0x5F;    /* hundredths font */
        sprite->mainFunc = fn_12_28C88;
        sprite->drawFunc = fn_12_28CE0;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = x;
        sprite->y = y;
        sprite->textAlign = ALIGN_CT;
        sprite->bmpId = 0x50B;
        sprite->mainFunc = fn_12_28CBC;
        sprintf(sprite->text, "fgt_time_asc");
        sprite->widescreenArg = 0x60;
        sprite->flags |= 0x01000000;
    }
}

/* timer digits: follow the timer position */
static void fn_12_28C88(s8 *status, struct Sprite *sprite)
{
    sprite->x = lbl_12_bss_1F918[0];
    sprite->y = 27.0f + lbl_12_bss_1F918[1];
}

/* timer clock face: follow the timer position */
static void fn_12_28CBC(s8 *status, struct Sprite *sprite)
{
    sprite->x = lbl_12_bss_1F918[0];
    sprite->y = lbl_12_bss_1F918[1];
}

/* timer digits: seconds left and hundredths */
static void fn_12_28CE0(struct Sprite *sprite)
{
    set_ui_widescreen_scale_mtx(0x60);
    textdraw_reset();
    textdraw_set_spacing(1.0f, 0.0f);
    textdraw_set_depth(sprite->depth);
    textdraw_set_alignment(ALIGN_CT);
    textdraw_set_font(sprite->fontId);
    textdraw_set_pos(sprite->x, sprite->y);
    textdraw_printf("%03d\n", mode_info.timerCurr / 60);
    textdraw_set_font(sprite->userVar);
    textdraw_set_pos(sprite->x - 5.0f, 40.0f + sprite->y);
    textdraw_printf(":%02d\n", (int)(100.0f * ((mode_info.timerCurr % 60) / 60.0f)));
    textdraw_reset();
    reset_ui_widescreen_scale_mtx();
}

/* timer ball: swings like a pendulum, faster in the last 10 seconds; when time is up it spins and
 * turns red, in the last 10 seconds it flashes red */
static void fn_12_28E30(s8 *status, struct Sprite *sprite)
{
    float f;

    sprite->x = 7.0f + lbl_12_bss_1F918[0];
    sprite->y = lbl_12_bss_1F918[1];
    if (mode_info.timerCurr == 0 || (mode_info.flags & INFO_FLAG_05))
        sprite->userVar += 0x20;
    else if (mode_info.timerCurr < 600)
        sprite->userVar -= 4;
    sprite->userVar = (sprite->userVar < 0) ? sprite->userVar : 0;
    if (mode_info.timerCurr == 0 || (mode_info.flags & INFO_FLAG_05) || !(mode_info.flags & INFO_FLAG_TIMER_PAUSED))
        sprite->rotation += sprite->userVar;
    if (mode_info.timerCurr == 0 && !(mode_info.flags & INFO_FLAG_TIMER_PAUSED))
    {
        sprite->mulR = sprite->mulR + 0.02f * (64 - sprite->mulR);
        sprite->mulG = sprite->mulG + 0.02f * -sprite->mulG;
        sprite->mulB = sprite->mulG;
    }
    else if (mode_info.timerCurr < 600 && !(mode_info.flags & INFO_FLAG_TIMER_PAUSED))
    {
        sprite->unk54 += (sprite->unk54 > 0x4000) ? sprite->unk50 * 6 : sprite->unk50;
        sprite->unk54 = (sprite->unk54 < 0) ? 0 : (sprite->unk54 > 0x8000) ? 0x8000 : sprite->unk54;
        f = 1.0f - math_sin(sprite->unk54);
        sprite->mulG = 255.0f - 192.0f * f;
        sprite->mulB = sprite->mulG;
        if (sprite->unk54 == 0x8000)
        {
            sprite->unk54 = 0;
            sprite->unk50 += 0x44;
        }
    }
}

/* timer frame: slide in from the top, out while the timer is paused */
static void fn_12_29140(s8 *status, struct Sprite *sprite)
{
    if (mode_info.flags & INFO_FLAG_TIMER_PAUSED)
    {
        if (lbl_12_bss_1F918[1] > -124.0f)
        {
            lbl_12_bss_1F918[1] -= 4.2666669f;
            if (lbl_12_bss_1F918[1] < -124.0f)
                lbl_12_bss_1F918[1] = -124.0f;
        }
    }
    else if (lbl_12_bss_1F918[1] < 4.0f)
    {
        lbl_12_bss_1F918[1] += 8.5333338f;
        if (lbl_12_bss_1F918[1] > 4.0f)
            lbl_12_bss_1F918[1] = 4.0f;
    }
    sprite->x = lbl_12_bss_1F918[0];
    sprite->y = lbl_12_bss_1F918[1];
}

static void fn_12_293D0(s8 *status, struct Sprite *sprite);
static void fn_12_29700(struct Sprite *sprite);
static void fn_12_29810(struct FightBlurBand *band);

/* Reset the blurred screen bands */
void fn_12_29238(void)
{
    memset(&lbl_12_bss_1F920, 0, sizeof(lbl_12_bss_1F920));
    lbl_12_bss_1F920.color.r = 384.0f;
    lbl_12_bss_1F920.color.g = 384.0f;
    lbl_12_bss_1F920.color.b = 384.0f;
}

/* Show the blurred screen bands fading to the given konst (RGBA) and tev (RGB) colours */
void fn_12_292B0(int unused, int arg, float *kcolor, float *color)
{
    struct Sprite *sprite;

    lbl_12_bss_1F920.unk0 = arg;
    lbl_12_bss_1F920.kcolorTarget = *(struct FightColorF *)kcolor;
    lbl_12_bss_1F920.colorTarget = *(struct FightColor3F *)color;
    if (lbl_12_bss_1F920.active)
    {
        lbl_12_bss_1F920.shown = 1;
        return;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->depth = 0.25f;
        sprite->mainFunc = fn_12_293D0;
        sprite->drawFunc = fn_12_29700;
        lbl_12_bss_1F920.active = 1;
        lbl_12_bss_1F920.shown = 1;
    }
}

/* Fade the blurred screen bands out (clear: immediately) */
void fn_12_2939C(int clear)
{
    lbl_12_bss_1F920.shown = 0;
    if (clear)
        lbl_12_bss_1F920.kcolor.a = 0.0f;
}

/* blurred screen bands: ease the colours towards the targets, or out */
static void fn_12_293D0(s8 *status, struct Sprite *sprite)
{
    if (lbl_12_bss_1F920.shown)
    {
        lbl_12_bss_1F920.kcolor.r += 0.1f * (lbl_12_bss_1F920.kcolorTarget.r - lbl_12_bss_1F920.kcolor.r);
        lbl_12_bss_1F920.kcolor.g += 0.1f * (lbl_12_bss_1F920.kcolorTarget.g - lbl_12_bss_1F920.kcolor.g);
        lbl_12_bss_1F920.kcolor.b += 0.1f * (lbl_12_bss_1F920.kcolorTarget.b - lbl_12_bss_1F920.kcolor.b);
        lbl_12_bss_1F920.kcolor.a += 0.2f * (lbl_12_bss_1F920.kcolorTarget.a - lbl_12_bss_1F920.kcolor.a);
        lbl_12_bss_1F920.color.r += 0.08f * (lbl_12_bss_1F920.colorTarget.r - lbl_12_bss_1F920.color.r);
        lbl_12_bss_1F920.color.g += 0.081f * (lbl_12_bss_1F920.colorTarget.g - lbl_12_bss_1F920.color.g);
        lbl_12_bss_1F920.color.b += 0.082f * (lbl_12_bss_1F920.colorTarget.b - lbl_12_bss_1F920.color.b);
    }
    else
    {
        lbl_12_bss_1F920.kcolor.r += 0.1f * (255.0f - lbl_12_bss_1F920.kcolor.r);
        lbl_12_bss_1F920.kcolor.g += 0.1f * (255.0f - lbl_12_bss_1F920.kcolor.g);
        lbl_12_bss_1F920.kcolor.b += 0.1f * (255.0f - lbl_12_bss_1F920.kcolor.b);
        lbl_12_bss_1F920.kcolor.a += 0.04f * (-1.0f - lbl_12_bss_1F920.kcolor.a);
        if (lbl_12_bss_1F920.kcolor.a <= 0.0f)
        {
            lbl_12_bss_1F920.active = 0;
            *status = 0;
        }
        lbl_12_bss_1F920.color.r += 0.08f * (-1.0f - lbl_12_bss_1F920.color.r);
        lbl_12_bss_1F920.color.g += 0.081f * (-1.0f - lbl_12_bss_1F920.color.g);
        lbl_12_bss_1F920.color.b += 0.082f * (-1.0f - lbl_12_bss_1F920.color.b);
    }
}

/* blurred screen bands: two bands at the top and two at the bottom of the screen */
static void fn_12_29700(struct Sprite *sprite)
{
    struct FightBlurBand band;

    if (lbl_12_bss_1F920.kcolor.a <= 0.0f)
        return;
    band.x0 = 0.0f;
    band.y0 = 0.0f;
    band.x1 = 640.0f;
    band.y1 = 24.0f;
    band.z = -sprite->depth;
    band.kcolor = lbl_12_bss_1F920.kcolor;
    band.color = lbl_12_bss_1F920.color;
    fn_12_29810(&band);
    band.y0 = 28.0f;
    band.y1 = 32.0f;
    fn_12_29810(&band);
    band.y0 = 448.0f;
    band.y1 = 452.0f;
    fn_12_29810(&band);
    band.y0 = 456.0f;
    band.y1 = 480.0f;
    fn_12_29810(&band);
    g_some_sprite_reset_gx_state_func();
}

/* Draw one band: copy that part of the EFB at half size and draw it back stretched, blurred
 * horizontally by three taps (-3.5, 0, +3.5 texels) and tinted */
static void fn_12_29810(struct FightBlurBand *band)
{
    GXColor c;
    GXColor k;
    GXTexObj tex;
    float fw;
    float fh;
    float x0;
    float y0;
    float x1;
    float y1;
    int left;
    int top;
    int fbWidth;
    int right;
    int bottom;
    int efbHeight;
    int halfW;
    int halfH;

    if (g_some_other_flags & 8)
        return;
    fight9_f32_to_u8(&band->kcolor.r, &c.r);
    fight9_f32_to_u8(&band->kcolor.g, &c.g);
    fight9_f32_to_u8(&band->kcolor.b, &c.b);
    fight9_f32_to_u8(&band->kcolor.a, &c.a);
    GXSetTevKColor_cached(0, c);
    fight9_f32_to_u8(&band->color.r, &c.r);
    fight9_f32_to_u8(&band->color.g, &c.g);
    fight9_f32_to_u8(&band->color.b, &c.b);
    GXSetTevColor(2, c);
    x0 = band->x0;
    y0 = band->y0;
    x1 = band->x1;
    y1 = band->y1;
    fbWidth = current_render_mode->fbWidth;
    efbHeight = current_render_mode->efbHeight;
    fw = fbWidth;
    fh = efbHeight;
    left = 0.0015625f * (x0 * fw);
    top = 0.0020833334f * (y0 * fh);
    left = (left + 3) & ~3;
    top = (top + 3) & ~3;
    if (left < 0)
        left = 0;
    if (top < 0)
        top = 0;
    right = 0.0015625f * (x1 * fw);
    bottom = 0.0020833334f * (y1 * fh);
    right = (right + 4) & ~3;
    bottom = (bottom + 4) & ~3;
    if (right > fbWidth)
        right = fbWidth;
    if (bottom > efbHeight)
        bottom = efbHeight;
    right -= left;
    bottom -= top;
    halfW = right >> 1;
    halfH = bottom >> 1;
    GXSetTexCopySrc_cached(left, top, right, bottom);
    GXSetTexCopyDst_cached(halfW, halfH, 0x28, 1);
    GXCopyTex(lbl_801988E0, 0);
    GXInitTexObj(&tex, lbl_801988E0, halfW, halfH, 1, 0, 0, 0);
    GXInvalidateTexAll();
    GXLoadTexObj_cached(&tex, 0);
    g_something_with_GX_vtx_desc(0x2200);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 7, 0);
    GXSetCullMode_cached(0);
    k.r = 0x55;
    k.g = 0x55;
    k.b = 0x55;
#ifndef __MWERKS__
    k.a = 0;  /* left unset in the original */
#endif
    GXSetTevKColor_cached(1, k);
    mtxa_from_translate_xyz(3.5f / right, 0.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    mtxa_from_translate_xyz(-3.5f / right, 0.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);

    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevKColorSel_cached(0, 0xD);
    GXSetTevColorIn_cached(0, 0xF, 8, 0xE, 0xF);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 4);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(0);

    GXSetTexCoordGen2_cached(1, 1, 4, 0x1E, 0, 0x7D);
    g_GXSetTevOrder_wrapper(1, 1, 0, 0xFF);
    GXSetTevKColorSel_cached(1, 0xD);
    GXSetTevColorIn_cached(1, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(1, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(1, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(1);

    GXSetTexCoordGen2_cached(2, 1, 4, 0x21, 0, 0x7D);
    g_GXSetTevOrder_wrapper(2, 2, 0, 0xFF);
    GXSetTevKColorSel_cached(2, 0xD);
    GXSetTevColorIn_cached(2, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(2, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(2, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(2);

    g_GXSetTevOrder_wrapper(3, 0xFF, 0xFF, 0xFF);
    GXSetTevKColorSel_cached(3, 0xC);
    GXSetTevKAlphaSel_cached(3, 0x1C);
    GXSetTevColorIn_cached(3, 0xF, 0xE, 0, 4);
    GXSetTevColorOp_cached(3, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(3, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(3, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(3);

    GXSetNumTevStages_cached(4);
    GXSetNumTexGens_cached(3);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(0);
    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();
    GXBegin(0x80, 0, 4);
    GXPosition3f32(band->x1, band->y0, band->z);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(band->x0, band->y0, band->z);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(band->x0, band->y1, band->z);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(band->x1, band->y1, band->z);
    GXTexCoord2f32(1.0f, 1.0f);
    GXEnd();
}
