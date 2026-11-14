#include "race2.h"

void fn_10_E008(struct Effect *effect);
void fn_10_E0F4(struct Effect *effect);
void fn_10_E160(struct Effect *effect);
void fn_10_E360(struct Effect *effect);
void fn_10_E364(struct Effect *effect);
void fn_10_E440(struct Effect *effect);
void fn_10_E4C4(struct Effect *effect);
void fn_10_E598(struct Effect *effect);
void fn_10_E59C(struct Effect *effect);
void fn_10_E5BC(struct Effect *effect);
void fn_10_E7CC(struct Effect *effect);
void fn_10_E8F0(struct Effect *effect);
void fn_10_E8F4(struct Effect *effect);
void fn_10_E9F4(struct Effect *effect);
void fn_10_EB1C(struct Effect *effect);
void fn_10_EB98(struct Effect *effect);
void fn_10_EB9C(struct Effect *effect);
void fn_10_EC6C(struct Effect *effect);
void fn_10_ED94(struct Effect *effect);
void fn_10_EE10(struct Effect *effect);
void fn_10_EE14(struct Effect *effect);
void fn_10_EE70(struct Effect *effect);
void fn_10_F034(struct Effect *effect);
void fn_10_F15C(struct Effect *effect);
void fn_10_F160(struct Effect *effect);
void fn_10_F16C(struct Effect *effect);
void fn_10_F214(struct Effect *effect);
void fn_10_F2BC(struct Race2DrawNode *arg0);
void fn_10_F460(struct Effect *effect);
void fn_10_F484(struct Race2DrawNode *arg0, s32 arg1);
void fn_10_F610(struct Race2DrawNode *arg0, s32 arg1);
void fn_10_F76C(struct Race2DrawNode *arg0, s32 arg1);
void fn_10_F8B4(struct Effect *effect);
void fn_10_F9DC(struct Effect *effect);
void fn_10_FD2C(struct Effect *effect);
void fn_10_10000(struct Effect *effect);
void fn_10_10004(struct Effect *effect);
void fn_10_1007C(struct Effect *effect);
void fn_10_10308(struct Effect *effect);
void fn_10_103A4(struct Effect *effect);
void fn_10_103A8(struct Effect *effect);
void fn_10_103B4(struct Effect *effect);
void fn_10_105C4(struct Effect *effect);
void fn_10_10720(struct Effect *effect);
void fn_10_10724(float arg2, Vec *arg0, Vec *arg1);

void fn_10_E008(struct Effect *effect)
{
    struct Effect sp20;
    s16 tr31;

    effect->u_otherTimer = -1;
    effect->timer = 0x64;
    memset(&sp20, 0, 0xB0);
    sp20.type = 1;
    sp20.playerId = effect->playerId;
    sp20.scale = (Vec){ 0.5f, 0.5f, 0.5f };
    sp20.unk8C = (Vec){ 4.0f, 0.2f, -0.01f };
    tr31 = sp20.type;
    sp20.type += 0x27;
    spawn_effect(&sp20);
    sp20.type = tr31;
}

void fn_10_E0F4(struct Effect *effect)
{
    effect->u_otherTimer++;
    if ((u32) effect->u_otherTimer < 0x27) {
        effect->model = minigameGma->modelEntries[lbl_10_data_F48[effect->u_otherTimer]].model;
        return;
    }
    g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
}

