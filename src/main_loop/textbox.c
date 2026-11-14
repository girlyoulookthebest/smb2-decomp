/* On-screen message boxes: per-box line queues, open/close/scroll states and box drawing. */
#include "types.h"
#include "va.h"
#include "dip.h"
#include "libc.h"
#include "macros.h"
#include "mathutil.h"
#include "sprite.h"
#include "textdraw.h"

#pragma unsigned_char on

#define TEXTBOX_COUNT 4
#define TEXTBOX_LINE_COUNT 20

struct TextBox
{
    /*0x00*/ s32 state;
    /*0x04*/ s32 timerMax;
    /*0x08*/ u32 timer;
    /*0x0C*/ s16 x;
    /*0x0E*/ s16 y;
    /*0x10*/ float width;
    /*0x14*/ s8 columns;
    /*0x15*/ s8 rows;
    /*0x16*/ s8 style;
    /*0x17*/ s8 unk17;
    /*0x18*/ s8 unk18;
    /*0x19*/ s8 unk19;
    /*0x1A*/ u8 filler1A[2];
    /*0x1C*/ void (*callback)(struct TextBox *);
    /*0x20*/ s32 id;
    /*0x24*/ u32 textColor;
    /*0x28*/ s8 textAlign;
    /*0x29*/ u8 filler29[3];
    /*0x2C*/ u32 flags;
    /*0x30*/ float textScale;
}; /* size = 0x34 */

struct TextBoxLine
{
    /*0x00*/ s8 state;
    /*0x01*/ char str[0x81];
    /*0x82*/ s16 timer;
}; /* size = 0x84 */

float fn_1_B25DC(char *str);
int fn_1_B2608(int fontId, char *str);
int g_smth_with_fonts_chara_load_wrapper_discard_result(char *str);
float textdraw_get_pixel_width_of_string(char *str);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_flags(u32 flags);
void textdraw_set_scale(float x, float y);
void textdraw_set_depth(float depth);
void textdraw_set_alpha(float alpha);
void textdraw_set_mul_color(u32 color);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_pos(float x, float y);
void g_textdraw_set_counter(u16 counter);
void textdraw_print(char *str);
void textdraw_print_and_fit_to_width(float maxWidth, char *str);
void set_ui_widescreen_scale_mtx(int arg);
void reset_ui_widescreen_scale_mtx(void);

void fn_1_B78F0(int id, struct TextBox *box);
void fn_1_B800C(int id, struct TextBox *box);
void fn_1_B92CC(int id);
void fn_1_B9340(struct TextBox *box, int *x, int *y);

struct TextBox lbl_1_bss_A5C40[TEXTBOX_COUNT];
static struct TextBox s_pendingBoxes[TEXTBOX_COUNT];
float g_font_char_var_1;
float g_font_char_var_2;
static float s_depthOffset;
static struct TextBoxLine s_lines[TEXTBOX_COUNT][TEXTBOX_LINE_COUNT];

void fn_1_B7760(void)
{
    int i;
    struct TextBox *box = lbl_1_bss_A5C40;
    struct TextBox *pending = s_pendingBoxes;

    for (i = 0; i < TEXTBOX_COUNT; i++, box++, pending++)
    {
        memset(box, 0, sizeof(*box));
        memset(pending, 0, sizeof(*pending));
        box->state = 0;
        box->timer = 0;
        box->callback = NULL;
        fn_1_B92CC(i);
    }
    g_font_char_var_1 = -1.0f;
    g_font_char_var_2 = 0.0f;
}

void fn_1_B7828(void)
{
    int i;
    struct TextBox *box;

    s_depthOffset = (g_some_other_flags & 8) ? 0.0001f : 0.05;
    box = lbl_1_bss_A5C40;
    for (i = 0; i < TEXTBOX_COUNT; i++, box++)
    {
        if (box->state != 0)
        {
            if (box->callback != NULL)
                box->callback(box);
            fn_1_B78F0(i, box);
        }
    }
}

