/* Heap setup: the game heaps are recreated from a per-mode size table. */
#include "types.h"
#include "os.h"

struct GameHeapConfig
{
    u32 stageHeapSize;
    u32 bgHeapSize;
    u32 charaHeapSize;
    u32 replayHeapSize;
    u32 flags;
};

extern int currentHeap;
extern u32 lbl_80198B48;
extern u32 lbl_80198B4C;

u32 OSCheckHeap(int);
void *OSAllocFromHeap(int, u32);
void OSFreeToHeap(int, void *);
void OSDestroyHeap(int);
int OSCreateHeap(void *, void *);
int OSSetCurrentHeap(int);
void *OSGetArenaLo(void);
void *OSGetArenaHi(void);
void OSSetArenaLo(void *);
void OSSetArenaHi(void *);
void *OSAllocFromArenaHi(u32, u32);
void SoundGroupLoad(int);
void g_something_with_sound9(void);
void g_something_calls_sndPopGroup(void);
void g_something_calls_sndPopGroup_2(void);
void g_something_with_iteratively_freeing_memory(void);
void unload_stage(void);
void fn_1_1118BC(void);
void fn_1_5FDC0(void);
void *alloc_from_heap_or_panic(int, u32, char *, int);
void *fn_1_1658(u32, u32);
void *fn_1_169C(u32, u32);
void create_game_heaps(int);
void destroy_game_heaps(void);
void fn_1_1128(void);