void fn_10_E160(struct Effect *effect)
{
    Vec sp14;
    Vec sp8;
    float tf31;
    float tf1_2;
    float tf30;
    float f0;
    float f1;
    struct Ball *tr4;
    struct Camera *tr29;

    tr29 = g_current_camera;
    tf31 = 240.0f * g_current_camera->sub28.viewportH;
    tf30 = (1.0f / tf31);
    f0 = tf30;
    tf1_2 = math_sqrt(mathutil_vec_sq_distance(&g_current_camera->eye, &effect->pos));
    tf1_2 = tr29->sub28.view.tanHalfFov * tf1_2;
    if ((tf31 / tf1_2) < 60.0f) {
        tf30 = 60.0f * tf1_2 * f0;
    } else {
        tf30 = 1.0f;
    }
    if (effect->playerId == current_ball->playerId) {
        sp14 = balls[effect->playerId].pos;
        sp14.y -= 1.0f;
    } else {
        sp14 = effect->pos;
    }
    mtxa_from_mtxb_translate( &sp14);
    mtxa_sq_from_identity();
    mtxa_translate_xyz(0.0f, 0.0f, 1.0f);
    PSVECScale(&effect->scale, &sp8, tf30);
    mtxa_scale(&sp8);
    if (sp8.y > sp8.z) {
        f0 = sp8.y;
    } else {
        f0 = sp8.z;
    }
    if (sp8.x > f0) {
        f1 = sp8.x;
    } else if (sp8.y > sp8.z) {
        f1 = sp8.y;
    } else {
        f1 = sp8.z;
    }
    avdisp_set_scale_factor(f1);
    avdisp_draw_model_culled_sort_always(effect->model);
}

void fn_10_E360(struct Effect *effect)
{

}

void fn_10_E364(struct Effect *effect)
{
    struct RaycastHit hit;

    if ((s32)raycast_stage_down(&balls[effect->playerId].pos, &hit, NULL) == 0) {
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        return;
    }
    effect->pos = hit.pos;
    effect->pos.y += 0.01;
    vec_to_euler_xy(&hit.normal, &effect->rotX, &effect->rotY);
    effect->rotX += 0x8000;
    effect->colorFactor = 1.0f;
    effect->timer = 0x3E8;
}

void fn_10_E440(struct Effect *effect)
{
    effect->scale.x += effect->unk8C.y;
    if (effect->scale.x > effect->unk8C.x) {
        effect->scale.x = effect->unk8C.x;
        effect->unk8C.y *= -1.5f;
    }
    if (effect->scale.x < 0.0f) {
        effect->scale.x = 0.0f;
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
    }
}

void fn_10_E4C4(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    avdisp_set_z_mode(1, 3, 0);
    mtxa_scale_s(effect->scale.x);
    avdisp_set_scale_factor(effect->scale.x);
    set_post_mult_color(1.0f, 1.0f, 0.3f, 1.0f);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_always(init_common_gma->modelEntries[0x5e].model);
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
}

void fn_10_E598(struct Effect *effect)
{

}

void fn_10_E59C(struct Effect *effect)
{
    effect->cameras = 1 << effect->playerId;
    effect->timer = 0x14;
}

void fn_10_E5BC(struct Effect *effect)
{
    struct RaycastHit hit;
    Vec sp2C;
    Vec sp20;
    float f1;
    struct Ball *tr29;

    tr29 = &balls[effect->playerId];
    PSVECScale(&tr29->vel, &sp2C, -0.5f);
    PSVECAdd(&sp2C, &tr29->pos, &sp2C);
    if ((s32)raycast_stage_down(&sp2C, &hit, NULL) != 0) {
        effect->pos = hit.pos;
        vec_to_euler_xy(&hit.normal, &effect->rotX, &effect->rotY);
        effect->rotX += 0x8000;
        mtxa_from_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rigid_inv_tf_vec(&tr29->vel, &sp20);
        effect->rotZ = math_atan2(sp20.y, sp20.x);
        if ((s32) effect->timer > 0xA) {
            f1 = 1.0f - ((float) (effect->timer - 0xA) / 10.0f);
        } else if ((s32) effect->timer > 5) {
            f1 = 1.0f;
        } else {
            f1 = effect->timer / 5.0f;
        }
        f1 *= 1.5f;
        effect->scale = (Vec){ 3.0f * f1, f1, f1 };
        return;
    }
    effect->pos = tr29->pos;
    effect->scale = (Vec){ 0.0f, 0.0f, 0.0f };
}

