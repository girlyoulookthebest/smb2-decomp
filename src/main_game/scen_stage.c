/* Story mode stage select. */
#include "types.h"
#include "background.h"
#include "camera.h"
#include "course.h"
#include "effect.h"
#include "event.h"
#include "input.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "scen.h"
#include "stage.h"


struct StoryUnlock
{
    /*0x00*/ s16 id;
    /*0x02*/ s8 type;
    /*0x03*/ s8 unk3;
    /*0x04*/ int *world;
}; /* size = 0x8 */

struct MemcardRequest
{
    /*0x00*/ u8 unk0;
    u8 filler1[3];
    /*0x04*/ u32 flags;
    /*0x08*/ char *fileName;
    /*0x0C*/ void *files;
    u8 filler10[8];
}; /* size = 0x18 */

struct StageSelect
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 state;
    /*0x06*/ s16 timer;
    /*0x08*/ s16 inputDelay;
    /*0x0C*/ int stageId;
    /*0x10*/ int nextStageId;
    /*0x14*/ int requestedStageId;
    /*0x18*/ int loadingStageId;
    /*0x1C*/ int loadedStageId;
    /*0x20*/ int unk20;
    /*0x24*/ u8 filler24[0x2C - 0x24];
    /*0x2C*/ float fade;
    /*0x30*/ float fadeTarget;
    /*0x34*/ float color[4];
}; /* size = 0x44 */

extern int lbl_2_data_1A00[];
extern u32 g_some_render_flag;
extern u8 storymode_save_files[];
extern s32 lbl_1_bss_CF3D8;
extern s8 g_last_used_memcard_slot_2;

void g_save_storymode_progress(int);
void g_save_game_data(void);
void g_sprintf_memcard_error(int file, char *dest);
void g_some_replay_func4(int a, int b, struct MemcardRequest *req);
u8 g_check_some_memcard_field(void);
int g_get_some_memcard_var(void);
void smd_game_over_save_child(void);
void fn_2_ADA8(void);
void fn_2_AD74(void);
void fn_2_E418(u8 *color, int a);
void call_SoundReqID_arg_0(int soundId);
s16 fn_1_32DC8(void);
s16 g_SoftStreamStart_with_some_defaults(int track);
void g_handle_world_bgm(int a);
int g_get_debug_level_select_loading_left_asterisks(void);
void g_create_storymode_select_sprites(int a);
int fn_1_D36C8(void);
int get_load_group_status(void);
void fn_1_D391C(int arg);
void queue_stage_load(int stageId);
void event_dest(int id);
void event_init(int id);
void load_stage(int stageId);
void g_init_rendefc_for_stage(void);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void g_something_with_rend_efc(int mask);
void g_smth_with_rendefc_reflective_height(float height);
void g_draw_bg(void);
void fn_1_D6F9C(int i);
void fn_1_D6D84(int i);
void ord_tbl_draw_nodes(void);

void g_set_some_storymode_stageids(void);
void fn_2_10100(void);
void fn_2_103E0(void);
int fn_2_FEE8(int world, int stage);
int fn_2_FF98(int world, int stage, Vec *input);
void g_load_new_storymode_stage(int stageId);

struct StageSelect lbl_2_bss_BB8;

struct StoryUnlock lbl_2_data_1A28[] = {
    { 0, 0, 0, NULL },
    { 1, 1, 1, &lbl_2_data_1A00[0] },
    { 2, 1, 1, &lbl_2_data_1A00[1] },
    { 3, 1, 1, &lbl_2_data_1A00[2] },
    { 4, 1, 1, &lbl_2_data_1A00[3] },
    { 5, 1, 1, &lbl_2_data_1A00[4] },
    { 6, 1, 1, &lbl_2_data_1A00[5] },
    { 7, 1, 1, &lbl_2_data_1A00[6] },
    { 8, 1, 1, &lbl_2_data_1A00[7] },
    { 9, 1, 1, &lbl_2_data_1A00[8] },
    { 10, 1, 2, &lbl_2_data_1A00[9] },
    { 11, 2, 4, NULL },
};

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

