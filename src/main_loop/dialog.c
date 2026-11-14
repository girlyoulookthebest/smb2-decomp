#pragma unsigned_char on

#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "file.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "sprite.h"

#ifndef __MWERKS__
int pc_text_is_lead(unsigned char c); /* PC: language-aware SJIS lead test (textdraw.c) */
#endif

struct DialogData
{
    /*0x00*/ char * DPTR name;
    u8 unk4[2];
    /*0x06*/ u16 drawFrame;
    /*0x08*/ u16 mode;
    /*0x0A*/ s16 offsetX;
    /*0x0C*/ s16 offsetY;
    /*0x0E*/ s16 depth;
    /*0x10*/ u16 tailMode;
    u8 unk12[2];
    /*0x14*/ float unk14;
    /*0x18*/ u16 unk18;
    /*0x1A*/ u16 unk1A;
    /*0x1C*/ u16 unk1C;
    /*0x1E*/ u16 width;
    /*0x20*/ u16 numLines;
    u8 unk22[2];
    /*0x24*/ float scaleX;
    /*0x28*/ float scaleY;
    /*0x2C*/ char * DPTR text;
};

struct DialogTbl
{
    /*0x00*/ s32 count;
    /*0x04*/ struct DialogData * DPTR * DPTR entries;
};

/* One glyph queued for drawing. */
struct TextEntry
{
    /*0x00*/ char *text;
    /*0x04*/ u16 x;
    /*0x06*/ u8 line;
    /*0x07*/ u8 color;
    /*0x08*/ float scale;
};  /* size = 0xC */

struct TextBox
{
    /*0x00*/ s32 drawFrame;
    /*0x04*/ s32 numLines;
    /*0x08*/ struct Sprite * DPTR sprite;
    /*0x0C*/ int heap;
    /*0x10*/ Vec pos;
    /*0x1C*/ float width;
    /*0x20*/ float height;
    /*0x24*/ float scaleX;
    /*0x28*/ float scaleY;
    /*0x2C*/ s32 tailMode;
    /*0x30*/ Vec tail;
    /*0x3C*/ Vec unk3C;
    /*0x48*/ float unk48;
    /*0x4C*/ float unk4C;
    /*0x50*/ s16 rotZ;
    /*0x52*/ s16 unk52;
    /*0x54*/ GXColor * DPTR colors;
    /*0x58*/ s32 colorCount;
    /*0x5C*/ float timer;
    /*0x60*/ float timerTarget;
    /*0x64*/ s32 pending;
    /*0x68*/ s32 curX;
    /*0x6C*/ float scroll;
    /*0x70*/ u16 scrollTarget;
    /*0x72*/ u16 line;
    /*0x74*/ u16 color;
    /*0x76*/ u16 textScale;
    /*0x78*/ u16 capacity;
    /*0x7A*/ u16 head;
    /*0x7C*/ u16 next;
    /*0x80*/ struct TextEntry * DPTR entries;
};  /* size = 0x84 */

/* Sprite.userVar is an s32 holding the TextBox address.  The box lives in a
 * game heap (GC RAM, below 4 GiB on x64), so read it back zero-extended: a
 * plain (void *) cast would sign-extend 0x8xxxxxxx on 64-bit hosts. */
#define TEXTBOX_FROM_USERVAR(v) ((struct TextBox *)(uptr)(u32)(v))

struct Dialog
{
    /*0x00*/ s32 frame;
    /*0x04*/ s16 rotVel;
    /*0x08*/ u32 flags;
    /*0x0C*/ struct Ape * DPTR ape;
    /*0x10*/ struct TextBox box;
    /*0x94*/ struct DialogData * DPTR data;
    /*0x98*/ char * DPTR cursor;
};

struct TextBoxFrame
{
    /*0x00*/ s32 unk0;
    /*0x04*/ float x0;
    /*0x08*/ float y0;
    /*0x0C*/ float x1;
    /*0x10*/ float y1;
    /*0x14*/ float z;
    /*0x18*/ float tailX;
    /*0x1C*/ float tailY;
    /*0x20*/ float tailWidth;
    /*0x24*/ GXColor color;
};  /* size = 0x28 */

struct ScreenRect
{
    /*0x00*/ float width;
    /*0x04*/ float height;
    /*0x08*/ float scaleW;
    /*0x0C*/ float scaleH;
    /*0x10*/ float minX;
    /*0x14*/ float minY;
    /*0x18*/ float maxX;
    /*0x1C*/ float maxY;
    /*0x20*/ float centerX;
    /*0x24*/ float centerY;
};  /* size = 0x28 */

double atof(const char *s);
void DVDChangeDir(char *path);
void GXSetTevColor(int id, GXColor color);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
float g_get_string_sprite_width_2(char *str);
void fn_800668FC(void);
void g_something_with_GX_vtx_desc(int a);
void GXSetNumChans_cached(int a);
void GXSetNumTexGens_cached(int a);
void GXSetNumTevStages_cached(int a);
void g_GXSetTevIndirect_zero_if_different(int a);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int a, int b);
void GXSetTexCoordGen2_cached(int a, int b, int c, int d, int e, int f);
void g_GXSetTevOrder_wrapper(int a, int b, int c, int d);
void GXSetTevSwapMode_cached(int a, int b, int c);
void GXSetTevColorIn_cached(int a, int b, int c, int d, int e);
void GXSetTevColorOp_cached(int a, int b, int c, int d, int e, int f);
void GXSetTevAlphaIn_cached(int a, int b, int c, int d, int e);
void GXSetTevAlphaOp_cached(int a, int b, int c, int d, int e, int f);
void g_something_with_raster_state(int a);
void GXSetCullMode_cached(int a);

