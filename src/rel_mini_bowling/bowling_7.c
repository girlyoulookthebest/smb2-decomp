/* Message sprites: shot banners, pin counts, results and the countdown. */
#include "types.h"
#include "ape_internal.h"
#include "mathutil.h"
#include "mode.h"
#include "sprite.h"

extern int locale;

int sprintf(char *buf, const char *format, ...);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
float fn_1_B2638(char *text);
void call_SoundReqID_arg_0(int soundId);

void fn_5_14D4C(s8 *status, struct Sprite *sprite);
void fn_5_14F7C(s8 *status, struct Sprite *sprite);
void fn_5_15248(s8 *status, struct Sprite *sprite);
void fn_5_15570(s8 *status, struct Sprite *sprite);
void result_banner_main(s8 *status, struct Sprite *sprite);
void fn_5_15858(s8 *status, struct Sprite *sprite);
void fn_5_15A64(s8 *status, struct Sprite *sprite);
void fn_5_15C0C(s8 *status, struct Sprite *sprite);
void fn_5_15F08(s8 *status, struct Sprite *sprite);

void fn_5_14B74(int counter, int playerNum)
{
    char *prefixes[] = {"", "SP", "J", "J", "G", ""};
    char *suffixes[] = {"P START", " START", " COMMENCE", " INICIA", " INIZIO", "P START"};
    char *startMsgs[] = {"START", "START", "COMMENCE", "EMPEZAR", "INIZIO", "START"};
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->depth = 0.05f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0x8C;
        sprite->mulB = 0;
        sprite->tag = 0x6B;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = counter;
        sprite->userVar = counter;
        sprite->scaleX = 0.85f;
        sprite->scaleY = 0.85f;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = fn_5_14D4C;
        if (playerNum > 0)
            sprintf(sprite->text, "%s%d%s", prefixes[locale], playerNum, suffixes[locale]);
        else
            strcpy(sprite->text, startMsgs[locale]);
    }
}

void fn_5_14D4C(s8 *status, struct Sprite *sprite)
{
    int total = sprite->userVar;
    int elapsed = total - sprite->counter;
    float t = math_sin((s16)((float)(elapsed << 15) / (float)total));

    sprite->opacity = t;
    if (elapsed > total / 2)
        sprite->scaleX = 0.85 * (1.5 - 0.5 * t);
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void fn_5_14E3C(int counter, char *text)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        float width;
        float maxWidth;

        sprite->x = 320.0f;
        sprite->y = 120.0f;
        sprite->depth = 0.05f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->tag = 0x6B;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = counter;
        sprite->userVar = counter;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = fn_5_14F7C;
        strcpy(sprite->text, text);
        width = 0.85 * fn_1_B2638(text);
        if (modeCtrl.wideMode != 0)
            maxWidth = 447.0f;
        else
            maxWidth = 596.0f;
        if (width > maxWidth)
            sprite->scaleX = 0.85 * maxWidth / width;
        else
            sprite->scaleX = 0.85f;
        sprite->scaleY = 0.85f;
    }
}

void fn_5_14F7C(s8 *status, struct Sprite *sprite)
{
    int elapsed = sprite->userVar - sprite->counter;

    if (elapsed < 30)
    {
        sprite->opacity = 0.033333333333333333 * elapsed;
        sprite->y = 120 - (30 - elapsed) * 6;
    }
    else if (elapsed < 60)
    {
        sprite->opacity = 1.0f;
        sprite->y = 120.0f - 32.0f * math_sin((elapsed - 30) * 0x444);
    }
    else if (elapsed < 90)
    {
        sprite->opacity = 1.0f;
        sprite->y = 120.0f - 8.0f * math_sin((elapsed - 60) * 0x444);
    }
    else if (sprite->counter <= 15)
        sprite->opacity = 0.066666666666666666 * sprite->counter;
    else
        sprite->opacity = 1.0f;
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void fn_5_15108(int counter, char *text)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        float width;
        float maxWidth;

        sprite->x = 320.0f;
        sprite->y = 120.0f;
        sprite->depth = 0.05f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->tag = 0x6B;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = counter;
        sprite->userVar = counter;
        sprite->flags |= 0x01000000;
        sprite->widescreenArg = 0x140;
        sprite->mainFunc = fn_5_15248;
        strcpy(sprite->text, text);
        width = 0.85 * fn_1_B2638(text);
        if (modeCtrl.wideMode != 0)
            maxWidth = 447.0f;
        else
            maxWidth = 596.0f;
        if (width > maxWidth)
            sprite->scaleX = 0.85 * maxWidth / width;
        else
            sprite->scaleX = 0.85f;
        sprite->scaleY = 0.85f;
    }
}

