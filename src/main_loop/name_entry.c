#pragma peephole off
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "input.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "rand.h"
#include "sprite.h"
#include "stage.h"
#include "stobj.h"

#define NUM_BUTTONS 48

/* data for each button stobj */
struct NameEntryButton
{
    s8 letter;
    s32 effectUid;
    struct Color3f color;
};

struct ScoreRecord
{
    /*0x00*/ char initials[4];
    /*0x04*/ s32 score;
    /*0x08*/ u32 unk8;
    /*0x0C*/ u8 floorNum;
    /*0x0D*/ s8 unkD;
    /*0x0E*/ s8 unkE;
};  /* size = 0x10 */

struct ScoreRecordSet
{
    struct ScoreRecord recs[5];
};

extern s32 sub_mode;
extern u32 g_font_char_var_3;
extern s32 active_monkey_id[];

void textdraw_reset(void);
void textdraw_set_font(int font);
void textdraw_set_depth(float depth);
void textdraw_set_pos(float x, float y);
void textdraw_put_char(u8 c);
void textdraw_print(char *str);
void textdraw_printf(char *fmt, ...);
void load_bmp_by_id(int id);
void call_free_bmp_by_id(int id);
void destroy_sprite_with_unique_id(int id);
int strcmp(const char *a, const char *b);
#ifdef __MWERKS__
unsigned long strlen(const char *s);
#endif
void call_SoundReqID_arg_0(int id);
void g_something_with_translating_items(int type, Vec *pos, float weight);
int g_smth_with_get_active_monkey_icon(int a, int b, int c);
void inv_tf_physicsball_by_mtxa(struct PhysicsBall *src, struct PhysicsBall *dst);
void tf_physicsball_by_mtxa(struct PhysicsBall *src, struct PhysicsBall *dst);
struct GMAModel *find_stage_or_bg_model(char *name);
void fn_1_46FF8(struct Ball *ball);
void fn_1_61798(char *name);
void fn_1_D0EC4(void);
void fn_1_D0FA4(int difficulty);
void fn_1_D1220(int a);
void fn_1_D0A84(int rank, int x, int y, struct ScoreRecord *rec);

u8 lbl_1_data_62EB8[] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0xFE, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x00, 0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0F, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0D, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

static s8 s_buttonLetters[NUM_BUTTONS] =
{
    0,   0,   ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
    'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V', 'W', 'X', 'Y', 'Z', '!', '&', '-', '.', '?', '@', '9',
    '8', '7', '6', '5', '4', '3', '2', '1', '0', '\b', 0,  0,
};

static char *s_buttonModelNames[NUM_BUTTONS] =
{
    NULL,
    NULL,
    "FONT_ARROW_R",
    "FONT_A",
    "FONT_B",
    "FONT_C",
    "FONT_D",
    "FONT_E",
    "FONT_F",
    "FONT_G",
    "FONT_H",
    "FONT_I",
    "FONT_J",
    "FONT_K",
    "FONT_L",
    "FONT_M",
    "FONT_N",
    "FONT_O",
    "FONT_P",
    "FONT_Q",
    "FONT_R",
    "FONT_S",
    "FONT_T",
    "FONT_U",
    "FONT_V",
    "FONT_W",
    "FONT_X",
    "FONT_Y",
    "FONT_Z",
    "FONT_EXCLAM",
    "FONT_AMPERSAND",
    "FONT_MINUS",
    "FONT_PERIOD",
    "FONT_QUESTION",
    "FONT_ATMARK",
    "FONT_NINE",
    "FONT_EIGHT",
    "FONT_SEVEN",
    "FONT_SIX",
    "FONT_FIVE",
    "FONT_FOUR",
    "FONT_THREE",
    "FONT_TWO",
    "FONT_ONE",
    "FONT_ZERO",
    "FONT_ARROW_L",
    NULL,
    NULL,
};

static struct GMAModel *s_buttonModels[NUM_BUTTONS];
static struct NameEntryButton s_nameEntryButtons[NUM_BUTTONS];

static struct
{
    int state;
    int counter;
    int cursorPos;
    u8 name[4];
    int unk10;
    int unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
    float unk38;
    float unk3C;
} s_nameEntry;

#ifdef __MWERKS__
static struct ScoreRecordSet s_records[4];
#else
/* GC: name_entry.c is linked from asm and game_mode.c reads this object by
 * its address name (challenge game over fades to white when the score makes
 * the ranking); one object instead of a zeroed stub (stubs_gen.c) */
#define s_records lbl_1_bss_D30FC
struct ScoreRecordSet s_records[4];
#endif

/* If any of these names is entered, it will be replaced with "---". */
static char *s_censoredNames[] =
{
    "SEX",
    "XXX",
    "XX ",
    "XX",
    " XX",
    "X X",
};

static struct NameEntryButton *s_pushedButton;

struct NameEntryScores
{
    s32 size;
    struct ScoreRecordSet sets[3];
};

static struct NameEntryScores s_scores;

