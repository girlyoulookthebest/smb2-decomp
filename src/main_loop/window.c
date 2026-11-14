/* window.c: debug text windows and the debug menu (item editing, dip switches). */
#include "event.h"
#include "types.h"
#include "macros.h"
#include "libc.h"
#include "input.h"
#include "gx.h"
#include "light.h"
#include "sound.h"
#include "dip.h"
#include "va.h"
#include "bitmap.h"
#include "ball.h"
#include "skeleton.h"
#include "pool.h"
#include "recplay.h"
#include "futsal_data.h"
#include "item.h"

/* A debug menu item. */
struct DebugMenuItem
{
    /*0x00*/ u32 type;  /* low byte: DMI_*; 0x200: value isn't indexed by the window's index */
    /*0x04*/ s32 x;  /* > 0: relative to the window, otherwise relative to the previous item */
    /*0x08*/ s32 y;
    /*0x0C*/ char *fmt;
    /*0x10*/ void *ptr;
    /*0x14*/ void *limits;  /* struct DebugMenuIntLimits / FloatLimits, or a bit mask */
}; /* size = 0x18 */

enum
{
    DMI_SUBMENU,
    DMI_S32,
    DMI_U32,
    DMI_U16,
    DMI_S16,
    DMI_U8,
    DMI_S8,
    DMI_FLOAT,
    DMI_FLAG,
    DMI_ITEMS,
    DMI_INDEX,
    DMI_WINDOW,
    DMI_END = 0x24,
};

struct DebugMenuIntLimits
{
    u32 flags;  /* 1: unbounded below, 2: unbounded above */
    s32 min;
    s32 max;
    s32 step;
};

struct DebugMenuFloatLimits
{
    u32 flags;
    float min;
    float max;
    float step;
};

/* Window definition (as stored in menu data). */
struct DebugWindowDesc
{
    /*0x00*/ s32 x;
    /*0x04*/ s32 y;
    /*0x08*/ s32 w;
    /*0x0C*/ s32 h;
    /*0x10*/ struct DebugMenuItem *items;
    /*0x14*/ s32 stride;  /* bytes between values selected by index */
    /*0x18*/ s32 index;
    /*0x1C*/ s32 cursor;
};

struct DebugWindow
{
    /*0x00*/ struct DebugWindowDesc desc;
    /*0x20*/ BOOL active;
    /*0x24*/ s32 moveX;
    /*0x28*/ s32 moveY;
}; /* size = 0x2C */

s32 g_debug_textdraw_pos_x_2;
s32 g_debug_textdraw_pos_x_1;
s32 g_debug_textdraw_pos_y;
s8 lbl_1_bss_6C7B4;
s8 lbl_1_bss_6C7B5;
s32 g_debug_clip_x;  /* text clip rectangle */
s32 g_debug_clip_y;
s32 g_debug_clip_w;
s32 g_debug_clip_h;
s32 lbl_1_bss_6C7C8;
s32 lbl_1_bss_6C7CC;
s32 lbl_1_bss_6C7D0;
s32 lbl_1_bss_6C7D4;
s32 lbl_1_bss_6C7D8;
s32 lbl_1_bss_6C7DC;
u32 lbl_1_bss_6C7E0;  /* debug windows visible */
struct DebugWindow *lbl_1_bss_6C7E4[16];  /* open windows, front first */
s32 lbl_1_bss_6C824;  /* focused window */
struct DebugWindow lbl_1_bss_6C828[16];
BOOL lbl_1_bss_6CAE8;  /* cursor up */
BOOL lbl_1_bss_6CAEC;  /* cursor down */
/* 53x37 character grids: buffer 0 is the window layer, 1 the text layer, 2 the overlay. */
u8 g_debug_text_buffer_0[37 * 53];
u8 g_debug_text_buffer_1[37 * 53];
u8 g_debug_text_color_buffer_0[37 * 53];
u8 g_debug_text_color_buffer_1[37 * 53];
s16 g_debug_overlay_x0;  /* region of buffer 2 drawn shifted */
s16 g_debug_overlay_y0;
s16 g_debug_overlay_x1;
s16 g_debug_overlay_y1;
u8 g_debug_text_buffer_2[37 * 53];
u8 g_debug_text_color_buffer_2[37 * 53];
u16 os_font_encoding;
extern u8 *g_debug_text_buffers[];
extern u8 *g_debug_color_bufers[];
BOOL lbl_1_bss_6F900;  /* cursor left */
BOOL lbl_1_bss_6F904;  /* cursor right */
s8 lbl_1_bss_6F908;  /* edited light's spot function */
s8 lbl_1_bss_6F909;  /* edited sound's volume */
s8 lbl_1_bss_6F90A;  /* edited track's volume */
BOOL lbl_1_bss_6F90C;  /* play the selected sound */
BOOL lbl_1_bss_6F910;  /* stop the selected sound */
BOOL lbl_1_bss_6F914;  /* apply aux settings */
u8 lbl_1_bss_6F918;  /* reverb */
u8 lbl_1_bss_6F919;  /* chorus */
char *lbl_1_bss_6F91C;  /* aux status text */
/* Values edited through the debug menu. */
u8 lbl_1_bss_6F920[0x100];
u32 lbl_1_bss_6FA20;
u32 lbl_1_bss_6FA24;
u8 lbl_1_bss_6FA28;
u8 lbl_1_bss_6FA29;
u32 lbl_1_bss_6FA2C;
u32 lbl_1_bss_6FA30;
u32 lbl_1_bss_6FA34;
u32 lbl_1_bss_6FA38;
u32 lbl_1_bss_6FA3C;
u8 lbl_1_bss_6FA40;
u8 lbl_1_bss_6FA41;
u16 lbl_1_bss_6FA42;
u8 lbl_1_bss_6FA44[0x8];
u32 lbl_1_bss_6FA4C;
u32 lbl_1_bss_6FA50;
u32 lbl_1_bss_6FA54;
u32 lbl_1_bss_6FA58;
u32 lbl_1_bss_6FA5C;
u32 lbl_1_bss_6FA60;
u8 lbl_1_bss_6FA64[0x8];
u32 lbl_1_bss_6FA6C;
u32 lbl_1_bss_6FA70;
u16 lbl_1_bss_6FA74;
u16 lbl_1_bss_6FA76;
u16 lbl_1_bss_6FA78;
u16 lbl_1_bss_6FA7A;
u32 lbl_1_bss_6FA7C;
u32 lbl_1_bss_6FA80;
u32 lbl_1_bss_6FA84;
u32 lbl_1_bss_6FA88;
u32 lbl_1_bss_6FA8C;
u32 lbl_1_bss_6FA90;
u16 lbl_1_bss_6FA94;
u16 lbl_1_bss_6FA96;
u32 lbl_1_bss_6FA98;
u32 lbl_1_bss_6FA9C;
u16 lbl_1_bss_6FAA0;
u16 lbl_1_bss_6FAA2;
u16 lbl_1_bss_6FAA4;
u16 lbl_1_bss_6FAA6;
u16 lbl_1_bss_6FAA8;
u16 lbl_1_bss_6FAAA;
u32 lbl_1_bss_6FAAC;
u32 lbl_1_bss_6FAB0;
u32 lbl_1_bss_6FAB4;
u32 lbl_1_bss_6FAB8;
u32 lbl_1_bss_6FABC;
u32 lbl_1_bss_6FAC0;
u32 lbl_1_bss_6FAC4;
u32 lbl_1_bss_6FAC8;
u32 lbl_1_bss_6FACC;
u32 lbl_1_bss_6FAD0;
u32 lbl_1_bss_6FAD4;
u32 lbl_1_bss_6FAD8;
u32 lbl_1_bss_6FADC;
u32 lbl_1_bss_6FAE0;
u16 lbl_1_bss_6FAE4;
u16 lbl_1_bss_6FAE6;
u8 lbl_1_bss_6FAE8[0x7C];
u32 lbl_1_bss_6FB64;
u32 lbl_1_bss_6FB68;
u32 lbl_1_bss_6FB6C;
u32 lbl_1_bss_6FB70;
u32 lbl_1_bss_6FB74;
u32 lbl_1_bss_6FB78;
u32 lbl_1_bss_6FB7C;
u32 lbl_1_bss_6FB80;
u32 lbl_1_bss_6FB84;
u16 lbl_1_bss_6FB88;
u16 lbl_1_bss_6FB8A;
u32 lbl_1_bss_6FB8C;

extern struct Light s_lightPool[];
extern s32 u_lightToPrint;
extern struct LightGroup s_lightGroups[];
/* Light editor state (defined elsewhere). */
extern struct Color3f lbl_1_bss_6F18;
extern s8 lbl_1_bss_6F24[8];
extern s8 lbl_1_bss_6F2C;
extern s8 lbl_1_bss_6F2D;
extern struct Light lbl_1_bss_6F30;
extern s32 lbl_1_bss_6F7C;
extern struct DebugWindow lbl_1_data_2D258;  /* main debug menu */
extern char lbl_1_data_2CDD4[];  /* replay file name */

typedef int OSHeapHandle;
long OSCheckHeap(OSHeapHandle heap);
extern OSHeapHandle main_heap;
extern OSHeapHandle stage_heap;
extern OSHeapHandle bg_heap;
extern OSHeapHandle chara_heap;
extern OSHeapHandle replay_heap;
extern long main_heap_size;
extern long stage_heap_size;
extern long bg_heap_size;
extern long chara_heap_size;
extern long replay_heap_size;
extern char *EVENT_STATUS_NAMES[];

