/* golf.c .text 0x0-0x900: module entry/exit and game init (w1) */

u32 *OSGetStackPointer(void);
int strcmp(const char *a, const char *b);
void fn_1_D5D7C(void (*tick)(void), void (*draw)(void), void (*extra)(void), void *menu, void *camera);
void g_very_similar_to_puts(const char *);
void fn_1_36140(void);
void fn_1_11C5E4(void);
void fn_1_358E8(void);
void load_stage(int stage);
void queue_stage_load(int stageId);
void dest_all_events(void);
void g_something_with_iteratively_freeing_memory(void);
void free_minigame_gma_tpl(void);
void g_something_calls_sndPopGroup(void);
void SoundGroupLoad(int);
s16 g_SoftStreamStart_with_some_defaults(int track);
void g_load_minigame_gma_tpl(int idx);
void load_bmp_by_id(int id);
void event_init(int id);
void fade_screen_to_color(s32 type, u32 color, int duration);
void textdraw_set_font(int fontId);
/* lbl_1_bss_1704: struct GolfSelSettings, declared in golf.h */

void fn_11_900(void);
void fn_11_9460(void);
void fn_11_11388(void);
/* minigame callbacks defined in golf_1.c / golf_4.c / golf_5.c (only their addresses are taken here) */
void fn_11_1E004(void);
void g_golf_init(void);
void fn_11_1B8(void);

/* Look up a model by exact name in a GMA. Unreferenced, but present in the REL. */
#pragma force_active on
struct GMAModel *fn_11_0(struct GMA *gma, char *name)
{
    struct GMAModelEntry *entry;
    int n;

    if (gma == NULL)
        return NULL;
    entry = gma->modelEntries;
    for (n = gma->numModels; n > 0; n--, entry++)
    {
        if (strcmp(entry->name, name) == 0)
            return entry->model;
    }
    return NULL;
}
#pragma force_active reset

void _prolog(void)
{
    fn_1_D5D7C(fn_11_900, fn_11_9460, fn_11_1E004, (void *)fn_11_171E8, (void *)fn_11_312F4);
    fn_11_13AC4();
    g_golf_init();
    g_very_similar_to_puts("RELOCATION GOLF2 LOADED");
}

void _epilog(void)
{
    fn_11_1B8();
    g_very_similar_to_puts("RELOCATION GOLF2 PURGED");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    for (sp = OSGetStackPointer(); sp != NULL && (uptr)sp != 0xFFFFFFFF && i++ < 16; sp = (u32 *)(uptr)*sp)
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
    OSPanic("golf2_mode.c", 0x287, "\n");
}

/* Module teardown. */
void fn_11_1B8(void)
{
    fn_1_36140();
    fn_11_13BAC();
    fn_1_11C5E4();
    load_stage(0xA1);
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    free_minigame_gma_tpl();
    g_something_calls_sndPopGroup();
}

