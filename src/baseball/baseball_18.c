#include "baseball.h"

GXColor lbl_18_data_13E0[10] = {
    {0x47, 0x5F, 0x5F, 0xFF},
    {0x5F, 0x57, 0x47, 0xFF},
    {0x47, 0x49, 0x5F, 0xFF},
    {0x5F, 0x4B, 0x5F, 0xFF},
    {0x5F, 0x4B, 0x4B, 0xFF},
    {0x47, 0x5F, 0x4B, 0xFF},
    {0x5C, 0x5A, 0x5A, 0xFF},
    {0x47, 0x47, 0x49, 0xFF},
    {0x47, 0x5F, 0x5F, 0xFF},
    {0x47, 0x5F, 0x5F, 0xFF},
};

s16 lbl_18_data_1408[10] = {0x9, 0x23, 0x24, 0x25, 0x27, 0x5, 0x6, 0x7, 0x8, 0x9};
s16 lbl_18_data_141C[10] = {0x9, 0x28, 0x29, 0x2A, 0x2C, 0xF, 0x10, 0x11, 0x12, 0x13};
s16 lbl_18_data_1430[10] = {0x9, 0x2D, 0x2E, 0x2F, 0x31, 0xA, 0xB, 0xC, 0xD, 0xE};
s16 lbl_18_data_1444[10] = {0x9, 0x3A, 0x3B, 0x3C, 0x3E, 0xD, 0xE, 0xF, 0x10, 0x11};
s16 lbl_18_data_1458[10] = {0x9, 0x3F, 0x40, 0x41, 0x43, 0x17, 0x18, 0x19, 0x1A, 0x1B};
s16 lbl_18_data_146C[10] = {0x9, 0x47, 0x48, 0x49, 0x4B, 0x12, 0x13, 0x14, 0x15, 0x16};
void fn_18_11584(void)
{
    GXColor color;

    if (!(cameras[0].sub28.viewportW > 0.0f && cameras[0].sub28.viewportH > 0.0f))
        return;
    if (fn_18_1798() == 0)
        color = (GXColor){0xB1, 0xE0, 0xFF, 0x00};
    else
        color = (GXColor){0x00, 0x00, 0x00, 0x00};
    GXSetCopyClear_cached(color, 0xFFFFFF);
    if ((lbl_18_bss_2E0.unk1C & 1) != 0 && (lbl_18_bss_2E0.unk1C & 2) == 0)
    {
        fn_18_125CC();
        fn_18_1232C();
    }
    if (events[EVENT_BALL].state == 2)
    {
        g_draw_ball_and_ape();
        fn_18_1179C();
    }
    fn_18_8C04();
    fn_18_11A84();
    draw_shadow_reqs();
    if ((lbl_18_bss_2E0.unk44 & 1) != 0 && (lbl_18_bss_2E0.unk44 & 2) == 0)
        fn_18_13AE4();
    if (lbl_18_bss_2E0.unk11C & 1)
        fn_18_153EC();
    if (lbl_18_bss_2E0.unk2C4 & 1)
        fn_18_142E0();
    if ((lbl_18_bss_2E0.unk638 & 1) != 0)
        fn_18_157C4();
    g_something_with_drawing_apes();
    if (events[EVENT_EFFECT].state == 2)
    {
        g_avdisp_set_ambient(0.8f, 0.8f, 0.8f);
        g_draw_effects();
        g_avdisp_set_ambient(0.35f, 0.35f, 0.35f);
    }
    fn_18_15A88();
    fn_18_19D9C();
    if (fn_18_1798() == 0)
    {
        fn_1_163ABC(0);
        ord_tbl_draw_nodes();
        fn_1_1639C0(0);
    }
    else
        ord_tbl_draw_nodes();
    init_shadow_reqs();
}

void fn_18_1179C(void)
{
    struct ShadowReq req;
    struct RaycastHit hit;
    Vec sp8;
    float t;
    float r;
    struct Ball *ball;
    s32 i;

    req.unk20 = 0.025f;
    req.unk24 = 0.05f;
    req.model = init_common_gma->modelEntries[0x67].model;
    ball = balls;
    for (i = 0; i < 8; i++, ball++)
    {
        if (ball->status != 0 && ball->status != 4 && (!(ball->flags & 0x10) || (ball->flags & 0x80)))
        {
            if (i == 1 && (s32)raycast_stage_down(&ball->pos, &hit, &sp8) == 0)
                continue;
            if (i == 1)
                t = 1.0 - 0.2 * (ball->pos.y - hit.pos.y);
            else
                t = 1.0 - 0.2 * (ball->pos.y - 0.01);
            if (!(t < 0.0f))
            {
                req.color = lbl_18_data_13E0[ball->colorId];
                req.color.r = req.color.r * t;
                req.color.g = req.color.g * t;
                req.color.b = req.color.b * t;
                r = ball->currRadius * (1.0f + 2.0f * (1.0f - t));
                req.scale.x = r;
                req.scale.y = req.scale.x;
                req.scale.z = req.scale.x;
                if (i != 1)
                {
                    hit.normal.x = 0.0f;
                    hit.normal.y = 1.0f;
                    hit.normal.z = 0.0f;
                }
                vec_to_euler(&hit.normal, &req.rot);
                req.rot.z = ball->rotZ;
                if (i == 1)
                    req.pos = hit.pos;
                else
                {
                    req.pos = ball->pos;
                    req.pos.y = 0.01f;
                }
                submit_shadow_req(&req);
            }
        }
    }
}

void fn_18_11A84(void)
{
    struct ShadowReq req;
    struct RaycastHit hit;
    float t;
    float r;
    struct BBPlay *pl;
    s32 i;

    req.unk20 = 0.025f;
    req.unk24 = 0.05f;
    req.model = init_common_gma->modelEntries[0x67].model;
    pl = lbl_18_bss_D68;
    for (i = 0; i < 6; i++, pl++)
    {
        if (pl->unk0 & 1)
        {
            t = 1.0 - 0.2 * (pl->pos.y - 0.01);
            if (!(t < 0.0f))
            {
                req.color = lbl_18_data_13E0[pl->colorId];
                req.color.r = req.color.r * t;
                req.color.g = req.color.g * t;
                req.color.b = req.color.b * t;
                r = pl->unk78 * (1.0f + 2.0f * (1.0f - t));
                req.scale.x = r;
                req.scale.y = req.scale.x;
                req.scale.z = req.scale.x;
                hit.normal.x = 0.0f;
                hit.normal.y = 1.0f;
                hit.normal.z = 0.0f;
                vec_to_euler(&hit.normal, &req.rot);
                req.rot.z = pl->unk80;
                req.pos = pl->pos;
                req.pos.y = 0.01f;
                submit_shadow_req(&req);
            }
        }
    }
}

