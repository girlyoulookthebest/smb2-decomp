/* Blob shadows (SMB1 mini_3): up to 64 requests queued each frame and drawn
 * as models, faded out by on-screen size. */
#pragma peephole off
#include "types.h"
#include "avdisp.h"
#include "camera.h"
#include "gx.h"
#include "mathutil.h"
#include "shadow.h"

extern void GXLoadNrmMtxImm(Mtx m, u32 id);
extern void GXLoadPosMtxImm(Mtx m, u32 id);

void init_shadow_reqs(void);

void fn_1_D7194(void)
{
    init_shadow_reqs();
}

s32 shadow_req_capacity;
struct ShadowReq *next_shadow_req;
struct ShadowReq shadow_reqs[64];

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void init_shadow_reqs(void)
{
    shadow_req_capacity = 64;
    next_shadow_req = shadow_reqs;
}

int submit_shadow_req(struct ShadowReq *req)
{
    if (shadow_req_capacity == 0)
        return 0;
    *next_shadow_req = *req;
    next_shadow_req->unk30 = MAX(MAX(next_shadow_req->scale.x, next_shadow_req->scale.y), next_shadow_req->scale.z);
    next_shadow_req->unk34 = next_shadow_req->unk30 * next_shadow_req->model->boundSphereRadius;
    next_shadow_req++;
    shadow_req_capacity--;
    return 1;
}

int fn_1_D73B4(void)
{
    return 64 - shadow_req_capacity;
}

void draw_shadow_reqs(void)
{
    Vec pos;
    Vec scale;
    int count;
    struct ShadowReq *req;
    float fade;
    float k;
    float t;

    count = 64 - shadow_req_capacity;
    if (count != 0)
    {
        float tan = -g_current_camera->sub28.view.tanHalfFov;

        k = tan / g_current_camera->sub28.viewportH;
        avdisp_set_z_mode(1, 3, 0);
        req = shadow_reqs;
        for (; count > 0; count--, req++)
        {
            mtxa_from_mtxb_translate(&req->pos);
            mtxa_rotate_y(req->rot.y);
            mtxa_rotate_x(req->rot.x);
            mtxa_rotate_z(req->rot.z);
            mathutil_mtxA_get_translate(&pos);
            if (pos.z < 0.0f)
            {
                fade = req->unk34 / (pos.z * k);
                if (fade < req->unk20)
                    continue;
                fade = (fade - req->unk20) / (req->unk24 - req->unk20);
                if (fade > 1.0f)
                    fade = 1.0f;
            }
            else
                fade = 1.0f;
            t = 1.0f - (0.2f / mathutil_vec_len(&pos));
            pos.x *= t;
            pos.y *= t;
            pos.z *= t;
            mathutil_mtxA_set_translate(&pos);
            scale.x = req->scale.x * t;
            scale.y = req->scale.y * t;
            scale.z = req->scale.z * t;
            mtxa_scale(&scale);
            avdisp_set_scale_factor(req->unk30);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            fade *= 0.003921569f;
            set_post_mult_color(req->color.r * fade, req->color.g * fade, req->color.b * fade, 1.0f);
            avdisp_draw_model_culled_sort_never(req->model);
            set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        }
        avdisp_set_z_mode(1, 3, 1);
    }
}
