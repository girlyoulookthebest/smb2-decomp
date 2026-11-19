/* fight_2_CA84: functions from 0xCA84 (included by src/fight/fight_2.c in address order) */

void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *out);
extern void *g_some_buffer_ptr;

void fn_12_F130(struct GMAModel *model, int count, struct FightRibbonJoint *joints);
void fn_12_F4C0(struct GMAModel *model, int count, struct FightRibbonJoint *joints);
void fn_12_F824(struct GMAModel *model, int count, struct FightRibbonJoint *joints);
void fn_12_FC2C(struct GMAModel *model, int count, struct FightRibbonJoint *joints);

/* Glove glow effect: init. Starts at the glove's model centre with a random spin. */
void fn_12_CA84(struct Effect *effect)
{
    struct FightGlove *glove = &lbl_12_bss_1ACA0.gloves[effect->playerId];
    struct Stobj *stobj = glove->stobj;

    effect->state = 0;
    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_translate(&stobj->model->boundSphereCenter);
    mathutil_mtxA_get_translate(&effect->pos);
    effect->scale.x = 0.5f * (1.0f + rand() / 32767.0f);
    effect->unk18 = 0.0f;
    effect->unk1C = 0.0f;
    effect->unk20 = 0.0f;
    effect->rotZ = rand() & 0x7FFF;
    effect->unk56 = 0;
}

/* Glove glow effect: tick. Grows while the glove charges, fades out when released. */
void fn_12_CBA8(struct Effect *effect)
{
    struct FightGlove *glove;
    float size;
    struct GMAModel *model;
    struct Stobj *stobj;
    float target;
    Vec pos;

    glove = &lbl_12_bss_1ACA0.gloves[effect->playerId];
    stobj = glove->stobj;
    size = stobj->model->boundSphereRadius * stobj->unk48;

    if (lbl_12_bss_1ACA0.players[glove->playerIdx].stunTimer != 0)
    {
        effect->timer++;
        return;
    }
    model = effect->model;
    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_translate(&stobj->model->boundSphereCenter);
    mathutil_mtxA_get_translate(&pos);
    switch (effect->state)
    {
    case 0:
    case 1:
        effect->unk56 += 0x80;
        target = 0.75f * (effect->colorFactor * (size / model->boundSphereRadius));
        effect->scale.x += 0.2f * (target - effect->scale.x);
        effect->unk18 += 0.125f * (1.0f - effect->unk18);
        effect->unk1C += 0.15f * (1.0f - effect->unk1C);
        effect->unk20 += 0.175f * (1.0f - effect->unk20);
        if (glove->flags & 0x20)
            effect->state = 2;
        else if (!(glove->flags & 0x10))
            effect->state = 4;
        break;
    case 2:
        effect->unk56 -= 0x100;
        target = 0.25f * effect->colorFactor * (size / model->boundSphereRadius);
        effect->scale.x += 0.3f * (target - effect->scale.x);
        if (glove->flags & 0x8)
            effect->state = 3;
        else if (!(glove->flags & 0x20))
            effect->state = 4;
        break;
    case 3:
        effect->state = 5;
    case 4:
        effect->state = 5;
    case 5:
        effect->scale.x -= 0.2f * (1.0 - effect->scale.x);
        effect->unk18 += 0.1f * (-0.1f - effect->unk18);
        effect->unk1C += 0.1f * (-0.1f - effect->unk1C);
        effect->unk20 += 0.1f * (-0.1f - effect->unk20);
        if (effect->scale.x < 0.0f || effect->unk18 < 0.0f || effect->unk1C < 0.0f || effect->unk20 < 0.0f)
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        break;
    }
    effect->unk56 = 0.96f * effect->unk56;
    effect->rotZ += effect->unk56;
    effect->pos = pos;
}

/* Glove glow effect: draw, pulled toward the camera so it shows in front of the glove. */
void fn_12_CF74(struct Effect *effect)
{
    struct FightGlove *glove = &lbl_12_bss_1ACA0.gloves[effect->playerId];
    struct Stobj *stobj = glove->stobj;
    struct GMAModel *model = effect->model;
    float size = stobj->model->boundSphereRadius * stobj->unk48;
    float t;
    Vec pos;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mathutil_mtxA_get_translate(&pos);
    if (pos.z < -(0.1f + (0.1f + size)))
    {
        t = (0.1f + (pos.z + size)) / pos.z;
        pos.x *= t;
        pos.y *= t;
        pos.z += 0.01f + size;
        mathutil_mtxA_set_translate(&pos);
        size = effect->scale.x * t;
        mtxa_scale_s(size);
        if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, size))
        {
            mtxa_rotate_z(effect->rotZ);
            set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_draw_model_unculled_sort_always(model);
            avdisp_set_z_mode(1, 3, 1);
            fade_color_base_default();
        }
    }
}

/* Glove glow effect: destroy (nothing to free). */
void fn_12_D11C(struct Effect *effect)
{
}

/* Glove spark effect: init. Like fn_12_CA84 but spins on all three axes. */
void fn_12_D120(struct Effect *effect)
{
    struct FightGlove *glove = &lbl_12_bss_1ACA0.gloves[effect->playerId];
    struct Stobj *stobj = glove->stobj;

    effect->state = 0;
    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_translate(&stobj->model->boundSphereCenter);
    mathutil_mtxA_get_translate(&effect->pos);
    effect->scale.x = 0.5f * (1.0f + rand() / 32767.0f);
    effect->unk18 = 0.0f;
    effect->unk1C = 0.0f;
    effect->unk20 = 0.0f;
    effect->rotX = rand() & 0x7FFF;
    effect->rotY = rand() & 0x7FFF;
    effect->rotZ = rand() & 0x7FFF;
    effect->unk52 = (rand() & 0xFFF) - 0x800;
    effect->unk54 = (rand() & 0xFFF) - 0x800;
    effect->unk56 = 0;
}

