/* Ape animation data: clip tables, blending and the per-bone event trees. */
#include "types.h"
#include "libc.h"
#include "ball.h"
#include "mathutil.h"
#include "skeleton.h"
#include "file.h"
#include "ape_internal.h"

static struct ApeEvent lbl_1_data_6B440 = {0};

struct ApeData *fn_1_114C1C(void)
{
    struct ApeData *d = OSAllocFromHeap(currentHeap, 0x60);

    d->unk0 = 0;
    d->flags = 0;
    d->clips = NULL;
    d->clipCount = 0;
    d->curGroup = 0;
    d->names = NULL;
    d->skelData = NULL;
    d->unk18 = 0;
    d->mta = NULL;
    d->events = NULL;
    d->eventCount = 0;
    d->boneCount = 0;
    d->skel = NULL;
    d->groups = OSAllocFromHeap(currentHeap, 0x20);
    d->count = 1;
    d->unk4C = 0;
    d->groupsAlt = NULL;
    d->clipsAlt = NULL;
    d->unk3C = NULL;
    d->namesAlt = NULL;
    d->unk44 = 0;
    d->clipCountAlt = 0;
    d->unk4A = 0;
    d->groups->name = "test";
    d->groups->clips = NULL;
    d->groups->count = 0;
    d->groups->current = 0;
    return d;
}

struct ApeData *g_get_structure_from_ape_common_mal_struct(struct ApeMal *mal)
{
    struct ApeData *d = OSAllocFromHeap(currentHeap, 0x60);
    int i;

    d->unk0 = mal->unk20;
    d->flags = mal->flags | 4;
    d->clips = OSAllocFromHeap(currentHeap, ALIGN32(mal->clipCount * 0x34));
    d->clipCount = mal->clipCount;
    d->curGroup = 0;
    d->mta = NULL;
    d->boneCount = mal->boneCount;
    d->eventCount = mal->unk10;
    d->unk18 = mal->unk18;
    d->count = mal->groupCount;
    d->groups = OSAllocFromHeap(currentHeap, ALIGN32(mal->groupCount * 0xC));
    d->names = (char *)mal + mal->namesOff;
    d->skelData = NULL;
    d->skel = NULL;
    d->groupsAlt = NULL;
    d->countAlt = 0;
    d->clipsAlt = NULL;
    d->unk3C = NULL;
    d->namesAlt = NULL;
    d->clipCountAlt = 0;
    d->unk4C = 0;
    d->unk44 = 0;
    d->unk4A = 0;
    if (d->eventCount != 0)
        d->events = (struct ApeEvent *)((u8 *)mal + mal->unk14);
    else
        d->events = NULL;
    d->unk2C = OSAllocFromHeap(currentHeap, ALIGN32(mal->boneCount << 4));
    d->unk30 = OSAllocFromHeap(currentHeap, ALIGN32(mal->boneCount << 4));
    for (i = 0; i < mal->boneCount; i++)
    {
        g_set_ape_stuff_in_chara_heap(&d->unk2C[i], &lbl_1_data_6B440);
        g_set_ape_stuff_in_chara_heap(&d->unk30[i], &lbl_1_data_6B440);
    }
    {
        struct ApeMalClip *clips = (struct ApeMalClip *)((u8 *)mal + mal->clipsOff);
        struct ApeTrack *tracks = (struct ApeTrack *)((u8 *)mal + mal->framesOff);
        struct ApeMalGroup *groups = (struct ApeMalGroup *)((u8 *)mal + mal->groupsOff);

        for (i = 0; i < mal->groupCount; i++)
        {
            struct ApeMalGroup *src;
            struct ApeClipGroup *dst;

            dst = &d->groups[i];
            src = &groups[i];

            dst->name = d->names + src->nameOff;
            dst->clips = &d->clips[src->firstClip];
            dst->count = src->count;
            dst->current = 0;
        }
        for (i = 0; i < mal->clipCount; i++)
        {
            struct ApeMalClip *src;
            struct ApeClipInfo *dst;

            dst = &d->clips[i];
            src = &clips[i];

            dst->flags = src->unk0;
            dst->unk4 = src->unk4;
            dst->unk5 = src->unk5;
            dst->unk6 = src->unk6;
            dst->unk8 = src->unk8;
            dst->unkA = src->unkA;
            dst->unkC = src->unkC;
            dst->unkE = src->unkE;
            dst->unk14 = src->unk10;
            dst->unk18 = src->unk14;
            dst->nameOff = src->nameOff;
            dst->unk20 = src->unk18;
            dst->flagsOff = src->flagsOff;
            dst->unk10 = 0;
            dst->next = NULL;
            dst->prev = NULL;
            dst->tracks = &tracks[i * mal->boneCount];
        }
    }
    return d;
}

