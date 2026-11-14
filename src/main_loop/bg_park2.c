/* Amusement park background: registers the PAR_PLANE models as a shadow group. */

#include "types.h"
#include "background.h"
#include "gx.h"
#include "mode.h"
#include "stage.h"

struct BgPark2Work
{
    /*0x00*/ s32 planeCount;
    /*0x04*/ struct StageBgModel *planes[8];
    /*0x24*/ struct BgShadowGroup shadowGroup;
    /*0x3C*/ GXTexObj shadowTex;
    /*0x5C*/ void *shadowTexData;
};  // size = 0x60

struct BgPark2ThemeWork
{
    u8 filler0[4];
    /*0x04*/ struct BgPark2Work *park;
};

extern void *bg_heap;
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);

static struct BgModelSearch s_bgModelSearch[] =
{
    { BG_MDL_CMP_PREFIX, "PAR_PLANE" },
    { BG_MDL_CMP_END, NULL },
};

void fn_1_94D48(struct BgPark2ThemeWork *work);
int fn_1_94E28(int index, struct StageBgModel *model);

void bg_park2_init(void)
{
    struct BgPark2ThemeWork *work = world_theme.work;
    struct BgPark2Work *park;

    bg_init_base();
    park = alloc_from_heap_or_panic(bg_heap, sizeof(*park), "bg_park.c", 0x74);
    memset(park, 0, sizeof(*park));
    work->park = park;
    park->shadowTexData = alloc_from_heap_or_panic(bg_heap, GXGetTexBufferSize(0x40, 0x80, 0, 0, 0), "bg_park.c", 0x7E);
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, s_bgModelSearch, fn_1_94E28);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, s_bgModelSearch, fn_1_94E28);
    fn_1_94D48(work);
}

void bg_park2_tick(void)
{
    g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (is_game_paused_and_in_view_stage())
        return;
}

void bg_park2_dest(void)
{
    struct BgPark2ThemeWork *work = world_theme.work;
    struct BgPark2Work *park = work->park;

    if (park != NULL)
    {
        if (park->shadowTexData != NULL)
            OSFreeToHeap(bg_heap, park->shadowTexData);
        OSFreeToHeap(bg_heap, work->park);
        work->park = NULL;
    }
}

void bg_park2_disp(void)
{
    g_smth_with_challenge_mode_var_and_3p();
    bg_disp_base();
}

void bg_park2_item_coin_coli(int a) {}

void fn_1_94D48(struct BgPark2ThemeWork *work)
{
    /* Stages where the plane shadows are not drawn */
    static s16 s_noShadowStages[] = { 229, 30, -1, 0 };
    struct BgPark2Work *park = work->park;
    s16 *id;
    int found;

    if (park->planes != NULL)
    {
        found = 0;
        for (id = s_noShadowStages; *id >= 0; id++)
        {
            if (*id == current_stage_id)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            GXInitTexObj(&park->shadowTex, park->shadowTexData, 0x40, 0x80, 0, 0, 0, 0);
            park->shadowGroup.count = park->planeCount;
            park->shadowGroup.models = park->planes;
            park->shadowGroup.scale = 1.0f;
            park->shadowGroup.texFmt = 0x20;
            park->shadowGroup.tex = &park->shadowTex;
            fn_1_70290(&park->shadowGroup);
        }
    }
}

int fn_1_94E28(int index, struct StageBgModel *model)
{
    struct BgPark2ThemeWork *work = world_theme.work;
    struct BgPark2Work *park;

    switch (index)
    {
    case 0:
        park = work->park;
        if (park->planeCount < 8)
        {
            park->planes[park->planeCount] = model;
            park->planeCount++;
        }
        break;
    }
    return 1;
}
