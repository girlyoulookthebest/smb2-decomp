#ifndef BITMAP_H
#define BITMAP_H

#include "types.h"

typedef struct
{
    u32 dummy[8];
} GXTexObj;  /* size = 0x20 */

struct TPLTextureHeader
{
    /*0x00*/ u32 format;
    /*0x04*/ u32 imageOffset;
    /*0x08*/ u16 width;
    /*0x0A*/ u16 height;
    /*0x0C*/ u16 unkC;
}; /* size = 0x10 */

struct TPL
{
    /*0x00*/ u32 numTextures;
    /*0x04*/ struct TPLTextureHeader * DPTR texHeaders;
    /*0x08*/ u8 * DPTR fileData;
    /*0x0C*/ GXTexObj * DPTR texObjs;
};

/* A bitmap (texture) archive and its load state. */
struct BitmapGroup
{
    /*0x00*/ s32 isLoaded;
    /*0x04*/ char *filename;
    /*0x08*/ char *name;
    /*0x0C*/ u32 unkC;
    /*0x10*/ struct TPL *tpl;
    /*0x14*/ void *heap;
}; /* size = 0x18 */

/* A queued 2D bitmap draw (SMB1's struct Bitmap). */
struct Bitmap
{
    /*0x00*/ float x;
    /*0x04*/ float y;
    /*0x08*/ float scaleX;
    /*0x0C*/ float scaleY;
    /*0x10*/ s16 imageId;  /* group << 8 | index */
    /*0x12*/ s16 rotation;
    /*0x14*/ u8 align;  /* ALIGN_* */
    /*0x15*/ u8 r;
    /*0x16*/ u8 g;
    /*0x17*/ u8 b;
    /*0x18*/ u8 a;
}; /* size = 0x1C */

enum
{
    ALIGN_LT, ALIGN_LC, ALIGN_LB,
    ALIGN_CT, ALIGN_CC, ALIGN_CB,
    ALIGN_RT, ALIGN_RC, ALIGN_RB,
};

/* A bitmap font: a grid of glyphs in one texture. */
struct FontParams
{
    /*0x00*/ s16 bmpId;
    /*0x02*/ u8 spaceWidth;
    /*0x03*/ u8 lineHeight;
    /*0x04*/ s32 firstChar;
    /*0x08*/ s32 lastChar;
    /*0x0C*/ s8 charsPerRow;
    /*0x10*/ float unk10;
    /*0x14*/ float unk14;
    /*0x18*/ float unk18;
    /*0x1C*/ float unk1C;
    /*0x20*/ s8 unk20;
    /*0x21*/ s8 unk21;
    /*0x22*/ s8 unk22;
    /*0x23*/ s8 unk23;
    u8 unk24[0x28 - 0x24];
    /*0x28*/ u16 size;
    /*0x2C*/ u16 *string;
    /*0x30*/ s16 *pageIdx;
    /*0x34*/ float unk34;
}; /* size = 0x38 */

extern struct FontParams FONT_DEFINITIONS[];

/* A request to draw one 2D sprite (SMB1's NaomiSpriteParams). */
struct SpriteDrawRequest
{
    /*0x00*/ s32 bmpId;
    /*0x04*/ float x;
    /*0x08*/ float y;
    /*0x0C*/ float z;
    /*0x10*/ float scaleX;
    /*0x14*/ float scaleY;
    /*0x18*/ float u1;
    /*0x1C*/ float v1;
    /*0x20*/ float u2;
    /*0x24*/ float v2;
    /*0x28*/ s32 rotation;
    /*0x2C*/ float opacity;
    /*0x30*/ s32 unk30;
    /*0x34*/ s32 flags;
    /*0x38*/ s32 mulColor;
    /*0x3C*/ s32 addColor;
    /*0x40*/ s16 unk40;
    u8 unk42[0x50 - 0x42];
}; /* size = 0x50 */

/* The UI element being set up (draw request plus one more word). */
struct UISpriteDrawRequest
{
    struct SpriteDrawRequest req;
    u32 unk50;
}; /* size = 0x54 */

extern struct UISpriteDrawRequest ui_sprite_draw_req;
extern struct SpriteDrawRequest sprite_draw_req_buffer[];
int draw_sprite_draw_request(struct SpriteDrawRequest *req);

#define BMP_GROUP_COUNT 25
#define BMP_ALL BMP_GROUP_COUNT

extern struct BitmapGroup bmp_infos[BMP_GROUP_COUNT];
extern struct TPL *bmp_com_tpl;

void load_bmp_com(void);
struct TPL *load_bmp(char *filename);
void queue_bmp_by_id_read(int grpId);
void load_bmp_by_id_child(int grpId);
void free_bmp_by_id(int grpId);
void free_nl2ngc_tpl_buf_to_heap(struct TPL *tpl);
void set_bmp_com_tpl_pointer(int grpId);
void g_zero_some_sprite_related_state(void);
void g_some_sprite_reset_gx_state_func(void);
void draw_ui(void);

#endif
