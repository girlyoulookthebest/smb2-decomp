#include "boat.h"
#include "macros.h"

struct BoatParticle
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ float alpha;
    /*0x1C*/ float scale;
    /*0x20*/ s16 rot;
    /*0x22*/ s8 model;
    /*0x23*/ u8 filler23[1];
};

struct BoatParticleGroup
{
    /*0x000*/ u32 flags;
    /*0x004*/ Vec accel;
    /*0x010*/ float drag;
    /*0x014*/ s16 life;
    /*0x016*/ s16 timer;
    /*0x018*/ s16 ids[200];
    /*0x1A8*/ s16 count;
    /*0x1AA*/ u8 filler1AA[2];
};

u8 lbl_14_data_2068[0x170] = {
        0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
        0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
        0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
        0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
        0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};
s8 lbl_14_data_21D8[4] = {4, 0, 0, 0};
Vec lbl_14_data_21DC = {0.0f, 0.0f, 0.0f};

struct BoatParticle *lbl_14_bss_6360;
struct BoatParticleGroup *lbl_14_bss_6364;
struct BoatParticleParam lbl_14_bss_6368;
s16 lbl_14_bss_639C;
s16 lbl_14_bss_639E;
u8 lbl_14_bss_63A0[8];

void fn_14_21098(void)
{
    u32 size;

    if (lbl_14_bss_6360 == NULL) {
        size = 0x3200;
        lbl_14_bss_6360 = alloc_from_heap_or_panic(currentHeap, size, "boat_particle.c", 0x5F);
    }
    if (lbl_14_bss_6364 == NULL) {
        size = 0x15E00;
        lbl_14_bss_6364 = alloc_from_heap_or_panic(currentHeap, size, "boat_particle.c", 0x63);
    }
    lbl_14_bss_6368.models = lbl_14_data_21D8;
    lbl_14_bss_6368.numModels = 1;
    lbl_14_bss_6368.flags = 0;
    lbl_14_bss_6368.life = 0x1E;
    lbl_14_bss_6368.count = 1;
    lbl_14_bss_6368.pos = &lbl_14_data_21DC;
    lbl_14_bss_6368.vel = &lbl_14_data_21DC;
    lbl_14_bss_6368.accel = lbl_14_data_21DC;
    lbl_14_bss_6368.velScale = 1.0f;
    lbl_14_bss_6368.drag = 1.0f;
    lbl_14_bss_6368.posScale = 0.0f;
    lbl_14_bss_6368.fadeTime = 0;
    fn_14_21980();
    fn_14_219C4();
}

void fn_14_211EC(void)
{
    s32 i;
    s32 j;
    struct BoatParticleGroup *group;
    s16 count;
    struct BoatParticle *part;
    float alpha;

    if (lbl_14_bss_639C > 0 && (u32)(g_some_other_flags & 0xA) == 0) {
        count = 0;
        i = 0;
        group = lbl_14_bss_6364;
        for (; i < 200; i++, group++) {
            if (count >= lbl_14_bss_639C) {
                break;
            }
            if (group->flags != 0) {
                count++;
                group->timer--;
                if (group->timer <= group->life) {
                    if (group->timer <= 0) {
                        count--;
                        lbl_14_bss_639C--;
                        group->flags = 0;
                        for (j = 0; j < group->count; j++) {
                            lbl_14_bss_6360[group->ids[j]].model = -1;
                        }
                        lbl_14_bss_639E -= group->count;
                    } else {
                        if (group->timer < 0x1E) {
                            alpha = 0.1f + 0.03f * (float)group->timer;
                        } else {
                            alpha = 1.0f;
                        }
                        for (j = 0; j < group->count; j++) {
                            part = &lbl_14_bss_6360[group->ids[j]];
                            PSVECScale(&part->vel, &part->vel, group->drag);
                            PSVECAdd(&group->accel, &part->vel, &part->vel);
                            PSVECAdd(&part->vel, &part->pos, &part->pos);
                            part->alpha = alpha;
                        }
                    }
                }
            }
        }
    }
}

