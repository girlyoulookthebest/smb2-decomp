/* Bitmap (TPL texture) archives and 2D UI sprite drawing. */
#include "types.h"
#include "bitmap.h"
#include "file.h"
#include "libc.h"
#include "gx.h"
#include "mathutil.h"
#include "dip.h"
#include "event.h"

/* Bitmap groups (archive file, name), indexed by bmpId >> 8. */
struct BitmapGroup bmp_infos[BMP_GROUP_COUNT] =
{
    { 0, "bmp/bmp_com.lz", "BMP_COM", 0, NULL, NULL },
    { 0, "bmp/bmp_adv.lz", "BMP_ADV", 0, NULL, NULL },
    { 0, "bmp/bmp_end.tpl", "BMP_END", 0, NULL, NULL },
    { 0, "bmp/bmp_rnk.tpl", "BMP_RNK", 0, NULL, NULL },
    { 0, "bmp/bmp_sel.tpl", "BMP_SEL", 0, NULL, NULL },
    { 0, "bmp/bmp_nml.tpl", "BMP_NML", 0, NULL, NULL },
    { 0, "bmp/bmp_bwl.tpl", "BMP_BWL", 0, NULL, NULL },
    { 0, "bmp/bmp_rac.tpl", "BMP_RAC", 0, NULL, NULL },
    { 0, "bmp/bmp_bil.tpl", "BMP_BIL", 0, NULL, NULL },
    { 0, "bmp/bmp_fgt.tpl", "BMP_FGT", 0, NULL, NULL },
    { 0, "bmp/bmp_glf.tpl", "BMP_GLF", 0, NULL, NULL },
    { 0, "bmp/bmp_tgt.tpl", "BMP_TGT", 0, NULL, NULL },
    { 0, "bmp/bmp_how.tpl", "BMP_HOW", 0, NULL, NULL },
    { 0, "bmp/bmp_cmd.tpl", "BMP_CMD", 0, NULL, NULL },
    { 0, "bmp/bmp_boa.tpl", "BMP_BOA", 0, NULL, NULL },
    { 0, "bmp/bmp_stm2.tpl", "BMP_STM2", 0, NULL, NULL },
    { 0, "bmp/bmp_glf2.tpl", "BMP_GLF2", 0, NULL, NULL },
    { 0, "bmp/bmp_fut2.tpl", "BMP_FUT2", 0, NULL, NULL },
    { 0, "bmp/bmp_dog2.tpl", "BMP_DOG2", 0, NULL, NULL },
    { 0, "bmp/bmp_fgt2.tpl", "BMP_FGT2", 0, NULL, NULL },
    { 0, "bmp/bmp_gun2.tpl", "BMP_GUN2", 0, NULL, NULL },
    { 0, "bmp/bmp_tennis.tpl", "BMP_TENNIS", 0, NULL, NULL },
    { 0, "bmp/bmp_bbl.tpl", "BMP_BBL", 0, NULL, NULL },
    { 0, "bmp/bmp_sel2.tpl", "BMP_SEL2", 0, NULL, NULL },
    { 0, "bmp/bmp_save.tpl", "BMP_SAVE", 0, NULL, NULL },
};

extern char *bmpComNames[];
extern char *bmpAdvNames[];
extern char *bmpEndNames[];
extern char *bmpRnkNames[];
extern char *bmpSelNames[];
extern char *bmpNmlNames[];
extern char *bmpBwlNames[];
extern char *bmpRacNames[];
extern char *bmpBilNames[];
extern char *bmpFgtNames[];
extern char *bmpGlfNames[];
extern char *bmpTgtNames[];
extern char *bmpHowNames[];
extern char *bmpCmdNames[];
extern char *bmpBoaNames[];
extern char *bmpStm2Names[];
extern char *bmpGlf2Names[];
extern char *bmpFut2Names[];
extern char *bmpDog2Names[];
extern char *bmpFgt2Names[];
extern char *bmpGun2Names[];
extern char *bmpTennisNames[];
extern char *bmpBblNames[];
extern char *bmpSel2Names[];
extern char *bmpSaveNames[];

