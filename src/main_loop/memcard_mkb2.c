/* Mkb2-specific memory card glue: game data and replay file packaging */

#include "types.h"
#include "libc.h"
#include "mode.h"
#include "recplay.h"
#ifndef __MWERKS__
/* pc_port: undeclared in the original; an implicit declaration passes float args as double on x86 */
u8 g_get_last_used_memcard_slot(void);
void fn_8000A730(void *dst);       /* stick gates -> save (input_sdl2.c) */
void fn_8000A75C(const void *src); /* save -> stick gates */
#endif

u8 fn_1_FD63C(void);
void *alloc_from_heap_or_panic(void *heap, u32 size, const char *file, int line);
void OSFreeToHeap(void *heap, void *ptr);
void OSTicksToCalendarTime(s64 ticks, void *date);
s64 OSGetTime(void);
u32 get_compressed_replay_size(void);
u32 compress_replay(void *dest);
void fn_1_62AA0(int id, void *header);

extern void *currentHeap;
extern u32 dip_switches;
extern int locale;
extern s8 g_last_used_memcard_slot_2;
extern s32 lbl_1_bss_CF3D4;
extern s32 lbl_1_bss_CF3D8;
extern u8 g_storymode_file_memcard_buffer[0xFA0];
extern u8 storymode_save_files[0xFA0];

void g_save_game_data(void)
{
    storymode_save_files[0x318] = modeCtrl.splitscreenMode;
    storymode_save_files[0x319] = modeCtrl.wideMode;
    storymode_save_files[0x31A] = fn_1_FD63C();
    fn_8000A730(storymode_save_files + 0x31C);
    fn_1_125C0();
    fn_1_16C348();
    g_save_storymode_unlock_entries();
    g_save_cm_unlock_entries();
    fn_1_13FCFC();
}

void g_some_loading_function(void)
{
    modeCtrl.splitscreenMode = storymode_save_files[0x318];
    modeCtrl.wideMode = storymode_save_files[0x319];
    fn_1_FD62C(storymode_save_files[0x31A]);
    fn_8000A75C(storymode_save_files + 0x31C);
    fn_1_1273C();
    fn_1_16C384();
    g_load_storymode_unlock_entries();
    g_load_cm_unlock_entries();
    fn_1_13FD84();
    set_render_mode(modeCtrl.wideMode);
    camera_update_aspect_ratios();
}

int fn_1_F0204(void)
{
    return memcmp(storymode_save_files, g_storymode_file_memcard_buffer, 0xF98);
}

void smd_game_over_save_child(void)
{
    memcpy_handler(g_storymode_file_memcard_buffer, storymode_save_files, 0xF98);
    lbl_1_bss_CF3D8 = 1;
    lbl_1_bss_CF3D4 = 1;
    g_last_used_memcard_slot_2 = g_get_last_used_memcard_slot();
}

void g_something_with_card2(int a, int b)
{
    if (lbl_1_bss_CF3D8 != 0)
    {
        switch (g_last_used_memcard_slot_2)
        {
        case 0:
            if (a == 0)
                lbl_1_bss_CF3D8 = 0;
            break;
        case 1:
            if (b == 0)
                lbl_1_bss_CF3D8 = 0;
            break;
        }
    }
}

struct GameStats
{
    u32 dipSwitches;
    u16 counters[0x5C8];
    u8 unkB94[0xC10 - 0xB94];
};

struct ReplaySaveBuffer
{
    void *heap;
    void *buffer;
};

struct ReplayFileInfo
{
    u8 flags;
    s8 difficulty;
    s8 floorNum;
    u8 character;
    u16 unk4;
    u32 timestamp;
};

extern struct GameStats lbl_1_bss_D1320;
extern struct GameStats lbl_1_bss_D1F40;
extern struct ReplaySaveBuffer lbl_1_bss_D2B50;