void g_something_with_smb1_ranking_main_game_defaults(void);
void fn_1_FBE2C(void);
void fn_1_FBE7C(int);
int fn_1_FC04C(u8 *);
int fn_1_FC208(int, struct ScoreRecord *);
void fn_1_FC25C(u8 *);
void fn_1_FC4B4(void);
void fn_1_FC8AC(struct Effect *, int);

void g_something_with_smb1_ranking_main_game_defaults_wrapper(void)
{
    g_something_with_smb1_ranking_main_game_defaults();
}

void event_name_entry_init(void)
{
    int i;
    struct GMAModel *buttonModel;
    struct GMAModel **modelIter;
    char **nameIter;
    struct NameEntryButton *button;
    struct Stobj stobj;

    s_nameEntry.state = 1;
    s_nameEntry.cursorPos = -2;
    memset(s_nameEntry.name, 0, sizeof(s_nameEntry.name));
    s_pushedButton = NULL;
    buttonModel = find_stage_or_bg_model("BUTTON");

    /* find button models */
    nameIter = s_buttonModelNames;
    modelIter = s_buttonModels;
    for (i = NUM_BUTTONS; i > 0; i--, nameIter++, modelIter++)
    {
        if (*nameIter == NULL)
            *modelIter = NULL;
        else
            *modelIter = find_stage_or_bg_model(*nameIter);
    }

    {
        struct NameEntryButton *colorIter = s_nameEntryButtons;

        for (i = NUM_BUTTONS; i > 0; i--, colorIter++)
        {
            colorIter->color.r = 0.25f;
            colorIter->color.g = 0.25f;
            colorIter->color.b = 0.25f;
        }
    }

    memset(&stobj, 0, sizeof(stobj));
    stobj.type = 0xA;
    stobj.model = buttonModel;

    /* spawn buttons */
    modelIter = s_buttonModels;
    button = s_nameEntryButtons;
    for (i = 0; i < NUM_BUTTONS; i++, button++, modelIter++)
    {
        if (*modelIter != 0)
        {
            stobj.model = buttonModel;
            stobj.rotY = (int)(s16)((-65536.0f * (0.5f + i)) / 48.0f);
            stobj.u_some_pos.x = 0.0f;
            stobj.u_some_pos.y = 0.0f;
            stobj.u_some_pos.z = -17.5f;
            mtxa_from_identity();
            mtxa_rotate_y((-65536.0f * (0.5f + i)) / 48.0f);
            mtxa_tf_vec(&stobj.u_some_pos, &stobj.u_some_pos);
            stobj.extraData = button;
            spawn_stobj(&stobj);
        }
    }

    load_bmp_by_id(3);
    s_nameEntry.unk10 = fn_1_FC208(modeCtrl.difficulty, &s_records[modeCtrl.currPlayer].recs[0]);
    s_nameEntry.unk14 = 0;
    s_nameEntry.unk18 = 640.0f;
    s_nameEntry.unk1C = 60.0f;
    s_nameEntry.unk20 = s_nameEntry.unk18;
    s_nameEntry.unk24 = s_nameEntry.unk1C;
    s_nameEntry.unk28 = 36.0f;
    s_nameEntry.unk2C = s_nameEntry.unk28;
    s_nameEntry.unk30 = 280.0f;
    s_nameEntry.unk34 = s_nameEntry.unk30;
    s_nameEntry.unk38 = 0.0f;
    s_nameEntry.unk3C = 0.0f;
}

