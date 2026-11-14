#include "boat.h"
#include "macros.h"

extern float lbl_1_data_657B4;
extern float lbl_1_data_657B8;
extern float lbl_1_data_657BC;
extern s16 lbl_1_data_657C0;

struct BoatObst lbl_14_bss_B7E8[0x20];

void (*lbl_14_data_21F8[1])(struct BoatObst *, struct BoatObst *) = { fn_14_23750 };
void (*lbl_14_data_21FC[1])(struct BoatObst *, struct Ball *, struct BoatPlayer *) = { fn_14_23830 };

void fn_14_234F4(void)
{
    s16 i;
    struct BoatObst *obst;

    memset(lbl_14_bss_B7E8, 0, sizeof(struct BoatObst));
    i = 0;
    obst = lbl_14_bss_B7E8;
    for (; i < 0x20; i++, obst++) {
        obst->type = -1;
    }
}

void fn_14_23554(struct BoatObst *req)
{
    s16 i;
    struct BoatObst *obst;

    i = 0;
    obst = lbl_14_bss_B7E8;
    for (; i < 0x20; i++, obst++) {
        if (obst->type < 0) {
            lbl_14_data_21F8[req->type](obst, req);
            if (obst->type >= 0) {
                obst->prevPos = obst->pos;
            }
            return;
        }
    }
    if ((dip_switches & 1) && i == 0x20) {
        printf("obst request : max\n");
    }
}

void fn_14_2363C(void)
{
    struct BoatObst *obst;
    s16 i;

    i = 0;
    obst = lbl_14_bss_B7E8;
    for (; i < 0x20; i++, obst++) {
        if (obst->type >= 0 && obst->timer >= 0) {
            obst->timer--;
            if (obst->timer < 0) {
                obst->type = -1;
            }
        }
    }
}

void fn_14_236A4(struct Ball *ball, struct BoatPlayer *player)
{
    struct BoatObst *obst;
    s16 i;

    i = 0;
    obst = lbl_14_bss_B7E8;
    for (; i < 0x20; i++, obst++) {
        if (obst->type >= 0) {
            lbl_14_data_21FC[obst->type](obst, ball, player);
        }
    }
}

void fn_14_23750(struct BoatObst *obst, struct BoatObst *req)
{
    float t;

    *obst = *req;
    obst->timer = lbl_1_data_657B4 * lbl_80198B34;
#ifdef __MWERKS__
    fn_14_23EB4(&obst->pos, lbl_1_data_657B8, -3.0f, t = lbl_1_data_657B4, t * lbl_80198B34);
#else
    /* pc_port: unsequenced in the original; MWCC evaluates left to right
     * (t is assigned before t * 60), x86 compilers need not. */
    t = lbl_1_data_657B4;
    fn_14_23EB4(&obst->pos, lbl_1_data_657B8, -3.0f, t, t * lbl_80198B34);
#endif
}

void fn_14_23830(struct BoatObst *obst, struct Ball *ball, struct BoatPlayer *player)
{
    Vec sp14;
    Vec sp8;
    float dot;

    if (obst->owner != ball->playerId && !(player->unk308 < 0.0f)) {
        PSVECSubtract(&obst->pos, &ball->pos, &sp14);
        if (!(vec_normalize_len(&sp14) > lbl_1_data_657B8)) {
            player->unk8 |= 0x200;
            if ((u32)(player->unkC & 0x200) == 0) {
                g_rumble_controller(player->unk1, 2, 0.3 * lbl_80198B34);
            }
            dot = mathutil_vec_dot_prod(&sp14, &ball->vel);
            if (dot < 0.0f) {
                PSVECScale(&sp14, &sp8, 0.5 * -dot);
                PSVECAdd(&ball->vel, &sp8, &ball->vel);
            }
            mtxa_from_rotate_y(lbl_1_data_657C0);
            mtxa_tf_vec(&sp14, &sp14);
            PSVECScale(&sp14, &sp14, lbl_1_data_657BC);
            PSVECAdd(&player->unk3D0, &sp14, &player->unk3D0);
        }
    }
}

