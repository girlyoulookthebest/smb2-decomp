/* Per-language stage name lists: /stgname/<lang>.str holds one name per line. */
#include "types.h"
#include "file.h"

#define STAGE_NAME_COUNT 0x1A5

/* Layout of the buffer filled by g_read_storymode_select_stage_names_from_dvd: one string pointer
 * per stage, then the raw file contents.  The pointer table grows on 64-bit hosts. */
#define STAGE_NAME_HEADER_SIZE (0x6A0 * PC_PTR_SCALE)

char *LOCALE_STRINGS[6] = { "usa", "ger", "fra", "spa", "ita", "jpn" };

s32 LOCALE_STGNAME_DVD_ENTRYNUMS[6];
struct
{
    /*0x000*/ s32 entrynum;
    /*0x004*/ u32 fileSize;
    /*0x008*/ u32 offsets[STAGE_NAME_COUNT + 1];
    /*0x6A0*/ u8 filler6A0[0x18];
} g_current_stagename_dvd_entry_number;

extern void *currentHeap;
extern void *g_some_buffer_ptr;

int sprintf(char *str, const char *fmt, ...);
char *strncpy(char *dst, const char *src, u32 n);
s32 DVDConvertPathToEntrynum(char *path);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void OSFreeToHeap(void *heap, void *ptr);
void g_something_with_dvd(s32 entrynum, struct File *file);
BOOL disc_queue_load(s32 entrynum);

BOOL g_load_stgname_dvd_entrynum(s32 entrynum);

void g_load_stgname_file(int locale)
{
    char path[0x108];
    int i;
    char **lang;
    s32 *entry;

    lang = LOCALE_STRINGS;
    entry = LOCALE_STGNAME_DVD_ENTRYNUMS;
    for (i = 6; i > 0; i--, lang++, entry++)
    {
        sprintf(path, "/stgname/%s.str", *lang);
        *entry = DVDConvertPathToEntrynum(path);
    }
    g_load_stgname_dvd_entrynum(LOCALE_STGNAME_DVD_ENTRYNUMS[locale]);
}

BOOL g_load_stgname_dvd_entrynum(s32 entrynum)
{
    struct File file;
    u32 size;
    char *buf;
    char *p;
    char *end;
    int i;
    u32 *offset;

    if (entrynum < 0)
        return FALSE;
    g_current_stagename_dvd_entry_number.entrynum = entrynum;
    g_something_with_dvd(entrynum, &file);
    size = (file_size(&file) + 0x1F) & ~0x1F;
    g_current_stagename_dvd_entry_number.fileSize = size;
    buf = alloc_from_heap_or_panic(currentHeap, size, "stage_name.c", 125);
    file_read(&file, buf, size, 0);
    p = buf;
    end = buf + size;
    offset = g_current_stagename_dvd_entry_number.offsets;
    for (i = 0; i < STAGE_NAME_COUNT + 1; i++, offset++)
    {
        *offset = p - buf;
        while ((u8)*p != 0 && (u8)*p != '\n' && p < end)
            p++;
        if (p < end)
            p++;
    }
    OSFreeToHeap(currentHeap, buf);
    file_close(&file);
    return TRUE;
}

int g_get_scen_stgname_buffer_size(void)
{
    int size = 0;

    size += STAGE_NAME_HEADER_SIZE;
    size += g_current_stagename_dvd_entry_number.fileSize;
    return size;
}

BOOL g_queue_stage_name_load(void)
{
    return disc_queue_load(g_current_stagename_dvd_entry_number.entrynum);
}

BOOL g_read_storymode_select_stage_names_from_dvd(char **names)
{
    struct File file;
    char *strs;
    char **name;
    int i;
    u32 *offset;

    strs = (char *)names + STAGE_NAME_HEADER_SIZE;
    g_something_with_dvd(g_current_stagename_dvd_entry_number.entrynum, &file);
    file_read(&file, strs, g_current_stagename_dvd_entry_number.fileSize, 0);
    name = names;
    offset = g_current_stagename_dvd_entry_number.offsets;
    for (i = 0; i < STAGE_NAME_COUNT; i++, name++, offset++)
        *name = strs + *offset;
    offset = g_current_stagename_dvd_entry_number.offsets;
    for (i = 0; i < STAGE_NAME_COUNT; i++, offset++)
        offset[1][(char *)strs - 1] = 0;
    file_close(&file);
    return TRUE;
}

char *read_stage_name_from_dvd(int stageId, char *out, u32 outSize)
{
    struct File file;
    void *buf;
    u32 offset;
    u32 len;
    u32 aligned;
    u32 readSize;

    buf = g_some_buffer_ptr;
    g_something_with_dvd(g_current_stagename_dvd_entry_number.entrynum, &file);
    offset = g_current_stagename_dvd_entry_number.offsets[stageId];
    len = g_current_stagename_dvd_entry_number.offsets[stageId + 1] - offset - 1;
    len = (outSize - 1 < len) ? outSize - 1 : len;
    if (len == 0)
    {
        *out = 0;
        return out;
    }
    aligned = offset & ~0x1F;
    readSize = ((offset + len + 0x1F) & ~0x1F) - aligned;
    file_read(&file, buf, readSize, aligned);
    strncpy(out, (char *)buf + (offset - aligned), len);
    out[len] = 0;
    file_close(&file);
    return out;
}

/* Number of leading characters two strings share, plus one if they don't end there. */
int fn_1_5CF50(u8 *a, u8 *b)
{
    int n = 0;

    while (*a == *b)
    {
        n++;
        if (*a == 0 || *b == 0)
            break;
        a++;
        b++;
    }
    return n;
}
