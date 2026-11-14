/* Storm background: wrapped rain field, ripples on the stage. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "effect.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"

extern struct { u32 flags; } g_some_render_flag;

struct BgStmDrop
{
    float unk0;
    float unk4;
    float unk8;
    float unkC;
};

struct BgStmWork
{
    s32 unk0;
    struct GMAModel *rain00Model;
    struct GMAModel *rain01Model;
    struct GMAModel *rain02Model;
    Vec unk10;
    Vec unk1C;
    struct BgStmDrop unk28[64];
};

void *OSAllocFromHeap(void *heap, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void g_something_with_GX_vtx_desc(u32 attrs);
void *g_yet_another_unk_draw_func(void *func);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void fn_1_E2F1C(void);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void vec_set_len(float len, Vec *src, Vec *dst);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
extern void *stage_heap;
extern void *lbl_801988E0;
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}

void debug_textdraw_set_pos(int x, int y);
void fn_1_7385C(void);

static struct BgModelSearch lbl_1_data_31824[] =
{
    { BG_MDL_CMP_FULL, "STM_RAIN00" },
    { BG_MDL_CMP_FULL, "STM_RAIN01" },
    { BG_MDL_CMP_FULL, "STM_RAIN02" },
    { BG_MDL_CMP_END,  NULL },
};

int fn_1_7BEB8(int, struct GMAModelEntry *);

void bg_stm_init(void)
{
    struct BgStmWork *work = (void *)world_theme.work;
    int i;
    struct BgStmDrop *r28;

    bg_init_base();
    if (work->unk0 == 0)
        g_smth_with_bg_model_names(lbl_1_data_31824, fn_1_7BEB8);
    work->unk10.x = (RAND_FLOAT() - 0.5f) * 6.0f;
    work->unk10.y = (RAND_FLOAT() - 0.5f) * 9.0f;
    work->unk10.z = (RAND_FLOAT() - 0.5f) * 6.0f;
    work->unk1C.x = (RAND_FLOAT() - 0.5f) * 0.1f;
    work->unk1C.y = -0.25f;
    work->unk1C.z = (RAND_FLOAT() - 0.5f) * 0.1f;
    r28 = work->unk28;
    for (i = 64; i > 0; i--, r28++)
    {
        r28->unk0 = (RAND_FLOAT() - 0.5f) * 6.0f;
        r28->unk4 = (RAND_FLOAT() - 0.5f) * 9.0f;
        r28->unk8 = (RAND_FLOAT() - 0.5f) * 6.0f;
        r28->unkC = ((RAND_FLOAT() * 0.5f) + 1.0f) * 0.01f;
    }
}

void bg_stm_tick(void)
{
    struct BgStmWork *work;
    Vec spDC;
    Vec spD0;
    struct RaycastHit spB4;
    struct Effect effect;
    int i;
    struct Camera *camera;

    if (is_game_paused_and_in_view_stage())
        return;

    work = (void *)world_theme.work;
    bg_tick_base();

    spDC = work->unk10;

    spDC.x += work->unk1C.x;
    spDC.y += work->unk1C.y;
    spDC.z += work->unk1C.z;

    while (spDC.x < -3.0f)
        spDC.x += 6.0f;
    while (spDC.x > 3.0f)
        spDC.x -= 6.0f;

    while (spDC.y < -4.5f)
        spDC.y += 9.0f;
    while (spDC.y > 4.5f)
        spDC.y -= 9.0f;

    while (spDC.z < -3.0f)
        spDC.z += 6.0f;
    while (spDC.z > 3.0f)
        spDC.z -= 6.0f;

    work->unk10 = spDC;

    memset(&effect, 0, sizeof(effect));
    effect.type = 0xE;
    if (g_some_render_flag.flags & 1)
    {
        spD0.x = RAND_FLOAT() - 0.5f;
        spD0.y = 0.0f;
        spD0.z = RAND_FLOAT() - 0.5f;
        vec_set_len((RAND_FLOAT() + 0.1f) * 3.6000001430511475f, &spD0, &effect.pos);
        effect.unk70.y = 1.0f;
        vec_to_euler_xy(&spB4.normal, &effect.rotX, &effect.rotY);
        effect.rotZ = rand() & 0x7FFF;
        effect.model = work->rain02Model;
        spawn_effect(&effect);
        return;
    }
    camera = cameras;
    for (i = 0; i < 5; i++, camera++)
    {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f)
        {
            spD0.x = RAND_FLOAT() - 0.5f;
            spD0.y = 0.0f;
            spD0.z = RAND_FLOAT() - 0.5f;
            vec_set_len((RAND_FLOAT() + 0.1f) * 3.6000001430511475f, &spD0, &spD0);
            spD0.x += camera->lookAt.x;
            spD0.y += camera->lookAt.y + 10.0f;
            spD0.z += camera->lookAt.z;
            if ((int)raycast_stage_down(&spD0, &spB4, &effect.unk80) != 0)
            {
                effect.pos = spB4.pos;
                effect.unk70 = spB4.normal;
                vec_to_euler_xy(&spB4.normal, &effect.rotX, &effect.rotY);
                effect.rotZ = rand() & 0x7FFF;
                effect.model = work->rain02Model;
                spawn_effect(&effect);
            }
        }
    }
}

void bg_stm_dest(void) {}

static Vec lbl_1_data_31844[] =
{
    {  0,  0,  0 },
    {  6,  0,  0 },
    { -6,  0,  0 },
    {  0,  9,  0 },
    {  0, -9,  0 },
    {  0,  0,  6 },
    {  0,  0, -6 },
};

void bg_stm_disp(void)
{
    struct BgStmWork *work = (void *)world_theme.work;
    Vec sp7C;
    Vec sp70;
    Vec sp64;
    int i;
    int j;
    struct BgStmDrop *r30;
    Vec *r26;
    struct GMAModel *raindropModel;
    float f25;

    bg_disp_base();
    if (g_some_render_flag.flags & (1 << 2))
        return;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_inv_tf_tl(&sp7C);
    sp70 = work->unk10;
    sp70.x += (mathutil_ceil((sp7C.x - sp70.x) * 0.1666666716337204f) - 0.5f) * 6.0f;
    sp70.y += (mathutil_ceil((sp7C.y - sp70.y) * 0.1111111119389534f) - 0.5f) * 9.0f;
    sp70.z += (mathutil_ceil((sp7C.z - sp70.z) * 0.1666666716337204f) - 0.5f) * 6.0f;
    sp64 = work->unk1C;
    raindropModel = work->rain00Model;

    r30 = work->unk28;
    for (i = 64; i > 0; i--, r30++)
    {
        Vec sp58;
        Vec sp4C;
        Vec sp40;
        Vec sp34;
        Vec sp28;
        S16Vec sp20;
        float f24 = r30->unkC;

        sp58.x = sp70.x + r30->unk0;
        sp58.y = sp70.y + r30->unk4;
        sp58.z = sp70.z + r30->unk8;

        r26 = lbl_1_data_31844;
        for (j = 7; j > 0; j--, r26++)
        {
            float alpha;

            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_tf_vec(r26, &sp28);
            if (sp28.z > 0.0f)
                continue;
            sp4C.x = sp58.x + r26->x;
            sp4C.y = sp58.y + r26->y;
            sp4C.z = sp58.z + r26->z;
            f25 = mathutil_vec_distance(&sp4C, &sp7C);
            if (f25 > 6.0f)
                continue;
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[4]);
            mtxa_tf_point_xyz(sp4C.x - sp64.x, sp4C.y - sp64.y, sp4C.z - sp64.z, &sp34);
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_tf_point(&sp4C, &sp40);
            if (sp40.z > 0.0f && sp34.z > 0.0f)
                continue;

            sp28.x = sp40.x - sp34.x;
            sp28.y = sp40.y - sp34.y;
            sp28.z = sp40.z - sp34.z;

            sp34.x += sp28.x * -0.5f;
            sp34.y += sp28.y * -0.5f;
            sp34.z += sp28.z * -0.5f;

            sp28.z = 0.0f;

            {float dumb = 0; dumb += 1.0f; dumb += 0.2f;}  // needed to match

            if (sp28.x != 0.0f || sp28.y != 0.0f)
                vec_set_len(f24, &sp28, &sp28);
            else
                sp28.y = f24;
            sp40.x += sp28.x;
            sp40.y += sp28.y;
            sp34.x -= sp28.x;
            sp34.y -= sp28.y;
            ray_to_euler(&sp40, &sp34, &sp20);
            sp20.x -= 16384;
            sp28.x = (sp40.x + sp34.x) * 0.5f;
            sp28.y = (sp40.y + sp34.y) * 0.5f;
            sp28.z = (sp40.z + sp34.z) * 0.5f;
            mtxa_from_translate(&sp28);
            mtxa_rotate_y(sp20.y);
            mtxa_rotate_x(sp20.x);
            mtxa_rigid_inv_tf_tl(&sp28);
            mtxa_rotate_y(0x4000 - math_atan2(sp28.z, sp28.x));
            mtxa_scale_xyz(f24, mathutil_vec_distance(&sp40, &sp34), f24);
            if (f25 < 1.0f)
                alpha = f25;
            else
                alpha = 1.0f - 0.2f * (f25 - 1.0f);
            avdisp_set_alpha(alpha);
            avdisp_draw_model_culled_sort_auto(raindropModel);
        }
    }
}

void bg_stm_item_coin_coli(int a) {}

int fn_1_7BEB8(int index, struct GMAModelEntry *entry)
{
    struct BgStmWork *work = (void *)world_theme.work;

    switch (index)
    {
    case 0:  // STM_RAIN00
        work->rain00Model = entry->model;
        break;
    case 1:  // STM_RAIN01
        work->rain01Model = entry->model;
        break;
    case 2:  // STM_RAIN02
        work->rain02Model = entry->model;
        break;
    }
    return 1;
}