void fn_10_E7CC(struct Effect *effect)
{
    float f0;
    float f1;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_translate_xyz(0.0f, 0.1f, 0.0f);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    avdisp_set_z_mode(1, 3, 0);
    mtxa_scale(&effect->scale);
    if (effect->scale.y > effect->scale.z) {
        f0 = effect->scale.y;
    } else {
        f0 = effect->scale.z;
    }
    if (effect->scale.x > f0) {
        f1 = effect->scale.x;
    } else if (effect->scale.y > effect->scale.z) {
        f1 = effect->scale.y;
    } else {
        f1 = effect->scale.z;
    }
    avdisp_set_scale_factor(f1);
    set_post_mult_color(1.0f, 1.0f, 0.3f, 1.0f);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_always(init_common_gma->modelEntries[0x5e].model);
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
}

void fn_10_E8F0(struct Effect *effect)
{

}

void fn_10_E8F4(struct Effect *effect)
{
    effect->timer = 0x1E;
    effect->unk8C = (Vec){ 0.0f, 0.0f, 0.0f };
    effect->unk98 = (Vec){ 0.0f, 0.25f, 0.0f };
    effect->scale = (Vec){ 0.0f, 0.0f, 0.0f };
    effect->rotX = 0x4000;
    effect->rotY = 0;
    effect->rotZ = 0;
    effect->unk52 = 0;
    effect->unk54 = 0;
    effect->unk56 = 0;
    effect->colorFactor = 1.0f;
    effect->model = lbl_10_bss_58[2];
}

void fn_10_E9F4(struct Effect *effect)
{
    float tf0;
    struct Ball *tr29;

    tr29 = &balls[effect->playerId];
    PSVECAdd(&effect->unk8C, &effect->unk98, &effect->unk8C);
    PSVECAdd(&tr29->pos, &effect->unk8C, &effect->pos);
    effect->pos.y += tr29->currRadius;
    effect->scale.z += 0.2;
    if (effect->scale.z > 4.0) {
        effect->scale.z = 4.0f;
    }
    tf0 = effect->scale.z;
    effect->scale.y = tf0;
    effect->scale.x = tf0;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    if ((s32) effect->timer <= 0xA) {
        effect->colorFactor = (0.1 * (float) effect->timer);
    }
}

void fn_10_EB1C(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale(&effect->scale);
    avdisp_set_scale_factor(effect->scale.x);
    avdisp_set_alpha(effect->colorFactor);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_always(effect->model);
}

void fn_10_EB98(struct Effect *effect)
{

}

void fn_10_EB9C(struct Effect *effect)
{
    effect->timer = 0x1E;
    effect->unk98.y = -0.2f;
    effect->unk8C.y = -effect->unk98.y * (float) effect->timer;
    effect->scale = (Vec){ 0.0f, 0.0f, 0.0f };
    effect->rotX = -0x4000;
    effect->rotY = 0;
    effect->rotZ = 0;
    effect->unk52 = 0;
    effect->unk54 = 0;
    effect->unk56 = 0;
    effect->colorFactor = 1.0f;
    effect->model = lbl_10_bss_58[2];
}

void fn_10_EC6C(struct Effect *effect)
{
    float tf0;
    struct Ball *tr29;

    tr29 = &balls[effect->playerId];
    PSVECAdd(&effect->unk8C, &effect->unk98, &effect->unk8C);
    PSVECAdd(&tr29->pos, &effect->unk8C, &effect->pos);
    effect->pos.y += tr29->currRadius;
    effect->scale.z += 0.2;
    if (effect->scale.z > 4.0) {
        effect->scale.z = 4.0f;
    }
    tf0 = effect->scale.z;
    effect->scale.y = tf0;
    effect->scale.x = tf0;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    if ((s32) effect->timer <= 5) {
        effect->colorFactor = (0.2 * (float) effect->timer);
    }
}

void fn_10_ED94(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale(&effect->scale);
    avdisp_set_scale_factor(effect->scale.x);
    avdisp_set_alpha(effect->colorFactor);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_always(effect->model);
}

void fn_10_EE10(struct Effect *effect)
{

}

