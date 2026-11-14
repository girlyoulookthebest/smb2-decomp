/* Effect callbacks used by the story mode scenes. */

#include "types.h"
#include "camera.h"
#include "effect.h"
#include "pool.h"
#include "macros.h"
#include "rand.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "frustum.h"
#include "gx.h"
#include "mathutil.h"
#include "mot_joint.h"
#include "nl2ngc.h"

struct EffectOrdNode
{
    struct OrdTblNode node;
    struct Effect *effect;
};

struct QuadDesc
{
    /*0x00*/ Vec pos;
    /*0x0C*/ float radius;
    /*0x10*/ s16 rot;
    /*0x12*/ u16 flags;
    /*0x14*/ GXColor color[4];
};

struct SceneQuad
{
    /*0x00*/ Vec pos;
    /*0x0C*/ float radius;
    /*0x10*/ s16 rot;
    /*0x12*/ u16 flags;
    /*0x14*/ GXColor color;
};

struct QuadBatchItem
{
    /*0x00*/ float scale;
    /*0x04*/ Mtx mtx;
    /*0x34*/ u8 filler34[2];
    /*0x36*/ GXColor color[4];
};  /* size = 0x48 */

/* The scene player's struct Instance (scnplay.c) an effect is attached to;
 * the fields up to mtx must keep Instance's layout (also on 64-bit hosts). */
struct SceneEffectJoint
{
    /*0x00*/ u16 type;
    /*0x02*/ u16 idx;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ char *name;
    /*0x0C*/ u32 flags;
    /*0x10*/ struct AnimJoint *joints;  /* joints[0].scale is the object's scale */
    /*0x14*/ Mtx mtx;
};

extern struct
{
    /*0x00*/ u32 flags;
} g_some_render_flag;
extern struct GMA *efcmdlGma;
extern u32 g_font_char_var_3;

static inline void f32_to_u8(register float *in, register u8 *out)
{
#ifdef __MWERKS__
    asm
    {
        lfs f1, 0(in)
        psq_st f1, 0(out), 1, 2
    }
#else
    *out = pc_psq_st_u8(*in);
#endif
}

void *ord_tbl_alloc_node(u32 size);
struct OrdTblNode *fn_1_CCC18(float z);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void fn_1_16B00C(void *quad);
void fn_1_1535CC(struct OrdTblNode *node_);
void fn_1_153B04(struct OrdTblNode *node_);
void fn_1_1548BC(struct OrdTblNode *node_);
void fn_1_155F10(struct OrdTblNode *node_);
void fn_1_1562E0(struct OrdTblNode *node_);
void fn_1_156F48(struct OrdTblNode *node_);
void fn_1_157308(struct OrdTblNode *node_);
void fn_1_157680(struct OrdTblNode *node_);
void fn_1_157A84(struct OrdTblNode *node_);
void fn_1_158338(struct OrdTblNode *node_);
void fn_1_158B84(struct OrdTblNode *node_);
void fn_1_1593F4(struct OrdTblNode *node_);
void fn_1_15A364(struct OrdTblNode *node_);
void fn_1_15AF80(struct OrdTblNode *node_);
void fn_1_15B7F8(struct OrdTblNode *node_);
struct LineVertex
{
    /*0x00*/ Vec pos;
    /*0x0C*/ GXColor color;
};

void fn_1_E33A8(u16 count, struct LineVertex *verts);
void fn_1_15CCEC(struct OrdTblNode *node_);
void fn_1_15DE84(struct OrdTblNode *node_);
void fn_1_15ECEC(struct OrdTblNode *node_);
void fn_1_15F4DC(struct OrdTblNode *node_);
void fn_1_E2F1C(void);
void fn_1_16B608(void *quad);
void fn_1_16BEC0(int count, struct QuadBatchItem *items);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
struct OrdTblNode *fn_1_CCD5C(int a);
void g_stores_doubles2(float a, float b, float c, float d);
void g_smth_with_gpu_and_locked_cache_mtx(void);

void fn_1_152EE8(struct Effect *effect);
void fn_1_152EEC(struct Effect *effect);
void fn_1_153110(struct Effect *effect);
void fn_1_1531B8(struct Effect *effect);
void fn_1_1531BC(struct Effect *effect);
void fn_1_153248(struct Effect *effect);
void fn_1_15351C(struct Effect *effect);
void fn_1_1537FC(struct Effect *effect);
void fn_1_153800(struct Effect *effect);
void fn_1_153960(struct Effect *effect);
void fn_1_153A68(struct Effect *effect);
void fn_1_153D50(struct Effect *effect);
void fn_1_153D54(struct Effect *effect);
void fn_1_153D58(struct Effect *effect);
void fn_1_153D8C(struct Effect *effect);
void fn_1_15414C(struct Effect *effect);
void fn_1_154150(struct Effect *effect);
void fn_1_1541D8(struct Effect *effect);
void fn_1_154520(struct Effect *effect);
void fn_1_154524(struct Effect *effect);
void fn_1_154528(struct Effect *effect);
void fn_1_154624(struct Effect *effect);
void fn_1_1547F8(struct Effect *effect);
void fn_1_154E08(struct Effect *effect);
void fn_1_154E0C(struct Effect *effect);
void fn_1_154E18(struct Effect *effect);
void fn_1_154EF8(struct Effect *effect);
void fn_1_155088(struct Effect *effect);
void fn_1_15508C(struct Effect *effect);
void fn_1_155090(struct Effect *effect);
void fn_1_155170(struct Effect *effect);
void fn_1_1552FC(struct Effect *effect);
void fn_1_155300(struct Effect *effect);
void fn_1_155390(struct Effect *effect);
void fn_1_155CE8(struct Effect *effect);
void fn_1_155E74(struct Effect *effect);
void fn_1_155E78(struct Effect *effect);
void fn_1_155E7C(struct Effect *effect);
void fn_1_155EA8(struct Effect *effect);
void fn_1_156060(struct Effect *effect);
void fn_1_156064(struct Effect *effect);
void fn_1_1560A0(struct Effect *effect);
void fn_1_156274(struct Effect *effect);
void fn_1_15691C(struct Effect *effect);
void fn_1_156920(struct Effect *effect);
void fn_1_156A34(struct Effect *effect);
void fn_1_156E9C(struct Effect *effect);
void fn_1_157060(struct Effect *effect);
void fn_1_157064(struct Effect *effect);
void fn_1_1570F8(struct Effect *effect);
void fn_1_15725C(struct Effect *effect);
void fn_1_15746C(struct Effect *effect);
void fn_1_157470(struct Effect *effect);
void fn_1_157474(struct Effect *effect);
void fn_1_1575C4(struct Effect *effect);
void fn_1_15785C(struct Effect *effect);
void fn_1_157860(struct Effect *effect);
void fn_1_15789C(struct Effect *effect);
void fn_1_1579C8(struct Effect *effect);
void fn_1_157CE0(struct Effect *effect);
void fn_1_157CE4(struct Effect *effect);
void fn_1_157CE8(struct Effect *effect);
void fn_1_158260(struct Effect *effect);
void fn_1_1584B4(struct Effect *effect);
void fn_1_1584B8(struct Effect *effect);
void fn_1_158574(struct Effect *effect);
void fn_1_158ABC(struct Effect *effect);
void fn_1_15902C(struct Effect *effect);
void fn_1_159030(struct Effect *effect);
void fn_1_15912C(struct Effect *effect);
void fn_1_159330(struct Effect *effect);
void fn_1_159940(struct Effect *effect);
void fn_1_159944(struct Effect *effect);
void fn_1_159948(struct Effect *effect);
void fn_1_159DA4(struct Effect *effect);
void fn_1_159E38(struct Effect *effect);
void fn_1_159E3C(struct Effect *effect);
void fn_1_159E40(struct Effect *effect);
void fn_1_159F8C(struct Effect *effect);
void fn_1_15A150(struct Effect *effect);
void fn_1_15A154(struct Effect *effect);
void fn_1_15A194(struct Effect *effect);
void fn_1_15A2F8(struct Effect *effect);
void fn_1_15ADC4(struct Effect *effect);
void fn_1_15ADC8(struct Effect *effect);
void fn_1_15ADCC(struct Effect *effect);
void fn_1_15AF00(struct Effect *effect);
void fn_1_15B4BC(struct Effect *effect);
void fn_1_15B4C0(struct Effect *effect);
void fn_1_15B5C8(struct Effect *effect);
void fn_1_15B78C(struct Effect *effect);
void fn_1_15BDF0(struct Effect *effect);
void fn_1_15BDF4(struct Effect *effect);
void fn_1_15BE28(struct Effect *effect);
void fn_1_15C3E4(struct Effect *effect);
void fn_1_15C79C(struct Effect *effect);
void fn_1_15C7A0(struct Effect *effect);
void fn_1_15C7BC(struct Effect *effect);
void fn_1_15CC70(struct Effect *effect);
void fn_1_15CF4C(struct Effect *effect);
void fn_1_15CF50(struct Effect *effect);
void fn_1_15CF54(struct Effect *effect);
void fn_1_15CF88(struct Effect *effect);
void fn_1_15D348(struct Effect *effect);
void fn_1_15D34C(struct Effect *effect);
void fn_1_15D3E8(struct Effect *effect);
void fn_1_15DD40(struct Effect *effect);
void fn_1_15E970(struct Effect *effect);
void fn_1_15E974(struct Effect *effect);
void fn_1_15E978(struct Effect *effect);
void fn_1_15EB78(struct Effect *effect);
void fn_1_15F25C(struct Effect *effect);
void fn_1_15F260(struct Effect *effect);
void fn_1_15F29C(struct Effect *effect);
void fn_1_15F470(struct Effect *effect);
void fn_1_15F71C(struct Effect *effect);

// lbl_1_data_706A0 has no other references here; the leading bytes are kept verbatim.
struct SceneEffectData
{
    u8 unk0[0x170];
    struct EffectFuncs funcs[29];
} lbl_1_data_706A0 = {
    {
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
    },
    {
        { fn_1_152EE8, fn_1_152EEC, fn_1_153110, fn_1_1531B8 },
        { fn_1_1531BC, fn_1_153248, fn_1_15351C, fn_1_1537FC },
        { fn_1_153800, fn_1_153960, fn_1_153A68, fn_1_153D50 },
        { fn_1_153D54, fn_1_153D58, fn_1_153D8C, fn_1_15414C },
        { fn_1_154150, fn_1_1541D8, fn_1_154520, fn_1_154524 },
        { fn_1_154528, fn_1_154624, fn_1_1547F8, fn_1_154E08 },
        { fn_1_154E0C, fn_1_154E18, fn_1_154EF8, fn_1_155088 },
        { fn_1_15508C, fn_1_155090, fn_1_155170, fn_1_1552FC },
        { fn_1_155300, fn_1_155390, fn_1_155CE8, fn_1_155E74 },
        { fn_1_155E78, fn_1_155E7C, fn_1_155EA8, fn_1_156060 },
        { fn_1_156064, fn_1_1560A0, fn_1_156274, fn_1_15691C },
        { fn_1_156920, fn_1_156A34, fn_1_156E9C, fn_1_157060 },
        { fn_1_157064, fn_1_1570F8, fn_1_15725C, fn_1_15746C },
        { fn_1_157470, fn_1_157474, fn_1_1575C4, fn_1_15785C },
        { fn_1_157860, fn_1_15789C, fn_1_1579C8, fn_1_157CE0 },
        { fn_1_157CE4, fn_1_157CE8, fn_1_158260, fn_1_1584B4 },
        { fn_1_1584B8, fn_1_158574, fn_1_158ABC, fn_1_15902C },
        { fn_1_159030, fn_1_15912C, fn_1_159330, fn_1_159940 },
        { fn_1_159944, fn_1_159948, fn_1_159DA4, fn_1_159E38 },
        { fn_1_159E3C, fn_1_159E40, fn_1_159F8C, fn_1_15A150 },
        { fn_1_15A154, fn_1_15A194, fn_1_15A2F8, fn_1_15ADC4 },
        { fn_1_15ADC8, fn_1_15ADCC, fn_1_15AF00, fn_1_15B4BC },
        { fn_1_15B4C0, fn_1_15B5C8, fn_1_15B78C, fn_1_15BDF0 },
        { fn_1_15BDF4, fn_1_15BE28, fn_1_15C3E4, fn_1_15C79C },
        { fn_1_15C7A0, fn_1_15C7BC, fn_1_15CC70, fn_1_15CF4C },
        { fn_1_15CF50, fn_1_15CF54, fn_1_15CF88, fn_1_15D348 },
        { fn_1_15D34C, fn_1_15D3E8, fn_1_15DD40, fn_1_15E970 },
        { fn_1_15E974, fn_1_15E978, fn_1_15EB78, fn_1_15F25C },
        { fn_1_15F260, fn_1_15F29C, fn_1_15F470, fn_1_15F71C },
    },
};

void fn_1_152BE8(void)
{
    struct SceneEffectData *data = &lbl_1_data_706A0;

    overwrite_effect_funcs(0x27, &data->funcs[0]);
    overwrite_effect_funcs(0x28, &data->funcs[1]);
    overwrite_effect_funcs(0x29, &data->funcs[2]);
    overwrite_effect_funcs(0x2A, &data->funcs[3]);
    overwrite_effect_funcs(0x2B, &data->funcs[4]);
    overwrite_effect_funcs(0x2C, &data->funcs[5]);
    overwrite_effect_funcs(0x2D, &data->funcs[6]);
    overwrite_effect_funcs(0x2E, &data->funcs[7]);
    overwrite_effect_funcs(0x2F, &data->funcs[8]);
    overwrite_effect_funcs(0x30, &data->funcs[9]);
    overwrite_effect_funcs(0x31, &data->funcs[10]);
    overwrite_effect_funcs(0x32, &data->funcs[11]);
    overwrite_effect_funcs(0x33, &data->funcs[12]);
    overwrite_effect_funcs(0x34, &data->funcs[13]);
    overwrite_effect_funcs(0x35, &data->funcs[14]);
    overwrite_effect_funcs(0x36, &data->funcs[15]);
    overwrite_effect_funcs(0x37, &data->funcs[16]);
    overwrite_effect_funcs(0x38, &data->funcs[17]);
    overwrite_effect_funcs(0x39, &data->funcs[18]);
    overwrite_effect_funcs(0x3A, &data->funcs[19]);
    overwrite_effect_funcs(0x3B, &data->funcs[20]);
    overwrite_effect_funcs(0x3C, &data->funcs[21]);
    overwrite_effect_funcs(0x3D, &data->funcs[22]);
    overwrite_effect_funcs(0x3E, &data->funcs[23]);
    overwrite_effect_funcs(0x3F, &data->funcs[24]);
    overwrite_effect_funcs(0x40, &data->funcs[25]);
    overwrite_effect_funcs(0x41, &data->funcs[26]);
    overwrite_effect_funcs(0x42, &data->funcs[27]);
    overwrite_effect_funcs(0x43, &data->funcs[28]);
}