/* Texture names in each group (debug use). */
char **bmp_tex_names[BMP_GROUP_COUNT] =
{
    bmpComNames,
    bmpAdvNames,
    bmpEndNames,
    bmpRnkNames,
    bmpSelNames,
    bmpNmlNames,
    bmpBwlNames,
    bmpRacNames,
    bmpBilNames,
    bmpFgtNames,
    bmpGlfNames,
    bmpTgtNames,
    bmpHowNames,
    bmpCmdNames,
    bmpBoaNames,
    bmpStm2Names,
    bmpGlf2Names,
    bmpFut2Names,
    bmpDog2Names,
    bmpFgt2Names,
    bmpGun2Names,
    bmpTennisNames,
    bmpBblNames,
    bmpSel2Names,
    bmpSaveNames,
};

u32 lbl_1_data_12AA4 = 0xFFFFFFFF;  /* SMB1's lbl_802F04A8 (set by text code 0x81) */

/* .bss */
u8 *textPtr;
u8 textBuf[0x800];
u16 bitmapCount;
u32 currFont;
s32 active_sprite_draw_req_count;
s32 g_smth_with_sprite_draw_reqs_widescreen;  /* drawing phase (SMB1's lbl_802F1D04) */


static u8 lzssHeader[0x20] __attribute__((aligned(32)));
struct TPL *bmp_com_tpl;
struct Bitmap bitmapList[256];
s32 has_last_widescreen_scale;
u16 last_widescreen_scale_pivot_x;
u16 textStartX;
u16 textX;
u16 textY;
float textScale;  /* SMB1's lbl_802F1CE8 */
struct SpriteDrawRequest sprite_draw_req_buffer[640];

/* Unreferenced (stripped at link); only fixes the .bss order, which follows first use. */
void bitmap_force_bss_order(void **p)
{
    p[0] = &textPtr;
    p[1] = textBuf;
    p[2] = &bitmapCount;
    p[3] = &currFont;
    p[4] = &active_sprite_draw_req_count;
    p[5] = &g_smth_with_sprite_draw_reqs_widescreen;
    p[6] = lzssHeader;
    p[7] = &bmp_com_tpl;
    p[8] = bitmapList;
    p[9] = &has_last_widescreen_scale;
    p[10] = &last_widescreen_scale_pivot_x;
    p[11] = &textStartX;
    p[12] = &textX;
    p[13] = &textY;
    p[14] = &textScale;
    p[15] = sprite_draw_req_buffer;
}

extern void *currentHeap;
extern void *main_heap;

