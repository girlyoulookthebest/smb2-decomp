#include "boat.h"
#include "macros.h"

struct BoatWakeSeg
{
    Vec left;
    Vec right;
    Vec center;
    Vec dir;
    float alpha;
    float v;
};

struct BoatWake
{
    s16 head;
    float v;
    struct BoatWakeSeg segs[48];
};

struct BoatWake lbl_14_bss_63A8[8];

void fn_14_22A9C(void)
{
    Vec pos;
    s32 j;
    s32 i;
    struct BoatWake *wake;
    struct BoatWakeSeg *seg;

    i = 0;
    wake = lbl_14_bss_63A8;
    for (; i < 8; i++, wake++) {
        pos = balls[i].pos;
        wake->head = 0;
        wake->v = 0.0f;
        j = 0;
        seg = wake->segs;
        for (; j < 48; j++, seg++) {
            seg->left = pos;
            seg->right = pos;
            seg->center = pos;
            seg->v = 0.0f;
        }
    }
}

void fn_14_22B80(void)
{
    Vec up = {0.0f, 1.0f, 0.0f};
    Vec pos;
    Vec vel;
    Vec side;
    float radius;
    float speed;
    float surface;
    float depth;
    s32 i;
    struct Ball *ball;
    struct BoatPlayer *player;
    struct BoatWake *wake;
    s16 head;
    s16 angle;
    struct BoatWakeSeg *seg;
    Vec *dir;
    Vec *right;
    Vec *center;
    s32 j;
    s32 k;

    if ((u32)(g_some_other_flags & 0xA) == 0) {
        i = 0;
        wake = lbl_14_bss_63A8;
        ball = balls;
        player = lbl_14_bss_208;
        for (; i < 8; i++, wake++, ball++, player++) {
            pos = ball->pos;
            radius = ball->currRadius * player->unk3B0;
            vel = ball->vel;
            speed = mathutil_vec_len(&vel);
            {
                float t = pos.y - radius;
                surface = player->unk478;
                depth = surface - t;
            }
            angle = math_atan2(vel.x, vel.z);
            if (ball->status == 2) {
                head = (wake->head + 1) % 48;
                wake->head = head;
                seg = &wake->segs[head];
                right = &seg->right;
                center = &seg->center;
                dir = &seg->dir;
                wake->v -= 0.2f * speed;
                seg->v = wake->v;
                if (depth <= 0.0f || depth >= 2.0f * radius) {
                    depth = 0.0f;
                } else {
                    depth = math_sqrt(depth * (2.0f * radius - depth));
                }
                side.x = depth * math_sin(angle + 0x4000);
                side.y = 0.0f;
                side.z = -depth * math_sin(angle);
                seg->left.x = pos.x - side.x;
                seg->left.y = surface;
                seg->left.z = pos.z - side.z;
                right->x = pos.x + side.x;
                right->y = surface;
                right->z = pos.z + side.z;
                center->x = pos.x;
                center->y = surface;
                center->z = pos.z;
                *dir = side;
                if (speed <= 0.05f) {
                    seg->alpha = 0.0f;
                } else if (speed >= 0.2f) {
                    seg->alpha = 1.0f;
                } else {
                    seg->alpha = 6.6666665f * (speed - 0.05f);
                }
                seg->alpha *= 0.3f + 0.7f * (0.5f * (1.0f + math_sin((g_font_char_var_3 & 0x1F) << 0xB)));
                k = (head + 47) % 48;
                for (j = 0; j < 47; j++) {
                    if (k != head) {
                        seg = &wake->segs[k];
                        right = &seg->right;
                        dir = &seg->dir;
                        seg->left.x -= 0.2f * dir->x;
                        seg->left.z -= 0.2f * dir->z;
                        right->x += 0.2f * dir->x;
                        right->z += 0.2f * dir->z;
                        k = (k + 47) % 48;
                    }
                }
            }
        }
    }
}

void fn_14_22FBC(void)
{
    s32 i;
    struct Ball *ball;
    struct BoatWake *wake;
    s32 j;
    s32 k;
    struct BoatWakeSeg *seg;
    Vec *a;
    Vec *b;
    Vec *c;
    float t;
    float f;
    s32 alpha;

    g_something_with_GX_vtx_desc(0x2A00);
    GXSetBlendMode_cached(1, 4, 1, 0);
    GXSetZMode_cached(1, 3, 0);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
    g_GXSetTevIndirect_zero_if_different(0);
    GXLoadTexObj_cached(lbl_14_bss_B4[10]->texObjs, 0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(1);
    mtxa_from_mtxb();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    i = 0;
    wake = lbl_14_bss_63A8;
    ball = balls;
    for (; i < 8; i++, wake++, ball++) {
        if (ball->status == 2) {
            GXBegin(0x98, 0, 0x5E);
            k = (wake->head + 1) % 48;
            for (j = 0; j < 47; j++) {
                seg = &wake->segs[k];
                a = &seg->center;
                t = j / 46.0f;
                if (t < 0.3f) {
                    f = 3.3333332008785725 * t;
                } else if (t < 0.95f) {
                    f = 1.0f;
                } else {
                    f = 19.999995231629555 - 19.999995231629555 * t;
                }
                alpha = 255.0f * (f * seg->alpha);
                GXPosition3f32(seg->left.x, seg->left.y, seg->left.z);
                GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
                GXTexCoord2f32(0.0f, seg->v);
                GXPosition3f32(a->x, a->y, a->z);
                GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
                GXTexCoord2f32(0.5f, seg->v);
                k = (k + 1) % 48;
            }
            GXBegin(0x98, 0, 0x5E);
            k = (wake->head + 1) % 48;
            for (j = 0; j < 47; j++) {
                seg = &wake->segs[k];
                b = &seg->right;
                c = &seg->center;
                t = j / 46.0f;
                if (t < 0.3f) {
                    f = 3.3333332008785725 * t;
                } else if (t < 0.95f) {
                    f = 1.0f;
                } else {
                    f = 19.999995231629555 - 19.999995231629555 * t;
                }
                alpha = 255.0f * (f * seg->alpha);
                GXPosition3f32(c->x, c->y, c->z);
                GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
                GXTexCoord2f32(0.5f, seg->v);
                GXPosition3f32(b->x, b->y, b->z);
                GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
                GXTexCoord2f32(1.0f, seg->v);
                k = (k + 1) % 48;
            }
        }
    }
}

