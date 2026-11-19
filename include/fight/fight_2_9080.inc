/* fight_2_9080: functions from 0x9080 (included by src/fight/fight_2.c in address order) */
/* Fight effects (effect func overrides, table lbl_12_data_390 in fight_1.c). */

void fn_12_9178(struct Effect *effect);
void fn_12_9624(struct Effect *effect);
void fn_12_9B48(struct Effect *effect);
void fn_12_9C80(struct Effect *effect);
void fn_12_FE18(struct Ball *ball);

/* Power-up spark ring: init */
void fn_12_9080(struct Effect *effect)
{
    effect->model = init_common_gma->modelEntries[37].model;
    effect->state = 0;
    effect->scale.x = 0.0f;
    effect->rotX = rand() & 0x7FFF;
    effect->rotY = rand() & 0x7FFF;
    effect->rotZ = rand() & 0x7FFF;
    effect->unk52 = (rand() & 0xFFF) - 0x800;
    effect->unk54 = (rand() & 0xFFF) - 0x800;
    effect->unk56 = (rand() & 0xFFF) - 0x800;
}

/* Spark ring: tick */
void fn_12_9144(struct Effect *effect)
{
    if (effect->state < 2)
        fn_12_9178(effect);
    else
        fn_12_9624(effect);
}

/* Spark ring tick, states 0-1: fly to the ball leaving a trail of sparks */
void fn_12_9178(struct Effect *effect)
{
    struct Ball *ball = &balls[effect->playerId];
    float t;
    struct Effect spark;
    GXColor color;

    switch (effect->state)
    {
    case 0:
        effect->state = 1;
        /* fall through */
    case 1:
        t = 1.0f / (float)(effect->u_otherTimer + 1);
        effect->vel.x = t * (ball->pos.x - effect->pos.x);
        effect->vel.z = t * (ball->pos.z - effect->pos.z);
        t = 1.0f / (1.0f + 0.375f * (float)effect->u_otherTimer);
        effect->vel.y = t * (ball->pos.y - effect->pos.y);
        effect->u_otherTimer--;
        if (effect->u_otherTimer < 0)
        {
            effect->state = 2;
            vec_to_euler_xy(&effect->vel, &effect->rotX, &effect->rotY);
            effect->rotX += 0x4000;
            effect->rotZ = rand() & 0x7FFF;
            effect->unk52 = (rand() & 0xFF) - 0x80;
            effect->unk54 = 0x200;
            effect->unk56 = (rand() & 0xFF) - 0x80;
            effect->unkA4 = rand() & 0x7FFF;
            effect->unkA6 = rand() & 0x7FFF;
            effect->unkA8 = rand() & 0x7FFF;
            effect->colorFactor = 2.0f;
            effect->model = init_common_gma->modelEntries[37].model;
            effect->unk8C.x = ball->currRadius;
            effect->unk98.x = 0.5f * (1.0f + 0.25f * ((float)rand() / 32767.0f));
            return;
        }
        break;
    }
    memset(&spark, 0, sizeof(spark));
    spark.type = 0x2D;
    spark.pos = effect->pos;
    mtxa_from_rotate_y(rand() & 0x7FFF);
    mtxa_rotate_x(rand() & 0x7FFF);
    mtxa_tf_vec_xyz(0.0f, 0.0f, 0.05f * ((float)rand() / 32767.0f), &spark.vel);
    spark.vel.x += 0.25f * effect->vel.x;
    spark.vel.y += 0.25f * effect->vel.y;
    spark.vel.z += 0.25f * effect->vel.z;
    spark.scale.x = effect->scale.x * effect->model->boundSphereRadius;
    spark.scale.x *= 1.5f * (0.5f + (float)rand() / 32767.0f);
    color = lbl_12_data_A218[ball->colorId];
    spark.unk18 = 0.002610966f * (128.0f + (float)color.r);
    spark.unk1C = 0.002610966f * (128.0f + (float)color.g);
    spark.unk20 = 0.002610966f * (128.0f + (float)color.b);
    spawn_effect(&spark);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->rotY += effect->unk54;
    effect->rotX += effect->unk52;
    effect->rotZ += effect->unk56;
    t = 2.0f * (ball->currRadius / effect->model->boundSphereRadius);
    effect->scale.x += 0.3f * (t - effect->scale.x);
}