void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void decompress_lz(void *src, void *dst);
void DCFlushRange(void *addr, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInvalidateTexAll(void);
void queue_disc_read(char *file);
#ifdef __MWERKS__
int strncmp(const char *a, const char *b, unsigned long n);
#endif
void fn_800668FC(void);
void g_something_with_GX_vtx_desc(u32 attrs);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevColor(int reg, GXColor color);
void g_something_with_raster_state(int a);
void GXSetCullMode_cached(int mode);
void GXLoadPosMtxImm(Mtx m, int id);
void GXGetProjectionv(float *p);
void C_MTXOrtho(Mtx44 m, float t, float b, float l, float r, float n, float f);
void g_smth_with_drawing_all_sprites(int a);
void g_draw_sprite_requests_from_buffer(void);
void g_smth_with_drawing_text_sprites(void);
void g_smth_with_screen_fading(void);
void g_something_with_card6(void);
void draw_debugtext(void);
void g_smth_with_projection_mtx(float *projParams);
void bitmap_draw(struct Bitmap *bmp);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void bitmap_draw_string(void);
void bitmap_draw_normal_char(unsigned char chr);
#ifdef __MWERKS__
unsigned long strlen(const char *s);
#endif

#ifndef __MWERKS__
void pc_swap_tpl(void *fileData);
#endif

void load_bmp_com(void)
{
    load_bmp_by_id_child(0);
    set_bmp_com_tpl_pointer(0);
    textPtr = textBuf;
    bitmapCount = 0;
    currFont = 0;
    active_sprite_draw_req_count = 0;
    g_smth_with_sprite_draw_reqs_widescreen = 0;
}

/* Load a TPL texture archive (LZ-compressed if the name ends in ".lz") and set up its
 * texture objects. */
struct TPL *load_bmp(char *filename)
{
    struct TPL *tpl;
    u8 *fileData;
    u32 size;
    u32 i;
    int len;
    struct File file;

    if (!file_open(filename, &file))
        return NULL;
    size = OSRoundUp32B(file_size(&file));
    len = strlen(filename);
    if (len >= 3 && strncmp(filename + (len - 3), ".lz", 3) == 0)
    {
        u32 compressedSize;
        void *lzData;

        if (file_read(&file, lzssHeader, 0x20, 0) < 0)
            return NULL;
        compressedSize = OSRoundUp32B(LWBRX(lzssHeader, 0));
        size = OSRoundUp32B(LWBRX(lzssHeader, 4));
        tpl = alloc_from_heap_or_panic(currentHeap, size + 16, "bitmap.c", 163);
        lzData = alloc_from_heap_or_panic(main_heap, compressedSize, "bitmap.c", 164);
        if (file_read(&file, lzData, compressedSize, 0) < 0)
            return NULL;
        if (file_close(&file) != 1)
            return NULL;
        fileData = (u8 *)OSRoundUp32B((u32)tpl + 0x10);
        decompress_lz(lzData, fileData);
        DCFlushRange(fileData, size);
        OSFreeToHeap(main_heap, lzData);
    }
    else
    {
        tpl = alloc_from_heap_or_panic(currentHeap, OSRoundUp32B(size) + 0x20, "bitmap.c", 176);
        fileData = (u8 *)OSRoundUp32B((u32)tpl + 0x10);
        file_read(&file, fileData, size, 0);
        file_close(&file);
    }

#ifndef __MWERKS__
    pc_swap_tpl(fileData); /* PC port: TPL headers are big-endian on disc */
#endif
    tpl->numTextures = *(u32 *)(fileData + 0);
    tpl->texHeaders = (struct TPLTextureHeader *)(fileData + 4);
    tpl->texObjs = alloc_from_heap_or_panic(currentHeap, tpl->numTextures * sizeof(GXTexObj), "bitmap.c", 185);
    for (i = 0; i < tpl->numTextures; i++)
    {
        struct TPLTextureHeader *texHdr = &tpl->texHeaders[i];
        void *imgPtr = fileData + texHdr->imageOffset;

        if (!(texHdr->format & (1 << 8)) && texHdr->width != 0 && texHdr->height != 0)
            GXInitTexObj(&tpl->texObjs[i], imgPtr, texHdr->width, texHdr->height, texHdr->format & 0x1F, 0, 0, 0);
    }
    GXInvalidateTexAll();
    return tpl;
}

void queue_bmp_by_id_read(int grpId)
{
    if (bmp_infos[grpId].isLoaded == 0)
        queue_disc_read(bmp_infos[grpId].filename);
}

void load_bmp_by_id_child(int grpId)
{
    struct BitmapGroup *grp = &bmp_infos[grpId];

    if (grp->isLoaded == 0)
    {
        grp->tpl = load_bmp(bmp_infos[grpId].filename);
        grp->isLoaded = 1;
        grp->heap = currentHeap;
    }
}

/* Free one bitmap group, or with BMP_ALL every group except the common one. */
void free_bmp_by_id(int grpId)
{
    int curr;
    int end;
    struct BitmapGroup *grp;

    if (grpId == BMP_ALL)
    {
        curr = 1;
        end = BMP_GROUP_COUNT;
    }
    else
    {
        curr = grpId;
        end = grpId + 1;
    }
    grp = &bmp_infos[curr];
    while (curr < end)
    {
        if (grp->isLoaded)
        {
            OSFreeToHeap(grp->heap, grp->tpl->texObjs);
            OSFreeToHeap(grp->heap, grp->tpl);
            grp->isLoaded = 0;
        }
        curr++;
        grp++;
    }
}

void free_nl2ngc_tpl_buf_to_heap(struct TPL *tpl)
{
    OSFreeToHeap(currentHeap, tpl->texObjs);
    OSFreeToHeap(currentHeap, tpl);
}

void set_bmp_com_tpl_pointer(int grpId)
{
    bmp_com_tpl = bmp_infos[grpId].tpl;
}

void g_zero_some_sprite_related_state(void)
{
    active_sprite_draw_req_count = 0;
    g_smth_with_sprite_draw_reqs_widescreen = 0;
}

/* GX state for 2D bitmap drawing: one texture stage, color = tex * C0 + C1, no lighting. */
void g_some_sprite_reset_gx_state_func(void)
{
    GXColor green = { 0, 255, 0, 255 };

    fn_800668FC();
    g_something_with_GX_vtx_desc((1 << 9) | (1 << 13));
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    GXSetNumIndStages_cached(0);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 60, 0, 125);
    g_GXSetTevOrder_wrapper(0, 0, 0, 255);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTevColorIn_cached(0, 15, 2, 8, 4);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 1, 4, 2);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevColor(1, green);
    GXSetZMode_cached(1, 1, 1);
    GXSetBlendMode_cached(1, 4, 5, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(2);
}

