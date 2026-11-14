/* Jungle background: scrolling cloud layers, sped up once half the clock has run out. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "gx.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"

#define IS_GAME_OVER_SUBMODE() \
    (sub_mode == SMD_GAME_CONTINUE_INIT || sub_mode == SMD_GAME_CONTINUE_MAIN \
     || sub_mode == 0x44 || sub_mode == 0x3D || sub_mode == 0x3E)

struct BgJunCloud
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ Vec texPos;
    /*0x10*/ Vec texVel;
    /*0x1C*/ Vec targetTexVel;
    /*0x28*/ Mtx texMtx;
};

struct BgJunWork
{
    u8 filler0[4];
    /*0x004*/ s32 cloudCount;
    /*0x008*/ struct BgJunCloud clouds[4];
    /*0x168*/ s32 mode;
};

struct BgModelSearch lbl_1_data_311C8[] =
{
    { BG_MDL_CMP_PREFIX, "JUN_FIG_CLOUD_" },
    { BG_MDL_CMP_END,    NULL },
};

int fn_1_74084(int index, struct StageBgModel *model);

void bg_jun_init(void)
{
    struct BgJunWork *work = world_theme.work;
    int i;
    struct BgJunCloud *cloud;
    Vec sp8;

    bg_init_base();
    fn_1_D68D4();
    fn_1_D7110(lbl_1_bss_2F48.infLightRotY, 0xC00);
    world_theme.unk8 |= 1;

    work->cloudCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_311C8, fn_1_74084);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_311C8, fn_1_74084);
    if (work->cloudCount == 0)
        return;

    work->mode = 0;
    cloud = work->clouds;
    for (i = work->cloudCount; i > 0; i--, cloud++)
    {
        cloud->texPos.x = RAND_FLOAT();
        cloud->texPos.y = RAND_FLOAT();
        cloud->texPos.z = RAND_FLOAT();
        mtxa_from_rotate_z(rand() & 0x7FFF);
        sp8.x = 0.0f;
        sp8.y = (RAND_FLOAT() * 0.2f + 0.9f) * 0.0018518518f;
        sp8.z = 0.0f;
        mtxa_tf_vec(&sp8, &cloud->targetTexVel);
        cloud->texVel = cloud->targetTexVel;
    }
}

void bg_jun_tick(void)
{
    struct BgJunWork *work = world_theme.work;
    int i;
    struct BgJunCloud *cloud;
    int speedUp;
    Vec sp8;

    fn_1_D6920();
    bg_tick_base();
    if (IS_GAME_OVER_SUBMODE())
    {
        world_theme.backdropColor.r = 0x55;
        world_theme.backdropColor.g = 0x55;
        world_theme.backdropColor.b = 0x55;
    }
    if (is_game_paused_and_in_view_stage())
        return;
    if (work->cloudCount == 0)
        return;

    if (work->mode == 0 && mode_info.timerCurr < (mode_info.timerMax >> 1))
    {
        speedUp = 1;
        work->mode = 1;
    }
    else
        speedUp = 0;

    cloud = work->clouds;
    for (i = work->cloudCount; i > 0; i--, cloud++)
    {
        if (speedUp)
        {
            mtxa_from_rotate_z(rand() & 0x7FFF);
            sp8.x = 0.0f;
            sp8.y = (RAND_FLOAT() * 0.2f + 0.9f) * 0.0037037036f;
            sp8.z = 0.0f;
            mtxa_tf_vec(&sp8, &cloud->targetTexVel);
        }
        cloud->texVel.x += (cloud->targetTexVel.x - cloud->texVel.x) * 0.05f;
        cloud->texVel.y += (cloud->targetTexVel.y - cloud->texVel.y) * 0.05f;
        cloud->texPos.x += cloud->texVel.x;
        cloud->texPos.y += cloud->texVel.y;
        mtxa_from_translate(&cloud->texPos);
        mtxa_to_mtx(cloud->texMtx);
    }
}

void bg_jun_dest(void) {}

void bg_jun_disp(void)
{
    struct BgJunWork *work = world_theme.work;
    struct StageBgModel *model;
    int i;
    struct BgJunCloud *cloud;
    u32 mask;

    if (IS_GAME_OVER_SUBMODE())
        set_post_mult_color(0.3f, 0.3f, 0.3f, 1.0f);

    mask = g_smth_with_challenge_mode_var_and_3p();

    cloud = work->clouds;
    for (i = work->cloudCount; i > 0; i--, cloud++)
        cloud->model->flags &= ~0x10000;

    bg_disp_base();

    if (work->cloudCount != 0)
    {
        g_something_with_texture_scroll_2(1);
        avdisp_set_z_mode(1, 3, 0);
        cloud = work->clouds;
        for (i = work->cloudCount; i > 0; i--, cloud++)
        {
            model = cloud->model;
            if (model->flags & mask)
            {
                fn_1_10A4CC(0, cloud->texMtx);
                mtxa_translate(&model->pos);
                mtxa_rotate_z(model->rotZ);
                mtxa_rotate_y(model->rotY);
                mtxa_rotate_x(model->rotX);
                mtxa_scale(&model->scale);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(model->model);
            }
        }
        avdisp_set_z_mode(1, 3, 1);
        g_something_with_texture_scroll_2(0);
    }

    if (IS_GAME_OVER_SUBMODE())
        fade_color_base_default();
}

void bg_jun_item_coin_coli(void) {}

int fn_1_74084(int index, struct StageBgModel *model)
{
    struct BgJunWork *work = world_theme.work;

    switch (index)
    {
    case 0:
        if (model->model != NULL && work->cloudCount < 2)
        {
            work->clouds[work->cloudCount].model = model;
            work->cloudCount++;
        }
        break;
    }
    return 1;
}
