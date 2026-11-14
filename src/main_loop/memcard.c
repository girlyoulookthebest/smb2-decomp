#pragma peephole off

#include "types.h"
#include "libc.h"
#include "os.h"
#include "file.h"
#include "input.h"
#include "va.h"
#include "mathutil.h"
#include "mode.h"
#include "textdraw.h"
#include "bitmap.h"

/* The main_loop memory card manager. Each slot runs a mode (save, load, list, delete) as a sequence
 * of submodes, and each submode has a request stage and a result stage (g_some_card_funcs and
 * g_some_card_funcs2). */

typedef struct
{
    s32 chan;
    s32 fileNo;
    s32 offset;
    s32 length;
    u16 iBlock;
} CARDFileInfo;

typedef struct
{
    char fileName[32];
    u32 length;
    u32 time;
    u8 gameName[4];
    u8 company[2];
    u8 bannerFormat;
    u32 iconAddr;
    u16 iconFormat;
    u16 iconSpeed;
    u32 commentAddr;
    u32 offsetBanner;
    u32 offsetBannerTlut;
    u32 offsetIcon[8];
    u32 offsetIconTlut;
    u32 offsetData;
} CARDStat;

struct MemcardFileDesc
{
    char name[0x1F];
    u8 index;
    char file[0x1E];
    u8 unk3E;
    u8 language;
    u32 size;
};

struct MemcardJob
{
    u8 file;
    u8 unk1;
    u16 timeout;
    s32 result;
    u32 flags;
    u32 size;
    s32 freeBytes;
    s32 freeFiles;
    u64 serial;
    CARDStat stat;
    const struct MemcardFileDesc * DPTR desc;  /* DPTR: allocated as 0xC0 */
    void * DPTR dest;
    u8 * DPTR buffer;
    char name[0x20];
    u32 dataSize;
    u8 unkBC[4];
};

struct MemcardInfo
{
    u8 chan;
    s8 mountResult;
    u8 mode;
    u8 submode;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    s16 message;
    s16 unkA;
    void *workArea;
    CARDFileInfo fileInfo;
    struct MemcardJob *job;
    s8 timer;
    u8 unk29;
    u8 flags;
    s8 unk2B;
    u8 unk2C;
    u8 unk2D;
    u32 xferredBytes;
    float unk34;
    float progress;
    s32 freeBlocks;
    s32 freeFiles;
};

struct MemcardListEntry
{
    char name[0x20];
    u32 size;
    u32 time;
};

typedef struct
{
    char gameName[4];
    char company[2];
    u8 diskNumber;
    u8 gameVersion;
    u8 streaming;
    u8 streamingBufSize;
    u8 padding[22];
} DVDDiskID;

DVDDiskID *DVDGetCurrentDiskID(void);
char *strstr(const char *, const char *);
void textdraw_set_scale(float x, float y);
void set_ui_element_sprite_rot_z(int rot);
u8 g_check_some_memcard_field(void);
void g_something_with_card2(int a, int b);
float textdraw_get_pixel_width_of_string(char *str);
void set_ui_element_sprite_pos(float x, float y);
void textdraw_set_depth(float depth);
void textdraw_set_pos(float x, float y);
void set_ui_element_sprite_scale(float x, float y);

struct MemcardRequest
{
    u8 file;
    u32 flags;
    char *comment;
    void *buffer;
    u32 size;
    char *name;
};