void fn_1_115094(struct ApeData *d, struct ApeMal *mal)
{
    struct ApeMalClip *clips;
    struct ApeTrack *tracks;
    int i;
    struct ApeMalGroup *groups;

    d->clipsAlt = OSAllocFromHeap(currentHeap, ALIGN32(mal->clipCount * 0x34));
    d->clipCountAlt = mal->clipCount;
    d->unk4A = mal->unk10;
    if (d->unk4A != 0)
        d->unk3C = (struct ApeEvent *)((u8 *)mal + mal->unk14);
    d->countAlt = mal->groupCount;
    d->groupsAlt = OSAllocFromHeap(currentHeap, ALIGN32(mal->groupCount * 0xC));
    d->namesAlt = (char *)mal + mal->namesOff;
    d->unk44 = mal->unk18;
    if ((u8)d->unk4C < d->count)
        d->unk4C = d->count;
    clips = (struct ApeMalClip *)((u8 *)mal + mal->clipsOff);
    tracks = (struct ApeTrack *)((u8 *)mal + mal->framesOff);
    groups = (struct ApeMalGroup *)((u8 *)mal + mal->groupsOff);
    for (i = 0; i < mal->groupCount; i++)
    {
        struct ApeMalGroup *src;
        struct ApeClipGroup *dst;

        dst = &d->groupsAlt[i];
        src = &groups[i];

        dst->name = d->namesAlt + src->nameOff;
        dst->clips = &d->clipsAlt[src->firstClip];
        dst->count = src->count;
        dst->current = 0;
    }
    for (i = 0; i < mal->clipCount; i++)
    {
        struct ApeClipInfo *dst = &d->clipsAlt[i];
        struct ApeMalClip *src = &clips[i];

        dst->flags = src->unk0;
        dst->unk4 = src->unk4;
        dst->unk5 = src->unk5;
        dst->unk6 = src->unk6;
        dst->unk8 = src->unk8;
        dst->unkA = src->unkA;
        dst->unkC = src->unkC;
        dst->unkE = src->unkE;
        dst->unk14 = src->unk10;
        dst->unk18 = src->unk14;
        dst->nameOff = src->nameOff;
        dst->unk20 = src->unk18;
        dst->flagsOff = src->flagsOff;
        dst->unk10 = 0;
        dst->next = NULL;
        dst->prev = NULL;
        dst->unk2 = g_table_index(d->mta, d->namesAlt + dst->nameOff);
        if (dst->unk2 == 0xFFFF)
            dst->unk2 = 0;
        dst->tracks = &tracks[i * mal->boneCount];
    }
}

void fn_1_1152D8(struct ApeData *d, u8 a)
{
    d->unk4C = a;
}

void g_something_freeing_heap_3(struct ApeData *d)
{
    if (d->flags & 1)
    {
        if (!(d->flags & 4))
        {
            int i;

            for (i = 0; i < d->clipCount; i++)
            {
                if (d->clips[i].tracks != NULL)
                    OSFreeToHeap(currentHeap, d->clips[i].tracks);
            }
            if (d->names != NULL)
                OSFreeToHeap(currentHeap, d->names);
            if (d->events != NULL)
                OSFreeToHeap(currentHeap, d->events);
        }
        OSFreeToHeap(currentHeap, d->clips);
    }
    else
    {
        struct ApeClipInfo *c = d->clips;

        while (c != NULL)
        {
            struct ApeClipInfo *next = c->next;

            if (c->tracks != NULL)
                OSFreeToHeap(currentHeap, c->tracks);
            OSFreeToHeap(currentHeap, c);
            c = next;
        }
        if (d->events != NULL)
            OSFreeToHeap(currentHeap, d->events);
    }
    if (d->groups != NULL)
        OSFreeToHeap(currentHeap, d->groups);
    if (d->groupsAlt != NULL)
        OSFreeToHeap(currentHeap, d->groupsAlt);
    if (d->clipsAlt != NULL)
        OSFreeToHeap(currentHeap, d->clipsAlt);
    if (d->unk2C != NULL)
        OSFreeToHeap(currentHeap, d->unk2C);
    if (d->unk30 != NULL)
        OSFreeToHeap(currentHeap, d->unk30);
    d->groupsAlt = NULL;
    d->countAlt = 0;
    d->clipsAlt = NULL;
    d->unk3C = NULL;
    d->namesAlt = NULL;
    d->clipCountAlt = 0;
    d->unk4C = 0;
    if (d->skel != NULL && !(d->flags & 2))
        OSFreeToHeap(currentHeap, d->skel);
    OSFreeToHeap(currentHeap, d);
}