/* Glove spark effect: tick. Follows the glove while charging, flies off on release. */
void fn_12_D28C(struct Effect *effect)
{
    struct FightGlove *glove = &lbl_12_bss_1ACA0.gloves[effect->playerId];
    struct Stobj *stobj = glove->stobj;
    float size = stobj->model->boundSphereRadius * stobj->unk48;
    struct GMAModel *model;
    int follow;
    float t;
    Vec pos;

    if (lbl_12_bss_1ACA0.players[glove->playerIdx].stunTimer != 0)
    {
        effect->timer++;
        return;
    }
    model = effect->model;
    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_translate(&stobj->model->boundSphereCenter);
    mathutil_mtxA_get_translate(&pos);
    follow = TRUE;
    switch (effect->state)
    {
    case 0:
    case 1:
        effect->unk56 += 0x80;
        size = effect->colorFactor * (size / model->boundSphereRadius);
        effect->scale.x += 0.2f * (size - effect->scale.x);
        effect->unk18 += 0.125f * (1.0f - effect->unk18);
        effect->unk1C += 0.15f * (1.0f - effect->unk1C);
        effect->unk20 += 0.175f * (1.0f - effect->unk20);
        if (glove->flags & 0x20)
            effect->state = 2;
        else if (!(glove->flags & 0x10))
            effect->state = 4;
        break;
    case 2:
        effect->unk52 = 1.05f * effect->unk52;
        effect->unk54 = 1.05f * effect->unk54;
        effect->unk56 -= 0x100;
        size = 1.2f * (effect->colorFactor * (size / model->boundSphereRadius));
        effect->scale.x += 0.3f * (size - effect->scale.x);
        if (glove->flags & 0x8)
        {
            effect->state = 3;
            follow = FALSE;
            effect->u_otherTimer = 60.0f * (0.125f + 0.125f * (rand() / 32767.0f));
            t = 0.25f * (effect->colorFactor * (0.8f * (1.0f + rand() / 32767.0f)));
            effect->vel.x += effect->unk8C.x * t;
            effect->vel.y += effect->unk8C.y * t;
            effect->vel.z += effect->unk8C.z * t;
            effect->scale.y = size * (2.0f + rand() / 32767.0f);
        }
        else if (!(glove->flags & 0x20))
            effect->state = 4;
        break;
    case 3:
        follow = FALSE;
        effect->unk52 = 0.85f * effect->unk52;
        effect->unk54 = 0.85f * effect->unk54;
        effect->scale.x += 0.1f * (effect->scale.y - effect->scale.x);
        t = 1.0f / effect->u_otherTimer;
        effect->unk18 += -effect->unk18 * t;
        effect->unk1C += -effect->unk1C * t;
        effect->unk20 += -effect->unk20 * t;
        effect->u_otherTimer--;
        if (effect->u_otherTimer <= 0)
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        break;
    case 4:
        effect->state = 5;
    case 5:
        follow = FALSE;
        effect->scale.x -= 0.2f * (1.0 - effect->scale.x);
        effect->unk18 += 0.1f * (-0.1f - effect->unk18);
        effect->unk1C += 0.1f * (-0.1f - effect->unk1C);
        effect->unk20 += 0.1f * (-0.1f - effect->unk20);
        if (effect->scale.x < 0.0f || effect->unk18 < 0.0f || effect->unk1C < 0.0f || effect->unk20 < 0.0f)
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        break;
    }
    effect->unk56 = 0.96f * effect->unk56;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    if (follow)
    {
        effect->vel.x = pos.x - effect->pos.x;
        effect->vel.y = pos.y - effect->pos.y;
        effect->vel.z = pos.z - effect->pos.z;
        effect->pos = pos;
    }
    else
    {
        effect->vel.x *= 0.95f;
        effect->vel.y *= 0.95f;
        effect->vel.z *= 0.95f;
        effect->pos.x += effect->vel.x;
        effect->pos.y += effect->vel.y;
        effect->pos.z += effect->vel.z;
    }
}

/* Glove spark effect: draw (mirrored when it is behind the camera plane). */
void fn_12_D9C4(struct Effect *effect)
{
    struct FightGlove *glove = &lbl_12_bss_1ACA0.gloves[effect->playerId];
    struct Stobj *stobj = glove->stobj;
    struct GMAModel *model = effect->model;
    float size = stobj->model->boundSphereRadius * stobj->unk48;
    float t;
    Vec pos;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mathutil_mtxA_get_translate(&pos);
    if (pos.z < -(0.1f + (0.01f + size)))
    {
        t = (0.01f + (pos.z + size)) / pos.z;
        pos.x *= t;
        pos.y *= t;
        pos.z += 0.01f + size;
        mathutil_mtxA_set_translate(&pos);
        size = effect->scale.x * t;
        mtxa_scale_s(size);
        if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, size))
        {
            mtxa_rigid_inv_tf_tl(&pos);
            if (pos.z < 0.0f)
                mtxa_scale_xyz(-1.0f, 1.0f, 1.0f);
            set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_draw_model_unculled_sort_always(model);
            avdisp_set_z_mode(1, 3, 1);
            fade_color_base_default();
        }
    }
}

/* Glove spark effect: destroy (nothing to free). */
void fn_12_DBA0(struct Effect *effect)
{
}

/* Stage model debris effect: init (lifetime 60-90 frames). */
void fn_12_DBA4(struct Effect *effect)
{
    effect->timer = 60.0f * (1.0f + 0.5f * (rand() / 32767.0f));
    effect->colorFactor = 1.0f;
}

/* Stage model debris effect: tick. Falls and tumbles; the spin decays and it fades at the end. */
void fn_12_DC34(struct Effect *effect)
{
    Vec v;

    effect->vel.y += -0.016f;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_rigid_inv_tf_vec(&effect->vel, &v);
    v.x *= effect->unk98.x;
    v.y *= effect->unk98.y;
    v.z *= effect->unk98.z;
    mtxa_tf_vec(&v, &effect->vel);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk52 = 0.992f * effect->unk52;
    effect->unk54 = 0.992f * effect->unk54;
    effect->unk56 = 0.992f * effect->unk56;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    if (effect->timer < 30)
        effect->colorFactor *= (float)effect->timer / (float)(effect->timer + 1);
}