void g_draw_debug_window_boundaries(struct DebugWindowDesc *desc);
void draw_debugtext_char_en(int x, int y, u8 chr, u8 color);
void draw_debugtext_char_jp(int x, int y, u8 *chr, u8 color);
void draw_debugtext_window_bg(float x0, float y0, float x1, float y1);
BOOL g_is_sjis_and_more_than_24_mib(void);
void GXSetTevColor(int reg, GXColor color);
u16 OSGetFontEncode(void);
void g_very_similar_to_puts(char *str);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
extern s32 is_more_than_24_mib;
int strcmp(const char *a, const char *b);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp,
                     u8 edgeLod, int maxAniso);

/* Dolphin OS ROM font header (subset). */
typedef struct
{
    u8 unk0[0x18];
    /*0x18*/ u16 sheetFormat;
    /*0x1A*/ u16 sheetColumn;
    /*0x1C*/ u16 sheetRow;
    /*0x1E*/ u16 sheetWidth;
    /*0x20*/ u16 sheetHeight;
} OSFontHeader;

extern OSFontHeader *lbl_1_bss_D2B7C;  /* loaded Shift-JIS ROM font */
struct Ape *get_ape_by_idx(int idx);
void fn_1_1157E4(s32 a, s32 b);
struct ApeFlagInfo *g_get_ape_flag(s32 a);
char *get_ape_lod_string(struct Ape *ape);
char *get_ape_chara_name(struct Ape *ape);
char *g_get_ape_skinning_or_stitching_name(struct Ape *ape);
char *get_ape_face_expr_name(struct Ape *ape);
char *get_ape_scene_name(struct Ape *ape);
char *fn_1_1137D8(struct Ape *ape);
char *get_ape_anim_flags(struct Ape *ape);
extern char *lbl_1_data_6ECB4[];
extern void *currentHeap;
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void OSFreeToHeap(void *heap, void *ptr);
void *fn_1_6333C(int replayId);
u32 fn_1_63354(void *replay);
void fn_1_6335C(void *replay, void *buf);
void *fn_8006F330(char *path, char *mode);  /* fopen */
#ifdef __MWERKS__
int fclose(void *fp);
u32 fwrite(void *buf, u32 size, u32 count, void *fp);
#endif
void fn_1_3FDF0(char *msg);
u32 get_compressed_replay_size(void);
void compress_replay(void *buf);
u32 get_gx_fifo_use_size(void);
u32 get_max_gx_fifo_use_size(void);  /* robot control state names */
char *fn_8000F954(u8 *string, void **image, s32 *x, s32 *y, s32 *width);  /* OSGetFontTexture */
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_set_color(u8 color);
int debug_window_printf(int buf, char *fmt, ...);
void debug_textdraw_put(int buf, char *str);
void fn_1_3E658(int x, int y, int w, int h);
void g_debug_print_ape(struct DebugWindowDesc *desc);
void fn_1_3FB0C(struct DebugWindowDesc *desc, s32 arg);
void debug_overlay_draw_fifo(struct DebugWindowDesc *desc);
void fn_1_3E13C(int moveX, int moveY, int x, int y, int w, int h);
void fn_1_3FC40(int replayId, char *msg);
void fn_1_3BAF8(struct DebugWindow *win);
void fn_1_32338(int reverb, int chorus);

#define DEBUG_INPUT(bit) \
    ((analog_inputs[0][4] & (bit)) || ((analog_inputs[0][0] & (bit)) && (analog_inputs[0][0] & 0x200)))

/* The original laid .bss out in definition order (MWCC orders it by first use); these
 * unreferenced placeholders are stripped at link time. */
FORCE_BSS_ORDER(g_debug_textdraw_pos_x_2)
FORCE_BSS_ORDER(g_debug_textdraw_pos_x_1)
FORCE_BSS_ORDER(g_debug_textdraw_pos_y)
FORCE_BSS_ORDER(lbl_1_bss_6C7B4)
FORCE_BSS_ORDER(lbl_1_bss_6C7B5)
FORCE_BSS_ORDER(g_debug_clip_x)
FORCE_BSS_ORDER(g_debug_clip_y)
FORCE_BSS_ORDER(g_debug_clip_w)
FORCE_BSS_ORDER(g_debug_clip_h)
FORCE_BSS_ORDER(lbl_1_bss_6C7C8)
FORCE_BSS_ORDER(lbl_1_bss_6C7CC)
FORCE_BSS_ORDER(lbl_1_bss_6C7D0)
FORCE_BSS_ORDER(lbl_1_bss_6C7D4)
FORCE_BSS_ORDER(lbl_1_bss_6C7D8)
FORCE_BSS_ORDER(lbl_1_bss_6C7DC)
FORCE_BSS_ORDER(lbl_1_bss_6C7E0)
FORCE_BSS_ORDER(lbl_1_bss_6C7E4)
FORCE_BSS_ORDER(lbl_1_bss_6C824)
FORCE_BSS_ORDER(lbl_1_bss_6C828)
FORCE_BSS_ORDER(lbl_1_bss_6CAE8)
FORCE_BSS_ORDER(lbl_1_bss_6CAEC)
FORCE_BSS_ORDER(g_debug_text_buffer_0)
FORCE_BSS_ORDER(g_debug_text_buffer_1)
FORCE_BSS_ORDER(g_debug_text_color_buffer_0)
FORCE_BSS_ORDER(g_debug_text_color_buffer_1)
FORCE_BSS_ORDER(g_debug_overlay_x0)
FORCE_BSS_ORDER(g_debug_overlay_y0)
FORCE_BSS_ORDER(g_debug_overlay_x1)
FORCE_BSS_ORDER(g_debug_overlay_y1)
FORCE_BSS_ORDER(g_debug_text_buffer_2)
FORCE_BSS_ORDER(g_debug_text_color_buffer_2)
FORCE_BSS_ORDER(os_font_encoding)
FORCE_BSS_ORDER(lbl_1_bss_6F900)
FORCE_BSS_ORDER(lbl_1_bss_6F904)
FORCE_BSS_ORDER(lbl_1_bss_6F908)
FORCE_BSS_ORDER(lbl_1_bss_6F909)
FORCE_BSS_ORDER(lbl_1_bss_6F90A)
FORCE_BSS_ORDER(lbl_1_bss_6F90C)
FORCE_BSS_ORDER(lbl_1_bss_6F910)
FORCE_BSS_ORDER(lbl_1_bss_6F914)
FORCE_BSS_ORDER(lbl_1_bss_6F918)
FORCE_BSS_ORDER(lbl_1_bss_6F919)
FORCE_BSS_ORDER(lbl_1_bss_6F91C)
FORCE_BSS_ORDER(lbl_1_bss_6F920)
FORCE_BSS_ORDER(lbl_1_bss_6FA20)
FORCE_BSS_ORDER(lbl_1_bss_6FA24)
FORCE_BSS_ORDER(lbl_1_bss_6FA28)
FORCE_BSS_ORDER(lbl_1_bss_6FA29)
FORCE_BSS_ORDER(lbl_1_bss_6FA2C)
FORCE_BSS_ORDER(lbl_1_bss_6FA30)
FORCE_BSS_ORDER(lbl_1_bss_6FA34)
FORCE_BSS_ORDER(lbl_1_bss_6FA38)
FORCE_BSS_ORDER(lbl_1_bss_6FA3C)
FORCE_BSS_ORDER(lbl_1_bss_6FA40)
FORCE_BSS_ORDER(lbl_1_bss_6FA41)
FORCE_BSS_ORDER(lbl_1_bss_6FA42)
FORCE_BSS_ORDER(lbl_1_bss_6FA44)
FORCE_BSS_ORDER(lbl_1_bss_6FA4C)
FORCE_BSS_ORDER(lbl_1_bss_6FA50)
FORCE_BSS_ORDER(lbl_1_bss_6FA54)
FORCE_BSS_ORDER(lbl_1_bss_6FA58)
FORCE_BSS_ORDER(lbl_1_bss_6FA5C)
FORCE_BSS_ORDER(lbl_1_bss_6FA60)
FORCE_BSS_ORDER(lbl_1_bss_6FA64)
FORCE_BSS_ORDER(lbl_1_bss_6FA6C)
FORCE_BSS_ORDER(lbl_1_bss_6FA70)
FORCE_BSS_ORDER(lbl_1_bss_6FA74)
FORCE_BSS_ORDER(lbl_1_bss_6FA76)
FORCE_BSS_ORDER(lbl_1_bss_6FA78)
FORCE_BSS_ORDER(lbl_1_bss_6FA7A)
FORCE_BSS_ORDER(lbl_1_bss_6FA7C)
FORCE_BSS_ORDER(lbl_1_bss_6FA80)
FORCE_BSS_ORDER(lbl_1_bss_6FA84)
FORCE_BSS_ORDER(lbl_1_bss_6FA88)
FORCE_BSS_ORDER(lbl_1_bss_6FA8C)
FORCE_BSS_ORDER(lbl_1_bss_6FA90)
FORCE_BSS_ORDER(lbl_1_bss_6FA94)
FORCE_BSS_ORDER(lbl_1_bss_6FA96)
FORCE_BSS_ORDER(lbl_1_bss_6FA98)
FORCE_BSS_ORDER(lbl_1_bss_6FA9C)
FORCE_BSS_ORDER(lbl_1_bss_6FAA0)
FORCE_BSS_ORDER(lbl_1_bss_6FAA2)
FORCE_BSS_ORDER(lbl_1_bss_6FAA4)
FORCE_BSS_ORDER(lbl_1_bss_6FAA6)
FORCE_BSS_ORDER(lbl_1_bss_6FAA8)
FORCE_BSS_ORDER(lbl_1_bss_6FAAA)
FORCE_BSS_ORDER(lbl_1_bss_6FAAC)
FORCE_BSS_ORDER(lbl_1_bss_6FAB0)
FORCE_BSS_ORDER(lbl_1_bss_6FAB4)
FORCE_BSS_ORDER(lbl_1_bss_6FAB8)
FORCE_BSS_ORDER(lbl_1_bss_6FABC)
FORCE_BSS_ORDER(lbl_1_bss_6FAC0)
FORCE_BSS_ORDER(lbl_1_bss_6FAC4)
FORCE_BSS_ORDER(lbl_1_bss_6FAC8)
FORCE_BSS_ORDER(lbl_1_bss_6FACC)
FORCE_BSS_ORDER(lbl_1_bss_6FAD0)
FORCE_BSS_ORDER(lbl_1_bss_6FAD4)
FORCE_BSS_ORDER(lbl_1_bss_6FAD8)
FORCE_BSS_ORDER(lbl_1_bss_6FADC)
FORCE_BSS_ORDER(lbl_1_bss_6FAE0)
FORCE_BSS_ORDER(lbl_1_bss_6FAE4)
FORCE_BSS_ORDER(lbl_1_bss_6FAE6)
FORCE_BSS_ORDER(lbl_1_bss_6FAE8)
FORCE_BSS_ORDER(lbl_1_bss_6FB64)
FORCE_BSS_ORDER(lbl_1_bss_6FB68)
FORCE_BSS_ORDER(lbl_1_bss_6FB6C)
FORCE_BSS_ORDER(lbl_1_bss_6FB70)
FORCE_BSS_ORDER(lbl_1_bss_6FB74)
FORCE_BSS_ORDER(lbl_1_bss_6FB78)
FORCE_BSS_ORDER(lbl_1_bss_6FB7C)
FORCE_BSS_ORDER(lbl_1_bss_6FB80)
FORCE_BSS_ORDER(lbl_1_bss_6FB84)
FORCE_BSS_ORDER(lbl_1_bss_6FB88)
FORCE_BSS_ORDER(lbl_1_bss_6FB8A)
FORCE_BSS_ORDER(lbl_1_bss_6FB8C)

