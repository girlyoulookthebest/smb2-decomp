/* Loading and freeing the common, minigame, world and effect model archives. */
#include "types.h"
#include "avdisp.h"

struct TPL;

struct GmaTplFiles
{
    char *gma;
    char *tpl;
};

/* Per-minigame model archives. */
struct GmaTplFiles minigameGmaTplFiles[16] =
{
    { "init/m_main.gma", "init/m_main.tpl" },
    { "init/m_race.gma", "init/m_race.tpl" },
    { "init/m_fight.gma", "init/m_fight.tpl" },
    { "init/m_tar2.gma", "init/m_tar2.tpl" },
    { "init/m_bowl.gma", "init/m_bowl.tpl" },
    { "init/m_golf.gma", "init/m_golf.tpl" },
    { "init/m_bill2.gma", "init/m_bill2.tpl" },
    { "init/roll.gma", "init/roll.tpl" },
    { "init/m_boa2.gma", "init/m_boa2.tpl" },
    { "init/m_dog2.gma", "init/m_dog2.tpl" },
    { "init/m_gun2_1.gma", "init/m_gun2_1.tpl" },
    { "init/m_gun2_2.gma", "init/m_gun2_2.tpl" },
    { "init/m_gun2_3.gma", "init/m_gun2_3.tpl" },
    { "init/m_fut2.gma", "init/m_fut2.tpl" },
    { "init/m_bowl2.gma", "init/m_bowl2.tpl" },
    { "init/m_fight2.gma", "init/m_fight2.tpl" },
};

struct TPL *minigameTpl;
struct GMA *minigameGma;
struct TPL *worldTpl;
struct GMA *worldGma;
struct GMA *efcmdlGma;
struct TPL *efcmdlTpl;
/* Not used here; unused globals are laid out in reverse. */
struct TPL *g_bg_tpl;
struct GMA *g_bg_gma;

extern u8 g_init_common_gma_buf[];
extern u8 g_init_common_tpl_buf[];
extern u8 g_ape_face_gma_buf[];
extern u8 g_ape_face_tpl_buf[];

void load_nlobj(void *gmaBuf, void *tplBuf, char *gmaFile, char *tplFile);
struct TPL *g_load_tpl(char *file);
struct GMA *g_load_gma(char *file, struct TPL *tpl);
void g_something_freeing_heap(struct GMA *gma);
void g_something_freeing_heap_2(struct TPL *tpl);
BOOL DVDChangeDir(char *dir);
void queue_disc_read(char *file);
void gp_wait_wrapper(void);
void free_minigame_gma_tpl(void);

void g_load_init_common(void)
{
    load_nlobj(g_init_common_gma_buf, g_init_common_tpl_buf, "init/common_p.lz", "init/common.lz");
}

/* Stand-in for a function the linker stripped (it loaded the monkeys' face models);
 * only its file names remain. */
void load_ape_face(void)
{
    load_nlobj(g_ape_face_gma_buf, g_ape_face_tpl_buf, "ape/face_p.lz", "ape/face.lz");
}

void g_load_minigame_gma_tpl(int idx)
{
    struct GmaTplFiles *files;

    if (idx >= 0 && idx < 16)
    {
        free_minigame_gma_tpl();
        files = &minigameGmaTplFiles[idx];
        if (files->tpl != NULL)
            minigameTpl = g_load_tpl(files->tpl);
        if (files->gma != NULL)
            minigameGma = g_load_gma(files->gma, minigameTpl);
    }
}

void free_minigame_gma_tpl(void)
{
    if (minigameTpl != NULL || minigameGma != NULL)
        gp_wait_wrapper();
    if (minigameTpl != NULL)
    {
        g_something_freeing_heap_2(minigameTpl);
        minigameTpl = NULL;
    }
    if (minigameGma != NULL)
    {
        g_something_freeing_heap(minigameGma);
        minigameGma = NULL;
    }
}

/* Per-world model archives (in the "world" directory). */
struct GmaTplFiles worldGmaTplFiles[8] =
{
    { "wl01.gma", "wl01.tpl" },
    { "wl02.gma", "wl02.tpl" },
    { "wl03.gma", "wl03.tpl" },
    { "wl04.gma", "wl04.tpl" },
    { "wl05.gma", "wl05.tpl" },
    { "wl06.gma", "wl06.tpl" },
    { "wl07.gma", "wl07.tpl" },
    { "wl08.gma", "wl08.tpl" },
};

void g_something_with_loading_wl_tpls(int world)
{
    struct GmaTplFiles *files;

    if (world > 0)
    {
        DVDChangeDir("world");
        files = &worldGmaTplFiles[world - 1];
        worldTpl = g_load_tpl(files->tpl);
        if (worldTpl != NULL)
        {
            worldGma = g_load_gma(files->gma, worldTpl);
            DVDChangeDir("/");
        }
    }
}

/* Note: waits for the GPU based on the minigame archives, not the world ones. */
void free_world_gma_tpl(void)
{
    if (minigameTpl != NULL || minigameGma != NULL)
        gp_wait_wrapper();
    if (worldTpl != NULL)
    {
        g_something_freeing_heap_2(worldTpl);
        worldTpl = NULL;
    }
    if (worldGma != NULL)
    {
        g_something_freeing_heap(worldGma);
        worldGma = NULL;
    }
}

void g_load_efcmdl_gmatpl(void)
{
    if (efcmdlGma == NULL)
    {
        efcmdlTpl = g_load_tpl("init/efcmdl.tpl");
        efcmdlGma = g_load_gma("init/efcmdl.gma", efcmdlTpl);
    }
}

void load_efcmdl_files_from_disc(void)
{
    queue_disc_read("init/efcmdl.gma");
    queue_disc_read("init/efcmdl.tpl");
}

void g_something_freeing_heap_parent(void)
{
    if (efcmdlTpl != NULL || efcmdlGma != NULL)
        gp_wait_wrapper();
    if (efcmdlTpl != NULL)
    {
        g_something_freeing_heap_2(efcmdlTpl);
        efcmdlTpl = NULL;
    }
    if (efcmdlGma != NULL)
    {
        g_something_freeing_heap(efcmdlGma);
        efcmdlGma = NULL;
    }
}