/* Draw all 2D: queued bitmaps, text, sprites, screen fade, memory card UI and debug text,
 * in an orthographic 640x480 projection. */
void draw_ui(void)
{
    float projParams[7];
    Mtx44 m;
    struct Bitmap *bmp;

    g_some_sprite_reset_gx_state_func();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    has_last_widescreen_scale = 0;
    last_widescreen_scale_pivot_x = 0;
    C_MTXPerspective(m, 60.0f, 1.3333334f, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(m, 0);
    g_smth_with_sprite_draw_reqs_widescreen = 1;
    if (!(dip_switches & DIP_SPRITE_OFF) && events[17].state == EV_STATE_RUNNING)
        g_smth_with_drawing_all_sprites(0);
    GXGetProjectionv(projParams);
    C_MTXOrtho(m, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, 20000.0f);
    g_something_with_GXSetProjectionv(m, 1);

    bmp = bitmapList;
    while (bitmapCount != 0)
    {
        bitmap_draw(bmp);
        bitmapCount--;
        bmp++;
    }
    bitmapCount = 0;

    bitmap_draw_string();
    g_smth_with_sprite_draw_reqs_widescreen = 2;
    if (!(dip_switches & DIP_SPRITE_OFF))
        g_draw_sprite_requests_from_buffer();
    active_sprite_draw_req_count = 0;
    g_smth_with_sprite_draw_reqs_widescreen = 1;
    if (!(dip_switches & DIP_SPRITE_OFF))
    {
        if (events[17].state == EV_STATE_RUNNING)
            g_smth_with_drawing_all_sprites(1);
        if (events[18].state == EV_STATE_RUNNING)
            g_smth_with_drawing_text_sprites();
    }
    g_smth_with_sprite_draw_reqs_widescreen = 3;
    g_smth_with_screen_fading();
    g_smth_with_sprite_draw_reqs_widescreen = 2;
    g_draw_sprite_requests_from_buffer();

    GXSetZMode_cached(1, 7, 0);
    g_something_with_card6();
    GXSetZMode_cached(1, 3, 1);

    bmp_com_tpl = bmp_infos[0].tpl;
    if (dip_switches & (DIP_DISP | DIP_TEST_TEAM))
        draw_debugtext();
    g_smth_with_projection_mtx(projParams);
}

/* Stand-ins for small functions the linker stripped; only their constants remain. */
void bitmap_stripped_literals(float *f)
{
    f[0] = 16.0f;
    f[1] = 320.0f;
    f[2] = 240.0f;
}

float get_lbl_802F3100(void) { return 1.0f; }
float bitmap_stripped_half(void) { return 0.5f; }

/* Top-left corner of a bitmap of the given size, from its position and alignment. */
static inline void bitmap_calc_pos(struct Bitmap *bmp, float *x, float *y, float width, float height)
{
    *x = bmp->x;
    *y = bmp->y;

    switch (bmp->align)
    {
    case ALIGN_CT:
    case ALIGN_CC:
    case ALIGN_CB:
        *x -= width / 2.0f;
        break;
    case ALIGN_RT:
    case ALIGN_RC:
    case ALIGN_RB:
        *x -= width;
        break;
    }

    switch (bmp->align)
    {
    case ALIGN_LC:
    case ALIGN_CC:
    case ALIGN_RC:
        *y -= height / 2.0f;
        break;
    case ALIGN_LB:
    case ALIGN_CB:
    case ALIGN_RB:
        *y -= height;
        break;
    }
}

void bitmap_draw(struct Bitmap *bmp)
{
    struct TPLTextureHeader *tex;
    float width;
    float height;
    float x;
    float y;
    Vec right = { 0.0f, 0.0f, 0.0f };
    Vec down = { 0.0f, 0.0f, 0.0f };
    Vec corner = { 0.0f, 0.0f, 0.0f };
    GXColor tevColor;

    tex = &bmp_infos[(bmp->imageId & 0xFF00) >> 8].tpl->texHeaders[bmp->imageId & 0xFF];
    width = tex->width * bmp->scaleX;
    height = tex->height * bmp->scaleY;
    bitmap_calc_pos(bmp, &x, &y, width, height);
    right.x = width;
    down.y = height;
    corner.x = width;
    corner.y = height;
    mtxa_from_rotate_z(-bmp->rotation);
    mtxa_tf_vec(&right, &right);
    mtxa_tf_vec(&down, &down);
    mtxa_tf_vec(&corner, &corner);

    GXLoadTexObj_cached(&bmp_infos[(bmp->imageId & 0xFF00) >> 8].tpl->texObjs[bmp->imageId & 0xFF], 0);

    tevColor.r = bmp->r;
    tevColor.g = bmp->g;
    tevColor.b = bmp->b;
    tevColor.a = bmp->a;
    GXSetTevColor(1, tevColor);

    tevColor.r = 0;
    tevColor.g = 0;
    tevColor.b = 0;
    tevColor.a = 0;
    GXSetTevColor(2, tevColor);

    GXBegin(0x80, 7, 4);
        GXPosition3f32(x, y, -128.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x + right.x, y + right.y, -128.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x + corner.x, y + corner.y, -128.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(x + down.x, y + down.y, -128.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    GXEnd();
}

void bitmap_draw_normal_char(unsigned char chr)
{
    int idx;
    int row;
    int col;
    struct FontParams *font;
    struct TPLTextureHeader *tex;
    float x;
    float y;
    float u;
    float v;
    float uScale;
    float vScale;
    GXColor tevColor;

    font = &FONT_DEFINITIONS[currFont];
    tex = &bmp_infos[(font->bmpId & 0xFF00) >> 8].tpl->texHeaders[font->bmpId & 0xFF];
    x = textX;
    y = textY;
    idx = chr - font->firstChar;
    row = idx / font->charsPerRow;
    col = idx % font->charsPerRow;

    uScale = 128.0f / tex->width;
    vScale = 128.0f / tex->height;

    u = uScale * (col * font->spaceWidth);
    v = vScale * (row * font->lineHeight);
    GXLoadTexObj_cached(&bmp_infos[(font->bmpId & 0xFF00) >> 8].tpl->texObjs[font->bmpId & 0xFF], 0);

    tevColor.r = 255;
    tevColor.g = 255;
    tevColor.b = 255;
    tevColor.a = 255;
    GXSetTevColor(1, tevColor);

    tevColor.r = 0;
    tevColor.g = 0;
    tevColor.b = 0;
    tevColor.a = 0;
    GXSetTevColor(2, tevColor);

    GXBegin(0x80, 7, 4);
        GXPosition3f32(x, y, -1.0f);
        GXTexCoord2f32(u, v);
        GXPosition3f32(x + font->spaceWidth - 1.0f, y, -1.0f);
        GXTexCoord2f32(u + font->spaceWidth * uScale - 1.0f, v);
        GXPosition3f32(x + font->spaceWidth - 1.0f, y + font->lineHeight - 1.0f, -1.0f);
        GXTexCoord2f32(u + font->spaceWidth * uScale - 1.0f, v + font->lineHeight * vScale - 1.0f);
        GXPosition3f32(x, y + font->lineHeight, -1.0f);
        GXTexCoord2f32(u, v + font->lineHeight * vScale - 1.0f);
    GXEnd();

    textX += font->spaceWidth;
}

static inline void bitmap_draw_char(unsigned char chr)
{
    struct FontParams *font = &FONT_DEFINITIONS[currFont];

    switch (chr)
    {
    case 0:
        break;
    case '\n':
        textX = textStartX;
        textY += font->lineHeight;
        break;
    case '\t':
        textX += font->spaceWidth * 8;
        textX &= ~(font->spaceWidth * 8 - 1);
        break;
    case '\r':
        textX = textStartX;
        break;
    case ' ':
        textX += font->spaceWidth;
        break;
    default:
        bitmap_draw_normal_char(chr);
        break;
    }
}

/* Draw the queued text: a byte stream of characters and control codes (0x80 font,
 * 0x81 ?, 0x82 position, 0x83 scale), ended by 0x84. */
void bitmap_draw_string(void)
{
    unsigned char *str;
    u16 x;
    u16 y;

    *textPtr = 0x84;
    str = textBuf;
    while (*str != 0x84)
    {
        switch (*str)
        {
        case 0x80:
            str++;
            currFont = *str;
            break;
        case 0x81:
            str++;
            lbl_1_data_12AA4 = *str;
            break;
        case 0x83:
            str++;
            textScale = *str / 16.0f;
            break;
        case 0x82:
            str++;
            x = *str << 8;
            str++;
            x |= *str;
            str++;
            y = *str << 8;
            str++;
            y |= *str;
            textX = x;
            textStartX = x;
            textY = y;
            break;
        default:
            bitmap_draw_char(*str);
            break;
        }
        str++;
    }
    textPtr = textBuf;
}

void g_draw_sprite_requests_from_buffer(void)
{
    int i;
    struct SpriteDrawRequest *req = sprite_draw_req_buffer;

    for (i = 0; i < active_sprite_draw_req_count; i++, req++)
        draw_sprite_draw_request(req);
}

int g_get_bmp_width(int bmpId)
{
    int grp = bmpId >> 8;

    if (bmp_infos[grp].isLoaded == 0)
        return 0;
    return bmp_infos[grp].tpl->texHeaders[bmpId & 0xFF].width;
}

int g_get_bmp_height(int bmpId)
{
    int grp = bmpId >> 8;

    if (bmp_infos[grp].isLoaded == 0)
        return 0;
    return bmp_infos[grp].tpl->texHeaders[bmpId & 0xFF].height;
}

/* UI elements: set up ui_sprite_draw_req field by field, then draw it. */
void init_ui_element_sprite_with_defaults(void)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    memset(req, 0, sizeof(*req));
    req->z = 1.0f;
    req->scaleX = 1.0f;
    req->scaleY = 1.0f;
    req->u2 = 1.0f;
    req->v2 = 1.0f;
    req->opacity = 1.0f;
    req->unk30 = -1;
    req->flags = 0xA;
    req->mulColor = 0xFFFFFFFF;
}

void set_ui_element_sprite_pos(float x, float y)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->x = x;
    req->y = y;
}

void set_ui_element_sprite_depth(float z)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->z = z;
}