void *alloc_from_heap_or_panic(void *heap, u32 size, const char *file, int line);
void g_something_with_locked_cache_2(void *dest, int value, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
void OSReport(const char *fmt, ...);
extern void *currentHeap;

extern s32 locale;

void fn_1_E9940(struct MemcardRequest *req, struct MemcardInfo *info);
void fn_1_EAFAC(struct MemcardRequest *req, struct MemcardInfo *info);
void fn_1_EBF30(struct MemcardRequest *req, struct MemcardInfo *info);
void fn_1_ECA60(struct MemcardRequest *req, struct MemcardInfo *info);
void g_related_to_memcard_rw(struct MemcardInfo *info);
void g_something_with_card12(struct MemcardInfo *info);
void fn_1_EBF3C(struct MemcardInfo *info);
void fn_1_ECA6C(struct MemcardInfo *info);
void fn_1_E7EE0(struct MemcardInfo *info);
void fn_1_E837C(struct MemcardInfo *info);
void fn_1_E83A4(struct MemcardInfo *info);
void fn_1_E8440(struct MemcardInfo *info);
void fn_1_E84F8(struct MemcardInfo *info);
void fn_1_E854C(struct MemcardInfo *info);
void fn_1_E857C(struct MemcardInfo *info);
void fn_1_E869C(struct MemcardInfo *info);
void fn_1_E8708(struct MemcardInfo *info);
void fn_1_E87C4(struct MemcardInfo *info);
void fn_1_E87D4(struct MemcardInfo *info);
void fn_1_E8830(struct MemcardInfo *info);
void fn_1_E8894(struct MemcardInfo *info);
void fn_1_E88E8(struct MemcardInfo *info);
void fn_1_E88F8(struct MemcardInfo *info);
void fn_1_E897C(struct MemcardInfo *info);
void fn_1_E8A10(struct MemcardInfo *info);
void fn_1_E8A9C(struct MemcardInfo *info);
void fn_1_E8ACC(struct MemcardInfo *info);
void fn_1_E8BEC(struct MemcardInfo *info);
void fn_1_E8C28(struct MemcardInfo *info);
void fn_1_E8D14(struct MemcardInfo *info);
void fn_1_E8D68(struct MemcardInfo *info);
void fn_1_E8D78(struct MemcardInfo *info);
void fn_1_E8E58(struct MemcardInfo *info);
void fn_1_E8F24(struct MemcardInfo *info);
void fn_1_E8F68(struct MemcardInfo *info);
void fn_1_E8FBC(struct MemcardInfo *info);
void fn_1_E9020(struct MemcardInfo *info);
void fn_1_E9074(struct MemcardInfo *info);
void fn_1_E90FC(struct MemcardInfo *info);
void fn_1_E9150(struct MemcardInfo *info);
void fn_1_E9160(struct MemcardInfo *info);
void fn_1_E91E4(struct MemcardInfo *info);
void fn_1_E92E0(struct MemcardInfo *info);
void fn_1_E92E4(struct MemcardInfo *info);
void fn_1_E9308(struct MemcardInfo *info);
void fn_1_E930C(struct MemcardInfo *info);
void fn_1_E934C(struct MemcardInfo *info);
void fn_1_E9350(struct MemcardInfo *info);
void fn_1_E9390(struct MemcardInfo *info);
void fn_1_E9394(struct MemcardInfo *info);
void fn_1_E93B8(struct MemcardInfo *info);
void fn_1_E93BC(struct MemcardInfo *info);
void fn_1_E9408(struct MemcardInfo *info);
void fn_1_E945C(struct MemcardInfo *info);
void fn_1_E9558(struct MemcardInfo *info);
void fn_1_E955C(struct MemcardInfo *info);
void fn_1_E9580(struct MemcardInfo *info);
void fn_1_E9584(struct MemcardInfo *info);
void fn_1_E95C4(struct MemcardInfo *info);
void fn_1_E966C(struct MemcardInfo *info);
void fn_1_E9780(struct MemcardInfo *info);
void fn_1_E9784(struct MemcardInfo *info);
void fn_1_E97C4(struct MemcardInfo *info);
void fn_1_E9818(struct MemcardInfo *info);
void fn_1_E983C(struct MemcardInfo *info);
void fn_1_E9840(struct MemcardInfo *info);
void fn_1_E993C(struct MemcardInfo *info);
void g_calls_CARDWriteAsync(struct MemcardInfo *info);
void g_memcard_func_1(struct MemcardInfo *info);
void g_memcard_func_2(struct MemcardInfo *info);
void g_memcard_func_4(struct MemcardInfo *info);
void g_open_card_file(struct MemcardInfo *info);
void g_something_free_card_blocks(struct MemcardInfo *info);
void mount_memory_card(struct MemcardInfo *info);
void print_card_submode_error(struct MemcardInfo *info);
void something_that_calls_CARDCreateAsync(struct MemcardInfo *info);

u16 fn_800648C8(u32 size, void *data);
#ifndef __MWERKS__
/* pc_port card shim: copy a game file to memcard/backup/ (corrupt saves) */
void pc_card_backup_file(s32 chan, s32 fileNo, const char *reason);
#endif

s8 g_last_used_memcard_slot_2 = 4;

#pragma dont_reuse_strings on
static char *memcard_messages[][6] =
{
#include "memcard_messages.inc"
};
#pragma dont_reuse_strings reset

static char *memcard_mode_names[] =
{
    "MEMMODE_SAVE",
    "MEMMODE_LOAD",
    "MEMMODE_LIST",
    "MEMMODE_DELETE",
    NULL,
};

static char *memcard_submode_names[] =
{
    "MEMSUB_HALT",
    "MEMSUB_ERROR",
    "MEMSUB_NOSAVE_EXIT",
    "MEMSUB_MOUNT",
    "MEMSUB_CHECK",
    "MEMSUB_FORMAT_QUERY",
    "MEMSUB_FORMAT",
    "MEMSUB_GET_SERIAL",
    "MEMSUB_DELETE_TMP",
    "MEMSUB_CHECK_FREE",
    "MEMSUB_CHECK_EXIST",
    "MEMSUB_OVERWRITE_QUERY",
    "MEMSUB_DELETE_FILE",
    "MEMSUB_CREATE_TMP",
    "MEMSUB_GET_STAT",
    "MEMSUB_SAVE_TMP",
    "MEMSUB_SET_STAT",
    "MEMSUB_DELETE_OLD",
    "MEMSUB_RENAME",
    "MEMSUB_CHECK_FREE_LAST",
    "MEMSUB_SAVE_END",
    "MEMSUB_NOLOAD_EXIT",
    "MEMSUB_FORMAT_END",
    "MEMSUB_LOAD_FILE",
    "MEMSUB_LOAD_END",
    "MEMSUB_GET_LIST",
    "MEMSUB_LIST_END",
    "MEMSUB_NOLIST_EXIT",
    "MEMSUB_DELETE_NAME",
    "MEMSUB_DELETE_END",
    "MEMSUB_NODEL_EXIT",
    "MEMSUB_NOFILE_CHECKFREE",
    "MEMSUB_NOLOAD_EXIT_NOMSG",
    "MEMSUB_NOLOAD_EXIT_NOMSG2",
    NULL,
};

static void (*memcard_begin_funcs[])(struct MemcardRequest *, struct MemcardInfo *) =
{
    fn_1_E9940,
    fn_1_EAFAC,
    fn_1_EBF30,
    fn_1_ECA60,
    NULL,
};

static void (*memcard_sequence_funcs[])(struct MemcardInfo *) =
{
    g_related_to_memcard_rw,
    g_something_with_card12,
    fn_1_EBF3C,
    fn_1_ECA6C,
    NULL,
};

static void (*g_some_card_funcs[])(struct MemcardInfo *) =
{
    print_card_submode_error,
    g_memcard_func_1,
    g_memcard_func_2,
    mount_memory_card,
    g_memcard_func_4,
    fn_1_E854C,
    fn_1_E869C,
    fn_1_E87C4,
    fn_1_E8830,
    fn_1_E88E8,
    fn_1_E8A10,
    fn_1_E8A9C,
    fn_1_E8BEC,
    something_that_calls_CARDCreateAsync,
    fn_1_E8D68,
    g_calls_CARDWriteAsync,
    fn_1_E8F24,
    fn_1_E8FBC,
    fn_1_E9074,
    fn_1_E9150,
    fn_1_E91E4,
    fn_1_E92E4,
    fn_1_E9394,
    fn_1_E93BC,
    fn_1_E945C,
    fn_1_E9584,
    fn_1_E966C,
    fn_1_E955C,
    fn_1_E9784,
    fn_1_E9840,
    fn_1_E9818,
    fn_1_E897C,
    fn_1_E930C,
    fn_1_E9350,
    NULL,
};

static void (*g_some_card_funcs2[])(struct MemcardInfo *) =
{
    fn_1_E7EE0,
    fn_1_E837C,
    fn_1_E83A4,
    fn_1_E8440,
    fn_1_E84F8,
    fn_1_E857C,
    fn_1_E8708,
    fn_1_E87D4,
    fn_1_E8894,
    fn_1_E88F8,
    g_open_card_file,
    fn_1_E8ACC,
    fn_1_E8C28,
    fn_1_E8D14,
    fn_1_E8D78,
    fn_1_E8E58,
    fn_1_E8F68,
    fn_1_E9020,
    fn_1_E90FC,
    fn_1_E9160,
    fn_1_E92E0,
    fn_1_E9308,
    fn_1_E93B8,
    fn_1_E9408,
    fn_1_E9558,
    fn_1_E95C4,
    fn_1_E9780,
    fn_1_E9580,
    fn_1_E97C4,
    fn_1_E993C,
    fn_1_E983C,
    g_something_free_card_blocks,
    fn_1_E934C,
    fn_1_E9390,
    NULL,
};

static const struct MemcardFileDesc memcard_file_descs[3] =
{
    {"super_monkey_ball_2", 0, "sys.bin", 0x12, 'C', 0xF98},
    {"debug", 1, "rep.bin", 0x02, 'D', 0xC10},
    {"rep", 2, "rep.bin", 0x03, 'E', 0},
};

static struct MemcardInfo memcard_infos[2];
static struct MemcardInfo lbl_1_bss_CF368;
static u8 lbl_1_bss_CF3AC;
static u8 g_last_used_memcard_slot;
static u8 lbl_1_bss_CF3AE;
static u32 g_result_code;
static u8 g_some_memcard_var;
static struct
{
    s8 slot;
    s16 unk2;
    s16 unk4;
    s16 unk6;
} g_maybe_last_selected_memcard_slot;
static u8 lbl_1_bss_CF3C0;
static s32 lbl_1_bss_CF3C4;
static u8 lbl_1_bss_CF3C8;
static s32 lbl_1_bss_CF3CC;
u8 storymode_save_files[0xFA0] __attribute__((aligned(32)));
u8 g_storymode_file_memcard_buffer[0xFA0] __attribute__((aligned(32)));
s32 lbl_1_bss_CF3D8;
s32 lbl_1_bss_CF3D4;
s32 lbl_1_bss_CF3D0;

#ifndef __MWERKS__
static int s_pc_memcard_inited;
#endif

void g_something_with_card13(void)
{
    int i;
    struct MemcardInfo *info = memcard_infos;

    for (i = 0; i < 2; i++, info++)
    {
        info->workArea = NULL;
        info->job = NULL;
        info->chan = i;
        info->mode = 0xFF;
        info->submode = 0xFF;
        info->unk4 = 0xFF;
        info->unk5 = 0xFF;
        info->unk6 = 0xFF;
        info->unk7 = 0;
        info->message = -1;
        info->unkA = -1;
        info->xferredBytes = 0;
        info->unk34 = 0.0f;
        info->progress = 0.0f;
        info->flags = 0;
        info->mountResult = -3;
        info->unk2B = -3;
        info->freeBlocks = -1;
        info->freeFiles = -1;
    }
    lbl_1_bss_CF368 = memcard_infos[0];
    lbl_1_bss_CF368.chan = 2;
    lbl_1_bss_CF3AC = 0xFF;
    g_last_used_memcard_slot = 0;
    lbl_1_bss_CF3AE = 0xFF;
    g_result_code = 0;
    g_some_memcard_var = 0;
    g_maybe_last_selected_memcard_slot.unk2 = -1;
#ifndef __MWERKS__
    s_pc_memcard_inited = 1;
#endif
}

#ifndef __MWERKS__
/* pc_port (pc_quit.c): TRUE while a memory card job may still touch the
 * card.  A job sitting in a step that only waits for a button press (its
 * result / error / question box: the steps of memcard_sequence_funcs that
 * poll lbl_1_bss_CF3C0) does no card I/O until answered, so quitting there
 * is safe; any other step of an active job counts as busy. */
static int pc_memcard_slot_busy(struct MemcardInfo *info)
{
    switch (info->mode)
    {
    case 0xFF:
        return FALSE;
    case 0: /* g_related_to_memcard_rw */
        return !(info->unk4 == 1 || info->unk4 == 2 || info->unk4 == 20);
    case 1: /* g_something_with_card12 */
        return !(info->unk4 == 1 || info->unk4 == 21 || info->unk4 == 22 || info->unk4 == 24);
    case 2: /* fn_1_EBF3C */
        return !(info->unk4 == 1 || info->unk4 == 22 || info->unk4 == 26 || info->unk4 == 27);
    case 3: /* fn_1_ECA6C */
        return !(info->unk4 == 1 || info->unk4 == 29 || info->unk4 == 30);
    default:
        return TRUE;
    }
}

int pc_memcard_io_busy(void)
{
    /* the slots are only valid (mode 0xFF = idle) after the init above;
     * before it they are zero */
    if (!s_pc_memcard_inited)
        return FALSE;
    return pc_memcard_slot_busy(&memcard_infos[0]) || pc_memcard_slot_busy(&memcard_infos[1])
        || pc_memcard_slot_busy(&lbl_1_bss_CF368);
}

/* pc_port (pc_quit.c): job state of slot ch (0, 1, 2 = the CF368 slot):
 * -1 idle, else (mode << 8) | step (the submode being run, unk4). */
int pc_memcard_job_state(int ch)
{
    struct MemcardInfo *info = ch == 2 ? &lbl_1_bss_CF368 : &memcard_infos[ch];

    if (!s_pc_memcard_inited || info->mode == 0xFF)
        return -1;
    return (info->mode << 8) | info->unk4;
}
#endif

void g_some_replay_func4(u8 mode, u8 slot, struct MemcardRequest *req)
{
    struct MemcardInfo *info;
    int a;
    int b;

#ifndef __MWERKS__
    /* pc_port: there is exactly one "card" (memcard/slotA next to the exe).
     * Send every request to Slot A, so the Slot A/B chooser (slot 2, or
     * slot 4 when the choice is ambiguous) and Slot B are never used. */
    slot = 0;
#endif
    switch (slot)
    {
    case 0:
    case 1:
        info = &memcard_infos[slot];
        g_last_used_memcard_slot = slot;
        break;
    case 4:
        a = memcard_infos[0].mountResult != (s8)-3;
        b = memcard_infos[1].mountResult != (s8)-3;
        if (a && !b)
        {
            g_last_used_memcard_slot = 0;
            info = &memcard_infos[g_last_used_memcard_slot];
            break;
        }
        if (b && !a)
        {
            g_last_used_memcard_slot = 1;
            info = &memcard_infos[g_last_used_memcard_slot];
            break;
        }
    case 2:
        info = &lbl_1_bss_CF368;
        lbl_1_bss_CF3AE = 2;
        break;
    case 3:
        info = &memcard_infos[g_last_used_memcard_slot];
        break;
    }
    info->job = alloc_from_heap_or_panic(currentHeap, 0xC0, "memcard.c", 0x1BA);
    g_something_with_locked_cache_2(info->job, 0, 0xC0);
    info->job->flags = req->flags;
    info->job->file = req->file;
    info->job->desc = &memcard_file_descs[req->file];
    info->mode = mode;
    g_some_memcard_var = 0;
    if (mode == 2)
    {
        info->job->flags |= 0x04000000;
        info->job->buffer = req->buffer;
    }
    else if (mode == 3)
    {
        info->job->flags |= 0x04000000;
        strcpy(info->job->name, info->job->desc->name);
        strncat(info->job->name, req->name, 0x1F - strlen(info->job->name));
        strcat(info->job->name, ".dat");
    }
    else if (info->job->desc->size == 0)
    {
        info->job->flags |= 0x04000000;
        info->job->dataSize = req->size;
        info->job->size = (req->size + 0x405F) & ~0x1FFF;
        strcpy(info->job->name, info->job->desc->name);
        strncat(info->job->name, req->name, 0x1F - strlen(info->job->name));
        info->job->buffer = req->buffer;
    }
    else
    {
        info->job->dataSize = info->job->desc->size;
        info->job->size = (info->job->desc->size + 0x405F) & ~0x1FFF;
        strcpy(info->job->name, info->job->desc->name);
        info->job->buffer = alloc_from_heap_or_panic(currentHeap, info->job->size, "memcard.c", 0x1EE);
        g_something_with_locked_cache_2(info->job->buffer, 0, info->job->size);
    }
    memcard_begin_funcs[mode](req, info);
}

/* Not linked. Whatever used these debug strings was stripped, but they stay in .data. */
static inline void load_save_header(struct MemcardInfo *info)
{
    const struct MemcardFileDesc *desc;
    char path[0x40];
    struct File file;

    info->submode = 13;
    desc = info->job->desc;
    info->job->buffer[2] = info->job->file;
    info->job->buffer[3] = (u8)desc->unk3E;
    strncpy((char *)info->job->buffer + 4, "Super Monkey Ball 2", 0x20);
    strcpy(path, "./memcard/");
    strcat(path, desc->file);
    if (!file_open(path, &file))
        OSPanic("memcard.c", 0x31C, "Open: cannot open.\n");
    if (info->job->flags & 0x04000000)
    {
        if (!file_read(&file, info->job->buffer + 0x1860, 0x800, 0x1800))
            OSPanic("memcard.c", 0x321, "Read: cannot read");
    }
    else
    {
        if (!file_read(&file, info->job->buffer + 0x60, 0x2000, 0))
            OSPanic("memcard.c", 0x325, "Read: cannot read");
    }
    file_close(&file);
    info->job->flags |= 0x00400000;
}

void memcard_unused_load_save_header(struct MemcardInfo *info)
{
    load_save_header(info);
    lbl_1_bss_CF3C0 = lbl_1_bss_CF3C4;
}

static inline int handle_slot_input(u8 *slot)
{
    if ((lbl_1_bss_CF3C0 & 1) && *slot == 1)
    {
        *slot = 0;
        lbl_1_bss_CF3C4 = 0;
    }
    else if ((lbl_1_bss_CF3C0 & 2) && *slot == 0)
    {
        *slot = 1;
        lbl_1_bss_CF3C4 = 0;
    }
    if (lbl_1_bss_CF3C0 & 0x20)
    {
        lbl_1_bss_CF3C4 = 0;
        return 2;
    }
    if (lbl_1_bss_CF3C0 & 0x10)
    {
        lbl_1_bss_CF3C4 = 0;
        return 1;
    }
    return 0;
}

static inline void reset_memcard_info(struct MemcardInfo *info)
{
    info->job = NULL;
    info->mode = 0xFF;
    info->submode = 0xFF;
    info->unk4 = 0xFF;
    info->unk5 = 0xFF;
    info->unk6 = 0xFF;
    info->unk7 = 0;
    info->message = -1;
    info->unkA = -1;
    info->xferredBytes = 0;
    info->unk34 = 0.0f;
    info->progress = 0.0f;
}

void g_something_with_card3(void)
{
    struct MemcardInfo *info;

    if (lbl_1_bss_CF3C8 == 0)
        lbl_1_bss_CF3C8 = handle_slot_input(&lbl_1_bss_CF3AE);
    switch (lbl_1_bss_CF3C8)
    {
    case 0:
        break;
    case 2:
        {
        info = &lbl_1_bss_CF368;
        switch (info->mode)
        {
        case 1:
            info->submode = 0x21;
            break;
        case 0:
            info->submode = 2;
            break;
        case 2:
            info->submode = 0x1B;
            break;
        case 3:
            info->submode = 0x1E;
            break;
        }
        }
        lbl_1_bss_CF3AE = 0;
    case 1:
        if (lbl_1_bss_CF3C8 == 1)
        {
            int done;

            if (!(lbl_1_bss_CF368.flags & 1))
            {
                lbl_1_bss_CF368.timer = 12;
                lbl_1_bss_CF368.flags |= 1;
                done = 0;
            }
            else if (lbl_1_bss_CF368.timer > 0)
            {
                lbl_1_bss_CF368.timer--;
                done = 0;
            }
            else
                done = 1;
            if (!done)
                break;
        }
        g_last_used_memcard_slot = lbl_1_bss_CF3AE;
        info = &memcard_infos[g_last_used_memcard_slot];
        info->mode = lbl_1_bss_CF368.mode;
        info->submode = lbl_1_bss_CF368.submode;
        info->unk4 = 0xFF;
        info->unk5 = 0xFF;
        info->unk6 = 0xFF;
        info->unk7 = 0;
        info->job = lbl_1_bss_CF368.job;
        if (lbl_1_bss_CF3C8 == 2)
        {
            info->flags |= 4;
            if (info->job != NULL)
                info->job->flags &= ~0x20000000;
        }
        lbl_1_bss_CF368.job->flags &= ~0x40000000;
        reset_memcard_info(&lbl_1_bss_CF368);
        lbl_1_bss_CF368.flags &= ~4;
        lbl_1_bss_CF368.freeBlocks = -1;
        lbl_1_bss_CF368.freeFiles = -1;
        lbl_1_bss_CF3AE = 0xFF;
        break;
    }
}

void print_card_submode_error(struct MemcardInfo *info)
{
    OSReport("%1d Wrong Submode.\nresult: %d\nsubmode:%s\n", info->chan, info->job->result,
             memcard_submode_names[info->unk5]);
    OSPanic("memcard.c", 0x33C, "halt.\n");
}

void fn_1_E7EE0(struct MemcardInfo *info) {}

void g_memcard_func_1(struct MemcardInfo *info)
{
    switch (info->job->result)
    {
    case -2:
        info->message = 1;
        break;
    case -3:
        info->message = 0;
        break;
    case -6:
    case -5:
        info->message = 0x1C;
        break;
    case -13:
        info->message = 0x1D;
        break;
    case -8:
        switch (info->job->desc->language)
        {
        default:
            info->message = 0x25;
            break;
        case 'E':
            info->message = 0x26;
            break;
        }
        break;
    case -9:
        switch (info->job->desc->language)
        {
        default:
            info->message = 0x27;
            break;
        case 'E':
            info->message = 0x28;
            break;
        }
        break;
    case -4:
        info->message = 0x33;
        break;
    case -123:
        info->message = 0x32;
        break;
    case -124:
        info->message = 0x31;
        break;
    case -125:
        info->message = 0x2A;
        break;
    case -126:
        info->message = 0x24;
        break;
    case -127:
        info->message = 0x1B;
        break;
    case -128:
        info->message = 2;
        break;
    case -122:
        info->message = 3;
        break;
    default:
        OSReport("result: %d\n", info->job->result);
        OSPanic("memcard.c", 0x3A2, "Wrong error occured.\n");
        break;
    }
    switch (info->mode)
    {
    case 1:
        switch ((int)(info->job->flags & 0xC0300000))
        {
        case 0x00100000:
            info->unkA = 0xD;
            break;
        case 0xC0000000:
            info->unkA = 0xC;
            break;
        case 0x80000000:
            info->unkA = 4;
            break;
        case 0x40000000:
            info->unkA = 0xE;
            break;
        case 0x00200000:
            info->unkA = 0x12;
            break;
        default:
            info->unkA = 8;
            break;
        }
        break;
    case 0:
        switch ((int)(info->job->flags & 0xC0200000))
        {
        case 0xC0000000:
            info->unkA = 0xC;
            break;
        case 0x80000000:
            info->unkA = 5;
            break;
        case 0x40000000:
            info->unkA = 0xF;
            break;
        case 0x00200000:
            info->unkA = 0x13;
            break;
        default:
            info->unkA = 9;
            break;
        }
        break;
    case 2:
        switch ((int)(info->job->flags & 0xC0200000))
        {
        case 0xC0000000:
            info->unkA = 0xC;
            break;
        case 0x80000000:
            info->unkA = 6;
            break;
        case 0x40000000:
            info->unkA = 0x10;
            break;
        case 0x00200000:
            info->unkA = 0x14;
            break;
        default:
            info->unkA = 0xA;
            break;
        }
        break;
    case 3:
        switch ((int)(info->job->flags & 0xC0200000))
        {
        case 0xC0000000:
            info->unkA = 0xC;
            break;
        case 0x80000000:
            info->unkA = 7;
            break;
        case 0x40000000:
            info->unkA = 0x11;
            break;
        case 0x00200000:
            info->unkA = 0x15;
            break;
        default:
            info->unkA = 0xB;
            break;
        }
        break;
    default:
        OSPanic("memcard.c", 0x3FB, "Please make caption list.\n");
        break;
    }
#ifndef __MWERKS__
    /* pc_port: no second slot, so drop "X: Choose a Slot" from the error
     * captions (handle_error_input ignores X on PC as well). */
    if (info->unkA >= 8 && info->unkA <= 0xB)
        info->unkA -= 4;    /* "... / Y: Retry / X: Choose a Slot" -> "... / Y: Retry" */
    else if (info->unkA >= 0xE && info->unkA <= 0x11)
        info->unkA = 0xC;   /* "... / X: Choose a Slot" -> "Please press A." */
#endif
    info->flags |= 4;
    info->job->flags &= ~0x20000000;
    info->job->timeout = 0;
}

void fn_1_E837C(struct MemcardInfo *info) {}

void g_memcard_func_2(struct MemcardInfo *info)
{
    info->message = 0x1A;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E83A4(struct MemcardInfo *info) {}

void fn_1_E83A8(s32 chan, s32 result)
{
    memcard_infos[chan].unk7 = 1;
}

void mount_memory_card(struct MemcardInfo *info)
{
    info->workArea = alloc_from_heap_or_panic(currentHeap, 0xA000, "memcard.c", 0x429);
    info->job->timeout = 300;
    info->unk7 = 0;
    CARDMountAsync(info->chan, info->workArea, fn_1_E83A8, NULL);
}

void fn_1_E8440(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result == -3 && info->mountResult == -2)
        info->job->result = info->mountResult;
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void g_memcard_func_4(struct MemcardInfo *info)
{
    info->job->timeout = 300;
    CARDCheckAsync(info->chan, NULL);
}

void fn_1_E84F8(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E854C(struct MemcardInfo *info)
{
    info->job->unk1 = 1;
    info->message = -1;
    info->unkA = -1;
    info->flags &= ~1;
}

void fn_1_E857C(struct MemcardInfo *info)
{
    if (!(info->flags & 1))
    {
        info->job->result = handle_slot_input(&info->job->unk1);
        if (info->job->result == 2)
            info->job->unk1 = 1;
    }
}

void fn_1_E869C(struct MemcardInfo *info)
{
    info->job->timeout = 300;
    info->xferredBytes = CARDGetXferredBytes(info->chan);
    info->progress = 0.0f;
    info->flags |= 2;
    CARDFormatAsync(info->chan, NULL);
}

void fn_1_E8708(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    info->progress = (float)(s32)(CARDGetXferredBytes(info->chan) - info->xferredBytes) / 40960.0f;
    if (info->job->result != -1)
    {
        info->flags &= ~2;
        info->job->timeout = 0;
        info->flags &= ~0x10;
    }
}

void fn_1_E87C4(struct MemcardInfo *info)
{
    info->job->timeout = 300;
}

void fn_1_E87D4(struct MemcardInfo *info)
{
    info->job->result = CARDGetSerialNo(info->chan, &info->job->serial);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E8830(struct MemcardInfo *info)
{
    char name[0x40];

    strcpy(name, info->job->name);
    strcat(name, ".000");
    info->job->timeout = 300;
    CARDDeleteAsync(info->chan, name, NULL);
}

void fn_1_E8894(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E88E8(struct MemcardInfo *info)
{
    info->job->timeout = 300;
}

void fn_1_E88F8(struct MemcardInfo *info)
{
    info->job->result = CARDFreeBlocks(info->chan, &info->job->freeBytes, &info->job->freeFiles);
    if (info->job->result == 0)
        info->job->flags |= 0x01000000;
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E897C(struct MemcardInfo *info)
{
    info->job->timeout = 300;
}

void g_something_free_card_blocks(struct MemcardInfo *info)
{
    info->job->result = CARDFreeBlocks(info->chan, &info->job->freeBytes, &info->job->freeFiles);
    if (info->job->result == 0)
        info->job->flags |= 0x01000000;
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E8A10(struct MemcardInfo *info)
{
    info->job->timeout = 300;
}

void g_open_card_file(struct MemcardInfo *info)
{
    char name[0x40];

    strcpy(name, info->job->name);
    strcat(name, ".dat");
    info->job->result = CARDOpen(info->chan, name, &info->fileInfo);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E8A9C(struct MemcardInfo *info)
{
    info->job->unk1 = 1;
    info->message = -1;
    info->unkA = -1;
    info->flags &= ~1;
}

void fn_1_E8ACC(struct MemcardInfo *info)
{
    if (!(info->flags & 1))
    {
        info->job->result = handle_slot_input(&info->job->unk1);
        if (info->job->result == 2)
            info->job->unk1 = 1;
    }
}

void fn_1_E8BEC(struct MemcardInfo *info)
{
    info->job->timeout = 300;
    CARDFastDeleteAsync(info->chan, info->fileInfo.fileNo, NULL);
}

void fn_1_E8C28(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void something_that_calls_CARDCreateAsync(struct MemcardInfo *info)
{
    char name[0x40];

    strcpy(name, info->job->name);
    if (info->job->flags & 0x04000000)
        strcat(name, ".dat");
    else
        strcat(name, ".000");
    info->job->timeout = 300;
    CARDCreateAsync(info->chan, name, info->job->size, &info->fileInfo, NULL);
}

void fn_1_E8D14(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E8D68(struct MemcardInfo *info)
{
    info->job->timeout = 300;
}

void fn_1_E8D78(struct MemcardInfo *info)
{
    info->job->result = CARDGetStatus(info->chan, info->fileInfo.fileNo, &info->job->stat);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void g_calls_CARDWriteAsync(struct MemcardInfo *info)
{
    info->job->timeout = 300;
    info->xferredBytes = CARDGetXferredBytes(info->chan);
    info->progress = 0.0f;
    info->flags |= 2;
    CARDWriteAsync(&info->fileInfo, info->job->buffer, info->job->size, 0, NULL);
}

void fn_1_E8E58(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    info->progress = (float)(s32)(CARDGetXferredBytes(info->chan) - info->xferredBytes) / (float)(info->job->size + 0x2000);
    if (info->job->result != -1)
    {
        info->flags &= ~2;
        info->job->timeout = 0;
    }
}

void fn_1_E8F24(struct MemcardInfo *info)
{
    info->job->timeout = 300;
    CARDSetStatusAsync(info->chan, info->fileInfo.fileNo, &info->job->stat, NULL);
}

void fn_1_E8F68(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E8FBC(struct MemcardInfo *info)
{
    char name[0x40];

    strcpy(name, info->job->name);
    strcat(name, ".dat");
    info->job->timeout = 300;
    CARDDeleteAsync(info->chan, name, NULL);
}

void fn_1_E9020(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E9074(struct MemcardInfo *info)
{
    char oldName[0x40];
    char newName[0x40];

    strcpy(oldName, info->job->name);
    strcpy(newName, info->job->name);
    strcat(oldName, ".000");
    strcat(newName, ".dat");
    info->job->timeout = 300;
    CARDRenameAsync(info->chan, oldName, newName, NULL);
}

void fn_1_E90FC(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E9150(struct MemcardInfo *info)
{
    info->job->timeout = 300;
}

void fn_1_E9160(struct MemcardInfo *info)
{
    info->job->result = CARDFreeBlocks(info->chan, &info->job->freeBytes, &info->job->freeFiles);
    if (info->job->result == 0)
        info->job->flags |= 0x01000000;
    if (info->job->result != -1)
        info->job->timeout = 0;
}

static inline void finish_memcard_job(struct MemcardInfo *info)
{
    if (info->workArea != NULL)
    {
        while (CARDUnmount(info->chan) == -1)
            ;
        OSFreeToHeap(currentHeap, info->workArea);
        info->unk7 = 0;
        info->workArea = NULL;
        info->job->serial = 0;
    }
    if (info->job->flags & 0x01000000)
    {
        info->freeBlocks = info->job->freeBytes / 0x2000;
        info->freeFiles = info->job->freeFiles;
    }
    if (!(info->job->flags & 0x04000000))
        OSFreeToHeap(currentHeap, info->job->buffer);
}

void fn_1_E91E4(struct MemcardInfo *info)
{
    finish_memcard_job(info);
    info->message = 0x2B;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E92E0(struct MemcardInfo *info) {}

void fn_1_E92E4(struct MemcardInfo *info)
{
    info->message = 0x2E;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E9308(struct MemcardInfo *info) {}

void fn_1_E930C(struct MemcardInfo *info)
{
    info->flags |= 4;
    if (info->job != NULL)
        info->job->result = -120;
    else
        g_result_code = -120;
}

void fn_1_E934C(struct MemcardInfo *info) {}

void fn_1_E9350(struct MemcardInfo *info)
{
    info->flags |= 4;
    if (info->job != NULL)
        info->job->result = -119;
    else
        g_result_code = -119;
}

void fn_1_E9390(struct MemcardInfo *info) {}

void fn_1_E9394(struct MemcardInfo *info)
{
    info->message = 0x2F;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E93B8(struct MemcardInfo *info) {}

void fn_1_E93BC(struct MemcardInfo *info)
{
    info->job->timeout = 300;
    CARDReadAsync(&info->fileInfo, info->job->buffer, info->job->size, 0, NULL);
}

void fn_1_E9408(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E945C(struct MemcardInfo *info)
{
    finish_memcard_job(info);
    info->message = 0x30;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E9558(struct MemcardInfo *info) {}

void fn_1_E955C(struct MemcardInfo *info)
{
    info->message = 0x34;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E9580(struct MemcardInfo *info) {}

void fn_1_E9584(struct MemcardInfo *info)
{
    info->unk2C = 0;
    info->unk2D = 0;
    info->progress = 0.0f;
    info->flags |= 2;
    info->job->timeout = 300;
}

void fn_1_E95C4(struct MemcardInfo *info)
{
    info->job->result = CARDGetStatus(info->chan, info->unk2C, &info->job->stat);
    info->progress = info->unk2C / 126.0;
    if (info->unk2C == 126)
    {
        info->flags &= ~2;
        info->job->timeout = 0;
    }
}

void fn_1_E966C(struct MemcardInfo *info)
{
    finish_memcard_job(info);
    if (info->unk2D == 0)
        info->message = 0x35;
    else
        info->message = 0x36;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E9780(struct MemcardInfo *info) {}

void fn_1_E9784(struct MemcardInfo *info)
{
    info->job->timeout = 300;
    CARDDeleteAsync(info->chan, info->job->name, NULL);
}

void fn_1_E97C4(struct MemcardInfo *info)
{
    info->job->result = CARDGetResultCode(info->chan);
    if (info->job->result != -1)
        info->job->timeout = 0;
}

void fn_1_E9818(struct MemcardInfo *info)
{
    info->message = 0x37;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E983C(struct MemcardInfo *info) {}

void fn_1_E9840(struct MemcardInfo *info)
{
    finish_memcard_job(info);
    info->message = 0x38;
    info->unkA = 0xC;
    info->flags |= 4;
}

void fn_1_E993C(struct MemcardInfo *info) {}

void fn_1_E9940(struct MemcardRequest *req, struct MemcardInfo *info)
{
    struct MemcardRequest *r = req;

    if (!(info->job->flags & 0x04000000))
    {
        if (r->comment != NULL)
            strncpy(info->job->buffer + 0x24, r->comment, 0x20);
        memcpy_handler(info->job->buffer + 0x2060, r->buffer, info->job->dataSize);
    }
    info->submode = 3;
}

static inline int memcard_wait_12(struct MemcardInfo *info)
{
    if (!(info->flags & 1))
    {
        info->timer = 12;
        info->flags |= 1;
        return 0;
    }
    if (info->timer > 0)
    {
        info->timer--;
        return 0;
    }
    return 1;
}

static inline int memcard_wait_30(struct MemcardInfo *info)
{
    if (!(info->job->flags & 0x10000000))
    {
        info->timer = 30;
        info->job->flags |= 0x10000000;
        return 0;
    }
    if (info->timer > 0)
    {
        info->timer--;
        return 0;
    }
    return 1;
}

static inline void memcard_end_job(struct MemcardInfo *info)
{
    g_result_code = info->job->result;
    OSFreeToHeap(currentHeap, info->job);
    reset_memcard_info(info);
    info->flags &= ~4;
    g_some_memcard_var = 1;
}

static inline void handle_error_input(struct MemcardInfo *info)
{
    if ((lbl_1_bss_CF3C0 & 0x80) && !(info->job->flags & 0x40000000))
    {
        if (info->unk5 == 3 && info->workArea != NULL)
        {
            while (CARDUnmount(info->chan) == -1)
                ;
            OSFreeToHeap(currentHeap, info->workArea);
            info->unk7 = 0;
            info->workArea = NULL;
            info->job->serial = 0;
        }
        if (info->unk6 != 0xFF)
        {
            info->submode = info->unk6;
            info->unk6 = 0xFF;
        }
        else
            info->submode = info->unk5;
        info->message = -1;
        info->unkA = -1;
        info->flags &= ~4;
    }
    else if ((lbl_1_bss_CF3C0 & 0x40) && (info->job->flags & 0x00200000))
    {
        info->submode = 5;
        info->job->flags &= ~0x00200000;
        info->job->flags |= 0x00400000;
        info->flags &= ~4;
        info->flags |= 0x10;
    }
#ifdef __MWERKS__
    else if ((lbl_1_bss_CF3C0 & 0x40) && !(info->job->flags & 0x80000000))
#else
    /* pc_port: X = "Choose a Slot" would hand the job to the Slot A/B
     * chooser; there is no Slot B, so never take this branch. */
    else if (0)
#endif
    {
        if (memcard_infos[0].mode == 0xFF || memcard_infos[1].mode == 0xFF)
        {
            if (info->workArea != NULL)
            {
                while (CARDUnmount(info->chan) == -1)
                    ;
                OSFreeToHeap(currentHeap, info->workArea);
                info->workArea = NULL;
                info->job->serial = 0;
            }
            lbl_1_bss_CF368.mode = info->mode;
            lbl_1_bss_CF368.job = info->job;
            lbl_1_bss_CF368.job->result = 0;
            lbl_1_bss_CF368.submode = 3;
            info->job->flags &= ~0x40000000;
            reset_memcard_info(info);
            info->flags &= ~4;
            info->freeBlocks = -1;
            info->freeFiles = -1;
            lbl_1_bss_CF3AE = 2;
            info->flags &= ~4;
        }
    }
}

void g_related_to_memcard_rw(struct MemcardInfo *info)
{
    if (info->unk4 == 0xFF)
        return;
    switch (info->unk4)
    {
    case 0:
    default:
        OSReport("submode    : %d\n", info->unk4);
        OSReport("submode req: %d\n", info->submode);
        OSPanic("memcard.c", 0x6CD, "submode error!!\n");
        break;
    case 1:
        info->job->flags &= ~0x00400000;
        if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            info->submode = 2;
        else
            handle_error_input(info);
        lbl_1_bss_CF3C0 = 0;
        break;
    case 2:
        if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            info->mode = 0xFE;
        break;
    case 3:
        if (info->mountResult == -127)
        {
            info->job->result = info->mountResult;
            info->submode = 1;
            break;
        }
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
        case -2:
            info->submode = 1;
            break;
        case -13:
            if (locale == 5)
            {
                info->submode = 5;
                break;
            }
            info->job->flags |= 0x00200000;
            info->submode = 1;
            break;
        case -6:
        case 0:
            info->submode = 4;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 4:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case -6:
            info->submode = 5;
            break;
        case -13:
            if (locale == 5)
            {
                info->submode = 5;
                break;
            }
            info->job->flags |= 0x00200000;
            info->submode = 1;
            break;
        case 0:
            info->submode = 7;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 5:
        if (info->job->result == 0)
            break;
        if (memcard_wait_12(info))
        {
            if (info->job->result == 2 || info->job->unk1 == 1)
            {
                info->job->result = -126;
                info->submode = 1;
                break;
            }
            info->submode = 6;
            info->job->flags |= 0x00400000;
        }
        break;
    case 6:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 7;
            info->job->flags &= ~0x00400000;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 7:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 8;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 8:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case -4:
        case 0:
            info->submode = 9;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 9:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -6:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 10;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 10:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -6:
        case -3:
            info->submode = 1;
            break;
        case -4:
            if (info->job->freeFiles == 0)
            {
                info->job->result = -8;
                info->submode = 1;
                break;
            }
            if (info->job->freeBytes < info->job->size)
            {
                info->job->result = -9;
                info->submode = 1;
                break;
            }
            load_save_header(info);
            break;
        case 0:
#ifdef __MWERKS__
            if (info->job->flags & 0x20000000)
#else
            /* pc_port: the Game Data file is the player's one save, so
             * "The Game Data exists in Slot A. Overwrite?" only exists because
             * of card handling: overwrite it like the auto-save does.  Replay
             * files (file 2) keep the question. */
            if ((info->job->flags & 0x20000000) || info->job->file == 0)
#endif
            {
                if (info->job->freeBytes < info->job->size || info->job->freeFiles == 0)
                {
                    info->submode = 12;
                    break;
                }
                load_save_header(info);
                break;
            }
            info->submode = 11;
            break;
        case -5:
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 11:
        if (info->job->result == 0)
            break;
        if (memcard_wait_12(info))
        {
            if (info->job->result == 2 || info->job->unk1 == 1)
            {
                info->job->result = -125;
                info->submode = 1;
                break;
            }
            if (info->job->freeBytes < info->job->size || info->job->freeFiles == 0)
            {
                info->submode = 12;
                break;
            }
            load_save_header(info);
        }
        break;
    case 12:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -5:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
            load_save_header(info);
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 13:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -12:
        case -9:
        case -8:
        case -7:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 14;
            break;
        case -6:
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 14:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->job->stat.commentAddr = (u32)(info->job->buffer + 4) - (u32)info->job->buffer;
            info->job->stat.bannerFormat = (u8)((info->job->stat.bannerFormat & ~3) | 2);
            info->job->stat.iconAddr = (u32)(info->job->buffer + 0x60) - (u32)info->job->buffer;
            info->job->stat.iconFormat = (u16)((info->job->stat.iconFormat & ~3) | 2);
            info->job->stat.iconSpeed = (u16)((info->job->stat.iconSpeed & ~3) | 3);
            info->job->stat.iconSpeed = (u16)(info->job->stat.iconSpeed & ~0xC);
            info->job->stat.bannerFormat = (u8)(info->job->stat.bannerFormat & ~4);
            *(u16 *)info->job->buffer = fn_800648C8(info->job->size - 2, info->job->buffer + 2);
            info->submode = 15;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 15:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -14:
        case -11:
        case -10:
        case -5:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 16;
            break;
        case -9:
        case -8:
        case -7:
        case -6:
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 16:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -5:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
            if (info->job->flags & 0x04000000)
            {
                info->submode = 19;
                info->job->flags &= ~0x00400000;
                break;
            }
            info->submode = 17;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 17:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case -4:
        case 0:
            info->submode = 18;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 18:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -12:
        case -10:
        case -7:
        case -5:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 19;
            info->job->flags &= ~0x00400000;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 19:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -6:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 20;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 20:
        if (info->job->flags & 0x20000000)
        {
            if (memcard_wait_30(info))
                memcard_end_job(info);
        }
        else if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            memcard_end_job(info);
        break;
    }
    if (info->job != NULL)
        g_result_code = info->job->result;
}

void fn_1_EAFAC(struct MemcardRequest *req, struct MemcardInfo *info)
{
    if (!(info->job->flags & 0x04000000))
        info->job->dest = req->buffer;
    info->submode = 3;
}

void g_something_with_card12(struct MemcardInfo *info)
{
    if (info->unk4 == 0xFF)
        return;
    switch (info->unk4)
    {
    default:
        OSReport("submode    : %d\n", info->unk4);
        OSReport("submode req: %d\n", info->submode);
        OSPanic("memcard.c", 0x95A, "submode error!!\n");
        break;
    case 1:
        info->job->flags &= ~0x00400000;
        if ((lbl_1_bss_CF3C0 & 0x10) && (info->job->flags & 0x00100000))
        {
            info->submode = 12;
            info->job->flags &= ~0x00100000;
            info->job->flags |= 0x00400000;
            info->flags &= ~4;
        }
        else if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
        {
            if (lbl_1_bss_CF3C0 & 0x10)
                info->submode = 21;
            else
                info->submode = 32;
        }
        else
            handle_error_input(info);
        lbl_1_bss_CF3C0 = 0;
        break;
    case 21:
        if (lbl_1_bss_CF3C0 & 0x10)
            info->mode = 0xFE;
        else if (lbl_1_bss_CF3C0 & 0x20)
            info->submode = 32;
        break;
    case 32:
        info->mode = 0xFE;
        break;
    case 33:
        info->mode = 0xFE;
        break;
    case 3:
        if (info->mountResult == -127)
        {
            info->job->result = info->mountResult;
            info->submode = 1;
            break;
        }
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
        case -2:
            info->submode = 1;
            break;
        case -13:
            if (locale == 5)
            {
                info->submode = 5;
                break;
            }
            info->job->flags |= 0x00200000;
            info->submode = 1;
            break;
        case -6:
        case 0:
            info->submode = 4;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 4:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case -6:
            info->submode = 5;
            break;
        case -13:
            if (locale == 5)
            {
                info->submode = 5;
                break;
            }
            info->job->flags |= 0x00200000;
            info->submode = 1;
            break;
        case 0:
            info->submode = 7;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 5:
        if (info->job->result == 0)
            break;
        if (memcard_wait_12(info))
        {
            if (info->job->result == 2 || info->job->unk1 == 1)
            {
                info->job->result = -126;
                info->submode = 1;
                break;
            }
            info->submode = 6;
            info->job->flags |= 0x00400000;
        }
        break;
    case 6:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 22;
            info->job->flags &= ~0x00400000;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 22:
        if (lbl_1_bss_CF3C0 & 0x10)
        {
            info->submode = 21;
            info->job->result = -121;
        }
        else if (lbl_1_bss_CF3C0 & 0x20)
        {
            info->submode = 32;
            info->job->result = -121;
        }
        break;
    case 7:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 10;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 10:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -6:
        case -3:
            info->submode = 1;
            break;
        case -4:
            info->submode = 18;
            break;
        case 0:
            info->submode = 14;
            break;
        case -5:
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 18:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -12:
        case -10:
        case -7:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case -4:
            if (info->job->flags & 0x00800000)
            {
                info->submode = 31;
                break;
            }
            info->submode = 1;
            break;
        case 0:
            info->submode = 10;
            break;
        case -11:
        case -9:
        case -8:
        case -6:
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 31:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -6:
        case -3:
            info->submode = 1;
            break;
        case 0:
            if (info->job->freeFiles == 0)
            {
                info->job->result = -8;
                info->mode = 0xFE;
                break;
            }
            if (info->job->freeBytes < info->job->size)
            {
                info->job->result = -9;
                info->mode = 0xFE;
                break;
            }
            info->job->result = -4;
#ifndef __MWERKS__
            /* pc_port: the card is fine but has no game file yet (first start).
             * Skip "There is no Game Data in Slot A" and end the job quietly,
             * like the no-space cases above; the boot code (adv.c) then runs
             * the game's own "save new file" path. */
            info->mode = 0xFE;
            break;
#endif
            info->submode = 1;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 14:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 23;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 23:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -14:
        case -11:
        case -10:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
#ifdef __MWERKS__
            if (*(u16 *)info->job->buffer != fn_800648C8(info->job->size - 2, info->job->buffer + 2))
#else
            /* pc_port: early PC builds wrote a 0 checksum (fn_800648C8);
             * accept those files (the next save writes a real checksum)
             * instead of calling them corrupt. */
            if (*(u16 *)info->job->buffer != fn_800648C8(info->job->size - 2, info->job->buffer + 2)
             && *(u16 *)info->job->buffer != 0)
#endif
            {
#ifndef __MWERKS__
                pc_card_backup_file(info->chan, info->fileInfo.fileNo, "corrupt");
#endif
                info->job->result = -124;
                info->job->flags |= 0x00100000;
                info->submode = 1;
                break;
            }
            if ((u8)info->job->buffer[3] != info->job->desc->unk3E)
            {
#ifndef __MWERKS__
                pc_card_backup_file(info->chan, info->fileInfo.fileNo, "badversion");
#endif
                info->job->result = -122;
                info->submode = 1;
                break;
            }
            if (!(info->job->flags & 0x04000000))
                memcpy_handler(info->job->dest, info->job->buffer + 0x2060, info->job->dataSize);
            info->submode = 8;
            break;
        case -9:
        case -8:
        case -7:
        case -6:
        case -5:
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 8:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case -4:
        case 0:
            info->submode = 9;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 12:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -5:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
#ifndef __MWERKS__
            /* pc_port: the player deleted a corrupt game file at boot (it was
             * backed up to memcard/backup/ first).  Report "no file" so the
             * boot code creates a fresh one instead of running unsaved. */
            if (info->job->flags & 0x00800000)
                info->job->result = -4;
#endif
            info->submode = 21;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 9:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -6:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 24;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 24:
        if (info->job->flags & 0x20000000)
        {
            if (memcard_wait_30(info))
                memcard_end_job(info);
        }
        else if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            memcard_end_job(info);
        break;
    }
    if (info->job != NULL)
        g_result_code = info->job->result;
}

void fn_1_EBF30(struct MemcardRequest *req, struct MemcardInfo *info)
{
    info->submode = 3;
}

void fn_1_EBF3C(struct MemcardInfo *info)
{
    if (info->unk4 == 0xFF)
        return;
    switch (info->unk4)
    {
    default:
        OSReport("submode    : %d\n", info->unk4);
        OSReport("submode req: %d\n", info->submode);
        OSPanic("memcard.c", 0xB88, "submode error!!\n");
        break;
    case 1:
        info->job->flags &= ~0x00400000;
        if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            info->submode = 27;
        else
            handle_error_input(info);
        lbl_1_bss_CF3C0 = 0;
        break;
    case 27:
        if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            info->mode = 0xFE;
        break;
    case 3:
        if (info->mountResult == -127)
        {
            info->job->result = info->mountResult;
            info->submode = 1;
            break;
        }
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
        case -2:
            info->submode = 1;
            break;
        case -13:
            if (locale == 5)
            {
                info->submode = 5;
                break;
            }
            info->job->flags |= 0x00200000;
            info->submode = 1;
            break;
        case -6:
        case 0:
            info->submode = 4;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 4:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case -6:
            info->submode = 5;
            break;
        case -13:
            if (locale == 5)
            {
                info->submode = 5;
                break;
            }
            info->job->flags |= 0x00200000;
            info->submode = 1;
            break;
        case 0:
            info->submode = 7;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 5:
        if (info->job->result == 0)
            break;
        if (memcard_wait_12(info))
        {
            if (info->job->result == 2 || info->job->unk1 == 1)
            {
                info->job->result = -126;
                info->submode = 1;
                break;
            }
            info->submode = 6;
            info->job->flags |= 0x00400000;
        }
        break;
    case 6:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 22;
            info->job->flags &= ~0x00400000;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 22:
        if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
        {
            info->submode = 27;
            info->job->result = -121;
        }
        break;
    case 7:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 25;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 25:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -3:
            info->submode = 1;
            break;
        case -10:
        case -4:
            info->unk2C++;
            break;
        case 0:
        {
            DVDDiskID *diskId = DVDGetCurrentDiskID();
            int prefixLen = strlen(info->job->desc->name);
            char *ext = strstr(info->job->stat.fileName, ".dat");

            if (strncmp((char *)info->job->stat.gameName, diskId->gameName, 4) == 0
             && strncmp((char *)info->job->stat.company, diskId->company, 2) == 0
             && strncmp(info->job->stat.fileName, info->job->desc->name, prefixLen) == 0
             && ext != NULL)
            {
                struct MemcardListEntry *entry = &((struct MemcardListEntry *)info->job->buffer)[info->unk2D];
                int nameLen = ext - &info->job->stat.fileName[prefixLen];

                strncpy(entry->name, &info->job->stat.fileName[prefixLen], nameLen);
                entry->name[nameLen] = 0;
                entry->size = info->job->stat.length;
                entry->time = info->job->stat.time;
                info->unk2D++;
            }
            info->unk2C++;
            break;
        }
        case -2:
        default:
            info->submode = 0;
            break;
        }
        if (info->unk2C == 127 || info->submode != 0xFF)
        {
            ((struct MemcardListEntry *)info->job->buffer)[info->unk2D].size = 0;
            if (info->submode == 0xFF)
                info->submode = 26;
        }
        break;
    case 26:
        if (info->job->flags & 0x20000000)
        {
            if (memcard_wait_30(info))
                memcard_end_job(info);
        }
        else if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            memcard_end_job(info);
        break;
    }
    if (info->job != NULL)
        g_result_code = info->job->result;
}

void fn_1_ECA60(struct MemcardRequest *req, struct MemcardInfo *info)
{
    info->submode = 3;
}

void fn_1_ECA6C(struct MemcardInfo *info)
{
    if (info->unk4 == 0xFF)
        return;
    switch (info->unk4)
    {
    default:
        OSReport("submode    : %d\n", info->unk4);
        OSReport("submode req: %d\n", info->submode);
        OSPanic("memcard.c", 0xCEF, "submode error!!\n");
        break;
    case 1:
        info->job->flags &= ~0x00400000;
        if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            info->submode = 30;
        else
            handle_error_input(info);
        lbl_1_bss_CF3C0 = 0;
        break;
    case 30:
        if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            info->mode = 0xFE;
        break;
    case 3:
        if (info->mountResult == -127)
        {
            info->job->result = info->mountResult;
            info->submode = 1;
            break;
        }
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
        case -2:
            info->submode = 1;
            break;
        case -13:
            if (locale == 5)
            {
                info->submode = 5;
                break;
            }
            info->job->flags |= 0x00200000;
            info->submode = 1;
            break;
        case -6:
        case 0:
            info->submode = 4;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 4:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case -6:
            info->submode = 5;
            break;
        case -13:
            if (locale == 5)
            {
                info->submode = 5;
                break;
            }
            info->job->flags |= 0x00200000;
            info->submode = 1;
            break;
        case 0:
            info->submode = 7;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 5:
        if (info->job->result == 0)
            break;
        if (memcard_wait_12(info))
        {
            if (info->job->result == 2 || info->job->unk1 == 1)
            {
                info->job->result = -126;
                info->submode = 1;
                break;
            }
            info->submode = 6;
            info->job->flags |= 0x00400000;
        }
        break;
    case 6:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -5:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 7;
            info->job->flags &= ~0x00400000;
            break;
        default:
            info->submode = 0;
            break;
        }
        break;
    case 7:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 28;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 28:
        switch (info->job->result)
        {
        case -1:
            break;
        case -128:
        case -10:
        case -5:
        case -4:
        case -3:
            info->submode = 1;
            break;
        case 0:
            info->submode = 29;
            break;
        case -2:
        default:
            info->submode = 0;
            break;
        }
        break;
    case 29:
        if ((lbl_1_bss_CF3C0 & 0x10) || (lbl_1_bss_CF3C0 & 0x20))
            memcard_end_job(info);
        break;
    }
    if (info->job != NULL)
        g_result_code = info->job->result;
}

static inline void release_memcard_job(struct MemcardInfo *info)
{
    finish_memcard_job(info);
    OSFreeToHeap(currentHeap, info->job);
    reset_memcard_info(info);
    info->flags &= ~4;
}

static inline int is_mount_ok(s32 result)
{
    if (result == 0 || result == -1)
        return 1;
    return 0;
}

void g_something_with_card(void)
{
    s32 sectorSize;
    struct MemcardInfo *info;
    int i;
    u32 j;

    lbl_1_bss_CF3C4++;
    info = memcard_infos;
    for (i = 0; i < 2; i++, info++)
    {
        info->mountResult = (s8)CARDProbeEx(i, NULL, &sectorSize);
        if (info->mountResult == 0 && sectorSize != 0x2000)
            info->mountResult = -127;
        if (info->unk2B != info->mountResult)
        {
            if (info->unk2B == 0)
            {
                info->freeBlocks = -1;
                info->freeFiles = -1;
            }
            info->unk2B = info->mountResult;
        }
    }
    g_something_with_card2(is_mount_ok(memcard_infos[0].mountResult), is_mount_ok(memcard_infos[1].mountResult));
    lbl_1_bss_CF3C0 = 0;
    if (lbl_1_bss_CF3AC == 0xFF)
    {
        lbl_1_bss_CF3C0 = __rlwimi(lbl_1_bss_CF3C0, merged_digital_inputs[2], 0, 28, 31);
        lbl_1_bss_CF3C0 = __rlwimi(lbl_1_bss_CF3C0, merged_digital_inputs[2], 28, 24, 27);
        j = merged_analog_inputs[2] & 0xF;
    }
    else
    {
        lbl_1_bss_CF3C0 = __rlwimi(lbl_1_bss_CF3C0, pad_status_groups[lbl_1_bss_CF3AC].unk0[2].button, 0, 28, 31);
        lbl_1_bss_CF3C0 = __rlwimi(lbl_1_bss_CF3C0, pad_status_groups[lbl_1_bss_CF3AC].unk0[2].button, 28, 24, 27);
        j = analog_inputs[lbl_1_bss_CF3AC][2] & 0xF;
    }
    if (j != 0)
        lbl_1_bss_CF3C0 = __rlwimi(lbl_1_bss_CF3C0, j, 0, 28, 31);
    if (lbl_1_bss_CF3AE != 0xFF)
    {
        if (lbl_1_bss_CF3AE == 2)
        {
            lbl_1_bss_CF3AE = g_last_used_memcard_slot;
            lbl_1_bss_CF3C8 = 0;
            lbl_1_bss_CF368.flags &= ~1;
        }
        g_something_with_card3();
        if (lbl_1_bss_CF3AE != 0xFF)
            return;
    }
    info = memcard_infos;
    for (j = 0; j < 2; j++, info++)
    {
        if (info->mode == 0xFF)
            continue;
        memcard_sequence_funcs[info->mode](info);
        if (info->mode == 0xFE)
        {
            release_memcard_job(info);
            continue;
        }
        if (info->unk7 != 0)
        {
            info->unk7 = 0;
            if (info->workArea != NULL)
            {
                while (CARDUnmount(info->chan) == -1)
                    ;
                OSFreeToHeap(currentHeap, info->workArea);
                info->workArea = NULL;
            }
            info->job->flags |= 0x40000000;
            info->job->flags &= ~0x00200000;
            info->job->result = -123;
            info->submode = 1;
            info->flags &= ~2;
        }
        if (info->job != NULL && info->job->timeout != 0)
        {
            if (info->unk34 == info->progress)
                info->job->timeout--;
            info->unk34 = info->progress;
            if (info->job->timeout == 0)
            {
                info->job->result = -5;
                info->submode = 1;
                info->flags &= ~2;
            }
        }
        if (info->submode != 0xFF)
        {
            info->unk5 = info->unk4;
            info->unk4 = info->submode;
            info->submode = 0xFF;
            g_some_card_funcs[info->unk4](info);
        }
        if (info->unk4 != 0xFF)
            g_some_card_funcs2[info->unk4](info);
        if (info->mode == 0xFE)
            release_memcard_job(info);
    }
}

void g_something_with_card10(char *str)
{
    float x = textdraw_pos_x.x;
    float y = textdraw_pos_x.y;
    float width = textdraw_get_pixel_width_of_string(str);
    float lines = textdraw_get_line_count_of_string(str);

    init_ui_element_sprite_with_defaults();
    switch (textdraw_pos_x.alignment)
    {
    case 0:
        x += 0.5 * width;
        y += 0.5 * lines;
        break;
    case 1:
        x += 0.5 * width;
        break;
    case 2:
        x += 0.5 * width;
        y -= 0.5 * lines;
        break;
    case 3:
        y += 0.5 * lines;
        break;
    case 5:
        y -= 0.5 * lines;
        break;
    case 6:
        x -= 0.5 * width;
        y += 0.5 * lines;
        break;
    case 7:
        x -= 0.5 * width;
        break;
    case 8:
        x -= 0.5 * width;
        y -= 0.5 * lines;
        break;
    }
    set_ui_element_sprite_pos(x, y);
    set_ui_element_sprite_scale((32.0 + width) / 416.0, (32.0 + 24.0 * lines) / 176.0);
    draw_ui_box(5);
    textdraw_print(str);
}

void g_some_printf_function_5(char *format, ...)
{
    va_list args;
    char buffer[512];

    va_start(args, format);
    vsprintf(buffer, format, args);
    g_something_with_card10(buffer);
}

static inline void replace_token(char *dst, char *src, char *token, char *replacement)
{
    char tmp[256];
    char *found;
    int len;

    strcpy(tmp, src);
    found = strstr(tmp, token);
    if (found == NULL)
    {
        strcpy(dst, tmp);
    }
    else
    {
        len = strlen(token);
        if (found != tmp)
        {
            strncpy(dst, tmp, found - tmp);
            dst[found - tmp] = 0;
        }
        else
            dst[0] = 0;
        strcat(dst, replacement);
        strcat(dst, found + len);
    }
}

static inline double clamp_range(float x, double lo, double hi)
{
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}

static inline float blink_value(int period, float fperiod)
{
    float t = (float)(lbl_1_bss_CF3C4 % period) / fperiod;

    return clamp_range(fabs((float)(2.0 * t - 1.0)), 0.0, 1.0);
}

u32 memcard_blink_color(u32 fast)
{
    u32 color;

    if (fast)
    {
        color = 255.0 * blink_value(6, 6.0f);
        color |= (color << 16) | (color << 8);
    }
    else
    {
        color = 255.0 * blink_value(60, 60.0f);
        color |= (color << 16) | (color << 8);
    }
    return color;
}

static inline void memcard_textdraw_setup(void)
{
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_depth(0.001f);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_drop_shadow();
    textdraw_set_flags(0x10000000);
    if (locale != 5)
        textdraw_set_flags(0x80000000);
}

void g_something_with_card11(void)
{
    char buffer[256];
    char *replacement;
    u32 color;

    memcard_textdraw_setup();
    textdraw_set_pos(320.0f, 144.0f);
    if (g_maybe_last_selected_memcard_slot.slot == 0)
        replacement = memcard_messages[32][locale];
    else
        replacement = memcard_messages[33][locale];
    replace_token(buffer, memcard_messages[62][locale], "SLOT", replacement);
    replacement = memcard_messages[g_maybe_last_selected_memcard_slot.unk6][locale];
    replace_token(buffer, buffer, "FILE", replacement);
    g_something_with_card10(buffer);
    if (modeCtrl.courseFlags & 4)
    {
        color = 255.0 * blink_value(6, 6.0f);
        color |= (color << 16) | (color << 8);
    }
    else
    {
        color = 255.0 * blink_value(60, 60.0f);
        color |= (color << 16) | (color << 8);
    }
    textdraw_set_border();
    textdraw_set_pos(288.0f, 234.0f);
    textdraw_set_alignment(7);
    if (modeCtrl.unk10 == 0)
    {
        textdraw_set_mul_color(-0x100);
        textdraw_set_add_color(color);
    }
    else
    {
        textdraw_set_mul_color(0xFFAFAF00);
        textdraw_set_add_color(0);
    }
    g_some_printf_function_5("%s", memcard_messages[34][locale]);
    textdraw_set_pos(352.0f, 234.0f);
    textdraw_set_alignment(1);
    if (modeCtrl.unk10 == 0)
    {
        textdraw_set_mul_color(0xFFAFAF00);
        textdraw_set_add_color(0);
    }
    else
    {
        textdraw_set_mul_color(-0x100);
        textdraw_set_add_color(color);
    }
    g_some_printf_function_5("%s", memcard_messages[35][locale]);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_add_color(0);
    textdraw_set_alignment(4);
    textdraw_set_drop_shadow();
    textdraw_set_pos(320.0f, 334.0f);
    g_something_with_card10(memcard_messages[25][locale]);
    textdraw_reset();
}

void g_something_with_card9(void)
{
    u32 color;

    memcard_textdraw_setup();
    textdraw_set_pos(320.0f, 152.0f);
    g_something_with_card10(memcard_messages[24][locale]);
    color = memcard_blink_color(lbl_1_bss_CF368.flags & 1);
    textdraw_set_border();
    textdraw_set_pos(288.0f, 240.0f);
    textdraw_set_alignment(7);
    if (lbl_1_bss_CF3AE == 0)
    {
        textdraw_set_mul_color(-0x100);
        textdraw_set_add_color(color);
    }
    else
    {
        textdraw_set_mul_color(0xFFAFAF00);
        textdraw_set_add_color(0);
    }
    g_some_printf_function_5("%s", memcard_messages[32][locale]);
    textdraw_set_pos(352.0f, 240.0f);
    textdraw_set_alignment(1);
    if (lbl_1_bss_CF3AE == 0)
    {
        textdraw_set_mul_color(0xFFAFAF00);
        textdraw_set_add_color(0);
    }
    else
    {
        textdraw_set_mul_color(-0x100);
        textdraw_set_add_color(color);
    }
    g_some_printf_function_5("%s", memcard_messages[33][locale]);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_add_color(0);
    textdraw_set_alignment(4);
    textdraw_set_drop_shadow();
    textdraw_set_pos(320.0f, 340.0f);
    g_something_with_card10(memcard_messages[25][locale]);
    textdraw_reset();
}

void g_something_with_card5(void)
{
    char buffer[256];
    struct MemcardInfo *info = memcard_infos[0].unk4 == 5 ? &memcard_infos[0] : &memcard_infos[1];
    char *message;
    char *replacement;
    u32 color;

    memcard_textdraw_setup();
    textdraw_set_pos(320.0f, 198.0f);
    message = memcard_messages[30][locale];
    if (info->flags & 0x10)
        message = memcard_messages[31][locale];
    if (info->chan == 0)
        replacement = memcard_messages[32][locale];
    else
        replacement = memcard_messages[33][locale];
    replace_token(buffer, message, "SLOT", replacement);
    g_something_with_card10(buffer);
    color = memcard_blink_color(info->flags & 1);
    textdraw_set_border();
    textdraw_set_pos(288.0f, 298.0f);
    textdraw_set_alignment(7);
    if (info->job->unk1 == 0)
    {
        textdraw_set_mul_color(-0x100);
        textdraw_set_add_color(color);
    }
    else
    {
        textdraw_set_mul_color(0xFFAFAF00);
        textdraw_set_add_color(0);
    }
    g_some_printf_function_5("%s", memcard_messages[34][locale]);
    textdraw_set_pos(352.0f, 298.0f);
    textdraw_set_alignment(1);
    if (info->job->unk1 == 0)
    {
        textdraw_set_mul_color(0xFFAFAF00);
        textdraw_set_add_color(0);
    }
    else
    {
        textdraw_set_mul_color(-0x100);
        textdraw_set_add_color(color);
    }
    g_some_printf_function_5("%s", memcard_messages[35][locale]);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_add_color(0);
    textdraw_set_alignment(4);
    textdraw_set_drop_shadow();
    textdraw_set_pos(320.0f, 400.0f);
    g_something_with_card10(memcard_messages[25][locale]);
    textdraw_reset();
}

void g_something_with_card4(void)
{
    char buffer[256];
    char *replacement;
    struct MemcardInfo *info = memcard_infos[0].unk4 == 11 ? &memcard_infos[0] : &memcard_infos[1];
    u32 color;

    memcard_textdraw_setup();
    textdraw_set_pos(320.0f, 192.0f);
    if (info->chan == 0)
        replacement = memcard_messages[32][locale];
    else
        replacement = memcard_messages[33][locale];
    replace_token(buffer, memcard_messages[41][locale], "SLOT", replacement);
    replacement = memcard_messages[info->job->desc->language][locale];
    replace_token(buffer, buffer, "FILE", replacement);
    g_something_with_card10(buffer);
    color = memcard_blink_color(info->flags & 1);
    textdraw_set_border();
    textdraw_set_pos(288.0f, 280.0f);
    textdraw_set_alignment(7);
    if (info->job->unk1 == 0)
    {
        textdraw_set_mul_color(-0x100);
        textdraw_set_add_color(color);
    }
    else
    {
        textdraw_set_mul_color(0xFFAFAF00);
        textdraw_set_add_color(0);
    }
    g_some_printf_function_5("%s", memcard_messages[34][locale]);
    textdraw_set_pos(352.0f, 282.0f);
    textdraw_set_alignment(1);
    if (info->job->unk1 == 0)
    {
        textdraw_set_mul_color(0xFFAFAF00);
        textdraw_set_add_color(0);
    }
    else
    {
        textdraw_set_mul_color(-0x100);
        textdraw_set_add_color(color);
    }
    g_some_printf_function_5("%s", memcard_messages[35][locale]);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_add_color(0);
    textdraw_set_alignment(4);
    textdraw_set_drop_shadow();
    textdraw_set_pos(320.0f, 382.0f);
    g_something_with_card10(memcard_messages[25][locale]);
    textdraw_reset();
}

static inline int clamp_frame(int x)
{
    if (x < 0)
        return 0;
    if (x > 4u)
        return 4;
    return x;
}

void g_something_with_card8(void)
{
    static s16 sprites[] = {0x000F, 0x45, 0x46, 0x47, 0x48};
    struct MemcardInfo *info = (memcard_infos[0].flags & 2) ? &memcard_infos[0] : &memcard_infos[1];
    int message;
    int frame;
    float progress;
    float x;
    float y;

    switch (info->unk4)
    {
    case 6:
        message = 57;
        break;
    case 15:
        message = 58;
        break;
    case 25:
        message = 59;
        break;
    default:
        message = 58;
        break;
    }
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_depth(1e-7f);
    textdraw_set_drop_shadow();
    if (locale != 5)
        textdraw_set_flags(0x80000000);
    textdraw_set_pos(320.0f, 240.0f);
    g_something_with_card10(memcard_messages[message][locale]);
    x = 320.0f;
    y = 392.0f;
    progress = info->progress;
    if (info->mode == 0)
    {
        progress = 1.5 * progress;
        progress = progress < 0.0 ? 0.0 : (progress > 1.0 ? 1.0 : progress);
    }
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(3);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_drop_shadow();
    textdraw_set_scale(22.916666f, 1.0f);
    textdraw_set_pos(x, y);
    textdraw_print("_");
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(250.0 + x, y);
    set_ui_element_sprite_scale(0.5f, 0.5f);
    draw_ui_element(0x59);
    init_ui_element_sprite_with_defaults();
    set_ui_element_sprite_pos(x - 250.0 + 500.0 * progress, y);
    set_ui_element_sprite_scale(0.5f, 0.328125f);
    set_ui_element_sprite_rot_z(4096.0f * math_sin(lbl_1_bss_CF3C4 * 0x640 + 0x4000));
    frame = 5.0f * progress;
    draw_ui_element(sprites[clamp_frame(frame)]);
    textdraw_reset();
}

static inline void memcard_textdraw_setup2(void)
{
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0xFFEF9A00);
    textdraw_set_depth(1e-7f);
    textdraw_set_drop_shadow();
    textdraw_set_flags(0x10000000);
    if (locale != 5)
        textdraw_set_flags(0x80000000);
}

void g_something_with_card6(void)
{
    static struct SpriteDrawRequest fade = {
        0x4B, 320.0f, 240.0f, 1.0f, 80.0f, 60.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.65f, -1, 10, 0xFF000000,
    };
    u32 status;
    int show;
    char *replacement;
    u32 chans;
    u32 flags;
    int notSafe;
    int file;
    int message[2];
    int detail[2];
    int fileMsg[2];

    if (g_check_some_memcard_field() != 0 || g_maybe_last_selected_memcard_slot.unk2 != -1)
    {
        if (++lbl_1_bss_CF3CC > 8)
            lbl_1_bss_CF3CC = 8;
    }
    else
    {
        if (--lbl_1_bss_CF3CC < 0)
            lbl_1_bss_CF3CC = 0;
    }
    if (lbl_1_bss_CF3CC > 0)
    {
        struct SpriteDrawRequest *req = &fade;

        req->opacity = 0.8 * (float)lbl_1_bss_CF3CC / 8.0;
        draw_sprite_draw_request(req);
    }
    if (lbl_1_bss_CF3AE != 0xFF && lbl_1_bss_CF3AE != 2)
    {
        set_ui_widescreen_scale_mtx(0x140);
        g_something_with_card9();
        reset_ui_widescreen_scale_mtx();
        return;
    }
    show = 0;
    status = g_check_some_memcard_field();
    if (status & 1)
    {
        if (memcard_infos[0].mode == 1)
        {
            if (!(memcard_infos[0].flags & 8) && !(memcard_infos[0].flags & 4))
                show |= 1;
        }
        else
        {
            if (!(memcard_infos[0].flags & 4))
                show |= 1;
        }
    }
    if (status & 2)
    {
        if (memcard_infos[1].mode == 1)
        {
            if (!(memcard_infos[1].flags & 8) && !(memcard_infos[1].flags & 4))
                show |= 1;
        }
        else
        {
            if (!(memcard_infos[1].flags & 4))
                show |= 1;
        }
    }
    if (show != 0)
    {
        char buffer[256];
        int msg;
        u32 active;

        set_ui_widescreen_scale_mtx(0x140);
        memcard_textdraw_setup2();
        notSafe = 0;
        file = 67;
        active = g_check_some_memcard_field();
        if (active & 1)
        {
            if (memcard_infos[0].mode == 1)
            {
                switch (memcard_infos[0].unk4)
                {
                case 5:
                case 6:
                case 22:
                    break;
                default:
                    notSafe = 1;
                    break;
                }
            }
            if (memcard_infos[0].job != NULL)
                file = memcard_infos[0].job->desc->language;
        }
        else if (active & 2)
        {
            if (memcard_infos[1].mode == 1)
            {
                switch (memcard_infos[0].unk4)
                {
                case 5:
                case 6:
                case 22:
                    break;
                default:
                    notSafe = 1;
                    break;
                }
            }
            if (memcard_infos[1].job != NULL)
                file = memcard_infos[1].job->desc->language;
        }
        msg = 61;
        if (notSafe != 0)
            msg = 60;
        replacement = memcard_messages[file][locale];
        replace_token(buffer, memcard_messages[msg][locale], "FILE", replacement);
        textdraw_set_pos(320.0f, 80.0f);
        g_something_with_card10(buffer);
        reset_ui_widescreen_scale_mtx();
    }
    if (memcard_infos[0].unk4 == 5 || memcard_infos[1].unk4 == 5)
    {
        set_ui_widescreen_scale_mtx(0x140);
        g_something_with_card5();
        reset_ui_widescreen_scale_mtx();
        return;
    }
    if (memcard_infos[0].unk4 == 11 || memcard_infos[1].unk4 == 11)
    {
        set_ui_widescreen_scale_mtx(0x140);
        g_something_with_card4();
        reset_ui_widescreen_scale_mtx();
        return;
    }
    if (g_maybe_last_selected_memcard_slot.unk2 == 62)
    {
        set_ui_widescreen_scale_mtx(0x140);
        g_something_with_card11();
        reset_ui_widescreen_scale_mtx();
        return;
    }
    if ((memcard_infos[0].flags | memcard_infos[1].flags) & 2)
    {
        set_ui_widescreen_scale_mtx(0x140);
        g_something_with_card8();
        reset_ui_widescreen_scale_mtx();
        return;
    }
    chans = 0;
    flags = 0;
    message[0] = -1;
    message[1] = -1;
    if (memcard_infos[0].message != -1)
    {
        chans |= 1;
        message[0] = memcard_infos[0].message;
        detail[0] = memcard_infos[0].unkA;
        fileMsg[0] = memcard_infos[0].job->desc->language;
        flags |= memcard_infos[0].job->flags;
    }
    if (memcard_infos[1].message != -1)
    {
        chans |= 2;
        message[1] = memcard_infos[1].message;
        detail[1] = memcard_infos[1].unkA;
        fileMsg[1] = memcard_infos[1].job->desc->language;
        flags |= memcard_infos[1].job->flags;
    }
    if (g_maybe_last_selected_memcard_slot.unk2 != -1 && message[g_maybe_last_selected_memcard_slot.slot] < 0)
    {
        chans |= g_maybe_last_selected_memcard_slot.slot + 1;
        message[g_maybe_last_selected_memcard_slot.slot] = g_maybe_last_selected_memcard_slot.unk2;
        detail[g_maybe_last_selected_memcard_slot.slot] = g_maybe_last_selected_memcard_slot.unk4;
        fileMsg[g_maybe_last_selected_memcard_slot.slot] = g_maybe_last_selected_memcard_slot.unk6;
    }
    if (chans == 0)
        return;
    set_ui_widescreen_scale_mtx(0x140);
    memcard_textdraw_setup2();
    if (chans < 3)
    {
        char buffer[256];

        if (chans - 1 == 0)
            replacement = memcard_messages[32][locale];
        else
            replacement = memcard_messages[33][locale];
        replace_token(buffer, memcard_messages[message[chans - 1]][locale], "SLOT", replacement);
        replacement = memcard_messages[fileMsg[chans - 1]][locale];
        replace_token(buffer, buffer, "FILE", replacement);
        if (memcard_infos[chans - 1].job != NULL)
        {
            char num[16];

            sprintf(num, "%d", memcard_infos[chans - 1].job->freeBytes / 0x2000);
            replace_token(buffer, buffer, "FREE", num);
        }
        if (memcard_infos[chans - 1].job != NULL)
        {
            char num[16];

            sprintf(num, "%d", memcard_infos[chans - 1].job->size / 0x2000);
            replace_token(buffer, buffer, "REQUIRED", num);
        }
        if (flags & 0x20000000)
        {
            textdraw_set_pos(320.0f, 240.0f);
            g_something_with_card10(buffer);
        }
        else
        {
            int lines = textdraw_get_line_count_of_string(buffer);

            if (lines > 6)
                textdraw_set_pos(320.0f, 190.0 - 12.0 * (lines - 6));
            else
                textdraw_set_pos(320.0f, 190.0f);
            g_something_with_card10(buffer);
            textdraw_set_pos(320.0f, 340.0f);
            g_something_with_card10(memcard_messages[detail[chans - 1]][locale]);
        }
    }
    else
    {
        textdraw_set_pos(320.0f, 120.0f);
        g_something_with_card10(memcard_messages[message[0]][locale]);
        if (!(memcard_infos[chans - 1].job->flags & 0x20000000))
        {
            textdraw_set_pos(320.0f, 200.0f);
            g_something_with_card10(memcard_messages[detail[0]][locale]);
        }
        textdraw_set_pos(320.0f, 280.0f);
        g_something_with_card10(memcard_messages[message[1]][locale]);
        if (!(memcard_infos[chans - 1].job->flags & 0x20000000))
        {
            textdraw_set_pos(320.0f, 360.0f);
            g_something_with_card10(memcard_messages[detail[1]][locale]);
        }
    }
    reset_ui_widescreen_scale_mtx();
    textdraw_reset();
}

u8 g_check_some_memcard_field(void)
{
    u8 ret = 0;

    if (memcard_infos[0].mode != 0xFF)
        ret |= 1;
    if (memcard_infos[1].mode != 0xFF)
        ret |= 2;
    if (lbl_1_bss_CF368.mode != 0xFF)
        ret |= 4;
    return ret;
}

s32 g_get_result_code(void)
{
    return g_result_code;
}

int fn_1_EFE64(u8 chan)
{
    switch (chan)
    {
    case 0:
    case 1:
        return memcard_infos[chan].mountResult;
    case 2:
        chan = 2;
        break;
    case 3:
        break;
    }
    return memcard_infos[g_last_used_memcard_slot].mountResult;
}

u8 g_get_some_memcard_var(void)
{
    return g_some_memcard_var;
}

static inline const struct MemcardFileDesc *get_file_desc(int file)
{
    return &memcard_file_descs[file];
}

void g_sprintf_memcard_error(int file, char *dest)
{
    sprintf(dest, "%s %s", memcard_messages[get_file_desc(file)->language][locale], memcard_messages[66][locale]);
}

void fn_1_EFF6C(int slot, int a, int b, int c)
{
    g_maybe_last_selected_memcard_slot.slot = (s8)slot;
    g_maybe_last_selected_memcard_slot.unk2 = (s16)a;
    g_maybe_last_selected_memcard_slot.unk4 = (s16)b;
    g_maybe_last_selected_memcard_slot.unk6 = (s16)c;
}

void fn_1_EFFB0(void)
{
    g_maybe_last_selected_memcard_slot.unk2 = -1;
}

u8 g_get_last_used_memcard_slot(void)
{
    return g_last_used_memcard_slot;
}

void fn_1_EFFD4(int file)
{
    char path[0x80];

    if (get_file_desc(file)->size != 0)
    {
        strcpy(path, "./memcard/");
        strcat(path, memcard_file_descs[file].file);
        queue_disc_read(path);
    }
}

int fn_1_F004C(void)
{
    int ret = 0;

    if (memcard_infos[0].job != NULL && (memcard_infos[0].job->flags & 0x400000))
        ret = 1;
    if (memcard_infos[1].job != NULL && (memcard_infos[1].job->flags & 0x400000))
        ret = 1;
    return ret;
}

int fn_1_F00BC(int chan)
{
    return memcard_infos[chan].mountResult != (s8)-3;
}