static u8 lbl_1_data_60B98[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};
static char *worldNames[6] = {"W.", "---", "---", "---", "---", "W."};
static char *masterExNames[6] = {"Ma.Ext.", "---", "---", "---", "---", "Ma.Ext."};
static char *masterNames[6] = {"Master", "Master", "Ma\xEEtre", "Maestro", "Master", "Master"};
static char *beginnerExNames[6] = {"Beg.Ext", "AN.Ex.", "DE.Ex.", "Ex.PR.", "Ex.NO.", "Beg.Ext"};
static char *advancedExNames[6] = {"Adv.Ext", "FO.Ex.", "CO.Ex.", "Ex.AV.", "Ex.AV.", "Adv.Ext"};
static char *expertExNames[6] = {"Exp.Ext", "EX.Ex.", "EX.Ex.", "Ex.EX.", "Ex.ES.", "Exp.Ext"};
static char *beginnerNames[6] = {"Beg.", "AN.", "DE.", "PR.", "NO.", "Beg."};
static char *advancedNames[6] = {"Adv.", "FO.", "CO.", "AV.", "AV.", "Adv."};
static char *expertNames[6] = {"Exp.", "EX.", "EX.", "EX.", "ES.", "Exp."};

void fn_1_F0314(void)
{
    lbl_1_bss_D1320.dipSwitches = dip_switches;
}

void fn_1_F0330(void)
{
    dip_switches = lbl_1_bss_D1320.dipSwitches;
}

void fn_1_F034C(void)
{
    memcpy(&lbl_1_bss_D1F40, &lbl_1_bss_D1320, sizeof(lbl_1_bss_D1F40));
}

void fn_1_F0380(void)
{
    u16 *dst = lbl_1_bss_D1320.counters;
    u16 *src = lbl_1_bss_D1F40.counters;

    while (dst < (u16 *)lbl_1_bss_D1320.unkB94)
    {
        int sum = *dst + *src;

        if (sum > 0xFFFF)
            sum = 0xFFFF;
        *dst = sum;
        dst++;
        src++;
    }
}

u8 memcard_mkb2_unused(void)
{
    return lbl_1_data_60B98[0];
}

void memcard_mkb2_report(int err)
{
    OSReport("memcard_error_code: %d\n", err);
}

void fn_1_F08C8(void);