/* Spark ring tick, states 2-5: ring around the ball, then fade out */
void fn_12_9624(struct Effect *effect)
{
    struct Ball *ball;
    float t;

    switch (effect->state)
    {
    case 2:
        effect->state = 3;
        effect->u_otherTimer = 7;
        /* fall through */
    case 3:
        ball = &balls[effect->playerId];
        effect->vel.x = 0.2f * (ball->pos.x - effect->pos.x);
        effect->vel.y = 0.2f * (ball->pos.y - effect->pos.y);
        effect->vel.z = 0.2f * (ball->pos.z - effect->pos.z);
        effect->colorFactor += 0.2f * (6.0f - effect->colorFactor);
        effect->unk98.x += -0.032f;
        effect->unk8C.x += effect->unk98.x;
        effect->unk54++;
        effect->u_otherTimer--;
        if (effect->u_otherTimer < 0 || !(ball->flags & 0x2000000000ULL))
            effect->state = 4;
        break;
    case 4:
        ball = &balls[effect->playerId];
        effect->vel.x = 0.2f * (ball->pos.x - effect->pos.x);
        effect->vel.y = 0.2f * (ball->pos.y - effect->pos.y);
        effect->vel.z = 0.2f * (ball->pos.z - effect->pos.z);
        effect->colorFactor += 0.1f * (1.0f - effect->colorFactor);
        effect->unk98.x *= 0.96f;
        effect->unk98.x += 0.1f * (0.2f * (1.0f - effect->unk8C.x) - effect->unk98.x);
        effect->unk8C.x += effect->unk98.x;
        effect->unk54++;
        if (!(ball->flags & 0x2000000000ULL))
            effect->state = 5;
        break;
    case 5:
        effect->vel.y += -0.008f;
        effect->vel.x *= 0.98f;
        effect->vel.y *= 0.98f;
        effect->vel.z *= 0.98f;
        effect->unk52 -= effect->unk52 >> 3;
        effect->unk56 -= effect->unk56 >> 3;
        effect->colorFactor += 0.05f * (-0.05f - effect->colorFactor);
        if (effect->colorFactor <= 0.0f)
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        effect->unk98.x *= 0.9f;
        effect->unk8C.x += effect->unk98.x;
        break;
    }
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    if (effect->unk8C.x < 1.0f)
        t = 1.0f;
    else
        t = 1.0f / (1.0f + 0.25f * (effect->unk8C.x - 1.0f));
    effect->rotY += t * (effect->unk54 + (float)(int)(256.0f * effect->vel.y));
    effect->rotX += effect->unk52 * t;
    effect->rotZ += effect->unk56 * t;
}

/* Spark ring: disp */
void fn_12_9B14(struct Effect *effect)
{
    if (effect->state < 2)
        fn_12_9B48(effect);
    else
        fn_12_9C80(effect);
}

/* Spark ring disp, states 0-1: the flying spark and its glow */
void fn_12_9B48(struct Effect *effect)
{
    float scale = effect->scale.x;
    struct GMAModel *model = effect->model;
    Vec pos;
    float t;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(scale);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_set_scale_factor(scale);
    avdisp_draw_model_culled_sort_auto(model);
    mathutil_mtxA_get_translate(&pos);
    t = -pos.z;
    t = (t - model->boundSphereRadius * scale) / t;
    pos.x *= t;
    pos.y *= t;
    pos.z *= t;
    scale *= 1.5f * (model->boundSphereRadius * t);
    mtxa_from_translate(&pos);
    mtxa_scale_s(scale);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_set_scale_factor(scale);
    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[94].model);
}

/* Spark ring disp, states 2-5: 12 sparks in a ring */
void fn_12_9C80(struct Effect *effect)
{
    int i;
    struct GMAModel *model;
    s32 angle;
    s32 rotY;
    s32 rotX;
    s32 rotZ;
    float scale;
    float radius;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    model = effect->model;
    angle = 0;
    rotX = effect->unkA4 + effect->rotX;
    rotY = effect->unkA6 + effect->rotY * 4;
    rotZ = effect->unkA8 + effect->rotZ;
    radius = effect->unk8C.x;
    for (i = 12; i > 0; i--, angle += 0x1555)
    {
        mtxa_push();
        mtxa_rotate_y(angle);
        mtxa_translate_xyz(0.0f, 0.0f, radius);
        if (i & 1)
            scale = 0.0076923077f * effect->colorFactor;
        else
            scale = 0.015384615f * effect->colorFactor;
        mtxa_scale_s(scale);
        if (!g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, scale))
        {
            mtxa_pop();
            continue;
        }
        mtxa_rotate_y((angle << 2) + rotY);
        mtxa_rotate_x(rotX);
        mtxa_rotate_z(angle + rotZ);
        avdisp_set_scale_factor(scale);
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_auto(model);
        mtxa_pop();
    }
}

/* Spark ring: dest */
void fn_12_9DEC(struct Effect *effect) {}

