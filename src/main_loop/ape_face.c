/* Ape face and eye models, the opening logo sprites, replay stage name and ape animation. */
#pragma peephole off
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "bitmap.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "sprite.h"
#include "textdraw.h"

struct GMATevLayer
{
    /*0x00*/ u32 flags;
    /*0x04*/ u16 texIndex;
    u8 unk6[2];
    /*0x08*/ GXTexObj * DPTR texObj;
    u8 unkC[0x20 - 0xC];
};

struct ApePart
{
    /*0x00*/ s16 modelId;
    u8 unk2[6];
    /*0x08*/ float unk8;
};

struct ApeFaceArg
{
    /*0x00*/ s32 type;
    /*0x04*/ float frame;
};

struct MorphVtx
{
    /*0x00*/ Vec a;
    /*0x0C*/ Vec b;
};

struct Morph
{
    /*0x00*/ Vec a;
    /*0x0C*/ Vec b;
    /*0x18*/ struct MorphVtx vtx[3];
    /*0x60*/ s16 idx[3];
};

void ray_scale(Vec *a, Vec *b, Vec *out, float t);
void call_g_avdisp_set_ambient(float r, float g, float b);

extern u32 g_some_other_flags;
extern u32 g_font_char_var_3;

s8 lbl_1_bss_C87E0;
s32 lbl_1_bss_C87E4[16];
s32 lbl_1_bss_C8824[17];
extern struct NlObj *lbl_1_bss_C8AB0;
extern GXTexObj *lbl_1_bss_C8AB4[4];
extern struct GMA *lbl_1_bss_C8AC4[12];

/* These unused functions were removed by the linker, but their literals
 * were not; they are what puts these constants ahead of the model tables. */
static s8 unused_func_0(void)
{
    return lbl_1_bss_C87E0;
}

static float unused_func_1(float a)
{
    return (a * 1.35f) + (a * 182.04445f);
}

const u32 lbl_1_rodata_B208[] = { 0x3D, 0x10, 0x10 };

static int unused_func_2(float a)
{
    return a == 0.0f;
}

static float unused_func_3(float a)
{
    return a * 0.0625f;
}

const u32 lbl_1_rodata_B21C[] = { 0x3E, 0x3D, 0x11, 0x10, 0x10, 0x10 };

static float unused_func_4(int a)
{
    return a;
}

void fn_1_CD4C8();
void fn_1_CD604();
void fn_1_CD704();
void fn_1_CD894();
void fn_1_CD898();
void fn_1_CD89C();
void fn_1_CDB14();
void fn_1_CEA4C();

struct MorphRay
{
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    u32 unk14;
};

#include "ape_face_rodata.inc"
#include "ape_face_data.inc"


void fn_1_CD4C8(struct Ape *ape)
{
    int idx = (ape->lod >> 1) + (ape->charaId * 2);
    int counter;

    if (ape->flags & 0x8000)
    {
        if (!(g_some_other_flags & 0xA))
            lbl_1_bss_C87E4[ape->apeIdx]++;
        counter = lbl_1_bss_C87E4[ape->apeIdx] % 45;
        if (counter == 44)
            ape->flags &= ~0x8000;
    }
    else
    {
        if (ape->unk250 != 0)
            ape->flags |= 0x8000;
        counter = lbl_1_bss_C87E4[ape->apeIdx] = 0;
    }
    avdisp_draw_model_unculled_sort_never(lbl_1_bss_C8AC4[idx]->modelEntries[lbl_1_data_51568[counter]].model);
}

void fn_1_CD604(struct Ape *ape)
{
    int counter;
    int idx = (ape->lod >> 1) + (ape->charaId * 2);
    int row;

    if (!(ape->flags & 0x10000))
    {
        if (ape->flags & 0x80)
        {
            counter = lbl_1_bss_C8824[ape->apeIdx] & 0x3F;
            if (counter > 15)
            {
                if (counter > 60)
                    ape->flags &= ~0x80;
                counter = 0;
            }
        }
        else
        {
            counter = 0;
            lbl_1_bss_C8824[ape->apeIdx] = counter;
        }
        row = ape->charaId + (ape->lod >> 1) * 4;
        avdisp_draw_model_unculled_sort_never(lbl_1_bss_C8AC4[idx]->modelEntries[lbl_1_data_51368[row][counter]].model);
    }
}