void *g_some_replay_func(void **a, u32 *b)
{
    void **outBuffer = a;
    u32 *outSize = b;
    u8 tpl[0x78];
    char title[0x44];
    char name[0x44];
    u8 date[0x28];
    struct ReplayHeader header;
    u32 size;
    u32 allocSize;
    u8 *buffer;
    u8 *file;

    if (lbl_1_bss_D2B50.buffer != NULL)
        fn_1_F08C8();
    fn_1_62AA0(0x11, &header);
    if (header.flags & 0x200)
    {
        if (outBuffer != NULL)
            *outBuffer = NULL;
        if (outSize != NULL)
            *outSize = 0;
        return NULL;
    }
    size = get_compressed_replay_size();
    allocSize = (size + 0x2074 + 0x1FFF) & ~0x1FFF;
    lbl_1_bss_D2B50.heap = currentHeap;
    buffer = alloc_from_heap_or_panic(lbl_1_bss_D2B50.heap, allocSize, "memcard_mkb2.c", 378);
    file = buffer + 0x2060;
    compress_replay(file + 0x14);
    *(u16 *)(file + 4) = header.flags;
    *(u16 *)(file + 6) = header.stageId;
    *(u8 *)(file + 8) = header.difficulty;
    *(u8 *)(file + 9) = header.floor;
    *(u8 *)(file + 10) = header.monkey;
    *(s32 *)(file + 12) = header.score;
    *(u32 *)(file + 16) = OSGetTime() / (*(u32 *)0x800000F8 / 4);
    fn_1_F9B54(tpl, buffer + 0x60, "/preview/banner96x32.tpl", *(u16 *)(file + 6), 0x60, 0x20, 5);
    fn_1_F9CBC(tpl);
    fn_1_F9E1C(tpl);
    if (header.flags & 0x1000)
    {
        sprintf(title, "%s%d-%d", worldNames[locale], header.difficulty + 1, header.floor + 1);
    }
    else
    {
        if (header.flags & 0x800)
            strcpy(name, masterExNames[locale]);
        else if (header.flags & 0x40)
            strcpy(name, masterNames[locale]);
        else if (header.flags & 0x20)
        {
            switch (header.difficulty)
            {
            case 0:
                strcpy(name, beginnerExNames[locale]);
                break;
            case 1:
                strcpy(name, advancedExNames[locale]);
                break;
            case 2:
                strcpy(name, expertExNames[locale]);
                break;
            }
        }
        else
        {
            switch (header.difficulty)
            {
            case 0:
                strcpy(name, beginnerNames[locale]);
                break;
            case 1:
                strcpy(name, advancedNames[locale]);
                break;
            case 2:
                strcpy(name, expertNames[locale]);
                break;
            }
        }
        sprintf(title, "%s%d", name, header.floor);
    }
    OSTicksToCalendarTime((u64)*(u32 *)(file + 16) * (*(u32 *)0x800000F8 / 4), date);
    switch (locale)
    {
    case 1:
        sprintf(name, "%s %02d.%02d.%02d %02d:%02d", title, ((s32 *)date)[4] + 1, ((s32 *)date)[3],
                ((s32 *)date)[5] % 100, ((s32 *)date)[2], ((s32 *)date)[1]);
        break;
    case 3:
        sprintf(name, "%s,%02d\xAD%02d\xAD%02d %02d:%02d", title, ((s32 *)date)[4] + 1, ((s32 *)date)[3],
                ((s32 *)date)[5] % 100, ((s32 *)date)[2], ((s32 *)date)[1]);
        break;
    case 2:
    default:
        sprintf(name, "%s %02d\xAD%02d\xAD%02d %02d:%02d", title, ((s32 *)date)[4] + 1, ((s32 *)date)[3],
                ((s32 *)date)[5] % 100, ((s32 *)date)[2], ((s32 *)date)[1]);
        break;
    }
    strncpy((char *)buffer + 0x24, name, 0x20);
    if (outBuffer != NULL)
        *outBuffer = buffer;
    if (outSize != NULL)
        *outSize = size;
    lbl_1_bss_D2B50.buffer = buffer;
    return lbl_1_bss_D2B50.buffer;
}

void fn_1_F08C8(void)
{
    if (lbl_1_bss_D2B50.buffer != NULL)
    {
        OSFreeToHeap(lbl_1_bss_D2B50.heap, lbl_1_bss_D2B50.buffer);
        lbl_1_bss_D2B50.buffer = NULL;
    }
    lbl_1_bss_D2B50.heap = (void *)-1;
}

void g_some_replay_func2(struct ReplayFileInfo *info)
{
    struct ReplayHeader header;

    memset(info, 0, sizeof(*info));
    fn_1_62AA0(0x11, &header);
    if (!(header.flags & 0x200))
    {
        memset(info, 0, sizeof(*info));
        if (header.flags & 0x1000)
            info->flags |= 1;
        if (header.flags & 0x20)
            info->flags |= 2;
        if (header.flags & 0x40)
            info->flags |= 4;
        if (header.flags & 0x800)
            info->flags |= 8;
        info->unk4 = header.stageId;
        info->difficulty = (s8)header.difficulty;
        info->floorNum = (s8)header.floor;
        info->character = header.monkey;
        info->timestamp = OSGetTime() / (*(u32 *)0x800000F8 / 4);
    }
}

char *g_some_replay_func3(u8 *a, char *b)
{
    int i;
    int n;
    char hex[8];
    u8 *bytes = a;
    char *out = b;

    strcpy(out, "");
    n = 12;
    i = n;
    while (i > 0)
    {
        sprintf(hex, "%02X", *bytes);
        strcat(out, hex);
        i--;
        bytes++;
    }
    return out;
}

struct ReplaySaveBuffer lbl_1_bss_D2B50;
struct GameStats lbl_1_bss_D1F40 __attribute__((aligned(32)));
struct GameStats lbl_1_bss_D1320 __attribute__((aligned(32)));