/* Stage model debris effect: draw. effect->model holds a stage model reference here. */
void fn_12_DE7C(struct Effect *effect)
{
    struct GMAModel *model;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    model = g_some_draw_func2((struct StageModelRef *)effect->model);
    if (model != NULL)
    {
        mtxa_translate_neg(&model->boundSphereCenter);
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
        {
            if (effect->colorFactor < 1.0f)
                avdisp_set_alpha(effect->colorFactor);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(model);
        }
    }
}

/* Stage model debris effect: destroy (nothing to free). */
void fn_12_DF38(struct Effect *effect)
{
}

/* Smoke puff effect: init. Faces along its velocity with a random spin, common model 48. */
void fn_12_DF3C(struct Effect *effect)
{
    float len;

    effect->timer = 60.0f * (0.1f + 0.2f * (rand() / 32767.0f));
    vec_to_euler_xy(&effect->vel, &effect->rotX, &effect->rotY);
    effect->rotZ = rand() & 0x7FFF;
    effect->unk56 = 1536.0f * (1.0f + rand() / 32767.0f);
    if (rand() & 1)
        effect->unk56 = -effect->unk56;
    effect->model = init_common_gma->modelEntries[48].model;
    effect->scale.x = 1.5f / effect->model->boundSphereRadius;
    len = mathutil_vec_len(&effect->vel);
    effect->unk98.x = effect->scale.x * (len * (0.2f + 0.04f * (rand() / 32767.0f)));
    effect->vel.x *= 0.5f;
    effect->vel.y *= 0.5f;
    effect->vel.z *= 0.5f;
    effect->colorFactor = 1.0f;
}

/* Smoke puff effect: tick. Slows down, keeps growing and fades over the last 30 frames. */
void fn_12_E12C(struct Effect *effect)
{
    if (lbl_12_bss_1ACA0.players[effect->playerId].stunTimer != 0)
    {
        effect->timer++;
        return;
    }
    effect->vel.x *= 0.94f;
    effect->vel.y *= 0.94f;
    effect->vel.z *= 0.94f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk56 = 1.01f * effect->unk56;
    effect->rotZ += effect->unk56;
    effect->unk98.x *= 0.97f;
    effect->scale.x += effect->unk98.x;
    if (effect->timer < 30)
        effect->colorFactor *= (float)effect->timer / (float)(effect->timer + 1);
}

/* Smoke puff effect: draw, turned to face the camera. */
void fn_12_E298(struct Effect *effect)
{
    struct GMAModel *model;
    Vec dir;
    Vec center;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    model = effect->model;
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, effect->scale.x))
    {
        mtxa_tf_point(&model->boundSphereCenter, &center);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
        if (mathutil_vec_dot_prod(&dir, &center) < 0.0f)
            mtxa_rotate_y(-0x8000);
        if (effect->colorFactor < 1.0f)
            avdisp_set_alpha(effect->colorFactor);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_always(model);
        avdisp_set_z_mode(1, 3, 1);
    }
}

/* Smoke puff effect: destroy (nothing to free). */
void fn_12_E3BC(struct Effect *effect)
{
}

/* Glove trail effect: init (lifetime 15-60 frames). */
void fn_12_E3C0(struct Effect *effect)
{
    effect->timer = 60.0f * (0.25f + 0.75f * (rand() / 32767.0f));
    effect->colorFactor = 1.0f;
    effect->state = 0;
}

/* Glove trail effect: tick. Falls, bounces off the stage below it and fades out at the end. */
void fn_12_E458(struct Effect *effect)
{
    struct RaycastHit hit;
    Vec d;
    float dot;
    float radius;

    effect->vel.y -= 0.008f;
    effect->vel.x *= effect->unk98.x;
    effect->vel.y *= effect->unk98.y;
    effect->vel.z *= effect->unk98.z;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk52 = 0.98f * effect->unk52;
    effect->unk54 = 0.98f * effect->unk54;
    effect->unk56 = 0.98f * effect->unk56;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    switch (effect->state)
    {
    case 0:
        if ((effect->timer & 3) == 0)
        {
            if ((s32)raycast_stage_down(&effect->pos, &hit, &effect->unk80) == 0)
                effect->state = 2;
            else
            {
                effect->state = 1;
                effect->unk64 = hit.pos;
                effect->unk70 = hit.normal;
            }
        }
        break;
    case 1:
        d.x = effect->pos.x - effect->unk64.x;
        d.y = effect->pos.y - effect->unk64.y;
        d.z = effect->pos.z - effect->unk64.z;
        dot = mathutil_vec_dot_prod(&d, &effect->unk70);
        radius = effect->model->boundSphereRadius * effect->scale.x;
        if (dot < radius)
        {
            d = effect->unk70;
            effect->pos.x += d.x * (radius - dot);
            effect->pos.y += d.y * (radius - dot);
            effect->pos.z += d.z * (radius - dot);
            dot = mathutil_vec_dot_prod(&d, &effect->vel);
            if (dot < 0.0f)
            {
                dot *= -2.0f;
                effect->vel.x += dot * d.x;
                effect->vel.y += dot * d.y;
                effect->vel.z += dot * d.z;
                if (dot > 0.01f)
                {
                    effect->unk52 += dot * (s16)(65536.0f * (rand() / 32767.0f - 0.5f));
                    effect->unk54 += dot * (s16)(32768.0f * (rand() / 32767.0f - 0.5f));
                    effect->unk56 += dot * (s16)(131072.0f * (rand() / 32767.0f - 0.5f));
                }
            }
        }
        if ((effect->timer & 7) == 0)
        {
            if ((s32)raycast_stage_down(&effect->pos, &hit, &effect->unk80) == 0)
                effect->state = 2;
            else
            {
                effect->unk64 = hit.pos;
                effect->unk70 = hit.normal;
            }
        }
        break;
    }
    if (effect->timer < 7)
        effect->colorFactor *= (float)effect->timer / (float)(effect->timer + 1);
}

