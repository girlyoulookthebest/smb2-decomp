/* Bonus stage 2 background: refractive glass, sky light, lens-flare stars, clouds. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "frustum.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"

struct BgBns2PeekEntry
{
    u32 unk0;
    u32 z;
};

struct BgBns2PeekQuery
{
    /*0x00*/ struct BgBns2PeekQuery *next;
    /*0x04*/ s32 count;
    /*0x08*/ struct BgBns2PeekEntry *entries;
    /*0x0C*/ struct BgBns2PeekEntry data[2];
};  /* size = 0x1C */

struct BgBns2Sky
{
    /*0x00*/ struct StageBgModel *bg;
    /*0x04*/ Vec color;
    /*0x10*/ Vec colorVel;
    /*0x1C*/ Vec dir;
    /*0x28*/ Vec axis;
};  /* size = 0x34 */

struct BgBns2Star
{
    /*0x00*/ struct StageBgModel *bg;
    /*0x04*/ s32 visible;
    /*0x08*/ Mtx mtx;
    /*0x38*/ s16 angle;
    /*0x3C*/ float alpha[2];
    /*0x44*/ Vec offsets[2];
    /*0x5C*/ struct BgBns2PeekQuery peek[4];
};  /* size = 0xCC */

struct BgBns2Work
{
    /*0x0000*/ s32 initialized;
    /*0x0004*/ s32 glassCount;
    /*0x0008*/ struct StageBgModel *glass[16];
    /*0x0048*/ s32 skyCount;
    /*0x004C*/ struct BgBns2Sky sky;
    /*0x0080*/ Vec screenPts[2];
    /*0x0098*/ s32 starCount;
    /*0x009C*/ struct BgBns2Star stars[32];
    /*0x1A1C*/ s32 kumoCount;
    /*0x1A20*/ struct StageBgModel *kumo[6];
    /*0x1A38*/ s32 asiKabaCount;
    /*0x1A3C*/ struct StageBgModel *asiKaba[32];
};

struct BgBns2GlassArg
{
    /*0x00*/ s32 unk0;
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;
    /*0x0C*/ u8 red;
    /*0x0D*/ u8 green;
    /*0x0E*/ u8 blue;
    /*0x0F*/ u8 alpha;
    /*0x10*/ s32 unk10;
    /*0x14*/ float unk14;
    /*0x18*/ float unk18;
    /*0x1C*/ float unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ float unk2C;
    /*0x30*/ float unk30;
    /*0x34*/ s32 unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ float unk3C;
    /*0x40*/ s32 unk40;
    /*0x44*/ s32 unk44;
    /*0x48*/ struct GMAModel *model;
    /*0x4C*/ u8 unk4C[0xC8 - 0x4C];
    /*0xC8*/ Mtx mtx;
    /*0xF8*/ u8 unkF8[0x6E0 - 0xF8];
};  /* size = 0x6E0 */

struct BgBns2OrdNode
{
    struct OrdTblNode node;
    struct BgBns2Work *work;
};

void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void fn_1_8A70(struct BgBns2GlassArg *arg);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
void GXGetProjectionv(float *proj);
void g_smth_with_gpu_and_locked_cache_mtx(void);
struct OrdTblNode *fn_1_CCD5C(int);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
u32 fn_1_118164(float z);
void fn_1_117E94(int count, Vec *pts, struct BgBns2PeekEntry *entries);
void fn_1_1181E8(struct BgBns2PeekQuery *query);
void fn_1_118210(struct BgBns2PeekQuery *query);
extern s32 lbl_1_bss_6F00;

extern struct
{
    /*0x00*/ u32 flags;
} g_some_render_flag;

static struct BgModelSearch lbl_1_data_33308[] =
{
    { BG_MDL_CMP_FULL,   "BNS_?" },
    { BG_MDL_CMP_END,    NULL },
};

static struct BgModelSearch lbl_1_data_3335C[] =
{
    { BG_MDL_CMP_FULL,   "BNS_GLASS" },
    { BG_MDL_CMP_FULL,   "BNS_SKY" },
    { BG_MDL_CMP_FULL,   "BNS_STAR_LIGHT" },
    { BG_MDL_CMP_PREFIX, "BNS_SKY_KUMO_" },
    { BG_MDL_CMP_FULL,   "BNS_ASI_KABA" },
    { BG_MDL_CMP_END,    NULL },
};

