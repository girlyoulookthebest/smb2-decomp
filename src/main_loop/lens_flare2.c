/* Scene variant of lens_flare.c: same sprites, but the sun direction comes
 * from the caller's view matrix and the on-screen limit is fixed. */
#pragma peephole off
#include "types.h"
#include "avdisp.h"
#include "mathutil.h"

struct LensFlareSprite
{
    /*0x00*/ float pos;  /* along the sun-to-center line, 1 at the sun */
    /*0x04*/ float scale;
    /*0x08*/ s16 modelId;
};  /* size = 0xC */

extern void GXLoadNrmMtxImm(Mtx m, u32 id);
extern void GXLoadPosMtxImm(Mtx m, u32 id);

static const struct LensFlareSprite s_sprites[] =
{
    {  0.990f, 0.010993125f, 49 },
    {  0.663f, 0.004125f,    43 },
    {  0.473f, 0.002475f,    48 },
    {  0.370f, 0.003135f,    48 },
    {  0.304f, 0.001155f,    48 },
    {  0.145f, 0.001320f,    46 },
    { -0.141f, 0.000825f,    47 },
    { -0.288f, 0.001815f,    46 },
    { -0.353f, 0.002970f,    46 },
    { -0.424f, 0.001155f,    46 },
    { -0.489f, 0.001155f,    45 },
    { -0.625f, 0.003630f,    44 },
    { -0.729f, 0.006105f,    43 },
};

static Vec s_sunPos;
static float s_scale;
static Vec s_spritePos[4][13];
static float s_intensity[4];
static float s_alpha[4];

void fn_1_16373C(Vec *pos)
{
    if (pos != NULL)
        s_sunPos = *pos;
    else
    {
        s_sunPos.x = 0.0f;
        s_sunPos.y = 10000.0f;
        s_sunPos.z = 0.0f;
    }
    s_scale = 0.5f;
}

void fn_1_1637B4(int cam, Mtx mtx)
{
    Vec sunView;
    float dot;
    int i;

    mtxa_from_mtx(mtx);
    mtxa_tf_point(&s_sunPos, &sunView);
    for (i = 0; i < sizeof(s_sprites) / sizeof(s_sprites[0]); i++)
    {
        float t = (-0.11 / sunView.z) * s_sprites[i].pos;

        s_spritePos[cam][i].x = sunView.x * t;
        s_spritePos[cam][i].y = sunView.y * t;
        s_spritePos[cam][i].z = sunView.z * t;
        s_spritePos[cam][i].z = -0.11 + (1e-05 * i);
    }
    {
        Vec sunDir;
        Vec forward = { 0.0f, 0.0f, -1.0f };

        sunDir = s_sunPos;
        mtxa_from_mtx(mtx);
        mtxa_tf_point(&sunDir, &sunDir);
        dot = vec_dot_normalized_safe(&sunDir, &forward);
        if (dot <= 0.70710678)
        {
            s_intensity[cam] = 0.0f;
            s_alpha[cam] = 0.0f;
        }
        else
        {
            s_intensity[cam] = (1.0 / (1.0 - 0.70710678)) * (dot - 0.70710678);
            s_alpha[cam] = 0.99 * s_intensity[cam];
        }
    }
}

void fn_1_1639BC(void) {}

void fn_1_1639C0(int cam)
{
    u32 i;
    Vec *pos;
    const struct LensFlareSprite *sprite;

    if (s_alpha[cam] > 0.0f)
    {
        for (i = 0, pos = s_spritePos[cam], sprite = s_sprites; i < 13; i++, pos++, sprite++)
        {
            float scale;

            avdisp_set_alpha(s_alpha[cam]);
            mtxa_from_translate(pos);
            scale = sprite->scale * s_scale;
            mtxa_scale_xyz(scale, scale, 1.0f);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[sprite->modelId].model);
        }
    }
}

void fn_1_163ABC(int cam)
{
    if (s_alpha[cam] > 0.0f)
    {
        mtxa_from_translate_xyz(0.0f, 0.0f, -0.2f);
        avdisp_set_alpha(0.75f * s_alpha[cam]);
        avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[31].model);
    }
}