void fn_1_115568(struct ApeData *d, int n)
{
    struct ApeClipGroup *group;
    struct ApeClipInfo *c;
    int i;

    if ((d->flags & 1) || n == 0)
        return;
    d->clipCount += n;
    group = fn_1_1157E4(d, d->curGroup);
    c = fn_1_117C64(0, d->unk0);
    if (group->clips != NULL)
    {
        struct ApeClipInfo *p;

        for (p = group->clips; p->next != NULL; p = p->next)
            ;
        p->next = c;
        c->prev = p;
    }
    else
    {
        group->clips = c;
        if (d->clips == NULL)
            d->clips = c;
    }
    if (--n != 0)
    {
        for (i = 0; i < n; i++)
        {
            struct ApeClipInfo *p = fn_1_117C64(0, d->unk0);

            p->prev = c;
            c->next = p;
            c = p;
        }
    }
}

struct ApeClipInfo *fn_1_115674(struct ApeData *d, int idx)
{
    struct ApeClipGroup *group = fn_1_1157E4(d, d->curGroup);

    if (d->flags & 1)
        return &group->clips[idx];
    {
        struct ApeClipInfo *c = group->clips;
        int i;

        for (i = 0; i < idx; i++)
            c = c->next;
        return c;
    }
}

struct ApeClipInfo *fn_1_1156F0(struct ApeData *d, int g, int idx)
{
    struct ApeClipGroup *group = fn_1_1157E4(d, g);

    if (d->flags & 1)
        return &group->clips[idx];
    {
        struct ApeClipInfo *c = group->clips;
        int i;

        for (i = 0; i < idx; i++)
            c = c->next;
        return c;
    }
}

struct ApeClipInfo *g_get_ape_flag(struct ApeData *d)
{
    struct ApeClipGroup *group = fn_1_1157E4(d, d->curGroup);

    if (d->flags & 1)
        return &group->clips[group->current];
    {
        struct ApeClipInfo *c = group->clips;
        int i;

        for (i = 0; i < group->current; i++)
            c = c->next;
        return c;
    }
}

struct ApeClipGroup *fn_1_1157E4(struct ApeData *d, int g)
{
    struct ApeClipGroup *group;

    if (g < d->count)
        group = &d->groups[g];
    else if (d->groupsAlt != NULL)
        group = &d->groupsAlt[g - d->unk4C];
    return group;
}

void fn_1_115828(struct ApeData *d, int *a, int *b)
{
    if (d->groups == NULL)
    {
        *a = -1;
        *b = 0;
    }
    else
    {
        struct ApeClipGroup *group = fn_1_1157E4(d, d->curGroup);

        *a = d->curGroup;
        *b = group->current;
    }
}

void fn_1_11589C(struct ApeData *d, void *a)
{
    ape_archive_offsets_to_pointers(a);
    d->skelData = a;
}

static inline u32 ape_clip_skeleton_size(struct Mta *mta, int idx)
{
    struct MtaClip *clip = mta->clips[idx];
    u8 *data = (u8 *)clip;

    return ape_skeleton_size(*(u16 *)data);
}

void *g_alloc_some_ape_state_0x8(struct ApeData *d, int idx)
{
    d->unk0 = idx;
    d->skel = OSAllocFromHeap(currentHeap, ALIGN32(ape_clip_skeleton_size(d->skelData, idx)));
    d->flags |= 2;
    return d->skel;
}

