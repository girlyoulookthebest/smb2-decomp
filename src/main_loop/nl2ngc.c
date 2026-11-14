/* NaomiLib model renderer: loads and draws models ported from the Naomi arcade version. */
#include "types.h"
#include "macros.h"
#include "mathutil.h"
#include "gx.h"
#include "bitmap.h"
#include "file.h"
#include "light.h"
#include "frustum.h"
#include "nl2ngc.h"
#include "futsal_data.h"

extern void *currentHeap;
extern void *main_heap;

void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void OSFreeToHeap(void *heap, void *ptr);
void decompress_lz(void *src, void *dst);
unsigned long strlen(const char *s);
int strcmp(const char *a, const char *b);
void fn_1_E2FD0(int width);
void fn_1_E3014(Vec *start, Vec *end, GXColor *color);
int GXGetTexObjWrapS(GXTexObj *obj);
int GXGetTexObjWrapT(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
u8 GXGetTexObjMipMap(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp,
                     u8 doEdgeLod, int maxAniso);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);  /* ord_tbl_insert_node */
void g_something_with_GX_vtx_desc(u32 attrs);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXLoadPosMtxImm(Mtx m, int id);
void GXLoadNrmMtxImm(Mtx m, int id);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetCullMode_cached(int mode);
void g_read_something_from_prev_GX_settings(u8 *compareEnable, u32 *compareFunc, u8 *updateEnable);
void fn_80066520(GXColor color);  /* set fog color */
void g_something_with_fog(int type, float startZ, float endZ);
void g_something_with_raster_state(int a);
void GXSetChanMatColor(int chan, GXColor color);
void GXSetChanAmbColor(int chan, GXColor color);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumChans_cached(u8 n);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);

static u8 s_lzssHeader[32] __attribute__((aligned(32)));
struct Color3f s_ambientColor;
s32 s_fogEnabled;

static struct
{
    BOOL isVtxTypeA;  /* type of vertices in the current display list */
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 meshType;
    u8 unkA;
    u8 fillerB[1];
    GXTexObj *texObj;
    int texMapId;
    GXColor materialColor;
    GXColor ambientColor;
    float alpha;
    u8 unk20;
    u8 filler21[3];
    u32 unk24;
    u8 unk28;
} s_nlMaterialCache;

GXColor s_fogColor;
int s_fogType;
float s_fogStartZ;
float s_fogEndZ;
u32 s_lightMask;

#pragma explicit_zero_data on
u32 lbl_1_data_2E040 = 0;
#pragma explicit_zero_data reset

