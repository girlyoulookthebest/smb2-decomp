#include "baseball.h"

struct BBScreen lbl_18_bss_19278;

struct BBScreen *fn_18_21768(s32 arg0)
{
    s32 i;

    lbl_18_bss_19270 = &lbl_18_bss_19278;
    lbl_18_bss_19270->unk0 = arg0;
    load_bmp_by_id(arg0 == 0xF ? 0x16 : 0x15);
    lbl_18_bss_19270->unk154 = 0;
    for (i = 0; i < 0x3B; i++) {
        lbl_18_bss_19270->unk24C[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        lbl_18_bss_19270->unk338[i] = NULL;
    }
    lbl_18_bss_19270->unk88 = 2;
    return lbl_18_bss_19270;
}

s32 fn_18_21854(struct BBScreen *arg0)
{
    struct Sprite **sprite;
    s32 i;

    i = 0x80;
    sprite = lbl_18_bss_19270->unk338;
    while (i > 0) {
        if (*sprite != NULL) {
            g_poolInfo.spritePool.statusList[(*sprite)->poolIdx] = 0;
            *sprite = NULL;
        }
        i--;
        sprite++;
    }
    call_free_bmp_by_id(arg0->unk0 == 0xF ? 0x16 : 0x15);
    return 1;
}

u32 fn_18_218EC(s32 arg0)
{
    return sndFXStartEx(arg0, 0xFF, 0xFF, 0);
}

u32 fn_18_21918(s32 arg0, Vec *arg1)
{
    Vec sp8;
    float tf31;
    u32 tr3;

    mtxa_from_mtxb();
    mtxa_tf_point(arg1, &sp8);
    tf31 = 1.0f + math_sqrt((sp8.x * sp8.x) + (sp8.z * sp8.z));
    tr3 = sndFXStartEx(arg0, 0xFF, (u8) (64.0f + ((64.0f * sp8.x) / tf31)), 0);
    if (tr3 != -1) {
        g_call_synthFXSetCtrl_with_irq_disabled(tr3, 0x83, (u8) (64.0f - ((64.0f * sp8.z) / tf31)));
    }
    return tr3;
}