void fn_1_115960(struct ApeData *d, struct Mta *mta)
{
    d->mta = mta;
    ape_archive_offsets_to_pointers(mta);
    if (d->flags & 4)
    {
        int i;

        for (i = 0; i < d->clipCount; i++)
        {
            struct ApeClipInfo *c = &d->clips[i];

            c->unk2 = g_table_index(mta, d->names + c->nameOff);
            if (c->unk2 == 0xFFFF)
                c->unk2 = 0;
        }
    }
}

void g_ape_anim_unk3(struct ApeData *d)
{
    if (d->skelData == NULL || d->mta == NULL)
    {
        OSReport("Warning: Motion or Skeleton unassigned yet.\n");
        return;
    }
    if (d->skel == NULL)
    {
        d->skel = OSAllocFromHeap(currentHeap, ALIGN32(ape_clip_skeleton_size(d->skelData, d->unk0)));
    }
    init_component_state(d->skel, (struct Bone *)(d->skel + 1), d->skelData->clips[d->unk0]);
    g_init_mta(d->skel, d->mta);
    if (d->clips != NULL)
        g_init_skeleton_animation(d->skel, NULL, d->clips->unk2);
    else
        g_init_skeleton_animation(d->skel, NULL, 0);
}

void fn_1_115B1C(struct ApeData *d, u8 bone, u8 kind, void *unused, u16 key, float value)
{
    struct ApeClipInfo *clip = g_get_ape_flag(d);
    struct ApeTrack *track;
    struct ApeEvent *events;
    u16 idx;

    OSReport("%p\n", clip->tracks);
    if (clip->tracks == NULL)
    {
        int i;

        clip->tracks = OSAllocFromHeap(currentHeap, ALIGN32(d->boneCount << 2));
        for (i = 0; i < d->boneCount; i++)
        {
            clip->tracks[i].count = 0;
            clip->tracks[i].root = 0xFFFF;
        }
    }
    track = &clip->tracks[bone];
    events = d->events;
    idx = d->eventCount;
    OSReport("Anim: %d, %d\n", track->count, track->root);
    if (track->count != 0)
    {
        u16 *p = &track->root;

        do
        {
            if (key == events[*p].key)
            {
                idx = *p;
                goto found;
            }
            if ((u16)events[*p].key < key)
                p = &events[*p].right;
            else
                p = &events[*p].left;
        } while (*p != 0xFFFF);
        *p = idx;
    }
    else
    {
        track->root = idx;
    }
    track->count++;
    d->eventCount++;
    events[idx].right = 0xFFFF;
    events[idx].left = 0xFFFF;
found:
    events[idx].kind = kind;
    events[idx].key = key;
    events[idx].value = value;
}

int fn_1_115DBC(struct ApeData *d, u8 bone, void *out)
{
    struct ApeClipInfo *clip = g_get_ape_flag(d);
    struct ApeTrack *track;

    if (clip->tracks == NULL)
        return 0;
    track = &clip->tracks[bone];
    if (track->count == 0)
        return 0;
    fn_1_1177A4(d, track->root, 0, track->count, out);
    return track->count;
}

void fn_1_115EB8(struct ApeData *d, u8 bone, u16 key)
{
    struct ApeClipInfo *clip = fn_1_115674(d, d->curGroup);

    if (clip->tracks != NULL)
    {
        struct ApeTrack *track = &clip->tracks[bone];

        if (track->count != 0)
        {
            struct ApeEvent *events = d->events;
            u16 *p = &track->root;

            do
            {
                if (key == events[*p].key)
                {
                    if (events[*p].right != 0xFFFF)
                    {
                        u16 *q = &events[*p].right;

                        while (events[*q].left != 0xFFFF)
                            q = &events[*q].left;
                        if ((u16)*q == events[*p].right)
                        {
                            events[*q].left = events[*p].left;
                            *p = *q;
                        }
                        else
                        {
                            u16 n = *q;

                            *q = events[n].right;
                            events[n].left = events[*p].left;
                            events[n].right = events[*p].right;
                            *p = n;
                        }
                    }
                    else if (events[*p].left != 0xFFFF)
                    {
                        u16 *q = &events[*p].left;

                        while (events[*q].right != 0xFFFF)
                            q = &events[*q].right;
                        if ((u16)*q == events[*p].left)
                        {
                            events[*q].right = events[*p].right;
                            *p = *q;
                        }
                        else
                        {
                            u16 n = *q;

                            *q = events[n].left;
                            events[n].left = events[*p].left;
                            events[n].right = events[*p].right;
                            *p = n;
                        }
                    }
                    else
                    {
                        *p = 0xFFFF;
                    }
                    track->count--;
                    return;
                }
                if ((u16)events[*p].key < key)
                    p = &events[*p].right;
                else
                    p = &events[*p].left;
            } while (p != NULL);
        }
    }
}