struct
{
    struct Color3f materialColor;
    float unused1;
    float unused2;
    float unused3;
    float scale;
    float prevScale;
} s_renderParams = {{1.0f, 1.0f, 1.0f}, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

static void reset_model_tev_material(void);
static void build_mesh_tev_material(struct NlMesh *pmesh);
static void reset_alpha_model_tev_material(void);

void nl2ngc_set_line_width(float width)
{
    fn_1_E2FD0(width * 6.0f);
}

void nl2ngc_draw_line(Vec *start, Vec *end, u32 color)
{
    GXColor gxColor;

    gxColor.r = (color >> 16) & 0xFF;
    gxColor.g = (color >> 8) & 0xFF;
    gxColor.b = (color >> 0) & 0xFF;
    gxColor.a = (color >> 24) & 0xFF;
    fn_1_E3014(start, end, &gxColor);
}

void nl2ngc_set_scale(float scale)
{
    s_renderParams.scale = scale;
}

void nl2ngc_set_material_color(float r, float g, float b)
{
    s_renderParams.materialColor.r = r;
    s_renderParams.materialColor.g = g;
    s_renderParams.materialColor.b = b;
}

static BOOL convert_nlobj_offsets_to_pointers(struct NlObj *obj);
static void init_model_flags(struct NlModel *model);

#ifndef __MWERKS__
void pc_swap_nlobj(void *data);
#endif

BOOL load_nlobj(struct NlObj **nlObj, struct TPL **tpl, char *nlobjPath, char *tplPath)
{
    struct NlModel * DPTR *pmodel;
    int len;
    void *compressed;
    u32 size;
    struct File file;

    /* Free the object if it's already loaded. */
    if (*nlObj != NULL)
    {
        OSFreeToHeap(currentHeap, *nlObj);
        *nlObj = NULL;
    }
    if (!file_open(nlobjPath, &file))
        return FALSE;
    len = strlen(nlobjPath);
    if (len >= 3 && strcmp(nlobjPath + (len - 3), ".lz") == 0)
    {
        u32 uncompSize;
        void *uncompressed;

        /* Read the LZSS header. */
        if (file_read(&file, s_lzssHeader, 32, 0) < 0)
            return FALSE;
        size = OSRoundUp32B(LWBRX(s_lzssHeader, 0));
        uncompSize = OSRoundUp32B(LWBRX(s_lzssHeader, 4));

        uncompressed = alloc_from_heap_or_panic(currentHeap, uncompSize, "nl2ngc.c", 460);
        compressed = alloc_from_heap_or_panic(main_heap, size, "nl2ngc.c", 461);
        if (file_read(&file, compressed, size, 0) < 0)
            return FALSE;
        if (file_close(&file) != 1)
            return FALSE;

        decompress_lz(compressed, uncompressed);
        OSFreeToHeap(main_heap, compressed);
        *nlObj = uncompressed;
        if (*nlObj == NULL)
            return FALSE;
    }
    else
    {
        size = OSRoundUp32B(file_size(&file));
        *nlObj = alloc_from_heap_or_panic(currentHeap, size, "nl2ngc.c", 475);
        file_read(&file, *nlObj, size, 0);
        file_close(&file);
    }

#ifndef __MWERKS__
    pc_swap_nlobj(*nlObj); /* PC port: NaomiLib data is big-endian on disc */
#endif
    convert_nlobj_offsets_to_pointers(*nlObj);
    if (*tpl != NULL)
        free_nl2ngc_tpl_buf_to_heap(*tpl);
    *tpl = load_bmp(tplPath);
    if (*tpl == NULL)
        return FALSE;

    pmodel = (*nlObj)->models;
    while (*pmodel != NULL)
    {
        init_nl_model_textures(*pmodel, *tpl);
        pmodel++;
    }
    return TRUE;
}

BOOL free_nlobj(struct NlObj **nlObj, struct TPL **tpl)
{
    if (*nlObj != NULL)
    {
        OSFreeToHeap(currentHeap, *nlObj);
        *nlObj = NULL;
    }
    if (*tpl != NULL)
    {
        free_nl2ngc_tpl_buf_to_heap(*tpl);
        *tpl = NULL;
    }
    return TRUE;
}

/* Turn all file offsets in the object into pointers. */
static BOOL convert_nlobj_offsets_to_pointers(struct NlObj *obj)
{
    struct NlModel * DPTR *pmodel = obj->models;
    struct NlObj_UnkChild * DPTR *unkptr;
    struct NlObj_UnkChild_Child *unkchild;

    while (*pmodel != NULL)
    {
        u32 *ptr;

        *pmodel = (void *)((u32)*pmodel + (u32)obj);

        ptr = (u32 *)*pmodel;
        ptr[-2] += (u32)obj;

        ptr = (u32 *)*pmodel;
        ptr[-1] += (u32)obj;

        init_model_flags(*pmodel);
        pmodel++;
    }

    obj->unk0 = (void *)((u32)obj->unk0 + (u32)obj);
    unkptr = obj->unk0;
    while (*unkptr != NULL)
    {
        u32 *ptr;

        *unkptr = (void *)((u32)*unkptr + (u32)obj);

        *(u32 *)*unkptr = *(u32 *)*unkptr + (u32)obj;

        ptr = (u32 *)*unkptr;
        ptr[1] += (u32)obj;

        unkchild = (*unkptr)->childStructs;
        while (unkchild->unk0 != 0)
        {
            unkchild->unk0 += (u32)obj;
            unkchild++;
        }
        unkptr++;
    }
    return TRUE;
}

static void init_model_flags(struct NlModel *model)
{
    if (model->u_valid != -1)
    {
        struct NlMesh *mesh;
        BOOL translucent = FALSE;
        BOOL opaque = FALSE;
        BOOL r8 = TRUE;
        BOOL r9 = TRUE;

        mesh = (struct NlMesh *)model->meshStart;
        while (mesh->flags != 0)
        {
            if ((((u32)mesh->flags >> 24) & 7) != 0)
            {
                translucent = TRUE;
                if (r8 && (mesh->type != -1 || mesh->type != -3))
                    r8 = FALSE;
            }
            else
            {
                opaque = TRUE;
                if (r9 && (mesh->type != -1 || mesh->type != -3))
                    r9 = FALSE;
            }
            mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
        }
        if (translucent)
            model->flags |= NL_MODEL_FLAG_TRANSLUCENT;
        if (opaque)
            model->flags |= NL_MODEL_FLAG_OPAQUE;
        if (r8)
            model->flags |= (1 << 10);
        if (r9)
            model->flags |= (1 << 11);
    }
}

void init_nl_model_textures(struct NlModel *model, struct TPL *tpl)
{
    if (model->u_valid != -1)
    {
        struct NlMesh *mesh = (struct NlMesh *)model->meshStart;

        while (mesh->flags != 0)
        {
            if (mesh->tplTexIdx >= 0)
            {
                GXTexObj *texObj = &tpl->texObjs[mesh->tplTexIdx];
                u32 flags = mesh->texFlags;
                int wrapS, wrapT;
                int minFilt, magFilt;

                if (flags & NL_TEX_FLAG_S_CLAMP)
                    wrapS = GXGetTexObjWrapS(texObj);
                else if (flags & NL_TEX_FLAG_S_MIRROR)
                    wrapS = 2;  /* GX_MIRROR */
                else
                    wrapS = 1;  /* GX_REPEAT */

                if (flags & NL_TEX_FLAG_T_CLAMP)
                    wrapT = GXGetTexObjWrapT(texObj);
                else if (flags & NL_TEX_FLAG_T_MIRROR)
                    wrapT = 2;
                else
                    wrapT = 1;

                GXInitTexObj(texObj,
                             GXGetTexObjData(texObj),
                             GXGetTexObjWidth(texObj),
                             GXGetTexObjHeight(texObj),
                             GXGetTexObjFmt(texObj),
                             wrapS,
                             wrapT,
                             tpl->texHeaders[mesh->tplTexIdx].unkC);

                switch ((flags >> 13) & 3)
                {
                case 0:
                    if (GXGetTexObjMipMap(texObj))
                        minFilt = 3;  /* GX_LIN_MIP_NEAR */
                    else
                        minFilt = 0;  /* GX_NEAR */
                    magFilt = 0;
                    break;
                default:
                    if (GXGetTexObjMipMap(texObj))
                        minFilt = 5;  /* GX_LIN_MIP_LIN */
                    else
                        minFilt = 1;  /* GX_LINEAR */
                    magFilt = 1;
                    break;
                }

                GXInitTexObjLOD(texObj, minFilt, magFilt, 0.0f, 0.0f, 0.0f, 0, 0, 0);
                mesh->texObj = texObj;
            }
            mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
        }
    }
}

struct DrawModelDeferredNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct NlModel *model;
    /*0x0C*/ Mtx viewFromModel;
    /*0x3C*/ struct Color3f materialColor;
    /*0x48*/ u32 lightGroup;
    /*0x4C*/ struct Color3f ambientColor;
    /*0x58*/ u32 fogEnabled;
};

static void draw_model_node_callback(struct DrawModelDeferredNode *node);