void fn_1_B78F0(int id, struct TextBox *box)
{
    int i;
    struct TextBoxLine *line;

    switch (box->state)
    {
    case 1:
        box->timer++;
        if (box->timer == box->timerMax)
        {
            box->state = 10;
            box->timer = 0;
            if (s_pendingBoxes[id].state == 20)
            {
                box->state = 20;
                s_pendingBoxes[id].state = 0;
            }
        }
        if (box->columns == 0)
        {
            s8 columns = 0;
            float maxWidth = 0.0f;

            line = s_lines[id];
            if (box->flags & 4)
            {
                textdraw_reset();
                textdraw_set_font(3);
                textdraw_set_flags(0x80000000);
                textdraw_set_scale(box->textScale, 1.0f);
            }
            for (i = 0; i < TEXTBOX_LINE_COUNT; i++, line++)
            {
                if (line->state == 0)
                    break;
                if (box->rows == 1)
                {
                    if (!(box->flags & 4))
                    {
                        columns = fn_1_B25DC(line->str);
                        maxWidth = fn_1_B2608(0x8F, line->str);
                    }
                    else
                    {
                        columns = g_smth_with_fonts_chara_load_wrapper_discard_result(line->str);
                        maxWidth = textdraw_get_pixel_width_of_string(line->str);
                    }
                }
                else
                {
                    s8 lineColumns = !(box->flags & 4)
                        ? fn_1_B25DC(line->str)
                        : (float)g_smth_with_fonts_chara_load_wrapper_discard_result(line->str);
                    float lineWidth = !(box->flags & 4)
                        ? (float)fn_1_B2608(0x8F, line->str)
                        : textdraw_get_pixel_width_of_string(line->str);

                    if (columns < lineColumns)
                        columns = lineColumns;
                    if (maxWidth < lineWidth)
                        maxWidth = lineWidth;
                }
            }
            if (box->flags & 4)
            {
                box->columns = (columns < 25) ? columns : 25;
                box->width = (maxWidth < 600.0f) ? maxWidth : 600.0f;
            }
            else
            {
                box->columns = columns;
                box->width = maxWidth;
            }
        }
        break;
    case 3:
        box->state = 1;
        box->timer = 0;
        break;
    case 10:
    {
        int count;

        box->timer++;
        count = 0;
        line = s_lines[id];
        for (i = 0; i < TEXTBOX_LINE_COUNT; i++, line++)
        {
            if (line->state != 0)
                count++;
        }
        if (count > box->rows)
        {
            box->state = 11;
            box->timer = 0;
        }
        break;
    }
    case 11:
        box->timer++;
        if (box->timer == 16)
        {
            box->state = 10;
            line = s_lines[id];
            for (i = 0; i < TEXTBOX_LINE_COUNT - 1; i++, line++)
                memcpy_handler(line, line + 1, sizeof(*line));
            line->state = 0;
            line->timer = 0;
        }
        break;
    case 20:
        box->timer++;
        if (box->timer == box->timerMax)
        {
            box->state = 0;
            box->timer = 0;
            box->callback = NULL;
            fn_1_B92CC(id);
        }
        break;
    case 21:
        box->timer++;
        if (box->timer == box->timerMax)
        {
            int n;

            for (n = 0; n < box->rows; n++)
            {
                line = s_lines[id];
                for (i = 0; i < TEXTBOX_LINE_COUNT - 1; i++, line++)
                    memcpy_handler(line, line + 1, sizeof(*line));
                line->state = 0;
                line->timer = 0;
            }
            memcpy_handler(box, &s_pendingBoxes[id], sizeof(*box));
            box->state = 1;
            box->timer = 0;
            s_pendingBoxes[id].state = 0;
        }
        break;
    case 22:
        box->timer++;
        if (box->timer == box->timerMax)
        {
            box->state = 2;
            box->timer = 0;
            fn_1_B92CC(id);
        }
        break;
    }

    line = s_lines[id];
    for (i = 0; i < TEXTBOX_LINE_COUNT; i++, line++)
    {
        if (box->state >= 10 && line->state != 0)
        {
            if (line->state == 2 && (box->state == 10 || box->state == 11))
            {
                if (i < box->rows && (i == 0 || s_lines[id][i - 1].state != 2))
                    line->timer += (box->flags & 0x40) ? 4 : 1;
                if (line->timer >= box->columns)
                {
                    line->state = 1;
                    if (box->flags & 4)
                    {
                        line->timer = 0;
                        if (i == box->rows - 1)
                            box->flags |= 2;
                    }
                }
            }
            else if (line->state == 1 && (box->flags & 4) && (box->flags & 2))
                line->timer++;
        }
    }
}