void event_name_entry_tick(void)
{
    struct Effect effect;
    struct NameEntryButton button;
    Vec sp14;
    Vec sp8;
    float r, g, b;  /* button color */
    float var_f28;
    float temp_f1_16;
    int i;
    int var_r28;
    struct Ball *ball;
    struct Camera *camera;
    struct Effect *eff;
    struct Sprite *sprite;
    struct NameEntryButton *btn;

    ball = &balls[modeCtrl.currPlayer];
    camera = &cameras[ball->playerId];
    if (g_some_other_flags & 0xA)
        return;
    switch (s_nameEntry.state)
    {
    case 1:
        s_nameEntry.state = 2;
        break;
    case 2:  /* init */
        if (camera->mode == 0x22 || sub_mode == 0x40)
        {
            s_nameEntry.state = 3;
            s_nameEntry.cursorPos = -1;
            s_nameEntry.counter = -15;
        }
        break;
    case 3:
        s_nameEntry.counter++;
        if (s_nameEntry.counter == 15.0)
            ball->flags |= 0x400;
        if (s_nameEntry.counter == 60.0)
        {
            fn_1_FBE2C();
            ball->flags &= ~0x40;
        }
        if (sub_mode == 0x40)
        {
            s_nameEntry.state = 4;
            s_nameEntry.cursorPos = 0;
            if (s_nameEntry.counter < 60.0)
                fn_1_FBE2C();
            sprite = create_sprite();
            if (sprite != NULL)
            {
                sprite->x = 320.0f;
                sprite->y = 106.0f;
                sprite->fontId = 1;
                sprite->textAlign = 3;
                sprintf(sprite->text, "PLEASE ENTER YOUR NAME");
            }
        }
        break;
    case 4:
        s_nameEntry.state = 5;
        ball->flags &= ~0x400;
        s_nameEntry.unk14 = 1;
        s_nameEntry.unk20 = 0.0f;
        s_nameEntry.unk24 = 60.0f;
        s_nameEntry.unk2C = 36.0f;
        s_nameEntry.unk34 = 187.0 + (31.0 * (float)s_nameEntry.cursorPos);
        break;
    case 5:
        if (s_pushedButton != NULL)
            s_nameEntry.state = 6;
        if ((mode_info.flags & 2) || (mode_info.flags & 1))
        {
            s_nameEntry.state = 7;
            mode_info.flags &= ~2;
            mode_info.flags |= 1;
            ball->flags |= 0x1000;
            mode_info.flags |= 8;
            fn_1_46FF8(ball);
        }
        break;
    case 6:  /* pushed button */
        button = *s_pushedButton;
        eff = fn_1_6876C(button.effectUid);
        if (eff != NULL)
        {
            fn_1_FC8AC(eff, 5);
            memcpy_handler(&effect, eff, sizeof(effect));
            effect.u_otherTimer = 15;
            s_pushedButton->effectUid = spawn_effect(&effect);
        }
        if (eff != NULL && eff->model != NULL)
        {
            memset(&effect, 0, sizeof(effect));
            effect.type = 0x10;
            effect.model = eff->model;
            effect.pos = eff->pos;
            effect.scale.x = 1.0f;
            effect.scale.y = 1.0f;
            effect.scale.z = 1.0f;
            spawn_effect(&effect);
        }
        switch (button.letter)
        {
        case '\b':  /* backspace */
            call_SoundReqID_arg_0(0x503D);
            if (s_nameEntry.cursorPos > 0)
            {
                /* delete letter */
                s_nameEntry.cursorPos--;
                s_nameEntry.name[s_nameEntry.cursorPos] = ' ';
            }
            if (s_nameEntry.cursorPos == 2)
            {
                fn_1_FBE7C(0);
                camera->mode = 0x24;
            }
            break;
        default:  /* normal character */
            call_SoundReqID_arg_0(0xC83C);
            if (s_nameEntry.cursorPos < 3)
            {
                /* add letter */
                s_nameEntry.name[s_nameEntry.cursorPos] = button.letter;
                s_nameEntry.cursorPos++;
                if (s_nameEntry.cursorPos == 3)
                {
                    fn_1_FBE7C(1);
                    camera->mode = 0x25;
                }
            }
            break;
        }
        s_nameEntry.unk34 = 187.0 + (31.0 * (float)s_nameEntry.cursorPos);
        s_nameEntry.state = s_nameEntry.cursorPos > 3 ? 7 : 4;
        s_pushedButton = NULL;
        break;
    case 7:
        s_nameEntry.state = 8;
        s_nameEntry.name[s_nameEntry.cursorPos] = 0;
        s_nameEntry.cursorPos = 4;
        if (strlen((char *)s_nameEntry.name) == 0)
            strcpy((char *)s_nameEntry.name, "A.V");
        if (fn_1_FC04C(s_nameEntry.name))
            strcpy((char *)s_nameEntry.name, "---");
        s_nameEntry.unk34 = 736.0f;
        memcpy_handler(&s_records[modeCtrl.currPlayer].recs[0].initials, s_nameEntry.name, 4);
        fn_1_FC4B4();
        fn_1_FC25C(s_nameEntry.name);
        fn_1_61798((char *)s_nameEntry.name);
        call_SoundReqID_arg_0(0x34);
        call_SoundReqID_arg_0(0x59);
        break;
    case 8:
        s_nameEntry.state = 9;
        s_nameEntry.counter = 0x5A;
        ball->state = 0x11;
        destroy_sprite_with_unique_id(0);
        destroy_sprite_with_unique_id(2);
        fn_1_D0EC4();
        fn_1_D0FA4(modeCtrl.difficulty);
        s_nameEntry.unk24 = (s_nameEntry.unk10 * 46) + 160;
        s_nameEntry.unk2C = 136.0f;
        /* fall through */
    case 9:
        s_nameEntry.counter--;
        if (s_nameEntry.counter < 0)
        {
            ball->state = 5;
            s_nameEntry.state = 0xA;
            s_nameEntry.counter = 0x14A;
        }
        if (ball->pos.z < -4.375)
        {
            mtxa_from_mtxb();
            mtxa_rigid_inv_tf_tl(&sp14);
            sp14.x -= ball->pos.x;
            sp14.y = 0.0f;
            sp14.z -= ball->pos.z;
            vec_set_len(0.0006f, &sp14, &sp14);
            ball->vel.x += sp14.x;
            ball->vel.y += sp14.y;
            ball->vel.z += sp14.z;
        }
        break;
    case 10:
        s_nameEntry.counter--;
        if (s_nameEntry.counter > 96.0)
        {
            if (pad_status_groups[player_pad_map[ball->playerId]].unk0[2].button & 0x100)
                s_nameEntry.counter = 0x61;
        }
        else if (s_nameEntry.counter == 96.0)
        {
            s_nameEntry.unk14 = 0;
            fn_1_D1220(0);
        }
        else if (s_nameEntry.counter < 0)
            s_nameEntry.state = 0xB;
        break;
    case 11:
        s_nameEntry.state = 0;
        camera->mode = 0xE;
        ball->state = 0x1B;
        fn_1_D1220(1);
        s_nameEntry.unk14 = 0;
        break;
    }

    if (s_nameEntry.unk18 < s_nameEntry.unk20)
    {
        s_nameEntry.unk18 += 21.333333333333332;
        if (s_nameEntry.unk18 > s_nameEntry.unk20)
            s_nameEntry.unk18 = s_nameEntry.unk20;
    }
    else if (s_nameEntry.unk18 > s_nameEntry.unk20)
    {
        s_nameEntry.unk18 -= 21.333333333333332;
        if (s_nameEntry.unk18 < s_nameEntry.unk20)
            s_nameEntry.unk18 = s_nameEntry.unk20;
    }
    if (s_nameEntry.unk1C < s_nameEntry.unk24)
    {
        s_nameEntry.unk1C += 7.111111111111111;
        if (s_nameEntry.unk1C > s_nameEntry.unk24)
            s_nameEntry.unk1C = s_nameEntry.unk24;
    }
    else if (s_nameEntry.unk1C > s_nameEntry.unk24)
    {
        s_nameEntry.unk1C -= 7.111111111111111;
        if (s_nameEntry.unk1C < s_nameEntry.unk24)
            s_nameEntry.unk1C = s_nameEntry.unk24;
    }
    if (s_nameEntry.unk28 < s_nameEntry.unk2C)
    {
        s_nameEntry.unk28 += 7.111111111111111;
        if (s_nameEntry.unk28 > s_nameEntry.unk2C)
            s_nameEntry.unk28 = s_nameEntry.unk2C;
    }
    else if (s_nameEntry.unk28 > s_nameEntry.unk2C)
    {
        s_nameEntry.unk28 -= 7.111111111111111;
        if (s_nameEntry.unk28 < s_nameEntry.unk2C)
            s_nameEntry.unk28 = s_nameEntry.unk2C;
    }

    var_f28 = 0.075 * (s_nameEntry.unk34 - s_nameEntry.unk30);
    if (var_f28 > 12.0)
        var_f28 = 12.0f;
    else if (var_f28 < -12.0)
        var_f28 = -12.0f;
    s_nameEntry.unk30 += var_f28;
    s_nameEntry.unk3C += -90.0 * var_f28 * math_sin((int)s_nameEntry.unk38 + 0x4000);
    s_nameEntry.unk3C += -256.0f * math_sin(s_nameEntry.unk38);
    s_nameEntry.unk3C *= 0.98;
    s_nameEntry.unk38 += s_nameEntry.unk3C;
    btn = s_nameEntryButtons;
    switch (s_nameEntry.cursorPos)
    {
    case -1:
        var_r28 = NUM_BUTTONS - (s_nameEntry.counter >> 1);
        for (i = NUM_BUTTONS; i > 0; i--, btn++)
        {
            if (i > var_r28)
            {
                r = 0.8f;
                g = 1.0f;
                b = 2.0f;
            }
            else
            {
                r = 0.6f;
                g = 0.55f;
                b = 0.5f;
            }
            btn->color.r += 0.05 * (r - btn->color.r);
            btn->color.g += 0.05 * (g - btn->color.g);
            btn->color.b += 0.05 * (b - btn->color.b);
        }
        break;
    case -2:
        break;
    case 3:
        var_r28 = (g_font_char_var_3 << 9) & 0x3FFF;
        for (i = NUM_BUTTONS; i > 0; i--, btn++)
        {
            if (btn->letter == '\b')  /* flash the backspace button */
            {
                temp_f1_16 = math_sin(var_r28);
                r = 0.5 + temp_f1_16;
                g = 0.25 + temp_f1_16;
                b = 0.125 + temp_f1_16;
            }
            else
            {
                r = 0.6f;
                g = 0.55f;
                b = 0.5f;
            }
            btn->color.r += 0.2 * (r - btn->color.r);
            btn->color.g += 0.2 * (g - btn->color.g);
            btn->color.b += 0.2 * (b - btn->color.b);
            var_r28 += 0x800;
            var_r28 &= 0x3FFF;
        }
        break;
    default:
        var_r28 = g_font_char_var_3 >> 5;
        for (i = NUM_BUTTONS; i > 0; i--, btn++, var_r28++)
        {
            if (var_r28 & 1)
            {
                r = 2.0f;
                g = 1.0f;
                b = 0.8f;
            }
            else
            {
                r = 1.0f;
                g = 0.7f;
                b = 0.4f;
            }
            btn->color.r += 0.1 * (r - btn->color.r);
            btn->color.g += 0.1 * (g - btn->color.g);
            btn->color.b += 0.1 * (b - btn->color.b);
        }
        break;
    }
    mtxa_from_mtxb();
    mtxa_rigid_inv_tf_tl(&sp8);
    g_something_with_translating_items(5, &sp8, 0.5f);
}

