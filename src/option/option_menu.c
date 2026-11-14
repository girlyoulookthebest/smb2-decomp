/* Options screen: the menu panels (replays, play points, sound, split screen, ...). */
#include "types.h"
#include "bitmap.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "option.h"
#include "os.h"
#include "sound.h"
#include "sprite.h"
#include "textdraw.h"

#define M2C_FIELD(expr, type, offset) (*(type)((u8 *)(expr) + (offset)))
#define M2C_BITWISE(type, expr) (expr)
#define M2C_UNK s32

struct TextLine
{
    /*0x00*/ char *text;
    /*0x04*/ s32 count;
};

struct TextList
{
    /*0x00*/ struct TextLine *lines;
    /*0x04*/ s32 count;
};

struct ViewportLabel
{
    /*0x00*/ float x0;
    /*0x04*/ float y0;
    /*0x08*/ float x1;
    /*0x0C*/ float y1;
    /*0x10*/ u32 color;
    /*0x14*/ char *label;
    /*0x18*/ float textX;
};

struct ViewportLayout
{
    /*0x00*/ float x;
    /*0x04*/ float y;
    /*0x08*/ struct ViewportLabel *labels;
    /*0x0C*/ s32 count;
    /*0x10*/ char *name;
    /*0x14*/ float textX;
};

typedef long long OSTime;
typedef struct OSCalendarTime
{
    /*0x00*/ int sec;
    /*0x04*/ int min;
    /*0x08*/ int hour;
    /*0x0C*/ int mday;
    /*0x10*/ int mon;
    /*0x14*/ int year;
    /*0x18*/ int wday;
    /*0x1C*/ int yday;
    /*0x20*/ int msec;
    /*0x24*/ int usec;
} OSCalendarTime;

void OSTicksToCalendarTime(OSTime ticks, OSCalendarTime *cal);
void GXLoadPosMtxImm(Mtx m, u32 id);
void textdraw_reset(void);
void textdraw_set_font(int font);
void textdraw_set_scale(float x, float y);
void textdraw_set_flags(int flags);
void textdraw_set_depth(float depth);
void textdraw_set_mul_color(u32 color);
void textdraw_set_add_color(u32 color);
void textdraw_set_pos(float x, float y);
float textdraw_print(char *text);
void textdraw_printf(char *fmt, ...);
float textdraw_get_pixel_width_of_string_as_f32(char *s);
struct Sprite *get_sprite_with_unique_id(int id);
void destroy_sprite_with_unique_id(int id);
int fn_1_16D420(void);
int are_all_party_games_unlocked(void);
int return_5(void);
u32 OSGetSoundMode(void);
u8 fn_1_FD63C(void);
void fn_1_FD62C(u8 a);
void fn_1_F9B04(void *a, void *b, int c, int d, int e, int f);
void fn_1_F9CAC(void *a, int b);
void fn_1_F9E1C(void *a);
void fn_1_F9EAC(void *a, float d, float e, float f, float g, float h, int b, int c);
void g_load_sprite_tex_from_dvd(void *a);
extern u32 g_font_char_var_3;
extern u32 g_some_other_flags;


static s32 lbl_9_bss_0;
static u8 lbl_9_bss_4[0x7C];

static void update_panel(struct Sprite *sprite)
{
    struct Panel *panel = fn_9_BC84(sprite->tag);

    if (panel != NULL)
    {
        switch (panel->state)
        {
        case 1:
            if (panel->counter < 0x1E)
            {
                panel->counter++;
                panel->x -= panel->counter * 2;
            }
            else
                destroy_sprite_with_unique_id(sprite->tag);
            break;
        case 2:
            if (panel->counter < 0x1E)
            {
                panel->counter++;
                panel->x += panel->counter * 2;
            }
            else
                destroy_sprite_with_unique_id(sprite->tag);
            break;
        case 3:
            if (panel->counter > 0)
            {
                panel->x += panel->counter * 2;
                panel->counter--;
            }
            else
                panel->x = 0.0f;
            break;
        case 4:
            if (panel->counter > 0)
            {
                panel->x -= panel->counter * 2;
                panel->counter--;
            }
            else
                panel->x = 0.0f;
            break;
        }
        panel->y *= 0.9;
    }
}

void fn_9_0(s32 arg0) {
    struct Panel *panel;

    panel = fn_9_BC84(arg0);
    if (panel != NULL) {
        panel->state = 1;
        if (panel->x > 0.0) {
            panel->y = panel->x;
            panel->x = 0.0f;
            panel->counter = 0;
        }
    }
}

void fn_9_68(s32 arg0) {
    struct Panel *panel;

    panel = fn_9_BC84(arg0);
    if (panel != NULL) {
        panel->state = 2;
        if (panel->x < 0.0) {
            panel->y = panel->x;
            panel->x = 0.0f;
            panel->counter = 0;
        }
    }
}

static void panel_slide_in_left(int id)
{
    struct Panel *panel = fn_9_BC84(id);

    if (panel != NULL)
    {
        panel->state = 3;
        panel->counter = 0x1E;
        panel->x = -930.0f;
        panel->y = 0.0f;
    }
}

static void panel_slide_in_right(int id)
{
    struct Panel *panel = fn_9_BC84(id);

    if (panel != NULL)
    {
        panel->state = 4;
        panel->counter = 0x1E;
        panel->x = 930.0f;
        panel->y = 0.0f;
    }
}

static void panel_reset(int id)
{
    struct Panel *panel = fn_9_BC84(id);

    if (panel != NULL)
    {
        panel->state = 0;
        panel->counter = 0;
        panel->x = 0.0f;
        panel->y = 0.0f;
    }
}

static void draw_text(struct Sprite *s)
{
    textdraw_reset();
    textdraw_set_font(s->fontId);
    textdraw_set_scale(s->scaleX, s->scaleY);
    textdraw_set_flags(s->flags);
    textdraw_set_depth(0.1 + s->depth);
    textdraw_set_mul_color(0);
    textdraw_set_add_color(s->addB | ((s->addR << 16) | (s->addG << 8)));
    textdraw_set_pos(2.0 + s->x, 2.0 + s->y);
    textdraw_print(s->text);
    textdraw_set_depth(s->depth);
    textdraw_set_mul_color(s->mulB | ((s->mulR << 16) | (s->mulG << 8)));
    textdraw_set_add_color(s->addB | ((s->addR << 16) | (s->addG << 8)));
    textdraw_set_pos(s->x, s->y);
    textdraw_print(s->text);
}

void fn_9_D0(struct SpriteDrawRequest *arg0, f32 farg0, f32 farg1) {
    struct SpriteDrawRequest sp8;
    f32 temp_f31_91;
    f32 temp_f30_93;

    temp_f31_91 = farg0 * arg0->scaleX;
    temp_f30_93 = farg1 * arg0->scaleY;
    sp8 = *arg0;
    sp8.u1 = (6.0 / farg0);
    sp8.v1 = (6.0 / farg1);
    sp8.u2 = (1.0 - (6.0 / farg0));
    sp8.v2 = (1.0 - (6.0 / farg1));
    sp8.scaleX = ((temp_f31_91 - 12.0) / farg0);
    sp8.scaleY = ((temp_f30_93 - 12.0) / farg1);
    draw_sprite_draw_request(&sp8);
    sp8 = *arg0;
    sp8.y = (3.0 + (arg0->y - (0.5 * temp_f30_93)));
    sp8.u1 = (6.0 / farg0);
    sp8.v1 = 0.0f;
    sp8.u2 = (1.0 - (6.0 / farg0));
    sp8.v2 = (6.0 / farg1);
    sp8.scaleX = ((temp_f31_91 - 12.0) / farg0);
    sp8.scaleY = (6.0 / farg1);
    draw_sprite_draw_request(&sp8);
    sp8 = *arg0;
    sp8.y = ((arg0->y + (0.5 * temp_f30_93)) - 3.0);
    sp8.u1 = (6.0 / farg0);
    sp8.v1 = (1.0 - (6.0 / farg1));
    sp8.u2 = (1.0 - (6.0 / farg0));
    sp8.v2 = 1.0f;
    sp8.scaleX = ((temp_f31_91 - 12.0) / farg0);
    sp8.scaleY = (6.0 / farg1);
    draw_sprite_draw_request(&sp8);
    sp8 = *arg0;
    sp8.x = (3.0 + (arg0->x - (0.5 * temp_f31_91)));
    sp8.u1 = 0.0f;
    sp8.v1 = (6.0 / farg1);
    sp8.u2 = (6.0 / farg0);
    sp8.v2 = (1.0 - (6.0 / farg1));
    sp8.scaleX = (6.0 / farg0);
    sp8.scaleY = ((temp_f30_93 - 12.0) / farg1);
    draw_sprite_draw_request(&sp8);
    sp8 = *arg0;
    sp8.x = ((arg0->x + (0.5 * temp_f31_91)) - 3.0);
    sp8.u1 = (1.0 - (6.0 / farg0));
    sp8.v1 = (6.0 / farg1);
    sp8.u2 = 1.0f;
    sp8.v2 = (1.0 - (6.0 / farg1));
    sp8.scaleX = (6.0 / farg0);
    sp8.scaleY = ((temp_f30_93 - 12.0) / farg1);
    draw_sprite_draw_request(&sp8);
    sp8 = *arg0;
    sp8.x = (3.0 + (arg0->x - (0.5 * temp_f31_91)));
    sp8.y = (3.0 + (arg0->y - (0.5 * temp_f30_93)));
    sp8.u1 = 0.0f;
    sp8.v1 = 0.0f;
    sp8.u2 = (6.0 / farg0);
    sp8.v2 = (6.0 / farg1);
    sp8.scaleX = (6.0 / farg0);
    sp8.scaleY = (6.0 / farg1);
    draw_sprite_draw_request(&sp8);
    sp8 = *arg0;
    sp8.x = ((arg0->x + (0.5 * temp_f31_91)) - 3.0);
    sp8.y = (3.0 + (arg0->y - (0.5 * temp_f30_93)));
    sp8.u1 = (1.0 - (6.0 / farg0));
    sp8.v1 = 0.0f;
    sp8.u2 = 1.0f;
    sp8.v2 = (6.0 / farg1);
    sp8.scaleX = (6.0 / farg0);
    sp8.scaleY = (6.0 / farg1);
    draw_sprite_draw_request(&sp8);
    sp8 = *arg0;
    sp8.x = (3.0 + (arg0->x - (0.5 * temp_f31_91)));
    sp8.y = ((arg0->y + (0.5 * temp_f30_93)) - 3.0);
    sp8.u1 = 0.0f;
    sp8.v1 = (1.0 - (6.0 / farg1));
    sp8.u2 = (6.0 / farg0);
    sp8.v2 = 1.0f;
    sp8.scaleX = (6.0 / farg0);
    sp8.scaleY = (6.0 / farg1);
    draw_sprite_draw_request(&sp8);
    sp8 = *arg0;
    sp8.x = ((arg0->x + (0.5 * temp_f31_91)) - 3.0);
    sp8.y = ((arg0->y + (0.5 * temp_f30_93)) - 3.0);
    sp8.u1 = (1.0 - (6.0 / farg0));
    sp8.v1 = (1.0 - (6.0 / farg1));
    sp8.u2 = 1.0f;
    sp8.v2 = 1.0f;
    sp8.scaleX = (6.0 / farg0);
    sp8.scaleY = (6.0 / farg1);
    draw_sprite_draw_request(&sp8);
}