void fn_1_B7F68(void)
{
    int i;
    struct TextBox *box = lbl_1_bss_A5C40;

    for (i = 0; i < TEXTBOX_COUNT; i++, box++)
        box->state = 0;
}

void fn_1_B7F98(void)
{
    int i;
    struct TextBox *box = lbl_1_bss_A5C40;

    for (i = 0; i < TEXTBOX_COUNT; i++, box++)
    {
        if (box->state != 0)
        {
            set_ui_widescreen_scale_mtx(0x140);
            fn_1_B800C(i, box);
            reset_ui_widescreen_scale_mtx();
        }
    }
}

void fn_1_B800C(int id, struct TextBox *box)
{
    float scaleX;
    float scaleY;
    float rowCount;
    float columnCount;
    float cellX;
    float cellY;
    float alpha;
    float t;
    float textAlpha;
    int x;
    int y;
    int lineX;
    int lineY;
    int i;
    int j;
    int scroll;
    struct TextBoxLine *line;

    if (box->state == 0 || box->state == 2 || box->state == 3)
        return;
    if (box->columns == 0)
        return;

    textdraw_reset();
    textdraw_set_font((box->flags & 8) ? 0x16 : 0x15);
    textdraw_set_depth(0.002f + s_depthOffset);
    textdraw_set_mul_color(0xFFFFFF);

    t = (float)box->timer / (float)box->timerMax;
    if (box->state == 1)
    {
        scaleX = (t < 0.5) ? 2.0f * t : 1.0;
        scaleY = (t < 0.5) ? 0.1 : 0.1 + 1.8 * (t - 0.5);
        alpha = (t < 0.5) ? 2.0f * t : 1.0;
    }
    else if (box->state >= 20 && t > 0.5)
    {
        t = 2.0 * (t - 0.5);
        scaleX = (t < 0.5) ? 1.0 : 2.0f * t;
        scaleY = (t < 0.5) ? 0.1 + 1.8 * (1.0 - t) : 0.1;
        alpha = 1.0 - t;
    }
    else
    {
        scaleX = 1.0f;
        scaleY = 1.0f;
        alpha = 1.0f;
    }
    textdraw_set_scale(scaleX, scaleY);
    textdraw_set_alpha(alpha);

    fn_1_B9340(box, &x, &y);
    x = x + 0.5 * ((1.0 - scaleX) * (48.0f + box->width));
    y = y + 0.5 * (24.0 * ((1.0 - scaleY) * box->rows));
    if (box->flags & 0x10)
    {
        x += 12;
        y += 12;
    }
    rowCount = box->rows;
    columnCount = box->columns;
    textdraw_set_pos(x - 24, y - 24);

    for (i = -1; i <= rowCount; i++)
    {
        for (j = -1; j <= columnCount; j++)
        {
            cellX = scaleX;
            cellY = scaleY;
            if ((box->flags & 0x10) && (i == -1 || i == rowCount))
                cellY *= 0.5;
            if ((box->flags & 0x10) && (j == -1 || j == columnCount))
                cellX *= 0.5;
            textdraw_set_scale(cellX, cellY);
            if (j == -1)
            {
                if (i == -1)
                    textdraw_print("\x01");
                else if (i == rowCount)
                    textdraw_print("\x0B");
                else
                    textdraw_print("\x06");
            }
            else if (j == columnCount)
            {
                if (i == -1)
                    textdraw_print("\x03");
                else if (i == rowCount)
                    textdraw_print("\x0D");
                else
                    textdraw_print("\x08");
            }
            else if ((box->flags & 8) && (i == -1 || i == rowCount))
            {
                float tiles = mathutil_floor(box->width / 24.0f);

                if (j < tiles)
                {
                    textdraw_set_scale(cellX * (box->width / tiles / 24.0f), cellY);
                    if (i == -1)
                        textdraw_print("\x02");
                    else if (i == rowCount)
                        textdraw_print("\x0C");
                    else
                        textdraw_print("\x07");
                    textdraw_set_scale(cellX, cellY);
                }
            }
            else if (j == 0)
            {
                textdraw_set_scale(cellX * box->width / 24.0f, cellY);
                if (i == -1)
                    textdraw_print("\x02");
                else if (i == rowCount)
                    textdraw_print("\x0C");
                else
                    textdraw_print("\x07");
                textdraw_set_scale(cellX, cellY);
            }
        }
        textdraw_print("\n");
    }

    textdraw_set_depth(0.001f + s_depthOffset);
    if (box->state >= 20)
        textAlpha = 1.0f - box->timer / 15.0f;
    else if (box->state == 10)
        textAlpha = box->timer / 15.0f;
    else
        textAlpha = 0.0f;
    textAlpha = (textAlpha < 0.0f) ? 0.0f : ((textAlpha > 1.0f) ? 1.0f : textAlpha);
    textdraw_set_alpha(textAlpha);

    if (!(box->flags & 0x20))
    {
        float tail = 24.0f * box->textScale;

        switch (box->style)
        {
        case 1:
            textdraw_set_pos(x - tail, y);
            textdraw_print("\x04");
            break;
        case 2:
            textdraw_set_pos(x - tail, y + 0.5f * (tail * (box->rows - 1)));
            textdraw_print("\x04");
            break;
        case 3:
            textdraw_set_pos(x - tail, y + tail * (box->rows - 1));
            textdraw_print("\x04");
            break;
        case 4:
            textdraw_set_pos(x + box->width, y);
            textdraw_print("\x05");
            break;
        case 5:
            textdraw_set_pos(x + box->width, y + 0.5f * (tail * (box->rows - 1)));
            textdraw_print("\x05");
            break;
        case 6:
            textdraw_set_pos(x + box->width, y + tail * (box->rows - 1));
            textdraw_print("\x05");
            break;
        case 7:
            textdraw_set_pos(14.0f + x, y - tail);
            textdraw_print("\x10");
            break;
        case 8:
            textdraw_set_pos((x + 0.5f * box->width) - 10.0f, y - tail);
            textdraw_print("\x10");
            break;
        case 9:
            textdraw_set_pos((x + box->width) - 34.0f, y - tail);
            textdraw_print("\x10");
            break;
        case 10:
            textdraw_set_pos(14.0f + x, y + tail * box->rows);
            textdraw_print("\x15");
            break;
        case 11:
            textdraw_set_pos((x + 0.5f * box->width) - 10.0f, y + tail * box->rows);
            textdraw_print("\x15");
            break;
        case 12:
            textdraw_set_pos((x + box->width) - 34.0f, y + tail * box->rows);
            textdraw_print("\x15");
            break;
        }
    }

    textdraw_set_scale(scaleX, scaleY);
    if (box->state < 10)
    {
        textdraw_reset();
        return;
    }

    textdraw_set_flags(0x220000);
    if (!(box->flags & 4))
        textdraw_set_font(0x8F);
    else
    {
        textdraw_set_font(3);
        textdraw_set_flags(0x80000000);
        textdraw_set_alignment(box->textAlign);
    }
    textdraw_set_depth(s_depthOffset);
    textdraw_set_mul_color(box->textColor);
    textdraw_set_scale(box->textScale, 1.0f);

    if (box->state == 11)
        scroll = -1.5 * box->timer;
    else
        scroll = 0;

    if (box->state >= 20)
    {
        float halfTime = 0.5 * box->timerMax;

        alpha = (box->timer < halfTime) ? 1.0f - box->timer * (1.0f / halfTime) : 0.0;
    }
    else
        alpha = 1.0f;

    line = s_lines[id];
    for (i = 0; i < TEXTBOX_LINE_COUNT; i++, line++)
    {
        float width;
        float frac;

        if (line->state == 0)
            break;
        if (!(box->flags & 1) || !(box->flags & 2) || line->timer % 60 <= 45)
        {
            g_font_char_var_1 = line->timer;
            g_font_char_var_2 = box->columns;
            if ((box->flags & 4) && line->state == 2)
                g_textdraw_set_counter(line->timer);
            if (box->state == 11)
            {
                if (i == 0)
                    alpha = (box->timer < 8) ? 1.0 - 0.14 * box->timer : 0.0;
                else
                    alpha = 1.0f;
            }
            textdraw_set_alpha(alpha);
            fn_1_B9340(box, &lineX, &lineY);
            if (box->textAlign == 6)
                lineX = lineX + box->width;
            else if (box->textAlign == 3)
                lineX = lineX + 0.5f * box->width;
            width = !(box->flags & 4)
                ? fn_1_B25DC(line->str)
                : (float)g_smth_with_fonts_chara_load_wrapper_discard_result(line->str);
            frac = width - mathutil_trunc(width);
            textdraw_set_pos(lineX + ((0.0f == frac) ? 0 : 6), lineY + i * 24 + scroll);
            if (box->flags & 4)
                textdraw_print_and_fit_to_width(600.0f, line->str);
            else
                textdraw_print(line->str);
        }
    }
    g_font_char_var_1 = -1.0f;
    textdraw_reset();
}