int g_get_storymode_next_world(void)
{
    int result = -1;
    int i;
    struct StoryUnlock *unlock;
    int found = 0;

    unlock = lbl_2_data_1A28;
    for (i = 0; i < 12; i++, unlock++)
    {
        if (!(scen_info.unk34[i >> 5] & (1 << (i & 0x1F))))
        {
            switch (unlock->type)
            {
            case 0:
                if (scen_info.flags & 1)
                    found = 1;
                break;
            case 1:
                {
                    int *world = unlock->world;

                    if (*world == scen_info.world && fn_1_A4D5C(*world))
                        found = 1;
                }
                break;
            case 2:
                if (fn_1_A4DA4() && (scen_info.flags & 0x10))
                    found = 1;
                break;
            }
            if (found)
            {
                result = i;
                break;
            }
        }
    }
    return result;
}

int fn_2_F1E0(void)
{
    int result = -1;
    int i;
    struct StoryUnlock *unlock;
    int found = 0;
    int *world;
    int remaining;

    unlock = lbl_2_data_1A28;
    for (i = 0; i < 12; i++, unlock++)
    {
        if (!(scen_info.unk34[i >> 5] & (1 << (i & 0x1F))))
        {
            switch (unlock->type)
            {
            case 0:
                if (scen_info.flags & 1)
                    found = 1;
                break;
            case 1:
                world = unlock->world;
                if (*world == scen_info.world)
                {
                    remaining = get_world_stage_count(*world);
                    remaining -= get_world_unbeaten_stage_count(*world);
                    remaining -= 1;
                    if (remaining <= 0)
                        found = 1;
                }
                break;
            case 2:
                if (scen_info.world >= 9)
                {
                    remaining = get_world_stage_count(9);
                    remaining -= get_world_unbeaten_stage_count(9);
                    remaining -= 1;
                    if (remaining <= 0)
                        found = 1;
                }
                break;
            }
            if (found)
            {
                result = i;
                break;
            }
        }
    }
    return result;
}

struct StoryUnlock *g_check_if_game_over_or_after_credits_sequence(int i, struct StoryUnlock *out)
{
    memcpy_handler(out, &lbl_2_data_1A28[i], sizeof(struct StoryUnlock));
    return out;
}

void fn_2_F398(void) {}

void g_some_scenario_init_func_3(void)
{
    memset(&lbl_2_bss_BB8, 0, sizeof(lbl_2_bss_BB8));
    g_set_some_storymode_stageids();
}