#define NEW_EVENT(i) new->events[new->eventCount + (u16)(i)]

struct ApeData *fn_1_1161F4(struct ApeData *d)
{
    struct ApeData *new;
    int i;
    int j;
    struct ApeClipInfo *old;
    struct ApeClipInfo *dst;
    struct ApeClipInfo *c;
    int eventCount;

    if (d->flags & 1)
        return NULL;
    new = OSAllocFromHeap(currentHeap, 0x60);
    new->unk0 = d->unk0;
    new->flags = d->flags;
    new->clips = OSAllocFromHeap(currentHeap, ALIGN32(d->clipCount * 0x34));
    new->clipCount = d->clipCount;
    new->curGroup = 0;
    new->skelData = NULL;
    new->unk18 = 0;
    new->mta = NULL;
    new->boneCount = d->boneCount;
    new->eventCount = 0;
    new->skel = NULL;
    eventCount = 0;
    i = 0;
    for (c = d->clips; c->next != NULL; c = c->next)
    {
        if (c->tracks != NULL)
        {
            for (j = 0; j < d->boneCount; j++)
                eventCount += c->tracks[j].count;
        }
        i += strlen(fn_1_10DFDC(d->mta, c->unk2)) + 1;
    }
    if (eventCount != 0)
        new->events = OSAllocFromHeap(currentHeap, ALIGN32(eventCount << 4));
    else
        new->events = NULL;
    new->names = OSAllocFromHeap(currentHeap, ALIGN32(i + 0x40));
    for (i = 0; i < d->clipCount; i++)
    {
        old = fn_1_115674(d, i);
        dst = &new->clips[i];
        dst->flags = old->flags;
        dst->unk2 = old->unk2;
        dst->unk4 = old->unk4;
        dst->unk5 = old->unk5;
        dst->unk6 = old->unk6;
        dst->unk8 = old->unk8;
        dst->unkC = old->unkC;
        dst->unkA = old->unkA;
        dst->unkE = old->unkE;
        dst->unk14 = old->unk14;
        dst->unk18 = old->unk18;
        dst->unk20 = old->unk20;
        dst->flagsOff = old->flagsOff;
        dst->unk10 = old->unk10;
        dst->nameOff = fn_1_117D44(new, fn_1_10DFDC(d->mta, old->unk2));
        dst->tracks = OSAllocFromHeap(currentHeap, ALIGN32(d->boneCount << 2));
        for (j = 0; j < d->boneCount; j++)
        {
            if (old->tracks != NULL && old->tracks[j].count != 0)
            {
                u16 n;
                int k;

                dst->tracks[j].count = old->tracks[j].count;
                n = fn_1_1177A4(d, old->tracks[j].root, 0, 0, &new->events[new->eventCount]);
                dst->tracks[j].root = new->eventCount + fn_1_117868(&new->events[new->eventCount], n);
                new->eventCount += n;
                for (k = 0; (u16)k < n; k++)
                {
                    u16 left = NEW_EVENT(k).left;
                    u16 right = NEW_EVENT(k).right;

                    if (left != 0xFFFF)
                        left += new->eventCount;
                    if (right != 0xFFFF)
                        right += new->eventCount;
                    NEW_EVENT(k).left = left;
                    NEW_EVENT(k).right = right;
                }
            }
            else
            {
                dst->tracks[j].count = 0;
                dst->tracks[j].root = 0xFFFF;
            }
        }
    }
    new->flags |= 1;
    return new;
}