void fn_1_152D70(void)
{
    overwrite_effect_funcs(0x27, NULL);
    overwrite_effect_funcs(0x28, NULL);
    overwrite_effect_funcs(0x29, NULL);
    overwrite_effect_funcs(0x2A, NULL);
    overwrite_effect_funcs(0x2B, NULL);
    overwrite_effect_funcs(0x2C, NULL);
    overwrite_effect_funcs(0x2D, NULL);
    overwrite_effect_funcs(0x2E, NULL);
    overwrite_effect_funcs(0x2F, NULL);
    overwrite_effect_funcs(0x30, NULL);
    overwrite_effect_funcs(0x31, NULL);
    overwrite_effect_funcs(0x32, NULL);
    overwrite_effect_funcs(0x33, NULL);
    overwrite_effect_funcs(0x34, NULL);
    overwrite_effect_funcs(0x35, NULL);
    overwrite_effect_funcs(0x36, NULL);
    overwrite_effect_funcs(0x37, NULL);
    overwrite_effect_funcs(0x38, NULL);
    overwrite_effect_funcs(0x39, NULL);
    overwrite_effect_funcs(0x3A, NULL);
    overwrite_effect_funcs(0x3B, NULL);
    overwrite_effect_funcs(0x3C, NULL);
    overwrite_effect_funcs(0x3D, NULL);
    overwrite_effect_funcs(0x3E, NULL);
    overwrite_effect_funcs(0x3F, NULL);
    overwrite_effect_funcs(0x40, NULL);
    overwrite_effect_funcs(0x41, NULL);
    overwrite_effect_funcs(0x42, NULL);
    overwrite_effect_funcs(0x43, NULL);
}

void fn_1_152EE8(struct Effect *effect)
{
}

void fn_1_152EEC(struct Effect *effect)
{
    Vec v;
    float t;

    v.x = effect->unk8C.x - effect->pos.x;
    v.y = effect->unk8C.y - effect->pos.y;
    v.z = effect->unk8C.z - effect->pos.z;
    if (0.0f != v.x || 0.0f != v.y || 0.0f != v.z)
        vec_set_len(1.0f, &v, &v);
    else
        v.y = 1.0f;
    effect->vel.y += -0.008f;
    t = effect->unk98.x;
    effect->vel.x = effect->vel.x + t * (v.x - effect->vel.x);
    effect->vel.y = effect->vel.y + t * (v.y - effect->vel.y);
    effect->vel.z = effect->vel.z + t * (v.z - effect->vel.z);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk52 = effect->unk52 - (effect->unk52 >> 6);
    effect->unk54 = effect->unk54 - (effect->unk54 >> 6);
    effect->unk56 = effect->unk56 - (effect->unk56 >> 6);
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    effect->scale.x = effect->scale.x + 0.15f * (effect->scale.y - effect->scale.x);
    if (effect->u_otherTimer > 0)
        effect->u_otherTimer--;
    else if (effect->unkA4 == 0)
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
}

void fn_1_153110(struct Effect *effect)
{
    struct GMAModel *model = effect->model;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
        effect->unkA4 = 1;
    }
    else
        effect->unkA4 = 0;
}

void fn_1_1531B8(struct Effect *effect)
{
}

void fn_1_1531BC(struct Effect *effect)
{
    float t = 0.25f + 0.5f * RAND_FLOAT();

    effect->unk18 = t;
    effect->unk1C = 0.7f * t;
    effect->unk20 = 0.2f * t;
}

void fn_1_153248(struct Effect *effect)
{
    Vec v;
    float t;

    effect->unk18 = effect->unk18 + 0.05f * (1.0f - effect->unk18);
    effect->unk1C = effect->unk1C + 0.05f * (0.98f - effect->unk1C);
    effect->unk20 = effect->unk20 + 0.05f * (0.8f - effect->unk20);
    v.x = effect->unk8C.x - effect->pos.x;
    v.y = effect->unk8C.y - effect->pos.y;
    v.z = effect->unk8C.z - effect->pos.z;
    if (0.0f != v.x || 0.0f != v.y || 0.0f != v.z)
        vec_set_len(1.0f, &v, &v);
    else
        v.y = 1.0f;
    effect->vel.y += -0.002f;
    t = effect->unk98.x;
    effect->vel.x = effect->vel.x + t * (v.x - effect->vel.x);
    effect->vel.y = effect->vel.y + t * (v.y - effect->vel.y);
    effect->vel.z = effect->vel.z + t * (v.z - effect->vel.z);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk56 = effect->unk56 - (effect->unk56 >> 7);
    effect->rotZ += effect->unk56;
    if ((float)effect->timer > 15.0f)
    {
        effect->scale.x = effect->scale.x + 0.15f * (effect->scale.y - effect->scale.x);
        effect->colorFactor = effect->colorFactor + 0.2f * (1.0f - effect->colorFactor);
    }
    else
    {
        float r = 1.0f / (effect->timer + 1);

        effect->scale.x *= 1.005f;
        effect->colorFactor *= 1.0f - r;
    }
    if (effect->u_otherTimer > 0)
        effect->u_otherTimer--;
    else if (effect->unkA4 == 0)
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
}

void fn_1_15351C(struct Effect *effect)
{
    struct GMAModel *model = effect->model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_1535CC;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
        effect->unkA4 = 1;
    }
    else
        effect->unkA4 = 0;
}

void fn_1_1535CC(struct OrdTblNode *node_)
{
    struct SceneQuad quad;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;

    GXSetNumChans_cached(0);
    GXSetCullMode_cached(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevSwapMode_cached(0, 0, 1);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0x1C);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xE);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 6, 4, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetZMode_cached(1, 3, 0);
    g_something_with_raster_state(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXLoadTexObj_cached(model->texObjs, 0);
    quad.pos = effect->pos;
    quad.radius = 0.70710677f * (model->boundSphereRadius * effect->scale.x);
    quad.rot = effect->rotZ;
    quad.color.r = 255.0f * effect->unk18;
    quad.color.g = 255.0f * effect->unk1C;
    quad.color.b = 255.0f * effect->unk20;
    quad.color.a = 255.0f * effect->colorFactor;
    mtxa_from_mtxb();
    fn_1_16B00C(&quad);
}

void fn_1_1537FC(struct Effect *effect)
{
}

void fn_1_153800(struct Effect *effect)
{
    float len = mathutil_vec_len(&effect->vel);

    effect->timer = 15.0f + 60.0f * len + (rand() & 0xF);
    if (effect->timer > 60)
        effect->timer = 60;
    effect->u_otherTimer = (rand() & 3) + 10;
    effect->vel.y = 0.0040833335f;
    effect->cameras = 1 << effect->playerId;
    if (0.0f == effect->scale.x)
        effect->scale.x = 1.0f;
    effect->scale.x *= 0.2f + 0.2f * RAND_FLOAT();
    effect->scale.y = effect->scale.x;
    effect->scale.x = 0.0f;
}

void fn_1_153960(struct Effect *effect)
{
    effect->model = init_common_gma->modelEntries[38].model;
    effect->vel.y -= 0.0008166666666666666;
    effect->vel.x *= 0.98f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk18 = effect->unk18 + 0.0755f * (1.0f - effect->unk18);
    effect->unk1C = effect->unk1C + 0.07525f * (1.0f - effect->unk1C);
    effect->unk20 = effect->unk20 + 0.075f * (1.0f - effect->unk20);
    effect->scale.x = effect->scale.x + 0.2f * (effect->scale.y - effect->scale.x);
}

void fn_1_153A68(struct Effect *effect)
{
    struct GMAModel *model = effect->model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_153B04;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_153B04(struct OrdTblNode *node_)
{
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    Vec spC;
    float t;
    float tanFov;
    float scale;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mathutil_mtxA_get_translate(&spC);
    mtxa_sq_from_identity();
    t = (float)(effect->timer % effect->u_otherTimer) / (float)effect->u_otherTimer;
    if (!(spC.z >= -1.1920929e-7f))
    {
        tanFov = math_tan(g_current_camera->sub28.view.fov / 2);
        scale = effect->scale.x * (0.5 + 0.5 * t) * math_sqrt((-spC.z * tanFov) / 0.577350269);
        mtxa_scale_s(scale);
        mtxa_rotate_z(262144.0f * ((spC.x + spC.y) / (spC.z * tanFov)));
        avdisp_set_scale_factor(scale);
        if (effect->timer >= 56)
            t *= (float)(60 - effect->timer) / 4.0;
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_z_mode(1, 3, 0);
        set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, t);
        avdisp_draw_model_unculled_sort_never(effect->model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void fn_1_153D50(struct Effect *effect)
{
}

void fn_1_153D54(struct Effect *effect)
{
}

void fn_1_153D58(struct Effect *effect)
{
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
}

void fn_1_153D8C(struct Effect *effect)
{
    Vec sp8;
    struct GMAModel *model = effect->model;
    float scale;
    float t;
    float f;
    float r, g, b;

    avdisp_set_z_mode(1, 3, 0);
    mtxa_from_mtxb();
    mtxa_mult_right(((struct SceneEffectJoint *)model)->mtx);
    mtxa_translate(&effect->unk8C);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp8);
    t = (float)fabs(sp8.z);
    model = efcmdlGma->modelEntries[8].model;
    scale = 0.5f * (1.0f + t) * effect->scale.x;
    if (g_is_sphere_visible(&model->boundSphereCenter, scale))
    {
        mtxa_push();
        mtxa_sq_from_identity();
        mtxa_scale_s(scale / model->boundSphereRadius);
        scale = effect->scale.x;
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < -(0.1f + scale))
        {
            f = (sp8.z + scale) / sp8.z;
            sp8.x *= f;
            sp8.y *= f;
            sp8.z += scale;
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
        }
        f = t;
        r = effect->unk18 * f;
        g = effect->unk1C * f;
        b = effect->unk20 * f;
        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;
        set_post_mult_color(r, g, b, 1.0f);
        avdisp_draw_model_unculled_sort_always(model);
        mtxa_pop();
    }
    model = efcmdlGma->modelEntries[10].model;
    t = t * (t * t);
    scale = t * effect->scale.x;
    if (g_is_sphere_visible(&model->boundSphereCenter, scale))
    {
        mtxa_sq_from_identity();
        mtxa_scale_s(scale / model->boundSphereRadius);
        scale = effect->scale.x;
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < -(0.1f + scale))
        {
            f = (sp8.z + scale) / sp8.z;
            sp8.x *= f;
            sp8.y *= f;
            sp8.z += scale;
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
        }
        mtxa_scale_xyz(1.0f, 0.75f, 1.0f);
        if (sp8.z < 0.0f)
            mtxa_rotate_z(16384.0f * (1.2f * sp8.x + sp8.y) / sp8.z);
        f = t;
        r = effect->unk18 * f;
        g = effect->unk1C * f;
        b = effect->unk20 * f;
        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;
        set_post_mult_color(r, g, b, 1.0f);
        avdisp_draw_model_unculled_sort_always(model);
    }
    avdisp_set_z_mode(1, 3, 1);
    fade_color_base_default();
}

void fn_1_15414C(struct Effect *effect)
{
}

void fn_1_154150(struct Effect *effect)
{
    struct SceneEffectJoint *joint = (void *)effect->model;

    if (joint != NULL)
    {
        mtxa_from_mtx(joint->mtx);
        mtxa_translate(&effect->unk8C);
        mathutil_mtxA_get_translate(&effect->pos);
    }
    effect->rotZ = (rand() & 0x7FFF) - 0x7FFF;
    effect->u_otherTimer = 1;
}

void fn_1_1541D8(struct Effect *effect)
{
    Vec prevPos;
    struct Effect spawn;
    struct SceneEffectJoint *joint = (void *)effect->model;
    float t;

    prevPos = effect->pos;
    if (joint != NULL)
    {
        struct AnimJoint *owner = joint->joints;

        mtxa_from_mtx(joint->mtx);
        mtxa_translate(&effect->unk8C);
        mathutil_mtxA_get_translate(&effect->pos);
        effect->vel.x = effect->pos.x - prevPos.x;
        effect->vel.y = effect->pos.y - prevPos.y;
        effect->vel.z = effect->pos.z - prevPos.z;
        effect->scale.x = effect->scale.y * owner->scale.x;
    }
    else
    {
        effect->vel.y += 0.004f;
        effect->vel.x *= 0.98f;
        effect->vel.y *= 0.98f;
        effect->vel.z *= 0.98f;
        effect->pos.x += effect->vel.x;
        effect->pos.y += effect->vel.y;
        effect->pos.z += effect->vel.z;
        effect->scale.x = effect->scale.y;
    }
    effect->u_otherTimer--;
    if (effect->u_otherTimer < 0)
    {
        effect->u_otherTimer = 0;
        memset(&spawn, 0, sizeof(spawn));
        spawn.type = 0x2C;
        spawn.pos = effect->pos;
        t = 0.025f * mathutil_vec_len(&effect->vel);
        spawn.vel.x = 0.5f * effect->vel.x + t * (RAND_FLOAT() - 0.5);
        spawn.vel.y = 0.5f * effect->vel.y + t * (RAND_FLOAT() - 0.5);
        spawn.vel.z = 0.5f * effect->vel.z + t * (RAND_FLOAT() - 0.5);
        spawn.scale.x = effect->scale.x * (0.5f + RAND_FLOAT());
        spawn.scale.y = 0.04f * spawn.scale.x;
        *(Vec *)&spawn.unk18 = *(Vec *)&effect->unk18;
        spawn.rotZ = (rand() & 0x7FFF) - 0x7FFF;
        spawn_effect(&spawn);
    }
}

void fn_1_154520(struct Effect *effect)
{
}

void fn_1_154524(struct Effect *effect)
{
}

void fn_1_154528(struct Effect *effect)
{
    if (effect->timer == 0)
        effect->timer = 60.0f * (1.0f + RAND_FLOAT());
    effect->colorFactor = 1.0f;
    effect->unk56 = 512.0f * (0.5f + RAND_FLOAT());
    if (rand() & 1)
        effect->unk56 = -effect->unk56;
}

void fn_1_154624(struct Effect *effect)
{
    effect->vel.x *= 0.99f;
    effect->vel.y *= 0.99f;
    effect->vel.z *= 0.99f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->rotZ += (s16)(effect->unk56 * mathutil_vec_len(&effect->vel));
    effect->scale.y *= 0.98f;
    effect->scale.x += effect->scale.y;
    effect->unk18 = effect->unk18 + 0.1f * -effect->unk18;
    effect->unk1C = effect->unk1C + 0.1f * -effect->unk1C;
    effect->unk20 = effect->unk20 + 0.1f * -effect->unk20;
    if ((float)effect->timer < 15.0f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
    }
}