void g_handle_storymode_stageselect_state(void)
{
    struct MemcardRequest req;
    char fileName[0x20];
    Vec input;
    int stage;
    int prevStage;
    int nextStage;

    switch (lbl_2_bss_BB8.state)
    {
    case 1:
        lbl_2_bss_BB8.state = 2;
        lbl_2_bss_BB8.unk20 = 1;
        scen_info.flags &= ~0xC;
        lbl_2_bss_BB8.fade = 0.0f;
        lbl_2_bss_BB8.fadeTarget = 0.0f;
        if ((scen_info.flags & 0x40) && storymode_save_files[0x31B] != 0)
        {
            scen_info.flags |= 0x20;
            g_save_storymode_progress(0);
            g_save_game_data();
            memset(&req, 0, sizeof(req));
            req.unk0 = 0;
            g_sprintf_memcard_error(0, fileName);
            req.fileName = fileName;
            req.files = storymode_save_files;
            if (lbl_1_bss_CF3D8 != 0)
                req.flags |= 0x20000000;
            g_some_replay_func4(0, (u8)g_last_used_memcard_slot_2, &req);
        }
        /* fall through */
    case 2:
        if (lbl_2_bss_BB8.timer != 0)
            lbl_2_bss_BB8.timer--;
        if (!(scen_info.flags & 0x20))
        {
            if (lbl_2_bss_BB8.timer == 0)
                lbl_2_bss_BB8.state = 3;
            break;
        }
        if (g_check_some_memcard_field() == 0)
        {
            scen_info.flags &= ~0x20;
            if (g_get_some_memcard_var() != 0)
            {
                smd_game_over_save_child();
                scen_info.flags &= ~0x40;
            }
        }
        break;
    case 3:
        lbl_2_bss_BB8.state = 4;
        lbl_2_bss_BB8.inputDelay = 0;
        lbl_2_bss_BB8.fade = 0.5f;
        lbl_2_bss_BB8.fadeTarget = 1.0f;
        fn_2_ADA8();
        scen_info.flags |= 4;
        /* fall through */
    case 4:
        stage = scen_info.unk6;
        prevStage = stage;
        if ((merged_digital_inputs[2] & 1) || (merged_digital_inputs[2] & 2)
         || (merged_digital_inputs[2] & 8) || (merged_digital_inputs[2] & 4)
         || (merged_analog_inputs[2] & 1) || (merged_analog_inputs[2] & 2)
         || (merged_analog_inputs[2] & 8) || (merged_analog_inputs[2] & 4))
            lbl_2_bss_BB8.inputDelay = 0;
        if (lbl_2_bss_BB8.inputDelay > 0)
            lbl_2_bss_BB8.inputDelay--;
        if (lbl_2_bss_BB8.inputDelay == 0)
        {
            input.x = 0.0f;
            input.y = 0.0f;
            input.z = 0.0f;
            if ((merged_digital_inputs[0] & 1) || (merged_analog_inputs[0] & 1))
                input.x += -1.0f;
            if ((merged_digital_inputs[0] & 2) || (merged_analog_inputs[0] & 2))
                input.x += 1.0f;
            if ((merged_digital_inputs[0] & 8) || (merged_analog_inputs[0] & 8))
                input.y += -1.0f;
            if ((merged_digital_inputs[0] & 4) || (merged_analog_inputs[0] & 4))
                input.y += 1.0f;
            if (0.0f != input.x || 0.0f != input.y)
            {
                stage = fn_2_FF98(scen_info.world, stage, &input);
                nextStage = fn_2_FF98(scen_info.world, stage, &input);
            }
        }
        scen_info.unk6 = (s16)stage;
        if (stage != prevStage)
        {
            lbl_2_bss_BB8.stageId = get_story_mode_stage_id(scen_info.world, stage);
            lbl_2_bss_BB8.nextStageId = get_story_mode_stage_id(scen_info.world, nextStage);
            if (current_stage_id != lbl_2_bss_BB8.stageId)
            {
                lbl_2_bss_BB8.requestedStageId = lbl_2_bss_BB8.stageId;
                camera_set_state(0x52);
            }
            lbl_2_bss_BB8.inputDelay = 30;
            lbl_2_bss_BB8.fade = 0.5f;
            call_SoundReqID_arg_0(0x6C);
        }
        if (merged_digital_inputs[2] & 0x100)
        {
            if (!is_storymode_stage_beaten(scen_info.world, stage))
            {
                lbl_2_bss_BB8.state = 5;
                scen_info.flags &= ~4;
                call_SoundReqID_arg_0(0x6A);
            }
            else
                call_SoundReqID_arg_0(0x6B);
        }
        break;
    case 5:
        lbl_2_bss_BB8.state = 6;
        lbl_2_bss_BB8.timer = 60;
        scen_info.flags |= 8;
        /* fall through */
    case 6:
        if (lbl_2_bss_BB8.timer > 0)
        {
            lbl_2_bss_BB8.timer--;
            if (30.0f == (float)lbl_2_bss_BB8.timer)
            {
                int track = fn_1_32DC8();

                if (track >= 0)
                    g_SoftStreamStart_with_some_defaults(track);
                g_handle_world_bgm(100);
            }
        }
        else if (lbl_2_bss_BB8.timer == 0 && g_get_debug_level_select_loading_left_asterisks() == 0)
            lbl_2_bss_BB8.unk0 = 2;
        break;
    case 7:
        lbl_2_bss_BB8.state = 8;
        lbl_2_bss_BB8.unk0 = 3;
        /* fall through */
    case 8:
        lbl_2_bss_BB8.state = 9;
        break;
    case 9:
        lbl_2_bss_BB8.state = 10;
        lbl_2_bss_BB8.unk20 = 0;
        /* fall through */
    case 10:
        lbl_2_bss_BB8.state = 0;
        break;
    case 0:
        break;
    }
    fn_2_10100();
    if (lbl_2_bss_BB8.fade > lbl_2_bss_BB8.fadeTarget)
    {
        lbl_2_bss_BB8.fade -= 0.06666667f;
        if (lbl_2_bss_BB8.fade < lbl_2_bss_BB8.fadeTarget)
            lbl_2_bss_BB8.fade = lbl_2_bss_BB8.fadeTarget;
    }
    else if (lbl_2_bss_BB8.fade < lbl_2_bss_BB8.fadeTarget)
    {
        lbl_2_bss_BB8.fade += 0.06666667f;
        if (lbl_2_bss_BB8.fade > lbl_2_bss_BB8.fadeTarget)
            lbl_2_bss_BB8.fade = lbl_2_bss_BB8.fadeTarget;
    }
}