static inline void begin_ambient(void)
{
    if (lbl_1_bss_C87E0 == 0)
    {
        struct Color3f color;

        get_curr_light_group_ambient(&color);
        call_g_avdisp_set_ambient(color.r, color.g, color.b);
    }
}

static inline void end_ambient(void)
{
    if (lbl_1_bss_C87E0 == 0)
        apply_curr_light_group_ambient();
}

static inline void set_ape_layers(struct GMAModel *model, int colorIdx)
{
    struct GMATevLayer *layers = (struct GMATevLayer *)((u8 *)model + 0x40);
    u16 *table = lbl_1_data_5161C;
    int i;

    for (i = 0; i < model->tevLayerCount; i++)
    {
        if (table[0] != layers[i].texIndex)
        {
            if (colorIdx != 0)
            {
                layers[i].texObj = &lbl_1_bss_C8AB4[2][table[colorIdx]];
                layers[i].flags |= 0x10000;
            }
            else
            {
                layers[i].texObj = &model->texObjs[i];
                layers[i].flags &= ~0x10000;
            }
            layers[i].texObj = &lbl_1_bss_C8AB4[2][table[1]];
        }
    }
}

void fn_1_CD704(struct Ape *ape, struct ApePart *part)
{
    struct GMAModel *model = lbl_1_bss_C8AC4[(ape->lod >> 1) + (ape->charaId * 2)]->modelEntries[part->modelId].model;

    begin_ambient();
    set_ape_layers(model, ape->colorIndex);
    avdisp_draw_model_unculled_sort_never(model);
    end_ambient();
}

void fn_1_CD894(void) {}
void fn_1_CD898(void) {}
static inline struct GMAShape *next_shape(struct GMAShape *shape)
{
    u8 *ret = (u8 *)shape + 0x60;
    int i;

    for (i = 0; i < 2; i++)
    {
        if (shape->dispListFlags & (1 << i))
            ret += shape->dispListSizes[i];
    }
    if (shape->dispListFlags & (GMA_SHAPE_HAS_DLIST2 | GMA_SHAPE_HAS_DLIST3))
    {
        struct GMAExtraDispLists *extra = (struct GMAExtraDispLists *)ret;

        ret = (u8 *)extra + 0x20;
        for (i = 0; i < 2; i++)
            ret += extra->dispListSizes[i];
    }
    return (struct GMAShape *)ret;
}

static inline struct GMAShape *find_shape_1(struct GMAModel *model)
{
    struct GMAShape *shape = (struct GMAShape *)((u8 *)model + model->headerSize);
    int i;

    for (i = 0; i < model->opaqueShapeCount; i++)
    {
        if (shape->tevLayerCount == 0)
            return shape;
        shape = next_shape(shape);
    }
    return NULL;
}

static inline struct GMAShape *find_shape_2(struct GMAModel *model)
{
    struct GMAShape *shape = (struct GMAShape *)((u8 *)model + model->headerSize);
    int i;
    int found = 0;

    for (i = 0; i < model->opaqueShapeCount; i++)
    {
        if (shape->tevLayerCount == 0)
        {
            if (found)
                return shape;
            found = 1;
        }
        shape = next_shape(shape);
    }
    return NULL;
}

static inline void set_shape_colors(struct GMAModel *model, int colorIdx)
{
    struct GMAShape *shape = find_shape_1(model);

    if (shape != NULL)
    {
        GXColor *color = &lbl_1_data_5164C[colorIdx];

        shape->materialColor = *color;
        shape->ambientColor = *color;
        shape = find_shape_2(model);
        if (shape != NULL)
        {
            shape->materialColor = *color;
            shape->ambientColor = *color;
        }
    }
}