u8 g_debug_textdraw_color = 0xFF;

#include "window_menu.inc"

/* Open a window from a definition. */
void fn_1_3BA00(struct DebugWindow *template)
{
    int i;
    struct DebugWindow *win;
    struct DebugWindow *prev;
    struct DebugWindow **slot;

    lbl_1_bss_6C7E0 = TRUE;
    if (lbl_1_bss_6C7E4[15] != NULL)
        return;
    for (i = 0; i < 16; i++)
    {
        if (!lbl_1_bss_6C828[i].active)
            break;
    }
    if (i >= 16)
        return;
    win = &lbl_1_bss_6C828[i];
    win->desc.x = template->desc.x;
    win->desc.y = template->desc.y;
    win->desc.w = template->desc.w;
    win->desc.h = template->desc.h;
    win->desc.items = template->desc.items;
    win->desc.stride = template->desc.stride;
    win->desc.index = template->desc.index;
    win->desc.cursor = template->desc.cursor;
    win->active = TRUE;
    win->moveX = template->moveX;
    win->moveY = template->moveY;
    prev = win;
    for (i = 0, slot = lbl_1_bss_6C7E4; i < 16; i++, slot++)
    {
        win = *slot;
        *slot = prev;
        prev = win;
        if (prev == NULL)
            break;
    }
    lbl_1_bss_6C824 = 0;
}

/* Window input: move/resize with the D-pad (holding B), move the cursor, and edit
 * the selected item with A/B. */
void fn_1_3BAF8(struct DebugWindow *win)
{
    float fval;
    float scaled;
    int min;
    float rounded;
    int digits;
    int max;
    float fmin;
    int pos;
    struct DebugMenuItem *item;
    float scale;
    int cursor;
    struct DebugWindowDesc *desc;
    u8 *addr;
    float fmax;
    int step;
    int mult;
    u32 flags;
    float fstep;
    int val;

    if (DEBUG_INPUT(0x80))
    {
        win->desc.h--;
        win->desc.h = (win->desc.h > 4) ? win->desc.h : 4;
    }
    else if (DEBUG_INPUT(0x40))
    {
        win->desc.h++;
        win->desc.h = (win->desc.h < 0x25 - win->desc.y) ? win->desc.h : 0x25 - win->desc.y;
    }
    if (DEBUG_INPUT(0x10))
    {
        win->desc.w--;
        win->desc.w = (win->desc.w > 4) ? win->desc.w : 4;
    }
    else if (DEBUG_INPUT(0x20))
    {
        win->desc.w++;
        win->desc.w = (win->desc.w < 0x35 - win->desc.x) ? win->desc.w : 0x35 - win->desc.x;
    }
    if (DEBUG_INPUT(0x8))
    {
        pos = win->desc.y - 1;
        pos = (pos > 0) ? pos : 0;
        win->moveY += (win->desc.y - pos) * 12;
        win->desc.y = pos;
    }
    else if (DEBUG_INPUT(0x4))
    {
        pos = win->desc.y + 1;
        pos = (pos < 0x25 - win->desc.h) ? pos : 0x25 - win->desc.h;
        win->moveY += (win->desc.y - pos) * 12;
        win->desc.y = pos;
    }
    if (DEBUG_INPUT(0x1))
    {
        pos = win->desc.x - 1;
        pos = (pos > 0) ? pos : 0;
        win->moveX += (win->desc.x - pos) * 12;
        win->desc.x = pos;
    }
    else if (DEBUG_INPUT(0x2))
    {
        pos = win->desc.x + 1;
        pos = (pos < 0x35 - win->desc.w) ? pos : 0x35 - win->desc.w;
        win->moveX += (win->desc.x - pos) * 12;
        win->desc.x = pos;
    }

    if (lbl_1_bss_6CAE8)
    {
        cursor = win->desc.cursor;
        do
        {
            if (--cursor < 0)
            {
                for (item = win->desc.items; (item->type & 0xFF) != DMI_END; item++)
                    cursor++;
            }
        } while (cursor != 0 && cursor != win->desc.cursor && (win->desc.items[cursor].type & 0xFF) > 12);
        win->desc.cursor = cursor;
    }
    else if (lbl_1_bss_6CAEC)
    {
        cursor = win->desc.cursor;
        do
        {
            cursor++;
            if ((win->desc.items[cursor].type & 0xFF) == DMI_END)
            {
                cursor = 0;
                break;
            }
        } while (cursor != win->desc.cursor && (win->desc.items[cursor].type & 0xFF) > 12);
        win->desc.cursor = cursor;
    }

    item = &win->desc.items[win->desc.cursor];
    switch (item->type)
    {
    case DMI_SUBMENU:
        if ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_A)
         && win->desc.items[win->desc.cursor].ptr != NULL)
        {
            desc = win->desc.items[win->desc.cursor].ptr;
            lbl_1_bss_6C7E0 = TRUE;
            lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.x = desc->x;
            lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.y = desc->y;
            lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.w = desc->w;
            lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.h = desc->h;
            lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.items = desc->items;
            lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.stride = desc->stride;
            lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.index = desc->index;
            lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.cursor = desc->cursor;
        }
        break;
    case DMI_WINDOW:
        if ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_A)
         && win->desc.items[win->desc.cursor].ptr != NULL)
            fn_1_3BA00(win->desc.items[win->desc.cursor].ptr);
        break;
    default:
        if (!(pad_status_groups[0].unk0[4].button & PAD_BUTTON_A)
         && !(pad_status_groups[0].unk0[4].button & PAD_BUTTON_B)
         && !((analog_inputs[0][0] & 0x200)
              && ((pad_status_groups[0].unk0[0].button & PAD_BUTTON_A)
                  || (pad_status_groups[0].unk0[0].button & PAD_BUTTON_B))))
            break;
        addr = (u8 *)item->ptr + win->desc.stride * ((item->type & 0x200) ? 0 : win->desc.index);
        mult = 1;
        switch (item->type & 0xFF)
        {
        case DMI_S32:
        case DMI_U32:
        case DMI_U16:
        case DMI_S16:
        case DMI_U8:
        case DMI_S8:
        case DMI_INDEX:
            switch (item->type & 0xFF)
            {
            case DMI_INDEX:
                val = win->desc.index;
                break;
            case DMI_S32:
                val = *(s32 *)addr;
                break;
            case DMI_U32:
                val = *(u32 *)addr;
                break;
            case DMI_U16:
                val = *(u16 *)addr;
                break;
            case DMI_S16:
                val = *(s16 *)addr;
                break;
            case DMI_U8:
                val = *(u8 *)addr;
                break;
            default:
                val = *(s8 *)addr;
                break;
            }
            flags = ((struct DebugMenuIntLimits *)item->limits)->flags;
            min = ((struct DebugMenuIntLimits *)item->limits)->min;
            max = ((struct DebugMenuIntLimits *)item->limits)->max;
            step = ((struct DebugMenuIntLimits *)item->limits)->step;
            step *= mult;
            if ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_A)
             || ((analog_inputs[0][0] & 0x200) && (pad_status_groups[0].unk0[0].button & PAD_BUTTON_A)))
            {
                if (flags & 2)
                    val += step;
                else if (val < val + step)
                    val = (val + step < max) ? val + step : max;
                else
                    val = ((u32)(val + step) < (u32)max) ? val + step : max;
            }
            else
            {
                if (flags & 1)
                    val -= step;
                else if (val > val - step)
                    val = (val - step > min) ? val - step : min;
                else
                    val = ((u32)(val - step) > (u32)min) ? val - step : min;
            }
            switch (item->type & 0xFF)
            {
            case DMI_S32:
            case DMI_U32:
                *(s32 *)addr = val;
                break;
            case DMI_INDEX:
                win->desc.index = val;
                break;
            case DMI_U16:
            case DMI_S16:
                *(s16 *)addr = val;
                break;
            case DMI_U8:
            case DMI_S8:
                *(s8 *)addr = val;
                break;
            }
            break;
        case DMI_FLOAT:
            fval = *(float *)addr;
            flags = ((struct DebugMenuFloatLimits *)item->limits)->flags;
            fmin = ((struct DebugMenuFloatLimits *)item->limits)->min;
            fmax = ((struct DebugMenuFloatLimits *)item->limits)->max;
            fstep = ((struct DebugMenuFloatLimits *)item->limits)->step;
            fstep *= mult;
            if ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_A)
             || ((analog_inputs[0][0] & 0x200) && (pad_status_groups[0].unk0[0].button & PAD_BUTTON_A)))
            {
                if (flags & 2)
                    fval += fstep;
                else
                    fval = (fval + fstep < fmax) ? fval + fstep : fmax;
            }
            else
            {
                if (flags & 1)
                    fval -= fstep;
                else
                    fval = (fval - fstep > fmin) ? fval - fstep : fmin;
            }
            /* round to the step's number of decimal places */
            scale = ((struct DebugMenuFloatLimits *)item->limits)->step;
            scaled = fval;
            for (digits = 0; scale < 1.0; digits++)
            {
                scale *= 10.0;
                scaled *= 10.0;
            }
            rounded = scaled;
            scaled = (int)scaled;
            if (fval >= 0.0)
            {
                if (rounded - scaled >= 0.5)
                    scaled += 1.0;
            }
            else if (rounded - scaled <= -0.5)
                scaled -= 1.0;
            while (digits > 0)
            {
                scaled *= 0.1;
                digits--;
            }
            fval = scaled;
            *(float *)addr = fval;
            break;
        case DMI_FLAG:
            *(u32 *)item->ptr ^= (u32)item->limits;
            break;
        case DMI_ITEMS:
            win->desc.items = item->ptr;
            break;
        }
        break;
    }
    win->moveX /= 2;
    win->moveY /= 2;
}