void fn_2_FC4C(void)
{
    switch (scen_info.unk0)
    {
    default:
        return;
    case 4:
    case 5:
    case 10:
    case 11:
    case 12:
    case 13:
    case 20:
    case 21:
        fn_2_103E0();
        ord_tbl_draw_nodes();
        break;
    }
}

void dmd_scen_sel_floor_init_child(void)
{
    struct Light light;

    lbl_2_bss_BB8.unk0 = 1;
    lbl_2_bss_BB8.unk2 = 0;
    lbl_2_bss_BB8.state = 1;
    lbl_2_bss_BB8.timer = 60;
    lbl_2_bss_BB8.fade = 0.0f;
    lbl_2_bss_BB8.fadeTarget = 0.0f;
    if (is_storymode_stage_beaten(scen_info.world, scen_info.unk6))
        scen_info.unk6 = (s16)fn_2_FEE8(scen_info.world, scen_info.unk6);
    g_set_some_storymode_stageids();
    lbl_2_bss_BB8.stageId = get_story_mode_stage_id(scen_info.world, scen_info.unk6);
    lbl_2_bss_BB8.loadedStageId = lbl_2_bss_BB8.stageId;
    g_create_storymode_select_sprites(lbl_2_bss_BB8.timer);
    camera_set_state(0x52);
    memset(&light, 0, sizeof(light));
    light.id = 8;
    light.inst = 0;
    light.type = 0;
    light.red = 1.0f;
    light.green = 1.0f;
    light.blue = 1.0f;
    light.rotX = 0x2000;
    light.rotY = 0x6000;
    add_light_to_pool(&light);
    light.id = 8;
    light.inst = 1;
    light.type = 0;
    light.red = 0.4f;
    light.green = 0.35f;
    light.blue = 0.0f;
    light.rotX = -0x400;
    light.rotY = -0x2000;
    add_light_to_pool(&light);
}

void fn_2_FE94(void)
{
    if (lbl_2_bss_BB8.state != 0)
        lbl_2_bss_BB8.state = 9;
    fn_2_AD74();
}

int fn_2_FED8(void)
{
    return lbl_2_bss_BB8.unk0;
}