void fn_1_CD89C(struct Ape *ape, struct ApePart *part)
{
    int chara = ape->charaId;
    struct GMAModel *model = lbl_1_bss_C8AC4[(ape->lod >> 1) + (chara * 2)]->modelEntries[part->modelId].model;

    begin_ambient();
    set_shape_colors(model, ape->colorIndex);
    avdisp_draw_model_unculled_sort_never(model);
    end_ambient();
}

static inline void draw_morph_table(struct Ape *ape, s32 *table, int frame)
{
    if (table != NULL)
    {
        lbl_1_bss_C8824[ape->apeIdx] = 0;
        ape->flags |= 0x10000;
        begin_ambient();
        avdisp_draw_model_unculled_sort_never(lbl_1_bss_C8AC4[ape->charaId * 2]->modelEntries[table[frame]].model);
        end_ambient();
    }
}

static inline void draw_morph_extra(struct Ape *ape, int frame)
{
    s32 *table = lbl_1_data_5189C[ape->charaId];

    if (table != NULL)
    {
        lbl_1_bss_C8824[ape->apeIdx] = 0;
        ape->flags |= 0x10000;
        begin_ambient();
        avdisp_draw_model_unculled_sort_never(lbl_1_bss_C8AC4[ape->charaId * 2]->modelEntries[table[frame]].model);
        end_ambient();
    }
}

static void set_nl_model_colors(struct NlMesh *mesh, int colorIdx)
{
    mesh = (struct NlMesh *)((struct NlModel *)mesh)->meshStart;

    while (mesh->flags != 0)
    {
        if (mesh->tplTexIdx == -1)
        {
            struct NlMesh_sub *sub = &mesh->sub2C;

            if (sub->materialColorA >= 1.0f)
            {
                sub->materialColorR = lbl_1_data_5164C[colorIdx].r / 255.0f;
                sub->materialColorG = lbl_1_data_5164C[colorIdx].g / 255.0f;
                sub->materialColorB = lbl_1_data_5164C[colorIdx].b / 255.0f;
            }
        }
        mesh = (struct NlMesh *)((u8 *)mesh + mesh->dispListSize);
        mesh++;
    }
}