/* Glove trail effect: draw. */
void fn_12_EA70(struct Effect *effect)
{
    struct GMAModel *model;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_xyz(effect->scale.x, effect->scale.y, effect->scale.z);
    model = effect->model;
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, effect->scale.x))
    {
        if (effect->colorFactor < 1.0f)
        {
            avdisp_set_alpha(effect->colorFactor);
            avdisp_draw_model_unculled_sort_always(model);
        }
        else
        {
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(model);
        }
    }
}

/* Glove trail effect: destroy (nothing to free). */
void fn_12_EB34(struct Effect *effect)
{
}

/* Balloon effect: init. Points the balloon down and builds the three ribbon ropes. */
void fn_12_EB38(struct Effect *effect)
{
    struct FightBalloon *balloon = (struct FightBalloon *)effect->model;
    float unused = 1.0f;  /* dead; a third distinct literal makes MWCC use the pool base here */

    effect->unk8C.x = 0.0f;
    effect->unk8C.y = -1.0f;
    effect->unk8C.z = 0.0f;
    effect->unk98.x = 0.0f;
    effect->unk98.y = 0.0f;
    effect->unk98.z = 0.0f;
    fn_12_F130(balloon->ribbonModels[0], balloon->jointCounts[0], balloon->joints[0]);
    fn_12_F130(balloon->ribbonModels[1], balloon->jointCounts[1], balloon->joints[1]);
    fn_12_F130(balloon->ribbonModels[2], balloon->jointCounts[2], balloon->joints[2]);
}

/* Balloon effect: tick. Simulates the ribbons, lets the balls push the balloon around and
   keeps effect->unk8C (balloon up direction) following the top ribbon joint. */
void fn_12_EBC8(struct Effect *effect)
{
    struct FightBalloon *balloon = (struct FightBalloon *)effect->model;
    struct FightRibbonJoint *tip;
    int i;
    struct Ball *ball;
    float dx, dy, dz;
    float k;  /* gravity, then the push radius (one variable in the original) */
    Vec d;
    Vec down;
    Vec push;
    Vec tipPos;
    Vec prevDir;

    mtxa_from_mtx(itemgroups[1].transform);
    mtxa_translate(&balloon->pos);
    mtxa_rotate_y(balloon->rotY);
    mtxa_rotate_x(balloon->rotX);
    mtxa_rotate_z(balloon->rotZ);
    fn_12_F4C0(balloon->ribbonModels[0], balloon->jointCounts[0], balloon->joints[0]);
    fn_12_F824(balloon->ribbonModels[1], balloon->jointCounts[1], balloon->joints[1]);
    fn_12_F824(balloon->ribbonModels[2], balloon->jointCounts[2], balloon->joints[2]);
    mtxa_rigid_inv_tf_vec_xyz(0.0f, -1.0f, 0.0f, &down);
    tip = &balloon->joints[0][balloon->jointCounts[0] - 1];
    k = -0.016f;
    tip->vel.x += down.x * k;
    tip->vel.y += down.y * k;
    tip->vel.z += down.z * k;
    mtxa_tf_point(&tip->pos, &tipPos);
    for (ball = balls, i = 8; i > 0; i--, ball++)
    {
        if (ball->status == 2)
        {
            d.x = ball->pos.x - tipPos.x;
            d.y = ball->pos.y - tipPos.y;
            d.z = ball->pos.z - tipPos.z;
            if (vec_normalize_len(&d) < 5.0f + ball->currRadius)
            {
                k = 5.0f + ball->currRadius;
                push.x = tipPos.x - 0.1f * (k * d.x);
                push.y = tipPos.y - 0.1f * (k * d.y);
                push.z = tipPos.z - 0.1f * (k * d.z);
                mtxa_rigid_inv_tf_point(&push, &push);
                tip->vel.x += push.x - tip->pos.x;
                tip->vel.y += push.y - tip->pos.y;
                tip->vel.z += push.z - tip->pos.z;
                tip->pos = push;
            }
        }
    }
    mtxa_tf_point(&tip->pos, &effect->pos);
    mtxa_tf_vec(&tip->vel, &effect->vel);
    prevDir = effect->unk8C;
    dx = 0.1f * (tip->dir.x - effect->unk8C.x);
    dy = 0.1f * (tip->dir.y - effect->unk8C.y);
    dz = 0.1f * (tip->dir.z - effect->unk8C.z);
    effect->unk98.x += 0.02f * (dx - effect->unk98.x);
    effect->unk98.y += 0.02f * (dy - effect->unk98.y);
    effect->unk98.z += 0.02f * (dz - effect->unk98.z);
    effect->unk8C.x += effect->unk98.x;
    effect->unk8C.y += effect->unk98.y;
    effect->unk8C.z += effect->unk98.z;
    vec_normalize_len(&effect->unk8C);
    effect->unk98.x = effect->unk8C.x - prevDir.x;
    effect->unk98.y = effect->unk8C.y - prevDir.y;
    effect->unk98.z = effect->unk8C.z - prevDir.z;
}