void fn_1_A0B90(struct BgBns2Work *work);
void fn_1_A0B94(struct BgBns2Work *work, u32 mask);
void fn_1_A0B98(struct BgBns2Work *work, u32 mask);
void fn_1_A0D2C(struct BgBns2Work *work);
void fn_1_A0EB8(struct BgBns2Work *work, u32 mask);
void fn_1_A1304(struct BgBns2Work *work);
void fn_1_A145C(struct BgBns2Work *work);
void fn_1_A14D0(struct BgBns2Work *work, u32 mask);
void fn_1_A14D4(struct BgBns2Work *work, u32 mask);
void fn_1_A1674(struct OrdTblNode *arg);
void fn_1_A1998(struct BgBns2Work *work);
void fn_1_A199C(struct BgBns2Work *work, u32 mask);
void fn_1_A19A0(struct BgBns2Work *work, u32 mask);
void fn_1_A19D0(struct BgBns2Work *work, u32 mask);
int fn_1_A1A94(int, struct GMAModelEntry *);
int fn_1_A1A9C(int, struct StageBgModel *);

void bg_bns2_init(void)
{
    struct BgBns2Work *work = (void *)world_theme.work;

    bg_init_base();
    if (!work->initialized)
    {
        g_smth_with_bg_model_names(lbl_1_data_33308, fn_1_A1A94);
        work->initialized = 1;
    }
    work->skyCount = 0;
    work->glassCount = 0;
    work->starCount = 0;
    work->kumoCount = 0;
    work->asiKabaCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_3335C, fn_1_A1A9C);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_3335C, fn_1_A1A9C);
    fn_1_A0D2C(work);
    fn_1_A0B90(work);
    fn_1_A1304(work);
    fn_1_A1998(work);
    g_some_render_flag.flags |= 0x100;
}

void bg_bns2_tick(void)
{
    struct BgBns2Work *work = (void *)world_theme.work;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();

    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        fn_1_A0EB8(work, mask);
        fn_1_A0B94(work, mask);
        fn_1_A14D0(work, mask);
        fn_1_A199C(work, mask);
    }
}

void bg_bns2_dest(void)
{
    fn_1_A145C((void *)world_theme.work);
    g_some_render_flag.flags &= ~0x100;
}

void bg_bns2_disp(void)
{
    struct BgBns2Work *work = (void *)world_theme.work;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();

    fn_1_A0B98(work, mask);
    fn_1_A14D4(work, mask);
    fn_1_A19A0(work, mask);
    bg_disp_base();
    fn_1_A19D0(work, mask);
}

void bg_bns2_item_coin_coli(int unused) {}

/* Glass */
void fn_1_A0B90(struct BgBns2Work *work) {}

void fn_1_A0B94(struct BgBns2Work *work, u32 mask) {}

void fn_1_A0B98(struct BgBns2Work *work, u32 mask)
{
    struct BgBns2GlassArg arg;
    int i;
    struct StageBgModel **glassp;
    struct StageBgModel *bg;
    struct GMAModel *model;

    glassp = work->glass;
    for (i = work->glassCount; i > 0; i--, glassp++)
    {
        bg = *glassp;
        if (!(bg->flags & mask))
            continue;
        bg->flags &= ~0x10000;
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&bg->pos);
        mtxa_rotate_z(bg->rotZ);
        mtxa_rotate_y(bg->rotY);
        mtxa_rotate_x(bg->rotX);
        mtxa_scale(&bg->scale);
        model = bg->model;
        if (!g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * bg->scale.x))
            continue;
        arg.unk0 = 1;
        arg.unk4 = 4;
        arg.unk8 = 4;
        arg.red = 0xD0;
        arg.green = 0xD0;
        arg.blue = 0xF0;
        arg.unk10 = 0;
        arg.alpha = 0xFF;
        arg.unk14 = 0.5f;
        arg.unk18 = 1.5f;
        arg.unk1C = 0.0f;
        arg.unk20 = -0.05f;
        arg.unk24 = 0.05f;
        arg.unk28 = 0;
        arg.unk2C = -0.3f;
        arg.unk30 = 0.025f;
        arg.unk34 = 0;
        arg.unk38 = -0.125f;
        arg.unk3C = 0.125f;
        arg.unk40 = 0;
        arg.unk44 = 1;
        arg.model = model;
        mtx_copy(mtxa->mtxA, arg.mtx);
        fn_1_8A70(&arg);
    }
}

