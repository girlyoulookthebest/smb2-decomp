#include "baseball.h"

void fn_18_DE1C(struct Ball *ball, u8 arg1, struct BBAim *aim)
{
    float dx = lbl_18_data_C48[arg1][ball->playerId - 2].pos.x - ball->pos.x;
    float dz = lbl_18_data_C48[arg1][ball->playerId - 2].pos.z - ball->pos.z;

    if (fabs(dx) > 0.02)
        ball->pos.x += dx / 10.0;
    if (fabs(dz) > 0.02)
        ball->pos.z += dz / 10.0;
    ball->prevPos = ball->pos;
    aim->unk10 = lbl_18_data_C48[arg1][ball->playerId - 2].unkC;
    aim->unk14 = lbl_18_data_C48[arg1][ball->playerId - 2].unk10;
}

