/* Replay viewer HUD: stage name banner, score, time and the blinking loading message. */
#include "types.h"
#include "bitmap.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"
#include "recplay.h"
#include "sprite.h"
#include "textdraw.h"

struct Color8
{
    u8 r, g, b, a;
};

extern s32 locale;
extern u32 g_some_other_flags;

int g_get_replay_time(void);
float textdraw_get_pixel_width_of_string_as_f32(const char *s);
void read_stage_name_from_dvd(int stageId, char *buf, int size);
struct Sprite *get_sprite_with_unique_id(int id);
void fn_1_AE99C(struct Sprite *sprite);

void create_replay_stage_name_sprites(float x, float y);
void fn_20_2218(float x, float y);
void fn_20_2438(float x, float y);
void fn_20_26A4(float x, float y);
void fn_20_1AC8(s8 *status, struct Sprite *sprite);
void fn_20_1BF4(struct Sprite *sprite);
void fn_20_2194(s8 *status, struct Sprite *sprite);
void fn_20_29F0(struct Sprite *sprite);
void fn_20_2A5C(s8 *status, struct Sprite *sprite);
void fn_20_2B84(s8 *status, struct Sprite *sprite);

u32 lbl_20_data_278[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};

char *lbl_20_data_4A0[2][6] = {
    {
        "NOW LOADING... PLEASE WAIT...",
        "L\xC4" "DT\x85 BITTE WARTEN\x85",
        "CHARGEMENT EN COURS\x85 UN PEU DE PATIENCE\x85",
        "CARGANDO\x85 POR FAVOR ESPERE",
        "IN CARICAMENTO, ATTENDI\x85",
        "NOW LOADING... PLEASE WAIT...",
    },
    {
        "REPLAY",
        "WIEDERHOLEN",
        "REPLAY",
        "REPETICI\xD3N",
        "REPLAY",
        "REPLAY",
    },
};

struct Color8 lbl_20_data_4D0[] = {
    {0x80, 0x80, 0xFF, 0xFF},
    {0xFF, 0xE0, 0x80, 0xFF},
};

static struct ReplayHeader lbl_20_bss_38;
static char lbl_20_bss_50[0x30];

void create_replay_hud_sprites(void)
{
    fn_1_62AA0(0x11, &lbl_20_bss_38);
    create_replay_stage_name_sprites(16.0f, 432.0f);
    fn_20_2218(16.0f, 24.0f);
    fn_20_2438(16.0f, 48.0f);
    fn_20_26A4(624.0f, 448.0f);
}

