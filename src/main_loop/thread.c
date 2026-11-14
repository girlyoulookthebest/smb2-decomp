#pragma peephole off
#include "types.h"

struct Thread
{
    void (*func)(void *, int);
    void *arg;
    struct Thread *next;
    struct Thread *prev;
    u32 unk10;
    u32 unk14;
    int id;
    u32 unk1C;
};

struct Thread *lbl_1_bss_C8AF8;
struct Thread *lbl_1_bss_C8AFC;
struct Thread *lbl_1_bss_C8B00[16];
struct Thread *lbl_1_bss_C8B40;
struct Thread *lbl_1_bss_C8B44;

/* Forces .bss order. */
static struct Thread *unused_bss(void)
{
    return lbl_1_bss_C8AF8;
}

void fn_1_D26B8(void *arg, int status)
{
}

void fn_1_D26BC(struct Thread *threads, int count)
{
    struct Thread *prev = NULL;
    int i;

    lbl_1_bss_C8AFC = threads;
    lbl_1_bss_C8AF8 = threads;
    for (i = 0; i < count; i++)
    {
        threads[i].func = NULL;
        threads[i].arg = NULL;
        threads[i].unk10 = 0;
        threads[i].unk14 = 0;
        threads[i].id = i;
        threads[i].unk1C = 0;
        threads[i].prev = prev;
        threads[i].next = &threads[i + 1];
        prev = &threads[i];
    }
    for (i = 0; i < 16; i++)
    {
        lbl_1_bss_C8B00[i] = &threads[i];
        threads[i].func = fn_1_D26B8;
        threads[i].next = NULL;
        threads[i].prev = NULL;
    }
    lbl_1_bss_C8B40 = &threads[i];
    lbl_1_bss_C8B44 = threads;
    threads[i].prev = NULL;
}

void fn_1_D27E4(int mask)
{
    int i;
    struct Thread *next;

    for (i = 0; i < 16; i++)
    {
        if (!(mask & (1 << i)))
        {
            struct Thread *t = lbl_1_bss_C8B00[i];

            while (t != NULL)
            {
                lbl_1_bss_C8AFC = t;
                next = t->next;
                t->func(t->arg, 0);
                t = next;
            }
        }
    }
}

int fn_1_D288C(void (*func)(void *, int), void *arg, int list)
{
    struct Thread *t = lbl_1_bss_C8B40;

    lbl_1_bss_C8B40 = t->next;
    lbl_1_bss_C8B40->prev = NULL;
    t->next = lbl_1_bss_C8B00[list]->next;
    if (t->next != NULL)
        t->next->prev = t;
    lbl_1_bss_C8B00[list]->next = t;
    t->prev = lbl_1_bss_C8B00[list];
    t->arg = arg;
    t->func = func;
    return t->id;
}

void fn_1_D292C(void)
{
    struct Thread *prev = lbl_1_bss_C8AFC->prev;
    struct Thread *next = lbl_1_bss_C8AFC->next;

    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;
    lbl_1_bss_C8AFC->prev = NULL;
    lbl_1_bss_C8AFC->next = lbl_1_bss_C8B40;
    if (lbl_1_bss_C8B40 != NULL)
        lbl_1_bss_C8B40->prev = lbl_1_bss_C8AFC;
    lbl_1_bss_C8B40 = lbl_1_bss_C8AFC;
}

void fn_1_D29E0(int id)
{
    struct Thread *t = &lbl_1_bss_C8B44[id];
    struct Thread *prev = t->prev;
    struct Thread *next = t->next;
    struct Thread *saved = lbl_1_bss_C8AFC;

    lbl_1_bss_C8AFC = t;
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;
    t->prev = NULL;
    t->next = lbl_1_bss_C8B40;
    if (lbl_1_bss_C8B40 != NULL)
        lbl_1_bss_C8B40->prev = t;
    lbl_1_bss_C8B40 = t;
    t->func(t->arg, 3);
    lbl_1_bss_C8AFC = saved;
}