/* Ring debris piece (bouncing chip): init */
void fn_12_9DF0(struct Effect *effect)
{
    struct RaycastHit hit;

    effect->cameras = 0xFFFF;
    effect->timer = 60.0f * (0.25f + 1.25f * RAND_FLOAT());
    effect->colorFactor = effect->model->boundSphereRadius;
    effect->scale.x = 1.0f;
    vec_to_euler_xy(&effect->model->boundSphereCenter, &effect->unkA4, &effect->unkA6);
    if ((int)raycast_stage_down(&effect->pos, &hit, &effect->unk80) == 0)
    {
        effect->state = 0;
        return;
    }
    effect->state = 1;
    effect->unk64 = hit.pos;
    effect->unk70 = hit.normal;
}

/* Debris piece: tick (spin, gravity, bounce on the stage below) */
void fn_12_9F08(struct Effect *effect)
{
    struct RaycastHit hit;
    Vec vel;
    Vec normal;
    float t;
    float d;
    float r;
    float u;

    effect->unk52 += (s16)(384.0f * effect->vel.y);
    effect->unk54 += (s16)(768.0f * effect->vel.z);
    effect->unk56 += (s16)(768.0f * effect->vel.x);
    t = 1.0f - 0.02f * effect->colorFactor;
    effect->unk52 *= t;
    effect->unk54 *= t;
    effect->unk56 *= t;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    t = 1.0f - 0.025f * effect->colorFactor;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_rotate_y(effect->unkA6);
    mtxa_rotate_x(effect->unkA4);
    vel = effect->vel;
    mtxa_rigid_inv_tf_vec(&vel, &vel);
    vel.x *= t;
    vel.y *= t;
    vel.z *= 0.85f * t;
    mtxa_tf_vec(&vel, &vel);
    effect->vel = vel;
    effect->vel.y += -0.008f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    if ((effect->timer & 3) == 0)
    {
        if ((int)raycast_stage_down(&effect->pos, &hit, &effect->unk80) == 0)
        {
            effect->state = 0;
        }
        else
        {
            effect->state = 1;
            effect->unk64 = hit.pos;
            effect->unk70 = hit.normal;
        }
    }
    if (effect->state == 1)
    {
        vel.x = effect->pos.x - effect->unk64.x;
        vel.y = effect->pos.y - effect->unk64.y;
        vel.z = effect->pos.z - effect->unk64.z;
        normal = effect->unk70;
        r = mathutil_vec_dot_prod(&normal, &vel);
        d = effect->colorFactor * effect->scale.x;
        if (r < d)
        {
            d -= r;
            effect->pos.x += d * normal.x;
            effect->pos.y += d * normal.y;
            effect->pos.z += d * normal.z;
            effect->rotX -= effect->rotX >> 5;
            effect->rotY -= effect->rotY >> 5;
            effect->rotZ -= effect->rotZ >> 5;
            vel = effect->vel;
            d = mathutil_vec_dot_prod(&normal, &vel);
            if (d < 0.0f)
            {
                d *= -1.0f;
                vel.x += d * normal.x;
                vel.y += d * normal.y;
                vel.z += d * normal.z;
                vel.x += 0.05f * (effect->unk80.x - vel.x);
                vel.y += 0.05f * (effect->unk80.y - vel.y);
                vel.z += 0.05f * (effect->unk80.z - vel.z);
                d *= 0.9f;
                vel.x += d * normal.x;
                vel.y += d * normal.y;
                vel.z += d * normal.z;
                effect->vel = vel;
            }
        }
    }
    if (effect->timer < 30)
    {
        u = (float)effect->timer / (float)(effect->timer + 1);
        effect->scale.x *= u;
    }
}

/* Debris piece: disp (reflection model below the water line) */
void fn_12_A4C8(struct Effect *effect)
{
    struct GMAModel *model = effect->model;

    if (g_some_render_flag & 4)
    {
        if (effect->pos.y < -model->boundSphereRadius)
            return;
        model = lbl_12_bss_0[effect->unkA8];
    }
    mtxa_from_mtxb_translate_xyz(effect->pos.x, effect->pos.y, effect->pos.z);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    mtxa_translate_neg(&model->boundSphereCenter);
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, 1.0f))
    {
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_set_scale_factor(effect->scale.x);
        avdisp_draw_model_unculled_sort_auto(model);
    }
}

/* Debris piece: dest */
void fn_12_A5AC(struct Effect *effect) {}

/* Screen-space flash (projected onto the near plane of camera 0): init */
void fn_12_A5B0(struct Effect *effect)
{
    struct Camera *camera;
    float t;

    effect->timer = 60;
    camera = &cameras[0];
    effect->cameras = 1;
    mtxa_from_mtx(camera->unk14C);
    mtxa_tf_point(&effect->pos, &effect->pos);
    t = -0.95f / effect->pos.z;
    effect->pos.x *= t;
    effect->pos.y *= t;
    effect->pos.z = -0.95f;
    effect->scale.x *= t;
    effect->unk8C.z = effect->rotZ;
    effect->unk98.z = 4096.0f;
    if (g_font_char_var_3 & 1)
        effect->unk98.z = -effect->unk98.z;
    effect->colorFactor = 1.5f;
}