/* Balloon effect: draw the balloon (tilted along effect->unk8C) and its three ribbons. */
void fn_12_EF50(struct Effect *effect)
{
    struct FightBalloon *balloon = (struct FightBalloon *)effect->model;
    struct GMAModel *model = balloon->balloonModel;
    Quat quat;
    Vec down;
    Mtx mtx;

    down.x = 0.0f;
    down.y = -1.0f;
    down.z = 0.0f;
    quat_from_dirs(&quat, &down, &effect->unk8C);
    mtxa_from_quat(&quat);
    mtxa_to_mtx(mtx);
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_mult_right(mtx);
    mtxa_translate_xyz(0.0f, 0.7f * model->boundSphereRadius - model->boundSphereCenter.y, 0.0f);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
    {
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_auto(model);
    }

    model = balloon->ribbonModels[0];
    mtxa_from_mtxb();
    mtxa_mult_right(itemgroups[1].transform);
    mtxa_translate(&balloon->pos);
    mtxa_rotate_y(balloon->rotY);
    mtxa_rotate_x(balloon->rotX);
    mtxa_rotate_z(balloon->rotZ);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
    {
        mtxa_push();
        fn_12_FC2C(model, balloon->jointCounts[0], balloon->joints[0]);
        mtxa_pop();
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_never(model);
    }
    model = balloon->ribbonModels[1];
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
    {
        mtxa_push();
        fn_12_FC2C(model, balloon->jointCounts[1], balloon->joints[1]);
        mtxa_pop();
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_auto(model);
    }
    model = balloon->ribbonModels[2];
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
    {
        mtxa_push();
        fn_12_FC2C(model, balloon->jointCounts[2], balloon->joints[2]);
        mtxa_pop();
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_auto(model);
    }
}

/* Balloon effect: destroy (nothing to free). */
void fn_12_F12C(struct Effect *effect)
{
}

/* Builds a ribbon rope from the bind pose of a skinned model: joint positions, random initial
   velocities, rest lengths/directions and a random twist that drifts along the rope. */
void fn_12_F130(struct GMAModel *model, int count, struct FightRibbonJoint *joints)
{
    s16 twist;
    s16 twistStep;
    int i;
    Mtx **ptr;
    Mtx *mtxBase;
    int n;
    Mtx **mtxPtrs;
    Mtx *mtx;
    struct FightRibbonJoint *joint;
    Vec d;

    n = count;
    mtxPtrs = g_some_buffer_ptr;
    mtxBase = (Mtx *)(mtxPtrs + n);
    for (ptr = mtxPtrs, mtx = mtxBase, i = n; i > 0; i--, ptr++, mtx++)
        *ptr = mtx;
    draw_poly(model, mtxPtrs);
    memset(joints, 0, count * sizeof(*joints));
    for (joint = joints, mtx = mtxBase, i = count; i > 0; i--, joint++, mtx++)
    {
        mtxa_from_mtx(*mtx);
        mathutil_mtxA_get_translate(&joint->pos);
        joint->vel.x = 0.2f * (RAND_FLOAT() - 0.5f);
        joint->vel.y = 0.2f * (RAND_FLOAT() - 0.5f);
        joint->vel.z = 0.2f * (RAND_FLOAT() - 0.5f);
    }
    joints[0].flags |= 1;
    joints[count - 1].flags |= 2;
    twistStep = (rand() & 0x1FFF) - 0x1000;
    twist = 0;
    for (i = count - 1, joint = &joints[1], mtx = mtxBase; i > 0; i--, joint++, mtx++)
    {
        if (joint->flags & 1)
            continue;
        d.x = joint[-1].pos.x - joint->pos.x;
        d.y = joint[-1].pos.y - joint->pos.y;
        d.z = joint[-1].pos.z - joint->pos.z;
        joint->length = vec_normalize_len(&d);
        joint->restDir = d;
        if (joint->length <= FLT_EPSILON)
        {
            joint->flags |= 1;
            continue;
        }
        joint->twist = twist;
        if (joint->flags & 2)
            joint->stiffness = 0.03f * (1.0f + RAND_FLOAT());
        else
        {
            joint->stiffness = 0.01f * (1.0f + RAND_FLOAT());
            d.x = joint[1].pos.x - joint->pos.x;
            d.y = joint[1].pos.y - joint->pos.y;
            d.z = joint[1].pos.z - joint->pos.z;
            joint->nextDir = d;
            joint->nextLen = vec_normalize_len(&joint->nextDir);
            joint->invNextLen = 1.0f / joint->nextLen;
        }
        twist += twistStep;
        twistStep = (rand() & 0x1FF) + twistStep - 0x100;
    }
}

/* Top ribbon physics: gravity, rope constraint to the previous joint and a gentle wind. */
void fn_12_F4C0(struct GMAModel *model, int count, struct FightRibbonJoint *joints)
{
    struct FightRibbonJoint *parent;
    int i;
    float windX;
    float windY;
    float windZ;
    float tx;
    float ty;
    float tz;
    float s;  /* gravity, then the joints length, then the velocity dot (one variable) */
    float k;
    Vec d;
    Vec down;

    mtxa_rigid_inv_tf_vec_xyz(0.0f, -1.0f, 0.0f, &down);
    windX = 0.04f * math_sin(g_font_char_var_3 << 8);
    windY = 0.01f * math_sin(g_font_char_var_3 << 3);
    windZ = 0.02f * (1.0f + math_sin(g_font_char_var_3 << 7));
    for (i = count; i > 0; i--, joints++)
    {
        if (joints->flags & 1)
            continue;
        parent = joints - 1;
        s = 0.001f;
        joints->vel.x += down.x * s;
        joints->vel.y += down.y * s;
        joints->vel.z += down.z * s;
        d.x = parent->pos.x - joints->pos.x;
        d.y = parent->pos.y - joints->pos.y;
        d.z = parent->pos.z - joints->pos.z;
        if (vec_normalize_len(&d) > joints->length)
        {
            s = joints->length;
            tx = parent->pos.x - d.x * s;
            ty = parent->pos.y - d.y * s;
            tz = parent->pos.z - d.z * s;
            joints->pos.x += 0.5f * (tx - joints->pos.x);
            joints->pos.y += 0.5f * (ty - joints->pos.y);
            joints->pos.z += 0.5f * (tz - joints->pos.z);
            s = mathutil_vec_dot_prod(&joints->vel, &d);
            joints->vel.x -= s * d.x;
            joints->vel.y -= s * d.y;
            joints->vel.z -= s * d.z;
            parent->vel.x += s * d.x;
            parent->vel.y += s * d.y;
            parent->vel.z += s * d.z;
        }
        k = joints->stiffness;
        joints->vel.x += k * (windX - joints->vel.x);
        joints->vel.y += k * (windY - joints->vel.y);
        joints->vel.z += k * (windZ - joints->vel.z);
        joints->pos.x += joints->vel.x;
        joints->pos.y += joints->vel.y;
        joints->pos.z += joints->vel.z;
        joints->dir = d;
    }
}