void create_replay_stage_name_sprites(float x, float y)
{
    struct Sprite *sprite;
    int isWorld;
    int floor;
    int isEx;
    float offset;

    isWorld = lbl_20_bss_38.flags & 0x1000;
    read_stage_name_from_dvd(lbl_20_bss_38.stageId, lbl_20_bss_50, 0x30);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->tag = 0x70;
        sprite->x = x;
        sprite->y = y;
        sprite->bmpId = 0x505;
        sprite->depth = 0.12f;
        sprite->mainFunc = fn_20_1AC8;
        sprite->drawFunc = fn_20_1BF4;
        sprite->counter = 0x3C;
        sprite->unk58 = 0.0f;
        sprite->unk5C = 0.0f;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "game_kiwaku_world");
    }
    floor = lbl_20_bss_38.floor;
    isEx = 0;
    if (((lbl_20_bss_38.flags & 0x20) && !(lbl_20_bss_38.flags & 0x40)) || (lbl_20_bss_38.flags & 0x800))
        isEx = 1;
    offset = 34.0f;
    if (modeCtrl.difficulty == 0)
        offset += 4.0f;
    if (isEx)
    {
        offset += 8.0f;
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 3.0f + (x + offset - 8.0f);
            sprite->y = 3.0f + (8.0f + y);
            sprite->fontId = 3;
            sprite->depth = 0.11f;
            sprite->unk58 = 0.45f;
            sprite->unk5C = 0.0f;
            sprite->opacity = 0.0f;
            sprite->mulR = 0;
            sprite->mulG = 0;
            sprite->mulB = 0;
            sprite->scaleX = 0.5f;
            sprite->scaleY = 0.7f;
            sprite->flags |= 0x21000000;
            sprite->unk20 = 0.5f;
            sprite->unk24 = 0;
            sprite->widescreenArg = 0x60;
            sprintf(sprite->text, "EX");
            sprite->mainFunc = fn_20_2194;
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 0x22;
            sprite->x = x + offset - 8.0f;
            sprite->y = 8.0f + y;
            sprite->fontId = 3;
            sprite->unk58 = 1.0f;
            sprite->unk5C = 0.0f;
            sprite->opacity = 0.0f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0xFF;
            sprite->mulB = 0;
            sprite->scaleX = 0.5f;
            sprite->scaleY = 0.7f;
            sprite->flags |= 0x21000000;
            sprite->unk20 = 1.0f;
            sprite->unk24 = 0;
            sprite->widescreenArg = 0x60;
            sprintf(sprite->text, "EX");
            sprite->mainFunc = fn_20_2194;
        }
        textdraw_reset();
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        offset += 5.0f + 0.5f * textdraw_get_pixel_width_of_string_as_f32(sprite->text);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 3.0f + (x + offset);
        sprite->y = 3.0f + (8.0f + y);
        sprite->fontId = 0x60;
        sprite->depth = 0.11f;
        sprite->unk58 = 0.5f;
        sprite->unk5C = 0.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x60;
        if (isWorld)
            sprintf(sprite->text, "%d:%d", lbl_20_bss_38.difficulty + 1, floor + 1);
        else
            sprintf(sprite->text, "%d", floor);
        sprite->mainFunc = fn_20_2194;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x23;
        sprite->x = x + offset;
        sprite->y = 8.0f + y;
        sprite->fontId = 0x60;
        sprite->unk58 = 1.0f;
        sprite->unk5C = 0.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x60;
        if (isWorld)
            sprintf(sprite->text, "%d:%d", lbl_20_bss_38.difficulty + 1, floor + 1);
        else
            sprintf(sprite->text, "%d", floor);
        sprite->mainFunc = fn_20_2194;
        textdraw_reset();
        textdraw_set_font(0x60);
        offset += 5.0f + textdraw_get_pixel_width_of_string_as_f32(sprite->text);
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 3.0f + (x + offset);
        sprite->y = 3.0f + (8.0f + y);
        sprite->fontId = 3;
        sprite->depth = 0.11f;
        sprite->unk58 = 0.45f;
        sprite->unk5C = 0.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->scaleX = 0.7f;
        sprite->scaleY = 0.7f;
        sprite->unk20 = 0.45f;
        sprite->unk24 = 0;
        sprite->flags |= 0xA1000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "%s", lbl_20_bss_50);
        sprite->mainFunc = fn_20_2194;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = x + offset;
        sprite->y = 8.0f + y;
        sprite->fontId = 3;
        sprite->unk58 = 1.0f;
        sprite->unk5C = 0.0f;
        sprite->opacity = 0.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->scaleX = 0.7f;
        sprite->scaleY = 0.7f;
        sprite->unk20 = 1.0f;
        sprite->unk24 = 0;
        sprite->flags |= 0xA1000000;
        sprite->widescreenArg = 0x60;
        sprintf(sprite->text, "%s", lbl_20_bss_50);
        sprite->mainFunc = fn_20_2194;
    }
}

void fn_20_1AC8(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter > 0)
        sprite->counter--;
    sprite->unk58 = -7.0f * ((sprite->counter / 24 + 1) * (sprite->counter / 24 + 1))
                  * math_sin(sprite->counter % 24 * 0x555);
    sprite->unk5C = 1.0f - 0.45f * (sprite->counter / 24.0f);
}