void g_set_ui_element_sprite_flags(u32 flags)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->flags = (req->flags & ~0xF) | (flags & 0xF);
}

void set_ui_element_sprite_scale(float x, float y)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->scaleX = x;
    req->scaleY = y;
}

void set_ui_element_sprite_mult_color_from_u32(u32 color)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->mulColor = color;
}

void set_ui_element_sprite_mult_color(u8 r, u8 g, u8 b, u8 a)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->mulColor = (r << 24) | (g << 16) | (b << 8) | a;
}

void set_ui_element_sprite_add_color_from_u32(u32 color)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->addColor = color;
}

void set_ui_element_sprite_add_color(u8 r, u8 g, u8 b, u8 a)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->addColor = (r << 24) | (g << 16) | (b << 8) | a;
}

void set_ui_element_sprite_alpha(float alpha)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->opacity = alpha;
}

void set_ui_element_sprite_rot_z(s32 rot)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->rotation = rot;
}

void g_set_some_ui_element_sprite_flag(u32 flags)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->flags |= flags & ~0xF;
}

void draw_ui_element(s32 bmpId)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;

    req->bmpId = bmpId;
    draw_sprite_draw_request(req);
}

static inline int bmp_tex_width(int bmpId)
{
    int grp = bmpId >> 8;

    return (bmp_infos[grp].isLoaded != 0) ? bmp_infos[grp].tpl->texHeaders[bmpId & 0xFF].width : 0;
}