/* Hanging ribbon physics: like fn_12_F4C0 plus a spring back toward the rest shape. */
void fn_12_F824(struct GMAModel *model, int count, struct FightRibbonJoint *joints)
{
    struct FightRibbonJoint *joint = joints;
    struct FightRibbonJoint *parent;
    int i = count;
    float windX, windY, windZ;
    float tx, ty, tz;
    float len;
    float dot;
    float k;
    float g;
    Vec d;
    Vec down;

    mtxa_rigid_inv_tf_vec_xyz(0.0f, -1.0f, 0.0f, &down);
    windX = 0.04f * math_sin(g_font_char_var_3 << 8);
    windY = 0.01f * math_sin(g_font_char_var_3 << 3);
    windZ = 0.025f * (1.0f + math_sin(g_font_char_var_3 << 7));
    for (; i > 0; i--, joint++)
    {
        if (joint->flags & 1)
            continue;
        parent = joint - 1;
        d = joint->restDir;
        len = joint->length;
        tx = parent->pos.x - d.x * len;
        ty = parent->pos.y - d.y * len;
        tz = parent->pos.z - d.z * len;
        joint->vel.x += 0.004f * (tx - joint->pos.x);
        joint->vel.y += 0.004f * (ty - joint->pos.y);
        joint->vel.z += 0.004f * (tz - joint->pos.z);
        g = 0.001f;
        joint->vel.x += down.x * g;
        joint->vel.y += down.y * g;
        joint->vel.z += down.z * g;
        d.x = parent->pos.x - joint->pos.x;
        d.y = parent->pos.y - joint->pos.y;
        d.z = parent->pos.z - joint->pos.z;
        dot = vec_normalize_len(&d);
        len = joint->length;
        tx = parent->pos.x - d.x * len;
        ty = parent->pos.y - d.y * len;
        tz = parent->pos.z - d.z * len;
        if (dot > joint->length)
        {
            joint->pos.x += 0.5f * (tx - joint->pos.x);
            joint->pos.y += 0.5f * (ty - joint->pos.y);
            joint->pos.z += 0.5f * (tz - joint->pos.z);
            dot = mathutil_vec_dot_prod(&joint->vel, &d);
            joint->vel.x -= dot * d.x;
            joint->vel.y -= dot * d.y;
            joint->vel.z -= dot * d.z;
            parent->vel.x += dot * d.x;
            parent->vel.y += dot * d.y;
            parent->vel.z += dot * d.z;
        }
        k = joint->stiffness;
        joint->vel.x += k * (windX - joint->vel.x);
        joint->vel.y += k * (windY - joint->vel.y);
        joint->vel.z += k * (windZ - joint->vel.z);
        joint->pos.x += joint->vel.x;
        joint->pos.y += joint->vel.y;
        joint->pos.z += joint->vel.z;
        joint->dir = d;
    }
}

/* Ribbon draw: turns each joint of the rope into its skin matrix (pinned last joint keeps the
   bind pose, the others are rotated/stretched toward the next joint), then commits them. */
void fn_12_FC2C(struct GMAModel *model, int count, struct FightRibbonJoint *joints)
{
    int i;
    Mtx **mtxPtrs;
    Mtx *mtxBase;
    Mtx *mtx;
    struct FightRibbonJoint *joint;
    Mtx **ptr;
    int n;
    float len;
    Vec d;
    Quat quat;

    joint = joints;
    n = count;
    mtxPtrs = g_some_buffer_ptr;
    mtxBase = (Mtx *)(mtxPtrs + n);
    for (ptr = mtxPtrs, mtx = mtxBase, i = n; i > 0; i--, ptr++, mtx++)
        *ptr = mtx;
    draw_poly(model, mtxPtrs);
    for (mtx = mtxBase, i = count; i > 0; i--, joint++, mtx++)
    {
        if (joint->flags & 1)
            continue;
        if (joint->flags & 2)
        {
            mtxa_from_mtx(*mtx);
            mathutil_mtxA_set_translate(&joint->pos);
        }
        else
        {
            d.x = joint[1].pos.x - joint->pos.x;
            d.y = joint[1].pos.y - joint->pos.y;
            d.z = joint[1].pos.z - joint->pos.z;
            len = vec_normalize_len(&d);
            quat_from_dirs(&quat, &joint->nextDir, &d);
            mtxa_from_quat(&quat);
            mtxa_mult_right(*mtx);
            mathutil_mtxA_set_translate(&joint->pos);
            mtxa_rotate_x(joint->twist);
            if (len < joint->nextLen)
                mtxa_rotate_x(16384.0f * (len - joint->nextLen));
            mtxa_scale_xyz(len * joint->invNextLen, 1.0f, 1.0f);
        }
        mtxa_to_mtx(*mtx);
    }
    pointer_range_advance(model, mtxBase);
}

/* ---- ball trail (FightPlayer.trail), drawn through the ord table ---- */


struct OrdTblNode *fn_1_CCC18(float z);
void fn_800668FC(void);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetLineWidth_cached(u8 width, int texOffset);
void g_something_with_GX_vtx_desc(u32 attrs);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
void GXSetTevColor(int id, GXColor color);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
extern struct GMA *minigameGma;

/* Ord table node of a ball's trail draw */
struct FightTrailNode
{
    struct OrdTblNode node;
    struct Ball *ball;
};

/* One trail point as drawn, in view space (built in g_some_buffer_ptr by fn_12_FEBC) */
struct FightTrailVtx
{
    float size;
    Vec pos;
};

void fn_12_FEBC(struct OrdTblNode *node);
void fn_12_1022C(struct Ball *ball, int count, struct FightTrailVtx *vtxs);
void fn_12_10740(struct Ball *ball, int count, struct FightTrailVtx *vtxs);
void fn_12_10ACC(struct Ball *ball, int count, struct FightTrailVtx *vtxs);
void fn_12_10EE0(struct Ball *ball, int count, struct FightTrailVtx *vtxs);