extern int locale;
extern float lbl_1_bss_6FB7C;
extern float lbl_1_bss_6FB80;
extern float lbl_1_bss_6FB84;

void fn_1_160D48(struct TextBox *box);
void fn_1_160ECC(struct TextBox *box);
char *fn_1_16133C(struct TextBox *box, char *p);
char *fn_1_1615BC(struct TextBox *box, char *p);
int fn_1_161944(struct TextBox *box, char *p, int maxLines);
int fn_1_161E64(struct TextBox *box, char *p);
void fn_1_162020(struct TextBox *box, GXColor *colors, int count);
void fn_1_16202C(struct TextBoxFrame *frame);
void fn_1_162260(struct TextBoxFrame *frame);
void fn_1_162524(s8 *a, struct Sprite *sprite);
void fn_1_162560(struct Sprite *sprite);
void fn_1_162584(struct TextBox *box);
float fn_1_1625B0(struct TextBox *box);
void fn_1_1625B8(struct TextBox *box);
int fn_1_1625F4(struct Dialog *dlg, Vec *out);
void fn_1_162A58(struct ScreenRect *out);

/* Not referenced by any code in this file. */
u8 lbl_1_data_70A10[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

static GXColor lbl_1_data_70B80[9] = {
    {0x00, 0x00, 0x00, 0xFF},
    {0x00, 0x00, 0xFF, 0xFF},
    {0x00, 0xFF, 0x00, 0xFF},
    {0x00, 0xFF, 0xFF, 0xFF},
    {0xFF, 0x00, 0x00, 0xFF},
    {0xFF, 0x00, 0xFF, 0xFF},
    {0xFF, 0xFF, 0x00, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xC0, 0x00, 0xFF},
};

static char *scene_filenames[] = {
    "scnall",
    "scn1a",
    "scn1b",
    "scn2",
    "scn3",
    "scn4",
    "scn5",
    "scn6",
    "scn7",
    "scn8",
    "scn9",
    "scn10",
    "scnlast",
    "scnend1",
    "scnend2",
    "scnend3",
    "scnend4",
};

char *lbl_1_data_70D98[] = {
    "DIA_CTGR_SCN_ALL",
    "DIA_CTGR_SCN1_A",
    "DIA_CTGR_SCN1_B",
    "DIA_CTGR_SCN2",
    "DIA_CTGR_SCN3",
    "DIA_CTGR_SCN4",
    "DIA_CTGR_SCN5",
    "DIA_CTGR_SCN6",
    "DIA_CTGR_SCN7",
    "DIA_CTGR_SCN8",
    "DIA_CTGR_SCN9",
    "DIA_CTGR_SCN10",
    "DIA_CTGR_SCN_LAST",
    "DIA_CTGR_SCN_END1",
    "DIA_CTGR_SCN_END2",
    "DIA_CTGR_SCN_END3",
    "DIA_CTGR_SCN_END4",
    NULL,
};

static char *locale_suffixes[] = {
    "_e",
    "_g",
    "_f",
    "_s",
    "_i",
    "",
};

void fn_1_15FB94(struct Dialog *dlg)
{
    struct ScreenRect rect;
    Vec prevTail;
    Vec target;
    Vec prevPos;
    Vec tmp;
    Vec a;
    Vec b;
    float halfH;
    float cx;
    float cy;
    float halfW;
    float maxY;
    float maxX;
    float minY;
    float minX;
    float x0, y0, x1, y1;
    float len;
    float t;
    struct DialogData *data;
    struct TextBox *box;
    int hasTarget;

    if (g_some_other_flags & 0xA)
        return;
    box = &dlg->box;
    data = dlg->data;
    if (data == NULL)
        return;
    fn_1_162A58(&rect);
    minX = rect.minX;
    minY = rect.minY;
    maxX = rect.maxX;
    maxY = rect.maxY;
    cx = rect.centerX;
    cy = rect.centerY;
    if (dlg->flags & 2)
    {
        box->scaleX = box->scaleX + 0.1f * (data->scaleX - box->scaleX);
        box->scaleY = box->scaleY + 0.1f * (data->scaleY - box->scaleY);
    }
    else
    {
        box->scaleX = box->scaleX + 0.15f * (-0.01f - box->scaleX);
        box->scaleY = box->scaleY + 0.15f * (-0.01f - box->scaleY);
    }
    box->unk48 = box->unk48 + 0.1f * (1.0f - box->unk48);
    box->unk4C = box->unk4C + 0.1f * (1.0f - box->unk4C);
    halfW = 0.5f * (box->width * box->scaleX);
    halfH = 0.5f * (box->height * box->scaleY);
    if (!(dlg->flags & 1))
    {
        dlg->frame++;
        len = 0.016666668f * (float)dlg->frame;
    }
    if (fn_1_1625B0(&dlg->box) < len)
        dlg->cursor = fn_1_16133C(&dlg->box, dlg->cursor);
    hasTarget = fn_1_1625F4(dlg, &target);
    prevTail = box->tail;
    switch (data->tailMode)
    {
    case 0:
        box->tail = box->pos;
        if (hasTarget != 0 && target.z < 0.0f)
            box->tailMode = data->tailMode;
        break;
    default:
        if (hasTarget == 0 || target.z > 0.0f)
        {
            hasTarget = 0;
            box->tailMode = 0;
            box->tail = box->pos;
        }
        else
        {
            box->tailMode = data->tailMode;
            if (target.x < minX)
            {
                t = (minX - cx) / (target.x - cx);
                target.x = minX;
                target.y = cy + t * (target.y - cy);
            }
            else if (target.x > maxX)
            {
                t = (maxX - cx) / (target.x - cx);
                target.x = maxX;
                target.y = cy + t * (target.y - cy);
            }
            if (target.y < minY)
            {
                t = (minY - cy) / (target.y - cy);
                target.x = cx + t * (target.x - cx);
                target.y = minY;
            }
            else if (target.y > maxY)
            {
                t = (maxY - cy) / (target.y - cy);
                target.x = cx + t * (target.x - cx);
                target.y = maxY;
            }
            box->tail.x = box->tail.x + 0.4f * (target.x - box->tail.x);
            box->tail.y = box->tail.y + 0.4f * (target.y - box->tail.y);
            mtxa_from_rotate_z(box->rotZ);
            if (0.0f == halfW && 0.0f == halfH)
            {
                target.x = 0.0f;
                target.y = 0.0f;
            }
            else
            {
                int clipped;

                mtxa_from_rotate_z(box->rotZ);
                target.x -= box->pos.x;
                target.y -= box->pos.y;
                target.z = 0.0f;
                mtxa_rigid_inv_tf_vec(&target, &target);
                tmp = target;
                len = mathutil_vec_len(&tmp);
                x0 = -halfW;
                y0 = -halfH;
                x1 = halfW;
                y1 = halfH;
                clipped = 0;
                if (tmp.x < x0)
                {
                    t = x0 / tmp.x;
                    tmp.x = x0;
                    tmp.y *= t;
                    clipped = 1;
                }
                else if (tmp.x > x1)
                {
                    t = x1 / tmp.x;
                    tmp.x = x1;
                    tmp.y *= t;
                    clipped = 1;
                }
                if (tmp.y < y0)
                {
                    t = y0 / tmp.y;
                    tmp.x *= t;
                    tmp.y = y0;
                    clipped = 1;
                }
                else if (tmp.y > y1)
                {
                    t = y1 / tmp.y;
                    tmp.x *= t;
                    tmp.y = y1;
                    clipped = 1;
                }
                if (clipped != 0)
                {
                    t = mathutil_vec_len(&tmp);
                    if (len - t > data->unk14)
                    {
                        t = (t + data->unk14) / t;
                        tmp.x *= t;
                        tmp.y *= t;
                        target = tmp;
                    }
                }
            }
            box->unk3C.x = box->unk3C.x + 0.3f * (target.x - box->unk3C.x);
            box->unk3C.y = box->unk3C.y + 0.3f * (target.y - box->unk3C.y);
        }
        break;
    }
    prevPos = box->pos;
    switch (data->mode)
    {
    case 1:
    {
        if (hasTarget != 0)
        {
            target.x = box->tail.x + (float)data->offsetX;
            target.y = box->tail.y + (float)data->offsetY;
        }
        else
        {
            target.x = box->pos.x;
            target.y = box->pos.y;
        }
        x0 = minX + halfW;
        x1 = maxX - halfW;
        y0 = minY + halfH;
        y1 = maxY - halfH;
        if (target.x < x0)
        {
            t = (x0 - cx) / (target.x - cx);
            target.x = x0;
            target.y = cy + t * (target.y - cy);
        }
        else if (target.x > x1)
        {
            t = (x1 - cx) / (target.x - cx);
            target.x = x1;
            target.y = cy + t * (target.y - cy);
        }
        if (target.y < y0)
        {
            t = (y0 - cy) / (target.y - cy);
            target.x = cx + t * (target.x - cx);
            target.y = y0;
        }
        else if (target.y > y1)
        {
            t = (y1 - cy) / (target.y - cy);
            target.x = cx + t * (target.x - cx);
            target.y = y1;
        }
        box->pos.x = box->pos.x + 0.15f * (target.x - box->pos.x);
        box->pos.y = box->pos.y + 0.15f * (target.y - box->pos.y);
        break;
    }
    }
    switch (data->unk1C)
    {
    case 1:
        if (hasTarget != 0)
        {
            a.x = prevTail.x - prevPos.x;
            a.y = prevTail.y - prevPos.y;
            a.z = 0.0f;
            b.x = box->tail.x - box->pos.x;
            b.y = box->tail.y - box->pos.y;
            b.z = 0.0f;
            t = vec_dot_normalized_safe(&a, &b);
            {
                s16 angle = math_atan2(math_sqrt(1.0f - t * t), t);

                dlg->rotVel = dlg->rotVel + (int)(0.125f * (float)angle);
            }
        }
        break;
    }
    dlg->rotVel += (s16)(1024.0f * math_sin(-box->rotZ));
    dlg->rotVel = (s16)(0.92f * (float)dlg->rotVel);
    box->rotZ += dlg->rotVel;
}

void fn_1_160518(struct Dialog *dlg, struct DialogData *data)
{
    Vec pos;
    struct ScreenRect rect;
    struct TextBox *box = &dlg->box;
    float minX, minY, maxX, maxY, cx, cy;
    float t;

    fn_1_162584(box);
    fn_1_1625B8(box);
    dlg->frame = 0;
    dlg->rotVel = 0;
    dlg->flags = 0;
    dlg->ape = NULL;
    dlg->data = data;
    dlg->cursor = data->text;
    box->drawFrame = data->drawFrame;
    if (data->numLines != 0)
        box->numLines = data->numLines;
    box->width = (float)data->width;
    box->height = 28.0f * (float)box->numLines;
    box->scaleX = data->scaleX;
    box->scaleY = data->scaleY;
    pos = box->pos;
    fn_1_162A58(&rect);
    minX = rect.minX;
    minY = rect.minY;
    maxX = rect.maxX;
    maxY = rect.maxY;
    cx = rect.centerX;
    cy = rect.centerY;
    pos.x *= rect.width / 640.0f;
    pos.y *= rect.height / 480.0f;
    if (pos.x < minX)
    {
        t = (minX - cx) / (pos.x - cx);
        pos.x = minX;
        pos.y = cy + t * (pos.y - cy);
    }
    else if (pos.x > maxX)
    {
        t = (maxX - cx) / (pos.x - cx);
        pos.x = maxX;
        pos.y = cy + t * (pos.y - cy);
    }
    if (pos.y < minY)
    {
        t = (minY - cy) / (pos.y - cy);
        pos.x = cx + t * (pos.x - cx);
        pos.y = minY;
    }
    else if (pos.y > maxY)
    {
        t = (maxY - cy) / (pos.y - cy);
        pos.x = cx + t * (pos.x - cx);
        pos.y = maxY;
    }
    box->pos = pos;
    box->tailMode = data->tailMode;
    box->scaleX = 0.0f;
    box->scaleY = 0.0f;
    box->unk48 = 0.0f;
    box->unk4C = 0.0f;
    if (!box->width)
        box->width = (float)fn_1_161944(&dlg->box, dlg->cursor, 0x1000);
}

void fn_1_1607DC(struct Dialog *dlg, struct Ape *ape)
{
    dlg->ape = ape;
}

void fn_1_1607E4(struct Dialog *dlg)
{
    dlg->flags &= ~1;
}

void fn_1_1607F4(struct Dialog *dlg, int a)
{
    struct DialogData *data = dlg->data;
    struct TextBox *box = &dlg->box;

    if (data != NULL)
    {
        switch (data->unk1A)
        {
        case 0:
            a = 1;
            break;
        }
        dlg->flags &= ~2;
        if (a != 0)
        {
            box->scaleX = 0.0f;
            box->scaleY = 0.0f;
            box->unk48 = 0.0f;
            box->unk4C = 0.0f;
        }
    }
}

void fn_1_160870(struct Dialog *dlg)
{
    struct ScreenRect rect2;
    struct ScreenRect rect;
    Vec v;
    struct DialogData *data = dlg->data;
    struct TextBox *box = &dlg->box;
    int visible;
    float cx, cy;

    dlg->flags |= 2;
    visible = box->scaleX > 0.0f && box->scaleY > 0.0f && box->unk48 > 0.0f && box->unk4C > 0.0f;
    box->pos.z = 0.1 + 0.01 * data->depth;
    switch (data->mode)
    {
    case 0:
        fn_1_162A58(&rect2);
        box->pos.x = (float)data->offsetX * (rect2.width / 640.0f);
        box->pos.y = (float)data->offsetY * (rect2.height / 480.0f);
        box->tail = box->pos;
        box->unk3C.x = 0.0f;
        box->unk3C.y = 0.0f;
        box->unk3C.z = box->pos.z;
        break;
    case 1:
        if (visible)
        {
            fn_1_162A58(&rect);
            cx = rect.centerX;
            cy = rect.centerY;
            if (fn_1_1625F4(dlg, &v) == 0)
            {
                v.x = cx;
                v.y = cy;
                box->tail = box->pos;
            }
            else
            {
                box->pos.x = v.x + (float)data->offsetX;
                box->pos.y = v.y + (float)data->offsetY;
                box->tail.x = v.x;
                box->tail.y = v.y;
            }
            box->unk3C.x = 0.0f;
            box->unk3C.y = 0.0f;
            box->unk3C.z = box->pos.z;
        }
        break;
    }
    switch (data->unk18)
    {
    case 0:
        box->scaleX = data->scaleX;
        box->scaleY = data->scaleY;
        box->unk48 = 1.0f;
        box->unk4C = 1.0f;
        break;
    case 1:
        if (visible)
        {
            box->scaleX = 0.0f;
            box->scaleY = 0.0f;
            box->unk48 = 0.0f;
            box->unk4C = 0.0f;
        }
        break;
    }
    box->rotZ = 0;
}

void fn_1_160B60(struct TextBox *box, u32 capacity)
{
    int heap = currentHeap;
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 0x32;
        memset(box, 0, sizeof(*box));
        box->drawFrame = 0;
        box->numLines = 3;
        box->sprite = sprite;
        box->heap = heap;
        box->pos.x = 320.0f;
        box->pos.y = 240.0f;
        box->pos.z = 0.1f;
        box->width = 512.0f;
        box->height = 28.0f * (float)box->numLines;
        box->scaleX = 1.0f;
        box->scaleY = 1.0f;
        box->tailMode = 0;
        box->tail = box->pos;
        box->unk3C = box->tail;
        box->unk48 = 1.0f;
        box->unk4C = 1.0f;
        box->rotZ = 0;
        fn_1_162020(box, lbl_1_data_70B80, 9);
        box->capacity = capacity;
        box->head = 0;
        box->next = 0;
        box->entries = alloc_from_heap_or_panic(heap, capacity * sizeof(struct TextEntry), "dialog.c", 911);
        sprite->depth = box->pos.z;
        sprite->mainFunc = fn_1_162524;
        sprite->drawFunc = fn_1_162560;
        sprite->userVar = (sptr)box;
    }
}