static inline int bmp_tex_height(int bmpId)
{
    int grp = bmpId >> 8;

    return (bmp_infos[grp].isLoaded != 0) ? bmp_infos[grp].tpl->texHeaders[bmpId & 0xFF].height : 0;
}

/* Draw a box from a bitmap as nine pieces: fixed 6-pixel corners and edges, and a centre
 * stretched to the requested size (bitmap size times the element's scale). */
void draw_ui_box(s32 bmpId)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;
    struct SpriteDrawRequest part;
    float texW;
    float texH;
    float w;
    float h;

    req->bmpId = bmpId;
    texW = bmp_tex_width(bmpId);
    texH = bmp_tex_height(bmpId);
    w = texW * req->scaleX;
    h = texH * req->scaleY;
    switch (req->flags & 3)
    {
    case 1:
        req->x += w / 2.0f;
        break;
    case 3:
        req->x -= w / 2.0f;
        break;
    }
    switch ((req->flags & 0xC) >> 2)
    {
    case 1:
        req->y += h / 2.0f;
        break;
    case 3:
        req->y -= h / 2.0f;
        break;
    }
    req->flags = (req->flags & ~0xFF) | 0xA;

    /* centre */
    part = *req;
    part.u1 = 6.0 / texW;
    part.v1 = 6.0 / texH;
    part.u2 = 1.0 - 6.0 / texW;
    part.v2 = 1.0 - 6.0 / texH;
    part.scaleX = (w - 12.0) / texW;
    part.scaleY = (h - 12.0) / texH;
    draw_sprite_draw_request(&part);

    /* top edge */
    part = *req;
    part.y = 3.0 + (req->y - 0.5 * h);
    part.u1 = 6.0 / texW;
    part.v1 = 0.0f;
    part.u2 = 1.0 - 6.0 / texW;
    part.v2 = 6.0 / texH;
    part.scaleX = (w - 12.0) / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* bottom edge */
    part = *req;
    part.y = req->y + 0.5 * h - 3.0;
    part.u1 = 6.0 / texW;
    part.v1 = 1.0 - 6.0 / texH;
    part.u2 = 1.0 - 6.0 / texW;
    part.v2 = 1.0f;
    part.scaleX = (w - 12.0) / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* left edge */
    part = *req;
    part.x = 3.0 + (req->x - 0.5 * w);
    part.u1 = 0.0f;
    part.v1 = 6.0 / texH;
    part.u2 = 6.0 / texW;
    part.v2 = 1.0 - 6.0 / texH;
    part.scaleX = 6.0 / texW;
    part.scaleY = (h - 12.0) / texH;
    draw_sprite_draw_request(&part);

    /* right edge */
    part = *req;
    part.x = req->x + 0.5 * w - 3.0;
    part.u1 = 1.0 - 6.0 / texW;
    part.v1 = 6.0 / texH;
    part.u2 = 1.0f;
    part.v2 = 1.0 - 6.0 / texH;
    part.scaleX = 6.0 / texW;
    part.scaleY = (h - 12.0) / texH;
    draw_sprite_draw_request(&part);

    /* top-left corner */
    part = *req;
    part.x = 3.0 + (req->x - 0.5 * w);
    part.y = 3.0 + (req->y - 0.5 * h);
    part.u1 = 0.0f;
    part.v1 = 0.0f;
    part.u2 = 6.0 / texW;
    part.v2 = 6.0 / texH;
    part.scaleX = 6.0 / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* top-right corner */
    part = *req;
    part.x = req->x + 0.5 * w - 3.0;
    part.y = 3.0 + (req->y - 0.5 * h);
    part.u1 = 1.0 - 6.0 / texW;
    part.v1 = 0.0f;
    part.u2 = 1.0f;
    part.v2 = 6.0 / texH;
    part.scaleX = 6.0 / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* bottom-left corner */
    part = *req;
    part.x = 3.0 + (req->x - 0.5 * w);
    part.y = req->y + 0.5 * h - 3.0;
    part.u1 = 0.0f;
    part.v1 = 1.0 - 6.0 / texH;
    part.u2 = 6.0 / texW;
    part.v2 = 1.0f;
    part.scaleX = 6.0 / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* bottom-right corner */
    part = *req;
    part.x = req->x + 0.5 * w - 3.0;
    part.y = req->y + 0.5 * h - 3.0;
    part.u1 = 1.0 - 6.0 / texW;
    part.v1 = 1.0 - 6.0 / texH;
    part.u2 = 1.0f;
    part.v2 = 1.0f;
    part.scaleX = 6.0 / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);
}