void event_name_entry_dest(void)
{
    call_free_bmp_by_id(3);
}

void g_something_with_name_entry(void)
{
    struct SpriteDrawRequest params;
    int x;
    int y;
    struct Ball *ball = &balls[modeCtrl.currPlayer];

    if (s_nameEntry.unk14 != 0)
    {
        x = s_nameEntry.unk18;
        y = s_nameEntry.unk28;

        textdraw_reset();

        /* column header */
        textdraw_set_font(3);
        textdraw_set_depth(1.03f);
        textdraw_set_pos(x + 65, y);
        textdraw_put_char(0x30);  /* RANK */
        textdraw_set_pos(x + 180, y);
        textdraw_put_char(0x31);  /* NAME */
        textdraw_set_pos(x + 289, y);
        textdraw_put_char(0x32);  /* FLOOR */
        textdraw_set_pos(x + 460, y);
        textdraw_put_char(0x33);  /* SCORE */

        /* rank */
        y = s_nameEntry.unk1C;
        textdraw_set_font(3);
        textdraw_set_pos(x + 65, y);
        textdraw_put_char(s_nameEntry.unk10 + 0x31);

        /* initials */
        textdraw_set_font(3);
        textdraw_set_pos(x + 172, y);
        textdraw_print((char *)s_nameEntry.name);

        /* floor */
        fn_1_D0A84(s_nameEntry.unk10, x, y, &s_records[modeCtrl.currPlayer].recs[0]);

        /* score number */
        textdraw_set_font(3);
        y = s_nameEntry.unk1C;
        textdraw_set_pos(x + 396, y);
        textdraw_printf("%07d", s_records[modeCtrl.currPlayer].recs[0].score);

        /* line start */
        params.bmpId = 0x329;
        params.z = 1.03f;
        params.rotation = 0;
        params.opacity = 1.0f;
        params.unk30 = 2;
        params.flags = 5;
        params.mulColor = 0x00FFFFFF;
        params.addColor = 0;
        params.x = x + 39;
        params.y = y + 32;
        params.scaleX = 1.0f;
        params.scaleY = 0.0234375f;
        params.u1 = 0.0f;
        params.v1 = 0.9765625f;
        params.u2 = 1.0f;
        params.v2 = 1.0f;
        draw_sprite_draw_request(&params);

        /* line middle */
        params.x += 256.0f;
        params.scaleX = 0.1953125f;
        params.u1 = 0.0f;
        params.v1 = 0.9296875f;
        params.u2 = 0.1953125f;
        params.v2 = 0.953125f;
        draw_sprite_draw_request(&params);

        /* line end */
        params.x += 50.0f;
        params.scaleX = 1.0f;
        params.u1 = 0.0f;
        params.v1 = 0.953125f;
        params.u2 = 1.0f;
        params.v2 = 0.9765625f;
        draw_sprite_draw_request(&params);

        /* monkey */
        params.bmpId = g_smth_with_get_active_monkey_icon(0, 0, active_monkey_id[ball->playerId]);
        params.x = x + s_nameEntry.unk30;
        params.y = 15.5 + y;
        params.z = 0.2f;
        params.rotation = (s16)s_nameEntry.unk38;
        params.opacity = 1.0f;
        params.unk30 = 2;
        params.flags = 0x100A;
        params.mulColor = 0x00FFFFFF;
        params.addColor = 0;
        params.scaleX = 0.3846154f;
        params.scaleY = 0.25f;
        params.u1 = 0.0f;
        params.v1 = 0.0f;
        params.u2 = 1.0f;
        params.v2 = 1.0f;
        draw_sprite_draw_request(&params);
    }
}

