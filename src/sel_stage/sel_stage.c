#include "types.h"
#include "libc.h"
#include "mode.h"
#include "sprite.h"

extern s16 g_another_stage_id;
extern char current_stage_name[];
extern s32 lbl_1_bss_1744[2];

void fn_4_11A8(s8 *status, struct Sprite *sprite);
void fn_4_1274(s8 *status, struct Sprite *sprite);

u32 lbl_4_data_3C8[] = {
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

void fn_4_1070(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 320.0f;
        sprite->y = 56.0f;
        sprite->fontId = 3;
        sprite->textAlign = 4;
        sprite->flags = 0x40000000;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        strcpy(sprite->text, "SELECT A STAGE");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 40.0f;
        sprite->y = 90.0f;
        sprite->fontId = 3;
        sprite->textAlign = 1;
        sprite->flags = 0x40000000;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->mainFunc = fn_4_11A8;
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->x = 40.0f;
        sprite->y = 128.0f;
        sprite->fontId = 3;
        sprite->textAlign = 1;
        sprite->flags = 0x40000000;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0;
        sprite->mainFunc = fn_4_1274;
    }
}

/* Unreferenced, but its literals are still in .rodata. */
static void unused_literals(struct Sprite *sprite, int a, u32 b)
{
    sprite->unk58 = sprite->unk5C * 0.05;
    sprite->x = b;
    sprite->y = a;
}

void fn_4_11A8(s8 *status, struct Sprite *sprite)
{
    static char *difficultyNames[] = {"BE", "AD", "EX", "MA"};

    if (lbl_1_bss_1744[0] == 0)
    {
        sprite->opacity = 0.0f;
    }
    else if (lbl_1_bss_1744[0] == 9)
    {
        sprite->opacity = 1.0f;
        strcpy(sprite->text, "STORY");
    }
    else
    {
        sprite->opacity = 1.0f;
        sprintf(sprite->text, "%s", difficultyNames[modeCtrl.difficulty % 4]);
    }
}

void fn_4_1274(s8 *status, struct Sprite *sprite)
{
    if (lbl_1_bss_1744[0] == 0)
    {
        sprintf(sprite->text, "STAGE %d %s", g_another_stage_id, current_stage_name);
    }
    else if (lbl_1_bss_1744[0] == 9)
    {
        sprintf(sprite->text, "WORLD %d-%d\n[%s]\nLIB. %d",
            lbl_1_bss_1744[1] / 10 + 1, lbl_1_bss_1744[1] % 10 + 1,
            current_stage_name, g_another_stage_id);
    }
    else if (modeCtrl.courseFlags & COURSE_FLAG_MASTER_EXTRA)
    {
        sprintf(sprite->text, "EXTRA MASTER EXTRA\nSTAGE %d\nLIB. %d", lbl_1_bss_1744[1], g_another_stage_id);
    }
    else if (modeCtrl.courseFlags & COURSE_FLAG_MASTER)
    {
        sprintf(sprite->text, "EXTRA MASTER\nSTAGE %d\nLIB. %d", lbl_1_bss_1744[1], g_another_stage_id);
    }
    else if (modeCtrl.courseFlags & COURSE_FLAG_EXTRA)
    {
        sprintf(sprite->text, "EXTRA\nSTAGE %d\nLIB. %d", lbl_1_bss_1744[1], g_another_stage_id);
    }
    else
    {
        sprintf(sprite->text, "STAGE %d\nLIB. %d", lbl_1_bss_1744[1], g_another_stage_id);
    }
}