/* draw_ui_box without the centre piece (and without applying the alignment flags). */
void draw_ui_box_only_borders(s32 bmpId)
{
    struct SpriteDrawRequest *req = &ui_sprite_draw_req.req;
    struct SpriteDrawRequest part;
    float texW;
    float texH;
    float w;
    float h;

    req->bmpId = bmpId;
    texW = bmp_tex_width(bmpId);
    texH = bmp_tex_height(bmpId);
    w = texW * req->scaleX;
    h = texH * req->scaleY;

    /* top edge */
    part = *req;
    part.y = 3.0 + (req->y - 0.5 * h);
    part.u1 = 6.0 / texW;
    part.v1 = 0.0f;
    part.u2 = 1.0 - 6.0 / texW;
    part.v2 = 6.0 / texH;
    part.scaleX = (w - 12.0) / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* bottom edge */
    part = *req;
    part.y = req->y + 0.5 * h - 3.0;
    part.u1 = 6.0 / texW;
    part.v1 = 1.0 - 6.0 / texH;
    part.u2 = 1.0 - 6.0 / texW;
    part.v2 = 1.0f;
    part.scaleX = (w - 12.0) / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* left edge */
    part = *req;
    part.x = 3.0 + (req->x - 0.5 * w);
    part.u1 = 0.0f;
    part.v1 = 6.0 / texH;
    part.u2 = 6.0 / texW;
    part.v2 = 1.0 - 6.0 / texH;
    part.scaleX = 6.0 / texW;
    part.scaleY = (h - 12.0) / texH;
    draw_sprite_draw_request(&part);

    /* right edge */
    part = *req;
    part.x = req->x + 0.5 * w - 3.0;
    part.u1 = 1.0 - 6.0 / texW;
    part.v1 = 6.0 / texH;
    part.u2 = 1.0f;
    part.v2 = 1.0 - 6.0 / texH;
    part.scaleX = 6.0 / texW;
    part.scaleY = (h - 12.0) / texH;
    draw_sprite_draw_request(&part);

    /* top-left corner */
    part = *req;
    part.x = 3.0 + (req->x - 0.5 * w);
    part.y = 3.0 + (req->y - 0.5 * h);
    part.u1 = 0.0f;
    part.v1 = 0.0f;
    part.u2 = 6.0 / texW;
    part.v2 = 6.0 / texH;
    part.scaleX = 6.0 / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* top-right corner */
    part = *req;
    part.x = req->x + 0.5 * w - 3.0;
    part.y = 3.0 + (req->y - 0.5 * h);
    part.u1 = 1.0 - 6.0 / texW;
    part.v1 = 0.0f;
    part.u2 = 1.0f;
    part.v2 = 6.0 / texH;
    part.scaleX = 6.0 / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* bottom-left corner */
    part = *req;
    part.x = 3.0 + (req->x - 0.5 * w);
    part.y = req->y + 0.5 * h - 3.0;
    part.u1 = 0.0f;
    part.v1 = 1.0 - 6.0 / texH;
    part.u2 = 6.0 / texW;
    part.v2 = 1.0f;
    part.scaleX = 6.0 / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);

    /* bottom-right corner */
    part = *req;
    part.x = req->x + 0.5 * w - 3.0;
    part.y = req->y + 0.5 * h - 3.0;
    part.u1 = 1.0 - 6.0 / texW;
    part.v1 = 1.0 - 6.0 / texH;
    part.u2 = 1.0f;
    part.v2 = 1.0f;
    part.scaleX = 6.0 / texW;
    part.scaleY = 6.0 / texH;
    draw_sprite_draw_request(&part);
}

/* Defaults for UI element sprites (init_ui_element_sprite_with_defaults sets the same). */
struct UISpriteDrawRequest ui_sprite_draw_req =
{
    { 0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 0xA, 0xFFFFFFFF, 0 },
};

#include "bitmap_names.inc"