void fn_1_160D00(struct TextBox *box)
{
    int heap = box->heap;

    if (box->entries != NULL)
    {
        OSFreeToHeap(heap, box->entries);
        box->entries = NULL;
    }
}

void fn_1_160D48(struct TextBox *box)
{
    int lines;
    float scroll;
    int n;
    struct TextEntry *e;
    int count;
    int i;
    int cap;

    lines = (float)box->line - box->scroll;
    if (lines >= box->numLines)
        box->scrollTarget = box->line - box->numLines + 1;
    scroll = (float)box->scrollTarget;
    if (scroll > box->scroll)
    {
        box->scroll += 0.13333334f;
        if (scroll < box->scroll)
            box->scroll = scroll;
    }
    cap = box->capacity;
    count = box->next - box->head;
    if (count < 0)
        count += cap;
    i = box->head;
    e = &box->entries[i];
    for (n = count; n > 0; n--)
    {
        if (e->scale < 1.0f)
        {
            e->scale += 0.2f;
            if (e->scale > 1.0f)
                e->scale = 1.0f;
        }
        else if (e->scale > 1.0f)
        {
            e->scale -= 0.2f;
            if (e->scale < 1.0f)
                e->scale = 1.0f;
        }
        if (i >= cap - 1)
        {
            i = 0;
            e = box->entries;
        }
        else
        {
            i++;
            e++;
        }
    }
}