void fn_20_1BF4(struct Sprite *sprite)
{
    struct SpriteDrawRequest req;
    struct SpriteDrawRequest req2;
    struct Sprite *other;
    int isWorld;
    int i;
    float inner;
    float width;

    width = 0.0f;
    isWorld = lbl_20_bss_38.flags & 0x1000;
    textdraw_reset();
    if (modeCtrl.difficulty == 0)
        width += 4.0f;
    other = get_sprite_with_unique_id(0x22);
    if (other != NULL)
    {
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        width += 5.0f + (8.0f + 0.5f * textdraw_get_pixel_width_of_string_as_f32(other->text));
    }
    other = get_sprite_with_unique_id(0x23);
    if (other != NULL)
    {
        textdraw_set_font(0x60);
        width += 5.0f + textdraw_get_pixel_width_of_string_as_f32(other->text);
    }
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    width += 5.0f + 0.7f * textdraw_get_pixel_width_of_string_as_f32(lbl_20_bss_50);
    inner = width - 40.0f;
    width = 0.05f * inner;
    {
        float u1[3] = {0.0f, 0.5f, 0.75f};
        float u2[3] = {0.5f, 0.75f, 1.0f};

        req.bmpId = sprite->bmpId;
        req.y = sprite->y;
        req.z = sprite->depth;
        req.v1 = 0.0f;
        req.v2 = 1.0f;
        req.scaleY = 1.0f;
        req.opacity = 1.0f;
        req.rotation = 0;
        req.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | 0xFFFFFF;
        req.addColor = 0;
        req.unk30 = -1;
        for (i = 0; i < 3; i++)
        {
            req.u1 = u1[i];
            req.u2 = u2[i];
            switch (i)
            {
            case 0:
                req.flags = (sprite->flags & ~0xF) | 5;
                req.scaleX = req.u2 - req.u1;
                req.x = sprite->x + sprite->unk58;
                break;
            case 1:
            {
                float t = -0.05f * sprite->unk58 * (1.0 / (1.0 + width));

                req.flags = (sprite->flags & ~0xF) | 7;
                req.scaleX = (1.0 + width) * ((1.0f + t) * (req.u2 - req.u1));
                req.x = inner + (sprite->x + (float)((bmp_infos[req.bmpId >> 8].tpl->texHeaders[req.bmpId & 0xFF].width / 4) * 3));
                break;
            }
            case 2:
                req.flags = (sprite->flags & ~0xF) | 5;
                req.scaleX = req.u2 - req.u1;
                req.x = inner + (sprite->x + (float)((bmp_infos[req.bmpId >> 8].tpl->texHeaders[req.bmpId & 0xFF].width / 4) * 3));
                break;
            }
            draw_sprite_draw_request(&req);
        }
    }
    req2.u1 = 0.0f;
    req2.v1 = 0.0f;
    req2.u2 = 1.0f;
    req2.v2 = 1.0f;
    req2.scaleX = sprite->unk5C;
    req2.scaleY = sprite->unk5C;
    req2.rotation = 0;
    req2.addColor = 0;
    req2.unk30 = -1;
    req2.flags = (sprite->flags & ~0xF) | 0xA;
    if (isWorld)
    {
        req2.bmpId = 0x512;
    }
    else
    {
        int ids[4] = {0x514, 0x513, 0x515, 0x517};

        req2.bmpId = ids[(modeCtrl.courseFlags & 0x10) ? 3 : modeCtrl.difficulty];
    }
    req2.x = 2.0f + (16.0f + sprite->x + sprite->unk58);
    req2.y = 2.0f + (16.0f + sprite->y);
    req2.z = sprite->depth - 0.01f;
    req2.opacity = 0.5f;
    req2.mulColor = (u8)(255.0f * sprite->opacity) << 24;
    draw_sprite_draw_request(&req2);
    req2.x = 16.0f + sprite->x + sprite->unk58;
    req2.y = 16.0f + sprite->y;
    req2.z = sprite->depth - 0.02f;
    req2.opacity = 1.0f;
    req2.mulColor = ((u8)(255.0f * sprite->opacity) << 24) | 0xFFFFFF;
    draw_sprite_draw_request(&req2);
}

void fn_20_2194(s8 *status, struct Sprite *sprite)
{
    struct Sprite *banner = get_sprite_with_unique_id(0x70);

    if (banner != NULL && banner->counter == 0 && sprite->unk5C < 1.0f)
        sprite->unk5C += 0.05f;
    sprite->opacity = sprite->unk58 * sprite->unk5C;
}