int fn_2_FEE8(int world, int stage)
{
    int i;
    int count;
    int found = 0;

    count = get_world_stage_count(world);
    for (i = stage + 1; i < count; i++)
    {
        if (!is_storymode_stage_beaten(world, i))
        {
            found = 1;
            break;
        }
    }
    if (found)
        return i;
    for (i = 0; i < stage; i++)
    {
        if (!is_storymode_stage_beaten(world, i))
            break;
    }
    return i;
}

float lbl_2_data_1AB4[][2] = {
    { 1.0f, 0.0f },
    { 0.5f, -0.866f },
    { -0.5f, -0.866f },
    { -1.0f, 0.0f },
    { -0.5f, 0.866f },
    { -0.5f, 0.866f },
};

s8 lbl_2_data_1AE4[10][6] = {
    { 1, -1, -1, -1, -1, 4 },
    { 2, -1, -1, 0, 4, 5 },
    { 3, -1, -1, 1, 5, 6 },
    { -1, -1, -1, 2, 6, -1 },
    { 5, 1, 0, -1, -1, 7 },
    { 6, 2, 1, 4, 7, 8 },
    { -1, 3, 2, 5, 8, -1 },
    { 8, 5, 4, -1, -1, 9 },
    { -1, 6, 5, 7, 9, -1 },
    { -1, 8, 7, -1, -1, -1 },
};

static inline int find_best_direction(Vec *input, s8 *neighbors)
{
    Vec norm;
    Vec dir;
    float best;
    float dot;
    int i;
    int bestIdx;
    float *dirs;

    mtxa_from_rotate_z(0xAAA);
    mtxa_tf_vec(input, &norm);
    vec_normalize_len(&norm);
    dir.z = 0.0f;
    best = -2.0f;
    bestIdx = -1;
    dirs = (float *)lbl_2_data_1AB4;
    for (i = 0; i < 6; i++, dirs += 2)
    {
        if (neighbors[i] >= 0)
        {
            dir.x = dirs[0];
            dir.y = dirs[1];
            dot = mathutil_vec_dot_prod(&norm, &dir);
            if (best < dot)
            {
                bestIdx = i;
                best = dot;
            }
        }
    }
    if (best < 0.1f)
        return -1;
    return bestIdx;
}

int fn_2_FF98(int world, int stage, Vec *input)
{
    s8 *neighbors2;
    s8 *neighbors;
    int result;

    neighbors = lbl_2_data_1AE4[stage];
    neighbors2 = lbl_2_data_1AE4[stage];
    result = find_best_direction(input, neighbors2);
    if (result < 0)
        return stage;
    return neighbors[result];
}

void g_set_some_storymode_stageids(void)
{
    lbl_2_bss_BB8.requestedStageId = -1;
    lbl_2_bss_BB8.loadingStageId = -1;
    lbl_2_bss_BB8.loadedStageId = -1;
}

void fn_2_10100(void)
{
    if (lbl_2_bss_BB8.requestedStageId != -1)
    {
        if (lbl_2_bss_BB8.loadingStageId != lbl_2_bss_BB8.requestedStageId
         && lbl_2_bss_BB8.loadedStageId != lbl_2_bss_BB8.requestedStageId)
        {
            lbl_2_bss_BB8.loadingStageId = lbl_2_bss_BB8.requestedStageId;
            if (fn_1_D36C8())
                fn_1_D391C(get_load_group_status());
            queue_stage_load(lbl_2_bss_BB8.requestedStageId);
            lbl_2_bss_BB8.loadedStageId = -1;
        }
        lbl_2_bss_BB8.requestedStageId = -1;
    }
    if (lbl_2_bss_BB8.loadingStageId != -1 && !fn_1_D36C8())
    {
        lbl_2_bss_BB8.loadedStageId = lbl_2_bss_BB8.loadingStageId;
        lbl_2_bss_BB8.loadingStageId = -1;
    }
    if (lbl_2_bss_BB8.stageId == lbl_2_bss_BB8.loadedStageId)
    {
        g_load_new_storymode_stage(lbl_2_bss_BB8.loadedStageId);
        if (lbl_2_bss_BB8.nextStageId >= 0)
            lbl_2_bss_BB8.requestedStageId = lbl_2_bss_BB8.nextStageId;
    }
}

