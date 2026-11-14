/* Lens flare: a row of sprites along the line through the screen center and
 * the sun, plus a full-screen wash. Balls in front of the sun dim both. */
#pragma peephole off
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "gx.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"

struct LensFlareSprite
{
    /*0x00*/ float pos;  /* along the sun-to-center line, 1 at the sun */
    /*0x04*/ float scale;
    /*0x08*/ s16 modelId;
};  /* size = 0xC */

extern void PSVECSubtract(Vec *a, Vec *b, Vec *dst);
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

void fn_1_D68D4(void)
{
    s_sunPos.x = 0.0f;
    s_sunPos.y = 10000.0f;
    s_sunPos.z = 0.0f;
    s_scale = 1.0f;
}

static inline float dist_sq_2d(register float x1, register float y1, register float x2, register float y2)
{
#ifdef __MWERKS__
    asm
    {
        fsubs x1, x1, x2
        fsubs y1, y1, y2
        fmuls x1, x1, x1
        fmadds x1, y1, y1, x1
    }
    return x1;
#else
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
#endif
}

void fn_1_D6920(void)
{
    Vec sunView;
    Vec sunDir;
    struct Camera *camera;
    int i;
    struct Ball *ball;
    s8 *status;
    int count;
    float limit;

    camera = cameras;
    for (i = 0; i < modeCtrl.playerCount; i++, camera++)
    {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f)
        {
            float tan;
            float dot;

            mtxa_from_mtx(camera->unk1AC);
            mtxa_tf_point(&s_sunPos, &sunView);
            {
                int j;

                for (j = 0; j < sizeof(s_sprites) / sizeof(s_sprites[0]); j++)
                {
                    float t = (-0.11 / sunView.z) * s_sprites[j].pos;

                    s_spritePos[i][j].x = sunView.x * t;
                    s_spritePos[i][j].y = sunView.y * t;
                    s_spritePos[i][j].z = sunView.z * t;
                    s_spritePos[i][j].z = -0.11 + (1e-05 * j);
                }
            }
            {
                Vec forward = { 0.0f, 0.0f, -1.0f };

                PSVECSubtract(&s_sunPos, &camera->eye, &sunDir);
                mtxa_from_mtx(camera->unk1AC);
                mtxa_tf_point(&sunDir, &sunDir);
                tan = camera->sub28.view.tanHalfFov;
                if (camera->sub28.view.aspect > 1.0f)
                    tan *= camera->sub28.view.aspect;
                limit = 1.0f / math_sqrt(1.0f + tan * tan);
                dot = vec_dot_normalized_safe(&sunDir, &forward);
                if (dot <= limit)
                {
                    s_intensity[i] = 0.0f;
                    s_alpha[i] = 0.0f;
                }
                else
                {
                    s_intensity[i] = (1.0 / (1.0 - limit)) * (dot - limit);
                    s_alpha[i] = 0.99 * s_intensity[i];
                }
                if (s_alpha[i] > 0.0f)
                {
                    ball = balls;
                    status = g_poolInfo.playerPool.statusList;
                    for (count = g_poolInfo.playerPool.count; count > 0; count--, ball++, status++)
                    {
                        if (*status != 0 && *status != 4 && !(ball->flags & 0x10))
                        {
                            Vec ballPos;
                            Vec flarePos;

                            mtxa_tf_point(&ball->pos, &ballPos);
                            if (!(ballPos.z >= 0.0f))
                            {
                                float t;
                                float t2;

                                t = -1.0f / (ballPos.z * camera->sub28.view.tanHalfFov);
                                ballPos.x *= t;
                                ballPos.y *= t;
                                limit = ball->currRadius * t;
                                flarePos = s_spritePos[i][0];
                                t2 = -1.0f / (flarePos.z * camera->sub28.view.tanHalfFov);
                                flarePos.x *= t2;
                                flarePos.y *= t2;
                                t = dist_sq_2d(ballPos.x, ballPos.y, flarePos.x, flarePos.y);
                                t = math_sqrt(t) / limit;
                                if (t < 1.0f)
                                {
                                    t = 1.0f - math_sqrt(1.0f - t * t);
                                    s_intensity[i] *= t;
                                    s_alpha[i] *= t;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void fn_1_D6D84(int cam)
{
    float tan;
    float zScale;
    u32 i;
    Vec *pos;
    const struct LensFlareSprite *sprite;

    if (!(cameras[cam].sub28.viewportW > 0.0f) || !(cameras[cam].sub28.viewportH > 0.0f))
        return;
    if (s_alpha[cam] == 0.0)
        return;
    if (main_mode == 0)
    {
        switch (sub_mode)
        {
        case 0xB:
        case 0xC:
        case 0xF:
        case 0x10:
        case 0x11:
        case 0x12:
            break;
        default:
            return;
        }
    }
    tan = g_current_camera->sub28.view.tanHalfFov;
    avdisp_set_z_mode(1, 3, 0);
    zScale = -0.00025f * (1.0f + s_alpha[cam]);
    for (i = 0, pos = s_spritePos[cam], sprite = s_sprites; i < 13; i++, pos++, sprite++)
    {
        float scale;

        avdisp_set_alpha(0.25f * s_alpha[cam]);
        mtxa_from_translate(pos);
        scale = (tan * (zScale * pos->z)) / ((7.0f * sprite->scale) * s_scale);
        mtxa_scale_xyz(scale, scale, 1.0f);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[sprite->modelId].model);
    }
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_D6F9C(int cam)
{
    if (!(cameras[cam].sub28.viewportW > 0.0f) || !(cameras[cam].sub28.viewportH > 0.0f))
        return;
    if (s_alpha[cam] == 0.0)
        return;
    if (main_mode == 0)
    {
        switch (sub_mode)
        {
        case 0xB:
        case 0xC:
        case 0xF:
        case 0x10:
        case 0x11:
        case 0x12:
            break;
        default:
            return;
        }
    }
    avdisp_set_z_mode(1, 3, 0);
    mtxa_from_translate_xyz(0.0f, 0.0f, -0.2f);
    avdisp_set_alpha(0.75f * s_alpha[cam]);
    avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[31].model);
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_D70EC(Vec *pos)
{
    s_sunPos = *pos;
}

void fn_1_D7110(int rotY, int rotX)
{
    mtxa_from_rotate_y(rotY);
    mtxa_rotate_x(rotX);
    mathutil_mtxA_get_col2_scaled(-1000000.0f, &s_sunPos);
}

void fn_1_D7184(float scale)
{
    s_scale = scale;
}