struct ApeMal *fn_1_1166B0(struct ApeData *d, u32 *size)
{
    struct ApeMal *mal;
    char *names;

    if (!(d->flags & 1))
        return NULL;
    *size = sizeof(struct ApeMal);
    *size += d->clipCount * sizeof(struct ApeMalClip);
    *size += d->eventCount * sizeof(struct ApeEvent);
    *size += d->unk18;
    *size += d->clipCount * (d->boneCount * sizeof(struct ApeTrack));
    mal = OSAllocFromHeap(currentHeap, ALIGN32(*size));
    mal->clipsOff = sizeof(struct ApeMal);
    mal->framesOff = mal->clipsOff + d->clipCount * sizeof(struct ApeMalClip);
    mal->unk14 = mal->framesOff + d->clipCount * (d->boneCount * sizeof(struct ApeTrack));
    mal->namesOff = mal->unk14 + d->eventCount * sizeof(struct ApeEvent);
    mal->version = 1.0f;
    mal->clipCount = d->clipCount;
    mal->unk10 = d->eventCount;
    mal->unk18 = d->unk18;
    mal->unk20 = d->unk0;
    mal->flags = d->flags;
    mal->boneCount = d->boneCount;
    {
        struct ApeMalClip *clips = (struct ApeMalClip *)((u8 *)mal + mal->clipsOff);
        struct ApeTrack *tracks = (struct ApeTrack *)((u8 *)mal + mal->framesOff);
        struct ApeEvent *events = (struct ApeEvent *)((u8 *)mal + mal->unk14);
        int i;

        names = (char *)mal + mal->namesOff;
        for (i = 0; i < d->clipCount; i++)
        {
            int j;
            struct ApeTrack *t = &tracks[i * d->boneCount];

            clips[i].unk0 = d->clips[i].flags;
            clips[i].unk4 = d->clips[i].unk4;
            clips[i].unk5 = d->clips[i].unk5;
            clips[i].unk6 = d->clips[i].unk6;
            clips[i].unk8 = d->clips[i].unk8;
            clips[i].unkC = d->clips[i].unkC;
            clips[i].unkA = d->clips[i].unkA;
            clips[i].unkE = d->clips[i].unkE;
            clips[i].unk10 = d->clips[i].unk14;
            clips[i].unk14 = d->clips[i].unk18;
            clips[i].nameOff = d->clips[i].nameOff;
            clips[i].unk18 = d->clips[i].unk20;
            clips[i].flagsOff = d->clips[i].flagsOff;
            if (d->clips[i].tracks != NULL)
            {
                for (j = 0; j < d->boneCount; j++)
                {
                    t[j].count = d->clips[i].tracks[j].count;
                    t[j].root = d->clips[i].tracks[j].root;
                }
            }
            else
            {
                for (j = 0; j < d->boneCount; j++)
                {
                    t[j].count = 0;
                    t[j].root = 0xFFFF;
                }
            }
        }
        memcpy(events, d->events, d->eventCount * sizeof(struct ApeEvent));
    }
    memcpy(names, d->names, d->unk18);
    return mal;
}

static inline void ape_set_group_current(struct ApeData *d, int group, u16 idx)
{
    struct ApeClipGroup *g;

    if (group < d->count)
        g = &d->groups[group];
    else
        g = &d->groupsAlt[group - d->unk4C];
    g->current = idx;
}

void g_set_ape_dance_state1(struct ApeData *d, int group, int idx)
{
    struct Skeleton *skel = d->skel;
    struct ApeClipInfo *clip = g_get_ape_flag(d);
    struct AnimClock info;
    u16 flags;

    d->curGroup = group;
    ape_set_group_current(d, group, idx);
    clip = g_get_ape_flag(d);
    info.loopStart = clip->unk8;
    info.unk6 = clip->unkC;
    info.end = clip->unkA;
    flags = 0;
    if (clip->flags & 1)
        flags |= 1;
    if (clip->flags & 2)
        flags |= 2;
    g_unk_ape_anim1(skel, &info, clip->unk2, clip->unk5, flags);
    if (lbl_80198B3C == 1)
        skel->speed = 1.2 * clip->unk18;
    else
        skel->speed = clip->unk18;
    if (clip->flags & 0x10)
        skel->clock.flags |= 1;
}

