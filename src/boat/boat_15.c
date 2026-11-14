#include "boat.h"
#include "macros.h"

s32 lbl_14_bss_BCE8 = 0;
struct BoatVortexMesh lbl_14_bss_BCEC[2];
struct BoatVortex lbl_14_bss_BD14[0x20];

const s8 lbl_14_rodata_11D8[2] = {0x13, 0x14};
u32 lbl_14_data_2218[8] = {0x278, 0x279, 0x27A, 0x27B, 0x27C, 0x27D, 0x27E, 0x27F};

void fn_14_239C0(void)
{
    s32 i;
    struct BoatVortexMesh *mesh;
    s32 j;
    s32 k;
    struct BoatVortex *vortex;

    fn_14_23E2C();
    i = 0;
    mesh = lbl_14_bss_BCEC;
    for (; i < 2; i++, mesh++) {
        mesh->model = lbl_14_bss_B4[lbl_14_rodata_11D8[i]];
        mesh->unk4 = fn_8006991C(mesh->model);
        mesh->verts = fn_80069870(mesh->model);
        mesh->count = *mesh->unk4;
        {
            u32 size = (mesh->count * sizeof(Vec) + 0x1F) & ~0x1F;
            mesh->orig = alloc_from_heap_or_panic(currentHeap, size, "boat_vortex.c", 0x88);
        }
        for (j = 0; j < mesh->count; j++) {
            mesh->orig[j] = mesh->verts[j].pos;
        }
    }
    k = 0;
    vortex = lbl_14_bss_BD14;
    for (; k < 0x20; k++, vortex++) {
        vortex->pos = (Vec){0.0f, 0.0f, 0.0f};
        vortex->radius = 0.0f;
        vortex->strength = 0.0f;
        vortex->alpha = 0.0f;
        vortex->timer = 0;
        vortex->duration = 0;
        vortex->angle = 0;
        vortex->sound = -1;
    }
    fn_14_247EC();
}

void fn_14_23B78(void)
{
    s32 i;
    struct BoatVortex *vortex;

    i = 0;
    vortex = lbl_14_bss_BD14;
    for (; i < 0x20; i++, vortex++) {
        if (vortex->timer > 0) {
            vortex->timer--;
            vortex->alpha = 2.0f * math_sin((float)(vortex->timer << 15) / (float)vortex->duration);
            vortex->alpha = vortex->alpha < 1.0f ? vortex->alpha : 1.0f;
            vortex->angle = -(vortex->timer << 8);
            if (vortex->sound >= 0) {
                fn_14_12BF4(lbl_14_data_2218[vortex->sound], vortex);
                if (vortex->timer == 0) {
                    fn_14_24758(vortex->sound);
                }
            }
        }
    }
}

void fn_14_23CE0(void)
{
    void *prev;
    s32 i;
    struct BoatVortex *vortex;
    s32 j;
    struct BoatVortexMesh *mesh;

    avdisp_set_z_mode(1, 3, 0);
    prev = g_yet_another_unk_draw_func(fn_14_2435C);
    i = 0;
    vortex = lbl_14_bss_BD14;
    for (; i < 0x20; i++, vortex++) {
        if (vortex->timer > 0) {
            fn_14_241F4(vortex);
            mtxa_from_mtxb_translate(&vortex->pos);
            mtxa_rotate_y(vortex->angle);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            j = 0;
            mesh = lbl_14_bss_BCEC;
            for (; j < 2; j++, mesh++) {
                set_post_mult_color(1.0f, 1.0f, 1.0f, vortex->alpha);
                avdisp_draw_model_unculled_sort_never(mesh->model);
            }
        }
    }
    g_yet_another_unk_draw_func(prev);
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    avdisp_set_z_mode(1, 3, 1);
}

void fn_14_23E2C(void)
{
    s32 i;
    struct BoatVortexMesh *mesh;

    fn_14_242E0();
    i = 0;
    mesh = lbl_14_bss_BCEC;
    for (; i < 2; i++, mesh++) {
        if (mesh->orig != NULL) {
            OSFreeToHeap(currentHeap, mesh->orig);
            mesh->orig = NULL;
            mesh->count = 0;
        }
    }
    fn_14_247EC();
}

void fn_14_23EB4(Vec *pos, float radius, float strength, float c, s16 time)
{
    struct RaycastHit hit;
    s32 i;
    struct BoatVortex *vortex;

    i = 0;
    vortex = lbl_14_bss_BD14;
    for (; i < 0x20; i++, vortex++) {
        if (vortex->timer <= 0) {
            if (!fn_14_13CA8(pos, &hit, 7)) {
                return;
            }
            vortex->pos.x = pos->x;
            vortex->pos.y = hit.pos.y;
            vortex->pos.z = pos->z;
            vortex->radius = radius;
            vortex->strength = strength;
            vortex->timer = time;
            vortex->duration = time;
            vortex->sound = fn_14_246A4(vortex);
            return;
        }
    }
}