void fn_14_213D4(void)
{
    if (lbl_14_bss_6360 != NULL) {
        OSFreeToHeap(currentHeap, lbl_14_bss_6360);
        lbl_14_bss_6360 = NULL;
    }
    if (lbl_14_bss_6364 != NULL) {
        OSFreeToHeap(currentHeap, lbl_14_bss_6364);
        lbl_14_bss_6364 = NULL;
    }
}

void fn_14_21470(void)
{
    s32 i;
    Vec sp8;
    struct BoatParticle *part;
    s16 count;

    if (lbl_14_bss_639E > 0) {
        avdisp_set_z_mode(1, 3, 0);
        count = 0;
        i = 0;
        part = lbl_14_bss_6360;
        for (; i < 200; i++, part++) {
            if (count >= lbl_14_bss_639E) {
                break;
            }
            if (part->model != -1) {
                count++;
                if (0.0f != part->alpha) {
                    mtxa_from_mtxb();
                    mtxa_tf_point(&part->pos, &sp8);
                    if (!(sp8.z < -100.0f) && !(sp8.z > -1.4f)) {
                        mtxa_from_translate(&sp8);
                        mtxa_rotate_z(part->rot);
                        mtxa_scale_xyz(part->scale, part->scale, 0.0f);
                        GXLoadPosMtxImm(mtxa->mtxA, 0);
                        set_post_mult_color(1.0f, 1.0f, 1.0f, part->alpha);
                        avdisp_draw_model_culled_sort_never(lbl_14_bss_B4[part->model]);
                    }
                }
            }
        }
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void fn_14_215F8(struct BoatParticleParam *param)
{
    Vec dir = {1.0f, 0.0f, 0.0f};
    float mag;
    s32 idx;
    s32 i;
    s32 j;
    struct BoatParticleGroup *group;
    struct BoatParticle *part;

    if (lbl_14_bss_639C < 200 && lbl_14_bss_639E < 200 && param->life > 0 && param->count > 0
        && param->models != NULL && param->numModels != 0) {
        i = 0;
        group = lbl_14_bss_6364;
        for (; i < 200; i++, group++) {
            if (group->flags == 0) {
                lbl_14_bss_639C++;
                group->flags = param->flags | 1;
                group->life = param->life;
                group->timer = param->life + param->fadeTime;
                group->accel = param->accel;
                group->drag = param->drag;
                group->count = 0;
                mtxa_from_identity();
                j = 0;
                part = lbl_14_bss_6360;
                for (; j < 200; j++, part++) {
                    if (group->count >= param->count) {
                        return;
                    }
                    if (part->model == -1) {
                        lbl_14_bss_639E++;
                        group->ids[group->count] = j;
                        group->count++;
                        idx = (s16)((rand() & 0x7FFF) % param->numModels);
                        part->model = param->models[(s16)idx];
                        part->alpha = 0.0f;
                        part->scale = param->scale;
                        part->rot = (rand() & 0x7FFF) * 2;
                        mtxa_rotate_y((rand() & 0x7FFF) * 2);
                        mtxa_rotate_x((rand() & 0x7FFF) * 2);
                        mtxa_tf_vec(&dir, &dir);
                        mag = param->posScale * ((float)rand() / 32767.0f);
                        part->pos.x = param->pos->x + mag * dir.x;
                        part->pos.y = param->pos->y + mag * dir.y;
                        part->pos.z = param->pos->z + mag * dir.z;
                        mag = param->velScale * ((float)rand() / 32767.0f);
                        part->vel.x = param->vel->x + mag * dir.x;
                        part->vel.y = param->vel->y + mag * dir.y;
                        part->vel.z = param->vel->z + mag * dir.z;
                    }
                }
                return;
            }
        }
    }
}

void fn_14_2190C(struct BoatParticleParam *param)
{
    *param = lbl_14_bss_6368;
}

void fn_14_21980(void)
{
    s32 i;

    for (i = 0; i < 200; i++) {
        lbl_14_bss_6360[i].model = -1;
    }
    lbl_14_bss_639E = 0;
}

void fn_14_219C4(void)
{
    s32 i;

    for (i = 0; i < 200; i++) {
        lbl_14_bss_6364[i].flags = 0;
    }
    lbl_14_bss_639C = 0;
}