/* Queues the trail draw of a ball (unless the BALL TAIL EFC debug setting is a multiple of 5) */
void fn_12_FE18(struct Ball *ball)
{
    struct OrdTblNode *list;
    struct FightTrailNode *node;

    if (lbl_1_bss_6FA94 % 5 != 0)
    {
        list = fn_1_CCC18(0.0f);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_12_FEBC;
            node->ball = ball;
            fn_1_CD418(list, &node->node);
        }
    }
}

/* Trail draw: walks the trail ring buffer from the newest point, interpolating between points by
   the frame phase (1/6 per frame), keeps the visible ones (in view space) and draws them in the
   style of the player's pad (CPU players use style 4). */
void fn_12_FEBC(struct OrdTblNode *node)
{
    Vec p;
    Vec pos;
    Vec prevPos;
    struct Ball *ball;
    struct FightPlayer *player;
    int idx;
    int left;
    struct FightTrail *trail;
    struct FightTrailPt *pt;
    int n;
    struct FightTrailVtx *out;
    float prevSize;
    float size;
    float u;
    float t;
    float s;
    int style;

    ball = ((struct FightTrailNode *)node)->ball;
    player = &lbl_12_bss_1ACA0.players[ball->playerId];
    t = (1.0f / 6.0f) * (frame_counter % 6);
    u = 1.0f - t;
    trail = &player->trail;
    prevPos = ball->pos;
    prevSize = 0.0f;
    left = trail->count;
    idx = trail->head;
    n = 0;
    out = g_some_buffer_ptr;
    mtxa_from_mtxb();
    if (idx > 0)
        pt = &trail->pts[idx - 1];
    else
        pt = &trail->pts[idx + 59];
    for (; left > 0; idx--, left--, pt--)
    {
        if (idx <= 0)
            pt = &trail->pts[idx + 59];
        pos = pt->pos;
        if (prevSize > 0.0f || pt->timer != 0)
        {
            size = (1.0f / 15.0f) * pt->timer;
            if (size < 0.0f)
                size = 0.0f;
            else if (size > 1.0f)
                size = 1.0f;
            p.x = u * pos.x + t * prevPos.x;
            p.y = u * pos.y + t * prevPos.y;
            p.z = u * pos.z + t * prevPos.z;
            s = u * size + t * prevSize;
            if (fn_1_233FC(&p, 0.2f * s))
            {
                out->size = s;
                mtxa_tf_point(&p, &out->pos);
                n++;
                out++;
            }
        }
        else
            size = 0.0f;
        prevPos = pos;
        prevSize = size;
    }
    if (n != 0)
    {
        if (player->flags & 1)
            style = 4;
        else
            style = player_pad_map[ball->playerId] + 1;
        switch (style)
        {
        case 2:
            fn_12_1022C(ball, n, g_some_buffer_ptr);
            break;
        case 1:
            fn_12_10740(ball, n, g_some_buffer_ptr);
            break;
        case 3:
            fn_12_10ACC(ball, n, g_some_buffer_ptr);
            break;
        case 4:
            fn_12_10EE0(ball, n, g_some_buffer_ptr);
            break;
        }
    }
}

/* Trail style 2: white-edged textured quads, then the same quads in the ball colour */
void fn_12_1022C(struct Ball *ball, int count, struct FightTrailVtx *vtxs)
{
    Vec pos;
    Vec color = lbl_1_data_2E418[ball->colorId];
    GXColor c;
    int i;
    struct FightTrailVtx *v;
    float r;
    float x0, y0, x1, y1;

    fn_800668FC();
    g_something_with_GX_vtx_desc(0x2200);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevSwapMode_cached(0, 0, 1);
    GXSetTexCoordGen2_cached(0, 1, 4, 60, 0, 125);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevColorIn_cached(0, 15, 15, 15, 2);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 4, 1, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    GXSetZMode_cached(1, 3, 0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetCullMode_cached(2);
    c.r = 0xFF;
    c.g = 0xFF;
    c.b = 0xFF;
    c.a = 0xC0;
    GXSetTevColor(1, c);
    GXLoadTexObj_cached(minigameGma->modelEntries[169].model->texObjs, 0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0x80, 0, count * 4);
    for (v = vtxs, i = count; i > 0; i--, v++)
    {
        pos = v->pos;
        r = 0.040000003f + 0.2f * v->size;
        x0 = pos.x + r;
        y0 = pos.y - r;
        x1 = pos.x - r;
        y1 = pos.y + r;
        GXPosition3f32(x0, y0, pos.z);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x1, y0, pos.z);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x1, y1, pos.z);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(x0, y1, pos.z);
        GXTexCoord2f32(0.0f, 1.0f);
    }
    color = lbl_1_data_2E418[ball->colorId];
    c.r = 255.0f * color.x;
    c.g = 255.0f * color.y;
    c.b = 255.0f * color.z;
    c.a = 0xC0;
    GXSetTevColor(1, c);
    GXBegin(0x80, 0, count * 4);
    for (v = vtxs, i = count; i > 0; i--, v++)
    {
        pos = v->pos;
        r = 0.2f * v->size;
        x0 = pos.x + r;
        y0 = pos.y - r;
        x1 = pos.x - r;
        y1 = pos.y + r;
        GXPosition3f32(x0, y0, pos.z);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x1, y0, pos.z);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x1, y1, pos.z);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(x0, y1, pos.z);
        GXTexCoord2f32(0.0f, 1.0f);
    }
}