void fn_1_B92CC(int id)
{
    int i;
    struct TextBoxLine *line = s_lines[id];

    for (i = 0; i < TEXTBOX_LINE_COUNT; i++, line++)
    {
        line->state = 0;
        line->timer = 0;
        strcpy(line->str, "");
    }
}

void fn_1_B9340(struct TextBox *box, int *x, int *y)
{
    float height = 24.0f * (box->rows - 1);
    float width = box->width;

    switch (box->style)
    {
    default:
        *x = box->x - 0.5f * width;
        *y = box->y - 12.0f * box->rows;
        break;
    case 1:
        *x = box->x + 18;
        *y = box->y - 12;
        break;
    case 2:
        *x = box->x + 18;
        *y = (box->y - 12) - 0.5 * height;
        break;
    case 3:
        *x = box->x + 18;
        *y = (box->y - 12) - height;
        break;
    case 4:
        *x = (box->x - width) - 18.0f;
        *y = box->y - 12;
        break;
    case 5:
        *x = (box->x - width) - 18.0f;
        *y = (box->y - 12) - 0.5f * height;
        break;
    case 6:
        *x = (box->x - width) - 18.0f;
        *y = (box->y - 12) - height;
        break;
    case 7:
        *x = box->x - 24;
        *y = box->y + 18;
        break;
    case 8:
        *x = box->x - 0.5 * width;
        *y = box->y + 18;
        break;
    case 9:
        *x = 24.0f + (box->x - width);
        *y = box->y + 18;
        break;
    case 10:
        *x = box->x - 24;
        *y = (box->y - 42) - height;
        break;
    case 11:
        *x = box->x - 0.5 * width;
        *y = (box->y - 42) - height;
        break;
    case 12:
        *x = 24.0f + (box->x - width);
        *y = (box->y - 42) - height;
        break;
    }
}