void fn_20_2218(float x, float y)
{
    char buf[0x48];
    struct Sprite *sprite;
    u8 *p;
    int i;
    float width;
    float cursor;

    textdraw_reset();
    textdraw_set_font(4);
    sprintf(buf, "%d", lbl_20_bss_38.score);
    width = textdraw_get_pixel_width_of_string_as_f32(buf);
    if (width > 64.0f)
        cursor = 0.0f;
    else
        cursor = 64.0f - width;
    i = 0;
    for (p = (u8 *)buf; *p != 0; p++)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 0.5f * cursor - width;
            sprite->y = y;
            sprite->userVar = 0;
            sprite->unk50 = i;
            sprite->unk58 = 104.0f + x + cursor;
            sprite->unk5C = 0.0f;
            sprite->unk60 = 0.0f;
            sprite->fontId = 4;
            sprite->textAlign = 1;
            sprite->mainFunc = fn_20_2B84;
            ((u8 *)sprite->text)[0] = *p;
            sprite->text[1] = 0;
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x60;
            cursor += textdraw_get_pixel_width_of_string_as_f32(sprite->text);
            i++;
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->fontId = 4;
        sprite->textAlign = 7;
        sprite->mainFunc = fn_20_2A5C;
        sprintf(sprite->text, "SCORE:");
        sprite->x = -width;
        sprite->y = y;
        sprite->userVar = 0;
        sprite->unk58 = 96.0f + x;
        sprite->unk5C = 0.0f;
        sprite->unk60 = 0.0f;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x60;
    }
}

void fn_20_2438(float x, float y)
{
    char buf[0x80];
    struct Sprite *sprite;
    int i;
    u8 *p;
    float width;
    float cursor;

    textdraw_reset();
    textdraw_set_font(4);
    if (lbl_20_bss_38.flags & 1)
    {
        int time = (s16)g_get_replay_time();

        sprintf(buf, "%5.2f", time / 60.0f);
    }
    else
        sprintf(buf, "--.--");
    width = textdraw_get_pixel_width_of_string_as_f32(buf);
    if (width > 64.0f)
        cursor = 0.0f;
    else
        cursor = 64.0f - width;
    i = 0;
    for (p = (u8 *)buf; *p != 0; p++)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 0.5f * cursor - width - 24.0f;
            sprite->y = y;
            sprite->userVar = 1;
            sprite->unk50 = i;
            sprite->unk58 = 104.0f + x + cursor;
            sprite->unk5C = 0.0f;
            sprite->unk60 = 0.0f;
            sprite->fontId = 4;
            sprite->textAlign = 1;
            sprite->mainFunc = fn_20_2B84;
            ((u8 *)sprite->text)[0] = *p;
            sprite->text[1] = 0;
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x60;
            cursor += textdraw_get_pixel_width_of_string_as_f32(sprite->text);
            i++;
        }
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->fontId = 4;
        sprite->textAlign = 7;
        sprite->mainFunc = fn_20_2A5C;
        sprintf(sprite->text, "TIME:");
        sprite->x = -width - 24.0f;
        sprite->y = y;
        sprite->userVar = 1;
        sprite->unk58 = 96.0f + x;
        sprite->unk5C = 0.0f;
        sprite->unk60 = 0.0f;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x60;
    }
}

void fn_20_26A4(float x, float y)
{
    struct Sprite *sprite;
    int width;

    textdraw_reset();
    textdraw_set_font(4);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->userVar = 0;
        strcpy(sprite->text, lbl_20_data_4A0[sprite->userVar][locale]);
        sprite->x = 896.0f;
        sprite->y = y;
        sprite->unk58 = x;
        width = textdraw_get_pixel_width_of_string_as_f32(sprite->text);
        if ((float)width > 256.0f)
            sprite->scaleX = 256.0f / (float)width;
        else
            sprite->scaleX = 1.0f;
        sprite->fontId = 4;
        sprite->textAlign = 7;
        sprite->drawFunc = fn_20_29F0;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x220;
    }
}

void fn_20_2808(s8 *status, struct Sprite *sprite)
{
    float x = sprite->x;
    float target = sprite->unk58;
    int msg = sprite->userVar;
    int next;
    int width;

    if (x > target)
    {
        x -= 10.666667f;
        if (x < target)
            x = target;
    }
    switch (sub_mode)
    {
    case 0x101:
    case 0x102:
        next = 0;
        break;
    default:
        next = 1;
        break;
    }
    if (msg != next)
    {
        msg = next;
        strcpy(sprite->text, lbl_20_data_4A0[msg][locale]);
        textdraw_reset();
        textdraw_set_font(4);
        width = textdraw_get_pixel_width_of_string_as_f32(sprite->text);
        if ((float)width > 256.0f)
            sprite->scaleX = 256.0f / (float)width;
        else
            sprite->scaleX = 1.0f;
        sprite->scaleY = 0.0f;
    }
    sprite->counter++;
    if (sprite->counter & 0x30)
        sprite->opacity = 1.0f;
    else
        sprite->opacity = 0.0f;
    if (sprite->scaleY < 1.0f)
    {
        sprite->scaleY += 0.06666667f;
        if (sprite->scaleY > 1.0f)
            sprite->scaleY = 1.0f;
    }
    sprite->x = x;
    sprite->unk58 = target;
    sprite->userVar = msg;
}