void fn_1_160ECC(struct TextBox *box)
{
    Mtx mtx;
    struct ScreenRect rect;
    struct TextBoxFrame frame;
    Vec pos;
    char str[3];
    GXColor color;
    float tailX, tailY;
    float x, y, scale;
    int n;
    struct TextEntry *e;
    int line;
    int count;
    int i;
    int cap;
    int colorIdx;

    if (box->scaleX <= 0.0f || box->scaleY <= 0.0f || box->unk48 <= 0.0f || box->unk4C <= 0.0f)
        return;
    mtxa_from_identity();
    if (modeCtrl.wideMode != 0)
    {
        fn_1_162A58(&rect);
        mtxa_scale_xyz(640.0f / rect.width, 480.0f / rect.height, 1.0f);
    }
    mtxa_translate(&box->tail);
    mtxa_rotate_z(box->unk52);
    mtxa_scale_xyz(box->unk48, box->unk4C, 1.0f);
    mtxa_translate_neg(&box->tail);
    mtxa_to_mtx(mtx);
    mtxa_tf_point(&box->pos, &pos);
    mtxa_from_translate(&pos);
    mtxa_rotate_z(box->rotZ);
    mtxa_scale_xyz(box->scaleX, box->scaleY, 1.0f);
    mtxa_translate_neg(&pos);
    mtxa_mult_right(mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    if (box->drawFrame != 0)
    {
        tailX = box->unk3C.x + box->pos.x;
        tailY = box->unk3C.y + box->pos.y;
        frame.unk0 = 0;
        frame.x0 = box->pos.x - 0.5f * box->width;
        frame.y0 = box->pos.y - 0.5f * box->height;
        frame.x1 = frame.x0 + box->width;
        frame.y1 = frame.y0 + box->height;
        frame.z = box->pos.z;
        frame.tailX = tailX;
        frame.tailY = tailY;
        switch (box->tailMode)
        {
        case 0:
            frame.tailWidth = 0.0f;
            break;
        default:
            frame.tailWidth = 0.05f * box->width;
            break;
        }
        frame.color.r = 0xFF;
        frame.color.g = 0xFF;
        frame.color.b = 0xFF;
        frame.color.a = 0x80;
        fn_1_16202C(&frame);
    }
    textdraw_reset();
    textdraw_set_depth(box->pos.z);
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    textdraw_set_mul_color(0);
    if (box->drawFrame != 0)
        GXSetZMode_cached(1, 2, 0);
    x = box->pos.x - 0.5f * box->width;
    y = box->pos.y - 0.5f * box->height - 28.0f * box->scroll;
    line = box->scroll;
    cap = box->capacity;
    count = box->next - box->head;
    if (count < 0)
        count += cap;
    colorIdx = -1;
    i = box->head;
    e = &box->entries[i];
    for (n = count; n > 0; n--)
    {
        scale = e->scale;
        if (scale > 0.0f && e->line >= line)
        {
            textdraw_set_pos(x + (float)e->x, y + 28.0f * ((float)e->line + 0.8f * (1.0f - scale)));
            str[0] = e->text[0];
#ifdef __MWERKS__
            if (!(str[0] & 0x80))
#else
            if (!pc_text_is_lead(str[0]))
#endif
            {
                str[1] = 0;
            }
            else
            {
                str[1] = e->text[1];
                str[2] = 0;
            }
            if (e->color != colorIdx)
            {
                colorIdx = e->color;
                color = box->colors[colorIdx];
                textdraw_set_mul_color(color.b | ((color.r << 16) | (color.g << 8)));
            }
            textdraw_set_scale(scale, scale);
            textdraw_print(str);
        }
        if (i >= cap - 1)
        {
            i = 0;
            e = box->entries;
        }
        else
        {
            i++;
            e++;
        }
    }
    g_some_sprite_reset_gx_state_func();
    mtxa_from_identity();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
}

char *fn_1_16133C(struct TextBox *box, char *p)
{
    char str[3];
    struct TextEntry *e;
    char c = *p;
    int n;

    str[2] = 0;
    if (box->pending == 0)
        fn_1_161E64(box, p);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    if (c == 0)
        return p;
    switch (c)
    {
    case '\\':
        p = fn_1_1615BC(box, p + 1);
        break;
    case '\n':
        box->line++;
        box->curX = 0;
        p++;
        break;
    default:
        e = &box->entries[box->next];
        box->next++;
        if ((u16)box->next >= box->capacity)
            box->next = 0;
        if ((u16)box->head == box->next)
        {
            box->head++;
            if ((u16)box->head >= box->capacity)
                box->head = 0;
        }
        e->text = p;
#ifdef __MWERKS__
        if (!(c & 0x80))
#else
        if (!pc_text_is_lead(c))
#endif
        {
            str[0] = c;
            str[1] = 0;
            p++;
        }
        else
        {
            str[0] = c;
            str[1] = p[1];
            p += 2;
        }
        e->x = box->curX;
        e->line = box->line;
        e->color = box->color;
        switch (box->textScale)
        {
        case 0:
            e->scale = 1.0f;
            break;
        case 1:
            e->scale = 0.25f;
            break;
        case 2:
            e->scale = 2.0f;
            break;
        }
        box->curX += (u32)g_get_string_sprite_width_2(str);
        n = box->pending - 1;
        if (n > 0)
        {
            float k = 1.0f / (float)(n + 1);

            box->timer = box->timer + k * (box->timerTarget - box->timer);
        }
        else
        {
            n = 0;
            box->timer = box->timerTarget;
        }
        box->pending = n;
        break;
    }
    return p;
}

char *fn_1_1615BC(struct TextBox *box, char *p)
{
    int n;

    switch ((u8)*p)
    {
    case 'c':
        p++;
        n = 0;
        while (*p >= '0' && *p <= '9')
        {
            if (n != 0)
                n *= 10;
            n += *p - '0';
            p++;
        }
        if (box != NULL)
        {
            box->color = n;
            if (box->color >= box->colorCount)
                box->color = box->colorCount - 1;
        }
        break;
    case 't':
        p++;
        atof(p);
        while ((*p >= '0' && *p <= '9') || *p == '.')
            p++;
        break;
    case 'l':
        p++;
        fn_1_1625B8(box);
        break;
    case 's':
        p++;
        while ((*p >= '0' && *p <= '9') || *p == '.')
            p++;
        break;
    case 'p':
        p++;
        n = 0;
        while (*p >= '0' && *p <= '9')
        {
            if (n != 0)
                n *= 10;
            n += *p - '0';
            p++;
        }
        if (box != NULL)
            box->textScale = n;
        break;
    }
    return p;
}

char *fn_1_161754(char *p)
{
    switch ((u8)*p)
    {
    case 'c':
    case 'p':
        p++;
        while (*p >= '0' && *p <= '9')
            p++;
        break;
    case 't':
        p++;
        while ((*p >= '0' && *p <= '9') || *p == '.')
            p++;
        break;
    case 'l':
        p++;
        break;
    case 's':
        p++;
        while ((*p >= '0' && *p <= '9') || *p == '.')
            p++;
        break;
    }
    return p;
}

int fn_1_161818(char *p)
{
    int n = 0;
    char c;

    while ((c = *p) != 0)
    {
        switch (c)
        {
        case '\\':
            p = fn_1_161754(p + 1);
            break;
        case '\n':
            p++;
            break;
        default:
            n++;
#ifdef __MWERKS__
            if (!(c & 0x80))
#else
            if (!pc_text_is_lead(c))
#endif
                p++;
            else
                p += 2;
            break;
        }
    }
    return n;
}

static inline int is_line_end(char *p, char **out)
{
    int c = *p;
    int ret;

    if (c == 0)
    {
        if (out != NULL)
            *out = p;
        return 1;
    }
    ret = 0;
    p++;
    switch (c)
    {
    case '\n':
        ret = 1;
        break;
    case '\\':
        c = *p;
        p++;
        switch (c)
        {
        case 'l':
            ret = 1;
            break;
        }
        break;
    }
    if (out != NULL)
        *out = p;
    return ret;
}

int fn_1_161944(struct TextBox *box, char *p, int n)
{
    char str[3];
    char c;
    int n2 = n;
    int w1;
    char *s2;
    char *s1 = p;
    int maxW;
    int w2;

    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    w1 = box->curX;
    {
        char str2[3];

        str2[2] = 0;
        while (s1 != NULL && (c = *s1) != 0)
        {
            if (is_line_end(s1, NULL))
            {
                s1 = NULL;
                continue;
            }
            switch (c)
            {
            case '\\':
                s1++;
                switch ((u8)*s1)
                {
                case 'c':
                case 'p':
                    s1++;
                    while (*s1 >= '0' && *s1 <= '9')
                        s1++;
                    break;
                case 't':
                    s1++;
                    while ((*s1 >= '0' && *s1 <= '9') || *s1 == '.')
                        s1++;
                    break;
                case 's':
                    s1++;
                    while ((*s1 >= '0' && *s1 <= '9') || *s1 == '.')
                        s1++;
                    break;
                }
                break;
            case '\n':
                s1 = NULL;
                break;
            default:
#ifdef __MWERKS__
                if (!(c & 0x80))
#else
                if (!pc_text_is_lead(c))
#endif
                {
                    str2[0] = c;
                    str2[1] = 0;
                    s1++;
                }
                else
                {
                    str2[0] = c;
                    str2[1] = s1[1];
                    s1 += 2;
                }
                w1 += (u32)g_get_string_sprite_width_2(str2);
                break;
            }
        }
    }
    maxW = w1;
    while (!is_line_end(p, &p))
        ;
    for (n2--; n2 > 0; n2--)
    {
        char *q = p;

        w2 = 0;
        s2 = q;
        str[2] = 0;
        while (s2 != NULL && (c = *s2) != 0)
        {
            if (is_line_end(s2, NULL))
            {
                s2 = NULL;
                continue;
            }
            switch (c)
            {
            case '\\':
                s2++;
                switch ((u8)*s2)
                {
                case 'c':
                case 'p':
                    s2++;
                    while (*s2 >= '0' && *s2 <= '9')
                        s2++;
                    break;
                case 't':
                    s2++;
                    while ((*s2 >= '0' && *s2 <= '9') || *s2 == '.')
                        s2++;
                    break;
                case 's':
                    s2++;
                    while ((*s2 >= '0' && *s2 <= '9') || *s2 == '.')
                        s2++;
                    break;
                }
                break;
            case '\n':
                s2 = NULL;
                break;
            default:
#ifdef __MWERKS__
                if (!(c & 0x80))
#else
                if (!pc_text_is_lead(c))
#endif
                {
                    str[0] = c;
                    str[1] = 0;
                    s2++;
                }
                else
                {
                    str[0] = c;
                    str[1] = s2[1];
                    s2 += 2;
                }
                w2 += (u32)g_get_string_sprite_width_2(str);
                break;
            }
        }
        if (maxW < w2)
            maxW = w2;
        while (!is_line_end(p, &p))
            ;
        if (*p == 0)
            break;
    }
    return maxW;
}

int fn_1_161E64(struct TextBox *box, char *p)
{
    int found = 0;
    int n = 0;
    float time;
    char c;

    while ((c = *p) != 0 && found == 0)
    {
        switch (c)
        {
        case '\\':
            if (p[1] != 't')
            {
                p = fn_1_161754(p + 1);
            }
            else
            {
                found = 1;
                time = atof(p + 2);
            }
            break;
        case '\n':
            p++;
            break;
        default:
            n++;
#ifdef __MWERKS__
            if (!(c & 0x80))
#else
            if (!pc_text_is_lead(c))
#endif
                p++;
            else
                p += 2;
            break;
        }
    }
    if (found != 0)
        box->timerTarget = time;
    box->pending = n;
    if (n == 0)
        box->timer = box->timerTarget;
    return found;
}

void fn_1_162020(struct TextBox *box, GXColor *colors, int count)
{
    box->colors = colors;
    box->colorCount = count;
}

void fn_1_16202C(struct TextBoxFrame *frame)
{
    float x0, y0, x1, y1;
    float w, h;
    float cw, ch;

    if (frame->tailWidth > 0.0f)
        fn_1_162260(frame);
    textdraw_reset();
    textdraw_set_font(0x15);
    textdraw_set_alpha(0.003921569f * (float)frame->color.a);
    textdraw_set_depth(frame->z);
    textdraw_set_mul_color(frame->color.b | ((frame->color.r << 16) | (frame->color.r << 8)));
    cw = 24.0f;
    ch = 24.0f;
    x0 = frame->x0;
    y0 = frame->y0;
    x1 = frame->x1;
    y1 = frame->y1;
    w = x1 - x0;
    h = y1 - y0;
    textdraw_set_pos(x0 - cw, y0 - ch);
    textdraw_print("\x01");
    textdraw_set_pos(x1, y0 - ch);
    textdraw_print("\x03");
    textdraw_set_pos(x0 - cw, y1);
    textdraw_print("\x0b");
    textdraw_set_pos(x1, y1);
    textdraw_print("\x0d");
    textdraw_set_scale(w / cw, 1.0f);
    textdraw_set_pos(x0, y0 - ch);
    textdraw_print("\x02");
    textdraw_set_pos(x0, y1);
    textdraw_print("\x0c");
    textdraw_set_scale(1.0f, h / ch);
    textdraw_set_pos(x0 - cw, y0);
    textdraw_print("\x06");
    textdraw_set_pos(x1, y0);
    textdraw_print("\x08");
    textdraw_set_scale(w / cw, h / ch);
    textdraw_set_pos(x0, y0);
    textdraw_print("\x07");
}

void fn_1_162260(struct TextBoxFrame *frame)
{
    GXColor color;
    Vec dir;
    float len;
    float cx, cy;
    float tipX, tipY;
    float ax, ay;

    fn_800668FC();
    g_something_with_GX_vtx_desc(0x200);
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    g_GXSetTevIndirect_zero_if_different(0);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 2);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 1);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetZMode_cached(1, 1, 1);
    GXSetBlendMode_cached(1, 4, 5, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    color = frame->color;
    GXSetTevColor(1, color);
    len = frame->tailWidth;
    cx = 0.5f * (frame->x0 + frame->x1);
    cy = 0.5f * (frame->y0 + frame->y1);
    dir.x = cx - frame->tailX;
    dir.y = cy - frame->tailY;
    dir.z = 0.0f;
    vec_set_len(len, &dir, &dir);
    tipX = frame->tailX;
    tipY = frame->tailY;
    ax = cx - dir.y;
    ay = cy + dir.x;
    cx = cx + dir.y;
    cy = cy - dir.x;
    GXBegin(0x90, 7, 3);
    GXPosition3f32(tipX, tipY, -frame->z);
    GXPosition3f32(ax, ay, -frame->z);
    GXPosition3f32(cx, cy, -frame->z);
    g_some_sprite_reset_gx_state_func();
}

