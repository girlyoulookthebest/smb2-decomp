/* Depth-ordered draw list: entries double as an array of insertion points and a linked list. */
#include "types.h"
#include "camera.h"
#include "dip.h"
#include "light.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "pc_hfr.h"

extern u32 monkey_flags;
extern struct
{
    u8 filler0[0x20];
    u32 unk20;
} perfInfo;

void OSReport(const char *msg, ...);

void fn_1_CCB24(void);
void fn_1_CD29C(struct OrdTblNode *ordTbl, int count, u32 order);
void fn_1_CD344(struct OrdTblNode *node);
void fn_1_CD398(void);

u8 lbl_1_bss_A87E0[0x20000 * PC_PTR_SCALE]; /* 0x4000 OrdTblNode */

static struct
{
    u8 *currPtr;
    u8 *initPtr;
    u32 initSize;
    u32 currSize;
} s_allocInfo;

void fn_1_CCAA0(float depthOffset, float minDepth, float maxDepth, void *buffer, int maxEntries)
{
    g_ord_tbl_stuff->entries = buffer;
    g_ord_tbl_stuff->maxEntries = maxEntries;
    g_ord_tbl_stuff->depthOffset = depthOffset;
    g_ord_tbl_stuff->minDepth = minDepth;
    g_ord_tbl_stuff->maxDepth = maxDepth;
    g_ord_tbl_stuff->depthRange = maxDepth - minDepth;
    fn_1_CCB24();
}

void fn_1_CCB24(void)
{
    perf_init_timer(8);
    fn_1_CD29C(g_ord_tbl_stuff->entries, g_ord_tbl_stuff->maxEntries, 1);
    g_ord_tbl_stuff->lastEntry = &g_ord_tbl_stuff->entries[g_ord_tbl_stuff->maxEntries - 1];
    g_ord_tbl_stuff->firstEntry = &g_ord_tbl_stuff->entries[0];
    perfInfo.unk20 += perf_stop_timer(8);
}

void g_smth_with_rendefc_reflective_height(float offset)
{
    g_ord_tbl_stuff->depthOffset = offset;
}

void fn_1_CCBFC(float offset)
{
    g_ord_tbl_stuff->depthOffset += offset;
}

struct OrdTblNode *fn_1_CCC18(float z)
{
    int index;
    struct OrdTblNode *entry;

    z = -z + g_ord_tbl_stuff->depthOffset - g_ord_tbl_stuff->minDepth;
    if (z < 0.0f)
        index = 0;
    else
    {
        index = g_ord_tbl_stuff->maxEntries * z / (z + g_ord_tbl_stuff->depthRange);
        if (index >= g_ord_tbl_stuff->maxEntries)
            index = g_ord_tbl_stuff->maxEntries - 1;
    }

    entry = &g_ord_tbl_stuff->entries[index];
    if (g_ord_tbl_stuff->lastEntry > entry)
        g_ord_tbl_stuff->lastEntry = entry;
    if (g_ord_tbl_stuff->firstEntry < entry)
        g_ord_tbl_stuff->firstEntry = entry;
    return entry;
}

struct OrdTblNode *fn_1_CCD5C(int index)
{
    struct OrdTblNode *entry = &g_ord_tbl_stuff->entries[index];

    if (g_ord_tbl_stuff->lastEntry > entry)
        g_ord_tbl_stuff->lastEntry = entry;
    if (g_ord_tbl_stuff->firstEntry < entry)
        g_ord_tbl_stuff->firstEntry = entry;
    return entry;
}

struct OrdTblNode *fn_1_CCDC8(Vec *pos)
{
    Vec posView = *pos;
    float minDepth;
    float depth;
    int index;
    struct OrdTblNode *entry;

    minDepth = g_ord_tbl_stuff->minDepth;
    posView.z = -posView.z;
    if (posView.z < minDepth)
        posView.z = minDepth;
    depth = mathutil_vec_len(&posView) + g_ord_tbl_stuff->depthOffset - minDepth;

    if (depth < 0.0f)
        index = 0;
    else
    {
        index = g_ord_tbl_stuff->maxEntries * depth / (depth + g_ord_tbl_stuff->depthRange);
        if (index >= g_ord_tbl_stuff->maxEntries)
            index = g_ord_tbl_stuff->maxEntries - 1;
    }

