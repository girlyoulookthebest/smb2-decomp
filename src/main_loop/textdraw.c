/* Drawing of text from fonts paged in from ARAM (font.c in the original game). */
#include "types.h"
#include "bitmap.h"
#include "dip.h"
#include "gx.h"
#include "macros.h"
#include "libc.h"
#include "mathutil.h"
#include "os.h"
#include "textdraw.h"

#pragma unsigned_char on

#define FONT_COUNT 0x91
#define FONT_SLOT_COUNT 0x200

struct AvTextGlyph
{
    /*0x00*/ char *str;
    /*0x04*/ s8 unk4;
    /*0x05*/ s8 unk5;
}; /* size = 0x8 */

extern int currentHeap;

u32 g_some_font_ptr_array[FONT_SLOT_COUNT];
s32 g_related_to_aram_font_pages[FONT_COUNT];
GXTexObj lbl_1_bss_A1704[FONT_SLOT_COUNT];
struct TextDrawInfo textdraw_pos_x;
#ifdef __MWERKS__
u16 lbl_1_bss_A575C[0x132];
#else
u16 lbl_1_bss_A575C[FONT_SLOT_COUNT]; /* PC: room for the Japanese kanji pages (pc_font_ext.c) */
#endif
struct TextdrawUnk8 g_textdraw_unk8;
s32 lbl_1_bss_A59D4;
u8 lbl_1_bss_A59D8[0x268];

FORCE_BSS_ORDER(g_some_font_ptr_array)
FORCE_BSS_ORDER(g_related_to_aram_font_pages)
FORCE_BSS_ORDER(lbl_1_bss_A1704)
FORCE_BSS_ORDER(textdraw_pos_x)
FORCE_BSS_ORDER(lbl_1_bss_A575C)
FORCE_BSS_ORDER(g_textdraw_unk8)
FORCE_BSS_ORDER(lbl_1_bss_A59D4)
FORCE_BSS_ORDER(lbl_1_bss_A59D8)

#ifndef __MWERKS__
/* PC port: language-aware text decoding (pc_port docs/language.md).
 * The game reads every byte >= 0x80 as a Shift-JIS lead byte.  That is kept
 * for English and Japanese.  For German/French/Spanish/Italian (locale 1-4)
 * only 0x81-0x84 (the font's SJIS rows) and 0x86 (the PC's composed Latin
 * glyphs, pc_text.h) lead a two-byte code; any other byte >= 0x80 is a
 * Latin-1/cp1252 letter of the game's own European strings and is drawn as
 * code 0x8600 | byte. */
extern s32 locale;
int pc_text_is_lead(unsigned char c)
{
    if (locale >= 1 && locale <= 4)
        return (c >= 0x81 && c <= 0x84) || c == 0x86;
    return c >= 0x80;
}
/* pc_font_ext.c: slot / width of a composed Latin glyph (code 0x86xx) */
int pc_font_ext_glyph(u16 code, s32 *slot, u16 *width);
#endif

void OSReport(const char *fmt, ...);
void *OSAllocFromHeap(int heap, u32 size);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, int mipmap);
void GXLoadTexObj_cached(GXTexObj *obj, int id);
void GXSetTevColor(int id, GXColor color);
void mtxa_from_translate_xyz(float x, float y, float z);
void set_ui_widescreen_scale_mtx(int arg);
void reset_ui_widescreen_scale_mtx(void);
void aram_font_to_mram_font(u8 fontId, int page, void *dest);
int g_get_font_char_width(char *str, int fontId, struct FontParams *font);
float g_get_font_char_width_scaling(char *str, int fontId);
void g_load_specific_font(int fontId);
u16 g_some_avtext_array_lookup(u16 chr, struct TextDrawInfo *info, struct FontParams *font);
float textdraw_chara_load(char *str, int single, s8 kind);
void fn_1_B7664(struct SpriteDrawRequest *req, struct TextDrawInfo *info, struct FontParams *font, int width);

u16 lbl_1_data_394A8[] =
{
    0x8141, 0x8142, 0x8143, 0x8144, 0x8145, 0x8146, 0x8147, 0x8148, 0x8149, 0x814D,
    0x814F, 0x8151, 0x8158, 0x815B, 0x815C, 0x815E, 0x815F, 0x8160, 0x8162, 0x8163,
    0x8165, 0x8166, 0x8167, 0x8168, 0x8169, 0x816A, 0x816D, 0x816E, 0x8175, 0x8176,
    0x8177, 0x8178, 0x817B, 0x817C, 0x817E, 0x8180, 0x8181, 0x8182, 0x8183, 0x8184,
    0x8187, 0x8189, 0x818A, 0x818F, 0x8190, 0x8193, 0x8194, 0x8195, 0x8196, 0x8197,
    0x8199, 0x819A, 0x819B, 0x819C, 0x81A0, 0x81A1, 0x81A2, 0x81A3, 0x81A6, 0x81A8,
    0x81CB, 0x81F4, 0x824F, 0x8250, 0x8251, 0x8252, 0x8253, 0x8254, 0x8255, 0x8256,
    0x8257, 0x8258, 0x8260, 0x8261, 0x8262, 0x8263, 0x8264, 0x8265, 0x8266, 0x8267,
    0x8268, 0x8269, 0x826A, 0x826B, 0x826C, 0x826D, 0x826E, 0x826F, 0x8270, 0x8271,
    0x8272, 0x8273, 0x8274, 0x8275, 0x8276, 0x8277, 0x8278, 0x8279, 0x8281, 0x8282,
    0x8283, 0x8284, 0x8285, 0x8286, 0x8287, 0x8288, 0x8289, 0x828A, 0x828B, 0x828C,
    0x828D, 0x828E, 0x828F, 0x8290, 0x8291, 0x8292, 0x8293, 0x8294, 0x8295, 0x8296,
    0x8297, 0x8298, 0x8299, 0x829A, 0x829F, 0x82A0, 0x82A1, 0x82A2, 0x82A3, 0x82A4,
    0x82A5, 0x82A6, 0x82A7, 0x82A8, 0x82A9, 0x82AA, 0x82AB, 0x82AC, 0x82AD, 0x82AE,
    0x82AF, 0x82B0, 0x82B1, 0x82B2, 0x82B3, 0x82B4, 0x82B5, 0x82B6, 0x82B7, 0x82B8,
    0x82B9, 0x82BA, 0x82BB, 0x82BC, 0x82BD, 0x82BE, 0x82BF, 0x82C0, 0x82C1, 0x82C2,
    0x82C3, 0x82C4, 0x82C5, 0x82C6, 0x82C7, 0x82C8, 0x82C9, 0x82CA, 0x82CB, 0x82CC,
    0x82CD, 0x82CE, 0x82CF, 0x82D0, 0x82D1, 0x82D2, 0x82D3, 0x82D4, 0x82D5, 0x82D6,
    0x82D7, 0x82D8, 0x82D9, 0x82DA, 0x82DB, 0x82DC, 0x82DD, 0x82DE, 0x82DF, 0x82E0,
    0x82E1, 0x82E2, 0x82E3, 0x82E4, 0x82E5, 0x82E6, 0x82E7, 0x82E8, 0x82E9, 0x82EA,
    0x82EB, 0x82EC, 0x82ED, 0x82EE, 0x82EF, 0x82F0, 0x82F1, 0x8340, 0x8341, 0x8342,
    0x8343, 0x8344, 0x8345, 0x8346, 0x8347, 0x8348, 0x8349, 0x834A, 0x834B, 0x834C,
    0x834D, 0x834E, 0x834F, 0x8350, 0x8351, 0x8352, 0x8353, 0x8354, 0x8355, 0x8356,
    0x8357, 0x8358, 0x8359, 0x835A, 0x835B, 0x835C, 0x835D, 0x835E, 0x835F, 0x8360,
    0x8361, 0x8362, 0x8363, 0x8364, 0x8365, 0x8366, 0x8367, 0x8368, 0x8369, 0x836A,
    0x836B, 0x836C, 0x836D, 0x836E, 0x836F, 0x8370, 0x8371, 0x8372, 0x8373, 0x8374,
    0x8375, 0x8376, 0x8377, 0x8378, 0x8379, 0x837A, 0x837B, 0x837C, 0x837D, 0x837E,
    0x8380, 0x8381, 0x8382, 0x8383, 0x8384, 0x8385, 0x8386, 0x8387, 0x8388, 0x8389,
    0x838A, 0x838B, 0x838C, 0x838D, 0x838E, 0x838F, 0x8390, 0x8391, 0x8392, 0x8393,
    0x8394, 0x839F, 0x83A0, 0x83A1, 0x83A4, 0x83A6, 0x83AD, 0x83AF, 0x83B2, 0x83B4,
    0x83B5, 0x84D0, 0x84D2, 0x84D4, 0x84D6, 0x0000,
};