void fn_10_EE14(struct Effect *effect)
{
    struct RacePlayer *p = &lbl_10_bss_1230[effect->playerId];

    effect->flags |= 0x10000000;
    effect->timer = p->unk304[7];
    effect->u_otherTimer = 0;
    effect->colorFactor = 0.0f;
    effect->scale.x = 5.0f;
    effect->scale.y = effect->scale.x;
    effect->scale.z = 1.0f;
}

void fn_10_EE70(struct Effect *effect)
{
    struct RacePlayer *p = &lbl_10_bss_1230[effect->playerId];

    effect->timer = p->unk304[7];
    if (effect->timer < (0.3 * lbl_80198B34)) {
        effect->scale.x += (1.4 / (0.3 * lbl_80198B34));
        if (effect->scale.x > 2.5) {
            effect->scale.x = 2.5f;
        }
        effect->colorFactor -= (1.0 / (0.3 * lbl_80198B34));
        if (effect->colorFactor < 0.0) {
            effect->colorFactor = 0.0f;
        }
    } else {
        effect->scale.x -= (3.9 / (0.2 * lbl_80198B34));
        if (effect->scale.x < 1.1) {
            effect->scale.x = 1.1f;
        }
        effect->colorFactor += (1.0 / (0.4 * lbl_80198B34));
        if (effect->colorFactor > 1.0) {
            effect->colorFactor = 1.0f;
        }
    }
    effect->scale.y = effect->scale.x;
    effect->model = lbl_10_bss_58[0x2C + effect->u_otherTimer];
    effect->u_otherTimer++;
    if ((s32) effect->u_otherTimer > 8) {
        effect->u_otherTimer = 0;
    }
}

void fn_10_F034(struct Effect *effect)
{
    float f0;
    float f0_2;
    float tf3;
    struct Ball *tr30;

    tr30 = &balls[effect->playerId];
    mtxa_from_mtxb_translate(&tr30->pos);
    mtxa_sq_from_identity();
    mtxa_translate_xyz(0.0f, 0.0f, 0.1f);
    mtxa_rotate_z(g_font_char_var_3 << 0xB);
    mtxa_scale(&effect->scale);
    mtxa_scale_s(tr30->modelScale);
    tf3 = tr30->modelScale;
    if (effect->scale.y > effect->scale.z) {
        f0 = effect->scale.y;
    } else {
        f0 = effect->scale.z;
    }
    if (effect->scale.x > f0) {
        f0_2 = effect->scale.x;
    } else if (effect->scale.y > effect->scale.z) {
        f0_2 = effect->scale.y;
    } else {
        f0_2 = effect->scale.z;
    }
    avdisp_set_scale_factor(tf3 * f0_2);
    avdisp_set_alpha(effect->colorFactor);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_never(effect->model);
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
}

void fn_10_F15C(struct Effect *effect)
{

}

void fn_10_F160(struct Effect *effect)
{
    effect->timer = 0x1E;
}

void fn_10_F16C(struct Effect *effect)
{
    struct RacePlayer *p = &lbl_10_bss_1230[effect->playerId];
    struct Race2EffectLink *tr5;

    tr5 = &lbl_10_bss_1D8[effect->poolIndex];
    effect->colorFactor = effect->timer / 30.0f;
    if (((u32) tr5->slot1 != 0) && ((u32) (p->unk2C2 & tr5->slot1) == 0)) {
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
    }
}

void fn_10_F214(struct Effect *effect)
{
    struct Ball *tr31;
    struct OrdTblNode *tr30;
    struct Race2DrawNode *tr3;

    tr31 = &balls[effect->playerId];
    tr30 = ord_tbl_get_entry_for_pos(&tr31->pos);
    tr3 = ord_tbl_alloc_node(sizeof(*tr3));
    if (tr3 != NULL) {
        tr3->func = fn_10_F2BC;
        tr3->lightGroup = get_active_light_group();
        tr3->unkC = tr31->playerId;
        tr3->unkE = effect->poolIndex;
        fn_1_CD418(tr30, (struct OrdTblNode *)tr3);
    }
}