/* Screen-space flash: tick (follows the ball, spins, fades) */
void fn_12_A6A8(struct Effect *effect)
{
    struct Ball *ball = &balls[effect->playerId];
    struct Camera *camera = &cameras[0];
    Vec pos;
    float t;
    float radius;
    float size;
    float scale;
    float up;
    float x;
    float y;
    float z;

    mtxa_from_mtx(camera->unk14C);
    mtxa_tf_point(&ball->pos, &pos);
    t = -0.95f / pos.z;
    pos.x *= t;
    pos.y *= t;
    pos.z = -0.95f;
    radius = ball->currRadius * t;
    size = 0.19f * camera->sub28.view.tanHalfFov;
    scale = size / effect->model->boundSphereRadius;
    up = 1.25f * size;
    if (effect->timer < 15.0f)
    {
        x = pos.x;
        y = pos.y;
        z = pos.z;
        effect->pos.x += 0.5f * (x - effect->pos.x);
        effect->pos.y += 0.1f * (y - effect->pos.y);
        effect->pos.z += 0.5f * (z - effect->pos.z);
        effect->unk98.z -= 512.0f;
        effect->unk8C.z += effect->unk98.z;
        t = (float)effect->timer / (float)(effect->timer + 1);
        effect->scale.x *= t;
        effect->colorFactor += 0.1f * (1.5f - effect->colorFactor);
    }
    else if (effect->timer < 45.0f)
    {
        x = pos.x;
        y = up + (pos.y + radius);
        z = pos.z;
        effect->pos.x += 0.25f * (x - effect->pos.x);
        effect->pos.y += 0.1f * (y - effect->pos.y);
        effect->pos.z += 0.25f * (z - effect->pos.z);
        effect->unk98.z *= 0.8f;
        effect->unk98.z += -1024.0f * math_sin(effect->unk8C.z);
        effect->unk8C.z += effect->unk98.z;
        effect->scale.x += 0.1f * (scale - effect->scale.x);
        effect->colorFactor += 0.05f * (-0.05f - effect->colorFactor);
    }
    else
    {
        x = pos.x;
        y = up + (pos.y + radius);
        z = pos.z;
        effect->pos.x += 0.1f * (x - effect->pos.x);
        effect->pos.y += 0.1f * (y - effect->pos.y);
        effect->pos.z += 0.1f * (z - effect->pos.z);
        effect->unk8C.z += effect->unk98.z;
        effect->scale.x += 0.025f * (scale - effect->scale.x);
        effect->colorFactor += 0.1f * (-0.05f - effect->colorFactor);
    }
}

/* Screen-space flash: disp */
void fn_12_AA7C(struct Effect *effect)
{
    float a;
    float r;
    float g;
    float b;

    if (g_some_render_flag & 4)
        return;
    mtxa_from_translate(&effect->pos);
    mtxa_rotate_z(effect->unk8C.z);
    mtxa_scale_s(effect->scale.x);
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_set_scale_factor(effect->scale.x);
    a = effect->colorFactor;
    if (a > 0.0f)
    {
        r = effect->unk18 * a;
        g = effect->unk1C * a;
        b = effect->unk20 * a;
        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;
        g_stores_doubles2(r, g, b, 0.0f);
    }
    avdisp_draw_model_culled_sort_auto(effect->model);
    if (a > 0.0f)
        g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
}

/* Screen-space flash: dest */
void fn_12_ABEC(struct Effect *effect) {}

/* Ball ground marker: init (does nothing) */
void fn_12_ABF0(struct Effect *effect) {}

/* Ball ground marker: tick (does nothing) */
void fn_12_ABF4(struct Effect *effect) {}

/* Ball ground marker: disp (glow under the ball when the player flag 8 is set) */
void fn_12_ABF8(struct Effect *effect)
{
    struct Ball *ball = &balls[effect->playerId];
    struct FightPlayer *player = &lbl_12_bss_1ACA0.players[effect->playerId];
    float radius;
    float t;
    Vec pos;

    if (player->unk6 & 8)
    {
        radius = ball->currRadius;
        mtxa_from_mtxb_translate(&ball->pos);
        mathutil_mtxA_get_translate(&pos);
        if (pos.z < -radius)
        {
            t = (pos.z + radius) / pos.z;
            pos.x *= t;
            pos.y *= t;
            pos.z += radius;
            mtxa_from_translate(&pos);
            mtxa_scale_s(1.5f);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[94].model);
            avdisp_set_z_mode(1, 3, 1);
        }
    }
    fn_12_FE18(ball);
}

