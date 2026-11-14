/* Original filename is also load.c (see the panic strings), which the unrelated
 * asset loader already uses here, hence disc_load.c.
 *
 * Background disc queue: reads files into ARAM in 0xF000 byte chunks and
 * keeps them in a file cache that file_open and friends look in first. */
#pragma peephole off
#include "types.h"
#include "file.h"
#include "os.h"

#define DISC_QUEUE_SIZE 128
#define CHUNK_SIZE 0xF000

enum
{
    QUEUE_STATE_OPEN,
    QUEUE_STATE_FIRST_READ,
    QUEUE_STATE_COPY,
    QUEUE_STATE_READ,
    QUEUE_STATE_CLOSE,
    QUEUE_STATE_CANCELLED,
};

struct DiscQueueEntry
{
    /*0x00*/ s32 state;
    /*0x04*/ s32 entryNum;
    /*0x08*/ s32 group;
}; /* size = 0xC */

typedef u8 ARQRequest[0x20];

struct DiscLoad
{
    /*0x00*/ u32 flags;
    /*0x04*/ struct DiscFileInfo fileInfo;
    /*0x40*/ struct FileCacheEntry *cacheEntry;
    /*0x44*/ u32 chunkIndex;
    /*0x48*/ u32 chunkSize;
    /*0x4C*/ u32 remaining;
    /*0x50*/ u32 totalSize;
    /*0x54*/ s32 group;
}; /* size = 0x58 */

void *memset(void *dst, int c, u32 n);
s32 DVDFastOpen(s32 entryNum, struct DiscFileInfo *fileInfo);
s32 DVDClose(struct DiscFileInfo *fileInfo);
s32 DVDConvertPathToEntrynum(char *path);
s32 DVDReadAsyncPrio(struct DiscFileInfo *fileInfo, void *addr, s32 length, s32 offset,
                     void (*callback)(s32, struct DiscFileInfo *), s32 prio);
s32 read_entire_file_using_dvdread_prio_async(struct DiscFileInfo *fileInfo, void *dest, u32 size, u32 offset);
void DCInvalidateRange(void *addr, u32 size);
void ARQPostRequest(void *request, u32 owner, u32 type, u32 priority, uptr source,
                    uptr dest, u32 length, void (*callback)(u32));
void perf_init_timer(int timer);

void g_some_arq_callback(u32 request);
void ARQPostRequest_on_finish(u32 request);
void fn_1_D33BC(s32 result, struct DiscFileInfo *fileInfo);
struct FileCacheEntry *g_some_dvd_callback(u32 size);
void g_something_with_dvd2(u32 addr, u32 size);
int add_one_wrap_if_over127(int index);
void disc_queue_status(int group);

extern void *lbl_1_bss_1320;

u32 lbl_1_data_52D20 = 0x800000;

s32 lbl_1_bss_C8B48;
s32 g_is_waiting_on_some_arq_request;
struct DiscQueueEntry lbl_1_bss_C8B50[DISC_QUEUE_SIZE];
s32 disc_queue_start;
s32 disc_queue_end;
s32 lbl_1_bss_C9158;
struct DiscLoad lbl_1_bss_C915C;
ARQRequest lbl_1_bss_C91B4;
struct FileCacheEntry file_cache_entries[DISC_QUEUE_SIZE];
char *g_last_filename_attempted_to_open;
s32 g_aram_read_pending;
s32 lbl_1_bss_C99DC;

/* Forces .bss order. */
static s32 unused_bss(void)
{
    s32 x = lbl_1_bss_C8B48;

    x += g_is_waiting_on_some_arq_request;
    x += lbl_1_bss_C8B50[0].state;
    x += disc_queue_start;
    x += disc_queue_end;
    x += lbl_1_bss_C9158;
    x += lbl_1_bss_C915C.flags;
    x += lbl_1_bss_C91B4[0];
    return x;
}

void g_some_arq_callback(u32 request)
{
    g_is_waiting_on_some_arq_request = 0;
}

