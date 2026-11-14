#include "boat.h"
#include "macros.h"

struct BoatWaveVtx
{
    Vec pos;
    Vec nrm;
    u8 filler18[0x28];
};

struct BoatWaveMesh
{
    u32 flags;
    struct GMAModel *model;
    u32 *vtxCount;
    struct BoatWaveVtx *vtx;
    Vec *basePos;
    Vec *baseNrm;
};

struct BoatWaveLayer
{
    float u;
    float v;
    float du;
    float dv;
};

struct BoatWave
{
    struct BoatWaveLayer layers[3];
    struct BoatWaveMesh *meshes;
    struct GMAModel *falls[3];
    u32 meshCount;
    u32 timer;
    Vec smoothPos;
    Vec smoothNrm;
};

struct BoatWaveTevState
{
    s32 unk0;
    s32 texCoord;
    s32 texMtx;
    s32 texMap;
    s32 indStage;
    s32 unk14;
    s32 unk18;
    s32 indMtx;
    s32 unk20;
    s32 unk24;
    s32 unk28;
};

struct BoatWaveTev
{
    /* BallEnvArg (avdisp.h) layout: tev follows an int and two pointers */
    /*0x00*/ s32 unk0;
    /*0x04*/ void *shape;
    /*0x08*/ void *layers;
    struct BoatWaveTevState state;
};

float lbl_14_bss_2A80 = 0.0f;
struct BoatWave lbl_14_bss_2A84;
void *lbl_14_bss_2AE4;

u32 lbl_14_data_1348[0x5C] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};
Vec lbl_14_data_14B8 = {0.0f, 1.0f, 0.0f};

s16 *tri_coli_grid_lookup(struct StageCollHeader *coll, float x, float z);
BOOL g_is_ball_in_ig_coli_range(struct ItemGroup *ig, struct StageCollHeader *coll, int igId, void *sphere);
extern u32 lbl_1_bss_D4328;
void fn_14_12C7C(void)
{
    char name[0x40];
    s32 i;
    struct BoatWaveMesh *mesh;
    s32 n;
    s32 size;
    s32 j;

    fn_14_13B94();
    lbl_14_bss_2A84.meshCount = fn_14_12504(stage_gma, "WAVE_GRID");
    size = lbl_14_bss_2A84.meshCount << 5;
#ifndef __MWERKS__
    /* pc_port: 0x20 per mesh is sizeof(struct BoatWaveMesh) rounded up; the
     * x64 struct is 0x30 (six pointers), so scale the stride. */
    size *= PC_PTR_SCALE;
#endif
    lbl_14_bss_2A84.meshes = alloc_from_heap_or_panic(currentHeap, size, "boat_wave.c", 0x97);
    i = 0;
    mesh = lbl_14_bss_2A84.meshes;
    while (i < (s32)lbl_14_bss_2A84.meshCount) {
        sprintf(name, "WAVE_GRID%02d_NODISP_vtx", i + 1);
        mesh->flags = 0;
        mesh->model = fn_14_12598(stage_gma, name);
        if (mesh->model == NULL) {
            sprintf(name, "WAVE_GRID%d_vtx", i + 1);
            mesh->flags = 0;
            mesh->model = fn_14_12598(stage_gma, name);
        }
        mesh->vtxCount = fn_8006991C(mesh->model);
        mesh->vtx = fn_80069870(mesh->model);
        n = *mesh->vtxCount;
        size = ((n * 0xC) + 0x1F) & 0xFFFFFFE0;
        mesh->basePos = alloc_from_heap_or_panic(currentHeap, size, "boat_wave.c", 0xB0);
        mesh->baseNrm = alloc_from_heap_or_panic(currentHeap, size, "boat_wave.c", 0xB1);
        for (j = 0; j < n; j++) {
            mesh->basePos[j] = mesh->vtx[j].pos;
            mesh->baseNrm[j] = mesh->vtx[j].nrm;
        }
        i++;
        mesh++;
    }
    if (current_stage_id == 0x8C) {
        lbl_14_bss_2A84.falls[0] = fn_14_12598(stage_gma, "FALL01");
        lbl_14_bss_2A84.falls[1] = fn_14_12598(stage_gma, "FALL02");
        lbl_14_bss_2A84.falls[2] = fn_14_12598(stage_gma, "FALL03");
    } else {
        lbl_14_bss_2A84.falls[0] = NULL;
        lbl_14_bss_2A84.falls[1] = NULL;
        lbl_14_bss_2A84.falls[2] = NULL;
    }
    fn_14_12F3C();
}

