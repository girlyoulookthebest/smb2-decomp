#pragma peephole off
#include "types.h"
#include "nl2ngc.h"

void fn_1_D2A88(struct NlModel *model, u32 mask, u32 value)
{
    struct NlMesh *mesh = (void *)model->meshStart;

    while ((u32)mesh->flags != 0)
    {
        mesh->flags &= mask;
        mesh->flags |= value;
        mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
    }
}

void fn_1_D2AC4(struct NlModel *model, u32 mask, u32 value)
{
    struct NlMesh *mesh = (void *)model->meshStart;

    while ((u32)mesh->flags != 0)
    {
        mesh->unk4 &= mask;
        mesh->unk4 |= value;
        mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
    }
}

void fn_1_D2B00(struct NlModel *model, u32 mask, u32 value)
{
    struct NlMesh *mesh = (void *)model->meshStart;

    while ((u32)mesh->flags != 0)
    {
        mesh->texFlags &= mask;
        mesh->texFlags |= value;
        mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
    }
}