void fn_1_1547F8(struct Effect *effect)
{
    struct GMAModel *model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    if (g_some_render_flag.flags & 4)
        return;
    model = efcmdlGma->modelEntries[21].model;
    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_1548BC;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_1548BC(struct OrdTblNode *node_)
{
    Vec sp20;
    Vec light;
    Vec c;
    struct QuadDesc quad;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = efcmdlGma->modelEntries[21].model;
    float r1, g1, b1;
    float r2, g2, b2;
    float t, u;
    u8 alpha;

    light.x = 0.5f;
    light.y = 1.0f;
    light.z = 0.0f;
    r1 = 1.25f;
    g1 = 1.25f;
    b1 = 1.25f;
    r2 = 0.35f;
    g2 = 0.35f;
    b2 = 0.4f;
    opti_GXSetChanCtrl(0, 1, 1, 1, 0, 0, 2);
    opti_GXSetChanCtrl(2, 1, 1, 1, 0, 0, 2);
    GXSetNumChans_cached(1);
    GXSetCullMode_cached(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 4);
    GXSetTevSwapMode_cached(0, 0, 1);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xA);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetZMode_cached(1, 3, 0);
    g_something_with_raster_state(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXLoadTexObj_cached(model->texObjs, 0);
    quad.pos = effect->pos;
    quad.radius = effect->scale.x;
    quad.rot = effect->rotZ;
    alpha = 255.0f * effect->colorFactor;
    mtxa_from_mtxb();
    mtxa_rotate_z(quad.rot);
    mtxa_tf_vec_xyz(-1.0f, -1.0f, 0.0f, &sp20);
    t = vec_dot_normalized_safe(&sp20, &light);
    t = 0.5f * (1.0f + t);
    u = 1.0f - t;
    c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
    c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
    c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
    f32_to_u8(&c.x, &quad.color[0].r);
    f32_to_u8(&c.y, &quad.color[0].g);
    f32_to_u8(&c.z, &quad.color[0].b);
    quad.color[0].a = alpha;
    c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
    c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
    c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
    f32_to_u8(&c.x, &quad.color[2].r);
    f32_to_u8(&c.y, &quad.color[2].g);
    f32_to_u8(&c.z, &quad.color[2].b);
    quad.color[2].a = alpha;
    mtxa_tf_vec_xyz(1.0f, -1.0f, 0.0f, &sp20);
    t = vec_dot_normalized_safe(&sp20, &light);
    t = 0.5f * (1.0f + t);
    u = 1.0f - t;
    c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
    c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
    c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
    f32_to_u8(&c.x, &quad.color[1].r);
    f32_to_u8(&c.y, &quad.color[1].g);
    f32_to_u8(&c.z, &quad.color[1].b);
    quad.color[1].a = alpha;
    c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
    c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
    c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
    f32_to_u8(&c.x, &quad.color[3].r);
    f32_to_u8(&c.y, &quad.color[3].g);
    f32_to_u8(&c.z, &quad.color[3].b);
    quad.color[3].a = alpha;
    mtxa_from_mtxb();
    fn_1_16B608(&quad);
}

void fn_1_154E08(struct Effect *effect)
{
}

void fn_1_154E0C(struct Effect *effect)
{
    effect->timer = 60;
}

void fn_1_154E18(struct Effect *effect)
{
    effect->scale.y *= 0.995f;
    effect->scale.x += effect->scale.y;
    effect->unk18 = effect->unk18 + 0.05 * (-0.01 - effect->unk18);
    effect->unk1C = effect->unk1C + 0.06 * (-0.01 - effect->unk1C);
    effect->unk20 = effect->unk20 + 0.08 * (-0.01 - effect->unk20);
    if (effect->unk18 < 0.0f && effect->unk1C < 0.0f && effect->unk20 < 0.0f)
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
}

void fn_1_154EF8(struct Effect *effect)
{
    struct GMAModel *model = effect->model;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        mtxa_sq_from_identity();
        mtxa_scale_s(effect->scale.x);
        {
            float r = effect->unk18;
            float g = effect->unk1C;
            float b = effect->unk20;

            if (r > 1.0f)
                r = 1.0f;
            else if (r < 0.0f)
                r = 0.0f;
            if (g > 1.0f)
                g = 1.0f;
            else if (g < 0.0f)
                g = 0.0f;
            if (b > 1.0f)
                b = 1.0f;
            else if (b < 0.0f)
                b = 0.0f;
            set_post_mult_color(r, g, b, 1.0f);
        }
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_always(model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void fn_1_155088(struct Effect *effect)
{
}

void fn_1_15508C(struct Effect *effect)
{
}

void fn_1_155090(struct Effect *effect)
{
    effect->scale.y *= 0.999f;
    effect->scale.x += effect->scale.y;
    effect->unk18 = effect->unk18 + 0.1 * (-0.01 - effect->unk18);
    effect->unk1C = effect->unk1C + 0.12 * (-0.01 - effect->unk1C);
    effect->unk20 = effect->unk20 + 0.16 * (-0.01 - effect->unk20);
    if (effect->unk18 < 0.0f && effect->unk1C < 0.0f && effect->unk20 < 0.0f)
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
}

void fn_1_155170(struct Effect *effect)
{
    struct GMAModel *model = effect->model;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        mtxa_scale_s(effect->scale.x);
        {
            float r = effect->unk18;
            float g = effect->unk1C;
            float b = effect->unk20;

            if (r > 1.0f)
                r = 1.0f;
            else if (r < 0.0f)
                r = 0.0f;
            if (g > 1.0f)
                g = 1.0f;
            else if (g < 0.0f)
                g = 0.0f;
            if (b > 1.0f)
                b = 1.0f;
            else if (b < 0.0f)
                b = 0.0f;
            set_post_mult_color(r, g, b, 1.0f);
        }
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_always(model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void fn_1_1552FC(struct Effect *effect)
{
}

void fn_1_155300(struct Effect *effect)
{
    Vec v;
    struct GMAModel *model = effect->model;

    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale(&effect->scale);
    mtxa_tf_vec(&model->boundSphereCenter, &v);
    effect->pos.x += v.x;
    effect->pos.y += v.y;
    effect->pos.z += v.z;
}

void fn_1_155390(struct Effect *effect)
{
    Vec v;
    struct Effect spawn;
    float d;
    float rx, ry, rz;
    float tmp;
    float size;
    float count;

    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_tf_vec(&effect->unk70, &v);
    size = 0.5f * effect->model->boundSphereRadius;
    effect->vel.y += -0.016f;
    d = mathutil_vec_dot_prod(&effect->vel, &v);
    rx = 8.0f * d;
    ry = 16.0f * d;
    rz = 4.0f * d;
    effect->vel.x -= d * v.x;
    effect->vel.y -= d * v.y;
    effect->vel.z -= d * v.z;
    rx += 8.0f * effect->vel.y;
    ry += 16.0f * effect->vel.z;
    rz += 4.0f * effect->vel.x;
    tmp = effect->unk64.x;
    effect->vel.x *= tmp;
    effect->vel.y *= tmp;
    effect->vel.z *= tmp;
    tmp = effect->unk64.z;
    d *= tmp;
    effect->vel.x += d * v.x;
    effect->vel.y += d * v.y;
    effect->vel.z += d * v.z;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    if (effect->pos.y < size)
    {
        tmp = math_sqrt(mathutil_sum_of_sq_2(effect->vel.x, effect->vel.z)) - 0.1f * size;
        if (tmp < 0.0f)
        {
            count = 0.0f;
            effect->vel.x *= 0.95f;
            effect->vel.y *= 0.95f;
            effect->vel.z *= 0.95f;
            effect->unk52 = 0.96f * effect->unk52;
            effect->unk54 = 0.96f * effect->unk54;
            effect->unk56 = 0.96f * effect->unk56;
        }
        else
        {
            count = 2.0f;
            effect->pos.y = size;
            if (effect->vel.y < 0.0f)
            {
                effect->vel.y *= -(0.4f + 0.3f * RAND_FLOAT());
                effect->vel.x *= 0.6f;
                effect->vel.z *= 0.6f;
                effect->unk52 >>= 1;
                effect->unk54 >>= 1;
                effect->unk56 >>= 1;
                tmp = 2048.0f * effect->vel.y;
                rx += tmp * (RAND_FLOAT() - 0.5f);
                ry += tmp * (RAND_FLOAT() - 0.5f);
                rz += tmp * (RAND_FLOAT() - 0.5f);
            }
        }
        if (count > 0.0f)
        {
            memset(&spawn, 0, sizeof(spawn));
            spawn.type = 0x31;
            while (count > 0.0f)
            {
                spawn.model = efcmdlGma->modelEntries[21].model;
                spawn.timer = 60.0f * (1.0f + 2.0f * RAND_FLOAT());
                spawn.colorFactor = 1.0f;
                spawn.pos.x = effect->pos.x + count * (size * (RAND_FLOAT() - 0.5f));
                spawn.pos.y = 0.0f;
                spawn.pos.z = effect->pos.z + count * (size * (RAND_FLOAT() - 0.5f));
                spawn.vel.x = 0.6f * effect->vel.x;
                spawn.vel.y = 0.0f;
                spawn.vel.z = 0.6f * effect->vel.z;
                spawn.unk8C.x = size * (0.35f + 0.7f * RAND_FLOAT());
                spawn.unk8C.y = size * (0.15f + 0.35f * RAND_FLOAT());
                spawn.unk98.x = 0.05f * spawn.unk8C.x;
                spawn.unk98.y = spawn.unk8C.y * (0.5f + 0.25f * RAND_FLOAT());
                spawn.rotZ = (rand() & 0x7FFF) - 0x7FFF;
                spawn.unk56 = 4096.0f * (RAND_FLOAT() - 0.5f);
                spawn_effect(&spawn);
                count -= 1.0f;
            }
        }
    }
    effect->unk52 = 0.98f * (effect->unk52 + rx);
    effect->unk54 = 0.98f * (effect->unk54 + ry);
    effect->unk56 = 0.98f * (effect->unk56 + rz);
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    if (effect->unkA4 > 0)
        effect->unkA4--;
    else
    {
        effect->unk18 = effect->unk18 + 0.1f * -effect->unk18;
        effect->unk1C = effect->unk1C + 0.11f * -effect->unk1C;
        effect->unk20 = effect->unk20 + 0.15f * -effect->unk20;
    }
    if (effect->timer > 0 && (float)effect->timer < 15.0f)
    {
        tmp = 1.0f / (effect->timer + 1);
        effect->colorFactor *= 1.0f - tmp;
    }
    else if (effect->colorFactor < 1.0f)
    {
        effect->colorFactor = effect->colorFactor + 0.15f * (1.1f - effect->colorFactor);
        if (effect->colorFactor > 1.0f)
            effect->colorFactor = 1.0f;
    }
}

void fn_1_155CE8(struct Effect *effect)
{
    struct GMAModel *model = effect->model;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    mtxa_translate_neg(&model->boundSphereCenter);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        float r = effect->unk18;
        float g = effect->unk1C;
        float b = effect->unk20;
        float a;

        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;
        g_stores_doubles2(r, g, b, 0.0f);
        a = effect->colorFactor;
        if (a < 1.0f)
        {
            set_post_mult_color(1.0f, 1.0f, 1.0f, a);
            avdisp_draw_model_unculled_sort_always(model);
            fade_color_base_default();
        }
        else
        {
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_unculled_sort_auto(model);
        }
        g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void fn_1_155E74(struct Effect *effect)
{
}

void fn_1_155E78(struct Effect *effect)
{
}

void fn_1_155E7C(struct Effect *effect)
{
    effect->scale.x = effect->scale.x + 0.11f * (effect->scale.y - effect->scale.x);
}

void fn_1_155EA8(struct Effect *effect)
{
    struct OrdTblNode *entry = fn_1_CCD5C(0);
    struct EffectOrdNode *node = ord_tbl_alloc_node(sizeof(*node));

    if (node != NULL)
    {
        node->node.drawFunc = fn_1_155F10;
        node->effect = effect;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_155F10(struct OrdTblNode *node_)
{
    Vec sp8;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    float scale;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mathutil_mtxA_get_translate(&sp8);
    if (!(sp8.z > -0.1f))
    {
        scale = g_current_camera->sub28.view.tanHalfFov * (effect->scale.x * -sp8.z) / model->boundSphereRadius;
        if (g_font_char_var_3 & 1)
            scale *= 0.95f;
        mtxa_scale_s(scale);
        avdisp_set_z_mode(1, 7, 0);
        set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
        mtxa_scale_s(0.125f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
        fade_color_base_default();
        avdisp_set_z_mode(1, 3, 1);
    }
}

void fn_1_156060(struct Effect *effect)
{
}

void fn_1_156064(struct Effect *effect)
{
    effect->scale.x = effect->unk8C.x;
    effect->scale.y = effect->unk8C.y;
    if (0.0f == effect->unk98.z)
        effect->unk98.z = -0.08f;
}

void fn_1_1560A0(struct Effect *effect)
{
    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.98f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk98.y += effect->unk98.z;
    effect->unk98.x *= 0.98f;
    effect->unk98.y *= 0.99f;
    effect->unk8C.x += effect->unk98.x;
    effect->unk8C.y += effect->unk98.y;
    if (effect->unk8C.y < 0.0f)
    {
        if ((float)effect->timer > 15.0f)
            effect->timer = 15;
    }
    else
    {
        effect->scale.x = effect->unk8C.x;
        effect->scale.y = effect->unk8C.y;
    }
    effect->unk56 = effect->unk56 - (effect->unk56 >> 6);
    effect->rotZ += effect->unk56;
    if ((float)effect->timer < 15.0f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
    }
    else
        effect->colorFactor = effect->colorFactor + 0.2f * (1.0f - effect->colorFactor);
}

void fn_1_156274(struct Effect *effect)
{
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb();
    entry = ord_tbl_get_entry_for_pos(&effect->pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_1_1562E0;
        node->effect = effect;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_1562E0(struct OrdTblNode *node_)
{
    Vec sp20;
    Vec light;
    Vec c;
    struct QuadBatchItem items[2];
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    struct QuadBatchItem *q;
    int count;
    u8 alpha;
    float r1, g1, b1;
    float r2, g2, b2;
    float t, u;

    light.x = 1.0f;
    light.y = 0.25f;
    light.z = 0.0f;
    r1 = 1.25f;
    g1 = 1.25f;
    b1 = 1.25f;
    r2 = 0.6f;
    g2 = 0.8f;
    b2 = 0.9f;
    q = items;
    count = 0;
    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, 2.0f * effect->scale.x))
    {
        mtxa_rotate_x(-0x4000);
        mtxa_rotate_z(effect->rotZ);
        q->scale = 2.0f * effect->scale.x;
        mtxa_to_mtx(q->mtx);
        q->color[0].r = 255;
        q->color[0].g = 255;
        q->color[0].b = 255;
        q->color[0].a = 192.0f * effect->colorFactor;
        q->color[1] = q->color[0];
        q->color[2] = q->color[0];
        q->color[3] = q->color[0];
        q++;
        count++;
    }
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rigid_inv_tf_tl(&sp20);
    mtxa_rotate_y(math_atan2(sp20.x, sp20.z));
    mtxa_scale_xyz(effect->scale.x, effect->scale.y, 1.0f);
    mtxa_translate_xyz(0.0f, 0.4f, 0.0f);
    if (g_is_sphere_visible(&model->boundSphereCenter, effect->scale.x > effect->scale.y ? effect->scale.x : effect->scale.y))
    {
        mtxa_rotate_z(effect->rotZ);
        q->scale = 1.0f;
        mtxa_to_mtx(q->mtx);
        alpha = 255.0f * effect->colorFactor;
        mtxa_tf_vec_xyz(-1.0f, -1.0f, 0.0f, &sp20);
        t = vec_dot_normalized_safe(&sp20, &light);
        t = 0.5f * (1.0f + t);
        u = 1.0f - t;
        c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
        c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
        c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
        f32_to_u8(&c.x, &q->color[0].r);
        f32_to_u8(&c.y, &q->color[0].g);
        f32_to_u8(&c.z, &q->color[0].b);
        q->color[0].a = alpha;
        c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
        c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
        c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
        f32_to_u8(&c.x, &q->color[2].r);
        f32_to_u8(&c.y, &q->color[2].g);
        f32_to_u8(&c.z, &q->color[2].b);
        q->color[2].a = alpha;
        mtxa_tf_vec_xyz(1.0f, -1.0f, 0.0f, &sp20);
        t = vec_dot_normalized_safe(&sp20, &light);
        t = 0.5f * (1.0f + t);
        u = 1.0f - t;
        c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
        c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
        c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
        f32_to_u8(&c.x, &q->color[1].r);
        f32_to_u8(&c.y, &q->color[1].g);
        f32_to_u8(&c.z, &q->color[1].b);
        q->color[1].a = alpha;
        c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
        c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
        c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
        f32_to_u8(&c.x, &q->color[3].r);
        f32_to_u8(&c.y, &q->color[3].g);
        f32_to_u8(&c.z, &q->color[3].b);
        q->color[3].a = alpha;
        count++;
    }
    if (count != 0)
    {
        opti_GXSetChanCtrl(0, 1, 1, 1, 0, 0, 2);
        opti_GXSetChanCtrl(2, 1, 1, 1, 0, 0, 2);
        GXSetNumChans_cached(1);
        GXSetCullMode_cached(0);
        GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
        g_GXSetTevOrder_wrapper(0, 0, 0, 4);
        GXSetTevSwapMode_cached(0, 0, 1);
        g_GXSetTevIndirect_zero_if_different(0);
        GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xA);
        GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
        GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
        GXSetNumTevStages_cached(1);
        GXSetNumTexGens_cached(1);
        GXSetZMode_cached(1, 3, 0);
        g_something_with_raster_state(0);
        GXSetBlendMode_cached(1, 4, 5, 0);
        GXLoadTexObj_cached(model->texObjs, 0);
        mtxa_from_identity();
        fn_1_16BEC0(count, items);
    }
}

void fn_1_15691C(struct Effect *effect)
{
}

void fn_1_156920(struct Effect *effect)
{
    effect->rotX = (rand() & 0x7FFF) - 0x7FFF;
    effect->rotY = (rand() & 0x7FFF) - 0x7FFF;
    effect->rotZ = (rand() & 0x1FFF) - 0x1000;
    effect->unk52 = (int)(16384.0f * (RAND_FLOAT() - 0.5f));
    effect->unk54 = (int)(16384.0f * (RAND_FLOAT() - 0.5f));
    effect->colorFactor = 1.0f;
}

void fn_1_156A34(struct Effect *effect)
{
    Vec dir;
    Vec prev;
    float d;
    float k;
    float sz = effect->scale.z;

    effect->scale.x = effect->scale.x + 0.2f * (effect->scale.y - effect->scale.x);
    prev = effect->vel;
    effect->vel.y += 0.0064f * sz;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
    d = mathutil_vec_dot_prod(&dir, &effect->vel);
    effect->vel.x -= d * dir.x;
    effect->vel.y -= d * dir.y;
    effect->vel.z -= d * dir.z;
    k = 0.99f;
    effect->vel.x *= k;
    effect->vel.y *= k;
    effect->vel.z *= k;
    k = 0.65f;
    d *= k;
    effect->vel.x += d * dir.x;
    effect->vel.y += d * dir.y;
    effect->vel.z += d * dir.z;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    if (effect->pos.y > effect->unk64.y)
    {
        effect->pos.y = effect->unk64.y;
        mathutil_vec_len(&effect->vel);
        effect->vel.y = 0.0f;
        if ((float)effect->timer > 30.0f)
            effect->timer = 30.0f * (0.5f + 0.5f * RAND_FLOAT());
    }
    {
        float a = 12288.0f * (effect->vel.x + effect->vel.y);
        float b = 12288.0f * (-effect->vel.y + effect->vel.z);
        float dx, dy, dz;

        effect->rotX += (s16)(0.97f * (effect->unk52 + a));
        effect->rotY += (s16)(0.97f * (effect->unk54 + b));
        dx = effect->vel.x - prev.x;
        dy = effect->vel.y - prev.y;
        dz = effect->vel.z - prev.z;
        effect->unk98.x = effect->unk98.x + 0.4f * (dx - effect->unk8C.x);
        effect->unk98.y = effect->unk98.y + 0.4f * (dy - effect->unk8C.y);
        effect->unk98.z = effect->unk98.z + 0.4f * (dz - effect->unk8C.z);
    }
    effect->unk98.x *= 0.98f;
    effect->unk98.y *= 0.98f;
    effect->unk98.z *= 0.98f;
    effect->unk8C.x += effect->unk98.x;
    effect->unk8C.y += effect->unk98.y;
    effect->unk8C.z += effect->unk98.z;
    if ((float)effect->timer < 7.5f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
    }
}

void fn_1_156E9C(struct Effect *effect)
{
    struct GMAModel *model = effect->model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    if (g_some_render_flag.flags & 4)
        return;
    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_156F48;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_156F48(struct OrdTblNode *node_)
{
    Vec sp8;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    float s = effect->scale.x;
    float m;
    s16 angle;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_tf_vec(&effect->unk8C, &sp8);
    mtxa_sq_from_identity();
    m = s / model->boundSphereRadius;
    s = s / (s + 0.75f * math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.y)));
    angle = math_atan2(sp8.y, sp8.x);
    mtxa_rotate_z(angle);
    mtxa_scale_xyz(m * s, m, m);
    mtxa_rotate_z(effect->rotZ - angle);
    avdisp_set_alpha(effect->colorFactor);
    avdisp_set_z_mode(1, 3, 0);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_unculled_sort_never(model);
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_157060(struct Effect *effect)
{
}

void fn_1_157064(struct Effect *effect)
{
    effect->rotZ = (rand() & 0x7FFF) - 0x7FFF;
    effect->unk56 = 1536.0f * (RAND_FLOAT() - 0.5f);
}

void fn_1_1570F8(struct Effect *effect)
{
    float sz = effect->scale.z;

    effect->vel.y += 0.0005f * sz;
    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.98f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->scale.y *= 0.995f;
    effect->scale.x += effect->scale.y;
    effect->unk56 = effect->unk56 - (effect->unk56 >> 6);
    effect->rotZ += effect->unk56;
    if ((float)effect->timer < 24.0f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
    }
    else
        effect->colorFactor = effect->colorFactor + 0.1f * (0.25f - effect->colorFactor);
}

void fn_1_15725C(struct Effect *effect)
{
    struct GMAModel *model = effect->model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    if (g_some_render_flag.flags & 4)
        return;
    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_157308;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_157308(struct OrdTblNode *node_)
{
    Vec sp8;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    float s = effect->scale.x;

    if (g_some_render_flag.flags & 4)
        return;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mathutil_mtxA_get_translate(&sp8);
    if (sp8.z < -(0.1f + s))
    {
        float f = (sp8.z + s) / sp8.z;

        sp8.x *= f;
        sp8.y *= f;
        sp8.z += s;
        mathutil_mtxA_set_translate(&sp8);
        mtxa_scale_s(s * f / model->boundSphereRadius);
    }
    else
        mtxa_scale_s(s / model->boundSphereRadius);
    mtxa_rotate_z(effect->rotZ);
    avdisp_set_alpha(effect->colorFactor);
    avdisp_set_z_mode(1, 3, 0);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_unculled_sort_never(model);
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_15746C(struct Effect *effect)
{
}

void fn_1_157470(struct Effect *effect)
{
}

void fn_1_157474(struct Effect *effect)
{
    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.98f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->rotZ += effect->unk56;
    if ((float)effect->timer < 7.5f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->scale.x *= 1.0f - r;
    }
    else
    {
        float r = 1.0f / (1.0f + ((float)effect->timer - 7.5f));

        effect->scale.y *= 1.0f - r;
        effect->scale.x += effect->scale.y;
    }
}

void fn_1_1575C4(struct Effect *effect)
{
    struct GMAModel *model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    if (g_some_render_flag.flags & 4)
        return;
    model = efcmdlGma->modelEntries[9].model;
    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_157680;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_157680(struct OrdTblNode *node_)
{
    Vec sp8;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    float s = effect->scale.x;
    float z;
    struct GMAModel *model;

    avdisp_set_z_mode(1, 3, 0);
    z = effect->scale.z;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mathutil_mtxA_get_translate(&sp8);
    if (sp8.z < -(0.1f + z))
    {
        float f = (sp8.z + z) / sp8.z;

        sp8.x *= f;
        sp8.y *= f;
        sp8.z += z;
        mathutil_mtxA_set_translate(&sp8);
        mtxa_scale_s(f);
    }
    model = efcmdlGma->modelEntries[9].model;
    mtxa_push();
    mtxa_scale_s(s / model->boundSphereRadius);
    mtxa_rotate_z(-effect->rotZ);
    set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_unculled_sort_never(model);
    fade_color_base_default();
    mtxa_pop();
    model = efcmdlGma->modelEntries[10].model;
    mtxa_scale_s(s / model->boundSphereRadius);
    mtxa_rotate_z(effect->unkA8);
    mtxa_scale(&effect->unk8C);
    mtxa_rotate_z(-effect->unkA8);
    mtxa_rotate_z(effect->rotZ);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_unculled_sort_never(model);
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_15785C(struct Effect *effect)
{
}

void fn_1_157860(struct Effect *effect)
{
    effect->rotZ = (rand() & 0x1FFF) - 0x1000;
}

void fn_1_15789C(struct Effect *effect)
{
    float sz = effect->scale.z;

    effect->vel.y += -0.001f * sz;
    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.98f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->rotZ += effect->unk56;
    if ((float)effect->timer < 7.5f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->scale.x *= 1.0f - r;
    }
    else
        effect->scale.x = effect->scale.x + 1.4f * (effect->scale.y - effect->scale.x);
}

void fn_1_1579C8(struct Effect *effect)
{
    struct GMAModel *model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    if (g_some_render_flag.flags & 4)
        return;
    model = efcmdlGma->modelEntries[9].model;
    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_157A84;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_157A84(struct OrdTblNode *node_)
{
    Vec sp8;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    float s = effect->scale.x;
    float t;
    struct GMAModel *model;
    s16 angle;

    avdisp_set_z_mode(1, 3, 0);
    model = efcmdlGma->modelEntries[33].model;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_tf_vec(&effect->vel, &sp8);
    mtxa_sq_from_identity();
    sp8.z = 0.0f;
    t = math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.y));
    angle = math_atan2(sp8.y, sp8.x);
    mtxa_rotate_z(angle);
    mtxa_scale_xyz(1.0f + 0.5f * t / s, 1.0f, 1.0f);
    mtxa_rotate_z(-angle);
    mtxa_push();
    mtxa_scale_s(s / model->boundSphereRadius);
    mtxa_rotate_z(effect->rotZ);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_unculled_sort_never(model);
    mtxa_pop();
    model = efcmdlGma->modelEntries[10].model;
    mtxa_translate_xyz(0.5f * s, 0.5f * s, 0.0f);
    mtxa_sq_from_identity();
    mathutil_mtxA_get_translate(&sp8);
    if (sp8.z < 0.0f)
    {
        t = 0.1f * s / effect->scale.y;
        mtxa_scale_s(g_current_camera->sub28.view.tanHalfFov * (0.5f * sp8.z * t) / model->boundSphereRadius);
        angle = (s16)(65536.0f * (0.5f * sp8.x + sp8.y) / (sp8.z * g_current_camera->sub28.view.tanHalfFov));
        mtxa_rotate_z(angle);
        set_post_mult_color(0.8f, 0.85f, 0.9f, 1.0f);
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_never(model);
        fade_color_base_default();
    }
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_157CE0(struct Effect *effect)
{
}

void fn_1_157CE4(struct Effect *effect)
{
}

void fn_1_157CE8(struct Effect *effect)
{
    struct Effect spawn;
    Mtx mtx;
    Vec newPos;
    Vec delta;
    Vec prevPos;
    Vec prevVel;
    Vec dir;
    float k;
    struct Ape *ape = get_ape_by_idx((int)(sptr)effect->model);
    int bone = effect->playerId;
    float sz = effect->scale.z;
    float ratio;

    if (ape == NULL || ape->unk88 != effect->u_otherTimer)
    {
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        return;
    }
    prevPos = effect->pos;
    prevVel = effect->vel;
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(mtx);
    mtxa_from_translate(&ape->pos);
    mtxa_scale_s(ape->scale);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(mtx);
    mtxa_mult_right(g_get_ape_anim_smth(ape, bone));
    mtxa_tf_point(&effect->unk8C, &newPos);
    mtxa_tf_vec(&effect->unk8C, &dir);
    vec_normalize_len(&dir);
    delta.x = newPos.x - prevPos.x;
    delta.y = newPos.y - prevPos.y;
    delta.z = newPos.z - prevPos.z;
    effect->pos = newPos;
    effect->vel = delta;
    k = 0.125f * effect->scale.x;
    effect->unk98.x = k * (RAND_FLOAT() - 0.5f);
    effect->unk98.y = k * (RAND_FLOAT() - 0.5f);
    effect->unk98.z = k * (RAND_FLOAT() - 0.5f);
    if ((float)effect->timer > 6.0f)
        effect->scale.x = effect->scale.x + 0.16f * (effect->scale.y - effect->scale.x);
    else
    {
        k = 1.0f / (effect->timer + 1);
        effect->scale.x *= 1.0f - k;
    }
    ratio = effect->scale.x / effect->scale.y;
    if (effect->unkA8 != 0 && ratio > 0.25f)
    {
        float s = 0.25f * effect->scale.x;
        float x, y, z;

        memset(&spawn, 0, sizeof(spawn));
        spawn.type = 56;
        spawn.pos = newPos;
        k = 0.25f * s * (1.0f + RAND_FLOAT());
        x = dir.x * k;
        y = dir.y * k;
        z = dir.z * k;
        spawn.vel.x = 0.5f * delta.x + x;
        spawn.vel.y = 0.5f * delta.y + y;
        spawn.vel.z = 0.5f * delta.z + z;
        k = 0.25f * mathutil_vec_len(&spawn.vel);
        spawn.vel.x += k * (RAND_FLOAT() - 0.5);
        spawn.vel.y += k * (RAND_FLOAT() - 0.5);
        spawn.vel.z += k * (RAND_FLOAT() - 0.5);
        spawn.unk98.x = 0.0f;
        spawn.unk98.y = 0.0001f * sz;
        spawn.unk98.z = 0.0f;
        spawn.scale.x = s * (0.25f + 0.25f * RAND_FLOAT());
        spawn.scale.y = 0.16f * spawn.scale.x;
        spawn.unk18 = 1.0f;
        spawn.unk1C = 1.0f;
        spawn.unk20 = 1.0f;
        spawn.rotZ = (rand() & 0x7FFF) - 0x7FFF;
        spawn_effect(&spawn);
    }
}

void fn_1_158260(struct Effect *effect)
{
    struct GMAModel *model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    if (g_some_render_flag.flags & 4)
        return;
    model = efcmdlGma->modelEntries[8].model;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_translate(&effect->unk98);
    if (g_is_sphere_visible(&model->boundSphereCenter, effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_158338;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
        effect->unkA8 = 1;
    }
    else
        effect->unkA8 = 0;
}

void fn_1_158338(struct OrdTblNode *node_)
{
    Vec sp8;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    float s = effect->scale.x;
    struct GMAModel *model;

    avdisp_set_z_mode(1, 3, 0);
    model = efcmdlGma->modelEntries[8].model;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_translate(&effect->unk98);
    mtxa_sq_from_identity();
    mathutil_mtxA_get_translate(&sp8);
    if (sp8.z < -(0.1f + 0.5f * s))
    {
        float f = (sp8.z + 0.5f * s) / sp8.z;

        sp8.x *= f;
        sp8.y *= f;
        sp8.z += 0.5f * s;
        mathutil_mtxA_set_translate(&sp8);
        mtxa_scale_s(f);
    }
    mtxa_scale_s(s / model->boundSphereRadius);
    set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_unculled_sort_never(model);
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_1584B4(struct Effect *effect)
{
}

void fn_1_1584B8(struct Effect *effect)
{
    struct SceneEffectJoint *joint = (struct SceneEffectJoint *)effect->model;

    if (joint != NULL)
    {
        mtxa_from_mtx(joint->mtx);
        mtxa_translate(&effect->unk8C);
        mathutil_mtxA_get_translate(&effect->pos);
    }
    effect->rotZ = (rand() & 0x7FFF) - 0x7FFF;
    effect->colorFactor = RAND_FLOAT();
}

void fn_1_158574(struct Effect *effect)
{
    struct Effect spawn;
    Vec prevPos;
    Vec prevVel;
    Vec pos;
    Vec vel;
    struct SceneEffectJoint *joint = (struct SceneEffectJoint *)effect->model;
    float count;
    float len;
    float total;

    prevPos = effect->pos;
    prevVel = effect->vel;
    if (joint != NULL)
    {
        struct AnimJoint *owner = joint->joints;

        mtxa_from_mtx(joint->mtx);
        mtxa_translate(&effect->unk8C);
        mathutil_mtxA_get_translate(&effect->pos);
        effect->vel.x = effect->pos.x - prevPos.x;
        effect->vel.y = effect->pos.y - prevPos.y;
        effect->vel.z = effect->pos.z - prevPos.z;
        effect->scale.x = effect->scale.y * owner->scale.x;
    }
    else
    {
        effect->vel.y += 0.004f;
        effect->vel.x *= 0.98f;
        effect->vel.y *= 0.98f;
        effect->vel.z *= 0.98f;
        effect->pos.x += effect->vel.x;
        effect->pos.y += effect->vel.y;
        effect->pos.z += effect->vel.z;
        effect->scale.x = effect->scale.y;
    }
    count = effect->scale.x;
    effect->unk98.x = count * (RAND_FLOAT() - 0.5f);
    effect->unk98.y = count * (RAND_FLOAT() - 0.5f);
    effect->unk98.z = count * (RAND_FLOAT() - 0.5f);
    effect->colorFactor += 2.0f * mathutil_vec_distance(&effect->pos, &prevPos) / effect->scale.y;
    count = effect->colorFactor;
    if (count > 1.0f)
    {
        pos = effect->pos;
        vel = effect->vel;
        memset(&spawn, 0, sizeof(spawn));
        spawn.type = 56;
        total = count;
        while (count > 1.0f)
        {
            float t = count / total;
            float u = 1.0f - t;

            spawn.pos.x = pos.x * t + prevPos.x * u;
            spawn.pos.y = pos.y * t + prevPos.y * u;
            spawn.pos.z = pos.z * t + prevPos.z * u;
            spawn.vel.x = 0.5f * (vel.x * t + prevVel.x * u);
            spawn.vel.y = 0.5f * (vel.y * t + prevVel.y * u);
            spawn.vel.z = 0.5f * (vel.z * t + prevVel.z * u);
            len = 0.025f * mathutil_vec_len(&spawn.vel);
            spawn.vel.x += len * (RAND_FLOAT() - 0.5);
            spawn.vel.y += len * (RAND_FLOAT() - 0.5);
            spawn.vel.z += len * (RAND_FLOAT() - 0.5);
            spawn.scale.x = effect->scale.x * (0.5f + RAND_FLOAT());
            spawn.scale.y = 0.04f * spawn.scale.x;
            *(Vec *)&spawn.unk18 = *(Vec *)&effect->unk18;
            spawn.rotZ = (rand() & 0x7FFF) - 0x7FFF;
            spawn_effect(&spawn);
            count -= 1.0f;
        }
        effect->colorFactor = count;
    }
}

void fn_1_158ABC(struct Effect *effect)
{
    Vec sp8;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb_translate(&effect->pos);
    sp8.x = 0.0f;
    sp8.y = 0.0f;
    sp8.z = 0.0f;
    if (g_is_sphere_visible(&sp8, 6.0f * effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3] + 5.0f * effect->scale.x);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_158B84;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_158B84(struct OrdTblNode *node_)
{
    Vec sp8;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model;
    float s;
    float f;

    avdisp_set_z_mode(1, 3, 0);
    model = efcmdlGma->modelEntries[2].model;
    s = effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_translate(&effect->unk98);
    mtxa_sq_from_identity();
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < 0.0f)
        {
            if (sp8.z < -(0.2f + s))
            {
                f = (sp8.z + s) / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z += s;
            }
            else
            {
                f = -0.2f / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
            set_post_mult_color(0.8f * effect->unk18, 0.8f * effect->unk1C, 0.8f * effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    model = efcmdlGma->modelEntries[6].model;
    s = 4.0f * effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_translate_neg(&effect->unk98);
    mtxa_sq_from_identity();
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < 0.0f)
        {
            if (sp8.z < -(0.2f + s))
            {
                f = (sp8.z + s) / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z += s;
            }
            else
            {
                f = -0.2f / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
            set_post_mult_color(0.4f * effect->unk18, 0.4f * effect->unk1C, 0.4f * effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    model = efcmdlGma->modelEntries[6].model;
    s = 6.0f * effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < 0.0f)
        {
            if (sp8.z < -(0.2f + s))
            {
                f = (sp8.z + s) / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z += s;
            }
            else
            {
                f = -0.2f / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
            set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    avdisp_set_z_mode(1, 3, 1);
    fade_color_base_default();
}

void fn_1_15902C(struct Effect *effect)
{
}

void fn_1_159030(struct Effect *effect)
{
    if (effect->timer == 0)
        effect->timer = 60.0f * (1.0f + RAND_FLOAT());
    effect->colorFactor = 1.0f;
    effect->unk56 = 384.0f * (0.5f + RAND_FLOAT());
    if (rand() & 1)
        effect->unk56 = -effect->unk56;
}

void fn_1_15912C(struct Effect *effect)
{
    effect->vel.x += effect->unk98.x;
    effect->vel.y += effect->unk98.y;
    effect->vel.z += effect->unk98.z;
    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.98f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->rotZ += (s16)(effect->unk56 * mathutil_vec_len(&effect->vel));
    effect->scale.y *= 0.98f;
    effect->scale.x += effect->scale.y;
    effect->unk18 = effect->unk18 + 0.1f * -effect->unk18;
    effect->unk1C = effect->unk1C + 0.1f * -effect->unk1C;
    effect->unk20 = effect->unk20 + 0.1f * -effect->unk20;
    if ((float)effect->timer < 15.0f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
    }
}

void fn_1_159330(struct Effect *effect)
{
    struct GMAModel *model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    if (g_some_render_flag.flags & 4)
        return;
    model = efcmdlGma->modelEntries[24].model;
    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_1593F4;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_1593F4(struct OrdTblNode *node_)
{
    Vec sp20;
    Vec light;
    Vec c;
    struct QuadDesc quad;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = efcmdlGma->modelEntries[21].model;
    float r1, g1, b1;
    float r2, g2, b2;
    float t, u;
    u8 alpha;

    light.x = 0.5f;
    light.y = 1.0f;
    light.z = 0.0f;
    r1 = 1.25f;
    g1 = 1.25f;
    b1 = 1.25f;
    r2 = 0.15f;
    g2 = 0.15f;
    b2 = 0.3f;
    opti_GXSetChanCtrl(0, 1, 1, 1, 0, 0, 2);
    opti_GXSetChanCtrl(2, 1, 1, 1, 0, 0, 2);
    GXSetNumChans_cached(1);
    GXSetCullMode_cached(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 4);
    GXSetTevSwapMode_cached(0, 0, 1);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xA);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetZMode_cached(1, 3, 0);
    g_something_with_raster_state(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXLoadTexObj_cached(model->texObjs, 0);
    quad.pos = effect->pos;
    quad.radius = effect->scale.x;
    quad.rot = effect->rotZ;
    alpha = 255.0f * effect->colorFactor;
    mtxa_from_mtxb();
    mtxa_rotate_z(quad.rot);
    mtxa_tf_vec_xyz(-1.0f, -1.0f, 0.0f, &sp20);
    t = vec_dot_normalized_safe(&sp20, &light);
    t = 0.5f * (1.0f + t);
    u = 1.0f - t;
    c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
    c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
    c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
    f32_to_u8(&c.x, &quad.color[0].r);
    f32_to_u8(&c.y, &quad.color[0].g);
    f32_to_u8(&c.z, &quad.color[0].b);
    quad.color[0].a = alpha;
    c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
    c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
    c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
    f32_to_u8(&c.x, &quad.color[2].r);
    f32_to_u8(&c.y, &quad.color[2].g);
    f32_to_u8(&c.z, &quad.color[2].b);
    quad.color[2].a = alpha;
    mtxa_tf_vec_xyz(1.0f, -1.0f, 0.0f, &sp20);
    t = vec_dot_normalized_safe(&sp20, &light);
    t = 0.5f * (1.0f + t);
    u = 1.0f - t;
    c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
    c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
    c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
    f32_to_u8(&c.x, &quad.color[1].r);
    f32_to_u8(&c.y, &quad.color[1].g);
    f32_to_u8(&c.z, &quad.color[1].b);
    quad.color[1].a = alpha;
    c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
    c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
    c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
    f32_to_u8(&c.x, &quad.color[3].r);
    f32_to_u8(&c.y, &quad.color[3].g);
    f32_to_u8(&c.z, &quad.color[3].b);
    quad.color[3].a = alpha;
    mtxa_from_mtxb();
    fn_1_16B608(&quad);
}

void fn_1_159940(struct Effect *effect)
{
}

void fn_1_159944(struct Effect *effect)
{
}

void fn_1_159948(struct Effect *effect)
{
    struct GMAModel *model = effect->model;
    float r;
    float f;

    effect->vel.x += effect->unk98.x;
    effect->vel.y += effect->unk98.y;
    effect->vel.z += effect->unk98.z;
    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.985f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    r = model->boundSphereRadius * effect->scale.x;
    if (effect->pos.y < effect->unk64.y + r)
    {
        effect->pos.y = effect->unk64.y + r;
        if (effect->vel.y < 0.0f)
        {
            f = fabs(effect->vel.y) / (0.25f * r);
            effect->unk52 >>= 1;
            effect->unk54 >>= 1;
            effect->unk56 >>= 1;
            effect->unk52 += (s16)(f * (256.0f * (RAND_FLOAT() - 0.5f)));
            effect->unk54 += (s16)(f * (128.0f * (RAND_FLOAT() - 0.5f)));
            effect->unk56 += (s16)(f * (64.0f * (RAND_FLOAT() - 0.5f)));
            f = 0.5 * fabs(effect->vel.y);
            effect->vel.x *= 0.8f;
            effect->vel.z *= 0.8f;
            effect->vel.x += f * (RAND_FLOAT() - 0.5f);
            effect->vel.z += f * (RAND_FLOAT() - 0.5f);
            effect->vel.y *= -(0.04f + (0.02f + RAND_FLOAT()));
        }
    }
    effect->unk52 = effect->unk52 - (effect->unk52 >> 7);
    effect->unk54 = effect->unk54 - (effect->unk54 >> 7);
    effect->unk56 = effect->unk56 - (effect->unk56 >> 8);
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    effect->scale.x = effect->scale.x + 0.1f * (effect->scale.y - effect->scale.x);
    if ((float)effect->timer < 15.0f)
    {
        f = 1.0f / (effect->timer + 1);
        effect->colorFactor *= 1.0f - f;
    }
}

void fn_1_159DA4(struct Effect *effect)
{
    struct GMAModel *model = effect->model;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
}

void fn_1_159E38(struct Effect *effect)
{
}

void fn_1_159E3C(struct Effect *effect)
{
}

void fn_1_159E40(struct Effect *effect)
{
    float r;

    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.985f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    r = 1.0f / (effect->timer + 1);
    effect->scale.x = effect->scale.x + r * (effect->scale.y - effect->scale.x);
    if (effect->timer < 6.0)
    {
        r = 1.0f / (effect->timer + 1);
        effect->colorFactor *= 1.0f - r;
    }
    else
        effect->colorFactor = effect->colorFactor + 0.2f * (1.0f - effect->colorFactor);
}

void fn_1_159F8C(struct Effect *effect)
{
    Vec sp8;
    struct GMAModel *model = effect->model;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    mtxa_translate(&effect->unk8C);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        float z = effect->scale.z;
        float f;

        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < -(0.1f + z))
        {
            f = (sp8.z + z) / sp8.z;
            sp8.x *= f;
            sp8.y *= f;
            sp8.z += z;
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
        }
        else if (sp8.z < 0.0f)
        {
            f = -0.1f / sp8.z;
            sp8.x *= f;
            sp8.y *= f;
            sp8.z = -0.1f;
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
        }
        f = effect->colorFactor;
        set_post_mult_color(f, f, f, 1.0f);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_always(model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void fn_1_15A150(struct Effect *effect)
{
}

void fn_1_15A154(struct Effect *effect)
{
    effect->unkA8 += rand() & 0x7FFF;
}

void fn_1_15A194(struct Effect *effect)
{
    float sz = effect->scale.z;

    effect->vel.y += -0.0024f * sz;
    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.985f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk56 = effect->unk56 - (effect->unk56 >> 7);
    effect->rotZ += effect->unk56;
    if ((float)effect->timer < 15.0f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
        effect->scale.x *= 1.12f;
        effect->vel.y *= 0.995f;
    }
    else
        effect->colorFactor = effect->colorFactor + 0.2f * (1.0f - effect->colorFactor);
}

void fn_1_15A2F8(struct Effect *effect)
{
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb();
    entry = ord_tbl_get_entry_for_pos(&effect->pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_1_15A364;
        node->effect = effect;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_15A364(struct OrdTblNode *node_)
{
    Vec sp20;
    Vec light;
    Vec c;
    struct QuadBatchItem items[2];
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    float r1, g1, b1;
    float r2, g2, b2;
    float s = effect->scale.x;
    float len;
    struct QuadBatchItem *q;
    int count;
    s16 angle;
    u8 alpha;
    float t, u;

    light.x = 1.0f;
    light.y = 0.25f;
    light.z = 0.0f;
    r1 = 1.125f;
    g1 = 1.125f;
    b1 = 1.125f;
    r2 = 0.8f * 0.9f;
    g2 = 0.8f * 0.9f;
    b2 = 0.95f * 0.9f;
    q = items;
    count = 0;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_tf_vec(&effect->vel, &sp20);
    mtxa_sq_from_identity();
    sp20.z = 0.0f;
    t = mathutil_sum_of_sq_2(sp20.x, sp20.y);
    t = math_sqrt(t);
    len = t;
    angle = math_atan2(sp20.y, sp20.x);
    mtxa_rotate_z(angle);
    len = 1.0f + 2.0f * (len / s);
    mtxa_scale_xyz(len, 1.0f, 1.0f);
    mtxa_rotate_z(-angle);
    if (g_is_sphere_visible(&model->boundSphereCenter, s * len))
    {
        q->scale = s;
        mtxa_rotate_z(effect->rotZ);
        mtxa_to_mtx(q->mtx);
        alpha = 255.0f * effect->colorFactor;
        mtxa_tf_vec_xyz(-1.0f, -1.0f, 0.0f, &sp20);
        t = vec_dot_normalized_safe(&sp20, &light);
        t = 0.5f * (1.0f + t);
        u = 1.0f - t;
        c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
        c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
        c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
        f32_to_u8(&c.x, &q->color[0].r);
        f32_to_u8(&c.y, &q->color[0].g);
        f32_to_u8(&c.z, &q->color[0].b);
        q->color[0].a = alpha;
        c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
        c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
        c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
        f32_to_u8(&c.x, &q->color[2].r);
        f32_to_u8(&c.y, &q->color[2].g);
        f32_to_u8(&c.z, &q->color[2].b);
        q->color[2].a = alpha;
        mtxa_tf_vec_xyz(1.0f, -1.0f, 0.0f, &sp20);
        t = vec_dot_normalized_safe(&sp20, &light);
        t = 0.5f * (1.0f + t);
        u = 1.0f - t;
        c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
        c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
        c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
        f32_to_u8(&c.x, &q->color[1].r);
        f32_to_u8(&c.y, &q->color[1].g);
        f32_to_u8(&c.z, &q->color[1].b);
        q->color[1].a = alpha;
        c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
        c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
        c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
        f32_to_u8(&c.x, &q->color[3].r);
        f32_to_u8(&c.y, &q->color[3].g);
        f32_to_u8(&c.z, &q->color[3].b);
        q->color[3].a = alpha;
        count++;
    }
    if (count != 0)
    {
        opti_GXSetChanCtrl(0, 1, 1, 1, 0, 0, 2);
        opti_GXSetChanCtrl(2, 1, 1, 1, 0, 0, 2);
        GXSetNumChans_cached(1);
        GXSetCullMode_cached(0);
        GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
        g_GXSetTevOrder_wrapper(0, 0, 0, 4);
        GXSetTevSwapMode_cached(0, 0, 1);
        g_GXSetTevIndirect_zero_if_different(0);
        GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xA);
        GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
        GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
        GXSetNumTevStages_cached(1);
        GXSetNumTexGens_cached(1);
        GXSetZMode_cached(1, 3, 0);
        g_something_with_raster_state(0);
        GXSetBlendMode_cached(1, 4, 5, 0);
        GXLoadTexObj_cached(model->texObjs, 0);
        mtxa_from_identity();
        fn_1_16BEC0(count, items);
    }
    q = items;
    count = 0;
    r1 = 1.0f;
    g1 = 1.0f;
    b1 = 1.0f;
    r2 = 0.0f;
    g2 = 0.064f;
    b2 = 0.128f;
    if (g_is_sphere_visible(&model->boundSphereCenter, s * len))
    {
        mtxa_from_mtx(q->mtx);
        mtxa_rotate_z(effect->unkA8);
        mtxa_to_mtx(q->mtx);
        len = 255.0f * effect->colorFactor;
        alpha = len;
        mtxa_tf_vec_xyz(-1.0f, -1.0f, 0.0f, &sp20);
        t = vec_dot_normalized_safe(&sp20, &light);
        t = 0.5f * (1.0f + t);
        u = 1.0f - t;
        c.x = len * (effect->unk18 + (r1 * t + r2 * u));
        c.y = len * (effect->unk1C + (g1 * t + g2 * u));
        c.z = len * (effect->unk20 + (b1 * t + b2 * u));
        f32_to_u8(&c.x, &q->color[0].r);
        f32_to_u8(&c.y, &q->color[0].g);
        f32_to_u8(&c.z, &q->color[0].b);
        q->color[0].a = alpha;
        c.x = len * (effect->unk18 + (r1 * u + r2 * t));
        c.y = len * (effect->unk1C + (g1 * u + g2 * t));
        c.z = len * (effect->unk20 + (b1 * u + b2 * t));
        f32_to_u8(&c.x, &q->color[2].r);
        f32_to_u8(&c.y, &q->color[2].g);
        f32_to_u8(&c.z, &q->color[2].b);
        q->color[2].a = alpha;
        mtxa_tf_vec_xyz(1.0f, -1.0f, 0.0f, &sp20);
        t = vec_dot_normalized_safe(&sp20, &light);
        t = 0.5f * (1.0f + t);
        u = 1.0f - t;
        c.x = len * (effect->unk18 + (r1 * t + r2 * u));
        c.y = len * (effect->unk1C + (g1 * t + g2 * u));
        c.z = len * (effect->unk20 + (b1 * t + b2 * u));
        f32_to_u8(&c.x, &q->color[1].r);
        f32_to_u8(&c.y, &q->color[1].g);
        f32_to_u8(&c.z, &q->color[1].b);
        q->color[1].a = alpha;
        c.x = len * (effect->unk18 + (r1 * u + r2 * t));
        c.y = len * (effect->unk1C + (g1 * u + g2 * t));
        c.z = len * (effect->unk20 + (b1 * u + b2 * t));
        f32_to_u8(&c.x, &q->color[3].r);
        f32_to_u8(&c.y, &q->color[3].g);
        f32_to_u8(&c.z, &q->color[3].b);
        q->color[3].a = alpha;
        count++;
    }
    if (count != 0)
    {
        model = efcmdlGma->modelEntries[35].model;
        opti_GXSetChanCtrl(0, 1, 1, 1, 0, 0, 2);
        opti_GXSetChanCtrl(2, 1, 1, 1, 0, 0, 2);
        GXSetNumChans_cached(1);
        GXSetCullMode_cached(0);
        GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
        g_GXSetTevOrder_wrapper(0, 0, 0, 4);
        GXSetTevSwapMode_cached(0, 0, 1);
        g_GXSetTevIndirect_zero_if_different(0);
        GXSetTevColorIn_cached(0, 0xF, 0xA, 8, 0xF);
        GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
        GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
        GXSetNumTevStages_cached(1);
        GXSetNumTexGens_cached(1);
        GXSetZMode_cached(1, 3, 0);
        g_something_with_raster_state(0);
        GXSetBlendMode_cached(1, 1, 1, 0);
        GXLoadTexObj_cached(model->texObjs, 0);
        mtxa_from_identity();
        fn_1_16BEC0(count, items);
    }
}

void fn_1_15ADC4(struct Effect *effect)
{
}

void fn_1_15ADC8(struct Effect *effect)
{
}

void fn_1_15ADCC(struct Effect *effect)
{
    effect->vel.x = effect->vel.x + 0.01f * (effect->unk8C.x - effect->vel.x);
    effect->vel.z = effect->vel.z + 0.01f * (effect->unk8C.z - effect->vel.z);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->scale.x += effect->scale.y;
    effect->rotZ += effect->unk56;
    if ((float)effect->timer > 30.0f)
        effect->colorFactor = effect->colorFactor + 0.05f * (1.0f - effect->colorFactor);
    else
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
    }
}

void fn_1_15AF00(struct Effect *effect)
{
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb();
    if (g_is_sphere_visible(&effect->pos, effect->scale.x))
    {
        entry = ord_tbl_get_entry_for_pos(&effect->pos);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_15AF80;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_15AF80(struct OrdTblNode *node_)
{
    Vec sp20;
    Vec light;
    Vec c;
    struct QuadDesc quad;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    float r1, g1, b1;
    float r2, g2, b2;
    float t, u;
    u8 alpha;

    light.x = 0.5f;
    light.y = 1.0f;
    light.z = 0.0f;
    r1 = 1.25f;
    g1 = 1.25f;
    b1 = 1.25f;
    r2 = 0.25f;
    g2 = 0.25f;
    b2 = 0.35f;
    opti_GXSetChanCtrl(0, 1, 1, 1, 0, 0, 2);
    opti_GXSetChanCtrl(2, 1, 1, 1, 0, 0, 2);
    GXSetNumChans_cached(1);
    GXSetCullMode_cached(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 4);
    GXSetTevSwapMode_cached(0, 0, 1);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xA);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetZMode_cached(1, 3, 0);
    fn_1_E2F1C();
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXLoadTexObj_cached(model->texObjs, 0);
    quad.pos = effect->pos;
    quad.radius = effect->scale.x;
    quad.rot = effect->rotZ;
    alpha = 255.0f * effect->colorFactor;
    mtxa_from_mtxb();
    mtxa_sq_from_identity();
    mtxa_rotate_z(quad.rot);
    mtxa_tf_vec_xyz(-1.0f, -1.0f, 0.0f, &sp20);
    t = vec_dot_normalized_safe(&sp20, &light);
    t = 0.5f * (1.0f + t);
    u = 1.0f - t;
    c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
    c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
    c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
    f32_to_u8(&c.x, &quad.color[0].r);
    f32_to_u8(&c.y, &quad.color[0].g);
    f32_to_u8(&c.z, &quad.color[0].b);
    quad.color[0].a = alpha;
    c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
    c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
    c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
    f32_to_u8(&c.x, &quad.color[2].r);
    f32_to_u8(&c.y, &quad.color[2].g);
    f32_to_u8(&c.z, &quad.color[2].b);
    quad.color[2].a = alpha;
    mtxa_tf_vec_xyz(1.0f, -1.0f, 0.0f, &sp20);
    t = vec_dot_normalized_safe(&sp20, &light);
    t = 0.5f * (1.0f + t);
    u = 1.0f - t;
    c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
    c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
    c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
    f32_to_u8(&c.x, &quad.color[1].r);
    f32_to_u8(&c.y, &quad.color[1].g);
    f32_to_u8(&c.z, &quad.color[1].b);
    quad.color[1].a = alpha;
    c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
    c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
    c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
    f32_to_u8(&c.x, &quad.color[3].r);
    f32_to_u8(&c.y, &quad.color[3].g);
    f32_to_u8(&c.z, &quad.color[3].b);
    quad.color[3].a = alpha;
    mtxa_from_mtxb();
    fn_1_16B608(&quad);
}

void fn_1_15B4BC(struct Effect *effect)
{
}

void fn_1_15B4C0(struct Effect *effect)
{
    float k;

    effect->unkA8 += rand() & 0x7FFF;
    k = 0.25f + 0.5f * RAND_FLOAT();
    effect->unk18 = 0.6f * k;
    effect->unk1C = 0.95f * 0.6f * k;
    effect->unk20 = 0.8f * 0.6f * k;
    k = 0.8f + 0.2f * RAND_FLOAT();
    effect->unk8C.x = 0.6f * k;
    effect->unk8C.y = 0.95f * 0.6f * k;
    effect->unk8C.z = 0.8f * 0.6f * k;
}

void fn_1_15B5C8(struct Effect *effect)
{
    float sz = effect->scale.z;

    effect->vel.y += -0.0005f * sz;
    effect->vel.x *= 0.975f;
    effect->vel.y *= 0.975f;
    effect->vel.z *= 0.975f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk18 = effect->unk18 + 0.1f * (effect->unk8C.x - effect->unk18);
    effect->unk1C = effect->unk1C + 0.1f * (effect->unk8C.y - effect->unk1C);
    effect->unk20 = effect->unk20 + 0.1f * (effect->unk8C.z - effect->unk20);
    effect->unk56 = effect->unk56 - (effect->unk56 >> 7);
    effect->rotZ += effect->unk56;
    effect->scale.x += effect->scale.y;
    if ((float)effect->timer < 30.0f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
        effect->scale.y *= 1.02f;
    }
    else
        effect->colorFactor = effect->colorFactor + 0.2f * (1.0f - effect->colorFactor);
}

void fn_1_15B78C(struct Effect *effect)
{
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb();
    entry = ord_tbl_get_entry_for_pos(&effect->pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_1_15B7F8;
        node->effect = effect;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_15B7F8(struct OrdTblNode *node_)
{
    Vec sp20;
    Vec light;
    Vec c;
    struct QuadBatchItem items[2];
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    float r1, g1, b1;
    float r2, g2, b2;
    float s = effect->scale.x;
    float len;
    struct QuadBatchItem *q;
    int count;
    s16 angle;
    u8 alpha;
    float t, u;

    light.x = 1.0f;
    light.y = 0.5f;
    light.z = 0.0f;
    r1 = effect->unk18;
    g1 = effect->unk1C;
    b1 = effect->unk20;
    r2 = 0.8f * r1;
    g2 = 0.7f * g1;
    b2 = 0.7f * b1;
    q = items;
    count = 0;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_tf_vec(&effect->vel, &sp20);
    mtxa_sq_from_identity();
    sp20.z = 0.0f;
    t = mathutil_sum_of_sq_2(sp20.x, sp20.y);
    t = math_sqrt(t);
    len = t;
    angle = math_atan2(sp20.y, sp20.x);
    mtxa_rotate_z(angle);
    len = 1.0f + 2.0f * (len / s);
    mtxa_scale_xyz(len, 1.0f, 1.0f);
    mtxa_rotate_z(-angle);
    if (g_is_sphere_visible(&model->boundSphereCenter, s * len))
    {
        q->scale = s;
        mtxa_rotate_z(effect->rotZ);
        mtxa_to_mtx(q->mtx);
        alpha = 255.0f * effect->colorFactor;
        mtxa_tf_vec_xyz(-1.0f, -1.0f, 0.0f, &sp20);
        t = vec_dot_normalized_safe(&sp20, &light);
        t = 0.5f * (1.0f + t);
        u = 1.0f - t;
        c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
        c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
        c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
        f32_to_u8(&c.x, &q->color[0].r);
        f32_to_u8(&c.y, &q->color[0].g);
        f32_to_u8(&c.z, &q->color[0].b);
        q->color[0].a = alpha;
        c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
        c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
        c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
        f32_to_u8(&c.x, &q->color[2].r);
        f32_to_u8(&c.y, &q->color[2].g);
        f32_to_u8(&c.z, &q->color[2].b);
        q->color[2].a = alpha;
        mtxa_tf_vec_xyz(1.0f, -1.0f, 0.0f, &sp20);
        t = vec_dot_normalized_safe(&sp20, &light);
        t = 0.5f * (1.0f + t);
        u = 1.0f - t;
        c.x = 255.0f * (effect->unk18 + (r1 * t + r2 * u));
        c.y = 255.0f * (effect->unk1C + (g1 * t + g2 * u));
        c.z = 255.0f * (effect->unk20 + (b1 * t + b2 * u));
        f32_to_u8(&c.x, &q->color[1].r);
        f32_to_u8(&c.y, &q->color[1].g);
        f32_to_u8(&c.z, &q->color[1].b);
        q->color[1].a = alpha;
        c.x = 255.0f * (effect->unk18 + (r1 * u + r2 * t));
        c.y = 255.0f * (effect->unk1C + (g1 * u + g2 * t));
        c.z = 255.0f * (effect->unk20 + (b1 * u + b2 * t));
        f32_to_u8(&c.x, &q->color[3].r);
        f32_to_u8(&c.y, &q->color[3].g);
        f32_to_u8(&c.z, &q->color[3].b);
        q->color[3].a = alpha;
        count++;
    }
    if (count != 0)
    {
        opti_GXSetChanCtrl(0, 1, 1, 1, 0, 0, 2);
        opti_GXSetChanCtrl(2, 1, 1, 1, 0, 0, 2);
        GXSetNumChans_cached(1);
        GXSetCullMode_cached(0);
        GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
        g_GXSetTevOrder_wrapper(0, 0, 0, 4);
        GXSetTevSwapMode_cached(0, 0, 1);
        g_GXSetTevIndirect_zero_if_different(0);
        GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xA);
        GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
        GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
        GXSetNumTevStages_cached(1);
        GXSetNumTexGens_cached(1);
        GXSetZMode_cached(1, 3, 0);
        g_something_with_raster_state(0);
        GXSetBlendMode_cached(1, 4, 5, 0);
        GXLoadTexObj_cached(model->texObjs, 0);
        mtxa_from_identity();
        fn_1_16BEC0(count, items);
    }
}

void fn_1_15BDF0(struct Effect *effect)
{
}

void fn_1_15BDF4(struct Effect *effect)
{
    effect->unk98.x = 30.0f * effect->scale.x;
    effect->unk98.z = 12.5f;
    effect->state = 0;
}

void fn_1_15BE28(struct Effect *effect)
{
    struct Effect spawn;
    Vec rnd;
    Vec dir;
    Vec pt;
    float k = 0.1f * effect->scale.x;
    float d;
    int i;

    effect->vel.x = k * (RAND_FLOAT() - 0.5f);
    effect->vel.y = k * (RAND_FLOAT() - 0.5f);
    effect->vel.z = k * (RAND_FLOAT() - 0.5f);
    switch (effect->state)
    {
    case 0:
        effect->state = 1;
        effect->u_otherTimer = 30;
    case 1:
        effect->u_otherTimer--;
        if (effect->u_otherTimer < 0)
            effect->state = 2;
        mtxa_from_rotate_y(0);
        mtxa_rotate_x(-0x4000);
        mtxa_rotate_z(0);
        mtxa_tf_point_xyz(0.0f, 0.0f, -1.0f, &dir);
        d = effect->unk98.z;
        pt.x = effect->pos.x + dir.x * d;
        pt.y = effect->pos.y + dir.y * d;
        pt.z = effect->pos.z + dir.z * d;
        memset(&spawn, 0, sizeof(spawn));
        spawn.type = 63;
        spawn.model = efcmdlGma->modelEntries[8].model;
        for (i = 2; i > 0; i--)
        {
            rnd.x = RAND_FLOAT() - 0.5f;
            rnd.y = RAND_FLOAT() - 0.5f;
            rnd.z = RAND_FLOAT() - 0.5f;
            d = -mathutil_vec_dot_prod(&rnd, &dir);
            rnd.x += d * dir.x;
            rnd.y += d * dir.y;
            rnd.z += d * dir.z;
            if (0.0f == vec_normalize_len(&rnd))
                rnd.z = -1.0f;
            spawn.u_otherTimer = 60.0f * (0.5f + 0.25f * RAND_FLOAT());
            spawn.scale.x = effect->scale.x * (0.5f + RAND_FLOAT());
            d = effect->unk98.x;
            spawn.pos = pt;
            spawn.pos.x += rnd.x * d;
            spawn.pos.y += rnd.y * d;
            spawn.pos.z += rnd.z * d;
            d = -0.1f * effect->unk98.x;
            spawn.vel.x = rnd.x * d;
            spawn.vel.y = rnd.y * d;
            spawn.vel.z = rnd.z * d;
            spawn.unk8C = effect->pos;
            *(Vec *)&spawn.unk18 = *(Vec *)&effect->unk18;
            spawn.unk18 *= 0.75f + 0.25f * RAND_FLOAT();
            spawn.unk1C *= 0.75f + 0.25f * RAND_FLOAT();
            spawn.unk20 *= 0.75f + 0.25f * RAND_FLOAT();
            spawn_effect(&spawn);
        }
        break;
    case 2:
        effect->state = 3;
        effect->u_otherTimer = 30;
    case 3:
        effect->u_otherTimer--;
        if (effect->u_otherTimer < 0)
        {
            effect->state = 0;
            effect->unk98.z = 25.0f * (3.0f * RAND_FLOAT());
        }
        break;
    }
}

void fn_1_15C3E4(struct Effect *effect)
{
    Vec sp8;
    struct GMAModel *model;
    float s;
    float f;

    avdisp_set_z_mode(1, 3, 0);
    model = effect->model;
    s = effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_translate(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        set_post_mult_color(0.8f * effect->unk18, 0.8f * effect->unk1C, 0.8f * effect->unk20, 1.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
    model = effect->model;
    s = 4.0f * effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_translate(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < 0.0f)
        {
            if (sp8.z < -(0.2f + s))
            {
                f = (sp8.z + s) / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z += s;
            }
            else
            {
                f = -0.2f / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
            set_post_mult_color(0.4f * effect->unk18, 0.4f * effect->unk1C, 0.4f * effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    model = efcmdlGma->modelEntries[6].model;
    s = 6.0f * effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_translate_neg(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < 0.0f)
        {
            if (sp8.z < -(0.2f + s))
            {
                f = (sp8.z + s) / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z += s;
            }
            else
            {
                f = -0.2f / sp8.z;
                sp8.x *= f;
                sp8.y *= f;
                sp8.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
            set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    avdisp_set_z_mode(1, 3, 1);
    fade_color_base_default();
}

void fn_1_15C79C(struct Effect *effect)
{
}

void fn_1_15C7A0(struct Effect *effect)
{
    effect->scale.y = effect->scale.x;
    effect->scale.x = 0.0f;
}

void fn_1_15C7BC(struct Effect *effect)
{
    Vec r;
    Vec d;
    Vec p;
    Vec tgt;
    Vec v;
    float k;

    if (effect->u_otherTimer > 0)
    {
        effect->vel.x *= 0.9f;
        effect->vel.y *= 0.9f;
        effect->vel.z *= 0.9f;
        effect->scale.x = effect->scale.x + 0.11f * (effect->scale.y - effect->scale.x);
        effect->u_otherTimer--;
        if (effect->u_otherTimer <= 0)
        {
            effect->timer = 60.0f * (0.2f + 0.3f * RAND_FLOAT());
            k = 0.5f * effect->scale.y;
            r.x = k * (RAND_FLOAT() - 0.5f);
            r.y = k * (RAND_FLOAT() - 0.5f);
            r.z = k * (RAND_FLOAT() - 0.5f);
            d.x = effect->unk8C.x - effect->pos.x;
            d.y = effect->unk8C.y - effect->pos.y;
            d.z = effect->unk8C.z - effect->pos.z;
            vec_normalize_len(&d);
            k = -mathutil_vec_dot_prod(&r, &d);
            r.x += k * d.x;
            r.y += k * d.y;
            r.z += k * d.z;
            effect->vel.x += r.x;
            effect->vel.y += r.y;
            effect->vel.z += r.z;
            effect->scale.x *= 3.0f;
        }
    }
    else
    {
        float ax;
        float t;
        float dt;
        float ay;
        float a;
        float az;
        float t2;
        float b;

        t = effect->timer;
        t2 = t * t;
        a = 0.5f / t;
        b = 0.5f / t2;
        p = effect->pos;
        tgt = effect->unk8C;
        v = effect->vel;
        dt = tgt.x - p.x;
        ax = dt * b - v.x * a;
        dt = tgt.y - p.y;
        ay = dt * b - v.y * a;
        dt = tgt.z - p.z;
        az = dt * b - v.z * a;
        effect->vel.x += ax;
        effect->vel.y += ay;
        effect->vel.z += az;
        effect->scale.x = effect->scale.x + 0.1f * -effect->scale.x;
    }
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    k = 0.1f * effect->scale.x;
    effect->pos.x += k * (RAND_FLOAT() - 0.5f);
    effect->pos.y += k * (RAND_FLOAT() - 0.5f);
    effect->pos.z += k * (RAND_FLOAT() - 0.5f);
}

void fn_1_15CC70(struct Effect *effect)
{
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb_translate(&effect->pos);
    entry = fn_1_CCC18(mtxa->mtxA[2][3]);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_1_15CCEC;
        node->effect = effect;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_15CCEC(struct OrdTblNode *node_)
{
    Vec b;
    Vec a;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    float s = effect->scale.x;
    float dx;
    register float dy;
    float f;
    s16 angle;

    if (s <= 0.0f)
        return;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[3]);
    mtxa_tf_point(&effect->prevPos, &a);
    mtxa_from_mtxb_translate(&effect->pos);
    mathutil_mtxA_get_translate(&b);
    if (b.z > -g_current_camera->sub28.view.nearZ || a.z > -g_current_camera->sub28.view.nearZ)
        return;
    f = b.z / a.z;
    a.x *= f;
    a.y *= f;
    dx = b.x - a.x;
    dy = b.y - a.y;
    mtxa_sq_from_identity();
    {
        register float q = dx;

#ifdef __MWERKS__
        asm
        {
            fmuls q, q, q
            fmadds q, dy, dy, q
        }
#else
        q = q * q + dy * dy;
#endif
        f = math_sqrt(q);
    }
    angle = math_atan2(dy, dx);
    mtxa_rotate_z(angle);
    mtxa_scale_xyz(1.0f + f / s, 1.0f, 1.0f);
    mtxa_rotate_z(-angle);
    if ((g_font_char_var_3 + effect->poolIndex) & 1)
        s *= 1.2f;
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
        avdisp_set_z_mode(1, 3, 0);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void fn_1_15CF4C(struct Effect *effect)
{
}

void fn_1_15CF50(struct Effect *effect)
{
}

void fn_1_15CF54(struct Effect *effect)
{
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
}

void fn_1_15CF88(struct Effect *effect)
{
    Vec sp8;
    struct GMAModel *model = effect->model;
    float scale;
    float t;
    float f;
    float r, g, b;

    avdisp_set_z_mode(1, 3, 0);
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp8);
    t = (float)fabs(sp8.z);
    mtxa_from_mtxb();
    mtxa_mult_right(((struct SceneEffectJoint *)model)->mtx);
    mtxa_translate(&effect->unk8C);
    model = efcmdlGma->modelEntries[8].model;
    scale = 0.5f * (1.0f + t) * effect->scale.x;
    if (g_is_sphere_visible(&model->boundSphereCenter, scale))
    {
        mtxa_push();
        mtxa_sq_from_identity();
        mtxa_scale_s(scale / model->boundSphereRadius);
        scale = effect->scale.x;
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < -(0.1f + scale))
        {
            f = (sp8.z + scale) / sp8.z;
            sp8.x *= f;
            sp8.y *= f;
            sp8.z += scale;
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
        }
        f = t;
        r = effect->unk18 * f;
        g = effect->unk1C * f;
        b = effect->unk20 * f;
        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;
        set_post_mult_color(r, g, b, 1.0f);
        avdisp_draw_model_unculled_sort_always(model);
        mtxa_pop();
    }
    model = efcmdlGma->modelEntries[10].model;
    t = t * (t * t);
    scale = t * effect->scale.x;
    if (g_is_sphere_visible(&model->boundSphereCenter, scale))
    {
        mtxa_sq_from_identity();
        mtxa_scale_s(scale / model->boundSphereRadius);
        scale = effect->scale.x;
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < -(0.1f + scale))
        {
            f = (sp8.z + scale) / sp8.z;
            sp8.x *= f;
            sp8.y *= f;
            sp8.z += scale;
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(f);
        }
        mtxa_scale_xyz(1.0f, 0.75f, 1.0f);
        if (sp8.z < 0.0f)
            mtxa_rotate_z(16384.0f * (1.2f * sp8.x + sp8.y) / sp8.z);
        f = t;
        r = effect->unk18 * f;
        g = effect->unk1C * f;
        b = effect->unk20 * f;
        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;
        set_post_mult_color(r, g, b, 1.0f);
        avdisp_draw_model_unculled_sort_always(model);
    }
    avdisp_set_z_mode(1, 3, 1);
    fade_color_base_default();
}

void fn_1_15D348(struct Effect *effect)
{
}

void fn_1_15D34C(struct Effect *effect)
{
    effect->unk98.x = 10.0f * effect->scale.x;
    effect->unk98.y = 25.0f;
    effect->unk98.z = 12.5f;
    effect->state = 0;
    effect->scale.y = effect->scale.x;
    effect->scale.z = effect->scale.y;
    effect->colorFactor = RAND_FLOAT();
}

static inline void get_ring_point(struct Effect *effect, Vec *outPos, Vec *outDir)
{
    Vec d;
    Vec p;
    float r;

    mtxa_from_rotate_y(0);
    mtxa_rotate_x(-0x4000);
    mtxa_rotate_z(0);
    mtxa_tf_point_xyz(0.0f, 0.0f, -1.0f, &d);
    r = effect->unk98.y;
    p.x = effect->pos.x + d.x * r;
    p.y = effect->pos.y + d.y * r;
    p.z = effect->pos.z + d.z * r;
    if (outPos != NULL)
        *outPos = p;
    if (outDir != NULL)
        *outDir = d;
}

void fn_1_15D3E8(struct Effect *effect)
{
    struct Effect spawn;
    Vec w;
    Vec v;
    Vec dir;
    Vec pos;
    float k = 0.2f * effect->scale.x;
    int i;

    effect->vel.x = k * (RAND_FLOAT() - 0.5f);
    effect->vel.y = k * (RAND_FLOAT() - 0.5f);
    effect->vel.z = k * (RAND_FLOAT() - 0.5f);
    switch (effect->state)
    {
    case 0:
        effect->state = 1;
        effect->u_otherTimer = 30;
    case 1:
        effect->u_otherTimer--;
        if (effect->u_otherTimer < 0)
            effect->state = 2;
        get_ring_point(effect, &pos, &dir);
        memset(&spawn, 0, sizeof(spawn));
        spawn.type = 63;
        spawn.model = efcmdlGma->modelEntries[8].model;
        for (i = 2; i > 0; i--)
        {
            k = 65536.0f * (effect->u_otherTimer + 0.5f * i) / 30.0f;
            mtxa_from_rotate_y(0);
            mtxa_rotate_x(-0x4000);
            mtxa_rotate_z((int)k);
            mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &w);
            spawn.u_otherTimer = 60.0f * (0.55f + 0.25f * RAND_FLOAT());
            spawn.scale.x = effect->scale.x * (0.5f + 0.5f * RAND_FLOAT());
            spawn.pos = effect->pos;
            k = 0.5f * effect->unk98.x;
            v.x = pos.x + w.x * k - spawn.pos.x;
            v.y = pos.y + w.y * k - spawn.pos.y;
            v.z = pos.z + w.z * k - spawn.pos.z;
            vec_set_len(0.05f * effect->unk98.x, &v, &spawn.vel);
            spawn.unk8C = effect->pos;
            k = effect->unk98.y + 0.20833334f * spawn.u_otherTimer;
            spawn.unk8C.x += k * dir.x;
            spawn.unk8C.y += k * dir.y;
            spawn.unk8C.z += k * dir.z;
            *(Vec *)&spawn.unk18 = *(Vec *)&effect->unk18;
            spawn.unk18 *= 0.6f + 0.4f * RAND_FLOAT();
            spawn.unk1C *= 0.6f + 0.4f * RAND_FLOAT();
            spawn.unk20 *= 0.6f + 0.4f * RAND_FLOAT();
            spawn_effect(&spawn);
        }
        break;
    case 2:
        effect->state = 3;
        effect->u_otherTimer = 30;
    case 3:
        effect->u_otherTimer--;
        if (effect->u_otherTimer < 0)
        {
            effect->state = 0;
            effect->unk98.z = effect->unk98.y * (0.5f * (0.5f + RAND_FLOAT()));
            effect->scale.z += effect->scale.x;
        }
        break;
    }
    k = effect->scale.y - 0.8f * effect->scale.x;
    if (k > 0.0f)
        effect->colorFactor = effect->colorFactor + 0.5f * k / effect->scale.x;
    if (effect->colorFactor > 1.0f)
    {
        memset(&spawn, 0, sizeof(spawn));
        spawn.type = 66;
        spawn.playerId = effect->uid;
        while (effect->colorFactor > 1.0f)
        {
            spawn.timer = 60.0f * (0.25f + RAND_FLOAT());
            if (rand() & 1)
            {
                spawn.unk8C.x = 0.0f;
                spawn.unk8C.y = 0.0f;
                spawn.unk8C.z = -5.3f;
                spawn.unk98.x = 42.0f;
                spawn.unk98.y = 0.0f;
                spawn.unk98.z = 0.0f;
            }
            else
            {
                spawn.unk8C.x = 0.0f;
                spawn.unk8C.y = 0.0f;
                spawn.unk8C.z = 66.2f;
                spawn.unk98.x = 41.0f;
                spawn.unk98.y = 0.0f;
                spawn.unk98.z = 0.0f;
            }
            spawn.unkA4 = -0x4000;
            spawn.unkA6 = 0;
            spawn.unkA8 = 0;
            spawn.rotZ = 65536.0f * (RAND_FLOAT() - 0.5f);
            spawn.unk56 = 384.0f * (RAND_FLOAT() - 0.5f);
            spawn.scale.x = effect->scale.x * (0.5f + 0.5f * RAND_FLOAT());
            *(Vec *)&spawn.unk18 = *(Vec *)&effect->unk18;
            spawn.unk18 *= 0.7f + 0.3f * RAND_FLOAT();
            spawn.unk1C *= 0.7f + 0.3f * RAND_FLOAT();
            spawn.unk20 *= 0.7f + 0.3f * RAND_FLOAT();
            spawn_effect(&spawn);
            effect->colorFactor -= 1.0f;
        }
    }
    effect->unk98.y += 0.20833334f;
    effect->scale.y = effect->scale.y + 0.1f * (effect->scale.z - effect->scale.y);
}

void fn_1_15DD40(struct Effect *effect)
{
    Vec pos;
    struct GMAModel *model = effect->model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    get_ring_point(effect, &pos, NULL);
    mtxa_from_mtxb_translate(&pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_15DE84;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_15DE84(struct OrdTblNode *node_)
{
    struct LineVertex verts[32];
    Vec sp3C;
    Vec end;
    Vec start;
    Vec bend;
    Vec col;
    GXColor c;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model;
    int count;
    struct LineVertex *v;
    int i;
    float x, y;
    float s;
    float f;
    float step;
    float w2;
    float w1;
    float t, u;

    avdisp_set_z_mode(1, 3, 0);
    model = effect->model;
    s = effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_translate(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        set_post_mult_color(0.8f * effect->unk18, 0.8f * effect->unk1C, 0.8f * effect->unk20, 1.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
    model = effect->model;
    s = 4.0f * effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_translate(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp3C);
        if (sp3C.z < 0.0f)
        {
            if (sp3C.z < -(0.2f + s))
            {
                f = (sp3C.z + s) / sp3C.z;
                sp3C.x *= f;
                sp3C.y *= f;
                sp3C.z += s;
            }
            else
            {
                f = -0.2f / sp3C.z;
                sp3C.x *= f;
                sp3C.y *= f;
                sp3C.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp3C);
            mtxa_scale_s(f);
            set_post_mult_color(0.4f * effect->unk18, 0.4f * effect->unk1C, 0.4f * effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    model = efcmdlGma->modelEntries[6].model;
    s = 6.0f * effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_translate_neg(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp3C);
        if (sp3C.z < 0.0f)
        {
            if (sp3C.z < -(0.2f + s))
            {
                f = (sp3C.z + s) / sp3C.z;
                sp3C.x *= f;
                sp3C.y *= f;
                sp3C.z += s;
            }
            else
            {
                f = -0.2f / sp3C.z;
                sp3C.x *= f;
                sp3C.y *= f;
                sp3C.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp3C);
            mtxa_scale_s(f);
            set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    mtxa_from_rotate_y(0);
    mtxa_rotate_x(-0x4000);
    mtxa_rotate_z(0);
    mtxa_tf_point_xyz(0.0f, 0.0f, -effect->unk98.y, &sp3C);
    end.x = effect->pos.x + sp3C.x;
    end.y = effect->pos.y + sp3C.y;
    end.z = effect->pos.z + sp3C.z;
    model = efcmdlGma->modelEntries[1].model;
    s = 5.0f * effect->scale.y;
    mtxa_from_mtxb_translate(&end);
    mtxa_sq_from_identity();
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        set_post_mult_color(0.95f, 0.7f, 0.4f, 1.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
    model = effect->model;
    s = effect->scale.y;
    mtxa_from_mtxb_translate(&end);
    mtxa_sq_from_identity();
    mtxa_translate_neg(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        set_post_mult_color(0.8f * effect->unk18, 0.8f * effect->unk1C, 0.8f * effect->unk20, 1.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
    model = effect->model;
    s = 4.0f * effect->scale.y;
    mtxa_from_mtxb_translate(&end);
    mtxa_sq_from_identity();
    mtxa_translate_neg(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp3C);
        if (sp3C.z < 0.0f)
        {
            if (sp3C.z < -(0.2f + s))
            {
                f = (sp3C.z + s) / sp3C.z;
                sp3C.x *= f;
                sp3C.y *= f;
                sp3C.z += s;
            }
            else
            {
                f = -0.2f / sp3C.z;
                sp3C.x *= f;
                sp3C.y *= f;
                sp3C.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp3C);
            mtxa_scale_s(f);
            set_post_mult_color(0.4f * effect->unk18, 0.4f * effect->unk1C, 0.4f * effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    model = efcmdlGma->modelEntries[6].model;
    s = 6.0f * effect->scale.y;
    mtxa_from_mtxb_translate(&end);
    mtxa_sq_from_identity();
    mtxa_translate(&effect->vel);
    mtxa_scale_s(s / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, s))
    {
        mathutil_mtxA_get_translate(&sp3C);
        if (sp3C.z < 0.0f)
        {
            if (sp3C.z < -(0.2f + s))
            {
                f = (sp3C.z + s) / sp3C.z;
                sp3C.x *= f;
                sp3C.y *= f;
                sp3C.z += s;
            }
            else
            {
                f = -0.2f / sp3C.z;
                sp3C.x *= f;
                sp3C.y *= f;
                sp3C.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp3C);
            mtxa_scale_s(f);
            set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    avdisp_set_z_mode(1, 3, 1);
    fade_color_base_default();
    count = 32;
    start = effect->pos;
    bend = end;
    w1 = 0.25f * effect->scale.x;
    w2 = 0.25f * effect->scale.y;
    col.x = 255.0f * effect->unk18;
    col.y = 255.0f * effect->unk1C;
    col.z = 255.0f * effect->unk20;
    f32_to_u8(&col.x, &c.r);
    f32_to_u8(&col.y, &c.g);
    f32_to_u8(&col.z, &c.b);
    c.a = 255;
    v = verts;
    v->pos = start;
    v->color = c;
    v++;
    step = 1.0f / count;
    for (i = count - 2; i > 0; i--, v++)
    {
        t = 1.0f - (i + 1) * step;
        u = 1.0f - t;
        x = bend.x * t + start.x * u;
        y = bend.y * t + start.y * u;
        s = bend.z * t + start.z * u;
        f = w2 * t + w1 * u;
        v->pos.x = x + f * (RAND_FLOAT() - 0.5f);
        v->pos.y = y + f * (RAND_FLOAT() - 0.5f);
        v->pos.z = s + f * (RAND_FLOAT() - 0.5f);
        v->color = c;
    }
    v->pos = bend;
    v->color = c;
    fn_1_E2FD0(12);
    fn_1_E2FE0(1, 1, 1, 0);
    mtxa_from_mtxb();
    fn_1_E33A8(count, verts);
}

void fn_1_15E970(struct Effect *effect)
{
}

void fn_1_15E974(struct Effect *effect)
{
}

void fn_1_15E978(struct Effect *effect)
{
    struct Effect *owner = fn_1_6876C(effect->playerId);

    if (owner == NULL)
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
    get_ring_point(owner, &effect->vel, NULL);
    effect->scale.z = owner->scale.x;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    mtxa_from_rotate_y(effect->unkA6);
    mtxa_rotate_x(effect->unkA4);
    mtxa_translate(&effect->unk8C);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_tf_point(&effect->unk98, &effect->pos);
    if ((float)effect->timer < 7.5f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
    }
    else
        effect->colorFactor = effect->colorFactor + 0.2f * (1.0f - effect->colorFactor);
}

void fn_1_15EB78(struct Effect *effect)
{
    Vec mid;
    Vec center;
    float len;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    len = mathutil_vec_distance(&effect->pos, &effect->vel);
    mid.x = 0.5f * (effect->pos.x + effect->vel.x);
    mid.y = 0.5f * (effect->pos.y + effect->vel.y);
    mid.z = 0.5f * (effect->pos.z + effect->vel.z);
    mtxa_from_mtxb_translate(&mid);
    center.x = 0.0f;
    center.y = 0.0f;
    center.z = 0.0f;
    if (g_is_sphere_visible(&center, len + effect->scale.x))
    {
        entry = fn_1_CCC18(mtxa->mtxA[2][3]);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_15ECEC;
            node->effect = effect;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_1_15ECEC(struct OrdTblNode *node_)
{
    struct LineVertex verts[32];
    Vec start;
    Vec bend;
    Vec col;
    Vec sp8;
    GXColor c;
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model;
    int count;
    struct LineVertex *v;
    int i;
    float x, y, z;
    float step;
    float w2;
    float w1;
    float k;
    float cf = effect->colorFactor;
    float t, u;

    start = effect->pos;
    bend = effect->vel;
    w1 = effect->scale.x * cf;
    w2 = effect->scale.z;
    model = efcmdlGma->modelEntries[8].model;
    k = 0.125f * w1;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    step = k * (RAND_FLOAT() - 0.5f);
    mtxa_translate_xyz(k * (RAND_FLOAT() - 0.5f), step, 0.0f);
    mtxa_scale_s(w1 / model->boundSphereRadius);
    if (g_is_sphere_visible(&model->boundSphereCenter, w1))
    {
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < 0.0f)
        {
            if (sp8.z < -(0.2f + w1))
            {
                k = (sp8.z + w1) / sp8.z;
                sp8.x *= k;
                sp8.y *= k;
                sp8.z += w1;
            }
            else
            {
                k = -0.2f / sp8.z;
                sp8.x *= k;
                sp8.y *= k;
                sp8.z = -0.2f;
            }
            mathutil_mtxA_set_translate(&sp8);
            mtxa_scale_s(k);
            avdisp_set_z_mode(1, 3, 0);
            set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
            avdisp_set_z_mode(1, 3, 1);
            fade_color_base_default();
        }
    }
    w1 *= 0.2f;
    w2 *= 0.5f;
    count = 32;
    col.x = 255.0f * effect->unk18;
    col.y = 255.0f * effect->unk1C;
    col.z = 255.0f * effect->unk20;
    f32_to_u8(&col.x, &c.r);
    f32_to_u8(&col.y, &c.g);
    f32_to_u8(&col.z, &c.b);
    c.a = 255;
    v = verts;
    v->pos = start;
    v->color = c;
    v++;
    step = 1.0f / count;
    for (i = count - 2; i > 0; i--, v++)
    {
        t = 1.0f - (i + 1) * step;
        u = 1.0f - t;
        x = bend.x * t + start.x * u;
        y = bend.y * t + start.y * u;
        z = bend.z * t + start.z * u;
        k = w2 * t + w1 * u;
        v->pos.x = x + k * (RAND_FLOAT() - 0.5f);
        v->pos.y = y + k * (RAND_FLOAT() - 0.5f);
        v->pos.z = z + k * (RAND_FLOAT() - 0.5f);
        v->color = c;
    }
    v->pos = bend;
    v->color = c;
    fn_1_E2FD0((u8)(12.0f * cf));
    fn_1_E2FE0(1, 1, 1, 0);
    mtxa_from_mtxb();
    fn_1_E33A8(count, verts);
}

void fn_1_15F25C(struct Effect *effect)
{
}

void fn_1_15F260(struct Effect *effect)
{
    effect->scale.x = effect->unk8C.x;
    effect->scale.y = effect->unk8C.y;
    if (0.0f == effect->unk98.z)
        effect->unk98.z = -0.08f;
}

void fn_1_15F29C(struct Effect *effect)
{
    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.98f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk98.y += effect->unk98.z;
    effect->unk98.x *= 0.98f;
    effect->unk98.y *= 0.99f;
    effect->unk8C.x += effect->unk98.x;
    effect->unk8C.y += effect->unk98.y;
    if (effect->unk8C.y < 0.0f)
    {
        if ((float)effect->timer > 15.0f)
            effect->timer = 15;
    }
    else
    {
        effect->scale.x = effect->unk8C.x;
        effect->scale.y = effect->unk8C.y;
    }
    effect->unk56 = effect->unk56 - (effect->unk56 >> 6);
    effect->rotZ += effect->unk56;
    if ((float)effect->timer < 15.0f)
    {
        float r = 1.0f / (effect->timer + 1);

        effect->colorFactor *= 1.0f - r;
    }
    else
        effect->colorFactor = effect->colorFactor + 0.2f * (1.0f - effect->colorFactor);
}

void fn_1_15F470(struct Effect *effect)
{
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    mtxa_from_mtxb();
    entry = ord_tbl_get_entry_for_pos(&effect->pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_1_15F4DC;
        node->effect = effect;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_15F4DC(struct OrdTblNode *node_)
{
    Vec light;
    struct QuadBatchItem items[2];
    struct Effect *effect = ((struct EffectOrdNode *)node_)->effect;
    struct GMAModel *model = effect->model;
    struct QuadBatchItem *q = items;
    int count = 0;

    light.x = 1.0f;
    light.y = 0.5f;
    light.z = 0.0f;
    mtxa_from_mtxb_translate(&effect->pos);
    if (g_is_sphere_visible(&model->boundSphereCenter, 2.0f * effect->scale.x))
    {
        mtxa_rotate_x(-0x4000);
        mtxa_rotate_z(effect->rotZ);
        q->scale = 2.0f * effect->scale.x;
        mtxa_to_mtx(q->mtx);
        q->color[0].r = 255;
        q->color[0].g = 255;
        q->color[0].b = 255;
        q->color[0].a = 192.0f * effect->colorFactor;
        q->color[1] = q->color[0];
        q->color[2] = q->color[0];
        q->color[3] = q->color[0];
        count++;
    }
    if (count != 0)
    {
        opti_GXSetChanCtrl(0, 1, 1, 1, 0, 0, 2);
        opti_GXSetChanCtrl(2, 1, 1, 1, 0, 0, 2);
        GXSetNumChans_cached(1);
        GXSetCullMode_cached(0);
        GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
        g_GXSetTevOrder_wrapper(0, 0, 0, 4);
        GXSetTevSwapMode_cached(0, 0, 1);
        g_GXSetTevIndirect_zero_if_different(0);
        GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xA);
        GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(0, 7, 5, 4, 7);
        GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
        GXSetNumTevStages_cached(1);
        GXSetNumTexGens_cached(1);
        GXSetZMode_cached(1, 3, 0);
        g_something_with_raster_state(0);
        GXSetBlendMode_cached(1, 4, 5, 0);
        GXLoadTexObj_cached(model->texObjs, 0);
        mtxa_from_identity();
        fn_1_16BEC0(count, items);
    }
}

void fn_1_15F71C(struct Effect *effect)
{
}
