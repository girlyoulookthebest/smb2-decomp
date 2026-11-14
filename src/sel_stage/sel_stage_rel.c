#include "types.h"
#include "camera.h"
#include "course.h"
#include "dip.h"
#include "event.h"
#include "input.h"
#include "libc.h"
#include "mode.h"
#include "os.h"
#include "pool.h"
#include "stage.h"

#define PAD_REPEAT(mask) \
    ((pad_status_groups[0].unk0[4].button & (mask)) || (analog_inputs[0][4] & (mask)) \
     || (((pad_status_groups[0].unk0[0].button & (mask)) || (analog_inputs[0][0] & (mask))) \
         && (analog_inputs[0][0] & PAD_BUTTON_B)))

extern s16 g_some_stage_id;
extern s16 g_another_stage_id;
extern s32 active_monkey_id;
extern s32 lbl_1_bss_1744[2];

void g_set_some_sel_ngc_global_func_ptrs(void (*a)(void), void (*b)(void), void (*c)(void), void (*d)(void));
void g_very_similar_to_puts(const char *s);
u32 *OSGetStackPointer(void);
void g_set_up_debugtext_buffer(void);
void dest_all_events(void);
void g_something_with_iteratively_freeing_memory(void);
void fn_1_73900(int stage);
void load_stage(int stage);
void load_bmp_by_id(int id);
void queue_stage_load(int stage);
void g_init_rendefc_for_stage(void);
void fn_1_12950(void);
void fade_screen_to_color(u32 flags, u32 color, int len);
int get_load_group_status(void);
int fn_1_D36C8(void);

void fn_4_FC(void);
void g_references_420_03(void);
void fn_4_1070(void);
void fn_4_145C(void);

struct StageListEntry
{
    u16 stageId;
    u16 unk2;
    float unk4;
};

void _prolog(void)
{
    g_set_some_sel_ngc_global_func_ptrs(g_references_420_03, fn_4_145C, NULL, NULL);
    fn_4_FC();
    sub_mode_request = 0x1E;
}

void _epilog(void)
{
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    sp = OSGetStackPointer();
    while (sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 16)
    {
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
        sp = (u32 *)sp[0];
    }
    OSPanic("sel_stage_rel.c", 72, "\n");
}

struct StageListEntry lbl_4_data_80[] = {
    {0x0C9, 0x1000, 1.0f},
    {0x0CA, 0x1000, 1.0f},
    {0x0CB, 0x1000, 1.0f},
    {0x0CC, 0x1000, 1.0f},
    {0x001, 0x1000, 1.0f},
    {0x002, 0x1000, 1.0f},
    {0x003, 0x1000, 1.0f},
    {0x004, 0x1800, 1.0f},
    {0x005, 0x1000, 1.0f},
    {0x006, 0x1000, 1.0f},
    {0x007, 0x1800, 1.0f},
    {0x008, 0x1800, 1.0f},
    {0x009, 0x1800, 1.0f},
    {0x00A, 0x1800, 1.0f},
    {0x00B, 0x1800, 1.0f},
    {0x00C, 0x1800, 1.0f},
    {0x00D, 0x1800, 1.0f},
    {0x00E, 0x1800, 1.0f},
    {0x00F, 0x1800, 1.0f},
    {0x010, 0x1800, 1.0f},
    {0x0E7, 0x1000, 1.0f},
    {0x0E8, 0x1800, 1.0f},
    {0x0E9, 0x1800, 1.0f},
    {0x0EA, 0x1800, 1.0f},
    {0x0EB, 0x1000, 1.0f},
    {0x0EC, 0x1800, 1.0f},
    {0x0ED, 0x1000, 1.0f},
    {0x0EE, 0x1000, 1.0f},
    {0x0EF, 0x1000, 1.0f},
    {0x011, 0x1800, 1.0f},
    {0x012, 0x1000, 1.0f},
    {0x013, 0x1800, 1.0f},
    {0x014, 0x1800, 1.0f},
    {0x015, 0x1000, 1.0f},
    {0x016, 0x1000, 1.0f},
    {0x017, 0x1800, 1.0f},
    {0x018, 0x1800, 1.0f},
    {0x019, 0x1800, 1.0f},
    {0x01A, 0x1800, 1.0f},
    {0x01B, 0x1800, 1.0f},
    {0x01C, 0x1800, 1.0f},
    {0x01D, 0x1800, 1.0f},
    {0x01E, 0x1000, 1.0f},
    {0x01F, 0x1800, 1.0f},
    {0x020, 0x1000, 1.0f},
    {0x021, 0x2800, 1.0f},
    {0x022, 0x1800, 1.0f},
    {0x023, 0x1800, 1.0f},
    {0x024, 0x1000, 1.0f},
    {0x025, 0x1000, 1.0f},
    {0x026, 0x0000, 1.0f},
    {0x027, 0x1800, 1.0f},
    {0x028, 0x1800, 1.0f},
    {0x029, 0x1800, 1.0f},
    {0x02A, 0x1800, 1.0f},
    {0x02B, 0x0800, 1.0f},
    {0x02C, 0x1000, 1.0f},
    {0x02D, 0x1800, 1.0f},
    {0x02E, 0x1800, 1.0f},
    {0x02F, 0x1800, 1.0f},
    {0x119, 0x1000, 1.0f},
    {0x11A, 0x1800, 1.0f},
    {0x11B, 0x1000, 1.0f},
    {0x11C, 0x1800, 1.0f},
    {0x11D, 0x1800, 1.0f},
    {0x11E, 0x1000, 1.0f},
    {0x11F, 0x1000, 1.0f},
    {0x120, 0x1800, 1.0f},
    {0x121, 0x1800, 1.0f},
    {0x030, 0x1000, 1.0f},
    {0x031, 0x1400, 1.0f},
    {0x032, 0x1400, 1.0f},
    {0x033, 0x1800, 1.0f},
    {0x034, 0xA000, 0.3f},
    {0x035, 0x1400, 1.0f},
    {0x036, 0x1800, 1.0f},
    {0x037, 0x1800, 1.0f},
    {0x038, 0x1200, 1.0f},
    {0x039, 0x1800, 1.0f},
    {0x03A, 0x1400, 1.0f},
    {0x03B, 0x1800, 1.0f},
    {0x03C, 0x1800, 1.0f},
    {0x03D, 0x1000, 1.0f},
    {0x03E, 0x1000, 1.0f},
    {0x03F, 0x1000, 1.0f},
    {0x040, 0x1800, 1.0f},
    {0x041, 0x1800, 1.0f},
    {0x042, 0x1000, 1.0f},
    {0x043, 0x1400, 1.0f},
    {0x044, 0x1800, 1.0f},
    {0x155, 0x1800, 1.0f},
    {0x156, 0x1800, 1.0f},
    {0x157, 0x1800, 1.0f},
    {0x158, 0x1800, 1.0f},
    {0x159, 0x1400, 1.0f},
    {0x15A, 0x1800, 1.0f},
    {0x15B, 0x1800, 1.0f},
    {0x15C, 0x1800, 1.0f},
    {0x15D, 0x1000, 1.0f},
    {0x15E, 0x1800, 1.0f},
};

