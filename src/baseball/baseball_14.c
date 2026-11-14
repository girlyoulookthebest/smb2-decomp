#include "baseball.h"

void fn_18_DF60(void)
{
    struct Ball *ball1 = &balls[1];
    u8 ids[4] = {2, 3, 4, 5};
    float dist[4];
    struct Ball *ball = &balls[2];
    struct BBGamePl *pl;
    int i;

    for (i = 2; i <= 5; i++, ball++)
    {
        register float dx = ball1->pos.x - ball->pos.x;
        register float dz = ball1->pos.z - ball->pos.z;
#ifdef __MWERKS__
        asm
        {
            fmuls dx, dx, dx
            fmadds dx, dz, dz, dx
        }
#else
        dx = dx * dx + dz * dz;
#endif
        dist[i - 2] = dx;
    }
    for (i = 1; i <= 3; i++)
    {
        if (dist[0] > dist[i])
        {
            dist[0] = dist[i];
            ids[0] = ids[i];
        }
    }
    pl = &lbl_18_bss_970.unkBC[2];
    for (i = 2; i <= 5; i++, pl++)
        pl->unk8 = 0;
    pl = &lbl_18_bss_970.unkBC[ids[0]];
    pl->unk8 = 1;
}