void fn_1_CDB14(struct Ape *ape, struct ApePart *part, struct ApeFaceArg *arg)
{
    struct GMAModel *model;
    s16 vtx;
    u32 c;
    u32 b;
    u32 a;
    int j;
    int i;
    struct NlModel *nlModel;
    int count;
    int chara = ape->charaId;
    struct Morph *morphs;
    int idx;
    int isEven = 0;
    float t;
    float one = 1.0f;

    ape->flags &= ~0x10000;
    if (arg != NULL && arg->frame != 0.0f)
    {
        if (ape->lod >= 2)
            chara += 4;
        switch (arg->type)
        {
        case 0:
            if (chara == 1 || chara == 3)
                isEven = 1;
        case 6:
            if (chara == 2)
            {
                idx = (ape->lod >> 1) + (chara * 2);
                model = lbl_1_bss_C8AC4[idx]->modelEntries[part->modelId].model;

                if (lbl_1_bss_C87E0 == 0)
                {
                    struct Color3f color;

                    get_curr_light_group_ambient(&color);
                    call_g_avdisp_set_ambient(color.r, color.g, color.b);
                }
                set_shape_colors(model, ape->colorIndex);
                avdisp_draw_model_unculled_sort_never(model);
                draw_morph_extra(ape, arg->frame);
                end_ambient();
                return;
            }
            idx = lbl_1_data_516FC[chara];
            if (idx == -1)
                goto fallback;
            count = lbl_1_data_516BC[chara];
            morphs = lbl_1_data_516DC[chara];
            nlModel = lbl_1_bss_C8AB0->models[idx];
            break;
        case 5:
            idx = lbl_1_data_517BC[chara];
            if (idx == -1)
                goto fallback;
            count = lbl_1_data_5177C[chara];
            morphs = lbl_1_data_5179C[chara];
            nlModel = lbl_1_bss_C8AB0->models[idx];
            break;
        case 1:
            idx = lbl_1_data_5175C[chara];
            if (idx == -1)
                goto fallback;
            count = lbl_1_data_5171C[chara];
            morphs = lbl_1_data_5173C[chara];
            nlModel = lbl_1_bss_C8AB0->models[idx];
            break;
        default:
            idx = lbl_1_data_5169C[chara];
            if (idx == -1)
                goto fallback;
            count = lbl_1_data_5165C[chara];
            morphs = lbl_1_data_5167C[chara];
            nlModel = lbl_1_bss_C8AB0->models[idx];
            break;
        }
        t = 1.0f - (arg->frame / 16.0f * one);
        for (i = 0; i < count; i++)
        {
            u32 v2[3];
            u32 v[3];

            ray_scale(&morphs[i].a, &morphs[i].b, (Vec *)v, t);
            a = v[0] | 1;
            b = v[1];
            c = v[2];
            for (j = 0; j < 3; j++)
            {
                struct MorphRay *p;
                u32 c2;
                u32 b2;
                u32 a2;

                if (morphs[i].idx[j] == -1)
                    break;
                vtx = morphs[i].idx[j] >> 2;
                ((u32 *)nlModel)[vtx] = a;
                p = (struct MorphRay *)((u32 *)nlModel + vtx);
                p->unk4 = b;
                p = (struct MorphRay *)((u32 *)nlModel + vtx);
                p->unk8 = c;
                ray_scale(&morphs[i].vtx[j].a, &morphs[i].vtx[j].b, (Vec *)v2, t);
                a2 = v2[0] | 1;
                b2 = v2[1];
                c2 = v2[2];
                p = (struct MorphRay *)((u32 *)nlModel + vtx);
                p->unkC = a2;
                p = (struct MorphRay *)((u32 *)nlModel + vtx);
                p->unk10 = b2;
                p = (struct MorphRay *)((u32 *)nlModel + vtx);
                p->unk14 = c2;
            }
        }
        mtxa_push();
        mtxa_scale_s(0.1f);
        if (lbl_1_bss_C87E0 == 0)
        {
            struct Color3f color;

            get_curr_light_group_ambient(&color);
            nl2ngc_set_ambient(1.35f * color.r, 1.35f * color.g, 1.35f * color.b);
        }
        if (chara == 2 && ape->lod < 2)
            set_nl_model_colors((struct NlMesh *)nlModel, ape->colorIndex);
        nl2ngc_draw_model_sort_translucent(nlModel);
        mtxa_pop();
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        if (isEven != 0)
        {
            j = arg->frame;
#ifdef __MWERKS__
            i = (int)lbl_1_data_5189C[ape->charaId];
            draw_morph_table(ape, (s32 *)i, j);
#else
            /* PC: no pointer round trip through int (truncates on x64) */
            draw_morph_table(ape, lbl_1_data_5189C[ape->charaId], j);
#endif
        }
        end_ambient();
        return;
    }
fallback:
    {
        idx = (ape->lod >> 1) + (ape->charaId * 2);
        model = lbl_1_bss_C8AC4[idx]->modelEntries[part->modelId].model;

        if (lbl_1_bss_C87E0 == 0)
        {
            struct Color3f color;

            get_curr_light_group_ambient(&color);
            call_g_avdisp_set_ambient(color.r, color.g, color.b);
        }
        if (idx == 4)
            set_shape_colors(model, ape->colorIndex);
        avdisp_draw_model_unculled_sort_never(model);
        end_ambient();
    }
}