/* Draw opaque meshes immediately and depth-sort translucent meshes. */
void nl2ngc_draw_model_sort_translucent(struct NlModel *model)
{
    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            /* A non-one scale is always reset before the next draw call. */
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius,
                                           s_renderParams.scale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
            s_renderParams.scale = 1.0f;
        }
        if (model->flags & NL_MODEL_FLAG_OPAQUE)
            nl2ngc_draw_opaque_model_meshes(model);
        if (model->flags & NL_MODEL_FLAG_TRANSLUCENT)
        {
            struct DrawModelDeferredNode *drawNode;
            struct OrdTblNode *list = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);

            drawNode = ord_tbl_alloc_node(sizeof(*drawNode));
            if (drawNode != NULL)
            {
                drawNode->node.drawFunc = (void *)draw_model_node_callback;
                drawNode->model = model;
                drawNode->materialColor.r = s_renderParams.materialColor.r;
                drawNode->materialColor.g = s_renderParams.materialColor.g;
                drawNode->materialColor.b = s_renderParams.materialColor.b;
                drawNode->lightGroup = get_active_light_group();
                drawNode->ambientColor.r = s_ambientColor.r;
                drawNode->ambientColor.g = s_ambientColor.g;
                drawNode->ambientColor.b = s_ambientColor.b;
                drawNode->fogEnabled = s_fogEnabled;
                mtxa_to_mtx(drawNode->viewFromModel);
                fn_1_CD418(list, &drawNode->node);
            }
        }
    }
}

Mtx textureMatrix =
{
    {1, 0, 0, 0},
    {0, -1, 0, 1},
    {0, 0, 1, 0},
};

/* Draw opaque and translucent meshes immediately, without depth sorting. */
void nl2ngc_draw_model_sort_none(struct NlModel *model)
{
    struct NlMesh *mesh;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius,
                                           s_renderParams.scale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
        }
        if (model->flags & NL_MODEL_FLAG_VTX_TYPE_A)
        {
            g_something_with_GX_vtx_desc((1 << 9) | (1 << 11) | (1 << 13));
            s_nlMaterialCache.isVtxTypeA = TRUE;
        }
        else
        {
            g_something_with_GX_vtx_desc((1 << 9) | (1 << 10) | (1 << 13));
            s_nlMaterialCache.isVtxTypeA = FALSE;
        }

        reset_model_tev_material();
        GXLoadTexMtxImm(textureMatrix, 30, 1);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);

        mesh = (struct NlMesh *)model->meshStart;
        while (mesh->flags != 0)
        {
            struct NlDispList *dlstart;
            struct NlMesh *next;

            build_mesh_tev_material(mesh);
            next = (void *)(mesh->dispListStart + mesh->dispListSize);
            dlstart = (void *)mesh->dispListStart;
            switch (mesh->type)
            {
            case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                break;
            case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                draw_nl_disp_list_type_a(dlstart, next);
                break;
            default:
                u_draw_nl_disp_list_type_b_1(dlstart, next);
                break;
            }
            mesh = next;
        }
        unk_empty();
    }
}

/* Same as DrawModelDeferredNode but with an alpha parameter. */
struct DrawAlphaModelDeferredNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct NlModel *model;
    /*0x0C*/ Mtx viewFromModel;
    /*0x3C*/ struct Color3f materialColor;
    /*0x48*/ float alpha;
    /*0x4C*/ u32 lightGroup;
    /*0x50*/ struct Color3f ambientColor;
    /*0x5C*/ u32 fogEnabled;
};

static void draw_alpha_model_node_callback(struct DrawAlphaModelDeferredNode *node);

/* Draw the model with a global alpha; all meshes are depth-sorted. */
void nl2ngc_draw_model_alpha_sort_all(struct NlModel *model, float alpha)
{
    struct DrawAlphaModelDeferredNode *node;
    struct OrdTblNode *entry;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius,
                                           s_renderParams.scale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
            s_renderParams.scale = 1.0f;
        }

        entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = (void *)draw_alpha_model_node_callback;
            node->model = model;
            node->alpha = alpha;
            node->materialColor.r = s_renderParams.materialColor.r;
            node->materialColor.g = s_renderParams.materialColor.g;
            node->materialColor.b = s_renderParams.materialColor.b;
            node->lightGroup = get_active_light_group();
            node->ambientColor.r = s_ambientColor.r;
            node->ambientColor.g = s_ambientColor.g;
            node->ambientColor.b = s_ambientColor.b;
            node->fogEnabled = s_fogEnabled;
            mtxa_to_mtx(node->viewFromModel);
            fn_1_CD418(entry, &node->node);
        }
    }
}

/* Draw the model with a global alpha; no meshes are depth-sorted. */
void nl2ngc_draw_model_alpha_sort_none(struct NlModel *model, float alpha)
{
    struct NlMesh *mesh;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius,
                                           s_renderParams.scale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
        }
        if (model->flags & NL_MODEL_FLAG_VTX_TYPE_A)
        {
            g_something_with_GX_vtx_desc((1 << 9) | (1 << 11) | (1 << 13));
            s_nlMaterialCache.isVtxTypeA = TRUE;
        }
        else
        {
            g_something_with_GX_vtx_desc((1 << 9) | (1 << 10) | (1 << 13));
            s_nlMaterialCache.isVtxTypeA = FALSE;
        }

        s_nlMaterialCache.alpha = alpha;
        reset_alpha_model_tev_material();
        GXLoadTexMtxImm(textureMatrix, 30, 1);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);

        mesh = (struct NlMesh *)model->meshStart;
        while (mesh->flags != 0)
        {
            struct NlDispList *dlstart;
            struct NlMesh *next;

            build_alpha_mesh_tev_material(mesh);
            next = (void *)(mesh->dispListStart + mesh->dispListSize);
            dlstart = (void *)mesh->dispListStart;
            switch (mesh->type)
            {
            case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                break;
            case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                draw_nl_disp_list_type_a_alpha(dlstart, next);
                break;
            default:
                u_draw_nl_disp_list_type_b_1(dlstart, next);
                break;
            }
            mesh = next;
        }
        unk_empty();
    }
}

void nl2ngc_draw_model_sort_translucent_alt(struct NlModel *model)
{
    nl2ngc_draw_model_sort_translucent(model);
}

/* Unused vertex attribute tables. */
u32 lbl_1_data_2E0A4[] = {9, 1, 10, 1, 13, 1, 255, 1};
u32 lbl_1_data_2E0C4[] = {9, 1, 4, 0, 10, 0, 4, 0, 13, 1, 4, 0, 255, 1, 4, 0};
u32 lbl_1_data_2E104[] = {9, 1, 11, 1, 13, 1, 255, 1};
u32 lbl_1_data_2E124[] = {9, 1, 4, 0, 11, 1, 5, 0, 13, 1, 4, 0, 255, 1, 4, 0};