static inline void debug_set_clip_rect(int x, int y, int w, int h)
{
    g_debug_clip_x = x;
    g_debug_clip_y = y;
    g_debug_clip_w = w;
    g_debug_clip_h = h;
}

/* Draw a window and its items; the focused window highlights the cursor. */
void g_draw_ingame_debug_menu(struct DebugWindowDesc *desc, u32 focused)
{
    struct DebugMenuItem *item;
    int i;
    u8 *addr;
    int x;
    int y;
    char **names;

    debug_set_clip_rect(desc->x, desc->y, desc->w, desc->h);
    if (focused)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xFF);
    g_draw_debug_window_boundaries(desc);
    debug_textdraw_set_color(0xFF);
    fn_1_3E658(desc->x + 1, desc->y + 1, desc->w - 2, desc->h - 2);
    debug_set_clip_rect(desc->x + 1, desc->y + 1, desc->w - 2, desc->h - 2);

    x = 0;
    y = 0;
    for (item = desc->items, i = 0; item->type != DMI_END; item++, i++)
    {
        if (item->x > 0)
            x = desc->x + item->x;
        else
            x -= item->x;
        if (item->y > 0)
            y = desc->y + item->y;
        else
            y -= item->y;
        debug_textdraw_set_pos(x, y);
        debug_textdraw_set_color(0xFF);
        switch (item->type & 0xFF)
        {
        case 13:
        case 27:
            if (item->y == 1 && focused)
                debug_textdraw_set_color(0x33);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (focused)
                debug_textdraw_set_color(0x33);
            break;
        }
        addr = (u8 *)item->ptr + desc->stride * ((item->type & 0x200) ? 0 : desc->index);
        switch (item->type & 0xFF)
        {
        case 0:
        case 9:
        case 11:
        case 13:
            debug_window_printf(2, item->fmt);
            break;
        case 1:
        case 14:
            debug_window_printf(2, item->fmt, *(s32 *)addr);
            break;
        case 2:
        case 15:
            debug_window_printf(2, item->fmt, *(u32 *)addr);
            break;
        case 10:
            debug_window_printf(2, item->fmt, desc->index);
            break;
        case 3:
        case 16:
            debug_window_printf(2, item->fmt, *(u16 *)addr);
            break;
        case 4:
        case 17:
            debug_window_printf(2, item->fmt, *(s16 *)addr);
            break;
        case 5:
        case 18:
            debug_window_printf(2, item->fmt, *(u8 *)addr);
            break;
        case 6:
        case 19:
            debug_window_printf(2, item->fmt, *(s8 *)addr);
            break;
        case 7:
        case 20:
            debug_window_printf(2, item->fmt, *(float *)addr);
            break;
        case 21:
            debug_window_printf(2, item->fmt);
            break;
        case 22:
            debug_window_printf(2, item->fmt, *(u32 *)addr);
            break;
        case 8:
        case 23:
            debug_window_printf(2, item->fmt, (*(u32 *)addr & (u32)item->limits) ? " ON" : "OFF");
            break;
        case 24:
            debug_window_printf(2, item->fmt, (*(u16 *)addr & (u32)item->limits) ? " ON" : "OFF");
            break;
        case 27:
            debug_window_printf(2, item->fmt, desc->index);
            break;
        case 25:
            debug_window_printf(2, item->fmt, (names = item->limits)[*(u8 *)addr]);
            break;
        case 26:
            if (desc->cursor > 0)
                debug_window_printf(2, item->fmt, ((char **)item->ptr)[desc->cursor - 1]);
            break;
        case 30:
            debug_window_printf(2, "                 free       size\n");
            if (main_heap >= 0)
                debug_window_printf(2, "      MainHeap %08X / %08X\n", OSCheckHeap(main_heap), main_heap_size);
            if (stage_heap >= 0)
                debug_window_printf(2, "     StageHeap %08X / %08X\n", OSCheckHeap(stage_heap), stage_heap_size);
            if (bg_heap >= 0)
                debug_window_printf(2, "BackgroundHeap %08X / %08X\n", OSCheckHeap(bg_heap), bg_heap_size);
            if (chara_heap >= 0)
                debug_window_printf(2, "     CharaHeap %08X / %08X\n", OSCheckHeap(chara_heap), chara_heap_size);
            if (replay_heap >= 0)
                debug_window_printf(2, "    ReplayHeap %08X / %08X\n", OSCheckHeap(replay_heap), replay_heap_size);
            break;
        case 31:
            debug_window_printf(2, item->fmt, EVENT_STATUS_NAMES[((s8 *)item->ptr)[desc->index]]);
            break;
        case 32:
            debug_window_printf(2, item->fmt, EVENT_STATUS_NAMES[*(s8 *)item->ptr]);
            break;
        case 33:
            g_debug_print_ape(desc);
            break;
        case 34:
            fn_1_3FB0C(desc, lbl_1_bss_6C7DC);
            break;
        case 35:
            debug_overlay_draw_fifo(desc);
            break;
        }
        if (desc->cursor == i && focused)
        {
            debug_textdraw_set_pos(x - 1, y);
            debug_textdraw_set_color(0xC3);
            debug_textdraw_put(2, "\x1C");
        }
    }
}

void g_draw_debug_window_boundaries(struct DebugWindowDesc *desc)
{
    int x;
    int y;

    debug_set_clip_rect(desc->x, desc->y, desc->w, desc->h);
    debug_textdraw_set_pos(desc->x, desc->y);
    debug_textdraw_put(2, "\x18");
    debug_textdraw_set_pos(desc->x + desc->w - 1, desc->y);
    debug_textdraw_put(2, "\x19");
    debug_textdraw_set_pos(desc->x, desc->y + desc->h - 1);
    debug_textdraw_put(2, "\x1A");
    debug_textdraw_set_pos(desc->x + desc->w - 1, desc->y + desc->h - 1);
    debug_textdraw_put(2, "\x1B");
    for (x = 1; x < desc->w - 1; x++)
    {
        debug_textdraw_set_pos(desc->x + x, desc->y);
        debug_textdraw_put(2, "\x16");
    }
    for (y = 1; y < desc->h - 1; y++)
    {
        debug_textdraw_set_pos(desc->x, desc->y + y);
        debug_textdraw_put(2, "\x17");
    }
    for (x = 1; x < desc->w - 1; x++)
    {
        debug_textdraw_set_pos(desc->x + x, desc->y + desc->h - 1);
        debug_textdraw_put(2, "\x16");
    }
    for (y = 1; y < desc->h - 1; y++)
    {
        debug_textdraw_set_pos(desc->x + desc->w - 1, desc->y + y);
        debug_textdraw_put(2, "\x17");
    }
}