void fn_10_F2BC(struct Race2DrawNode *arg0)
{
    struct Effect *e = &effects[arg0->unkE];
    struct Race2EffectLink *tr30;

    tr30 = &lbl_10_bss_1D8[arg0->unkE];
    if (e->colorFactor > 0.5) {
        if ((u32) (tr30->slot0 & 8) != 0) {
            fn_10_F610(arg0, 0);
        } else if ((u32) (tr30->slot0 & 4) != 0) {
            fn_10_F484(arg0, 0);
        } else {
            fn_10_F76C(arg0, 0);
        }
        if ((u32) (tr30->slot1 & 8) != 0) {
            fn_10_F610(arg0, 1);
            return;
        }
        if ((u32) (tr30->slot1 & 4) != 0) {
            fn_10_F484(arg0, 1);
            return;
        }
        fn_10_F76C(arg0, 1);
        return;
    }
    if ((u32) (tr30->slot1 & 8) != 0) {
        fn_10_F610(arg0, 1);
    } else if ((u32) (tr30->slot1 & 4) != 0) {
        fn_10_F484(arg0, 1);
    } else {
        fn_10_F76C(arg0, 1);
    }
    if ((u32) (tr30->slot0 & 8) != 0) {
        fn_10_F610(arg0, 0);
        return;
    }
    if ((u32) (tr30->slot0 & 4) != 0) {
        fn_10_F484(arg0, 0);
        return;
    }
    fn_10_F76C(arg0, 0);
}

void fn_10_F460(struct Effect *effect)
{
    struct RacePlayer *tr3;

    tr3 = &lbl_10_bss_1230[effect->playerId];
    tr3->flags &= 0xFFFDFFFF;
}

void fn_10_F484(struct Race2DrawNode *arg0, s32 arg1)
{
    Mtx sp8;
    float f31;
    float f30;
    struct Ball *tr31;
    struct Effect *tr7;
    struct RacePlayer *tr30;

    tr31 = &balls[arg0->unkC];
    tr7 = &effects[arg0->unkE];
    tr30 = &lbl_10_bss_1230[arg0->unkC];
    if (g_poolInfo.effectPool.statusList[arg0->unkE] != 3) {
        if (arg1 != 0) {
            f31 = (tr31->modelScale + (2.0 * tr7->colorFactor));
            f30 = (1.0 - tr7->colorFactor);
        } else {
            f31 = (tr31->modelScale + (2.0 * (1.0 - tr7->colorFactor)));
            f30 = tr7->colorFactor;
        }
        load_light_group_cached(arg0->lightGroup);
        mtxa_from_quat(&tr30->unk2D4);
        mtxa_to_mtx(sp8);
        mtxa_from_mtxb_translate(&tr31->pos);
        mtxa_mult_right(sp8);
        mtxa_scale_s(f31);
        avdisp_set_scale_factor(f31);
        avdisp_set_alpha(f30);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(minigameGma->modelEntries[0x14].model);
    }
}

void fn_10_F610(struct Race2DrawNode *arg0, s32 arg1)
{
    float f31;
    float f30;
    struct Ball *tr31;
    struct Effect *tr6;

    tr31 = &balls[arg0->unkC];
    tr6 = &effects[arg0->unkE];
    if (g_poolInfo.effectPool.statusList[arg0->unkE] != 3) {
        if (arg1 != 0) {
            f31 = (tr31->modelScale + (2.0 * tr6->colorFactor));
            f30 = (1.0 - tr6->colorFactor);
        } else {
            f31 = (tr31->modelScale + (2.0 * (1.0 - tr6->colorFactor)));
            f30 = tr6->colorFactor;
        }
        load_light_group_cached(arg0->lightGroup);
        mtxa_from_mtxb();
        mtxa_mult_right(tr31->unk30);
        mtxa_scale_s(f31);
        avdisp_set_scale_factor(f31);
        avdisp_set_alpha(f30);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(minigameGma->modelEntries[0x15].model);
    }
}

