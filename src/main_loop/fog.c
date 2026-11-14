#pragma peephole off

#include "types.h"
#include "gx.h"
#include "avdisp.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "stage.h"

struct StageFogState
{
    /*0x00*/ s8 type;
    /*0x01*/ u8 red;
    /*0x02*/ u8 green;
    /*0x03*/ u8 blue;
    /*0x04*/ float start;
    /*0x08*/ float end;
};

struct FogThemeEntry
{
    /*0x00*/ s8 theme;
    /*0x01*/ s8 type;
    /*0x04*/ float start;
    /*0x08*/ float end;
    /*0x0C*/ u8 r;
    /*0x0D*/ u8 g;
    /*0x0E*/ u8 b;
};

struct FogParams
{
    /*0x00*/ s8 type;
    /*0x04*/ float start;
    /*0x08*/ float end;
    /*0x0C*/ u8 r;
    /*0x0D*/ u8 g;
    /*0x0E*/ u8 b;
    /*0x0F*/ s8 enabled;
};

struct LineParams
{
    /*0x00*/ u8 width;
    /*0x04*/ int blendType;
    /*0x08*/ int blendSrc;
    /*0x0C*/ int blendDst;
    /*0x10*/ int blendOp;
    /*0x14*/ int texOffset;
    /*0x18*/ int unk18;
};

struct LineVertex
{
    /*0x00*/ Vec pos;
    /*0x0C*/ GXColor color;
};

struct LineNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ Mtx mtx;
    /*0x38*/ int primitive;
    /*0x3C*/ u16 count;
    /*0x40*/ struct LineVertex *verts;
    /*0x44*/ struct LineParams params;
};

extern struct StageFogState g_fog_type;

void fn_1_10A608(int enabled);
void avdisp_set_fog_params(int type, float start, float end);
void avdisp_set_fog_color(u8 r, u8 g, u8 b);
void g_something_with_GX_vtx_desc(u32 attrs);
void g_something_with_raster_state(int a);
void g_something_with_fog(int type, float start, float end);
void fn_80066520(GXColor color);
void GXBegin(int primitive, int vtxFmt, u16 count);
void GXLoadPosMtxImm(Mtx m, u32 id);
void GXSetLineWidth_cached(u8 width, int texOffset);
void GXSetNumChans_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);
void g_GXSetTevIndirect_zero_if_different(int stage);
void *ord_tbl_alloc_node(u32 size);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);

void fn_1_E2D20(void);
void fn_1_E35BC(void);
void fn_1_E36A0(struct OrdTblNode *node);

const struct FogThemeEntry ICE_FOG_THEME_ID[] = {
    { 8, 5, 150.0f, 1880.0f, 0xEC, 0xFA, 0xFF },
    { -1, 0, 0.0f, 0.0f, 0, 0, 0 },
};

s8 lbl_1_bss_CF228;
struct FogParams lbl_1_bss_CF22C;

struct LineParams lbl_1_data_53410 = { 6, 0, 1, 0, 0, 0, 0 };

void fn_1_E2918(void)
{
    int i;
    const struct FogThemeEntry *entry;

    lbl_1_bss_CF228 = 0;
    if (g_fog_type.type >= 0)
    {
        lbl_1_bss_CF22C.enabled = 1;
        lbl_1_bss_CF22C.type = g_fog_type.type;
        lbl_1_bss_CF22C.start = g_fog_type.start;
        lbl_1_bss_CF22C.end = g_fog_type.end;
        lbl_1_bss_CF22C.r = g_fog_type.red;
        lbl_1_bss_CF22C.g = g_fog_type.green;
        lbl_1_bss_CF22C.b = g_fog_type.blue;
    }
    else
    {
        entry = ICE_FOG_THEME_ID;
        for (i = 0; entry->theme != -1; i++, entry++)
        {
            if (entry->theme == world_theme)
            {
                lbl_1_bss_CF22C.enabled = 1;
                lbl_1_bss_CF22C.type = entry->type;
                lbl_1_bss_CF22C.start = entry->start;
                lbl_1_bss_CF22C.end = entry->end;
                lbl_1_bss_CF22C.r = entry->r;
                lbl_1_bss_CF22C.g = entry->g;
                lbl_1_bss_CF22C.b = entry->b;
                return;
            }
        }
        lbl_1_bss_CF22C.enabled = 0;
        lbl_1_bss_CF22C.type = 5;
        lbl_1_bss_CF22C.start = 0.0f;
        lbl_1_bss_CF22C.end = 100.0f;
        lbl_1_bss_CF22C.r = 0;
        lbl_1_bss_CF22C.g = 0;
        lbl_1_bss_CF22C.b = 0;
    }
}