char *FONT_NAME_LIST[] =
{
    "FONT_ASCII",
    "FONT_ASC_8x16",
    "FONT_ASC_12x12",
    "FONT_ASC_24x24",
    "FONT_ASC_16x16P",
    "FONT_ASC_16x16",
    "FONT_DMY03",
    "FONT_DMY04",
    "FONT_DMY05",
    "FONT_ASC_72x64",
    "FONT_DMY06",
    "FONT_DMY07",
    "FONT_DMY08",
    "FONT_DMY09",
    "FONT_DMY10",
    "FONT_DMY11",
    "FONT_DMY12",
    "FONT_DMY13",
    "FONT_DMY14",
    "FONT_DMY15",
    "FONT_DMY16",
    "FONT_ICON_SD",
    "FONT_ICON_SD2",
    "FONT_DMY20",
    "FONT_DMY21",
    "FONT_DMY22",
    "FONT_DMY23",
    "FONT_DMY_RNK00",
    "FONT_DMY_RNK01",
    "FONT_DMY_RNK02",
    "FONT_DMY_RNK03",
    "FONT_DMY_RNK04",
    "FONT_RNK_32x32",
    "FONT_RNK_NUM",
    "FONT_MINI_RNK",
    "FONT_SCORE_NUM",
    "FONT_DMY_RNK0",
    "FONT_DMY_RNK1",
    "FONT_DMY_RNK2",
    "FONT_DMY_RNK3",
    "FONT_DMY_RNK4",
    "FONT_DMY_RNK5",
    "FONT_DMY_RNK6",
    "FONT_DMY_RNK7",
    "FONT_DMY_RNK8",
    "FONT_DMY_RNK9",
    "FONT_DMY_RNKa",
    "FONT_DMY_RNKb",
    "FONT_DMY_RNKc",
    "FONT_DMY_RNKd",
    "FONT_DMY_RNKe",
    "FONT_DMY_RNKf",
    "FONT_SEL_CTRL_PORT",
    "FONT_SEL_CTRL_R",
    "FONT_SEL_CTRL_B",
    "FONT_SEL_CTRL_Y",
    "FONT_SEL_CTRL_G",
    "FONT_SEL_CTRL_SDW",
    "FONT_DMY42",
    "FONT_DMY43",
    "FONT_BWL_SCORE",
    "FONT_DMY50",
    "FONT_DMY51",
    "FONT_DMY52",
    "FONT_DMY53",
    "FONT_DMY54",
    "FONT_DMY55",
    "FONT_DMY56",
    "FONT_DMY57",
    "FONT_RAC_DMY01",
    "FONT_RAC_PLAYER",
    "FONT_RAC_RANK",
    "FONT_RAC_LAP_MARK_NUM",
    "FONT_RAC_LAP_NUM",
    "FONT_RAC_TIME_NUM",
    "FONT_RAC_LAP_MARK_NUM_S",
    "FONT_RAC_DMY08",
    "FONT_RAC_DMY09",
    "FONT_RAC_DMY10",
    "FONT_RAC_SPD_NUM",
    "FONT_RAC_DMY12",
    "FONT_RAC_DMY13",
    "FONT_TGT_PLAYER",
    "FONT_TGT_MSCORE",
    "FONT_TGT_SCORE",
    "FONT_TGT_SCORE_S",
    "FONT_TGT_SPEED",
    "FONT_TGT_ROUND",
    "FONT_TGT_ALT",
    "FONT_TGT_WIND",
    "FONT_DMY71",
    "FONT_DMY72",
    "FONT_DMY73",
    "FONT_NUM_NML_SCORE",
    "FONT_NUM_NML_TIME",
    "FONT_NUM_NML_TIME_S",
    "FONT_NUM_NML_SPEED",
    "FONT_NUM_NML_SARU",
    "FONT_ASC_NML_D5",
    "FONT_ASC_NML_D6",
    "FONT_ASC_NML_D7",
    "FONT_ASC_NML_D8",
    "FONT_ASC_NML_D9",
    "FONT_DMY79",
    "FONT_DMY7a",
    "FONT_DMY7b",
    "FONT_DMY7c",
    "FONT_DMY7d",
    "FONT_DMY7e",
    "FONT_DMY7f",
    "FONT_BOA_TIME_NUM",
    "FONT_BOA_DMY02",
    "FONT_BOA_RANK",
    "FONT_BOA_PLAYER",
    "FONT_BOA_SPD_NUM",
    "FONT_BOA_LAP_MARK_NUM",
    "FONT_BOA_LAP_MARK_NUM_S",
    "FONT_BOA_DMY08",
    "FONT_BOA_DMY09",
    "FONT_BOA_DMY10",
    "FONT_BOA_LAP_NUM",
    "FONT_BOA_DMY12",
    "FONT_BOA_DMY13",
    "FONT_BOA_DMY14",
    "FONT_BOA_DMY15",
    "FONT_BOA_DMY16",
    "FONT_DOG_SPEED",
    "FONT_DOG_SPEED_S",
    "FONT_DOG_MSCORE",
    "FONT_DOG_SCORE",
    "FONT_DOG_SCORE_M",
    "FONT_DOG_SCORE_S",
    "FONT_DOG_ROUND",
    "FONT_DOG_ALT",
    "FONT_DOG_RESULT",
    "FONT_DOG_TIME",
    "FONT_DOG_DMY02",
    "FONT_DOG_DMY03",
    "FONT_DOG_DMY04",
    "FONT_DOG_DMY05",
    "FONT_JAP_TAG",
    "FONT_JAP_DMY",
    "FONT_JAP_24x24_2",
    "FONT_JAP_24x24_2P",
    "FONT_JAP_24x24_I",
};