void fn_10_F76C(struct Race2DrawNode *arg0, s32 arg1)
{
    float f31;
    float f30;
    struct Ball *tr30;
    struct Effect *tr5;

    tr30 = &balls[arg0->unkC];
    tr5 = &effects[arg0->unkE];
    if (arg1 != 0) {
        f31 = (tr30->modelScale + (2.0 * tr5->colorFactor));
        f30 = (1.0 - tr5->colorFactor);
    } else {
        f31 = (tr30->modelScale + (2.0 * (1.0 - tr5->colorFactor)));
        f30 = tr5->colorFactor;
    }
    if (g_poolInfo.effectPool.statusList[arg0->unkE] != 3) {
        load_light_group_cached(arg0->lightGroup);
        mtxa_from_mtxb();
        mtxa_mult_right(tr30->unk30);
        mtxa_scale_s(f31);
        avdisp_set_scale_factor(f31);
        set_post_mult_color(1.0f, 1.0f, 1.0f, f30);
        fn_1_4D8A0(mtxa->mtxA, tr30->colorId, tr30->unk179);
        fade_color_base_default();
    }
}

void fn_10_F8B4(struct Effect *effect)
{
    effect->flags |= 2;
    effect->state = 0;
    effect->timer = ((0.1 * lbl_80198B34) + (0.3 * lbl_80198B34 * ((float) rand() / 32767.0f)));
    effect->u_otherTimer = 0;
    effect->scale.x = 1.0f;
    if ((0.0 == effect->unk18) && (0.0 == effect->unk1C) && (0.0 == effect->unk20)) {
        effect->unk18 = 0.9f;
        effect->unk1C = 0.8f;
        effect->unk20 = 0.4f;
    }
    effect->cameras = 1 << effect->playerId;
    effect->u_otherTimer = 0;
}

void fn_10_F9DC(struct Effect *effect)
{
    struct World *tw;
    struct RaycastHit hit;
    Vec sp14;
    Vec sp8;
    float tf0;
    float tf0_2;
    float tf2;
    float tf31;

    tw = current_world_info;
    fn_10_10724(0.008f, &tw->unk10, &effect->vel);
    PSVECScale(&effect->vel, &effect->vel, 0.992f);
    PSVECAdd(&effect->pos, &effect->vel, &effect->pos);
    if ((s32) (effect->timer & 0xF) == 0) {
        effect->state = 0;
        if (((s32)raycast_stage_down(&effect->pos, &hit, &effect->unk80) != 0) && (hit.pos.y > (effect->pos.y - 0.5))) {
            effect->state = 1;
            effect->unk64 = hit.pos;
            effect->unk70 = hit.normal;
        }
    }
    if ((s32) effect->timer < 0xA) {
        effect->unk18 *= 0.96;
        effect->unk1C *= 0.87;
        effect->unk20 *= 0.86;
        effect->scale.x = ((float) effect->timer / 24.0);
    }
    switch (effect->state) {                              /* irregular */
    case 0:
        if (effect->colorFactor > 0.0) {
            effect->colorFactor -= 0.125;
            if (effect->colorFactor < 0.0) {
                effect->colorFactor = 0.0f;
            }
        }
        break;
    case 1:
        sp14 = effect->unk70;
        PSVECSubtract(&effect->pos, &effect->unk64, &sp8);
        tf0 = mathutil_vec_dot_prod(&sp14, &sp8);
        tf2 = effect->colorFactor;
        effect->colorFactor = (tf2 + (0.25 * (tf0 - effect->colorFactor)));
        PSVECScale(&sp14, &sp8, tf0 - 0.02);
        PSVECSubtract(&effect->pos, &sp8, &effect->unk8C);
        vec_to_euler_xy(&sp14, &effect->unkA4, &effect->unkA6);
        effect->unkA4 += 0x8000;
        if (tf0 < 0.025) {
            tf0 = 0.025 - tf0;
            fn_10_10724(tf0, &sp14, &effect->pos);
            tf0 = mathutil_vec_dot_prod(&effect->vel, &sp14);
            if (tf0 < 0.0) {
                tf0 *= -1.0;
                fn_10_10724(tf0, &sp14, &effect->vel);
                PSVECSubtract(&effect->unk80, &effect->vel, &sp8);
                fn_10_10724(0.03f, &sp8, &effect->vel);
                fn_10_10724(0.9 * tf0, &sp14, &effect->vel);
            }
        }
        break;
    }
}

