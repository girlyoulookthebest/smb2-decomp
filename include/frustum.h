#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "types.h"
#include "mathutil.h"

struct WorldFrustum
{
    Vec origin;
    Vec normals[4];  /* inward side-plane normals, world space */
};

extern struct WorldFrustum worldFrustum;
extern Vec frustumViewNormals[4];   /* inward plane normals, view space */

void fn_1_22D64(Vec *eye, S16Vec *rot, float fovDegrees, float aspect, float offsetX, float offsetY);
void fn_1_23150(Vec *eye, float fovDegrees);
BOOL fn_1_233FC(Vec *point, float radius);
BOOL g_is_sphere_visible(Vec *center, float radius);
BOOL g_is_sphere_visible_scaled(Vec *center, float radius, float scale);

#endif