char *FONT_STYLE_NAME_LIST[] =
{
    "ORIGINAL",
    "REGULAR",
    "BOLD",
    "ITALIC",
    "BOLD ITALIC",
    "TEGAKI",
};

static const float lbl_1_rodata_9EA8[6][8] =
{
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
    { 2.6f, 0.0f, 2.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 1.6f, -1.0f, 3.6f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
    { -1.4f, 3.7f, 1.3f, 0.0f, 1.6f, 0.5f, -0.9f, -0.8f },
};

void g_something_loading_fonts(void)
{
    int i;
    int j;
    struct FontParams *font;
    u16 *p;

    for (i = 0; i < FONT_SLOT_COUNT; i++)
        g_some_font_ptr_array[i] = 0;
    font = FONT_DEFINITIONS;
    for (i = 0; i < FONT_COUNT; i++, font++)
    {
        g_related_to_aram_font_pages[i] = 0;
        if (font->bmpId == -1)
        {
            for (j = 0, p = font->string; *p != 0; j++, p++)
            {
                font->pageIdx[j] = -1;
                g_related_to_aram_font_pages[i]++;
            }
        }
    }
    g_load_specific_font(3);
}

#ifndef __MWERKS__
/* PC port: init a font page's texture.  Pad-button glyphs of font 3 use the
 * current button prompt style's high-resolution icon when there is one
 * (pc_port button_prompts.c), else the page itself. */
int pc_button_prompts_hires(unsigned code, void **image, int *size);
static void pc_init_font_page_texobj(int fontId, u16 code, s32 slot, int fmt)
{
    struct FontParams *font = &FONT_DEFINITIONS[fontId];
    void *image;
    int size;

    if (fontId == 3 && fmt == 0xE && pc_button_prompts_hires(code, &image, &size))
        GXInitTexObj(&lbl_1_bss_A1704[slot], image, size, size, 6 /* GX_TF_RGBA8 */, 0, 0, 1);
    else
        GXInitTexObj(&lbl_1_bss_A1704[slot], (void *)g_some_font_ptr_array[slot], font->spaceWidth, font->lineHeight, fmt, 0, 0, 0);
}
#endif

void g_load_specific_font(int fontId)
{
    struct TextDrawInfo saved = textdraw_pos_x;
    int slot;
    int j;
    u16 *p;
    struct FontParams *font = &FONT_DEFINITIONS[fontId];
    int i;
    int fmt;

    if (font->bmpId == -1)
    {
        textdraw_pos_x.fontId = fontId;
        textdraw_pos_x.flags = 0x80000000;
        for (p = font->string, j = 0; *p != 0; j++, p++)
        {
            if (font->pageIdx[j] == -1)
            {
                slot = -1;
                for (i = 0; i < FONT_SLOT_COUNT; i++)
                {
                    if (g_some_font_ptr_array[i] == 0)
                    {
                        slot = i;
                        break;
                    }
                }
                if (slot == -1)
                {
                    if (dip_switches & 1)
                    {
                        OSReport("\n> %s\n", FONT_NAME_LIST[fontId]);
                        OSReport("  chara No.%d can't load.\n", j);
                        OSReport("  ARAM FONT BUFFER OVER!!\n\n");
                    }
                }
                else
                {
                    g_some_font_ptr_array[slot] = (u32)OSAllocFromHeap(currentHeap, font->size);
                    if (g_some_font_ptr_array[slot] == 0)
                        OSPanic("font.c", 0xF8, "cannot OSAlloc at chara_load()\n");
                    aram_font_to_mram_font(fontId, j, (void *)g_some_font_ptr_array[slot]);
                    fmt = 2;
                    if (font->string[j] >= 0x839F && font->string[j] <= 0x83BE)
                        fmt = 0xE;
#ifdef __MWERKS__
                    GXInitTexObj(&lbl_1_bss_A1704[slot], (void *)g_some_font_ptr_array[slot], font->spaceWidth, font->lineHeight, fmt, 0, 0, 0);
#else
                    pc_init_font_page_texobj(fontId, font->string[j], slot, fmt);
#endif
                    font->pageIdx[j] = slot;
                }
                lbl_1_bss_A575C[j] = g_some_avtext_array_lookup(*p, &textdraw_pos_x, font);
            }
        }
        textdraw_pos_x = saved;
    }
}

#ifndef __MWERKS__
/* PC port: the button prompt style changed (pc_port button_prompts.c).  Re-fetch
 * the loaded pad-button glyph pages of font 3 (codes 0x839F-0x83BE, CMPR);
 * aram_font_to_mram_font hands out the current style's icons. */
void pc_textdraw_reload_button_glyphs(void)
{
    struct FontParams *font = &FONT_DEFINITIONS[3];
    int j;
    s32 slot;
    u16 *p;

    if (font->bmpId != -1 || g_related_to_aram_font_pages[3] == 0)
        return;  /* fonts not loaded yet */
    for (j = 0, p = font->string; *p != 0; j++, p++)
    {
        if (*p < 0x839F || *p > 0x83BE)
            continue;
        slot = font->pageIdx[j];
        if (slot < 0 || slot >= FONT_SLOT_COUNT || g_some_font_ptr_array[slot] == 0)
            continue;
        aram_font_to_mram_font(3, j, (void *)g_some_font_ptr_array[slot]);
        pc_init_font_page_texobj(3, *p, slot, 0xE);
    }
}
#endif

void g_draw_sprite_draw_request_unbuffered(struct SpriteDrawRequest *req)
{
    Vec tl = {0.0f, 0.0f, 0.0f};
    Vec tr = {0.0f, 0.0f, 0.0f};
    Vec bl = {0.0f, 0.0f, 0.0f};
    Vec br = {0.0f, 0.0f, 0.0f};
    GXColor color;
    int page;
    int fontIdx;
    struct FontParams *font;
    s8 style;
    float x;
    float y;
    float z;
    float w;
    float h;
    float u1;
    float u2;
    float v1;
    float v2;

    if (req->flags & (1 << 24))
        set_ui_widescreen_scale_mtx(req->unk40);
    page = req->bmpId & 0xFFFF;
    fontIdx = (req->bmpId >> 16) & 0xFF;
    style = (u32)req->bmpId >> 24;
    font = &FONT_DEFINITIONS[fontIdx];
    x = req->x;
    y = req->y;
    z = -req->z;
    w = font->spaceWidth;
    h = font->lineHeight;

    switch (req->flags & 3)
    {
    default:
    case 1:
        tl.x = bl.x = 0.0f;
        tr.x = br.x = w;
        break;
    case 2:
        tl.x = bl.x = -w / 2.0;
        tr.x = br.x = w / 2.0;
        break;
    case 3:
        tl.x = bl.x = -w;
        tr.x = br.x = 0.0f;
        break;
    }

    switch ((req->flags & 0xC) >> 2)
    {
    default:
    case 1:
        tl.y = tr.y = 0.0f;
        bl.y = br.y = h;
        break;
    case 2:
        tl.y = tr.y = -h / 2.0;
        bl.y = br.y = h / 2.0;
        break;
    case 3:
        tl.y = tr.y = -h;
        bl.y = br.y = 0.0f;
        break;
    }

    mtxa_from_translate_xyz(x, y, 0.0f);
    mtxa_rotate_z((s16)-req->rotation);
    mtxa_scale_xyz(req->scaleX, req->scaleY, 1.0f);
    mtxa_tf_point(&tl, &tl);
    mtxa_tf_point(&tr, &tr);
    mtxa_tf_point(&bl, &bl);
    mtxa_tf_point(&br, &br);

    if (style != 0)
    {
        int idx = style;
        float sx = 1.0f + req->scaleX;
        float sy = 1.0f + req->scaleY;

        if (idx != 0)
        {
            const float *t = lbl_1_rodata_9EA8[idx];

            tl.x += 0.5 * t[0] * sx;
            tl.y += 0.5 * t[1] * sy;
            tr.x += 0.5 * t[2] * sx;
            tr.y += 0.5 * t[3] * sy;
            bl.x += 0.5 * t[4] * sx;
            bl.y += 0.5 * t[5] * sy;
            br.x += 0.5 * t[6] * sx;
            br.y += 0.5 * t[7] * sy;
        }
    }

    GXLoadTexObj_cached(&lbl_1_bss_A1704[page], 0);

    color.r = (req->mulColor & 0xFF0000) >> 16;
    color.g = (req->mulColor & 0xFF00) >> 8;
    color.b = req->mulColor & 0xFF;
    color.a = 255.0f * req->opacity;
    GXSetTevColor(1, color);
    color.r = (req->addColor & 0xFF0000) >> 16;
    color.g = (req->addColor & 0xFF00) >> 8;
    color.b = req->addColor & 0xFF;
    color.a = (req->addColor & 0xFF000000) >> 24;
    GXSetTevColor(2, color);

    if (req->flags & (1 << 18))
    {
        float f0 = 0.002405626 * req->z;

        tl.x = (tl.x - 320.0f) * f0;
        tl.y = -((tl.y - 240.0f) * f0);
        tr.x = (tr.x - 320.0f) * f0;
        tr.y = -((tr.y - 240.0f) * f0);
        bl.x = (bl.x - 320.0f) * f0;
        bl.y = -((bl.y - 240.0f) * f0);
        br.x = (br.x - 320.0f) * f0;
        br.y = -((br.y - 240.0f) * f0);
    }

    if (req->flags & (1 << 19))
    {
        u1 = req->u2;
        u2 = req->u1;
    }
    else
    {
        u1 = req->u1;
        u2 = req->u2;
    }
    if (req->flags & (1 << 20))
    {
        v1 = req->v2;
        v2 = req->v1;
    }
    else
    {
        v1 = req->v1;
        v2 = req->v2;
    }

    GXBegin(0x80, 7, 4);
    GXPosition3f32(tl.x, tl.y, z);
    GXTexCoord2f32(u1, v1);
    GXPosition3f32(tr.x, tr.y, z);
    GXTexCoord2f32(u2, v1);
    GXPosition3f32(br.x, br.y, z);
    GXTexCoord2f32(u2, v2);
    GXPosition3f32(bl.x, bl.y, z);
    GXTexCoord2f32(u1, v2);

    if (req->flags & (1 << 24))
        reset_ui_widescreen_scale_mtx();
}

u16 g_parse_avtext_non_alphanumeric(u16 c)
{
    char *unused = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

    if (c >= 'A' && c <= 'Z')
    {
        c += 0x821F;
        return c;
    }
    if (c >= 'a' && c <= 'z')
    {
        c += 0x8220;
        return c;
    }
    if (c >= '0' && c <= '9')
    {
        c += 0x821F;
        return c;
    }
    switch ((u8)c)
    {
    case '!':
        return 0x8149;
    case '"':
        return 0x8168;
    case '#':
        return 0x8194;
    case '$':
        return 0x8190;
    case '%':
        return 0x8193;
    case '&':
        return 0x8195;
    case '\'':
        return 0x8166;
    case '(':
        return 0x8169;
    case ')':
        return 0x816A;
    case '*':
        return 0x8196;
    case '+':
        return 0x817B;
    case ',':
        return 0x8143;
    case '-':
        return 0x817C;
    case '.':
        return 0x8144;
    case '/':
        return 0x815E;
    case ':':
        return 0x8146;
    case ';':
        return 0x8147;
    case '<':
        return 0x8183;
    case '=':
        return 0x8181;
    case '>':
        return 0x8184;
    case '?':
        return 0x8148;
    case '@':
        return 0x8197;
    case '[':
        return 0x816D;
    case '\\':
        return 0x815F;
    case ']':
        return 0x816E;
    case '^':
        return 0x814F;
    case '_':
        return 0x8151;
    case '`':
        return 0x814D;
    case '|':
        return 0x8162;
    case '~':
        c = 0x8160;
        break;
    }
    return c;
}

s8 g_parse_avtext_other_codes(u8 *str, u16 *out)
{
    int i;
    int shift;

    if (str[0] == 'a' && str[1] == '/')
    {
        *out = 0;
        return 1;
    }
    if (str[0] == 'p' && str[1] == '/')
    {
        if (str[2] == 'L')
        {
            *out = 0x83AD;
            return 7;
        }
        if (str[9] == 'A')
        {
            *out = 0x839F;
            return 10;
        }
        if (str[9] == 'B')
        {
            *out = 0x83A0;
            return 10;
        }
    }
    if (*str != '/')
        return 0;
    str++;
    if (*str != 'c')
        return 0;
    str++;
    if (*str != 'd')
        return 0;
    if (str[5] != '/')
        return 0;
    *out = 0;
    str++;
    for (shift = 12, i = 0; i < 4; i++, shift -= 4)
    {
        switch (str[i])
        {
        case '0':
            break;
        case '1':
            *out |= 1 << shift;
            break;
        case '2':
            *out |= 2 << shift;
            break;
        case '3':
            *out |= 3 << shift;
            break;
        case '4':
            *out |= 4 << shift;
            break;
        case '5':
            *out |= 5 << shift;
            break;
        case '6':
            *out |= 6 << shift;
            break;
        case '7':
            *out |= 7 << shift;
            break;
        case '8':
            *out |= 8 << shift;
            break;
        case '9':
            *out |= 9 << shift;
            break;
        case 'A':
            *out |= 10 << shift;
            break;
        case 'a':
            *out |= 10 << shift;
            break;
        case 'B':
            *out |= 11 << shift;
            break;
        case 'b':
            *out |= 11 << shift;
            break;
        case 'C':
            *out |= 12 << shift;
            break;
        case 'c':
            *out |= 12 << shift;
            break;
        case 'D':
            *out |= 13 << shift;
            break;
        case 'd':
            *out |= 13 << shift;
            break;
        case 'E':
            *out |= 14 << shift;
            break;
        case 'e':
            *out |= 14 << shift;
            break;
        case 'F':
            *out |= 15 << shift;
            break;
        case 'f':
            *out |= 15 << shift;
            break;
        }
    }
    return 7;
}

s8 parse_avtext_color_codes(u8 *str, struct SpriteDrawRequest *req)
{
    int color = 0;
    int i;
    int shift;
    int mode;
    s8 ret;

    if (str[0] == 'c' && str[1] == '/' && str[2] == '0' && str[3] == 'x')
    {
        mode = 0;
        str += 4;
        ret = 10;
    }
    else
    {
        if (*str++ != '/')
            return 0;
        if (*str != 'b' && *str != 'o')
            return 0;
        if (*str == 'b')
            mode = 0;
        else
            mode = 1;
        str++;
        if (*str != 'c')
            return 0;
        if (str[7] != '/')
            return 0;
        str++;
        ret = 9;
    }
    for (shift = 0x14, i = 0; i < 6; i++, shift -= 4)
    {
        switch (str[i])
        {
        case '0':
            break;
        case '1':
            color |= 1 << shift;
            break;
        case '2':
            color |= 2 << shift;
            break;
        case '3':
            color |= 3 << shift;
            break;
        case '4':
            color |= 4 << shift;
            break;
        case '5':
            color |= 5 << shift;
            break;
        case '6':
            color |= 6 << shift;
            break;
        case '7':
            color |= 7 << shift;
            break;
        case '8':
            color |= 8 << shift;
            break;
        case '9':
            color |= 9 << shift;
            break;
        case 'A':
            color |= 10 << shift;
            break;
        case 'a':
            color |= 10 << shift;
            break;
        case 'B':
            color |= 11 << shift;
            break;
        case 'b':
            color |= 11 << shift;
            break;
        case 'C':
            color |= 12 << shift;
            break;
        case 'c':
            color |= 12 << shift;
            break;
        case 'D':
            color |= 13 << shift;
            break;
        case 'd':
            color |= 13 << shift;
            break;
        case 'E':
            color |= 14 << shift;
            break;
        case 'e':
            color |= 14 << shift;
            break;
        case 'F':
            color |= 15 << shift;
            break;
        case 'f':
            color |= 15 << shift;
            break;
        }
    }
    if (mode == 0)
    {
        if (req != NULL)
            req->mulColor = color;
    }
    else
    {
        if (req != NULL)
            req->addColor = color;
    }
    return ret;
}

const struct AvTextGlyph g_some_avtext_array[] =
{
    { " ", 0, 9 },
    { "\x81\x41", 8, 8 },
    { "\x81\x42", 8, 8 },
    { "\x81\x43", 8, 8 },
    { "\x81\x44", 8, 8 },
    { "\x81\x45", 8, 8 },
    { "\x81\x46", 8, 8 },
    { "\x81\x47", 8, 8 },
    { "\x81\x48", 5, 14 },
    { "\x81\x49", 8, 8 },
    { "\x81\x4C", 8, 8 },
    { "\x81\x4D", 8, 8 },
    { "\x81\x4F", 6, 12 },
    { "\x81\x62", 9, 6 },
    { "\x81\x65", 8, 8 },
    { "\x81\x66", 8, 8 },
    { "\x81\x67", 5, 14 },
    { "\x81\x68", 5, 14 },
    { "\x81\x69", 8, 8 },
    { "\x81\x6A", 8, 8 },
    { "\x81\x6D", 7, 10 },
    { "\x81\x6E", 7, 10 },
    { "\x82\x4F", 5, 14 },
    { "\x82\x50", 7, 10 },
    { "\x82\x51", 5, 14 },
    { "\x82\x52", 5, 14 },
    { "\x82\x53", 5, 14 },
    { "\x82\x54", 5, 14 },
    { "\x82\x55", 5, 14 },
    { "\x82\x56", 5, 14 },
    { "\x82\x57", 5, 14 },
    { "\x82\x58", 5, 14 },
    { "\x82\x81", 5, 14 },
    { "\x82\x82", 5, 14 },
    { "\x82\x83", 5, 14 },
    { "\x82\x84", 5, 14 },
    { "\x82\x85", 5, 14 },
    { "\x82\x86", 7, 10 },
    { "\x82\x87", 5, 14 },
    { "\x82\x88", 5, 14 },
    { "\x82\x89", 8, 8 },
    { "\x82\x8A", 8, 8 },
    { "\x82\x8B", 6, 12 },
    { "\x82\x8C", 8, 8 },
    { "\x82\x8D", 2, 20 },
    { "\x82\x8E", 5, 14 },
    { "\x82\x8F", 4, 16 },
    { "\x82\x90", 5, 14 },
    { "\x82\x91", 5, 14 },
    { "\x82\x92", 7, 10 },
    { "\x82\x93", 5, 14 },
    { "\x82\x94", 7, 10 },
    { "\x82\x95", 5, 14 },
    { "\x82\x96", 5, 14 },
    { "\x82\x97", 3, 18 },
    { "\x82\x98", 6, 12 },
    { "\x82\x99", 6, 12 },
    { "\x82\x9A", 6, 12 },
    { "\x82\x60", 3, 18 },
    { "\x82\x61", 4, 16 },
    { "\x82\x62", 3, 18 },
    { "\x82\x63", 3, 18 },
    { "\x82\x64", 4, 16 },
    { "\x82\x65", 4, 16 },
    { "\x82\x66", 3, 18 },
    { "\x82\x67", 4, 16 },
    { "\x82\x68", 8, 8 },
    { "\x82\x69", 5, 14 },
    { "\x82\x6A", 4, 16 },
    { "\x82\x6B", 4, 14 },
    { "\x82\x6C", 2, 20 },
    { "\x82\x6D", 4, 16 },
    { "\x82\x6E", 2, 20 },
    { "\x82\x6F", 4, 16 },
    { "\x82\x70", 2, 20 },
    { "\x82\x71", 4, 16 },
    { "\x82\x72", 4, 16 },
    { "\x82\x73", 3, 18 },
    { "\x82\x74", 4, 16 },
    { "\x82\x75", 4, 16 },
    { "\x82\x76", 1, 22 },
    { "\x82\x77", 4, 16 },
    { "\x82\x78", 4, 16 },
    { "\x82\x79", 4, 16 },
    { "", 0, 0 },
    { NULL, 0, 0 },
};

u16 g_some_avtext_array_lookup(u16 chr, struct TextDrawInfo *info, struct FontParams *font)
{
    double unused = 1.0;
    u16 width = font->spaceWidth;
    const struct AvTextGlyph *desc;
    int found;
    s8 a;
    s8 b;

    if (!(info->flags & 0x80000000))
        return width;
    if (font->bmpId == -1)
    {
        desc = NULL;
        switch (info->fontId)
        {
        case 3:
            desc = g_some_avtext_array;
            break;
        }
        found = 0;
        if (desc != NULL)
        {
            if (chr >= 0x8000)
            {
                while (desc->unk5 != 0)
                {
                    if ((u8)desc->str[0] >= 0x80 && chr == (u16)(((u8)desc->str[0] << 8) | (u8)desc->str[1]))
                    {
                        found = 1;
                        break;
                    }
                    desc++;
                }
            }
            else
            {
                while (desc->unk5 != 0)
                {
                    if ((u8)desc->str[0] < 0x80 && (u8)desc->str[0] == chr)
                    {
                        found = 1;
                        break;
                    }
                    desc++;
                }
            }
            if (found != 0)
            {
                a = desc->unk4 - 1 > 0 ? desc->unk4 - 1 : 0;
                b = desc->unk5 + 2 < width ? desc->unk5 + 2 : width;
                width = (a << 8) | b;
            }
        }
    }
    return width;
}

float textdraw_print_internal_func_2(struct TextDrawInfo *info, float width, s16 count, float extra)
{
    if (info->flags & (1 << 23))
        return info->x;
    switch (info->alignment & 0xF)
    {
    case 6:
    case 7:
    case 8:
        return info->x - width - info->spacingX * (count - 1) - extra * (count - 1);
    case 3:
    case 4:
    case 5:
        return info->x - 0.5 * width - 0.5 * (info->spacingX * (count - 1)) - 0.5 * (extra * (count - 1));
    }
    return info->x;
}

float textdraw_print_internal_func_3(struct TextDrawInfo *info, struct FontParams *font, s16 count, int adjust)
{
    if (info->flags & (1 << 23))
        return info->y;
    switch (info->alignment & 0xF)
    {
    case 2:
    case 5:
    case 8:
        return info->y - count * (font->lineHeight * info->scaleY) - info->spacingY * (count - 1)
             + (adjust != 0 ? font->lineHeight * info->scaleY : 0.0);
    case 1:
    case 4:
    case 7:
        return info->y - 0.5 * (count * (font->lineHeight * info->scaleY)) - 0.5 * (info->spacingY * (count - 1))
             + (adjust != 0 ? 0.5 * (font->lineHeight * info->scaleY) : 0.0);
    }
    return info->y;
}

#pragma dont_inline on
int g_get_tex_id(struct SpriteDrawRequest *req, u16 c, u16 *p, struct TextDrawInfo *info)
{
    int step = 0x65;
    s8 first = 1;
    s8 count = 0;
    int idx = 0;

    for (;;)
    {
        if (count == g_related_to_aram_font_pages[info->fontId])
            break;
        if (*p > c)
        {
            if (first == 1 && step != 1)
                step >>= 1;
            idx -= step;
            p -= step;
            first = 0;
            count++;
        }
        else if (*p < c)
        {
            if (first == 0 && step != 1)
                step >>= 1;
            idx += step;
            p += step;
            if (idx >= g_related_to_aram_font_pages[info->fontId])
                idx += g_related_to_aram_font_pages[info->fontId] - idx;
            first = 1;
            count++;
        }
        else
            return idx;
    }
    OSReport("ARAM FONT SEARCH ERROR at get_tex_id [ch:%d]\n", c);
    return -1;
}
#pragma dont_inline off

static inline u16 get_glyph_width(struct SpriteDrawRequest *req, struct TextDrawInfo *info, struct FontParams *font)
{
    int idx = req->bmpId;
    int dflt = font->spaceWidth;

    if (info->fontId != 3)
        return dflt;
    if (!(info->flags & 0x80000000))
        return dflt;
    return lbl_1_bss_A575C[idx];
}

void textdraw_print_internal_func_1(char *str)
{
    struct TextDrawInfo *info;
    struct FontParams *font;
    u16 *p;
    s16 lineCount;
    u16 width;
    s16 lineLen;
    u16 count;
    u8 errors;
    u16 orig;
    int newline;
    s8 n;
    float lineWidth;
    float extra;
    float savedSpacing;
    struct SpriteDrawRequest req;
    struct SpriteDrawRequest saved;
    struct SpriteDrawRequest saved2;
    u16 c;

    errors = 0;
    count = 0;
    info = &textdraw_pos_x;
    font = &FONT_DEFINITIONS[info->fontId];
    req.bmpId = font->bmpId;
    req.z = info->depth;
    req.opacity = info->alpha;
    req.rotation = info->rotZ;
    req.mulColor = info->mulColor;
    req.addColor = info->addColor;
    req.unk30 = -1;
    req.flags = (info->flags & ~0xF) | 5;
    req.flags |= 0x400000;
    newline = 1;
    lineCount = textdraw_chara_load(str, 0, 3);
    savedSpacing = info->spacingX;

    for (; *str != 0; str++)
    {
        if (newline != 0)
        {
            newline = 0;
            lineWidth = (1.0 / font->unk34) * (info->scaleX * textdraw_chara_load(str, 1, 0));
            lineLen = textdraw_chara_load(str, 1, 1);
            if ((info->alignment & 0x10) && info->unk32 > lineWidth)
            {
                extra = (info->unk32 - lineWidth) / (float)(lineLen - 1);
                info->spacingX = 0.0f;
            }
            else
                extra = 0.0f;
        }
        if (count >= info->counter)
            break;
        c = (u8)*str;
        orig = c;
        if (c < 0x80)
        {
            if (c != '\n' && c != ' ')
            {
                n = parse_avtext_color_codes((u8 *)str, &req);
                if (n != 0)
                {
                    str += n;
                    continue;
                }
                n = g_parse_avtext_other_codes((u8 *)str, &c);
                if (c == 0)
                {
                    str += n;
                    continue;
                }
                if (n != 0)
                    str += n;
                if (n == 0)
                    c = g_parse_avtext_non_alphanumeric(c);
                orig = c;
            }
            if (c < 0x80)
            {
                width = g_some_avtext_array_lookup(c, info, font);
                if (c == '\n')
                {
                    info->x = info->startX;
                    info->y += info->spacingY + font->lineHeight * info->scaleY;
                    newline = 1;
                    continue;
                }
                if (c == ' ')
                {
                    if (g_textdraw_unk8.unk0 != 0)
                    {
                        req.x = textdraw_print_internal_func_2(info, lineWidth, lineLen, extra);
                        req.y = textdraw_print_internal_func_3(info, font, lineCount, 0);
                        fn_1_B7664(&req, info, font, width & 0xFF);
                    }
                    info->x += info->spacingX + (width & 0xFF) * info->scaleX;
                    count++;
                    continue;
                }
                if (c < font->firstChar || c > font->lastChar)
                {
                    info->x += info->spacingX + (width & 0xFF) * info->scaleX;
                    continue;
                }
            }
            else
            {
                int id;

                req.bmpId = -2;
                p = font->string;
                id = g_get_tex_id(&req, c, p, info);
                if (id != -1)
                {
                    req.bmpId = font->pageIdx[id];
                    width = get_glyph_width(&req, info, font);
                    if (req.bmpId != -1)
                        c = 0;
                }
            }
        }
        else
        {
            int j;

#ifdef __MWERKS__
            str++;
            c = (c << 8) | (u8)*str;
#else
            if (pc_text_is_lead(c))
            {
                str++;
                c = (c << 8) | (u8)*str;
            }
            else
                c = 0x8600 | c;
#endif
            if (c == 0x8140)
            {
                width = g_some_avtext_array_lookup(c, info, font);
                if (g_textdraw_unk8.unk0 != 0)
                {
                    req.x = textdraw_print_internal_func_2(info, lineWidth, lineLen, extra);
                    req.y = textdraw_print_internal_func_3(info, font, lineCount, 0);
                    fn_1_B7664(&req, info, font, width & 0xFF);
                }
                info->x += info->spacingX + (width & 0xFF) * info->scaleX;
                count++;
                continue;
            }
            req.bmpId = -2;
            for (j = 0, p = font->string; *p != 0; j++, p++)
            {
                if ((u16)*p == c)
                {
                    req.bmpId = font->pageIdx[j];
                    width = get_glyph_width(&req, info, font);
                    if (req.bmpId != -1)
                        c = 0;
                    break;
                }
            }
#ifndef __MWERKS__
            if (req.bmpId == -2 && info->fontId == 3 && (c >> 8) == 0x86)
            {
                s32 slot;
                if (pc_font_ext_glyph(c, &slot, &width))
                {
                    req.bmpId = slot;
                    c = 0;
                }
            }
#endif
        }
        if (req.bmpId < 0)
        {
            if (lbl_1_bss_A59D4 == 0)
            {
                if (dip_switches & 1)
                {
                    if (errors == 0)
                    {
                        OSReport("\n> %s\n", FONT_NAME_LIST[info->fontId]);
                        if (req.bmpId == -1)
                            OSReport("  is no load ...\n");
                        else
                            OSReport("  is no data ...\n");
                    }
                    if (errors % 4 == 0)
                        OSReport("  ");
                    OSReport("%X ", c);
                    if (errors % 4 == 3)
                        OSReport("\n");
                }
                errors++;
            }
        }
        else
        {
            int index;
            int row;
            int col;

            req.bmpId &= 0xFFFF;
            req.bmpId |= (info->fontId & 0xFF) << 16;
            req.bmpId |= ((s8)info->fontStyle & 0xFF) << 24;
            index = c - font->firstChar;
            row = index / font->charsPerRow;
            col = index % font->charsPerRow;
            req.x = textdraw_print_internal_func_2(info, lineWidth, lineLen, extra);
            req.y = textdraw_print_internal_func_3(info, font, lineCount, 0);
            {
                float u;
                float v;
                float kern = ((width >> 8) & 0xFF) * info->scaleX;
                float w = font->unk10;

                req.x -= kern;
                kern = 0.0f;
                u = font->unk10 * col;
                v = font->unk14 * row;
                req.u1 = u + kern;
                req.v1 = v;
                req.u2 = w + (u + kern);
                req.v2 = v + font->unk14;
                req.scaleX = info->scaleX * w;
                req.scaleY = info->scaleY * font->unk14;
                if (*p >= 0x839F && *p <= 0x83B8)
                {
                    float s = 1.25f * (0.5f * (info->scaleX + info->scaleY));
                    float cw = width & 0xFF;

                    s = (s < info->scaleX) ? info->scaleX : ((s > info->scaleY) ? info->scaleY : s);
                    req.scaleX = s * w;
                    req.scaleY = s * font->unk14;
                    req.x += cw * (0.5f * (info->scaleX - s));
                    req.y += font->lineHeight * (0.5f * (info->scaleY - s));
                }
            }
            if (orig == 0x8287 || orig == 0x93)
                req.y += 0.1f * (font->lineHeight * info->scaleY);
            if (g_textdraw_unk8.unk0 != 0)
                fn_1_B7664(&req, info, font, width & 0xFF);
            if (info->flags & (1 << 30))
            {
                memcpy_handler(&saved, &req, 0x50);
                req.x += info->shadowX * info->scaleX;
                req.y += info->shadowY * info->scaleY;
                req.z += 0.001f;
                req.opacity = info->alpha * info->shadowAlpha;
                req.mulColor = info->unk3C;
                draw_sprite_draw_request(&req);
                memcpy_handler(&req, &saved, 0x50);
            }
            else if (info->flags & (1 << 29))
            {
                float xo[4] = { 1.0f, 1.0f, -1.0f, -1.0f };
                float yo[4] = { -1.0f, 1.0f, 1.0f, -1.0f };
                int i;

                memcpy_handler(&saved2, &req, 0x50);
                req.opacity = info->alpha * info->unk40;
                req.mulColor = 0;
                req.z += 0.009f;
                for (i = 0; i < 4; i++)
                {
                    req.x = saved2.x + info->scaleX * xo[i];
                    req.y = saved2.y + info->scaleY * yo[i];
                    req.z -= 0.0015f;
                    draw_sprite_draw_request(&req);
                }
                memcpy_handler(&req, &saved2, 0x50);
            }
            {
            u32 savedColor = req.mulColor;
            if (*p >= 0x839F && *p <= 0x83B8 && (info->flags & (1 << 28)))
                req.mulColor = 0xFFFFFF;
            draw_sprite_draw_request(&req);
            if (*p >= 0x839F && *p <= 0x83B8 && (info->flags & (1 << 28)))
                req.mulColor = savedColor;
            }
        }
        info->x += extra + (info->spacingX + (width & 0xFF) * info->scaleX);
        count++;
    }

    if (c != '\n')
        info->y = textdraw_print_internal_func_3(info, font, lineCount, 1);
    info->spacingX = savedSpacing;
    if ((dip_switches & 1) && errors != 0)
        lbl_1_bss_A59D4 = 1;
}

#define CHARA_LOAD_WIDTH() \
    if (font->bmpId != -1) \
        width = chara_width(str, i, info, font); \
    else \
        width = g_some_avtext_array_lookup(c, info, font)
static inline u32 chara_width(char *str, int i, struct TextDrawInfo *info, struct FontParams *font)
{
    u32 w = g_get_font_char_width(str + i, info->fontId, font);
    float ratio = g_get_font_char_width_scaling(str + i, info->fontId);

    if (ratio != 1.0)
        w += ratio * font->spaceWidth - font->spaceWidth;
    return w;
}

float textdraw_chara_load(char *str, int single, s8 kind)
{
    int slot;
    int fontId;
    struct FontParams *font;
    int i;
    struct TextDrawInfo *info;
    s8 n;
    u32 width;
    int found;
    u16 *p;
    int j;
    int k;
    int fmt;
    float a;
    float b;
    float d;
    float e;
    u16 c;

    info = &textdraw_pos_x;
    font = &FONT_DEFINITIONS[info->fontId];
    if (kind == 0 || kind == 4)
    {
        a = 0.0f;
        b = 0.0f;
    }
    else if (kind == 1)
    {
        d = 0.0f;
        e = 0.0f;
    }
    else if (kind == 3)
        d = 0.0f;
    else if (kind == 2)
        d = 1.0f;
    else if (kind == 5)
        d = 0.0f;

    for (i = 0; str[i] != 0; i++)
    {
        c = (u8)str[i];
        if (kind == 0 && kind == 4)
        {
            CHARA_LOAD_WIDTH();
        }
        if (c < 0x80)
        {
            n = parse_avtext_color_codes((u8 *)str + i, NULL);
            if (n != 0)
            {
                i += n;
                continue;
            }
            n = g_parse_avtext_other_codes((u8 *)str + i, &c);
            if (c == 0)
            {
                i += n;
                continue;
            }
            if (n != 0)
                i += n;
            if (n == 0)
                c = g_parse_avtext_non_alphanumeric(c);
            if (c == '\n')
            {
                if (single != 0)
                    break;
                if (kind == 0)
                {
                    if (a > b)
                        b = a;
                    a = 0.0f;
                }
                else if (kind == 1)
                {
                    if (d > e)
                        e = d;
                    d = 0.0f;
                }
                else if (kind == 3)
                    d += 1.0f;
                continue;
            }
            if (kind == 2 && c == ' ')
                continue;
            if (kind == 0 || kind == 4)
            {
                CHARA_LOAD_WIDTH();
            }
        }
        else
        {
#ifdef __MWERKS__
            i++;
            if (kind != 3)
            {
                if (kind == 2 && c == 0x8140)
                    continue;
                if (kind != 1)
                    c = (c << 8) | (u8)str[i];
                if (kind == 0 || kind == 4)
                    width = g_some_avtext_array_lookup(c, info, font);
            }
#else
            int lead = pc_text_is_lead(c);

            if (lead)
                i++;
            if (kind != 3)
            {
                if (kind == 2 && c == 0x8140)
                    continue;
                if (kind != 1)
                    c = lead ? ((c << 8) | (u8)str[i]) : (0x8600 | c);
                if ((c >> 8) == 0x86 && info->fontId == 3)
                {
                    s32 slot;
                    u16 w;

                    if (kind == 2 || kind == 5)
                        continue;  /* composed on demand, nothing to load */
                    if ((kind == 0 || kind == 4) && pc_font_ext_glyph(c, &slot, &w))
                    {
                        width = w;
                        goto pc_have_width;
                    }
                }
                if (kind == 0 || kind == 4)
                    width = g_some_avtext_array_lookup(c, info, font);
            }
        pc_have_width:;
#endif
        }
        if (kind == 0)
            a += (u8)width;
        else if (kind == 4)
            a += (u8)width;
        else if (kind == 1)
            d += 1.0f;
        else if (kind == 2 || kind == 5)
        {
            found = 1;
            for (p = font->string, j = 0; *p != 0; j++, p++)
            {
                if ((u16)*p == c)
                {
                    if (kind == 2)
                    {
                        found = 0;
                        if (font->pageIdx[(u16)j] != -1)
                            continue;
                        d = 0.0f;
                        break;
                    }
                    fontId = info->fontId;
                    slot = -1;
                    for (k = 0; k < FONT_SLOT_COUNT; k++)
                    {
                        if (g_some_font_ptr_array[k] == 0)
                        {
                            slot = k;
                            break;
                        }
                    }
                    if (slot == -1)
                    {
                        if (dip_switches & 1)
                        {
                            OSReport("\n> %s\n", FONT_NAME_LIST[fontId]);
                            OSReport("  chara No.%d can't load.\n", i);
                            OSReport("  ARAM FONT BUFFER OVER!!\n\n");
                        }
                    }
                    else
                    {
                        g_some_font_ptr_array[slot] = (u32)OSAllocFromHeap(currentHeap, font->size);
                        if (g_some_font_ptr_array[slot] == 0)
                            OSPanic("font.c", 0xF8, "cannot OSAlloc at chara_load()\n");
                        aram_font_to_mram_font(fontId, i, (void *)g_some_font_ptr_array[slot]);
                        fmt = 2;
                        if (font->string[i] >= 0x839F && font->string[i] <= 0x83BE)
                            fmt = 0xE;
                        GXInitTexObj(&lbl_1_bss_A1704[slot], (void *)g_some_font_ptr_array[slot], font->spaceWidth, font->lineHeight, fmt, 0, 0, 0);
                        font->pageIdx[i] = slot;
                    }
                    d += 1.0f;
                    break;
                }
            }
            if (kind == 2)
            {
                if (found != 0)
                    d = 0.0f;
                if (0.0f == d)
                    break;
            }
        }
    }

    if (kind == 0)
    {
        if (a > b)
            b = a;
        a = b * font->unk34;
        return a;
    }
    if (kind == 4)
        return a;
    if (kind == 1)
    {
        if (d > e)
            e = d;
        return e;
    }
    if (kind == 3)
    {
        if (c != '\n')
            d += 1.0f;
        return d;
    }
    if (kind == 2 || kind == 5)
        return d;
    return 0.0f;
}

float fn_1_B74CC(char *str)
{
    return textdraw_chara_load(str, 1, 4);
}

float g_get_string_sprite_width_3(char *str)
{
    return textdraw_pos_x.scaleX * fn_1_B74CC(str);
}

float textdraw_get_pixel_width_of_string_as_f32_child(char *str)
{
    return textdraw_chara_load(str, 0, 0);
}

float textdraw_get_pixel_width_of_string_child(char *str)
{
    return textdraw_pos_x.scaleX * textdraw_get_pixel_width_of_string_as_f32_child(str);
}

int g_smth_with_fonts_chara_load_wrapper(char *str)
{
    return textdraw_chara_load(str, 0, 1);
}

int textdraw_get_line_count_of_string_child(char *str)
{
    return textdraw_chara_load(str, 0, 3);
}

void fn_1_B75F4(void)
{
    g_textdraw_unk8.unk0 = 1;
    g_textdraw_unk8.unk4 = 640.0f;
    g_textdraw_unk8.unk8 = 0.0f;
    g_textdraw_unk8.unkC = 480.0f;
    g_textdraw_unk8.unk10 = 0.0f;
}

void fn_1_B7650(void)
{
    g_textdraw_unk8.unk0 = 0;
}

void fn_1_B7664(struct SpriteDrawRequest *req, struct TextDrawInfo *info, struct FontParams *font, int width)
{
    float x = req->x;
    float y;
    float v;

    if (x < g_textdraw_unk8.unk4)
        g_textdraw_unk8.unk4 = x;
    v = req->x + (float)width * info->scaleX;
    if (v > g_textdraw_unk8.unk8)
        g_textdraw_unk8.unk8 = v;
    y = req->y;
    if (y < g_textdraw_unk8.unkC)
        g_textdraw_unk8.unkC = y;
    v = req->y + font->lineHeight * info->scaleY;
    if (v > g_textdraw_unk8.unk10)
        g_textdraw_unk8.unk10 = v;
}