/* Ball ground marker: dest */
void fn_12_AD4C(struct Effect *effect) {}

/* Sparkle: init */
void fn_12_AD50(struct Effect *effect)
{
    effect->timer = 30.0f * (0.1f + RAND_FLOAT());
    effect->unkA8 = rand() & 0x7FFF;
}

/* Sparkle: tick (drift, fade to white, shrink at the end) */
void fn_12_ADE0(struct Effect *effect)
{
    float t;

    effect->vel.x *= 0.98f;
    effect->vel.y *= 0.98f;
    effect->vel.z *= 0.98f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk18 += 0.1f * (1.0f - effect->unk18);
    effect->unk1C += 0.1f * (1.0f - effect->unk1C);
    effect->unk20 += 0.1f * (1.0f - effect->unk20);
    if (effect->timer < 15.0f)
    {
        t = (float)effect->timer / (float)(effect->timer + 1);
        effect->scale.x *= t;
    }
}

/* Sparkle: disp (billboard, half of them skipped in the reflection pass) */
void fn_12_AF34(struct Effect *effect)
{
    if ((g_some_render_flag & 4) && (effect->pos.y < 0.0f || (effect->poolIndex & 1)))
        return;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_rotate_z(effect->unkA8 + (s32)(6144.0f * (mtxa->mtxA[0][3] + mtxa->mtxA[1][3])));
    mtxa_scale_s(effect->scale.x);
    set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
    avdisp_set_scale_factor(effect->scale.x);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[38].model);
    avdisp_set_z_mode(1, 3, 1);
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
}

/* Sparkle: dest */
void fn_12_B080(struct Effect *effect) {}

/* Speed lines / dash streak: init */
void fn_12_B084(struct Effect *effect)
{
    float len;

    effect->timer = 60.0f * (0.2f + 0.125f * RAND_FLOAT());
    effect->model = init_common_gma->modelEntries[95].model;
    len = 0.5f + math_sqrt(fabs(20.0f * effect->colorFactor));
    effect->scale.x = 0.125f * len;
    effect->scale.y = len;
    switch (world_theme.bgId)
    {
    case 8:
        effect->unk18 = 0.3f;
        break;
    default:
        effect->unk18 = 1.0f;
        break;
    }
    effect->cameras = 1 << effect->playerId;
    vec_to_euler_xy(&effect->unk8C, &effect->rotX, &effect->rotY);
    effect->rotX += 0x8000;
}

/* Speed streak: tick (frozen while the player's unk12 is set) */
void fn_12_B1B4(struct Effect *effect)
{
    int frozen = lbl_12_bss_1ACA0.players[effect->playerId].stunTimer;
    float speed;
    float t;

    if (frozen == 0)
        speed = 1.0f;
    else
    {
        effect->timer++;
        return;
    }
    if (frozen == 0)
    {
        effect->vel.x *= 0.85f;
        effect->vel.y *= 0.85f;
        effect->vel.z *= 0.85f;
    }
    effect->pos.x += effect->vel.x * speed;
    effect->pos.y += effect->vel.y * speed;
    effect->pos.z += effect->vel.z * speed;
    effect->scale.x += 0.2f * speed * (effect->scale.y - effect->scale.x);
    if (effect->timer < 12.0f)
    {
        t = effect->timer;
        t = (t - 1.0f) / t;
        effect->unk18 *= t;
    }
}

/* Speed streak: disp (two additive layers, pushed toward the camera) */
void fn_12_B2FC(struct Effect *effect)
{
    struct GMAModel *model;
    float scale;
    float t;
    Vec pos;

    if (!(g_some_render_flag & 4))
    {
        model = effect->model;
        scale = effect->scale.x;
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        mtxa_rigid_inv_tf_tl(&pos);
        if (pos.z < 0.0f)
            mtxa_rotate_y(-0x8000);
        mathutil_mtxA_get_translate(&pos);
        if (!(pos.z >= -scale))
        {
            t = (pos.z + scale) / pos.z;
            pos.x *= t;
            pos.y *= t;
            pos.z *= t;
            scale *= t;
            mathutil_mtxA_set_translate(&pos);
            mtxa_scale_s(scale);
            set_post_mult_color(effect->unk18, effect->unk18, effect->unk18, 1.0f);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_set_scale_factor(scale);
            avdisp_draw_model_culled_sort_auto(model);
            mtxa_sq_from_identity();
            mtxa_scale_s(0.75f * scale);
            avdisp_set_scale_factor(0.75f * scale);
            avdisp_draw_model_culled_sort_auto(model);
            fade_color_base_default();
            avdisp_set_z_mode(1, 3, 1);
        }
    }
}

