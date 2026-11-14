#include "baseball.h"
#include "mathutil.h"

struct BBBatFuncs lbl_18_data_AB0[8] = {
    {(void *) fn_18_8FC0, (void *) fn_18_91AC, (void *) fn_18_91A4, (void *) fn_18_91A8},
    {(void *) fn_18_9294, (void *) fn_18_937C, (void *) fn_18_92E4, (void *) fn_18_92E8},
    {(void *) fn_18_9474, (void *) fn_18_94BC, (void *) fn_18_94B4, (void *) fn_18_94B8},
    {(void *) fn_18_95B8, (void *) fn_18_9730, (void *) fn_18_95F8, (void *) fn_18_95FC},
    {(void *) fn_18_9960, (void *) fn_18_99B0, (void *) fn_18_99A8, (void *) fn_18_99AC},
    {(void *) fn_18_9AD4, (void *) fn_18_9B08, (void *) fn_18_9B00, (void *) fn_18_9B04},
    {(void *) fn_18_9830, (void *) fn_18_9860, (void *) fn_18_985C, (void *) fn_18_995C},
    {(void *) fn_18_9C0C, (void *) fn_18_9C48, (void *) fn_18_9C40, (void *) fn_18_9C44},
};

u16 lbl_18_data_B30[8][2] = {
    {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {0xA, 0xB}, {0xC, 0xD}, {0, 0},
};

static inline void bat_set_state(s32 id, u8 state) {
    struct BBBat *s;
    struct Ball *ball;

    s = (struct BBBat *) lbl_18_bss_970.unk78;
    ball = &balls[id];
    if (lbl_18_data_AB0[s->state].unk8 != NULL) {
        lbl_18_data_AB0[s->state].unk8(ball, s);
    }
    s->state = state;
    if (lbl_18_data_AB0[s->state].unk0 != NULL) {
        lbl_18_data_AB0[s->state].unk0(ball, s);
    }
}

static inline void bat_set_unk7(u8 v) {
    struct BBBat *s = (struct BBBat *) lbl_18_bss_970.unk78;

    s->unk7 = v;
}
void fn_18_8CD4(u8 arg0)
{
    struct BBPlay *tr31;

    if (arg0 < 6) {
        tr31 = &lbl_18_bss_D68[arg0];
        if ((uptr) tr31->unk64 != 0) {
            g_something_with_freeing_chara_heap_ape(tr31->unk64);
        }
        memset(tr31, 0, sizeof(struct BBPlay));
    }
}

void fn_18_8D3C(void)
{
    struct BBPlay *tr31;
    s32 r30;

    for (r30 = 0; r30 < 6; r30++) {
        if ((u8) r30 < 6) {
            tr31 = &lbl_18_bss_D68[(u8) r30];
            if ((uptr) tr31->unk64 != 0) {
                g_something_with_freeing_chara_heap_ape(tr31->unk64);
            }
            memset(tr31, 0, sizeof(struct BBPlay));
        }
    }
}

void fn_18_8DC0(struct Ball *arg0)
{
    struct BBBat *s = (struct BBBat *) lbl_18_bss_970.unk78;

    s->state = 0;
    if (lbl_18_data_AB0[s->state].unk0 != NULL) {
        lbl_18_data_AB0[s->state].unk0(arg0, s);
    }
}

void fn_18_8E30(void)
{

}

void fn_18_8E34(struct Ball *arg0)
{
    u8 sp8[0x60];

    fn_18_84D4(arg0);
    fn_18_8EE0(arg0);
    fn_18_8EB0(arg0, sp8);
    fn_18_8558(arg0);
    fn_18_8784(arg0);
    if ((s32) (lbl_18_bss_2E0.unk44 & 1) != 0) {
        fn_18_138F8();
    }
    arg0->unk80++;
}

void fn_18_8EB0(struct Ball *arg0, void *arg1)
{
    PSVECAdd(&arg0->pos, &arg0->vel, &arg0->pos);
}

void fn_18_8EE0(struct Ball *arg0)
{
    struct BBBat *s = (struct BBBat *) lbl_18_bss_970.unk78;

    if (lbl_18_data_AB0[s->state].unkC != NULL) {
        lbl_18_data_AB0[s->state].unkC(arg0, s);
    }
    if (lbl_18_data_AB0[s->state].unk4 != NULL) {
        lbl_18_data_AB0[s->state].unk4(arg0, s);
    }
}

u8 fn_18_8FAC(void)
{
    struct BBBat *s = (struct BBBat *) lbl_18_bss_970.unk78;

    return s->state;
}

void fn_18_8FC0(struct Ball *arg0)
{
    mtxa_from_identity();
    mtxa_to_quat(&arg0->ape->rot);
    arg0->pos.x = stagedef->startPos->pos.x - 1.2f;
    arg0->pos.y = 0.5f;
    arg0->pos.z = stagedef->startPos->pos.z;
    arg0->prevPos = arg0->pos;
    arg0->vel.x = 0.0f;
    arg0->vel.y = 0.0f;
    arg0->vel.z = 0.0f;
    arg0->rotX = 0x2000;
    arg0->rotY = (s16) (stagedef->startPos->yrot - 0x4000);
    arg0->rotZ = 0;
    mtxa_from_translate(&arg0->pos);
    mtxa_rotate_y(arg0->rotY);
    mtxa_rotate_x(arg0->rotX);
    mtxa_rotate_z(arg0->rotZ);
    mtxa_to_mtx(arg0->unk30);
    mtxa_to_mtx(arg0->unkD0);
    arg0->speed = arg0->unkCC = 0.0f;
    {
        Vec sp18 = {0.0f, 0.0f, 0.0f};
        Vec *v = &sp18;

        arg0->unkC0 = *v;
    }
    arg0->ape->flags &= ~0x4000;
    {
        Quat sp8 = {0.0f, 0.0f, 0.0f, 1.0f};
        Quat *q = &sp8;

        arg0->unkB0 = *q;
    }
    arg0->unkA0 = arg0->unkB0;
    fn_18_9E20((u8) arg0->playerId, 0);
    fn_18_19F4(0);
}

void fn_18_91A4(void)
{

}

void fn_18_91A8(void)
{

}

void fn_18_91AC(struct Ball *arg0)
{
    bat_set_state(arg0->playerId, 1);
}

void fn_18_9294(struct Ball *arg0, struct BBBat *arg1)
{
    arg1->unk5 = 0;
    fn_18_9E88(arg0);
    fn_18_19F4(0);
    fn_18_9E20((u8) arg0->playerId, 0);
}

void fn_18_92E4(void)
{

}

void fn_18_92E8(struct Ball *arg0, struct BBBat *arg1)
{
    u16 spA;
    s8 sp9;
    s8 sp8;

    fn_18_1240(&sp9, &sp8, &spA);
    if (fn_18_A014(arg0, arg1, sp9, sp8) == 1) {
        fn_18_9E20((u8) arg0->playerId, 5);
        return;
    }
    if (fn_18_1584(arg0->ape) != 0) {
        fn_18_9E20((u8) arg0->playerId, 0);
    }
}

void fn_18_937C(struct Ball *arg0)
{
    if (fn_18_1A14() == 1) {
        bat_set_state(arg0->playerId, 2);
    }
}

void fn_18_9474(struct Ball *arg0)
{
    fn_18_19F4(1);
    fn_18_9E20((u8) arg0->playerId, 1);
}

void fn_18_94B4(void)
{

}

void fn_18_94B8(void)
{

}

void fn_18_94BC(struct Ball *arg0)
{
    if (fn_18_1584(arg0->ape) != 0) {
        bat_set_state(arg0->playerId, 3);
    }
}

void fn_18_95B8(struct Ball *arg0)
{
    fn_18_19F4(2);
    fn_18_9E20((u8) arg0->playerId, 2);
}

void fn_18_95F8(void)
{

}

void fn_18_95FC(struct Ball *arg0, struct BBBat *arg1)
{
    u16 spA;
    s8 sp9;
    s8 sp8;

    fn_18_1240(&sp9, &sp8, &spA);
    fn_18_A014(arg0, arg1, sp9, sp8);
    if ((spA & 0x100) != 0) {
        fn_18_18E30(0x5E);
        arg1->unk5 = 1;
        bat_set_state(arg0->playerId, 4);
    }
}

void fn_18_9730(struct Ball *arg0)
{
    fn_18_A2F4(arg0);
    if (fn_18_1A14() == 5) {
        bat_set_state(arg0->playerId, 6);
    }
}

void fn_18_9830(struct Ball *arg0)
{
    fn_18_9E20((u8) arg0->playerId, 6);
}

void fn_18_985C(void)
{

}

void fn_18_9860(struct Ball *arg0)
{
    if (fn_18_1584(arg0->ape) != 0) {
        bat_set_state(arg0->playerId, 7);
    }
}

void fn_18_995C(void)
{

}

void fn_18_9960(struct Ball *arg0)
{
    fn_18_18ED8(1);
    fn_18_9E20((u8) arg0->playerId, 3);
    fn_18_19F4(3);
}

void fn_18_99A8(void)
{

}

void fn_18_99AC(void)
{

}

void fn_18_99B0(struct Ball *arg0)
{
    fn_18_A2F4(arg0);
    if (((fn_18_A210(6) == 1) || (fn_18_A210(7) == 1)) && (fn_18_1584(arg0->ape) != 0)) {
        bat_set_state(arg0->playerId, 5);
    }
}

void fn_18_9AD4(struct Ball *arg0)
{
    fn_18_9E20((u8) arg0->playerId, 4);
}

void fn_18_9B00(void)
{

}

void fn_18_9B04(void)
{

}

void fn_18_9B08(struct Ball *arg0)
{
    fn_18_A2F4(arg0);
    if (fn_18_1584(arg0->ape) != 0) {
        bat_set_state(arg0->playerId, 7);
    }
}

void fn_18_9C0C(struct Ball *arg0)
{
    fn_18_9E20((u8) arg0->playerId, 0);
    fn_18_19F4(0);
}

void fn_18_9C40(void)
{

}

void fn_18_9C44(void)
{

}

void fn_18_9C48(struct Ball *arg0)
{
    fn_18_A2F4(arg0);
    if (fn_18_3EF4() == 1) {
        if (fn_18_1A14() != 1) {
            bat_set_state(arg0->playerId, 1);
        }
    } else if (fn_18_3EF4() == 4) {
        bat_set_state(arg0->playerId, 1);
    }
}

void fn_18_9E20(u8 arg0, u16 arg1)
{
    struct BBBat *s = (struct BBBat *) lbl_18_bss_970.unk78;
    u16 *t = lbl_18_data_B30[arg1];
    u8 r0;

    if (s->unk7 == 1) {
        r0 = 0;
    } else {
        r0 = 1;
    }
    t += r0;
    fn_18_1510(arg0, *t);
}

void fn_18_9E88(struct Ball *arg0)
{
    struct BBBat *s = (struct BBBat *) lbl_18_bss_970.unk78;

    fn_18_9FC8();
    mtxa_from_identity();
    mtxa_to_quat(&arg0->ape->rot);
    arg0->vel.x = arg0->vel.y = arg0->vel.z = 0.0f;
    arg0->rotX = 0x2000;
    arg0->rotY = -0x4000;
    arg0->rotZ = 0;
    if (s->unk6 % 2 == 0) {
        bat_set_unk7(1);
        arg0->pos.x = 1.2f;
        arg0->pos.y = 0.5f;
        arg0->pos.z = 0.0f;
        arg0->prevPos = arg0->pos;
    } else {
        bat_set_unk7(0);
        arg0->pos.x = -1.2f;
        arg0->pos.y = 0.5f;
        arg0->pos.z = 0.0f;
        arg0->prevPos = arg0->pos;
    }
    s->unk8 = 0.0f;
    s->unkC = 0.0f;
}

void fn_18_9FC8(void)
{
    struct BBBat *s = (struct BBBat *) lbl_18_bss_970.unk78;

    s->unk6 = fn_18_19978();
}

u8 fn_18_A000(void)
{
    struct BBBat *s = (struct BBBat *) lbl_18_bss_970.unk78;

    return s->unk7;
}

s32 fn_18_A014(struct Ball *arg0, struct BBBat *arg1, s8 arg2, s8 arg3)
{
    float sp8[2];
    float tf2;
    float tf3;
    float tf31;
    float tf30;
    s32 r30;

    r30 = 0;
    tf31 = fabs((float) arg2 / 60.0f);
    tf30 = fabs((float) arg3 / 60.0f);
    math_sin_cos_v(math_atan2((float) arg2, (float) arg3), sp8);
    tf2 = 0.025f * tf31 * sp8[0];
    tf3 = 0.025f * tf30 * sp8[1];
    if ((__abs(arg2) > 0xF) && (fabs(arg1->unk8 + tf2) < 0.4)) {
        arg0->pos.x += tf2;
        arg1->unk8 += tf2;
        r30 = 1;
    }
    if ((__abs(arg3) > 0xF) && (fabs(arg1->unkC - tf3) < 0.8)) {
        arg0->pos.z -= tf3;
        arg1->unkC -= tf3;
        r30 = 1;
    }
    return r30;
}