/* Sky */
void fn_1_A0D2C(struct BgBns2Work *work)
{
    struct BgBns2Sky *sky;
    Vec v1;
    Vec v2;
    Vec sum;
    s16 rotX;
    s16 rotY;

    if (work->skyCount == 0)
        return;
    sky = &work->sky;
    mtxa_from_rotate_y(0x6000);
    mtxa_rotate_x(0x2000);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &v1);
    mtxa_from_rotate_y(0x6F00);
    mtxa_rotate_x(-0x4000);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &v2);
    sky->dir = v1;
    sum.x = v1.x + v2.x;
    sum.y = v1.y + v2.y;
    sum.z = v1.z + v2.z;
    if (sum.x != 0.0f || sum.y != 0.0f || sum.z != 0.0f)
        vec_normalize_len(&sum);
    else
    {
        vec_to_euler_xy(&v1, &rotX, &rotY);
        mtxa_from_rotate_y(rotY);
        mtxa_rotate_x(rotX);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sum);
    }
    sky->axis = sum;
    sky->color.x = 0.6f;
    sky->color.y = 0.6f;
    sky->color.z = 0.6f;
    sky->colorVel.x = 0.0f;
    sky->colorVel.y = 0.0f;
    sky->colorVel.z = 0.0f;
}

void fn_1_A0EB8(struct BgBns2Work *work, u32 mask)
{
    struct BgBns2Sky *sky;
    struct Light *light;
    float day;
    float night;
    float r;
    float g;
    float b;
    Vec v2;
    Vec v1;
    Quat q;
    Vec c;

    if (work->skyCount == 0)
        return;
    sky = &work->sky;
    day = 0.5 * (1.0 + math_sin(sky->bg->rotZ + 0x4000));
    night = 1.0f - day;
    light = alloc_pool_light(4, 0);
    if (light != NULL)
    {
        quat_from_axis_angle(&q, &sky->axis, sky->bg->rotZ);
        mtxa_from_quat(&q);
        mtxa_tf_vec(&sky->dir, &v1);
        mtxa_from_rotate_y(light->rotY);
        mtxa_rotate_x(light->rotX);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &v2);
        v2.x += 0.1f * (v1.x - v2.x);
        v2.y += 0.1f * (v1.y - v2.y);
        v2.z += 0.1f * (v1.z - v2.z);
        vec_to_euler_xy(&v2, &light->rotX, &light->rotY);
        r = day + 0.5f * night;
        g = day + 0.55f * night;
        b = day + 0.6f * night;
        light->red += 0.015f * (r - light->red);
        light->green += 0.012f * (g - light->green);
        light->blue += 0.01f * (b - light->blue);
    }
    light = alloc_pool_light(1, 0);
    if (light != NULL)
    {
        r = day + 0.65f * night;
        g = day + 0.7f * night;
        b = day + 0.9f * night;
        light->red += 0.017f * (r - light->red);
        light->green += 0.014f * (g - light->green);
        light->blue += 0.012f * (b - light->blue);
    }
    r = 0.6f * day + 0.4f * night;
    g = 0.6f * day + 0.45f * night;
    b = 0.6f * day + 0.6f * night;
    sky->colorVel.x *= 0.92f;
    sky->colorVel.y *= 0.92f;
    sky->colorVel.z *= 0.9f;
    sky->colorVel.x += 0.05f * (r - sky->color.x);
    sky->colorVel.y += 0.05f * (g - sky->color.y);
    sky->colorVel.z += 0.05f * (b - sky->color.z);
    sky->color.x += sky->colorVel.x;
    sky->color.y += sky->colorVel.y;
    sky->color.z += sky->colorVel.z;
    if (lbl_1_bss_6F00 == 0)
    {
        c = work->sky.color;
        if (c.x < 0.0f)
            c.x = 0.0f;
        else if (c.x > 1.0f)
            c.x = 1.0f;
        if (c.y < 0.0f)
            c.y = 0.0f;
        else if (c.y > 1.0f)
            c.y = 1.0f;
        if (c.z < 0.0f)
            c.z = 0.0f;
        else if (c.z > 1.0f)
            c.z = 1.0f;
        set_bg_ambient(c.x, c.y, c.z);
    }
}

