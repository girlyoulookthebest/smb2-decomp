/* Queued framebuffer Z reads, used to test whether points are occluded. */
#include "types.h"
#include "mathutil.h"

struct ZQuery
{
    s16 x;
    s16 y;
    u32 z;
};

struct ZQueryList
{
    struct ZQueryList *next;
    s32 count;
    struct ZQuery *queries;
};

void GXGetProjectionv(float *proj);
void GXGetViewportv(float *vp);
void GXPeekZ(u16 x, u16 y, u32 *z);

struct ZQueryList *lbl_1_bss_D5870;
struct ZQueryList *g_ptr_to_something;

void fn_1_117DB0(void)
{
    lbl_1_bss_D5870 = NULL;
    g_ptr_to_something = (struct ZQueryList *)&lbl_1_bss_D5870;
}

void g_something_with_GXPeekZ(void)
{
    int i;
    struct ZQueryList *cur;
    struct ZQueryList *list;
    struct ZQuery *query;

    for (cur = lbl_1_bss_D5870; cur != NULL; cur = cur->next)
    {
        list = cur;
        query = list->queries;
        for (i = list->count; i > 0; i--, query++)
        {
        if (query->z == 0)
            GXPeekZ(query->x, query->y, &query->z);
        }
    }
    fn_1_117DB0();
}

void fn_1_117E90(void) {}

void fn_1_117E94(int count, Vec *points, struct ZQuery *queries)
{
    float proj[7];
    float vp[6];
    Vec v;
    float cx;
    float cy;
    float right;
    float bottom;
    float t;
    int i;
    Vec *p;
    struct ZQuery *q;

    GXGetProjectionv(proj);
    GXGetViewportv(vp);
    cx = vp[0] + 0.5f * vp[2];
    cy = vp[1] + 0.5f * vp[3];
    right = vp[0] + vp[2] - 1.0f;
    bottom = vp[1] + vp[3] - 1.0f;
    p = points;
    q = queries;
    for (i = count; i > 0; i--, p++, q++)
    {
        v = *p;
        q->z = 0x1000000;
        if (v.z < 0.0f)
        {
            t = 1.0f / v.z;
            v.x = proj[2] + t * (v.x * proj[1]);
            v.y = proj[4] + t * (v.y * proj[3]);
            t = 0.5f * (1.0f + v.x);
            v.x = vp[0] * t + vp[2] * (1.0f - t);
            t = 0.5f * (1.0f + v.y);
            v.y = vp[1] * (1.0f - t) + vp[3] * t;
            if (v.x < vp[0])
            {
                t = (vp[0] - cx) / (v.x - cx);
                v.x = vp[0];
                v.y = cy + t * (v.y - cy);
            }
            else if (v.x > right)
            {
                t = (right - cx) / (v.x - cx);
                v.x = right;
                v.y = cy + t * (v.y - cy);
            }
            if (v.y < vp[1])
            {
                t = (vp[1] - cy) / (v.y - cy);
                v.x = cx + t * (v.x - cx);
                v.y = vp[1];
            }
            else if (v.y > bottom)
            {
                t = (bottom - cy) / (v.y - cy);
                v.x = cx + t * (v.x - cx);
                v.y = bottom;
            }
            q->x = v.x;
            q->y = v.y;
            q->z = 0;
        }
    }
}

u32 fn_1_118164(float z)
{
    float proj[7];
    float depth;

    GXGetProjectionv(proj);
    depth = (proj[6] + proj[5] * z) / z;
    depth = 16777216.0f * (1.0f - depth);
    if (depth < 0.0f)
        return 0;
    return depth;
}

void fn_1_1181E8(struct ZQueryList *list)
{
    g_ptr_to_something->next = list;
    list->next = NULL;
    g_ptr_to_something = list;
}

void fn_1_118210(struct ZQueryList *list)
{
    struct ZQueryList *prev = (struct ZQueryList *)&lbl_1_bss_D5870;

    while (prev->next != NULL && prev->next != list)
        prev = prev->next;
    if (prev->next != NULL)
    {
        prev->next = list->next;
        if (g_ptr_to_something == list)
            g_ptr_to_something = prev;
    }
}
