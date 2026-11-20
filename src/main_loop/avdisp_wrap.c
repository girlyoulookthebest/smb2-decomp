/* avdisp_wrap.c - Wrappers around the avdisp model renderer: sets up its
 * heap, keeps a block of render state, and defers translucent models to the
 * ordering table */
#pragma peephole off
#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "file.h"
#include "frustum.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "os.h"

struct AvdispState
{
    /*0x00*/ float postMultR, postMultG, postMultB, postMultA;
    /*0x10*/ float postAddR, postAddG, postAddB, postAddA;
    /*0x20*/ float alpha;
    /*0x24*/ u8 zCompareEnable;
    /*0x25*/ u8 zCompareFunc;
    /*0x26*/ u8 zUpdateEnable;
    /*0x27*/ u8 unk27;
    /*0x28*/ u8 fogEnabled;
    /*0x29*/ u8 fogType;
    /*0x2A*/ GXColor fogColor;
    /*0x30*/ float fogStart;
    /*0x34*/ float fogEnd;
    /*0x38*/ int unk38;
    /*0x3C*/ u32 unk3C;
};

/* Size 0x84, allocated from the ordering table */
struct AvdispNode
{
    struct OrdTblNode node;
    /*0x08*/ struct GMAModel *model;
    /*0x0C*/ Mtx mtx;
    /*0x3C*/ int lightGroup;
    /*0x40*/ struct AvdispState state;
    /*0x80*/ Mtx *texMtx;
};

extern int currentHeap;

int OSSetCurrentHeap(int heap);
void *OSAllocFromHeap(int heap, u32 size);
void OSFreeToHeap(int heap, void *ptr);

void fn_800694F4(void);
void *fn_80069578(int, int);
void g_free_some_memory(void);
void fn_1_E767C(void);
void set_alloc_from_heap_func_ptr(void *(*func)(u32));
void set_free_to_heap_func_ptr(void (*func)(void *));
void g_something_with_texture_scroll_3(int id, Mtx mtx);
void g_avdisp_draw_model_now1(struct GMAModel *model);
void g_avdisp_draw_model_now2(struct GMAModel *model);
void g_avdisp_draw_model_now3(struct GMAModel *model);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *entry, struct OrdTblNode *node);
void g_avdisp_set_ambient(float r, float g, float b);
void fn_80066BE0(u32 lightMask);
void fn_80066BF0(Vec *dir);
void fn_80066C30(float r, float g, float b);
#ifdef __MWERKS__
void fn_80066A58(int);
#else
u32 fn_80066A58(u32);
#endif
void fn_80069860(float alpha);
void fn_80069AC0(u32);
void fn_80069AD0(int);
void avdisp_set_post_mult_color(float r, float g, float b, float a);
void avdisp_set_post_add_color(float r, float g, float b, float a);
void g_something_with_raster_state(int);
void fn_80066520(GXColor color);
void g_something_with_fog(int type, float start, float end);
void g_set_some_func_ptrs2(void **funcs);

void *alloc_from_current_heap(u32 size);
void free_from_current_heap(void *ptr);
void fn_1_10A958(void);
void g_some_ord_node_func1(struct OrdTblNode *node);
void g_some_ord_node_func2(struct OrdTblNode *node);
void fn_1_10A6C8(struct AvdispState *st);
void fn_1_10A75C(struct AvdispState *st);
void g_avdisp_reset_alpha_and_bound_sphere_scale(void);
void fn_1_10A864(void);

static int g_some_heap_handle;
static int s_unkCount;
static void *s_unkBuffer;
float g_avdisp_bound_sphere_scale;
static struct AvdispState s_state;
static int s_customTexMtx;
Mtx avdisp_tex_mtx;
Vec lbl_1_bss_D4244;
static u8 s_dvdFileInfo[0x40];

void g_init_something_with_heaps(void)
{
    g_some_heap_handle = currentHeap;
    fn_800694F4();
    fn_1_10A958();
    fn_1_E767C();
    set_alloc_from_heap_func_ptr(alloc_from_current_heap);
    set_free_to_heap_func_ptr(free_from_current_heap);
    s_unkCount = 0x40;
    s_unkBuffer = fn_80069578(s_unkCount, 1);
    g_avdisp_bound_sphere_scale = 1.0f;
    memset(&s_state, 0, sizeof(s_state));
    s_state.postMultR = 1.0f;
    s_state.postMultG = 1.0f;
    s_state.postMultB = 1.0f;
    s_state.postMultA = 1.0f;
    s_state.postAddR = 0.0f;
    s_state.postAddG = 0.0f;
    s_state.postAddB = 0.0f;
    s_state.postAddA = 0.0f;
    s_state.zCompareEnable = 1;
    s_state.zCompareFunc = 3;
    s_state.zUpdateEnable = 1;
    s_state.unk27 = 0;
    g_something_with_texture_scroll_3(0, NULL);
    s_customTexMtx = 0;
}