void fn_20_29F0(struct Sprite *sprite)
{
    void (*drawFunc)(struct Sprite *);

    if (!(g_some_other_flags & 2))
        fn_20_2808(NULL, sprite);
    drawFunc = sprite->drawFunc;
    sprite->drawFunc = NULL;
    fn_1_AE99C(sprite);
    sprite->drawFunc = drawFunc;
}

void fn_20_2A5C(s8 *status, struct Sprite *sprite)
{
    struct Color8 color;
    float x = sprite->x;
    float target = sprite->unk58;
    float velocity = sprite->unk5C;
    float scale = sprite->scaleX;
    float scaleVel = sprite->unk60;

    if (sprite->unk50 != 0)
    {
        color = lbl_20_data_4D0[sprite->userVar];
        if (sprite->mulR > color.r)
            sprite->mulR--;
        if (sprite->mulG > color.g)
            sprite->mulG--;
        if (sprite->mulB > color.b)
            sprite->mulB--;
    }
    velocity += 0.5f;
    velocity *= 0.99f;
    x += velocity;
    if (x > target)
    {
        float impact;

        x = target;
        impact = fabs(velocity) - 2.0f;
        if (impact > 0.0f)
            scaleVel -= 0.025f * impact;
        if (velocity > 0.0f)
            velocity *= -0.6f;
        sprite->unk50 = 1;
    }
    sprite->x = x;
    sprite->unk58 = target;
    sprite->unk5C = velocity;
    sprite->scaleX = scale;
    sprite->unk60 = scaleVel;
}

void fn_20_2B84(s8 *status, struct Sprite *sprite)
{
    struct Color8 color;
    float x = sprite->x;
    float target = sprite->unk58;
    float velocity = sprite->unk5C;
    float scale = sprite->scaleX;
    float scaleVel = sprite->unk60;
    int index = sprite->unk50;
    float t;

    if (sprite->unk50 != 0)
    {
        color = lbl_20_data_4D0[sprite->userVar];
        if (sprite->mulR > color.r)
            sprite->mulR--;
        if (sprite->mulG > color.g)
            sprite->mulG--;
        if (sprite->mulB > color.b)
            sprite->mulB--;
    }
    if (x < target)
    {
        velocity += 0.5f;
        velocity *= 0.99f;
        x += velocity;
        if (x >= target)
        {
            t = 0.2f * (index + 1);
            velocity *= t * (0.4f + 0.6f * (rand() / 32767.0f));
        }
    }
    else
    {
        velocity -= 0.5f;
        velocity *= 0.99f;
        x += velocity;
        if (x < target)
        {
            x = target;
            t = fabs(velocity) - 2.0f;
            if (t > 0.0f)
                scaleVel -= 0.025f * t;
            if (velocity < 0.0f)
                velocity *= -(0.6f + 0.3f * (rand() / 32767.0f));
            sprite->unk50 = 1;
            color = lbl_20_data_4D0[sprite->userVar];
            sprite->mulR = (u8)((u32)(sprite->mulR + color.r) >> 1);
            sprite->mulG = (u8)((u32)(sprite->mulG + color.g) >> 1);
            sprite->mulB = (u8)((u32)(sprite->mulB + color.b) >> 1);
        }
    }
    scaleVel += 0.2f * (1.0f - scale);
    scaleVel *= 0.96f;
    scale += scaleVel;
    if (scale < 0.1f)
    {
        scale = 0.1f;
        if (scaleVel < 0.1f)
            scaleVel *= -0.5f;
    }
    if (fabs(scale - 1.0f) < 0.05f)
        scale = 1.0f;
    sprite->x = x;
    sprite->unk58 = target;
    sprite->unk5C = velocity;
    sprite->scaleX = scale;
    sprite->unk60 = scaleVel;
}