void fn_5_15248(s8 *status, struct Sprite *sprite)
{
    int elapsed = sprite->userVar - sprite->counter;

    if (elapsed < 30)
    {
        sprite->opacity = 0.033333333333333333 * elapsed;
        sprite->y = 120 - (30 - elapsed) * 6;
    }
    else if (elapsed < 60)
    {
        sprite->opacity = 1.0f;
        sprite->y = 120.0f - 32.0f * math_sin((elapsed - 30) * 0x444);
    }
    else if (elapsed < 90)
    {
        sprite->opacity = 1.0f;
        sprite->y = 120.0f - 8.0f * math_sin((elapsed - 60) * 0x444);
    }
    else if (sprite->counter <= 15)
        sprite->opacity = 0.066666666666666666 * sprite->counter;
    else
        sprite->opacity = 1.0f;
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void fn_5_153D4(int counter, int pins)
{
    char *singular[] = {"1 PIN", "1 PIN", "1 QUILLE", "1 BOLO", "1 BIRILLO", "1 PIN"};
    char *plural[] = {"PINS", "PINS", "QUILLES", "BOLOS", "BIRILLI", "PINS"};
    struct Sprite *sprite;

    if (pins > 0)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->x = 320.0f;
            sprite->y = 120.0f;
            sprite->depth = 0.05f;
            sprite->mulR = 0xFF;
            sprite->mulG = 0x8C;
            sprite->mulB = 0;
            sprite->tag = 0x6B;
            sprite->fontId = 9;
            sprite->textAlign = 4;
            sprite->counter = counter;
            sprite->userVar = counter;
            sprite->flags |= 0x01000000;
            sprite->widescreenArg = 0x140;
            sprite->mainFunc = fn_5_15570;
            if (pins == 1)
                strcpy(sprite->text, singular[locale]);
            else
            {
                sprintf(sprite->text, "%d ", pins);
                strcat(sprite->text, plural[locale]);
            }
        }
    }
}

void fn_5_15570(s8 *status, struct Sprite *sprite)
{
    int elapsed = sprite->userVar - sprite->counter;

    if (elapsed < 30)
    {
        sprite->opacity = 0.033333333333333333 * elapsed;
        sprite->y = 120 - (30 - elapsed) * 6;
    }
    else if (elapsed < 60)
    {
        sprite->opacity = 1.0f;
        sprite->y = 120.0f - 32.0f * math_sin((elapsed - 30) * 0x444);
    }
    else if (elapsed < 90)
    {
        sprite->opacity = 1.0f;
        sprite->y = 120.0f - 8.0f * math_sin((elapsed - 60) * 0x444);
    }
    else if (sprite->counter <= 15)
        sprite->opacity = 0.066666666666666666 * sprite->counter;
    else
        sprite->opacity = 1.0f;
    sprite->scaleX = 0.85f;
    sprite->scaleY = sprite->scaleX;
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

/* Unreferenced: the linker strips these, but their data stays in the REL. */
void show_result_banner(int counter)
{
    char *msgs[] = {"RESULT", "ERGEBNIS", "R\xD9SULTAT", "RESULTADO", "RISULTATO", "RESULT"};
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        float width;
        float maxWidth;

        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0;
        sprite->mulB = 0x20;
        sprite->tag = 0x6B;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = counter;
        sprite->userVar = counter;
        sprite->depth = 0.1f;
        sprite->mainFunc = result_banner_main;
        strcpy(sprite->text, msgs[locale]);
        width = fn_1_B2638(sprite->text);
        if (modeCtrl.wideMode != 0)
            maxWidth = 447.0f;
        else
            maxWidth = 596.0f;
        if (width > maxWidth)
            sprite->scaleX = maxWidth / width;
        else
            sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
    }
}

void result_banner_main(s8 *status, struct Sprite *sprite)
{
    int elapsed = sprite->userVar - sprite->counter;

    if (sprite->counter < 60)
        sprite->opacity = 0.016666666666666666 * sprite->counter;
    else if (elapsed < 30)
        sprite->opacity = 0.033333333333333333 * elapsed;
    else
        sprite->opacity = 1.0f;
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void fn_5_1570C(int counter)
{
    char *msgs[] = {"GAME OVER", "GAME OVER", "PARTIE TERMIN\xC9""E", "FIN DEL JUEGO", "FINE PARTITA", "GAME OVER"};
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        float width;
        float maxWidth;

        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0;
        sprite->mulB = 0x20;
        sprite->tag = 0x6B;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = counter;
        sprite->userVar = counter;
        sprite->depth = 0.1f;
        sprite->mainFunc = fn_5_15858;
        strcpy(sprite->text, msgs[locale]);
        width = fn_1_B2638(sprite->text);
        if (modeCtrl.wideMode != 0)
            maxWidth = 447.0f;
        else
            maxWidth = 596.0f;
        if (width > maxWidth)
            sprite->scaleX = maxWidth / width;
        else
            sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
    }
}