void load_disc_queue(void)
{
    struct DiscQueueEntry *entry;
    int index;
    uptr addr;

    index = disc_queue_start;
    entry = &lbl_1_bss_C8B50[index];
    while (index != disc_queue_end && entry->state == QUEUE_STATE_CANCELLED)
    {
        index = add_one_wrap_if_over127(index);
        entry = &lbl_1_bss_C8B50[index];
    }
    disc_queue_start = index;
    if (disc_queue_start == disc_queue_end)
        return;

    entry = &lbl_1_bss_C8B50[disc_queue_start];
    switch (entry->state)
    {
    case QUEUE_STATE_OPEN:
        if (lbl_1_bss_C9158 > 0)
            break;
        memset(&lbl_1_bss_C915C, 0, sizeof(lbl_1_bss_C915C));
        if (DVDFastOpen(entry->entryNum, &lbl_1_bss_C915C.fileInfo) != 1)
            OSPanic("load.c", 0x91, "cannot open file\n");
        lbl_1_bss_C915C.flags |= 1;
        lbl_1_bss_C915C.totalSize = OSRoundUp32B(lbl_1_bss_C915C.fileInfo.length);
        lbl_1_bss_C915C.chunkIndex = 0;
        lbl_1_bss_C915C.remaining = lbl_1_bss_C915C.totalSize;
        lbl_1_bss_C915C.chunkSize =
            (lbl_1_bss_C915C.remaining < CHUNK_SIZE) ? lbl_1_bss_C915C.remaining : CHUNK_SIZE;
        lbl_1_bss_C915C.cacheEntry = g_some_dvd_callback(lbl_1_bss_C915C.totalSize);
        lbl_1_bss_C915C.cacheEntry->entryNum = entry->entryNum;
        lbl_1_bss_C9158 = 1;
        if (DVDReadAsyncPrio(&lbl_1_bss_C915C.fileInfo, lbl_1_bss_1320, lbl_1_bss_C915C.chunkSize,
                             lbl_1_bss_C915C.chunkIndex * CHUNK_SIZE, fn_1_D33BC, 2) != 1)
            OSPanic("load.c", 0xA1, "cannot read file\n");
        entry->state = QUEUE_STATE_FIRST_READ;
        break;
    case QUEUE_STATE_FIRST_READ:
        if (lbl_1_bss_C9158 < 0)
        {
            disc_queue_start = add_one_wrap_if_over127(disc_queue_start);
            lbl_1_bss_C9158 = 0;
            break;
        }
        if (lbl_1_bss_C9158 != 0)
            break;
    case QUEUE_STATE_COPY:
    copy:
        addr = (uptr)lbl_1_bss_1320 + (lbl_1_bss_C915C.chunkIndex & 1) * CHUNK_SIZE;
        {
            u32 dest = lbl_1_bss_C915C.cacheEntry->aramAddr + lbl_1_bss_C915C.chunkIndex * CHUNK_SIZE;

            g_is_waiting_on_some_arq_request = 1;
            ARQPostRequest(lbl_1_bss_C91B4, 0, 0, 1, addr, dest, lbl_1_bss_C915C.chunkSize,
                           g_some_arq_callback);
        }
        lbl_1_bss_C915C.chunkIndex++;
        lbl_1_bss_C915C.remaining -= lbl_1_bss_C915C.chunkSize;
        if (lbl_1_bss_C915C.remaining == 0)
        {
            entry->state = QUEUE_STATE_CLOSE;
            break;
        }
        lbl_1_bss_C915C.chunkSize =
            (lbl_1_bss_C915C.remaining < CHUNK_SIZE) ? lbl_1_bss_C915C.remaining : CHUNK_SIZE;
        addr = (uptr)lbl_1_bss_1320 + (lbl_1_bss_C915C.chunkIndex & 1) * CHUNK_SIZE;
        {
            u32 offset = lbl_1_bss_C915C.chunkIndex * CHUNK_SIZE;

            lbl_1_bss_C9158 = 1;
            if (DVDReadAsyncPrio(&lbl_1_bss_C915C.fileInfo, (void *)addr, lbl_1_bss_C915C.chunkSize,
                                 offset, fn_1_D33BC, 2) != 1)
                OSPanic("load.c", 0xDD, "cannot read file\n");
        }
        entry->state = QUEUE_STATE_READ;
    case QUEUE_STATE_READ:
        if (g_is_waiting_on_some_arq_request != 0)
            break;
        if (lbl_1_bss_C9158 < 0)
        {
            disc_queue_start = add_one_wrap_if_over127(disc_queue_start);
            lbl_1_bss_C9158 = 0;
            break;
        }
        if (lbl_1_bss_C9158 != 0)
            break;
        goto copy;
    case QUEUE_STATE_CLOSE:
        if (g_is_waiting_on_some_arq_request != 0)
            break;
        lbl_1_bss_C915C.cacheEntry->valid = 1;
        DVDClose(&lbl_1_bss_C915C.fileInfo);
        lbl_1_bss_C915C.flags &= ~1;
        disc_queue_start = add_one_wrap_if_over127(disc_queue_start);
        break;
    }
}