void fog_main(void)
{
    if (g_fog_type.type >= 0 && lbl_1_bss_CF228 == 0)
    {
        lbl_1_bss_CF22C.type = g_fog_type.type;
        lbl_1_bss_CF22C.start = g_fog_type.start;
        lbl_1_bss_CF22C.end = g_fog_type.end;
        lbl_1_bss_CF22C.r = g_fog_type.red;
        lbl_1_bss_CF22C.g = g_fog_type.green;
        lbl_1_bss_CF22C.b = g_fog_type.blue;
    }
    fn_1_10A608(lbl_1_bss_CF22C.enabled);
    nl2ngc_enable_fog(lbl_1_bss_CF22C.enabled);
    if (lbl_1_bss_CF22C.enabled)
    {
        avdisp_set_fog_params(lbl_1_bss_CF22C.type, lbl_1_bss_CF22C.start, lbl_1_bss_CF22C.end);
        avdisp_set_fog_color(lbl_1_bss_CF22C.r, lbl_1_bss_CF22C.g, lbl_1_bss_CF22C.b);
        nl2ngc_set_fog_params(lbl_1_bss_CF22C.type, lbl_1_bss_CF22C.start, lbl_1_bss_CF22C.end);
        nl2ngc_set_fog_color(lbl_1_bss_CF22C.r, lbl_1_bss_CF22C.g, lbl_1_bss_CF22C.b);
    }
}

void g_set_something3(int enabled)
{
    fn_1_10A608((s8)enabled);
    nl2ngc_enable_fog((s8)enabled);
}

void fn_1_E2D20(void)
{
    fn_1_10A608(lbl_1_bss_CF22C.enabled);
    nl2ngc_enable_fog(lbl_1_bss_CF22C.enabled);
}

void fn_1_E2D64(s8 type, float start, float end, u8 r, u8 g, u8 b)
{
    fn_1_10A608(1);
    avdisp_set_fog_params(type, start, end);
    avdisp_set_fog_color(r, g, b);
    nl2ngc_enable_fog(1);
    nl2ngc_set_fog_params(type, start, end);
    nl2ngc_set_fog_color(r, g, b);
}

void fn_1_E2E18(void)
{
    fn_1_10A608(lbl_1_bss_CF22C.enabled);
    nl2ngc_enable_fog(lbl_1_bss_CF22C.enabled);
    if (lbl_1_bss_CF22C.enabled)
    {
        avdisp_set_fog_params(lbl_1_bss_CF22C.type, lbl_1_bss_CF22C.start, lbl_1_bss_CF22C.end);
        avdisp_set_fog_color(lbl_1_bss_CF22C.r, lbl_1_bss_CF22C.g, lbl_1_bss_CF22C.b);
        nl2ngc_set_fog_params(lbl_1_bss_CF22C.type, lbl_1_bss_CF22C.start, lbl_1_bss_CF22C.end);
        nl2ngc_set_fog_color(lbl_1_bss_CF22C.r, lbl_1_bss_CF22C.g, lbl_1_bss_CF22C.b);
    }
}

void fn_1_E2F1C(void)
{
    GXColor color;

    color.r = lbl_1_bss_CF22C.r;
    color.g = lbl_1_bss_CF22C.g;
    color.b = lbl_1_bss_CF22C.b;
    if (lbl_1_bss_CF22C.enabled)
    {
        g_something_with_raster_state(1);
        fn_80066520(color);
        g_something_with_fog(lbl_1_bss_CF22C.type, lbl_1_bss_CF22C.start, lbl_1_bss_CF22C.end);
    }
    else
    {
        g_something_with_raster_state(0);
    }
}

void fn_1_E2FD0(u8 width)
{
    lbl_1_data_53410.width = width;
}