void draw_debugtext(void)
{
    GXColor color = {0, 0, 0, 0};
    int x0;
    struct DebugWindow **winp;
    BOOL wasSkip0;
    u8 *col0;
    u8 *chr0;
    int row;
    int y0;
    int col;
    int minX;
    int minY;
    int maxX;
    BOOL wasSkip1;
    int maxY;
    int x1;
    int offsetX;
    int y1;
    int offsetY;
    BOOL inCol;
    u8 *chr1;
    BOOL skip1;
    BOOL inRow;
    u8 *chr;
    u8 *colorp;
    int x;
    int y;
    BOOL skip0;
    int x2;
    int y2;
    u8 *col1;

    GXSetZMode_cached(1, 3, 0);
    GXSetTevColor(2, color);

    chr0 = g_debug_text_buffer_0;
    chr1 = g_debug_text_buffer_1;
    col0 = g_debug_text_color_buffer_0;
    col1 = g_debug_text_color_buffer_1;
    for (row = 0; row < 37; row++)
    {
        skip0 = FALSE;
        skip1 = FALSE;
        for (col = 0; col < 53; col++, chr0++, chr1++, col0++, col1++)
        {
            wasSkip0 = skip0;
            wasSkip1 = skip1;
            if (skip0)
                skip0 = FALSE;
            else if (*chr0 >= 0x80)
                skip0 = TRUE;
            if (skip1)
                skip1 = FALSE;
            else if (*chr1 >= 0x80)
                skip1 = TRUE;
            if (*chr1 != ' ')
            {
                if (wasSkip1)
                    continue;
                chr = chr1;
                colorp = col1;
            }
            else
            {
                if (*chr0 == ' ' || wasSkip0)
                    continue;
                chr = chr0;
                colorp = col0;
            }
            x = col * 12;
            y = row * 12;
            if (*chr < 0x80)
                draw_debugtext_char_en(x, y, *chr, *colorp);
            else if (g_is_sjis_and_more_than_24_mib())
                draw_debugtext_char_jp(x, y, chr, *colorp);
        }
    }

    GXSetZMode_cached(1, 1, 1);
    for (winp = lbl_1_bss_6C7E4, col = 0; col < 16; col++, winp++)
    {
        if (*winp == NULL)
            continue;
        x0 = (*winp)->desc.x * 12 + 6;
        y0 = (*winp)->desc.y * 12 + 6;
        x1 = ((*winp)->desc.x + (*winp)->desc.w) * 12 - 6;
        y1 = ((*winp)->desc.y + (*winp)->desc.h) * 12 - 6;
        if (col == lbl_1_bss_6C824 && lbl_1_bss_6C7E0)
        {
            x0 += (*winp)->moveX;
            y0 += (*winp)->moveY;
            x1 += (*winp)->moveX;
            y1 += (*winp)->moveY;
        }
        draw_debugtext_window_bg(x0, y0, x1, y1);
    }

    GXSetZMode_cached(1, 7, 0);
    chr0 = g_debug_text_buffer_2;
    col0 = g_debug_text_color_buffer_2;
    offsetX = lbl_1_bss_6C7B4;
    offsetY = lbl_1_bss_6C7B5;
    minX = g_debug_overlay_x0;
    minY = g_debug_overlay_y0;
    maxX = g_debug_overlay_x1;
    maxY = g_debug_overlay_y1;
    for (row = 0; row < 37; row++)
    {
        skip0 = FALSE;
        inRow = (minY <= row && row < maxY);
        for (col = 0; col < 53; col++, chr0++, col0++)
        {
            wasSkip0 = skip0;
            if (skip0)
                skip0 = FALSE;
            else if (*chr0 >= 0x80)
                skip0 = TRUE;
            if (*chr0 == ' ' || *chr0 == 0 || wasSkip0)
                continue;
            x2 = col * 12;
            y2 = row * 12;
            inCol = (minX <= col && col < maxX);
            if (inCol && inRow)
            {
                x2 += offsetX;
                y2 += offsetY;
            }
            if (*chr0 < 0x80)
                draw_debugtext_char_en(x2, y2, *chr0, *col0);
            else if (g_is_sjis_and_more_than_24_mib())
                draw_debugtext_char_jp(x2, y2, chr0, *col0);
        }
    }

    GXSetZMode_cached(1, 3, 1);
    memset(g_debug_text_buffer_1, ' ', sizeof(g_debug_text_buffer_1));
    memset(g_debug_text_color_buffer_1, 0xFF, sizeof(g_debug_text_color_buffer_1));
    memset(g_debug_text_buffer_2, 0, sizeof(g_debug_text_buffer_2));
    memset(g_debug_text_color_buffer_2, 0xFF, sizeof(g_debug_text_color_buffer_2));
}

void window_init(void)
{
    int i;

    lbl_1_bss_6C824 = 0;
    lbl_1_bss_6C7E0 = FALSE;
    for (i = 0; i < 16; i++)
        lbl_1_bss_6C828[i].active = FALSE;
    memset(g_debug_text_buffer_1, ' ', sizeof(g_debug_text_buffer_1));
    memset(g_debug_text_buffer_0, ' ', sizeof(g_debug_text_buffer_0));
    memset(g_debug_text_buffer_2, ' ', sizeof(g_debug_text_buffer_2));
    memset(g_debug_text_color_buffer_1, 0xFF, sizeof(g_debug_text_color_buffer_1));
    memset(g_debug_text_color_buffer_0, 0xFF, sizeof(g_debug_text_color_buffer_0));
    memset(g_debug_text_color_buffer_2, 0xFF, sizeof(g_debug_text_color_buffer_2));
    os_font_encoding = OSGetFontEncode();
}