void fn_14_12F3C(void)
{
    s32 i;

    lbl_14_bss_2A84.timer = 0;
    for (i = 0; i < 3; i++) {
        lbl_14_bss_2A84.layers[i].u = 0.0f;
        lbl_14_bss_2A84.layers[i].v = 0.0f;
    }
    switch (current_stage_id) {
    default:
    case 0x8A:
        lbl_14_bss_2AE4 = fn_14_14334;
        lbl_14_bss_2A84.layers[0].du = 0.002f;
        lbl_14_bss_2A84.layers[0].dv = 0.00375f;
        lbl_14_bss_2A84.layers[1].du = -0.002f;
        lbl_14_bss_2A84.layers[1].dv = 0.00375f;
        lbl_14_bss_2A84.layers[2].du = 0.0f;
        lbl_14_bss_2A84.layers[2].dv = 0.001f;
        break;
    case 0x8B:
        lbl_14_bss_2AE4 = fn_14_14334;
        lbl_14_bss_2A84.layers[0].du = 0.002f;
        lbl_14_bss_2A84.layers[0].dv = 0.007f;
        lbl_14_bss_2A84.layers[1].du = -0.002f;
        lbl_14_bss_2A84.layers[1].dv = 0.007f;
        lbl_14_bss_2A84.layers[2].du = 0.0f;
        lbl_14_bss_2A84.layers[2].dv = 0.002f;
        break;
    case 0x8C:
        lbl_14_bss_2AE4 = fn_14_14334;
        lbl_14_bss_2A84.layers[0].du = 0.002f;
        lbl_14_bss_2A84.layers[0].dv = 0.01f;
        lbl_14_bss_2A84.layers[1].du = -0.002f;
        lbl_14_bss_2A84.layers[1].dv = 0.01f;
        lbl_14_bss_2A84.layers[2].du = 0.0f;
        lbl_14_bss_2A84.layers[2].dv = 0.003f;
        lbl_14_bss_2A84.meshes[0x12].flags |= 0xC;
        break;
    }
}