void fn_1_E2FE0(int type, int src, int dst, int op)
{
    lbl_1_data_53410.blendType = type;
    lbl_1_data_53410.blendSrc = src;
    lbl_1_data_53410.blendDst = dst;
    lbl_1_data_53410.blendOp = op;
}

void fn_1_E3014(Vec *a, Vec *b, GXColor *c)
{
    GXColor color;

    fn_1_E35BC();
    color = *c;
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0xA8, 0, 2);
    GXPosition3f32(a->x, a->y, a->z);
    GXColor4u8((u8)color.r, (u8)color.g, (u8)color.b, (u8)color.a);
    GXPosition3f32(b->x, b->y, b->z);
    GXColor4u8((u8)color.r, (u8)color.g, (u8)color.b, (u8)color.a);
}

void fn_1_E3164(u16 count, Vec *points, GXColor *c)
{
    GXColor color;
    int i;
    Vec *p;

    if (count < 2)
        return;
    fn_1_E35BC();
    color = *c;
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0xB0, 0, count);
    p = points;
    for (i = count; i > 0; i--, p++)
    {
        GXPosition3f32(p->x, p->y, p->z);
        GXColor4u8((u8)color.r, (u8)color.g, (u8)color.b, (u8)color.a);
    }
}

void fn_1_E326C(struct LineVertex *a, struct LineVertex *b)
{
    fn_1_E35BC();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0xA8, 0, 2);
    GXPosition3f32(a->pos.x, a->pos.y, a->pos.z);
    GXColor4u8((u8)a->color.r, (u8)a->color.g, (u8)a->color.b, (u8)a->color.a);
    GXPosition3f32(b->pos.x, b->pos.y, b->pos.z);
    GXColor4u8((u8)b->color.r, (u8)b->color.g, (u8)b->color.b, (u8)a->color.a);
}

void fn_1_E33A8(u16 count, struct LineVertex *verts)
{
    int i;
    struct LineVertex *v;

    if (count < 2)
        return;
    fn_1_E35BC();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0xB0, 0, count);
    v = verts;
    for (i = count; i > 0; i--, v++)
    {
        GXPosition3f32(v->pos.x, v->pos.y, v->pos.z);
        GXColor4u8((u8)v->color.r, (u8)v->color.g, (u8)v->color.b, (u8)v->color.a);
    }
}

void fn_1_E349C(struct LineVertex *a, struct LineVertex *b)
{
    struct LineNode *node;
    struct OrdTblNode *entry;
    struct LineVertex *verts;

    entry = ord_tbl_get_entry_for_pos(&a->pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        verts = ord_tbl_alloc_node(sizeof(*verts) * 2);
        node->node.drawFunc = fn_1_E36A0;
        node->primitive = 0xA8;
        node->count = 2;
        node->params = lbl_1_data_53410;
        verts[0] = *a;
        verts[1] = *b;
        node->verts = verts;
        mtxa_to_mtx(node->mtx);
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_E35BC(void)
{
    GXSetLineWidth_cached(lbl_1_data_53410.width, lbl_1_data_53410.texOffset);
    g_something_with_GX_vtx_desc(0xA00);
    GXSetBlendMode_cached(lbl_1_data_53410.blendType, lbl_1_data_53410.blendSrc, lbl_1_data_53410.blendDst, lbl_1_data_53410.blendOp);
    g_something_with_raster_state(0);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(0);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(1);
}

void fn_1_E36A0(struct OrdTblNode *node_)
{
    struct LineNode *node = (struct LineNode *)node_;
    struct LineParams saved;
    int i;
    struct LineVertex *v;

    saved = lbl_1_data_53410;
    lbl_1_data_53410 = node->params;
    fn_1_E35BC();
    GXSetZMode_cached(1, 3, 1);
    GXLoadPosMtxImm(node->mtx, 0);
    GXBegin(node->primitive, 0, node->count);
    v = node->verts;
    for (i = node->count; i > 0; i--, v++)
    {
        GXPosition3f32(v->pos.x, v->pos.y, v->pos.z);
        GXColor4u8((u8)v->color.r, (u8)v->color.g, (u8)v->color.b, (u8)v->color.a);
    }
    lbl_1_data_53410 = saved;
}