/* Debug menu: window management input, edit the selected item, draw the windows. */
void g_something_with_replays3(void)
{
    int n;
    struct DebugWindow *win;
    int k;
    BOOL canEdit;
    struct DebugWindow *front;
    s32 soundId;
    s32 soundGroup;
    struct DebugWindow **p;
    int j;
    struct DebugWindow **q;
    int i;
    int track;
    struct DebugWindow **r;

    canEdit = TRUE;
    lbl_1_bss_6CAE8 = (pad_status_groups[0].unk0[4].button & PAD_BUTTON_UP)
                   || ((pad_status_groups[0].unk0[0].button & PAD_BUTTON_UP) && (analog_inputs[0][0] & 0x200));
    lbl_1_bss_6CAEC = (pad_status_groups[0].unk0[4].button & PAD_BUTTON_DOWN)
                   || ((pad_status_groups[0].unk0[0].button & PAD_BUTTON_DOWN) && (analog_inputs[0][0] & 0x200));
    lbl_1_bss_6F900 = (pad_status_groups[0].unk0[4].button & PAD_BUTTON_LEFT)
                   || ((pad_status_groups[0].unk0[0].button & PAD_BUTTON_LEFT) && (analog_inputs[0][0] & 0x200));
    lbl_1_bss_6F904 = (pad_status_groups[0].unk0[4].button & PAD_BUTTON_RIGHT)
                   || ((pad_status_groups[0].unk0[0].button & PAD_BUTTON_RIGHT) && (analog_inputs[0][0] & 0x200));

    if (analog_inputs[0][0] & 0x100)
    {
        if ((dip_switches & DIP_DEBUG) && (pad_status_groups[0].unk0[2].button & 0x1000))
        {
            fn_1_3BA00(&lbl_1_data_2D258);
        }
        else if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_Y)
        {
            /* close the focused window */
            if (lbl_1_bss_6C7E0 && lbl_1_bss_6C7E4[lbl_1_bss_6C824] != NULL)
            {
                lbl_1_bss_6C7E0 = TRUE;
                lbl_1_bss_6C7E4[lbl_1_bss_6C824]->active = FALSE;
                for (n = lbl_1_bss_6C824, q = &lbl_1_bss_6C7E4[n]; n < 15; n++, q++)
                    q[0] = q[1];
                *q = NULL;
                for (j = 0; j < 16; j++)
                {
                    k = (lbl_1_bss_6C824 - j + 16) % 16;
                    if (lbl_1_bss_6C7E4[k] != NULL)
                    {
                        lbl_1_bss_6C824 = k;
                        lbl_1_bss_6C7E0 = TRUE;
                        break;
                    }
                }
                if (j >= 16)
                {
                    lbl_1_bss_6C824 = 0;
                    lbl_1_bss_6C7E0 = FALSE;
                }
            }
        }
        else if (pad_status_groups[0].unk0[4].button & PAD_BUTTON_A)
        {
            /* focus the previous window */
            if (!lbl_1_bss_6C7E0)
            {
                for (n = 15, q = &lbl_1_bss_6C7E4[n]; n >= 0; n--, q--)
                {
                    if (*q != NULL)
                        break;
                }
                if (n >= 0)
                {
                    lbl_1_bss_6C824 = n;
                    lbl_1_bss_6C7E0 = TRUE;
                }
            }
            else
            {
                lbl_1_bss_6C824--;
                if (lbl_1_bss_6C824 < 0 || lbl_1_bss_6C7E4[lbl_1_bss_6C824] == NULL)
                {
                    lbl_1_bss_6C824 = 0;
                    lbl_1_bss_6C7E0 = FALSE;
                }
            }
            canEdit = FALSE;
        }
        else if (pad_status_groups[0].unk0[4].button & PAD_BUTTON_B)
        {
            /* cycle the window order */
            if (lbl_1_bss_6C7E0)
            {
                if (lbl_1_bss_6C824 == 0)
                {
                    front = lbl_1_bss_6C7E4[0];
                    for (n = 0, r = lbl_1_bss_6C7E4; n < 15 && r[1] != NULL; n++, r++)
                        r[0] = r[1];
                    *r = front;
                    lbl_1_bss_6C824 = n;
                }
                else
                {
                    front = lbl_1_bss_6C7E4[lbl_1_bss_6C824];
                    for (n = lbl_1_bss_6C824, r = &lbl_1_bss_6C7E4[n]; n > 0; n--, r--)
                        r[0] = r[-1];
                    lbl_1_bss_6C7E4[0] = front;
                    lbl_1_bss_6C824 = 0;
                }
            }
        }
    }

    /* copy editable values in */
    lbl_1_bss_6F7C = u_lightToPrint;
    lbl_1_bss_6F30 = s_lightPool[u_lightToPrint];
    lbl_1_bss_6F908 = lbl_1_bss_6F30.spotFn;
    lbl_1_bss_6F2C = lbl_1_bss_6F2D;
    lbl_1_bss_6F24[0] = s_lightGroups[lbl_1_bss_6F2D].lightPoolIdxs[0];
    lbl_1_bss_6F24[1] = s_lightGroups[lbl_1_bss_6F2D].lightPoolIdxs[1];
    lbl_1_bss_6F24[2] = s_lightGroups[lbl_1_bss_6F2D].lightPoolIdxs[2];
    lbl_1_bss_6F24[3] = s_lightGroups[lbl_1_bss_6F2D].lightPoolIdxs[3];
    lbl_1_bss_6F24[4] = s_lightGroups[lbl_1_bss_6F2D].lightPoolIdxs[4];
    lbl_1_bss_6F24[5] = s_lightGroups[lbl_1_bss_6F2D].lightPoolIdxs[5];
    lbl_1_bss_6F24[6] = s_lightGroups[lbl_1_bss_6F2D].lightPoolIdxs[6];
    lbl_1_bss_6F24[7] = s_lightGroups[lbl_1_bss_6F2D].lightPoolIdxs[7];
    lbl_1_bss_6F18 = s_lightGroups[lbl_1_bss_6F2D].ambient;
    lbl_1_bss_6F909 = lbl_1_bss_24980[lbl_1_bss_217F0];
    soundId = lbl_1_bss_217F0;
    soundGroup = lbl_1_bss_217EC;
    lbl_1_bss_6F90A = lbl_1_bss_25918[lbl_1_bss_217FD];
    track = lbl_1_bss_217FD;

    if (canEdit && lbl_1_bss_6C7E0 && lbl_1_bss_6C7E4[lbl_1_bss_6C824] != NULL)
        fn_1_3BAF8(lbl_1_bss_6C7E4[lbl_1_bss_6C824]);

    /* copy edited values back */
    lbl_1_bss_6F30.spotFn = lbl_1_bss_6F908;
    s_lightPool[lbl_1_bss_6F7C] = lbl_1_bss_6F30;
    s_lightGroups[lbl_1_bss_6F2C].lightPoolIdxs[0] = lbl_1_bss_6F24[0];
    s_lightGroups[lbl_1_bss_6F2C].lightPoolIdxs[1] = lbl_1_bss_6F24[1];
    s_lightGroups[lbl_1_bss_6F2C].lightPoolIdxs[2] = lbl_1_bss_6F24[2];
    s_lightGroups[lbl_1_bss_6F2C].lightPoolIdxs[3] = lbl_1_bss_6F24[3];
    s_lightGroups[lbl_1_bss_6F2C].lightPoolIdxs[4] = lbl_1_bss_6F24[4];
    s_lightGroups[lbl_1_bss_6F2C].lightPoolIdxs[5] = lbl_1_bss_6F24[5];
    s_lightGroups[lbl_1_bss_6F2C].lightPoolIdxs[6] = lbl_1_bss_6F24[6];
    s_lightGroups[lbl_1_bss_6F2C].lightPoolIdxs[7] = lbl_1_bss_6F24[7];
    s_lightGroups[lbl_1_bss_6F2C].ambient = lbl_1_bss_6F18;
    lbl_1_bss_24980[soundId] = lbl_1_bss_6F909;
    if (lbl_1_rodata_148C[lbl_1_bss_217F0].flags & 0x2000)
    {
        if (lbl_1_bss_217F0 == lbl_1_rodata_1284[lbl_1_bss_217EC].firstSoundId)
            lbl_1_bss_217F0 = lbl_1_rodata_1284[lbl_1_bss_217EC + 1].firstSoundId - 1;
        else
            lbl_1_bss_217F0 = lbl_1_rodata_1284[lbl_1_bss_217EC].firstSoundId + 1;
    }
    if (soundGroup != lbl_1_bss_217EC)
        lbl_1_bss_217F0 = lbl_1_rodata_1284[lbl_1_bss_217EC].firstSoundId + 1;
    if (lbl_1_bss_6F90C)
    {
        SoundReqDirect(lbl_1_bss_217F0);
        lbl_1_bss_6F90C = FALSE;
    }
    if (lbl_1_bss_6F910)
    {
        SoundOff(lbl_1_bss_217F0);
        lbl_1_bss_6F910 = FALSE;
    }
    lbl_1_bss_25918[track] = lbl_1_bss_6F90A;
    if (lbl_1_bss_6F914)
    {
        fn_1_32338((s8)lbl_1_bss_6F918, (s8)lbl_1_bss_6F919);
        lbl_1_bss_6F914 = FALSE;
    }
    lbl_1_bss_6F91C = lbl_1_bss_259AC ? "ENABLE" : "DISABLE";

    for (i = 15, p = &lbl_1_bss_6C7E4[i]; i >= 0; i--, p--)
    {
        if (*p != NULL)
        {
            if (i == lbl_1_bss_6C824 && lbl_1_bss_6C7E0)
                g_draw_ingame_debug_menu(&(*p)->desc, TRUE);
            else
                g_draw_ingame_debug_menu(&(*p)->desc, FALSE);
        }
    }
    if (lbl_1_bss_6C7E0)
    {
        win = lbl_1_bss_6C7E4[lbl_1_bss_6C824];
        fn_1_3E13C(win->moveX, win->moveY, win->desc.x, win->desc.y, win->desc.w, win->desc.h);
    }
    else
    {
        fn_1_3E13C(0, 0, 0, 0, 0, 0);
    }
    debug_textdraw_set_color(0xFF);
    if (lbl_1_bss_6C7D8)
    {
        fn_1_3FC40(lbl_1_bss_6C7DC, lbl_1_data_2CDD4);
        printf("[ RecPlaySave ]: %s\n", lbl_1_data_2CDD4);
        lbl_1_bss_6C7D8 = 0;
    }
    if (lbl_1_bss_6C7E0 && lbl_1_bss_6C7E4[lbl_1_bss_6C824] != NULL)
    {
        lbl_1_bss_6C7C8 = lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.x;
        lbl_1_bss_6C7CC = lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.y;
        lbl_1_bss_6C7D0 = lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.w;
        lbl_1_bss_6C7D4 = lbl_1_bss_6C7E4[lbl_1_bss_6C824]->desc.h;
    }
    else
    {
        lbl_1_bss_6C7C8 = 0;
        lbl_1_bss_6C7CC = 0;
        lbl_1_bss_6C7D0 = 0;
        lbl_1_bss_6C7D4 = 0;
    }
}

void debug_textdraw_set_pos(int x, int y)
{
    g_debug_textdraw_pos_x_2 = g_debug_textdraw_pos_x_1 = x;
    g_debug_textdraw_pos_y = y;
}

void fn_1_3E100(int dx, int dy)
{
    g_debug_textdraw_pos_x_1 += dx;
    g_debug_textdraw_pos_y += dy;
}

/* Set the overlay's shifted region (the focused window, while it slides). */
void fn_1_3E13C(int moveX, int moveY, int x, int y, int w, int h)
{
    lbl_1_bss_6C7B4 = moveX;
    lbl_1_bss_6C7B5 = moveY;
    g_debug_overlay_x0 = x;
    g_debug_overlay_y0 = y;
    g_debug_overlay_x1 = x + w;
    g_debug_overlay_y1 = y + h;
}

int debug_window_printf(int buf, char *fmt, ...)
{
    va_list args;
    char str[0x200];

    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
    debug_textdraw_put(buf, str);
    return 0;
}

void debug_textdraw_set_color(u8 color)
{
    g_debug_textdraw_color = color;
}

/* Whether column 'col' of a text line is the second byte of a Shift-JIS character. */
static inline BOOL is_sjis_trail_byte(u8 *line, int col)
{
    BOOL trail;
    int i;

    if (col >= 53)
        return FALSE;
    trail = FALSE;
    for (i = col; i > 0; i--, line++)
    {
        if (trail)
            trail = FALSE;
        else if (*line >= 0x80)
            trail = TRUE;
    }
    return trail;
}

void debug_textdraw_put(int bufIdx, char *str)
{
    u8 *chars;
    u8 *colors;
    u8 color;
    int minX;
    int minY;
    int maxX;
    int maxY;
    int startX;
    int x;
    int y;

    startX = g_debug_textdraw_pos_x_2;
    x = g_debug_textdraw_pos_x_1;
    y = g_debug_textdraw_pos_y;
    color = g_debug_textdraw_color;
    chars = g_debug_text_buffers[bufIdx];
    colors = g_debug_color_bufers[bufIdx];
    if (bufIdx == 2)
    {
        minX = g_debug_clip_x;
        minY = g_debug_clip_y;
        maxX = g_debug_clip_x + g_debug_clip_w;
        maxY = g_debug_clip_y + g_debug_clip_h;
        if (minX < 0)
            minX = 0;
        if (minY < 0)
            minY = 0;
        if (maxX > 53)
            maxX = 53;
        if (maxY > 37)
            maxY = 37;
    }
    else
    {
        minX = 0;
        minY = 0;
        maxX = 53;
        maxY = 37;
    }
    while (*(u8 *)str != 0)
    {
        if (*(u8 *)str == '\n')
        {
            y++;
            x = startX;
            chars = g_debug_text_buffers[bufIdx];
            colors = g_debug_color_bufers[bufIdx];
            str++;
        }
        else if (*(u8 *)str < 0x80)
        {
            if (minX <= x && x < maxX && minY <= y && y < maxY)
            {
                if (is_sjis_trail_byte(chars + y * 53, x + 1))
                    (chars + y * 53)[x + 1] = ' ';
                (chars + y * 53)[x] = *(u8 *)str;
                (colors + y * 53)[x] = color;
                if (is_sjis_trail_byte(chars + y * 53, x))
                    (chars + y * 53)[x - 1] = ' ';
            }
            x++;
            str++;
        }
        else
        {
            if (g_is_sjis_and_more_than_24_mib()
             && minX <= x && x + 1 < maxX && minY <= y && y + 1 < maxY)
            {
                if (is_sjis_trail_byte(chars + y * 53, x + 2))
                    (chars + y * 53)[x + 2] = ' ';
                (chars + y * 53)[x] = ((u8 *)str)[0];
                (colors + y * 53)[x] = color;
                (chars + y * 53)[x + 1] = ((u8 *)str)[1];
                (colors + y * 53)[x + 1] = color;
                if (is_sjis_trail_byte(chars + y * 53, x))
                    (chars + y * 53)[x - 1] = ' ';
            }
            x += 2;
            str += 2;
        }
    }
    g_debug_textdraw_pos_x_2 = startX;
    g_debug_textdraw_pos_x_1 = x;
    g_debug_textdraw_pos_y = y;
}