/* Stars (lens flare) */
void fn_1_A1304(struct BgBns2Work *work)
{
    int count;
    int i;
    struct BgBns2Star *star;
    struct StageBgModel *bg;
    struct GMAModel *model;
    Vec *offset;

    star = work->stars;
    for (count = work->starCount; count > 0; count--, star++)
    {
        bg = star->bg;
        model = bg->model;
        for (i = 0; i < 4; i++)
        {
            star->peek[i].count = 2;
            star->peek[i].entries = star->peek[i].data;
        }
        memset(star->alpha, 0, sizeof(star->alpha));
        offset = star->offsets;
        for (i = 2; i > 0; i--, offset++)
        {
            offset->x = 0.1f * ((model->boundSphereRadius * bg->scale.x) * (RAND_FLOAT() - 0.5f));
            offset->y = 0.1f * ((model->boundSphereRadius * bg->scale.y) * (RAND_FLOAT() - 0.5f));
        }
    }
}

void fn_1_A145C(struct BgBns2Work *work)
{
    int count;
    int i;
    struct BgBns2Star *star;

    star = work->stars;
    for (count = work->starCount; count > 0; count--, star++)
    {
        for (i = 0; i < 4; i++)
            fn_1_118210(&star->peek[i]);
    }
}

void fn_1_A14D0(struct BgBns2Work *work, u32 mask) {}

void fn_1_A14D4(struct BgBns2Work *work, u32 mask)
{
    int count;
    struct BgBns2Star *star;
    struct StageBgModel *bg;
    struct GMAModel *model;
    struct OrdTblNode *list;
    struct BgBns2OrdNode *node;

    avdisp_set_z_mode(1, 3, 0);
    star = work->stars;
    for (count = work->starCount; count > 0; count--, star++)
    {
        star->visible = 0;
        bg = star->bg;
        if (!(bg->flags & 0x10000))
            continue;
        if (!(bg->flags & mask))
            continue;
        if (bg->translucency >= 1.0f)
            continue;
        if (bg->scale.x == 0.0f)
            continue;
        bg->flags &= ~0x10000;
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&bg->pos);
        mtxa_rotate_z(bg->rotZ);
        mtxa_rotate_y(bg->rotY);
        mtxa_rotate_x(bg->rotX);
        mtxa_scale(&bg->scale);
        mtxa_to_mtx(star->mtx);
        model = bg->model;
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * bg->scale.x))
        {
            star->visible = 1;
            if (bg->translucency != 0.0f)
                avdisp_set_alpha(1.0 - bg->translucency);
            avdisp_draw_model_unculled_sort_always(model);
        }
    }
    avdisp_set_z_mode(1, 3, 1);
    list = fn_1_CCD5C(0);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_1_A1674;
        node->work = work;
        fn_1_CD418(list, &node->node);
    }
}

