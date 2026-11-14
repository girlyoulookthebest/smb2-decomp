/* View frustum setup and sphere visibility tests. */
#include "types.h"
#include "mathutil.h"
#include "frustum.h"

struct WorldFrustum worldFrustum;
Vec frustumViewNormals[4];

/* Stands in for a function the linker stripped from this file. Only its float literals
 * survive (at the start of .rodata), so it is unreferenced and dropped at link time. */
void frustum_stripped_literals(double *d, float *f, float fovDegrees)
{
    d[0] = 10.0 / 7.0;
    d[1] = 182.04444885253906 * fovDegrees;
    d[2] = 0.5 * fovDegrees;
    f[0] = 182.04444885253906f * fovDegrees;
    f[1] = 0.0f;
}

/* Build the four side planes of the view frustum (FOV in degrees; offsets shift the
 * projection centre), in view space and, through the eye transform, in world space. */
void fn_1_22D64(Vec *eye, S16Vec *rot, float fovDegrees, float aspect, float offsetX, float offsetY)
{
    s16 bottom;
    s16 top;
    s16 right;
    s16 left;
    s16 temp;
    s16 temp2;
    float tanHalfX;
    float tanHalfY;
    float sincos[2];

    worldFrustum.origin = *eye;
    if (offsetY == 0.0f)
    {
        temp = (s16)(fovDegrees * 182.04444885253906f);
        bottom = -temp / 2;
        top = temp / 2;
    }
    else
    {
        tanHalfY = math_tan((s16)((fovDegrees * 0.5f) * 182.04444885253906f));
        bottom = -math_atan(tanHalfY * (1.0 - offsetY));
        top = math_atan(tanHalfY * (1.0 + offsetY));
    }
    if (offsetX == 0.0f)
    {
        temp2 = math_atan(math_tan((s16)((fovDegrees * 0.5) * 182.04444885253906)) * aspect) * 2;
        right = temp2 / 2;
        left = -temp2 / 2;
    }
    else
    {
        if (offsetY == 0.0f)
            tanHalfX = aspect * math_tan((s16)((fovDegrees * 0.5f) * 182.04444885253906f));
        else
            tanHalfX = tanHalfY * aspect;
        right = math_atan(tanHalfX * (1.0 + offsetX));
        left = -math_atan(tanHalfX * (1.0 - offsetX));
    }

    mtxa_from_translate(eye);
    mtxa_rotate_y(rot->y);
    mtxa_rotate_x(rot->x);
    mtxa_rotate_z(rot->z);

    math_sin_cos_v(right - 0x4000, sincos);
    frustumViewNormals[0].x = -sincos[0];
    frustumViewNormals[0].y = 0.0f;
    frustumViewNormals[0].z = -sincos[1];
    mtxa_tf_vec(&frustumViewNormals[0], &worldFrustum.normals[0]);

    math_sin_cos_v(left + 0x4000, sincos);
    frustumViewNormals[1].x = -sincos[0];
    frustumViewNormals[1].y = 0.0f;
    frustumViewNormals[1].z = -sincos[1];
    mtxa_tf_vec(&frustumViewNormals[1], &worldFrustum.normals[1]);

    math_sin_cos_v(bottom + 0x4000, sincos);
    frustumViewNormals[2].x = 0.0f;
    frustumViewNormals[2].y = -sincos[0];
    frustumViewNormals[2].z = -sincos[1];
    mtxa_tf_vec(&frustumViewNormals[2], &worldFrustum.normals[2]);

    math_sin_cos_v(top - 0x4000, sincos);
    frustumViewNormals[3].x = 0.0f;
    frustumViewNormals[3].y = -sincos[0];
    frustumViewNormals[3].z = -sincos[1];
    mtxa_tf_vec(&frustumViewNormals[3], &worldFrustum.normals[3]);
}

/* Like fn_1_22D64 for a centred projection with a fixed 10:7 aspect, using the
 * current matrix A as the eye transform. */
void fn_1_23150(Vec *eye, float fovDegrees)
{
    s16 temp2;
    s16 temp;
    s16 bottom;
    s16 top;
    s16 right;
    s16 left;
    float sincos[2];

    worldFrustum.origin = *eye;
    temp2 = math_atan(math_tan((s16)((fovDegrees * 0.5) * 182.04444885253906)) * (10.0 / 7.0)) * 2;
    temp = (s16)(fovDegrees * 182.04444885253906f);
    bottom = -temp / 2;
    top = temp / 2;
    right = temp2 / 2;
    left = -temp2 / 2;

    math_sin_cos_v(right - 0x4000, sincos);
    frustumViewNormals[0].x = -sincos[0];
    frustumViewNormals[0].y = 0.0f;
    frustumViewNormals[0].z = -sincos[1];
    mtxa_tf_vec(&frustumViewNormals[0], &worldFrustum.normals[0]);

    math_sin_cos_v(left + 0x4000, sincos);
    frustumViewNormals[1].x = -sincos[0];
    frustumViewNormals[1].y = 0.0f;
    frustumViewNormals[1].z = -sincos[1];
    mtxa_tf_vec(&frustumViewNormals[1], &worldFrustum.normals[1]);

    math_sin_cos_v(bottom + 0x4000, sincos);
    frustumViewNormals[2].x = 0.0f;
    frustumViewNormals[2].y = -sincos[0];
    frustumViewNormals[2].z = -sincos[1];
    mtxa_tf_vec(&frustumViewNormals[2], &worldFrustum.normals[2]);

    math_sin_cos_v(top - 0x4000, sincos);
    frustumViewNormals[3].x = 0.0f;
    frustumViewNormals[3].y = -sincos[0];
    frustumViewNormals[3].z = -sincos[1];
    mtxa_tf_vec(&frustumViewNormals[3], &worldFrustum.normals[3]);
}

/* Is the world-space sphere inside (or touching) all four side planes? */
BOOL fn_1_233FC(Vec *point, float radius)
{
    Vec *normal = worldFrustum.normals;
    Vec v;
    int i;

    v.x = point->x - worldFrustum.origin.x;
    v.y = point->y - worldFrustum.origin.y;
    v.z = point->z - worldFrustum.origin.z;
    radius = -radius;
    for (i = 4; i > 0; i--, normal++)
    {
        if (v.x * normal->x + v.y * normal->y + v.z * normal->z < radius)
            return FALSE;
    }
    return TRUE;
}

/* Is the sphere (transformed by matrix A into view space) potentially visible? */
BOOL g_is_sphere_visible(Vec *center, float radius)
{
    Vec v;
    float x, y, z;
    Vec *normal;
    int i;

    mtxa_tf_point(center, &v);
    x = v.x;
    y = v.y;
    z = v.z;
    if (z > radius)
        return FALSE;
    radius = -radius;
    normal = frustumViewNormals;
    for (i = 4; i > 0; i--, normal++)
    {
        if (x * normal->x + y * normal->y + z * normal->z < radius)
            return FALSE;
    }
    return TRUE;
}

BOOL g_is_sphere_visible_scaled(Vec *center, float radius, float scale)
{
    Vec v;
    float x, y, z;
    Vec *normal;
    int i;

    mtxa_tf_point(center, &v);
    x = v.x;
    y = v.y;
    z = v.z;
    radius *= scale;
    if (z > radius)
        return FALSE;
    radius = -radius;
    normal = frustumViewNormals;
    for (i = 4; i > 0; i--, normal++)
    {
        if (x * normal->x + y * normal->y + z * normal->z < radius)
            return FALSE;
    }
    return TRUE;
}
