/* Background spectator monkeys ("guests"): a small set of apes loaded per theme,
 * animated each frame and drawn at the stagedef's guest positions, sorted by view
 * depth. Only the nearest few are drawn as full apes; the rest (Bonus theme) are
 * drawn as billboard sprites. */

#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "background.h"
#include "camera.h"
#include "frustum.h"
#include "macros.h"
#include "gx.h"
#include "mathutil.h"
#include "mode.h"
#include "stage.h"

struct BgGuestAttach
{
    /*0x00*/ char *name;
    /*0x04*/ s16 bone;
    /*0x08*/ Vec offset;
    /*0x14*/ s16 rotX;
    /*0x16*/ s16 rotY;
    /*0x18*/ s16 rotZ;
    /*0x1C*/ float scale;
    /*0x20*/ s16 apeIdx;
    /*0x22*/ u16 flags;
    /*0x24*/ struct GMAModel *model;
};  /* size = 0x28 */

struct BgGuestTheme
{
    /*0x00*/ s8 bgId;
    /*0x01*/ s8 animBase;
    /*0x02*/ s8 apeCount;
    /*0x04*/ float scale;
    /*0x08*/ s8 charaId;
    /*0x09*/ s8 variant;
    /*0x0A*/ s8 attachCount;
    /*0x0C*/ struct BgGuestAttach *attach;
};  /* size = 0x10 */

/* depth-sorted draw list node */
struct BgGuestNode
{
    /*0x00*/ struct StageBgGuest *guest;
    /*0x04*/ float depth;
    /*0x08*/ struct BgGuestNode *prev;
    /*0x0C*/ struct BgGuestNode *next;
};  /* size = 0x10 */

/* GMA texture descriptor following the model header */
struct GMATevLayer
{
    u8 unk0[8];
    /*0x08*/ GXTexObj * DPTR texObj;
    u8 unkC[0x20 - 0xC];
};

extern void *bg_heap;
#ifndef __MWERKS__
extern void *chara_heap;
#endif
extern struct GMA *g_bg_gma;
extern s8 lbl_1_bss_87F4C;

s8 lbl_1_bss_87FE0;
Mtx lbl_1_bss_87FE4;

void *OSFreeToHeap(void *heap, void *ptr);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void *memset(void *dst, int c, unsigned long n);
struct Ape *load_ape(int a, int b, int lod, int variant);
struct Ape *load_ape_gameplay(int chara, int lod, int variant);
void g_some_set_ape_anim(struct Ape *ape, int a, int b);
void run_anim_funcs(struct Ape *ape);
void fn_1_11355C(struct Ape *ape, float speed);
void g_ape_anim_body(struct Ape *ape);
void g_something_with_freeing_chara_heap_ape(struct Ape *ape);
void *get_GmaBuffer_entry(struct GMA *gma, char *name);
void *GXGetTexObjData(GXTexObj *obj);
void fn_1_113F64(struct Ape *ape, void *texData);
void fn_1_113FF8(struct Ape *ape);
void draw_ape(struct Ape *ape);
void *g_get_ape_anim_smth(struct Ape *ape, int bone);
void mtxa_from_mtxb_translate(Vec *v);
void mtxa_from_mtxb_translate_xyz(float x, float y, float z);
void g_smth_with_gpu_and_locked_cache_mtx(void);

void g_load_ape_bg(void);
void fn_1_A00D0(void);
void fn_1_A01C8(void);
void fn_1_A0298(void);
void fn_1_A08D4(float speed);
void fn_1_A08E4(void);
void fn_1_A08FC(void);
void fn_1_A0914(int maxDraw);

static struct BgGuestAttach s_billAttach[] =
{
    { "BILL_ERIMAKI0", 1, { 0.0f, 0.0f, 0.0f }, -0x1E6F, -0x4000, -0x1E6F, 1.0f, 0, 0, NULL },
    { "BILL_CUP", 8, { 0.6f, -0.1f, 0.28f }, 0, -0x8000, 0, 0.4082f, 0, 1, NULL },
    { "BILL_ERIMAKI1", 1, { 0.0f, 0.0f, 0.0f }, -0x1E6F, -0x4000, -0x1E6F, 1.0f, 1, 0, NULL },
    { "BILL_HAT", 3, { 0.0f, 0.0f, 0.0f }, 0, 0, 0, 1.0f, 1, 0, NULL },
    { "BILL_BIN", 12, { 0.37f, 0.73f, -0.7f }, 0, 0, 0, 0.4082f, 1, 2, NULL },
};

static struct BgGuestTheme s_themes[] =
{
    { 0x1E, 0, 3, 10.0f, 0, 1, 5, s_billAttach },
    { 0x1B, 8, 8, 1.0f, 2, 1, 0, NULL },
};