void g_display_playpoint_or_gift_message_child(int id, int state, struct TextBox *src)
{
    int style;
    int x;
    int y;
    struct TextBox *box = &lbl_1_bss_A5C40[id];

    if (state == 20 && box->state >= 20)
    {
        box->state = 20;
        return;
    }
    if (state == 20 && box->state != 1 && box->state < 10)
        return;
    if (src == NULL)
        src = box;

    box->timerMax = 30;
    box->timer = 0;
    style = (src->style == 0) ? box->style : src->style;
    x = (src->x == 0) ? box->x : src->x;
    y = (src->y == 0) ? box->y : src->y;

    if (state == 1 || state == 2)
    {
        fn_1_B92CC(id);
        box->id = id;
        box->state = state;
        box->x = (s16)x;
        box->y = (s16)y;
        box->columns = src->columns;
        box->width = box->columns * 24;
        box->rows = src->rows;
        box->style = (s8)style;
        box->textAlign = src->textAlign;
        box->textColor = src->textColor;
        box->flags = src->flags | 4;
        box->unk17 = src->unk17;
        box->unk18 = src->unk18;
        box->unk19 = src->unk19;
        box->textScale = (0.0f == src->textScale) ? 1.0f : src->textScale;
        box->callback = src->callback;
    }
    else if (state == 21)
    {
        if (box->state < 10)
        {
            box->state = 3;
            box->x = (s16)x;
            box->y = (s16)y;
            box->columns = src->columns;
            box->width = box->columns * 24;
            box->rows = src->rows;
            box->style = (s8)style;
            box->textAlign = src->textAlign;
            box->textColor = src->textColor;
            box->flags = src->flags | 4;
        }
        else
        {
            box->state = state;
            memcpy_handler(&s_pendingBoxes[id], box, sizeof(*box));
            s_pendingBoxes[id].x = (s16)x;
            s_pendingBoxes[id].y = (s16)y;
            s_pendingBoxes[id].columns = src->columns;
            s_pendingBoxes[id].rows = src->rows;
            s_pendingBoxes[id].style = (s8)style;
            s_pendingBoxes[id].textAlign = src->textAlign;
            s_pendingBoxes[id].textColor = src->textColor;
            s_pendingBoxes[id].flags = src->flags | 4;
        }
    }
    else
        box->state = state;
}