void *lbl_1_data_2E164[] =
{
    lbl_1_data_2E0A4,
    lbl_1_data_2E0C4,
    lbl_1_data_2E104,
    lbl_1_data_2E124,
};

int s_nlToGXSrcBlend[] = {0, 1, 2, 3, 4, 5, 6, 7};
int s_nlToGXDstBlend[] = {0, 1, 2, 3, 4, 5, 6, 7};
int s_nlToGXCompare[] = {0, 6, 2, 6, 3, 5, 3, 7};
int s_nlToGXCullModes[] = {3, 0, 2, 1, 0};

static void reset_model_tev_material(void)
{
    GXColor ambColor;

    s_nlMaterialCache.unk4 = 0;
    s_nlMaterialCache.unk5 = 1;
    s_nlMaterialCache.unk6 = 0;
    GXSetBlendMode_cached(0, s_nlToGXSrcBlend[1], s_nlToGXDstBlend[0], 0);
    g_read_something_from_prev_GX_settings(&s_nlMaterialCache.unk20, &s_nlMaterialCache.unk24,
                                           &s_nlMaterialCache.unk28);
    s_nlMaterialCache.unk7 = 4;
    s_nlMaterialCache.unk8 = 0;

    GXSetZMode_cached(1, s_nlToGXCompare[s_nlMaterialCache.unk7], !s_nlMaterialCache.unk8);

    if (s_fogEnabled != 0)
    {
        fn_80066520(s_fogColor);
        g_something_with_fog(s_fogType, s_fogStartZ, s_fogEndZ);
        g_something_with_raster_state(1);
    }
    else
        g_something_with_raster_state(0);

    s_nlMaterialCache.unkA = 2;
    GXSetCullMode_cached(s_nlToGXCullModes[2]);
    s_nlMaterialCache.texObj = 0;
    s_nlMaterialCache.texMapId = 0;

    s_nlMaterialCache.materialColor.r = s_renderParams.materialColor.r * 255.0f;
    s_nlMaterialCache.materialColor.g = s_renderParams.materialColor.g * 255.0f;
    s_nlMaterialCache.materialColor.b = s_renderParams.materialColor.b * 255.0f;
    s_nlMaterialCache.materialColor.a = 255;
    GXSetChanMatColor(4, s_nlMaterialCache.materialColor);

    ambColor.r = s_nlMaterialCache.ambientColor.r = 0;
    ambColor.g = s_nlMaterialCache.ambientColor.g = 0;
    ambColor.b = s_nlMaterialCache.ambientColor.b = 0;
    ambColor.a = s_nlMaterialCache.ambientColor.a = s_nlMaterialCache.materialColor.a;
    GXSetChanAmbColor(4, ambColor);

    s_nlMaterialCache.meshType = 0;
    opti_GXSetChanCtrl(4, 1, 0, 0, s_lightMask, 2, 1);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(1);
    GXSetTexCoordGen2_cached(0, 1, 4, 30, 0, 125);
}

static void build_mesh_tev_material(struct NlMesh *pmesh)
{
    struct NlMesh mesh = *pmesh;
    GXColor color;
    u32 r29;
    u32 r28;

    switch ((mesh.flags >> 24) & 7)
    {
    case 0:
        if (s_nlMaterialCache.unk4 != 0)
        {
            GXSetBlendMode_cached(0, 1, 0, 0);
            s_nlMaterialCache.unk4 = 0;
            s_nlMaterialCache.unk5 = 1;
            s_nlMaterialCache.unk6 = 0;
        }
        break;
    default:
        r28 = mesh.texFlags >> 29;
        r29 = (mesh.texFlags >> 26) & 7;
        if (s_nlMaterialCache.unk4 != 2 || s_nlMaterialCache.unk5 != r28 || s_nlMaterialCache.unk6 != r29)
        {
            GXSetBlendMode_cached(1, s_nlToGXSrcBlend[r28], s_nlToGXDstBlend[r29], 0);
            s_nlMaterialCache.unk4 = 2;
            s_nlMaterialCache.unk5 = r28;
            s_nlMaterialCache.unk6 = r29;
        }
        break;
    }

    r28 = mesh.unk4 >> 29;
    r29 = mesh.unk4 & 0x4000000;
    if (s_nlMaterialCache.unk7 != r28 || s_nlMaterialCache.unk8 != r29)
    {
        GXSetZMode_cached(1, s_nlToGXCompare[r28], !r29);
        s_nlMaterialCache.unk7 = r28;
        s_nlMaterialCache.unk8 = r29;
    }

    if (s_fogEnabled != 0)
    {
        fn_80066520(s_fogColor);
        g_something_with_fog(s_fogType, s_fogStartZ, s_fogEndZ);
        g_something_with_raster_state(1);
    }
    else
        g_something_with_raster_state(0);

    if (mesh.tplTexIdx < 0)
    {
        g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);  /* GX_PASSCLR */
    }
    else
    {
        int texMapId = s_nlMaterialCache.texMapId;

        if (s_nlMaterialCache.texObj != mesh.texObj)
        {
            s_nlMaterialCache.texObj = mesh.texObj;
            if (--texMapId < 0)
                texMapId = 7;
            s_nlMaterialCache.texMapId = texMapId;
            GXLoadTexObj_cached(mesh.texObj, texMapId);
        }
        g_GXSetTevOrder_wrapper(0, 0, texMapId, 4);
        switch ((mesh.texFlags >> 6) & 3)
        {
        case 0:
            g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 3);  /* GX_REPLACE */
            break;
        case 1:
            GXSetTevColorIn_cached(0, 15, 10, 8, 15);
            GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn_cached(0, 7, 7, 7, 4);
            GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
            break;
        case 2:
            GXSetTevColorIn_cached(0, 10, 8, 9, 15);
            GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn_cached(0, 7, 7, 7, 5);
            GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
            break;
        case 3:
            GXSetTevColorIn_cached(0, 15, 10, 8, 15);
            GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
            GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
            break;
        }
    }

    color.r = mesh.sub2C.materialColorR * s_renderParams.materialColor.r * 255.0f;
    color.g = mesh.sub2C.materialColorG * s_renderParams.materialColor.g * 255.0f;
    color.b = mesh.sub2C.materialColorB * s_renderParams.materialColor.b * 255.0f;
    color.a = mesh.sub2C.materialColorA * 255.0f;
    if (s_nlMaterialCache.materialColor.r != color.r || s_nlMaterialCache.materialColor.g != color.g
     || s_nlMaterialCache.materialColor.b != color.b || s_nlMaterialCache.materialColor.a != color.a)
    {
        GXSetChanMatColor(4, color);
        s_nlMaterialCache.materialColor = color;
    }

    color.r = mesh.ambientColorScale * s_ambientColor.r * 255.0f;
    color.g = mesh.ambientColorScale * s_ambientColor.g * 255.0f;
    color.b = mesh.ambientColorScale * s_ambientColor.b * 255.0f;
    color.a = s_nlMaterialCache.materialColor.a;
    if (s_nlMaterialCache.ambientColor.r != color.r || s_nlMaterialCache.ambientColor.g != color.g
     || s_nlMaterialCache.ambientColor.b != color.b || s_nlMaterialCache.ambientColor.a != color.a)
    {
        GXSetChanAmbColor(4, color);
        s_nlMaterialCache.ambientColor = color;
    }

    if (s_nlMaterialCache.meshType != mesh.type)
    {
        s_nlMaterialCache.meshType = mesh.type;
        switch (mesh.type)
        {
        case NL_MODEL_TYPE_UNLIT_CONST_MAT_COLOR:
            opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 1);
            break;
        case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
            opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
            break;
        case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
        default:
            opti_GXSetChanCtrl(4, 1, 0, 0, s_lightMask, 2, 1);
            break;
        }
    }
}

