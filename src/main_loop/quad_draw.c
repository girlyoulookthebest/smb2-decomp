/* Flat coloured quads, single and batched, with a dummy strip sent first. */
#include "types.h"
#include "frustum.h"
#include "gx.h"
#include "mathutil.h"

struct QuadDesc
{
    /*0x00*/ Vec pos;
    /*0x0C*/ float radius;
    /*0x10*/ s16 rot;
    /*0x12*/ u16 flags;
    /*0x14*/ GXColor color[4];
}; /* size = 0x24 */

struct QuadBatchItem
{
    /*0x00*/ float radius;
    /*0x04*/ Mtx mtx;
    /*0x34*/ u16 flags;
    /*0x36*/ GXColor color[4];
}; /* size = 0x48 */

void g_something_with_GX_vtx_desc(u32 attrs);
void GXSetTevKColor_cached(int id, GXColor color);
void GXLoadPosMtxImm(Mtx m, int id);
void fn_800668FC(void);
void fn_1_16B9F4(void);

void fn_1_16B00C(struct QuadDesc *quad)
{
    Vec v[4];
    float r;
    float x0;
    float y0;
    float x1;
    float y1;

    if (!g_is_sphere_visible(&quad->pos, quad->radius))
    {
        quad->flags |= 1;
        return;
    }
    quad->flags &= ~1;
    fn_800668FC();
    g_something_with_GX_vtx_desc(0x2200);
    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();
    GXSetTevKColor_cached(0, quad->color[0]);
    GXBegin(0x90, 0, 8);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXBegin(0x80, 0, 4);
    r = quad->radius;
    x0 = -r;
    y0 = -r;
    x1 = r;
    y1 = r;
    mtxa_push();
    mtxa_translate(&quad->pos);
    mtxa_sq_from_identity();
    if (quad->rot != 0)
        mtxa_rotate_z(quad->rot);
    mtxa_tf_point_xyz(x0, y0, 0.0f, &v[0]);
    mtxa_tf_point_xyz(x1, y0, 0.0f, &v[1]);
    mtxa_tf_point_xyz(x1, y1, 0.0f, &v[2]);
    mtxa_tf_point_xyz(x0, y1, 0.0f, &v[3]);
    mtxa_pop();
    GXPosition3f32(v[0].x, v[0].y, v[0].z);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(v[1].x, v[1].y, v[1].z);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(v[2].x, v[2].y, v[2].z);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(v[3].x, v[3].y, v[3].z);
    GXTexCoord2f32(0.0f, 1.0f);
}

void fn_1_16B608(struct QuadDesc *quad)
{
    Vec v[4];
    float r;
    float x0;
    float y0;
    float x1;
    float y1;

    if (!g_is_sphere_visible(&quad->pos, quad->radius))
    {
        quad->flags |= 1;
        return;
    }
    quad->flags &= ~1;
    fn_800668FC();
    g_something_with_GX_vtx_desc(0x2A00);
    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();
    fn_1_16B9F4();
    GXBegin(0x80, 0, 4);
    r = quad->radius;
    x0 = -r;
    y0 = -r;
    x1 = r;
    y1 = r;
    mtxa_push();
    mtxa_translate(&quad->pos);
    mtxa_sq_from_identity();
    if (quad->rot != 0)
        mtxa_rotate_z(quad->rot);
    mtxa_tf_point_xyz(x0, y0, 0.0f, &v[0]);
    mtxa_tf_point_xyz(x1, y0, 0.0f, &v[1]);
    mtxa_tf_point_xyz(x1, y1, 0.0f, &v[2]);
    mtxa_tf_point_xyz(x0, y1, 0.0f, &v[3]);
    mtxa_pop();
    GXPosition3f32(v[0].x, v[0].y, v[0].z);
    GXColor4u8((u8)quad->color[0].r, (u8)quad->color[0].g, (u8)quad->color[0].b, (u8)quad->color[0].a);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(v[1].x, v[1].y, v[1].z);
    GXColor4u8((u8)quad->color[1].r, (u8)quad->color[1].g, (u8)quad->color[1].b, (u8)quad->color[1].a);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(v[2].x, v[2].y, v[2].z);
    GXColor4u8((u8)quad->color[2].r, (u8)quad->color[2].g, (u8)quad->color[2].b, (u8)quad->color[2].a);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(v[3].x, v[3].y, v[3].z);
    GXColor4u8((u8)quad->color[3].r, (u8)quad->color[3].g, (u8)quad->color[3].b, (u8)quad->color[3].a);
    GXTexCoord2f32(0.0f, 1.0f);
}

void fn_1_16B9F4(void)
{
    GXBegin(0x90, 0, 8);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(0, 0, 0, 0);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(0, 0, 0, 0);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(0, 0, 0, 0);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(0, 0, 0, 0);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(0, 0, 0, 0);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(0, 0, 0, 0);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(0, 0, 0, 0);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(0, 0, 0, 0);
    GXTexCoord2f32(0.0f, 0.0f);
}

void fn_1_16BEC0(int count, struct QuadBatchItem *items)
{
    Vec v[4];
    Vec center;
    float r;
    float x0;
    float y0;
    float x1;
    float y1;
    int visible = 0;
    int i;
    struct QuadBatchItem *item;

    item = items;
    for (i = count; i > 0; i--, item++)
    {
        center.x = item->mtx[0][3];
        center.y = item->mtx[1][3];
        center.z = item->mtx[2][3];
        if (g_is_sphere_visible(&center, item->radius))
        {
            item->flags &= ~1;
            visible++;
        }
        else
            item->flags |= 1;
    }
    if (visible == 0)
        return;
    fn_800668FC();
    g_something_with_GX_vtx_desc(0x2A00);
    fn_1_16B9F4();
    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();
    GXBegin(0x80, 0, visible * 4);
    item = items;
    for (i = count; i > 0; i--, item++)
    {
        if (item->flags & 1)
            continue;
        r = item->radius;
        x0 = -r;
        y0 = -r;
        x1 = r;
        y1 = r;
        mtxa_from_mtx(item->mtx);
        mtxa_tf_point_xyz(x0, y0, 0.0f, &v[0]);
        mtxa_tf_point_xyz(x1, y0, 0.0f, &v[1]);
        mtxa_tf_point_xyz(x1, y1, 0.0f, &v[2]);
        mtxa_tf_point_xyz(x0, y1, 0.0f, &v[3]);
        GXPosition3f32(v[0].x, v[0].y, v[0].z);
        GXColor4u8((u8)item->color[0].r, (u8)item->color[0].g, (u8)item->color[0].b, (u8)item->color[0].a);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(v[1].x, v[1].y, v[1].z);
        GXColor4u8((u8)item->color[1].r, (u8)item->color[1].g, (u8)item->color[1].b, (u8)item->color[1].a);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(v[2].x, v[2].y, v[2].z);
        GXColor4u8((u8)item->color[2].r, (u8)item->color[2].g, (u8)item->color[2].b, (u8)item->color[2].a);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(v[3].x, v[3].y, v[3].z);
        GXColor4u8((u8)item->color[3].r, (u8)item->color[3].g, (u8)item->color[3].b, (u8)item->color[3].a);
        GXTexCoord2f32(0.0f, 1.0f);
    }
}