/* Game init: players, holes of the chosen course, cameras, cup, wind. */
void g_golf_init(void)
{
    int i;

    modeCtrl.gameType = 8;
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    SoundGroupLoad(0xC);
    fn_1_358E8();
    g_SoftStreamStart_with_some_defaults(0x1C);
    g_SoftStreamStart_with_some_defaults(0x1D);
    g_SoftStreamStart_with_some_defaults(0x1E);
    g_SoftStreamStart_with_some_defaults(0x1F);
    g_SoftStreamStart_with_some_defaults(0x20);
    g_SoftStreamStart_with_some_defaults(0x21);
    g_load_minigame_gma_tpl(5);
    load_bmp_by_id(0x10);
    for (i = 0; i < 8; i++)
        g_poolInfo.playerPool.statusList[i] = balls[i].status = 0;
    for (i = 0; i < modeCtrl.playerCount; i++)
        g_poolInfo.playerPool.statusList[i] = balls[i].status = 2;
    modeCtrl.currPlayer = 0;
    lbl_11_bss_20 = &balls[modeCtrl.currPlayer];
    lbl_11_bss_24 = &cameras[modeCtrl.currPlayer];
    lbl_11_bss_28 = &cameras[(modeCtrl.currPlayer + 1) % 5];
    current_ball = &balls[modeCtrl.currPlayer];
    g_current_camera = &cameras[modeCtrl.currPlayer];
    if (lbl_1_bss_1704.unk1 == 0)
    {
        lbl_11_bss_2C = 0;
        lbl_11_bss_2E = 17;
    }
    else if (lbl_1_bss_1704.unk1 == 1)
    {
        lbl_11_bss_2C = 0;
        lbl_11_bss_2E = 8;
    }
    else if (lbl_1_bss_1704.unk1 == 2)
    {
        lbl_11_bss_2C = 9;
        lbl_11_bss_2E = 17;
    }
    load_stage(lbl_11_data_204[lbl_11_bss_2C]);
    queue_stage_load(lbl_11_data_204[lbl_11_bss_2C + 1]);
    event_init(0);
    fn_11_11388();
    event_init(3);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xB);
    event_init(0xE);
    event_init(0x14);
    lbl_11_bss_18 = 1;
    modeCtrl.currPlayer = 0;
    enable_camera(modeCtrl.currPlayer);
    camera_set_state(0x40);
    for (i = 0; i < 5; i++)
        set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
    cameras[modeCtrl.currPlayer].unk1F = 0;
    cameras[(modeCtrl.currPlayer + 1) % 5].unk1F = 9;
    set_camera_view_settings(modeCtrl.currPlayer, 0.0f, 0.0f, 1.0f, 1.0f);
    set_camera_view_settings((modeCtrl.currPlayer + 1) % 5, 0.775f, 0.0f, 0.225f, 0.2875f);
    fade_screen_to_color(0x100, 0, 60);
    lbl_11_bss_30 = 1;
    lbl_11_bss_34 = 3;
    if (stagedef->extraAnims != NULL)
    {
        lbl_11_bss_38 = stagedef->extraAnims->cupPos;
        printf("other_data->cup\n");
        printf("x : %lf\n", lbl_11_bss_38.x);
        printf("y : %lf\n", lbl_11_bss_38.y);
        printf("z : %lf\n", lbl_11_bss_38.z);
    }
    else
    {
        lbl_11_bss_38.x = lbl_11_bss_38.y = lbl_11_bss_38.z = 0.0f;
    }
    lbl_11_bss_44 = modeCtrl.playerCount;
    lbl_11_bss_45 = 0;
    lbl_11_bss_46 = 0;
    lbl_11_bss_47 = 0;
    lbl_11_bss_48 = 0;
    lbl_11_bss_49 = 0;
    textdraw_set_font(0x8F);
    lbl_11_bss_4A = 1;
    lbl_11_data_254 = -1;
    lbl_11_bss_4C = 0;
    lbl_11_bss_50 = 0;
    lbl_11_bss_54 = 0;
    lbl_11_bss_58 = lbl_11_data_2FC[0];
    lbl_11_bss_5C = 0.0f;
    lbl_11_bss_60 = 0;
    golf_wind_angle = 65536.0f * ((f32)rand() / 32767.0f) - 32768.0f;
    golf_wind_velocity = (f32)((rand() & 0x7FFF) % 600) / 60.0f;
#ifndef __MWERKS__
    pc_golf_test_wind(); /* test hook: SMB2_PC_GOLF_WIND (pc_launcher.c) */
#endif
    lbl_11_bss_6C = 0;
    lbl_11_bss_70 = 0;
    lbl_11_bss_74 = 0;
    lbl_11_bss_78 = 0;
    lbl_11_bss_4 = 0;
    for (i = 0; i < modeCtrl.playerCount; i++)
    {
        g_poolInfo.playerPool.statusList[i] = 4;
        balls[i].status = 4;
    }
    g_poolInfo.playerPool.statusList[modeCtrl.currPlayer] = 2;
    balls[modeCtrl.currPlayer].status = 2;
    lbl_11_bss_7C = 0;
    lbl_11_bss_80 = -1;
    lbl_11_bss_84 = 0;
    lbl_11_bss_1C = 0;
    lbl_11_bss_88 = 0;
    sub_mode_request = 0xBC;
}