void g_free_some_memory2(void)
{
    int oldHeap = OSSetCurrentHeap(g_some_heap_handle);

    g_free_some_memory();
    s_unkCount = 0;
    OSSetCurrentHeap(oldHeap);
}

#define FILL_NODE(n, m, func)                                                                      \
    (n)->node.drawFunc = func;                                                                     \
    (n)->model = m;                                                                                \
    mtxa_to_mtx((n)->mtx);                                                                         \
    (n)->lightGroup = get_active_light_group();                                                    \
    (n)->state = s_state;                                                                          \
    if (s_customTexMtx == 0)                                                                       \
        (n)->texMtx = NULL;                                                                        \
    else                                                                                           \
    {                                                                                              \
        (n)->texMtx = ord_tbl_alloc_node(sizeof(Mtx));                                             \
        if ((n)->texMtx != NULL)                                                                   \
            mtx_copy(avdisp_tex_mtx, *(n)->texMtx);                                                \
    }

void avdisp_draw_model_unculled_sort_auto(struct GMAModel *model)
{
    struct AvdispNode *node;
    struct OrdTblNode *entry;

    if (1.0f == s_state.alpha)
    {
        if (model->translucentShapeCount == 0)
            avdisp_draw_model_unculled_sort_never(model);
        else
        {
            if (model->opaqueShapeCount != 0)
            {
                fn_1_10A6C8(&s_state);
                g_avdisp_draw_model_now2(model);
            }
            entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
            node = ord_tbl_alloc_node(sizeof(*node));
            if (node != NULL)
            {
                FILL_NODE(node, model, g_some_ord_node_func2)
                fn_1_CD418(entry, &node->node);
            }
            g_avdisp_reset_alpha_and_bound_sphere_scale();
        }
    }
    else
        avdisp_draw_model_unculled_sort_always(model);
}

void avdisp_draw_model_culled_sort_auto(struct GMAModel *model)
{
    struct OrdTblNode *entry;
    struct AvdispNode *node;

    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, g_avdisp_bound_sphere_scale))
    {
        if (1.0f == s_state.alpha)
        {
            if (model->translucentShapeCount == 0)
                avdisp_draw_model_unculled_sort_never(model);
            else
            {
                if (model->opaqueShapeCount != 0)
                {
                    fn_1_10A6C8(&s_state);
                    g_avdisp_draw_model_now2(model);
                }
                entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
                node = ord_tbl_alloc_node(sizeof(*node));
                if (node != NULL)
                {
                    FILL_NODE(node, model, g_some_ord_node_func2)
                    fn_1_CD418(entry, &node->node);
                }
                g_avdisp_reset_alpha_and_bound_sphere_scale();
            }
        }
        else
            avdisp_draw_model_unculled_sort_always(model);
    }
    else
    {
        g_avdisp_bound_sphere_scale = 1.0f;
        s_state.alpha = 1.0f;
    }
}

void avdisp_draw_model_unculled_sort_never(struct GMAModel *model)
{
    fn_1_10A6C8(&s_state);
    g_avdisp_draw_model_now1(model);
    g_avdisp_reset_alpha_and_bound_sphere_scale();
}

void avdisp_draw_model_culled_sort_never(struct GMAModel *model)
{
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, g_avdisp_bound_sphere_scale))
    {
        fn_1_10A6C8(&s_state);
        g_avdisp_draw_model_now1(model);
        g_avdisp_reset_alpha_and_bound_sphere_scale();
    }
    else
    {
        g_avdisp_bound_sphere_scale = 1.0f;
        s_state.alpha = 1.0f;
    }
}

void avdisp_draw_model_unculled_sort_always(struct GMAModel *model)
{
    struct AvdispNode *node;
    struct OrdTblNode *entry;

    entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        FILL_NODE(node, model, g_some_ord_node_func1)
        fn_1_CD418(entry, &node->node);
    }
    g_avdisp_bound_sphere_scale = 1.0f;
    s_state.alpha = 1.0f;
}