    entry = &g_ord_tbl_stuff->entries[index];
    if (g_ord_tbl_stuff->lastEntry > entry)
        g_ord_tbl_stuff->lastEntry = entry;
    if (g_ord_tbl_stuff->firstEntry < entry)
        g_ord_tbl_stuff->firstEntry = entry;
    return entry;
}

struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos)
{
    Vec posView;
    float minDepth;
    float depth;
    int index;
    struct OrdTblNode *entry;

    mtxa_tf_point(pos, &posView);
    minDepth = g_ord_tbl_stuff->minDepth;
    posView.z = -posView.z;
    if (posView.z < minDepth)
        posView.z = minDepth;
    depth = mathutil_vec_len(&posView) + g_ord_tbl_stuff->depthOffset - minDepth;

    if (depth < 0.0f)
        index = 0;
    else
    {
        index = g_ord_tbl_stuff->maxEntries * depth / (depth + g_ord_tbl_stuff->depthRange);
        if (index >= g_ord_tbl_stuff->maxEntries)
            index = g_ord_tbl_stuff->maxEntries - 1;
    }

    entry = &g_ord_tbl_stuff->entries[index];
    if (g_ord_tbl_stuff->lastEntry > entry)
        g_ord_tbl_stuff->lastEntry = entry;
    if (g_ord_tbl_stuff->firstEntry < entry)
        g_ord_tbl_stuff->firstEntry = entry;
    return entry;
}

void ord_tbl_draw_nodes(void)
{
    perf_init_timer(3);
    if ((monkey_flags & 0x100) && g_ord_tbl_stuff->firstEntry >= g_ord_tbl_stuff->lastEntry)
    {
        if (g_ord_tbl_stuff->lastEntry > &g_ord_tbl_stuff->entries[0])
            g_ord_tbl_stuff->lastEntry[-1].next = NULL;
        fn_1_CD344(g_ord_tbl_stuff->firstEntry);
    }
    perfInfo.unk20 += perf_stop_timer(3);
    fn_1_CCB24();
}

void fn_1_CD29C(struct OrdTblNode *ordTbl, int count, u32 order)
{
    int i;
    struct OrdTblNode *node;

    if (order == 0)
    {
        node = ordTbl;
        for (i = count - 1; i > 0; i--, node++)
        {
            node->next = node + 1;
            node->drawFunc = NULL;
        }
        node->next = NULL;
        node->drawFunc = NULL;
    }
    else
    {
        node = &ordTbl[count - 1];
        for (i = count - 1; i > 0; i--, node--)
        {
            node->next = node - 1;
            node->drawFunc = NULL;
        }
        node->next = NULL;
        node->drawFunc = NULL;
    }
    fn_1_CD398();
}

void fn_1_CD344(struct OrdTblNode *node)
{
    while (node != NULL)
    {
        if (node->drawFunc != NULL)
        {
            HFR_NODE_BEGIN(node, node->drawFunc);
            node->drawFunc(node);
            HFR_NODE_END();
        }
        node = node->next;
    }
}

void fn_1_CD398(void)
{
    s_allocInfo.currPtr = s_allocInfo.initPtr;
    s_allocInfo.currSize = s_allocInfo.initSize;
}

void fn_1_CD3CC(void *addr, u32 size)
{
    s_allocInfo.initPtr = addr;
    s_allocInfo.initSize = size;
    fn_1_CD398();
}

void fn_1_CD418(struct OrdTblNode *tblEntry, struct OrdTblNode *newNode)
{
    newNode->next = tblEntry->next;
    tblEntry->next = newNode;
}

void *ord_tbl_alloc_node(u32 size)
{
    void *ptr;

    if (s_allocInfo.currSize < size)
    {
        if (g_some_other_flags & 4)
            OSReport("Ordering Primitive Over!!");
        return NULL;
    }
    ptr = s_allocInfo.currPtr;
    s_allocInfo.currPtr += size;
    s_allocInfo.currSize -= size;
    HFR_NODE_ALLOC(ptr);
    return ptr;
}