/* Speed streak: dest */
void fn_12_B4B4(struct Effect *effect) {}

/* Dust puff: init */
void fn_12_B4B8(struct Effect *effect)
{
    float size;

    effect->timer = 60.0f * (0.25f + 0.5f * RAND_FLOAT());
    effect->model = minigameGma->modelEntries[169].model;
    size = 0.1f + 0.175f * RAND_FLOAT();
    effect->scale.x = 0.5f * size;
    effect->scale.y = size;
    effect->unk8C.x = 1.025f - 0.075f * RAND_FLOAT();
    effect->unk8C.y = 1.025f - 0.075f * RAND_FLOAT();
    effect->unk8C.z = 1.025f - 0.075f * RAND_FLOAT();
    effect->cameras = 1 << effect->playerId;
}

/* Dust puff: tick (frozen while the player's unk12 is set) */
void fn_12_B668(struct Effect *effect)
{
    int frozen = lbl_12_bss_1ACA0.players[effect->playerId].stunTimer;
    float speed;
    float t;

    if (frozen == 0)
        speed = 1.0f;
    else
    {
        effect->timer++;
        return;
    }
    if (frozen == 0)
    {
        effect->vel.x *= effect->unk8C.x;
        effect->vel.y *= effect->unk8C.x;
        effect->vel.z *= effect->unk8C.x;
    }
    effect->pos.x += effect->vel.x * speed;
    effect->pos.y += effect->vel.y * speed;
    effect->pos.z += effect->vel.z * speed;
    if (effect->timer < 12.0f)
    {
        t = effect->timer;
        t = (t - 1.0f) / t;
        effect->scale.x *= t;
    }
    else
        effect->scale.x += 0.15f * (effect->scale.y - effect->scale.x);
}

/* Dust puff: disp */
void fn_12_B7B0(struct Effect *effect)
{
    struct GMAModel *model;
    float scale;

    if (g_some_render_flag & 4)
        return;
    model = effect->model;
    scale = effect->scale.x;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_rotate_y(-0x8000);
    mtxa_scale_s(scale);
    set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_set_scale_factor(scale);
    avdisp_draw_model_culled_sort_always(model);
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
}

/* Dust puff: dest */
void fn_12_B87C(struct Effect *effect) {}

/* Score number (screen-space digits near the HUD): init */
void fn_12_B880(struct Effect *effect)
{
    struct Camera *camera = &cameras[0];
    int row = effect->unkA6;
    Vec pos;
    float t;
    float h;
    float w;

    effect->cameras = 0xFFFF;
    effect->state = 0;
    effect->scale.x = 0.0f;
    effect->scale.y = 0.0f;
    effect->scale.z = 0.5f * (1.0f + 0.2f * RAND_FLOAT());
    effect->unk56 = (rand() & 0x1FFF) - 0x1000;
    mtxa_from_mtx(camera->unk14C);
    mtxa_tf_point(&effect->pos, &pos);
    if (pos.z > -0.105f)
        pos.z = -0.105f;
    t = -0.105f / pos.z;
    pos.x *= t;
    pos.y *= t;
    pos.z = -0.105f;
    effect->pos = pos;
    h = 0.06824999f * camera->sub28.view.tanHalfFov;
    w = 0.08925f * (camera->sub28.view.aspect * camera->sub28.view.tanHalfFov);
    if (pos.x < -w)
        pos.x = -w;
    else if (pos.x > w)
        pos.x = w;
    if (pos.y < -h)
        pos.y = -h;
    else if (pos.y > h)
        pos.y = h;
    effect->unk8C.x = pos.x;
    effect->unk8C.y = pos.y + row * (0.015749998f * camera->sub28.view.tanHalfFov);
    effect->unk8C.z = pos.z;
    effect->u_otherTimer = 60.0f * (0.6f + 0.1f * row);
}