/* Trail style 1: additive textured quads tinted with the (lightened) ball colour */
void fn_12_10740(struct Ball *ball, int count, struct FightTrailVtx *vtxs)
{
    Vec pos;
    Vec color = lbl_1_data_2E418[ball->colorId];
    GXColor c;
    int i;
    struct FightTrailVtx *v;
    float r;
    float x0, y0, x1, y1;

    fn_800668FC();
    g_something_with_GX_vtx_desc(0x2200);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevSwapMode_cached(0, 0, 1);
    GXSetTexCoordGen2_cached(0, 1, 4, 60, 0, 125);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevColorIn_cached(0, 15, 8, 2, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 4, 1, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    GXSetZMode_cached(1, 3, 0);
    GXSetBlendMode_cached(1, 1, 1, 0);
    GXSetCullMode_cached(2);
    c.r = 255.0f * (0.2f + 0.8f * color.x);
    c.g = 255.0f * (0.2f + 0.8f * color.y);
    c.b = 255.0f * (0.2f + 0.8f * color.z);
    c.a = 0xFF;
    GXSetTevColor(1, c);
    GXLoadTexObj_cached(minigameGma->modelEntries[169].model->texObjs, 0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0x80, 0, count * 4);
    for (v = vtxs, i = count; i > 0; i--, v++)
    {
        pos = v->pos;
        r = 0.28f * v->size;
        x0 = pos.x + r;
        y0 = pos.y - r;
        x1 = pos.x - r;
        y1 = pos.y + r;
        GXPosition3f32(x0, y0, pos.z);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x1, y0, pos.z);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x1, y1, pos.z);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(x0, y1, pos.z);
        GXTexCoord2f32(0.0f, 1.0f);
    }
}

/* Trail style 3: additive textured quads rotated by the point's position */
void fn_12_10ACC(struct Ball *ball, int count, struct FightTrailVtx *vtxs)
{
    Vec pos;
    Vec color = lbl_1_data_2E418[ball->colorId];
    GXColor c;
    Vec d;
    int i;
    struct FightTrailVtx *v;
    float r;
    float x0, y0, x1, y1, x2, y2, x3, y3;

    fn_800668FC();
    g_something_with_GX_vtx_desc(0x2200);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTexCoordGen2_cached(0, 1, 4, 60, 0, 125);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevColorIn_cached(0, 15, 12, 2, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 4, 1, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    GXSetZMode_cached(1, 3, 0);
    GXSetBlendMode_cached(1, 4, 1, 0);
    GXSetCullMode_cached(0);
    c.r = 255.0f * (0.2f + 0.8f * color.x);
    c.g = 255.0f * (0.2f + 0.8f * color.y);
    c.b = 255.0f * (0.2f + 0.8f * color.z);
    c.a = 0xFF;
    GXSetTevColor(1, c);
    GXLoadTexObj_cached(init_common_gma->modelEntries[38].model->texObjs, 0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0x80, 0, count * 4);
    for (i = count, v = vtxs; i > 0; i--, v++)
    {
        pos = v->pos;
        r = 0.6f * v->size;
        mtxa_from_rotate_z(8192.0f * (pos.x + pos.y + pos.z));
        mtxa_tf_vec_xyz(r, 0.0f, 0.0f, &d);
        x0 = pos.x + d.x;
        y0 = pos.y + d.y;
        x1 = pos.x - d.y;
        y1 = pos.y + d.x;
        x2 = pos.x - d.x;
        y2 = pos.y - d.y;
        x3 = pos.x + d.y;
        y3 = pos.y - d.x;
        GXPosition3f32(x0, y0, pos.z);
        GXTexCoord2f32(-2.0f, -3.0f);
        GXPosition3f32(x1, y1, pos.z);
        GXTexCoord2f32(4.0f, -3.0f);
        GXPosition3f32(x2, y2, pos.z);
        GXTexCoord2f32(4.0f, 3.0f);
        GXPosition3f32(x3, y3, pos.z);
        GXTexCoord2f32(-2.0f, 3.0f);
    }
}

/* Trail style 4 (CPU players): additive square outlines rotated by the point's position */
void fn_12_10EE0(struct Ball *ball, int count, struct FightTrailVtx *vtxs)
{
    Vec pos;
    Vec color = lbl_1_data_2E418[ball->colorId];
    GXColor c;
    Vec d;
    int i;
    struct FightTrailVtx *v;
    float r;
    float x0, y0, x1, y1, x2, y2, x3, y3;

    fn_800668FC();
    g_something_with_GX_vtx_desc(0x200);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevSwapMode_cached(0, 0, 1);
    GXSetTexCoordGen2_cached(0, 1, 4, 60, 0, 125);
    g_GXSetTevOrder_wrapper(0, 0, 0xFF, 0xFF);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTevColorIn_cached(0, 15, 15, 15, 2);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 1);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    GXSetZMode_cached(1, 3, 0);
    GXSetBlendMode_cached(1, 1, 1, 0);
    GXSetCullMode_cached(0);
    c.r = 255.0f * (0.2f + 0.8f * color.x);
    c.g = 255.0f * (0.2f + 0.8f * color.y);
    c.b = 255.0f * (0.2f + 0.8f * color.z);
    c.a = 0xFF;
    GXSetTevColor(1, c);
    GXSetLineWidth_cached(9, 0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0xA8, 0, count * 8);
    for (i = count, v = vtxs; i > 0; i--, v++)
    {
        pos = v->pos;
        r = 0.28f * v->size;
        mtxa_from_rotate_z(8192.0f * (pos.x + pos.y + pos.z));
        mtxa_tf_vec_xyz(r, 0.0f, 0.0f, &d);
        x0 = pos.x + d.x;
        y0 = pos.y + d.y;
        x1 = pos.x - d.y;
        y1 = pos.y + d.x;
        x2 = pos.x - d.x;
        y2 = pos.y - d.y;
        x3 = pos.x + d.y;
        y3 = pos.y - d.x;
        GXPosition3f32(x0, y0, pos.z);
        GXPosition3f32(x1, y1, pos.z);
        GXPosition3f32(x1, y1, pos.z);
        GXPosition3f32(x2, y2, pos.z);
        GXPosition3f32(x2, y2, pos.z);
        GXPosition3f32(x3, y3, pos.z);
        GXPosition3f32(x3, y3, pos.z);
        GXPosition3f32(x0, y0, pos.z);
    }
}