static inline void write_vtx_type_b(struct NlVtxTypeB *vtx)
{
    GXPosition3f32(vtx->x, vtx->y, vtx->z);
    GXNormal3f32(vtx->nx, vtx->ny, vtx->nz);
    GXTexCoord2f32(vtx->s, vtx->t);
}

void u_draw_nl_disp_list_type_b_1(struct NlDispList *dl, void *end)
{
    g_something_with_GX_vtx_desc((1 << 9) | (1 << 10) | (1 << 13));

    if (s_nlMaterialCache.isVtxTypeA != 0)
        s_nlMaterialCache.isVtxTypeA = FALSE;

    while (dl < (struct NlDispList *)end)
    {
        int faceCount = dl->faceCount;
        int i;
        struct NlVtxTypeB *vtx;
        u8 *vtxData = dl->vtxData;
        u8 r4 = dl->flags & 3;

        if (s_nlMaterialCache.unkA != r4)
        {
            s_nlMaterialCache.unkA = r4;
            GXSetCullMode_cached(s_nlToGXCullModes[r4]);
        }

        if (dl->flags & NL_DLIST_FLAG_TRIANGLESTRIP)
        {
            GXBegin(0x98, 0, faceCount);
            while (faceCount > 0)
            {
                if (*(u32 *)vtxData & 1)
                {
                    write_vtx_type_b((void *)vtxData);
                    vtxData += 32;
                }
                else
                {
                    vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    write_vtx_type_b(vtx);
                    vtxData += 8;
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & NL_DLIST_FLAG_TRIANGLES)
        {
            GXBegin(0x90, 0, faceCount * 3);
            while (faceCount > 0)
            {
                for (i = 3; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        write_vtx_type_b((void *)vtxData);
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    write_vtx_type_b(vtx);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & NL_DLIST_FLAG_QUADS)
        {
            GXBegin(0x80, 0, faceCount * 4);
            while (faceCount > 0)
            {
                for (i = 4; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        write_vtx_type_b((void *)vtxData);
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    write_vtx_type_b(vtx);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        dl = (void *)vtxData;
    }
}

static inline void color4u8_x(u8 r, u8 g, u8 b, u32 a)
{
#ifndef __MWERKS__
    pc_gx_color4u8(r, g, b, (u8)(a >> 24));
#else
    GXWGFifo.u8 = r;
    GXWGFifo.u8 = g;
    GXWGFifo.u8 = b;
    GXWGFifo.u8 = (u8)(a >> 24);
#endif
}

static inline void write_vtx_type_a(struct NlVtxTypeA *vtx)
{
    u32 color;

    GXPosition3f32(vtx->x, vtx->y, vtx->z);
    color = vtx->color;
    color4u8_x(((color >> 16) & 0xFF) * s_renderParams.materialColor.r,
               ((color >> 8) & 0xFF) * s_renderParams.materialColor.g,
               ((color >> 0) & 0xFF) * s_renderParams.materialColor.b,
               color);
    GXTexCoord2f32(vtx->s, vtx->t);
}

void draw_nl_disp_list_type_a(struct NlDispList *dl, void *end)
{
    g_something_with_GX_vtx_desc((1 << 9) | (1 << 11) | (1 << 13));

    if (s_nlMaterialCache.isVtxTypeA != 1)
        s_nlMaterialCache.isVtxTypeA = TRUE;

    while (dl < (struct NlDispList *)end)
    {
        int faceCount = dl->faceCount;
        int i;
        struct NlVtxTypeA *vtx;
        u8 *vtxData = dl->vtxData;
        u8 r4 = dl->flags & 3;

        if (s_nlMaterialCache.unkA != r4)
        {
            s_nlMaterialCache.unkA = r4;
            GXSetCullMode_cached(s_nlToGXCullModes[r4]);
        }

        if (dl->flags & NL_DLIST_FLAG_TRIANGLESTRIP)
        {
            GXBegin(0x98, 0, faceCount);
            while (faceCount > 0)
            {
                if (*(u32 *)vtxData & 1)
                {
                    write_vtx_type_a((void *)vtxData);
                    vtxData += 32;
                }
                else
                {
                    vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    write_vtx_type_a(vtx);
                    vtxData += 8;
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & NL_DLIST_FLAG_TRIANGLES)
        {
            GXBegin(0x90, 0, faceCount * 3);
            while (faceCount > 0)
            {
                for (i = 3; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        write_vtx_type_a((void *)vtxData);
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    write_vtx_type_a(vtx);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & NL_DLIST_FLAG_QUADS)
        {
            GXBegin(0x80, 0, faceCount * 4);
            while (faceCount > 0)
            {
                for (i = 4; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        write_vtx_type_a((void *)vtxData);
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    write_vtx_type_a(vtx);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        dl = (void *)vtxData;
    }
}

static void reset_alpha_model_tev_material(void)
{
    GXColor ambColor;

    s_nlMaterialCache.unk4 = 0;
    s_nlMaterialCache.unk5 = 4;
    s_nlMaterialCache.unk6 = 5;
    GXSetBlendMode_cached(1, s_nlToGXSrcBlend[4], s_nlToGXDstBlend[5], 0);
    //! passes unk20 twice
    g_read_something_from_prev_GX_settings(&s_nlMaterialCache.unk20, (u32 *)&s_nlMaterialCache.unk20,
                                           &s_nlMaterialCache.unk28);
    s_nlMaterialCache.unk7 = 4;
    s_nlMaterialCache.unk8 = 0;

    GXSetZMode_cached(1, s_nlToGXCompare[s_nlMaterialCache.unk7], !s_nlMaterialCache.unk8);

    if (s_fogEnabled != 0)
    {
        fn_80066520(s_fogColor);
        g_something_with_fog(s_fogType, s_fogStartZ, s_fogEndZ);
        g_something_with_raster_state(1);
    }
    else
        g_something_with_raster_state(0);

    s_nlMaterialCache.unkA = 2;
    GXSetCullMode_cached(s_nlToGXCullModes[2]);
    s_nlMaterialCache.texObj = 0;
    s_nlMaterialCache.texMapId = 0;

    s_nlMaterialCache.materialColor.r = s_renderParams.materialColor.r * 255.0f;
    s_nlMaterialCache.materialColor.g = s_renderParams.materialColor.g * 255.0f;
    s_nlMaterialCache.materialColor.b = s_renderParams.materialColor.b * 255.0f;
    s_nlMaterialCache.materialColor.a = s_nlMaterialCache.alpha * 255.0f;
    GXSetChanMatColor(4, s_nlMaterialCache.materialColor);

    ambColor.r = s_nlMaterialCache.ambientColor.r = 0;
    ambColor.g = s_nlMaterialCache.ambientColor.g = 0;
    ambColor.b = s_nlMaterialCache.ambientColor.b = 0;
    ambColor.a = s_nlMaterialCache.ambientColor.a = s_nlMaterialCache.materialColor.a;
    GXSetChanAmbColor(4, ambColor);

    s_nlMaterialCache.meshType = 0;
    opti_GXSetChanCtrl(4, 1, 0, 0, s_lightMask, 2, 1);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(1);
    GXSetTexCoordGen2_cached(0, 1, 4, 30, 0, 125);
}

void build_alpha_mesh_tev_material(struct NlMesh *pmesh)
{
    struct NlMesh mesh = *pmesh;
    GXColor color;
    u32 r29;
    u32 r28;

    switch ((mesh.flags >> 24) & 7)
    {
    case 0:
        if (s_nlMaterialCache.unk4 != 0)
        {
            GXSetBlendMode_cached(1, 4, 5, 0);
            s_nlMaterialCache.unk4 = 0;
            s_nlMaterialCache.unk5 = 4;
            s_nlMaterialCache.unk6 = 5;
        }
        break;
    default:
        r28 = mesh.texFlags >> 29;
        r29 = (mesh.texFlags >> 26) & 7;
        if (s_nlMaterialCache.unk4 != 2 || s_nlMaterialCache.unk5 != r28 || s_nlMaterialCache.unk6 != r29)
        {
            GXSetBlendMode_cached(1, s_nlToGXSrcBlend[r28], s_nlToGXDstBlend[r29], 0);
            s_nlMaterialCache.unk4 = 2;
            s_nlMaterialCache.unk5 = r28;
            s_nlMaterialCache.unk6 = r29;
        }
        break;
    }

    r28 = mesh.unk4 >> 29;
    r29 = mesh.unk4 & 0x4000000;
    if (s_nlMaterialCache.unk7 != r28 || s_nlMaterialCache.unk8 != r29)
    {
        GXSetZMode_cached(1, s_nlToGXCompare[r28], !r29);
        s_nlMaterialCache.unk7 = r28;
        s_nlMaterialCache.unk8 = r29;
    }

    if (s_fogEnabled != 0)
    {
        fn_80066520(s_fogColor);
        g_something_with_fog(s_fogType, s_fogStartZ, s_fogEndZ);
        g_something_with_raster_state(1);
    }
    else
        g_something_with_raster_state(0);

    if (mesh.tplTexIdx < 0)
    {
        g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);  /* GX_PASSCLR */
    }
    else
    {
        int texMapId = s_nlMaterialCache.texMapId;

        if (s_nlMaterialCache.texObj != mesh.texObj)
        {
            s_nlMaterialCache.texObj = mesh.texObj;
            if (--texMapId < 0)
                texMapId = 7;
            s_nlMaterialCache.texMapId = texMapId;
            GXLoadTexObj_cached(mesh.texObj, texMapId);
        }
        g_GXSetTevOrder_wrapper(0, 0, texMapId, 4);
        switch ((mesh.texFlags >> 6) & 3)
        {
        case 0:
            GXSetTevColorIn_cached(0, 15, 15, 15, 8);
            GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn_cached(0, 7, 4, 5, 7);
            GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
            break;
        case 1:
            GXSetTevColorIn_cached(0, 15, 10, 8, 15);
            GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn_cached(0, 7, 4, 5, 7);
            GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
            break;
        case 2:
            GXSetTevColorIn_cached(0, 10, 8, 9, 15);
            GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn_cached(0, 7, 7, 7, 5);
            GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
            break;
        case 3:
            GXSetTevColorIn_cached(0, 15, 10, 8, 15);
            GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
            GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
            break;
        }
    }

    color.r = mesh.sub2C.materialColorR * s_renderParams.materialColor.r * 255.0f;
    color.g = mesh.sub2C.materialColorG * s_renderParams.materialColor.g * 255.0f;
    color.b = mesh.sub2C.materialColorB * s_renderParams.materialColor.b * 255.0f;
    color.a = mesh.sub2C.materialColorA * s_nlMaterialCache.alpha * 255.0f;
    if (s_nlMaterialCache.materialColor.r != color.r || s_nlMaterialCache.materialColor.g != color.g
     || s_nlMaterialCache.materialColor.b != color.b || s_nlMaterialCache.materialColor.a != color.a)
    {
        GXSetChanMatColor(4, color);
        s_nlMaterialCache.materialColor = color;
    }

    color.r = mesh.ambientColorScale * s_ambientColor.r * 255.0f;
    color.g = mesh.ambientColorScale * s_ambientColor.g * 255.0f;
    color.b = mesh.ambientColorScale * s_ambientColor.b * 255.0f;
    color.a = s_nlMaterialCache.materialColor.a;
    if (s_nlMaterialCache.ambientColor.r != color.r || s_nlMaterialCache.ambientColor.g != color.g
     || s_nlMaterialCache.ambientColor.b != color.b || s_nlMaterialCache.ambientColor.a != color.a)
    {
        GXSetChanAmbColor(4, color);
        s_nlMaterialCache.ambientColor = color;
    }

    if (s_nlMaterialCache.meshType != mesh.type)
    {
        s_nlMaterialCache.meshType = mesh.type;
        switch (mesh.type)
        {
        case NL_MODEL_TYPE_UNLIT_CONST_MAT_COLOR:
            opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 1);
            break;
        case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
            opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
            break;
        case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
        default:
            opti_GXSetChanCtrl(4, 1, 0, 0, s_lightMask, 2, 1);
            break;
        }
    }
}

#define WRITE_VTX_TYPE_A_WITH_ALPHA(v) \
    { \
        GXPosition3f32((v)->x, (v)->y, (v)->z); \
        color = (v)->color; \
        GXColor4u8(((color >> 16) & 0xFF) * s_renderParams.materialColor.r, \
                   ((color >> 8) & 0xFF) * s_renderParams.materialColor.g, \
                   ((color >> 0) & 0xFF) * s_renderParams.materialColor.b, \
                   (color >> 24) * s_nlMaterialCache.alpha); \
        GXTexCoord2f32((v)->s, (v)->t); \
    }

void draw_nl_disp_list_type_a_alpha(struct NlDispList *dl, void *end)
{
    g_something_with_GX_vtx_desc((1 << 9) | (1 << 11) | (1 << 13));

    if (s_nlMaterialCache.isVtxTypeA != 1)
        s_nlMaterialCache.isVtxTypeA = TRUE;

    while (dl < (struct NlDispList *)end)
    {
        int faceCount = dl->faceCount;
        int i;
        u32 color;
        struct NlVtxTypeA *vtx;
        u8 *vtxData = dl->vtxData;
        u8 r4 = dl->flags & 3;

        if (s_nlMaterialCache.unkA != r4)
        {
            s_nlMaterialCache.unkA = r4;
            GXSetCullMode_cached(s_nlToGXCullModes[r4]);
        }

        if (dl->flags & NL_DLIST_FLAG_TRIANGLESTRIP)
        {
            GXBegin(0x98, 0, faceCount);
            while (faceCount > 0)
            {
                if (*(u32 *)vtxData & 1)
                {
                    WRITE_VTX_TYPE_A_WITH_ALPHA((struct NlVtxTypeA *)vtxData)
                    vtxData += 32;
                }
                else
                {
                    vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    WRITE_VTX_TYPE_A_WITH_ALPHA(vtx)
                    vtxData += 8;
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & NL_DLIST_FLAG_TRIANGLES)
        {
            GXBegin(0x90, 0, faceCount * 3);
            while (faceCount > 0)
            {
                for (i = 3; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        WRITE_VTX_TYPE_A_WITH_ALPHA((struct NlVtxTypeA *)vtxData)
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    WRITE_VTX_TYPE_A_WITH_ALPHA(vtx)
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & NL_DLIST_FLAG_QUADS)
        {
            GXBegin(0x80, 0, faceCount * 4);
            while (faceCount > 0)
            {
                for (i = 4; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        WRITE_VTX_TYPE_A_WITH_ALPHA((struct NlVtxTypeA *)vtxData)
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(s32 *)(vtxData + 4) + 8);
                    WRITE_VTX_TYPE_A_WITH_ALPHA(vtx)
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        dl = (void *)vtxData;
    }
}

void nl2ngc_set_light_mask(u32 lightMask)
{
    s_lightMask = lightMask;
}

void nl2ngc_set_ambient(float r, float g, float b)
{
    s_ambientColor.r = r;
    s_ambientColor.g = g;
    s_ambientColor.b = b;
}

void nl2ngc_enable_fog(int enabled)
{
    s_fogEnabled = enabled;
}

void nl2ngc_set_fog_params(int type, float startZ, float endZ)
{
    s_fogType = type;
    s_fogStartZ = startZ;
    s_fogEndZ = endZ;
}

void nl2ngc_set_fog_color(u8 r, u8 g, u8 b)
{
    s_fogColor.r = r;
    s_fogColor.g = g;
    s_fogColor.b = b;
}

static inline void set_model_vtx_desc(void *p)
{
    struct NlModel *model = p;
    if (model->flags & NL_MODEL_FLAG_VTX_TYPE_A)
    {
        g_something_with_GX_vtx_desc((1 << 9) | (1 << 11) | (1 << 13));
        s_nlMaterialCache.isVtxTypeA = TRUE;
    }
    else
    {
        g_something_with_GX_vtx_desc((1 << 9) | (1 << 10) | (1 << 13));
        s_nlMaterialCache.isVtxTypeA = FALSE;
    }
}

void nl2ngc_draw_opaque_model_meshes(struct NlModel *model)
{
    struct NlMesh *mesh;

    set_model_vtx_desc(model);

    reset_model_tev_material();
    GXLoadTexMtxImm(textureMatrix, 30, 1);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);

    mesh = (struct NlMesh *)model->meshStart;
    while (mesh->flags != 0)
    {
        struct NlDispList *dlstart;
        struct NlMesh *next;

        build_mesh_tev_material(mesh);
        next = (void *)(mesh->dispListStart + mesh->dispListSize);
        if ((((u32)mesh->flags >> 24) & 7) != 0)
            mesh = next;
        else
        {
            dlstart = (void *)mesh->dispListStart;
            switch (mesh->type)
            {
            case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                break;
            case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                draw_nl_disp_list_type_a(dlstart, next);
                break;
            default:
                u_draw_nl_disp_list_type_b_1(dlstart, next);
                break;
            }
            mesh = next;
        }
    }
    unk_empty();
}

static void draw_model_node_callback(struct DrawModelDeferredNode *node)
{
    float r, g, b;

    mtxa_from_mtx(node->viewFromModel);

    r = s_renderParams.materialColor.r;
    g = s_renderParams.materialColor.g;
    b = s_renderParams.materialColor.b;

    s_renderParams.materialColor.r = node->materialColor.r;
    s_renderParams.materialColor.g = node->materialColor.g;
    s_renderParams.materialColor.b = node->materialColor.b;
    if (!(node->model->flags & (1 << 10)))
    {
        load_light_group_cached(node->lightGroup);
        nl2ngc_set_ambient(node->ambientColor.r, node->ambientColor.g, node->ambientColor.b);
    }
    s_fogEnabled = node->fogEnabled;
    nl2ngc_draw_translucent_model_meshes(node->model);

    s_renderParams.materialColor.r = r;
    s_renderParams.materialColor.g = g;
    s_renderParams.materialColor.b = b;
}

void nl2ngc_draw_translucent_model_meshes(struct NlModel *model)
{
    struct NlMesh *mesh;

    set_model_vtx_desc(model);

    reset_model_tev_material();
    GXLoadTexMtxImm(textureMatrix, 30, 1);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);

    mesh = (struct NlMesh *)model->meshStart;
    while (mesh->flags != 0)
    {
        struct NlDispList *dlstart;
        struct NlMesh *next;

        build_mesh_tev_material(mesh);
        next = (void *)(mesh->dispListStart + mesh->dispListSize);
        if ((((u32)mesh->flags >> 24) & 7) == 0)
            mesh = next;
        else
        {
            dlstart = (void *)mesh->dispListStart;
            switch (mesh->type)
            {
            case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                break;
            case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                draw_nl_disp_list_type_a(dlstart, next);
                break;
            default:
                u_draw_nl_disp_list_type_b_1(dlstart, next);
                break;
            }
            mesh = next;
        }
    }
    unk_empty();
}

static void draw_alpha_model_node_callback(struct DrawAlphaModelDeferredNode *node)
{
    float r, g, b;

    mtxa_from_mtx(node->viewFromModel);

    r = s_renderParams.materialColor.r;
    g = s_renderParams.materialColor.g;
    b = s_renderParams.materialColor.b;

    s_renderParams.materialColor.r = node->materialColor.r;
    s_renderParams.materialColor.g = node->materialColor.g;
    s_renderParams.materialColor.b = node->materialColor.b;
    s_nlMaterialCache.alpha = node->alpha;
    if (!(node->model->flags & (1 << 10)))
    {
        load_light_group_cached(node->lightGroup);
        nl2ngc_set_ambient(node->ambientColor.r, node->ambientColor.g, node->ambientColor.b);
    }
    s_fogEnabled = node->fogEnabled;
    nl2ngc_draw_all_model_meshes_alpha(node->model);

    s_renderParams.materialColor.r = r;
    s_renderParams.materialColor.g = g;
    s_renderParams.materialColor.b = b;
}

void nl2ngc_draw_all_model_meshes_alpha(struct NlModel *model)
{
    struct NlMesh *mesh;
    struct NlModel *m = model;

    if (m->u_valid == -1)
        return;

    set_model_vtx_desc(m);

    reset_alpha_model_tev_material();
    GXLoadTexMtxImm(textureMatrix, 30, 1);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);

    mesh = (struct NlMesh *)model->meshStart;
    while (mesh->flags != 0)
    {
        struct NlDispList *dlstart;
        struct NlMesh *next;

        build_alpha_mesh_tev_material(mesh);
        next = (void *)(mesh->dispListStart + mesh->dispListSize);
        dlstart = (void *)mesh->dispListStart;
        switch (mesh->type)
        {
        case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
            break;
        case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
            draw_nl_disp_list_type_a_alpha(dlstart, next);
            break;
        default:
            u_draw_nl_disp_list_type_b_1(dlstart, next);
            break;
        }
        mesh = next;
    }
    unk_empty();
}

void u_nl2ngc_draw_model_with_mesh_func(struct NlModel *model, int (*func)(struct NlMesh *))
{
    struct NlMesh *mesh;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius,
                                           s_renderParams.prevScale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
        }
        if (model->flags & NL_MODEL_FLAG_VTX_TYPE_A)
        {
            g_something_with_GX_vtx_desc((1 << 9) | (1 << 11) | (1 << 13));
            s_nlMaterialCache.isVtxTypeA = TRUE;
        }
        else
        {
            g_something_with_GX_vtx_desc((1 << 9) | (1 << 10) | (1 << 13));
            s_nlMaterialCache.isVtxTypeA = FALSE;
        }

        s_nlMaterialCache.alpha = 1.0f;
        mesh = (struct NlMesh *)model->meshStart;
        while (mesh->flags != 0)
        {
            struct NlDispList *dlstart;
            struct NlMesh *next = (void *)(mesh->dispListStart + mesh->dispListSize);

            if (func(mesh) == 0)
                mesh = next;
            else
            {
                dlstart = (void *)mesh->dispListStart;
                switch (mesh->type)
                {
                case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                    break;
                case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                    draw_nl_disp_list_type_a(dlstart, next);
                    break;
                default:
                    u_draw_nl_disp_list_type_b_1(dlstart, next);
                    break;
                }
                mesh = next;
            }
        }
    }
}

void unk_empty(void)
{
}