void fn_5_15858(s8 *status, struct Sprite *sprite)
{
    int elapsed = sprite->userVar - sprite->counter;

    if (sprite->counter < 60)
        sprite->opacity = 0.016666666666666666 * sprite->counter;
    else if (elapsed < 30)
        sprite->opacity = 0.033333333333333333 * elapsed;
    else
        sprite->opacity = 1.0f;
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void fn_5_15918(int counter)
{
    char *msgs[] = {"ALL CLEAR", "ALLES ERL^", "TOUT REUSSI", "PLENO", "VINTO TUTTO[", "ALL CLEAR"};
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        float width;
        float maxWidth;

        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->tag = 0x6B;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = counter;
        sprite->userVar = counter;
        sprite->depth = 0.1f;
        sprite->mainFunc = fn_5_15A64;
        strcpy(sprite->text, msgs[locale]);
        width = fn_1_B2638(sprite->text);
        if (modeCtrl.wideMode != 0)
            maxWidth = 447.0f;
        else
            maxWidth = 596.0f;
        if (width > maxWidth)
            sprite->scaleX = maxWidth / width;
        else
            sprite->scaleX = 1.0f;
        sprite->scaleY = 1.0f;
    }
}

void fn_5_15A64(s8 *status, struct Sprite *sprite)
{
    int elapsed = sprite->userVar - sprite->counter;

    if (sprite->counter < 60)
        sprite->opacity = 0.016666666666666666 * sprite->counter;
    else if (elapsed < 30)
        sprite->opacity = 0.033333333333333333 * elapsed;
    else
        sprite->opacity = 1.0f;
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void fn_5_15B24(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->x = 552.0f;
        sprite->y = 240.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0xFF;
        sprite->tag = 0x6B;
        sprite->fontId = 9;
        sprite->textAlign = 4;
        sprite->counter = lbl_80198B38 * 5;
        sprite->userVar = lbl_80198B38 * 5;
        sprite->scaleX = 0.5f;
        sprite->scaleY = 0.5f;
        sprite->depth = 0.1f;
        sprite->opacity = 1.0f;
        sprite->mainFunc = fn_5_15C0C;
        strcpy(sprite->text, "5");
        call_SoundReqID_arg_0(6);
    }
}

void fn_5_15C0C(s8 *status, struct Sprite *sprite)
{
    if (sprite->counter == lbl_80198B38)
    {
        sprite->mulR = 0xFF;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->scaleX = 1.3f;
        sprite->scaleY = 1.3f;
        sprite->opacity = 1.0f;
        strcpy(sprite->text, "1");
        call_SoundReqID_arg_0(6);
    }
    else if (sprite->counter == lbl_80198B38 * 2)
    {
        sprite->mulR = 0xFF;
        sprite->mulG = 0x7F;
        sprite->mulB = 0;
        sprite->scaleX = 1.1f;
        sprite->scaleY = 1.1f;
        sprite->opacity = 1.0f;
        strcpy(sprite->text, "2");
        call_SoundReqID_arg_0(6);
    }
    else if (sprite->counter == lbl_80198B38 * 3)
    {
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->scaleX = 0.9f;
        sprite->scaleY = 0.9f;
        sprite->opacity = 1.0f;
        strcpy(sprite->text, "3");
        call_SoundReqID_arg_0(6);
    }
    else if (sprite->counter == lbl_80198B38 * 4)
    {
        sprite->mulR = 0;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->scaleX = 0.7f;
        sprite->scaleY = 0.7f;
        sprite->opacity = 1.0f;
        strcpy(sprite->text, "4");
        call_SoundReqID_arg_0(6);
    }
    sprite->scaleX += 0.01;
    sprite->scaleY += 0.01;
    sprite->opacity -= 0.016666666666666666;
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}

void fn_5_15E3C(int counter)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 0.0f;
        sprite->y = 0.0f;
        sprite->depth = 0.3f;
        sprite->opacity = 0.5f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->tag = 0x6B;
        sprite->textAlign = 0;
        sprite->counter = counter;
        sprite->bmpId = 0x4B;
        sprite->scaleX = 80.0f;
        sprite->scaleY = 60.0f;
        sprite->flags |= 0x1000;
        sprite->mainFunc = fn_5_15F08;
    }
}

void fn_5_15F08(s8 *status, struct Sprite *sprite)
{
    sprite->counter--;
    if (sprite->counter <= 0)
        *status = 0;
}