void avdisp_draw_model_culled_sort_always(struct GMAModel *model)
{
    struct OrdTblNode *entry;
    struct AvdispNode *node;

    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, g_avdisp_bound_sphere_scale))
    {
        entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            FILL_NODE(node, model, g_some_ord_node_func1)
            fn_1_CD418(entry, &node->node);
        }
        g_avdisp_bound_sphere_scale = 1.0f;
        s_state.alpha = 1.0f;
    }
    else
    {
        g_avdisp_bound_sphere_scale = 1.0f;
        s_state.alpha = 1.0f;
    }
}

void g_some_ord_node_func1(struct OrdTblNode *ordNode)
{
    struct AvdispNode *node = (struct AvdispNode *)ordNode;

    mtxa_from_mtx(node->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    fn_1_10A75C(&node->state);
    load_light_group_cached(node->lightGroup);
    if (node->texMtx != NULL)
        g_something_with_texture_scroll_3(0, *node->texMtx);
    else if (s_customTexMtx != 0)
        g_something_with_texture_scroll_3(0, NULL);
    g_avdisp_draw_model_now1(node->model);
    fn_1_10A864();
    if (s_customTexMtx != 0)
        g_something_with_texture_scroll_3(0, avdisp_tex_mtx);
    else if (node->texMtx != NULL)
        g_something_with_texture_scroll_3(0, NULL);
}

void g_some_ord_node_func2(struct OrdTblNode *ordNode)
{
    struct AvdispNode *node = (struct AvdispNode *)ordNode;

    mtxa_from_mtx(node->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    fn_1_10A75C(&node->state);
    load_light_group_cached(node->lightGroup);
    if (node->texMtx != NULL)
        g_something_with_texture_scroll_3(0, *node->texMtx);
    else if (s_customTexMtx != 0)
        g_something_with_texture_scroll_3(0, NULL);
    g_avdisp_draw_model_now3(node->model);
    fn_1_10A864();
    if (s_customTexMtx != 0)
        g_something_with_texture_scroll_3(0, avdisp_tex_mtx);
    else if (node->texMtx != NULL)
        g_something_with_texture_scroll_3(0, NULL);
}

void fn_1_10A2C0(struct GMAModel *model)
{
    fn_1_10A6C8(&s_state);
    g_avdisp_draw_model_now2(model);
    g_avdisp_reset_alpha_and_bound_sphere_scale();
}

void fn_1_10A300(struct GMAModel *model)
{
    fn_1_10A6C8(&s_state);
    g_avdisp_draw_model_now3(model);
    g_avdisp_reset_alpha_and_bound_sphere_scale();
}

void avdisp_set_scale_factor(float scale)
{
    g_avdisp_bound_sphere_scale = scale;
}

void call_g_avdisp_set_ambient(float r, float g, float b)
{
    g_avdisp_set_ambient(r, g, b);
}

void avdisp_set_alpha(float alpha)
{
    s_state.alpha = alpha;
}

void fn_1_10A380(u32 lightMask)
{
    fn_80066BE0(lightMask);
}

void fn_1_10A3A0(Vec *dir)
{
    Vec v;

    fn_80066BF0(dir);
    v = *dir;
    vec_normalize_len(&v);
    lbl_1_bss_D4244 = v;
}

void fn_1_10A410(float r, float g, float b)
{
    fn_80066C30(r, g, b);
}

void avdisp_set_z_mode(int compareEnable, int compareFunc, int updateEnable)
{
    s_state.zCompareEnable = compareEnable;
    s_state.zCompareFunc = compareFunc;
    s_state.zUpdateEnable = updateEnable;
}

int g_something_with_texture_scroll_2(int enable)
{
    int old = s_customTexMtx;

    s_customTexMtx = enable;
    if (enable == 0)
        g_something_with_texture_scroll_3(0, NULL);
    else
        g_something_with_texture_scroll_3(0, avdisp_tex_mtx);
    return old;
}

void fn_1_10A4CC(int id, Mtx mtx)
{
    if (mtx != NULL)
        mtx_copy(mtx, avdisp_tex_mtx);
    g_something_with_texture_scroll_3(id, mtx);
}

void set_post_mult_color(float r, float g, float b, float a)
{
    s_state.postMultR = r;
    s_state.postMultG = g;
    s_state.postMultB = b;
    s_state.postMultA = a;
    avdisp_set_post_mult_color(r, g, b, a);
}

void g_stores_doubles2(float r, float g, float b, float a)
{
    s_state.postAddR = r;
    s_state.postAddG = g;
    s_state.postAddB = b;
    s_state.postAddA = a;
    avdisp_set_post_add_color(r, g, b, a);
}

void fn_1_10A5C4(float *r, float *g, float *b, float *a)
{
    *r = s_state.postAddR;
    *g = s_state.postAddG;
    *b = s_state.postAddB;
    *a = s_state.postAddA;
}

void fn_1_10A608(int enable)
{
    s_state.fogEnabled = enable;
}

void avdisp_set_fog_params(int type, float start, float end)
{
    s_state.fogType = type;
    s_state.fogStart = start;
    s_state.fogEnd = end;
}

void avdisp_set_fog_color(u8 r, u8 g, u8 b)
{
    s_state.fogColor.r = r;
    s_state.fogColor.g = g;
    s_state.fogColor.b = b;
}

#ifdef __MWERKS__
void g_yet_another_unk_draw_func(int a)
{
    s_state.unk38 = a;
    fn_80066A58(a);
}
#else
/* returns the previous TEV material callback (fn_80066A58's result, left
 * in r3 on GC); callers restore it with a second call */
void *g_yet_another_unk_draw_func(void *a)
{
    s_state.unk38 = (u32)(uintptr_t)a;
    return (void *)(uintptr_t)fn_80066A58((u32)(uintptr_t)a);
}
#endif

void fn_1_10A69C(u32 a)
{
    s_state.unk3C = a;
    fn_80069AC0(a);
}

void fn_1_10A6C8(struct AvdispState *st)
{
    fn_80069860(st->alpha);
    GXSetZMode_cached((u8)st->zCompareEnable, st->zCompareFunc, (u8)st->zUpdateEnable);
    if (st->unk3C == 0)
    {
        if (st->fogEnabled != 0)
        {
            g_something_with_raster_state(1);
            fn_80066520(st->fogColor);
            g_something_with_fog(st->fogType, st->fogStart, st->fogEnd);
        }
        else
            g_something_with_raster_state(0);
    }
}

void fn_1_10A75C(struct AvdispState *st)
{
    fn_80069860(st->alpha);
    avdisp_set_post_mult_color(st->postMultR, st->postMultG, st->postMultB, st->postMultA);
    avdisp_set_post_add_color(st->postAddR, st->postAddG, st->postAddB, st->postAddA);
    GXSetZMode_cached((u8)st->zCompareEnable, st->zCompareFunc, (u8)st->zUpdateEnable);
    fn_80066A58(st->unk38);
    fn_80069AC0(st->unk3C);
    fn_80069AD0(st->unk27);
    if (st->unk3C == 0)
    {
        if (st->fogEnabled != 0)
        {
            g_something_with_raster_state(1);
            fn_80066520(st->fogColor);
            g_something_with_fog(st->fogType, st->fogStart, st->fogEnd);
        }
        else
            g_something_with_raster_state(0);
    }
}

void g_avdisp_reset_alpha_and_bound_sphere_scale(void)
{
    g_avdisp_bound_sphere_scale = 1.0f;
    s_state.alpha = 1.0f;
}

void fn_1_10A864(void)
{
    fn_80066A58(s_state.unk38);
    fn_80069AC0(s_state.unk3C);
    avdisp_set_post_mult_color(s_state.postMultR, s_state.postMultG, s_state.postMultB, s_state.postMultA);
    avdisp_set_post_add_color(s_state.postAddR, s_state.postAddG, s_state.postAddB, s_state.postAddA);
    fn_80069AD0(s_state.unk27);
    GXSetZMode_cached((u8)s_state.zCompareEnable, s_state.zCompareFunc, (u8)s_state.zUpdateEnable);
}

void *g_get_some_dvd_file_info2(void)
{
    return s_dvdFileInfo;
}

void fn_1_10A954(void)
{
}

void fn_1_10A958(void)
{
    void *funcs[6];

    funcs[0] = g_get_some_dvd_file_info2;
    funcs[1] = file_open;
    funcs[2] = file_size;
    funcs[3] = file_read;
    funcs[4] = file_close;
    funcs[5] = fn_1_10A954;
    g_set_some_func_ptrs2(funcs);
}

void *alloc_from_current_heap(u32 size)
{
    return OSAllocFromHeap(currentHeap, size);
}

void free_from_current_heap(void *ptr)
{
    OSFreeToHeap(currentHeap, ptr);
}

void fn_1_10AA24(int a)
{
    s_state.unk27 = a;
    fn_80069AD0(a);
}
