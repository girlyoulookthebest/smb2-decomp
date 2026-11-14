#ifndef AVDISP_H
#define AVDISP_H

#include "types.h"
#include "mathutil.h"
#include "bitmap.h"
#include "gx.h"

/* GMA model archives, drawn by the "avdisp" renderer. */
struct GMAModel
{
    /*0x00*/ u32 magic;  /* "GCMF" */
    /*0x04*/ u32 flags;
    /*0x08*/ Vec boundSphereCenter;
    /*0x14*/ float boundSphereRadius;
    /*0x18*/ u16 tevLayerCount;
    /*0x1A*/ u16 opaqueShapeCount;
    /*0x1C*/ u16 translucentShapeCount;
    /*0x1E*/ u8 mtxCount;
    /*0x1F*/ u8 filler1F[1];
    /*0x20*/ u32 headerSize;  /* offset of the first shape */
    /*0x24*/ GXTexObj * DPTR texObjs;
};

enum
{
    GMA_SHAPE_HAS_DLIST0 = 1 << 0,
    GMA_SHAPE_HAS_DLIST1 = 1 << 1,
    GMA_SHAPE_HAS_DLIST2 = 1 << 2,
    GMA_SHAPE_HAS_DLIST3 = 1 << 3,
};

/* Two extra display lists that may follow a shape's first two */
struct GMAExtraDispLists
{
    /*0x00*/ u8 mtxIndices[8];
    /*0x08*/ u32 dispListSizes[2];
    /*0x10*/ u8 filler10[0x20 - 0x10];
    /*0x20*/ u8 dispLists[0];
};

struct GMAShape
{
    /*0x00*/ u32 flags;
    /*0x04*/ GXColor materialColor;
    /*0x08*/ GXColor ambientColor;
    /*0x0C*/ GXColor specularColor;
    /*0x10*/ u8 filler10[1];
    /*0x11*/ u8 alpha;
    /*0x12*/ u8 tevLayerCount;
    /*0x13*/ u8 dispListFlags;
    /*0x14*/ u8 unk14;
    /*0x15*/ u8 unk15;
    /*0x16*/ u16 tevLayerIdxs[3];
    /*0x1C*/ u32 vtxAttrs;
    /*0x20*/ u8 mtxIndices[8];
    /*0x28*/ u32 dispListSizes[2];
    /*0x30*/ Vec origin;
    /*0x3C*/ u8 filler3C[4];
    /*0x40*/ u32 blendFactors;
    /*0x44*/ u8 filler44[0x60 - 0x44];
    /*0x60*/ u8 dispLists[0];
};  /* size = 0x60 */

/* Running TEV stage / texcoord / texmap allocation while a material is built */
struct TevStageInfo
{
    /*0x00*/ s32 tevStage;
    /*0x04*/ s32 texCoordId;
    /*0x08*/ s32 texMtxId;
    /*0x0C*/ s32 texMapId;
    /*0x10*/ s32 tevIndStage;
    /*0x14*/ s32 postTexMtxId;
    /*0x18*/ u32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ u32 unk20;
    /*0x24*/ s32 texCoordId2;
    /*0x28*/ s32 texMapId2;
};  /* size = 0x2C */

struct BallEnvShape
{
    /*0x00*/ u8 filler0[0x12];
    /*0x12*/ u8 unk12;
    /*0x13*/ u8 filler13[2];
    /*0x15*/ u8 unk15;
};

/* Argument passed to background ball env-map callbacks */
struct BallEnvArg
{
    /*0x00*/ s32 unk0;
    /*0x04*/ struct BallEnvShape *shape;
    /*0x08*/ void *unk8;
    /*0x0C*/ struct TevStageInfo tev;
};

struct GMAModelEntry
{
    struct GMAModel * DPTR model;
    char * DPTR name;
};

struct GMA
{
    /*0x00*/ u32 numModels;
    /*0x04*/ u8 * DPTR modelsBase;
    /*0x08*/ struct GMAModelEntry * DPTR modelEntries;
    /*0x0C*/ char * DPTR namesBase;
};

extern struct GMA *init_common_gma;

void avdisp_set_scale_factor(float scale);
void avdisp_set_alpha(float alpha);
void avdisp_draw_model_culled_sort_always(struct GMAModel *model);
void avdisp_draw_model_culled_sort_never(struct GMAModel *model);
void avdisp_draw_model_culled_sort_auto(struct GMAModel *model);
void avdisp_draw_model_unculled_sort_auto(struct GMAModel *model);
void avdisp_draw_model_unculled_sort_always(struct GMAModel *model);
void avdisp_draw_model_unculled_sort_never(struct GMAModel *model);
void avdisp_set_z_mode(int compareEnable, int compareFunc, int updateEnable);
void set_post_mult_color(float r, float g, float b, float a);
void load_gx_pos_nrm_mtx(Mtx mtx, int id);

#endif