struct Race2LineVtx {
    Vec pos;
    GXColor color;
};

void fn_10_FD2C(struct Effect *effect)
{
    struct Race2LineVtx sp30;
    struct Race2LineVtx sp20;
    Vec sp14;
    Vec sp8;
    float tf1_2;
    float f1;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[3]);
    mtxa_tf_point(&effect->prevPos, &sp14);
    mtxa_from_mtxb();
    if ((s32) effect->u_otherTimer == 0) {
        effect->u_otherTimer = 1;
        return;
    }
    mtxa_tf_point(&effect->pos, &sp20.pos);
    sp30.pos.x = (1.5 * sp14.x) - (0.5 * sp20.pos.x);
    sp30.pos.y = (1.5 * sp14.y) - (0.5 * sp20.pos.y);
    sp30.pos.z = (1.5 * sp14.z) - (0.5 * sp20.pos.z);
    mtxa_from_identity();
    tf1_2 = 0.5 * math_sqrt(mathutil_vec_sq_distance(&sp20.pos, &sp30.pos));
    sp8.x = 0.5 * (sp20.pos.x + sp30.pos.y);
    sp8.y = 0.5 * (sp20.pos.y + sp30.pos.y);
    sp8.z = 0.5 * (sp20.pos.z + sp30.pos.z);
    if (g_is_sphere_visible(&sp8, tf1_2) != 0) {
        sp20.color.r = (255.0 * effect->unk18);
        sp20.color.g = (255.0 * effect->unk1C);
        sp20.color.b = (255.0 * effect->unk20);
        sp20.color.a = 0xFF;
        sp30.color.r = (96.0 * effect->unk18);
        sp30.color.g = (88.0 * effect->unk1C);
        sp30.color.b = (8.0 * effect->unk20);
        sp30.color.a = 0xFF;
        if (sp20.pos.z < -3.0) {
            f1 = 1.5f;
        } else if (sp20.pos.z > 0.0) {
            f1 = 4.5f;
        } else {
            f1 = 1.5 + (3.0 + sp20.pos.z);
        }
        fn_1_E2FD0(12.0 * f1);
        fn_1_E2FE0(1, 1, 1, 0);
        fn_1_E349C(&sp20, &sp30);
    }
}

void fn_10_10000(struct Effect *effect)
{

}

void fn_10_10004(struct Effect *effect)
{
    struct RacePlayer *p = &lbl_10_bss_1230[effect->playerId];

    effect->flags |= 0x10000000;
    effect->state = 0;
    effect->u_otherTimer = (1.0 + (0.3 * lbl_80198B34));
    effect->scale.x = 0.0f;
    effect->timer = p->unk304[0];
}

void fn_10_1007C(struct Effect *effect)
{
    Vec sp8;
    struct Ball *tr30;
    struct RacePlayer *p;

    tr30 = &balls[effect->playerId];
    p = &lbl_10_bss_1230[effect->playerId];
    if ((s32) effect->u_otherTimer > 0) {
        effect->u_otherTimer--;
    }
    effect->timer = p->unk304[0];
    switch (effect->state) {                              /* irregular */
    case 0:
        effect->scale.x += (2.0 / (0.3 * lbl_80198B34));
        if (effect->scale.x > 2.0) {
            effect->scale.x = 2.0f;
        }
        if ((s32) effect->u_otherTimer <= 0) {
            effect->state = 1;
        }
        break;
    case 1:
        if (effect->timer > (0.5 * lbl_80198B34)) {
            if (effect->scale.x > 1.0) {
                effect->scale.x -= (1.0 / (0.1 * lbl_80198B34));
                if (effect->scale.x < 1.0) {
                    effect->scale.x = 1.0f;
                }
            }
        } else {
            effect->scale.x -= (1.0 / (0.5 * lbl_80198B34));
        }
        break;
    }
    if (mathutil_vec_sq_len(&tr30->vel) > 0.00002143347050754458) {
        mtxa_from_rotate_y((s16) (math_atan2(tr30->vel.x, tr30->vel.z) + 0x8000));
    } else {
        mtxa_from_quat(&tr30->ape->rot);
    }
    mtxa_tf_vec_xyz(0.0f, 0.0f, (float) (0.2 + tr30->currRadius), &sp8);
    PSVECAdd(&tr30->pos, &sp8, &effect->pos);
    effect->pos.y -= tr30->currRadius;
    effect->model = lbl_10_bss_58[0xB + ((g_font_char_var_3 + (tr30->playerId * 5)) & 0x1F)];
}