void fn_1_FBE2C(void)
{
    int i;
    struct NameEntryButton *button;

    button = s_nameEntryButtons;
    for (i = 0; i < NUM_BUTTONS; i++, button++)
        button->effectUid = -1;

    fn_1_FBE7C(0);
}

void fn_1_FBE7C(int arg0)
{
    struct Effect effect;
    int i;
    struct GMAModel **modelIter;
    u8 *letterIter;
    struct NameEntryButton *button;
    struct Effect *r3;

    memset(&effect, 0, sizeof(effect));
    effect.type = 0xF;
    button = s_nameEntryButtons;
    letterIter = (u8 *)s_buttonLetters;
    modelIter = s_buttonModels;
    for (i = 0; i < NUM_BUTTONS; i++, modelIter++, letterIter++, button++)
    {
        if (*modelIter != NULL)
        {
            if (button->effectUid >= 0)
            {
                if ((r3 = fn_1_6876C(button->effectUid)) != NULL)
                    fn_1_FC8AC(r3, 5);
                button->effectUid = -1;
            }
            if (arg0 == 0 || button->letter == '\b')
            {
                effect.model = *modelIter;
                effect.pos.x = 0.0f;
                effect.pos.y = 1.5f;
                effect.pos.z = -16.5f;
                mtxa_from_identity();
                mtxa_rotate_y((-65536.0f * (0.5f + i)) / 48.0f);
                mtxa_tf_vec(&effect.pos, &effect.pos);
                effect.u_otherTimer = (rand() & 0x1F) + 0x10;
                button->effectUid = spawn_effect(&effect);
                button->letter = (s8)*letterIter;
            }
        }
    }
}

