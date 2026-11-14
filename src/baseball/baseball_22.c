#include "baseball.h"

void fn_18_15A08(void)
{
    lbl_18_bss_11E8 = fn_18_21768(0xF);
}

void fn_18_15A38(void)
{
    if ((s32 *) lbl_18_bss_11E8 != NULL) {
        fn_18_21854(lbl_18_bss_11E8);
        lbl_18_bss_11E8 = NULL;
    }
}

void fn_18_15A88(void)
{
    if ((s32 *) lbl_18_bss_11E8 != NULL) {
        fn_18_1F400(lbl_18_bss_11E8, fn_18_334());
    }
}

void fn_18_15AD0(void)
{
    u8 *game = (u8 *) &lbl_18_bss_970 + 0x18;
    struct Ball *ball = &balls[1];
    struct Ball *ball2 = &balls[2];
    Vec pos;
    float absX;
    float scale;
    float limit;
    s32 i;
    u8 n;

    pos = ball->pos;
    if (__abs((s32) pos.x) > 116.0) {
        pos.x = 116.0f;
    }
    absX = __fabs(pos.x);
    limit = -35.0 * (absX < 90.0 ? (float) (1.0 - absX / 90.0) : 1.0f);
    if (pos.z < limit) {
        pos.z = limit;
    }
    lbl_18_bss_11E8->unkE4 = pos.x;
    lbl_18_bss_11E8->unkE8 = pos.z;
    for (i = 0; i < 4; i++) {
        ball = &ball2[i];
        pos = ball->pos;
        if (__abs((s32) pos.x) > 116.0) {
            pos.x = 116.0f;
        }
        absX = __fabs(pos.x);
        limit = -35.0 * (absX < 90.0 ? (float) (1.0 - absX / 90.0) : 1.0f);
        if (pos.z < limit) {
            pos.z = limit;
        }
        lbl_18_bss_11E8->unkEC[i].x = pos.x;
        lbl_18_bss_11E8->unkEC[i].z = pos.z;
    }
    lbl_18_bss_11E8->unk144 = game[0xF];
    n = 0;
    if (fn_18_19B90() == 1) {
        lbl_18_bss_11E8->unk124[n].x = lbl_18_data_368[1].x;
        lbl_18_bss_11E8->unk124[n].z = lbl_18_data_368[1].z;
        n++;
    }
    if (fn_18_19BB8() == 1) {
        lbl_18_bss_11E8->unk124[n].x = lbl_18_data_368[2].x;
        lbl_18_bss_11E8->unk124[n].z = lbl_18_data_368[2].z;
        n++;
    }
    if (fn_18_19BE0() == 1) {
        lbl_18_bss_11E8->unk124[n].x = lbl_18_data_368[3].x;
        lbl_18_bss_11E8->unk124[n].z = lbl_18_data_368[3].z;
    }
}