void fn_1_CE618(struct Ape *ape, int type, int frame)
{
    s16 vtx;
    u32 c;
    u32 b;
    u32 a;
    int j;
    int i;
    struct NlModel *nlModel;
    int chara = ape->charaId;
    int count;
    struct Morph *morphs;
    int idx;
    u32 v2[3];
    u32 v[3];

    switch (type)
    {
    case 0:
    case 6:
        if (chara == 2)
        {
            avdisp_draw_model_unculled_sort_never(lbl_1_bss_C8AC4[chara * 2]->modelEntries[15].model);
            draw_morph_extra(ape, frame);
        }
        idx = lbl_1_data_516FC[chara];
        if (idx == -1)
            return;
        count = lbl_1_data_516BC[chara];
        morphs = lbl_1_data_516DC[chara];
        nlModel = lbl_1_bss_C8AB0->models[idx];
        break;
    case 1:
        idx = lbl_1_data_5175C[chara];
        if (idx == -1)
            return;
        count = lbl_1_data_5171C[chara];
        morphs = lbl_1_data_5173C[chara];
        nlModel = lbl_1_bss_C8AB0->models[idx];
        break;
    case 5:
        idx = lbl_1_data_517BC[chara];
        if (idx == -1)
            return;
        count = lbl_1_data_5177C[chara];
        morphs = lbl_1_data_5179C[chara];
        nlModel = lbl_1_bss_C8AB0->models[idx];
        break;
    default:
        idx = lbl_1_data_5169C[chara];
        if (idx == -1)
            return;
        count = lbl_1_data_5165C[chara];
        morphs = lbl_1_data_5167C[chara];
        nlModel = lbl_1_bss_C8AB0->models[idx];
        break;
    }
    for (i = 0; i < count; i++)
    {
        ray_scale(&morphs[i].a, &morphs[i].b, (Vec *)v, 1.0f - frame / 16.0f);
        a = v[0] | 1;
        b = v[1];
        c = v[2];
        for (j = 0; j < 3; j++)
        {
            struct MorphRay *p;
            u32 c2;
            u32 b2;
            u32 a2;

            if (morphs[i].idx[j] == -1)
                break;
            vtx = morphs[i].idx[j] >> 2;
            ((u32 *)nlModel)[vtx] = a;
            p = (struct MorphRay *)((u32 *)nlModel + vtx);
            p->unk4 = b;
            p = (struct MorphRay *)((u32 *)nlModel + vtx);
            p->unk8 = c;
            ray_scale(&morphs[i].vtx[j].a, &morphs[i].vtx[j].b, (Vec *)v2, 1.0f - frame / 16.0f);
            a2 = v2[0] | 1;
            b2 = v2[1];
            c2 = v2[2];
            p = (struct MorphRay *)((u32 *)nlModel + vtx);
            p->unkC = a2;
            p = (struct MorphRay *)((u32 *)nlModel + vtx);
            p->unk10 = b2;
            p = (struct MorphRay *)((u32 *)nlModel + vtx);
            p->unk14 = c2;
        }
    }
    mtxa_scale_s(0.1f);
    nl2ngc_draw_model_sort_translucent(nlModel);
}

void fn_1_CEA4C(struct Ape *ape, struct ApePart *part, struct ApeFaceArg *arg)
{
    struct GMAModel *model = lbl_1_bss_C8AC4[(ape->lod >> 1) + (ape->charaId * 2)]->modelEntries[part->modelId].model;

    begin_ambient();
    if ((ape->unk250 == 1 && ape->unk2A4 != 5) || ape->unk250 == 0)
    {
        u32 t = g_font_char_var_3 << 12;

        if (!(t & 0x30000))
        {
            float s = math_sin(t);

            if (part->unk8 < 0.0f)
                s = -s;
            mtxa_rotate_z(182.04445f * (10.0f * s));
        }
    }
    else if (arg != NULL)
        mtxa_rotate_z(182.04445f * arg->frame);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    if (ape->charaId == 2)
        set_shape_colors(model, ape->colorIndex);
    avdisp_draw_model_unculled_sort_never(model);
    end_ambient();
}