void fn_1_FBFFC(void)
{
    int i, j;
    struct NameEntryScores *p = &s_scores;
    struct ScoreRecord *row = p->sets[0].recs;
    struct ScoreRecord *rec;

    for (i = 3; i > 0; i--, row += 5)
    {
        rec = row;
        for (j = 5; j > 0; j--, rec++)
            rec->unkE = -1;
    }
}

/* returns TRUE if name is one of the censored names */
int fn_1_FC04C(u8 *name)
{
    int found = FALSE;
    int i;
    char **stringIter;

    stringIter = s_censoredNames;
    for (i = 6; i > 0; i--, stringIter++)
    {
        if (strcmp(*stringIter, (char *)name) == 0)
        {
            found = TRUE;
            break;
        }
    }
    return found;
}

#pragma force_active on
static u8 s_unused[48] = {0};
#pragma force_active reset

static char *s_defaultNames[3][5] =
{
    { "NAG", "@RI", "SHO", "SAK", "A.V" },
    { "NAG", "JAM", "MKA", "ODA", "M.S" },
    { "NAG", "H.E", "JUN", "Y.S", "AGE" },
};

void g_something_with_smb1_ranking_main_game_defaults(void)
{
    int i, j;
    struct NameEntryScores *p = &s_scores;
    struct ScoreRecord *rec;
    struct ScoreRecord *row;
    int score;
    u32 unk8;

    memset(p, 0, sizeof(*p));
    p->size = sizeof(*p);
    row = p->sets[0].recs;
    for (i = 0; i < 3; i++, row += 5)
    {
        score = 50000;
        unk8 = 0x171240;
        rec = row;
        for (j = 0; j < 5; j++, rec++, score -= 1000)
        {
            strcpy(rec->initials, s_defaultNames[i][j]);
            rec->score = score;
            rec->unk8 = unk8;
            rec->floorNum = 0xFF;
            rec->unkD = 0;
        }
    }
}

void *fn_1_FC1AC(int arg0, struct ScoreRecord *arg1)
{
    struct NameEntryScores *p = &s_scores;
    struct ScoreRecord *ptr = p->sets[arg0].recs;

    if (arg1 != NULL)
    {
        memcpy_handler(arg1, ptr, 5 * sizeof(*arg1));
        return arg1;
    }
    return ptr;
}

int fn_1_FC208(int arg0, struct ScoreRecord *arg1)
{
    struct NameEntryScores *p = &s_scores;
    struct ScoreRecord *temp = p->sets[arg0].recs;
    struct ScoreRecord *ptr = temp;
    int i;

    for (i = 0; i < 5; i++, ptr++)
    {
        if (arg1->score - ptr->score > 0)
            break;
    }
    if (i >= 5)
        i = -1;
    return i;
}

void fn_1_FC25C(u8 *name)
{
    int i, j;
    struct NameEntryScores *p = &s_scores;
    struct ScoreRecord *row;
    struct ScoreRecord *rec;

    row = p->sets[0].recs;
    for (i = 3; i > 0; i--, row += 5)
    {
        rec = row;
        for (j = 5; j > 0; j--, rec++)
        {
            if (rec->unkE == modeCtrl.currPlayer)
            {
                strcpy(rec->initials, (char *)name);
                rec->unkE = -1;
            }
        }
    }
}

void fn_1_FC300(void) {}

void fn_1_FC304(void)
{
    struct ScoreRecord spC;
    struct Ball *ball;
    int i;
    struct ScoreRecord *temp;
    struct ScoreRecord *ptr;
    int k;

    ball = &balls[modeCtrl.currPlayer];
    strcpy(spC.initials, "---");
    spC.score = ball->score;
    spC.unk8 = mode_info.unk8;
    spC.floorNum = mode_info.currFloor;
    if (modeCtrl.courseFlags & 0x800000)
        spC.unkD = 3;
    else if (modeCtrl.courseFlags & 0x10)
        spC.unkD = 2;
    else if (modeCtrl.courseFlags & 8)
        spC.unkD = 1;
    else
        spC.unkD = 0;

    temp = s_records[modeCtrl.currPlayer].recs;
    ptr = temp;
    for (i = 0; i < 5; i++, ptr++)
    {
        if (ptr->score - spC.score < 0)
            break;
    }
    if (i >= 5)
        return;
    k = i;
    ptr = temp + 4;
    for (i = 5; i > k + 1; i--, ptr--)
        ptr[0] = ptr[-1];
    temp[k] = spC;
}

static inline void fn_1_FC4B4_inline(struct ScoreRecord *rec)
{
    int j;
    struct ScoreRecord *temp;
    struct ScoreRecord *iter;
    int idx;
    int diff = modeCtrl.difficulty;
    struct NameEntryScores *p = &s_scores;

    temp = p->sets[diff].recs;
    idx = fn_1_FC208(diff, rec);
    if (idx >= 0)
    {
        iter = temp + 3;
        for (j = 4; j > idx; j--, iter--)
            memcpy_handler(iter + 1, iter, sizeof(*iter));
        memcpy_handler(&temp[idx], rec, sizeof(*iter));
        temp[idx].unkE = modeCtrl.currPlayer;
    }
}