void fn_10_10308(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_scale_xyz(effect->scale.x, effect->scale.x, 1.0f);
    avdisp_set_scale_factor(effect->scale.x > 1.0 ? effect->scale.x : 1.0);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_never(effect->model);
}

void fn_10_103A4(struct Effect *effect)
{

}

void fn_10_103A8(struct Effect *effect)
{
    effect->timer = 0x14;
}

void fn_10_103B4(struct Effect *effect)
{
    struct RaycastHit hit;
    Vec sp2C;
    Vec sp20;
    float f1;
    struct Ball *tr29;

    tr29 = &balls[effect->playerId];
    PSVECScale(&tr29->vel, &sp2C, -0.5f);
    PSVECAdd(&sp2C, &tr29->pos, &sp2C);
    if ((s32)raycast_stage_down(&sp2C, &hit, NULL) != 0) {
        effect->pos = hit.pos;
        vec_to_euler_xy(&hit.normal, &effect->rotX, &effect->rotY);
        effect->rotX += 0x8000;
        mtxa_from_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rigid_inv_tf_vec(&tr29->vel, &sp20);
        effect->rotZ = math_atan2(sp20.y, sp20.x);
        if ((s32) effect->timer > 0xA) {
            f1 = 1.0f - ((float) (effect->timer - 0xA) / 10.0f);
        } else if ((s32) effect->timer > 5) {
            f1 = 1.0f;
        } else {
            f1 = effect->timer / 5.0f;
        }
        f1 *= 1.5f;
        effect->scale = (Vec){ 3.0f * f1, f1, f1 };
        return;
    }
    effect->pos = tr29->pos;
    effect->scale = (Vec){ 0.0f, 0.0f, 0.0f };
}

void fn_10_105C4(struct Effect *effect)
{
    float f0;
    float f1;

    if ((0.5 != g_current_camera->sub28.viewportW) || (0.5 != g_current_camera->sub28.viewportH)) {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_translate_xyz(0.0f, 0.1f, 0.0f);
        mtxa_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        avdisp_set_z_mode(1, 3, 0);
        mtxa_scale(&effect->scale);
        if (effect->scale.y > effect->scale.z) {
            f0 = effect->scale.y;
        } else {
            f0 = effect->scale.z;
        }
        if (effect->scale.x > f0) {
            f1 = effect->scale.x;
        } else if (effect->scale.y > effect->scale.z) {
            f1 = effect->scale.y;
        } else {
            f1 = effect->scale.z;
        }
        avdisp_set_scale_factor(f1);
        set_post_mult_color(1.0f, 1.0f, 0.3f, 1.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_always(init_common_gma->modelEntries[0x5e].model);
        fade_color_base_default();
        avdisp_set_z_mode(1, 3, 1);
    }
}

void fn_10_10720(struct Effect *effect)
{

}

void fn_10_10724(float arg2, Vec *arg0, Vec *arg1)
{
    Vec sp8;

    PSVECScale(arg0, &sp8, arg2);
    PSVECAdd(&sp8, arg1, arg1);
}


s16 lbl_10_data_F48[] = {
    0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x39, 0x4C,
    0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
    0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C,
    0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64,
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x00,
};