void g_set_ape_dance_state2(struct ApeData *d, int group, int idx)
{
    struct AnimClock info;
    u16 flags;
    struct ApeClipGroup *g;
    struct ApeClipInfo *guard;
    struct Skeleton *skel;
    struct ApeClipInfo *clip;

    if (group < d->count)
        g = &d->groups[group];
    else
        g = &d->groupsAlt[group - d->unk4C];
    if (group == d->curGroup && g->current == idx)
        return;
    guard = g_get_ape_flag(d);
    if (guard->flags & 8)
    {
        struct Skeleton *s = d->skel;

        if (s->clock.flags & 1)
        {
            if (!(s->clock.flags & 2))
                return;
        }
        else
        {
            if (s->clock.loops == 0)
                return;
        }
    }
    skel = d->skel;
    guard = g_get_ape_flag(d);
    d->curGroup = group;
    ape_set_group_current(d, group, idx);
    clip = g_get_ape_flag(d);
    info.loopStart = clip->unk8;
    info.unk6 = clip->unkC;
    info.end = clip->unkA;
    flags = 0;
    if (clip->flags & 1)
        flags |= 1;
    if (clip->flags & 2)
        flags |= 2;
    g_unk_ape_anim1(skel, &info, clip->unk2, clip->unk5, flags);
    if (lbl_80198B3C == 1)
        skel->speed = 1.2 * clip->unk18;
    else
        skel->speed = clip->unk18;
    if (clip->flags & 0x10)
        skel->clock.flags |= 1;
}

void fn_1_117018(struct ApeData *d)
{
    int i;
    struct ApeClipInfo *clip;
    struct ApeEvent *cur;
    struct ApeEvent *a;
    struct ApeEvent *b;
    struct ApeEvent *next;

    fn_1_10BE3C(d->skel);
    clip = g_get_ape_flag(d);
    for (i = 0; i < d->boneCount; i++)
    {
        cur = g_some_ape_anim_func2(d, i);
        a = &d->unk2C[i];
        b = &d->unk30[i];
        if (cur != NULL)
        {
            next = fn_1_1175A0(d, i);
        }
        else
        {
            cur = &lbl_1_data_6B440;
            next = &lbl_1_data_6B440;
        }
        if (!(a->unk0 & 4))
            g_set_ape_stuff_in_chara_heap(a, next);
        g_set_ape_stuff_in_chara_heap(b, cur);
    }
}

void g_ape_anim_unk4(struct ApeData *d, int bone, struct ApeAnimOut *out)
{
    struct ApeEvent *a = &d->unk2C[bone];
    struct ApeEvent *b = &d->unk30[bone];
    float diff;

    out->a = a->kind;
    out->b = b->kind;
    diff = b->key - a->key;
    if (0.0f == diff)
        diff = 1.0f;
    out->t = (d->skel->clock.frame - a->key) / diff;
    if ((u16)out->a == out->b)
    {
        float v0 = a->value;
        float v1 = b->value;

        out->t = v0 + out->t * (v1 - v0);
        out->t /= 10000.0f;
    }
    out->flags = b->unk0;
}

void fn_1_1172C8(struct ApeData *d, int type, int (*func)(void *, char *), void *arg)
{
    int i;
    struct ApeEvent *ev;

    for (i = 0; i < d->eventCount; i++)
    {
        ev = &d->events[i];
        if (ev->type == type)
            ev->kind = (u16)func(arg, d->names + ev->kind);
    }
    for (i = 0; i < d->unk4A; i++)
    {
        ev = &d->unk3C[i];
        if (ev->type == type)
            ev->kind = (u16)func(arg, d->namesAlt + ev->kind);
    }
}

struct ApeEvent *g_some_ape_anim_func2(struct ApeData *d, int bone)
{
    struct ApeClipInfo *clip = g_get_ape_flag(d);
    struct ApeTrack *track = &clip->tracks[bone];
    u16 t = d->skel->clock.frame;
    struct ApeEvent *events;