void fn_1_FC4B4(void)
{
    int i;
    struct ScoreRecord *rec;
    struct ScoreRecordSet *set = &s_records[modeCtrl.currPlayer];

    rec = set->recs;
    for (i = 5; i > 0; i--, rec++)
        fn_1_FC4B4_inline(rec);
}

void fn_1_FC5E8(void)
{
    int j;
    int i;
    struct ScoreRecord *rec;
    struct ScoreRecordSet *set;
    u8 filler[4];

    for (i = 0; i < 4; i++)
    {
        set = &s_records[i];
        rec = set->recs;
        for (j = 5; j > 0; j--, rec++)
        {
            strcpy(rec->initials, "---");
            rec->score = -1;
            rec->unk8 = 0;
            rec->floorNum = 0xFF;
        }
    }
}

void effect_nameent_code_init(struct Effect *effect)
{
    effect->state = 0;
    effect->scale.x = 1.0f;
    effect->scale.y = 0.0f;
}

void effect_nameent_code_tick(struct Effect *effect)
{
    switch (effect->state)
    {
    case 0:
        effect->u_otherTimer--;
        if (effect->u_otherTimer < 0)
            effect->state = 1;
        break;
    case 1:
        effect->scale.y += 0.1 * (1.0 - effect->scale.y);
        break;
    case 2:
        effect->scale.y += 0.2 * (4.0 - effect->scale.y);
        effect->scale.x -= 0.06666666666666667;
        if (effect->scale.x < 0.0)
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        break;
    }
}

void effect_nameent_code_disp(struct Effect *effect)
{
    Vec sp18;
    float yscale;
    float xscale;
    u8 filler[12];

    mtxa_from_mtxb_translate(&effect->pos);
    sp18.x = 0.0f;
    sp18.y = 0.0f;
    sp18.z = 1.0f;
    mtxa_rigid_inv_tf_vec(&sp18, &sp18);
    mtxa_rotate_y(math_atan2(sp18.x, sp18.z));
    yscale = effect->scale.y;
    xscale = (1.5 * effect->scale.x) / (0.5 + effect->scale.y);
    mtxa_scale_xyz(xscale, yscale, 1.0f);
    avdisp_set_scale_factor((xscale > yscale) ? xscale : yscale);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(effect->model);
}

void effect_nameent_code_dest(struct Effect *effect)
{
    effect->model = NULL;
}

void fn_1_FC8AC(struct Effect *effect, int arg1)
{
    switch (arg1)
    {
    case 6:
        break;
    case 5:
    case 7:
        effect->state = 2;
        break;
    }
}

void effect_get_nameent_code_init(struct Effect *effect)
{
    struct Ball *ball = &balls[modeCtrl.currPlayer];

    effect->state = 0;
    effect->u_otherTimer = 60;
    effect->unk8C.x = effect->pos.x - ball->pos.x;
    effect->unk8C.y = effect->pos.y - ball->pos.y;
    effect->unk8C.z = effect->pos.z - ball->pos.z;
    effect->unk98.x = 0.5 * effect->scale.x;
    effect->unk98.y = 0.5 * effect->scale.y;
    effect->unk98.z = 0.5 * effect->scale.z;
    effect->unkA6 = 0x4000;
}

void effect_get_nameent_code_tick(struct Effect *effect)
{
    float var_f4;
    float tmp;
    struct Ball *ball = &balls[modeCtrl.currPlayer];

    switch (effect->state)
    {
    case 0:
        tmp = 1.0 - (effect->u_otherTimer / 60.0f);
        var_f4 = 1.5 * tmp;
        effect->u_otherTimer--;
        if (effect->u_otherTimer < 0)
        {
            effect->state = 1;
            effect->unk98.x = -0.05f;
        }
        break;
    default:
    case 1:
        var_f4 = 2.0f;
        if (effect->scale.x < 0.0)
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        break;
    }
    effect->unk8C.x *= 0.8;
    effect->unk8C.y += 0.2 * (var_f4 - effect->unk8C.y);
    effect->unk8C.z *= 0.8;
    effect->pos.x = ball->pos.x + effect->unk8C.x;
    effect->pos.y = ball->pos.y + effect->unk8C.y;
    effect->pos.z = ball->pos.z + effect->unk8C.z;
    effect->scale.x += 0.2 * (effect->unk98.x - effect->scale.x);
    effect->scale.y += 0.2 * (effect->unk98.y - effect->scale.y);
    effect->scale.z += 0.2 * (effect->unk98.z - effect->scale.z);
    effect->unkA6 += 0x800;
    effect->unkA6 &= 0x7FFF;
}