/* Score number: tick (pops up, flies to the ball's screen position, then fades) */
void fn_12_BAC8(struct Effect *effect)
{
    struct Camera *camera = &cameras[0];
    struct Ball *ball = &balls[effect->playerId];
    float speed;
    float size = 1.0f;
    Vec pos;
    Vec vel;
    float t;

    switch (effect->state)
    {
    case 0:
        vel.x = 0.2f * (effect->unk8C.x - effect->pos.x);
        vel.y = 0.2f * (effect->unk8C.y - effect->pos.y);
        vel.z = 0.0f;
        effect->vel = vel;
        effect->pos.x += vel.x;
        effect->pos.y += vel.y;
        effect->pos.z += vel.z;
        effect->unk56 = 0.9f * effect->unk56;
        effect->unk56 += (s16)(-768.0f * math_sin(effect->rotZ));
        if (effect->u_otherTimer < 30.0f)
        {
            t = 0.5f / (float)(effect->u_otherTimer + 1);
            effect->unk18 += (1.0f - effect->unk18) * t;
            effect->unk1C += (1.0f - effect->unk1C) * t;
            effect->unk20 += (1.0f - effect->unk20) * t;
        }
        if (effect->unk98.x != 1.0f && effect->u_otherTimer < 18.0f)
        {
            if (effect->u_otherTimer < 9.0f)
                effect->unk98.x = 1.0f;
            else
            {
                t = 1.0f / ((float)(effect->u_otherTimer + 1) - 9.0f);
                effect->unk98.x += (1.0f - effect->unk98.x) * t;
            }
        }
        effect->u_otherTimer--;
        if (effect->u_otherTimer == 0)
        {
            effect->state = 1;
            effect->unk56 = (rand() & 0x7FF) + 0x1000;
            effect->unk98.x = 1.0f;
            effect->u_otherTimer = 60;
        }
        break;
    case 1:
    {
        Vec color;

        size *= 0.45f;
        speed = 0.0058333334f * camera->sub28.view.tanHalfFov;
        mtxa_from_mtx(camera->unk14C);
        mtxa_tf_point(&ball->pos, &pos);
        if (pos.z > -0.105f)
            pos.z = -0.105f;
        t = -0.105f / pos.z;
        pos.x *= t;
        pos.y *= t;
        effect->u_otherTimer--;
        if (effect->u_otherTimer == 0
         || math_sqrt(mathutil_sum_of_sq_2(pos.x - effect->pos.x, pos.y - effect->pos.y)) < 2.0f * speed)
        {
            effect->state = 2;
            effect->u_otherTimer = 30;
        }
        else
        {
            t = 1.0f / (float)(effect->u_otherTimer + 1);
            vel.x = t * (pos.x - effect->pos.x);
            vel.y = t * (pos.y - effect->pos.y);
            if (math_sqrt(mathutil_sum_of_sq_2(vel.x, vel.y)) < speed)
            {
                vel.z = 0.0f;
                vec_set_len(speed, &vel, &vel);
            }
            effect->vel.x += 0.2f * (vel.x - effect->vel.x);
            effect->vel.y += 0.2f * (vel.y - effect->vel.y);
            effect->pos.x += effect->vel.x;
            effect->pos.y += effect->vel.y;
        }
        color = lbl_1_data_2E418[ball->colorId];
        effect->unk18 += 0.1f * (color.x - effect->unk18);
        effect->unk1C += 0.1f * (color.y - effect->unk1C);
        effect->unk20 += 0.1f * (color.z - effect->unk20);
        effect->unk56 = 0.98f * effect->unk56;
        if (ball->flags & 0x10ULL)
        {
            effect->state = 3;
            effect->u_otherTimer = 30;
        }
        break;
    }
    case 2:
        mtxa_from_mtx(camera->unk14C);
        mtxa_tf_point(&ball->pos, &pos);
        if (pos.z > -0.105f)
            pos.z = -0.105f;
        t = -0.105f / pos.z;
        pos.x *= t;
        pos.y *= t;
        t = 0.1f + 0.03f * effect->u_otherTimer;
        vel.x = t * (pos.x - effect->pos.x);
        vel.y = t * (pos.y - effect->pos.y);
        t = 0.2f + 0.026666667f * effect->u_otherTimer;
        effect->vel.x += t * (vel.x - effect->vel.x);
        effect->vel.y += t * (vel.y - effect->vel.y);
        effect->pos.x += effect->vel.x;
        effect->pos.y += effect->vel.y;
        effect->unk56 = 0.8f * effect->unk56;
        effect->unk56 += (s16)(-1024.0f * math_sin(effect->rotZ));
        if (effect->u_otherTimer < 30.0f)
        {
            Vec color;

            t = 1.0f / (float)(effect->u_otherTimer + 1);
            color = lbl_1_data_2E418[ball->colorId];
            effect->unk18 += t * (color.x - effect->unk18);
            effect->unk1C += t * (color.y - effect->unk1C);
            effect->unk20 += t * (color.z - effect->unk20);
        }
        if (effect->u_otherTimer < 6.0f)
            size = 0.0f;
        else
            size *= 0.6f;
        effect->u_otherTimer--;
        if (effect->u_otherTimer == 0)
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        if (ball->flags & 0x10ULL)
            effect->state = 3;
        break;
    case 3:
        effect->vel.x *= 0.7f;
        effect->vel.y *= 0.7f;
        effect->pos.x += effect->vel.x;
        effect->pos.y += effect->vel.y;
        effect->unk56 = 0.8f * effect->unk56;
        effect->unk56 += (s16)(-1024.0f * math_sin(effect->rotZ));
        if (effect->u_otherTimer < 30.0f)
        {
            Vec color;

            t = 1.0f / (float)(effect->u_otherTimer + 1);
            color = lbl_1_data_2E418[ball->colorId];
            effect->unk18 += t * (color.x - effect->unk18);
            effect->unk1C += t * (color.y - effect->unk1C);
            effect->unk20 += t * (color.z - effect->unk20);
        }
        if (effect->u_otherTimer < 6.0f)
            size = 0.0f;
        else
            size *= 0.45f;
        effect->u_otherTimer--;
        if (effect->u_otherTimer == 0)
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        effect->unk98.x = 0.2f * (1.0f - effect->unk98.x);
        break;
    }
    effect->scale.x += 0.15f * (size - effect->scale.x);
    effect->scale.z *= 0.99f;
    effect->scale.z += -0.041666668f;
    effect->scale.y += effect->scale.z;
    if (effect->scale.y < 0.0f)
    {
        effect->scale.y = 0.0f;
        if (effect->scale.z < 0.0f)
            effect->scale.z *= -0.6f;
    }
    effect->rotZ += effect->unk56;
}