    if (d->curGroup >= d->count)
        events = d->unk3C;
    else
        events = d->events;
    if (track->count != 0 || events != NULL)
    {
        if (d->flags & 4)
        {
            struct ApeEvent *base;
            int i;

            if (track->count == 0)
                return NULL;
            base = &events[track->root];
            for (i = 0; i < track->count; i++)
            {
                struct ApeEvent *ev = &base[i];

                if (t < ev->key)
                    break;
            }
            if (i == track->count)
                i = 0;
            return &base[i];
        }
        else
        {
            u16 idx = track->root;
            u16 best = 0xFFFF;

            do
            {
                struct ApeEvent *ev = &events[idx];

                if (t < ev->key)
                {
                    best = idx;
                    idx = ev->left;
                }
                else
                {
                    idx = ev->right;
                }
            } while (idx != 0xFFFF);
            if (best == 0xFFFF)
            {
                best = track->root;
                while (events[best].left != 0xFFFF)
                    best = events[best].left;
            }
            return &events[best];
        }
    }
    return NULL;
}

struct ApeEvent *fn_1_1175A0(struct ApeData *d, int bone)
{
    struct ApeClipInfo *clip = g_get_ape_flag(d);
    struct ApeTrack *track = &clip->tracks[bone];
    u16 t = d->skel->clock.frame;
    struct ApeEvent *events;

    if (d->curGroup >= d->count)
        events = d->unk3C;
    else
        events = d->events;
    if (track->count != 0 || events != NULL)
    {
        if (d->flags & 4)
        {
            struct ApeEvent *base;
            int i;

            base = &events[track->root];
            for (i = 0; i < track->count; i++)
            {
                struct ApeEvent *ev = &base[i];

                if (t < ev->key)
                    break;
            }
            if (i == 0)
                i = track->count;
            return &base[i - 1];
        }
        else
        {
            u16 idx = track->root;
            u16 best = 0xFFFF;

            do
            {
                struct ApeEvent *ev = &events[idx];

                if (t < ev->key)
                {
                    best = idx;
                    idx = ev->left;
                }
                else
                {
                    idx = ev->right;
                }
            } while (idx != 0xFFFF);
            if (best == 0xFFFF)
            {
                best = track->root;
                while (events[best].left != 0xFFFF)
                    best = events[best].left;
            }
            return &events[best];
        }
    }
    return NULL;
}

void g_set_ape_stuff_in_chara_heap(struct ApeEvent *dst, struct ApeEvent *src)
{
    dst->unk0 = src->unk0;
    dst->kind = src->kind;
    dst->key = src->key;
    dst->value = src->value;
}

u16 fn_1_1177A4(struct ApeData *d, u16 root, u16 n, u16 count, struct ApeEvent *out)
{
    int i;

    for (i = 0; i < count; i++)
    {
        out[n].kind = d->events[root + i].kind;
        out[n].value = d->events[root + i].value;
        out[n].key = d->events[root + i].key;
        out[n].left = out[n].right = 0xFFFF;
        n++;
    }
    return n;
}

u16 fn_1_117868(struct ApeEvent *events, u16 count)
{
    u16 mid;

    if (count == 0)
        return 0xFFFF;
    mid = (count - 1) / 2;
    events[mid].left = fn_1_117868(events, mid);
    events[mid].right = mid + fn_1_117868(events + mid + 1, count - mid - 1);
    return mid;
}

struct ApeClipInfo *fn_1_117C64(struct ApeClipInfo *clip, u8 b)
{
    if (clip == NULL)
        clip = OSAllocFromHeap(currentHeap, 0x40);
    clip->flags = 0;
    clip->unk2 = 0;
    clip->unk4 = b;
    clip->unk5 = 10;
    clip->unk6 = 0;
    clip->unk8 = 1;
    clip->unkA = 0;
    clip->unkC = 1;
    clip->unkE = 0;
    clip->unk14 = 0.0f;
    clip->unk18 = 1.0f;
    clip->nameOff = 0;
    clip->unk20 = -1;
    clip->flagsOff = 0;
    clip->unk10 = 0;
    clip->tracks = NULL;
    clip->prev = NULL;
    clip->next = NULL;
    return clip;
}

int fn_1_117D44(struct ApeData *d, char *name)
{
    int off = d->unk18;

    strcpy(d->names + off, name);
    d->unk18 += strlen(name) + 1;
    return off;
}