BOOL g_something_with_dvd(s32 entryNum, struct File *file)
{
    struct FileCacheEntry *entry;
    int i;

    if (entryNum < 0)
        return FALSE;
    entry = file_cache_entries;
    for (i = 0; i < DISC_QUEUE_SIZE; i++, entry++)
    {
        if (entry->valid == 1 && entry->entryNum == entryNum)
        {
            file->isCached = 1;
            file->cache = *entry;
            return TRUE;
        }
    }
    file->isCached = 0;
    return DVDFastOpen(entryNum, &file->fileInfo);
}

BOOL file_open(char *filename, struct File *file)
{
    s32 entryNum;
    BOOL ret;

    g_last_filename_attempted_to_open = filename;
    entryNum = DVDConvertPathToEntrynum(filename);
    ret = g_something_with_dvd(entryNum, file);
    g_last_filename_attempted_to_open = NULL;
    return ret;
}

BOOL file_close(struct File *file)
{
    switch (file->isCached)
    {
    case 1:
        return TRUE;
    case 0:
    default:
        return DVDClose(&file->fileInfo);
    }
}

void ARQPostRequest_on_finish(u32 request)
{
    g_aram_read_pending = 0;
}

s32 file_read(struct File *file, void *dest, u32 size, u32 offset)
{
    ARQRequest request;

    switch (file->isCached)
    {
    case 1:
        g_aram_read_pending = 1;
        DCInvalidateRange(dest, size);
        ARQPostRequest(request, 0, 1, 1, file->cache.aramAddr + offset, (uptr)dest, size,
                       ARQPostRequest_on_finish);
        while (g_aram_read_pending != 0)
            ;
        return size;
    default:
        return read_entire_file_using_dvdread_prio_async(&file->fileInfo, dest, size, offset);
    }
}

u32 file_size(struct File *file)
{
    switch (file->isCached)
    {
    case 1:
        return file->cache.size;
    default:
        return file->fileInfo.length;
    }
}

void fn_1_D33BC(s32 result, struct DiscFileInfo *fileInfo)
{
    lbl_1_bss_C9158 = (result < 0) ? result : 0;
}

struct FileCacheEntry *g_some_dvd_callback(u32 size)
{
    struct FileCacheEntry *entry;
    u32 addr;

    size = OSRoundUp32B(size);
    if (0x1000000 - lbl_1_data_52D20 > size)
    {
        addr = lbl_1_data_52D20;
        lbl_1_data_52D20 += size;
    }
    else
    {
        addr = 0x800000;
        lbl_1_data_52D20 = addr + size;
    }
    g_something_with_dvd2(addr, size);
    entry = &file_cache_entries[lbl_1_bss_C99DC];
    if (++lbl_1_bss_C99DC >= DISC_QUEUE_SIZE)
        lbl_1_bss_C99DC = 0;
    entry->valid = 0;
    entry->aramAddr = addr;
    entry->size = size;
    return entry;
}

void g_something_with_dvd2(u32 addr, u32 size)
{
    struct FileCacheEntry *entry = file_cache_entries;
    int i;

    for (i = 0; i < DISC_QUEUE_SIZE; i++, entry++)
    {
        if (entry->valid != 0 && entry->aramAddr < addr + size && entry->aramAddr + entry->size > addr)
            entry->valid = 0;
    }
}

int add_one_wrap_if_over127(int index)
{
    index++;
    if (index >= DISC_QUEUE_SIZE)
        index = 0;
    return index;
}