void effect_get_nameent_code_disp(struct Effect *effect)
{
    Vec spC;

    mtxa_from_mtxb_translate(&effect->pos);
    spC.x = 0.0f;
    spC.y = 0.0f;
    spC.z = 1.0f;
    mtxa_rigid_inv_tf_vec(&spC, &spC);
    mtxa_rotate_y(effect->unkA6 + (math_atan2(spC.x, spC.z) - 0x4000));
    mtxa_scale_xyz(effect->scale.x, effect->scale.y, effect->scale.z);
    avdisp_set_scale_factor(effect->scale.y);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(effect->model);
}

void effect_get_nameent_code_dest(struct Effect *effect) {}

void stobj_nameent_btn_init(struct Stobj *stobj)
{
    stobj->state = 0;
    stobj->unk8 |= 0x12;
    stobj->boundSphereRadius = 0.75 * stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->unkA8 = stobj->u_some_pos;
}

void stobj_nameent_btn_tick(struct Stobj *stobj)
{
    switch (stobj->state)
    {
    case 0:
        break;
    case 1:
        stobj->state = 2;
        stobj->counter = 8;
        /* fall through */
    case 2:
        stobj->counter--;
        if (stobj->counter < 0)
            stobj->unk8 |= 2;
        break;
    }

    stobj->unk64.x += 0.1 * (stobj->unkA8.x - stobj->u_some_pos.x);
    stobj->unk64.y += 0.1 * (stobj->unkA8.y - stobj->u_some_pos.y);
    stobj->unk64.z += 0.1 * (stobj->unkA8.z - stobj->u_some_pos.z);
    stobj->unk64.x *= 0.9;
    stobj->unk64.y *= 0.9;
    stobj->unk64.z *= 0.9;
    stobj->u_some_pos.x += stobj->unk64.x;
    stobj->u_some_pos.y += stobj->unk64.y;
    stobj->u_some_pos.z += stobj->unk64.z;
}

void stobj_nameent_btn_disp(struct Stobj *stobj)
{
    struct Color3f color;
    Vec pos;
    float temp_f31;
    float temp_f1_2;
    struct NameEntryButton *button = stobj->extraData;

    color = button->color;
    if (color.r > 1.0)
        color.r = 1.0f;
    if (color.g > 1.0)
        color.g = 1.0f;
    if (color.b > 1.0)
        color.b = 1.0f;
    set_post_mult_color(color.r, color.g, color.b, 1.0f);
    mtxa_from_mtxb_translate(&stobj->u_some_pos);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(stobj->model);
    mtxa_tf_point(&stobj->model->boundSphereCenter, &pos);
    if (pos.z < 0.0f)
    {
        temp_f1_2 = mathutil_vec_len(&pos);
        temp_f1_2 = (temp_f1_2 - 1.179) / temp_f1_2;
        pos.x *= temp_f1_2;
        pos.y *= temp_f1_2;
        pos.z *= temp_f1_2;
        temp_f31 = temp_f1_2 * (0.5f + (0.16666667f * (color.b + (color.r + color.g))));
        mtxa_from_translate(&pos);
        mtxa_scale_s(temp_f31);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
        avdisp_set_scale_factor(temp_f31);
        avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[94].model);
    }
    fade_color_base_default();
}

void stobj_nameent_btn_coli(struct Stobj *stobj, struct PhysicsBall *ball)
{
    struct PhysicsBall sp10;
    Vec sp6C;
    float temp_f2;
    struct NameEntryButton *button;

    mtxa_from_translate(&stobj->position);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    inv_tf_physicsball_by_mtxa(ball, &sp10);
    temp_f2 = 0.7 + ball->radius;
    sp6C.x = 0.0f;
    sp6C.y = 0.0f;
    if (sp10.pos.z > temp_f2)
        sp6C.z = 0.0f;
    else
    {
        sp6C.z = sp10.pos.z - temp_f2;
        sp10.vel.z += -0.05 * sp6C.z;
    }
    mtxa_tf_vec(&sp6C, &sp6C);
    stobj->u_some_pos.x += sp6C.x;
    stobj->u_some_pos.y += sp6C.y;
    stobj->u_some_pos.z += sp6C.z;
    stobj->unk64.x = stobj->u_some_pos.x - stobj->unk7C.x;
    stobj->unk64.y = stobj->u_some_pos.y - stobj->unk7C.y;
    stobj->unk64.z = stobj->u_some_pos.z - stobj->unk7C.z;
    if (s_pushedButton == NULL)
    {
        if (mathutil_vec_distance(&stobj->unkA8, &stobj->u_some_pos) > 1.0)
        {
            button = stobj->extraData;
            s_pushedButton = button;
            button->color.r += 1.0;
            button->color.g += 1.0;
            button->color.b += 1.0;
            stobj->state = 1;
            stobj->unk8 &= 0xFFFFFFFD;
            if (sp10.vel.z < 0.25)
                sp10.vel.z = 0.25f;
            sp10.vel.y += 0.125;
            call_SoundReqID_arg_0(0x33);
        }
    }
    tf_physicsball_by_mtxa(&sp10, ball);
}

void stobj_nameent_btn_dest(struct Stobj *stobj) {}

void stobj_nameent_btn_cb_f(struct Stobj *stobj) {}