void fn_1_A1674(struct OrdTblNode *arg)
{
    u32 depth;
    int i;
    float *alpha;
    Vec *pt;
    Vec *offset;
    int count;
    struct BgBns2Work *work;
    struct BgBns2Star *star;
    struct BgBns2PeekEntry *entry;
    struct BgBns2PeekQuery *query;
    struct GMAModel *model;
    struct StageBgModel *bg;
    float screenX;
    float screenY;
    float t; /* reciprocal Z, then brightness */
    float sum;
    float proj[7];
    Vec pos;

    GXGetProjectionv(proj);
    work = ((struct BgBns2OrdNode *)arg)->work;
    star = work->stars;
    for (count = work->starCount; count > 0; count--, star++)
    {
        if (!star->visible)
            continue;
        mtxa_from_mtx(star->mtx);
        mathutil_mtxA_get_translate(&pos);
        t = 1.0f / pos.z;
        screenX = proj[2] + t * (pos.x * proj[1]);
        screenY = proj[4] + t * (pos.y * proj[3]);
        query = &star->peek[g_something_with_camera3];
        depth = fn_1_118164(pos.z);
        entry = query->entries;
        alpha = star->alpha;
        for (i = 2; i > 0; i--, alpha++, entry++)
        {
            if (entry->z > depth)
                *alpha += 0.1f * (1.0f - *alpha);
            else
                *alpha += 0.2f * (-0.1f - *alpha);
        }
        pt = work->screenPts;
        offset = star->offsets;
        for (i = 2; i > 0; i--, pt++, offset++)
        {
            *pt = pos;
            pt->x += offset->x;
            pt->y += offset->y;
        }
        fn_1_117E94(2, work->screenPts, query->entries);
        fn_1_1181E8(query);
        star->angle = 8192.0f * (0.7f * screenX + screenY);
    }
    avdisp_set_z_mode(1, 7, 0);
    star = work->stars;
    for (count = work->starCount; count > 0; count--, star++)
    {
        if (!star->visible)
            continue;
        bg = star->bg;
        model = bg->model;
        mtxa_from_mtx(star->mtx);
        sum = 0.0f;
        alpha = star->alpha;
        for (i = 2; i > 0; i--, alpha++, entry++)
        {
            if (*alpha > 0.0f)
                sum += *alpha;
        }
        if (sum <= 0.0f)
            continue;
        sum *= 0.5f;
        mtxa_from_mtx(star->mtx);
        mtxa_rotate_x(star->angle);
        t = (1.0f - bg->translucency) * sum;
        set_post_mult_color(t, t, t, 1.0f);
        mtxa_scale_s(0.5f + 0.5f * sum);
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_never(model);
    }
    avdisp_set_z_mode(1, 3, 1);
    fade_color_base_default();
}

/* Clouds */
void fn_1_A1998(struct BgBns2Work *work) {}

void fn_1_A199C(struct BgBns2Work *work, u32 mask) {}

void fn_1_A19A0(struct BgBns2Work *work, u32 mask)
{
    int i;
    struct StageBgModel **kumop;
    struct StageBgModel *bg;

    kumop = work->kumo;
    for (i = work->kumoCount; i > 0; i--, kumop++)
    {
        bg = *kumop;
        bg->flags &= ~0x10000;
    }
}

void fn_1_A19D0(struct BgBns2Work *work, u32 mask)
{
    int i;
    struct StageBgModel **kumop;
    struct StageBgModel *bg;
    struct GMAModel *model;

    kumop = work->kumo;
    for (i = work->kumoCount; i > 0; i--, kumop++)
    {
        bg = *kumop;
        if (!(bg->flags & mask))
            continue;
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&bg->pos);
        mtxa_rotate_z(bg->rotZ);
        mtxa_rotate_y(bg->rotY);
        mtxa_rotate_x(bg->rotX);
        mtxa_scale(&bg->scale);
        model = bg->model;
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * bg->scale.x))
        {
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
}

int fn_1_A1A94(int index, struct GMAModelEntry *entry)
{
    return 1;
}

int fn_1_A1A9C(int index, struct StageBgModel *bg)
{
    struct BgBns2Work *work = (void *)world_theme.work;

    switch (index)
    {
    case 0:
        if (work->glassCount < 16)
        {
            work->glass[work->glassCount] = bg;
            work->glassCount++;
        }
        break;
    case 1:
        work->sky.bg = bg;
        work->skyCount++;
        break;
    case 2:
        if (work->starCount < 32)
        {
            work->stars[work->starCount].bg = bg;
            work->starCount++;
        }
        break;
    case 3:
        if (work->kumoCount < 6)
        {
            work->kumo[work->kumoCount] = bg;
            work->kumoCount++;
        }
        break;
    case 4:
        if (work->asiKabaCount < 32)
        {
            work->asiKaba[work->asiKabaCount] = bg;
            work->asiKabaCount++;
        }
        break;
    }
    return 1;
}