u8 *g_debug_text_buffers[] = { g_debug_text_buffer_0, g_debug_text_buffer_1, g_debug_text_buffer_2 };
u8 *g_debug_color_bufers[] = { g_debug_text_color_buffer_0, g_debug_text_color_buffer_1, g_debug_text_color_buffer_2 };

void g_set_up_debugtext_buffer(void)
{
    memset(g_debug_text_buffer_0, ' ', sizeof(g_debug_text_buffer_0));
    memset(g_debug_text_color_buffer_0, 0xFF, sizeof(g_debug_text_color_buffer_0));
}

/* Clear a rectangle of the overlay layer. */
void fn_1_3E658(int x, int y, int w, int h)
{
    int i;
    int j;
    u8 *chr;
    u8 *col;

    for (j = y; j < y + h; j++)
    {
        chr = g_debug_text_buffer_2 + j * 53 + x;
        col = g_debug_text_color_buffer_2 + j * 53 + x;
        for (i = x; i < x + w; i++)
        {
            *chr++ = ' ';
            *col++ = 0xFF;
        }
    }
}

int debug_textdraw_vsprintf_buf_0(char *fmt, ...)
{
    va_list args;
    char str[0x200];

    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
    debug_textdraw_put(0, str);
    return 0;
}

int debug_textdraw_vsprintf_buf_1(char *fmt, ...)
{
    va_list args;
    char str[0x200];

    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
    debug_textdraw_put(1, str);
    return 0;
}

void debug_textdraw_put_buf_1(char *str)
{
    debug_textdraw_put(1, str);
}

int g_some_printf_function_2(int unused, char *fmt, ...)
{
    va_list args;
    char str[0x200];

    if (!(g_some_other_flags & 4))
        return 0;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
    g_very_similar_to_puts(str);
    return 0;
}

/* Draw an ASCII character from the 16x8 debug font (texture 31 of the common TPL). */
void draw_debugtext_char_en(int x, int y, u8 chr, u8 color)
{
    float u0;
    float v0;
    float u1;
    float v1;
    float x0;
    float y0;
    float x1;
    float y1;
    float z;
    GXColor tevColor;

    GXLoadTexObj_cached(&bmp_com_tpl->texObjs[31], 0);
    u0 = 12.0f * (chr & 0xF) / 192.0;
    v0 = 12.0f * ((chr & 0xF0) >> 4) / 96.0;
    u1 = u0 + 0.0625;
    v1 = v0 + 0.125;
    x0 = x;
    y0 = 480.0 * y / 448.0;
    x1 = x0 + 12.0f;
    y1 = y0 + 12.857142857142858;
    z = -0.0078125f;
    tevColor.r = (color & 0xC0) | 0x3F;
    tevColor.g = ((color & 0x30) << 2) | 0x3F;
    tevColor.b = ((color & 0x0C) << 4) | 0x3F;
    tevColor.a = ((color & 0x03) << 6) | 0x3F;
    GXSetTevColor(1, tevColor);
    GXBegin(0x80, 7, 4);
        GXPosition3f32(x0, y0, z);
        GXTexCoord2f32(u0, v0);
        GXPosition3f32(x1, y0, z);
        GXTexCoord2f32(u1, v0);
        GXPosition3f32(x1, y1, z);
        GXTexCoord2f32(u1, v1);
        GXPosition3f32(x0, y1, z);
        GXTexCoord2f32(u0, v1);
    GXEnd();
}

/* Draw a Shift-JIS character from the ROM font, with a black outline. */
void draw_debugtext_char_jp(int x, int y, u8 *chr, u8 color)
{
    float u0;
    float v0;
    float u1;
    float v1;
    float x0;
    float y0;
    float x1;
    float y1;
    float z;
    GXColor tevColor = {0, 0, 0, 0xFF};
    void *image;
    s32 s;
    s32 t;
    s32 w;
    GXTexObj texObj;

    fn_8000F954(chr, &image, &s, &t, &w);
    GXInitTexObj(&texObj, image, lbl_1_bss_D2B7C->sheetWidth, lbl_1_bss_D2B7C->sheetHeight,
                 lbl_1_bss_D2B7C->sheetFormat, 0, 0, 0);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj_cached(&texObj, 0);
    u0 = (float)s / lbl_1_bss_D2B7C->sheetWidth;
    v0 = (float)t / lbl_1_bss_D2B7C->sheetHeight;
    u1 = u0 + (float)w / lbl_1_bss_D2B7C->sheetWidth;
    v1 = v0 + 24.0f / lbl_1_bss_D2B7C->sheetHeight;
    x0 = x + 12 - w / 2;
    y0 = 480.0 * y / 448.0;
    x1 = x + 12 + w / 2;
    y1 = y0 + 25.714285714285715;
    z = -0.0078125f;

    GXSetTevColor(1, tevColor);
    GXBegin(0x80, 7, 4);
        GXPosition3f32(x0 - 1.0f, y0 - 1.0f, z);
        GXTexCoord2f32(u0, v0);
        GXPosition3f32(x1 - 1.0f, y0 - 1.0f, z);
        GXTexCoord2f32(u1, v0);
        GXPosition3f32(x1 - 1.0f, y1 - 1.0f, z);
        GXTexCoord2f32(u1, v1);
        GXPosition3f32(x0 - 1.0f, y1 - 1.0f, z);
        GXTexCoord2f32(u0, v1);
    GXEnd();
    GXBegin(0x80, 7, 4);
        GXPosition3f32(x0 - 1.0f, y0 + 1.0f, z);
        GXTexCoord2f32(u0, v0);
        GXPosition3f32(x1 - 1.0f, y0 + 1.0f, z);
        GXTexCoord2f32(u1, v0);
        GXPosition3f32(x1 - 1.0f, y1 + 1.0f, z);
        GXTexCoord2f32(u1, v1);
        GXPosition3f32(x0 - 1.0f, y1 + 1.0f, z);
        GXTexCoord2f32(u0, v1);
    GXEnd();
    GXBegin(0x80, 7, 4);
        GXPosition3f32(x0 + 1.0f, y0 - 1.0f, z);
        GXTexCoord2f32(u0, v0);
        GXPosition3f32(x1 + 1.0f, y0 - 1.0f, z);
        GXTexCoord2f32(u1, v0);
        GXPosition3f32(x1 + 1.0f, y1 - 1.0f, z);
        GXTexCoord2f32(u1, v1);
        GXPosition3f32(x0 + 1.0f, y1 - 1.0f, z);
        GXTexCoord2f32(u0, v1);
    GXEnd();
    GXBegin(0x80, 7, 4);
        GXPosition3f32(x0 + 1.0f, y0 + 1.0f, z);
        GXTexCoord2f32(u0, v0);
        GXPosition3f32(x1 + 1.0f, y0 + 1.0f, z);
        GXTexCoord2f32(u1, v0);
        GXPosition3f32(x1 + 1.0f, y1 + 1.0f, z);
        GXTexCoord2f32(u1, v1);
        GXPosition3f32(x0 + 1.0f, y1 + 1.0f, z);
        GXTexCoord2f32(u0, v1);
    GXEnd();

    tevColor.r = (color & 0xC0) | 0x3F;
    tevColor.g = ((color & 0x30) << 2) | 0x3F;
    tevColor.b = ((color & 0x0C) << 4) | 0x3F;
    tevColor.a = ((color & 0x03) << 6) | 0x3F;
    GXSetTevColor(1, tevColor);
    GXBegin(0x80, 7, 4);
        GXPosition3f32(x0, y0, z);
        GXTexCoord2f32(u0, v0);
        GXPosition3f32(x1, y0, z);
        GXTexCoord2f32(u1, v0);
        GXPosition3f32(x1, y1, z);
        GXTexCoord2f32(u1, v1);
        GXPosition3f32(x0, y1, z);
        GXTexCoord2f32(u0, v1);
    GXEnd();
}

/* Draw a window's translucent black background (screen coordinates). */
void draw_debugtext_window_bg(float x0, float y0, float x1, float y1)
{
    float z;
    GXColor tevColor = {0, 0, 0, 0x80};

    GXLoadTexObj_cached(&bmp_com_tpl->texObjs[75], 0);
    y0 *= 1.0714285714285714;
    y1 *= 1.0714285714285714;
    z = -0.0078125f;
    GXSetTevColor(1, tevColor);
    GXBegin(0x80, 7, 4);
        GXPosition3f32(x0, y0, z);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x1, y0, z);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x1, y1, z);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(x0, y1, z);
        GXTexCoord2f32(0.0f, 1.0f);
    GXEnd();
}

BOOL g_is_sjis_and_more_than_24_mib(void)
{
    if (os_font_encoding != 1)
        return FALSE;
    if ((dip_switches & DIP_ONLY24MB) || !is_more_than_24_mib)
        return FALSE;
    return TRUE;
}

