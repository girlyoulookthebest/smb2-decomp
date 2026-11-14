#include "types.h"
#include "file.h"
#include "os.h"

struct RelModuleHeader
{
    u8 filler0[0x20];
    u32 bssSize;
    u8 filler24[0x34 - 0x24];
    void (*prolog)(void);
    void (*epilog)(void);
}; /* partial OSModuleHeader */

struct RelInfo
{
    struct RelModuleHeader *info;
    void *bss;
};

extern int currentHeap;

void OSFreeToHeap(int heap, void *ptr);
void OSLink(struct RelModuleHeader *module, void *bss);
void OSUnlink(struct RelModuleHeader *module);
void *alloc_from_heap_or_panic(int heap, u32 size, char *file, int line);
int DVDOpen(char *path, struct DiscFileInfo *file);
int DVDClose(struct DiscFileInfo *file);
int read_entire_file_using_dvdread_prio_async(struct DiscFileInfo *file, void *buffer, u32 size, u32 offset);

int g_loaded_rel_count;
struct RelInfo *g_loaded_rel_infos[11];

static inline void *rel_alloc(u32 size, int line)
{
    return alloc_from_heap_or_panic(currentHeap, size, "relocation.c", line);
}

void load_additional_rel(char *name, struct RelInfo *rel)
{
    struct DiscFileInfo file;
    u32 size;

    if (g_loaded_rel_count >= 10)
        OSPanic("relocation.c", 58, "no more relocation module\n");
    g_loaded_rel_infos[g_loaded_rel_count++] = rel;
    rel->info = NULL;
    rel->bss = NULL;
    if (DVDOpen(name, &file))
    {
        size = OSRoundUp32B(file.length);
        rel->info = rel_alloc(size, 68);
        if (read_entire_file_using_dvdread_prio_async(&file, rel->info, size, 0) != 0)
        {
            rel->bss = rel_alloc(rel->info->bssSize != 0 ? rel->info->bssSize : 32, 71);
            OSLink(rel->info, rel->bss);
            rel->info->prolog();
        }
        else
        {
            OSFreeToHeap(currentHeap, rel->info);
            rel->info = NULL;
        }
        DVDClose(&file);
    }
}

void g_purge_rel(struct RelInfo *rel)
{
    if (rel->info != NULL)
    {
        if (g_loaded_rel_infos[g_loaded_rel_count - 1] != rel)
            OSPanic("relocation.c", 90, "rel_dest: illeagal purge order\n");
        g_loaded_rel_count--;
        rel->info->epilog();
        OSUnlink(rel->info);
        OSFreeToHeap(currentHeap, rel->bss);
        OSFreeToHeap(currentHeap, rel->info);
        rel->bss = rel->info = NULL;
    }
}

void g_purge_rel2(void)
{
    int i;

    for (i = g_loaded_rel_count - 1; i >= 0; i--)
        g_purge_rel(g_loaded_rel_infos[i]);
}