void fn_1_162524(s8 *a, struct Sprite *sprite)
{
    struct TextBox *box = TEXTBOX_FROM_USERVAR(sprite->userVar);

    if (box != NULL && box->entries != NULL)
    {
        fn_1_160D48(box);
        return;
    }
}

void fn_1_162560(struct Sprite *sprite)
{
    fn_1_160ECC(TEXTBOX_FROM_USERVAR(sprite->userVar));
}

void fn_1_162584(struct TextBox *box)
{
    box->timer = 0.0f;
    box->timerTarget = 0.0f;
    box->pending = 0;
}

float fn_1_1625B0(struct TextBox *box)
{
    return box->timer;
}

void fn_1_1625B8(struct TextBox *box)
{
    box->head = 0;
    box->next = 0;
    box->curX = 0;
    box->scroll = 0.0f;
    box->scrollTarget = 0;
    box->line = 0;
}

int fn_1_1625F4(struct Dialog *dlg, Vec *out)
{
    static Vec lbl_1_data_70F60[17] = {
        {0.0599999987f, 0.0f, -0.0199999996f},
        {0.150000006f, 0.0f, -0.0500000007f},
        {0.170000002f, 0.0f, -0.0399999991f},
        {0.209999993f, 0.0f, -0.0399999991f},
        {0.270000011f, 0.0f, 0.0199999996f},
        {0.270000011f, 0.0f, 0.0199999996f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.270000011f, 0.0f, 0.0199999996f},
        {0.270000011f, 0.0f, 0.0199999996f},
        {0.270000011f, 0.0f, 0.0199999996f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
    };
    Mtx mtx;
    struct ScreenRect rect;
    Vec v;
    struct Camera *camera;
    struct Ape *ape;
    float s;

    fn_1_162A58(&rect);
    camera = cameras;
    ape = dlg->ape;
    if (ape == NULL)
        return 0;
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(mtx);
    mtxa_from_mtx(camera->unk14C);
    mtxa_translate(&ape->pos);
    mtxa_scale_s(ape->scale);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(mtx);
    mtxa_mult_right(g_get_ape_anim_smth(ape, 3));
    mtxa_translate(&lbl_1_data_70F60[ape->charaId]);
    mtxa_translate_xyz(0.1f * lbl_1_bss_6FB84, 0.1f * lbl_1_bss_6FB80, 0.1f * lbl_1_bss_6FB7C);
    mathutil_mtxA_get_translate(&v);
    if (v.z < 0.0f)
    {
        s = 1.0f / (camera->sub28.view.tanHalfFov * v.z);
        v.x = v.x * (s / camera->sub28.view.aspect);
        v.y = v.y * s;
        v.x -= camera->sub28.view.unk28;
        v.y -= camera->sub28.view.unk2C;
        v.x = 0.5f * rect.width + -v.x * (0.5f * rect.scaleW);
        v.y = 0.5f * rect.height + v.y * (0.5f * rect.scaleH);
    }
    *out = v;
    return 1;
}

void *load_dialog_tbl(int a)
{
    char path[0x80];
    struct File file;
    char *name;
    u32 size;
    struct DialogTbl *tbl;
    int i;
    struct DialogData * DPTR *entry;
    struct DialogData *e;

    DVDChangeDir("dlg");
    name = scene_filenames[a];
    sprintf(path, "%s%s.bin", name, locale_suffixes[locale]);
    if (file_open(path, &file) == 0)
    {
        sprintf(path, "%s%s.bin", name, locale_suffixes[0]);
        file_open(path, &file);
    }
    size = (file_size(&file) + 0x1F) & ~0x1F;
    {
        int hdr = 0x20;
        u32 total = hdr + size;
        void *p = alloc_from_heap_or_panic(currentHeap, total, "dialog.c", 2203);
        *(int *)p = currentHeap;
        tbl = (void *)((u8 *)p + hdr);
    }
    file_read(&file, tbl, size, 0);
    file_close(&file);
    DVDChangeDir("/");
#ifndef __MWERKS__
    {
        /* pc_port: dlg/*.bin is a big-endian DialogTbl image */
        extern void pc_swap_dialog_tbl(void *tbl);
        pc_swap_dialog_tbl(tbl);
    }
#endif
    tbl->entries = (void *)((uptr)tbl->entries + (uptr)tbl);
    entry = tbl->entries;
    for (i = tbl->count; i > 0; i--, entry++)
    {
        *entry = (void *)((uptr)*entry + (uptr)tbl);
        e = *entry;
        e->name = (void *)((uptr)e->name + (uptr)tbl);
        e->text = (void *)((uptr)e->text + (uptr)tbl);
    }
    return tbl;
}

void fn_1_162978(void *tbl)
{
    int hdr = 0x20;
    int *p = (void *)((u8 *)tbl - hdr);

    OSFreeToHeap(*p, p);
}

void *g_smth_with_dialog2(struct DialogTbl *tbl, int i)
{
    if (i < 0 || i >= tbl->count)
        return NULL;
    return tbl->entries[i];
}

void *fn_1_1629D0(struct DialogTbl *tbl, char *name)
{
    int i;
    struct DialogData * DPTR *entries;
    struct DialogData *e;

    entries = tbl->entries;
    i = tbl->count;
    for (; i > 0; i--, entries++)
    {
        e = *entries;
        if (strcmp(e->name, name) == 0)
            break;
    }
    if (i == 0)
        return NULL;
    return e;
}

void fn_1_162A58(struct ScreenRect *out)
{
    static struct ScreenRect lbl_1_data_71040 = {
        640.0f, 480.0f, 640.0f, 480.0f, 16.0f, 16.0f, 624.0f, 464.0f, 320.0f, 240.0f,
    };
    static struct ScreenRect lbl_1_data_71068[4] = {
        {853.333313f, 480.0f, 853.333313f, 480.0f, 16.0f, 16.0f, 837.333313f, 464.0f, 426.666656f, 240.0f},
        {640.0f, 360.0f, 853.333313f, 480.0f, 16.0f, 16.0f, 624.0f, 344.0f, 320.0f, 180.0f},
        {739.008362f, 415.6922f, 853.333313f, 480.0f, 16.0f, 16.0f, 723.008362f, 399.6922f, 369.504181f, 207.8461f},
        {640.0f, 480.0f, 640.0f, 480.0f, 16.0f, 16.0f, 624.0f, 464.0f, 320.0f, 240.0f},
    };

    if (modeCtrl.wideMode == 0)
        *out = lbl_1_data_71040;
    else
        *out = lbl_1_data_71068[g_related_to_fov2];
}
