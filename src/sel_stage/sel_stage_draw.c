#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "camera.h"
#include "effect.h"
#include "event.h"
#include "input.h"
#include "item.h"
#include "mathutil.h"
#include "stage.h"

void g_maybe_some_bg_effect_handler(void);
void g_draw_stage(void);
void g_draw_bg(void);
void g_something_with_rend_efc(int a);
void g_draw_stobjs(void);
void ord_tbl_draw_nodes(void);

void fn_4_145C(void)
{
    struct StageCollHeader *coli;
    int i;
    struct StageGoal *goal;
    int j;

    g_maybe_some_bg_effect_handler();
    g_draw_stage();
    mtxa_from_mtxb();
    mtxa_translate((Vec *)stagedef->startPos);
    mtxa_rotate_y((s16)(g_font_char_var_3 << 9));
    mtxa_scale_xyz(0.6f, 0.6f, 0.6f);
    avdisp_set_scale_factor(0.6f);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x4D].model);

    coli = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, coli++)
    {
        goal = coli->goals;
        for (j = 0; j < coli->goalCount; j++)
        {
            mtxa_from_mtxb();
            mtxa_translate((Vec *)goal);
            mtxa_translate_xyz(0.0f, 2.0f, 0.0f);
            mtxa_rotate_y((s16)(g_font_char_var_3 << 9));
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x2A].model);
        }
    }

    if (!(pad_status_groups[0].unk0[0].button & PAD_BUTTON_Y))
    {
        if (events[20].state == 2)
        {
            g_smth_with_rendefc_reflective_height(400.0f);
            g_draw_bg();
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        if (events[21].state == 2)
            g_something_with_rend_efc(0x10);
        if (events[5].state == 2)
            draw_items();
        if (events[4].state == 2)
            g_draw_stobjs();
        if (events[14].state == 2)
            g_draw_effects();
    }
    camera_apply_main_projection();
    ord_tbl_draw_nodes();
}