float fn_14_23F98(Vec *pos, Vec *out)
{
    Vec sp8;
    float sum;
    float dist;
    float dx;
    float dz;
    float s;
    float t;
    s32 i;
    struct BoatVortex *vortex;

    sum = 0.0f;
    if (out != NULL) {
        sp8 = *out;
    }
    i = 0;
    vortex = lbl_14_bss_BD14;
    for (; i < 0x20; i++, vortex++) {
        if (sum > 3.0f || sum < -3.0f) {
            break;
        }
        if (vortex->timer > 0) {
            float vx;
            float vz;
            float px;
            float pz;
            pz = pos->z;
            px = pos->x;
            vz = vortex->pos.z;
            vx = vortex->pos.x;
            vx -= px;
            vz -= pz;
            dist = math_sqrt(mathutil_sum_of_sq_2(vx, vz));
            if (!(dist >= vortex->radius)) {
                s = math_sin((float)(vortex->timer << 15) / (float)vortex->duration);
                t = 1.0f - dist / vortex->radius;
                dist = vortex->strength * s;
                sum += dist * t;
                if (out != NULL) {
                    dx = vortex->pos.x - pos->x;
                    dz = vortex->pos.z - pos->z;
                    {
                        float inv = 1.0f / math_sqrt(dx * dx + dz * dz);
                        dx *= inv;
                        dz *= inv;
                    }
                    sp8.x -= 0.1f * dist * dx;
                    sp8.z -= 0.1f * dist * dz;
                }
            }
        }
    }
    if (out != NULL) {
        vec_normalize_len(&sp8);
        *out = sp8;
    }
    sum = sum < -3.0f ? -3.0f : (sum > 3.0f ? 3.0f : sum);
    return sum;
}

void fn_14_241F4(struct BoatVortex *vortex)
{
    Vec sp8;
    float d;
    float y;
    s32 i;
    struct BoatVortexMesh *mesh;
    s32 j;

    mtxa_from_translate(&vortex->pos);
    mtxa_rotate_y(vortex->angle);
    i = 0;
    mesh = lbl_14_bss_BCEC;
    for (; i < 2; i++, mesh++) {
        for (j = 0; j < mesh->count; j++) {
            mtxa_tf_point(&mesh->orig[j], &sp8);
            d = fn_14_23F98(&sp8, NULL);
            y = fn_14_14114(&sp8, 0);
            y = y + d;
            mesh->verts[j].pos.y = y + mesh->orig[j].y;
        }
    }
}

void fn_14_242E0(void)
{
    s32 i;
    struct BoatVortexMesh *mesh;
    s32 j;

    i = 0;
    mesh = lbl_14_bss_BCEC;
    for (; i < 2; i++, mesh++) {
        if (mesh->orig != NULL) {
            for (j = 0; j < mesh->count; j++) {
                mesh->verts[j].pos = mesh->orig[j];
            }
        }
    }
}

inline void inline_texcoord_gen(int dst, int func, int src, int mtx) {
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}

void fn_14_2435C(struct BallEnvArg *arg0)
{
    float sp10[2][3] = {{0.0f, 0.03f, 0.0f}, {0.0f, 0.0f, 0.03f}};
    struct TevStageInfo tev = arg0->tev;

    GXLoadTexObj_cached(lbl_14_bss_B4[16]->texObjs, tev.texMapId);
    mtxa_push();
    mtxa_from_translate_xyz(0.0f, -0.005f * (float)g_font_char_var_3, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    mtxa_from_translate_xyz(0.0f, -0.01f * (float)g_font_char_var_3, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);
    mtxa_from_translate_xyz(0.0f, -0.002f * (float)g_font_char_var_3, 0.0f);
    mtxa_scale_s(4.0f);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId, 1);
    mtxa_pop();
    inline_texcoord_gen(0, 1, 4, 0x1E);
    inline_texcoord_gen(1, 1, 4, 0x21);
    inline_texcoord_gen(tev.texCoordId, 1, 4, tev.texMtxId);
    GXSetIndTexOrder_cached(tev.tevIndStage, tev.texCoordId, tev.texMapId);
    g_GXSetIndTexMtx_wrapper(tev.unk1C, sp10, 0);
    GXSetTevIndirect_cached(0, tev.tevIndStage, 0, 0, tev.unk1C, 0, 0, 0, 0, 0);
    GXSetTevIndirect_cached(1, tev.tevIndStage, 0, 0, tev.unk1C, 0, 0, 0, 0, 0);
    tev.unk1C += 1;
    tev.tevIndStage += 1;
    tev.texCoordId += 1;
    tev.texMtxId += 3;
    tev.texMapId += 1;
    arg0->tev = tev;
}

s16 fn_14_246A4(struct BoatVortex *vortex)
{
    s16 i;

    for (i = 0; (u32)i < 8; i++) {
        if ((u32)(lbl_14_bss_BCE8 & (1 << i)) == 0) {
            fn_14_12B24(lbl_14_data_2218[i], vortex);
            lbl_14_bss_BCE8 |= 1 << i;
            return i;
        }
    }
    return -1;
}

void fn_14_24758(s16 sound)
{
    if ((u32)(lbl_14_bss_BCE8 & (1 << sound)) != 0) {
        SoundOffID(lbl_14_data_2218[sound]);
        lbl_14_bss_BCE8 &= (s32)~(1 << sound);
    }
}

void fn_14_247EC(void)
{
    s16 i;

    for (i = 0; (u32)i < 8; i++) {
        if ((u32)(lbl_14_bss_BCE8 & (1 << i)) != 0) {
            SoundOffID(lbl_14_data_2218[i]);
            lbl_14_bss_BCE8 &= (s32)~(1 << i);
        }
    }
}