void fn_14_13144(void)
{
#ifdef __MWERKS__
    register Vec *pb;
#endif
    Vec pos;
    struct BoatParticleParam param;
    struct BoatWaveMesh *mesh;
    s32 i;
    s32 j;
    s32 n;

    if (lbl_14_bss_2A84.meshes != NULL && !(lbl_1_bss_D4328 & 0x10000)) {
        lbl_14_bss_2A84.timer++;
        lbl_14_bss_2A84.layers[0].u += lbl_14_bss_2A84.layers[0].du;
        lbl_14_bss_2A84.layers[0].v += lbl_14_bss_2A84.layers[0].dv;
        lbl_14_bss_2A84.layers[1].u += lbl_14_bss_2A84.layers[1].du;
        lbl_14_bss_2A84.layers[1].v += lbl_14_bss_2A84.layers[1].dv;
        lbl_14_bss_2A84.layers[2].v += lbl_14_bss_2A84.layers[2].dv;
        i = 0;
        mesh = lbl_14_bss_2A84.meshes;
        /* Keep the shared radius live through both named-register distance kernels. */
#ifdef __MWERKS__
        while (i < (s32)lbl_14_bss_2A84.meshCount) {
            register float radius, z2, y2, x2, x1, y1, z1;
            pos = mesh->model->boundSphereCenter;
            {
                register struct GMAModel *radiusModel = mesh->model;
                asm
                {
                    lfs radius, radiusModel->boundSphereRadius
                }
            }
            if (mesh->flags & 0x10) {
                mesh->flags |= 2;
                mesh->flags &= ~0x10;
            } else {
                s32 k;
                mesh->flags &= ~2;
                for (k = 0; k < 5; k++) {
                    if (g_poolInfo.cameraPool.statusList[k] != 0 && g_poolInfo.cameraPool.statusList[k] != 4) {
                        register float one, limit, d;
                        {
                            register Vec *pa;
                            pb = &pos;
                            pa = &cameras[k].eye;
                            asm
                            {
                                lfs x1, pa->x
                                lfs x2, pb->x
                                lfs y1, pa->y
                                lfs y2, pb->y
                                lfs z1, pa->z
                                lfs z2, pb->z
                                fsubs x1, x1, x2
                                fsubs y1, y1, y2
                                fsubs z1, z1, z2
                                fmuls x1, x1, x1
                                fmadds x1, y1, y1, x1
                                fmadds x1, z1, z1, x1
                                fmr d, x1
                            }
                        }
                        one = 1.0f;
                        asm
                        {
                            fadds limit, one, radius
                        }
                        one = 1.0f;
                        asm
                        {
                            fadds one, one, radius
                            fmuls one, limit, one
                            fcmpo cr0, d, one
                            bge cameraNext
                        }
                        mesh->flags |= 2;
                        break;
                    cameraNext:
                        ;
                    }
                }
                if (!(mesh->flags & 2)) {
                    s32 l;
                    for (l = 0; l < 8; l++) {
                        if (balls[l].status == 2) {
                            register float one, limit, d;
                            {
                                register Vec *pa;
                                pb = &pos;
                                pa = &balls[l].pos;
                                asm
                                {
                                    lfs x1, pa->x
                                    lfs x2, pb->x
                                    lfs y1, pa->y
                                    lfs y2, pb->y
                                    lfs z1, pa->z
                                    lfs z2, pb->z
                                    fsubs x1, x1, x2
                                    fsubs y1, y1, y2
                                    fsubs z1, z1, z2
                                    fmuls x1, x1, x1
                                    fmadds x1, y1, y1, x1
                                    fmadds x1, z1, z1, x1
                                    fmr d, x1
                                }
                            }
                            one = balls[l].currRadius;
                            asm
                            {
                                fadds limit, radius, one
                            }
                            one = balls[l].currRadius;
                            asm
                            {
                                fadds one, radius, one
                                fmuls one, limit, one
                                fcmpo cr0, d, one
                                bge ballNext
                            }
                            mesh->flags |= 2;
                            break;
                        ballNext:
                            ;
                        }
                    }
                }
            }
            i++;
            mesh++;
        }
#else
        while (i < (s32)lbl_14_bss_2A84.meshCount) {
            float radius;
            pos = mesh->model->boundSphereCenter;
            radius = mesh->model->boundSphereRadius;
            if (mesh->flags & 0x10) {
                mesh->flags |= 2;
                mesh->flags &= ~0x10;
            } else {
                s32 k;
                mesh->flags &= ~2;
                for (k = 0; k < 5; k++) {
                    if (g_poolInfo.cameraPool.statusList[k] != 0 && g_poolInfo.cameraPool.statusList[k] != 4) {
                        float d = mathutil_vec_sq_distance(&cameras[k].eye, &pos);
                        if (d < (1.0f + radius) * (1.0f + radius)) {
                            mesh->flags |= 2;
                            break;
                        }
                    }
                }
                if (!(mesh->flags & 2)) {
                    s32 l;
                    for (l = 0; l < 8; l++) {
                        if (balls[l].status == 2) {
                            float d = mathutil_vec_sq_distance(&balls[l].pos, &pos);
                            if (d < (radius + balls[l].currRadius) * (radius + balls[l].currRadius)) {
                                mesh->flags |= 2;
                                break;
                            }
                        }
                    }
                }
            }
            i++;
            mesh++;
        }
#endif
        i = 0;
        mesh = lbl_14_bss_2A84.meshes;
        while (i < (s32)lbl_14_bss_2A84.meshCount) {
            n = *lbl_14_bss_2A84.meshes[i].vtxCount;
            if (mesh->flags & 2) {
                for (j = 0; j < n; j++) {
                    Vec *vtx = &mesh->vtx[j].pos;
                    Vec *base = &mesh->basePos[j];
                    float wave;
                    float h = fn_14_23F98(vtx, NULL);
                    wave = base->y + fn_14_14114(vtx, NULL);
                    wave += h;
                    vtx->y = wave;
                }
            }
            i++;
            mesh++;
        }
        if (current_stage_id == 0x8C) {
            s8 models[1] = {0x0F};
            Vec locs[6] = {
                {67.7f, -57.35f, -210.3f}, {64.9f, -57.65f, -212.9f}, {62.1f, -57.85f, -215.5f},
                {5.2f, -88.15f, -304.2f}, {4.9f, -89.35f, -308.4f}, {4.6f, -90.45f, -312.6f},
            };
            Vec vel = {-0.06f, 0.225f, 0.03f};
            u32 m;

            fn_14_2190C(&param);
            param.vel = &vel;
            param.accel.y = -0.012495f;
            param.velScale = 0.152f;
            param.drag = 0.95f;
            param.scale = 1.6f;
            param.posScale = 1.4399999f;
            param.life = 0x1E;
            param.count = 1;
            param.models = models;
            param.numModels = 1;
            for (m = 0; m < 6; m++) {
                if ((s32)(m & 3) == (s32)(lbl_14_bss_2A84.timer & 3)) {
                    for (j = 0; j < 0x5D; j++) {
                        if (g_poolInfo.cameraPool.statusList[j] == 2) {
                            float d = mathutil_vec_sq_distance(&cameras[j].eye, &locs[m]);
                            if (!(d > 10000.0f)) {
                                param.pos = &locs[m];
                                fn_14_215F8(&param);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void fn_14_136D4(void)
{
    Vec pos;
    Vec nrm;
    void *prev;
    struct BoatWaveMesh *mesh;
    s32 i;

    if (lbl_14_bss_2A84.meshes != NULL && !(dip_switches & 4)) {
        pos = current_ball->pos;
        pos.y = fn_14_1400C(&pos, &nrm);
        lbl_14_bss_2A84.smoothPos.x += 0.1f * (pos.x - lbl_14_bss_2A84.smoothPos.x);
        lbl_14_bss_2A84.smoothPos.y += 0.1f * (pos.y - lbl_14_bss_2A84.smoothPos.y);
        lbl_14_bss_2A84.smoothPos.z += 0.1f * (pos.z - lbl_14_bss_2A84.smoothPos.z);
        lbl_14_bss_2A84.smoothNrm.x += 0.1f * (nrm.x - lbl_14_bss_2A84.smoothNrm.x);
        lbl_14_bss_2A84.smoothNrm.y += 0.1f * (nrm.y - lbl_14_bss_2A84.smoothNrm.y);
        lbl_14_bss_2A84.smoothNrm.z += 0.1f * (nrm.z - lbl_14_bss_2A84.smoothNrm.z);
        fn_14_14720();
        mtxa_from_mtxb();
        if (!(dip_switches & 1) || !(lbl_1_bss_D4328 & 0x4000)) {
            prev = g_yet_another_unk_draw_func(lbl_14_bss_2AE4);
        }
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        i = 0;
        mesh = lbl_14_bss_2A84.meshes;
        while (i < (s32)lbl_14_bss_2A84.meshCount) {
            if (mesh->model != NULL && (mesh->flags & 1)) {
                if (mesh->flags & 4) {
                    if (!(dip_switches & 1) || !(lbl_1_bss_D4328 & 0x4000)) {
                        g_yet_another_unk_draw_func(fn_14_14644);
                    }
                } else if (!(dip_switches & 1) || !(lbl_1_bss_D4328 & 0x4000)) {
                    g_yet_another_unk_draw_func(lbl_14_bss_2AE4);
                }
                avdisp_draw_model_unculled_sort_never(mesh->model);
            }
            i++;
            mesh++;
        }
        if (!(dip_switches & 1) || !(lbl_1_bss_D4328 & 0x4000)) {
            g_yet_another_unk_draw_func(prev);
        }
        if (current_stage_id == 0x8C && g_current_camera->eye.y < -3.0f) {
            prev = g_yet_another_unk_draw_func(fn_14_14644);
            for (i = 0; i < 3; i++) {
                avdisp_draw_model_culled_sort_never(lbl_14_bss_2A84.falls[i]);
            }
            g_yet_another_unk_draw_func(prev);
        }
    }
}

void fn_14_13AB0(void)
{
    struct BoatWaveMesh *mesh;
    s32 i;

    if (lbl_14_bss_2A84.meshes != NULL) {
        mtxa_from_mtxb();
        mtxa_translate_xyz(0.0f, -0.02f, 0.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        i = 0;
        mesh = lbl_14_bss_2A84.meshes;
        while (i < (s32)lbl_14_bss_2A84.meshCount) {
            if (mesh->model != NULL && (mesh->flags & 1) && !(mesh->flags & 8)) {
                avdisp_draw_model_unculled_sort_never(mesh->model);
            }
            i++;
            mesh++;
        }
    }
}

void fn_14_13B94(void)
{
    s32 i;

    if (lbl_14_bss_2A84.meshes != NULL) {
        fn_14_14284();
        for (i = 0; i < (s32)lbl_14_bss_2A84.meshCount; i++) {
            OSFreeToHeap(currentHeap, lbl_14_bss_2A84.meshes[i].basePos);
            OSFreeToHeap(currentHeap, lbl_14_bss_2A84.meshes[i].baseNrm);
        }
        OSFreeToHeap(currentHeap, lbl_14_bss_2A84.meshes);
        lbl_14_bss_2A84.meshes = NULL;
    }
}

float fn_14_13C6C(Vec *pos)
{
    return fn_14_13E1C(pos, NULL) - pos->y;
}

s32 fn_14_13CA8(Vec *pos, struct RaycastHit *hit, u32 mask)
{
    if (!(mask & 3) || current_stage_id == 0x8C) {
        return fn_14_14890(pos, hit, mask);
    }
    hit->flags = 1;
    hit->normal = (Vec){0.0f, 1.0f, 0.0f};
    hit->pos = (Vec){0.0f, 0.0f, 0.0f};
    if (mask & 2) {
        return 1;
    }
    if (mask & 4) {
        float h = fn_14_23F98(pos, NULL);
        float w = fn_14_14114(pos, NULL);
        w += h;
        hit->pos.y += w;
    } else {
        float h = fn_14_23F98(pos, &hit->normal);
        float w = fn_14_14114(pos, &hit->normal);
        w += h;
        hit->pos.y += w;
    }
    return 1;
}

float fn_14_13E1C(Vec *pos, Vec *nrm)
{
    struct RaycastHit hit;
    u32 flags = 1;
    s32 ret;

    if (nrm == NULL) {
        flags |= 4;
    }
    ret = fn_14_13CA8(pos, &hit, flags);
    if (ret != 0) {
        lbl_14_bss_2A80 = hit.pos.y;
        lbl_14_data_14B8 = hit.normal;
    }
    if (nrm != NULL) {
        *nrm = lbl_14_data_14B8;
    }
    return lbl_14_bss_2A80;
}

float fn_14_1400C(Vec *pos, Vec *nrm)
{
    struct RaycastHit hit;

    if (current_stage_id == 0x8C) {
        if (fn_14_14890(pos, &hit, 1) != 0) {
            lbl_14_bss_2A80 = hit.pos.y;
            lbl_14_data_14B8 = hit.normal;
        }
        if (nrm != NULL) {
            *nrm = lbl_14_data_14B8;
        }
        return lbl_14_bss_2A80;
    }
    if (nrm != NULL) {
        *nrm = (Vec){0.0f, 1.0f, 0.0f};
    }
    return 0.0f;
}

float fn_14_14114(Vec *pos, Vec *nrm)
{
    float amp = 0.2f;
    float div = 12.0f;
    s16 t = (lbl_14_bss_2A84.timer & 0x7F) << 9;
    s16 a = (s16)((float)t + ((65536.0f * pos->x) / div));
    s16 b = (s16)((float)t + ((65536.0f * pos->z) / div));
    float sa;
    float sb;

    if (nrm != NULL) {
        nrm->x -= 0.1f * math_sin((s16)(a + 0x4000));
        nrm->z -= 0.1f * math_sin((s16)(b + 0x4000));
        vec_normalize_len(nrm);
    }
    sb = math_sin(b);
    sa = math_sin(a);
    return amp * (sa + sb);
}

void fn_14_14284(void)
{
    struct BoatWaveMesh *mesh;
    s32 i;
    u32 j;

    i = 0;
    mesh = lbl_14_bss_2A84.meshes;
    while (i < (s32)lbl_14_bss_2A84.meshCount) {
        for (j = 0; j < *mesh->vtxCount; j++) {
            mesh->vtx[j].pos = mesh->basePos[j];
            mesh->vtx[j].nrm = mesh->baseNrm[j];
        }
        i++;
        mesh++;
    }
}

const float lbl_14_rodata_AF0[9] = {-2.0f, 0.5f, 6.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f};

static inline void wave_texgen(s32 coord, s32 mtx)
{
    GXSetTexCoordGen2_cached(coord, 1, 4, mtx, 0, 0x7D);
}

void fn_14_14334(struct BoatWaveTev *tev)
{
    float indMtx[2][3] = {{0.0f, 0.03f, 0.0f}, {0.0f, 0.0f, 0.03f}};
    struct BoatWaveTevState s;

    s = tev->state;
    GXLoadTexObj_cached(lbl_14_bss_B4[0x10]->texObjs, s.texMap);
    mtxa_push();
    mtxa_from_translate_xyz(lbl_14_bss_2A84.layers[0].u, lbl_14_bss_2A84.layers[0].v, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    mtxa_from_translate_xyz(lbl_14_bss_2A84.layers[1].u, lbl_14_bss_2A84.layers[1].v, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);
    mtxa_from_translate_xyz(4.0f * lbl_14_bss_2A84.layers[2].u, 4.0f * lbl_14_bss_2A84.layers[2].v, 0.0f);
    mtxa_scale_s(4.0f);
    GXLoadTexMtxImm(mtxa->mtxA, s.texMtx, 1);
    mtxa_pop();
    GXSetTexCoordGen2_cached(0, 1, 4, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2_cached(1, 1, 4, 0x21, 0, 0x7D);
    wave_texgen(s.texCoord, s.texMtx);
    GXSetIndTexOrder_cached(s.indStage, s.texCoord, s.texMap);
    g_GXSetIndTexMtx_wrapper(s.indMtx, indMtx, 0);
    GXSetTevIndirect_cached(0, s.indStage, 0, 0, s.indMtx, 0, 0, 0, 0, 0);
    GXSetTevIndirect_cached(1, s.indStage, 0, 0, s.indMtx, 0, 0, 0, 0, 0);
    s.indMtx++;
    s.indStage++;
    s.texCoord++;
    s.texMtx += 3;
    s.texMap++;
    tev->state = s;
}

void fn_14_14644(void)
{
    mtxa_push();
    mtxa_from_translate_xyz(lbl_14_bss_2A84.layers[0].u, lbl_14_bss_2A84.layers[0].v, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    mtxa_from_translate_xyz(lbl_14_bss_2A84.layers[1].u, lbl_14_bss_2A84.layers[1].v, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);
    mtxa_pop();
    GXSetTexCoordGen2_cached(0, 1, 4, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2_cached(1, 1, 4, 0x21, 0, 0x7D);
}

const float lbl_14_rodata_B30[12] = {0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f};

void fn_14_14720(void)
{
    Vec eye;
    Vec pos;
    float maxDist;
    struct BoatWaveMesh *mesh;
    s32 i;

    eye = g_current_camera->eye;
    if (current_stage_id == 0x8C && eye.y < -3.0f) {
        maxDist = 22500.0f;
    } else {
        maxDist = 19600.0f;
    }
    i = 0;
    mesh = lbl_14_bss_2A84.meshes;
    while (i < (s32)lbl_14_bss_2A84.meshCount) {
        float radius;

        pos = mesh->model->boundSphereCenter;
        radius = mesh->model->boundSphereRadius;
        if (mathutil_vec_sq_distance(&eye, &pos) - (radius * radius) > maxDist) {
            mesh->flags &= ~1;
        } else if (fn_1_233FC(&pos, radius) != 0) {
            mesh->flags |= 0x11;
        } else {
            mesh->flags &= ~1;
        }
        i++;
        mesh++;
    }
}

const float lbl_14_rodata_B68 = -1.0f;

s32 fn_14_14890(Vec *pos, struct RaycastHit *hit, u32 flags)
{
    struct StageColiTri tmpTri;
    Vec dir;
    struct { Vec pos; float radius; } sphere;
    Vec p;
    Vec p2;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    s32 i;
    struct ItemGroup *bestIg;
    struct StageColiTri *bestTri;
    s16 *cell;
    s16 *t;
    struct StageColiTri *tri;

    dir = (Vec){0.0f, -1.0f, 0.0f};
    hit->flags = 0;
    sphere.pos = *pos;
    sphere.radius = 0.0f;
    ig = itemgroups;
    coll = stagedef->collHeaders;
    i = 0;
    while (i < (s32)stagedef->itemGroupCount) {
        if (g_is_ball_in_ig_coli_range(ig, coll, i, &sphere) != 0) {
            cell = tri_coli_grid_lookup(coll, pos->x, pos->z);
            if (cell != NULL) {
                t = cell;
                while (*t >= 0) {
                    tri = &coll->triangles[*t];
                    if (tri->flags & 0x10) {
                        p = *pos;
                        if (fn_14_1520C(&p, &dir, tri) != 0) {
                            mtxa_from_mtx(ig->transform);
                            mtxa_tf_point(&p, &p);
                            if (!(hit->flags & 1) || p.y > hit->pos.y) {
                                bestIg = ig;
                                bestTri = tri;
                                hit->flags = tri->flags | 1;
                                hit->pos = p;
                            }
                        }
                    }
                    t++;
                }
            }
        }
        i++;
        ig++;
        coll++;
    }
    if (!(hit->flags & 1)) {
        return 0;
    }
    if (flags & 1) {
        mtxa_from_mtx(bestIg->transform);
        mtxa_tf_vec(&bestTri->normal, &hit->normal);
    } else {
        p2 = *pos;
        fn_14_14B28(bestTri, &tmpTri);
        bestTri = &tmpTri;
        fn_14_1520C(&p2, &dir, bestTri);
        mtxa_from_mtx(bestIg->transform);
        mtxa_tf_point(&p2, &p2);
        hit->pos = p2;
        mtxa_tf_vec(&bestTri->normal, &hit->normal);
    }
    return 1;
}

void fn_14_14B28(struct StageColiTri *in, struct StageColiTri *out)
{
    Vec v3;
    Vec v2;
    Vec v1;
    Vec nrm;
    Vec e;
    Vec a;
    Vec b;
    float yaw;
    float pitch;
    float roll;
    float nx1;
    float ny1;
    float x;
    float y;
    float len;
    float inv;
    float nx2;
    float ny2;

    mtxa_from_translate(&in->pos);
    mtxa_rotate_y(in->rot.y);
    mtxa_rotate_x(in->rot.x);
    mtxa_rotate_z(in->rot.z);
    v1 = in->pos;
    mtxa_tf_point_xyz(in->vert2.x, in->vert2.y, 0.0f, &v2);
    mtxa_tf_point_xyz(in->vert3.x, in->vert3.y, 0.0f, &v3);
    v1.y += fn_14_14114(&v1, NULL) + fn_14_23F98(&v1, NULL);
    v2.y += fn_14_14114(&v2, NULL) + fn_14_23F98(&v2, NULL);
    v3.y += fn_14_14114(&v3, NULL) + fn_14_23F98(&v3, NULL);
    {
        Vec *p0 = &v1;
        Vec *p1 = &v2;
        Vec *p2 = &v3;
        a.x = p1->x - p0->x;
        a.y = p1->y - p0->y;
        a.z = p1->z - p0->z;
        b.x = p2->x - p0->x;
        b.y = p2->y - p0->y;
        b.z = p2->z - p0->z;
    }
    mathutil_vec_cross_prod(&a, &b, &nrm);
    yaw = math_atan2(nrm.x, nrm.z);
    pitch = -math_atan2(nrm.y, math_sqrt((nrm.x * nrm.x) + (nrm.z * nrm.z)));
    mtxa_from_identity();
    mtxa_rotate_y(yaw);
    mtxa_rotate_x(pitch);
    mtxa_rigid_inv_tf_vec(&a, &e);
    roll = math_atan2(e.y, e.x);
    mtxa_rotate_z(roll);
    mtxa_rigid_inv_tf_point(&a, &a);
    mtxa_rigid_inv_tf_point(&b, &b);
    nrm.x = mtxa->mtxA[0][2];
    nrm.y = mtxa->mtxA[1][2];
    nrm.z = mtxa->mtxA[2][2];
    x = -(b.y - a.y);
    y = b.x - a.x;
    len = (x * x) + (y * y);
    if (len <= 1.1920929e-07f) {
        OSPanic("boat_wave.c", 0x561, "boat_wave.c: get_offset_triangle: too small triangle");
    }
    inv = math_rsqrt(len);
    nx1 = x * inv;
    ny1 = y * inv;
    x = (double)b.y;
    y = -(double)b.x;
    len = (x * x) + (y * y);
    if (len <= 1.1920929e-07f) {
        OSPanic("boat_wave.c", 0x569, "boat_wave.c: get_offset_triangle: too small triangle");
    }
    inv = math_rsqrt(len);
    nx2 = x * inv;
    ny2 = y * inv;
    out->pos = v1;
    out->normal = nrm;
    out->rot.x = pitch;
    out->rot.y = yaw;
    out->rot.z = roll;
    out->flags = in->flags;
    out->vert2.x = a.x;
    out->vert2.y = a.y;
    out->vert3.x = b.x;
    out->vert3.y = b.y;
    out->edge2Normal.x = nx1;
    out->edge2Normal.y = ny1;
    out->edge3Normal.x = nx2;
    out->edge3Normal.y = ny2;
}

s32 fn_14_1520C(Vec *pos, Vec *dir, struct StageColiTri *tri)
{
    Vec p;
    Vec d;
    float t;
    float nx;
    float ny;
    float dx;
    float dy;

    mtxa_from_translate(&tri->pos);
    mtxa_rotate_y(tri->rot.y);
    mtxa_rotate_x(tri->rot.x);
    mtxa_rotate_z(tri->rot.z);
    mtxa_rigid_inv_tf_point(pos, &p);
    mtxa_rigid_inv_tf_vec(dir, &d);
    if (fabs(d.z) <= 1.1920929e-07f) {
        return 0;
    }
    t = -p.z / d.z;
    p.x += d.x * t;
    p.y += d.y * t;
    p.z = 0.0f;
    nx = 0.0f;
    ny = 1.0f;
    dx = p.x;
    dy = p.y;
    if ((nx * dx) + (ny * dy) < -0.01) {
        return 0;
    }
    nx = tri->edge2Normal.x;
    ny = tri->edge2Normal.y;
    dx = p.x - tri->vert2.x;
    dy = p.y - tri->vert2.y;
    if ((nx * dx) + (ny * dy) < -0.01) {
        return 0;
    }
    nx = tri->edge3Normal.x;
    ny = tri->edge3Normal.y;
    dx = p.x - tri->vert3.x;
    dy = p.y - tri->vert3.y;
    if ((nx * dx) + (ny * dy) < -0.01) {
        return 0;
    }
    mtxa_tf_point(&p, pos);
    return 1;
}