s32 lbl_4_bss_0;

void fn_4_FC(void)
{
    if ((g_some_other_flags & 0xA) != 0)
        return;
    modeCtrl.submodeTimer = 0;
    modeCtrl.playerCount = 1;
    modeCtrl.unk30 = 1;
    modeCtrl.gameType = 0;
    modeCtrl.unk40 = 0;
    g_poolInfo.playerPool.statusList[0] = 2;
    g_poolInfo.playerPool.statusList[1] = 0;
    g_poolInfo.playerPool.statusList[2] = 0;
    g_poolInfo.playerPool.statusList[3] = 0;
    modeCtrl.currPlayer = 0;
    g_set_up_debugtext_buffer();
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    fn_1_73900(g_some_stage_id);
    load_stage(g_some_stage_id);
    load_bmp_by_id(5);
    queue_stage_load(g_some_stage_id);
    lbl_4_bss_0 = g_some_stage_id;
    event_init(0);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(5);
    event_init(4);
    event_init(0x14);
    event_init(0x15);
    g_init_rendefc_for_stage();
    enable_cameras_for_active_players();
    camera_set_state(0xC);
    in_practice_mode = 0;
    fn_1_12950();
    fn_4_1070();
    init_course();
    g_another_stage_id = mode_info.currStageId;
    fade_screen_to_color(0, 0, 30);
    sub_mode_request = 0x1E;
}