struct SpriteDrawRequest lbl_9_data_0 = { 0x40d, 320.0f, 60.0f, 5.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 };
struct SpriteDrawRequest lbl_9_data_50 = { 0xf, 0.0f, 0.0f, 1.0f, 0.5f, 0.328125f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 };
struct SpriteDrawRequest lbl_9_data_A0 = { 0x417, 320.0f, 228.0f, 5.0f, 1.0f, 1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 };
struct SpriteDrawRequest lbl_9_data_F0 = { 0x416, 320.0f, 408.0f, 5.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 };
void fn_9_7E4(void) {
    struct Sprite *temp;

    temp = create_sprite();
    if (temp != NULL) {
        temp->type = 1;
        temp->bmpId = 0x404;
        temp->x = 320.0f;
        temp->y = 240.1f;
        temp->depth = 10000.0f;
        temp->textAlign = 4;
        temp->flags |= 0x40000;
        sprintf(temp->text, "bkg");
    }
}

void fn_9_86C(s8 *status, struct Sprite *sprite) {
    struct Panel *temp_r29_587;

    temp_r29_587 = (struct Panel *)sprite->tex;
    update_panel(sprite);
    lbl_9_bss_84.panels[0].unkC += 0.5 * ((8.0 + (120.0 + (32.0 * temp_r29_587->sel))) - lbl_9_bss_84.panels[0].unkC);
}