void g_load_ape_bg(void)
{
    struct BgGuestTheme *theme;
    int i;
    struct Ape *ape;
    struct BgGuestAttach *attach;

#ifndef __MWERKS__
    /* PC: the sel_stage debug stage picker (PC launcher "Practice") runs in
     * game heap config 0x11, which has no chara heap; the guest apes of the
     * futsal/bonus backgrounds would be allocated from heap -1 (undefined on
     * GC, an OSPanic here).  Show those backgrounds without their guests. */
    if ((sptr)chara_heap < 0)
    {
        world_theme.guestTheme = NULL;
        return;
    }
#endif
    theme = s_themes;
    for (i = 0; i < ARRAY_COUNT(s_themes); i++, theme++)
    {
        if (theme->bgId == world_theme.bgId)
        {
            world_theme.guestTheme = theme;
            break;
        }
    }
    if (theme != NULL)
    {
        world_theme.guestApes = alloc_from_heap_or_panic(bg_heap, theme->apeCount * sizeof(*world_theme.guestApes), "bg_guest.c", 125);
        memset(world_theme.guestApes, 0, theme->apeCount * sizeof(*world_theme.guestApes));
        for (i = 0; i < theme->apeCount; i++)
        {
            if (main_mode == 1)
                ape = world_theme.guestApes[i] = load_ape(0, 0, 0x19, 1);
            else
                ape = world_theme.guestApes[i] = load_ape_gameplay(0, theme->charaId, theme->variant);
            ape->flags |= 0x05000000;
            g_some_set_ape_anim(ape, 8, theme->animBase | i);
            run_anim_funcs(ape);
        }
        world_theme.guestAnimSpeed = 1.0f;
        attach = theme->attach;
        for (i = 0; i < theme->attachCount; i++, attach++)
        {
            struct GMAModel *model = get_GmaBuffer_entry(stage_gma, attach->name);

            if (model == NULL)
                model = get_GmaBuffer_entry(g_bg_gma, attach->name);
            attach->model = model;
        }
        world_theme.guestMaxDraw = 10;
        if (main_mode == 1)
            world_theme.guestMaxDraw = 5;
    }
}

/* model indices in g_bg_gma for the Bonus theme billboard guests, by ape index */
static s8 s_billboardModelIdx[8] = { 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A };

void fn_1_A00D0(void)
{
    struct BgGuestTheme *theme;
    int i;
    struct Ape *ape;

    if (!is_game_paused_and_in_view_stage() && world_theme.guestTheme != NULL)
    {
        theme = world_theme.guestTheme;
        for (i = 0; i < theme->apeCount; i++)
        {
            ape = world_theme.guestApes[i];
            if (ape != NULL)
            {
                if (world_theme.guestFlags & 1)
                    ape->robParam |= 0x40;
                else
                    ape->robParam &= ~0x40;
                run_anim_funcs(ape);
                fn_1_11355C(ape, world_theme.guestAnimSpeed);
                g_ape_anim_body(ape);
            }
        }
    }
}

void fn_1_A01C8(void)
{
    struct BgGuestTheme *theme;
    int i;
    struct Ape *ape;

    theme = world_theme.guestTheme;
    if (theme != NULL)
    {
        for (i = 0; i < theme->apeCount; i++)
        {
            ape = world_theme.guestApes[i];
            if (ape != NULL)
            {
                g_something_with_freeing_chara_heap_ape(ape);
                world_theme.guestApes[i] = NULL;
            }
        }
        world_theme.guestTheme = NULL;
        OSFreeToHeap(bg_heap, world_theme.guestApes);
        world_theme.guestApes = NULL;
    }
}