void g_references_420_03(void)
{
    int id;

    if ((g_some_other_flags & 0xA) != 0)
        return;

    if (PAD_REPEAT(PAD_BUTTON_UP))
    {
        if (--lbl_1_bss_1744[0] < 0)
            lbl_1_bss_1744[0] = 9;
    }
    else if (PAD_REPEAT(PAD_BUTTON_DOWN))
    {
        if (++lbl_1_bss_1744[0] >= 10)
            lbl_1_bss_1744[0] = 0;
    }

    switch (lbl_1_bss_1744[0])
    {
    case 0:
        modeCtrl.courseFlags &= ~1;
        modeCtrl.courseFlags &= ~COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        id = g_some_stage_id;
        if (pad_status_groups[0].unk0[0].button & PAD_TRIGGER_R)
        {
            if (PAD_REPEAT(PAD_BUTTON_LEFT))
                id -= 2;
            if (PAD_REPEAT(PAD_BUTTON_RIGHT))
                id += 2;
        }
        else
        {
            if (PAD_REPEAT(PAD_BUTTON_LEFT))
                id -= 1;
            if (PAD_REPEAT(PAD_BUTTON_RIGHT))
                id += 1;
        }
        if (id > 420)
            id = 1;
        else if (id < 1)
            id = 420;
        g_some_stage_id = (s16)id;
        break;
    case 1:
        modeCtrl.difficulty = DIFFICULTY_BEGINNER;
        modeCtrl.courseFlags |= 1;
        modeCtrl.courseFlags &= ~COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        break;
    case 2:
        modeCtrl.difficulty = DIFFICULTY_ADVANCED;
        modeCtrl.courseFlags |= 1;
        modeCtrl.courseFlags &= ~COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        break;
    case 3:
        modeCtrl.difficulty = DIFFICULTY_EXPERT;
        modeCtrl.courseFlags |= 1;
        modeCtrl.courseFlags &= ~COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        break;
    case 4:
        modeCtrl.difficulty = DIFFICULTY_BEGINNER;
        modeCtrl.courseFlags |= 1;
        modeCtrl.courseFlags |= COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        break;
    case 5:
        modeCtrl.difficulty = DIFFICULTY_ADVANCED;
        modeCtrl.courseFlags |= 1;
        modeCtrl.courseFlags |= COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        break;
    case 6:
        modeCtrl.difficulty = DIFFICULTY_EXPERT;
        modeCtrl.courseFlags |= 1;
        modeCtrl.courseFlags |= COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        break;
    case 7:
        modeCtrl.difficulty = DIFFICULTY_EXPERT;
        modeCtrl.courseFlags |= 1;
        modeCtrl.courseFlags |= COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags |= COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        break;
    case 8:
        modeCtrl.difficulty = DIFFICULTY_EXPERT;
        modeCtrl.courseFlags |= 1;
        modeCtrl.courseFlags |= COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags |= COURSE_FLAG_MASTER;
        modeCtrl.courseFlags |= COURSE_FLAG_MASTER_EXTRA;
        break;
    case 9:
        modeCtrl.courseFlags &= ~1;
        modeCtrl.courseFlags &= ~COURSE_FLAG_EXTRA;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER;
        modeCtrl.courseFlags &= ~COURSE_FLAG_MASTER_EXTRA;
        break;
    }

    if (lbl_1_bss_1744[0] != 0)
    {
        if (lbl_1_bss_1744[0] == 9)
        {
            if (PAD_REPEAT(PAD_BUTTON_LEFT))
                lbl_1_bss_1744[1]--;
            if (PAD_REPEAT(PAD_BUTTON_RIGHT))
                lbl_1_bss_1744[1]++;
            lbl_1_bss_1744[1] = lbl_1_bss_1744[1] < 0 ? 0
                : lbl_1_bss_1744[1] > sizeof(lbl_4_data_80) / sizeof(lbl_4_data_80[0]) - 1
                    ? sizeof(lbl_4_data_80) / sizeof(lbl_4_data_80[0]) - 1
                    : lbl_1_bss_1744[1];
            g_some_stage_id = (s16)lbl_4_data_80[lbl_1_bss_1744[1]].stageId;
        }
        else
        {
            if (PAD_REPEAT(PAD_BUTTON_LEFT))
                lbl_1_bss_1744[1]--;
            if (PAD_REPEAT(PAD_BUTTON_RIGHT))
                lbl_1_bss_1744[1]++;
            lbl_1_bss_1744[1] = lbl_1_bss_1744[1] < 1 ? 1
                : lbl_1_bss_1744[1] > g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags)
                    ? g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags)
                    : lbl_1_bss_1744[1];
            g_some_stage_id = (s16)g_update_cm_course(modeCtrl.difficulty, lbl_1_bss_1744[1], modeCtrl.courseFlags);
        }
    }

    if (lbl_4_bss_0 != g_some_stage_id)
    {
        get_load_group_status();
        queue_stage_load(g_some_stage_id);
        lbl_4_bss_0 = g_some_stage_id;
    }
    g_another_stage_id = mode_info.currStageId = g_some_stage_id;

    if (lbl_4_bss_0 != current_stage_id && fn_1_D36C8() == 0)
    {
        event_dest(0xE);
        event_dest(5);
        event_dest(4);
        event_dest(0x14);
        event_dest(0x15);
        fn_1_73900(lbl_4_bss_0);
        load_stage(lbl_4_bss_0);
        event_init(0xE);
        event_init(5);
        event_init(4);
        event_init(0x14);
        event_init(0x15);
        camera_set_state(0xC);
        g_init_rendefc_for_stage();
    }

    if ((pad_status_groups[0].unk0[2].button & PAD_BUTTON_A)
     || (pad_status_groups[0].unk0[2].button & PAD_BUTTON_B)
     || (pad_status_groups[0].unk0[2].button & PAD_BUTTON_X)
     || (pad_status_groups[0].unk0[2].button & PAD_BUTTON_Y))
    {
        if (pad_status_groups[0].unk0[0].button & PAD_BUTTON_Y)
            active_monkey_id = 3;
        else if (pad_status_groups[0].unk0[0].button & PAD_BUTTON_X)
            active_monkey_id = 2;
        else if (pad_status_groups[0].unk0[0].button & PAD_BUTTON_B)
            active_monkey_id = 1;
        else
            active_monkey_id = 0;
        main_mode_request = 2;
        sub_mode_request = 0x3C;
    }
}