u32 lbl_1_data_2A8[] =
{
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

static struct GameHeapConfig s_gameHeapConfigs[] =
{
    { 0x00320020, 0x00230020, 0x00480020, 0x00028020, 0x00000000 },
    { 0x00300020, 0x00250020, 0x004A0020, 0x00000000, 0x00000000 },
    { 0x00200020, 0x00001020, 0x00680020, 0x00000000, 0x00000000 },
    { 0x002C3020, 0x0022D020, 0x00430020, 0x00000000, 0x00000000 },
    { 0x00250020, 0x00230020, 0x00440020, 0x00000000, 0x00000000 },
    { 0x00040020, 0x00290020, 0x00600020, 0x00000000, 0x00000000 },
    { 0x00400020, 0x00000000, 0x004A0020, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002 },
    { 0x00360000, 0x00000120, 0x00480000, 0x00000120, 0x00000001 },
    { 0x003E0020, 0x00000120, 0x005E0020, 0x00000120, 0x00000001 },
    { 0x00300020, 0x00210020, 0x00470020, 0x00000000, 0x00000000 },
    { 0x00280020, 0x00200020, 0x00600020, 0x00000000, 0x00000000 },
    { 0x00400020, 0x00000000, 0x00440020, 0x00000000, 0x00000000 },
    { 0x00200020, 0x00250020, 0x005A0020, 0x00000000, 0x00000000 },
    { 0x00170020, 0x00230020, 0x00430020, 0x00000000, 0x00000000 },
    { 0x00300020, 0x00230020, 0x004A0020, 0x00000000, 0x00000000 },
    { 0x00520020, 0x00480020, 0x00000000, 0x00000000, 0x00000000 },
    { 0x00340020, 0x00240020, 0x00640020, 0x00000000, 0x00000000 },
};

/* sptr == int on GC; pointer-sized on x64 because other TUs declare these
 * "extern void *" (docs/x64.md, cross-TU declarations) */
sptr main_heap = -1;
sptr stage_heap = -1;
sptr bg_heap = -1;
sptr chara_heap = -1;
sptr replay_heap = -1;

/* [0..1] and [2..3] are arena bounds, [4] is the current config index */
u32 g_some_other_heap_lo[5];
u32 main_heap_size;
u32 stage_heap_size;
u32 bg_heap_size;
u32 chara_heap_size;
u32 replay_heap_size;

void g_remake_initial_main_heap_somehow(void)
{
    u32 size = OSCheckHeap(main_heap);

    g_some_other_heap_lo[0] = (u32)alloc_from_heap_or_panic(currentHeap, size, "alloc.c", 0x47);
    g_some_other_heap_lo[1] = g_some_other_heap_lo[0] + size;
    OSFreeToHeap(currentHeap, (void *)g_some_other_heap_lo[0]);
    OSDestroyHeap(main_heap);
    main_heap = OSCreateHeap((void *)g_some_other_heap_lo[0], (void *)g_some_other_heap_lo[1]);
    OSSetCurrentHeap(main_heap);
}

void g_setup_and_create_game_heaps(void)
{
    u32 size = OSCheckHeap(main_heap);

    g_some_other_heap_lo[2] = (u32)alloc_from_heap_or_panic(currentHeap, size, "alloc.c", 0x58);
    g_some_other_heap_lo[3] = g_some_other_heap_lo[2] + size;
    OSDestroyHeap(main_heap);
    create_game_heaps(0);
}

void g_something_with_sound7_and_game_heaps(int config)
{
    u32 size;
    u32 ptr;
    u32 end;

    if (config != (int)g_some_other_heap_lo[4])
    {
        g_something_with_sound9();
        fn_1_1128();
        destroy_game_heaps();
        if (s_gameHeapConfigs[g_some_other_heap_lo[4]].flags & 1)
        {
            if (!(s_gameHeapConfigs[config].flags & 1))
            {
                main_heap = OSCreateHeap((void *)g_some_other_heap_lo[0], (void *)g_some_other_heap_lo[1]);
                OSSetCurrentHeap(main_heap);
                SoundGroupLoad(1);
                SoundGroupLoad(15);
                SoundGroupLoad(21);
                size = OSCheckHeap(main_heap);
                ptr = (u32)alloc_from_heap_or_panic(main_heap, size, "alloc.c", 0x79);
                end = g_some_other_heap_lo[2] + size;
                if (ptr != g_some_other_heap_lo[2] || end != g_some_other_heap_lo[3])
                    OSPanic("alloc.c", 0x7B, "sound size conflict");
                OSDestroyHeap(main_heap);
                main_heap = -1;
            }
        }
        else if (s_gameHeapConfigs[config].flags & 1)
        {
            g_something_calls_sndPopGroup();
            g_something_calls_sndPopGroup();
            g_something_calls_sndPopGroup();
            g_something_calls_sndPopGroup_2();
            g_something_calls_sndPopGroup_2();
            g_something_calls_sndPopGroup_2();
        }
        create_game_heaps(config);
    }
}

void fn_1_1128(void)
{
    g_something_with_sound9();
    g_something_with_iteratively_freeing_memory();
    unload_stage();
    fn_1_1118BC();
    fn_1_5FDC0();
}

void create_game_heaps(int config)
{
    struct GameHeapConfig *heaps = &s_gameHeapConfigs[config];
    void *replayMem;
    void *charaMem;
    void *bgMem;
    void *stageMem;
    void *arenaHi;
    void *arenaLo;

    if (heaps->flags & 1)
    {
        OSSetArenaLo((void *)g_some_other_heap_lo[0]);
        OSSetArenaHi((void *)g_some_other_heap_lo[1]);
    }
    else
    {
        OSSetArenaLo((void *)g_some_other_heap_lo[2]);
        OSSetArenaHi((void *)g_some_other_heap_lo[3]);
    }

    if (heaps->replayHeapSize != 0)
        replayMem = OSAllocFromArenaHi(heaps->replayHeapSize, 0x20);
    if (heaps->charaHeapSize != 0)
        charaMem = OSAllocFromArenaHi(heaps->charaHeapSize, 0x20);
    if (heaps->bgHeapSize != 0)
        bgMem = OSAllocFromArenaHi(heaps->bgHeapSize, 0x20);
    if (heaps->stageHeapSize != 0)
        stageMem = OSAllocFromArenaHi(heaps->stageHeapSize, 0x20);

    arenaHi = OSGetArenaHi();
    arenaLo = OSGetArenaLo();
    main_heap = OSCreateHeap(arenaLo, arenaHi);
    main_heap_size = OSCheckHeap(main_heap);
    OSSetArenaLo(arenaHi);

    if (heaps->stageHeapSize != 0)
    {
        stage_heap = OSCreateHeap(stageMem, (u8 *)stageMem + heaps->stageHeapSize);
        stage_heap_size = OSCheckHeap(stage_heap);
    }
    else
    {
        stage_heap = -1;
        stage_heap_size = 0;
    }
    if (heaps->bgHeapSize != 0)
    {
        bg_heap = OSCreateHeap(bgMem, (u8 *)bgMem + heaps->bgHeapSize);
        bg_heap_size = OSCheckHeap(bg_heap);
    }
    else
    {
        bg_heap = -1;
        bg_heap_size = 0;
    }
    if (heaps->charaHeapSize != 0)
    {
        chara_heap = OSCreateHeap(charaMem, (u8 *)charaMem + heaps->charaHeapSize);
        chara_heap_size = OSCheckHeap(chara_heap);
    }
    else
    {
        chara_heap = -1;
        chara_heap_size = 0;
    }
    if (heaps->replayHeapSize != 0)
    {
        replay_heap = OSCreateHeap(replayMem, (u8 *)replayMem + heaps->replayHeapSize);
        replay_heap_size = OSCheckHeap(replay_heap);
    }
    else
    {
        replay_heap = -1;
        replay_heap_size = 0;
    }

    OSSetCurrentHeap(main_heap);
    if (heaps->flags & 2)
    {
        if (stage_heap < 0)
            stage_heap = main_heap;
        if (bg_heap < 0)
            bg_heap = main_heap;
        if (chara_heap < 0)
            chara_heap = main_heap;
        if (replay_heap < 0)
            replay_heap = main_heap;
    }
    g_some_other_heap_lo[4] = config;
}

void destroy_game_heaps(void)
{
    struct GameHeapConfig *heaps = &s_gameHeapConfigs[g_some_other_heap_lo[4]];

    if (heaps->flags & 2)
    {
        if (heaps->stageHeapSize == 0)
            stage_heap = -1;
        if (heaps->bgHeapSize == 0)
            bg_heap = -1;
        if (heaps->charaHeapSize == 0)
            chara_heap = -1;
        if (heaps->replayHeapSize == 0)
            replay_heap = -1;
    }
    if (main_heap >= 0)
    {
        OSDestroyHeap(main_heap);
        main_heap = -1;
    }
    if (stage_heap >= 0)
    {
        OSDestroyHeap(stage_heap);
        stage_heap = -1;
    }
    if (bg_heap >= 0)
    {
        OSDestroyHeap(bg_heap);
        bg_heap = -1;
    }
    if (chara_heap >= 0)
    {
        OSDestroyHeap(chara_heap);
        chara_heap = -1;
    }
    if (replay_heap >= 0)
    {
        OSDestroyHeap(replay_heap);
        replay_heap = -1;
    }
}

#ifndef __MWERKS__
/* PC: allocation site for the SMB2_HEAP_CHECK guards (pc_port os_shim.c) */
extern const char *pc_alloc_site_file;
extern int pc_alloc_site_line;
#endif

void *alloc_from_heap_or_panic(int heap, u32 size, char *file, int line)
{
#ifndef __MWERKS__
    void *ptr;

    pc_alloc_site_file = file;
    pc_alloc_site_line = line;
    ptr = OSAllocFromHeap(heap, size);
    pc_alloc_site_file = NULL;
#else
    void *ptr = OSAllocFromHeap(heap, size);
#endif

    if (ptr == NULL)
        OSPanic(file, line, "cannot allocate memory");
    return ptr;
}

void *fn_1_1588(u32 size, u32 align, char *file, int line)
{
    void *ptr = fn_1_1658(size, align);

    if (ptr == NULL)
        OSPanic(file, line, "cannot allocate memory");
    return ptr;
}

void *fn_1_15F0(u32 size, u32 align, char *file, int line)
{
    void *ptr = fn_1_169C(size, align);

    if (ptr == NULL)
        OSPanic(file, line, "cannot allocate memory");
    return ptr;
}

void *fn_1_1658(u32 size, u32 align)
{
    u32 ptr = lbl_80198B48 - size;

    ptr = ptr / align * align;

    if (ptr < lbl_80198B4C)
        return NULL;
    lbl_80198B48 = ptr;
    return (void *)ptr;
}

void *fn_1_169C(u32 size, u32 align)
{
    u32 ptr = lbl_80198B4C + size;

    ptr = ((ptr - 1) / align + 1) * align;

    if (ptr > lbl_80198B48)
        return NULL;
    lbl_80198B4C = ptr;
    return (void *)ptr;
}