void fn_1_A0298(void)
{
    struct BgGuestTheme *theme;
    struct StageBgGuest *guest;
    struct BgGuestNode *nodes;
    struct BgGuestNode *head;
    struct BgGuestNode *node;
    int nodeIdx;
    struct Ape *ape;
    int i;
    struct BgGuestAttach *attach;
    struct GMAModel *model;
    int j;
    struct StageBgGuest *guest2;
    struct BgGuestNode *tail;
    Mtx tmp;
    Vec viewPos;

    if (stagedef->bgGuestCount <= 0)
        return;
    theme = world_theme.guestTheme;
    if (theme == NULL)
        return;

    nodes = alloc_from_heap_or_panic(bg_heap, (stagedef->bgGuestCount + 2) * sizeof(struct BgGuestNode), "bg_guest.c", 246);
    nodeIdx = 0;
    head = &nodes[nodeIdx++];
    tail = &nodes[nodeIdx++];
    head->guest = NULL;
    head->depth = 0.0f;
    head->prev = NULL;
    head->next = tail;
    tail->guest = NULL;
    tail->depth = 0.0f;
    tail->prev = head;
    tail->next = NULL;

    /* build a list of visible guests sorted by view depth (nearest first) */
    mtxa_from_mtxb();
    guest = stagedef->bgGuests;
    for (i = 0; i < stagedef->bgGuestCount; i++, guest++)
    {
        if (g_is_sphere_visible(&guest->pos, 0.5 * guest->scale * theme->scale)
         && (world_theme.bgId != 0x1B || lbl_1_bss_87F4C == 0 || !(guest->pos.z > 10.9)))
        {
            struct BgGuestNode *newNode;

            mtxa_tf_point(&guest->pos, &viewPos);
            node = head->next;
            while (node->guest != NULL)
            {
                if (viewPos.z > node->depth)
                    break;
                node = node->next;
            }
            newNode = &nodes[nodeIdx++];
            newNode->guest = guest;
            newNode->depth = viewPos.z;
            newNode->prev = node->prev;
            newNode->next = node;
            node->prev->next = newNode;
            node->prev = newNode;
        }
    }

    lbl_1_bss_87FE0 = 0;
    i = 0;
    for (node = head->next; node->guest != NULL; i++, node = node->next)
    {
        guest = node->guest;
        ape = world_theme.guestApes[guest->apeIdx];
        if (i < world_theme.guestMaxDraw)
        {
            mtxa_from_translate(&guest->pos);
            mtxa_rotate_z(guest->rotZ);
            mtxa_rotate_y(guest->rotY);
            mtxa_rotate_x(guest->rotX);
            mtxa_to_quat(&ape->rot);
            mathutil_mtxA_get_translate(&ape->pos);
            ape->scale = guest->scale * theme->scale;
            if (world_theme.bgId == 0x1E)
            {
                struct GMATevLayer *layer = (void *)((u8 *)stage_gma->modelEntries[0xB].model + 0x40);

                ape->colorIndex = 1;
                fn_1_113F64(ape, GXGetTexObjData(layer->texObj));
                ape->colorIndex = 0;
                draw_ape(ape);
                ape->colorIndex = 1;
                fn_1_113FF8(ape);
                ape->colorIndex = 0;
            }
            else
            {
                ape->colorIndex = guest->colorIdx;
                draw_ape(ape);
            }

            attach = theme->attach;
            for (j = 0; j < theme->attachCount; j++, attach++)
            {
                if (attach->apeIdx == guest->apeIdx && (model = attach->model) != NULL)
                {
                    if ((attach->flags & 1) && !(ape->robParam & 0x80))
                    {
                        mtxa_from_mtxb_translate_xyz(83.204475f, -4.701142f, 28.890257f);
                        mtxa_rotate_z(guest->rotZ);
                        mtxa_rotate_y(guest->rotY);
                        mtxa_rotate_x(guest->rotX);
                        mtxa_rotate_y(-0x3258);
                        g_smth_with_gpu_and_locked_cache_mtx();
                        avdisp_draw_model_unculled_sort_auto(model);
                    }
                    else
                    {
                        mtxa_from_quat(&ape->rot);
                        mtxa_to_mtx(tmp);
                        mtxa_from_mtxb_translate(&ape->pos);
                        mtxa_mult_right(tmp);
                        mtxa_scale_s(ape->scale);
                        mtxa_mult_right(g_get_ape_anim_smth(ape, attach->bone));
                        mtxa_scale_s(guest->scale / ape->scale);
                        mtxa_translate(&attach->offset);
                        mtxa_rotate_z(attach->rotZ);
                        mtxa_rotate_y(attach->rotY);
                        mtxa_rotate_x(attach->rotX);
                        mtxa_scale_s(attach->scale);
                        if (attach->flags & 2)
                        {
                            mtxa_to_mtx(lbl_1_bss_87FE4);
                            lbl_1_bss_87FE0 = 1;
                        }
                        g_smth_with_gpu_and_locked_cache_mtx();
                        avdisp_draw_model_unculled_sort_auto(model);
                    }
                }
            }
        }
        else if (world_theme.bgId == 0x1B)
        {
            float *bone;

            mtxa_from_mtxb_translate(&guest->pos);
            bone = g_get_ape_anim_smth(ape, 0);
            mtxa_translate_xyz(bone[3], bone[7], bone[11]);
            mtxa_rotate_y(g_current_camera->rotY);
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_unculled_sort_auto(g_bg_gma->modelEntries[s_billboardModelIdx[guest->apeIdx]].model);
        }
    }

    /* shadow discs under the full-size Bonus theme guests */
    i = 0;
    for (node = head->next; node->guest != NULL; i++, node = node->next)
    {
        if (i >= world_theme.guestMaxDraw)
            break;
        if (world_theme.bgId != 0x1B)
            break;
        guest2 = node->guest;
        switch (guest2->apeIdx)
        {
        case 0:
        case 1:
        case 2:
        case 5:
        case 6:
            set_post_mult_color(0.25f, 0.25f, 0.25f, 0.25f);
            mtxa_from_mtxb_translate(&guest2->pos);
            mtxa_translate_xyz(0.0f, 0.01f, 0.0f);
            mtxa_rotate_x(0x4000);
            mtxa_scale_s(0.36f);
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_set_z_mode(1, 3, 0);
            avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[0x67].model);
            avdisp_set_z_mode(1, 3, 1);
            set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        }
    }
    OSFreeToHeap(bg_heap, nodes);
}

void fn_1_A08D4(float speed)
{
    world_theme.guestAnimSpeed = speed;
}

void fn_1_A08E4(void)
{
    world_theme.guestFlags |= 1;
}

void fn_1_A08FC(void)
{
    world_theme.guestFlags &= ~1;
}

void fn_1_A0914(int maxDraw)
{
    world_theme.guestMaxDraw = maxDraw;
}