static int sub_one_wrap(int index)
{
    index--;
    if (index < 0)
        index = DISC_QUEUE_SIZE - 1;
    return index;
}

static BOOL queue_entry(s32 entryNum)
{
    int next;
    struct DiscQueueEntry *entry;

    if (disc_queue_end == disc_queue_start)
        perf_init_timer(2);
    next = disc_queue_end;
    next++;
    if (next >= DISC_QUEUE_SIZE)
        next = 0;
    if (disc_queue_start == next)
        return FALSE;
    entry = &lbl_1_bss_C8B50[disc_queue_end];
    entry->state = QUEUE_STATE_OPEN;
    entry->entryNum = entryNum;
    entry->group = lbl_1_bss_C915C.group;
    disc_queue_end = next;
    return TRUE;
}

BOOL disc_queue_load(s32 entryNum)
{
    BOOL r;

    if (entryNum < 0)
        return FALSE;
    r = queue_entry(entryNum);
    return r;
}

BOOL queue_disc_read(char *filename)
{
    s32 entryNum = DVDConvertPathToEntrynum(filename);
    struct DiscQueueEntry *entry;
    int next;

    if (entryNum < 0)
        return FALSE;
    if (disc_queue_end == disc_queue_start)
        perf_init_timer(2);
    next = disc_queue_end;
    next++;
    if (next >= DISC_QUEUE_SIZE)
        next = 0;
    if (disc_queue_start == next)
        return FALSE;
    entry = &lbl_1_bss_C8B50[disc_queue_end];
    entry->state = QUEUE_STATE_OPEN;
    entry->entryNum = entryNum;
    entry->group = lbl_1_bss_C915C.group;
    disc_queue_end = next;
    return TRUE;
}

BOOL fn_1_D36C8(void)
{
    return disc_queue_end != disc_queue_start;
}

void get_load_group_status(void)
{
    disc_queue_status(lbl_1_bss_C915C.group);
}

void disc_queue_status(int group)
{
    int i;

    for (i = disc_queue_start; i != disc_queue_end; i = add_one_wrap_if_over127(i))
    {
        if (group == lbl_1_bss_C8B50[i].group)
            lbl_1_bss_C8B50[i].state = QUEUE_STATE_CANCELLED;
    }
    if (disc_queue_start != disc_queue_end)
    {
        for (i = sub_one_wrap(disc_queue_end); i != disc_queue_start; i = sub_one_wrap(i))
        {
            if (lbl_1_bss_C8B50[i].state != QUEUE_STATE_CANCELLED)
                return;
            disc_queue_end = i;
        }
    }
}

void fn_1_D37E8(int group)
{
    int i;

    for (i = disc_queue_start; i != disc_queue_end; i = add_one_wrap_if_over127(i))
    {
        if (group != lbl_1_bss_C8B50[i].group)
            lbl_1_bss_C8B50[i].state = QUEUE_STATE_CANCELLED;
    }
}

int disc_queue_distance(int group)
{
    int count = 0;
    int i;

    for (i = disc_queue_start; i != disc_queue_end; i = add_one_wrap_if_over127(i))
    {
        if (lbl_1_bss_C8B50[i].state != QUEUE_STATE_CANCELLED && group == lbl_1_bss_C8B50[i].group)
            count++;
    }
    return count;
}

int g_get_debug_level_select_loading_left_asterisks(void)
{
    if (disc_queue_end < disc_queue_start)
        return disc_queue_end + DISC_QUEUE_SIZE - disc_queue_start;
    return disc_queue_end - disc_queue_start;
}

void fn_1_D391C(void)
{
    lbl_1_data_52D20 = 0x800000;
}

void fn_1_D3930(void)
{
    struct FileCacheEntry *entry;
    int i;

    disc_queue_status(lbl_1_bss_C915C.group);
    entry = file_cache_entries;
    for (i = 0; i < DISC_QUEUE_SIZE; i++, entry++)
        entry->valid = 0;
    lbl_1_data_52D20 = 0x800000;
}

int g_swapDiscQueueGroup(int group)
{
    int old = lbl_1_bss_C915C.group;

    lbl_1_bss_C915C.group = group;
    return old;
}