char lbl_9_data_144[] = "k/RIPUREI";
char lbl_9_data_150[] = "k/PUREIPOINNTO";
char lbl_9_data_160[] = "k/RANNKINNGU";
char lbl_9_data_170[] = "k/SAUNNDO";
char lbl_9_data_17C[] = "h/GAMENNBUNNKATU";
char lbl_9_data_190[] = "k/KONNTORO-RA";
char lbl_9_data_1A0[] = "k/GE-MUDE-TA";
char *lbl_9_data_1B0[7] = { lbl_9_data_144, lbl_9_data_150, lbl_9_data_160, lbl_9_data_170, lbl_9_data_17C, lbl_9_data_190, lbl_9_data_1A0 };
struct TextLine lbl_9_data_210[2] = {
    { "k/MEMORI-KA-DOh/NIk/SE-BUh/SITA", 13 },
    { "k/RIPUREIh/WOMIRUKOTOGADEKIMASU.", 14 },
};
char lbl_9_data_220[] = "k/PUREIPOINNTOh/JYOUHOUDESU.";
struct TextLine lbl_9_data_240[1] = {
    { lbl_9_data_220, 14 },
};
struct TextLine lbl_9_data_27C[2] = {
    { "k/GE-MUh/NOk/RANNKINNGUh/WO", 10 },
    { "h/MIRUKOTOGADEKIMASU.", 9 },
};
struct TextLine lbl_9_data_2A4[1] = {
    { "k/SAUNNDOh/SETTEIDESU.", 10 },
};
struct TextLine lbl_9_data_2EC[2] = {
    { "k/3p/NINN/p/DOUJI/k/PUREIh/NO", 8 },
    { "h/GAMENNBUNNKATUNOSETTEIDESU.", 14 },
};
struct TextLine lbl_9_data_31C[1] = {
    { "k/KONNTORO-RAh/NOSETTEIDESU.", 13 },
};
struct TextLine lbl_9_data_364[2] = {
    { "k/GE-MUDE-TAh/NOk/SE-BUa//k/RO-DOh/YA", 15 },
    { "h/SETTEIWOOKONAIMASU.", 11 },
};
struct TextList lbl_9_data_374[7] = {
    { lbl_9_data_210, 2 },
    { lbl_9_data_240, 1 },
    { lbl_9_data_27C, 2 },
    { lbl_9_data_2A4, 1 },
    { lbl_9_data_2EC, 2 },
    { lbl_9_data_31C, 1 },
    { lbl_9_data_364, 2 },
};
void fn_9_B00(struct Sprite *sprite) {
    struct Sprite s;
    struct TextList *temp_r30_1015;
    struct Panel *temp_r29_768;
    s32 i28;
    struct Panel *temp_r3_771;
    struct SpriteDrawRequest *req;

    temp_r29_768 = (struct Panel *)sprite->tex;
    temp_r3_771 = fn_9_BC84(sprite->tag);
    if (temp_r3_771 != NULL) {
        mtxa_from_translate_xyz(temp_r3_771->x + temp_r3_771->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    draw_sprite_draw_request(&lbl_9_data_F0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0;
    s.x = 260.0f;
    s.y = 46.0f;
    strcpy(s.text, "k/OPUSHONN");
    draw_text(&s);
    fn_9_D0(&lbl_9_data_A0, 336.0f, 176.0f);
    req = &lbl_9_data_50;
    req->x = 208.0f;
    req->y = lbl_9_bss_84.panels[0].unkC;
    draw_sprite_draw_request(req);
    for (i28 = 0; i28 < 7U; i28++) {
        s.x = 258.0f;
        s.y = (120.0 + (32.0 * i28));
        s.addR = 0;
        s.addG = 0;
        s.addB = 0;
        if ((s32) temp_r29_768->sel == (s32) i28) {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
        } else {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
        }
        strcpy(s.text, lbl_9_data_1B0[i28]);
        draw_text(&s);
    }
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    temp_r30_1015 = &lbl_9_data_374[temp_r29_768->sel];
    for (i28 = 0; i28 < (s32) temp_r30_1015->count; i28++) {
        s.x = (320.0 - (0.5 * (temp_r30_1015->lines[i28].count * 0x18)));
        s.y = ((394.0 - (16.0 * (temp_r30_1015->count - 1))) + (i28 << 5));
        strcpy(s.text, temp_r30_1015->lines[i28].text);
        draw_text(&s);
    }
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_10E8(void) {
    struct Panel *panel;
    struct Sprite *temp_r3_1221;

    if (get_sprite_with_unique_id(0x58) != NULL) {
        lbl_9_bss_84.panels[0].state = 3;
        return;
    }
    panel = &lbl_9_bss_84.panels[0];
    if ((u32) (lbl_9_bss_84.flags & 1) != 0) {
        panel_slide_in_left(0x58);
    } else {
        panel_reset(0x58);
        lbl_9_bss_84.flags |= 1;
    }
    lbl_9_bss_84.panels[0].unkC = (128.0 + (32.0 * panel->sel));
    temp_r3_1221 = create_sprite();
    if (temp_r3_1221 != NULL) {
        temp_r3_1221->tag = 0x58;
        temp_r3_1221->mainFunc = fn_9_86C;
        temp_r3_1221->drawFunc = fn_9_B00;
        temp_r3_1221->tex = (struct SpriteTexture *)panel;
        strcpy(temp_r3_1221->text, "menu main");
    }
}

void fn_9_125C(s8 *status, struct Sprite *sprite) {
    f32 var_f3_1390;
    f32 var_f4_1391;
    f32 var_f3_1433;
    s32 i5;
    s32 i6;
    struct Panel *temp_r29_1258;

    temp_r29_1258 = (struct Panel *)sprite->tex;
    update_panel(sprite);
    var_f3_1390 = 228.0f;
    var_f4_1391 = 780.0f;
    switch (temp_r29_1258->mode) {
    case 0:
    case 1:
    case 6:
        var_f3_1390 = 320.0f;
        var_f4_1391 = 780.0f;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        var_f3_1390 = 220.0f;
        var_f4_1391 = 496.0f;
        break;
    }
    lbl_9_bss_84.unkC[0] += 0.1 * (var_f3_1390 - lbl_9_bss_84.unkC[0]);
    lbl_9_bss_84.unkC[1] += 0.1 * (var_f4_1391 - lbl_9_bss_84.unkC[1]);
    var_f3_1433 = 900.0f;
    switch (temp_r29_1258->mode) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
        var_f3_1433 = 900.0f;
        break;
    case 5:
        var_f3_1433 = 432.0f;
        break;
    }
    lbl_9_bss_84.unkC[2] += 0.1 * (var_f3_1433 - lbl_9_bss_84.unkC[2]);
    lbl_9_bss_84.scrollOffset /= 2;
    i6 = 0;
    for (i5 = 1; i5 < 8; i5++) {
        while ((s8) lbl_9_bss_634C[lbl_9_bss_84.scroll + i5 + i6].unk14 == -2) {
            i6 += 1;
        }
    }
    if ((s32) temp_r29_1258->sel >= lbl_9_bss_84.scroll + 7 + i6) {
        lbl_9_bss_84.scroll = (temp_r29_1258->sel - 6) - i6;
        lbl_9_bss_84.scrollOffset = 0x10;
    }
    if ((s32) temp_r29_1258->sel < lbl_9_bss_84.scroll) {
        lbl_9_bss_84.scroll = (s16) temp_r29_1258->sel;
        lbl_9_bss_84.scrollOffset = -0x10;
    }
    if ((s32) (lbl_9_bss_634C[temp_r29_1258->sel].unk2 & 0x100) == 0) {
        if (((s8) lbl_9_bss_634C[temp_r29_1258->sel].unk14 >= 0) && ((s32) lbl_9_bss_0 != (s32) lbl_9_bss_634C[temp_r29_1258->sel].unk4)) {
            lbl_9_bss_0 = (s32) lbl_9_bss_634C[temp_r29_1258->sel].unk4;
            fn_1_F9CAC(&lbl_9_bss_4, lbl_9_bss_0 - 1);
        }
        g_load_sprite_tex_from_dvd(&lbl_9_bss_4);
    }
}

char *lbl_9_data_3E8[3] = { "p/SHOKYUU/ ", "p/CHUKYUU/ ", "p/JYOKYUU/ " };
char *lbl_9_data_404[2] = { "h/MIRU", "h/KESU" };
char lbl_9_data_40C[] = "h/HAI";
struct TextLine lbl_9_data_41C[2] = {
    { lbl_9_data_40C, 2 },
    { "h/IIE", 3 },
};
struct SpriteDrawRequest lbl_9_data_42C[1] = {
    { 0x417, 320.0f, 228.0f, 5.0f, 1.0f, 1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest lbl_9_data_47C[1] = {
    { 0xf, 0.0f, 0.0f, 1.0f, 0.38125f, 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
s16 lbl_9_data_4CC[4] = { 15, 32, 51, 3 };
struct SpriteDrawRequest lbl_9_data_4D4[1] = {
    { 0x429, 496.0f, 160.0f, 5.0f, 1.0f, 0.6f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest lbl_9_data_524[1] = {
    { 0x429, 432.0f, 280.0f, 2.0f, 1.68f, 0.93f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
void fn_9_1748(struct Sprite *sprite) {
    struct Sprite s;
    OSCalendarTime cal;
    struct Panel *tex;
    s32 i28;
    s32 i27;
    s32 i26;
    s32 var_r0_2001;
    struct Panel *panel;
    struct SpriteDrawRequest *req;
    char *i5;
    char *i6;

    tex = (struct Panel *)sprite->tex;
    panel = fn_9_BC84(sprite->tag);
    if (panel != NULL) {
        mtxa_from_translate_xyz(panel->x + panel->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    req = lbl_9_data_42C;
    req->x = lbl_9_bss_84.unkC[0];
    fn_9_D0(lbl_9_data_42C, 336.0f, 176.0f);
    draw_sprite_draw_request(&lbl_9_data_F0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0x200000;
    s.x = 272.0f;
    s.y = 46.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    strcpy(s.text, "k/RIPUREI");
    draw_text(&s);
    s.depth = 3.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    s.fontId = 0x8F;
    s.x = (lbl_9_bss_84.unkC[0] - 12.0);
    i27 = 0;
    for (i28 = 0; i28 < lbl_9_bss_84.scroll; i28++) {
        if ((s8) lbl_9_bss_634C[i28].unk14 == -2) {
            i27 += 1;
        }
    }
    if (i28 > i27) {
        s.y = (95.0 - (f32) ((u32) (g_font_char_var_3 % 30) >> 1U));
        strcpy(s.text, "p/SANNKAKU_U/");
        draw_text(&s);
    }
    i27 = 0;
    for (i28 = 0; ((s8) lbl_9_bss_634C[i28 + lbl_9_bss_84.scroll].unk14 != -1) && (i28 < 0x7F); i28++) {
        if ((s8) lbl_9_bss_634C[i28 + lbl_9_bss_84.scroll].unk14 == -2) {
            i27 += 1;
        }
    }
    if ((s32) (i28 - i27) > 7) {
        s.y = (340.0 + (f32) ((u32) (g_font_char_var_3 % 30) >> 1U));
        strcpy(s.text, "p/SANNKAKU_B/");
        draw_text(&s);
    }
    if (((u32) (g_some_other_flags & 0xA) == 0) && (lbl_9_bss_84.unk3B > 0)) {
        lbl_9_bss_84.unk3B--;
    }
    s.depth = 3.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    s.fontId = 0x8F;
    switch (tex->mode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6: {
            i27 = 0;
            for (i28 = 0; i28 < 7 && (s8) lbl_9_bss_634C[i28].unk14 != -1; i28++) {
                var_r0_2001 = 0;
                i26 = lbl_9_bss_84.scroll + i27;
                i26 = i28 + i26;
                while ((s8) lbl_9_bss_634C[i26].unk14 == -2) {
                    i27 += 1;
                    i26 = lbl_9_bss_84.scroll + i27;
                i26 = i28 + i26;
                }
                if ((s8) lbl_9_bss_634C[i26].unk14 == -1) {
                    break;
                }
                {
                    s.x = (lbl_9_bss_84.unkC[0] - 84.0);
                    s.y = 120.0 + (32.0 * i28) + lbl_9_bss_84.scrollOffset + (i26 > lbl_9_bss_84.unk3A ? lbl_9_bss_84.unk3B : 0);
                    if ((s32) (lbl_9_bss_634C[i26].unk2 & 0x100) != 0) {
                        if (i26 == (s32) tex->sel) {
                            s.mulR = 0xFF;
                            s.mulG = 0;
                            s.mulB = 0;
                        } else {
                            s.mulR = 0x8F;
                            s.mulG = 0;
                            s.mulB = 0;
                        }
                        sprintf(s.text, "h/HASONk/DE-TA");
                        draw_text(&s);
                    } else {
                        if (i26 == (s32) tex->sel) {
                            s.mulR = 0xFF;
                            s.mulG = 0xFF;
                            s.mulB = 0;
                        } else {
                            s.mulR = 0x8F;
                            s.mulG = 0x8F;
                            s.mulB = 0;
                        }
                        i6 = "STAGE";
                        if ((s32) (lbl_9_bss_634C[i26].unk2 & 0x20) != 0) {
                            i6 = "EXTRA";
                        }
                        if ((s32) (lbl_9_bss_634C[i26].unk2 & 0x40) != 0) {
                            i6 = "MASTER";
                            var_r0_2001 = 1;
                        }
                        if (var_r0_2001 != 0) {
                            i5 = "";
                        } else {
                            i5 = lbl_9_data_3E8[lbl_9_bss_634C[i26].unk5];
                        }
                        sprintf(s.text, "%sa/%s %d", i5, i6, lbl_9_bss_634C[i26].unk6);
                        draw_text(&s);
                        req = lbl_9_data_47C;
                        req->bmpId = lbl_9_data_4CC[lbl_9_bss_634C[i26].unk7];
                        req->x = (lbl_9_bss_84.unkC[0] - 122.0);
                        req->y = 10.0 + (120.0 + (32.0 * i28)) + lbl_9_bss_84.scrollOffset + (i26 > lbl_9_bss_84.unk3A ? lbl_9_bss_84.unk3B : 0);
                        if (i26 == (s32) tex->sel) {
                            req->mulColor = -1;
                        } else {
                            req->mulColor = -0x707071;
                        }
                        draw_sprite_draw_request(req);
                    }
                }
            }
        } break;
    }
    s.depth = 0.1f;
    req = lbl_9_data_4D4;
    req->x = lbl_9_bss_84.unkC[1];
    draw_sprite_draw_request(req);
    for (i28 = 0; i28 < 2U; i28++) {
        if ((s32) i28 == (s32) tex->sel2) {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
        } else {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
        }
        s.x = (lbl_9_bss_84.unkC[1] - 24.0);
        s.y = (131.0 + (32.0 * i28));
        strcpy(s.text, lbl_9_data_404[i28]);
        draw_text(&s);
    }
    req = lbl_9_data_524;
    req->x = lbl_9_bss_84.unkC[2];
    draw_sprite_draw_request(req);
    s.mulR = 0xEF;
    s.mulG = 0x9A;
    s.mulB = 0;
    s.x = (lbl_9_bss_84.unkC[2] - 144.0);
    s.y = 228.0f;
    strcpy(s.text, "h/KESITEMOYOROSIIDESUKA?");
    draw_text(&s);
    for (i28 = 0; i28 < 2U; i28++) {
        if ((s32) i28 == (s32) lbl_9_bss_84.unk4) {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
        } else {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
        }
        s.x = (lbl_9_bss_84.unkC[2] - (12.0 * lbl_9_data_41C[i28].count));
        s.y = (270.0 + (32.0 * i28));
        strcpy(s.text, lbl_9_data_41C[i28].text);
        draw_text(&s);
    }
    if ((s32) (lbl_9_bss_634C[tex->sel].unk2 & 0x100) != 0) {
        s.mulR = 0xFF;
        s.mulG = 0xFF;
        s.mulB = 0xFF;
        s.x = 145.0f;
        s.y = 378.0f;
        sprintf(s.text, "h/KONOk/DE-TAh/HA HASONNSHITEIMASU.");
        draw_text(&s);
        s.x = 246.0f;
        s.y = 414.0f;
        sprintf(s.text, "h/KESHITEKUDASAI.");
        draw_text(&s);
    } else {
        s.mulR = 0xFF;
        s.mulG = 0xFF;
        s.mulB = 0xFF;
        s.x = 230.0f;
        s.y = 378.0f;
        sprintf(s.text, "%6dpts", lbl_9_bss_634C[tex->sel].unk8);
        draw_text(&s);
        s.x = 270.0f;
        s.y = 414.0f;
        s.fontId = 0x8F;
        OSTicksToCalendarTime((OSTime)(u32)(lbl_9_bss_634C[tex->sel].unkC) * (*(u32 *)0x800000F8 / 4), &cal);
        sprintf(s.text, "%02d/%02d/%02d %02d:%02d\n", cal.year % 100, cal.mon + 1, cal.mday, cal.hour, cal.min);
        draw_text(&s);
        if ((s8) lbl_9_bss_634C[tex->sel].unk14 >= 0) {
            fn_1_F9EAC(&lbl_9_bss_4, 102.0f, 374.0f, 1.0f, 96.0f, 68.0f, -1, 0);
        }
    }
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_2D84(struct Sprite *sprite) {
    fn_1_F9E1C(&lbl_9_bss_4);
}

void fn_9_2DAC(void) {
    struct Panel *panel;
    struct Sprite *temp_r3_3156;

    if (get_sprite_with_unique_id(0x59) != NULL) {
        lbl_9_bss_84.panels[1].state = 4;
        return;
    }
    panel = &lbl_9_bss_84.panels[1];
    panel_slide_in_right(0x59);
    lbl_9_bss_84.scrollOffset = 0;
    lbl_9_bss_84.unkC[0] = 320.0f;
    lbl_9_bss_84.unkC[1] = 780.0f;
    lbl_9_bss_84.unkC[2] = 900.0f;
    if ((s8) lbl_9_bss_634C[panel->sel].unk14 >= 0) {
        lbl_9_bss_0 = (s32) lbl_9_bss_634C[panel->sel].unk4;
    }
    fn_1_F9B04(&lbl_9_bss_4, &"preview/96x68.tpl", lbl_9_bss_0 - 1, 0x60, 0x44, 5);
    temp_r3_3156 = create_sprite();
    if (temp_r3_3156 != NULL) {
        temp_r3_3156->tag = 0x59;
        temp_r3_3156->mainFunc = fn_9_125C;
        temp_r3_3156->drawFunc = fn_9_1748;
        temp_r3_3156->destFunc = fn_9_2D84;
        temp_r3_3156->tex = (struct SpriteTexture *)panel;
        strcpy(temp_r3_3156->text, "menu main");
    }
}

void fn_9_2F44(s8 *status, struct Sprite *sprite) {

    update_panel(sprite);
}

struct SpriteDrawRequest lbl_9_data_664[2] = {
    { 0x40e, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
    { 0x411, 0.0f, 0.0f, 1.0f, 0.38f, 0.11111111f, 0.0f, 0.5555556f, 0.38f, 0.6666667f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest lbl_9_data_704[1] = {
    { 0x411, 0.0f, 0.0f, 1.0f, 0.37f, 0.11111111f, 0.0f, 0.6666667f, 0.37f, 0.7777778f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest lbl_9_data_754[1] = {
    { 0x411, 0.0f, 0.0f, 1.0f, 0.36f, 0.11111111f, 0.0f, 0.7777778f, 0.36f, 0.8888889f, 0, 1.0f, -1, 10, -1, 0 },
};
void *lbl_9_data_7A4[23] = {
    (void *)0x411,
    (void *)0x0,
    (void *)0x0,
    (void *)0x3f800000,
    (void *)0x3e570a3d,
    (void *)0x3de38e39,
    (void *)0x3f251eb8,
    (void *)0x3f638e39,
    (void *)0x3f5ae148,
    (void *)0x3f800000,
    (void *)0x0,
    (void *)0x3f800000,
    (void *)0xffffffff,
    (void *)0xa,
    (void *)0xffffffff,
    (void *)0x0,
    (void *)0x0,
    (void *)0x0,
    (void *)0x0,
    (void *)0x0,
    lbl_9_data_704,
    lbl_9_data_754,
    lbl_9_data_7A4
};
struct SpriteDrawRequest lbl_9_data_800[1] = {
    { 0x5f, 320.0f, 228.0f, 5.0f, 1.28f, 1.13f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
char *lbl_9_data_8C0[6] = { "h/GENNZAINOk/PUREIPOINNTOp/SUU/", "k/MINIGE-MUGETTOh/MADEATO", "k/GETTOh/SITAk/MINIGE-MU", "", "", "k/KONNTEXINYU-p/KAISUU/" };
void fn_9_318C(struct Sprite *sprite) {
    struct Sprite s;
    s32 i5;
    s32 temp_ret_3829;
    struct Panel *panel;
    s32 i29;

    panel = fn_9_BC84(sprite->tag);
    if (panel != NULL) {
        mtxa_from_translate_xyz(panel->x + panel->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0x200000;
    s.x = 236.0f;
    s.y = 46.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    strcpy(s.text, "k/PUREIPOINNTO");
    draw_text(&s);
    fn_9_D0(lbl_9_data_800, 416.0f, 208.0f);
    for (i29 = 0; i29 < 6U; i29++) {
        s.x = 80.0f;
        s.y = (135.0 + (32.0 * i29));
        if ((s32) i29 == 0) {
            if (fn_1_16D420() != 0) {
                strcpy(s.text, "h/SAISINNNOk/PUREIPOINNTOh/KIROKU");
                s.scaleX = 0.9f;
            } else {
                strcpy(s.text, lbl_9_data_8C0[i29]);
                s.scaleX = 1.0f;
            }
        } else if ((s32) i29 == 1) {
            if (fn_1_16D420() != 0) {
                strcpy(s.text, "k/PUREIPOINNTOh/NOk/SAIKO-h/KIROKU");
                s.scaleX = 0.9f;
            } else if (are_all_party_games_unlocked() != 0) {
                strcpy(s.text, "k/KONNTEXINYU-p/KAISUU/k/APPUh/MADEATO");
                s.scaleX = 0.8f;
            } else {
                strcpy(s.text, lbl_9_data_8C0[i29]);
                s.scaleX = 1.0f;
            }
        } else {
            strcpy(s.text, lbl_9_data_8C0[i29]);
            s.scaleX = 1.0f;
        }
        draw_text(&s);
    }
    s.x = 464.0f;
    s.y = 135.0f;
    sprintf(s.text, "%4d", 0);
    draw_text(&s);
    if (fn_1_16D420() != 0) {
        s.x = 464.0f;
        s.y = 167.0f;
        sprintf(s.text, "%4d", 0);
        draw_text(&s);
    } else {
        i5 = 0x9C4;
        if (i5 < 0) {
            i5 = 0;
        }
        s.x = 464.0f;
        s.y = 167.0f;
        sprintf(s.text, "%4d", i5);
        draw_text(&s);
    }
    s.x = 464.0f;
    s.y = 295.0f;
    if (fn_1_16D420() != 0) {
        strcpy(s.text, " h/MUGENN");
    } else {
        temp_ret_3829 = return_5();
        sprintf(s.text, "%4d", temp_ret_3829);
    }
    draw_text(&s);
    mtxa_from_mtx(lbl_9_bss_84.mtx);
    mtxa_translate_xyz(300.0f, 260.0f, 0.0f);
    mtxa_scale_s(0.66f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_from_mtx(lbl_9_bss_84.mtx);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    draw_sprite_draw_request(&lbl_9_data_F0);
    s.x = 152.0f;
    s.y = 394.0f;
    strcpy(s.text, lbl_9_data_220);
    draw_text(&s);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_3B78(void) {
    struct Sprite *temp_r3_4030;

    if (get_sprite_with_unique_id(0x5A) != NULL) {
        lbl_9_bss_84.panels[2].state = 4;
        return;
    }
    panel_slide_in_right(0x5A);
    temp_r3_4030 = create_sprite();
    if (temp_r3_4030 != NULL) {
        temp_r3_4030->tag = 0x5A;
        temp_r3_4030->mainFunc = fn_9_2F44;
        temp_r3_4030->drawFunc = fn_9_318C;
        strcpy(temp_r3_4030->text, "menu main");
    }
}

void fn_9_3C34(s8 *status, struct Sprite *sprite) {

    update_panel(sprite);
}

struct SpriteDrawRequest lbl_9_data_968[1] = {
    { 0x408, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.5f, -1, 10, -1, 0 },
};
void fn_9_3E7C(struct Sprite *sprite) {
    struct Sprite s;
    struct Panel *panel;
    struct SpriteDrawRequest *temp_r3_4544;
    u32 temp_r29_4403;

    panel = fn_9_BC84(sprite->tag);
    if (panel != NULL) {
        mtxa_from_translate_xyz(panel->x + panel->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0x200000;
    s.x = 272.0f;
    s.y = 46.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    strcpy(s.text, "k/SAUNNDO");
    draw_text(&s);
    fn_9_D0(lbl_9_data_800, 416.0f, 208.0f);
    s.x = 78.0f;
    s.y = 216.0f;
    strcpy(s.text, "k/SAUNNDOh/SETTEI");
    draw_text(&s);
    temp_r29_4403 = OSGetSoundMode();
    s.x = 426.0f;
    s.y = 216.0f;
    s.scaleX = 0.5f;
    strcpy(s.text, "p/SANNKAKU_L/         p/SANNKAKU_R/");
    draw_text(&s);
    s.scaleX = 1.0f;
    s.x = 444.0f;
    s.y = 216.0f;
    if (temp_r29_4403 == 1U) {
        strcpy(s.text, "k/SUTEREO");
    } else {
        strcpy(s.text, "k/MONORARU");
    }
    draw_text(&s);
    temp_r3_4544 = lbl_9_data_968;
    temp_r3_4544->x = 492.0f;
    temp_r3_4544->y = 228.0f;
    temp_r3_4544->scaleX = 5.0f;
    temp_r3_4544->scaleY = 1.3333334f;
    draw_sprite_draw_request(temp_r3_4544);
    draw_sprite_draw_request(&lbl_9_data_F0);
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    s.x = 176.0f;
    s.y = 394.0f;
    strcpy(s.text, "k/SUPI-KA-h/NOSETTEIDESU.");
    draw_text(&s);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_4510(void) {
    struct Sprite *temp_r3_4673;

    if (get_sprite_with_unique_id(0x5C) != NULL) {
        lbl_9_bss_84.panels[4].state = 4;
        return;
    }
    panel_slide_in_right(0x5C);
    temp_r3_4673 = create_sprite();
    if (temp_r3_4673 != NULL) {
        temp_r3_4673->tag = 0x5C;
        temp_r3_4673->mainFunc = fn_9_3C34;
        temp_r3_4673->drawFunc = fn_9_3E7C;
        strcpy(temp_r3_4673->text, "sound menu");
    }
}

void fn_9_45CC(s8 *status, struct Sprite *sprite) {

    update_panel(sprite);
}

struct SpriteDrawRequest lbl_9_data_A3C[1] = {
    { 0x429, 0.0f, 0.0f, 5.0f, 0.8f, 0.8f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest lbl_9_data_A8C[2] = {
    { 0x4b, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
    { 0x4b, 0.0f, 0.0f, 3.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.5f, -1, 10, -1, 0 },
};
struct ViewportLabel lbl_9_data_B48[3] = {
    { 0.0f, 0.0f, 1.0f, 0.5f, 0xffff0000, "a/1P WIDE", 62.0f },
    { 0.0f, 0.5f, 0.5f, 0.5f, 0xff2d65ff, "a/2P", 20.0f },
    { 0.5f, 0.5f, 0.5f, 0.5f, 0xfffbff2d, "a/3P", 20.0f },
};
struct ViewportLabel lbl_9_data_BB0[3] = {
    { 0.0f, 0.5f, 0.5f, 0.5f, 0xffff0000, "a/1P", 20.0f },
    { 0.0f, 0.0f, 1.0f, 0.5f, 0xff2d65ff, "a/2P WIDE", 62.0f },
    { 0.5f, 0.5f, 0.5f, 0.5f, 0xfffbff2d, "a/3P", 20.0f },
};
struct ViewportLabel lbl_9_data_C10[3] = {
    { 0.0f, 0.0f, 0.5f, 0.5f, 0xffff0000, "a/1P", 20.0f },
    { 0.5f, 0.0f, 0.5f, 0.5f, 0xff2d65ff, "a/2P", 20.0f },
    { 0.0f, 0.5f, 1.0f, 0.5f, 0xfffbff2d, "a/3P WIDE", 62.0f },
};
struct ViewportLabel lbl_9_data_C6C[4] = {
    { 0.0f, 0.0f, 0.5f, 0.5f, 0xffff0000, "a/1P", 20.0f },
    { 0.5f, 0.0f, 0.5f, 0.5f, 0xff2d65ff, "a/2P", 20.0f },
    { 0.0f, 0.5f, 0.5f, 0.5f, 0xfffbff2d, "a/3P", 20.0f },
    { 0.5f, 0.5f, 0.5f, 0.5f, 0xff2dff57, "a/LIVE", 34.0f },
};
struct ViewportLayout lbl_9_data_D10[4] = {
    { 235.0f, 166.0f, lbl_9_data_B48, 3, "a/1Pk/WAIDO", -200.0f },
    { 405.0f, 166.0f, lbl_9_data_BB0, 3, "a/2Pk/WAIDO", 95.0f },
    { 235.0f, 294.0f, lbl_9_data_C10, 3, "a/3Pk/WAIDO", -200.0f },
    { 405.0f, 294.0f, lbl_9_data_C6C, 4, "a/4h/BUNNKATU", 95.0f },
};
void fn_9_4814(struct Sprite *sprite) {
    struct Sprite s;
    struct SpriteDrawRequest *req;
    s32 i28;
    struct ViewportLabel *i27;
    s32 i26;
    struct Panel *panel;
    u32 col;
    u8 b;
    u8 a;
    u8 c;
    u8 d;

    panel = fn_9_BC84(sprite->tag);
    if (panel != NULL) {
        mtxa_from_translate_xyz(panel->x + panel->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0x200000;
    s.x = 152.0f;
    s.y = 46.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    strcpy(s.text, "h/3p/NINN/p/DOUJI/k/PUREIh/NOk/GE-MUh/GAMENN");
    draw_text(&s);
    s.fontId = 0x8F;
    for (i28 = 0; i28 < 4U; i28++) {
        req = lbl_9_data_A3C;
        req->x = lbl_9_data_D10[i28].x;
        req->y = lbl_9_data_D10[i28].y;
        draw_sprite_draw_request(req);
        if ((s32) i28 == (s32) modeCtrl.splitscreenMode) {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
        } else {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
        }
        s.x = lbl_9_data_D10[i28].x + lbl_9_data_D10[i28].textX;
        s.y = (lbl_9_data_D10[i28].y - 12.0);
        strcpy(s.text, lbl_9_data_D10[i28].name);
        draw_text(&s);
        i27 = lbl_9_data_D10[i28].labels;
        i26 = 0;
        while (i26 < (s32) lbl_9_data_D10[i28].count) {
            req = lbl_9_data_A8C;
            req->x = (lbl_9_data_D10[i28].x + (150.4 * ((i27->x0 + (0.5 * i27->x1)) - 0.5)));
            req->y = (lbl_9_data_D10[i28].y + (112.0 * ((i27->y0 + (0.5 * i27->y1)) - 0.5)));
            req->scaleX = (150.4 * i27->x1 / 8.0);
            req->scaleY = (112.0 * i27->y1 / 8.0);
            if ((s32) i28 == (s32) modeCtrl.splitscreenMode) {
                req->mulColor = i27->color;
            } else {
                col = i27->color;
                a = col >> 24;
                b = (col >> 16) & 0xFF;
                c = (col >> 8) & 0xFF;
                d = col;
                a *= 0.5f;
                b *= 0.5f;
                c *= 0.5f;
                d *= 0.5f;
                col = d | ((c << 8) | ((a << 24) | (b << 16)));
                req->mulColor = col;
            }
            draw_sprite_draw_request(req);
            s.x = req->x - i27->textX;
            s.y = (req->y - 12.0);
            strcpy(s.text, i27->label);
            if ((s32) i28 == (s32) modeCtrl.splitscreenMode) {
                s.mulR = 0xFF;
                s.mulG = 0xFF;
                s.mulB = 0xFF;
            } else {
                s.mulR = 0x8F;
                s.mulG = 0x8F;
                s.mulB = 0x8F;
            }
            draw_text(&s);
            i26 += 1;
            i27++;
        }
    }
    s.fontId = 0x8E;
    draw_sprite_draw_request(&lbl_9_data_F0);
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    s.x = 152.0f;
    s.y = 378.0f;
    strcpy(s.text, "h/3p/NINN/p/DOUJI/k/PUREIh/DEASOBUTOKINO");
    draw_text(&s);
    s.x = 128.0f;
    s.y = 410.0f;
    strcpy(s.text, "k/GE-MUh/GAMENNk/REIAUTOh/WOKIMEMASU.");
    draw_text(&s);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_5194(void) {
    struct Sprite *temp_r3_5511;

    if (get_sprite_with_unique_id(0x5D) != NULL) {
        lbl_9_bss_84.panels[5].state = 4;
        return;
    }
    panel_slide_in_right(0x5D);
    temp_r3_5511 = create_sprite();
    if (temp_r3_5511 != NULL) {
        temp_r3_5511->tag = 0x5D;
        temp_r3_5511->mainFunc = fn_9_45CC;
        temp_r3_5511->drawFunc = fn_9_4814;
        strcpy(temp_r3_5511->text, "sound menu");
    }
}

void fn_9_5250(s8 *status, struct Sprite *sprite) {
    f32 var_f3_5680;
    f32 var_f4_5681;
    s32 i5;
    struct Panel *temp_r29_5545;

    temp_r29_5545 = (struct Panel *)sprite->tex;
    update_panel(sprite);
    for (i5 = 0; i5 < 3; i5++) {
        var_f3_5680 = 0.8f;
        var_f4_5681 = 40.0f;
        if (i5 == (s32) temp_r29_5545->sel) {
            var_f3_5680 = 1.2f;
            var_f4_5681 = 0.0f;
        }
        lbl_9_bss_84.scale[i5] += 0.1 * (var_f3_5680 - lbl_9_bss_84.scale[i5]);
        lbl_9_bss_84.yOffset[i5] += 0.1 * (var_f4_5681 - lbl_9_bss_84.yOffset[i5]);
    }
}

struct SpriteDrawRequest lbl_9_data_DF4[1] = {
    { 0x411, 0.0f, 0.0f, 1.0f, 0.235f, 0.11111111f, 0.765f, 0.33333334f, 1.0f, 0.44444445f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest lbl_9_data_E44[1] = {
    { 0x411, 0.0f, 0.0f, 1.0f, 0.29f, 0.11111111f, 0.71f, 0.6666667f, 1.0f, 0.7777778f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest lbl_9_data_E94[1] = {
    { 0x411, 0.0f, 0.0f, 1.0f, 0.205f, 0.11111111f, 0.79f, 0.44444445f, 0.995f, 0.5555556f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest *lbl_9_data_EE4[3] = { lbl_9_data_DF4, lbl_9_data_E44, lbl_9_data_E94 };
struct SpriteDrawRequest lbl_9_data_EF0[1] = {
    { 0x411, 0.0f, 0.0f, 1.0f, 0.265f, 0.11111111f, 0.725f, 0.5555556f, 0.99f, 0.6666667f, 0, 1.0f, -1, 10, -1, 0 },
};
struct TextLine lbl_9_data_F9C[2] = {
    { "k/MEINNGE-MUh/NOk/NO-MARUMO-DO", 14 },
    { "p/SHOKYUU/h/,p/CHUKYUU/h/,p/JYOKYUU/h/NOk/RANNKINNGUh/DESU.", 16 },
};
struct TextLine lbl_9_data_FFC[2] = {
    { "k/PA-TEXIGE-MUh/NOk/MONNKI-RE-SU,", 15 },
    { "k/MONNKI-TA-GETTOh/NOk/RANNKINNGUh/DESU.", 17 },
};
struct TextLine lbl_9_data_1058[2] = {
    { "k/MINIGE-MUh/NOk/MONNKI-BOURINNGU,", 15 },
    { "k/MONNKI-GORUHUh/NOk/RANNKINNGUh/DESU.", 15 },
};
struct TextList lbl_9_data_1068[3] = {
    { lbl_9_data_F9C, 2 },
    { lbl_9_data_FFC, 2 },
    { lbl_9_data_1058, 2 },
};
void fn_9_552C(struct Sprite *sprite) {
    struct Sprite s;
    struct TextList *temp_r30_5980;
    struct Panel *temp_r29_5745;
    s32 i28;
    s32 i27;
    u32 i26;
    struct Panel *temp_r3_5748;
    struct SpriteDrawRequest *temp_r3_5896;
    struct SpriteDrawRequest *temp_r3_5940;
    struct SpriteDrawRequest *temp_r3_5948;

    temp_r29_5745 = (struct Panel *)sprite->tex;
    temp_r3_5748 = fn_9_BC84(sprite->tag);
    if (temp_r3_5748 != NULL) {
        mtxa_from_translate_xyz(temp_r3_5748->x + temp_r3_5748->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0x200000;
    s.x = 260.0f;
    s.y = 46.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    strcpy(s.text, "k/RANNKINNGU");
    draw_text(&s);
    for (i28 = 0; i28 < 3; i28++) {
        mtxa_from_mtx(lbl_9_bss_84.mtx);
        mtxa_translate_xyz((320.0 + (188.0 * (i28 - 1))), (214.0 + lbl_9_bss_84.yOffset[i28]), 0.0f);
        mtxa_scale_s(lbl_9_bss_84.scale[i28]);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        temp_r3_5896 = lbl_9_data_664;
        temp_r3_5896->x = 0.0f;
        temp_r3_5896->y = 0.0f;
        if (i28 == (s32) temp_r29_5745->sel) {
            temp_r3_5896->mulColor = -1;
        } else {
            temp_r3_5896->mulColor = -0x707071;
        }
        draw_sprite_draw_request(temp_r3_5896);
        if (i28 == (s32) temp_r29_5745->sel) {
            i27 = -1;
            i26 = (u32) (96.0 * fabs(math_sin((s16) (g_font_char_var_3 << 9))));
            i26 = i26 | ((i26 << 0x10) | (i26 << 8));
        } else {
            i27 = 0xFF8F8F8F;
            i26 = 0;
        }
        temp_r3_5940 = lbl_9_data_EE4[i28];
        temp_r3_5940->x = 0.0f;
        temp_r3_5940->y = -20.0f;
        temp_r3_5940->mulColor = i27;
        temp_r3_5940->addColor = i26;
        draw_sprite_draw_request(temp_r3_5940);
        temp_r3_5948 = lbl_9_data_EF0;
        temp_r3_5948->x = 0.0f;
        temp_r3_5948->y = 20.0f;
        temp_r3_5948->mulColor = i27;
        temp_r3_5948->addColor = i26;
        draw_sprite_draw_request(temp_r3_5948);
    }
    mtxa_from_mtx(lbl_9_bss_84.mtx);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    draw_sprite_draw_request(&lbl_9_data_F0);
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    temp_r30_5980 = &lbl_9_data_1068[temp_r29_5745->sel];
    for (i28 = 0; i28 < (s32) temp_r30_5980->count; i28++) {
        s.x = (320.0 - (0.5 * (temp_r30_5980->lines[i28].count * 0x18)));
        s.y = ((394.0 - (16.0 * (temp_r30_5980->count - 1))) + (32.0 * i28));
        strcpy(s.text, temp_r30_5980->lines[i28].text);
        draw_text(&s);
    }
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_5AD0(void) {
    struct Panel *panel;
    s32 var;
    struct Sprite *temp_r3_6195;

    if (get_sprite_with_unique_id(0x5B) != NULL) {
        if (lbl_9_bss_84.panels[3].state == 1) {
            lbl_9_bss_84.panels[3].state = 3;
        } else {
            lbl_9_bss_84.panels[3].state = 4;
        }
        return;
    }
    panel = &lbl_9_bss_84.panels[3];
    if (lbl_9_bss_84.panels[3].state == 1) {
        panel_slide_in_left(0x5B);
    } else {
        panel_slide_in_right(0x5B);
    }
    for (var = 0; var < 3; var++) {
        lbl_9_bss_84.scale[var] = 0.0f;
        lbl_9_bss_84.yOffset[var] = 40.0f;
    }
    temp_r3_6195 = create_sprite();
    if (temp_r3_6195 != NULL) {
        temp_r3_6195->tag = 0x5B;
        temp_r3_6195->mainFunc = fn_9_5250;
        temp_r3_6195->drawFunc = fn_9_552C;
        temp_r3_6195->tex = (struct SpriteTexture *)panel;
        strcpy(temp_r3_6195->text, "sound menu");
    }
}

void fn_9_5C60(s8 *status, struct Sprite *sprite) {
    struct Panel *temp_r29_6232;

    temp_r29_6232 = (struct Panel *)sprite->tex;
    update_panel(sprite);
    lbl_9_bss_84.panels[6].unkC += 0.5 * ((206.0 + (32.0 * temp_r29_6232->sel2)) - lbl_9_bss_84.panels[6].unkC);
}

struct SpriteDrawRequest lbl_9_data_1090[1] = {
    { 0x5f, 320.0f, 228.0f, 5.0f, 1.26f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
struct SpriteDrawRequest lbl_9_data_10E0[1] = {
    { 0x5f, 320.0f, 228.0f, 5.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
struct TextLine lbl_9_data_1164[2] = {
    { "k/KONNTORO-RUSUTEXIKKUh/NO", 12 },
    { "h/CHOUSEIWOOKONAIMASU.", 12 },
};
struct TextLine lbl_9_data_119C[1] = {
    { "k/KONNTORO-RAh/NOSINNDOUWOSETTEISIMASU.", 19 },
};
struct TextList lbl_9_data_11A4[2] = {
    { lbl_9_data_1164, 2 },
    { lbl_9_data_119C, 1 },
};
char lbl_9_data_11B4[] = "k/KONNTORO-RUSUTEXIKKUh/TYOUSEI";
char lbl_9_data_11D4[] = "k/BAIBURE-SYONN";
char *lbl_9_data_11E4[2] = { lbl_9_data_11B4, lbl_9_data_11D4 };
void fn_9_5EEC(struct Sprite *sprite) {
    struct Sprite s;
    struct TextList *list;
    struct Panel *tex;
    s32 i28;
    struct Panel *panel;
    struct SpriteDrawRequest *req;

    tex = (struct Panel *)sprite->tex;
    panel = fn_9_BC84(sprite->tag);
    if (panel != NULL) {
        mtxa_from_translate_xyz(panel->x + panel->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0x200000;
    s.x = 248.0f;
    s.y = 46.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    strcpy(s.text, "k/KONNTORO-RA");
    draw_text(&s);
    req = &lbl_9_data_50;
    req->x = 110.0f;
    req->y = lbl_9_bss_84.panels[6].unkC;
    draw_sprite_draw_request(req);
    fn_9_D0(lbl_9_data_1090, 416.0f, 208.0f);
    for (i28 = 0; i28 < 2U; i28++) {
        s.x = 160.0f;
        s.y = (198.0 + (32.0 * i28));
        if ((s32) i28 == (s32) tex->sel2) {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
        } else {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
        }
        strcpy(s.text, lbl_9_data_11E4[i28]);
        draw_text(&s);
    }
    draw_sprite_draw_request(&lbl_9_data_F0);
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    list = &lbl_9_data_11A4[tex->sel2];
    for (i28 = 0; i28 < (s32) list->count; i28++) {
        s.x = (320.0 - (0.5 * (list->lines[i28].count * 0x18)));
        s.y = ((394.0 - (16.0 * (list->count - 1))) + (32.0 * i28));
        strcpy(s.text, list->lines[i28].text);
        draw_text(&s);
    }
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_64A8(void) {
    struct Panel *panel;
    struct Sprite *temp_r3_6848;

    if (get_sprite_with_unique_id(0x5E) != NULL) {
        if (lbl_9_bss_84.panels[6].state == 1) {
            lbl_9_bss_84.panels[6].state = 3;
        } else {
            lbl_9_bss_84.panels[6].state = 4;
        }
        return;
    }
    panel = &lbl_9_bss_84.panels[6];
    if (lbl_9_bss_84.panels[6].state == 1) {
        panel_slide_in_left(0x5E);
    } else {
        panel_slide_in_right(0x5E);
    }
    lbl_9_bss_84.panels[6].unkC = 206.0f;
    temp_r3_6848 = create_sprite();
    if (temp_r3_6848 != NULL) {
        temp_r3_6848->tag = 0x5E;
        temp_r3_6848->mainFunc = fn_9_5C60;
        temp_r3_6848->drawFunc = fn_9_5EEC;
        temp_r3_6848->tex = (struct SpriteTexture *)panel;
        strcpy(temp_r3_6848->text, "sound menu");
    }
}

void fn_9_6604(s8 *status, struct Sprite *sprite) {

    update_panel(sprite);
    lbl_9_bss_84.panels[8].unkC += 0.5 * ((176.0 + (32.0 * lbl_9_bss_84.panels[8].sel2)) - lbl_9_bss_84.panels[8].unkC);
}

struct SpriteDrawRequest lbl_9_data_11FC[1] = {
    { 0x408, 454.0f, 180.0f, 0.5f, 4.0f, 1.3333334f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.5f, -1, 10, -1, 0 },
};
void fn_9_688C(struct Sprite *sprite) {
    struct Sprite s;
    s32 i29;
    u32 temp_r28_7393;
    struct Panel *panel;
    struct SpriteDrawRequest *req;

    panel = fn_9_BC84(sprite->tag);
    if (panel != NULL) {
        mtxa_from_translate_xyz(panel->x + panel->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0x200000;
    s.x = 224.0f;
    s.y = 46.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    strcpy(s.text, lbl_9_data_11D4);
    draw_text(&s);
    req = &lbl_9_data_50;
    req->x = 164.0f;
    req->y = lbl_9_bss_84.panels[8].unkC;
    draw_sprite_draw_request(req);
    draw_sprite_draw_request(lbl_9_data_10E0);
    for (i29 = 0; i29 < 4; i29++) {
        s.x = 210.0f;
        s.y = (168.0 + (32.0 * i29));
        if (i29 == (s32) lbl_9_bss_84.panels[8].sel2) {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
        } else {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
        }
        sprintf(s.text, "k/KONNTORO-RA%d", i29 + 1);
        draw_text(&s);
    }
    req = lbl_9_data_11FC;
    req->y = (174.0 + (32.0 * lbl_9_bss_84.panels[8].sel2));
    draw_sprite_draw_request(req);
    s.x = 394.0f;
    s.y = (168.0 + (32.0 * lbl_9_bss_84.panels[8].sel2));
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    s.scaleX = 0.5f;
    strcpy(s.text, "p/SANNKAKU_L/a/        p/SANNKAKU_R/");
    draw_text(&s);
    s.scaleX = 1.0f;
    temp_r28_7393 = fn_1_FD63C();
    for (i29 = 0; i29 < 4; i29++) {
        if (i29 == (s32) lbl_9_bss_84.panels[8].sel2) {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
        } else {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
        }
        if ((temp_r28_7393 & (1 << i29)) != 0) {
            s.x = 430.0f;
            s.y = (168.0 + (32.0 * i29));
            sprintf(s.text, "a/ON");
        } else {
            s.x = 418.0f;
            s.y = (168.0 + (32.0 * i29));
            sprintf(s.text, "a/OFF");
        }
        draw_text(&s);
    }
    draw_sprite_draw_request(&lbl_9_data_F0);
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    s.x = 80.0f;
    s.y = 394.0f;
    sprintf(s.text, "k/KONNTORO-RA%dh/NOSINNDOUWOSETTEISIMASU.", lbl_9_bss_84.panels[8].sel2 + 1);
    draw_text(&s);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_712C(void) {
    struct Sprite *temp_r3_7652;

    if (get_sprite_with_unique_id(0x60) != NULL) {
        lbl_9_bss_84.panels[8].state = 4;
        return;
    }
    panel_slide_in_right(0x60);
    lbl_9_bss_84.panels[8].unkC = 176.0f;
    temp_r3_7652 = create_sprite();
    if (temp_r3_7652 != NULL) {
        temp_r3_7652->tag = 0x60;
        temp_r3_7652->mainFunc = fn_9_6604;
        temp_r3_7652->drawFunc = fn_9_688C;
        strcpy(temp_r3_7652->text, "sound menu");
    }
}

void fn_9_71F8(s8 *status, struct Sprite *sprite) {

    update_panel(sprite);
    lbl_9_bss_84.panels[9].unkC += 0.5 * ((176.0 + (32.0 * lbl_9_bss_84.panels[9].sel2)) - lbl_9_bss_84.panels[9].unkC);
}

struct SpriteDrawRequest lbl_9_data_12C0[1] = {
    { 0x5f, 320.0f, 408.0f, 5.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 10, -1, 0 },
};
struct TextLine lbl_9_data_1388[3] = {
    { "h/SUBETENOk/KONNTORO-RAh/NOk/KONNTORO-RUSUTEXIKKUh/WO", 23 },
    { "h/IPPAINIMAWASITEKUDASAI.", 13 },
    { "h/YOKEREBA WOp/OSU/h/SITEKUDASAI.", 14 },
};
struct TextLine lbl_9_data_13B0[3] = {
    { "", 0 },
    { "", 0 },
    { "    p/BUTTON_A/", 14 },
};
struct TextList lbl_9_data_13C8[1] = {
    { lbl_9_data_13B0, 3 },
};
struct TextLine lbl_9_data_13F0[2] = {
    { "h/KONOTYOUSEIDEYOROSIIDESUKA?", 15 },
    { "", 0 },
};
struct TextLine lbl_9_data_1458[2] = {
    { "h/KONNGO,KONOTYOUSEIWOp/SHIYOU/h/SIMASU.", 17 },
    { "h/NANIKAk/BOTANNh/WOp/OSU/h/SITEKUDASAI.", 14 },
};
struct TextLine lbl_9_data_148C[2] = {
    { "h/KONOTYOUSEIHAp/SHIYOU/h/SIMASENN.", 14 },
    { "h/NANIKAk/BOTANNh/WOp/OSU/h/SITEKUDASAI.", 14 },
};
struct TextLine lbl_9_data_1500[3] = {
    { "h/TYOUSEIGAKANNZENNDEHAARIMASENN.", 17 },
    { "h/SUBETENOk/KONNTORO-RAh/DE,", 11 },
    { "h/KITINNTONYUURYOKUSITEKUDASAI.", 16 },
};
struct TextList lbl_9_data_1518[5] = {
    { lbl_9_data_1388, 3 },
    { lbl_9_data_13F0, 2 },
    { lbl_9_data_1458, 2 },
    { lbl_9_data_148C, 2 },
    { lbl_9_data_1500, 3 },
};
void fn_9_7480(struct Sprite *sprite) {
    struct Sprite s;
    struct TextList *temp_r29_8011;
    s32 i28;
    u32 temp_r3_8289;
    struct Panel *panel;
    struct SpriteDrawRequest *temp_r3_7974;

    panel = fn_9_BC84(sprite->tag);
    if (panel != NULL) {
        mtxa_from_translate_xyz(panel->x + panel->y, 0.0f, 0.0f);
        mtxa_to_mtx(lbl_9_bss_84.mtx);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
    }
    draw_sprite_draw_request(&lbl_9_data_0);
    s.depth = 0.1f;
    s.scaleX = 1.0f;
    s.scaleY = 1.0f;
    s.fontId = 0x8E;
    s.addR = 0;
    s.addG = 0;
    s.addB = 0;
    s.flags = 0x200000;
    s.x = 128.0f;
    s.y = 46.0f;
    s.mulR = 0xFF;
    s.mulG = 0xFF;
    s.mulB = 0;
    strcpy(s.text, lbl_9_data_11B4);
    draw_text(&s);
    temp_r3_7974 = lbl_9_data_12C0;
    temp_r3_7974->x = 320.0f;
    temp_r3_7974->y = 408.0f;
    temp_r3_7974->scaleX = 1.42f;
    temp_r3_7974->scaleY = 0.6f;
    fn_9_D0(temp_r3_7974, 416.0f, 208.0f);
    if ((s32) lbl_9_bss_84.panels[9].unk1C == 1) {
        s.mulR = 0xEF;
        s.mulG = 0x9A;
        s.mulB = 0;
    } else {
        s.mulR = 0xFF;
        s.mulG = 0xFF;
        s.mulB = 0;
    }
    temp_r29_8011 = &lbl_9_data_1518[lbl_9_bss_84.panels[9].unk1C];
    for (i28 = 0; i28 < (s32) temp_r29_8011->count; i28++) {
        s.x = (320.0 - (0.5 * (temp_r29_8011->lines[i28].count * 0x18)));
        s.y = ((394.0 - (16.0 * (temp_r29_8011->count - 1))) + (32.0 * i28));
        strcpy(s.text, temp_r29_8011->lines[i28].text);
        draw_text(&s);
    }
    if (temp_r29_8011->lines == lbl_9_data_1388) {
        s.mulR = 0xFF;
        s.mulG = 0xFF;
        s.mulB = 0xFF;
        temp_r29_8011 = lbl_9_data_13C8;
        for (i28 = 0; i28 < (s32) temp_r29_8011->count; i28++) {
            s.x = (320.0 - (0.5 * (temp_r29_8011->lines[i28].count * 0x18)));
            s.y = ((394.0 - (16.0 * (temp_r29_8011->count - 1))) + (32.0 * i28));
            strcpy(s.text, temp_r29_8011->lines[i28].text);
            draw_text(&s);
        }
    }
    if ((s32) lbl_9_bss_84.panels[9].unk1C == 1) {
        temp_r3_8289 = 255.0 * ((f32) abs((s32) ((f32) (g_font_char_var_3 % 60) - 30.0)) / 30.0);
        s.x = 248.0f;
        s.y = 410.0f;
        if ((s32) lbl_9_bss_84.panels[9].sel2 == 0) {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
            s.addR = 0;
            s.addG = 0;
            s.addB = 0;
        } else {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
            s.addR = temp_r3_8289;
            s.addG = temp_r3_8289;
            s.addB = temp_r3_8289;
        }
        strcpy(s.text, lbl_9_data_40C);
        draw_text(&s);
        s.x = 248.0f;
        s.y = 410.0f;
        if ((s32) lbl_9_bss_84.panels[9].sel2 == 0) {
            s.mulR = 0xFF;
            s.mulG = 0xFF;
            s.mulB = 0;
            s.addR = temp_r3_8289;
            s.addG = temp_r3_8289;
            s.addB = temp_r3_8289;
        } else {
            s.mulR = 0x8F;
            s.mulG = 0x8F;
            s.mulB = 0;
            s.addR = 0;
            s.addG = 0;
            s.addB = 0;
        }
        strcpy(s.text, "h/   IIE");
        draw_text(&s);
    }
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_9_7E5C(void) {
    struct Sprite *temp_r3_8535;

    if (get_sprite_with_unique_id(0x61) != NULL) {
        lbl_9_bss_84.panels[9].state = 4;
        return;
    }
    panel_slide_in_right(0x61);
    lbl_9_bss_84.panels[9].unkC = 176.0f;
    temp_r3_8535 = create_sprite();
    if (temp_r3_8535 != NULL) {
        temp_r3_8535->tag = 0x61;
        temp_r3_8535->mainFunc = fn_9_71F8;
        temp_r3_8535->drawFunc = fn_9_7480;
        strcpy(temp_r3_8535->text, "sound menu");
    }
}

struct SpriteDrawRequest lbl_9_data_154C[4] = {
    { 0x5f, 3.2e+02f, 214.0f, 5.0f, 1.26f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xa, -1, 0 },
    { 0x408, 456.0f, 224.0f, 2.0f, 4.0f, 1.3333334f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.5f, -1, 0xa, -1, 0 },
    { 0x416, 3.2e+02f, 408.0f, 5.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xa, -1, 0 },
    { 0x416, 3.2e+02f, 378.0f, 0.5f, 1.08f, 1.8f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0x20000a, -1, 0 },
};
char *lbl_9_data_16CC[4] = {
    "k/RO-DOh/SURU",
    "k/SE-BUh/SURU",
    "k/O-TOSE-BU",
    "h/SAISHONIMODOSU",
};
struct TextLine lbl_9_data_1700[1] = {
    { "k/GE-MUDE-TAh/WOk/RO-DOh/SIMASU.", 13 },
};
struct TextLine lbl_9_data_176C[3] = {
    { "k/GE-MUDE-TAh/WOk/SE-BUh/SIMASU.", 13 },
    { "p/KOME/h/UWAGAKINOBAAIDEMO,", 12 },
    { "a/3k/BUROKKUh/NOAKIGAHITUYOUDESU.", 15 },
};
struct TextLine lbl_9_data_17A4[3] = {
    { "h/JIDOUDEk/SE-BUh/WOOKONAIMASU.", 14 },
    { "p/KOME/h/UWAGAKINOBAAIDEMO,", 12 },
    { "a/3k/BUROKKUh/NOAKIGAHITUYOUDESU.", 15 },
};
struct TextLine lbl_9_data_1810[2] = {
    { "k/GE-MUDE-TAh/WOSAISYONIMODOSIMASU.", 17 },
    { "p/KOME/k/MEMORI-KA-DOh/NIk/SE-BUh/HASAREMASENN.", 18 },
};
struct TextLine lbl_9_data_18A4[4] = {
    { "k/GE-MUPUREIh/SETTEIk/,RANNKINNGU,PUREIPOINNTO,", 24 },
    { "k/MINIGE-MU,KONNTEXINYU-p/KAISUU/h/GA", 16 },
    { "h/SUBETESAISYONIMODORIMASUGAYOROSIIDESUKA?", 21 },
    { "", 0 },
};
struct TextLine lbl_9_data_18EC[2] = {
    { "k/GE-MUDE-TAh/WOSAISYONIMODOSIMASITA.", 0x12 },
    { "h/NANIKAk/BOTANNh/WOp/OSU/h/SITEKUDASAI.", 0xE },
};
struct TextList lbl_9_data_18FC[6] = {
    { lbl_9_data_1700, 1 },
    { lbl_9_data_176C, 3 },
    { lbl_9_data_17A4, 3 },
    { lbl_9_data_1810, 2 },
    { lbl_9_data_18A4, 4 },
    { lbl_9_data_18EC, 2 },
};

static void unused_strings(void) {
    strcpy(0, "k/GE-MUDE-TA");
    strcpy(0, "p/SANNKAKU_L/        p/SANNKAKU_R/");
    strcpy(0, "OFF");
}

static void unused_literals(double *d, float *f) {
    *d = 156.0;
    *d = 1220.0;
    *d = 148.0;
    *f = 396.0f;
    *f = 212.0f;
    *f = 420.0f;
    *f = 392.0f;
    *f = 1.4f;
    *f = 544.0f;
    *f = 88.0f;
    *d = 378.0;
    *f = 1.15f;
    *d = 364.0;
    *d = 248.0;
    *f = 412.0f;
    *f = 122.0f;
    *f = 1220.0f;
}