void g_load_new_storymode_stage(int stageId)
{
    struct Light light;

    if (current_stage_id != stageId)
    {
        event_dest(EVENT_BACKGROUND);
        event_dest(EVENT_STAGE);
        event_dest(EVENT_EFFECT);
        event_dest(EVENT_STOBJ);
        event_dest(EVENT_REND_EFC);
        event_dest(EVENT_ITEM);
        load_stage(stageId);
        event_init(EVENT_ITEM);
        event_init(EVENT_REND_EFC);
        event_init(EVENT_STOBJ);
        event_init(EVENT_EFFECT);
        event_init(EVENT_STAGE);
        event_init(EVENT_BACKGROUND);
        light_init(current_stage_id);
        memset(&light, 0, sizeof(light));
        light.id = 8;
        light.inst = 0;
        light.type = 0;
        light.red = 1.0f;
        light.green = 1.0f;
        light.blue = 1.0f;
        light.rotX = 0x2000;
        light.rotY = 0x6000;
        add_light_to_pool(&light);
        light.id = 8;
        light.inst = 1;
        light.type = 0;
        light.red = 0.4f;
        light.green = 0.35f;
        light.blue = 0.0f;
        light.rotX = -0x400;
        light.rotY = -0x2000;
        add_light_to_pool(&light);
        g_init_rendefc_for_stage();
        camera_set_state(0x52);
    }
}

void fn_2_103E0(void)
{
    u8 color[4];

    g_some_render_flag |= 0x200;
    camera_apply_viewport_if_changed(0);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(0);
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(4);
    if (events[EVENT_BACKGROUND].state == 2)
    {
        g_smth_with_rendefc_reflective_height(400.0f);
        g_draw_bg();
        g_smth_with_rendefc_reflective_height(0.0f);
    }
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(0x10);
    if (events[EVENT_EFFECT].state == 2)
        g_draw_effects();
    if (world_theme.unk8 & 1)
        fn_1_D6F9C(0);
    ord_tbl_draw_nodes();
    if (world_theme.unk8 & 1)
        fn_1_D6D84(0);
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(8);
    g_some_render_flag &= ~0x200;
    if (scen_info.flags & 8)
    {
        lbl_2_bss_BB8.color[0] += 0.1f * (255.0f - lbl_2_bss_BB8.color[0]);
        lbl_2_bss_BB8.color[1] += 0.1f * (254.0f - lbl_2_bss_BB8.color[1]);
        lbl_2_bss_BB8.color[2] += 0.2f * (248.0f - lbl_2_bss_BB8.color[2]);
        lbl_2_bss_BB8.color[3] += 0.075f * (204.0f - lbl_2_bss_BB8.color[3]);
        f32_to_u8(&lbl_2_bss_BB8.color[0], &color[0]);
        f32_to_u8(&lbl_2_bss_BB8.color[1], &color[1]);
        f32_to_u8(&lbl_2_bss_BB8.color[2], &color[2]);
        f32_to_u8(&lbl_2_bss_BB8.color[3], &color[3]);
        fn_2_E418(color, 1);
    }
    else
    {
        lbl_2_bss_BB8.color[0] = 255.0f;
        lbl_2_bss_BB8.color[1] = 255.0f;
        lbl_2_bss_BB8.color[2] = 255.0f;
        lbl_2_bss_BB8.color[3] = 0.0f;
        color[0] = 0xFF;
        color[1] = 0xFE;
        color[2] = 0xF8;
        color[3] = 0xCC;
        fn_2_E418(color, 4);
    }
}

BOOL fn_2_106FC(void)
{
    return lbl_2_bss_BB8.stageId == current_stage_id;
}