static inline void add_line(int id, char *str)
{
    int i;
    struct TextBoxLine *line;
    struct TextBox *box;

    box = &lbl_1_bss_A5C40[id];
    line = s_lines[id];
    for (i = 0; i < TEXTBOX_LINE_COUNT; i++, line++)
    {
        if (line->state == 0)
        {
            line->state = 2;
            strcpy(line->str, str);
            line->timer = 0;
            if (i == box->rows && box->state == 10)
            {
                box->state = 11;
                box->timer = 0;
            }
            break;
        }
    }
}

void g_smth_with_playpoint_or_gift_msg(int id, char *str)
{
    char buf[0x200];
    int len = 0;
    int lines = 0;

    for (; *str != 0; str++)
    {
        if (*str == '\n')
        {
            buf[len] = 0;
            add_line(id, buf);
            lines++;
            len = 0;
        }
        else
        {
            buf[len] = *str;
            len++;
        }
    }
    buf[len] = 0;
    add_line(id, buf);
    lines++;
    if (lines > s_pendingBoxes[id].rows)
        s_pendingBoxes[id].rows = (s8)lines;
    lbl_1_bss_A5C40[id].flags &= ~2;
}

void g_some_printf_function_4(int id, char *fmt, ...)
{
    char text[0x200];
    va_list args;

    va_start(args, fmt);
    vsprintf(text, fmt, args);
    va_end(args);
    g_smth_with_playpoint_or_gift_msg(id, text);
}