/* Score number: disp (digits right to left, then the sign/plate) */
void fn_12_C66C(struct Effect *effect)
{
    float t;
    float squash;
    float scale;
    Vec pos;
    int n;
    int digits;
    int digit;
    struct GMAModel *model;

    if (g_some_render_flag & 4)
        return;
    t = 1.0f + 0.01f * effect->poolIndex;
    pos.x = effect->pos.x * t;
    pos.y = effect->pos.y * t;
    pos.z = effect->pos.z * t;
    mtxa_from_translate(&pos);
    t = (-0.06f * (g_current_camera->sub28.view.tanHalfFov * pos.z)) / 0.5f;
    mtxa_scale_s(t);
    mtxa_translate_xyz(0.0f, effect->scale.y, 0.0f);
    mtxa_scale_s(effect->scale.x);
    scale = t * effect->scale.x;
    digits = 1;
    for (n = effect->unkA4; n > 0; digits++)
        n /= 10;
    mtxa_translate_xyz(0.375f * digits, 0.0f, 0.0f);
    if (effect->rotZ != 0)
        mtxa_rotate_z(effect->rotZ);
    set_post_mult_color(effect->unk18, effect->unk1C, effect->unk20, 1.0f);
    for (n = effect->unkA4; n != 0; )
    {
        digit = n % 10;
        n /= 10;
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        model = minigameGma->modelEntries[lbl_12_data_5F0[digit]].model;
        avdisp_set_scale_factor(scale);
        avdisp_draw_model_culled_sort_never(model);
        mtxa_translate_xyz(-0.75f, 0.0f, 0.0f);
    }
    avdisp_set_scale_factor(scale);
    t = effect->unk98.x;
    squash = fabs(t);
    if (squash != 1.0f)
    {
        mtxa_translate_xyz(0.0f, 0.25f, 0.0f);
        mtxa_scale_xyz(1.0f, squash, 1.0f);
        mtxa_translate_xyz(0.0f, -0.25f, 0.0f);
    }
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    if (t < 0.0f)
        model = minigameGma->modelEntries[108].model;
    else
        model = minigameGma->modelEntries[118].model;
    avdisp_draw_model_culled_sort_never(model);
    fade_color_base_default();
}

/* Score number: dest */
void fn_12_C970(struct Effect *effect) {}

/* Stage gimmick effect: init (forwards to the stage type's hook) */
void fn_12_C974(struct Effect *effect)
{
    if (lbl_12_bss_1EE38.hooks[0] != NULL)
        ((void (*)(struct Effect *))lbl_12_bss_1EE38.hooks[0])(effect);
}

/* Stage gimmick effect: tick */
void fn_12_C9B8(struct Effect *effect)
{
    if (lbl_12_bss_1EE38.hooks[1] != NULL)
        ((void (*)(struct Effect *))lbl_12_bss_1EE38.hooks[1])(effect);
}

/* Stage gimmick effect: disp */
void fn_12_C9FC(struct Effect *effect)
{
    if (lbl_12_bss_1EE38.hooks[2] != NULL)
        ((void (*)(struct Effect *))lbl_12_bss_1EE38.hooks[2])(effect);
}

/* Stage gimmick effect: dest */
void fn_12_CA40(struct Effect *effect)
{
    if (lbl_12_bss_1EE38.hooks[3] != NULL)
        ((void (*)(struct Effect *))lbl_12_bss_1EE38.hooks[3])(effect);
}
