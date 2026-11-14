/* Per-frame subsystem table: every event has init, tick and dest callbacks. */
#include "types.h"
#include "event.h"
#include "light.h"

int printf(const char *, ...);
void fn_1_E2918(void);
void event_init(int id);
void event_dest(int id);
void event_stage_init(void);
void event_stage_tick(void);
void event_stage_dest(void);
void event_world_init(void);
void event_world_tick(void);
void event_world_dest(void);
void event_ball_init(void);
void event_ball_tick(void);
void event_ball_dest(void);
void event_ape_init(void);
void event_ape_tick(void);
void event_ape_dest(void);
void event_stobj_init(void);
void event_stobj_tick(void);
void event_stobj_dest(void);
void event_item_init(void);
void event_item_tick(void);
void event_item_dest(void);
void event_recplay_init(void);
void event_recplay_tick(void);
void event_recplay_dest(void);
void event_stobj_collision_init(void);
void event_stobj_collision_tick(void);
void event_stobj_collision_dest(void);
void event_name_entry_init(void);
void event_name_entry_tick(void);
void event_name_entry_dest(void);
void event_info_init(void);
void event_info_tick(void);
void event_info_dest(void);
void event_course_init(void);
void event_course_tick(void);
void event_course_dest(void);
void event_vibration_init(void);
void event_vibration_tick(void);
void event_vibration_dest(void);
void event_commend_init(void);
void event_commend_tick(void);
void event_commend_dest(void);
void event_view_init(void);
void event_view_tick(void);
void event_view_dest(void);
void event_effect_init(void);
void event_effect_tick(void);
void event_effect_dest(void);
void event_minimap_init(void);
void event_minimap_tick(void);
void event_minimap_dest(void);
void event_camera_init(void);
void event_camera_tick(void);
void event_camera_dest(void);
void event_sprite_init(void);
void event_sprite_tick(void);
void event_sprite_dest(void);
void event_mouse_init(void);
void event_mouse_tick(void);
void event_mouse_dest(void);
void event_sound_init(void);
void event_sound_tick(void);
void event_sound_dest(void);
void event_background_init(void);
void event_background_tick(void);
void event_background_dest(void);
void event_rend_efc_init(void);
void event_rend_efc_tick(void);
void event_rend_efc_dest(void);
void event_adx_init(void);
void event_adx_tick(void);
void event_adx_dest(void);

struct Event events[] =
{
    { 0, "STAGE",         event_stage_init, event_stage_tick, event_stage_dest, 0 },
    { 0, "WORLD",         event_world_init, event_world_tick, event_world_dest, 0 },
    { 0, "BALL",          event_ball_init, event_ball_tick, event_ball_dest, 0 },
    { 0, "APE",           event_ape_init, event_ape_tick, event_ape_dest, 0 },
    { 0, "STOBJ",         event_stobj_init, event_stobj_tick, event_stobj_dest, 0 },
    { 0, "ITEM",          event_item_init, event_item_tick, event_item_dest, 0 },
    { 0, "RECPLAY",       event_recplay_init, event_recplay_tick, event_recplay_dest, 0 },
    { 0, "OBJ_COLLISION", event_stobj_collision_init, event_stobj_collision_tick, event_stobj_collision_dest, 0 },
    { 0, "NAME_ENTRY",    event_name_entry_init, event_name_entry_tick, event_name_entry_dest, 0 },
    { 0, "INFO",          event_info_init, event_info_tick, event_info_dest, 0 },
    { 0, "COURSE",        event_course_init, event_course_tick, event_course_dest, 0 },
    { 0, "VIBRATION",     event_vibration_init, event_vibration_tick, event_vibration_dest, 0 },
    { 0, "COMMEND",       event_commend_init, event_commend_tick, event_commend_dest, 0 },
    { 0, "VIEW",          event_view_init, event_view_tick, event_view_dest, 0 },
    { 0, "EFFECT",        event_effect_init, event_effect_tick, event_effect_dest, 0 },
    { 0, "MINIMAP",       event_minimap_init, event_minimap_tick, event_minimap_dest, 0 },
    { 0, "CAMERA",        event_camera_init, event_camera_tick, event_camera_dest, 0 },
    { 0, "SPRITE",        event_sprite_init, event_sprite_tick, event_sprite_dest, 0 },
    { 0, "MOUSE",         event_mouse_init, event_mouse_tick, event_mouse_dest, 0 },
    { 0, "SOUND",         event_sound_init, event_sound_tick, event_sound_dest, 0 },
    { 0, "BACKGROUND",    event_background_init, event_background_tick, event_background_dest, 0 },
    { 0, "REND_EFC",      event_rend_efc_init, event_rend_efc_tick, event_rend_efc_dest, 0 },
    { 0, "ADX",           event_adx_init, event_adx_tick, event_adx_dest, 0 },
};

#pragma opt_unroll_count 1

void init_events(void)
{
    struct Event *ev = events;
    int i;

    for (i = 0; i < 23; i++, ev++)
        ev->state = EV_STATE_INACTIVE;
}

void tick_events(void)
{
    struct Event *ev;
    int i;

    for (ev = events, i = 0; i < 23; i++, ev++)
    {
        perf_init_timer(5);
        switch (ev->state)
        {
        case EV_STATE_INITIALIZING:
            event_init(i);
            /* fall through */
        case EV_STATE_RUNNING:
            ev->main();
            break;
        case EV_STATE_FINISHING:
            event_dest(i);
            break;
        }
        ev->time = perf_stop_timer(5);
    }
}

void event_init(int id)
{
    if (events[id].state != EV_STATE_INACTIVE)
        event_dest(id);
    events[id].start();
    events[id].state = EV_STATE_RUNNING;
}

void event_dest(int id)
{
    if (events[id].state != EV_STATE_INACTIVE)
    {
        events[id].finish();
        events[id].state = EV_STATE_INACTIVE;
    }
}

void event_freeze(int id)
{
    events[id].state = EV_STATE_SUSPENDED;
}

void event_restart(int id)
{
    if (events[id].state == EV_STATE_SUSPENDED)
        events[id].state = EV_STATE_RUNNING;
    else
        printf("ev_restart: event %s is not suspended\n", events[id].name);
}

void dest_all_events(void)
{
    struct Event *ev = events;
    int i;

    for (i = 0; i < 23; i++, ev++)
    {
        if (ev->state != EV_STATE_INACTIVE)
            event_dest(i);
    }
}

void fn_1_5368(void)
{
    fn_1_E2918();
}