/* Index of the open window whose first item is labelled 'name', or -1. */
int fn_1_3F61C(char *name)
{
    s16 i;
    struct DebugWindow *win;

    for (i = 0; i < 16; i++)
    {
        win = &lbl_1_bss_6C828[i];
        if (win->active)
        {
            struct DebugMenuItem *items = win->desc.items;
            if (strcmp(name, items->fmt) == 0)
                return win->desc.index;
        }
    }
    return -1;
}

/* Character (ape) info window. */
void g_debug_print_ape(struct DebugWindowDesc *desc)
{
    struct Ape *ape;
    struct ApeFlagInfo *flagInfo;

    ape = get_ape_by_idx(desc->index);
    if (ape == NULL)
        return;
    fn_1_1157E4(ape->unk0, ape->unk78);
    flagInfo = g_get_ape_flag(ape->unk0);
    debug_window_printf(2, "%s(%s)\n", get_ape_chara_name(ape), get_ape_lod_string(ape));
    debug_window_printf(2, " %s\n", g_get_ape_skinning_or_stitching_name(ape));
    debug_window_printf(2, " posx %8.3f\n", ape->pos.x);
    debug_window_printf(2, " posy %8.3f\n", ape->pos.y);
    debug_window_printf(2, " posz %8.3f\n", ape->pos.z);
    debug_window_printf(2, " flag %08X\n", ape->flags);
    debug_window_printf(2, " face %s\n", get_ape_face_expr_name(ape));
    debug_window_printf(2, " handl %4hX : %4.2f\n", ape->lhandId, ape->lhandBlend);
    debug_window_printf(2, " handr %4hX : %4.2f\n", ape->rhandId, ape->rhandBlend);
    debug_window_printf(2, "\n");
    debug_window_printf(2, "ROB_CONTROL:\n");
    debug_window_printf(2, " %s\n", lbl_1_data_6ECB4[ape->unk86]);
    debug_window_printf(2, " PARAM: %04hX STEP: %d\n", ape->robParam, ape->robStep);
    debug_window_printf(2, "\n");
    debug_window_printf(2, "MOTION:\n");
    debug_window_printf(2, " BODY [%5.1f/%5.1f]\n", ((struct Skeleton *)ape->bodyAnim)->clock.frame, ((struct Skeleton *)ape->bodyAnim)->clock.length);
    debug_window_printf(2, " LHAND[%5.1f/%5.1f]\n", ((struct Skeleton *)ape->lhandAnim)->clock.frame, ((struct Skeleton *)ape->lhandAnim)->clock.length);
    debug_window_printf(2, " RHAND[%5.1f/%5.1f]\n", ((struct Skeleton *)ape->rhandAnim)->clock.frame, ((struct Skeleton *)ape->rhandAnim)->clock.length);
    debug_window_printf(2, " FACE [%5.1f/%5.1f]\n", ((struct Skeleton *)ape->faceAnim)->clock.frame, ((struct Skeleton *)ape->faceAnim)->clock.length);
    debug_window_printf(2, " speed: %5.1f\n", flagInfo->speed);
    debug_window_printf(2, " float: %5.1f\n", flagInfo->unk14);
    debug_window_printf(2, " game    : %s\n", get_ape_scene_name(ape));
    debug_window_printf(2, " category: %s\n", fn_1_1137D8(ape));
    debug_window_printf(2, " status  : %s\n", get_ape_anim_flags(ape));
    debug_window_printf(2, "\n");
    debug_window_printf(2, "FLAG:\n");
    if (flagInfo->flags & 1)
        debug_window_printf(2, " RateIntp  ");
    if (flagInfo->flags & 2)
        debug_window_printf(2, " CalcIntp  ");
    if ((flagInfo->flags & 1) || (flagInfo->flags & 2))
        debug_window_printf(2, "\n");
    if (flagInfo->flags & 4)
        debug_window_printf(2, " Face Dir  ");
    if (flagInfo->flags & 0x10)
        debug_window_printf(2, " No loop   ");
    if ((flagInfo->flags & 4) || (flagInfo->flags & 0x10))
        debug_window_printf(2, "\n");
    if (flagInfo->flags & 8)
        debug_window_printf(2, " Protect  ");
    if (flagInfo->flags & 8)
        debug_window_printf(2, "          ");
    if ((flagInfo->flags & 8) || (flagInfo->flags & 8))
        debug_window_printf(2, "\n");
    debug_window_printf(2, "\n");
}

/* Replay info window. Replay 0x11 is the one kept for the memory card. */
void fn_1_3FB0C(struct DebugWindowDesc *desc, int replayId)
{
    struct ReplayHeader header;

    if (replayId == 0x11)
    {
        debug_textdraw_set_color(0xF3);
        debug_window_printf(2, "[ Data For Memcard Save ]\n");
        debug_textdraw_set_color(0xFF);
    }
    fn_1_62AA0(replayId, &header);
    if (header.flags & 0x200)
    {
        debug_textdraw_set_color(0xC3);
        debug_window_printf(2, "       :NO DATA\n");
        debug_textdraw_set_color(0xFF);
        return;
    }
    if (!(header.flags & 0x1000))
        debug_window_printf(2, "  STAGE:%d-%03d(Lib No.%03d)\n", header.difficulty, header.floor, header.stageId);
    else
        debug_window_printf(2, "  WORLD:%d-%d(Lib No.%03d)\n", header.difficulty, header.floor, header.stageId);
    debug_window_printf(2, "  MONKY: %3d\n", header.monkey);
    debug_window_printf(2, "   NAME: %s\n", header.name);
    debug_window_printf(2, "  GRADE: %f\n", header.grade);
}

/* Save a replay to the host file system (replay/<stage><n>.<flags>). */
void fn_1_3FC40(int replayId, char *msg)
{
    struct ReplayHeader header;
    char path[0x40];
    void *replay;
    u32 size;
    void *buf;
    void *fp;

    fn_1_62AA0(replayId, &header);
    if (header.flags & 0x200)
    {
        sprintf(msg, "Empty data!");
        return;
    }
    if (replayId == 0x11)
    {
        fn_1_3FDF0(msg);
        return;
    }
    replay = fn_1_6333C(replayId);
    size = fn_1_63354(replay);
    buf = alloc_from_heap_or_panic(currentHeap, size, "window.c", 3260);
    fn_1_6335C(replay, buf);
    replayId = 0;
    sprintf(path, "replay/%03X%1X.%04X", header.stageId & 0xFFF, replayId, header.flags & 0xFFFF);
    while ((replay = fn_8006F330(path, "rb")) != NULL)
    {
        fclose(replay);
        sprintf(path, "replay/%03X%1X.%04X", header.stageId & 0xFFF, replayId, header.flags & 0xFFFF);
        replayId++;
    }
    fp = fn_8006F330(path, "wb");
    if (fp == NULL)
    {
        sprintf(msg, "ERROR: Can't open %s!", path);
        return;
    }
    fwrite(buf, 1, size, fp);
    sprintf(msg, "Saved in %s", path);
    fclose(fp);
    OSFreeToHeap(currentHeap, buf);
}

/* Save the memory card replay (0x11), compressed. */
void fn_1_3FDF0(char *msg)
{
    struct ReplayHeader header;
    char path[0x40];
    u32 size;
    void *buf;
    void *fp;
    int i;

    size = get_compressed_replay_size();
    buf = alloc_from_heap_or_panic(currentHeap, size, "window.c", 3318);
    compress_replay(buf);
    fn_1_62AA0(0x11, &header);
    if (header.flags & 0x200)
    {
        sprintf(msg, "Empty data!");
        return;
    }
    i = 0;
    sprintf(path, "mc_replay/%03X%1X.%04X", header.stageId & 0xFFF, i, header.flags & 0xFFFF);
    while ((fp = fn_8006F330(path, "rb")) != NULL)
    {
        fclose(fp);
        sprintf(path, "replay/%03X%1X.%04X", header.stageId & 0xFFF, i, header.flags & 0xFFFF);
        i++;
    }
    fp = fn_8006F330(path, "wb");
    if (fp == NULL)
    {
        sprintf(msg, "ERROR: Can't open %s!", path);
        return;
    }
    fwrite(buf, 1, size, fp);
    sprintf(msg, "Saved in %s", path);
    fclose(fp);
    OSFreeToHeap(currentHeap, buf);
}

/* GX FIFO usage window. */
void debug_overlay_draw_fifo(struct DebugWindowDesc *desc)
{
    u32 size;
    float ratio;

    debug_window_printf(2, "    MAX SIZE: 0x%08X\n", 0x100000);

    size = get_gx_fifo_use_size();
    ratio = (float)size / 1048576.0f;
    debug_window_printf(2, "    USE SIZE: 0x%08X (", size);
    if (ratio < 0.7f)
        debug_textdraw_set_color(0x33);
    else if (ratio < 0.85f)
        debug_textdraw_set_color(0xF3);
    else
        debug_textdraw_set_color(0xC3);
    debug_window_printf(2, "%5.1f%%", 100.0f * ratio);
    debug_textdraw_set_color(0xFF);
    debug_window_printf(2, ")\n\n", size);

    size = get_max_gx_fifo_use_size();
    ratio = (float)size / 1048576.0f;
    debug_window_printf(2, "RECENTRY MAX: 0x%08X (", size);
    if (ratio < 0.7f)
        debug_textdraw_set_color(0x33);
    else if (ratio < 0.85f)
        debug_textdraw_set_color(0xF3);
    else
        debug_textdraw_set_color(0xC3);
    debug_window_printf(2, "%5.1f%%", 100.0f * ratio);
    debug_textdraw_set_color(0xFF);
    debug_window_printf(2, ")\n\n", size);
}

